# Support QA ATE function
HAS_QA_SUPPORT=n

HAS_RSSI_FEEDBACK=n

# Support XLINK mode
HAS_XLINK=n


HAS_NINTENDO=n

# Support LLTD function
HAS_LLTD=n

# Support WDS function
HAS_WDS=n

# Support AP-Client function
HAS_APCLI=n

# Support Wpa_Supplicant
# i.e. wpa_supplicant -Dralink
HAS_WPA_SUPPLICANT=y


# Support Native WpaSupplicant for Network Maganger
# i.e. wpa_supplicant -Dwext
HAS_NATIVE_WPA_SUPPLICANT_SUPPORT=y

#Support Net interface block while Tx-Sw queue full
HAS_BLOCK_NET_IF=n

#Support IGMP-Snooping function.
HAS_IGMP_SNOOP_SUPPORT=n

#Support DFS function
HAS_DFS_SUPPORT=n

#Support Carrier-Sense function
HAS_CS_SUPPORT=n

#Support WEPAUTO mode try Open first then shared
HAS_WEPAUTO_OPEN_FIRST_SUPPORT=n


# Support user specific transmit rate of Multicast packet.
HAS_MCAST_RATE_SPECIFIC_SUPPORT=n

#Support for PCI-MSI
HAS_MSI_SUPPORT=n


#Support for IEEE802.11e DLS
HAS_QOS_DLS_SUPPORT=n

#Support for IDS
HAS_IDS_SUPPORT=n


#Support for Net-SNMP
HAS_SNMP_SUPPORT=n

#Support features of 802.11n Draft3
HAS_DOT11N_DRAFT3_SUPPORT=y

#Support features of 802.11n
HAS_DOT11_N_SUPPORT=y

#Support for 802.11ac VHT
HAS_DOT11_VHT_SUPPORT=y



#Support for 2860/2880 co-exist
HAS_RT2880_RT2860_COEXIST=n

HAS_KTHREAD_SUPPORT=n





#Support for WiFi Display
HAS_WFD_SUPPORT=n

#Support for Auto channel select enhance
HAS_AUTO_CH_SELECT_ENHANCE=n

#Support statistics count
HAS_STATS_COUNT=y

#Support TSSI Antenna Variation
HAS_TSSI_ANTENNA_VARIATION=n

#Support USB_BULK_BUF_ALIGMENT
HAS_USB_BULK_BUF_ALIGMENT=n

#Support for USB_SUPPORT_SELECTIVE_SUSPEND
HAS_USB_SUPPORT_SELECTIVE_SUSPEND=n

#Support USB load firmware by multibyte
HAS_USB_FIRMWARE_MULTIBYTE_WRITE=n

#Support TXRX SW Antenna Diversity
HAS_TXRX_SW_ANTDIV_SUPPORT=n

#Support for Bridge Fast Path & Bridge Fast Path function open to other module
HAS_BGFP_SUPPORT=n
HAS_BGFP_OPEN_SUPPORT=n

# Support HOSTAPD function
HAS_HOSTAPD_SUPPORT=n

#Support GreenAP function
HAS_GREENAP_SUPPORT=n

#Support MAC80211 LINUX-only function
#Please make sure the version for CFG80211.ko and MAC80211.ko is same as the one
#our driver references to.
HAS_CFG80211_SUPPORT=y

#Support RFKILL hardware block/unblock LINUX-only function
HAS_RFKILL_HW_SUPPORT=n



HAS_APCLI_WPA_SUPPLICANT=n

HAS_RTMP_FLASH_SUPPORT=n

HAS_LED_CONTROL_SUPPORT=y


#Support WIDI feature
#Must enable HAS_WSC at the same time.

HAS_NEW_RATE_ADAPT_SUPPORT=y

HAS_RATE_ADAPT_AGS_SUPPORT=n





HAS_TEMPERATURE_TX_ALC=y

HAS_WOW_SUPPORT=n
HAS_WOW_IFDOWN_SUPPORT=n

HAS_HDR_TRANS_SUPPORT=n

HAS_MAC_REPEATER_SUPPORT=n

HAS_WIFI_P2P_CONCURRENT_FAST_SCAN=n

STA_DRIVER_BUILD := 20141022-1327

#################################################

ifndef $(WFLAGS)
WFLAGS := -DSTA_DRIVER_BUILD="\"${STA_DRIVER_BUILD}\""
else
WFLAGS += -DSTA_DRIVER_BUILD="\"${STA_DRIVER_BUILD}\""
endif

