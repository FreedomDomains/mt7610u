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


#ifndef __RT_LINUX_H__
#define __RT_LINUX_H__

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/ethtool.h>
#include <linux/wireless.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/if_arp.h>
#include <linux/ctype.h>
#include <linux/vmalloc.h>
#include <linux/usb.h>
#include <linux/wireless.h>
#include <net/iw_handler.h>

/* load firmware */
#define __KERNEL_SYSCALLS__
#include <linux/unistd.h>
#include <asm/uaccess.h>
#include <asm/types.h>
#include <asm/unaligned.h>	/* for get_unaligned() */

#include <linux/pid.h>

#ifdef RT_CFG80211_SUPPORT
#include <net/mac80211.h>
#define EXT_BUILD_CHANNEL_LIST		/* must define with CRDA */
#endif /* RT_CFG80211_SUPPORT */


/* must put the definition before include "os/rt_linux_cmm.h" */
#define KTHREAD_SUPPORT 1

#ifdef KTHREAD_SUPPORT
#include <linux/err.h>
#include <linux/kthread.h>
#endif /* KTHREAD_SUPPORT */


#include "os/rt_linux_cmm.h"

#ifdef RT_CFG80211_SUPPORT
#include "cfg80211.h"
#endif /* RT_CFG80211_SUPPORT */

#undef AP_WSC_INCLUDED
#undef STA_WSC_INCLUDED
#undef WSC_INCLUDED


#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */


#define MT7610_FIRMWARE_NAME	"mt7610u.bin"
#define MT7650_FIRMWARE_NAME	"mt7650u.bin"

typedef struct usb_device	*PUSB_DEV;
typedef struct usb_ctrlrequest devctrlrequest;

/***********************************************************************************
 *	Profile related sections
 ***********************************************************************************/


#ifdef CONFIG_STA_SUPPORT

extern	const struct iw_handler_def rt28xx_iw_handler_def;
#endif /* CONFIG_STA_SUPPORT */

/***********************************************************************************
 *	Compiler related definitions
 ***********************************************************************************/
#undef __inline
#define __inline		static inline
#define IN
#define OUT
#define INOUT


/***********************************************************************************
 *	OS Specific definitions and data structures
 ***********************************************************************************/
typedef struct pci_dev 		* PPCI_DEV;
typedef	dma_addr_t			NDIS_PHYSICAL_ADDRESS;
typedef	dma_addr_t			* PNDIS_PHYSICAL_ADDRESS;
typedef char 				* PNDIS_BUFFER;

typedef struct ifreq		NET_IOCTL;
typedef struct ifreq		* PNET_IOCTL;

typedef	struct pid *	RTMP_OS_PID;

typedef int (*HARD_START_XMIT_FUNC)(struct sk_buff *skb, struct net_device *net_dev);


#define RT_MOD_INC_USE_COUNT() \
	if (!try_module_get(THIS_MODULE)) \
	{ \
		DBGPRINT(RT_DEBUG_ERROR, ("%s: cannot reserve module\n", __FUNCTION__)); \
		return -1; \
	}

#define RT_MOD_DEC_USE_COUNT() module_put(THIS_MODULE);

#define RTMP_INC_REF(_A)		0
#define RTMP_DEC_REF(_A)		0
#define RTMP_GET_REF(_A)		0


/* This function will be called when query /proc */
struct iw_statistics *rt28xx_get_wireless_stats(
    IN struct net_device *net_dev);


/***********************************************************************************
 *	Network related constant definitions
 ***********************************************************************************/
#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif

#define ETH_LENGTH_OF_ADDRESS	6

#define NDIS_STATUS_SUCCESS                     0x00
#define NDIS_STATUS_FAILURE                     0x01
#define NDIS_STATUS_INVALID_DATA				0x02
#define NDIS_STATUS_RESOURCES                   0x03

#define NDIS_SET_PACKET_STATUS(_p, _status)			do{} while(0)
#define NdisWriteErrorLogEntry(_a, _b, _c, _d)		do{} while(0)

/* statistics counter */
#define STATS_INC_RX_PACKETS(_pAd, _dev)
#define STATS_INC_TX_PACKETS(_pAd, _dev)

#define STATS_INC_RX_BYTESS(_pAd, _dev, len)
#define STATS_INC_TX_BYTESS(_pAd, _dev, len)

#define STATS_INC_RX_ERRORS(_pAd, _dev)
#define STATS_INC_TX_ERRORS(_pAd, _dev)

#define STATS_INC_RX_DROPPED(_pAd, _dev)
#define STATS_INC_TX_DROPPED(_pAd, _dev)


