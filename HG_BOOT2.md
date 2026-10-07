# HG-BOOT2 PS2 optimization branch

This branch turns the old melonDS PS2 proof-of-concept into a low-memory,
diagnostic-first base for the ndsps2 project.

## Phase 1 implemented

- Cartridge ROM is no longer allocated in full in EE RAM.
- Only the first 32 KiB (header + secure area) remain resident.
- Remaining cartridge data is streamed from the backing file.
- Direct boot streams ARM9/ARM7 executable sections in 16 KiB chunks.
- Power-of-two cartridge mirroring is preserved without allocating the padded image.
- Removed an unused pair of 64 KiB EE thread stacks.
- Removed unused audio buffers from the single-thread diagnostic path.
- Removed a per-frame debug printf from the unused frame worker.
- Removed a redundant 256x384x32-bit framebuffer self-copy.
- Added HG2 boot-stage logging.
- Enabled conservative R5900 compiler optimization while disabling strict aliasing.

## Why this is the new base

The original PS2 port already contains mature ARM7/ARM9, DMA, timers, IRQ,
IPC, GPU, SPU, SPI, RTC and cartridge logic. Reimplementing all of that in
HG-BOOT1 would cost compatibility and time. HG-BOOT2 keeps that behavior and
replaces PS2-hostile pieces incrementally.

## Next measured targets

1. Build with a current PS2DEV/PS2SDK toolchain.
2. Boot on AetherSX2/PCSX2 and capture the [HG2] log.
3. Verify Pokemon HeartGold reaches ARM9/ARM7 direct boot.
4. Profile ARM interpreter + NDS memory dispatch.
5. Add direct MainRAM/WRAM fast paths.
6. Replace event polling hot paths with cheaper PS2-oriented scheduling where safe.
7. Optimize GPU2D before attempting expensive GPU3D work.
8. Re-enable audio only after the CPU/frame path is stable.

No Nintendo ROM, BIOS or firmware data is included.

## Phase 2: interpreter/memory hot path

- ARM7 Main RAM reads/writes now bypass the generic NDS address dispatcher.
- 8/16/32-bit accesses retain the original alignment and waitstate accounting.
- ARM9 is deliberately left behind CP15 handlers: bypassing CP15 would break
  TCM/protection semantics and is not a safe optimization.
- Instruction fetch already uses melonDS MemRegion caching, so the next CPU
  optimization should be driven by measurements rather than duplicating that path.

The next validation gate is a PS2SDK build plus an HG2 boot log. If it reaches
the direct-boot-ready marker, profile scheduler/IRQ frequency and GPU2D cost.
