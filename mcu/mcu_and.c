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
#include <linux/firmware.h>


#ifdef RTMP_MAC_USB
void usb_uploadfw_complete(struct urb *urb)
{
	RTMP_OS_COMPLETION *load_fw_done = urb->context;

	RTMP_OS_COMPLETE(load_fw_done);
}

static int usb_load_ivb(struct rtmp_adapter*ad, u8 *fw_image)
{
	int Status = NDIS_STATUS_SUCCESS;
	struct rtmp_chip_cap *cap = &ad->chipCap;


	if (cap->load_iv) {
		Status = RTUSB_VendorRequest(ad,
				 DEVICE_VENDOR_REQUEST_OUT,
				 0x01,
				 0x12,
				 0x00,
				 fw_image + 32,
				 64);
	} else {
		Status = RTUSB_VendorRequest(ad,
				 DEVICE_VENDOR_REQUEST_OUT,
				 0x01,
				 0x12,
				 0x00,
				 NULL,
				 0x00);
	}

	if (Status) {
			DBGPRINT(RT_DEBUG_ERROR, ("Upload IVB Fail\n"));
			return Status;
	}

	return Status;
}

int mt7610u_mcu_usb_loadfw(struct rtmp_adapter*ad)
{
	const struct firmware *fw;
	struct usb_device *udev = ad->OS_Cookie->pUsb_Dev;
	struct urb *urb;
	dma_addr_t fw_dma;
	u8 *fw_data;
	struct txinfo_nmac_cmd *tx_info;
	s32 sent_len;
	u32 cur_len = 0;
	u32 mac_value, loop = 0;
	u16 value;
	int ret = 0;
	struct rtmp_chip_cap *cap = &ad->chipCap;
	USB_DMA_CFG_STRUC cfg;
	u32 ilm_len = 0, dlm_len = 0;
	u16 fw_ver, build_ver;
	RTMP_OS_COMPLETION load_fw_done;
	USB_DMA_CFG_STRUC UsbCfg;
	u8 *fw_image = NULL;

	dev_info(&udev->dev, "loading firmware %s\n", cap->fw_name);

	ret = request_firmware(&fw, cap->fw_name, &udev->dev);
	if (ret) {
		dev_info(&udev->dev, "loading failed %s\n", cap->fw_name);
		return ret;
	}

	RtmpOsMsDelay(5);

	fw_image = (u8 *) fw->data;

	if (!fw_image) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s:Please assign a fw image\n", __FUNCTION__));
		return NDIS_STATUS_FAILURE;
	}

	dev_info(&udev->dev, "firmware %s loaded\n", cap->fw_name);

	if (cap->IsComboChip) {
loadfw_protect:
		mt7610u_read32(ad, SEMAPHORE_00, &mac_value);
		loop++;

		if (((mac_value & 0x01) == 0) && (loop < GET_SEMAPHORE_RETRY_MAX)) {
			RtmpOsMsDelay(1);
			goto loadfw_protect;
		}

		if (loop >= GET_SEMAPHORE_RETRY_MAX) {
			DBGPRINT(RT_DEBUG_ERROR, ("%s: can not get the hw semaphore\n", __FUNCTION__));
			return NDIS_STATUS_FAILURE;
		}
	}

	mt7610u_write32(ad, 0x1004, 0x2c);

	/* Enable USB_DMA_CFG */
	mt7610u_read32(ad, USB_DMA_CFG, &UsbCfg.word);
	UsbCfg.field.RxBulkAggTOut = 0x20;
	UsbCfg.field.TxBulkEn = 1;
	UsbCfg.field.RxBulkEn = 1;
	mt7610u_write32(ad, USB_DMA_CFG, UsbCfg.word);
	//USB_CFG_WRITE(ad, 0x00c00020);

	/* Check MCU if ready */
	mt7610u_read32(ad, COM_REG0, &mac_value);

	if (((mac_value & 0x01) == 0x01) && (cap->IsComboChip)) {
		goto error0;
	}

	RTUSBVenderReset(ad);
	RtmpOsMsDelay(5);

	/* Get FW information */
	ilm_len = le32_to_cpu(*((u32 *) (((u8 *) fw_image))));;

	dlm_len = le32_to_cpu(*((u32 *) (((u8 *) fw_image) + 4)));

	fw_ver = le16_to_cpu(*((u16 *) (((u8 *) fw_image) + 10)));

	build_ver = le16_to_cpu(*((u16 *) (((u8 *) fw_image) +8)));

	DBGPRINT(RT_DEBUG_OFF, ("fw version:%d.%d.%02d ", (fw_ver & 0xf000) >> 8,
						(fw_ver & 0x0f00) >> 8, fw_ver & 0x00ff));
	DBGPRINT(RT_DEBUG_OFF, ("build:%x\n", build_ver));
	DBGPRINT(RT_DEBUG_OFF, ("build time:"));

	for (loop = 0; loop < 16; loop++)
		DBGPRINT(RT_DEBUG_OFF, ("%c", *(fw_image + 16 + loop)));

	DBGPRINT(RT_DEBUG_OFF, ("\n"));

	DBGPRINT(RT_DEBUG_OFF, ("ilm length = %d(bytes)\n", ilm_len));
	DBGPRINT(RT_DEBUG_OFF, ("dlm length = %d(bytes)\n", dlm_len));

	/* Enable FCE */
	mt7610u_write32(ad, FCE_PSE_CTRL, 0x01);

	/* FCE tx_fs_base_ptr */
	mt7610u_write32(ad, TX_CPU_PORT_FROM_FCE_BASE_PTR, 0x400230);

	/* FCE tx_fs_max_cnt */
	mt7610u_write32(ad, TX_CPU_PORT_FROM_FCE_MAX_COUNT, 0x01);

	/* FCE pdma enable */
	mt7610u_write32(ad, FCE_PDMA_GLOBAL_CONF, 0x44);

	/* FCE skip_fs_en */
	mt7610u_write32(ad, FCE_SKIP_FS, 0x03);

	if (IS_MT76x0(ad)) {
		mt7610u_read32(ad, USB_DMA_CFG, &UsbCfg.word);

		cfg.field.UDMA_TX_WL_DROP = 1;

		mt7610u_write32(ad, USB_DMA_CFG, UsbCfg.word);

		cfg.field.UDMA_TX_WL_DROP = 0;

		mt7610u_write32(ad, USB_DMA_CFG, UsbCfg.word);
	}

	/* Allocate URB */
	urb = RTUSB_ALLOC_URB(0);

	if (!urb) {
		DBGPRINT(RT_DEBUG_ERROR, ("can not allocate URB\n"));
		ret = NDIS_STATUS_RESOURCES;
		goto error0;
	}

	/* Allocate TransferBuffer */
	fw_data = RTUSB_URB_ALLOC_BUFFER(udev, UPLOAD_FW_UNIT, &fw_dma);

	if (!fw_data) {
		ret = NDIS_STATUS_RESOURCES;
		goto error1;
	}

	DBGPRINT(RT_DEBUG_OFF, ("loading fw"));

	RTMP_OS_INIT_COMPLETION(&load_fw_done);

	if (cap->load_iv)
		cur_len = 0x40;
	else
		cur_len = 0x00;

	/* Loading ILM */
	while (1) {
		s32 sent_len_max = UPLOAD_FW_UNIT - sizeof(*tx_info) - USB_END_PADDING;
		sent_len = (ilm_len - cur_len) >=  sent_len_max ? sent_len_max : (ilm_len - cur_len);

		if (sent_len > 0) {
			tx_info = (struct txinfo_nmac_cmd *)fw_data;
			tx_info->info_type = CMD_PACKET;
			tx_info->pkt_len = sent_len;
			tx_info->d_port = CPU_TX_PORT;

#ifdef RT_BIG_ENDIAN
			RTMPDescriptorEndianChange((u8 *)tx_info, TYPE_TXINFO);
#endif
			memmove(fw_data + sizeof(*tx_info), fw_image + FW_INFO_SIZE + cur_len, sent_len);

			/* four zero bytes for end padding */
			memset(fw_data + sizeof(*tx_info) + sent_len, 0, USB_END_PADDING);

			value = (cur_len + cap->ilm_offset) & 0xFFFF;

			/* Set FCE DMA descriptor */
			ret = RTUSB_VendorRequest(ad,
					 DEVICE_VENDOR_REQUEST_OUT,
					 0x42,
					 value,
					 0x230,
					 NULL,
					 0);


			if (ret) {
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma descriptor fail\n"));
				goto error2;
			}

			value = (((cur_len + cap->ilm_offset) & 0xFFFF0000) >> 16);

			/* Set FCE DMA descriptor */
			ret = RTUSB_VendorRequest(ad,
					 DEVICE_VENDOR_REQUEST_OUT,
					 0x42,
					 value,
					 0x232,
					 NULL,
					 0);

			if (ret) {
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma descriptor fail\n"));
				goto error2;
			}

			cur_len += sent_len;

			while ((sent_len % 4) != 0)
				sent_len++;

			value = ((sent_len << 16) & 0xFFFF);

			/* Set FCE DMA length */
			ret = RTUSB_VendorRequest(ad,
					 DEVICE_VENDOR_REQUEST_OUT,
					 0x42,
					 value,
					 0x234,
					 NULL,
					 0);

			if (ret) {
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma length fail\n"));
				goto error2;
			}

			value = (((sent_len << 16) & 0xFFFF0000) >> 16);

			/* Set FCE DMA length */
			ret = RTUSB_VendorRequest(ad,
					 DEVICE_VENDOR_REQUEST_OUT,
					 0x42,
					 value,
					 0x236,
					 NULL,
					 0);

			if (ret) {
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma length fail\n"));
				goto error2;
			}

			/* Initialize URB descriptor */
			RTUSB_FILL_HTTX_BULK_URB(urb,
					 udev,
					 cap->CommandBulkOutAddr,
					 fw_data,
					 sent_len + sizeof(*tx_info) + USB_END_PADDING,
					 usb_uploadfw_complete,
					 &load_fw_done,
					 fw_dma);

			ret = RTUSB_SUBMIT_URB(urb);

			if (ret) {
				DBGPRINT(RT_DEBUG_ERROR, ("submit urb fail\n"));
				goto error2;
			}

			if (!RTMP_OS_WAIT_FOR_COMPLETION_TIMEOUT(&load_fw_done, RTMPMsecsToJiffies(UPLOAD_FW_TIMEOUT))) {
				RTUSB_UNLINK_URB(urb);
				ret = NDIS_STATUS_FAILURE;
				DBGPRINT(RT_DEBUG_ERROR, ("upload fw timeout(%dms)\n", UPLOAD_FW_TIMEOUT));
				DBGPRINT(RT_DEBUG_ERROR, ("%s: submit urb, sent_len = %d, ilm_ilm = %d, cur_len = %d\n", __FUNCTION__, sent_len, ilm_len, cur_len));

				goto error2;
			}
			DBGPRINT(RT_DEBUG_OFF, ("."));

			mt7610u_read32(ad, TX_CPU_PORT_FROM_FCE_CPU_DESC_INDEX, &mac_value);
			mac_value++;
			mt7610u_write32(ad, TX_CPU_PORT_FROM_FCE_CPU_DESC_INDEX, mac_value);

			RtmpOsMsDelay(5);
		} else {
			break;
		}

	}

	RTMP_OS_EXIT_COMPLETION(&load_fw_done);

	/* Re-Initialize completion */
	RTMP_OS_INIT_COMPLETION(&load_fw_done);

	cur_len = 0x00;

	/* Loading DLM */
	while (1) {
		s32 sent_len_max = UPLOAD_FW_UNIT - sizeof(*tx_info) - USB_END_PADDING;
		sent_len = (dlm_len - cur_len) >= sent_len_max ? sent_len_max : (dlm_len - cur_len);

		if (sent_len > 0) {
			tx_info = (struct txinfo_nmac_cmd *)fw_data;
			tx_info->info_type = CMD_PACKET;
			tx_info->pkt_len = sent_len;
			tx_info->d_port = CPU_TX_PORT;

#ifdef RT_BIG_ENDIAN
			RTMPDescriptorEndianChange((u8 *)tx_info, TYPE_TXINFO);
#endif
			memmove(fw_data + sizeof(*tx_info), fw_image + FW_INFO_SIZE + ilm_len + cur_len, sent_len);

			memset(fw_data + sizeof(*tx_info) + sent_len, 0, USB_END_PADDING);

			value = ((cur_len + cap->dlm_offset) & 0xFFFF);

			/* Set FCE DMA descriptor */
			ret = RTUSB_VendorRequest(ad,
					 DEVICE_VENDOR_REQUEST_OUT,
					 0x42,
					 value,
					 0x230,
					 NULL,
					 0);


			if (ret) {
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma descriptor fail\n"));
				goto error2;
			}

			value = (((cur_len + cap->dlm_offset) & 0xFFFF0000) >> 16);

			/* Set FCE DMA descriptor */
			ret = RTUSB_VendorRequest(ad,
					  DEVICE_VENDOR_REQUEST_OUT,
					  0x42,
					  value,
					  0x232,
					  NULL,
					  0);

			if (ret) {
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma descriptor fail\n"));
				goto error2;
			}

			cur_len += sent_len;

			while ((sent_len % 4) != 0)
				sent_len++;

			value = ((sent_len << 16) & 0xFFFF);

			/* Set FCE DMA length */
			ret = RTUSB_VendorRequest(ad,
					  DEVICE_VENDOR_REQUEST_OUT,
					  0x42,
					  value,
					  0x234,
					  NULL,
					  0);

			if (ret) {
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma length fail\n"));
				goto error2;
			}

			value = (((sent_len << 16) & 0xFFFF0000) >> 16);

			/* Set FCE DMA length */
			ret = RTUSB_VendorRequest(ad,
					  DEVICE_VENDOR_REQUEST_OUT,
					  0x42,
					  value,
					  0x236,
					  NULL,
					  0);

			if (ret) {
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma length fail\n"));
				goto error2;
			}

			/* Initialize URB descriptor */
			RTUSB_FILL_HTTX_BULK_URB(urb,
					 udev,
					 cap->CommandBulkOutAddr,
					 fw_data,
					 sent_len + sizeof(*tx_info) + USB_END_PADDING,
					 usb_uploadfw_complete,
					 &load_fw_done,
					 fw_dma);

			ret = RTUSB_SUBMIT_URB(urb);

			if (ret) {
				DBGPRINT(RT_DEBUG_ERROR, ("submit urb fail\n"));
				goto error2;
			}

			if (!RTMP_OS_WAIT_FOR_COMPLETION_TIMEOUT(&load_fw_done, RTMPMsecsToJiffies(UPLOAD_FW_TIMEOUT))) {
				RTUSB_UNLINK_URB(urb);
				ret = NDIS_STATUS_FAILURE;
				DBGPRINT(RT_DEBUG_ERROR, ("upload fw timeout(%dms)\n", UPLOAD_FW_TIMEOUT));
				DBGPRINT(RT_DEBUG_INFO, ("%s: submit urb, sent_len = %d, dlm_len = %d, cur_len = %d\n", __FUNCTION__, sent_len, dlm_len, cur_len));

				goto error2;
			}
			DBGPRINT(RT_DEBUG_OFF, ("."));

			mt7610u_read32(ad, TX_CPU_PORT_FROM_FCE_CPU_DESC_INDEX, &mac_value);
			mac_value++;
			mt7610u_write32(ad, TX_CPU_PORT_FROM_FCE_CPU_DESC_INDEX, mac_value);
			RtmpOsMsDelay(5);
		} else 	{
			break;
		}

	}

	RTMP_OS_EXIT_COMPLETION(&load_fw_done);

	/* Upload new 64 bytes interrupt vector or reset andes */
	DBGPRINT(RT_DEBUG_OFF, ("\n"));
	usb_load_ivb(ad, fw_image);

	/* Check MCU if ready */
	loop = 0;
	do {
		mt7610u_read32(ad, COM_REG0, &mac_value);
		if ((mac_value & 0x01) == 0x01)
			break;
		RtmpOsMsDelay(10);
		loop++;
	} while (loop <= 100);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: COM_REG0(0x%x) = 0x%x\n", __FUNCTION__, COM_REG0, mac_value));

	if ((mac_value & 0x01) != 0x01)
		ret = NDIS_STATUS_FAILURE;

