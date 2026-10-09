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

## Session entry — 2026-10-08: CI failure diagnosis and regression tests

**Track:** native.
**Finding:** CI run `37781896476` failed in `native-runtime` while `hybrid-bridge` passed. The native job log identifies literal backslash-n sequences in `hgps2/include/hgps2/compat.h` (stray '\\' and unknown type `nint`), introduced in the prior GitHub source edit.
**Fix:** replaced malformed header lines; restored `hgps2/src/compat.c` from the previous valid revision and reinserted the bounded-read function using actual line breaks, preserving existing C string escapes. Corrected implementation commit: `c6c93eb2b3cd0d0793247fd3a5919ff09d09194b`.
**Tests added:** `hgps2/tests/test_file_range.c` checks successful partial read, zero-length read, truncated read, null buffer, and missing file. Workflow `.github/workflows/hgps2.yml` now has a `host-file-tests` job. Commits: `a2480c2ede4fe2924646340922f524e54c120876` and `4a7117bda261ca19156eb2ca6845ee44afeaf3a2`.
**Status:** fixes committed; final CI and runtime results pending. Earlier failed run must not be counted as PASS.
**Next:** verify the latest GitHub Actions run and correct any compiler/test failure before integrating a resource loader.

## Session entry — 2026-10-08: bounded NitroFS file-ID loader

**Track:** native HGPS2.
**CI baseline:** run `37788569112` completed SUCCESS at commit `4a7117bda261ca19156eb2ca6845ee44afeaf3a2` (native ELF, hybrid ELF, host range-read tests). This success predates the NitroFS changes.
**Changes:** added `hgps2/include/hgps2/nitrofs.h` and `hgps2/src/nitrofs.c`. The loader reads NDS header FAT offset/size at 0x48/0x4C, validates FAT bounds, reads one 8-byte FAT entry per request, and loads caller-specified subranges of file-ID resources directly from a user-provided ROM path. It does not allocate a full ROM or cache resources in EE RAM. Added optional ROM-path metadata probe in `hgps2/src/main.c`, added `src/nitrofs.o` to native Makefile, and synthetic-ROM host tests in `hgps2/tests/test_nitrofs.c` plus CI workflow.
**Memory:** fixed stack metadata (80-byte header, 8-byte FAT entry) and caller-owned read buffer; no full-ROM allocation. Reading a resource still requires an external valid ROM path; PS2 optical path / CLI argument behavior needs runtime verification. This is a FAT file-ID primitive, **not** FNT filename resolution, NARC decompression, or game resource integration.
**Security/rights:** test fixture is synthetic; no copyrighted game ROM, BIOS, or assets added.
**Build/runtime:** NitroFS changes awaiting new CI confirmation; no gameplay or on-device test claimed.
**Next:** confirm CI jobs, fix failures, then implement a bounded NARC member reader or game-facing resource API with explicit maximum buffer size.

## Session entry — 2026-10-08: NARC member reader

**Track:** native HGPS2.
**Verified baseline:** GitHub Actions run `37791226933` at `1bc932e9d8d25269ec9466a5eb3b31d38b310f78` completed SUCCESS, including the native ELF and synthetic NitroFS tests. No console runtime test.
**Implementation:** `hgps2/include/hgps2/narc.h` and `hgps2/src/narc.c` parse a NARC header and BTAF/GMIF chunks, index file members, and read bounded subranges through the NitroFS reader without loading the archive. Synthetic NARC test: `hgps2/tests/test_narc.c`. Native Makefile and host CI updated.
**Limitations:** no NARC decompression, BTNF name resolution, asset decoding, or game execution. Current parser requires a 16-byte header, little-endian BOM, version 0x0100, and exact archive length; real game compatibility must be tested against user-owned inputs. Current source tests use synthetic bytes only.
**Build:** new NARC CI run pending at time of entry; do not mark it PASS before results.
**Next:** inspect the latest CI logs, fix any errors, and validate archive layouts against locally supplied lawful game files without committing ROM/assets.
