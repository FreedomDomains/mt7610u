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
    ==========================================================================
    Description:
        dls state machine init, including state transition and timer init
    Parameters:
        Sm - pointer to the dls state machine
    Note:
        The state machine looks like this

                            DLS_IDLE
    MT2_MLME_DLS_REQUEST   MlmeDlsReqAction
    MT2_PEER_DLS_REQUEST   PeerDlsReqAction
    MT2_PEER_DLS_RESPONSE  PeerDlsRspAction
    MT2_MLME_DLS_TEARDOWN  MlmeTearDownAction
    MT2_PEER_DLS_TEARDOWN  PeerTearDownAction

	IRQL = PASSIVE_LEVEL

    ==========================================================================
 */
void DlsStateMachineInit(
	IN struct rtmp_adapter *pAd,
	IN STATE_MACHINE *Sm,
	OUT STATE_MACHINE_FUNC Trans[])
{
	u8 i;

	StateMachineInit(Sm, (STATE_MACHINE_FUNC *) Trans, MAX_DLS_STATE,
			 MAX_DLS_MSG, (STATE_MACHINE_FUNC) Drop, DLS_IDLE,
			 DLS_MACHINE_BASE);

	/* the first column */
	StateMachineSetAction(Sm, DLS_IDLE, MT2_MLME_DLS_REQ,
			      (STATE_MACHINE_FUNC) MlmeDlsReqAction);
	StateMachineSetAction(Sm, DLS_IDLE, MT2_PEER_DLS_REQ,
			      (STATE_MACHINE_FUNC) PeerDlsReqAction);
	StateMachineSetAction(Sm, DLS_IDLE, MT2_PEER_DLS_RSP,
			      (STATE_MACHINE_FUNC) PeerDlsRspAction);
	StateMachineSetAction(Sm, DLS_IDLE, MT2_MLME_DLS_TEAR_DOWN,
			      (STATE_MACHINE_FUNC) MlmeDlsTearDownAction);
	StateMachineSetAction(Sm, DLS_IDLE, MT2_PEER_DLS_TEAR_DOWN,
			      (STATE_MACHINE_FUNC) PeerDlsTearDownAction);

	for (i = 0; i < MAX_NUM_OF_DLS_ENTRY; i++) {
		pAd->StaCfg.DLSEntry[i].pAd = pAd;
		RTMPInitTimer(pAd, &pAd->StaCfg.DLSEntry[i].Timer,
			      GET_TIMER_FUNCTION(DlsTimeoutAction), pAd, false);
	}
}

/*
    ==========================================================================
    Description:

	IRQL = DISPATCH_LEVEL

    ==========================================================================
 */
void MlmeDlsReqAction(
	IN struct rtmp_adapter *pAd,
	IN MLME_QUEUE_ELEM *Elem)
{
	u8 *pOutBuffer = NULL;
	int NStatus;
	ULONG FrameLen = 0;
	HEADER_802_11 DlsReqHdr;
	PRT_802_11_DLS pDLS = NULL;
	u8 Category = CATEGORY_DLS;
	u8 Action = ACTION_DLS_REQUEST;
	ULONG tmp;
	USHORT reason;
	ULONG Timeout;
	bool TimerCancelled;

	if (!MlmeDlsReqSanity(pAd, Elem->Msg, Elem->MsgLen, &pDLS, &reason))
		return;

	DBGPRINT(RT_DEBUG_TRACE, ("DLS - MlmeDlsReqAction() \n"));

	NStatus = kmalloc(&pOutBuffer, MGMT_DMA_BUFFER_SIZE);	/*Get an unused nonpaged memory */
	if (NStatus != NDIS_STATUS_SUCCESS) {
		DBGPRINT(RT_DEBUG_ERROR,
			 ("DLS - MlmeDlsReqAction() allocate memory failed \n"));
		return;
	}

	ActHeaderInit(pAd, &DlsReqHdr, pAd->CommonCfg.Bssid,
		      pAd->CurrentAddress, pAd->CommonCfg.Bssid);

	/* Build basic frame first */
	MakeOutgoingFrame(pOutBuffer, &FrameLen,
			  sizeof (HEADER_802_11), &DlsReqHdr,
			  1, &Category,
			  1, &Action,
			  6, &pDLS->MacAddr,
			  6, pAd->CurrentAddress,
			  2, &pAd->StaActive.CapabilityInfo,
			  2, &pDLS->TimeOut,
			  1, &SupRateIe,
			  1, &pAd->MlmeAux.SupRateLen,
			  pAd->MlmeAux.SupRateLen, pAd->MlmeAux.SupRate,
			  END_OF_ARGS);

	if (pAd->MlmeAux.ExtRateLen != 0) {
		MakeOutgoingFrame(pOutBuffer + FrameLen, &tmp,
				  1, &ExtRateIe,
				  1, &pAd->MlmeAux.ExtRateLen,
				  pAd->MlmeAux.ExtRateLen, pAd->MlmeAux.ExtRate,
				  END_OF_ARGS);
		FrameLen += tmp;
	}
#ifdef DOT11_N_SUPPORT
	if (WMODE_CAP_N(pAd->CommonCfg.PhyMode)) {
		u8 HtLen;

#ifdef RT_BIG_ENDIAN
		HT_CAPABILITY_IE HtCapabilityTmp;
#endif

		/* add HT Capability IE */
		HtLen = sizeof (HT_CAPABILITY_IE);
#ifndef RT_BIG_ENDIAN
		MakeOutgoingFrame(pOutBuffer + FrameLen, &tmp,
				  1, &HtCapIe,
				  1, &HtLen,
				  HtLen, &pAd->CommonCfg.HtCapability,
				  END_OF_ARGS);
#else
		memmove(&HtCapabilityTmp, &pAd->CommonCfg.HtCapability, HtLen);
		*(USHORT *) (&HtCapabilityTmp.HtCapInfo) = SWAP16(*(USHORT *) (&HtCapabilityTmp.HtCapInfo));
		*(USHORT *) (&HtCapabilityTmp.ExtHtCapInfo) = SWAP16(*(USHORT *) (&HtCapabilityTmp.ExtHtCapInfo));

		MakeOutgoingFrame(pOutBuffer + FrameLen, &tmp,
				  1, &HtCapIe,
				  1, &HtLen,
				  HtLen, &HtCapabilityTmp, END_OF_ARGS);
#endif
		FrameLen = FrameLen + tmp;
	}
#endif /* DOT11_N_SUPPORT */

	RTMPCancelTimer(&pDLS->Timer, &TimerCancelled);
	Timeout = DLS_TIMEOUT;
	RTMPSetTimer(&pDLS->Timer, Timeout);

	MiniportMMRequest(pAd, QID_AC_BE, pOutBuffer, FrameLen);
	kfree(pOutBuffer);
}

/*
    ==========================================================================
    Description:

	IRQL = DISPATCH_LEVEL

    ==========================================================================
 */
void PeerDlsReqAction(
	IN struct rtmp_adapter *pAd,
	IN MLME_QUEUE_ELEM *Elem)
{
	u8 *pOutBuffer = NULL;
	int NStatus;
	ULONG FrameLen = 0;
	USHORT StatusCode = MLME_SUCCESS;
	HEADER_802_11 DlsRspHdr;
	u8 Category = CATEGORY_DLS;
	u8 Action = ACTION_DLS_RESPONSE;
	ULONG tmp;
	USHORT CapabilityInfo;
	u8 DA[ETH_ALEN], SA[ETH_ALEN];
	USHORT DLSTimeOut;
	SHORT i;
	ULONG Timeout;
	bool TimerCancelled;
	PRT_802_11_DLS pDLS = NULL;
	u8 MaxSupportedRateIn500Kbps = 0;
	u8 SupportedRatesLen;
	u8 SupportedRates[MAX_LEN_OF_SUPPORTED_RATES];
	u8 HtCapabilityLen;
	HT_CAPABILITY_IE HtCapability;

	if (!PeerDlsReqSanity
	    (pAd, Elem->Msg, Elem->MsgLen, DA, SA, &CapabilityInfo, &DLSTimeOut,
	     &SupportedRatesLen, &SupportedRates[0], &HtCapabilityLen,
	     &HtCapability))
		return;

	/* supported rates array may not be sorted. sort it and find the maximum rate */
	MaxSupportedRateIn500Kbps = dot11_max_sup_rate(SupportedRatesLen, &SupportedRates[0], 0, NULL);

	DBGPRINT(RT_DEBUG_TRACE,
		 ("DLS - PeerDlsReqAction() from %02x:%02x:%02x:%02x:%02x:%02x\n",
		  SA[0], SA[1], SA[2], SA[3], SA[4], SA[5]));

	NStatus = kmalloc(&pOutBuffer, MGMT_DMA_BUFFER_SIZE);	/*Get an unused nonpaged memory */
	if (NStatus != NDIS_STATUS_SUCCESS) {
		DBGPRINT(RT_DEBUG_ERROR,
			 ("DLS - PeerDlsReqAction() allocate memory failed \n"));
		return;
	}

	if (!INFRA_ON(pAd)) {
		StatusCode = MLME_REQUEST_DECLINED;
	} else if (!pAd->CommonCfg.bWmmCapable) {
		StatusCode = MLME_DEST_STA_IS_NOT_A_QSTA;
	} else if (!pAd->CommonCfg.bDLSCapable) {
		StatusCode = MLME_REQUEST_DECLINED;
	} else {
		/* find table to update parameters */
		for (i = (MAX_NUM_OF_DLS_ENTRY - 1); i >= 0; i--) {
			if (pAd->StaCfg.DLSEntry[i].Valid
			    && MAC_ADDR_EQUAL(SA,
					      pAd->StaCfg.DLSEntry[i].
					      MacAddr)) {
				if (pAd->StaCfg.AuthMode >=
				    Ndis802_11AuthModeWPA)
					pAd->StaCfg.DLSEntry[i].Status =
					    DLS_WAIT_KEY;
				else {
					RTMPCancelTimer(&pAd->StaCfg.
							DLSEntry[i].Timer,
							&TimerCancelled);
					pAd->StaCfg.DLSEntry[i].Status =
					    DLS_FINISH;
				}

				pAd->StaCfg.DLSEntry[i].Sequence = 0;
				pAd->StaCfg.DLSEntry[i].TimeOut = DLSTimeOut;
				pAd->StaCfg.DLSEntry[i].CountDownTimer =
				    DLSTimeOut;
				if (HtCapabilityLen != 0)
					pAd->StaCfg.DLSEntry[i].bHTCap = true;
				else
					pAd->StaCfg.DLSEntry[i].bHTCap = false;
				pDLS = &pAd->StaCfg.DLSEntry[i];
				break;
			}
		}

		/* can not find in table, create a new one */
		if (i < 0) {
			DBGPRINT(RT_DEBUG_TRACE,
				 ("DLS - PeerDlsReqAction() can not find same entry \n"));
			for (i = (MAX_NUM_OF_DLS_ENTRY - 1);
			     i >= MAX_NUM_OF_INIT_DLS_ENTRY; i--) {
				if (!pAd->StaCfg.DLSEntry[i].Valid) {
					MAC_TABLE_ENTRY *pEntry;
					u8 MaxSupportedRate = RATE_11;

					if (pAd->StaCfg.AuthMode >= Ndis802_11AuthModeWPA) {
						pAd->StaCfg.DLSEntry[i].Status = DLS_WAIT_KEY;
					} else {
						RTMPCancelTimer(&pAd->StaCfg.DLSEntry[i].Timer, &TimerCancelled);
						pAd->StaCfg.DLSEntry[i].Status = DLS_FINISH;
					}

					pAd->StaCfg.DLSEntry[i].Sequence = 0;
					pAd->StaCfg.DLSEntry[i].Valid = true;
					pAd->StaCfg.DLSEntry[i].TimeOut = DLSTimeOut;
					pAd->StaCfg.DLSEntry[i].CountDownTimer = DLSTimeOut;
					memmove(pAd->StaCfg.DLSEntry[i].MacAddr, SA, ETH_ALEN);
					if (HtCapabilityLen != 0)
						pAd->StaCfg.DLSEntry[i].bHTCap = true;
					else
						pAd->StaCfg.DLSEntry[i].bHTCap = false;
					pDLS = &pAd->StaCfg.DLSEntry[i];
					pEntry = MacTableInsertDlsEntry(pAd, SA, i);

					MaxSupportedRate = dot11_2_ra_rate(MaxSupportedRateIn500Kbps);

					pEntry->MaxSupportedRate = min(pAd->CommonCfg.MaxTxRate, MaxSupportedRate);

					set_entry_phy_cfg(pAd, pEntry);

					pEntry->MaxHTPhyMode.field.BW = BW_20;
					pEntry->MinHTPhyMode.field.BW = BW_20;

#ifdef DOT11_N_SUPPORT
					pEntry->HTCapability.MCSSet[0] = 0;
					pEntry->HTCapability.MCSSet[1] = 0;

					/* If this Entry supports 802.11n, upgrade to HT rate. */
					if ((HtCapabilityLen != 0) && WMODE_CAP_N(pAd->CommonCfg.PhyMode))
					{
						DBGPRINT(RT_DEBUG_TRACE,
							 ("DLS - PeerDlsReqAction() Receive Peer HT Capable STA from %02x:%02x:%02x:%02x:%02x:%02x\n",
							  PRINT_MAC(SA)));

						ht_mode_adjust(pAd, pEntry, &HtCapability, &pAd->CommonCfg.DesiredHtPhy);

						/* find max fixed rate */
						pEntry->MaxHTPhyMode.field.MCS = get_ht_max_mcs(pAd, &pAd->StaCfg.DesiredHtPhyInfo.MCSSet[0], &HtCapability.MCSSet[0]);

						if (pAd->StaCfg.DesiredTransmitSetting.field.MCS != MCS_AUTO) {

							DBGPRINT(RT_DEBUG_OFF,
								 ("@@@ pAd->CommonCfg.RegTransmitSetting.field.MCS = %d\n",
								  pAd->StaCfg.DesiredTransmitSetting.field.MCS));
							set_ht_fixed_mcs(pAd, pEntry, pAd->StaCfg.DesiredTransmitSetting.field.MCS, pAd->StaCfg.HTPhyMode.field.MCS);
						}

						pEntry->MaxHTPhyMode.field.STBC = (HtCapability.HtCapInfo.RxSTBC & (pAd->CommonCfg.DesiredHtPhy.TxSTBC));
						pEntry->MpduDensity = HtCapability.HtCapParm.MpduDensity;
						pEntry->MaxRAmpduFactor = HtCapability.HtCapParm.MaxRAmpduFactor;
						pEntry->MmpsMode = (u8) HtCapability.HtCapInfo.MimoPs;
						pEntry->AMsduSize = (u8) HtCapability.HtCapInfo.AMsduSize;

						pEntry->HTPhyMode.word = pEntry->MaxHTPhyMode.word;

						if (HtCapability.HtCapInfo.ShortGIfor20)
							CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_SGI20_CAPABLE);
						if (HtCapability.HtCapInfo.ShortGIfor40)
							CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_SGI40_CAPABLE);
						if (HtCapability.HtCapInfo.TxSTBC)
							CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_TxSTBC_CAPABLE);
						if (HtCapability.HtCapInfo.RxSTBC)
							CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_RxSTBC_CAPABLE);
						if (HtCapability.ExtHtCapInfo.PlusHTC)
							CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_HTC_CAPABLE);
						if (pAd->CommonCfg.bRdg
						    && HtCapability.ExtHtCapInfo.RDGSupport)
							CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_RDG_CAPABLE);
						if (HtCapability.ExtHtCapInfo.MCSFeedback == 0x03)
							CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_MCSFEEDBACK_CAPABLE);

						memmove(&pEntry->HTCapability,
							       &HtCapability, sizeof(HT_CAPABILITY_IE));
					}