error2:
	/* Free TransferBuffer */
	RTUSB_URB_FREE_BUFFER(udev, UPLOAD_FW_UNIT, fw_data, fw_dma);

error1:
	/* Free URB */
	RTUSB_FREE_URB(urb);

error0:
	release_firmware(fw);

	if (cap->IsComboChip)
		mt7610u_write32(ad, SEMAPHORE_00, 0x1);

	mt7610u_write32(ad, FCE_PSE_CTRL, 0x01);

	return ret;
}
#endif

static struct cmd_msg *mt7610u_mcu_alloc_cmd_msg(struct rtmp_adapter*ad, unsigned int length)
{
	struct cmd_msg *msg = NULL;
	struct rtmp_chip_cap *cap = &ad->chipCap;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;
#ifdef RTMP_USB_SUPPORT
	struct urb *urb = NULL;
#endif

	struct sk_buff * net_pkt = RTMP_AllocateFragPacketBuffer(ad, cap->cmd_header_len + length + cap->cmd_padding_len);

	if (!net_pkt) {
		DBGPRINT(RT_DEBUG_ERROR, ("can not allocate net_pkt\n"));
		goto error0;
	}

	OS_PKT_RESERVE(net_pkt, cap->cmd_header_len);

	msg = kmalloc(sizeof(*msg), GFP_ATOMIC);

