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



#ifdef CONFIG_STA_SUPPORT
void AsicUpdateAutoFallBackTable(
	IN	struct rtmp_adapter *pAd,
	IN	u8 *		pRateTable)
{
	u8 				i;
	HT_FBK_CFG0_STRUC		HtCfg0;
	HT_FBK_CFG1_STRUC		HtCfg1;
	LG_FBK_CFG0_STRUC		LgCfg0;
	LG_FBK_CFG1_STRUC		LgCfg1;
#ifdef DOT11N_SS3_SUPPORT
	TX_FBK_CFG_3S_0_STRUC	Ht3SSCfg0;
	TX_FBK_CFG_3S_1_STRUC	Ht3SSCfg1;
#endif /* DOT11N_SS3_SUPPORT */
	RTMP_RA_LEGACY_TB *pCurrTxRate, *pNextTxRate;

#ifdef AGS_SUPPORT
	RTMP_RA_AGS_TB *pCurrTxRate_AGS, *pNextTxRate_AGS;
	bool 				bUseAGS = false;

	if (AGS_IS_USING(pAd, pRateTable))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s: Use AGS\n", __FUNCTION__));

		bUseAGS = true;

		Ht3SSCfg0.word = 0x1211100f;
		Ht3SSCfg1.word = 0x16151413;
	}
#endif /* AGS_SUPPORT */

#ifdef DOT11N_SS3_SUPPORT
	if (IS_RT3883(pAd))
	{
		Ht3SSCfg0.word = 0x12111008;
		Ht3SSCfg1.word = 0x16151413;
	}
#endif /* DOT11N_SS3_SUPPORT */

	/* set to initial value*/
	HtCfg0.word = 0x65432100;
	HtCfg1.word = 0xedcba980;
	LgCfg0.word = 0xedcba988;
	LgCfg1.word = 0x00002100;

#if	defined(MT76x0) || defined(MT76x2)
	/*
		0x1360
		[23:20] - Auto fall back MCS as VHT 1SS MCS=9
		[19:16] - Auto fall back MCS as VHT 1SS MCS=8
	*/
	if (IS_MT76x0(pAd) || IS_MT76x2(pAd))
		LgCfg1.word = 0x00872100;
#endif /* MT76x0 */

#ifdef NEW_RATE_ADAPT_SUPPORT
	/* Use standard fallback if using new rate table */
	if (ADAPT_RATE_TABLE(pRateTable))
		goto skipUpdate;
#endif /* NEW_RATE_ADAPT_SUPPORT */

#ifdef AGS_SUPPORT
	if (bUseAGS)
	{
		pNextTxRate_AGS = (RTMP_RA_AGS_TB *)pRateTable+1;
		pNextTxRate = (RTMP_RA_LEGACY_TB *)pNextTxRate_AGS;
	}
	else
#endif /* AGS_SUPPORT */
		pNextTxRate = (RTMP_RA_LEGACY_TB *)pRateTable+1;

	for (i = 1; i < *((u8 *) pRateTable); i++)
	{
#ifdef AGS_SUPPORT
		if (bUseAGS)
		{
			pCurrTxRate_AGS = (RTMP_RA_AGS_TB *)pRateTable+1+i;
			pCurrTxRate = (RTMP_RA_LEGACY_TB *)pCurrTxRate_AGS;
		}
		else
#endif /* AGS_SUPPORT */
			pCurrTxRate = (RTMP_RA_LEGACY_TB *)pRateTable+1+i;

		switch (pCurrTxRate->Mode)
		{
			case 0:		/* CCK */
				break;
			case 1:		/* OFDM */
				{
					switch(pCurrTxRate->CurrMCS)
					{
						case 0:
							LgCfg0.field.OFDMMCS0FBK = (pNextTxRate->Mode == MODE_OFDM) ? (pNextTxRate->CurrMCS+8): pNextTxRate->CurrMCS;
							break;
						case 1:
							LgCfg0.field.OFDMMCS1FBK = (pNextTxRate->Mode == MODE_OFDM) ? (pNextTxRate->CurrMCS+8): pNextTxRate->CurrMCS;
							break;
						case 2:
							LgCfg0.field.OFDMMCS2FBK = (pNextTxRate->Mode == MODE_OFDM) ? (pNextTxRate->CurrMCS+8): pNextTxRate->CurrMCS;
							break;
						case 3:
							LgCfg0.field.OFDMMCS3FBK = (pNextTxRate->Mode == MODE_OFDM) ? (pNextTxRate->CurrMCS+8): pNextTxRate->CurrMCS;
							break;
						case 4:
							LgCfg0.field.OFDMMCS4FBK = (pNextTxRate->Mode == MODE_OFDM) ? (pNextTxRate->CurrMCS+8): pNextTxRate->CurrMCS;
							break;
						case 5:
							LgCfg0.field.OFDMMCS5FBK = (pNextTxRate->Mode == MODE_OFDM) ? (pNextTxRate->CurrMCS+8): pNextTxRate->CurrMCS;
							break;
						case 6:
							LgCfg0.field.OFDMMCS6FBK = (pNextTxRate->Mode == MODE_OFDM) ? (pNextTxRate->CurrMCS+8): pNextTxRate->CurrMCS;
							break;
						case 7:
							LgCfg0.field.OFDMMCS7FBK = (pNextTxRate->Mode == MODE_OFDM) ? (pNextTxRate->CurrMCS+8): pNextTxRate->CurrMCS;
							break;
					}
				}
				break;
#ifdef DOT11_N_SUPPORT
			case 2:		/* HT-MIX */
			case 3:		/* HT-GF */
				{
					if ((pNextTxRate->Mode >= MODE_HTMIX) && (pCurrTxRate->CurrMCS != pNextTxRate->CurrMCS))
					{
						if (pCurrTxRate->CurrMCS <= 15)
						{
							switch(pCurrTxRate->CurrMCS)
							{
								case 0:
									HtCfg0.field.HTMCS0FBK = pNextTxRate->CurrMCS;
									break;
								case 1:
									HtCfg0.field.HTMCS1FBK = pNextTxRate->CurrMCS;
									break;
								case 2:
									HtCfg0.field.HTMCS2FBK = pNextTxRate->CurrMCS;
									break;
								case 3:
									HtCfg0.field.HTMCS3FBK = pNextTxRate->CurrMCS;
									break;
								case 4:
									HtCfg0.field.HTMCS4FBK = pNextTxRate->CurrMCS;
									break;
								case 5:
									HtCfg0.field.HTMCS5FBK = pNextTxRate->CurrMCS;
									break;
								case 6:
									HtCfg0.field.HTMCS6FBK = pNextTxRate->CurrMCS;
									break;
								case 7:
									HtCfg0.field.HTMCS7FBK = pNextTxRate->CurrMCS;
									break;
								case 8:
									HtCfg1.field.HTMCS8FBK = 0;//pNextTxRate->CurrMCS;
									break;
								case 9:
									HtCfg1.field.HTMCS9FBK = pNextTxRate->CurrMCS;
									break;
								case 10:
									HtCfg1.field.HTMCS10FBK = pNextTxRate->CurrMCS;
									break;
								case 11:
									HtCfg1.field.HTMCS11FBK = pNextTxRate->CurrMCS;
									break;
								case 12:
									HtCfg1.field.HTMCS12FBK = pNextTxRate->CurrMCS;
									break;
								case 13:
									HtCfg1.field.HTMCS13FBK = pNextTxRate->CurrMCS;
									break;
								case 14:
									HtCfg1.field.HTMCS14FBK = pNextTxRate->CurrMCS;
									break;
								case 15:
									HtCfg1.field.HTMCS15FBK = pNextTxRate->CurrMCS;
									break;
							}
						}
						else
#ifdef AGS_SUPPORT
						if ((bUseAGS == true) &&
							(pCurrTxRate->CurrMCS >= 16) && (pCurrTxRate->CurrMCS <= 23))
						{
							switch(pCurrTxRate->CurrMCS)
							{
								case 16:
									Ht3SSCfg0.field.HTMCS16FBK = pNextTxRate->CurrMCS;
									break;
								case 17:
									Ht3SSCfg0.field.HTMCS17FBK = pNextTxRate->CurrMCS;
									break;
								case 18:
									Ht3SSCfg0.field.HTMCS18FBK = pNextTxRate->CurrMCS;
									break;
								case 19:
									Ht3SSCfg0.field.HTMCS19FBK = pNextTxRate->CurrMCS;
									break;
								case 20:
									Ht3SSCfg1.field.HTMCS20FBK = pNextTxRate->CurrMCS;
									break;
								case 21:
									Ht3SSCfg1.field.HTMCS21FBK = pNextTxRate->CurrMCS;
									break;
								case 22:
									Ht3SSCfg1.field.HTMCS22FBK = pNextTxRate->CurrMCS;
									break;
								case 23:
									Ht3SSCfg1.field.HTMCS23FBK = pNextTxRate->CurrMCS;
									break;
							}
						}
						else
#endif /* AGS_SUPPORT */
							DBGPRINT(RT_DEBUG_ERROR, ("AsicUpdateAutoFallBackTable: not support CurrMCS=%d\n", pCurrTxRate->CurrMCS));
					}
				}
				break;
#endif /* DOT11_N_SUPPORT */
		}

		pNextTxRate = pCurrTxRate;
	}

#ifdef AGS_SUPPORT
	if (bUseAGS == true)
	{
		Ht3SSCfg0.field.HTMCS16FBK = 0x8; // MCS 16 -> MCS 8
		HtCfg1.field.HTMCS8FBK = 0x0; // MCS 8 -> MCS 0

		LgCfg0.field.OFDMMCS2FBK = 0x3; // OFDM 12 -> CCK 11
		LgCfg0.field.OFDMMCS1FBK = 0x2; // OFDM 9 -> CCK 5.5
		LgCfg0.field.OFDMMCS0FBK = 0x2; // OFDM 6 -> CCK 5.5
	}
#endif /* AGS_SUPPORT */

#ifdef NEW_RATE_ADAPT_SUPPORT
skipUpdate:
#endif /* NEW_RATE_ADAPT_SUPPORT */

	RTMP_IO_WRITE32(pAd, HT_FBK_CFG0, HtCfg0.word);
	RTMP_IO_WRITE32(pAd, HT_FBK_CFG1, HtCfg1.word);
	RTMP_IO_WRITE32(pAd, LG_FBK_CFG0, LgCfg0.word);
	RTMP_IO_WRITE32(pAd, LG_FBK_CFG1, LgCfg1.word);

#ifdef DOT11N_SS3_SUPPORT
	if (IS_RT2883(pAd) || IS_RT3883(pAd)
#ifdef AGS_SUPPORT
		|| (bUseAGS == true)
#endif /* AGS_SUPPORT */
	)
	{
		RTMP_IO_WRITE32(pAd, TX_FBK_CFG_3S_0, Ht3SSCfg0.word);
		RTMP_IO_WRITE32(pAd, TX_FBK_CFG_3S_1, Ht3SSCfg1.word);
		DBGPRINT(RT_DEBUG_TRACE, ("AsicUpdateAutoFallBackTable: Ht3SSCfg0=0x%x, Ht3SSCfg1=0x%x\n", Ht3SSCfg0.word, Ht3SSCfg1.word));
	}
#endif /* DOT11N_SS3_SUPPORT */

}
#endif /* CONFIG_STA_SUPPORT */


