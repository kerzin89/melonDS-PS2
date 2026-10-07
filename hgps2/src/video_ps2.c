#include "hgps2/compat.h"

#ifdef __PS2__
#include <gsKit.h>
#include <dmaKit.h>

static GSGLOBAL *s_gs;
static hg_u32 s_frame;

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
    gsKit_init_screen(s_gs);
    gsKit_mode_switch(s_gs, GS_ONESHOT);
    s_frame = 0;
    return 1;
}

void hgps2_video_begin(void)
{
    u64 bg;
    if (!s_gs) return;
    gsKit_queue_reset(s_gs->Os_Queue);
    /* Deliberately animate the boot color so a tester can prove frames advance. */
    bg = GS_SETREG_RGBAQ((s_frame >> 1) & 0x3f, 0x20,
                         0x50 + ((s_frame >> 2) & 0x2f), 0x80, 0);
    gsKit_clear(s_gs, bg);
}

void hgps2_video_present(void)
{
    u64 marker;
    float x;
    if (!s_gs) return;

    x = 24.0f + (float)(s_frame % 240);
    marker = GS_SETREG_RGBAQ(0xff, 0xff, 0xff, 0x80, 0);
    gsKit_prim_sprite(s_gs, x, 96.0f, x + 48.0f, 144.0f, 1, marker);
    gsKit_queue_exec(s_gs);
    gsKit_sync_flip(s_gs);
    ++s_frame;
}
#else
int hgps2_video_init(int width, int height) { (void)width; (void)height; return 1; }
void hgps2_video_begin(void) {}
void hgps2_video_present(void) {}
#endif
