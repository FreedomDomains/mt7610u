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


#ifndef __RTMP_H__
#define __RTMP_H__

#include "link_list.h"
#include "spectrum_def.h"

#include "rtmp_dot11.h"
#include "wpa_cmm.h"


#include "wsc.h"







#ifdef CLIENT_WDS
#include "client_wds_cmm.h"
#endif /* CLIENT_WDS */




#ifdef WFD_SUPPORT
#include "wfd_cmm.h"
#endif /* WFD_SUPPORT */

#include "drs_extr.h"

struct _RTMP_RA_LEGACY_TB;

struct rtmp_adapter;

typedef struct _RTMP_CHIP_OP_ RTMP_CHIP_OP;
typedef struct _RTMP_CHIP_CAP_ RTMP_CHIP_CAP;

typedef struct _UAPSD_INFO {
	BOOLEAN bAPSDCapable;
} UAPSD_INFO;

#include "mcu/mcu.h"


#ifdef CONFIG_ANDES_SUPPORT
#include "mcu/mcu_and.h"
#endif

#include "rtmp_chip.h"

#include "radar.h"

#ifdef CARRIER_DETECTION_SUPPORT
#include "cs.h"
#endif /* CARRIER_DETECTION_SUPPORT */

#ifdef DFS_SUPPORT
#include "dfs.h"
#endif /* DFS_SUPPORT */

#ifdef LED_CONTROL_SUPPORT
#include "rt_led.h"
#endif /* LED_CONTROL_SUPPORT */


#ifdef RALINK_ATE
#include "rt_ate.h"
#endif /* RALINK_ATE */


/*#define DBG		1 */

/*#define DBG_DIAGNOSE		1 */
#define ASIC_VERSION	0x0000

/*+++Used for merge MiniportMMRequest() and MiniportDataMMRequest() into one function */
#define MAX_DATAMM_RETRY	3
#define MGMT_USE_QUEUE_FLAG	0x80
/*---Used for merge MiniportMMRequest() and MiniportDataMMRequest() into one function */
/* The number of channels for per-channel Tx power offset */


#define	MAXSEQ		(0xFFF)

#ifdef DOT11N_SS3_SUPPORT
#define MAX_MCS_SET 24		/* From MCS 0 ~ MCS 23 */
#else
#define MAX_MCS_SET 16		/* From MCS 0 ~ MCS 15 */
#endif /* DOT11N_SS3_SUPPORT */


#define MAX_TXPOWER_ARRAY_SIZE	5

extern unsigned char CISCO_OUI[];
extern UCHAR BaSizeArray[4];

extern UCHAR BROADCAST_ADDR[MAC_ADDR_LEN];
extern UCHAR ZERO_MAC_ADDR[MAC_ADDR_LEN];
extern ULONG BIT32[32];
extern char *CipherName[];
extern UCHAR RxwiMCSToOfdmRate[12];
extern UCHAR SNAP_802_1H[6];
extern UCHAR SNAP_BRIDGE_TUNNEL[6];
extern UCHAR EAPOL[2];
extern UCHAR IPX[2];
extern UCHAR TPID[];
extern UCHAR APPLE_TALK[2];
extern UCHAR OfdmRateToRxwiMCS[];
extern UCHAR MapUserPriorityToAccessCategory[8];

extern unsigned char RateIdToMbps[];
extern USHORT RateIdTo500Kbps[];

extern UCHAR CipherSuiteWpaNoneTkip[];
extern UCHAR CipherSuiteWpaNoneTkipLen;

extern UCHAR CipherSuiteWpaNoneAes[];
extern UCHAR CipherSuiteWpaNoneAesLen;

extern UCHAR SsidIe;
extern UCHAR SupRateIe;
extern UCHAR ExtRateIe;

#ifdef DOT11_N_SUPPORT
extern UCHAR HtCapIe;
extern UCHAR AddHtInfoIe;
extern UCHAR NewExtChanIe;
extern UCHAR BssCoexistIe;
extern UCHAR ExtHtCapIe;
#ifdef CONFIG_STA_SUPPORT
extern UCHAR PRE_N_HT_OUI[];
#endif /* CONFIG_STA_SUPPORT */
#endif /* DOT11_N_SUPPORT */
extern UCHAR ExtCapIe;

extern UCHAR ErpIe;
extern UCHAR DsIe;
extern UCHAR TimIe;
extern UCHAR WpaIe;
extern UCHAR Wpa2Ie;
extern UCHAR IbssIe;
extern UCHAR WapiIe;

extern UCHAR WPA_OUI[];
extern UCHAR RSN_OUI[];
extern UCHAR WAPI_OUI[];
extern UCHAR WME_INFO_ELEM[];
extern UCHAR WME_PARM_ELEM[];
extern UCHAR RALINK_OUI[];
extern UCHAR PowerConstraintIE[];

typedef union _CAPTURE_MODE_PACKET_BUFFER {
	struct
	{
		UINT32       BYTE0:8;
		UINT32       BYTE1:8;
		UINT32       BYTE2:8;
		UINT32       BYTE3:8;
	} field;
	UINT32                   Value;
}CAPTURE_MODE_PACKET_BUFFER, *PCAPTURE_MODE_PACKET_BUFFER;

typedef struct _RSSI_SAMPLE {
	CHAR LastRssi0;		/* last received RSSI */
	CHAR LastRssi1;		/* last received RSSI */
	CHAR LastRssi2;		/* last received RSSI */
	CHAR AvgRssi0;
	CHAR AvgRssi1;
	CHAR AvgRssi2;
	SHORT AvgRssi0X8;
	SHORT AvgRssi1X8;
	SHORT AvgRssi2X8;
	CHAR LastSnr0;
	CHAR LastSnr1;
	CHAR LastSnr2;
	CHAR AvgSnr0;
	CHAR AvgSnr1;
	CHAR AvgSnr2;
	SHORT AvgSnr0X8;
	SHORT AvgSnr1X8;
	SHORT AvgSnr2X8;
	CHAR LastNoiseLevel0;
	CHAR LastNoiseLevel1;
	CHAR LastNoiseLevel2;
} RSSI_SAMPLE;

/* */
/*  Queue structure and macros */
/* */
#define InitializeQueueHeader(QueueHeader)              \
{                                                       \
	(QueueHeader)->Head = (QueueHeader)->Tail = NULL;   \
	(QueueHeader)->Number = 0;                          \
}

#define RemoveHeadQueue(QueueHeader)                \
(QueueHeader)->Head;                                \
{                                                   \
	PQUEUE_ENTRY pNext;                             \
	if ((QueueHeader)->Head != NULL)				\
	{												\
		pNext = (QueueHeader)->Head->Next;          \
		(QueueHeader)->Head->Next = NULL;		\
		(QueueHeader)->Head = pNext;                \
		if (pNext == NULL)                          \
			(QueueHeader)->Tail = NULL;             \
		(QueueHeader)->Number--;                    \
	}												\
}

#define InsertHeadQueue(QueueHeader, QueueEntry)            \
{                                                           \
		((PQUEUE_ENTRY)QueueEntry)->Next = (QueueHeader)->Head; \
		(QueueHeader)->Head = (PQUEUE_ENTRY)(QueueEntry);       \
		if ((QueueHeader)->Tail == NULL)                        \
			(QueueHeader)->Tail = (PQUEUE_ENTRY)(QueueEntry);   \
		(QueueHeader)->Number++;                                \
}

#define InsertTailQueue(QueueHeader, QueueEntry)				\
{                                                               \
	((PQUEUE_ENTRY)QueueEntry)->Next = NULL;                    \
	if ((QueueHeader)->Tail)                                    \
		(QueueHeader)->Tail->Next = (PQUEUE_ENTRY)(QueueEntry); \
	else                                                        \
		(QueueHeader)->Head = (PQUEUE_ENTRY)(QueueEntry);       \
	(QueueHeader)->Tail = (PQUEUE_ENTRY)(QueueEntry);           \
	(QueueHeader)->Number++;                                    \
}

#define InsertTailQueueAc(pAd, pEntry, QueueHeader, QueueEntry)			\
{																		\
	((PQUEUE_ENTRY)QueueEntry)->Next = NULL;							\
	if ((QueueHeader)->Tail)											\
		(QueueHeader)->Tail->Next = (PQUEUE_ENTRY)(QueueEntry);			\
	else																\
		(QueueHeader)->Head = (PQUEUE_ENTRY)(QueueEntry);				\
	(QueueHeader)->Tail = (PQUEUE_ENTRY)(QueueEntry);					\
	(QueueHeader)->Number++;											\
}
void DisplayTxAgg (struct rtmp_adapter*pAd);


/* */
/*  Macros for flag and ref count operations */
/* */
#define RTMP_SET_FLAG(_M, _F)       ((_M)->Flags |= (_F))
#define RTMP_CLEAR_FLAG(_M, _F)     ((_M)->Flags &= ~(_F))
#define RTMP_CLEAR_FLAGS(_M)        ((_M)->Flags = 0)
#define RTMP_TEST_FLAG(_M, _F)      (((_M)->Flags & (_F)) != 0)
#define RTMP_TEST_FLAGS(_M, _F)     (((_M)->Flags & (_F)) == (_F))
/* Macro for power save flag. */
#define RTMP_SET_PSFLAG(_M, _F)       ((_M)->PSFlags |= (_F))
#define RTMP_CLEAR_PSFLAG(_M, _F)     ((_M)->PSFlags &= ~(_F))
#define RTMP_CLEAR_PSFLAGS(_M)        ((_M)->PSFlags = 0)
#define RTMP_TEST_PSFLAG(_M, _F)      (((_M)->PSFlags & (_F)) != 0)
#define RTMP_TEST_PSFLAGS(_M, _F)     (((_M)->PSFlags & (_F)) == (_F))

#define OPSTATUS_SET_FLAG(_pAd, _F)     ((_pAd)->CommonCfg.OpStatusFlags |= (_F))
#define OPSTATUS_CLEAR_FLAG(_pAd, _F)   ((_pAd)->CommonCfg.OpStatusFlags &= ~(_F))
#define OPSTATUS_TEST_FLAG(_pAd, _F)    (((_pAd)->CommonCfg.OpStatusFlags & (_F)) != 0)

#define WIFI_TEST_SET_FLAG(_pAd, _F)     ((_pAd)->CommonCfg.WiFiTestFlags |= (_F))
#define WIFI_TEST_CLEAR_FLAG(_pAd, _F)   ((_pAd)->CommonCfg.WiFiTestFlags &= ~(_F))
#define WIFI_TEST_CHECK_FLAG(_pAd, _F)    (((_pAd)->CommonCfg.WiFiTestFlags & (_F)) != 0)

#define CLIENT_STATUS_SET_FLAG(_pEntry,_F)      ((_pEntry)->ClientStatusFlags |= (_F))
#define CLIENT_STATUS_CLEAR_FLAG(_pEntry,_F)    ((_pEntry)->ClientStatusFlags &= ~(_F))
#define CLIENT_STATUS_TEST_FLAG(_pEntry,_F)     (((_pEntry)->ClientStatusFlags & (_F)) != 0)

#define RX_FILTER_SET_FLAG(_pAd, _F)    ((_pAd)->CommonCfg.PacketFilter |= (_F))
#define RX_FILTER_CLEAR_FLAG(_pAd, _F)  ((_pAd)->CommonCfg.PacketFilter &= ~(_F))
#define RX_FILTER_TEST_FLAG(_pAd, _F)   (((_pAd)->CommonCfg.PacketFilter & (_F)) != 0)

#define RTMP_SET_MORE_FLAG(_M, _F)       ((_M)->MoreFlags |= (_F))
#define RTMP_TEST_MORE_FLAG(_M, _F)      (((_M)->MoreFlags & (_F)) != 0)
#define RTMP_CLEAR_MORE_FLAG(_M, _F)     ((_M)->MoreFlags &= ~(_F))

#define SET_ASIC_CAP(_pAd, _caps)		((_pAd)->chipCap.asic_caps |= (_caps))
#define IS_ASIC_CAP(_pAd, _caps)			(((_pAd)->chipCap.asic_caps & (_caps)) != 0)
#define CLR_ASIC_CAP(_pAd, _caps)		((_pAd)->chipCap.asic_caps &= ~(_caps))


#ifdef CONFIG_STA_SUPPORT
#define STA_NO_SECURITY_ON(_p)          (_p->StaCfg.WepStatus == Ndis802_11EncryptionDisabled)
#define STA_WEP_ON(_p)                  (_p->StaCfg.WepStatus == Ndis802_11Encryption1Enabled)
#define STA_TKIP_ON(_p)                 (_p->StaCfg.WepStatus == Ndis802_11Encryption2Enabled)
#define STA_AES_ON(_p)                  (_p->StaCfg.WepStatus == Ndis802_11Encryption3Enabled)

#define STA_TGN_WIFI_ON(_p)             (_p->StaCfg.bTGnWifiTest == TRUE)
#endif /* CONFIG_STA_SUPPORT */

#define CKIP_KP_ON(_p)				((((_p)->StaCfg.CkipFlag) & 0x10) && ((_p)->StaCfg.bCkipCmicOn == TRUE))
#define CKIP_CMIC_ON(_p)			((((_p)->StaCfg.CkipFlag) & 0x08) && ((_p)->StaCfg.bCkipCmicOn == TRUE))

#define INC_RING_INDEX(_idx, _RingSize)    \
{                                          \
    (_idx) = (_idx+1) % (_RingSize);       \
}

#ifdef USB_BULK_BUF_ALIGMENT
#define CUR_WRITE_IDX_INC(_idx, _RingSize)    \
{                                          \
    (_idx) = (_idx+1) % (_RingSize);       \
}
#endif /* USB_BULK_BUF_ALIGMENT */

#ifdef DOT11_N_SUPPORT
/* StaActive.SupportedHtPhy.MCSSet is copied from AP beacon.  Don't need to update here. */
#define COPY_HTSETTINGS_FROM_MLME_AUX_TO_ACTIVE_CFG(_pAd)                                 \
{                                                                                       \
	_pAd->StaActive.SupportedHtPhy.ChannelWidth = _pAd->MlmeAux.HtCapability.HtCapInfo.ChannelWidth;      \
	_pAd->StaActive.SupportedHtPhy.MimoPs = _pAd->MlmeAux.HtCapability.HtCapInfo.MimoPs;      \
	_pAd->StaActive.SupportedHtPhy.GF = _pAd->MlmeAux.HtCapability.HtCapInfo.GF;      \
	_pAd->StaActive.SupportedHtPhy.ShortGIfor20 = _pAd->MlmeAux.HtCapability.HtCapInfo.ShortGIfor20;      \
	_pAd->StaActive.SupportedHtPhy.ShortGIfor40 = _pAd->MlmeAux.HtCapability.HtCapInfo.ShortGIfor40;      \
	_pAd->StaActive.SupportedHtPhy.TxSTBC = _pAd->MlmeAux.HtCapability.HtCapInfo.TxSTBC;      \
	_pAd->StaActive.SupportedHtPhy.RxSTBC = _pAd->MlmeAux.HtCapability.HtCapInfo.RxSTBC;      \
	_pAd->StaActive.SupportedHtPhy.ExtChanOffset = _pAd->MlmeAux.AddHtInfo.AddHtInfo.ExtChanOffset;      \
	_pAd->StaActive.SupportedHtPhy.RecomWidth = _pAd->MlmeAux.AddHtInfo.AddHtInfo.RecomWidth;      \
	_pAd->StaActive.SupportedHtPhy.OperaionMode = _pAd->MlmeAux.AddHtInfo.AddHtInfo2.OperaionMode;      \
	_pAd->StaActive.SupportedHtPhy.NonGfPresent = _pAd->MlmeAux.AddHtInfo.AddHtInfo2.NonGfPresent;      \
	NdisMoveMemory((_pAd)->MacTab.Content[BSSID_WCID].HTCapability.MCSSet, (_pAd)->StaActive.SupportedPhyInfo.MCSSet, sizeof(UCHAR) * 16);\
}

#define COPY_AP_HTSETTINGS_FROM_BEACON(_pAd, _pHtCapability)                                 \
{                                                                                       \
	_pAd->MacTab.Content[BSSID_WCID].AMsduSize = (UCHAR)(_pHtCapability->HtCapInfo.AMsduSize);	\
	_pAd->MacTab.Content[BSSID_WCID].MmpsMode= (UCHAR)(_pHtCapability->HtCapInfo.MimoPs);	\
	_pAd->MacTab.Content[BSSID_WCID].MaxRAmpduFactor = (UCHAR)(_pHtCapability->HtCapParm.MaxRAmpduFactor);	\
}
#endif /* DOT11_N_SUPPORT */

#ifdef DOT11_VHT_AC
#define COPY_VHT_FROM_MLME_AUX_TO_ACTIVE_CFG(_pAd)                                 \
{                                                                                       \
}
#endif /* DOT11_VHT_AC */


/*
	Common fragment list structure -  Identical to the scatter gather frag list structure
*/
#define NIC_MAX_PHYS_BUF_COUNT              8

typedef struct _RTMP_SCATTER_GATHER_ELEMENT {
	void *Address;
	ULONG Length;
	PULONG Reserved;
} RTMP_SCATTER_GATHER_ELEMENT, *PRTMP_SCATTER_GATHER_ELEMENT;

typedef struct _RTMP_SCATTER_GATHER_LIST {
	ULONG NumberOfElements;
	PULONG Reserved;
	RTMP_SCATTER_GATHER_ELEMENT Elements[NIC_MAX_PHYS_BUF_COUNT];
} RTMP_SCATTER_GATHER_LIST, *PRTMP_SCATTER_GATHER_LIST;


/*
	Some utility macros
*/
#ifndef min
#define min(_a, _b)     (((_a) < (_b)) ? (_a) : (_b))
#endif

#ifndef max
#define max(_a, _b)     (((_a) > (_b)) ? (_a) : (_b))
#endif

#define GET_LNA_GAIN(_pAd)	((_pAd->LatchRfRegs.Channel <= 14) ? (_pAd->BLNAGain) : ((_pAd->LatchRfRegs.Channel <= 64) ? (_pAd->ALNAGain0) : ((_pAd->LatchRfRegs.Channel <= 128) ? (_pAd->ALNAGain1) : (_pAd->ALNAGain2))))

#define INC_COUNTER64(Val)          (Val.QuadPart++)

#define INFRA_ON(_p)                (OPSTATUS_TEST_FLAG(_p, fOP_STATUS_INFRA_ON))
#define ADHOC_ON(_p)                (OPSTATUS_TEST_FLAG(_p, fOP_STATUS_ADHOC_ON))
#define MONITOR_ON(_p)              (((_p)->StaCfg.BssType) == BSS_MONITOR)
#define IDLE_ON(_p)                 (!INFRA_ON(_p) && !ADHOC_ON(_p))

/* Check LEAP & CCKM flags */
#define LEAP_ON(_p)                 (((_p)->StaCfg.LeapAuthMode) == CISCO_AuthModeLEAP)
#define LEAP_CCKM_ON(_p)            ((((_p)->StaCfg.LeapAuthMode) == CISCO_AuthModeLEAP) && ((_p)->StaCfg.LeapAuthInfo.CCKM == TRUE))

/* if orginal Ethernet frame contains no LLC/SNAP, then an extra LLC/SNAP encap is required */
#define EXTRA_LLCSNAP_ENCAP_FROM_PKT_START(_pBufVA, _pExtraLlcSnapEncap)		\
{																\
	if (((*(_pBufVA + 12) << 8) + *(_pBufVA + 13)) > 1500)		\
	{															\
		_pExtraLlcSnapEncap = SNAP_802_1H;						\
		if (NdisEqualMemory(IPX, _pBufVA + 12, 2) || 			\
			NdisEqualMemory(APPLE_TALK, _pBufVA + 12, 2))		\
		{														\
			_pExtraLlcSnapEncap = SNAP_BRIDGE_TUNNEL;			\
		}														\
	}															\
	else														\
	{															\
		_pExtraLlcSnapEncap = NULL;								\
	}															\
}

/* New Define for new Tx Path. */
#define EXTRA_LLCSNAP_ENCAP_FROM_PKT_OFFSET(_pBufVA, _pExtraLlcSnapEncap)	\
{																\
	if (((*(_pBufVA) << 8) + *(_pBufVA + 1)) > 1500)			\
	{															\
		_pExtraLlcSnapEncap = SNAP_802_1H;						\
		if (NdisEqualMemory(IPX, _pBufVA, 2) || 				\
			NdisEqualMemory(APPLE_TALK, _pBufVA, 2))			\
		{														\
			_pExtraLlcSnapEncap = SNAP_BRIDGE_TUNNEL;			\
		}														\
	}															\
	else														\
	{															\
		_pExtraLlcSnapEncap = NULL;								\
	}															\
}

#define MAKE_802_3_HEADER(_p, _pMac1, _pMac2, _pType)                   \
{                                                                       \
    NdisMoveMemory(_p, _pMac1, MAC_ADDR_LEN);                           \
    NdisMoveMemory((_p + MAC_ADDR_LEN), _pMac2, MAC_ADDR_LEN);          \
    NdisMoveMemory((_p + MAC_ADDR_LEN * 2), _pType, LENGTH_802_3_TYPE); \
}

/*
	if pData has no LLC/SNAP (neither RFC1042 nor Bridge tunnel),
		keep it that way.
	else if the received frame is LLC/SNAP-encaped IPX or APPLETALK,
		preserve the LLC/SNAP field
	else remove the LLC/SNAP field from the result Ethernet frame

	Patch for WHQL only, which did not turn on Netbios but use IPX within its payload
	Note:
		_pData & _DataSize may be altered (remove 8-byte LLC/SNAP) by this MACRO
		_pRemovedLLCSNAP: pointer to removed LLC/SNAP; NULL is not removed
*/
#define CONVERT_TO_802_3(_p8023hdr, _pDA, _pSA, _pData, _DataSize, _pRemovedLLCSNAP)      \
{                                                                       \
    char LLC_Len[2];                                                    \
                                                                        \
    _pRemovedLLCSNAP = NULL;                                            \
    if (NdisEqualMemory(SNAP_802_1H, _pData, 6)  ||                     \
        NdisEqualMemory(SNAP_BRIDGE_TUNNEL, _pData, 6))                 \
    {                                                                   \
        PUCHAR pProto = _pData + 6;                                     \
                                                                        \
        if ((NdisEqualMemory(IPX, pProto, 2) || NdisEqualMemory(APPLE_TALK, pProto, 2)) &&  \
            NdisEqualMemory(SNAP_802_1H, _pData, 6))                    \
        {                                                               \
            LLC_Len[0] = (UCHAR)(_DataSize >> 8);                       \
            LLC_Len[1] = (UCHAR)(_DataSize & (256 - 1));                \
            MAKE_802_3_HEADER(_p8023hdr, _pDA, _pSA, LLC_Len);          \
        }                                                               \
        else                                                            \
        {                                                               \
            MAKE_802_3_HEADER(_p8023hdr, _pDA, _pSA, pProto);           \
            _pRemovedLLCSNAP = _pData;                                  \
            _DataSize -= LENGTH_802_1_H;                                \
            _pData += LENGTH_802_1_H;                                   \
        }                                                               \
    }                                                                   \
    else                                                                \
    {                                                                   \
        LLC_Len[0] = (UCHAR)(_DataSize >> 8);                           \
        LLC_Len[1] = (UCHAR)(_DataSize & (256 - 1));                    \
        MAKE_802_3_HEADER(_p8023hdr, _pDA, _pSA, LLC_Len);              \
    }                                                                   \
}

/*
	Enqueue this frame to MLME engine
	We need to enqueue the whole frame because MLME need to pass data type
	information from 802.11 header
*/
#ifdef RTMP_MAC_USB
#define REPORT_MGMT_FRAME_TO_MLME(_pAd, Wcid, _pFrame, _FrameSize, _Rssi0, _Rssi1, _Rssi2, _MinSNR, _OpMode)        \
{                                                                                       \
    UINT32 High32TSF=0, Low32TSF=0;                                                          \
    MlmeEnqueueForRecv(_pAd, Wcid, High32TSF, Low32TSF, (UCHAR)_Rssi0, (UCHAR)_Rssi1,(UCHAR)_Rssi2,_FrameSize, _pFrame, (UCHAR)_MinSNR, _OpMode);   \
}
#endif /* RTMP_MAC_USB */

#define MAC_ADDR_EQUAL(pAddr1,pAddr2)           RTMPEqualMemory((void *)(pAddr1), (void *)(pAddr2), MAC_ADDR_LEN)
#define SSID_EQUAL(ssid1, len1, ssid2, len2)    ((len1==len2) && (RTMPEqualMemory(ssid1, ssid2, len1)))


#ifdef CONFIG_STA_SUPPORT
#define STA_EXTRA_SETTING(_pAd)

#define STA_PORT_SECURED(_pAd) \
{ \
	BOOLEAN	Cancelled; \
	(_pAd)->StaCfg.PortSecured = WPA_802_1X_PORT_SECURED; \
	RTMP_IndicateMediaState(_pAd, NdisMediaStateConnected); \
	NdisAcquireSpinLock(&((_pAd)->MacTabLock)); \
	(_pAd)->MacTab.Content[BSSID_WCID].PortSecured = (_pAd)->StaCfg.PortSecured; \
	(_pAd)->MacTab.Content[BSSID_WCID].PrivacyFilter = Ndis802_11PrivFilterAcceptAll;\
	NdisReleaseSpinLock(&(_pAd)->MacTabLock); \
	RTMPCancelTimer(&((_pAd)->Mlme.LinkDownTimer), &Cancelled);\
	STA_EXTRA_SETTING(_pAd); \
}
#endif /* CONFIG_STA_SUPPORT */

/* */
/*  Data buffer for DMA operation, the buffer must be contiguous physical memory */
/*  Both DMA to / from CPU use the same structure. */
/* */
typedef struct _RTMP_DMABUF {
	ULONG AllocSize;
	void *AllocVa;		/* TxBuf virtual address */
	NDIS_PHYSICAL_ADDRESS AllocPa;	/* TxBuf physical address */
} RTMP_DMABUF, *PRTMP_DMABUF;

/* */
/* Control block (Descriptor) for all ring descriptor DMA operation, buffer must be */
/* contiguous physical memory. NDIS_PACKET stored the binding Rx packet descriptor */
/* which won't be released, driver has to wait until upper layer return the packet */
/* before giveing up this rx ring descriptor to ASIC. NDIS_BUFFER is assocaited pair */
/* to describe the packet buffer. For Tx, NDIS_PACKET stored the tx packet descriptor */
/* which driver should ACK upper layer when the tx is physically done or failed. */
/* */
typedef struct _RTMP_DMACB {
	ULONG AllocSize;	/* Control block size */
	void *AllocVa;		/* Control block virtual address */
	NDIS_PHYSICAL_ADDRESS AllocPa;	/* Control block physical address */
	PNDIS_PACKET pNdisPacket;
	PNDIS_PACKET pNextNdisPacket;

	RTMP_DMABUF DmaBuf;	/* Associated DMA buffer structure */
#ifdef CACHE_LINE_32B
	RXD_STRUC LastBDInfo;
#endif /* CACHE_LINE_32B */
} RTMP_DMACB, *PRTMP_DMACB;

typedef struct _RTMP_TX_RING {
	RTMP_DMACB Cell[TX_RING_SIZE];
	UINT32 TxCpuIdx;
	UINT32 TxDmaIdx;
	UINT32 TxSwFreeIdx;	/* software next free tx index */
} RTMP_TX_RING, *PRTMP_TX_RING;

typedef struct _RTMP_RX_RING {
	RTMP_DMACB Cell[RX_RING_SIZE];
	UINT32 RxCpuIdx;
	UINT32 RxDmaIdx;
	INT32 RxSwReadIdx;	/* software next read index */
} RTMP_RX_RING, *PRTMP_RX_RING;

typedef struct _RTMP_MGMT_RING {
	RTMP_DMACB Cell[MGMT_RING_SIZE];
	UINT32 TxCpuIdx;
	UINT32 TxDmaIdx;
	UINT32 TxSwFreeIdx;	/* software next free tx index */
} RTMP_MGMT_RING, *PRTMP_MGMT_RING;

typedef struct _RTMP_CTRL_RING {
	RTMP_DMACB Cell[16];
	UINT32 TxCpuIdx;
	UINT32 TxDmaIdx;
	UINT32 TxSwFreeIdx;	/* software next free tx index */
} RTMP_CTRL_RING, *PRTMP_CTRL_RING;

/* */
/*  Statistic counter structure */
/* */
typedef struct _COUNTER_802_3 {
	/* General Stats */
	ULONG GoodTransmits;
	ULONG GoodReceives;
	ULONG TxErrors;
	ULONG RxErrors;
	ULONG RxNoBuffer;

	/* Ethernet Stats */
	ULONG RcvAlignmentErrors;
	ULONG OneCollision;
	ULONG MoreCollisions;

} COUNTER_802_3, *PCOUNTER_802_3;

typedef struct _COUNTER_802_11 {
	ULONG Length;
/*	LARGE_INTEGER   LastTransmittedFragmentCount; */
	LARGE_INTEGER TransmittedFragmentCount;
	LARGE_INTEGER MulticastTransmittedFrameCount;
	LARGE_INTEGER FailedCount;
	LARGE_INTEGER RetryCount;
	LARGE_INTEGER MultipleRetryCount;
	LARGE_INTEGER RTSSuccessCount;
	LARGE_INTEGER RTSFailureCount;
	LARGE_INTEGER ACKFailureCount;
	LARGE_INTEGER FrameDuplicateCount;
	LARGE_INTEGER ReceivedFragmentCount;
	LARGE_INTEGER MulticastReceivedFrameCount;
	LARGE_INTEGER FCSErrorCount;
	LARGE_INTEGER TransmittedFrameCount;
	LARGE_INTEGER WEPUndecryptableCount;
	LARGE_INTEGER TransmitCountFrmOs;
} COUNTER_802_11, *PCOUNTER_802_11;


#ifdef RT3290
typedef struct _ANT_DIVERSITY
{
	BOOLEAN		RateUp;
	ULONG		TrainCounter;
	ULONG		AntennaDiversityState;	// 0->Stable state 1->training state
	ULONG		AntennaDiversityPER[2];  // 0 ->main 1->aux
	ULONG		AntennaDiversityTxPacketCount[2];  // 0 ->main 1->aux
	ULONG		AntennaDiversityRxPacketCount[2];
	CHAR		Rssi[2];
	ULONG		AntennaDiversityCount;
	ULONG		AntennaDiversityTrigger;
}ANT_DIVERSITY, *PANT_DIVERSITY;
#endif /* RT3290 */

typedef struct _COUNTER_RALINK {
	UINT32 OneSecStart;	/* for one sec count clear use */
	UINT32 OneSecBeaconSentCnt;
	UINT32 OneSecFalseCCACnt;	/* CCA error count, for debug purpose, might move to global counter */
	UINT32 OneSecRxFcsErrCnt;	/* CRC error */
	UINT32 OneSecRxOkCnt;	/* RX without error */
	UINT32 OneSecTxFailCount;
	UINT32 OneSecTxNoRetryOkCount;
	UINT32 OneSecTxRetryOkCount;
	UINT32 OneSecRxOkDataCnt;	/* unicast-to-me DATA frame count */
	UINT32 OneSecTransmittedByteCount;	/* both successful and failure, used to calculate TX throughput */

	ULONG OneSecOsTxCount[NUM_OF_TX_RING];
	ULONG OneSecDmaDoneCount[NUM_OF_TX_RING];
	UINT32 OneSecTxDoneCount;
	ULONG OneSecRxCount;
	UINT32 OneSecReceivedByteCount;
	UINT32 OneSecTxAggregationCount;
	UINT32 OneSecRxAggregationCount;
	UINT32 OneSecEnd;	/* for one sec count clear use */

	ULONG TransmittedByteCount;	/* both successful and failure, used to calculate TX throughput */
	ULONG ReceivedByteCount;	/* both CRC okay and CRC error, used to calculate RX throughput */
#ifdef RT3290
	// TODO: shiang, check the purpose of following parameter
	ULONG OneSecRxOkCnt2; /* RX without error */
#endif /* RT3290 */
	ULONG BadCQIAutoRecoveryCount;
	ULONG PoorCQIRoamingCount;
	ULONG MgmtRingFullCount;
	ULONG RxCountSinceLastNULL;
	ULONG RxCount;
	ULONG KickTxCount;
	LARGE_INTEGER RealFcsErrCount;
	ULONG PendingNdisPacketCount;
	ULONG FalseCCACnt;                    /* CCA error count */

	UINT32 LastOneSecTotalTxCount;	/* OneSecTxNoRetryOkCount + OneSecTxRetryOkCount + OneSecTxFailCount */
	UINT32 LastOneSecRxOkDataCnt;	/* OneSecRxOkDataCnt */
	ULONG DuplicateRcv;
	ULONG TxAggCount;
	ULONG TxNonAggCount;
	ULONG TxAgg1MPDUCount;
	ULONG TxAgg2MPDUCount;
	ULONG TxAgg3MPDUCount;
	ULONG TxAgg4MPDUCount;
	ULONG TxAgg5MPDUCount;
	ULONG TxAgg6MPDUCount;
	ULONG TxAgg7MPDUCount;
	ULONG TxAgg8MPDUCount;
	ULONG TxAgg9MPDUCount;
	ULONG TxAgg10MPDUCount;
	ULONG TxAgg11MPDUCount;
	ULONG TxAgg12MPDUCount;
	ULONG TxAgg13MPDUCount;
	ULONG TxAgg14MPDUCount;
	ULONG TxAgg15MPDUCount;
	ULONG TxAgg16MPDUCount;

	LARGE_INTEGER TransmittedOctetsInAMSDU;
	LARGE_INTEGER TransmittedAMSDUCount;
	LARGE_INTEGER ReceivedOctesInAMSDUCount;
	LARGE_INTEGER ReceivedAMSDUCount;
	LARGE_INTEGER TransmittedAMPDUCount;
	LARGE_INTEGER TransmittedMPDUsInAMPDUCount;
	LARGE_INTEGER TransmittedOctetsInAMPDUCount;
	LARGE_INTEGER MPDUInReceivedAMPDUCount;

	ULONG PhyErrCnt;
	ULONG PlcpErrCnt;
} COUNTER_RALINK, *PCOUNTER_RALINK;

typedef struct _COUNTER_DRS {
	/* to record the each TX rate's quality. 0 is best, the bigger the worse. */
	USHORT TxQuality[MAX_TX_RATE_INDEX+1];
	UCHAR PER[MAX_TX_RATE_INDEX+1];
	UCHAR TxRateUpPenalty;	/* extra # of second penalty due to last unstable condition */
	ULONG CurrTxRateStableTime;	/* # of second in current TX rate */
	/*BOOLEAN         fNoisyEnvironment; */
	BOOLEAN fLastSecAccordingRSSI;
	UCHAR LastSecTxRateChangeAction;	/* 0: no change, 1:rate UP, 2:rate down */
	UCHAR LastTimeTxRateChangeAction;	/*Keep last time value of LastSecTxRateChangeAction */
	ULONG LastTxOkCount;
} COUNTER_DRS, *PCOUNTER_DRS;


#ifdef DOT11_N_SUPPORT
#ifdef TXBF_SUPPORT
typedef
    struct {
	ULONG TxSuccessCount;
	ULONG TxRetryCount;
	ULONG TxFailCount;
	ULONG ETxSuccessCount;
	ULONG ETxRetryCount;
	ULONG ETxFailCount;
	ULONG ITxSuccessCount;
	ULONG ITxRetryCount;
	ULONG ITxFailCount;
} COUNTER_TXBF;
#endif /* TXBF_SUPPORT */
#endif /* DOT11_N_SUPPORT */


#ifdef STREAM_MODE_SUPPORT
typedef struct _STREAM_MODE_ENTRY_{
#define STREAM_MODE_STATIC		1
	USHORT flag;
	UCHAR macAddr[MAC_ADDR_LEN];
}STREAM_MODE_ENTRY;
#endif /* STREAM_MODE_SUPPORT */

/***************************************************************************
  *	security key related data structure
  **************************************************************************/

/* structure to define WPA Group Key Rekey Interval */
typedef struct GNU_PACKED _RT_802_11_WPA_REKEY {
	ULONG ReKeyMethod;	/* mechanism for rekeying: 0:disable, 1: time-based, 2: packet-based */
	ULONG ReKeyInterval;	/* time-based: seconds, packet-based: kilo-packets */
} RT_WPA_REKEY,*PRT_WPA_REKEY, RT_802_11_WPA_REKEY, *PRT_802_11_WPA_REKEY;


#ifdef RTMP_MAC_USB
/***************************************************************************
  *	RTUSB I/O related data structure
  **************************************************************************/

/* for USB interface, avoid in interrupt when write key */
typedef struct RT_ADD_PAIRWISE_KEY_ENTRY {
	UCHAR MacAddr[6];
	USHORT MacTabMatchWCID;	/* ASIC */
	CIPHER_KEY CipherKey;
} RT_ADD_PAIRWISE_KEY_ENTRY,*PRT_ADD_PAIRWISE_KEY_ENTRY;


/* Cipher suite type for mixed mode group cipher, P802.11i-2004 */
typedef enum _RT_802_11_CIPHER_SUITE_TYPE {
	Cipher_Type_NONE,
	Cipher_Type_WEP40,
	Cipher_Type_TKIP,
	Cipher_Type_RSVD,
	Cipher_Type_CCMP,
	Cipher_Type_WEP104
} RT_802_11_CIPHER_SUITE_TYPE, *PRT_802_11_CIPHER_SUITE_TYPE;
#endif /* RTMP_MAC_USB */

typedef struct {
	UCHAR Addr[MAC_ADDR_LEN];
	UCHAR ErrorCode[2];	/*00 01-Invalid authentication type */
	/*00 02-Authentication timeout */
	/*00 03-Challenge from AP failed */
	/*00 04-Challenge to AP failed */
	BOOLEAN Reported;
} ROGUEAP_ENTRY, *PROGUEAP_ENTRY;

typedef struct {
	UCHAR RogueApNr;
	ROGUEAP_ENTRY RogueApEntry[MAX_LEN_OF_BSS_TABLE];
} ROGUEAP_TABLE, *PROGUEAP_TABLE;

/*
  *	Fragment Frame structure
  */
typedef struct _FRAGMENT_FRAME {
	PNDIS_PACKET pFragPacket;
	ULONG RxSize;
	USHORT Sequence;
	USHORT LastFrag;
	ULONG Flags;		/* Some extra frame information. bit 0: LLC presented */
} FRAGMENT_FRAME, *PFRAGMENT_FRAME;


/* */
/* Tkip Key structure which RC4 key & MIC calculation */
/* */
typedef struct _TKIP_KEY_INFO {
	UINT nBytesInM;		/* # bytes in M for MICKEY */
	ULONG IV16;
	ULONG IV32;
	ULONG K0;		/* for MICKEY Low */
	ULONG K1;		/* for MICKEY Hig */
	ULONG L;		/* Current state for MICKEY */
	ULONG R;		/* Current state for MICKEY */
	ULONG M;		/* Message accumulator for MICKEY */
	UCHAR RC4KEY[16];
	UCHAR MIC[8];
} TKIP_KEY_INFO, *PTKIP_KEY_INFO;


