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


#ifndef __RAL_NMAC_H__
#define __RAL_NMAC_H__

enum INFO_TYPE {
	NORMAL_PACKET,
	CMD_PACKET,
};

enum D_PORT {
	WLAN_PORT,
	CPU_RX_PORT,
	CPU_TX_PORT,
	HOST_PORT,
	VIRTUAL_CPU_RX_PORT,
	VIRTUAL_CPU_TX_PORT,
	DISCARD,
};

#include "rtmp_type.h"

#ifdef RT_BIG_ENDIAN
struct __attribute__ ((packed)) txinfo_nmac_pkt {
	u32 info_type:2;
	u32 d_port:3;
	u32 QSEL:2;
	u32 wiv:1;
	u32 rsv1:2;
	u32 cso:1;
	u32 tso:1;
	u32 pkt_80211:1;
	u32 sw_lst_rnd:1;
	u32 tx_burst:1;
	u32 next_vld:1;
	u32 pkt_len:16;
};
#else
struct __attribute__ ((packed)) txinfo_nmac_pkt {
	u32 pkt_len:16;
	u32 next_vld:1;
	u32 tx_burst:1;
	u32 sw_lst_rnd:1;
	u32 pkt_80211:1;
	u32 tso:1;
	u32 cso:1;
	u32 rsv1:2;
	u32 wiv:1;
	u32 QSEL:2;
	u32 d_port:3;
	u32 info_type:2;
};
#endif /* RT_BIG_ENDIAN */

#ifdef RT_BIG_ENDIAN
struct __attribute__ ((packed)) txinfo_nmac_cmd{
	u32 info_type:2;
	u32 d_port:3;
	u32 cmd_type:7;
	u32 cmd_seq:4;
	u32 pkt_len:16;
};
#else
struct __attribute__ ((packed)) txinfo_nmac_cmd{
	u32 pkt_len:16;
	u32 cmd_seq:4;
	u32 cmd_type:7;
	u32 d_port:3;
	u32 info_type:2;
};
#endif /* RT_BIG_ENDIAN */

#ifdef RT_BIG_ENDIAN
struct __attribute__ ((packed)) rtmp_led_cmd{
	u32  rsv:8;
	u32 CmdID:8;
	u32 Arg0:8;
	u32 Arg1:8;
};
#else
struct __attribute__ ((packed)) rtmp_led_cmd{
	u32 Arg1:8;
	u32 Arg0:8;
	u32 CmdID:8;
	u32 rsv:8;
};
#endif /* RT_BIG_ENDIAN */

#ifdef RT_BIG_ENDIAN
struct __attribute__ ((packed)) txwi_nmac {
	/* Word 0 */
	u32		PHYMODE:3;
	u32		iTxBF:1;
	u32		eTxBF:1;
	u32		STBC:1;
	u32		ShortGI:1;
	u32		BW:2;			/* channel bandwidth 20/40/80 MHz */
	u32		LDPC:1
	u32		MCS:6;

	u32		lut_en:1;
	u32		rsv0:1;
	u32		Sounding:1;
	u32		NDPSndBW:1;	/* NDP sounding BW */
	u32		NDPSndRate:2;	/* 0 : MCS0, 1: MCS8, 2: MCS16, 3: reserved */
	u32		txop:2;

	u32		MpduDensity:3;
	u32		AMPDU:1;
	u32		TS:1;
	u32		CFACK:1;
	u32		MIMOps:1;	/* the remote peer is in dynamic MIMO-PS mode */
	u32		FRAG:1;		/* 1 to inform TKIP engine this is a fragment. */

	/* Word 1 */
	u32		TIM:1;
	u32		TXBF_PT_SCA:1;
	u32		MPDUtotalByteCnt:14;
	u32		wcid:8;
	u32		BAWinSize:6;
	u32		NSEQ:1;
	u32		ACK:1;

	/* Word 2 */
	u32		IV;
	/* Word 3 */
	u32		EIV;

	/* Word 4 */
	u32		TxPktId:8;
	u32		Rsv4:4;
	u32		TxPwrAdj:4;
	u32		TxStreamMode:8;
	u32		TxEAPId:8;
};
#else
struct __attribute__ ((packed)) txwi_nmac {
	/* Word	0 */
	/* ex: 00 03 00 40 means txop = 3, PHYMODE = 1 */
	u32		FRAG:1;		/* 1 to inform TKIP engine this is a fragment. */
	u32		MIMOps:1;	/* the remote peer is in dynamic MIMO-PS mode */
	u32		CFACK:1;
	u32		TS:1;
	u32		AMPDU:1;
	u32		MpduDensity:3;

