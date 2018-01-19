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

static void mt7610u_disbale_txrx(struct rtmp_adapter*pAd);

void mt7610u_radio_off(struct rtmp_adapter*pAd, u8 Stage)
{
	u32 ret;

	DBGPRINT(RT_DEBUG_TRACE, ("--> %s\n", __FUNCTION__));

	mt7610u_disbale_txrx(pAd);

	ret = down_interruptible(&pAd->hw_atomic);
	if (ret != 0) {
		DBGPRINT(RT_DEBUG_ERROR, ("reg_atomic get failed(ret=%d)\n", ret));
		return;
	}

	RTMP_SET_PSFLAG(pAd, fRTMP_PS_MCU_SLEEP);

	mt7610u_mcu_ctrl_exit(pAd);
	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);
	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF);

	/* Stop bulkin pipe*/
	//if((pAd->PendingRx > 0) && (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
	if((!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
	{
		RTUSBCancelPendingBulkInIRP(pAd);
		//pAd->PendingRx = 0;
	}

	up(&pAd->hw_atomic);

	DBGPRINT(RT_DEBUG_TRACE, ("<== %s\n", __FUNCTION__));
}


void mt7610u_radio_on(struct rtmp_adapter*pAd, u8 Stage)
{
	u32 MACValue = 0;
	u32 rx_filter_flag;
	u32 ret;

	RTMP_CLEAR_PSFLAG(pAd, fRTMP_PS_MCU_SLEEP);

	if (!(pAd->WlanFunCtrl & MT_WLAN_FUN_CTRL_WLAN_EN))
		mt7610u_chip_onoff(pAd, true, false);

	/* make some traffic to invoke EvtDeviceD0Entry callback function*/
	MACValue = mt76u_reg_read(pAd,0x1000);
	DBGPRINT(RT_DEBUG_TRACE,("A MAC query to invoke EvtDeviceD0Entry, MACValue = 0x%x\n",MACValue));

	rx_filter_flag = STANORMAL;     /* Staion not drop control frame will fail WiFi Certification.*/

	mt76u_reg_write(pAd, RX_FILTR_CFG, rx_filter_flag);
	mt76u_reg_write(pAd, MAC_SYS_CTRL, 0x0c);

	/* 4. Clear idle flag*/
	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF);
	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);
	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_SUSPEND);

	/* Send Bulkin IRPs after flag fRTMP_ADAPTER_IDLE_RADIO_OFF is cleared.*/
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		RTUSBBulkReceive(pAd);
	}
#endif /* CONFIG_STA_SUPPORT */

	mt7610u_mcu_ctrl_init(pAd);

	ret = down_interruptible(&pAd->hw_atomic);
	if (ret != 0) {
		DBGPRINT(RT_DEBUG_ERROR, ("reg_atomic get failed(ret=%d)\n", ret));
		return;
	}

	mt76u_reg_write(pAd, MAC_SYS_CTRL, 0x0c);

	up(&pAd->hw_atomic);

	DBGPRINT(RT_DEBUG_TRACE, ("<== %s\n", __FUNCTION__));
}

/* ULLI : Warning
 * We can use mt7612u functions here, there is some other IP on
 * the USB3 interface which uses some other vendor commands */

static void mt7610u_stop_dma_rx(struct rtmp_adapter*pAd)
{
	struct sk_buff *	skb;
	RX_BLK			RxBlk, *pRxBlk;
	u32 RxPending = 0, MacReg = 0, MTxCycle = 0;
	bool bReschedule = false;
	bool bCmdRspPacket = false;
	u8 IdleNums = 0;

	/*
		process whole rx ring
	*/
	while (1) {
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
			return;
		pRxBlk = &RxBlk;
		skb = GetPacketFromRxRing(pAd, pRxBlk, &bReschedule, &RxPending, &bCmdRspPacket);
		if ((RxPending == 0) && (bReschedule == false))
			break;
		else
			dev_kfree_skb_any(skb);
	}

	/*
		Check DMA Rx idle
	*/
	for (MTxCycle = 0; MTxCycle < 2000; MTxCycle++) {

		MacReg = mt76u_reg_read(pAd, USB_DMA_CFG);
		//mt76u_reg_read(pAd, USB_DMA_CFG, &MacReg);
		if ((MacReg & 0x40000000) && (IdleNums < 10)) {
			IdleNums++;
			udelay(50);
		} else {
			break;
		}

		if (MacReg == 0xFFFFFFFF) {
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST);
			return;
		}
	}

	if (MTxCycle >= 2000)
		DBGPRINT(RT_DEBUG_ERROR, ("%s:RX DMA busy!! DMA_CFG = 0x%08x\n", __FUNCTION__, MacReg));

}

