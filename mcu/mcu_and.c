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



/* Known USB Vendor Commands */
#define MT7610U_VENDOR_DEVICE_MODE	0x01
#define MT7610U_VENDOR_SINGLE_WRITE	0x02
#define MT7610U_VENDOR_WRITE_MAC	0x06
#define MT7610U_VENDOR_READ_MAC		0x07
#define MT7610U_VENDOR_WRITE_EEPROM	0x08	/* Not used */
#define MT7610U_VENDOR_READ_EEPROM	0x09

#define MT7610U_VENDOR_WRITE_FCE	0x42

void usb_uploadfw_complete(struct urb *urb)
{
	struct completion *load_fw_done = urb->context;

	complete(load_fw_done);
}

static int usb_load_ivb(struct rtmp_adapter *ad, u8 *fw_image)
{
	int status = NDIS_STATUS_SUCCESS;
	struct rtmp_chip_cap *cap = &ad->chipCap;


	if (cap->load_iv) {
		status = mt7610u_vendor_request(ad,
				 DEVICE_VENDOR_REQUEST_OUT,
				 MT7610U_VENDOR_DEVICE_MODE,
				 0x12, 0x00,
				 fw_image + 32, 64);
	} else {
		status = mt7610u_vendor_request(ad,
				 DEVICE_VENDOR_REQUEST_OUT,
				 MT7610U_VENDOR_DEVICE_MODE,
				 0x12, 0x00,
				 NULL, 0x00);
	}

	if (status)
		DBGPRINT(RT_DEBUG_ERROR, ("Upload IVB Fail\n"));

	return status;
}


/*
 *	========================================================================
 *
 *	Routine Description: Write Firmware to NIC.
 *
 *	Arguments:
 *
 *	Return Value:
 *
 *	IRQL =
 *
 *	Note:
 *
 *	========================================================================
 */

static void mt7610u_vendor_reset(struct rtmp_adapter *pAd)
{
	mt7610u_vendor_request(pAd,
		DEVICE_VENDOR_REQUEST_OUT,
		MT7610U_VENDOR_DEVICE_MODE,
		0x1, 0,
		NULL, 0);
}

