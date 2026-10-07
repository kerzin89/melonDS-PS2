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


## P0.1 - Native GS frame loop

Implemented on 2026-10-07:

- split the PS2 video backend into `hgps2/src/video_ps2.c`;
- initialize gsKit and dmaKit on the Emotion Engine;
- use a 640x448 16-bit framebuffer;
- enable double buffering;
- execute/synchronize a real GS frame loop;
- run a 300-frame diagnostic phase before entering a persistent render loop;
- animate a visible rectangle and background so a real PS2/PCSX2 test can distinguish a live frame loop from a frozen framebuffer;
- link gsKit/dmaKit in the HGPS2 Makefile.

This milestone intentionally uses original diagnostic graphics rather than copyrighted HeartGold assets. Once the runtime is proven, the renderer can begin accepting game-facing graphics operations.

### Definition of success

The P0.1 ELF/ISO boots, displays the diagnostic screen, visibly animates for at least 300 frames, and remains alive afterward. Serial/console logs should report frames 0, 60, 120, 180 and 240.

### Next

Add native pad input and a minimal HeartGold startup compatibility state machine, then begin mapping the reconstructed `NitroMain()` dependencies.
