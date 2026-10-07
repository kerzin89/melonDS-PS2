#include "hgps2/compat.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#ifdef __PS2__
#include <kernel.h>
#endif

static hg_u64 s_boot_ticks;

void hgps2_log(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

hg_u64 hgps2_ticks(void)
{
#ifdef __PS2__
    return (hg_u64)clock();
#else
    return (hg_u64)clock();
#endif
}

void *hgps2_alloc(size_t size, size_t alignment)
{
    if (alignment < sizeof(void *)) alignment = sizeof(void *);
#ifdef __PS2__
    void *p = memalign(alignment, size);
    return p;
#else
    size_t padded = (size + alignment - 1) & ~(alignment - 1);
    return aligned_alloc(alignment, padded);
#endif
}

void hgps2_free(void *ptr)
{
    free(ptr);
}

int hgps2_init(void)
{
    s_boot_ticks = hgps2_ticks();
    hgps2_log("[HGPS2] compatibility runtime init\n");
    hgps2_log("[HGPS2] phase=P0 native-r5900-runtime\n");
    return 1;
}

void hgps2_shutdown(void)
{
    hgps2_log("[HGPS2] shutdown ticks=%llu\n",
              (unsigned long long)(hgps2_ticks() - s_boot_ticks));
}

int hgps2_file_read_all(const char *path, void **data, size_t *size)
{
    FILE *f;
    long n;
    void *p;
    if (!path || !data || !size) return 0;
    f = fopen(path, "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (n < 0) { fclose(f); return 0; }
    p = hgps2_alloc((size_t)n, 64);
    if (!p) { fclose(f); return 0; }
    if (fread(p, 1, (size_t)n, f) != (size_t)n) {
        hgps2_free(p); fclose(f); return 0;
    }
    fclose(f);
    *data = p;
    *size = (size_t)n;
    return 1;
}

int hgps2_file_write_all(const char *path, const void *data, size_t size)
{
    FILE *f = fopen(path, "wb");
    if (!f) return 0;
    if (fwrite(data, 1, size, f) != size) { fclose(f); return 0; }
    fclose(f);
    return 1;
}

/* P0 stubs. Each intentionally returns failure until its PS2 backend lands. */
int hgps2_pad_poll(HgPs2PadState *state)
{
    if (state) memset(state, 0, sizeof(*state));
    return 0;
}
int hgps2_video_init(int width, int height) { (void)width; (void)height; return 0; }
void hgps2_video_begin(void) {}
void hgps2_video_present(void) {}
int hgps2_audio_init(unsigned sample_rate) { (void)sample_rate; return 0; }
void hgps2_audio_submit(const int16_t *stereo, size_t frames)
{ (void)stereo; (void)frames; }
