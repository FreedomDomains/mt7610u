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


#define RTMP_MODULE_OS
#define RTMP_MODULE_OS_UTIL

#include "rtmp_comm.h"
#include "rtmp_osabl.h"
#include "rt_os_util.h"

/* TODO */
#undef RT_CONFIG_IF_OPMODE_ON_AP
#undef RT_CONFIG_IF_OPMODE_ON_STA

#define RT_CONFIG_IF_OPMODE_ON_AP(__OpMode)
#define RT_CONFIG_IF_OPMODE_ON_STA(__OpMode)

int RTDebugLevel = RT_DEBUG_ERROR;
int RTDebugFunc = 0;

/*
 * the lock will not be used in TX/RX
 * path so throughput should not be impacted
 */
bool FlgIsUtilInit = false;

bool RTMP_OS_Alloc_RscOnly(void *pRscSrc, u32 RscLen);
bool RTMP_OS_Remove_Rsc(LIST_HEADER *pRscList, void *pRscSrc);
/*
========================================================================
Routine Description:
	Initialize something in UTIL module.

Arguments:
	None

Return Value:
	None

Note:
========================================================================
*/
void RtmpUtilInit(void)
{
	if (FlgIsUtilInit == false) {
		FlgIsUtilInit = true;
	}
}

ULONG RTMPMsecsToJiffies(u32 m)
{

	return msecs_to_jiffies(m);
}

/* pAd MUST allow to be NULL */

int os_alloc_mem_suspend(
	IN void *pReserved,
	OUT u8 **mem,
	IN ULONG size)
{
	*mem = (u8 *) kmalloc(size, GFP_KERNEL);
	if (*mem) {

		return NDIS_STATUS_SUCCESS;
	} else
		return NDIS_STATUS_FAILURE;
}

struct sk_buff *RTMPAllocateNdisPacket(u8 *pHeader, UINT HeaderLen,
				       u8 *pData, UINT DataLen)
{
	struct sk_buff *skb;

	skb = dev_alloc_skb(HeaderLen + DataLen + RTMP_PKT_TAIL_PADDING);
	if (skb == NULL) {
#ifdef DEBUG
		printk(KERN_ERR "RTMPAllocateNdisPacket Fail\n\n");
#endif
		return NULL;
	}

	/* Clone the frame content and update the length of packet */
	if (HeaderLen > 0)
		memmove(skb->data, pHeader, HeaderLen);
	if (DataLen > 0)
		memmove(skb->data + HeaderLen, pData, DataLen);
	skb_put(skb, HeaderLen + DataLen);
/* printk(KERN_ERR "%s : pPacket = %p, len = %d\n", __FUNCTION__, pPacket, pPacket->len);*/

	return skb;
}


/* IRQL = DISPATCH_LEVEL */
/* NOTE: we do have an assumption here, that Byte0 and Byte1
 * always reasid at the same scatter gather buffer
 */
int Sniff2BytesFromNdisBuffer(
	IN char *pFirstBuffer,
	IN u8 DesiredOffset,
	OUT u8 *pByte0,
	OUT u8 *pByte1)
{
	*pByte0 = *(u8 *) (pFirstBuffer + DesiredOffset);
	*pByte1 = *(u8 *) (pFirstBuffer + DesiredOffset + 1);

	return NDIS_STATUS_SUCCESS;
}


void RTMP_QueryPacketInfo(
	IN struct sk_buff * pPacket,
	OUT PACKET_INFO *info,
	OUT u8 **pSrcBufVA,
	OUT UINT *pSrcBufLen)
{
	info->BufferCount = 1;
	info->pFirstBuffer = pPacket->data;
	info->PhysicalBufferCount = 1;
	info->TotalPacketLength = pPacket->len;

	*pSrcBufVA = pPacket->data;
	*pSrcBufLen = pPacket->len;
}




struct sk_buff * DuplicatePacket(
	IN struct net_device *pNetDev,
	IN struct sk_buff * pPacket,
	IN u8 FromWhichBSSID)
{
	struct sk_buff *skb;
	struct sk_buff * pRetPacket = NULL;
	USHORT DataSize;
	u8 *pData;

	DataSize = pPacket->len;
	pData = pPacket->data;

	skb = skb_clone(pPacket, MEM_ALLOC_FLAG);
	if (skb) {
		skb->dev = pNetDev;	/*get_netdev_from_bssid(pAd, FromWhichBSSID); */
		pRetPacket = skb;
	}

	return pRetPacket;

}


struct sk_buff * duplicate_pkt(
	IN struct net_device *pNetDev,
	IN u8 *pHeader802_3,
	IN UINT HdrLen,
	IN u8 *pData,
	IN ULONG DataSize,
	IN u8 FromWhichBSSID)
{
	struct sk_buff *skb;
	struct sk_buff * pPacket = NULL;

	if ((skb =
	     __dev_alloc_skb(HdrLen + DataSize + 2, MEM_ALLOC_FLAG)) != NULL) {

		skb_reserve(skb, 2);
		memmove(skb_tail_pointer(skb), pHeader802_3, HdrLen);
		skb_put(skb, HdrLen);
		memmove(skb_tail_pointer(skb), pData, DataSize);
		skb_put(skb, DataSize);
		skb->dev = pNetDev;	/*get_netdev_from_bssid(pAd, FromWhichBSSID); */
		pPacket = skb;
	}

	return pPacket;
}


#define TKIP_TX_MIC_SIZE		8
struct sk_buff * duplicate_pkt_with_TKIP_MIC(
	IN void *pReserved,
	IN struct sk_buff * pPacket)
{
	struct sk_buff *skb, *newskb;

	skb = pPacket;
	if (skb_tailroom(skb) < TKIP_TX_MIC_SIZE) {
		/* alloc a new skb and copy the packet */
		newskb = skb_copy_expand(skb, skb_headroom(skb), TKIP_TX_MIC_SIZE, GFP_ATOMIC);

		dev_kfree_skb_any(skb);

		if (newskb == NULL) {
			DBGPRINT(RT_DEBUG_ERROR, ("Extend Tx.MIC for packet failed!, dropping packet!\n"));
			return NULL;
		}
		skb = newskb;
	}

	return skb;


}


/*
	========================================================================

	Routine Description:
		Send a L2 frame to upper daemon to trigger state machine

	Arguments:
		pAd - pointer to our pAdapter context

	Return Value:

	Note:

	========================================================================
*/
bool RTMPL2FrameTxAction(
	IN void * pCtrlBkPtr,
	IN struct net_device *pNetDev,
	IN RTMP_CB_8023_PACKET_ANNOUNCE _announce_802_3_packet,
	IN u8 apidx,
	IN u8 *pData,
	IN u32 data_len,
	IN	u8 		OpMode)
{
	struct sk_buff *skb = dev_alloc_skb(data_len + 2);

	if (!skb) {
		DBGPRINT(RT_DEBUG_ERROR,
			 ("%s : Error! Can't allocate a skb.\n", __FUNCTION__));
		return false;
	}

	/*get_netdev_from_bssid(pAd, apidx)); */
	skb->dev = pNetDev;

	/* 16 byte align the IP header */
	skb_reserve(skb, 2);

	/* Insert the frame content */
	memmove(skb->data, pData, data_len);

	/* End this frame */
	skb_put(skb, data_len);

	DBGPRINT(RT_DEBUG_TRACE, ("%s doen\n", __FUNCTION__));

	_announce_802_3_packet(pCtrlBkPtr, skb, OpMode);

	return true;

}


struct sk_buff * ExpandPacket(
	IN void *pReserved,
	IN struct sk_buff * pPacket,
	IN u32 ext_head_len,
	IN u32 ext_tail_len)
{
	struct sk_buff *skb, *newskb;

	skb = pPacket;
	if (skb_cloned(skb) || (skb_headroom(skb) < ext_head_len)
	    || (skb_tailroom(skb) < ext_tail_len)) {
		u32 head_len =
		    (skb_headroom(skb) <
		     ext_head_len) ? ext_head_len : skb_headroom(skb);
		u32 tail_len =
		    (skb_tailroom(skb) <
		     ext_tail_len) ? ext_tail_len : skb_tailroom(skb);

		/* alloc a new skb and copy the packet */
		newskb = skb_copy_expand(skb, head_len, tail_len, GFP_ATOMIC);

		dev_kfree_skb_any(skb);

		if (newskb == NULL) {
			DBGPRINT(RT_DEBUG_ERROR,
				 ("Extend Tx buffer for WPI failed!, dropping packet!\n"));
			return NULL;
		}
		skb = newskb;
	}

	return skb;

}

struct sk_buff * ClonePacket(
	IN void *pReserved,
	IN struct sk_buff * pPacket,
	IN u8 *pData,
	IN ULONG DataSize)
{
	struct sk_buff *pRxPkt;
	struct sk_buff *pClonedPkt;

	ASSERT(pPacket);
	pRxPkt = pPacket;

	/* clone the packet */
	pClonedPkt = skb_clone(pRxPkt, MEM_ALLOC_FLAG);

	if (pClonedPkt) {
		/* set the correct dataptr and data len */
		pClonedPkt->dev = pRxPkt->dev;
		pClonedPkt->data = pData;
		pClonedPkt->len = DataSize;
		skb_set_tail_pointer(pClonedPkt, pClonedPkt->len);
		ASSERT(DataSize < 1530);
	}
	return pClonedPkt;
}

void RtmpOsPktInit(
	IN struct sk_buff * pNetPkt,
	IN struct net_device *pNetDev,
	IN u8 *pData,
	IN USHORT DataSize)
{
	struct sk_buff * pRxPkt;

	pRxPkt = pNetPkt;

	pRxPkt->dev = pNetDev;
	pRxPkt->data = pData;
	pRxPkt->len = DataSize;
	skb_set_tail_pointer(pRxPkt, DataSize);
}


void wlan_802_11_to_802_3_packet(
	IN struct net_device *pNetDev,
	IN u8 OpMode,
	IN USHORT VLAN_VID,
	IN USHORT VLAN_Priority,
	IN struct sk_buff * skb,
	IN u8 *pData,
	IN ULONG DataSize,
	IN u8 *pHeader802_3,
	IN u8 FromWhichBSSID,
	IN u8 *TPID)
{
	struct sk_buff *pOSPkt;

	ASSERT(pHeader802_3);

	pOSPkt = skb;

	/*get_netdev_from_bssid(pAd, FromWhichBSSID); */
	pOSPkt->dev = pNetDev;
	pOSPkt->data = pData;
	pOSPkt->len = DataSize;
	skb_set_tail_pointer(pOSPkt, pOSPkt->len);

	/* copy 802.3 header */

#ifdef CONFIG_STA_SUPPORT
	RT_CONFIG_IF_OPMODE_ON_STA(OpMode)
	{
	    memmove(skb_push(pOSPkt, LENGTH_802_3), pHeader802_3, LENGTH_802_3);
	}
#endif /* CONFIG_STA_SUPPORT */

}

