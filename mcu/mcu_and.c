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
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0))
#include <linux/bitfield.h>
#else
/* Force a compilation error if a constant expression is not a power of 2 */
#define __BUILD_BUG_ON_NOT_POWER_OF_2(n)	\
	BUILD_BUG_ON(((n) & ((n) - 1)) != 0)
#define BUILD_BUG_ON_NOT_POWER_OF_2(n)			\
	BUILD_BUG_ON((n) == 0 || (((n) & ((n) - 1)) != 0))
#include <bitfield.h>
#endif

static void mt7610u_mcu_bh_schedule(struct rtmp_adapter *ad);

struct mt7610u_dma_buf {
	struct urb *urb;
	void *buf;
	dma_addr_t dma;
	size_t len;
};

bool mt7610u_usb_alloc_buf(struct rtmp_adapter *ad, size_t len,
			   struct mt7610u_dma_buf *buf)
{
	struct usb_device *usb_dev = mt7610u_to_usb_dev(ad);

	buf->len = len;
	buf->urb = usb_alloc_urb(0, GFP_KERNEL);
	buf->buf = usb_alloc_coherent(usb_dev, buf->len, GFP_KERNEL, &buf->dma);

	return !buf->urb || !buf->buf;
}

void mt7610u_usb_free_buf(struct rtmp_adapter *ad, struct mt7610u_dma_buf *buf)
{
	struct usb_device *usb_dev = mt7610u_to_usb_dev(ad);

	usb_free_coherent(usb_dev, buf->len, buf->buf, buf->dma);
	usb_free_urb(buf->urb);
}

inline int mt7610u_dma_skb_wrap(struct sk_buff *skb,
				       enum D_PORT d_port,
				       enum INFO_TYPE type, u32 flags)
{
	u32 info;

	/* Buffer layout:
	 *	|   4B   | xfer len |      pad       |  4B  |
	 *	| TXINFO | pkt/cmd  | zero pad to 4B | zero |
	 *
	 * length field of TXINFO should be set to 'xfer len'.
	 */

	info = flags |
		FIELD_PREP(MT_TXD_INFO_LEN, round_up(skb->len, 4)) |
		FIELD_PREP(MT_TXD_INFO_D_PORT, d_port) |
		FIELD_PREP(MT_TXD_INFO_TYPE, type);

	put_unaligned_le32(info, skb_push(skb, sizeof(info)));
	return skb_put_padto(skb, round_up(skb->len, 4) + 4);
}

static inline void mt7610u_dma_skb_wrap_cmd(struct sk_buff *skb,
					    u8 seq, enum mcu_cmd_type cmd)
{
	WARN_ON(mt7610u_dma_skb_wrap(skb, CPU_TX_PORT, CMD_PACKET,
				     FIELD_PREP(MT_TXD_CMD_INFO_SEQ, seq) |
				     FIELD_PREP(MT_TXD_CMD_INFO_TYPE, cmd)));
}

