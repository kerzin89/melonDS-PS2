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

The native runtime is an early Stage 1 compatibility runtime, not a playable HeartGold port. The R5900 target now builds successfully in the official PS2DEV container. GS video, PS2 timing, DualShock 2 input, HeartGold-like keypad state and a native overlay lifecycle are implemented. Native audio and actual reconstructed HeartGold game integration are still incomplete.

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


## P0.2 - Hybrid real-frame bridge

A temporary compatibility route now exists for reaching real HeartGold frames before the native port is complete.

The legacy PS2 melonDS frontend probes the boot disc for `HEARTGOLD.NDS` through `cdrom0:` / `cdfs:` paths before opening the USB browser. If the embedded user-owned dump is accessible, the existing core proceeds through `NDS::LoadROM()`, `NDS::RunFrame()`, and uploads `GPU::Framebuffer` to the PS2 GS.

This is a transitional diagnostic path, **not the final architecture**. It lets us answer an important question early: can the PS2 build advance this specific game far enough to produce genuine game frames?

A private ISO builder is provided at `tools/make_hybrid_iso.sh`. It requires the user to supply their own HeartGold dump, DS BIOS/firmware and compiled PS2 ELF. Those proprietary inputs are never committed.

Current unverified risk: CD/DVD filesystem initialization and newlib path behavior on real hardware/PCSX2. The code probes both `cdrom0:` and `cdfs:` spellings and logs each attempt. PS2SDK provides CD/DVD access, but this stage must be tested rather than assumed successful.

If the hybrid route reaches game frames, it becomes a behavioral oracle while HGPS2 replaces emulator services one subsystem at a time.


## Stage 1 build/runtime update — 2026-10-07

The native `HGPS2.ELF` has now compiled successfully through GitHub Actions using the current PS2DEV toolchain. This verifies the R5900 build path; it does **not** yet verify execution on PCSX2 or physical hardware.

A private test ISO was also assembled from user-supplied HeartGold/BIOS/firmware inputs and structurally validated as ISO9660/Joliet. The commercial inputs are not stored in this repository. No real HeartGold frame has yet been confirmed from that ISO.

The legacy hybrid bridge exposed additional modern-toolchain build errors, which are being fixed in CI rather than hidden. See `STAGE1_STATUS.md` for the current factual checklist.