/***********************************************************************************
 *	Ralink Specific network related constant definitions
 ***********************************************************************************/
#define MIN_NET_DEVICE_FOR_MBSSID		0x00		/*0x00,0x10,0x20,0x30 */
#define MIN_NET_DEVICE_FOR_WDS			0x10		/*0x40,0x50,0x60,0x70 */
#define MIN_NET_DEVICE_FOR_APCLI		0x20
#define MIN_NET_DEVICE_FOR_MESH			0x30
#ifdef CONFIG_STA_SUPPORT
#define MIN_NET_DEVICE_FOR_DLS			0x40
#define MIN_NET_DEVICE_FOR_TDLS			0x50
#endif /* CONFIG_STA_SUPPORT */

#define NET_DEVICE_REAL_IDX_MASK		0x0f		/* for each operation mode, we maximum support 15 entities. */


#ifdef CONFIG_STA_SUPPORT
#define NDIS_PACKET_TYPE_DIRECTED		0
#define NDIS_PACKET_TYPE_MULTICAST		1
#define NDIS_PACKET_TYPE_BROADCAST		2
#define NDIS_PACKET_TYPE_ALL_MULTICAST	3
#define NDIS_PACKET_TYPE_PROMISCUOUS	4
#endif /* CONFIG_STA_SUPPORT */


/***********************************************************************************
 *	OS signaling related constant definitions
 ***********************************************************************************/


/***********************************************************************************
 *	OS file operation related data structure definitions
 ***********************************************************************************/
typedef struct file* RTMP_OS_FD;

typedef struct _OS_FS_INFO_
{
// https://git.kernel.org/cgit/linux/kernel/git/stable/linux-stable.git/commit/include/linux/uidgid.h?id=7a4e7408c5cadb240e068a662251754a562355e3
// https://git.kernel.org/cgit/linux/kernel/git/stable/linux-stable.git/tree/include/linux?h=linux-3.4.y
// https://git.kernel.org/cgit/linux/kernel/git/stable/linux-stable.git/tree/include/linux?h=linux-3.5.y
// (uidgid.h)
	kuid_t				fsuid;
	kgid_t				fsgid;
	mm_segment_t	fs;
} OS_FS_INFO;

#define IS_FILE_OPEN_ERR(_fd) 	((_fd == NULL) || IS_ERR((_fd)))


/***********************************************************************************
 *	OS semaphore related data structure and definitions
 ***********************************************************************************/
struct os_lock  {
	spinlock_t		lock;
	unsigned long  	flags;
};


/* */
/*  spin_lock enhanced for Nested spin lock */
/* */


#define OS_SEM_UNLOCK(__lock)					\
{												\
	spin_unlock_bh((spinlock_t *)(__lock));		\
}


/* sample, use semaphore lock to replace IRQ lock, 2007/11/15 */

#define OS_INT_LOCK(__lock, __irqflags)			\
{												\
	spin_lock_irqsave((spinlock_t *)__lock, __irqflags);	\
}

#define OS_INT_UNLOCK(__lock, __irqflag)		\
{												\
	spin_unlock_irqrestore((spinlock_t *)(__lock), ((unsigned long)__irqflag));	\
}



#ifndef wait_event_interruptible_timeout
#define __wait_event_interruptible_timeout(wq, condition, ret) \
do { \
        wait_queue_t __wait; \
        init_waitqueue_entry(&__wait, current); \
        add_wait_queue(&wq, &__wait); \
        for (;;) { \
                set_current_state(TASK_INTERRUPTIBLE); \
                if (condition) \
                        break; \
                if (!signal_pending(current)) { \
                        ret = schedule_timeout(ret); \
                        if (!ret) \
                                break; \
                        continue; \
                } \
                ret = -ERESTARTSYS; \
                break; \
        } \
        current->state = TASK_RUNNING; \
        remove_wait_queue(&wq, &__wait); \
} while (0)

#define wait_event_interruptible_timeout(wq, condition, timeout) \
({ \
        long __ret = timeout; \
        if (!(condition)) \
                __wait_event_interruptible_timeout(wq, condition, __ret); \
        __ret; \
})
#endif

#define RTCMDUp					OS_RTCMDUp

#ifdef KTHREAD_SUPPORT
#define RTMP_WAIT_EVENT_INTERRUPTIBLE(_Status, _pTask) \
do { \
		wait_event_interruptible(_pTask->kthread_q, \
								 _pTask->kthread_running || kthread_should_stop()); \
		_pTask->kthread_running = false; \
		if (kthread_should_stop()) \
		{ \
			(_Status) = -1; \
			break; \
		} \
		else (_Status) = 0; \
} while(0)
#endif

