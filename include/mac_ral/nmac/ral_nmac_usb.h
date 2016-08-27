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
#ifdef RT65xx
		u32		Rsv:3;
		u32		GPIOModeLed2:1;
		u32		GPIOModeLed1:1;
		u32		CsrUartMode:1;
#else
		u32      Rsv:2;
		u32		LDO25_FRC_ON:1;//4
		u32		LDO25_LARGEA:1;
		u32		LDO25_LEVEL:2;
#endif
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
#ifdef RT65xx
		u32		CsrUartMode:1;
		u32		GPIOModeLed1:1;
		u32		GPIOModeLed2:1;
		u32		Rsv:3;
#else
		u32		LDO25_LEVEL:2;
		u32		LDO25_LARGEA:1;
		u32		LDO25_FRC_ON:1;//4
		u32       	Rsv:2;
#endif /* RT65xx */
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

#ifdef RT_BIG_ENDIAN
typedef	union _USB_DMA_CFG_STRUC {
	struct {
		u32 TxBusy:1;			/* USB DMA TX FSM busy. debug only */
		u32 RxBusy:1;			/* USB DMA RX FSM busy. debug only */
		u32 EpoutValid:6;		/* OUT endpoint data valid. debug only */
		u32 TxBulkEn:1;			/* Enable USB DMA Tx */
		u32 RxBulkEn:1;			/* Enable USB DMA Rx */
		u32 RxBulkAggEn:1;		/* Enable Rx Bulk Aggregation */
		u32 WL_LPK_EN:1;
		u32 TxClear:1;        	/* Clear USB DMA TX path */
		u32 RX_DROP_OR_PADDING:1 /* Drop current WL RX packets in UDMA */
		u32 WAKEUP_EN:1;			/* USB wakeup host enable */
		u32 UDMA_TX_WL_DROP:1;	/* Drop current WL TX packets in UDMA */
		u32 RxBulkAggLmt:8;		/* Rx Bulk Aggregation Limit  in unit of 1024 bytes */
		u32 RxBulkAggTOut:8;		/* Rx Bulk Aggregation TimeOut  in unit of 33ns */
	} field;
	u32 word;
} USB_DMA_CFG_STRUC, *PUSB_DMA_CFG_STRUC;
#else
typedef	union _USB_DMA_CFG_STRUC {
	struct {
		u32 RxBulkAggTOut:8;			/* Rx Bulk Aggregation TimeOut  in unit of 33ns */
		u32 RxBulkAggLmt:8;			/* Rx Bulk Aggregation Limit  in unit of 256 bytes */
		u32 UDMA_TX_WL_DROP:1;		/* Drop current WL TX packets in UDMA */
		u32 WAKEUP_EN:1;				/* USB wakeup host enable */
		u32 RX_DROP_OR_PADDING:1; 	/* Drop current WL RX packets in UDMA */
		u32 TxClear:1;				/* Clear USB DMA TX path */
		u32 WL_LPK_EN:1;
		u32 RxBulkAggEn:1;			/* Enable Rx Bulk Aggregation */
		u32 RxBulkEn:1;				/* Enable USB DMA Rx */
		u32 TxBulkEn:1;				/* Enable USB DMA Tx */
		u32 EpoutValid:6;        	/* OUT endpoint data valid */
		u32 RxBusy:1;				/* USB DMA RX FSM busy */
		u32 TxBusy:1;				/* USB DMA TX FSM busy */
	} field;
	u32 word;
} USB_DMA_CFG_STRUC, *PUSB_DMA_CFG_STRUC;
#endif

#endif /*__RAL_OMAC_USB_H__ */

