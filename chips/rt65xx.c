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


#ifdef RT65xx

#include	"rt_config.h"


#ifdef RTMP_USB_SUPPORT
void MT76x0UsbAsicRadioOff(struct rtmp_adapter*pAd, u8 Stage)
{
	u32 ret;

	DBGPRINT(RT_DEBUG_TRACE, ("--> %s\n", __FUNCTION__));

	DISABLE_TX_RX(pAd, RTMP_HALT);

	if (IS_USB_INF(pAd)) {
		ret = down_interruptible(&pAd->hw_atomic);
		if (ret != 0) {
			DBGPRINT(RT_DEBUG_ERROR, ("reg_atomic get failed(ret=%d)\n", ret));
			return;
		}
	}

	RTMP_SET_PSFLAG(pAd, fRTMP_PS_MCU_SLEEP);
	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD);

	if (!IS_MT76x0(pAd))
	{
		if (Stage == MLME_RADIO_OFF)
			PWR_SAVING_OP(pAd, RADIO_OFF, 1, 0, 0, 0, 0);
	}

	MCU_CTRL_EXIT(pAd);
	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);
	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF);

	/* Stop bulkin pipe*/
	//if((pAd->PendingRx > 0) && (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
	if((!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
	{
		RTUSBCancelPendingBulkInIRP(pAd);
		//pAd->PendingRx = 0;
	}

	if (IS_USB_INF(pAd)) {
		up(&pAd->hw_atomic);
	}

	DBGPRINT(RT_DEBUG_TRACE, ("<== %s\n", __FUNCTION__));
}


void MT76x0UsbAsicRadioOn(struct rtmp_adapter*pAd, u8 Stage)
{
	u32 MACValue = 0;
	u32 rx_filter_flag;
	u32 ret;

	RTMP_CLEAR_PSFLAG(pAd, fRTMP_PS_MCU_SLEEP);

	if (pAd->WlanFunCtrl.field.WLAN_EN == 0)
		MT76x0_WLAN_ChipOnOff(pAd, true, false);

	/* make some traffic to invoke EvtDeviceD0Entry callback function*/
	MACValue = mt7610u_read32(pAd,0x1000);
	DBGPRINT(RT_DEBUG_TRACE,("A MAC query to invoke EvtDeviceD0Entry, MACValue = 0x%x\n",MACValue));

	rx_filter_flag = STANORMAL;     /* Staion not drop control frame will fail WiFi Certification.*/

	mt7610u_write32(pAd, RX_FILTR_CFG, rx_filter_flag);
	mt7610u_write32(pAd, MAC_SYS_CTRL, 0x0c);

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

	MCU_CTRL_INIT(pAd);

	if (IS_USB_INF(pAd)) {
		ret = down_interruptible(&pAd->hw_atomic);
		if (ret != 0) {
			DBGPRINT(RT_DEBUG_ERROR, ("reg_atomic get failed(ret=%d)\n", ret));
			return;
		}
	}

	mt7610u_write32(pAd, MAC_SYS_CTRL, 0x0c);

	if (IS_USB_INF(pAd)) {
		up(&pAd->hw_atomic);
	}

	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD);

	if (!IS_MT76x0(pAd)) {
		if (Stage == MLME_RADIO_ON)
			PWR_SAVING_OP(pAd, RADIO_ON, 1, 0, 0, 0, 0);
	}

	DBGPRINT(RT_DEBUG_TRACE, ("<== %s\n", __FUNCTION__));
}
#endif

