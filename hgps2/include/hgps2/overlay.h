#ifndef HGPS2_OVERLAY_H
#define HGPS2_OVERLAY_H

#include "hgps2/compat.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HgPs2OverlayManager HgPs2OverlayManager;
typedef int (*HgPs2OverlayFn)(HgPs2OverlayManager *manager, int *state);

typedef struct HgPs2OverlayTemplate {
    HgPs2OverlayFn init;
    HgPs2OverlayFn exec;
    HgPs2OverlayFn exit;
    int overlay_id;
    const char *name;
} HgPs2OverlayTemplate;

struct HgPs2OverlayManager {
    HgPs2OverlayTemplate app;
    int phase;
    int proc_state;
    void *args;
    void *data;
};

void hgps2_overlay_manager_init(HgPs2OverlayManager *manager,
                                const HgPs2OverlayTemplate *app,
                                void *args);
int  hgps2_overlay_manager_run(HgPs2OverlayManager *manager);
int  hgps2_overlay_manager_finished(const HgPs2OverlayManager *manager);
void *hgps2_overlay_data_alloc(HgPs2OverlayManager *manager,
                               size_t size,
                               size_t alignment);
void hgps2_overlay_data_free(HgPs2OverlayManager *manager);

#ifdef __cplusplus
}
#endif

#endif
