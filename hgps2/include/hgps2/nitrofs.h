#ifndef HGPS2_NITROFS_H
#define HGPS2_NITROFS_H

#include "hgps2/compat.h"

/* HeartGold NitroFS file IDs are indexed through the cartridge FAT.
 * The cartridge file remains on disk; only a bounded caller-owned buffer
 * is needed. No ARM emulation, game code, or assets are included. */
typedef struct HgPs2NitroFs {
    const char *rom_path;
    hg_u32 fat_offset;
    hg_u32 file_count;
    hg_u32 rom_size;
} HgPs2NitroFs;

int hgps2_nitrofs_open(HgPs2NitroFs *fs, const char *rom_path);
int hgps2_nitrofs_file_size(const HgPs2NitroFs *fs, hg_u32 file_id, hg_u32 *size);
int hgps2_nitrofs_read(const HgPs2NitroFs *fs, hg_u32 file_id,
                      hg_u32 offset, void *buffer, size_t size);

#endif
