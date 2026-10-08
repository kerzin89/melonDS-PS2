#include "hgps2/nitrofs.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void put32(unsigned char *p, unsigned v)
{
    p[0] = (unsigned char)v;
    p[1] = (unsigned char)(v >> 8);
    p[2] = (unsigned char)(v >> 16);
    p[3] = (unsigned char)(v >> 24);
}

int main(void)
{
    const char *path = "hgps2_nitrofs_fixture.tmp";
    unsigned char rom[256] = {0};
    unsigned char data[8] = {0};
    HgPs2NitroFs fs;
    hg_u32 size = 0;
    FILE *f;
    put32(rom + 0x48, 0x80); /* FAT location */
    put32(rom + 0x4c, 16);   /* two FAT entries */
    put32(rom + 0x80, 0xa0);
    put32(rom + 0x84, 0xa6);
    put32(rom + 0x88, 0xb0);
    put32(rom + 0x8c, 0xb3);
    memcpy(rom + 0xa0, "HGDATA", 6);
    memcpy(rom + 0xb0, "PS2", 3);
    f = fopen(path, "wb");
    assert(f);
    assert(fwrite(rom, 1, sizeof(rom), f) == sizeof(rom));
    assert(fclose(f) == 0);

    assert(hgps2_nitrofs_open(&fs, path));
    assert(fs.file_count == 2);
    assert(hgps2_nitrofs_file_size(&fs, 0, &size) && size == 6);
    assert(hgps2_nitrofs_read(&fs, 0, 2, data, 4));
    assert(memcmp(data, "DATA", 4) == 0);
    assert(hgps2_nitrofs_read(&fs, 1, 0, data, 3));
    assert(memcmp(data, "PS2", 3) == 0);
    assert(!hgps2_nitrofs_read(&fs, 0, 4, data, 3));
    assert(!hgps2_nitrofs_read(&fs, 2, 0, data, 1));
    assert(remove(path) == 0);
    puts("HGPS2 NitroFS FAT tests: PASS");
    return 0;
}
