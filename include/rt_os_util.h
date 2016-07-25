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


#ifndef __RT_OS_UTIL_H__
#define __RT_OS_UTIL_H__

/* ============================ rt_linux.c ================================== */
/* General */
void RtmpUtilInit(void);

/* OS Time */
void RTMPusecDelay(
	IN	ULONG					usec);

void RtmpOsUsDelay(ULONG value);

void RtmpOsMsDelay(
	IN	ULONG					msec);

void RTMP_GetCurrentSystemTime(
	IN	LARGE_INTEGER			*time);

ULONG RTMPMsecsToJiffies(UINT msec);

void RTMP_GetCurrentSystemTick(
	IN	ULONG					*pNow);

void RtmpOsWait(
	IN	UINT32					Time);

UINT32 RtmpOsTimerAfter(
	IN	ULONG					a,
	IN	ULONG					b);

UINT32 RtmpOsTimerBefore(
	IN	ULONG					a,
	IN	ULONG					b);

void RtmpOsGetSystemUpTime(
	IN	ULONG					*pTime);

UINT32 RtmpOsTickUnitGet(void);

/* OS Memory */
NDIS_STATUS os_alloc_mem(
	IN	void 				*pReserved,
	OUT	UCHAR					**mem,
	IN	ULONG					size);

NDIS_STATUS os_alloc_mem_suspend(
	IN	void 				*pReserved,
	OUT	UCHAR					**mem,
	IN	ULONG					size);

NDIS_STATUS os_free_mem(
	IN	void 				*pReserved,
	IN	void *				mem);

NDIS_STATUS AdapterBlockAllocateMemory(
	IN	void *				handle,
	OUT	void *				*ppAd,
	IN	UINT32					SizeOfpAd);

void *RtmpOsVmalloc(
	IN	ULONG					Size);

void RtmpOsVfree(
	IN	void 				*pMem);

ULONG RtmpOsCopyFromUser(
	OUT	void 				*to,
	IN	const void				*from,
	IN	ULONG					n);

ULONG RtmpOsCopyToUser(
	OUT void 				*to,
	IN	const void				*from,
	IN	ULONG					n);

BOOLEAN RtmpOsStatsAlloc(
	IN	void 				**ppStats,
	IN	void 				**ppIwStats);

/* OS Packet */
PNDIS_PACKET RtmpOSNetPktAlloc(
	IN	void 				*pReserved,
	IN	int						size);

PNDIS_PACKET RTMP_AllocateFragPacketBuffer(
	IN	void 				*pReserved,
	IN	ULONG					Length);

NDIS_STATUS RTMPAllocateNdisPacket(
	IN	void 				*pReserved,
	OUT PNDIS_PACKET			*ppPacket,
	IN	PUCHAR					pHeader,
	IN	UINT					HeaderLen,
	IN	PUCHAR					pData,
	IN	UINT					DataLen);

void RTMPFreeNdisPacket(
	IN	void 				*pReserved,
	IN	PNDIS_PACKET			pPacket);

NDIS_STATUS Sniff2BytesFromNdisBuffer(
	IN  PNDIS_BUFFER			pFirstBuffer,
	IN  UCHAR           		DesiredOffset,
	OUT PUCHAR          		pByte0,
	OUT PUCHAR          		pByte1);

void RTMP_QueryPacketInfo(
	IN  PNDIS_PACKET			pPacket,
	OUT PACKET_INFO  			*pPacketInfo,
	OUT PUCHAR		 			*pSrcBufVA,
	OUT	UINT		 			*pSrcBufLen);

PNDIS_PACKET DuplicatePacket(
	IN	PNET_DEV				pNetDev,
	IN	PNDIS_PACKET			pPacket,
	IN	UCHAR					FromWhichBSSID);

PNDIS_PACKET duplicate_pkt(
	IN	PNET_DEV				pNetDev,
	IN	PUCHAR					pHeader802_3,
    IN  UINT            		HdrLen,
	IN	PUCHAR					pData,
	IN	ULONG					DataSize,
	IN	UCHAR					FromWhichBSSID);

PNDIS_PACKET duplicate_pkt_with_TKIP_MIC(
	IN	void 				*pReserved,
	IN	PNDIS_PACKET			pOldPkt);

PNDIS_PACKET duplicate_pkt_with_VLAN(
	IN	PNET_DEV				pNetDev,
	IN	USHORT					VLAN_VID,
	IN	USHORT					VLAN_Priority,
	IN	PUCHAR					pHeader802_3,
    IN  UINT            		HdrLen,
	IN	PUCHAR					pData,
	IN	ULONG					DataSize,
	IN	UCHAR					FromWhichBSSID,
	IN	UCHAR					*TPID);

