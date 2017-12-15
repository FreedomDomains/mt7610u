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

/* Map USB endpoint number to Q id in the DMA engine */
static enum mt76_qsel ep2dmaq(u8 ep)
{
	if (ep == 5)
		return MT_QSEL_MGMT;
	return MT_QSEL_EDCA;
}



/*
	========================================================================

	Routine	Description:
		This subroutine will scan through releative ring descriptor to find
		out avaliable free ring descriptor and compare with request size.

	Arguments:
		pAd	Pointer	to our adapter
		RingType	Selected Ring

	Return Value:
		NDIS_STATUS_FAILURE		Not enough free descriptor
		NDIS_STATUS_SUCCESS		Enough free descriptor

	Note:

	========================================================================
*/
int	RTUSBFreeDescRequest(
	IN struct rtmp_adapter*pAd,
	IN u8 BulkOutPipeId,
	IN u32 req_cnt)
{
	int	 Status = NDIS_STATUS_FAILURE;
	unsigned long IrqFlags;
	HT_TX_CONTEXT *pHTTXContext;


	pHTTXContext = &pAd->TxContext[BulkOutPipeId];
	spin_lock_bh(&pAd->TxContextQueueLock[BulkOutPipeId]);
	if ((pHTTXContext->CurWritePosition < pHTTXContext->NextBulkOutPosition) && ((pHTTXContext->CurWritePosition + req_cnt + LOCAL_TXBUF_SIZE) > pHTTXContext->NextBulkOutPosition))
	{

		RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_NORMAL << BulkOutPipeId));
	}
	else if ((pHTTXContext->CurWritePosition == 8) && (pHTTXContext->NextBulkOutPosition < (req_cnt + LOCAL_TXBUF_SIZE)))
	{
		RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_NORMAL << BulkOutPipeId));
	}
	else if (pHTTXContext->bCurWriting == true)
	{
		DBGPRINT(RT_DEBUG_TRACE,("RTUSBFreeD c3 --> QueIdx=%d, CWPos=%ld, NBOutPos=%ld!\n", BulkOutPipeId, pHTTXContext->CurWritePosition, pHTTXContext->NextBulkOutPosition));
		RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_NORMAL << BulkOutPipeId));
	}
	else
	{
		Status = NDIS_STATUS_SUCCESS;
	}
	spin_unlock_bh(&pAd->TxContextQueueLock[BulkOutPipeId]);


	return Status;
}


bool RTUSBNeedQueueBackForAgg(struct rtmp_adapter*pAd, u8 BulkOutPipeId)
{
	HT_TX_CONTEXT *pHTTXContext;
	bool needQueBack = false;
	unsigned long   IrqFlags;


	pHTTXContext = &pAd->TxContext[BulkOutPipeId];

	spin_lock_bh(&pAd->TxContextQueueLock[BulkOutPipeId]);
	if ((pHTTXContext->IRPPending == true)  /*&& (pAd->TxSwQueue[BulkOutPipeId].Number == 0) */)
	{
		if ((pHTTXContext->CurWritePosition < pHTTXContext->ENextBulkOutPosition) &&
			(((pHTTXContext->ENextBulkOutPosition+MAX_AGGREGATION_SIZE) < MAX_TXBULK_LIMIT) || (pHTTXContext->CurWritePosition > MAX_AGGREGATION_SIZE)))
		{
			needQueBack = true;
		}
		else if ((pHTTXContext->CurWritePosition > pHTTXContext->ENextBulkOutPosition) &&
				 ((pHTTXContext->ENextBulkOutPosition + MAX_AGGREGATION_SIZE) < pHTTXContext->CurWritePosition))
		{
			needQueBack = true;
		}
	}
	spin_unlock_bh(&pAd->TxContextQueueLock[BulkOutPipeId]);

	return needQueBack;

}


/*
	========================================================================

	Routine	Description:
		Calculates the duration which is required to transmit out frames
	with given size and specified rate.

	Arguments:
		pTxD		Pointer to transmit descriptor
		Ack			Setting for Ack requirement bit
		Fragment	Setting for Fragment bit
		RetryMode	Setting for retry mode
		Ifs			Setting for IFS gap
		Rate		Setting for transmit rate
		Service		Setting for service
		Length		Frame length
		TxPreamble  Short or Long preamble when using CCK rates
		QueIdx - 0-3, according to 802.11e/d4.4 June/2003

	Return Value:
		None

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	========================================================================
*/
static void rlt_usb_write_txinfo(struct mt7610_txinfo_pkt *txinfo,
	USHORT USBDMApktLen, bool bWiv, u8 QueueSel)
{
	txinfo->pkt_80211 = 1;
	txinfo->info_type = 0;
	txinfo->d_port = 0;
	txinfo->cso = 0;
	txinfo->tso = 0;

	txinfo->pkt_len = USBDMApktLen;
	txinfo->QSEL = QueueSel;
	if (QueueSel != MT_QSEL_EDCA)
		DBGPRINT(RT_DEBUG_TRACE, ("====> QueueSel != FIFO_EDCA <====\n"));
	txinfo->next_vld = false; /*NextValid;   Need to check with Jan about this.*/
	txinfo->tx_burst = false;
	txinfo->wiv = bWiv;
	txinfo->sw_lst_rnd = 0;
}

#ifdef CONFIG_STA_SUPPORT
void ComposePsPoll(struct rtmp_adapter*pAd)
{
	struct mt7610_txinfo_pkt *txinfo;
	struct mt7610u_txwi *txwi;
	u8 TXWISize = sizeof(struct mt7610u_txwi);
	u8 *buf;
	USHORT data_len;


	DBGPRINT(RT_DEBUG_TRACE, ("ComposePsPoll\n"));
	memset(&pAd->PsPollFrame, 0, sizeof (PSPOLL_FRAME));

	pAd->PsPollFrame.FC.PwrMgmt = 0;
	pAd->PsPollFrame.FC.Type = BTYPE_CNTL;
	pAd->PsPollFrame.FC.SubType = SUBTYPE_PS_POLL;
	pAd->PsPollFrame.Aid = pAd->StaActive.Aid | 0xC000;
	memcpy(pAd->PsPollFrame.Bssid, pAd->CommonCfg.Bssid, ETH_ALEN);
	memcpy(pAd->PsPollFrame.Ta, pAd->CurrentAddress, ETH_ALEN);

	buf = &pAd->PsPollContext.TransferBuffer->field.WirelessPacket[0];

	txinfo = (struct mt7610_txinfo_pkt *)buf;
	txwi = (struct mt7610u_txwi *)&buf[MT_DMA_HDR_LEN];
	memset(buf, 0, 100);
	data_len = sizeof (PSPOLL_FRAME);
	rlt_usb_write_txinfo(txinfo, data_len + TXWISize, true,
						ep2dmaq(MGMTPIPEIDX));
	RTMPWriteTxWI(pAd, txwi, false, false, false, false, true, false, 0,
		      BSSID_WCID, data_len, 0, 0,
		      (u8) pAd->CommonCfg.MlmeTransmit.field.MCS,
		      IFS_BACKOFF, false, &pAd->CommonCfg.MlmeTransmit);
	memmove((void *)&buf[TXWISize + MT_DMA_HDR_LEN], (void *)&pAd->PsPollFrame, data_len);
	/* Append 4 extra zero bytes. */
	pAd->PsPollContext.BulkOutSize = MT_DMA_HDR_LEN + TXWISize + data_len + 4;
}
#endif /* CONFIG_STA_SUPPORT */


