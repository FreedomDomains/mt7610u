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


#include	"rt_config.h"

#define MAX_VENDOR_REQ_RETRY_COUNT  10

void RTUSBMultiWrite(struct rtmp_adapter *pAd, USHORT Offset,
		    u8 *key, int keylen)
{
	u16 val, idx = 0;

	do {
		val =(u16)( *key  | (*(key + 1) << 8));

		mt7610u_vendor_request(pAd,
				    DEVICE_VENDOR_REQUEST_OUT,
				    MT_VEND_SINGLE_WRITE,
				    val, Offset + idx,
				    NULL, 0);

		key += 2;
		idx += 2;
		keylen -= 2;

        } while(keylen > 0);
}

/*
	========================================================================

	Routine Description: Read 32-bit MAC register

	Arguments:

	Return Value:

	IRQL =

	Note:

	========================================================================
*/
u32 mt76u_reg_read(struct rtmp_adapter *pAd, USHORT Offset)
{
	int status;
	u32 val;

	status = mt7610u_vendor_request(pAd,
		DEVICE_VENDOR_REQUEST_IN,
		MT_VEND_MULTI_READ,
		0, Offset,
		&val, 4);

	if (status != 0)
		val = 0xffffffff;

	return le2cpu32(val);
}


/*
	========================================================================

	Routine Description: Write 32-bit MAC register

	Arguments:

	Return Value:

	IRQL =

	Note:

	========================================================================
*/
void mt76u_reg_write(
	struct rtmp_adapter *pAd,
	USHORT Offset,
	u32 _val)
{
	u32 val = cpu2le32(_val);

	/* MT76xx HW has 4 byte alignment constrained */

	mt7610u_vendor_request(pAd,
			DEVICE_VENDOR_REQUEST_OUT,
			MT_VEND_MULTI_WRITE,
			0, Offset,
			&val, 4);
}

/*
	========================================================================

	Routine Description:

	Arguments:

	Return Value:

	IRQL =

	Note:

	========================================================================
*/

u16 mt76u_read_eeprom(struct rtmp_adapter *pAd, u16 offset)
{
	u16 localData;

	mt7610u_vendor_request(pAd,
			DEVICE_VENDOR_REQUEST_IN,
			MT_VEND_READ_EEPROM,
			0, offset,
			&localData, 2);

	return le2cpu16(localData);
}

/*
    ========================================================================
 	Routine Description:
		mt7610u_vendor_request - Builds a ralink specific request, sends it off to USB endpoint zero and waits for completion

	Arguments:
		@pAd:
	  	@TransferFlags:
	  	@RequestType: USB message request type value
	  	@Request: USB message request value
	  	@Value: USB message value
	  	@Index: USB message index value
	  	@TransferBuffer: USB data to be sent
	  	@TransferBufferLength: Lengths in bytes of the data to be sent

	Context: ! in atomic context

	Return Value:
		NDIS_STATUS_SUCCESS
		NDIS_STATUS_FAILURE

	Note:
		This function sends a simple control message to endpoint zero
		and waits for the message to complete, or CONTROL_TIMEOUT_JIFFIES timeout.
		Because it is synchronous transfer, so don't use this function within an atomic context,
		otherwise system will hang, do be careful.

		TransferBuffer may located in stack region which may not in DMA'able region in some embedded platforms,
		so need to copy TransferBuffer to vend_buf allocated by kmalloc to do DMA transfer.
		Use UsbVendorReq_semaphore to protect this region which may be accessed by multi task.
		Normally, coherent issue is resloved by low-level HC driver, so do not flush this zone by mt7610u_vendor_request.

	========================================================================
*/

