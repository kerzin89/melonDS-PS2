#ifndef HGPS2_LZ77_H
#define HGPS2_LZ77_H
#include "hgps2/compat.h"

/* Nintendo DS BIOS-style LZ10 (type 0x10). The caller provides both buffers.
 * Rejects malformed streams and output larger than the caller's budget.
 * No dynamic allocation and no unbounded output. */
int hgps2_lz10_output_size(const void *src, size_t src_len,
                          size_t max_output, size_t *output_size);
int hgps2_lz10_decode(const void *src, size_t src_len,
                      void *dst, size_t dst_capacity, size_t *written);
#endif