	if (!msg) {
		DBGPRINT(RT_DEBUG_ERROR, ("can not allocate cmd msg\n"));
		goto error1;
	}

	CMD_MSG_CB(net_pkt)->msg = msg;

	memset(msg, 0x00, sizeof(*msg));

#ifdef RTMP_USB_SUPPORT
	urb = RTUSB_ALLOC_URB(0);

	if (!urb) {
		DBGPRINT(RT_DEBUG_ERROR, ("can not allocate urb\n"));
		goto error2;
	}

	msg->urb = urb;
#endif

	msg->priv = ad;
	msg->skb = net_pkt;

	ctl->alloc_cmd_msg++;

	return msg;

error2:
	kfree(msg);
error1:
	RTMPFreeNdisPacket(ad, net_pkt);
error0:
	return NULL;
}

static void mt7610u_mcu_init_cmd_msg(struct cmd_msg *msg, u8 type, bool need_wait, u16 timeout,
							   bool need_retransmit, bool need_rsp, u16 rsp_payload_len,
							   char *rsp_payload, MSG_RSP_HANDLER rsp_handler)
{
	msg->type = type;
	msg->need_wait= need_wait;
	msg->timeout = timeout;

	if (need_wait)
		RTMP_OS_INIT_COMPLETION(&msg->ack_done);

	msg->need_retransmit = false;

	if (need_retransmit)
		msg->retransmit_times = CMD_MSG_RETRANSMIT_TIMES;

	msg->need_rsp = need_rsp;
	msg->rsp_payload_len = rsp_payload_len;
	msg->rsp_payload = rsp_payload;
	msg->rsp_handler = rsp_handler;
}

static void mt7610u_mcu_append_cmd_msg(struct cmd_msg *msg, char *data, unsigned int len)
{
	struct sk_buff * net_pkt = msg->skb;

	if (data)
		memcpy(OS_PKT_TAIL_BUF_EXTEND(net_pkt, len), data, len);
}

void mt7610u_mcu_free_cmd_msg(struct cmd_msg *msg)
{
	struct sk_buff * net_pkt = msg->skb;
	struct rtmp_adapter*ad = msg->priv;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;

	if (msg->need_wait)
		RTMP_OS_EXIT_COMPLETION(&msg->ack_done);

#ifdef RTMP_USB_SUPPORT
	RTUSB_FREE_URB(msg->urb);
#endif

	kfree(msg);

	RTMPFreeNdisPacket(ad, net_pkt);
	ctl->free_cmd_msg++;
}

bool is_inband_cmd_processing(struct rtmp_adapter*ad)
{
	bool ret = 0;

	return ret;
}

u8 get_cmd_rsp_num(struct rtmp_adapter*ad)
{
	u8 Num = 0;

	return Num;
}

static inline void mt7610u_mcu_inc_error_count(struct mt7610u_mcu_ctrl *ctl, enum cmd_msg_error_type type)
{
	if (OS_TEST_BIT(MCU_INIT, &ctl->flags)) {
		switch (type) {
			case error_tx_kickout_fail:
				ctl->tx_kickout_fail_count++;
			break;
			case error_tx_timeout_fail:
				ctl->tx_timeout_fail_count++;
			break;
			case error_rx_receive_fail:
				ctl->rx_receive_fail_count++;
			break;
			default:
				DBGPRINT(RT_DEBUG_ERROR, ("%s:unknown cmd_msg_error_type(%d)\n", __FUNCTION__, type));
		}
	}
}

static spinlock_t *mt7610u_mcu_get_spin_lock(struct mt7610u_mcu_ctrl *ctl, DL_LIST *list)
{
	spinlock_t *lock = NULL;

	if (list == &ctl->txq)
		lock = &ctl->txq_lock;
	else if (list == &ctl->rxq)
		lock = &ctl->rxq_lock;
	else if (list == &ctl->ackq)
		lock = &ctl->ackq_lock;
	else if (list == &ctl->kickq)
		lock = &ctl->kickq_lock;
	else if (list == &ctl->tx_doneq)
		lock = &ctl->tx_doneq_lock;
	else if (list == &ctl->rx_doneq)
		lock = &ctl->rx_doneq_lock;
	else
		DBGPRINT(RT_DEBUG_ERROR, ("%s:illegal list\n", __FUNCTION__));

	return lock;
}

static inline u8 mt7610u_mcu_get_cmd_msg_seq(struct rtmp_adapter*ad)
{
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;
	struct cmd_msg *msg;

	spin_lock_irq(&ctl->ackq_lock);
get_seq:
	ctl->cmd_seq >= 0xf ? ctl->cmd_seq = 1 : ctl->cmd_seq++;
	DlListForEach(msg, &ctl->ackq, struct cmd_msg, list) {
		if (msg->seq == ctl->cmd_seq) {
			DBGPRINT(RT_DEBUG_ERROR, ("command(seq: %d) is still running\n", ctl->cmd_seq));
			DBGPRINT(RT_DEBUG_ERROR, ("command response nums = %d\n", get_cmd_rsp_num(ad)));
			goto get_seq;
		}
	}
	spin_unlock_irq(&ctl->ackq_lock);

	return ctl->cmd_seq;
}

static void _mt7610u_mcu_queue_tail_cmd_msg(DL_LIST *list, struct cmd_msg *msg,
										enum cmd_msg_state state)
{
	msg->state = state;
	DlListAddTail(list, &msg->list);
}

static void mt7610u_mcu_queue_tail_cmd_msg(DL_LIST *list, struct cmd_msg *msg,
										enum cmd_msg_state state)
{
	unsigned long flags;
	spinlock_t *lock;
	struct rtmp_adapter*ad = msg->priv;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;

	lock = mt7610u_mcu_get_spin_lock(ctl, list);

	spin_lock_irqsave(lock, flags);
	_mt7610u_mcu_queue_tail_cmd_msg(list, msg, state);
	spin_unlock_irqrestore(lock, flags);
}

static void _mt7610u_mcu_queue_head_cmd_msg(DL_LIST *list, struct cmd_msg *msg,
										enum cmd_msg_state state)
{
	msg->state = state;
	DlListAdd(list, &msg->list);
}

static void mt7610u_mcu_queue_head_cmd_msg(DL_LIST *list, struct cmd_msg *msg,
										enum cmd_msg_state state)
{
	unsigned long flags;
	spinlock_t *lock;
	struct rtmp_adapter*ad = msg->priv;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;

	lock = mt7610u_mcu_get_spin_lock(ctl, list);

	spin_lock_irqsave(lock, flags);
	_mt7610u_mcu_queue_head_cmd_msg(list, msg, state);
	spin_unlock_irqrestore(lock, flags);
}

static bool mt7610u_mcu_queue_empty(struct mt7610u_mcu_ctrl *ctl, DL_LIST *list)
{
	unsigned long flags;
	bool is_empty;
	spinlock_t *lock;

	lock = mt7610u_mcu_get_spin_lock(ctl, list);

	spin_lock_irqsave(lock, flags);
	is_empty = DlListEmpty(list);
	spin_unlock_irqrestore(lock, flags);

	return is_empty;
}

static void mt7610u_mcu_queue_init(struct mt7610u_mcu_ctrl *ctl, DL_LIST *list)
{

	unsigned long flags;
	spinlock_t *lock;

	lock = mt7610u_mcu_get_spin_lock(ctl, list);

	spin_lock_irqsave(lock, flags);
	DlListInit(list);
	spin_unlock_irqrestore(lock, flags);
}

static void _mt7610u_mcu_unlink_cmd_msg(struct cmd_msg *msg, DL_LIST *list)
{
	if (!msg)
		return;

	DlListDel(&msg->list);
}

static void mt7610u_mcu_unlink_cmd_msg(struct cmd_msg *msg, DL_LIST *list)
{
	unsigned long flags;
	spinlock_t *lock;
	struct rtmp_adapter*ad = msg->priv;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;

	lock = mt7610u_mcu_get_spin_lock(ctl, list);

	spin_lock_irqsave(lock, flags);
	_mt7610u_mcu_unlink_cmd_msg(msg, list);
	spin_unlock_irqrestore(lock, flags);
}

