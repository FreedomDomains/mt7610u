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


#ifndef __DRS_EXTR_H__
#define __DRS_EXTR_H__

struct rtmp_adapter;
struct _MAC_TABLE_ENTRY;


typedef struct _RTMP_TX_RATE {
	u8 mode;
	u8 bw;
	u8 mcs;
	u8 nss;
	u8 sgi;
	u8 stbc;
}RTMP_TX_RATE;


typedef struct _RTMP_RA_LEGACY_TB
{
	u8   ItemNo;
#ifdef RT_BIG_ENDIAN
	u8 Rsv2:1;
	u8 Mode:3;
	u8 BW:2;
	u8 ShortGI:1;
	u8 STBC:1;
#else
	u8 STBC:1;
	u8 ShortGI:1;
	u8 BW:2;
	u8 Mode:3;
	u8 Rsv2:1;
#endif
	u8   CurrMCS;
	u8   TrainUp;
	u8   TrainDown;
} RTMP_RA_LEGACY_TB;

#define PTX_RA_LEGACY_ENTRY(pTable, idx)	((RTMP_RA_LEGACY_TB *)&(pTable[(idx+1)*5]))


#ifdef NEW_RATE_ADAPT_SUPPORT
typedef struct  _RTMP_RA_GRP_TB
{
	u8   ItemNo;
#ifdef RT_BIG_ENDIAN
	u8 Rsv2:1;
	u8 Mode:3;
	u8 BW:2;
	u8 ShortGI:1;
	u8 STBC:1;
#else
	u8 STBC:1;
	u8 ShortGI:1;
	u8 BW:2;
	u8 Mode:3;
	u8 Rsv2:1;
#endif
	u8   CurrMCS;
	u8   TrainUp;
	u8   TrainDown;
	u8 downMcs;
	u8 upMcs3;
	u8 upMcs2;
	u8 upMcs1;
	u8 dataRate;
} RTMP_RA_GRP_TB;

#define PTX_RA_GRP_ENTRY(pTable, idx)	((RTMP_RA_GRP_TB *)&(pTable[(idx+1)*10]))
#endif /* NEW_RATE_ADAPT_SUPPORT */

#define RATE_TABLE_SIZE(pTable)			((pTable)[0])		/* Byte 0 is number of rate indices */
#define RATE_TABLE_INIT_INDEX(pTable)	((pTable)[1])		/* Byte 1 is initial rate index */

enum RATE_ADAPT_ALG{
	RATE_ALG_GRP = 2,
	RATE_ALG_MAX_NUM
};


typedef enum {
	RAL_OLD_DRS,
	RAL_NEW_DRS,
	RAL_QUICK_DRS
}RA_LOG_TYPE;


extern u8 RateSwitchTable11B[];
extern u8 RateSwitchTable11G[];
extern u8 RateSwitchTable11BG[];

#ifdef DOT11_N_SUPPORT
extern u8 RateSwitchTable11BGN1S[];
extern u8 RateSwitchTable11BGN2S[];
extern u8 RateSwitchTable11BGN2SForABand[];
extern u8 RateSwitchTable11N1S[];
extern u8 RateSwitchTable11N1SForABand[];
extern u8 RateSwitchTable11N2S[];
extern u8 RateSwitchTable11N2SForABand[];
extern u8 RateSwitchTable11BGN3S[];
extern u8 RateSwitchTable11BGN3SForABand[];

#ifdef NEW_RATE_ADAPT_SUPPORT
extern u8 RateSwitchTableAdapt11N1S[];
extern u8 RateSwitchTableAdapt11N2S[];
extern u8 RateSwitchTableAdapt11N3S[];

#define PER_THRD_ADJ			1

/* ADAPT_RATE_TABLE - true if pTable is one of the Adaptive Rate Switch tables */
extern u8 RateTableVht1S[];
extern u8 RateTableVht1S_MCS7[];
extern u8 RateTableVht2S[];

