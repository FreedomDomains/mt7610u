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


#ifndef __RT_ATE_H__
#define __RT_ATE_H__

#ifdef RALINK_ATE
#ifndef STATS_COUNT_SUPPORT
#error "For ATE support, please set HAS_ATE=y and HAS_STATS_COUNT=y."
#endif /* !STATS_COUNT_SUPPORT */
#endif /* RALINK_ATE */

typedef struct _ATE_CHIP_STRUCT {
	/* functions */
	void (*ChannelSwitch)(struct rtmp_adapter *pAd);
	INT		(*TxPwrHandler)(struct rtmp_adapter *pAd, char index);
	INT		(*TssiCalibration)(struct rtmp_adapter *pAd, char *arg);
	INT		(*ExtendedTssiCalibration)(struct rtmp_adapter *pAd, char *arg);
	void (*RxVGAInit)(struct rtmp_adapter *pAd);
	void (*AsicSetTxRxPath)(struct rtmp_adapter *pAd);
	void (*AdjustTxPower)(struct rtmp_adapter *pAd);
	void (*AsicExtraPowerOverMAC)(struct rtmp_adapter *pAd);

	/* command handlers */
	INT		(*Set_BW_Proc)(struct rtmp_adapter *pAd, char *arg);
	INT		(*Set_FREQ_OFFSET_Proc)(struct rtmp_adapter *pAd, char *arg);

	/* variables */
	INT maxTxPwrCnt;
	BOOLEAN bBBPStoreTXCARR;
	BOOLEAN bBBPStoreTXCARRSUPP;
	BOOLEAN bBBPStoreTXCONT;
	BOOLEAN bBBPLoadATESTOP;

}ATE_CHIP_STRUCT, *PATE_CHIP_STRUCT;

typedef union _CAPTURE_MODE_SHARE_MEMORY {
	struct
	{
		UINT32       LOW_BYTE0:8;
		UINT32       LOW_BYTE1:8;
		UINT32       HIGH_BYTE0:8;
		UINT32       HIGH_BYTE1:8;
	} field;
	UINT32                   Value;
}CAPTURE_MODE_SHARE_MEMORY, *PCAPTURE_MODE_SHARE_MEMORY;

