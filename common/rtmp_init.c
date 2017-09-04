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


#include	"rt_config.h"
#include "bitfield.h"

#define RT3090A_DEFAULT_INTERNAL_LNA_GAIN	0x0A
u8 NUM_BIT8[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
#ifdef DBG
char*   CipherName[] = {"none","wep64","wep128","TKIP","AES","CKIP64","CKIP128","CKIP152","SMS4"};
#endif


/*
	ASIC register initialization sets
*/
struct rtmp_reg_pair MACRegTable[] = {
#if defined(HW_BEACON_OFFSET) && (HW_BEACON_OFFSET == 0x200)
	{BCN_OFFSET0,			0xf8f0e8e0}, /* 0x3800(e0), 0x3A00(e8), 0x3C00(f0), 0x3E00(f8), 512B for each beacon */
	{BCN_OFFSET1,			0x6f77d0c8}, /* 0x3200(c8), 0x3400(d0), 0x1DC0(77), 0x1BC0(6f), 512B for each beacon */
#elif defined(HW_BEACON_OFFSET) && (HW_BEACON_OFFSET == 0x100)
	{BCN_OFFSET0,			0xece8e4e0}, /* 0x3800, 0x3A00, 0x3C00, 0x3E00, 512B for each beacon */
	{BCN_OFFSET1,			0xfcf8f4f0}, /* 0x3800, 0x3A00, 0x3C00, 0x3E00, 512B for each beacon */
#endif /* HW_BEACON_OFFSET */

	{LEGACY_BASIC_RATE,		0x0000013f}, /*  Basic rate set bitmap*/
	{HT_BASIC_RATE,		0x00008003}, /* Basic HT rate set , 20M, MCS=3, MM. Format is the same as in TXWI.*/
	{MAC_SYS_CTRL,		0x00}, /* 0x1004, , default Disable RX*/
	{RX_FILTR_CFG,		0x17f97}, /*0x1400  , RX filter control,  */
	{BKOFF_SLOT_CFG,	0x209}, /* default set short slot time, CC_DELAY_TIME should be 2	 */
	/*{TX_SW_CFG0,		0x40a06},  Gary,2006-08-23 */
	{TX_SW_CFG0,		0x0}, 		/* Gary,2008-05-21 for CWC test */
	{TX_SW_CFG1,		0x80606}, /* Gary,2006-08-23 */
	{TX_LINK_CFG,		0x1020},		/* Gary,2006-08-23 */
	/*{TX_TIMEOUT_CFG,	0x00182090},	 CCK has some problem. So increase timieout value. 2006-10-09 MArvek RT*/
	{TX_TIMEOUT_CFG,	0x000a2090},	/* CCK has some problem. So increase timieout value. 2006-10-09 MArvek RT , Modify for 2860E ,2007-08-01*/
	{MAX_LEN_CFG,		MAX_AGGREGATION_SIZE | 0x00001000},	/* 0x3018, MAX frame length. Max PSDU = 16kbytes.*/
	{LED_CFG,		0x7f031e46}, /* Gary, 2006-08-23*/

	{TX_MAX_PCNT,		0x1fbf1f1f /*0xbfbf3f1f*/},
	{RX_MAX_PCNT,		0x9f},


	/*{TX_RTY_CFG,			0x6bb80408},	 Jan, 2006/11/16*/
/* WMM_ACM_SUPPORT*/
/*	{TX_RTY_CFG,			0x6bb80101},	 sample*/
	{TX_RTY_CFG,			0x47d01f0f},	/* Jan, 2006/11/16, Set TxWI->ACK =0 in Probe Rsp Modify for 2860E ,2007-08-03*/

	{AUTO_RSP_CFG,			0x00000013},	/* Initial Auto_Responder, because QA will turn off Auto-Responder*/
	{CCK_PROT_CFG,			0x05740003 /*0x01740003*/},	/* Initial Auto_Responder, because QA will turn off Auto-Responder. And RTS threshold is enabled. */
	{OFDM_PROT_CFG,			0x05740003 /*0x01740003*/},	/* Initial Auto_Responder, because QA will turn off Auto-Responder. And RTS threshold is enabled. */
	{PBF_CFG, 				0xf40006}, 		/* Only enable Queue 2*/
	{MM40_PROT_CFG,			0x3F44084},		/* Initial Auto_Responder, because QA will turn off Auto-Responder*/
	{WPDMA_GLO_CFG,			0x00000030},
	{GF20_PROT_CFG,			0x01744004},    /* set 19:18 --> Short NAV for MIMO PS*/
	{GF40_PROT_CFG,			0x03F44084},
	{MM20_PROT_CFG,			0x01744004},
	{TXOP_CTRL_CFG,			0x0000583f, /*0x0000243f*/ /*0x000024bf*/},	/*Extension channel backoff.*/
	{TX_RTS_CFG,			0x00092b20},

	{EXP_ACK_TIME,			0x002400ca},	/* default value */
	{TXOP_HLDR_ET, 			0x00000002},

	/* Jerry comments 2008/01/16: we use SIFS = 10us in CCK defaultly, but it seems that 10us
		is too small for INTEL 2200bg card, so in MBSS mode, the delta time between beacon0
		and beacon1 is SIFS (10us), so if INTEL 2200bg card connects to BSS0, the ping
		will always lost. So we change the SIFS of CCK from 10us to 16us. */
	{XIFS_TIME_CFG,			0x33a41010},
	{PWR_PIN_CFG,			0x00000000},
};


#ifdef CONFIG_STA_SUPPORT
struct rtmp_reg_pair STAMACRegTable[] =	{
	{WMM_AIFSN_CFG,	0x00002273},
	{WMM_CWMIN_CFG,	0x00002344},
	{WMM_CWMAX_CFG,	0x000034aa},
};
#endif /* CONFIG_STA_SUPPORT */


#define	NUM_MAC_REG_PARMS		(sizeof(MACRegTable) / sizeof(struct rtmp_reg_pair))
#ifdef CONFIG_STA_SUPPORT
#define	NUM_STA_MAC_REG_PARMS	(sizeof(STAMACRegTable) / sizeof(struct rtmp_reg_pair))
#endif /* CONFIG_STA_SUPPORT */


/*
	Use the global variable is not a good solution.
	But we can not put it to pAd and use the lock in pAd of RALINK_TIMER_STRUCT;
	Or when the structure is cleared, we maybe get NULL for pAd and can not lock.
	Maybe we can put pAd in RTMPSetTimer/ RTMPModTimer/ RTMPCancelTimer.
*/
spinlock_t TimerSemLock;


/*
	========================================================================

	Routine Description:
		Allocate struct rtmp_adapterdata block and do some initialization

	Arguments:
		Adapter		Pointer to our adapter

	Return Value:
		NDIS_STATUS_SUCCESS
		NDIS_STATUS_FAILURE

	IRQL = PASSIVE_LEVEL

	Note:

	========================================================================
*/
int RTMPAllocAdapterBlock(struct os_cookie *handle, struct rtmp_adapter **ppAdapter)
{
	struct rtmp_adapter *pAd = NULL;
	int		Status;
	INT 			index;
	u8 		*pBeaconBuf = NULL;

	DBGPRINT(RT_DEBUG_TRACE, ("--> RTMPAllocAdapterBlock\n"));

	/* init UTIL module */
	RtmpUtilInit();

	*ppAdapter = NULL;

	do {
		/* Allocate struct rtmp_adaptermemory block*/
/*		pBeaconBuf = kmalloc(MAX_BEACON_SIZE, MEM_ALLOC_FLAG);*/
		pBeaconBuf = kmalloc(MAX_BEACON_SIZE, GFP_ATOMIC);
		if (pBeaconBuf == NULL) {
			Status = NDIS_STATUS_FAILURE;
			DBGPRINT_ERR(("Failed to allocate memory - BeaconBuf!\n"));
			break;
		}
		memset(pBeaconBuf, 0, MAX_BEACON_SIZE);

		Status = AdapterBlockAllocateMemory(&pAd, sizeof(struct rtmp_adapter));
		if (Status != NDIS_STATUS_SUCCESS) {
			DBGPRINT_ERR(("Failed to allocate memory - ADAPTER\n"));
			break;
		} else 	{
			/* init resource list (must be after pAd allocation) */
			initList(&pAd->RscTimerMemList);
			initList(&pAd->RscTaskMemList);
			initList(&pAd->RscLockMemList);
			initList(&pAd->RscTaskletMemList);
			initList(&pAd->RscSemMemList);
			initList(&pAd->RscAtomicMemList);

			initList(&pAd->RscTimerCreateList);

			pAd->OS_Cookie = handle;
		}
		pAd->BeaconBuf = pBeaconBuf;
		DBGPRINT(RT_DEBUG_OFF, ("\n\n=== pAd = %p, size = %d ===\n\n", pAd, (u32)sizeof(struct rtmp_adapter)));

		if (RtmpOsStatsAlloc(&pAd->stats, &pAd->iw_stats) == false) {
			Status = NDIS_STATUS_FAILURE;
			break;
		}

		/* Init spin locks*/
		spin_lock_init(&pAd->MgmtRingLock);

		for (index =0 ; index < NUM_OF_TX_RING; index++) {
			spin_lock_init(&pAd->TxSwQueueLock[index]);
			spin_lock_init(&pAd->DeQueueLock[index]);
			pAd->DeQueueRunning[index] = false;
		}

		/*
			move this function from rt28xx_init() to here. now this function only allocate memory and
			leave the initialization job to RTMPInitTxRxRingMemory() which called in rt28xx_init().
		*/
		Status = RTMPAllocTxRxRingMemory(pAd);
		if (Status != NDIS_STATUS_SUCCESS) {
			DBGPRINT_ERR(("Failed to allocate memory - TxRxRing\n"));
			break;
		}

		spin_lock_init(&pAd->irq_lock);


		spin_lock_init(&TimerSemLock);


		/* assign function pointers*/
	} while (false);

	if ((Status != NDIS_STATUS_SUCCESS) && (pBeaconBuf)) {
		kfree(pBeaconBuf);
		pAd->BeaconBuf = NULL;
	}

	if ((Status != NDIS_STATUS_SUCCESS) && (pAd != NULL)) {
		if (pAd->stats != NULL)
			kfree(pAd->stats);

		if (pAd->iw_stats != NULL)
			kfree(pAd->iw_stats);
	}

	if (pAd != NULL) /* compile warning: avoid use NULL pointer when pAd == NULL */
		*ppAdapter = (void *)pAd;


	/*
		Init ProbeRespIE Table
	*/
	for (index = 0; index < MAX_LEN_OF_BSS_TABLE; index++) 	{
		pAd->ProbeRespIE[index].pIe = kmalloc(MAX_VIE_LEN, GFP_ATOMIC);
		if (pAd->ProbeRespIE[index].pIe)
			memset(pAd->ProbeRespIE[index].pIe, 0, MAX_VIE_LEN);
		else
			pAd->ProbeRespIE[index].pIe = NULL;
	}

	DBGPRINT_S(Status, ("<-- RTMPAllocAdapterBlock, Status=%x\n", Status));
	return Status;
}


/*
	========================================================================

	Routine Description:
		Read initial parameters from EEPROM

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:

	========================================================================
*/
void NICReadEEPROMParameters(struct rtmp_adapter*pAd)
{
	u16 i, value, value2;
	EEPROM_TX_PWR_STRUC Power;
	EEPROM_VERSION_STRUC Version;
	EEPROM_ANTENNA_STRUC Antenna;
	EEPROM_NIC_CONFIG0_STRUC NicCfg0;
	u32 reg_val = 0;
	EEPROM_NIC_CONFIG2_STRUC NicConfig2;
	u16  Addr01,Addr23,Addr45 ;
	MAC_DW0_STRUC csr2;
	MAC_DW1_STRUC csr3;


	DBGPRINT(RT_DEBUG_TRACE, ("--> NICReadEEPROMParameters\n"));

	/* Read MAC setting from EEPROM and record as permanent MAC address */
	DBGPRINT(RT_DEBUG_TRACE, ("Initialize MAC Address from E2PROM \n"));

	Addr01 = mt7610u_read_eeprom16(pAd, 0x04);
	Addr23 = mt7610u_read_eeprom16(pAd, 0x06);
	Addr45 = mt7610u_read_eeprom16(pAd, 0x08);

	pAd->PermanentAddress[0] = (u8)(Addr01 & 0xff);
	pAd->PermanentAddress[1] = (u8)(Addr01 >> 8);
	pAd->PermanentAddress[2] = (u8)(Addr23 & 0xff);
	pAd->PermanentAddress[3] = (u8)(Addr23 >> 8);
	pAd->PermanentAddress[4] = (u8)(Addr45 & 0xff);
	pAd->PermanentAddress[5] = (u8)(Addr45 >> 8);

	/*more conveninet to test mbssid, so ap's bssid &0xf1*/
	if (pAd->PermanentAddress[0] == 0xff)
		pAd->PermanentAddress[0] = RandomByte(pAd)&0xf8;

	DBGPRINT(RT_DEBUG_TRACE, ("E2PROM MAC: =%02x:%02x:%02x:%02x:%02x:%02x\n",
								PRINT_MAC(pAd->PermanentAddress)));

	memcpy(pAd->CurrentAddress, pAd->PermanentAddress, ETH_ALEN);
	DBGPRINT(RT_DEBUG_TRACE, ("Use the MAC address what is assigned from EEPROM. \n"));

	/* Set the current MAC to ASIC */
	csr2.field.Byte0 = pAd->CurrentAddress[0];
	csr2.field.Byte1 = pAd->CurrentAddress[1];
	csr2.field.Byte2 = pAd->CurrentAddress[2];
	csr2.field.Byte3 = pAd->CurrentAddress[3];
	mt7610u_write32(pAd, MAC_ADDR_DW0, csr2.word);
	csr3.word = 0;
	csr3.field.Byte4 = pAd->CurrentAddress[4];
	{
		csr3.field.Byte5 = pAd->CurrentAddress[5];
		csr3.field.U2MeMask = 0xff;
	}
	mt7610u_write32(pAd, MAC_ADDR_DW1, csr3.word);

	DBGPRINT_RAW(RT_DEBUG_TRACE,("Current MAC: =%02x:%02x:%02x:%02x:%02x:%02x\n",
					PRINT_MAC(pAd->CurrentAddress)));

	/* if not return early. cause fail at emulation.*/
	/* Init the channel number for TX channel power*/

	MT76x0_ReadChannelPwr(pAd);

	/* if E2PROM version mismatch with driver's expectation, then skip*/
	/* all subsequent E2RPOM retieval and set a system error bit to notify GUI*/
	Version.word = mt7610u_read_eeprom16(pAd, EEPROM_VERSION_OFFSET);
	pAd->EepromVersion = Version.field.Version + Version.field.FaeReleaseNumber * 256;
	DBGPRINT(RT_DEBUG_TRACE, ("E2PROM: Version = %d, FAE release #%d\n", Version.field.Version, Version.field.FaeReleaseNumber));

	if (Version.field.Version > VALID_EEPROM_VERSION)
	{
		DBGPRINT_ERR(("E2PROM: WRONG VERSION 0x%x, should be %d\n",Version.field.Version, VALID_EEPROM_VERSION));
	}

	/* Read BBP default value from EEPROM and store to array(EEPROMDefaultValue) in pAd*/
	value = mt7610u_read_eeprom16(pAd, EEPROM_NIC1_OFFSET);
	pAd->EEPROMDefaultValue[EEPROM_NIC_CFG1_OFFSET] = value;

	value = mt7610u_read_eeprom16(pAd, EEPROM_NIC2_OFFSET);
	pAd->EEPROMDefaultValue[EEPROM_NIC_CFG2_OFFSET] = value;

	{
		value = mt7610u_read_eeprom16(pAd, EEPROM_COUNTRY_REGION);	/* Country Region*/
		pAd->EEPROMDefaultValue[EEPROM_COUNTRY_REG_OFFSET] = value;
	}

	/*
	 * ULLI : Debug output of BT coexistance word,
	 * ULLI : maybe we need this in future
	 */

	value = mt7610u_read_eeprom16(pAd, EEPROM_NIC3_OFFSET);
	DBGPRINT(RT_DEBUG_OFF, ("BT Coexistence word [%d] = %08x\n",
		EEPROM_NIC3_OFFSET, value));


	/* We have to parse NIC configuration 0 at here.*/
	/* If TSSI did not have preloaded value, it should reset the TxAutoAgc to false*/
	/* Therefore, we have to read TxAutoAgc control beforehand.*/
	/* Read Tx AGC control bit*/

	value = mt7610u_read_eeprom16(pAd, 0x24);
	reg_val = mt7610u_read32(pAd, 0x0104);
	DBGPRINT(RT_DEBUG_OFF, ("0x24 = 0x%04x, 0x0104 = 0x%08x\n", value, reg_val));
	NicCfg0.word = pAd->EEPROMDefaultValue[EEPROM_NIC_CFG1_OFFSET];
	DBGPRINT(RT_DEBUG_OFF, ("EEPROM_NIC_CFG1_OFFSET = 0x%04x\n", NicCfg0.word));
	pAd->chipCap.PAType = NicCfg0.field.PAType;
	Antenna.word = 0;
	Antenna.field.TxPath = NicCfg0.field.TxPath;
	Antenna.field.RxPath = NicCfg0.field.RxPath;

	/* ULLI : looks Mediatek wants to reduce streams, needed ?? */

	pAd->CommonCfg.TxStream = Antenna.field.TxPath;
	pAd->CommonCfg.RxStream = Antenna.field.RxPath;

	/* EEPROM offset 0x36 - NIC Configuration 1 */
	NicConfig2.word = pAd->EEPROMDefaultValue[EEPROM_NIC_CFG2_OFFSET];



#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if ((NicConfig2.word & 0x00ff) == 0xff)
			NicConfig2.word &= 0xff00;

		if ((NicConfig2.word >> 8) == 0xff)
			NicConfig2.word &= 0x00ff;
	}
#endif /* CONFIG_STA_SUPPORT */

	if (NicConfig2.field.DynamicTxAgcControl == 1)
		pAd->bAutoTxAgcA = pAd->bAutoTxAgcG = true;
	else
		pAd->bAutoTxAgcA = pAd->bAutoTxAgcG = false;


	/* Save value for future using */
	pAd->NicConfig2.word = NicConfig2.word;

	DBGPRINT_RAW(RT_DEBUG_TRACE, ("NICReadEEPROMParameters: RxPath = %d, TxPath = %d, RfIcType = %d\n",
		Antenna.field.RxPath, Antenna.field.TxPath, Antenna.field.RfIcType));

	/* Save the antenna for future use*/
	pAd->Antenna.word = Antenna.word;

	/* Set the RfICType here, then we can initialize RFIC related operation callbacks*/
	pAd->Mlme.RealRxPath = (u8) Antenna.field.RxPath;

	pAd->RfIcType = (u8) Antenna.field.RfIcType;

	if (IS_MT7650(pAd))
			pAd->RfIcType = RFIC_7650;

	if (IS_MT7630(pAd))
		pAd->RfIcType = RFIC_7630;

	if (IS_MT7610U(pAd))
		pAd->RfIcType = RFIC_7610U;


	/* check if the chip supports 5G band */
	if (WMODE_CAP_5G(pAd->CommonCfg.PhyMode))
	{
		if (!RFIC_IS_5G_BAND(pAd))
		{
			DBGPRINT_RAW(RT_DEBUG_ERROR,
						("%s():Err! chip not support 5G band (%d)!\n",
						__FUNCTION__, pAd->RfIcType));
			/* change to bgn mode */
			Set_WirelessMode_Proc(pAd, "9");
			pAd->RFICType = RFIC_24GHZ;
		}
		pAd->RFICType = RFIC_24GHZ | RFIC_5GHZ;
	}
	else
	{
		pAd->RFICType = RFIC_24GHZ;
	}

	if (IS_MT76x0(pAd)) {
		mt76x0_read_tx_alc_info_from_eeprom(pAd);
	} else
	/* Read TSSI reference and TSSI boundary for temperature compensation. This is ugly*/
	/* 0. 11b/g*/
	{
		/* these are tempature reference value (0x00 ~ 0xFE)
		   ex: 0x00 0x15 0x25 0x45 0x88 0xA0 0xB5 0xD0 0xF0
		   TssiPlusBoundaryG [4] [3] [2] [1] [0] (smaller) +
		   TssiMinusBoundaryG[0] [1] [2] [3] [4] (larger) */
		{
			Power.word = mt7610u_read_eeprom16(pAd, EEPROM_G_TSSI_BOUND1);
			pAd->TssiMinusBoundaryG[4] = Power.field.Byte0;
			pAd->TssiMinusBoundaryG[3] = Power.field.Byte1;
			Power.word = mt7610u_read_eeprom16(pAd, EEPROM_G_TSSI_BOUND2);
			pAd->TssiMinusBoundaryG[2] = Power.field.Byte0;
			pAd->TssiMinusBoundaryG[1] = Power.field.Byte1;
			Power.word = mt7610u_read_eeprom16(pAd, EEPROM_G_TSSI_BOUND3);
			pAd->TssiRefG   = Power.field.Byte0; /* reference value [0] */
			pAd->TssiPlusBoundaryG[1] = Power.field.Byte1;
			Power.word = mt7610u_read_eeprom16(pAd, EEPROM_G_TSSI_BOUND4);
			pAd->TssiPlusBoundaryG[2] = Power.field.Byte0;
			pAd->TssiPlusBoundaryG[3] = Power.field.Byte1;
			Power.word = mt7610u_read_eeprom16(pAd, EEPROM_G_TSSI_BOUND5);
			pAd->TssiPlusBoundaryG[4] = Power.field.Byte0;
			pAd->TxAgcStepG = Power.field.Byte1;
			pAd->TxAgcCompensateG = 0;
			pAd->TssiMinusBoundaryG[0] = pAd->TssiRefG;
			pAd->TssiPlusBoundaryG[0]  = pAd->TssiRefG;

			/* Disable TxAgc if the based value is not right*/
			if (pAd->TssiRefG == 0xff)
				pAd->bAutoTxAgcG = false;
		}

		DBGPRINT(RT_DEBUG_TRACE,("E2PROM: G Tssi[-4 .. +4] = %d %d %d %d - %d -%d %d %d %d, step=%d, tuning=%d\n",
			pAd->TssiMinusBoundaryG[4], pAd->TssiMinusBoundaryG[3], pAd->TssiMinusBoundaryG[2], pAd->TssiMinusBoundaryG[1],
			pAd->TssiRefG,
			pAd->TssiPlusBoundaryG[1], pAd->TssiPlusBoundaryG[2], pAd->TssiPlusBoundaryG[3], pAd->TssiPlusBoundaryG[4],
			pAd->TxAgcStepG, pAd->bAutoTxAgcG));
	}

	if (IS_MT76x0(pAd) || IS_MT76x2(pAd))
	{
		; // TODO: wait TC6008 EEPROM format
	}
	else
	/* 1. 11a*/
	{
		{
		}

#if 0 	/* ULLI : disabled */
		DBGPRINT(RT_DEBUG_TRACE,("E2PROM: A Tssi[-4 .. +4] = %d %d %d %d - %d -%d %d %d %d, step=%d, tuning=%d\n",
			pAd->TssiMinusBoundaryA[4], pAd->TssiMinusBoundaryA[3], pAd->TssiMinusBoundaryA[2], pAd->TssiMinusBoundaryA[1],
			pAd->TssiRefA,
			pAd->TssiPlusBoundaryA[1], pAd->TssiPlusBoundaryA[2], pAd->TssiPlusBoundaryA[3], pAd->TssiPlusBoundaryA[4],
			pAd->TxAgcStepA, pAd->bAutoTxAgcA));
#endif
	}
	pAd->BbpRssiToDbmDelta = 0x0;

	/* Read frequency offset setting for RF*/
		value = mt7610u_read_eeprom16(pAd, EEPROM_FREQ_OFFSET);

	if ((value & 0x00FF) != 0x00FF)
		pAd->RfFreqOffset = (ULONG) (value & 0x00FF);
	else
		pAd->RfFreqOffset = 0;


	DBGPRINT(RT_DEBUG_TRACE, ("E2PROM: RF FreqOffset=0x%lx \n", pAd->RfFreqOffset));

	/*CountryRegion byte offset (38h)*/
	{
		value = pAd->EEPROMDefaultValue[EEPROM_COUNTRY_REG_OFFSET] >> 8;		/* 2.4G band*/
		value2 = pAd->EEPROMDefaultValue[EEPROM_COUNTRY_REG_OFFSET] & 0x00FF;	/* 5G band*/
	}

	if ((value <= REGION_MAXIMUM_BG_BAND) || (value == REGION_32_BG_BAND) || (value == REGION_33_BG_BAND))
	{
		pAd->CommonCfg.CountryRegion = ((u8) value) | 0x80;
	}

	if (value2 <= REGION_MAXIMUM_A_BAND)
	{
		pAd->CommonCfg.CountryRegionForABand = ((u8) value2) | 0x80;
	}

	/* Get RSSI Offset on EEPROM 0x9Ah & 0x9Ch.*/
	/* The valid value are (-10 ~ 10) */
	/* */
	{
		value = mt7610u_read_eeprom16(pAd, EEPROM_RSSI_BG_OFFSET);
		pAd->BGRssiOffset[0] = value & 0x00ff;
		pAd->BGRssiOffset[1] = (value >> 8);
	}

	{
		value = mt7610u_read_eeprom16(pAd, EEPROM_RSSI_BG_OFFSET+2);
		/*
			External LNA gain for 5GHz Band(CH100~CH128)
		*/
		if (IS_MT76x0(pAd))
		{
			pAd->ALNAGain1 = (value >> 8);
		}
		else
		{
/*		if (IS_RT2860(pAd))  RT2860 supports 3 Rx and the 2.4 GHz RSSI #2 offset is in the EEPROM 0x48*/
			pAd->BGRssiOffset[2] = value & 0x00ff;
			pAd->ALNAGain1 = (value >> 8);
		}
	}

	{
		value = mt7610u_read_eeprom16(pAd, EEPROM_LNA_OFFSET);
		pAd->BLNAGain = value & 0x00ff;
		/*
			External LNA gain for 5GHz Band(CH36~CH64)
		*/
		pAd->ALNAGain0 = (value >> 8);
	}


	{
		value = mt7610u_read_eeprom16(pAd, EEPROM_RSSI_A_OFFSET);
		pAd->ARssiOffset[0] = value & 0x00ff;
		pAd->ARssiOffset[1] = (value >> 8);
	}

	{
		value = mt7610u_read_eeprom16(pAd, (EEPROM_RSSI_A_OFFSET+2));
		if (IS_MT76x0(pAd))
		{
			/*
				External LNA gain for 5GHz Band(CH132~CH165)
			*/
			pAd->ALNAGain2 = (value >> 8);
		}
		else
		{
			pAd->ARssiOffset[2] = value & 0x00ff;
			pAd->ALNAGain2 = (value >> 8);
		}
	}


	if (((u8)pAd->ALNAGain1 == 0xFF) || (pAd->ALNAGain1 == 0x00))
		pAd->ALNAGain1 = pAd->ALNAGain0;
	if (((u8)pAd->ALNAGain2 == 0xFF) || (pAd->ALNAGain2 == 0x00))
		pAd->ALNAGain2 = pAd->ALNAGain0;

	DBGPRINT(RT_DEBUG_TRACE, ("ALNAGain0 = %d, ALNAGain1 = %d, ALNAGain2 = %d\n",
					pAd->ALNAGain0, pAd->ALNAGain1, pAd->ALNAGain2));

	/* Validate 11a/b/g RSSI 0/1/2 offset.*/
	for (i =0 ; i < 3; i++)
	{
		if ((pAd->BGRssiOffset[i] < -10) || (pAd->BGRssiOffset[i] > 10))
			pAd->BGRssiOffset[i] = 0;

		if ((pAd->ARssiOffset[i] < -10) || (pAd->ARssiOffset[i] > 10))
			pAd->ARssiOffset[i] = 0;
	}


#ifdef LED_CONTROL_SUPPORT
	/* LED Setting */
	RTMPGetLEDSetting(pAd);
#endif /* LED_CONTROL_SUPPORT */

	if (IS_MT76x0(pAd))
	{
		value = mt7610u_read_eeprom16(pAd, 0xD0);
		value = (value & 0xFF00) >> 8;
		DBGPRINT(RT_DEBUG_TRACE, ("%s: EEPROM_MT76x0_TEMPERATURE_OFFSET = 0x%x\n", __FUNCTION__, value));
		if ((value & 0xFF) == 0xFF)
			pAd->chipCap.TemperatureOffset = -10;
		else
		{
			if ((value & 0x80) == 0x00)
				value &= 0x7F; /* Positive number */
			else
				value |= 0xFF00; /* Negative number */
			pAd->chipCap.TemperatureOffset = value;
		}
		DBGPRINT(RT_DEBUG_TRACE, ("%s: TemperatureOffset = 0x%x\n", __FUNCTION__, pAd->chipCap.TemperatureOffset));

		value = mt7610u_read_eeprom16(pAd, EEPROM_MT76x0_A_BAND_MB);
		pAd->chipCap.a_band_mid_ch = value & 0x00ff;
		if (pAd->chipCap.a_band_mid_ch == 0xFF)
			pAd->chipCap.a_band_mid_ch = 100;
		pAd->chipCap.a_band_high_ch = (value >> 8);
		if (pAd->chipCap.a_band_high_ch == 0xFF)
			pAd->chipCap.a_band_high_ch = 137;
		DBGPRINT(RT_DEBUG_TRACE, ("%s: a_band_mid_ch = %d, a_band_high_ch = %d\n",
			__FUNCTION__, pAd->chipCap.a_band_mid_ch, pAd->chipCap.a_band_high_ch));
	}

	DBGPRINT(RT_DEBUG_TRACE, ("<-- NICReadEEPROMParameters\n"));
}


