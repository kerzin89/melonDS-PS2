# HGPS2 — continuity and handoff log

Last updated: 2026-10-08
Branch: `hg-boot2`
Purpose: preserve decisions, verified evidence, unresolved issues, and exact next steps across development sessions.

## Project objective — do not confuse the two tracks

**Primary objective:** native Pokémon HeartGold port for PlayStation 2 (R5900/PS2SDK), using reconstructed game logic and replacing Nintendo DS/Nitro hardware services with PS2 implementations. The native source track is `/hgps2`.

**Secondary experimental track:** `/src` melonDS-PS2 hybrid/diagnostic bridge. This is emulation, not a native game port. Use it as a behavioral reference and for graphics/boot experiments; do not present DISPLAY6 as a completed native port.

References: `hgps2/README.md`, `hgps2/PORTING_PLAN.md`, `HGPS2-OpenProject/README.md`, `HGPS2-OpenProject/docs/STAGE1_STATUS.md`, `HG_BOOT2.md`.

## Confirmed from repository documentation

- The native prototype includes PS2SDK/R5900 build infrastructure, GS frame presentation, timing/FPS, pad input, keypad repeat logic and an overlay lifecycle manager. Runtime verification on physical PS2 is still pending; see `STAGE1_STATUS.md`.
- The hybrid bridge previously reached `NDS::RunFrame()` and drew a DS framebuffer texture on the PS2 GS.
- A `GSTEXTURE` initialization bug was documented and corrected using zero initialization and explicit texture metadata.
- Multi-frame diagnostic heartbeat was observed advancing in AetherSX2 while the main DS surface remained white.
- A six-indicator display-engine diagnostic was added in recent commits; check `HG_BOOT2.md`, `STAGE1_STATUS.md`, and the commit history before changing the renderer.
- `HG_BOOT2.md` describes ROM streaming from a backing file to reduce EE RAM usage: keep the first 32 KiB resident, stream remaining cartridge data, and load ARM sections in chunks. This optimization belongs to the **hybrid emulator track**; it is not evidence that the native port uses file-backed virtual RAM.

## Important open questions

1. For DISPLAY6, determine whether blank/white output is caused by the DS display state, frame generation, pixel transfer, or cache coherency. Do not attribute it to ROM streaming without a controlled test.
2. For the native track, verify the latest CI build and then run the P0 ELF in PCSX2/AetherSX2 or hardware; confirm GS diagnostic rendering and pad input.
3. Inventory the actual reconstructed HeartGold dependencies required for the first native game-driven screen. Do not claim gameplay code has been ported without evidence.

## Immediate development order

1. Review `hgps2/src`, `hgps2/include`, `hgps2/Makefile` and the native build workflow.
2. Check CI status and exact commit SHA; reproduce the native P0 build.
3. Test P0 runtime and record video/input/log observations separately from compilation results.
4. Implement one narrow native subsystem or game-facing API at a time, with a test for each.
5. Keep hybrid DISPLAY6 diagnostics as a separate line of work; never silently merge emulator fixes into native-port status.

## Logging rules for every future session

Append a dated entry with:
- **Track:** native or hybrid.
- **Baseline:** branch and commit SHA.
- **Goal and files changed:** exact paths.
- **Build:** command, toolchain, result, artifact hash/path if available.
- **Runtime test:** emulator/hardware, test input, observed result, logs/screenshots.
- **Status:** PASS (tested), BUILD ONLY (compiled), WIP, or BLOCKED.
- **Next step:** one concrete action.

Never mark a change tested merely because it compiled. Never claim an ISO/ELF exists or is playable without verifying the artifact and test. Do not commit ROMs, BIOS, firmware, extracted proprietary assets, or credentials.

## Session entry — 2026-10-08

**Track:** both, planning/continuity only.
**Baseline:** `hg-boot2`; recent repository history includes the six-indicator display-engine probe.
**Finding:** the existing `HGPS2-OpenProject` and `hgps2` folders already define the native port, while `src` and `HG_BOOT2.md` document the emulation-based experimental bridge. Earlier discussion conflated the two.
**Action:** added this continuity log to prevent losing prior decisions and repeating already documented diagnostics.
**Build/runtime:** not executed in this session.
**Next:** inspect native runtime source and CI, then implement/test the next native milestone; log evidence here.

## Session entry — 2026-10-08: native bounded file reads

**Track:** native HGPS2 only (`hgps2/`).
**Baseline:** `hg-boot2`; prior native build success recorded by GitHub Actions at commit `6e001a1981682e31981d24c0a5397b381f1cf8c2`. This is an earlier build, not validation of the changes below.
**Goal:** avoid loading entire large game-data files into 32 MiB EE RAM.
**Files changed:** `hgps2/include/hgps2/compat.h`, `hgps2/src/compat.c`.
**Changes:** new `hgps2_file_read_range(path, offset, buffer, size)` API using seek + exact-length read, with null-pointer and signed-long offset checks. Caller owns the bounded buffer; no full-file allocation. Existing `hgps2_file_read_all` remains unchanged for smaller files.
**Commits:** `eb315086337322113a41c3f266b33b76cc56f3f7` (declaration), `78d3bff6ecd20c5398b7ddf177f612e947de0c2e` (implementation).
**Build/runtime:** NOT YET VERIFIED for these commits. GitHub Actions should be checked after the push. No PS2 runtime test or game-data integration has been performed.
**Next step:** verify CI, add host-side boundary tests (zero-length, short-read, out-of-range, valid offsets), then use this API in a game asset loader with a strict memory budget. Do not confuse streaming with virtual RAM or claim that game assets are ported.
