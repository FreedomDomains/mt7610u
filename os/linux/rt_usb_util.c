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

#ifdef RTMP_MAC_USB
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



#ifdef CONFIG_STA_SUPPORT
#ifdef CONFIG_PM
#ifdef USB_SUPPORT_SELECTIVE_SUSPEND


void rausb_autopm_put_interface( void *intf)
{
	usb_autopm_put_interface((struct usb_interface *)intf);
}

int  rausb_autopm_get_interface( void *intf)
{
	return usb_autopm_get_interface((struct usb_interface *)intf);
}



/*
========================================================================
Routine Description:
	RTMP_Usb_AutoPM_Put_Interface

Arguments:


Return Value:


Note:
========================================================================
*/

int RTMP_Usb_AutoPM_Put_Interface (
	IN	void 		*pUsb_Devsrc,
	IN	void 		*intfsrc)
{

	INT	 pm_usage_cnt;
	struct usb_interface	*intf =(struct usb_interface *)intfsrc;

		pm_usage_cnt = atomic_read(&intf->pm_usage_cnt);

		if (pm_usage_cnt == 1)
		{
			rausb_autopm_put_interface(intf);

              }

			return 0;
}

EXPORT_SYMBOL(RTMP_Usb_AutoPM_Put_Interface);

/*
========================================================================
Routine Description:
	RTMP_Usb_AutoPM_Get_Interface

Arguments:


Return Value: (-1)  error (resume fail )    1 success ( resume success)  2  (do  nothing)


Note:
========================================================================
*/

int RTMP_Usb_AutoPM_Get_Interface (
	IN	void 		*pUsb_Devsrc,
	IN	void 		*intfsrc)
{

	INT	 pm_usage_cnt;
	struct usb_interface	*intf =(struct usb_interface *)intfsrc;

	pm_usage_cnt = (INT)atomic_read(&intf->pm_usage_cnt);

	if (pm_usage_cnt == 0)
	{
		int res=1;

		res = rausb_autopm_get_interface(intf);
		if (res)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("AsicSwitchChannel autopm_resume fail ------\n"));
			return (-1);
		}

	}
	return 2;


}

EXPORT_SYMBOL(RTMP_Usb_AutoPM_Get_Interface);

#endif /* USB_SUPPORT_SELECTIVE_SUSPEND */
#endif /* CONFIG_PM */
#endif /* CONFIG_STA_SUPPORT */





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
	while(i < 25)
	{
/*		unsigned long IrqFlags; */

		RTMP_SEM_LOCK(pBulkInLock);
		if (*pPendingRx == 0)
		{
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
	IN	void 		*pUrbSrc,
	IN	void 		*pUsb_Dev,
	IN	UINT			BulkOutEpAddr,
	IN	u8 *			pSrc,
	IN	ULONG			BulkOutSize,
	IN	USB_COMPLETE_HANDLER	Func,
	IN	void 		*pTxContext,
	IN	dma_addr_t		TransferDma)
{
	PURB pUrb = (PURB)pUrbSrc;
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
	IN	void 		*pUrbSrc,
	IN	void 		*pUsb_Dev,
	IN	UINT			BulkInEpAddr,
	IN	u8 		*pTransferBuffer,
	IN	u32			BufSize,
	IN	USB_COMPLETE_HANDLER	Func,
	IN	void 		*pRxContext,
	IN	dma_addr_t		TransferDma)
{
	PURB pUrb = (PURB)pUrbSrc;
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

void RtmpOsUsbDmaMapping(
	IN	void 		*pUrb)
{
	RTUSB_URB_DMA_MAPPING(((purbb_t)pUrb));
}


/*
========================================================================
Routine Description:
	Get the data pointer from the URB.

Arguments:
	pUrb			- USB URB

Return Value:
	the data pointer

Note:
========================================================================
*/
void *RtmpOsUsbUrbDataGet(
	IN	void 				*pUrb)
{
	return RTMP_USB_URB_DATA_GET(pUrb);
}


/*
========================================================================
Routine Description:
	Get the status from the URB.

Arguments:
	pUrb			- USB URB

Return Value:
	the status

Note:
========================================================================
*/
int RtmpOsUsbUrbStatusGet(
	IN	void 				*pUrb)
{
	return RTMP_USB_URB_STATUS_GET(pUrb);
}


/*
========================================================================
Routine Description:
	Get the data length from the URB.

Arguments:
	pUrb			- USB URB

Return Value:
	the data length

Note:
========================================================================
*/
ULONG RtmpOsUsbUrbLenGet(
	IN	void 				*pUrb)
{
	return RTMP_USB_URB_LEN_GET(pUrb);
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

#endif /* RTMP_MAC_USB */