/*
	========================================================================

	Routine Description:
		Set default value from EEPROM

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:

	========================================================================
*/
void NICInitAsicFromEEPROM(
	IN	struct rtmp_adapter *pAd)
{
#ifdef CONFIG_STA_SUPPORT
	u32 data = 0;
#endif /* CONFIG_STA_SUPPORT */
	USHORT i;
	EEPROM_NIC_CONFIG2_STRUC NicConfig2;

	DBGPRINT(RT_DEBUG_TRACE, ("--> NICInitAsicFromEEPROM\n"));

	NicConfig2.word = pAd->NicConfig2.word;

	if (pAd->chipOps.AsicRfInit)
		pAd->chipOps.AsicRfInit(pAd);


#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* Read Hardware controlled Radio state enable bit*/
		if (NicConfig2.field.HardwareRadioControl == 1)
		{
			bool radioOff = false;
			pAd->StaCfg.bHardwareRadio = true;

			{
				/* Read GPIO pin2 as Hardware controlled radio state*/
				data = mt7610u_read32(pAd, GPIO_CTRL_CFG);
				if ((data & 0x04) == 0)
					radioOff = true;
			}

			if (radioOff)
			{
				pAd->StaCfg.bHwRadio = false;
				pAd->StaCfg.bRadio = false;
/*				mt7610u_write32(pAd, PWR_PIN_CFG, 0x00001818);*/
				RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);
			}
		}
		else
			pAd->StaCfg.bHardwareRadio = false;