int mt7610u_mcu_usb_loadfw(struct rtmp_adapter *ad)
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
	struct completion load_fw_done;
	USB_DMA_CFG_STRUC UsbCfg;
	u8 *fw_image = NULL;

	dev_info(&udev->dev, "loading firmware %s\n", cap->fw_name);

	ret = request_firmware(&fw, cap->fw_name, &udev->dev);
	if (ret) {
		dev_info(&udev->dev, "loading failed %s\n", cap->fw_name);
		return ret;
	}

	mdelay(5);

	fw_image = (u8 *) fw->data;

	dev_info(&udev->dev, "firmware %s loaded\n", cap->fw_name);

	if (cap->IsComboChip) {
loadfw_protect:
		mac_value = mt7610u_read32(ad, SEMAPHORE_00);
		loop++;

		if (((mac_value & 0x01) == 0) &&
		    (loop < GET_SEMAPHORE_RETRY_MAX)) {
			mdelay(1);
			goto loadfw_protect;
		}

		if (loop >= GET_SEMAPHORE_RETRY_MAX) {
			DBGPRINT(RT_DEBUG_ERROR, ("%s: can not get the hw semaphore\n", __FUNCTION__));
			return NDIS_STATUS_FAILURE;
		}
	}

	mt7610u_write32(ad, 0x1004, 0x2c);

	/* Enable USB_DMA_CFG */
	UsbCfg.word = mt7610u_read32(ad, USB_DMA_CFG);
	UsbCfg.field.RxBulkAggTOut = 0x20;
	UsbCfg.field.TxBulkEn = 1;
	UsbCfg.field.RxBulkEn = 1;
	mt7610u_write32(ad, USB_DMA_CFG, UsbCfg.word);
	//USB_CFG_WRITE(ad, 0x00c00020);

	/* Check MCU if ready */
	mac_value = mt7610u_read32(ad, COM_REG0);

	if (((mac_value & 0x01) == 0x01) &&
	    (cap->IsComboChip))
		goto error0;

	mt7610u_vendor_reset(ad);
	mdelay(5);

	/* Get FW information */
	ilm_len = le32_to_cpu(*((u32 *) (((u8 *) fw_image))));

	dlm_len = le32_to_cpu(*((u32 *) (((u8 *) fw_image) + 4)));

	fw_ver = le16_to_cpu(*((u16 *) (((u8 *) fw_image) + 10)));

	build_ver = le16_to_cpu(*((u16 *) (((u8 *) fw_image) + 8)));

	DBGPRINT(RT_DEBUG_OFF, ("fw version:%d.%d.%02d ", (fw_ver & 0xf000) >> 8,
						(fw_ver & 0x0f00) >> 8, fw_ver & 0x00ff));
	DBGPRINT(RT_DEBUG_OFF, ("build:%x\n", build_ver));
	DBGPRINT(RT_DEBUG_OFF, ("build time:"));

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
		UsbCfg.word = mt7610u_read32(ad, USB_DMA_CFG);

		cfg.field.UDMA_TX_WL_DROP = 1;

		mt7610u_write32(ad, USB_DMA_CFG, UsbCfg.word);

		cfg.field.UDMA_TX_WL_DROP = 0;

		mt7610u_write32(ad, USB_DMA_CFG, UsbCfg.word);
	}

	/* Allocate URB */
	urb = usb_alloc_urb(0, GFP_ATOMIC);

	if (!urb) {
		DBGPRINT(RT_DEBUG_ERROR, ("can not allocate URB\n"));
		ret = NDIS_STATUS_RESOURCES;
		goto error0;
	}

	/* Allocate TransferBuffer */
	fw_data = usb_alloc_coherent(udev, UPLOAD_FW_UNIT, GFP_ATOMIC, &fw_dma);

	if (!fw_data) {
		ret = NDIS_STATUS_RESOURCES;
		goto error1;
	}

	DBGPRINT(RT_DEBUG_OFF, ("loading fw"));

	init_completion(&load_fw_done);

	if (cap->load_iv)
		cur_len = 0x40;
	else
		cur_len = 0x00;

	/* Loading ILM */
	while (1) {
		s32 sent_len_max = UPLOAD_FW_UNIT - sizeof(*tx_info) - USB_END_PADDING;

		sent_len = ((ilm_len - cur_len) >=  sent_len_max) ?
				sent_len_max : (ilm_len - cur_len);

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
			ret = mt7610u_vendor_request(ad,
					 DEVICE_VENDOR_REQUEST_OUT,
					 MT7610U_VENDOR_WRITE_FCE,
					 value, 0x230,
					 NULL, 0);


			if (ret) {
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma descriptor fail\n"));
				goto error2;
			}

			value = (((cur_len + cap->ilm_offset) & 0xFFFF0000) >> 16);

			/* Set FCE DMA descriptor */
			ret = mt7610u_vendor_request(ad,
					 DEVICE_VENDOR_REQUEST_OUT,
					 MT7610U_VENDOR_WRITE_FCE,
					 value, 0x232,
					 NULL, 0);

			if (ret) {
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma descriptor fail\n"));
				goto error2;
			}

			cur_len += sent_len;

			while ((sent_len % 4) != 0)
				sent_len++;

			value = ((sent_len << 16) & 0xFFFF);

			/* Set FCE DMA length */
			ret = mt7610u_vendor_request(ad,
					 DEVICE_VENDOR_REQUEST_OUT,
					 MT7610U_VENDOR_WRITE_FCE,
					 value, 0x234,
					 NULL, 0);

			if (ret) {
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma length fail\n"));
				goto error2;
			}

			value = (((sent_len << 16) & 0xFFFF0000) >> 16);

			/* Set FCE DMA length */
			ret = mt7610u_vendor_request(ad,
					 DEVICE_VENDOR_REQUEST_OUT,
					 MT7610U_VENDOR_WRITE_FCE,
					 value, 0x236,
					 NULL, 0);

			if (ret) {
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma length fail\n"));
				goto error2;
			}

			/* Initialize URB descriptor */
			RTUSB_FILL_HTTX_BULK_URB(urb,
					 udev,
					 MT_COMMAND_BULK_OUT_ADDR,
					 fw_data,
					 sent_len + sizeof(*tx_info) + USB_END_PADDING,
					 usb_uploadfw_complete,
					 &load_fw_done,
					 fw_dma);

			ret = usb_submit_urb(urb, GFP_ATOMIC);

			if (ret) {
				DBGPRINT(RT_DEBUG_ERROR, ("submit urb fail\n"));
				goto error2;
			}

			if (!wait_for_completion_timeout(&load_fw_done,  msecs_to_jiffies(UPLOAD_FW_TIMEOUT))) {
				usb_kill_urb(urb);
				ret = NDIS_STATUS_FAILURE;
				DBGPRINT(RT_DEBUG_ERROR, ("upload fw timeout(%dms)\n",
					UPLOAD_FW_TIMEOUT));
				DBGPRINT(RT_DEBUG_ERROR, ("%s: submit urb, sent_len = %d, ilm_ilm = %d, cur_len = %d\n",
					__func__, sent_len, ilm_len, cur_len));

				goto error2;
			}
			DBGPRINT(RT_DEBUG_OFF, ("."));

			mac_value = mt7610u_read32(ad, TX_CPU_PORT_FROM_FCE_CPU_DESC_INDEX);
			mac_value++;
			mt7610u_write32(ad, TX_CPU_PORT_FROM_FCE_CPU_DESC_INDEX, mac_value);

			mdelay(5);
		} else {
			break;
		}

	}

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
			ret = mt7610u_vendor_request(ad,
					 DEVICE_VENDOR_REQUEST_OUT,
					 MT7610U_VENDOR_WRITE_FCE,
					 value, 0x230,
					 NULL, 0);


			if (ret) {
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma descriptor fail\n"));
				goto error2;
			}

			value = (((cur_len + cap->dlm_offset) & 0xFFFF0000) >> 16);

			/* Set FCE DMA descriptor */
			ret = mt7610u_vendor_request(ad,
					  DEVICE_VENDOR_REQUEST_OUT,
					  MT7610U_VENDOR_WRITE_FCE,
					  value, 0x232,
					  NULL, 0);

			if (ret) {
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma descriptor fail\n"));
				goto error2;
			}

			cur_len += sent_len;

			while ((sent_len % 4) != 0)
				sent_len++;

			value = ((sent_len << 16) & 0xFFFF);

			/* Set FCE DMA length */
			ret = mt7610u_vendor_request(ad,
					  DEVICE_VENDOR_REQUEST_OUT,
					  MT7610U_VENDOR_WRITE_FCE,
					  value, 0x234,
					  NULL, 0);

			if (ret) {
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma length fail\n"));
				goto error2;
			}

			value = (((sent_len << 16) & 0xFFFF0000) >> 16);

			/* Set FCE DMA length */
			ret = mt7610u_vendor_request(ad,
					  DEVICE_VENDOR_REQUEST_OUT,
					  MT7610U_VENDOR_WRITE_FCE,
					  value, 0x236,
					  NULL, 0);

			if (ret) {
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma length fail\n"));
				goto error2;
			}

			/* Initialize URB descriptor */
			RTUSB_FILL_HTTX_BULK_URB(urb,
					 udev,
					 MT_COMMAND_BULK_OUT_ADDR,
					 fw_data,
					 sent_len + sizeof(*tx_info) + USB_END_PADDING,
					 usb_uploadfw_complete,
					 &load_fw_done,
					 fw_dma);

			ret = usb_submit_urb(urb, GFP_ATOMIC);

			if (ret) {
				DBGPRINT(RT_DEBUG_ERROR, ("submit urb fail\n"));
				goto error2;
			}

			if (!wait_for_completion_timeout(&load_fw_done, msecs_to_jiffies(UPLOAD_FW_TIMEOUT))) {
				usb_kill_urb(urb);
				ret = NDIS_STATUS_FAILURE;
				DBGPRINT(RT_DEBUG_ERROR, ("upload fw timeout(%dms)\n", UPLOAD_FW_TIMEOUT));
				DBGPRINT(RT_DEBUG_INFO, ("%s: submit urb, sent_len = %d, dlm_len = %d, cur_len = %d\n", __func__, sent_len, dlm_len, cur_len));

				goto error2;
			}
			DBGPRINT(RT_DEBUG_OFF, ("."));

			mac_value = mt7610u_read32(ad, TX_CPU_PORT_FROM_FCE_CPU_DESC_INDEX);
			mac_value++;
			mt7610u_write32(ad, TX_CPU_PORT_FROM_FCE_CPU_DESC_INDEX, mac_value);
			mdelay(5);
		} else {
			break;
		}

	}

	/* Upload new 64 bytes interrupt vector or reset andes */
	DBGPRINT(RT_DEBUG_OFF, ("\n"));
	usb_load_ivb(ad, fw_image);

	/* Check MCU if ready */
	loop = 0;
	do {
		mac_value = mt7610u_read32(ad, COM_REG0);
		if ((mac_value & 0x01) == 0x01)
			break;
		mdelay(10);
		loop++;
	} while (loop <= 100);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: COM_REG0(0x%x) = 0x%x\n", __func__, COM_REG0, mac_value));

	if ((mac_value & 0x01) != 0x01)
		ret = NDIS_STATUS_FAILURE;

