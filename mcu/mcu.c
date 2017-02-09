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


#include	"rt_config.h"

void ChipOpsMCUHook(struct rtmp_adapter *pAd)
{

	struct rtmp_chip_ops  *pChipOps = &pAd->chipOps;

	pChipOps->MCU_CtrlInit = mt7610u_mcu_ctrl_init;
	pChipOps->MCU_CtrlExit = mt7610u_mcu_ctrl_exit;
	pChipOps->MCU_Calibration = mt7610u_mcu_calibration;
	pChipOps->MCU_BurstWrite =  mt7610u_mcu_burst_write;
	pChipOps->MCU_RFRandomRead = mt7610u_mcu_rf_random_read;
	pChipOps->MCU_ReadModifyWrite = mt7610u_mcu_read_modify_write;
	pChipOps->MCU_RandomWrite = mt7610u_mcu_random_write;
	pChipOps->MCU_RFRandomWrite = mt7610u_mcu_rf_random_write;
}