WFLAGS += -g -DAGGREGATION_SUPPORT -DPIGGYBACK_SUPPORT -DWMM_SUPPORT  -DLINUX -Wall -Wstrict-prototypes -Wno-trigraphs
WFLAGS += -DRT28xx_MODE=$(RT28xx_MODE) -DCHIPSET=$(MODULE) -DRESOURCE_PRE_ALLOC -DENHANCED_STAT_DISPLAY
WFLAGS += -I$(PWD)/include





ifeq ($(HAS_WIFI_P2P_CONCURRENT_FAST_SCAN),y)
WFLAGS += -DWIFI_P2P_CONCURRENT_FAST_SCAN
endif

ifeq ($(HAS_HE_SUPPORT),y)
WFLAGS += -DHE_SUPPORT
WFLAGS += -DIP_ASSEMBLY
endif

ifeq ($(HAS_HE_BD_SUPPORT),y)
WFLAGS += -DHE_BD_SUPPORT
endif

ifeq ($(HAS_WEPAUTO_OPEN_FIRST_SUPPORT),y)
WFLAGS += -DWEPAUTO_OPEN_FIRST
endif

ifeq ($(HAS_HE_TV_SUPPORT),y)
WFLAGS += -DHE_TV_SUPPORT
endif

ifeq ($(HAS_KTHREAD_SUPPORT),y)
WFLAGS += -DKTHREAD_SUPPORT
endif

ifeq ($(HAS_RTMP_FLASH_SUPPORT),y)
WFLAGS += -DRTMP_FLASH_SUPPORT
endif

ifeq ($(HAS_HDR_TRANS_SUPPORT),y)
WFLAGS += -DHDR_TRANS_SUPPORT
endif

ifeq ($(HAS_DOT11_VHT_SUPPORT),y)
WFLAGS += -DDOT11_VHT_AC
endif


ifeq ($(HAS_MAC_REPEATER_SUPPORT),y)
WFLAGS += -DMAC_REPEATER_SUPPORT
endif


#################################################

# config for STA mode

ifeq ($(RT28xx_MODE),STA)
WFLAGS += -DCONFIG_STA_SUPPORT -DSCAN_SUPPORT -DDBG

ifeq ($(HAS_HDR_TRANS_SUPPORT),y)
WFLAGS += -DHDR_TRANS_TX_SUPPORT
endif

ifeq ($(HAS_XLINK),y)
WFLAGS += -DXLINK_SUPPORT
endif


ifeq ($(HAS_WPA_SUPPLICANT),y)
WFLAGS += -DWPA_SUPPLICANT_SUPPORT
ifeq ($(HAS_NATIVE_WPA_SUPPLICANT_SUPPORT),y)
WFLAGS += -DNATIVE_WPA_SUPPLICANT_SUPPORT
endif
endif

ifeq ($(HAS_SNMP_SUPPORT),y)
WFLAGS += -DSNMP_SUPPORT
endif

ifeq ($(HAS_QOS_DLS_SUPPORT),y)
WFLAGS += -DQOS_DLS_SUPPORT
endif

ifeq ($(HAS_DOT11_N_SUPPORT),y)
WFLAGS += -DDOT11_N_SUPPORT

ifeq ($(HAS_DOT11N_DRAFT3_SUPPORT),y)
WFLAGS += -DDOT11N_DRAFT3
endif

ifeq ($(HAS_NEW_RATE_ADAPT_SUPPORT),y)
WFLAGS += -DNEW_RATE_ADAPT_SUPPORT
endif

endif


ifeq ($(HAS_WFD_SUPPORT),y)
HAS_P2P_SUPPORT=y
HAS_P2P_SPECIFIC_WIRELESS_EVENT=y
WFLAGS += -DWFD_SUPPORT
endif


ifeq ($(HAS_CS_SUPPORT),y)
WFLAGS += -DCARRIER_DETECTION_SUPPORT
endif

ifeq ($(HAS_STATS_COUNT),y)
WFLAGS += -DSTATS_COUNT_SUPPORT
endif

ifeq ($(HAS_TSSI_ANTENNA_VARIATION),y)
WFLAGS += -DTSSI_ANTENNA_VARIATION
endif

ifeq ($(HAS_USB_SUPPORT_SELECTIVE_SUSPEND),y)
WFLAGS += -DUSB_SUPPORT_SELECTIVE_SUSPEND
endif

ifeq ($(HAS_USB_FIRMWARE_MULTIBYTE_WRITE),y)
WFLAGS += -DUSB_FIRMWARE_MULTIBYTE_WRITE -DMULTIWRITE_BYTES=4
endif