/*
	========================================================================

	Routine Description:
		Set MAC register value according operation mode.
		OperationMode AND bNonGFExist are for MM and GF Proteciton.
		If MM or GF mask is not set, those passing argument doesn't not take effect.

		Operation mode meaning:
		= 0 : Pure HT, no preotection.
		= 0x01; there may be non-HT devices in both the control and extension channel, protection is optional in BSS.
		= 0x10: No Transmission in 40M is protected.
		= 0x11: Transmission in both 40M and 20M shall be protected
		if (bNonGFExist)
			we should choose not to use GF. But still set correct ASIC registers.
	========================================================================
*/
typedef enum _PROT_REG_IDX_{
	REG_IDX_CCK = 0,	/* 0x1364 */
	REG_IDX_OFDM = 1,	/* 0x1368 */
	REG_IDX_MM20 = 2,  /* 0x136c */
	REG_IDX_MM40 = 3, /* 0x1370 */
	REG_IDX_GF20 = 4, /* 0x1374 */
	REG_IDX_GF40 = 5, /* 0x1378 */

}PROT_REG_IDX;

void AsicUpdateProtect(
	IN struct rtmp_adapter *pAd,
	IN USHORT OperationMode,
	IN u8 SetMask,
	IN bool bDisableBGProtect,
	IN bool bNonGFExist)
{
	PROT_CFG_STRUC	ProtCfg, ProtCfg4;
	u32 Protect[6];
	USHORT offset;
	u32 i, PhyMode = 0x4000;
	u32 MacReg = 0;

#ifdef DOT11_N_SUPPORT
	if (!(pAd->CommonCfg.bHTProtect) && (OperationMode != 8))
		return;

	if (pAd->BATable.numDoneOriginator)
	{
		/* enable the RTS/CTS to avoid channel collision*/
		SetMask |= ALLN_SETPROTECT;
		OperationMode = 8;
	}
#endif /* DOT11_N_SUPPORT */

	/* Config ASIC RTS threshold register*/
	mt7610u_read32(pAd, TX_RTS_CFG, &MacReg);
	MacReg &= 0xFF0000FF;
	/* If the user want disable RtsThreshold and enbale Amsdu/Ralink-Aggregation, set the RtsThreshold as 4096*/
        if ((
#ifdef DOT11_N_SUPPORT
			(pAd->CommonCfg.BACapability.field.AmsduEnable) ||
#endif /* DOT11_N_SUPPORT */
			(pAd->CommonCfg.bAggregationCapable == true))
            && pAd->CommonCfg.RtsThreshold == MAX_RTS_THRESHOLD)
        {
			MacReg |= (0x1000 << 8);
        }
        else
        {
			MacReg |= (pAd->CommonCfg.RtsThreshold << 8);
        }

	RTMP_IO_WRITE32(pAd, TX_RTS_CFG, MacReg);

	/* Initial common protection settings*/
	memset(Protect, 0, sizeof(Protect));
	ProtCfg4.word = 0;
	ProtCfg.word = 0;
	ProtCfg.field.TxopAllowGF40 = 1;
	ProtCfg.field.TxopAllowGF20 = 1;
	ProtCfg.field.TxopAllowMM40 = 1;
	ProtCfg.field.TxopAllowMM20 = 1;
	ProtCfg.field.TxopAllowOfdm = 1;
	ProtCfg.field.TxopAllowCck = 1;
	ProtCfg.field.RTSThEn = 1;
	ProtCfg.field.ProtectNav = ASIC_SHORTNAV;

#ifdef DOT11_VHT_AC
	PhyMode = 0x2000; /* Bit 15:13, 0:Legacy CCK, 1: Legacy OFDM, 2: HT mix mode, 3: HT green field, 4: VHT mode, 5-7: Reserved */
#endif /* DOT11_VHT_AC */

	/* update PHY mode and rate*/
	if (pAd->OpMode == OPMODE_AP)
	{
		/* update PHY mode and rate*/
		if (pAd->CommonCfg.Channel > 14)
			ProtCfg.field.ProtectRate = PhyMode;
		ProtCfg.field.ProtectRate |= pAd->CommonCfg.RtsRate;
	}
	else if (pAd->OpMode == OPMODE_STA)
	{
		// Decide Protect Rate for Legacy packet
		if (pAd->CommonCfg.Channel > 14)
		{
			ProtCfg.field.ProtectRate = PhyMode; // OFDM 6Mbps
		}
		else
		{
			ProtCfg.field.ProtectRate = 0x0000; // CCK 1Mbps
			if (pAd->CommonCfg.MinTxRate > RATE_11)
				ProtCfg.field.ProtectRate |= PhyMode; // OFDM 6Mbps
		}
	}

	/* Handle legacy(B/G) protection*/
	if (bDisableBGProtect)
	{
		/*ProtCfg.field.ProtectRate = pAd->CommonCfg.RtsRate;*/
		ProtCfg.field.ProtectCtrl = 0;
		Protect[REG_IDX_CCK] = ProtCfg.word;
		Protect[REG_IDX_OFDM] = ProtCfg.word;
		pAd->FlgCtsEnabled = 0; /* CTS-self is not used */
	}
	else
	{
		/*ProtCfg.field.ProtectRate = pAd->CommonCfg.RtsRate;*/
		ProtCfg.field.ProtectCtrl = 0;			/* CCK do not need to be protected*/
		Protect[REG_IDX_CCK] = ProtCfg.word;
		ProtCfg.field.ProtectCtrl = ASIC_CTS;	/* OFDM needs using CCK to protect*/
		Protect[REG_IDX_OFDM] = ProtCfg.word;
		pAd->FlgCtsEnabled = 1; /* CTS-self is used */
	}

#ifdef DOT11_N_SUPPORT
	/* Decide HT frame protection.*/
	if ((SetMask & ALLN_SETPROTECT) != 0)
	{
		switch(OperationMode)
		{
			case 0x0:
				/* NO PROTECT */
				/* 1.All STAs in the BSS are 20/40 MHz HT*/
				/* 2. in ai 20/40MHz BSS*/
				/* 3. all STAs are 20MHz in a 20MHz BSS*/
				/* Pure HT. no protection.*/

				/* MM20_PROT_CFG*/
				/*	Reserved (31:27)*/
				/* 	PROT_TXOP(25:20) -- 010111*/
				/*	PROT_NAV(19:18)  -- 01 (Short NAV protection)*/
				/*  PROT_CTRL(17:16) -- 00 (None)*/
#ifdef DOT11_VHT_AC
				/* 	PROT_RATE(15:0)  -- 0x2004 (OFDM 24M)*/
				Protect[2] = 0x01742004;
#else /* DOT11_VHT_AC */
				/* 	PROT_RATE(15:0)  -- 0x4004 (OFDM 24M)*/
				Protect[2] = 0x01744004;
#endif /* !DOT11_VHT_AC */

				/* MM40_PROT_CFG*/
				/*	Reserved (31:27)*/
				/* 	PROT_TXOP(25:20) -- 111111*/
				/*	PROT_NAV(19:18)  -- 01 (Short NAV protection)*/
				/*  PROT_CTRL(17:16) -- 00 (None) */
#ifdef DOT11_VHT_AC
				/* 	PROT_RATE(15:0)  -- 0x2084 (duplicate OFDM 24M)*/
				Protect[3] = 0x03f42084;
#else
				/* 	PROT_RATE(15:0)  -- 0x4084 (duplicate OFDM 24M)*/
				Protect[3] = 0x03f44084;
#endif

				/* CF20_PROT_CFG*/
				/*	Reserved (31:27)*/
				/* 	PROT_TXOP(25:20) -- 010111*/
				/*	PROT_NAV(19:18)  -- 01 (Short NAV protection)*/
				/*  PROT_CTRL(17:16) -- 00 (None)*/
#ifdef DOT11_VHT_AC
				/* 	PROT_RATE(15:0)  -- 0x2004 (OFDM 24M)*/
				Protect[4] = 0x01742004;
#else
				/* 	PROT_RATE(15:0)  -- 0x4004 (OFDM 24M)*/
				Protect[4] = 0x01744004;
#endif

				/* CF40_PROT_CFG*/
				/*	Reserved (31:27)*/
				/* 	PROT_TXOP(25:20) -- 111111*/
				/*	PROT_NAV(19:18)  -- 01 (Short NAV protection)*/
				/*  PROT_CTRL(17:16) -- 00 (None)*/
#ifdef DOT11_VHT_AC
				/* 	PROT_RATE(15:0)  -- 0x2084 (duplicate OFDM 24M)*/
				Protect[5] = 0x03f42084;
#else
				/* 	PROT_RATE(15:0)  -- 0x4084 (duplicate OFDM 24M)*/
				Protect[5] = 0x03f44084;
#endif

				if (bNonGFExist)
				{
					/* PROT_NAV(19:18)  -- 01 (Short NAV protectiion)*/
					/* PROT_CTRL(17:16) -- 01 (RTS/CTS)*/
#ifdef DOT11_VHT_AC
					Protect[REG_IDX_GF20] = 0x01752004;
					Protect[REG_IDX_GF40] = 0x03f52084;
#else
					Protect[REG_IDX_GF20] = 0x01754004;
					Protect[REG_IDX_GF40] = 0x03f54084;
#endif
				}
				pAd->CommonCfg.IOTestParm.bRTSLongProtOn = false;

#ifdef DOT11_VHT_AC
#ifdef RT65xx
				// TODO: shiang-6590, fix me for this protection mechanism
				if (IS_RT65XX(pAd))
				{
					PROT_CFG_STRUC vht_port_cfg;

					mt7610u_read32(pAd, TX_PROT_CFG6, &vht_port_cfg.word);
					vht_port_cfg.field.ProtectCtrl = 0;
					RTMP_IO_WRITE32(pAd, TX_PROT_CFG6, vht_port_cfg.word);

					mt7610u_read32(pAd, TX_PROT_CFG7, &vht_port_cfg.word);
					vht_port_cfg.field.ProtectCtrl = 0;
					RTMP_IO_WRITE32(pAd, TX_PROT_CFG7, vht_port_cfg.word);

					mt7610u_read32(pAd, TX_PROT_CFG8, &vht_port_cfg.word);
					vht_port_cfg.field.ProtectCtrl = 0;
					RTMP_IO_WRITE32(pAd, TX_PROT_CFG8, vht_port_cfg.word);
				}
#endif /* RT65xx */
#endif /* DOT11_VHT_AC */
				break;

 			case 1:
				/* This is "HT non-member protection mode."*/
				/* If there may be non-HT STAs my BSS*/
#ifdef DOT11_VHT_AC
				ProtCfg.word = 0x01742004;	/* PROT_CTRL(17:16) : 0 (None)*/
				ProtCfg4.word = 0x03f42084; /* duplicaet legacy 24M. BW set 1.*/
#else
				ProtCfg.word = 0x01744004;	/* PROT_CTRL(17:16) : 0 (None)*/
				ProtCfg4.word = 0x03f44084; /* duplicaet legacy 24M. BW set 1.*/
#endif
				if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_BG_PROTECTION_INUSED))
				{
					ProtCfg.word = 0x01740003;	/*ERP use Protection bit is set, use protection rate at Clause 18..*/
					ProtCfg4.word = 0x03f40003; /* Don't duplicate RTS/CTS in CCK mode. 0x03f40083; */
				}
				/*Assign Protection method for 20&40 MHz packets*/
				ProtCfg.field.ProtectCtrl = ASIC_RTS;
				ProtCfg.field.ProtectNav = ASIC_SHORTNAV;
				ProtCfg4.field.ProtectCtrl = ASIC_RTS;
				ProtCfg4.field.ProtectNav = ASIC_SHORTNAV;
				Protect[REG_IDX_MM20] = ProtCfg.word;
				Protect[REG_IDX_MM40] = ProtCfg4.word;
				Protect[REG_IDX_GF20] = ProtCfg.word;
				Protect[REG_IDX_GF40] = ProtCfg4.word;
				pAd->CommonCfg.IOTestParm.bRTSLongProtOn = true;

#ifdef DOT11_VHT_AC
#ifdef RT65xx
				// TODO: shiang-6590, fix me for this protection mechanism
				if (IS_RT65XX(pAd))
				{
					// Temporary tuen on RTS in VHT, MAC: TX_PROT_CFG6, TX_PROT_CFG7, TX_PROT_CFG8
					PROT_CFG_STRUC vht_port_cfg;

					mt7610u_read32(pAd, TX_PROT_CFG6, &vht_port_cfg.word);
					vht_port_cfg.field.ProtectCtrl = ASIC_RTS;
					vht_port_cfg.field.ProtectNav = ASIC_SHORTNAV;
					RTMP_IO_WRITE32(pAd, TX_PROT_CFG6, vht_port_cfg.word);

					mt7610u_read32(pAd, TX_PROT_CFG7, &vht_port_cfg.word);
					vht_port_cfg.field.ProtectCtrl = ASIC_RTS;
					vht_port_cfg.field.ProtectNav = ASIC_SHORTNAV;
					RTMP_IO_WRITE32(pAd, TX_PROT_CFG7, vht_port_cfg.word);

					mt7610u_read32(pAd, TX_PROT_CFG8, &vht_port_cfg.word);
					vht_port_cfg.field.ProtectCtrl = ASIC_RTS;
					vht_port_cfg.field.ProtectNav = ASIC_SHORTNAV;
					RTMP_IO_WRITE32(pAd, TX_PROT_CFG8, vht_port_cfg.word);
				}
#endif /* RT65xx */
#endif /* DOT11_VHT_AC */

				break;

			case 2:
				/* If only HT STAs are in BSS. at least one is 20MHz. Only protect 40MHz packets*/
#ifdef DOT11_VHT_AC
				ProtCfg.word = 0x01742004;  /* PROT_CTRL(17:16) : 0 (None)*/
				ProtCfg4.word = 0x03f42084; /* duplicaet legacy 24M. BW set 1.*/
#else
				ProtCfg.word = 0x01744004;  /* PROT_CTRL(17:16) : 0 (None)*/
				ProtCfg4.word = 0x03f44084; /* duplicaet legacy 24M. BW set 1.*/
#endif
				if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_BG_PROTECTION_INUSED))
				{
					ProtCfg.word = 0x01740003;	/*ERP use Protection bit is set, use protection rate at Clause 18..*/
					ProtCfg4.word = 0x03f40003; /* Don't duplicate RTS/CTS in CCK mode. 0x03f40083; */
				}
				/*Assign Protection method for 40MHz packets*/
				ProtCfg4.field.ProtectCtrl = ASIC_RTS;
				ProtCfg4.field.ProtectNav = ASIC_SHORTNAV;
				Protect[REG_IDX_MM20] = ProtCfg.word;
				Protect[REG_IDX_MM40] = ProtCfg4.word;
				if (bNonGFExist)
				{
					ProtCfg.field.ProtectCtrl = ASIC_RTS;
					ProtCfg.field.ProtectNav = ASIC_SHORTNAV;
				}
				Protect[REG_IDX_GF20] = ProtCfg.word;
				Protect[REG_IDX_GF40] = ProtCfg4.word;

				pAd->CommonCfg.IOTestParm.bRTSLongProtOn = false;

