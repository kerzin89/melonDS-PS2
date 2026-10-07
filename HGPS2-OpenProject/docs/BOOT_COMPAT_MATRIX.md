# HeartGold -> PS2 boot compatibility matrix

Reference: pret/pokeheartgold @ `9d8b7591f09b65804da2fb2dfd56f320633e0d36`.

This matrix starts at `NitroMain()` and classifies the earliest dependencies needed to reach the title/intro path.

| HeartGold service | Upstream behavior | HGPS2 strategy | Status |
| --- | --- | --- | --- |
| `InitSystemForTheGame` | OS, fixed-point, GX, tick, heaps, task queues, IRQ, FS, CRC | split into portable state init + PS2 timing/heap/filesystem/video backends | mapped |
| `InitGraphicMemory` | clears DS VRAM/OAM/palette regions | no DS VRAM emulation in native path; initialize HGPS2 render resources | mapped |
| `InitKeypadAndTouchpad` | resets input state and initializes/calibrates touch panel | preserve state/reset semantics; source input from DualShock 2; virtual touch later | mapped |
| `ReadKeypadAndTouchpad` | reads PAD + touch and computes held/new/repeat states | keep the state-transition algorithm; replace raw device reads with PS2 pad backend | mapped |
| `OS_InitTick / OS_GetTick` | DS timing | PS2 EE timer/clock abstraction | partial |
| VBlank IRQ/waits | drives frame/task timing | GS/VSync based frame event | partial |
| `FS_Init / FS_*` | NitroFS filesystem | local asset/package filesystem generated from user-owned dump | planned |
| `HandleLoadOverlay` | Nitro overlay load into main RAM/ITCM/DTCM | native registry; no literal DS overlay RAM placement | planned |
| `OverlayManager_Run` | init/exec/exit state machine | preserve semantics in native HGPS2 implementation | portable candidate |
| RTC | DS RTC | PS2 clock/RTC compatibility layer | planned |
| Save/CARD | DS backup flash | HGPS2 save provider; Memory Card/file backend | planned |
| Sound | DS sound runtime | PS2 audio backend | planned |
| GX/G2/G3/NNS graphics | DS graphics hardware/API | translate game-facing operations into PS2 renderer | planned |
| lid/backlight/power | DS physical state | safe no-op / PS2-specific behavior | planned |
| WFC/wireless | DS wireless | unsupported stub during Stage 1 | planned |

## Key architectural decision

Do not emulate ITCM/DTCM placement merely because the original overlay loader did so. Native R5900 code can keep compiled application modules resident and use a registry of overlay IDs/templates.

The portable part worth preserving is the application lifecycle:

`register -> init -> execute frames -> exit -> release`

not the DS memory-loading mechanism.

## Stage 1 integration order

1. timing/VBlank abstraction;
2. input-state compatibility;
3. task queues and heap boundary;
4. native overlay manager;
5. filesystem access to locally extracted user-owned game data;
6. minimal graphics bridge;
7. intro/title application path.

The temporary hybrid melonDS route remains available to produce real-game-frame diagnostics while these pieces are replaced natively.
