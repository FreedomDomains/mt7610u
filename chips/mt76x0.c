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


#include "rt_config.h"

#define MT7650_EFUSE_CTRL	0x0024
#define LDO_CTRL1			0x0070

static RTMP_REG_PAIR	MT76x0_MACRegTable[] = {
	{ PBF_SYS_CTRL,		0x80c00 },
	{ PBF_CFG,		0x77723c1f },
	{ FCE_PSE_CTRL,		0x1 },

	{ AMPDU_MAX_LEN_20M1S,	0xBAA99887 },	/* Recommended by JerryCK @20120905 */

	{ TX_SW_CFG0,		0x601 },	/* Delay bb_tx_pe for proper tx_mcs_pwr update */
	{ TX_SW_CFG1,		0x00040000 },	/* Set rf_tx_pe deassert time to 1us by Chee's comment @MT7650_CR_setting_1018.xlsx */
	{TX_SW_CFG2,		0x0 },

// TODO: shiang-6590, check what tx report will send to us when following default value set as 2
	{ 0xa44,		0x0 },		/* disable Tx info report */


	{ HEADER_TRANS_CTRL_REG, 0x0 },
	{ TSO_CTRL,		0x0 },

	/* BB_PA_MODE_CFG0(0x1214) Keep default value @20120903 */
	{ BB_PA_MODE_CFG1,	0x00500055 },

	/* RF_PA_MODE_CFG0(0x121C) Keep default value @20120903 */
	{ RF_PA_MODE_CFG1,	0x00500055 },

	{ TX_ALC_CFG_0,		0x2F2F000C },
	{ TX0_BB_GAIN_ATTEN,	0x00000000 },	/* set BBP atten gain = 0 */

	{ TX_PWR_CFG_0,		0x3A3A3A3A },
	{ TX_PWR_CFG_1,		0x3A3A3A3A },
	{ TX_PWR_CFG_2,		0x3A3A3A3A },
	{ TX_PWR_CFG_3,		0x3A3A3A3A },
	{ TX_PWR_CFG_4,		0x3A3A3A3A },
	{ TX_PWR_CFG_7,		0x3A3A3A3A },
	{ TX_PWR_CFG_8,		0x3A },
	{ TX_PWR_CFG_9,		0x3A },
	{ 0x150C,		0x00000002 },	/*Enable Tx length > 4095 byte */
	{ 0x1238,		0x001700C8 },	/* Disable bt_abort_tx_en(0x1238[21] = 0) which is not used at MT7650 @MT7650_E3_CR_setting_1115.xlsx */
	{ LDO_CTRL1,		0x6B006464 },	/* Default LDO_DIG supply 1.26V, change to 1.2V */
};

static u8 MT76x0_NUM_MAC_REG_PARMS = (sizeof(MT76x0_MACRegTable) / sizeof(RTMP_REG_PAIR));

static RTMP_REG_PAIR MT76x0_DCOC_Tab[] = {
	{ CAL_R47,	0x000010F0 },
	{ CAL_R48,	0x00008080 },
	{ CAL_R49,	0x00000F07 },
	{ CAL_R50,	0x00000040 },
	{ CAL_R51,	0x00000404 },
	{ CAL_R52,	0x00080803 },
	{ CAL_R53,	0x00000704 },
	{ CAL_R54,	0x00002828 },
	{ CAL_R55,	0x00005050 },
};
static u8 MT76x0_DCOC_Tab_Size = (sizeof(MT76x0_DCOC_Tab) / sizeof(RTMP_REG_PAIR));

static RTMP_REG_PAIR MT76x0_BBP_Init_Tab[] = {
	{ CORE_R1,	0x00000002 },
	{ CORE_R4,	0x00000000 },
	{ CORE_R24,	0x00000000 },
	{ CORE_R32,	0x4003000a },
	{ CORE_R42,	0x00000000 },
	{ CORE_R44,	0x00000000 },

	{ IBI_R11,	0x00000080 },

	/*
	 *	0x2300[5] Default Antenna:
	 *	0 for WIFI main antenna
	 *	1 for WIFI aux  antenna
	*/

	{ AGC1_R0,	0x00021400 },
	{ AGC1_R1,	0x00000003 },
	{ AGC1_R2,	0x003A6464 },
	{ AGC1_R15,	0x88A28CB8 },
	{ AGC1_R22,	0x00001E21 },
	{ AGC1_R23,	0x0000272C },
	{ AGC1_R24,	0x00002F3A },
	{ AGC1_R25,	0x8000005A },
	{ AGC1_R26,	0x007C2005 },
	{ AGC1_R34,	0x000A0C0C },
	{ AGC1_R37,	0x2121262C },
	{ AGC1_R41,	0x38383E45 },
	{ AGC1_R57,	0x00001010 },
	{ AGC1_R59,	0xBAA20E96 },
	{ AGC1_R63,	0x00000001 },

	{ TXC_R0,	0x00280403 },
	{ TXC_R1,	0x00000000 },

	{ RXC_R1,	0x00000012 },
	{ RXC_R2,	0x00000011 },
	{ RXC_R3,	0x00000005 },
	{ RXC_R4,	0x00000000 },
	{ RXC_R5,	0xF977C4EC },
	{ RXC_R7,	0x00000090 },

	{ TXO_R8,	0x00000000 },

	{ TXBE_R0,	0x00000000 },
	{ TXBE_R4,	0x00000004 },
	{ TXBE_R6,	0x00000000 },
	{ TXBE_R8,	0x00000014 },
	{ TXBE_R9,	0x20000000 },
	{ TXBE_R10,	0x00000000 },
	{ TXBE_R12,	0x00000000 },
	{ TXBE_R13,	0x00000000 },
	{ TXBE_R14,	0x00000000 },
	{ TXBE_R15,	0x00000000 },
	{ TXBE_R16,	0x00000000 },
	{ TXBE_R17,	0x00000000 },

	{ RXFE_R1,	0x00008800 }, /* Add for E3 */
	{ RXFE_R3,	0x00000000 },
	{ RXFE_R4,	0x00000000 },

	{ RXO_R13,	0x00000092 },
	{ RXO_R14,	0x00060612 },
	{ RXO_R15,	0xC8321B18 },
	{ RXO_R16,	0x0000001E },
	{ RXO_R17,	0x00000000 },
	{ RXO_R18,	0xCC00A993 },
	{ RXO_R19,	0xB9CB9CB9 },
	{ RXO_R20,	0x26c00057 }, /* IOT issue with Broadcom AP */
	{ RXO_R21,	0x00000001 },
	{ RXO_R24,	0x00000006 },
};
static u8 MT76x0_BBP_Init_Tab_Size = (sizeof(MT76x0_BBP_Init_Tab) / sizeof(RTMP_REG_PAIR));


MT76x0_BBP_Table MT76x0_BPP_SWITCH_Tab[] = {
	{RF_G_BAND | RF_BW_20 | RF_BW_40,		{ AGC1_R8, 0x0E344EF0} },
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ AGC1_R8, 0x122C54F2} },

	{RF_G_BAND | RF_BW_20 | RF_BW_40,		{ AGC1_R14, 0x310F2E39} },
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ AGC1_R14, 0x310F2A3F} },

	{RF_G_BAND | RF_BW_20 | RF_BW_40,		{ AGC1_R32, 0x00003230} },
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ AGC1_R32, 0x0000181C} },

	{RF_G_BAND | RF_BW_20 | RF_BW_40,		{ AGC1_R33, 0x00003240} },
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ AGC1_R33, 0x00003218} },

	{RF_G_BAND | RF_BW_20 | RF_BW_40,		{ AGC1_R35, 0x11112016} },
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ AGC1_R35, 0x11112016} },

	{RF_G_BAND | RF_BW_20 | RF_BW_40,		{ RXO_R28, 0x0000008A } },
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ RXO_R28, 0x0000008A } },

	{RF_G_BAND | RF_BW_20 | RF_BW_40,		{ AGC1_R4, 0x1FEDA049 } },
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ AGC1_R4, 0x1FECA054 } },

	{RF_G_BAND | RF_BW_20 | RF_BW_40,		{ AGC1_R6, 0x00000045 } },
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ AGC1_R6, 0x0000000A } },

	{RF_G_BAND | RF_BW_20,				{ AGC1_R12, 0x05052879 } },
	{RF_G_BAND | RF_BW_40,				{ AGC1_R12, 0x050528F9 } },
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ AGC1_R12, 0x050528F9 } },

	{RF_G_BAND | RF_BW_20 | RF_BW_40,		{ AGC1_R13, 0x35050004 } },
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ AGC1_R13, 0x2C3A0406 } },

	{RF_G_BAND | RF_BW_20 | RF_BW_40,		{AGC1_R27, 0x000000E1 } },
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{AGC1_R27, 0x000000EC } },

	{RF_G_BAND | RF_BW_20,				{ AGC1_R28, 0x00060806} },
	{RF_G_BAND | RF_BW_40,				{ AGC1_R28, 0x00050806} },
	{RF_A_BAND | RF_BW_40,				{ AGC1_R28, 0x00060801} },
	{RF_A_BAND | RF_BW_20 | RF_BW_80,		{ AGC1_R28, 0x00060806} },

	{RF_G_BAND | RF_BW_20 | RF_BW_40,		{ AGC1_R31, 0x00000F23} },
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ AGC1_R31, 0x00000F13} },

	{RF_G_BAND | RF_BW_20,				{ AGC1_R39, 0x2A2A3036} },
	{RF_G_BAND | RF_BW_40,				{ AGC1_R39, 0x2A2A2C36} },
	{RF_A_BAND | RF_BW_20 | RF_BW_40,		{ AGC1_R39, 0x2A2A3036} },
	{RF_A_BAND | RF_BW_80,				{AGC1_R39, 0x2A2A2A36} },

	{RF_G_BAND | RF_BW_20,				{AGC1_R43, 0x27273438} },
	{RF_G_BAND | RF_BW_40,				{AGC1_R43, 0x27272D38} },
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{AGC1_R43, 0x27272B30} },

	{RF_G_BAND | RF_BW_20 | RF_BW_40,		{ AGC1_R51, 0x17171C1C} },
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ AGC1_R51, 0xFFFFFFFF} },

	{RF_G_BAND | RF_BW_20,				{ AGC1_R53, 0x26262A2F} },
	{RF_G_BAND | RF_BW_40,				{ AGC1_R53, 0x2626322F} },
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ AGC1_R53, 0xFFFFFFFF} },

	{RF_G_BAND | RF_BW_20,				{ AGC1_R55, 0x40404E58} },
	{RF_G_BAND | RF_BW_40,				{ AGC1_R55, 0x40405858} },
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ AGC1_R55, 0xFFFFFFFF} },

	{RF_G_BAND | RF_BW_20 | RF_BW_40,		{ AGC1_R58, 0x00001010} },
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ AGC1_R58, 0x00000000} },

	{RF_G_BAND | RF_BW_20 | RF_BW_40,		{ RXFE_R0, 0x3D5000E0} },
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ RXFE_R0, 0x895000E0} },
};

u8 MT76x0_BPP_SWITCH_Tab_Size = (sizeof(MT76x0_BPP_SWITCH_Tab) / sizeof(MT76x0_BBP_Table));

/* Bank	Register Value(Hex) */
static BANK_RF_REG_PAIR MT76x0_RF_Central_RegTb[] = {
/*
	Bank 0 - For central blocks: BG, PLL, XTAL, LO, ADC/DAC
*/
	{ RF_BANK0,	RF_R01,	0x01 },
	{ RF_BANK0,	RF_R02,	0x11 },

	/*
		R3 ~ R7: VCO Cal.
	*/
	{ RF_BANK0,	RF_R03, 0x73 }, /* VCO Freq Cal - No Bypass, VCO Amp Cal - No Bypass */
	{ RF_BANK0,	RF_R04, 0x30 }, /* R4 b<7>=1, VCO cal */
	{ RF_BANK0,	RF_R05, 0x00 },
	{ RF_BANK0,	RF_R06, 0x41 }, /* Set the open loop amplitude to middle since bypassing amplitude calibration */
	{ RF_BANK0,	RF_R07, 0x00 },

	/*
		XO
	*/
	{ RF_BANK0,	RF_R08, 0x00 },
	{ RF_BANK0,	RF_R09, 0x00 },
	{ RF_BANK0,	RF_R10, 0x0C },
	{ RF_BANK0,	RF_R11, 0x00 },
	{ RF_BANK0,	RF_R12, 0x00 },

	/*
		BG
	*/
	{ RF_BANK0,	RF_R13, 0x00 },
	{ RF_BANK0,	RF_R14, 0x00 },
	{ RF_BANK0,	RF_R15, 0x00 },

	/*
		LDO
	*/
	{ RF_BANK0,	RF_R19, 0x20 },
	/*
		XO
	*/
	{ RF_BANK0,	RF_R20, 0x22 },
	{ RF_BANK0,	RF_R21, 0x12 },
	{ RF_BANK0,	RF_R23, 0x00 },
	{ RF_BANK0,	RF_R24, 0x33 }, /* See band selection for R24<1:0> */
	{ RF_BANK0,	RF_R25, 0x00 },

	/*
		PLL, See Freq Selection
	*/
	{ RF_BANK0,	RF_R26, 0x00 },
	{ RF_BANK0,	RF_R27, 0x00 },
	{ RF_BANK0,	RF_R28, 0x00 },
	{ RF_BANK0,	RF_R29, 0x00 },
	{ RF_BANK0,	RF_R30, 0x00 },
	{ RF_BANK0,	RF_R31, 0x00 },
	{ RF_BANK0,	RF_R32, 0x00 },
	{ RF_BANK0,	RF_R33, 0x00 },
	{ RF_BANK0,	RF_R34, 0x00 },
	{ RF_BANK0,	RF_R35, 0x00 },
	{ RF_BANK0,	RF_R36, 0x00 },
	{ RF_BANK0,	RF_R37, 0x00 },

	/*
		LO Buffer
	*/
	{ RF_BANK0,	RF_R38, 0x2F },

	/*
		Test Ports
	*/
	{ RF_BANK0,	RF_R64, 0x00 },
	{ RF_BANK0,	RF_R65, 0x80 },
	{ RF_BANK0,	RF_R66, 0x01 },
	{ RF_BANK0,	RF_R67, 0x04 },

	/*
		ADC/DAC
	*/
	{ RF_BANK0,	RF_R68, 0x00 },
	{ RF_BANK0,	RF_R69, 0x08 },
	{ RF_BANK0,	RF_R70, 0x08 },
	{ RF_BANK0,	RF_R71, 0x40 },
	{ RF_BANK0,	RF_R72, 0xD0 },
	{ RF_BANK0,	RF_R73, 0x93 },
};
static u32 MT76x0_RF_Central_RegTb_Size = (sizeof(MT76x0_RF_Central_RegTb) / sizeof(BANK_RF_REG_PAIR));

static BANK_RF_REG_PAIR MT76x0_RF_2G_Channel_0_RegTb[] = {
/*
	Bank 5 - Channel 0 2G RF registers
*/
	/*
		RX logic operation
	*/
	/* RF_R00 Change in SelectBand6590 */

	{ RF_BANK5,	RF_R02, 0x0C }, /* 5G+2G (MT7610U) */
	{ RF_BANK5,	RF_R03, 0x00 },

	/*
		TX logic operation
	*/
	{ RF_BANK5,	RF_R04, 0x00 },
	{ RF_BANK5,	RF_R05, 0x84 },
	{ RF_BANK5,	RF_R06, 0x02 },

	/*
		LDO
	*/
	{ RF_BANK5,	RF_R07, 0x00 },
	{ RF_BANK5,	RF_R08, 0x00 },
	{ RF_BANK5,	RF_R09, 0x00 },

	/*
		RX
	*/
	{ RF_BANK5,	RF_R10, 0x51 },
	{ RF_BANK5,	RF_R11, 0x22 },
	{ RF_BANK5,	RF_R12, 0x22 },
	{ RF_BANK5,	RF_R13, 0x0F },
	{ RF_BANK5,	RF_R14, 0x47 }, /* Increase mixer current for more gain */
	{ RF_BANK5,	RF_R15, 0x25 },
	{ RF_BANK5,	RF_R16, 0xC7 }, /* Tune LNA2 tank */
	{ RF_BANK5,	RF_R17, 0x00 },
	{ RF_BANK5,	RF_R18, 0x00 },
	{ RF_BANK5,	RF_R19, 0x30 }, /* Improve max Pin */
	{ RF_BANK5,	RF_R20, 0x33 },
	{ RF_BANK5,	RF_R21, 0x02 },
	{ RF_BANK5,	RF_R22, 0x32 }, /* Tune LNA1 tank */
	{ RF_BANK5,	RF_R23, 0x00 },
	{ RF_BANK5,	RF_R24, 0x25 },
	{ RF_BANK5,	RF_R26, 0x00 },
	{ RF_BANK5,	RF_R27, 0x12 },
	{ RF_BANK5,	RF_R28, 0x0F },
	{ RF_BANK5,	RF_R29, 0x00 },

	/*
		LOGEN
	*/
	{ RF_BANK5,	RF_R30, 0x51 }, /* Tune LOGEN tank */
	{ RF_BANK5,	RF_R31, 0x35 },
	{ RF_BANK5,	RF_R32, 0x31 },
	{ RF_BANK5,	RF_R33, 0x31 },
	{ RF_BANK5,	RF_R34, 0x34 },
	{ RF_BANK5,	RF_R35, 0x03 },
	{ RF_BANK5,	RF_R36, 0x00 },

	/*
		TX
	*/
	{ RF_BANK5,	RF_R37, 0xDD }, /* Improve 3.2GHz spur */
	{ RF_BANK5,	RF_R38, 0xB3 },
	{ RF_BANK5,	RF_R39, 0x33 },
	{ RF_BANK5,	RF_R40, 0xB1 },
	{ RF_BANK5,	RF_R41, 0x71 },
	{ RF_BANK5,	RF_R42, 0xF2 },
	{ RF_BANK5,	RF_R43, 0x47 },
	{ RF_BANK5,	RF_R44, 0x77 },
	{ RF_BANK5,	RF_R45, 0x0E },
	{ RF_BANK5,	RF_R46, 0x10 },
	{ RF_BANK5,	RF_R47, 0x00 },
	{ RF_BANK5,	RF_R48, 0x53 },
	{ RF_BANK5,	RF_R49, 0x03 },
	{ RF_BANK5,	RF_R50, 0xEF },
	{ RF_BANK5,	RF_R51, 0xC7 },
	{ RF_BANK5,	RF_R52, 0x62 },
	{ RF_BANK5,	RF_R53, 0x62 },
	{ RF_BANK5,	RF_R54, 0x00 },
	{ RF_BANK5,	RF_R55, 0x00 },
	{ RF_BANK5,	RF_R56, 0x0F },
	{ RF_BANK5,	RF_R57, 0x0F },
	{ RF_BANK5,	RF_R58, 0x16 },
	{ RF_BANK5,	RF_R59, 0x16 },
	{ RF_BANK5,	RF_R60, 0x10 },
	{ RF_BANK5,	RF_R61, 0x10 },
	{ RF_BANK5,	RF_R62, 0xD0 },
	{ RF_BANK5,	RF_R63, 0x6C },
	{ RF_BANK5,	RF_R64, 0x58 },
	{ RF_BANK5, 	RF_R65, 0x58 },
	{ RF_BANK5,	RF_R66, 0xF2 },
	{ RF_BANK5,	RF_R67, 0xE8 },
	{ RF_BANK5,	RF_R68, 0xF0 },
	{ RF_BANK5,	RF_R69, 0xF0 },
	{ RF_BANK5,	RF_R127, 0x04 },
};
static u32 MT76x0_RF_2G_Channel_0_RegTb_Size = (sizeof(MT76x0_RF_2G_Channel_0_RegTb) / sizeof(BANK_RF_REG_PAIR));

