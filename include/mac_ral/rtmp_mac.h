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


#ifndef __RTMP_MAC_H__
#define __RTMP_MAC_H__

#include "mac_ral/nmac/ral_nmac.h"


/*
	TX / RX ring descriptor format

	TX:
		PCI/RBUS_Descriptor + TXINFO + TXWI + 802.11

	Rx:
		PCI/RBUS/USB_Descripotr + (PCI/RBUS RXFCE_INFO) + (PCI/RBUS  RXINFO) + RXWI + 802.11 + (USB RXINFO)

*/

/* the first 24-byte in TXD is called TXINFO and will be DMAed to MAC block through TXFIFO. */
/* MAC block use this TXINFO to control the transmission behavior of this frame. */
#define FIFO_MGMT	0
#define FIFO_HCCA	1
#define FIFO_EDCA	2

#define TXINFO_SIZE			4
union __attribute__ ((packed)) txinfo_nmac {
	struct txinfo_nmac_pkt txinfo_nmac_pkt;
	struct txinfo_nmac_cmd txinfo_nmac_cmd;
	u32 word;
};


/*
	RXWI wireless information format, in PBF. invisible in driver.
*/

#define RXINFO_SIZE			4
#ifdef RT_BIG_ENDIAN
struct __attribute__ ((packed)) rtmp_rxinfo {
	u32		ip_sum_err:1;		/* IP checksum error */
	u32		tcp_sum_err:1;	/* TCP checksum error */
	u32		rsv:1;
	u32		action_wanted:1;
	u32		deauth:1;
	u32		disasso:1;
	u32		beacon:1;
	u32		probe_rsp:1;
	u32		sw_fc_type1:1;
	u32		sw_fc_type0:1;
	u32		pn_len:3;
	u32		wapi_kidx:1;
	u32		BssIdx3:1;
	u32		Decrypted:1;
	u32		AMPDU:1;
	u32		L2PAD:1;
	u32		RSSI:1;
	u32		HTC:1;
	u32		AMSDU:1;		/* rx with 802.3 header, not 802.11 header. obsolete. */
	u32		CipherErr:2;        /* 0: decryption okay, 1:ICV error, 2:MIC error, 3:KEY not valid */
	u32		Crc:1;			/* 1: CRC error */
	u32		MyBss:1;		/* 1: this frame belongs to the same BSSID */
	u32		Bcast:1;			/* 1: this is a broadcast frame */
	u32		Mcast:1;			/* 1: this is a multicast frame */
	u32		U2M:1;			/* 1: this RX frame is unicast to me */
	u32		FRAG:1;
	u32		NULLDATA:1;
	u32		DATA:1;
	u32		BA:1;
};
#else
struct __attribute__ ((packed)) rtmp_rxinfo {
	u32		BA:1;
	u32		DATA:1;
	u32		NULLDATA:1;
	u32		FRAG:1;
	u32		U2M:1;
	u32		Mcast:1;
	u32		Bcast:1;
	u32		MyBss:1;
	u32		Crc:1;
	u32		CipherErr:2;
	u32		AMSDU:1;
	u32		HTC:1;
	u32		RSSI:1;
	u32		L2PAD:1;
	u32		AMPDU:1;
	u32		Decrypted:1;
	u32		BssIdx3:1;
	u32		wapi_kidx:1;
	u32		pn_len:3;
	u32		sw_fc_type0:1;
	u32      	sw_fc_type1:1;
	u32      	probe_rsp:1;
	u32		beacon:1;
	u32		disasso:1;
	u32      	deauth:1;
	u32      	action_wanted:1;
	u32      	rsv:1;
	u32		tcp_sum_err:1;
	u32		ip_sum_err:1;
};
#endif

#define TSO_SIZE		0


/* ================================================================================= */
/* Register format */
/* ================================================================================= */


/*
	SCH/DMA registers - base address 0x0200
*/
#define WMM_AIFSN_CFG   0x0214
#ifdef RT_BIG_ENDIAN
typedef	union _AIFSN_CSR_STRUC{
	struct {
	    u32   Aifsn7:4;       /* for AC_VO */
	    u32   Aifsn6:4;       /* for AC_VI */
	    u32   Aifsn5:4;       /* for AC_BK */
	    u32   Aifsn4:4;       /* for AC_BE */
	    u32   Aifsn3:4;       /* for AC_VO */
	    u32   Aifsn2:4;       /* for AC_VI */
	    u32   Aifsn1:4;       /* for AC_BK */
	    u32   Aifsn0:4;       /* for AC_BE */
	}field;
	u32 word;
} AIFSN_CSR_STRUC;
#else
typedef union _AIFSN_CSR_STRUC {
	struct {
	    u32   Aifsn0:4;
	    u32   Aifsn1:4;
	    u32   Aifsn2:4;
	    u32   Aifsn3:4;
	    u32   Aifsn4:4;
	    u32   Aifsn5:4;
	    u32   Aifsn6:4;
	    u32   Aifsn7:4;
	} field;
	u32 word;
} AIFSN_CSR_STRUC;
#endif

/* CWMIN_CSR: CWmin for each EDCA AC */
#define WMM_CWMIN_CFG   0x0218
#ifdef RT_BIG_ENDIAN
typedef	union _CWMIN_CSR_STRUC	{
	struct {
		u32   Cwmin7:4;       /* for AC_VO */
		u32   Cwmin6:4;       /* for AC_VI */
		u32   Cwmin5:4;       /* for AC_BK */
		u32   Cwmin4:4;       /* for AC_BE */
		u32   Cwmin3:4;       /* for AC_VO */
		u32   Cwmin2:4;       /* for AC_VI */
		u32   Cwmin1:4;       /* for AC_BK */
		u32   Cwmin0:4;       /* for AC_BE */
	} field;
	u32 word;
} CWMIN_CSR_STRUC;
#else
typedef	union _CWMIN_CSR_STRUC	{
	struct {
	    u32   Cwmin0:4;
	    u32   Cwmin1:4;
	    u32   Cwmin2:4;
	    u32   Cwmin3:4;
	    u32   Cwmin4:4;
	    u32   Cwmin5:4;
	    u32   Cwmin6:4;
	    u32   Cwmin7:4;
	} field;
	u32 word;
} CWMIN_CSR_STRUC;
#endif


/* CWMAX_CSR: CWmin for each EDCA AC */
#define WMM_CWMAX_CFG   0x021c
#ifdef RT_BIG_ENDIAN
typedef	union _CWMAX_CSR_STRUC {
	struct {
		u32   Cwmax7:4;       /* for AC_VO */
		u32   Cwmax6:4;       /* for AC_VI */
		u32   Cwmax5:4;       /* for AC_BK */
		u32   Cwmax4:4;       /* for AC_BE */
		u32   Cwmax3:4;       /* for AC_VO */
		u32   Cwmax2:4;       /* for AC_VI */
		u32   Cwmax1:4;       /* for AC_BK */
		u32   Cwmax0:4;       /* for AC_BE */
	} field;
	u32 word;
} CWMAX_CSR_STRUC;
#else
typedef	union _CWMAX_CSR_STRUC {
	struct {
	    u32   Cwmax0:4;
	    u32   Cwmax1:4;
	    u32   Cwmax2:4;
	    u32   Cwmax3:4;
	    u32   Cwmax4:4;
	    u32   Cwmax5:4;
	    u32   Cwmax6:4;
	    u32   Cwmax7:4;
	} field;
	u32 word;
}	CWMAX_CSR_STRUC;
#endif


/* AC_TXOP_CSR0: AC_BK/AC_BE TXOP register */
#define WMM_TXOP0_CFG    0x0220
#ifdef RT_BIG_ENDIAN
typedef	union _AC_TXOP_CSR0_STRUC {
	struct {
	    UINT16  Ac1Txop; /* for AC_BE, in unit of 32us */
	    UINT16  Ac0Txop; /* for AC_BK, in unit of 32us */
	} field;
	u32 word;
} AC_TXOP_CSR0_STRUC;
#else
typedef	union _AC_TXOP_CSR0_STRUC {
	struct {
	    UINT16  Ac0Txop;
	    UINT16  Ac1Txop;
	} field;
	u32 word;
} AC_TXOP_CSR0_STRUC;
#endif


/* AC_TXOP_CSR1: AC_VO/AC_VI TXOP register */
#define WMM_TXOP1_CFG    0x0224
#ifdef RT_BIG_ENDIAN
typedef	union _AC_TXOP_CSR1_STRUC {
	struct {
	    UINT16  Ac3Txop; /* for AC_VO, in unit of 32us */
	    UINT16  Ac2Txop; /* for AC_VI, in unit of 32us */
	} field;
	u32 word;
} AC_TXOP_CSR1_STRUC;
#else
typedef	union _AC_TXOP_CSR1_STRUC {
	struct {
	    UINT16 Ac2Txop;
	    UINT16 Ac3Txop;
	} field;
	u32 word;
} AC_TXOP_CSR1_STRUC;
#endif


#define WMM_TXOP2_CFG 0x0228
#define WMM_TXOP3_CFG 0x022c

#define WMM_CTRL	0x0230



/*================================================================================= */
/* MAC  registers                                                                                                                                                                 */
/*================================================================================= */
/*  4.1 MAC SYSTEM  configuration registers (offset:0x1000) */
#define MAC_CSR0            0x1000
#ifdef RT_BIG_ENDIAN
typedef	union _ASIC_VER_ID_STRUC {
	struct {
	    UINT16  ASICVer;        /* version */
	    UINT16  ASICRev;        /* reversion */
	} field;
	u32 word;
} ASIC_VER_ID_STRUC;
#else
typedef	union _ASIC_VER_ID_STRUC {
	struct {
	    UINT16  ASICRev;
	    UINT16  ASICVer;
	} field;
	u32 word;
} ASIC_VER_ID_STRUC;
#endif

#define MAC_SYS_CTRL            0x1004
#define MAC_ADDR_DW0		0x1008
#define MAC_ADDR_DW1		0x100c

