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

#ifdef SYSTEM_LOG_SUPPORT
/* for wireless system event message */
char const *pWirelessSysEventText[IW_SYS_EVENT_TYPE_NUM] = {
	/* system status event */
    "had associated successfully",							/* IW_ASSOC_EVENT_FLAG */
    "had disassociated",									/* IW_DISASSOC_EVENT_FLAG */
    "had deauthenticated",									/* IW_DEAUTH_EVENT_FLAG */
    "had been aged-out and disassociated",					/* IW_AGEOUT_EVENT_FLAG */
    "occurred CounterMeasures attack",						/* IW_COUNTER_MEASURES_EVENT_FLAG */
    "occurred replay counter different in Key Handshaking",	/* IW_REPLAY_COUNTER_DIFF_EVENT_FLAG */
    "occurred RSNIE different in Key Handshaking",			/* IW_RSNIE_DIFF_EVENT_FLAG */
    "occurred MIC different in Key Handshaking",			/* IW_MIC_DIFF_EVENT_FLAG */
    "occurred ICV error in RX",								/* IW_ICV_ERROR_EVENT_FLAG */
    "occurred MIC error in RX",								/* IW_MIC_ERROR_EVENT_FLAG */
	"Group Key Handshaking timeout",						/* IW_GROUP_HS_TIMEOUT_EVENT_FLAG */
	"Pairwise Key Handshaking timeout",						/* IW_PAIRWISE_HS_TIMEOUT_EVENT_FLAG */
	"RSN IE sanity check failure",							/* IW_RSNIE_SANITY_FAIL_EVENT_FLAG */
	"set key done in WPA/WPAPSK",							/* IW_SET_KEY_DONE_WPA1_EVENT_FLAG */
	"set key done in WPA2/WPA2PSK",                         /* IW_SET_KEY_DONE_WPA2_EVENT_FLAG */
	"connects with our wireless client",                    /* IW_STA_LINKUP_EVENT_FLAG */
	"disconnects with our wireless client",                 /* IW_STA_LINKDOWN_EVENT_FLAG */
	"scan completed",										/* IW_SCAN_COMPLETED_EVENT_FLAG */
	"scan terminate!! Busy!! Enqueue fail!!",				/* IW_SCAN_ENQUEUE_FAIL_EVENT_FLAG */
	"channel switch to ",									/* IW_CHANNEL_CHANGE_EVENT_FLAG */
	"wireless mode is not support",							/* IW_STA_MODE_EVENT_FLAG */
	"blacklisted in MAC filter list",						/* IW_MAC_FILTER_LIST_EVENT_FLAG */
	"Authentication rejected because of challenge failure",	/* IW_AUTH_REJECT_CHALLENGE_FAILURE */
	"Scanning",												/* IW_SCANNING_EVENT_FLAG */
	"Start a new IBSS",										/* IW_START_IBSS_FLAG */
	"Join the IBSS",										/* IW_JOIN_IBSS_FLAG */
	"Shared WEP fail",										/* IW_SHARED_WEP_FAIL*/
	};

#ifdef IDS_SUPPORT
/* for wireless IDS_spoof_attack event message */
char const *pWirelessSpoofEventText[IW_SPOOF_EVENT_TYPE_NUM] = {
    "detected conflict SSID",								/* IW_CONFLICT_SSID_EVENT_FLAG */
    "detected spoofed association response",				/* IW_SPOOF_ASSOC_RESP_EVENT_FLAG */
    "detected spoofed reassociation responses",				/* IW_SPOOF_REASSOC_RESP_EVENT_FLAG */
    "detected spoofed probe response",						/* IW_SPOOF_PROBE_RESP_EVENT_FLAG */
    "detected spoofed beacon",								/* IW_SPOOF_BEACON_EVENT_FLAG */
    "detected spoofed disassociation",						/* IW_SPOOF_DISASSOC_EVENT_FLAG */
    "detected spoofed authentication",						/* IW_SPOOF_AUTH_EVENT_FLAG */
    "detected spoofed deauthentication",					/* IW_SPOOF_DEAUTH_EVENT_FLAG */
    "detected spoofed unknown management frame",			/* IW_SPOOF_UNKNOWN_MGMT_EVENT_FLAG */
	"detected replay attack"								/* IW_REPLAY_ATTACK_EVENT_FLAG */
	};

