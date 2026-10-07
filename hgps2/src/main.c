#include "hgps2/compat.h"

int main(int argc, char **argv)
{
    hg_u32 frame;
    (void)argc;
    (void)argv;

    if (!hgps2_init()) return 1;
    hgps2_log("[HGPS2] P0.1 PS2 visual boot test\n");

    if (!hgps2_video_init(640, 448)) {
        hgps2_log("[HGPS2] GS init failed\n");
        return 2;
    }

    /*
     * First compatibility milestone: execute real PS2 frames instead of
     * returning immediately. 300 frames is long enough to diagnose boot,
     * GS setup and vsync before HeartGold's NitroMain dependencies land.
     */
    for (frame = 0; frame < 300; ++frame) {
        hgps2_video_begin();
        hgps2_video_present();
        if ((frame % 60) == 0)
            hgps2_log("[HGPS2] frame=%u\n", frame);
    }

    hgps2_log("[HGPS2] P0.1 completed 300 frames\n");
    for (;;) {
        hgps2_video_begin();
        hgps2_video_present();
    }
    return 0;
}