#endif /* DOT11_N_SUPPORT */

					pEntry->HTPhyMode.word = pEntry->MaxHTPhyMode.word;
					pEntry->CurrTxRate = pEntry->MaxSupportedRate;
					CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_WMM_CAPABLE);

					if (pAd->StaCfg.bAutoTxRateSwitch == true) {
						u8 TableSize = 0;

						MlmeSelectTxRateTable(pAd,
								      pEntry,
								      &pEntry->pTable,
								      &TableSize,
								      &pEntry->CurrTxRateIndex);
						pEntry->bAutoTxRateSwitch = true;
					} else {
						pEntry->HTPhyMode.field.MODE = pAd->StaCfg.HTPhyMode.field.MODE;
						pEntry->HTPhyMode.field.MCS = pAd->StaCfg.HTPhyMode.field.MCS;
						pEntry->bAutoTxRateSwitch = false;

						RTMPUpdateLegacyTxSetting((u8) pAd->StaCfg.DesiredTransmitSetting.field.FixedTxMode, pEntry);
					}
					pEntry->RateLen = SupportedRatesLen;

					break;
				}
			}
		}
		StatusCode = MLME_SUCCESS;

		/* can not find in table, create a new one */
		if (i < 0) {
			StatusCode = MLME_QOS_UNSPECIFY;
			DBGPRINT(RT_DEBUG_ERROR,
				 ("DLS - PeerDlsReqAction() DLSEntry table full(only can support %d DLS session) \n",
				  MAX_NUM_OF_DLS_ENTRY -
				  MAX_NUM_OF_INIT_DLS_ENTRY));
		} else {
			DBGPRINT(RT_DEBUG_TRACE,
				 ("DLS - PeerDlsReqAction() use entry(%d) %02x:%02x:%02x:%02x:%02x:%02x\n",
				  i, SA[0], SA[1], SA[2], SA[3], SA[4], SA[5]));
		}
	}

	ActHeaderInit(pAd, &DlsRspHdr, pAd->CommonCfg.Bssid,
		      pAd->CurrentAddress, pAd->CommonCfg.Bssid);

	/* Build basic frame first */
	if (StatusCode == MLME_SUCCESS) {
		MakeOutgoingFrame(pOutBuffer, &FrameLen,
				  sizeof (HEADER_802_11), &DlsRspHdr,
				  1, &Category,
				  1, &Action,
				  2, &StatusCode,
				  6, SA,
				  6, pAd->CurrentAddress,
				  2, &pAd->StaActive.CapabilityInfo,
				  1, &SupRateIe,
				  1, &pAd->MlmeAux.SupRateLen,
				  pAd->MlmeAux.SupRateLen, pAd->MlmeAux.SupRate,
				  END_OF_ARGS);

		if (pAd->MlmeAux.ExtRateLen != 0) {
			MakeOutgoingFrame(pOutBuffer + FrameLen, &tmp,
					  1, &ExtRateIe,
					  1, &pAd->MlmeAux.ExtRateLen,
					  pAd->MlmeAux.ExtRateLen,
					  pAd->MlmeAux.ExtRate, END_OF_ARGS);
			FrameLen += tmp;
		}
#ifdef DOT11_N_SUPPORT
		if (WMODE_CAP_N(pAd->CommonCfg.PhyMode)) {
			u8 HtLen;

#ifdef RT_BIG_ENDIAN
			HT_CAPABILITY_IE HtCapabilityTmp;
#endif

			/* add HT Capability IE */
			HtLen = sizeof (HT_CAPABILITY_IE);
#ifndef RT_BIG_ENDIAN
			MakeOutgoingFrame(pOutBuffer + FrameLen, &tmp,
					  1, &HtCapIe,
					  1, &HtLen,
					  HtLen, &pAd->CommonCfg.HtCapability,
					  END_OF_ARGS);
#else
			memmove(&HtCapabilityTmp, &pAd->CommonCfg.HtCapability, HtLen);
			*(USHORT *) (&HtCapabilityTmp.HtCapInfo) = SWAP16(*(USHORT *) (&HtCapabilityTmp.HtCapInfo));
			*(USHORT *) (&HtCapabilityTmp.ExtHtCapInfo) = SWAP16(*(USHORT *) (&HtCapabilityTmp.ExtHtCapInfo));

			MakeOutgoingFrame(pOutBuffer + FrameLen, &tmp,
					  1, &HtCapIe,
					  1, &HtLen,
					  HtLen, &HtCapabilityTmp, END_OF_ARGS);
#endif
			FrameLen = FrameLen + tmp;
		}
#endif /* DOT11_N_SUPPORT */

		if (pDLS && (pDLS->Status != DLS_FINISH)) {
			RTMPCancelTimer(&pDLS->Timer, &TimerCancelled);
			Timeout = DLS_TIMEOUT;
			RTMPSetTimer(&pDLS->Timer, Timeout);
		}
	} else {
		MakeOutgoingFrame(pOutBuffer, &FrameLen,
				  sizeof (HEADER_802_11), &DlsRspHdr,
				  1, &Category,
				  1, &Action,
				  2, &StatusCode,
				  6, SA, 6, pAd->CurrentAddress, END_OF_ARGS);
	}

	MiniportMMRequest(pAd, QID_AC_BE, pOutBuffer, FrameLen);
	kfree(pOutBuffer);
}

/*
    ==========================================================================
    Description:

	IRQL = DISPATCH_LEVEL

    ==========================================================================
 */
void PeerDlsRspAction(
	IN struct rtmp_adapter *pAd,
	IN MLME_QUEUE_ELEM *Elem)
{
	USHORT CapabilityInfo;
	u8 DA[ETH_ALEN], SA[ETH_ALEN];
	USHORT StatusCode;
	SHORT i;
	bool TimerCancelled;
	u8 MaxSupportedRateIn500Kbps = 0;
	u8 SupportedRatesLen;
	u8 SupportedRates[MAX_LEN_OF_SUPPORTED_RATES];
	u8 HtCapabilityLen;
	HT_CAPABILITY_IE HtCapability;

	if (!pAd->CommonCfg.bDLSCapable)
		return;

	if (!INFRA_ON(pAd))
		return;

	if (!PeerDlsRspSanity
	    (pAd, Elem->Msg, Elem->MsgLen, DA, SA, &CapabilityInfo, &StatusCode,
	     &SupportedRatesLen, &SupportedRates[0], &HtCapabilityLen,
	     &HtCapability))
		return;

	/* supported rates array may not be sorted. sort it and find the maximum rate */
	MaxSupportedRateIn500Kbps = dot11_max_sup_rate(SupportedRatesLen, &SupportedRates[0], 0, NULL);

	DBGPRINT(RT_DEBUG_TRACE,
		 ("DLS - PeerDlsRspAction() from %02x:%02x:%02x:%02x:%02x:%02x with StatusCode=%d, CapabilityInfo=0x%x\n",
		  PRINT_MAC(SA), StatusCode,
		  CapabilityInfo));

	for (i = 0; i < MAX_NUM_OF_INIT_DLS_ENTRY; i++) {
		if (pAd->StaCfg.DLSEntry[i].Valid
		    && MAC_ADDR_EQUAL(SA, pAd->StaCfg.DLSEntry[i].MacAddr)) {
			if (StatusCode == MLME_SUCCESS) {
				MAC_TABLE_ENTRY *pEntry;
				u8 MaxSupportedRate = RATE_11;

				pEntry = MacTableInsertDlsEntry(pAd, SA, i);

				MaxSupportedRate = dot11_2_ra_rate(MaxSupportedRateIn500Kbps);
				pEntry->MaxSupportedRate = min(pAd->CommonCfg.MaxTxRate, MaxSupportedRate);

				set_entry_phy_cfg(pAd, pEntry);

				pEntry->MaxHTPhyMode.field.BW = BW_20;
				pEntry->MinHTPhyMode.field.BW = BW_20;

#ifdef DOT11_N_SUPPORT
				pEntry->HTCapability.MCSSet[0] = 0;
				pEntry->HTCapability.MCSSet[1] = 0;

				/* If this Entry supports 802.11n, upgrade to HT rate. */
				if ((HtCapabilityLen != 0) && WMODE_CAP_N(pAd->CommonCfg.PhyMode))
				{
					DBGPRINT(RT_DEBUG_OFF,
						 ("DLS - PeerDlsRspAction Receive Peer HT Capable STA from %02x:%02x:%02x:%02x:%02x:%02x\n",
						  PRINT_MAC(SA)));

					ht_mode_adjust(pAd, pEntry, &HtCapability, &pAd->CommonCfg.DesiredHtPhy);
					/* find max fixed rate */
					pEntry->MaxHTPhyMode.field.MCS = get_ht_max_mcs(pAd, &pAd->StaCfg.DesiredHtPhyInfo.MCSSet[0], &HtCapability.MCSSet[0]);

					if (pAd->StaCfg.DesiredTransmitSetting.field.MCS != MCS_AUTO)
						set_ht_fixed_mcs(pAd, pEntry, pAd->StaCfg.DesiredTransmitSetting.field.MCS, pAd->StaCfg.HTPhyMode.field.MCS);

					pEntry->MaxHTPhyMode.field.STBC = (HtCapability.HtCapInfo.RxSTBC & (pAd->CommonCfg.DesiredHtPhy.TxSTBC));
					pEntry->MpduDensity = HtCapability.HtCapParm.MpduDensity;
					pEntry->MaxRAmpduFactor = HtCapability.HtCapParm.MaxRAmpduFactor;
					pEntry->MmpsMode = (u8) HtCapability.HtCapInfo.MimoPs;
					pEntry->AMsduSize = (u8) HtCapability.HtCapInfo.AMsduSize;
					pEntry->HTPhyMode.word = pEntry->MaxHTPhyMode.word;

					if (HtCapability.HtCapInfo.ShortGIfor20)
						CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_SGI20_CAPABLE);
					if (HtCapability.HtCapInfo.ShortGIfor40)
						CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_SGI40_CAPABLE);
					if (HtCapability.HtCapInfo.TxSTBC)
						CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_TxSTBC_CAPABLE);
					if (HtCapability.HtCapInfo.RxSTBC)
						CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_RxSTBC_CAPABLE);
					if (HtCapability.ExtHtCapInfo.PlusHTC)
						CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_HTC_CAPABLE);
					if (pAd->CommonCfg.bRdg
					    && HtCapability.ExtHtCapInfo.RDGSupport)
						CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_RDG_CAPABLE);
					if (HtCapability.ExtHtCapInfo.MCSFeedback == 0x03)
						CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_MCSFEEDBACK_CAPABLE);

					memmove(&pEntry->HTCapability,
						       &HtCapability,
						       sizeof(HT_CAPABILITY_IE));
				}