/* for wireless IDS_flooding_attack event message */
char const *pWirelessFloodEventText[IW_FLOOD_EVENT_TYPE_NUM] = {
	"detected authentication flooding",						/* IW_FLOOD_AUTH_EVENT_FLAG */
    "detected association request flooding",				/* IW_FLOOD_ASSOC_REQ_EVENT_FLAG */
    "detected reassociation request flooding",				/* IW_FLOOD_REASSOC_REQ_EVENT_FLAG */
    "detected probe request flooding",						/* IW_FLOOD_PROBE_REQ_EVENT_FLAG */
    "detected disassociation flooding",						/* IW_FLOOD_DISASSOC_EVENT_FLAG */
    "detected deauthentication flooding",					/* IW_FLOOD_DEAUTH_EVENT_FLAG */
    "detected 802.1x eap-request flooding"					/* IW_FLOOD_EAP_REQ_EVENT_FLAG */
	};
#endif /* IDS_SUPPORT */


#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */
#endif /* SYSTEM_LOG_SUPPORT */


static char *RT2870STA_dat =
"#The word of \"Default\" must not be removed\n"
"Default\n"
"CountryRegion=5\n"
"CountryRegionABand=7\n"
"CountryCode=\n"
"ChannelGeography=1\n"
"\nSSID=dummy-dummy"
"NetworkType=Infra\n"
"WirelessMode=13\n"
"EfuseBufferMode=0\n"
"Channel=0\n"
"BeaconPeriod=100\n"
"TxPower=100\n"
"BGProtection=0\n"
"TxPreamble=0\n"
"RTSThreshold=2347\n"
"FragThreshold=2346\n"
"TxBurst=1\n"
"PktAggregate=0\n"
"WmmCapable=0\n"
"AckPolicy=0;0;0;0\n"
"AuthMode=OPEN\n"
"EncrypType=NONE\n"
"WPAPSK=\n"
"DefaultKeyID=1\n"
"Key1Type=0\n"
"Key1Str=\n"
"Key2Type=0\n"
"Key2Str=\n"
"Key3Type=0\n"
"Key3Str=\n"
"Key4Type=0\n"
"Key4Str=\n"
"PSMode=CAM\n"
"AutoRoaming=0\n"
"RoamThreshold=70\n"
"APSDCapable=0\n"
"APSDAC=0;0;0;0\n"
"HT_RDG=1\n"
"HT_EXTCHA=0\n"
"HT_OpMode=0\n"
"HT_MpduDensity=4\n"
"HT_BW=1\n"
"HT_BADecline=0\n"
"HT_AutoBA=1\n"
"HT_AMSDU=0\n"
"HT_BAWinSize=64\n"
"HT_GI=1\n"
"HT_MCS=33\n"
"HT_MIMOPSMode=3\n"
"HT_DisallowTKIP=1\n"
"HT_STBC=0\n"
"VHT_BW=1\n"
"VHT_SGI=1\n"
"VHT_STBC=0\n"
"EthConvertMode=\n"
"EthCloneMac=\n"
"IEEE80211H=0\n"
"TGnWifiTest=0\n"
"WirelessEvent=0\n"
"MeshId=MESH\n"
"MeshAutoLink=1\n"
"MeshAuthMode=OPEN\n"
"MeshEncrypType=NONE\n"
"MeshWPAKEY=\n"
"MeshDefaultkey=1\n"
"MeshWEPKEY=\n"
"CarrierDetect=0\n"
"AntDiversity=0\n"
"BeaconLostTime=4\n"
"FtSupport=0\n"
"Wapiifname=ra0\n"
"WapiPsk=\n"
"WapiPskType=\n"
"WapiUserCertPath=\n"
"WapiAsCertPath=\n"
"PSP_XLINK_MODE=0\n"
"WscManufacturer=\n"
"WscModelName=\n"
"WscDeviceName=\n"
"WscModelNumber=\n"
"WscSerialNumber=\n"
"RadioOn=1\n"
"WIDIEnable=1\n"
"P2P_L2SD_SCAN_TOGGLE=3\n"
"Wsc4digitPinCode=0\n"
"P2P_WIDIEnable=0\n"
"PMFMFPC=0\n"
"PMFMFPR=0\n"
"PMFSHA256=0\n";