/* IRQL = DISPATCH_LEVEL */
void ComposeNullFrame(struct rtmp_adapter*pAd)
{
	struct mt7610_txinfo_pkt *txinfo;
	struct mt7610u_txwi *txwi;
	u8 *buf;
	u8 TXWISize = sizeof(struct mt7610u_txwi);
	USHORT data_len = sizeof(pAd->NullFrame);;


	memset(&pAd->NullFrame, 0, data_len);
	pAd->NullFrame.FC.Type = BTYPE_DATA;
	pAd->NullFrame.FC.SubType = SUBTYPE_NULL_FUNC;
	pAd->NullFrame.FC.ToDs = 1;
	memcpy(pAd->NullFrame.Addr1, pAd->CommonCfg.Bssid, ETH_ALEN);
	memcpy(pAd->NullFrame.Addr2, pAd->CurrentAddress, ETH_ALEN);
	memcpy(pAd->NullFrame.Addr3, pAd->CommonCfg.Bssid, ETH_ALEN);
	buf = &pAd->NullContext.TransferBuffer->field.WirelessPacket[0];
	memset(buf, 0, 100);
	txinfo = (struct mt7610_txinfo_pkt *)buf;
	txwi = (struct mt7610u_txwi *)&buf[MT_DMA_HDR_LEN];
	rlt_usb_write_txinfo(txinfo,
			(USHORT)(data_len + TXWISize), true,
			ep2dmaq(MGMTPIPEIDX));
	RTMPWriteTxWI(pAd, txwi, false, false, false, false, true, false, 0,
		      BSSID_WCID, data_len, 0, 0,
		      (u8)pAd->CommonCfg.MlmeTransmit.field.MCS,
		      IFS_BACKOFF, false, &pAd->CommonCfg.MlmeTransmit);
	memmove((void *)&buf[TXWISize + MT_DMA_HDR_LEN], (void *)&pAd->NullFrame, data_len);
	pAd->NullContext.BulkOutSize = MT_DMA_HDR_LEN + TXWISize + data_len + 4;
}


/*
	We can do copy the frame into pTxContext when match following conditions.
		=>
		=>
		=>
*/
static inline int RtmpUSBCanDoWrite(
	IN struct rtmp_adapter*pAd,
	IN u8 QueIdx,
	IN HT_TX_CONTEXT *pHTTXContext)
{
	int	canWrite = NDIS_STATUS_RESOURCES;


	if (((pHTTXContext->CurWritePosition) < pHTTXContext->NextBulkOutPosition) && (pHTTXContext->CurWritePosition + LOCAL_TXBUF_SIZE) > pHTTXContext->NextBulkOutPosition)
	{
		DBGPRINT(RT_DEBUG_ERROR,("RtmpUSBCanDoWrite c1!\n"));
		RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_NORMAL << QueIdx));
	}
	else if ((pHTTXContext->CurWritePosition == 8) && (pHTTXContext->NextBulkOutPosition < LOCAL_TXBUF_SIZE))
	{
		DBGPRINT(RT_DEBUG_ERROR,("RtmpUSBCanDoWrite c2!\n"));
		RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_NORMAL << QueIdx));
	}
	else if (pHTTXContext->bCurWriting == true)
	{
		DBGPRINT(RT_DEBUG_ERROR,("RtmpUSBCanDoWrite c3!\n"));
	}
	else if ((pHTTXContext->ENextBulkOutPosition == 8)  && ((pHTTXContext->CurWritePosition + 7912 ) > MAX_TXBULK_LIMIT)  )
	{
		DBGPRINT(RT_DEBUG_ERROR,("RtmpUSBCanDoWrite c4!\n"));
		RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_NORMAL << QueIdx));
	}

	else
	{
		canWrite = NDIS_STATUS_SUCCESS;
	}


	return canWrite;
}