typedef void (*RTMP_CB_8023_PACKET_ANNOUNCE)(
			IN	void 		*pCtrlBkPtr,
			IN	PNDIS_PACKET	pPacket,
			IN	UCHAR			OpMode);

BOOLEAN RTMPL2FrameTxAction(
	IN  void 				*pCtrlBkPtr,
	IN	PNET_DEV				pNetDev,
	IN	RTMP_CB_8023_PACKET_ANNOUNCE _announce_802_3_packet,
	IN	UCHAR					apidx,
	IN	PUCHAR					pData,
	IN	UINT32					data_len,
	IN	UCHAR			OpMode);

PNDIS_PACKET ExpandPacket(
	IN	void 				*pReserved,
	IN	PNDIS_PACKET			pPacket,
	IN	UINT32					ext_head_len,
	IN	UINT32					ext_tail_len);

PNDIS_PACKET ClonePacket(
	IN	void 				*pReserved,
	IN	PNDIS_PACKET			pPacket,
	IN	PUCHAR					pData,
	IN	ULONG					DataSize);

void wlan_802_11_to_802_3_packet(
	IN	PNET_DEV				pNetDev,
	IN	UCHAR					OpMode,
	IN	USHORT					VLAN_VID,
	IN	USHORT					VLAN_Priority,
	IN	PNDIS_PACKET			pRxPacket,
	IN	UCHAR					*pData,
	IN	ULONG					DataSize,
	IN	PUCHAR					pHeader802_3,
	IN  UCHAR					FromWhichBSSID,
	IN	UCHAR					*TPID);

void send_monitor_packets(
	IN	PNET_DEV				pNetDev,
	IN	PNDIS_PACKET			pRxPacket,
	IN	PHEADER_802_11			pHeader,
	IN	UCHAR					*pData,
	IN	USHORT					DataSize,
	IN	UCHAR					L2PAD,
	IN	UCHAR					PHYMODE,
	IN	UCHAR					BW,
	IN	UCHAR					ShortGI,
	IN	UCHAR					MCS,
	IN	UCHAR					AMPDU,
	IN	UCHAR					STBC,
	IN	UCHAR					RSSI1,
	IN	UCHAR					BssMonitorFlag11n,
	IN	UCHAR					*pDevName,
	IN	UCHAR					Channel,
	IN	UCHAR					CentralChannel,
	IN	UINT32					MaxRssi);

UCHAR VLAN_8023_Header_Copy(
	IN	USHORT					VLAN_VID,
	IN	USHORT					VLAN_Priority,
	IN	PUCHAR					pHeader802_3,
	IN	UINT            		HdrLen,
	OUT PUCHAR					pData,
	IN	UCHAR					FromWhichBSSID,
	IN	UCHAR					*TPID);

void RtmpOsPktBodyCopy(
	IN	PNET_DEV				pNetDev,
	IN	PNDIS_PACKET			pNetPkt,
	IN	ULONG					ThisFrameLen,
	IN	PUCHAR					pData);

INT RtmpOsIsPktCloned(
	IN	PNDIS_PACKET			pNetPkt);

PNDIS_PACKET RtmpOsPktCopy(
	IN	PNDIS_PACKET			pNetPkt);

PNDIS_PACKET RtmpOsPktClone(
	IN	PNDIS_PACKET			pNetPkt);

void RtmpOsPktDataPtrAssign(
	IN	PNDIS_PACKET			pNetPkt,
	IN	UCHAR					*pData);

void RtmpOsPktLenAssign(
	IN	PNDIS_PACKET			pNetPkt,
	IN	LONG					Len);

void RtmpOsPktTailAdjust(
	IN	PNDIS_PACKET			pNetPkt,
	IN	UINT					removedTagLen);

PUCHAR RtmpOsPktTailBufExtend(
	IN	PNDIS_PACKET			pNetPkt,
	IN	UINT					Len);

PUCHAR RtmpOsPktHeadBufExtend(
	IN	PNDIS_PACKET			pNetPkt,
	IN	UINT					Len);

void RtmpOsPktReserve(
	IN	PNDIS_PACKET			pNetPkt,
	IN	UINT					Len);

void RtmpOsPktProtocolAssign(
	IN	PNDIS_PACKET			pNetPkt);

void RtmpOsPktInfPpaSend(
	IN	PNDIS_PACKET			pNetPkt);

void RtmpOsPktRcvHandle(
	IN	PNDIS_PACKET			pNetPkt);

void RtmpOsPktNatMagicTag(
	IN	PNDIS_PACKET			pNetPkt);

void RtmpOsPktNatNone(
	IN	PNDIS_PACKET			pNetPkt);