#ifdef CONFIG_STA_SUPPORT
INT32 ralinkrate[] = {
	2,  4, 11, 22,
	12, 18, 24, 36, 48, 72, 96, 108, 109, 110, 111, 112, /* CCK and OFDM */
	13, 26, 39, 52, 78, 104, 117, 130, 26, 52, 78, 104, 156, 208, 234, 260,
	39, 78, 117, 156, 234, 312, 351, 390, /* BW 20, 800ns GI, MCS 0~23 */
	27, 54, 81, 108, 162, 216, 243, 270, 54, 108, 162, 216, 324, 432, 486, 540,
	81, 162, 243, 324, 486, 648, 729, 810, /* BW 40, 800ns GI, MCS 0~23 */
	14, 29, 43, 57, 87, 115, 130, 144, 29, 59, 87, 115, 173, 230, 260, 288,
	43, 87, 130, 173, 260, 317, 390, 433, /* BW 20, 400ns GI, MCS 0~23 */
	30, 60, 90, 120, 180, 240, 270, 300, 60, 120, 180, 240, 360, 480, 540, 600,
	90, 180, 270, 360, 540, 720, 810, 900, /* BW 40, 400ns GI, MCS 0~23 */
	13, 26, 39, 52, 78, 104, 117, 130, 156, /* AC: 20Mhz, 800ns GI, MCS: 0~8 */
	27, 54, 81, 108, 162, 216, 243, 270, 324, 360, /* AC: 40Mhz, 800ns GI, MCS: 0~9 */
	59, 117, 176, 234, 351, 468, 527, 585, 702, 780, /* AC: 80Mhz, 800ns GI, MCS: 0~9 */
	14, 29, 43, 57, 87, 115, 130, 144, 173, /* AC: 20Mhz, 400ns GI, MCS: 0~8 */
	30, 60, 90, 120, 180, 240, 270, 300, 360, 400, /* AC: 40Mhz, 400ns GI, MCS: 0~9 */
	65, 130, 195, 260, 390, 520, 585, 650, 780, 867, /* AC: 80Mhz, 400ns GI, MCS: 0~9 */
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
	40, 41, 42, 43, 44, 45, 46, 47 /* 3*3 */
};

u32 RT_RateSize = sizeof (ralinkrate);

void send_monitor_packets(struct net_device *ndev, struct sk_buff *skb,
			  PHEADER_802_11 pHeader, u8 * pData,
			  USHORT DataSize, u8 L2PAD,
			  u8 PHYMODE, u8 BW, u8 ShortGI, u8 MCS,
			  u8 AMPDU, u8 STBC, u8 RSSI1,
			  IN u8 BssMonitorFlag11n,
			  u8 * pDevName, u8 Channel,
			  u8 CentralChannel, u32 MaxRssi)
{
	wlan_ng_prism2_header *ph;
	int rate_index = 0;
	USHORT header_len = 0;
	u8 temp_header[40] = { 0 };

	skb->dev = ndev;	/*get_netdev_from_bssid(pAd, BSS0); */
	if (pHeader->FC.Type == BTYPE_DATA) {
		DataSize -= LENGTH_802_11;
		if ((pHeader->FC.ToDs == 1) && (pHeader->FC.FrDs == 1))
			header_len = LENGTH_802_11_WITH_ADDR4;
		else
			header_len = LENGTH_802_11;

		/* QOS */
		if (pHeader->FC.SubType & 0x08) {
			header_len += 2;
			/* Data skip QOS contorl field */
			DataSize -= 2;
		}

		/* Order bit: A-Ralink or HTC+ */
		if (pHeader->FC.Order) {
			header_len += 4;
			/* Data skip HTC contorl field */
			DataSize -= 4;
		}

		/* Copy Header */
		if (header_len <= 40)
			memmove(temp_header, pData, header_len);

		/* skip HW padding */
		if (L2PAD)
			pData += (header_len + 2);
		else
			pData += header_len;
	}

	if (DataSize < skb->len) {
		skb_trim(skb, DataSize);
	} else {
		skb_put(skb, (DataSize - skb->len));
	}

	if ((pData - skb->data) > 0) {
		skb_put(skb, (pData - skb->data));
		skb_pull(skb, (pData - skb->data));
	}

	if (skb_headroom(skb) < (sizeof (wlan_ng_prism2_header) + header_len)) {
		if (pskb_expand_head(skb, (sizeof (wlan_ng_prism2_header) + header_len), 0, GFP_ATOMIC)) {
			DBGPRINT(RT_DEBUG_ERROR,
				 ("%s : Reallocate header size of sk_buff fail!\n",
				  __FUNCTION__));
			goto err_free_sk_buff;
		}
	}

	if (header_len > 0)
		memmove(skb_push(skb, header_len), temp_header,
			       header_len);

	{
		ph = (wlan_ng_prism2_header *) skb_push(skb,
							sizeof(wlan_ng_prism2_header));
		memset(ph, 0, sizeof(wlan_ng_prism2_header));

		ph->msgcode = DIDmsg_lnxind_wlansniffrm;
		ph->msglen = sizeof (wlan_ng_prism2_header);
		strcpy((char *) ph->devname, (char *) pDevName);

		ph->hosttime.did = DIDmsg_lnxind_wlansniffrm_hosttime;
		ph->hosttime.status = 0;
		ph->hosttime.len = 4;
		ph->hosttime.data = jiffies;

		ph->mactime.did = DIDmsg_lnxind_wlansniffrm_mactime;
		ph->mactime.status = 0;
		ph->mactime.len = 0;
		ph->mactime.data = 0;

		ph->istx.did = DIDmsg_lnxind_wlansniffrm_istx;
		ph->istx.status = 0;
		ph->istx.len = 0;
		ph->istx.data = 0;

		ph->channel.did = DIDmsg_lnxind_wlansniffrm_channel;
		ph->channel.status = 0;
		ph->channel.len = 4;

		ph->channel.data = (u_int32_t) Channel;

		ph->rssi.did = DIDmsg_lnxind_wlansniffrm_rssi;
		ph->rssi.status = 0;
		ph->rssi.len = 4;
		ph->rssi.data = MaxRssi;
		ph->signal.did = DIDmsg_lnxind_wlansniffrm_signal;
		ph->signal.status = 0;
		ph->signal.len = 4;
		ph->signal.data = 0;	/*rssi + noise; */

		ph->noise.did = DIDmsg_lnxind_wlansniffrm_noise;
		ph->noise.status = 0;
		ph->noise.len = 4;
		ph->noise.data = 0;

		if (PHYMODE >= MODE_HTMIX) {
			rate_index = 12 + ((u8) BW * 24) + ((u8) ShortGI * 48) + ((u8) MCS);
		} else
		if (PHYMODE == MODE_OFDM)
			rate_index = (u8) (MCS) + 4;
		else
			rate_index = (u8) (MCS);

		if (rate_index < 0)
			rate_index = 0;
		if (rate_index >= (sizeof (ralinkrate) / sizeof (ralinkrate[0])))
			rate_index = (sizeof (ralinkrate) / sizeof (ralinkrate[0])) - 1;

		ph->rate.did = DIDmsg_lnxind_wlansniffrm_rate;
		ph->rate.status = 0;
		ph->rate.len = 4;
		/* real rate = ralinkrate[rate_index] / 2 */
		ph->rate.data = ralinkrate[rate_index];

		ph->frmlen.did = DIDmsg_lnxind_wlansniffrm_frmlen;
		ph->frmlen.status = 0;
		ph->frmlen.len = 4;
		ph->frmlen.data = (u_int32_t) DataSize;
	}

	skb->pkt_type = PACKET_OTHERHOST;
	skb->protocol = eth_type_trans(skb, skb->dev);
	skb->ip_summed = CHECKSUM_NONE;
	netif_rx(skb);

	return;

err_free_sk_buff:
	dev_kfree_skb_any(skb);
	return;

}
#endif /* CONFIG_STA_SUPPORT */


/*******************************************************************************

	File open/close related functions.

 *******************************************************************************/
RTMP_OS_FD RtmpOSFileOpen(char *pPath, int flag, int mode)
{
	struct file *filePtr;

	if (flag == RTMP_FILE_RDONLY)
		flag = O_RDONLY;
	else if (flag == RTMP_FILE_WRONLY)
		flag = O_WRONLY;
	else if (flag == RTMP_FILE_CREAT)
		flag = O_CREAT;
	else if (flag == RTMP_FILE_TRUNC)
		flag = O_TRUNC;

	filePtr = filp_open(pPath, flag, 0);
	if (IS_ERR(filePtr)) {
		DBGPRINT(RT_DEBUG_ERROR,
			 ("%s(): Error %ld opening %s\n", __FUNCTION__,
			  -PTR_ERR(filePtr), pPath));
	}

	return (RTMP_OS_FD) filePtr;
}

int RtmpOSFileClose(RTMP_OS_FD osfd)
{
	filp_close(osfd, NULL);
	return 0;
}

void RtmpOSFileSeek(RTMP_OS_FD osfd, int offset)
{
	osfd->f_pos = offset;
}


int RtmpOSFileRead(RTMP_OS_FD osfd, char *pDataPtr, int readLen)
{
	/* The object must have a read method */
	if (osfd->f_op && osfd->f_op->read) {
		return osfd->f_op->read(osfd, pDataPtr, readLen, &osfd->f_pos);
	} else {
		DBGPRINT(RT_DEBUG_ERROR, ("no file read method\n"));
		return -1;
	}
}

int RtmpOSFileWrite(RTMP_OS_FD osfd, char *pDataPtr, int writeLen)
{
	return osfd->f_op->write(osfd, pDataPtr, (size_t) writeLen, &osfd->f_pos);
}