static int usb_load_ivb(struct rtmp_adapter *ad, u8 *fw_image)
{
	int status = NDIS_STATUS_SUCCESS;
	struct rtmp_chip_cap *cap = &ad->chipCap;


	if (cap->load_iv) {
		status = mt7610u_vendor_request(ad,
				 DEVICE_VENDOR_REQUEST_OUT,
				 MT_VEND_DEVICE_MODE,
				 0x12, 0x00,
				 fw_image + 32, 64);
	} else {
		status = mt7610u_vendor_request(ad,
				 DEVICE_VENDOR_REQUEST_OUT,
				 MT_VEND_DEVICE_MODE,
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
		MT_VEND_DEVICE_MODE,
		0x1, 0,
		NULL, 0);
}

void mt7610u_complete_urb(struct urb *urb)
{
	struct completion *cmpl = urb->context;

	complete(cmpl);
}

static int __mt7610u_dma_fw(struct rtmp_adapter *ad,
			    const struct mt7610u_dma_buf *dma_buf,
			    const void *data, u32 len, u32 dst_addr)
{
	DECLARE_COMPLETION_ONSTACK(cmpl);
	struct mt7610u_dma_buf buf = *dma_buf; /* we need to fake length */
	struct usb_device *udev = mt7610u_to_usb_dev(ad);
	u16 value;
	u32 mac_value;
	int ret = 0;
	__le32 reg;

	reg = cpu_to_le32(FIELD_PREP(MT_TXD_INFO_TYPE, CMD_PACKET) |
			  FIELD_PREP(MT_TXD_INFO_D_PORT, CPU_TX_PORT) |
			  FIELD_PREP(MT_TXD_INFO_LEN, len));

	memmove(buf.buf, &reg, sizeof(reg));
	memmove(buf.buf + sizeof(reg), data, len);

	/* four zero bytes for end padding */
	memset(buf.buf + sizeof(reg) + len, 0, USB_END_PADDING);

	value = dst_addr & 0xFFFF;

	/* Set FCE DMA descriptor */
	ret = mt7610u_vendor_request(ad,
			 DEVICE_VENDOR_REQUEST_OUT,
			 MT_VEND_WRITE_FCE,
			 value, 0x230,
			 NULL, 0);


	if (ret) {
		DBGPRINT(RT_DEBUG_ERROR, ("set fce dma descriptor fail\n"));
		return ret;
	}

	value = ((dst_addr & 0xFFFF0000) >> 16);

	/* Set FCE DMA descriptor */
	ret = mt7610u_vendor_request(ad,
			 DEVICE_VENDOR_REQUEST_OUT,
			 MT_VEND_WRITE_FCE,
			 value, 0x232,
			 NULL, 0);

	if (ret) {
		DBGPRINT(RT_DEBUG_ERROR, ("set fce dma descriptor fail\n"));
		return ret;
	}

	len = roundup(len, 4);

	value = ((len << 16) & 0xFFFF);

	/* Set FCE DMA length */
	ret = mt7610u_vendor_request(ad,
			 DEVICE_VENDOR_REQUEST_OUT,
			 MT_VEND_WRITE_FCE,
			 value, 0x234,
			 NULL, 0);

	if (ret) {
		DBGPRINT(RT_DEBUG_ERROR, ("set fce dma length fail\n"));
		return ret;
	}

	value = (((len << 16) & 0xFFFF0000) >> 16);

	/* Set FCE DMA length */
	ret = mt7610u_vendor_request(ad,
			 DEVICE_VENDOR_REQUEST_OUT,
			 MT_VEND_WRITE_FCE,
			 value, 0x236,
			 NULL, 0);

	if (ret) {
		DBGPRINT(RT_DEBUG_ERROR, ("set fce dma length fail\n"));
		return ret;
	}

	/* Initialize URB descriptor */
	usb_fill_bulk_urb(buf.urb, udev,
			  usb_sndbulkpipe(udev, MT_COMMAND_BULK_OUT_ADDR),
			  buf.buf, len + sizeof(reg) + USB_END_PADDING,
			  mt7610u_complete_urb, &cmpl);
	buf.urb->transfer_dma = buf.dma;
	buf.urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

	ret = usb_submit_urb(buf.urb, GFP_ATOMIC);

	if (ret) {
		DBGPRINT(RT_DEBUG_ERROR, ("__mt7610u_dma_fw() submit urb fail\n"));

	}

	if (!wait_for_completion_timeout(&cmpl,  msecs_to_jiffies(UPLOAD_FW_TIMEOUT))) {
		usb_kill_urb(buf.urb);
		DBGPRINT(RT_DEBUG_ERROR, ("__mt7610u_dma_fw() timeout(%dms)\n",
			UPLOAD_FW_TIMEOUT));

		return ret;
	}
	DBGPRINT(RT_DEBUG_OFF, ("."));

	mac_value = mt7610u_read32(ad, TX_CPU_PORT_FROM_FCE_CPU_DESC_INDEX);
	mac_value++;
	mt7610u_write32(ad, TX_CPU_PORT_FROM_FCE_CPU_DESC_INDEX, mac_value);

	mdelay(5);

	return 0;
}

static int mt7610u_dma_fw(struct rtmp_adapter *ad,
			  const struct mt7610u_dma_buf *dma_buf,
			  const void *data, int len, u32 dst_addr)
{
	int pos = 0;
	int ret = 0;
	int sent_len_max = UPLOAD_PATCH_UNIT - MT_DMA_HDR_LEN - USB_END_PADDING;

	while (len > 0) {
		int sent_len = min(len, sent_len_max);

		DBGPRINT(RT_DEBUG_OFF, ("pos = %d\n", pos));
		DBGPRINT(RT_DEBUG_OFF, ("sent_len = %d\n", sent_len));

		__mt7610u_dma_fw(ad, dma_buf,
				data + pos, sent_len,
				dst_addr + pos);
		if (ret)
			return ret;


		pos += sent_len;
		len -= sent_len;

	}

	return ret;
}


int mt7610u_mcu_usb_loadfw(struct rtmp_adapter *ad)
{
	const struct firmware *fw;
	struct usb_device *udev = mt7610u_to_usb_dev(ad);
	struct mt7610u_dma_buf dma_buf;
	int pos = 0, ilm_len = 0, dlm_len = 0;
	u32 mac_value, loop = 0;
	int ret = 0;
	struct rtmp_chip_cap *cap = &ad->chipCap;
	u32 val;
	u16 fw_ver, build_ver;
	u8 *fw_image = NULL;
	int fw_chunk_len;

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
	val = MT_USB_DMA_CFG_RX_BULK_EN |
	      MT_USB_DMA_CFG_TX_BULK_EN |
	      FIELD_PREP(MT_USB_DMA_CFG_RX_BULK_AGG_TOUT, 0x20);
	mt7610u_write32(ad, USB_DMA_CFG, val);
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

	val = mt7610u_read32(ad, USB_DMA_CFG);

	val |= MT_USB_DMA_CFG_UDMA_TX_WL_DROP;
	mt7610u_write32(ad, USB_DMA_CFG, val);

	val &= ~MT_USB_DMA_CFG_UDMA_TX_WL_DROP;
	mt7610u_write32(ad, USB_DMA_CFG, val);

	/* Allocate TransferBuffer */
	if (mt7610u_usb_alloc_buf(ad, UPLOAD_FW_UNIT, &dma_buf)) {
		ret = -ENOMEM;
		goto error0;
	}

	DBGPRINT(RT_DEBUG_OFF, ("loading fw"));

	pos = (cap->load_iv) ? 0x40 : 0x00;
	fw_chunk_len = ilm_len - pos;

	/* Loading ILM */
	ret = mt7610u_dma_fw(ad, &dma_buf,
			     fw_image + FW_INFO_SIZE + pos, fw_chunk_len,
			     pos + cap->ilm_offset);

	if (ret < 0)
		goto  error2;

	pos = 0x00;
	fw_chunk_len = dlm_len - pos;

	/* Loading DLM */
	ret = mt7610u_dma_fw(ad, &dma_buf,
			     fw_image + FW_INFO_SIZE + ilm_len + pos, fw_chunk_len,
			     pos + cap->dlm_offset);

	if (ret < 0)
		goto  error2;

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
	mt7610u_usb_free_buf(ad, &dma_buf);

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
	struct urb *urb = NULL;

	/* ULLI :
	 * orignal drver used 4 bytes padding, we need 8 bytes due
	 * skb_panic in skb_put() mt7610u_dma_skb_wrap_cmd()
	 * wll check ths later on */
	struct sk_buff *skb = dev_alloc_skb(MT_DMA_HDR_LEN + length + 8);

	if (!skb) {
		DBGPRINT(RT_DEBUG_ERROR, ("can not allocate net_pkt\n"));
		goto error0;
	}

	skb_reserve(skb, MT_DMA_HDR_LEN);

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

static void mt7610u_mcu_free_cmd_msg(struct cmd_msg *msg)
{
	struct sk_buff *skb = msg->skb;

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

	for (msg = DlListEntry((&ctl->ackq)->Next, struct cmd_msg, list);
		&msg->list != &ctl->ackq;
		msg = DlListEntry(msg->list.Next, struct cmd_msg, list)) {

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
	struct mt7610u_rxfce_info_cmd *rx_info = (struct mt7610u_rxfce_info_cmd *) skb->data;
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

		for (msg = DlListEntry((&ctl->ackq)->Next, struct cmd_msg, list), msg_tmp = DlListEntry(msg->list.Next, struct cmd_msg, list);
			&msg->list != &ctl->ackq;
			msg = msg_tmp, msg_tmp = DlListEntry(msg_tmp->list.Next, struct cmd_msg, list)) {

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
	struct usb_device *udev = mt7610u_to_usb_dev(ad);
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;
	enum cmd_msg_state state;
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
	struct usb_device *udev = mt7610u_to_usb_dev(ad);
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

static void mt7610u_mcu_cmd_msg_bh(unsigned long param)
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

static void mt7610u_mcu_bh_schedule(struct rtmp_adapter *ad)
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
	struct usb_device *udev = mt7610u_to_usb_dev(ad);
	int pipe;
	int ret = 0;
	struct sk_buff *skb = msg->skb;

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

static void mt7610u_mcu_usb_unlink_urb(struct rtmp_adapter *ad, DL_LIST *list)
{
	unsigned long flags;
	struct cmd_msg *msg, *msg_tmp;
	spinlock_t *lock;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;

	lock = mt7610u_mcu_get_spin_lock(ctl, list);

	spin_lock_irqsave(lock, flags);

	for (msg = DlListEntry(list->Next, struct cmd_msg, list), msg_tmp = DlListEntry(msg->list.Next, struct cmd_msg, list);
		&msg->list != &ctl->ackq;
		msg = msg_tmp, msg_tmp = DlListEntry(msg_tmp->list.Next, struct cmd_msg, list)) {

		spin_unlock_irqrestore(lock, flags);
		if ((msg->state == WAIT_CMD_OUT_AND_ACK) || (msg->state == WAIT_CMD_OUT) ||
						(msg->state == TX_START) || (msg->state == RX_START))
			usb_kill_urb(msg->urb);
		spin_lock_irqsave(lock, flags);
	}
	spin_unlock_irqrestore(lock, flags);
}

static void mt7610u_mcu_cleanup_cmd_msg(struct rtmp_adapter *ad, DL_LIST *list)
{
	unsigned long flags;
	struct cmd_msg *msg, *msg_tmp;
	spinlock_t *lock;
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;

	lock = mt7610u_mcu_get_spin_lock(ctl, list);

	spin_lock_irqsave(lock, flags);

	for (msg = DlListEntry(list->Next, struct cmd_msg, list), msg_tmp = DlListEntry(msg->list.Next, struct cmd_msg, list);
		&msg->list != &ctl->ackq;
		msg = msg_tmp, msg_tmp = DlListEntry(msg_tmp->list.Next, struct cmd_msg, list)) {

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

	while (1) {
		bool tmp;

		msg = mt7610u_mcu_dequeue_cmd_msg(ctl, &ctl->txq);
		if (!msg)
			break;

		if (RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_NIC_NOT_EXIST) ||
		    RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_SUSPEND)) {
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

		mt7610u_dma_skb_wrap_cmd(skb, msg->seq, msg->type);

		ret = usb_kick_out_cmd_msg(ad, msg);

		if (ret) {
			DBGPRINT(RT_DEBUG_ERROR, ("kick out msg fail\n"));
			break;
		}
	}

	mt7610u_mcu_bh_schedule(ad);

	return ret;
}

static int mt7610u_mcu_send_cmd_msg(struct rtmp_adapter *ad, struct cmd_msg *msg)
{
	struct mt7610u_mcu_ctrl *ctl = &ad->MCUCtrl;
	int ret = 0;
	bool need_wait = msg->need_wait;

	ret = down_interruptible(&(ad->mcu_atomic));
	if (ret != 0) {
		DBGPRINT(RT_DEBUG_ERROR, ("reg_atomic get failed(ret=%d)\n", ret));
		return -1;
	}

	if (RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_NIC_NOT_EXIST) ||
	    RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_SUSPEND)) {
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
	unsigned int var_len, offset_num, pos = 0, sent_len;
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

	while (pos < var_len) {
		sent_len = (var_len - pos) > MT_INBAND_PACKET_MAX_LEN
									? MT_INBAND_PACKET_MAX_LEN : (var_len - pos);

		if ((sent_len < MT_INBAND_PACKET_MAX_LEN) || ((pos + MT_INBAND_PACKET_MAX_LEN) == var_len))
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
		pos += MT_INBAND_PACKET_MAX_LEN;
	}

error:
	return ret;
}

static void mt7610u_mcu_rf_random_read_callback(struct cmd_msg *msg, char *rsp_payload, u16 rsp_payload_len)
{
	u32 i;
	struct mt7610u_rf_entry *reg_pair =
		(struct mt7610u_rf_entry *)msg->rsp_payload;

	/* ULLI : horrible ..., the magic value 4 is the size of the rxfce/cmd header */

	for (i = 0; i < msg->rsp_payload_len / 8; i++)
		memmove(&reg_pair[i].Value, rsp_payload + 8 * i + 4, 1);
}

#define MT_RF_SELECT	BIT(31)
#define MT_RF_BANK	GENMASK(23, 16)
#define MT_RF_INDEX	GENMASK(15, 0)

int mt7610u_mcu_rf_random_read(struct rtmp_adapter *ad, struct mt7610u_rf_entry *reg_pair, u32 num)
{
	struct cmd_msg *msg;
	unsigned int var_len = num * 8, pos = 0, receive_len;
	u32 i, value, cur_index = 0;
	int ret = 0;

	if (!reg_pair)
		return -1;

	while (pos < var_len) {
		receive_len =
			(var_len - pos) > MT_INBAND_PACKET_MAX_LEN ?
			MT_INBAND_PACKET_MAX_LEN :
			(var_len - pos);

		msg = mt7610u_mcu_alloc_cmd_msg(ad, receive_len);

		if (!msg) {
			ret = NDIS_STATUS_RESOURCES;
			goto error;
		}

		mt7610u_mcu_init_cmd_msg(msg, CMD_RANDOM_READ, true,
					 true, true, receive_len, (char *)&reg_pair[cur_index], mt7610u_mcu_rf_random_read_callback);

		for (i = 0; i < (receive_len) / 8; i++) {
			value  = cpu2le32(MT_RF_SELECT |
					  FIELD_PREP(MT_RF_BANK, reg_pair[i + cur_index].Bank) |
					  FIELD_PREP(MT_RF_INDEX, reg_pair[i + cur_index].Register)
				 );

			mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);
			value = 0;
			mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);
		}

		ret = mt7610u_mcu_send_cmd_msg(ad, msg);

		cur_index += receive_len / 8;
		pos += MT_INBAND_PACKET_MAX_LEN;
	}

error:
	return ret;
}

int mt7610u_mcu_random_write(struct rtmp_adapter *ad,
			struct rtmp_reg_pair *reg_pair, u32 num)
{
	struct cmd_msg *msg;
	unsigned int var_len = num * 8, pos = 0, sent_len;
	u32 value, i, cur_index = 0;
	struct rtmp_chip_cap *cap = &ad->chipCap;
	int ret = 0;
	bool last_packet = false;

	if (!reg_pair)
		return -1;

	while (pos < var_len) {
		sent_len =
			(var_len - pos) > MT_INBAND_PACKET_MAX_LEN ?
			MT_INBAND_PACKET_MAX_LEN :
			(var_len - pos);

		if ((sent_len < MT_INBAND_PACKET_MAX_LEN) ||
		    (pos + MT_INBAND_PACKET_MAX_LEN) == var_len)
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
		pos += MT_INBAND_PACKET_MAX_LEN;
	}

error:
	return ret;
}

int mt7610u_mcu_rf_random_write(struct rtmp_adapter *ad, struct mt7610u_rf_entry *reg_pair, u32 num)
{
	struct cmd_msg *msg;
	unsigned int var_len = num * 8, pos = 0, sent_len;
	u32 value, i, cur_index = 0;
	int ret = 0;
	bool last_packet = false;

	if (!reg_pair)
		return -1;

	while (pos < var_len) {
		sent_len =
			(var_len - pos) > MT_INBAND_PACKET_MAX_LEN ?
			MT_INBAND_PACKET_MAX_LEN :
			(var_len - pos);

		if ((sent_len < MT_INBAND_PACKET_MAX_LEN) ||
		    (pos + MT_INBAND_PACKET_MAX_LEN) == var_len)
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
			value  = cpu2le32(MT_RF_SELECT |
					  FIELD_PREP(MT_RF_BANK, reg_pair[i + cur_index].Bank) |
					  FIELD_PREP(MT_RF_INDEX, reg_pair[i + cur_index].Register)
				 );

			mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);

			/* UpdateData */
			value = reg_pair[i + cur_index].Value & 0xff;
			value = cpu2le32(value);
			mt7610u_mcu_append_cmd_msg(msg, (char *)&value, 4);
		}

		ret = mt7610u_mcu_send_cmd_msg(ad, msg);

		cur_index += (sent_len / 8);
		pos += MT_INBAND_PACKET_MAX_LEN;
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