ifeq ($(HAS_CFG80211_SUPPORT),y)
WFLAGS += -DRT_CFG80211_SUPPORT
ifeq ($(HAS_RFKILL_HW_SUPPORT),y)
WFLAGS += -DRFKILL_HW_SUPPORT
endif
endif

ifeq ($(HAS_TXRX_SW_ANTDIV_SUPPORT),y)
WFLAGS += -DTXRX_SW_ANTDIV_SUPPORT
endif


ifeq ($(HAS_WIDI_SUPPORT),y)
WFLAGS += -DWIDI_SUPPORT

ifeq ($(HAS_P2P_SUPPORT),y)
ifeq ($(HAS_INTEL_WFD_SUPPORT),y)
WFLAGS += -DINTEL_WFD_SUPPORT
endif

ifeq ($(HAS_WFA_WFD_SUPPORT),y)
WFLAGS += -DWFA_WFD_SUPPORT
endif
endif

endif

ifeq ($(HAS_WOW_SUPPORT),y)
WFLAGS += -DWOW_SUPPORT
endif

ifeq ($(HAS_WOW_IFDOWN_SUPPORT),y)
WFLAGS += -DWOW_IFDOWN_SUPPORT
endif

endif
# endif of ifeq ($(RT28xx_MODE),STA)

#################################################

#################################################

#
# Common compiler flag
#

ifeq ($(HAS_IDS_SUPPORT),y)
WFLAGS += -DIDS_SUPPORT
endif

ifeq ($(HAS_BGFP_SUPPORT),y)
WFLAGS += -DBG_FT_SUPPORT
endif

ifeq ($(HAS_BGFP_OPEN_SUPPORT),y)
WFLAGS += -DBG_FT_OPEN_SUPPORT
endif


ifeq ($(HAS_LED_CONTROL_SUPPORT),y)
WFLAGS += -DLED_CONTROL_SUPPORT
endif

ifeq ($(HAS_TEMPERATURE_TX_ALC),y)
WFLAGS += -DRTMP_TEMPERATURE_TX_ALC
endif

ifneq ($(or $(findstring mt7650u,$(CHIPSET)),$(findstring mt7630u,$(CHIPSET)),$(findstring mt7610u,$(CHIPSET))),)
WFLAGS += -DMT76x0 -DRT65xx -DRLT_MAC -DRLT_RF -DRTMP_MAC_USB -DRTMP_USB_SUPPORT -DRTMP_TIMER_TASK_SUPPORT -DA_BAND_SUPPORT -DRTMP_EFUSE_SUPPORT -DCONFIG_ANDES_SUPPORT
#-DRTMP_FREQ_CALIBRATION_SUPPORT
#-DRX_DMA_SCATTER
ifneq ($(findstring mt7650u,$(CHIPSET)),)
WFLAGS += -DMT7650
endif

ifneq ($(findstring mt7630u,$(CHIPSET)),)
WFLAGS += -DMT7630
endif

ifneq ($(findstring mt7610u,$(CHIPSET)),)
WFLAGS += -DMT7610
endif

ifneq ($(findstring $(RT28xx_MODE),AP),)
#WFLAGS += -DSPECIFIC_BCN_BUF_SUPPORT
endif

ifeq ($(HAS_CSO_SUPPORT), y)
WFLAGS += -DCONFIG_CSO_SUPPORT -DCONFIG_TSO_SUPPORT
endif

CHIPSET_DAT = 2870
endif

ifneq ($(or $(findstring mt7662u,$(CHIPSET)),$(findstring mt7662u,$(CHIPSET))),)
WFLAGS += -DMT76x2 -DRT65xx -DRLT_MAC -DRLT_RF -DRTMP_MAC_USB -DRTMP_USB_SUPPORT -DRTMP_TIMER_TASK_SUPPORT -DA_BAND_SUPPORT -DRTMP_EFUSE_SUPPORT -DCONFIG_ANDES_SUPPORT

ifneq ($(findstring mt7662u,$(CHIPSET)),)
WFLAGS += -DMT7662
endif

ifneq ($(findstring mt7612u,$(CHIPSET)),)
WFLAGS += -DMT7612
endif

ifneq ($(findstring $(RT28xx_MODE),AP),)
#WFLAGS += -DSPECIFIC_BCN_BUF_SUPPORT
endif

ifeq ($(HAS_CSO_SUPPORT), y)
WFLAGS += -DCONFIG_CSO_SUPPORT -DCONFIG_TSO_SUPPORT
endif

