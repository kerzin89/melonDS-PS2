#include "hgps2/lz77.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void)
{
    /* LZ10: literal A, B, C then copy 6 bytes from distance 3. */
    const unsigned char good[] = {0x10,9,0,0,0x10,'A','B','C',0x30,0x02};
    const unsigned char bad_distance[] = {0x10,3,0,0,0x80,0x00,0x00};
    const unsigned char bad_overrun[] = {0x10,4,0,0,0x40,'A',0x10,0x00};
    unsigned char out[16] = {0};
    size_t size = 0, written = 0;
    assert(hgps2_lz10_output_size(good, sizeof(good), 16, &size) && size == 9);
    assert(!hgps2_lz10_output_size(good, sizeof(good), 8, &size));
    assert(hgps2_lz10_decode(good, sizeof(good), out, sizeof(out), &written));
    assert(written == 9 && memcmp(out, "ABCABCABC", 9) == 0);
    assert(!hgps2_lz10_decode(good, sizeof(good) - 1, out, sizeof(out), &written));
    assert(!hgps2_lz10_decode(good, sizeof(good), out, 8, &written));
    assert(!hgps2_lz10_decode(bad_distance, sizeof(bad_distance), out, sizeof(out), &written));
    assert(!hgps2_lz10_decode(bad_overrun, sizeof(bad_overrun), out, sizeof(out), &written));
    assert(!hgps2_lz10_decode(good, sizeof(good), NULL, sizeof(out), &written));
    puts("HGPS2 LZ10 tests: PASS");
    return 0;
}
