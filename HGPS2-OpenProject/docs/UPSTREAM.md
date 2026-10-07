# Upstream HeartGold reference

Primary reconstruction reference:

- Repository: https://github.com/pret/pokeheartgold
- Default branch: `master`
- Pinned reference commit for the current HGPS2 analysis: `9d8b7591f09b65804da2fb2dfd56f320633e0d36`
- Upstream description: decompilation/disassembly of Pokémon HeartGold/SoulSilver.

HGPS2 does not vendor the upstream project. The repository currently exposes no GitHub-detected license, so HGPS2 treats it as a technical/reconstruction reference and keeps original HGPS2 compatibility code separate.

## Important entry point

The reconstructed game entry point is `src/main.c::NitroMain()`.

The startup path initializes system services, graphics memory, keypad/touch, RTC, overlays, fonts, save data, audio, timers and then enters the game frame loop.

This makes `NitroMain()` the main compatibility boundary for the native PS2 effort.

## Files currently mapped

- `src/main.c` — game startup and main frame loop.
- `src/system.c` — system/input/task initialization mixed with DS-specific IRQ/GX/TP/FS services.
- `src/overlay_manager.c` — mostly portable application state machine.
- `src/poke_overlay.c` — DS overlay loading and ITCM/DTCM/NitroFS-specific behavior.
- `include/system.h` — global input/frame/task state.
- `include/overlay_manager.h` — overlay manager data structures and callbacks.

When upstream changes, update the pinned commit only after reviewing differences that affect HGPS2.
