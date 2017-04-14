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


#include <rt_config.h>


void mgmt_tb_set_mcast_entry(struct rtmp_adapter*pAd)
{
	MAC_TABLE_ENTRY *pEntry = &pAd->MacTab.Content[MCAST_WCID];

	pEntry->Sst = SST_ASSOC;
	pEntry->Aid = MCAST_WCID;	/* Softap supports 1 BSSID and use WCID=0 as multicast Wcid index*/
	pEntry->PsMode = PWR_ACTIVE;
	pEntry->CurrTxRate = pAd->CommonCfg.MlmeRate;
}


void set_entry_phy_cfg(struct rtmp_adapter*pAd, MAC_TABLE_ENTRY *pEntry)
{

	if (pEntry->MaxSupportedRate < RATE_FIRST_OFDM_RATE)
	{
		pEntry->MaxHTPhyMode.field.MODE = MODE_CCK;
		pEntry->MaxHTPhyMode.field.MCS = pEntry->MaxSupportedRate;
		pEntry->MinHTPhyMode.field.MODE = MODE_CCK;
		pEntry->MinHTPhyMode.field.MCS = pEntry->MaxSupportedRate;
		pEntry->HTPhyMode.field.MODE = MODE_CCK;
		pEntry->HTPhyMode.field.MCS = pEntry->MaxSupportedRate;
	}
	else
	{
		pEntry->MaxHTPhyMode.field.MODE = MODE_OFDM;
		pEntry->MaxHTPhyMode.field.MCS = OfdmRateToRxwiMCS[pEntry->MaxSupportedRate];
		pEntry->MinHTPhyMode.field.MODE = MODE_OFDM;
		pEntry->MinHTPhyMode.field.MCS = OfdmRateToRxwiMCS[pEntry->MaxSupportedRate];
		pEntry->HTPhyMode.field.MODE = MODE_OFDM;
		pEntry->HTPhyMode.field.MCS = OfdmRateToRxwiMCS[pEntry->MaxSupportedRate];
	}
}



/*
	==========================================================================
	Description:
		Look up the MAC address in the MAC table. Return NULL if not found.
	Return:
		pEntry - pointer to the MAC entry; NULL is not found
	==========================================================================
*/
MAC_TABLE_ENTRY *MacTableLookup(
	IN struct rtmp_adapter *pAd,
	u8 *pAddr)
{
	ULONG HashIdx;
	MAC_TABLE_ENTRY *pEntry = NULL;

	HashIdx = MAC_ADDR_HASH_INDEX(pAddr);
	pEntry = pAd->MacTab.Hash[HashIdx];

	while (pEntry && !IS_ENTRY_NONE(pEntry))
	{
		if (MAC_ADDR_EQUAL(pEntry->Addr, pAddr))
		{
			break;
		}
		else
			pEntry = pEntry->pNext;
	}

	return pEntry;
}


