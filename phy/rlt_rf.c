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


#ifdef RLT_RF

#include "rt_config.h"

int rlt_rf_write(
	IN struct rtmp_adapter *pAd,
	IN u8 bank,
	IN u8 regID,
	IN u8 value)
{
	int	 ret = 0;

	{
		struct mt7610u_rf_entry  reg;
		reg.Bank = bank;
		reg.Register = regID;
		reg.Value = value;

		mt7610u_mcu_rf_random_write(pAd, &reg, 1);
	} 

	return ret;
}

/*
	========================================================================

	Routine Description: Read RT30xx RF register through MAC

	Arguments:

	Return Value:

	IRQL =

	Note:

	========================================================================
*/
int rlt_rf_read(
	IN struct rtmp_adapter*pAd,
	IN u8 bank,
	IN u8 regID,
	IN u8 *pValue)
{
	int	 ret = 0;
	{
		struct mt7610u_rf_entry  reg;
		reg.Bank = bank;
		reg.Register = regID;
		mt7610u_mcu_rf_random_read(pAd, &reg, 1);

		*pValue = reg.Value;
	}
	return ret;
}
#endif /* RLT_RF */