int mt7610u_vendor_request(struct rtmp_adapter *pAd, u8 requesttype, enum mt_vendor_req request,
			u16 value, u16 index, void *data, u16 size)
{
	int ret = 0;
	struct usb_device *udev = mt7610u_to_usb_dev(pAd);

	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)) {
		DBGPRINT(RT_DEBUG_ERROR, ("WIFI device has been disconnected\n"));
		return NDIS_STATUS_FAILURE;
	} else if (RTMP_TEST_PSFLAG(pAd, fRTMP_PS_MCU_SLEEP)) {
		DBGPRINT(RT_DEBUG_ERROR, ("MCU has entered sleep mode\n"));
		return NDIS_STATUS_FAILURE;
	} else {
		int RetryCount = 0; /* RTUSB_CONTROL_MSG retry counts*/

		ret = down_interruptible(&(pAd->UsbVendorReq_semaphore));
		if (ret != 0) {
			DBGPRINT(RT_DEBUG_ERROR, ("UsbVendorReq_semaphore get failed\n"));
			return NDIS_STATUS_FAILURE;
		}

		if ((size > 0) && (requesttype == DEVICE_VENDOR_REQUEST_OUT))
			memmove(pAd->vend_buf, data, size);

		do {
			unsigned int pipe = (requesttype == DEVICE_VENDOR_REQUEST_OUT) ?
					     usb_sndctrlpipe(udev, 0) :
					     usb_rcvctrlpipe(udev, 0);

			ret = usb_control_msg(udev,
					    pipe,
					    request,
					    requesttype,
					    value,
					    index,
					    pAd->vend_buf,
					    size,
					    CONTROL_TIMEOUT_JIFFIES);

			if (ret < 0 ) {
				DBGPRINT(RT_DEBUG_OFF, ("#\n"));
				if (ret == -ENODEV) {
					RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST);
					break;
				}
				RetryCount++;
				mdelay(5); /* wait for 5ms*/
			}
		} while((ret < 0 ) && (RetryCount < MAX_VENDOR_REQ_RETRY_COUNT));

		if (ret >= 0 && size > 0 && requesttype == DEVICE_VENDOR_REQUEST_IN)
			memmove(data, pAd->vend_buf, size);

		up(&(pAd->UsbVendorReq_semaphore));

		if (ret < 0) {
			DBGPRINT(RT_DEBUG_ERROR, ("mt7610u_vendor_request failed(%d), ReqType=%s, Req=0x%x, Idx=0x%x,pAd->Flags=0x%lx\n",
						ret, (requesttype == DEVICE_VENDOR_REQUEST_OUT ? "OUT" : "IN"), request, index, pAd->Flags));

			if (request == MT_VEND_SINGLE_WRITE)
				DBGPRINT(RT_DEBUG_ERROR, ("\tRequest Value=0x%04x!\n", value));

			if (ret == -ENODEV)
					RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST);

		}

	}

	if (ret < 0)
		return NDIS_STATUS_FAILURE;
	else
		return NDIS_STATUS_SUCCESS;

}

int CheckGPIOHdlr(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
#ifdef CONFIG_STA_SUPPORT

		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			u32 data;
			/* Read GPIO pin2 as Hardware controlled radio state*/

			data = mt76u_reg_read( pAd, GPIO_CTRL_CFG);

			if (data & 0x04)
			{
				pAd->StaCfg.bHwRadio = true;
			}
			else
			{
				pAd->StaCfg.bHwRadio = false;
			}

			if (pAd->StaCfg.bRadio != (pAd->StaCfg.bHwRadio && pAd->StaCfg.bSwRadio))
			{
				pAd->StaCfg.bRadio = (pAd->StaCfg.bHwRadio && pAd->StaCfg.bSwRadio);
				if (pAd->StaCfg.bRadio == true)
				{
					DBGPRINT_RAW(RT_DEBUG_ERROR, ("!!! Radio On !!!\n"));

					MlmeRadioOn(pAd);
					/* Update extra information                                                                                                             */
					pAd->ExtraInfo = EXTRA_INFO_CLEAR;
				}
				else
				{
					DBGPRINT_RAW(RT_DEBUG_ERROR, ("!!! Radio Off !!!\n"));

					MlmeRadioOff(pAd);
					/* Update extra information                                                                                                             */
					pAd->ExtraInfo = HW_RADIO_OFF;
				}
			}
		} /* end IF_DEV_CONFIG_OPMODE_ON_STA*/

#endif /* CONFIG_STA_SUPPORT */

	return NDIS_STATUS_SUCCESS;
}