#endif /* DOT11_N_SUPPORT */
				pEntry->HTPhyMode.word = pEntry->MaxHTPhyMode.word;
				pEntry->CurrTxRate = pEntry->MaxSupportedRate;
				CLIENT_STATUS_SET_FLAG(pEntry,
						       fCLIENT_STATUS_WMM_CAPABLE);

				if (pAd->StaCfg.bAutoTxRateSwitch == true) {
					u8 TableSize = 0;

					MlmeSelectTxRateTable(pAd, pEntry,
							      &pEntry->pTable,
							      &TableSize,
							      &pEntry->CurrTxRateIndex);
					pEntry->bAutoTxRateSwitch = true;
				} else {
					pEntry->HTPhyMode.field.MODE = pAd->StaCfg.HTPhyMode.field.MODE;
					pEntry->HTPhyMode.field.MCS = pAd->StaCfg.HTPhyMode.field.MCS;
					pEntry->bAutoTxRateSwitch = false;

					RTMPUpdateLegacyTxSetting((u8) pAd->StaCfg.DesiredTransmitSetting.field.FixedTxMode,
								  pEntry);
				}
				pEntry->RateLen = SupportedRatesLen;

				if (pAd->StaCfg.AuthMode >= Ndis802_11AuthModeWPA) {
					/* If support WPA or WPA2, start STAKey hand shake, */
					/* If failed hand shake, just tear down peer DLS */
					if (RTMPSendSTAKeyRequest
					    (pAd,
					     pAd->StaCfg.DLSEntry[i].MacAddr) != NDIS_STATUS_SUCCESS) {
						MLME_DLS_REQ_STRUCT MlmeDlsReq;
						USHORT reason = REASON_QOS_CIPHER_NOT_SUPPORT;

						DlsParmFill(pAd, &MlmeDlsReq,
							    &pAd->StaCfg.DLSEntry[i],
							    reason);
						MlmeEnqueue(pAd,
							    DLS_STATE_MACHINE,
							    MT2_MLME_DLS_TEAR_DOWN,
							    sizeof(MLME_DLS_REQ_STRUCT),
							    &MlmeDlsReq, 0);
						pAd->StaCfg.DLSEntry[i].Status =DLS_NONE;
						pAd->StaCfg.DLSEntry[i].Valid =false;
						DBGPRINT(RT_DEBUG_ERROR,
							 ("DLS - PeerDlsRspAction failed when call RTMPSendSTAKeyRequest \n"));
					} else {
						pAd->StaCfg.DLSEntry[i].Status = DLS_WAIT_KEY;
						DBGPRINT(RT_DEBUG_TRACE,
							 ("DLS - waiting for STAKey handshake procedure\n"));
					}
				} else {
					RTMPCancelTimer(&pAd->StaCfg.DLSEntry[i].Timer, &TimerCancelled);
					pAd->StaCfg.DLSEntry[i].Status = DLS_FINISH;
					DBGPRINT(RT_DEBUG_TRACE,
						 ("DLS - PeerDlsRspAction() from %02x:%02x:%02x:%02x:%02x:%02x Succeed with WEP or no security\n",
						  PRINT_MAC(SA)));
				}

				/*initialize seq no for DLS frames. */
				pAd->StaCfg.DLSEntry[i].Sequence = 0;
				if (HtCapabilityLen != 0)
					pAd->StaCfg.DLSEntry[i].bHTCap = true;
				else
					pAd->StaCfg.DLSEntry[i].bHTCap = false;
			} else {
				/* DLS setup procedure failed. */
				pAd->StaCfg.DLSEntry[i].Status = DLS_NONE;
				pAd->StaCfg.DLSEntry[i].Valid = false;
				RTMPCancelTimer(&pAd->StaCfg.DLSEntry[i].Timer,
						&TimerCancelled);
				DBGPRINT(RT_DEBUG_ERROR,
					 ("DLS - PeerDlsRspAction failed with StatusCode=%d \n",
					  StatusCode));
			}
		}
	}

	if (i >= MAX_NUM_OF_INIT_DLS_ENTRY) {
		DBGPRINT(RT_DEBUG_TRACE,
			 ("DLS - PeerDlsRspAction() update timeout value \n"));
		for (i = (MAX_NUM_OF_DLS_ENTRY - 1);
		     i >= MAX_NUM_OF_INIT_DLS_ENTRY; i--) {
			if (pAd->StaCfg.DLSEntry[i].Valid
			    && MAC_ADDR_EQUAL(SA, pAd->StaCfg.DLSEntry[i].MacAddr)) {
				if (StatusCode == MLME_SUCCESS) {
					MAC_TABLE_ENTRY *pEntry;
					u8 MaxSupportedRate = RATE_11;

					pEntry = MacTableInsertDlsEntry(pAd, SA, i);

					MaxSupportedRate = dot11_2_ra_rate(MaxSupportedRateIn500Kbps);

					pEntry->MaxSupportedRate = min(pAd->CommonCfg.MaxTxRate, MaxSupportedRate);

					set_entry_phy_cfg(pAd, pEntry);

					pEntry->MaxHTPhyMode.field.BW = BW_20;
					pEntry->MinHTPhyMode.field.BW = BW_20;

#ifdef DOT11_N_SUPPORT
					pEntry->HTCapability.MCSSet[0] = 0;
					pEntry->HTCapability.MCSSet[1] = 0;

					/* If this Entry supports 802.11n, upgrade to HT rate. */
					if ((HtCapabilityLen != 0) && WMODE_CAP_N(pAd->CommonCfg.PhyMode)) {
						DBGPRINT(RT_DEBUG_TRACE,
							 ("DLS - PeerDlsRspAction Receive Peer HT Capable STA from %02x:%02x:%02x:%02x:%02x:%02x\n",
							  PRINT_MAC(SA)));

						if ((HtCapability.HtCapInfo.GF) && (pAd->CommonCfg.DesiredHtPhy.GF)) {
							pEntry->MaxHTPhyMode.field.MODE = MODE_HTGREENFIELD;
						} else {
							pEntry->MaxHTPhyMode.field.MODE = MODE_HTMIX;
							pAd->MacTab.fAnyStationNonGF = true;
							pAd->CommonCfg.AddHTInfo.AddHtInfo2.NonGfPresent = 1;
						}

						if ((HtCapability.HtCapInfo.ChannelWidth) && (pAd->CommonCfg.DesiredHtPhy.ChannelWidth)) {
							pEntry->MaxHTPhyMode.field.BW = BW_40;
							pEntry->MaxHTPhyMode.field.ShortGI = ((pAd->CommonCfg.DesiredHtPhy.ShortGIfor40) & (HtCapability.HtCapInfo.ShortGIfor40));
						} else {
							pEntry->MaxHTPhyMode.field.BW = BW_20;
							pEntry->MaxHTPhyMode.field.ShortGI = ((pAd->CommonCfg.DesiredHtPhy.ShortGIfor20) & (HtCapability.HtCapInfo.ShortGIfor20));
							pAd->MacTab.fAnyStation20Only = true;
						}

						/* find max fixed rate */
						pEntry->MaxHTPhyMode.field.MCS = get_ht_max_mcs(pAd, &pAd->StaCfg.DesiredHtPhyInfo.MCSSet[0], &HtCapability.MCSSet[0]);

						if (pAd->StaCfg.DesiredTransmitSetting.field.MCS != MCS_AUTO) {
							DBGPRINT(RT_DEBUG_OFF,
								 ("@@@ pAd->CommonCfg.RegTransmitSetting.field.MCS = %d\n",
								  pAd->StaCfg.DesiredTransmitSetting.field.MCS));
								set_ht_fixed_mcs(pAd, pEntry, pAd->StaCfg.DesiredTransmitSetting.field.MCS, pAd->StaCfg.HTPhyMode.field.MCS);
						}

						pEntry->MaxHTPhyMode.field.STBC = (HtCapability.HtCapInfo.RxSTBC & (pAd->CommonCfg.DesiredHtPhy.TxSTBC));
						pEntry->MpduDensity = HtCapability.HtCapParm.MpduDensity;
						pEntry->MaxRAmpduFactor = HtCapability.HtCapParm.MaxRAmpduFactor;
						pEntry->MmpsMode = (u8) HtCapability.HtCapInfo.MimoPs;
						pEntry->AMsduSize = (u8) HtCapability.HtCapInfo.AMsduSize;
						pEntry->HTPhyMode.word = pEntry->MaxHTPhyMode.word;

						if (HtCapability.HtCapInfo.ShortGIfor20)
							CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_SGI20_CAPABLE);
						if (HtCapability.HtCapInfo.ShortGIfor40)
							CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_SGI40_CAPABLE);
						if (HtCapability.HtCapInfo.TxSTBC)
							CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_TxSTBC_CAPABLE);
						if (HtCapability.HtCapInfo.RxSTBC)
							CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_RxSTBC_CAPABLE);
						if (HtCapability.ExtHtCapInfo.PlusHTC)
							CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_HTC_CAPABLE);
						if (pAd->CommonCfg.bRdg && HtCapability.ExtHtCapInfo.RDGSupport)
							CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_RDG_CAPABLE);
						if (HtCapability.ExtHtCapInfo.MCSFeedback == 0x03)
							CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_MCSFEEDBACK_CAPABLE);

						memmove(&pEntry->HTCapability,
							       &HtCapability,
							       sizeof(HT_CAPABILITY_IE));
					}
