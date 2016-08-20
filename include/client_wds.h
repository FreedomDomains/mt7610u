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


#ifndef __CLIENT_WDS_H__
#define __CLIENT_WDS_H__

#include "client_wds_cmm.h"

void CliWds_ProxyTabInit(
	IN struct rtmp_adapter *pAd);

void CliWds_ProxyTabDestory(
	IN struct rtmp_adapter *pAd);

PCLIWDS_PROXY_ENTRY CliWdsEntyAlloc(
	IN struct rtmp_adapter *pAd);


void CliWdsEntyFree(
	IN struct rtmp_adapter *pAd,
	IN PCLIWDS_PROXY_ENTRY pCliWdsEntry);


u8 *CliWds_ProxyLookup(
	IN struct rtmp_adapter *pAd,
	IN u8 *pMac);


void CliWds_ProxyTabUpdate(
	IN struct rtmp_adapter *pAd,
	IN SHORT Aid,
	IN u8 *pMac);


void CliWds_ProxyTabMaintain(
	IN struct rtmp_adapter *pAd);

#endif /* __CLIENT_WDS_H__ */

