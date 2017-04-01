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

ULONG RTMPMsecsToJiffies(UINT msec);

void RtmpOsWait(
	IN	u32					Time);

u32 RtmpOsTimerAfter(
	IN	ULONG					a,
	IN	ULONG					b);

u32 RtmpOsTimerBefore(
	IN	ULONG					a,
	IN	ULONG					b);

void RtmpOsGetSystemUpTime(
	IN	ULONG					*pTime);

u32 RtmpOsTickUnitGet(void);

/* OS Memory */

int os_alloc_mem_suspend(
	IN	void 				*pReserved,
	OUT	u8 				**mem,
	IN	ULONG					size);

int AdapterBlockAllocateMemory(struct rtmp_adapter **ppAd, u32 SizeOfpAd);

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

bool RtmpOsStatsAlloc(
	IN	void 				**ppStats,
	IN	void 				**ppIwStats);

/* OS Packet */

struct sk_buff *RTMPAllocateNdisPacket(u8 *pHeader, UINT HeaderLen,
				       u8 *pData, UINT DataLen);

int Sniff2BytesFromNdisBuffer(
	IN  PNDIS_BUFFER			pFirstBuffer,
	IN  u8           		DesiredOffset,
	OUT u8 *         		pByte0,
	OUT u8 *         		pByte1);

void RTMP_QueryPacketInfo(
	IN  struct sk_buff *			pPacket,
	OUT PACKET_INFO  			*pPacketInfo,
	OUT u8 *	 			*pSrcBufVA,
	OUT	UINT		 			*pSrcBufLen);

struct sk_buff * DuplicatePacket(
	IN	struct net_device *			pNetDev,
	IN	struct sk_buff *			pPacket,
	IN	u8 				FromWhichBSSID);

struct sk_buff * duplicate_pkt(
	IN	struct net_device *			pNetDev,
	IN	u8 *				pHeader802_3,
    IN  UINT            		HdrLen,
	IN	u8 *				pData,
	IN	ULONG					DataSize,
	IN	u8 				FromWhichBSSID);

struct sk_buff * duplicate_pkt_with_TKIP_MIC(
	IN	void 				*pReserved,
	IN	struct sk_buff *			pOldPkt);

struct sk_buff * duplicate_pkt_with_VLAN(
	IN	struct net_device *			pNetDev,
	IN	USHORT					VLAN_VID,
	IN	USHORT					VLAN_Priority,
	IN	u8 *				pHeader802_3,
    IN  UINT            		HdrLen,
	IN	u8 *				pData,
	IN	ULONG					DataSize,
	IN	u8 				FromWhichBSSID,
	IN	u8 				*TPID);

typedef void (*RTMP_CB_8023_PACKET_ANNOUNCE)(
			struct rtmp_adapter *pAd,
			struct sk_buff *skb,
			u8 OpMode);

bool RTMPL2FrameTxAction(
	IN  void 				*pCtrlBkPtr,
	IN	struct net_device *			pNetDev,
	IN	RTMP_CB_8023_PACKET_ANNOUNCE _announce_802_3_packet,
	IN	u8 				apidx,
	IN	u8 *				pData,
	IN	u32					data_len,
	IN	u8 		OpMode);

struct sk_buff * ExpandPacket(
	IN	void 				*pReserved,
	IN	struct sk_buff *			pPacket,
	IN	u32					ext_head_len,
	IN	u32					ext_tail_len);

struct sk_buff * ClonePacket(
	IN	void 				*pReserved,
	IN	struct sk_buff *			pPacket,
	IN	u8 *				pData,
	IN	ULONG					DataSize);

void wlan_802_11_to_802_3_packet(
	IN	struct net_device *			pNetDev,
	IN	u8 				OpMode,
	IN	USHORT					VLAN_VID,
	IN	USHORT					VLAN_Priority,
	IN	struct sk_buff *			skb,
	IN	u8 				*pData,
	IN	ULONG					DataSize,
	IN	u8 *				pHeader802_3,
	IN  u8 				FromWhichBSSID,
	IN	u8 				*TPID);

