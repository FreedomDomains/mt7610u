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


#ifndef __RADAR_H__
#define __RADAR_H__

/* RESTRICTION_BAND_1: 5600MHz ~ 5650MHz */
#define RESTRICTION_BAND_1(_pAd)												\
	_pAd->CommonCfg.RegTransmitSetting.field.BW == BW_40 ? 						\
	((_pAd->CommonCfg.Channel >= 116) && (_pAd->CommonCfg.Channel <= 128)) :	\
	((_pAd->CommonCfg.Channel >= 120) && (_pAd->CommonCfg.Channel <= 128))

/* 802.11H */
typedef struct _DOT11_H {
	/* 802.11H and DFS related params */
	u8 CSCount;		/*Channel switch counter */
	u8 CSPeriod; 	/*Channel switch period (beacon count) */
	USHORT RDCount; 	/*Radar detection counter, if RDCount >  ChMovingTime, start to send beacons*/
	u8 RDMode;		/*Radar Detection mode */
	USHORT ChMovingTime;
	bool bDFSIndoor;
	ULONG InServiceMonitorCount;	/* unit: sec */
} DOT11_H, *PDOT11_H;

bool RadarChannelCheck(
	IN struct rtmp_adapter *pAd,
	IN u8 		Ch);

ULONG JapRadarType(
	IN struct rtmp_adapter *pAd);


void RadarDetectPeriodic(
	IN struct rtmp_adapter *pAd);

INT	Set_CSPeriod_Proc(
	IN	struct rtmp_adapter *pAdapter,
	IN	char *		arg);

INT Set_ChMovingTime_Proc(
	IN struct rtmp_adapter *pAd,
	IN char *arg);

INT Set_BlockChReset_Proc(
	IN struct rtmp_adapter *pAd,
	IN char *arg);

#if defined(DFS_SUPPORT)
INT	Set_RadarShow_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg);

void CckMrcStatusCtrl(
	IN struct rtmp_adapter *pAd);

void RadarGLRTCompensate(
	IN struct rtmp_adapter *pAd);

#endif /*defined(DFS_SUPPORT) */

#endif /* __RADAR_H__ */