#ifdef DOT11_VHT_AC
#ifdef RT65xx
				// TODO: shiang-6590, fix me for this protection mechanism
				if (IS_RT65XX(pAd))
				{
					PROT_CFG_STRUC vht_port_cfg;

					mt7610u_read32(pAd, TX_PROT_CFG6, &vht_port_cfg.word);
					vht_port_cfg.field.ProtectCtrl = 0;
					RTMP_IO_WRITE32(pAd, TX_PROT_CFG6, vht_port_cfg.word);

					mt7610u_read32(pAd, TX_PROT_CFG7, &vht_port_cfg.word);
					vht_port_cfg.field.ProtectCtrl = ASIC_RTS;
					vht_port_cfg.field.ProtectNav = ASIC_SHORTNAV;
					RTMP_IO_WRITE32(pAd, TX_PROT_CFG7, vht_port_cfg.word);

					mt7610u_read32(pAd, TX_PROT_CFG8, &vht_port_cfg.word);
					vht_port_cfg.field.ProtectCtrl = ASIC_RTS;
					vht_port_cfg.field.ProtectNav = ASIC_SHORTNAV;
					RTMP_IO_WRITE32(pAd, TX_PROT_CFG8, vht_port_cfg.word);
				}
#endif /* RT65xx */
#endif /* DOT11_VHT_AC */
				break;

			case 3:
				/* HT mixed mode.	 PROTECT ALL!*/
				/* Assign Rate*/
#ifdef DOT11_VHT_AC
				ProtCfg.word = 0x01742004;	/*duplicaet legacy 24M. BW set 1.*/
				ProtCfg4.word = 0x03f42084;
#else
				ProtCfg.word = 0x01744004;	/*duplicaet legacy 24M. BW set 1.*/
				ProtCfg4.word = 0x03f44084;
#endif
				/* both 20MHz and 40MHz are protected. Whether use RTS or CTS-to-self depends on the*/
				if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_BG_PROTECTION_INUSED))
				{
					ProtCfg.word = 0x01740003;	/*ERP use Protection bit is set, use protection rate at Clause 18..*/
					ProtCfg4.word = 0x03f40003; /* Don't duplicate RTS/CTS in CCK mode. 0x03f40083*/
				}
				/*Assign Protection method for 20&40 MHz packets*/
				ProtCfg.field.ProtectCtrl = ASIC_RTS;
				ProtCfg.field.ProtectNav = ASIC_SHORTNAV;
				ProtCfg4.field.ProtectCtrl = ASIC_RTS;
				ProtCfg4.field.ProtectNav = ASIC_SHORTNAV;
				Protect[REG_IDX_MM20] = ProtCfg.word;
				Protect[REG_IDX_MM40] = ProtCfg4.word;
				Protect[REG_IDX_GF20] = ProtCfg.word;
				Protect[REG_IDX_GF40] = ProtCfg4.word;
				pAd->CommonCfg.IOTestParm.bRTSLongProtOn = true;

#ifdef DOT11_VHT_AC
#ifdef RT65xx
				// TODO: shiang-6590, fix me for this protection mechanism
				if (IS_RT65XX(pAd))
				{
					// Temporary tuen on RTS in VHT, MAC: TX_PROT_CFG6, TX_PROT_CFG7, TX_PROT_CFG8
					PROT_CFG_STRUC vht_port_cfg;

					mt7610u_read32(pAd, TX_PROT_CFG6, &vht_port_cfg.word);
					vht_port_cfg.field.ProtectCtrl = ASIC_RTS;
					vht_port_cfg.field.ProtectNav = ASIC_SHORTNAV;
					RTMP_IO_WRITE32(pAd, TX_PROT_CFG6, vht_port_cfg.word);

					mt7610u_read32(pAd, TX_PROT_CFG7, &vht_port_cfg.word);
					vht_port_cfg.field.ProtectCtrl = ASIC_RTS;
					vht_port_cfg.field.ProtectNav = ASIC_SHORTNAV;
					RTMP_IO_WRITE32(pAd, TX_PROT_CFG7, vht_port_cfg.word);

					mt7610u_read32(pAd, TX_PROT_CFG8, &vht_port_cfg.word);
					vht_port_cfg.field.ProtectCtrl = ASIC_RTS;
					vht_port_cfg.field.ProtectNav = ASIC_SHORTNAV;
					RTMP_IO_WRITE32(pAd, TX_PROT_CFG8, vht_port_cfg.word);
				}
#endif /* RT65xx */
#endif /* DOT11_VHT_AC */
				break;

			case 8:
				/* Special on for Atheros problem n chip.*/
#ifdef DOT11_VHT_AC
				ProtCfg.word = 0x01752004;	/*duplicaet legacy 24M. BW set 1.*/
				ProtCfg4.word = 0x03f52084;
#else
				ProtCfg.word = 0x01754004;	/*duplicaet legacy 24M. BW set 1.*/
				ProtCfg4.word = 0x03f54084;
#endif
				if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_BG_PROTECTION_INUSED))
				{
					ProtCfg.word = 0x01750003;	/*ERP use Protection bit is set, use protection rate at Clause 18..*/
					ProtCfg4.word = 0x03f50003; /* Don't duplicate RTS/CTS in CCK mode. 0x03f40083*/
				}

				Protect[REG_IDX_MM20] = ProtCfg.word; 	/*0x01754004;*/
				Protect[REG_IDX_MM40] = ProtCfg4.word; /*0x03f54084;*/
				Protect[REG_IDX_GF20] = ProtCfg.word; 	/*0x01754004;*/
				Protect[REG_IDX_GF40] = ProtCfg4.word; /*0x03f54084;*/
				pAd->CommonCfg.IOTestParm.bRTSLongProtOn = true;
				break;
		}
	}
#endif /* DOT11_N_SUPPORT */

	offset = CCK_PROT_CFG;
	for (i = 0;i < 6;i++)
	{
		if ((SetMask & (1<< i)))
		{
			RTMP_IO_WRITE32(pAd, offset + i*4, Protect[i]);
		}
	}

#ifdef DOT11_VHT_AC
#ifdef RT65xx
	if (IS_RT65XX(pAd))
	{
		mt7610u_read32(pAd, TX_PROT_CFG8, &MacReg);
		MacReg &= (~0x18000000);
		if (pAd->CommonCfg.vht_bw_signal)
		{
			if (pAd->CommonCfg.vht_bw_signal == 1) /* static */
				MacReg |= 0x08000000;
			else if (pAd->CommonCfg.vht_bw_signal == 2)/* dynamic */
				MacReg |= 0x18000000;
		}
		RTMP_IO_WRITE32(pAd, TX_PROT_CFG8, MacReg);
	}
#endif /* RT65xx */
#endif /* DOT11_VHT_AC */
}


/*
	==========================================================================
	Description:

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicSwitchChannel(
	IN struct rtmp_adapter*pAd,
	IN u8 Channel,
	IN bool bScan)
{
	u8 bw;
#ifdef CONFIG_STA_SUPPORT
#ifdef CONFIG_PM
#ifdef USB_SUPPORT_SELECTIVE_SUSPEND
	struct os_cookie * pObj = pAd->OS_Cookie;
#endif /* USB_SUPPORT_SELECTIVE_SUSPEND */
#endif /* CONFIG_PM */
#endif /* CONFIG_STA_SUPPORT */

	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF))
		return;

	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_SUSPEND);
#ifdef CONFIG_STA_SUPPORT
#ifdef CONFIG_PM
#ifdef USB_SUPPORT_SELECTIVE_SUSPEND

			if( (RTMP_Usb_AutoPM_Get_Interface(pObj->pUsb_Dev,pObj->intf)) == 1)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s(): autopm_resume success\n", __FUNCTION__));
				RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_SUSPEND);
			}
			else if ((RTMP_Usb_AutoPM_Get_Interface(pObj->pUsb_Dev,pObj->intf)) == (-1))
			{
				DBGPRINT(RT_DEBUG_ERROR, ("%s(): autopm_resume fail\n", __FUNCTION__));
				RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_SUSPEND);
				return;
			}
			else
				DBGPRINT(RT_DEBUG_TRACE, ("%s(): autopm_resume do nothing\n", __FUNCTION__));

