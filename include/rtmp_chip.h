/*
 *************************************************************************
 * Ralink Tech Inc.
 * 5F., No.36, Taiyuan St., Jhubei City,
 * Hsinchu County 302,
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2010, Ralink Technology, Inc.
 *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program; if not, write to the                         *
 * Free Software Foundation, Inc.,                                       *
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                       *
 *************************************************************************/


#ifndef	__RTMP_CHIP_H__
#define	__RTMP_CHIP_H__

#include "rtmp_type.h"

struct rtmp_adapter;
struct _RSSI_SAMPLE;

#include "mac_ral/pbf.h"

#include "eeprom.h"


#ifdef RTMP_MAC_USB
#include "mac_ral/rtmp_mac.h"
#include "mac_ral/mac_usb.h"
#endif /* RTMP_MAC_USB */

#ifdef RT65xx
#include "chip/rt65xx.h"
#endif

#ifdef MT76x0
#include "chip/mt76x0.h"
#endif



#include "mcu/mcu.h"

#define IS_RT65XX(_pAd)		((((_pAd)->MACVersion & 0xFFFF0000) == 0x65900000) ||\
							 (((_pAd)->MACVersion & 0xfffff000) == 0x85592000) ||\
							 (((_pAd)->MACVersion & 0xffff0000) == 0x76500000) ||\
							 (((_pAd)->MACVersion & 0xffff0000) == 0x76620000))

#define IS_MT76x0(_pAd)		((((_pAd)->MACVersion & 0xffff0000) == 0x65900000) ||\
							 (((_pAd)->MACVersion & 0xffff0000) == 0x76500000))
#define IS_MT7650(_pAd)		(((_pAd)->chipCap.ChipID & 0xffff0000) == 0x76500000)
#define IS_MT7650U(_pAd)	((((_pAd)->chipCap.ChipID & 0xffff0000) == 0x76500000) && (IS_USB_INF(_pAd)))
#define IS_MT7630(_pAd)		(((_pAd)->chipCap.ChipID & 0xffff0000) == 0x76300000)
#define IS_MT7630U(_pAd)	((((_pAd)->chipCap.ChipID & 0xffff0000) == 0x76300000) && (IS_USB_INF(_pAd)))
#define IS_MT7610(_pAd)		(((_pAd)->chipCap.ChipID & 0xffff0000) == 0x76100000)
#define IS_MT7610U(_pAd)	((((_pAd)->chipCap.ChipID & 0xffff0000) == 0x76100000) && (IS_USB_INF(_pAd)))
#define IS_MT76x2(_pAd)		(((_pAd)->MACVersion & 0xffff0000) == 0x76620000)
#define IS_MT7662(_pAd)		(((_pAd)->chipCap.ChipID & 0xffff0000) == 0x76620000)
#define IS_MT7662U(_pAd)	((((_pAd)->chipCap.ChipID & 0xffff0000) == 0x76620000) && (IS_USB_INF(_pAd)))
#define IS_MT7632(_pAd)		(((_pAd)->chipCap.ChipID & 0xffff0000) == 0x76320000)
#define IS_MT7632U(_pAd)	((((_pAd)->chipCap.ChipID & 0xffff0000) == 0x76320000) && (IS_USB_INF(_pAd)))
#define IS_MT7612(_pAd)		(((_pAd)->chipCap.ChipID & 0xffff0000) == 0x76120000)
#define IS_MT7612U(_pAd)	((((_pAd)->chipCap.ChipID & 0xffff0000) == 0x76120000) && (IS_USB_INF(_pAd)))
#define IS_MT76x0U(_pAd)	(IS_MT7650U(_pAd) || IS_MT7630U(_pAd) || IS_MT7610U(_pAd))
#define IS_MT76xx(_pAd)		(IS_MT76x0(_pAd) || IS_MT76x2(_pAd))

/* RT3592BC8 (WiFi + BT) */

#define IS_USB_INF(_pAd)		((_pAd)->infType == RTMP_DEV_INF_USB)

#define RT_REV_LT(_pAd, _chip, _rev)\
	IS_##_chip(_pAd) && (((_pAd)->MACVersion & 0x0000FFFF) < (_rev))

#define RT_REV_GTE(_pAd, _chip, _rev)\
	IS_##_chip(_pAd) && (((_pAd)->MACVersion & 0x0000FFFF) >= (_rev))

/* Dual-band NIC (RF/BBP/MAC are in the same chip.) */