static void mt7610u_stop_dma_tx(struct rtmp_adapter*pAd)
{
	u32 MacReg = 0, MTxCycle = 0;
	u8 IdleNums = 0;

	for (MTxCycle = 0; MTxCycle < 2000; MTxCycle++) {
		MacReg = mt76u_reg_read(pAd, USB_DMA_CFG);
		//mt76u_reg_read(pAd, USB_DMA_CFG, &MacReg);
		if (((MacReg & 0x80000000) == 0) && IdleNums > 10) {
			break;
		} else {
			IdleNums++;
			udelay(50);
		}

		if (MacReg == 0xFFFFFFFF) {
			//RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST);
			return;
		}
	}

	if (MTxCycle >= 2000)
		DBGPRINT(RT_DEBUG_ERROR, ("TX DMA busy!! DMA_CFG(%x)\n", MacReg));

}

void mt7610u_disbale_txrx(struct rtmp_adapter*pAd)
{
	u32 MacReg = 0;
	u32 MTxCycle;
	bool bResetWLAN = false;
	bool bFree = true;
	u8 CheckFreeTimes = 0;

	DBGPRINT(RT_DEBUG_TRACE, ("----> %s\n", __FUNCTION__));

	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_ACTIVE);

	DBGPRINT(RT_DEBUG_TRACE, ("%s Tx success = %ld\n",
		__FUNCTION__, (ULONG)pAd->WlanCounters.TransmittedFragmentCount.u.LowPart));
	DBGPRINT(RT_DEBUG_TRACE, ("%s Rx success = %ld\n",
		__FUNCTION__, (ULONG)pAd->WlanCounters.ReceivedFragmentCount.QuadPart));

	mt7610u_stop_dma_tx(pAd);

	/*
		Check page count in TxQ,
	*/
	for (MTxCycle = 0; MTxCycle < 2000; MTxCycle++) {
		bFree = true;
		MacReg = mt76u_reg_read(pAd, 0x438);
		if (MacReg != 0)
			bFree = false;
		MacReg = mt76u_reg_read(pAd, 0xa30);
		if (MacReg & 0x000000FF)
			bFree = false;
		MacReg = mt76u_reg_read(pAd, 0xa34);
		if (MacReg & 0xFF00FF00)
			bFree = false;
		if (bFree)
			break;
		if (MacReg == 0xFFFFFFFF) {
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST);
			return;
		}
	}

	if (MTxCycle >= 2000) {
		DBGPRINT(RT_DEBUG_ERROR, ("Check TxQ page count max\n"));
		MacReg = mt76u_reg_read(pAd, 0x0a30);
		DBGPRINT(RT_DEBUG_TRACE, ("0x0a30 = 0x%08x\n", MacReg));

		MacReg = mt76u_reg_read(pAd, 0x0a34);
		DBGPRINT(RT_DEBUG_TRACE, ("0x0a34 = 0x%08x\n", MacReg));

		MacReg = mt76u_reg_read(pAd, 0x438);
		DBGPRINT(RT_DEBUG_TRACE, ("0x438 = 0x%08x\n", MacReg));
		bResetWLAN = true;
	}

	/*
		Check MAC Tx idle
	*/
	for (MTxCycle = 0; MTxCycle < 2000; MTxCycle++) {
		MacReg = mt76u_reg_read(pAd, MAC_STATUS_CFG);
		if (MacReg & 0x1)
			udelay(50);
		else
			break;

		if (MacReg == 0xFFFFFFFF) {
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST);
			return;
		}
	}

	if (MTxCycle >= 2000) {
		DBGPRINT(RT_DEBUG_ERROR, ("Check MAC Tx idle max(0x%08x)\n", MacReg));
		bResetWLAN = true;
	}

	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST) == false) {
			/*
				Disable MAC TX/RX
			*/
			MacReg = mt76u_reg_read(pAd, MAC_SYS_CTRL);
			MacReg &= ~(0x0000000c);
			mt76u_reg_write(pAd, MAC_SYS_CTRL, MacReg);
	}

	/*
		Check page count in RxQ,
	*/
	for (MTxCycle = 0; MTxCycle < 2000; MTxCycle++) {
		bFree = true;
		MacReg = mt76u_reg_read(pAd, 0x430);

		if (MacReg & (0x00FF0000))
			bFree = false;

		MacReg = mt76u_reg_read(pAd, 0xa30);

		if (MacReg != 0)
			bFree = false;

		MacReg = mt76u_reg_read(pAd, 0xa34);

		if (MacReg != 0)
			bFree = false;

		if (bFree && (CheckFreeTimes > 20))
			break;

		if (bFree)
			CheckFreeTimes++;

		if (MacReg == 0xFFFFFFFF) {
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST);
			return;
		}
		RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_POLL_IDLE);
		usb_rx_cmd_msgs_receive(pAd);
		RTUSBBulkReceive(pAd);
	}

	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_POLL_IDLE);

	if (MTxCycle >= 2000) {
		DBGPRINT(RT_DEBUG_ERROR, ("Check RxQ page count max\n"));

		MacReg = mt76u_reg_read(pAd, 0x0a30);
		DBGPRINT(RT_DEBUG_TRACE, ("0x0a30 = 0x%08x\n", MacReg));

		MacReg = mt76u_reg_read(pAd, 0x0a34);
		DBGPRINT(RT_DEBUG_TRACE, ("0x0a34 = 0x%08x\n", MacReg));

		MacReg = mt76u_reg_read(pAd, 0x0430);
		DBGPRINT(RT_DEBUG_TRACE, ("0x0430 = 0x%08x\n", MacReg));
		bResetWLAN = true;
	}

	/*
		Check MAC Rx idle
	*/
	for (MTxCycle = 0; MTxCycle < 2000; MTxCycle++) {
		MacReg = mt76u_reg_read(pAd, MAC_STATUS_CFG);
		if (MacReg & 0x2)
			udelay(50);
		else
			break;
		if (MacReg == 0xFFFFFFFF) {
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST);
			return;
		}
	}

	if (MTxCycle >= 2000) {
		DBGPRINT(RT_DEBUG_ERROR, ("Check MAC Rx idle max(0x%08x)\n", MacReg));
		bResetWLAN = true;
	}

	mt7610u_stop_dma_rx(pAd);

	if ((RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST) == false)) {

		/*
 		 * Disable RF/MAC and do not do reset WLAN under below cases
 		 * 1. Combo card
 		 * 2. suspend including wow application
 		 * 3. radion off command
 		 */
		if ((pAd->chipCap.IsComboChip) ||
		    RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_SUSPEND) ||
		    RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_CMD_RADIO_OFF))
			bResetWLAN = 0;

		mt7610u_chip_onoff(pAd, false, bResetWLAN);
	}

	DBGPRINT(RT_DEBUG_TRACE, ("<---- %s\n", __FUNCTION__));
}