void send_monitor_packets(
	IN	struct net_device *			pNetDev,
	IN	struct sk_buff *			skb,
	IN	PHEADER_802_11			pHeader,
	IN	u8 				*pData,
	IN	USHORT					DataSize,
	IN	u8 				L2PAD,
	IN	u8 				PHYMODE,
	IN	u8 				BW,
	IN	u8 				ShortGI,
	IN	u8 				MCS,
	IN	u8 				AMPDU,
	IN	u8 				STBC,
	IN	u8 				RSSI1,
	IN	u8 				BssMonitorFlag11n,
	IN	u8 				*pDevName,
	IN	u8 				Channel,
	IN	u8 				CentralChannel,
	IN	u32					MaxRssi);

u8 VLAN_8023_Header_Copy(
	IN	USHORT					VLAN_VID,
	IN	USHORT					VLAN_Priority,
	IN	u8 *				pHeader802_3,
	IN	UINT            		HdrLen,
	OUT u8 *				pData,
	IN	u8 				FromWhichBSSID,
	IN	u8 				*TPID);

void RtmpOsPktBodyCopy(
	IN	struct net_device *			pNetDev,
	IN	struct sk_buff *			pNetPkt,
	IN	ULONG					ThisFrameLen,
	IN	u8 *				pData);

INT RtmpOsIsPktCloned(
	IN	struct sk_buff *			pNetPkt);

struct sk_buff * RtmpOsPktCopy(
	IN	struct sk_buff *			pNetPkt);

struct sk_buff * RtmpOsPktClone(
	IN	struct sk_buff *			pNetPkt);

void RtmpOsPktDataPtrAssign(
	IN	struct sk_buff *			pNetPkt,
	IN	u8 				*pData);

void RtmpOsPktLenAssign(
	IN	struct sk_buff *			pNetPkt,
	IN	LONG					Len);

void RtmpOsPktTailAdjust(
	IN	struct sk_buff *			pNetPkt,
	IN	UINT					removedTagLen);

u8 *RtmpOsPktTailBufExtend(
	IN	struct sk_buff *			pNetPkt,
	IN	UINT					Len);

u8 *RtmpOsPktHeadBufExtend(
	IN	struct sk_buff *			pNetPkt,
	IN	UINT					Len);

void RtmpOsPktReserve(
	IN	struct sk_buff *			pNetPkt,
	IN	UINT					Len);

void RtmpOsPktInfPpaSend(
	IN	struct sk_buff *			pNetPkt);

void RtmpOsPktInit(
	IN	struct sk_buff *			pNetPkt,
	IN	struct net_device *			pNetDev,
	IN	u8 				*pData,
	IN	USHORT					DataSize);

struct sk_buff * RtmpOsPktIappMakeUp(
	IN	struct net_device *			pNetDev,
	IN	u8					*pMac);

bool RtmpOsPktOffsetInit(void);

/*
========================================================================
Routine Description:
	Initialize the OS atomic_t.
*/

UINT16 RtmpOsNtohs(
	IN	UINT16					Value);

UINT16 RtmpOsHtons(
	IN	UINT16					Value);

u32 RtmpOsNtohl(
	IN	u32					Value);

u32 RtmpOsHtonl(
	IN	u32					Value);

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
	IN	bool 				bSet);

/* OS Network Interface */
int RtmpOSNetDevAddrSet(
	IN u8 				OpMode,
	IN struct net_device *				pNetDev,
	IN u8 *				pMacAddr,
	IN u8 *				dev_name);

void RtmpOSNetDevClose(
	IN struct net_device *				pNetDev);

void RtmpOSNetDevFree(
	IN	struct net_device *			pNetDev);

INT RtmpOSNetDevAlloc(
	IN	struct net_device *			*new_dev_p,
	IN	u32					privDataSize);

INT RtmpOSNetDevOpsAlloc(
	IN	void *				*pNetDevOps);


struct net_device *RtmpOSNetDevGetByName(
	IN	struct net_device *			pNetDev,
	IN	char *				pDevName);

void RtmpOSNetDeviceRefPut(
	IN	struct net_device *			pNetDev);

INT RtmpOSNetDevDestory(
	IN	void 				*pReserved,
	IN	struct net_device *			pNetDev);

void RtmpOSNetDevDetach(
	IN	struct net_device *			pNetDev);