typedef struct _ATE_INFO {
	PATE_CHIP_STRUCT pChipStruct;
	UCHAR Mode;
	BOOLEAN PassiveMode;
#ifdef RT3350
	UCHAR   PABias;
#endif /* RT3350 */
	CHAR TxPower0;
	CHAR TxPower1;
#ifdef DOT11N_SS3_SUPPORT
	CHAR TxPower2;
#endif /* DOT11N_SS3_SUPPORT */
	CHAR MinTxPowerBandA; /* Power range of early chipsets is -7~15 in A band */
	CHAR MaxTxPowerBandA; /* Power range of early chipsets is -7~15 in A band */
	CHAR TxAntennaSel;
	CHAR RxAntennaSel;
#ifdef RLT_MAC
	USHORT TxInfoLen;
	USHORT TxWILen;
#endif /* RLT_MAC */
	TXINFO_STRUC TxInfo;	/* TxInfo */
	TXWI_STRUC TxWI;	/* TXWI */
	USHORT QID;
	UCHAR Addr1[MAC_ADDR_LEN];
	UCHAR Addr2[MAC_ADDR_LEN];
	UCHAR Addr3[MAC_ADDR_LEN];
	UCHAR Channel;
	UCHAR Payload;		/* Payload pattern */
	UCHAR TxMethod; /* Early chipsets must be applied old TXCONT/TXCARR/TXCARS mechanism. */
	UINT32 TxLength;
	UINT32 TxCount;
	UINT32 TxDoneCount;	/* Tx DMA Done */
	UINT32 RFFreqOffset;
	UINT32 IPG;
	BOOLEAN bRxFER;		/* Show Rx Frame Error Rate */
	BOOLEAN	bQAEnabled;	/* QA is used. */
	BOOLEAN bQATxStart;	/* Have compiled QA in and use it to ATE tx. */
	BOOLEAN bQARxStart;	/* Have compiled QA in and use it to ATE rx. */
	BOOLEAN bAutoTxAlc;	/* Set Auto Tx Alc */
	BOOLEAN bAutoVcoCal;/* Set Auto VCO periodic calibration. */
#ifdef RTMP_INTERNAL_TX_ALC
#if defined(RT3350) || defined(RT3352)
	BOOLEAN bTSSICalbrEnableG; /* Enable TSSI calibration */
	CHAR	TssiRefPerChannel[CFG80211_NUM_OF_CHAN_2GHZ];
	CHAR	TssiDeltaPerChannel[CFG80211_NUM_OF_CHAN_2GHZ];
#endif /* defined(RT3350) || defined(RT3352) */
#endif /* RTMP_INTERNAL_TX_ALC */
#if defined(RT5592) || defined(RT6352)
#ifdef RTMP_TEMPERATURE_COMPENSATION
	BOOLEAN bTSSICalbrEnableG; /* Enable G-band TSSI calibration */
	BOOLEAN bTSSICalbrEnableA; /* Enable A-band TSSI calibration */
	CHAR	TssiReadSampleG[TSSI_READ_SAMPLE_NUM];
	CHAR	TssiReadSampleA[TSSI_READ_SAMPLE_NUM];
#endif /* RTMP_TEMPERATURE_COMPENSATION */
	UCHAR	rx_agc_fc_offset20M;
	UCHAR	rx_agc_fc_offset40M;
	UCHAR	CaliBW20RfR24;
	UCHAR	CaliBW40RfR24;
#endif /* defined(RT5592) || defined(RT6352) */
#ifdef TXBF_SUPPORT
	BOOLEAN bTxBF;		/* Enable Tx Bean Forming */
	SHORT	txSoundingMode;	/* Sounding mode for non-QA ATE. 0=none, 1=Data Sounding, 2=NDP */
	UCHAR	calParams[2];
#endif				/* TXBF_SUPPORT */
	UINT32 RxTotalCnt;
	UINT32 RxCntPerSec;
	UCHAR	forceBBPReg;	/* force to not update the specific BBP register, now used for ATE TxBF */

	CHAR LastSNR0;		/* last received SNR */
	CHAR LastSNR1;		/* last received SNR for 2nd  antenna */
#ifdef DOT11N_SS3_SUPPORT
	CHAR LastSNR2;
#endif				/* DOT11N_SS3_SUPPORT */
	CHAR LastRssi0;		/* last received RSSI */
	CHAR LastRssi1;		/* last received RSSI for 2nd  antenna */
	CHAR LastRssi2;		/* last received RSSI for 3rd  antenna */
	CHAR AvgRssi0;		/* last 8 frames' average RSSI */
	CHAR AvgRssi1;		/* last 8 frames' average RSSI */
	CHAR AvgRssi2;		/* last 8 frames' average RSSI */
	SHORT AvgRssi0X8;	/* sum of last 8 frames' RSSI */
	SHORT AvgRssi1X8;	/* sum of last 8 frames' RSSI */
	SHORT AvgRssi2X8;	/* sum of last 8 frames' RSSI */
	UINT32 NumOfAvgRssiSample;
	UINT32 Default_TX_PIN_CFG;
	USHORT HLen;		/* Header Length */

#ifdef RALINK_QA
	/* Tx frame */
#ifdef RTMP_MAC_USB
//	TXINFO_STRUC TxInfo;	/* TxInfo */
#endif /* RTMP_MAC_USB */
	USHORT PLen;		/* Pattern Length */
	UCHAR Header[32];	/* Header buffer */
	UCHAR Pattern[32];	/* Pattern buffer */
	USHORT DLen;		/* Data Length */
	USHORT seq;
	UINT32 CID;
	RTMP_OS_PID AtePid;
	/* counters */
	UINT32 U2M;
	UINT32 OtherData;
	UINT32 Beacon;
	UINT32 OtherCount;
	UINT32 TxAc0;
	UINT32 TxAc1;
	UINT32 TxAc2;
	UINT32 TxAc3;
	UINT32 TxHCCA;
	UINT32 TxMgmt;
	UINT32 RSSI0;
	UINT32 RSSI1;
	UINT32 RSSI2;
	UINT32 SNR0;
	UINT32 SNR1;
#ifdef DOT11N_SS3_SUPPORT
	UINT32 SNR2;
#endif /* DOT11N_SS3_SUPPORT */
	INT32 BF_SNR[3];	/* Last RXWI BF SNR. Units=0.25 dB */
	/* TxStatus : 0 --> task is idle, 1 --> task is running */
	UCHAR TxStatus;
#endif /* RALINK_QA */
#ifdef TXBF_SUPPORT
#define MAX_SOUNDING_RESPONSE_SIZE	(57*2*2*9+3+2+6)	/* Assume 114 carriers (40MHz), 3x3, 8bits/coeff, + SNR + HT HEADER + MIMO CONTROL FIELD */
	UCHAR sounding;
	UINT32 sounding_jiffies;
	CHAR soundingSNR[3];
	UINT32 LastRxRate;
	UINT32 LastTxRate;
	UINT32 soundingRespSize;	/* Size of Sounding response */
	UCHAR soundingResp[MAX_SOUNDING_RESPONSE_SIZE];	/* Entire Sounding response */
#endif /* TXBF_SUPPORT */
	RALINK_TIMER_STRUCT PeriodicTimer;
	ULONG OneSecPeriodicRound;
	ULONG PeriodicRound;
} ATE_INFO, *PATE_INFO;