static inline void __RtmpOSFSInfoChange(OS_FS_INFO * pOSFSInfo, bool bSet)
{
	if (bSet) {
		/* Save uid and gid used for filesystem access. */
		/* Set user and group to 0 (root) */
		pOSFSInfo->fsuid = current_fsuid();
		pOSFSInfo->fsgid = current_fsgid();
		pOSFSInfo->fs = get_fs();
		set_fs(KERNEL_DS);
	} else {
		set_fs(pOSFSInfo->fs);
	}
}


/*******************************************************************************

	Task create/management/kill related functions.

 *******************************************************************************/
static inline int __RtmpOSTaskKill(OS_TASK *pTask)
{
	int ret = NDIS_STATUS_FAILURE;

#ifdef KTHREAD_SUPPORT
	if (pTask->kthread_task) {
		kthread_stop(pTask->kthread_task);
		ret = NDIS_STATUS_SUCCESS;
	}
#else
	CHECK_PID_LEGALITY(pTask->taskPID) {
		DBGPRINT(RT_DEBUG_TRACE,
			 ("Terminate the task(%s) with pid(%d)!\n",
			  pTask->taskName, GET_PID_NUMBER(pTask->taskPID)));
		mb();
		pTask->task_killed = 1;
		mb();
		ret = KILL_THREAD_PID(pTask->taskPID, SIGTERM, 1);
		if (ret) {
			printk(KERN_WARNING
			       "kill task(%s) with pid(%d) failed(retVal=%d)!\n",
			       pTask->taskName, GET_PID_NUMBER(pTask->taskPID),
			       ret);
		} else {
			wait_for_completion(&pTask->taskComplete);
			pTask->taskPID = THREAD_PID_INIT_VALUE;
			pTask->task_killed = 0;
			RTMP_SEM_EVENT_DESTORY(&pTask->taskSema);
			ret = NDIS_STATUS_SUCCESS;
		}
	}
#endif

	return ret;

}

static inline INT __RtmpOSTaskNotifyToExit(OS_TASK *pTask)
{
#ifndef KTHREAD_SUPPORT
	pTask->taskPID = THREAD_PID_INIT_VALUE;
	complete_and_exit(&pTask->taskComplete, 0);
#endif

	return 0;
}

static inline void __RtmpOSTaskCustomize(OS_TASK *pTask)
{
#ifndef KTHREAD_SUPPORT

	daemonize((char *) & pTask->taskName[0] /*"%s",pAd->net_dev->name */ );

	allow_signal(SIGTERM);
	allow_signal(SIGKILL);
	current->flags |= PF_NOFREEZE;

	RTMP_GET_OS_PID(pTask->taskPID, current->pid);

	/* signal that we've started the thread */
	complete(&pTask->taskComplete);

#endif
}

static inline int __RtmpOSTaskAttach(
	IN OS_TASK *pTask,
	IN RTMP_OS_TASK_CALLBACK fn,
	IN ULONG arg)
{
	int status = NDIS_STATUS_SUCCESS;
#ifndef KTHREAD_SUPPORT
	pid_t pid_number = -1;
#endif /* KTHREAD_SUPPORT */

#ifdef KTHREAD_SUPPORT
	pTask->task_killed = 0;
	pTask->kthread_task = NULL;
	pTask->kthread_task =
	    kthread_run((cast_fn) fn, (void *)arg, pTask->taskName);
	if (IS_ERR(pTask->kthread_task))
		status = NDIS_STATUS_FAILURE;
#else
	pid_number =
	    kernel_thread((cast_fn) fn, (void *)arg, RTMP_OS_MGMT_TASK_FLAGS);
	if (pid_number < 0) {
		DBGPRINT(RT_DEBUG_ERROR,
			 ("Attach task(%s) failed!\n", pTask->taskName));
		status = NDIS_STATUS_FAILURE;
	} else {
		/* Wait for the thread to start */
		wait_for_completion(&pTask->taskComplete);
		status = NDIS_STATUS_SUCCESS;
	}
#endif
	return status;
}

static inline int __RtmpOSTaskInit(
	IN OS_TASK *pTask,
	IN char *pTaskName,
	IN void *pPriv,
	IN LIST_HEADER *pSemList)
{
	int len;

	ASSERT(pTask);

#ifndef KTHREAD_SUPPORT
	memset((u8 *) (pTask), 0, sizeof (OS_TASK));
#endif

	len = strlen(pTaskName);
	len = len > (RTMP_OS_TASK_NAME_LEN - 1) ? (RTMP_OS_TASK_NAME_LEN - 1) : len;
	memmove(&pTask->taskName[0], pTaskName, len);
	pTask->priv = pPriv;

#ifndef KTHREAD_SUPPORT
	RTMP_SEM_EVENT_INIT_LOCKED(&(pTask->taskSema), pSemList);
	pTask->taskPID = THREAD_PID_INIT_VALUE;
	init_completion(&pTask->taskComplete);
#endif

#ifdef KTHREAD_SUPPORT
	init_waitqueue_head(&(pTask->kthread_q));
#endif /* KTHREAD_SUPPORT */

	return NDIS_STATUS_SUCCESS;
}

bool __RtmpOSTaskWait(
	IN void *pReserved,
	IN OS_TASK *pTask,
	IN INT32 *pStatus)
{
#ifdef KTHREAD_SUPPORT
	RTMP_WAIT_EVENT_INTERRUPTIBLE((*pStatus), pTask);

	if ((pTask->task_killed == 1) || ((*pStatus) != 0))
		return false;
#else

	RTMP_SEM_EVENT_WAIT(&(pTask->taskSema), (*pStatus));

	/* unlock the device pointers */
	if ((*pStatus) != 0) {
/*		RTMP_SET_FLAG_(*pFlags, fRTMP_ADAPTER_HALT_IN_PROGRESS); */
		return false;
	}
#endif /* KTHREAD_SUPPORT */

	return true;
}


static u32 RtmpOSWirelessEventTranslate(IN u32 eventType)
{
	switch (eventType) {
	case RT_WLAN_EVENT_CUSTOM:
		eventType = IWEVCUSTOM;
		break;

	case RT_WLAN_EVENT_CGIWAP:
		eventType = SIOCGIWAP;
		break;

	case RT_WLAN_EVENT_ASSOC_REQ_IE:
		eventType = IWEVASSOCREQIE;
		break;

	case RT_WLAN_EVENT_SCAN:
		eventType = SIOCGIWSCAN;
		break;

	case RT_WLAN_EVENT_EXPIRED:
		eventType = IWEVEXPIRED;
		break;

	default:
		printk("Unknown event: 0x%x\n", eventType);
		break;
	}

	return eventType;
}

int RtmpOSWrielessEventSend(
	IN struct net_device *pNetDev,
	IN u32 eventType,
	IN INT flags,
	IN u8 *pSrcMac,
	IN u8 *pData,
	IN u32 dataLen)
{
	union iwreq_data wrqu;

	/* translate event type */
	eventType = RtmpOSWirelessEventTranslate(eventType);

	memset(&wrqu, 0, sizeof (wrqu));

	if (flags > -1)
		wrqu.data.flags = flags;

	if (pSrcMac)
		memcpy(wrqu.ap_addr.sa_data, pSrcMac, ETH_ALEN);

	if ((pData != NULL) && (dataLen > 0))
		wrqu.data.length = dataLen;
	else
		wrqu.data.length = 0;

	wireless_send_event(pNetDev, eventType, &wrqu, (char *)pData);
	return 0;
}

int RtmpOSWrielessEventSendExt(
	IN struct net_device *pNetDev,
	IN u32 eventType,
	IN INT flags,
	IN u8 *pSrcMac,
	IN u8 *pData,
	IN u32 dataLen,
	IN u32 family)
{
	union iwreq_data wrqu;

	/* translate event type */
	eventType = RtmpOSWirelessEventTranslate(eventType);

	/* translate event type */
	memset(&wrqu, 0, sizeof (wrqu));

	if (flags > -1)
		wrqu.data.flags = flags;

	if (pSrcMac)
		memcpy(wrqu.ap_addr.sa_data, pSrcMac, ETH_ALEN);

	if ((pData != NULL) && (dataLen > 0))
		wrqu.data.length = dataLen;

	wrqu.addr.sa_family = family;

	wireless_send_event(pNetDev, eventType, &wrqu, (char *)pData);
	return 0;
}

int RtmpOSNetDevAddrSet(
	IN u8 OpMode,
	IN struct net_device *pNetDev,
	IN u8 *pMacAddr,
	IN u8 *dev_name)
{
	struct net_device *net_dev;

	net_dev = pNetDev;
/*	GET_PAD_FROM_NET_DEV(pAd, net_dev); */

#ifdef CONFIG_STA_SUPPORT
	/* work-around for the SuSE due to it has it's own interface name management system. */
	RT_CONFIG_IF_OPMODE_ON_STA(OpMode) {
/*		memset(pAd->StaCfg.dev_name, 0, 16); */
/*		memmove(pAd->StaCfg.dev_name, net_dev->name, strlen(net_dev->name)); */
		if (dev_name != NULL) {
			memset(dev_name, 0, 16);
			memmove(dev_name, net_dev->name, strlen(net_dev->name));
		}
	}
#endif /* CONFIG_STA_SUPPORT */

	memmove(net_dev->dev_addr, pMacAddr, 6);

	return 0;
}

/*
  *	Assign the network dev name for created Ralink WiFi interface.
  */
static int RtmpOSNetDevRequestName(
	IN INT32 MC_RowID,
	IN u32 *pIoctlIF,
	IN struct net_device *dev,
	IN char *pPrefixStr,
	IN INT devIdx)
{
	struct net_device *existNetDev;
	STRING suffixName[IFNAMSIZ];
	STRING desiredName[IFNAMSIZ];
	int ifNameIdx,
	 prefixLen,
	 slotNameLen;
	int Status;

	prefixLen = strlen(pPrefixStr);
	ASSERT((prefixLen < IFNAMSIZ));

	for (ifNameIdx = devIdx; ifNameIdx < 32; ifNameIdx++) {
		memset(suffixName, 0, IFNAMSIZ);
		memset(desiredName, 0, IFNAMSIZ);
		strncpy(&desiredName[0], pPrefixStr, prefixLen);

			sprintf(suffixName, "%d", ifNameIdx);

		slotNameLen = strlen(suffixName);
		ASSERT(((slotNameLen + prefixLen) < IFNAMSIZ));
		strcat(desiredName, suffixName);

		existNetDev = RtmpOSNetDevGetByName(dev, &desiredName[0]);
		if (existNetDev == NULL)
			break;
		else
			RtmpOSNetDeviceRefPut(existNetDev);
	}

	if (ifNameIdx < 32) {
#ifdef HOSTAPD_SUPPORT
		*pIoctlIF = ifNameIdx;
#endif /*HOSTAPD_SUPPORT */
		strcpy(&dev->name[0], &desiredName[0]);
		Status = NDIS_STATUS_SUCCESS;
	} else {
		DBGPRINT(RT_DEBUG_ERROR,
			 ("Cannot request DevName with preifx(%s) and in range(0~32) as suffix from OS!\n",
			  pPrefixStr));
		Status = NDIS_STATUS_FAILURE;
	}

	return Status;
}

