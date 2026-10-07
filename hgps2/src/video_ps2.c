#include "hgps2/compat.h"

#ifdef __PS2__
#include <gsKit.h>
#include <dmaKit.h>

static GSGLOBAL *s_gs;
static hg_u32 s_frame;
static float s_marker_x = 24.0f;
static float s_marker_y = 96.0f;

int hgps2_video_init(int width, int height)
{
    s_gs = gsKit_init_global();
    if (!s_gs) return 0;

    s_gs->Width = width;
    s_gs->Height = height;
    s_gs->PSM = GS_PSM_CT16;
    s_gs->PSMZ = GS_PSMZ_16;
    s_gs->ZBuffering = GS_SETTING_OFF;
    s_gs->DoubleBuffering = GS_SETTING_ON;

    dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
                D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
    dmaKit_chan_init(DMA_CHANNEL_GIF);
    gsKit_vram_clear(s_gs);
    gsKit_init_screen(s_gs);
    gsKit_mode_switch(s_gs, GS_ONESHOT);
    s_frame = 0;
    return 1;
}

void hgps2_video_debug_input(hg_u16 held)
{
    const float speed = 3.0f;

    if (held & HGPS2_BTN_LEFT)  s_marker_x -= speed;
    if (held & HGPS2_BTN_RIGHT) s_marker_x += speed;
    if (held & HGPS2_BTN_UP)    s_marker_y -= speed;
    if (held & HGPS2_BTN_DOWN)  s_marker_y += speed;

    if (s_marker_x < 0.0f)   s_marker_x = 0.0f;
    if (s_marker_x > 592.0f) s_marker_x = 592.0f;
    if (s_marker_y < 0.0f)   s_marker_y = 0.0f;
    if (s_marker_y > 400.0f) s_marker_y = 400.0f;
}

void hgps2_video_begin(void)
{
    u64 bg;
    if (!s_gs) return;

    gsKit_queue_reset(s_gs->Os_Queue);
    bg = GS_SETREG_RGBAQ((s_frame >> 1) & 0x3f, 0x20,
                         0x50 + ((s_frame >> 2) & 0x2f), 0x80, 0);
    gsKit_clear(s_gs, bg);
}

void hgps2_video_present(void)
{
    u64 marker;
    if (!s_gs) return;

    marker = GS_SETREG_RGBAQ(0xff, 0xff, 0xff, 0x80, 0);
    gsKit_prim_sprite(s_gs,
                      s_marker_x, s_marker_y,
                      s_marker_x + 48.0f, s_marker_y + 48.0f,
                      1, marker);
    gsKit_queue_exec(s_gs);
    gsKit_sync_flip(s_gs);
    ++s_frame;
}
#else
int hgps2_video_init(int width, int height) { (void)width; (void)height; return 1; }
void hgps2_video_begin(void) {}
void hgps2_video_present(void) {}
void hgps2_video_debug_input(hg_u16 held) { (void)held; }
#endif