#ifdef LED_CONTROL_SUPPORT
		if (pAd->StaCfg.bRadio == false)
		{
			RTMPSetLED(pAd, LED_RADIO_OFF);
		}
		else
		{
			RTMPSetLED(pAd, LED_RADIO_ON);
		}
#endif /* LED_CONTROL_SUPPORT */

	}
#endif /* CONFIG_STA_SUPPORT */
	/* Turn off patching for cardbus controller*/
	if (NicConfig2.field.CardbusAcceleration == 1)
	{
/*		pAd->bTest1 = true;*/
	}

	if (NicConfig2.field.DynamicTxAgcControl == 1)
		pAd->bAutoTxAgcA = pAd->bAutoTxAgcG = true;
	else
		pAd->bAutoTxAgcA = pAd->bAutoTxAgcG = false;

	mt7610u_bbp_set_rxpath(pAd, pAd->Antenna.field.RxPath);

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* Handle the difference when 1T*/
		{
			if(pAd->Antenna.field.TxPath == 1)
				mt7610u_bbp_set_txdac(pAd, 0);
			else
				mt7610u_bbp_set_txdac(pAd, 2);
		}

		DBGPRINT(RT_DEBUG_TRACE, ("Use Hw Radio Control Pin=%d; if used Pin=%d;\n",
					pAd->StaCfg.bHardwareRadio, pAd->StaCfg.bHardwareRadio));
	}
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */
	DBGPRINT(RT_DEBUG_OFF, ("TxPath = %d, RxPath = %d, RFIC=%d\n",
				pAd->Antenna.field.TxPath, pAd->Antenna.field.RxPath, pAd->RfIcType));
	DBGPRINT(RT_DEBUG_TRACE, ("<-- NICInitAsicFromEEPROM\n"));
}


#ifdef DBG
void dump_pdma_reg(struct rtmp_adapter*pAd)
{


}
#endif /* DBG */




void AsicInitBcnBuf(IN struct rtmp_adapter*pAd)
{
	int idx;
	struct rtmp_chip_cap *pChipCap = &pAd->chipCap;


	for (idx = 0; idx < pChipCap->BcnMaxHwNum; idx++)
		pAd->BeaconOffset[idx] = pChipCap->BcnBase[idx];

	DBGPRINT(RT_DEBUG_TRACE, ("< Beacon Information: >\n"));
	DBGPRINT(RT_DEBUG_TRACE, ("\tFlgIsSupSpecBcnBuf = %s\n", pChipCap->FlgIsSupSpecBcnBuf ? "true" : "false"));
	DBGPRINT(RT_DEBUG_TRACE, ("\tBcnMaxHwNum = %d\n", pChipCap->BcnMaxHwNum));
	DBGPRINT(RT_DEBUG_TRACE, ("\tBcnMaxNum = %d\n", pChipCap->BcnMaxNum));
	DBGPRINT(RT_DEBUG_TRACE, ("\tBcnMaxHwSize = 0x%x\n", pChipCap->BcnMaxHwSize));
	DBGPRINT(RT_DEBUG_TRACE, ("\tWcidHwRsvNum = %d\n", pChipCap->WcidHwRsvNum));
	for (idx = 0; idx < pChipCap->BcnMaxHwNum; idx++) {
		DBGPRINT(RT_DEBUG_TRACE, ("\t\tBcnBase[%d] = 0x%x, pAd->BeaconOffset[%d]=0x%x\n",
					idx, pChipCap->BcnBase[idx], idx, pAd->BeaconOffset[idx]));
	}


	{
		struct rtmp_reg_pair bcn_mac_reg_tb[] = {
			{BCN_OFFSET0, 0x18100800},
			{BCN_OFFSET1, 0x38302820},
			{BCN_OFFSET2, 0x58504840},
			{BCN_OFFSET3, 0x78706860},
		};
		for (idx = 0; idx < 4; idx ++)
		{
			mt7610u_write32(pAd, (USHORT)bcn_mac_reg_tb[idx].Register,
									bcn_mac_reg_tb[idx].Value);
		}
	}

}


