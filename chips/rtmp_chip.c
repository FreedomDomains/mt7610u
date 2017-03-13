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


/*
========================================================================
Routine Description:
	Initialize specific beacon frame architecture.

Arguments:
	pAd				- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
void rlt_bcn_buf_init(struct rtmp_adapter*pAd)
{
	struct rtmp_chip_cap *pChipCap = &pAd->chipCap;

	pChipCap->FlgIsSupSpecBcnBuf = false;
	pChipCap->BcnMaxHwNum = 16;
	pChipCap->WcidHwRsvNum = 255;

/*
	In 16-MBSS support mode, if AP-Client is enabled,
	the last 8-MBSS would be occupied for AP-Client using.
*/
#ifdef APCLI_SUPPORT
	pChipCap->BcnMaxNum = (8 - MAX_MESH_NUM);
#else
	pChipCap->BcnMaxNum = (pChipCap->BcnMaxHwNum - MAX_MESH_NUM);
#endif /* APCLI_SUPPORT */

	pChipCap->BcnMaxHwSize = 0x2000;

	pChipCap->BcnBase[0] = 0xc000;
	pChipCap->BcnBase[1] = 0xc200;
	pChipCap->BcnBase[2] = 0xc400;
	pChipCap->BcnBase[3] = 0xc600;
	pChipCap->BcnBase[4] = 0xc800;
	pChipCap->BcnBase[5] = 0xca00;
	pChipCap->BcnBase[6] = 0xcc00;
	pChipCap->BcnBase[7] = 0xce00;
	pChipCap->BcnBase[8] = 0xd000;
	pChipCap->BcnBase[9] = 0xd200;
	pChipCap->BcnBase[10] = 0xd400;
	pChipCap->BcnBase[11] = 0xd600;
	pChipCap->BcnBase[12] = 0xd800;
	pChipCap->BcnBase[13] = 0xda00;
	pChipCap->BcnBase[14] = 0xdc00;
	pChipCap->BcnBase[15] = 0xde00;

	DBGPRINT(RT_DEBUG_TRACE, ("< Beacon Spec Information: >\n"));
	DBGPRINT(RT_DEBUG_TRACE, ("\tBcnMaxHwNum = \t%d\n", pChipCap->BcnMaxHwNum));
	DBGPRINT(RT_DEBUG_TRACE, ("\tBcnMaxNum = \t%d\n", pChipCap->BcnMaxNum));
	DBGPRINT(RT_DEBUG_TRACE, ("\tBcnMaxHwSize = \t0x%x\n", pChipCap->BcnMaxHwSize));
	DBGPRINT(RT_DEBUG_TRACE, ("\tWcidHwRsvNum = \t%d\n", pChipCap->WcidHwRsvNum));
	DBGPRINT(RT_DEBUG_TRACE, ("\tBcnBase[0] = \t0x%x\n", pChipCap->BcnBase[0]));
	DBGPRINT(RT_DEBUG_TRACE, ("\tBcnBase[8] = \t0x%x\n", pChipCap->BcnBase[8]));

}

INT WaitForAsicReady(
	IN struct rtmp_adapter*pAd)
{
	u32 mac_val = 0, reg = MAC_CSR0;
	int idx = 0;

	do
	{
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
			return false;

		mac_val = mt7610u_read32(pAd, reg);
		if ((mac_val != 0x00) && (mac_val != 0xFFFFFFFF))
			return true;

		mdelay(5);
	} while (idx++ < 500);

	DBGPRINT(RT_DEBUG_ERROR,
				("%s(0x%x):AsicNotReady!\n",
				__FUNCTION__, mac_val));


	return false;
}

/*
========================================================================
Routine Description:
	Initialize chip related information.

Arguments:
	pCB				- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
void RtmpChipOpsHook(struct rtmp_adapter*pAd)
{
	/* sanity check */
	if (WaitForAsicReady(pAd) == false)
		return;

	pAd->MACVersion = mt7610u_read32(pAd, MAC_CSR0);

	MT76x0_Init(pAd);
}
