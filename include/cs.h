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


#ifndef __CS_H__
#define __CS_H__

#ifdef CARRIER_DETECTION_SUPPORT
#define CARRIER_DETECT_RECHECK_TIME			3
#define CARRIER_GONE_TRESHOLD				10 /* Radar tone count in 1 sec */
#define CARRIER_DETECT_THRESHOLD			0x0fffffff
#define CARRIER_DETECT_STOP_RATIO			2
#define CARRIER_DETECT_STOP_RECHECK_TIME		4
#define CARRIER_DETECT_DEFAULT_MASK			20
#define CARRIER_DETECT_DELTA				7
#define CARRIER_DETECT_DIV_FLAG				0
#define CD_INT_POLLING_CMD			0x67
#define CARRIER_DETECT_CRITIRIA				7000

typedef enum CD_STATE_n {
	CD_NORMAL,
	CD_SILENCE,
	CD_MAX_STATE
} CD_STATE;

typedef enum _TONE_RADAR_VERSION {
	DISABLE_TONE_RADAR = 0,
	TONE_RADAR_V1,
	TONE_RADAR_V2,
	TONE_RADAR_V3
} TONE_RADAR_VERSION;

typedef struct CARRIER_DETECTION_s {
	BOOLEAN Enable;
	u8 CDSessionTime;
	u8 CDPeriod;
	CD_STATE CD_State;
	u8 delta;
	u8 SymRund;
	u8 div_flag;
	UINT32 threshold;
	u8 recheck;
	u8 recheck1;
	UINT32 TimeStamp; /*unit:16us*/
	UINT32 criteria;
	ULONG idle_time;
	ULONG busy_time;
	ULONG Debug;
	ULONG OneSecIntCount;
	u8 CarrierGoneThreshold;
	u8 VGA_Mask;
	u8 Packet_End_Mask;
	u8 Rx_PE_Mask;
} CARRIER_DETECTION_STRUCT, *PCARRIER_DETECTION_STRUCT;

#ifdef CARRIER_DETECTION_FIRMWARE_SUPPORT
/* Mcu command */
#define CD_ONOFF_MCU_CMD			0x65
#define CD_CHECKOUT_MCU_CMD			0x66
/* share memory offsets */
#define CD_CRITERIA     			0x4CB2
#define CD_CHECK_COUNT  			0x4CB9
#define RADAR_TONE_COUNT			0x4CBE
#define CD_RECHECK      			0x4CBF
#undef CARRIER_DETECT_RECHECK_TIME
#undef CARRIER_GONE_TRESHOLD
#undef CARRIER_DETECT_THRESHOLD
#define CARRIER_DETECT_RECHECK_TIME			5
#define CARRIER_GONE_TRESHOLD				35
#define CARRIER_DETECT_THRESHOLD			0x4fffffff

/* Parameters needed to decide the Carrier Detect State */
typedef struct _CARRIER_DETECT_PARAM {
		u8	RadarToneCount; /* Number of radar tones in 100 msec*/
		u8	ReCheck;
} CARRIER_DETECT_PARAM, *PCARRIER_DETECT_PARAM;

/* For debug print */
typedef struct _CARRIER_DETECT_DEBUG {
	 	u8	delta_div;
		u8	internalRadarToneCount;
		UINT16	Criteria;
		UINT32	Threshold;
		u8	Count;
		u8	CheckCount;
		u8 	Reserved;
		u8	VGA_Mask;
		u8	PckMask;
		u8	RxPeMask;
		u8	RadarToneCount;
		u8	ReCheck;
} CARRIER_DETECT_DEBUG, *PCARRIER_DETECT_DEBUG;
#endif /*CARRIER_DETECTION_FIRMWARE_SUPPORT*/

INT isCarrierDetectExist(
	IN struct rtmp_adapter *pAd);

INT CarrierDetectReset(
	IN struct rtmp_adapter *pAd);

extern void RtmpOsMsDelay(
	IN	ULONG	msec);

INT Set_CarrierCriteria_Proc(
	IN struct rtmp_adapter *	pAd,
	IN char *			arg);

INT Set_CarrierReCheck_Proc(
	IN struct rtmp_adapter *pAd,
	IN char *			arg);

INT Set_CarrierGoneThreshold_Proc(
	IN struct rtmp_adapter *pAd,
	IN char *		arg);

INT Set_CarrierStopCheck_Proc(
	IN struct rtmp_adapter *	pAd,
	IN char *			arg);

INT	Set_CarrierDebug_Proc(
	IN struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_CarrierDelta_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_CarrierDivFlag_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT	Set_CarrierThrd_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

/* v2 functions */
INT	Set_CarrierSymRund_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

INT Set_CarrierMask_Proc(
    IN struct rtmp_adapter *  pAd,
    IN char *        arg);

void CSInit(
	IN struct rtmp_adapter *pAd);

void CarrierDetectionStart(
	IN struct rtmp_adapter *pAd);

void RTMPHandleRadarInterrupt(
	IN struct rtmp_adapter * pAd);

void CarrierDetectionStop(
	IN struct rtmp_adapter *pAd);

void ToneRadarProgram_v1(
	IN struct rtmp_adapter *pAd,
	IN ULONG threshold);

void ToneRadarProgram_v2(
	IN struct rtmp_adapter *pAd,
	IN ULONG threshold);

void ToneRadarProgram_v3(
	IN struct rtmp_adapter *pAd,
	IN ULONG threshold);

#ifdef CARRIER_DETECTION_FIRMWARE_SUPPORT
void CarrierDetectionPeriodicStateCtrl(
	IN struct rtmp_adapter *pAd);
#endif /* CARRIER_DETECTION_FIRMWARE_SUPPORT */

#endif /* CARRIER_DETECTION_SUPPORT */

#endif /*__CS_H__*/