/* */
/* Private / Misc data, counters for driver internal use */
/* */
typedef struct __PRIVATE_STRUC {
	UINT SystemResetCnt;	/* System reset counter */
	UINT TxRingFullCnt;	/* Tx ring full occurrance number */
	UINT PhyRxErrCnt;	/* PHY Rx error count, for debug purpose, might move to global counter */
	/* Variables for WEP encryption / decryption in rtmp_wep.c */
	/* Tkip stuff */
	TKIP_KEY_INFO Tx;
	TKIP_KEY_INFO Rx;
} PRIVATE_STRUC, *PPRIVATE_STRUC;


/***************************************************************************
  *	Channel and BBP related data structures
  **************************************************************************/
/* structure to tune BBP R66 (BBP TUNING) */
typedef struct _BBP_R66_TUNING {
	BOOLEAN bEnable;
	USHORT FalseCcaLowerThreshold;	/* default 100 */
	USHORT FalseCcaUpperThreshold;	/* default 512 */
	UCHAR R66Delta;
	UCHAR R66CurrentValue;
	BOOLEAN R66LowerUpperSelect;	/*Before LinkUp, Used LowerBound or UpperBound as R66 value. */
} BBP_R66_TUNING, *PBBP_R66_TUNING;


#ifdef DOT11_N_SUPPORT
#ifdef DOT11N_DRAFT3
#define EFFECTED_CH_SECONDARY 0x1
#define EFFECTED_CH_PRIMARY	0x2
#define EFFECTED_CH_LEGACY		0x4
#endif /* DOT11N_DRAFT3 */
#endif /* DOT11_N_SUPPORT */

/* structure to store channel TX power */
typedef struct _CHANNEL_TX_POWER {
	USHORT RemainingTimeForUse;	/*unit: sec */
	UCHAR Channel;
#ifdef DOT11N_DRAFT3
	BOOLEAN bEffectedChannel;	/* For BW 40 operating in 2.4GHz , the "effected channel" is the channel that is covered in 40Mhz. */
#endif /* DOT11N_DRAFT3 */
	CHAR Power;
	CHAR Power2;
#ifdef DOT11N_SS3_SUPPORT
	CHAR Power3;
#endif /* DOT11N_SS3_SUPPORT */
	UCHAR MaxTxPwr;
	UCHAR DfsReq;
	UCHAR RegulatoryDomain;

/*
	Channel property:

	CHANNEL_DISABLED: The channel is disabled.
	CHANNEL_PASSIVE_SCAN: Only passive scanning is allowed.
	CHANNEL_NO_IBSS: IBSS is not allowed.
	CHANNEL_RADAR: Radar detection is required.
	CHANNEL_NO_FAT_ABOVE: Extension channel above this channel is not allowed.
	CHANNEL_NO_FAT_BELOW: Extension channel below this channel is not allowed.
	CHANNEL_40M_CAP: 40 BW channel group
 */
#define CHANNEL_DEFAULT_PROP	0x00
#define CHANNEL_DISABLED		0x01	/* no use */
#define CHANNEL_PASSIVE_SCAN	0x02
#define CHANNEL_NO_IBSS			0x04
#define CHANNEL_RADAR			0x08
#define CHANNEL_NO_FAT_ABOVE	0x10
#define CHANNEL_NO_FAT_BELOW	0x20
#define CHANNEL_40M_CAP			0x40

	UCHAR Flags;

} CHANNEL_TX_POWER, *PCHANNEL_TX_POWER;

/* Channel list subset */
typedef struct _CHANNEL_LIST_SUB {
	UCHAR	Channel;
	UCHAR	IdxMap; /* Index mapping to original channel list */
} CHANNEL_LIST_SUB, *PCHANNEL_LIST_SUB;


typedef struct _SOFT_RX_ANT_DIVERSITY_STRUCT {
	UCHAR EvaluatePeriod;	/* 0:not evalute status, 1: evaluate status, 2: switching status */
	UCHAR EvaluateStableCnt;
	UCHAR Pair1PrimaryRxAnt;	/* 0:Ant-E1, 1:Ant-E2 */
	UCHAR Pair1SecondaryRxAnt;	/* 0:Ant-E1, 1:Ant-E2 */
#ifdef CONFIG_STA_SUPPORT
	SHORT Pair1AvgRssi[2];	/* AvgRssi[0]:E1, AvgRssi[1]:E2 */
	SHORT Pair2AvgRssi[2];	/* AvgRssi[0]:E3, AvgRssi[1]:E4 */
#endif /* CONFIG_STA_SUPPORT */
	SHORT Pair1LastAvgRssi;	/* */
	SHORT Pair2LastAvgRssi;	/* */
	ULONG RcvPktNumWhenEvaluate;
	BOOLEAN FirstPktArrivedWhenEvaluate;
} SOFT_RX_ANT_DIVERSITY, *PSOFT_RX_ANT_DIVERSITY;

typedef enum _ABGBAND_STATE_ {
	UNKNOWN_BAND,
	BG_BAND,
	A_BAND,
} ABGBAND_STATE;

#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */
/***************************************************************************
  *	structure for MLME state machine
  **************************************************************************/
typedef struct _MLME_STRUCT {
#ifdef CONFIG_STA_SUPPORT
	/* STA state machines */
	STATE_MACHINE CntlMachine;
	STATE_MACHINE AssocMachine;
	STATE_MACHINE AuthMachine;
	STATE_MACHINE AuthRspMachine;
	STATE_MACHINE SyncMachine;
	STATE_MACHINE WpaPskMachine;
	STATE_MACHINE LeapMachine;
	STATE_MACHINE_FUNC AssocFunc[ASSOC_FUNC_SIZE];
	STATE_MACHINE_FUNC AuthFunc[AUTH_FUNC_SIZE];
	STATE_MACHINE_FUNC AuthRspFunc[AUTH_RSP_FUNC_SIZE];
	STATE_MACHINE_FUNC SyncFunc[SYNC_FUNC_SIZE];

#endif /* CONFIG_STA_SUPPORT */
	STATE_MACHINE_FUNC ActFunc[ACT_FUNC_SIZE];
	/* Action */
	STATE_MACHINE ActMachine;


#ifdef QOS_DLS_SUPPORT
	STATE_MACHINE DlsMachine;
	STATE_MACHINE_FUNC DlsFunc[DLS_FUNC_SIZE];
#endif /* QOS_DLS_SUPPORT */



	/* common WPA state machine */
	STATE_MACHINE WpaMachine;
	STATE_MACHINE_FUNC WpaFunc[WPA_FUNC_SIZE];


	ULONG ChannelQuality;	/* 0..100, Channel Quality Indication for Roaming */
	ULONG Now32;		/* latch the value of NdisGetSystemUpTime() */
	ULONG LastSendNULLpsmTime;

	BOOLEAN bRunning;
	NDIS_SPIN_LOCK TaskLock;
	MLME_QUEUE Queue;

	UINT ShiftReg;

	RALINK_TIMER_STRUCT PeriodicTimer;
	RALINK_TIMER_STRUCT APSDPeriodicTimer;
	RALINK_TIMER_STRUCT LinkDownTimer;
	RALINK_TIMER_STRUCT LinkUpTimer;
	ULONG PeriodicRound;
	ULONG GPIORound;
	ULONG OneSecPeriodicRound;

	UCHAR RealRxPath;
	BOOLEAN bLowThroughput;
	BOOLEAN bEnableAutoAntennaCheck;
	RALINK_TIMER_STRUCT RxAntEvalTimer;


#ifdef RTMP_MAC_USB
	RALINK_TIMER_STRUCT AutoWakeupTimer;
	BOOLEAN AutoWakeupTimerRunning;
#endif /* RTMP_MAC_USB */
} MLME_STRUCT, *PMLME_STRUCT;

#ifdef DOT11_N_SUPPORT
/***************************************************************************
  *	802.11 N related data structures
  **************************************************************************/
struct reordering_mpdu {
	struct reordering_mpdu *next;
	PNDIS_PACKET pPacket;	/* coverted to 802.3 frame */
	int Sequence;		/* sequence number of MPDU */
	BOOLEAN bAMSDU;
	UCHAR					OpMode;
};

struct reordering_list {
	struct reordering_mpdu *next;
	int qlen;
};

struct reordering_mpdu_pool {
	void *mem;
	NDIS_SPIN_LOCK lock;
	struct reordering_list freelist;
};

typedef enum _REC_BLOCKACK_STATUS {
	Recipient_NONE = 0,
	Recipient_USED,
	Recipient_HandleRes,
	Recipient_Accept
} REC_BLOCKACK_STATUS, *PREC_BLOCKACK_STATUS;

typedef enum _ORI_BLOCKACK_STATUS {
	Originator_NONE = 0,
	Originator_USED,
	Originator_WaitRes,
	Originator_Done
} ORI_BLOCKACK_STATUS, *PORI_BLOCKACK_STATUS;

typedef struct _BA_ORI_ENTRY {
	UCHAR Wcid;
	UCHAR TID;
	UCHAR BAWinSize;
	UCHAR Token;
/* Sequence is to fill every outgoing QoS DATA frame's sequence field in 802.11 header. */
	USHORT Sequence;
	USHORT TimeOutValue;
	ORI_BLOCKACK_STATUS ORI_BA_Status;
	RALINK_TIMER_STRUCT ORIBATimer;
	void *pAdapter;
} BA_ORI_ENTRY, *PBA_ORI_ENTRY;

typedef struct _BA_REC_ENTRY {
	UCHAR Wcid;
	UCHAR TID;
	UCHAR BAWinSize;	/* 7.3.1.14. each buffer is capable of holding a max AMSDU or MSDU. */
	/*UCHAR NumOfRxPkt; */
	/*UCHAR    Curindidx; // the head in the RX reordering buffer */
	USHORT LastIndSeq;
/*	USHORT		LastIndSeqAtTimer; */
	USHORT TimeOutValue;
	RALINK_TIMER_STRUCT RECBATimer;
	ULONG LastIndSeqAtTimer;
	ULONG nDropPacket;
	ULONG rcvSeq;
	REC_BLOCKACK_STATUS REC_BA_Status;
/*	UCHAR	RxBufIdxUsed; */
	/* corresponding virtual address for RX reordering packet storage. */
	/*RTMP_REORDERDMABUF MAP_RXBuf[MAX_RX_REORDERBUF]; */
	NDIS_SPIN_LOCK RxReRingLock;	/* Rx Ring spinlock */
/*	struct _BA_REC_ENTRY *pNext; */
	void *pAdapter;
	struct reordering_list list;
} BA_REC_ENTRY, *PBA_REC_ENTRY;


typedef struct {
	ULONG numAsRecipient;	/* I am recipient of numAsRecipient clients. These client are in the BARecEntry[] */
	ULONG numAsOriginator;	/* I am originator of   numAsOriginator clients. These clients are in the BAOriEntry[] */
	ULONG numDoneOriginator;	/* count Done Originator sessions */
	BA_ORI_ENTRY BAOriEntry[MAX_LEN_OF_BA_ORI_TABLE];
	BA_REC_ENTRY BARecEntry[MAX_LEN_OF_BA_REC_TABLE];
} BA_TABLE, *PBA_TABLE;

/*For QureyBATableOID use; */
typedef struct GNU_PACKED _OID_BA_REC_ENTRY {
	UCHAR MACAddr[MAC_ADDR_LEN];
	UCHAR BaBitmap;		/* if (BaBitmap&(1<<TID)), this session with{MACAddr, TID}exists, so read BufSize[TID] for BufferSize */
	UCHAR rsv;
	UCHAR BufSize[8];
	REC_BLOCKACK_STATUS REC_BA_Status[8];
} OID_BA_REC_ENTRY, *POID_BA_REC_ENTRY;

/*For QureyBATableOID use; */
typedef struct GNU_PACKED _OID_BA_ORI_ENTRY {
	UCHAR MACAddr[MAC_ADDR_LEN];
	UCHAR BaBitmap;		/* if (BaBitmap&(1<<TID)), this session with{MACAddr, TID}exists, so read BufSize[TID] for BufferSize, read ORI_BA_Status[TID] for status */
	UCHAR rsv;
	UCHAR BufSize[8];
	ORI_BLOCKACK_STATUS ORI_BA_Status[8];
} OID_BA_ORI_ENTRY, *POID_BA_ORI_ENTRY;

typedef struct _QUERYBA_TABLE {
	OID_BA_ORI_ENTRY BAOriEntry[32];
	OID_BA_REC_ENTRY BARecEntry[32];
	UCHAR OriNum;		/* Number of below BAOriEntry */
	UCHAR RecNum;		/* Number of below BARecEntry */
} QUERYBA_TABLE, *PQUERYBA_TABLE;

typedef union _BACAP_STRUC {
#ifdef RT_BIG_ENDIAN
	struct {
		UINT32:4;
		UINT32 b2040CoexistScanSup:1;	/*As Sta, support do 2040 coexistence scan for AP. As Ap, support monitor trigger event to check if can use BW 40MHz. */
		UINT32 bHtAdhoc:1;	/* adhoc can use ht rate. */
		UINT32 MMPSmode:2;	/* MIMO power save more, 0:static, 1:dynamic, 2:rsv, 3:mimo enable */
		UINT32 AmsduSize:1;	/* 0:3839, 1:7935 bytes. UINT  MSDUSizeToBytes[]        = { 3839, 7935}; */
		UINT32 AmsduEnable:1;	/*Enable AMSDU transmisstion */
		UINT32 MpduDensity:3;
		UINT32 Policy:2;	/* 0: DELAY_BA 1:IMMED_BA  (//BA Policy subfiled value in ADDBA frame)   2:BA-not use */
		UINT32 AutoBA:1;	/* automatically BA */
		UINT32 TxBAWinLimit:8;
		UINT32 RxBAWinLimit:8;
	} field;
#else
	struct {
		UINT32 RxBAWinLimit:8;
		UINT32 TxBAWinLimit:8;
		UINT32 AutoBA:1;	/* automatically BA */
		UINT32 Policy:2;	/* 0: DELAY_BA 1:IMMED_BA  (//BA Policy subfiled value in ADDBA frame)   2:BA-not use */
		UINT32 MpduDensity:3;
		UINT32 AmsduEnable:1;	/*Enable AMSDU transmisstion */
		UINT32 AmsduSize:1;	/* 0:3839, 1:7935 bytes. UINT  MSDUSizeToBytes[]        = { 3839, 7935}; */
		UINT32 MMPSmode:2;	/* MIMO power save more, 0:static, 1:dynamic, 2:rsv, 3:mimo enable */
		UINT32 bHtAdhoc:1;	/* adhoc can use ht rate. */
		UINT32 b2040CoexistScanSup:1;	/*As Sta, support do 2040 coexistence scan for AP. As Ap, support monitor trigger event to check if can use BW 40MHz. */
		 UINT32:4;
	} field;
#endif
	UINT32 word;
} BACAP_STRUC, *PBACAP_STRUC;

typedef struct {
	BOOLEAN IsRecipient;
	UCHAR MACAddr[MAC_ADDR_LEN];
	UCHAR TID;
	UCHAR nMSDU;
	USHORT TimeOut;
	BOOLEAN bAllTid;	/* If True, delete all TID for BA sessions with this MACaddr. */
} OID_ADD_BA_ENTRY, *POID_ADD_BA_ENTRY;

#ifdef DOT11N_DRAFT3
typedef enum _BSS2040COEXIST_FLAG {
	BSS_2040_COEXIST_DISABLE = 0,
	BSS_2040_COEXIST_TIMER_FIRED = 1,
	BSS_2040_COEXIST_INFO_SYNC = 2,
	BSS_2040_COEXIST_INFO_NOTIFY = 4,
} BSS2040COEXIST_FLAG;

typedef struct _BssCoexChRange_ {
	UCHAR primaryCh;
	UCHAR secondaryCh;
	UCHAR effectChStart;
	UCHAR effectChEnd;
} BSS_COEX_CH_RANGE;
#endif /* DOT11N_DRAFT3 */

#define IS_HT_STA(_pMacEntry)	\
	(_pMacEntry->MaxHTPhyMode.field.MODE >= MODE_HTMIX)

#define IS_HT_RATE(_pMacEntry)	\
	(_pMacEntry->HTPhyMode.field.MODE >= MODE_HTMIX)

#define PEER_IS_HT_RATE(_pMacEntry)	\
	(_pMacEntry->HTPhyMode.field.MODE >= MODE_HTMIX)

#endif /* DOT11_N_SUPPORT */

/*This structure is for all 802.11n card InterOptibilityTest action. Reset all Num every n second.  (Details see MLMEPeriodic) */
typedef struct _IOT_STRUC {
	BOOLEAN bRTSLongProtOn;
#ifdef CONFIG_STA_SUPPORT
	BOOLEAN bLastAtheros;
	BOOLEAN bCurrentAtheros;
	BOOLEAN bNowAtherosBurstOn;
	BOOLEAN bNextDisableRxBA;
	BOOLEAN bToggle;
#endif /* CONFIG_STA_SUPPORT */
} IOT_STRUC, *PIOT_STRUC;

/* This is the registry setting for 802.11n transmit setting.  Used in advanced page. */
typedef union _REG_TRANSMIT_SETTING {
#ifdef RT_BIG_ENDIAN
	struct {
		UINT32 rsv:13;
		UINT32 EXTCHA:2;
		UINT32 HTMODE:1;
		UINT32 TRANSNO:2;
		UINT32 STBC:1;	/*SPACE */
		UINT32 ShortGI:1;
		UINT32 BW:1;	/*channel bandwidth 20MHz or 40 MHz */
		UINT32 TxBF:1;	/* 3*3 */
		UINT32 ITxBfEn:1;
		UINT32 rsv0:9;
		/*UINT32  MCS:7;                 // MCS */
		/*UINT32  PhyMode:4; */
	} field;
#else
	struct {
		/*UINT32  PhyMode:4; */
		/*UINT32  MCS:7;                 // MCS */
		UINT32 rsv0:9;
		UINT32 ITxBfEn:1;
		UINT32 TxBF:1;
		UINT32 BW:1;	/*channel bandwidth 20MHz or 40 MHz */
		UINT32 ShortGI:1;
		UINT32 STBC:1;	/*SPACE */
		UINT32 TRANSNO:2;
		UINT32 HTMODE:1;
		UINT32 EXTCHA:2;
		UINT32 rsv:13;
	} field;
#endif
	UINT32 word;
} REG_TRANSMIT_SETTING, *PREG_TRANSMIT_SETTING;


typedef union _DESIRED_TRANSMIT_SETTING {
#ifdef RT_BIG_ENDIAN
	struct {
		USHORT rsv:3;
		USHORT FixedTxMode:2;	/* If MCS isn't AUTO, fix rate in CCK, OFDM or HT mode. */
		USHORT PhyMode:4;
		USHORT MCS:7;	/* MCS */
	} field;
#else
	struct {
		USHORT MCS:7;	/* MCS */
		USHORT PhyMode:4;
		USHORT FixedTxMode:2;	/* If MCS isn't AUTO, fix rate in CCK, OFDM or HT mode. */
		USHORT rsv:3;
	} field;
#endif
	USHORT word;
} DESIRED_TRANSMIT_SETTING, *PDESIRED_TRANSMIT_SETTING;


struct hw_setting{
	UCHAR prim_ch;
	UCHAR cent_ch;
	UCHAR bbp_bw;
	UCHAR rf_band;
	UCHAR cur_ch_pwr[3];
};


struct wifi_dev{
	NDIS_802_11_AUTHENTICATION_MODE AuthMode;
	NDIS_802_11_WEP_STATUS WepStatus;
	NDIS_802_11_WEP_STATUS GroupKeyWepStatus;
	WPA_MIX_PAIR_CIPHER WpaMixPairCipher;

	RT_PHY_INFO DesiredHtPhyInfo;
	DESIRED_TRANSMIT_SETTING DesiredTransmitSetting;	/* Desired transmit setting. this is for reading registry setting only. not useful. */
	BOOLEAN bAutoTxRateSwitch;

	BOOLEAN bWmmCapable;	/* 0:disable WMM, 1:enable WMM */

	HTTRANSMIT_SETTING HTPhyMode, MaxHTPhyMode, MinHTPhyMode;
};


#ifdef RTMP_MAC_USB
/***************************************************************************
  *	USB-based chip Beacon related data structures
  **************************************************************************/
#define BEACON_BITMAP_MASK		0xff
typedef struct _BEACON_SYNC_STRUCT_ {
	UCHAR BeaconBuf[HW_BEACON_MAX_NUM][HW_BEACON_OFFSET];
	UCHAR *BeaconTxWI[HW_BEACON_MAX_NUM];
	ULONG TimIELocationInBeacon[HW_BEACON_MAX_NUM];
	ULONG CapabilityInfoLocationInBeacon[HW_BEACON_MAX_NUM];
	BOOLEAN EnableBeacon;	/* trigger to enable beacon transmission. */
	UCHAR BeaconBitMap;	/* NOTE: If the MAX_MBSSID_NUM is larger than 8, this parameter need to change. */
	UCHAR DtimBitOn;	/* NOTE: If the MAX_MBSSID_NUM is larger than 8, this parameter need to change. */
} BEACON_SYNC_STRUCT;
#endif /* RTMP_MAC_USB */

/***************************************************************************
  *	Multiple SSID related data structures
  **************************************************************************/
#define WLAN_MAX_NUM_OF_TIM			((MAX_LEN_OF_MAC_TABLE >> 3) + 1)	/* /8 + 1 */
#define WLAN_CT_TIM_BCMC_OFFSET		0	/* unit: 32B */

/* clear bcmc TIM bit */
#define WLAN_MR_TIM_BCMC_CLEAR(apidx) \
	pAd->ApCfg.MBSSID[apidx].TimBitmaps[WLAN_CT_TIM_BCMC_OFFSET] &= ~NUM_BIT8[0];

/* set bcmc TIM bit */
#define WLAN_MR_TIM_BCMC_SET(apidx) \
	pAd->ApCfg.MBSSID[apidx].TimBitmaps[WLAN_CT_TIM_BCMC_OFFSET] |= NUM_BIT8[0];

/* clear a station PS TIM bit */
#define WLAN_MR_TIM_BIT_CLEAR(ad_p, apidx, wcid) \
	{	UCHAR tim_offset = wcid >> 3; \
		UCHAR bit_offset = wcid & 0x7; \
		ad_p->ApCfg.MBSSID[apidx].TimBitmaps[tim_offset] &= (~NUM_BIT8[bit_offset]); }

/* set a station PS TIM bit */
#define WLAN_MR_TIM_BIT_SET(ad_p, apidx, wcid) \
	{	UCHAR tim_offset = wcid >> 3; \
		UCHAR bit_offset = wcid & 0x7; \
		ad_p->ApCfg.MBSSID[apidx].TimBitmaps[tim_offset] |= NUM_BIT8[bit_offset]; }




/* configuration common to OPMODE_AP as well as OPMODE_STA */
struct common_config {
	BOOLEAN bCountryFlag;
	UCHAR CountryCode[3];
#ifdef EXT_BUILD_CHANNEL_LIST
	UCHAR Geography;
	UCHAR DfsType;
	PUCHAR pChDesp;
#endif				/* EXT_BUILD_CHANNEL_LIST */
	UCHAR CountryRegion;	/* Enum of country region, 0:FCC, 1:IC, 2:ETSI, 3:SPAIN, 4:France, 5:MKK, 6:MKK1, 7:Israel */
	UCHAR CountryRegionForABand;	/* Enum of country region for A band */
	UCHAR PhyMode;
	UCHAR cfg_wmode;
	UCHAR SavedPhyMode;
	USHORT Dsifs;		/* in units of usec */
	ULONG PacketFilter;	/* Packet filter for receiving */
	u8 RegulatoryClass[MAX_NUM_OF_REGULATORY_CLASS];

	CHAR Ssid[MAX_LEN_OF_SSID];	/* NOT NULL-terminated */
	UCHAR SsidLen;		/* the actual ssid length in used */
	UCHAR LastSsidLen;	/* the actual ssid length in used */
	CHAR LastSsid[MAX_LEN_OF_SSID];	/* NOT NULL-terminated */
	UCHAR LastBssid[MAC_ADDR_LEN];

	UCHAR Bssid[MAC_ADDR_LEN];
	USHORT BeaconPeriod;
	UCHAR Channel;
	UCHAR CentralChannel;	/* Central Channel when using 40MHz is indicating. not real channel. */

	UCHAR SupRate[MAX_LEN_OF_SUPPORTED_RATES];
	UCHAR SupRateLen;
	UCHAR ExtRate[MAX_LEN_OF_SUPPORTED_RATES];
	UCHAR ExtRateLen;
	UCHAR DesireRate[MAX_LEN_OF_SUPPORTED_RATES];	/* OID_802_11_DESIRED_RATES */
	UCHAR MaxDesiredRate;
	UCHAR ExpectedACKRate[MAX_LEN_OF_SUPPORTED_RATES];

	ULONG BasicRateBitmap;	/* backup basic ratebitmap */
	ULONG BasicRateBitmapOld;	/* backup basic ratebitmap */

	BOOLEAN bInServicePeriod;


	BOOLEAN bAPSDAC_BE;
	BOOLEAN bAPSDAC_BK;
	BOOLEAN bAPSDAC_VI;
	BOOLEAN bAPSDAC_VO;


	/* because TSPEC can modify the APSD flag, we need to keep the APSD flag
	   requested in association stage from the station;
	   we need to recover the APSD flag after the TSPEC is deleted. */
	BOOLEAN bACMAPSDBackup[4];	/* for delivery-enabled & trigger-enabled both */
	BOOLEAN bACMAPSDTr[4];	/* no use */
	UCHAR MaxSPLength;

	BOOLEAN bNeedSendTriggerFrame;
	BOOLEAN bAPSDForcePowerSave;	/* Force power save mode, should only use in APSD-STAUT */
	ULONG TriggerTimerCount;
	UCHAR BBPCurrentBW;	/* BW_10, BW_20, BW_40, BW_80 */
	REG_TRANSMIT_SETTING RegTransmitSetting;	/*registry transmit setting. this is for reading registry setting only. not useful. */
	UCHAR TxRate;		/* Same value to fill in TXD. TxRate is 6-bit */
	UCHAR MaxTxRate;	/* RATE_1, RATE_2, RATE_5_5, RATE_11 */
	UCHAR TxRateIndex;	/* Tx rate index in Rate Switch Table */
	UCHAR MinTxRate;	/* RATE_1, RATE_2, RATE_5_5, RATE_11 */
	UCHAR RtsRate;		/* RATE_xxx */
	HTTRANSMIT_SETTING MlmeTransmit;	/* MGMT frame PHY rate setting when operatin at Ht rate. */
	UCHAR MlmeRate;		/* RATE_xxx, used to send MLME frames */
	UCHAR BasicMlmeRate;	/* Default Rate for sending MLME frames */

	USHORT RtsThreshold;	/* in unit of BYTE */
	USHORT FragmentThreshold;	/* in unit of BYTE */

	UCHAR TxPower;		/* in unit of mW */
	ULONG TxPowerPercentage;	/* 0~100 % */
	ULONG TxPowerDefault;	/* keep for TxPowerPercentage */
	u8 PwrConstraint;

#ifdef DOT11_N_SUPPORT
	BACAP_STRUC BACapability;	/*   NO USE = 0XFF  ;  IMMED_BA =1  ;  DELAY_BA=0 */
	BACAP_STRUC REGBACapability;	/*   NO USE = 0XFF  ;  IMMED_BA =1  ;  DELAY_BA=0 */
#endif /* DOT11_N_SUPPORT */

#ifdef DOT11_VHT_AC
	BOOLEAN force_vht;
	UCHAR vht_bw;
	UCHAR vht_sgi_80;
	UCHAR vht_stbc;
	UCHAR vht_bw_signal;
	UCHAR vht_cent_ch;
	UCHAR vht_cent_ch2;
#endif /* DOT11_VHT_AC */

	IOT_STRUC IOTestParm;	/* 802.11n InterOpbility Test Parameter; */
	ULONG TxPreamble;	/* Rt802_11PreambleLong, Rt802_11PreambleShort, Rt802_11PreambleAuto */
	BOOLEAN bUseZeroToDisableFragment;	/* Microsoft use 0 as disable */
	ULONG UseBGProtection;	/* 0: auto, 1: always use, 2: always not use */
	BOOLEAN bUseShortSlotTime;	/* 0: disable, 1 - use short slot (9us) */
	BOOLEAN bEnableTxBurst;	/* 1: enble TX PACKET BURST (when BA is established or AP is not a legacy WMM AP), 0: disable TX PACKET BURST */
	BOOLEAN bAggregationCapable;	/* 1: enable TX aggregation when the peer supports it */
	BOOLEAN bPiggyBackCapable;	/* 1: enable TX piggy-back according MAC's version */
	BOOLEAN bIEEE80211H;	/* 1: enable IEEE802.11h spec. */
	UCHAR RDDurRegion; /* Region of radar detection */
	ULONG DisableOLBCDetect;	/* 0: enable OLBC detect; 1 disable OLBC detect */

#ifdef DOT11_N_SUPPORT
	BOOLEAN bRdg;
#endif /* DOT11_N_SUPPORT */
	BOOLEAN bWmmCapable;	/* 0:disable WMM, 1:enable WMM */
	QOS_CAPABILITY_PARM APQosCapability;	/* QOS capability of the current associated AP */
	EDCA_PARM APEdcaParm;	/* EDCA parameters of the current associated AP */
	QBSS_LOAD_PARM APQbssLoad;	/* QBSS load of the current associated AP */
	UCHAR AckPolicy[4];	/* ACK policy of the specified AC. see ACK_xxx */
#ifdef CONFIG_STA_SUPPORT
	BOOLEAN bDLSCapable;	/* 0:disable DLS, 1:enable DLS */
#endif /* CONFIG_STA_SUPPORT */
	/* a bitmap of BOOLEAN flags. each bit represent an operation status of a particular */
	/* BOOLEAN control, either ON or OFF. These flags should always be accessed via */
	/* OPSTATUS_TEST_FLAG(), OPSTATUS_SET_FLAG(), OP_STATUS_CLEAR_FLAG() macros. */
	/* see fOP_STATUS_xxx in RTMP_DEF.C for detail bit definition */
	ULONG OpStatusFlags;

	BOOLEAN NdisRadioStateOff;	/*For HCT 12.0, set this flag to TRUE instead of called MlmeRadioOff. */

#ifdef DFS_SUPPORT
	/* IEEE802.11H--DFS. */
	RADAR_DETECT_STRUCT RadarDetect;
#endif /* DFS_SUPPORT */
#ifdef CARRIER_DETECTION_SUPPORT
	CARRIER_DETECTION_STRUCT CarrierDetect;
#endif /* CARRIER_DETECTION_SUPPORT */

#ifdef DOT11_N_SUPPORT
	/* HT */
	RT_HT_CAPABILITY DesiredHtPhy;
	HT_CAPABILITY_IE HtCapability;
	ADD_HT_INFO_IE AddHTInfo;	/* Useful as AP. */
	/*This IE is used with channel switch announcement element when changing to a new 40MHz. */
	/*This IE is included in channel switch ammouncement frames 7.4.1.5, beacons, probe Rsp. */
	NEW_EXT_CHAN_IE NewExtChanOffset;	/*7.3.2.20A, 1 if extension channel is above the control channel, 3 if below, 0 if not present */

	EXT_CAP_INFO_ELEMENT ExtCapIE;	/* this is the extened capibility IE appreed in MGMT frames. Doesn't need to update once set in Init. */

#ifdef DOT11N_DRAFT3
	BOOLEAN bBssCoexEnable;
	/*
	   Following two paramters now only used for the initial scan operation. the AP only do
	   bandwidth fallback when BssCoexApCnt > BssCoexApCntThr
	   By default, the "BssCoexApCntThr" is set as 0 in "UserCfgInit()".
	 */
	UCHAR BssCoexApCntThr;
	UCHAR BssCoexApCnt;

	UCHAR Bss2040CoexistFlag;	/* bit 0: bBssCoexistTimerRunning, bit 1: NeedSyncAddHtInfo. */
	RALINK_TIMER_STRUCT Bss2040CoexistTimer;
	UCHAR Bss2040NeedFallBack; 	/* 1: Need Fall back to 20MHz */

	/*This IE is used for 20/40 BSS Coexistence. */
	BSS_2040_COEXIST_IE BSS2040CoexistInfo;

	USHORT Dot11OBssScanPassiveDwell;	/* Unit : TU. 5~1000 */
	USHORT Dot11OBssScanActiveDwell;	/* Unit : TU. 10~1000 */
	USHORT Dot11BssWidthTriggerScanInt;	/* Unit : Second */
	USHORT Dot11OBssScanPassiveTotalPerChannel;	/* Unit : TU. 200~10000 */
	USHORT Dot11OBssScanActiveTotalPerChannel;	/* Unit : TU. 20~10000 */
	USHORT Dot11BssWidthChanTranDelayFactor;
	USHORT Dot11OBssScanActivityThre;	/* Unit : percentage */

	ULONG Dot11BssWidthChanTranDelay;	/* multiple of (Dot11BssWidthTriggerScanInt * Dot11BssWidthChanTranDelayFactor) */
	ULONG CountDownCtr;	/* CountDown Counter from (Dot11BssWidthTriggerScanInt * Dot11BssWidthChanTranDelayFactor) */

	BSS_2040_COEXIST_IE LastBSSCoexist2040;
	BSS_2040_COEXIST_IE BSSCoexist2040;
	TRIGGER_EVENT_TAB TriggerEventTab;
	UCHAR ChannelListIdx;

	BOOLEAN bOverlapScanning;
	BOOLEAN bBssCoexNotify;
#endif /* DOT11N_DRAFT3 */

	BOOLEAN bHTProtect;
	BOOLEAN bMIMOPSEnable;
	BOOLEAN bBADecline;
	BOOLEAN bDisableReordering;
	BOOLEAN bForty_Mhz_Intolerant;
	BOOLEAN bExtChannelSwitchAnnouncement;
	BOOLEAN bRcvBSSWidthTriggerEvents;
	ULONG LastRcvBSSWidthTriggerEventsTime;

	UCHAR TxBASize;

	BOOLEAN bRalinkBurstMode;
	UINT32 RestoreBurstMode;
#endif /* DOT11_N_SUPPORT */

#ifdef DOT11_VHT_AC
	UINT32 cfg_vht;
	VHT_CAP_INFO vht_info;
#endif /* DOT11_VHT_AC */

	BOOLEAN bWiFiTest;	/* Enable this parameter for WiFi test */

	/* Tx & Rx Stream number selection */
	UCHAR TxStream;
	UCHAR RxStream;

	/* transmit phy mode, trasmit rate for Multicast. */
#ifdef MCAST_RATE_SPECIFIC
	UCHAR McastTransmitMcs;
	UCHAR McastTransmitPhyMode;
#endif /* MCAST_RATE_SPECIFIC */

	BOOLEAN bHardwareRadio;	/* Hardware controlled Radio enabled */

#ifdef RTMP_MAC_USB
	BOOLEAN bMultipleIRP;	/* Multiple Bulk IN flag */
	UCHAR NumOfBulkInIRP;	/* if bMultipleIRP == TRUE, NumOfBulkInIRP will be 4 otherwise be 1 */
	RT_HT_CAPABILITY SupportedHtPhy;
	ULONG MaxPktOneTxBulk;
	UCHAR TxBulkFactor;
	UCHAR RxBulkFactor;

	BOOLEAN IsUpdateBeacon;
	BEACON_SYNC_STRUCT *pBeaconSync;
	RALINK_TIMER_STRUCT BeaconUpdateTimer;
	UINT32 BeaconAdjust;
	UINT32 BeaconFactor;
	UINT32 BeaconRemain;
#endif				/* RTMP_MAC_USB */



	NDIS_SPIN_LOCK MeasureReqTabLock;
	PMEASURE_REQ_TAB pMeasureReqTab;

	NDIS_SPIN_LOCK TpcReqTabLock;
	PTPC_REQ_TAB pTpcReqTab;

	/* transmit phy mode, trasmit rate for Multicast. */
#ifdef MCAST_RATE_SPECIFIC
	HTTRANSMIT_SETTING MCastPhyMode;
#endif /* MCAST_RATE_SPECIFIC */

#ifdef SINGLE_SKU
	UINT16 DefineMaxTxPwr;
	BOOLEAN bSKUMode;
	UINT16 AntGain;
	UINT16 BandedgeDelta;
	UINT16 ModuleTxpower;
#endif /* SINGLE_SKU */



	BOOLEAN HT_DisallowTKIP;	/* Restrict the encryption type in 11n HT mode */

	BOOLEAN HT_Disable;	/* 1: disable HT function; 0: enable HT function */


#ifdef PRE_ANT_SWITCH
	BOOLEAN PreAntSwitch;	/* Preamble Antenna Switch */
	SHORT PreAntSwitchRSSI;	/* Preamble Antenna Switch RSSI threshold */
	SHORT PreAntSwitchTimeout; /* Preamble Antenna Switch timeout in seconds */
#endif /* PRE_ANT_SWITCH */

#ifdef CFO_TRACK
	SHORT	CFOTrack;	/* CFO Tracking. 0=>use default, 1=>track, 2-7=> track 8-n times, 8=>done tracking */
#endif /* CFO_TRACK */

#ifdef NEW_RATE_ADAPT_SUPPORT
	USHORT	lowTrafficThrd;		/* Threshold for reverting to default MCS when traffic is low */
	BOOLEAN	TrainUpRule;		/* QuickDRS train up criterion: 0=>Throughput, 1=>PER, 2=> Throughput & PER */
	SHORT	TrainUpRuleRSSI;	/* If TrainUpRule=2 then use Hybrid rule when RSSI < TrainUpRuleRSSI */
	USHORT	TrainUpLowThrd;		/* QuickDRS Hybrid train up low threshold */
	USHORT	TrainUpHighThrd;	/* QuickDRS Hybrid train up high threshold */
#endif /* NEW_RATE_ADAPT_SUPPORT */

#ifdef STREAM_MODE_SUPPORT
#define		STREAM_MODE_STA_NUM		4

	UCHAR	StreamMode; /* 0=disabled, 1=enable for 1SS, 2=enable for 2SS, 3=enable for 1,2SS */
	UCHAR	StreamModeMac[STREAM_MODE_STA_NUM][MAC_ADDR_LEN];
	UINT16	StreamModeMCS;	/* Bit map for enabling Stream Mode based on MCS */
#endif /* STREAM_MODE_SUPPORT */

#ifdef DOT11_N_SUPPORT
#ifdef TXBF_SUPPORT
	ULONG ITxBfTimeout;
	ULONG ETxBfTimeout;
	ULONG	ETxBfEnCond;		/* Enable sending of sounding and beamforming */
	BOOLEAN	ETxBfNoncompress;	/* Force non-compressed Sounding Response */
	BOOLEAN	ETxBfIncapable;		/* Report Incapable of BF in TX BF Capabilities */
#endif /* TXBF_SUPPORT */
#endif /* DOT11_N_SUPPORT */

#ifdef DBG_CTRL_SUPPORT
	ULONG DebugFlags;	/* Temporary debug flags */
#endif /* DBG_CTRL_SUPPORT */