/*
	========================================================================

	Routine Description:
		Initialize NIC hardware

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:

	========================================================================
*/
int	NICInitializeAdapter(
	IN	struct rtmp_adapter *pAd,
	IN   bool    bHardReset)
{
	int     Status = NDIS_STATUS_SUCCESS;
	ULONG j=0;


	DBGPRINT(RT_DEBUG_TRACE, ("--> NICInitializeAdapter\n"));

	/* Set DMA global configuration except TX_DMA_EN and RX_DMA_EN bits */
retry:

	if (AsicWaitPDMAIdle(pAd, 100, 1000) != true) {
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
			return NDIS_STATUS_FAILURE;
	}



	/* Initialze ASIC for TX & Rx operation*/
	if (NICInitializeAsic(pAd , bHardReset) != NDIS_STATUS_SUCCESS)
	{
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
			return NDIS_STATUS_FAILURE;

		if (j++ == 0) {
			mt7610u_mcu_usb_loadfw(pAd);
			goto retry;
		}
		return NDIS_STATUS_FAILURE;
	}



	DBGPRINT(RT_DEBUG_TRACE, ("<-- NICInitializeAdapter\n"));
	return Status;
}

/*
	========================================================================

	Routine Description:
		Initialize ASIC

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:

	========================================================================
*/
int	NICInitializeAsic(
	IN	struct rtmp_adapter *pAd,
	IN  bool 	bHardReset)
{
	ULONG			Index = 0;
	u32			MACValue = 0;
	u32			Counter = 0;
	u32 val;

	struct rtmp_chip_cap *pChipCap = &pAd->chipCap;

	DBGPRINT(RT_DEBUG_TRACE, ("--> NICInitializeAsic\n"));



	/* Make sure MAC gets ready after NICLoadFirmware().*/

	Index = 0;

	/*To avoid hang-on issue when interface up in kernel 2.4, */
	/*we use a local variable "MacCsr0" instead of using "pAd->MACVersion" directly.*/
	if (WaitForAsicReady(pAd) != true)
			return NDIS_STATUS_FAILURE;

	// TODO: shiang, how about the value setting of pAd->MACVersion?? Original it assigned here

	DBGPRINT(RT_DEBUG_TRACE, ("%s():MACVersion[Ver:Rev=0x%08x]\n",
			__FUNCTION__, pAd->MACVersion));
	/* turn on bit13 (set to zero) after rt2860D. This is to solve high-current issue.*/
	MACValue = mt7610u_read32(pAd, PBF_SYS_CTRL);
	MACValue &= (~0x2000);
	mt7610u_write32(pAd, PBF_SYS_CTRL, MACValue);

	mdelay(200);

	/* for last packet, PBF might use more than limited, so minus 2 to prevent from error */
	val = FIELD_PREP(MT_USB_DMA_CFG_RX_BULK_AGG_LMT, (MAX_RXBULK_SIZE / 1024) - 3) |
	      FIELD_PREP(MT_USB_DMA_CFG_RX_BULK_AGG_TOUT, 0x80) |
	      MT_USB_DMA_CFG_RX_BULK_EN |
	      MT_USB_DMA_CFG_TX_BULK_EN;

	/* USB1.1 do not use bulk in aggregation */
	if (pAd->in_max_packet >= 512)
		val |= MT_USB_DMA_CFG_RX_BULK_AGG_EN;
	else {
		DBGPRINT(RT_DEBUG_OFF, ("disable usb rx aggregagion\n"));
	}

	mt7610u_write32(pAd, USB_DMA_CFG, val);


	/* check MCU if ready */
	MACValue = mt7610u_read32(pAd, COM_REG0);

	val = mt7610u_read32(pAd, USB_DMA_CFG);

	if (IS_MT76x0(pAd)) {
		val |= MT_USB_DMA_CFG_RX_DROP_OR_PAD;
		mt7610u_write32(pAd, USB_DMA_CFG, val);

		val &= ~MT_USB_DMA_CFG_RX_DROP_OR_PAD;
		mt7610u_write32(pAd, USB_DMA_CFG, val);
	} else if (IS_MT76x2(pAd)){
		val |= MT_USB_DMA_CFG_RX_DROP_OR_PAD;
		mt7610u_write32(pAd, USB_DMA_CFG, val);
	}

	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_START_UP);

	mt7610u_write32(pAd, HEADER_TRANS_CTRL_REG, 0x0);
	mt7610u_write32(pAd, TSO_CTRL, 0x0);

	/* Select Q2 to receive command response */
	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD);
	mt7610u_mcu_fun_set(pAd, Q_SELECT, pChipCap->CmdRspRxRing);
	usb_rx_cmd_msgs_receive(pAd);

	//RTUSBBulkReceive(pAd);

	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_START_UP);


	mt7610u_mcu_random_write(pAd, MACRegTable, NUM_MAC_REG_PARMS);



	AsicInitBcnBuf(pAd);

	/* re-set specific MAC registers for individual chip */
	if (pAd->chipOps.AsicMacInit != NULL)
		pAd->chipOps.AsicMacInit(pAd);


	/* Before program BBP, we need to wait BBP/RF get wake up.*/
	Index = 0;
	do
	{
		MACValue = mt7610u_read32(pAd, MAC_STATUS_CFG);

		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
			return NDIS_STATUS_FAILURE;

		if ((MACValue & 0x03) == 0)	/* if BB.RF is stable*/
			break;

		DBGPRINT(RT_DEBUG_TRACE, ("Check MAC_STATUS_CFG  = Busy = %x\n", MACValue));
		RTMPusecDelay(1000);
	} while (Index++ < 100);


	/* The commands to firmware should be after these commands, these commands will init firmware*/
	/* PCI and USB are not the same because PCI driver needs to wait for PCI bus ready*/
		mt7610u_write32(pAd, H2M_BBP_AGENT, 0); /* initialize BBP R/W access agent. */
		mt7610u_write32(pAd,H2M_MAILBOX_CSR,0);
		mt7610u_write32(pAd, H2M_INT_SRC, 0);

	/* Wait to be stable.*/
	RTMPusecDelay(1000);
	pAd->LastMCUCmd = 0x72;

	NICInitBBP(pAd);


	{
		/* enlarge MAX_LEN_CFG*/
		u32 csr;

		csr = mt7610u_read32(pAd, MAX_LEN_CFG);
		csr |= 0x3fff;
		mt7610u_write32(pAd, MAX_LEN_CFG, csr);
	}

{
		u32 MACValue[254 * 2];

		for (Index = 0; Index < 254 * 2; Index += 2)
		{
			MACValue[Index] = 0xffffffff;
			MACValue[Index + 1] = 0x00ffffff;
		}

		mt7610u_mcu_burst_write(pAd, MAC_WCID_BASE, MACValue, 254 * 2);
}

#ifdef CONFIG_STA_SUPPORT
	/* Add radio off control*/
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if (pAd->StaCfg.bRadio == false)
		{
/*			mt7610u_write32(pAd, PWR_PIN_CFG, 0x00001818);*/
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);
			DBGPRINT(RT_DEBUG_TRACE, ("Set Radio Off\n"));
		}
	}
#endif /* CONFIG_STA_SUPPORT */

	/* Clear raw counters*/
	NicResetRawCounters(pAd);

	/* ASIC will keep garbage value after boot*/
	/* Clear all shared key table when initial*/
	/* This routine can be ignored in radio-ON/OFF operation. */
	if (bHardReset)
	{
		{
			u32 MACValue[4];

			for (Index = 0; Index < 4; Index++)
				MACValue[Index] = 0;

			mt7610u_mcu_burst_write(pAd, SHARED_KEY_MODE_BASE, MACValue, 4);
		}

		/* Clear all pairwise key table when initial*/
		{
			u32 MACValue[256];

			for (Index = 0; Index < 256; Index++)
				MACValue[Index] = 1;

			mt7610u_mcu_burst_write(pAd, MAC_WCID_ATTRIBUTE_BASE, MACValue, 256);
		}

	}

	/* It isn't necessary to clear this space when not hard reset. */
	if (bHardReset == true)
	{
		/* clear all on-chip BEACON frame space */
	}

	AsicDisableSync(pAd);

	/* Clear raw counters*/
	NicResetRawCounters(pAd);

	/* Default PCI clock cycle per ms is different as default setting, which is based on PCI.*/
	Counter = mt7610u_read32(pAd, USB_CYC_CFG);
	Counter&=0xffffff00;
	Counter|=0x000001e;
	mt7610u_write32(pAd, USB_CYC_CFG, Counter);
	RTUSBBulkReceive(pAd);

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* for rt2860E and after, init TXOP_CTRL_CFG with 0x583f. This is for extension channel overlapping IOT.*/
		if ((pAd->MACVersion&0xffff) != 0x0101)
			mt7610u_write32(pAd, TXOP_CTRL_CFG, 0x583f);
	}
#endif /* CONFIG_STA_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("<-- NICInitializeAsic\n"));
	return NDIS_STATUS_SUCCESS;
}





void NICUpdateFifoStaCounters(
	IN struct rtmp_adapter *pAd)
{
	TX_STA_FIFO_STRUC	StaFifo;
	MAC_TABLE_ENTRY		*pEntry = NULL;
	u32				i = 0;
	u8 			pid = 0, wcid = 0;
	INT32				reTry;
	u8 			succMCS;

#ifdef CONFIG_STA_SUPPORT
	if(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF))
		return;
#endif /* CONFIG_STA_SUPPORT */


}

/*
	========================================================================

	Routine Description:
		Read Tx statistic raw counters from hardware registers and record to
		related software variables for later on query

	Arguments:
		pAd					Pointer to our adapter
		pStaTxCnt0			Pointer to record "TX_STA_CNT0" (0x170c)
		pStaTxCnt1			Pointer to record "TX_STA_CNT1" (0x1710)

	Return Value:
		None

	========================================================================
*/
void NicGetTxRawCounters(
	IN struct rtmp_adapter*pAd,
	IN TX_STA_CNT0_STRUC *pStaTxCnt0,
	IN TX_STA_CNT1_STRUC *pStaTxCnt1)
{

	pStaTxCnt0->word = mt7610u_read32(pAd, TX_STA_CNT0);
	pStaTxCnt1->word = mt7610u_read32(pAd, TX_STA_CNT1);

	pAd->bUpdateBcnCntDone = true;	/* not appear in Rory's code */
	pAd->RalinkCounters.OneSecBeaconSentCnt += pStaTxCnt0->field.TxBeaconCount;
	pAd->RalinkCounters.OneSecTxRetryOkCount += pStaTxCnt1->field.TxRetransmit;
	pAd->RalinkCounters.OneSecTxNoRetryOkCount += pStaTxCnt1->field.TxSuccess;
	pAd->RalinkCounters.OneSecTxFailCount += pStaTxCnt0->field.TxFailCount;
	pAd->WlanCounters.TransmittedFragmentCount.u.LowPart += pStaTxCnt1->field.TxSuccess;
	pAd->WlanCounters.RetryCount.u.LowPart += pStaTxCnt1->field.TxRetransmit;
	pAd->WlanCounters.FailedCount.u.LowPart += pStaTxCnt0->field.TxFailCount;
}


/*
	========================================================================

	Routine Description:
		Clean all Tx/Rx statistic raw counters from hardware registers

	Arguments:
		pAd					Pointer to our adapter

	Return Value:
		None

	========================================================================
*/
void NicResetRawCounters(struct rtmp_adapter*pAd)
{
	u32 Counter;

	Counter = mt7610u_read32(pAd, RX_STA_CNT0);
	Counter = mt7610u_read32(pAd, RX_STA_CNT1);
	Counter = mt7610u_read32(pAd, RX_STA_CNT2);
	Counter = mt7610u_read32(pAd, TX_STA_CNT0);
	Counter = mt7610u_read32(pAd, TX_STA_CNT1);
	Counter = mt7610u_read32(pAd, TX_STA_CNT2);
}