static int ResetBulkOutHdlr(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{

	INT32 MACValue = 0;
	u8 Index = 0;
	int ret=0;
	PHT_TX_CONTEXT	pHTTXContext;
/*	RTMP_TX_RING *pTxRing;*/
	unsigned long IrqFlags;

	DBGPRINT(RT_DEBUG_TRACE, ("CMDTHREAD_RESET_BULK_OUT(ResetPipeid=0x%0x)===>\n", pAd->bulkResetPipeid));

	/* All transfers must be aborted or cancelled before attempting to reset the pipe.						*/
	/*RTUSBCancelPendingBulkOutIRP(pAd);*/
	/* Wait 10ms to let previous packet that are already in HW FIFO to clear. by MAXLEE 12-25-2007*/
	do
	{
		if(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
			break;

		MACValue = mt76u_reg_read(pAd, TXRXQ_PCNT);
		if ((MACValue & 0xf00000/*0x800000*/) == 0)
			break;

		Index++;
		mdelay(10);
	}while(Index < 100);

	MACValue = mt76u_reg_read(pAd, USB_DMA_CFG);

	/* 2nd, to prevent Read Register error, we check the validity.*/
	if ((MACValue & 0xc00000) == 0)
		MACValue = mt76u_reg_read(pAd, USB_DMA_CFG);

	/* 3rd, to prevent Read Register error, we check the validity.*/
	if ((MACValue & 0xc00000) == 0)
		MACValue = mt76u_reg_read(pAd, USB_DMA_CFG);

	MACValue |= 0x80000;
	mt76u_reg_write(pAd, USB_DMA_CFG, MACValue);

	/* Wait 1ms to prevent next URB to bulkout before HW reset. by MAXLEE 12-25-2007*/
	mdelay(1);

	MACValue &= (~0x80000);
	mt76u_reg_write(pAd, USB_DMA_CFG, MACValue);
	DBGPRINT(RT_DEBUG_TRACE, ("\tSet 0x2a0 bit19. Clear USB DMA TX path\n"));

	/* Wait 5ms to prevent next URB to bulkout before HW reset. by MAXLEE 12-25-2007*/
	/*udelay(5000);*/

	if ((pAd->bulkResetPipeid & BULKOUT_MGMT_RESET_FLAG) == BULKOUT_MGMT_RESET_FLAG)
	{
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_BULKOUT_RESET);

		if (pAd->MgmtRing.TxSwFreeIdx < MGMT_RING_SIZE /* pMLMEContext->bWaitingBulkOut == true */)
			RTUSB_SET_BULK_FLAG(pAd, fRTUSB_BULK_OUT_MLME);

		RTUSBKickBulkOut(pAd);
		DBGPRINT(RT_DEBUG_TRACE, ("\tTX MGMT RECOVER Done!\n"));
	}
	else
	{
		pHTTXContext = &(pAd->TxContext[pAd->bulkResetPipeid]);

		/*spin_lock_bh(&pAd->BulkOutLock[pAd->bulkResetPipeid]);*/
		RTMP_INT_LOCK(&pAd->BulkOutLock[pAd->bulkResetPipeid], IrqFlags);
		if ( pAd->BulkOutPending[pAd->bulkResetPipeid] == false)
		{
			pAd->BulkOutPending[pAd->bulkResetPipeid] = true;
			pHTTXContext->IRPPending = true;
			pAd->watchDogTxPendingCnt[pAd->bulkResetPipeid] = 1;

			/* no matter what, clean the flag*/
			RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_BULKOUT_RESET);

			/*spin_unlock_bh(&pAd->BulkOutLock[pAd->bulkResetPipeid]);*/
			RTMP_INT_UNLOCK(&pAd->BulkOutLock[pAd->bulkResetPipeid], IrqFlags);

			{
				RTUSBInitHTTxDesc(pAd, pHTTXContext, pAd->bulkResetPipeid,
													pHTTXContext->BulkOutSize,
													RtmpUsbBulkOutDataPacketComplete);

				if ((ret = usb_submit_urb(pHTTXContext->pUrb, GFP_ATOMIC))!=0)
				{
						RTMP_INT_LOCK(&pAd->BulkOutLock[pAd->bulkResetPipeid], IrqFlags);
						pAd->BulkOutPending[pAd->bulkResetPipeid] = false;
						pHTTXContext->IRPPending = false;
						pAd->watchDogTxPendingCnt[pAd->bulkResetPipeid] = 0;
						RTMP_INT_UNLOCK(&pAd->BulkOutLock[pAd->bulkResetPipeid], IrqFlags);

						DBGPRINT(RT_DEBUG_ERROR, ("CMDTHREAD_RESET_BULK_OUT:Submit Tx URB failed %d\n", ret));
				}
				else
				{
						RTMP_INT_LOCK(&pAd->BulkOutLock[pAd->bulkResetPipeid], IrqFlags);

						DBGPRINT(RT_DEBUG_TRACE,("\tCMDTHREAD_RESET_BULK_OUT: TxContext[%d]:CWPos=%ld, NBPos=%ld, ENBPos=%ld, bCopy=%d, pending=%d!\n",
											pAd->bulkResetPipeid, pHTTXContext->CurWritePosition, pHTTXContext->NextBulkOutPosition,
											pHTTXContext->ENextBulkOutPosition, pHTTXContext->bCopySavePad,
											pAd->BulkOutPending[pAd->bulkResetPipeid]));
						DBGPRINT(RT_DEBUG_TRACE,("\t\tBulkOut Req=0x%lx, Complete=0x%lx, Other=0x%lx\n",
											pAd->BulkOutReq, pAd->BulkOutComplete, pAd->BulkOutCompleteOther));

						RTMP_INT_UNLOCK(&pAd->BulkOutLock[pAd->bulkResetPipeid], IrqFlags);

						DBGPRINT(RT_DEBUG_TRACE, ("\tCMDTHREAD_RESET_BULK_OUT: Submit Tx DATA URB for failed BulkReq(0x%lx) Done, status=%d!\n",
											pAd->bulkResetReq[pAd->bulkResetPipeid],
											pHTTXContext->pUrb->status));
				}
			}
		}
		else
		{
			/*spin_unlock_bh(&pAd->BulkOutLock[pAd->bulkResetPipeid]);*/
			/*RTMP_INT_UNLOCK(&pAd->BulkOutLock[pAd->bulkResetPipeid], IrqFlags);*/

			DBGPRINT(RT_DEBUG_ERROR, ("CmdThread : TX DATA RECOVER FAIL for BulkReq(0x%lx) because BulkOutPending[%d] is true!\n",
								pAd->bulkResetReq[pAd->bulkResetPipeid], pAd->bulkResetPipeid));

			if (pAd->bulkResetPipeid == 0)
			{
				u8 pendingContext = 0;
				PHT_TX_CONTEXT pHTTXContext = (PHT_TX_CONTEXT)(&pAd->TxContext[pAd->bulkResetPipeid ]);
				PTX_CONTEXT pMLMEContext = (PTX_CONTEXT)(pAd->MgmtRing.Cell[pAd->MgmtRing.TxDmaIdx].AllocVa);
				PTX_CONTEXT pNULLContext = (PTX_CONTEXT)(&pAd->PsPollContext);
				PTX_CONTEXT pPsPollContext = (PTX_CONTEXT)(&pAd->NullContext);

				if (pHTTXContext->IRPPending)
					pendingContext |= 1;
				else if (pMLMEContext->IRPPending)
					pendingContext |= 2;
				else if (pNULLContext->IRPPending)
					pendingContext |= 4;
				else if (pPsPollContext->IRPPending)
					pendingContext |= 8;
				else
					pendingContext = 0;

				DBGPRINT(RT_DEBUG_ERROR, ("\tTX Occupied by %d!\n", pendingContext));
			}

			/* no matter what, clean the flag*/
			RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_BULKOUT_RESET);

			RTMP_INT_UNLOCK(&pAd->BulkOutLock[pAd->bulkResetPipeid], IrqFlags);

			RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_NORMAL << pAd->bulkResetPipeid));
		}

		RTMPDeQueuePacket(pAd, false, NUM_OF_TX_RING, MAX_TX_PROCESS);
		/*RTUSBKickBulkOut(pAd);*/
	}

	DBGPRINT_RAW(RT_DEBUG_TRACE, ("CmdThread : CMDTHREAD_RESET_BULK_OUT<===\n"));
	return NDIS_STATUS_SUCCESS;


}