	//brian
	UINT32 nLoopBack_Count;
	UINT32 nWIFI_Info_Idx;

#ifdef CONFIG_WIFI_TEST
	ULONG WiFiTestFlags;
#endif
};

#ifdef DBG_CTRL_SUPPORT
/* DebugFlag definitions */
#define DBF_NO_BF_AWARE_RA		0x0001	/* Revert to older Rate Adaptation that is not BF aware */
#define DBF_SHOW_BF_STATS		0x0002	/* Display BF statistics in AP "iwpriv stat" display */
#define DBF_NO_TXBF_3SS			0x0004	/* Disable TXBF for MCS > 20 */
#define DBF_UNUSED0008			0x0008	/* Unused */
#define DBF_DBQ_RA_LOG			0x0010	/* Log RA information in DBQ */
#define DBF_INIT_MCS_MARGIN		0x0020	/* Use 6 dB margin when selecting initial MCS */
#define DBF_INIT_MCS_DIS1		0x0040	/* Disable highest MCSs when selecting initial MCS */
#define DBF_FORCE_QUICK_DRS		0x0080	/* Force Quick DRS even if rate didn't change */
#define DBF_FORCE_SGI			0x0100	/* Force Short GI */
#define DBF_DBQ_NO_BCN			0x0200	/* Disable logging of RX Beacon frames */
#define DBF_LOG_VCO_CAL			0x0400	/* Log VCO cal */
#define DBF_DISABLE_CAL			0x0800	/* Disable Divider Calibration at channel change */
#ifdef INCLUDE_DEBUG_QUEUE
#define DBF_DBQ_TXFIFO			0x1000	/* Enable logging of TX information from FIFO */
#define DBF_DBQ_TXFRAME			0x2000	/* Enable logging of Frames queued for TX */
#define DBF_DBQ_RXWI_FULL		0x4000	/* Enable logging of full RXWI */
#define DBF_DBQ_RXWI			0x8000	/* Enable logging of partial RXWI */
#endif /* INCLUDE_DEBUG_QUEUE */

#define DBF_SHOW_RA_LOG			0x010000	/* Display concise Rate Adaptation information */
#define DBF_SHOW_ZERO_RA_LOG	0x020000	/* Include RA Log entries when TxCount is 0 */
#define DBF_FORCE_20MHZ			0x040000	/* Force 20 MHz TX */
#define DBF_FORCE_40MHZ 		0x080000	/* Force 40 MHz Tx */
#define DBF_DISABLE_CCK			0x100000	/* Disable CCK */
#define DBF_UNUSED200000		0x200000	/* Unused */
#define DBF_ENABLE_HT_DUP		0x400000	/* Allow HT Duplicate mode in TX rate table */
#define DBF_ENABLE_CCK_5G		0x800000	/* Enable CCK rates in 5G band */
#define DBF_UNUSED0100000		0x0100000	/* Unused */
#define DBF_ENABLE_20MHZ_MCS8	0x02000000	/* Substitute 20MHz MCS8 for 40MHz MCS8 */
#define DBF_DISABLE_20MHZ_MCS0	0x04000000	/* Disable substitution of 20MHz MCS0 for 40MHz MCS32 */
#define DBF_DISABLE_20MHZ_MCS1	0x08000000	/* Disable substitution of 20MHz MCS1 for 40MHz MCS0 */
#endif /* DBG_CTRL_SUPPORT */

#ifdef CONFIG_STA_SUPPORT

#ifdef CREDENTIAL_STORE
typedef struct _STA_CONNECT_INFO {
	BOOLEAN Changeable;
	BOOLEAN IEEE8021X;
	CHAR Ssid[MAX_LEN_OF_SSID]; // NOT NULL-terminated
	UCHAR SsidLen; // the actual ssid length in used
	NDIS_802_11_AUTHENTICATION_MODE AuthMode; // This should match to whatever microsoft defined
	NDIS_802_11_WEP_STATUS WepStatus;
	UCHAR DefaultKeyId;
	UCHAR PMK[LEN_PMK]; // WPA PSK mode PMK
	UCHAR WpaPassPhrase[64]; // WPA PSK pass phrase
	UINT WpaPassPhraseLen; // the length of WPA PSK pass phrase
	u8 WpaState;
	CIPHER_KEY SharedKey[1][4]; // STA always use SharedKey[BSS0][0..3]
	NDIS_SPIN_LOCK Lock;
} STA_CONNECT_INFO, *P_STA_CONNECT_INFO;
#endif /* CREDENTIAL_STORE */


/* Modified by Wu Xi-Kun 4/21/2006 */
/* STA configuration and status */
typedef struct _STA_ADMIN_CONFIG {
	struct wifi_dev wdev;

	/*
		GROUP 1 -
		User configuration loaded from Registry, E2PROM or OID_xxx. These settings describe
		the user intended configuration, but not necessary fully equal to the final
		settings in ACTIVE BSS after negotiation/compromize with the BSS holder (either
		AP or IBSS holder).
		Once initialized, user configuration can only be changed via OID_xxx
	*/
	UCHAR BssType;		/* BSS_INFRA or BSS_ADHOC */

#ifdef MONITOR_FLAG_11N_SNIFFER_SUPPORT
#define MONITOR_FLAG_11N_SNIFFER		0x01
	UCHAR BssMonitorFlag;	/* Specific flag for monitor */
#endif				/* MONITOR_FLAG_11N_SNIFFER_SUPPORT */

	USHORT AtimWin;		/* used when starting a new IBSS */

	/*
		GROUP 2 -
		User configuration loaded from Registry, E2PROM or OID_xxx. These settings describe
		the user intended configuration, and should be always applied to the final
		settings in ACTIVE BSS without compromising with the BSS holder.
		Once initialized, user configuration can only be changed via OID_xxx
	*/
	UCHAR RssiTrigger;
	UCHAR RssiTriggerMode;	/* RSSI_TRIGGERED_UPON_BELOW_THRESHOLD or RSSI_TRIGGERED_UPON_EXCCEED_THRESHOLD */
	USHORT DefaultListenCount;	/* default listen count; */
	ULONG WindowsPowerMode;	/* Power mode for AC power */
	ULONG WindowsBatteryPowerMode;	/* Power mode for battery if exists */
	BOOLEAN bWindowsACCAMEnable;	/* Enable CAM power mode when AC on */
	BOOLEAN bAutoReconnect;	/* Set to TRUE when setting OID_802_11_SSID with no matching BSSID */
	ULONG WindowsPowerProfile;	/* Windows power profile, for NDIS5.1 PnP */

	BOOLEAN	 FlgPsmCanNotSleep; /* TRUE: can not switch ASIC to sleep */
	/* MIB:ieee802dot11.dot11smt(1).dot11StationConfigTable(1) */
	USHORT Psm;		/* power management mode   (PWR_ACTIVE|PWR_SAVE) */
	USHORT DisassocReason;
	UCHAR DisassocSta[MAC_ADDR_LEN];
	USHORT DeauthReason;
	UCHAR DeauthSta[MAC_ADDR_LEN];
	USHORT AuthFailReason;
	UCHAR AuthFailSta[MAC_ADDR_LEN];

	NDIS_802_11_PRIVACY_FILTER PrivacyFilter;	/* PrivacyFilter enum for 802.1X */
	NDIS_802_11_AUTHENTICATION_MODE AuthMode;	/* This should match to whatever microsoft defined */
	NDIS_802_11_WEP_STATUS WepStatus;

	/* Add to support different cipher suite for WPA2/WPA mode */
	NDIS_802_11_ENCRYPTION_STATUS GroupCipher;	/* Multicast cipher suite */
	NDIS_802_11_ENCRYPTION_STATUS PairCipher;	/* Unicast cipher suite */
	BOOLEAN bMixCipher;	/* Indicate current Pair & Group use different cipher suites */
	USHORT RsnCapability;

	NDIS_802_11_WEP_STATUS GroupKeyWepStatus;

	UCHAR WpaPassPhrase[64];	/* WPA PSK pass phrase */
	UINT WpaPassPhraseLen;	/* the length of WPA PSK pass phrase */
	UCHAR PMK[LEN_PMK];	/* WPA PSK mode PMK */
	UCHAR PTK[LEN_PTK];	/* WPA PSK mode PTK */
	UCHAR GMK[LEN_GMK];	/* WPA PSK mode GMK */
	UCHAR GTK[MAX_LEN_GTK];	/* GTK from authenticator */
	UCHAR GNonce[32];	/* GNonce for WPA2PSK from authenticator */
	CIPHER_KEY TxGTK;
	BSSID_INFO SavedPMK[PMKID_NO];
	UINT SavedPMKNum;	/* Saved PMKID number */

	UCHAR DefaultKeyId;


	/* WPA 802.1x port control, WPA_802_1X_PORT_SECURED, WPA_802_1X_PORT_NOT_SECURED */
	UCHAR PortSecured;

	/* For WPA countermeasures */
	ULONG LastMicErrorTime;	/* record last MIC error time */
	ULONG MicErrCnt;	/* Should be 0, 1, 2, then reset to zero (after disassoiciation). */
	BOOLEAN bBlockAssoc;	/* Block associate attempt for 60 seconds after counter measure occurred. */
	/* For WPA-PSK supplicant state */
	u8 WpaState;		/* Default is SS_NOTUSE and handled by microsoft 802.1x */
	UCHAR ReplayCounter[8];
	UCHAR ANonce[32];	/* ANonce for WPA-PSK from aurhenticator */
	UCHAR SNonce[32];	/* SNonce for WPA-PSK */

	UCHAR LastSNR0;		/* last received BEACON's SNR */
	UCHAR LastSNR1;		/* last received BEACON's SNR for 2nd  antenna */
#ifdef DOT11N_SS3_SUPPORT
	UCHAR LastSNR2;		/* last received BEACON's SNR for 3nd  antenna */
	INT32 BF_SNR[3];	/* Last RXWI BF SNR. Units=0.25 dB */
#endif /* DOT11N_SS3_SUPPORT */
	RSSI_SAMPLE RssiSample;
	ULONG NumOfAvgRssiSample;

	ULONG LastBeaconRxTime;	/* OS's timestamp of the last BEACON RX time */
	ULONG Last11bBeaconRxTime;	/* OS's timestamp of the last 11B BEACON RX time */
	ULONG Last11gBeaconRxTime;	/* OS's timestamp of the last 11G BEACON RX time */
	ULONG Last20NBeaconRxTime;	/* OS's timestamp of the last 20MHz N BEACON RX time */

	ULONG LastScanTime;	/* Record last scan time for issue BSSID_SCAN_LIST */
	BOOLEAN bNotFirstScan;	/* Sam add for ADHOC flag to do first scan when do initialization */
	BOOLEAN bSwRadio;	/* Software controlled Radio On/Off, TRUE: On */
	BOOLEAN bHwRadio;	/* Hardware controlled Radio On/Off, TRUE: On */
	BOOLEAN bRadio;		/* Radio state, And of Sw & Hw radio state */
	BOOLEAN bHardwareRadio;	/* Hardware controlled Radio enabled */
	BOOLEAN bShowHiddenSSID;	/* Show all known SSID in SSID list get operation */

	/* New for WPA, windows want us to to keep association information and */
	/* Fixed IEs from last association response */
	NDIS_802_11_ASSOCIATION_INFORMATION AssocInfo;
	USHORT ReqVarIELen;	/* Length of next VIE include EID & Length */
	UCHAR ReqVarIEs[MAX_VIE_LEN];	/* The content saved here should be little-endian format. */
	USHORT ResVarIELen;	/* Length of next VIE include EID & Length */
	UCHAR ResVarIEs[MAX_VIE_LEN];

	UCHAR RSNIE_Len;
	UCHAR RSN_IE[MAX_LEN_OF_RSNIE];	/* The content saved here should be little-endian format. */

	ULONG CLBusyBytes;	/* Save the total bytes received durning channel load scan time */
	USHORT RPIDensity[8];	/* Array for RPI density collection */

	UCHAR RMReqCnt;		/* Number of measurement request saved. */
	UCHAR CurrentRMReqIdx;	/* Number of measurement request saved. */
	BOOLEAN ParallelReq;	/* Parallel measurement, only one request performed, */
	/* It must be the same channel with maximum duration */
	USHORT ParallelDuration;	/* Maximum duration for parallel measurement */
	UCHAR ParallelChannel;	/* Only one channel with parallel measurement */
	USHORT IAPPToken;	/* IAPP dialog token */
	/* Hack for channel load and noise histogram parameters */
	UCHAR NHFactor;		/* Parameter for Noise histogram */
	UCHAR CLFactor;		/* Parameter for channel load */

	RALINK_TIMER_STRUCT StaQuickResponeForRateUpTimer;
	BOOLEAN StaQuickResponeForRateUpTimerRunning;

	UCHAR DtimCount;	/* 0.. DtimPeriod-1 */
	UCHAR DtimPeriod;	/* default = 3 */

#ifdef QOS_DLS_SUPPORT
	RT_802_11_DLS DLSEntry[MAX_NUM_OF_DLS_ENTRY];
	UCHAR DlsReplayCounter[8];
#endif				/* QOS_DLS_SUPPORT */


	RALINK_TIMER_STRUCT WpaDisassocAndBlockAssocTimer;
	/* Fast Roaming */
	BOOLEAN bAutoRoaming;	/* 0:disable auto roaming by RSSI, 1:enable auto roaming by RSSI */
	CHAR dBmToRoam;		/* the condition to roam when receiving Rssi less than this value. It's negative value. */

#ifdef WPA_SUPPLICANT_SUPPORT
	BOOLEAN IEEE8021X;
	BOOLEAN IEEE8021x_required_keys;
	CIPHER_KEY DesireSharedKey[4];	/* Record user desired WEP keys */
	UCHAR DesireSharedKeyId;

	/* 0x00: driver ignores wpa_supplicant */
	/* 0x01: wpa_supplicant initiates scanning and AP selection */
	/* 0x02: driver takes care of scanning, AP selection, and IEEE 802.11 association parameters */
	/* 0x80: wpa_supplicant trigger driver to do WPS */
	UCHAR WpaSupplicantUP;
	UCHAR WpaSupplicantScanCount;
	BOOLEAN bRSN_IE_FromWpaSupplicant;
	BOOLEAN bLostAp;
	UCHAR *pWpsProbeReqIe;
	UINT WpsProbeReqIeLen;
	UCHAR *pWpaAssocIe;
	UINT WpaAssocIeLen;
#endif /* WPA_SUPPLICANT_SUPPORT */

	CHAR dev_name[16];
	USHORT OriDevType;

	BOOLEAN bTGnWifiTest;
	BOOLEAN bSkipAutoScanConn;

	HTTRANSMIT_SETTING HTPhyMode, MaxHTPhyMode, MinHTPhyMode;	/* For transmit phy setting in TXWI. */
	DESIRED_TRANSMIT_SETTING DesiredTransmitSetting;
	RT_PHY_INFO DesiredHtPhyInfo;
	BOOLEAN bAutoTxRateSwitch;


#ifdef EXT_BUILD_CHANNEL_LIST
	UCHAR IEEE80211dClientMode;
	UCHAR StaOriCountryCode[3];
	UCHAR StaOriGeography;
#endif /* EXT_BUILD_CHANNEL_LIST */





	BOOLEAN bAutoConnectByBssid;
	ULONG BeaconLostTime;	/* seconds */
	BOOLEAN bForceTxBurst;	/* 1: force enble TX PACKET BURST, 0: disable */
#ifdef XLINK_SUPPORT
	BOOLEAN PSPXlink;	/* 0: Disable. 1: Enable */
#endif /* XLINK_SUPPORT */
	BOOLEAN bAutoConnectIfNoSSID;
#ifdef DOT11_N_SUPPORT
#ifdef DOT11N_DRAFT3
	UCHAR RegClass;		/*IE_SUPP_REG_CLASS: 2009 PF#3: For 20/40 Intolerant Channel Report */
#endif /* DOT11N_DRAFT3 */
	BOOLEAN bAdhocN;
#endif /* DOT11_N_SUPPORT */
	BOOLEAN bAdhocCreator;	/*TRUE indicates divice is Creator. */


	/*
	   Enhancement Scanning Mechanism
	   To decrease the possibility of ping loss
	 */
	BOOLEAN bImprovedScan;
	BOOLEAN BssNr;
	UCHAR ScanChannelCnt;	/* 0 at the beginning of scan, stop at 7 */
	UCHAR LastScanChannel;
	/************************************/

	BOOLEAN bFastConnect;

/*connectinfo  for tmp store connect info from UI*/
	BOOLEAN Connectinfoflag;
	UCHAR   ConnectinfoBssid[MAC_ADDR_LEN];
	UCHAR   ConnectinfoChannel;
	UCHAR   ConnectinfoSsidLen;
	CHAR    ConnectinfoSsid[MAX_LEN_OF_SSID];
	UCHAR ConnectinfoBssType;




	/* UAPSD information: such as enable or disable, do not remove */
	UAPSD_INFO UapsdInfo;

#ifdef WFD_SUPPORT
	RT_WFD_CONFIG WfdCfg;
#endif /* WFD_SUPPORT */

#ifdef RT3290
	// TODO: shiang, check the purpose of following parameters
	ULONG AntS;
	ULONG AntSAuxDelta;
	ULONG AntSRssiFactor;
	ULONG AntSPERFactor;
	CHAR Rssi[2];
#endif /* RT3290 */
} STA_ADMIN_CONFIG, *PSTA_ADMIN_CONFIG;


/*
	This data structure keep the current active BSS/IBSS's configuration that
	this STA had agreed upon joining the network. Which means these parameters
	are usually decided by the BSS/IBSS creator instead of user configuration.
	Data in this data structurre is valid only when either ADHOC_ON()/INFRA_ON()
	is TRUE. Normally, after SCAN or failed roaming attempts, we need to
	recover back to the current active settings
*/
typedef struct _STA_ACTIVE_CONFIG {
	USHORT Aid;
	USHORT AtimWin;		/* in kusec; IBSS parameter set element */
	USHORT CapabilityInfo;
	EXT_CAP_INFO_ELEMENT ExtCapInfo;
	USHORT CfpMaxDuration;
	USHORT CfpPeriod;

	/* Copy supported rate from desired AP's beacon. We are trying to match */
	/* AP's supported and extended rate settings. */
	UCHAR SupRate[MAX_LEN_OF_SUPPORTED_RATES];
	UCHAR ExtRate[MAX_LEN_OF_SUPPORTED_RATES];
	UCHAR SupRateLen;
	UCHAR ExtRateLen;
	/* Copy supported ht from desired AP's beacon. We are trying to match */
	RT_PHY_INFO SupportedPhyInfo;
	RT_HT_CAPABILITY SupportedHtPhy;
	RT_VHT_CAP	SupVhtCap;
} STA_ACTIVE_CONFIG, *PSTA_ACTIVE_CONFIG;



#endif /* CONFIG_STA_SUPPORT */



/* Values of LastSecTxRateChangeAction */
#define RATE_NO_CHANGE	0		/* No change in rate */
#define RATE_UP			1		/* Trying higher rate or same rate with different BF */
#define RATE_DOWN		2		/* Trying lower rate */



#ifdef WFA_VHT_PF
#ifdef IP_ASSEMBLY
struct ip_frag_q{
	QUEUE_HEADER ip_queue[4];
	INT32 ip_id[4];
	INT32 ip_id_FragSize[4];
	ULONG ip_pkt_jiffies[4];
};
#endif /* IP_ASSEMBLY */
#endif /* WFA_VHT_PF */

typedef struct _MAC_TABLE_ENTRY {
	/*
	   0:Invalid,
	   Bit 0: AsCli, Bit 1: AsWds, Bit 2: AsAPCLI,
	   Bit 3: AsMesh, Bit 4: AsDls, Bit 5: AsTDls
	 */
	UINT32 EntryType;

	BOOLEAN isCached;
	BOOLEAN bIAmBadAtheros;	/* Flag if this is Atheros chip that has IOT problem.  We need to turn on RTS/CTS protection. */
	UCHAR wcid;

	/* WPA/WPA2 4-way database */
	UCHAR EnqueueEapolStartTimerRunning;	/* Enqueue EAPoL-Start for triggering EAP SM */
	RALINK_TIMER_STRUCT EnqueueStartForPSKTimer;	/* A timer which enqueue EAPoL-Start for triggering PSK SM */

	/*jan for wpa */
	/* record which entry revoke MIC Failure , if it leaves the BSS itself, AP won't update aMICFailTime MIB */
	UCHAR CMTimerRunning;
	UCHAR apidx;		/* MBSS number */
	UCHAR RSNIE_Len;
	UCHAR RSN_IE[MAX_LEN_OF_RSNIE];
	UCHAR ANonce[LEN_KEY_DESC_NONCE];
	UCHAR SNonce[LEN_KEY_DESC_NONCE];
	UCHAR R_Counter[LEN_KEY_DESC_REPLAY];
	UCHAR PTK[64];
	UCHAR ReTryCounter;
	RALINK_TIMER_STRUCT RetryTimer;
#ifdef TXBF_SUPPORT
	RALINK_TIMER_STRUCT eTxBfProbeTimer;
#endif /* TXBF_SUPPORT */
	NDIS_802_11_AUTHENTICATION_MODE AuthMode;	/* This should match to whatever microsoft defined */
	NDIS_802_11_WEP_STATUS WepStatus;
	NDIS_802_11_WEP_STATUS GroupKeyWepStatus;
	u8 WpaState;
	u8 GTKState;
	USHORT PortSecured;
	NDIS_802_11_PRIVACY_FILTER PrivacyFilter;	/* PrivacyFilter enum for 802.1X */
	CIPHER_KEY PairwiseKey;
	void *pAd;
	INT PMKID_CacheIdx;
	UCHAR PMKID[LEN_PMKID];
	UCHAR NegotiatedAKM[LEN_OUI_SUITE];	/* It indicate the negotiated AKM suite */


	UCHAR Addr[MAC_ADDR_LEN];
	UCHAR HdrAddr1[MAC_ADDR_LEN];
	UCHAR HdrAddr2[MAC_ADDR_LEN];
	UCHAR HdrAddr3[MAC_ADDR_LEN];
	UCHAR PsMode;
	UCHAR FlgPsModeIsWakeForAWhile; /* wake up for a while until a condition */
	UCHAR VirtualTimeout; /* peer power save virtual timeout */
	SST Sst;
	AUTH_STATE AuthState;	/* for SHARED KEY authentication state machine used only */
	BOOLEAN IsReassocSta;	/* Indicate whether this is a reassociation procedure */
	USHORT Aid;
	USHORT CapabilityInfo;
	UCHAR LastRssi;
	ULONG NoDataIdleCount;
	UINT16 StationKeepAliveCount;	/* unit: second */
	ULONG PsQIdleCount;
	QUEUE_HEADER PsQueue;

	UINT32 StaConnectTime;	/* the live time of this station since associated with AP */
	UINT32 StaIdleTimeout;	/* idle timeout per entry */

#ifdef UAPSD_SUPPORT
	/* these UAPSD states are used on the fly */
	/* 0:AC_BK, 1:AC_BE, 2:AC_VI, 3:AC_VO */
	BOOLEAN bAPSDCapablePerAC[4];	/* for trigger-enabled */
	BOOLEAN bAPSDDeliverEnabledPerAC[4];	/* for delivery-enabled */


	UCHAR MaxSPLength;

	BOOLEAN bAPSDAllAC;	/* 1: all AC are delivery-enabled U-APSD */

	QUEUE_HEADER UAPSDQueue[WMM_NUM_OF_AC];	/* queue for each U-APSD */
	USHORT UAPSDQIdleCount;	/* U-APSD queue timeout */

	PQUEUE_ENTRY pUAPSDEOSPFrame;	/* the last U-APSD frame */
	USHORT UAPSDTxNum;	/* total U-APSD frame number */
	BOOLEAN bAPSDFlagEOSPOK;	/* 1: EOSP frame is tx by ASIC */
	BOOLEAN bAPSDFlagSPStart;	/* 1: SP is started */

	/* need to use unsigned long, because time parameters in OS is defined as
	   unsigned long */
	unsigned long UAPSDTimeStampLast;	/* unit: 1000000/OS_HZ */
	BOOLEAN bAPSDFlagSpRoughUse;	/* 1: use rough SP (default: accurate) */

	/* we will set the flag when PS-poll frame is received and
	   clear it when statistics handle.
	   if the flag is set when PS-poll frame is received then calling
	   statistics handler to clear it. */
	BOOLEAN bAPSDFlagLegacySent;	/* 1: Legacy PS sent but
					   yet statistics handle */

#ifdef RTMP_MAC_USB
	UINT32 UAPSDTagOffset[WMM_NUM_OF_AC];
#endif /* RTMP_MAC_USB */
#endif /* UAPSD_SUPPORT */

#ifdef DOT11_N_SUPPORT
	BOOLEAN bSendBAR;
	USHORT NoBADataCountDown;

	UINT32 CachedBuf[16];	/* UINT (4 bytes) for alignment */

#ifdef TXBF_SUPPORT
	COUNTER_TXBF TxBFCounters;		/* TxBF Statistics */
	UINT LastETxCount;		/* Used to compute %BF statistics */
	UINT LastITxCount;
	UINT LastTxCount;
#endif /* TXBF_SUPPORT */
#endif /* DOT11_N_SUPPORT */

#ifdef STREAM_MODE_SUPPORT
	UINT32 StreamModeMACReg;	/* MAC reg used to control stream mode for this client. 0=>No stream mode */
#endif // STREAM_MODE_SUPPORT //

	UINT FIFOCount;
	UINT DebugFIFOCount;
	UINT DebugTxCount;
	BOOLEAN bDlsInit;

/*==================================================== */
/* WDS entry needs these */
/* If ValidAsWDS==TRUE, MatchWDSTabIdx is the index in WdsTab.MacTab */
	UINT MatchWDSTabIdx;
	UCHAR MaxSupportedRate;
	UCHAR CurrTxRate;
	UCHAR CurrTxRateIndex;
	UCHAR lastRateIdx;
	UCHAR *pTable;	/* Pointer to this entry's Tx Rate Table */

#ifdef NEW_RATE_ADAPT_SUPPORT
	UCHAR lowTrafficCount;
	UCHAR fewPktsCnt;
	BOOLEAN perThrdAdj;
	UCHAR mcsGroup;/* the mcs group to be tried */
#endif /* NEW_RATE_ADAPT_SUPPORT */
	enum RATE_ADAPT_ALG rateAlg;

#ifdef MFB_SUPPORT
	UCHAR lastLegalMfb;	/* last legal mfb which is used to set rate */
	BOOLEAN isMfbChanged;	/* purpose: true when mfb has changed but the new mfb is not adopted for Tx */
	struct _RTMP_RA_LEGACY_TB *LegalMfbRS;
	BOOLEAN fLastChangeAccordingMfb;
	NDIS_SPIN_LOCK fLastChangeAccordingMfbLock;
/* Tx MRQ */
	BOOLEAN toTxMrq;
	UCHAR msiToTx, mrqCnt;	/*mrqCnt is used to count down the inverted-BF mrq to be sent */
/* Rx mfb */
	UCHAR pendingMfsi;
/* Tx MFB */
	BOOLEAN toTxMfb;
	UCHAR	mfbToTx;
	UCHAR	mfb0, mfb1;
#endif	/* MFB_SUPPORT */
#ifdef TXBF_SUPPORT
	UCHAR			TxSndgType;
	NDIS_SPIN_LOCK	TxSndgLock;

/* ETxBF */
	UCHAR		bfState;
	UCHAR		sndgMcs;
	UCHAR		sndgBW;
	INT			sndg0Snr0, sndg0Snr1, sndg0Snr2;
	UCHAR		sndg0Mcs;
#ifdef ETXBF_EN_COND3_SUPPORT
	UCHAR		bestMethod;
	UCHAR		sndgRateIdx;
	UCHAR		bf0Mcs, sndg0RateIdx, bf0RateIdx;
	UCHAR		sndg1Mcs, bf1Mcs, sndg1RateIdx, bf1RateIdx;
	INT			sndg1Snr0, sndg1Snr1, sndg1Snr2;
#endif /* ETXBF_EN_COND3_SUPPORT */
	UCHAR		noSndgCnt;
	UCHAR		eTxBfEnCond;
	UCHAR		noSndgCntThrd, ndpSndgStreams;
	UCHAR		iTxBfEn;

	BOOLEAN		phyETxBf;			/* True=>Set ETxBF bit in PHY rate */
	BOOLEAN		phyITxBf;			/* True=>Set ITxBF bit in PHY rate */
	UCHAR		lastNonBfRate;		/* Last good non-BF rate */
	BOOLEAN		lastRatePhyTxBf;	/* For Quick Check. True if last rate was BF */
	USHORT      BfTxQuality[MAX_TX_RATE_INDEX + 1];	/* Beamformed TX Quality */
#endif /* TXBF_SUPPORT */

	/* to record the each TX rate's quality. 0 is best, the bigger the worse. */
	USHORT      TxQuality[MAX_TX_RATE_INDEX + 1];
	UINT32		OneSecTxNoRetryOkCount;
	UINT32      OneSecTxRetryOkCount;
	UINT32      OneSecTxFailCount;
	UINT32      OneSecRxLGICount;		/* unicast-to-me Long GI count */
	UINT32      OneSecRxSGICount;      	/* unicast-to-me Short GI count */

#ifdef FIFO_EXT_SUPPORT
	UINT32		fifoTxSucCnt;
	UINT32		fifoTxRtyCnt;
#endif /* FIFO_EXT_SUPPORT */


	BOOLEAN fLastSecAccordingRSSI;
	UCHAR LastSecTxRateChangeAction;	/* 0: no change, 1:rate UP, 2:rate down */
	CHAR LastTimeTxRateChangeAction;	/* Keep last time value of LastSecTxRateChangeAction */
	ULONG LastTxOkCount; /* TxSuccess count in last Rate Adaptation interval */
	UCHAR LastTxPER;	/* Tx PER in last Rate Adaptation interval */
	UCHAR PER[MAX_TX_RATE_INDEX + 1];

	UINT32 ContinueTxFailCnt;
	UINT32 CurrTxRateStableTime;	/* # of second in current TX rate */
	UCHAR TxRateUpPenalty;	/* extra # of second penalty due to last unstable condition */
#ifdef WDS_SUPPORT
	BOOLEAN LockEntryTx;	/* TRUE = block to WDS Entry traffic, FALSE = not. */
#endif /* WDS_SUPPORT */
	ULONG TimeStamp_toTxRing;

/*==================================================== */



#ifdef CONFIG_STA_SUPPORT
#ifdef QOS_DLS_SUPPORT
	UINT MatchDlsEntryIdx;	/* indicate the index in pAd->StaCfg.DLSEntry */
#endif /* QOS_DLS_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

	/*
		A bitmap of BOOLEAN flags. each bit represent an operation status of a particular
		BOOLEAN control, either ON or OFF. These flags should always be accessed via
		CLIENT_STATUS_TEST_FLAG(), CLIENT_STATUS_SET_FLAG(), CLIENT_STATUS_CLEAR_FLAG() macros.
		see fOP_STATUS_xxx in RTMP_DEF.C for detail bit definition. fCLIENT_STATUS_AMSDU_INUSED
	*/
	ULONG ClientStatusFlags;

	HTTRANSMIT_SETTING HTPhyMode, MaxHTPhyMode, MinHTPhyMode;	/* For transmit phy setting in TXWI. */

#ifdef DOT11_N_SUPPORT
	/* HT EWC MIMO-N used parameters */
	USHORT RXBAbitmap;	/* fill to on-chip  RXWI_BA_BITMASK in 8.1.3RX attribute entry format */
	USHORT TXBAbitmap;	/* This bitmap as originator, only keep in software used to mark AMPDU bit in TXWI */
	USHORT TXAutoBAbitmap;
	USHORT BADeclineBitmap;
	USHORT BARecWcidArray[NUM_OF_TID];	/* The mapping wcid of recipient session. if RXBAbitmap bit is masked */
	USHORT BAOriWcidArray[NUM_OF_TID];	/* The mapping wcid of originator session. if TXBAbitmap bit is masked */
	USHORT BAOriSequence[NUM_OF_TID];	/* The mapping wcid of originator session. if TXBAbitmap bit is masked */

	/* 802.11n features. */
	UCHAR MpduDensity;
	UCHAR MaxRAmpduFactor;
	UCHAR AMsduSize;
	UCHAR MmpsMode;		/* MIMO power save more. */

	HT_CAPABILITY_IE HTCapability;

#ifdef DOT11N_DRAFT3
	UCHAR BSS2040CoexistenceMgmtSupport;
	BOOLEAN bForty_Mhz_Intolerant;
#endif /* DOT11N_DRAFT3 */

#ifdef DOT11_VHT_AC
	VHT_CAP_IE vht_cap_ie;
#endif /* DOT11_VHT_AC */

#endif /* DOT11_N_SUPPORT */


	BOOLEAN bAutoTxRateSwitch;

	UCHAR RateLen;
	struct _MAC_TABLE_ENTRY *pNext;
	USHORT TxSeq[NUM_OF_TID];
	USHORT NonQosDataSeq;

	RSSI_SAMPLE RssiSample;
	UINT32 LastRxRate;
	SHORT freqOffset;		/* Last RXWI FOFFSET */
	SHORT freqOffsetValid;	/* Set when freqOffset field has been updated */


	BOOLEAN bWscCapable;
	UCHAR Receive_EapolStart_EapRspId;

	UINT32 TXMCSExpected[MAX_MCS_SET];
	UINT32 TXMCSSuccessful[MAX_MCS_SET];
	UINT32 TXMCSFailed[MAX_MCS_SET];
	UINT32 TXMCSAutoFallBack[MAX_MCS_SET][MAX_MCS_SET];

#ifdef CONFIG_STA_SUPPORT
	ULONG LastBeaconRxTime;
#endif /* CONFIG_STA_SUPPORT */



	ULONG AssocDeadLine;




	ULONG ChannelQuality;	/* 0..100, Channel Quality Indication for Roaming */


#ifdef VENDOR_FEATURE1_SUPPORT
	/* total 128B, use UINT32 to avoid alignment problem */
	UINT32 HeaderBuf[32];	/* (total 128B) TempBuffer for TX_INFO + TX_WI + 802.11 Header + padding + AMSDU SubHeader + LLC/SNAP */

	UCHAR HdrPadLen;	/* recording Header Padding Length; */
	UCHAR MpduHeaderLen;
	UINT16 Protocol;
#endif /* VENDOR_FEATURE1_SUPPORT */

#ifdef AGS_SUPPORT
	AGS_CONTROL AGSCtrl;	/* AGS control */
#endif /* AGS_SUPPORT */

#ifdef WFD_SUPPORT
	BOOLEAN bWfdClient;
#endif /* WFD_SUPPORT */

UCHAR	SupportRateMode; /* 1: CCK 2:OFDM 4: HT, 8:VHT */
BOOLEAN SupportCCKMCS[MAX_LEN_OF_CCK_RATES];
BOOLEAN SupportOFDMMCS[MAX_LEN_OF_OFDM_RATES];
BOOLEAN SupportHTMCS[MAX_LEN_OF_HT_RATES];


#ifdef DOT11_VHT_AC
	BOOLEAN SupportVHTMCS[MAX_LEN_OF_VHT_RATES];
#endif /* DOT11_VHT_AC */

#ifdef WFA_VHT_PF
#ifdef IP_ASSEMBLY
	QUEUE_HEADER ip_queue1[4];
	INT32 ip_id1[4];
	INT32 ip_id1_FragSize[4];
	ULONG ip_pkt1_jiffies[4];

	QUEUE_HEADER ip_queue2[4];
	INT32 ip_id2[4];
	INT32 ip_id2_FragSize[4];
	ULONG ip_pkt2_jiffies[4];

	struct ip_frag_q ip_fragQ[2];
	BOOLEAN ip_queue_inited;
#endif /* IP_ASSEMBLY */
#endif /* WFA_VHT_PF */

#ifdef MAC_REPEATER_SUPPORT
	UCHAR ExtAid;
	UCHAR ClientType; /* 0x00: apcli, 0x01: wireless station, 0x02: ethernet entry */
	BOOLEAN bConnToRootAP;
#endif /* MAC_REPEATER_SUPPORT */
} MAC_TABLE_ENTRY, *PMAC_TABLE_ENTRY;


typedef struct _MAC_TABLE {
	MAC_TABLE_ENTRY *Hash[HASH_TABLE_SIZE];
	MAC_TABLE_ENTRY Content[MAX_LEN_OF_MAC_TABLE];
	USHORT Size;
	QUEUE_HEADER McastPsQueue;
	ULONG PsQIdleCount;
	BOOLEAN fAnyStationInPsm;
	BOOLEAN fAnyStationBadAtheros;	/* Check if any Station is atheros 802.11n Chip.  We need to use RTS/CTS with Atheros 802,.11n chip. */
	BOOLEAN fAnyTxOPForceDisable;	/* Check if it is necessary to disable BE TxOP */
	BOOLEAN fAllStationAsRalink;	/* Check if all stations are ralink-chipset */
#ifdef DOT11_N_SUPPORT
	BOOLEAN fAnyStationIsLegacy;	/* Check if I use legacy rate to transmit to my BSS Station/ */
	BOOLEAN fAnyStationNonGF;	/* Check if any Station can't support GF. */
	BOOLEAN fAnyStation20Only;	/* Check if any Station can't support GF. */
	BOOLEAN fAnyStationMIMOPSDynamic;	/* Check if any Station is MIMO Dynamic */
	BOOLEAN fAnyBASession;	/* Check if there is BA session.  Force turn on RTS/CTS */
	BOOLEAN fAnyStaFortyIntolerant;	/* Check if still has any station set the Intolerant bit on! */
	BOOLEAN fAllStationGainGoodMCS; /* Check if all stations more than MCS threshold */

#endif				/* DOT11_N_SUPPORT */

	USHORT			MsduLifeTime; /* life time for PS packet */

} MAC_TABLE, *PMAC_TABLE;



#ifdef BLOCK_NET_IF
typedef struct _BLOCK_QUEUE_ENTRY {
	BOOLEAN SwTxQueueBlockFlag;
	LIST_HEADER NetIfList;
} BLOCK_QUEUE_ENTRY, *PBLOCK_QUEUE_ENTRY;
#endif /* BLOCK_NET_IF */


struct wificonf {
	BOOLEAN bShortGI;
	BOOLEAN bGreenField;
};

typedef struct _RTMP_DEV_INFO_ {
	UCHAR chipName[16];
	RTMP_INF_TYPE infType;
} RTMP_DEV_INFO;

