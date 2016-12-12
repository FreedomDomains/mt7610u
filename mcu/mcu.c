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

#ifdef RTMP_USB_SUPPORT
	pChipOps->MCU_loadFirmware = andes_usb_loadfw;
#endif
	pChipOps->MCU_CtrlInit = andes_ctrl_init;
	pChipOps->MCU_CtrlExit = andes_ctrl_exit;
	pChipOps->MCU_Calibration = andes_calibration;
	pChipOps->MCU_BurstWrite =  andes_burst_write;
	pChipOps->MCU_BurstRead = andes_burst_read;
	pChipOps->MCU_RandomRead = andes_random_read;
	pChipOps->MCU_RFRandomRead = andes_rf_random_read;
	pChipOps->MCU_ReadModifyWrite = andes_read_modify_write;
	pChipOps->MCU_RandomWrite = andes_random_write;
	pChipOps->MCU_RFRandomWrite = andes_rf_random_write;
	pChipOps->MCU_PwrSavingOP = andes_pwr_saving;
}