USHORT	RtmpUSB_WriteFragTxResource(
	IN struct rtmp_adapter*pAd,
	IN TX_BLK *pTxBlk,
	IN u8 fragNum,
	OUT	USHORT *freeCnt)
{
	HT_TX_CONTEXT	*pHTTXContext;
	USHORT			hwHdrLen;	/* The hwHdrLen consist of 802.11 header length plus the header padding length.*/
	u32			fillOffset;
	struct mt7610_txinfo_pkt	*txinfo;
	struct mt7610u_txwi 	*txwi;
	u8 *		pWirelessPacket = NULL;
	u8 		QueIdx;
	int		Status;
	unsigned long	IrqFlags;
	u32			USBDMApktLen = 0, DMAHdrLen, padding;
	bool 		TxQLastRound = false;
	u8 TXWISize = sizeof(struct mt7610u_txwi);


	/* get Tx Ring Resource & Dma Buffer address*/

	QueIdx = pTxBlk->QueIdx;
	pHTTXContext  = &pAd->TxContext[QueIdx];

	spin_lock_bh(&pAd->TxContextQueueLock[QueIdx]);

	pHTTXContext  = &pAd->TxContext[QueIdx];
	fillOffset = pHTTXContext->CurWritePosition;

	if(fragNum == 0)
	{
		/* Check if we have enough space for this bulk-out batch.*/
		Status = RtmpUSBCanDoWrite(pAd, QueIdx, pHTTXContext);
		if (Status == NDIS_STATUS_SUCCESS)
		{
			pHTTXContext->bCurWriting = true;

			/* Reserve space for 8 bytes padding.*/
			if ((pHTTXContext->ENextBulkOutPosition == pHTTXContext->CurWritePosition))
			{
				pHTTXContext->ENextBulkOutPosition += 8;
				pHTTXContext->CurWritePosition += 8;
				fillOffset += 8;
			}
			pTxBlk->Priv = 0;
			pHTTXContext->CurWriteRealPos = pHTTXContext->CurWritePosition;
		}
		else
		{
			spin_unlock_bh(&pAd->TxContextQueueLock[QueIdx]);

			dev_kfree_skb_any(pTxBlk->pPacket);
			return(Status);
		}
	}
	else
	{
		/* For sub-sequent frames of this bulk-out batch. Just copy it to our bulk-out buffer.*/
		Status = ((pHTTXContext->bCurWriting == true) ? NDIS_STATUS_SUCCESS : NDIS_STATUS_FAILURE);
		if (Status == NDIS_STATUS_SUCCESS)
		{
			fillOffset += pTxBlk->Priv;
		}
		else
		{
			spin_unlock_bh(&pAd->TxContextQueueLock[QueIdx]);

			dev_kfree_skb_any(pTxBlk->pPacket);
			return(Status);
		}
	}

	memset((u8 *)(&pTxBlk->HeaderBuf[0]), 0, MT_DMA_HDR_LEN);
	txinfo = (struct mt7610_txinfo_pkt *)(&pTxBlk->HeaderBuf[0]);
	txwi= (struct mt7610u_txwi *)(&pTxBlk->HeaderBuf[MT_DMA_HDR_LEN]);

	pWirelessPacket = &pHTTXContext->TransferBuffer->field.WirelessPacket[fillOffset];

	/* copy TXWI + WLAN Header + LLC into DMA Header Buffer*/
	/*hwHdrLen = ROUND_UP(pTxBlk->MpduHeaderLen, 4);*/
	hwHdrLen = pTxBlk->MpduHeaderLen + pTxBlk->HdrPadLen;

	/* Build our URB for USBD*/
	DMAHdrLen = TXWISize + hwHdrLen;
	USBDMApktLen = DMAHdrLen + pTxBlk->SrcBufLen;
	padding = (4 - (USBDMApktLen % 4)) & 0x03;	/* round up to 4 byte alignment*/
	USBDMApktLen += padding;

	pTxBlk->Priv += (MT_DMA_HDR_LEN + USBDMApktLen);

	/* For TxInfo, the length of USBDMApktLen = TXWI_SIZE + 802.11 header + payload*/
	rlt_usb_write_txinfo(txinfo, (USHORT)(USBDMApktLen), false, MT_QSEL_EDCA);

	if (fragNum == pTxBlk->TotalFragNum)
	{
		txinfo->tx_burst = 0;

		if ((pHTTXContext->CurWritePosition + pTxBlk->Priv + 3906)> MAX_TXBULK_LIMIT)
		{
			txinfo->sw_lst_rnd = 1;
			TxQLastRound = true;
		}
	}
	else
	{
		txinfo->tx_burst = 1;
	}

	memmove(pWirelessPacket, pTxBlk->HeaderBuf, MT_DMA_HDR_LEN + TXWISize + hwHdrLen);
#ifdef RT_BIG_ENDIAN
	RTMPFrameEndianChange(pAd, (u8 *)(pWirelessPacket + MT_DMA_HDR_LEN + TXWISize), DIR_WRITE, false);
#endif /* RT_BIG_ENDIAN */
	pWirelessPacket += (MT_DMA_HDR_LEN + TXWISize + hwHdrLen);
	pHTTXContext->CurWriteRealPos += (MT_DMA_HDR_LEN + TXWISize + hwHdrLen);

	spin_unlock_bh(&pAd->TxContextQueueLock[QueIdx]);

	memmove(pWirelessPacket, pTxBlk->pSrcBufData, pTxBlk->SrcBufLen);

	/*	Zero the last padding.*/
	pWirelessPacket += pTxBlk->SrcBufLen;
	memset(pWirelessPacket, 0, padding + 8);

	if (fragNum == pTxBlk->TotalFragNum)
	{
		spin_lock_bh(&pAd->TxContextQueueLock[QueIdx]);

		/* Update the pHTTXContext->CurWritePosition. 3906 used to prevent the NextBulkOut is a A-RALINK/A-MSDU Frame.*/
		pHTTXContext->CurWritePosition += pTxBlk->Priv;
		if (TxQLastRound == true)
			pHTTXContext->CurWritePosition = 8;

		pHTTXContext->CurWriteRealPos = pHTTXContext->CurWritePosition;


		/* Finally, set bCurWriting as false*/
	pHTTXContext->bCurWriting = false;

		spin_unlock_bh(&pAd->TxContextQueueLock[QueIdx]);

		/* succeed and release the skb buffer*/
		dev_kfree_skb_any(pTxBlk->pPacket);
	}


	return(Status);

}