#endif /* DOT11_N_SUPPORT */

					pEntry->HTPhyMode.word = pEntry->MaxHTPhyMode.word;
					pEntry->CurrTxRate = pEntry->MaxSupportedRate;
					CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_WMM_CAPABLE);

					if (pAd->StaCfg.bAutoTxRateSwitch == true) {
						u8 TableSize = 0;

						MlmeSelectTxRateTable(pAd,
								      pEntry,
								      &pEntry->pTable,
								      &TableSize,
								      &pEntry->CurrTxRateIndex);
						pEntry->bAutoTxRateSwitch = true;
					} else {
						pEntry->HTPhyMode.field.MODE = pAd->StaCfg.HTPhyMode.field.MODE;
						pEntry->HTPhyMode.field.MCS = pAd->StaCfg.HTPhyMode.field.MCS;
						pEntry->bAutoTxRateSwitch = false;

						RTMPUpdateLegacyTxSetting((u8) pAd->StaCfg.DesiredTransmitSetting.field.FixedTxMode, pEntry);
					}
					pEntry->RateLen = SupportedRatesLen;

					if (pAd->StaCfg.AuthMode >= Ndis802_11AuthModeWPA) {
						/* If support WPA or WPA2, start STAKey hand shake, */
						/* If failed hand shake, just tear down peer DLS */
						if (RTMPSendSTAKeyRequest
						    (pAd, pAd->StaCfg.DLSEntry[i].MacAddr) != NDIS_STATUS_SUCCESS) {
							MLME_DLS_REQ_STRUCT MlmeDlsReq;
							USHORT reason = REASON_QOS_CIPHER_NOT_SUPPORT;

							DlsParmFill(pAd,
								    &MlmeDlsReq,
								    &pAd->StaCfg.DLSEntry[i],
								    reason);
							MlmeEnqueue(pAd,
								    DLS_STATE_MACHINE,
								    MT2_MLME_DLS_TEAR_DOWN,
								    sizeof(MLME_DLS_REQ_STRUCT),
								    &MlmeDlsReq,
								    0);
							pAd->StaCfg.DLSEntry[i].Status = DLS_NONE;
							pAd->StaCfg.DLSEntry[i].Valid = false;
							DBGPRINT(RT_DEBUG_ERROR,
								 ("DLS - PeerDlsRspAction failed when call RTMPSendSTAKeyRequest \n"));
						} else {
							pAd->StaCfg.DLSEntry[i].Status = DLS_WAIT_KEY;
							DBGPRINT(RT_DEBUG_TRACE,
								 ("DLS - waiting for STAKey handshake procedure\n"));
						}
					} else {
						RTMPCancelTimer(&pAd->StaCfg.DLSEntry[i].Timer, &TimerCancelled);
						pAd->StaCfg.DLSEntry[i].Status = DLS_FINISH;
						DBGPRINT(RT_DEBUG_TRACE,
							 ("DLS - PeerDlsRspAction() from %02x:%02x:%02x:%02x:%02x:%02x Succeed with WEP or no security\n",
							  PRINT_MAC(SA)));
					}
					pAd->StaCfg.DLSEntry[i].Sequence = 0;
					if (HtCapabilityLen != 0)
						pAd->StaCfg.DLSEntry[i].bHTCap = true;
					else
						pAd->StaCfg.DLSEntry[i].bHTCap = false;
				} else {
					/* DLS setup procedure failed. */
					pAd->StaCfg.DLSEntry[i].Status = DLS_NONE;
					pAd->StaCfg.DLSEntry[i].Valid = false;
					RTMPCancelTimer(&pAd->StaCfg.DLSEntry[i].Timer, &TimerCancelled);
					DBGPRINT(RT_DEBUG_ERROR,
						 ("DLS - PeerDlsRspAction failed with StatusCode=%d \n",
						  StatusCode));
				}
			}
		}
	}
}

/*
    ==========================================================================
    Description:

	IRQL = DISPATCH_LEVEL

    ==========================================================================
 */
void MlmeDlsTearDownAction(
	IN struct rtmp_adapter *pAd,
	IN MLME_QUEUE_ELEM *Elem)
{
	u8 *pOutBuffer = NULL;
	int NStatus;
	ULONG FrameLen = 0;
	u8 Category = CATEGORY_DLS;
	u8 Action = ACTION_DLS_TEARDOWN;
	USHORT ReasonCode = REASON_QOS_UNSPECIFY;
	HEADER_802_11 DlsTearDownHdr;
	PRT_802_11_DLS pDLS;
	bool TimerCancelled;
	u8 i;

	if (!MlmeDlsReqSanity(pAd, Elem->Msg, Elem->MsgLen, &pDLS, &ReasonCode))
		return;

	DBGPRINT(RT_DEBUG_TRACE,
		 ("DLS - MlmeDlsTearDownAction() with ReasonCode=%d \n",
		  ReasonCode));

	NStatus = kmalloc(&pOutBuffer, MGMT_DMA_BUFFER_SIZE);	/*Get an unused nonpaged memory */
	if (NStatus != NDIS_STATUS_SUCCESS) {
		DBGPRINT(RT_DEBUG_ERROR,
			 ("DLS - MlmeDlsTearDownAction() allocate memory failed \n"));
		return;
	}

	ActHeaderInit(pAd, &DlsTearDownHdr, pAd->CommonCfg.Bssid,
		      pAd->CurrentAddress, pAd->CommonCfg.Bssid);

	/* Build basic frame first */
	MakeOutgoingFrame(pOutBuffer, &FrameLen,
			  sizeof (HEADER_802_11), &DlsTearDownHdr,
			  1, &Category,
			  1, &Action,
			  6, &pDLS->MacAddr,
			  6, pAd->CurrentAddress, 2, &ReasonCode, END_OF_ARGS);

	MiniportMMRequest(pAd, QID_AC_BE, pOutBuffer, FrameLen);
	kfree(pOutBuffer);
	RTMPCancelTimer(&pDLS->Timer, &TimerCancelled);

	/* Remove key in local dls table entry */
	for (i = 0; i < MAX_NUM_OF_INIT_DLS_ENTRY; i++) {
		if (MAC_ADDR_EQUAL
		    (pDLS->MacAddr, pAd->StaCfg.DLSEntry[i].MacAddr)) {
			MacTableDeleteDlsEntry(pAd,
					       pAd->StaCfg.DLSEntry[i].MacTabMatchWCID,
					       pAd->StaCfg.DLSEntry[i].MacAddr);
		}
	}

	/* clear peer dls table entry */
	for (i = MAX_NUM_OF_INIT_DLS_ENTRY; i < MAX_NUM_OF_DLS_ENTRY; i++) {
		if (MAC_ADDR_EQUAL
		    (pDLS->MacAddr, pAd->StaCfg.DLSEntry[i].MacAddr)) {
			pAd->StaCfg.DLSEntry[i].Status = DLS_NONE;
			pAd->StaCfg.DLSEntry[i].Valid = false;
			RTMPCancelTimer(&pAd->StaCfg.DLSEntry[i].Timer,
					&TimerCancelled);
			MacTableDeleteDlsEntry(pAd,
					       pAd->StaCfg.DLSEntry[i].MacTabMatchWCID,
					       pAd->StaCfg.DLSEntry[i].MacAddr);
		}
	}
}

/*
    ==========================================================================
    Description:

	IRQL = DISPATCH_LEVEL

    ==========================================================================
 */
void PeerDlsTearDownAction(
	IN struct rtmp_adapter *pAd,
	IN MLME_QUEUE_ELEM *Elem)
{
	u8 DA[ETH_ALEN], SA[ETH_ALEN];
	USHORT ReasonCode;
	UINT i;
	bool TimerCancelled;

	if (!pAd->CommonCfg.bDLSCapable)
		return;

	if (!INFRA_ON(pAd))
		return;

	if (!PeerDlsTearDownSanity
	    (pAd, Elem->Msg, Elem->MsgLen, DA, SA, &ReasonCode))
		return;

	DBGPRINT(RT_DEBUG_TRACE,
		 ("DLS - PeerDlsTearDownAction() from %02x:%02x:%02x:%02x:%02x:%02x with ReasonCode=%d\n",
		  SA[0], SA[1], SA[2], SA[3], SA[4], SA[5], ReasonCode));

	/* clear local dls table entry */
	for (i = 0; i < MAX_NUM_OF_INIT_DLS_ENTRY; i++) {
		if (pAd->StaCfg.DLSEntry[i].Valid
		    && MAC_ADDR_EQUAL(SA, pAd->StaCfg.DLSEntry[i].MacAddr)) {
			pAd->StaCfg.DLSEntry[i].Status = DLS_NONE;
			pAd->StaCfg.DLSEntry[i].Valid = false;
			RTMPCancelTimer(&pAd->StaCfg.DLSEntry[i].Timer,
					&TimerCancelled);
			/*AsicDelWcidTab(pAd, pAd->StaCfg.DLSEntry[i].MacTabMatchWCID); */
			/*AsicRemovePairwiseKeyEntry(pAd, pAd->StaCfg.DLSEntry[i].MacTabMatchWCID); */
			MacTableDeleteDlsEntry(pAd,
					       pAd->StaCfg.DLSEntry[i].MacTabMatchWCID,
					       pAd->StaCfg.DLSEntry[i].MacAddr);
		}
	}

	/* clear peer dls table entry */
	for (i = MAX_NUM_OF_INIT_DLS_ENTRY; i < MAX_NUM_OF_DLS_ENTRY; i++) {
		if (pAd->StaCfg.DLSEntry[i].Valid
		    && MAC_ADDR_EQUAL(SA, pAd->StaCfg.DLSEntry[i].MacAddr)) {
			pAd->StaCfg.DLSEntry[i].Status = DLS_NONE;
			pAd->StaCfg.DLSEntry[i].Valid = false;
			RTMPCancelTimer(&pAd->StaCfg.DLSEntry[i].Timer,
					&TimerCancelled);
			/*AsicDelWcidTab(pAd, pAd->StaCfg.DLSEntry[i].MacTabMatchWCID); */
			/*AsicRemovePairwiseKeyEntry(pAd, pAd->StaCfg.DLSEntry[i].MacTabMatchWCID); */
			MacTableDeleteDlsEntry(pAd,
					       pAd->StaCfg.DLSEntry[i].MacTabMatchWCID,
					       pAd->StaCfg.DLSEntry[i].MacAddr);
		}
	}
}

/*
    ==========================================================================
    Description:

	IRQL = DISPATCH_LEVEL

    ==========================================================================
 */
void RTMPCheckDLSTimeOut(
	IN struct rtmp_adapter *pAd)
{
	ULONG i;
	MLME_DLS_REQ_STRUCT MlmeDlsReq;
	USHORT reason = REASON_QOS_UNSPECIFY;

	if (!pAd->CommonCfg.bDLSCapable)
		return;

	if (!INFRA_ON(pAd))
		return;

	/* If timeout value is equaled to zero, it means always not be timeout. */

	/* update local dls table entry */
	for (i = 0; i < MAX_NUM_OF_INIT_DLS_ENTRY; i++) {
		if ((pAd->StaCfg.DLSEntry[i].Valid)
		    && (pAd->StaCfg.DLSEntry[i].Status == DLS_FINISH)
		    && (pAd->StaCfg.DLSEntry[i].TimeOut != 0)) {
			pAd->StaCfg.DLSEntry[i].CountDownTimer--;

			if (pAd->StaCfg.DLSEntry[i].CountDownTimer == 0) {
				reason = REASON_QOS_REQUEST_TIMEOUT;
				pAd->StaCfg.DLSEntry[i].Valid = false;
				pAd->StaCfg.DLSEntry[i].Status = DLS_NONE;
				DlsParmFill(pAd, &MlmeDlsReq,
					    &pAd->StaCfg.DLSEntry[i], reason);
				MlmeEnqueue(pAd, DLS_STATE_MACHINE,
					    MT2_MLME_DLS_TEAR_DOWN,
					    sizeof (MLME_DLS_REQ_STRUCT),
					    &MlmeDlsReq, 0);
			}
		}
	}

	/* update peer dls table entry */
	for (i = MAX_NUM_OF_INIT_DLS_ENTRY; i < MAX_NUM_OF_DLS_ENTRY; i++) {
		if ((pAd->StaCfg.DLSEntry[i].Valid)
		    && (pAd->StaCfg.DLSEntry[i].Status == DLS_FINISH)
		    && (pAd->StaCfg.DLSEntry[i].TimeOut != 0)) {
			pAd->StaCfg.DLSEntry[i].CountDownTimer--;

			if (pAd->StaCfg.DLSEntry[i].CountDownTimer == 0) {
				reason = REASON_QOS_REQUEST_TIMEOUT;
				pAd->StaCfg.DLSEntry[i].Valid = false;
				pAd->StaCfg.DLSEntry[i].Status = DLS_NONE;
				DlsParmFill(pAd, &MlmeDlsReq,
					    &pAd->StaCfg.DLSEntry[i], reason);
				MlmeEnqueue(pAd, DLS_STATE_MACHINE,
					    MT2_MLME_DLS_TEAR_DOWN,
					    sizeof (MLME_DLS_REQ_STRUCT),
					    &MlmeDlsReq, 0);
			}
		}
	}
}