/*
	========================================================================

	Routine Description:
		Read statistical counters from hardware registers and record them
		in software variables for later on query

	Arguments:
		pAd					Pointer to our adapter

	Return Value:
		None

	IRQL = DISPATCH_LEVEL

	========================================================================
*/
void NICUpdateRawCounters(
	IN struct rtmp_adapter *pAd)
{
	u32	OldValue;/*, Value2;*/
	/*ULONG	PageSum, OneSecTransmitCount;*/
	/*ULONG	TxErrorRatio, Retry, Fail;*/
	RX_STA_CNT0_STRUC	 RxStaCnt0;
	RX_STA_CNT1_STRUC   RxStaCnt1;
	RX_STA_CNT2_STRUC   RxStaCnt2;
	TX_STA_CNT0_STRUC 	 TxStaCnt0;
	TX_STA_CNT1_STRUC	 StaTx1;
	TX_STA_CNT2_STRUC	 StaTx2;
	TX_NAG_AGG_CNT_STRUC	TxAggCnt;
	TX_AGG_CNT0_STRUC	TxAggCnt0;
	TX_AGG_CNT1_STRUC	TxAggCnt1;
	TX_AGG_CNT2_STRUC	TxAggCnt2;
	TX_AGG_CNT3_STRUC	TxAggCnt3;
	TX_AGG_CNT4_STRUC	TxAggCnt4;
	TX_AGG_CNT5_STRUC	TxAggCnt5;
	TX_AGG_CNT6_STRUC	TxAggCnt6;
	TX_AGG_CNT7_STRUC	TxAggCnt7;
	COUNTER_RALINK		*pRalinkCounters;


	pRalinkCounters = &pAd->RalinkCounters;
	if(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF))
		return;

	RxStaCnt0.word = mt7610u_read32(pAd, RX_STA_CNT0);
	RxStaCnt2.word = mt7610u_read32(pAd, RX_STA_CNT2);

	pAd->RalinkCounters.PhyErrCnt += RxStaCnt0.field.PhyErr;
	{
		RxStaCnt1.word = mt7610u_read32(pAd, RX_STA_CNT1);
		pAd->RalinkCounters.PlcpErrCnt += RxStaCnt1.field.PlcpErr;
	    /* Update RX PLCP error counter*/
	    pAd->PrivateInfo.PhyRxErrCnt += RxStaCnt1.field.PlcpErr;
		/* Update False CCA counter*/
		pAd->RalinkCounters.OneSecFalseCCACnt += RxStaCnt1.field.FalseCca;
		pAd->RalinkCounters.FalseCCACnt += RxStaCnt1.field.FalseCca;
	}

	/* Update FCS counters*/
	OldValue= pAd->WlanCounters.FCSErrorCount.u.LowPart;
	pAd->WlanCounters.FCSErrorCount.u.LowPart += (RxStaCnt0.field.CrcErr); /* >> 7);*/
	if (pAd->WlanCounters.FCSErrorCount.u.LowPart < OldValue)
		pAd->WlanCounters.FCSErrorCount.u.HighPart++;

	/* Add FCS error count to private counters*/
	pRalinkCounters->OneSecRxFcsErrCnt += RxStaCnt0.field.CrcErr;
	OldValue = pRalinkCounters->RealFcsErrCount.u.LowPart;
	pRalinkCounters->RealFcsErrCount.u.LowPart += RxStaCnt0.field.CrcErr;
	if (pRalinkCounters->RealFcsErrCount.u.LowPart < OldValue)
		pRalinkCounters->RealFcsErrCount.u.HighPart++;

	/* Update Duplicate Rcv check*/
	pRalinkCounters->DuplicateRcv += RxStaCnt2.field.RxDupliCount;
	pAd->WlanCounters.FrameDuplicateCount.u.LowPart += RxStaCnt2.field.RxDupliCount;
	/* Update RX Overflow counter*/
	pAd->Counters8023.RxNoBuffer += (RxStaCnt2.field.RxFifoOverflowCount);

	/*pAd->RalinkCounters.RxCount = 0;*/
	if (pRalinkCounters->RxCount != pAd->watchDogRxCnt)
	{
		pAd->watchDogRxCnt = pRalinkCounters->RxCount;
		pAd->watchDogRxOverFlowCnt = 0;
	}
	else
	{
		if (RxStaCnt2.field.RxFifoOverflowCount)
			pAd->watchDogRxOverFlowCnt++;
		else
			pAd->watchDogRxOverFlowCnt = 0;
	}


	/*if (!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_TX_RATE_SWITCH_ENABLED) || */
	/*	(OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_TX_RATE_SWITCH_ENABLED) && (pAd->MacTab.Size != 1)))*/
	if (!pAd->bUpdateBcnCntDone)
	{
		/* Update BEACON sent count*/
		NicGetTxRawCounters(pAd, &TxStaCnt0, &StaTx1);
		StaTx2.word = mt7610u_read32(pAd, TX_STA_CNT2);
	}


	/*if (pAd->bStaFifoTest == true)*/
	TxAggCnt.word = mt7610u_read32(pAd, TX_AGG_CNT);
	TxAggCnt0.word = mt7610u_read32(pAd, TX_AGG_CNT0);
	TxAggCnt1.word = mt7610u_read32(pAd, TX_AGG_CNT1);
	TxAggCnt2.word = mt7610u_read32(pAd, TX_AGG_CNT2);
	TxAggCnt3.word = mt7610u_read32(pAd, TX_AGG_CNT3);
	TxAggCnt4.word = mt7610u_read32(pAd, TX_AGG_CNT4);
	TxAggCnt5.word = mt7610u_read32(pAd, TX_AGG_CNT5);
	TxAggCnt6.word = mt7610u_read32(pAd, TX_AGG_CNT6);
	TxAggCnt7.word = mt7610u_read32(pAd, TX_AGG_CNT7);
	pRalinkCounters->TxAggCount += TxAggCnt.field.AggTxCount;
	pRalinkCounters->TxNonAggCount += TxAggCnt.field.NonAggTxCount;
	pRalinkCounters->TxAgg1MPDUCount += TxAggCnt0.field.AggSize1Count;
	pRalinkCounters->TxAgg2MPDUCount += TxAggCnt0.field.AggSize2Count;

	pRalinkCounters->TxAgg3MPDUCount += TxAggCnt1.field.AggSize3Count;
	pRalinkCounters->TxAgg4MPDUCount += TxAggCnt1.field.AggSize4Count;
	pRalinkCounters->TxAgg5MPDUCount += TxAggCnt2.field.AggSize5Count;
	pRalinkCounters->TxAgg6MPDUCount += TxAggCnt2.field.AggSize6Count;

	pRalinkCounters->TxAgg7MPDUCount += TxAggCnt3.field.AggSize7Count;
	pRalinkCounters->TxAgg8MPDUCount += TxAggCnt3.field.AggSize8Count;
	pRalinkCounters->TxAgg9MPDUCount += TxAggCnt4.field.AggSize9Count;
	pRalinkCounters->TxAgg10MPDUCount += TxAggCnt4.field.AggSize10Count;

	pRalinkCounters->TxAgg11MPDUCount += TxAggCnt5.field.AggSize11Count;
	pRalinkCounters->TxAgg12MPDUCount += TxAggCnt5.field.AggSize12Count;
	pRalinkCounters->TxAgg13MPDUCount += TxAggCnt6.field.AggSize13Count;
	pRalinkCounters->TxAgg14MPDUCount += TxAggCnt6.field.AggSize14Count;

	pRalinkCounters->TxAgg15MPDUCount += TxAggCnt7.field.AggSize15Count;
	pRalinkCounters->TxAgg16MPDUCount += TxAggCnt7.field.AggSize16Count;

	/* Calculate the transmitted A-MPDU count*/
	pRalinkCounters->TransmittedAMPDUCount.u.LowPart += TxAggCnt0.field.AggSize1Count;
	pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt0.field.AggSize2Count >> 1);

	pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt1.field.AggSize3Count / 3);
	pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt1.field.AggSize4Count >> 2);

	pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt2.field.AggSize5Count / 5);
	pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt2.field.AggSize6Count / 6);

	pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt3.field.AggSize7Count / 7);
	pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt3.field.AggSize8Count >> 3);

	pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt4.field.AggSize9Count / 9);
	pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt4.field.AggSize10Count / 10);

	pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt5.field.AggSize11Count / 11);
	pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt5.field.AggSize12Count / 12);

	pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt6.field.AggSize13Count / 13);
	pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt6.field.AggSize14Count / 14);

	pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt7.field.AggSize15Count / 15);
	pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt7.field.AggSize16Count >> 4);

#ifdef DBG_DIAGNOSE
	{
		RtmpDiagStruct	*pDiag;
		u8 		ArrayCurIdx, i;

		pDiag = &pAd->DiagStruct;
		ArrayCurIdx = pDiag->ArrayCurIdx;

		if (pDiag->inited == 0)
		{
			memset(pDiag, 0, sizeof(struct _RtmpDiagStrcut_));
			pDiag->ArrayStartIdx = pDiag->ArrayCurIdx = 0;
			pDiag->inited = 1;
		}
		else
		{
			/* Tx*/
			pDiag->TxFailCnt[ArrayCurIdx] = TxStaCnt0.field.TxFailCount;
			pDiag->TxAggCnt[ArrayCurIdx] = TxAggCnt.field.AggTxCount;
			pDiag->TxNonAggCnt[ArrayCurIdx] = TxAggCnt.field.NonAggTxCount;

			pDiag->TxAMPDUCnt[ArrayCurIdx][0] = TxAggCnt0.field.AggSize1Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][1] = TxAggCnt0.field.AggSize2Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][2] = TxAggCnt1.field.AggSize3Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][3] = TxAggCnt1.field.AggSize4Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][4] = TxAggCnt2.field.AggSize5Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][5] = TxAggCnt2.field.AggSize6Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][6] = TxAggCnt3.field.AggSize7Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][7] = TxAggCnt3.field.AggSize8Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][8] = TxAggCnt4.field.AggSize9Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][9] = TxAggCnt4.field.AggSize10Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][10] = TxAggCnt5.field.AggSize11Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][11] = TxAggCnt5.field.AggSize12Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][12] = TxAggCnt6.field.AggSize13Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][13] = TxAggCnt6.field.AggSize14Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][14] = TxAggCnt7.field.AggSize15Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][15] = TxAggCnt7.field.AggSize16Count;

			pDiag->RxCrcErrCnt[ArrayCurIdx] = RxStaCnt0.field.CrcErr;

			INC_RING_INDEX(pDiag->ArrayCurIdx,  DIAGNOSE_TIME);
			ArrayCurIdx = pDiag->ArrayCurIdx;
			for (i =0; i < 9; i++)
			{
				pDiag->TxDescCnt[ArrayCurIdx][i]= 0;
				pDiag->TxSWQueCnt[ArrayCurIdx][i] =0;
				pDiag->TxMcsCnt[ArrayCurIdx][i] = 0;
				pDiag->RxMcsCnt[ArrayCurIdx][i] = 0;
			}
			pDiag->TxDataCnt[ArrayCurIdx] = 0;
			pDiag->TxFailCnt[ArrayCurIdx] = 0;
			pDiag->RxDataCnt[ArrayCurIdx] = 0;
			pDiag->RxCrcErrCnt[ArrayCurIdx]  = 0;
/*			for (i = 9; i < 16; i++)*/
			for (i = 9; i < 24; i++) /* 3*3*/
			{
				pDiag->TxDescCnt[ArrayCurIdx][i] = 0;
				pDiag->TxMcsCnt[ArrayCurIdx][i] = 0;
				pDiag->RxMcsCnt[ArrayCurIdx][i] = 0;
}

			if (pDiag->ArrayCurIdx == pDiag->ArrayStartIdx)
				INC_RING_INDEX(pDiag->ArrayStartIdx,  DIAGNOSE_TIME);
		}

	}
#endif /* DBG_DIAGNOSE */


}