static struct cmd_msg *_mt7610u_mcu_dequeue_cmd_msg(DL_LIST *list)
{
	struct cmd_msg *msg;

	msg = DlListFirst(list, struct cmd_msg, list);

	_mt7610u_mcu_unlink_cmd_msg(msg, list);

	return msg;
}

static struct cmd_msg *mt7610u_mcu_dequeue_cmd_msg(struct mt7610u_mcu_ctrl *ctl, DL_LIST *list)
{
	unsigned long flags;
	struct cmd_msg *msg;
	spinlock_t *lock;

	lock = mt7610u_mcu_get_spin_lock(ctl, list);

	spin_lock_irqsave(lock, flags);
	msg = _mt7610u_mcu_dequeue_cmd_msg(list);
	spin_unlock_irqrestore(lock, flags);

	return msg;
}

void mt7610u_mcu_rx_process_cmd_msg(struct rtmp_adapter*ad, struct cmd_msg *rx_msg)
{
	struct sk_buff * net_pkt = rx_msg->skb;
	struct cmd_msg *msg, *msg_tmp;
	struct rxfce_info_cmd *rx_info = (struct rxfce_info_cmd *) net_pkt->data;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;

#ifdef RT_BIG_ENDIAN
	RTMPDescriptorEndianChange((u8 *)rx_info, TYPE_RXINFO);
#endif


	if ((rx_info->info_type != CMD_PACKET)) {
		DBGPRINT(RT_DEBUG_ERROR, ("packet is not command response/self event\n"));
		return;
	}

	if (rx_info->self_gen) {
		/* if have callback function */
		RTEnqueueInternalCmd(ad, CMDTHREAD_RESPONSE_EVENT_CALLBACK,
								net_pkt->data + sizeof(*rx_info), rx_info->pkt_len);
	} else {
		spin_lock_irq(&ctl->ackq_lock);
		DlListForEachSafe(msg, msg_tmp, &ctl->ackq, struct cmd_msg, list) {
			if (msg->seq == rx_info->cmd_seq) {
				_mt7610u_mcu_unlink_cmd_msg(msg, &ctl->ackq);
				spin_unlock_irq(&ctl->ackq_lock);

				if ((msg->rsp_payload_len == rx_info->pkt_len) && (msg->rsp_payload_len != 0)) {
					msg->rsp_handler(msg, net_pkt->data + sizeof(*rx_info), rx_info->pkt_len);
				} else if ((msg->rsp_payload_len == 0) && (rx_info->pkt_len == 8)) {
					DBGPRINT(RT_DEBUG_INFO, ("command response(ack) success\n"));
				} else {
					DBGPRINT(RT_DEBUG_ERROR, ("expect response len(%d), command response len(%d) invalid\n", msg->rsp_payload_len, rx_info->pkt_len));
					msg->rsp_payload_len = rx_info->pkt_len;
				}

				if (msg->need_wait)
					RTMP_OS_COMPLETE(&msg->ack_done);
				else
					mt7610u_mcu_free_cmd_msg(msg);

				spin_lock_irq(&ctl->ackq_lock);
				break;
			}
		}
		spin_unlock_irq(&ctl->ackq_lock);
	}
}

#ifdef RTMP_USB_SUPPORT
static void usb_rx_cmd_msg_complete(struct urb *urb)
{
	struct sk_buff * net_pkt = urb->context;
	struct cmd_msg *msg = CMD_MSG_CB(net_pkt)->msg;
	struct rtmp_adapter*ad = msg->priv;
	struct os_cookie *pObj = ad->OS_Cookie;
	struct rtmp_chip_cap *pChipCap = &ad->chipCap;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;
	enum cmd_msg_state state;
	unsigned long flags;
	int ret = 0;

	mt7610u_mcu_unlink_cmd_msg(msg, &ctl->rxq);

	OS_PKT_TAIL_BUF_EXTEND(net_pkt, RTMP_USB_URB_LEN_GET(urb));

	if (RTMP_USB_URB_STATUS_GET(urb) == 0) {
		state = rx_done;
	} else {
		state = rx_receive_fail;
		mt7610u_mcu_inc_error_count(ctl, error_rx_receive_fail);
		DBGPRINT(RT_DEBUG_ERROR, ("receive cmd msg fail(%d)\n", RTMP_USB_URB_STATUS_GET(urb)));
	}

	spin_lock_irqsave(&ctl->rx_doneq_lock, flags);
	_mt7610u_mcu_queue_tail_cmd_msg(&ctl->rx_doneq, msg, state);
	spin_unlock_irqrestore(&ctl->rx_doneq_lock, flags);

	if (OS_TEST_BIT(MCU_INIT, &ctl->flags)) {
		msg = mt7610u_mcu_alloc_cmd_msg(ad, 512);

		if (!msg) {
			return;
		}

		net_pkt = msg->skb;

		RTUSB_FILL_BULK_URB(msg->urb, pObj->pUsb_Dev,
							usb_rcvbulkpipe(pObj->pUsb_Dev, pChipCap->CommandRspBulkInAddr),
							net_pkt->data, 512, usb_rx_cmd_msg_complete, net_pkt);

		mt7610u_mcu_queue_tail_cmd_msg(&ctl->rxq, msg, rx_start);

		ret = RTUSB_SUBMIT_URB(msg->urb);

		if (ret) {
			mt7610u_mcu_unlink_cmd_msg(msg, &ctl->rxq);
			mt7610u_mcu_inc_error_count(ctl, error_rx_receive_fail);
			DBGPRINT(RT_DEBUG_ERROR, ("%s:submit urb fail(%d)\n", __FUNCTION__, ret));
			mt7610u_mcu_queue_tail_cmd_msg(&ctl->rx_doneq, msg, rx_receive_fail);
		}

	}

	mt7610u_mcu_bh_schedule(ad);
}

int usb_rx_cmd_msg_submit(struct rtmp_adapter*ad)
{
	struct rtmp_chip_cap *pChipCap = &ad->chipCap;
	struct os_cookie *pObj = ad->OS_Cookie;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;
	struct cmd_msg *msg = NULL;
	struct sk_buff * net_pkt = NULL;
	int ret = 0;

	if (!OS_TEST_BIT(MCU_INIT, &ctl->flags))
		return ret;

	msg =  mt7610u_mcu_alloc_cmd_msg(ad, 512);

	if (!msg) {
		ret = NDIS_STATUS_RESOURCES;
		return ret;
	}

	net_pkt = msg->skb;

	RTUSB_FILL_BULK_URB(msg->urb, pObj->pUsb_Dev,
						usb_rcvbulkpipe(pObj->pUsb_Dev, pChipCap->CommandRspBulkInAddr),
						net_pkt->data, 512, usb_rx_cmd_msg_complete, net_pkt);

	mt7610u_mcu_queue_tail_cmd_msg(&ctl->rxq, msg, rx_start);

	ret = RTUSB_SUBMIT_URB(msg->urb);

	if (ret) {
		mt7610u_mcu_unlink_cmd_msg(msg, &ctl->rxq);
		mt7610u_mcu_inc_error_count(ctl, error_rx_receive_fail);
		DBGPRINT(RT_DEBUG_ERROR, ("%s:submit urb fail(%d)\n", __FUNCTION__, ret));
		mt7610u_mcu_queue_tail_cmd_msg(&ctl->rx_doneq, msg, rx_receive_fail);
	}

	return ret;
}

int usb_rx_cmd_msgs_receive(struct rtmp_adapter*ad)
{
	int ret = 0;
	int i;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;

	for (i = 0; (i < 1) && mt7610u_mcu_queue_empty(ctl, &ctl->rxq); i++) {
		ret = usb_rx_cmd_msg_submit(ad);
		if (ret)
			break;
	}

	return ret;
}

void mt7610u_mcu_cmd_msg_bh(unsigned long param)
{
	struct rtmp_adapter*ad = (struct rtmp_adapter*)param;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;
	struct cmd_msg *msg = NULL;

	while ((msg = mt7610u_mcu_dequeue_cmd_msg(ctl, &ctl->rx_doneq))) {
		switch (msg->state) {
		case rx_done:
			mt7610u_mcu_rx_process_cmd_msg(ad, msg);
			mt7610u_mcu_free_cmd_msg(msg);
			continue;
		case rx_receive_fail:
			mt7610u_mcu_free_cmd_msg(msg);
			continue;
		default:
			DBGPRINT(RT_DEBUG_ERROR, ("unknow msg state(%d)\n", msg->state));
		}
	}

	while ((msg = mt7610u_mcu_dequeue_cmd_msg(ctl, &ctl->tx_doneq))) {
		switch (msg->state) {
		case tx_done:
		case tx_kickout_fail:
		case tx_timeout_fail:
			mt7610u_mcu_free_cmd_msg(msg);
			continue;
		default:
			DBGPRINT(RT_DEBUG_ERROR, ("unknow msg state(%d)\n", msg->state));
		}
	}

	if (OS_TEST_BIT(MCU_INIT, &ctl->flags)) {
		mt7610u_mcu_bh_schedule(ad);
		usb_rx_cmd_msgs_receive(ad);
	}
}

