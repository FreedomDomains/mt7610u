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


#ifndef	__WFD_H__
#define	__WFD_H__

#ifdef WFD_SUPPORT

#include "rtmp_type.h"

INT Set_WfdEnable_Proc(
    IN  struct rtmp_adapter *	pAd,
    IN  char *		arg);

#ifdef RT_CFG80211_SUPPORT
INT Set_WfdInsertIe_Proc
(
	IN	struct rtmp_adapter *	pAd,
	IN	char *		arg);
#endif /* RT_CFG80211_SUPPORT */

INT Set_WfdDeviceType_Proc(
    IN  struct rtmp_adapter *	pAd,
    IN  char *		arg);

INT Set_WfdCouple_Proc(
    IN  struct rtmp_adapter *	pAd,
    IN  char *		arg);

INT Set_WfdSessionAvailable_Proc(
    IN  struct rtmp_adapter *	pAd,
    IN  char *		arg);

INT Set_WfdCP_Proc(
    IN  struct rtmp_adapter *	pAd,
    IN  char *		arg);

INT	Set_WfdRtspPort_Proc(
    IN  struct rtmp_adapter *	pAd,
    IN  char *		arg);

INT	Set_WfdMaxThroughput_Proc(
    IN  struct rtmp_adapter *	pAd,
    IN  char *		arg);

INT Set_WfdLocalIp_Proc(
    IN  struct rtmp_adapter *	pAd,
    IN  char *		arg);

INT Set_PeerRtspPort_Proc(
    IN  struct rtmp_adapter *	pAd,
    IN  char *		arg);

void WfdMakeWfdIE(
	IN	struct rtmp_adapter *pAd,
	IN 	ULONG			WfdIeBitmap,
	OUT	PUCHAR			pOutBuf,
	OUT	PULONG			pIeLen);

ULONG InsertWfdSubelmtTlv(
	IN struct rtmp_adapter *	pAd,
	IN UCHAR			SubId,
	IN PUCHAR			pInBuffer,
	IN PUCHAR			pOutBuffer,
	IN UINT				Action);

void WfdParseSubElmt(
	IN struct rtmp_adapter *	pAd,
	IN PWFD_ENTRY_INFO	pWfdEntryInfo,
	IN void 				*Msg,
	IN ULONG 			MsgLen);

void WfdCfgInit(
	IN struct rtmp_adapter *pAd);

#endif /* WFD_SUPPORT */
#endif /* __WFD_H__ */