USHORT RtmpUSB_WriteSingleTxResource(
	IN struct rtmp_adapter*pAd,
	IN TX_BLK *pTxBlk,
	IN bool bIsLast,
	OUT	USHORT *freeCnt)
{
	HT_TX_CONTEXT *pHTTXContext;
	u32 fillOffset;
	struct mt7610_txinfo_pkt *txinfo;
	struct mt7610u_txwi *txwi;
	u8 *pWirelessPacket, *buf;
	u8 QueIdx;
	unsigned long	IrqFlags;
	int Status;
	u32 hdr_copy_len, hdr_len, dma_len = 0, padding;
	bool bTxQLastRound = false;
	u8 TXWISize = sizeof(struct mt7610u_txwi);


	/* get Tx Ring Resource & Dma Buffer address*/
	QueIdx = pTxBlk->QueIdx;

	spin_lock_bh(&pAd->TxContextQueueLock[QueIdx]);
	pHTTXContext  = &pAd->TxContext[QueIdx];
	fillOffset = pHTTXContext->CurWritePosition;



	/* Check ring full */
	Status = RtmpUSBCanDoWrite(pAd, QueIdx, pHTTXContext);
	if(Status == NDIS_STATUS_SUCCESS)
	{
		pHTTXContext->bCurWriting = true;
		buf = &pTxBlk->HeaderBuf[0];
		txinfo = (struct mt7610_txinfo_pkt *)buf;
		txwi= (struct mt7610u_txwi *)&buf[MT_DMA_HDR_LEN];

		/* Reserve space for 8 bytes padding.*/
		if ((pHTTXContext->ENextBulkOutPosition == pHTTXContext->CurWritePosition))
		{
			pHTTXContext->ENextBulkOutPosition += 8;
			pHTTXContext->CurWritePosition += 8;
			fillOffset += 8;
		}
		pHTTXContext->CurWriteRealPos = pHTTXContext->CurWritePosition;

		pWirelessPacket = &pHTTXContext->TransferBuffer->field.WirelessPacket[fillOffset];

		/* Build our URB for USBD */
		hdr_len = TXWISize + pTxBlk->MpduHeaderLen + pTxBlk->HdrPadLen;
		hdr_copy_len = MT_DMA_HDR_LEN + hdr_len;
		dma_len = hdr_len + pTxBlk->SrcBufLen;
		padding = (4 - (dma_len % 4)) & 0x03;	/* round up to 4 byte alignment*/
		dma_len += padding;

		pTxBlk->Priv = (MT_DMA_HDR_LEN + dma_len);

		/* For TxInfo, the length of USBDMApktLen = TXWI_SIZE + 802.11 header + payload */
		rlt_usb_write_txinfo(txinfo, (USHORT)(dma_len), false, MT_QSEL_EDCA);


		if ((pHTTXContext->CurWritePosition + 3906 + pTxBlk->Priv) > MAX_TXBULK_LIMIT)
		{
			txinfo->sw_lst_rnd = 1;
			bTxQLastRound = true;
		}

		memmove(pWirelessPacket, pTxBlk->HeaderBuf, hdr_copy_len);
#ifdef RT_BIG_ENDIAN
		RTMPFrameEndianChange(pAd, (u8 *)(pWirelessPacket + MT_DMA_HDR_LEN + TXWISize), DIR_WRITE, false);
#endif /* RT_BIG_ENDIAN */
		pWirelessPacket += (hdr_copy_len);

		/* We unlock it here to prevent the first 8 bytes maybe over-writed issue.*/
		/*	1. First we got CurWritePosition but the first 8 bytes still not write to the pTxcontext.*/
		/*	2. An interrupt break our routine and handle bulk-out complete.*/
		/*	3. In the bulk-out compllete, it need to do another bulk-out, */
		/*			if the ENextBulkOutPosition is just the same as CurWritePosition, it will save the first 8 bytes from CurWritePosition,*/
		/*			but the payload still not copyed. the pTxContext->SavedPad[] will save as allzero. and set the bCopyPad = true.*/
		/*	4. Interrupt complete.*/
		/*  5. Our interrupted routine go back and fill the first 8 bytes to pTxContext.*/
		/*	6. Next time when do bulk-out, it found the bCopyPad==true and will copy the SavedPad[] to pTxContext->NextBulkOutPosition.*/
		/*		and the packet will wrong.*/
		pHTTXContext->CurWriteRealPos += hdr_copy_len;
		spin_unlock_bh(&pAd->TxContextQueueLock[QueIdx]);

		{
			memmove(pWirelessPacket, pTxBlk->pSrcBufData, pTxBlk->SrcBufLen);
			pWirelessPacket += pTxBlk->SrcBufLen;
		}

		memset(pWirelessPacket, 0, padding + 8);
		spin_lock_bh(&pAd->TxContextQueueLock[QueIdx]);

		pHTTXContext->CurWritePosition += pTxBlk->Priv;
		if (bTxQLastRound)
			pHTTXContext->CurWritePosition = 8;

		pHTTXContext->CurWriteRealPos = pHTTXContext->CurWritePosition;
		pHTTXContext->bCurWriting = false;
	}


	spin_unlock_bh(&pAd->TxContextQueueLock[QueIdx]);


	/* succeed and release the skb buffer*/
	dev_kfree_skb_any(pTxBlk->pPacket);

	return(Status);

}


