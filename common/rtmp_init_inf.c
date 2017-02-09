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

int rt28xx_init(struct rtmp_adapter *pAd)
{
	UINT index;
	int Status;

	if (pAd == NULL)
		return false;

#ifdef RT65xx
	if (pAd->WlanFunCtrl.field.WLAN_EN == 0)
		MT76x0_WLAN_ChipOnOff(pAd, true, false);
#endif /* RT65xx */

	/* reset Adapter flags*/
	RTMP_CLEAR_FLAGS(pAd);

	/* Init BssTab & ChannelInfo tabbles for auto channel select.*/

#ifdef DOT11_N_SUPPORT
	/* Allocate BA Reordering memory*/
	if (ba_reordering_resource_init(pAd, MAX_REORDERING_MPDU_NUM) != true)
		goto err1;
#endif /* DOT11_N_SUPPORT */

	/* Make sure MAC gets ready.*/
	index = 0;
	if (WaitForAsicReady(pAd) != true)
		goto err1;

	DBGPRINT(RT_DEBUG_OFF, ("MAC[Ver=0x%08x]\n",
				pAd->MACVersion));


	if (MAX_LEN_OF_MAC_TABLE > MAX_AVAILABLE_CLIENT_WCID(pAd))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("MAX_LEN_OF_MAC_TABLE can not be larger than MAX_AVAILABLE_CLIENT_WCID!!!!\n"));
		goto err1;
	}


	RT28XXDMADisable(pAd);

	NICLoadFirmware(pAd);

	MCU_CTRL_INIT(pAd);

	/* Disable interrupts here which is as soon as possible*/
	/* This statement should never be true. We might consider to remove it later*/

#ifdef RESOURCE_PRE_ALLOC
	Status = RTMPInitTxRxRingMemory(pAd);
#else
	Status = RTMPAllocTxRxRingMemory(pAd);
#endif /* RESOURCE_PRE_ALLOC */

	if (Status != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT_ERR(("RTMPAllocTxRxMemory failed, Status[=0x%08x]\n", Status));
		goto err2;
	}

	Status = RtmpNetTaskInit(pAd);
	if (Status != NDIS_STATUS_SUCCESS)
		goto err5;

#ifdef WLAN_SKB_RECYCLE
    skb_queue_head_init(&pAd->rx0_recycle);
#endif /* WLAN_SKB_RECYCLE */

	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE);

	/* initialize MLME*/
	Status = RtmpMgmtTaskInit(pAd);
	if (Status != NDIS_STATUS_SUCCESS)
		goto err3;

#ifdef RMTP_RBUS_SUPPORT
#ifdef VIDEO_TURBINE_SUPPORT
	VideoConfigInit(pAd);
#endif /* VIDEO_TURBINE_SUPPORT */
#endif /* RMTP_RBUS_SUPPORT */

	/* Initialize pAd->StaCfg, pAd->ApCfg, pAd->CommonCfg to manufacture default*/
	UserCfgInit(pAd);


	CfgInitHook(pAd);


#ifdef BLOCK_NET_IF
	initblockQueueTab(pAd);
