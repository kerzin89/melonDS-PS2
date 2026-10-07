#include "hgps2/system_compat.h"

#include <string.h>

void hgps2_system_init(HgPs2SystemState *system)
{
    if (!system)
        return;

    memset(system, 0, sizeof(*system));
    system->keyRepeatContinueDelay = 4;
    system->keyRepeatStartDelay = 8;
    system->bootTimeUs = hgps2_time_us();
    system->lastFpsSampleUs = system->bootTimeUs;
}

int hgps2_system_read_input(HgPs2SystemState *system)
{
    HgPs2PadState pad;
    hg_u16 raw;

    if (!system)
        return 0;

    if (!hgps2_pad_poll(&pad))
        return 0;

    raw = pad.held | system->simulatedInputs;
    system->simulatedInputs = 0;

    system->newKeysRaw = raw & (hg_u16)(raw ^ system->heldKeysRaw);
    system->newAndRepeatedKeysRaw = system->newKeysRaw;

    if (raw != 0 && system->heldKeysRaw == raw) {
        if (system->keyRepeatCounter > 0)
            --system->keyRepeatCounter;

        if (system->keyRepeatCounter == 0) {
            system->newAndRepeatedKeysRaw = raw;
            system->keyRepeatCounter = system->keyRepeatContinueDelay;
        }
    } else {
        system->keyRepeatCounter = system->keyRepeatStartDelay;
    }

    system->heldKeysRaw = raw;

    /* Stage 1 uses the normal HeartGold button mode. Alternative button modes
     * can be implemented when their callers are linked. */
    system->newKeys = system->newKeysRaw;
    system->heldKeys = system->heldKeysRaw;
    system->newAndRepeatedKeys = system->newAndRepeatedKeysRaw;
    return 1;
}

void hgps2_system_on_vblank(HgPs2SystemState *system)
{
    if (!system)
        return;

    ++system->vblankCounter;
    ++system->frameCounter;
}

void hgps2_system_measure_frame(HgPs2SystemState *system)
{
    hg_u64 now;
    hg_u64 elapsed;

    if (!system)
        return;

    ++system->fpsSampleFrames;
    now = hgps2_time_us();
    elapsed = now - system->lastFpsSampleUs;

    if (elapsed >= 1000000ULL) {
        system->measuredFps =
            ((float)system->fpsSampleFrames * 1000000.0f) / (float)elapsed;
        system->fpsSampleFrames = 0;
        system->lastFpsSampleUs = now;
    }
}

void hgps2_system_simulate_keys(HgPs2SystemState *system, hg_u16 keys)
{
    if (system)
        system->simulatedInputs |= keys;
}
