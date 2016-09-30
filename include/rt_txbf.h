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


#ifndef _RT_TXBF_H_
#define _RT_TXBF_H_

#ifdef TXBF_SUPPORT

//#define MRQ_FORCE_TX		//Force MRQ regardless the capability of the station


// TxSndgPkt Sounding type definitions
#define SNDG_TYPE_DISABLE		0
#define SNDG_TYPE_SOUNDING	1
#define SNDG_TYPE_NDP			2

// Explicit TxBF feedback mechanism
#define ETXBF_FB_DISABLE	0
#define ETXBF_FB_CSI		1
#define ETXBF_FB_NONCOMP	2
#define ETXBF_FB_COMP		4


//#define MRQ_FORCE_TX		//Force MRQ regardless the capability of the station

/*
	eTxBfEnCond values:
	 0:no etxbf,
	 1:etxbf update periodically,
	 2:etxbf updated if mcs changes in RateSwitchingAdapt() or APQuickResponeForRateUpExecAdapt().
	 3:auto-selection: if mfb changes or timer expires, then send sounding packets <------not finished yet!!!
	 note:
		when = 1 or 3, NO_SNDG_CNT_THRD controls the frequency to update the
		matrix(ETXBF_EN_COND=1) or activate the whole bf evaluation process(not defined)
*/

// Defines to include optional code.
//	NOTE: Do not define these options. ETxBfEnCond==3 and
//		MCS Feedback are not fully implemented
//#define ETXBF_EN_COND3_SUPPORT	// Include ETxBfEnCond==3 code
//#define MFB_SUPPORT				// Include MCS Feedback code

// MCS FB definitions
#define MSI_TOGGLE_BF		6
#define TOGGLE_BF_PKTS		5// the number of packets with inverted BF status

// TXBF State definitions
#define READY_FOR_SNDG0		0//jump to WAIT_SNDG_FB0 when channel change or periodically
#define WAIT_SNDG_FB0		1//jump to WAIT_SNDG_FB1 when bf report0 is received
#define WAIT_SNDG_FB1		2
#define WAIT_MFB			3
#define WAIT_USELESS_RSP	4
#define WAIT_BEST_SNDG		5

#define NO_SNDG_CNT_THRD	0//send sndg packet if there is no sounding for (NO_SNDG_CNT_THRD+1)*500msec. If this =0, bf matrix is updated at each call of APMlmeDynamicTxRateSwitchingAdapt()


// ------------ BEAMFORMING PROFILE HANDLING ------------

#define IMP_MAX_BYTES		14		// Implicit: 14 bytes per subcarrier
#define IMP_MAX_BYTES_ONE_COL	7	// Implicit: 7 bytes per subcarrier, when reading first column
#define EXP_MAX_BYTES		18		// Explicit: 18 bytes per subcarrier
#define IMP_COEFF_SIZE		 9		// 9 bits/coeff
#define IMP_COEFF_MASK		0x1FF

#define PROFILE_MAX_CARRIERS_20		56		// Number of subcarriers in 20 MHz mode
#define PROFILE_MAX_CARRIERS_40		114		// Number of subcarriers in 40 MHz mode

// Indices of valid rows in Implicit and Explicit profiles for 20 and 40 MHz
typedef struct {
	int lwb1, upb1;
	int lwb2, upb2;
} SC_TABLE_ENTRY;


typedef struct {
	BOOLEAN impProfile;
	BOOLEAN fortyMHz;
	int rows, columns;
	int grouping;
	u8 tag[EXP_MAX_BYTES];
	u8 data[PROFILE_MAX_CARRIERS_40][EXP_MAX_BYTES];
} PROFILE_DATA;

extern PROFILE_DATA profData;


typedef
struct {
	u8 gBeg[2];
	u8 gEnd[2];
	u8 aLowBeg[2];
	u8 aLowEnd[2];
	u8 aMidBeg[2];
	u8 aMidEnd[2];
	u8 aHighBeg[2];
	u8 aHighEnd[2];
} ITXBF_PHASE_PARAMS;			// ITxBF BBP reg phase calibration parameters

typedef
struct {
	u8 gBeg[2];
	u8 gEnd[2];
	u8 aLowBeg[2];
	u8 aLowEnd[2];
	u8 aMidBeg[2];
	u8 aMidEnd[2];
	u8 aHighBeg[2];
	u8 aHighEnd[2];
} ITXBF_LNA_PARAMS;			// ITxBF BBP reg LNA calibration parameters

typedef
struct {
	u8 gBeg[2];
	u8 gEnd[2];
	u8 aLow[2];
	u8 aMid[2];
	u8 aHigh[2];
} ITXBF_DIV_PARAMS;				// ITxBF Divider Calibration parameters

void ITxBFGetEEPROM(
	IN struct rtmp_adapter*pAd,
	IN ITXBF_PHASE_PARAMS *phaseParams,
	IN ITXBF_LNA_PARAMS *lnaParams,
	IN ITXBF_DIV_PARAMS *divParams);

INT ITxBFDividerCalibration(
	IN struct rtmp_adapter*pAd,
	IN int calFunction,
	IN int calMethod,
	OUT u8 *divPhase);

void ITxBFLoadLNAComp(
	IN struct rtmp_adapter*pAd);

int ITxBFLNACalibration(
	IN struct rtmp_adapter*pAd,
	IN int calFunction,
	IN int calMethod,
	IN BOOLEAN gBand);

void Read_TxBfProfile(
	IN	struct rtmp_adapter*pAd,
	IN	PROFILE_DATA	*prof,
	IN	int				profileNum,
	IN	BOOLEAN			implicitProfile);

void Write_TxBfProfile(
	IN	struct rtmp_adapter*pAd,
	IN	PROFILE_DATA	*prof,
	IN	int				profileNum);

void Read_TagField(
	IN	struct rtmp_adapter *pAd,
	IN  u8 *row,
	IN  int		profileNum);

// Write_TagField - write a profile tagfield
void Write_TagField(
	IN	struct rtmp_adapter*pAd,
	IN  u8 *row,
	IN  int		profileNum);

// displayTagfield - display one tagfield
void displayTagfield(
	IN	struct rtmp_adapter*pAd,
	IN	int		profileNum,
	IN	BOOLEAN implicitProfile);

// Unpack an ITxBF matrix element from a row of bytes
int Unpack_IBFValue(
	IN u8 *row,
	IN int elemNum);

int iCalcCalibration(
	IN struct rtmp_adapter*pAd,
	IN int calParams[2],
	IN int profileNum);

void ITxBFSetEEPROM(
	IN struct rtmp_adapter*pAd,
	IN ITXBF_PHASE_PARAMS *phaseParams,
	IN ITXBF_LNA_PARAMS *lnaParams,
	IN ITXBF_DIV_PARAMS *divParams);

#endif // TXBF_SUPPORT //

#endif // _RT_TXBF_H_