/*
	Use bitmap to allow coexist of ATE_TXFRAME
	and ATE_RXFRAME(i.e.,to support LoopBack mode).
*/
#define fATE_IDLE					0x00
#define fATE_TX_ENABLE				0x01
#define fATE_RX_ENABLE				0x02
#define fATE_TXCONT_ENABLE			0x04
#define fATE_TXCARR_ENABLE			0x08
#define fATE_TXCARRSUPP_ENABLE		0x10
#define fATE_RESERVED_1				0x20
#define fATE_RESERVED_2				0x40
#define fATE_EXIT					0x80

/* Enter/Reset ATE */
#define	ATE_START                   (fATE_IDLE)
/* Stop/Exit ATE */
#define	ATE_STOP                    (fATE_EXIT)
/* Continuous Transmit Frames (without time gap) */
#define	ATE_TXCONT                  ((fATE_TX_ENABLE)|(fATE_TXCONT_ENABLE))
/* Transmit Carrier */
#define	ATE_TXCARR                  ((fATE_TX_ENABLE)|(fATE_TXCARR_ENABLE))
/* Transmit Carrier Suppression (information without carrier) */
#define	ATE_TXCARRSUPP              ((fATE_TX_ENABLE)|(fATE_TXCARRSUPP_ENABLE))
/* Transmit Frames */
#define	ATE_TXFRAME                 (fATE_TX_ENABLE)
/* Receive Frames */
#define	ATE_RXFRAME                 (fATE_RX_ENABLE)


#ifdef RTMP_INTERNAL_TX_ALC
#define EEPROM_TSSI_ENABLE 0x36
#define EEPROM_TSSI_MODE_EXTEND 0x76

#define ATE_MDSM_NORMAL_TX_POWER						0x00
#define ATE_MDSM_DROP_TX_POWER_BY_6dBm					0x01
#define ATE_MDSM_DROP_TX_POWER_BY_12dBm					0x02
#define ATE_MDSM_ADD_TX_POWER_BY_6dBm					0x03
#define ATE_MDSM_BBP_R1_STATIC_TX_POWER_CONTROL_MASK	0x03
#endif /* RTMP_INTERNAL_TX_ALC */

#define	LEN_OF_ARG 16
#define ATE_ON(_p)              (((_p)->ate.Mode) != ATE_STOP)
#define TX_METHOD_0 0 /* Early chipsets must be applied this original TXCONT/TXCARR/TXCARS mechanism. */
#define TX_METHOD_1 1 /* Default TXCONT/TXCARR/TXCARS mechanism is TX_METHOD_1 */
#define ANT_ALL 0
#define ANT_0 1
#define ANT_1 2
#ifdef DOT11N_SS3_SUPPORT
#define ANT_2 3
#endif /* DOT11N_SS3_SUPPORT */