void RtmpOsPktInit(
	IN	PNDIS_PACKET			pNetPkt,
	IN	PNET_DEV				pNetDev,
	IN	UCHAR					*pData,
	IN	USHORT					DataSize);

PNDIS_PACKET RtmpOsPktIappMakeUp(
	IN	PNET_DEV				pNetDev,
	IN	u8					*pMac);

BOOLEAN RtmpOsPktOffsetInit(void);

/*
========================================================================
Routine Description:
	Initialize the OS atomic_t.
*/

UINT16 RtmpOsNtohs(
	IN	UINT16					Value);

UINT16 RtmpOsHtons(
	IN	UINT16					Value);

UINT32 RtmpOsNtohl(
	IN	UINT32					Value);

UINT32 RtmpOsHtonl(
	IN	UINT32					Value);

/* OS File */
RTMP_OS_FD RtmpOSFileOpen(char *pPath,  int flag, int mode);
int RtmpOSFileClose(RTMP_OS_FD osfd);
void RtmpOSFileSeek(RTMP_OS_FD osfd, int offset);
int RtmpOSFileRead(RTMP_OS_FD osfd, char *pDataPtr, int readLen);
int RtmpOSFileWrite(RTMP_OS_FD osfd, char *pDataPtr, int writeLen);

INT32 RtmpOsFileIsErr(
	IN	void 				*pFile);

void RtmpOSFSInfoChange(
	IN	RTMP_OS_FS_INFO			*pOSFSInfoOrg,
	IN	BOOLEAN					bSet);

/* OS Network Interface */
int RtmpOSNetDevAddrSet(
	IN UCHAR					OpMode,
	IN PNET_DEV 				pNetDev,
	IN PUCHAR					pMacAddr,
	IN PUCHAR					dev_name);

void RtmpOSNetDevClose(
	IN PNET_DEV					pNetDev);

void RtmpOSNetDevFree(
	IN	PNET_DEV				pNetDev);

INT RtmpOSNetDevAlloc(
	IN	PNET_DEV				*new_dev_p,
	IN	UINT32					privDataSize);

INT RtmpOSNetDevOpsAlloc(
	IN	void *				*pNetDevOps);


PNET_DEV RtmpOSNetDevGetByName(
	IN	PNET_DEV				pNetDev,
	IN	char *				pDevName);

void RtmpOSNetDeviceRefPut(
	IN	PNET_DEV				pNetDev);

INT RtmpOSNetDevDestory(
	IN	void 				*pReserved,
	IN	PNET_DEV				pNetDev);

void RtmpOSNetDevDetach(
	IN	PNET_DEV				pNetDev);

int RtmpOSNetDevAttach(
	IN	UCHAR					OpMode,
	IN	PNET_DEV				pNetDev,
	IN	RTMP_OS_NETDEV_OP_HOOK	*pDevOpHook);

void RtmpOSNetDevProtect(
	IN BOOLEAN lock_it);

PNET_DEV RtmpOSNetDevCreate(
	IN	INT32					MC_RowID,
	IN	UINT32					*pIoctlIF,
	IN	INT 					devType,
	IN	INT						devNum,
	IN	INT						privMemSize,
	IN	char *				pNamePrefix);

BOOLEAN RtmpOSNetDevIsUp(
	IN	void 				*pDev);

unsigned char *RtmpOsNetDevGetPhyAddr(
	IN	void 				*pDev);

void RtmpOsNetQueueStart(
	IN	PNET_DEV				pDev);

void RtmpOsNetQueueStop(
	IN	PNET_DEV				pDev);

void RtmpOsNetQueueWake(
	IN	PNET_DEV				pDev);

void RtmpOsSetPktNetDev(
	IN	void 				*pPkt,
	IN	void 				*pDev);

PNET_DEV RtmpOsPktNetDevGet(
	IN	void 				*pPkt);

char *RtmpOsGetNetDevName(
	IN	void 				*pDev);

void RtmpOsSetNetDevPriv(
	IN	void 				*pDev,
	IN	void 				*pPriv);

void *RtmpOsGetNetDevPriv(
	IN	void 				*pDev);

USHORT RtmpDevPrivFlagsGet(
	IN	void 				*pDev);

void RtmpDevPrivFlagsSet(
	IN	void 				*pDev,
	IN	USHORT					PrivFlags);

void RtmpOsSetNetDevType(void *pDev, USHORT Type);

void RtmpOsSetNetDevTypeMonitor(void *pDev);