#define IS_RT_NEW_DUAL_BAND_NIC(_pAd) ((false))


/* Is the NIC dual-band NIC? */

#define IS_DUAL_BAND_NIC(_pAd) (((_pAd->RfIcType == RFIC_2850) || (_pAd->RfIcType == RFIC_2750) || (_pAd->RfIcType == RFIC_3052)		\
								|| (_pAd->RfIcType == RFIC_3053) || (_pAd->RfIcType == RFIC_2853) || (_pAd->RfIcType == RFIC_3853) 	\
								|| IS_RT_NEW_DUAL_BAND_NIC(_pAd)) && !IS_RT5390(_pAd))


/* RT3593 over PCIe bus */
#define RT3593OverPCIe(_pAd) (IS_RT3593(_pAd) && (_pAd->CommonCfg.bPCIeBus == true))

/* RT3593 over PCI bus */
#define RT3593OverPCI(_pAd) (IS_RT3593(_pAd) && (_pAd->CommonCfg.bPCIeBus == false))

/*RT3390,RT3370 */
#define IS_RT3390(_pAd)				(((_pAd)->MACVersion & 0xFFFF0000) == 0x33900000)

#define CCA_AVG_MAX_COUNT	5

/* ------------------------------------------------------ */
/* PCI registers - base address 0x0000 */
/* ------------------------------------------------------ */
#define CHIP_PCI_CFG		0x0000
#define CHIP_PCI_EECTRL		0x0004
#define CHIP_PCI_MCUCTRL	0x0008

#define OPT_14			0x114

#define RETRY_LIMIT		10

/* ------------------------------------------------------ */
/* BBP & RF	definition */
/* ------------------------------------------------------ */
#define	BUSY		                1
#define	IDLE		                0

/*------------------------------------------------------------------------- */
/* EEPROM definition */
/*------------------------------------------------------------------------- */
#define EEDO                        0x08
#define EEDI                        0x04
#define EECS                        0x02
#define EESK                        0x01
#define EERL                        0x80

#define EEPROM_WRITE_OPCODE         0x05
#define EEPROM_READ_OPCODE          0x06
#define EEPROM_EWDS_OPCODE          0x10
#define EEPROM_EWEN_OPCODE          0x13

#define NUM_EEPROM_BBP_PARMS		19	/* Include NIC Config 0, 1, CR, TX ALC step, BBPs */
#define NUM_EEPROM_TX_G_PARMS		7

#define VALID_EEPROM_VERSION        1
#define EEPROM_VERSION_OFFSET       0x02
#define EEPROM_NIC1_OFFSET          0x34	/* The address is from NIC config 0, not BBP register ID */
#define EEPROM_NIC2_OFFSET          0x36	/* The address is from NIC config 1, not BBP register ID */


#define EEPROM_COUNTRY_REGION			0x38

#define EEPROM_DEFINE_MAX_TXPWR			0x4e

#define EEPROM_FREQ_OFFSET			0x3a
#define EEPROM_LEDAG_CONF_OFFSET	0x3c
#define EEPROM_LEDACT_CONF_OFFSET	0x3e
#define EEPROM_LED_POLARITY_OFFSET	0x40
#if defined(BT_COEXISTENCE_SUPPORT)
#define	EEPROM_NIC3_OFFSET			0x42
#endif /* defined(BT_COEXISTENCE_SUPPORT) */

#define EEPROM_LNA_OFFSET			0x44

#define EEPROM_RSSI_BG_OFFSET			0x46
#define EEPROM_RSSI_A_OFFSET			0x4a
#define EEPROM_TXMIXER_GAIN_2_4G		0x48
#define EEPROM_TXMIXER_GAIN_5G			0x4c

#define EEPROM_TXPOWER_DELTA			0x50	/* 20MHZ AND 40 MHZ use different power. This is delta in 40MHZ. */

#define EEPROM_G_TX_PWR_OFFSET			0x52
#define EEPROM_G_TX2_PWR_OFFSET			0x60

#define EEPROM_G_TSSI_BOUND1			0x6e
#define EEPROM_G_TSSI_BOUND2			0x70
#define EEPROM_G_TSSI_BOUND3			0x72
#define EEPROM_G_TSSI_BOUND4			0x74
#define EEPROM_G_TSSI_BOUND5			0x76

#define EEPROM_A_TX_PWR_OFFSET      		0x78
#define EEPROM_A_TX2_PWR_OFFSET			0xa6

#define MBSSID_MODE0 0
#define MBSSID_MODE1 1