#ifdef DBG_DIAGNOSE
#define DIAGNOSE_TIME	10	/* 10 sec */
typedef struct _RtmpDiagStrcut_ {	/* Diagnosis Related element */
	unsigned char inited;
	unsigned char qIdx;
	unsigned char ArrayStartIdx;
	unsigned char ArrayCurIdx;
	/* Tx Related Count */
	USHORT TxDataCnt[DIAGNOSE_TIME];
	USHORT TxFailCnt[DIAGNOSE_TIME];
/*	USHORT TxDescCnt[DIAGNOSE_TIME][16];		// TxDesc queue length in scale of 0~14, >=15 */
	USHORT TxDescCnt[DIAGNOSE_TIME][24];	/* 3*3    // TxDesc queue length in scale of 0~14, >=15 */
/*	USHORT TxMcsCnt[DIAGNOSE_TIME][16];			// TxDate MCS Count in range from 0 to 15, step in 1. */
	USHORT TxMcsCnt[DIAGNOSE_TIME][MAX_MCS_SET];	/* 3*3 */
	USHORT TxSWQueCnt[DIAGNOSE_TIME][9];	/* TxSwQueue length in scale of 0, 1, 2, 3, 4, 5, 6, 7, >=8 */

	USHORT TxAggCnt[DIAGNOSE_TIME];
	USHORT TxNonAggCnt[DIAGNOSE_TIME];
/*	USHORT TxAMPDUCnt[DIAGNOSE_TIME][16];		// 10 sec, TxDMA APMDU Aggregation count in range from 0 to 15, in setp of 1. */
	USHORT TxAMPDUCnt[DIAGNOSE_TIME][MAX_MCS_SET];	/* 3*3 // 10 sec, TxDMA APMDU Aggregation count in range from 0 to 15, in setp of 1. */
	USHORT TxRalinkCnt[DIAGNOSE_TIME];	/* TxRalink Aggregation Count in 1 sec scale. */
	USHORT TxAMSDUCnt[DIAGNOSE_TIME];	/* TxAMSUD Aggregation Count in 1 sec scale. */

	/* Rx Related Count */
	USHORT RxDataCnt[DIAGNOSE_TIME];	/* Rx Total Data count. */
	USHORT RxCrcErrCnt[DIAGNOSE_TIME];
/*	USHORT			RxMcsCnt[DIAGNOSE_TIME][16];		// Rx MCS Count in range from 0 to 15, step in 1. */
	USHORT RxMcsCnt[DIAGNOSE_TIME][MAX_MCS_SET];	/* 3*3 */
} RtmpDiagStruct;
#endif /* DBG_DIAGNOSE */

#if defined(RTMP_INTERNAL_TX_ALC) || defined(RTMP_TEMPERATURE_COMPENSATION)
/*
	The number of channels for per-channel Tx power offset
*/
#define NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET	14

/* The Tx power control using the internal ALC */
#ifdef RT8592
// TODO: shiang-6590, fix me for this ugly definition!
#define LOOKUP_TB_SIZE		45
#else
#define LOOKUP_TB_SIZE		33
#endif /* RT8592 */

typedef struct _TX_POWER_CONTROL {
	BOOLEAN bInternalTxALC; /* Internal Tx ALC */
	BOOLEAN bExtendedTssiMode; /* The extended TSSI mode (each channel has different Tx power if needed) */
	CHAR PerChTxPwrOffset[NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET + 1]; /* Per-channel Tx power offset */
	CHAR idxTxPowerTable; /* The index of the Tx power table for ant0 */
	CHAR idxTxPowerTable2; /* The index of the Tx power table for ant1 */
	CHAR RF_TX_ALC; /* 3390: RF R12[4:0]: Tx0 ALC, 3352: RF R47[4:0]: Tx0 ALC, 5390: RF R49[5:0]: Tx0 ALC */
	CHAR MAC_PowerDelta; /* Tx power control over MAC 0x1314~0x1324 */
	CHAR MAC_PowerDelta2; /* Tx power control for Tx1 */
	CHAR TotalDeltaPower2; /* Tx power control for Tx1 */
#ifdef RTMP_TEMPERATURE_COMPENSATION
	INT LookupTable[IEEE80211_BAND_NUMS][LOOKUP_TB_SIZE];
	INT RefTemp[IEEE80211_BAND_NUMS];
	UCHAR TssiGain[IEEE80211_BAND_NUMS];
	/* Index offset, -7....25. */
	INT LookupTableIndex;
#endif /* RTMP_TEMPERATURE_COMPENSATION */

} TX_POWER_CONTROL, *PTX_POWER_CONTROL;
#endif /* RTMP_INTERNAL_TX_ALC || RTMP_TEMPERATURE_COMPENSATION */

/* */
/* The entry of transmit power control over MAC */
/* */
typedef struct _TX_POWER_CONTROL_OVER_MAC_ENTRY {
	USHORT MACRegisterOffset;	/* MAC register offset */
	ULONG RegisterValue;	/* Register value */
} TX_POWER_CONTROL_OVER_MAC_ENTRY, *PTX_POWER_CONTROL_OVER_MAC_ENTRY;

/* */
/* The maximum registers of transmit power control */
/* */
#define MAX_TX_PWR_CONTROL_OVER_MAC_REGISTERS 5



#ifdef MT76x0
#undef MAX_TX_PWR_CONTROL_OVER_MAC_REGISTERS
#define MAX_TX_PWR_CONTROL_OVER_MAC_REGISTERS	6
#endif /* MT76x0 */

/* */
/* The configuration of the transmit power control over MAC */
/* */
typedef struct _CONFIGURATION_OF_TX_POWER_CONTROL_OVER_MAC {
	UCHAR NumOfEntries;	/* Number of entries */
	TX_POWER_CONTROL_OVER_MAC_ENTRY TxPwrCtrlOverMAC[MAX_TX_PWR_CONTROL_OVER_MAC_REGISTERS];
} CONFIGURATION_OF_TX_POWER_CONTROL_OVER_MAC, *PCONFIGURATION_OF_TX_POWER_CONTROL_OVER_MAC;

/* */
/* The extension of the transmit power control over MAC */
/* */
typedef struct _TX_POWER_CONTROL_EXT_OVER_MAC {
	struct {
		ULONG TxPwrCfg0;	/* MAC 0x1314 */
		ULONG TxPwrCfg0Ext;	/* MAC 0x1390 */
		ULONG TxPwrCfg1;	/* MAC 0x1318 */
		ULONG TxPwrCfg1Ext;	/* MAC 0x1394 */
		ULONG TxPwrCfg2;	/* MAC 0x131C */
		ULONG TxPwrCfg2Ext;	/* MAC 0x1398 */
		ULONG TxPwrCfg3;	/* MAC 0x1320 */
		ULONG TxPwrCfg3Ext;	/* MAC 0x139C */
		ULONG TxPwrCfg4;	/* MAC 0x1324 */
		ULONG TxPwrCfg4Ext;	/* MAC 0x13A0 */
		ULONG TxPwrCfg5;	/* MAC 0x1384 */
		ULONG TxPwrCfg6;	/* MAC 0x1388 */
		ULONG TxPwrCfg7;	/* MAC 0x13D4 */
		ULONG TxPwrCfg8;	/* MAC 0x13D8 */
		ULONG TxPwrCfg9;	/* MAC 0x13DC */
	} BW20Over2Dot4G;

	struct {
		ULONG TxPwrCfg0;	/* MAC 0x1314 */
		ULONG TxPwrCfg0Ext;	/* MAC 0x1390 */
		ULONG TxPwrCfg1;	/* MAC 0x1318 */
		ULONG TxPwrCfg1Ext;	/* MAC 0x1394 */
		ULONG TxPwrCfg2;	/* MAC 0x131C */
		ULONG TxPwrCfg2Ext;	/* MAC 0x1398 */
		ULONG TxPwrCfg3;	/* MAC 0x1320 */
		ULONG TxPwrCfg3Ext;	/* MAC 0x139C */
		ULONG TxPwrCfg4;	/* MAC 0x1324 */
		ULONG TxPwrCfg4Ext;	/* MAC 0x13A0 */
		ULONG TxPwrCfg5;	/* MAC 0x1384 */
		ULONG TxPwrCfg6;	/* MAC 0x1388 */
		ULONG TxPwrCfg7;	/* MAC 0x13D4 */
		ULONG TxPwrCfg8;	/* MAC 0x13D8 */
		ULONG TxPwrCfg9;	/* MAC 0x13DC */
	} BW40Over2Dot4G;

	struct {
		ULONG TxPwrCfg0;	/* MAC 0x1314 */
		ULONG TxPwrCfg0Ext;	/* MAC 0x1390 */
		ULONG TxPwrCfg1;	/* MAC 0x1318 */
		ULONG TxPwrCfg1Ext;	/* MAC 0x1394 */
		ULONG TxPwrCfg2;	/* MAC 0x131C */
		ULONG TxPwrCfg2Ext;	/* MAC 0x1398 */
		ULONG TxPwrCfg3;	/* MAC 0x1320 */
		ULONG TxPwrCfg3Ext;	/* MAC 0x139C */
		ULONG TxPwrCfg4;	/* MAC 0x1324 */
		ULONG TxPwrCfg4Ext;	/* MAC 0x13A0 */
		ULONG TxPwrCfg5;	/* MAC 0x1384 */
		ULONG TxPwrCfg6;	/* MAC 0x1388 */
		ULONG TxPwrCfg7;	/* MAC 0x13D4 */
		ULONG TxPwrCfg8;	/* MAC 0x13D8 */
		ULONG TxPwrCfg9;	/* MAC 0x13DC */
	} BW20Over5G;

	struct {
		ULONG TxPwrCfg0;	/* MAC 0x1314 */
		ULONG TxPwrCfg0Ext;	/* MAC 0x1390 */
		ULONG TxPwrCfg1;	/* MAC 0x1318 */
		ULONG TxPwrCfg1Ext;	/* MAC 0x1394 */
		ULONG TxPwrCfg2;	/* MAC 0x131C */
		ULONG TxPwrCfg2Ext;	/* MAC 0x1398 */
		ULONG TxPwrCfg3;	/* MAC 0x1320 */
		ULONG TxPwrCfg3Ext;	/* MAC 0x139C */
		ULONG TxPwrCfg4;	/* MAC 0x1324 */
		ULONG TxPwrCfg4Ext;	/* MAC 0x13A0 */
		ULONG TxPwrCfg5;	/* MAC 0x1384 */
		ULONG TxPwrCfg6;	/* MAC 0x1388 */
		ULONG TxPwrCfg7;	/* MAC 0x13D4 */
		ULONG TxPwrCfg8;	/* MAC 0x13D8 */
		ULONG TxPwrCfg9;	/* MAC 0x13DC */
	} BW40Over5G;
} TX_POWER_CONTROL_EXT_OVER_MAC, *PTX_POWER_CONTROL_EXT_OVER_MAC;

/* For Wake on Wireless LAN */
#ifdef WOW_SUPPORT
#ifdef RTMP_MAC_USB
typedef struct _WOW_CFG_STRUCT {
	BOOLEAN			bEnable;		/* Enable WOW function*/
	BOOLEAN			bWOWFirmware;	/* Enable WOW function, trigger to reload WOW-support firmware */
	u8			nSelectedGPIO;	/* Side band signal to wake up system */
	u8			nDelay;			/* Delay number is multiple of 3 secs, and it used to postpone the WOW function */
	u8           nHoldTime;      /* GPIO puls hold time, unit: 10ms */
} WOW_CFG_STRUCT, *PWOW_CFG_STRUCT;
#endif /* RTMP_MAC_USB */
#endif /* WOW_SUPPORT */

/*
	Packet drop reason code
*/
typedef enum{
	PKT_ATE_ON = 1 << 8,
	PKT_RADAR_ON = 2 << 8,
	PKT_RRM_QUIET = 3 << 8,
	PKT_TX_STOP = 4 <<8,
	PKT_TX_JAM = 5 << 8,

	PKT_NETDEV_DOWN = 6 < 8,
	PKT_NETDEV_NO_MATCH = 7 << 8,
	PKT_NOT_ALLOW_SEND = 8 << 8,

	PKT_INVALID_DST = 9<< 8,
	PKT_INVALID_SRC = 10 << 8,
	PKT_INVALID_PKT_DATA = 11 << 8,
	PKT_INVALID_PKT_LEN = 12 << 8,
	PKT_INVALID_ETH_TYPE = 13 << 8,
	PKT_INVALID_TXBLK_INFO = 14 << 8,
	PKT_INVALID_SW_ENCRYPT = 15 << 8,
	PKT_INVALID_PKT_TYPE = 16 << 8,
	PKT_INVALID_PKT_MIC = 17 << 8,

	PKT_PORT_NOT_SECURE = 18 << 8,
	PKT_TSPEC_NO_MATCH  = 19 << 8,
	PKT_NO_ASSOCED_STA = 20 << 8,
	PKT_INVALID_MAC_ENTRY = 21 << 8,

	PKT_TX_QUE_FULL = 22 << 8,
	PKT_TX_QUE_ADJUST = 23<<8,

	PKT_PS_QUE_TIMEOUT = 24 <<8,
	PKT_PS_QUE_CLEAN = 25 << 8,
	PKT_MCAST_PS_QUE_FULL = 26 << 8,
	PKT_UCAST_PS_QUE_FULL = 27 << 8,

	PKT_RX_EAPOL_SANITY_FAIL = 28 <<8,
	PKT_RX_NOT_TO_KERNEL = 29 << 8,
	PKT_RX_MESH_SIG_FAIL = 30 << 8,
	PKT_APCLI_FAIL = 31 << 8,
	PKT_ZERO_DATA = 32 <<8,
	PKT_SW_DECRYPT_FAIL = 33 << 8,
	PKT_TX_SW_ENC_FAIL = 34 << 8,

	PKT_ACM_FAIL = 35 << 8,
	PKT_IGMP_GRP_FAIL = 36 << 8,
	PKT_MGMT_FAIL = 37 << 8,
	PKT_AMPDU_OUT_ORDER = 38 << 8,
	PKT_UAPSD_EOSP = 39 << 8,
	PKT_UAPSD_Q_FULL = 40 << 8,

	PKT_DRO_REASON_MAX = 41,
}PKT_DROP_REASON;

/* Packet drop Direction code */
typedef enum{
	PKT_TX = 0,
	PKT_RX = 1 << 31,
}PKT_DROP_DIECTION;




typedef struct _BBP_RESET_CTL
{
#define BBP_RECORD_NUM	47
	REG_PAIR BBPRegDB[BBP_RECORD_NUM];
	BOOLEAN	AsicCheckEn;
} BBP_RESET_CTL, *PBBP_RESET_CTL;


typedef struct tx_agc_ctrl{
	BOOLEAN bAutoTxAgc;	/* Enable driver auto Tx Agc control */
	UCHAR TssiRef;		/* Store Tssi reference value as 25 temperature. */
	UCHAR TssiPlusBoundary[5];	/* Tssi boundary for increase Tx power to compensate. */
	UCHAR TssiMinusBoundary[5];	/* Tssi boundary for decrease Tx power to compensate. */
	UCHAR TxAgcStep;	/* Store Tx TSSI delta increment / decrement value */
	CHAR TxAgcComp;	/* Store the compensation (TxAgcStep * (idx-1)) */
}TX_AGC_CTRL;


/*
	The miniport adapter structure
*/
struct rtmp_adapter {
	void *OS_Cookie;	/* save specific structure relative to OS */
	PNET_DEV net_dev;
	ULONG VirtualIfCnt;

	//BOOLEAN PollIdle;

	RTMP_CHIP_OP chipOps;
	RTMP_CHIP_CAP chipCap;

#ifdef CONFIG_STA_SUPPORT
	USHORT ThisTbttNumToNextWakeUp;
#endif /* CONFIG_STA_SUPPORT */

#ifdef HOSTAPD_SUPPORT
	UINT32 IoctlIF;
#endif /* HOSTAPD_SUPPORT */

	NDIS_SPIN_LOCK irq_lock;

	/*======Cmd Thread in PCI/RBUS/USB */
	CmdQ CmdQ;
	NDIS_SPIN_LOCK CmdQLock;	/* CmdQLock spinlock */
	RTMP_OS_TASK cmdQTask;

#ifdef RTMP_MAC_USB
/*****************************************************************************************/
/*      USB related parameters                                                           */
/*****************************************************************************************/
/*	struct usb_config_descriptor		*config; */
	void *config;

	UINT NumberOfPipes;
	USHORT BulkOutMaxPacketSize;
	USHORT BulkInMaxPacketSize;
	u8 BulkOutEpAddr[6];
	u8 BulkInEpAddr[2];

	/*======Control Flags */
	ULONG BulkFlags;
	BOOLEAN bUsbTxBulkAggre;	/* Flags for bulk out data priority */

	/*======Cmd Thread */
/*	CmdQ					CmdQ; */
/*	NDIS_SPIN_LOCK			CmdQLock;				// CmdQLock spinlock */
/*	RTMP_OS_TASK			cmdQTask; */

	/*======Semaphores (event) */
	RTMP_OS_SEM UsbVendorReq_semaphore;
	RTMP_OS_SEM reg_atomic;
	RTMP_OS_SEM hw_atomic;
	RTMP_OS_SEM cal_atomic;
	RTMP_OS_SEM wlan_en_atomic;
	RTMP_OS_SEM mcu_atomic;
	void *UsbVendorReqBuf;
/*	wait_queue_head_t	 *wait; */
	void *wait;

	/* lock for ATE */
#ifdef RALINK_ATE
	NDIS_SPIN_LOCK GenericLock;	/* ATE Tx/Rx generic spinlock */
#endif /* RALINK_ATE */

#endif /* RTMP_MAC_USB */

/*****************************************************************************************/
/*      RBUS related parameters                                                           								  */
/*****************************************************************************************/

/*****************************************************************************************/
/*      Both PCI/USB related parameters                                                  							  */
/*****************************************************************************************/
	/*RTMP_DEV_INFO                 chipInfo; */
	RTMP_INF_TYPE infType;

/*****************************************************************************************/
/*      Driver Mgmt related parameters                                                  							  */
/*****************************************************************************************/
	RTMP_OS_TASK mlmeTask;
#ifdef RTMP_TIMER_TASK_SUPPORT
	/* If you want use timer task to handle the timer related jobs, enable this. */
	RTMP_TIMER_TASK_QUEUE TimerQ;
	NDIS_SPIN_LOCK TimerQLock;
	RTMP_OS_TASK timerTask;
#endif /* RTMP_TIMER_TASK_SUPPORT */

/*****************************************************************************************/
/*      Tx related parameters                                                           */
/*****************************************************************************************/
	BOOLEAN DeQueueRunning[NUM_OF_TX_RING];	/* for ensuring RTUSBDeQueuePacket get call once */
	NDIS_SPIN_LOCK DeQueueLock[NUM_OF_TX_RING];

#ifdef RTMP_MAC_USB
	/* Data related context and AC specified, 4 AC supported */
	NDIS_SPIN_LOCK BulkOutLock[6];	/* BulkOut spinlock for 4 ACs */
	NDIS_SPIN_LOCK MLMEBulkOutLock;	/* MLME BulkOut lock */

	HT_TX_CONTEXT TxContext[NUM_OF_TX_RING];
	NDIS_SPIN_LOCK TxContextQueueLock[NUM_OF_TX_RING];	/* TxContextQueue spinlock */

	/* 4 sets of Bulk Out index and pending flag */
	/*
	   array size of NextBulkOutIndex must be larger than or equal to 5;
	   Or BulkOutPending[0] will be overwrited in NICInitTransmit().
	 */
	UCHAR NextBulkOutIndex[NUM_OF_TX_RING];	/* only used for 4 EDCA bulkout pipe */

	BOOLEAN BulkOutPending[6];	/* used for total 6 bulkout pipe */
	UCHAR bulkResetPipeid;
	BOOLEAN MgmtBulkPending;
	ULONG bulkResetReq[6];
#ifdef INF_AMAZON_SE
	ULONG BulkOutDataSizeCount[NUM_OF_TX_RING];
	BOOLEAN BulkOutDataFlag[NUM_OF_TX_RING];
	ULONG BulkOutDataSizeLimit[NUM_OF_TX_RING];
	UCHAR RunningQueueNoCount;
	UCHAR LastRunningQueueNo;
#endif /* #ifdef INF_AMAZON_SE */

#ifdef CONFIG_STA_SUPPORT
	USHORT CountDowntoPsm;
#endif /* CONFIG_STA_SUPPORT */

#endif /* RTMP_MAC_USB */

	/* resource for software backlog queues */
	QUEUE_HEADER TxSwQueue[NUM_OF_TX_RING];	/* 4 AC + 1 HCCA */
	NDIS_SPIN_LOCK TxSwQueueLock[NUM_OF_TX_RING];	/* TxSwQueue spinlock */

	/* Maximum allowed tx software Queue length */
	UINT32					TxSwQMaxLen;

	RTMP_DMABUF MgmtDescRing;	/* Shared memory for MGMT descriptors */
	RTMP_MGMT_RING MgmtRing;
	NDIS_SPIN_LOCK MgmtRingLock;	/* Prio Ring spinlock */

#ifdef CONFIG_ANDES_SUPPORT
	RTMP_CTRL_RING CtrlRing;
	NDIS_SPIN_LOCK CtrlRingLock;	/* Ctrl Ring spinlock */
#endif

	UCHAR LastMCUCmd;

/*****************************************************************************************/
/*      Rx related parameters                                                           */
/*****************************************************************************************/


#ifdef RTMP_MAC_USB
	RX_CONTEXT RxContext[RX_RING_SIZE];	/* 1 for redundant multiple IRP bulk in. */
	NDIS_SPIN_LOCK BulkInLock;	/* BulkIn spinlock for 4 ACs */
	NDIS_SPIN_LOCK CmdRspLock;
	UCHAR PendingRx;	/* The Maximum pending Rx value should be       RX_RING_SIZE. */
	UCHAR NextRxBulkInIndex;	/* Indicate the current RxContext Index which hold by Host controller. */
	UCHAR NextRxBulkInReadIndex;	/* Indicate the current RxContext Index which driver can read & process it. */
	ULONG NextRxBulkInPosition;	/* Want to contatenate 2 URB buffer while 1st is bulkin failed URB. This Position is 1st URB TransferLength. */
	ULONG TransferBufferLength;	/* current length of the packet buffer */
	ULONG ReadPosition;	/* current read position in a packet buffer */

	CMD_RSP_CONTEXT CmdRspEventContext;
#endif /* RTMP_MAC_USB */

/*****************************************************************************************/
/*      ASIC related parameters                                                          */
/*****************************************************************************************/
	UINT32 MACVersion;	/* MAC version. Record rt2860C(0x28600100) or rt2860D (0x28600101).. */
	UINT32 MacIcVersion;

	/* --------------------------- */
	/* E2PROM */
	/* --------------------------- */
	ULONG EepromVersion;	/* byte 0: version, byte 1: revision, byte 2~3: unused */
	ULONG FirmwareVersion;	/* byte 0: Minor version, byte 1: Major version, otherwise unused. */
	USHORT EEPROMDefaultValue[NUM_EEPROM_BBP_PARMS];
#ifdef TXBF_SUPPORT
	USHORT EEPROMITxBFCalParams[6];
#endif /* TXBF_SUPPORT */
	UCHAR EEPROMAddressNum;	/* 93c46=6  93c66=8 */
	BOOLEAN EepromAccess;
	UCHAR EFuseTag;

	/* --------------------------- */
	/* BBP Control */
	/* --------------------------- */
	UCHAR BbpWriteLatch[MAX_BBP_ID + 1];	/* record last BBP register value written via BBP_IO_WRITE/BBP_IO_WRITE_VY_REG_ID */
	CHAR BbpRssiToDbmDelta;	/* change from UCHAR to CHAR for high power */
	BBP_R66_TUNING BbpTuning;

	/* ---------------------------- */
	/* RFIC control */
	/* ---------------------------- */
	UCHAR RfIcType;		/* RFIC_xxx */
	ULONG RfFreqOffset;	/* Frequency offset for channel switching */


	RTMP_RF_REGS LatchRfRegs;	/* latch th latest RF programming value since RF IC doesn't support READ */

	EEPROM_ANTENNA_STRUC Antenna;	/* Since ANtenna definition is different for a & g. We need to save it for future reference. */
	EEPROM_NIC_CONFIG2_STRUC NicConfig2;
#if defined(BT_COEXISTENCE_SUPPORT) || defined(RT3290) || defined(RT8592)
	EEPROM_NIC_CONFIG3_STRUC NicConfig3;
#endif /* defined(BT_COEXISTENCE_SUPPORT) || defined(RT3290) || defined(RT8592) */

	/* This soft Rx Antenna Diversity mechanism is used only when user set */
	/* RX Antenna = DIVERSITY ON */
	SOFT_RX_ANT_DIVERSITY RxAnt;

	CHANNEL_TX_POWER TxPower[MAX_NUM_OF_CHANNELS];	/* Store Tx power value for all channels. */
	CHANNEL_TX_POWER ChannelList[MAX_NUM_OF_CHANNELS];	/* list all supported channels for site survey */



	UCHAR ChannelListNum;	/* number of channel in ChannelList[] */
	UCHAR Bbp94;
	BOOLEAN BbpForCCK;
	ULONG Tx20MPwrCfgABand[MAX_TXPOWER_ARRAY_SIZE];
	ULONG Tx20MPwrCfgGBand[MAX_TXPOWER_ARRAY_SIZE];
	ULONG Tx40MPwrCfgABand[MAX_TXPOWER_ARRAY_SIZE];
	ULONG Tx40MPwrCfgGBand[MAX_TXPOWER_ARRAY_SIZE];
#ifdef DOT11_VHT_AC
	ULONG Tx80MPwrCfgABand[MAX_TXPOWER_ARRAY_SIZE]; // Per-rate Tx power control for VHT BW80 (5GHz only)
#endif /* DOT11_VHT_AC */


	BOOLEAN bAutoTxAgcA;	/* Enable driver auto Tx Agc control */
	UCHAR TssiRefA;		/* Store Tssi reference value as 25 temperature. */
	UCHAR TssiPlusBoundaryA[2][8];	/* Tssi boundary for increase Tx power to compensate. [Group][Boundary Index]*/
	UCHAR TssiMinusBoundaryA[2][8];	/* Tssi boundary for decrease Tx power to compensate. [Group][Boundary Index]*/
	UCHAR TxAgcStepA;	/* Store Tx TSSI delta increment / decrement value */
	CHAR TxAgcCompensateA;	/* Store the compensation (TxAgcStep * (idx-1)) */

	BOOLEAN bAutoTxAgcG;	/* Enable driver auto Tx Agc control */
	UCHAR TssiRefG;		/* Store Tssi reference value as 25 temperature. */
	UCHAR TssiPlusBoundaryG[8];	/* Tssi boundary for increase Tx power to compensate. */
	UCHAR TssiMinusBoundaryG[8];	/* Tssi boundary for decrease Tx power to compensate. */
	UCHAR TxAgcStepG;	/* Store Tx TSSI delta increment / decrement value */
	CHAR TxAgcCompensateG;	/* Store the compensation (TxAgcStep * (idx-1)) */

#ifdef RTMP_TEMPERATURE_TX_ALC
	CHAR TssiCalibratedOffset;	/* reference temperature(e2p[D1h]) */
	UCHAR ChBndryIdx;			/* 5G Channel Group Boundary Index for Temperature Compensation */

	CHAR mp_delta_pwr;			 /* calculated by MP ATE temperature */
	CHAR CurrTemperature;
	CHAR DeltaPwrBeforeTempComp;
	CHAR LastTempCompDeltaPwr;
#endif /* RTMP_TEMPERATURE_TX_ALC */

#if defined(RTMP_INTERNAL_TX_ALC) || defined(RTMP_TEMPERATURE_COMPENSATION)
	TX_POWER_CONTROL TxPowerCtrl;	/* The Tx power control using the internal ALC */
	CHAR curr_temp;
	CHAR rx_temp_base[2];	/* initial VGA value for chain 0/1,  used for base of rx temp compensation power base */
#endif /* RTMP_INTERNAL_TX_ALC || RTMP_TEMPERATURE_COMPENSATION */


	signed char BGRssiOffset[3]; /* Store B/G RSSI #0/1/2 Offset value on EEPROM 0x46h */
	signed char ARssiOffset[3]; /* Store A RSSI 0/1/2 Offset value on EEPROM 0x4Ah */

	CHAR BLNAGain;		/* Store B/G external LNA#0 value on EEPROM 0x44h */
	CHAR ALNAGain0;		/* Store A external LNA#0 value for ch36~64 */
	CHAR ALNAGain1;		/* Store A external LNA#1 value for ch100~128 */
	CHAR ALNAGain2;		/* Store A external LNA#2 value for ch132~165 */


#ifdef LED_CONTROL_SUPPORT
	/* LED control */
	LED_CONTROL LedCntl;
#endif /* LED_CONTROL_SUPPORT */

	/* ---------------------------- */
	/* MAC control */
	/* ---------------------------- */

#ifdef RT8592
	CHAR bw_cal[3];	// bw cal value for RF_R32, 0:20M, 1:40M, 2:80M
// TODO: shiang-6590, temporary get from windows and need to revise it!!
	/* IQ Calibration */
	UCHAR IQGainTx[3][4];
	UCHAR IQPhaseTx[3][4];
	USHORT IQControl;
#endif /* RT8592 */

#if defined(RT3290) || defined(RT65xx)

	WLAN_FUN_CTRL_STRUC WlanFunCtrl;
#endif /* defined(RT3290) || defined(RT65xx) */
#ifdef RT3290
	// TODO: shiang, check about the purpose of this parameter
	CMB_CTRL_STRUC	CmbCtrl;
	WLAN_FUN_INFO_STRUC WlanFunInfo;
	BT_FUN_CTRL_STRUC BtFunCtrl;
	WLAN_BT_COEX_SETTING WlanBTCoexInfo;
	BOOLEAN RateUp;
	ULONG AntennaDiversityState;/* 0->Stable state 1->training state */
	ULONG AntennaDiversityPER[2];  // 0 ->main 1->aux
	ULONG AntennaDiversityTxPacketCount[2];  // 0 ->main 1->aux
	ULONG AntennaDiversityRxPacketCount[2];
	ULONG AntennaDiversityTrigger;
	ULONG AntennaDiversityCount;
	ULONG TrainCounter;
	ANT_DIVERSITY AntennaDiversityInfo;
#endif /* RT3290 */

	struct hw_setting hw_cfg;

/*****************************************************************************************/
/*      802.11 related parameters                                                        */
/*****************************************************************************************/
	/* outgoing BEACON frame buffer and corresponding TXD */
	TXWI_STRUC BeaconTxWI;
	PUCHAR BeaconBuf;
	USHORT BeaconOffset[HW_BEACON_MAX_NUM];

	/* pre-build PS-POLL and NULL frame upon link up. for efficiency purpose. */
#ifdef CONFIG_STA_SUPPORT
	PSPOLL_FRAME PsPollFrame;
#endif /* CONFIG_STA_SUPPORT */
	HEADER_802_11 NullFrame;

#ifdef RTMP_MAC_USB
	TX_CONTEXT NullContext;
	TX_CONTEXT PsPollContext;
#endif /* RTMP_MAC_USB */


#ifdef UAPSD_SUPPORT
	NDIS_SPIN_LOCK UAPSDEOSPLock;	/* EOSP frame access lock use */
	BOOLEAN bAPSDFlagSPSuspend;	/* 1: SP is suspended; 0: SP is not */
#endif /* UAPSD_SUPPORT */

/*=======STA=========== */
#ifdef CONFIG_STA_SUPPORT
	/* ----------------------------------------------- */
	/* STA specific configuration & operation status */
	/* used only when pAd->OpMode == OPMODE_STA */
	/* ----------------------------------------------- */
	STA_ADMIN_CONFIG StaCfg;	/* user desired settings */
	STA_ACTIVE_CONFIG StaActive;	/* valid only when ADHOC_ON(pAd) || INFRA_ON(pAd) */
	CHAR nickname[IW_ESSID_MAX_SIZE + 1];	/* nickname, only used in the iwconfig i/f */
	NDIS_MEDIA_STATE PreMediaState;
#endif /* CONFIG_STA_SUPPORT */

/*=======Common=========== */
	/* OP mode: either AP or STA */
	UCHAR OpMode;		/* OPMODE_STA, OPMODE_AP */

	enum RATE_ADAPT_ALG rateAlg;		/* Rate adaptation algorithm */

	NDIS_MEDIA_STATE IndicateMediaState;	/* Base on Indication state, default is NdisMediaStateDisConnected */

#ifdef PROFILE_STORE
	RTMP_OS_TASK 	WriteDatTask;
	BOOLEAN			bWriteDat;
#endif /* PROFILE_STORE */

#ifdef CREDENTIAL_STORE
	STA_CONNECT_INFO StaCtIf;
#endif /* CREDENTIAL_STORE */



	/* MAT related parameters */


	/*
		Frequency setting for rate adaptation
			@ra_interval: 		for baseline time interval
			@ra_fast_interval:	for quick response time interval
	*/
	UINT32			ra_interval;
	UINT32			ra_fast_interval;

	/* configuration: read from Registry & E2PROM */
	BOOLEAN bLocalAdminMAC;	/* Use user changed MAC */
	UCHAR PermanentAddress[MAC_ADDR_LEN];	/* Factory default MAC address */
	UCHAR CurrentAddress[MAC_ADDR_LEN];	/* User changed MAC address */

	/* ------------------------------------------------------ */
	/* common configuration to both OPMODE_STA and OPMODE_AP */
	/* ------------------------------------------------------ */
	struct common_config CommonCfg;
	MLME_STRUCT Mlme;

	/* AP needs those vaiables for site survey feature. */
	MLME_AUX MlmeAux;	/* temporary settings used during MLME state machine */
#if defined(AP_SCAN_SUPPORT) || defined(CONFIG_STA_SUPPORT)
	BSS_TABLE ScanTab;	/* store the latest SCAN result */
#endif /* defined(AP_SCAN_SUPPORT) || defined(CONFIG_STA_SUPPORT) */

	/*About MacTab, the sta driver will use #0 and #1 for multicast and AP. */
	MAC_TABLE MacTab;	/* ASIC on-chip WCID entry table.  At TX, ASIC always use key according to this on-chip table. */
	NDIS_SPIN_LOCK MacTabLock;

#ifdef DOT11_N_SUPPORT
	BA_TABLE BATable;
	NDIS_SPIN_LOCK BATabLock;
	RALINK_TIMER_STRUCT RECBATimer;
#endif /* DOT11_N_SUPPORT */

	/* DOT11_H */
	DOT11_H Dot11_H;

	/* encryption/decryption KEY tables */
	CIPHER_KEY SharedKey[HW_BEACON_MAX_NUM + MAX_P2P_NUM][4];	/* STA always use SharedKey[BSS0][0..3] */

	/* RX re-assembly buffer for fragmentation */
	FRAGMENT_FRAME FragFrame;	/* Frame storage for fragment frame */

	/* various Counters */
	COUNTER_802_3 Counters8023;	/* 802.3 counters */
	COUNTER_802_11 WlanCounters;	/* 802.11 MIB counters */
	COUNTER_RALINK RalinkCounters;	/* Ralink propriety counters */
	/* COUNTER_DRS DrsCounters;	*/ /* counters for Dynamic TX Rate Switching */
	PRIVATE_STRUC PrivateInfo;	/* Private information & counters */

	/* flags, see fRTMP_ADAPTER_xxx flags */
	ULONG Flags;		/* Represent current device status */
	ULONG PSFlags;		/* Power Save operation flag. */
	ULONG MoreFlags;	/* Represent specific requirement */

	/* current TX sequence # */
	USHORT Sequence;

	/* Control disconnect / connect event generation */
	/*+++Didn't used anymore */
	ULONG LinkDownTime;
	/*--- */
	ULONG LastRxRate;
	ULONG LastTxRate;
	/*+++Used only for Station */
	BOOLEAN bConfigChanged;	/* Config Change flag for the same SSID setting */
	/*--- */

	ULONG ExtraInfo;	/* Extra information for displaying status */
	ULONG SystemErrorBitmap;	/* b0: E2PROM version error */

	BOOLEAN HTCEnable;

	/*****************************************************************************************/
	/*      Statistic related parameters                                                     */
	/*****************************************************************************************/
#ifdef RTMP_MAC_USB
	ULONG BulkOutDataOneSecCount;
	ULONG BulkInDataOneSecCount;
	ULONG BulkLastOneSecCount;	/* BulkOutDataOneSecCount + BulkInDataOneSecCount */
	ULONG watchDogRxCnt;
	ULONG watchDogRxOverFlowCnt;
	ULONG watchDogTxPendingCnt[NUM_OF_TX_RING];
#endif /* RTMP_MAC_USB */

	BOOLEAN bUpdateBcnCntDone;

	ULONG macwd;
	/* ---------------------------- */
	/* DEBUG paramerts */
	/* ---------------------------- */

	/* ---------------------------- */
	/* rt2860c emulation-use Parameters */
	/* ---------------------------- */
	/*ULONG         rtsaccu[30]; */
	/*ULONG         ctsaccu[30]; */
	/*ULONG         cfendaccu[30]; */
	/*ULONG         bacontent[16]; */
	/*ULONG         rxint[RX_RING_SIZE+1]; */
	/*UCHAR         rcvba[60]; */
	BOOLEAN bLinkAdapt;
	BOOLEAN bForcePrintTX;
	BOOLEAN bForcePrintRX;
	/*BOOLEAN               bDisablescanning;               //defined in RT2870 USB */
	BOOLEAN bStaFifoTest;
	BOOLEAN bProtectionTest;
	BOOLEAN bHCCATest;
	BOOLEAN bGenOneHCCA;
	BOOLEAN bBroadComHT;
	/*+++Following add from RT2870 USB. */
	ULONG BulkOutReq;
	ULONG BulkOutComplete;
	ULONG BulkOutCompleteOther;
	ULONG BulkOutCompleteCancel;	/* seems not use now? */
	ULONG BulkInReq;
	ULONG BulkInComplete;
	ULONG BulkInCompleteFail;
	/*--- */

	struct wificonf WIFItestbed;

	UCHAR		TssiGain;
#ifdef RALINK_ATE
	ATE_INFO ate;
#ifdef RTMP_MAC_USB
	BOOLEAN ContinBulkOut;	/*ATE bulk out control */
	BOOLEAN ContinBulkIn;	/*ATE bulk in control */
	RTMP_OS_ATOMIC BulkOutRemained;
	RTMP_OS_ATOMIC BulkInRemained;
#endif /* RTMP_MAC_USB */
#endif /* RALINK_ATE */

#ifdef DOT11_N_SUPPORT
	struct reordering_mpdu_pool mpdu_blk_pool;
#endif /* DOT11_N_SUPPORT */

	/* statistics count */

	void *iw_stats;
	void *stats;

#ifdef BLOCK_NET_IF
	BLOCK_QUEUE_ENTRY blockQueueTab[NUM_OF_TX_RING];
#endif /* BLOCK_NET_IF */



#ifdef MULTIPLE_CARD_SUPPORT
	INT32 MC_RowID;
	STRING MC_FileName[256];
#endif /* MULTIPLE_CARD_SUPPORT */

	ULONG TbttTickCount;	/* beacon timestamp work-around */
#ifdef PCI_MSI_SUPPORT
	BOOLEAN HaveMsi;
#endif /* PCI_MSI_SUPPORT */


