# HGPS2 porting plan

## Rule 1: preserve game logic, replace hardware

Gameplay C should change as little as practical. NitroSDK calls, DS MMIO, ARM-only
assembly and DS graphics/audio backends are the boundary to replace.

## P0 - native runtime
Build a PS2 ELF that boots, initializes the compatibility layer, polls the pad and
renders a diagnostic frame.

## P1 - source inventory
Import/track upstream pokeheartgold source without vendoring ROM output. Classify
translation units as portable C, Nitro-dependent C, ARM assembly, graphics, audio,
filesystem, wireless, or DS-hardware-specific.

## P2 - core services
Implement heap, time, filesystem, save data, input and overlay management shims.

## P3 - renderer
Translate the game-facing BG/sprite/texture operations to a PS2 GS backend. Do not
emulate DS MMIO if a higher-level game call can be mapped directly.

## P4 - game boot path
Port the minimum title/new-game path and resolve dependencies outward from that call
graph. Keep unsupported services as loud stubs.

## P5 - field/battle
Bring up overworld first, then battle renderer and audio. Profile EE memory and GS
uploads continuously.

## P6 - packaging
Extract required user-owned data at build time and generate SYSTEM.CNF + PS2 ISO.

## Assembly policy
ARM/Thumb assembly is not mechanically linked into the PS2 build. Prefer an existing
matching C reconstruction. For remaining ASM, use REA/Ghidra plus call graphs and
existing symbols to reconstruct semantics, then compile the reconstruction for R5900.
A tiny interpreter is only a temporary fallback for unusually difficult functions.
