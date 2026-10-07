# Stage 1 — Functionality status

Last updated: 2026-10-07

Legend: **PASS** = verified by build/test; **READY** = implemented and compiled but runtime test pending; **WIP** = being implemented; **BLOCKED** = external/test dependency.

| Micro-stage | Status | Evidence / next test |
| --- | --- | --- |
| Native R5900 toolchain build | **PASS** | `HGPS2.ELF` compiled successfully in the official PS2DEV container in GitHub Actions. |
| Native GS frame loop | **READY** | gsKit/dmaKit double-buffered loop compiles; real PS2/PCSX2 execution still must be confirmed. |
| PS2 timing/FPS measurement | **READY** | PS2SDK system timer backend and FPS sampling compile in the native target. |
| DualShock 2 input | **READY** | Pad backend compiles and maps PS2 controls to HeartGold/DS keypad bits; runtime test pending. |
| HeartGold-style input repeat state | **READY** | held/new/repeated state logic implemented from the reconstructed behavior. |
| Native overlay lifecycle | **READY** | `init -> exec -> exit` manager implemented without DS ITCM/DTCM placement. |
| Private hybrid ISO assembly | **PASS (structure)** | ISO9660/Joliet image assembled from the freshly compiled hybrid ELF and byte-checked against the user-supplied ROM/BIOS inputs. Runtime test still pending. |
| Hybrid bridge current-source build | **PASS** | the current branch now compiles successfully with the official PS2DEV toolchain. |
| First genuine HeartGold frame on PS2 | **WIP: first-frame probe** | ROM/BIOS resolution and cartridge loading now reach the transition into the frame loop; isolated RunFrame/render probe is ready for AetherSX2 test. |
| Stable game-frame loop | **WIP** | depends on first genuine frame. |
| Minimum game input | **WIP** | native pad layer exists; game integration depends on frame/boot path. |
| Minimum audio | **WIP** | native HGPS2 audio remains optional/stubbed for first visual milestone. |
| Minimum persistent save | **WIP** | hybrid optical-disc builds route save files away from the read-only ISO; native Memory Card backend remains later Stage 1 work. |

## Current native runtime

The native runtime now includes:

- R5900/PS2SDK build;
- GS frame presentation;
- PS2 system timer;
- FPS measurement;
- DualShock 2 input;
- HeartGold-compatible keypad bit layout and repeat state;
- native overlay/application lifecycle;
- diagnostic logs for frame count, FPS and button transitions.

This is infrastructure, not yet the game.

## Current hybrid bridge

The legacy melonDS PS2 core remains a temporary bridge for the earliest real-game-frame experiment. It can:

- execute `NDS::RunFrame()`;
- expose `GPU::Framebuffer` to the PS2 GS path;
- probe optical-disc HeartGold paths;
- read BIOS/firmware from ISO or USB after current-source rebuild;
- redirect writable save state away from optical media.

The hybrid bridge will be removed or reduced as native HGPS2 replacements become functional.

The current-source hybrid bridge and native runtime both pass the R5900 build in GitHub Actions. The private Stage 1 ISO places `HEARTGOLD.NDS`, BIOS7, BIOS9 and firmware at paths expected by the bridge and keeps saves on writable external storage.

## Immediate exit condition

The next major Stage 1 checkpoint is **one verified, advancing genuine HeartGold framebuffer on PCSX2 or real PS2**. Until that is observed, no game-FPS claim is valid.


## Black-screen diagnostic — 2026-10-07

The first private Stage 1 HeartGold ISO reached the hardware test but produced a black screen. The bridge previously initialized GS only after IOP reset/module loading, which made every early boot failure indistinguishable from an ELF that never started.

The bridge now initializes GS immediately on EE entry and displays a sequence of solid boot-stage colors before the game framebuffer is available:

- red: EE + GS entered;
- orange: IOP reset/sync complete;
- yellow: core IOP modules complete;
- blue: storage modules complete;
- green: pad/USB path complete;
- purple: font/UI initialized and ROM lookup starting;
- cyan: ROM/BIOS paths resolved and NDS core startup beginning;
- magenta: bounded IOP reset/sync timeout.

The IOP reset and sync loops are now bounded instead of being able to hang forever on a black screen. Both native and hybrid targets compile successfully after these diagnostics were added.


## From-scratch sanity ISO — 2026-10-07

After a second black-screen report, the ISO path was isolated from the HeartGold/melonDS path completely.

A new `hgps2/sanity` target now builds a minimal R5900 ELF that only:

1. initializes gsKit/dmaKit;
2. initializes the GS;
3. clears the screen red;
4. draws a white rectangle;
5. remains in the GS flip loop.

It does not initialize pad, audio, filesystem, IOP modules, melonDS, BIOS or HeartGold.

The sanity ISO is built from scratch with xorriso in CI rather than modifying the previous ISO template. Its `SYSTEM.CNF` uses the canonical single-backslash boot path:

`BOOT2 = cdrom0:\HGBOOT.ELF;1`

The first xorriso build exposed and fixed an escaping bug that had produced two backslashes in the sanity `BOOT2` path. The corrected sanity ISO now builds successfully and is the lowest-level test for the user's launcher/emulator/console path.

Interpretation:
- red screen + white rectangle = ISO loader, ELF startup, DMA and GS are proven;
- black screen = failure occurs before or during minimal ELF/GS startup and is unrelated to HeartGold;
- sanity passes but hybrid stays black = continue debugging the hybrid initialization path.


## AetherSX2 hybrid observation: cyan -> black

The private pre-main-fix build progressed through all early boot colors. The captured final diagnostic color before black was verified as the Stage 7 cyan value (`RGB 0,96,96`).

In that build, Stage 7 cyan is displayed immediately before `NDS::Init()`. The transition to black only occurs after `NDS::LoadROM()` reports success. This narrows the failure from general boot/ROM loading to the first emulation/render loop.

A new first-frame probe was committed in `src/ps2/main.cpp`:

- cyan — ROM/BIOS paths resolved, about to initialize NDS;
- lime — cartridge loaded successfully;
- gray — `SetScreenLayout()` returned;
- pink — GS texture wrapper for `GPU::Framebuffer` allocated/configured;
- navy — immediately before the first isolated `NDS::RunFrame()`;
- gold — the first `NDS::RunFrame()` returned;
- game framebuffer with a white/green corner heartbeat — draw/flip works and frames continue advancing.

The probe intentionally executes one NDS frame before pad polling, savestates or semaphore synchronization so those systems cannot hide the first-frame result.