/* MAC_CSR2: STA MAC register 0 */
#ifdef RT_BIG_ENDIAN
typedef	union _MAC_DW0_STRUC {
	struct {
		u8 Byte3;		/* MAC address byte 3 */
		u8 Byte2;		/* MAC address byte 2 */
		u8 Byte1;		/* MAC address byte 1 */
		u8 Byte0;		/* MAC address byte 0 */
	} field;
	u32 word;
} MAC_DW0_STRUC;
#else
typedef	union _MAC_DW0_STRUC {
	struct {
		u8 Byte0;
		u8 Byte1;
		u8 Byte2;
		u8 Byte3;
	} field;
	u32 word;
} MAC_DW0_STRUC;
#endif


/* MAC_CSR3: STA MAC register 1 */
#ifdef RT_BIG_ENDIAN
typedef	union _MAC_DW1_STRUC {
	struct {
		u8		Rsvd1;
		u8		U2MeMask;
		u8		Byte5;		/* MAC address byte 5 */
		u8		Byte4;		/* MAC address byte 4 */
	} field;
	u32 word;
} MAC_DW1_STRUC;
#else
typedef	union _MAC_DW1_STRUC {
	struct {
		u8 Byte4;
		u8 Byte5;
		u8 U2MeMask;
		u8 Rsvd1;
	} field;
	u32 word;
}	MAC_DW1_STRUC;
#endif

#define MAC_BSSID_DW0		0x1010
#define MAC_BSSID_DW1		0x1014
/* MAC_CSR5: BSSID register 1 */
#ifdef RT_BIG_ENDIAN
typedef	union	_MAC_BSSID_DW1_STRUC {
	struct {
		u32 NMBssMode3:1;
		u32 NMBssMode2:1;
		u32 NMBssMode:1;
		u32 MBssBcnNum:3;
		u32 MBssMode:2; /* 0: one BSSID, 10: 4 BSSID,  01: 2 BSSID , 11: 8BSSID */
		u32 Byte5:8;		 /* BSSID byte 5 */
		u32 Byte4:8;		 /* BSSID byte 4 */
	} field;
	u32 word;
} MAC_BSSID_DW1_STRUC;
#else
typedef	union	_MAC_BSSID_DW1_STRUC {
	struct {
		u32 Byte4:8;
		u32 Byte5:8;
		u32 MBssMode:2;
		u32 MBssBcnNum:3;
		u32 NMBssMode:1;
		u32 NMBssMode2:1;
		u32 NMBssMode3:1;
	} field;
	u32 word;
} MAC_BSSID_DW1_STRUC;
#endif

/* rt2860b max 16k bytes. bit12:13 Maximum PSDU length (power factor) 0:2^13, 1:2^14, 2:2^15, 3:2^16 */
#define MAX_LEN_CFG              0x1018


/* BBP_CSR_CFG: BBP serial control register */
#define BBP_CSR_CFG            	0x101c
#ifdef RT_BIG_ENDIAN
typedef	union _BBP_CSR_CFG_STRUC {
	struct {
		u32		:12;
		u32		BBP_RW_MODE:1;	/* 0: use serial mode  1:parallel */
		u32		BBP_PAR_DUR:1;		/* 0: 4 MAC clock cycles  1: 8 MAC clock cycles */
		u32		Busy:1;				/* 1: ASIC is busy execute BBP programming. */
		u32		fRead:1;				/* 0: Write BBP, 1:	Read BBP */
		u32		RegNum:8;			/* Selected BBP register */
		u32		Value:8;				/* Register value to program into BBP */
	} field;
	u32 word;
} BBP_CSR_CFG_STRUC;
#else
typedef	union _BBP_CSR_CFG_STRUC {
	struct {
		u32		Value:8;
		u32		RegNum:8;
		u32		fRead:1;
		u32		Busy:1;
		u32		BBP_PAR_DUR:1;
		u32		BBP_RW_MODE:1;
		u32		:12;
	} field;
	u32 word;
} BBP_CSR_CFG_STRUC;
#endif


/* RF_CSR_CFG: RF control register */
#define RF_CSR_CFG0            		0x1020
#ifdef RT_BIG_ENDIAN
typedef	union _RF_CSR_CFG0_STRUC {
	struct {
		u32 Busy:1;		    /* 0: idle 1: 8busy */
		u32 Sel:1;			/* 0:RF_LE0 activate  1:RF_LE1 activate */
		u32 StandbyMode:1;	/* 0: high when stand by 1:	low when standby */
		u32 bitwidth:5;		/* Selected BBP register */
		u32 RegIdAndContent:24;	/* Register value to program into BBP */
	} field;
	u32 word;
} RF_CSR_CFG0_STRUC;
#else
typedef	union _RF_CSR_CFG0_STRUC {
	struct {
		u32 RegIdAndContent:24;
		u32 bitwidth:5;
		u32 StandbyMode:1;
		u32 Sel:1;
		u32 Busy:1;
	} field;
	u32 word;
} RF_CSR_CFG0_STRUC;
#endif


#define RF_CSR_CFG1           		0x1024
#ifdef RT_BIG_ENDIAN
typedef	union _RF_CSR_CFG1_STRUC {
	struct {
		u32 rsv:7;	/* 0: idle 1: 8busy */
		u32 RFGap:5;	/* Gap between BB_CONTROL_RF and RF_LE. 0: 3 system clock cycle (37.5usec) 1: 5 system clock cycle (62.5usec) */
		u32 RegIdAndContent:24;	/* Register value to program into BBP */
	} field;
	u32 word;
} RF_CSR_CFG1_STRUC;
#else
typedef	union _RF_CSR_CFG1_STRUC {
	struct {
		u32 RegIdAndContent:24;
		u32 RFGap:5;
		u32 rsv:7;
	} field;
	u32 word;
} RF_CSR_CFG1_STRUC;
#endif


#define RF_CSR_CFG2           		0x1028
#ifdef RT_BIG_ENDIAN
typedef	union _RF_CSR_CFG2_STRUC {
	struct {
		u32 rsv:8;		    /* 0: idle 1: 8busy */
		u32 RegIdAndContent:24; /* Register value to program into BBP */
	} field;
	u32 word;
}	RF_CSR_CFG2_STRUC;
#else
typedef	union _RF_CSR_CFG2_STRUC {
	struct {
		u32 RegIdAndContent:24;
		u32 rsv:8;
	} field;
	u32 word;
} RF_CSR_CFG2_STRUC;
#endif


#define LED_CFG           		0x102c
#ifdef RT_BIG_ENDIAN
typedef	union _LED_CFG_STRUC {
	struct {
		u32		:1;
		u32		LedPolar:1;			/* Led Polarity.  0: active low1: active high */
		u32		YLedMode:2;			/* yellow Led Mode */
		u32		GLedMode:2;			/* green Led Mode */
		u32		RLedMode:2;			/* red Led Mode    0: off1: blinking upon TX2: periodic slow blinking3: always on */
		u32		rsv:2;
		u32		SlowBlinkPeriod:6;			/* slow blinking period. unit:1ms */
		u32		OffPeriod:8;			/* blinking off period unit 1ms */
		u32		OnPeriod:8;			/* blinking on period unit 1ms */
	} field;
	u32 word;
} LED_CFG_STRUC;
#else
typedef	union _LED_CFG_STRUC {
	struct {
		u32		OnPeriod:8;
		u32		OffPeriod:8;
		u32		SlowBlinkPeriod:6;
		u32		rsv:2;
		u32		RLedMode:2;
		u32		GLedMode:2;
		u32		YLedMode:2;
		u32		LedPolar:1;
		u32		:1;
	} field;
	u32 word;
} LED_CFG_STRUC;
#endif


#define AMPDU_MAX_LEN_20M1S	0x1030
#define AMPDU_MAX_LEN_20M2S	0x1034
#define AMPDU_MAX_LEN_40M1S	0x1038
#define AMPDU_MAX_LEN_40M2S	0x103c
#define AMPDU_MAX_LEN			0x1040


/* The number of the Tx chains */
#define NUM_OF_TX_CHAIN		4

#define TX_CHAIN_ADDR0_L	0x1044		/* Stream mode MAC address registers */
#define TX_CHAIN_ADDR0_H	0x1048
#define TX_CHAIN_ADDR1_L	0x104C
#define TX_CHAIN_ADDR1_H	0x1050
#define TX_CHAIN_ADDR2_L	0x1054
#define TX_CHAIN_ADDR2_H	0x1058
#define TX_CHAIN_ADDR3_L	0x105C
#define TX_CHAIN_ADDR3_H	0x1060

#ifdef RT_BIG_ENDIAN
typedef union _TX_CHAIN_ADDR0_L_STRUC {
	struct {
		u8 TxChainAddr0L_Byte3; /* Destination MAC address of Tx chain0 (byte 3) */
		u8 TxChainAddr0L_Byte2; /* Destination MAC address of Tx chain0 (byte 2) */
		u8 TxChainAddr0L_Byte1; /* Destination MAC address of Tx chain0 (byte 1) */
		u8 TxChainAddr0L_Byte0; /* Destination MAC address of Tx chain0 (byte 0) */
	} field;
	u32 word;
} TX_CHAIN_ADDR0_L_STRUC;
#else
typedef union _TX_CHAIN_ADDR0_L_STRUC {
	struct {
		u8 TxChainAddr0L_Byte0;
		u8 TxChainAddr0L_Byte1;
		u8 TxChainAddr0L_Byte2;
		u8 TxChainAddr0L_Byte3;
	} field;
	u32 word;
} TX_CHAIN_ADDR0_L_STRUC;
#endif


#ifdef RT_BIG_ENDIAN
typedef union _TX_CHAIN_ADDR0_H_STRUC {
	struct {
		UINT16	Reserved:12; /* Reserved */
		UINT16	TxChainSel0:4; /* Selection value of Tx chain0 */
		u8	TxChainAddr0H_Byte5; /* Destination MAC address of Tx chain0 (byte 5) */
		u8	TxChainAddr0H_Byte4; /* Destination MAC address of Tx chain0 (byte 4) */
	} field;
	u32 word;
} TX_CHAIN_ADDR0_H_STRUC;
#else
typedef union _TX_CHAIN_ADDR0_H_STRUC {
	struct {
		u8	TxChainAddr0H_Byte4; /* Destination MAC address of Tx chain0 (byte 4) */
		u8	TxChainAddr0H_Byte5; /* Destination MAC address of Tx chain0 (byte 5) */
		UINT16	TxChainSel0:4; /* Selection value of Tx chain0 */
		UINT16	Reserved:12; /* Reserved */
	} field;
	u32 word;
} TX_CHAIN_ADDR0_H_STRUC;
#endif