	u32		txop:2;
	u32		NDPSndRate:2; /* 0 : MCS0, 1: MCS8, 2: MCS16, 3: reserved */
	u32		NDPSndBW:1; /* NDP sounding BW */
	u32		Sounding:1;
	u32		rsv0:1;
	u32		lut_en:1;

	u32		MCS:6;
	u32		LDPC:1;
	u32		BW:2;		/*channel bandwidth 20/40/80 MHz */
	u32		ShortGI:1;
	u32		STBC:1;
	u32		eTxBF:1;
	u32		iTxBF:1;
	u32		PHYMODE:3;

	/* Word1 */
	/* ex:  1c ff 38 00 means ACK=0, BAWinSize=7, MPDUtotalByteCnt = 0x38 */
	u32		ACK:1;
	u32		NSEQ:1;
	u32		BAWinSize:6;
	u32		wcid:8;
	u32		MPDUtotalByteCnt:14;
	u32		TXBF_PT_SCA:1;
	u32		TIM:1;

	/*Word2 */
	u32		IV;

	/*Word3 */
	u32		EIV;

	/* Word 4 */
	u32		TxEAPId:8;
	u32		TxStreamMode:8;
	u32		TxPwrAdj:4;
	u32		Rsv4:4;
	u32		TxPktId:8;
};
#endif

#define TxWITIM				TIM
#define TxWITxBfPTSca			TXBF_PT_SCA
#define TxWIMPDUByteCnt			MPDUtotalByteCnt
#define TxWIWirelessCliID		wcid
#define TxWIFRAG			FRAG
#define TxWICFACK			CFACK
#define TxWITS				TS
#define TxWIAMPDU			AMPDU
#define TxWIACK				ACK
#define TxWITXOP			txop
#define TxWINSEQ			NSEQ
#define TxWIBAWinSize			BAWinSize
#define TxWIShortGI			ShortGI
#define TxWISTBC			STBC
#define TxWIPacketId			TxPktId
#define TxWIBW				BW
#define TxWILDPC			LDPC
#define TxWIMCS				MCS
#define TxWIPHYMODE			PHYMODE
#define TxWIMIMOps			MIMOps
#define TxWIMpduDensity			MpduDensity
#define TxWILutEn			lut_en


/*
	Rx Memory layout:

	1. Rx Descriptor
		Rx Descriptor(12 Bytes) + RX_FCE_Info(4 Bytes)
	2. Rx Buffer
		RxInfo(4 Bytes) + RXWI() + 802.11 packet
*/


#ifdef RT_BIG_ENDIAN
typedef struct __attribute__ ((packed)) _RXFCE_INFO{
	u32 info_type:2;
	u32 s_port:3;
	u32 qsel:2;
	u32 pcie_intr:1;

	u32 mac_len:3;
	u32 l3l4_done:1;
	u32 pkt_80211:1;
	u32 ip_err:1;
	u32 tcp_err:1;
	u32 udp_err:1;

	u32 rsv:2;
	u32 pkt_len:14;
}RXFCE_INFO;
#else
typedef struct __attribute__ ((packed)) _RXFCE_INFO{
	u32 pkt_len:14;
	u32 rsv:2;

	u32 udp_err:1;
	u32 tcp_err:1;
	u32 ip_err:1;
	u32 pkt_80211:1;
	u32 l3l4_done:1;
	u32 mac_len:3;

	u32 pcie_intr:1;
	u32 qsel:2;
	u32 s_port:3;
	u32 info_type:2;
}RXFCE_INFO;
#endif /* RT_BIG_ENDIAN */

#ifdef RT_BIG_ENDIAN
struct __attribute__ ((packed)) rxfce_info_cmd {
	u32 info_type:2;
	u32 d_port:3;
	u32 qsel:2;
	u32 pcie_intr:1;
	u32 evt_type:4;
	u32 cmd_seq:4;
	u32 self_gen:1;
	u32 rsv:1;
	u32 pkt_len:14;
};
#else
struct __attribute__ ((packed)) rxfce_info_cmd {
	u32 pkt_len:14;
	u32 rsv:1;
	u32 self_gen:1;
	u32 cmd_seq:4;
	u32 evt_type:4;
	u32 pcie_intr:1;
	u32 qsel:2;
	u32 d_port:3;
	u32 info_type:2;
};
#endif