void mt7610u_chip_onoff(struct rtmp_adapter *pAd, bool enable, bool reset)
{
	u32 WlanFunCtrl;
	u32 ret;

	ret = down_interruptible(&pAd->wlan_en_atomic);
	if (ret != 0) {
		DBGPRINT(RT_DEBUG_ERROR, ("wlan_en_atomic get failed(ret=%d)\n", ret));
		return;
	}

	WlanFunCtrl = mt76u_reg_read(pAd, MT_WLAN_FUN_CTRL);
	DBGPRINT(RT_DEBUG_OFF, ("==>%s(): OnOff:%d, Reset= %d, pAd->WlanFunCtrl:0x%x, Reg-WlanFunCtrl=0x%x\n",
				__FUNCTION__, enable, reset, pAd->WlanFunCtrl, WlanFunCtrl));

	if (reset == true) {
		WlanFunCtrl |= MT_WLAN_FUN_CTRL_GPIO_OUT_EN;
		WlanFunCtrl &= ~MT_WLAN_FUN_CTRL_INV_ANT_SEL;

		if (pAd->WlanFunCtrl & MT_WLAN_FUN_CTRL_WLAN_EN) {
			/*
				Restore all HW default value and reset RF.
			*/
			WlanFunCtrl |= MT_WLAN_FUN_CTRL_WLAN_RESET;
			WlanFunCtrl |= MT_WLAN_FUN_CTRL_WLAN_RESET_RF;
			DBGPRINT(RT_DEBUG_TRACE, ("Reset(1) WlanFunCtrl = 0x%x\n", WlanFunCtrl));
			mt76u_reg_write(pAd, MT_WLAN_FUN_CTRL, WlanFunCtrl);
			udelay(20);
			WlanFunCtrl &= ~MT_WLAN_FUN_CTRL_WLAN_RESET;
			WlanFunCtrl &= ~MT_WLAN_FUN_CTRL_WLAN_RESET_RF;
			DBGPRINT(RT_DEBUG_TRACE, ("Reset(2) WlanFunCtrl = 0x%x\n", WlanFunCtrl));
			mt76u_reg_write(pAd, MT_WLAN_FUN_CTRL, WlanFunCtrl);
			udelay(20);
		}
	}

	if (enable == true) {
		/*
			Enable WLAN function and clock
			WLAN_FUN_CTRL[1:0] = 0x3
		*/
		WlanFunCtrl |= MT_WLAN_FUN_CTRL_WLAN_CLK_EN;
		WlanFunCtrl |= MT_WLAN_FUN_CTRL_WLAN_EN;
	} else {
		/*
			Diable WLAN function and clock
			WLAN_FUN_CTRL[1:0] = 0x0
		*/
		WlanFunCtrl &= ~MT_WLAN_FUN_CTRL_PCIE_CLK_REQ;
		WlanFunCtrl &= ~MT_WLAN_FUN_CTRL_WLAN_CLK_EN;
		WlanFunCtrl &= ~MT_WLAN_FUN_CTRL_WLAN_EN;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("WlanFunCtrl = 0x%x\n", WlanFunCtrl));
	mt76u_reg_write(pAd, MT_WLAN_FUN_CTRL, WlanFunCtrl);
	udelay(20);

	if (enable) {
		pAd->mac_rev = mt76u_reg_read(pAd, MT_MAC_CSR0);
		DBGPRINT(RT_DEBUG_OFF, ("mac_rev = 0x%08x\n", pAd->mac_rev));
	}

	if (enable == true) {
		UINT index = 0;
		CMB_CTRL_STRUC CmbCtrl;

		CmbCtrl.word = 0;

		do {
			do {
				CmbCtrl.word = mt76u_reg_read(pAd, CMB_CTRL);

				/*
					Check status of PLL_LD & XTAL_RDY.
					HW issue: Must check PLL_LD&XTAL_RDY when setting EEP to disable PLL power down
				*/
				if ((CmbCtrl.field.PLL_LD == 1) && (CmbCtrl.field.XTAL_RDY == 1))
					break;

				udelay(20);
			} while (index++ < MAX_CHECK_COUNT);

			if (index >= MAX_CHECK_COUNT) {
				DBGPRINT(RT_DEBUG_ERROR,
						("Lenny:[boundary]Check PLL_LD ..CMB_CTRL 0x%08x, index=%d!\n",
						CmbCtrl.word, index));
				/*
					Disable WLAN then enable WLAN again
				*/
				WlanFunCtrl &= ~MT_WLAN_FUN_CTRL_PCIE_CLK_REQ;
				WlanFunCtrl &= ~MT_WLAN_FUN_CTRL_WLAN_CLK_EN;
				WlanFunCtrl &= ~MT_WLAN_FUN_CTRL_WLAN_EN;

				mt76u_reg_write(pAd, MT_WLAN_FUN_CTRL, WlanFunCtrl);
				udelay(20);

				WlanFunCtrl |= MT_WLAN_FUN_CTRL_WLAN_CLK_EN;
				WlanFunCtrl |= MT_WLAN_FUN_CTRL_WLAN_EN;

				mt76u_reg_write(pAd, MT_WLAN_FUN_CTRL, WlanFunCtrl);
				udelay(20);
			} else {
				break;
			}
		}
		while (true);
	}

	pAd->WlanFunCtrl = WlanFunCtrl;
	WlanFunCtrl = mt76u_reg_read(pAd, MT_WLAN_FUN_CTRL);
	DBGPRINT(RT_DEBUG_TRACE,
		("<== %s():  pAd->WlanFunCtrl.word = 0x%x, Reg->WlanFunCtrl=0x%x!\n",
		__FUNCTION__, pAd->WlanFunCtrl, WlanFunCtrl));


	up(&pAd->wlan_en_atomic);
}