NDIS_STATUS	RTMPReadParametersHook(
	IN	struct rtmp_adapter *pAd)
{
	INT   retval = NDIS_STATUS_FAILURE;
	char *buffer;

#ifdef HOSTAPD_SUPPORT
	int i;
#endif /*HOSTAPD_SUPPORT */

/*	buffer = kmalloc(MAX_INI_BUFFER_SIZE, MEM_ALLOC_FLAG); */
	os_alloc_mem(pAd, (UCHAR **)&buffer, MAX_INI_BUFFER_SIZE);
	if(buffer == NULL)
		return NDIS_STATUS_FAILURE;
	memset(buffer, 0x00, MAX_INI_BUFFER_SIZE);

	{
		{
			strcpy(buffer, RT2870STA_dat);
			RTMPSetProfileParameters(pAd, buffer);
			retval = NDIS_STATUS_SUCCESS;

		}

	}

#ifdef HOSTAPD_SUPPORT
		for (i = 0; i < pAd->ApCfg.BssidNum; i++)
		{
			pAd->ApCfg.MBSSID[i].Hostapd=FALSE;
			DBGPRINT(RT_DEBUG_TRACE, ("Reset ra%d hostapd support=FLASE", i));

		}
#endif /*HOSTAPD_SUPPORT */

#ifdef SINGLE_SKU_V2
	RTMPSetSingleSKUParameters(pAd);
#endif /* SINGLE_SKU_V2 */

/*	kfree(buffer); */
	kfree(buffer);

	return (retval);

}


#ifdef SYSTEM_LOG_SUPPORT
/*
	========================================================================

	Routine Description:
		Send log message through wireless event

		Support standard iw_event with IWEVCUSTOM. It is used below.

		iwreq_data.data.flags is used to store event_flag that is defined by user.
		iwreq_data.data.length is the length of the event log.

		The format of the event log is composed of the entry's MAC address and
		the desired log message (refer to pWirelessEventText).

			ex: 11:22:33:44:55:66 has associated successfully

		p.s. The requirement of Wireless Extension is v15 or newer.

	========================================================================
*/
void RtmpDrvSendWirelessEvent(
	IN	void 				*pAdSrc,
	IN	USHORT					Event_flag,
	IN	PUCHAR 					pAddr,
	IN  UCHAR					BssIdx,
	IN	CHAR					Rssi)
{
	struct rtmp_adapter *pAd = (struct rtmp_adapter *)pAdSrc;
	char *pBuf = NULL, *pBufPtr = NULL;
	USHORT	event, type, BufLen;
	UCHAR	event_table_len = 0;

	if (pAd->CommonCfg.bWirelessEvent == FALSE)
		return;

	type = Event_flag & 0xFF00;
	event = Event_flag & 0x00FF;

	switch (type)
	{
		case IW_SYS_EVENT_FLAG_START:
			event_table_len = IW_SYS_EVENT_TYPE_NUM;
			break;
#ifdef IDS_SUPPORT
		case IW_SPOOF_EVENT_FLAG_START:
			event_table_len = IW_SPOOF_EVENT_TYPE_NUM;
			break;

		case IW_FLOOD_EVENT_FLAG_START:
			event_table_len = IW_FLOOD_EVENT_TYPE_NUM;
			break;
#endif /* IDS_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */
	}

	if (event_table_len == 0)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : The type(%0x02x) is not valid.\n", __FUNCTION__, type));
		return;
	}

	if (event >= event_table_len)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : The event(%0x02x) is not valid.\n", __FUNCTION__, event));
		return;
	}

	/*Allocate memory and copy the msg. */