#endif /* USB_SUPPORT_SELECTIVE_SUSPEND */
#endif /* CONFIG_PM */
#endif /* CONFIG_STA_SUPPORT */



#ifdef DOT11_VHT_AC
	if (pAd->CommonCfg.BBPCurrentBW == BW_80)
		pAd->hw_cfg.cent_ch = pAd->CommonCfg.vht_cent_ch;
	else
#endif /* DOT11_VHT_AC */
		pAd->hw_cfg.cent_ch = Channel;

	if (pAd->chipOps.ChipSwitchChannel)
		pAd->chipOps.ChipSwitchChannel(pAd, Channel, bScan);
	else
		DBGPRINT(RT_DEBUG_ERROR, ("For this chip, no specified channel switch function!\n"));

	/* R66 should be set according to Channel and use 20MHz when scanning*/
	if (bScan)
		bw = BW_20;
	else {
		bw = pAd->CommonCfg.BBPCurrentBW;
	}
}


/*
	==========================================================================
	Description:
		This function is required for 2421 only, and should not be used during
		site survey. It's only required after NIC decided to stay at a channel
		for a longer period.
		When this function is called, it's always after AsicSwitchChannel().

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicLockChannel(
	IN struct rtmp_adapter *pAd,
	IN u8 Channel)
{
}

/*
	==========================================================================
	Description:

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	==========================================================================
 */

void AsicResetBBPAgent(struct rtmp_adapter *pAd)
{
	/* Still need to find why BBP agent keeps busy, but in fact, hardware still function ok. Now clear busy first.	*/
	/* IF chipOps.AsicResetBbpAgent == NULL, run "else" part */
	RTMP_CHIP_ASIC_RESET_BBP_AGENT(pAd);
}


#ifdef CONFIG_STA_SUPPORT
/*
	==========================================================================
	Description:
		put PHY to sleep here, and set next wakeup timer. PHY doesn't not wakeup
		automatically. Instead, MCU will issue a TwakeUpInterrupt to host after
		the wakeup timer timeout. Driver has to issue a separate command to wake
		PHY up.

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicSleepThenAutoWakeup(
	IN struct rtmp_adapter *pAd,
	IN USHORT TbttNumToNextWakeUp)
{
	RTMP_STA_SLEEP_THEN_AUTO_WAKEUP(pAd, TbttNumToNextWakeUp);
}

/*
	==========================================================================
	Description:
		AsicForceWakeup() is used whenever manual wakeup is required
		AsicForceSleep() should only be used when not in INFRA BSS. When
		in INFRA BSS, we should use AsicSleepThenAutoWakeup() instead.
	==========================================================================
 */
void AsicForceSleep(
	IN struct rtmp_adapter *pAd)
{

}

/*
	==========================================================================
	Description:
		AsicForceWakeup() is used whenever Twakeup timer (set via AsicSleepThenAutoWakeup)
		expired.

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL
	==========================================================================
 */
void AsicForceWakeup(
	IN struct rtmp_adapter *pAd,
	IN bool    bFromTx)
{
    DBGPRINT(RT_DEBUG_INFO, ("--> AsicForceWakeup \n"));
    RTMP_STA_FORCE_WAKEUP(pAd, bFromTx);
}
#endif /* CONFIG_STA_SUPPORT */


/*
	==========================================================================
	Description:
		Set My BSSID

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicSetBssid(
	IN struct rtmp_adapter *pAd,
	IN u8 *pBssid)
{
	ULONG		  Addr4;

	DBGPRINT(RT_DEBUG_TRACE, ("===> AsicSetBssid %x:%x:%x:%x:%x:%x\n",
				PRINT_MAC(pBssid)));

	Addr4 = (ULONG)(pBssid[0]) |
			(ULONG)(pBssid[1] << 8)  |
			(ULONG)(pBssid[2] << 16) |
			(ULONG)(pBssid[3] << 24);
	RTMP_IO_WRITE32(pAd, MAC_BSSID_DW0, Addr4);


	Addr4 = 0;
	/* always one BSSID in STA mode*/
	Addr4 = (ULONG)(pBssid[4]) | (ULONG)(pBssid[5] << 8);


	RTMP_IO_WRITE32(pAd, MAC_BSSID_DW1, Addr4);
}


#ifdef DOT11_N_SUPPORT
/*
	==========================================================================
	Description:

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicEnableRDG(
	IN struct rtmp_adapter *pAd)
{
	TX_LINK_CFG_STRUC	TxLinkCfg;
	u32				Data = 0;

	mt7610u_read32(pAd, TX_LINK_CFG, &TxLinkCfg.word);
	TxLinkCfg.field.TxRDGEn = 1;
	RTMP_IO_WRITE32(pAd, TX_LINK_CFG, TxLinkCfg.word);

	mt7610u_read32(pAd, EDCA_AC0_CFG, &Data);
	Data  &= 0xFFFFFF00;
	Data  |= 0x80;
	RTMP_IO_WRITE32(pAd, EDCA_AC0_CFG, Data);
}

/*
	==========================================================================
	Description:

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicDisableRDG(
	IN struct rtmp_adapter *pAd)
{
	TX_LINK_CFG_STRUC	TxLinkCfg;
	u32				Data = 0;



	mt7610u_read32(pAd, TX_LINK_CFG, &TxLinkCfg.word);
	TxLinkCfg.field.TxRDGEn = 0;
	RTMP_IO_WRITE32(pAd, TX_LINK_CFG, TxLinkCfg.word);

	mt7610u_read32(pAd, EDCA_AC0_CFG, &Data);

	Data  &= 0xFFFFFF00;
	/*Data  |= 0x20;*/
#ifndef WIFI_TEST
	/*if ( pAd->CommonCfg.bEnableTxBurst )	*/
	/*	Data |= 0x60;  for performance issue not set the TXOP to 0*/
#endif
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_DYNAMIC_BE_TXOP_ACTIVE)
#ifdef DOT11_N_SUPPORT
		&& (pAd->MacTab.fAnyStationMIMOPSDynamic == false)
#endif /* DOT11_N_SUPPORT */
	)
	{
		/* For CWC test, change txop from 0x30 to 0x20 in TxBurst mode*/
		if (pAd->CommonCfg.bEnableTxBurst)
		Data |= 0x20;
	}
	RTMP_IO_WRITE32(pAd, EDCA_AC0_CFG, Data);

}
#endif /* DOT11_N_SUPPORT */

/*
	==========================================================================
	Description:

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicDisableSync(
	IN struct rtmp_adapter *pAd)
{
	BCN_TIME_CFG_STRUC csr;

	DBGPRINT(RT_DEBUG_TRACE, ("--->Disable TSF synchronization\n"));

	/* 2003-12-20 disable TSF and TBTT while NIC in power-saving have side effect*/
	/*			  that NIC will never wakes up because TSF stops and no more */
	/*			  TBTT interrupts*/
	pAd->TbttTickCount = 0;
	mt7610u_read32(pAd, BCN_TIME_CFG, &csr.word);
	csr.field.bBeaconGen = 0;
	csr.field.bTBTTEnable = 0;
	csr.field.TsfSyncMode = 0;
	csr.field.bTsfTicking = 0;
	RTMP_IO_WRITE32(pAd, BCN_TIME_CFG, csr.word);

}

/*
	==========================================================================
	Description:

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicEnableBssSync(
	IN struct rtmp_adapter *pAd)
{
	BCN_TIME_CFG_STRUC csr;

	DBGPRINT(RT_DEBUG_TRACE, ("--->AsicEnableBssSync(INFRA mode)\n"));

	mt7610u_read32(pAd, BCN_TIME_CFG, &csr.word);
/*	RTMP_IO_WRITE32(pAd, BCN_TIME_CFG, 0x00000000);*/
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		csr.field.BeaconInterval = pAd->CommonCfg.BeaconPeriod << 4; /* ASIC register in units of 1/16 TU*/
		csr.field.bTsfTicking = 1;
		csr.field.TsfSyncMode = 1; /* sync TSF in INFRASTRUCTURE mode*/
		csr.field.bBeaconGen  = 0; /* do NOT generate BEACON*/
		csr.field.bTBTTEnable = 1;
	}
#endif /* CONFIG_STA_SUPPORT */
	RTMP_IO_WRITE32(pAd, BCN_TIME_CFG, csr.word);
}

/*
	==========================================================================
	Description:
	Note:
		BEACON frame in shared memory should be built ok before this routine
		can be called. Otherwise, a garbage frame maybe transmitted out every
		Beacon period.

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicEnableIbssSync(
	IN struct rtmp_adapter *pAd)
{
	BCN_TIME_CFG_STRUC csr9;
	u8 *		ptr;
	UINT i;
	ULONG beaconBaseLocation = 0;
	USHORT			beaconLen = (USHORT) pAd->BeaconTxWI.TxWIMPDUByteCnt;
	u8 TXWISize = sizeof(struct txwi_nmac);
	u32 longptr;

#ifdef RT_BIG_ENDIAN
	{
	struct txwi_nmac 	localTxWI;

	memmove((u8 *)&localTxWI, (u8 *)&pAd->BeaconTxWI, TXWISize);
	RTMPWIEndianChange(&localTxWI, sizeof(*localTxWI));
	beaconLen = (USHORT) localTxWI.TxWIMPDUByteCnt;
	}
#endif /* RT_BIG_ENDIAN */

	DBGPRINT(RT_DEBUG_TRACE, ("--->AsicEnableIbssSync(TxWIMPDUByteCnt=%d, beaconLen=%d)\n", pAd->BeaconTxWI.TxWIMPDUByteCnt, beaconLen));


	DBGPRINT(RT_DEBUG_TRACE, ("--->AsicEnableIbssSync(ADHOC mode. TxWIMPDUByteCnt = %d)\n", pAd->BeaconTxWI.TxWIMPDUByteCnt));

	mt7610u_read32(pAd, BCN_TIME_CFG, &csr9.word);
	csr9.field.bBeaconGen = 0;
	csr9.field.bTBTTEnable = 0;
	csr9.field.bTsfTicking = 0;
	RTMP_IO_WRITE32(pAd, BCN_TIME_CFG, csr9.word);
	beaconBaseLocation = HW_BEACON_BASE0(pAd);


#ifdef RTMP_MAC_USB
	/* move BEACON TXD and frame content to on-chip memory*/
	ptr = (u8 *)&pAd->BeaconTxWI;
	for (i=0; i < TXWISize; i+=2)
	{
		longptr =  *ptr + (*(ptr+1)<<8);
		RTMP_CHIP_UPDATE_BEACON(pAd, HW_BEACON_BASE0(pAd) + i, longptr, 2);
		ptr += 2;
	}

	/* start right after the 16-byte TXWI field*/
	ptr = pAd->BeaconBuf;
	/*for (i=0; i< pAd->BeaconTxWI.TxWIMPDUByteCnt; i+=2)*/
	for (i=0; i< beaconLen; i+=2)
	{
		longptr =  *ptr + (*(ptr+1)<<8);
		RTMP_CHIP_UPDATE_BEACON(pAd, HW_BEACON_BASE0(pAd) + TXWISize + i, longptr, 2);
		ptr +=2;
	}
