#include "hgps2/resource.h"
#include <string.h>

/* Nitro graphics container FourCCs are stored in byte-reversed order.
 * This is signature classification, not a renderer or decompressor. */
HgPs2ResourceKind hgps2_resource_identify(const void *bytes, size_t length)
{
    const unsigned char *p = (const unsigned char *)bytes;
    if (!p || length < 4) return HGPS2_RESOURCE_UNKNOWN;
    if (!memcmp(p, "NARC", 4)) return HGPS2_RESOURCE_NARC;
    if (!memcmp(p, "RGCN", 4)) return HGPS2_RESOURCE_NCGR;
    if (!memcmp(p, "RLCN", 4)) return HGPS2_RESOURCE_NCLR;
    if (!memcmp(p, "RCSN", 4)) return HGPS2_RESOURCE_NSCR;
    if (!memcmp(p, "RECN", 4)) return HGPS2_RESOURCE_NCER;
    if (!memcmp(p, "RNAN", 4)) return HGPS2_RESOURCE_NANR;
    if (!memcmp(p, "BMD0", 4)) return HGPS2_RESOURCE_NSBMD;
    if (!memcmp(p, "BTX0", 4)) return HGPS2_RESOURCE_NSBTX;
    return HGPS2_RESOURCE_UNKNOWN;
}
const char *hgps2_resource_kind_name(HgPs2ResourceKind kind)
{
    switch (kind) {
    case HGPS2_RESOURCE_NARC: return "NARC";
    case HGPS2_RESOURCE_NCGR: return "NCGR (tile graphics)";
    case HGPS2_RESOURCE_NCLR: return "NCLR (palette)";
    case HGPS2_RESOURCE_NSCR: return "NSCR (tile map)";
    case HGPS2_RESOURCE_NCER: return "NCER (sprite cells)";
    case HGPS2_RESOURCE_NANR: return "NANR (animation)";
    case HGPS2_RESOURCE_NSBMD: return "NSBMD (3D model)";
    case HGPS2_RESOURCE_NSBTX: return "NSBTX (3D textures)";
    default: return "unknown / possibly compressed";
    }
}
int hgps2_resource_probe_member(const HgPs2Narc *archive, hg_u32 member_id,
                                HgPs2ResourceKind *kind, hg_u32 *length)
{
    unsigned char signature[4];
    hg_u32 size;
    if (!kind || !length || !hgps2_narc_member_size(archive, member_id, &size))
        return 0;
    *length = size;
    *kind = HGPS2_RESOURCE_UNKNOWN;
    if (size < sizeof(signature)) return 1;
    if (!hgps2_narc_read(archive, member_id, 0, signature, sizeof(signature)))
        return 0;
    *kind = hgps2_resource_identify(signature, sizeof(signature));
    return 1;
}