enum FREQ_CAL_INIT_MODE {
	FREQ_CAL_INIT_MODE0,
	FREQ_CAL_INIT_MODE1,
	FREQ_CAL_INIT_MODE2,
	FREQ_CAL_INIT_UNKNOW,
};

enum FREQ_CAL_MODE {
	FREQ_CAL_MODE0,
	FREQ_CAL_MODE1,
	FREQ_CAL_MODE2,
};

enum RXWI_FRQ_OFFSET_FIELD {
	RXWI_FRQ_OFFSET_FIELD0, /* SNR1 */
	RXWI_FRQ_OFFSET_FIELD1, /* Frequency Offset */
};


#ifdef MT76x0
#define EEPROM_MT76x0_TEMPERATURE_OFFSET	0xd1 /* signed value */
#define EEPROM_MT76x0_A_BAND_MB				0xdc
#define EEPROM_MT76x0_A_BAND_HB				0xdd

#define EEPROM_MT76x0_2G_TARGET_POWER		0xd0
#define EEPROM_MT76x0_5G_TARGET_POWER		0xd2
#define EEPROM_MT76x0_2G_SLOPE_OFFSET		0x6E
#define EEPROM_MT76x0_5G_SLOPE_OFFSET		0xf0
#define EEPROM_MT76x0_5G_CHANNEL_BOUNDARY 	0xd4
#endif /* MT76x0 */

#define EEPROM_A_TSSI_BOUND1		0xd4
#define EEPROM_A_TSSI_BOUND2		0xd6
#define EEPROM_A_TSSI_BOUND3		0xd8
#define EEPROM_A_TSSI_BOUND4		0xda
#define EEPROM_A_TSSI_BOUND5		0xdc

/* ITxBF calibration values EEPROM locations 0x1a0 to 0x1ab */
#define EEPROM_ITXBF_CAL				0x1a0

#else
#define EEPROM_TXPOWER_BYRATE 			0xde	/* 20MHZ power. */
#define EEPROM_TXPOWER_BYRATE_20MHZ_2_4G	0xde	/* 20MHZ 2.4G tx power. */
#define EEPROM_TXPOWER_BYRATE_40MHZ_2_4G	0xee	/* 40MHZ 2.4G tx power. */
#define EEPROM_TXPOWER_BYRATE_20MHZ_5G		0xfa	/* 20MHZ 5G tx power. */
#define EEPROM_TXPOWER_BYRATE_40MHZ_5G		0x10a	/* 40MHZ 5G tx power. */

#define EEPROM_BBP_BASE_OFFSET			0xf0	/* The address is from NIC config 0, not BBP register ID */

/* */
/* Bit mask for the Tx ALC and the Tx fine power control */
/* */
#define GET_TX_ALC_BIT_MASK					0x1F	/* Valid: 0~31, and in 0.5dB step */
#define GET_TX_FINE_POWER_CTRL_BIT_MASK	0xE0	/* Valid: 0~4, and in 0.1dB step */
#define NUMBER_OF_BITS_FOR_TX_ALC			5	/* The length, in bit, of the Tx ALC field */


/* TSSI gain and TSSI attenuation */

#define EEPROM_TSSI_GAIN_AND_ATTENUATION	0x76

/*#define EEPROM_Japan_TX_PWR_OFFSET      0x90 // 802.11j */
/*#define EEPROM_Japan_TX2_PWR_OFFSET      0xbe */
/*#define EEPROM_TSSI_REF_OFFSET	0x54 */
/*#define EEPROM_TSSI_DELTA_OFFSET	0x24 */
/*#define EEPROM_CCK_TX_PWR_OFFSET  0x62 */
/*#define EEPROM_CALIBRATE_OFFSET	0x7c */

#define EEPROM_NIC_CFG1_OFFSET		0
#define EEPROM_NIC_CFG2_OFFSET		1
#define EEPROM_NIC_CFG3_OFFSET		2
#define EEPROM_COUNTRY_REG_OFFSET	3
#define EEPROM_BBP_ARRAY_OFFSET		4