USHORT RtmpUSB_WriteMultiTxResource(
	IN struct rtmp_adapter*pAd,
	IN TX_BLK *pTxBlk,
	IN u8 frmNum,
	OUT USHORT *freeCnt)
{
	HT_TX_CONTEXT *pHTTXContext;
	USHORT hwHdrLen;	/* The hwHdrLen consist of 802.11 header length plus the header padding length.*/
	u32 fillOffset;
	struct mt7610_txinfo_pkt *txinfo;
	struct mt7610u_txwi *txwi;
	u8 *pWirelessPacket = NULL;
	u8 QueIdx;
	int Status;
	unsigned long IrqFlags;
	u8 TXWISize = sizeof(struct mt7610u_txwi);


	/* get Tx Ring Resource & Dma Buffer address*/
	QueIdx = pTxBlk->QueIdx;
	pHTTXContext  = &pAd->TxContext[QueIdx];

	spin_lock_bh(&pAd->TxContextQueueLock[QueIdx]);

	if(frmNum == 0)
	{
		/* Check if we have enough space for this bulk-out batch.*/
		Status = RtmpUSBCanDoWrite(pAd, QueIdx, pHTTXContext);
		if (Status == NDIS_STATUS_SUCCESS)
		{
			pHTTXContext->bCurWriting = true;

			txinfo = (struct mt7610_txinfo_pkt *)(&pTxBlk->HeaderBuf[0]);
			txwi= (struct mt7610u_txwi *)(&pTxBlk->HeaderBuf[MT_DMA_HDR_LEN]);

			/* Reserve space for 8 bytes padding.*/
			if ((pHTTXContext->ENextBulkOutPosition == pHTTXContext->CurWritePosition))
			{

				pHTTXContext->CurWritePosition += 8;
				pHTTXContext->ENextBulkOutPosition += 8;
			}
			fillOffset = pHTTXContext->CurWritePosition;
			pHTTXContext->CurWriteRealPos = pHTTXContext->CurWritePosition;

			pWirelessPacket = &pHTTXContext->TransferBuffer->field.WirelessPacket[fillOffset];


			/* Copy TXINFO + TXWI + WLAN Header + LLC into DMA Header Buffer*/

			if (pTxBlk->TxFrameType == TX_AMSDU_FRAME)
				/*hwHdrLen = ROUND_UP(pTxBlk->MpduHeaderLen-LENGTH_AMSDU_SUBFRAMEHEAD, 4)+LENGTH_AMSDU_SUBFRAMEHEAD;*/
				hwHdrLen = pTxBlk->MpduHeaderLen-LENGTH_AMSDU_SUBFRAMEHEAD + pTxBlk->HdrPadLen + LENGTH_AMSDU_SUBFRAMEHEAD;
			else if (pTxBlk->TxFrameType == TX_RALINK_FRAME)
				/*hwHdrLen = ROUND_UP(pTxBlk->MpduHeaderLen-LENGTH_ARALINK_HEADER_FIELD, 4)+LENGTH_ARALINK_HEADER_FIELD;*/
				hwHdrLen = pTxBlk->MpduHeaderLen-LENGTH_ARALINK_HEADER_FIELD + pTxBlk->HdrPadLen + LENGTH_ARALINK_HEADER_FIELD;
			else
				/*hwHdrLen = ROUND_UP(pTxBlk->MpduHeaderLen, 4);*/
				hwHdrLen = pTxBlk->MpduHeaderLen + pTxBlk->HdrPadLen;

			/* Update the pTxBlk->Priv.*/
			pTxBlk->Priv = MT_DMA_HDR_LEN + TXWISize + hwHdrLen;

			/*	txinfo->USBDMApktLen now just a temp value and will to correct latter.*/
			rlt_usb_write_txinfo(txinfo, (USHORT)(pTxBlk->Priv), false, MT_QSEL_EDCA);

			/* Copy it.*/
			memmove(pWirelessPacket, pTxBlk->HeaderBuf, pTxBlk->Priv);
#ifdef RT_BIG_ENDIAN
			RTMPFrameEndianChange(pAd, (u8 *)(pWirelessPacket+ MT_DMA_HDR_LEN + TXWISize), DIR_WRITE, false);
#endif /* RT_BIG_ENDIAN */
			pHTTXContext->CurWriteRealPos += pTxBlk->Priv;
			pWirelessPacket += pTxBlk->Priv;
		}
	}
	else
	{	/* For sub-sequent frames of this bulk-out batch. Just copy it to our bulk-out buffer.*/

		Status = ((pHTTXContext->bCurWriting == true) ? NDIS_STATUS_SUCCESS : NDIS_STATUS_FAILURE);
		if (Status == NDIS_STATUS_SUCCESS)
		{
			fillOffset =  (pHTTXContext->CurWritePosition + pTxBlk->Priv);
			pWirelessPacket = &pHTTXContext->TransferBuffer->field.WirelessPacket[fillOffset];

			/*hwHdrLen = pTxBlk->MpduHeaderLen;*/
			memmove(pWirelessPacket, pTxBlk->HeaderBuf, pTxBlk->MpduHeaderLen);
			pWirelessPacket += (pTxBlk->MpduHeaderLen);
			pTxBlk->Priv += pTxBlk->MpduHeaderLen;
		}
		else
		{	/* It should not happened now unless we are going to shutdown.*/
			DBGPRINT(RT_DEBUG_ERROR, ("WriteMultiTxResource():bCurWriting is false when handle sub-sequent frames.\n"));
			Status = NDIS_STATUS_FAILURE;
		}
	}


	/*
		We unlock it here to prevent the first 8 bytes maybe over-write issue.
		1. First we got CurWritePosition but the first 8 bytes still not write to the pTxContext.
		2. An interrupt break our routine and handle bulk-out complete.
		3. In the bulk-out compllete, it need to do another bulk-out,
			if the ENextBulkOutPosition is just the same as CurWritePosition, it will save the first 8 bytes from CurWritePosition,
			but the payload still not copyed. the pTxContext->SavedPad[] will save as allzero. and set the bCopyPad = true.
		4. Interrupt complete.
		5. Our interrupted routine go back and fill the first 8 bytes to pTxContext.
		6. Next time when do bulk-out, it found the bCopyPad==true and will copy the SavedPad[] to pTxContext->NextBulkOutPosition.
			and the packet will wrong.
	*/
	spin_unlock_bh(&pAd->TxContextQueueLock[QueIdx]);

	if (Status != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR,("WriteMultiTxResource: CWPos = %ld, NBOutPos = %ld.\n", pHTTXContext->CurWritePosition, pHTTXContext->NextBulkOutPosition));
		goto done;
	}

	/* Copy the frame content into DMA buffer and update the pTxBlk->Priv*/
	memmove(pWirelessPacket, pTxBlk->pSrcBufData, pTxBlk->SrcBufLen);
	pWirelessPacket += pTxBlk->SrcBufLen;
	pTxBlk->Priv += pTxBlk->SrcBufLen;

done:
	/* Release the skb buffer here*/
	dev_kfree_skb_any(pTxBlk->pPacket);

	return(Status);

}


void RtmpUSB_FinalWriteTxResource(
	IN struct rtmp_adapter*pAd,
	IN TX_BLK *pTxBlk,
	IN USHORT totalMPDUSize,
	IN USHORT TxIdx)
{
	u8 		QueIdx;
	HT_TX_CONTEXT	*pHTTXContext;
	u32			fillOffset;
	struct mt7610_txinfo_pkt	*txinfo;
	struct mt7610u_txwi 	*txwi;
	u32			USBDMApktLen, padding;
	unsigned long	IrqFlags;
	u8 *		pWirelessPacket;

	QueIdx = pTxBlk->QueIdx;
	pHTTXContext  = &pAd->TxContext[QueIdx];

	spin_lock_bh(&pAd->TxContextQueueLock[QueIdx]);

	if (pHTTXContext->bCurWriting == true)
	{
		fillOffset = pHTTXContext->CurWritePosition;
		if (((pHTTXContext->ENextBulkOutPosition == pHTTXContext->CurWritePosition) || ((pHTTXContext->ENextBulkOutPosition-8) == pHTTXContext->CurWritePosition))
			&& (pHTTXContext->bCopySavePad == true))
			pWirelessPacket = (u8 *)(&pHTTXContext->SavedPad[0]);
		else
			pWirelessPacket = (u8 *)(&pHTTXContext->TransferBuffer->field.WirelessPacket[fillOffset]);


		/* Update TxInfo->USBDMApktLen , */
		/*		the length = TXWI_SIZE + 802.11_hdr + 802.11_hdr_pad + payload_of_all_batch_frames + Bulk-Out-padding*/

		txinfo = (struct mt7610_txinfo_pkt *)(pWirelessPacket);

		/* Calculate the bulk-out padding*/
		USBDMApktLen = pTxBlk->Priv - MT_DMA_HDR_LEN;
		padding = (4 - (USBDMApktLen % 4)) & 0x03;	/* round up to 4 byte alignment*/
		USBDMApktLen += padding;

		txinfo->pkt_len = USBDMApktLen;


		/*
			Update TXWI->MPDUtotalByteCnt,
				the length = 802.11 header + payload_of_all_batch_frames
		*/
		txwi= (struct mt7610u_txwi *)(pWirelessPacket + MT_DMA_HDR_LEN);
		txwi->MPDUtotalByteCnt = totalMPDUSize;


		/* Update the pHTTXContext->CurWritePosition*/

		pHTTXContext->CurWritePosition += (MT_DMA_HDR_LEN + USBDMApktLen);
		if ((pHTTXContext->CurWritePosition + 3906)> MAX_TXBULK_LIMIT)
		{	/* Add 3906 for prevent the NextBulkOut packet size is a A-RALINK/A-MSDU Frame.*/
			pHTTXContext->CurWritePosition = 8;
			txinfo->sw_lst_rnd = 1;
		}

		pHTTXContext->CurWriteRealPos = pHTTXContext->CurWritePosition;



		/*	Zero the last padding.*/
		pWirelessPacket = (&pHTTXContext->TransferBuffer->field.WirelessPacket[fillOffset + pTxBlk->Priv]);
		memset(pWirelessPacket, 0, padding + 8);

		/* Finally, set bCurWriting as false*/
		pHTTXContext->bCurWriting = false;

	}
	else
	{	/* It should not happened now unless we are going to shutdown.*/
		DBGPRINT(RT_DEBUG_ERROR, ("FinalWriteTxResource():bCurWriting is false when handle last frames.\n"));
	}

	spin_unlock_bh(&pAd->TxContextQueueLock[QueIdx]);

}