MAC_TABLE_ENTRY *MacTableInsertEntry(
	IN  struct rtmp_adapter *  pAd,
	IN  u8 *		pAddr,
	IN	u8 		apidx,
	IN	u8 		OpMode,
	IN bool CleanAll)
{
	u8 HashIdx;
	int i, FirstWcid;
	MAC_TABLE_ENTRY *pEntry = NULL, *pCurrEntry;
/*	USHORT	offset;*/
/*	ULONG	addr;*/
	bool Cancelled;
	u32 MaxWcidNum = MAX_LEN_OF_MAC_TABLE;
#ifdef MAC_REPEATER_SUPPORT
	bool bAPCLI = false;
#endif /* MAC_REPEATER_SUPPORT */

#ifdef MAC_REPEATER_SUPPORT
	if ((apidx >= MIN_NET_DEVICE_FOR_APCLI) &&
		(apidx < MIN_NET_DEVICE_FOR_MESH) &&
		(pAd->ApCfg.bMACRepeaterEn == true))
	{
		MaxWcidNum = MAX_LEN_OF_MAC_TABLE + (MAX_EXT_MAC_ADDR_SIZE * MAX_APCLI_NUM);
		bAPCLI = true;
	}
#endif /* MAC_REPEATER_SUPPORT */

	/* if FULL, return*/
	if (pAd->MacTab.Size >= MaxWcidNum)
		return NULL;

#ifdef MAC_REPEATER_SUPPORT
	if (bAPCLI)
		FirstWcid = 3;
	else
#endif /* MAC_REPEATER_SUPPORT */
	FirstWcid = 1;
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	if (pAd->StaCfg.BssType == BSS_INFRA)
		FirstWcid = 2;
#endif /* CONFIG_STA_SUPPORT */

	/* allocate one MAC entry*/
	RTMP_SEM_LOCK(&pAd->MacTabLock);
	for (i = FirstWcid; i< MaxWcidNum; i++)   /* skip entry#0 so that "entry index == AID" for fast lookup*/
	{
		/* pick up the first available vacancy*/
		if (IS_ENTRY_NONE(&pAd->MacTab.Content[i]))
		{
			pEntry = &pAd->MacTab.Content[i];

			/* ENTRY PREEMPTION: initialize the entry */
			RTMPCancelTimer(&pEntry->RetryTimer, &Cancelled);
			RTMPCancelTimer(&pEntry->EnqueueStartForPSKTimer, &Cancelled);

			memset(pEntry, 0, sizeof(MAC_TABLE_ENTRY));

#ifdef WFA_VHT_PF
#ifdef IP_ASSEMBLY
			if (pEntry->ip_queue_inited == 0) {
				int q_idx, ac_idx;
				struct ip_frag_q *fragQ = &pEntry->ip_fragQ[q_idx];

				for (ac_idx = 0; ac_idx < 4; ac_idx++) {
					InitializeQueueHeader(&pEntry->ip_queue1[ac_idx]);
					InitializeQueueHeader(&pEntry->ip_queue2[ac_idx]);
					pEntry->ip_id1[ac_idx] = pEntry->ip_id2[ac_idx] = -1;
					pEntry->ip_id1_FragSize[ac_idx] = pEntry->ip_id2_FragSize[ac_idx] = -1;
				}
				pEntry->ip_queue_inited = 1;
			}
#endif /* IP_ASSEMBLY */
#endif /* WFA_VHT_PF */

			if (CleanAll == true)
			{
				pEntry->MaxSupportedRate = RATE_11;
				pEntry->CurrTxRate = RATE_11;
				memset(pEntry, 0, sizeof(MAC_TABLE_ENTRY));
				pEntry->PairwiseKey.KeyLen = 0;
				pEntry->PairwiseKey.CipherAlg = CIPHER_NONE;
			}

			do
			{
#ifdef CONFIG_STA_SUPPORT
#ifdef QOS_DLS_SUPPORT
			if (apidx >= MIN_NET_DEVICE_FOR_DLS)
			{
				SET_ENTRY_DLS(pEntry);
				pEntry->isCached = false;
				break;
			}
			else
#endif /* QOS_DLS_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */



						SET_ENTRY_CLIENT(pEntry);

		} while (false);

			pEntry->bIAmBadAtheros = false;

			RTMPInitTimer(pAd, &pEntry->EnqueueStartForPSKTimer, GET_TIMER_FUNCTION(EnqueueStartForPSKExec), pEntry, false);

#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */

			pEntry->pAd = pAd;
			pEntry->CMTimerRunning = false;
			pEntry->EnqueueEapolStartTimerRunning = EAPOL_START_DISABLE;
			pEntry->RSNIE_Len = 0;
			memset(pEntry->R_Counter, 0, sizeof(pEntry->R_Counter));
			pEntry->ReTryCounter = PEER_MSG1_RETRY_TIMER_CTR;

			if (IS_ENTRY_MESH(pEntry))
				pEntry->apidx = (apidx - MIN_NET_DEVICE_FOR_MESH);
			else if (IS_ENTRY_APCLI(pEntry))
				pEntry->apidx = (apidx - MIN_NET_DEVICE_FOR_APCLI);
			else if (IS_ENTRY_WDS(pEntry))
				pEntry->apidx = (apidx - MIN_NET_DEVICE_FOR_WDS);
#ifdef CONFIG_STA_SUPPORT
#ifdef QOS_DLS_SUPPORT
			else if (IS_ENTRY_DLS(pEntry))
				pEntry->apidx = (apidx - MIN_NET_DEVICE_FOR_DLS);
#endif /* QOS_DLS_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */
			else
				pEntry->apidx = apidx;


			do
			{


#ifdef CONFIG_STA_SUPPORT
				IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
				{
					pEntry->AuthMode = pAd->StaCfg.AuthMode;
					pEntry->WepStatus = pAd->StaCfg.WepStatus;
					pEntry->PrivacyFilter = Ndis802_11PrivFilterAcceptAll;
				}
#endif /* CONFIG_STA_SUPPORT */
			} while (false);

			pEntry->GTKState = REKEY_NEGOTIATING;
			pEntry->PairwiseKey.KeyLen = 0;
			pEntry->PairwiseKey.CipherAlg = CIPHER_NONE;
#ifdef CONFIG_STA_SUPPORT
#ifdef QOS_DLS_SUPPORT
			if (IS_ENTRY_DLS(pEntry))
				pEntry->PortSecured = WPA_802_1X_PORT_SECURED;
			else
#endif /* QOS_DLS_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */
				pEntry->PortSecured = WPA_802_1X_PORT_NOT_SECURED;

			pEntry->PMKID_CacheIdx = ENTRY_NOT_FOUND;
			memcpy(pEntry->Addr, pAddr, ETH_ALEN);
			memcpy(pEntry->HdrAddr1, pAddr, ETH_ALEN);

			do
			{
#ifdef APCLI_SUPPORT
				if (IS_ENTRY_APCLI(pEntry))
				{
					memcpy(pEntry->HdrAddr2, pAd->ApCfg.ApCliTab[pEntry->apidx].CurrentAddress, ETH_ALEN);
					memcpy(pEntry->HdrAddr3, pAddr, ETH_ALEN);
					break;
				}
#endif // APCLI_SUPPORT //
#ifdef CONFIG_STA_SUPPORT
				if (OpMode == OPMODE_STA)
				{
					memcpy(pEntry->HdrAddr2, pAd->CurrentAddress, ETH_ALEN);
					memcpy(pEntry->HdrAddr3, pAddr, ETH_ALEN);
					break;
				}
#endif // CONFIG_STA_SUPPORT //
			} while (false);

			pEntry->Sst = SST_NOT_AUTH;
			pEntry->AuthState = AS_NOT_AUTH;
			pEntry->Aid = (USHORT)i;  /*0;*/
			pEntry->CapabilityInfo = 0;
			pEntry->PsMode = PWR_ACTIVE;
			pEntry->PsQIdleCount = 0;
			pEntry->NoDataIdleCount = 0;
			pEntry->AssocDeadLine = MAC_TABLE_ASSOC_TIMEOUT;
			pEntry->ContinueTxFailCnt = 0;
			pEntry->TimeStamp_toTxRing = 0;
			InitializeQueueHeader(&pEntry->PsQueue);



			pAd->MacTab.Size ++;

			/* Set the security mode of this entry as OPEN-NONE in ASIC */
			RTMP_REMOVE_PAIRWISE_KEY_ENTRY(pAd, (u8)i);

			/* Add this entry into ASIC RX WCID search table */
			RTMP_STA_ENTRY_ADD(pAd, pEntry);

			DBGPRINT(RT_DEBUG_TRACE, ("MacTableInsertEntry - allocate entry #%d, Total= %d\n",i, pAd->MacTab.Size));
			break;
		}
	}

	/* add this MAC entry into HASH table */
	if (pEntry)
	{

		HashIdx = MAC_ADDR_HASH_INDEX(pAddr);
		if (pAd->MacTab.Hash[HashIdx] == NULL)
		{
			pAd->MacTab.Hash[HashIdx] = pEntry;
		}
		else
		{
			pCurrEntry = pAd->MacTab.Hash[HashIdx];
			while (pCurrEntry->pNext != NULL)
				pCurrEntry = pCurrEntry->pNext;
			pCurrEntry->pNext = pEntry;
		}


	}

	RTMP_SEM_UNLOCK(&pAd->MacTabLock);
	return pEntry;
}