#endif /* BLOCK_NET_IF */

	Status = MeasureReqTabInit(pAd);
	if (Status != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT_ERR(("MeasureReqTabInit failed, Status[=0x%08x]\n",Status));
		goto err6;
	}
	Status = TpcReqTabInit(pAd);
	if (Status != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT_ERR(("TpcReqTabInit failed, Status[=0x%08x]\n",Status));
		goto err6;
	}

	/* Init the hardware, we need to init asic before read registry, otherwise mac register will be reset*/
	Status = NICInitializeAdapter(pAd, true);
	if (Status != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT_ERR(("NICInitializeAdapter failed, Status[=0x%08x]\n", Status));
		if (Status != NDIS_STATUS_SUCCESS)
		goto err6;
	}

	Status = MlmeInit(pAd);
	if (Status != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT_ERR(("MlmeInit failed, Status[=0x%08x]\n", Status));
		goto err4;
	}


	/* Read parameters from Config File */
	/* unknown, it will be updated in NICReadEEPROMParameters */
	pAd->RfIcType = RFIC_UNKNOWN;
	Status = RTMPReadParametersHook(pAd);

	DBGPRINT(RT_DEBUG_OFF, ("1. Phy Mode = %d\n", pAd->CommonCfg.PhyMode));
	if (Status != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT_ERR(("RTMPReadParametersHook failed, Status[=0x%08x]\n",Status));
		goto err6;
	}

	pAd->CommonCfg.SsidLen = 0;
	memset(pAd->CommonCfg.Ssid, 0, NDIS_802_11_LENGTH_SSID);
	pAd->CommonCfg.LastSsidLen= pAd->CommonCfg.SsidLen;
	memset(pAd->CommonCfg.LastSsid, 0, NDIS_802_11_LENGTH_SSID);
	pAd->MlmeAux.AutoReconnectSsidLen = pAd->CommonCfg.SsidLen;
	memset(pAd->MlmeAux.AutoReconnectSsid, 0, NDIS_802_11_LENGTH_SSID);
	pAd->MlmeAux.SsidLen = pAd->CommonCfg.SsidLen;
	memset(pAd->MlmeAux.Ssid, 0, NDIS_802_11_LENGTH_SSID);

#ifdef RTMP_MAC_USB
	pAd->CommonCfg.bMultipleIRP = false;

	if (pAd->CommonCfg.bMultipleIRP)
		pAd->CommonCfg.NumOfBulkInIRP = RX_RING_SIZE;
	else
		pAd->CommonCfg.NumOfBulkInIRP = 1;
#endif /* RTMP_MAC_USB */

#ifdef DOT11_N_SUPPORT
   	/*Init Ba Capability parameters.*/
/*	RT28XX_BA_INIT(pAd);*/
	pAd->CommonCfg.DesiredHtPhy.MpduDensity = (u8)pAd->CommonCfg.BACapability.field.MpduDensity;
	pAd->CommonCfg.DesiredHtPhy.AmsduEnable = (USHORT)pAd->CommonCfg.BACapability.field.AmsduEnable;
	pAd->CommonCfg.DesiredHtPhy.AmsduSize = (USHORT)pAd->CommonCfg.BACapability.field.AmsduSize;
	pAd->CommonCfg.DesiredHtPhy.MimoPs = (USHORT)pAd->CommonCfg.BACapability.field.MMPSmode;
	/* UPdata to HT IE*/
	pAd->CommonCfg.HtCapability.HtCapInfo.MimoPs = (USHORT)pAd->CommonCfg.BACapability.field.MMPSmode;
	pAd->CommonCfg.HtCapability.HtCapInfo.AMsduSize = (USHORT)pAd->CommonCfg.BACapability.field.AmsduSize;
	pAd->CommonCfg.HtCapability.HtCapParm.MpduDensity = (u8)pAd->CommonCfg.BACapability.field.MpduDensity;
#endif /* DOT11_N_SUPPORT */

	/* after reading Registry, we now know if in AP mode or STA mode*/

	DBGPRINT(RT_DEBUG_OFF, ("2. Phy Mode = %d\n", pAd->CommonCfg.PhyMode));

	/* We should read EEPROM for all cases.  rt2860b*/
	NICReadEEPROMParameters(pAd);
#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */

	DBGPRINT(RT_DEBUG_OFF, ("3. Phy Mode = %d\n", pAd->CommonCfg.PhyMode));

#ifdef LED_CONTROL_SUPPORT
	/* Send LED Setting to MCU */
	RTMPInitLEDMode(pAd);
#endif /* LED_CONTROL_SUPPORT */

	NICInitAsicFromEEPROM(pAd); /* rt2860b */

	/* Set PHY to appropriate mode*/
	RTMPSetPhyMode(pAd, pAd->CommonCfg.PhyMode);

	/* No valid channels.*/
	if (pAd->ChannelListNum == 0)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Wrong configuration. No valid channel found. Check \"ContryCode\" and \"ChannelGeography\" setting.\n"));
		goto err6;
	}