/* OS Semaphore */
void RtmpOsCmdUp(RTMP_OS_TASK *pCmdQTask);
BOOLEAN RtmpOsSemaInitLocked(RTMP_OS_SEM *pSemOrg, LIST_HEADER *pSemList);
BOOLEAN RtmpOsSemaInit(RTMP_OS_SEM *pSemOrg, LIST_HEADER *pSemList);
BOOLEAN RtmpOsSemaDestroy(RTMP_OS_SEM *pSemOrg);
INT RtmpOsSemaWaitInterruptible(RTMP_OS_SEM *pSemOrg);
void RtmpOsSemaWakeUp(RTMP_OS_SEM *pSemOrg);
void RtmpOsMlmeUp(RTMP_OS_TASK *pMlmeQTask);

void RtmpOsInitCompletion(RTMP_OS_COMPLETION *pCompletion);
void RtmpOsExitCompletion(RTMP_OS_COMPLETION *pCompletion);
void RtmpOsComplete(RTMP_OS_COMPLETION *pCompletion);
ULONG RtmpOsWaitForCompletionTimeout(RTMP_OS_COMPLETION *pCompletion, ULONG Timeout);

/* OS Task */
BOOLEAN RtmpOsTaskletSche(RTMP_NET_TASK_STRUCT *pTasklet);

BOOLEAN RtmpOsTaskletInit(
	RTMP_NET_TASK_STRUCT *pTasklet,
	void (*pFunc)(unsigned long data),
	ULONG Data,
	LIST_HEADER *pTaskletList);

BOOLEAN RtmpOsTaskletKill(RTMP_NET_TASK_STRUCT *pTasklet);

void RtmpOsTaskletDataAssign(
	RTMP_NET_TASK_STRUCT *pTasklet,
	ULONG Data);

void RtmpOsTaskWakeUp(RTMP_OS_TASK *pTaskOrg);

INT32 RtmpOsTaskIsKilled(RTMP_OS_TASK *pTaskOrg);

BOOLEAN RtmpOsCheckTaskLegality(RTMP_OS_TASK *pTaskOrg);

BOOLEAN RtmpOSTaskAlloc(
	IN	RTMP_OS_TASK			*pTask,
	IN	LIST_HEADER				*pTaskList);

void RtmpOSTaskFree(
	IN	RTMP_OS_TASK			*pTask);

NDIS_STATUS RtmpOSTaskKill(
	IN	RTMP_OS_TASK			*pTaskOrg);

INT RtmpOSTaskNotifyToExit(
	IN	RTMP_OS_TASK			*pTaskOrg);

void RtmpOSTaskCustomize(
	IN	RTMP_OS_TASK			*pTaskOrg);

NDIS_STATUS RtmpOSTaskAttach(
	IN	RTMP_OS_TASK			*pTaskOrg,
	IN	RTMP_OS_TASK_CALLBACK	fn,
	IN	ULONG					arg);

NDIS_STATUS RtmpOSTaskInit(
	IN	RTMP_OS_TASK			*pTaskOrg,
	IN	char *				pTaskName,
	IN	void 				*pPriv,
	IN	LIST_HEADER				*pTaskList,
	IN	LIST_HEADER				*pSemList);

BOOLEAN RtmpOSTaskWait(
	IN	void 				*pReserved,
	IN	RTMP_OS_TASK			*pTaskOrg,
	IN	INT32					*pStatus);

void *RtmpOsTaskDataGet(RTMP_OS_TASK *pTaskOrg);

INT32 RtmpThreadPidKill(RTMP_OS_PID	 PID);

/* OS Cache */
void RtmpOsDCacheFlush(ULONG AddrStart, ULONG Size);

/* OS Timer */
void RTMP_SetPeriodicTimer(
	IN	NDIS_MINIPORT_TIMER *pTimerOrg,
	IN	unsigned long timeout);

void RTMP_OS_Init_Timer(
	IN	void 					*pReserved,
	IN	NDIS_MINIPORT_TIMER		*pTimerOrg,
	IN	TIMER_FUNCTION			function,
	IN	void *				data,
	IN	LIST_HEADER				*pTimerList);

void RTMP_OS_Add_Timer(
	IN	NDIS_MINIPORT_TIMER *pTimerOrg,
	IN	unsigned long timeout);

void RTMP_OS_Mod_Timer(
	IN	NDIS_MINIPORT_TIMER *pTimerOrg,
	IN	unsigned long timeout);

void RTMP_OS_Del_Timer(
	IN	NDIS_MINIPORT_TIMER		*pTimerOrg,
	OUT	BOOLEAN					*pCancelled);

void RTMP_OS_Release_Timer(
	IN	NDIS_MINIPORT_TIMER		*pTimerOrg);

BOOLEAN RTMP_OS_Alloc_Rsc(
	IN	LIST_HEADER				*pRscList,
	IN	void 					*pRsc,
	IN	UINT32					RscLen);

void RTMP_OS_Free_Rscs(
	IN	LIST_HEADER				*pRscList);

