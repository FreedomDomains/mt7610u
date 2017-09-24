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


#include "rt_config.h"


/*
	========================================================================

	Routine Description:
		Calculates the duration which is required to transmit out frames
	with given size and specified rate.

	Arguments:
		pTxWI		Pointer to head of each MPDU to HW.
		Ack 		Setting for Ack requirement bit
		Fragment	Setting for Fragment bit
		RetryMode	Setting for retry mode
		Ifs 		Setting for IFS gap
		Rate		Setting for transmit rate
		Service 	Setting for service
		Length		Frame length
		TxPreamble	Short or Long preamble when using CCK rates
		QueIdx - 0-3, according to 802.11e/d4.4 June/2003

	Return Value:
		None

	See also : BASmartHardTransmit()    !!!

	========================================================================
*/
void RTMPWriteTxWI(
	IN struct rtmp_adapter*pAd,
	IN struct mt7610u_txwi *pOutTxWI,
	IN bool FRAG,
	IN bool CFACK,
	IN bool InsTimestamp,
	IN bool AMPDU,
	IN bool Ack,
	IN bool NSeq,		/* HW new a sequence.*/
	IN u8 BASize,
	IN u8 WCID,
	IN ULONG Length,
	IN u8 PID,
	IN u8 TID,
	IN u8 TxRate,
	IN u8 Txopmode,
	IN bool CfAck,
	IN HTTRANSMIT_SETTING *pTransmit)
{
	PMAC_TABLE_ENTRY pMac = NULL;
	struct mt7610u_txwi TxWI, *pTxWI;
	u8 TXWISize = sizeof(struct mt7610u_txwi);

	if (WCID < MAX_LEN_OF_MAC_TABLE)
		pMac = &pAd->MacTab.Content[WCID];


	/*
		Always use Long preamble before verifiation short preamble functionality works well.
		Todo: remove the following line if short preamble functionality works
	*/
	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_SHORT_PREAMBLE_INUSED);
	memset(&TxWI, 0, TXWISize);
	pTxWI = &TxWI;
	pTxWI->FRAG= FRAG;
	pTxWI->CFACK = CFACK;
	pTxWI->TS= InsTimestamp;
	pTxWI->AMPDU = AMPDU;
	pTxWI->ACK = Ack;
	pTxWI->txop= Txopmode;

	pTxWI->NSEQ = NSeq;
	/* John tune the performace with Intel Client in 20 MHz performance*/
	BASize = pAd->CommonCfg.TxBASize;
	if (BASize > 31)
		BASize =31;

	pTxWI->BAWinSize = BASize;
	pTxWI->ShortGI = pTransmit->field.ShortGI;
	pTxWI->STBC = pTransmit->field.STBC;


	pTxWI->wcid = WCID;
	pTxWI->MPDUtotalByteCnt = Length;
	pTxWI->TxPktId = PID;

	/* If CCK or OFDM, BW must be 20*/
	pTxWI->BW = (pTransmit->field.MODE <= MODE_OFDM) ? (BW_20) : (pTransmit->field.BW);
	if (pTxWI->BW)
		pTxWI->BW = (pAd->CommonCfg.AddHTInfo.AddHtInfo.RecomWidth == 0) ? (BW_20) : (pTransmit->field.BW);

	pTxWI->MCS = pTransmit->field.MCS;
	pTxWI->PHYMODE = pTransmit->field.MODE;
	pTxWI->CFACK = CfAck;

	if (pMac)
	{
        if (pAd->CommonCfg.bMIMOPSEnable)
        {
    		if ((pMac->MmpsMode == MMPS_DYNAMIC) && (pTransmit->field.MCS > 7))
			{
				/* Dynamic MIMO Power Save Mode*/
				pTxWI->MIMOps = 1;
			}
			else if (pMac->MmpsMode == MMPS_STATIC)
			{
				/* Static MIMO Power Save Mode*/
				if (pTransmit->field.MODE >= MODE_HTMIX && pTransmit->field.MCS > 7)
				{
					pTxWI->MCS = 7;
					pTxWI->MIMOps = 0;
				}
			}
        }
		/*pTxWI->MIMOps = (pMac->PsMode == PWR_MMPS)? 1:0;*/
		{
			pTxWI->MpduDensity = pMac->MpduDensity;
		}
	}


	pTxWI->TxPktId = pTxWI->MCS;
	memmove(pOutTxWI, &TxWI, TXWISize);
//---Add by shiang for debug
}