/*
    ==========================================================================
    Description:

	IRQL = DISPATCH_LEVEL

    ==========================================================================
 */
bool RTMPRcvFrameDLSCheck(
	IN struct rtmp_adapter*pAd,
	IN PHEADER_802_11 pHeader,
	IN ULONG Len,
	IN RXD_STRUC *pRxD)
{
	ULONG i;
	bool bFindEntry = false;
	bool bSTAKeyFrame = false;
	PEAPOL_PACKET pEap;
	u8 *pProto, pAddr = NULL;
	u8 *pSTAKey = NULL;
	u8 ZeroReplay[LEN_KEY_DESC_REPLAY];
	u8 Mic[16], OldMic[16];
	u8 digest[80];
	u8 DlsPTK[80];
	u8 temp[64];
	bool TimerCancelled;
	CIPHER_KEY PairwiseKey;

	if (!pAd->CommonCfg.bDLSCapable)
		return bSTAKeyFrame;

	if (!INFRA_ON(pAd))
		return bSTAKeyFrame;

	if (Len < LENGTH_802_11 + 6 + 2)	/* LENGTH_802_11 + LLC + EAPOL protocol type */
		return bSTAKeyFrame;

	pProto = (u8 *) pHeader + LENGTH_802_11;

	if ((pHeader->FC.SubType & 0x08))
		pProto += 2;	/* QOS Control field */

	/* Skip 4-bytes for HTC */
	if (pHeader->FC.Order
	    && (!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_AGGREGATION_INUSED))) {
		pProto += 4;
	}

	/* L2PAD bit on will pad 2 bytes at LLC */
	if (pRxD->L2PAD) {
		pProto += 2;
	}

	pProto += 6;		/* 0xAA 0xAA 0xAA 0x00 0x00 0x00 */

	if ((!(pHeader->FC.SubType & 0x08))
	    && (!RTMPEqualMemory(EAPOL, pProto, 2)))
		return bSTAKeyFrame;

	pAddr = pHeader->Addr2;

	if (RTMPEqualMemory(EAPOL, pProto, 2)
	    && (pAd->StaCfg.AuthMode >= Ndis802_11AuthModeWPA)) {
		pEap = (PEAPOL_PACKET) (pProto + 2);

		DBGPRINT(RT_DEBUG_TRACE,
			 ("DLS - Sniff Len=%ld, DataLen=%d, KeyMic=%d, Install=%d, KeyAck=%d, Secure=%d, EKD_DL=%d, Error=%d, Request=%d\n",
			  Len,
			  (LENGTH_802_11 + 6 + 2 + 2 +
			   MIN_LEN_OF_EAPOL_KEY_MSG + 16),
			  pEap->KeyDesc.KeyInfo.KeyMic,
			  pEap->KeyDesc.KeyInfo.Install,
			  pEap->KeyDesc.KeyInfo.KeyAck,
			  pEap->KeyDesc.KeyInfo.Secure,
			  pEap->KeyDesc.KeyInfo.EKD_DL,
			  pEap->KeyDesc.KeyInfo.Error,
			  pEap->KeyDesc.KeyInfo.Request));

		if ((Len >=
		     (LENGTH_802_11 + 6 + 2 + 2 + MIN_LEN_OF_EAPOL_KEY_MSG +
		      16)) && pEap->KeyDesc.KeyInfo.KeyMic
		    && pEap->KeyDesc.KeyInfo.Install
		    && pEap->KeyDesc.KeyInfo.KeyAck
		    && pEap->KeyDesc.KeyInfo.Secure
		    && pEap->KeyDesc.KeyInfo.EKD_DL
		    && !pEap->KeyDesc.KeyInfo.Error
		    && !pEap->KeyDesc.KeyInfo.Request) {
			/* First validate replay counter, only accept message with larger replay counter */
			/* Let equal pass, some AP start with all zero replay counter */
			memset(ZeroReplay, 0, LEN_KEY_DESC_REPLAY);
			if ((RTMPCompareMemory
			     (pEap->KeyDesc.ReplayCounter,
			      pAd->StaCfg.ReplayCounter,
			      LEN_KEY_DESC_REPLAY) != 1)
			    &&
			    (RTMPCompareMemory
			     (pEap->KeyDesc.ReplayCounter, ZeroReplay,
			      LEN_KEY_DESC_REPLAY) != 0))
				return bSTAKeyFrame;

			/*memmove(pAd->StaCfg.ReplayCounter, pEap->KeyDesc.ReplayCounter, LEN_KEY_DESC_REPLAY); */
			memmove(pAd->StaCfg.DlsReplayCounter,
				       pEap->KeyDesc.ReplayCounter,
				       LEN_KEY_DESC_REPLAY);
			DBGPRINT(RT_DEBUG_TRACE,
				 ("DLS - Sniff replay counter (%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x) Len=%ld, KeyDataLen=%d\n",
				  pAd->StaCfg.ReplayCounter[0],
				  pAd->StaCfg.ReplayCounter[1],
				  pAd->StaCfg.ReplayCounter[2],
				  pAd->StaCfg.ReplayCounter[3],
				  pAd->StaCfg.ReplayCounter[4],
				  pAd->StaCfg.ReplayCounter[5],
				  pAd->StaCfg.ReplayCounter[6],
				  pAd->StaCfg.ReplayCounter[7], Len,
				  pEap->KeyDesc.KeyData[1]));

			/* put these code segment to get the replay counter */
			if (pAd->StaCfg.PortSecured ==
			    WPA_802_1X_PORT_NOT_SECURED)
				return bSTAKeyFrame;

			/* Check MIC value */
			/* Save the MIC and replace with zero */
			/* use proprietary PTK */
			memset(temp, 0, 64);
			memmove(temp, "IEEE802.11 WIRELESS ACCESS POINT", 32);
			WpaDerivePTK(pAd, temp, temp, pAd->CommonCfg.Bssid,
				     temp, pAd->CurrentAddress, DlsPTK, LEN_PTK);

			memmove(OldMic, pEap->KeyDesc.KeyMic,
				       LEN_KEY_DESC_MIC);
			memset(pEap->KeyDesc.KeyMic, 0, LEN_KEY_DESC_MIC);
			if (pAd->StaCfg.WepStatus == Ndis802_11Encryption3Enabled) {
				/* AES */
				RT_HMAC_SHA1(DlsPTK, LEN_PTK_KCK, (u8 *) pEap,
					     pEap->Body_Len[1] + 4, digest,
					     SHA1_DIGEST_SIZE);
				memmove(Mic, digest, LEN_KEY_DESC_MIC);
			} else {
				RT_HMAC_MD5(DlsPTK, LEN_PTK_KCK, (u8 *) pEap,
					    pEap->Body_Len[1] + 4, Mic,
					    MD5_DIGEST_SIZE);
			}

			if (memcmp(OldMic, Mic, LEN_KEY_DESC_MIC) != 0) {
				DBGPRINT(RT_DEBUG_ERROR,
					 ("MIC Different in Msg1 of STAKey handshake! \n"));
				return bSTAKeyFrame;
			} else
				DBGPRINT(RT_DEBUG_TRACE,
					 ("MIC VALID in Msg1 of STAKey handshake! \n"));
			if ((pEap->KeyDesc.KeyData[0] == 0xDD)
			    && (pEap->KeyDesc.KeyData[2] == 0x00)
			    && (pEap->KeyDesc.KeyData[3] == 0x0C)
			    && (pEap->KeyDesc.KeyData[4] == 0x43)
			    && (pEap->KeyDesc.KeyData[5] == 0x02)) {
				pAddr = pEap->KeyDesc.KeyData + 8;	/* Tpe(1), Len(1), OUI(3), DataType(1), Reserved(2) */
				pSTAKey = pEap->KeyDesc.KeyData + 14;	/* Tpe(1), Len(1), OUI(3), DataType(1), Reserved(2), STAKey_Mac_Addr(6) */

				DBGPRINT(RT_DEBUG_TRACE,
					 ("DLS - Receive STAKey Message-1 from %02x:%02x:%02x:%02x:%02x:%02x Len=%ld, KeyDataLen=%d\n",
					  PRINT_MAC(pAddr), Len,
					  pEap->KeyDesc.KeyData[1]));

				bSTAKeyFrame = true;
			}

		}
		else if (Len >= (LENGTH_802_11 + 6 + 2 + 2 + MIN_LEN_OF_EAPOL_KEY_MSG))
		{
			memmove(pAd->StaCfg.DlsReplayCounter,
				       pEap->KeyDesc.ReplayCounter,
				       LEN_KEY_DESC_REPLAY);
			DBGPRINT(RT_DEBUG_TRACE,
				 ("DLS - Sniff replay counter 2(%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x) Len=%ld, KeyDataLen=%d\n",
				  pAd->StaCfg.ReplayCounter[0],
				  pAd->StaCfg.ReplayCounter[1],
				  pAd->StaCfg.ReplayCounter[2],
				  pAd->StaCfg.ReplayCounter[3],
				  pAd->StaCfg.ReplayCounter[4],
				  pAd->StaCfg.ReplayCounter[5],
				  pAd->StaCfg.ReplayCounter[6],
				  pAd->StaCfg.ReplayCounter[7], Len,
				  pEap->KeyDesc.KeyData[1]));
		}
	}

	/* If timeout value is equaled to zero, it means always not be timeout. */
	/* update local dls table entry */
	for (i = 0; i < MAX_NUM_OF_INIT_DLS_ENTRY; i++) {
		if (pAd->StaCfg.DLSEntry[i].Valid
		    && MAC_ADDR_EQUAL(pAddr, pAd->StaCfg.DLSEntry[i].MacAddr)) {
			if (bSTAKeyFrame) {
				PMAC_TABLE_ENTRY pEntry;

				/* STAKey frame, add pairwise key table */
				pAd->StaCfg.DLSEntry[i].Status = DLS_FINISH;
				RTMPCancelTimer(&pAd->StaCfg.DLSEntry[i].Timer,
						&TimerCancelled);

				PairwiseKey.KeyLen = LEN_TK;
				memmove(PairwiseKey.Key, &pSTAKey[0], LEN_TK);
				memmove(PairwiseKey.TxMic, &pSTAKey[16], LEN_TKIP_MIC);
				memmove(PairwiseKey.RxMic, &pSTAKey[24], LEN_TKIP_MIC);

				if (pAd->StaCfg.PairCipher == Ndis802_11Encryption2Enabled)
					PairwiseKey.CipherAlg = CIPHER_TKIP;
				else if (pAd->StaCfg.PairCipher == Ndis802_11Encryption3Enabled)
					PairwiseKey.CipherAlg = CIPHER_AES;

				pEntry = DlsEntryTableLookup(pAd,
							pAd->StaCfg.DLSEntry[i].MacAddr, true);

				/* Add Pair-wise key to Asic */
				RTMP_ASIC_PAIRWISE_KEY_TABLE(pAd,
							     (u8) pAd->StaCfg.DLSEntry[i].MacTabMatchWCID,
							     &PairwiseKey);

				RTMP_SET_WCID_SEC_INFO(pAd,
						       BSS0,
						       0,
						       PairwiseKey.CipherAlg,
						       (u8) pAd->StaCfg.DLSEntry[i].MacTabMatchWCID,
						       PAIRWISEKEYTABLE);

				memmove(&pEntry->PairwiseKey,
					       &PairwiseKey,
					       sizeof (CIPHER_KEY));
				DBGPRINT(RT_DEBUG_TRACE,
					 ("DLS - Receive STAKey Message-1 (Peer STA MAC Address STAKey) \n"));

				RTMPSendSTAKeyHandShake(pAd, pAd->StaCfg.DLSEntry[i].MacAddr);

				DBGPRINT(RT_DEBUG_TRACE,
					 ("DLS - Finish STAKey handshake procedure (Initiator side)\n"));
			} else {
				/* Data frame, update timeout value */
				if (pAd->StaCfg.DLSEntry[i].Status == DLS_FINISH) {
					pAd->StaCfg.DLSEntry[i].CountDownTimer = pAd->StaCfg.DLSEntry[i].TimeOut;
				}
			}

			bFindEntry = true;
		}
	}

	/* update peer dls table entry */
	for (i = MAX_NUM_OF_INIT_DLS_ENTRY; i < MAX_NUM_OF_DLS_ENTRY; i++) {
		if (pAd->StaCfg.DLSEntry[i].Valid
		    && MAC_ADDR_EQUAL(pAddr, pAd->StaCfg.DLSEntry[i].MacAddr)) {
			if (bSTAKeyFrame) {
				PMAC_TABLE_ENTRY pEntry = NULL;

				/* STAKey frame, add pairwise key table, and send STAkey Msg-2 */
				pAd->StaCfg.DLSEntry[i].Status = DLS_FINISH;
				RTMPCancelTimer(&pAd->StaCfg.DLSEntry[i].Timer, &TimerCancelled);

				PairwiseKey.KeyLen = LEN_TK;
				memmove(PairwiseKey.Key, &pSTAKey[0],
					       LEN_TK);
				memmove(PairwiseKey.TxMic, &pSTAKey[16],
					       LEN_TKIP_MIC);
				memmove(PairwiseKey.RxMic, &pSTAKey[24],
					       LEN_TKIP_MIC);

				/*PairwiseKey.CipherAlg = pAd->SharedKey[BSS0][pAd->StaCfg.DefaultKeyId].CipherAlg; */
				if (pAd->StaCfg.PairCipher == Ndis802_11Encryption2Enabled)
					PairwiseKey.CipherAlg = CIPHER_TKIP;
				else if (pAd->StaCfg.PairCipher == Ndis802_11Encryption3Enabled)
					PairwiseKey.CipherAlg = CIPHER_AES;

				pEntry =
				    DlsEntryTableLookup(pAd,
							pAd->StaCfg.DLSEntry[i].MacAddr, true);
				/*AsicAddKeyEntry(pAd, (USHORT)(i + 2), BSS0, 0, &PairwiseKey, true, true);     // reserve 0 for multicast, 1 for unicast */
				/*AsicUpdateRxWCIDTable(pAd, (USHORT)(i + 2), pAddr); */

				/* Add Pair-wise key to Asic */
				RTMP_ASIC_PAIRWISE_KEY_TABLE(pAd,
							     (u8) pAd->StaCfg.DLSEntry[i].MacTabMatchWCID,
							     &PairwiseKey);

				RTMP_SET_WCID_SEC_INFO(pAd,
						       BSS0,
						       0,
						       PairwiseKey.CipherAlg,
						       (u8) pAd->StaCfg.DLSEntry[i].MacTabMatchWCID,
						       PAIRWISEKEYTABLE);

				memmove(&pEntry->PairwiseKey,
					       &PairwiseKey,
					       sizeof (CIPHER_KEY));
				DBGPRINT(RT_DEBUG_TRACE,
					 ("DLS - Receive STAKey Message-1 (Initiator STA MAC Address STAKey)\n"));

				/* If support WPA or WPA2, start STAKey hand shake, */
				/* If failed hand shake, just tear down peer DLS */
				if (RTMPSendSTAKeyHandShake(pAd, pAddr) != NDIS_STATUS_SUCCESS) {
					MLME_DLS_REQ_STRUCT MlmeDlsReq;
					USHORT reason = REASON_QOS_CIPHER_NOT_SUPPORT;

					pAd->StaCfg.DLSEntry[i].Valid = false;
					pAd->StaCfg.DLSEntry[i].Status = DLS_NONE;
					DlsParmFill(pAd, &MlmeDlsReq,
						    &pAd->StaCfg.DLSEntry[i],
						    reason);
					MlmeEnqueue(pAd, DLS_STATE_MACHINE,
						    MT2_MLME_DLS_TEAR_DOWN,
						    sizeof(MLME_DLS_REQ_STRUCT),
						    &MlmeDlsReq, 0);
				} else {
					DBGPRINT(RT_DEBUG_TRACE,
						 ("DLS - Finish STAKey handshake procedure (Peer side)\n"));
				}
			} else {
				/* Data frame, update timeout value */
				if (pAd->StaCfg.DLSEntry[i].Status == DLS_FINISH) {
					pAd->StaCfg.DLSEntry[i].CountDownTimer = pAd->StaCfg.DLSEntry[i].TimeOut;
				}
			}

			bFindEntry = true;
		}
	}

	return bSTAKeyFrame;
}