/*	if((pBuf = kmalloc(IW_CUSTOM_MAX_LEN, GFP_ATOMIC)) != NULL) */
	os_alloc_mem(NULL, (UCHAR **)&pBuf, IW_CUSTOM_MAX_LEN);
	if(pBuf != NULL)
	{
		/*Prepare the payload */
		memset(pBuf, 0, IW_CUSTOM_MAX_LEN);

		pBufPtr = pBuf;

		if (pAddr)
			pBufPtr += sprintf(pBufPtr, "(RT2860) STA(%02x:%02x:%02x:%02x:%02x:%02x) ", PRINT_MAC(pAddr));
		else if (BssIdx < MAX_MBSSID_NUM(pAd))
			pBufPtr += sprintf(pBufPtr, "(RT2860) BSS(ra%d) ", BssIdx);
		else
			pBufPtr += sprintf(pBufPtr, "(RT2860) ");

		if (type == IW_SYS_EVENT_FLAG_START)
        {
			pBufPtr += sprintf(pBufPtr, "%s", pWirelessSysEventText[event]);

            if (Event_flag == IW_CHANNEL_CHANGE_EVENT_FLAG)
		  	{
			 	pBufPtr += sprintf(pBufPtr, "%3d", Rssi);
			}
		}
#ifdef IDS_SUPPORT
		else if (type == IW_SPOOF_EVENT_FLAG_START)
			pBufPtr += sprintf(pBufPtr, "%s (RSSI=%d)", pWirelessSpoofEventText[event], Rssi);
		else if (type == IW_FLOOD_EVENT_FLAG_START)
			pBufPtr += sprintf(pBufPtr, "%s", pWirelessFloodEventText[event]);
#endif /* IDS_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */
		else
			pBufPtr += sprintf(pBufPtr, "%s", "unknown event");

		pBufPtr[pBufPtr - pBuf] = '\0';
		BufLen = pBufPtr - pBuf;

		RtmpOSWrielessEventSend(pAd->net_dev, RT_WLAN_EVENT_CUSTOM, Event_flag, NULL, (PUCHAR)pBuf, BufLen);
		/*DBGPRINT(RT_DEBUG_TRACE, ("%s : %s\n", __FUNCTION__, pBuf)); */

/*		kfree(pBuf); */
		kfree(pBuf);
	}
	else
		DBGPRINT(RT_DEBUG_ERROR, ("%s : Can't allocate memory for wireless event.\n", __FUNCTION__));
}
#endif /* SYSTEM_LOG_SUPPORT */


void RTMP_IndicateMediaState(
	IN	struct rtmp_adapter *	pAd,
	IN  NDIS_MEDIA_STATE	media_state)
{
	pAd->IndicateMediaState = media_state;

#ifdef SYSTEM_LOG_SUPPORT
		if (pAd->IndicateMediaState == NdisMediaStateConnected)
		{
			RTMPSendWirelessEvent(pAd, IW_STA_LINKUP_EVENT_FLAG, pAd->MacTab.Content[BSSID_WCID].Addr, BSS0, 0);
		}
		else
		{
			RTMPSendWirelessEvent(pAd, IW_STA_LINKDOWN_EVENT_FLAG, pAd->MacTab.Content[BSSID_WCID].Addr, BSS0, 0);
		}
#endif /* SYSTEM_LOG_SUPPORT */
}


void tbtt_tasklet(unsigned long data)
{
}


void announce_802_3_packet(
	IN void *pAdSrc,
	IN PNDIS_PACKET pPacket,
	IN UCHAR OpMode)
{
	struct rtmp_adapter*pAd = (struct rtmp_adapter*)pAdSrc;
	PNDIS_PACKET pRxPkt = pPacket;

	ASSERT(pPacket);
	MEM_DBG_PKT_FREE_INC(pPacket);



#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */

    /* Push up the protocol stack */

#ifdef INF_PPA_SUPPORT
	if (ppa_hook_directpath_send_fn && pAd->PPAEnable==TRUE )
	{
		RtmpOsPktInfPpaSend(pRxPkt);
		pRxPkt=NULL;
		return;
	}
#endif /* INF_PPA_SUPPORT */

	{
#ifdef CONFIG_RT2880_BRIDGING_ONLY
		PACKET_CB_ASSIGN(pRxPkt, 22) = 0xa8;
#endif

#if defined(CONFIG_RA_CLASSIFIER)||defined(CONFIG_RA_CLASSIFIER_MODULE)
		if(ra_classifier_hook_rx!= NULL)
		{
			unsigned int flags;

			RTMP_IRQ_LOCK(&pAd->page_lock, flags);
			ra_classifier_hook_rx(pRxPkt, classifier_cur_cycle);
			RTMP_IRQ_UNLOCK(&pAd->page_lock, flags);
		}
#endif /* CONFIG_RA_CLASSIFIER */

	}


		RtmpOsPktProtocolAssign(pRxPkt);
		RtmpOsPktRcvHandle(pRxPkt);
}


