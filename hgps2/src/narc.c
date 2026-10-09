#include "hgps2/narc.h"
#include <string.h>

static hg_u16 u16le(const hg_u8 *p) { return (hg_u16)(p[0] | ((hg_u16)p[1] << 8)); }
static hg_u32 u32le(const hg_u8 *p)
{
    return (hg_u32)p[0] | ((hg_u32)p[1] << 8) |
           ((hg_u32)p[2] << 16) | ((hg_u32)p[3] << 24);
}
static int read_at(const HgPs2Narc *n, hg_u32 off, void *buf, size_t len)
{
    return off <= n->archive_size && len <= n->archive_size - off &&
           hgps2_nitrofs_read(n->fs, n->file_id, off, buf, len);
}
static int member_bounds(const HgPs2Narc *n, hg_u32 id,
                         hg_u32 *start, hg_u32 *end)
{
    hg_u8 e[8];
    hg_u32 off;
    if (!n || !n->fs || id >= n->member_count || !start || !end) return 0;
    off = n->fat_entries_offset + id * 8u;
    if (!read_at(n, off, e, sizeof(e))) return 0;
    *start = u32le(e);
    *end = u32le(e + 4);
    return *start <= *end && *end <= n->data_size;
}
int hgps2_narc_open(HgPs2Narc *n, const HgPs2NitroFs *fs, hg_u32 file_id)
{
    hg_u8 header[16], chunk[8], fat[12];
    hg_u32 length, declared, pos, block_size;
    hg_u16 blocks, count;
    int found_fat = 0, found_data = 0;
    if (!n || !fs || !hgps2_nitrofs_file_size(fs, file_id, &length) ||
        length < 16) return 0;
    memset(n, 0, sizeof(*n));
    n->fs = fs;
    n->file_id = file_id;
    n->archive_size = length;
    if (!read_at(n, 0, header, sizeof(header))) return 0;
    if (memcmp(header, "NARC", 4) != 0 || u16le(header + 4) != 0xfffe ||
        u16le(header + 6) != 0x0100 || u16le(header + 12) != 16)
        return 0;
    declared = u32le(header + 8);
    blocks = u16le(header + 14);
    if (declared != length || blocks < 2 || blocks > 16) return 0;
    pos = 16;
    while (blocks--) {
        if (!read_at(n, pos, chunk, sizeof(chunk))) return 0;
        block_size = u32le(chunk + 4);
        if (block_size < 8 || block_size > length - pos) return 0;
        if (memcmp(chunk, "BTAF", 4) == 0 && !found_fat) {
            if (block_size < 12 || !read_at(n, pos, fat, sizeof(fat))) return 0;
            count = u16le(fat + 8);
            if ((hg_u32)count * 8u > block_size - 12u) return 0;
            n->fat_entries_offset = pos + 12;
            n->member_count = count;
            found_fat = 1;
        } else if (memcmp(chunk, "GMIF", 4) == 0 && !found_data) {
            n->data_offset = pos + 8;
            n->data_size = block_size - 8;
            found_data = 1;
        }
        pos += block_size;
    }
    return found_fat && found_data && pos == length;
}
int hgps2_narc_member_size(const HgPs2Narc *n, hg_u32 id, hg_u32 *size)
{
    hg_u32 start, end;
    if (!size || !member_bounds(n, id, &start, &end)) return 0;
    *size = end - start;
    return 1;
}
int hgps2_narc_read(const HgPs2Narc *n, hg_u32 id,
                    hg_u32 offset, void *buffer, size_t size)
{
    hg_u32 start, end;
    if (!member_bounds(n, id, &start, &end) || offset > end - start ||
        size > (size_t)(end - start - offset))
        return 0;
    return read_at(n, n->data_offset + start + offset, buffer, size);
}