#define ADAPT_RATE_TABLE(pTable)	((pTable)==RateSwitchTableAdapt11N1S ||\
									(pTable)==RateSwitchTableAdapt11N2S ||\
									(pTable)==RateSwitchTableAdapt11N3S ||\
									(pTable)==RateTableVht1S ||\
									(pTable)==RateTableVht1S_MCS7 ||\
									(pTable)==RateTableVht2S)
#else
#define ADAPT_RATE_TABLE(pTable)	((pTable)==RateSwitchTableAdapt11N1S || \
									(pTable)==RateSwitchTableAdapt11N2S || \
									(pTable)==RateSwitchTableAdapt11N3S)
#endif /* NEW_RATE_ADAPT_SUPPORT */
#endif /* DOT11_N_SUPPORT */


/* FUNCTION */
void MlmeGetSupportedMcs(
	IN struct rtmp_adapter *pAd,
	IN u8 *pTable,
	OUT CHAR mcs[]);

u8 MlmeSelectTxRate(
	IN struct rtmp_adapter *pAd,
	IN struct _MAC_TABLE_ENTRY *pEntry,
	IN CHAR mcs[],
	IN CHAR Rssi,
	IN CHAR RssiOffset);

void MlmeClearTxQuality(struct _MAC_TABLE_ENTRY *pEntry);
void MlmeClearAllTxQuality(struct _MAC_TABLE_ENTRY *pEntry);
void MlmeDecTxQuality(struct _MAC_TABLE_ENTRY *pEntry, u8 rateIndex);
USHORT MlmeGetTxQuality(struct _MAC_TABLE_ENTRY *pEntry, u8 rateIndex);
void MlmeSetTxQuality(
	IN struct _MAC_TABLE_ENTRY *pEntry,
	IN u8 rateIndex,
	IN USHORT txQuality);



void MlmeOldRateAdapt(
	IN struct rtmp_adapter *pAd,
	IN struct _MAC_TABLE_ENTRY *pEntry,
	IN u8 		CurrRateIdx,
	IN u8 		UpRateIdx,
	IN u8 		DownRateIdx,
	IN ULONG			TrainUp,
	IN ULONG			TrainDown,
	IN ULONG			TxErrorRatio);

void MlmeRestoreLastRate(
	IN struct _MAC_TABLE_ENTRY *pEntry);

void MlmeCheckRDG(
	IN struct rtmp_adapter *pAd,
	IN struct _MAC_TABLE_ENTRY *pEntry);

void RTMPSetSupportMCS(
	IN struct rtmp_adapter *pAd,
	IN u8 OpMode,
	IN struct _MAC_TABLE_ENTRY *pEntry,
	IN u8 SupRate[],
	IN u8 SupRateLen,
	IN u8 ExtRate[],
	IN u8 ExtRateLen,
	IN u8 vht_cap_len,
	IN VHT_CAP_IE *vht_cap,
	IN HT_CAPABILITY_IE *pHtCapability,
	IN u8 HtCapabilityLen);

#ifdef NEW_RATE_ADAPT_SUPPORT
void MlmeSetMcsGroup(struct rtmp_adapter *pAd, struct _MAC_TABLE_ENTRY *pEnt);

u8 MlmeSelectUpRate(
	IN struct rtmp_adapter *pAd,
	IN struct _MAC_TABLE_ENTRY *pEntry,
	IN RTMP_RA_GRP_TB *pCurrTxRate);

u8 MlmeSelectDownRate(
	IN struct rtmp_adapter *pAd,
	IN struct _MAC_TABLE_ENTRY *pEntry,
	IN u8 		CurrRateIdx);

void MlmeGetSupportedMcsAdapt(
	IN struct rtmp_adapter *pAd,
	IN struct _MAC_TABLE_ENTRY *pEntry,
	IN u8 mcs23GI,
	OUT CHAR 	mcs[]);

