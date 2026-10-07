# HGPS2 Development Stages

The project is divided into three major stages. A stage is only considered complete when its acceptance criteria are met.

## Stage 1 — Functionality

Goal: make HeartGold actually run on PlayStation 2 hardware/emulation, even if incomplete, ugly or unstable.

### 1.1 Boot
- PS2 ISO is recognized by OPL/PCSX2.
- `SYSTEM.CNF` launches the correct ELF.
- Runtime reaches the game path without returning to browser/menu.
- Boot failures produce useful logs instead of silent black screens.

### 1.2 Real game frames
- HeartGold ROM/data is found from the private test ISO or external media.
- The game core initializes successfully.
- At least one genuine HeartGold framebuffer reaches the PS2 GS.
- The frame counter advances continuously rather than displaying a frozen image.

### 1.3 Stable frame loop
- Multiple consecutive game frames execute without crashing.
- VSync and framebuffer upload remain synchronized.
- No unnecessary framebuffer copies in the hot path.
- Measure actual FPS and frame time.

Initial target:
- first measurable goal: 10+ FPS in a reproducible test scene;
- intermediate target: 20+ FPS;
- Stage 1 exit target: stable enough to navigate the title/menu without constant hangs.

The exact final FPS target will be revised from measurements rather than guessed in advance.

### 1.4 Input
- DualShock 2 D-pad maps to DS D-pad.
- Cross/Circle/Square/Triangle are mapped to A/B/X/Y.
- Start/Select/L1/R1 are usable.
- Touchscreen-dependent actions have at least a temporary controller mapping.

### 1.5 Minimal audio
- Audio subsystem initializes.
- Game produces recognizable audio.
- Audio failure must not stop video/game execution.
- Basic synchronization is measured.

### 1.6 Minimal save path
- Game can start without a save.
- Save backend does not corrupt memory.
- Read/write path is isolated for later Memory Card support.

### Stage 1 acceptance criteria

Stage 1 is complete when a private PS2 ISO can boot HeartGold, display continuously advancing real game frames, accept enough controller input to navigate the initial game/menu, and remain running long enough for reproducible testing.

---

## Stage 2 — Correction

Goal: take the functioning build and systematically remove bugs, incompatibilities and major performance problems.

### 2.1 Graphics correctness
- Correct top/bottom screen composition.
- Fix corrupted textures, palettes, sprites and backgrounds.
- Fix 2D/3D ordering and blending.
- Correct aspect ratio/scaling.
- Resolve flicker and framebuffer artifacts.

### 2.2 Performance
- Profile the R5900 hot paths.
- Reduce ARM emulation or replace it with native reconstructed C where possible.
- Remove redundant memory copies.
- Optimize texture uploads and GS synchronization.
- Reduce stalls in overlay/file access.
- Establish repeatable FPS benchmarks for title, overworld and battle.

### 2.3 Input correctness
- Fix missed/repeated inputs.
- Improve touchscreen emulation/mapping.
- Add configurable layout if necessary.

### 2.4 Audio correctness
- Correct sample-rate conversion.
- Reduce crackling, underruns and latency.
- Synchronize audio with frame timing.

### 2.5 Save correctness
- Implement reliable save persistence.
- Add PS2 Memory Card backend or another explicit supported save target.
- Validate save/load cycles and recovery from interrupted writes.

### 2.6 Game-specific bugs
Track issues by reproducible location:
- boot/title;
- intro/new game;
- overworld;
- menus;
- dialogue;
- battles;
- transitions;
- scripted events;
- map/overlay changes.

Each issue should include expected behavior, observed behavior, reproduction steps and whether it happens on real PS2, PCSX2 or both.

### Stage 2 acceptance criteria

Stage 2 is complete when the main game loop is reliable, major graphical/audio/input/save bugs are fixed, FPS is predictable, and normal progression no longer depends on developer workarounds.

---

## Stage 3 — Playable

Goal: turn the corrected prototype into something a normal player can reasonably use.

### 3.1 Full progression
- New game works.
- Existing save loads correctly.
- Overworld traversal works.
- Battles work.
- Menus and inventory work.
- Major scripted sequences work.
- Map/overlay transitions are reliable.

### 3.2 Long-session stability
- Test 30-minute, 1-hour and multi-hour sessions.
- No progressive memory leaks.
- No increasing audio desynchronization.
- No random overlay/file failures.

### 3.3 User-facing controls
- Final DualShock 2 mapping.
- Touchscreen solution documented.
- Reset/exit behavior.
- Optional control configuration if practical.

### 3.4 Packaging
- Reproducible private ISO builder.
- User supplies their own HeartGold dump and required firmware/BIOS when still necessary.
- No copyrighted ROM/game assets distributed by this repository.
- OPL and PCSX2 instructions.
- Build/version identification shown in logs.

### 3.5 Release quality
- Known-issues list.
- Compatibility notes.
- Performance expectations.
- Reproducible build instructions.
- Tagged test releases.

### Stage 3 acceptance criteria

The project can reasonably be called playable when a user can boot the game, control it, save progress, play normal game sections for extended periods, and continue progression without routine crashes or developer intervention.

---

## Development rule

Do not optimize features belonging to a later stage if an earlier-stage blocker prevents basic testing.

Priority order:

`boot -> real frames -> stable frames -> input -> minimum audio/save -> correctness -> performance -> full playability`

Every meaningful project change should update this roadmap/progress documentation together with the code.
