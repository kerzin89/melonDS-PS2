#include "hgps2/narc.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void le16(unsigned char *p, unsigned v)
{ p[0] = (unsigned char)v; p[1] = (unsigned char)(v >> 8); }
static void le32(unsigned char *p, unsigned v)
{ p[0] = (unsigned char)v; p[1] = (unsigned char)(v >> 8);
  p[2] = (unsigned char)(v >> 16); p[3] = (unsigned char)(v >> 24); }

int main(void)
{
    unsigned char rom[512] = {0}, data[8] = {0};
    const char *path = "hgps2_narc_fixture.tmp";
    HgPs2NitroFs fs;
    HgPs2Narc n;
    hg_u32 sz;
    FILE *f;
    /* ROM FAT: NARC lives at 0x100 and is 0x44 bytes long. */
    le32(rom + 0x48, 0x80); le32(rom + 0x4c, 8);
    le32(rom + 0x80, 0x100); le32(rom + 0x84, 0x144);
    memcpy(rom + 0x100, "NARC", 4);
    le16(rom + 0x104, 0xfffe); le16(rom + 0x106, 0x100);
    le32(rom + 0x108, 0x44); le16(rom + 0x10c, 16);
    le16(rom + 0x10e, 3);
    memcpy(rom + 0x110, "BTAF", 4); le32(rom + 0x114, 0x1c);
    le16(rom + 0x118, 2);
    le32(rom + 0x11c, 0); le32(rom + 0x120, 4);
    le32(rom + 0x124, 4); le32(rom + 0x128, 8);
    memcpy(rom + 0x12c, "BTNF", 4); le32(rom + 0x130, 8);
    memcpy(rom + 0x134, "GMIF", 4); le32(rom + 0x138, 16);
    memcpy(rom + 0x13c, "ABCDWXYZ", 8);
    f = fopen(path, "wb"); assert(f);
    assert(fwrite(rom, 1, sizeof(rom), f) == sizeof(rom));
    assert(fclose(f) == 0);
    assert(hgps2_nitrofs_open(&fs, path));
    assert(hgps2_narc_open(&n, &fs, 0));
    assert(n.member_count == 2);
    assert(hgps2_narc_member_size(&n, 1, &sz) && sz == 4);
    assert(hgps2_narc_read(&n, 1, 1, data, 3));
    assert(memcmp(data, "XYZ", 3) == 0);
    assert(!hgps2_narc_read(&n, 1, 3, data, 2));
    assert(!hgps2_narc_read(&n, 2, 0, data, 1));
    assert(remove(path) == 0);
    puts("HGPS2 NARC member tests: PASS");
    return 0;
}