void mt7610u_mcu_bh_schedule(struct rtmp_adapter*ad)
{
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;

	if (!OS_TEST_BIT(MCU_INIT, &ctl->flags))
		return;

	if ((!mt7610u_mcu_queue_empty(ctl, &ctl->rx_doneq) ||
	     !mt7610u_mcu_queue_empty(ctl, &ctl->tx_doneq)) &&
	    OS_TEST_BIT(MCU_INIT, &ctl->flags)) {
		RTMP_NET_TASK_DATA_ASSIGN(&ctl->cmd_msg_task, (unsigned long)(ad));
		RTMP_OS_TASKLET_SCHE(&ctl->cmd_msg_task);
	}
}

static void usb_kick_out_cmd_msg_complete(struct urb *urb)
{
	struct sk_buff * net_pkt = urb->context;
	struct cmd_msg *msg = CMD_MSG_CB(net_pkt)->msg;
	struct rtmp_adapter*ad = msg->priv;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;

	if (!OS_TEST_BIT(MCU_INIT, &ctl->flags))
		return;

	if (RTMP_USB_URB_STATUS_GET(urb) == 0) {
		if (!msg->need_rsp) {
			mt7610u_mcu_unlink_cmd_msg(msg, &ctl->kickq);
			mt7610u_mcu_queue_tail_cmd_msg(&ctl->tx_doneq, msg, tx_done);
		} else {
			msg->state = wait_ack;
		}
	} else {
		if (!msg->need_rsp) {
			mt7610u_mcu_unlink_cmd_msg(msg, &ctl->kickq);
			mt7610u_mcu_queue_tail_cmd_msg(&ctl->tx_doneq, msg, tx_kickout_fail);
			mt7610u_mcu_inc_error_count(ctl, error_tx_kickout_fail);
		} else {
			mt7610u_mcu_unlink_cmd_msg(msg, &ctl->ackq);
			msg->state = tx_kickout_fail;
			mt7610u_mcu_inc_error_count(ctl, error_tx_kickout_fail);
			RTMP_OS_COMPLETE(&msg->ack_done);
		}

		DBGPRINT(RT_DEBUG_ERROR, ("kick out cmd msg fail(%d)\n", RTMP_USB_URB_STATUS_GET(urb)));
	}

	mt7610u_mcu_bh_schedule(ad);
}

int usb_kick_out_cmd_msg(struct rtmp_adapter *ad, struct cmd_msg *msg)
{
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;
	struct os_cookie *pObj = ad->OS_Cookie;
	int ret = 0;
	struct sk_buff * net_pkt = msg->skb;
	struct rtmp_chip_cap *pChipCap = &ad->chipCap;

	/* append four zero bytes padding when usb aggregate enable */
	memset(OS_PKT_TAIL_BUF_EXTEND(net_pkt, 4), 0x00, 4);

	RTUSB_FILL_BULK_URB(msg->urb, pObj->pUsb_Dev,
						usb_sndbulkpipe(pObj->pUsb_Dev, pChipCap->CommandBulkOutAddr),
						net_pkt->data, net_pkt->len, usb_kick_out_cmd_msg_complete, net_pkt);

	if (msg->need_rsp)
		mt7610u_mcu_queue_tail_cmd_msg(&ctl->ackq, msg, wait_cmd_out_and_ack);
	else
		mt7610u_mcu_queue_tail_cmd_msg(&ctl->kickq, msg, wait_cmd_out);

	if (!OS_TEST_BIT(MCU_INIT, &ctl->flags))
		return -1;

	ret = RTUSB_SUBMIT_URB(msg->urb);

	if (ret) {
		if (!msg->need_rsp) {
			mt7610u_mcu_unlink_cmd_msg(msg, &ctl->kickq);
			mt7610u_mcu_queue_tail_cmd_msg(&ctl->tx_doneq, msg, tx_kickout_fail);
			mt7610u_mcu_inc_error_count(ctl, error_tx_kickout_fail);
		} else {
			mt7610u_mcu_unlink_cmd_msg(msg, &ctl->ackq);
			msg->state = tx_kickout_fail;
			mt7610u_mcu_inc_error_count(ctl, error_tx_kickout_fail);
			RTMP_OS_COMPLETE(&msg->ack_done);
		}

		DBGPRINT(RT_DEBUG_ERROR, ("%s:submit urb fail(%d)\n", __FUNCTION__, ret));
	}

	return ret;
}

void mt7610u_mcu_usb_unlink_urb(struct rtmp_adapter*ad, DL_LIST *list)
{
	unsigned long flags;
	struct cmd_msg *msg, *msg_tmp;
	spinlock_t *lock;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;

	lock = mt7610u_mcu_get_spin_lock(ctl, list);

	spin_lock_irqsave(lock, flags);
	DlListForEachSafe(msg, msg_tmp, list, struct cmd_msg, list) {
		spin_unlock_irqrestore(lock, flags);
		if ((msg->state == wait_cmd_out_and_ack) || (msg->state == wait_cmd_out) ||
						(msg->state == tx_start) || (msg->state == rx_start) ||
						(msg->state == tx_retransmit))
			RTUSB_UNLINK_URB(msg->urb);
		spin_lock_irqsave(lock, flags);
	}
	spin_unlock_irqrestore(lock, flags);
}

#endif

void mt7610u_mcu_cleanup_cmd_msg(struct rtmp_adapter*ad, DL_LIST *list)
{
	unsigned long flags;
	struct cmd_msg *msg, *msg_tmp;
	spinlock_t *lock;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;

	lock = mt7610u_mcu_get_spin_lock(ctl, list);

	spin_lock_irqsave(lock, flags);
	DlListForEachSafe(msg, msg_tmp, list, struct cmd_msg, list) {
		_mt7610u_mcu_unlink_cmd_msg(msg, list);
		mt7610u_mcu_free_cmd_msg(msg);
	}
	DlListInit(list);
	spin_unlock_irqrestore(lock, flags);
}

void mt7610u_mcu_ctrl_init(struct rtmp_adapter*ad)
{
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;
	int ret = 0;

	OS_SEM_EVENT_WAIT(&(ad->mcu_atomic), ret);
	RTMP_CLEAR_FLAG(ad, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD);
	ctl->cmd_seq = 0;
	RTMP_OS_TASKLET_INIT(ad, &ctl->cmd_msg_task, mt7610u_mcu_cmd_msg_bh, (unsigned long)ad);
	spin_lock_init(&ctl->txq_lock);
	mt7610u_mcu_queue_init(ctl, &ctl->txq);
	spin_lock_init(&ctl->rxq_lock);
	mt7610u_mcu_queue_init(ctl, &ctl->rxq);
	spin_lock_init(&ctl->ackq_lock);
	mt7610u_mcu_queue_init(ctl, &ctl->ackq);
	spin_lock_init(&ctl->kickq_lock);
	mt7610u_mcu_queue_init(ctl, &ctl->kickq);
	spin_lock_init(&ctl->tx_doneq_lock);
	mt7610u_mcu_queue_init(ctl, &ctl->tx_doneq);
	spin_lock_init(&ctl->rx_doneq_lock);
	mt7610u_mcu_queue_init(ctl, &ctl->rx_doneq);
	ctl->tx_kickout_fail_count = 0;
	ctl->tx_timeout_fail_count = 0;
	ctl->rx_receive_fail_count = 0;
	ctl->alloc_cmd_msg = 0;
	ctl->free_cmd_msg = 0;
	OS_SET_BIT(MCU_INIT, &ctl->flags);
	usb_rx_cmd_msgs_receive(ad);
	OS_SEM_EVENT_UP(&(ad->mcu_atomic));
}