#ifdef KTHREAD_SUPPORT
#define WAKE_UP(_pTask) \
	do{ \
		if ((_pTask)->kthread_task) \
        { \
			(_pTask)->kthread_running = true; \
	        wake_up(&(_pTask)->kthread_q); \
		} \
	}while(0)
#endif

/***********************************************************************************
 *	OS Memory Access related data structure and definitions
 ***********************************************************************************/
#define MEM_ALLOC_FLAG      (GFP_ATOMIC) /*(GFP_DMA | GFP_ATOMIC) */

/***********************************************************************************
 *	OS task related data structure and definitions
 ***********************************************************************************/
#define RTMP_OS_MGMT_TASK_FLAGS	CLONE_VM

/*typedef	struct pid *	THREAD_PID; // no use */
#define	THREAD_PID_INIT_VALUE	NULL
/* TODO: Use this IOCTL carefully when linux kernel version larger than 2.6.27, because the PID only correct when the user space task do this ioctl itself. */
/*#define RTMP_GET_OS_PID(_x, _y)    _x = get_task_pid(current, PIDTYPE_PID); */
#define RT_GET_OS_PID(_x, _y)		do{rcu_read_lock(); _x=(ULONG)current->pids[PIDTYPE_PID].pid; rcu_read_unlock();}while(0)
#define RTMP_GET_OS_PID(_a, _b)			RT_GET_OS_PID(_a, _b)
#define	GET_PID_NUMBER(_v)	pid_nr((_v))
#define CHECK_PID_LEGALITY(_pid)	if (pid_nr((_pid)) > 0)
#define KILL_THREAD_PID(_A, _B, _C)	kill_pid((_A), (_B), (_C))

#define ATE_KILL_THREAD_PID(PID)		KILL_THREAD_PID(PID, SIGTERM, 1)

typedef int (*cast_fn)(void *);
typedef INT (*RTMP_OS_TASK_CALLBACK)(ULONG);

typedef struct tasklet_struct OS_NET_TASK_STRUCT;
typedef struct tasklet_struct  *POS_NET_TASK_STRUCT;

/***********************************************************************************
 * Timer related definitions and data structures.
 **********************************************************************************/
#define OS_HZ			HZ

typedef void (*TIMER_FUNCTION)(unsigned long);

#define RTMP_TIME_AFTER(a,b)		\
	(typecheck(unsigned long, (unsigned long)a) && \
	 typecheck(unsigned long, (unsigned long)b) && \
	 ((long)(b) - (long)(a) < 0))

#define RTMP_TIME_AFTER_EQ(a,b)	\
	(typecheck(unsigned long, (unsigned long)a) && \
	 typecheck(unsigned long, (unsigned long)b) && \
	 ((long)(a) - (long)(b) >= 0))
#define RTMP_TIME_BEFORE(a,b)	RTMP_TIME_AFTER_EQ(b,a)

#define ONE_TICK 1

static inline void NdisGetSystemUpTime(ULONG *time)
{
	*time = jiffies;
}


/***********************************************************************************
 *	OS specific cookie data structure binding to RTMP_ADAPTER
 ***********************************************************************************/

struct os_cookie {

	struct usb_device		*pUsb_Dev;

	RTMP_NET_TASK_STRUCT rx_done_task;
	RTMP_NET_TASK_STRUCT cmd_rsp_event_task;
	RTMP_NET_TASK_STRUCT mgmt_dma_done_task;
	RTMP_NET_TASK_STRUCT ac0_dma_done_task;
	RTMP_NET_TASK_STRUCT ac1_dma_done_task;
	RTMP_NET_TASK_STRUCT ac2_dma_done_task;
	RTMP_NET_TASK_STRUCT ac3_dma_done_task;
	RTMP_NET_TASK_STRUCT hcca_dma_done_task;
	RTMP_NET_TASK_STRUCT tbtt_task;



	RTMP_NET_TASK_STRUCT null_frame_complete_task;
	RTMP_NET_TASK_STRUCT pspoll_frame_complete_task;

	RTMP_OS_PID			apd_pid; /*802.1x daemon pid */
	unsigned long			apd_pid_nr;
	INT						ioctl_if_type;
	INT 					ioctl_if;

	struct completion SentToMCUDone;
};

/***********************************************************************************
 *	OS debugging and printing related definitions and data structure
 ***********************************************************************************/
#define PRINT_MAC(addr)	\
	addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]

#ifdef DBG
extern int RTDebugLevel;
extern int RTDebugFunc;

