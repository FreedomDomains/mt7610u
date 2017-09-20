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
		This routine is used to do insert packet into power-saveing queue.

	Arguments:
		pAd: Pointer to our adapter
		pPacket: Pointer to send packet
		pMacEntry: portint to entry of MacTab. the pMacEntry store attribute of client (STA).
		QueIdx: Priority queue idex.

	Return Value:
		NDIS_STATUS_SUCCESS:If succes to queue the packet into TxSwQueue.
		NDIS_STATUS_FAILURE: If failed to do en-queue.
========================================================================
*/
int RtmpInsertPsQueue(
	IN struct rtmp_adapter *pAd,
	IN struct sk_buff * pPacket,
	IN MAC_TABLE_ENTRY *pMacEntry,
	IN u8 QueIdx)
{
	ULONG IrqFlags;
	{
		if (pMacEntry->PsQueue.Number >= MAX_PACKETS_IN_PS_QUEUE)
		{
			dev_kfree_skb_any(pPacket);
			return NDIS_STATUS_FAILURE;
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE, ("legacy ps> queue a packet!\n"));
			spin_lock_bh(&pAd->irq_lock);
			InsertTailQueue(&pMacEntry->PsQueue, PACKET_TO_QUEUE_ENTRY(pPacket));
			spin_unlock_bh(&pAd->irq_lock);
		}
	}


	return NDIS_STATUS_SUCCESS;
}


/*
	==========================================================================
	Description:
		This routine is used to clean up a specified power-saving queue. It's
		used whenever a wireless client is deleted.
	==========================================================================
 */
void RtmpCleanupPsQueue(
	IN  struct rtmp_adapter *  pAd,
	IN  PQUEUE_HEADER   pQueue)
{
	PQUEUE_ENTRY pEntry;
	struct sk_buff * pPacket;

	DBGPRINT(RT_DEBUG_TRACE, ("RtmpCleanupPsQueue (0x%08lx)...\n", (ULONG)pQueue));

	while (pQueue->Head)
	{
		DBGPRINT(RT_DEBUG_TRACE,
					("RtmpCleanupPsQueue %ld...\n",pQueue->Number));

		pEntry = RemoveHeadQueue(pQueue);
		/*pPacket = CONTAINING_RECORD(pEntry, NDIS_PACKET, MiniportReservedEx); */
		pPacket = QUEUE_ENTRY_TO_PACKET(pEntry);
		dev_kfree_skb_any(pPacket);

		DBGPRINT(RT_DEBUG_TRACE, ("RtmpCleanupPsQueue pkt = %lx...\n", (ULONG)pPacket));
	}
}