/* OS Lock */
BOOLEAN RtmpOsAllocateLock(
	IN	NDIS_SPIN_LOCK			*pLock,
	IN	LIST_HEADER				*pLockList);

void RtmpOsFreeSpinLock(
	IN	NDIS_SPIN_LOCK			*pLockOrg);

void RtmpOsSpinLockBh(
	IN	NDIS_SPIN_LOCK			*pLockOrg);

void RtmpOsSpinUnLockBh(NDIS_SPIN_LOCK *pLockOrg);
void RtmpOsIntLock(NDIS_SPIN_LOCK *pLockOrg, ULONG *pIrqFlags);
void RtmpOsIntUnLock(NDIS_SPIN_LOCK *pLockOrg, ULONG IrqFlags);

/* OS PID */
void RtmpOsGetPid(ULONG *pDst, ULONG PID);
void RtmpOsTaskPidInit(RTMP_OS_PID *pPid);

/* OS I/O */
void RTMP_PCI_Writel(ULONG Value, void *pAddr);
void RTMP_PCI_Writew(ULONG Value, void *pAddr);
void RTMP_PCI_Writeb(ULONG Value, void *pAddr);
ULONG RTMP_PCI_Readl(void *pAddr);
ULONG RTMP_PCI_Readw(void *pAddr);
ULONG RTMP_PCI_Readb(void *pAddr);

int RtmpOsPciConfigReadWord(
	IN	void 				*pDev,
	IN	UINT32					Offset,
	OUT UINT16					*pValue);

int RtmpOsPciConfigWriteWord(
	IN	void 				*pDev,
	IN	UINT32					Offset,
	IN	UINT16					Value);

int RtmpOsPciConfigReadDWord(
	IN	void 				*pDev,
	IN	UINT32					Offset,
	OUT UINT32					*pValue);

int RtmpOsPciConfigWriteDWord(
	IN	void 				*pDev,
	IN	UINT32					Offset,
	IN	UINT32					Value);

int RtmpOsPciFindCapability(
	IN	void 				*pDev,
	IN	int						Cap);

void *RTMPFindHostPCIDev(void *pPciDevSrc);

int RtmpOsPciMsiEnable(void *pDev);
void RtmpOsPciMsiDisable(void *pDev);

/* OS Wireless */
ULONG RtmpOsMaxScanDataGet(void);

/* OS Interrutp */
INT32 RtmpOsIsInInterrupt(void);

/* OS USB */
void *RtmpOsUsbUrbDataGet(void *pUrb);
NTSTATUS RtmpOsUsbUrbStatusGet(void *pUrb);
ULONG RtmpOsUsbUrbLenGet(void *pUrb);

/* OS Atomic */
BOOLEAN RtmpOsAtomicInit(RTMP_OS_ATOMIC *pAtomic, LIST_HEADER *pAtomicList);
void RtmpOsAtomicDestroy(RTMP_OS_ATOMIC *pAtomic);
LONG RtmpOsAtomicRead(RTMP_OS_ATOMIC *pAtomic);
void RtmpOsAtomicDec(RTMP_OS_ATOMIC *pAtomic);
void RtmpOsAtomicInterlockedExchange(RTMP_OS_ATOMIC *pAtomicSrc, LONG Value);

/* OS Utility */
void hex_dump(char *str, unsigned char *pSrcBufVA, unsigned int SrcBufLen);

typedef void (*RTMP_OS_SEND_WLAN_EVENT)(
	IN	void 				*pAdSrc,
	IN	USHORT					Event_flag,
	IN	PUCHAR 					pAddr,
	IN  UCHAR					BssIdx,
	IN	CHAR					Rssi);

void RtmpOsSendWirelessEvent(
	IN	void 		*pAd,
	IN	USHORT			Event_flag,
	IN	PUCHAR 			pAddr,
	IN	UCHAR			BssIdx,
	IN	CHAR			Rssi,
	IN	RTMP_OS_SEND_WLAN_EVENT pFunc);


int RtmpOSWrielessEventSend(
	IN	PNET_DEV				pNetDev,
	IN	UINT32					eventType,
	IN	INT						flags,
	IN	PUCHAR					pSrcMac,
	IN	PUCHAR					pData,
	IN	UINT32					dataLen);

int RtmpOSWrielessEventSendExt(
	IN	PNET_DEV				pNetDev,
	IN	UINT32					eventType,
	IN	INT						flags,
	IN	PUCHAR					pSrcMac,
	IN	PUCHAR					pData,
	IN	UINT32					dataLen,
	IN	UINT32					family);

UINT RtmpOsWirelessExtVerGet(void);

