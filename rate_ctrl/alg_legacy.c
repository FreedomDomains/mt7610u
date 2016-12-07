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




#ifdef CONFIG_STA_SUPPORT
/*
	==========================================================================
	Description:
		This routine calculates the acumulated TxPER of eaxh TxRate. And
		according to the calculation result, change CommonCfg.TxRate which
		is the stable TX Rate we expect the Radio situation could sustained.

		CommonCfg.TxRate will change dynamically within {RATE_1/RATE_6, MaxTxRate}
	Output:
		CommonCfg.TxRate -

	IRQL = DISPATCH_LEVEL

	NOTE:
		call this routine every second
	==========================================================================
 */
void MlmeDynamicTxRateSwitching(
	IN struct rtmp_adapter *pAd)
{
	u8 *				pTable;
	u8 				TableSize = 0;
	u8 				UpRateIdx = 0, DownRateIdx = 0, CurrRateIdx;
	ULONG					i, TxTotalCnt;
	ULONG					TxErrorRatio = 0;
	MAC_TABLE_ENTRY			*pEntry;
	RTMP_RA_LEGACY_TB *pCurrTxRate, *pTmpTxRate = NULL;
	u8 				InitTxRateIdx, TrainUp, TrainDown;
	TX_STA_CNT1_STRUC		StaTx1;
	TX_STA_CNT0_STRUC		TxStaCnt0;
	CHAR					Rssi, TmpIdx = 0;
	ULONG					TxRetransmit = 0, TxSuccess = 0, TxFailCount = 0;
	RSSI_SAMPLE				*pRssi = &pAd->StaCfg.RssiSample;
#ifdef AGS_SUPPORT
	AGS_STATISTICS_INFO		AGSStatisticsInfo = {0};
#endif /* AGS_SUPPORT */

	/* Update statistic counter */
	NicGetTxRawCounters(pAd, &TxStaCnt0, &StaTx1);

	TxRetransmit = StaTx1.field.TxRetransmit;
	TxSuccess = StaTx1.field.TxSuccess;
	TxFailCount = TxStaCnt0.field.TxFailCount;
	TxTotalCnt = TxRetransmit + TxSuccess + TxFailCount;

	/* walk through MAC table, see if need to change AP's TX rate toward each entry */
   	for (i = 1; i < MAX_LEN_OF_MAC_TABLE; i++)
	{
		pEntry = &pAd->MacTab.Content[i];

		if (IS_ENTRY_NONE(pEntry))
			continue;

		/* check if this entry need to switch rate automatically */
		if (RTMPCheckEntryEnableAutoRateSwitch(pAd, pEntry) == false)
			continue;

		MlmeSelectTxRateTable(pAd, pEntry, &pTable, &TableSize, &InitTxRateIdx);
		pEntry->pTable = pTable;

#ifdef NEW_RATE_ADAPT_SUPPORT
		if (ADAPT_RATE_TABLE(pTable))
		{
			MlmeDynamicTxRateSwitchingAdapt(pAd, i, TxSuccess, TxRetransmit, TxFailCount);
			continue;
		}
#endif /* NEW_RATE_ADAPT_SUPPORT */

		if ((pAd->MacTab.Size == 1) || IS_ENTRY_DLS(pEntry))
		{
			Rssi = RTMPAvgRssi(pAd, pRssi);

			if (TxTotalCnt)
				TxErrorRatio = ((TxRetransmit + TxFailCount) * 100) / TxTotalCnt;

			DBGPRINT(RT_DEBUG_INFO | DBG_FUNC_RA,
					("DRS:Aid=%d, TxSuccess=%ld, TxRetransmit=%ld, TxFailCount=%ld \n",
					pEntry->Aid, TxSuccess, TxRetransmit, TxFailCount));

#ifdef AGS_SUPPORT
			if (SUPPORT_AGS(pAd))
			{

				/* Gather the statistics information*/

				AGSStatisticsInfo.RSSI = Rssi;
				AGSStatisticsInfo.TxErrorRatio = TxErrorRatio;
				AGSStatisticsInfo.AccuTxTotalCnt = TxTotalCnt;
				AGSStatisticsInfo.TxTotalCnt = TxTotalCnt;
				AGSStatisticsInfo.TxSuccess = TxSuccess;
				AGSStatisticsInfo.TxRetransmit = TxRetransmit;
				AGSStatisticsInfo.TxFailCount = TxFailCount;
			}
#endif /* AGS_SUPPORT */
		}
		else
		{
			if (INFRA_ON(pAd) && (i == 1))
				Rssi = RTMPAvgRssi(pAd, pRssi);
			else
				Rssi = RTMPAvgRssi(pAd, &pEntry->RssiSample);
			TxSuccess = pEntry->OneSecTxNoRetryOkCount;

			TxTotalCnt = pEntry->OneSecTxNoRetryOkCount +
				 pEntry->OneSecTxRetryOkCount +
				 pEntry->OneSecTxFailCount;

			if (TxTotalCnt)
				TxErrorRatio = ((pEntry->OneSecTxRetryOkCount + pEntry->OneSecTxFailCount) * 100) / TxTotalCnt;

			DBGPRINT(RT_DEBUG_INFO | DBG_FUNC_RA,
				("DRS:Aid=%d, OneSecTxNoRetry=%d, OneSecTxRetry=%d, OneSecTxFail=%d\n",
				pEntry->Aid,
				pEntry->OneSecTxNoRetryOkCount,
				pEntry->OneSecTxRetryOkCount,
				pEntry->OneSecTxFailCount));

#ifdef AGS_SUPPORT
			if (SUPPORT_AGS(pAd))
			{

				/* Gather the statistics information*/

				AGSStatisticsInfo.RSSI = Rssi;
				AGSStatisticsInfo.TxErrorRatio = TxErrorRatio;
				AGSStatisticsInfo.AccuTxTotalCnt = TxTotalCnt;
				AGSStatisticsInfo.TxTotalCnt = TxTotalCnt;
				AGSStatisticsInfo.TxSuccess = pEntry->OneSecTxNoRetryOkCount;
				AGSStatisticsInfo.TxRetransmit = pEntry->OneSecTxRetryOkCount;
				AGSStatisticsInfo.TxFailCount = pEntry->OneSecTxFailCount;
			}
#endif /* AGS_SUPPORT */
		}

		if (TxTotalCnt)
		{
			if (TxErrorRatio == 100)
			{
				TX_RTY_CFG_STRUC	TxRtyCfg,TxRtyCfgtmp;
				ULONG	Index;
				u32	MACValue;

				mt7610u_read32(pAd, TX_RTY_CFG, &TxRtyCfg.word);
				TxRtyCfgtmp.word = TxRtyCfg.word;
				TxRtyCfg.field.LongRtyLimit = 0x0;
				TxRtyCfg.field.ShortRtyLimit = 0x0;
				mt7610u_write32(pAd, TX_RTY_CFG, TxRtyCfg.word);

				RTMPusecDelay(1);

				Index = 0;
				MACValue = 0;
				do
				{
					mt7610u_read32(pAd, TXRXQ_PCNT, &MACValue);
					if ((MACValue & 0xffffff) == 0)
						break;
					Index++;
					RTMPusecDelay(1000);
				}while((Index < 330)&&(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS)));

				mt7610u_read32(pAd, TX_RTY_CFG, &TxRtyCfg.word);
				TxRtyCfg.field.LongRtyLimit = TxRtyCfgtmp.field.LongRtyLimit;
				TxRtyCfg.field.ShortRtyLimit = TxRtyCfgtmp.field.ShortRtyLimit;
				mt7610u_write32(pAd, TX_RTY_CFG, TxRtyCfg.word);
			}

		}

		CurrRateIdx = pEntry->CurrTxRateIndex;

#ifdef AGS_SUPPORT
		if (AGS_IS_USING(pAd, pTable))
		{
			/* The dynamic Tx rate switching for AGS (Adaptive Group Switching)*/
			MlmeDynamicTxRateSwitchingAGS(pAd, pEntry, pTable, TableSize, &AGSStatisticsInfo, InitTxRateIdx);

			continue;
		}
#endif /* AGS_SUPPORT */

		if (CurrRateIdx >= TableSize)
			CurrRateIdx = TableSize - 1;

		UpRateIdx = DownRateIdx = CurrRateIdx;

		/* Save LastTxOkCount, LastTxPER and last MCS action for StaQuickResponeForRateUpExec */
		pEntry->LastTxOkCount = TxSuccess;
		pEntry->LastTxPER = (TxTotalCnt == 0 ? 0 : (u8)TxErrorRatio);
		pEntry->LastTimeTxRateChangeAction = pEntry->LastSecTxRateChangeAction;

		/*
			When switch from Fixed rate -> auto rate, the REAL TX rate might be different from pEntry->TxRateIndex.
			So need to sync here.
		*/
		pCurrTxRate = PTX_RA_LEGACY_ENTRY(pTable, CurrRateIdx);
		if (pEntry->HTPhyMode.field.MCS != pCurrTxRate->CurrMCS)
		{
			/*
				Need to sync Real Tx rate and our record.
				Then return for next DRS.
			*/
			pEntry->CurrTxRateIndex = InitTxRateIdx;
			MlmeNewTxRate(pAd, pEntry);

			/* reset all OneSecTx counters */
			RESET_ONE_SEC_TX_CNT(pEntry);
			continue;
		}

		/* decide the next upgrade rate and downgrade rate, if any */
		if ((pCurrTxRate->Mode <= MODE_CCK) && (pEntry->SupportRateMode <= SUPPORT_CCK_MODE))
		{
			TmpIdx = CurrRateIdx + 1;
			while(TmpIdx < TableSize)
			{
				pTmpTxRate = PTX_RA_LEGACY_ENTRY(pTable, TmpIdx);
				if (pEntry->SupportCCKMCS[pTmpTxRate->CurrMCS] == true)
				{
					UpRateIdx = TmpIdx;
					break;
				}
				TmpIdx++;
			}

			TmpIdx = CurrRateIdx - 1;
			while(TmpIdx >= 0)
			{
				pTmpTxRate = PTX_RA_LEGACY_ENTRY(pTable, TmpIdx);
				if (pEntry->SupportCCKMCS[pTmpTxRate->CurrMCS] == true)
				{
					DownRateIdx = TmpIdx;
					break;
				}
				TmpIdx--;
			}
		}
		else if ((pCurrTxRate->Mode <= MODE_OFDM) && (pEntry->SupportRateMode < SUPPORT_HT_MODE))
		{
			TmpIdx = CurrRateIdx + 1;
			while(TmpIdx < TableSize)
			{
				pTmpTxRate = PTX_RA_LEGACY_ENTRY(pTable, TmpIdx);
				if (pEntry->SupportOFDMMCS[pTmpTxRate->CurrMCS] == true)
				{
					UpRateIdx = TmpIdx;
					break;
				}
				TmpIdx++;
			}

			TmpIdx = CurrRateIdx - 1;
			while(TmpIdx >= 0)
			{
				pTmpTxRate = PTX_RA_LEGACY_ENTRY(pTable, TmpIdx);
				if (pEntry->SupportOFDMMCS[pTmpTxRate->CurrMCS] == true)
				{
					DownRateIdx = TmpIdx;
					break;
				}
				TmpIdx--;
			}
		}
		else
		{
			/* decide the next upgrade rate and downgrade rate, if any*/
		if ((CurrRateIdx > 0) && (CurrRateIdx < (TableSize - 1)))
		{
				TmpIdx = CurrRateIdx + 1;
				while(TmpIdx < TableSize)
				{
					pTmpTxRate = PTX_RA_LEGACY_ENTRY(pTable, TmpIdx);
					if (pEntry->SupportHTMCS[pTmpTxRate->CurrMCS] == true)
					{
						UpRateIdx = TmpIdx;
						break;
					}
					TmpIdx++;
				}

				TmpIdx = CurrRateIdx - 1;
				while(TmpIdx >= 0)
				{
					pTmpTxRate = PTX_RA_LEGACY_ENTRY(pTable, TmpIdx);
					if (pEntry->SupportHTMCS[pTmpTxRate->CurrMCS] == true)
					{
						DownRateIdx = TmpIdx;
						break;
					}
					TmpIdx--;
				}
		}
		else if (CurrRateIdx == 0)
		{
				TmpIdx = CurrRateIdx + 1;
				while(TmpIdx < TableSize)
				{
					pTmpTxRate = PTX_RA_LEGACY_ENTRY(pTable, TmpIdx);
					if (pEntry->SupportHTMCS[pTmpTxRate->CurrMCS] == true)
					{
						UpRateIdx = TmpIdx;
						break;
					}
					TmpIdx++;
				}

			DownRateIdx = CurrRateIdx;
		}
		else if (CurrRateIdx == (TableSize - 1))
		{
			UpRateIdx = CurrRateIdx;

				TmpIdx = CurrRateIdx - 1;
				while(TmpIdx >= 0)
				{
					pTmpTxRate = PTX_RA_LEGACY_ENTRY(pTable, TmpIdx);
					if (pEntry->SupportHTMCS[pTmpTxRate->CurrMCS] == true)
					{
						DownRateIdx = TmpIdx;
						break;
					}
					TmpIdx--;
				}
			}
		}

		pCurrTxRate = PTX_RA_LEGACY_ENTRY(pTable, CurrRateIdx);

#ifdef DOT11_N_SUPPORT

		/*
			when Rssi > -65, there is a lot of interference usually. therefore, the
			algorithm tends to choose the mcs lower than the optimal one.
			by increasing the thresholds, the chosen mcs will be closer to the optimal mcs
		*/
		if ((Rssi > -65) && (pCurrTxRate->Mode >= MODE_HTMIX))
		{
			TrainUp = (pCurrTxRate->TrainUp + (pCurrTxRate->TrainUp >> 1));
			TrainDown = (pCurrTxRate->TrainDown + (pCurrTxRate->TrainDown >> 1));
		}
		else
#endif /* DOT11_N_SUPPORT */
		{
			TrainUp = pCurrTxRate->TrainUp;
			TrainDown = pCurrTxRate->TrainDown;
		}


#ifdef DBG_CTRL_SUPPORT
		/* Debug option: Concise RA log */
		if (pAd->CommonCfg.DebugFlags & DBF_SHOW_RA_LOG)
			MlmeRALog(pAd, pEntry, RAL_OLD_DRS, TxErrorRatio, TxTotalCnt);
#endif /* DBG_CTRL_SUPPORT */



		/*
			CASE 1. when TX samples are fewer than 15, then decide TX rate solely on RSSI
			     (criteria copied from RT2500 for Netopia case)
		*/
		if (TxTotalCnt <= 15)
		{
			u8 TxRateIdx;
			CHAR	mcs[24];
			CHAR	RssiOffset = 0;

			/* Check existence and get the index of each MCS */
			MlmeGetSupportedMcs(pAd, pTable, mcs);

			if (pAd->LatchRfRegs.Channel <= 14)
			{
				RssiOffset = pAd->NicConfig2.field.ExternalLNAForG? 2: 5;
			}
			else
			{
				RssiOffset = pAd->NicConfig2.field.ExternalLNAForA? 5: 8;
			}

			/* Select the Tx rate based on the RSSI */
			TxRateIdx = MlmeSelectTxRate(pAd, pEntry, mcs, Rssi, RssiOffset);


			{
				pEntry->CurrTxRateIndex = TxRateIdx;
				MlmeNewTxRate(pAd, pEntry);
				if (!pEntry->fLastSecAccordingRSSI)
					DBGPRINT_RAW(RT_DEBUG_INFO,("DRS: TxTotalCnt <= 15, switch MCS according to RSSI (%d), RssiOffset=%d\n", Rssi, RssiOffset));
			}

			MlmeClearAllTxQuality(pEntry);
			pEntry->fLastSecAccordingRSSI = true;

			/* reset all OneSecTx counters */
			RESET_ONE_SEC_TX_CNT(pEntry);


			continue;
		}

		if (pEntry->fLastSecAccordingRSSI == true)
		{
			pEntry->fLastSecAccordingRSSI = false;
			pEntry->LastSecTxRateChangeAction = RATE_NO_CHANGE;
			/* reset all OneSecTx counters */
			RESET_ONE_SEC_TX_CNT(pEntry);


			continue;
		}

		pEntry->PER[CurrRateIdx] = (u8)TxErrorRatio;

		/* Select rate based on PER */
		MlmeOldRateAdapt(pAd, pEntry, CurrRateIdx, UpRateIdx, DownRateIdx, TrainUp, TrainDown, TxErrorRatio);

#ifdef DOT11N_SS3_SUPPORT
		/* Turn off RDG when 3s and rx count > tx count*5 */
		MlmeCheckRDG(pAd, pEntry);
#endif /* DOT11N_SS3_SUPPORT */

		/* reset all OneSecTx counters */
		RESET_ONE_SEC_TX_CNT(pEntry);

		}
}