int RtmpOSNetDevAttach(
	IN	u8 				OpMode,
	IN	struct net_device *			pNetDev,
	IN	RTMP_OS_NETDEV_OP_HOOK	*pDevOpHook);

void RtmpOSNetDevProtect(
	IN bool lock_it);

struct net_device *RtmpOSNetDevCreate(
	IN	INT32					MC_RowID,
	IN	u32					*pIoctlIF,
	IN	INT 					devType,
	IN	INT						devNum,
	IN	INT						privMemSize,
	IN	char *				pNamePrefix);

bool RtmpOSNetDevIsUp(
	IN	void 				*pDev);

unsigned char *RtmpOsNetDevGetPhyAddr(
	IN	void 				*pDev);

void RtmpOsNetQueueStart(
	IN	struct net_device *			pDev);

void RtmpOsNetQueueStop(
	IN	struct net_device *			pDev);

void RtmpOsNetQueueWake(
	IN	struct net_device *			pDev);

void RtmpOsSetPktNetDev(
	IN	void 				*pPkt,
	IN	void 				*pDev);

void RtmpOsSetNetDevPriv(struct net_device *pDev, struct rtmp_adapter *pPriv);
struct rtmp_adapter *RtmpOsGetNetDevPriv(struct net_device *pDev);
u32 RtmpDevPrivFlagsGet(struct net_device *pDev);
void RtmpDevPrivFlagsSet(struct net_device *pDev, u32 PrivFlags);

void RtmpOsSetNetDevType(void *pDev, USHORT Type);

void RtmpOsSetNetDevTypeMonitor(void *pDev);


/* OS Semaphore */
void RtmpOsCmdUp(RTMP_OS_TASK *pCmdQTask);
bool RtmpOsSemaInitLocked(struct semaphore *pSemOrg, LIST_HEADER *pSemList);
bool RtmpOsSemaInit(struct semaphore *pSemOrg, LIST_HEADER *pSemList);
bool RtmpOsSemaDestroy(struct semaphore *pSemOrg);
INT RtmpOsSemaWaitInterruptible(struct semaphore *pSemOrg);
void RtmpOsSemaWakeUp(struct semaphore *pSemOrg);
void RtmpOsMlmeUp(RTMP_OS_TASK *pMlmeQTask);

/* OS Task */
bool RtmpOsTaskletSche(RTMP_NET_TASK_STRUCT *pTasklet);

bool RtmpOsTaskletInit(
	RTMP_NET_TASK_STRUCT *pTasklet,
	void (*pFunc)(unsigned long data),
	ULONG Data,
	LIST_HEADER *pTaskletList);

bool RtmpOsTaskletKill(RTMP_NET_TASK_STRUCT *pTasklet);

void RtmpOsTaskletDataAssign(
	RTMP_NET_TASK_STRUCT *pTasklet,
	ULONG Data);

void RtmpOsTaskWakeUp(RTMP_OS_TASK *pTaskOrg);

INT32 RtmpOsTaskIsKilled(RTMP_OS_TASK *pTaskOrg);

bool RtmpOsCheckTaskLegality(RTMP_OS_TASK *pTaskOrg);

bool RtmpOSTaskAlloc(
	IN	RTMP_OS_TASK			*pTask,
	IN	LIST_HEADER				*pTaskList);

void RtmpOSTaskFree(
	IN	RTMP_OS_TASK			*pTask);

int RtmpOSTaskKill(
	IN	RTMP_OS_TASK			*pTaskOrg);

INT RtmpOSTaskNotifyToExit(
	IN	RTMP_OS_TASK			*pTaskOrg);

void RtmpOSTaskCustomize(
	IN	RTMP_OS_TASK			*pTaskOrg);

int RtmpOSTaskAttach(
	IN	RTMP_OS_TASK			*pTaskOrg,
	IN	RTMP_OS_TASK_CALLBACK	fn,
	IN	ULONG					arg);

int RtmpOSTaskInit(
	IN	RTMP_OS_TASK			*pTaskOrg,
	IN	char *				pTaskName,
	IN	void 				*pPriv,
	IN	LIST_HEADER				*pTaskList,
	IN	LIST_HEADER				*pSemList);