/*
  ========================================================================
  Description:
	This routine frees all packets in PSQ that's destined to a specific DA.
	BCAST/MCAST in DTIMCount=0 case is also handled here, just like a PS-POLL
	is received from a WSTA which has MAC address FF:FF:FF:FF:FF:FF
  ========================================================================
*/
void RtmpHandleRxPsPoll(
	IN	struct rtmp_adapter *pAd,
	IN	u8 *		pAddr,
	IN	USHORT			Aid,
    IN	bool 		isActive)
{
	PQUEUE_ENTRY	  pEntry;
	PMAC_TABLE_ENTRY  pMacEntry;
	unsigned long		IrqFlags;

	/*DBGPRINT(RT_DEBUG_TRACE,("rcv PS-POLL (AID=%d) from %02x:%02x:%02x:%02x:%02x:%02x\n", */
	/*	  Aid, pAddr[0], pAddr[1], pAddr[2], pAddr[3], pAddr[4], pAddr[5])); */

	pMacEntry = &pAd->MacTab.Content[Aid];
	if (memcmp(pMacEntry->Addr, pAddr, ETH_ALEN) == 0)
	{
		/*
			Sta is change to Power Active stat.
			Reset ContinueTxFailCnt
		*/
		pMacEntry->ContinueTxFailCnt = 0;


		/*spin_lock_bh(&pAd->MacTabLock); */
		/*spin_lock_bh(&pAd->TxSwQueueLock); */
		spin_lock_bh(&pAd->irq_lock);
		if (isActive == false)
		{
			if (pMacEntry->PsQueue.Head)
			{
				pEntry = RemoveHeadQueue(&pMacEntry->PsQueue);
				InsertTailQueueAc(pAd, pMacEntry, &pAd->TxSwQueue[QID_AC_BE], pEntry);

			}
			else
			{
				/*
					or transmit a (QoS) Null Frame;

					In addtion, in Station Keep Alive mechanism, we need to
					send a QoS Null frame to detect the station live status.
				*/
				bool bQosNull = false;

				if (CLIENT_STATUS_TEST_FLAG(pMacEntry, fCLIENT_STATUS_WMM_CAPABLE))
					bQosNull = true;

				RtmpEnqueueNullFrame(pAd, pMacEntry->Addr, pMacEntry->CurrTxRate,
										Aid, pMacEntry->apidx, bQosNull, true, 0);
			}
		}
		else
		{

			while(pMacEntry->PsQueue.Head)
			{
/*				if (pAd->TxSwQueue[QID_AC_BE].Number <= */
/*                    (pAd->PortCfg.TxQueueSize + (MAX_PACKETS_IN_PS_QUEUE>>1))) */
				{
					pEntry = RemoveHeadQueue(&pMacEntry->PsQueue);
					InsertTailQueueAc(pAd, pMacEntry, &pAd->TxSwQueue[QID_AC_BE], pEntry);
				}
/*                else */
/*					break; */
				/* End of if */
			} /* End of while */
		} /* End of if */

		/*spin_unlock_bh(&pAd->TxSwQueueLock); */
		/*spin_unlock_bh(&pAd->MacTabLock); */

		if ((Aid > 0) && (Aid < MAX_LEN_OF_MAC_TABLE) &&
			(pMacEntry->PsQueue.Number == 0))
		{
			/* clear corresponding TIM bit because no any PS packet */
			pMacEntry->PsQIdleCount = 0;
		}

		spin_unlock_bh(&pAd->irq_lock);

		/* Dequeue outgoing frames from TxSwQueue0..3 queue and process it */
		/* TODO: 2004-12-27 it's not a good idea to handle "More Data" bit here. because the */
		/* RTMPDeQueue process doesn't guarantee to de-queue the desired MSDU from the corresponding */
		/* TxSwQueue/PsQueue when QOS in-used. We should consider "HardTransmt" this MPDU */
		/* using MGMT queue or things like that. */
		RTMPDeQueuePacket(pAd, false, NUM_OF_TX_RING, MAX_TX_PROCESS);

	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR,("rcv PS-POLL (AID=%d not match) from %02x:%02x:%02x:%02x:%02x:%02x\n",
			  Aid, pAddr[0], pAddr[1], pAddr[2], pAddr[3], pAddr[4], pAddr[5]));

	}
}


/*
	==========================================================================
	Description:
		Update the station current power save mode. Calling this routine also
		prove the specified client is still alive. Otherwise AP will age-out
		this client once IdleCount exceeds a threshold.
	==========================================================================
 */
bool RtmpPsIndicate(
	IN struct rtmp_adapter *pAd,
	IN u8 *pAddr,
	IN ULONG Wcid,
	IN u8 Psm)
{
	MAC_TABLE_ENTRY *pEntry;
    u8 old_psmode;

	if (Wcid >= MAX_LEN_OF_MAC_TABLE)
	{
		return PWR_ACTIVE;
	}

	pEntry = &pAd->MacTab.Content[Wcid];
	old_psmode = pEntry->PsMode;
/*	if (pEntry) */
	{
		/*
			Change power save mode first because we will call
			RTMPDeQueuePacket() in RtmpHandleRxPsPoll().

			Or when Psm = PWR_ACTIVE, we will not do Aggregation in
			RTMPDeQueuePacket().
		*/
		pEntry->NoDataIdleCount = 0;
		pEntry->PsMode = Psm;

		if ((old_psmode == PWR_ACTIVE) && (Psm == PWR_SAVE))
		{
			DBGPRINT(RT_DEBUG_TRACE,
					("RtmpPsIndicate - %02x:%02x:%02x:%02x:%02x:%02x sleep!\n",
					pAddr[0],pAddr[1],pAddr[2],pAddr[3],pAddr[4],pAddr[5]));
		}

		if ((old_psmode == PWR_SAVE) && (Psm == PWR_ACTIVE))
		{
			/* TODO: For RT2870, how to handle about the BA when STA in PS mode???? */

			DBGPRINT(RT_DEBUG_TRACE,
					("RtmpPsIndicate - %02x:%02x:%02x:%02x:%02x:%02x wakes up, "
					"act like rx PS-POLL\n",
					pAddr[0],pAddr[1],pAddr[2],pAddr[3],pAddr[4],pAddr[5]));

			/* sleep station awakes, move all pending frames from PSQ to TXQ if any */
			RtmpHandleRxPsPoll(pAd, pAddr, pEntry->Aid, true);
		}

		/* move to above section */
/*		pEntry->NoDataIdleCount = 0; */
/*		pEntry->PsMode = Psm; */
	}
/*	else */
/*	{ */
		/* not in table, try to learn it ???? why bother? */
/*	} */
	return old_psmode;
}


