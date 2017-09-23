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


#include "rt_config.h"


INT get_pkt_phymode_by_rxwi(struct mt7610u_rxwi *rxwi)
{
	return rxwi->phy_mode;
}

INT get_pkt_rssi_by_rxwi(struct mt7610u_rxwi *rxwi, INT size, CHAR *rssi)
{
	if (size < sizeof(rxwi->rssi)/ sizeof(u8))
		memmove(rssi, &rxwi->rssi[0], size);

	return 0;
}


INT get_pkt_snr_by_rxwi(struct mt7610u_rxwi *rxwi, INT size, u8 *snr)
{
	// TODO: shiang-6590, fix me for SNR info of RXWI!!
	if (size < 3)
		memmove(snr, &rxwi->bbp_rxinfo[0], size);

	return 0;
}