void RtmpOSNetDevClose(struct net_device *pNetDev)
{
	dev_close(pNetDev);
}

void RtmpOSNetDevFree(struct net_device *pNetDev)
{
	struct dev_priv_info *pDevInfo = NULL;


	ASSERT(pNetDev);

	/* free assocaited private information */
	pDevInfo = (struct dev_priv_info *) netdev_priv(pNetDev);
	if (pDevInfo != NULL)
		kfree(pDevInfo);

	free_netdev(pNetDev);
}

INT RtmpOSNetDevAlloc(
	IN struct net_device **new_dev_p,
	IN u32 privDataSize)
{
	*new_dev_p = NULL;

	DBGPRINT(RT_DEBUG_TRACE,
		 ("Allocate a net device with private data size=%d!\n",
		  privDataSize));
	*new_dev_p = alloc_etherdev(privDataSize);

	if (*new_dev_p)
		return NDIS_STATUS_SUCCESS;
	else
		return NDIS_STATUS_FAILURE;
}


INT RtmpOSNetDevOpsAlloc(void **pNetDevOps)
{
	*pNetDevOps = (void *) vmalloc(sizeof (struct net_device_ops));
	if (*pNetDevOps) {
		memset(*pNetDevOps, 0, sizeof (struct net_device_ops));
		return NDIS_STATUS_SUCCESS;
	} else {
		return NDIS_STATUS_FAILURE;
	}
}


struct net_device *RtmpOSNetDevGetByName(struct net_device *pNetDev, char *pDevName)
{
	struct net_device *pTargetNetDev = NULL;

	pTargetNetDev = dev_get_by_name(dev_net(pNetDev), pDevName);

	return pTargetNetDev;
}


void RtmpOSNetDeviceRefPut(struct net_device *pNetDev)
{
	/*
	   every time dev_get_by_name is called, and it has returned a valid struct
	   net_device*, dev_put should be called afterwards, because otherwise the
	   machine hangs when the device is unregistered (since dev->refcnt > 1).
	 */
	if (pNetDev)
		dev_put(pNetDev);
}


INT RtmpOSNetDevDestory(void *pReserved, struct net_device *pNetDev)
{

	/* TODO: Need to fix this */
	printk("WARNING: This function(%s) not implement yet!!!\n",
	       __FUNCTION__);
	return 0;
}


void RtmpOSNetDevDetach(struct net_device *pNetDev)
{
	struct net_device_ops *pNetDevOps = (struct net_device_ops *)pNetDev->netdev_ops;

	unregister_netdev(pNetDev);

	vfree(pNetDevOps);
}

void RtmpOSNetDevProtect(bool lock_it)
{

/*
	if (lock_it)
		rtnl_lock();
	else
		rtnl_unlock();
*/
}

static void RALINK_ET_DrvInfoGet(
	struct net_device *pDev,
	struct ethtool_drvinfo *pInfo)
{
	strcpy(pInfo->driver, "RALINK WLAN");


	sprintf(pInfo->bus_info, "CSR 0x%lx", pDev->base_addr);
}

static struct ethtool_ops RALINK_Ethtool_Ops = {
	.get_drvinfo = RALINK_ET_DrvInfoGet,
};


int RtmpOSNetDevAttach(
	IN u8 OpMode,
	IN struct net_device *pNetDev,
	IN RTMP_OS_NETDEV_OP_HOOK *pDevOpHook)
{
	int ret,
	 rtnl_locked = false;

	struct net_device_ops *pNetDevOps = (struct net_device_ops *)pNetDev->netdev_ops;

	DBGPRINT(RT_DEBUG_TRACE, ("RtmpOSNetDevAttach()--->\n"));

	/* If we need hook some callback function to the net device structrue, now do it. */
	if (pDevOpHook) {
/*		struct rtmp_adapter *pAd = NULL; */

/*		GET_PAD_FROM_NET_DEV(pAd, pNetDev); */

		pNetDevOps->ndo_open = pDevOpHook->open;
		pNetDevOps->ndo_stop = pDevOpHook->stop;
		pNetDevOps->ndo_start_xmit =
		    (HARD_START_XMIT_FUNC) (pDevOpHook->xmit);
		pNetDevOps->ndo_do_ioctl = pDevOpHook->ioctl;

		pNetDev->ethtool_ops = &RALINK_Ethtool_Ops;

		/* if you don't implement get_stats, just leave the callback function as NULL, a dummy
		   function will make kernel panic.
		 */
		if (pDevOpHook->get_stats)
			pNetDevOps->ndo_get_stats = pDevOpHook->get_stats;

		/* OS specific flags, here we used to indicate if we are virtual interface */
/*		pNetDev->priv_flags = pDevOpHook->priv_flags; */
		RT_DEV_PRIV_FLAGS_SET(pNetDev, pDevOpHook->priv_flags);

#ifdef CONFIG_STA_SUPPORT
		if (OpMode == OPMODE_STA) {
/*			pNetDev->wireless_handlers = &rt28xx_iw_handler_def; */
			pNetDev->wireless_handlers = &rt28xx_iw_handler_def;
		}
#endif /* CONFIG_STA_SUPPORT */

		/* copy the net device mac address to the net_device structure. */
		memmove(pNetDev->dev_addr, &pDevOpHook->devAddr[0],
			       ETH_ALEN);

		rtnl_locked = pDevOpHook->needProtcted;

	}
	pNetDevOps->ndo_validate_addr = NULL;
	/*pNetDev->netdev_ops = ops; */

	if (rtnl_locked)
		ret = register_netdevice(pNetDev);
	else
		ret = register_netdev(pNetDev);

	netif_stop_queue(pNetDev);

	DBGPRINT(RT_DEBUG_TRACE, ("<---RtmpOSNetDevAttach(), ret=%d\n", ret));
	if (ret == 0)
		return NDIS_STATUS_SUCCESS;
	else
		return NDIS_STATUS_FAILURE;
}

struct net_device *RtmpOSNetDevCreate(
	IN INT32 MC_RowID,
	IN u32 *pIoctlIF,
	IN INT devType,
	IN INT devNum,
	IN INT privMemSize,
	IN char *pNamePrefix)
{
	struct net_device *pNetDev = NULL;
	struct net_device_ops *pNetDevOps = NULL;
	int status;

	/* allocate a new network device */
	status = RtmpOSNetDevAlloc(&pNetDev, sizeof(struct dev_priv_info *) /*privMemSize */ );
	if (status != NDIS_STATUS_SUCCESS) {
		/* allocation fail, exit */
		DBGPRINT(RT_DEBUG_ERROR, ("Allocate network device fail (%s)...\n", pNamePrefix));
		return NULL;
	}
	status = RtmpOSNetDevOpsAlloc((void *) & pNetDevOps);
	if (status != NDIS_STATUS_SUCCESS) {
		/* error! no any available ra name can be used! */
		DBGPRINT(RT_DEBUG_TRACE, ("Allocate net device ops fail!\n"));
		RtmpOSNetDevFree(pNetDev);

		return NULL;
	} else {
		DBGPRINT(RT_DEBUG_TRACE, ("Allocate net device ops success!\n"));
		pNetDev->netdev_ops = pNetDevOps;
	}
	/* find a available interface name, max 32 interfaces */
	status = RtmpOSNetDevRequestName(MC_RowID, pIoctlIF, pNetDev, pNamePrefix, devNum);
	if (status != NDIS_STATUS_SUCCESS) {
		/* error! no any available ra name can be used! */
		DBGPRINT(RT_DEBUG_ERROR, ("Assign interface name (%s with suffix 0~32) failed...\n",
			  pNamePrefix));
		RtmpOSNetDevFree(pNetDev);

		return NULL;
	} else {
		DBGPRINT(RT_DEBUG_TRACE, ("The name of the new %s interface is %s...\n",
			  pNamePrefix, pNetDev->name));
	}

	return pNetDev;
}





/*
========================================================================
Routine Description:
    Allocate memory for adapter control block.

Arguments:
    pAd					Pointer to our adapter

Return Value:
	NDIS_STATUS_SUCCESS
	NDIS_STATUS_FAILURE
	NDIS_STATUS_RESOURCES

Note:
========================================================================
*/
int AdapterBlockAllocateMemory(struct rtmp_adapter **ppAd, u32 SizeOfpAd)
{
	*ppAd = (void *) vmalloc(SizeOfpAd);	/*pci_alloc_consistent(pci_dev, sizeof(struct rtmp_adapter), phy_addr); */
	if (*ppAd) {
		memset(*ppAd, 0, SizeOfpAd);
		return NDIS_STATUS_SUCCESS;
	} else
		return NDIS_STATUS_FAILURE;
}


/* ========================================================================== */

UINT RtmpOsWirelessExtVerGet(void)
{
	return WIRELESS_EXT;
}


void RtmpDrvAllMacPrint(
	IN void *pReserved,
	IN u32 *pBufMac,
	IN u32 AddrStart,
	IN u32 AddrEnd,
	IN u32 AddrStep)
{
	struct file *file_w;
	char *fileName = "MacDump.txt";
	mm_segment_t orig_fs;
	STRING *msg;
	u32 macAddr = 0, macValue = 0;

	msg = kmalloc(1024, GFP_ATOMIC);
	if (!msg)
		return;

	orig_fs = get_fs();
	set_fs(KERNEL_DS);

	/* open file */
	file_w = filp_open(fileName, O_WRONLY | O_CREAT, 0);
	if (IS_ERR(file_w)) {
		DBGPRINT(RT_DEBUG_TRACE,
			 ("-->2) %s: Error %ld opening %s\n", __FUNCTION__,
			  -PTR_ERR(file_w), fileName));
	} else {
		if (file_w->f_op && file_w->f_op->write) {
			file_w->f_pos = 0;
			macAddr = AddrStart;

			while (macAddr <= AddrEnd) {
/*				mt76u_reg_read(pAd, macAddr, &macValue); // sample */
				macValue = *pBufMac++;
				sprintf(msg, "0x%04X = 0x%08X\n", macAddr, macValue);

				/* write data to file */
				file_w->f_op->write(file_w, msg, strlen(msg), &file_w->f_pos);

				printk("%s", msg);
				macAddr += AddrStep;
			}
			sprintf(msg, "\nDump all MAC values to %s\n", fileName);
		}
		filp_close(file_w, NULL);
	}
	set_fs(orig_fs);
	kfree(msg);
}