/*
	========================================================================

	Routine	Description:
		Check if the frame can be sent through DLS direct link interface

	Arguments:
		pAd		Pointer	to adapter

	Return Value:
		DLS entry index

	Note:

	========================================================================
*/
INT RTMPCheckDLSFrame(
	IN struct rtmp_adapter *pAd,
	IN u8 *pDA)
{
	INT rval = -1;
	INT i;

	if (!pAd->CommonCfg.bDLSCapable)
		return rval;

	if (!INFRA_ON(pAd))
		return rval;

	do {
		/* check local dls table entry */
		for (i = 0; i < MAX_NUM_OF_INIT_DLS_ENTRY; i++) {
			if (pAd->StaCfg.DLSEntry[i].Valid
			    && (pAd->StaCfg.DLSEntry[i].Status == DLS_FINISH)
			    && MAC_ADDR_EQUAL(pDA, pAd->StaCfg.DLSEntry[i].MacAddr)) {
				rval = i;
				break;
			}
		}

		/* check peer dls table entry */
		for (i = MAX_NUM_OF_INIT_DLS_ENTRY; i < MAX_NUM_OF_DLS_ENTRY;
		     i++) {
			if (pAd->StaCfg.DLSEntry[i].Valid
			    && (pAd->StaCfg.DLSEntry[i].Status == DLS_FINISH)
			    && MAC_ADDR_EQUAL(pDA, pAd->StaCfg.DLSEntry[i].MacAddr)) {
				rval = i;
				break;
			}
		}
	} while (false);

	return rval;
}

/*
    ==========================================================================
    Description:

	IRQL = DISPATCH_LEVEL

    ==========================================================================
 */
void RTMPSendDLSTearDownFrame(
	IN struct rtmp_adapter *pAd,
	IN u8 *pDA)
{
	u8 *pOutBuffer = NULL;
	int NStatus;
	HEADER_802_11 DlsTearDownHdr;
	ULONG FrameLen = 0;
	USHORT Reason = REASON_QOS_QSTA_LEAVING_QBSS;
	u8 Category = CATEGORY_DLS;
	u8 Action = ACTION_DLS_TEARDOWN;
	u8 i = 0;

	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RESET_IN_PROGRESS) ||
	    RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS))
		return;

	DBGPRINT(RT_DEBUG_TRACE, ("Send DLS TearDown Frame \n"));

	NStatus = kmalloc(&pOutBuffer, MGMT_DMA_BUFFER_SIZE);	/*Get an unused nonpaged memory */
	if (NStatus != NDIS_STATUS_SUCCESS) {
		DBGPRINT(RT_DEBUG_ERROR,
			 ("ASSOC - RTMPSendDLSTearDownFrame() allocate memory failed \n"));
		return;
	}

	ActHeaderInit(pAd, &DlsTearDownHdr, pAd->CommonCfg.Bssid,
		      pAd->CurrentAddress, pAd->CommonCfg.Bssid);
	MakeOutgoingFrame(pOutBuffer, &FrameLen, sizeof (HEADER_802_11),
			  &DlsTearDownHdr, 1, &Category, 1, &Action, 6, pDA, 6,
			  pAd->CurrentAddress, 2, &Reason, END_OF_ARGS);

	MiniportMMRequest(pAd, 0, pOutBuffer, FrameLen);
	kfree(pOutBuffer);

	/* Remove key in local dls table entry */
	for (i = 0; i < MAX_NUM_OF_INIT_DLS_ENTRY; i++) {
		if (pAd->StaCfg.DLSEntry[i].Valid
		    && (pAd->StaCfg.DLSEntry[i].Status == DLS_FINISH)
		    && MAC_ADDR_EQUAL(pDA, pAd->StaCfg.DLSEntry[i].MacAddr)) {
			MacTableDeleteDlsEntry(pAd,
					       pAd->StaCfg.DLSEntry[i].MacTabMatchWCID,
					       pAd->StaCfg.DLSEntry[i].MacAddr);
		}
	}

	/* Remove key in peer dls table entry */
	for (i = MAX_NUM_OF_INIT_DLS_ENTRY; i < MAX_NUM_OF_DLS_ENTRY; i++) {
		if (pAd->StaCfg.DLSEntry[i].Valid
		    && (pAd->StaCfg.DLSEntry[i].Status == DLS_FINISH)
		    && MAC_ADDR_EQUAL(pDA, pAd->StaCfg.DLSEntry[i].MacAddr)) {
			MacTableDeleteDlsEntry(pAd,
					       pAd->StaCfg.DLSEntry[i].MacTabMatchWCID,
					       pAd->StaCfg.DLSEntry[i].MacAddr);
		}
	}

	DBGPRINT(RT_DEBUG_TRACE,
		 ("Send DLS TearDown Frame and remove key in (i=%d) \n", i));
}

/*
    ==========================================================================
    Description:

	IRQL = DISPATCH_LEVEL

    ==========================================================================
 */
