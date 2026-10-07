#!/bin/sh
set -eu

# Build a PRIVATE test ISO from files supplied by the user.
# None of the required Nintendo files are stored in this repository.
#
# Usage:
#   ./tools/make_hybrid_iso.sh melonDS.elf HeartGold.nds bios7.bin bios9.bin firmware.bin HGPS2-HYBRID.iso

if [ "$#" -ne 6 ]; then
    echo "usage: $0 <melonDS.elf> <HeartGold.nds> <bios7.bin> <bios9.bin> <firmware.bin> <output.iso>" >&2
    exit 2
fi

ELF="$1"; ROM="$2"; B7="$3"; B9="$4"; FW="$5"; OUT="$6"
for f in "$ELF" "$ROM" "$B7" "$B9" "$FW"; do
    [ -f "$f" ] || { echo "missing: $f" >&2; exit 1; }
done

MKISOFS="$(command -v mkisofs || command -v genisoimage || true)"
[ -n "$MKISOFS" ] || { echo "mkisofs/genisoimage not found" >&2; exit 1; }

STAGE="${TMPDIR:-/tmp}/hgps2-hybrid-$$"
trap 'rm -rf "$STAGE"' EXIT
mkdir -p "$STAGE"

cp "$ELF" "$STAGE/MELONDS.ELF"
cp "$ROM" "$STAGE/HEARTGOLD.NDS"
cp "$B7" "$STAGE/BIOS7.BIN"
cp "$B9" "$STAGE/BIOS9.BIN"
cp "$FW" "$STAGE/FIRMWARE.BIN"
cat > "$STAGE/SYSTEM.CNF" <<'CNF'
BOOT2 = cdrom0:\\MELONDS.ELF;1
VER = 1.00
VMODE = NTSC
CNF

"$MKISOFS" -o "$OUT" -V HGPS2_HYBRID -sysid PLAYSTATION -iso-level 1 "$STAGE"
echo "created private test ISO: $OUT"