#ifdef RT_BIG_ENDIAN
typedef union _EEPROM_ANTENNA_STRUC {
	struct {
		USHORT RssiIndicationMode:1; 	/* RSSI indication mode */
		USHORT Rsv:1;
		USHORT BoardType:2; 		/* 0: mini card; 1: USB pen */
		USHORT RfIcType:4;			/* see E2PROM document */
		USHORT TxPath:4;			/* 1: 1T, 2: 2T, 3: 3T */
		USHORT RxPath:4;			/* 1: 1R, 2: 2R, 3: 3R */
	} field;
	USHORT word;
} EEPROM_ANTENNA_STRUC, *PEEPROM_ANTENNA_STRUC;
#else
typedef union _EEPROM_ANTENNA_STRUC {
	struct {
		USHORT RxPath:4;			/* 1: 1R, 2: 2R, 3: 3R */
		USHORT TxPath:4;			/* 1: 1T, 2: 2T, 3: 3T */
		USHORT RfIcType:4;			/* see E2PROM document */
		USHORT BoardType:2; 		/* 0: mini card; 1: USB pen */
		USHORT Rsv:1;
		USHORT RssiIndicationMode:1; 	/* RSSI indication mode */
	} field;
	USHORT word;
} EEPROM_ANTENNA_STRUC, *PEEPROM_ANTENNA_STRUC;
#endif

struct RF_BANK_OFFSET {
	u8 RFBankIndex;
	UINT16 RFStart;
	UINT16 RFEnd;
};

/*
	2860: 28xx
	2870: 28xx

	30xx:
		3090
		3070
		2070 3070

	33xx:	30xx
		3390 3090
		3370 3070

	35xx:	30xx
		3572, 2870, 28xx
		3062, 2860, 28xx
		3562, 2860, 28xx

	3593, 28xx, 30xx, 35xx

	< Note: 3050, 3052, 3350 can not be compiled simultaneously. >
	305x:
		3052
		3050
		3350, 3050

	3352: 305x

	2880: 28xx
	2883:
	3883:
*/

struct rtmp_chip_cap {
	u32 ChipID;

	u32 MaxNumOfRfId;
	u32 MaxNumOfBbpId;

#define RF_REG_WT_METHOD_NONE			0
#define RF_REG_WT_METHOD_STEP_ON		1
	u8 RfReg17WtMethod;

	/* beacon */
	bool FlgIsSupSpecBcnBuf;	/* SPECIFIC_BCN_BUF_SUPPORT */
	u8 BcnMaxNum;	/* software use */
	u8 BcnMaxHwNum;	/* hardware limitation */
	u8 WcidHwRsvNum;	/* hardware available WCID number */
	UINT16 BcnMaxHwSize;	/* hardware maximum beacon size */
	UINT16 BcnBase[HW_BEACON_MAX_NUM];	/* hardware beacon base address */

	/* VCO calibration mode */
	u8	VcoPeriod; /* default 10s */
#define VCO_CAL_DISABLE		0	/* not support */
#define VCO_CAL_MODE_1		1	/* toggle RF7[0] */
#define VCO_CAL_MODE_2		2	/* toggle RF3[7] */
#define VCO_CAL_MODE_3		3	/* toggle RF4[7] */
	u8	FlgIsVcoReCalMode;

#ifdef FIFO_EXT_SUPPORT
	bool FlgHwFifoExtCap;
#endif /* FIFO_EXT_SUPPORT */


	enum ASIC_CAP asic_caps;
	enum PHY_CAP phy_caps;

	/* ---------------------------- signal ---------------------------------- */
#define SNR_FORMULA1		0	/* ((0xeb     - pAd->StaCfg.LastSNR0) * 3) / 16; */
#define SNR_FORMULA2		1	/* (pAd->StaCfg.LastSNR0 * 3 + 8) >> 4; */
#define SNR_FORMULA3		2	/* (pAd->StaCfg.LastSNR0) * 3) / 16; */
	u8 SnrFormula;

	u8 MaxNss;			/* maximum Nss, 3 for 3883 or 3593 */

	bool bTempCompTxALC;
	bool rx_temp_comp;

	bool bLimitPowerRange; /* TSSI compensation range limit */

	/* ---------------------------- packet ---------------------------------- */

	/* ---------------------------- others ---------------------------------- */
#ifdef RTMP_EFUSE_SUPPORT
	UINT16 EFUSE_USAGE_MAP_START;
	UINT16 EFUSE_USAGE_MAP_END;
	u8 EFUSE_USAGE_MAP_SIZE;
#endif /* RTMP_EFUSE_SUPPORT */

#ifdef DFS_SUPPORT
	u8 DfsEngineNum;
#endif /* DFS_SUPPORT */