#define DBGPRINT_RAW(Level, Fmt)    \
do{                                   \
	ULONG __gLevel = (Level) & 0xff;\
	ULONG __fLevel = ((Level)>>8) & 0xffffff;\
    if (__gLevel <= RTDebugLevel)      \
    {                               \
    	if ((RTDebugFunc == 0) || \
		((RTDebugFunc != 0) && (((__fLevel & RTDebugFunc)!= 0) || (__gLevel <= RT_DEBUG_ERROR))))\
        printk Fmt;               \
    }                               \
}while(0)

#define DBGPRINT(Level, Fmt)    DBGPRINT_RAW(Level, Fmt)


#define DBGPRINT_ERR(Fmt)           \
{                                   \
    printk("ERROR!!! ");          \
    printk Fmt;                  \
}

#define DBGPRINT_S(Status, Fmt)		\
{									\
	printk Fmt;					\
}
#else
#define DBGPRINT(Level, Fmt)
#define DBGPRINT_RAW(Level, Fmt)
#define DBGPRINT_S(Status, Fmt)
#define DBGPRINT_ERR(Fmt)
#endif

#undef  ASSERT
#ifdef DBG
#define ASSERT(x)                                                               \
{                                                                               \
    if (!(x))                                                                   \
    {                                                                           \
        printk(KERN_WARNING __FILE__ ":%d assert " #x "failed\n", __LINE__);    \
    }                                                                           \
}
#else
#define ASSERT(x)
#endif /* DBG */


/*********************************************************************************************************
	The following code are not revised, temporary put it here.
  *********************************************************************************************************/


/***********************************************************************************
 * Device DMA Access related definitions and data structures.
 **********************************************************************************/
dma_addr_t linux_pci_map_single(void *handle, void *ptr, size_t size, int sd_idx, int direction);
void linux_pci_unmap_single(void *handle, dma_addr_t dma_addr, size_t size, int direction);

#define PCI_MAP_SINGLE_DEV(_handle, _ptr, _size, _sd_idx, _dir)				\
	linux_pci_map_single(_handle, _ptr, _size, _sd_idx, _dir)

#define PCI_UNMAP_SINGLE(_pAd, _ptr, _size, _dir)						\
	linux_pci_unmap_single(((struct os_cookie *)(_pAd->OS_Cookie))->pci_dev, _ptr, _size, _dir)

#define PCI_ALLOC_CONSISTENT(_pci_dev, _size, _ptr) \
	pci_alloc_consistent(_pci_dev, _size, _ptr)

#define PCI_FREE_CONSISTENT(_pci_dev, _size, _virtual_addr, _physical_addr) \
	pci_free_consistent(_pci_dev, _size, _virtual_addr, _physical_addr)



/*#define PCI_MAP_SINGLE(_handle, _ptr, _size, _dir) (ULONG)0 */
/*#define PCI_UNMAP_SINGLE(_handle, _ptr, _size, _dir) */


/*
 * ULONG
 * RTMP_GetPhysicalAddressLow(
 *   IN NDIS_PHYSICAL_ADDRESS  PhysicalAddress);
 */
#define RTMP_GetPhysicalAddressLow(PhysicalAddress)		(PhysicalAddress)

/*
 * ULONG
 * RTMP_GetPhysicalAddressHigh(
 *   IN NDIS_PHYSICAL_ADDRESS  PhysicalAddress);
 */
#define RTMP_GetPhysicalAddressHigh(PhysicalAddress)		(0)

/*
 * VOID
 * RTMP_SetPhysicalAddressLow(
 *   IN NDIS_PHYSICAL_ADDRESS  PhysicalAddress,
 *   IN ULONG  Value);
 */
#define RTMP_SetPhysicalAddressLow(PhysicalAddress, Value)	\
			PhysicalAddress = Value;

/*
 * VOID
 * RTMP_SetPhysicalAddressHigh(
 *   IN NDIS_PHYSICAL_ADDRESS  PhysicalAddress,
 *   IN ULONG  Value);
 */
#define RTMP_SetPhysicalAddressHigh(PhysicalAddress, Value)

#define NdisMIndicateStatus(_w, _x, _y, _z)



/***********************************************************************************
 * Device Register I/O Access related definitions and data structures.
 **********************************************************************************/

#define RTMP_OS_NETDEV_STATE_RUNNING(_pNetDev)	((_pNetDev)->flags & IFF_UP)

#define RTMP_OS_NETDEV_GET_DEVNAME(_pNetDev)	((_pNetDev)->name)
#define RTMP_OS_NETDEV_GET_PHYADDR(_pNetDev)	((_pNetDev)->dev_addr)

/* Get & Set NETDEV interface hardware type */
#define RTMP_OS_NETDEV_GET_TYPE(_pNetDev)			((_pNetDev)->type)
#define RTMP_OS_NETDEV_SET_TYPE(_pNetDev, _type)	((_pNetDev)->type = (_type))
#define RTMP_OS_NETDEV_SET_TYPE_MONITOR(_pNetDev)	RTMP_OS_NETDEV_SET_TYPE(_pNetDev, ARPHRD_IEEE80211_PRISM)

#define RTMP_OS_NETDEV_START_QUEUE(_pNetDev)	netif_start_queue((_pNetDev))
#define RTMP_OS_NETDEV_STOP_QUEUE(_pNetDev)	netif_stop_queue((_pNetDev))
#define RTMP_OS_NETDEV_WAKE_QUEUE(_pNetDev)	netif_wake_queue((_pNetDev))
#define RTMP_OS_NETDEV_CARRIER_OFF(_pNetDev)	netif_carrier_off((_pNetDev))

#define QUEUE_ENTRY_TO_PACKET(pEntry) \
	(struct sk_buff *)(pEntry)

#define PACKET_TO_QUEUE_ENTRY(pPacket) \
	(PQUEUE_ENTRY)(pPacket)

#define GET_SG_LIST_FROM_PACKET(_p, _sc)	\
    rt_get_sg_list_from_packet(_p, _sc)

/*
 * packet helper
 * 	- convert internal rt packet to os packet or
 *             os packet to rt packet
 */



#define get_unaligned32							get_unaligned
#define get_unalignedlong						get_unaligned

#define OS_NTOHS(_Val) \
		(ntohs((_Val)))
#define OS_HTONS(_Val) \
		(htons((_Val)))
#define OS_NTOHL(_Val) \
		(ntohl((_Val)))
#define OS_HTONL(_Val) \
		(htonl((_Val)))

#define CB_OFF  10

#define GET_OS_PKT_CB(_p)						((_p)->cb)

/* User Priority */
#define RTMP_SET_PACKET_UP(_p, _prio)			((_p)->cb[CB_OFF+0] = _prio)
#define RTMP_GET_PACKET_UP(_p)					((_p)->cb[CB_OFF+0])

/* Fragment # */
#define RTMP_SET_PACKET_FRAGMENTS(_p, _num)		((_p)->cb[CB_OFF+1] = _num)
#define RTMP_GET_PACKET_FRAGMENTS(_p)			((_p)->cb[CB_OFF+1])

/* 0x0 ~0x7f: TX to AP's own BSS which has the specified AID. if AID>127, set bit 7 in RTMP_SET_PACKET_EMACTAB too. */
/*(this value also as MAC(on-chip WCID) table index) */
/* 0x80~0xff: TX to a WDS link. b0~6: WDS index */
#define RTMP_SET_PACKET_WCID(_p, _wdsidx)		((_p)->cb[CB_OFF+2] = _wdsidx)
#define RTMP_GET_PACKET_WCID(_p)          		((u8)((_p)->cb[CB_OFF+2]))

/* RTS/CTS-to-self protection method */
#define RTMP_SET_PACKET_RTS(_p, _num)      		((_p)->cb[CB_OFF+4] = _num)
#define RTMP_GET_PACKET_RTS(_p)          		((_p)->cb[CB_OFF+4])
/* see RTMP_S(G)ET_PACKET_EMACTAB */

/* TX rate index */
#define RTMP_SET_PACKET_TXRATE(_p, _rate)		((_p)->cb[CB_OFF+5] = _rate)
#define RTMP_GET_PACKET_TXRATE(_p)		  		((_p)->cb[CB_OFF+5])

/* From which Interface */
#define RTMP_SET_PACKET_IF(_p, _ifdx)		((_p)->cb[CB_OFF+6] = _ifdx)
#define RTMP_GET_PACKET_IF(_p)		  		((_p)->cb[CB_OFF+6])
#define RTMP_SET_PACKET_NET_DEVICE_MBSSID(_p, _bss)		RTMP_SET_PACKET_IF((_p), (_bss))
#define RTMP_SET_PACKET_NET_DEVICE_WDS(_p, _bss)		RTMP_SET_PACKET_IF((_p), ((_bss) + MIN_NET_DEVICE_FOR_WDS))
#define RTMP_SET_PACKET_NET_DEVICE_APCLI(_p, _idx)   	RTMP_SET_PACKET_IF((_p), ((_idx) + MIN_NET_DEVICE_FOR_APCLI))
#define RTMP_SET_PACKET_NET_DEVICE_MESH(_p, _idx)   	RTMP_SET_PACKET_IF((_p), ((_idx) + MIN_NET_DEVICE_FOR_MESH))
#define RTMP_SET_PACKET_NET_DEVICE_P2P(_p, _idx)   		RTMP_SET_PACKET_IF((_p), ((_idx) + MIN_NET_DEVICE_FOR_P2P_GO))
#define RTMP_GET_PACKET_NET_DEVICE_MBSSID(_p)			RTMP_GET_PACKET_IF((_p))
#define RTMP_GET_PACKET_NET_DEVICE(_p)					RTMP_GET_PACKET_IF((_p))

#define RTMP_SET_PACKET_MOREDATA(_p, _morebit)		((_p)->cb[CB_OFF+7] = _morebit)
#define RTMP_GET_PACKET_MOREDATA(_p)				((_p)->cb[CB_OFF+7])




/* */
/*	Sepcific Pakcet Type definition */
/* */
#define RTMP_PACKET_SPECIFIC_CB_OFFSET	11

#define RTMP_PACKET_SPECIFIC_DHCP		0x01
#define RTMP_PACKET_SPECIFIC_EAPOL		0x02
#define RTMP_PACKET_SPECIFIC_IPV4		0x04
#define RTMP_PACKET_SPECIFIC_WAI		0x08
#define RTMP_PACKET_SPECIFIC_VLAN		0x10
#define RTMP_PACKET_SPECIFIC_LLCSNAP	0x20
#define RTMP_PACKET_SPECIFIC_TDLS		0x40

/*Specific */
#define RTMP_SET_PACKET_SPECIFIC(_p, _flg)	   	((_p)->cb[CB_OFF+11] = _flg)

/*DHCP */
#define RTMP_SET_PACKET_DHCP(_p, _flg)   													\
			do{																				\
				if (_flg)																	\
					((_p)->cb[CB_OFF+11]) |= (RTMP_PACKET_SPECIFIC_DHCP);		\
				else																		\
					((_p)->cb[CB_OFF+11]) &= (~RTMP_PACKET_SPECIFIC_DHCP);	\
			}while(0)
#define RTMP_GET_PACKET_DHCP(_p)		((_p)->cb[CB_OFF+11] & RTMP_PACKET_SPECIFIC_DHCP)

/*EAPOL */
#define RTMP_SET_PACKET_EAPOL(_p, _flg)   													\
			do{																				\
				if (_flg)																	\
					((_p)->cb[CB_OFF+11]) |= (RTMP_PACKET_SPECIFIC_EAPOL);		\
				else																		\
					((_p)->cb[CB_OFF+11]) &= (~RTMP_PACKET_SPECIFIC_EAPOL);	\
			}while(0)
#define RTMP_GET_PACKET_EAPOL(_p)		((_p)->cb[CB_OFF+11] & RTMP_PACKET_SPECIFIC_EAPOL)

/*WAI */
#define RTMP_SET_PACKET_WAI(_p, _flg)   													\
			do{																				\
				if (_flg)																	\
					((_p)->cb[CB_OFF+11]) |= (RTMP_PACKET_SPECIFIC_WAI);		\
				else																		\
					((_p)->cb[CB_OFF+11]) &= (~RTMP_PACKET_SPECIFIC_WAI);	\
			}while(0)
#define RTMP_GET_PACKET_WAI(_p)		((_p)->cb[CB_OFF+11] & RTMP_PACKET_SPECIFIC_WAI)

#define RTMP_GET_PACKET_LOWRATE(_p)		((_p)->cb[CB_OFF+11] & (RTMP_PACKET_SPECIFIC_EAPOL | RTMP_PACKET_SPECIFIC_DHCP | RTMP_PACKET_SPECIFIC_WAI))

/*VLAN */
#define RTMP_SET_PACKET_VLAN(_p, _flg)   													\
			do{																				\
				if (_flg)																	\
					((_p)->cb[CB_OFF+11]) |= (RTMP_PACKET_SPECIFIC_VLAN);		\
				else																		\
					((_p)->cb[CB_OFF+11]) &= (~RTMP_PACKET_SPECIFIC_VLAN);	\
			}while(0)
#define RTMP_GET_PACKET_VLAN(_p)		((_p)->cb[CB_OFF+11] & RTMP_PACKET_SPECIFIC_VLAN)

/*LLC/SNAP */
#define RTMP_SET_PACKET_LLCSNAP(_p, _flg)   													\
			do{																				\
				if (_flg)																	\
					((_p)->cb[CB_OFF+11]) |= (RTMP_PACKET_SPECIFIC_LLCSNAP);		\
				else																		\
					((_p)->cb[CB_OFF+11]) &= (~RTMP_PACKET_SPECIFIC_LLCSNAP);		\
			}while(0)

#define RTMP_GET_PACKET_LLCSNAP(_p)		((_p)->cb[CB_OFF+11] & RTMP_PACKET_SPECIFIC_LLCSNAP)

/* IP */
#define RTMP_SET_PACKET_IPV4(_p, _flg)														\
			do{																				\
				if (_flg)																	\
					((_p)->cb[CB_OFF+11]) |= (RTMP_PACKET_SPECIFIC_IPV4);		\
				else																		\
					((_p)->cb[CB_OFF+11]) &= (~RTMP_PACKET_SPECIFIC_IPV4);	\
			}while(0)

#define RTMP_GET_PACKET_IPV4(_p)		((_p)->cb[CB_OFF+11] & RTMP_PACKET_SPECIFIC_IPV4)

// TDLS
#define RTMP_SET_PACKET_TDLS(_p, _flg)														\
			do{																				\
				if (_flg)																	\
					((_p)->cb[CB_OFF+11]) |= (RTMP_PACKET_SPECIFIC_TDLS);		\
				else																		\
					((_p)->cb[CB_OFF+11]) &= (~RTMP_PACKET_SPECIFIC_TDLS);	\
			}while(0)

#define RTMP_GET_PACKET_TDLS(_p)		((_p)->cb[CB_OFF+11] & RTMP_PACKET_SPECIFIC_TDLS)

/* If this flag is set, it indicates that this EAPoL frame MUST be clear. */
#define RTMP_SET_PACKET_CLEAR_EAP_FRAME(_p, _flg)   ((_p)->cb[CB_OFF+12] = _flg)
#define RTMP_GET_PACKET_CLEAR_EAP_FRAME(_p)         ((_p)->cb[CB_OFF+12])


#ifdef WFA_VHT_PF
#define MAX_PACKETS_IN_QUEUE				2048 /*(512)*/
#else
#define MAX_PACKETS_IN_QUEUE				1024 /*(512)*/
#endif /* WFA_VHT_PF */


/* use bit3 of cb[CB_OFF+16] */
#define RTMP_SET_PACKET_MGMT_PKT(_p, _flg)	        \
        (_p)->cb[CB_OFF+16] = ((_p)->cb[CB_OFF+16] & 0xF7) | ((_flg & 0x01) << 3);
#define RTMP_GET_PACKET_MGMT_PKT(_p)				\
		(((_p)->cb[CB_OFF+16] & 0x08) >> 3)

/* use bit0 of cb[CB_OFF+20] */
#define RTMP_SET_PACKET_MGMT_PKT_DATA_QUE(_p, _flg)	\
        (_p)->cb[CB_OFF+20] = ((_p)->cb[CB_OFF+20] & 0xFE) | (_flg & 0x01);
#define RTMP_GET_PACKET_MGMT_PKT_DATA_QUE(_p)		\
		((_p)->cb[CB_OFF+20] & 0x01)

#define RTMP_SET_PACKET_5VT(_p, _flg)   ((_p)->cb[CB_OFF+22] = _flg)
#define RTMP_GET_PACKET_5VT(_p)         ((_p)->cb[CB_OFF+22])

#define RTMP_SET_PACKET_PROTOCOL(_p, _protocol) {\
	((_p)->cb[CB_OFF+23] = (u8)((_protocol) & 0x00ff)); \
	((_p)->cb[CB_OFF+24] = (u8)(((_protocol) & 0xff00) >> 8)); \
}

