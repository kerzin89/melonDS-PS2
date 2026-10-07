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
| First HeartGold-driven framebuffer on PS2 | **PASS** | AetherSX2 reached the post-RunFrame renderer and displayed the 256x384 DS framebuffer texture plus the diagnostic corner marker. Visible game imagery is still blank/white at this early frame. |
| Stable game-frame loop | **PASS** | AetherSX2 visibly cycled the heartbeat across hundreds of consecutive NDS frames. |
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


## Gold first-frame result — PASS

AetherSX2 reached the gold Stage 12 diagnostic color. This color is displayed only after the first isolated `NDS::RunFrame()` returns.

This confirms that the emulator core can advance at least one real HeartGold frame. The remaining failure is in the PS2 presentation path after frame execution.

The renderer inspection found a concrete bug: `GSTEXTURE` was allocated with `malloc()` and only a subset of fields was initialized. Fields such as `Clut`, `Vram`, `VramClut`, `TBW`, `Delayed` and CLUT metadata could contain garbage. `gsKit_TexManager_bind()` checks and uses these fields, so garbage state can cause an invalid palette/cache/VRAM path exactly after the first frame.

The texture is now zero-initialized with `calloc()`, all critical fields are set explicitly, and framebuffer upload is diagnosed separately from sprite submission.


## Framebuffer presentation result — PASS

After zero-initializing `GSTEXTURE` and explicitly initializing its CLUT/VRAM/TBW state, AetherSX2 displayed the 256x384 Nintendo DS framebuffer region on the PS2 GS. The separate white corner marker was also visible, confirming that the post-frame sprite submission and GS flip completed.

The visible DS framebuffer was still solid white in the captured frame. This is not currently treated as a renderer failure: the first DS frame can legitimately be blank/forced-white during startup. The next diagnostic therefore focuses on **multi-frame progression**, not pixel-format changes.

A new autonomous probe executes 300 `NDS::RunFrame()` calls before reintroducing controller/savestate work. It renders every frame and adds:

- a top-left marker cycling red -> green -> blue -> white every 16 frames;
- a second marker that turns green once sampled framebuffer pixels contain data other than known blank/reset values;
- console counters every 30 frames.

If the marker cycles, the emulator is advancing multiple HeartGold frames. If the lower marker turns green, the game framebuffer has begun producing non-blank visual content even if the texture presentation still needs correction.


## Multi-frame progression result — PASS

The 300-frame probe was executed in AetherSX2. The top heartbeat square repeatedly changed colors, paused on white for part of the cycle, then resumed changing. This proves the emulation loop is continuing across many consecutive `NDS::RunFrame()` calls rather than freezing on the first frame.

The second diagnostic square remained green, which means sampled framebuffer contents differed from the initial all-white/all-black/reset patterns at some point during the run.

The large DS surface remained visually white. Source inspection showed that melonDS intentionally renders an inactive/forced-blank DS display as white, so the remaining white surface may reflect DS display state rather than a failed GS upload.

The next diagnostic therefore records ARM9/ARM7 PC movement and both DS `DISPCNT` values, forces `DirectBoot=1`, disables legacy threaded 3D, and converts melonDS BGRA8888 into gsKit RGBA8888 before texture upload.


## Renderer regression rollback

The combined BGRA->RGBA conversion / forced renderer configuration / display-register diagnostic build regressed to a fully black output in AetherSX2. Because the preceding 300-frame build was known-good, those changes were intentionally rolled back rather than debugged as a bundle.

The current CPU-liveness probe is based directly on the verified 300-frame renderer path and adds only:
- ARM9 program-counter movement tracking;
- ARM7 program-counter movement tracking;
- two additional red/green status squares.

No framebuffer conversion, DS display-register reads, or forced Threaded3D change is included in this probe. This keeps the experiment single-variable and preserves the last verified visual path.


## CPU liveness result — PASS

The rollback CPU-liveness probe was executed in AetherSX2 and all four indicators reached green:

1. frame heartbeat is advancing;
2. framebuffer contents are changing;
3. ARM9 program counter is moving;
4. ARM7 program counter is moving.

This confirms that the HeartGold core is not merely repainting a frozen buffer: both emulated processors continue executing across the multi-frame loop.

### Stage 1 interpretation

The project has now proven the following chain in AetherSX2:

`PS2 ISO -> R5900 ELF -> DS core init -> HeartGold ROM load -> ARM9/ARM7 execution -> repeated NDS frames -> changing framebuffer -> PS2 GS presentation`

The large DS surface remains white. This is now treated as a graphics/display-state correctness problem rather than a general boot or CPU-execution failure.

### Next target

Investigate DS display activation and 2D/3D rendering state without disturbing the known-good CPU/frame loop. Changes must be introduced one at a time against the CPU-liveness baseline.
