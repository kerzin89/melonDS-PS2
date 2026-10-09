#include "hgps2/resource.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void)
{
    assert(hgps2_resource_identify("RGCN", 4) == HGPS2_RESOURCE_NCGR);
    assert(hgps2_resource_identify("RLCN", 4) == HGPS2_RESOURCE_NCLR);
    assert(hgps2_resource_identify("RCSN", 4) == HGPS2_RESOURCE_NSCR);
    assert(hgps2_resource_identify("RECN", 4) == HGPS2_RESOURCE_NCER);
    assert(hgps2_resource_identify("RNAN", 4) == HGPS2_RESOURCE_NANR);
    assert(hgps2_resource_identify("BMD0", 4) == HGPS2_RESOURCE_NSBMD);
    assert(hgps2_resource_identify("BTX0", 4) == HGPS2_RESOURCE_NSBTX);
    assert(hgps2_resource_identify("NARC", 4) == HGPS2_RESOURCE_NARC);
    assert(hgps2_resource_identify("RGCN", 3) == HGPS2_RESOURCE_UNKNOWN);
    assert(hgps2_resource_identify(NULL, 4) == HGPS2_RESOURCE_UNKNOWN);
    assert(hgps2_resource_identify("ABCD", 4) == HGPS2_RESOURCE_UNKNOWN);
    assert(strstr(hgps2_resource_kind_name(HGPS2_RESOURCE_NSCR), "tile map"));
    puts("HGPS2 resource signatures: PASS");
    return 0;
}