#define ATE_MAC_TX_ENABLE(_A, _I, _pV)	\
{										\
	RTMP_IO_READ32(_A, _I, _pV);		\
	(*(_pV)) |= (1 << 2);				\
	RTMP_IO_WRITE32(_A, _I, (*(_pV)));	\
}

#define ATE_MAC_TX_DISABLE(_A, _I, _pV)	\
{										\
	RTMP_IO_READ32(_A, _I, _pV);		\
	(*(_pV)) &= ~(1 << 2);				\
	RTMP_IO_WRITE32(_A, _I, (*(_pV)));	\
}

#define ATE_MAC_RX_ENABLE(_A, _I, _pV)	\
{										\
	RTMP_IO_READ32(_A, _I, _pV);		\
	(*(_pV)) |= (1 << 3);				\
	RTMP_IO_WRITE32(_A, _I, (*(_pV)));	\
}

#define ATE_MAC_RX_DISABLE(_A, _I, _pV)	\
{										\
	RTMP_IO_READ32(_A, _I, _pV);		\
	(*(_pV)) &= ~(1 << 3);				\
	RTMP_IO_WRITE32(_A, _I, (*(_pV)));	\
}

/* Set MAC_SYS_CTRL(0x1004) Continuous Tx Production Test (bit4) = 1. */
#define ATE_MAC_TX_CTS_ENABLE(_A, _I, _pV)	\
{											\
	RTMP_IO_READ32(_A, _I, _pV);			\
	(*(_pV)) |= (1 << 4);					\
	RTMP_IO_WRITE32(_A, _I, (*(_pV)));		\
}

/* Clear MAC_SYS_CTRL(0x1004) Continuous Tx Production Test (bit4) = 0. */
#define ATE_MAC_TX_CTS_DISABLE(_A, _I, _pV)	\
{											\
	RTMP_IO_READ32(_A, _I, _pV);			\
	(*(_pV)) &= ~(1 << 4);					\
	RTMP_IO_WRITE32(_A, _I, (*(_pV)));		\
}

/* Clear BBP R22 to reset Tx Mode (bit7~bit0) = 0. */
#ifdef RT65xx
#define ATE_BBP_RESET_TX_MODE(_A, _I, _pV)			\
{													\
	UINT32 _bbp_val;\
	RTMP_BBP_IO_READ32(_A, _I, &_bbp_val);	\
	(*(_pV)) &= (0x00);						\
	RTMP_BBP_IO_WRITE32(_A, _I, _bbp_val);	\
}
#else
#define ATE_BBP_RESET_TX_MODE(_A, _I, _pV)			\
{													\
	ATE_BBP_IO_READ8_BY_REG_ID(_A, _I, _pV);		\
	(*(_pV)) &= (0x00);							\
	ATE_BBP_IO_WRITE8_BY_REG_ID(_A, _I, (*(_pV)));	\
}
#endif /* RT65xx */

/* Set BBP R22 to start Continuous Tx Mode (bit7) = 1. */
#define ATE_BBP_START_CTS_TX_MODE(_A, _I, _pV)		\
{													\
	ATE_BBP_IO_READ8_BY_REG_ID(_A, _I, _pV);		\
	(*(_pV)) |= (1 << 7);							\
	ATE_BBP_IO_WRITE8_BY_REG_ID(_A, _I, (*(_pV)));	\
}

/* Clear BBP R22 to stop Continuous Tx Mode (bit7) = 0. */
#define ATE_BBP_STOP_CTS_TX_MODE(_A, _I, _pV)		\
{													\
	ATE_BBP_IO_READ8_BY_REG_ID(_A, _I, _pV);		\
	(*(_pV)) &= ~(1 << 7);							\
	ATE_BBP_IO_WRITE8_BY_REG_ID(_A, _I, (*(_pV)));	\
}

/* Set BBP R24 to send out Continuous Tx sin wave (bit0) = 1. */
#define ATE_BBP_CTS_TX_SIN_WAVE_ENABLE(_A, _I, _pV)	\
{													\
	ATE_BBP_IO_READ8_BY_REG_ID(_A, _I, _pV);		\
	(*(_pV)) |= (1 << 0);							\
	ATE_BBP_IO_WRITE8_BY_REG_ID(_A, _I, (*(_pV)));	\
}