void mt7610u_mcu_ctrl_exit(struct rtmp_adapter*ad)
{
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;
	int ret = 0;

	OS_SEM_EVENT_WAIT(&(ad->mcu_atomic), ret);
	RTMP_CLEAR_FLAG(ad, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD);
	OS_CLEAR_BIT(MCU_INIT, &ctl->flags);
	mt7610u_mcu_usb_unlink_urb(ad, &ctl->txq);
	mt7610u_mcu_usb_unlink_urb(ad, &ctl->kickq);
	mt7610u_mcu_usb_unlink_urb(ad, &ctl->ackq);
	mt7610u_mcu_usb_unlink_urb(ad, &ctl->rxq);
	RTMP_OS_TASKLET_KILL(&ctl->cmd_msg_task);
	mt7610u_mcu_cleanup_cmd_msg(ad, &ctl->txq);
	mt7610u_mcu_cleanup_cmd_msg(ad, &ctl->ackq);
	mt7610u_mcu_cleanup_cmd_msg(ad, &ctl->rxq);
	mt7610u_mcu_cleanup_cmd_msg(ad, &ctl->kickq);
	mt7610u_mcu_cleanup_cmd_msg(ad, &ctl->tx_doneq);
	mt7610u_mcu_cleanup_cmd_msg(ad, &ctl->rx_doneq);
	DBGPRINT(RT_DEBUG_OFF, ("tx_kickout_fail_count = %ld\n", ctl->tx_kickout_fail_count));
	DBGPRINT(RT_DEBUG_OFF, ("tx_timeout_fail_count = %ld\n", ctl->tx_timeout_fail_count));
	DBGPRINT(RT_DEBUG_OFF, ("rx_receive_fail_count = %ld\n", ctl->rx_receive_fail_count));
	DBGPRINT(RT_DEBUG_OFF, ("alloc_cmd_msg = %ld\n", ctl->alloc_cmd_msg));
	DBGPRINT(RT_DEBUG_OFF, ("free_cmd_msg = %ld\n", ctl->free_cmd_msg));
	OS_SEM_EVENT_UP(&(ad->mcu_atomic));
}

static int mt7610u_mcu_dequeue_and_kick_out_cmd_msgs(struct rtmp_adapter*ad)
{
	struct cmd_msg *msg = NULL;
	struct sk_buff * net_pkt = NULL;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;
	int ret = NDIS_STATUS_SUCCESS;
	struct txinfo_nmac_cmd *tx_info;

	while ((msg = mt7610u_mcu_dequeue_cmd_msg(ctl, &ctl->txq)) != NULL) {
		if (!RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD)
				|| RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_NIC_NOT_EXIST)
				|| RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_SUSPEND)) {
			if (!msg->need_rsp)
				mt7610u_mcu_free_cmd_msg(msg);
			continue;
		}

		if (!mt7610u_mcu_queue_empty(ctl, &ctl->ackq)) {
			mt7610u_mcu_queue_head_cmd_msg(&ctl->txq, msg, msg->state);
			ret = NDIS_STATUS_FAILURE;
			continue;
		}

		net_pkt = msg->skb;

		if (msg->need_rsp)
			msg->seq = mt7610u_mcu_get_cmd_msg_seq(ad);
		else
			msg->seq = 0;

		tx_info = (struct txinfo_nmac_cmd *)OS_PKT_HEAD_BUF_EXTEND(net_pkt, sizeof(*tx_info));
		tx_info->info_type = CMD_PACKET;
		tx_info->d_port = CPU_TX_PORT;
		tx_info->cmd_type = msg->type;
		tx_info->cmd_seq = msg->seq;
		tx_info->pkt_len = net_pkt->len - sizeof(*tx_info);

#ifdef RT_BIG_ENDIAN
		RTMPDescriptorEndianChange((u8 *)tx_info, TYPE_TXINFO);
#endif


#ifdef RTMP_USB_SUPPORT
		ret = usb_kick_out_cmd_msg(ad, msg);
#endif


		if (ret) {
			DBGPRINT(RT_DEBUG_ERROR, ("kick out msg fail\n"));
			break;
		}
	}

	mt7610u_mcu_bh_schedule(ad);

	return ret;
}

int mt7610u_mcu_send_cmd_msg(struct rtmp_adapter *ad, struct cmd_msg *msg)
{
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;
	int ret = 0;
	bool need_wait = msg->need_wait;

	OS_SEM_EVENT_WAIT(&(ad->mcu_atomic), ret);

	if (!RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD)
				|| RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_NIC_NOT_EXIST)
				|| RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_SUSPEND)) {
		mt7610u_mcu_free_cmd_msg(msg);
		OS_SEM_EVENT_UP(&(ad->mcu_atomic));
		return NDIS_STATUS_FAILURE;
	}

	mt7610u_mcu_queue_tail_cmd_msg(&ctl->txq, msg, tx_start);

retransmit:
	mt7610u_mcu_dequeue_and_kick_out_cmd_msgs(ad);

	/* Wait for response */
	if (need_wait)
	{
		enum cmd_msg_state state;
		unsigned long expire;
		unsigned long timeout = msg->timeout;
		expire = timeout ? RTMPMsecsToJiffies(timeout) : RTMPMsecsToJiffies(CMD_MSG_TIMEOUT);
		if (!RTMP_OS_WAIT_FOR_COMPLETION_TIMEOUT(&msg->ack_done, expire)) {
			ret = NDIS_STATUS_FAILURE;
			if (OS_TEST_BIT(MCU_INIT, &ctl->flags)) {
				if (msg->state == wait_cmd_out_and_ack) {
					RTUSB_UNLINK_URB(msg->urb);
				} else if (msg->state == wait_ack) {
					mt7610u_mcu_unlink_cmd_msg(msg, &ctl->ackq);
				}
			}

			mt7610u_mcu_inc_error_count(ctl, error_tx_timeout_fail);
			state = tx_timeout_fail;
			msg->retransmit_times--;
			DBGPRINT(RT_DEBUG_ERROR, ("msg->retransmit_times = %d\n", msg->retransmit_times));
		} else {
			if (msg->state == tx_kickout_fail) {
				state = tx_kickout_fail;
				msg->retransmit_times--;
			} else {
				state = tx_done;
				msg->retransmit_times = 0;
			}
		}

		if (OS_TEST_BIT(MCU_INIT, &ctl->flags)) {
			if (msg->need_retransmit && (msg->retransmit_times > 0)) {
				RTMP_OS_EXIT_COMPLETION(&msg->ack_done);
				RTMP_OS_INIT_COMPLETION(&msg->ack_done);
				state = tx_retransmit;
				mt7610u_mcu_queue_head_cmd_msg(&ctl->txq, msg, state);
				goto retransmit;
			} else {
				mt7610u_mcu_queue_tail_cmd_msg(&ctl->tx_doneq, msg, state);
			}
		} else {
			mt7610u_mcu_free_cmd_msg(msg);
		}
	}

	OS_SEM_EVENT_UP(&(ad->mcu_atomic));

	return ret;
}

static void mt7610u_mcu_pwr_event_handler(struct rtmp_adapter*ad, char *payload, u16 payload_len)
{


}


static void mt7610u_mcu_wow_event_handler(struct rtmp_adapter*ad, char *payload, u16 payload_len)
{


}

static void mt7610u_mcu_carrier_detect_event_handler(struct rtmp_adapter*ad, char *payload, u16 payload_len)
{



}

static void mt7610u_mcu_dfs_detect_event_handler(struct rtmp_adapter *ad, char *payload, u16 payload_len)
{



}

MSG_EVENT_HANDLER msg_event_handler_tb[] =
{
	mt7610u_mcu_pwr_event_handler,
	mt7610u_mcu_wow_event_handler,
	mt7610u_mcu_carrier_detect_event_handler,
	mt7610u_mcu_dfs_detect_event_handler,
};


int mt7610u_mcu_burst_write(struct rtmp_adapter*ad, u32 offset, u32 *data, u32 cnt)
{
	struct cmd_msg *msg;
	unsigned int var_len, offset_num, cur_len = 0, sent_len;
	u32 value, i, cur_index = 0;
	struct rtmp_chip_cap *cap = &ad->chipCap;
	int ret = 0;
	bool last_packet = false;

	if (!data)
		return -1;

	offset_num = cnt / ((cap->InbandPacketMaxLen - sizeof(offset)) / 4);

	if (cnt % ((cap->InbandPacketMaxLen - sizeof(offset)) / 4))
		var_len = sizeof(offset) * (offset_num + 1) + 4 * cnt;
	else
		var_len = sizeof(offset) * offset_num + 4 * cnt;

	while (cur_len < var_len) {
		sent_len = (var_len - cur_len) > cap->InbandPacketMaxLen
									? cap->InbandPacketMaxLen : (var_len - cur_len);

		if ((sent_len < cap->InbandPacketMaxLen) || ((cur_len + cap->InbandPacketMaxLen) == var_len))
			last_packet = true;

		msg = mt7610u_mcu_alloc_cmd_msg(ad, sent_len);

		if (!msg) {
			ret = NDIS_STATUS_RESOURCES;
			goto error;
		}

		if (last_packet) {
			mt7610u_mcu_init_cmd_msg(msg, CMD_BURST_WRITE, true, 0, true, true, 0, NULL, NULL);
		}else {
			mt7610u_mcu_init_cmd_msg(msg, CMD_BURST_WRITE, false, 0, false, false, 0, NULL, NULL);
		}


		value = cpu2le32(offset + cap->WlanMemmapOffset + cur_index * 4);
		mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);

		for (i = 0; i < ((sent_len - 4) / 4); i++) {
			value = cpu2le32(data[i + cur_index]);
			mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);
		}

		ret = mt7610u_mcu_send_cmd_msg(ad, msg);


		cur_index += ((sent_len - 4) / 4);
		cur_len += cap->InbandPacketMaxLen;
	}