	/*
		Define the burst size of WPDMA of PCI
		0 : 4 DWORD (16bytes)
		1 : 8 DWORD (32 bytes)
		2 : 16 DWORD (64 bytes)
		3 : 32 DWORD (128 bytes)
	*/
	u8 WPDMABurstSIZE;

	/*
 	 * 0: MBSSID_MODE0
 	 * (The multiple MAC_ADDR/BSSID are distinguished by [bit2:bit0] of byte5)
 	 * 1: MBSSID_MODE1
 	 * (The multiple MAC_ADDR/BSSID are distinguished by [bit4:bit2] of byte0)
 	 */
	u8 MBSSIDMode;

	/* 2nd CCA Detection ++ */
	bool b2ndCCACheck;
	u8 CCARatioA;
	u8 CCARatioB;
	u8 CCACheckPeriod; /* unit: second */
	CHAR RssiThreshold;
	CHAR RssiAvg;
	u32 CCAThresholdA;
	u32 CCAThresholdB;
	ULONG CCARecordCnt; /* unit: second */
	ULONG CCAAvg[CCA_AVG_MAX_COUNT];
	u8 CCAAvgIdx;
	void *pWeakestEntry;
	/* 2nd CCA Detection -- */


#ifdef CONFIG_STA_SUPPORT
	u8 init_vga_gain_5G;
	u8 init_vga_gain_2G;
#endif /* CONFIG_STA_SUPPORT */

#ifdef RT5592EP_SUPPORT
	u32 Priv; /* Flag for RT5592 EP */
#endif /* RT5592EP_SUPPORT */

#ifdef RT65xx
	u8 PAType; /* b'00: 2.4G+5G external PA, b'01: 5G external PA, b'10: 2.4G external PA, b'11: Internal PA */
#endif /* RT65xx */

#ifdef CONFIG_ANDES_SUPPORT
	u32 WlanMemmapOffset;
	u32 InbandPacketMaxLen; /* must be 48 multible */
	u8 CmdRspRxRing;
	bool IsComboChip;
	u8 load_iv;
	u32 ilm_offset;
	u32 dlm_offset;
#endif

	u8 cmd_header_len;
	u8 cmd_padding_len;

#ifdef RTMP_USB_SUPPORT
	u8 DataBulkInAddr;
	u8 CommandRspBulkInAddr;
	u8 WMM0ACBulkOutAddr[4];
	u8 WMM1ACBulkOutAddr;
	u8 CommandBulkOutAddr;
#endif

	u8 *fw_name;		/* ULLI : rename to catch compiler errors */

#ifdef MT76x0
	bool bDoTemperatureSensor;
	SHORT TemperatureOffset;
	SHORT LastTemperatureforVCO;
	SHORT LastTemperatureforCal;
	SHORT NowTemperature;
	u8 a_band_mid_ch;
	u8 a_band_high_ch;
	u8 ext_pa_current_setting;
#endif /* MT76x0 */
};

typedef void (*CHIP_SPEC_FUNC)(void *pAd, void *pData, ULONG Data);

/* The chip specific function ID */
typedef enum _CHIP_SPEC_ID
{
	CHIP_SPEC_RESV_FUNC
} CHIP_SPEC_ID;

#define CHIP_SPEC_ID_NUM 	CHIP_SPEC_RESV_FUNC


struct rtmp_chip_ops {
	/*  Calibration access related callback functions */

	/* MCU related callback functions */
	int (*MCU_loadFirmware)(struct rtmp_adapter *pAd);
	int (*eraseFirmware)(struct rtmp_adapter *pAd);
#ifdef CONFIG_ANDES_SUPPORT
	int (*sendCommandToAndesMcu)(struct rtmp_adapter *pAd, u8 QueIdx, u8 cmd, u8 *pData, USHORT DataLen, bool FlgIsNeedLocked);
#endif

	void (*AsicRfInit)(struct rtmp_adapter *pAd);
	void (*AsicBbpInit)(struct rtmp_adapter *pAd);
	void (*AsicMacInit)(struct rtmp_adapter *pAd);


	/* Power save */
	void (*EnableAPMIMOPS)(struct rtmp_adapter *pAd, IN bool ReduceCorePower);
	void (*DisableAPMIMOPS)(struct rtmp_adapter *pAd);
	INT (*MCU_PwrSavingOP)(struct rtmp_adapter *pAd, u32 PwrOP, u32 PwrLevel,
							u32 ListenInterval, u32 PreTBTTLeadTime,
							u8 TIMByteOffset, u8 TIMBytePattern);