/* Clear BBP R24 to stop Continuous Tx sin wave (bit0) = 0. */
#define ATE_BBP_CTS_TX_SIN_WAVE_DISABLE(_A, _I, _pV)	\
{													\
	ATE_BBP_IO_READ8_BY_REG_ID(_A, _I, _pV);		\
	(*(_pV)) &= ~(1 << 0);							\
	ATE_BBP_IO_WRITE8_BY_REG_ID(_A, _I, (*(_pV)));	\
}

/*
==========================================================================
	Description:
		This routine sets initial value of VGA in the RX chain.
		AGC is the abbreviation of "Automatic Gain Controller",
		while VGA (Variable Gain Amplifier) is a part of AGC loop.
		(i.e., VGA + level detector + feedback loop = AGC)

    Return:
        VOID
==========================================================================
*/
#define ATE_CHIP_RX_VGA_GAIN_INIT(__pAd)								\
	if (__pAd->ate.pChipStruct->RxVGAInit != NULL)	\
		__pAd->ate.pChipStruct->RxVGAInit(__pAd)

#define ATE_CHIP_SET_TX_RX_PATH(__pAd)								\
	if (__pAd->ate.pChipStruct->AsicSetTxRxPath != NULL)	\
		__pAd->ate.pChipStruct->AsicSetTxRxPath(__pAd)



#ifdef RTMP_MAC_USB
#define ATE_BBP_IO_READ8_BY_REG_ID(_A, _I, _pV)    RTMP_BBP_IO_READ8_BY_REG_ID(_A, _I, _pV)
#define ATE_BBP_IO_WRITE8_BY_REG_ID(_A, _I, _V)    RTMP_BBP_IO_WRITE8_BY_REG_ID(_A, _I, _V)

#define BULK_OUT_LOCK(pLock, IrqFlags)	\
		if(1 /*!(in_interrupt() & 0xffff0000)*/)	\
			RTMP_IRQ_LOCK((pLock), IrqFlags);

#define BULK_OUT_UNLOCK(pLock, IrqFlags)	\
		if(1 /*!(in_interrupt() & 0xffff0000)*/)	\
			RTMP_IRQ_UNLOCK((pLock), IrqFlags);

void ATE_RTUSBBulkOutDataPacket(
	IN	struct rtmp_adapter *pAd,
	IN	UCHAR			BulkOutPipeId);

void ATE_RTUSBCancelPendingBulkInIRP(
	IN	struct rtmp_adapter *pAd);

void ATEResetBulkIn(
	IN struct rtmp_adapter *pAd);

INT ATEResetBulkOut(
	IN struct rtmp_adapter *pAd);
#endif /* RTMP_MAC_USB */





#if defined(RT28xx) || defined(RT2880)
void RT28xxATEAsicSwitchChannel(
    IN struct rtmp_adapter *pAd);

INT RT28xxATETxPwrHandler(
	IN struct rtmp_adapter *pAd,
	IN char index);
#endif /* defined(RT28xx) || defined(RT2880) */


#ifdef RALINK_QA
void ATE_QA_Statistics(
	IN struct rtmp_adapter*pAd,
	IN RXWI_STRUC *pRxWI,
	IN RXINFO_STRUC *pRxInfo,
	IN PHEADER_802_11 pHeader);

INT RtmpDoAte(
	IN struct rtmp_adapter*pAd,
	IN RTMP_IOCTL_INPUT_STRUCT *wrq,
	IN char *wrq_name);

INT Set_TxStop_Proc(
	IN struct rtmp_adapter*pAd,
	IN char *arg);

INT Set_RxStop_Proc(
	IN struct rtmp_adapter*pAd,
	IN char *arg);

#ifdef DBG
INT Set_EERead_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_EEWrite_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_BBPRead_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_BBPWrite_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_RFWrite_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);
#endif /* DBG */
#endif /* RALINK_QA */