#ifdef DOT11_N_SUPPORT
	DBGPRINT(RT_DEBUG_OFF, ("MCS Set = %02x %02x %02x %02x %02x\n", pAd->CommonCfg.HtCapability.MCSSet[0],
           pAd->CommonCfg.HtCapability.MCSSet[1], pAd->CommonCfg.HtCapability.MCSSet[2],
           pAd->CommonCfg.HtCapability.MCSSet[3], pAd->CommonCfg.HtCapability.MCSSet[4]));
#endif /* DOT11_N_SUPPORT */



	/* APInitialize(pAd);*/

#ifdef RTMP_MAC_USB
	RTMPusecDelay(10000);
#endif /* RTMP_MAC_USB */

	/*
		Some modules init must be called before APStartUp().
		Or APStartUp() will make up beacon content and call
		other modules API to get some information to fill.
	*/



	if (pAd && (Status != NDIS_STATUS_SUCCESS))
	{
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE))
		{
			RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE);
		}
	}
	else if (pAd)
	{
		/* Microsoft HCT require driver send a disconnect event after driver initialization.*/
		OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED);
		OPSTATUS_CLEAR_FLAG(pAd, fOP_AP_STATUS_MEDIA_STATE_CONNECTED);
		RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_MEDIA_STATE_CHANGE);

		DBGPRINT(RT_DEBUG_TRACE, ("NDIS_STATUS_MEDIA_DISCONNECT Event B!\n"));


#ifdef RTMP_MAC_USB
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_RESET_IN_PROGRESS);
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_REMOVE_IN_PROGRESS);


		/* Support multiple BulkIn IRP,*/
		/* the value on pAd->CommonCfg.NumOfBulkInIRP may be large than 1.*/

		for(index=0; index<pAd->CommonCfg.NumOfBulkInIRP; index++)
		{
			RTUSBBulkReceive(pAd);
			DBGPRINT(RT_DEBUG_TRACE, ("RTUSBBulkReceive!\n" ));
		}
#endif /* RTMP_MAC_USB */
	}/* end of else*/

	/* Set up the Mac address*/
#ifdef CONFIG_STA_SUPPORT
	RtmpOSNetDevAddrSet(pAd->OpMode, pAd->net_dev, &pAd->CurrentAddress[0], (u8 *)(pAd->StaCfg.dev_name));
#endif /* CONFIG_STA_SUPPORT */

	/* Various AP function init*/

#ifdef UAPSD_SUPPORT
        UAPSD_Init(pAd);
#endif /* UAPSD_SUPPORT */

	/* assign function pointers*/





#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{

#ifdef WPA_SUPPLICANT_SUPPORT
#ifndef NATIVE_WPA_SUPPLICANT_SUPPORT
		/* send wireless event to wpa_supplicant for infroming interface up.*/
		RtmpOSWrielessEventSend(pAd->net_dev, RT_WLAN_EVENT_CUSTOM, RT_INTERFACE_UP, NULL, NULL, 0);
#endif /* NATIVE_WPA_SUPPLICANT_SUPPORT */
#endif /* WPA_SUPPLICANT_SUPPORT */

	}
#endif /* CONFIG_STA_SUPPORT */

	/* auto-fall back settings */
#ifdef RANGE_EXTEND
	mt7610u_write32(pAd, HT_FBK_CFG1, 0xedcba980);
#endif // RANGE_EXTEND //
#ifdef DOT11N_SS3_SUPPORT
	if (pAd->CommonCfg.TxStream >= 3)
	{
		mt7610u_write32(pAd, TX_FBK_CFG_3S_0, 0x12111008);
		mt7610u_write32(pAd, TX_FBK_CFG_3S_1, 0x16151413);
	}
#endif /* DOT11N_SS3_SUPPORT */