#ifdef RT_BIG_ENDIAN
typedef struct __attribute__ ((packed)) _RXINFO_NMAC{
	u32 ic_err:1;
	u32 tc_err:1;
	u32 rsv:1;
	u32 action_wanted:1;
	u32 deauth:1;
	u32 disasso:1;
	u32 beacon:1;
	u32 probe_rsp:1;
	u32 sw_fc_type1:1;
	u32 sw_fc_type0:1;
	u32 pn_len:3;
	u32 wapi_kid:1;
	u32 bssid_idx3:1;
	u32 dec:1;
	u32 ampdu:1;
	u32 l2pad:1;
	u32 rssi:1;
	u32 htc:1;
	u32 amsdu:1;
	u32 mic_err:1;
	u32 icv_err:1;
	u32 crc_err:1;
	u32 mybss:1;
	u32 bc:1;
	u32 mc:1;
	u32 u2me:1;
	u32 frag:1;
	u32 null:1;
	u32 data:1;
	u32 ba:1;
};
#else
typedef struct __attribute__ ((packed)) _RXINFO_NMAC{
	u32 ba:1;
	u32 data:1;
	u32 null:1;
	u32 frag:1;
	u32 u2me:1;
	u32 mcast:1;
	u32 bcast:1;
	u32 mybss:1;
	u32 crc_err:1;
	u32 icv_err:1;
	u32 mic_err:1;
	u32 amsdu:1;
	u32 htc:1;
	u32 rssi:1;
	u32 l2pad:1;
	u32 ampdu:1;
	u32 dec:1;
	u32 bssid_idx3:1;
	u32 wapi_kid:1;
	u32 pn_len:3;
	u32 sw_fc_type0:1;
	u32 sw_fc_type1:1;
	u32 probe_rsp:1;
	u32 beacon:1;
	u32 disasso:1;
	u32 deauth:1;
	u32 action_wanted:1;
	u32 rsv:1;
	u32 tc_err:1;
	u32 ic_err:1;
}RXINFO_NMAC;
#endif /* RT_BIG_ENDIAN */


/*
	RXWI wireless information format.
*/
#ifdef RT_BIG_ENDIAN
struct rxwi_nmac __attribute__ ((packed)) {
	/* Word 0 */
	u32 eof:1;
	u32 rsv:1;
	u32 MPDUtotalByteCnt:14;  /* mpdu_total_byte = rxfceinfo_len - rxwi_len- rxinfo_len - l2pad */
	u32 udf:3;
	u32 bss_idx:3;
	u32 key_idx:2;
	u32 wcid:8;

	/* Word 1 */
	u32 phy_mode:3;
	u32 rsv1:1;
	u32 ldpc_ex_sym:1;
	u32 stbc:1;
	u32 sgi:1;
	u32 bw:2;
	u32 ldpc:1;
	u32 mcs:6;
	u32 sn:12;
	u32 tid:4;

	/* Word 2 */
	u8 rssi[4];

	/* Word 3~6 */
	u8 bbp_rxinfo[16];
};
#else
struct __attribute__ ((packed)) rxwi_nmac {
	/* Word 0 */
	u32 wcid:8;
	u32 key_idx:2;
	u32 bss_idx:3;
	u32 udf:3;
	u32 MPDUtotalByteCnt:14; /* mpdu_total_byte = rxfceinfo_len - rxwi_len- rxinfo_len - l2pad */
	u32 rsv:1;
	u32 eof:1;

	/* Word 1 */
	u32 tid:4;
	u32 sn:12;
	u32 mcs:6;
	u32 ldpc:1;
	u32 bw:2;
	u32 sgi:1;
	u32 stbc:1;
	u32 ldpc_ex_sym:1;
	u32 rsv1:1;
	u32 phy_mode:3;

	/* Word 2 */
	u8 rssi[4];

	/* Word 3~6 */
	u8 bbp_rxinfo[16];
};
#endif /* RT_BIG_ENDIAN */

