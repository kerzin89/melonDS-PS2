EE_BIN = HGBOOT.ELF
EE_OBJS = main.o
EE_INCS = -I$(GSKIT)/include
EE_CFLAGS += -O2 -G0 -Wall -Wextra
EE_LIBS += -L$(GSKIT)/lib -lgskit -ldmakit -lkernel

all: $(EE_BIN)

clean:
	rm -f $(EE_BIN) $(EE_OBJS)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