error:
	return ret;
}

static void mt7610u_mcu_burst_read_callback(struct cmd_msg *msg, char *rsp_payload, u16 rsp_payload_len)
{
	u32 i;
	u32 *data;
	memmove(msg->rsp_payload, rsp_payload + 4, rsp_payload_len - 4);

	for (i = 0; i < (msg->rsp_payload_len - 4) / 4; i++) {
		data = (u32 *)(msg->rsp_payload + i * 4);
		*data = le2cpu32(*data);
	}
}

int mt7610u_mcu_burst_read(struct rtmp_adapter*ad, u32 offset, u32 cnt, u32 *data)
{
	struct cmd_msg *msg;
	unsigned int cur_len = 0, rsp_len, offset_num, receive_len;
	u32 value, cur_index = 0;
	struct rtmp_chip_cap *cap = &ad->chipCap;
	int ret = 0;

	if (!data)
		return -1;

	offset_num = cnt / ((cap->InbandPacketMaxLen - sizeof(offset)) / 4);

	if (cnt % ((cap->InbandPacketMaxLen - sizeof(offset)) / 4))
		rsp_len = sizeof(offset) * (offset_num + 1) + 4 * cnt;
	else
		rsp_len = sizeof(offset) * offset_num + 4 * cnt;

	while (cur_len < rsp_len) {
		receive_len = (rsp_len - cur_len) > cap->InbandPacketMaxLen
									   ? cap->InbandPacketMaxLen
									   : (rsp_len - cur_len);

		msg = mt7610u_mcu_alloc_cmd_msg(ad, 8);

		if (!msg) {
			ret = NDIS_STATUS_RESOURCES;
			goto error;
		}

		mt7610u_mcu_init_cmd_msg(msg, CMD_BURST_READ, true, 0, true, true, receive_len,
									(char *)(&data[cur_index]), mt7610u_mcu_burst_read_callback);

		value = cpu2le32(offset + cap->WlanMemmapOffset + cur_index * 4);
		mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);

		value = cpu2le32((receive_len - 4) / 4);
		mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);

		ret = mt7610u_mcu_send_cmd_msg(ad, msg);

		if (ret) {
			if (cnt == 1)
				*data = 0xffffffff;
		}

		cur_index += ((receive_len - 4) / 4);
		cur_len += cap->InbandPacketMaxLen;
	}

error:
	return ret;
}

static void mt7610u_mcu_random_read_callback(struct cmd_msg *msg, char *rsp_payload, u16 rsp_payload_len)
{
	u32 i;
	RTMP_REG_PAIR *reg_pair = (RTMP_REG_PAIR *)msg->rsp_payload;

	for (i = 0; i < msg->rsp_payload_len / 8; i++) {
		memmove(&reg_pair[i].Value, rsp_payload + 8 * i + 4, 4);
		reg_pair[i].Value = le2cpu32(reg_pair[i].Value);
	}
}

int mt7610u_mcu_random_read(struct rtmp_adapter*ad, RTMP_REG_PAIR *reg_pair, u32 num)
{
	struct cmd_msg *msg;
	unsigned int var_len = num * 8, cur_len = 0, receive_len;
	u32 i, value, cur_index = 0;
	struct rtmp_chip_cap *cap = &ad->chipCap;
	int ret = 0;

	if (!reg_pair)
		return -1;

	while (cur_len < var_len)
	{
		receive_len = (var_len - cur_len) > cap->InbandPacketMaxLen
									   ? cap->InbandPacketMaxLen
									   : (var_len - cur_len);

		msg = mt7610u_mcu_alloc_cmd_msg(ad, receive_len);

		if (!msg) {
			ret = NDIS_STATUS_RESOURCES;
			goto error;
		}

		mt7610u_mcu_init_cmd_msg(msg, CMD_RANDOM_READ, true, 0, true, true, receive_len,
									(char *)&reg_pair[cur_index], mt7610u_mcu_random_read_callback);

		for (i = 0; i < receive_len / 8; i++) {
			value = cpu2le32(reg_pair[i + cur_index].Register + cap->WlanMemmapOffset);
			mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);
			value = 0;
			mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);
		}


		ret = mt7610u_mcu_send_cmd_msg(ad, msg);


		cur_index += receive_len / 8;
		cur_len += cap->InbandPacketMaxLen;
	}

error:
	return ret;
}

static void mt7610u_mcu_rf_random_read_callback(struct cmd_msg *msg, char *rsp_payload, u16 rsp_payload_len)
{
	u32 i;
	BANK_RF_REG_PAIR *reg_pair = (BANK_RF_REG_PAIR *)msg->rsp_payload;

	for (i = 0; i < msg->rsp_payload_len / 8; i++) {
		memmove(&reg_pair[i].Value, rsp_payload + 8 * i + 4, 1);
	}
}

int mt7610u_mcu_rf_random_read(struct rtmp_adapter*ad, BANK_RF_REG_PAIR *reg_pair, u32 num)
{
	struct cmd_msg *msg;
	unsigned int var_len = num * 8, cur_len = 0, receive_len;
	u32 i, value, cur_index = 0;
	struct rtmp_chip_cap *cap = &ad->chipCap;
	int ret = 0;

	if (!reg_pair)
		return -1;

	while (cur_len < var_len) {
		receive_len = (var_len - cur_len) > cap->InbandPacketMaxLen
									   ? cap->InbandPacketMaxLen
									   : (var_len - cur_len);

		msg = mt7610u_mcu_alloc_cmd_msg(ad, receive_len);

		if (!msg) {
			ret = NDIS_STATUS_RESOURCES;
			goto error;
		}

		mt7610u_mcu_init_cmd_msg(msg, CMD_RANDOM_READ, true, 0, true, true, receive_len,
									(char *)&reg_pair[cur_index], mt7610u_mcu_rf_random_read_callback);

		for (i = 0; i < (receive_len) / 8; i++) {
			value = 0;

			/* RF selection */
			value = (value & ~0x80000000) | 0x80000000;

			/* RF bank */
			value = (value & ~0x00ff0000) | (reg_pair[i + cur_index].Bank << 16);

			/* RF Index */
			value = (value & ~0x0000ffff) | reg_pair[i + cur_index].Register;

			value = cpu2le32(value);
			mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);
			value = 0;
			mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);
		}

		ret = mt7610u_mcu_send_cmd_msg(ad, msg);


		cur_index += receive_len / 8;
		cur_len += cap->InbandPacketMaxLen;
	}

error:
	return ret;
}

int mt7610u_mcu_read_modify_write(struct rtmp_adapter*ad, R_M_W_REG *reg_pair, u32 num)
{
	struct cmd_msg *msg;
	unsigned int var_len = num * 12, cur_len = 0, sent_len;
	u32 value, i, cur_index = 0;
	struct rtmp_chip_cap *cap = &ad->chipCap;
	int ret = 0;
	bool last_packet = false;

	if (!reg_pair)
		return -1;

	while (cur_len < var_len) {
		sent_len = (var_len - cur_len) > cap->InbandPacketMaxLen
									? cap->InbandPacketMaxLen : (var_len - cur_len);

		if ((sent_len < cap->InbandPacketMaxLen) || (cur_len + cap->InbandPacketMaxLen) == var_len)
			last_packet = true;

		msg = mt7610u_mcu_alloc_cmd_msg(ad, sent_len);

		if (!msg) {
			ret = NDIS_STATUS_RESOURCES;
			goto error;
		}

		if (last_packet)
			mt7610u_mcu_init_cmd_msg(msg, CMD_READ_MODIFY_WRITE, true, 0, true, true, 0, NULL, NULL);
		else
			mt7610u_mcu_init_cmd_msg(msg, CMD_READ_MODIFY_WRITE, false, 0, false, false, 0, NULL, NULL);

		for (i = 0; i < (sent_len / 12); i++) {
			/* Address */
			value = cpu2le32(reg_pair[i + cur_index].Register + cap->WlanMemmapOffset);
			mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);

			/* ClearBitMask */
			value = cpu2le32(reg_pair[i + cur_index].ClearBitMask);
			mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);

			/* UpdateData */
			value = cpu2le32(reg_pair[i + cur_index].Value);
			mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);
		}

		ret = mt7610u_mcu_send_cmd_msg(ad, msg);


		cur_index += (sent_len / 12);
		cur_len += cap->InbandPacketMaxLen;
	}