#ifdef RT_BIG_ENDIAN
typedef union _TX_CHAIN_ADDR1_L_STRUC {
	struct {
		u8	TxChainAddr1L_Byte3; /* Destination MAC address of Tx chain1 (byte 3) */
		u8	TxChainAddr1L_Byte2; /* Destination MAC address of Tx chain1 (byte 2) */
		u8	TxChainAddr1L_Byte1; /* Destination MAC address of Tx chain1 (byte 1) */
		u8	TxChainAddr1L_Byte0; /* Destination MAC address of Tx chain1 (byte 0) */
	} field;
	u32 word;
} TX_CHAIN_ADDR1_L_STRUC, *PTX_CHAIN_ADDR1_L_STRUC;
#else
typedef union _TX_CHAIN_ADDR1_L_STRUC {
	struct {
		u8	TxChainAddr1L_Byte0;
		u8	TxChainAddr1L_Byte1;
		u8	TxChainAddr1L_Byte2;
		u8	TxChainAddr1L_Byte3;
	} field;
	u32 word;
} TX_CHAIN_ADDR1_L_STRUC, *PTX_CHAIN_ADDR1_L_STRUC;
#endif


#ifdef RT_BIG_ENDIAN
typedef union _TX_CHAIN_ADDR1_H_STRUC {
	struct {
		UINT16	Reserved:12; /* Reserved */
		UINT16	TxChainSel0:4; /* Selection value of Tx chain0 */
		u8	TxChainAddr1H_Byte5; /* Destination MAC address of Tx chain1 (byte 5) */
		u8	TxChainAddr1H_Byte4; /* Destination MAC address of Tx chain1 (byte 4) */
	} field;
	u32 word;
} TX_CHAIN_ADDR1_H_STRUC ;
#else
typedef union _TX_CHAIN_ADDR1_H_STRUC {
	struct {
		u8	TxChainAddr1H_Byte4;
		u8	TxChainAddr1H_Byte5;
		UINT16	TxChainSel0:4;
		UINT16	Reserved:12;
	} field;
	u32 word;
} TX_CHAIN_ADDR1_H_STRUC ;
#endif


#ifdef RT_BIG_ENDIAN
typedef union _TX_CHAIN_ADDR2_L_STRUC {
	struct {
		u8	TxChainAddr2L_Byte3; /* Destination MAC address of Tx chain2 (byte 3) */
		u8	TxChainAddr2L_Byte2; /* Destination MAC address of Tx chain2 (byte 2) */
		u8	TxChainAddr2L_Byte1; /* Destination MAC address of Tx chain2 (byte 1) */
		u8	TxChainAddr2L_Byte0; /* Destination MAC address of Tx chain2 (byte 0) */
	} field;
	u32 word;
} TX_CHAIN_ADDR2_L_STRUC;
#else
typedef union _TX_CHAIN_ADDR2_L_STRUC {
	struct {
		u8	TxChainAddr2L_Byte0;
		u8	TxChainAddr2L_Byte1;
		u8	TxChainAddr2L_Byte2;
		u8	TxChainAddr2L_Byte3;
	} field;
	u32 word;
} TX_CHAIN_ADDR2_L_STRUC;
#endif


#ifdef RT_BIG_ENDIAN
typedef union _TX_CHAIN_ADDR2_H_STRUC {
	struct {
		UINT16	Reserved:12; /* Reserved */
		UINT16	TxChainSel0:4; /* Selection value of Tx chain0 */
		u8	TxChainAddr2H_Byte5; /* Destination MAC address of Tx chain2 (byte 5) */
		u8	TxChainAddr2H_Byte4; /* Destination MAC address of Tx chain2 (byte 4) */
	} field;
	u32 word;
} TX_CHAIN_ADDR2_H_STRUC;
#else
typedef union _TX_CHAIN_ADDR2_H_STRUC {
	struct {
		u8	TxChainAddr2H_Byte4;
		u8	TxChainAddr2H_Byte5;
		UINT16	TxChainSel0:4;
		UINT16	Reserved:12;
	} field;
	u32 word;
} TX_CHAIN_ADDR2_H_STRUC;
#endif


#ifdef RT_BIG_ENDIAN
typedef union _TX_CHAIN_ADDR3_L_STRUC {
	struct {
		u8	TxChainAddr3L_Byte3; /* Destination MAC address of Tx chain3 (byte 3) */
		u8	TxChainAddr3L_Byte2; /* Destination MAC address of Tx chain3 (byte 2) */
		u8	TxChainAddr3L_Byte1; /* Destination MAC address of Tx chain3 (byte 1) */
		u8	TxChainAddr3L_Byte0; /* Destination MAC address of Tx chain3 (byte 0) */
	} field;
	u32 word;
} TX_CHAIN_ADDR3_L_STRUC, *PTX_CHAIN_ADDR3_L_STRUC;
#else
typedef union _TX_CHAIN_ADDR3_L_STRUC {
	struct {
		u8	TxChainAddr3L_Byte0;
		u8	TxChainAddr3L_Byte1;
		u8	TxChainAddr3L_Byte2;
		u8	TxChainAddr3L_Byte3;
	} field;
	u32 word;
}	TX_CHAIN_ADDR3_L_STRUC, *PTX_CHAIN_ADDR3_L_STRUC;
#endif


#ifdef RT_BIG_ENDIAN
typedef union _TX_CHAIN_ADDR3_H_STRUC {
	struct {
		UINT16	Reserved:12; /* Reserved */
		UINT16	TxChainSel0:4; /* Selection value of Tx chain0 */
		u8	TxChainAddr3H_Byte5; /* Destination MAC address of Tx chain3 (byte 5) */
		u8	TxChainAddr3H_Byte4; /* Destination MAC address of Tx chain3 (byte 4) */
	} field;
	u32 word;
} TX_CHAIN_ADDR3_H_STRUC, *PTX_CHAIN_ADDR3_H_STRUC;
#else
typedef union _TX_CHAIN_ADDR3_H_STRUC {
	struct {
		u8	TxChainAddr3H_Byte4;
		u8	TxChainAddr3H_Byte5;
		UINT16	TxChainSel0:4;
		UINT16	Reserved:12;
	} field;
	u32 word;
} TX_CHAIN_ADDR3_H_STRUC;
#endif



/*  4.2 MAC TIMING  configuration registers (offset:0x1100) */
#define XIFS_TIME_CFG             0x1100
#ifdef RT_BIG_ENDIAN
typedef	union _IFS_SLOT_CFG_STRUC {
	struct {
	    u32  rsv:2;
	    u32  BBRxendEnable:1;        /*  reference RXEND signal to begin XIFS defer */
	    u32  EIFS:9;        /*  unit 1us */
	    u32  OfdmXifsTime:4;        /*OFDM SIFS. unit 1us. Applied after OFDM RX when MAC doesn't reference BBP signal BBRXEND */
	    u32  OfdmSifsTime:8;        /*  unit 1us. Applied after OFDM RX/TX */
	    u32  CckmSifsTime:8;        /*  unit 1us. Applied after CCK RX/TX */
	} field;
	u32 word;
} IFS_SLOT_CFG_STRUC;
#else
typedef	union _IFS_SLOT_CFG_STRUC {
	struct {
	    u32  CckmSifsTime:8;
	    u32  OfdmSifsTime:8;
	    u32  OfdmXifsTime:4;
	    u32  EIFS:9;
	    u32  BBRxendEnable:1;
	    u32  rsv:2;
	} field;
	u32 word;
} IFS_SLOT_CFG_STRUC;
#endif

#define BKOFF_SLOT_CFG		0x1104
#define NAV_TIME_CFG		0x1108
#define CH_TIME_CFG			0x110C
#define PBF_LIFE_TIMER		0x1110	/*TX/RX MPDU timestamp timer (free run)Unit: 1us */


/* BCN_TIME_CFG : Synchronization control register */
#define BCN_TIME_CFG             0x1114
#ifdef RT_BIG_ENDIAN
typedef	union _BCN_TIME_CFG_STRUC {
	struct {
		u32 TxTimestampCompensate:8;
		u32 :3;
		u32 bBeaconGen:1;		/* Enable beacon generator */
		u32 bTBTTEnable:1;
		u32 TsfSyncMode:2;		/* Enable TSF sync, 00: disable, 01: infra mode, 10: ad-hoc mode */
		u32 bTsfTicking:1;		/* Enable TSF auto counting */
		u32 BeaconInterval:16;  /* in unit of 1/16 TU */
	} field;
	u32 word;
} BCN_TIME_CFG_STRUC;
#else
typedef union _BCN_TIME_CFG_STRUC {
	struct {
		u32 BeaconInterval:16;
		u32 bTsfTicking:1;
		u32 TsfSyncMode:2;
		u32 bTBTTEnable:1;
		u32 bBeaconGen:1;
		u32 :3;
		u32 TxTimestampCompensate:8;
	} field;
	u32 word;
} BCN_TIME_CFG_STRUC;
#endif


#define TBTT_SYNC_CFG		0x1118
#define TSF_TIMER_DW0		0x111c	/* Local TSF timer lsb 32 bits. Read-only */
#define TSF_TIMER_DW1		0x1120	/* msb 32 bits. Read-only. */
#define TBTT_TIMER			0x1124	/* TImer remains till next TBTT. Read-only */
#define INT_TIMER_CFG		0x1128
#define INT_TIMER_EN		0x112c	/* GP-timer and pre-tbtt Int enable */
#define CH_IDLE_STA			0x1130	/* channel idle time */
#define CH_BUSY_STA			0x1134	/* channle busy time */
#define CH_BUSY_STA_SEC	0x1138	/* channel busy time for secondary channel */


/*  4.2 MAC POWER  configuration registers (offset:0x1200) */
#define MAC_STATUS_CFG		0x1200
#define PWR_PIN_CFG		0x1204


