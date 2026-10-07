#!/usr/bin/env python3
"""
Build a private Stage 1 hybrid PS2 ISO from user-supplied files.

This script does not ship any Nintendo ROM, BIOS, firmware or game assets.
It takes an existing small bootable ISO template, replaces/redirects the
MELONDS.ELF entry, adds BIOS/firmware to the root directory, creates a ROMS
subdirectory and places the user-owned HeartGold dump there.

The template is expected to be ISO9660 with a primary volume descriptor,
a Joliet supplementary descriptor, a root directory that fits in one sector,
and a SYSTEM.CNF that boots MELONDS.ELF.
"""

from __future__ import annotations

import argparse
from pathlib import Path

SECTOR = 2048


def dir_record(extent: int, size: int, name: bytes, flags: int = 0) -> bytes:
    pad = 1 if len(name) % 2 == 0 else 0
    rec = bytearray(33 + len(name) + pad)
    rec[0] = len(rec)
    rec[1] = 0
    rec[2:6] = extent.to_bytes(4, "little")
    rec[6:10] = extent.to_bytes(4, "big")
    rec[10:14] = size.to_bytes(4, "little")
    rec[14:18] = size.to_bytes(4, "big")
    rec[18:25] = bytes([126, 10, 7, 0, 0, 0, 0])
    rec[25] = flags
    rec[26] = 0
    rec[27] = 0
    rec[28:30] = (1).to_bytes(2, "little")
    rec[30:32] = (1).to_bytes(2, "big")
    rec[32] = len(name)
    rec[33:33 + len(name)] = name
    return bytes(rec)


def path_entry(extent: int, parent: int, name: bytes, little: bool) -> bytes:
    pad = 1 if len(name) % 2 else 0
    rec = bytearray(8 + len(name) + pad)
    rec[0] = len(name)
    rec[1] = 0
    rec[2:6] = extent.to_bytes(4, "little" if little else "big")
    rec[6:8] = parent.to_bytes(2, "little" if little else "big")
    rec[8:8 + len(name)] = name
    return bytes(rec)


def append_blob(image: bytearray, data: bytes) -> tuple[int, int]:
    if len(image) % SECTOR:
        image.extend(b"\0" * (SECTOR - (len(image) % SECTOR)))
    extent = len(image) // SECTOR
    image.extend(data)
    if len(image) % SECTOR:
        image.extend(b"\0" * (SECTOR - (len(image) % SECTOR)))
    return extent, len(data)


def root_extent(image: bytearray, descriptor_sector: int) -> int:
    base = descriptor_sector * SECTOR
    return int.from_bytes(image[base + 158:base + 162], "little")


def append_root_records(image: bytearray, extent: int, records: list[bytes]) -> None:
    base = extent * SECTOR
    offset = 0
    while offset < SECTOR:
        length = image[base + offset]
        if length == 0:
            break
        offset += length
    for rec in records:
        if offset + len(rec) > SECTOR:
            raise RuntimeError("root directory sector is full")
        image[base + offset:base + offset + len(rec)] = rec
        offset += len(rec)


def replace_root_file(image: bytearray, root: int, wanted: bytes,
                      new_extent: int, new_size: int) -> None:
    base = root * SECTOR
    offset = 0
    while offset < SECTOR:
        length = image[base + offset]
        if length == 0:
            break
        rec = image[base + offset:base + offset + length]
        name_len = rec[32]
        name = bytes(rec[33:33 + name_len])
        if name == wanted:
            image[base + offset + 2:base + offset + 6] = new_extent.to_bytes(4, "little")
            image[base + offset + 6:base + offset + 10] = new_extent.to_bytes(4, "big")
            image[base + offset + 10:base + offset + 14] = new_size.to_bytes(4, "little")
            image[base + offset + 14:base + offset + 18] = new_size.to_bytes(4, "big")
            return
        offset += length
    raise RuntimeError(f"missing root entry: {wanted!r}")