#ifdef CONFIG_STA_SUPPORT
void STA_MonPktSend(
	IN struct rtmp_adapter*pAd,
	IN RX_BLK *pRxBlk)
{
	PNET_DEV pNetDev;
	PNDIS_PACKET pRxPacket;
	PHEADER_802_11 pHeader;
	USHORT DataSize;
	UINT32 MaxRssi;
	UCHAR L2PAD, PHYMODE, BW, ShortGI, MCS, AMPDU, STBC, RSSI1;
	UCHAR BssMonitorFlag11n, Channel, CentralChannel;
	UCHAR *pData, *pDevName;


	/* sanity check */
    ASSERT(pRxBlk->pRxPacket);
    if (pRxBlk->DataSize < 10)
    {
        DBGPRINT(RT_DEBUG_ERROR, ("%s : Size is too small! (%d)\n", __FUNCTION__, pRxBlk->DataSize));
		goto err_free_sk_buff;
    }

    if (pRxBlk->DataSize + sizeof(wlan_ng_prism2_header) > RX_BUFFER_AGGRESIZE)
    {
        DBGPRINT(RT_DEBUG_ERROR, ("%s : Size is too large! (%d)\n", __FUNCTION__, pRxBlk->DataSize + sizeof(wlan_ng_prism2_header)));
		goto err_free_sk_buff;
    }

	/* init */
	MaxRssi = RTMPMaxRssi(pAd,
						ConvertToRssi(pAd, pRxBlk->pRxWI->RxWIRSSI0, RSSI_0),
						ConvertToRssi(pAd, pRxBlk->pRxWI->RxWIRSSI1, RSSI_1),
						ConvertToRssi(pAd, pRxBlk->pRxWI->RxWIRSSI2, RSSI_2));

	pNetDev = get_netdev_from_bssid(pAd, BSS0);
	pRxPacket = pRxBlk->pRxPacket;
	pHeader = pRxBlk->pHeader;
	pData = pRxBlk->pData;
	DataSize = pRxBlk->DataSize;
	L2PAD = pRxBlk->pRxInfo->L2PAD;
	PHYMODE = pRxBlk->pRxWI->RxWIPhyMode;
	BW = pRxBlk->pRxWI->RxWIBW;
	ShortGI = pRxBlk->pRxWI->RxWISGI;
	MCS = pRxBlk->pRxWI->RxWIMCS;
	AMPDU = pRxBlk->pRxInfo->AMPDU;
	STBC = pRxBlk->pRxWI->RxWISTBC;
	RSSI1 = pRxBlk->pRxWI->RxWIRSSI1;
	BssMonitorFlag11n = 0;
#ifdef MONITOR_FLAG_11N_SNIFFER_SUPPORT
	BssMonitorFlag11n = (pAd->StaCfg.BssMonitorFlag & MONITOR_FLAG_11N_SNIFFER);
#endif /* MONITOR_FLAG_11N_SNIFFER_SUPPORT */
	pDevName = (UCHAR *)RtmpOsGetNetDevName(pAd->net_dev);
	Channel = pAd->CommonCfg.Channel;
	CentralChannel = pAd->CommonCfg.CentralChannel;

	/* pass the packet */
	send_monitor_packets(pNetDev, pRxPacket, pHeader, pData, DataSize,
						L2PAD, PHYMODE, BW, ShortGI, MCS, AMPDU, STBC, RSSI1,
						BssMonitorFlag11n, pDevName, Channel, CentralChannel,
						MaxRssi);
	return;

err_free_sk_buff:
	RELEASE_NDIS_PACKET(pAd, pRxBlk->pRxPacket, NDIS_STATUS_FAILURE);
	return;
}
#endif /* CONFIG_STA_SUPPORT */


extern NDIS_SPIN_LOCK TimerSemLock;

