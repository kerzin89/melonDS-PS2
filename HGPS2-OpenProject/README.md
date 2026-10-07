# HGPS2 Open Project

Experimental effort to bring Pokémon HeartGold's reconstructed game logic to the PlayStation 2 as native R5900 code.

> Status: research / early bring-up. This is **not yet a playable port**.

## Goal

The project does not aim to emulate an entire Nintendo DS. The long-term architecture is:

```
user-owned HeartGold dump
        |
        +--> metadata/assets extraction (local only)
        |
pret/pokeheartgold reconstructed logic
        |
        +--> remaining ARM/Thumb analysis where necessary
        |
        v
HGPS2 compatibility layer
        |
        +-- PS2SDK / R5900
        +-- GS graphics backend
        +-- pad input
        +-- audio
        +-- filesystem/save
        +-- RTC/timing
        v
     PS2 ELF
```

The old melonDS-PS2 code remains useful as a behavioral reference and fallback while the native port is developed.

## What has been done

1. Investigated the existing melonDS PS2 port and its memory/ROM-loading bottlenecks.
2. Added streaming/direct-boot experiments on the `hg-boot2` branch.
3. Inspected a user-owned HeartGold dump and mapped the NDS boot structures.
4. Identified the reconstructed `NitroMain()` in pret/pokeheartgold and its startup sequence.
5. Created the initial HGPS2 native compatibility runtime under `/hgps2`.
6. Added a PS2SDK/R5900 GitHub Actions build definition.
7. Established the native-port strategy: preserve game logic and replace DS/Nitro hardware services.

## ROM analysis used during development

The private development dump identified itself as:

- Internal title: `POKEMON HG`
- Game code: `IPKE`
- ROM size: 128 MiB
- ARM9 offset: `0x4000`
- ARM7 offset: `0x2F7400`
- ARM9 overlays: 129
- ARM7 overlays: 0
- FAT entries: 513
- SHA-256: `3c6e41fe038b616c425ca45ce5da74d8af77f71698a90db41982b5b82cbbadd3`

The ROM itself and extracted copyrighted game binaries/assets are intentionally **not included**.

## Source references

- pret/pokeheartgold — ongoing HeartGold/SoulSilver reconstruction. It provides reconstructed C and remaining ARM/Thumb assembly used as a reference for this effort.
- PS2DEV / PS2SDK — PlayStation 2 homebrew toolchain/runtime.
- melonDS — Nintendo DS emulator used as a behavioral reference.
- Super Mario 64 decomp/ports — architectural inspiration for separating reconstructed game logic from platform backends.

No affiliation or endorsement by Nintendo, The Pokémon Company, Game Freak, Sony, pret, melonDS, or PS2DEV is implied.

## Current layout

- `/hgps2` — current native PS2 runtime prototype.
- `/HGPS2-OpenProject/docs` — research notes and porting documentation.
- `/HGPS2-OpenProject/tools` — clean-room/local tooling added as development progresses.
- `.github/workflows/hgps2.yml` — R5900 build workflow.

## How to contribute

Help is welcome in R5900/PS2SDK bring-up, GS rendering, pad mapping, SPU2 audio, filesystem/save handling, RTC, overlay conversion, dependency inventory, and reconstruction of functions that remain ARM/Thumb assembly.

Do not submit commercial ROMs, BIOS files, Nintendo SDK binaries, extracted copyrighted assets, or other proprietary files.

See `docs/STAGES.md`, `docs/STAGE1_STATUS.md`, `docs/ARCHITECTURE.md`, `docs/PROGRESS.md`, `docs/ROM_POLICY.md`, and `CONTRIBUTING.md`.

## License

Original HGPS2 code and documentation in this project are released under the MIT License where the contributor has the right to license that material. Third-party projects and reconstructed game material retain their own licenses/copyrights. See `LICENSE` and `docs/ROM_POLICY.md`.
