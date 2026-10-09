#ifndef HGPS2_RESOURCE_H
#define HGPS2_RESOURCE_H
#include "hgps2/narc.h"

typedef enum HgPs2ResourceKind {
    HGPS2_RESOURCE_UNKNOWN = 0,
    HGPS2_RESOURCE_NARC,
    HGPS2_RESOURCE_NCGR,
    HGPS2_RESOURCE_NCLR,
    HGPS2_RESOURCE_NSCR,
    HGPS2_RESOURCE_NCER,
    HGPS2_RESOURCE_NANR,
    HGPS2_RESOURCE_NSBMD,
    HGPS2_RESOURCE_NSBTX
} HgPs2ResourceKind;

HgPs2ResourceKind hgps2_resource_identify(const void *bytes, size_t length);
const char *hgps2_resource_kind_name(HgPs2ResourceKind kind);
int hgps2_resource_probe_member(const HgPs2Narc *archive, hg_u32 member_id,
                                HgPs2ResourceKind *kind, hg_u32 *length);
#endif