error:
	return ret;
}

int mt7610u_mcu_random_write(struct rtmp_adapter*ad, RTMP_REG_PAIR *reg_pair, u32 num)
{
	struct cmd_msg *msg;
	unsigned int var_len = num * 8, cur_len = 0, sent_len;
	u32 value, i, cur_index = 0;
	struct rtmp_chip_cap *cap = &ad->chipCap;
	int ret = 0;
	bool last_packet = false;

	if (!reg_pair)
		return -1;

	while (cur_len < var_len) {
		sent_len = (var_len - cur_len) > cap->InbandPacketMaxLen
									? cap->InbandPacketMaxLen : (var_len - cur_len);

		if ((sent_len < cap->InbandPacketMaxLen) || (cur_len + cap->InbandPacketMaxLen) == var_len)
			last_packet = true;

		msg = mt7610u_mcu_alloc_cmd_msg(ad, sent_len);

		if (!msg) {
			ret = NDIS_STATUS_RESOURCES;
			goto error;
		}

		if (last_packet)
			mt7610u_mcu_init_cmd_msg(msg, CMD_RANDOM_WRITE, true, 0, true, true, 0, NULL, NULL);
		else
			mt7610u_mcu_init_cmd_msg(msg, CMD_RANDOM_WRITE, false, 0, false, false, 0, NULL, NULL);

		for (i = 0; i < (sent_len / 8); i++)
		{
			/* Address */
			value = cpu2le32(reg_pair[i + cur_index].Register + cap->WlanMemmapOffset);
			mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);

			/* UpdateData */
			value = cpu2le32(reg_pair[i + cur_index].Value);
			mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);
		};

		ret = mt7610u_mcu_send_cmd_msg(ad, msg);


		cur_index += (sent_len / 8);
		cur_len += cap->InbandPacketMaxLen;
	}

error:
	return ret;
}

int mt7610u_mcu_rf_random_write(struct rtmp_adapter*ad, BANK_RF_REG_PAIR *reg_pair, u32 num)
{
	struct cmd_msg *msg;
	unsigned int var_len = num * 8, cur_len = 0, sent_len;
	u32 value, i, cur_index = 0;
	struct rtmp_chip_cap *cap = &ad->chipCap;
	int ret = 0;
	bool last_packet = false;

	if (!reg_pair)
		return -1;

	while (cur_len < var_len) {
		sent_len = (var_len - cur_len) > cap->InbandPacketMaxLen
									? cap->InbandPacketMaxLen : (var_len - cur_len);

		if ((sent_len < cap->InbandPacketMaxLen) || (cur_len + cap->InbandPacketMaxLen) == var_len)
			last_packet = true;

		msg = mt7610u_mcu_alloc_cmd_msg(ad, sent_len);

		if (!msg) {
			ret = NDIS_STATUS_RESOURCES;
			goto error;
		}

		if (last_packet)
			mt7610u_mcu_init_cmd_msg(msg, CMD_RANDOM_WRITE, true, 0, true, true, 0, NULL, NULL);
		else
			mt7610u_mcu_init_cmd_msg(msg, CMD_RANDOM_WRITE, false, 0, false, false, 0, NULL, NULL);

		for (i = 0; i < (sent_len / 8); i++) {
			value = 0;
			/* RF selection */
			value = (value & ~0x80000000) | 0x80000000;

			/* RF bank */
			value = (value & ~0x00ff0000) | (reg_pair[i + cur_index].Bank << 16);

			/* RF Index */
			value = (value & ~0x000000ff) | reg_pair[i + cur_index].Register;

			value = cpu2le32(value);
			mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);

			value = 0;
			/* UpdateData */
			value = (value & ~0x000000ff) | reg_pair[i + cur_index].Value;
			value = cpu2le32(value);
			mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);
		}

		ret = mt7610u_mcu_send_cmd_msg(ad, msg);


		cur_index += (sent_len / 8);
		cur_len += cap->InbandPacketMaxLen;
	}

error:
	return ret;
}

int mt7610u_mcu_pwr_saving(struct rtmp_adapter*ad, u32 op, u32 level,
					 u32 listen_interval, u32 pre_tbtt_lead_time,
					 u8 tim_byte_offset, u8 tim_byte_pattern)
{
	struct cmd_msg *msg;
	unsigned int var_len;
	u32 value;
	int ret = 0;

	/* Power operation and Power Level */
	var_len = 8;

	if (op == RADIO_OFF_ADVANCE) {
		/* Listen interval, Pre-TBTT, TIM info */
		var_len += 12;
	}

	msg = mt7610u_mcu_alloc_cmd_msg(ad, var_len);

	if (!msg) {
		ret = NDIS_STATUS_RESOURCES;
		goto error;
	}

	mt7610u_mcu_init_cmd_msg(msg, CMD_POWER_SAVING_OP, false, 0, false, false, 0, NULL, NULL);

	/* Power operation */
	value = cpu2le32(op);
	mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);

	/* Power Level */
	value = cpu2le32(level);
	mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);

	if (op == RADIO_OFF_ADVANCE) {
		/* Listen interval */
		value = cpu2le32(listen_interval);
		mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);


		/* Pre TBTT lead time */
		value = cpu2le32(pre_tbtt_lead_time);
		mt7610u_mcu_append_cmd_msg(msg, (char*)&value, 4);

		/* TIM Info */
		value = (value & ~0x000000ff) | tim_byte_pattern;
		value = (value & ~0x0000ff00) | (tim_byte_offset << 8);
		value = cpu2le32(value);
		mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);
	}

	ret = mt7610u_mcu_send_cmd_msg(ad, msg);

error:
	return ret;
}

int mt7610u_mcu_fun_set(struct rtmp_adapter*ad, u32 fun_id, u32 param)
{
	struct cmd_msg *msg;
	u32 value;
	int ret = 0;

	/* Function ID and Parameter */
	msg = mt7610u_mcu_alloc_cmd_msg(ad, 8);

	if (!msg) {
		ret = NDIS_STATUS_RESOURCES;
		goto error;
	}

	if (fun_id != Q_SELECT)
		mt7610u_mcu_init_cmd_msg(msg, CMD_FUN_SET_OP, true, 0, true, true, 0, NULL, NULL);
	else
		mt7610u_mcu_init_cmd_msg(msg, CMD_FUN_SET_OP, false, 0, false, false, 0, NULL, NULL);

	/* Function ID */
	value = cpu2le32(fun_id);
	mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);

	/* Parameter */
	value = cpu2le32(param);
	mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);

	ret = mt7610u_mcu_send_cmd_msg(ad, msg);

error:
	return ret;
}

int mt7610u_mcu_calibration(struct rtmp_adapter*ad, u32 cal_id, u32 param)
{
	struct cmd_msg *msg;
	u32 value;
	int ret = 0;

	//DBGPRINT(RT_DEBUG_OFF, ("%s:cal_id(%d)\n", __FUNCTION__, cal_id));

	/* Calibration ID and Parameter */
	msg = mt7610u_mcu_alloc_cmd_msg(ad, 8);

	if (!msg) {
		ret = NDIS_STATUS_RESOURCES;
		goto error;
	}

	mt7610u_mcu_init_cmd_msg(msg, CMD_CALIBRATION_OP, true, 0, true, true, 0, NULL, NULL);

	/* Calibration ID */
	value = cpu2le32(cal_id);
	mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);

	/* Parameter */
	value = cpu2le32(param);
	mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);

	ret = mt7610u_mcu_send_cmd_msg(ad, msg);

error:
	return ret;
}

int mt7610u_mcu_led_op(struct rtmp_adapter*ad, u32 led_idx, u32 link_status)
{
	struct cmd_msg *msg;
	u32 value;
	int ret = 0;

	msg = mt7610u_mcu_alloc_cmd_msg(ad, 8);

	if (!msg) {
		ret = NDIS_STATUS_RESOURCES;
		goto error;
	}

	mt7610u_mcu_init_cmd_msg(msg, CMD_LED_MODE_OP, false, 0, false, false, 0, NULL, NULL);

	/* Led index */
	value = cpu2le32(led_idx);
	mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);

	/* Link status */
	value = cpu2le32(link_status);
	mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);

	ret = mt7610u_mcu_send_cmd_msg(ad, msg);

error:
	return ret;
}