void RtmpDrvAllMacPrint(
	IN void 					*pReserved,
	IN UINT32					*pBufMac,
	IN UINT32					AddrStart,
	IN UINT32					AddrEnd,
	IN UINT32					AddrStep);

void RtmpDrvAllE2PPrint(
	IN	void 				*pReserved,
	IN	USHORT					*pMacContent,
	IN	UINT32					AddrEnd,
	IN	UINT32					AddrStep);

void RtmpDrvAllRFPrint(
	IN void *pReserved,
	IN UINT32 *pBuf,
	IN UINT32 BufLen);

int RtmpOSIRQRelease(
	IN	PNET_DEV				pNetDev,
	IN	UINT32					infType,
	IN	PPCI_DEV				pci_dev,
	IN	BOOLEAN					*pHaveMsi);

void RtmpOsWlanEventSet(
	IN	void 				*pReserved,
	IN	BOOLEAN					*pCfgWEnt,
	IN	BOOLEAN					FlgIsWEntSup);

UINT16 RtmpOsGetUnaligned(UINT16 *pWord);

UINT32 RtmpOsGetUnaligned32(UINT32 *pWord);

ULONG RtmpOsGetUnalignedlong(ULONG *pWord);

long RtmpOsSimpleStrtol(
	IN	const char				*cp,
	IN	char 					**endp,
	IN	unsigned int			base);

void RtmpOsOpsInit(RTMP_OS_ABL_OPS *pOps);

/* ============================ rt_os_util.c ================================ */
void RtmpDrvMaxRateGet(
	IN void *pReserved,
	IN u8 MODE,
	IN u8 ShortGI,
	IN u8 BW,
	IN u8 MCS,
	OUT UINT32 *pRate);

char * rtstrchr(const char * s, int c);

char *  WscGetAuthTypeStr(USHORT authFlag);

char *  WscGetEncryTypeStr(USHORT encryFlag);

USHORT WscGetAuthTypeFromStr(char *arg);

USHORT WscGetEncrypTypeFromStr(char *arg);

void RtmpMeshDown(
	IN void *pDrvCtrlBK,
	IN BOOLEAN WaitFlag,
	IN BOOLEAN (*RtmpMeshLinkCheck)(IN void *pAd));

USHORT RtmpOsNetPrivGet(PNET_DEV pDev);

BOOLEAN RtmpOsCmdDisplayLenCheck(
	IN	UINT32					LenSrc,
	IN	UINT32					Offset);

void    WpaSendMicFailureToWpaSupplicant(
	IN	PNET_DEV				pNetDev,
    IN  BOOLEAN					bUnicast);

int wext_notify_event_assoc(
	IN	PNET_DEV				pNetDev,
	IN	UCHAR					*ReqVarIEs,
	IN	UINT32					ReqVarIELen);

void    SendAssocIEsToWpaSupplicant(
	IN	PNET_DEV				pNetDev,
	IN	UCHAR					*ReqVarIEs,
	IN	UINT32					ReqVarIELen);

/* ============================ rt_rbus_pci_util.c ========================== */
void RtmpAllocDescBuf(
	IN PPCI_DEV pPciDev,
	IN UINT Index,
	IN ULONG Length,
	IN BOOLEAN Cached,
	OUT void **VirtualAddress,
	OUT PNDIS_PHYSICAL_ADDRESS	PhysicalAddress);

void RtmpFreeDescBuf(
	IN PPCI_DEV pPciDev,
	IN ULONG Length,
	IN void *VirtualAddress,
	IN NDIS_PHYSICAL_ADDRESS	PhysicalAddress);

void RTMP_AllocateFirstTxBuffer(
	IN PPCI_DEV pPciDev,
	IN UINT Index,
	IN ULONG Length,
	IN BOOLEAN Cached,
	OUT void **VirtualAddress,
	OUT PNDIS_PHYSICAL_ADDRESS	PhysicalAddress);

void RTMP_FreeFirstTxBuffer(
	IN	PPCI_DEV				pPciDev,
	IN	ULONG					Length,
	IN	BOOLEAN					Cached,
	IN	void *				VirtualAddress,
	IN	NDIS_PHYSICAL_ADDRESS	PhysicalAddress);

PNDIS_PACKET RTMP_AllocateRxPacketBuffer(
	IN	void 				*pReserved,
	IN	void 				*pPciDev,
	IN	ULONG					Length,
	IN	BOOLEAN					Cached,
	OUT	void *				*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS	PhysicalAddress);

#ifdef CONFIG_STA_SUPPORT
#ifdef CONFIG_PM
#ifdef USB_SUPPORT_SELECTIVE_SUSPEND

int RTMP_Usb_AutoPM_Put_Interface(
	IN	void 		*pUsb_Dev,
	IN	void 		*intf);

