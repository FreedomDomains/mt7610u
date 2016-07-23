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

#OS ABL - YES or NO
OSABL = NO

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

export OSABL PWD RT28xx_MODE KSRC CROSS_COMPILE CROSS_COMPILE_INCLUDE PLATFORM RELEASE CHIPSET MODULE RTMP_SRC_DIR TARGET HAS_WOW_SUPPORT

# The targets that may be used.
PHONY += all build_tools test UCOS THREADX LINUX release prerelease clean libwapi osabl

all: build_tools $(TARGET)

build_tools:
	$(MAKE) -C tools
	$(PWD)/tools/bin2h

test:
	$(MAKE) -C tools test

LINUX:
ifeq ($(OSABL),YES)
	cp -f os/linux/Makefile.6.util $(PWD)/os/linux/Makefile
	$(MAKE) -C $(KSRC) SUBDIRS=$(PWD)/os/linux modules
endif
	cp -f os/linux/Makefile.6 $(PWD)/os/linux/Makefile
	$(MAKE) -C $(KSRC) SUBDIRS=$(PWD)/os/linux modules

ifeq ($(OSABL),YES)
	cp -f os/linux/Makefile.6.netif $(PWD)/os/linux/Makefile
	$(MAKE) -C $(KSRC) SUBDIRS=$(PWD)/os/linux modules
endif

ifeq ($(RT28xx_MODE),AP)
	cp -f $(PWD)/os/linux/$(MODULE)_ap.ko /tftpboot
ifeq ($(OSABL),YES)
	cp -f $(PWD)/os/linux/$(MODULE)_ap_util.ko /tftpboot
	cp -f $(PWD)/os/linux/$(MODULE)_ap_net.ko /tftpboot
endif
	rm -f os/linux/$(MODULE)_ap.ko.lzma
	/root/bin/lzma e os/linux/$(MODULE)_ap.ko os/linux/$(MODULE)_ap.ko.lzma
else
ifeq ($(RT28xx_MODE),APSTA)
	cp -f $(PWD)/os/linux/$(MODULE)_apsta.ko /tftpboot
ifeq ($(OSABL),YES)
	cp -f $(PWD)/os/linux/$(MODULE)_apsta_util.ko /tftpboot
	cp -f $(PWD)/os/linux/$(MODULE)_apsta_net.ko /tftpboot
endif
else
	cp -f $(PWD)/os/linux/$(MODULE)_sta.ko /tftpboot 2>/dev/null || :
ifeq ($(OSABL),YES)
	cp -f $(PWD)/os/linux/$(MODULE)_sta_util.ko /tftpboot 2>/dev/null || :
	cp -f $(PWD)/os/linux/$(MODULE)_sta_net.ko /tftpboot 2>/dev/null || :
endif
endif
endif


release: build_tools
	$(MAKE) -C $(PWD)/striptool -f Makefile.release clean
	$(MAKE) -C $(PWD)/striptool -f Makefile.release
	striptool/striptool.out
ifeq ($(RELEASE), DPO)
	gcc -o striptool/banner striptool/banner.c
	./striptool/banner -b striptool/copyright.gpl -s DPO/ -d DPO_GPL -R
	./striptool/banner -b striptool/copyright.frm -s DPO_GPL/include/firmware.h
endif

prerelease:
ifeq ($(MODULE), 2880)
	$(MAKE) -C $(PWD)/os/linux -f Makefile.release.2880 prerelease
else
	$(MAKE) -C $(PWD)/os/linux -f Makefile.release prerelease
endif
	cp $(PWD)/os/linux/Makefile.DPB $(RTMP_SRC_DIR)/os/linux/.
	cp $(PWD)/os/linux/Makefile.DPA $(RTMP_SRC_DIR)/os/linux/.
	cp $(PWD)/os/linux/Makefile.DPC $(RTMP_SRC_DIR)/os/linux/.
ifeq ($(RT28xx_MODE),STA)
	cp $(PWD)/os/linux/Makefile.DPD $(RTMP_SRC_DIR)/os/linux/.
	cp $(PWD)/os/linux/Makefile.DPO $(RTMP_SRC_DIR)/os/linux/.
endif

clean:
ifeq ($(TARGET), LINUX)
	cp -f os/linux/Makefile.clean os/linux/Makefile
	$(MAKE) -C os/linux clean
	rm -rf os/linux/Makefile
endif

libwapi:
	cp -f os/linux/Makefile.libwapi.6 $(PWD)/os/linux/Makefile
	$(MAKE) -C  $(KSRC) SUBDIRS=$(PWD)/os/linux modules

osutil:
ifeq ($(OSABL),YES)
	cp -f os/linux/Makefile.6.util $(PWD)/os/linux/Makefile
	$(MAKE) -C $(KSRC) SUBDIRS=$(PWD)/os/linux modules
endif

osnet:
ifeq ($(OSABL),YES)
	cp -f os/linux/Makefile.6.netif $(PWD)/os/linux/Makefile
	$(MAKE) -C $(KSRC) SUBDIRS=$(PWD)/os/linux modules
endif

osdrv:
	cp -f os/linux/Makefile.6 $(PWD)/os/linux/Makefile
	$(MAKE) -C $(KSRC) SUBDIRS=$(PWD)/os/linux modules

# Declare the contents of the .PHONY variable as phony.  We keep that information in a variable
.PHONY: $(PHONY)