/* AUTO_WAKEUP_CFG: Manual power control / status register */
#define AUTO_WAKEUP_CFG	0x1208
#ifdef RT_BIG_ENDIAN
typedef	union _AUTO_WAKEUP_STRUC {
	struct {
		u32 :16;
		u32 EnableAutoWakeup:1;	/* 0:sleep, 1:awake */
		u32 NumofSleepingTbtt:7;          /* ForceWake has high privilege than PutToSleep when both set */
		u32 AutoLeadTime:8;
	} field;
	u32 word;
} AUTO_WAKEUP_STRUC;
#else
typedef	union _AUTO_WAKEUP_STRUC {
	struct {
		u32 AutoLeadTime:8;
		u32 NumofSleepingTbtt:7;
		u32 EnableAutoWakeup:1;
		u32 :16;
	} field;
	u32 word;
} AUTO_WAKEUP_STRUC;
#endif


/*  4.3 MAC TX  configuration registers (offset:0x1300) */
#define EDCA_AC0_CFG	0x1300
#define EDCA_AC1_CFG	0x1304
#define EDCA_AC2_CFG	0x1308
#define EDCA_AC3_CFG	0x130c
#ifdef RT_BIG_ENDIAN
typedef	union _EDCA_AC_CFG_STRUC {
	struct {
	    u32 :12;
	    u32 Cwmax:4;	/* unit power of 2 */
	    u32 Cwmin:4;
	    u32 Aifsn:4;	/* # of slot time */
	    u32 AcTxop:8;	/*  in unit of 32us */
	} field;
	u32 word;
} EDCA_AC_CFG_STRUC;
#else
typedef	union _EDCA_AC_CFG_STRUC {
	struct {
	    u32 AcTxop:8;
	    u32 Aifsn:4;
	    u32 Cwmin:4;
	    u32 Cwmax:4;
	    u32 :12;
	} field;
	u32 word;
} EDCA_AC_CFG_STRUC;
#endif

#define EDCA_TID_AC_MAP	0x1310


/* Default Tx power */
#define DEFAULT_TX_POWER	0x6

#define TX_PWR_CFG_0		0x1314
#define TX_PWR_CFG_0_EXT	0x1390
#define TX_PWR_CFG_1		0x1318
#define TX_PWR_CFG_1_EXT	0x1394
#define TX_PWR_CFG_2		0x131C
#define TX_PWR_CFG_2_EXT	0x1398
#define TX_PWR_CFG_3		0x1320
#define TX_PWR_CFG_3_EXT	0x139C
#define TX_PWR_CFG_4		0x1324
#define TX_PWR_CFG_4_EXT	0x13A0
#define TX_PWR_CFG_5		0x1384
#define TX_PWR_CFG_6		0x1388
#define TX_PWR_CFG_7		0x13D4
#define TX_PWR_CFG_8		0x13D8
#define TX_PWR_CFG_9		0x13DC

#ifdef RT_BIG_ENDIAN
typedef	union _TX_PWR_CFG_STRUC {
	struct {
	    u32 Byte3:8;
	    u32 Byte2:8;
	    u32 Byte1:8;
	    u32 Byte0:8;
	} field;
	u32 word;
} TX_PWR_CFG_STRUC;
#else
typedef	union _TX_PWR_CFG_STRUC {
	struct {
	    u32 Byte0:8;
	    u32 Byte1:8;
	    u32 Byte2:8;
	    u32 Byte3:8;
	} field;
	u32 word;
} TX_PWR_CFG_STRUC;
#endif


#define TX_PIN_CFG		0x1328
#define TX_BAND_CFG	0x132c	/* 0x1 use upper 20MHz. 0 juse lower 20MHz */
#define TX_SW_CFG0		0x1330
#define TX_SW_CFG1		0x1334
#define TX_SW_CFG2		0x1338


#define TXOP_THRES_CFG	0x133c
#ifdef RT_BIG_ENDIAN
typedef union _TXOP_THRESHOLD_CFG_STRUC {
	struct {
		u32	TXOP_REM_THRES:8; /* Remaining TXOP threshold (unit: 32us) */
		u32	CF_END_THRES:8; /* CF-END threshold (unit: 32us) */
		u32	RDG_IN_THRES:8; /* Rx RDG threshold (unit: 32us) */
		u32	RDG_OUT_THRES:8; /* Tx RDG threshold (unit: 32us) */
	} field;
	u32 word;
} TXOP_THRESHOLD_CFG_STRUC;
#else
typedef union _TXOP_THRESHOLD_CFG_STRUC {
	struct {
		u32	RDG_OUT_THRES:8;
		u32	RDG_IN_THRES:8;
		u32	CF_END_THRES:8;
		u32	TXOP_REM_THRES:8;
	} field;
	u32 word;
} TXOP_THRESHOLD_CFG_STRUC;
#endif

#define TXOP_CTRL_CFG 0x1340


#define TX_RTS_CFG 0x1344
#ifdef RT_BIG_ENDIAN
typedef	union _TX_RTS_CFG_STRUC {
	struct {
	    u32 rsv:7;
	    u32 RtsFbkEn:1;    /* enable rts rate fallback */
	    u32 RtsThres:16;    /* unit:byte */
	    u32 AutoRtsRetryLimit:8;
	} field;
	u32 word;
} TX_RTS_CFG_STRUC;
#else
typedef	union _TX_RTS_CFG_STRUC	 {
	struct {
	    u32 AutoRtsRetryLimit:8;
	    u32 RtsThres:16;
	    u32 RtsFbkEn:1;
	    u32 rsv:7;
	} field;
	u32 word;
} TX_RTS_CFG_STRUC;
#endif


#define TX_TXBF_CFG_0 0x138c
#define TX_TXBF_CFG_1 0x13A4
#define TX_TXBF_CFG_2 0x13A8
#define TX_TXBF_CFG_3 0x13AC
typedef	union _TX_TXBF_CFG_0_STRUC {
	struct {
#ifdef RT_BIG_ENDIAN
	    u32       EtxbfFbkRate:16;
	    u32       EtxbfFbkEn:1;
	    u32       EtxbfFbkSeqEn:1;
	    u32       EtxbfFbkCoef:2;
	    u32       EtxbfFbkCode:2;
	    u32       EtxbfFbkNg:2;
	    u32       CsdBypass:1;
	    u32       EtxbfForce:1;
	    u32       EtxbfEnable:1;
	    u32       AutoTxbfEn:3;
	    u32       ItxbfForce:1;
	    u32       ItxbfEn:1;
#else
	    u32       ItxbfEn:1;
	    u32       ItxbfForce:1;
	    u32       AutoTxbfEn:3;
	    u32       EtxbfEnable:1;
	    u32       EtxbfForce:1;
	    u32       CsdBypass:1;
	    u32       EtxbfFbkNg:2;
	    u32       EtxbfFbkCode:2;
	    u32       EtxbfFbkCoef:2;
	    u32       EtxbfFbkSeqEn:1;
	    u32       EtxbfFbkEn:1;
	    u32       EtxbfFbkRate:16;
#endif
	} field;
	u32 word;
} TX_TXBF_CFG_0_STRUC;


#define TX_TIMEOUT_CFG	0x1348
#ifdef RT_BIG_ENDIAN
typedef	union _TX_TIMEOUT_CFG_STRUC {
	struct {
	    u32 rsv2:8;
	    u32 TxopTimeout:8;	/*TXOP timeout value for TXOP truncation.  It is recommended that (SLOT_TIME) > (TX_OP_TIMEOUT) > (RX_ACK_TIMEOUT) */
	    u32 RxAckTimeout:8;	/* unit:slot. Used for TX precedure */
	    u32 MpduLifeTime:4;    /*  expiration time = 2^(9+MPDU LIFE TIME)  us */
	    u32 rsv:4;
	} field;
	u32 word;
} TX_TIMEOUT_CFG_STRUC;
#else
typedef	union _TX_TIMEOUT_CFG_STRUC {
	struct {
	    u32 rsv:4;
	    u32 MpduLifeTime:4;
	    u32 RxAckTimeout:8;
	    u32 TxopTimeout:8;
	    u32 rsv2:8;
	} field;
	u32 word;
} TX_TIMEOUT_CFG_STRUC;
#endif


#define TX_RTY_CFG	0x134c
#ifdef RT_BIG_ENDIAN
typedef	union _TX_RTY_CFG_STRUC {
	struct {
	    u32 rsv:1;
	    u32 TxautoFBEnable:1;    /* Tx retry PHY rate auto fallback enable */
	    u32 AggRtyMode:1;	/* Aggregate MPDU retry mode.  0:expired by retry limit, 1: expired by mpdu life timer */
	    u32 NonAggRtyMode:1;	/* Non-Aggregate MPDU retry mode.  0:expired by retry limit, 1: expired by mpdu life timer */
	    u32 LongRtyThre:12;	/* Long retry threshoold */
	    u32 LongRtyLimit:8;	/*long retry limit */
	    u32 ShortRtyLimit:8;	/*  short retry limit */
	} field;
	u32 word;
} TX_RTY_CFG_STRUC;
#else
typedef	union _TX_RTY_CFG_STRUC {
	struct {
	    u32 ShortRtyLimit:8;
	    u32 LongRtyLimit:8;
	    u32 LongRtyThre:12;
	    u32 NonAggRtyMode:1;
	    u32 AggRtyMode:1;
	    u32 TxautoFBEnable:1;
	    u32 rsv:1;
	} field;
	u32 word;
} TX_RTY_CFG_STRUC;
#endif


#define TX_LINK_CFG	0x1350
#ifdef RT_BIG_ENDIAN
typedef	union _TX_LINK_CFG_STRUC {
	struct {
	    u32       RemotMFS:8;	/*remote MCS feedback sequence number */
	    u32       RemotMFB:8;    /*  remote MCS feedback */
	    u32       rsv:3;	/* */
	    u32       TxCFAckEn:1;	/*   Piggyback CF-ACK enable */
	    u32       TxRDGEn:1;	/* RDG TX enable */
	    u32       TxMRQEn:1;	/*  MCS request TX enable */
	    u32       RemoteUMFSEnable:1;	/*  remote unsolicit  MFB enable.  0: not apply remote remote unsolicit (MFS=7) */
	    u32       MFBEnable:1;	/*  TX apply remote MFB 1:enable */
	    u32       RemoteMFBLifeTime:8;	/*remote MFB life time. unit : 32us */
	} field;
	u32 word;
} TX_LINK_CFG_STRUC;
#else
typedef	union _TX_LINK_CFG_STRUC {
	struct {
	    u32       RemoteMFBLifeTime:8;
	    u32       MFBEnable:1;
	    u32       RemoteUMFSEnable:1;
	    u32       TxMRQEn:1;
	    u32       TxRDGEn:1;
	    u32       TxCFAckEn:1;
	    u32       rsv:3;
	    u32       RemotMFB:8;
	    u32       RemotMFS:8;
	} field;
	u32 word;
} TX_LINK_CFG_STRUC;
#endif


