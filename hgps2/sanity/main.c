#include <gsKit.h>
#include <dmaKit.h>

int main(int argc, char **argv)
{
    GSGLOBAL *gs;
    u64 red;
    u64 white;

    (void)argc;
    (void)argv;

    gs = gsKit_init_global();
    if (!gs)
        for (;;) {}

    gs->PSM = GS_PSM_CT16;
    gs->PSMZ = GS_PSMZ_16;
    gs->ZBuffering = GS_SETTING_OFF;
    gs->DoubleBuffering = GS_SETTING_OFF;

    dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
                D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
    dmaKit_chan_init(DMA_CHANNEL_GIF);

    gsKit_init_screen(gs);
    gsKit_mode_switch(gs, GS_PERSISTENT);

    red = GS_SETREG_RGBAQ(0xFF, 0x00, 0x00, 0x80, 0x00);
    white = GS_SETREG_RGBAQ(0xFF, 0xFF, 0xFF, 0x80, 0x00);

    gsKit_clear(gs, red);
    gsKit_prim_sprite(gs, 64.0f, 64.0f, 192.0f, 192.0f, 1, white);

    for (;;) {
        gsKit_queue_exec(gs);
        gsKit_sync_flip(gs);
    }

    return 0;
}