error2:
	/* Free TransferBuffer */
	usb_free_coherent(udev, UPLOAD_FW_UNIT, fw_data, fw_dma);

error1:
	/* Free URB */
	usb_free_urb(urb);

error0:
	release_firmware(fw);

	if (cap->IsComboChip)
		mt7610u_write32(ad, SEMAPHORE_00, 0x1);

	mt7610u_write32(ad, FCE_PSE_CTRL, 0x01);

	return ret;
}

static struct cmd_msg *mt7610u_mcu_alloc_cmd_msg(struct rtmp_adapter *ad, unsigned int length)
{
	struct cmd_msg *msg = NULL;
	struct rtmp_chip_cap *cap = &ad->chipCap;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;
	struct urb *urb = NULL;

	struct sk_buff *skb = dev_alloc_skb(cap->cmd_header_len + length + cap->cmd_padding_len);

	if (!skb) {
		DBGPRINT(RT_DEBUG_ERROR, ("can not allocate net_pkt\n"));
		goto error0;
	}

	skb_reserve(skb, cap->cmd_header_len);

	msg = kmalloc(sizeof(*msg), GFP_ATOMIC);

	if (!msg) {
		DBGPRINT(RT_DEBUG_ERROR, ("can not allocate cmd msg\n"));
		goto error1;
	}

	CMD_MSG_CB(skb)->msg = msg;

	memset(msg, 0x00, sizeof(*msg));

	urb = usb_alloc_urb(0, GFP_ATOMIC);

	if (!urb) {
		DBGPRINT(RT_DEBUG_ERROR, ("can not allocate urb\n"));
		goto error2;
	}

	msg->urb = urb;

	msg->priv = ad;
	msg->skb = skb;

	return msg;

error2:
	kfree(msg);
error1:
	dev_kfree_skb_any(skb);
error0:
	return NULL;
}

static void mt7610u_mcu_init_cmd_msg(struct cmd_msg *msg, enum mcu_cmd_type type,
				     bool need_wait,
				     bool need_retransmit, bool need_rsp,
				     u16 rsp_payload_len,
				     char *rsp_payload, MSG_RSP_HANDLER rsp_handler)
{
	msg->type = type;
	msg->need_wait = need_wait;

	if (need_wait)
		init_completion(&msg->ack_done);

	msg->need_rsp = need_rsp;
	msg->rsp_payload_len = rsp_payload_len;
	msg->rsp_payload = rsp_payload;
	msg->rsp_handler = rsp_handler;
}

static void mt7610u_mcu_append_cmd_msg(struct cmd_msg *msg, char *data, unsigned int len)
{
	struct sk_buff *skb = msg->skb;

	if (data)
		memcpy(skb_put(skb, len), data, len);
}

void mt7610u_mcu_free_cmd_msg(struct cmd_msg *msg)
{
	struct sk_buff *skb = msg->skb;
	struct rtmp_adapter *ad = msg->priv;

	usb_free_urb(msg->urb);
	kfree(msg);
	dev_kfree_skb_any(skb);
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
	else if (list == &ctl->rx_doneq)
		lock = &ctl->rx_doneq_lock;
	else
		DBGPRINT(RT_DEBUG_ERROR, ("%s:illegal list\n", __func__));

	return lock;
}