#ifdef DOT11_N_SUPPORT
#endif /* DOT11_N_SUPPORT */

	DBGPRINT_S(Status, ("<==== rt28xx_init, Status=%x\n", Status));

	return true;

/*err7:
	APStop(pAd);*/
err6:

#ifdef IGMP_SNOOP_SUPPORT
	MultiCastFilterTableReset(&pAd->pMulticastFilterTable);
#endif /* IGMP_SNOOP_SUPPORT */

	MeasureReqTabExit(pAd);
	TpcReqTabExit(pAd);
err5:
	RtmpNetTaskExit(pAd);
	UserCfgExit(pAd);
err4:
	MlmeHalt(pAd);
	RTMP_TimerListRelease(pAd);
err3:
	RtmpMgmtTaskExit(pAd);
#ifdef RTMP_TIMER_TASK_SUPPORT
#endif /* RTMP_TIMER_TASK_SUPPORT */
err2:
	MCU_CTRL_EXIT(pAd);
#ifdef RESOURCE_PRE_ALLOC
	RTMPResetTxRxRingMemory(pAd);
#else
	RTMPFreeTxRxRingMemory(pAd);
#endif /* RESOURCE_PRE_ALLOC */

err1:

#ifdef DOT11_N_SUPPORT
	if(pAd->mpdu_blk_pool.mem)
		kfree(pAd->mpdu_blk_pool.mem); /* free BA pool*/
#endif /* DOT11_N_SUPPORT */

	/* shall not set priv to NULL here because the priv didn't been free yet.*/
	/*net_dev->priv = 0;*/
#ifdef ST
err0:
#endif /* ST */

	DBGPRINT(RT_DEBUG_ERROR, ("!!! rt28xx init fail !!!\n"));
	return false;
}


#ifdef CONFIG_STA_SUPPORT
void RTMPDrvSTAOpen(struct rtmp_adapter *pAd)
{
	u32 reg = 0;

	RTMP_CLEAR_PSFLAG(pAd, fRTMP_PS_MCU_SLEEP);

	/* Enable Interrupt*/
	RTMP_IRQ_ENABLE(pAd);

	/* Now Enable RxTx*/
	RTMPEnableRxTx(pAd);

	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_START_UP);

#ifdef MT76x0
	if (IS_MT76x0(pAd))
	{
		/* Select Q2 to receive command response */
		mt7610u_mcu_fun_set(pAd, Q_SELECT, pAd->chipCap.CmdRspRxRing);

	}
#endif /* MT76x0 */

	reg = mt7610u_read32(pAd, 0x1300);  /* clear garbage interrupts*/
	DBGPRINT(RT_DEBUG_OFF, ("0x1300 = %08x\n", reg));




//+++Add by shiang for debug
	DBGPRINT(RT_DEBUG_OFF, ("%s(1):Check if PDMA is idle!\n", __FUNCTION__));
	AsicWaitPDMAIdle(pAd, 5, 10);
//---Add by shiang for debug

	/*
		To reduce connection time,
		do auto reconnect here instead of waiting STAMlmePeriodicExec to do auto reconnect.
	*/

//+++Add by shiang for debug
	DBGPRINT(RT_DEBUG_OFF, ("%s(2):Check if PDMA is idle!\n", __FUNCTION__));
	AsicWaitPDMAIdle(pAd, 5, 10);
//---Add by shiang for debug



}

void RTMPDrvSTAClose(
	IN struct rtmp_adapter *pAd,
	IN struct net_device *net_dev)
{
	bool Cancelled;
	u32 i = 0;
	Cancelled = false;

	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* If dirver doesn't wake up firmware here,*/
		/* NICLoadFirmware will hang forever when interface is up again.*/
		if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE))
        {
		    AsicForceWakeup(pAd, true);
        }

#ifdef RTMP_MAC_USB
		RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_REMOVE_IN_PROGRESS);
#endif /* RTMP_MAC_USB */

	}

	/* RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD); */
	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS);