	/* MAC */
	void (*BeaconUpdate)(struct rtmp_adapter *pAd, USHORT Offset, u32 Value, u8 Unit);

	/* Channel */
	void (*ChipSwitchChannel)(struct rtmp_adapter *pAd, u8 ch, bool bScan);

	/* IQ Calibration */
	void (*ChipIQCalibration)(struct rtmp_adapter *pAd, u8 Channel);

	void (*AsicExtraPowerOverMAC)(struct rtmp_adapter *pAd);

	/* high power tuning */
	void (*HighPowerTuning)(struct rtmp_adapter *pAd, struct _RSSI_SAMPLE *pRssi);

	/* The chip specific function list */
	CHIP_SPEC_FUNC ChipSpecFunc[CHIP_SPEC_ID_NUM];

	void (*CckMrcStatusCtrl)(struct rtmp_adapter *pAd);
	void (*RadarGLRTCompensate)(struct rtmp_adapter *pAd);

	int (*MCU_Calibration)(struct rtmp_adapter *pAd, u32 CalibrationID, u32 Parameter);
	void (*SecondCCADetection)(struct rtmp_adapter *pAd);

	int (*MCU_BurstWrite)(struct rtmp_adapter *ad, u32 Offset, u32 *Data, u32 Cnt);

	int (*MCU_RandomRead)(struct rtmp_adapter *ad, RTMP_REG_PAIR *RegPair, u32 Num);

	int (*MCU_RFRandomRead)(struct rtmp_adapter *ad, BANK_RF_REG_PAIR *RegPair, u32 Num);

	int (*MCU_ReadModifyWrite)(struct rtmp_adapter *ad, R_M_W_REG *RegPair, u32 Num);

	int (*MCU_RandomWrite)(struct rtmp_adapter *ad, RTMP_REG_PAIR *RegPair, u32 Num);

	int (*MCU_RFRandomWrite)(struct rtmp_adapter *ad, BANK_RF_REG_PAIR *RegPair, u32 Num);

	void (*DisableTxRx)(struct rtmp_adapter *ad, u8 Level);

	void (*AsicRadioOn)(struct rtmp_adapter *ad, u8 Stage);

	void (*AsicRadioOff)(struct rtmp_adapter *ad, u8 Stage);

	void (*MCU_CtrlInit)(struct rtmp_adapter *ad);

	void (*MCU_CtrlExit)(struct rtmp_adapter *ad);
};

#define RTMP_CHIP_ENABLE_AP_MIMOPS(__pAd, __ReduceCorePower)	\
do {	\
		if (__pAd->chipOps.EnableAPMIMOPS != NULL)	\
			__pAd->chipOps.EnableAPMIMOPS(__pAd, __ReduceCorePower);	\
} while (0)

#define RTMP_CHIP_DISABLE_AP_MIMOPS(__pAd)	\
do {	\
		if (__pAd->chipOps.DisableAPMIMOPS != NULL)	\
			__pAd->chipOps.DisableAPMIMOPS(__pAd);	\
} while (0)

#define PWR_SAVING_OP(__pAd, __PwrOP, __PwrLevel, __ListenInterval, \
						__PreTBTTLeadTime, __TIMByteOffset, __TIMBytePattern)	\
do {	\
		if (__pAd->chipOps.MCU_PwrSavingOP != NULL)	\
			__pAd->chipOps.MCU_PwrSavingOP(__pAd, __PwrOP, __PwrLevel,	\
										__ListenInterval,__PreTBTTLeadTime, \
										__TIMByteOffset, __TIMBytePattern);	\
} while (0)

#define RTMP_CHIP_ASIC_TSSI_TABLE_INIT(__pAd)	\
do {	\
		if (__pAd->chipOps.InitDesiredTSSITable != NULL)	\
			__pAd->chipOps.InitDesiredTSSITable(__pAd);	\
} while (0)

#define RTMP_CHIP_ATE_TSSI_CALIBRATION(__pAd, __pData)	\
do {	\
		if (__pAd->chipOps.ATETssiCalibration != NULL)	\
			__pAd->chipOps.ATETssiCalibration(__pAd, __pData);	\
} while (0)

#define RTMP_CHIP_ATE_TSSI_CALIBRATION_EXTEND(__pAd, __pData)	\
do {	\
		if (__pAd->chipOps.ATETssiCalibrationExtend != NULL)	\
			__pAd->chipOps.ATETssiCalibrationExtend(__pAd, __pData);	\
} while (0)