static inline u8 mt7610u_mcu_get_cmd_msg_seq(struct rtmp_adapter *ad)
{
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;
	struct cmd_msg *msg;

	spin_lock_irq(&ctl->ackq_lock);
get_seq:
	ctl->cmd_seq >= 0xf ? ctl->cmd_seq = 1 : ctl->cmd_seq++;
	DlListForEach(msg, &ctl->ackq, struct cmd_msg, list) {
		if (msg->seq == ctl->cmd_seq) {
			DBGPRINT(RT_DEBUG_ERROR, ("command(seq: %d) is still running\n", ctl->cmd_seq));
			goto get_seq;
		}
	}
	spin_unlock_irq(&ctl->ackq_lock);

	return ctl->cmd_seq;
}

static void mt7610u_mcu_queue_tail_cmd_msg(DL_LIST *list, struct cmd_msg *msg,
					   enum cmd_msg_state state)
{
	unsigned long flags;
	spinlock_t *lock;
	struct rtmp_adapter *ad = msg->priv;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;

	lock = mt7610u_mcu_get_spin_lock(ctl, list);

	spin_lock_irqsave(lock, flags);
	msg->state = state;
	DlListAddTail(list, &msg->list);
	spin_unlock_irqrestore(lock, flags);
}

static void mt7610u_mcu_queue_head_cmd_msg(DL_LIST *list, struct cmd_msg *msg,
					   enum cmd_msg_state state)
{
	unsigned long flags;
	spinlock_t *lock;
	struct rtmp_adapter *ad = msg->priv;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;

	lock = mt7610u_mcu_get_spin_lock(ctl, list);

