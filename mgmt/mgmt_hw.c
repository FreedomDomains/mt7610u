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


#include <rt_config.h>


INT dev_adjust_radio(struct rtmp_adapter*pAd)
{
	struct hw_setting *hw_cfg = &pAd->hw_cfg, new_cfg;


	memset(&new_cfg, 0, sizeof(struct hw_setting));


	/* For all wdev, find the maximum inter-set */


	if (hw_cfg->bbp_bw != new_cfg.bbp_bw)
	{
		mt7610u_bbp_set_bw(pAd, new_cfg.bbp_bw);
		hw_cfg->bbp_bw = new_cfg.bbp_bw;
	}

	if (hw_cfg->cent_ch != new_cfg.cent_ch)
	{
		u8 ext_ch = EXTCHA_NONE;

		mt7610u_bbp_set_ctrlch(pAd, ext_ch);
		rtmp_mac_set_ctrlch(pAd, ext_ch);
	}

	return true;
}