bool RtmpOSTaskWait(
	IN	void 				*pReserved,
	IN	RTMP_OS_TASK			*pTaskOrg,
	IN	INT32					*pStatus);

void *RtmpOsTaskDataGet(RTMP_OS_TASK *pTaskOrg);

INT32 RtmpThreadPidKill(RTMP_OS_PID	 PID);

/* OS Cache */
void RtmpOsDCacheFlush(ULONG AddrStart, ULONG Size);

/* OS Timer */
void RTMP_SetPeriodicTimer(struct timer_list *pTimerOrg, unsigned long timeout);

void RTMP_OS_Init_Timer(struct timer_list *pTimerOrg,
			TIMER_FUNCTION function, void *data,
			LIST_HEADER *pTimerList);

void RTMP_OS_Add_Timer(struct timer_list *pTimerOrg, unsigned long timeout);

void RTMP_OS_Mod_Timer(struct timer_list *pTimerOrg, unsigned long timeout);

void RTMP_OS_Del_Timer(struct timer_list *pTimerOrg, bool *pCancelled);

void RTMP_OS_Release_Timer(struct timer_list *pTimerOrg);

bool RTMP_OS_Alloc_Rsc(
	IN	LIST_HEADER				*pRscList,
	IN	void 					*pRsc,
	IN	u32					RscLen);

void RTMP_OS_Free_Rscs(
	IN	LIST_HEADER				*pRscList);

/* OS Lock */
bool RtmpOsAllocateLock(
	IN	spinlock_t			*pLock,
	IN	LIST_HEADER				*pLockList);

void RtmpOsFreeSpinLock(
	IN	spinlock_t			*pLockOrg);

void RtmpOsSpinLockBh(
	IN	spinlock_t			*pLockOrg);

void RtmpOsSpinUnLockBh(spinlock_t *pLockOrg);
void RtmpOsIntLock(spinlock_t *pLockOrg, ULONG *pIrqFlags);
void RtmpOsIntUnLock(spinlock_t *pLockOrg, ULONG IrqFlags);

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
	IN	u32					Offset,
	OUT UINT16					*pValue);

int RtmpOsPciConfigWriteWord(
	IN	void 				*pDev,
	IN	u32					Offset,
	IN	UINT16					Value);

int RtmpOsPciConfigReadDWord(
	IN	void 				*pDev,
	IN	u32					Offset,
	OUT u32					*pValue);

int RtmpOsPciConfigWriteDWord(
	IN	void 				*pDev,
	IN	u32					Offset,
	IN	u32					Value);

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
int RtmpOsUsbUrbStatusGet(void *pUrb);
ULONG RtmpOsUsbUrbLenGet(void *pUrb);

/* OS Atomic */
bool RtmpOsAtomicInit(RTMP_OS_ATOMIC *pAtomic, LIST_HEADER *pAtomicList);
void RtmpOsAtomicDestroy(RTMP_OS_ATOMIC *pAtomic);
LONG RtmpOsAtomicRead(RTMP_OS_ATOMIC *pAtomic);
void RtmpOsAtomicDec(RTMP_OS_ATOMIC *pAtomic);
void RtmpOsAtomicInterlockedExchange(RTMP_OS_ATOMIC *pAtomicSrc, LONG Value);

int RtmpOSWrielessEventSend(
	IN	struct net_device *			pNetDev,
	IN	u32					eventType,
	IN	INT						flags,
	IN	u8 *				pSrcMac,
	IN	u8 *				pData,
	IN	u32					dataLen);

int RtmpOSWrielessEventSendExt(
	IN	struct net_device *			pNetDev,
	IN	u32					eventType,
	IN	INT						flags,
	IN	u8 *				pSrcMac,
	IN	u8 *				pData,
	IN	u32					dataLen,
	IN	u32					family);

UINT RtmpOsWirelessExtVerGet(void);

void RtmpDrvAllMacPrint(
	IN void 					*pReserved,
	IN u32					*pBufMac,
	IN u32					AddrStart,
	IN u32					AddrEnd,
	IN u32					AddrStep);