#define HT_FBK_CFG0	0x1354
#ifdef RT_BIG_ENDIAN
typedef	union _HT_FBK_CFG0_STRUC {
	struct {
	    u32 HTMCS7FBK:4;
	    u32 HTMCS6FBK:4;
	    u32 HTMCS5FBK:4;
	    u32 HTMCS4FBK:4;
	    u32 HTMCS3FBK:4;
	    u32 HTMCS2FBK:4;
	    u32 HTMCS1FBK:4;
	    u32 HTMCS0FBK:4;
	} field;
	u32 word;
} HT_FBK_CFG0_STRUC;
#else
typedef	union _HT_FBK_CFG0_STRUC {
	struct {
	    u32 HTMCS0FBK:4;
	    u32 HTMCS1FBK:4;
	    u32 HTMCS2FBK:4;
	    u32 HTMCS3FBK:4;
	    u32 HTMCS4FBK:4;
	    u32 HTMCS5FBK:4;
	    u32 HTMCS6FBK:4;
	    u32 HTMCS7FBK:4;
	} field;
	u32 word;
} HT_FBK_CFG0_STRUC;
#endif


#define HT_FBK_CFG1	0x1358
#ifdef RT_BIG_ENDIAN
typedef	union _HT_FBK_CFG1_STRUC {
	struct {
	    u32       HTMCS15FBK:4;
	    u32       HTMCS14FBK:4;
	    u32       HTMCS13FBK:4;
	    u32       HTMCS12FBK:4;
	    u32       HTMCS11FBK:4;
	    u32       HTMCS10FBK:4;
	    u32       HTMCS9FBK:4;
	    u32       HTMCS8FBK:4;
	} field;
	u32 word;
} HT_FBK_CFG1_STRUC;
#else
typedef	union _HT_FBK_CFG1_STRUC {
	struct {
	    u32       HTMCS8FBK:4;
	    u32       HTMCS9FBK:4;
	    u32       HTMCS10FBK:4;
	    u32       HTMCS11FBK:4;
	    u32       HTMCS12FBK:4;
	    u32       HTMCS13FBK:4;
	    u32       HTMCS14FBK:4;
	    u32       HTMCS15FBK:4;
	} field;
	u32 word;
} HT_FBK_CFG1_STRUC;
#endif


#define LG_FBK_CFG0	0x135c
#ifdef RT_BIG_ENDIAN
typedef	union _LG_FBK_CFG0_STRUC {
	struct {
	    u32       OFDMMCS7FBK:4;
	    u32       OFDMMCS6FBK:4;
	    u32       OFDMMCS5FBK:4
	    u32       OFDMMCS4FBK:4;
	    u32       OFDMMCS3FBK:4;
	    u32       OFDMMCS2FBK:4;
	    u32       OFDMMCS1FBK:4;
	    u32       OFDMMCS0FBK:4;
	} field;
	u32 word;
} LG_FBK_CFG0_STRUC;
#else
typedef	union _LG_FBK_CFG0_STRUC {
	struct {
	    u32       OFDMMCS0FBK:4;
	    u32       OFDMMCS1FBK:4;
	    u32       OFDMMCS2FBK:4;
	    u32       OFDMMCS3FBK:4;
	    u32       OFDMMCS4FBK:4;
	    u32       OFDMMCS5FBK:4;
	    u32       OFDMMCS6FBK:4;
	    u32       OFDMMCS7FBK:4;
	} field;
	u32 word;
} LG_FBK_CFG0_STRUC;
#endif


#define LG_FBK_CFG1		0x1360
#ifdef RT_BIG_ENDIAN
typedef	union _LG_FBK_CFG1_STRUC {
	struct {
	    u32       rsv:16;
	    u32       CCKMCS3FBK:4;
	    u32       CCKMCS2FBK:4;
	    u32       CCKMCS1FBK:4;
	    u32       CCKMCS0FBK:4;
	} field;
	u32 word;
}	LG_FBK_CFG1_STRUC;
#else
typedef	union _LG_FBK_CFG1_STRUC {
	struct {
	    u32       CCKMCS0FBK:4;
	    u32       CCKMCS1FBK:4;
	    u32       CCKMCS2FBK:4;
	    u32       CCKMCS3FBK:4;
	    u32       rsv:16;
	}	field;
	u32 word;
}	LG_FBK_CFG1_STRUC;
#endif


/*======================================================= */
/*                                     Protection Paramater                                                         */
/*======================================================= */
#define ASIC_SHORTNAV	1
#define ASIC_LONGNAV	2
#define ASIC_RTS		1
#define ASIC_CTS		2

#define CCK_PROT_CFG	0x1364	/* CCK Protection */
#define OFDM_PROT_CFG	0x1368	/* OFDM Protection */
#define MM20_PROT_CFG	0x136C	/* MM20 Protection */
#define MM40_PROT_CFG	0x1370	/* MM40 Protection */
#define GF20_PROT_CFG	0x1374	/* GF20 Protection */
#define GF40_PROT_CFG	0x1378	/* GR40 Protection */
#ifdef RT_BIG_ENDIAN
typedef	union _PROT_CFG_STRUC {
	struct {
		u32		 ProtectTxop:3; /* TXOP allowance */
		u32       DynCbw:1;  /* RTS use dynamic channel bandwidth when TX signaling mode is turned on */
		u32       RtsTaSignal:1; /* RTS TA signaling mode */
	    u32       RTSThEn:1;	/*RTS threshold enable on CCK TX */
	    u32       TxopAllowGF40:1;	/*CCK TXOP allowance.0:disallow. */
	    u32       TxopAllowGF20:1;	/*CCK TXOP allowance.0:disallow. */
	    u32       TxopAllowMM40:1;	/*CCK TXOP allowance.0:disallow. */
	    u32       TxopAllowMM20:1;	/*CCK TXOP allowance. 0:disallow. */
	    u32       TxopAllowOfdm:1;	/*CCK TXOP allowance.0:disallow. */
	    u32       TxopAllowCck:1;	/*CCK TXOP allowance.0:disallow. */
	    u32       ProtectNav:2;	/*TXOP protection type for CCK TX. 0:None, 1:ShortNAVprotect,  2:LongNAVProtect, 3:rsv */
	    u32       ProtectCtrl:2;	/*Protection control frame type for CCK TX. 1:RTS/CTS, 2:CTS-to-self, 0:None, 3:rsv */
	    u32       ProtectRate:16;	/*Protection control frame rate for CCK TX(RTS/CTS/CFEnd). */
	} field;
	u32 word;
} PROT_CFG_STRUC;
#else
typedef	union _PROT_CFG_STRUC {
	struct {
	    u32       ProtectRate:16;
	    u32       ProtectCtrl:2;
	    u32       ProtectNav:2;
	    u32       TxopAllowCck:1;
	    u32       TxopAllowOfdm:1;
	    u32       TxopAllowMM20:1;
	    u32       TxopAllowMM40:1;
	    u32       TxopAllowGF20:1;
	    u32       TxopAllowGF40:1;
	    u32       RTSThEn:1;
		u32       RtsTaSignal:1; /* RTS TA signaling mode */
		u32       DynCbw:1;  /* RTS use dynamic channel bandwidth when TX signaling mode is turned on */
		u32		 ProtectTxop:3; /* TXOP allowance */
	} field;
	u32 word;
} PROT_CFG_STRUC;
#endif


#define EXP_CTS_TIME	0x137C
#define EXP_ACK_TIME	0x1380


#define HT_FBK_TO_LEGACY	0x1384

#define TX_FBK_LIMIT		0x1398

#define TX_AC_RTY_LIMIT		0x13cc
#define TX_AC_FBK_SPEED	0x13d0



/*  4.4 MAC RX configuration registers (offset:0x1400) */

/* RX_FILTR_CFG:  /RX configuration register */
#define RX_FILTR_CFG	0x1400
#ifdef RT_BIG_ENDIAN
typedef	union _RX_FILTR_CFG_STRUC {
	struct {
		u32 rsv:15;
		u32 DropRsvCntlType:1;
        	u32 DropBAR:1;
		u32 DropBA:1;
		u32 DropPsPoll:1;		/* Drop Ps-Poll */
		u32 DropRts:1;		/* Drop Ps-Poll */
		u32 DropCts:1;		/* Drop Ps-Poll */
		u32 DropAck:1;		/* Drop Ps-Poll */
		u32 DropCFEnd:1;		/* Drop Ps-Poll */
		u32 DropCFEndAck:1;		/* Drop Ps-Poll */
		u32 DropDuplicate:1;		/* Drop duplicate frame */
		u32 DropBcast:1;		/* Drop broadcast frames */
		u32 DropMcast:1;		/* Drop multicast frames */
		u32 DropVerErr:1;	    /* Drop version error frame */
		u32 DropNotMyBSSID:1;			/* Drop fram ToDs bit is true */
		u32 DropNotToMe:1;		/* Drop not to me unicast frame */
		u32 DropPhyErr:1;		/* Drop physical error */
		u32 DropCRCErr:1;		/* Drop CRC error */
	} field;
	u32 word;
} RX_FILTR_CFG_STRUC;
#else
typedef	union _RX_FILTR_CFG_STRUC {
	struct {
		u32 DropCRCErr:1;
		u32 DropPhyErr:1;
		u32 DropNotToMe:1;
		u32 DropNotMyBSSID:1;
		u32 DropVerErr:1;
		u32 DropMcast:1;
		u32 DropBcast:1;
		u32 DropDuplicate:1;
		u32 DropCFEndAck:1;
		u32 DropCFEnd:1;
		u32 DropAck:1;
		u32 DropCts:1;
		u32 DropRts:1;
		u32 DropPsPoll:1;
		u32 DropBA:1;
        	u32  DropBAR:1;
		u32 DropRsvCntlType:1;
		u32 rsv:15;
	} field;
	u32 word;
}	RX_FILTR_CFG_STRUC;
#endif