/*
	========================================================================

	Routine Description:
		Compare two memory block

	Arguments:
		pSrc1		Pointer to first memory address
		pSrc2		Pointer to second memory address

	Return Value:
		0:			memory is equal
		1:			pSrc1 memory is larger
		2:			pSrc2 memory is larger

	IRQL = DISPATCH_LEVEL

	Note:

	========================================================================
*/
ULONG	RTMPCompareMemory(
	IN	void *pSrc1,
	IN	void *pSrc2,
	IN	ULONG	Length)
{
	u8 *pMem1;
	u8 *pMem2;
	ULONG	Index = 0;

	pMem1 = (u8 *) pSrc1;
	pMem2 = (u8 *) pSrc2;

	for (Index = 0; Index < Length; Index++)
	{
		if (pMem1[Index] > pMem2[Index])
			return (1);
		else if (pMem1[Index] < pMem2[Index])
			return (2);
	}

	/* Equal*/
	return (0);
}



/*
	========================================================================

	Routine Description:
		Copy data from memory block 1 to memory block 2

	Arguments:
		pDest		Pointer to destination memory address
		pSrc		Pointer to source memory address
		Length		Copy size

	Return Value:
		None

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	Note:

	========================================================================
*/

void UserCfgExit(
	IN struct rtmp_adapter*pAd)
{
	BATableExit(pAd);
}

/*
	========================================================================

	Routine Description:
		Initialize port configuration structure

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:

	========================================================================
*/
void UserCfgInit(struct rtmp_adapter*pAd)
{
	UINT i;
/*	EDCA_PARM DefaultEdcaParm;*/
    UINT key_index, bss_index;
	u8 TXWISize = sizeof(struct txwi_nmac);

	DBGPRINT(RT_DEBUG_TRACE, ("--> UserCfgInit\n"));

	pAd->IndicateMediaState = NdisMediaStateDisconnected;

	/*  part I. intialize common configuration*/
	pAd->CommonCfg.BasicRateBitmap = 0xF;
	pAd->CommonCfg.BasicRateBitmapOld = 0xF;

	pAd->BulkOutReq = 0;

	pAd->BulkOutComplete = 0;
	pAd->BulkOutCompleteOther = 0;
	pAd->BulkOutCompleteCancel = 0;
	pAd->BulkInReq = 0;
	pAd->BulkInComplete = 0;
	pAd->BulkInCompleteFail = 0;

	/*pAd->QuickTimerP = 100;*/
	/*pAd->TurnAggrBulkInCount = 0;*/
	pAd->bUsbTxBulkAggre = 0;

	pAd->CommonCfg.MaxPktOneTxBulk = 2;
	pAd->CommonCfg.TxBulkFactor = 1;
	pAd->CommonCfg.RxBulkFactor =1;

	pAd->CommonCfg.TxPower = 100; /*mW*/

	memset(&pAd->CommonCfg.IOTestParm, 0, sizeof(pAd->CommonCfg.IOTestParm));
#ifdef CONFIG_STA_SUPPORT
	pAd->CountDowntoPsm = 0;
	pAd->StaCfg.Connectinfoflag = false;
#endif /* CONFIG_STA_SUPPORT */

	for(key_index=0; key_index<SHARE_KEY_NUM; key_index++)
	{
		for(bss_index = 0; bss_index < MAX_MBSSID_NUM(pAd) + MAX_P2P_NUM; bss_index++)
		{
			pAd->SharedKey[bss_index][key_index].KeyLen = 0;
			pAd->SharedKey[bss_index][key_index].CipherAlg = CIPHER_NONE;
		}
	}

	pAd->EepromAccess = false;

	pAd->Antenna.word = 0;
	pAd->CommonCfg.BBPCurrentBW = BW_20;


	pAd->bAutoTxAgcA = false;			/* Default is OFF*/
	pAd->bAutoTxAgcG = false;			/* Default is OFF*/

	/* Init timer for reset complete event*/
	pAd->CommonCfg.CentralChannel = 42;
	pAd->bForcePrintTX = false;
	pAd->bForcePrintRX = false;
	pAd->bStaFifoTest = false;
	pAd->bProtectionTest = false;
	pAd->bHCCATest = false;
	pAd->bGenOneHCCA = false;
	pAd->CommonCfg.Dsifs = 10;      /* in units of usec */
	pAd->CommonCfg.TxPower = 100; /* mW*/
	pAd->CommonCfg.TxPowerPercentage = 0xffffffff; /* AUTO*/
	pAd->CommonCfg.TxPowerDefault = 0xffffffff; /* AUTO*/
	pAd->CommonCfg.TxPreamble = Rt802_11PreambleAuto; /* use Long preamble on TX by defaut*/
	pAd->CommonCfg.bUseZeroToDisableFragment = false;
	pAd->CommonCfg.RtsThreshold = 2347;
	pAd->CommonCfg.FragmentThreshold = 2346;
	pAd->CommonCfg.UseBGProtection = 0;    /* 0: AUTO*/
	pAd->CommonCfg.bEnableTxBurst = true; /* 0;    	*/
	pAd->CommonCfg.PhyMode = 0xff;     /* unknown*/
	pAd->CommonCfg.SavedPhyMode = pAd->CommonCfg.PhyMode;

#ifdef DFS_SUPPORT
	pAd->CommonCfg.RadarDetect.bDfsInit = false;
#endif /* DFS_SUPPORT */

	pAd->Dot11_H.ChMovingTime = 65;

	pAd->CommonCfg.bNeedSendTriggerFrame = false;
	pAd->CommonCfg.TriggerTimerCount = 0;
	pAd->CommonCfg.bAPSDForcePowerSave = false;
	/*pAd->CommonCfg.bCountryFlag = false;*/
	pAd->CommonCfg.TxStream = 0;
	pAd->CommonCfg.RxStream = 0;

	memset(&pAd->BeaconTxWI, 0, TXWISize);

	memset(&pAd->CommonCfg.HtCapability, 0, sizeof(pAd->CommonCfg.HtCapability));
	pAd->HTCEnable = false;
	pAd->bBroadComHT = false;
	pAd->CommonCfg.bRdg = false;

	pAd->CommonCfg.Dot11OBssScanPassiveDwell = dot11OBSSScanPassiveDwell;	/* Unit : TU. 5~1000*/
	pAd->CommonCfg.Dot11OBssScanActiveDwell = dot11OBSSScanActiveDwell;	/* Unit : TU. 10~1000*/
	pAd->CommonCfg.Dot11BssWidthTriggerScanInt = dot11BSSWidthTriggerScanInterval;	/* Unit : Second	*/
	pAd->CommonCfg.Dot11OBssScanPassiveTotalPerChannel = dot11OBSSScanPassiveTotalPerChannel;	/* Unit : TU. 200~10000*/
	pAd->CommonCfg.Dot11OBssScanActiveTotalPerChannel = dot11OBSSScanActiveTotalPerChannel;	/* Unit : TU. 20~10000*/
	pAd->CommonCfg.Dot11BssWidthChanTranDelayFactor = dot11BSSWidthChannelTransactionDelayFactor;
	pAd->CommonCfg.Dot11OBssScanActivityThre = dot11BSSScanActivityThreshold;	/* Unit : percentage*/
	pAd->CommonCfg.Dot11BssWidthChanTranDelay = (pAd->CommonCfg.Dot11BssWidthTriggerScanInt * pAd->CommonCfg.Dot11BssWidthChanTranDelayFactor);

	pAd->CommonCfg.bBssCoexEnable = true; /* by default, we enable this feature, you can disable it via the profile or ioctl command*/
	pAd->CommonCfg.BssCoexApCntThr = 0;
	pAd->CommonCfg.Bss2040NeedFallBack = 0;

	memset(&pAd->CommonCfg.AddHTInfo, 0, sizeof(pAd->CommonCfg.AddHTInfo));
	pAd->CommonCfg.BACapability.field.MMPSmode = MMPS_ENABLE;
	pAd->CommonCfg.BACapability.field.MpduDensity = 0;
	pAd->CommonCfg.BACapability.field.Policy = IMMED_BA;
	pAd->CommonCfg.BACapability.field.RxBAWinLimit = 64; /*32;*/
	pAd->CommonCfg.BACapability.field.TxBAWinLimit = 64; /*32;*/
	DBGPRINT(RT_DEBUG_TRACE, ("--> UserCfgInit. BACapability = 0x%x\n", pAd->CommonCfg.BACapability.word));

	pAd->CommonCfg.BACapability.field.AutoBA = false;
	BATableInit(pAd, &pAd->BATable);

	pAd->CommonCfg.bExtChannelSwitchAnnouncement = 1;
	pAd->CommonCfg.bHTProtect = 1;
	pAd->CommonCfg.bMIMOPSEnable = true;
	pAd->CommonCfg.bBADecline = false;
	pAd->CommonCfg.bDisableReordering = false;

	if (pAd->MACVersion == 0x28720200)
		pAd->CommonCfg.TxBASize = 13; /*by Jerry recommend*/
	else
		pAd->CommonCfg.TxBASize = 7;

	pAd->CommonCfg.REGBACapability.word = pAd->CommonCfg.BACapability.word;

	/*pAd->CommonCfg.HTPhyMode.field.BW = BW_20;*/
	/*pAd->CommonCfg.HTPhyMode.field.MCS = MCS_AUTO;*/
	/*pAd->CommonCfg.HTPhyMode.field.ShortGI = GI_800;*/
	/*pAd->CommonCfg.HTPhyMode.field.STBC = STBC_NONE;*/
	pAd->CommonCfg.TxRate = RATE_6;

	pAd->CommonCfg.MlmeTransmit.field.MCS = MCS_RATE_6;
	pAd->CommonCfg.MlmeTransmit.field.BW = BW_20;
	pAd->CommonCfg.MlmeTransmit.field.MODE = MODE_OFDM;

	pAd->CommonCfg.BeaconPeriod = 100;     /* in mSec*/

#ifdef NEW_RATE_ADAPT_SUPPORT
	pAd->CommonCfg.lowTrafficThrd = 2;
	pAd->CommonCfg.TrainUpRule = 2; // 1;
	pAd->CommonCfg.TrainUpRuleRSSI = -70; // 0;
	pAd->CommonCfg.TrainUpLowThrd = 90;
	pAd->CommonCfg.TrainUpHighThrd = 110;
#endif /* NEW_RATE_ADAPT_SUPPORT */



#ifdef CFO_TRACK
#endif /* CFO_TRACK */

#ifdef DBG_CTRL_SUPPORT
	pAd->CommonCfg.DebugFlags = 0;
#endif /* DBG_CTRL_SUPPORT */


#ifdef MCAST_RATE_SPECIFIC
	pAd->CommonCfg.MCastPhyMode.word
	= pAd->MacTab.Content[MCAST_WCID].HTPhyMode.word;
#endif /* MCAST_RATE_SPECIFIC */

	/* WFA policy - disallow TH rate in WEP or TKIP cipher */
	pAd->CommonCfg.HT_DisallowTKIP = true;

	/* Frequency for rate adaptation */
	pAd->ra_interval = DEF_RA_TIME_INTRVAL;
	pAd->ra_fast_interval = DEF_QUICK_RA_TIME_INTERVAL;


	/* Tx Sw queue length setting */
	pAd->TxSwQMaxLen = MAX_PACKETS_IN_QUEUE;

	pAd->CommonCfg.bRalinkBurstMode = false;

#ifdef CONFIG_STA_SUPPORT
	/* part II. intialize STA specific configuration*/
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		RX_FILTER_SET_FLAG(pAd, fRX_FILTER_ACCEPT_DIRECT);
		RX_FILTER_CLEAR_FLAG(pAd, fRX_FILTER_ACCEPT_MULTICAST);
		RX_FILTER_SET_FLAG(pAd, fRX_FILTER_ACCEPT_BROADCAST);
		RX_FILTER_SET_FLAG(pAd, fRX_FILTER_ACCEPT_ALL_MULTICAST);

		pAd->StaCfg.Psm = PWR_ACTIVE;

		pAd->StaCfg.PairCipher = Ndis802_11EncryptionDisabled;
		pAd->StaCfg.GroupCipher = Ndis802_11EncryptionDisabled;
		pAd->StaCfg.bMixCipher = false;
		pAd->StaCfg.DefaultKeyId = 0;

		/* 802.1x port control*/
		pAd->StaCfg.PrivacyFilter = Ndis802_11PrivFilter8021xWEP;
		pAd->StaCfg.PortSecured = WPA_802_1X_PORT_NOT_SECURED;
		pAd->StaCfg.LastMicErrorTime = 0;
		pAd->StaCfg.MicErrCnt        = 0;
		pAd->StaCfg.bBlockAssoc      = false;
		pAd->StaCfg.WpaState         = SS_NOTUSE;

		pAd->CommonCfg.NdisRadioStateOff = false;		/* New to support microsoft disable radio with OID command*/

		pAd->StaCfg.RssiTrigger = 0;
		memset(&pAd->StaCfg.RssiSample, 0, sizeof(RSSI_SAMPLE));
		pAd->StaCfg.RssiTriggerMode = RSSI_TRIGGERED_UPON_BELOW_THRESHOLD;
		pAd->StaCfg.AtimWin = 0;
		pAd->StaCfg.DefaultListenCount = 3;/*default listen count;*/
		pAd->StaCfg.BssType = BSS_INFRA;  /* BSS_INFRA or BSS_ADHOC or BSS_MONITOR*/
		pAd->StaCfg.bSkipAutoScanConn = false;
		OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_DOZE);
		OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_WAKEUP_NOW);

		pAd->StaCfg.bAutoTxRateSwitch = true;
		pAd->StaCfg.DesiredTransmitSetting.field.MCS = MCS_AUTO;
		pAd->StaCfg.bAutoConnectIfNoSSID = false;
	}