int  RTMP_Usb_AutoPM_Get_Interface(
	IN	void 		*pUsb_Dev,
	IN	void 		*intf);

#endif /* USB_SUPPORT_SELECTIVE_SUSPEND */
#endif /* CONFIG_PM */
#endif /* CONFIG_STA_SUPPORT */



ra_dma_addr_t linux_pci_map_single(void *pPciDev, void *ptr, size_t size, int sd_idx, int direction);

void linux_pci_unmap_single(void *pPciDev, ra_dma_addr_t dma_addr, size_t size, int direction);

/* ============================ rt_usb_util.c =============================== */
#ifdef RTMP_MAC_USB
typedef void (*USB_COMPLETE_HANDLER)(void *);

void dump_urb(void *purb);

int rausb_register(void * new_driver);

void rausb_deregister(void * driver);

/*struct urb *rausb_alloc_urb(int iso_packets); */

void rausb_free_urb(void *urb);

void rausb_put_dev(void *dev);

struct usb_device *rausb_get_dev(void *dev);

int rausb_submit_urb(void *urb);

void *rausb_buffer_alloc(void *dev,
							size_t size,
							ra_dma_addr_t *dma);

void rausb_buffer_free(void *dev,
							size_t size,
							void *addr,
							ra_dma_addr_t dma);

int rausb_control_msg(void *dev,
						unsigned int pipe,
						__u8 request,
						__u8 requesttype,
						__u16 value,
						__u16 index,
						void *data,
						__u16 size,
						int timeout);

void rausb_fill_bulk_urb(void *urb,
						 void *dev,
						 unsigned int pipe,
						 void *transfer_buffer,
						 int buffer_length,
						 USB_COMPLETE_HANDLER complete_fn,
						 void *context);

unsigned int rausb_sndctrlpipe(void *dev, ULONG address);

unsigned int rausb_rcvctrlpipe(void *dev, ULONG address);


unsigned int rausb_sndbulkpipe(void *dev, ULONG address);
unsigned int rausb_rcvbulkpipe(void *dev, ULONG address);

void rausb_kill_urb(void *urb);

void RtmpOsUsbEmptyUrbCheck(
	IN	void 			**ppWait,
	IN	NDIS_SPIN_LOCK		*pBulkInLock,
	IN	UCHAR				*pPendingRx);


void RtmpOsUsbInitHTTxDesc(
	IN	void 		*pUrbSrc,
	IN	void 		*pUsb_Dev,
	IN	UINT			BulkOutEpAddr,
	IN	PUCHAR			pSrc,
	IN	ULONG			BulkOutSize,
	IN	USB_COMPLETE_HANDLER	Func,
	IN	void 		*pTxContext,
	IN	ra_dma_addr_t		TransferDma);

void RtmpOsUsbInitRxDesc(
	IN	void 		*pUrbSrc,
	IN	void 		*pUsb_Dev,
	IN	UINT			BulkInEpAddr,
	IN	UCHAR			*pTransferBuffer,
	IN	UINT32			BufSize,
	IN	USB_COMPLETE_HANDLER	Func,
	IN	void 		*pRxContext,
	IN	ra_dma_addr_t		TransferDma);

void *RtmpOsUsbContextGet(
	IN	void 		*pUrb);

NTSTATUS RtmpOsUsbStatusGet(
	IN	void 		*pUrb);

void RtmpOsUsbDmaMapping(
	IN	void 		*pUrb);
#endif /* RTMP_MAC_USB */

#if defined(RTMP_RBUS_SUPPORT) || defined(RTMP_FLASH_SUPPORT)
void RtmpFlashRead(
	UCHAR * p,
	ULONG a,
	ULONG b);

void RtmpFlashWrite(
	UCHAR * p,
	ULONG a,
	ULONG b);
#endif /* defined(RTMP_RBUS_SUPPORT) || defined(RTMP_FLASH_SUPPORT) */

UINT32 RtmpOsGetUsbDevVendorID(
	IN void *pUsbDev);

UINT32 RtmpOsGetUsbDevProductID(
	IN void *pUsbDev);

/* CFG80211 */
#ifdef RT_CFG80211_SUPPORT
typedef struct __CFG80211_BAND {

	u8 RFICType;
	u8 MpduDensity;
	u8 TxStream;
	u8 RxStream;
	UINT32 MaxTxPwr;
	UINT32 MaxBssTable;

	UINT16 RtsThreshold;
	UINT16 FragmentThreshold;
	UINT32 RetryMaxCnt; /* bit0~7: short; bit8 ~ 15: long */
	BOOLEAN FlgIsBMode;
} CFG80211_BAND;

void CFG80211OS_UnRegister(
	IN void 					*pCB,
	IN void 					*pNetDev);

