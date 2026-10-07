# ROM and proprietary-content policy

HGPS2 does not distribute Pokémon HeartGold/SoulSilver ROM images, Nintendo DS BIOS/firmware, Nintendo SDK binaries, or extracted commercial game assets.

A contributor may analyze a dump they are legally entitled to use and may contribute original interoperability code, documentation, metadata, hashes, scripts, patches, or clean-room replacements when they have the right to do so.

Do not open pull requests containing:

- `.nds` ROM images;
- Nintendo DS BIOS/firmware;
- extracted ARM9/ARM7 game binaries;
- extracted graphics, music, maps, text or other proprietary game assets;
- Nintendo NitroSDK binaries/source;
- keys or circumvention material.

The intended build design is similar to other reconstruction/porting projects: the user supplies their own game dump locally and tooling derives required data during the build.

The repository's license applies only to material that contributors have the right to license. It does not grant rights to Pokémon, Nintendo, Game Freak, Creatures, Sony, or third-party project material.
