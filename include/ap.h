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


#ifndef __AP_H__
#define __AP_H__




/* ============================================================= */
/*      Common definition */
/* ============================================================= */
#define MBSS_VLAN_INFO_GET(												\
	__pAd, __VLAN_VID, __VLAN_Priority, __FromWhichBSSID) 				\
{																		\
	if ((__FromWhichBSSID < __pAd->ApCfg.BssidNum) &&					\
		(__FromWhichBSSID < HW_BEACON_MAX_NUM) &&						\
		(__pAd->ApCfg.MBSSID[__FromWhichBSSID].VLAN_VID != 0))			\
	{																	\
		__VLAN_VID = __pAd->ApCfg.MBSSID[__FromWhichBSSID].VLAN_VID;	\
		__VLAN_Priority = __pAd->ApCfg.MBSSID[__FromWhichBSSID].VLAN_Priority; \
	}																	\
}

/* ============================================================= */
/*      Function Prototypes */
/* ============================================================= */

/* ap_data.c */

bool APBridgeToWirelessSta(
    IN  struct rtmp_adapter *  pAd,
    IN  u8 *         pHeader,
    IN  UINT            HdrLen,
    IN  u8 *         pData,
    IN  UINT            DataLen,
    IN  ULONG           fromwdsidx);

void RTMP_BASetup(
	IN struct rtmp_adapter *pAd,
	IN PMAC_TABLE_ENTRY pMacEntry,
	IN u8 UserPriority);

void APSendPackets(
	IN	struct rtmp_adapter *  pAd,
	IN	struct sk_buff **ppPacketArray,
	IN	UINT			NumberOfPackets);

int APSendPacket(
    IN  struct rtmp_adapter *  pAd,
    IN  struct sk_buff *    pPacket);

int APInsertPsQueue(
	IN struct rtmp_adapter *pAd,
	IN struct sk_buff * pPacket,
	IN MAC_TABLE_ENTRY *pMacEntry,
	IN u8 QueIdx);

int APHardTransmit(
	IN	struct rtmp_adapter *pAd,
	IN	TX_BLK			*pTxBlk,
	IN	u8 		QueIdx);

void APRxEAPOLFrameIndicate(
	IN	struct rtmp_adapter *pAd,
	IN	MAC_TABLE_ENTRY	*pEntry,
	IN	RX_BLK			*pRxBlk,
	IN	u8 		FromWhichBSSID);

int APCheckRxError(
	IN struct rtmp_adapter*pAd,
	IN struct mt7610u_rxinfo *pRxInfo,
	IN u8 Wcid);

bool APCheckClass2Class3Error(
    IN  struct rtmp_adapter *  pAd,
	IN ULONG Wcid,
	IN  PHEADER_802_11  pHeader);

void APHandleRxPsPoll(
	IN	struct rtmp_adapter *pAd,
	IN	u8 *		pAddr,
	IN	USHORT			Aid,
    IN	bool 		isActive);

void    RTMPDescriptorEndianChange(
    IN  u8 *         pData,
    IN  ULONG           DescriptorType);

void    RTMPFrameEndianChange(
    IN  struct rtmp_adapter *  pAd,
    IN  u8 *         pData,
    IN  ULONG           Dir,
    IN  bool         FromRxDoneInt);

/* ap_assoc.c */

void APAssocStateMachineInit(
    IN  struct rtmp_adapter *  pAd,
    IN  STATE_MACHINE *S,
    OUT STATE_MACHINE_FUNC Trans[]);


void MbssKickOutStas(
	IN struct rtmp_adapter *pAd,
	IN INT apidx,
	IN USHORT Reason);

void APMlmeKickOutSta(
    IN struct rtmp_adapter *pAd,
	IN u8 *pStaAddr,
	IN u8 Wcid,
	IN USHORT Reason);



void  APCls3errAction(
    IN  struct rtmp_adapter *  pAd,
	IN 	ULONG Wcid,
    IN	PHEADER_802_11	pHeader);