void RtmpUSBDataLastTxIdx(
	IN struct rtmp_adapter*pAd,
	IN u8 QueIdx,
	IN USHORT TxIdx)
{
	/* DO nothing for USB.*/
}


/*
	When can do bulk-out:
		1. TxSwFreeIdx < TX_RING_SIZE;
			It means has at least one Ring entity is ready for bulk-out, kick it out.
		2. If TxSwFreeIdx == TX_RING_SIZE
			Check if the CurWriting flag is false, if it's false, we can do kick out.

*/
void RtmpUSBDataKickOut(
	IN struct rtmp_adapter*pAd,
	IN TX_BLK *pTxBlk,
	IN u8 QueIdx)
{
	RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_NORMAL << QueIdx));
	RTUSBKickBulkOut(pAd);

}


/*
	Must be run in Interrupt context
	This function handle RT2870 specific TxDesc and cpu index update and kick the packet out.
 */
int RtmpUSBMgmtKickOut(
	IN struct rtmp_adapter*pAd,
	IN u8 QueIdx,
	IN struct sk_buff * pPacket,
	IN u8 *pSrcBufVA,
	IN UINT SrcBufLen)
{
	struct mt7610_txinfo_pkt *txinfo;
	ULONG BulkOutSize;
	u8 padLen;
	u8 *pDest;
	ULONG SwIdx = pAd->MgmtRing.TxCpuIdx;
	TX_CONTEXT *pMLMEContext = (PTX_CONTEXT)pAd->MgmtRing.Cell[SwIdx].AllocVa;
	ULONG IrqFlags;


	txinfo = (struct mt7610_txinfo_pkt *)(pSrcBufVA);

	/* Build our URB for USBD*/
	BulkOutSize = (SrcBufLen + 3) & (~3);
	rlt_usb_write_txinfo(txinfo, (USHORT)(BulkOutSize - MT_DMA_HDR_LEN), true, ep2dmaq(MGMTPIPEIDX));

	BulkOutSize += 4; /* Always add 4 extra bytes at every packet.*/

//---Add by shiang for debug

/* WY , it cause Tx hang on Amazon_SE , Max said the padding is useless*/
	/* If BulkOutSize is multiple of BulkOutMaxPacketSize, add extra 4 bytes again.*/
/*	if ((BulkOutSize % pAd->BulkOutMaxPacketSize) == 0)*/
/*		BulkOutSize += 4;*/

	padLen = BulkOutSize - SrcBufLen;
	ASSERT((padLen <= RTMP_PKT_TAIL_PADDING));

	/* Now memzero all extra padding bytes.*/
	pDest = (u8 *)(pSrcBufVA + SrcBufLen);
	skb_put(pPacket, padLen);
	memset(pDest, 0, padLen);

	spin_lock_bh(&pAd->MLMEBulkOutLock);

	pAd->MgmtRing.Cell[pAd->MgmtRing.TxCpuIdx].pNdisPacket = pPacket;
	pMLMEContext->TransferBuffer = (TX_BUFFER *) pPacket->data;

	/* Length in TxInfo should be 8 less than bulkout size.*/
	pMLMEContext->BulkOutSize = BulkOutSize;
	pMLMEContext->InUse = true;
	pMLMEContext->bWaitingBulkOut = true;


/*
	pAd->RalinkCounters.KickTxCount++;
	pAd->RalinkCounters.OneSecTxDoneCount++;

	if (pAd->MgmtRing.TxSwFreeIdx == MGMT_RING_SIZE)
		needKickOut = true;
*/

	/* Decrease the TxSwFreeIdx and Increase the TX_CTX_IDX*/
	pAd->MgmtRing.TxSwFreeIdx--;
	INC_RING_INDEX(pAd->MgmtRing.TxCpuIdx, MGMT_RING_SIZE);

	spin_unlock_bh(&pAd->MLMEBulkOutLock);

	RTUSB_SET_BULK_FLAG(pAd, fRTUSB_BULK_OUT_MLME);
	/*if (needKickOut)*/
	RTUSBKickBulkOut(pAd);

	return 0;
}


void RtmpUSBNullFrameKickOut(
	IN struct rtmp_adapter*pAd,
	IN u8 QueIdx,
	IN u8 *pNullFrame,
	IN u32 frameLen)
{
	if (pAd->NullContext.InUse == false)
	{
		PTX_CONTEXT pNullContext;
		struct mt7610_txinfo_pkt *txinfo;
		struct mt7610u_txwi *txwi;
		u8 *pWirelessPkt;
		u8 TXWISize = sizeof(struct mt7610u_txwi);

		pNullContext = &(pAd->NullContext);

		/* Set the in use bit*/
		pNullContext->InUse = true;
		pWirelessPkt = (u8 *)&pNullContext->TransferBuffer->field.WirelessPacket[0];

		memset(&pWirelessPkt[0], 0, 100);
		txinfo = (struct mt7610_txinfo_pkt *)&pWirelessPkt[0];
		rlt_usb_write_txinfo(txinfo, (USHORT)(frameLen + TXWISize), true, ep2dmaq(MGMTPIPEIDX));
		txinfo->QSEL = MT_QSEL_EDCA;
		txwi = (struct mt7610u_txwi *)&pWirelessPkt[MT_DMA_HDR_LEN];
		RTMPWriteTxWI(pAd, txwi,  false, false, false, false, true, false, 0, BSSID_WCID, frameLen,
			0, 0, (u8)pAd->CommonCfg.MlmeTransmit.field.MCS, IFS_HTTXOP, false, &pAd->CommonCfg.MlmeTransmit);
#ifdef RT_BIG_ENDIAN
		RTMPWIEndianChange(txwi, sizeof(*txwi));
#endif /* RT_BIG_ENDIAN */
		memmove(&pWirelessPkt[TXWISize + MT_DMA_HDR_LEN], pNullFrame, frameLen);
#ifdef RT_BIG_ENDIAN
		RTMPFrameEndianChange(pAd, (u8 *)&pWirelessPkt[MT_DMA_HDR_LEN + TXWISize], DIR_WRITE, false);
#endif /* RT_BIG_ENDIAN */
		pAd->NullContext.BulkOutSize =  MT_DMA_HDR_LEN + TXWISize + frameLen + 4;
		pAd->NullContext.BulkOutSize = ( pAd->NullContext.BulkOutSize + 3) & (~3);

		/* Fill out frame length information for global Bulk out arbitor*/
		/*pNullContext->BulkOutSize = TransferBufferLength;*/
		DBGPRINT(RT_DEBUG_TRACE, ("%s - Send NULL Frame @%d Mbps...\n", __FUNCTION__, RateIdToMbps[pAd->CommonCfg.TxRate]));
		RTUSB_SET_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_NULL);

		pAd->Sequence = (pAd->Sequence+1) & MAXSEQ;

		/* Kick bulk out */
		RTUSBKickBulkOut(pAd);
	}

}