#endif /* RTMP_MAC_USB */



	/* For Wi-Fi faily generated beacons between participating stations. */
	/* Set TBTT phase adaptive adjustment step to 8us (default 16us)*/
	/* don't change settings 2006-5- by Jerry*/
	/*RTMP_IO_WRITE32(pAd, TBTT_SYNC_CFG, 0x00001010);*/

	/* start sending BEACON*/
	csr9.field.BeaconInterval = pAd->CommonCfg.BeaconPeriod << 4; /* ASIC register in units of 1/16 TU*/
	csr9.field.bTsfTicking = 1;
	/*
		(STA ad-hoc mode) Upon the reception of BEACON frame from associated BSS,
		local TSF is updated with remote TSF only if the remote TSF is greater than local TSF
	*/
	csr9.field.TsfSyncMode = 2; /* sync TSF in IBSS mode*/
	csr9.field.bTBTTEnable = 1;
	csr9.field.bBeaconGen = 1;
	RTMP_IO_WRITE32(pAd, BCN_TIME_CFG, csr9.word);
}

/*
	==========================================================================
	Description:

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicSetEdcaParm(
	IN struct rtmp_adapter *pAd,
	IN PEDCA_PARM	 pEdcaParm)
{
	EDCA_AC_CFG_STRUC   Ac0Cfg, Ac1Cfg, Ac2Cfg, Ac3Cfg;
	AC_TXOP_CSR0_STRUC csr0;
	AC_TXOP_CSR1_STRUC csr1;
	AIFSN_CSR_STRUC    AifsnCsr;
	CWMIN_CSR_STRUC    CwminCsr;
	CWMAX_CSR_STRUC    CwmaxCsr;
	int i;

	Ac0Cfg.word = 0;
	Ac1Cfg.word = 0;
	Ac2Cfg.word = 0;
	Ac3Cfg.word = 0;
	if ((pEdcaParm == NULL) || (pEdcaParm->bValid == false))
	{
		DBGPRINT(RT_DEBUG_TRACE,("AsicSetEdcaParm\n"));
		OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_WMM_INUSED);
		for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
		{
			if (IS_ENTRY_CLIENT(&pAd->MacTab.Content[i]) || IS_ENTRY_APCLI(&pAd->MacTab.Content[i]))
				CLIENT_STATUS_CLEAR_FLAG(&pAd->MacTab.Content[i], fCLIENT_STATUS_WMM_CAPABLE);
		}

		/*========================================================*/
		/*      MAC Register has a copy .*/
		/*========================================================*/
/*#ifndef WIFI_TEST*/
		if( pAd->CommonCfg.bEnableTxBurst )
		{
			/* For CWC test, change txop from 0x30 to 0x20 in TxBurst mode*/
			Ac0Cfg.field.AcTxop = 0x20; /* Suggest by John for TxBurst in HT Mode*/
		}
		else
			Ac0Cfg.field.AcTxop = 0;	/* QID_AC_BE*/
/*#else*/
/*		Ac0Cfg.field.AcTxop = 0;	 QID_AC_BE*/
/*#endif					*/
		Ac0Cfg.field.Cwmin = CW_MIN_IN_BITS;
		Ac0Cfg.field.Cwmax = CW_MAX_IN_BITS;
		Ac0Cfg.field.Aifsn = 2;
		RTMP_IO_WRITE32(pAd, EDCA_AC0_CFG, Ac0Cfg.word);

		Ac1Cfg.field.AcTxop = 0;	/* QID_AC_BK*/
		Ac1Cfg.field.Cwmin = CW_MIN_IN_BITS;
		Ac1Cfg.field.Cwmax = CW_MAX_IN_BITS;
		Ac1Cfg.field.Aifsn = 2;
		RTMP_IO_WRITE32(pAd, EDCA_AC1_CFG, Ac1Cfg.word);

		if (WMODE_EQUAL(pAd->CommonCfg.PhyMode, WMODE_B))
		{
			Ac2Cfg.field.AcTxop = 192;	/* AC_VI: 192*32us ~= 6ms*/
			Ac3Cfg.field.AcTxop = 96;	/* AC_VO: 96*32us  ~= 3ms*/
		}
		else
		{
			Ac2Cfg.field.AcTxop = 96;	/* AC_VI: 96*32us ~= 3ms*/
			Ac3Cfg.field.AcTxop = 48;	/* AC_VO: 48*32us ~= 1.5ms*/
		}
		Ac2Cfg.field.Cwmin = CW_MIN_IN_BITS;
		Ac2Cfg.field.Cwmax = CW_MAX_IN_BITS;
		Ac2Cfg.field.Aifsn = 2;
		RTMP_IO_WRITE32(pAd, EDCA_AC2_CFG, Ac2Cfg.word);
		Ac3Cfg.field.Cwmin = CW_MIN_IN_BITS;
		Ac3Cfg.field.Cwmax = CW_MAX_IN_BITS;
		Ac3Cfg.field.Aifsn = 2;
		RTMP_IO_WRITE32(pAd, EDCA_AC3_CFG, Ac3Cfg.word);

		/*========================================================*/
		/*      DMA Register has a copy too.*/
		/*========================================================*/
		csr0.field.Ac0Txop = 0;		/* QID_AC_BE*/
		csr0.field.Ac1Txop = 0;		/* QID_AC_BK*/
		RTMP_IO_WRITE32(pAd, WMM_TXOP0_CFG, csr0.word);
		if (WMODE_EQUAL(pAd->CommonCfg.PhyMode, WMODE_B))
		{
			csr1.field.Ac2Txop = 192;		/* AC_VI: 192*32us ~= 6ms*/
			csr1.field.Ac3Txop = 96;		/* AC_VO: 96*32us  ~= 3ms*/
		}
		else
		{
			csr1.field.Ac2Txop = 96;		/* AC_VI: 96*32us ~= 3ms*/
			csr1.field.Ac3Txop = 48;		/* AC_VO: 48*32us ~= 1.5ms*/
		}
		RTMP_IO_WRITE32(pAd, WMM_TXOP1_CFG, csr1.word);

		CwminCsr.word = 0;
		CwminCsr.field.Cwmin0 = CW_MIN_IN_BITS;
		CwminCsr.field.Cwmin1 = CW_MIN_IN_BITS;
		CwminCsr.field.Cwmin2 = CW_MIN_IN_BITS;
		CwminCsr.field.Cwmin3 = CW_MIN_IN_BITS;
		RTMP_IO_WRITE32(pAd, WMM_CWMIN_CFG, CwminCsr.word);

		CwmaxCsr.word = 0;
		CwmaxCsr.field.Cwmax0 = CW_MAX_IN_BITS;
		CwmaxCsr.field.Cwmax1 = CW_MAX_IN_BITS;
		CwmaxCsr.field.Cwmax2 = CW_MAX_IN_BITS;
		CwmaxCsr.field.Cwmax3 = CW_MAX_IN_BITS;
		RTMP_IO_WRITE32(pAd, WMM_CWMAX_CFG, CwmaxCsr.word);

		RTMP_IO_WRITE32(pAd, WMM_AIFSN_CFG, 0x00002222);

		memset(&pAd->CommonCfg.APEdcaParm, 0, sizeof(EDCA_PARM));

	}
	else
	{
		OPSTATUS_SET_FLAG(pAd, fOP_STATUS_WMM_INUSED);
		/*========================================================*/
		/*      MAC Register has a copy.*/
		/*========================================================*/

		/* Modify Cwmin/Cwmax/Txop on queue[QID_AC_VI], Recommend by Jerry 2005/07/27*/
		/* To degrade our VIDO Queue's throughput for WiFi WMM S3T07 Issue.*/

		/*pEdcaParm->Txop[QID_AC_VI] = pEdcaParm->Txop[QID_AC_VI] * 7 / 10;  rt2860c need this		*/

		Ac0Cfg.field.AcTxop =  pEdcaParm->Txop[QID_AC_BE];
		Ac0Cfg.field.Cwmin= pEdcaParm->Cwmin[QID_AC_BE];
		Ac0Cfg.field.Cwmax = pEdcaParm->Cwmax[QID_AC_BE];
		Ac0Cfg.field.Aifsn = pEdcaParm->Aifsn[QID_AC_BE]; /*+1;*/

		Ac1Cfg.field.AcTxop =  pEdcaParm->Txop[QID_AC_BK];
		Ac1Cfg.field.Cwmin = pEdcaParm->Cwmin[QID_AC_BK]; /*+2; */
		Ac1Cfg.field.Cwmax = pEdcaParm->Cwmax[QID_AC_BK];
		Ac1Cfg.field.Aifsn = pEdcaParm->Aifsn[QID_AC_BK]; /*+1;*/


		Ac2Cfg.field.AcTxop = (pEdcaParm->Txop[QID_AC_VI] * 6) / 10;
		{
		Ac2Cfg.field.Cwmin = pEdcaParm->Cwmin[QID_AC_VI];
		Ac2Cfg.field.Cwmax = pEdcaParm->Cwmax[QID_AC_VI];
		}
		/*sync with window 20110524*/
		Ac2Cfg.field.Aifsn = pEdcaParm->Aifsn[QID_AC_VI] + 1; /* 5.2.27 T6 Pass Tx VI+BE, but will impack 5.2.27/28 T7. Tx VI*/

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			/* Tuning for Wi-Fi WMM S06*/
			if (pAd->CommonCfg.bWiFiTest &&
				pEdcaParm->Aifsn[QID_AC_VI] == 10)
				Ac2Cfg.field.Aifsn -= 1;

			/* Tuning for TGn Wi-Fi 5.2.32*/
			/* STA TestBed changes in this item: conexant legacy sta ==> broadcom 11n sta*/
			if (STA_TGN_WIFI_ON(pAd) &&
				pEdcaParm->Aifsn[QID_AC_VI] == 10)
			{
				Ac0Cfg.field.Aifsn = 3;
				Ac2Cfg.field.AcTxop = 5;
			}

		}
#endif /* CONFIG_STA_SUPPORT */

		Ac3Cfg.field.AcTxop = pEdcaParm->Txop[QID_AC_VO];
		Ac3Cfg.field.Cwmin = pEdcaParm->Cwmin[QID_AC_VO];
		Ac3Cfg.field.Cwmax = pEdcaParm->Cwmax[QID_AC_VO];
		Ac3Cfg.field.Aifsn = pEdcaParm->Aifsn[QID_AC_VO];

/*#ifdef WIFI_TEST*/
		if (pAd->CommonCfg.bWiFiTest)
		{
			if (Ac3Cfg.field.AcTxop == 102)
			{
			Ac0Cfg.field.AcTxop = pEdcaParm->Txop[QID_AC_BE] ? pEdcaParm->Txop[QID_AC_BE] : 10;
				Ac0Cfg.field.Aifsn  = pEdcaParm->Aifsn[QID_AC_BE]-1; /* AIFSN must >= 1 */
			Ac1Cfg.field.AcTxop = pEdcaParm->Txop[QID_AC_BK];
				Ac1Cfg.field.Aifsn  = pEdcaParm->Aifsn[QID_AC_BK];
			Ac2Cfg.field.AcTxop = pEdcaParm->Txop[QID_AC_VI];
			}
		}
