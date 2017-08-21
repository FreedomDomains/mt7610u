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


#ifndef __RAL_OMAC_USB_H__
#define __RAL_OMAC_USB_H__
#define CMB_CTRL		0x20
#ifdef RT_BIG_ENDIAN
typedef union _CMB_CTRL_STRUC{
	struct{
		u32       	LDO0_EN:1;
		u32       	LDO3_EN:1;
		u32       	LDO_BGSEL:2;
		u32       	LDO_CORE_LEVEL:4;
		u32       	PLL_LD:1;
		u32       	XTAL_RDY:1;
		u32		Rsv:3;
		u32		GPIOModeLed2:1;
		u32		GPIOModeLed1:1;
		u32		CsrUartMode:1;
		u32		AUX_OPT_Bit15_Two_AntennaMode:1;
		u32		AUX_OPT_Bit14_TRSW1_as_GPIO:1;
		u32		AUX_OPT_Bit13_GPIO7_as_GPIO:1;
		u32		AUX_OPT_Bit12_TRSW0_as_WLAN_ANT_SEL:1;
		u32		AUX_OPT_Bit11_Rsv:1;
		u32		AUX_OPT_Bit10_NotSwap_WL_LED_ACT_RDY:1;
		u32		AUX_OPT_Bit9_GPIO3_as_GPIO:1;
		u32		AUX_OPT_Bit8_AuxPower_Exists:1;
		u32		AUX_OPT_Bit7_KeepInterfaceClk:1;
		u32		AUX_OPT_Bit6_KeepXtal_On:1;
		u32		AUX_OPT_Bit5_RemovePCIePhyClk_BTOff:1;
		u32		AUX_OPT_Bit4_RemovePCIePhyClk_WLANOff:1;
		u32		AUX_OPT_Bit3_PLLOn_L1:1;
		u32		AUX_OPT_Bit2_PCIeCoreClkOn_L1:1;
		u32		AUX_OPT_Bit1_PCIePhyClkOn_L1:1;
		u32		AUX_OPT_Bit0_InterfaceClk_40Mhz:1;
	}field;
	u32 word;
}CMB_CTRL_STRUC, *PCMB_CTRL_STRUC;
#else
typedef union _CMB_CTRL_STRUC{
	struct{
		u32		AUX_OPT_Bit0_InterfaceClk_40Mhz:1;
		u32		AUX_OPT_Bit1_PCIePhyClkOn_L1:1;
		u32		AUX_OPT_Bit2_PCIeCoreClkOn_L1:1;
		u32		AUX_OPT_Bit3_PLLOn_L1:1;
		u32		AUX_OPT_Bit4_RemovePCIePhyClk_WLANOff:1;
		u32		AUX_OPT_Bit5_RemovePCIePhyClk_BTOff:1;
		u32		AUX_OPT_Bit6_KeepXtal_On:1;
		u32		AUX_OPT_Bit7_KeepInterfaceClk:1;
		u32		AUX_OPT_Bit8_AuxPower_Exists:1;
		u32		AUX_OPT_Bit9_GPIO3_as_GPIO:1;
		u32		AUX_OPT_Bit10_NotSwap_WL_LED_ACT_RDY:1;
		u32		AUX_OPT_Bit11_Rsv:1;
		u32		AUX_OPT_Bit12_TRSW0_as_WLAN_ANT_SEL:1;
		u32		AUX_OPT_Bit13_GPIO7_as_GPIO:1;
		u32		AUX_OPT_Bit14_TRSW1_as_GPIO:1;
		u32		AUX_OPT_Bit15_Two_AntennaMode:1;
		u32		CsrUartMode:1;
		u32		GPIOModeLed1:1;
		u32		GPIOModeLed2:1;
		u32		Rsv:3;
		u32       	XTAL_RDY:1;
		u32       	PLL_LD:1;
		u32       	LDO_CORE_LEVEL:4;
		u32       	LDO_BGSEL:2;
		u32       	LDO3_EN:1;
		u32       	LDO0_EN:1;
	}field;
	u32 word;
}CMB_CTRL_STRUC, *PCMB_CTRL_STRUC;
#endif




#define USB_DMA_CFG 0x0238
#define U3DMA_WLCFG 0x9018

#define MT_USB_DMA_CFG_RX_BULK_AGG_TOUT	GENMASK(7, 0)	/* Rx Bulk Aggregation TimeOut  in unit of 33ns */
#define MT_USB_DMA_CFG_RX_BULK_AGG_LMT	GENMASK(15, 8)	/* Rx Bulk Aggregation Limit  in unit of 256 bytes */
#define MT_USB_DMA_CFG_UDMA_TX_WL_DROP	BIT(16)		/* Drop current WL TX packets in UDMA */
#define MT_USB_DMA_CFG_WAKEUP_EN	BIT(17)		/* USB wakeup host enable */
#define MT_USB_DMA_CFG_RX_DROP_OR_PAD	BIT(18)		/* Drop current WL RX packets in UDMA */
#define MT_USB_DMA_CFG_TX_CLR		BIT(19)		/* Clear USB DMA TX path */
#define MT_USB_DMA_CFG_WL_LPK_EN	BIT(20)		/* Halt TXOP count down when TX buffer is full.*/	
#define MT_USB_DMA_CFG_RX_BULK_AGG_EN	BIT(21)		/* Enable Rx Bulk Aggregation */
#define MT_USB_DMA_CFG_RX_BULK_EN	BIT(22)		/* Enable USB DMA Rx */
#define MT_USB_DMA_CFG_TX_BULK_EN	BIT(23)		/* Enable USB DMA Tx */
#define MT_USB_DMA_CFG_EP_OUT_VALID	GENMASK(29, 24)	/* OUT endpoint data valid */
#define MT_USB_DMA_CFG_RX_BUSY		BIT(30)		/* USB DMA RX FSM busy */
#define MT_USB_DMA_CFG_TX_BUSY		BIT(31)		/* USB DMA TX FSM busy */



#endif /*__RAL_OMAC_USB_H__ */

