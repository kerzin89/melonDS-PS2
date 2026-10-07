#ifndef HGPS2_SYSTEM_COMPAT_H
#define HGPS2_SYSTEM_COMPAT_H

#include "hgps2/compat.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HgPs2SystemState {
    hg_u16 heldKeysRaw;
    hg_u16 newKeysRaw;
    hg_u16 newAndRepeatedKeysRaw;
    hg_u16 heldKeys;
    hg_u16 newKeys;
    hg_u16 newAndRepeatedKeys;
    hg_u16 simulatedInputs;
    int keyRepeatCounter;
    int keyRepeatContinueDelay;
    int keyRepeatStartDelay;
    hg_u32 frameCounter;
    hg_u32 vblankCounter;
    hg_u64 bootTimeUs;
    hg_u64 lastFpsSampleUs;
    hg_u32 fpsSampleFrames;
    float measuredFps;
} HgPs2SystemState;

void hgps2_system_init(HgPs2SystemState *system);
int  hgps2_system_read_input(HgPs2SystemState *system);
void hgps2_system_on_vblank(HgPs2SystemState *system);
void hgps2_system_measure_frame(HgPs2SystemState *system);
void hgps2_system_simulate_keys(HgPs2SystemState *system, hg_u16 keys);

#ifdef __cplusplus
}
#endif

#endif