#define RTMP_CHIP_ATE_READ_EXTERNAL_TSSI(__pAd, __pData)	\
do {	\
		if (__pAd->chipOps.ATEReadExternalTSSI != NULL)	\
			__pAd->chipOps.ATEReadExternalTSSI(__pAd, __pData);	\
} while (0)

#define RTMP_CHIP_ASIC_EXTRA_POWER_OVER_MAC(__pAd)	\
do {	\
		if (__pAd->chipOps.AsicExtraPowerOverMAC != NULL)	\
			__pAd->chipOps.AsicExtraPowerOverMAC(__pAd);	\
} while (0)

#define RTMP_CHIP_ASIC_GET_TSSI_RATIO(__pAd, __DeltaPwr)	\
do {	\
		if (__pAd->chipOps.AsicFreqCalStop != NULL)	\
			__pAd->chipOps.TSSIRatio(__DeltaPwr);	\
} while (0)

#define RTMP_CHIP_ASIC_FREQ_CAL_STOP(__pAd)	\
do {	\
		if (__pAd->chipOps.AsicFreqCalStop != NULL)	\
			__pAd->chipOps.AsicFreqCalStop(__pAd);	\
} while (0)

#define RTMP_CHIP_IQ_CAL(__pAd, __pChannel)	\
do {	\
		if (__pAd->chipOps.ChipIQCalibration != NULL)	\
			 __pAd->chipOps.ChipIQCalibration(__pAd, __pChannel);	\
} while (0)

#define RTMP_CHIP_HIGH_POWER_TUNING(__pAd, __pRssi)	\
do {	\
		if (__pAd->chipOps.HighPowerTuning != NULL)	\
			__pAd->chipOps.HighPowerTuning(__pAd, __pRssi);	\
} while (0)

#define RTMP_CHIP_SPECIFIC(__pAd, __FuncId, __pData, __Data)	\
do {	\
		if ((__FuncId >= 0) && (__FuncId < CHIP_SPEC_RESV_FUNC))	\
		{	\
			if (__pAd->chipOps.ChipSpecFunc[__FuncId] != NULL)	\
				__pAd->chipOps.ChipSpecFunc[__FuncId](__pAd, __pData, __Data);	\
		}	\
} while (0)

#define RTMP_CHIP_ASIC_RESET_BBP_AGENT(__pAd)	\
do {	\
		BBP_CSR_CFG_STRUC	BbpCsr;	\
		DBGPRINT(RT_DEBUG_INFO, ("Reset BBP Agent busy bit.!! \n"));	\
		mt7610u_read32(__pAd, H2M_BBP_AGENT, &BbpCsr.word);	\
		BbpCsr.field.Busy = 0;	\
		mt7610u_write32(__pAd, H2M_BBP_AGENT, BbpCsr.word);	\
} while (0)

#define RTMP_CHIP_UPDATE_BEACON(__pAd, Offset, Value, Unit)	\
do {	\
		if (__pAd->chipOps.BeaconUpdate != NULL)	\
			__pAd->chipOps.BeaconUpdate(__pAd, Offset, Value, Unit);	\
} while (0)

#define RTMP_CHIP_CCK_MRC_STATUS_CTRL(__pAd)	\
do {	\
		if(__pAd->chipOps.CckMrcStatusCtrl != NULL)	\
			__pAd->chipOps.CckMrcStatusCtrl(__pAd);	\
} while (0)

#define RTMP_CHIP_RADAR_GLRT_COMPENSATE(__pAd) \
do {	\
		if(__pAd->chipOps.RadarGLRTCompensate != NULL)	\
			__pAd->chipOps.RadarGLRTCompensate(__pAd);	\
} while (0)


#define CHIP_CALIBRATION(__pAd, __CalibrationID, __parameter) \
do {	\
	if(__pAd->chipOps.MCU_Calibration != NULL) \
		__pAd->chipOps.MCU_Calibration(__pAd, __CalibrationID, __parameter); \
} while (0)

#define RTMP_CHIP_CALIBRATION(__pAd, __CalibrationID, __parameter) \
do {	\
	if(__pAd->chipOps.MCU_Calibration != NULL) \
		__pAd->chipOps.MCU_Calibration(__pAd, __CalibrationID, __parameter); \
} while (0)

#define BURST_WRITE(_pAd, _Offset, _pData, _Cnt)	\
do {												\
		if (_pAd->chipOps.MCU_BurstWrite != NULL)		\
			_pAd->chipOps.MCU_BurstWrite(_pAd, _Offset, _pData, _Cnt);\
} while (0)

