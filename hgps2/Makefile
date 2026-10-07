EE_BIN = HGPS2.ELF
EE_OBJS = src/main.o src/compat.o src/video_ps2.o
EE_INCS = -Iinclude -I$(GSKIT)/include
EE_CFLAGS += -O2 -G0 -Wall -Wextra -D__PS2__
EE_LIBS += -L$(GSKIT)/lib -ldmakit -lgskit -lkernel

all: $(EE_BIN)

clean:
	rm -f $(EE_BIN) $(EE_OBJS)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