#ifdef CONFIG_STA_SUPPORT
/*
========================================================================
Routine Description:
    Check if PM of any packet is set.

Arguments:
	pAd		Pointer to our adapter

Return Value:
    true	can set
	false	can not set

Note:
========================================================================
*/
bool RtmpPktPmBitCheck(
    IN  struct rtmp_adapter *      pAd)
{
	bool FlgCanPmBitSet = true;



	if (FlgCanPmBitSet == true)
		return (pAd->StaCfg.Psm == PWR_SAVE);

	return false;
}


void RtmpPsActiveExtendCheck(
	IN struct rtmp_adapter *	pAd)
{
	/* count down the TDLS active counter */
}


void RtmpPsModeChange(
	IN struct rtmp_adapter *	pAd,
	IN u32				PsMode)
{
	if (pAd->StaCfg.BssType == BSS_INFRA)
	{
		/* reset ps mode */
		if (PsMode == Ndis802_11PowerModeMAX_PSP)
		{
			// do NOT turn on PSM bit here, wait until MlmeCheckForPsmChange()
			// to exclude certain situations.
			//	   MlmeSetPsm(pAd, PWR_SAVE);
			OPSTATUS_SET_FLAG(pAd, fOP_STATUS_RECEIVE_DTIM);
			if (pAd->StaCfg.bWindowsACCAMEnable == false)
				pAd->StaCfg.WindowsPowerMode = Ndis802_11PowerModeMAX_PSP;
			pAd->StaCfg.WindowsBatteryPowerMode = Ndis802_11PowerModeMAX_PSP;
			pAd->StaCfg.DefaultListenCount = 5;
		}
		else if (PsMode == Ndis802_11PowerModeFast_PSP)
		{
			// do NOT turn on PSM bit here, wait until MlmeCheckForPsmChange()
			// to exclude certain situations.
			OPSTATUS_SET_FLAG(pAd, fOP_STATUS_RECEIVE_DTIM);
			if (pAd->StaCfg.bWindowsACCAMEnable == false)
				pAd->StaCfg.WindowsPowerMode = Ndis802_11PowerModeFast_PSP;
			pAd->StaCfg.WindowsBatteryPowerMode = Ndis802_11PowerModeFast_PSP;
			pAd->StaCfg.DefaultListenCount = 3;
		}
		else if (PsMode == Ndis802_11PowerModeLegacy_PSP)
		{
			// do NOT turn on PSM bit here, wait until MlmeCheckForPsmChange()
			// to exclude certain situations.
			OPSTATUS_SET_FLAG(pAd, fOP_STATUS_RECEIVE_DTIM);
			if (pAd->StaCfg.bWindowsACCAMEnable == false)
				pAd->StaCfg.WindowsPowerMode = Ndis802_11PowerModeLegacy_PSP;
			pAd->StaCfg.WindowsBatteryPowerMode = Ndis802_11PowerModeLegacy_PSP;
			pAd->StaCfg.DefaultListenCount = 3;
		}
		else
		{ //Default Ndis802_11PowerModeCAM
			// clear PSM bit immediately
			RTMP_SET_PSM_BIT(pAd, PWR_ACTIVE);
			OPSTATUS_SET_FLAG(pAd, fOP_STATUS_RECEIVE_DTIM);
			if (pAd->StaCfg.bWindowsACCAMEnable == false)
				pAd->StaCfg.WindowsPowerMode = Ndis802_11PowerModeCAM;
			pAd->StaCfg.WindowsBatteryPowerMode = Ndis802_11PowerModeCAM;
		}

		/* change ps mode */
		RTMPSendNullFrame(pAd, pAd->CommonCfg.TxRate, true, false);

		DBGPRINT(RT_DEBUG_TRACE, ("PSMode=%ld\n", pAd->StaCfg.WindowsPowerMode));
	}
}
#endif /* CONFIG_STA_SUPPORT */