/* All transfers must be aborted or cancelled before attempting to reset the pipe.*/
static int ResetBulkInHdlr(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
	int ntStatus = NDIS_STATUS_SUCCESS;

	DBGPRINT_RAW(RT_DEBUG_TRACE, ("CmdThread : CMDTHREAD_RESET_BULK_IN === >\n"));

#ifdef CONFIG_STA_SUPPORT
	if(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF))
	{
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_BULKIN_RESET);
		return NDIS_STATUS_SUCCESS;
	}
#endif /* CONFIG_STA_SUPPORT */

	{
		/*while ((atomic_read(&pAd->PendingRx) > 0) && (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))) */
		if((pAd->PendingRx > 0) && (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
		{
			DBGPRINT_RAW(RT_DEBUG_ERROR, ("BulkIn IRP Pending!!!\n"));
			RTUSBCancelPendingBulkInIRP(pAd);
			mdelay(100);
			pAd->PendingRx = 0;
		}
	}

	/* Wait 10ms before reading register.*/
	mdelay(10);

	/* It must be removed. Or ATE will have no RX success. */
	if ((NT_SUCCESS(ntStatus) == true) &&
				(!(RTMP_TEST_FLAG(pAd, (fRTMP_ADAPTER_RESET_IN_PROGRESS | fRTMP_ADAPTER_RADIO_OFF |
												fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST)))))
	{
		u8 i;

		if (RTMP_TEST_FLAG(pAd, (fRTMP_ADAPTER_RESET_IN_PROGRESS | fRTMP_ADAPTER_RADIO_OFF |
									fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST)))
			return NDIS_STATUS_SUCCESS;

		pAd->NextRxBulkInPosition = pAd->RxContext[pAd->NextRxBulkInIndex].BulkInOffset;

		DBGPRINT(RT_DEBUG_TRACE, ("BULK_IN_RESET: NBIIdx=0x%x,NBIRIdx=0x%x, BIRPos=0x%lx. BIReq=x%lx, BIComplete=0x%lx, BICFail0x%lx\n",
					pAd->NextRxBulkInIndex,  pAd->NextRxBulkInReadIndex, pAd->NextRxBulkInPosition, pAd->BulkInReq, pAd->BulkInComplete, pAd->BulkInCompleteFail));

		for (i = 0; i < RX_RING_SIZE; i++)
		{
 			DBGPRINT(RT_DEBUG_TRACE, ("\tRxContext[%d]: IRPPending=%d, InUse=%d, Readable=%d!\n"
							, i, pAd->RxContext[i].IRPPending, pAd->RxContext[i].InUse, pAd->RxContext[i].Readable));
		}

		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_BULKIN_RESET);

		for (i = 0; i < pAd->CommonCfg.NumOfBulkInIRP; i++)
		{
			/*RTUSBBulkReceive(pAd);*/
			PRX_CONTEXT		pRxContext;
			struct urb *		pUrb;
			int				ret = 0;
			unsigned long	IrqFlags;

			spin_lock_bh(&pAd->BulkInLock);
			pRxContext = &(pAd->RxContext[pAd->NextRxBulkInIndex]);

			if ((pAd->PendingRx > 0) || (pRxContext->Readable == true) || (pRxContext->InUse == true))
			{
				spin_unlock_bh(&pAd->BulkInLock);
				return NDIS_STATUS_SUCCESS;
			}

			pRxContext->InUse = true;
			pRxContext->IRPPending = true;
			pAd->PendingRx++;
			pAd->BulkInReq++;
			spin_unlock_bh(&pAd->BulkInLock);

			/* Init Rx context descriptor*/
			RTUSBInitRxDesc(pAd, pRxContext);
			pUrb = pRxContext->pUrb;
			if ((ret = usb_submit_urb(pUrb, GFP_ATOMIC))!=0)
			{	/* fail*/
				spin_lock_bh(&pAd->BulkInLock);
				pRxContext->InUse = false;
				pRxContext->IRPPending = false;
				pAd->PendingRx--;
				pAd->BulkInReq--;
				spin_unlock_bh(&pAd->BulkInLock);
				DBGPRINT(RT_DEBUG_ERROR, ("CMDTHREAD_RESET_BULK_IN: Submit Rx URB failed(%d), status=%d\n", ret, pUrb->status));
			}
			else
			{	/* success*/
				/*DBGPRINT(RT_DEBUG_TRACE, ("BIDone, Pend=%d,BIIdx=%d,BIRIdx=%d!\n", */
				/*							pAd->PendingRx, pAd->NextRxBulkInIndex, pAd->NextRxBulkInReadIndex));*/
				DBGPRINT_RAW(RT_DEBUG_TRACE, ("CMDTHREAD_RESET_BULK_IN: Submit Rx URB Done, status=%d!\n", pUrb->status));
				ASSERT((pRxContext->InUse == pRxContext->IRPPending));
			}
		}

	}
	else
	{
		/* Card must be removed*/
		if (NT_SUCCESS(ntStatus) != true)
		{
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST);
			DBGPRINT_RAW(RT_DEBUG_ERROR, ("CMDTHREAD_RESET_BULK_IN: Read Register Failed!Card must be removed!!\n\n"));
		}
		else
			DBGPRINT_RAW(RT_DEBUG_ERROR, ("CMDTHREAD_RESET_BULK_IN: Cannot do bulk in because flags(0x%lx) on !\n", pAd->Flags));
	}

	DBGPRINT_RAW(RT_DEBUG_TRACE, ("CmdThread : CMDTHREAD_RESET_BULK_IN <===\n"));
	return NDIS_STATUS_SUCCESS;
}