/*
	==========================================================================
	Description:
		Delete a specified client from MAC table
	==========================================================================
 */
bool MacTableDeleteEntry(
	IN struct rtmp_adapter *pAd,
	IN USHORT wcid,
	IN u8 *pAddr)
{
	USHORT HashIdx;
	MAC_TABLE_ENTRY *pEntry, *pPrevEntry, *pProbeEntry;
	bool Cancelled;
	/*USHORT	offset;	 unused variable*/
	/*u8 j;			 unused variable*/

	if (wcid >= MAX_LEN_OF_MAC_TABLE)
		return false;

	RTMP_SEM_LOCK(&pAd->MacTabLock);

	HashIdx = MAC_ADDR_HASH_INDEX(pAddr);
	/*pEntry = pAd->MacTab.Hash[HashIdx];*/
	pEntry = &pAd->MacTab.Content[wcid];

	if (pEntry && !IS_ENTRY_NONE(pEntry))
	{
		/* ENTRY PREEMPTION: Cancel all timers */
		RTMPCancelTimer(&pEntry->RetryTimer, &Cancelled);
		RTMPCancelTimer(&pEntry->EnqueueStartForPSKTimer, &Cancelled);

		if (MAC_ADDR_EQUAL(pEntry->Addr, pAddr))
		{

			/* Delete this entry from ASIC on-chip WCID Table*/
			RTMP_STA_ENTRY_MAC_RESET(pAd, wcid);

#ifdef DOT11_N_SUPPORT
			/* free resources of BA*/
			BASessionTearDownALL(pAd, pEntry->Aid);
#endif /* DOT11_N_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */


			pPrevEntry = NULL;
			pProbeEntry = pAd->MacTab.Hash[HashIdx];
			ASSERT(pProbeEntry);
			if (pProbeEntry != NULL)
			{
				/* update Hash list*/
				do
				{
					if (pProbeEntry == pEntry)
					{
						if (pPrevEntry == NULL)
						{
							pAd->MacTab.Hash[HashIdx] = pEntry->pNext;
						}
						else
						{
							pPrevEntry->pNext = pEntry->pNext;
						}
						break;
					}

					pPrevEntry = pProbeEntry;
					pProbeEntry = pProbeEntry->pNext;
				} while (pProbeEntry);
			}

			/* not found !!!*/
			ASSERT(pProbeEntry != NULL);

			/*RTMP_REMOVE_PAIRWISE_KEY_ENTRY(pAd, wcid);*/

#ifdef WFA_VHT_PF
#ifdef IP_ASSEMBLY
			if (pAd->ip_assemble == true)
			{
				if (pEntry->ip_queue_inited)
				{
					PQUEUE_ENTRY qe;
					struct sk_buff * q_pkt;

					qe = pEntry->ip_queue1;
					while (qe->Head)
					{
						DBGPRINT(RT_DEBUG_TRACE, ("%s():%ld...\n", __FUNCTION__, qe->Number));

						pEntry = RemoveHeadQueue(qe);
						/*pPacket = CONTAINING_RECORD(pEntry, NDIS_PACKET, MiniportReservedEx); */
						q_pkt = QUEUE_ENTRY_TO_PACKET(pEntry);
						RELEASE_NDIS_PACKET(pAd, q_pkt, NDIS_STATUS_FAILURE);
					}
					pEntry->ip_queue_inited = 0;
				}
			}
#endif /* IP_ASSEMBLY */
#endif /* WFA_VHT_PF */


		if (pEntry->EnqueueEapolStartTimerRunning != EAPOL_START_DISABLE)
		{
            RTMPCancelTimer(&pEntry->EnqueueStartForPSKTimer, &Cancelled);
			pEntry->EnqueueEapolStartTimerRunning = EAPOL_START_DISABLE;
        }
		RTMPReleaseTimer(&pEntry->EnqueueStartForPSKTimer, &Cancelled);




//   			memset(pEntry, 0, sizeof(MAC_TABLE_ENTRY));
//			memset(pEntry->Addr, 0, ETH_ALEN);
//			/* invalidate the entry */
//			SET_ENTRY_NONE(pEntry);

//                      memset(pEntry, 0, sizeof(MAC_TABLE_ENTRY));
                        memset(pEntry->Addr, 0, ETH_ALEN);
                        /* invalidate the entry */
                        SET_ENTRY_NONE(pEntry);

			pAd->MacTab.Size --;

			DBGPRINT(RT_DEBUG_TRACE, ("MacTableDeleteEntry1 - Total= %d\n", pAd->MacTab.Size));
		}
		else
		{
			DBGPRINT(RT_DEBUG_OFF, ("\n%s: Impossible Wcid = %d !!!!!\n", __FUNCTION__, wcid));
		}
	}

	RTMP_SEM_UNLOCK(&pAd->MacTabLock);

	/*Reset operating mode when no Sta.*/
	if (pAd->MacTab.Size == 0)
	{
#ifdef DOT11_N_SUPPORT
		pAd->CommonCfg.AddHTInfo.AddHtInfo2.OperaionMode = 0;
#endif /* DOT11_N_SUPPORT */
		RTMP_UPDATE_PROTECT(pAd, 0, ALLN_SETPROTECT, true, 0);
	}

	pAd->chipCap.pWeakestEntry = NULL;
	return true;
}