#define RTMP_GET_PACKET_PROTOCOL(_p) \
	((((UINT16)((_p)->cb[CB_OFF+24]) & 0x00ff) << 8) \
	| ((UINT16)((_p)->cb[CB_OFF+23]) & 0x00ff))

#define NAF_TYPE_CSO	1<<0
#define NAF_TYPE_TSO	1<<1
#define RTMP_GET_PKT_NAF_TYPE(_p)	(((_p)->cb[CB_OFF+31]) & (NAF_TYPE_CSO | NAF_TYPE_TSO))



//#ifdef MAC_REPEATER_SUPPORT
#define RTMP_SET_PKT_MAT_FREE(_p, _flg)	(((_p)->cb[CB_OFF+36]) = (_flg))
#define RTMP_GET_PKT_MAT_FREE(_p)		(((_p)->cb[CB_OFF+36]))
//#endif /* MAC_REPEATER_SUPPORT */

/* Max skb->cb = 48B = [CB_OFF+38] */



/***********************************************************************************
 *	Other function prototypes definitions
 ***********************************************************************************/
int rt28xx_packet_xmit(void *skb);


INT rt28xx_ioctl(
	IN	struct net_device *	net_dev,
	IN	OUT	struct ifreq	*rq,
	IN	INT			cmd);