/*#endif  WIFI_TEST */
#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */


		RTMP_IO_WRITE32(pAd, EDCA_AC0_CFG, Ac0Cfg.word);
		RTMP_IO_WRITE32(pAd, EDCA_AC1_CFG, Ac1Cfg.word);
		RTMP_IO_WRITE32(pAd, EDCA_AC2_CFG, Ac2Cfg.word);
		RTMP_IO_WRITE32(pAd, EDCA_AC3_CFG, Ac3Cfg.word);


		/*========================================================*/
		/*      DMA Register has a copy too.*/
		/*========================================================*/
		csr0.field.Ac0Txop = Ac0Cfg.field.AcTxop;
		csr0.field.Ac1Txop = Ac1Cfg.field.AcTxop;
		RTMP_IO_WRITE32(pAd, WMM_TXOP0_CFG, csr0.word);

		csr1.field.Ac2Txop = Ac2Cfg.field.AcTxop;
		csr1.field.Ac3Txop = Ac3Cfg.field.AcTxop;
		RTMP_IO_WRITE32(pAd, WMM_TXOP1_CFG, csr1.word);

		CwminCsr.word = 0;
		CwminCsr.field.Cwmin0 = pEdcaParm->Cwmin[QID_AC_BE];
		CwminCsr.field.Cwmin1 = pEdcaParm->Cwmin[QID_AC_BK];
		CwminCsr.field.Cwmin2 = pEdcaParm->Cwmin[QID_AC_VI];
#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
			CwminCsr.field.Cwmin3 = pEdcaParm->Cwmin[QID_AC_VO] - 1; /*for TGn wifi test*/
#endif /* CONFIG_STA_SUPPORT */
		RTMP_IO_WRITE32(pAd, WMM_CWMIN_CFG, CwminCsr.word);

		CwmaxCsr.word = 0;
		CwmaxCsr.field.Cwmax0 = pEdcaParm->Cwmax[QID_AC_BE];
		CwmaxCsr.field.Cwmax1 = pEdcaParm->Cwmax[QID_AC_BK];
		CwmaxCsr.field.Cwmax2 = pEdcaParm->Cwmax[QID_AC_VI];
		CwmaxCsr.field.Cwmax3 = pEdcaParm->Cwmax[QID_AC_VO];
		RTMP_IO_WRITE32(pAd, WMM_CWMAX_CFG, CwmaxCsr.word);

		AifsnCsr.word = 0;
		AifsnCsr.field.Aifsn0 = Ac0Cfg.field.Aifsn; /*pEdcaParm->Aifsn[QID_AC_BE];*/
		AifsnCsr.field.Aifsn1 = Ac1Cfg.field.Aifsn; /*pEdcaParm->Aifsn[QID_AC_BK];*/
#ifdef CONFIG_STA_SUPPORT
#ifdef RTMP_MAC_USB
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			if(pAd->Antenna.field.TxPath == 1)
				AifsnCsr.field.Aifsn1 = Ac1Cfg.field.Aifsn + 2; 	/*5.2.27 T7 Pass*/
		}
#endif /* RTMP_MAC_USB */
#endif /* CONFIG_STA_SUPPORT */
		AifsnCsr.field.Aifsn2 = Ac2Cfg.field.Aifsn; /*pEdcaParm->Aifsn[QID_AC_VI];*/

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			/* Tuning for Wi-Fi WMM S06*/
			if (pAd->CommonCfg.bWiFiTest &&
				pEdcaParm->Aifsn[QID_AC_VI] == 10)
				AifsnCsr.field.Aifsn2 = Ac2Cfg.field.Aifsn - 4;

			/* Tuning for TGn Wi-Fi 5.2.32*/
			/* STA TestBed changes in this item: connexant legacy sta ==> broadcom 11n sta*/
			if (STA_TGN_WIFI_ON(pAd) &&
				pEdcaParm->Aifsn[QID_AC_VI] == 10)
			{
				AifsnCsr.field.Aifsn0 = 3;
				AifsnCsr.field.Aifsn2 = 7;
			}

			if (INFRA_ON(pAd))
				CLIENT_STATUS_SET_FLAG(&pAd->MacTab.Content[BSSID_WCID], fCLIENT_STATUS_WMM_CAPABLE);
		}
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			AifsnCsr.field.Aifsn3 = Ac3Cfg.field.Aifsn - 1; /*pEdcaParm->Aifsn[QID_AC_VO]; for TGn wifi test*/
		}
#endif /* CONFIG_STA_SUPPORT */
		RTMP_IO_WRITE32(pAd, WMM_AIFSN_CFG, AifsnCsr.word);

		memmove(&pAd->CommonCfg.APEdcaParm, pEdcaParm, sizeof(EDCA_PARM));
		if (!ADHOC_ON(pAd))
		{
			DBGPRINT(RT_DEBUG_TRACE,("EDCA [#%d]: AIFSN CWmin CWmax  TXOP(us)  ACM\n", pEdcaParm->EdcaUpdateCount));
			DBGPRINT(RT_DEBUG_TRACE,("     AC_BE      %2d     %2d     %2d      %4d     %d\n",
									 pEdcaParm->Aifsn[0],
									 pEdcaParm->Cwmin[0],
									 pEdcaParm->Cwmax[0],
									 pEdcaParm->Txop[0]<<5,
									 pEdcaParm->bACM[0]));
			DBGPRINT(RT_DEBUG_TRACE,("     AC_BK      %2d     %2d     %2d      %4d     %d\n",
									 pEdcaParm->Aifsn[1],
									 pEdcaParm->Cwmin[1],
									 pEdcaParm->Cwmax[1],
									 pEdcaParm->Txop[1]<<5,
									 pEdcaParm->bACM[1]));
			DBGPRINT(RT_DEBUG_TRACE,("     AC_VI      %2d     %2d     %2d      %4d     %d\n",
									 pEdcaParm->Aifsn[2],
									 pEdcaParm->Cwmin[2],
									 pEdcaParm->Cwmax[2],
									 pEdcaParm->Txop[2]<<5,
									 pEdcaParm->bACM[2]));
			DBGPRINT(RT_DEBUG_TRACE,("     AC_VO      %2d     %2d     %2d      %4d     %d\n",
									 pEdcaParm->Aifsn[3],
									 pEdcaParm->Cwmin[3],
									 pEdcaParm->Cwmax[3],
									 pEdcaParm->Txop[3]<<5,
									 pEdcaParm->bACM[3]));
		}

	}

	pAd->CommonCfg.RestoreBurstMode = Ac0Cfg.word;
}

/*
	==========================================================================
	Description:

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void 	AsicSetSlotTime(
	IN struct rtmp_adapter *pAd,
	IN bool bUseShortSlotTime)
{
	ULONG	SlotTime;
	u32	RegValue = 0;

#ifdef CONFIG_STA_SUPPORT
	if (pAd->CommonCfg.Channel > 14)
		bUseShortSlotTime = true;
#endif /* CONFIG_STA_SUPPORT */

	if (bUseShortSlotTime && OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_SHORT_SLOT_INUSED))
		return;
	else if ((!bUseShortSlotTime) && (!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_SHORT_SLOT_INUSED)))
		return;

	if (bUseShortSlotTime)
		OPSTATUS_SET_FLAG(pAd, fOP_STATUS_SHORT_SLOT_INUSED);
	else
		OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_SHORT_SLOT_INUSED);

	SlotTime = (bUseShortSlotTime)? 9 : 20;

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* force using short SLOT time for FAE to demo performance when TxBurst is ON*/
		if (((pAd->StaActive.SupportedPhyInfo.bHtEnable == false) && (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_WMM_INUSED)))
#ifdef DOT11_N_SUPPORT
			|| ((pAd->StaActive.SupportedPhyInfo.bHtEnable == true) && (pAd->CommonCfg.BACapability.field.Policy == BA_NOTUSE))
#endif /* DOT11_N_SUPPORT */
			)
		{
			/* In this case, we will think it is doing Wi-Fi test*/
			/* And we will not set to short slot when bEnableTxBurst is true.*/
		}
		else if (pAd->CommonCfg.bEnableTxBurst)
		{
			OPSTATUS_SET_FLAG(pAd, fOP_STATUS_SHORT_SLOT_INUSED);
			SlotTime = 9;
		}
	}
#endif /* CONFIG_STA_SUPPORT */


	/* For some reasons, always set it to short slot time.*/
	/* ToDo: Should consider capability with 11B*/
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if (pAd->StaCfg.BssType == BSS_ADHOC)
		{
			OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_SHORT_SLOT_INUSED);
			SlotTime = 20;
		}
	}
#endif /* CONFIG_STA_SUPPORT */

	mt7610u_read32(pAd, BKOFF_SLOT_CFG, &RegValue);
	RegValue = RegValue & 0xFFFFFF00;

	RegValue |= SlotTime;

	RTMP_IO_WRITE32(pAd, BKOFF_SLOT_CFG, RegValue);
}

/*
	========================================================================
	Description:
		Add Shared key information into ASIC.
		Update shared key, TxMic and RxMic to Asic Shared key table
		Update its cipherAlg to Asic Shared key Mode.

    Return:
	========================================================================
*/
void AsicAddSharedKeyEntry(
	IN struct rtmp_adapter *	pAd,
	IN u8 	 	BssIndex,
	IN u8 	 	KeyIdx,
	IN PCIPHER_KEY		pCipherKey)
{
	ULONG offset; /*, csr0;*/
	SHAREDKEY_MODE_STRUC csr1;
	UINT16 SharedKeyTableBase, SharedKeyModeBase;

	u8 *	pKey = pCipherKey->Key;
	u8 *	pTxMic = pCipherKey->TxMic;
	u8 *	pRxMic = pCipherKey->RxMic;
	u8 	CipherAlg = pCipherKey->CipherAlg;

	DBGPRINT(RT_DEBUG_TRACE, ("AsicAddSharedKeyEntry BssIndex=%d, KeyIdx=%d\n", BssIndex,KeyIdx));
/*============================================================================================*/

	DBGPRINT(RT_DEBUG_TRACE,("AsicAddSharedKeyEntry: %s key #%d\n", CipherName[CipherAlg], BssIndex*4 + KeyIdx));
	DBGPRINT_RAW(RT_DEBUG_TRACE, (" 	Key = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
		pKey[0],pKey[1],pKey[2],pKey[3],pKey[4],pKey[5],pKey[6],pKey[7],pKey[8],pKey[9],pKey[10],pKey[11],pKey[12],pKey[13],pKey[14],pKey[15]));
	if (pRxMic)
	{
		DBGPRINT_RAW(RT_DEBUG_TRACE, (" 	Rx MIC Key = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
			pRxMic[0],pRxMic[1],pRxMic[2],pRxMic[3],pRxMic[4],pRxMic[5],pRxMic[6],pRxMic[7]));
	}
	if (pTxMic)
	{
		DBGPRINT_RAW(RT_DEBUG_TRACE, (" 	Tx MIC Key = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
			pTxMic[0],pTxMic[1],pTxMic[2],pTxMic[3],pTxMic[4],pTxMic[5],pTxMic[6],pTxMic[7]));
	}
/*============================================================================================*/

	/* fill key material - key + TX MIC + RX MIC*/

	if (BssIndex >= 8)
	{
		SharedKeyTableBase = SHARED_KEY_TABLE_BASE_EXT;
		SharedKeyModeBase = SHARED_KEY_MODE_BASE_EXT;
		BssIndex -= 8;
	}
	else
	{
		SharedKeyTableBase = SHARED_KEY_TABLE_BASE;
		SharedKeyModeBase = SHARED_KEY_MODE_BASE;
	}

	offset = SharedKeyTableBase + (4*BssIndex + KeyIdx)*HW_KEY_ENTRY_SIZE;


#ifdef RTMP_MAC_USB
{
	RTUSBMultiWrite(pAd, offset, pKey, MAX_LEN_OF_SHARE_KEY);

	offset += MAX_LEN_OF_SHARE_KEY;
	if (pTxMic)
	{
		RTUSBMultiWrite(pAd, offset, pTxMic, 8);
	}

	offset += 8;
	if (pRxMic)
	{
		RTUSBMultiWrite(pAd, offset, pRxMic, 8);
	}
}
#endif /* RTMP_MAC_USB */


	/* Update cipher algorithm. WSTA always use BSS0*/
	mt7610u_read32(pAd, SharedKeyModeBase+4*(BssIndex/2), &csr1.word);
	DBGPRINT(RT_DEBUG_TRACE,("Read: SHARED_KEY_MODE_BASE at this Bss[%d] KeyIdx[%d]= 0x%x \n", BssIndex,KeyIdx, csr1.word));
	if ((BssIndex%2) == 0)
	{
		if (KeyIdx == 0)
			csr1.field.Bss0Key0CipherAlg = CipherAlg;
		else if (KeyIdx == 1)
			csr1.field.Bss0Key1CipherAlg = CipherAlg;
		else if (KeyIdx == 2)
			csr1.field.Bss0Key2CipherAlg = CipherAlg;
		else
			csr1.field.Bss0Key3CipherAlg = CipherAlg;
	}
	else
	{
		if (KeyIdx == 0)
			csr1.field.Bss1Key0CipherAlg = CipherAlg;
		else if (KeyIdx == 1)
			csr1.field.Bss1Key1CipherAlg = CipherAlg;
		else if (KeyIdx == 2)
			csr1.field.Bss1Key2CipherAlg = CipherAlg;
		else
			csr1.field.Bss1Key3CipherAlg = CipherAlg;
	}
	DBGPRINT(RT_DEBUG_TRACE,("Write: SHARED_KEY_MODE_BASE at this Bss[%d] = 0x%x \n", BssIndex, csr1.word));
	RTMP_IO_WRITE32(pAd, SharedKeyModeBase+4*(BssIndex/2), csr1.word);

}