typedef struct __attribute__ ((packed)) _HW_RATE_CTRL_STRUCT_{
#ifdef RT_BIG_ENDIAN
	UINT16 PHYMODE:3;
	UINT16 iTxBF:1;
	UINT16 eTxBF:1;
	UINT16 STBC:1;
	UINT16 ShortGI:1;
	UINT16 BW:2;			/* channel bandwidth 20/40/80 MHz */
	UINT16 MCS:7;
#else
	UINT16 MCS:7;
	UINT16 BW:2;
	UINT16 ShortGI:1;
	UINT16 STBC:1;
	UINT16 eTxBF:1;
	UINT16 iTxBF:1;
	UINT16 PHYMODE:3;
#endif /* RT_BIG_ENDIAN */
}HW_RATE_CTRL_STRUCT;


/* ================================================================================= */
/* Register format */
/* ================================================================================= */


#define WLAN_FUN_CTRL		0x80
#ifdef RT_BIG_ENDIAN
union rtmp_wlan_func_ctrl {
	struct{
		u32 GPIO0_OUT_OE_N:8;
		u32 GPIO0_OUT:8;
		u32 GPIO0_IN:8;
		u32 WLAN_ACC_BT:1;
		u32 INV_TR_SW0:1;
		u32 FRC_WL_ANT_SET:1;
		u32 PCIE_APP0_CLK_REQ:1;
		u32 WLAN_RESET:1;
		u32 WLAN_RESET_RF:1;
		u32 WLAN_CLK_EN:1;
		u32 WLAN_EN:1;
	}field;
	u32 word;
};
#else
union rtmp_wlan_func_ctrl{
	struct{
		u32 WLAN_EN:1;
		u32 WLAN_CLK_EN:1;
		u32 WLAN_RESET_RF:1;
		u32 WLAN_RESET:1;
		u32 PCIE_APP0_CLK_REQ:1;
		u32 FRC_WL_ANT_SET:1;
		u32 INV_TR_SW0:1;
		u32 WLAN_ACC_BT:1;
		u32 GPIO0_IN:8;
		u32 GPIO0_OUT:8;
		u32 GPIO0_OUT_OE_N:8;
	}field;
	u32 word;
};
#endif


#define WLAN_FUN_INFO		0x84
#ifdef RT_BIG_ENDIAN
typedef union _WLAN_FUN_INFO_STRUC{
	struct{
		u32		BT_EEP_BUSY:1; /* Read-only for WLAN Driver */
		u32		Rsv1:26;
		u32		COEX_MODE:5; /* WLAN function enable */
	}field;
	u32 word;
}WLAN_FUN_INFO_STRUC, *PWLAN_FUN_INFO_STRUC;
#else
typedef union _WLAN_FUN_INFO_STRUC{
	struct{
		u32		COEX_MODE:5; /* WLAN function enable */
		u32		Rsv1:26;
		u32		BT_EEP_BUSY:1; /* Read-only for WLAN Driver */
	}field;
	u32 word;
}WLAN_FUN_INFO_STRUC, *PWLAN_FUN_INFO_STRUC;
#endif


#define BT_FUN_CTRL		0xC0
#ifdef RT_BIG_ENDIAN
typedef union _BT_FUN_CTRL_STRUC{
	struct{
		u32		GPIO1_OUT_OE_N:8;
		u32		GPIO1_OUT:8;
		u32		GPIO1_IN:8;
		u32		BT_ACC_WLAN:1;
		u32		INV_TR_SW1:1;
		u32		URXD_GPIO_MODE:1;
		u32		PCIE_APP1_CLK_REQ:1;
		u32		BT_RESET:1;
		u32		BT_RF_EN:1;
		u32		BT_CLK_EN:1;
		u32		BT_EN:1;
	}field;
	u32 word;
}BT_FUN_CTRL_STRUC, *PBT_FUN_CTRL_STRUC;
#else
typedef union _BT_FUN_CTRL_STRUC	{
	struct{
		u32		BT_EN:1;
		u32		BT_CLK_EN:1;
		u32		BT_RF_EN:1;
		u32		BT_RESET:1;
		u32		PCIE_APP1_CLK_REQ:1;
		u32		URXD_GPIO_MODE:1;
		u32		INV_TR_SW1:1;
		u32		BT_ACC_WLAN:1;
		u32		GPIO1_IN:8;
		u32		GPIO1_OUT:8;
		u32		GPIO1_OUT_OE_N:8;
	}field;
	u32 word;
}BT_FUN_CTRL_STRUC, *PBT_FUN_CTRL_STRUC;
#endif