extern int ra_mtd_write(int num, loff_t to, size_t len, const u_char *buf);
extern int ra_mtd_read(int num, loff_t from, size_t len, u_char *buf);


#define _GET_PAD_FROM_NET_DEV(_pAd, _net_dev)	(_pAd) = (_net_dev)->ml_priv;

#define GET_PAD_FROM_NET_DEV(_pAd, _net_dev)						\
	_pAd = RTMP_OS_NETDEV_GET_PRIV(_net_dev);

/******************************************************************************

  	USB related definitions

******************************************************************************/

#define BULKAGGRE_SIZE				60 /* 100 */

/*#define RT28XX_PUT_DEVICE			usb_put_dev */

#define RT28XX_PUT_DEVICE(dev_p)

/* unlink urb */

/* Prototypes of completion funuc. */
#define RtmpUsbBulkOutDataPacketComplete		RTUSBBulkOutDataPacketComplete
#define RtmpUsbBulkOutMLMEPacketComplete		RTUSBBulkOutMLMEPacketComplete
#define RtmpUsbBulkOutNullFrameComplete			RTUSBBulkOutNullFrameComplete
#define RtmpUsbBulkOutRTSFrameComplete			RTUSBBulkOutRTSFrameComplete
#define RtmpUsbBulkOutPsPollComplete			RTUSBBulkOutPsPollComplete
#define RtmpUsbBulkRxComplete					RTUSBBulkRxComplete
#define RtmpUsbBulkCmdRspEventComplete			RTUSBBulkCmdRspEventComplete