void RTMPWriteTxWI_Data(struct rtmp_adapter*pAd, struct mt7610u_txwi *pTxWI, TX_BLK *pTxBlk)
{
	HTTRANSMIT_SETTING *pTransmit;
	MAC_TABLE_ENTRY *pMacEntry;
	u8 BASize;
	u8 TXWISize = sizeof(struct mt7610u_txwi);


	ASSERT(pTxWI);

	pTransmit = pTxBlk->pTransmit;
	pMacEntry = pTxBlk->pMacEntry;

	/*
		Always use Long preamble before verifiation short preamble functionality works well.
		Todo: remove the following line if short preamble functionality works
	*/
	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_SHORT_PREAMBLE_INUSED);
	memset(pTxWI, 0, TXWISize);

	pTxWI->FRAG = TX_BLK_TEST_FLAG(pTxBlk, fTX_bAllowFrag);
	pTxWI->ACK = TX_BLK_TEST_FLAG(pTxBlk, fTX_bAckRequired);
	pTxWI->txop = pTxBlk->FrameGap;

#ifdef CONFIG_STA_SUPPORT
#ifdef QOS_DLS_SUPPORT
	if (pMacEntry && IS_ENTRY_DLS(pMacEntry) &&
		(pAd->StaCfg.BssType == BSS_INFRA))
		pTxWI->wcid = BSSID_WCID;
	else
#endif /* QOS_DLS_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */
	pTxWI->wcid = pTxBlk->Wcid;

	pTxWI->MPDUtotalByteCnt = pTxBlk->MpduHeaderLen + pTxBlk->SrcBufLen;
	pTxWI->CFACK = TX_BLK_TEST_FLAG(pTxBlk, fTX_bPiggyBack);

#ifdef WFA_VHT_PF
	if (pAd->force_noack == true)
		pTxWI->ACK = 0;
#endif /* WFA_VHT_PF */

	pTxWI->ShortGI = pTransmit->field.ShortGI;
	pTxWI->STBC = pTransmit->field.STBC;
	pTxWI->MCS = pTransmit->field.MCS;
	pTxWI->PHYMODE = pTransmit->field.MODE;

	/* If CCK or OFDM, BW must be 20 */
	pTxWI->BW = (pTransmit->field.MODE <= MODE_OFDM) ? (BW_20) : (pTransmit->field.BW);
	if (pTxWI->BW)
		pTxWI->BW = (pAd->CommonCfg.AddHTInfo.AddHtInfo.RecomWidth == 0) ? (BW_20) : (pTransmit->field.BW);

	pTxWI->AMPDU = ((pTxBlk->TxFrameType == TX_AMPDU_FRAME) ? true : false);
	BASize = pAd->CommonCfg.TxBASize;
	if((pTxBlk->TxFrameType == TX_AMPDU_FRAME) && (pMacEntry))
	{
		u8 RABAOriIdx = pTxBlk->pMacEntry->BAOriWcidArray[pTxBlk->UserPriority];

		BASize = pAd->BATable.BAOriEntry[RABAOriIdx].BAWinSize;
	}

	pTxWI->BAWinSize = BASize;



	if (pMacEntry)
	{
		if ((pMacEntry->MmpsMode == MMPS_DYNAMIC) && (pTransmit->field.MCS > 7))
		{
			/* Dynamic MIMO Power Save Mode*/
			pTxWI->MIMOps = 1;
		}
		else if (pMacEntry->MmpsMode == MMPS_STATIC)
		{
			/* Static MIMO Power Save Mode*/
			if ((pTransmit->field.MODE == MODE_HTMIX || pTransmit->field.MODE == MODE_HTGREENFIELD) &&
				(pTransmit->field.MCS > 7))
			{
				pTxWI->MCS = 7;
				pTxWI->MIMOps = 0;
			}
		}

		pTxWI->MpduDensity = pMacEntry->MpduDensity;
	}

#ifdef DBG_DIAGNOSE
	if (pTxBlk->QueIdx== 0)
	{
		pAd->DiagStruct.TxDataCnt[pAd->DiagStruct.ArrayCurIdx]++;
		pAd->DiagStruct.TxMcsCnt[pAd->DiagStruct.ArrayCurIdx][pTxWI->MCS]++;
	}
#endif /* DBG_DIAGNOSE */

	/* for rate adapation*/
	pTxWI->TxPktId = pTxWI->MCS;

#ifdef MCS_LUT_SUPPORT
	if ((RTMP_TEST_MORE_FLAG(pAd, fASIC_CAP_MCS_LUT)) &&
		(pTxWI->wcid < 128) &&
		(pMacEntry && pMacEntry->bAutoTxRateSwitch == true))
	{
		HTTRANSMIT_SETTING rate_ctrl;

		rate_ctrl.field.MODE = pTxWI->PHYMODE;
		rate_ctrl.field.STBC = pTxWI->STBC;
		rate_ctrl.field.ShortGI = pTxWI->ShortGI;
		rate_ctrl.field.BW = pTxWI->BW;
		rate_ctrl.field.MCS = pTxWI->MCS;
		if (rate_ctrl.word == pTransmit->word)
			pTxWI->TxWILutEn = 1;
		pTxWI->TxWILutEn = 0;
	}
#endif /* MCS_LUT_SUPPORT */

}