#ifdef EXT_BUILD_CHANNEL_LIST
	pAd->StaCfg.IEEE80211dClientMode = Rt802_11_D_None;
#endif /* EXT_BUILD_CHANNEL_LIST */

#endif /* CONFIG_STA_SUPPORT */

	/* global variables mXXXX used in MAC protocol state machines*/
	OPSTATUS_SET_FLAG(pAd, fOP_STATUS_RECEIVE_DTIM);
	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_ADHOC_ON);
	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_INFRA_ON);

	/* PHY specification*/
	pAd->CommonCfg.PhyMode = (WMODE_B | WMODE_G);		/* default PHY mode*/
	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_SHORT_PREAMBLE_INUSED);  /* CCK use LONG preamble*/

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* user desired power mode*/
		pAd->StaCfg.WindowsPowerMode = Ndis802_11PowerModeCAM;
		pAd->StaCfg.WindowsBatteryPowerMode = Ndis802_11PowerModeCAM;
		pAd->StaCfg.bWindowsACCAMEnable = false;

		pAd->StaCfg.bHwRadio  = true; /* Default Hardware Radio status is On*/
		pAd->StaCfg.bSwRadio  = true; /* Default Software Radio status is On*/
		pAd->StaCfg.bRadio    = true; /* bHwRadio && bSwRadio*/
		pAd->StaCfg.bHardwareRadio = false;		/* Default is OFF*/
		pAd->StaCfg.bShowHiddenSSID = false;		/* Default no show*/

		/* Nitro mode control*/

		pAd->StaCfg.LastScanTime = 0;

#ifdef WPA_SUPPLICANT_SUPPORT
		pAd->StaCfg.IEEE8021X = false;
		pAd->StaCfg.IEEE8021x_required_keys = false;
		pAd->StaCfg.WpaSupplicantUP = WPA_SUPPLICANT_DISABLE;
		pAd->StaCfg.bRSN_IE_FromWpaSupplicant = false;
#ifdef NATIVE_WPA_SUPPLICANT_SUPPORT
		pAd->StaCfg.WpaSupplicantUP = WPA_SUPPLICANT_ENABLE;
#endif /* NATIVE_WPA_SUPPLICANT_SUPPORT */
		pAd->StaCfg.bLostAp = false;
		pAd->StaCfg.pWpsProbeReqIe = NULL;
		pAd->StaCfg.WpsProbeReqIeLen = 0;
		pAd->StaCfg.pWpaAssocIe = NULL;
		pAd->StaCfg.WpaAssocIeLen = 0;
		pAd->StaCfg.WpaSupplicantScanCount = 0;
#endif /* WPA_SUPPLICANT_SUPPORT */

		memset(pAd->StaCfg.ReplayCounter, 0, 8);


		pAd->StaCfg.bAutoConnectByBssid = false;
		pAd->StaCfg.BeaconLostTime = BEACON_LOST_TIME;
		memset(pAd->StaCfg.WpaPassPhrase, 0, 64);
		pAd->StaCfg.WpaPassPhraseLen = 0;
		pAd->StaCfg.bAutoRoaming = false;
		pAd->StaCfg.bForceTxBurst = false;
		pAd->StaCfg.bNotFirstScan = false;
		pAd->StaCfg.bImprovedScan = false;
		pAd->StaCfg.bAdhocN = true;
		pAd->StaCfg.bFastConnect = false;
		pAd->StaCfg.bAdhocCreator = false;
	}
#endif /* CONFIG_STA_SUPPORT */

	/* Default for extra information is not valid*/
	pAd->ExtraInfo = EXTRA_INFO_CLEAR;

	/* Default Config change flag*/
	pAd->bConfigChanged = false;

	/* */
	/* part III. AP configurations*/




	/* part IV. others*/

	/* dynamic BBP R66:sensibity tuning to overcome background noise*/
	pAd->BbpTuning.bEnable                = true;
	pAd->BbpTuning.FalseCcaLowerThreshold = 100;
	pAd->BbpTuning.FalseCcaUpperThreshold = 512;
	pAd->BbpTuning.R66Delta               = 4;
	pAd->Mlme.bEnableAutoAntennaCheck = true;


	/* Also initial R66CurrentValue, RTUSBResumeMsduTransmission might use this value.*/
	/* if not initial this value, the default value will be 0.*/

	pAd->BbpTuning.R66CurrentValue = 0x38;

	pAd->Bbp94 = BBPR94_DEFAULT;
	pAd->BbpForCCK = false;

	/* Default is false for test bit 1*/
	/*pAd->bTest1 = false;*/

	/* initialize MAC table and allocate spin lock*/
	memset(&pAd->MacTab, 0, sizeof(MAC_TABLE));
	InitializeQueueHeader(&pAd->MacTab.McastPsQueue);
	spin_lock_init(&pAd->MacTabLock);

	/*RTMPInitTimer(pAd, &pAd->RECBATimer, RECBATimerTimeout, pAd, true);*/
	/*RTMPSetTimer(&pAd->RECBATimer, REORDER_EXEC_INTV);*/


	pAd->RxAnt.Pair1PrimaryRxAnt = 0;
	pAd->RxAnt.Pair1SecondaryRxAnt = 1;

		pAd->RxAnt.EvaluatePeriod = 0;
		pAd->RxAnt.RcvPktNumWhenEvaluate = 0;
#ifdef CONFIG_STA_SUPPORT
		pAd->RxAnt.Pair1AvgRssi[0] = pAd->RxAnt.Pair1AvgRssi[1] = 0;
#endif /* CONFIG_STA_SUPPORT */

#if defined(AP_SCAN_SUPPORT) || defined(CONFIG_STA_SUPPORT)
	for (i = 0; i < MAX_LEN_OF_BSS_TABLE; i++)
	{
		PBSS_ENTRY	pBssEntry = &pAd->ScanTab.BssEntry[i];

		if (pAd->ProbeRespIE[i].pIe)
			pBssEntry->pVarIeFromProbRsp = pAd->ProbeRespIE[i].pIe;
		else
			pBssEntry->pVarIeFromProbRsp = NULL;
	}
#endif /* defined(AP_SCAN_SUPPORT) || defined(CONFIG_STA_SUPPORT) */

	/* 802.11H and DFS related params*/
	pAd->Dot11_H.CSCount = 0;
	pAd->Dot11_H.CSPeriod = 10;
	pAd->Dot11_H.RDMode = RD_NORMAL_MODE;
	pAd->Dot11_H.bDFSIndoor = 1;

	pAd->chipCap.LastTemperatureforVCO = 0x7FFF;
	pAd->chipCap.LastTemperatureforCal = 0x7FFF;
	pAd->chipCap.NowTemperature = 0x7FFF;

	DBGPRINT(RT_DEBUG_TRACE, ("<-- UserCfgInit\n"));
}

/* IRQL = PASSIVE_LEVEL*/
u8 BtoH(STRING ch)
{
	if (ch >= '0' && ch <= '9') return (ch - '0');        /* Handle numerals*/
	if (ch >= 'A' && ch <= 'F') return (ch - 'A' + 0xA);  /* Handle capitol hex digits*/
	if (ch >= 'a' && ch <= 'f') return (ch - 'a' + 0xA);  /* Handle small hex digits*/
	return(255);
}


/*  FUNCTION: AtoH(char *, u8 *, int)*/

/*  PURPOSE:  Converts ascii string to network order hex*/

/*  PARAMETERS:*/
/*    src    - pointer to input ascii string*/
/*    dest   - pointer to output hex*/
/*    destlen - size of dest*/

/*  COMMENTS:*/

/*    2 ascii bytes make a hex byte so must put 1st ascii byte of pair*/
/*    into upper nibble and 2nd ascii byte of pair into lower nibble.*/

/* IRQL = PASSIVE_LEVEL*/

void AtoH(char *src, u8 *dest, int destlen)
{
	char *srcptr;
	u8 *destTemp;

	srcptr = src;
	destTemp = (u8 *) dest;

	while(destlen--)
	{
		*destTemp = BtoH(*srcptr++) << 4;    /* Put 1st ascii byte in upper nibble.*/
		*destTemp += BtoH(*srcptr++);      /* Add 2nd ascii byte to above.*/
		destTemp++;
	}
}


/*
========================================================================
Routine Description:
	Add a timer to the timer list.

Arguments:
	pAd				- WLAN control block pointer
	pRsc			- the OS resource

Return Value:
	None

Note:
========================================================================
*/
void RTMP_TimerListAdd(
	IN	struct rtmp_adapter *		pAd,
	IN	void 				*pRsc)
{
	LIST_HEADER *pRscList = &pAd->RscTimerCreateList;
	LIST_RESOURCE_OBJ_ENTRY *pObj;


	/* try to find old entry */
	pObj = (LIST_RESOURCE_OBJ_ENTRY *)(pRscList->pHead);
	while(1)
	{
		if (pObj == NULL)
			break;
		if ((ULONG)(pObj->pRscObj) == (ULONG)pRsc)
			return; /* exists */
		pObj = pObj->pNext;
	}

	/* allocate a timer record entry */
	pObj = kmalloc(sizeof(LIST_RESOURCE_OBJ_ENTRY), GFP_ATOMIC);
	if (!pObj) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s: alloc timer obj fail!\n", __FUNCTION__));
		return;
	} else {
		pObj->pRscObj = pRsc;
		insertTailList(pRscList, (LIST_ENTRY *)pObj);
		DBGPRINT(RT_DEBUG_ERROR, ("%s: add timer obj %lx!\n", __FUNCTION__, (ULONG)pRsc));
	}
}