#define RTUSBBulkOutDataPacketComplete(Status, pURB, pt_regs)    RTUSBBulkOutDataPacketComplete(pURB)
#define RTUSBBulkOutMLMEPacketComplete(Status, pURB, pt_regs)    RTUSBBulkOutMLMEPacketComplete(pURB)
#define RTUSBBulkOutNullFrameComplete(Status, pURB, pt_regs)     RTUSBBulkOutNullFrameComplete(pURB)
#define RTUSBBulkOutRTSFrameComplete(Status, pURB, pt_regs)      RTUSBBulkOutRTSFrameComplete(pURB)
#define RTUSBBulkOutPsPollComplete(Status, pURB, pt_regs)        RTUSBBulkOutPsPollComplete(pURB)
#define RTUSBBulkRxComplete(Status, pURB, pt_regs)               RTUSBBulkRxComplete(pURB)
#define RTUSBBulkCmdRspEventComplete(Status, pURB, pt_regs)		 RTUSBBulkCmdRspEventComplete(pURB)

/*extern void dump_urb(struct urb *purb); */

#define InterlockedIncrement 	 	atomic_inc
#define NdisInterlockedIncrement 	atomic_inc
#define InterlockedDecrement		atomic_dec
#define NdisInterlockedDecrement 	atomic_dec
#define InterlockedExchange			atomic_set