/*	IRQL = DISPATCH_LEVEL*/
void AsicRemoveSharedKeyEntry(
	IN struct rtmp_adapter *pAd,
	IN u8 	 BssIndex,
	IN u8 	 KeyIdx)
{
	/*ULONG SecCsr0;*/
	SHAREDKEY_MODE_STRUC csr1;
	UINT16 SharedKeyTableBase, SharedKeyModeBase;

	DBGPRINT(RT_DEBUG_TRACE,("AsicRemoveSharedKeyEntry: #%d \n", BssIndex*4 + KeyIdx));

	if (BssIndex >= 8)
	{
		SharedKeyTableBase = SHARED_KEY_TABLE_BASE_EXT;
		SharedKeyModeBase = SHARED_KEY_MODE_BASE_EXT;
		BssIndex -= 8;
	}
	else
	{
		SharedKeyTableBase = SHARED_KEY_TABLE_BASE;
		SharedKeyModeBase = SHARED_KEY_MODE_BASE;
	}

	mt7610u_read32(pAd, SharedKeyTableBase+4*(BssIndex/2), &csr1.word);
	if ((BssIndex%2) == 0)
	{
		if (KeyIdx == 0)
			csr1.field.Bss0Key0CipherAlg = 0;
		else if (KeyIdx == 1)
			csr1.field.Bss0Key1CipherAlg = 0;
		else if (KeyIdx == 2)
			csr1.field.Bss0Key2CipherAlg = 0;
		else
			csr1.field.Bss0Key3CipherAlg = 0;
	}
	else
	{
		if (KeyIdx == 0)
			csr1.field.Bss1Key0CipherAlg = 0;
		else if (KeyIdx == 1)
			csr1.field.Bss1Key1CipherAlg = 0;
		else if (KeyIdx == 2)
			csr1.field.Bss1Key2CipherAlg = 0;
		else
			csr1.field.Bss1Key3CipherAlg = 0;
	}
	DBGPRINT(RT_DEBUG_TRACE,("Write: SHARED_KEY_MODE_BASE at this Bss[%d] = 0x%x \n", BssIndex, csr1.word));
	RTMP_IO_WRITE32(pAd, SharedKeyModeBase+4*(BssIndex/2), csr1.word);
	ASSERT(BssIndex < 4);
	ASSERT(KeyIdx < 4);

}

void AsicUpdateWCIDIVEIV(
	IN struct rtmp_adapter *pAd,
	IN USHORT		WCID,
	IN ULONG        uIV,
	IN ULONG        uEIV)
{
	ULONG	offset;

	offset = MAC_IVEIV_TABLE_BASE + (WCID * HW_IVEIV_ENTRY_SIZE);

	RTMP_IO_WRITE32(pAd, offset, uIV);
	RTMP_IO_WRITE32(pAd, offset + 4, uEIV);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: wcid(%d) 0x%08lx, 0x%08lx \n",
									__FUNCTION__, WCID, uIV, uEIV));
}


void AsicUpdateRxWCIDTable(
	IN struct rtmp_adapter *pAd,
	IN USHORT		WCID,
	IN u8 *       pAddr)
{
	ULONG offset;
	ULONG Addr;

	offset = MAC_WCID_BASE + (WCID * HW_WCID_ENTRY_SIZE);
	Addr = pAddr[0] + (pAddr[1] << 8) +(pAddr[2] << 16) +(pAddr[3] << 24);
	RTMP_IO_WRITE32(pAd, offset, Addr);
	Addr = pAddr[4] + (pAddr[5] << 8);
	RTMP_IO_WRITE32(pAd, offset + 4, Addr);
}


/*
	========================================================================
	Description:
		Add Client security information into ASIC WCID table and IVEIV table.
    Return:

    Note :
		The key table selection rule :
    	1.	Wds-links and Mesh-links always use Pair-wise key table.
		2. 	When the CipherAlg is TKIP, AES, SMS4 or the dynamic WEP is enabled,
			it needs to set key into Pair-wise Key Table.
		3.	The pair-wise key security mode is set NONE, it means as no security.
		4.	In STA Adhoc mode, it always use shared key table.
		5.	Otherwise, use shared key table

	========================================================================
*/
void AsicUpdateWcidAttributeEntry(
	IN	struct rtmp_adapter *pAd,
	IN	u8 		BssIdx,
	IN 	u8 	 	KeyIdx,
	IN 	u8 	 	CipherAlg,
	IN	u8			Wcid,
	IN	u8			KeyTabFlag)
{
	WCID_ATTRIBUTE_STRUC WCIDAttri;
	USHORT		offset;

	/* Initialize the content of WCID Attribue  */
	WCIDAttri.word = 0;

	/* The limitation of HW WCID table */
	if (/*Wcid < 1 ||*/ Wcid > 254)
	{
		DBGPRINT(RT_DEBUG_WARN, ("%s: Wcid is invalid (%d). \n",
										__FUNCTION__, Wcid));
		return;
	}

	/* Update the pairwise key security mode.
	   Use bit10 and bit3~1 to indicate the pairwise cipher mode */
	WCIDAttri.field.PairKeyModeExt = ((CipherAlg & 0x08) >> 3);
	WCIDAttri.field.PairKeyMode = (CipherAlg & 0x07);

	/* Update the MBSS index.
	   Use bit11 and bit6~4 to indicate the BSS index */
	WCIDAttri.field.BSSIdxExt = ((BssIdx & 0x08) >> 3);
	WCIDAttri.field.BSSIdx = (BssIdx & 0x07);


	/* Assign Key Table selection */
	WCIDAttri.field.KeyTab = KeyTabFlag;

	/* Update related information to ASIC */
	offset = MAC_WCID_ATTRIBUTE_BASE + (Wcid * HW_WCID_ATTRI_SIZE);
	RTMP_IO_WRITE32(pAd, offset, WCIDAttri.word);

	DBGPRINT(RT_DEBUG_TRACE, ("%s : WCID #%d, KeyIndex #%d, Alg=%s\n", __FUNCTION__, Wcid, KeyIdx, CipherName[CipherAlg]));
	DBGPRINT(RT_DEBUG_TRACE, ("		WCIDAttri = 0x%x \n", WCIDAttri.word));

}

/*
	==========================================================================
	Description:

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicDelWcidTab(struct rtmp_adapter*pAd, u8 wcid_idx)
{
	u32 offset;
	u8 cnt, cnt_s, cnt_e;
#ifdef MCS_LUT_SUPPORT
	u8 mcs_tb_offset = 0;
#endif /* MCS_LUT_SUPPORT */


	DBGPRINT(RT_DEBUG_TRACE, ("AsicDelWcidTab==>wcid_idx = 0x%x\n",wcid_idx));
	if (wcid_idx == WCID_ALL) {
		cnt_s = 0;
		cnt_e = (WCID_ALL - 1);
	} else {
#ifdef MCS_LUT_SUPPORT
		if (RTMP_TEST_MORE_FLAG(pAd, fASIC_CAP_MCS_LUT))
			mcs_tb_offset = 0x400;
#endif /* MCS_LUT_SUPPORT */
		cnt_s = cnt_e = wcid_idx;
	}

	for (cnt = cnt_s; cnt_s <= cnt_e; cnt_s++)
	{
		offset = MAC_WCID_BASE + cnt * HW_WCID_ENTRY_SIZE;
		RTMP_IO_WRITE32(pAd, offset, 0x0);
		RTMP_IO_WRITE32(pAd, offset + 4, 0x0);
#ifdef MCS_LUT_SUPPORT
		if (mcs_tb_offset) {
			offset += mcs_tb_offset;
			RTMP_IO_WRITE32(pAd, offset, 0x0);
			RTMP_IO_WRITE32(pAd, offset + 4, 0x0);
		}
#endif /* MCS_LUT_SUPPORT */
	}
}



/*
	========================================================================
	Description:
		Add Pair-wise key material into ASIC.
		Update pairwise key, TxMic and RxMic to Asic Pair-wise key table

    Return:
	========================================================================
*/
void AsicAddPairwiseKeyEntry(
	IN struct rtmp_adapter *	pAd,
	IN u8 		WCID,
	IN PCIPHER_KEY		pCipherKey)
{
	INT i;
	ULONG 		offset;
	u8 *	 pKey = pCipherKey->Key;
	u8 *	 pTxMic = pCipherKey->TxMic;
	u8 *	 pRxMic = pCipherKey->RxMic;
	u8 	CipherAlg = pCipherKey->CipherAlg;

	/* EKEY*/
	offset = PAIRWISE_KEY_TABLE_BASE + (WCID * HW_KEY_ENTRY_SIZE);
#ifdef RTMP_MAC_USB
	RTUSBMultiWrite(pAd, offset, &pCipherKey->Key[0], MAX_LEN_OF_PEER_KEY);
#endif /* RTMP_MAC_USB */
	for (i=0; i<MAX_LEN_OF_PEER_KEY; i+=4)
	{
		u32 Value;
		mt7610u_read32(pAd, offset + i, &Value);
	}

	offset += MAX_LEN_OF_PEER_KEY;

	/*  MIC KEY*/
	if (pTxMic)
	{
#ifdef RTMP_MAC_USB
		RTUSBMultiWrite(pAd, offset, &pCipherKey->TxMic[0], 8);
#endif /* RTMP_MAC_USB */
	}
	offset += 8;
	if (pRxMic)
	{
#ifdef RTMP_MAC_USB
		RTUSBMultiWrite(pAd, offset, &pCipherKey->RxMic[0], 8);
#endif /* RTMP_MAC_USB */
	}
	DBGPRINT(RT_DEBUG_TRACE,("AsicAddPairwiseKeyEntry: WCID #%d Alg=%s\n",WCID, CipherName[CipherAlg]));
	DBGPRINT(RT_DEBUG_TRACE,("	Key = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
		pKey[0],pKey[1],pKey[2],pKey[3],pKey[4],pKey[5],pKey[6],pKey[7],pKey[8],pKey[9],pKey[10],pKey[11],pKey[12],pKey[13],pKey[14],pKey[15]));
	if (pRxMic)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("	Rx MIC Key = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
			pRxMic[0],pRxMic[1],pRxMic[2],pRxMic[3],pRxMic[4],pRxMic[5],pRxMic[6],pRxMic[7]));
	}
	if (pTxMic)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("	Tx MIC Key = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
			pTxMic[0],pTxMic[1],pTxMic[2],pTxMic[3],pTxMic[4],pTxMic[5],pTxMic[6],pTxMic[7]));
	}
}