/*
void RTMPAddClientSec(
	IN	struct rtmp_adapter *pAd,
	IN	u8 BssIdx,
	IN u8 	 KeyIdx,
	IN u8 	 CipherAlg,
	IN u8 *	 pKey,
	IN u8 *	 pTxMic,
	IN u8 *	 pRxMic,
	IN MAC_TABLE_ENTRY *pEntry);
*/

/* ap_auth.c */

void APAuthStateMachineInit(
    IN struct rtmp_adapter *pAd,
    IN STATE_MACHINE *Sm,
    OUT STATE_MACHINE_FUNC Trans[]);

void APCls2errAction(
    IN struct rtmp_adapter *pAd,
	IN 	ULONG Wcid,
    IN	PHEADER_802_11	pHeader);

/* ap_connect.c */


void APMakeBssBeacon(
    IN  struct rtmp_adapter *  pAd,
	IN	INT				apidx);

void  APUpdateBeaconFrame(
    IN  struct rtmp_adapter *  pAd,
	IN	INT				apidx);

void APMakeAllBssBeacon(
    IN  struct rtmp_adapter *  pAd);

void  APUpdateAllBeaconFrame(
    IN  struct rtmp_adapter *  pAd);


/* ap_sync.c */

void APSyncStateMachineInit(
    IN struct rtmp_adapter *pAd,
    IN STATE_MACHINE *Sm,
    OUT STATE_MACHINE_FUNC Trans[]);

void APScanTimeout(void *FunctionContext);

void APInvalidStateWhenScan(
	IN struct rtmp_adapter *pAd,
	IN MLME_QUEUE_ELEM *Elem);

void APScanTimeoutAction(
	IN struct rtmp_adapter *pAd,
	IN MLME_QUEUE_ELEM *Elem);

void APPeerProbeReqAction(
    IN  struct rtmp_adapter *pAd,
    IN  MLME_QUEUE_ELEM *Elem);

void APPeerBeaconAction(
    IN struct rtmp_adapter *pAd,
    IN MLME_QUEUE_ELEM *Elem);

void APMlmeScanReqAction(
	IN struct rtmp_adapter *pAd,
	IN MLME_QUEUE_ELEM *Elem);

void APPeerBeaconAtScanAction(
	IN struct rtmp_adapter *pAd,
	IN MLME_QUEUE_ELEM *Elem);

void APScanCnclAction(
	IN struct rtmp_adapter *pAd,
	IN MLME_QUEUE_ELEM *Elem);

void ApSiteSurvey(
	IN	struct rtmp_adapter * 		pAd,
	IN	PNDIS_802_11_SSID	pSsid,
	IN	u8 			ScanType,
	IN	bool 			ChannelSel);

void SupportRate(
	IN u8 *SupRate,
	IN u8 SupRateLen,
	IN u8 *ExtRate,
	IN u8 ExtRateLen,
	OUT u8 **Rates,
	OUT u8 *RatesLen,
	OUT u8 *pMaxSupportRate);


bool ApScanRunning(
	IN struct rtmp_adapter *pAd);

void APOverlappingBSSScan(
	IN struct rtmp_adapter*pAd);

INT GetBssCoexEffectedChRange(
	IN struct rtmp_adapter*pAd,
	IN BSS_COEX_CH_RANGE *pCoexChRange);

/* ap_wpa.c */
void WpaStateMachineInit(
    IN  struct rtmp_adapter *  pAd,
    IN  STATE_MACHINE *Sm,
    OUT STATE_MACHINE_FUNC Trans[]);

/* ap_mlme.c */
void APMlmePeriodicExec(
    IN  struct rtmp_adapter *  pAd);

bool APMsgTypeSubst(
    IN struct rtmp_adapter *pAd,
    IN PFRAME_802_11 pFrame,
    OUT INT *Machine,
    OUT INT *MsgType);

void APQuickResponeForRateUpExec(void *FunctionContext);

void BeaconUpdateExec(void *FunctionContext);

void RTMPSetPiggyBack(
	IN struct rtmp_adapter *pAd,
	IN bool 		bPiggyBack);

void APAsicEvaluateRxAnt(
	IN struct rtmp_adapter *pAd);

