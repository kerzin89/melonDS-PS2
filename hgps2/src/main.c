#include "hgps2/compat.h"

int main(int argc, char **argv)
{
    HgPs2PadState pad;
    (void)argc;
    (void)argv;

    if (!hgps2_init()) return 1;

    hgps2_log("[HGPS2] native PS2 port bootstrap\n");
    hgps2_log("[HGPS2] Nintendo DS emulation is not active in this target\n");

    hgps2_pad_poll(&pad);

    hgps2_shutdown();
    return 0;
}