/* AUTO_RSP_CFG: Auto-Responder */
#define AUTO_RSP_CFG	0x1404
#ifdef RT_BIG_ENDIAN
typedef union _AUTO_RSP_CFG_STRUC {
	struct {
		u32        :24;
		u32       AckCtsPsmBit:1;   /* Power bit value in conrtrol frame */
		u32       DualCTSEn:1;   /* Power bit value in conrtrol frame */
		u32       rsv:1;   /* Power bit value in conrtrol frame */
		u32       AutoResponderPreamble:1;    /* 0:long, 1:short preamble */
		u32       CTS40MRef:1;  /* Response CTS 40MHz duplicate mode */
		u32       CTS40MMode:1;  /* Response CTS 40MHz duplicate mode */
		u32       BACAckPolicyEnable:1;    /* 0:long, 1:short preamble */
		u32       AutoResponderEnable:1;
	} field;
	u32 word;
} AUTO_RSP_CFG_STRUC;
#else
typedef union _AUTO_RSP_CFG_STRUC {
	struct {
		u32       AutoResponderEnable:1;
		u32       BACAckPolicyEnable:1;
		u32       CTS40MMode:1;
		u32       CTS40MRef:1;
		u32       AutoResponderPreamble:1;
		u32       rsv:1;
		u32       DualCTSEn:1;
		u32       AckCtsPsmBit:1;
		u32        :24;
	} field;
	u32   word;
} AUTO_RSP_CFG_STRUC;
#endif


#define LEGACY_BASIC_RATE	0x1408
#define HT_BASIC_RATE		0x140c
#define HT_CTRL_CFG			0x1410
#define SIFS_COST_CFG		0x1414
#define RX_PARSER_CFG		0x1418	/*Set NAV for all received frames */

#define EXT_CCA_CFG			0x141c

/*  4.5 MAC Security configuration (offset:0x1500) */
#define TX_SEC_CNT0		0x1500
#define RX_SEC_CNT0		0x1504
#define CCMP_FC_MUTE	0x1508


/*  4.6 HCCA/PSMP (offset:0x1600) */
#define TXOP_HLDR_ADDR0		0x1600
#define TXOP_HLDR_ADDR1		0x1604
#define TXOP_HLDR_ET			0x1608
#define QOS_CFPOLL_RA_DW0		0x160c
#define QOS_CFPOLL_A1_DW1		0x1610
#define QOS_CFPOLL_QC			0x1614


/*  4.7 MAC Statistis registers (offset:0x1700) */
/* RX_STA_CNT0_STRUC: RX PLCP error count & RX CRC error count */
#define RX_STA_CNT0		0x1700
#ifdef RT_BIG_ENDIAN
typedef	union _RX_STA_CNT0_STRUC {
	struct {
	    UINT16  PhyErr;
	    UINT16  CrcErr;
	} field;
	u32 word;
} RX_STA_CNT0_STRUC;
#else
typedef	union _RX_STA_CNT0_STRUC {
	struct {
	    UINT16  CrcErr;
	    UINT16  PhyErr;
	} field;
	u32 word;
} RX_STA_CNT0_STRUC;
#endif


/* RX_STA_CNT1_STRUC: RX False CCA count & RX LONG frame count */
#define RX_STA_CNT1		0x1704
#ifdef RT_BIG_ENDIAN
typedef	union _RX_STA_CNT1_STRUC {
	struct {
	    UINT16  PlcpErr;
	    UINT16  FalseCca;
	} field;
	u32 word;
} RX_STA_CNT1_STRUC;
#else
typedef	union _RX_STA_CNT1_STRUC {
	struct {
	    UINT16  FalseCca;
	    UINT16  PlcpErr;
	} field;
	u32 word;
} RX_STA_CNT1_STRUC;
#endif


/* RX_STA_CNT2_STRUC: */
#define RX_STA_CNT2		0x1708
#ifdef RT_BIG_ENDIAN
typedef	union _RX_STA_CNT2_STRUC {
	struct {
	    UINT16  RxFifoOverflowCount;
	    UINT16  RxDupliCount;
	} field;
	u32 word;
} RX_STA_CNT2_STRUC;
#else
typedef	union _RX_STA_CNT2_STRUC {
	struct {
	    UINT16  RxDupliCount;
	    UINT16  RxFifoOverflowCount;
	} field;
	u32 word;
} RX_STA_CNT2_STRUC;
#endif


/* STA_CSR3: TX Beacon count */
#define TX_STA_CNT0		0x170C
#ifdef RT_BIG_ENDIAN
typedef	union _TX_STA_CNT0_STRUC {
	struct {
	    UINT16  TxBeaconCount;
	    UINT16  TxFailCount;
	} field;
	u32 word;
} TX_STA_CNT0_STRUC;
#else
typedef	union _TX_STA_CNT0_STRUC {
	struct {
	    UINT16  TxFailCount;
	    UINT16  TxBeaconCount;
	} field;
	u32 word;
} TX_STA_CNT0_STRUC;
#endif



/* TX_STA_CNT1: TX tx count */
#define TX_STA_CNT1		0x1710
#ifdef RT_BIG_ENDIAN
typedef	union _TX_STA_CNT1_STRUC {
	struct {
	    UINT16  TxRetransmit;
	    UINT16  TxSuccess;
	} field;
	u32 word;
} TX_STA_CNT1_STRUC;
#else
typedef	union _TX_STA_CNT1_STRUC {
	struct {
	    UINT16  TxSuccess;
	    UINT16  TxRetransmit;
	} field;
	u32 word;
} TX_STA_CNT1_STRUC;
#endif


/* TX_STA_CNT2: TX tx count */
#define TX_STA_CNT2		0x1714
#ifdef RT_BIG_ENDIAN
typedef	union _TX_STA_CNT2_STRUC {
	struct {
	    UINT16  TxUnderFlowCount;
	    UINT16  TxZeroLenCount;
	} field;
	u32 word;
} TX_STA_CNT2_STRUC;
#else
typedef	union _TX_STA_CNT2_STRUC {
	struct {
	    UINT16  TxZeroLenCount;
	    UINT16  TxUnderFlowCount;
	} field;
	u32 word;
} TX_STA_CNT2_STRUC;
#endif


/* TX_STA_FIFO_STRUC: TX Result for specific PID status fifo register */
#define TX_STA_FIFO		0x1718
#ifdef RT_BIG_ENDIAN
typedef	union _TX_STA_FIFO_STRUC {
	struct {
		u32		Reserve:2;
		u32		iTxBF:1; /* iTxBF enable */
		u32		Sounding:1; /* Sounding enable */
		u32		eTxBF:1; /* eTxBF enable */
		u32		SuccessRate:11;	/*include MCS, mode ,shortGI, BW settingSame format as TXWI Word 0 Bit 31-16. */
		u32		wcid:8;		/*wireless client index */
		u32       	TxAckRequired:1;    /* ack required */
		u32       	TxAggre:1;    /* Tx is aggregated */
		u32       	TxSuccess:1;   /* Tx success. whether success or not */
		u32       	PidType:4;
		u32       	bValid:1;   /* 1:This register contains a valid TX result */
	} field;
	u32 word;
} TX_STA_FIFO_STRUC;
#else
typedef	union _TX_STA_FIFO_STRUC {
	struct {
		u32       	bValid:1;
		u32       	PidType:4;
		u32       	TxSuccess:1;
		u32       	TxAggre:1;
		u32       	TxAckRequired:1;
		u32		wcid:8;
		u32		SuccessRate:11;
		u32		eTxBF:1;
		u32		Sounding:1;
		u32		iTxBF:1;
		u32		Reserve:2;
	} field;
	u32 word;
} TX_STA_FIFO_STRUC;
#endif


/*
	Debug counters
*/
#define TX_AGG_CNT		0x171c
#ifdef RT_BIG_ENDIAN
typedef	union _TX_NAG_AGG_CNT_STRUC {
	struct {
	    UINT16  AggTxCount;
	    UINT16  NonAggTxCount;
	} field;
	u32 word;
} TX_NAG_AGG_CNT_STRUC;
#else
typedef	union _TX_NAG_AGG_CNT_STRUC {
	struct {
	    UINT16  NonAggTxCount;
	    UINT16  AggTxCount;
	} field;
	u32 word;
} TX_NAG_AGG_CNT_STRUC;
#endif


#define TX_AGG_CNT0	0x1720
#ifdef RT_BIG_ENDIAN
typedef	union _TX_AGG_CNT0_STRUC {
	struct {
	    UINT16  AggSize2Count;
	    UINT16  AggSize1Count;
	} field;
	u32 word;
} TX_AGG_CNT0_STRUC;
#else
typedef	union _TX_AGG_CNT0_STRUC {
	struct {
	    UINT16  AggSize1Count;
	    UINT16  AggSize2Count;
	} field;
	u32 word;
} TX_AGG_CNT0_STRUC;
#endif


#define TX_AGG_CNT1	0x1724
#ifdef RT_BIG_ENDIAN
typedef	union _TX_AGG_CNT1_STRUC {
	struct {
	    UINT16  AggSize4Count;
	    UINT16  AggSize3Count;
	} field;
	u32 word;
} TX_AGG_CNT1_STRUC;
#else
typedef	union _TX_AGG_CNT1_STRUC {
	struct {
	    UINT16  AggSize3Count;
	    UINT16  AggSize4Count;
	} field;
	u32 word;
} TX_AGG_CNT1_STRUC;
#endif


#define TX_AGG_CNT2	0x1728
#ifdef RT_BIG_ENDIAN
typedef	union _TX_AGG_CNT2_STRUC {
	struct {
	    UINT16  AggSize6Count;
	    UINT16  AggSize5Count;
	} field;
	u32 word;
} TX_AGG_CNT2_STRUC;
#else
typedef	union _TX_AGG_CNT2_STRUC {
	struct {
	    UINT16  AggSize5Count;
	    UINT16  AggSize6Count;
	} field;
	u32 word;
} TX_AGG_CNT2_STRUC;
#endif