void RtmpDrvAllE2PPrint(
	IN void *pReserved,
	IN USHORT *pMacContent,
	IN u32 AddrEnd,
	IN u32 AddrStep)
{
	struct file *file_w;
	char *fileName = "EEPROMDump.txt";
	mm_segment_t orig_fs;
	STRING *msg;
	USHORT eepAddr = 0;
	USHORT eepValue;

	msg = kmalloc(1024, GFP_ATOMIC);
	if (!msg)
		return;

	orig_fs = get_fs();
	set_fs(KERNEL_DS);

	/* open file */
	file_w = filp_open(fileName, O_WRONLY | O_CREAT, 0);
	if (IS_ERR(file_w)) {
		DBGPRINT(RT_DEBUG_TRACE,
			 ("-->2) %s: Error %ld opening %s\n", __FUNCTION__,
			  -PTR_ERR(file_w), fileName));
	} else {
		if (file_w->f_op && file_w->f_op->write) {
			file_w->f_pos = 0;
			eepAddr = 0x00;

			while (eepAddr <= AddrEnd) {
				eepValue = *pMacContent;
				sprintf(msg, "%08x = %04x\n", eepAddr, eepValue);

				/* write data to file */
				file_w->f_op->write(file_w, msg, strlen(msg), &file_w->f_pos);

				printk("%s", msg);
				eepAddr += AddrStep;
				pMacContent += (AddrStep >> 1);
			}
			sprintf(msg, "\nDump all EEPROM values to %s\n",
				fileName);
		}
		filp_close(file_w, NULL);
	}
	set_fs(orig_fs);
	kfree(msg);
}

/*
========================================================================
Routine Description:
	Check if the network interface is up.

Arguments:
	*pDev			- Network Interface

Return Value:
	None

Note:
========================================================================
*/
bool RtmpOSNetDevIsUp(void *pDev)
{
	struct net_device *pNetDev = (struct net_device *)pDev;

	if ((pNetDev == NULL) || !(pNetDev->flags & IFF_UP))
		return false;

	return true;
}


/*
========================================================================
Routine Description:
	Wake up the command thread.

Arguments:
	pAd				- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
void RtmpOsCmdUp(RTMP_OS_TASK *pCmdQTask)
{
	OS_TASK *pTask = RTMP_OS_TASK_GET(pCmdQTask);
#ifdef KTHREAD_SUPPORT
	pTask->kthread_running = true;
	wake_up(&pTask->kthread_q);
#else
	CHECK_PID_LEGALITY(pTask->taskPID) {
		RTMP_SEM_EVENT_UP(&(pTask->taskSema));
	}
#endif /* KTHREAD_SUPPORT */
}


/*
========================================================================
Routine Description:
	Wake up USB Mlme thread.

Arguments:
	pAd				- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
void RtmpOsMlmeUp(IN RTMP_OS_TASK *pMlmeQTask)
{
	OS_TASK *pTask = RTMP_OS_TASK_GET(pMlmeQTask);

#ifdef KTHREAD_SUPPORT
	if ((pTask != NULL) && (pTask->kthread_task)) {
		pTask->kthread_running = true;
		wake_up(&pTask->kthread_q);
	}
#else
	if (pTask != NULL) {
		CHECK_PID_LEGALITY(pTask->taskPID) {
			RTMP_SEM_EVENT_UP(&(pTask->taskSema));
		}
	}
#endif /* KTHREAD_SUPPORT */
}


/*
========================================================================
Routine Description:
	Check if the file is error.

Arguments:
	pFile			- the file

Return Value:
	OK or any error

Note:
	rt_linux.h, not rt_drv.h
========================================================================
*/
INT32 RtmpOsFileIsErr(IN void *pFile)
{
	return IS_FILE_OPEN_ERR(pFile);
}

int RtmpOSIRQRelease(
	IN struct net_device *pNetDev,
	IN u32 infType,
	IN PPCI_DEV pci_dev,
	IN bool *pHaveMsi)
{
	struct net_device *net_dev = (struct net_device *)pNetDev;



	return 0;
}


/*
========================================================================
Routine Description:
	Enable or disable wireless event sent.

Arguments:
	pReserved		- Reserved
	FlgIsWEntSup	- true or false

Return Value:
	None

Note:
========================================================================
*/
void RtmpOsWlanEventSet(
	IN void *pReserved,
	IN bool *pCfgWEnt,
	IN bool FlgIsWEntSup)
{
/*	pAd->CommonCfg.bWirelessEvent = FlgIsWEntSup; */
	*pCfgWEnt = FlgIsWEntSup;
}

/*
========================================================================
Routine Description:
	vmalloc

Arguments:
	Size			- memory size

Return Value:
	the memory

Note:
========================================================================
*/
void *RtmpOsVmalloc(ULONG Size)
{
	return vmalloc(Size);
}

/*
========================================================================
Routine Description:
	vfree

Arguments:
	pMem			- the memory

Return Value:
	None

Note:
========================================================================
*/
void RtmpOsVfree(void *pMem)
{
	if (pMem != NULL)
		vfree(pMem);
}


bool RtmpOsStatsAlloc(
	IN void **ppStats,
	IN void **ppIwStats)
{
	*ppStats = kmalloc(sizeof (struct net_device_stats), GFP_ATOMIC);
	if ((*ppStats) == NULL)
		return false;
	memset((u8 *) *ppStats, 0, sizeof (struct net_device_stats));

	*ppIwStats = kmalloc(sizeof (struct iw_statistics), GFP_ATOMIC);
	if ((*ppIwStats) == NULL) {
		kfree(*ppStats);
		return false;
	}
	memset((u8 *)* ppIwStats, 0, sizeof (struct iw_statistics));

	return true;
}

void RtmpOsTaskPidInit(RTMP_OS_PID *pPid)
{
	*pPid = THREAD_PID_INIT_VALUE;
}

#ifdef IAPP_SUPPORT
/* Layer 2 Update frame to switch/bridge */
/* For any Layer2 devices, e.g., bridges, switches and other APs, the frame
   can update their forwarding tables with the correct port to reach the new
   location of the STA */
typedef struct __attribute__ ((packed)) _RT_IAPP_L2_UPDATE_FRAME {

	u8 DA[ETH_ALEN];	/* broadcast MAC address */
	u8 SA[ETH_ALEN];	/* the MAC address of the STA that has just associated
				   or reassociated */
	USHORT Len;		/* 8 octets */
	u8 DSAP;		/* null */
	u8 SSAP;		/* null */
	u8 Control;		/* reference to IEEE Std 802.2 */
	u8 XIDInfo[3];	/* reference to IEEE Std 802.2 */
} RT_IAPP_L2_UPDATE_FRAME, *PRT_IAPP_L2_UPDATE_FRAME;


struct sk_buff * RtmpOsPktIappMakeUp(
	IN struct net_device *pNetDev,
	IN u8 *pMac)
{
	RT_IAPP_L2_UPDATE_FRAME frame_body;
	INT size = sizeof (RT_IAPP_L2_UPDATE_FRAME);
	struct sk_buff * pNetBuf;

	if (pNetDev == NULL)
		return NULL;

	pNetBuf = dev_alloc_skb(size + 2);
	if (!pNetBuf) {
		DBGPRINT(RT_DEBUG_ERROR, ("Error! Can't allocate a skb.\n"));
		return NULL;
	}

	/* init the update frame body */
	memset(&frame_body, 0, size);

	memset(frame_body.DA, 0xFF, ETH_ALEN);
	memcpy(frame_body.SA, pMac, ETH_ALEN);

	frame_body.Len = OS_HTONS(ETH_ALEN);
	frame_body.DSAP = 0;
	frame_body.SSAP = 0x01;
	frame_body.Control = 0xAF;

	frame_body.XIDInfo[0] = 0x81;
	frame_body.XIDInfo[1] = 1;
	frame_body.XIDInfo[2] = 1 << 1;

	pNetBuf->len = pNetDev;
	skb_reserve(pNetBuf, 2);
	memcpy(skb_put(pNetBuf, size), &frame_body, size);
	return pNetBuf;
}
#endif /* IAPP_SUPPORT */


#ifdef RT_CFG80211_SUPPORT
/* all available channels */
u8 Cfg80211_Chan[] = {
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,

	/* 802.11 UNI / HyperLan 2 */
	36, 38, 40, 44, 46, 48, 52, 54, 56, 60, 62, 64,

	/* 802.11 HyperLan 2 */
	100, 104, 108, 112, 116, 118, 120, 124, 126, 128, 132, 134, 136,

	/* 802.11 UNII */
	140, 149, 151, 153, 157, 159, 161, 165, 167, 169, 171, 173,

	/* Japan */
	184, 188, 192, 196, 208, 212, 216,
};

/*
	Array of bitrates the hardware can operate with
	in this band. Must be sorted to give a valid "supported
	rates" IE, i.e. CCK rates first, then OFDM.

	For HT, assign MCS in another structure, ieee80211_sta_ht_cap.
*/
const struct ieee80211_rate Cfg80211_SupRate[12] = {
	{
		.flags = IEEE80211_RATE_SHORT_PREAMBLE,
		.bitrate = 10,
		.hw_value = 0,
		.hw_value_short = 0,
	},
	{
		.flags = IEEE80211_RATE_SHORT_PREAMBLE,
		.bitrate = 20,
		.hw_value = 1,
		.hw_value_short = 1,
	},
	{
		.flags = IEEE80211_RATE_SHORT_PREAMBLE,
		.bitrate = 55,
		.hw_value = 2,
		.hw_value_short = 2,
	},
	{
		.flags = IEEE80211_RATE_SHORT_PREAMBLE,
		.bitrate = 110,
		.hw_value = 3,
		.hw_value_short = 3,
	},
	{
		.flags = 0,
		.bitrate = 60,
		.hw_value = 4,
		.hw_value_short = 4,
	},
	{
		.flags = 0,
		.bitrate = 90,
		.hw_value = 5,
		.hw_value_short = 5,
	},
	{
		.flags = 0,
		.bitrate = 120,
		.hw_value = 6,
		.hw_value_short = 6,
	},
	{
		.flags = 0,
		.bitrate = 180,
		.hw_value = 7,
		.hw_value_short = 7,
	},
	{
		.flags = 0,
		.bitrate = 240,
		.hw_value = 8,
		.hw_value_short = 8,
	},
	{
		.flags = 0,
		.bitrate = 360,
		.hw_value = 9,
		.hw_value_short = 9,
	},
	{
		.flags = 0,
		.bitrate = 480,
		.hw_value = 10,
		.hw_value_short = 10,
	},
	{
		.flags = 0,
		.bitrate = 540,
		.hw_value = 11,
		.hw_value_short = 11,
	},
};