#ifdef RLT_RF
/* for MT7650 */
#define ATE_RF_IO_READ8_BY_REG_ID(_A, _B, _I, _pV)     rlt_rf_read(_A, _B, _I, _pV)
#define ATE_RF_IO_WRITE8_BY_REG_ID(_A, _B, _I, _V)     rlt_rf_write(_A, _B, _I, _V)
#endif /* RLT_RF */
#ifndef RLT_RF
#define ATE_RF_IO_READ8_BY_REG_ID(_A, _I, _pV)     RT30xxReadRFRegister(_A, _I, _pV)
#define ATE_RF_IO_WRITE8_BY_REG_ID(_A, _I, _V)     RT30xxWriteRFRegister(_A, _I, _V)
#endif /* !RLT_RF */

#ifdef RALINK_QA
#define SYNC_CHANNEL_WITH_QA(_A, _pV)\
	if ((_A->bQATxStart == TRUE) || (_A->bQARxStart == TRUE))\
	{\
		return;\
	}\
	else\
		*_pV = _A->Channel
#else
#define SYNC_CHANNEL_WITH_QA(_A, _pV)\
	*_pV = _A->Channel
#endif /* RALINK_QA */

void rt_ee_read_all(
	IN  struct rtmp_adapter *  pAd,
	OUT USHORT *Data);

void rt_ee_write_all(
	IN  struct rtmp_adapter *  pAd,
	IN  USHORT *Data);

void rt_ee_write_bulk(
	IN  struct rtmp_adapter *pAd,
	IN  USHORT *Data,
	IN  USHORT offset,
	IN  USHORT length);

INT Set_ATE_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ATE_DA_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ATE_SA_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ATE_BSSID_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);


INT	Set_ATE_CHANNEL_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ATE_INIT_CHAN_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_ADCDump_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);
#ifdef RTMP_INTERNAL_TX_ALC
INT Set_ATE_TSSI_CALIBRATION_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_ATE_TSSI_CALIBRATION_EX_Proc(
	IN      struct rtmp_adapter *  pAd,
	IN      char *                arg);


#if defined(RT3350) || defined(RT3352)
INT RT335x_Set_ATE_TSSI_CALIBRATION_ENABLE_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

CHAR InsertTssi(
	IN UCHAR InChannel,
	IN UCHAR Channel0,
	IN UCHAR Channel1,
	IN CHAR Tssi0,
	IN CHAR Tssi1);

INT RT335xATETssiCalibrationExtend(
	IN struct rtmp_adapter		*pAd,
	IN char *				arg);
#endif /* defined(RT3350) || defined(RT3352) */

CHAR ATEGetDesiredTSSI(
	IN struct rtmp_adapter *	pAd);

#endif /* RTMP_INTERNAL_TX_ALC */

#ifdef RTMP_TEMPERATURE_COMPENSATION

INT Set_ATE_TEMP_CAL_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_ATE_SHOW_TSSI_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

#ifdef MT76x0
INT MT76x0_ATETempCalibration(
	IN struct rtmp_adapter		*pAd,
	IN char *				arg);
#endif /* MT76x0 */


INT Set_ATE_READ_EXTERNAL_TSSI_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);
#endif /* RTMP_TEMPERATURE_COMPENSATION */

INT	Set_ATE_TX_POWER0_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ATE_TX_POWER1_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

#ifdef DOT11N_SS3_SUPPORT
INT	Set_ATE_TX_POWER2_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);
#endif /* DOT11N_SS3_SUPPORT */

INT	Set_ATE_TX_Antenna_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ATE_RX_Antenna_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

void DefaultATEAsicExtraPowerOverMAC(
	IN	struct rtmp_adapter *		pAd);

void ATEAsicExtraPowerOverMAC(
	IN	struct rtmp_adapter *pAd);
#ifdef RT3350
INT	Set_ATE_PA_Bias_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);
#endif /* RT3350 */





#if defined(RT28xx) || defined(RT2880)
INT	RT28xx_Set_ATE_TX_FREQ_OFFSET_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);
#endif /* defined(RT28xx) || defined(RT2880) */


INT	Set_ATE_TX_FREQ_OFFSET_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);





