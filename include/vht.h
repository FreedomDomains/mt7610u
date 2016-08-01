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


#include "dot11ac_vht.h"


struct rtmp_adapter;
struct _RT_PHY_INFO;


void dump_vht_cap(struct rtmp_adapter *pAd, VHT_CAP_IE *vht_ie);
void dump_vht_op(struct rtmp_adapter *pAd, VHT_OP_IE *vht_ie);

INT build_vht_ies(struct rtmp_adapter *pAd, UCHAR *buf, UCHAR frm);
INT build_vht_cap_ie(struct rtmp_adapter*pAd, UCHAR *buf);

UCHAR vht_prim_ch_idx(UCHAR vht_cent_ch, UCHAR prim_ch);
UCHAR vht_cent_ch_freq(struct rtmp_adapter *pAd, UCHAR prim_ch);
INT vht_mode_adjust(struct rtmp_adapter *pAd, MAC_TABLE_ENTRY *pEntry, VHT_CAP_IE *cap, VHT_OP_IE *op);
INT SetCommonVHT(struct rtmp_adapter *pAd);
void rtmp_set_vht(struct rtmp_adapter *pAd, struct _RT_PHY_INFO *phy_info);

void assoc_vht_info_debugshow(
	IN struct rtmp_adapter*pAd,
	IN MAC_TABLE_ENTRY *pEntry,
	IN VHT_CAP_IE *vht_cap,
	IN VHT_OP_IE *vht_op);