#define RF_RANDOM_READ(_pAd, _RegPair, _Num)	\
do {											\
		if (_pAd->chipOps.MCU_RFRandomRead != NULL)	\
			_pAd->chipOps.MCU_RFRandomRead(_pAd, _RegPair, _Num); \
} while (0)

#define READ_MODIFY_WRITE(_pAd, _RegPair, _Num)	\
do {	\
		if (_pAd->chipOps.MCU_ReadModifyWrite != NULL)	\
			_pAd->chipOps.MCU_ReadModifyWrite(_pAd, _RegPair, _Num);	\
} while (0)

#define RF_READ_MODIFY_WRITE(_pAd, _RegPair, _Num)	\
do {	\
		if (_pAd->chipOps.RFReadModifyWrite != NULL)	\
			_pAd->chipOps.RFReadModifyWrite(_pAd, _RegPair, _Num);	\
} while (0)

#define RANDOM_WRITE(_pAd, _RegPair, _Num)	\
do {	\
		if (_pAd->chipOps.MCU_RandomWrite != NULL)	\
			_pAd->chipOps.MCU_RandomWrite(_pAd, _RegPair, _Num);	\
} while (0)

#define RF_RANDOM_WRITE(_pAd, _RegPair, _Num)	\
do {	\
		if (_pAd->chipOps.MCU_RFRandomWrite != NULL)	\
			_pAd->chipOps.MCU_RFRandomWrite(_pAd, _RegPair, _Num);	\
} while (0)

#define RTMP_SECOND_CCA_DETECTION(__pAd)	\
do {	\
	if (__pAd->chipOps.SecondCCADetection != NULL)	\
	{	\
		__pAd->chipOps.SecondCCADetection(__pAd);	\
	}	\
} while (0)

#define DISABLE_TX_RX(_pAd, _Level)	\
do {	\
	if (_pAd->chipOps.DisableTxRx != NULL)	\
		_pAd->chipOps.DisableTxRx(_pAd, _Level);	\
} while (0)

#define ASIC_RADIO_ON(_pAd, _Stage)	\
do {	\
	if (_pAd->chipOps.AsicRadioOn != NULL)	\
		_pAd->chipOps.AsicRadioOn(_pAd, _Stage);	\
} while (0)

#define ASIC_RADIO_OFF(_pAd, _Stage)	\
do {	\
	if (_pAd->chipOps.AsicRadioOff != NULL)	\
		_pAd->chipOps.AsicRadioOff(_pAd, _Stage);	\
} while (0)

#define MCU_CTRL_INIT(_pAd)	\
do {	\
	if (_pAd->chipOps.MCU_CtrlInit != NULL)	\
		_pAd->chipOps.MCU_CtrlInit(_pAd);	\
} while (0)

#define MCU_CTRL_EXIT(_pAd)	\
do {	\
	if (_pAd->chipOps.MCU_CtrlExit != NULL)	\
		_pAd->chipOps.MCU_CtrlExit(_pAd);	\
} while (0)

int RtmpChipOpsHook(void *pCB);
void RtmpChipBcnInit(struct rtmp_adapter *pAd);
void RtmpChipBcnSpecInit(struct rtmp_adapter *pAd);
void rlt_bcn_buf_init(struct rtmp_adapter *pAd);

void RtmpChipWriteMemory(
	IN	struct rtmp_adapter *pAd,
	IN	USHORT			Offset,
	IN	u32			Value,
	IN	u8			Unit);

/* global variable */
extern FREQUENCY_ITEM RtmpFreqItems3020[];
extern FREQUENCY_ITEM FreqItems3020_Xtal20M[];
extern u8 NUM_OF_3020_CHNL;
extern FREQUENCY_ITEM *FreqItems3020;
extern RTMP_RF_REGS RF2850RegTable[];
extern u8 NUM_OF_2850_CHNL;

bool AsicWaitPDMAIdle(struct rtmp_adapter *pAd, INT round, INT wait_us);
INT AsicSetPreTbttInt(struct rtmp_adapter *pAd, bool enable);
INT AsicReadAggCnt(struct rtmp_adapter *pAd, ULONG *aggCnt, int cnt_len);

void StopDmaTx(struct rtmp_adapter *pAd, u8 Level);
void StopDmaRx(struct rtmp_adapter *pAd, u8 Level);

#endif /* __RTMP_CHIP_H__ */