	/* for detect_wmm_traffic() BE TXOP use */
	ULONG OneSecondnonBEpackets;	/* record non BE packets per second */
	UCHAR is_on;

	/* for detect_wmm_traffic() BE/BK TXOP use */
#define TIME_BASE			(1000000/OS_HZ)
#define TIME_ONE_SECOND		(1000000/TIME_BASE)
	UCHAR flg_be_adjust;
	ULONG be_adjust_last_time;

#ifdef NINTENDO_AP
	NINDO_CTRL_BLOCK nindo_ctrl_block;
#endif /* NINTENDO_AP */

#ifdef DBG_DIAGNOSE
	RtmpDiagStruct DiagStruct;
#endif /* DBG_DIAGNOSE */


	u8 FlgCtsEnabled;
	u8 PM_FlgSuspend;

#ifdef RTMP_EFUSE_SUPPORT
	BOOLEAN bUseEfuse;
	BOOLEAN bEEPROMFile;
	BOOLEAN bFroceEEPROMBuffer;
	UCHAR EEPROMImage[1024];
#endif /* RTMP_EFUSE_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */

	EXT_CAP_INFO_ELEMENT ExtCapInfo;


#ifdef VENDOR_FEATURE1_SUPPORT
	UCHAR FifoUpdateDone, FifoUpdateRx;
#endif /* VENDOR_FEATURE1_SUPPORT */

	u8 RFICType;

#ifdef LINUX
#ifdef RT_CFG80211_SUPPORT
	void *pCfgDev;
	void *pCfg80211_CB;

	BOOLEAN FlgCfg80211Scanning;
	BOOLEAN FlgCfg80211Connecting;
	UCHAR Cfg80211_Alpha2[2];
#endif /* RT_CFG80211_SUPPORT */
#endif /* LINUX */

#ifdef OS_ABL_SUPPORT
#endif /* OS_ABL_SUPPORT */

	UINT32 ContinueMemAllocFailCount;

	struct {
		INT IeLen;
		UCHAR *pIe;
	} ProbeRespIE[MAX_LEN_OF_BSS_TABLE];

	/* purpose: We free all kernel resources when module is removed */
	LIST_HEADER RscTimerMemList;	/* resource timers memory */
	LIST_HEADER RscTaskMemList;	/* resource tasks memory */
	LIST_HEADER RscLockMemList;	/* resource locks memory */
	LIST_HEADER RscTaskletMemList;	/* resource tasklets memory */
	LIST_HEADER RscSemMemList;	/* resource semaphore memory */
	LIST_HEADER RscAtomicMemList;	/* resource atomic memory */

	/* purpose: Cancel all timers when module is removed */
	LIST_HEADER RscTimerCreateList;	/* timers list */

#ifdef OS_ABL_SUPPORT
#endif /* OS_ABL_SUPPORT */





#ifdef WOW_SUPPORT
#ifdef RTMP_MAC_USB
	WOW_CFG_STRUCT WOW_Cfg; /* data structure for wake on wireless */
#endif /* RTMP_MAC_USB */
#endif /* WOW_SUPPORT */

#ifdef WLAN_SKB_RECYCLE
    struct sk_buff_head rx0_recycle;
#endif /* WLAN_SKB_RECYCLE */

#ifdef CONFIG_FPGA_MODE
	struct fpga_ctrl fpga_ctl;
#ifdef CAPTURE_MODE
	BOOLEAN cap_support;	/* 0: no cap mode; 1: cap mode enable */
	UCHAR cap_type;			/* 1: ADC6, 2: ADC8, 3: FEQ */
	UCHAR cap_trigger;		/* 1: manual trigger, 2: auto trigger */
	BOOLEAN do_cap;			/* 1: start to do cap, if auto, will triggered depends on trigger condition, if manual, start immediately */
	BOOLEAN cap_done;		/* 1: capture done, 0: capture not finish yet */
	UINT32 trigger_offset;	/* in unit of bytes */
	UCHAR *cap_buf;
#endif /* CAPTURE_MODE */
#endif /* CONFIG_FPGA_MODE */
#ifdef WFA_VHT_PF
	BOOLEAN force_amsdu;
	BOOLEAN force_noack;
	BOOLEAN vht_force_sgi;
	BOOLEAN vht_force_tx_stbc;
#ifdef IP_ASSEMBLY
	BOOLEAN ip_assemble;
#endif /* IP_ASSEMBLY */
#endif /* WFA_VHT_PF */

	UCHAR bloopBackTest;
	BOOLEAN bHwTxLookupRate;
	TXWI_STRUC NullTxWI;
	BOOLEAN TestMulMac;

	struct MCU_CTRL MCUCtrl;

#ifdef SINGLE_SKU_V2
	DL_LIST SingleSkuPwrList;
	CHAR DefaultTargetPwr;
	CHAR SingleSkuRatePwrDiff[19];
#endif /* SINGLE_SKU_V2 */
};

#if defined(RTMP_INTERNAL_TX_ALC) || defined(RTMP_TEMPERATURE_COMPENSATION)
/* The offset of the Tx power tuning entry (zero-based array) */
#define TX_POWER_TUNING_ENTRY_OFFSET			30

/* The lower-bound of the Tx power tuning entry */
#define LOWERBOUND_TX_POWER_TUNING_ENTRY		-30

/* The upper-bound of the Tx power tuning entry in G band */
#define UPPERBOUND_TX_POWER_TUNING_ENTRY(__pAd)		((__pAd)->chipCap.TxAlcTxPowerUpperBound_2G)

#ifdef A_BAND_SUPPORT
/* The upper-bound of the Tx power tuning entry in A band */
#define UPPERBOUND_TX_POWER_TUNING_ENTRY_5G(__pAd)		((__pAd)->chipCap.TxAlcTxPowerUpperBound_5G)
#endif /* A_BAND_SUPPORT */

/* Temperature compensation lookup table */

#define TEMPERATURE_COMPENSATION_LOOKUP_TABLE_OFFSET	7

/* The lower/upper power delta index for the TSSI rate table */

#define LOWER_POWER_DELTA_INDEX		0
#define UPPER_POWER_DELTA_INDEX		24

/* The offset of the TSSI rate table */

#define TSSI_RATIO_TABLE_OFFSET	12


/* Get the power delta bound */

#define GET_TSSI_RATE_TABLE_INDEX(x) (((x) > UPPER_POWER_DELTA_INDEX) ? (UPPER_POWER_DELTA_INDEX) : (((x) < LOWER_POWER_DELTA_INDEX) ? (LOWER_POWER_DELTA_INDEX) : ((x))))

/* 802.11b CCK TSSI information */

typedef union _CCK_TSSI_INFO
{
#ifdef RT_BIG_ENDIAN
	struct
	{
		UCHAR	Reserved:1;
		UCHAR	ShortPreamble:1;
		UCHAR	Rate:2;
		UCHAR	Tx40MSel:2;
		UCHAR	TxType:2;
	} field;
#else
	struct
	{
		UCHAR	TxType:2;
		UCHAR	Tx40MSel:2;
		UCHAR	Rate:2;
		UCHAR	ShortPreamble:1;
		UCHAR	Reserved:1;
	} field;
#endif /* RT_BIG_ENDIAN */

	UCHAR	value;
} CCK_TSSI_INFO, *PCCK_TSSI_INFO;


/* 802.11a/g OFDM TSSI information */

typedef union _OFDM_TSSI_INFO
{
#ifdef RT_BIG_ENDIAN
	struct
	{
		UCHAR	Rate:4;
		UCHAR	Tx40MSel:2;
		UCHAR	TxType:2;
	} field;
#else
	struct
	{
		UCHAR	TxType:2;
		UCHAR	Tx40MSel:2;
		UCHAR	Rate:4;
	} field;
#endif /* RT_BIG_ENDIAN */

	UCHAR	value;
} OFDM_TSSI_INFO, *POFDM_TSSI_INFO;


/* 802.11n HT TSSI information */

typedef struct _HT_TSSI_INFO {
	union {
#ifdef RT_BIG_ENDIAN
		struct {
			UCHAR SGI:1;
			UCHAR STBC:2;
			UCHAR Aggregation:1;
			UCHAR Tx40MSel:2;
			UCHAR TxType:2;
		} field;
#else
		struct {
			UCHAR TxType:2;
			UCHAR Tx40MSel:2;
			UCHAR Aggregation:1;
			UCHAR STBC:2;
			UCHAR SGI:1;
		} field;
#endif /* RT_BIG_ENDIAN */

		UCHAR value;
	} PartA;

	union {
#ifdef RT_BIG_ENDIAN
		struct {
			UCHAR BW:1;
			UCHAR MCS:7;
		} field;
#else
		struct {
			UCHAR MCS:7;
			UCHAR BW:1;
		} field;
#endif /* RT_BIG_ENDIAN */
		UCHAR	value;
	} PartB;
} HT_TSSI_INFO, *PHT_TSSI_INFO;

typedef struct _TSSI_INFO_{
	UCHAR tssi_info_0;
	union {
		CCK_TSSI_INFO cck_tssi_info;
		OFDM_TSSI_INFO ofdm_tssi_info;
		HT_TSSI_INFO ht_tssi_info_1;
		UCHAR byte;
	}tssi_info_1;
	HT_TSSI_INFO ht_tssi_info_2;
}TSSI_INFO;

#endif /* RTMP_INTERNAL_TX_ALC || RTMP_TEMPERATURE_COMPENSATION */


/***************************************************************************
  *	Rx Path software control block related data structures
  **************************************************************************/
typedef struct _RX_BLK_
{
	UCHAR hw_rx_info[RXD_SIZE]; /* include "RXD_STRUC RxD" and "RXINFO_STRUC rx_info " */
	RXINFO_STRUC *pRxInfo;
#ifdef RLT_MAC
	RXFCE_INFO *pRxFceInfo;
#endif /* RLT_MAC */
	RXWI_STRUC *pRxWI;
	PHEADER_802_11 pHeader;
	PNDIS_PACKET pRxPacket;
	UCHAR *pData;
	USHORT DataSize;
	USHORT Flags;
	UCHAR UserPriority;	/* for calculate TKIP MIC using */
	UCHAR OpMode;	/* 0:OPMODE_STA 1:OPMODE_AP */
	UCHAR wcid;		/* copy of pRxWI->RxWIWirelessCliID */
	UCHAR mcs;
	UCHAR U2M;
#ifdef HDR_TRANS_SUPPORT
	BOOLEAN	bHdrRxTrans;	/* this packet's header is translated to 802.3 by HW  */
	BOOLEAN bHdrVlanTaged;	/* VLAN tag is added to this header */
	UCHAR *pTransData;
	USHORT TransDataSize;
#endif /* HDR_TRANS_SUPPORT */
} RX_BLK;


#define RX_BLK_SET_FLAG(_pRxBlk, _flag)		(_pRxBlk->Flags |= _flag)
#define RX_BLK_TEST_FLAG(_pRxBlk, _flag)		(_pRxBlk->Flags & _flag)
#define RX_BLK_CLEAR_FLAG(_pRxBlk, _flag)	(_pRxBlk->Flags &= ~(_flag))


#define fRX_WDS			0x0001
#define fRX_AMSDU		0x0002
#define fRX_ARALINK		0x0004
#define fRX_HTC			0x0008
#define fRX_PAD			0x0010
#define fRX_AMPDU		0x0020
#define fRX_QOS			0x0040
#define fRX_INFRA		0x0080
#define fRX_EAP			0x0100
#define fRX_MESH		0x0200
#define fRX_APCLI		0x0400
#define fRX_DLS			0x0800
#define fRX_WPI			0x1000
#define fRX_P2PGO		0x2000
#define fRX_P2PCLI		0x4000

#define LENGTH_AMSDU_SUBFRAMEHEAD	14
#define LENGTH_ARALINK_SUBFRAMEHEAD	14
#define LENGTH_ARALINK_HEADER_FIELD	 2


/***************************************************************************
  *	Tx Path software control block related data structures
  **************************************************************************/
#define TX_UNKOWN_FRAME		0x00
#define TX_MCAST_FRAME			0x01
#define TX_LEGACY_FRAME		0x02
#define TX_AMPDU_FRAME		0x04
#define TX_AMSDU_FRAME		0x08
#define TX_RALINK_FRAME		0x10
#define TX_FRAG_FRAME			0x20


/*	Currently the sizeof(TX_BLK) is 148 bytes. */
typedef struct _TX_BLK_
{
	UCHAR				QueIdx;
	UCHAR				TxFrameType;				/* Indicate the Transmission type of the all frames in one batch */
	UCHAR				TotalFrameNum;				/* Total frame number want to send-out in one batch */
	USHORT				TotalFragNum;				/* Total frame fragments required in one batch */
	USHORT				TotalFrameLen;				/* Total length of all frames want to send-out in one batch */

	QUEUE_HEADER		TxPacketList;
	MAC_TABLE_ENTRY	*pMacEntry;					/* NULL: packet with 802.11 RA field is multicast/broadcast address */
	HTTRANSMIT_SETTING	*pTransmit;

	/* Following structure used for the characteristics of a specific packet. */
	PNDIS_PACKET		pPacket;
	PUCHAR				pSrcBufHeader;				/* Reference to the head of sk_buff->data */
	PUCHAR				pSrcBufData;				/* Reference to the sk_buff->data, will changed depends on hanlding progresss */
	UINT				SrcBufLen;					/* Length of packet payload which not including Layer 2 header */

	PUCHAR				pExtraLlcSnapEncap;			/* NULL means no extra LLC/SNAP is required */
#ifndef VENDOR_FEATURE1_SUPPORT
	/*
		Note: Can not insert any other new parameters
		between pExtraLlcSnapEncap & HeaderBuf; Or
		the start address of HeaderBuf will not be aligned by 4.

		But we can not change HeaderBuf[128] to HeaderBuf[32] because
		many codes use HeaderBuf[index].
	*/
	UCHAR				HeaderBuf[128];				/* TempBuffer for TX_INFO + TX_WI + TSO_INFO + 802.11 Header + padding + AMSDU SubHeader + LLC/SNAP */
#else
	UINT32				HeaderBuffer[32];			/* total 128B, use UINT32 to avoid alignment problem */
	UCHAR				*HeaderBuf;
#endif /* VENDOR_FEATURE1_SUPPORT */
	UCHAR				MpduHeaderLen;				/* 802.11 header length NOT including the padding */
	UCHAR				HdrPadLen;					/* recording Header Padding Length; */
	UCHAR				apidx;						/* The interface associated to this packet */
	UCHAR				Wcid;						/* The MAC entry associated to this packet */
	UCHAR				UserPriority;				/* priority class of packet */
	UCHAR				FrameGap;					/* what kind of IFS this packet use */
	UCHAR				MpduReqNum;					/* number of fragments of this frame */
	UCHAR				TxRate;						/* TODO: Obsoleted? Should change to MCS? */
	UCHAR				CipherAlg;					/* cipher alogrithm */
	PCIPHER_KEY			pKey;
	UCHAR				KeyIdx;						/* Indicate the transmit key index */


	UINT32				Flags;						/*See following definitions for detail. */

	/*YOU SHOULD NOT TOUCH IT! Following parameters are used for hardware-depended layer. */
	ULONG				Priv;						/* Hardware specific value saved in here. */


#ifdef TXBF_SUPPORT
	UCHAR				TxSndgPkt; /* 1: sounding 2: NDP sounding */
	UCHAR				TxNDPSndgBW;
	UCHAR				TxNDPSndgMcs;
#endif /* TXBF_SUPPORT */

	UCHAR				naf_type;
#ifdef TX_PKT_SG
	PACKET_INFO pkt_info;
#endif /* TX_PKT_SG */
	UCHAR				OpMode;

#ifdef HDR_TRANS_SUPPORT
	BOOLEAN				NeedTrans;	/* indicate the packet needs to do hw header translate */
#endif /* HDR_TRANS_SUPPORT */

} TX_BLK, *PTX_BLK;


#define fTX_bRtsRequired			0x0001	/* Indicate if need send RTS frame for protection. Not used in RT2860/RT2870. */
#define fTX_bAckRequired			0x0002	/* the packet need ack response */
#define fTX_bPiggyBack			0x0004	/* Legacy device use Piggback or not */
#define fTX_bHTRate				0x0008	/* allow to use HT rate */
#define fTX_bForceNonQoS		0x0010	/* force to transmit frame without WMM-QoS in HT mode */
#define fTX_bAllowFrag			0x0020	/* allow to fragment the packet, A-MPDU, A-MSDU, A-Ralink is not allowed to fragment */
#define fTX_bMoreData			0x0040	/* there are more data packets in PowerSave Queue */
#define fTX_bWMM				0x0080	/* QOS Data */
#define fTX_bClearEAPFrame		0x0100

#define	fTX_bSwEncrypt			0x0400	/* this packet need to be encrypted by software before TX */

#ifdef UAPSD_SUPPORT
#define	fTX_bWMM_UAPSD_EOSP	0x0800	/* Used when UAPSD_SUPPORT */
#endif /* UAPSD_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */



#ifdef CLIENT_WDS
#define fTX_bClientWDSFrame		0x10000
#endif /* CLIENT_WDS */


#define TX_BLK_SET_FLAG(_pTxBlk, _flag)		(_pTxBlk->Flags |= _flag)
#define TX_BLK_TEST_FLAG(_pTxBlk, _flag)	(((_pTxBlk->Flags & _flag) == _flag) ? 1 : 0)
#define TX_BLK_CLEAR_FLAG(_pTxBlk, _flag)	(_pTxBlk->Flags &= ~(_flag))




#ifdef RT_BIG_ENDIAN
/***************************************************************************
  *	Endian conversion related functions
  **************************************************************************/
/*
	========================================================================

	Routine Description:
		Endian conversion of Tx/Rx descriptor .

	Arguments:
		pAd 	Pointer to our adapter
		pData			Pointer to Tx/Rx descriptor
		DescriptorType	Direction of the frame

	Return Value:
		None

	Note:
		Call this function when read or update descriptor
	========================================================================
*/
static inline void RTMPWIEndianChange(
	IN	struct rtmp_adapter *pAd,
	IN	PUCHAR			pData,
	IN	ULONG			DescriptorType)
{
	int size;
	int i;
	u8 TXWISize = pAd->chipCap.TXWISize;
	u8 RXWISize = pAd->chipCap.RXWISize;

	size = ((DescriptorType == TYPE_TXWI) ? TXWISize : RXWISize);

	if(DescriptorType == TYPE_TXWI)
	{
		*((UINT32 *)(pData)) = SWAP32(*((UINT32 *)(pData)));		/* Byte 0~3 */
		*((UINT32 *)(pData + 4)) = SWAP32(*((UINT32 *)(pData+4)));	/* Byte 4~7 */
	}
	else
	{
		for(i=0; i < size/4 ; i++)
			*(((UINT32 *)pData) +i) = SWAP32(*(((UINT32 *)pData)+i));
	}
}




/*
	========================================================================

	Routine Description:
		Endian conversion of Tx/Rx descriptor .

	Arguments:
		pAd 	Pointer to our adapter
		pData			Pointer to Tx/Rx descriptor
		DescriptorType	Direction of the frame

	Return Value:
		None

	Note:
		Call this function when read or update descriptor
	========================================================================
*/

#ifdef RTMP_MAC_USB
static inline void RTMPDescriptorEndianChange(UCHAR *pData, ULONG DescType)
{
	*((UINT32 *)(pData)) = SWAP32(*((UINT32 *)(pData)));
}
#endif /* RTMP_MAC_USB */
/*
	========================================================================

	Routine Description:
		Endian conversion of all kinds of 802.11 frames .

	Arguments:
		pAd 	Pointer to our adapter
		pData			Pointer to the 802.11 frame structure
		Dir 			Direction of the frame
		FromRxDoneInt	Caller is from RxDone interrupt

	Return Value:
		None

	Note:
		Call this function when read or update buffer data
	========================================================================
*/
static inline void RTMPFrameEndianChange(
	IN	struct rtmp_adapter *pAd,
	IN	PUCHAR			pData,
	IN	ULONG			Dir,
	IN	BOOLEAN 		FromRxDoneInt)
{
	PHEADER_802_11 pFrame;
	PUCHAR	pMacHdr;

	/* swab 16 bit fields - Frame Control field */
	if(Dir == DIR_READ)
	{
		*(USHORT *)pData = SWAP16(*(USHORT *)pData);
	}

	pFrame = (PHEADER_802_11) pData;
	pMacHdr = (PUCHAR) pFrame;

	/* swab 16 bit fields - Duration/ID field */
	*(USHORT *)(pMacHdr + 2) = SWAP16(*(USHORT *)(pMacHdr + 2));

	if (pFrame->FC.Type != BTYPE_CNTL)
	{
		/* swab 16 bit fields - Sequence Control field */
		*(USHORT *)(pMacHdr + 22) = SWAP16(*(USHORT *)(pMacHdr + 22));
	}

	if(pFrame->FC.Type == BTYPE_MGMT)
	{
		switch(pFrame->FC.SubType)
		{
			case SUBTYPE_ASSOC_REQ:
			case SUBTYPE_REASSOC_REQ:
				/* swab 16 bit fields - CapabilityInfo field */
				pMacHdr += sizeof(HEADER_802_11);
				*(USHORT *)pMacHdr = SWAP16(*(USHORT *)pMacHdr);

				/* swab 16 bit fields - Listen Interval field */
				pMacHdr += 2;
				*(USHORT *)pMacHdr = SWAP16(*(USHORT *)pMacHdr);
				break;

			case SUBTYPE_ASSOC_RSP:
			case SUBTYPE_REASSOC_RSP:
				/* swab 16 bit fields - CapabilityInfo field */
				pMacHdr += sizeof(HEADER_802_11);
				*(USHORT *)pMacHdr = SWAP16(*(USHORT *)pMacHdr);

				/* swab 16 bit fields - Status Code field */
				pMacHdr += 2;
				*(USHORT *)pMacHdr = SWAP16(*(USHORT *)pMacHdr);

				/* swab 16 bit fields - AID field */
				pMacHdr += 2;
				*(USHORT *)pMacHdr = SWAP16(*(USHORT *)pMacHdr);
				break;

			case SUBTYPE_AUTH:
				 /* When the WEP bit is on, don't do the conversion here.
					This is only shared WEP can hit this condition.
					For AP, it shall do conversion after decryption.
					For STA, it shall do conversion before encryption. */
				if (pFrame->FC.Wep == 1)
					break;
				else
				{
					/* swab 16 bit fields - Auth Alg No. field */
					pMacHdr += sizeof(HEADER_802_11);
					*(USHORT *)pMacHdr = SWAP16(*(USHORT *)pMacHdr);

					/* swab 16 bit fields - Auth Seq No. field */
					pMacHdr += 2;
					*(USHORT *)pMacHdr = SWAP16(*(USHORT *)pMacHdr);

					/* swab 16 bit fields - Status Code field */
					pMacHdr += 2;
					*(USHORT *)pMacHdr = SWAP16(*(USHORT *)pMacHdr);
				}
				break;

			case SUBTYPE_BEACON:
			case SUBTYPE_PROBE_RSP:
				/* swab 16 bit fields - BeaconInterval field */
				pMacHdr += (sizeof(HEADER_802_11) + TIMESTAMP_LEN);
				*(USHORT *)pMacHdr = SWAP16(*(USHORT *)pMacHdr);

				/* swab 16 bit fields - CapabilityInfo field */
				pMacHdr += sizeof(USHORT);
				*(USHORT *)pMacHdr = SWAP16(*(USHORT *)pMacHdr);
				break;

			case SUBTYPE_DEAUTH:
			case SUBTYPE_DISASSOC:
				/* If the PMF is negotiated, those frames shall be encrypted */
				if(!FromRxDoneInt && pFrame->FC.Wep == 1)
					break;
				else
				{
					/* swab 16 bit fields - Reason code field */
					pMacHdr += sizeof(HEADER_802_11);
					*(USHORT *)pMacHdr = SWAP16(*(USHORT *)pMacHdr);
				}
				break;
		}
	}
	else if( pFrame->FC.Type == BTYPE_DATA )
	{
	}
	else if(pFrame->FC.Type == BTYPE_CNTL)
	{
		switch(pFrame->FC.SubType)
		{
			case SUBTYPE_BLOCK_ACK_REQ:
				{
					PFRAME_BA_REQ pBAReq = (PFRAME_BA_REQ)pFrame;
					*(USHORT *)(&pBAReq->BARControl) = SWAP16(*(USHORT *)(&pBAReq->BARControl));
					pBAReq->BAStartingSeq.word = SWAP16(pBAReq->BAStartingSeq.word);
				}
				break;
			case SUBTYPE_BLOCK_ACK:
				/* For Block Ack packet, the HT_CONTROL field is in the same offset with Addr3 */
				*(UINT32 *)(&pFrame->Addr3[0]) = SWAP32(*(UINT32 *)(&pFrame->Addr3[0]));
				break;

			case SUBTYPE_ACK:
				/*For ACK packet, the HT_CONTROL field is in the same offset with Addr2 */
				*(UINT32 *)(&pFrame->Addr2[0])=	SWAP32(*(UINT32 *)(&pFrame->Addr2[0]));
				break;
		}
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR,("Invalid Frame Type!!!\n"));
	}

	/* swab 16 bit fields - Frame Control */
	if(Dir == DIR_WRITE)
	{
		*(USHORT *)pData = SWAP16(*(USHORT *)pData);
	}
}
#endif /* RT_BIG_ENDIAN */


/***************************************************************************
  *	Other static inline function definitions
  **************************************************************************/
static inline void ConvertMulticastIP2MAC(
	IN PUCHAR pIpAddr,
	IN PUCHAR *ppMacAddr,
	IN UINT16 ProtoType)
{
	if (pIpAddr == NULL)
		return;

	if (ppMacAddr == NULL || *ppMacAddr == NULL)
		return;

	switch (ProtoType)
	{
		case ETH_P_IPV6:
/*			memset(*ppMacAddr, 0, ETH_LENGTH_OF_ADDRESS); */
			*(*ppMacAddr) = 0x33;
			*(*ppMacAddr + 1) = 0x33;
			*(*ppMacAddr + 2) = pIpAddr[12];
			*(*ppMacAddr + 3) = pIpAddr[13];
			*(*ppMacAddr + 4) = pIpAddr[14];
			*(*ppMacAddr + 5) = pIpAddr[15];
			break;

		case ETH_P_IP:
		default:
/*			memset(*ppMacAddr, 0, ETH_LENGTH_OF_ADDRESS); */
			*(*ppMacAddr) = 0x01;
			*(*ppMacAddr + 1) = 0x00;
			*(*ppMacAddr + 2) = 0x5e;
			*(*ppMacAddr + 3) = pIpAddr[1] & 0x7f;
			*(*ppMacAddr + 4) = pIpAddr[2];
			*(*ppMacAddr + 5) = pIpAddr[3];
			break;
	}

	return;
}


char *get_phymode_str(int phy_mode);
char *get_bw_str(int bandwidth);


BOOLEAN RTMPCheckForHang(
	IN  NDIS_HANDLE MiniportAdapterContext);

/*
	Private routines in rtmp_init.c
*/
NDIS_STATUS RTMPAllocTxRxRingMemory(
	IN  struct rtmp_adapter *  pAd);

#ifdef RESOURCE_PRE_ALLOC
NDIS_STATUS RTMPInitTxRxRingMemory(
	IN struct rtmp_adapter*pAd);
#endif /* RESOURCE_PRE_ALLOC */

NDIS_STATUS	RTMPReadParametersHook(
	IN	struct rtmp_adapter *pAd);

NDIS_STATUS	RTMPSetProfileParameters(
	IN struct rtmp_adapter*pAd,
	IN char *	pBuffer);

#ifdef SINGLE_SKU_V2
NDIS_STATUS	RTMPSetSingleSKUParameters(
	IN struct rtmp_adapter*pAd);
#endif /* SINGLE_SKU_V2 */

INT RTMPGetKeyParameter(
    IN char *key,
    OUT char *dest,
    IN INT destsize,
    IN char *buffer,
    IN BOOLEAN bTrimSpace);




#ifdef RLT_RF
NDIS_STATUS rlt_rf_write(
	IN struct rtmp_adapter*pAd,
	IN UCHAR bank,
	IN UCHAR regID,
	IN UCHAR value);

NDIS_STATUS rlt_rf_read(
	IN struct rtmp_adapter*pAd,
	IN UCHAR bank,
	IN UCHAR regID,
	IN UCHAR *pValue);
#endif /* RLT_RF */

void NICReadEEPROMParameters(
	IN  struct rtmp_adapter *pAd);

void NICInitAsicFromEEPROM(
	IN  struct rtmp_adapter *      pAd);

NDIS_STATUS NICInitializeAdapter(
	IN  struct rtmp_adapter *  pAd,
	IN   BOOLEAN    bHardReset);

NDIS_STATUS NICInitializeAsic(
	IN  struct rtmp_adapter *  pAd,
	IN  BOOLEAN		bHardReset);


void RTMPRingCleanUp(
	IN  struct rtmp_adapter *  pAd,
	IN  UCHAR           RingType);

void UserCfgExit(
	IN  struct rtmp_adapter*pAd);

void UserCfgInit(
	IN  struct rtmp_adapter *  pAd);

NDIS_STATUS NICLoadFirmware(
	IN  struct rtmp_adapter *  pAd);

void NICEraseFirmware(
	IN struct rtmp_adapter *pAd);

void NICUpdateFifoStaCounters(
	IN struct rtmp_adapter *pAd);

void NICUpdateRawCounters(
	IN  struct rtmp_adapter *  pAd);

#ifdef FIFO_EXT_SUPPORT
BOOLEAN NicGetMacFifoTxCnt(
	IN struct rtmp_adapter*pAd,
	IN MAC_TABLE_ENTRY *pEntry);

void AsicFifoExtSet(
	IN struct rtmp_adapter*pAd);

void AsicFifoExtEntryClean(
	IN struct rtmp_adapter* pAd,
	IN MAC_TABLE_ENTRY *pEntry);
#endif /* FIFO_EXT_SUPPORT */

void NicResetRawCounters(struct rtmp_adapter*pAd);

void NicGetTxRawCounters(
	IN struct rtmp_adapter*pAd,
	IN TX_STA_CNT0_STRUC *pStaTxCnt0,
	IN TX_STA_CNT1_STRUC *pStaTxCnt1);

void RTMPZeroMemory(
	IN  void *  pSrc,
	IN  ULONG   Length);

ULONG RTMPCompareMemory(
	IN  void *  pSrc1,
	IN  void *  pSrc2,
	IN  ULONG   Length);

void RTMPMoveMemory(
	OUT void *  pDest,
	IN  void *  pSrc,
	IN  ULONG   Length);

void AtoH(
	char *src,
	PUCHAR dest,
	int		destlen);

UCHAR BtoH(
	char ch);

void RTMP_TimerListAdd(
	IN	struct rtmp_adapter *		pAd,
	IN	void 				*pRsc);

void RTMP_TimerListRelease(
	IN	struct rtmp_adapter *		pAd);

void RTMPInitTimer(
	IN  struct rtmp_adapter *          pAd,
	IN  PRALINK_TIMER_STRUCT    pTimer,
	IN  void *                  pTimerFunc,
	IN	void *				pData,
	IN  BOOLEAN                 Repeat);

void RTMPSetTimer(
	IN  PRALINK_TIMER_STRUCT    pTimer,
	IN  ULONG                   Value);


void RTMPModTimer(
	IN	PRALINK_TIMER_STRUCT	pTimer,
	IN	ULONG					Value);

void RTMPCancelTimer(
	IN  PRALINK_TIMER_STRUCT    pTimer,
	OUT BOOLEAN                 *pCancelled);

void RTMPReleaseTimer(
	IN  PRALINK_TIMER_STRUCT    pTimer,
	OUT BOOLEAN                 *pCancelled);

void RTMPEnableRxTx(
	IN struct rtmp_adapter *pAd);

void AntCfgInit(
	IN  struct rtmp_adapter *  pAd);

/* */
/* prototype in action.c */
/* */
void ActionStateMachineInit(
    IN	struct rtmp_adapter *pAd,
    IN  STATE_MACHINE *S,
    OUT STATE_MACHINE_FUNC Trans[]);

void MlmeADDBAAction(
    IN struct rtmp_adapter *pAd,
    IN MLME_QUEUE_ELEM *Elem);

void MlmeDELBAAction(
    IN struct rtmp_adapter *pAd,
    IN MLME_QUEUE_ELEM *Elem);

void MlmeDLSAction(
    IN struct rtmp_adapter *pAd,
    IN MLME_QUEUE_ELEM *Elem);

void MlmeInvalidAction(
    IN struct rtmp_adapter *pAd,
    IN MLME_QUEUE_ELEM *Elem);

void MlmeQOSAction(
    IN struct rtmp_adapter *pAd,
    IN MLME_QUEUE_ELEM *Elem);

#ifdef DOT11_N_SUPPORT
void PeerAddBAReqAction(
	IN struct rtmp_adapter *pAd,
	IN MLME_QUEUE_ELEM *Elem);

void PeerAddBARspAction(
	IN struct rtmp_adapter *pAd,
	IN MLME_QUEUE_ELEM *Elem);

void PeerDelBAAction(
	IN struct rtmp_adapter *pAd,
	IN MLME_QUEUE_ELEM *Elem);

void PeerBAAction(
    IN struct rtmp_adapter *pAd,
    IN MLME_QUEUE_ELEM *Elem);
#endif /* DOT11_N_SUPPORT */

void SendPSMPAction(
	IN struct rtmp_adapter *pAd,
	IN UCHAR			Wcid,
	IN UCHAR			Psmp);


void PeerRMAction(
	IN struct rtmp_adapter *pAd,
	IN MLME_QUEUE_ELEM *Elem);

void PeerPublicAction(
	IN struct rtmp_adapter *pAd,
	IN MLME_QUEUE_ELEM *Elem);

#ifdef CONFIG_STA_SUPPORT
void StaPublicAction(
	IN struct rtmp_adapter *pAd,
	IN BSS_2040_COEXIST_IE *pBss2040CoexIE);
#endif /* CONFIG_STA_SUPPORT */


void PeerBSSTranAction(
	IN struct rtmp_adapter *pAd,
	IN MLME_QUEUE_ELEM *Elem);

#ifdef DOT11_N_SUPPORT
void PeerHTAction(
	IN struct rtmp_adapter *pAd,
	IN MLME_QUEUE_ELEM *Elem);
#endif /* DOT11_N_SUPPORT */

void PeerQOSAction(
    IN struct rtmp_adapter *pAd,
    IN MLME_QUEUE_ELEM *Elem);

#ifdef QOS_DLS_SUPPORT
void PeerDLSAction(
    IN struct rtmp_adapter *pAd,
    IN MLME_QUEUE_ELEM *Elem);
#endif /* QOS_DLS_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
#ifdef QOS_DLS_SUPPORT
void DlsParmFill(
	IN struct rtmp_adapter *pAd,
	IN OUT MLME_DLS_REQ_STRUCT *pDlsReq,
	IN PRT_802_11_DLS pDls,
	IN USHORT reason);
#endif /* QOS_DLS_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

#ifdef DOT11_N_SUPPORT
void RECBATimerTimeout(
    IN void *SystemSpecific1,
    IN void *FunctionContext,
    IN void *SystemSpecific2,
    IN void *SystemSpecific3);

void ORIBATimerTimeout(
	IN	struct rtmp_adapter *pAd);

void SendRefreshBAR(
	IN	struct rtmp_adapter *pAd,
	IN	MAC_TABLE_ENTRY	*pEntry);

#ifdef DOT11N_DRAFT3
void RTMP_11N_D3_TimerInit(
	IN struct rtmp_adapter *pAd);

void SendBSS2040CoexistMgmtAction(
	IN	struct rtmp_adapter *pAd,
	IN	UCHAR	Wcid,
	IN	UCHAR	apidx,
	IN	UCHAR	InfoReq);

void SendNotifyBWActionFrame(
	IN struct rtmp_adapter *pAd,
	IN UCHAR  Wcid,
	IN UCHAR apidx);

BOOLEAN ChannelSwitchSanityCheck(
	IN	struct rtmp_adapter *pAd,
	IN    UCHAR  Wcid,
	IN    UCHAR  NewChannel,
	IN    UCHAR  Secondary);

void ChannelSwitchAction(
	IN	struct rtmp_adapter *pAd,
	IN    UCHAR  Wcid,
	IN    UCHAR  Channel,
	IN    UCHAR  Secondary);

ULONG BuildIntolerantChannelRep(
	IN	struct rtmp_adapter *pAd,
	IN    PUCHAR  pDest);

void Update2040CoexistFrameAndNotify(
	IN	struct rtmp_adapter *pAd,
	IN    UCHAR  Wcid,
	IN	BOOLEAN	bAddIntolerantCha);

void Send2040CoexistAction(
	IN	struct rtmp_adapter *pAd,
	IN    UCHAR  Wcid,
	IN	BOOLEAN	bAddIntolerantCha);

void UpdateBssScanParm(
	IN struct rtmp_adapter *pAd,
	IN OVERLAP_BSS_SCAN_IE APBssScan);
#endif /* DOT11N_DRAFT3 */

void AsicEnableRalinkBurstMode(
	IN struct rtmp_adapter *pAd);

void AsicDisableRalinkBurstMode(
	IN struct rtmp_adapter *pAd);
#endif /* DOT11_N_SUPPORT */

void ActHeaderInit(
    IN	struct rtmp_adapter *pAd,
    IN OUT PHEADER_802_11 pHdr80211,
    IN PUCHAR Addr1,
    IN PUCHAR Addr2,
    IN PUCHAR Addr3);

void BarHeaderInit(
	IN	struct rtmp_adapter *pAd,
	IN OUT PFRAME_BAR pCntlBar,
	IN PUCHAR pDA,
	IN PUCHAR pSA);

void InsertActField(
	IN struct rtmp_adapter *pAd,
	OUT PUCHAR pFrameBuf,
	OUT PULONG pFrameLen,
	IN u8 Category,
	IN u8 ActCode);

BOOLEAN QosBADataParse(
	IN struct rtmp_adapter *pAd,
	IN BOOLEAN bAMSDU,
	IN PUCHAR p8023Header,
	IN UCHAR	WCID,
	IN UCHAR	TID,
	IN USHORT Sequence,
	IN UCHAR DataOffset,
	IN USHORT Datasize,
	IN UINT   CurRxIndex);

#ifdef DOT11_N_SUPPORT
BOOLEAN CntlEnqueueForRecv(
    IN	struct rtmp_adapter *pAd,
	IN ULONG Wcid,
    IN ULONG MsgLen,
	IN PFRAME_BA_REQ pMsg);

void BaAutoManSwitch(
	IN	struct rtmp_adapter *pAd);
#endif /* DOT11_N_SUPPORT */

void HTIOTCheck(
	IN	struct rtmp_adapter *pAd,
	IN    UCHAR     BatRecIdx);

/* */
/* Private routines in rtmp_data.c */
/* */
BOOLEAN RTMPHandleTxRingDmaDoneInterrupt(
	IN struct rtmp_adapter *pAd,
	IN UINT32 int_reg);

