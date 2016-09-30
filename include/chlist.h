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


#ifndef __CHLIST_H__
#define __CHLIST_H__

#include "rtmp_type.h"
#include "rtmp_def.h"


typedef struct _CH_DESC {
	u8 FirstChannel;
	u8 NumOfCh;
	u8 ChannelProp;
}CH_DESC, *PCH_DESC;

typedef struct _COUNTRY_REGION_CH_DESC {
	u8 RegionIndex;
	PCH_DESC pChDesc;
}COUNTRY_REGION_CH_DESC, *PCOUNTRY_REGION_CH_DESC;

#ifdef EXT_BUILD_CHANNEL_LIST
#define ODOR			0
#define IDOR			1
#define BOTH			2

typedef struct _CH_DESP {
	u8 FirstChannel;
	u8 NumOfCh;
	CHAR MaxTxPwr;			/* dBm */
	u8 Geography;			/* 0:out door, 1:in door, 2:both */
	BOOLEAN DfsReq;			/* Dfs require, 0: No, 1: yes. */
} CH_DESP, *PCH_DESP;

typedef struct _CH_REGION {
	u8 CountReg[3];
	u8 DfsType;			/* 0: CE, 1: FCC, 2: JAP, 3:JAP_W53, JAP_W56 */
	CH_DESP *pChDesp;
} CH_REGION, *PCH_REGION;

extern CH_REGION ChRegion[];
#endif /* EXT_BUILD_CHANNEL_LIST */

#ifdef SINGLE_SKU_V2
#define SINGLE_SKU_TABLE_LENGTH			(SINGLE_SKU_TABLE_CCK_LENGTH+SINGLE_SKU_TABLE_OFDM_LENGTH+(SINGLE_SKU_TABLE_HT_LENGTH*2)+SINGLE_SKU_TABLE_VHT_LENGTH)

#define SINGLE_SKU_TABLE_CCK_LENGTH		4
#define SINGLE_SKU_TABLE_OFDM_LENGTH		8
#define SINGLE_SKU_TABLE_HT_LENGTH		16
#define SINGLE_SKU_TABLE_VHT_LENGTH		10 /* VHT MCS 0 ~ 9 */

typedef struct _CH_POWER_{
	DL_LIST		List;
	u8 	channel;
	u8 	num;
	u8 	PwrCCK[SINGLE_SKU_TABLE_CCK_LENGTH];
	u8 	PwrOFDM[SINGLE_SKU_TABLE_OFDM_LENGTH];
	u8 	PwrHT20[SINGLE_SKU_TABLE_HT_LENGTH];
	u8 	PwrHT40[SINGLE_SKU_TABLE_HT_LENGTH];
	u8 	PwrVHT80[SINGLE_SKU_TABLE_VHT_LENGTH];
}CH_POWER;
#endif /* SINGLE_SKU_V2 */

typedef struct _CH_FREQ_MAP_{
	UINT16		channel;
	UINT16		freqKHz;
}CH_FREQ_MAP;

extern CH_FREQ_MAP CH_HZ_ID_MAP[];
extern int CH_HZ_ID_MAP_NUM;


#define     MAP_CHANNEL_ID_TO_KHZ(_ch, _khz)                 \
			RTMP_MapChannelID2KHZ(_ch, (u32 *)&(_khz))
#define     MAP_KHZ_TO_CHANNEL_ID(_khz, _ch)                 \
			RTMP_MapKHZ2ChannelID(_khz, (INT *)&(_ch))

/* Check if it is Japan W53(ch52,56,60,64) channel. */
#define JapanChannelCheck(_ch)  ((_ch == 52) || (_ch == 56) || (_ch == 60) || (_ch == 64))


#ifdef EXT_BUILD_CHANNEL_LIST
void BuildChannelListEx(
	IN struct rtmp_adapter *pAd);

void BuildBeaconChList(
	IN struct rtmp_adapter *pAd,
	OUT u8 *pBuf,
	OUT	PULONG pBufLen);
#endif /* EXT_BUILD_CHANNEL_LIST */

#ifdef DOT11_N_SUPPORT
void N_ChannelCheck(struct rtmp_adapter*pAd);
u8 N_SetCenCh(struct rtmp_adapter*pAd, u8 channel);
BOOLEAN N_ChannelGroupCheck(struct rtmp_adapter*pAd, u8 channel);

#endif /* DOT11_N_SUPPORT */

u8 GetCuntryMaxTxPwr(
	IN struct rtmp_adapter *pAd,
	IN u8 channel);

void RTMP_MapChannelID2KHZ(
	IN u8 Ch,
	OUT u32 *pFreq);

void RTMP_MapKHZ2ChannelID(
	IN ULONG Freq,
	OUT INT *pCh);

u8 GetChannel_5GHZ(
	IN PCH_DESC pChDesc,
	IN u8 index);

u8 GetChannel_2GHZ(
	IN PCH_DESC pChDesc,
	IN u8 index);

u8 GetChannelFlag(
	IN PCH_DESC pChDesc,
	IN u8 index);

UINT16 TotalChNum(
	IN PCH_DESC pChDesc);

#endif /* __CHLIST_H__ */

