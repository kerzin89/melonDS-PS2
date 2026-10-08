EE_BIN = HGPS2.ELF
EE_OBJS = src/main.o src/compat.o src/video_ps2.o src/pad_ps2.o src/overlay.o src/system_compat.o src/nitrofs.o
EE_INCS = -Iinclude -I$(PS2DEV)/gsKit/include
EE_CFLAGS += -O2 -G0 -Wall -Wextra -fno-strict-aliasing -D__PS2__
EE_LIBS += -L$(PS2DEV)/gsKit/lib -lpad -lgskit -ldmakit -lkernel

all: $(EE_BIN)

clean:
	rm -f $(EE_BIN) $(EE_OBJS)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