#define BT_FUN_INFO		0xC4
#ifdef RT_BIG_ENDIAN
typedef union _BT_FUN_INFO_STRUC{
	struct{
		u32		WLAN_EEP_BUSY:1;
		u32		BTPower1:7;	/* Peer */
		u32		BTPower0:8; /* Self */
		u32		AFH_END_CH:8;
		u32		AFH_START_CH:8;
	}field;
	u32 word;
} BT_FUN_INFO_STRUC, *PBT_FUN_INFO_STRUC;
#else
typedef	union _BT_FUN_INFO_STRUC	{
	struct{
		u32		AFH_START_CH:8;
		u32		AFH_END_CH:8;
		u32		BTPower0:8;	/* Self */
		u32		BTPower1:7;	/* Peer */
		u32		WLAN_EEP_BUSY:1;
	}field;
	u32 word;
}BT_FUN_INFO_STRUC, *PBT_FUN_INFO_STRUC;
#endif

// TODO: shiang, this data structure is not defined for register. may can move to other place
typedef struct _WLAN_BT_COEX_SETTING
{
	bool 				ampduOff;
	bool 				coexSettingRunning;
	bool 				RateSelectionForceToUseRSSI;
	u8 				TxQualityFlag;
	ULONG					alc;
	ULONG					slna;
}WLAN_BT_COEX_SETTING, *PWLAN_BT_COEX_SETTING;


#define MCU_CMD_CFG 0x0234


#define TSO_CTRL	0x0250
#ifdef RT_BIG_ENDIAN
typedef union _TSO_CTRL_STRUC {
	struct {
		u32 rsv:13;
		u32 TSO_WR_LEN_EN:1;
		u32 TSO_SEG_EN:1;
		u32 TSO_EN:1;
		u32 RXWI_LEN:4;
		u32 RX_L2_FIX_LEN:4;
		u32 TXWI_LEN:4;
		u32 TX_L2_FIX_LEN:4;
	} field;
	u32 word;
} TSO_CTRL_STRUC;
#else
typedef union _TSO_CTRL_STRUC {
	struct {
		u32 TX_L2_FIX_LEN:4;
		u32 TXWI_LEN:4;
		u32 RX_L2_FIX_LEN:4;
		u32 RXWI_LEN:4;
		u32 TSO_EN:1;
		u32 TSO_SEG_EN:1;
		u32 TSO_WR_LEN_EN:1;
		u32 rsv:13;
	} field;
	u32 word;
} TSO_CTRL_STRUC;
#endif /* RT_BIG_ENDIAN */


#define TX_PROT_CFG6    0x13E0    // VHT 20 Protection
#define TX_PROT_CFG7    0x13E4    // VHT 40 Protection
#define TX_PROT_CFG8    0x13E8    // VHT 80 Protection
#define PIFS_TX_CFG     0x13EC    // PIFS setting

//----------------------------------------------------------------
// Header Translation
//----------------------------------------------------------------

/*
	WIFI INFO for TX translation
	|TXINO|TXWI|WIFI INFO|802.3 MAC Header|Pyaload|
*/

#define WIFI_INFO_SIZE		0
#ifdef RT_BIG_ENDIAN
typedef union __attribute__ ((packed)) _WIFI_INFO_STRUC {
	struct {
    	u32 More_Data:1;
    	u32 WEP:1;
    	u32 PS:1;
    	u32 RDG:1;
    	u32 QoS:1;
    	u32 EOSP:1;
    	u32 TID:4;
    	u32 Mode:2;
    	u32 VLAN:1;
    	u32 Rsv:3;
    	u32 BssIdx:4;
    	u32 Seq_Num:12;
	} field;
	u32 word;
} WIFI_INFO_STRUC, *PWIFI_INFO_STRUC;
#else
typedef union __attribute__ ((packed)) _WIFI_INFO_STRUC {
	struct {
    	u32 Seq_Num:12;
    	u32 BssIdx:4;
    	u32 Rsv:3;
    	u32 VLAN:1;
    	u32 Mode:2;
    	u32 TID:4;
    	u32 EOSP:1;
    	u32 QoS:1;
    	u32 RDG:1;
    	u32 PS:1;
    	u32 WEP:1;
    	u32 More_Data:1;
	} field;
	u32 word;
} WIFI_INFO_STRUC, *PWIFI_INFO_STRUC;
#endif /* RT_BIG_ENDIAN */

