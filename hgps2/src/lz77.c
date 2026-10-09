#include "hgps2/lz77.h"

static int header_size(const hg_u8 *s, size_t n, size_t limit, size_t *out)
{
    size_t size;
    if (!s || n < 4 || s[0] != 0x10 || !out) return 0;
    size = (size_t)s[1] | ((size_t)s[2] << 8) | ((size_t)s[3] << 16);
    if (size == 0 || size > limit) return 0;
    *out = size;
    return 1;
}
int hgps2_lz10_output_size(const void *src, size_t src_len,
                          size_t max_output, size_t *output_size)
{
    return header_size((const hg_u8 *)src, src_len, max_output, output_size);
}
int hgps2_lz10_decode(const void *src, size_t src_len,
                      void *dst, size_t dst_capacity, size_t *written)
{
    const hg_u8 *s = (const hg_u8 *)src;
    hg_u8 *d = (hg_u8 *)dst;
    size_t target, si = 4, di = 0;
    if (written) *written = 0;
    if (!dst || !header_size(s, src_len, dst_capacity, &target)) return 0;
    /* Decoding in place is not supported. Callers must use separate buffers. */
    if ((const void *)src == dst) return 0;
    while (di < target) {
        unsigned flags, bit;
        if (si >= src_len) return 0;
        flags = s[si++];
        for (bit = 0; bit < 8 && di < target; ++bit) {
            if ((flags & (0x80u >> bit)) == 0) {
                if (si >= src_len) return 0;
                d[di++] = s[si++];
            } else {
                unsigned token, run, distance, k;
                if (src_len - si < 2) return 0;
                token = ((unsigned)s[si] << 8) | s[si + 1];
                si += 2;
                run = (token >> 12) + 3u;
                distance = (token & 0x0fffu) + 1u;
                if (distance > di || run > target - di) return 0;
                for (k = 0; k < run; ++k) {
                    d[di] = d[di - distance];
                    ++di;
                }
            }
        }
    }
    if (written) *written = di;
    return 1;
}