void RTMPHandleMgmtRingDmaDoneInterrupt(
	IN struct rtmp_adapter *pAd);

void RTMPHandleTBTTInterrupt(
	IN struct rtmp_adapter *pAd);

void RTMPHandlePreTBTTInterrupt(
	IN struct rtmp_adapter *pAd);

void RTMPHandleTwakeupInterrupt(
	IN struct rtmp_adapter *pAd);

void RTMPHandleRxCoherentInterrupt(
	IN struct rtmp_adapter *pAd);



NDIS_STATUS STASendPacket(
	IN  struct rtmp_adapter *  pAd,
	IN  PNDIS_PACKET    pPacket);

void STASendPackets(
	IN  NDIS_HANDLE     MiniportAdapterContext,
	IN  PPNDIS_PACKET   ppPacketArray,
	IN  UINT            NumberOfPackets);

void RTMPDeQueuePacket(
	IN struct rtmp_adapter*pAd,
   	IN BOOLEAN bIntContext,
	IN UCHAR QueIdx,
	IN INT Max_Tx_Packets);

NDIS_STATUS	RTMPHardTransmit(
	IN struct rtmp_adapter *pAd,
	IN PNDIS_PACKET		pPacket,
	IN  UCHAR			QueIdx,
	OUT	PULONG			pFreeTXDLeft);

NDIS_STATUS	STAHardTransmit(
	IN struct rtmp_adapter *pAd,
	IN TX_BLK			*pTxBlk,
	IN  UCHAR			QueIdx);

void STARxEAPOLFrameIndicate(
	IN	struct rtmp_adapter *pAd,
	IN	MAC_TABLE_ENTRY	*pEntry,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);

NDIS_STATUS RTMPFreeTXDRequest(
	IN  struct rtmp_adapter *  pAd,
	IN  UCHAR           RingType,
	IN  UCHAR           NumberRequired,
	IN 	PUCHAR          FreeNumberIs);

NDIS_STATUS MlmeHardTransmit(
	IN  struct rtmp_adapter *  pAd,
	IN  UCHAR	QueIdx,
	IN  PNDIS_PACKET    pPacket,
	IN	BOOLEAN			FlgDataQForce,
	IN	BOOLEAN			FlgIsLocked);

NDIS_STATUS MlmeHardTransmitMgmtRing(
	IN  struct rtmp_adapter *  pAd,
	IN  UCHAR	QueIdx,
	IN  PNDIS_PACKET    pPacket);


USHORT RTMPCalcDuration(
	IN struct rtmp_adapter*pAd,
	IN UCHAR Rate,
	IN ULONG Size);

void RTMPWriteTxWI(
	IN struct rtmp_adapter*pAd,
	IN TXWI_STRUC *pTxWI,
	IN BOOLEAN FRAG,
	IN BOOLEAN CFACK,
	IN BOOLEAN InsTimestamp,
	IN BOOLEAN AMPDU,
	IN BOOLEAN Ack,
	IN BOOLEAN NSeq, /* HW new a sequence. */
	IN UCHAR BASize,
	IN UCHAR WCID,
	IN ULONG Length,
	IN UCHAR PID,
	IN UCHAR TID,
	IN UCHAR TxRate,
	IN UCHAR Txopmode,
	IN BOOLEAN CfAck,
	IN HTTRANSMIT_SETTING *pTransmit);


void RTMPWriteTxWI_Data(
	IN struct rtmp_adapter*pAd,
	INOUT TXWI_STRUC *pTxWI,
	IN TX_BLK *pTxBlk);


void RTMPWriteTxWI_Cache(
	IN struct rtmp_adapter*pAd,
	INOUT TXWI_STRUC *pTxWI,
	IN TX_BLK *pTxBlk);

void RTMPSuspendMsduTransmission(
	IN struct rtmp_adapter*pAd);

void RTMPResumeMsduTransmission(
	IN struct rtmp_adapter*pAd);

NDIS_STATUS MiniportMMRequest(
	IN struct rtmp_adapter*pAd,
	IN UCHAR QueIdx,
	IN UCHAR *pData,
	IN UINT Length);

void RTMPSendNullFrame(
	IN struct rtmp_adapter*pAd,
	IN UCHAR TxRate,
	IN BOOLEAN bQosNull,
	IN USHORT PwrMgmt);

#ifdef CONFIG_STA_SUPPORT
void RTMPReportMicError(
	IN  struct rtmp_adapter *  pAd,
	IN  PCIPHER_KEY     pWpaKey);

void WpaMicFailureReportFrame(
	IN  struct rtmp_adapter *   pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void    WpaDisassocApAndBlockAssoc(
    IN  void *SystemSpecific1,
    IN  void *FunctionContext,
    IN  void *SystemSpecific2,
    IN  void *SystemSpecific3);

void WpaStaPairwiseKeySetting(
	IN	struct rtmp_adapter *pAd);

void WpaStaGroupKeySetting(
	IN	struct rtmp_adapter *pAd);

void    WpaSendEapolStart(
	IN	struct rtmp_adapter *pAdapter,
	IN  PUCHAR          pBssid);

#endif /* CONFIG_STA_SUPPORT */



BOOLEAN RTMPFreeTXDUponTxDmaDone(
	IN struct rtmp_adapter *   pAd,
	IN UCHAR            QueIdx);

BOOLEAN RTMPCheckEtherType(
	IN	struct rtmp_adapter *pAd,
	IN	PNDIS_PACKET	pPacket,
	IN	PMAC_TABLE_ENTRY pMacEntry,
	IN	UCHAR			OpMode,
	OUT PUCHAR pUserPriority,
	OUT PUCHAR pQueIdx);


void RTMPCckBbpTuning(
	IN	struct rtmp_adapter *pAd,
	IN	UINT			TxRate);
/* */
/* MLME routines */
/* */

/* Asic/RF/BBP related functions */
void AsicGetTxPowerOffset(
	IN struct rtmp_adapter *			pAd,
	IN PULONG					TxPwr);

void AsicGetAutoAgcOffsetForExternalTxAlc(
	IN struct rtmp_adapter *		pAd,
	IN char *				pDeltaPwr,
	IN char *				pTotalDeltaPwr,
	IN char *				pAgcCompensate,
	IN char *				pDeltaPowerByBbpR1);

void AsicExtraPowerOverMAC(struct rtmp_adapter*pAd);

#ifdef RTMP_TEMPERATURE_COMPENSATION
void AsicGetAutoAgcOffsetForTemperatureSensor(
	IN struct rtmp_adapter *		pAd,
	IN char *			pDeltaPwr,
	IN char *			pTotalDeltaPwr,
	IN char *			pAgcCompensate,
	IN char *				pDeltaPowerByBbpR1);
#endif /* RTMP_TEMPERATURE_COMPENSATION */

#ifdef SINGLE_SKU
void GetSingleSkuDeltaPower(
	IN 		struct rtmp_adapter *	pAd,
	IN 		char *			pTotalDeltaPower,
	INOUT 	PULONG			pSingleSKUTotalDeltaPwr,
	INOUT  	PUCHAR              	pSingleSKUBbpR1Offset);
#endif /* SINGLE_SKU*/

void AsicPercentageDeltaPower(
	IN 		struct rtmp_adapter *		pAd,
	IN		CHAR				Rssi,
	INOUT	char *			pDeltaPwr,
	INOUT	char *			pDeltaPowerByBbpR1);

void AsicCompensatePowerViaBBP(
	IN 		struct rtmp_adapter *		pAd,
	INOUT	char *			pTotalDeltaPower);

void AsicAdjustTxPower(
	IN struct rtmp_adapter *pAd);

void 	AsicUpdateProtect(
	IN		struct rtmp_adapter *pAd,
	IN 		USHORT			OperaionMode,
	IN 		UCHAR			SetMask,
	IN		BOOLEAN			bDisableBGProtect,
	IN		BOOLEAN			bNonGFExist);

void AsicBBPAdjust(
	IN struct rtmp_adapter*pAd);

void AsicSwitchChannel(
	IN  struct rtmp_adapter *  pAd,
	IN	UCHAR			Channel,
	IN	BOOLEAN			bScan);

INT AsicSetChannel(
	IN struct rtmp_adapter*pAd,
	IN UCHAR ch,
	IN UCHAR bw,
	IN UCHAR ext_ch,
	IN BOOLEAN bScan);

void AsicLockChannel(
	IN struct rtmp_adapter *pAd,
	IN UCHAR Channel) ;

void AsicAntennaSelect(
	IN  struct rtmp_adapter *  pAd,
	IN  UCHAR           Channel);

void AsicResetBBPAgent(
	IN struct rtmp_adapter *pAd);

#ifdef CONFIG_STA_SUPPORT
void AsicSleepThenAutoWakeup(
	IN  struct rtmp_adapter *  pAd,
	IN  USHORT TbttNumToNextWakeUp);

void AsicForceSleep(
	IN struct rtmp_adapter *pAd);

void AsicForceWakeup(
	IN struct rtmp_adapter *pAd,
	IN BOOLEAN    bFromTx);
#endif /* CONFIG_STA_SUPPORT */

void AsicSetBssid(
	IN  struct rtmp_adapter *  pAd,
	IN  PUCHAR pBssid);

void AsicDelWcidTab(
	IN struct rtmp_adapter *pAd,
	IN UCHAR	Wcid);

#ifdef RLT_MAC
void AsicSetApCliBssid(
	IN struct rtmp_adapter *pAd,
	IN PUCHAR pBssid,
	IN UCHAR index);

#ifdef MAC_REPEATER_SUPPORT
void AsicSetExtendedMacAddr(
	IN struct rtmp_adapter *pAd,
	IN PUCHAR pMacAddr,
	IN UINT32 Idx);
#endif /* MAC_REPEATER_SUPPORT */
#endif /* RLT_MAC */

#ifdef DOT11_N_SUPPORT
void AsicEnableRDG(
	IN struct rtmp_adapter *pAd);

void AsicDisableRDG(
	IN struct rtmp_adapter *pAd);
#endif /* DOT11_N_SUPPORT */

void AsicDisableSync(
	IN  struct rtmp_adapter *  pAd);

void AsicEnableBssSync(
	IN  struct rtmp_adapter *  pAd);

void AsicEnableIbssSync(
	IN  struct rtmp_adapter *  pAd);

void AsicSetEdcaParm(
	IN struct rtmp_adapter *pAd,
	IN PEDCA_PARM    pEdcaParm);

void AsicSetSlotTime(
	IN struct rtmp_adapter *pAd,
	IN BOOLEAN bUseShortSlotTime);

void AsicAddSharedKeyEntry(
	IN struct rtmp_adapter *pAd,
	IN UCHAR         BssIndex,
	IN UCHAR         KeyIdx,
	IN PCIPHER_KEY	 pCipherKey);

void AsicRemoveSharedKeyEntry(
	IN struct rtmp_adapter *pAd,
	IN UCHAR         BssIndex,
	IN UCHAR         KeyIdx);

void AsicUpdateWCIDIVEIV(
	IN struct rtmp_adapter *pAd,
	IN USHORT		WCID,
	IN ULONG        uIV,
	IN ULONG        uEIV);

void AsicUpdateRxWCIDTable(
	IN struct rtmp_adapter *pAd,
	IN USHORT		WCID,
	IN PUCHAR        pAddr);

void AsicUpdateWcidAttributeEntry(
	IN	struct rtmp_adapter *pAd,
	IN	UCHAR			BssIdx,
	IN 	UCHAR		 	KeyIdx,
	IN 	UCHAR		 	CipherAlg,
	IN	u8				Wcid,
	IN	u8				KeyTabFlag);

void AsicAddPairwiseKeyEntry(
	IN struct rtmp_adapter *	pAd,
	IN UCHAR			WCID,
	IN PCIPHER_KEY		pCipherKey);

void AsicRemovePairwiseKeyEntry(
	IN struct rtmp_adapter * pAd,
	IN UCHAR		 Wcid);

BOOLEAN AsicSendCommandToMcu(
	IN struct rtmp_adapter *pAd,
	IN UCHAR         Command,
	IN UCHAR         Token,
	IN UCHAR         Arg0,
	IN UCHAR         Arg1,
	IN BOOLEAN in_atomic);

BOOLEAN AsicSendCommandToMcuBBP(
	IN struct rtmp_adapter *pAd,
	IN UCHAR		 Command,
	IN UCHAR		 Token,
	IN UCHAR		 Arg0,
	IN UCHAR		 Arg1,
	IN BOOLEAN		FlgIsNeedLocked);





#ifdef STREAM_MODE_SUPPORT
UINT32 StreamModeRegVal(
	IN struct rtmp_adapter*pAd);

void AsicSetStreamMode(
	IN struct rtmp_adapter*pAd,
	IN PUCHAR pMacAddr,
	IN INT chainIdx,
	IN BOOLEAN bEnabled);

void RtmpStreamModeInit(
	IN struct rtmp_adapter*pAd);

/*
	Update the Tx chain address
	Parameters
		pAd: The adapter data structure
		pMacAddress: The MAC address of the peer STA

	Return Value:
		None
*/
void AsicUpdateTxChainAddress(
	IN struct rtmp_adapter *pAd,
	IN PUCHAR pMacAddress);

INT Set_StreamMode_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *        arg);

INT Set_StreamModeMac_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *         arg);

INT Set_StreamModeMCS_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *        arg);
#endif /* STREAM_MODE_SUPPORT */

#ifdef WOW_SUPPORT
#ifdef RTMP_MAC_USB
/* For WOW, 8051 MUC send full frame */
void AsicWOWSendNullFrame(
    IN struct rtmp_adapter *pAd,
    IN UCHAR TxRate,
    IN BOOLEAN bQosNull);

void AsicLoadWOWFirmware(
    IN struct rtmp_adapter *pAd,
    IN BOOLEAN WOW);
#endif /* RTMP_MAC_USB */
#endif /* WOW_SUPPORT */

void MacAddrRandomBssid(
	IN  struct rtmp_adapter *  pAd,
	OUT PUCHAR pAddr);

void MgtMacHeaderInit(
	IN  struct rtmp_adapter *    pAd,
	IN OUT PHEADER_802_11 pHdr80211,
	IN UCHAR SubType,
	IN UCHAR ToDs,
	IN PUCHAR pDA,
	IN PUCHAR pBssid);

void MlmeRadioOff(
	IN struct rtmp_adapter *pAd);

void MlmeRadioOn(
	IN struct rtmp_adapter *pAd);


void BssTableInit(
	IN BSS_TABLE *Tab);

#ifdef DOT11_N_SUPPORT
void BATableInit(
	IN struct rtmp_adapter *pAd,
    IN BA_TABLE *Tab);

void BATableExit(
	IN struct rtmp_adapter*pAd);
#endif /* DOT11_N_SUPPORT */

ULONG BssTableSearch(
	IN BSS_TABLE *Tab,
	IN PUCHAR pBssid,
	IN UCHAR Channel);

ULONG BssSsidTableSearch(
	IN BSS_TABLE *Tab,
	IN PUCHAR    pBssid,
	IN PUCHAR    pSsid,
	IN UCHAR     SsidLen,
	IN UCHAR     Channel);

ULONG BssTableSearchWithSSID(
	IN BSS_TABLE *Tab,
	IN PUCHAR    Bssid,
	IN PUCHAR    pSsid,
	IN UCHAR     SsidLen,
	IN UCHAR     Channel);

ULONG BssSsidTableSearchBySSID(
	IN BSS_TABLE *Tab,
	IN PUCHAR	 pSsid,
	IN UCHAR	 SsidLen);

void BssTableDeleteEntry(
	IN OUT  PBSS_TABLE pTab,
	IN      PUCHAR pBssid,
	IN      UCHAR Channel);

ULONG BssTableSetEntry(
	IN struct rtmp_adapter *pAd,
	OUT BSS_TABLE *Tab,
	IN BCN_IE_LIST *ie_list,
	IN CHAR Rssi,
	IN USHORT LengthVIE,
	IN PNDIS_802_11_VARIABLE_IEs pVIE);


#ifdef DOT11_N_SUPPORT
void BATableInsertEntry(
    IN	struct rtmp_adapter *pAd,
	IN USHORT Aid,
    IN USHORT		TimeOutValue,
	IN USHORT		StartingSeq,
    IN UCHAR TID,
	IN UCHAR BAWinSize,
	IN UCHAR OriginatorStatus,
    IN BOOLEAN IsRecipient);

#ifdef DOT11N_DRAFT3
void Bss2040CoexistTimeOut(
	IN void *SystemSpecific1,
	IN void *FunctionContext,
	IN void *SystemSpecific2,
	IN void *SystemSpecific3);


void  TriEventInit(
	IN	struct rtmp_adapter *pAd);

INT TriEventTableSetEntry(
	IN	struct rtmp_adapter *pAd,
	OUT TRIGGER_EVENT_TAB *Tab,
	IN PUCHAR pBssid,
	IN HT_CAPABILITY_IE *pHtCapability,
	IN UCHAR			HtCapabilityLen,
	IN UCHAR			RegClass,
	IN UCHAR ChannelNo);

#endif /* DOT11N_DRAFT3 */
#endif /* DOT11_N_SUPPORT */

void BssTableSsidSort(
	IN  struct rtmp_adapter *  pAd,
	OUT BSS_TABLE *OutTab,
	IN  CHAR Ssid[],
	IN  UCHAR SsidLen);

void  BssTableSortByRssi(
	IN OUT BSS_TABLE *OutTab);

void BssCipherParse(
	IN OUT  PBSS_ENTRY  pBss);

NDIS_STATUS  MlmeQueueInit(
	IN struct rtmp_adapter *pAd,
	IN MLME_QUEUE *Queue);

void  MlmeQueueDestroy(
	IN MLME_QUEUE *Queue);

BOOLEAN MlmeEnqueue(
	IN struct rtmp_adapter *pAd,
	IN ULONG Machine,
	IN ULONG MsgType,
	IN ULONG MsgLen,
	IN void *Msg,
	IN ULONG Priv);

BOOLEAN MlmeEnqueueForRecv(
	IN  struct rtmp_adapter *  pAd,
	IN ULONG Wcid,
	IN ULONG TimeStampHigh,
	IN ULONG TimeStampLow,
	IN UCHAR Rssi0,
	IN UCHAR Rssi1,
	IN UCHAR Rssi2,
	IN ULONG MsgLen,
	IN void *Msg,
	IN UCHAR Signal,
	IN UCHAR OpMode);


BOOLEAN MlmeDequeue(
	IN MLME_QUEUE *Queue,
	OUT MLME_QUEUE_ELEM **Elem);

void    MlmeRestartStateMachine(
	IN  struct rtmp_adapter *  pAd);

BOOLEAN  MlmeQueueEmpty(
	IN MLME_QUEUE *Queue);

BOOLEAN  MlmeQueueFull(
	IN MLME_QUEUE *Queue,
	IN UCHAR SendId);

BOOLEAN  MsgTypeSubst(
	IN struct rtmp_adapter *pAd,
	IN PFRAME_802_11 pFrame,
	OUT INT *Machine,
	OUT INT *MsgType);

void StateMachineInit(
	IN STATE_MACHINE *Sm,
	IN STATE_MACHINE_FUNC Trans[],
	IN ULONG StNr,
	IN ULONG MsgNr,
	IN STATE_MACHINE_FUNC DefFunc,
	IN ULONG InitState,
	IN ULONG Base);

void StateMachineSetAction(
	IN STATE_MACHINE *S,
	IN ULONG St,
	ULONG Msg,
	IN STATE_MACHINE_FUNC F);

void StateMachinePerformAction(
	IN  struct rtmp_adapter *  pAd,
	IN STATE_MACHINE *S,
	IN MLME_QUEUE_ELEM *Elem,
	IN ULONG CurrState);

void Drop(
	IN  struct rtmp_adapter *  pAd,
	IN MLME_QUEUE_ELEM *Elem);

void AssocStateMachineInit(
	IN  struct rtmp_adapter *  pAd,
	IN  STATE_MACHINE *Sm,
	OUT STATE_MACHINE_FUNC Trans[]);

void ReassocTimeout(
	IN void *SystemSpecific1,
	IN void *FunctionContext,
	IN void *SystemSpecific2,
	IN void *SystemSpecific3);

void AssocTimeout(
	IN void *SystemSpecific1,
	IN void *FunctionContext,
	IN void *SystemSpecific2,
	IN void *SystemSpecific3);

void DisassocTimeout(
	IN void *SystemSpecific1,
	IN void *FunctionContext,
	IN void *SystemSpecific2,
	IN void *SystemSpecific3);

