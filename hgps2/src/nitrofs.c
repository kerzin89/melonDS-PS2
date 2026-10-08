#include "hgps2/nitrofs.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>

static hg_u32 read_le32(const hg_u8 *p)
{
    return (hg_u32)p[0] | ((hg_u32)p[1] << 8) |
           ((hg_u32)p[2] << 16) | ((hg_u32)p[3] << 24);
}

static int fat_entry(const HgPs2NitroFs *fs, hg_u32 id,
                     hg_u32 *start, hg_u32 *end)
{
    hg_u8 entry[8];
    size_t entry_offset;
    if (!fs || !fs->rom_path || id >= fs->file_count || !start || !end)
        return 0;
    entry_offset = (size_t)fs->fat_offset + (size_t)id * 8u;
    if (!hgps2_file_read_range(fs->rom_path, entry_offset, entry, sizeof(entry)))
        return 0;
    *start = read_le32(entry);
    *end = read_le32(entry + 4);
    return *start <= *end && *end <= fs->rom_size;
}

int hgps2_nitrofs_open(HgPs2NitroFs *fs, const char *rom_path)
{
    hg_u8 hdr[0x50];
    hg_u32 fat_offset, fat_size;
    FILE *f;
    long length;
    if (!fs || !rom_path) return 0;
    memset(fs, 0, sizeof(*fs));
    f = fopen(rom_path, "rb");
    if (!f) return 0;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return 0; }
    length = ftell(f);
    fclose(f);
    if (length < 0 || (unsigned long)length > UINT32_MAX || length < 0x50)
        return 0;
    if (!hgps2_file_read_range(rom_path, 0, hdr, sizeof(hdr)))
        return 0;
    /* NDS header: FAT offset at 0x48, FAT size at 0x4C. */
    fat_offset = read_le32(hdr + 0x48);
    fat_size = read_le32(hdr + 0x4c);
    if (!fat_size || (fat_size & 7u) || fat_offset > (hg_u32)length ||
        fat_size > (hg_u32)length - fat_offset)
        return 0;
    fs->rom_path = rom_path;
    fs->fat_offset = fat_offset;
    fs->file_count = fat_size / 8u;
    fs->rom_size = (hg_u32)length;
    return 1;
}

int hgps2_nitrofs_file_size(const HgPs2NitroFs *fs, hg_u32 file_id, hg_u32 *size)
{
    hg_u32 start, end;
    if (!size || !fat_entry(fs, file_id, &start, &end)) return 0;
    *size = end - start;
    return 1;
}

int hgps2_nitrofs_read(const HgPs2NitroFs *fs, hg_u32 file_id,
                      hg_u32 offset, void *buffer, size_t size)
{
    hg_u32 start, end;
    if (!fat_entry(fs, file_id, &start, &end) ||
        offset > end - start || size > (size_t)(end - start - offset))
        return 0;
    return hgps2_file_read_range(fs->rom_path, (size_t)start + offset, buffer, size);
}