static const u32 CipherSuites[] = {
	WLAN_CIPHER_SUITE_WEP40,
	WLAN_CIPHER_SUITE_WEP104,
	WLAN_CIPHER_SUITE_TKIP,
	WLAN_CIPHER_SUITE_CCMP,
};

/*
========================================================================
Routine Description:
	UnRegister MAC80211 Module.

Arguments:
	pCB				- CFG80211 control block pointer
	pNetDev			- Network device

Return Value:
	NONE

Note:
========================================================================
*/
void CFG80211OS_UnRegister(
	IN void *pCB,
	IN void *pNetDevOrg)
{
	CFG80211_CB *pCfg80211_CB = (CFG80211_CB *)pCB;
	struct net_device *pNetDev = (struct net_device *)pNetDevOrg;


	/* unregister */
	if (pCfg80211_CB->pCfg80211_Wdev != NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("80211> unregister/free wireless device\n"));

		/*
			Must unregister, or you will suffer problem when you change
			regulatory domain by using iw.
		*/

#ifdef RFKILL_HW_SUPPORT
		wiphy_rfkill_stop_polling(pCfg80211_CB->pCfg80211_Wdev->wiphy);
#endif /* RFKILL_HW_SUPPORT */
		wiphy_unregister(pCfg80211_CB->pCfg80211_Wdev->wiphy);
		wiphy_free(pCfg80211_CB->pCfg80211_Wdev->wiphy);
		kfree(pCfg80211_CB->pCfg80211_Wdev);

		if (pCfg80211_CB->pCfg80211_Channels != NULL)
			kfree(pCfg80211_CB->pCfg80211_Channels);

		if (pCfg80211_CB->pCfg80211_Rates != NULL)
			kfree(pCfg80211_CB->pCfg80211_Rates);

		pCfg80211_CB->pCfg80211_Wdev = NULL;
		pCfg80211_CB->pCfg80211_Channels = NULL;
		pCfg80211_CB->pCfg80211_Rates = NULL;

		/* must reset to NULL; or kernel will panic in unregister_netdev */
		pNetDev->ieee80211_ptr = NULL;
		SET_NETDEV_DEV(pNetDev, NULL);
	}

	kfree(pCfg80211_CB);
}


/*
========================================================================
Routine Description:
	Initialize wireless channel in 2.4GHZ and 5GHZ.

Arguments:
	pAd				- WLAN control block pointer
	pWiphy			- WLAN PHY interface
	pChannels		- Current channel info
	pRates			- Current rate info

Return Value:
	true			- init successfully
	false			- init fail

Note:
	TX Power related:

	1. Suppose we can send power to 15dBm in the board.
	2. A value 0x0 ~ 0x1F for a channel. We will adjust it based on 15dBm/
		54Mbps. So if value == 0x07, the TX power of 54Mbps is 15dBm and
		the value is 0x07 in the EEPROM.
	3. Based on TX power value of 54Mbps/channel, adjust another value
		0x0 ~ 0xF for other data rate. (-6dBm ~ +6dBm)

	Other related factors:
	1. TX power percentage from UI/users;
	2. Maximum TX power limitation in the regulatory domain.
========================================================================
*/
bool CFG80211_SupBandInit(
	IN void *pCB,
	IN CFG80211_BAND *pBandInfo,
	IN void *pWiphyOrg,
	IN void *pChannelsOrg,
	IN void *pRatesOrg)
{
	CFG80211_CB *pCfg80211_CB = (CFG80211_CB *)pCB;
	struct wiphy *pWiphy = (struct wiphy *)pWiphyOrg;
	struct ieee80211_channel *pChannels = (struct ieee80211_channel *)pChannelsOrg;
	struct ieee80211_rate *pRates = (struct ieee80211_rate *)pRatesOrg;
	struct ieee80211_supported_band *pBand;
	u32 NumOfChan, NumOfRate;
	u32 IdLoop;
	u32 CurTxPower;


	/* sanity check */
	if (pBandInfo->RFICType == 0)
		pBandInfo->RFICType = RFIC_24GHZ | RFIC_5GHZ;

	CFG80211DBG(RT_DEBUG_ERROR, ("80211> RFICType = %d\n",
				pBandInfo->RFICType));

	/* init */
	if (pBandInfo->RFICType & RFIC_5GHZ)
		NumOfChan = CFG80211_NUM_OF_CHAN_2GHZ + CFG80211_NUM_OF_CHAN_5GHZ;
	else
		NumOfChan = CFG80211_NUM_OF_CHAN_2GHZ;

	if (pBandInfo->FlgIsBMode == true)
		NumOfRate = 4;
	else
		NumOfRate = 4 + 8;

	if (pChannels == NULL)
	{
		pChannels = kzalloc(sizeof(*pChannels) * NumOfChan, GFP_KERNEL);
		if (!pChannels)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("80211> ieee80211_channel allocation fail!\n"));
			return false;
		}
	}

	CFG80211DBG(RT_DEBUG_ERROR, ("80211> Number of channel = %d\n",
				(int) CFG80211_NUM_OF_CHAN_5GHZ));

	if (pRates == NULL)
	{
		pRates = kzalloc(sizeof(*pRates) * NumOfRate, GFP_KERNEL);
		if (!pRates)
		{
			kfree(pChannels);
			DBGPRINT(RT_DEBUG_ERROR, ("80211> ieee80211_rate allocation fail!\n"));
			return false;
		}
	}

	CFG80211DBG(RT_DEBUG_ERROR, ("80211> Number of rate = %d\n", NumOfRate));

	/* get TX power */
	CurTxPower = 0; /* unknown */

	CFG80211DBG(RT_DEBUG_ERROR, ("80211> CurTxPower = %d dBm\n", CurTxPower));

	/* init channel */
	for(IdLoop=0; IdLoop<NumOfChan; IdLoop++)
	{
		// @andy 2/24/2016
		// 2.6.39 is the first kernel with ieee80211_channel_to_frequency requiring two parameters
		// https://git.kernel.org/cgit/linux/kernel/git/stable/linux-stable.git/tree/include/net/cfg80211.h?h=linux-2.6.38.y
		// https://git.kernel.org/cgit/linux/kernel/git/stable/linux-stable.git/tree/include/net/cfg80211.h?h=linux-2.6.39.y
		// http://www.infty.nl/wordpress/2011/0/
		// https://github.com/coolshou/mt7610u/pull/1/files?diff=split
		pChannels[IdLoop].center_freq = \
					ieee80211_channel_to_frequency(Cfg80211_Chan[IdLoop],
						(IdLoop<CFG80211_NUM_OF_CHAN_2GHZ)?NL80211_BAND_2GHZ:NL80211_BAND_5GHZ);
		pChannels[IdLoop].hw_value = IdLoop;

		if (IdLoop < CFG80211_NUM_OF_CHAN_2GHZ)
			pChannels[IdLoop].max_power = CurTxPower;
		else
			pChannels[IdLoop].max_power = CurTxPower;

		pChannels[IdLoop].max_antenna_gain = 0xff;
	}

	/* init rate */
	for(IdLoop=0; IdLoop<NumOfRate; IdLoop++)
		memcpy(&pRates[IdLoop], &Cfg80211_SupRate[IdLoop], sizeof(*pRates));

	pBand = &pCfg80211_CB->Cfg80211_bands[NL80211_BAND_2GHZ];
	if (pBandInfo->RFICType & RFIC_24GHZ)
	{
		pBand->n_channels = CFG80211_NUM_OF_CHAN_2GHZ;
		pBand->n_bitrates = NumOfRate;
		pBand->channels = pChannels;
		pBand->bitrates = pRates;

		/* for HT, assign pBand->ht_cap */
		pBand->ht_cap.ht_supported = true;
		pBand->ht_cap.cap = IEEE80211_HT_CAP_SUP_WIDTH_20_40 |
					       IEEE80211_HT_CAP_SM_PS |
					       IEEE80211_HT_CAP_SGI_40 |
					       IEEE80211_HT_CAP_DSSSCCK40;
		pBand->ht_cap.ampdu_factor = 3; /* 2 ^ 16 */
		pBand->ht_cap.ampdu_density = pBandInfo->MpduDensity;

		memset(&pBand->ht_cap.mcs, 0, sizeof(pBand->ht_cap.mcs));
		CFG80211DBG(RT_DEBUG_ERROR,
					("80211> TxStream = %d\n", pBandInfo->TxStream));

		switch(pBandInfo->TxStream)
		{
			case 1:
			default:
				pBand->ht_cap.mcs.rx_mask[0] = 0xff;
				break;

			case 2:
				pBand->ht_cap.mcs.rx_mask[0] = 0xff;
				pBand->ht_cap.mcs.rx_mask[1] = 0xff;
				break;

			case 3:
				pBand->ht_cap.mcs.rx_mask[0] = 0xff;
				pBand->ht_cap.mcs.rx_mask[1] = 0xff;
				pBand->ht_cap.mcs.rx_mask[2] = 0xff;
				break;
		}

		pBand->ht_cap.mcs.tx_params = IEEE80211_HT_MCS_TX_DEFINED;

		pWiphy->bands[NL80211_BAND_2GHZ] = pBand;
	}
	else
	{
		pWiphy->bands[NL80211_BAND_2GHZ] = NULL;
		pBand->channels = NULL;
		pBand->bitrates = NULL;
	}

	pBand = &pCfg80211_CB->Cfg80211_bands[NL80211_BAND_5GHZ];
	if (pBandInfo->RFICType & RFIC_5GHZ)
	{
		pBand->n_channels = CFG80211_NUM_OF_CHAN_5GHZ;
		pBand->n_bitrates = NumOfRate - 4;
		pBand->channels = &pChannels[CFG80211_NUM_OF_CHAN_2GHZ];
		pBand->bitrates = &pRates[4];

		/* for HT, assign pBand->ht_cap */
		/* for HT, assign pBand->ht_cap */
		pBand->ht_cap.ht_supported = true;
		pBand->ht_cap.cap = IEEE80211_HT_CAP_SUP_WIDTH_20_40 |
					       IEEE80211_HT_CAP_SM_PS |
					       IEEE80211_HT_CAP_SGI_40 |
					       IEEE80211_HT_CAP_DSSSCCK40;
		pBand->ht_cap.ampdu_factor = 3; /* 2 ^ 16 */
		pBand->ht_cap.ampdu_density = pBandInfo->MpduDensity;

		memset(&pBand->ht_cap.mcs, 0, sizeof(pBand->ht_cap.mcs));
		switch(pBandInfo->RxStream)
		{
			case 1:
			default:
				pBand->ht_cap.mcs.rx_mask[0] = 0xff;
				break;

			case 2:
				pBand->ht_cap.mcs.rx_mask[0] = 0xff;
				pBand->ht_cap.mcs.rx_mask[1] = 0xff;
				break;

			case 3:
				pBand->ht_cap.mcs.rx_mask[0] = 0xff;
				pBand->ht_cap.mcs.rx_mask[1] = 0xff;
				pBand->ht_cap.mcs.rx_mask[2] = 0xff;
				break;
		}

		pBand->ht_cap.mcs.tx_params = IEEE80211_HT_MCS_TX_DEFINED;

		pWiphy->bands[NL80211_BAND_5GHZ] = pBand;
	}
	else
	{
		pWiphy->bands[NL80211_BAND_5GHZ] = NULL;
		pBand->channels = NULL;
		pBand->bitrates = NULL;
	}

	pCfg80211_CB->pCfg80211_Channels = pChannels;
	pCfg80211_CB->pCfg80211_Rates = pRates;

	return true;
}


