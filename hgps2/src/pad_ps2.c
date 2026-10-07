#include "hgps2/compat.h"

#ifdef __PS2__
#include <kernel.h>
#include <libpad.h>
#include <loadfile.h>
#include <sifrpc.h>

static char s_pad_buf[256] __attribute__((aligned(64)));
static hg_u16 s_previous;
static int s_open;

static hg_u16 map_buttons(unsigned int ps2)
{
    hg_u16 out = 0;

    if (ps2 & PAD_CROSS)    out |= HGPS2_BTN_A;
    if (ps2 & PAD_CIRCLE)   out |= HGPS2_BTN_B;
    if (ps2 & PAD_SELECT)   out |= HGPS2_BTN_SELECT;
    if (ps2 & PAD_START)    out |= HGPS2_BTN_START;
    if (ps2 & PAD_RIGHT)    out |= HGPS2_BTN_RIGHT;
    if (ps2 & PAD_LEFT)     out |= HGPS2_BTN_LEFT;
    if (ps2 & PAD_UP)       out |= HGPS2_BTN_UP;
    if (ps2 & PAD_DOWN)     out |= HGPS2_BTN_DOWN;
    if (ps2 & PAD_R1)       out |= HGPS2_BTN_R;
    if (ps2 & PAD_L1)       out |= HGPS2_BTN_L;
    if (ps2 & PAD_SQUARE)   out |= HGPS2_BTN_X;
    if (ps2 & PAD_TRIANGLE) out |= HGPS2_BTN_Y;

    return out;
}

int hgps2_pad_init(void)
{
    int ret;

    SifInitRpc(0);

    ret = SifLoadModule("rom0:SIO2MAN", 0, 0);
    if (ret < 0)
        hgps2_log("[HGPS2][PAD] SIO2MAN load returned %d; continuing\n", ret);

    ret = SifLoadModule("rom0:PADMAN", 0, 0);
    if (ret < 0)
        hgps2_log("[HGPS2][PAD] PADMAN load returned %d; continuing\n", ret);

    if (padInit(0) == 0) {
        hgps2_log("[HGPS2][PAD] padInit failed\n");
        return 0;
    }

    if (padPortOpen(0, 0, s_pad_buf) == 0) {
        hgps2_log("[HGPS2][PAD] padPortOpen failed\n");
        return 0;
    }

    s_previous = 0;
    s_open = 1;
    hgps2_log("[HGPS2][PAD] port0 slot0 opened\n");
    return 1;
}

void hgps2_pad_shutdown(void)
{
    if (s_open) {
        padPortClose(0, 0);
        s_open = 0;
    }
    padEnd();
}

int hgps2_pad_poll(HgPs2PadState *state)
{
    struct padButtonStatus buttons;
    unsigned int raw;
    hg_u16 held;
    int ps;

    if (!state)
        return 0;

    state->held = 0;
    state->pressed = 0;
    state->released = 0;

    if (!s_open)
        return 0;

    ps = padGetState(0, 0);
    if (ps != PAD_STATE_STABLE && ps != PAD_STATE_FINDCTP1)
        return 0;

    if (padRead(0, 0, &buttons) == 0)
        return 0;

    raw = 0xffffu ^ buttons.btns;
    held = map_buttons(raw);

    state->held = held;
    state->pressed = held & (hg_u16)~s_previous;
    state->released = s_previous & (hg_u16)~held;
    s_previous = held;
    return 1;
}

#else

int hgps2_pad_init(void) { return 1; }
void hgps2_pad_shutdown(void) {}

int hgps2_pad_poll(HgPs2PadState *state)
{
    if (!state) return 0;
    state->held = 0;
    state->pressed = 0;
    state->released = 0;
    return 1;
}

#endif