void MT76x0DisableTxRx(
	struct rtmp_adapter*pAd,
	u8 Level)
{
	u32 MacReg = 0;
	u32 MTxCycle;
	bool bResetWLAN = false;
	bool bFree = true;
	u8 CheckFreeTimes = 0;

	if (!IS_RT65XX(pAd))
		return;

	DBGPRINT(RT_DEBUG_TRACE, ("----> %s\n", __FUNCTION__));

	if (Level == RTMP_HALT)
	{
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_ACTIVE);
	}

	DBGPRINT(RT_DEBUG_TRACE, ("%s Tx success = %ld\n",
		__FUNCTION__, (ULONG)pAd->WlanCounters.TransmittedFragmentCount.u.LowPart));
	DBGPRINT(RT_DEBUG_TRACE, ("%s Rx success = %ld\n",
		__FUNCTION__, (ULONG)pAd->WlanCounters.ReceivedFragmentCount.QuadPart));

	StopDmaTx(pAd, Level);

	/*
		Check page count in TxQ,
	*/
	for (MTxCycle = 0; MTxCycle < 2000; MTxCycle++)
	{
		bFree = true;
		MacReg = mt7610u_read32(pAd, 0x438);
		if (MacReg != 0)
			bFree = false;
		MacReg = mt7610u_read32(pAd, 0xa30);
		if (MacReg & 0x000000FF)
			bFree = false;
		MacReg = mt7610u_read32(pAd, 0xa34);
		if (MacReg & 0xFF00FF00)
			bFree = false;
		if (bFree)
			break;
		if (MacReg == 0xFFFFFFFF)
		{
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST);
			return;
		}
	}

	if (MTxCycle >= 2000)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Check TxQ page count max\n"));
		MacReg = mt7610u_read32(pAd, 0x0a30);
		DBGPRINT(RT_DEBUG_TRACE, ("0x0a30 = 0x%08x\n", MacReg));

		MacReg = mt7610u_read32(pAd, 0x0a34);
		DBGPRINT(RT_DEBUG_TRACE, ("0x0a34 = 0x%08x\n", MacReg));

		MacReg = mt7610u_read32(pAd, 0x438);
		DBGPRINT(RT_DEBUG_TRACE, ("0x438 = 0x%08x\n", MacReg));
		bResetWLAN = true;
	}

	/*
		Check MAC Tx idle
	*/
	for (MTxCycle = 0; MTxCycle < 2000; MTxCycle++)
	{
		MacReg = mt7610u_read32(pAd, MAC_STATUS_CFG);
		if (MacReg & 0x1)
			RTMPusecDelay(50);
		else
			break;

		if (MacReg == 0xFFFFFFFF)
		{
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST);
			return;
		}
	}

	if (MTxCycle >= 2000)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Check MAC Tx idle max(0x%08x)\n", MacReg));
		bResetWLAN = true;
	}

	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST) == false)
	{
		if (Level == RTMP_HALT)
		{
			/*
				Disable MAC TX/RX
			*/
			MacReg = mt7610u_read32(pAd, MAC_SYS_CTRL);
			MacReg &= ~(0x0000000c);
			mt7610u_write32(pAd, MAC_SYS_CTRL, MacReg);
		}
		else
		{
			/*
				Disable MAC RX
			*/
			MacReg = mt7610u_read32(pAd, MAC_SYS_CTRL);
			MacReg &= ~(0x00000008);
			mt7610u_write32(pAd, MAC_SYS_CTRL, MacReg);
		}
	}

	/*
		Check page count in RxQ,
	*/
	for (MTxCycle = 0; MTxCycle < 2000; MTxCycle++)
	{
		bFree = true;
		MacReg = mt7610u_read32(pAd, 0x430);

		if (MacReg & (0x00FF0000))
			bFree = false;

		MacReg = mt7610u_read32(pAd, 0xa30);

		if (MacReg != 0)
			bFree = false;

		MacReg = mt7610u_read32(pAd, 0xa34);

		if (MacReg != 0)
			bFree = false;

		if (bFree && (CheckFreeTimes > 20) && (!is_inband_cmd_processing(pAd)))
			break;

		if (bFree)
			CheckFreeTimes++;

		if (MacReg == 0xFFFFFFFF)
		{
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST);
			return;
		}
#ifdef RTMP_MAC_USB
		RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_POLL_IDLE);
		usb_rx_cmd_msgs_receive(pAd);
		RTUSBBulkReceive(pAd);
#endif /* RTMP_MAC_USB */
	}

	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_POLL_IDLE);

	if (MTxCycle >= 2000)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Check RxQ page count max\n"));

		MacReg = mt7610u_read32(pAd, 0x0a30);
		DBGPRINT(RT_DEBUG_TRACE, ("0x0a30 = 0x%08x\n", MacReg));

		MacReg = mt7610u_read32(pAd, 0x0a34);
		DBGPRINT(RT_DEBUG_TRACE, ("0x0a34 = 0x%08x\n", MacReg));

		MacReg = mt7610u_read32(pAd, 0x0430);
		DBGPRINT(RT_DEBUG_TRACE, ("0x0430 = 0x%08x\n", MacReg));
		bResetWLAN = true;
	}

	/*
		Check MAC Rx idle
	*/
	for (MTxCycle = 0; MTxCycle < 2000; MTxCycle++)
	{
		MacReg = mt7610u_read32(pAd, MAC_STATUS_CFG);
		if (MacReg & 0x2)
			RTMPusecDelay(50);
		else
			break;
		if (MacReg == 0xFFFFFFFF)
		{
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST);
			return;
		}
	}

	if (MTxCycle >= 2000)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Check MAC Rx idle max(0x%08x)\n", MacReg));
		bResetWLAN = true;
	}

	StopDmaRx(pAd, Level);

	if ((Level == RTMP_HALT) &&
		(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST) == false))
	{

		/*
 		 * Disable RF/MAC and do not do reset WLAN under below cases
 		 * 1. Combo card
 		 * 2. suspend including wow application
 		 * 3. radion off command
 		 */
		if ((pAd->chipCap.IsComboChip) || RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_SUSPEND)
				|| RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_CMD_RADIO_OFF))
			bResetWLAN = 0;

		MT76x0_WLAN_ChipOnOff(pAd, false, bResetWLAN);
	}

	DBGPRINT(RT_DEBUG_TRACE, ("<---- %s\n", __FUNCTION__));
}

