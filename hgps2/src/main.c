#include "hgps2/compat.h"
#include "hgps2/overlay.h"
#include "hgps2/nitrofs.h"
#include "hgps2/system_compat.h"

static int diagnostic_init(HgPs2OverlayManager *manager, int *state)
{
    (void)manager;
    (void)state;
    hgps2_log("[HGPS2][OVY] diagnostic init\n");
    return 1;
}

static int diagnostic_exec(HgPs2OverlayManager *manager, int *state)
{
    (void)manager;
    ++(*state);
    return *state >= 300;
}

static int diagnostic_exit(HgPs2OverlayManager *manager, int *state)
{
    (void)manager;
    (void)state;
    hgps2_log("[HGPS2][OVY] diagnostic completed 300 application frames\n");
    return 1;
}

int main(int argc, char **argv)
{
    HgPs2SystemState system;
    HgPs2OverlayManager diagnostic;
    HgPs2OverlayTemplate app = {
        diagnostic_init,
        diagnostic_exec,
        diagnostic_exit,
        -1,
        "stage1-diagnostic"
    };
    hg_u32 last_report = 0;
    int pad_ok;

    /* Optional ROM path: inspect NitroFS metadata only; never load the ROM
     * into EE RAM or execute Nintendo DS code in this native diagnostic. */
    if (argc > 1 && argv[1]) {
        HgPs2NitroFs fs;
        if (hgps2_nitrofs_open(&fs, argv[1]))
            hgps2_log("[HGPS2][NITROFS] files=%u ROM bytes=%u\n",
                      (unsigned)fs.file_count, (unsigned)fs.rom_size);
        else
            hgps2_log("[HGPS2][NITROFS] ROM metadata unavailable\n");
    }

    if (!hgps2_init())
        return 1;

    hgps2_log("[HGPS2] Stage 1 / functionality runtime\n");

    if (!hgps2_video_init(640, 448)) {
        hgps2_log("[HGPS2] GS init failed\n");
        return 2;
    }

    pad_ok = hgps2_pad_init();
    hgps2_log("[HGPS2] pad=%s\n", pad_ok ? "ready" : "unavailable");

    hgps2_system_init(&system);
    hgps2_overlay_manager_init(&diagnostic, &app, &system);

    for (;;) {
        hgps2_system_read_input(&system);

        if (!hgps2_overlay_manager_finished(&diagnostic))
            hgps2_overlay_manager_run(&diagnostic);

        hgps2_video_debug_input(system.heldKeys);
        hgps2_video_begin();
        hgps2_video_present();

        hgps2_system_on_vblank(&system);
        hgps2_system_measure_frame(&system);

        if (system.newKeys) {
            hgps2_log("[HGPS2][INPUT] new=%04x held=%04x\n",
                      (unsigned)system.newKeys,
                      (unsigned)system.heldKeys);
        }

        if ((system.frameCounter - last_report) >= 120) {
            last_report = system.frameCounter;
            hgps2_log("[HGPS2][PERF] frame=%u vblank=%u fps=%.2f\n",
                      (unsigned)system.frameCounter,
                      (unsigned)system.vblankCounter,
                      system.measuredFps);
        }

        if ((system.heldKeys & (HGPS2_BTN_START | HGPS2_BTN_SELECT)) ==
            (HGPS2_BTN_START | HGPS2_BTN_SELECT)) {
            hgps2_log("[HGPS2] START+SELECT soft-reset gesture detected\n");
        }
    }

    hgps2_pad_shutdown();
    hgps2_shutdown();
    return 0;
}