	spin_lock_irqsave(lock, flags);
	msg->state = state;
	DlListAdd(list, &msg->list);
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
	struct rtmp_adapter *ad = msg->priv;
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

void mt7610u_mcu_rx_process_cmd_msg(struct rtmp_adapter *ad, struct cmd_msg *rx_msg)
{
	struct sk_buff *skb = rx_msg->skb;
	struct cmd_msg *msg, *msg_tmp;
	struct rxfce_info_cmd *rx_info = (struct rxfce_info_cmd *) skb->data;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;

#ifdef RT_BIG_ENDIAN
	RTMPDescriptorEndianChange((u8 *)rx_info, TYPE_RXINFO);
#endif


	if (rx_info->info_type != CMD_PACKET) {
		DBGPRINT(RT_DEBUG_ERROR, ("packet is not command response/self event\n"));
		return;
	}

	if (rx_info->self_gen) {
		/* if have callback function */
		RTEnqueueInternalCmd(ad, CMDTHREAD_RESPONSE_EVENT_CALLBACK,
				     skb->data + sizeof(*rx_info), rx_info->pkt_len);
	} else {
		spin_lock_irq(&ctl->ackq_lock);
		DlListForEachSafe(msg, msg_tmp, &ctl->ackq, struct cmd_msg, list) {
			if (msg->seq == rx_info->cmd_seq) {
				_mt7610u_mcu_unlink_cmd_msg(msg, &ctl->ackq);
				spin_unlock_irq(&ctl->ackq_lock);

				if ((msg->rsp_payload_len == rx_info->pkt_len) && (msg->rsp_payload_len != 0)) {
					msg->rsp_handler(msg, skb->data + sizeof(*rx_info), rx_info->pkt_len);
				} else if ((msg->rsp_payload_len == 0) && (rx_info->pkt_len == 8)) {
					DBGPRINT(RT_DEBUG_INFO, ("command response(ack) success\n"));
				} else {
					DBGPRINT(RT_DEBUG_ERROR, ("expect response len(%d), command response len(%d) invalid\n", msg->rsp_payload_len, rx_info->pkt_len));
					msg->rsp_payload_len = rx_info->pkt_len;
				}

				if (msg->need_wait)
					complete(&msg->ack_done);
				else
					mt7610u_mcu_free_cmd_msg(msg);

				spin_lock_irq(&ctl->ackq_lock);
				break;
			}
		}
		spin_unlock_irq(&ctl->ackq_lock);
	}
}

static void usb_rx_cmd_msg_complete(struct urb *urb)
{
	struct sk_buff *skb = urb->context;
	struct cmd_msg *msg = CMD_MSG_CB(skb)->msg;
	struct rtmp_adapter *ad = msg->priv;
	struct usb_device *udev = ad->OS_Cookie->pUsb_Dev;
	struct rtmp_chip_cap *pChipCap = &ad->chipCap;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;
	enum cmd_msg_state state;
	unsigned long flags;
	int ret = 0;

	mt7610u_mcu_unlink_cmd_msg(msg, &ctl->rxq);

	skb_put(skb, urb->actual_length);

	if (urb->status == 0) {
		state = RX_DONE;
	} else {
		state = RX_RECEIVE_FAIL;
		if (OS_TEST_BIT(MCU_INIT, &ctl->flags))
			ctl->rx_receive_fail_count++;

		DBGPRINT(RT_DEBUG_ERROR, ("receive cmd msg fail(%d)\n", urb->status));
	}

	mt7610u_mcu_queue_tail_cmd_msg(&ctl->rx_doneq, msg, state);

	if (OS_TEST_BIT(MCU_INIT, &ctl->flags)) {
		int pipe;
		msg = mt7610u_mcu_alloc_cmd_msg(ad, 512);

		if (!msg)
			return;

		skb = msg->skb;

		pipe = usb_rcvbulkpipe(udev, MT_COMMAND_RSP_BULK_IN_ADDR);
		usb_fill_bulk_urb(msg->urb, udev, pipe,
				  skb->data, 512,
				  usb_rx_cmd_msg_complete, skb);

		mt7610u_mcu_queue_tail_cmd_msg(&ctl->rxq, msg, RX_START);

		ret = usb_submit_urb(msg->urb, GFP_ATOMIC);

		if (ret) {
			mt7610u_mcu_unlink_cmd_msg(msg, &ctl->rxq);
			if (OS_TEST_BIT(MCU_INIT, &ctl->flags))
				ctl->rx_receive_fail_count++;

			DBGPRINT(RT_DEBUG_ERROR, ("%s:submit urb fail(%d)\n", __func__, ret));
			mt7610u_mcu_queue_tail_cmd_msg(&ctl->rx_doneq, msg, RX_RECEIVE_FAIL);
		}

	}

	mt7610u_mcu_bh_schedule(ad);
}

static int usb_rx_cmd_msg_submit(struct rtmp_adapter *ad)
{
	struct rtmp_chip_cap *pChipCap = &ad->chipCap;
	struct usb_device *udev = ad->OS_Cookie->pUsb_Dev;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;
	struct cmd_msg *msg = NULL;
	struct sk_buff *skb = NULL;
	int pipe;
	int ret = 0;

	if (!OS_TEST_BIT(MCU_INIT, &ctl->flags))
		return ret;

	msg =  mt7610u_mcu_alloc_cmd_msg(ad, 512);

	if (!msg) {
		ret = NDIS_STATUS_RESOURCES;
		return ret;
	}

	skb = msg->skb;

	pipe = usb_rcvbulkpipe(udev, MT_COMMAND_RSP_BULK_IN_ADDR);
	usb_fill_bulk_urb(msg->urb, udev, pipe,
			  skb->data, 512,
			  usb_rx_cmd_msg_complete, skb);

	mt7610u_mcu_queue_tail_cmd_msg(&ctl->rxq, msg, RX_START);

	ret = usb_submit_urb(msg->urb, GFP_ATOMIC);

	if (ret) {
		mt7610u_mcu_unlink_cmd_msg(msg, &ctl->rxq);
		if (OS_TEST_BIT(MCU_INIT, &ctl->flags))
			ctl->rx_receive_fail_count++;

		DBGPRINT(RT_DEBUG_ERROR, ("%s:submit urb fail(%d)\n", __func__, ret));
		mt7610u_mcu_queue_tail_cmd_msg(&ctl->rx_doneq, msg, RX_RECEIVE_FAIL);
	}

	return ret;
}

int usb_rx_cmd_msgs_receive(struct rtmp_adapter *ad)
{
	bool tmp;
	int ret = 0;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;

	tmp = mt7610u_mcu_queue_empty(ctl, &ctl->rx_doneq);
	if (!tmp)
		return ret;

	ret = usb_rx_cmd_msg_submit(ad);

	return ret;
}

void mt7610u_mcu_cmd_msg_bh(unsigned long param)
{
	struct rtmp_adapter *ad = (struct rtmp_adapter *)param;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;
	struct cmd_msg *msg = NULL;

	while (1) {
		msg = mt7610u_mcu_dequeue_cmd_msg(ctl, &ctl->rx_doneq);
		if (!msg)
			break;

		switch (msg->state) {
		case RX_DONE:
			mt7610u_mcu_rx_process_cmd_msg(ad, msg);
			mt7610u_mcu_free_cmd_msg(msg);
			continue;
		case RX_RECEIVE_FAIL:
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

void mt7610u_mcu_bh_schedule(struct rtmp_adapter *ad)
{
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;
	bool tmp;

	if (!OS_TEST_BIT(MCU_INIT, &ctl->flags))
		return;

	tmp = mt7610u_mcu_queue_empty(ctl, &ctl->rx_doneq);
	if (!tmp) {
		RTMP_NET_TASK_DATA_ASSIGN(&ctl->cmd_msg_task, (unsigned long)(ad));
		RTMP_OS_TASKLET_SCHE(&ctl->cmd_msg_task);
	}
}

static void usb_kick_out_cmd_msg_complete(struct urb *urb)
{
	struct sk_buff *skb = urb->context;
	struct cmd_msg *msg = CMD_MSG_CB(skb)->msg;
	struct rtmp_adapter *ad = msg->priv;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;

	if (!OS_TEST_BIT(MCU_INIT, &ctl->flags))
		return;

	if (urb->status == 0) {
		if (!msg->need_rsp) {
			mt7610u_mcu_unlink_cmd_msg(msg, &ctl->kickq);
		} else {
			msg->state = WAIT_ACK;
		}
	} else {
		if (!msg->need_rsp) {
			mt7610u_mcu_unlink_cmd_msg(msg, &ctl->kickq);
			if (OS_TEST_BIT(MCU_INIT, &ctl->flags))
				ctl->tx_kickout_fail_count++;
		} else {
			mt7610u_mcu_unlink_cmd_msg(msg, &ctl->ackq);
			msg->state = TX_KICKOUT_FAIL;
			if (OS_TEST_BIT(MCU_INIT, &ctl->flags))
				ctl->tx_kickout_fail_count++;
			complete(&msg->ack_done);
		}

		DBGPRINT(RT_DEBUG_ERROR, ("kick out cmd msg fail(%d)\n", urb->status));
	}

	mt7610u_mcu_bh_schedule(ad);
}

static int usb_kick_out_cmd_msg(struct rtmp_adapter *ad, struct cmd_msg *msg)
{
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;
	struct usb_device *udev = ad->OS_Cookie->pUsb_Dev;
	int pipe;
	int ret = 0;
	struct sk_buff *skb = msg->skb;
	struct rtmp_chip_cap *pChipCap = &ad->chipCap;

	/* append four zero bytes padding when usb aggregate enable */
	memset(skb_put(skb, 4), 0x00, 4);

	pipe = usb_sndbulkpipe(udev, MT_COMMAND_RSP_BULK_IN_ADDR),
	usb_fill_bulk_urb(msg->urb, udev, pipe,
			  skb->data, 512,
			  usb_kick_out_cmd_msg_complete, skb);

	if (msg->need_rsp)
		mt7610u_mcu_queue_tail_cmd_msg(&ctl->ackq, msg, WAIT_CMD_OUT_AND_ACK);
	else
		mt7610u_mcu_queue_tail_cmd_msg(&ctl->kickq, msg, WAIT_CMD_OUT);

	if (!OS_TEST_BIT(MCU_INIT, &ctl->flags))
		return -1;

	ret = usb_submit_urb(msg->urb, GFP_ATOMIC);

	if (ret) {
		if (!msg->need_rsp) {
			mt7610u_mcu_unlink_cmd_msg(msg, &ctl->kickq);
			if (OS_TEST_BIT(MCU_INIT, &ctl->flags))
				ctl->tx_kickout_fail_count++;
		} else {
			mt7610u_mcu_unlink_cmd_msg(msg, &ctl->ackq);
			msg->state = TX_KICKOUT_FAIL;
			if (OS_TEST_BIT(MCU_INIT, &ctl->flags))
				ctl->tx_kickout_fail_count++;
			complete(&msg->ack_done);
		}

		DBGPRINT(RT_DEBUG_ERROR, ("%s:submit urb fail(%d)\n", __func__, ret));
	}

	return ret;
}

void mt7610u_mcu_usb_unlink_urb(struct rtmp_adapter *ad, DL_LIST *list)
{
	unsigned long flags;
	struct cmd_msg *msg, *msg_tmp;
	spinlock_t *lock;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;

	lock = mt7610u_mcu_get_spin_lock(ctl, list);

	spin_lock_irqsave(lock, flags);
	DlListForEachSafe(msg, msg_tmp, list, struct cmd_msg, list) {
		spin_unlock_irqrestore(lock, flags);
		if ((msg->state == WAIT_CMD_OUT_AND_ACK) || (msg->state == WAIT_CMD_OUT) ||
						(msg->state == TX_START) || (msg->state == RX_START))
			usb_kill_urb(msg->urb);
		spin_lock_irqsave(lock, flags);
	}
	spin_unlock_irqrestore(lock, flags);
}

void mt7610u_mcu_cleanup_cmd_msg(struct rtmp_adapter *ad, DL_LIST *list)
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

void mt7610u_mcu_ctrl_init(struct rtmp_adapter *ad)
{
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;
	int ret = 0;

	ret = down_interruptible(&(ad->mcu_atomic));
	if (ret != 0) {
		DBGPRINT(RT_DEBUG_ERROR, ("reg_atomic get failed(ret=%d)\n", ret));
		return;
	}

	RTMP_CLEAR_FLAG(ad, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD);
	ctl->cmd_seq = 0;
	RTMP_OS_TASKLET_INIT(ad, &ctl->cmd_msg_task, mt7610u_mcu_cmd_msg_bh, (unsigned long)ad);

	DlListInit(&ctl->txq);
	DlListInit(&ctl->rxq);
	DlListInit(&ctl->ackq);
	DlListInit(&ctl->kickq);
	DlListInit(&ctl->rx_doneq);

	spin_lock_init(&ctl->txq_lock);
	spin_lock_init(&ctl->rxq_lock);
	spin_lock_init(&ctl->ackq_lock);
	spin_lock_init(&ctl->kickq_lock);
	spin_lock_init(&ctl->rx_doneq_lock);

	ctl->tx_kickout_fail_count = 0;
	ctl->tx_timeout_fail_count = 0;
	ctl->rx_receive_fail_count = 0;
	OS_SET_BIT(MCU_INIT, &ctl->flags);
	usb_rx_cmd_msgs_receive(ad);
	up(&(ad->mcu_atomic));
	ctl->power_on = false;
}

void mt7610u_mcu_ctrl_exit(struct rtmp_adapter *ad)
{
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;
	int ret = 0;

	ret = down_interruptible(&(ad->mcu_atomic));
	if (ret != 0) {
		DBGPRINT(RT_DEBUG_ERROR, ("reg_atomic get failed(ret=%d)\n", ret));
		return;
	}

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
	mt7610u_mcu_cleanup_cmd_msg(ad, &ctl->rx_doneq);
	DBGPRINT(RT_DEBUG_OFF, ("tx_kickout_fail_count = %ld\n", ctl->tx_kickout_fail_count));
	DBGPRINT(RT_DEBUG_OFF, ("tx_timeout_fail_count = %ld\n", ctl->tx_timeout_fail_count));
	DBGPRINT(RT_DEBUG_OFF, ("rx_receive_fail_count = %ld\n", ctl->rx_receive_fail_count));
	up(&(ad->mcu_atomic));
	ctl->power_on = false;
}

static int mt7610u_mcu_dequeue_and_kick_out_cmd_msgs(struct rtmp_adapter *ad)
{
	struct cmd_msg *msg = NULL;
	struct sk_buff *skb = NULL;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;
	int ret = NDIS_STATUS_SUCCESS;
	struct txinfo_nmac_cmd *tx_info;

	while (1) {
		bool tmp;

		msg = mt7610u_mcu_dequeue_cmd_msg(ctl, &ctl->txq);
		if (!msg)
			break;

		if (!RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD)
				|| RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_NIC_NOT_EXIST)
				|| RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_SUSPEND)) {
			if (!msg->need_rsp)
				mt7610u_mcu_free_cmd_msg(msg);
			continue;
		}

		tmp = mt7610u_mcu_queue_empty(ctl, &ctl->ackq);
		if (!tmp) {
			mt7610u_mcu_queue_head_cmd_msg(&ctl->txq, msg, msg->state);
			ret = NDIS_STATUS_FAILURE;
			continue;
		}

		skb = msg->skb;

		if (msg->need_rsp)
			msg->seq = mt7610u_mcu_get_cmd_msg_seq(ad);
		else
			msg->seq = 0;

		tx_info = (struct txinfo_nmac_cmd *)skb_push(skb, sizeof(*tx_info));
		tx_info->info_type = CMD_PACKET;
		tx_info->d_port = CPU_TX_PORT;
		tx_info->cmd_type = msg->type;
		tx_info->cmd_seq = msg->seq;
		tx_info->pkt_len = skb->len - sizeof(*tx_info);

#ifdef RT_BIG_ENDIAN
		RTMPDescriptorEndianChange((u8 *)tx_info, TYPE_TXINFO);
#endif

		ret = usb_kick_out_cmd_msg(ad, msg);

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

	ret = down_interruptible(&(ad->mcu_atomic));
	if (ret != 0) {
		DBGPRINT(RT_DEBUG_ERROR, ("reg_atomic get failed(ret=%d)\n", ret));
		return -1;
	}

	if (!RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD)
				|| RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_NIC_NOT_EXIST)
				|| RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_SUSPEND)) {
		mt7610u_mcu_free_cmd_msg(msg);
		up(&(ad->mcu_atomic));
		return NDIS_STATUS_FAILURE;
	}

	mt7610u_mcu_queue_tail_cmd_msg(&ctl->txq, msg, TX_START);