void MT76x0_WLAN_ChipOnOff(
	IN struct rtmp_adapter*pAd,
	IN bool bOn,
	IN bool bResetWLAN)
{
	union rtmp_wlan_func_ctrl WlanFunCtrl = {.word=0};


#ifdef RTMP_MAC_USB
	u32 ret;

	if (IS_USB_INF(pAd)) {
		ret = down_interruptible(&pAd->wlan_en_atomic);
		if (ret != 0) {
			DBGPRINT(RT_DEBUG_ERROR, ("wlan_en_atomic get failed(ret=%d)\n", ret));
			return;
		}
	}
#endif /* RTMP_MAC_USB */

	WlanFunCtrl.word = mt7610u_read32(pAd, WLAN_FUN_CTRL);
	DBGPRINT(RT_DEBUG_OFF, ("==>%s(): OnOff:%d, Reset= %d, pAd->WlanFunCtrl:0x%x, Reg-WlanFunCtrl=0x%x\n",
				__FUNCTION__, bOn, bResetWLAN, pAd->WlanFunCtrl.word, WlanFunCtrl.word));

	if (bResetWLAN == true)
	{
		WlanFunCtrl.field.GPIO0_OUT_OE_N = 0xFF;
		WlanFunCtrl.field.FRC_WL_ANT_SET = 0;

		if (pAd->WlanFunCtrl.field.WLAN_EN)
		{
			/*
				Restore all HW default value and reset RF.
			*/
			WlanFunCtrl.field.WLAN_RESET = 1;
			WlanFunCtrl.field.WLAN_RESET_RF = 1;
			DBGPRINT(RT_DEBUG_TRACE, ("Reset(1) WlanFunCtrl.word = 0x%x\n", WlanFunCtrl.word));
			mt7610u_write32(pAd, WLAN_FUN_CTRL, WlanFunCtrl.word);
			RTMPusecDelay(20);
			WlanFunCtrl.field.WLAN_RESET = 0;
			WlanFunCtrl.field.WLAN_RESET_RF = 0;
			DBGPRINT(RT_DEBUG_TRACE, ("Reset(2) WlanFunCtrl.word = 0x%x\n", WlanFunCtrl.word));
			mt7610u_write32(pAd, WLAN_FUN_CTRL, WlanFunCtrl.word);
			RTMPusecDelay(20);
		}
	}

	if (bOn == true)
	{
		/*
			Enable WLAN function and clock
			WLAN_FUN_CTRL[1:0] = 0x3
		*/
		ENABLE_WLAN_FUN(WlanFunCtrl);
	}
	else
	{
		/*
			Diable WLAN function and clock
			WLAN_FUN_CTRL[1:0] = 0x0
		*/
		DISABLE_WLAN_FUN(WlanFunCtrl);
	}

	DBGPRINT(RT_DEBUG_TRACE, ("WlanFunCtrl.word = 0x%x\n", WlanFunCtrl.word));
	mt7610u_write32(pAd, WLAN_FUN_CTRL, WlanFunCtrl.word);
	RTMPusecDelay(20);

	if (bOn)
	{
		pAd->MACVersion = mt7610u_read32(pAd, MAC_CSR0);
		DBGPRINT(RT_DEBUG_TRACE, ("MACVersion = 0x%08x\n", pAd->MACVersion));
	}

	if (bOn == true)
	{
		UINT index = 0;
		CMB_CTRL_STRUC CmbCtrl;

		CmbCtrl.word = 0;

		do
		{
			do
			{
				CmbCtrl.word = mt7610u_read32(pAd, CMB_CTRL);

				/*
					Check status of PLL_LD & XTAL_RDY.
					HW issue: Must check PLL_LD&XTAL_RDY when setting EEP to disable PLL power down
				*/
				if ((CmbCtrl.field.PLL_LD == 1) && (CmbCtrl.field.XTAL_RDY == 1))
					break;

				RTMPusecDelay(20);
			} while (index++ < MAX_CHECK_COUNT);

			if (index >= MAX_CHECK_COUNT)
			{
				DBGPRINT(RT_DEBUG_ERROR,
						("Lenny:[boundary]Check PLL_LD ..CMB_CTRL 0x%08x, index=%d!\n",
						CmbCtrl.word, index));
				/*
					Disable WLAN then enable WLAN again
				*/
				DISABLE_WLAN_FUN(WlanFunCtrl);
				mt7610u_write32(pAd, WLAN_FUN_CTRL, WlanFunCtrl.word);
				RTMPusecDelay(20);

				ENABLE_WLAN_FUN(WlanFunCtrl);
				mt7610u_write32(pAd, WLAN_FUN_CTRL, WlanFunCtrl.word);
				RTMPusecDelay(20);
			}
			else
			{
				break;
			}
		}
		while (true);
	}

	pAd->WlanFunCtrl.word = WlanFunCtrl.word;
	WlanFunCtrl.word = mt7610u_read32(pAd, WLAN_FUN_CTRL);
	DBGPRINT(RT_DEBUG_TRACE,
		("<== %s():  pAd->WlanFunCtrl.word = 0x%x, Reg->WlanFunCtrl=0x%x!\n",
		__FUNCTION__, pAd->WlanFunCtrl.word, WlanFunCtrl.word));


#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd)) {
		up(&pAd->wlan_en_atomic);
	}
#endif /* RTMP_MAC_USB */
}

#endif /* RT65xx */