/*
	header translation control register
	bit0 --> TX translation enable
	bit1 --> RX translation enable
*/
#define HEADER_TRANS_CTRL_REG	0x0260
#define HT_TX_ENABLE			0x1
#define HT_RX_ENABLE			0x2

#define HT_MAC_ADDR_DW0		0x02A4
#define HT_MAC_ADDR_DW1		0x02A8
#define HT_MAC_BSSID_DW0		0x02AC
#define HT_MAC_BSSID_DW1		0x02B0

#ifdef RT_BIG_ENDIAN
typedef union __attribute__ ((packed)) _HDR_TRANS_CTRL_STRUC {
	struct {
    	u32 Rsv:30;
    	u32 Rx_En:1;
    	u32 Tx_En:1;
	} field;
	u32 word;
} HDR_TRANS_CTRL_STRUC, *PHDR_TRANS_CTRL_STRUC;
#else
typedef union __attribute__ ((packed)) _HDR_TRANS_CTRL_STRUC {
	struct {
    	u32 Tx_En:1;
    	u32 Rx_En:1;
    	u32 Rsv:30;
	} field;
	u32 word;
} HDR_TRANS_CTRL_STRUC, *PHDR_TRANS_CTRL_STRUC;
#endif /* RT_BIG_ENDIAN */

/* RX header translation enable by WCID */
#define HT_RX_WCID_EN_BASE	0x0264
#define HT_RX_WCID_OFFSET	32
#define HT_RX_WCID_SIZE		(HT_RX_WCID_OFFSET * 8)	/*	256 WCIDs */
#ifdef RT_BIG_ENDIAN
typedef union __attribute__ ((packed)) _HT_RX_WCID_EN_STRUC {
	struct {
    	u32 RX_WCID31_TRAN_EN:1;
    	u32 RX_WCID30_TRAN_EN:1;
    	u32 RX_WCID29_TRAN_EN:1;
    	u32 RX_WCID28_TRAN_EN:1;
    	u32 RX_WCID27_TRAN_EN:1;
    	u32 RX_WCID26_TRAN_EN:1;
    	u32 RX_WCID25_TRAN_EN:1;
    	u32 RX_WCID24_TRAN_EN:1;
    	u32 RX_WCID23_TRAN_EN:1;
    	u32 RX_WCID22_TRAN_EN:1;
    	u32 RX_WCID21_TRAN_EN:1;
    	u32 RX_WCID20_TRAN_EN:1;
    	u32 RX_WCID19_TRAN_EN:1;
    	u32 RX_WCID18_TRAN_EN:1;
    	u32 RX_WCID17_TRAN_EN:1;
    	u32 RX_WCID16_TRAN_EN:1;
    	u32 RX_WCID15_TRAN_EN:1;
    	u32 RX_WCID14_TRAN_EN:1;
    	u32 RX_WCID13_TRAN_EN:1;
    	u32 RX_WCID12_TRAN_EN:1;
    	u32 RX_WCID11_TRAN_EN:1;
    	u32 RX_WCID10_TRAN_EN:1;
    	u32 RX_WCID9_TRAN_EN:1;
    	u32 RX_WCID8_TRAN_EN:1;
    	u32 RX_WCID7_TRAN_EN:1;
    	u32 RX_WCID6_TRAN_EN:1;
    	u32 RX_WCID5_TRAN_EN:1;
    	u32 RX_WCID4_TRAN_EN:1;
    	u32 RX_WCID3_TRAN_EN:1;
    	u32 RX_WCID2_TRAN_EN:1;
    	u32 RX_WCID1_TRAN_EN:1;
    	u32 RX_WCID0_TRAN_EN:1;
	} field;
	u32 word;
} HT_RX_WCID_EN_STRUC, *PHT_RX_WCID_EN_STRUC;
#else
typedef union __attribute__ ((packed)) _HT_RX_WCID_EN_STRUC {
	struct {
    	u32 RX_WCID0_TRAN_EN:1;
    	u32 RX_WCID1_TRAN_EN:1;
    	u32 RX_WCID2_TRAN_EN:1;
    	u32 RX_WCID3_TRAN_EN:1;
    	u32 RX_WCID4_TRAN_EN:1;
    	u32 RX_WCID5_TRAN_EN:1;
    	u32 RX_WCID6_TRAN_EN:1;
    	u32 RX_WCID7_TRAN_EN:1;
    	u32 RX_WCID8_TRAN_EN:1;
    	u32 RX_WCID9_TRAN_EN:1;
    	u32 RX_WCID10_TRAN_EN:1;
    	u32 RX_WCID11_TRAN_EN:1;
    	u32 RX_WCID12_TRAN_EN:1;
    	u32 RX_WCID13_TRAN_EN:1;
    	u32 RX_WCID14_TRAN_EN:1;
    	u32 RX_WCID15_TRAN_EN:1;
    	u32 RX_WCID16_TRAN_EN:1;
    	u32 RX_WCID17_TRAN_EN:1;
    	u32 RX_WCID18_TRAN_EN:1;
    	u32 RX_WCID19_TRAN_EN:1;
    	u32 RX_WCID20_TRAN_EN:1;
    	u32 RX_WCID21_TRAN_EN:1;
    	u32 RX_WCID22_TRAN_EN:1;
    	u32 RX_WCID23_TRAN_EN:1;
    	u32 RX_WCID24_TRAN_EN:1;
    	u32 RX_WCID25_TRAN_EN:1;
    	u32 RX_WCID26_TRAN_EN:1;
    	u32 RX_WCID27_TRAN_EN:1;
    	u32 RX_WCID28_TRAN_EN:1;
    	u32 RX_WCID29_TRAN_EN:1;
    	u32 RX_WCID30_TRAN_EN:1;
    	u32 RX_WCID31_TRAN_EN:1;
	} field;
	u32 word;
} HT_RX_WCID_EN_STRUC, *PHT_RX_WCID_EN_STRUC;
#endif /* RT_BIG_ENDIAN */