void RTMPFreeAdapter(
	IN	void 	*pAdSrc)
{
	struct rtmp_adapter *pAd = (struct rtmp_adapter *)pAdSrc;
	POS_COOKIE os_cookie;
	int index;

	os_cookie=(POS_COOKIE)pAd->OS_Cookie;

	if (pAd->BeaconBuf)
		kfree(pAd->BeaconBuf);


	NdisFreeSpinLock(&pAd->MgmtRingLock);


#if defined(RT3290) || defined(RT65xx)
#endif /* defined(RT3290) || defined(RT65xx) */

	for (index =0 ; index < NUM_OF_TX_RING; index++)
	{
		NdisFreeSpinLock(&pAd->TxSwQueueLock[index]);
		NdisFreeSpinLock(&pAd->DeQueueLock[index]);
		pAd->DeQueueRunning[index] = FALSE;
	}

	NdisFreeSpinLock(&pAd->irq_lock);



#ifdef UAPSD_SUPPORT
	NdisFreeSpinLock(&pAd->UAPSDEOSPLock); /* OS_ABL_SUPPORT */
#endif /* UAPSD_SUPPORT */

#ifdef DOT11_N_SUPPORT
	NdisFreeSpinLock(&pAd->mpdu_blk_pool.lock);
#endif /* DOT11_N_SUPPORT */

	if (pAd->iw_stats)
	{
		kfree(pAd->iw_stats);
		pAd->iw_stats = NULL;
	}
	if (pAd->stats)
	{
		kfree(pAd->stats);
		pAd->stats = NULL;
	}

	NdisFreeSpinLock(&TimerSemLock);

#ifdef RALINK_ATE
#ifdef RTMP_MAC_USB
	RTMP_OS_ATMOIC_DESTROY(&pAd->BulkOutRemained);
	RTMP_OS_ATMOIC_DESTROY(&pAd->BulkInRemained);
#endif /* RTMP_MAC_USB */
#endif /* RALINK_ATE */

	RTMP_OS_FREE_TIMER(pAd);
	RTMP_OS_FREE_LOCK(pAd);
	RTMP_OS_FREE_TASKLET(pAd);
	RTMP_OS_FREE_TASK(pAd);
	RTMP_OS_FREE_SEM(pAd);
	RTMP_OS_FREE_ATOMIC(pAd);

	RtmpOsVfree(pAd); /* pci_free_consistent(os_cookie->pci_dev,sizeof(struct rtmp_adapter),pAd,os_cookie->pAd_pa); */
	if (os_cookie)
		kfree(os_cookie);
}


int	RTMPSendPackets(
	IN NDIS_HANDLE dev_hnd,
	IN PPNDIS_PACKET ppPacketArray,
	IN UINT NumberOfPackets,
	IN UINT32 PktTotalLen,
	IN RTMP_NET_ETH_CONVERT_DEV_SEARCH Func)
{
	struct rtmp_adapter*pAd = (struct rtmp_adapter*)dev_hnd;
	PNDIS_PACKET pPacket = ppPacketArray[0];


	INC_COUNTER64(pAd->WlanCounters.TransmitCountFrmOs);

	if (pPacket == NULL)
		goto done;

	/* RT2870STA does this in RTMPSendPackets() */
#ifdef RALINK_ATE
	if (ATE_ON(pAd))
	{
		RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_RESOURCES);
		return 0;
	}
#endif /* RALINK_ATE */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* Drop send request since we are in monitor mode */
		if (MONITOR_ON(pAd))
		{
			RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_FAILURE);
			return 0;
		}
	}
#endif /* CONFIG_STA_SUPPORT */

        /* EapolStart size is 18 */
	if (PktTotalLen < 14)
	{
		/*printk("bad packet size: %d\n", pkt->len); */
		hex_dump("bad packet", GET_OS_PKT_DATAPTR(pPacket), PktTotalLen);
		RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_FAILURE);
		return 0;
	}



	RTMP_SET_PACKET_5VT(pPacket, 0);
/*	MiniportMMRequest(pAd, pkt->data, pkt->len); */
#ifdef CONFIG_5VT_ENHANCE
    if (*(int*)(GET_OS_PKT_CB(pPacket)) == BRIDGE_TAG) {
		RTMP_SET_PACKET_5VT(pPacket, 1);
    }
#endif


#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{

		STASendPackets((NDIS_HANDLE)pAd, (PPNDIS_PACKET) &pPacket, 1);
	}

#endif /* CONFIG_STA_SUPPORT */

done:
	return 0;
}


PNET_DEV get_netdev_from_bssid(
	IN	struct rtmp_adapter *pAd,
	IN	UCHAR			FromWhichBSSID)
{
	PNET_DEV dev_p = NULL;

	do
	{
#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */
		{
			dev_p = pAd->net_dev;
		}

	} while (FALSE);

	ASSERT(dev_p);
	return dev_p; /* return one of MBSS */
}




#ifdef WDS_SUPPORT
void AP_WDS_KeyNameMakeUp(
	IN	STRING						*pKey,
	IN	UINT32						KeyMaxSize,
	IN	INT							KeyId)
{
	snprintf(pKey, KeyMaxSize, "Wds%dKey", KeyId);
}
#endif /* WDS_SUPPORT */