#if defined(RT28xx) || defined(RT2880)
INT	RT28xx_Set_ATE_TX_BW_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);
#endif /* defined(RT28xx) || defined(RT2880) */


INT	Set_ATE_TX_BW_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ATE_TX_LENGTH_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ATE_TX_COUNT_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ATE_TX_MCS_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ATE_TX_STBC_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ATE_TX_MODE_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ATE_TX_GI_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);


INT	Set_ATE_RX_FER_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_ATE_Read_RF_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

#if (!defined(RTMP_RF_RW_SUPPORT)) && (!defined(RLT_RF))
INT Set_ATE_Write_RF1_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_ATE_Write_RF2_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_ATE_Write_RF3_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_ATE_Write_RF4_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);
#endif /* (!defined(RTMP_RF_RW_SUPPORT)) && (!defined(RLT_RF)) */

INT Set_ATE_Load_E2P_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_ATE_Read_E2P_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

#ifdef LED_CONTROL_SUPPORT
#endif /* LED_CONTROL_SUPPORT */

INT	Set_ATE_AUTO_ALC_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ATE_IPG_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_ATE_Payload_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *        arg);


#ifdef TXBF_SUPPORT
INT	Set_ATE_TXBF_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ATE_TXSOUNDING_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ATE_TXBF_DIVCAL_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ATE_TXBF_LNACAL_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_ATE_TXBF_INIT_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_ATE_TXBF_CAL_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_ATE_TXBF_GOLDEN_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_ATE_TXBF_VERIFY_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_ATE_TXBF_VERIFY_NoComp_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_ATE_ForceBBP_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);
#endif /* TXBF_SUPPORT */


INT	Set_ATE_Show_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ATE_Help_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);


void ATEAsicAdjustTxPower(
	IN struct rtmp_adapter *pAd);


void ATESampleRssi(
	IN struct rtmp_adapter *pAd,
	IN RXWI_STRUC *pRxWI);


#ifdef RTMP_MAC_USB
INT TxDmaBusy(
	IN struct rtmp_adapter *pAd);

INT RxDmaBusy(
	IN struct rtmp_adapter *pAd);

void RtmpDmaEnable(
	IN struct rtmp_adapter *pAd,
	IN INT Enable);

INT ATESetUpFrame(
	IN struct rtmp_adapter *pAd,
	IN UINT32 TxIdx);

void RTUSBRejectPendingPackets(
	IN	struct rtmp_adapter *pAd);
#endif /* RTMP_MAC_USB */


NDIS_STATUS ChipStructAssign(
 IN	struct rtmp_adapter *pAd);

NDIS_STATUS ATEInit(
 IN struct rtmp_adapter *pAd);

#ifdef RALINK_QA
void ReadQATxTypeFromBBP(
 IN	struct rtmp_adapter *pAd);
#endif /* RALINK_QA */

#ifdef RTMP_MAC
NDIS_STATUS ATEBBPWriteWithRxChain(
 IN struct rtmp_adapter*pAd,
 IN UCHAR bbpId,
 IN CHAR bbpVal,
 IN RX_CHAIN_IDX rx_ch_idx);
#endif /* RTMP_MAC */






#if defined(RT28xx) || defined(RT2880)
void RT28xxATERxVGAInit(
 IN struct rtmp_adapter *	pAd);
#endif /* defined(RT28xx) || defined(RT2880) */


void  ATEPeriodicExec(
	IN void *SystemSpecific1,
	IN void *FunctionContext,
	IN void *SystemSpecific2,
	IN void *SystemSpecific3);

void ATEAsicSetTxRxPath(
    IN struct rtmp_adapter *pAd);

void RtmpRfIoWrite(
	IN struct rtmp_adapter *pAd);

void ATEAsicSwitchChannel(
    IN struct rtmp_adapter *pAd);

void BbpSoftReset(
	IN struct rtmp_adapter *pAd);

#ifdef MT76x0
void mt76x0_ate_adjust_per_rate_pwr(
	IN struct rtmp_adapter *pAd);
#endif /* MT76x0 */

#endif /* __RT_ATE_H__ */