void RTMPWriteTxWI_Cache(
	IN struct rtmp_adapter*pAd,
	INOUT struct mt7610u_txwi *pTxWI,
	IN TX_BLK *pTxBlk)
{
	HTTRANSMIT_SETTING *pTransmit;
	MAC_TABLE_ENTRY *pMacEntry;


	/* update TXWI */
	pMacEntry = pTxBlk->pMacEntry;
	pTransmit = pTxBlk->pTransmit;

	if (pMacEntry->bAutoTxRateSwitch)
	{
		pTxWI->txop = IFS_HTTXOP;

		/* If CCK or OFDM, BW must be 20*/
		pTxWI->BW = (pTransmit->field.MODE <= MODE_OFDM) ? (BW_20) : (pTransmit->field.BW);
		pTxWI->ShortGI = pTransmit->field.ShortGI;
		pTxWI->STBC = pTransmit->field.STBC;

		pTxWI->MCS = pTransmit->field.MCS;
		pTxWI->PHYMODE = pTransmit->field.MODE;

		/* set PID for TxRateSwitching*/
		pTxWI->TxPktId = pTransmit->field.MCS;

	}

	pTxWI->AMPDU = ((pMacEntry->NoBADataCountDown == 0) ? true: false);

	pTxWI->MIMOps = 0;

	if (pTxWI->BW)
		pTxWI->BW = (pAd->CommonCfg.AddHTInfo.AddHtInfo.RecomWidth == 0) ? (BW_20) : (pTransmit->field.BW);

    if (pAd->CommonCfg.bMIMOPSEnable)
    {
		/* MIMO Power Save Mode*/
		if ((pMacEntry->MmpsMode == MMPS_DYNAMIC) && (pTransmit->field.MCS > 7))
		{
			/* Dynamic MIMO Power Save Mode*/
			pTxWI->MIMOps = 1;
		}
		else if (pMacEntry->MmpsMode == MMPS_STATIC)
		{
			/* Static MIMO Power Save Mode*/
			if ((pTransmit->field.MODE >= MODE_HTMIX) && (pTransmit->field.MCS > 7))
			{
				pTxWI->MCS = 7;
				pTxWI->MIMOps = 0;
			}
		}
    }


#ifdef DBG_DIAGNOSE
	if (pTxBlk->QueIdx== 0)
	{
		pAd->DiagStruct.TxDataCnt[pAd->DiagStruct.ArrayCurIdx]++;
		pAd->DiagStruct.TxMcsCnt[pAd->DiagStruct.ArrayCurIdx][pTxWI->MCS]++;
	}
#endif /* DBG_DIAGNOSE */

	pTxWI->MPDUtotalByteCnt = pTxBlk->MpduHeaderLen + pTxBlk->SrcBufLen;


#ifdef WFA_VHT_PF
	if (pAd->force_noack == true)
		pTxWI->ACK = 0;
	else
#endif /* WFA_VHT_PF */
		pTxWI->ACK = TX_BLK_TEST_FLAG(pTxBlk, fTX_bAckRequired);

#ifdef MCS_LUT_SUPPORT
	if (RTMP_TEST_MORE_FLAG(pAd, fASIC_CAP_MCS_LUT) &&
		(pTxWI->wcid < 128) &&
		(pMacEntry && pMacEntry->bAutoTxRateSwitch == true))
	{
		HTTRANSMIT_SETTING rate_ctrl;

		rate_ctrl.field.MODE = pTxWI->PHYMODE;
		rate_ctrl.field.STBC = pTxWI->STBC;
		rate_ctrl.field.ShortGI = pTxWI->ShortGI;
		rate_ctrl.field.BW = pTxWI->BW;
		rate_ctrl.field.MCS = pTxWI->MCS;
		if (rate_ctrl.word == pTransmit->word)
			pTxWI->TxWILutEn = 1;
		pTxWI->TxWILutEn = 0;
	}
#endif /* MCS_LUT_SUPPORT */

}

void mt7610u_mac_set_ctrlch(struct rtmp_adapter*pAd, int extch)
{
	u32 val, band_cfg;


	band_cfg = mt7610u_read32(pAd, TX_BAND_CFG);
	val = band_cfg & (~0x1);
	switch (extch)
	{
		case EXTCHA_ABOVE:
			val &= (~0x1);
			break;
		case EXTCHA_BELOW:
			val |= (0x1);
			break;
		case EXTCHA_NONE:
			val &= (~0x1);
			break;
	}

	if (val != band_cfg)
		mt7610u_write32(pAd, TX_BAND_CFG, val);
}


INT rtmp_mac_set_mmps(struct rtmp_adapter*pAd, INT ReduceCorePower)
{
	u32 mac_val, org_val;

	org_val = mt7610u_read32(pAd, 0x1210);
	mac_val = org_val;
	if (ReduceCorePower)
		mac_val |= 0x09;
	else
		mac_val &= ~0x09;

	if (mac_val != org_val)
		mt7610u_write32(pAd, 0x1210, mac_val);

	return true;
}