/*
	==========================================================================
	Description:
		This routine reset the entire MAC table. All packets pending in
		the power-saving queues are freed here.
	==========================================================================
 */
void MacTableReset(
	IN  struct rtmp_adapter * pAd)
{
	int         i;
	bool     Cancelled;

	DBGPRINT(RT_DEBUG_TRACE, ("MacTableReset\n"));
	/*RTMP_SEM_LOCK(&pAd->MacTabLock);*/


	for (i=1; i<MAX_LEN_OF_MAC_TABLE; i++)
	{
		if (IS_ENTRY_CLIENT(&pAd->MacTab.Content[i]))
	   {
	   		/* Delete a entry via WCID */

			/*MacTableDeleteEntry(pAd, i, pAd->MacTab.Content[i].Addr);*/
			RTMPReleaseTimer(&pAd->MacTab.Content[i].EnqueueStartForPSKTimer, &Cancelled);
#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */
            pAd->MacTab.Content[i].EnqueueEapolStartTimerRunning = EAPOL_START_DISABLE;


			/* Delete a entry via WCID */
			MacTableDeleteEntry(pAd, i, pAd->MacTab.Content[i].Addr);
		}
		else
		{
			/* Delete a entry via WCID */
			MacTableDeleteEntry(pAd, i, pAd->MacTab.Content[i].Addr);
		}
	}

	return;
}

#ifdef MAC_REPEATER_SUPPORT
MAC_TABLE_ENTRY *InsertMacRepeaterEntry(
	IN  struct rtmp_adapter *  pAd,
	IN  u8 *		pAddr,
	IN  u8 		IfIdx)
{
	MAC_TABLE_ENTRY *pEntry = NULL;
	PAPCLI_STRUCT pApCliEntry = NULL;

	kmalloc(&pEntry, sizeof(MAC_TABLE_ENTRY));

	if (pEntry)
	{
		pApCliEntry = &pAd->ApCfg.ApCliTab[IfIdx];
		pEntry->Aid = pApCliEntry->MacTabWCID + 1; // TODO: We need to record count of STAs
		memcpy(pEntry->Addr, pApCliEntry->ApCliMlmeAux.Bssid, ETH_ALEN);
		printk("sn - InsertMacRepeaterEntry: Aid = %d\n", pEntry->Aid);
		/* Add this entry into ASIC RX WCID search table */
		RTMP_STA_ENTRY_ADD(pAd, pEntry);
		kfree(pEntry);
	}

	return pEntry;
}

#endif /* MAC_REPEATER_SUPPORT */