void RtmpDrvAllE2PPrint(
	IN	void 				*pReserved,
	IN	USHORT					*pMacContent,
	IN	u32					AddrEnd,
	IN	u32					AddrStep);

int RtmpOSIRQRelease(
	IN	struct net_device *			pNetDev,
	IN	u32					infType,
	IN	PPCI_DEV				pci_dev,
	IN	bool 				*pHaveMsi);

void RtmpOsWlanEventSet(
	IN	void 				*pReserved,
	IN	bool 				*pCfgWEnt,
	IN	bool 				FlgIsWEntSup);

UINT16 RtmpOsGetUnaligned(UINT16 *pWord);

u32 RtmpOsGetUnaligned32(u32 *pWord);

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
	OUT u32 *pRate);

char * rtstrchr(const char * s, int c);

char *  WscGetAuthTypeStr(USHORT authFlag);

char *  WscGetEncryTypeStr(USHORT encryFlag);

USHORT WscGetAuthTypeFromStr(char *arg);

USHORT WscGetEncrypTypeFromStr(char *arg);

void RtmpMeshDown(
	IN void *pDrvCtrlBK,
	IN bool WaitFlag,
	IN bool (*RtmpMeshLinkCheck)(IN void *pAd));

USHORT RtmpOsNetPrivGet(struct net_device *pDev);

bool RtmpOsCmdDisplayLenCheck(
	IN	u32					LenSrc,
	IN	u32					Offset);

void    WpaSendMicFailureToWpaSupplicant(
	IN	struct net_device *			pNetDev,
    IN  bool 				bUnicast);

int wext_notify_event_assoc(
	IN	struct net_device *			pNetDev,
	IN	u8 				*ReqVarIEs,
	IN	u32					ReqVarIELen);

void    SendAssocIEsToWpaSupplicant(
	IN	struct net_device *			pNetDev,
	IN	u8 				*ReqVarIEs,
	IN	u32					ReqVarIELen);

/* ============================ rt_rbus_pci_util.c ========================== */
void RtmpAllocDescBuf(
	IN PPCI_DEV pPciDev,
	IN UINT Index,
	IN ULONG Length,
	IN bool Cached,
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
	IN bool Cached,
	OUT void **VirtualAddress,
	OUT PNDIS_PHYSICAL_ADDRESS	PhysicalAddress);

void RTMP_FreeFirstTxBuffer(
	IN	PPCI_DEV				pPciDev,
	IN	ULONG					Length,
	IN	bool 				Cached,
	IN	void *				VirtualAddress,
	IN	NDIS_PHYSICAL_ADDRESS	PhysicalAddress);

struct sk_buff * RTMP_AllocateRxPacketBuffer(
	IN	void 				*pReserved,
	IN	void 				*pPciDev,
	IN	ULONG					Length,
	IN	bool 				Cached,
	OUT	void *				*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS	PhysicalAddress);

dma_addr_t linux_pci_map_single(void *pPciDev, void *ptr, size_t size, int sd_idx, int direction);

void linux_pci_unmap_single(void *pPciDev, dma_addr_t dma_addr, size_t size, int direction);

/* ============================ rt_usb_util.c =============================== */
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
							dma_addr_t *dma);

void rausb_buffer_free(void *dev,
							size_t size,
							void *addr,
							dma_addr_t dma);

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
	IN	spinlock_t		*pBulkInLock,
	IN	u8 			*pPendingRx);


void RtmpOsUsbInitHTTxDesc(
	struct urb *pUrb,
	struct usb_device *pUsb_Dev,
	UINT BulkOutEpAddr,
	u8 *pSrc,
	ULONG BulkOutSize,
	USB_COMPLETE_HANDLER Func,
	void *pTxContext,
	dma_addr_t TransferDma);

void RtmpOsUsbInitRxDesc(
	struct urb *pUrb,
	struct usb_device *pUsb_Dev,
	UINT BulkInEpAddr,
	u8 *pTransferBuffer,
	u32 BufSize,
	USB_COMPLETE_HANDLER Func,
	void *pRxContext,
	dma_addr_t TransferDma);


u32 RtmpOsGetUsbDevVendorID(
	IN void *pUsbDev);

u32 RtmpOsGetUsbDevProductID(
	IN void *pUsbDev);