/*
========================================================================
Routine Description:
    Get a received packet.

Arguments:
	pAd					device control block
	pSaveRxD			receive descriptor information
	*pbReschedule		need reschedule flag
	*pRxPending			pending received packet flag

Return Value:
    the recieved packet

Note:
========================================================================
*/
struct sk_buff *GetPacketFromRxRing(
	IN struct rtmp_adapter*pAd,
	OUT RX_BLK *pRxBlk,
	OUT bool *pbReschedule,
	INOUT u32 *pRxPending,
	OUT bool *bCmdRspPacket)
{
	RX_CONTEXT *pRxContext;
	struct sk_buff *skb;
	u8 *pData, *RXDMA;
	ULONG ThisFrameLen, RxBufferLength, valid_len;
	struct mt7610u_rxwi *pRxWI;
	u8 RXWISize = sizeof(struct mt7610u_rxwi);
	struct mt7610u_rxinfo *pRxInfo;
	struct mt7610u_rxfce_info_pkt *pRxFceInfo;

	*bCmdRspPacket = false;

	pRxContext = &pAd->RxContext[pAd->NextRxBulkInReadIndex];
	if ((pRxContext->Readable == false) || (pRxContext->InUse == true))
		return NULL;

	RxBufferLength = pRxContext->BulkInOffset - pAd->ReadPosition;
	valid_len = RXDMA_FIELD_SIZE * 2;

	if (RxBufferLength < valid_len)
		return NULL;

	pData = &pRxContext->TransferBuffer[pAd->ReadPosition];

	RXDMA = pData;
	/* The RXDMA field is 4 bytes, now just use the first 2 bytes. The Length including the (RXWI + MSDU + Padding) */
	ThisFrameLen = *pData + (*(pData+1)<<8);

	if (ThisFrameLen == 0) {
		DBGPRINT(RT_DEBUG_TRACE, ("BIRIdx(%d): RXDMALen is zero.[%ld], BulkInBufLen = %ld)\n",
								pAd->NextRxBulkInReadIndex, ThisFrameLen, pRxContext->BulkInOffset));
		return NULL;
	}

	if ((ThisFrameLen & 0x3) != 0) {
		DBGPRINT(RT_DEBUG_ERROR, ("BIRIdx(%d): RXDMALen not multiple of 4.[%ld], BulkInBufLen = %ld)\n",
								pAd->NextRxBulkInReadIndex, ThisFrameLen, pRxContext->BulkInOffset));
		return NULL;
	}

	if ((ThisFrameLen + 8) > RxBufferLength) {	/* 8 for (RXDMA_FIELD_SIZE + sizeof(struct mt7610u_rxinfo))*/
		DBGPRINT(RT_DEBUG_ERROR,("BIRIdx(%d):FrameLen(0x%lx) outranges. BulkInLen=0x%lx, remaining RxBufLen=0x%lx, ReadPos=0x%lx\n",
						pAd->NextRxBulkInReadIndex, ThisFrameLen, pRxContext->BulkInOffset, RxBufferLength, pAd->ReadPosition));

		/* error frame. finish this loop*/
		return NULL;
	}

	/* skip USB frame length field*/
	pData += RXDMA_FIELD_SIZE;

	pRxFceInfo = (struct mt7610u_rxfce_info_pkt *)(pData + ThisFrameLen);

	pRxInfo = (struct mt7610u_rxinfo *)pData;

	pData += RXINFO_SIZE;

	pRxWI = (struct mt7610u_rxwi *)pData;

#ifdef RT_BIG_ENDIAN
	RTMPWIEndianChange(pData, sizeof(struct rxwi_nmac));
#endif /* RT_BIG_ENDIAN */
	if (pRxWI->MPDUtotalByteCnt > ThisFrameLen) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s():pRxWIMPDUtotalByteCount(%d) large than RxDMALen(%ld)\n",
									__FUNCTION__, pRxWI->MPDUtotalByteCnt, ThisFrameLen));
		return NULL;
	}
#ifdef RT_BIG_ENDIAN
	RTMPWIEndianChange(pData, sizeof(struct rxwi_nmac));
#endif /* RT_BIG_ENDIAN */

	/* allocate a rx packet*/
	skb = dev_alloc_skb(ThisFrameLen);
	if (skb == NULL) {
		DBGPRINT(RT_DEBUG_ERROR,("%s():Cannot Allocate sk buffer for this Bulk-In buffer!\n", __FUNCTION__));
		return NULL;
	}

	/* copy the rx packet*/
	/* ULLI memcpy a whole packet very is fast or better slow  ... */
	memcpy(skb_put(skb, ThisFrameLen), pData, ThisFrameLen);
	skb->dev = get_netdev_from_bssid(pAd, BSS0);

#ifdef RT_BIG_ENDIAN
	RTMPDescriptorEndianChange((u8 *)pRxInfo, TYPE_RXINFO);
#endif /* RT_BIG_ENDIAN */

	memmove((void *)&pRxBlk->hw_rx_info[0], (void *)pRxFceInfo, sizeof(struct mt7610u_rxfce_info_pkt));
	pRxBlk->pRxFceInfo = (struct mt7610u_rxfce_info_pkt *)&pRxBlk->hw_rx_info[0];

	memmove(&pRxBlk->hw_rx_info[RXINFO_OFFSET], pRxInfo, RXINFO_SIZE);
	pRxBlk->pRxInfo = (struct mt7610u_rxinfo *)&pRxBlk->hw_rx_info[RXINFO_OFFSET];


	/* update next packet read position.*/
	pAd->ReadPosition += (ThisFrameLen + RXDMA_FIELD_SIZE + RXINFO_SIZE);	/* 8 for (RXDMA_FIELD_SIZE + sizeof(struct mt7610u_rxinfo))*/

	return skb;
}


