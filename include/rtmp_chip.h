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


#include "mac_ral/rtmp_mac.h"
#include "mac_ral/mac_usb.h"

#include "chip/rt65xx.h"

#include "chip/mt76x0.h"



#include "mcu/mcu.h"

#define IS_MT76x0(_pAd)		((((_pAd)->mac_rev & 0xffff0000) == 0x65900000) ||\
							 (((_pAd)->mac_rev & 0xffff0000) == 0x76500000))
#define IS_MT7650(_pAd)		(((_pAd)->chipCap.asic_rev & 0xffff0000) == 0x76500000)
#define IS_MT7650U(_pAd)	((((_pAd)->chipCap.asic_rev & 0xffff0000) == 0x76500000))
#define IS_MT7630(_pAd)		(((_pAd)->chipCap.asic_rev & 0xffff0000) == 0x76300000)
#define IS_MT7630U(_pAd)	((((_pAd)->chipCap.asic_rev & 0xffff0000) == 0x76300000))
#define IS_MT7610(_pAd)		(((_pAd)->chipCap.asic_rev & 0xffff0000) == 0x76100000)
#define IS_MT7610U(_pAd)	((((_pAd)->chipCap.asic_rev & 0xffff0000) == 0x76100000))
#define IS_MT76x0U(_pAd)	(IS_MT7650U(_pAd) || IS_MT7630U(_pAd) || IS_MT7610U(_pAd))

/* RT3592BC8 (WiFi + BT) */

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
#define	EEPROM_NIC3_OFFSET	    0x42	/* ULLI BT Coexistence ?? */

#define EEPROM_COUNTRY_REGION			0x38

#define EEPROM_DEFINE_MAX_TXPWR			0x4e

#define EEPROM_FREQ_OFFSET			0x3a
#define EEPROM_LEDAG_CONF_OFFSET	0x3c
#define EEPROM_LEDACT_CONF_OFFSET	0x3e
#define EEPROM_LED_POLARITY_OFFSET	0x40

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


#define EEPROM_MT76x0_TEMPERATURE_OFFSET	0xd1 /* signed value */
#define EEPROM_MT76x0_A_BAND_MB				0xdc
#define EEPROM_MT76x0_A_BAND_HB				0xdd

#define EEPROM_MT76x0_2G_TARGET_POWER		0xd0
#define EEPROM_MT76x0_5G_TARGET_POWER		0xd2
#define EEPROM_MT76x0_2G_SLOPE_OFFSET		0x6E
#define EEPROM_MT76x0_5G_SLOPE_OFFSET		0xf0
#define EEPROM_MT76x0_5G_CHANNEL_BOUNDARY 	0xd4

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
	u32 asic_rev;

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
	UINT16 EFUSE_USAGE_MAP_START;
	UINT16 EFUSE_USAGE_MAP_END;
	u8 EFUSE_USAGE_MAP_SIZE;

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

	u8 PAType; /* b'00: 2.4G+5G external PA, b'01: 5G external PA, b'10: 2.4G external PA, b'11: Internal PA */

#ifdef CONFIG_ANDES_SUPPORT
	u32 WlanMemmapOffset;
	u8 CmdRspRxRing;
	bool IsComboChip;
	u8 load_iv;
	u32 ilm_offset;
	u32 dlm_offset;
#endif

	u8 *fw_name;		/* ULLI : rename to catch compiler errors */

	SHORT TemperatureOffset;
	SHORT LastTemperatureforVCO;
	SHORT LastTemperatureforCal;
	SHORT NowTemperature;
	u8 a_band_mid_ch;
	u8 a_band_high_ch;
	u8 ext_pa_current_setting;
};

typedef void (*CHIP_SPEC_FUNC)(void *pAd, void *pData, ULONG Data);

/* The chip specific function ID */
typedef enum _CHIP_SPEC_ID
{
	CHIP_SPEC_RESV_FUNC
} CHIP_SPEC_ID;

#define CHIP_SPEC_ID_NUM 	CHIP_SPEC_RESV_FUNC

void RtmpChipOpsHook(struct rtmp_adapter *pAd);
void rlt_bcn_buf_init(struct rtmp_adapter *pAd);

bool mt7610u_wait_pdma_usecs(struct rtmp_adapter *pAd, int round, int wait_us);
INT AsicSetPreTbttInt(struct rtmp_adapter *pAd, bool enable);
INT AsicReadAggCnt(struct rtmp_adapter *pAd, ULONG *aggCnt, int cnt_len);

#endif /* __RTMP_CHIP_H__ */
