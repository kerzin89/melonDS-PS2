#ifndef HGPS2_COMPAT_H
#define HGPS2_COMPAT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t  hg_u8;
typedef uint16_t hg_u16;
typedef uint32_t hg_u32;
typedef int32_t  hg_s32;
typedef uint64_t hg_u64;

typedef struct HgPs2PadState {
    hg_u16 held;
    hg_u16 pressed;
    hg_u16 released;
} HgPs2PadState;

int  hgps2_init(void);
void hgps2_shutdown(void);
hg_u64 hgps2_ticks(void);
void *hgps2_alloc(size_t size, size_t alignment);
void hgps2_free(void *ptr);

int hgps2_pad_poll(HgPs2PadState *state);
int hgps2_file_read_all(const char *path, void **data, size_t *size);
int hgps2_file_write_all(const char *path, const void *data, size_t size);

int  hgps2_video_init(int width, int height);
void hgps2_video_begin(void);
void hgps2_video_present(void);

int  hgps2_audio_init(unsigned sample_rate);
void hgps2_audio_submit(const int16_t *stereo, size_t frames);

void hgps2_log(const char *fmt, ...);

#ifdef __cplusplus
}
#endif
#endif
