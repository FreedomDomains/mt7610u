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

#ifdef OS_ABL_SUPPORT
MODULE_LICENSE("GPL");
#endif /* OS_ABL_SUPPORT */

/*
========================================================================
Routine Description:
	Dump URB information.

Arguments:
	purb_org		- the URB

Return Value:
	None

Note:
========================================================================
*/
void dump_urb(void *purb_org)
{
	struct urb *purb = (struct urb *)purb_org;

	printk("urb                  :0x%08lx\n", (unsigned long)purb);
	printk("\tdev                   :0x%08lx\n", (unsigned long)purb->dev);
	printk("\t\tdev->state          :0x%d\n", purb->dev->state);
	printk("\tpipe                  :0x%08x\n", purb->pipe);
	printk("\tstatus                :%d\n", purb->status);
	printk("\ttransfer_flags        :0x%08x\n", purb->transfer_flags);
	printk("\ttransfer_buffer       :0x%08lx\n", (unsigned long)purb->transfer_buffer);
	printk("\ttransfer_buffer_length:%d\n", purb->transfer_buffer_length);
	printk("\tactual_length         :%d\n", purb->actual_length);
	printk("\tsetup_packet          :0x%08lx\n", (unsigned long)purb->setup_packet);
	printk("\tstart_frame           :%d\n", purb->start_frame);
	printk("\tnumber_of_packets     :%d\n", purb->number_of_packets);
	printk("\tinterval              :%d\n", purb->interval);
	printk("\terror_count           :%d\n", purb->error_count);
	printk("\tcontext               :0x%08lx\n", (unsigned long)purb->context);
	printk("\tcomplete              :0x%08lx\n\n", (unsigned long)purb->complete);
}

void RtmpOsUsbEmptyUrbCheck(
	IN	void 			**ppWait,
	IN	spinlock_t		*pBulkInLock,
	IN	u8 			*pPendingRx)
{
	u32 i = 0;
	DECLARE_WAIT_QUEUE_HEAD(unlink_wakeup);
	DECLARE_WAITQUEUE(wait, current);


	/* ensure there are no more active urbs. */
	add_wait_queue (&unlink_wakeup, &wait);
	*ppWait = &unlink_wakeup;

	/* maybe wait for deletions to finish. */
	i = 0;
	/*while((i < 25) && atomic_read(&pAd->PendingRx) > 0) */
	while(i < 25) {
/*		unsigned long IrqFlags; */

		RTMP_SEM_LOCK(pBulkInLock);
		if (*pPendingRx == 0) {
			RTMP_SEM_UNLOCK(pBulkInLock);
			break;
		}
		RTMP_SEM_UNLOCK(pBulkInLock);

		msleep(UNLINK_TIMEOUT_MS);	/*Time in millisecond */
		i++;
	}
	*ppWait = NULL;
	remove_wait_queue (&unlink_wakeup, &wait);
}


void RtmpOsUsbInitHTTxDesc(
	struct urb *pUrb,
	struct usb_device *pUsb_Dev,
	UINT BulkOutEpAddr,
	u8 *pSrc,
	ULONG BulkOutSize,
	USB_COMPLETE_HANDLER Func,
	void *pTxContext,
	dma_addr_t TransferDma)
{
	dma_addr_t DmaAddr = (dma_addr_t)(TransferDma);


	ASSERT(pUrb);

	/*Initialize a tx bulk urb */
	RTUSB_FILL_HTTX_BULK_URB(pUrb,
						pUsb_Dev,
						BulkOutEpAddr,
						pSrc,
						BulkOutSize,
						(usb_complete_t)Func,
						pTxContext,
						DmaAddr);
}


void RtmpOsUsbInitRxDesc(
	struct urb *pUrb,
	struct usb_device *pUsb_Dev,
	UINT BulkInEpAddr,
	u8 *pTransferBuffer,
	u32 BufSize,
	USB_COMPLETE_HANDLER Func,
	void *pRxContext,
	dma_addr_t TransferDma)
{
	dma_addr_t DmaAddr = (dma_addr_t)(TransferDma);


	ASSERT(pUrb);

	/*Initialize a rx bulk urb */
	RTUSB_FILL_RX_BULK_URB(pUrb,
						pUsb_Dev,
						BulkInEpAddr,
						pTransferBuffer,
						BufSize,
						(usb_complete_t)Func,
						pRxContext,
						DmaAddr);
}


/*
========================================================================
Routine Description:
	Get USB Vendor ID.

Arguments:
	pUsbDev			- the usb device

Return Value:
	the name

Note:
========================================================================
*/
u32 RtmpOsGetUsbDevVendorID(IN void *pUsbDev) {
	return ((struct usb_device *) pUsbDev)->descriptor.idVendor;
}

/*
========================================================================
Routine Description:
	Get USB Product ID.

Arguments:
	pUsbDev			- the usb device

Return Value:
	the name

Note:
========================================================================
*/
u32 RtmpOsGetUsbDevProductID(IN void *pUsbDev) {
	return ((struct usb_device *) pUsbDev)->descriptor.idProduct;
}