static int SetAsicWcidHdlr(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
	RT_SET_ASIC_WCID	SetAsicWcid;
	USHORT		offset;
	u32		MACValue, MACRValue = 0;
	SetAsicWcid = *((PRT_SET_ASIC_WCID)(CMDQelmt->buffer));

	if (SetAsicWcid.WCID >= MAX_LEN_OF_MAC_TABLE)
		return NDIS_STATUS_FAILURE;

	offset = MAC_WCID_BASE + ((u8)SetAsicWcid.WCID)*HW_WCID_ENTRY_SIZE;

	DBGPRINT_RAW(RT_DEBUG_TRACE, ("CmdThread : CMDTHREAD_SET_ASIC_WCID : WCID = %ld, SetTid  = %lx, DeleteTid = %lx.\n",
						SetAsicWcid.WCID, SetAsicWcid.SetTid, SetAsicWcid.DeleteTid));

	MACValue = (pAd->MacTab.Content[SetAsicWcid.WCID].Addr[3]<<24)+(pAd->MacTab.Content[SetAsicWcid.WCID].Addr[2]<<16)+(pAd->MacTab.Content[SetAsicWcid.WCID].Addr[1]<<8)+(pAd->MacTab.Content[SetAsicWcid.WCID].Addr[0]);

	DBGPRINT_RAW(RT_DEBUG_TRACE, ("1-MACValue= %x,\n", MACValue));
	mt76u_reg_write(pAd, offset, MACValue);
	/* Read bitmask*/
	MACValue = mt76u_reg_read(pAd, offset+4);
	if ( SetAsicWcid.DeleteTid != 0xffffffff)
		MACRValue &= (~SetAsicWcid.DeleteTid);
	if (SetAsicWcid.SetTid != 0xffffffff)
		MACRValue |= (SetAsicWcid.SetTid);

	MACRValue &= 0xffff0000;
	MACValue = (pAd->MacTab.Content[SetAsicWcid.WCID].Addr[5]<<8)+pAd->MacTab.Content[SetAsicWcid.WCID].Addr[4];
	MACValue |= MACRValue;
	mt76u_reg_write(pAd, offset+4, MACValue);

	DBGPRINT_RAW(RT_DEBUG_TRACE, ("2-MACValue= %x,\n", MACValue));

	return NDIS_STATUS_SUCCESS;
}

