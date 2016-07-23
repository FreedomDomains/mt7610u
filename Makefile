ifeq ($(WIFI_MODE),)
RT28xx_MODE = STA
# RT28xx_MODE = AP
else
RT28xx_MODE = $(WIFI_MODE)
endif

TARGET = LINUX

# CHIPSET
# rt2860, rt2870, rt2880, rt2070, rt3070, rt3090, rt3572, rt3062, rt3562, rt3593, rt3573
# rt3562(for rt3592), rt3050, rt3350, rt3352, rt5350, rt5370, rt5390, rt5572, rt5592,
# rt8592(for rt85592), mt7650e, mt7630e, mt7610e, mt7650u, mt7630u, mt7610u

ifeq ($(CHIPSET),)
CHIPSET = mt7610u
endif

MODULE = $(word 1, $(CHIPSET))

PWD = $(shell pwd)

include $(PWD)/os/linux/config.mk

RTMP_SRC_DIR = $(PWD)/RT$(MODULE)

#PLATFORM: Target platform
PLATFORM = PC

#APSOC

#RELEASE Package
RELEASE = DPO


MAKE = make

ifeq ($(PLATFORM),PC)
# Linux 2.6
KSRC = /lib/modules/$(shell uname -r)/build
# Linux 2.4 Change to your local setting
#KSRC = /usr/src/linux-2.4
CROSS_COMPILE =
endif

export PWD RT28xx_MODE KSRC CROSS_COMPILE CROSS_COMPILE_INCLUDE PLATFORM RELEASE CHIPSET MODULE RTMP_SRC_DIR TARGET HAS_WOW_SUPPORT

# The targets that may be used.
PHONY += all build_tools test LINUX clean

all: build_tools $(TARGET)

build_tools:
	$(MAKE) -C tools
	$(PWD)/tools/bin2h

test:
	$(MAKE) -C tools test

LINUX:
	$(MAKE) -C $(KSRC) SUBDIRS=$(PWD)/os/linux modules

clean:
	rm -f common/*.o
	rm -f common/.*.{cmd,flags,d}
	rm -f os/linux/*.{o,ko,mod.{o,c}}
	rm -f os/linux/.*.{cmd,flags,d}
	rm -fr os/linux/.tmp_versions
#Must clean Module.symvers; or you will suffer symbol version not match
#when OS_ABL = YES.
	rm -f os/linux/Module.symvers
	rm -f os/linux/Modules.symvers
	rm -f os/linux/Module.markers
	rm -f os/linux/modules.order
	rm -f chips/*.o
	rm -f chips/.*.{cmd,flags,d}
ifeq ($(RT28xx_MODE),AP)
	rm -f ap/*.o
	rm -f ap/.*.{cmd,flags,d}
else
ifeq ($(RT28xx_MODE),STA)
	rm -f sta/*.o
	rm -f sta/.*.{cmd,flags,d}
else
ifeq ($(RT28xx_MODE),APSTA)
	rm -f ap/*.o
	rm -f ap/.*.{cmd,flags,d}
	rm -f sta/*.o
	rm -f sta/.*.{cmd,flags,d}
endif
endif
endif

# Declare the contents of the .PHONY variable as phony.  We keep that information in a variable
.PHONY: $(PHONY)