CHIPSET_DAT = 2870
endif

#################################################


ifeq ($(HAS_BLOCK_NET_IF),y)
WFLAGS += -DBLOCK_NET_IF
endif

ifeq ($(HAS_DFS_SUPPORT),y)
WFLAGS += -DDFS_SUPPORT
endif

ifeq ($(HAS_LLTD),y)
WFLAGS += -DLLTD_SUPPORT
endif


#kernel build options for 2.4
# move to Makefile outside LINUX_SRC := /opt/star/kernel/linux-2.4.27-star

ifeq ($(PLATFORM),PC)
	EXTRA_CFLAGS := $(WFLAGS)
endif


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
CHIPSET = mt7610u mt7630u mt7650u
endif

MODULE = $(word 1, $(CHIPSET))

PWD = $(shell pwd)

RTMP_SRC_DIR = $(PWD)/RT$(MODULE)

#PLATFORM: Target platform
PLATFORM = PC

#APSOC

#RELEASE Package
RELEASE = DPO

obj_wsc :=
obj_vht :=
obj_cmm := \
		common/crypt_md5.o\
		common/crypt_sha2.o\
		common/crypt_hmac.o\
		common/crypt_aes.o\
		common/crypt_arc4.o\
		common/mlme.o\
		common/cmm_wep.o\
		common/action.o\
		common/cmm_data.o\
		common/rtmp_init.o\
		common/rtmp_init_inf.o\
		common/cmm_tkip.o\
		common/cmm_aes.o\
		common/cmm_sync.o\
		common/eeprom.o\
		common/cmm_sanity.o\
		common/cmm_info.o\
		common/cmm_cfg.o\
		common/cmm_wpa.o\
		common/cmm_radar.o\
		common/spectrum.o\
		common/rtmp_timer.o\
		common/rt_channel.o\
		common/cmm_profile.o\
		common/cmm_asic.o\
		common/scan.o\
		common/cmm_cmd.o\
		common/uapsd.o\
		common/ps.o\
		rate_ctrl/ra_ctrl.o\
		rate_ctrl/alg_legacy.o\
		rate_ctrl/alg_ags.o\
		chips/rtmp_chip.o\
		common/txpower.o\
		mac/rtmp_mac.o\
		mgmt/mgmt_hw.o\
		mgmt/mgmt_entrytb.o\
		phy/rtmp_phy.o\
		phy/rlt_phy.o\
		phy/rlt_rf.o

ifeq ($(HAS_BLOCK_NET_IF),y)
obj_cmm += 	common/netif_block.o
endif

ifeq ($(HAS_NEW_RATE_ADAPT_SUPPORT),y)
obj_cmm += 	rate_ctrl/alg_grp.o
endif

ifeq ($(HAS_RATE_ADAPT_AGS_SUPPORT),y)
obj_cmm += 	rate_ctrl/alg_ags.o
endif

ifeq ($(HAS_DFS_SUPPORT),y)
obj_cmm += 	common/cmm_dfs.o
endif

ifeq ($(HAS_CS_SUPPORT),y)
obj_cmm += 	common/cmm_cs.o
endif

#ifdef DOT11_N_SUPPORT
ifeq ($(HAS_DOT11_N_SUPPORT),y)
obj_cmm += \
		common/ba_action.o\
		mgmt/mgmt_ht.o

endif
#endif // DOT11_N_SUPPORT //

#ifdef DOT11_VHT_SUPPORT
ifeq ($(HAS_DOT11_VHT_SUPPORT),y)
obj_vht += 	mgmt/mgmt_vht.o\
		common/vht.o
endif
#endif // DOT11_VHT_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
MOD_NAME = $(MODULE)
#endif // CONFIG_STA_SUPPORT //

obj-m := $(MOD_NAME).o

#ifdef CONFIG_STA_SUPPORT
ifeq ($(RT28xx_MODE), STA)
$(MOD_NAME)-objs := \
		os/linux/rt_profile.o\
		sta/assoc.o\
		sta/auth.o\
		sta/auth_rsp.o\
		sta/sync.o\
		sta/sanity.o\
		sta/rtmp_data.o\
		sta/connect.o\
		sta/wpa.o\
		sta/sta_cfg.o\
		$(obj_vht)\
		$(obj_cmm)\
		$(obj_wsc)

$(MOD_NAME)-objs += \
		common/rt_os_util.o\
		os/linux/sta_ioctl.o\
		os/linux/rt_linux.o\
		os/linux/rt_main_dev.o