/* RX header translation - black list */
#define HT_RX_BL_BASE		0x0284
#define HT_RX_BL_OFFSET		2
#define HT_RX_BL_SIZE		8
#ifdef RT_BIG_ENDIAN
typedef union __attribute__ ((packed)) _HT_RX_BLACK_LIST_STRUC {
	struct {
    	u32 BLACK_ETHER_TYPE1:16;
    	u32 BLACK_ETHER_TYPE0:16;
	} field;
	u32 word;
} HT_RX_BLACK_LIST_STRUC, *PHT_RX_BLACK_LIST_STRUC;
#else
typedef union __attribute__ ((packed)) _HT_RX_BLACK_LIST_STRUC {
	struct {
    	u32 BLACK_ETHER_TYPE0:16;
    	u32 BLACK_ETHER_TYPE1:16;
	} field;
	u32 word;
} HT_RX_BLACK_LIST_STRUC, *PHT_RX_BLACK_LIST_STRUC;
#endif /* RT_BIG_ENDIAN */

/* RX VLAN Mapping (TCI) */
#define HT_BSS_VLAN_BASE	0x0294
#define HT_BSS_VLAN_OFFSET	2
#define HT_BSS_VLAN_SIZE	8
#ifdef RT_BIG_ENDIAN
typedef union __attribute__ ((packed)) _HT_BSS_VLAN_STRUC {
	struct {
    	u32 TCI1_VID:12;
    	u32 TCI1_CFI:1;
    	u32 TCI1_PCP:3;
    	u32 TCI0_VID:12;
    	u32 TCI0_CFI:1;
    	u32 TCI0_PCP:3;
	} field;
	u32 word;
} HT_BSS_VLAN_STRUC, *PHT_BSS_VLAN_STRUC;
#else
typedef union __attribute__ ((packed)) _HT_BSS_VLAN_STRUC {
	struct {
    	u32 TCI0_PCP:3;
    	u32 TCI0_CFI:1;
    	u32 TCI0_VID:12;
    	u32 TCI1_PCP:3;
    	u32 TCI1_CFI:1;
    	u32 TCI1_VID:12;
	} field;
	u32 word;
} HT_BSS_VLAN_STRUC, *PHT_BSS_VLAN_STRUC;
#endif /* RT_BIG_ENDIAN */


// TODO: shiang-6590, following definitions are dummy and not used for RT6590, shall remove/correct these!
#define GPIO_CTRL_CFG	0x0228
#define PBF_MAX_PCNT	0x0408 //actually, it's the TX_MAX_PCNT
// TODO:shiang-6590 --------------------------


#define PAIRWISE_KEY_TABLE_BASE     0x8000      /* 32-byte * 256-entry =  -byte */
#define HW_KEY_ENTRY_SIZE           0x20