static int DelAsicWcidHdlr(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
	RT_SET_ASIC_WCID SetAsicWcid;
	SetAsicWcid = *((PRT_SET_ASIC_WCID)(CMDQelmt->buffer));

	if (SetAsicWcid.WCID >= MAX_LEN_OF_MAC_TABLE)
		return NDIS_STATUS_FAILURE;

        AsicDelWcidTab(pAd, (u8)SetAsicWcid.WCID);

        return NDIS_STATUS_SUCCESS;
}

static int SetWcidSecInfoHdlr(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
	PRT_ASIC_WCID_SEC_INFO pInfo;

	pInfo = (PRT_ASIC_WCID_SEC_INFO)CMDQelmt->buffer;
	RTMPSetWcidSecurityInfo(pAd,
							 pInfo->BssIdx,
							 pInfo->KeyIdx,
							 pInfo->CipherAlg,
							 pInfo->Wcid,
							 pInfo->KeyTabFlag);

	return NDIS_STATUS_SUCCESS;
}


static int SetAsicWcidIVEIVHdlr(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
	PRT_ASIC_WCID_IVEIV_ENTRY pInfo;

	pInfo = (PRT_ASIC_WCID_IVEIV_ENTRY)CMDQelmt->buffer;
	AsicUpdateWCIDIVEIV(pAd,
						  pInfo->Wcid,
						  pInfo->Iv,
						  pInfo->Eiv);

	return NDIS_STATUS_SUCCESS;
}


static int SetAsicWcidAttrHdlr(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
	PRT_ASIC_WCID_ATTR_ENTRY pInfo;

	pInfo = (PRT_ASIC_WCID_ATTR_ENTRY)CMDQelmt->buffer;
	AsicUpdateWcidAttributeEntry(pAd,
								  pInfo->BssIdx,
								  pInfo->KeyIdx,
								  pInfo->CipherAlg,
								  pInfo->Wcid,
								  pInfo->KeyTabFlag);

	return NDIS_STATUS_SUCCESS;
}

static int SETAsicSharedKeyHdlr(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
	PRT_ASIC_SHARED_KEY pInfo;

	pInfo = (PRT_ASIC_SHARED_KEY)CMDQelmt->buffer;
	AsicAddSharedKeyEntry(pAd,
						       pInfo->BssIndex,
							pInfo->KeyIdx,
							&pInfo->CipherKey);

	return NDIS_STATUS_SUCCESS;
}

static int SetAsicPairwiseKeyHdlr(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
	PRT_ASIC_PAIRWISE_KEY pInfo;

	pInfo = (PRT_ASIC_PAIRWISE_KEY)CMDQelmt->buffer;
	AsicAddPairwiseKeyEntry(pAd,
							 pInfo->WCID,
							 &pInfo->CipherKey);

	return NDIS_STATUS_SUCCESS;
}

#ifdef CONFIG_STA_SUPPORT
static int SetPortSecuredHdlr(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
	STA_PORT_SECURED(pAd);
	return NDIS_STATUS_SUCCESS;
}
#endif /* CONFIG_STA_SUPPORT */


static int RemovePairwiseKeyHdlr(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
	u8 Wcid = *((u8 *)(CMDQelmt->buffer));

	AsicRemovePairwiseKeyEntry(pAd, Wcid);
	return NDIS_STATUS_SUCCESS;
}


static int SetClientMACEntryHdlr(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
	PRT_SET_ASIC_WCID pInfo;

	pInfo = (PRT_SET_ASIC_WCID)CMDQelmt->buffer;
	AsicUpdateRxWCIDTable(pAd, pInfo->WCID, pInfo->Addr);
	return NDIS_STATUS_SUCCESS;
}