typedef void USBHST_STATUS;
typedef INT32 URBCompleteStatus;
typedef struct pt_regs pregs;

USBHST_STATUS RTUSBBulkOutDataPacketComplete(URBCompleteStatus Status, struct urb *pURB, pregs *pt_regs);
USBHST_STATUS RTUSBBulkOutMLMEPacketComplete(URBCompleteStatus Status, struct urb *pURB, pregs *pt_regs);
USBHST_STATUS RTUSBBulkOutNullFrameComplete(URBCompleteStatus Status, struct urb *pURB, pregs *pt_regs);
USBHST_STATUS RTUSBBulkOutRTSFrameComplete(URBCompleteStatus Status, struct urb *pURB, pregs *pt_regs);
USBHST_STATUS RTUSBBulkOutPsPollComplete(URBCompleteStatus Status, struct urb *pURB, pregs *pt_regs);
USBHST_STATUS RTUSBBulkRxComplete(URBCompleteStatus Status, struct urb *pURB, pregs *pt_regs);
USBHST_STATUS RTUSBBulkCmdRspEventComplete(URBCompleteStatus Status, struct urb *pURB, pregs *pt_regs);

/* Fill Bulk URB Macro */

#define USB_CONTROL_MSG		usb_control_msg


INT RtmpOSNetDevOpsAlloc(
	IN void **pNetDevOps);

#define RTMP_OS_MAX_SCAN_DATA_GET()		IW_SCAN_MAX_DATA

#include "os/rt_os.h"

#endif /* __RT_LINUX_H__ */