/*
========================================================================
Routine Description:
	Cancel all timers in the timer list.

Arguments:
	pAd				- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
void RTMP_TimerListRelease(
	IN	struct rtmp_adapter *		pAd)
{
	LIST_HEADER *pRscList = &pAd->RscTimerCreateList;
	LIST_RESOURCE_OBJ_ENTRY *pObj, *pObjOld;
	bool Cancel;


	/* try to find old entry */
	pObj = (LIST_RESOURCE_OBJ_ENTRY *)(pRscList->pHead);
	while(1)
	{
		if (pObj == NULL)
			break;
		DBGPRINT(RT_DEBUG_TRACE, ("%s: Cancel timer obj %lx!\n", __FUNCTION__, (ULONG)(pObj->pRscObj)));
		RTMPReleaseTimer(pObj->pRscObj, &Cancel);
		pObjOld = pObj;
		pObj = pObj->pNext;
		kfree(pObjOld);
	}

	/* reset TimerList */
	initList(&pAd->RscTimerCreateList);
}


/*
	========================================================================

	Routine Description:
		Init timer objects

	Arguments:
		pAd			Pointer to our adapter
		pTimer				Timer structure
		pTimerFunc			Function to execute when timer expired
		Repeat				Ture for period timer

	Return Value:
		None

	Note:

	========================================================================
*/
void RTMPInitTimer(
	IN	struct rtmp_adapter *		pAd,
	IN	PRALINK_TIMER_STRUCT	pTimer,
	IN	void *				pTimerFunc,
	IN	void *				pData,
	IN	bool 				Repeat)
{
	RTMP_SEM_LOCK(&TimerSemLock);

	RTMP_TimerListAdd(pAd, pTimer);


	/* Set Valid to true for later used.*/
	/* It will crash if we cancel a timer or set a timer */
	/* that we haven't initialize before.*/
	/* */
	pTimer->Valid      = true;

	pTimer->PeriodicType = Repeat;
	pTimer->State      = false;
	pTimer->cookie = (ULONG) pData;
	pTimer->pAd = pAd;

	RTMP_OS_Init_Timer(&pTimer->TimerObj,	pTimerFunc, (void *) pTimer, &pAd->RscTimerMemList);

	RTMP_SEM_UNLOCK(&TimerSemLock);
}


/*
	========================================================================

	Routine Description:
		Init timer objects

	Arguments:
		pTimer				Timer structure
		Value				Timer value in milliseconds

	Return Value:
		None

	Note:
		To use this routine, must call RTMPInitTimer before.

	========================================================================
*/
void RTMPSetTimer(
	IN	PRALINK_TIMER_STRUCT	pTimer,
	IN	ULONG					Value)
{
	RTMP_SEM_LOCK(&TimerSemLock);

	if (pTimer->Valid)
	{
		struct rtmp_adapter*pAd;

		pAd = (struct rtmp_adapter*)pTimer->pAd;
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
		{
			DBGPRINT_ERR(("RTMPSetTimer failed, Halt in Progress!\n"));
			RTMP_SEM_UNLOCK(&TimerSemLock);
			return;
		}

		pTimer->TimerValue = Value;
		pTimer->State      = false;
		if (pTimer->PeriodicType == true)
		{
			pTimer->Repeat = true;
			RTMP_SetPeriodicTimer(&pTimer->TimerObj, Value);
		}
		else
		{
			pTimer->Repeat = false;
			RTMP_OS_Add_Timer(&pTimer->TimerObj, Value);
		}

		DBGPRINT(RT_DEBUG_INFO,("%s: %lx\n",__FUNCTION__, (ULONG)pTimer));
	}
	else
	{
		DBGPRINT_ERR(("RTMPSetTimer failed, Timer hasn't been initialize!\n"));
	}
	RTMP_SEM_UNLOCK(&TimerSemLock);
}


/*
	========================================================================

	Routine Description:
		Init timer objects

	Arguments:
		pTimer				Timer structure
		Value				Timer value in milliseconds

	Return Value:
		None

	Note:
		To use this routine, must call RTMPInitTimer before.

	========================================================================
*/
void RTMPModTimer(
	IN	PRALINK_TIMER_STRUCT	pTimer,
	IN	ULONG					Value)
{
	bool Cancel;


	RTMP_SEM_LOCK(&TimerSemLock);

	if (pTimer->Valid)
	{
		pTimer->TimerValue = Value;
		pTimer->State      = false;
		if (pTimer->PeriodicType == true)
		{
			RTMP_SEM_UNLOCK(&TimerSemLock);
			RTMPCancelTimer(pTimer, &Cancel);
			RTMPSetTimer(pTimer, Value);
		}
		else
		{
			RTMP_OS_Mod_Timer(&pTimer->TimerObj, Value);
			RTMP_SEM_UNLOCK(&TimerSemLock);
		}
		DBGPRINT(RT_DEBUG_TRACE,("%s: %lx\n",__FUNCTION__, (ULONG)pTimer));
	}
	else
	{
		DBGPRINT_ERR(("RTMPModTimer failed, Timer hasn't been initialize!\n"));
		RTMP_SEM_UNLOCK(&TimerSemLock);
	}
}


/*
	========================================================================

	Routine Description:
		Cancel timer objects

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	Note:
		1.) To use this routine, must call RTMPInitTimer before.
		2.) Reset NIC to initial state AS IS system boot up time.

	========================================================================
*/
void RTMPCancelTimer(
	IN	PRALINK_TIMER_STRUCT	pTimer,
	OUT	bool 				*pCancelled)
{
	RTMP_SEM_LOCK(&TimerSemLock);

	if (pTimer->Valid)
	{
		if (pTimer->State == false)
			pTimer->Repeat = false;

		RTMP_OS_Del_Timer(&pTimer->TimerObj, pCancelled);

		if (*pCancelled == true)
			pTimer->State = true;

#ifdef RTMP_TIMER_TASK_SUPPORT
		/* We need to go-through the TimerQ to findout this timer handler and remove it if */
		/*		it's still waiting for execution.*/
		RtmpTimerQRemove(pTimer->pAd, pTimer);
#endif /* RTMP_TIMER_TASK_SUPPORT */

		DBGPRINT(RT_DEBUG_INFO,("%s: %lx\n",__FUNCTION__, (ULONG)pTimer));
	}
	else
	{
		DBGPRINT(RT_DEBUG_INFO,("RTMPCancelTimer failed, Timer hasn't been initialize!\n"));
	}

	RTMP_SEM_UNLOCK(&TimerSemLock);
}


void RTMPReleaseTimer(
	IN	PRALINK_TIMER_STRUCT	pTimer,
	OUT	bool 				*pCancelled)
{
	RTMP_SEM_LOCK(&TimerSemLock);

	if (pTimer->Valid)
	{
		if (pTimer->State == false)
			pTimer->Repeat = false;

		RTMP_OS_Del_Timer(&pTimer->TimerObj, pCancelled);

		if (*pCancelled == true)
			pTimer->State = true;

#ifdef RTMP_TIMER_TASK_SUPPORT
		/* We need to go-through the TimerQ to findout this timer handler and remove it if */
		/*		it's still waiting for execution.*/
		RtmpTimerQRemove(pTimer->pAd, pTimer);
#endif /* RTMP_TIMER_TASK_SUPPORT */

		/* release timer */
		RTMP_OS_Release_Timer(&pTimer->TimerObj);

		pTimer->Valid = false;

		DBGPRINT(RT_DEBUG_INFO,("%s: %lx\n",__FUNCTION__, (ULONG)pTimer));
	}
	else
	{
		DBGPRINT(RT_DEBUG_INFO,("RTMPReleasefailed, Timer hasn't been initialize!\n"));
	}

	RTMP_SEM_UNLOCK(&TimerSemLock);
}


/*
	========================================================================

	Routine Description:
		Enable RX

	Arguments:
		pAd						Pointer to our adapter

	Return Value:
		None

	IRQL <= DISPATCH_LEVEL

	Note:
		Before Enable RX, make sure you have enabled Interrupt.
	========================================================================
*/
void RTMPEnableRxTx(
	IN struct rtmp_adapter *pAd)
{
	u32 rx_filter_flag;

	DBGPRINT(RT_DEBUG_TRACE, ("==> RTMPEnableRxTx\n"));

	RT28XXDMAEnable(pAd);

	/* enable RX of MAC block*/
	if (pAd->OpMode == OPMODE_AP)
	{
		rx_filter_flag = APNORMAL;


		mt7610u_write32(pAd, RX_FILTR_CFG, rx_filter_flag);     /* enable RX of DMA block*/
	}
#ifdef CONFIG_STA_SUPPORT
	else
	{
			rx_filter_flag = STANORMAL;     /* Staion not drop control frame will fail WiFi Certification.*/
		mt7610u_write32(pAd, RX_FILTR_CFG, rx_filter_flag);
	}
#endif /* CONFIG_STA_SUPPORT */

	{
		mt7610u_write32(pAd, MAC_SYS_CTRL, 0xc);
//+++Add by shiang for debug for pbf_loopback
//			mt7610u_write32(pAd, MAC_SYS_CTRL, 0x2c);
//---Add by shiang for debug
//+++Add by shiang for debug invalid RxWI->WCID
//---Add by shiang for  debug invalid RxWI->WCID



	}

	DBGPRINT(RT_DEBUG_TRACE, ("<== RTMPEnableRxTx\n"));
}


void CfgInitHook(struct rtmp_adapter *pAd)
{
	/*pAd->bBroadComHT = true;*/
}


static void RtmpChipOpsRegister(struct rtmp_adapter*pAd)
{
	struct rtmp_chip_ops  *pChipOps = &pAd->chipOps;
	struct rtmp_chip_cap *pChipCap = &pAd->chipCap;

	memset(pChipOps, 0, sizeof(*pChipOps));
	memset(pChipCap, 0, sizeof(*pChipCap));

	RtmpChipOpsHook(pAd);
}

INT RtmpRaDevCtrlInit(struct rtmp_adapter *pAd)
{
	u8 i;
	int ret = 0;

#ifdef CONFIG_STA_SUPPORT
	pAd->OpMode = OPMODE_STA;
#endif /* CONFIG_STA_SUPPORT */

	sema_init(&(pAd->UsbVendorReq_semaphore), 1);
	sema_init(&(pAd->reg_atomic), 1);
	sema_init(&(pAd->hw_atomic), 1);
	sema_init(&(pAd->cal_atomic), 1);
	sema_init(&(pAd->wlan_en_atomic), 1);
	sema_init(&(pAd->mcu_atomic), 1);
	pAd->vend_buf = kmalloc(MAX_PARAM_BUFFER_SIZE - 1, GFP_ATOMIC);
	if (pAd->vend_buf == NULL) {
		DBGPRINT(RT_DEBUG_ERROR, ("Allocate vendor request temp buffer failed!\n"));
		return false;
	}

		mt7610u_chip_onoff(pAd, true, false);

	RtmpChipOpsRegister(pAd);

#ifdef MCS_LUT_SUPPORT
	if (pAd->chipCap.asic_caps & fASIC_CAP_MCS_LUT) {
		if (MAX_LEN_OF_MAC_TABLE < 128) {
			RTMP_SET_MORE_FLAG(pAd, fASIC_CAP_MCS_LUT);
		} else {
			DBGPRINT(RT_DEBUG_WARN, ("%s(): MCS_LUT not used becasue MacTb size(%d) > 128!\n",
						__FUNCTION__, MAX_LEN_OF_MAC_TABLE));
		}
	}
#endif /* MCS_LUT_SUPPORT */

	return 0;
}


bool RtmpRaDevCtrlExit(struct rtmp_adapter *pAd)
{
	INT index;

	if ((IS_MT76x0(pAd) || IS_MT76x2(pAd))&& (pAd->WlanFunCtrl.field.WLAN_EN == 1)) {
		mt7610u_chip_onoff(pAd, false, false);
	}

	kfree(pAd->vend_buf);
	pAd->vend_buf = NULL;

	/*
		Free ProbeRespIE Table
	*/
	for (index = 0; index < MAX_LEN_OF_BSS_TABLE; index++) {
		if (pAd->ProbeRespIE[index].pIe)
			kfree(pAd->ProbeRespIE[index].pIe);
	}

	RTMPFreeTxRxRingMemory(pAd);

	RTMPFreeAdapter(pAd);

	return true;
}
