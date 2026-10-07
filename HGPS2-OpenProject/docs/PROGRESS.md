# Progress

## Completed research

- Located and inspected the original PS2 melonDS port.
- Implemented experimental streamed ROM access/direct boot changes in the `hg-boot2` branch.
- Reduced some unnecessary memory/work in the emulator path.
- Inspected the old PS2 ELF and confirmed R5900/N32 MIPS characteristics and useful symbols.
- Pivoted from full DS emulation to a native game-specific port.
- Located pret/pokeheartgold as the primary reconstruction reference.
- Mapped the target dump header, ARM binaries, overlay tables, FNT/FAT and NitroFS metadata.
- Located the reconstructed HeartGold `NitroMain()` startup flow.
- Created `hgps2/include/hgps2/compat.h`, `hgps2/src/compat.c`, `hgps2/src/main.c`, PS2SDK Makefile and `SYSTEM.CNF`.
- Added a GitHub Actions definition using the PS2DEV toolchain image.

## Current state

The native runtime is an early skeleton. It is not a playable HeartGold port. Pad/video/audio functions still contain P0 stubs and the reconstructed game logic is not yet linked into the R5900 target.

## Next technical work

- make the P0 ELF reproducibly compile in CI;
- implement real PS2 controller input;
- establish frame/tick timing;
- inventory the minimum dependency graph from `NitroMain()` to the intro/title path;
- replace overlay loading with a native registry;
- implement graphics initialization and first visible diagnostic frame;
- progressively connect reconstructed game code.

## Historical commits

The `hg-boot2` branch also contains earlier emulator experiments. They are intentionally retained as research/reference rather than presented as the final architecture.