/*---------------------------------------------- */
void MlmeDisassocReqAction(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void MlmeAssocReqAction(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void MlmeReassocReqAction(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void MlmeDisassocReqAction(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void PeerAssocRspAction(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void PeerReassocRspAction(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void PeerDisassocAction(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void DisassocTimeoutAction(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void AssocTimeoutAction(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void  ReassocTimeoutAction(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void  Cls3errAction(
	IN  struct rtmp_adapter *  pAd,
	IN  PUCHAR pAddr);

void  InvalidStateWhenAssoc(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void  InvalidStateWhenReassoc(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void InvalidStateWhenDisassociate(
	IN  struct rtmp_adapter *pAd,
	IN  MLME_QUEUE_ELEM *Elem);

#ifdef RTMP_MAC_USB
void MlmeCntlConfirm(
	IN struct rtmp_adapter *pAd,
	IN ULONG MsgType,
	IN USHORT Msg);
#endif /* RTMP_MAC_USB */

void  ComposePsPoll(
	IN  struct rtmp_adapter *  pAd);

void  ComposeNullFrame(
	IN  struct rtmp_adapter *pAd);

void  AssocPostProc(
	IN  struct rtmp_adapter *pAd,
	IN  PUCHAR pAddr2,
	IN  USHORT CapabilityInfo,
	IN  USHORT Aid,
	IN  UCHAR SupRate[],
	IN  UCHAR SupRateLen,
	IN  UCHAR ExtRate[],
	IN  UCHAR ExtRateLen,
	IN PEDCA_PARM pEdcaParm,
	IN IE_LISTS *ie_list,
	IN HT_CAPABILITY_IE *pHtCapability,
	IN  UCHAR HtCapabilityLen,
	IN ADD_HT_INFO_IE *pAddHtInfo);

void AuthStateMachineInit(
	IN  struct rtmp_adapter *  pAd,
	IN PSTATE_MACHINE sm,
	OUT STATE_MACHINE_FUNC Trans[]);

void AuthTimeout(
	IN void *SystemSpecific1,
	IN void *FunctionContext,
	IN void *SystemSpecific2,
	IN void *SystemSpecific3);

void MlmeAuthReqAction(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void PeerAuthRspAtSeq2Action(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void PeerAuthRspAtSeq4Action(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void AuthTimeoutAction(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void Cls2errAction(
	IN  struct rtmp_adapter *  pAd,
	IN  PUCHAR pAddr);

void MlmeDeauthReqAction(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void InvalidStateWhenAuth(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

/*============================================= */

void AuthRspStateMachineInit(
	IN  struct rtmp_adapter *  pAd,
	IN  PSTATE_MACHINE Sm,
	IN  STATE_MACHINE_FUNC Trans[]);

void PeerDeauthAction(
	IN struct rtmp_adapter *pAd,
	IN MLME_QUEUE_ELEM *Elem);

void PeerAuthSimpleRspGenAndSend(
	IN  struct rtmp_adapter *  pAd,
	IN  PHEADER_802_11  pHdr80211,
	IN  USHORT Alg,
	IN  USHORT Seq,
	IN  USHORT Reason,
	IN  USHORT Status);

/* */
/* Private routines in dls.c */
/* */

#ifdef CONFIG_STA_SUPPORT
#ifdef QOS_DLS_SUPPORT
void DlsStateMachineInit(
    IN struct rtmp_adapter *pAd,
    IN STATE_MACHINE *Sm,
    OUT STATE_MACHINE_FUNC Trans[]);

void MlmeDlsReqAction(
    IN struct rtmp_adapter *pAd,
    IN MLME_QUEUE_ELEM *Elem);

void PeerDlsReqAction(
    IN struct rtmp_adapter *pAd,
    IN MLME_QUEUE_ELEM	*Elem);

void PeerDlsRspAction(
    IN struct rtmp_adapter *pAd,
    IN MLME_QUEUE_ELEM	*Elem);

void MlmeDlsTearDownAction(
    IN struct rtmp_adapter *pAd,
    IN MLME_QUEUE_ELEM *Elem);

void PeerDlsTearDownAction(
    IN struct rtmp_adapter *pAd,
    IN MLME_QUEUE_ELEM	*Elem);

void RTMPCheckDLSTimeOut(
	IN struct rtmp_adapter *pAd);

BOOLEAN RTMPRcvFrameDLSCheck(
	IN struct rtmp_adapter*pAd,
	IN PHEADER_802_11 pHeader,
	IN ULONG Len,
	IN RXD_STRUC *pRxD);

INT	RTMPCheckDLSFrame(
	IN struct rtmp_adapter*pAd,
	IN UCHAR *pDA);

void RTMPSendDLSTearDownFrame(
	IN	struct rtmp_adapter *pAd,
	IN  PUCHAR          pDA);

NDIS_STATUS RTMPSendSTAKeyRequest(
	IN	struct rtmp_adapter *pAd,
	IN	PUCHAR			pDA);

NDIS_STATUS RTMPSendSTAKeyHandShake(
	IN	struct rtmp_adapter *pAd,
	IN	PUCHAR			pDA);

void DlsTimeoutAction(
	IN void *SystemSpecific1,
	IN void *FunctionContext,
	IN void *SystemSpecific2,
	IN void *SystemSpecific3);

BOOLEAN MlmeDlsReqSanity(
	IN struct rtmp_adapter *pAd,
    IN void *Msg,
    IN ULONG MsgLen,
    OUT PRT_802_11_DLS *pDLS,
    OUT PUSHORT pReason);

INT Set_DlsEntryInfo_Display_Proc(
	IN struct rtmp_adapter *pAd,
	IN PUCHAR arg);

MAC_TABLE_ENTRY *MacTableInsertDlsEntry(
	IN  struct rtmp_adapter *  pAd,
	IN  PUCHAR	pAddr,
	IN  UINT	DlsEntryIdx);

BOOLEAN MacTableDeleteDlsEntry(
	IN struct rtmp_adapter *pAd,
	IN USHORT wcid,
	IN PUCHAR pAddr);

MAC_TABLE_ENTRY *DlsEntryTableLookup(
	IN struct rtmp_adapter *pAd,
	IN PUCHAR	pAddr,
	IN BOOLEAN	bResetIdelCount);

MAC_TABLE_ENTRY *DlsEntryTableLookupByWcid(
	IN struct rtmp_adapter *pAd,
	IN UCHAR	wcid,
	IN PUCHAR	pAddr,
	IN BOOLEAN	bResetIdelCount);

INT	Set_DlsAddEntry_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_DlsTearDownEntry_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);
#endif /* QOS_DLS_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

#ifdef QOS_DLS_SUPPORT
BOOLEAN PeerDlsReqSanity(
    IN struct rtmp_adapter *pAd,
    IN void *Msg,
    IN ULONG MsgLen,
    OUT PUCHAR pDA,
    OUT PUCHAR pSA,
    OUT USHORT *pCapabilityInfo,
    OUT USHORT *pDlsTimeout,
    OUT UCHAR *pRatesLen,
    OUT UCHAR Rates[],
    OUT UCHAR *pHtCapabilityLen,
    OUT HT_CAPABILITY_IE *pHtCapability);

BOOLEAN PeerDlsRspSanity(
    IN struct rtmp_adapter *pAd,
    IN void *Msg,
    IN ULONG MsgLen,
    OUT PUCHAR pDA,
    OUT PUCHAR pSA,
    OUT USHORT *pCapabilityInfo,
    OUT USHORT *pStatus,
    OUT UCHAR *pRatesLen,
    OUT UCHAR Rates[],
    OUT UCHAR *pHtCapabilityLen,
    OUT HT_CAPABILITY_IE *pHtCapability);

BOOLEAN PeerDlsTearDownSanity(
    IN struct rtmp_adapter *pAd,
    IN void *Msg,
    IN ULONG MsgLen,
    OUT PUCHAR pDA,
    OUT PUCHAR pSA,
    OUT USHORT *pReason);
#endif /* QOS_DLS_SUPPORT */

BOOLEAN PeerProbeReqSanity(
    IN struct rtmp_adapter *pAd,
    IN void *Msg,
    IN ULONG MsgLen,
    OUT PUCHAR pAddr2,
    OUT CHAR Ssid[],
    OUT UCHAR *SsidLen,
    OUT BOOLEAN *bRequestRssi);

/*======================================== */

void SyncStateMachineInit(
	IN  struct rtmp_adapter *  pAd,
	IN  STATE_MACHINE *Sm,
	OUT STATE_MACHINE_FUNC Trans[]);

void BeaconTimeout(
	IN void *SystemSpecific1,
	IN void *FunctionContext,
	IN void *SystemSpecific2,
	IN void *SystemSpecific3);

void ScanTimeout(
	IN void *SystemSpecific1,
	IN void *FunctionContext,
	IN void *SystemSpecific2,
	IN void *SystemSpecific3);

void MlmeScanReqAction(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void InvalidStateWhenScan(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void InvalidStateWhenJoin(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void InvalidStateWhenStart(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void PeerBeacon(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void EnqueueProbeRequest(
	IN struct rtmp_adapter *pAd);

BOOLEAN ScanRunning(
		IN struct rtmp_adapter *pAd);
/*========================================= */

void MlmeCntlInit(
	IN  struct rtmp_adapter *  pAd,
	IN  STATE_MACHINE *S,
	OUT STATE_MACHINE_FUNC Trans[]);

void MlmeCntlMachinePerformAction(
	IN  struct rtmp_adapter *  pAd,
	IN  STATE_MACHINE *S,
	IN  MLME_QUEUE_ELEM *Elem);

void CntlIdleProc(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void CntlOidScanProc(
	IN  struct rtmp_adapter *pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void CntlOidSsidProc(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM * Elem);

void CntlOidRTBssidProc(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM * Elem);

void CntlMlmeRoamingProc(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM * Elem);

void CntlWaitDisassocProc(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void CntlWaitJoinProc(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void CntlWaitReassocProc(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void CntlWaitStartProc(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void CntlWaitAuthProc(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void CntlWaitAuthProc2(
	IN  struct rtmp_adapter *pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void CntlWaitAssocProc(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

#ifdef QOS_DLS_SUPPORT
void CntlOidDLSSetupProc(
	IN struct rtmp_adapter *pAd,
	IN MLME_QUEUE_ELEM *Elem);
#endif /* QOS_DLS_SUPPORT */


void LinkUp(
	IN  struct rtmp_adapter *  pAd,
	IN  UCHAR BssType);

void LinkDown(
	IN  struct rtmp_adapter *  pAd,
	IN  BOOLEAN         IsReqFromAP);

void IterateOnBssTab(
	IN  struct rtmp_adapter *  pAd);

void IterateOnBssTab2(
	IN  struct rtmp_adapter *  pAd);;

void JoinParmFill(
	IN  struct rtmp_adapter *  pAd,
	IN  OUT MLME_JOIN_REQ_STRUCT *JoinReq,
	IN  ULONG BssIdx);

void AssocParmFill(
	IN  struct rtmp_adapter *  pAd,
	IN OUT MLME_ASSOC_REQ_STRUCT *AssocReq,
	IN  PUCHAR pAddr,
	IN  USHORT CapabilityInfo,
	IN  ULONG Timeout,
	IN  USHORT ListenIntv);

void ScanParmFill(
	IN  struct rtmp_adapter *  pAd,
	IN  OUT MLME_SCAN_REQ_STRUCT *ScanReq,
	IN  STRING Ssid[],
	IN  UCHAR SsidLen,
	IN  UCHAR BssType,
	IN  UCHAR ScanType);

void DisassocParmFill(
	IN  struct rtmp_adapter *  pAd,
	IN  OUT MLME_DISASSOC_REQ_STRUCT *DisassocReq,
	IN  PUCHAR pAddr,
	IN  USHORT Reason);

void StartParmFill(
	IN  struct rtmp_adapter *  pAd,
	IN  OUT MLME_START_REQ_STRUCT *StartReq,
	IN  CHAR Ssid[],
	IN  UCHAR SsidLen);

void AuthParmFill(
	IN  struct rtmp_adapter *  pAd,
	IN  OUT MLME_AUTH_REQ_STRUCT *AuthReq,
	IN  PUCHAR pAddr,
	IN  USHORT Alg);

void EnqueuePsPoll(
	IN  struct rtmp_adapter *  pAd);

void EnqueueBeaconFrame(
	IN  struct rtmp_adapter *  pAd);

void MlmeJoinReqAction(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void MlmeScanReqAction(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void MlmeStartReqAction(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void MlmeForceJoinReqAction(
	IN struct rtmp_adapter *pAd,
	IN MLME_QUEUE_ELEM *Elem);

void MlmeForceScanReqAction(
	IN struct rtmp_adapter *pAd,
	IN MLME_QUEUE_ELEM *Elem);

void ScanTimeoutAction(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void BeaconTimeoutAtJoinAction(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void PeerBeaconAtScanAction(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void PeerBeaconAtJoinAction(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void PeerBeacon(
	IN  struct rtmp_adapter *  pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void PeerProbeReqAction(
	IN  struct rtmp_adapter *pAd,
	IN  MLME_QUEUE_ELEM *Elem);

void ScanNextChannel(
	IN  struct rtmp_adapter *  pAd,
	IN	UCHAR	OpMode);


ULONG MakeIbssBeacon(
	IN  struct rtmp_adapter *  pAd);

#ifdef CONFIG_STA_SUPPORT
void InitChannelRelatedValue(
	IN  struct rtmp_adapter *  pAd);

void AdjustChannelRelatedValue(
	IN struct rtmp_adapter *pAd,
	OUT UCHAR *pBwFallBack,
	IN USHORT ifIndex,
	IN BOOLEAN BandWidth,
	IN UCHAR PriCh,
	IN UCHAR ExtraCh);
#endif /* CONFIG_STA_SUPPORT */

BOOLEAN MlmeScanReqSanity(
	IN  struct rtmp_adapter *  pAd,
	IN  void *Msg,
	IN  ULONG MsgLen,
	OUT UCHAR *BssType,
	OUT CHAR ssid[],
	OUT UCHAR *SsidLen,
	OUT UCHAR *ScanType);


BOOLEAN PeerBeaconAndProbeRspSanity_Old(
	IN  struct rtmp_adapter *  pAd,
	IN  void *Msg,
	IN  ULONG MsgLen,
	IN  UCHAR MsgChannel,
	OUT PUCHAR pAddr2,
	OUT PUCHAR pBssid,
	OUT CHAR Ssid[],
	OUT UCHAR *pSsidLen,
	OUT UCHAR *pBssType,
	OUT USHORT *pBeaconPeriod,
	OUT UCHAR *pChannel,
	OUT UCHAR *pNewChannel,
	OUT LARGE_INTEGER *pTimestamp,
	OUT CF_PARM *pCfParm,
	OUT USHORT *pAtimWin,
	OUT USHORT *pCapabilityInfo,
	OUT UCHAR *pErp,
	OUT UCHAR *pDtimCount,
	OUT UCHAR *pDtimPeriod,
	OUT UCHAR *pBcastFlag,
	OUT UCHAR *pMessageToMe,
	OUT UCHAR SupRate[],
	OUT UCHAR *pSupRateLen,
	OUT UCHAR ExtRate[],
	OUT UCHAR *pExtRateLen,
	OUT	UCHAR *pCkipFlag,
	OUT	UCHAR *pAironetCellPowerLimit,
	OUT PEDCA_PARM       pEdcaParm,
	OUT PQBSS_LOAD_PARM  pQbssLoad,
	OUT PQOS_CAPABILITY_PARM pQosCapability,
	OUT ULONG *pRalinkIe,
	OUT UCHAR		 *pHtCapabilityLen,
#ifdef CONFIG_STA_SUPPORT
	OUT UCHAR		 *pPreNHtCapabilityLen,
#endif /* CONFIG_STA_SUPPORT */
	OUT HT_CAPABILITY_IE *pHtCapability,
	OUT EXT_CAP_INFO_ELEMENT *pExtCapInfo,
	OUT UCHAR		 *AddHtInfoLen,
	OUT ADD_HT_INFO_IE *AddHtInfo,
	OUT UCHAR *NewExtChannel,
	OUT USHORT *LengthVIE,
	OUT PNDIS_802_11_VARIABLE_IEs pVIE);


BOOLEAN PeerBeaconAndProbeRspSanity(
	IN struct rtmp_adapter *pAd,
	IN void *Msg,
	IN ULONG MsgLen,
	IN UCHAR  MsgChannel,
	OUT BCN_IE_LIST *ie_list,
	OUT USHORT *LengthVIE,
	OUT PNDIS_802_11_VARIABLE_IEs pVIE);


#ifdef DOT11_N_SUPPORT
#ifdef DOT11N_DRAFT3
BOOLEAN PeerBeaconAndProbeRspSanity2(
	IN struct rtmp_adapter *pAd,
	IN void *Msg,
	IN ULONG MsgLen,
	IN OVERLAP_BSS_SCAN_IE *BssScan,
	OUT UCHAR 	*RegClass);
#endif /* DOT11N_DRAFT3 */
#endif /* DOT11_N_SUPPORT */

BOOLEAN PeerAddBAReqActionSanity(
    IN struct rtmp_adapter *pAd,
    IN void *pMsg,
    IN ULONG MsgLen,
	OUT PUCHAR pAddr2);

BOOLEAN PeerAddBARspActionSanity(
    IN struct rtmp_adapter *pAd,
    IN void *pMsg,
    IN ULONG MsgLen);

BOOLEAN PeerDelBAActionSanity(
    IN struct rtmp_adapter *pAd,
    IN UCHAR Wcid,
    IN void *pMsg,
    IN ULONG MsgLen);

BOOLEAN MlmeAssocReqSanity(
	IN  struct rtmp_adapter *  pAd,
	IN  void *Msg,
	IN  ULONG MsgLen,
	OUT PUCHAR pApAddr,
	OUT USHORT *CapabilityInfo,
	OUT ULONG *Timeout,
	OUT USHORT *ListenIntv);

BOOLEAN MlmeAuthReqSanity(
	IN  struct rtmp_adapter *  pAd,
	IN  void *Msg,
	IN  ULONG MsgLen,
	OUT PUCHAR pAddr,
	OUT ULONG *Timeout,
	OUT USHORT *Alg);

BOOLEAN MlmeStartReqSanity(
	IN  struct rtmp_adapter *  pAd,
	IN  void *Msg,
	IN  ULONG MsgLen,
	OUT CHAR Ssid[],
	OUT UCHAR *Ssidlen);

BOOLEAN PeerAuthSanity(
	IN  struct rtmp_adapter *  pAd,
	IN  void *Msg,
	IN  ULONG MsgLen,
	OUT PUCHAR pAddr,
	OUT USHORT *Alg,
	OUT USHORT *Seq,
	OUT USHORT *Status,
	OUT CHAR ChlgText[]);

BOOLEAN PeerAssocRspSanity(
	IN  struct rtmp_adapter *  pAd,
    IN void *pMsg,
	IN  ULONG MsgLen,
	OUT PUCHAR pAddr2,
	OUT USHORT *pCapabilityInfo,
	OUT USHORT *pStatus,
	OUT USHORT *pAid,
	OUT UCHAR SupRate[],
	OUT UCHAR *pSupRateLen,
	OUT UCHAR ExtRate[],
	OUT UCHAR *pExtRateLen,
    OUT HT_CAPABILITY_IE		*pHtCapability,
    OUT ADD_HT_INFO_IE		*pAddHtInfo,	/* AP might use this additional ht info IE */
    OUT UCHAR			*pHtCapabilityLen,
    OUT UCHAR			*pAddHtInfoLen,
    OUT UCHAR			*pNewExtChannelOffset,
	OUT PEDCA_PARM pEdcaParm,
	OUT EXT_CAP_INFO_ELEMENT *pExtCapInfo,
	OUT UCHAR *pCkipFlag,
	OUT IE_LISTS *ie_list);

BOOLEAN PeerDisassocSanity(
	IN  struct rtmp_adapter *  pAd,
	IN  void *Msg,
	IN  ULONG MsgLen,
	OUT PUCHAR pAddr2,
	OUT USHORT *Reason);

BOOLEAN PeerDeauthSanity(
	IN  struct rtmp_adapter *  pAd,
	IN  void *Msg,
	IN  ULONG MsgLen,
	OUT PUCHAR pAddr1,
	OUT PUCHAR pAddr2,
	OUT PUCHAR pAddr3,
	OUT USHORT *Reason);

BOOLEAN GetTimBit(
	IN  CHAR *Ptr,
	IN  USHORT Aid,
	OUT UCHAR *TimLen,
	OUT UCHAR *BcastFlag,
	OUT UCHAR *DtimCount,
	OUT UCHAR *DtimPeriod,
	OUT UCHAR *MessageToMe);

UCHAR ChannelSanity(
	IN struct rtmp_adapter *pAd,
	IN UCHAR channel);

NDIS_802_11_NETWORK_TYPE NetworkTypeInUseSanity(
	IN PBSS_ENTRY pBss);

BOOLEAN MlmeDelBAReqSanity(
    IN struct rtmp_adapter *pAd,
    IN void *Msg,
    IN ULONG MsgLen);

BOOLEAN MlmeAddBAReqSanity(
    IN struct rtmp_adapter *pAd,
    IN void *Msg,
    IN ULONG MsgLen,
    OUT PUCHAR pAddr2);

ULONG MakeOutgoingFrame(
	OUT UCHAR *Buffer,
	OUT ULONG *Length, ...);

UCHAR RandomByte(
	IN  struct rtmp_adapter *  pAd);

UCHAR RandomByte2(
	IN  struct rtmp_adapter *  pAd);

void AsicUpdateAutoFallBackTable(
	IN	struct rtmp_adapter *pAd,
	IN	PUCHAR			pTxRate);



void  MlmePeriodicExec(
	IN void *SystemSpecific1,
	IN void *FunctionContext,
	IN void *SystemSpecific2,
	IN void *SystemSpecific3);

void LinkDownExec(
	IN void *SystemSpecific1,
	IN void *FunctionContext,
	IN void *SystemSpecific2,
	IN void *SystemSpecific3);

void LinkUpExec(
	IN void *SystemSpecific1,
	IN void *FunctionContext,
	IN void *SystemSpecific2,
	IN void *SystemSpecific3);

void STAMlmePeriodicExec(
	struct rtmp_adapter *pAd);

void MlmeAutoScan(
	IN struct rtmp_adapter *pAd);

void MlmeAutoReconnectLastSSID(
	IN struct rtmp_adapter *pAd);

BOOLEAN MlmeValidateSSID(
	IN PUCHAR pSsid,
	IN UCHAR  SsidLen);

void MlmeCheckForRoaming(
	IN struct rtmp_adapter *pAd,
	IN ULONG    Now32);

BOOLEAN MlmeCheckForFastRoaming(
	IN  struct rtmp_adapter *  pAd);

#ifdef TXBF_SUPPORT
BOOLEAN MlmeTxBfAllowed(
	IN struct rtmp_adapter *		pAd,
	IN PMAC_TABLE_ENTRY		pEntry,
	IN struct _RTMP_RA_LEGACY_TB *pTxRate);
#endif /* TXBF_SUPPORT */

#ifdef AGS_SUPPORT
INT Show_AGS_Proc(
    IN  struct rtmp_adapter *pAd,
    IN  char *		arg);

#ifdef CONFIG_STA_SUPPORT
void MlmeDynamicTxRateSwitchingAGS(
	IN struct rtmp_adapter *pAd,
	IN PMAC_TABLE_ENTRY pEntry,
	IN PUCHAR pTable,
	IN UCHAR TableSize,
	IN PAGS_STATISTICS_INFO pAGSStatisticsInfo,
	IN UCHAR InitTxRateIdx);

void StaQuickResponeForRateUpExecAGS(
	IN struct rtmp_adapter *pAd,
	IN PMAC_TABLE_ENTRY pEntry,
	IN PUCHAR pTable,
	IN UCHAR TableSize,
	IN PAGS_STATISTICS_INFO pAGSStatisticsInfo,
	IN UCHAR InitTxRateIdx);
#endif /* CONFIG_STA_SUPPORT */

#endif /* AGS_SUPPORT */

void MlmeCalculateChannelQuality(
	IN struct rtmp_adapter *pAd,
	IN PMAC_TABLE_ENTRY pMacEntry,
	IN ULONG Now);

void MlmeCheckPsmChange(
	IN struct rtmp_adapter *pAd,
	IN ULONG    Now32);

void MlmeSetPsmBit(
	IN struct rtmp_adapter *pAd,
	IN USHORT psm);

void MlmeSetTxPreamble(
	IN struct rtmp_adapter *pAd,
	IN USHORT TxPreamble);

void UpdateBasicRateBitmap(
	IN	struct rtmp_adapter *pAd);

void MlmeUpdateTxRates(
	IN struct rtmp_adapter *	pAd,
	IN 	BOOLEAN		 	bLinkUp,
	IN	UCHAR			apidx);

#ifdef DOT11_N_SUPPORT
void MlmeUpdateHtTxRates(
	IN struct rtmp_adapter *		pAd,
	IN	UCHAR				apidx);
#endif /* DOT11_N_SUPPORT */

void    RTMPCheckRates(
	IN      struct rtmp_adapter *  pAd,
	IN OUT  UCHAR           SupRate[],
	IN OUT  UCHAR           *SupRateLen);

#ifdef CONFIG_STA_SUPPORT
BOOLEAN RTMPCheckChannel(
	IN struct rtmp_adapter *pAd,
	IN UCHAR		CentralChannel,
	IN UCHAR		Channel);
#endif /* CONFIG_STA_SUPPORT */

BOOLEAN RTMPCheckHt(
	IN struct rtmp_adapter*pAd,
	IN UCHAR Wcid,
	INOUT HT_CAPABILITY_IE *pHtCapability,
	INOUT ADD_HT_INFO_IE *pAddHtInfo);

#ifdef DOT11_VHT_AC
BOOLEAN RTMPCheckVht(
	IN struct rtmp_adapter*pAd,
	IN UCHAR Wcid,
	IN VHT_CAP_IE *vht_cap,
	IN VHT_OP_IE *vht_op);
#endif /* DOT11_VHT_AC */

void RTMPUpdateMlmeRate(
	IN struct rtmp_adapter *pAd);

CHAR RTMPMaxRssi(
	IN struct rtmp_adapter *pAd,
	IN CHAR				Rssi0,
	IN CHAR				Rssi1,
	IN CHAR				Rssi2);

CHAR RTMPAvgRssi(
        IN struct rtmp_adapter *pAd,
        IN RSSI_SAMPLE		*pRssi);


CHAR RTMPMinSnr(
	IN struct rtmp_adapter *pAd,
	IN CHAR				Snr0,
	IN CHAR				Snr1);

void AsicSetRxAnt(
	IN struct rtmp_adapter *pAd,
	IN UCHAR			Ant);

#ifdef RTMP_EFUSE_SUPPORT
INT set_eFuseGetFreeBlockCount_Proc(
   	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT set_eFusedump_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT set_eFuseLoadFromBin_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

void eFusePhysicalReadRegisters(
	IN	struct rtmp_adapter *pAd,
	IN	USHORT Offset,
	IN	USHORT Length,
	OUT	USHORT* pData);

int RtmpEfuseSupportCheck(
	IN struct rtmp_adapter*pAd);

#endif /* RTMP_EFUSE_SUPPORT */





void AsicEvaluateRxAnt(
	IN struct rtmp_adapter *pAd);

void AsicRxAntEvalTimeout(
	IN void *SystemSpecific1,
	IN void *FunctionContext,
	IN void *SystemSpecific2,
	IN void *SystemSpecific3);

void APSDPeriodicExec(
	IN void *SystemSpecific1,
	IN void *FunctionContext,
	IN void *SystemSpecific2,
	IN void *SystemSpecific3);

BOOLEAN RTMPCheckEntryEnableAutoRateSwitch(
	IN struct rtmp_adapter *   pAd,
	IN PMAC_TABLE_ENTRY	pEntry);

UCHAR RTMPStaFixedTxMode(
	IN struct rtmp_adapter *   pAd,
	IN PMAC_TABLE_ENTRY	pEntry);

void RTMPUpdateLegacyTxSetting(
		UCHAR				fixed_tx_mode,
		PMAC_TABLE_ENTRY	pEntry);

BOOLEAN RTMPAutoRateSwitchCheck(
	IN struct rtmp_adapter *   pAd);

NDIS_STATUS MlmeInit(
	IN  struct rtmp_adapter *  pAd);


#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */

#ifdef RTMP_TEMPERATURE_COMPENSATION
void InitLookupTable(
	IN struct rtmp_adapter *pAd);
#endif /* RTMP_TEMPERATURE_COMPENSATION */

void MlmeHandler(
	IN  struct rtmp_adapter *  pAd);

void MlmeHalt(
	IN  struct rtmp_adapter *  pAd);

void MlmeResetRalinkCounters(
	IN  struct rtmp_adapter *  pAd);

void BuildChannelList(
	IN struct rtmp_adapter *pAd);

UCHAR FirstChannel(
	IN  struct rtmp_adapter *  pAd);

UCHAR NextChannel(
	IN  struct rtmp_adapter *  pAd,
	IN  UCHAR channel);

void ChangeToCellPowerLimit(
	IN struct rtmp_adapter *pAd,
	IN UCHAR         AironetCellPowerLimit);

/* */
/* Prototypes of function definition in cmm_tkip.c */
/* */
void    RTMPInitMICEngine(
	IN  struct rtmp_adapter *  pAd,
	IN  PUCHAR          pKey,
	IN  PUCHAR          pDA,
	IN  PUCHAR          pSA,
	IN  UCHAR           UserPriority,
	IN  PUCHAR          pMICKey);

BOOLEAN RTMPTkipCompareMICValue(
	IN  struct rtmp_adapter *  pAd,
	IN  PUCHAR          pSrc,
	IN  PUCHAR          pDA,
	IN  PUCHAR          pSA,
	IN  PUCHAR          pMICKey,
	IN	UCHAR			UserPriority,
	IN  UINT            Len);

void    RTMPCalculateMICValue(
	IN  struct rtmp_adapter *  pAd,
	IN  PNDIS_PACKET    pPacket,
	IN  PUCHAR          pEncap,
	IN  PCIPHER_KEY     pKey,
	IN	UCHAR			apidx);

void    RTMPTkipAppendByte(
	IN  PTKIP_KEY_INFO  pTkip,
	IN  UCHAR           uChar);

void    RTMPTkipAppend(
	IN  PTKIP_KEY_INFO  pTkip,
	IN  PUCHAR          pSrc,
	IN  UINT            nBytes);

void    RTMPTkipGetMIC(
	IN  PTKIP_KEY_INFO  pTkip);

/* */
/* Prototypes of function definition in cmm_cfg.c */
/* */
INT RT_CfgSetCountryRegion(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg,
	IN INT				band);

INT RT_CfgSetWirelessMode(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

UCHAR cfgmode_2_wmode(UCHAR cfg_mode);
UCHAR *wmode_2_str(UCHAR wmode);


INT RT_CfgSetShortSlot(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	RT_CfgSetWepKey(
	IN	struct rtmp_adapter *pAd,
	IN	char *		keyString,
	IN	CIPHER_KEY		*pSharedKey,
	IN	INT				keyIdx);

INT RT_CfgSetWPAPSKKey(
	IN struct rtmp_adapter*pAd,
	IN char *	keyString,
	IN INT			keyStringLen,
	IN UCHAR		*pHashStr,
	IN INT			hashStrLen,
	OUT PUCHAR		pPMKBuf);

INT	RT_CfgSetFixedTxPhyMode(
	IN	char *		arg);

INT	RT_CfgSetMacAddress(
	IN 	struct rtmp_adapter *	pAd,
	IN	char *		arg);

INT	RT_CfgSetTxMCSProc(
	IN	char *		arg,
	OUT	BOOLEAN			*pAutoRate);

INT	RT_CfgSetAutoFallBack(
	IN 	struct rtmp_adapter *	pAd,
	IN	char *		arg);


INT	Set_Antenna_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);


/* */
/* Prototypes of function definition in cmm_info.c */
/* */
NDIS_STATUS RTMPWPARemoveKeyProc(
	IN  struct rtmp_adapter *  pAd,
	IN  void *          pBuf);

void    RTMPWPARemoveAllKeys(
	IN  struct rtmp_adapter *  pAd);

BOOLEAN RTMPCheckStrPrintAble(
    IN  CHAR *pInPutStr,
    IN  UCHAR strLen);

void    RTMPSetPhyMode(
	IN  struct rtmp_adapter *  pAd,
	IN  ULONG phymode);

void RTMPUpdateHTIE(
	IN	RT_HT_CAPABILITY	*pRtHt,
	IN		UCHAR				*pMcsSet,
	OUT		HT_CAPABILITY_IE *pHtCapability,
	OUT		ADD_HT_INFO_IE		*pAddHtInfo);

void RTMPAddWcidAttributeEntry(
	IN	struct rtmp_adapter *pAd,
	IN	UCHAR			BssIdx,
	IN 	UCHAR		 	KeyIdx,
	IN 	UCHAR		 	CipherAlg,
	IN 	MAC_TABLE_ENTRY *pEntry);

char *GetEncryptType(
	CHAR enc);

char *GetAuthMode(
	CHAR auth);

#ifdef DOT11_N_SUPPORT
void RTMPSetHT(
	IN	struct rtmp_adapter *pAd,
	IN	OID_SET_HT_PHYMODE *pHTPhyMode);

void RTMPSetIndividualHT(
	IN	struct rtmp_adapter *	pAd,
	IN	UCHAR				apidx);

UCHAR get_cent_ch_by_htinfo(
	struct rtmp_adapter*pAd,
	ADD_HT_INFO_IE *ht_op,
	HT_CAPABILITY_IE *ht_cap);

INT get_ht_cent_ch(struct rtmp_adapter*pAd, UCHAR *rf_bw, UCHAR *ext_ch);
INT ht_mode_adjust(struct rtmp_adapter*pAd, MAC_TABLE_ENTRY *pEntry, HT_CAPABILITY_IE *peer, RT_HT_CAPABILITY *my);
INT set_ht_fixed_mcs(struct rtmp_adapter*pAd, MAC_TABLE_ENTRY *pEntry, UCHAR fixed_mcs, UCHAR mcs_bound);
INT get_ht_max_mcs(struct rtmp_adapter*pAd, UCHAR *desire_mcs, UCHAR *cap_mcs);
#endif /* DOT11_N_SUPPORT */

void RTMPDisableDesiredHtInfo(
	IN	struct rtmp_adapter *pAd);

CHAR    ConvertToRssi(
	IN struct rtmp_adapter * pAd,
	IN CHAR				Rssi,
	IN UCHAR    RssiNumber);

CHAR    ConvertToSnr(
	IN struct rtmp_adapter * pAd,
	IN UCHAR				Snr);

#ifdef DOT11N_DRAFT3
void BuildEffectedChannelList(
	IN struct rtmp_adapter *pAd);


void DeleteEffectedChannelList(
	IN struct rtmp_adapter *pAd);

void CntlChannelWidth(
	IN struct rtmp_adapter *pAd,
	IN UCHAR			PrimaryChannel,
	IN UCHAR			CentralChannel,
	IN UCHAR			ChannelWidth,
	IN UCHAR			SecondaryChannelOffset);

#endif /* DOT11N_DRAFT3 */


void APAsicEvaluateRxAnt(
	IN struct rtmp_adapter *pAd);


void APAsicRxAntEvalTimeout(
	IN struct rtmp_adapter *pAd);


/* */
/* function prototype in ap_wpa.c */
/* */
void RTMPGetTxTscFromAsic(
	IN  struct rtmp_adapter *  pAd,
	IN	UCHAR			apidx,
	OUT	PUCHAR			pTxTsc);

MAC_TABLE_ENTRY *PACInquiry(
	IN  struct rtmp_adapter *  pAd,
	IN  ULONG           Wcid);

UINT	APValidateRSNIE(
	IN struct rtmp_adapter *   pAd,
	IN PMAC_TABLE_ENTRY pEntry,
	IN PUCHAR			pRsnIe,
	IN UCHAR			rsnie_len);

void HandleCounterMeasure(
	IN struct rtmp_adapter *pAd,
	IN MAC_TABLE_ENTRY  *pEntry);

void WPAStart4WayHS(
	IN  struct rtmp_adapter *  pAd,
	IN  MAC_TABLE_ENTRY *pEntry,
	IN	ULONG			TimeInterval);

void WPAStart2WayGroupHS(
	IN  struct rtmp_adapter *  pAd,
	IN  MAC_TABLE_ENTRY *pEntry);

void PeerPairMsg1Action(
	IN struct rtmp_adapter *pAd,
	IN MAC_TABLE_ENTRY  *pEntry,
	IN MLME_QUEUE_ELEM *Elem);

void PeerPairMsg2Action(
	IN struct rtmp_adapter *pAd,
	IN MAC_TABLE_ENTRY  *pEntry,
	IN MLME_QUEUE_ELEM *Elem);

void PeerPairMsg3Action(
	IN struct rtmp_adapter *pAd,
	IN MAC_TABLE_ENTRY  *pEntry,
	IN MLME_QUEUE_ELEM *Elem);

void PeerPairMsg4Action(
	IN struct rtmp_adapter *pAd,
	IN MAC_TABLE_ENTRY  *pEntry,
	IN MLME_QUEUE_ELEM *Elem);

void PeerGroupMsg1Action(
	IN  struct rtmp_adapter *   pAd,
	IN  PMAC_TABLE_ENTRY pEntry,
    IN  MLME_QUEUE_ELEM  *Elem);

void PeerGroupMsg2Action(
	IN  struct rtmp_adapter *   pAd,
	IN  PMAC_TABLE_ENTRY pEntry,
	IN  void             *Msg,
	IN  UINT             MsgLen);

void CMTimerExec(
	IN void *SystemSpecific1,
	IN void *FunctionContext,
	IN void *SystemSpecific2,
	IN void *SystemSpecific3);

void WPARetryExec(
	IN void *SystemSpecific1,
	IN void *FunctionContext,
	IN void *SystemSpecific2,
	IN void *SystemSpecific3);

#ifdef TXBF_SUPPORT
void eTxBfProbeTimerExec(
	IN void *SystemSpecific1,
	IN void *FunctionContext,
	IN void *SystemSpecific2,
	IN void *SystemSpecific3);
#endif /* TXBF_SUPPORT */

void EnqueueStartForPSKExec(
    IN void *SystemSpecific1,
    IN void *FunctionContext,
    IN void *SystemSpecific2,
    IN void *SystemSpecific3);


void RTMPHandleSTAKey(
    IN struct rtmp_adapter *   pAdapter,
    IN MAC_TABLE_ENTRY  *pEntry,
    IN MLME_QUEUE_ELEM  *Elem);

void MlmeDeAuthAction(
	IN  struct rtmp_adapter *   pAd,
	IN  PMAC_TABLE_ENTRY pEntry,
	IN  USHORT           Reason,
	IN  BOOLEAN          bDataFrameFirst);


void GREKEYPeriodicExec(
	IN  void *  SystemSpecific1,
	IN  void *  FunctionContext,
	IN  void *  SystemSpecific2,
	IN  void *  SystemSpecific3);

void AES_128_CMAC(
	IN	PUCHAR	key,
	IN	PUCHAR	input,
	IN	INT		len,
	OUT	PUCHAR	mac);

#ifdef DOT1X_SUPPORT
void    WpaSend(
    IN  struct rtmp_adapter *  pAdapter,
    IN  PUCHAR          pPacket,
    IN  ULONG           Len);

void RTMPAddPMKIDCache(
	IN  struct rtmp_adapter *  		pAd,
	IN	INT						apidx,
	IN	PUCHAR				pAddr,
	IN	UCHAR					*PMKID,
	IN	UCHAR					*PMK);

INT RTMPSearchPMKIDCache(
	IN  struct rtmp_adapter *  pAd,
	IN	INT				apidx,
	IN	PUCHAR		pAddr);

void RTMPDeletePMKIDCache(
	IN  struct rtmp_adapter *  pAd,
	IN	INT				apidx,
	IN  INT				idx);

void RTMPMaintainPMKIDCache(
	IN  struct rtmp_adapter *  pAd);
#else
#define RTMPMaintainPMKIDCache(_pAd)
#endif /* DOT1X_SUPPORT */

#ifdef RESOURCE_PRE_ALLOC
void RTMPResetTxRxRingMemory(
	IN  struct rtmp_adapter  *pAd);
#endif /* RESOURCE_PRE_ALLOC */

void RTMPFreeTxRxRingMemory(
    IN  struct rtmp_adapter *  pAd);

BOOLEAN RTMP_FillTxBlkInfo(
	IN struct rtmp_adapter*pAd,
	IN TX_BLK *pTxBlk);

 void announce_802_3_packet(
	IN	void 		*pAdSrc,
	IN	PNDIS_PACKET	pPacket,
	IN	UCHAR			OpMode);

#ifdef DOT11_N_SUPPORT
UINT BA_Reorder_AMSDU_Annnounce(
	IN	struct rtmp_adapter *pAd,
	IN	PNDIS_PACKET	pPacket,
	IN	UCHAR			OpMode);
#endif /* DOT11_N_SUPPORT */

PNET_DEV get_netdev_from_bssid(
	IN	struct rtmp_adapter *pAd,
	IN	UCHAR			FromWhichBSSID);


#ifdef DOT11_N_SUPPORT
void ba_flush_reordering_timeout_mpdus(
	IN struct rtmp_adapter *pAd,
	IN PBA_REC_ENTRY	pBAEntry,
	IN ULONG			Now32);


void BAOriSessionSetUp(
			IN struct rtmp_adapter *   pAd,
			IN MAC_TABLE_ENTRY	*pEntry,
			IN UCHAR			TID,
			IN USHORT			TimeOut,
			IN ULONG			DelayTime,
			IN BOOLEAN		isForced);

void BASessionTearDownALL(
	IN OUT	struct rtmp_adapter *pAd,
	IN		UCHAR Wcid);

void BAOriSessionTearDown(
	IN OUT	struct rtmp_adapter *pAd,
	IN		UCHAR			Wcid,
	IN		UCHAR			TID,
	IN		BOOLEAN			bPassive,
	IN		BOOLEAN			bForceSend);

void BARecSessionTearDown(
	IN OUT	struct rtmp_adapter *pAd,
	IN		UCHAR			Wcid,
	IN		UCHAR			TID,
	IN		BOOLEAN			bPassive);
#endif /* DOT11_N_SUPPORT */

BOOLEAN ba_reordering_resource_init(struct rtmp_adapter *pAd, int num);
void ba_reordering_resource_release(struct rtmp_adapter *pAd);

INT ComputeChecksum(
	IN UINT PIN);

UINT GenerateWpsPinCode(
	IN	struct rtmp_adapter *pAd,
    IN  BOOLEAN         bFromApcli,
	IN	UCHAR	apidx);




#ifdef NINTENDO_AP
void InitNINTENDO_TABLE(
	IN struct rtmp_adapter *pAd);

UCHAR	CheckNINTENDO_TABLE(
	IN struct rtmp_adapter *pAd,
	char *pDS_Ssid,
	UCHAR DS_SsidLen,
	PUCHAR pDS_Addr);

UCHAR	DelNINTENDO_ENTRY(
	IN	struct rtmp_adapter *pAd,
	UCHAR * pDS_Addr);

void RTMPIoctlNintendoCapable(
	IN	struct rtmp_adapter *pAd,
	IN	RTMP_IOCTL_INPUT_STRUCT			*wrq);

void RTMPIoctlNintendoGetTable(
	IN	struct rtmp_adapter *pAd,
	IN	RTMP_IOCTL_INPUT_STRUCT			*wrq);

void RTMPIoctlNintendoSetTable(
	IN	struct rtmp_adapter *pAd,
	IN	RTMP_IOCTL_INPUT_STRUCT			*wrq);

#endif /* NINTENDO_AP */

BOOLEAN rtstrmactohex(
	IN char *s1,
	IN char *s2);

BOOLEAN rtstrcasecmp(
	IN char *s1,
	IN char *s2);

char *rtstrstruncasecmp(
	IN char *s1,
	IN char *s2);

char *rtstrstr(
	IN	const char *s1,
	IN	const char *s2);

char *rstrtok(
	IN char *s,
	IN const char *ct);

int rtinet_aton(
	const char *cp,
	unsigned int *addr);

/*//////// common ioctl functions ////////*/
INT Set_DriverVersion_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_CountryRegion_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_CountryRegionABand_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_WirelessMode_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_MBSS_WirelessMode_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_Channel_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);
INT	Set_ShortSlot_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_TxPower_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_BGProtection_Proc(
	IN  struct rtmp_adapter *	pAd,
	IN  char *		arg);

INT Set_TxPreamble_Proc(
	IN  struct rtmp_adapter *	pAd,
	IN  char *		arg);

INT Set_RTSThreshold_Proc(
	IN  struct rtmp_adapter *	pAd,
	IN  char *		arg);

INT Set_FragThreshold_Proc(
	IN  struct rtmp_adapter *	pAd,
	IN  char *		arg);

INT Set_TxBurst_Proc(
	IN  struct rtmp_adapter *	pAd,
	IN  char *		arg);


#ifdef AGGREGATION_SUPPORT
INT	Set_PktAggregate_Proc(
	IN  struct rtmp_adapter *	pAd,
	IN  char *		arg);
#endif /* AGGREGATION_SUPPORT */

INT	Set_IEEE80211H_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

#ifdef EXT_BUILD_CHANNEL_LIST
INT Set_ExtCountryCode_Proc(
    IN  struct rtmp_adapter *  pAdapter,
    IN  char *         arg);

INT Set_ExtDfsType_Proc(
    IN      struct rtmp_adapter *  pAd,
    IN      char *        arg);

INT Set_ChannelListAdd_Proc(
    IN      struct rtmp_adapter *  pAd,
    IN      char *        arg);

INT Set_ChannelListShow_Proc(
    IN      struct rtmp_adapter *pAd,
    IN      char *		arg);
INT Set_ChannelListDel_Proc(
    IN      struct rtmp_adapter *pAd,
    IN      char *		arg);
#endif /* EXT_BUILD_CHANNEL_LIST */

#ifdef DBG
INT	Set_Debug_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_DebugFunc_Proc(
	IN struct rtmp_adapter*pAd,
	IN char *arg);
#endif

#ifdef TXBF_SUPPORT
INT	Set_ReadITxBf_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ReadETxBf_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_WriteITxBf_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_WriteETxBf_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_StatITxBf_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_StatETxBf_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_TxBfTag_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_ITxBfTimeout_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *         arg);

INT Set_ETxBfTimeout_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *         arg);

INT	Set_InvTxBfTag_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ITxBfCal_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ITxBfDivCal_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ITxBfLnaCal_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ETxBfEnCond_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_ETxBfCodebook_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *         arg);

INT Set_ETxBfCoefficient_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *         arg);

INT Set_ETxBfGrouping_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *         arg);

INT Set_ETxBfNoncompress_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *         arg);

INT Set_ETxBfIncapable_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *         arg);

INT	Set_NoSndgCntThrd_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_NdpSndgStreams_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_Trigger_Sounding_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ITxBfEn_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

#endif /* TXBF_SUPPORT */

INT Set_RateAdaptInterval(
	IN struct rtmp_adapter*pAd,
	IN char *arg);


#ifdef PRE_ANT_SWITCH
INT Set_PreAntSwitch_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *        arg);

INT Set_PreAntSwitchRSSI_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *        arg);

INT Set_PreAntSwitchTimeout_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *        arg);

#endif /* PRE_ANT_SWITCH */


#ifdef CFO_TRACK
INT Set_CFOTrack_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *        arg);

#ifdef CFO_TRACK
#endif /* CFO_TRACK */

#endif // CFO_TRACK //

#ifdef DBG_CTRL_SUPPORT
INT Set_DebugFlags_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *         arg);

#ifdef INCLUDE_DEBUG_QUEUE
INT Set_DebugQueue_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *        arg);

void dbQueueEnqueue(
	IN UCHAR type,
	IN UCHAR *data);

void dbQueueEnqueueTxFrame(
	IN UCHAR *pTxWI,
	IN UCHAR *pHeader_802_11);

void dbQueueEnqueueRxFrame(
	IN UCHAR *pRxWI,
	IN UCHAR *pHeader_802_11,
	IN ULONG flags);
#endif /* INCLUDE_DEBUG_QUEUE */
#endif /* DBG_CTRL_SUPPORT */

INT	Show_DescInfo_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Show_MacTable_Proc(struct rtmp_adapter*pAd, char **arg);
INT show_devinfo_proc(struct rtmp_adapter*pAd, char *arg);
INT show_trinfo_proc(struct rtmp_adapter*pAd, char *arg);

INT	Set_ResetStatCounter_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

#ifdef DOT11_N_SUPPORT
INT	Set_BASetup_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_BADecline_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_BAOriTearDown_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_BARecTearDown_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_HtBw_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_HtMcs_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_HtGi_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_HtOpMode_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_HtStbc_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_HtHtc_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_HtExtcha_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_HtMpduDensity_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_HtBaWinSize_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_HtRdg_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_HtLinkAdapt_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_HtAmsdu_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_HtAutoBa_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_HtProtect_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_HtMimoPs_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

#ifdef DOT11N_DRAFT3
INT Set_HT_BssCoex_Proc(
	IN	struct rtmp_adapter *	pAd,
	IN	char *			pParam);

INT Set_HT_BssCoexApCntThr_Proc(
	IN	struct rtmp_adapter *	pAd,
	IN	char *			pParam);
#endif /* DOT11N_DRAFT3 */



INT	Set_ForceShortGI_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_ForceGF_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	SetCommonHT(struct rtmp_adapter*pAd);

INT	Set_SendPSMPAction_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

void convert_reordering_packet_to_preAMSDU_or_802_3_packet(
	IN	struct rtmp_adapter *pAd,
	IN	RX_BLK			*pRxBlk,
	IN  UCHAR			FromWhichBSSID);

INT	Set_HtMIMOPSmode_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);


INT	Set_HtTxBASize_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_HtDisallowTKIP_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_BurstMode_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);
#endif /* DOT11_N_SUPPORT */


#ifdef DOT11_VHT_AC
INT Set_VhtBw_Proc(struct rtmp_adapter*pAd, char *arg);
INT Set_VhtStbc_Proc(struct rtmp_adapter*pAd, char *arg);
INT Set_VhtBwSignal_Proc(struct rtmp_adapter*pAd, char *arg);
#endif /* DOT11_VHT_AC */


#ifdef APCLI_SUPPORT
INT RTMPIoctlConnStatus(
    IN  struct rtmp_adapter *  pAdapter,
    IN  char *         arg);

#endif /*APCLI_SUPPORT*/




#ifdef CONFIG_STA_SUPPORT
void RTMPSendDLSTearDownFrame(
	IN	struct rtmp_adapter *pAd,
	IN	PUCHAR			pDA);

#ifdef DOT11_N_SUPPORT
void QueryBATABLE(
	IN  struct rtmp_adapter *pAd,
	OUT PQUERYBA_TABLE pBAT);
#endif /* DOT11_N_SUPPORT */

#ifdef WPA_SUPPLICANT_SUPPORT
INT	    WpaCheckEapCode(
	IN  struct rtmp_adapter *  	pAd,
	IN  PUCHAR				pFrame,
	IN  USHORT				FrameLen,
	IN  USHORT				OffSet);
#endif /* WPA_SUPPLICANT_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */



#ifdef DOT11_N_SUPPORT
void Handle_BSS_Width_Trigger_Events(struct rtmp_adapter*pAd);

void build_ext_channel_switch_ie(
	IN struct rtmp_adapter*pAd,
	IN HT_EXT_CHANNEL_SWITCH_ANNOUNCEMENT_IE *pIE);

void assoc_ht_info_debugshow(
	IN struct rtmp_adapter*pAd,
	IN MAC_TABLE_ENTRY *pEntry,
	IN UCHAR ht_cap_len,
	IN HT_CAPABILITY_IE *pHTCapability);
#endif /* DOT11_N_SUPPORT */

BOOLEAN APRxDoneInterruptHandle(struct rtmp_adapter*pAd);
BOOLEAN STARxDoneInterruptHandle(struct rtmp_adapter*pAd, BOOLEAN argc);
BOOLEAN RxDoneInterruptHandle(struct rtmp_adapter*pAd);

#ifdef DOT11_N_SUPPORT
/* AMPDU packet indication */
void Indicate_AMPDU_Packet(
	IN	struct rtmp_adapter *pAd,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);

#ifdef HDR_TRANS_SUPPORT
void Indicate_AMPDU_Packet_Hdr_Trns(
	IN	struct rtmp_adapter *pAd,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);
#endif /* HDR_TRANS_SUPPORT */

/* AMSDU packet indication */
void Indicate_AMSDU_Packet(
	IN	struct rtmp_adapter *pAd,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);

void BaReOrderingBufferMaintain(
    IN struct rtmp_adapter *pAd);
#endif /* DOT11_N_SUPPORT */

/* Normal legacy Rx packet indication */
void Indicate_Legacy_Packet(
	IN	struct rtmp_adapter *pAd,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);

#ifdef HDR_TRANS_SUPPORT
void Indicate_Legacy_Packet_Hdr_Trns(
	IN	struct rtmp_adapter *pAd,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);
#endif /* HDR_TRANS_SUPPORT */

void Indicate_EAPOL_Packet(
	IN	struct rtmp_adapter *pAd,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);

UINT deaggregate_AMSDU_announce(
	IN	struct rtmp_adapter *pAd,
	PNDIS_PACKET		pPacket,
	IN	PUCHAR			pData,
	IN	ULONG			DataSize,
	IN	UCHAR			OpMode);

#ifdef TXBF_SUPPORT
BOOLEAN clientSupportsETxBF(struct rtmp_adapter*pAd, HT_BF_CAP *pTxBFCap);
void setETxBFCap(struct rtmp_adapter*pAd, HT_BF_CAP *pTxBFCap);

#ifdef ETXBF_EN_COND3_SUPPORT
void txSndgSameMcs(struct rtmp_adapter*pAd, MAC_TABLE_ENTRY * pEnt, UCHAR smoothMfb);
void txSndgOtherGroup(struct rtmp_adapter*pAd, MAC_TABLE_ENTRY *pEntry);
void txMrqInvTxBF(struct rtmp_adapter*pAd, MAC_TABLE_ENTRY *pEntry);
void chooseBestMethod(struct rtmp_adapter*pAd, MAC_TABLE_ENTRY *pEntry, UCHAR mfb);
void rxBestSndg(struct rtmp_adapter*pAd, MAC_TABLE_ENTRY *pEntry);
#endif /* ETXBF_EN_COND3_SUPPORT */

void handleBfFb(struct rtmp_adapter*pAd, RX_BLK *pRxBlk);

void TxBFInit(
	IN struct rtmp_adapter *	pAd,
	IN MAC_TABLE_ENTRY	*pEntry,
	IN BOOLEAN			supportsETxBF);

void eTxBFProbing(
 	IN struct rtmp_adapter *	pAd,
	IN MAC_TABLE_ENTRY	*pEntry);

void Trigger_Sounding_Packet(
	IN	struct rtmp_adapter *pAd,
	IN	UCHAR			SndgType,
	IN	UCHAR			SndgBW,
	IN	UCHAR			SndgMcs,
	IN  MAC_TABLE_ENTRY *pEntry);

void rtmp_asic_set_bf(
	IN struct rtmp_adapter*pAd);

BOOLEAN rtmp_chk_itxbf_calibration(
	IN struct rtmp_adapter*pAd);

#endif /* TXBF_SUPPORT */

BOOLEAN CmdRspEventCallbackHandle(struct rtmp_adapter *pAd, PUCHAR pRspBuffer);


#ifdef CONFIG_STA_SUPPORT
/* remove LLC and get 802_3 Header */
#define  RTMP_802_11_REMOVE_LLC_AND_CONVERT_TO_802_3(_pRxBlk, _pHeader802_3)	\
{																				\
	PUCHAR _pRemovedLLCSNAP = NULL, _pDA, _pSA;                                 \
																				\
	if (RX_BLK_TEST_FLAG(_pRxBlk, fRX_WDS) || RX_BLK_TEST_FLAG(_pRxBlk, fRX_MESH)) \
	{                                                                           \
		_pDA = _pRxBlk->pHeader->Addr3;                                         \
		_pSA = (PUCHAR)_pRxBlk->pHeader + sizeof(HEADER_802_11);                \
	}                                                                           \
	else                                                                        \
	{                                                                           \
		if (RX_BLK_TEST_FLAG(_pRxBlk, fRX_INFRA))                              	\
		{                                                                       \
			_pDA = _pRxBlk->pHeader->Addr1;                                     \
		if (RX_BLK_TEST_FLAG(_pRxBlk, fRX_DLS))									\
			_pSA = _pRxBlk->pHeader->Addr2;										\
		else																	\
			_pSA = _pRxBlk->pHeader->Addr3;                                     \
		}                                                                       \
		else                                                                    \
		{                                                                       \
			_pDA = _pRxBlk->pHeader->Addr1;                                     \
			_pSA = _pRxBlk->pHeader->Addr2;                                     \
		}                                                                       \
	}                                                                           \
																				\
	CONVERT_TO_802_3(_pHeader802_3, _pDA, _pSA, _pRxBlk->pData, 				\
		_pRxBlk->DataSize, _pRemovedLLCSNAP);                                   \
}
#endif /* CONFIG_STA_SUPPORT */


BOOLEAN APFowardWirelessStaToWirelessSta(
	IN	struct rtmp_adapter *pAd,
	IN	PNDIS_PACKET	pPacket,
	IN	ULONG			FromWhichBSSID);

void Announce_or_Forward_802_3_Packet(
	IN	struct rtmp_adapter *pAd,
	IN	PNDIS_PACKET	pPacket,
	IN	UCHAR			FromWhichBSSID);

void Sta_Announce_or_Forward_802_3_Packet(
	IN	struct rtmp_adapter *pAd,
	IN	PNDIS_PACKET	pPacket,
	IN	UCHAR			FromWhichBSSID);


#ifdef CONFIG_STA_SUPPORT
#define ANNOUNCE_OR_FORWARD_802_3_PACKET(_pAd, _pPacket, _FromWhichBSS)\
			Sta_Announce_or_Forward_802_3_Packet(_pAd, _pPacket, _FromWhichBSS);
			/*announce_802_3_packet(_pAd, _pPacket); */
#endif /* CONFIG_STA_SUPPORT */




/* Normal, AMPDU or AMSDU */
void CmmRxnonRalinkFrameIndicate(
	IN struct rtmp_adapter*pAd,
	IN RX_BLK *pRxBlk,
	IN UCHAR FromWhichBSSID);

#ifdef HDR_TRANS_SUPPORT
void CmmRxnonRalinkFrameIndicate_Hdr_Trns(
	IN struct rtmp_adapter*pAd,
	IN RX_BLK *pRxBlk,
	IN UCHAR FromWhichBSSID);
#endif /* HDR_TRANS_SUPPORT */

void CmmRxRalinkFrameIndicate(
	IN struct rtmp_adapter*pAd,
	IN MAC_TABLE_ENTRY *pEntry,
	IN RX_BLK *pRxBlk,
	IN UCHAR FromWhichBSSID);

void Update_Rssi_Sample(
	IN struct rtmp_adapter*pAd,
	IN RSSI_SAMPLE *pRssi,
	IN RXWI_STRUC *pRxWI);

PNDIS_PACKET GetPacketFromRxRing(
	IN struct rtmp_adapter*pAd,
	OUT RX_BLK *pRxBlk,
	OUT BOOLEAN	 *pbReschedule,
	INOUT UINT32 *pRxPending,
	BOOLEAN *bCmdRspPacket);

PNDIS_PACKET RTMPDeFragmentDataFrame(
	IN struct rtmp_adapter*pAd,
	IN RX_BLK *pRxBlk);

/*////////////////////////////////////*/

#if defined (AP_SCAN_SUPPORT) || defined (CONFIG_STA_SUPPORT)
void RTMPIoctlGetSiteSurvey(
	IN	struct rtmp_adapter *pAdapter,
	IN	RTMP_IOCTL_INPUT_STRUCT *wrq);
#endif





#ifdef SNMP_SUPPORT
/*for snmp */
typedef struct _DefaultKeyIdxValue
{
	UCHAR	KeyIdx;
	UCHAR	Value[16];
} DefaultKeyIdxValue, *PDefaultKeyIdxValue;
#endif


#ifdef CONFIG_STA_SUPPORT

/* The radio capture header precedes the 802.11 header. */
typedef struct GNU_PACKED _ieee80211_radiotap_header {
    u8	it_version;	/* Version 0. Only increases
				 * for drastic changes,
				 * introduction of compatible
				 * new fields does not count.
				 */
    u8	it_pad;
    UINT16     it_len;         /* length of the whole
				 * header in bytes, including
				 * it_version, it_pad,
				 * it_len, and data fields.
				 */
    UINT32   it_present;	/* A bitmap telling which
					 * fields are present. Set bit 31
					 * (0x80000000) to extend the
					 * bitmap by another 32 bits.
					 * Additional extensions are made
					 * by setting bit 31.
					 */
}ieee80211_radiotap_header ;

enum ieee80211_radiotap_type {
    IEEE80211_RADIOTAP_TSFT = 0,
    IEEE80211_RADIOTAP_FLAGS = 1,
    IEEE80211_RADIOTAP_RATE = 2,
    IEEE80211_RADIOTAP_CHANNEL = 3,
    IEEE80211_RADIOTAP_FHSS = 4,
    IEEE80211_RADIOTAP_DBM_ANTSIGNAL = 5,
    IEEE80211_RADIOTAP_DBM_ANTNOISE = 6,
    IEEE80211_RADIOTAP_LOCK_QUALITY = 7,
    IEEE80211_RADIOTAP_TX_ATTENUATION = 8,
    IEEE80211_RADIOTAP_DB_TX_ATTENUATION = 9,
    IEEE80211_RADIOTAP_DBM_TX_POWER = 10,
    IEEE80211_RADIOTAP_ANTENNA = 11,
    IEEE80211_RADIOTAP_DB_ANTSIGNAL = 12,
    IEEE80211_RADIOTAP_DB_ANTNOISE = 13
};

#define WLAN_RADIOTAP_PRESENT (			\
	(1 << IEEE80211_RADIOTAP_TSFT)	|	\
	(1 << IEEE80211_RADIOTAP_FLAGS) |	\
	(1 << IEEE80211_RADIOTAP_RATE)  | 	\
	 0)

typedef struct _wlan_radiotap_header {
	ieee80211_radiotap_header wt_ihdr;
	INT64 wt_tsft;
	u8 wt_flags;
	u8 wt_rate;
} wlan_radiotap_header;
/* Definition from madwifi */


void STA_MonPktSend(
	IN	struct rtmp_adapter *pAd,
	IN	RX_BLK			*pRxBlk);

void    RTMPSetDesiredRates(
    IN  struct rtmp_adapter *  pAdapter,
    IN  LONG            Rates);

#ifdef XLINK_SUPPORT
INT Set_XlinkMode_Proc(
	IN	struct rtmp_adapter *pAdapter,
	IN	char *		arg);
#endif /* XLINK_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

INT	Set_FixedTxMode_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_LongRetryLimit_Proc(
	IN	struct rtmp_adapter *pAdapter,
	IN	char *		arg);

INT Set_ShortRetryLimit_Proc(
	IN	struct rtmp_adapter *pAdapter,
	IN	char *		arg);

INT Set_AutoFallBack_Proc(
	IN	struct rtmp_adapter *pAdapter,
	IN	char *		arg);


void RT28XXDMADisable(
	IN struct rtmp_adapter		*pAd);

void RT28XXDMAEnable(
	IN struct rtmp_adapter		*pAd);

void RT28xx_UpdateBeaconToAsic(
	IN struct rtmp_adapter* pAd,
	IN INT apidx,
	IN ULONG BeaconLen,
	IN ULONG UpdatePos);

void CfgInitHook(struct rtmp_adapter *pAd);


NDIS_STATUS RtmpNetTaskInit(
	IN struct rtmp_adapter*pAd);

void RtmpNetTaskExit(
	IN struct rtmp_adapter *pAd);

NDIS_STATUS RtmpMgmtTaskInit(
	IN struct rtmp_adapter*pAd);

void RtmpMgmtTaskExit(
	IN struct rtmp_adapter*pAd);

void tbtt_tasklet(unsigned long data);





#ifdef CONFIG_STA_SUPPORT
#ifdef CREDENTIAL_STORE
NDIS_STATUS RecoverConnectInfo(
	IN  struct rtmp_adapter*pAd);

NDIS_STATUS StoreConnectInfo(
	IN  struct rtmp_adapter*pAd);
#endif /* CREDENTIAL_STORE */
#endif /* CONFIG_STA_SUPPORT */

void AsicTurnOffRFClk(
	IN struct rtmp_adapter *   pAd,
	IN	UCHAR           Channel);



#ifdef RTMP_TIMER_TASK_SUPPORT
INT RtmpTimerQThread(
	IN ULONG Context);

RTMP_TIMER_TASK_ENTRY *RtmpTimerQInsert(
	IN struct rtmp_adapter*pAd,
	IN RALINK_TIMER_STRUCT *pTimer);

BOOLEAN RtmpTimerQRemove(
	IN struct rtmp_adapter*pAd,
	IN RALINK_TIMER_STRUCT *pTimer);

void RtmpTimerQExit(
	IN struct rtmp_adapter*pAd);

void RtmpTimerQInit(
	IN struct rtmp_adapter*pAd);
#endif /* RTMP_TIMER_TASK_SUPPORT */


#ifdef RTMP_MAC_USB

NTSTATUS RTUSBMultiRead(
	IN	struct rtmp_adapter *pAd,
	IN	USHORT			Offset,
	OUT	PUCHAR			pData,
	IN	USHORT			length);

NTSTATUS RTUSBMultiWrite(
	IN	struct rtmp_adapter *pAd,
	IN	USHORT			Offset,
	IN	PUCHAR			pData,
	IN	USHORT			length,
	IN	BOOLEAN			bWriteHigh);

NTSTATUS RTUSBMultiWrite_nBytes(
        IN      struct rtmp_adapter *  pAd,
        IN      USHORT                  Offset,
        IN      PUCHAR                  pData,
        IN      USHORT                  length,
        IN      USHORT                  batchLen);

NTSTATUS RTUSBMultiWrite_OneByte(
	IN	struct rtmp_adapter *pAd,
	IN	USHORT			Offset,
	IN	PUCHAR			pData);

NTSTATUS RTUSBReadBBPRegister(
	IN	struct rtmp_adapter *pAd,
	IN	UCHAR			Id,
	IN	PUCHAR			pValue);

NTSTATUS RTUSBWriteBBPRegister(
	IN	struct rtmp_adapter *pAd,
	IN	UCHAR			Id,
	IN	UCHAR			Value);

NTSTATUS RTUSBWriteRFRegister(
	IN	struct rtmp_adapter *pAd,
	IN	UINT32			Value);

NTSTATUS RTUSB_VendorRequest(
	IN	struct rtmp_adapter *pAd,
	IN	UINT32			TransferFlags,
	IN	UCHAR			ReservedBits,
	IN	UCHAR			Request,
	IN	USHORT			Value,
	IN	USHORT			Index,
	IN	void *		TransferBuffer,
	IN	UINT32			TransferBufferLength);

NTSTATUS RTUSBReadEEPROM(
	IN	struct rtmp_adapter *pAd,
	IN	USHORT			Offset,
	OUT	PUCHAR			pData,
	IN	USHORT			length);

NTSTATUS RTUSBWriteEEPROM(
	IN	struct rtmp_adapter *pAd,
	IN	USHORT			Offset,
	IN	PUCHAR			pData,
	IN	USHORT			length);

void RTUSBPutToSleep(
	IN	struct rtmp_adapter *pAd);

NTSTATUS RTUSBWakeUp(
	IN	struct rtmp_adapter *pAd);

NDIS_STATUS	RTUSBEnqueueCmdFromNdis(
	IN	struct rtmp_adapter *pAd,
	IN	NDIS_OID		Oid,
	IN	BOOLEAN			SetInformation,
	IN	void *		pInformationBuffer,
	IN	UINT32			InformationBufferLength);

void RTUSBDequeueCmd(
	IN	PCmdQ		cmdq,
	OUT	PCmdQElmt	*pcmdqelmt);

INT RTUSBCmdThread(
	IN ULONG Context);

void RTUSBBssBeaconExit(
	IN struct rtmp_adapter*pAd);

void RTUSBBssBeaconStop(
	IN struct rtmp_adapter*pAd);

void RTUSBBssBeaconStart(
	IN struct rtmp_adapter* pAd);

void RTUSBBssBeaconInit(
	IN struct rtmp_adapter*pAd);

void RTUSBWatchDog(
	IN struct rtmp_adapter*pAd);

NTSTATUS RTUSBWriteMACRegister(
	IN	struct rtmp_adapter *pAd,
	IN	USHORT			Offset,
	IN	UINT32			Value,
	IN	BOOLEAN			bWriteHigh);

NTSTATUS RTUSBReadMACRegister(
	IN	struct rtmp_adapter *pAd,
	IN	USHORT			Offset,
	OUT	PUINT32			pValue);

NTSTATUS RTUSBSingleWrite(
	IN 	struct rtmp_adapter	*pAd,
	IN	USHORT			Offset,
	IN	USHORT			Value,
	IN	BOOLEAN			bWriteHigh);

NTSTATUS RTUSBFirmwareWrite(
	IN struct rtmp_adapter *pAd,
	IN PUCHAR		pFwImage,
	IN ULONG		FwLen);

NTSTATUS	RTUSBVenderReset(
	IN	struct rtmp_adapter *pAd);

NDIS_STATUS RTUSBSetHardWareRegister(
	IN	struct rtmp_adapter *pAdapter,
	IN	void *		pBuf);

NDIS_STATUS RTUSBQueryHardWareRegister(
	IN	struct rtmp_adapter *pAdapter,
	IN	void *		pBuf);

/*void CMDHandler( */
/*    IN struct rtmp_adapter *pAd); */

NDIS_STATUS	RTUSBWriteHWMACAddress(
	IN struct rtmp_adapter*pAd);

void MlmeSetPsm(
	IN struct rtmp_adapter*pAd,
	IN USHORT psm);

NDIS_STATUS RTMPWPAAddKeyProc(
	IN struct rtmp_adapter*pAd,
	IN void *pBuf);

void AsicRxAntEvalAction(
	IN struct rtmp_adapter*pAd);

void append_pkt(
	IN struct rtmp_adapter*pAd,
	IN UCHAR *pHeader802_3,
	IN UINT HdrLen,
	IN UCHAR *pData,
	IN ULONG DataSize,
	OUT PNDIS_PACKET *ppPacket);


void RTUSBMlmeHardTransmit(
	IN struct rtmp_adapter*pAd,
	IN MGMT_STRUC *pMgmt);

INT MlmeThread(ULONG Context);


/*
	Function Prototype in rtusb_data.c
*/
NDIS_STATUS	RTUSBFreeDescRequest(
	IN struct rtmp_adapter*pAd,
	IN UCHAR BulkOutPipeId,
	IN UINT32 req_cnt);


BOOLEAN	RTUSBNeedQueueBackForAgg(
	IN struct rtmp_adapter*pAd,
	IN UCHAR		BulkOutPipeId);


/* Function Prototype in cmm_data_usb.c */
USHORT RtmpUSB_WriteSubTxResource(
	IN	struct rtmp_adapter *pAd,
	IN	TX_BLK			*pTxBlk,
	IN	BOOLEAN			bIsLast,
	OUT	USHORT			*FreeNumber);

USHORT RtmpUSB_WriteSingleTxResource(
	IN	struct rtmp_adapter *pAd,
	IN	TX_BLK			*pTxBlk,
	IN	BOOLEAN			bIsLast,
	OUT	USHORT			*FreeNumber);

USHORT	RtmpUSB_WriteFragTxResource(
	IN	struct rtmp_adapter *pAd,
	IN	TX_BLK			*pTxBlk,
	IN	UCHAR			fragNum,
	OUT	USHORT			*FreeNumber);

USHORT RtmpUSB_WriteMultiTxResource(
	IN	struct rtmp_adapter *pAd,
	IN	TX_BLK			*pTxBlk,
	IN	UCHAR			frameNum,
	OUT	USHORT			*FreeNumber);

void RtmpUSB_FinalWriteTxResource(
	IN	struct rtmp_adapter *pAd,
	IN	TX_BLK			*pTxBlk,
	IN	USHORT			totalMPDUSize,
	IN	USHORT			TxIdx);

void RtmpUSBDataLastTxIdx(
	IN	struct rtmp_adapter *pAd,
	IN	UCHAR			QueIdx,
	IN	USHORT			TxIdx);

void RtmpUSBDataKickOut(
	IN	struct rtmp_adapter *pAd,
	IN	TX_BLK			*pTxBlk,
	IN	UCHAR			QueIdx);

int RtmpUSBMgmtKickOut(
	IN struct rtmp_adapter	*pAd,
	IN UCHAR 			QueIdx,
	IN PNDIS_PACKET		pPacket,
	IN PUCHAR			pSrcBufVA,
	IN UINT 			SrcBufLen);

void RtmpUSBNullFrameKickOut(
	IN struct rtmp_adapter*pAd,
	IN UCHAR		QueIdx,
	IN UCHAR		*pNullFrame,
	IN UINT32		frameLen);

void RtmpUsbStaAsicForceWakeupTimeout(
	IN void *SystemSpecific1,
	IN void *FunctionContext,
	IN void *SystemSpecific2,
	IN void *SystemSpecific3);

void RT28xxUsbStaAsicForceWakeup(
	IN struct rtmp_adapter *pAd,
	IN BOOLEAN       bFromTx);

void RT28xxUsbStaAsicSleepThenAutoWakeup(
	IN struct rtmp_adapter *pAd,
	IN USHORT TbttNumToNextWakeUp);

void RT28xxUsbMlmeRadioOn(
	IN struct rtmp_adapter *pAd);

void RT28xxUsbMlmeRadioOFF(
	IN struct rtmp_adapter *pAd);

void RT28xxUsbAsicRadioOff(
	IN struct rtmp_adapter *pAd);

void RT28xxUsbAsicRadioOn(
	IN struct rtmp_adapter *pAd);

BOOLEAN AsicCheckCommandOk(
	IN struct rtmp_adapter *pAd,
	IN UCHAR		 Command);

void RT28xxUsbAsicWOWEnable(
	IN struct rtmp_adapter *pAd);

void RT28xxUsbAsicWOWDisable(
	IN struct rtmp_adapter *pAd);

#endif /* RTMP_MAC_USB */

NDIS_STATUS RTMPCheckRxError(
	IN struct rtmp_adapter*pAd,
	IN PHEADER_802_11 pHeader,
	IN RXWI_STRUC *pRxWI,
	IN RXINFO_STRUC *pRxInfo);


/*////////////////////////////////////*/

#ifdef AP_QLOAD_SUPPORT
void QBSS_LoadInit(
 	IN		struct rtmp_adapter*pAd);

void QBSS_LoadAlarmReset(
 	IN		struct rtmp_adapter*pAd);

void QBSS_LoadAlarmResume(
 	IN		struct rtmp_adapter*pAd);

UINT32 QBSS_LoadBusyTimeGet(
 	IN		struct rtmp_adapter*pAd);

BOOLEAN QBSS_LoadIsAlarmIssued(
 	IN		struct rtmp_adapter*pAd);

BOOLEAN QBSS_LoadIsBusyTimeAccepted(
 	IN		struct rtmp_adapter*pAd,
	IN		UINT32			BusyTime);

UINT32 QBSS_LoadElementAppend(
 	IN		struct rtmp_adapter*pAd,
	OUT		u8			*buf_p);

UINT32 QBSS_LoadElementParse(
 	IN		struct rtmp_adapter*pAd,
	IN		u8			*pElement,
	OUT		UINT16			*pStationCount,
	OUT		u8			*pChanUtil,
	OUT		UINT16			*pAvalAdmCap);

void QBSS_LoadUpdate(
 	IN		struct rtmp_adapter*pAd,
	IN		ULONG			UpTime);

void QBSS_LoadStatusClear(
 	IN		struct rtmp_adapter*pAd);

INT	Show_QoSLoad_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);
#endif /* AP_QLOAD_SUPPORT */

/*///////////////////////////////////*/
INT RTMPShowCfgValue(
	IN	struct rtmp_adapter *pAd,
	IN	char *		pName,
	IN	char *		pBuf,
	IN	UINT32			MaxLen);

char *RTMPGetRalinkAuthModeStr(
    IN  NDIS_802_11_AUTHENTICATION_MODE authMode);

char *RTMPGetRalinkEncryModeStr(
    IN  USHORT encryMode);
/*//////////////////////////////////*/

#ifdef CONFIG_STA_SUPPORT
void AsicStaBbpTuning(
	IN struct rtmp_adapter *pAd);

BOOLEAN StaAddMacTableEntry(
	IN  struct rtmp_adapter *	pAd,
	IN  PMAC_TABLE_ENTRY	pEntry,
	IN  UCHAR				MaxSupportedRateIn500Kbps,
	IN  HT_CAPABILITY_IE	*pHtCapability,
	IN  UCHAR				HtCapabilityLen,
	IN  ADD_HT_INFO_IE		*pAddHtInfo,
	IN  UCHAR				AddHtInfoLen,
	IN IE_LISTS *ie_list,
	IN  USHORT        		CapabilityInfo);


BOOLEAN	AUTH_ReqSend(
	IN  struct rtmp_adapter *		pAd,
	IN  PMLME_QUEUE_ELEM	pElem,
	IN  PRALINK_TIMER_STRUCT pAuthTimer,
	IN  char *			pSMName,
	IN  USHORT				SeqNo,
	IN  PUCHAR				pNewElement,
	IN  ULONG				ElementLen);
#endif /* CONFIG_STA_SUPPORT */


void ReSyncBeaconTime(struct rtmp_adapter*pAd);
void RTMPSetAGCInitValue(struct rtmp_adapter*pAd, UCHAR BandWidth);

#ifdef TXBF_SUPPORT
void handleHtcField(
	IN	struct rtmp_adapter *pAd,
	IN	RX_BLK			*pRxBlk);
#endif /* TXBF_SUPPORT */

#ifdef MFB_SUPPORT
void MFB_PerPareMRQ(
	IN	struct rtmp_adapter *pAd,
	OUT	void *pBuf,
	IN	PMAC_TABLE_ENTRY pEntry);

void MFB_PerPareMFB(
	IN	struct rtmp_adapter *pAd,
	OUT	void *pBuf,
	IN	PMAC_TABLE_ENTRY pEntry);
#endif /* MFB_SUPPORT */

#define VIRTUAL_IF_INC(__pAd) ((__pAd)->VirtualIfCnt++)
#define VIRTUAL_IF_DEC(__pAd) ((__pAd)->VirtualIfCnt--)
#define VIRTUAL_IF_NUM(__pAd) ((__pAd)->VirtualIfCnt)



#ifdef RTMP_USB_SUPPORT
/*
 * Function Prototype in rtusb_bulk.c
 */

#ifdef INF_AMAZON_SE
void SoftwareFlowControl(
	IN struct rtmp_adapter *pAd) ;
#endif /* INF_AMAZON_SE */


void RTUSBInitTxDesc(
	IN	struct rtmp_adapter *pAd,
	IN	PTX_CONTEXT		pTxContext,
	IN	UCHAR			BulkOutPipeId,
	IN	usb_complete_t	Func);

void RTUSBInitHTTxDesc(
	IN	struct rtmp_adapter *pAd,
	IN	PHT_TX_CONTEXT	pTxContext,
	IN	UCHAR			BulkOutPipeId,
	IN	ULONG			BulkOutSize,
	IN	usb_complete_t	Func);

void RTUSBInitRxDesc(
	IN	struct rtmp_adapter *pAd,
	IN	PRX_CONTEXT		pRxContext);

void RTUSBCleanUpDataBulkOutQueue(
	IN	struct rtmp_adapter *pAd);

void RTUSBCancelPendingBulkOutIRP(
	IN	struct rtmp_adapter *pAd);

void RTUSBBulkOutDataPacket(
	IN	struct rtmp_adapter *pAd,
	IN	UCHAR			BulkOutPipeId,
	IN	UCHAR			Index);

void RTUSBBulkOutNullFrame(
	IN	struct rtmp_adapter *pAd);

void RTUSBBulkOutRTSFrame(
	IN	struct rtmp_adapter *pAd);

void RTUSBCancelPendingBulkInIRP(
	IN	struct rtmp_adapter *pAd);

void RTUSBCancelPendingIRPs(
	IN	struct rtmp_adapter *pAd);

void RTUSBBulkOutMLMEPacket(
	IN	struct rtmp_adapter *pAd,
	IN	UCHAR			Index);

void RTUSBBulkOutPsPoll(
	IN	struct rtmp_adapter *pAd);

void RTUSBCleanUpMLMEBulkOutQueue(
	IN	struct rtmp_adapter *pAd);

void RTUSBKickBulkOut(
	IN	struct rtmp_adapter *pAd);

void RTUSBBulkReceive(
	IN	struct rtmp_adapter *pAd);

void RTUSBBulkCmdRspEventReceive(
	IN struct rtmp_adapter *pAd);

void DoBulkIn(
	IN struct rtmp_adapter*pAd);

void RTUSBInitRxDesc(
	IN	struct rtmp_adapter *pAd,
	IN  PRX_CONTEXT		pRxContext);

void RTUSBBulkRxHandle(
	IN unsigned long data);
#endif /* RTMP_USB_SUPPORT */


#ifdef SOFT_ENCRYPT
BOOLEAN RTMPExpandPacketForSwEncrypt(
	IN  struct rtmp_adapter *  pAd,
	IN	PTX_BLK			pTxBlk);

void RTMPUpdateSwCacheCipherInfo(
	IN  struct rtmp_adapter *  pAd,
	IN	PTX_BLK			pTxBlk,
	IN	PUCHAR			pHdr);
#endif /* SOFT_ENCRYPT */


/*
	OS Related funciton prototype definitions.
	TODO: Maybe we need to move these function prototypes to other proper place.
*/

void RTInitializeCmdQ(
	IN	PCmdQ	cmdq);

INT RTPCICmdThread(
	IN ULONG Context);

void CMDHandler(
    IN struct rtmp_adapter *pAd);

void RTThreadDequeueCmd(
	IN	PCmdQ		cmdq,
	OUT	PCmdQElmt	*pcmdqelmt);

NDIS_STATUS RTEnqueueInternalCmd(
	IN struct rtmp_adapter *pAd,
	IN NDIS_OID			Oid,
	IN void *		pInformationBuffer,
	IN UINT32			InformationBufferLength);

#ifdef HOSTAPD_SUPPORT
void ieee80211_notify_michael_failure(
	IN	struct rtmp_adapter *   pAd,
	IN	PHEADER_802_11   pHeader,
	IN	UINT            keyix,
	IN	INT              report);

const CHAR* ether_sprintf(const u8 *mac);
#endif/*HOSTAPD_SUPPORT*/

#ifdef VENDOR_FEATURE3_SUPPORT
void RTMP_IO_WRITE32(
	struct rtmp_adapter *pAd,
	UINT32 Offset,
	UINT32 Value);

void RTMP_BBP_IO_READ8_BY_REG_ID(
	struct rtmp_adapter *pAd,
	UINT32 Offset,
	u8 *pValue);

void RTMP_BBP_IO_READ8(
	struct rtmp_adapter *pAd,
	UCHAR Offset,
	u8 *pValue,
	BOOLEAN FlgValidMCR);

void RTMP_BBP_IO_WRITE8_BY_REG_ID(
	struct rtmp_adapter *pAd,
	UINT32 Offset,
	u8 Value);

void RTMP_BBP_IO_WRITE8(
	struct rtmp_adapter *pAd,
	UCHAR Offset,
	u8 Value,
	BOOLEAN FlgValidMCR);
#endif /* VENDOR_FEATURE3_SUPPORT */


INT AsicGetMacVersion(
	IN struct rtmp_adapter*pAd);

INT WaitForAsicReady(
	IN struct rtmp_adapter*pAd);

BOOLEAN CHAN_PropertyCheck(
	IN struct rtmp_adapter *pAd,
	IN UINT32			ChanNum,
	IN UCHAR			Property);

#ifdef CONFIG_STA_SUPPORT

/* command */
INT Set_SSID_Proc(
    IN  struct rtmp_adapter *  pAdapter,
    IN  char *         arg);

#ifdef WMM_SUPPORT
INT	Set_WmmCapable_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);
#endif

INT Set_NetworkType_Proc(
    IN  struct rtmp_adapter *  pAdapter,
    IN  char *         arg);

INT Set_AuthMode_Proc(
    IN  struct rtmp_adapter *  pAdapter,
    IN  char *         arg);

INT Set_EncrypType_Proc(
    IN  struct rtmp_adapter *  pAdapter,
    IN  char *         arg);

INT Set_DefaultKeyID_Proc(
    IN  struct rtmp_adapter *  pAdapter,
    IN  char *         arg);

INT Set_Wep_Key_Proc(struct rtmp_adapter *pAd, char *Key, INT KeyLen, INT KeyId);

INT Set_Key1_Proc(
    IN  struct rtmp_adapter *  pAdapter,
    IN  char *         arg);

INT Set_Key2_Proc(
    IN  struct rtmp_adapter *  pAdapter,
    IN  char *         arg);

INT Set_Key3_Proc(
    IN  struct rtmp_adapter *  pAdapter,
    IN  char *         arg);

INT Set_Key4_Proc(
    IN  struct rtmp_adapter *  pAdapter,
    IN  char *         arg);

INT Set_WPAPSK_Proc(
    IN  struct rtmp_adapter *  pAdapter,
    IN  char *         arg);


INT Set_PSMode_Proc(
    IN  struct rtmp_adapter *  pAdapter,
    IN  char *         arg);

#ifdef WPA_SUPPLICANT_SUPPORT
INT Set_Wpa_Support(
    IN	struct rtmp_adapter *pAd,
	IN	char *		arg);
#endif /* WPA_SUPPLICANT_SUPPORT */

#ifdef DBG

void RTMPIoctlMAC(
	IN	struct rtmp_adapter *pAdapter,
	IN	RTMP_IOCTL_INPUT_STRUCT	*wrq);

void RTMPIoctlE2PROM(
    IN  struct rtmp_adapter *  pAdapter,
    IN  RTMP_IOCTL_INPUT_STRUCT *wrq);
#endif /* DBG */


NDIS_STATUS RTMPWPANoneAddKeyProc(
    IN  struct rtmp_adapter *  pAd,
    IN	void *		pBuf);

INT Set_FragTest_Proc(
    IN  struct rtmp_adapter *  pAdapter,
    IN  char *         arg);

#ifdef DOT11_N_SUPPORT
INT Set_TGnWifiTest_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *         arg);
#endif /* DOT11_N_SUPPORT */

INT Set_LongRetryLimit_Proc(
	IN	struct rtmp_adapter *pAdapter,
	IN	char *		arg);

INT Set_ShortRetryLimit_Proc(
	IN	struct rtmp_adapter *pAdapter,
	IN	char *		arg);

#ifdef EXT_BUILD_CHANNEL_LIST
INT Set_Ieee80211dClientMode_Proc(
    IN  struct rtmp_adapter *  pAdapter,
    IN  char *         arg);
#endif /* EXT_BUILD_CHANNEL_LIST */

INT	Show_Adhoc_MacTable_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		extra,
	IN	UINT32			size);



INT Set_BeaconLostTime_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *        arg);

INT Set_AutoRoaming_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *        arg);

INT Set_SiteSurvey_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_ForceTxBurst_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *        arg);

void RTMPAddKey(
	IN	struct rtmp_adapter *    pAd,
	IN	PNDIS_802_11_KEY    pKey);


void StaSiteSurvey(
	IN	struct rtmp_adapter * 		pAd,
	IN	PNDIS_802_11_SSID	pSsid,
	IN	UCHAR				ScanType);

void MaintainBssTable(
	IN  struct rtmp_adapter *pAd,
	IN OUT	BSS_TABLE *Tab,
	IN  ULONG	MaxRxTimeDiff,
	IN  UCHAR	MaxSameRxTimeCount);
#endif /* CONFIG_STA_SUPPORT */

void  getRate(
    IN HTTRANSMIT_SETTING HTSetting,
    OUT ULONG* fLastTxRxRate);


#ifdef APCLI_SUPPORT
#ifdef APCLI_WPA_SUPPLICANT_SUPPORT
void    ApcliSendAssocIEsToWpaSupplicant(
    IN  struct rtmp_adapter *pAd,
    IN UINT ifIndex);

INT	    ApcliWpaCheckEapCode(
	IN  struct rtmp_adapter *  		pAd,
	IN  PUCHAR				pFrame,
	IN  USHORT				FrameLen,
	IN  USHORT				OffSet);

void    ApcliWpaSendEapolStart(
	IN	struct rtmp_adapter *pAd,
	IN  PUCHAR          pBssid,
	IN  PMAC_TABLE_ENTRY pMacEntry,
	IN	PAPCLI_STRUCT pApCliEntry);


void ApCliRTMPSendNullFrame(
	IN	struct rtmp_adapter *pAd,
	IN	UCHAR			TxRate,
	IN	BOOLEAN 		bQosNull,
	IN PMAC_TABLE_ENTRY pMacEntry);

#endif/*APCLI_WPA_SUPPLICANT_SUPPORT*/
#endif/*APCLI_SUPPORT*/


void RTMP_IndicateMediaState(
	IN	struct rtmp_adapter *	pAd,
	IN  NDIS_MEDIA_STATE	media_state);

#if defined(RT3350) || defined(RT33xx)
void RTMP_TxEvmCalibration(
	IN struct rtmp_adapter *pAd);
#endif /* defined(RT3350) || defined(RT33xx) */

INT RTMPSetInformation(
    IN struct rtmp_adapter*pAd,
    IN OUT RTMP_IOCTL_INPUT_STRUCT *rq,
    IN INT cmd);

INT RTMPQueryInformation(
    IN struct rtmp_adapter*pAd,
    INOUT RTMP_IOCTL_INPUT_STRUCT *rq,
    IN INT cmd);

void RTMPIoctlShow(
	IN struct rtmp_adapter*pAd,
	IN RTMP_IOCTL_INPUT_STRUCT *rq,
	IN UINT32 subcmd,
	IN void *pData,
	IN ULONG Data);

INT RTMP_COM_IoctlHandle(
	IN void *pAdSrc,
	IN RTMP_IOCTL_INPUT_STRUCT *wrq,
	IN INT cmd,
	IN USHORT subcmd,
	IN void *pData,
	IN ULONG Data);



INT Set_VcoPeriod_Proc(
	IN struct rtmp_adapter*pAd,
	IN char *arg);

INT	Set_RateAlg_Proc(
	IN struct rtmp_adapter*pAd,
	IN char *arg);

#ifdef SINGLE_SKU
INT Set_ModuleTxpower_Proc(
	IN struct rtmp_adapter*pAd,
	IN char *arg);
#endif /* SINGLE_SKU */

void RtmpEnqueueNullFrame(
	IN struct rtmp_adapter *pAd,
	IN PUCHAR        pAddr,
	IN UCHAR         TxRate,
	IN UCHAR         PID,
	IN UCHAR         apidx,
    IN BOOLEAN       bQosNull,
    IN BOOLEAN       bEOSP,
    IN UCHAR         OldUP);

void RtmpCleanupPsQueue(
	IN  struct rtmp_adapter *  pAd,
	IN  PQUEUE_HEADER   pQueue);

#ifdef CONFIG_STA_SUPPORT
BOOLEAN RtmpPktPmBitCheck(
    IN  struct rtmp_adapter *      pAd);

void RtmpPsActiveExtendCheck(
	IN struct rtmp_adapter *	pAd);

void RtmpPsModeChange(
	IN struct rtmp_adapter *	pAd,
	IN UINT32				PsMode);
#endif /* CONFIG_STA_SUPPORT */

UCHAR dot11_2_ra_rate(UCHAR MaxSupportedRateIn500Kbps);
UCHAR dot11_max_sup_rate(INT SupRateLen, UCHAR *SupRate, INT ExtRateLen, UCHAR *ExtRate);

void mgmt_tb_set_mcast_entry(struct rtmp_adapter*pAd);
void set_entry_phy_cfg(struct rtmp_adapter*pAd, MAC_TABLE_ENTRY *pEntry);
void MacTableReset(struct rtmp_adapter*pAd);
MAC_TABLE_ENTRY *MacTableLookup(struct rtmp_adapter*pAd, UCHAR *pAddr);
BOOLEAN MacTableDeleteEntry(struct rtmp_adapter*pAd, USHORT wcid, UCHAR *pAddr);
MAC_TABLE_ENTRY *MacTableInsertEntry(
    IN struct rtmp_adapter*pAd,
    IN UCHAR *pAddr,
	IN UCHAR apidx,
	IN UCHAR OpMode,
	IN BOOLEAN CleanAll);

#ifdef MAC_REPEATER_SUPPORT
MAC_TABLE_ENTRY *InsertMacRepeaterEntry(
	IN  struct rtmp_adapter *  pAd,
	IN  PUCHAR			pAddr,
	IN  UCHAR			IfIdx);
#endif /* MAC_REPEATER_SUPPORT */

void dumpTxWI(struct rtmp_adapter*pAd, TXWI_STRUC *pTxWI);
void dump_rxwi(struct rtmp_adapter*pAd, RXWI_STRUC *pRxWI);
void dump_txinfo(struct rtmp_adapter*pAd, TXINFO_STRUC *pTxInfo);
void dump_rxinfo(struct rtmp_adapter*pAd, RXINFO_STRUC *pRxInfo);
void dumpRxFCEInfo(struct rtmp_adapter*pAd, RXFCE_INFO *pRxFceInfo);



#ifdef CONFIG_FPGA_MODE
INT set_tr_stop(struct rtmp_adapter*pAd, char *arg);
INT set_tx_kickcnt(struct rtmp_adapter*pAd, char *arg);
INT set_data_phy_mode(struct rtmp_adapter*pAd, char *arg);
INT set_data_bw(struct rtmp_adapter*pAd, char *arg);
INT set_data_ldpc(struct rtmp_adapter*pAd, char *arg);
INT set_data_mcs(struct rtmp_adapter*pAd, char *arg);
INT set_data_gi(struct rtmp_adapter*pAd, char *arg);
INT set_data_basize(struct rtmp_adapter*pAd, char *arg);
INT set_fpga_mode(struct rtmp_adapter*pAd, char *arg);

#ifdef CAPTURE_MODE
void cap_mode_init(struct rtmp_adapter*pAd);
void cap_mode_deinit(struct rtmp_adapter*pAd);
INT set_cap_start(struct rtmp_adapter*pAd, char *arg);
INT set_cap_trigger(struct rtmp_adapter*pAd, char *arg);
INT asic_cap_start(struct rtmp_adapter*pAd);
INT asic_cap_stop(struct rtmp_adapter*pAd);
INT cap_status_chk_and_get(struct rtmp_adapter*pAd);
INT set_cap_dump(struct rtmp_adapter*pAd, char *arg);
void cap_dump(struct rtmp_adapter*pAd, CHAR *bank1, CHAR *bank2, INT len);
#endif /* CAPTURE_MODE */
#endif /* CONFIG_FPGA_MODE */

#ifdef WFA_VHT_PF
INT set_force_amsdu(struct rtmp_adapter*pAd, char *arg);
INT set_force_noack(struct rtmp_adapter*pAd, char *arg);
INT set_force_vht_sgi(struct rtmp_adapter*pAd, char *arg);
INT set_force_vht_tx_stbc(struct rtmp_adapter*pAd, char *arg);
INT set_force_ext_cca(struct rtmp_adapter*pAd, char *arg);
#ifdef IP_ASSEMBLY
INT set_force_ip_assemble(struct rtmp_adapter*pAd, char *arg);
#endif /* IP_ASSEMBLY */
#endif /* WFA_VHT_PF */



#ifdef RLT_RF
INT set_rf(struct rtmp_adapter*pAd, char *arg);
#endif /* RLT_RF */

#ifdef RTMP_MAC_USB
BOOLEAN CmdRspEventHandle(struct rtmp_adapter*pAd);
#endif /* RTMP_MAC_USB */

#endif  /* __RTMP_H__ */

#ifdef MT76x0
INT set_temp_sensor_proc(
	IN struct rtmp_adapter	*pAd,
	IN char *		arg);
#endif /* MT76x0 */
