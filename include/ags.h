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


#ifndef __AGS_H__
#define __AGS_H__


extern u8 AGS1x1HTRateTable[];
extern u8 AGS2x2HTRateTable[];
extern u8 AGS3x3HTRateTable[];
#ifdef DOT11_VHT_AC
extern u8 Ags1x1VhtRateTable[];
extern u8 Ags2x2VhtRateTable[];
#endif /* DOT11_VHT_AC */

#define AGS_TX_QUALITY_WORST_BOUND       8
#define AGS_QUICK_RA_TIME_INTERVAL        50	/* 50ms */

/* The size, in bytes, of an AGS entry in the rate switch table */
#define SIZE_OF_AGS_RATE_TABLE_ENTRY	9

typedef struct _RTMP_RA_AGS_TB {
	u8 ItemNo;

	u8 STBC:1;
	u8 ShortGI:1;
	u8 BW:2;
	u8 Mode:3;
	u8 Rsv1:1;

	u8 Nss:2; // NSS_XXX (VHT only)
	u8 rsv2:6; // Reserved

	u8 CurrMCS;
	u8 TrainUp;
	u8 TrainDown;
	u8 downMcs;
	u8 upMcs3;
	u8 upMcs2;
	u8 upMcs1;
}RTMP_RA_AGS_TB;


/* AGS control */
typedef struct _AGS_CONTROL {
	u8 MCSGroup; /* The MCS group under testing */
	u8 lastRateIdx;
} AGS_CONTROL,*PAGS_CONTROL;


/* The statistics information for AGS */
typedef struct _AGS_STATISTICS_INFO {
	CHAR	RSSI;
	ULONG	TxErrorRatio;
	ULONG	AccuTxTotalCnt;
	ULONG	TxTotalCnt;
	ULONG	TxSuccess;
	ULONG	TxRetransmit;
	ULONG	TxFailCount;
} AGS_STATISTICS_INFO, *PAGS_STATISTICS_INFO;


/* Support AGS (Adaptive Group Switching) */
#define SUPPORT_AGS(__pAd)		((__pAd)->rateAlg == RATE_ALG_AGS)

#ifdef DOT11_VHT_AC
#define AGS_IS_USING(__pAd, __pRateTable)	\
    (SUPPORT_AGS(__pAd) && \
     ((__pRateTable == AGS1x1HTRateTable) ||\
      (__pRateTable == AGS2x2HTRateTable) ||\
      (__pRateTable == AGS3x3HTRateTable) ||\
      (__pRateTable == Ags1x1VhtRateTable) ||\
      (__pRateTable == Ags2x2VhtRateTable)))
#else
#define AGS_IS_USING(__pAd, __pRateTable)	\
    (SUPPORT_AGS(__pAd) && \
     ((__pRateTable == AGS1x1HTRateTable) || \
      (__pRateTable == AGS2x2HTRateTable) || \
      (__pRateTable == AGS3x3HTRateTable)))
#endif /* DOT11_VHT_AC */

#endif /* __AGS_H__ */