static int UpdateProtectHdlr(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
	PRT_ASIC_PROTECT_INFO pAsicProtectInfo;

	pAsicProtectInfo = (PRT_ASIC_PROTECT_INFO)CMDQelmt->buffer;
	AsicUpdateProtect(pAd, pAsicProtectInfo->OperationMode, pAsicProtectInfo->SetMask,
							pAsicProtectInfo->bDisableBGProtect, pAsicProtectInfo->bNonGFExist);

	return NDIS_STATUS_SUCCESS;
}






#ifdef CONFIG_STA_SUPPORT
static int SetPSMBitHdlr(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		USHORT *pPsm = (USHORT *)CMDQelmt->buffer;
		MlmeSetPsmBit(pAd, *pPsm);
	}

	return NDIS_STATUS_SUCCESS;
}


static int ForceWakeUpHdlr(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		AsicForceWakeup(pAd, true);

	return NDIS_STATUS_SUCCESS;
}


static int ForceSleepAutoWakeupHdlr(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
	USHORT  TbttNumToNextWakeUp;
	USHORT  NextDtim = pAd->StaCfg.DtimPeriod;
	ULONG   Now;

	NdisGetSystemUpTime(&Now);
	NextDtim -= (USHORT)(Now - pAd->StaCfg.LastBeaconRxTime)/pAd->CommonCfg.BeaconPeriod;

	TbttNumToNextWakeUp = pAd->StaCfg.DefaultListenCount;
	if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_RECEIVE_DTIM) && (TbttNumToNextWakeUp > NextDtim))
		TbttNumToNextWakeUp = NextDtim;

	RTMP_SET_PSM_BIT(pAd, PWR_SAVE);

	/* if WMM-APSD is failed, try to disable following line*/
	AsicSleepThenAutoWakeup(pAd, TbttNumToNextWakeUp);

	return NDIS_STATUS_SUCCESS;
}


int QkeriodicExecutHdlr(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
	StaQuickResponeForRateUpExec(pAd);
	return NDIS_STATUS_SUCCESS;
}
#endif /* CONFIG_STA_SUPPORT*/




#ifdef LED_CONTROL_SUPPORT
static int SetLEDStatusHdlr(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
	u8 LEDStatus = *((u8 *)(CMDQelmt->buffer));

	RTMPSetLEDStatus(pAd, LEDStatus);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: CMDTHREAD_SET_LED_STATUS (LEDStatus = %d)\n",
								__FUNCTION__, LEDStatus));

	return NDIS_STATUS_SUCCESS;
}
#endif /* LED_CONTROL_SUPPORT */





#ifdef LINUX
#ifdef RT_CFG80211_SUPPORT
static int RegHintHdlr (IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
	RT_CFG80211_CRDA_REG_HINT(pAd, CMDQelmt->buffer, CMDQelmt->bufferlength);
	return NDIS_STATUS_SUCCESS;
}

static int RegHint11DHdlr(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
	RT_CFG80211_CRDA_REG_HINT11D(pAd, CMDQelmt->buffer, CMDQelmt->bufferlength);
	return NDIS_STATUS_SUCCESS;
}

static int RT_Mac80211_ScanEnd(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
	RT_CFG80211_SCAN_END(pAd, false);
	return NDIS_STATUS_SUCCESS;
}

static int RT_Mac80211_ConnResultInfom(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
	RT_CFG80211_CONN_RESULT_INFORM(pAd,
								pAd->MlmeAux.Bssid,
								CMDQelmt->buffer, CMDQelmt->bufferlength,
								CMDQelmt->buffer, CMDQelmt->bufferlength,
								1);
	return NDIS_STATUS_SUCCESS;
}
#endif /* RT_CFG80211_SUPPORT */
#endif /* LINUX */

static int CmdRspEventCallback(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt)
{
	struct mt7610u_rxfce_info_cmd *pFceInfo = CMDQelmt->buffer;

	return NDIS_STATUS_SUCCESS;
}


typedef int (*CMDHdlr)(IN struct rtmp_adapter *pAd, struct rtmp_queue_elem *CMDQelmt);

