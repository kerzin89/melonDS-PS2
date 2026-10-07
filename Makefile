EE_BIN = melonDS.elf
GIT_VERSION := $(shell git describe --abbrev=6 --dirty --always --tags)

BIN2C = $(PS2SDK)/bin/bin2c

CPPSOURCES  := src src/ps2
INCLUDES := src

EE_LIBS = -L$(PS2SDK)/ports/lib -L$(PS2DEV)/gsKit/lib/ -lpatches -lfileXio -lpad -ldebug -lgskit_toolkit -lgskit -ldmakit -lpng -lz -lmc -laudsrv

EE_INCS += -I$(PS2DEV)/gsKit/include -I$(PS2SDK)/ports/include -I$(PS2SDK)/ports/include/zlib

IOP_MODULES = src/iomanx.o src/filexio.o src/sio2man.o src/mcman.o src/mcserv.o src/padman.o src/libsd.o \
			  src/usbd.o src/audsrv.o src/bdm.o src/bdmfs_vfat.o \
			  src/usbmass_bd.o

CPPFILES   := $(foreach dir,$(CPPSOURCES), $(wildcard $(dir)/*.cpp))
BINFILES := $(foreach dir,$(DATA), $(wildcard $(dir)/*.bin))
EE_OBJS     := $(IOP_MODULES) $(addsuffix .o,$(BINFILES)) $(CPPFILES:.cpp=.o)

export INCLUDE	:= $(foreach dir,$(INCLUDES),-I$(dir))

# HG-BOOT2: conservative R5900 optimization. The old core relies heavily on
# type-punning, so keep strict aliasing disabled while optimizing hot CPU/MMIO paths.
EE_CXXFLAGS += -O2 -fomit-frame-pointer -fno-strict-aliasing -fno-rtti -mtune=r5900 -msingle-float -fno-exceptions -std=gnu++11 -D__PS2__

all: $(EE_BIN)

#-------------------- Embedded IOP Modules ------------------------#
src/iomanx.c: $(PS2SDK)/iop/irx/iomanX.irx
	echo "Embedding iomanX Driver..."
	$(BIN2C) $< $@ iomanX_irx

src/filexio.c: $(PS2SDK)/iop/irx/fileXio.irx
	echo "Embedding fileXio Driver..."
	$(BIN2C) $< $@ fileXio_irx

src/sio2man.c: $(PS2SDK)/iop/irx/sio2man.irx
	echo "Embedding SIO2MAN Driver..."
	$(BIN2C) $< $@ sio2man_irx
	
src/mcman.c: $(PS2SDK)/iop/irx/mcman.irx
	echo "Embedding MCMAN Driver..."
	$(BIN2C) $< $@ mcman_irx

src/mcserv.c: $(PS2SDK)/iop/irx/mcserv.irx
	echo "Embedding MCSERV Driver..."
	$(BIN2C) $< $@ mcserv_irx

src/padman.c: $(PS2SDK)/iop/irx/padman.irx
	echo "Embedding PADMAN Driver..."
	$(BIN2C) $< $@ padman_irx
	
src/libsd.c: $(PS2SDK)/iop/irx/libsd.irx
	echo "Embedding LIBSD Driver..."
	$(BIN2C) $< $@ libsd_irx

src/usbd.c: $(PS2SDK)/iop/irx/usbd.irx
	echo "Embedding USB Driver..."
	$(BIN2C) $< $@ usbd_irx

src/audsrv.c: $(PS2SDK)/iop/irx/audsrv.irx
	echo "Embedding AUDSRV Driver..."
	$(BIN2C) $< $@ audsrv_irx

src/bdm.c: $(PS2SDK)/iop/irx/bdm.irx
	echo "Embedding Block Device Manager(BDM)..."
	$(BIN2C) $< $@ bdm_irx

src/bdmfs_vfat.c: $(PS2SDK)/iop/irx/bdmfs_vfat.irx
	echo "Embedding BDM VFAT Driver..."
	$(BIN2C) $< $@ bdmfs_vfat_irx

src/usbmass_bd.c: $(PS2SDK)/iop/irx/usbmass_bd.irx
	echo "Embedding BD USB Mass Driver..."
	$(BIN2C) $< $@ usbmass_bd_irx

#------------------------------------------------------------------#

clean:
	@rm -rf $(EE_BIN) $(EE_OBJS)
	rm -f src/sio2man.c
	rm -f src/mcman.c
	rm -f src/mcserv.c
	rm -f src/padman.c
	rm -f src/libsd.c
	rm -f src/bdm.c
	rm -f src/usbd.c
	rm -f src/audsrv.c
	rm -f src/bdmfs_vfat.c
	rm -f src/usbmass_bd.c

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal_cpp