BOOLEAN CFG80211_SupBandInit(
	IN void 					*pCB,
	IN CFG80211_BAND 			*pBandInfo,
	IN void 					*pWiphyOrg,
	IN void 					*pChannelsOrg,
	IN void 					*pRatesOrg);

BOOLEAN CFG80211OS_SupBandReInit(
	IN void 					*pCB,
	IN CFG80211_BAND 			*pBandInfo);

void CFG80211OS_RegHint(
	IN void 					*pCB,
	IN UCHAR					*pCountryIe,
	IN ULONG					CountryIeLen);

void CFG80211OS_RegHint11D(
	IN void 					*pCB,
	IN UCHAR					*pCountryIe,
	IN ULONG					CountryIeLen);

BOOLEAN CFG80211OS_BandInfoGet(
	IN void 					*pCB,
	IN void 					*pWiphyOrg,
	OUT void 				**ppBand24,
	OUT void 				**ppBand5);

UINT32 CFG80211OS_ChanNumGet(
	IN void 					*pCB,
	IN void 					*pWiphyOrg,
	IN UINT32					IdBand);

BOOLEAN CFG80211OS_ChanInfoGet(
	IN void 					*pCB,
	IN void 					*pWiphyOrg,
	IN UINT32					IdBand,
	IN UINT32					IdChan,
	OUT UINT32					*pChanId,
	OUT UINT32					*pPower,
	OUT BOOLEAN					*pFlgIsRadar);

BOOLEAN CFG80211OS_ChanInfoInit(
	IN void 					*pCB,
	IN UINT32					InfoIndex,
	IN UCHAR					ChanId,
	IN UCHAR					MaxTxPwr,
	IN BOOLEAN					FlgIsNMode,
	IN BOOLEAN					FlgIsBW20M);

void CFG80211OS_Scaning(
	IN void 					*pCB,
	IN UINT32					ChanId,
	IN UCHAR					*pFrame,
	IN UINT32					FrameLen,
	IN INT32					RSSI,
	IN BOOLEAN					FlgIsNMode,
	IN u8					BW);

void CFG80211OS_ScanEnd(
	IN void 					*pCB,
	IN BOOLEAN					FlgIsAborted);

void CFG80211OS_ConnectResultInform(
	IN void 					*pCB,
	IN UCHAR					*pBSSID,
	IN UCHAR					*pReqIe,
	IN UINT32					ReqIeLen,
	IN UCHAR					*pRspIe,
	IN UINT32					RspIeLen,
	IN UCHAR					FlgIsSuccess);
#endif /* RT_CFG80211_SUPPORT */




/* ================================ MACRO =================================== */
#define RTMP_UTIL_DCACHE_FLUSH(__AddrStart, __Size)

/* ================================ EXTERN ================================== */
extern UCHAR SNAP_802_1H[6];
extern UCHAR SNAP_BRIDGE_TUNNEL[6];
extern UCHAR EAPOL[2];
extern UCHAR TPID[];
extern UCHAR IPX[2];
extern UCHAR APPLE_TALK[2];
extern UCHAR NUM_BIT8[8];
extern ULONG RTPktOffsetData, RTPktOffsetLen, RTPktOffsetCB;

extern ULONG OS_NumOfMemAlloc, OS_NumOfMemFree;

extern INT32 ralinkrate[];
extern UINT32 RT_RateSize;

#ifdef PLATFORM_UBM_IPX8
#include "vrut_ubm.h"
#endif /* PLATFORM_UBM_IPX8 */

INT32  RtPrivIoctlSetVal(void);

void OS_SPIN_LOCK_IRQSAVE(NDIS_SPIN_LOCK *lock, unsigned long *flags);
void OS_SPIN_UNLOCK_IRQRESTORE(NDIS_SPIN_LOCK *lock, unsigned long *flags);
void OS_SPIN_LOCK_IRQ(NDIS_SPIN_LOCK *lock);
void OS_SPIN_UNLOCK_IRQ(NDIS_SPIN_LOCK *lock);
void RtmpOsSpinLockIrqSave(NDIS_SPIN_LOCK *lock, unsigned long *flags);
void RtmpOsSpinUnlockIrqRestore(NDIS_SPIN_LOCK *lock, unsigned long *flags);
void RtmpOsSpinLockIrq(NDIS_SPIN_LOCK *lock);
void RtmpOsSpinUnlockIrq(NDIS_SPIN_LOCK *lock);
int OS_TEST_BIT(int bit, unsigned long *flags);
void OS_SET_BIT(int bit, unsigned long *flags);
void OS_CLEAR_BIT(int bit, unsigned long *flags);

#endif /* __RT_OS_UTIL_H__ */