/*
	========================================================================
	Routine Description:
		Station side, Auto TxRate faster train up timer call back function.

	Arguments:
		SystemSpecific1			- Not used.
		FunctionContext			- Pointer to our Adapter context.
		SystemSpecific2			- Not used.
		SystemSpecific3			- Not used.

	Return Value:
		None

	========================================================================
*/
void StaQuickResponeForRateUpExec(
	IN void *SystemSpecific1,
	IN void *FunctionContext,
	IN void *SystemSpecific2,
	IN void *SystemSpecific3)
{
	struct rtmp_adapter *		pAd = (struct rtmp_adapter *)FunctionContext;
	ULONG					i;
	u8 *				pTable;
	u8 				TableSize = 0;
	u8 				CurrRateIdx;
	ULONG					TxTotalCnt;
	ULONG					TxErrorRatio = 0;
	RTMP_RA_LEGACY_TB *pCurrTxRate;
	u8 				InitTxRateIdx, TrainUp, TrainDown;
	CHAR					Rssi, ratio;
	ULONG					TxSuccess, TxRetransmit, TxFailCount;
	MAC_TABLE_ENTRY			*pEntry;
#ifdef AGS_SUPPORT
	AGS_STATISTICS_INFO		AGSStatisticsInfo = {0};
#endif /* AGS_SUPPORT */

	pAd->StaCfg.StaQuickResponeForRateUpTimerRunning = false;

    /* walk through MAC table, see if need to change AP's TX rate toward each entry */
	for (i = 1; i < MAX_LEN_OF_MAC_TABLE; i++)
	{
		pEntry = &pAd->MacTab.Content[i];

		if (IS_ENTRY_NONE(pEntry))
			continue;

		/* check if this entry need to switch rate automatically */
		if (RTMPCheckEntryEnableAutoRateSwitch(pAd, pEntry) == false)
			continue;

		/* Do nothing if this entry didn't change */
		if (pEntry->LastSecTxRateChangeAction == RATE_NO_CHANGE
#ifdef DBG_CTRL_SUPPORT
			&& (pAd->CommonCfg.DebugFlags & DBF_FORCE_QUICK_DRS)==0
#endif /* DBG_CTRL_SUPPORT */
		)
			continue;

		if (INFRA_ON(pAd) && (i == 1))
			Rssi = RTMPAvgRssi(pAd, &pAd->StaCfg.RssiSample);
		else
			Rssi = RTMPAvgRssi(pAd, &pEntry->RssiSample);

		MlmeSelectTxRateTable(pAd, pEntry, &pTable, &TableSize, &InitTxRateIdx);
		pEntry->pTable = pTable;

#ifdef NEW_RATE_ADAPT_SUPPORT
		if (ADAPT_RATE_TABLE(pTable))
		{
			StaQuickResponeForRateUpExecAdapt(pAd, i, Rssi);
			continue;
		}
#endif /* NEW_RATE_ADAPT_SUPPORT */

		CurrRateIdx = pEntry->CurrTxRateIndex;
		pCurrTxRate = PTX_RA_LEGACY_ENTRY(pTable, CurrRateIdx);

#ifdef DOT11_N_SUPPORT
		if ((Rssi > -65) && (pCurrTxRate->Mode >= MODE_HTMIX))
		{
			TrainUp		= (pCurrTxRate->TrainUp + (pCurrTxRate->TrainUp >> 1));
			TrainDown	= (pCurrTxRate->TrainDown + (pCurrTxRate->TrainDown >> 1));
		}
		else
#endif /* DOT11_N_SUPPORT */
		{
			TrainUp		= pCurrTxRate->TrainUp;
			TrainDown	= pCurrTxRate->TrainDown;
		}

		if (pAd->MacTab.Size == 1)
		{
			/* Update statistic counter */
			TX_STA_CNT1_STRUC	StaTx1;
			TX_STA_CNT0_STRUC	TxStaCnt0;

			mt7610u_read32(pAd, TX_STA_CNT0, &TxStaCnt0.word);
			mt7610u_read32(pAd, TX_STA_CNT1, &StaTx1.word);

			TxRetransmit = StaTx1.field.TxRetransmit;
			TxSuccess = StaTx1.field.TxSuccess;
			TxFailCount = TxStaCnt0.field.TxFailCount;
			TxTotalCnt = TxRetransmit + TxSuccess + TxFailCount;

			pAd->RalinkCounters.OneSecTxRetryOkCount += StaTx1.field.TxRetransmit;
			pAd->RalinkCounters.OneSecTxNoRetryOkCount += StaTx1.field.TxSuccess;
			pAd->RalinkCounters.OneSecTxFailCount += TxStaCnt0.field.TxFailCount;

#ifdef STATS_COUNT_SUPPORT
			pAd->WlanCounters.TransmittedFragmentCount.u.LowPart += StaTx1.field.TxSuccess;
			pAd->WlanCounters.RetryCount.u.LowPart += StaTx1.field.TxRetransmit;
			pAd->WlanCounters.FailedCount.u.LowPart += TxStaCnt0.field.TxFailCount;
#endif /* STATS_COUNT_SUPPORT */

			if (TxTotalCnt)
				TxErrorRatio = ((TxRetransmit + TxFailCount) * 100) / TxTotalCnt;

#ifdef AGS_SUPPORT
			if (SUPPORT_AGS(pAd))
			{

				/* Gather the statistics information*/

				AGSStatisticsInfo.RSSI = Rssi;
				AGSStatisticsInfo.TxErrorRatio = TxErrorRatio;
				AGSStatisticsInfo.AccuTxTotalCnt = TxTotalCnt;
				AGSStatisticsInfo.TxTotalCnt = TxTotalCnt;
				AGSStatisticsInfo.TxSuccess = TxSuccess;
				AGSStatisticsInfo.TxRetransmit = TxRetransmit;
				AGSStatisticsInfo.TxFailCount = TxFailCount;
			}
#endif /* AGS_SUPPORT */
		}
		else
		{
			TxRetransmit = pEntry->OneSecTxRetryOkCount;
			TxSuccess = pEntry->OneSecTxNoRetryOkCount;
			TxFailCount = pEntry->OneSecTxFailCount;

			TxTotalCnt = TxRetransmit + TxSuccess + TxFailCount;
			if (TxTotalCnt)
				TxErrorRatio = ((TxRetransmit + TxFailCount) * 100) / TxTotalCnt;

#ifdef FIFO_EXT_SUPPORT
			if (pAd->chipCap.FlgHwFifoExtCap)
			{
				if (pEntry->Aid >= 1 && pEntry->Aid <= 8)
				{
					WCID_TX_CNT_STRUC wcidTxCnt;
					u32 regAddr, offset;
					ULONG HwTxCnt, HwErrRatio = 0;

					regAddr = WCID_TX_CNT_0 + (pEntry->Aid - 1) * 4;
					mt7610u_read32(pAd, regAddr, &wcidTxCnt.word);

					HwTxCnt = wcidTxCnt.field.succCnt + wcidTxCnt.field.reTryCnt;
					if (HwTxCnt)
						HwErrRatio = (wcidTxCnt.field.reTryCnt * 100) / HwTxCnt;

					DBGPRINT(RT_DEBUG_INFO | DBG_FUNC_RA, ("%s():TxErrRatio(Aid:%d, MCS:%d, Hw:0x%x-0x%x, Sw:0x%x-%x)\n",
							__FUNCTION__, pEntry->Aid, pEntry->HTPhyMode.field.MCS,
							HwTxCnt, HwErrRatio, TxTotalCnt, TxErrorRatio));

					TxSuccess = wcidTxCnt.field.succCnt;
					TxRetransmit = wcidTxCnt.field.reTryCnt;
					TxErrorRatio = HwErrRatio;
					TxTotalCnt = HwTxCnt;
				}
			}
#endif /* FIFO_EXT_SUPPORT */

#ifdef AGS_SUPPORT
			if (SUPPORT_AGS(pAd))
			{

				/* Gather the statistics information*/

				AGSStatisticsInfo.RSSI = Rssi;
				AGSStatisticsInfo.TxErrorRatio = TxErrorRatio;
				AGSStatisticsInfo.AccuTxTotalCnt = TxTotalCnt;
				AGSStatisticsInfo.TxTotalCnt = TxTotalCnt;
				AGSStatisticsInfo.TxSuccess = pEntry->OneSecTxNoRetryOkCount;
				AGSStatisticsInfo.TxRetransmit = pEntry->OneSecTxRetryOkCount;
				AGSStatisticsInfo.TxFailCount = pEntry->OneSecTxFailCount;
			}
#endif /* AGS_SUPPORT */
		}

#ifdef AGS_SUPPORT
		if (AGS_IS_USING(pAd, pTable))
		{

			/* The dynamic Tx rate switching for AGS (Adaptive Group Switching)*/

			StaQuickResponeForRateUpExecAGS(pAd, pEntry, pTable, TableSize, &AGSStatisticsInfo, InitTxRateIdx);

			continue; /* Skip the remaining procedure of the old Tx rate switching*/
		}
#endif /* AGS_SUPPORT */

#ifdef DBG_CTRL_SUPPORT
		/* Debug option: Concise RA log */
		if (pAd->CommonCfg.DebugFlags & DBF_SHOW_RA_LOG)
			MlmeRALog(pAd, pEntry, RAL_QUICK_DRS, TxErrorRatio, TxTotalCnt);
#endif /* DBG_CTRL_SUPPORT */

		/*
			CASE 1. when TX samples are fewer than 15, then decide TX rate solely on RSSI
			     (criteria copied from RT2500 for Netopia case)
		*/
		if (TxTotalCnt <= 12)
		{
			MlmeClearAllTxQuality(pEntry);

			/* Set current up MCS at the worst quality */
			if (pEntry->LastSecTxRateChangeAction == RATE_UP)
			{
				MlmeSetTxQuality(pEntry, CurrRateIdx, DRS_TX_QUALITY_WORST_BOUND);
			}

			/* Go back to the original rate */
			MlmeRestoreLastRate(pEntry);
			MlmeNewTxRate(pAd, pEntry);

			// TODO: should we reset all OneSecTx counters?
			/* RESET_ONE_SEC_TX_CNT(pEntry); */

			continue;
		}

		pEntry->PER[CurrRateIdx] = (u8)TxErrorRatio;

       /* Compare throughput */
		do
		{
			ULONG OneSecTxNoRetryOKRationCount;

			/*
				Compare throughput.
				LastTxCount is based on a time interval of 500 msec or "500-pAd->ra_fast_interval" ms.
			*/
			if ((pEntry->LastTimeTxRateChangeAction == RATE_NO_CHANGE)
#ifdef DBG_CTRL_SUPPORT
				&& (pAd->CommonCfg.DebugFlags & DBF_FORCE_QUICK_DRS)==0
#endif /* DBG_CTRL_SUPPORT */
			)
				ratio = RA_INTERVAL/pAd->ra_fast_interval;
			else
				ratio = (RA_INTERVAL-pAd->ra_fast_interval)/pAd->ra_fast_interval;

			OneSecTxNoRetryOKRationCount = (TxSuccess * ratio);

			/* downgrade TX quality if PER >= Rate-Down threshold */
			if (TxErrorRatio >= TrainDown)
			{
				MlmeSetTxQuality(pEntry, CurrRateIdx, DRS_TX_QUALITY_WORST_BOUND);
			}

			/* perform DRS - consider TxRate Down first, then rate up. */
			if (pEntry->LastSecTxRateChangeAction == RATE_UP)
			{
				if (TxErrorRatio >= TrainDown)
				{
						MlmeSetTxQuality(pEntry, CurrRateIdx, DRS_TX_QUALITY_WORST_BOUND);

					MlmeRestoreLastRate(pEntry);
					DBGPRINT(RT_DEBUG_INFO | DBG_FUNC_RA,("   QuickDRS: (Up) bad tx ok count (L:%ld, C:%ld)\n", pEntry->LastTxOkCount, OneSecTxNoRetryOKRationCount));
				}
				else
				{
					DBGPRINT(RT_DEBUG_INFO | DBG_FUNC_RA,("   QuickDRS: (Up) keep rate-up (L:%ld, C:%ld)\n", pEntry->LastTxOkCount, OneSecTxNoRetryOKRationCount));
				}
			}
			else if (pEntry->LastSecTxRateChangeAction == RATE_DOWN)
			{
				/* Note: AP had "(TxErrorRatio >= 50) && (TxErrorRatio >= TrainDown)" */
				if ((TxErrorRatio >= 50) || (TxErrorRatio >= TrainDown))
				{
					DBGPRINT(RT_DEBUG_INFO | DBG_FUNC_RA,("   QuickDRS: (Down) direct train down (TxErrorRatio >= TrainDown)\n"));
				}
				else if ((pEntry->LastTxOkCount + 2) >= OneSecTxNoRetryOKRationCount)
				{
					MlmeRestoreLastRate(pEntry);
					DBGPRINT(RT_DEBUG_INFO | DBG_FUNC_RA,("   QuickDRS: (Down) bad tx ok count (L:%ld, C:%ld)\n", pEntry->LastTxOkCount, OneSecTxNoRetryOKRationCount));
				}
				else
					DBGPRINT(RT_DEBUG_INFO | DBG_FUNC_RA,("   QuickDRS: (Down) keep rate-down (L:%ld, C:%ld)\n", pEntry->LastTxOkCount, OneSecTxNoRetryOKRationCount));
			}
		}while (false);


		/* If rate changed then update the history and set the new tx rate */
		if ((pEntry->CurrTxRateIndex != CurrRateIdx)
		)
		{
			/* if rate-up happen, clear all bad history of all TX rates */
			if (pEntry->LastSecTxRateChangeAction == RATE_DOWN)
			{
				/* DBGPRINT_RAW(RT_DEBUG_INFO,("   QuickDRS: ++TX rate from %d to %d \n", CurrRateIdx, pEntry->CurrTxRateIndex)); */

				pEntry->TxRateUpPenalty = 0;
				if (pEntry->CurrTxRateIndex != CurrRateIdx)
					MlmeClearTxQuality(pEntry);
			}
			/* if rate-down happen, only clear DownRate's bad history */
			else if (pEntry->LastSecTxRateChangeAction == RATE_UP)
			{
				/* DBGPRINT_RAW(RT_DEBUG_INFO,("   QuickDRS: --TX rate from %d to %d \n", CurrRateIdx, pEntry->CurrTxRateIndex)); */

				pEntry->TxRateUpPenalty = 0;           /* no penalty */
				MlmeSetTxQuality(pEntry, pEntry->CurrTxRateIndex, 0);
				pEntry->PER[pEntry->CurrTxRateIndex] = 0;
			}

			MlmeNewTxRate(pAd, pEntry);
		}

		// TODO: should we reset all OneSecTx counters?
		/* RESET_ONE_SEC_TX_CNT(pEntry); */
	}
}
#endif /* CONFIG_STA_SUPPORT */