static BANK_RF_REG_PAIR MT76x0_RF_5G_Channel_0_RegTb[] = {
/*
	Bank 6 - Channel 0 5G RF registers
*/
	/*
		RX logic operation
	*/
	/* RF_R00 Change in SelectBandMT76x0 */

	{ RF_BANK6,	RF_R02, 0x0C },
	{ RF_BANK6,	RF_R03, 0x00 },

	/*
		TX logic operation
	*/
	{ RF_BANK6,	RF_R04, 0x00 },
	{ RF_BANK6,	RF_R05, 0x84 },
	{ RF_BANK6,	RF_R06, 0x02 },

	/*
		LDO
	*/
	{ RF_BANK6,	RF_R07, 0x00 },
	{ RF_BANK6,	RF_R08, 0x00 },
	{ RF_BANK6,	RF_R09, 0x00 },

	/*
		RX
	*/
	{ RF_BANK6,	RF_R10, 0x00 },
	{ RF_BANK6,	RF_R11, 0x01 },

	{ RF_BANK6,	RF_R13, 0x23 },
	{ RF_BANK6,	RF_R14, 0x00 },
	{ RF_BANK6,	RF_R15, 0x04 },
	{ RF_BANK6,	RF_R16, 0x22 },

	{ RF_BANK6,	RF_R18, 0x08 },
	{ RF_BANK6,	RF_R19, 0x00 },
	{ RF_BANK6,	RF_R20, 0x00 },
	{ RF_BANK6,	RF_R21, 0x00 },
	{ RF_BANK6,	RF_R22, 0xFB },

	/*
		LOGEN5G
	*/
	{ RF_BANK6,	RF_R25, 0x76 },
	{ RF_BANK6,	RF_R26, 0x24 },
	{ RF_BANK6,	RF_R27, 0x04 },
	{ RF_BANK6,	RF_R28, 0x00 },
	{ RF_BANK6,	RF_R29, 0x00 },

	/*
		TX
	*/
	{ RF_BANK6,	RF_R37, 0xBB },
	{ RF_BANK6,	RF_R38, 0xB3 },

	{ RF_BANK6,	RF_R40, 0x33 },
	{ RF_BANK6,	RF_R41, 0x33 },

	{ RF_BANK6,	RF_R43, 0x03 },
	{ RF_BANK6,	RF_R44, 0xB3 },

	{ RF_BANK6,	RF_R46, 0x17 },
	{ RF_BANK6,	RF_R47, 0x0E },
	{ RF_BANK6,	RF_R48, 0x10 },
	{ RF_BANK6,	RF_R49, 0x07 },

	{ RF_BANK6,	RF_R62, 0x00 },
	{ RF_BANK6,	RF_R63, 0x00 },
	{ RF_BANK6,	RF_R64, 0xF1 },
	{ RF_BANK6,	RF_R65, 0x0F },
};
static u32 MT76x0_RF_5G_Channel_0_RegTb_Size = (sizeof(MT76x0_RF_5G_Channel_0_RegTb) / sizeof(BANK_RF_REG_PAIR));

static BANK_RF_REG_PAIR MT76x0_RF_VGA_Channel_0_RegTb[] = {
/*
	Bank 7 - Channel 0 VGA RF registers
*/
	/* E3 CR */
	{ RF_BANK7,	RF_R00, 0x47 }, /* Allow BBP/MAC to do calibration */
	{ RF_BANK7,	RF_R01, 0x00 },
	{ RF_BANK7,	RF_R02, 0x00 },
	{ RF_BANK7,	RF_R03, 0x00 },
	{ RF_BANK7,	RF_R04, 0x00 },

	{ RF_BANK7,	RF_R10, 0x13 },
	{ RF_BANK7,	RF_R11, 0x0F },
	{ RF_BANK7,	RF_R12, 0x13 }, /* For DCOC */
	{ RF_BANK7,	RF_R13, 0x13 }, /* For DCOC */
	{ RF_BANK7,	RF_R14, 0x13 }, /* For DCOC */
	{ RF_BANK7,	RF_R15, 0x20 }, /* For DCOC */
	{ RF_BANK7,	RF_R16, 0x22 }, /* For DCOC */

	{ RF_BANK7,	RF_R17, 0x7C },

	{ RF_BANK7,	RF_R18, 0x00 },
	{ RF_BANK7,	RF_R19, 0x00 },
	{ RF_BANK7,	RF_R20, 0x00 },
	{ RF_BANK7,	RF_R21, 0xF1 },
	{ RF_BANK7,	RF_R22, 0x11 },
	{ RF_BANK7,	RF_R23, 0xC2 },
	{ RF_BANK7,	RF_R24, 0x41 },
	{ RF_BANK7,	RF_R25, 0x20 },
	{ RF_BANK7,	RF_R26, 0x40 },
	{ RF_BANK7,	RF_R27, 0xD7 },
	{ RF_BANK7,	RF_R28, 0xA2 },
	{ RF_BANK7,	RF_R29, 0x60 },
	{ RF_BANK7,	RF_R30, 0x49 },
	{ RF_BANK7,	RF_R31, 0x20 },
	{ RF_BANK7,	RF_R32, 0x44 },
	{ RF_BANK7,	RF_R33, 0xC1 },
	{ RF_BANK7,	RF_R34, 0x60 },
	{ RF_BANK7,	RF_R35, 0xC0 },

	{ RF_BANK7,	RF_R61, 0x01 },

	{ RF_BANK7,	RF_R72, 0x3C },
	{ RF_BANK7,	RF_R73, 0x34 },
	{ RF_BANK7,	RF_R74, 0x00 },
};
static u32 MT76x0_RF_VGA_Channel_0_RegTb_Size = (sizeof(MT76x0_RF_VGA_Channel_0_RegTb) / sizeof(BANK_RF_REG_PAIR));

