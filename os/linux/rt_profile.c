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

void RTMP_IndicateMediaState(
	IN	struct rtmp_adapter *	pAd,
	IN  NDIS_MEDIA_STATE	media_state)
{
	pAd->IndicateMediaState = media_state;
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
	struct net_device *pNetDev;
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


struct net_device *get_netdev_from_bssid(
	IN	struct rtmp_adapter *pAd,
	IN	UCHAR			FromWhichBSSID)
{
	struct net_device *dev_p = NULL;

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