#ifdef EXT_BUILD_CHANNEL_LIST
	if (pAd->CommonCfg.pChDesp != NULL)
		kfree(pAd->CommonCfg.pChDesp);
	pAd->CommonCfg.pChDesp = NULL;
	pAd->CommonCfg.DfsType = MAX_RD_REGION;
#endif /* EXT_BUILD_CHANNEL_LIST */
	pAd->CommonCfg.bCountryFlag = false;


	for (i = 0 ; i < NUM_OF_TX_RING; i++)
	{
		while (pAd->DeQueueRunning[i] == true)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("Waiting for TxQueue[%d] done..........\n", i));
			RTMPusecDelay(1000);
		}
	}

	/* Stop Mlme state machine*/
	MlmeHalt(pAd);

	/* Close net tasklets*/
	RtmpNetTaskExit(pAd);


	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		MacTableReset(pAd);
		MlmeRadioOff(pAd);
	}

	MeasureReqTabExit(pAd);
	TpcReqTabExit(pAd);

#ifdef LED_CONTROL_SUPPORT
	RTMPExitLEDMode(pAd);
#endif // LED_CONTROL_SUPPORT

	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD);

	/* Close kernel threads*/
	RtmpMgmtTaskExit(pAd);

	//MCU_CTRL_EXIT(pAd);


	/* Free IRQ*/
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE))
	{
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE);
	}

#ifdef RESOURCE_PRE_ALLOC
	RTMPResetTxRxRingMemory(pAd);
#else
	/* Free Ring or USB buffers*/
	RTMPFreeTxRxRingMemory(pAd);
#endif /* RESOURCE_PRE_ALLOC */

	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS);

#ifdef WLAN_SKB_RECYCLE
	skb_queue_purge(&pAd->rx0_recycle);
#endif /* WLAN_SKB_RECYCLE */

#ifdef DOT11_N_SUPPORT
	/* Free BA reorder resource*/
	ba_reordering_resource_release(pAd);
#endif /* DOT11_N_SUPPORT */

	UserCfgExit(pAd); /* must after ba_reordering_resource_release */


	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_START_UP);

	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
	}

	/* clear MAC table */
	/* TODO: do not clear spin lock, such as fLastChangeAccordingMfbLock */
	memset(&pAd->MacTab, 0, sizeof(MAC_TABLE));

	/* release all timers */
	RTMPusecDelay(2000);
	RTMP_TimerListRelease(pAd);

#ifdef RTMP_TIMER_TASK_SUPPORT
#endif /* RTMP_TIMER_TASK_SUPPORT */
}
#endif