retransmit:
	mt7610u_mcu_dequeue_and_kick_out_cmd_msgs(ad);

	/* Wait for response */
	if (need_wait) {
		enum cmd_msg_state state;
		unsigned long expire;

		expire = msecs_to_jiffies(CMD_MSG_TIMEOUT);
		if (!wait_for_completion_timeout(&msg->ack_done, expire)) {
			ret = NDIS_STATUS_FAILURE;
			if (OS_TEST_BIT(MCU_INIT, &ctl->flags)) {
				if (msg->state == WAIT_CMD_OUT_AND_ACK)
					usb_kill_urb(msg->urb);
				else if (msg->state == WAIT_ACK)
					mt7610u_mcu_unlink_cmd_msg(msg, &ctl->ackq);
			}

			if (OS_TEST_BIT(MCU_INIT, &ctl->flags))
				ctl->tx_timeout_fail_count++;

			state = TX_TIMEOUT_FAIL;
		} else {
			if (msg->state == TX_KICKOUT_FAIL)
				state = TX_KICKOUT_FAIL;
			else
				state = TX_DONE;
		}

		if (OS_TEST_BIT(MCU_INIT, &ctl->flags))
			;
		else
			mt7610u_mcu_free_cmd_msg(msg);
	}

	up(&(ad->mcu_atomic));

	return ret;
}