#define TX_AGG_CNT3	0x172c
#ifdef RT_BIG_ENDIAN
typedef	union _TX_AGG_CNT3_STRUC {
	struct {
	    UINT16  AggSize8Count;
	    UINT16  AggSize7Count;
	} field;
	u32 word;
} TX_AGG_CNT3_STRUC;
#else
typedef	union _TX_AGG_CNT3_STRUC {
	struct {
	    UINT16  AggSize7Count;
	    UINT16  AggSize8Count;
	} field;
	u32 word;
} TX_AGG_CNT3_STRUC;
#endif


#define TX_AGG_CNT4	0x1730
#ifdef RT_BIG_ENDIAN
typedef	union _TX_AGG_CNT4_STRUC {
	struct {
	    UINT16  AggSize10Count;
	    UINT16  AggSize9Count;
	} field;
	u32 word;
} TX_AGG_CNT4_STRUC;
#else
typedef	union _TX_AGG_CNT4_STRUC {
	struct {
	    UINT16  AggSize9Count;
	    UINT16  AggSize10Count;
	} field;
	u32 word;
} TX_AGG_CNT4_STRUC;
#endif


#define TX_AGG_CNT5	0x1734
#ifdef RT_BIG_ENDIAN
typedef	union _TX_AGG_CNT5_STRUC {
	struct {
	    UINT16  AggSize12Count;
	    UINT16  AggSize11Count;
	} field;
	u32 word;
} TX_AGG_CNT5_STRUC;
#else
typedef	union _TX_AGG_CNT5_STRUC {
	struct {
	    UINT16  AggSize11Count;
	    UINT16  AggSize12Count;
	} field;
	u32 word;
} TX_AGG_CNT5_STRUC;
#endif


#define TX_AGG_CNT6		0x1738
#ifdef RT_BIG_ENDIAN
typedef	union _TX_AGG_CNT6_STRUC {
	struct {
	    UINT16  AggSize14Count;
	    UINT16  AggSize13Count;
	} field;
	u32 word;
} TX_AGG_CNT6_STRUC;
#else
typedef	union _TX_AGG_CNT6_STRUC {
	struct {
	    UINT16  AggSize13Count;
	    UINT16  AggSize14Count;
	} field;
	u32 word;
} TX_AGG_CNT6_STRUC;
#endif


#define TX_AGG_CNT7		0x173c
#ifdef RT_BIG_ENDIAN
typedef	union _TX_AGG_CNT7_STRUC {
	struct {
	    UINT16  AggSize16Count;
	    UINT16  AggSize15Count;
	} field;
	u32 word;
} TX_AGG_CNT7_STRUC;
#else
typedef	union _TX_AGG_CNT7_STRUC {
	struct {
	    UINT16  AggSize15Count;
	    UINT16  AggSize16Count;
	} field;
	u32 word;
} TX_AGG_CNT7_STRUC;
#endif


#define TX_AGG_CNT8	0x174c	/* AGG_SIZE  = 17,18 */
#define TX_AGG_CNT9	0x1750	/* AGG_SIZE  = 19,20 */
#define TX_AGG_CNT10	0x1754	/* AGG_SIZE  = 21,22 */
#define TX_AGG_CNT11	0x1758	/* AGG_SIZE  = 23,24 */
#define TX_AGG_CNT12	0x175c	/* AGG_SIZE  = 25,26 */
#define TX_AGG_CNT13	0x1760	/* AGG_SIZE  = 27,28 */
#define TX_AGG_CNT14	0x1764	/* AGG_SIZE  = 29,30 */
#define TX_AGG_CNT15	0x1768	/* AGG_SIZE  = 31,32 */
#ifdef RT_BIG_ENDIAN
typedef	union _TX_AGG_CNT_STRUC {
	struct {
	    UINT16  AggCnt_y;	/* the count of aggregation size = x + 1 */
	    UINT16  AggCnt_x;	/* the count of aggregation size = x */
	} field;
	u32 word;
} TX_AGG_CNT_STRUC;
#else
typedef	union _TX_AGG_CNT_STRUC {
	struct {
	    UINT16  AggCnt_x;
	    UINT16  AggCnt_y;
	} field;
	u32 word;
} TX_AGG_CNT_STRUC;
#endif

typedef	union _TX_AGG_CNTN_STRUC {
	struct {
#ifdef RT_BIG_ENDIAN
	    UINT16  AggSizeHighCount;
	    UINT16  AggSizeLowCount;
#else
	    UINT16  AggSizeLowCount;
	    UINT16  AggSizeHighCount;
#endif
	} field;
	u32 word;
} TX_AGG_CNTN_STRUC;


#define MPDU_DENSITY_CNT		0x1740
#ifdef RT_BIG_ENDIAN
typedef	union _MPDU_DEN_CNT_STRUC {
	struct {
	    UINT16  RXZeroDelCount;	/*RX zero length delimiter count */
	    UINT16  TXZeroDelCount;	/*TX zero length delimiter count */
	} field;
	u32 word;
} MPDU_DEN_CNT_STRUC;
#else
typedef	union _MPDU_DEN_CNT_STRUC {
	struct {
	    UINT16  TXZeroDelCount;
	    UINT16  RXZeroDelCount;
	} field;
	u32 word;
} MPDU_DEN_CNT_STRUC;
#endif

/* Security key table memory, base address = 0x1000 */
#define MAC_WCID_BASE		0x1800 /*8-bytes(use only 6-bytes) * 256 entry = */
#define HW_WCID_ENTRY_SIZE   8

#ifdef MCS_LUT_SUPPORT
#define MAC_MCS_LUT_BASE	0x1c00
#endif /* MCS_LUT_SUPPORT */

#ifdef RT_BIG_ENDIAN
typedef	union _SHAREDKEY_MODE_STRUC {
	struct {
		u32       Bss1Key3CipherAlg:4;
		u32       Bss1Key2CipherAlg:4;
		u32       Bss1Key1CipherAlg:4;
		u32       Bss1Key0CipherAlg:4;
		u32       Bss0Key3CipherAlg:4;
		u32       Bss0Key2CipherAlg:4;
		u32       Bss0Key1CipherAlg:4;
		u32       Bss0Key0CipherAlg:4;
	} field;
	u32 word;
} SHAREDKEY_MODE_STRUC;
#else
typedef	union _SHAREDKEY_MODE_STRUC {
	struct {
		u32       Bss0Key0CipherAlg:4;
		u32       Bss0Key1CipherAlg:4;
		u32       Bss0Key2CipherAlg:4;
		u32       Bss0Key3CipherAlg:4;
		u32       Bss1Key0CipherAlg:4;
		u32       Bss1Key1CipherAlg:4;
		u32       Bss1Key2CipherAlg:4;
		u32       Bss1Key3CipherAlg:4;
	} field;
	u32 word;
} SHAREDKEY_MODE_STRUC;
#endif


/* 64-entry for pairwise key table, 8-byte per entry  */
typedef struct _HW_WCID_ENTRY {
    u8   Address[6];
    u8   Rsv[2];
} HW_WCID_ENTRY;


/* ================================================================================= */
/* WCID  format */
/* ================================================================================= */
/*7.1	WCID  ENTRY  format  : 8bytes */
typedef	struct _WCID_ENTRY_STRUC {
	u8		RXBABitmap7;    /* bit0 for TID8, bit7 for TID 15 */
	u8		RXBABitmap0;    /* bit0 for TID0, bit7 for TID 7 */
	u8		MAC[6];	/* 0 for shared key table.  1 for pairwise key table */
} WCID_ENTRY_STRUC;


/*8.1.1	SECURITY  KEY  format  : 8DW */
/* 32-byte per entry, total 16-entry for shared key table, 64-entry for pairwise key table */
typedef struct _HW_KEY_ENTRY {
    u8   Key[16];
    u8   TxMic[8];
    u8   RxMic[8];
} HW_KEY_ENTRY;


/*8.1.2	IV/EIV  format  : 2DW */

/* RX attribute entry format  : 1DW */
#ifdef RT_BIG_ENDIAN
typedef	union _WCID_ATTRIBUTE_STRUC {
	struct {
		u32		WAPIKeyIdx:8;
		u32		WAPI_rsv:8;
		u32		WAPI_MCBC:1;
		u32		rsv:3;
		u32		BSSIdxExt:1;
		u32		PairKeyModeExt:1;
		u32		RXWIUDF:3;
		u32		BSSIdx:3; /*multipleBSS index for the WCID */
		u32		PairKeyMode:3;
		u32		KeyTab:1;	/* 0 for shared key table.  1 for pairwise key table */
	} field;
	u32 word;
} WCID_ATTRIBUTE_STRUC;
#else
typedef	union _WCID_ATTRIBUTE_STRUC {
	struct {
		u32		KeyTab:1;	/* 0 for shared key table.  1 for pairwise key table */
		u32		PairKeyMode:3;
		u32		BSSIdx:3; 		/*multipleBSS index for the WCID */
		u32		RXWIUDF:3;
		u32		PairKeyModeExt:1;
		u32		BSSIdxExt:1;
		u32		rsv:3;
		u32		WAPI_MCBC:1;
		u32		WAPI_rsv:8;
		u32		WAPIKeyIdx:8;
	} field;
	u32 word;
} WCID_ATTRIBUTE_STRUC;
#endif


/* ================================================================================= */
/* HOST-MCU communication data structure */
/* ================================================================================= */

/* H2M_MAILBOX_CSR: Host-to-MCU Mailbox */
#ifdef RT_BIG_ENDIAN
typedef union _H2M_MAILBOX_STRUC {
    struct {
        u32       Owner:8;
        u32       CmdToken:8;    /* 0xff tells MCU not to report CmdDoneInt after excuting the command */
        u32       HighByte:8;
        u32       LowByte:8;
    } field;
    u32 word;
} H2M_MAILBOX_STRUC;
#else
typedef union _H2M_MAILBOX_STRUC {
    struct {
        u32       LowByte:8;
        u32       HighByte:8;
        u32       CmdToken:8;
        u32       Owner:8;
    } field;
    u32 word;
} H2M_MAILBOX_STRUC;
#endif