void RTMPInfClose(struct rtmp_adapter *pAd)
{
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
#ifdef QOS_DLS_SUPPORT
		/* send DLS-TEAR_DOWN message, */
		if (pAd->CommonCfg.bDLSCapable)
		{
			u8 i;

			/* tear down local dls table entry*/
			for (i=0; i<MAX_NUM_OF_INIT_DLS_ENTRY; i++)
			{
				if (pAd->StaCfg.DLSEntry[i].Valid && (pAd->StaCfg.DLSEntry[i].Status == DLS_FINISH))
				{
					RTMPSendDLSTearDownFrame(pAd, pAd->StaCfg.DLSEntry[i].MacAddr);
					pAd->StaCfg.DLSEntry[i].Status	= DLS_NONE;
					pAd->StaCfg.DLSEntry[i].Valid	= false;
				}
			}

			/* tear down peer dls table entry*/
			for (i=MAX_NUM_OF_INIT_DLS_ENTRY; i<MAX_NUM_OF_DLS_ENTRY; i++)
			{
				if (pAd->StaCfg.DLSEntry[i].Valid && (pAd->StaCfg.DLSEntry[i].Status == DLS_FINISH))
				{
					RTMPSendDLSTearDownFrame(pAd, pAd->StaCfg.DLSEntry[i].MacAddr);
					pAd->StaCfg.DLSEntry[i].Status = DLS_NONE;
					pAd->StaCfg.DLSEntry[i].Valid	= false;
				}
			}
			RTMP_MLME_HANDLER(pAd);
		}
#endif /* QOS_DLS_SUPPORT */

		if (INFRA_ON(pAd) &&
			(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
		{
			MLME_DISASSOC_REQ_STRUCT	DisReq;
			MLME_QUEUE_ELEM *MsgElem;/* = (MLME_QUEUE_ELEM *) kmalloc(sizeof(MLME_QUEUE_ELEM), MEM_ALLOC_FLAG);*/

			MsgElem = kmalloc(sizeof(MLME_QUEUE_ELEM), GFP_ATOMIC);
			if (MsgElem)
			{
			memcpy(DisReq.Addr, pAd->CommonCfg.Bssid, ETH_ALEN);
			DisReq.Reason =  REASON_DEAUTH_STA_LEAVING;

			MsgElem->Machine = ASSOC_STATE_MACHINE;
			MsgElem->MsgType = MT2_MLME_DISASSOC_REQ;
			MsgElem->MsgLen = sizeof(MLME_DISASSOC_REQ_STRUCT);
			memmove(MsgElem->Msg, &DisReq, sizeof(MLME_DISASSOC_REQ_STRUCT));

			/* Prevent to connect AP again in STAMlmePeriodicExec*/
			pAd->MlmeAux.AutoReconnectSsidLen= 32;
			memset(pAd->MlmeAux.AutoReconnectSsid, 0, pAd->MlmeAux.AutoReconnectSsidLen);

			pAd->Mlme.CntlMachine.CurrState = CNTL_WAIT_OID_DISASSOC;
			MlmeDisassocReqAction(pAd, MsgElem);
/*			kfree(MsgElem);*/
			kfree(MsgElem);
			}

			RTMPusecDelay(1000);
		}

#ifdef WPA_SUPPLICANT_SUPPORT
#ifndef NATIVE_WPA_SUPPLICANT_SUPPORT
		/* send wireless event to wpa_supplicant for infroming interface down.*/
		RtmpOSWrielessEventSend(pAd->net_dev, RT_WLAN_EVENT_CUSTOM, RT_INTERFACE_DOWN, NULL, NULL, 0);
#endif /* NATIVE_WPA_SUPPLICANT_SUPPORT */

		if (pAd->StaCfg.pWpsProbeReqIe)
		{
/*			kfree(pAd->StaCfg.pWpsProbeReqIe);*/
			kfree(pAd->StaCfg.pWpsProbeReqIe);
			pAd->StaCfg.pWpsProbeReqIe = NULL;
			pAd->StaCfg.WpsProbeReqIeLen = 0;
		}

		if (pAd->StaCfg.pWpaAssocIe)
		{
/*			kfree(pAd->StaCfg.pWpaAssocIe);*/
			kfree(pAd->StaCfg.pWpaAssocIe);
			pAd->StaCfg.pWpaAssocIe = NULL;
			pAd->StaCfg.WpaAssocIeLen = 0;
		}
#endif /* WPA_SUPPLICANT_SUPPORT */


	}
#endif /* CONFIG_STA_SUPPORT */
}




struct net_device *RtmpPhyNetDevMainCreate(struct rtmp_adapter *pAd)
{
	struct net_device *pDevNew;
	u32 MC_RowID = 0, IoctlIF = 0;

#ifdef HOSTAPD_SUPPORT
	IoctlIF = pAd->IoctlIF;
#endif /* HOSTAPD_SUPPORT */

	pDevNew = RtmpOSNetDevCreate((INT32)MC_RowID, (u32 *)&IoctlIF,
					INT_MAIN, 0, sizeof(struct rtmp_adapter *), "wlan");

#ifdef HOSTAPD_SUPPORT
	pAd->IoctlIF = IoctlIF;
#endif /* HOSTAPD_SUPPORT */

	return pDevNew;
}


