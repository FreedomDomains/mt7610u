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


#ifdef VIDEO_TURBINE_SUPPORT



bool UpdateFromGlobal = false;

void VideoTurbineUpdate(
	IN struct rtmp_adapter *pAd)
{
	if (UpdateFromGlobal == true)
	{
		pAd->VideoTurbine.Enable = GLOBAL_AP_VIDEO_CONFIG.Enable;
		pAd->VideoTurbine.ClassifierEnable = GLOBAL_AP_VIDEO_CONFIG.ClassifierEnable;
		pAd->VideoTurbine.HighTxMode = GLOBAL_AP_VIDEO_CONFIG.HighTxMode;
		pAd->VideoTurbine.TxPwr = GLOBAL_AP_VIDEO_CONFIG.TxPwr;
		pAd->VideoTurbine.VideoMCSEnable = GLOBAL_AP_VIDEO_CONFIG.VideoMCSEnable;
		pAd->VideoTurbine.VideoMCS = GLOBAL_AP_VIDEO_CONFIG.VideoMCS;
		pAd->VideoTurbine.TxBASize = GLOBAL_AP_VIDEO_CONFIG.TxBASize;
		pAd->VideoTurbine.TxLifeTimeMode = GLOBAL_AP_VIDEO_CONFIG.TxLifeTimeMode;
		pAd->VideoTurbine.TxLifeTime = GLOBAL_AP_VIDEO_CONFIG.TxLifeTime;
		pAd->VideoTurbine.TxRetryLimit = GLOBAL_AP_VIDEO_CONFIG.TxRetryLimit;
	}
}


void TxSwQDepthAdjust(IN struct rtmp_adapter*pAd, IN u32 qLen)
{
	ULONG IrqFlags;
	INT qIdx;
	QUEUE_HEADER *pTxQ, *pEntry;
	struct sk_buff * pPacket;

	spin_lock_bh(&pAd->irq_lock);
	pAd->TxSwQMaxLen = qLen;
	for (qIdx = 0; qIdx < NUM_OF_TX_RING; qIdx++)
	{
		pTxQ = &pAd->TxSwQueue[qIdx];
		while(pTxQ->Number >= pAd->TxSwQMaxLen)
		{
			pEntry = RemoveHeadQueue(pTxQ);
			if (pEntry)
			{
				pPacket = QUEUE_ENTRY_TO_PACKET(pEntry);
				dev_kfree_skb_any(pPacket);
			}
			else
				break;
		}
	}
	spin_unlock_bh(&pAd->irq_lock);

	DBGPRINT(RT_DEBUG_OFF, ("%s():Set TxSwQMaxLen as %d\n",
			__FUNCTION__, pAd->TxSwQMaxLen));
}


void VideoTurbineDynamicTune(
	IN struct rtmp_adapter *pAd)
{
	if (pAd->VideoTurbine.Enable == true)
	{
			u32 MacReg = 0;

		{
			/* Tx retry limit = 2F,1F */
			mt76u_reg_read(pAd, TX_RTY_CFG, &MacReg);
			MacReg &= 0xFFFF0000;
			MacReg |= GetAsicVideoRetry(pAd);
			mt76u_reg_write(pAd, TX_RTY_CFG, MacReg);
		}

		pAd->VideoTurbine.TxBASize = GetAsicVideoTxBA(pAd);

		Set_RateAdaptInterval(pAd, "100:50");
		TxSwQDepthAdjust(pAd, 1024);

	}
	else
	{
			u32 MacReg = 0;


		/* Default Tx retry limit = 1F,0F */
		mt76u_reg_read(pAd, TX_RTY_CFG, &MacReg);
		MacReg &= 0xFFFF0000;
			MacReg |= GetAsicDefaultRetry(pAd);
		mt76u_reg_write(pAd, TX_RTY_CFG, MacReg);

		pAd->VideoTurbine.TxBASize = GetAsicDefaultTxBA(pAd);

		/* reset to default rate adaptation simping interval */
		if ((pAd->ra_interval != DEF_RA_TIME_INTRVAL) ||
			(pAd->ra_fast_interval != DEF_QUICK_RA_TIME_INTERVAL))
			Set_RateAdaptInterval(pAd, "500:100");

		TxSwQDepthAdjust(pAd, MAX_PACKETS_IN_QUEUE);
	}
}

u32 GetAsicDefaultRetry(
	IN struct rtmp_adapter *pAd)
{
	u32 RetryLimit;

	RetryLimit = 0x1F0F;

	return RetryLimit;
}

u8 GetAsicDefaultTxBA(
	IN struct rtmp_adapter *pAd)
{
        return pAd->CommonCfg.TxBASize;
}

u32 GetAsicVideoRetry(
	IN struct rtmp_adapter *pAd)
{
	return pAd->VideoTurbine.TxRetryLimit;
}

u8 GetAsicVideoTxBA(
	IN struct rtmp_adapter *pAd)
{
	return pAd->VideoTurbine.TxBASize;
}

void VideoConfigInit(
	IN struct rtmp_adapter *pAd)
{
	pAd->VideoTurbine.Enable = false;
	pAd->VideoTurbine.TxRetryLimit = 0x2F1F;
	pAd->VideoTurbine.TxBASize = pAd->CommonCfg.TxBASize;
}

#endif /* VIDEO_TURBINE_SUPPORT */