static MT76x0_FREQ_ITEM MT76x0_Frequency_Plan[] = {
	{1,	RF_G_BAND,	0x02, 0x3F, 0x28, 0xDD, 0xE2, 0x40, 0x02, 0x40, 0x02, 0, 0, 1, 0x28, 0, 0x30, 0, 0, 0x3}, /* Freq 2412 */
	{2, 	RF_G_BAND,	0x02, 0x3F, 0x3C, 0xDD, 0xE4, 0x40, 0x07, 0x40, 0x02, 0, 0, 1, 0xA1, 0, 0x30, 0, 0, 0x1}, /* Freq 2417 */
	{3, 	RF_G_BAND,	0x02, 0x3F, 0x3C, 0xDD, 0xE2, 0x40, 0x07, 0x40, 0x0B, 0, 0, 1, 0x50, 0, 0x30, 0, 0, 0x0}, /* Freq 2422 */
	{4, 	RF_G_BAND,	0x02, 0x3F, 0x28, 0xDD, 0xD4, 0x40, 0x02, 0x40, 0x09, 0, 0, 1, 0x50, 0, 0x30, 0, 0, 0x0}, /* Freq 2427 */
	{5, 	RF_G_BAND,	0x02, 0x3F, 0x3C, 0xDD, 0xD4, 0x40, 0x07, 0x40, 0x02, 0, 0, 1, 0xA2, 0, 0x30, 0, 0, 0x1}, /* Freq 2432 */
	{6, 	RF_G_BAND,	0x02, 0x3F, 0x3C, 0xDD, 0xD4, 0x40, 0x07, 0x40, 0x07, 0, 0, 1, 0xA2, 0, 0x30, 0, 0, 0x1}, /* Freq 2437 */
	{7, 	RF_G_BAND,	0x02, 0x3F, 0x28, 0xDD, 0xE2, 0x40, 0x02, 0x40, 0x07, 0, 0, 1, 0x28, 0, 0x30, 0, 0, 0x3}, /* Freq 2442 */
	{8, 	RF_G_BAND,	0x02, 0x3F, 0x3C, 0xDD, 0xD4, 0x40, 0x07, 0x40, 0x02, 0, 0, 1, 0xA3, 0, 0x30, 0, 0, 0x1}, /* Freq 2447 */
	{9, 	RF_G_BAND,	0x02, 0x3F, 0x3C, 0xDD, 0xF2, 0x40, 0x07, 0x40, 0x0D, 0, 0, 1, 0x28, 0, 0x30, 0, 0, 0x3}, /* Freq 2452 */
	{10, 	RF_G_BAND,	0x02, 0x3F, 0x28, 0xDD, 0xD4, 0x40, 0x02, 0x40, 0x09, 0, 0, 1, 0x51, 0, 0x30, 0, 0, 0x0}, /* Freq 2457 */
	{11, 	RF_G_BAND,	0x02, 0x3F, 0x3C, 0xDD, 0xD4, 0x40, 0x07, 0x40, 0x02, 0, 0, 1, 0xA4, 0, 0x30, 0, 0, 0x1}, /* Freq 2462 */
	{12, 	RF_G_BAND,	0x02, 0x3F, 0x3C, 0xDD, 0xD4, 0x40, 0x07, 0x40, 0x07, 0, 0, 1, 0xA4, 0, 0x30, 0, 0, 0x1}, /* Freq 2467 */
	{13, 	RF_G_BAND,	0x02, 0x3F, 0x28, 0xDD, 0xF2, 0x40, 0x02, 0x40, 0x02, 0, 0, 1, 0x29, 0, 0x30, 0, 0, 0x3}, /* Freq 2472 */
	{14, 	RF_G_BAND,	0x02, 0x3F, 0x28, 0xDD, 0xF2, 0x40, 0x02, 0x40, 0x04, 0, 0, 1, 0x29, 0, 0x30, 0, 0, 0x3}, /* Freq 2484 */

	{183, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x70, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x17, 0, 0, 1, 0x28, 0, 0x30, 0, 0, 0x3}, /* Freq 4915 */
	{184, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x00, 0, 0, 1, 0x29, 0, 0x30, 0, 0, 0x3}, /* Freq 4920 */
	{185, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x01, 0, 0, 1, 0x29, 0, 0x30, 0, 0, 0x3}, /* Freq 4925 */
	{187, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x03, 0, 0, 1, 0x29, 0, 0x30, 0, 0, 0x3}, /* Freq 4935 */
	{188, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x02, 0, 0, 1, 0x29, 0, 0x30, 0, 0, 0x3}, /* Freq 4940 */
	{189, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x05, 0, 0, 1, 0x29, 0, 0x30, 0, 0, 0x3}, /* Freq 4945 */
	{192, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x04, 0, 0, 1, 0x29, 0, 0x30, 0, 0, 0x3}, /* Freq 4960 */
	{196, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x06, 0, 0, 1, 0x29, 0, 0x30, 0, 0, 0x3}, /* Freq 4980 */

	{36, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x02, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5180 */
	{37, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x05, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5185 */
	{38, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x03, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5190 */
	{39, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x07, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5195 */
	{40, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x04, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5200 */
	{41, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x09, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5205 */
	{42, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x05, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5210 */
	{43, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0B, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5215 */
	{44, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x06, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5220 */
	{45, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0D, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5225 */
	{46, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x07, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5230 */
	{47, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0F, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5235 */
	{48, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x08, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5240 */
	{49, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x11, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5245 */
	{50, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x09, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5250 */
	{51, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x13, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5255 */
	{52, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x0A, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5260 */
	{53, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x15, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5265 */
	{54, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x0B, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5270 */
	{55, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x70, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x17, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5275 */
	{56, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x00, 0, 0, 1, 0x2C, 0, 0x30, 0, 0, 0x3}, /* Freq 5280 */
	{57, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x01, 0, 0, 1, 0x2C, 0, 0x30, 0, 0, 0x3}, /* Freq 5285 */
	{58, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x01, 0, 0, 1, 0x2C, 0, 0x30, 0, 0, 0x3}, /* Freq 5290 */
	{59, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x03, 0, 0, 1, 0x2C, 0, 0x30, 0, 0, 0x3}, /* Freq 5295 */
	{60, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x02, 0, 0, 1, 0x2C, 0, 0x30, 0, 0, 0x3}, /* Freq 5300 */
	{61, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x05, 0, 0, 1, 0x2C, 0, 0x30, 0, 0, 0x3}, /* Freq 5305 */
	{62, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x03, 0, 0, 1, 0x2C, 0, 0x30, 0, 0, 0x3}, /* Freq 5310 */
	{63, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x07, 0, 0, 1, 0x2C, 0, 0x30, 0, 0, 0x3}, /* Freq 5315 */
	{64, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x04, 0, 0, 1, 0x2C, 0, 0x30, 0, 0, 0x3}, /* Freq 5320 */

	{100, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x0A, 0, 0, 1, 0x2D, 0, 0x30, 0, 0, 0x3}, /* Freq 5500 */
	{101, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x15, 0, 0, 1, 0x2D, 0, 0x30, 0, 0, 0x3}, /* Freq 5505 */
	{102, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x0B, 0, 0, 1, 0x2D, 0, 0x30, 0, 0, 0x3}, /* Freq 5510 */
	{103, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x70, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x17, 0, 0, 1, 0x2D, 0, 0x30, 0, 0, 0x3}, /* Freq 5515 */
	{104, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x00, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5520 */
	{105, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x01, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5525 */
	{106, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x01, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5530 */
	{107, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x03, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5535 */
	{108, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x02, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5540 */
	{109, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x05, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5545 */
	{110, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x03, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5550 */
	{111, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x07, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5555 */
	{112, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x04, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5560 */
	{113, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x09, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5565 */
	{114, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x05, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5570 */
	{115, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0B, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5575 */
	{116, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x06, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5580 */
	{117, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0D, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5585 */
	{118, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x07, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5590 */
	{119, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0F, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5595 */
	{120, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x08, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5600 */
	{121, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x11, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5605 */
	{122, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x09, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5610 */
	{123, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x13, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5615 */
	{124, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x0A, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5620 */
	{125, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x15, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5625 */
	{126, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x0B, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5630 */
	{127, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x70, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x17, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5635 */
	{128, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x00, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5640 */
	{129, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x01, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5645 */
	{130, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x01, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5650 */
	{131, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x03, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5655 */
	{132, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x02, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5660 */
	{133, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x05, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5665 */
	{134, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x03, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5670 */
	{135, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x07, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5675 */
	{136, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x04, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5680 */

	{137, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x09, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5685 */
	{138, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x05, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5690 */
	{139, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0B, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5695 */
	{140, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x06, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5700 */
	{141, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0D, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5705 */
	{142, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x07, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5710 */
	{143, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0F, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5715 */
	{144, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x08, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5720 */
	{145, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x11, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5725 */
	{146, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x09, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5730 */
	{147, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x13, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5735 */
	{148, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x0A, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5740 */
	{149, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x15, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5745 */
	{150, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x0B, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5750 */
	{151, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x70, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x17, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5755 */
	{152, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x00, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5760 */
	{153, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x01, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5765 */
	{154, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x01, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5770 */
	{155, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x03, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5775 */
	{156, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x02, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5780 */
	{157, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x05, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5785 */
	{158, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x03, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5790 */
	{159, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x07, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5795 */
	{160, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x04, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5800 */
	{161, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x09, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5805 */
	{162, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x05, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5810 */
	{163, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0B, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5815 */
	{164, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x06, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5820 */
	{165, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0D, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5825 */
	{166, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x07, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5830 */
	{167, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0F, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5835 */
	{168, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x08, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5840 */
	{169, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x11, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5845 */
	{170, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x09, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5850 */
	{171, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x13, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5855 */
	{172, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x0A, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5860 */
	{173, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x15, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5865 */
};
u8 NUM_OF_MT76x0_CHNL = (sizeof(MT76x0_Frequency_Plan) / sizeof(MT76x0_FREQ_ITEM));


static MT76x0_FREQ_ITEM MT76x0_SDM_Frequency_Plan[] = {
	{1,	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x28, 0, 0x0, 0x8, 0xCCCC,  0x3}, /* Freq 2412 */
	{2, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x28, 0, 0x0, 0x8, 0x12222, 0x3}, /* Freq 2417 */
	{3, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x28, 0, 0x0, 0x8, 0x17777, 0x3}, /* Freq 2422 */
	{4, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x28, 0, 0x0, 0x8, 0x1CCCC, 0x3}, /* Freq 2427 */
	{5, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x28, 0, 0x0, 0x8, 0x22222, 0x3}, /* Freq 2432 */
	{6, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x28, 0, 0x0, 0x8, 0x27777, 0x3}, /* Freq 2437 */
	{7, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x28, 0, 0x0, 0x8, 0x2CCCC, 0x3}, /* Freq 2442 */
	{8, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x28, 0, 0x0, 0x8, 0x32222, 0x3}, /* Freq 2447 */
	{9, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x28, 0, 0x0, 0x8, 0x37777, 0x3}, /* Freq 2452 */
	{10, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x28, 0, 0x0, 0x8, 0x3CCCC, 0x3}, /* Freq 2457 */
	{11, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x29, 0, 0x0, 0x8, 0x2222, 0x3}, /* Freq 2462 */
	{12, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x29, 0, 0x0, 0x8, 0x7777, 0x3}, /* Freq 2467 */
	{13, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x29, 0, 0x0, 0x8, 0xCCCC, 0x3}, /* Freq 2472 */
	{14, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x29, 0, 0x0, 0x8, 0x19999, 0x3}, /* Freq 2484 */

	{183, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x28, 0, 0x0, 0x8, 0x3D555, 0x3}, /* Freq 4915 */
	{184, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x29, 0, 0x0, 0x8, 0x0,     0x3}, /* Freq 4920 */
	{185, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x29, 0, 0x0, 0x8, 0x2AAA,  0x3}, /* Freq 4925 */
	{187, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x29, 0, 0x0, 0x8, 0x8000,  0x3}, /* Freq 4935 */
	{188, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x29, 0, 0x0, 0x8, 0xAAAA,  0x3}, /* Freq 4940 */
	{189, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x29, 0, 0x0, 0x8, 0xD555,  0x3}, /* Freq 4945 */
	{192, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x29, 0, 0x0, 0x8, 0x15555, 0x3}, /* Freq 4960 */
	{196, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x29, 0, 0x0, 0x8, 0x20000, 0x3}, /* Freq 4980 */

	{36, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0xAAAA,  0x3}, /* Freq 5180 */
	{37, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0xD555,  0x3}, /* Freq 5185 */
	{38, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x10000, 0x3}, /* Freq 5190 */
	{39, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x12AAA, 0x3}, /* Freq 5195 */
	{40, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x15555, 0x3}, /* Freq 5200 */
	{41, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x18000, 0x3}, /* Freq 5205 */
	{42, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x1AAAA, 0x3}, /* Freq 5210 */
	{43, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x1D555, 0x3}, /* Freq 5215 */
	{44, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x20000, 0x3}, /* Freq 5220 */
	{45, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x22AAA, 0x3}, /* Freq 5225 */
	{46, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x25555, 0x3}, /* Freq 5230 */
	{47, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x28000, 0x3}, /* Freq 5235 */
	{48, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x2AAAA, 0x3}, /* Freq 5240 */
	{49, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x2D555, 0x3}, /* Freq 5245 */
	{50, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x30000, 0x3}, /* Freq 5250 */
	{51, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x32AAA, 0x3}, /* Freq 5255 */
	{52, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x35555, 0x3}, /* Freq 5260 */
	{53, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x38000, 0x3}, /* Freq 5265 */
	{54, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x3AAAA, 0x3}, /* Freq 5270 */
	{55, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x3D555, 0x3}, /* Freq 5275 */
	{56, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2C, 0, 0x0, 0x8, 0x00000, 0x3}, /* Freq 5280 */
	{57, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2C, 0, 0x0, 0x8, 0x02AAA, 0x3}, /* Freq 5285 */
	{58, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2C, 0, 0x0, 0x8, 0x05555, 0x3}, /* Freq 5290 */
	{59, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2C, 0, 0x0, 0x8, 0x08000, 0x3}, /* Freq 5295 */
	{60, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2C, 0, 0x0, 0x8, 0x0AAAA, 0x3}, /* Freq 5300 */
	{61, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2C, 0, 0x0, 0x8, 0x0D555, 0x3}, /* Freq 5305 */
	{62, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2C, 0, 0x0, 0x8, 0x10000, 0x3}, /* Freq 5310 */
	{63, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2C, 0, 0x0, 0x8, 0x12AAA, 0x3}, /* Freq 5315 */
	{64, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2C, 0, 0x0, 0x8, 0x15555, 0x3}, /* Freq 5320 */

	{100, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2D, 0, 0x0, 0x8, 0x35555, 0x3}, /* Freq 5500 */
	{101, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2D, 0, 0x0, 0x8, 0x38000, 0x3}, /* Freq 5505 */
	{102, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2D, 0, 0x0, 0x8, 0x3AAAA, 0x3}, /* Freq 5510 */
	{103, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2D, 0, 0x0, 0x8, 0x3D555, 0x3}, /* Freq 5515 */
	{104, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x00000, 0x3}, /* Freq 5520 */
	{105, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x02AAA, 0x3}, /* Freq 5525 */
	{106, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x05555, 0x3}, /* Freq 5530 */
	{107, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x08000, 0x3}, /* Freq 5535 */
	{108, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x0AAAA, 0x3}, /* Freq 5540 */
	{109, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x0D555, 0x3}, /* Freq 5545 */
	{110, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x10000, 0x3}, /* Freq 5550 */
	{111, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x12AAA, 0x3}, /* Freq 5555 */
	{112, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x15555, 0x3}, /* Freq 5560 */
	{113, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x18000, 0x3}, /* Freq 5565 */
	{114, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x1AAAA, 0x3}, /* Freq 5570 */
	{115, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x1D555, 0x3}, /* Freq 5575 */
	{116, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x20000, 0x3}, /* Freq 5580 */
	{117, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x22AAA, 0x3}, /* Freq 5585 */
	{118, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x25555, 0x3}, /* Freq 5590 */
	{119, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x28000, 0x3}, /* Freq 5595 */
	{120, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x2AAAA, 0x3}, /* Freq 5600 */
	{121, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x2D555, 0x3}, /* Freq 5605 */
	{122, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x30000, 0x3}, /* Freq 5610 */
	{123, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x32AAA, 0x3}, /* Freq 5615 */
	{124, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x35555, 0x3}, /* Freq 5620 */
	{125, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x38000, 0x3}, /* Freq 5625 */
	{126, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x3AAAA, 0x3}, /* Freq 5630 */
	{127, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x3D555, 0x3}, /* Freq 5635 */
	{128, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x00000, 0x3}, /* Freq 5640 */
	{129, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x02AAA, 0x3}, /* Freq 5645 */
	{130, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x05555, 0x3}, /* Freq 5650 */
	{131, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x08000, 0x3}, /* Freq 5655 */
	{132, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x0AAAA, 0x3}, /* Freq 5660 */
	{133, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x0D555, 0x3}, /* Freq 5665 */
	{134, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x10000, 0x3}, /* Freq 5670 */
	{135, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x12AAA, 0x3}, /* Freq 5675 */
	{136, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x15555, 0x3}, /* Freq 5680 */

	{137, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x18000, 0x3}, /* Freq 5685 */
	{138, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x1AAAA, 0x3}, /* Freq 5690 */
	{139, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x1D555, 0x3}, /* Freq 5695 */
	{140, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x20000, 0x3}, /* Freq 5700 */
	{141, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x22AAA, 0x3}, /* Freq 5705 */
	{142, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x25555, 0x3}, /* Freq 5710 */
	{143, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x28000, 0x3}, /* Freq 5715 */
	{144, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x2AAAA, 0x3}, /* Freq 5720 */
	{145, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x2D555, 0x3}, /* Freq 5725 */
	{146, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x30000, 0x3}, /* Freq 5730 */
	{147, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x32AAA, 0x3}, /* Freq 5735 */
	{148, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x35555, 0x3}, /* Freq 5740 */
	{149, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x38000, 0x3}, /* Freq 5745 */
	{150, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x3AAAA, 0x3}, /* Freq 5750 */
	{151, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x3D555, 0x3}, /* Freq 5755 */
	{152, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x00000, 0x3}, /* Freq 5760 */
	{153, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x02AAA, 0x3}, /* Freq 5765 */
	{154, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x05555, 0x3}, /* Freq 5770 */
	{155, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x08000, 0x3}, /* Freq 5775 */
	{156, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x0AAAA, 0x3}, /* Freq 5780 */
	{157, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x0D555, 0x3}, /* Freq 5785 */
	{158, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x10000, 0x3}, /* Freq 5790 */
	{159, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x12AAA, 0x3}, /* Freq 5795 */
	{160, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x15555, 0x3}, /* Freq 5800 */
	{161, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x18000, 0x3}, /* Freq 5805 */
	{162, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x1AAAA, 0x3}, /* Freq 5810 */
	{163, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x1D555, 0x3}, /* Freq 5815 */
	{164, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x20000, 0x3}, /* Freq 5820 */
	{165, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x22AAA, 0x3}, /* Freq 5825 */
	{166, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x25555, 0x3}, /* Freq 5830 */
	{167, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x28000, 0x3}, /* Freq 5835 */
	{168, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x2AAAA, 0x3}, /* Freq 5840 */
	{169, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x2D555, 0x3}, /* Freq 5845 */
	{170, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x30000, 0x3}, /* Freq 5850 */
	{171, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x32AAA, 0x3}, /* Freq 5855 */
	{172, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x35555, 0x3}, /* Freq 5860 */
	{173, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x38000, 0x3}, /* Freq 5865 */
};
u8 NUM_OF_MT76x0_SDM_CHNL = (sizeof(MT76x0_SDM_Frequency_Plan) / sizeof(MT76x0_FREQ_ITEM));

static u8 MT76x0_SDM_Channel[] = {
	183, 185,  43,  45,  54,  55,  57,  58,
	102, 103, 105, 106, 115, 117, 126, 127,
	129, 130, 139, 141, 150, 151, 153, 154,
	163, 165 };

static u8 MT76x0_SDM_Channel_Size = (sizeof(MT76x0_SDM_Channel) / sizeof(u8));

static const MT76x0_RF_SWITCH_ITEM MT76x0_RF_BW_Switch[] = {
	/*   Bank, 		Register,	Bw/Band, 	Value */
	{ RF_BANK0,	RF_R17,	RF_G_BAND | BW_20,	0x00 },
	{ RF_BANK0,	RF_R17,	RF_G_BAND | BW_40,	0x00 },
	{ RF_BANK0,	RF_R17,	RF_A_BAND | BW_20,	0x00 },
	{ RF_BANK0,	RF_R17,	RF_A_BAND | BW_40,	0x00 },
	{ RF_BANK0,	RF_R17,	RF_A_BAND | BW_80,	0x00 },

	// TODO: need to check B7.R6 & B7.R7 setting for 2.4G again @20121112
	{ RF_BANK7,	RF_R06,	RF_G_BAND | BW_20,	0x40 },
	{ RF_BANK7,	RF_R06,	RF_G_BAND | BW_40,	0x1C },
	{ RF_BANK7,	RF_R06,	RF_A_BAND | BW_20,	0x40 },
	{ RF_BANK7,	RF_R06,	RF_A_BAND | BW_40,	0x20 },
	{ RF_BANK7,	RF_R06,	RF_A_BAND | BW_80,	0x10 },

	{ RF_BANK7,	RF_R07,	RF_G_BAND | BW_20,	0x40 },
	{ RF_BANK7,	RF_R07,	RF_G_BAND | BW_40,	0x20 },
	{ RF_BANK7,	RF_R07,	RF_A_BAND | BW_20,	0x40 },
	{ RF_BANK7,	RF_R07,	RF_A_BAND | BW_40,	0x20 },
	{ RF_BANK7,	RF_R07,	RF_A_BAND | BW_80,	0x10 },

	{ RF_BANK7,	RF_R08,	RF_G_BAND | BW_20,	0x03 },
	{ RF_BANK7,	RF_R08,	RF_G_BAND | BW_40,	0x01 },
	{ RF_BANK7,	RF_R08,	RF_A_BAND | BW_20,	0x03 },
	{ RF_BANK7,	RF_R08,	RF_A_BAND | BW_40,	0x01 },
	{ RF_BANK7,	RF_R08,	RF_A_BAND | BW_80,	0x00 },

	// TODO: need to check B7.R58 & B7.R59 setting for 2.4G again @20121112
	{ RF_BANK7,	RF_R58,	RF_G_BAND | BW_20,	0x40 },
	{ RF_BANK7,	RF_R58,	RF_G_BAND | BW_40,	0x40 },
	{ RF_BANK7,	RF_R58,	RF_A_BAND | BW_20,	0x40 },
	{ RF_BANK7,	RF_R58,	RF_A_BAND | BW_40,	0x40 },
	{ RF_BANK7,	RF_R58,	RF_A_BAND | BW_80,	0x10 },

	{ RF_BANK7,	RF_R59,	RF_G_BAND | BW_20,	0x40 },
	{ RF_BANK7,	RF_R59,	RF_G_BAND | BW_40,	0x40 },
	{ RF_BANK7,	RF_R59,	RF_A_BAND | BW_20,	0x40 },
	{ RF_BANK7,	RF_R59,	RF_A_BAND | BW_40,	0x40 },
	{ RF_BANK7,	RF_R59,	RF_A_BAND | BW_80,	0x10 },

	{ RF_BANK7,	RF_R60,	RF_G_BAND | BW_20,	0xAA },
	{ RF_BANK7,	RF_R60,	RF_G_BAND | BW_40,	0xAA },
	{ RF_BANK7,	RF_R60,	RF_A_BAND | BW_20,	0xAA },
	{ RF_BANK7,	RF_R60,	RF_A_BAND | BW_40,	0xAA },
	{ RF_BANK7,	RF_R60,	RF_A_BAND | BW_80,	0xAA },

	{ RF_BANK7,	RF_R76,	BW_20,			0x40},
	{ RF_BANK7,	RF_R76,	BW_40,			0x40},
	{ RF_BANK7,	RF_R76,	BW_80,			0x10},

	{ RF_BANK7,	RF_R77,	BW_20,			0x40},
	{ RF_BANK7,	RF_R77,	BW_40,			0x40},
	{ RF_BANK7,	RF_R77,	BW_80,			0x10},
};
u8 MT76x0_RF_BW_Switch_Size = (sizeof(MT76x0_RF_BW_Switch) / sizeof(MT76x0_RF_SWITCH_ITEM));

static const MT76x0_RF_SWITCH_ITEM MT76x0_RF_Band_Switch[] = {
	/*   Bank, 	Register,	Bw/Band, 	Value */
	{ RF_BANK0,	RF_R16,		RF_G_BAND,	0x20 },
	{ RF_BANK0,	RF_R16,		RF_A_BAND,	0x20 },

	{ RF_BANK0,	RF_R18,		RF_G_BAND,	0x00 },
	{ RF_BANK0,	RF_R18,		RF_A_BAND,	0x00 },

	{ RF_BANK0,	RF_R39,		RF_G_BAND,	0x36 },
	{ RF_BANK0,	RF_R39,		RF_A_BAND_LB,	0x34 },
	{ RF_BANK0,	RF_R39,		RF_A_BAND_MB,	0x33 },
	{ RF_BANK0,	RF_R39,		RF_A_BAND_HB,	0x31 },
	{ RF_BANK0,	RF_R39,		RF_A_BAND_11J,	0x36 },

	{ RF_BANK6,	RF_R12,		RF_A_BAND_LB,	0x44 },
	{ RF_BANK6,	RF_R12,		RF_A_BAND_MB,	0x44 },
	{ RF_BANK6,	RF_R12,		RF_A_BAND_HB,	0x55 },
	{ RF_BANK6,	RF_R12,		RF_A_BAND_11J,	0x44 },

	{ RF_BANK6,	RF_R17,		RF_A_BAND_LB,	0x02 },
	{ RF_BANK6,	RF_R17,		RF_A_BAND_MB,	0x00 },
	{ RF_BANK6,	RF_R17,		RF_A_BAND_HB,	0x00 },
	{ RF_BANK6,	RF_R17,		RF_A_BAND_11J,	0x05 },

	{ RF_BANK6,	RF_R24,		RF_A_BAND_LB,	0xA1 },
	{ RF_BANK6,	RF_R24,		RF_A_BAND_MB,	0x41 },
	{ RF_BANK6,	RF_R24,		RF_A_BAND_HB,	0x21 },
	{ RF_BANK6,	RF_R24,		RF_A_BAND_11J,	0xE1 },

	{ RF_BANK6,	RF_R39,		RF_A_BAND_LB,	0x36 },
	{ RF_BANK6,	RF_R39,		RF_A_BAND_MB,	0x34 },
	{ RF_BANK6,	RF_R39,		RF_A_BAND_HB,	0x32 },
	{ RF_BANK6,	RF_R39,		RF_A_BAND_11J,	0x37 },

	{ RF_BANK6,	RF_R42,		RF_A_BAND_LB,	0xFB },
	{ RF_BANK6,	RF_R42,		RF_A_BAND_MB,	0xF3 },
	{ RF_BANK6,	RF_R42,		RF_A_BAND_HB,	0xEB },
	{ RF_BANK6,	RF_R42,		RF_A_BAND_11J,	0xEB },

	/* Move R6-R45, R50~R59 to MT76x0_RF_INT_PA_5G_Channel_0_RegTb/MT76x0_RF_EXT_PA_5G_Channel_0_RegTb */

	{ RF_BANK6,	RF_R127,	RF_G_BAND,	0x84 },
	{ RF_BANK6,	RF_R127,	RF_A_BAND,	0x04 },

	{ RF_BANK7,	RF_R05,		RF_G_BAND,	0x40 },
	{ RF_BANK7,	RF_R05,		RF_A_BAND,	0x00 },

	{ RF_BANK7,	RF_R09,		RF_G_BAND,	0x00 },
	{ RF_BANK7,	RF_R09,		RF_A_BAND,	0x00 },

	{ RF_BANK7,	RF_R70,		RF_G_BAND,	0x00 },
	{ RF_BANK7,	RF_R70,		RF_A_BAND,	0x6D },

	{ RF_BANK7,	RF_R71,		RF_G_BAND,	0x00 },
	{ RF_BANK7,	RF_R71,		RF_A_BAND,	0xB0 },

	{ RF_BANK7,	RF_R78,		RF_G_BAND,	0x00 },
	{ RF_BANK7,	RF_R78,		RF_A_BAND,	0x55 },

	{ RF_BANK7,	RF_R79,		RF_G_BAND,	0x00 },
	{ RF_BANK7,	RF_R79,		RF_A_BAND,	0x55 },
};
u8 MT76x0_RF_Band_Switch_Size = (sizeof(MT76x0_RF_Band_Switch) / sizeof(MT76x0_RF_SWITCH_ITEM));

/*
	External PA
*/
static MT76x0_RF_SWITCH_ITEM MT76x0_RF_EXT_PA_RegTb[] = {
	{ RF_BANK6,	RF_R45,		RF_A_BAND_LB,	0x63 },
	{ RF_BANK6,	RF_R45,		RF_A_BAND_MB,	0x43 },
	{ RF_BANK6,	RF_R45,		RF_A_BAND_HB,	0x33 },
	{ RF_BANK6,	RF_R45,		RF_A_BAND_11J,	0x73 },

	{ RF_BANK6,	RF_R50,		RF_A_BAND_LB,	0x02 },
	{ RF_BANK6,	RF_R50,		RF_A_BAND_MB,	0x02 },
	{ RF_BANK6,	RF_R50,		RF_A_BAND_HB,	0x02 },
	{ RF_BANK6,	RF_R50,		RF_A_BAND_11J,	0x02 },

	{ RF_BANK6,	RF_R51,		RF_A_BAND_LB,	0x02 },
	{ RF_BANK6,	RF_R51,		RF_A_BAND_MB,	0x02 },
	{ RF_BANK6,	RF_R51,		RF_A_BAND_HB,	0x02 },
	{ RF_BANK6,	RF_R51,		RF_A_BAND_11J,	0x02 },

	{ RF_BANK6,	RF_R52,		RF_A_BAND_LB,	0x08 },
	{ RF_BANK6,	RF_R52,		RF_A_BAND_MB,	0x08 },
	{ RF_BANK6,	RF_R52,		RF_A_BAND_HB,	0x08 },
	{ RF_BANK6,	RF_R52,		RF_A_BAND_11J,	0x08 },

	{ RF_BANK6,	RF_R53,		RF_A_BAND_LB,	0x08 },
	{ RF_BANK6,	RF_R53,		RF_A_BAND_MB,	0x08 },
	{ RF_BANK6,	RF_R53,		RF_A_BAND_HB,	0x08 },
	{ RF_BANK6,	RF_R53,		RF_A_BAND_11J,	0x08 },

	{ RF_BANK6,	RF_R54,		RF_A_BAND_LB,	0x0A },
	{ RF_BANK6,	RF_R54,		RF_A_BAND_MB,	0x0A },
	{ RF_BANK6,	RF_R54,		RF_A_BAND_HB,	0x0A },
	{ RF_BANK6,	RF_R54,		RF_A_BAND_11J,	0x0A },

	{ RF_BANK6,	RF_R55,		RF_A_BAND_LB,	0x0A },
	{ RF_BANK6,	RF_R55,		RF_A_BAND_MB,	0x0A },
	{ RF_BANK6,	RF_R55,		RF_A_BAND_HB,	0x0A },
	{ RF_BANK6,	RF_R55,		RF_A_BAND_11J,	0x0A },

	{ RF_BANK6,	RF_R56,		RF_A_BAND_LB,	0x05 },
	{ RF_BANK6,	RF_R56,		RF_A_BAND_MB,	0x05 },
	{ RF_BANK6,	RF_R56,		RF_A_BAND_HB,	0x05 },
	{ RF_BANK6,	RF_R56,		RF_A_BAND_11J,	0x05 },

	{ RF_BANK6,	RF_R57,		RF_A_BAND_LB,	0x05 },
	{ RF_BANK6,	RF_R57,		RF_A_BAND_MB,	0x05 },
	{ RF_BANK6,	RF_R57,		RF_A_BAND_HB,	0x05 },
	{ RF_BANK6,	RF_R57,		RF_A_BAND_11J,	0x05 },

	{ RF_BANK6,	RF_R58,		RF_A_BAND_LB,	0x05 },
	{ RF_BANK6,	RF_R58,		RF_A_BAND_MB,	0x03 },
	{ RF_BANK6,	RF_R58,		RF_A_BAND_HB,	0x02 },
	{ RF_BANK6,	RF_R58,		RF_A_BAND_11J,	0x07 },

	{ RF_BANK6,	RF_R59,		RF_A_BAND_LB,	0x05 },
	{ RF_BANK6,	RF_R59,		RF_A_BAND_MB,	0x03 },
	{ RF_BANK6,	RF_R59,		RF_A_BAND_HB,	0x02 },
	{ RF_BANK6,	RF_R59,		RF_A_BAND_11J,	0x07 },
};

static u32 MT76x0_RF_EXT_PA_RegTb_Size = (sizeof(MT76x0_RF_EXT_PA_RegTb) / sizeof(MT76x0_RF_SWITCH_ITEM));

/*
	Internal PA
*/
static MT76x0_RF_SWITCH_ITEM MT76x0_RF_INT_PA_RegTb[] = {
};
static u32 MT76x0_RF_INT_PA_RegTb_Size = (sizeof(MT76x0_RF_INT_PA_RegTb) / sizeof(MT76x0_RF_SWITCH_ITEM));

//
// Initialize FCE
//
void InitFce(
	struct rtmp_adapter *pAd)
{
	L2_STUFFING_STRUC L2Stuffing;

	L2Stuffing.word = 0;

	DBGPRINT(RT_DEBUG_TRACE, ("%s: -->\n", __FUNCTION__));

	L2Stuffing.word = mt7610u_read32(pAd, FCE_L2_STUFF);
	L2Stuffing.field.FS_WR_MPDU_LEN_EN = 0;
	mt7610u_write32(pAd, FCE_L2_STUFF, L2Stuffing.word);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: <--\n", __FUNCTION__));
}


/*
	Select 2.4/5GHz band
*/
void SelectBandMT76x0(struct rtmp_adapter *pAd, u8 Channel)
{
	DBGPRINT(RT_DEBUG_INFO, ("%s: -->\n", __FUNCTION__));

	if (Channel <= 14) {
		/* Select 2.4GHz */
		RF_RANDOM_WRITE(pAd, MT76x0_RF_2G_Channel_0_RegTb, MT76x0_RF_2G_Channel_0_RegTb_Size);

		rlt_rf_write(pAd, RF_BANK5, RF_R00, 0x45);
		rlt_rf_write(pAd, RF_BANK6, RF_R00, 0x44);

		rtmp_mac_set_band(pAd, BAND_24G);

		mt7610u_write32(pAd, TX_ALC_VGA3, 0x00050007);
		mt7610u_write32(pAd, TX0_RF_GAIN_CORR, 0x003E0002);
	} else {
		/* Select 5GHz */
		RF_RANDOM_WRITE(pAd, MT76x0_RF_5G_Channel_0_RegTb, MT76x0_RF_5G_Channel_0_RegTb_Size);

		rlt_rf_write(pAd, RF_BANK5, RF_R00, 0x44);
		rlt_rf_write(pAd, RF_BANK6, RF_R00, 0x45);

		rtmp_mac_set_band(pAd, BAND_5G);

		mt7610u_write32(pAd, TX_ALC_VGA3, 0x00000005);
		mt7610u_write32(pAd, TX0_RF_GAIN_CORR, 0x01010102);
	}

	DBGPRINT(RT_DEBUG_INFO, ("%s: <--\n", __FUNCTION__));
}

/*
	Set RF channel frequency parameters:
	Rdiv: R24[1:0]
	N: R29[7:0], R30[0]
	Nominator: R31[4:0]
	Non-Sigma: !SDM R31[7:5]
	Den: (Denomina - 8) R32[4:0]
	Loop Filter Config: R33, R34
	Pll_idiv: frac comp R35[6:0]
*/
void SetRfChFreqParametersMT76x0(struct rtmp_adapter *pAd, u8 Channel)
{
	u32 i = 0, RfBand = 0, MacReg = 0;
	u8 RFValue = 0;
	bool bSDM = false;
	MT76x0_FREQ_ITEM *pMT76x0_freq_item = NULL;

	DBGPRINT(RT_DEBUG_INFO, ("%s: -->\n", __FUNCTION__));

	for (i = 0; i < MT76x0_SDM_Channel_Size; i++) {
		if (Channel == MT76x0_SDM_Channel[i]) {
			bSDM = true;
			break;
		}
	}

	for (i = 0; i < NUM_OF_MT76x0_CHNL; i++) {
		if (Channel == MT76x0_Frequency_Plan[i].Channel) {
			RfBand = MT76x0_Frequency_Plan[i].Band;

			if (bSDM)
				pMT76x0_freq_item = &(MT76x0_SDM_Frequency_Plan[i]);
			else
				pMT76x0_freq_item = &(MT76x0_Frequency_Plan[i]);

			rlt_rf_write(pAd, RF_BANK0, RF_R37, pMT76x0_freq_item->pllR37);
			rlt_rf_write(pAd, RF_BANK0, RF_R36, pMT76x0_freq_item->pllR36);
			rlt_rf_write(pAd, RF_BANK0, RF_R35, pMT76x0_freq_item->pllR35);
			rlt_rf_write(pAd, RF_BANK0, RF_R34, pMT76x0_freq_item->pllR34);
			rlt_rf_write(pAd, RF_BANK0, RF_R33, pMT76x0_freq_item->pllR33);

			/* R32<7:5> */
			rlt_rf_read(pAd, RF_BANK0, RF_R32, &RFValue);
			RFValue &= ~(0xE0);
			RFValue |= pMT76x0_freq_item->pllR32_b7b5;
			rlt_rf_write(pAd, RF_BANK0, RF_R32, RFValue);

			/* R32<4:0> pll_den: (Denomina - 8) */
			rlt_rf_read(pAd, RF_BANK0, RF_R32, &RFValue);
			RFValue &= ~(0x1F);
			RFValue |= pMT76x0_freq_item->pllR32_b4b0;
			rlt_rf_write(pAd, RF_BANK0, RF_R32, RFValue);

			/* R31<7:5> */
			rlt_rf_read(pAd, RF_BANK0, RF_R31, &RFValue);
			RFValue &= ~(0xE0);
			RFValue |= pMT76x0_freq_item->pllR31_b7b5;
			rlt_rf_write(pAd, RF_BANK0, RF_R31, RFValue);

			/* R31<4:0> pll_k(Nominator) */
			rlt_rf_read(pAd, RF_BANK0, RF_R31, &RFValue);
			RFValue &= ~(0x1F);
			RFValue |= pMT76x0_freq_item->pllR31_b4b0;
			rlt_rf_write(pAd, RF_BANK0, RF_R31, RFValue);

			/* R30<7> sdm_reset_n */
			rlt_rf_read(pAd, RF_BANK0, RF_R30, &RFValue);
			RFValue &= ~(0x80);
			if (bSDM) {
				rlt_rf_write(pAd, RF_BANK0, RF_R30, RFValue);
				RFValue |= (0x80);
				rlt_rf_write(pAd, RF_BANK0, RF_R30, RFValue);
			} else {
				RFValue |= pMT76x0_freq_item->pllR30_b7;
				rlt_rf_write(pAd, RF_BANK0, RF_R30, RFValue);
			}

			/* R30<6:2> sdmmash_prbs,sin */
			rlt_rf_read(pAd, RF_BANK0, RF_R30, &RFValue);
			RFValue &= ~(0x7C);
			RFValue |= pMT76x0_freq_item->pllR30_b6b2;
			rlt_rf_write(pAd, RF_BANK0, RF_R30, RFValue);

			/* R30<1> sdm_bp */
			rlt_rf_read(pAd, RF_BANK0, RF_R30, &RFValue);
			RFValue &= ~(0x02);
			RFValue |= (pMT76x0_freq_item->pllR30_b1 << 1);
			rlt_rf_write(pAd, RF_BANK0, RF_R30, RFValue);

			/* R30<0> R29<7:0> (hex) pll_n */
			RFValue = pMT76x0_freq_item->pll_n & 0x00FF;
			rlt_rf_write(pAd, RF_BANK0, RF_R29, RFValue);

			rlt_rf_read(pAd, RF_BANK0, RF_R30, &RFValue);
			RFValue &= ~(0x1);
			RFValue |= ((pMT76x0_freq_item->pll_n >> 8) & 0x0001);
			rlt_rf_write(pAd, RF_BANK0, RF_R30, RFValue);

			/* R28<7:6> isi_iso */
			rlt_rf_read(pAd, RF_BANK0, RF_R28, &RFValue);
			RFValue &= ~(0xC0);
			RFValue |= pMT76x0_freq_item->pllR28_b7b6;
			rlt_rf_write(pAd, RF_BANK0, RF_R28, RFValue);

			/* R28<5:4> pfd_dly */
			rlt_rf_read(pAd, RF_BANK0, RF_R28, &RFValue);
			RFValue &= ~(0x30);
			RFValue |= pMT76x0_freq_item->pllR28_b5b4;
			rlt_rf_write(pAd, RF_BANK0, RF_R28, RFValue);

			/* R28<3:2> clksel option */
			rlt_rf_read(pAd, RF_BANK0, RF_R28, &RFValue);
			RFValue &= ~(0x0C);
			RFValue |= pMT76x0_freq_item->pllR28_b3b2;
			rlt_rf_write(pAd, RF_BANK0, RF_R28, RFValue);

			/* R28<1:0> R27<7:0> R26<7:0> (hex) sdm_k */
			RFValue = pMT76x0_freq_item->Pll_sdm_k & 0x000000FF;
			rlt_rf_write(pAd, RF_BANK0, RF_R26, RFValue);

			RFValue = ((pMT76x0_freq_item->Pll_sdm_k >> 8) & 0x000000FF);
			rlt_rf_write(pAd, RF_BANK0, RF_R27, RFValue);

			rlt_rf_read(pAd, RF_BANK0, RF_R28, &RFValue);
			RFValue &= ~(0x3);
			RFValue |= ((pMT76x0_freq_item->Pll_sdm_k >> 16) & 0x0003);
			rlt_rf_write(pAd, RF_BANK0, RF_R28, RFValue);

			/* R24<1:0> xo_div */
			rlt_rf_read(pAd, RF_BANK0, RF_R24, &RFValue);
			RFValue &= ~(0x3);
			RFValue |= pMT76x0_freq_item->pllR24_b1b0;
			rlt_rf_write(pAd, RF_BANK0, RF_R24, RFValue);


			pAd->LatchRfRegs.Channel = Channel; /* Channel latch */

			DBGPRINT(RT_DEBUG_TRACE,
				("%s: SwitchChannel#%d(Band = 0x%02X, RF=%d, %dT), "
				"0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, %u, 0x%02X, %u, 0x%02X, 0x%02X, 0x%02X, 0x%04X, 0x%02X, 0x%02X, 0x%02X, 0x%08X, 0x%02X\n",
				__FUNCTION__,
				Channel,
				RfBand,
				pAd->RfIcType,
				pAd->Antenna.field.TxPath,
				pMT76x0_freq_item->pllR37,
				pMT76x0_freq_item->pllR36,
				pMT76x0_freq_item->pllR35,
				pMT76x0_freq_item->pllR34,
				pMT76x0_freq_item->pllR33,
				pMT76x0_freq_item->pllR32_b7b5,
				pMT76x0_freq_item->pllR32_b4b0,
				pMT76x0_freq_item->pllR31_b7b5,
				pMT76x0_freq_item->pllR31_b4b0,
				pMT76x0_freq_item->pllR30_b7,
				pMT76x0_freq_item->pllR30_b6b2,
				pMT76x0_freq_item->pllR30_b1,
				pMT76x0_freq_item->pll_n,
				pMT76x0_freq_item->pllR28_b7b6,
				pMT76x0_freq_item->pllR28_b5b4,
				pMT76x0_freq_item->pllR28_b3b2,
				pMT76x0_freq_item->Pll_sdm_k,
				pMT76x0_freq_item->pllR24_b1b0));
			break;
		}
	}


	for (i = 0; i < MT76x0_RF_BW_Switch_Size; i++) {
		if (pAd->CommonCfg.BBPCurrentBW == MT76x0_RF_BW_Switch[i].BwBand) {
			rlt_rf_write(pAd,
						MT76x0_RF_BW_Switch[i].Bank,
						MT76x0_RF_BW_Switch[i].Register,
						MT76x0_RF_BW_Switch[i].Value);
		} else if ((pAd->CommonCfg.BBPCurrentBW == (MT76x0_RF_BW_Switch[i].BwBand & 0xFF)) &&
				 (RfBand & MT76x0_RF_BW_Switch[i].BwBand)) {
			rlt_rf_write(pAd,
						MT76x0_RF_BW_Switch[i].Bank,
						MT76x0_RF_BW_Switch[i].Register,
						MT76x0_RF_BW_Switch[i].Value);
		}
	}

	for (i = 0; i < MT76x0_RF_Band_Switch_Size; i++) {
		if (MT76x0_RF_Band_Switch[i].BwBand & RfBand) {
			rlt_rf_write(pAd,
				     MT76x0_RF_Band_Switch[i].Bank,
				     MT76x0_RF_Band_Switch[i].Register,
				     MT76x0_RF_Band_Switch[i].Value);
		}
	}

	MacReg = mt7610u_read32(pAd, RF_MISC);
	MacReg &= ~(0xC); /* Clear 0x518[3:2] */
	mt7610u_write32(pAd, RF_MISC, MacReg);

	DBGPRINT(RT_DEBUG_INFO, ("\n\n*********** PAType = %d ***********\n\n", pAd->chipCap.PAType));
	if ((pAd->chipCap.PAType == INT_PA_2G_5G) ||
		((pAd->chipCap.PAType == EXT_PA_5G_ONLY) && (RfBand & RF_G_BAND)) ||
		((pAd->chipCap.PAType == EXT_PA_2G_ONLY) && (RfBand & RF_A_BAND))) {
		/* Internal PA */
		for (i = 0; i < MT76x0_RF_INT_PA_RegTb_Size; i++) {
			if (MT76x0_RF_INT_PA_RegTb[i].BwBand & RfBand) {
				rlt_rf_write(pAd,
							MT76x0_RF_INT_PA_RegTb[i].Bank,
							MT76x0_RF_INT_PA_RegTb[i].Register,
							MT76x0_RF_INT_PA_RegTb[i].Value);

				DBGPRINT(RT_DEBUG_INFO, ("%s: INT_PA_RegTb - B%d.R%02d = 0x%02x\n",
							__FUNCTION__,
							MT76x0_RF_INT_PA_RegTb[i].Bank,
							MT76x0_RF_INT_PA_RegTb[i].Register,
							MT76x0_RF_INT_PA_RegTb[i].Value));
			}
		}
	} else {
		/*
			RF_MISC (offset: 0x0518)
			[2]1'b1: enable external A band PA, 1'b0: disable external A band PA
			[3]1'b1: enable external G band PA, 1'b0: disable external G band PA
		*/
		if (RfBand & RF_A_BAND) {
			MacReg = mt7610u_read32(pAd, RF_MISC);
			MacReg |= (0x4);
			mt7610u_write32(pAd, RF_MISC, MacReg);
		} else {
			MacReg = mt7610u_read32(pAd, RF_MISC);
			MacReg |= (0x8);
			mt7610u_write32(pAd, RF_MISC, MacReg);
		}

		/* External PA */
		for (i = 0; i < MT76x0_RF_EXT_PA_RegTb_Size; i++) {
			if (MT76x0_RF_EXT_PA_RegTb[i].BwBand & RfBand) {
				rlt_rf_write(pAd,
					     MT76x0_RF_EXT_PA_RegTb[i].Bank,
					     MT76x0_RF_EXT_PA_RegTb[i].Register,
					     MT76x0_RF_EXT_PA_RegTb[i].Value);

				DBGPRINT(RT_DEBUG_INFO, ("%s: EXT_PA_RegTb - B%d.R%02d = 0x%02x\n",
							__FUNCTION__,
							MT76x0_RF_EXT_PA_RegTb[i].Bank,
							MT76x0_RF_EXT_PA_RegTb[i].Register,
							MT76x0_RF_EXT_PA_RegTb[i].Value));
			}
		}
	}

	if (RfBand & RF_G_BAND) {
		mt7610u_write32(pAd, TX0_RF_GAIN_ATTEN, 0x63707400);
		/* Set Atten mode = 2 for G band and disable Tx Inc DCOC Cal by Chee's comment */
		MacReg = mt7610u_read32(pAd, TX_ALC_CFG_1);
		MacReg &= 0x896400FF;
		mt7610u_write32(pAd, TX_ALC_CFG_1, MacReg);
	} else {
		mt7610u_write32(pAd, TX0_RF_GAIN_ATTEN, 0x686A7800);
		/* Set Atten mode = 0 For Ext A band and disable Tx Inc DCOC Cal by Chee's comment */
		MacReg = mt7610u_read32(pAd, TX_ALC_CFG_1);
		MacReg &= 0x890400FF;
		mt7610u_write32(pAd, TX_ALC_CFG_1, MacReg);
	}

	DBGPRINT(RT_DEBUG_INFO, ("%s: <--\n", __FUNCTION__));
}

static void NICInitMT76x0RFRegisters(struct rtmp_adapter *pAd)
{

	u32 IdReg;
	u8 RFValue;


	RF_RANDOM_WRITE(pAd, MT76x0_RF_Central_RegTb, MT76x0_RF_Central_RegTb_Size);

	RF_RANDOM_WRITE(pAd, MT76x0_RF_2G_Channel_0_RegTb, MT76x0_RF_2G_Channel_0_RegTb_Size);

	RF_RANDOM_WRITE(pAd, MT76x0_RF_5G_Channel_0_RegTb, MT76x0_RF_5G_Channel_0_RegTb_Size);

	RF_RANDOM_WRITE(pAd, MT76x0_RF_VGA_Channel_0_RegTb, MT76x0_RF_VGA_Channel_0_RegTb_Size);

	for (IdReg = 0; IdReg < MT76x0_RF_BW_Switch_Size; IdReg++) {
		if (pAd->CommonCfg.BBPCurrentBW == MT76x0_RF_BW_Switch[IdReg].BwBand) {
			rlt_rf_write(pAd,
				     MT76x0_RF_BW_Switch[IdReg].Bank,
				     MT76x0_RF_BW_Switch[IdReg].Register,
				     MT76x0_RF_BW_Switch[IdReg].Value);
		} else if ((BW_20 == (MT76x0_RF_BW_Switch[IdReg].BwBand & 0xFF)) &&
				 (RF_G_BAND & MT76x0_RF_BW_Switch[IdReg].BwBand)) {
			rlt_rf_write(pAd,
				     MT76x0_RF_BW_Switch[IdReg].Bank,
				     MT76x0_RF_BW_Switch[IdReg].Register,
				     MT76x0_RF_BW_Switch[IdReg].Value);
		}
	}

	for (IdReg = 0; IdReg < MT76x0_RF_Band_Switch_Size; IdReg++) {
		if (MT76x0_RF_Band_Switch[IdReg].BwBand & RF_G_BAND) {
			rlt_rf_write(pAd,
				     MT76x0_RF_Band_Switch[IdReg].Bank,
				     MT76x0_RF_Band_Switch[IdReg].Register,
				     MT76x0_RF_Band_Switch[IdReg].Value);
		}
	}

	/*
		Frequency calibration
		E1: B0.R22<6:0>: xo_cxo<6:0>
		E2: B0.R21<0>: xo_cxo<0>, B0.R22<7:0>: xo_cxo<8:1>
	*/
	RFValue = (u8)(pAd->RfFreqOffset & 0xFF);
	RFValue = min(RFValue, (u8)0xBF); /* Max of 9-bit built-in crystal oscillator C1 code */
	rlt_rf_write(pAd, RF_BANK0, RF_R22, RFValue);

	rlt_rf_read(pAd, RF_BANK0, RF_R22, &RFValue);
	DBGPRINT(RT_DEBUG_TRACE, ("%s: B0.R22 = 0x%02x\n", __FUNCTION__, RFValue));

	/*
		Reset the DAC (Set B0.R73<7>=1, then set B0.R73<7>=0, and then set B0.R73<7>) during power up.
	*/
	rlt_rf_read(pAd, RF_BANK0, RF_R73, &RFValue);
	RFValue |= 0x80;
	rlt_rf_write(pAd, RF_BANK0, RF_R73, RFValue);
	RFValue &= (~0x80);
	rlt_rf_write(pAd, RF_BANK0, RF_R73, RFValue);
	RFValue |= 0x80;
	rlt_rf_write(pAd, RF_BANK0, RF_R73, RFValue);

	/*
		vcocal_en (initiate VCO calibration (reset after completion)) - It should be at the end of RF configuration.
	*/
	rlt_rf_read(pAd, RF_BANK0, RF_R04, &RFValue);
	RFValue = ((RFValue & ~0x80) | 0x80);
	rlt_rf_write(pAd, RF_BANK0, RF_R04, RFValue);
	return;
}


/*
========================================================================
Routine Description:
	Initialize specific MAC registers.

Arguments:
	pAd					- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
static void NICInitMT76x0MacRegisters(struct rtmp_adapter *pAd)
{
	u32 MacReg = 0;
	u16 trsw_mode = 0;

	/*
		Enable PBF and MAC clock
		SYS_CTRL[11:10] = 0x3
	*/
	RANDOM_WRITE(pAd, MT76x0_MACRegTable, MT76x0_NUM_MAC_REG_PARMS);

	trsw_mode = mt7610u_read_eeprom16(pAd, 0x24);
	if (((trsw_mode & ~(0xFFCF)) >> 4) == 0x3) {
		mt7610u_write32(pAd, TX_SW_CFG1, 0x00040200); /* Adjust TR_SW off delay for TRSW mode */
		DBGPRINT(RT_DEBUG_TRACE, ("%s: TRSW = 0x%x\n", __FUNCTION__, ((trsw_mode & ~(0xFFCF)) >> 4)));
	}

	/*
		Release BBP and MAC reset
		MAC_SYS_CTRL[1:0] = 0x0
	*/
	MacReg = mt7610u_read32(pAd, MAC_SYS_CTRL);
	MacReg &= ~(0x3);
	mt7610u_write32(pAd, MAC_SYS_CTRL, MacReg);


	/*
		Set 0x141C[15:12]=0xF
	*/
	MacReg = mt7610u_read32(pAd, EXT_CCA_CFG);
	MacReg |= (0x0000F000);
	mt7610u_write32(pAd, EXT_CCA_CFG, MacReg);

	InitFce(pAd);

	/*
		TxRing 9 is for Mgmt frame.
		TxRing 8 is for In-band command frame.
		WMM_RG0_TXQMA: This register setting is for FCE to define the rule of TxRing 9.
		WMM_RG1_TXQMA: This register setting is for FCE to define the rule of TxRing 8.
	*/
	MacReg = mt7610u_read32(pAd, WMM_CTRL);
	MacReg &= ~(0x000003FF);
	MacReg |= (0x00000201);
	mt7610u_write32(pAd, WMM_CTRL, MacReg);

#ifdef MCS_LUT_SUPPORT
	mt7610u_read32(pAd, TX_FBK_LIMIT, &MacReg);
	if (RTMP_TEST_MORE_FLAG(pAd, fASIC_CAP_MCS_LUT))
		MacReg |= 0x40000;
	else
		MacReg &= (~0x40000);
	mt7610u_write32(pAd, TX_FBK_LIMIT, MacReg);
#endif /* MCS_LUT_SUPPORT */

	return;
}


/*
========================================================================
Routine Description:
	Initialize specific BBP registers.

Arguments:
	pAd					- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
static void NICInitMT76x0BbpRegisters(struct rtmp_adapter *pAd)
{

	INT IdReg;

	RANDOM_WRITE(pAd, MT76x0_BBP_Init_Tab, MT76x0_BBP_Init_Tab_Size);

	for (IdReg = 0; IdReg < MT76x0_BPP_SWITCH_Tab_Size; IdReg++) {
		if (((RF_G_BAND | RF_BW_20) & MT76x0_BPP_SWITCH_Tab[IdReg].BwBand) == (RF_G_BAND | RF_BW_20)) {
			RTMP_BBP_IO_WRITE32(pAd, MT76x0_BPP_SWITCH_Tab[IdReg].RegDate.Register,
					MT76x0_BPP_SWITCH_Tab[IdReg].RegDate.Value);
		}
	}

	RANDOM_WRITE(pAd, MT76x0_DCOC_Tab, MT76x0_DCOC_Tab_Size);


	return;
}

static void MT76x0_ChipSwitchChannel(
	struct rtmp_adapter *pAd,
	u8 Channel,
	bool bScan)
{
	CHAR TxPwer = 0; /* Bbp94 = BBPR94_DEFAULT, TxPwer2 = DEFAULT_RF_TX_POWER; */
	u8 RFValue = 0;
	u32 RegValue = 0;
	u32 Index;
	u32 rf_phy_mode, rf_bw = RF_BW_20;
	u8 bbp_ch_idx, delta_pwr;
	u32 ret;
	ULONG Old, New, Diff;
	u32 Value;

	RTMP_GetCurrentSystemTick(&Old);

	bbp_ch_idx = vht_prim_ch_idx(Channel, pAd->CommonCfg.Channel);

	DBGPRINT(RT_DEBUG_TRACE, ("%s(): MAC_STATUS_CFG = 0x%08x, bbp_ch_idx = %d, Channel=%d\n",
				__FUNCTION__, RegValue, bbp_ch_idx, Channel));

	if (Channel > 14)
		rf_phy_mode = RF_A_BAND;
	else
		rf_phy_mode = RF_G_BAND;

#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd)) {
		ret = down_interruptible(&pAd->hw_atomic);
		if (ret != 0) {
			DBGPRINT(RT_DEBUG_ERROR, ("reg_atomic get failed(ret=%d)\n", ret));
			return;
		}
	}
#endif /* RTMP_MAC_USB */

	RegValue = mt7610u_read32(pAd, EXT_CCA_CFG);
	RegValue &= ~(0xFFF);
	if (pAd->CommonCfg.BBPCurrentBW == BW_80) {
		rf_bw = RF_BW_80;
		if (bbp_ch_idx == 0)
			RegValue |= 0x1e4;
		else if (bbp_ch_idx == 1)
			RegValue |= 0x2e1;
		else if (bbp_ch_idx == 2)
			RegValue |= 0x41e;
		else if (bbp_ch_idx == 3)
			RegValue |= 0x81b;

	} else if (pAd->CommonCfg.BBPCurrentBW == BW_40) {
		rf_bw = RF_BW_40;

		if (pAd->CommonCfg.CentralChannel > pAd->CommonCfg.Channel)
			RegValue |= 0x1e4;
		else
			RegValue |= 0x2e1;
	} else {
		rf_bw = RF_BW_20;
		RegValue |= 0x1e4;

	}
	mt7610u_write32(pAd, EXT_CCA_CFG, RegValue);


	/*
		Configure 2.4/5GHz before accessing other MAC/BB/RF registers
	*/
	SelectBandMT76x0(pAd, Channel);

	/*
		Set RF channel frequency parameters (Rdiv, N, K, D and Ksd)
	*/
	SetRfChFreqParametersMT76x0(pAd, Channel);


	for (Index = 0; Index < MAX_NUM_OF_CHANNELS; Index++) {
		if (Channel == pAd->TxPower[Index].Channel) {
			TxPwer = pAd->TxPower[Index].Power;
			break;
		}
	}

	/* set Japan Tx filter at channel 14 */
	RegValue = RTMP_BBP_IO_READ32(pAd, CORE_R1);
	if (Channel == 14)
		RegValue |= 0x20;
	else
		RegValue &= (~0x20);
	RTMP_BBP_IO_WRITE32(pAd, CORE_R1, RegValue);

	for (Index = 0; Index < MT76x0_BPP_SWITCH_Tab_Size; Index++) {
		if (((rf_phy_mode | rf_bw) & MT76x0_BPP_SWITCH_Tab[Index].BwBand) == (rf_phy_mode | rf_bw)) {
			if ((MT76x0_BPP_SWITCH_Tab[Index].RegDate.Register == AGC1_R8)) {
				u32 eLNAgain = (MT76x0_BPP_SWITCH_Tab[Index].RegDate.Value & 0x0000FF00) >> 8;

				if (Channel > 14) {
					if (Channel < pAd->chipCap.a_band_mid_ch)
						eLNAgain -= (pAd->ALNAGain0*2);
					else if (Channel < pAd->chipCap.a_band_high_ch)
						eLNAgain -= (pAd->ALNAGain1*2);
					else
						eLNAgain -= (pAd->ALNAGain2*2);
				} else
					eLNAgain -= (pAd->BLNAGain*2);

				RTMP_BBP_IO_WRITE32(pAd, MT76x0_BPP_SWITCH_Tab[Index].RegDate.Register,
						(MT76x0_BPP_SWITCH_Tab[Index].RegDate.Value&(~0x0000FF00))|(eLNAgain << 8));
			} else {
				RTMP_BBP_IO_WRITE32(pAd, MT76x0_BPP_SWITCH_Tab[Index].RegDate.Register,
						MT76x0_BPP_SWITCH_Tab[Index].RegDate.Value);
			}
		}
	}

	/*
		VCO calibration (mode 3)
	*/
	MT76x0_VCO_CalibrationMode3(pAd, Channel);

	if (bScan)
		MT76x0_Calibration(pAd, Channel, false, false, false);

	RTMPusecDelay(1000);

	Value = mt7610u_read32(pAd, TX_ALC_CFG_0);
	Value = Value & (~0x3F3F);
	Value |= TxPwer;
	Value |= (0x2F2F << 16);
	mt7610u_write32(pAd, TX_ALC_CFG_0, Value);

#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd))
		up(&pAd->hw_atomic);

#endif /* RTMP_MAC_USB */

	if (Channel > 14)
		mt7610u_write32(pAd, XIFS_TIME_CFG, 0x33a41010);
	else
		mt7610u_write32(pAd, XIFS_TIME_CFG, 0x33a4100A);

	RTMP_GetCurrentSystemTick(&New);
	Diff = (New - Old) * 1000 / OS_HZ;
	DBGPRINT(RT_DEBUG_TRACE, ("Switch Channel spent %ldms\n", Diff));

	return;
}

/*
	NOTE: MAX_NUM_OF_CHANNELS shall  equal sizeof(txpwr_chlist))
*/
static u8 mt76x0_txpwr_chlist[] = {
	  1,   2,   3,   4,   5,   6,   7,
	  8,   9,  10,  11,  12,  13,  14,
	 36,  38,  40,  44,  46,  48,  52, 54,
	 56,  60,  62,  64,
	100, 102, 104, 108, 110, 112, 116, 118,
	120, 124, 126, 128, 132, 134, 136, 140,
	149, 151, 153, 157, 159, 161, 165, 167,
	169, 171, 173,
	 42,  58, 106, 122, 155,
};

INT MT76x0_ReadChannelPwr(struct rtmp_adapter *pAd)
{
	u32 i, choffset, idx, ss_offset_g, ss_num;
	EEPROM_TX_PWR_STRUC Power;
	CHAR tx_pwr1, tx_pwr2;

	DBGPRINT(RT_DEBUG_TRACE, ("%s()--->\n", __FUNCTION__));

	choffset = 0;
	ss_num = 1;

	for (i = 0; i < sizeof(mt76x0_txpwr_chlist); i++) {
		pAd->TxPower[i].Channel = mt76x0_txpwr_chlist[i];
		pAd->TxPower[i].Power = DEFAULT_RF_TX_POWER;
	}


	/* 0. 11b/g, ch1 - ch 14, 1SS */
	ss_offset_g = EEPROM_G_TX_PWR_OFFSET;
	for (i = 0; i < 7; i++) {
		idx = i * 2;
		Power.word = mt7610u_read_eeprom16(pAd, ss_offset_g + idx);

		tx_pwr1 = tx_pwr2 = DEFAULT_RF_TX_POWER;

		if ((Power.field.Byte0 <= 0x3F) && (Power.field.Byte0 >= 0))
			tx_pwr1 = Power.field.Byte0;

		if ((Power.field.Byte1 <= 0x3F) || (Power.field.Byte1 >= 0))
			tx_pwr2 = Power.field.Byte1;

		pAd->TxPower[idx].Power = tx_pwr1;
		pAd->TxPower[idx + 1].Power = tx_pwr2;
		choffset++;
	}

	/* 1. U-NII lower/middle band: 36, 38, 40; 44, 46, 48; 52, 54, 56; 60, 62, 64 (including central frequency in BW 40MHz)*/
	ASSERT((pAd->TxPower[choffset].Channel == 36));
	choffset = 14;
	ASSERT((pAd->TxPower[choffset].Channel == 36));
	for (i = 0; i < 6; i++) {
		idx = i * 2;
		Power.word = mt7610u_read_eeprom16(pAd, EEPROM_A_TX_PWR_OFFSET + idx);

		if ((Power.field.Byte0 <= 0x3F) && (Power.field.Byte0 >= 0))
			pAd->TxPower[idx + choffset + 0].Power = Power.field.Byte0;

		if ((Power.field.Byte1 <= 0x3F) && (Power.field.Byte1 >= 0))
			pAd->TxPower[idx + choffset + 1].Power = Power.field.Byte1;
	}


	/* 2. HipperLAN 2 100, 102 ,104; 108, 110, 112; 116, 118, 120; 124, 126, 128; 132, 134, 136; 140 (including central frequency in BW 40MHz)*/
	choffset = 14 + 12;
	ASSERT((pAd->TxPower[choffset].Channel == 100));
	for (i = 0; i < 8; i++) {

		idx = i * 2;
		Power.word = mt7610u_read_eeprom16(pAd, EEPROM_A_TX_PWR_OFFSET + (choffset - 14) + idx);

		if ((Power.field.Byte0 <= 0x3F) && (Power.field.Byte0 >= 0))
			pAd->TxPower[idx + choffset + 0].Power = Power.field.Byte0;

		if ((Power.field.Byte1 <= 0x3F) && (Power.field.Byte1 >= 0))
			pAd->TxPower[idx + choffset + 1].Power = Power.field.Byte1;
	}


	/* 3. U-NII upper band: 149, 151, 153; 157, 159, 161; 165, 167, 169; 171, 173 (including central frequency in BW 40MHz)*/
	choffset = 14 + 12 + 16;
	ASSERT((pAd->TxPower[choffset].Channel == 149));
	for (i = 0; i < 6; i++) {
		idx = i * 2;
		Power.word = mt7610u_read_eeprom16(pAd, EEPROM_A_TX_PWR_OFFSET + (choffset - 14) + idx);

		if ((Power.field.Byte0 <= 0x3F) && (Power.field.Byte0 >= 0))
			pAd->TxPower[idx + choffset + 0].Power = Power.field.Byte0;

		if ((Power.field.Byte1 <= 0x3F) && (Power.field.Byte1 >= 0))
			pAd->TxPower[idx + choffset + 1].Power = Power.field.Byte1;
	}

	/* choffset = 14 + 12 + 16 + 7; */
	choffset = 14 + 12 + 16 + 11;

#ifdef DOT11_VHT_AC
	ASSERT((pAd->TxPower[choffset].Channel == 42));

	/* For VHT80MHz, we need assign tx power for central channel 42, 58, 106, 122, and 155 */
	DBGPRINT(RT_DEBUG_TRACE, ("%s: Update Tx power control of the central channel (42, 58, 106, 122 and 155) for VHT BW80\n", __FUNCTION__));

	memmove(&pAd->TxPower[53], &pAd->TxPower[16], sizeof(CHANNEL_TX_POWER)); // channel 42 = channel 40
	memmove(&pAd->TxPower[54], &pAd->TxPower[22], sizeof(CHANNEL_TX_POWER)); // channel 58 = channel 56
	memmove(&pAd->TxPower[55], &pAd->TxPower[28], sizeof(CHANNEL_TX_POWER)); // channel 106 = channel 104
	memmove(&pAd->TxPower[56], &pAd->TxPower[34], sizeof(CHANNEL_TX_POWER)); // channel 122 = channel 120
	memmove(&pAd->TxPower[57], &pAd->TxPower[44], sizeof(CHANNEL_TX_POWER)); // channel 155 = channel 153

	pAd->TxPower[choffset].Channel = 42;
	pAd->TxPower[choffset+1].Channel = 58;
	pAd->TxPower[choffset+2].Channel = 106;
	pAd->TxPower[choffset+3].Channel = 122;
	pAd->TxPower[choffset+4].Channel = 155;

	choffset += 5;		/* the central channel of VHT80 */

	choffset = (MAX_NUM_OF_CHANNELS - 1);
#endif /* DOT11_VHT_AC */


	/* 4. Print and Debug*/
	for (i = 0; i < choffset; i++) {
		DBGPRINT(RT_DEBUG_TRACE, ("E2PROM: TxPower[%03d], Channel=%d, Power[Tx:%d]\n",
					i, pAd->TxPower[i].Channel, pAd->TxPower[i].Power));
	}

	return true;
}

void MT76x0_AsicExtraPowerOverMAC(struct rtmp_adapter *pAd)
{
	u32 ExtraPwrOverMAC = 0;
	u32 ExtraPwrOverTxPwrCfg7 = 0, ExtraPwrOverTxPwrCfg8 = 0, ExtraPwrOverTxPwrCfg9 = 0;

	/*
		For OFDM_54 and HT_MCS_7, extra fill the corresponding register value into MAC 0x13D4
		bit 21:16 -> HT/VHT MCS 7
		bit 5:0 -> OFDM 54
	*/
	ExtraPwrOverMAC = mt7610u_read32(pAd, TX_PWR_CFG_1);
	ExtraPwrOverTxPwrCfg7 |= (ExtraPwrOverMAC & 0x00003F00) >> 8; /* Get Tx power for OFDM 54 */
	ExtraPwrOverMAC = mt7610u_read32(pAd, TX_PWR_CFG_2);
	ExtraPwrOverTxPwrCfg7 |= (ExtraPwrOverMAC & 0x00003F00) << 8; /* Get Tx power for HT MCS 7 */
	mt7610u_write32(pAd, TX_PWR_CFG_7, ExtraPwrOverTxPwrCfg7);

	/*
		For HT_MCS_15, extra fill the corresponding register value into MAC 0x13D8
		bit 29:24 -> VHT 1SS MCS 9
		bit 21:16 -> VHT 1SS MCS 8
		bit 5:0 -> HT MCS 15
	*/
	ExtraPwrOverMAC = mt7610u_read32(pAd, TX_PWR_CFG_3);
#ifdef DOT11_VHT_AC
	ExtraPwrOverTxPwrCfg8 = pAd->Tx80MPwrCfgABand[0] | (ExtraPwrOverMAC & 0x0000FF00) >> 8; /* Get Tx power for HT MCS 15 */
#else
	ExtraPwrOverTxPwrCfg8 |= (ExtraPwrOverMAC & 0x0000FF00) >> 8; /* Get Tx power for HT MCS 15 */
#endif /* DOT11_VHT_AC */
	mt7610u_write32(pAd, TX_PWR_CFG_8, ExtraPwrOverTxPwrCfg8);

	/*
		For STBC_MCS_7, extra fill the corresponding register value into MAC 0x13DC
		bit 5:0 -> STBC MCS 7
	*/
	ExtraPwrOverMAC = mt7610u_read32(pAd, TX_PWR_CFG_4);
	ExtraPwrOverTxPwrCfg9 |= (ExtraPwrOverMAC & 0x00003F00) >> 8; /* Get Tx power for STBC MCS 7 */
	mt7610u_write32(pAd, TX_PWR_CFG_9, ExtraPwrOverTxPwrCfg9);

	DBGPRINT(RT_DEBUG_INFO, ("0x13D4 = 0x%08X, 0x13D8 = 0x%08X, 0x13D4 = 0x%08X\n",
			(UINT)ExtraPwrOverTxPwrCfg7, (UINT)ExtraPwrOverTxPwrCfg8, (UINT)ExtraPwrOverTxPwrCfg9));
}

static void calc_bw_delta_pwr(bool is_dec_delta, USHORT input_pwr,
	USHORT bw_delta, CHAR *tx_pwr1, CHAR *tx_pwr2)
{
	CHAR tp_pwr1 = 0, tp_pwr2 = 0;

	if (is_dec_delta == false) {
		if (input_pwr & 0x20) {
			tp_pwr1 = (input_pwr & 0x1F) + bw_delta;
			if (tp_pwr1 <= 0x1F)
				tp_pwr1 |= 0x20;
			else
				tp_pwr1 &= ~(0x20);
		} else {
			tp_pwr1 = (input_pwr & 0x1F) + bw_delta;
			if (tp_pwr1 > 0x1F)
				tp_pwr1 = 0x1F;
		}

		if (input_pwr & 0x2000) {
			tp_pwr2 = ((input_pwr & 0x1F00) >> 8) + bw_delta;
			if (tp_pwr2 <= 0x1F)
				tp_pwr2 |= 0x20;
			else
				tp_pwr2 &= ~(0x20);
		} else {
			tp_pwr2 = ((input_pwr & 0x1F00) >> 8) + bw_delta;
			if (tp_pwr2 > 0x1F)
				tp_pwr2 = 0x1F;
		}
	} else {
		if (input_pwr & 0x20) {
			tp_pwr1 = (input_pwr & 0x1F) - bw_delta;
			tp_pwr1 |= 0x20;
			if (tp_pwr1 > 0x3F)
				tp_pwr1 = 0x3F;
		} else {
			tp_pwr1 = (input_pwr & 0x1F) - bw_delta;
			if (tp_pwr1 < 0)
				tp_pwr1 &= 0x3F;
		}

		if (input_pwr & 0x2000) {
			tp_pwr2 = ((input_pwr & 0x1F00) >> 8) - bw_delta;
			tp_pwr2 |= 0x20;
			if (tp_pwr2 > 0x3F)
				tp_pwr2 = 0x3F;
		} else {
			tp_pwr2 = ((input_pwr & 0x1F00) >> 8) - bw_delta;
			if (tp_pwr2 < 0)
				tp_pwr2 &= 0x3F;
		}
	}

	*tx_pwr1 = tp_pwr1;
	*tx_pwr2 = tp_pwr2;
}

#define EEPROM_TXPOWER_BYRATE_STBC	(0xEC)
#define EEPROM_TXPOWER_BYRATE_5G	(0x120)
//
// VHT BW80 delta power control (+4~-4dBm) for per-rate Tx power control
//
#define EEPROM_VHT_BW80_TX_POWER_DELTA	(0xD3)


/*
========================================================================
Routine Description:
	Initialize MT76x0

Arguments:
	pAd					- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
void MT76x0_Init(struct rtmp_adapter *pAd)
{
	struct rtmp_chip_ops  *pChipOps = &pAd->chipOps;
	struct rtmp_chip_cap *pChipCap = &pAd->chipCap;
	u32 Value;

	DBGPRINT(RT_DEBUG_TRACE, ("-->%s():\n", __FUNCTION__));

	/*
		Init chip capabilities
	*/
	Value = mt7610u_read32(pAd, 0x00);
	pChipCap->ChipID = Value;

	pChipCap->MaxNss = 1;

	pChipCap->SnrFormula = SNR_FORMULA2;
	pChipCap->VcoPeriod = 10;
	pChipCap->FlgIsVcoReCalMode = VCO_CAL_MODE_3;

	pChipCap->asic_caps |= (fASIC_CAP_PMF_ENC);

	pChipCap->phy_caps = (fPHY_CAP_24G | fPHY_CAP_5G);
	pChipCap->phy_caps |= (fPHY_CAP_HT | fPHY_CAP_VHT);

	pChipCap->RfReg17WtMethod = RF_REG_WT_METHOD_STEP_ON;

	pChipCap->MaxNumOfRfId = MAX_RF_ID;

	pChipCap->MaxNumOfBbpId = 200;

#ifdef DFS_SUPPORT
	pChipCap->DfsEngineNum = 4;
#endif /* DFS_SUPPORT */

	pChipCap->MBSSIDMode = MBSSID_MODE1;

#ifdef CONFIG_STA_SUPPORT
	pChipCap->init_vga_gain_5G = 0x54;
	pChipCap->init_vga_gain_2G = 0x4E;
#endif /* CONFIG_STA_SUPPORT */

	pChipCap->EFUSE_USAGE_MAP_START = 0x1e0;
	pChipCap->EFUSE_USAGE_MAP_END = 0x1FC;
	pChipCap->EFUSE_USAGE_MAP_SIZE = 29;

#ifdef CONFIG_ANDES_SUPPORT
	pChipCap->WlanMemmapOffset = 0x410000;
	pChipCap->InbandPacketMaxLen = 192;
	pChipCap->CmdRspRxRing = RX_RING1;
	if (IS_MT7610(pAd))
		pChipCap->IsComboChip = false;
	else
		pChipCap->IsComboChip = true;

	pChipCap->load_iv = true;
	pChipCap->ilm_offset = 0x00000;
	pChipCap->dlm_offset = 0x80000;
#endif /* CONFIG_ANDES_SUPPORT */

	pChipCap->cmd_header_len = sizeof(struct txinfo_nmac_cmd);


#ifdef RTMP_USB_SUPPORT
	pChipCap->cmd_padding_len = 4;
#endif /* RTMP_USB_SUPPORT */

	if (IS_MT7650(pAd))
		pChipCap->fw_name = MT7650_FIRMWARE_NAME;

	if (IS_MT7630(pAd))
		pChipCap->fw_name = MT7650_FIRMWARE_NAME;

	if (IS_MT7610(pAd))
		pChipCap->fw_name = MT7610_FIRMWARE_NAME;

	pChipCap->bDoTemperatureSensor = true;

	pAd->rateAlg = RATE_ALG_GRP;

	/*
		Following function configure beacon related parameters
		in pChipCap
			FlgIsSupSpecBcnBuf / BcnMaxHwNum /
			WcidHwRsvNum / BcnMaxHwSize / BcnBase[]
	*/
	rlt_bcn_buf_init(pAd);

	/*
		init operator
	*/

	/* Channel */
	pChipOps->ChipSwitchChannel = MT76x0_ChipSwitchChannel;

	pChipOps->AsicMacInit = NICInitMT76x0MacRegisters;
	pChipOps->AsicBbpInit = NICInitMT76x0BbpRegisters;
	pChipOps->AsicRfInit = NICInitMT76x0RFRegisters;

	/* MAC */

	/* Chip tuning */
	pChipOps->AsicExtraPowerOverMAC = MT76x0_AsicExtraPowerOverMAC;

/*
	Following callback functions already initiailized in RtmpChipOpsHook()
	1. Power save related
*/

	pChipOps->DisableTxRx = MT76x0DisableTxRx;
}


void MT76x0_AntennaSelCtrl(struct rtmp_adapter *pAd)
{
	USHORT e2p_val = 0;
	u32 WlanFunCtrl = 0, CmbCtrl = 0, CoexCfg0 = 0, CoexCfg3 = 0;
	u32 ret;


#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd)) {
		ret = down_interruptible(&pAd->wlan_en_atomic);
		if (ret != 0) {
			DBGPRINT(RT_DEBUG_ERROR, ("wlan_en_atomic get failed(ret=%d)\n", ret));
			return;
		}
	}
#endif /* RTMP_MAC_USB */

	WlanFunCtrl = mt7610u_read32(pAd, WLAN_FUN_CTRL);
	CmbCtrl = mt7610u_read32(pAd, CMB_CTRL);
	CoexCfg0 = mt7610u_read32(pAd, COEXCFG0);
	CoexCfg3 = mt7610u_read32(pAd, COEXCFG3);

	CoexCfg0 &= ~BIT(2);
	CmbCtrl &= ~(BIT(14) | BIT(12));
	WlanFunCtrl &= ~(BIT(6) | BIT(5));
	CoexCfg3 &= ~(BIT(5) | BIT(4) | BIT(3) | BIT(2) | BIT(1));

	/*
		0x23[7]
		0x1: Chip is in dual antenna mode
		0x0: Chip is in single antenna mode
	*/
	e2p_val = mt7610u_read_eeprom16(pAd, 0x22);

	if (e2p_val & 0x8000) {
		if (pAd->NicConfig2.field.AntOpt == 0 &&
		    pAd->NicConfig2.field.AntDiversity == 1) {
			CmbCtrl |= BIT(12); /* 0x20[12]=1 */
		} else {
			CoexCfg3 |= BIT(4); /* 0x4C[4]=1 */
		}

		CoexCfg3 |= BIT(3); /* 0x4C[3]=1 */

		if (WMODE_CAP_2G(pAd->CommonCfg.PhyMode))
			WlanFunCtrl |= BIT(6); /* 0x80[6]=1 */

		DBGPRINT(RT_DEBUG_TRACE, ("%s - Dual antenna mode\n", __FUNCTION__));
	} else {
		if (WMODE_CAP_5G(pAd->CommonCfg.PhyMode)) {
			CoexCfg3 |= (BIT(3) | BIT(4)); /* 0x4C[3]=1, 0x4C[4]=1 */
		} else {
			WlanFunCtrl |= BIT(6); /* 0x80[6]=1 */
			CoexCfg3 |= BIT(1); /* 0x4C[1]=1 */
		}
		DBGPRINT(RT_DEBUG_TRACE, ("%s - Single antenna mode\n", __FUNCTION__));
	}

	mt7610u_write32(pAd, WLAN_FUN_CTRL, WlanFunCtrl);
	mt7610u_write32(pAd, CMB_CTRL, CmbCtrl);
	mt7610u_write32(pAd, COEXCFG0, CoexCfg0);
	mt7610u_write32(pAd, COEXCFG3, CoexCfg3);

#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd))
		up(&pAd->wlan_en_atomic);

#endif /* RTMP_MAC_USB */

}

void MT76x0_dynamic_vga_tuning(struct rtmp_adapter *pAd)
{
	struct rtmp_chip_cap *pChipCap = &pAd->chipCap;
	u32 reg_val = 0, init_vga = 0, rssi = 0;

	rssi = pAd->StaCfg.RssiSample.AvgRssi0 - pAd->BbpRssiToDbmDelta;

	if (pAd->CommonCfg.CentralChannel > 14)
		init_vga = pChipCap->init_vga_gain_5G;
	else
		init_vga = pChipCap->init_vga_gain_2G;

	if (rssi > -60)
		init_vga -= 0x20;
	else if ((rssi <= -60) && (rssi > -70))
		init_vga -= 0x10;

	reg_val = mt7610u_read32(pAd, AGC1_R8);
	reg_val &= 0xFFFF80FF;
	reg_val |= (init_vga << 8);
	mt7610u_write32(pAd, AGC1_R8, reg_val);

	DBGPRINT(RT_DEBUG_TRACE, ("%s(): RSSI=%d, BBP 2320=0x%x\n", __FUNCTION__, rssi, reg_val));
}

void MT76x0_VCO_CalibrationMode3(struct rtmp_adapter *pAd, u8 Channel)
{
	/*
		VCO_Calibration_MT7650E2.docx:
		2.	Calibration Procedure:
			i.	Set the configuration (examples in section 12)
			ii.	Set B0.R04.[7] vcocal_en to "high" (1.2V).  After completing the calibration procedure, it would return to "low" automatically.
	*/

	u8 RFValue = 0, Mode = 0;

	rlt_rf_read(pAd, RF_BANK0, RF_R04, &RFValue);
	Mode = (RFValue & 0x70);
	if (Mode == 0x30) {
		DBGPRINT(RT_DEBUG_TRACE, ("%s - Calibration Mode: Open loop, closed loop, and amplitude @ ch%d\n",
						__FUNCTION__, Channel));
		/*
			Calibration Mode - Open loop, closed loop, and amplitude:
			B0.R06.[0]: 1
			B0.R06.[3:1] bp_close_code: 100
			B0.R05.[7:0] bp_open_code: 0x0
			B0.R04.[2:0] cal_bits: 000
			B0.R03.[2:0] startup_time: 011
			B0.R03.[6:4] settle_time:
						80MHz channel: 110
						40MHz channel: 101
						20MHz channel: 100
		*/
		rlt_rf_read(pAd, RF_BANK0, RF_R06, &RFValue);
		RFValue &= ~(0x0F);
		RFValue |= 0x09;
		rlt_rf_write(pAd, RF_BANK0, RF_R06, RFValue);

		rlt_rf_read(pAd, RF_BANK0, RF_R05, &RFValue);
		if (RFValue != 0)
			rlt_rf_write(pAd, RF_BANK0, RF_R05, 0x0);

		rlt_rf_read(pAd, RF_BANK0, RF_R04, &RFValue);
		RFValue &= ~(0x07);
		rlt_rf_write(pAd, RF_BANK0, RF_R04, RFValue);

		rlt_rf_read(pAd, RF_BANK0, RF_R03, &RFValue);
		RFValue &= ~(0x77);
		if ((Channel == 1) || (Channel == 7) || (Channel == 9) || (Channel >= 13))
			RFValue |= 0x63;
		else if ((Channel == 3) || (Channel == 4) || (Channel == 10))
			RFValue |= 0x53;
		else if ((Channel == 2) || (Channel == 5) || (Channel == 6) || (Channel == 8) || (Channel == 11) || (Channel == 12))
			RFValue |= 0x43;
		else {
			DBGPRINT(RT_DEBUG_OFF, ("%s - wrong input channel\n", __FUNCTION__));
			return;
		}
		rlt_rf_write(pAd, RF_BANK0, RF_R03, RFValue);

		rlt_rf_read(pAd, RF_BANK0, RF_R04, &RFValue);
		RFValue = ((RFValue & ~(0x80)) | 0x80);
		rlt_rf_write(pAd, RF_BANK0, RF_R04, RFValue);

		RTMPusecDelay(2200);
	}

	return;
}

void MT76x0_Calibration(struct rtmp_adapter *pAd, u8 Channel, bool bPowerOn,
	bool bDoTSSI, bool bFullCal)
{
	u32 MacReg = 0, reg_val = 0, reg_tx_alc = 0;
#ifdef RTMP_MAC_USB
	u32 ret;
#endif /* RTMP_MAC_USB */

	DBGPRINT(RT_DEBUG_TRACE, ("%s - Channel = %d, bPowerOn = %d, bFullCal = %d\n", __FUNCTION__, Channel, bPowerOn, bFullCal));


#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd)) {
		ret = down_interruptible(&pAd->cal_atomic);
		if (ret != 0) {
			DBGPRINT(RT_DEBUG_ERROR, ("cal_atomic get failed(ret=%d)\n", ret));
			return;
		}
	}
#endif /* RTMP_MAC_USB */

	if (!(bPowerOn || bDoTSSI || bFullCal))
		goto RXDC_Calibration;

	if (bPowerOn) {
		/*
			Do Power on calibration.
			The calibration sequence is very important, please do NOT change it.
			1 XTAL Setup (already done in AsicRfInit)
			2 R-calibration
			3 VCO calibration
		*/

		/*
			2 R-calibration
		*/
		RTMP_CHIP_CALIBRATION(pAd, R_CALIBRATION, 0x0);

		/*
			3 VCO calibration (mode 3)
		*/
		MT76x0_VCO_CalibrationMode3(pAd, Channel);
	}

	reg_tx_alc = mt7610u_read32(pAd, TX_ALC_CFG_0); /* We need to restore 0x13b0 after calibration. */
	mt7610u_write32(pAd, TX_ALC_CFG_0, 0x0);
	RTMPusecDelay(500);

	reg_val = mt7610u_read32(pAd, 0x2124); /* We need to restore 0x2124 after calibration. */
	MacReg = 0xFFFFFF7E; /* Disable 0x2704, 0x2708 controlled by MAC. */
	mt7610u_write32(pAd, 0x2124, MacReg);

	/*
		Do calibration.
		The calibration sequence is very important, please do NOT change it.
		4  RX DCOC calibration
		5  LC tank calibration
		6  TX Filter BW --> not ready yet @20121003
		7  RX Filter BW --> not ready yet @20121003
		8  TX RF LOFT
		9  TX I/Q
		10 TX Group Delay
		11 RX I/Q
		12 RX Group Delay
		13 TSSI Zero Reference --> not ready yet @20121016
		14 TX 2G DPD
		15 TX 2G IM3 --> not ready yet @20121016
		16 On-chip temp sensor reading --> not ready yet @20130129
		17 RX DCOC calibration
	*/
	if (bFullCal) {
		int band_nr = (Channel > 14) ? 1 : 0;
		/*
			4. RXDC Calibration parameter
				0:Back Ground Disable
		*/
		RTMP_CHIP_CALIBRATION(pAd, RXDCOC_CALIBRATION, 0);

		/*
			5. LC-Calibration parameter
				Bit[0:7]
					0: 2G
					1: 5G + External PA
					2: 5G + Internal PA
				Bit[8:15]
					0: Full Calibration
					1: Partial Calibration
					2: G-Band Full Calibration + Save
					3: A-Band (Low) Full Calibration + Save
					4: A-Band (Mid) Full Calibration + Save
					5: A-Band (High) Full Calibration + Save
					6: G-Band Restore Calibration
					7: A-Band (Low) Restore Calibration
					8: A-Band (Mid) Restore Calibration
					9: A-Band (High) Restore Calibration
		*/

		// TODO: check PA setting from EEPROM @20121016
		RTMP_CHIP_CALIBRATION(pAd, LC_CALIBRATION, band_nr);

		/*
			6,7. BW-Calibration
				Bit[0:7] (0:RX, 1:TX)
				Bit[8:15] (0:BW20, 1:BW40, 2:BW80)
				Bit[16:23]
					0: Full Calibration
					1: Partial Calibration
					2: G-Band Full Calibration + Save
					3: A-Band (Low) Full Calibration + Save
					4: A-Band (Mid) Full Calibration + Save
					5: A-Band (High) Full Calibration + Save
					6: G-Band Restore Calibration
					7: A-Band (Low) Restore Calibration
					8: A-Band (Mid) Restore Calibration
					9: A-Band (High) Restore Calibration
		*/

		/*
			8. RF LOFT-Calibration parameter
				Bit[0:7] (0:G-Band, 1: A-Band)
				Bit[8:15]
					0: Full Calibration
					1: Partial Calibration
					2: G-Band Full Calibration + Save
					3: A-Band (Low) Full Calibration + Save
					4: A-Band (Mid) Full Calibration + Save
					5: A-Band (High) Full Calibration + Save
					6: G-Band Restore Calibration
					7: A-Band (Low) Restore Calibration
					8: A-Band (Mid) Restore Calibration
					9: A-Band (High) Restore Calibration
		*/

		RTMP_CHIP_CALIBRATION(pAd, LOFT_CALIBRATION, band_nr);

		/*
			9. TXIQ-Calibration parameter
				Bit[0:7] (0:G-Band, 1: A-Band)
				Bit[8:15]
					0: Full Calibration
					1: Partial Calibration
					2: G-Band Full Calibration + Save
					3: A-Band (Low) Full Calibration + Save
					4: A-Band (Mid) Full Calibration + Save
					5: A-Band (High) Full Calibration + Save
					6: G-Band Restore Calibration
					7: A-Band (Low) Restore Calibration
					8: A-Band (Mid) Restore Calibration
					9: A-Band (High) Restore Calibration
		*/

		RTMP_CHIP_CALIBRATION(pAd, TXIQ_CALIBRATION, band_nr);

		/*
			10. TX Group-Delay Calibation parameter
				Bit[0:7] (0:G-Band, 1: A-Band)
				Bit[8:15]
					0: Full Calibration
					1: Partial Calibration
					2: G-Band Full Calibration + Save
					3: A-Band (Low) Full Calibration + Save
					4: A-Band (Mid) Full Calibration + Save
					5: A-Band (High) Full Calibration + Save
					6: G-Band Restore Calibration
					7: A-Band (Low) Restore Calibration
					8: A-Band (Mid) Restore Calibration
					9: A-Band (High) Restore Calibration
		*/

		RTMP_CHIP_CALIBRATION(pAd, TX_GROUP_DELAY_CALIBRATION, band_nr);

		/*
			11. RXIQ-Calibration parameter
				Bit[0:7] (0:G-Band, 1: A-Band)
				Bit[8:15]
					0: Full Calibration
					1: Partial Calibration
					2: G-Band Full Calibration + Save
					3: A-Band (Low) Full Calibration + Save
					4: A-Band (Mid) Full Calibration + Save
					5: A-Band (High) Full Calibration + Save
					6: G-Band Restore Calibration
					7: A-Band (Low) Restore Calibration
					8: A-Band (Mid) Restore Calibration
					9: A-Band (High) Restore Calibration
		*/

		RTMP_CHIP_CALIBRATION(pAd, RXIQ_CALIBRATION, band_nr);

		/*
			12. RX Group-Delay Calibation parameter
				Bit[0:7] (0:G-Band, 1: A-Band)
				Bit[8:15]
					0: Full Calibration
					1: Partial Calibration
					2: G-Band Full Calibration + Save
					3: A-Band (Low) Full Calibration + Save
					4: A-Band (Mid) Full Calibration + Save
					5: A-Band (High) Full Calibration + Save
					6: G-Band Restore Calibration
					7: A-Band (Low) Restore Calibration
					8: A-Band (Mid) Restore Calibration
					9: A-Band (High) Restore Calibration
		*/

		RTMP_CHIP_CALIBRATION(pAd, RX_GROUP_DELAY_CALIBRATION, band_nr);

		/*
			14. TX 2G DPD - Only 2.4G needs to do DPD Calibration.
				Bit[0:7] (1~14 Channel)
				Bit[8:15] (0:BW20, 1:BW40)
				NOTE: disable DPD calibration for USB products
		*/
	}

	/* Restore 0x2124 & TX_ALC_CFG_0 after calibration completed */
	mt7610u_write32(pAd, 0x2124, reg_val);
	mt7610u_write32(pAd, TX_ALC_CFG_0, reg_tx_alc);
	RTMPusecDelay(100000); // TODO: check response packet from FW

RXDC_Calibration:
	/*
		17. RXDC Calibration parameter
			1:Back Ground Enable
	*/
	RTMP_CHIP_CALIBRATION(pAd, RXDCOC_CALIBRATION, 1);


#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd))
		up(&pAd->cal_atomic);

#endif /* RTMP_MAC_USB */
}

void MT76x0_TempSensor(struct rtmp_adapter *pAd)
{
	u8 rf_b7_73 = 0, rf_b0_66 = 0, rf_b0_67 = 0;
	u32 reg_val = 0;
	SHORT temperature = 0;
	INT32 Dout = 0;
	u32 MTxCycle = 0;
#ifdef RTMP_MAC_USB
	u32 ret;
#endif /* RTMP_MAC_USB */


#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd)) {
		ret = down_interruptible(&pAd->cal_atomic);
		if (ret != 0) {
			DBGPRINT(RT_DEBUG_ERROR, ("cal_atomic get failed(ret=%d)\n", ret));
			return;
		}
	}
#endif /* RTMP_MAC_USB */

	rlt_rf_read(pAd, RF_BANK7, RF_R73, &rf_b7_73);
	rlt_rf_read(pAd, RF_BANK0, RF_R66, &rf_b0_66);
	rlt_rf_read(pAd, RF_BANK0, RF_R67, &rf_b0_67);

	/*
		1. Set 0dB Gain:
			WIFI_RF_CR_WRITE(7,73,0x02)
	*/
	rlt_rf_write(pAd, RF_BANK7, RF_R73, 0x02);

	/*
		2. Calibration Switches:
			WIFI_RF_CR_WRITE(0,66,0x23)
	*/
	rlt_rf_write(pAd, RF_BANK0, RF_R66, 0x23);

	/*
		3. Offset-measurement configuration:
			WIFI_RF_CR_WRITE(0,67,0x01)
	*/
	rlt_rf_write(pAd, RF_BANK0, RF_R67, 0x01);

	/*
		4. Select Level meter from ADC.q:
			WIFI_BBP_CR_WRITE(0x2088,0x00080055)
	*/
	RTMP_BBP_IO_WRITE32(pAd, CORE_R34, 0x00080055);

	/*
		5. Wait until it's done:
			wait until 0x2088[4] = 0
	*/
	for (MTxCycle = 0; MTxCycle < 2000; MTxCycle++) {
		reg_val = RTMP_BBP_IO_READ32(pAd, CORE_R34);
		if ((reg_val & 0x10) == 0)
			break;
		RTMPusecDelay(3);
	}

	if (MTxCycle >= 2000) {
		reg_val &= ~(0x10);
		RTMP_BBP_IO_WRITE32(pAd, CORE_R34, reg_val);
		goto done;
	}

	/*
		6. Read Dout (0x0041208c<7:0>=adc_out<8:1>):
			WIFI_BBP_CR_READ(0x208c) //$Dout

	*/
	Dout = RTMP_BBP_IO_READ32(pAd, CORE_R35);
	Dout &= 0xFF;

	if ((Dout & 0x80) == 0x00)
		Dout &= 0x7F; /* Positive number */
	else
		Dout |= 0xFFFFFF00; /* Negative number */

	/*
		7. Read D25 from EEPROM:
			Read EEPROM 0xD1 // $Offset (signed integer)
	*/

	/*
		8. Calculate temperature:
			T = 3.5*(Dout-D25) + 25
	*/
	temperature = (35*(Dout-pAd->chipCap.TemperatureOffset))/10 + 25;
	DBGPRINT(RT_DEBUG_TRACE, ("%s - Dout=%d, TemperatureOffset = %d, temperature = %d\n", __FUNCTION__, Dout, pAd->chipCap.TemperatureOffset, temperature));
	if (pAd->chipCap.LastTemperatureforVCO == 0x7FFF)
		pAd->chipCap.LastTemperatureforVCO = temperature;
	if (pAd->chipCap.LastTemperatureforCal == 0x7FFF)
		pAd->chipCap.LastTemperatureforCal = temperature;
	pAd->chipCap.NowTemperature = temperature;

done:
	/*
		9. Restore RF CR:
			B7. R73, B0.R66, B0.R67
	*/
	rlt_rf_write(pAd, RF_BANK7, RF_R73, rf_b7_73);
	rlt_rf_write(pAd, RF_BANK0, RF_R66, rf_b0_66);
	rlt_rf_write(pAd, RF_BANK0, RF_R67, rf_b0_67);


#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd))
		up(&pAd->cal_atomic);

#endif /* RTMP_MAC_USB */
}

bool mt76x0_get_tssi_report(struct rtmp_adapter *pAd, bool bResetTssiInfo,
	char *pTssiReport)
{
	u32 wait = 0, reg_val = 0;
	u8 rf_b7_73 = 0, rf_b0_66 = 0, rf_b0_67 = 0;
	bool status;

	rlt_rf_read(pAd, RF_BANK7, RF_R73, &rf_b7_73);
	rlt_rf_read(pAd, RF_BANK0, RF_R66, &rf_b0_66);
	rlt_rf_read(pAd, RF_BANK0, RF_R67, &rf_b0_67);

	/*
		1. Set 0dB Gain:
			WIFI_RF_CR_WRITE(7,73,0x02)
	*/
	rlt_rf_write(pAd, RF_BANK7, RF_R73, 0x02);

	/*
		2. Calibration Switches:
			WIFI_RF_CR_WRITE(0,66,0x23)
	*/
	rlt_rf_write(pAd, RF_BANK0, RF_R66, 0x23);

	/*
		3. Offset-measurement configuration:
			WIFI_RF_CR_WRITE(0,67,0x01)
	*/
	rlt_rf_write(pAd, RF_BANK0, RF_R67, 0x01);

	/*
		4. Select Level meter from ADC.q:
			WIFI_BBP_CR_WRITE(0x2088,0x00080055)
	*/
	RTMP_BBP_IO_WRITE32(pAd, CORE_R34, 0x00080055);

	/*
		5. Wait until it's done:
			wait until 0x2088[4] = 0
	*/
	for (wait = 0; wait < 2000; wait++) {
		reg_val = RTMP_BBP_IO_READ32(pAd, CORE_R34);
		if ((reg_val & 0x10) == 0)
			break;
		RTMPusecDelay(3);
	}

	if (wait >= 2000) {
		reg_val &= ~(0x10);
		RTMP_BBP_IO_WRITE32(pAd, CORE_R34, reg_val);
		status = false;
		goto done;
	}

	/*
		6. Read Dout (0x0041208c<7:0>=adc_out<8:1>):
			WIFI_BBP_CR_READ(0x208c) //$Dout

	*/
	reg_val = RTMP_BBP_IO_READ32(pAd, CORE_R35);
	reg_val &= 0xFF;
	if ((reg_val & 0x80) == 0x80)
		reg_val |= 0xFFFFFF00; /* Negative number */
	*pTssiReport = reg_val;

	status = true;

done:
	/*
		Restore RF CR
			B7. R73, B0.R66, B0.R67
	*/
	rlt_rf_write(pAd, RF_BANK7, RF_R73, rf_b7_73);
	rlt_rf_write(pAd, RF_BANK0, RF_R66, rf_b0_66);
	rlt_rf_write(pAd, RF_BANK0, RF_R67, rf_b0_67);

	return status;
}

bool get_temp_tx_alc_level(
	IN struct rtmp_adapter *pAd,
	IN bool enable_tx_alc,
	IN CHAR temp_ref,
	IN char *temp_minus_bdy,
	IN char *temp_plus_bdy,
	IN u8 max_bdy_level,
	IN u8 tx_alc_step,
	IN CHAR current_temp,
	OUT char *comp_level)
{
	INT idx = 0;

	if ((temp_minus_bdy == NULL) || (temp_plus_bdy == NULL)) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s(): temp table boundary is NULL\n", __FUNCTION__));
		return false;
	}

	if (enable_tx_alc) {
		if (current_temp < temp_minus_bdy[1]) {
			/*
				Reading is larger than the reference value check
				for how large we need to decrease the Tx power
			*/
			for (idx = 1; idx < max_bdy_level; idx++) {
				if (current_temp >= temp_minus_bdy[idx])
					break; /* level range found */
			}

			/* The index is the step we should decrease, idx = 0 means there is nothing to compensate */
			*comp_level = -(tx_alc_step * (idx-1));
			DBGPRINT(RT_DEBUG_TRACE,
				("-- Tx Power:: current_temp=%d, temp_ref=%d, tx_alc_step=%d, step = -%d, comp_level = %d\n",
				current_temp, temp_ref, tx_alc_step, idx-1, *comp_level));
		} else if (current_temp > temp_plus_bdy[1]) {
			/*
				Reading is smaller than the reference value check
				for how large we need to increase the Tx power
			*/
			for (idx = 1; idx < max_bdy_level; idx++) {
				if (current_temp <= temp_plus_bdy[idx])
					break; /* level range found */
			}

			/* The index is the step we should increase, idx = 0 means there is nothing to compensate */
			*comp_level = tx_alc_step * (idx-1);
			DBGPRINT(RT_DEBUG_TRACE,
				("++ Tx Power:: current_temp=%d, temp_ref=%d, tx_alc_step=%d, step = +%d, , comp_level = %d\n",
				current_temp, temp_ref, tx_alc_step, idx-1, *comp_level));
		} else {
			*comp_level = 0;
			DBGPRINT(RT_DEBUG_TRACE,
				("  Tx Power:: current_temp=%d, temp_ref=%d, tx_alc_step=%d, step = +%d\n",
				current_temp, temp_ref, tx_alc_step, 0));
		}
	} else {
		DBGPRINT(RT_DEBUG_ERROR, ("%s(): enable_tx_alc = %s\n",
			__FUNCTION__, (enable_tx_alc) == true ? "true" : "false"));
		return false;
	}

	return true;
}


void mt76x0_temp_tx_alc(struct rtmp_adapter *pAd)
{
	bool bResetTssiInfo = true, enable_tx_alc;
	u8 *temp_minus_bdy, *temp_plus_bdy, *tx_alc_comp;
	u8 temp_ref;

	if ((pAd->CommonCfg.Channel > 14) ?
		(pAd->bAutoTxAgcA == false) : (pAd->bAutoTxAgcG == false))
		return;

	if (pAd->CommonCfg.Channel <= 14) {
		/* Use group settings of G band */
		temp_ref = pAd->TssiRefG;
		enable_tx_alc = pAd->bAutoTxAgcG;
		tx_alc_comp = &pAd->TxAgcCompensateG;
		temp_minus_bdy = pAd->TssiMinusBoundaryG;
		temp_plus_bdy = pAd->TssiPlusBoundaryG;
	} else if (pAd->CommonCfg.Channel <= pAd->ChBndryIdx) {
		/* Use group#1 settings of A band */
		temp_ref = pAd->TssiRefA;
		enable_tx_alc = pAd->bAutoTxAgcA;
		tx_alc_comp = &pAd->TxAgcCompensateA;
		temp_minus_bdy = pAd->TssiMinusBoundaryA[0];
		temp_plus_bdy = pAd->TssiPlusBoundaryA[0];
	} else {
		/* Use group#2 settings of A band */
		temp_ref = pAd->TssiRefA;
		enable_tx_alc = pAd->bAutoTxAgcA;
		tx_alc_comp = &pAd->TxAgcCompensateA;
		temp_minus_bdy = pAd->TssiMinusBoundaryA[1];
		temp_plus_bdy = pAd->TssiPlusBoundaryA[1];
	}

	if (mt76x0_get_tssi_report(pAd, bResetTssiInfo, &pAd->CurrTemperature) == true) {
		if (get_temp_tx_alc_level(pAd, enable_tx_alc, temp_ref,
					temp_minus_bdy, temp_plus_bdy,
					8, /* to do: make a definition */
					2, pAd->CurrTemperature,
					tx_alc_comp) == true) {
			u32 mac_val;
			CHAR last_delta_pwr, delta_pwr = 0;

			/* adjust compensation value by MP temperature readings (i.e., e2p[77h]) */
			if (pAd->CommonCfg.Channel <= 14)
				delta_pwr = pAd->TxAgcCompensateG - pAd->mp_delta_pwr;
			else
				delta_pwr = pAd->TxAgcCompensateA - pAd->mp_delta_pwr;

			mac_val = mt7610u_read32(pAd, TX_ALC_CFG_1);
			/* 6-bit representation ==> 8-bit representation (2's complement) */
			pAd->DeltaPwrBeforeTempComp = (mac_val & 0x20) ?
						      ((mac_val & 0x3F) | 0xC0) :
						      (mac_val & 0x3f);

			last_delta_pwr = pAd->LastTempCompDeltaPwr;
			pAd->LastTempCompDeltaPwr = delta_pwr;
			pAd->DeltaPwrBeforeTempComp -= last_delta_pwr;
			delta_pwr += pAd->DeltaPwrBeforeTempComp;
			/* 8-bit representation ==> 6-bit representation (2's complement) */
			delta_pwr = (delta_pwr & 0x80) ? \
							((delta_pwr & 0x1f) | 0x20) : (delta_pwr & 0x3f);
			/*
				Write compensation value into TX_ALC_CFG_1,
				delta_pwr (unit: 0.5dB) will be compensated by TX_ALC_CFG_1
			*/
			mac_val = mt7610u_read32(pAd, TX_ALC_CFG_1);
			mac_val = (mac_val & (~0x3f)) | delta_pwr;
			mt7610u_write32(pAd, TX_ALC_CFG_1, mac_val);

			DBGPRINT(RT_DEBUG_TRACE,
				("%s - delta_pwr = %d, TssiCalibratedOffset = %d, TssiMpOffset = %d, 0x13B4 = 0x%08x, %s = %d, DeltaPwrBeforeTempComp = %d, LastTempCompDeltaPwr =%d\n",
				__FUNCTION__,
				pAd->LastTempCompDeltaPwr,
				pAd->TssiCalibratedOffset,
				pAd->mp_delta_pwr,
				mac_val,
				(pAd->CommonCfg.Channel <= 14) ? "TxAgcCompensateG" : "TxAgcCompensateA",
				(pAd->CommonCfg.Channel <= 14) ? pAd->TxAgcCompensateG : pAd->TxAgcCompensateA,
				pAd->DeltaPwrBeforeTempComp,
				last_delta_pwr));
		} else {
			DBGPRINT(RT_DEBUG_ERROR, ("%s(): failed to get the compensation level\n", __FUNCTION__));
		}
	}
}


static void adjust_temp_tx_alc_table(struct rtmp_adapter *pAd, CHAR band,
	char *temp_minus_bdy, char *temp_plus_bdy, CHAR temp_reference)
{
	INT idx = 0;
	CHAR upper_bound = 127, lower_bound = -128;

	DBGPRINT(RT_DEBUG_OFF, ("%s: upper_bound = 0x%02x (%d), lower_bound = 0x%02x (%d)\n",
		__FUNCTION__, upper_bound, upper_bound, lower_bound, lower_bound));

	DBGPRINT(RT_DEBUG_OFF, ("*** %s: %s_temp_bdy_table[-7 .. +7] = %d %d %d %d %d %d %d * %d * %d %d %d %d %d %d %d, temp_reference=%d\n",
		__func__,
		(band == A_BAND) ? "5G" : "2.4G",
		temp_minus_bdy[7], temp_minus_bdy[6], temp_minus_bdy[5],
		temp_minus_bdy[4], temp_minus_bdy[3], temp_minus_bdy[2], temp_minus_bdy[1],
		(band == A_BAND) ? (CHAR)pAd->TssiRefA : (CHAR)pAd->TssiRefG,
		temp_plus_bdy[1], temp_plus_bdy[2], temp_plus_bdy[3], temp_plus_bdy[4],
		temp_plus_bdy[5], temp_plus_bdy[6], temp_plus_bdy[7], temp_reference));

	for (idx = 0; idx < 8; idx++) {
		if ((lower_bound - temp_minus_bdy[idx]) <= temp_reference)
			temp_minus_bdy[idx] += temp_reference;
		else
			temp_minus_bdy[idx] = lower_bound;

		if ((upper_bound - temp_plus_bdy[idx]) >= temp_reference)
			temp_plus_bdy[idx] += temp_reference;
		else
			temp_plus_bdy[idx] = upper_bound;

		ASSERT(temp_minus_bdy[idx] >= lower_bound);
		ASSERT(temp_plus_bdy[idx] <= upper_bound);
	}

	if (band == A_BAND)
		pAd->TssiRefA = temp_minus_bdy[0];
	else
		pAd->TssiRefG = temp_minus_bdy[0];

	DBGPRINT(RT_DEBUG_OFF,("%s: %s_temp_bdy_table[-7 .. +7] = %d %d %d %d %d %d %d * %d * %d %d %d %d %d %d %d, temp_reference=%d\n",
		__FUNCTION__,
		(band == A_BAND) ? "5G" : "2.4G",
		temp_minus_bdy[7], temp_minus_bdy[6], temp_minus_bdy[5],
		temp_minus_bdy[4], temp_minus_bdy[3], temp_minus_bdy[2], temp_minus_bdy[1],
		(band == A_BAND) ? (CHAR)pAd->TssiRefA : (CHAR)pAd->TssiRefG,
		temp_plus_bdy[1], temp_plus_bdy[2], temp_plus_bdy[3], temp_plus_bdy[4],
		temp_plus_bdy[5], temp_plus_bdy[6], temp_plus_bdy[7], temp_reference));
}


static void adjust_mp_temp(struct rtmp_adapter *pAd, char *temp_minus_bdy,
	char *temp_plus_bdy)
{
	EEPROM_TX_PWR_STRUC e2p_value;
	CHAR mp_temp, idx = 0, mp_offset = 0;

	e2p_value.word = mt7610u_read_eeprom16(pAd, 0x10C);
	mp_temp = e2p_value.field.Byte1;

	if (mp_temp < temp_minus_bdy[1]) {
		/*
			mp_temperature is larger than the reference value
			check for how large we need to adjust the Tx power
		*/
		for (idx = 1; idx < 8; idx++) {
			if (mp_temp >= temp_minus_bdy[idx]) /* the range has been found */
				break;
		}

		/*
			The index is the step we should decrease,
			idx = 0 means there is no need to adjust the Tx power
		*/
		mp_offset = -(2 * (idx-1));
		pAd->mp_delta_pwr = mp_offset;
		DBGPRINT(RT_DEBUG_OFF, ("mp_temperature=0x%02x, step = -%d\n", mp_temp, idx-1));
	} else if (mp_temp > temp_plus_bdy[1]) {
		/*
			mp_temperature is smaller than the reference value
			check for how large we need to adjust the Tx power
		*/
		for (idx = 1; idx < 8; idx++) {
			if (mp_temp <= temp_plus_bdy[idx]) /* the range has been found */
				break;
		}

		/*
			The index is the step we should increase,
			idx = 0 means there is no need to adjust the Tx power
		*/
		mp_offset = 2 * (idx-1);
		pAd->mp_delta_pwr = mp_offset;
		DBGPRINT(RT_DEBUG_OFF, ("mp_temperature=0x%02x, step = +%d\n", mp_temp, idx-1));
	} else {
		pAd->mp_delta_pwr = 0;
		DBGPRINT(RT_DEBUG_OFF, ("mp_temperature=0x%02x, step = +%d\n", mp_temp, 0));
	}
}


bool load_temp_tx_alc_table(struct rtmp_adapter *pAd, CHAR band,
	USHORT e2p_start_addr, USHORT e2p_end_addr,
	u8 *bdy_table, const INT start_idx, const u32 table_size)
{
	u16 e2p_value;
	INT e2p_idx = 0, table_idx = 0;
	CHAR table_sign; /* +1 for plus table; -1 for minus table */

	table_sign = (e2p_start_addr < e2p_end_addr) ? 1 : (-1);

	if (start_idx < table_size) {
		table_idx = start_idx;
	} else {
		DBGPRINT(RT_DEBUG_ERROR, ("%s(): ERROR - incorrect start index (%d)\n",
			__FUNCTION__, start_idx));
		return false;
	}

	DBGPRINT(RT_DEBUG_OFF, ("%s(): load %s %s table from eeprom 0x%x to 0x%x (start_idx = %d)\n",
		__FUNCTION__,
		(band == A_BAND) ? "5G" : "2.4G",
		(table_sign == 1) ? "plus" : "minus",
		e2p_start_addr, e2p_end_addr, start_idx));

	for (e2p_idx = e2p_start_addr;
	     e2p_idx != (e2p_end_addr + (2*table_sign));
	     e2p_idx = e2p_idx + (2*table_sign)) {
		if ((e2p_start_addr % 2) != 0) {
			e2p_start_addr--;
			e2p_idx--;
		}

		e2p_value = mt7610u_read_eeprom16(pAd, e2p_idx);

		if (e2p_idx == e2p_start_addr) {
			if (table_sign > 0)
				bdy_table[table_idx++] = (u8)((e2p_value >> 8) & 0xFF);
			else
				bdy_table[table_idx++] = (u8)(e2p_value & 0xFF);
		} else {
			if (table_sign > 0) {
				bdy_table[table_idx++] = (u8)(e2p_value & 0xFF);
				bdy_table[table_idx++] = (u8)((e2p_value >> 8) & 0xFF);
			} else {
				bdy_table[table_idx++] = (u8)((e2p_value >> 8) & 0xFF);
				bdy_table[table_idx++] = (u8)(e2p_value & 0xFF);
			}
		}

		if (table_idx >= table_size)
			break;
	}

	if (table_idx > table_size) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s(): WARNING - eeprom table may not be loaded completely\n", __FUNCTION__));
		return false;
	} else {
		for (table_idx = 0; table_idx < table_size; table_idx++)
			DBGPRINT(RT_DEBUG_TRACE, ("\tboundary_table[%d] = %3d (0x%02X)\n",
				table_idx, (CHAR)bdy_table[table_idx], bdy_table[table_idx]));
	}

	return true;
}


void mt76x0_temp_tx_alc_init(struct rtmp_adapter *pAd)
{
	/* Initialize group settings of A band */
	pAd->TxAgcCompensateA = 0;
	pAd->TssiRefA = 0;
	pAd->TssiMinusBoundaryA[0][0] = 0;
	pAd->TssiMinusBoundaryA[1][0] = 0;
	pAd->TssiPlusBoundaryA[0][0] = 0;
	pAd->TssiPlusBoundaryA[1][0] = 0;

	/* Initialize group settings of G band */
	pAd->TxAgcCompensateG = 0;
	pAd->TssiRefG = 0;
	pAd->TssiMinusBoundaryG[0] = 0;
	pAd->TssiPlusBoundaryG[0] = 0;

	pAd->DeltaPwrBeforeTempComp = 0;
	pAd->LastTempCompDeltaPwr = 0;

	adjust_temp_tx_alc_table(pAd, A_BAND, pAd->TssiMinusBoundaryA[0],
								pAd->TssiPlusBoundaryA[0], pAd->TssiCalibratedOffset);
	adjust_mp_temp(pAd, pAd->TssiMinusBoundaryA[0], pAd->TssiPlusBoundaryA[0]);

	adjust_temp_tx_alc_table(pAd, A_BAND, pAd->TssiMinusBoundaryA[1],
								pAd->TssiPlusBoundaryA[1], pAd->TssiCalibratedOffset);
	adjust_mp_temp(pAd, pAd->TssiMinusBoundaryA[1], pAd->TssiPlusBoundaryA[1]);

	adjust_temp_tx_alc_table(pAd, BG_BAND, pAd->TssiMinusBoundaryG,
								pAd->TssiPlusBoundaryG, pAd->TssiCalibratedOffset);
	adjust_mp_temp(pAd, pAd->TssiMinusBoundaryG, pAd->TssiPlusBoundaryG);
}

void mt76x0_read_tx_alc_info_from_eeprom(struct rtmp_adapter *pAd)
{
	bool status = true;
	u16 e2p_value = 0;

	if (IS_MT76x0(pAd)) {
		e2p_value = mt7610u_read_eeprom16(pAd, 0xD0);
		e2p_value = (e2p_value & 0xFF00) >> 8;
		DBGPRINT(RT_DEBUG_OFF, ("%s: EEPROM_MT76x0_TEMPERATURE_OFFSET (0xD1) = 0x%x\n",
			__FUNCTION__, e2p_value));

		if ((e2p_value & 0xFF) == 0xFF) {
			pAd->chipCap.TemperatureOffset = -10;
		} else {
			if ((e2p_value & 0x80) == 0x80) /* Negative number */
				e2p_value |= 0xFF00;

			pAd->chipCap.TemperatureOffset = (SHORT)e2p_value;
		}
		DBGPRINT(RT_DEBUG_OFF, ("%s: TemperatureOffset = 0x%x\n",
			__FUNCTION__, pAd->chipCap.TemperatureOffset));
	}

	if (pAd->bAutoTxAgcG | pAd->bAutoTxAgcA) {
		e2p_value = mt7610u_read_eeprom16(pAd, 0xD0);
		pAd->TssiCalibratedOffset = (e2p_value >> 8);

		/* 5G Tx power compensation channel boundary index */
		e2p_value = mt7610u_read_eeprom16(pAd, 0x10C);
		pAd->ChBndryIdx = (u8)(e2p_value & 0xFF);
		DBGPRINT(RT_DEBUG_OFF, ("%s(): channel boundary index = %u, temp reference offset = %d\n",
			__FUNCTION__, pAd->ChBndryIdx, pAd->TssiCalibratedOffset));

		/* Load group#1 settings of A band */
		load_temp_tx_alc_table(
			pAd, A_BAND, 0xF6, 0xF0, pAd->TssiMinusBoundaryA[0], 1, sizeof(pAd->TssiMinusBoundaryA[0]));
		load_temp_tx_alc_table(
			pAd, A_BAND, 0xF7, 0xFD, pAd->TssiPlusBoundaryA[0], 1, sizeof(pAd->TssiPlusBoundaryA[0]));

		/* Load group#2 settings of A band */
		load_temp_tx_alc_table(
			pAd, A_BAND, 0x104, 0xFE, pAd->TssiMinusBoundaryA[1], 1, sizeof(pAd->TssiMinusBoundaryA[1]));
		load_temp_tx_alc_table(
			pAd, A_BAND, 0x105, 0x10B, pAd->TssiPlusBoundaryA[1], 1, sizeof(pAd->TssiPlusBoundaryA[1]));

		/* Load group settings of G band */
		load_temp_tx_alc_table(
			pAd, BG_BAND, 0x1B6, 0x1B0, pAd->TssiMinusBoundaryG, 1, sizeof(pAd->TssiMinusBoundaryG));
		load_temp_tx_alc_table(
			pAd, BG_BAND, 0x1B7, 0x1BD, pAd->TssiPlusBoundaryG, 1, sizeof(pAd->TssiPlusBoundaryG));

		mt76x0_temp_tx_alc_init(pAd);
	} else
		DBGPRINT(RT_DEBUG_ERROR, ("Temperature Tx ALC not enabled\n"));
}

