#include "hgps2/overlay.h"

#include <string.h>

enum {
    HGPS2_OVERLAY_INIT = 0,
    HGPS2_OVERLAY_EXEC = 1,
    HGPS2_OVERLAY_EXIT = 2,
    HGPS2_OVERLAY_DONE = 3
};

void hgps2_overlay_manager_init(HgPs2OverlayManager *manager,
                                const HgPs2OverlayTemplate *app,
                                void *args)
{
    if (!manager || !app)
        return;

    memset(manager, 0, sizeof(*manager));
    manager->app = *app;
    manager->args = args;
    manager->phase = HGPS2_OVERLAY_INIT;
    manager->proc_state = 0;

    hgps2_log("[HGPS2][OVY] start id=%d name=%s\n",
              manager->app.overlay_id,
              manager->app.name ? manager->app.name : "(unnamed)");
}

int hgps2_overlay_manager_run(HgPs2OverlayManager *manager)
{
    if (!manager)
        return 1;

    switch (manager->phase) {
    case HGPS2_OVERLAY_INIT:
        if (!manager->app.init || manager->app.init(manager, &manager->proc_state)) {
            manager->phase = HGPS2_OVERLAY_EXEC;
            manager->proc_state = 0;
        }
        break;

    case HGPS2_OVERLAY_EXEC:
        if (!manager->app.exec || manager->app.exec(manager, &manager->proc_state)) {
            manager->phase = HGPS2_OVERLAY_EXIT;
            manager->proc_state = 0;
        }
        break;

    case HGPS2_OVERLAY_EXIT:
        if (!manager->app.exit || manager->app.exit(manager, &manager->proc_state)) {
            manager->phase = HGPS2_OVERLAY_DONE;
            manager->proc_state = 0;
            hgps2_log("[HGPS2][OVY] finished id=%d name=%s\n",
                      manager->app.overlay_id,
                      manager->app.name ? manager->app.name : "(unnamed)");
        }
        break;

    default:
        return 1;
    }

    return manager->phase == HGPS2_OVERLAY_DONE;
}

int hgps2_overlay_manager_finished(const HgPs2OverlayManager *manager)
{
    return !manager || manager->phase == HGPS2_OVERLAY_DONE;
}

void *hgps2_overlay_data_alloc(HgPs2OverlayManager *manager,
                               size_t size,
                               size_t alignment)
{
    if (!manager || manager->data)
        return 0;

    manager->data = hgps2_alloc(size, alignment);
    return manager->data;
}

void hgps2_overlay_data_free(HgPs2OverlayManager *manager)
{
    if (!manager || !manager->data)
        return;

    hgps2_free(manager->data);
    manager->data = 0;
}