/*
	MlmeOldRateAdapt - perform Rate Adaptation based on PER using old RA algorithm
		pEntry - the MAC table entry
		CurrRateIdx - the index of the current rate
		UpRateIdx, DownRateIdx - UpRate and DownRate index
		TrainUp, TrainDown - TrainUp and Train Down threhsolds
		TxErrorRatio - the PER

		On exit:
			pEntry->LastSecTxRateChangeAction = RATE_UP or RATE_DOWN if there was a change
			pEntry->CurrTxRateIndex = new rate index
			pEntry->TxQuality is updated
*/
void MlmeOldRateAdapt(
	IN struct rtmp_adapter *	pAd,
	IN PMAC_TABLE_ENTRY	pEntry,
	IN u8 		CurrRateIdx,
	IN u8 		UpRateIdx,
	IN u8 		DownRateIdx,
	IN ULONG			TrainUp,
	IN ULONG			TrainDown,
	IN ULONG			TxErrorRatio)
{
	bool bTrainUp = false;

	pEntry->LastSecTxRateChangeAction = RATE_NO_CHANGE;

	pEntry->CurrTxRateStableTime++;

	/* Downgrade TX quality if PER >= Rate-Down threshold */
	if (TxErrorRatio >= TrainDown)
	{
		MlmeSetTxQuality(pEntry, CurrRateIdx, DRS_TX_QUALITY_WORST_BOUND);
		if (CurrRateIdx != DownRateIdx)
		{
			pEntry->CurrTxRateIndex = DownRateIdx;
			pEntry->LastSecTxRateChangeAction = RATE_DOWN;
		}
	}
	else
	{
		/* Upgrade TX quality if PER <= Rate-Up threshold */
		if (TxErrorRatio <= TrainUp)
		{
			bTrainUp = true;
			MlmeDecTxQuality(pEntry, CurrRateIdx);  /* quality very good in CurrRate */

			if (pEntry->TxRateUpPenalty)
				pEntry->TxRateUpPenalty --;
			else
				MlmeDecTxQuality(pEntry, UpRateIdx);    /* may improve next UP rate's quality */
		}

		if (bTrainUp)
		{
			/* Train up if up rate quality is 0 */
			if ((CurrRateIdx != UpRateIdx) && (MlmeGetTxQuality(pEntry, UpRateIdx) <= 0))
			{
				pEntry->CurrTxRateIndex = UpRateIdx;
				pEntry->LastSecTxRateChangeAction = RATE_UP;
			}
		}
	}

	/* Handle the rate change */
	if (pEntry->LastSecTxRateChangeAction != RATE_NO_CHANGE)
	{
		pEntry->CurrTxRateStableTime = 0;
		pEntry->TxRateUpPenalty = 0;

		/* Save last rate information */
		pEntry->lastRateIdx = CurrRateIdx;

		/* Update TxQuality */
		if (pEntry->LastSecTxRateChangeAction == RATE_UP)
		{
			/* Clear history if normal train up */
			if (pEntry->lastRateIdx != pEntry->CurrTxRateIndex)
				MlmeClearTxQuality(pEntry);
		}
		else
		{
			/* Clear the down rate history */
			MlmeSetTxQuality(pEntry, pEntry->CurrTxRateIndex, 0);
			pEntry->PER[pEntry->CurrTxRateIndex] = 0;
		}

		/* Set timer for check in 100 msec */
#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			if (!pAd->StaCfg.StaQuickResponeForRateUpTimerRunning)
			{
				RTMPSetTimer(&pAd->StaCfg.StaQuickResponeForRateUpTimer, pAd->ra_fast_interval);
				pAd->StaCfg.StaQuickResponeForRateUpTimerRunning = true;
			}
		}
#endif /* CONFIG_STA_SUPPORT */

		/* Update PHY rate */
		MlmeNewTxRate(pAd, pEntry);
	}
}