/*
	========================================================================
	Description:
		Remove Pair-wise key material from ASIC.

    Return:
	========================================================================
*/
void AsicRemovePairwiseKeyEntry(
	IN struct rtmp_adapter *pAd,
	IN u8 	 Wcid)
{
	/* Set the specific WCID attribute entry as OPEN-NONE */
	AsicUpdateWcidAttributeEntry(pAd,
							  BSS0,
							  0,
							  CIPHER_NONE,
							  Wcid,
							  PAIRWISEKEYTABLE);

	DBGPRINT(RT_DEBUG_TRACE, ("%s : Wcid #%d \n", __FUNCTION__, Wcid));
}

bool AsicSendCommandToMcu(
	IN struct rtmp_adapter*pAd,
	IN u8 Command,
	IN u8 Token,
	IN u8 Arg0,
	IN u8 Arg1,
	IN bool in_atomic)
{
#ifdef RT65xx
	// TODO: shiang-6590, fix me, currently firmware is not ready yet, so ignore it!
	if (IS_RT65XX(pAd))
		return true;
#endif /* RT65xx */

	if (pAd->chipOps.sendCommandToMcu)
		return pAd->chipOps.sendCommandToMcu(pAd, Command, Token, Arg0, Arg1, in_atomic);
	else
		return false;
}


bool AsicSendCommandToMcuBBP(
	IN struct rtmp_adapter *pAd,
	IN u8 	 Command,
	IN u8 	 Token,
	IN u8 	 Arg0,
	IN u8 	 Arg1,
	IN bool 	FlgIsNeedLocked)
{
#ifdef RT65xx
	// TODO: shiang-6590, fix me, currently firmware is not ready yet, so ignore it!
	if (IS_RT65XX(pAd)) {
		return true;
	}
#endif /* RT65xx */

	if (pAd->chipOps.sendCommandToMcu)
		return pAd->chipOps.sendCommandToMcu(pAd, Command, Token, Arg0, Arg1, FlgIsNeedLocked);
	else
		return false;
}

/*
	========================================================================
	Description:
		For 1x1 chipset : 2070 / 3070 / 3090 / 3370 / 3390 / 5370 / 5390
		Usage :	1. Set Default Antenna as initialize
				2. Antenna Diversity switching used
				3. iwpriv command switch Antenna

    Return:
	========================================================================
 */
void AsicSetRxAnt(
	IN struct rtmp_adapter *pAd,
	IN u8 		Ant)
{
	if (pAd->chipOps.SetRxAnt)
		pAd->chipOps.SetRxAnt(pAd, Ant);
}


void AsicTurnOffRFClk(
	IN struct rtmp_adapter *pAd,
	IN	u8 	Channel)
{

		switch (pAd->RfIcType)
		{
			default:
				DBGPRINT(RT_DEBUG_TRACE, ("AsicTurnOffRFClk#%d : Unkonwn RFIC=%d\n",
											Channel, pAd->RfIcType));
				break;
		}
}

#ifdef DOT11_N_SUPPORT
/*
	==========================================================================
	Description:

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicEnableRalinkBurstMode(
	IN struct rtmp_adapter *pAd)
{
	u32				Data = 0;

	mt7610u_read32(pAd, EDCA_AC0_CFG, &Data);
	pAd->CommonCfg.RestoreBurstMode = Data;
	Data  &= 0xFFF00000;
	Data  |= 0x86380;
	RTMP_IO_WRITE32(pAd, EDCA_AC0_CFG, Data);
}

/*
	==========================================================================
	Description:

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicDisableRalinkBurstMode(
	IN struct rtmp_adapter *pAd)
{
	u32				Data = 0;

	mt7610u_read32(pAd, EDCA_AC0_CFG, &Data);

	Data = pAd->CommonCfg.RestoreBurstMode;
	Data &= 0xFFFFFF00;

	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_DYNAMIC_BE_TXOP_ACTIVE)
#ifdef DOT11_N_SUPPORT
		&& (pAd->MacTab.fAnyStationMIMOPSDynamic == false)
#endif // DOT11_N_SUPPORT //
	)
	{
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RDG_ACTIVE))
			Data |= 0x80;
		else if (pAd->CommonCfg.bEnableTxBurst)
			Data |= 0x20;
	}
	RTMP_IO_WRITE32(pAd, EDCA_AC0_CFG, Data);
}
#endif // DOT11_N_SUPPORT //

INT AsicSetPreTbttInt(struct rtmp_adapter*pAd, bool enable)
{
	u32 val;

	mt7610u_read32(pAd, INT_TIMER_CFG, &val);
	val &= 0xffff0000;
	val |= 6 << 4; /* Pre-TBTT is 6ms before TBTT interrupt. 1~10 ms is reasonable. */
	RTMP_IO_WRITE32(pAd, INT_TIMER_CFG, val);
	/* Enable pre-tbtt interrupt */
	mt7610u_read32(pAd, INT_TIMER_EN, &val);
	val |=0x1;
	RTMP_IO_WRITE32(pAd, INT_TIMER_EN, val);

	return true;
}


bool AsicWaitPDMAIdle(struct rtmp_adapter *pAd, INT round, INT wait_us)
{
	INT i = 0;
	WPDMA_GLO_CFG_STRUC GloCfg;


	do {
		mt7610u_read32(pAd, WPDMA_GLO_CFG, &GloCfg.word);
		if ((GloCfg.field.TxDMABusy == 0)  && (GloCfg.field.RxDMABusy == 0)) {
			DBGPRINT(RT_DEBUG_TRACE, ("==>  DMAIdle, GloCfg=0x%x\n", GloCfg.word));
			return true;
		}
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
			return false;
		RTMPusecDelay(wait_us);
	}while ((i++) < round);

	DBGPRINT(RT_DEBUG_TRACE, ("==>  DMABusy, GloCfg=0x%x\n", GloCfg.word));

	return false;
}


#ifdef DOT11_N_SUPPORT
#if defined(RT65xx)
#define MAX_AGG_CNT	32
#elif defined(RT2883) || defined(RT3883)
#define MAX_AGG_CNT	16
#else
#define MAX_AGG_CNT	8
#endif
INT AsicReadAggCnt(struct rtmp_adapter*pAd, ULONG *aggCnt, int cnt_len)
{
	u32 reg_addr;
	TX_AGG_CNT_STRUC reg_val;
	int i, cnt, seg;
	static USHORT aggReg[] = {
						TX_AGG_CNT, TX_AGG_CNT3,
#if MAX_AGG_CNT > 8
						TX_AGG_CNT4, TX_AGG_CNT7,
#endif
#if MAX_AGG_CNT > 16
						TX_AGG_CNT8, TX_AGG_CNT15,
#endif
	};


	memset(aggCnt, 0, cnt_len * sizeof(ULONG));
	seg = (sizeof(aggReg) /sizeof(USHORT));

	cnt = 0;
	for (i = 0; i < seg; i += 2)
	{
		for (reg_addr = aggReg[i] ; reg_addr <= aggReg[i+1] ; reg_addr += 4)
		{
			mt7610u_read32(pAd, reg_addr, &reg_val.word);
			if (cnt < (cnt_len -1)) {
				aggCnt[cnt] = reg_val.field.AggCnt_x;
				aggCnt[cnt+1] = reg_val.field.AggCnt_y;
				DBGPRINT(RT_DEBUG_TRACE, ("%s():Get AggSize at Reg(0x%x) with val(0x%08x) [AGG_%d=>%ld, AGG_%d=>%ld]\n",
						__FUNCTION__, reg_addr, reg_val.word, cnt, aggCnt[cnt], cnt+1, aggCnt[cnt+1]));
				cnt += 2;
			} else {
				DBGPRINT(RT_DEBUG_TRACE, ("%s():Get AggSize at Reg(0x%x) failed, no enough buffer(cnt_len=%d, cnt=%d)\n",
							__FUNCTION__, reg_addr, cnt_len, cnt));
			}
		}
	}

	return true;
}

#endif /* DOT11_N_SUPPORT */


INT AsicSetChannel(struct rtmp_adapter*pAd, u8 ch, u8 bw, u8 ext_ch, bool bScan)
{
	rtmp_bbp_set_bw(pAd, bw);

	/*  Tx/RX : control channel setting */
	rtmp_bbp_set_ctrlch(pAd, ext_ch);
	rtmp_mac_set_ctrlch(pAd, ext_ch);

	/* Let BBP register at 20MHz to do scan */
	AsicSwitchChannel(pAd, ch, bScan);
	AsicLockChannel(pAd, ch);

	return 0;
}

/*
	==========================================================================
	Description:
		Set BSSID of Root AP

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicSetApCliBssid(
	IN struct rtmp_adapter *pAd,
	IN u8 *pBssid,
	IN u8 index)
{
	u32		  Addr4 = 0;

	DBGPRINT(RT_DEBUG_TRACE, ("===> AsicSetApCliBssid %x:%x:%x:%x:%x:%x\n",
				PRINT_MAC(pBssid)));

	Addr4 = (u32)(pBssid[0]) |
			(u32)(pBssid[1] << 8)  |
			(u32)(pBssid[2] << 16) |
			(u32)(pBssid[3] << 24);
	RTMP_IO_WRITE32(pAd, MAC_APCLI_BSSID_DW0, Addr4);

	Addr4 = 0;
	Addr4 = (ULONG)(pBssid[4]) | (ULONG)(pBssid[5] << 8);
	/* Enable APCLI mode */
	Addr4 |= 0x10000;

	RTMP_IO_WRITE32(pAd, MAC_APCLI_BSSID_DW1, Addr4);
}

#ifdef MAC_REPEATER_SUPPORT
/*
	==========================================================================
	Description:
		Set Extended MAC Address

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicSetExtendedMacAddr(
	IN struct rtmp_adapter *pAd,
	IN u8 *pMacAddr,
	IN u32 Idx)
{
	u32		  Addr4 = 0;

	DBGPRINT(RT_DEBUG_TRACE, ("===> AsicSetExtendedMacAddr %x:%x:%x:%x:%x:%x\n",
				PRINT_MAC(pMacAddr)));

	Addr4 = (u32)(pMacAddr[0]) |
			(u32)(pMacAddr[1] << 8)  |
			(u32)(pMacAddr[2] << 16) |
			(u32)(pMacAddr[3] << 24);
	RTMP_IO_WRITE32(pAd, MAC_ADDR_EXT0_31_0 + (Idx*8), Addr4);

	Addr4 = 0;
	Addr4 = (ULONG)(pMacAddr[4]) | (ULONG)(pMacAddr[5] << 8);
	/* Enable APCLI mode */
	Addr4 |= 0x10000;

	RTMP_IO_WRITE32(pAd, MAC_ADDR_EXT0_47_32 + (Idx*8), Addr4);
}
#endif /* MAC_REPEATER_SUPPORT */

