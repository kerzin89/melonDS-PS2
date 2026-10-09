#ifndef HGPS2_NARC_H
#define HGPS2_NARC_H
#include "hgps2/nitrofs.h"

/* NARC archive inside one NitroFS file ID. All offsets are relative to that
 * file; no archive payload is allocated. The caller owns output buffers. */
typedef struct HgPs2Narc {
    const HgPs2NitroFs *fs;
    hg_u32 file_id;
    hg_u32 archive_size;
    hg_u32 fat_entries_offset;
    hg_u32 data_offset;
    hg_u32 data_size;
    hg_u32 member_count;
} HgPs2Narc;

int hgps2_narc_open(HgPs2Narc *narc, const HgPs2NitroFs *fs, hg_u32 file_id);
int hgps2_narc_member_size(const HgPs2Narc *narc, hg_u32 member_id, hg_u32 *size);
int hgps2_narc_read(const HgPs2Narc *narc, hg_u32 member_id,
                    hg_u32 offset, void *buffer, size_t size);
#endif