/*
========================================================================
Routine Description:
	Re-Initialize wireless channel/PHY in 2.4GHZ and 5GHZ.

Arguments:
	pCB				- CFG80211 control block pointer
	pBandInfo		- Band information

Return Value:
	true			- re-init successfully
	false			- re-init fail

Note:
	CFG80211_SupBandInit() is called in xx_probe().
	But we do not have complete chip information in xx_probe() so we
	need to re-init bands in xx_open().
========================================================================
*/
bool CFG80211OS_SupBandReInit(
	IN void *pCB,
	IN CFG80211_BAND *pBandInfo)
{
	CFG80211_CB *pCfg80211_CB = (CFG80211_CB *)pCB;
	struct wiphy *pWiphy;


	if ((pCfg80211_CB == NULL) || (pCfg80211_CB->pCfg80211_Wdev == NULL))
		return false;

	pWiphy = pCfg80211_CB->pCfg80211_Wdev->wiphy;

	if (pWiphy != NULL)
	{
		CFG80211DBG(RT_DEBUG_ERROR, ("80211> re-init bands...\n"));

		/* re-init bands */
		CFG80211_SupBandInit(pCfg80211_CB, pBandInfo, pWiphy,
							pCfg80211_CB->pCfg80211_Channels,
							pCfg80211_CB->pCfg80211_Rates);

		/* re-init PHY */
		pWiphy->rts_threshold = pBandInfo->RtsThreshold;
		pWiphy->frag_threshold = pBandInfo->FragmentThreshold;
		pWiphy->retry_short = pBandInfo->RetryMaxCnt & 0xff;
		pWiphy->retry_long = (pBandInfo->RetryMaxCnt & 0xff00)>>8;

		return true;
	}

	return false;
}


/*
========================================================================
Routine Description:
	Hint to the wireless core a regulatory domain from driver.

Arguments:
	pAd				- WLAN control block pointer
	pCountryIe		- pointer to the country IE
	CountryIeLen	- length of the country IE

Return Value:
	NONE

Note:
	Must call the function in kernel thread.
========================================================================
*/
void CFG80211OS_RegHint(
	IN void *pCB,
	IN u8 *pCountryIe,
	IN ULONG CountryIeLen)
{
	CFG80211_CB *pCfg80211_CB = (CFG80211_CB *)pCB;


	CFG80211DBG(RT_DEBUG_ERROR,
			("crda> regulatory domain hint: %c%c\n",
			pCountryIe[0], pCountryIe[1]));

	if ((pCfg80211_CB->pCfg80211_Wdev == NULL) || (pCountryIe == NULL))
	{
		CFG80211DBG(RT_DEBUG_ERROR, ("crda> regulatory domain hint not support!\n"));
		return;
	}

	/* hints a country IE as a regulatory domain "without" channel/power info. */
/*	regulatory_hint(pCfg80211_CB->pMac80211_Hw->wiphy, pCountryIe); */
	regulatory_hint(pCfg80211_CB->pCfg80211_Wdev->wiphy, (const char *)pCountryIe);
}


/*
========================================================================
Routine Description:
	Hint to the wireless core a regulatory domain from country element.

Arguments:
	pAdCB			- WLAN control block pointer
	pCountryIe		- pointer to the country IE
	CountryIeLen	- length of the country IE

Return Value:
	NONE

Note:
	Must call the function in kernel thread.
========================================================================
*/
void CFG80211OS_RegHint11D(
	IN void *pCB,
	IN u8 *pCountryIe,
	IN ULONG CountryIeLen)
{
	/* no regulatory_hint_11d() in 2.6.32 */
}


bool CFG80211OS_BandInfoGet(
	IN void *pCB,
	IN void *pWiphyOrg,
	OUT void **ppBand24,
	OUT void **ppBand5)
{
	CFG80211_CB *pCfg80211_CB = (CFG80211_CB *)pCB;
	struct wiphy *pWiphy = (struct wiphy *)pWiphyOrg;


	if (pWiphy == NULL)
	{
		if ((pCfg80211_CB != NULL) && (pCfg80211_CB->pCfg80211_Wdev != NULL))
			pWiphy = pCfg80211_CB->pCfg80211_Wdev->wiphy;
	}

	if (pWiphy == NULL)
		return false;

	*ppBand24 = pWiphy->bands[NL80211_BAND_2GHZ];
	*ppBand5 = pWiphy->bands[NL80211_BAND_5GHZ];
	return true;
}


u32 CFG80211OS_ChanNumGet(
	IN void 					*pCB,
	IN void 					*pWiphyOrg,
	IN u32					IdBand)
{
	CFG80211_CB *pCfg80211_CB = (CFG80211_CB *)pCB;
	struct wiphy *pWiphy = (struct wiphy *)pWiphyOrg;


	if (pWiphy == NULL)
	{
		if ((pCfg80211_CB != NULL) && (pCfg80211_CB->pCfg80211_Wdev != NULL))
			pWiphy = pCfg80211_CB->pCfg80211_Wdev->wiphy;
	}

	if (pWiphy == NULL)
		return 0;

	if (pWiphy->bands[IdBand] != NULL)
		return pWiphy->bands[IdBand]->n_channels;

	return 0;
}


bool CFG80211OS_ChanInfoGet(
	IN void 					*pCB,
	IN void 					*pWiphyOrg,
	IN u32					IdBand,
	IN u32					IdChan,
	OUT u32					*pChanId,
	OUT u32					*pPower,
	OUT bool 				*pFlgIsRadar)
{
	CFG80211_CB *pCfg80211_CB = (CFG80211_CB *)pCB;
	struct wiphy *pWiphy = (struct wiphy *)pWiphyOrg;
	struct ieee80211_supported_band *pSband;
	struct ieee80211_channel *pChan;


	if (pWiphy == NULL)
	{
		if ((pCfg80211_CB != NULL) && (pCfg80211_CB->pCfg80211_Wdev != NULL))
			pWiphy = pCfg80211_CB->pCfg80211_Wdev->wiphy;
	}

	if (pWiphy == NULL)
		return false;

	pSband = pWiphy->bands[IdBand];
	pChan = &pSband->channels[IdChan];

	*pChanId = ieee80211_frequency_to_channel(pChan->center_freq);

	if (pChan->flags & IEEE80211_CHAN_DISABLED)
	{
		CFG80211DBG(RT_DEBUG_ERROR,
					("Chan %03d (frq %d):\tnot allowed!\n",
					(*pChanId), pChan->center_freq));
		return false;
	}

	*pPower = pChan->max_power;

	if (pChan->flags & IEEE80211_CHAN_RADAR)
		*pFlgIsRadar = true;
	else
		*pFlgIsRadar = false;

	return true;
}


/*
========================================================================
Routine Description:
	Initialize a channel information used in scan inform.

Arguments:

Return Value:
	true		- Successful
	false		- Fail

Note:
========================================================================
*/
bool CFG80211OS_ChanInfoInit(
	IN void 					*pCB,
	IN u32					InfoIndex,
	IN u8 				ChanId,
	IN u8 				MaxTxPwr,
	IN bool 				FlgIsNMode,
	IN bool 				FlgIsBW20M)
{
	CFG80211_CB *pCfg80211_CB = (CFG80211_CB *)pCB;
	struct ieee80211_channel *pChan;


	if (InfoIndex >= MAX_NUM_OF_CHANNELS)
		return false;

	pChan = (struct ieee80211_channel *)&(pCfg80211_CB->ChanInfo[InfoIndex]);
	memset(pChan, 0, sizeof(*pChan));

	if (ChanId > 14)
		pChan->band = NL80211_BAND_5GHZ;
	else
		pChan->band = NL80211_BAND_2GHZ;

	pChan->center_freq = ieee80211_channel_to_frequency(ChanId, pChan->band);


	/* no use currently in 2.6.30 */
/*	if (ieee80211_is_beacon(((struct ieee80211_mgmt *)pFrame)->frame_control)) */
/*		pChan->beacon_found = 1; */

	return true;
}