int mt7610u_mcu_burst_write(struct rtmp_adapter *ad, u32 offset, u32 *data, u32 cnt)
{
	struct cmd_msg *msg;
	unsigned int var_len, offset_num, cur_len = 0, sent_len;
	u32 value, i, cur_index = 0;
	struct rtmp_chip_cap *cap = &ad->chipCap;
	int ret = 0;
	bool last_packet = false;

	if (!data)
		return -1;

	offset_num = cnt / ((MT_INBAND_PACKET_MAX_LEN - sizeof(offset)) / 4);

	if (cnt % ((MT_INBAND_PACKET_MAX_LEN - sizeof(offset)) / 4))
		var_len = sizeof(offset) * (offset_num + 1) + 4 * cnt;
	else
		var_len = sizeof(offset) * offset_num + 4 * cnt;

	while (cur_len < var_len) {
		sent_len = (var_len - cur_len) > MT_INBAND_PACKET_MAX_LEN
									? MT_INBAND_PACKET_MAX_LEN : (var_len - cur_len);

		if ((sent_len < MT_INBAND_PACKET_MAX_LEN) || ((cur_len + MT_INBAND_PACKET_MAX_LEN) == var_len))
			last_packet = true;

		msg = mt7610u_mcu_alloc_cmd_msg(ad, sent_len);

		if (!msg) {
			ret = NDIS_STATUS_RESOURCES;
			goto error;
		}

		if (last_packet)
			mt7610u_mcu_init_cmd_msg(msg, CMD_BURST_WRITE, true,
						 true, true, 0, NULL, NULL);
		else
			mt7610u_mcu_init_cmd_msg(msg, CMD_BURST_WRITE, false,
						false, false, 0, NULL, NULL);

		value = cpu2le32(offset + cap->WlanMemmapOffset + cur_index * 4);
		mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);

		for (i = 0; i < ((sent_len - 4) / 4); i++) {
			value = cpu2le32(data[i + cur_index]);
			mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);
		}

		ret = mt7610u_mcu_send_cmd_msg(ad, msg);


		cur_index += ((sent_len - 4) / 4);
		cur_len += MT_INBAND_PACKET_MAX_LEN;
	}

error:
	return ret;
}