/* CFG80211 */
#ifdef RT_CFG80211_SUPPORT
typedef struct __CFG80211_BAND {

	u8 RFICType;
	u8 MpduDensity;
	u8 TxStream;
	u8 RxStream;
	u32 MaxTxPwr;
	u32 MaxBssTable;

	UINT16 RtsThreshold;
	UINT16 FragmentThreshold;
	u32 RetryMaxCnt; /* bit0~7: short; bit8 ~ 15: long */
	bool FlgIsBMode;
} CFG80211_BAND;

void CFG80211OS_UnRegister(
	IN void 					*pCB,
	IN void 					*pNetDev);

bool CFG80211_SupBandInit(
	IN void 					*pCB,
	IN CFG80211_BAND 			*pBandInfo,
	IN void 					*pWiphyOrg,
	IN void 					*pChannelsOrg,
	IN void 					*pRatesOrg);

bool CFG80211OS_SupBandReInit(
	IN void 					*pCB,
	IN CFG80211_BAND 			*pBandInfo);

void CFG80211OS_RegHint(
	IN void 					*pCB,
	IN u8 				*pCountryIe,
	IN ULONG					CountryIeLen);

void CFG80211OS_RegHint11D(
	IN void 					*pCB,
	IN u8 				*pCountryIe,
	IN ULONG					CountryIeLen);

bool CFG80211OS_BandInfoGet(
	IN void 					*pCB,
	IN void 					*pWiphyOrg,
	OUT void 				**ppBand24,
	OUT void 				**ppBand5);

u32 CFG80211OS_ChanNumGet(
	IN void 					*pCB,
	IN void 					*pWiphyOrg,
	IN u32					IdBand);

bool CFG80211OS_ChanInfoGet(
	IN void 					*pCB,
	IN void 					*pWiphyOrg,
	IN u32					IdBand,
	IN u32					IdChan,
	OUT u32					*pChanId,
	OUT u32					*pPower,
	OUT bool 				*pFlgIsRadar);

bool CFG80211OS_ChanInfoInit(
	IN void 					*pCB,
	IN u32					InfoIndex,
	IN u8 				ChanId,
	IN u8 				MaxTxPwr,
	IN bool 				FlgIsNMode,
	IN bool 				FlgIsBW20M);

void CFG80211OS_Scaning(
	IN void 					*pCB,
	IN u32					ChanId,
	IN u8 				*pFrame,
	IN u32					FrameLen,
	IN INT32					RSSI,
	IN bool 				FlgIsNMode,
	IN u8					BW);

void CFG80211OS_ScanEnd(
	IN void 					*pCB,
	IN bool 				FlgIsAborted);

void CFG80211OS_ConnectResultInform(
	IN void 					*pCB,
	IN u8 				*pBSSID,
	IN u8 				*pReqIe,
	IN u32					ReqIeLen,
	IN u8 				*pRspIe,
	IN u32					RspIeLen,
	IN u8 				FlgIsSuccess);
#endif /* RT_CFG80211_SUPPORT */




/* ================================ MACRO =================================== */
#define RTMP_UTIL_DCACHE_FLUSH(__AddrStart, __Size)

/* ================================ EXTERN ================================== */
extern u8 SNAP_802_1H[6];
extern u8 SNAP_BRIDGE_TUNNEL[6];
extern u8 EAPOL[2];
extern u8 TPID[];
extern u8 IPX[2];
extern u8 APPLE_TALK[2];
extern u8 NUM_BIT8[8];
extern ULONG RTPktOffsetData, RTPktOffsetLen, RTPktOffsetCB;

extern ULONG OS_NumOfMemAlloc, OS_NumOfMemFree;

extern INT32 ralinkrate[];
extern u32 RT_RateSize;

#ifdef PLATFORM_UBM_IPX8
#include "vrut_ubm.h"
#endif /* PLATFORM_UBM_IPX8 */

int OS_TEST_BIT(int bit, unsigned long *flags);
void OS_SET_BIT(int bit, unsigned long *flags);
void OS_CLEAR_BIT(int bit, unsigned long *flags);

#endif /* __RT_OS_UTIL_H__ */
