# HGPS2

Experimental native PlayStation 2 porting layer for Pokemon HeartGold.

This directory is the new native-port track. It does not contain Nintendo ROMs,
game assets, NitroSDK binaries, or proprietary compiler files.

## Architecture

- pret/pokeheartgold is the reconstruction reference for game C/ASM and overlays.
- A user-supplied HeartGold dump is the source for assets/data not stored here.
- hgps2/compat replaces Nintendo DS/Nitro services with PS2 implementations.
- PS2SDK provides the EE/R5900 runtime.
- The old melonDS-PS2 code remains a behavioral oracle while subsystems are replaced.

## Milestone P0

P0 intentionally compiles only the PS2 runtime skeleton. Game source is migrated
subsystem-by-subsystem instead of trying to feed the entire DS build into GCC at once.

Initial compatibility surfaces:
- OS init/ticks
- heap allocation
- pad/input abstraction
- filesystem abstraction
- video abstraction
- audio abstraction
- overlay registry abstraction

The end state is a PS2 ELF packaged into an ISO; it is not a DS emulator.

## External references

- pret/pokeheartgold
- fgsfdsfgs/sm64-port PS2 work
- PS2SDK

See PORTING_PLAN.md.