#ifdef ETH_CONVERT
ifeq ($(HAS_ETH_CONVERT_SUPPORT), y)
$(MOD_NAME)-objs += \
		common/cmm_mat.o \
		common/cmm_mat_iparp.o \
		common/cmm_mat_pppoe.o \
		common/cmm_mat_ipv6.o
endif
#endif // ETH_CONVERT //

ifeq ($(HAS_QOS_DLS_SUPPORT),y)
$(MOD_NAME)-objs += \
		sta/dls.o
endif


#ifdef WFD_SUPPORT
ifeq ($(HAS_WFD_SUPPORT),y)
$(MOD_NAME)-objs += \
		common/wfd.o
endif
#endif // WFD_SUPPORT //


#ifdef LED_CONTROL_SUPPORT
ifeq ($(HAS_LED_CONTROL_SUPPORT),y)
$(MOD_NAME)-objs += \
		common/rt_led.o
endif
#endif // LED_CONTROL_SUPPORT //

#ifdef CRDA_SUPPORT
ifeq ($(HAS_CFG80211_SUPPORT),y)
$(MOD_NAME)-objs += \
		os/linux/cfg80211.o \
		os/linux/cfg80211drv.o
endif

#endif // CRDA_SUPPORT //
endif
#endif // CONFIG_STA_SUPPORT //



#chip releated

#ifdef MT76x0
ifneq ($(or $(findstring mt7650u,$(CHIPSET)),$(findstring mt7630u,$(CHIPSET)),$(findstring mt7610u,$(CHIPSET))),)
$(MOD_NAME)-objs += \
		common/cmm_mac_usb.o\
		common/cmm_data_usb.o\
		common/rtusb_io.o\
		common/rtusb_data.o\
		common/rtusb_bulk.o\
		os/linux/rt_usb.o\
		common/ee_prom.o\
		common/ee_efuse.o\
		common/rt_rf.o\
		chips/rt65xx.o\
		chips/mt76x0.o\
		mac/ral_nmac.o\
		mcu/mcu.o\
		mcu/mcu_and.o

ifeq ($(HAS_RTMP_FLASH_SUPPORT),y)
$(MOD_NAME)-objs += \
		common/ee_flash.o
endif

ifeq ($(HAS_TSO_SUPPORT),y)
$(MOD_NAME)-objs += \
		naf/net_acc.o\
		naf/cso.o
endif

ifeq ($(HAS_CSO_SUPPORT), y)
$(MOD_NAME)-objs += \
		naf/net_acc.o\
		naf/cso.o
endif

$(MOD_NAME)-objs += \
                os/linux/rt_usb.o\
                os/linux/rt_usb_util.o\
                os/linux/usb_main_dev.o

ifneq ($(findstring $(RT28xx_MODE),STA APSTA),)
$(MOD_NAME)-objs += \
		common/frq_cal.o
endif

endif
#endif // MT76x0 //

EXTRA_CFLAGS += -Wno-unused-label
EXTRA_CFLAGS += -Wno-unused

MAKE = make

ifeq ($(PLATFORM),PC)
# Linux 2.6
KSRC = /lib/modules/$(shell uname -r)/build
CROSS_COMPILE =
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
SUBARCH := $(shell uname -m | sed -e s/i.86/i386/)
ARCH ?= $(SUBARCH)
endif

export PWD RT28xx_MODE KSRC CROSS_COMPILE CROSS_COMPILE_INCLUDE PLATFORM RELEASE CHIPSET MODULE RTMP_SRC_DIR TARGET HAS_WOW_SUPPORT

# The targets that may be used.
PHONY += all build_tools test LINUX clean

all: modules

modules:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KSRC) M=$(PWD) modules

clean:
	rm -f */*.o
	rm -f */.*.{cmd,flags,d}
	rm -f *.{o,ko,mod.{o,c}}
	rm -f */*/*.{o,ko,mod.{o,c}}
	rm -f */*/.*.{cmd,flags,d}
	rm -fr .tmp_versions
	rm -f include/mcu/mt76*
#Must clean Module.symvers; or you will suffer symbol version not match
#when OS_ABL = YES.
	rm -f Module.symvers
	rm -f Modules.symvers
	rm -f Module.markers
	rm -f modules.order

installfw:
	cp -n firmware/* /lib/firmware

help:
	@echo "options :"
	@echo "modules		build this module"
	@echo "installfw	install firmware file"
	@echo "clean		clean"
	@echo "help		this help text"


# Declare the contents of the .PHONY variable as phony.  We keep that information in a variable
.PHONY: $(PHONY)