#define PAIRWISE_IVEIV_TABLE_BASE     0xa000      /* 8-byte * 256-entry =  -byte */
#define MAC_IVEIV_TABLE_BASE     0xa000      /* 8-byte * 256-entry =  -byte */
#define HW_IVEIV_ENTRY_SIZE   8

#define MAC_WCID_ATTRIBUTE_BASE     0xa800      /* 4-byte * 256-entry =  -byte */
#define HW_WCID_ATTRI_SIZE   4

#define SHARED_KEY_TABLE_BASE       0xac00      /* 32-byte * 16-entry = 512-byte */
#define SHARED_KEY_MODE_BASE       0xb000      /* 32-byte * 16-entry = 512-byte */

#define SHARED_KEY_TABLE_BASE_EXT      0xb400      /* for BSS_IDX=8~15, 32-byte * 16-entry = 512-byte */
#define SHARED_KEY_MODE_BASE_EXT       0xb3f0      /* for BSS_IDX=8~15, 32-byte * 16-entry = 512-byte */

#define HW_SHARED_KEY_MODE_SIZE   4
#define SHAREDKEYTABLE			0
#define PAIRWISEKEYTABLE			1

/* This resgiser is ONLY be supported for RT3883 or later.
   It conflicted with BCN#0 offset of previous chipset. */
#define WAPI_PN_TABLE_BASE		0xb800
#define WAPI_PN_ENTRY_SIZE   		8

#define RF_MISC	0x0518
#ifdef RT_BIG_ENDIAN
typedef union _RF_MISC_STRUC{
	struct{
		u32 Rsv1:29;
		u32 EXT_PA_EN:1;
		u32 ADDAC_LDO_ADC9_EN:1;
		u32 ADDAC_LDO_ADC6_EN:1;
	}field;
	u32 word;
}RF_MISC_STRUC, *PRF_MISC_STRUC;
#else
typedef union _RF_MISC_STRUC{
	struct{
		u32 ADDAC_LDO_ADC6_EN:1;
		u32 ADDAC_LDO_ADC9_EN:1;
		u32 EXT_PA_EN:1;
		u32 Rsv1:29;
	}field;
	u32 word;
}RF_MISC_STRUC, *PRF_MISC_STRUC;
#endif

void ral_wlan_chip_onoff(
	IN struct rtmp_adapter *pAd,
	IN bool bOn,
	IN bool bResetWLAN);

#define AUX_CLK_CFG		0x120C
#define BB_PA_MODE_CFG0	0x1214
#define BB_PA_MODE_CFG1	0x1218
#define RF_PA_MODE_CFG0 0x121C
#define RF_PA_MODE_CFG1	0x1220
#define TX_ALC_CFG_0	0x13B0
#define TX_ALC_CFG_1	0x13B4

#define TX0_RF_GAIN_CORR	0x13A0
#define TX0_RF_GAIN_ATTEN	0x13A8
#define TX0_BB_GAIN_ATTEN	0x13C0
#define TX_ALC_VGA3			0x13C8

#define CPU_CTL				0x0704
#define RESET_CTL			0x070C
#define INT_LEVEL			0x0718
#define COM_REG0			0x0730
#define COM_REG1			0x0734
#define COM_REG2			0x0738
#define COM_REG3			0x073C
#define PCIE_REMAP_BASE1	0x0740
#define PCIE_REMAP_BASE2	0x0744
#define PCIE_REMAP_BASE3	0x0748
#define	PCIE_REMAP_BASE4	0x074C
#define LED_CTRL			0x0770
#define LED_TX_BLINK_0		0x0774
#define	LED_TX_BLINK_1		0x0778
#define LED0_S0				0x077C
#define LED0_S1				0x0780
#define SEMAPHORE_00		0x07B0

#define APCLI_BSSID_IDX			8
#define MAC_APCLI_BSSID_DW0		0x1090
#define MAC_APCLI_BSSID_DW1		0x1094

#ifdef MAC_REPEATER_SUPPORT
#define MAC_ADDR_EXT_EN			0x147C
#define MAC_ADDR_EXT0_31_0		0x1480
#define MAC_ADDR_EXT0_47_32		0x1484
#define MAX_EXT_MAC_ADDR_SIZE	16

#define UNKOWN_APCLI_IDX		0xFF

#define CLIENT_APCLI			0x00
#define CLIENT_STA				0x01
#define CLIENT_ETH				0x02
#endif /* MAC_REPEATER_SUPPORT */

#endif /* __RAL_NMAC_H__ */