void APAsicRxAntEvalTimeout(
	IN struct rtmp_adapter *pAd);

/* ap.c */
int APInitialize(
    IN  struct rtmp_adapter *  pAd);

void APShutdown(
    IN struct rtmp_adapter *   pAd);

void APStartUp(
    IN  struct rtmp_adapter *  pAd);

void APStop(
    IN  struct rtmp_adapter *  pAd);

void APCleanupPsQueue(
    IN  struct rtmp_adapter *  pAd,
    IN  PQUEUE_HEADER   pQueue);


void MacTableMaintenance(
    IN struct rtmp_adapter *pAd);

u32 MacTableAssocStaNumGet(
	IN struct rtmp_adapter *pAd);

MAC_TABLE_ENTRY *APSsPsInquiry(
    IN  struct rtmp_adapter *  pAd,
    IN  u8 *         pAddr,
    OUT SST             *Sst,
    OUT USHORT          *Aid,
    OUT u8           *PsMode,
    OUT u8           *Rate);

bool APPsIndicate(
    IN  struct rtmp_adapter *  pAd,
    IN  u8 *         pAddr,
	IN ULONG Wcid,
    IN  u8           Psm);

void APUpdateOperationMode(
    IN struct rtmp_adapter *pAd);

void APUpdateCapabilityAndErpIe(
	IN struct rtmp_adapter *pAd);

bool ApCheckAccessControlList(
	IN struct rtmp_adapter *pAd,
	IN u8 *       pAddr,
	IN u8         Apidx);

void ApUpdateAccessControlList(
    IN struct rtmp_adapter *pAd,
    IN u8         Apidx);

void ApEnqueueNullFrame(
	IN struct rtmp_adapter *pAd,
	IN u8 *       pAddr,
	IN u8         TxRate,
	IN u8         PID,
	IN u8         apidx,
    IN bool       bQosNull,
    IN bool       bEOSP,
    IN u8         OldUP);

/* ap_sanity.c */


bool PeerAssocReqCmmSanity(
    IN struct rtmp_adapter *pAd,
	IN bool isRessoc,
    IN void *Msg,
    IN INT MsgLen,
    IN IE_LISTS *ie_lists);


bool PeerDisassocReqSanity(
    IN struct rtmp_adapter *pAd,
    IN void *Msg,
    IN ULONG MsgLen,
    OUT u8 *pAddr2,
    OUT	UINT16	*SeqNum,
    OUT USHORT *Reason);

bool PeerDeauthReqSanity(
    IN struct rtmp_adapter *pAd,
    IN void *Msg,
    IN ULONG MsgLen,
    OUT u8 *pAddr2,
   	OUT	UINT16	*SeqNum,
    OUT USHORT *Reason);

bool APPeerAuthSanity(
    IN struct rtmp_adapter *pAd,
    IN void *Msg,
    IN ULONG MsgLen,
	OUT u8 *pAddr1,
    OUT u8 *pAddr2,
    OUT USHORT *Alg,
    OUT USHORT *Seq,
    OUT USHORT *Status,
    OUT CHAR *ChlgText
	);


#ifdef DOT1X_SUPPORT
/* ap_cfg.h */
INT	Set_OwnIPAddr_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_EAPIfName_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_PreAuthIfName_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

/* Define in ap.c */
bool DOT1X_InternalCmdAction(
    IN  struct rtmp_adapter *pAd,
    IN  MAC_TABLE_ENTRY *pEntry,
    IN	u8			cmd);

bool DOT1X_EapTriggerAction(
    IN  struct rtmp_adapter *pAd,
    IN  MAC_TABLE_ENTRY *pEntry);
#endif /* DOT1X_SUPPORT */
#endif  /* __AP_H__ */

void AP_E2PROM_IOCTL_PostCtrl(
	IN	RTMP_IOCTL_INPUT_STRUCT	*wrq,
	IN	char *				msg);

void IAPP_L2_UpdatePostCtrl(
	IN struct rtmp_adapter *pAd,
    IN u8 *mac_p,
    IN INT  bssid);