#ifdef CONFIG_STA_SUPPORT
/*
	========================================================================

	Routine	Description:
		Check Rx descriptor, return NDIS_STATUS_FAILURE if any error dound

	Arguments:
		pRxD		Pointer	to the Rx descriptor

	Return Value:
		NDIS_STATUS_SUCCESS		No err
		NDIS_STATUS_FAILURE		Error

	Note:

	========================================================================
*/
int	RTMPCheckRxError(
	IN struct rtmp_adapter*pAd,
	IN PHEADER_802_11 pHeader,
	IN struct mt7610u_rxwi *pRxWI,
	IN struct mt7610u_rxinfo *pRxInfo)
{
	PCIPHER_KEY pWpaKey;
	INT dBm;

	if(pRxInfo == NULL)
		return(NDIS_STATUS_FAILURE);

	/* Phy errors & CRC errors*/
	if (pRxInfo->Crc)
	{
		/* Check RSSI for Noise Hist statistic collection.*/
		dBm = (INT) (pRxWI->rssi[0]) - pAd->BbpRssiToDbmDelta;
		if (dBm <= -87)
			pAd->StaCfg.RPIDensity[0] += 1;
		else if (dBm <= -82)
			pAd->StaCfg.RPIDensity[1] += 1;
		else if (dBm <= -77)
			pAd->StaCfg.RPIDensity[2] += 1;
		else if (dBm <= -72)
			pAd->StaCfg.RPIDensity[3] += 1;
		else if (dBm <= -67)
			pAd->StaCfg.RPIDensity[4] += 1;
		else if (dBm <= -62)
			pAd->StaCfg.RPIDensity[5] += 1;
		else if (dBm <= -57)
			pAd->StaCfg.RPIDensity[6] += 1;
		else if (dBm > -57)
			pAd->StaCfg.RPIDensity[7] += 1;

		return(NDIS_STATUS_FAILURE);
	}

	/* Add Rx size to channel load counter, we should ignore error counts*/
	pAd->StaCfg.CLBusyBytes += (pRxWI->MPDUtotalByteCnt + 14);

#ifndef CLIENT_WDS
	if (pHeader->FC.ToDs
		)
	{
		DBGPRINT_RAW(RT_DEBUG_ERROR, ("Err;FC.ToDs\n"));
		return NDIS_STATUS_FAILURE;
	}
#endif /* CLIENT_WDS */

	/* Paul 04-03 for OFDM Rx length issue*/
	if (pRxWI->MPDUtotalByteCnt > MAX_AGGREGATION_SIZE)
	{
		DBGPRINT_RAW(RT_DEBUG_ERROR, ("received packet too long\n"));
		return NDIS_STATUS_FAILURE;
	}

	/* Drop not U2M frames, cant's drop here because we will drop beacon in this case*/
	/* I am kind of doubting the U2M bit operation*/
	/* if (pRxD->U2M == 0)*/
	/*	return(NDIS_STATUS_FAILURE);*/

	/* drop decyption fail frame*/
	if (pRxInfo->Decrypted && pRxInfo->CipherErr)
	{
		/* MIC Error*/
		if ((pRxInfo->CipherErr == 2) && pRxInfo->MyBss)
		{
			pWpaKey = &pAd->SharedKey[BSS0][pRxWI->key_idx];
#ifdef WPA_SUPPLICANT_SUPPORT
            if (pAd->StaCfg.WpaSupplicantUP)
                WpaSendMicFailureToWpaSupplicant(pAd->net_dev,
                                   (pWpaKey->Type == PAIRWISEKEY) ? true:false);
            else
#endif /* WPA_SUPPLICANT_SUPPORT */
			RTMPReportMicError(pAd, pWpaKey);
			DBGPRINT_RAW(RT_DEBUG_ERROR,("Rx MIC Value error\n"));
		}

		if (pRxInfo->Decrypted &&
			(pAd->SharedKey[BSS0][pRxWI->key_idx].CipherAlg == CIPHER_AES) &&
			(pHeader->Sequence == pAd->FragFrame.Sequence))
		{

			/* Acceptable since the First FragFrame no CipherErr problem.*/
			return(NDIS_STATUS_SUCCESS);
		}

		return(NDIS_STATUS_FAILURE);
	}

	return(NDIS_STATUS_SUCCESS);
}


void RtmpUsbStaAsicForceWakeupTimeout(void *FunctionContext)
{
	struct rtmp_adapter*pAd = (struct rtmp_adapter*)FunctionContext;



	if (pAd && pAd->Mlme.AutoWakeupTimerRunning)
	{
		RTUSBBulkReceive(pAd);

		OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_DOZE);
		pAd->Mlme.AutoWakeupTimerRunning = false;
	}
}


void RT28xxUsbStaAsicForceWakeup(
	IN struct rtmp_adapter *pAd,
	IN bool       bFromTx)
{
	bool Canceled;

	if (pAd->Mlme.AutoWakeupTimerRunning)
	{
		RTMPCancelTimer(&pAd->Mlme.AutoWakeupTimer, &Canceled);
		pAd->Mlme.AutoWakeupTimerRunning = false;
	}

	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_DOZE);
}


void RT28xxUsbStaAsicSleepThenAutoWakeup(
	IN struct rtmp_adapter *pAd,
	IN USHORT TbttNumToNextWakeUp)
{


	/* Not going to sleep if in the Count Down Time*/
	if (pAd->CountDowntoPsm > 0)
		return;


	/* we have decided to SLEEP, so at least do it for a BEACON period.*/
	if (TbttNumToNextWakeUp == 0)
		TbttNumToNextWakeUp = 1;

	RTMPSetTimer(&pAd->Mlme.AutoWakeupTimer, AUTO_WAKEUP_TIMEOUT);
	pAd->Mlme.AutoWakeupTimerRunning = true;

	/* cancel bulk-in IRPs prevent blocking CPU enter C3.*/
	if((pAd->PendingRx > 0) && (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
	{
		RTUSBCancelPendingBulkInIRP(pAd);
		/* resend bulk-in IRPs to receive beacons after a period of (pAd->CommonCfg.BeaconPeriod - 40) ms*/
		pAd->PendingRx = 0;
	}


	OPSTATUS_SET_FLAG(pAd, fOP_STATUS_DOZE);

}
#endif /* CONFIG_STA_SUPPORT */


