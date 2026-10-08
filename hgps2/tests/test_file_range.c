#include "hgps2/compat.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    const char *path = "hgps2_range_test.tmp";
    const char payload[] = "0123456789abcdef";
    char buf[8] = {0};
    FILE *f = fopen(path, "wb");
    assert(f != NULL);
    assert(fwrite(payload, 1, sizeof(payload) - 1, f) == sizeof(payload) - 1);
    assert(fclose(f) == 0);

    assert(hgps2_file_read_range(path, 4, buf, 5) == 1);
    assert(memcmp(buf, "45678", 5) == 0);
    assert(hgps2_file_read_range(path, 0, buf, 0) == 1);
    assert(hgps2_file_read_range(path, 14, buf, 5) == 0);
    assert(hgps2_file_read_range(path, 0, NULL, 1) == 0);
    assert(hgps2_file_read_range("hgps2_no_such_file", 0, buf, 1) == 0);
    assert(remove(path) == 0);
    puts("HGPS2 range-read tests: PASS");
    return 0;
}
