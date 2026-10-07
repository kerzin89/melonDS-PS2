# Architecture

## Principle

Preserve portable game logic. Replace Nintendo DS/Nitro hardware and operating-system services with PlayStation 2 implementations.

## Original startup path

The reconstructed HeartGold `NitroMain()` performs, broadly:

1. system initialization
2. graphics-memory initialization
3. keypad/touch initialization
4. power/backlight setup
5. 3D swap state
6. RTC initialization
7. overlay-manager reset
8. fonts
9. save allocation/detection
10. sound initialization
11. timer initialization
12. intro/main-menu overlay registration
13. RNG initialization
14. per-frame task, input, RTC, graphics, fade and audio loop

HGPS2 will replace those platform-facing services rather than emulate DS MMIO whenever practical.

## Layers

### Game logic
Reconstructed portable logic, with architecture-specific pieces converted carefully.

### HGPS2 compatibility
Stable interfaces for memory, timing, input, files, saves, video, audio, overlays and logging.

### PS2 backend
PS2SDK/R5900 implementations. Graphics eventually target GS/gsKit-style primitives; audio targets PS2 facilities; controller input maps DS buttons to DualShock 2.

### Assets
Never stored as a commercial ROM in this repository. A future local extractor will accept a user-supplied dump and create build-time data.

## Overlays

The target ROM contains 129 ARM9 overlays and no ARM7 overlays. Native PS2 code does not need to reproduce the DS overlay memory mechanism byte-for-byte. The preferred design is a native registry/module model preserving application state and call behavior.

## Assembly

ARM/Thumb assembly cannot be linked into an R5900 executable. Priority order:

1. use an already reconstructed matching C implementation;
2. reconstruct behavior into portable C;
3. use reverse-engineering tools to understand call graphs and semantics;
4. use a small interpreter only as a temporary fallback for unusually difficult code.

## Milestones

P0: R5900 runtime boots and logs.
P1: source/dependency inventory.
P2: timing, heap, input, filesystem, save and overlay shims.
P3: graphics backend.
P4: title/new-game boot path.
P5: overworld and battle.
P6: reproducible user-owned asset extraction and PS2 packaging.