int RTMPSendSTAKeyRequest(
	IN struct rtmp_adapter *pAd,
	IN u8 *pDA)
{
	u8 Header802_3[14];
	int NStatus;
	ULONG FrameLen = 0;
	PEAPOL_PACKET pPacket;
	u8 *mpool;
	u8 Mic[16];
	u8 digest[80];
	u8 *pOutBuffer = NULL;
	struct sk_buff * pNdisPacket;
	u8 temp[64];
	u8 DlsPTK[80];

	DBGPRINT(RT_DEBUG_TRACE,
		 ("DLS - RTMPSendSTAKeyRequest() to %02x:%02x:%02x:%02x:%02x:%02x\n",
		  PRINT_MAC(pDA)));

	pAd->Sequence++;
	MAKE_802_3_HEADER(Header802_3, pAd->CommonCfg.Bssid,
			  pAd->CurrentAddress, EAPOL);

	/* Allocate memory for output */
	kmalloc((u8 **) & mpool, TX_EAPOL_BUFFER);
	if (mpool == NULL) {
		DBGPRINT(RT_DEBUG_ERROR,
			 ("!!!%s : no memory!!!\n", __FUNCTION__));
		return NDIS_STATUS_FAILURE;
	}

	/* Zero message body */
	pPacket = (PEAPOL_PACKET) mpool;
	memset(pPacket, 0, TX_EAPOL_BUFFER);

	pPacket->ProVer = EAPOL_VER;
	pPacket->ProType = EAPOLKey;
	pPacket->Body_Len[1] = sizeof (KEY_DESCRIPTER) + 6 + ETH_ALEN;	/* data field contain KDE andPeer MAC address */

	/* STAKey Message is as EAPOL-Key(1,1,0,0,G/0,0,0, MIC, 0,Peer MAC KDE) */
	if ((pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPA)
	    || (pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPAPSK)) {
		pPacket->KeyDesc.Type = WPA1_KEY_DESC;
	} else if ((pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPA2)
		   || (pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPA2PSK)) {
		pPacket->KeyDesc.Type = WPA2_KEY_DESC;
	}

	/* Key descriptor version */
	pPacket->KeyDesc.KeyInfo.KeyDescVer =
	    (((pAd->StaCfg.PairCipher == Ndis802_11Encryption3Enabled)
	      || (pAd->StaCfg.GroupCipher ==
		  Ndis802_11Encryption3Enabled)) ? (KEY_DESC_AES)
	     : (KEY_DESC_TKIP));

	pPacket->KeyDesc.KeyInfo.KeyMic = 1;
	pPacket->KeyDesc.KeyInfo.Secure = 1;
	pPacket->KeyDesc.KeyInfo.Request = 1;

	pPacket->KeyDesc.KeyDataLen[1] = 12;

	/* use our own OUI to distinguish proprietary with standard. */
	pPacket->KeyDesc.KeyData[0] = 0xDD;
	pPacket->KeyDesc.KeyData[1] = 0x0A;
	pPacket->KeyDesc.KeyData[2] = 0x00;
	pPacket->KeyDesc.KeyData[3] = 0x0C;
	pPacket->KeyDesc.KeyData[4] = 0x43;
	pPacket->KeyDesc.KeyData[5] = 0x03;
	memmove(&pPacket->KeyDesc.KeyData[6], pDA, ETH_ALEN);

	memmove(pPacket->KeyDesc.ReplayCounter,
		       pAd->StaCfg.DlsReplayCounter, LEN_KEY_DESC_REPLAY);

	/* Allocate buffer for transmitting message */
	NStatus = kmalloc(&pOutBuffer, MGMT_DMA_BUFFER_SIZE);
	if (NStatus != NDIS_STATUS_SUCCESS) {
		kfree(mpool);
		return NStatus;
	}
	/* Prepare EAPOL frame for MIC calculation */
	/* Be careful, only EAPOL frame is counted for MIC calculation */
	MakeOutgoingFrame(pOutBuffer, &FrameLen,
			  pPacket->Body_Len[1] + 4, pPacket, END_OF_ARGS);

	/* use proprietary PTK */
	memset(temp, 0, 64);
	memmove(temp, "IEEE802.11 WIRELESS ACCESS POINT", 32);
	WpaDerivePTK(pAd, temp, temp, pAd->CommonCfg.Bssid, temp,
		     pAd->CurrentAddress, DlsPTK, LEN_PTK);

	/* calculate MIC */
	if (pAd->StaCfg.WepStatus == Ndis802_11Encryption3Enabled) {
		/* AES */
		memset(digest, 0, sizeof (digest));
		RT_HMAC_SHA1(DlsPTK, LEN_PTK_KCK, pOutBuffer, FrameLen, digest,
			     SHA1_DIGEST_SIZE);
		memmove(pPacket->KeyDesc.KeyMic, digest,
			       LEN_KEY_DESC_MIC);
	} else {
		memset(Mic, 0, sizeof (Mic));
		RT_HMAC_MD5(DlsPTK, LEN_PTK_KCK, pOutBuffer, FrameLen, Mic,
			    MD5_DIGEST_SIZE);
		memmove(pPacket->KeyDesc.KeyMic, Mic, LEN_KEY_DESC_MIC);
	}

	MakeOutgoingFrame(pOutBuffer, &FrameLen,
			  sizeof (Header802_3), Header802_3,
			  pPacket->Body_Len[1] + 4, pPacket, END_OF_ARGS);

	NStatus =
	    RTMPAllocateNdisPacket(pAd, &pNdisPacket, NULL, 0, pOutBuffer,
				   FrameLen);
	if (NStatus == NDIS_STATUS_SUCCESS) {
		RTMP_SET_PACKET_WCID(pNdisPacket, BSSID_WCID);
		STASendPacket(pAd, pNdisPacket);
		RTMPDeQueuePacket(pAd, false, NUM_OF_TX_RING, MAX_TX_PROCESS);
	}

	kfree(pOutBuffer);
	kfree(mpool);

	DBGPRINT(RT_DEBUG_TRACE,
		 ("RTMPSendSTAKeyRequest- Send STAKey request (NStatus=%x, FrameLen=%ld)\n",
		  NStatus, FrameLen));

	return NStatus;
}

/*
    ==========================================================================
    Description:

	IRQL = DISPATCH_LEVEL

    ==========================================================================
 */
int RTMPSendSTAKeyHandShake(
	IN struct rtmp_adapter *pAd,
	IN u8 *pDA)
{
	u8 Header802_3[14];
	int NStatus;
	ULONG FrameLen = 0;
	PEAPOL_PACKET pPacket;
	u8 *mpool;
	u8 Mic[16];
	u8 digest[80];
	u8 *pOutBuffer = NULL;
	struct sk_buff * pNdisPacket;
	u8 temp[64];
	u8 DlsPTK[80];	/* Due to dirver can not get PTK, use proprietary PTK */

	DBGPRINT(RT_DEBUG_TRACE,
		 ("DLS - RTMPSendSTAKeyHandShake() to %02x:%02x:%02x:%02x:%02x:%02x\n",
		  PRINT_MAC(pDA)));

	pAd->Sequence++;
	MAKE_802_3_HEADER(Header802_3, pAd->CommonCfg.Bssid,
			  pAd->CurrentAddress, EAPOL);

	/* Allocate memory for output */
	kmalloc((u8 **) & mpool, TX_EAPOL_BUFFER);
	if (mpool == NULL) {
		DBGPRINT(RT_DEBUG_ERROR,
			 ("!!!%s : no memory!!!\n", __FUNCTION__));
		return NDIS_STATUS_FAILURE;
	}

	/* Zero message body */
	pPacket = (PEAPOL_PACKET) mpool;
	memset(pPacket, 0, TX_EAPOL_BUFFER);

	pPacket->ProVer = EAPOL_VER;
	pPacket->ProType = EAPOLKey;
	pPacket->Body_Len[1] = sizeof (KEY_DESCRIPTER) + 6 + ETH_ALEN;	/* data field contain KDE and Peer MAC address */

	/* STAKey Message is as EAPOL-Key(1,1,0,0,G/0,0,0, MIC, 0,Peer MAC KDE) */
	if ((pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPA)
	    || (pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPAPSK)) {
		pPacket->KeyDesc.Type = WPA1_KEY_DESC;
	} else if ((pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPA2)
		   || (pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPA2PSK)) {
		pPacket->KeyDesc.Type = WPA2_KEY_DESC;
	}

	/* Key descriptor version */
	pPacket->KeyDesc.KeyInfo.KeyDescVer =
	    (((pAd->StaCfg.PairCipher == Ndis802_11Encryption3Enabled)
	      || (pAd->StaCfg.GroupCipher ==
		  Ndis802_11Encryption3Enabled)) ? (KEY_DESC_AES)
	     : (KEY_DESC_TKIP));

	pPacket->KeyDesc.KeyInfo.KeyMic = 1;
	pPacket->KeyDesc.KeyInfo.Secure = 1;

	pPacket->KeyDesc.KeyDataLen[1] = 12;

	/* use our own OUI to distinguish proprietary with standard. */
	pPacket->KeyDesc.KeyData[0] = 0xDD;
	pPacket->KeyDesc.KeyData[1] = 0x0A;
	pPacket->KeyDesc.KeyData[2] = 0x00;
	pPacket->KeyDesc.KeyData[3] = 0x0C;
	pPacket->KeyDesc.KeyData[4] = 0x43;
	pPacket->KeyDesc.KeyData[5] = 0x03;
	memmove(&pPacket->KeyDesc.KeyData[6], pDA, ETH_ALEN);

	memmove(pPacket->KeyDesc.ReplayCounter,
		       pAd->StaCfg.DlsReplayCounter, LEN_KEY_DESC_REPLAY);

	/* Allocate buffer for transmitting message */
	NStatus = kmalloc(&pOutBuffer, MGMT_DMA_BUFFER_SIZE);
	if (NStatus != NDIS_STATUS_SUCCESS) {
		kfree(mpool);
		return NStatus;
	}
	/* Prepare EAPOL frame for MIC calculation */
	/* Be careful, only EAPOL frame is counted for MIC calculation */
	MakeOutgoingFrame(pOutBuffer, &FrameLen,
			  pPacket->Body_Len[1] + 4, pPacket, END_OF_ARGS);

	/* use proprietary PTK */
	memset(temp, 0, 64);
	memmove(temp, "IEEE802.11 WIRELESS ACCESS POINT", 32);
	WpaDerivePTK(pAd, temp, temp, pAd->CommonCfg.Bssid, temp,
		     pAd->CurrentAddress, DlsPTK, LEN_PTK);

	/* calculate MIC */
	if (pAd->StaCfg.WepStatus == Ndis802_11Encryption3Enabled) {
		/* AES */
		memset(digest, 0, sizeof (digest));
		RT_HMAC_SHA1(DlsPTK, LEN_PTK_KCK, pOutBuffer, FrameLen, digest,
			     SHA1_DIGEST_SIZE);
		memmove(pPacket->KeyDesc.KeyMic, digest,
			       LEN_KEY_DESC_MIC);
	} else {
		memset(Mic, 0, sizeof (Mic));
		RT_HMAC_MD5(DlsPTK, LEN_PTK_KCK, pOutBuffer, FrameLen, Mic,
			    MD5_DIGEST_SIZE);
		memmove(pPacket->KeyDesc.KeyMic, Mic, LEN_KEY_DESC_MIC);
	}

	MakeOutgoingFrame(pOutBuffer, &FrameLen,
			  sizeof (Header802_3), Header802_3,
			  pPacket->Body_Len[1] + 4, pPacket, END_OF_ARGS);

	NStatus =
	    RTMPAllocateNdisPacket(pAd, &pNdisPacket, NULL, 0, pOutBuffer,
				   FrameLen);
	if (NStatus == NDIS_STATUS_SUCCESS) {
		RTMP_SET_PACKET_WCID(pNdisPacket, BSSID_WCID);
		STASendPacket(pAd, pNdisPacket);
		RTMPDeQueuePacket(pAd, false, NUM_OF_TX_RING, MAX_TX_PROCESS);
	}

	kfree(pOutBuffer);
	kfree(mpool);

	DBGPRINT(RT_DEBUG_TRACE,
		 ("RTMPSendSTAKeyHandShake- Send STAKey Message-2 (NStatus=%x, FrameLen=%ld)\n",
		  NStatus, FrameLen));

	return NStatus;
}

void DlsTimeoutAction(
	IN void *SystemSpecific1,
	IN void *FunctionContext,
	IN void *SystemSpecific2,
	IN void *SystemSpecific3)
{
	MLME_DLS_REQ_STRUCT MlmeDlsReq;
	USHORT reason;
	PRT_802_11_DLS pDLS = (PRT_802_11_DLS) FunctionContext;
	struct rtmp_adapter *pAd = NULL;

	if (pDLS == NULL)
		return;

	pAd = (struct rtmp_adapter *)pDLS->pAd;

	DBGPRINT(RT_DEBUG_TRACE,
		 ("DlsTimeout - Tear down DLS links (%02x:%02x:%02x:%02x:%02x:%02x)\n",
		  PRINT_MAC(pDLS->MacAddr)));

	if ((pDLS) && (pDLS->Valid)) {
		reason = REASON_QOS_REQUEST_TIMEOUT;
		pDLS->Valid = false;
		pDLS->Status = DLS_NONE;
		DlsParmFill(pAd, &MlmeDlsReq, pDLS, reason);
		MlmeEnqueue(pAd, DLS_STATE_MACHINE, MT2_MLME_DLS_TEAR_DOWN,
			    sizeof (MLME_DLS_REQ_STRUCT), &MlmeDlsReq, 0);
		RTMP_MLME_HANDLER(pAd);
	}
}