static void mt7610u_mcu_rf_random_read_callback(struct cmd_msg *msg, char *rsp_payload, u16 rsp_payload_len)
{
	u32 i;
	BANK_RF_REG_PAIR *reg_pair = (BANK_RF_REG_PAIR *)msg->rsp_payload;

	for (i = 0; i < msg->rsp_payload_len / 8; i++)
		memmove(&reg_pair[i].Value, rsp_payload + 8 * i + 4, 1);
}

int mt7610u_mcu_rf_random_read(struct rtmp_adapter *ad, BANK_RF_REG_PAIR *reg_pair, u32 num)
{
	struct cmd_msg *msg;
	unsigned int var_len = num * 8, cur_len = 0, receive_len;
	u32 i, value, cur_index = 0;
	struct rtmp_chip_cap *cap = &ad->chipCap;
	int ret = 0;

	if (!reg_pair)
		return -1;

	while (cur_len < var_len) {
		receive_len =
			(var_len - cur_len) > MT_INBAND_PACKET_MAX_LEN ?
			MT_INBAND_PACKET_MAX_LEN :
			(var_len - cur_len);

		msg = mt7610u_mcu_alloc_cmd_msg(ad, receive_len);

		if (!msg) {
			ret = NDIS_STATUS_RESOURCES;
			goto error;
		}

		mt7610u_mcu_init_cmd_msg(msg, CMD_RANDOM_READ, true,
					 true, true, receive_len, (char *)&reg_pair[cur_index], mt7610u_mcu_rf_random_read_callback);

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
		cur_len += MT_INBAND_PACKET_MAX_LEN;
	}

error:
	return ret;
}

int mt7610u_mcu_random_write(struct rtmp_adapter *ad,
			struct rtmp_reg_pair *reg_pair, u32 num)
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
		sent_len =
			(var_len - cur_len) > MT_INBAND_PACKET_MAX_LEN ?
			MT_INBAND_PACKET_MAX_LEN :
			(var_len - cur_len);

		if ((sent_len < MT_INBAND_PACKET_MAX_LEN) ||
		    (cur_len + MT_INBAND_PACKET_MAX_LEN) == var_len)
			last_packet = true;

		msg = mt7610u_mcu_alloc_cmd_msg(ad, sent_len);

		if (!msg) {
			ret = NDIS_STATUS_RESOURCES;
			goto error;
		}

		if (last_packet)
			mt7610u_mcu_init_cmd_msg(msg, CMD_RANDOM_WRITE, true,
						 true, true, 0, NULL, NULL);
		else
			mt7610u_mcu_init_cmd_msg(msg, CMD_RANDOM_WRITE, false,
						 false, false, 0, NULL, NULL);

		for (i = 0; i < (sent_len / 8); i++) {
			/* Address */
			value = cpu2le32(reg_pair[i + cur_index].Register + cap->WlanMemmapOffset);
			mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);

			/* UpdateData */
			value = cpu2le32(reg_pair[i + cur_index].Value);
			mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);
		};

		ret = mt7610u_mcu_send_cmd_msg(ad, msg);

		cur_index += (sent_len / 8);
		cur_len += MT_INBAND_PACKET_MAX_LEN;
	}

error:
	return ret;
}

int mt7610u_mcu_rf_random_write(struct rtmp_adapter *ad, BANK_RF_REG_PAIR *reg_pair, u32 num)
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
		sent_len =
			(var_len - cur_len) > MT_INBAND_PACKET_MAX_LEN ?
			MT_INBAND_PACKET_MAX_LEN :
			(var_len - cur_len);

		if ((sent_len < MT_INBAND_PACKET_MAX_LEN) ||
		    (cur_len + MT_INBAND_PACKET_MAX_LEN) == var_len)
			last_packet = true;

		msg = mt7610u_mcu_alloc_cmd_msg(ad, sent_len);

		if (!msg) {
			ret = NDIS_STATUS_RESOURCES;
			goto error;
		}

		if (last_packet)
			mt7610u_mcu_init_cmd_msg(msg, CMD_RANDOM_WRITE, true,
						 true, true, 0, NULL, NULL);
		else
			mt7610u_mcu_init_cmd_msg(msg, CMD_RANDOM_WRITE, false,
						 false, false, 0, NULL, NULL);

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
		cur_len += MT_INBAND_PACKET_MAX_LEN;
	}

error:
	return ret;
}

int mt7610u_mcu_fun_set(struct rtmp_adapter *ad, u32 fun_id, u32 param)
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
		mt7610u_mcu_init_cmd_msg(msg, CMD_FUN_SET_OP, true,
					 true, true, 0, NULL, NULL);
	else
		mt7610u_mcu_init_cmd_msg(msg, CMD_FUN_SET_OP, false,
					 false, false, 0, NULL, NULL);

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

int mt7610u_mcu_calibration(struct rtmp_adapter *ad, enum mt7610u_mcu_calibration cal_id, u32 param)
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

	mt7610u_mcu_init_cmd_msg(msg, CMD_CALIBRATION_OP, true,
				 true, true, 0, NULL, NULL);

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

int mt7610u_mcu_led_op(struct rtmp_adapter *ad, u32 led_idx, u32 link_status)
{
	struct cmd_msg *msg;
	u32 value;
	int ret = 0;

	msg = mt7610u_mcu_alloc_cmd_msg(ad, 8);

	if (!msg) {
		ret = NDIS_STATUS_RESOURCES;
		goto error;
	}

	mt7610u_mcu_init_cmd_msg(msg, CMD_LED_MODE_OP, false,
				 false, false, 0, NULL, NULL);

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