u8 MlmeSelectTxRateAdapt(
	IN struct rtmp_adapter *pAd,
	IN struct _MAC_TABLE_ENTRY *pEntry,
	IN CHAR		mcs[],
	IN CHAR		Rssi,
	IN CHAR		RssiOffset);

bool MlmeRAHybridRule(
	IN struct rtmp_adapter *pAd,
	IN struct _MAC_TABLE_ENTRY *pEntry,
	IN RTMP_RA_GRP_TB *pCurrTxRate,
	IN ULONG			NewTxOkCount,
	IN ULONG			TxErrorRatio);

void MlmeNewRateAdapt(
	IN struct rtmp_adapter *pAd,
	IN struct _MAC_TABLE_ENTRY *pEntry,
	IN u8 		UpRateIdx,
	IN u8 		DownRateIdx,
	IN ULONG			TrainUp,
	IN ULONG			TrainDown,
	IN ULONG			TxErrorRatio);

INT	Set_PerThrdAdj_Proc(
	IN struct rtmp_adapter *pAd,
	IN char *arg);

INT	Set_LowTrafficThrd_Proc(
	IN struct rtmp_adapter *pAd,
	IN char *		arg);

INT	Set_TrainUpRule_Proc(
	IN struct rtmp_adapter *pAd,
	IN char *		arg);

INT	Set_TrainUpRuleRSSI_Proc(
	IN struct rtmp_adapter *pAd,
	IN char *		arg);

INT	Set_TrainUpLowThrd_Proc(
	IN struct rtmp_adapter *pAd,
	IN char *		arg);

INT	Set_TrainUpHighThrd_Proc(
	IN struct rtmp_adapter *pAd,
	IN char *		arg);

INT Set_RateTable_Proc(
	IN  struct rtmp_adapter *pAd,
	IN  char *arg);


#ifdef CONFIG_STA_SUPPORT
void StaQuickResponeForRateUpExecAdapt(
	IN struct rtmp_adapter *pAd,
	IN ULONG i,
	IN CHAR Rssi);

void MlmeDynamicTxRateSwitchingAdapt(
	IN struct rtmp_adapter *pAd,
	IN ULONG i,
	IN ULONG TxSuccess,
	IN ULONG TxRetransmit,
	IN ULONG TxFailCount);
#endif /* CONFIG_STA_SUPPORT */
#endif /* NEW_RATE_ADAPT_SUPPORT */


#ifdef CONFIG_STA_SUPPORT
void MlmeDynamicTxRateSwitching(
	IN struct rtmp_adapter *pAd);

void StaQuickResponeForRateUpExec(
	IN void *SystemSpecific1,
	IN void *FunctionContext,
	IN void *SystemSpecific2,
	IN void *SystemSpecific3);

void MlmeSetTxRate(
	IN struct rtmp_adapter *pAd,
	IN struct _MAC_TABLE_ENTRY *pEntry,
	IN RTMP_RA_LEGACY_TB *pTxRate);
#endif /* CONFIG_STA_SUPPORT */

void MlmeRAInit(struct rtmp_adapter *pAd, struct _MAC_TABLE_ENTRY *pEntry);
void MlmeNewTxRate(struct rtmp_adapter *pAd, struct _MAC_TABLE_ENTRY *pEntry);

void MlmeRALog(
	IN struct rtmp_adapter *pAd,
	IN struct _MAC_TABLE_ENTRY *pEntry,
	IN RA_LOG_TYPE raLogType,
	IN ULONG TxErrorRatio,
	IN ULONG TxTotalCnt);

void MlmeSelectTxRateTable(
	IN struct rtmp_adapter *pAd,
	IN struct _MAC_TABLE_ENTRY *pEntry,
	IN u8 **ppTable,
	IN u8 *pTableSize,
	IN u8 *pInitTxRateIdx);

#endif /* __DRS_EXTR_H__ */

/* End of drs_extr.h */