/*
========================================================================
Routine Description:
	Inform us that a scan is got.

Arguments:
	pAdCB				- WLAN control block pointer

Return Value:
	NONE

Note:
	Call RT_CFG80211_SCANNING_INFORM, not CFG80211_Scaning
========================================================================
*/
void CFG80211OS_Scaning(
	IN void 					*pCB,
	IN u32					ChanId,
	IN u8 				*pFrame,
	IN u32					FrameLen,
	IN INT32					RSSI,
	IN bool 				FlgIsNMode,
	IN u8					BW)
{
	struct cfg80211_bss *ret;
#ifdef CONFIG_STA_SUPPORT
	CFG80211_CB *pCfg80211_CB = (CFG80211_CB *)pCB;
	u32 IdChan;
	u32 CenFreq;

	/* get channel information */
	CenFreq = ieee80211_channel_to_frequency(ChanId,
		(ChanId<CFG80211_NUM_OF_CHAN_2GHZ)?NL80211_BAND_2GHZ:NL80211_BAND_5GHZ);

	for(IdChan=0; IdChan<MAX_NUM_OF_CHANNELS; IdChan++)
	{
		if (pCfg80211_CB->ChanInfo[IdChan].center_freq == CenFreq)
			break;
	}
	if (IdChan >= MAX_NUM_OF_CHANNELS)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("80211> Can not find any chan info!\n"));
		return;
	}

	/* inform 80211 a scan is got */
	/* we can use cfg80211_inform_bss in 2.6.31, it is easy more than the one */
	/* in cfg80211_inform_bss_frame(), it will memcpy pFrame but pChan */
	ret = cfg80211_inform_bss_frame(pCfg80211_CB->pCfg80211_Wdev->wiphy,
								&pCfg80211_CB->ChanInfo[IdChan],
								(struct ieee80211_mgmt *)pFrame,
								FrameLen,
								RSSI,
								GFP_ATOMIC);
	ret = ret;

	CFG80211DBG(RT_DEBUG_ERROR, ("80211> cfg80211_inform_bss_frame\n"));
#endif /* CONFIG_STA_SUPPORT */
}


/*
========================================================================
Routine Description:
	Inform us that scan ends.

Arguments:
	pAdCB			- WLAN control block pointer
	FlgIsAborted	- 1: scan is aborted

Return Value:
	NONE

Note:
========================================================================
*/
void CFG80211OS_ScanEnd(
	IN void *pCB,
	IN bool FlgIsAborted)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,8,0))
	struct cfg80211_scan_info info = {
		.aborted = FlgIsAborted,
	};
#endif

#ifdef CONFIG_STA_SUPPORT
	CFG80211_CB *pCfg80211_CB = (CFG80211_CB *)pCB;


	CFG80211DBG(RT_DEBUG_ERROR, ("80211> cfg80211_scan_done\n"));
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,8,0))
	cfg80211_scan_done(pCfg80211_CB->pCfg80211_ScanReq, &info);
#else
	cfg80211_scan_done(pCfg80211_CB->pCfg80211_ScanReq, FlgIsAborted);
#endif

#endif /* CONFIG_STA_SUPPORT */
}


/*
========================================================================
Routine Description:
	Inform CFG80211 about association status.

Arguments:
	pAdCB			- WLAN control block pointer
	pBSSID			- the BSSID of the AP
	pReqIe			- the element list in the association request frame
	ReqIeLen		- the request element length
	pRspIe			- the element list in the association response frame
	RspIeLen		- the response element length
	FlgIsSuccess	- 1: success; otherwise: fail

Return Value:
	None

Note:
========================================================================
*/
void CFG80211OS_ConnectResultInform(
	IN void *pCB,
	IN u8 *pBSSID,
	IN u8 *pReqIe,
	IN u32 ReqIeLen,
	IN u8 *pRspIe,
	IN u32 RspIeLen,
	IN u8 FlgIsSuccess)
{
	CFG80211_CB *pCfg80211_CB = (CFG80211_CB *)pCB;


	if ((pCfg80211_CB->pCfg80211_Wdev->netdev == NULL) || (pBSSID == NULL))
		return;

	if (FlgIsSuccess)
	{
		cfg80211_connect_result(pCfg80211_CB->pCfg80211_Wdev->netdev,
								pBSSID,
								pReqIe,
								ReqIeLen,
								pRspIe,
								RspIeLen,
								WLAN_STATUS_SUCCESS,
								GFP_KERNEL);
	}
	else
	{
		cfg80211_connect_result(pCfg80211_CB->pCfg80211_Wdev->netdev,
								pBSSID,
								NULL, 0, NULL, 0,
								WLAN_STATUS_UNSPECIFIED_FAILURE,
								GFP_KERNEL);
	}
}
#endif /* RT_CFG80211_SUPPORT */


/*
========================================================================
Routine Description:
	Flush a data cache line.

Arguments:
	AddrStart		- the start address
	Size			- memory size

Return Value:
	None

Note:
========================================================================
*/
void RtmpOsDCacheFlush(
	IN ULONG AddrStart,
	IN ULONG Size)
{
	RTMP_UTIL_DCACHE_FLUSH(AddrStart, Size);
}


/*
========================================================================
Routine Description:
	Assign private data pointer to the network interface.

Arguments:
	pDev			- the device
	pPriv			- the pointer

Return Value:
	None

Note:
========================================================================
*/
void RtmpOsSetNetDevPriv(struct net_device *pDev, struct rtmp_adapter *pPriv)
{
	struct dev_priv_info *pDevInfo = NULL;

	pDevInfo = (struct dev_priv_info *) netdev_priv(pDev);
	if (pDevInfo == NULL)
	{
		pDevInfo = kmalloc(sizeof(*pDevInfo), GFP_ATOMIC);
		if (pDevInfo == NULL)
			return;
	}

	pDevInfo->pPriv = pPriv;
	pDevInfo->priv_flags = 0;

	pDev->ml_priv = pDevInfo;
}


/*
========================================================================
Routine Description:
	Get private data pointer from the network interface.

Arguments:
	pDev			- the device
	pPriv			- the pointer

Return Value:
	None

Note:
========================================================================
*/
struct rtmp_adapter *RtmpOsGetNetDevPriv(struct net_device *pDev)
{
	struct dev_priv_info *pDevInfo = NULL;

	pDevInfo = (struct dev_priv_info *) netdev_priv(pDev);
	if (pDevInfo != NULL)
		return pDevInfo->pPriv;
	return NULL;
}


/*
========================================================================
Routine Description:
	Get private flags from the network interface.

Arguments:
	pDev			- the device

Return Value:
	pPriv			- the pointer

Note:
========================================================================
*/
u32 RtmpDevPrivFlagsGet(struct net_device *pDev)
{
	struct dev_priv_info *pDevInfo = NULL;

	pDevInfo = (struct dev_priv_info *) netdev_priv(pDev);
	if (pDevInfo != NULL)
		return pDevInfo->priv_flags;
	return 0;
}


/*
========================================================================
Routine Description:
	Get private flags from the network interface.

Arguments:
	pDev			- the device

Return Value:
	pPriv			- the pointer

Note:
========================================================================
*/
void RtmpDevPrivFlagsSet(struct net_device *pDev, u32 PrivFlags)
{
	struct dev_priv_info *pDevInfo = NULL;


	pDevInfo = (struct dev_priv_info *) netdev_priv(pDev);
	if (pDevInfo != NULL)
		pDevInfo->priv_flags = PrivFlags;
}

int OS_TEST_BIT(int bit, unsigned long *flags)
{
	return test_bit(bit, flags);
}

void OS_SET_BIT(int bit, unsigned long *flags)
{
	set_bit(bit, flags);
}

void OS_CLEAR_BIT(int bit, unsigned long *flags)
{
	clear_bit(bit, flags);
}


void RtmpOSFSInfoChange(RTMP_OS_FS_INFO *pOSFSInfoOrg, bool bSet)
{
	__RtmpOSFSInfoChange(pOSFSInfoOrg, bSet);
}


/* timeout -- ms */
void RTMP_SetPeriodicTimer(struct timer_list *pTimer, unsigned long timeout)
{
	timeout = ((timeout * OS_HZ) / 1000);
	pTimer->expires = jiffies + timeout;
	add_timer(pTimer);
}


/* convert NdisMInitializeTimer --> RTMP_OS_Init_Timer */
void RTMP_OS_Init_Timer(
	struct timer_list *pTimer,
	TIMER_FUNCTION function,
	void *data)
{
	if (!timer_pending(pTimer)) {
		init_timer(pTimer);
		pTimer->data = (unsigned long)data;
		pTimer->function = function;
	}

}


void RTMP_OS_Add_Timer(struct timer_list *pTimer, unsigned long timeout)
{
	if (timer_pending(pTimer))
		return;

	timeout = ((timeout * OS_HZ) / 1000);
	pTimer->expires = jiffies + timeout;
	add_timer(pTimer);
}


void RTMP_OS_Mod_Timer(struct timer_list *pTimer, unsigned long timeout)
{
	timeout = ((timeout * OS_HZ) / 1000);
	mod_timer(pTimer, jiffies + timeout);
}


void RTMP_OS_Del_Timer(struct timer_list *pTimer, bool *pCancelled)
{
	if (timer_pending(pTimer))
		*pCancelled = del_timer_sync(pTimer);
	else
		*pCancelled = true;
}


void RTMP_OS_Release_Timer(struct timer_list *pTimer)
{
}


int RtmpOSTaskKill(RTMP_OS_TASK *pTask)
{
	return __RtmpOSTaskKill(pTask);
}


INT RtmpOSTaskNotifyToExit(RTMP_OS_TASK *pTask)
{
	return __RtmpOSTaskNotifyToExit(pTask);
}


void RtmpOSTaskCustomize(RTMP_OS_TASK *pTask)
{
	__RtmpOSTaskCustomize(pTask);
}


int RtmpOSTaskAttach(
	RTMP_OS_TASK *pTask,
	RTMP_OS_TASK_CALLBACK fn,
	ULONG arg)
{
	return __RtmpOSTaskAttach(pTask, fn, arg);
}


int RtmpOSTaskInit(
	RTMP_OS_TASK *pTask,
	char *pTaskName,
	void *pPriv,
	LIST_HEADER *pTaskList,
	LIST_HEADER *pSemList)
{
	return __RtmpOSTaskInit(pTask, pTaskName, pPriv, pSemList);
}


bool RtmpOSTaskWait(void *pReserved, RTMP_OS_TASK * pTask, INT32 *pStatus)
{
	return __RtmpOSTaskWait(pReserved, pTask, pStatus);
}


void RtmpOsTaskWakeUp(RTMP_OS_TASK *pTask)
{
#ifdef KTHREAD_SUPPORT
	WAKE_UP(pTask);
#else
	RTMP_SEM_EVENT_UP(&pTask->taskSema);
#endif
}