/*
================================================================
Description : because DLS and CLI share the same WCID table in ASIC.
Mesh entry also insert to pAd->MacTab.content[].
Also fills the pairwise key.
Because front MAX_AID_BA entries have direct mapping to BAEntry, which is only used as CLI. So we insert Dls
from index MAX_AID_BA.
================================================================
*/
MAC_TABLE_ENTRY *MacTableInsertDlsEntry(
	IN struct rtmp_adapter *pAd,
	IN u8 *pAddr,
	IN UINT DlsEntryIdx)
{
	PMAC_TABLE_ENTRY pEntry = NULL;

	DBGPRINT(RT_DEBUG_TRACE, ("====> MacTableInsertDlsEntry\n"));
	/* if FULL, return */
	if (pAd->MacTab.Size >= MAX_LEN_OF_MAC_TABLE)
		return NULL;

	do {
		if ((pEntry = DlsEntryTableLookup(pAd, pAddr, true)) != NULL)
			break;

		/* allocate one MAC entry */
		pEntry =
		    MacTableInsertEntry(pAd, pAddr,
					DlsEntryIdx + MIN_NET_DEVICE_FOR_DLS,
					OPMODE_STA,
					true);
		if (pEntry) {
			pAd->StaCfg.DLSEntry[DlsEntryIdx].MacTabMatchWCID = pEntry->Aid;
			pEntry->MatchDlsEntryIdx = DlsEntryIdx;
			pEntry->AuthMode = pAd->StaCfg.AuthMode;
			pEntry->WepStatus = pAd->StaCfg.WepStatus;
			pEntry->PortSecured = WPA_802_1X_PORT_SECURED;

			DBGPRINT(RT_DEBUG_TRACE,
				 ("MacTableInsertDlsEntry - allocate entry #%d, Total= %d\n",
				  pEntry->Aid, pAd->MacTab.Size));

			/* If legacy WEP is used, set pair-wise cipherAlg into WCID attribute table for this entry */
			if (IS_ENTRY_DLS(pEntry)
			    && (pAd->StaCfg.WepStatus == Ndis802_11WEPEnabled)) {
				u8 KeyIdx = pAd->StaCfg.DefaultKeyId;
				u8 CipherAlg;

				CipherAlg = pAd->SharedKey[BSS0][KeyIdx].CipherAlg;

				RTMPSetWcidSecurityInfo(pAd,
							BSS0,
							pAd->StaCfg.DefaultKeyId,
							pAd->SharedKey[BSS0][pAd->StaCfg.DefaultKeyId].CipherAlg, pEntry->Aid,
							SHAREDKEYTABLE);
			}

			break;
		}
	} while (false);

	DBGPRINT(RT_DEBUG_TRACE, ("<==== MacTableInsertDlsEntry\n"));

	return pEntry;
}

/*
	==========================================================================
	Description:
		Delete all Mesh Entry in pAd->MacTab
	==========================================================================
 */
bool MacTableDeleteDlsEntry(
	IN struct rtmp_adapter *pAd,
	IN USHORT wcid,
	IN u8 *pAddr)
{
	DBGPRINT(RT_DEBUG_TRACE, ("====> MacTableDeleteDlsEntry\n"));

	if (!VALID_WCID(wcid))
		return false;

	MacTableDeleteEntry(pAd, wcid, pAddr);

	DBGPRINT(RT_DEBUG_TRACE, ("<==== MacTableDeleteDlsEntry\n"));

	return true;
}

MAC_TABLE_ENTRY *DlsEntryTableLookup(
	IN struct rtmp_adapter *pAd,
	IN u8 *pAddr,
	IN bool bResetIdelCount)
{
	ULONG HashIdx;
	MAC_TABLE_ENTRY *pEntry = NULL;

	RTMP_SEM_LOCK(&pAd->MacTabLock);
	HashIdx = MAC_ADDR_HASH_INDEX(pAddr);
	pEntry = pAd->MacTab.Hash[HashIdx];

	while (pEntry) {
		if (IS_ENTRY_DLS(pEntry)
		    && MAC_ADDR_EQUAL(pEntry->Addr, pAddr)) {
			if (bResetIdelCount)
				pEntry->NoDataIdleCount = 0;
			break;
		} else
			pEntry = pEntry->pNext;
	}

	RTMP_SEM_UNLOCK(&pAd->MacTabLock);
	return pEntry;
}

MAC_TABLE_ENTRY *DlsEntryTableLookupByWcid(
	IN struct rtmp_adapter *pAd,
	IN u8 wcid,
	IN u8 *pAddr,
	IN bool bResetIdelCount)
{
	ULONG DLsIndex;
	PMAC_TABLE_ENTRY pCurEntry = NULL;
	PMAC_TABLE_ENTRY pEntry = NULL;

	if (!VALID_WCID(wcid))
		return NULL;

	RTMP_SEM_LOCK(&pAd->MacTabLock);

	do {
		pCurEntry = &pAd->MacTab.Content[wcid];

		DLsIndex = 0xff;
		if ((pCurEntry) && IS_ENTRY_DLS(pCurEntry)) {
			DLsIndex = pCurEntry->MatchDlsEntryIdx;
		}

		if (DLsIndex == 0xff)
			break;

		if (MAC_ADDR_EQUAL(pCurEntry->Addr, pAddr)) {
			if (bResetIdelCount)
				pCurEntry->NoDataIdleCount = 0;
			pEntry = pCurEntry;
			break;
		}
	} while (false);

	RTMP_SEM_UNLOCK(&pAd->MacTabLock);

	return pEntry;
}

INT Set_DlsEntryInfo_Display_Proc(
	IN struct rtmp_adapter *pAd,
	IN u8 *arg)
{
	INT i;

	DBGPRINT(RT_DEBUG_OFF, ("\n%-19s%-8s\n", "MAC", "TIMEOUT\n"));
	for (i = 0; i < MAX_NUM_OF_DLS_ENTRY; i++) {
		if ((pAd->StaCfg.DLSEntry[i].Valid)
		    && (pAd->StaCfg.DLSEntry[i].Status == DLS_FINISH)) {
			PMAC_TABLE_ENTRY pEntry =
			    &pAd->MacTab.Content[pAd->StaCfg.DLSEntry[i].MacTabMatchWCID];

			DBGPRINT(RT_DEBUG_OFF,
				 ("%02x:%02x:%02x:%02x:%02x:%02x  ",
				  PRINT_MAC(pAd->StaCfg.DLSEntry[i].MacAddr)));
			DBGPRINT(RT_DEBUG_OFF,
				 ("%-8d\n", pAd->StaCfg.DLSEntry[i].TimeOut));

			DBGPRINT(RT_DEBUG_OFF, ("\n"));
			DBGPRINT(RT_DEBUG_OFF,
				 ("\n%-19s%-4s%-4s%-4s%-4s%-7s%-7s%-7s", "MAC",
				  "AID", "BSS", "PSM", "WMM", "RSSI0", "RSSI1",
				  "RSSI2"));
#ifdef DOT11_N_SUPPORT
			DBGPRINT(RT_DEBUG_OFF,
				 ("%-8s%-10s%-6s%-6s%-6s%-6s", "MIMOPS", "PhMd",
				  "BW", "MCS", "SGI", "STBC"));
#endif /* DOT11_N_SUPPORT */
			DBGPRINT(RT_DEBUG_OFF,
				 ("\n%02X:%02X:%02X:%02X:%02X:%02X  ",
				  pEntry->Addr[0], pEntry->Addr[1],
				  pEntry->Addr[2], pEntry->Addr[3],
				  pEntry->Addr[4], pEntry->Addr[5]));
			DBGPRINT(RT_DEBUG_OFF, ("%-4d", (int)pEntry->Aid));
			DBGPRINT(RT_DEBUG_OFF, ("%-4d", (int)pEntry->apidx));
			DBGPRINT(RT_DEBUG_OFF, ("%-4d", (int)pEntry->PsMode));
			DBGPRINT(RT_DEBUG_OFF,
				 ("%-4d",
				  (int)CLIENT_STATUS_TEST_FLAG(pEntry,
							       fCLIENT_STATUS_WMM_CAPABLE)));
			DBGPRINT(RT_DEBUG_OFF,
				 ("%-7d", pEntry->RssiSample.AvgRssi0));
			DBGPRINT(RT_DEBUG_OFF,
				 ("%-7d", pEntry->RssiSample.AvgRssi1));
			DBGPRINT(RT_DEBUG_OFF,
				 ("%-7d", pEntry->RssiSample.AvgRssi2));
#ifdef DOT11_N_SUPPORT
			DBGPRINT(RT_DEBUG_OFF, ("%-8d", (int)pEntry->MmpsMode));
			DBGPRINT(RT_DEBUG_OFF,
				 ("%-10s",
				  get_phymode_str(pEntry->HTPhyMode.field.MODE)));
			DBGPRINT(RT_DEBUG_OFF,
				 ("%-6s", get_bw_str(pEntry->HTPhyMode.field.BW)));
			DBGPRINT(RT_DEBUG_OFF,
				 ("%-6d", pEntry->HTPhyMode.field.MCS));
			DBGPRINT(RT_DEBUG_OFF,
				 ("%-6d", pEntry->HTPhyMode.field.ShortGI));
			DBGPRINT(RT_DEBUG_OFF,
				 ("%-6d", pEntry->HTPhyMode.field.STBC));
#endif /* DOT11_N_SUPPORT */
			DBGPRINT(RT_DEBUG_OFF,
				 ("%-10d, %d, %d%%\n", pEntry->DebugFIFOCount,
				  pEntry->DebugTxCount,
				  (pEntry->DebugTxCount) ? ((pEntry->DebugTxCount - pEntry->DebugFIFOCount) * 100 / pEntry->DebugTxCount) : 0));
			DBGPRINT(RT_DEBUG_OFF, ("\n"));

		}
	}

	return true;
}

INT Set_DlsAddEntry_Proc(
	IN struct rtmp_adapter *pAd,
	IN char *arg)
{
	u8 mac[ETH_ALEN];
	USHORT Timeout;
	char *token;
	STRING sepValue[] = ":", DASH = '-';
	INT i;
	RT_802_11_DLS Dls;

	if (strlen(arg) < 19)	/*Mac address acceptable format 01:02:03:04:05:06 length 17 plus the "-" and timeout value in decimal format. */
		return false;

	token = strchr(arg, DASH);
	if ((token != NULL) && (strlen(token) > 1)) {
		Timeout = (USHORT) simple_strtol((token + 1), 0, 10);

		*token = '\0';
		for (i = 0, token = rstrtok(arg, &sepValue[0]); token;
		     token = rstrtok(NULL, &sepValue[0]), i++) {
			if ((strlen(token) != 2) || (!isxdigit(*token))
			    || (!isxdigit(*(token + 1))))
				return false;
			AtoH(token, (&mac[i]), 1);
		}
		if (i != 6)
			return false;

		DBGPRINT(RT_DEBUG_OFF,
			 ("\n%02x:%02x:%02x:%02x:%02x:%02x-%d", mac[0], mac[1],
			  mac[2], mac[3], mac[4], mac[5], (int)Timeout));

		memset(&Dls, 0, sizeof (RT_802_11_DLS));
		Dls.TimeOut = Timeout;
		memcpy(Dls.MacAddr, mac, ETH_ALEN);
		Dls.Valid = 1;

		MlmeEnqueue(pAd,
			    MLME_CNTL_STATE_MACHINE,
			    RT_OID_802_11_SET_DLS_PARAM,
			    sizeof (RT_802_11_DLS), &Dls, 0);

		return true;
	}

	return false;

}

INT Set_DlsTearDownEntry_Proc(
	IN struct rtmp_adapter *pAd,
	IN char *arg)
{
	u8 macAddr[ETH_ALEN];
	char *value;
	INT i;
	RT_802_11_DLS Dls;

	if (strlen(arg) != 17)	/*Mac address acceptable format 01:02:03:04:05:06 length 17 */
		return false;

	for (i = 0, value = rstrtok(arg, ":"); value;
	     value = rstrtok(NULL, ":")) {
		if ((strlen(value) != 2) || (!isxdigit(*value))
		    || (!isxdigit(*(value + 1))))
			return false;	/*Invalid */

		AtoH(value, &macAddr[i++], 2);
	}

	DBGPRINT(RT_DEBUG_OFF, ("\n%02x:%02x:%02x:%02x:%02x:%02x", PRINT_MAC(macAddr)));

	memset(&Dls, 0, sizeof (RT_802_11_DLS));
	memcpy(Dls.MacAddr, macAddr, ETH_ALEN);
	Dls.Valid = 0;

	MlmeEnqueue(pAd,
		    MLME_CNTL_STATE_MACHINE,
		    RT_OID_802_11_SET_DLS_PARAM,
		    sizeof (RT_802_11_DLS), &Dls, 0);

	return true;
}