def patch_legacy_elf(data: bytes) -> bytes:
    out = bytearray(data)

    replacements = [
        (b"mass:/melonDS/", b"cdrom0:\\"),
        (b"roms/", b"ROMS/"),
        (b".nds", b".NDS"),
        (b"bios7.bin", b"BIOS7.BIN"),
        (b"bios9.bin", b"BIOS9.BIN"),
        (b"firmware.bin", b"FIRMWARE.BIN"),
    ]

    for old, new in replacements:
        start = 0
        while True:
            pos = out.find(old, start)
            if pos < 0:
                break
            if len(new) > len(old):
                raise RuntimeError(f"replacement too large: {old!r}")
            out[pos:pos + len(old)] = new + b"\0" * (len(old) - len(new))
            start = pos + len(old)

    return bytes(out)


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--template", required=True, type=Path)
    ap.add_argument("--elf", required=True, type=Path)
    ap.add_argument("--rom", required=True, type=Path)
    ap.add_argument("--bios7", required=True, type=Path)
    ap.add_argument("--bios9", required=True, type=Path)
    ap.add_argument("--firmware", required=True, type=Path)
    ap.add_argument("--out", required=True, type=Path)
    ap.add_argument("--patch-legacy-elf", action="store_true")
    args = ap.parse_args()

    image = bytearray(args.template.read_bytes())

    if image[16 * SECTOR + 1:16 * SECTOR + 6] != b"CD001":
        raise RuntimeError("template is not an ISO9660 image")
    if image[17 * SECTOR] != 2 or image[17 * SECTOR + 1:17 * SECTOR + 6] != b"CD001":
        raise RuntimeError("template does not contain the expected Joliet descriptor")

    p_root = root_extent(image, 16)
    j_root = root_extent(image, 17)

    elf = args.elf.read_bytes()
    if args.patch_legacy_elf:
        elf = patch_legacy_elf(elf)

    p_roms, _ = append_blob(image, b"\0" * SECTOR)
    j_roms, _ = append_blob(image, b"\0" * SECTOR)
    elf_extent, elf_size = append_blob(image, elf)
    b7_extent, b7_size = append_blob(image, args.bios7.read_bytes())
    b9_extent, b9_size = append_blob(image, args.bios9.read_bytes())
    fw_extent, fw_size = append_blob(image, args.firmware.read_bytes())
    rom_extent, rom_size = append_blob(image, args.rom.read_bytes())

    pdir = bytearray(SECTOR)
    pos = 0
    for rec in (
        dir_record(p_roms, SECTOR, b"\x00", flags=2),
        dir_record(p_root, SECTOR, b"\x01", flags=2),
        dir_record(rom_extent, rom_size, b"HEARTGOLD.NDS;1"),
    ):
        pdir[pos:pos + len(rec)] = rec
        pos += len(rec)
    image[p_roms * SECTOR:(p_roms + 1) * SECTOR] = pdir

    jdir = bytearray(SECTOR)
    pos = 0
    for rec in (
        dir_record(j_roms, SECTOR, b"\x00", flags=2),
        dir_record(j_root, SECTOR, b"\x01", flags=2),
        dir_record(rom_extent, rom_size, "HEARTGOLD.NDS".encode("utf-16-be")),
    ):
        jdir[pos:pos + len(rec)] = rec
        pos += len(rec)
    image[j_roms * SECTOR:(j_roms + 1) * SECTOR] = jdir

    append_root_records(image, p_root, [
        dir_record(p_roms, SECTOR, b"ROMS", flags=2),
        dir_record(b7_extent, b7_size, b"BIOS7.BIN;1"),
        dir_record(b9_extent, b9_size, b"BIOS9.BIN;1"),
        dir_record(fw_extent, fw_size, b"FIRMWARE.BIN;1"),
    ])
    append_root_records(image, j_root, [
        dir_record(j_roms, SECTOR, "ROMS".encode("utf-16-be"), flags=2),
        dir_record(b7_extent, b7_size, "BIOS7.BIN".encode("utf-16-be")),
        dir_record(b9_extent, b9_size, "BIOS9.BIN".encode("utf-16-be")),
        dir_record(fw_extent, fw_size, "FIRMWARE.BIN".encode("utf-16-be")),
    ])

    replace_root_file(image, p_root, b"MELONDS.ELF;1", elf_extent, elf_size)
    replace_root_file(image, j_root, "MELONDS.ELF".encode("utf-16-be"),
                      elf_extent, elf_size)

    for descriptor, extent, name in (
        (16, p_roms, b"ROMS"),
        (17, j_roms, "ROMS".encode("utf-16-be")),
    ):
        base = descriptor * SECTOR
        old_size = int.from_bytes(image[base + 132:base + 136], "little")
        lpt = int.from_bytes(image[base + 140:base + 144], "little")
        mpt = int.from_bytes(image[base + 148:base + 152], "big")
        le = path_entry(extent, 1, name, True)
        be = path_entry(extent, 1, name, False)
        image[lpt * SECTOR + old_size:lpt * SECTOR + old_size + len(le)] = le
        image[mpt * SECTOR + old_size:mpt * SECTOR + old_size + len(be)] = be
        new_size = old_size + len(le)
        image[base + 132:base + 136] = new_size.to_bytes(4, "little")
        image[base + 136:base + 140] = new_size.to_bytes(4, "big")

    total_sectors = len(image) // SECTOR
    for descriptor in (16, 17):
        base = descriptor * SECTOR
        image[base + 80:base + 84] = total_sectors.to_bytes(4, "little")
        image[base + 84:base + 88] = total_sectors.to_bytes(4, "big")

    args.out.write_bytes(image)
    print(f"created {args.out} ({len(image)} bytes, {total_sectors} sectors)")


if __name__ == "__main__":
    main()