/* M2H_CMD_DONE_CSR: MCU-to-Host command complete indication */
#ifdef RT_BIG_ENDIAN
typedef union _M2H_CMD_DONE_STRUC {
    struct {
        u32       CmdToken3;
        u32       CmdToken2;
        u32       CmdToken1;
        u32       CmdToken0;
    } field;
    u32 word;
} M2H_CMD_DONE_STRUC;
#else
typedef union _M2H_CMD_DONE_STRUC {
    struct {
        u32       CmdToken0;
        u32       CmdToken1;
        u32       CmdToken2;
        u32       CmdToken3;
    } field;
    u32 word;
} M2H_CMD_DONE_STRUC;
#endif


/* HOST_CMD_CSR: For HOST to interrupt embedded processor */
#ifdef RT_BIG_ENDIAN
typedef	union _HOST_CMD_CSR_STRUC {
	struct {
	    u32   Rsv:24;
	    u32   HostCommand:8;
	} field;
	u32 word;
} HOST_CMD_CSR_STRUC;
#else
typedef	union _HOST_CMD_CSR_STRUC {
	struct {
	    u32   HostCommand:8;
	    u32   Rsv:24;
	} field;
	u32 word;
} HOST_CMD_CSR_STRUC;
#endif


// TODO: shiang-6590, Need to check following definitions are useful or not!!!
/* AIFSN_CSR: AIFSN for each EDCA AC */


/* E2PROM_CSR: EEPROM control register */
#ifdef RT_BIG_ENDIAN
typedef	union _E2PROM_CSR_STRUC {
	struct {
		u32		Rsvd:25;
		u32       LoadStatus:1;   /* 1:loading, 0:done */
		u32		Type:1;			/* 1: 93C46, 0:93C66 */
		u32		EepromDO:1;
		u32		EepromDI:1;
		u32		EepromCS:1;
		u32		EepromSK:1;
		u32		Reload:1;		/* Reload EEPROM content, write one to reload, self-cleared. */
	} field;
	u32 word;
} E2PROM_CSR_STRUC;
#else
typedef	union _E2PROM_CSR_STRUC {
	struct {
		u32		Reload:1;
		u32		EepromSK:1;
		u32		EepromCS:1;
		u32		EepromDI:1;
		u32		EepromDO:1;
		u32		Type:1;
		u32       LoadStatus:1;
		u32		Rsvd:25;
	} field;
	u32 word;
} E2PROM_CSR_STRUC;
#endif


/* QOS_CSR0: TXOP holder address0 register */
#ifdef RT_BIG_ENDIAN
typedef	union _QOS_CSR0_STRUC {
	struct {
		u8		Byte3;		/* MAC address byte 3 */
		u8		Byte2;		/* MAC address byte 2 */
		u8		Byte1;		/* MAC address byte 1 */
		u8		Byte0;		/* MAC address byte 0 */
	} field;
	u32 word;
} QOS_CSR0_STRUC;
#else
typedef	union _QOS_CSR0_STRUC {
	struct {
		u8		Byte0;
		u8		Byte1;
		u8		Byte2;
		u8		Byte3;
	} field;
	u32 word;
} QOS_CSR0_STRUC;
#endif


/* QOS_CSR1: TXOP holder address1 register */
#ifdef RT_BIG_ENDIAN
typedef	union _QOS_CSR1_STRUC {
	struct {
		u8		Rsvd1;
		u8		Rsvd0;
		u8		Byte5;		/* MAC address byte 5 */
		u8		Byte4;		/* MAC address byte 4 */
	} field;
	u32 word;
} QOS_CSR1_STRUC;
#else
typedef	union _QOS_CSR1_STRUC {
	struct {
		u8		Byte4;		/* MAC address byte 4 */
		u8		Byte5;		/* MAC address byte 5 */
		u8		Rsvd0;
		u8		Rsvd1;
	} field;
	u32 word;
} QOS_CSR1_STRUC;
#endif

// TODO: shiang-6590, check upper definitions are useful or not!



/* Other on-chip shared memory space, base = 0x2000 */

/* CIS space - base address = 0x2000 */
#define HW_CIS_BASE             0x2000

/* Carrier-sense CTS frame base address. It's where mac stores carrier-sense frame for carrier-sense function. */
#define HW_CS_CTS_BASE			0x7700
/* DFS CTS frame base address. It's where mac stores CTS frame for DFS. */
#define HW_DFS_CTS_BASE			0x7780
#define HW_CTS_FRAME_SIZE		0x80

/* 2004-11-08 john - since NULL frame won't be that long (256 byte). We steal 16 tail bytes */
/* to save debugging settings */
#define HW_DEBUG_SETTING_BASE   0x77f0  /* 0x77f0~0x77ff total 16 bytes */
#define HW_DEBUG_SETTING_BASE2  0x7770  /* 0x77f0~0x77ff total 16 bytes */

/*
	On-chip BEACON frame space -
   	1. HW_BEACON_OFFSET/64B must be 0;
   	2. BCN_OFFSETx(0~) must also be changed in MACRegTable(common/rtmp_init.c)
 */
#define HW_BEACON_OFFSET		0x0200


/* 	In order to support maximum 8 MBSS and its maximum length is 512 for each beacon
	Three section discontinue memory segments will be used.
	1. The original region for BCN 0~3
	2. Extract memory from FCE table for BCN 4~5
	3. Extract memory from Pair-wise key table for BCN 6~7
  		It occupied those memory of wcid 238~253 for BCN 6
		and wcid 222~237 for BCN 7  	*/
/*#define HW_BEACON_MAX_COUNT     8 */
#define HW_BEACON_MAX_SIZE(__pAd)      ((__pAd)->chipCap.BcnMaxHwSize)
#define HW_BEACON_BASE0(__pAd)         ((__pAd)->chipCap.BcnBase[0])
/*#define HW_BEACON_BASE1         0x7A00 */
/*#define HW_BEACON_BASE2         0x7C00 */
/*#define HW_BEACON_BASE3         0x7E00 */
/*#define HW_BEACON_BASE4         0x7200 */
/*#define HW_BEACON_BASE5         0x7400 */
/*#define HW_BEACON_BASE6         0x5DC0 */
/*#define HW_BEACON_BASE7         0x5BC0 */


/* Higher 8KB shared memory */
#define HW_BEACON_BASE0_REDIRECTION	0x4000
#define HW_BEACON_BASE1_REDIRECTION	0x4200
#define HW_BEACON_BASE2_REDIRECTION	0x4400
#define HW_BEACON_BASE3_REDIRECTION	0x4600
#define HW_BEACON_BASE4_REDIRECTION	0x4800
#define HW_BEACON_BASE5_REDIRECTION	0x4A00
#define HW_BEACON_BASE6_REDIRECTION	0x4C00
#define HW_BEACON_BASE7_REDIRECTION	0x4E00


/* HOST-MCU shared memory - base address = 0x2100 */
#define HOST_CMD_CSR		0x404
#define H2M_MAILBOX_CSR         0x7010
#define H2M_MAILBOX_CID         0x7014
#define H2M_MAILBOX_STATUS      0x701c
#define H2M_INT_SRC             0x7024
#define H2M_BBP_AGENT           0x7028
#define M2H_CMD_DONE_CSR        0x000c
#define MCU_TXOP_ARRAY_BASE     0x000c   /* TODO: to be provided by Albert */
#define MCU_TXOP_ENTRY_SIZE     32       /* TODO: to be provided by Albert */
#define MAX_NUM_OF_TXOP_ENTRY   16       /* TODO: must be same with 8051 firmware */
#define MCU_MBOX_VERSION        0x01     /* TODO: to be confirmed by Albert */
#define MCU_MBOX_VERSION_OFFSET 5        /* TODO: to be provided by Albert */


/* Host DMA registers - base address 0x200 .  TX0-3=EDCAQid0-3, TX4=HCCA, TX5=MGMT, */
/*  DMA RING DESCRIPTOR */
#define E2PROM_CSR          0x0004
#define IO_CNTL_CSR         0x77d0



/* ================================================================ */
/* Tx /	Rx / Mgmt ring descriptor definition */
/* ================================================================ */

/* the following PID values are used to mark outgoing frame type in TXD->PID so that */
/* proper TX statistics can be collected based on these categories */
/* b3-2 of PID field - */
#define PID_MGMT			0x05
#define PID_BEACON			0x0c
#define PID_DATA_NORMALUCAST	 	0x02
#define PID_DATA_AMPDU	 	0x04
#define PID_DATA_NO_ACK    	0x08
#define PID_DATA_NOT_NORM_ACK	 	0x03
/* value domain of pTxD->HostQId (4-bit: 0~15) */
#define QID_AC_BK               1   /* meet ACI definition in 802.11e */
#define QID_AC_BE               0   /* meet ACI definition in 802.11e */
#define QID_AC_VI               2
#define QID_AC_VO               3
#define QID_HCCA                4
#define NUM_OF_TX_RING          5
#define QID_MGMT                13
#define QID_RX                  14
#define QID_OTHER               15




#define RTMP_MAC_SHR_MSEL_PROTECT_LOCK(__pAd, __IrqFlags)	__IrqFlags = __IrqFlags;
#define RTMP_MAC_SHR_MSEL_PROTECT_UNLOCK(__pAd, __IrqFlags) __IrqFlags = __IrqFlags;


#ifdef DFS_SUPPORT
#define BBPR127TABLE_OWNERID			0x4CA0
#define BBPR127TABLE_OFFSET				0x4D00
#endif /* DFS_SUPPORT */


struct rtmp_adapter;

INT get_pkt_phymode_by_rxwi(struct rxwi_nmac *rxwi);
INT get_pkt_rssi_by_rxwi(struct rxwi_nmac *rxwi, INT size, CHAR *rssi);
INT get_pkt_snr_by_rxwi(struct rxwi_nmac *rxwi, INT size, u8 *snr);

void mt7610u_mac_set_ctrlch(struct rtmp_adapter*pAd, int extch);
INT rtmp_mac_set_mmps(struct  rtmp_adapter *pAd, INT ReduceCorePower);

#endif /* __RTMP_MAC_H__ */