static CMDHdlr CMDHdlrTable[] = {
	ResetBulkOutHdlr,				/* CMDTHREAD_RESET_BULK_OUT*/
	ResetBulkInHdlr,					/* CMDTHREAD_RESET_BULK_IN*/
	CheckGPIOHdlr,					/* CMDTHREAD_CHECK_GPIO	*/
	SetAsicWcidHdlr,					/* CMDTHREAD_SET_ASIC_WCID*/
	DelAsicWcidHdlr,					/* CMDTHREAD_DEL_ASIC_WCID*/
	SetClientMACEntryHdlr,			/* CMDTHREAD_SET_CLIENT_MAC_ENTRY*/

#ifdef CONFIG_STA_SUPPORT
	SetPSMBitHdlr,					/* CMDTHREAD_SET_PSM_BIT*/
	ForceWakeUpHdlr,				/* CMDTHREAD_FORCE_WAKE_UP*/
	ForceSleepAutoWakeupHdlr,		/* CMDTHREAD_FORCE_SLEEP_AUTO_WAKEUP*/
	QkeriodicExecutHdlr,				/* CMDTHREAD_QKERIODIC_EXECUT*/
#else
	NULL,
	NULL,
	NULL,
	NULL,
#endif /* CONFIG_STA_SUPPORT */

	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,

#ifdef LED_CONTROL_SUPPORT
	SetLEDStatusHdlr,			/* CMDTHREAD_SET_LED_STATUS*/
#else
    NULL,
#endif /* LED_CONTROL_SUPPORT */

	NULL,

	/* Security related */
	SetWcidSecInfoHdlr,				/* CMDTHREAD_SET_WCID_SEC_INFO*/
	SetAsicWcidIVEIVHdlr,			/* CMDTHREAD_SET_ASIC_WCID_IVEIV*/
	SetAsicWcidAttrHdlr,				/* CMDTHREAD_SET_ASIC_WCID_ATTR*/
	SETAsicSharedKeyHdlr,			/* CMDTHREAD_SET_ASIC_SHARED_KEY*/
	SetAsicPairwiseKeyHdlr,			/* CMDTHREAD_SET_ASIC_PAIRWISE_KEY*/
	RemovePairwiseKeyHdlr,			/* CMDTHREAD_REMOVE_PAIRWISE_KEY*/

#ifdef CONFIG_STA_SUPPORT
	SetPortSecuredHdlr,				/* CMDTHREAD_SET_PORT_SECURED*/
#else
	NULL,
#endif /* CONFIG_STA_SUPPORT */

	NULL,

	UpdateProtectHdlr,				/* CMDTHREAD_UPDATE_PROTECT*/


#ifdef LINUX
#ifdef RT_CFG80211_SUPPORT
	RegHintHdlr,
	RegHint11DHdlr,
	RT_Mac80211_ScanEnd,
	RT_Mac80211_ConnResultInfom,
#else
	NULL,
	NULL,
	NULL,
	NULL,
#endif /* RT_CFG80211_SUPPORT */

#else
	NULL,
	NULL,
	NULL,
	NULL,
#endif /* LINUX */

	NULL,

	NULL,

	NULL,

	CmdRspEventCallback, /* CMDTHREAD_RESPONSE_EVENT_CALLBACK */
};


static inline bool ValidCMD(struct rtmp_queue_elem *CMDQelmt)
{
	SHORT CMDIndex = CMDQelmt->command - CMDTHREAD_FIRST_CMD_ID;
	USHORT CMDHdlrTableLength= sizeof(CMDHdlrTable) / sizeof(CMDHdlr);

	if ( (CMDIndex >= 0) && (CMDIndex < CMDHdlrTableLength))
	{
		if (CMDHdlrTable[CMDIndex] > 0)
			return true;
		else
		{
			DBGPRINT(RT_DEBUG_ERROR, ("No corresponding CMDHdlr for this CMD(%x)\n",  CMDQelmt->command));
			return false;
		}
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("CMD(%x) is out of boundary\n", CMDQelmt->command));
		return false;
	}
}


void CMDHandler(struct rtmp_adapter *pAd)
{
	struct rtmp_queue_elem *cmdqelmt;
	int		NdisStatus = NDIS_STATUS_SUCCESS;
	int		ntStatus;
/*	unsigned long	IrqFlags;*/

	while (pAd && pAd->CmdQ.size > 0) {
		NdisStatus = NDIS_STATUS_SUCCESS;

		spin_lock_bh(&pAd->CmdQLock);
		RTThreadDequeueCmd(&pAd->CmdQ, &cmdqelmt);
		spin_unlock_bh(&pAd->CmdQLock);

		if (cmdqelmt == NULL)
			break;


		if(!(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST) || RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS)))
		{
			if(ValidCMD(cmdqelmt))
				ntStatus = (*CMDHdlrTable[cmdqelmt->command - CMDTHREAD_FIRST_CMD_ID])(pAd, cmdqelmt);
		}

		if ((cmdqelmt->buffer != NULL) && (cmdqelmt->bufferlength != 0))
			kfree(cmdqelmt->buffer);
		kfree(cmdqelmt);
	}	/* end of while */
}

