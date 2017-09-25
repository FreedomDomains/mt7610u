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


INT	Set_Cmm_WirelessMode_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg,
	IN	bool 		FlgIsDiffMbssModeUsed)
{
	INT	success = true;
	success = RT_CfgSetWirelessMode(pAd, arg);

	if (success)
	{

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			/* clean up previous SCAN result */
			BssTableInit(&pAd->ScanTab);
			pAd->StaCfg.LastScanTime = 0;

			RTMPSetPhyMode(pAd, pAd->CommonCfg.PhyMode);
			/* Set AdhocMode rates*/
			if (pAd->StaCfg.BssType == BSS_ADHOC)
			{
				MlmeUpdateTxRates(pAd, false, 0);
				MakeIbssBeacon(pAd);           /* re-build BEACON frame*/
				AsicEnableIbssSync(pAd);       /* copy to on-chip memory*/
			}
		}
#endif /* CONFIG_STA_SUPPORT */


	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Set_WirelessMode_Proc::parameters out of range\n"));
	}

	return success;
}



/*
    ==========================================================================
    Description:
        Set Wireless Mode
    Return:
        true if all parameters are OK, false otherwise
    ==========================================================================
*/
INT	Set_WirelessMode_Proc(struct rtmp_adapter*pAd, char *arg)
{
	return Set_Cmm_WirelessMode_Proc(pAd, arg, 0);
}




/*
    ==========================================================================
    Description:
        Set Channel
    Return:
        true if all parameters are OK, false otherwise
    ==========================================================================
*/
INT	Set_Channel_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
 	INT		success = true;
	u8 Channel;

	Channel = (u8) simple_strtol(arg, 0, 10);

	/* check if this channel is valid*/
	if (ChannelSanity(pAd, Channel) == true)
	{
#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			pAd->CommonCfg.Channel = Channel;
			/* Save the channel on MlmeAux for CntlOidRTBssidProc used. */
			pAd->MlmeAux.Channel = Channel;

			if (MONITOR_ON(pAd))
			{
				u8 rf_channel;

				N_ChannelCheck(pAd);
				if (WMODE_CAP_N(pAd->CommonCfg.PhyMode) &&
					pAd->CommonCfg.RegTransmitSetting.field.BW == BW_40)
					rf_channel = N_SetCenCh(pAd, pAd->CommonCfg.Channel);
				else
					rf_channel = pAd->CommonCfg.Channel;

				AsicSwitchChannel(pAd, rf_channel, false);
				AsicLockChannel(pAd, rf_channel);
				DBGPRINT(RT_DEBUG_TRACE, ("%s(): CtrlChannel(%d), CentralChannel(%d) \n",
							__FUNCTION__, pAd->CommonCfg.Channel,
							pAd->CommonCfg.CentralChannel));
			}
		}
#endif /* CONFIG_STA_SUPPORT */
		success = true;
	}
	else
	{

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		success = false;
		DBGPRINT(RT_DEBUG_WARN,("This channel is out of channel list, nothing to do!\n "));
#endif /* CONFIG_STA_SUPPORT */
	}


	if (success == true)
		DBGPRINT(RT_DEBUG_TRACE, ("Set_Channel_Proc::(Channel=%d)\n", pAd->CommonCfg.Channel));

	return success;
}




#ifdef AGGREGATION_SUPPORT
/*
    ==========================================================================
    Description:
        Set TxBurst
    Return:
        true if all parameters are OK, false otherwise
    ==========================================================================
*/
INT	Set_PktAggregate_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	LONG aggre;

	aggre = simple_strtol(arg, 0, 10);

	if (aggre == 1)
		pAd->CommonCfg.bAggregationCapable = true;
	else if (aggre == 0)
		pAd->CommonCfg.bAggregationCapable = false;
	else
		return false;  /*Invalid argument */


	DBGPRINT(RT_DEBUG_TRACE, ("Set_PktAggregate_Proc::(AGGRE=%d)\n", pAd->CommonCfg.bAggregationCapable));

	return true;
}
#endif

/*
    ==========================================================================
    Description:
        Set IEEE80211H.
        This parameter is 1 when needs radar detection, otherwise 0
    Return:
        true if all parameters are OK, false otherwise
    ==========================================================================
*/
INT	Set_IEEE80211H_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
    LONG ieee80211h;

	ieee80211h = simple_strtol(arg, 0, 10);

	if (ieee80211h == 1)
		pAd->CommonCfg.bIEEE80211H = true;
	else if (ieee80211h == 0)
		pAd->CommonCfg.bIEEE80211H = false;
	else
		return false;  /*Invalid argument */

	DBGPRINT(RT_DEBUG_TRACE, ("Set_IEEE80211H_Proc::(IEEE80211H=%d)\n", pAd->CommonCfg.bIEEE80211H));

	return true;
}

#ifdef EXT_BUILD_CHANNEL_LIST
/*
    ==========================================================================
    Description:
        Set Country Code.
    Return:
        true if all parameters are OK, false otherwise
    ==========================================================================
*/
INT Set_ExtCountryCode_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{

	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) == NDIS_STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s can only be used when interface is down.\n", __FUNCTION__));
		return true;
	}

	if(strlen(arg) == 2)
	{
		memmove(pAd->CommonCfg.CountryCode, arg, 2);
		pAd->CommonCfg.bCountryFlag = true;
	}
	else
	{
		memset(pAd->CommonCfg.CountryCode, 0, 3);
		pAd->CommonCfg.bCountryFlag = false;
	}

	{
		u8 CountryCode[3] = {0};
		memmove(CountryCode, pAd->CommonCfg.CountryCode, 2);
		DBGPRINT(RT_DEBUG_TRACE, ("Set_CountryCode_Proc::(bCountryFlag=%d, CountryCode=%s)\n",
							pAd->CommonCfg.bCountryFlag,
							CountryCode));
	}
	return true;
}
/*
    ==========================================================================
    Description:
        Set Ext DFS Type
    Return:
        true if all parameters are OK, false otherwise
    ==========================================================================
*/
INT Set_ExtDfsType_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	u8 *pDfsType = &pAd->CommonCfg.DfsType;
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) == NDIS_STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s can only be used when interface is down.\n", __FUNCTION__));
		return true;
	}

	if (!strcmp(arg, "CE"))
		*pDfsType = CE;
	else if (!strcmp(arg, "FCC"))
		*pDfsType = FCC;
	else if (!strcmp(arg, "JAP"))
		*pDfsType = JAP;
	else
		DBGPRINT(RT_DEBUG_TRACE, ("Unsupported DFS type:%s (Legal types are: CE, FCC, JAP)\n", arg));

	return true;
}

/*
    ==========================================================================
    Description:
        Add new channel list
    Return:
        true if all parameters are OK, false otherwise
    ==========================================================================
*/
INT Set_ChannelListAdd_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	CH_DESP		inChDesp;
	PCH_REGION pChRegion = NULL;

	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) == NDIS_STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s can only be used when interface is down.\n", __FUNCTION__));
		return true;
	}

	/* Get Channel Region (CountryCode)*/
	{
		INT loop = 0;

		while (strcmp((char *) ChRegion[loop].CountReg, "") != 0)
		{
			if (strncmp((char *) ChRegion[loop].CountReg, pAd->CommonCfg.CountryCode, 2) == 0)
			{
				pChRegion = &ChRegion[loop];
				break;
			}
			loop++;
		}
		if (pChRegion == NULL)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("CountryCode is not configured or not valid\n"));
			return true;
		}
	}

	/* Parsing the arg, IN:arg; OUT:inChRegion */
	{
		u8 strBuff[64], count = 0;
		u8 *pStart, *pEnd, *tempIdx, *tempBuff[5];

		if (strlen(arg) <64)
			memcpy(strBuff, arg, strlen(arg));

		if ((pStart = rtstrchr(strBuff, '[')) != NULL)
		{
			if ((pEnd = rtstrchr(pStart++, ']')) != NULL)
			{
				tempBuff[count++] = pStart;
				for(tempIdx = pStart ;tempIdx != pEnd; tempIdx++)
				{
					if(*tempIdx == ',')
					{
						*tempIdx = '\0';
						tempBuff[count++] = ++tempIdx;
					}
				}
				*(pEnd) = '\0';

				if (count != 5)
				{
					DBGPRINT(RT_DEBUG_TRACE, ("Input Error. Too more or too less parameters.\n"));
					return true;
				}
				else
				{
					inChDesp.FirstChannel = (u8) simple_strtol(tempBuff[0], 0, 10);
					inChDesp.NumOfCh = (u8) simple_strtol(tempBuff[1], 0, 10);
					inChDesp.MaxTxPwr = (u8) simple_strtol(tempBuff[2], 0, 10);
					inChDesp.Geography = (!strcmp(tempBuff[3], "BOTH") ? BOTH: (!strcmp(tempBuff[3], "IDOR") ? IDOR : ODOR));
					inChDesp.DfsReq= (!strcmp(tempBuff[4], "true") ? true : false);
				}
			}
			else
			{
				DBGPRINT(RT_DEBUG_TRACE, ("Missing End \"]\"\n"));
				return true;
			}
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE, ("%s: Invalid input format.\n", __FUNCTION__));
			return true;
		}
	}

	/* Add entry to Channel List*/
	{
		u8 EntryIdx;
		PCH_DESP pChDesp = NULL;
		u8 CountryCode[3] = {0};
		if (pAd->CommonCfg.pChDesp == NULL) {
			pAd->CommonCfg.pChDesp =
				kmalloc(MAX_PRECONFIG_DESP_ENTRY_SIZE*sizeof(CH_DESP), GFP_ATOMIC);
			pChDesp = (PCH_DESP) pAd->CommonCfg.pChDesp;
			if (pChDesp) {
				for (EntryIdx= 0; pChRegion->pChDesp[EntryIdx].FirstChannel != 0; EntryIdx++)
				{
					if (EntryIdx == (MAX_PRECONFIG_DESP_ENTRY_SIZE-2)) /* Keep an NULL entry in the end of table*/
					{
						DBGPRINT(RT_DEBUG_TRACE, ("Table is full.\n"));
						return true;
					}
					memcpy(&pChDesp[EntryIdx], &pChRegion->pChDesp[EntryIdx], sizeof(CH_DESP));
				}
				/* Copy the NULL entry*/
				memcpy(&pChDesp[EntryIdx], &pChRegion->pChDesp[EntryIdx], sizeof(CH_DESP));
			}
			else
			{
				DBGPRINT(RT_DEBUG_ERROR, ("os_alloc_mem failded.\n"));
				return false;
			}
		}
		else
		{
			pChDesp = (PCH_DESP) pAd->CommonCfg.pChDesp;
			for (EntryIdx= 0; pChDesp[EntryIdx].FirstChannel != 0; EntryIdx++)
			{
				if(EntryIdx ==  (MAX_PRECONFIG_DESP_ENTRY_SIZE-2)) /* Keep an NULL entry in the end of table*/
				{
					DBGPRINT(RT_DEBUG_TRACE, ("Table is full.\n"));
					return true;
				}
			}
		}
		memmove(CountryCode, pAd->CommonCfg.CountryCode, 2);
		DBGPRINT(RT_DEBUG_TRACE, ("Add channel lists {%u, %u, %u, %s, %s} to %s.\n",
							inChDesp.FirstChannel,
							inChDesp.NumOfCh,
							inChDesp.MaxTxPwr,
							(inChDesp.Geography == BOTH) ? "BOTH" : (inChDesp.Geography == IDOR) ?  "IDOR" : "ODOR",
							(inChDesp.DfsReq == true) ? "true" : "false",
							CountryCode));
		memcpy(&pChDesp[EntryIdx], &inChDesp, sizeof(CH_DESP));
		pChDesp[++EntryIdx].FirstChannel = 0;
	}
	return true;
}

INT Set_ChannelListShow_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	PCH_REGION	pChRegion = NULL;
	u8 	EntryIdx, CountryCode[3]={0};

	/* Get Channel Region (CountryCode)*/
	{
		INT loop = 0;

		while (strcmp((char *) ChRegion[loop].CountReg, "") != 0)
		{
			if (strncmp((char *) ChRegion[loop].CountReg, pAd->CommonCfg.CountryCode, 2) == 0)
			{
				pChRegion = &ChRegion[loop];
				break;
			}
			loop++;
		}
		if (pChRegion == NULL)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("CountryCode is not configured or not valid\n"));
			return true;
		}
	}

	memmove(CountryCode, pAd->CommonCfg.CountryCode, 2);
	if (pAd->CommonCfg.DfsType == MAX_RD_REGION)
		pAd->CommonCfg.DfsType = pChRegion->DfsType;
	DBGPRINT(RT_DEBUG_ERROR, ("=========================================\n"));
	DBGPRINT(RT_DEBUG_ERROR, ("CountryCode:%s\n", CountryCode));
	DBGPRINT(RT_DEBUG_ERROR, ("DfsType:%s\n",
					(pAd->CommonCfg.DfsType == JAP) ? "JAP" :
					((pAd->CommonCfg.DfsType == FCC) ? "FCC" : "CE" )));

	if (pAd->CommonCfg.pChDesp != NULL)
	{
		PCH_DESP pChDesp = (PCH_DESP) pAd->CommonCfg.pChDesp;
		for (EntryIdx = 0; pChDesp[EntryIdx].FirstChannel != 0; EntryIdx++)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%u. {%3u, %2u, %2u, %s, %5s}.\n",
						EntryIdx,
						pChDesp[EntryIdx].FirstChannel,
						pChDesp[EntryIdx].NumOfCh,
						pChDesp[EntryIdx].MaxTxPwr,
						(pChDesp[EntryIdx].Geography == BOTH) ? "BOTH" : (pChDesp[EntryIdx].Geography == IDOR) ?  "IDOR" : "ODOR",
						(pChDesp[EntryIdx].DfsReq == true) ? "true" : "false"));
		}
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Default channel list table:\n"));
		for (EntryIdx = 0; pChRegion->pChDesp[EntryIdx].FirstChannel != 0; EntryIdx++)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%u. {%3u, %2u, %2u, %s, %5s}.\n",
						EntryIdx,
						pChRegion->pChDesp[EntryIdx].FirstChannel,
						pChRegion->pChDesp[EntryIdx].NumOfCh,
						pChRegion->pChDesp[EntryIdx].MaxTxPwr,
						(pChRegion->pChDesp[EntryIdx].Geography == BOTH) ? "BOTH" : (pChRegion->pChDesp[EntryIdx].Geography == IDOR) ?  "IDOR" : "ODOR",
						(pChRegion->pChDesp[EntryIdx].DfsReq == true) ? "true" : "false"));
		}
	}
	DBGPRINT(RT_DEBUG_ERROR, ("=========================================\n"));
	return true;
}

INT Set_ChannelListDel_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	u8 EntryIdx, TargetIdx, NumOfEntry;
	PCH_REGION	pChRegion = NULL;
	PCH_DESP pChDesp = NULL;
	TargetIdx = simple_strtol(arg, 0, 10);
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) == NDIS_STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s can only be used when interface is down.\n", __FUNCTION__));
		return true;
	}

	/* Get Channel Region (CountryCode)*/
	{
		INT loop = 0;
		while (strcmp((char *) ChRegion[loop].CountReg, "") != 0)
		{
			if (strncmp((char *) ChRegion[loop].CountReg, pAd->CommonCfg.CountryCode, 2) == 0)
			{
				pChRegion = &ChRegion[loop];
				break;
			}
			loop++;
		}
		if (pChRegion == NULL)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("CountryCode is not configured or not valid\n"));
			return true;
		}
	}

	if (pAd->CommonCfg.pChDesp == NULL) {
		pAd->CommonCfg.pChDesp =
			kmalloc(MAX_PRECONFIG_DESP_ENTRY_SIZE*sizeof(CH_DESP), GFP_ATOMIC);
		if (pAd->CommonCfg.pChDesp) {
			pChDesp = (PCH_DESP) pAd->CommonCfg.pChDesp;
			for (EntryIdx= 0; pChRegion->pChDesp[EntryIdx].FirstChannel != 0; EntryIdx++)
			{
				if (EntryIdx == (MAX_PRECONFIG_DESP_ENTRY_SIZE-2)) /* Keep an NULL entry in the end of table*/
				{
					DBGPRINT(RT_DEBUG_TRACE, ("Table is full.\n"));
					return true;
				}
				memcpy(&pChDesp[EntryIdx], &pChRegion->pChDesp[EntryIdx], sizeof(CH_DESP));
			}
			/* Copy the NULL entry*/
			memcpy(&pChDesp[EntryIdx], &pChRegion->pChDesp[EntryIdx], sizeof(CH_DESP));
		}
		else
		{
			DBGPRINT(RT_DEBUG_ERROR, ("os_alloc_mem failded.\n"));
			return false;
		}
	}
	else
		pChDesp = (PCH_DESP) pAd->CommonCfg.pChDesp;

	if (!strcmp(arg, "default"))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Default table used.\n" ));
		if (pAd->CommonCfg.pChDesp != NULL)
			kfree(pAd->CommonCfg.pChDesp);
		pAd->CommonCfg.pChDesp = NULL;
		pAd->CommonCfg.DfsType = MAX_RD_REGION;
	}
	else if (!strcmp(arg, "all"))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Remove all entries.\n" ));
		for (EntryIdx = 0; EntryIdx < MAX_PRECONFIG_DESP_ENTRY_SIZE; EntryIdx++)
			memset(&pChDesp[EntryIdx], 0, sizeof(CH_DESP));
	}
	else if (TargetIdx < (MAX_PRECONFIG_DESP_ENTRY_SIZE-1))
	{
		for (EntryIdx= 0; pChDesp[EntryIdx].FirstChannel != 0; EntryIdx++)
		{
			if(EntryIdx ==  (MAX_PRECONFIG_DESP_ENTRY_SIZE-2)) /* Keep an NULL entry in the end of table */
			{
				DBGPRINT(RT_DEBUG_TRACE, ("Last entry should be NULL.\n"));
				pChDesp[EntryIdx].FirstChannel = 0;
				return true;
			}
		}
		NumOfEntry = EntryIdx;
		if (TargetIdx >= NumOfEntry)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("Out of table range.\n"));
			return true;
		}
		for (EntryIdx = TargetIdx; EntryIdx < NumOfEntry; EntryIdx++)
			memcpy(&pChDesp[EntryIdx], &pChDesp[EntryIdx+1], sizeof(CH_DESP));
		memset(&pChDesp[EntryIdx], 0, sizeof(CH_DESP)); /*NULL entry*/
		DBGPRINT(RT_DEBUG_TRACE, ("Entry %u deleted.\n", TargetIdx));
	}
	else
		DBGPRINT(RT_DEBUG_TRACE, ("Entry not found.\n"));

	return true;
}
#endif /* EXT_BUILD_CHANNEL_LIST  */


#ifdef DBG
/*
    ==========================================================================
    Description:
        For Debug information
    Return:
        true if all parameters are OK, false otherwise
    ==========================================================================
*/
INT	Set_Debug_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	DBGPRINT(RT_DEBUG_TRACE, ("==>%s()\n", __FUNCTION__));

    if(simple_strtol(arg, 0, 10) <= RT_DEBUG_LOUD)
        RTDebugLevel = simple_strtol(arg, 0, 10);

	DBGPRINT(RT_DEBUG_TRACE, ("<==%s(RTDebugLevel = %d)\n",
				__FUNCTION__, RTDebugLevel));

	return true;
}


/*
    ==========================================================================
    Description:
        For DebugFunc information
    Return:
        true if all parameters are OK, false otherwise
    ==========================================================================
*/
INT	Set_DebugFunc_Proc(
	IN struct rtmp_adapter*pAd,
	IN char *arg)
{
	DBGPRINT_S(RT_DEBUG_TRACE, ("==>%s()\n", __FUNCTION__));
	RTDebugFunc = simple_strtol(arg, 0, 10);
	DBGPRINT_S(RT_DEBUG_TRACE, ("Set RTDebugFunc = 0x%x\n", RTDebugFunc));

	return true;
}
#endif


INT	Show_DescInfo_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{

	return true;
}

/*
    ==========================================================================
    Description:
        Reset statistics counter

    Arguments:
        pAd            Pointer to our adapter
        arg

    Return:
        true if all parameters are OK, false otherwise
    ==========================================================================
*/
INT	Set_ResetStatCounter_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	/*u8           i;*/
	/*MAC_TABLE_ENTRY *pEntry;*/

	DBGPRINT(RT_DEBUG_TRACE, ("==>Set_ResetStatCounter_Proc\n"));

	/* add the most up-to-date h/w raw counters into software counters*/
	NICUpdateRawCounters(pAd);

	memset(&pAd->WlanCounters, 0, sizeof(COUNTER_802_11));
	memset(&pAd->Counters8023, 0, sizeof(COUNTER_802_3));
	memset(&pAd->RalinkCounters, 0, sizeof(COUNTER_RALINK));

	return true;
}


bool RTMPCheckStrPrintAble(
    IN  CHAR *pInPutStr,
    IN  u8 strLen)
{
    u8 i=0;

    for (i=0; i<strLen; i++)
    {
        if ((pInPutStr[i] < 0x20) ||
            (pInPutStr[i] > 0x7E))
            return false;
    }

    return true;
}

/*
	========================================================================

	Routine Description:
		Remove WPA Key process

	Arguments:
		pAd 					Pointer to our adapter
		pBuf							Pointer to the where the key stored

	Return Value:
		NDIS_SUCCESS					Add key successfully

	IRQL = DISPATCH_LEVEL

	Note:

	========================================================================
*/
#ifdef CONFIG_STA_SUPPORT
void    RTMPSetDesiredRates(
    IN  struct rtmp_adapter *  pAdapter,
    IN  LONG            Rates)
{
    NDIS_802_11_RATES aryRates;

    memset(&aryRates, 0x00, sizeof(NDIS_802_11_RATES));
    switch (pAdapter->CommonCfg.PhyMode)
    {
        case (WMODE_A): /* A only*/
            switch (Rates)
            {
                case 6000000: /*6M*/
                    aryRates[0] = 0x0c; /* 6M*/
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_0;
                    break;
                case 9000000: /*9M*/
                    aryRates[0] = 0x12; /* 9M*/
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_1;
                    break;
                case 12000000: /*12M*/
                    aryRates[0] = 0x18; /* 12M*/
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_2;
                    break;
                case 18000000: /*18M*/
                    aryRates[0] = 0x24; /* 18M*/
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_3;
                    break;
                case 24000000: /*24M*/
                    aryRates[0] = 0x30; /* 24M*/
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_4;
                    break;
                case 36000000: /*36M*/
                    aryRates[0] = 0x48; /* 36M*/
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_5;
                    break;
                case 48000000: /*48M*/
                    aryRates[0] = 0x60; /* 48M*/
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_6;
                    break;
                case 54000000: /*54M*/
                    aryRates[0] = 0x6c; /* 54M*/
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_7;
                    break;
                case -1: /*Auto*/
                default:
                    aryRates[0] = 0x6c; /* 54Mbps*/
                    aryRates[1] = 0x60; /* 48Mbps*/
                    aryRates[2] = 0x48; /* 36Mbps*/
                    aryRates[3] = 0x30; /* 24Mbps*/
                    aryRates[4] = 0x24; /* 18M*/
                    aryRates[5] = 0x18; /* 12M*/
                    aryRates[6] = 0x12; /* 9M*/
                    aryRates[7] = 0x0c; /* 6M*/
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_AUTO;
                    break;
            }
            break;
        case (WMODE_B | WMODE_G): /* B/G Mixed*/
        case (WMODE_B): /* B only*/
        case (WMODE_A | WMODE_B | WMODE_G): /* A/B/G Mixed*/
        default:
            switch (Rates)
            {
                case 1000000: /*1M*/
                    aryRates[0] = 0x02;
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_0;
                    break;
                case 2000000: /*2M*/
                    aryRates[0] = 0x04;
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_1;
                    break;
                case 5000000: /*5.5M*/
                    aryRates[0] = 0x0b; /* 5.5M*/
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_2;
                    break;
                case 11000000: /*11M*/
                    aryRates[0] = 0x16; /* 11M*/
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_3;
                    break;
                case 6000000: /*6M*/
                    aryRates[0] = 0x0c; /* 6M*/
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_0;
                    break;
                case 9000000: /*9M*/
                    aryRates[0] = 0x12; /* 9M*/
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_1;
                    break;
                case 12000000: /*12M*/
                    aryRates[0] = 0x18; /* 12M*/
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_2;
                    break;
                case 18000000: /*18M*/
                    aryRates[0] = 0x24; /* 18M*/
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_3;
                    break;
                case 24000000: /*24M*/
                    aryRates[0] = 0x30; /* 24M*/
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_4;
                    break;
                case 36000000: /*36M*/
                    aryRates[0] = 0x48; /* 36M*/
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_5;
                    break;
                case 48000000: /*48M*/
                    aryRates[0] = 0x60; /* 48M*/
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_6;
                    break;
                case 54000000: /*54M*/
                    aryRates[0] = 0x6c; /* 54M*/
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_7;
                    break;
                case -1: /*Auto*/
                default:
                    if (pAdapter->CommonCfg.PhyMode == WMODE_B)
                    { /*B Only*/
                        aryRates[0] = 0x16; /* 11Mbps*/
                        aryRates[1] = 0x0b; /* 5.5Mbps*/
                        aryRates[2] = 0x04; /* 2Mbps*/
                        aryRates[3] = 0x02; /* 1Mbps*/
                    }
                    else
                    { /*(B/G) Mixed or (A/B/G) Mixed*/
                        aryRates[0] = 0x6c; /* 54Mbps*/
                        aryRates[1] = 0x60; /* 48Mbps*/
                        aryRates[2] = 0x48; /* 36Mbps*/
                        aryRates[3] = 0x30; /* 24Mbps*/
                        aryRates[4] = 0x16; /* 11Mbps*/
                        aryRates[5] = 0x0b; /* 5.5Mbps*/
                        aryRates[6] = 0x04; /* 2Mbps*/
                        aryRates[7] = 0x02; /* 1Mbps*/
                    }
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_AUTO;
                    break;
            }
            break;
    }

    memset(pAdapter->CommonCfg.DesireRate, 0, MAX_LEN_OF_SUPPORTED_RATES);
    memmove(pAdapter->CommonCfg.DesireRate, &aryRates, sizeof(NDIS_802_11_RATES));
    DBGPRINT(RT_DEBUG_TRACE, (" RTMPSetDesiredRates (%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x)\n",
        pAdapter->CommonCfg.DesireRate[0],pAdapter->CommonCfg.DesireRate[1],
        pAdapter->CommonCfg.DesireRate[2],pAdapter->CommonCfg.DesireRate[3],
        pAdapter->CommonCfg.DesireRate[4],pAdapter->CommonCfg.DesireRate[5],
        pAdapter->CommonCfg.DesireRate[6],pAdapter->CommonCfg.DesireRate[7] ));
    /* Changing DesiredRate may affect the MAX TX rate we used to TX frames out*/
    MlmeUpdateTxRates(pAdapter, false, 0);
}
#endif /* CONFIG_STA_SUPPORT */

#if defined(CONFIG_STA_SUPPORT) || defined(APCLI_WPA_SUPPLICANT_SUPPORT)
int RTMPWPARemoveKeyProc(
	IN	struct rtmp_adapter *pAd,
	IN	void *		pBuf)
{
	PNDIS_802_11_REMOVE_KEY pKey;
	ULONG					KeyIdx;
	int 			Status = NDIS_STATUS_FAILURE;
	bool 	bTxKey; 		/* Set the key as transmit key*/
	bool 	bPairwise;		/* Indicate the key is pairwise key*/
	bool 	bKeyRSC;		/* indicate the receive  SC set by KeyRSC value.*/
								/* Otherwise, it will set by the NIC.*/
	bool 	bAuthenticator; /* indicate key is set by authenticator.*/
	INT 		i;
#ifdef APCLI_SUPPORT
#ifdef APCLI_WPA_SUPPLICANT_SUPPORT
	u8 ifIndex;
	bool apcliEn=false;
	INT 		idx, BssIdx;
	struct os_cookie *pObj = pAd->OS_Cookie;
#endif/*APCLI_WPA_SUPPLICANT_SUPPORT*/
#endif/*APCLI_SUPPORT*/
	DBGPRINT(RT_DEBUG_TRACE,("---> RTMPWPARemoveKeyProc\n"));

	pKey = (PNDIS_802_11_REMOVE_KEY) pBuf;
	KeyIdx = pKey->KeyIndex & 0xff;
	/* Bit 31 of Add-key, Tx Key*/
	bTxKey		   = (pKey->KeyIndex & 0x80000000) ? true : false;
	/* Bit 30 of Add-key PairwiseKey*/
	bPairwise	   = (pKey->KeyIndex & 0x40000000) ? true : false;
	/* Bit 29 of Add-key KeyRSC*/
	bKeyRSC 	   = (pKey->KeyIndex & 0x20000000) ? true : false;
	/* Bit 28 of Add-key Authenticator*/
	bAuthenticator = (pKey->KeyIndex & 0x10000000) ? true : false;

	/* 1. If bTx is true, return failure information*/
	if (bTxKey == true)
		return(NDIS_STATUS_INVALID_DATA);

	/* 2. Check Pairwise Key*/
	if (bPairwise)
	{
		/* a. If BSSID is broadcast, remove all pairwise keys.*/
		/* b. If not broadcast, remove the pairwise specified by BSSID*/
		for (i = 0; i < SHARE_KEY_NUM; i++)
		{
#ifdef APCLI_SUPPORT
#ifdef APCLI_WPA_SUPPLICANT_SUPPORT
			if (pObj->ioctl_if_type == INT_APCLI)
			{
				/*if (MAC_ADDR_EQUAL(pAd->ApCfg.ApCliTab[ifIndex].SharedKey[i].BssId, pKey->BSSID)) */
				{
					ifIndex = pObj->ioctl_if;
					BssIdx = pAd->ApCfg.BssidNum + MAX_MESH_NUM + ifIndex;
					DBGPRINT(RT_DEBUG_TRACE,("APCLI RTMPWPARemoveKeyProc(KeyIdx=%d)\n", i));
					pAd->ApCfg.ApCliTab[ifIndex].SharedKey[i].KeyLen = 0;
					pAd->ApCfg.ApCliTab[ifIndex].SharedKey[i].CipherAlg = CIPHER_NONE;
					AsicRemoveSharedKeyEntry(pAd, BssIdx, (u8)i);
					Status = NDIS_STATUS_SUCCESS;
					break;
				}
			}
			else
#endif/*APCLI_WPA_SUPPLICANT_SUPPORT*/
#endif/*APCLI_SUPPORT*/
			{
#ifdef CONFIG_STA_SUPPORT
				if (MAC_ADDR_EQUAL(pAd->SharedKey[BSS0][i].BssId, pKey->BSSID))
				{
					DBGPRINT(RT_DEBUG_TRACE,("RTMPWPARemoveKeyProc(KeyIdx=%d)\n", i));
					pAd->SharedKey[BSS0][i].KeyLen = 0;
					pAd->SharedKey[BSS0][i].CipherAlg = CIPHER_NONE;
					AsicRemoveSharedKeyEntry(pAd, BSS0, (u8)i);
					Status = NDIS_STATUS_SUCCESS;
					break;
				}
#endif/*CONFIG_STA_SUPPORT*/
			}
		}
	}
	/* 3. Group Key*/
	else
	{
		/* a. If BSSID is broadcast, remove all group keys indexed*/
		/* b. If BSSID matched, delete the group key indexed.*/
		DBGPRINT(RT_DEBUG_TRACE,("RTMPWPARemoveKeyProc(KeyIdx=%ld)\n", KeyIdx));
		pAd->SharedKey[BSS0][KeyIdx].KeyLen = 0;
		pAd->SharedKey[BSS0][KeyIdx].CipherAlg = CIPHER_NONE;
		AsicRemoveSharedKeyEntry(pAd, BSS0, (u8)KeyIdx);
		Status = NDIS_STATUS_SUCCESS;
	}

	return (Status);
}
#endif /* defined(CONFIG_STA_SUPPORT) || defined(APCLI_WPA_SUPPLICANT_SUPPORT) */


#ifdef CONFIG_STA_SUPPORT
/*
	========================================================================

	Routine Description:
		Remove All WPA Keys

	Arguments:
		pAd 					Pointer to our adapter

	Return Value:
		None

	IRQL = DISPATCH_LEVEL

	Note:

	========================================================================
*/
void RTMPWPARemoveAllKeys(
	IN	struct rtmp_adapter *pAd)
{

	u8 	i;

	DBGPRINT(RT_DEBUG_TRACE,("RTMPWPARemoveAllKeys(AuthMode=%d, WepStatus=%d)\n", pAd->StaCfg.AuthMode, pAd->StaCfg.WepStatus));
	/*
		For WEP/CKIP, there is no need to remove it, since WinXP won't set it
		again after Link up. And it will be replaced if user changed it.
	*/
	if (pAd->StaCfg.AuthMode < Ndis802_11AuthModeWPA)
		return;

	/* For WPA-None, there is no need to remove it, since WinXP won't set it again after*/
	/* Link up. And it will be replaced if user changed it.*/
	if (pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPANone)
		return;

	/* set BSSID wcid entry of the Pair-wise Key table as no-security mode*/
	AsicRemovePairwiseKeyEntry(pAd, BSSID_WCID);

	/* set all shared key mode as no-security. */
	for (i = 0; i < SHARE_KEY_NUM; i++)
    {
		DBGPRINT(RT_DEBUG_TRACE,("remove %s key #%d\n", CipherName[pAd->SharedKey[BSS0][i].CipherAlg], i));
		memset(&pAd->SharedKey[BSS0][i], 0, sizeof(CIPHER_KEY));

		AsicRemoveSharedKeyEntry(pAd, BSS0, i);
	}

}
#endif /* CONFIG_STA_SUPPORT */


/*
	========================================================================
	Routine Description:
		Change NIC PHY mode. Re-association may be necessary

	Arguments:
		pAd - Pointer to our adapter
		phymode  -

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	========================================================================
*/
void RTMPSetPhyMode(
	IN struct rtmp_adapter*pAd,
	IN ULONG phymode)
{
	INT i;
	/* the selected phymode must be supported by the RF IC encoded in E2PROM*/

	pAd->CommonCfg.PhyMode = (u8)phymode;

	DBGPRINT(RT_DEBUG_TRACE,("RTMPSetPhyMode : PhyMode=%d, channel=%d \n", pAd->CommonCfg.PhyMode, pAd->CommonCfg.Channel));
#ifdef EXT_BUILD_CHANNEL_LIST
	BuildChannelListEx(pAd);
#else
	BuildChannelList(pAd);
#endif /* EXT_BUILD_CHANNEL_LIST */

	/* sanity check user setting*/
	for (i = 0; i < pAd->ChannelListNum; i++)
	{
		if (pAd->CommonCfg.Channel == pAd->ChannelList[i].Channel)
			break;
	}

	if (i == pAd->ChannelListNum)
	{
#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
			pAd->CommonCfg.Channel = FirstChannel(pAd);
#endif /* CONFIG_STA_SUPPORT */
		DBGPRINT(RT_DEBUG_ERROR, ("RTMPSetPhyMode: channel is out of range, use first channel=%d \n", pAd->CommonCfg.Channel));
	}

	memset(pAd->CommonCfg.SupRate, 0, MAX_LEN_OF_SUPPORTED_RATES);
	memset(pAd->CommonCfg.ExtRate, 0, MAX_LEN_OF_SUPPORTED_RATES);
	memset(pAd->CommonCfg.DesireRate, 0, MAX_LEN_OF_SUPPORTED_RATES);
	switch (phymode) {
		case (WMODE_B):
			pAd->CommonCfg.SupRate[0]  = 0x82;	  /* 1 mbps, in units of 0.5 Mbps, basic rate*/
			pAd->CommonCfg.SupRate[1]  = 0x84;	  /* 2 mbps, in units of 0.5 Mbps, basic rate*/
			pAd->CommonCfg.SupRate[2]  = 0x8B;	  /* 5.5 mbps, in units of 0.5 Mbps, basic rate*/
			pAd->CommonCfg.SupRate[3]  = 0x96;	  /* 11 mbps, in units of 0.5 Mbps, basic rate*/
			pAd->CommonCfg.SupRateLen  = 4;
			pAd->CommonCfg.ExtRateLen  = 0;
			pAd->CommonCfg.DesireRate[0]  = 2;	   /* 1 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[1]  = 4;	   /* 2 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[2]  = 11;    /* 5.5 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[3]  = 22;    /* 11 mbps, in units of 0.5 Mbps*/
			/*pAd->CommonCfg.HTPhyMode.field.MODE = MODE_CCK;  This MODE is only FYI. not use*/
			break;

		/*
			In current design, we will put supported/extended rate element in
			beacon even we are 11n-only mode.
			Or some 11n stations will not connect to us if we do not put
			supported/extended rate element in beacon.
		*/
		case (WMODE_G):
		case (WMODE_B | WMODE_G):
		case (WMODE_A | WMODE_B | WMODE_G):
		case (WMODE_GN):
		case (WMODE_A | WMODE_B | WMODE_G | WMODE_GN | WMODE_AN):
		case (WMODE_B | WMODE_G | WMODE_GN):
		case (WMODE_G | WMODE_GN):
			pAd->CommonCfg.SupRate[0]  = 0x82;	  /* 1 mbps, in units of 0.5 Mbps, basic rate*/
			pAd->CommonCfg.SupRate[1]  = 0x84;	  /* 2 mbps, in units of 0.5 Mbps, basic rate*/
			pAd->CommonCfg.SupRate[2]  = 0x8B;	  /* 5.5 mbps, in units of 0.5 Mbps, basic rate*/
			pAd->CommonCfg.SupRate[3]  = 0x96;	  /* 11 mbps, in units of 0.5 Mbps, basic rate*/
			pAd->CommonCfg.SupRate[4]  = 0x12;	  /* 9 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.SupRate[5]  = 0x24;	  /* 18 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.SupRate[6]  = 0x48;	  /* 36 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.SupRate[7]  = 0x6c;	  /* 54 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.SupRateLen  = 8;
			pAd->CommonCfg.ExtRate[0]  = 0x0C;	  /* 6 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.ExtRate[1]  = 0x18;	  /* 12 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.ExtRate[2]  = 0x30;	  /* 24 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.ExtRate[3]  = 0x60;	  /* 48 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.ExtRateLen  = 4;
			pAd->CommonCfg.DesireRate[0]  = 2;	   /* 1 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[1]  = 4;	   /* 2 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[2]  = 11;    /* 5.5 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[3]  = 22;    /* 11 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[4]  = 12;    /* 6 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[5]  = 18;    /* 9 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[6]  = 24;    /* 12 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[7]  = 36;    /* 18 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[8]  = 48;    /* 24 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[9]  = 72;    /* 36 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[10] = 96;    /* 48 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[11] = 108;   /* 54 mbps, in units of 0.5 Mbps*/
			break;

		case (WMODE_A):
		case (WMODE_A | WMODE_AN):
		case (WMODE_A | WMODE_G | WMODE_GN | WMODE_AN):
		case (WMODE_AN):
		case (WMODE_A | WMODE_AN | WMODE_AC):
		case (WMODE_AN | WMODE_AC):
			pAd->CommonCfg.SupRate[0]  = 0x8C;	  /* 6 mbps, in units of 0.5 Mbps, basic rate*/
			pAd->CommonCfg.SupRate[1]  = 0x12;	  /* 9 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.SupRate[2]  = 0x98;	  /* 12 mbps, in units of 0.5 Mbps, basic rate*/
			pAd->CommonCfg.SupRate[3]  = 0x24;	  /* 18 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.SupRate[4]  = 0xb0;	  /* 24 mbps, in units of 0.5 Mbps, basic rate*/
			pAd->CommonCfg.SupRate[5]  = 0x48;	  /* 36 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.SupRate[6]  = 0x60;	  /* 48 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.SupRate[7]  = 0x6c;	  /* 54 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.SupRateLen  = 8;
			pAd->CommonCfg.ExtRateLen  = 0;
			pAd->CommonCfg.DesireRate[0]  = 12;    /* 6 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[1]  = 18;    /* 9 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[2]  = 24;    /* 12 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[3]  = 36;    /* 18 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[4]  = 48;    /* 24 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[5]  = 72;    /* 36 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[6]  = 96;    /* 48 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[7]  = 108;   /* 54 mbps, in units of 0.5 Mbps*/
			/*pAd->CommonCfg.HTPhyMode.field.MODE = MODE_OFDM;  This MODE is only FYI. not use*/
			break;

		default:
			break;
	}



	SetCommonHT(pAd);

	SetCommonVHT(pAd);
}


/*
    ==========================================================================
    Description:
        Parse encryption type
Arguments:
    pAdapter                    Pointer to our adapter
    wrq                         Pointer to the ioctl argument

    Return Value:
        None

    Note:
    ==========================================================================
*/
char *GetEncryptType(CHAR enc)
{
	if(enc == Ndis802_11WEPDisabled)
		return "NONE";
	if(enc == Ndis802_11WEPEnabled)
		return "WEP";
	if(enc == Ndis802_11Encryption2Enabled)
		return "TKIP";
	if(enc == Ndis802_11Encryption3Enabled)
		return "AES";
	if(enc == Ndis802_11Encryption4Enabled)
		return "TKIPAES";

	return "UNKNOW";
}

char *GetAuthMode(CHAR auth)
{
	if(auth == Ndis802_11AuthModeOpen)
		return "OPEN";
	if(auth == Ndis802_11AuthModeShared)
		return "SHARED";
	if(auth == Ndis802_11AuthModeAutoSwitch)
		return "AUTOWEP";
	if(auth == Ndis802_11AuthModeWPA)
		return "WPA";
	if(auth == Ndis802_11AuthModeWPAPSK)
		return "WPAPSK";
	if(auth == Ndis802_11AuthModeWPANone)
		return "WPANONE";
	if(auth == Ndis802_11AuthModeWPA2)
		return "WPA2";
	if(auth == Ndis802_11AuthModeWPA2PSK)
		return "WPA2PSK";
	if(auth == Ndis802_11AuthModeWPA1WPA2)
		return "WPA1WPA2";
	if(auth == Ndis802_11AuthModeWPA1PSKWPA2PSK)
		return "WPA1PSKWPA2PSK";

	return "UNKNOW";
}


/*
    ==========================================================================
    Description:
        Get site survey results
	Arguments:
	    pAdapter                    Pointer to our adapter
	    wrq                         Pointer to the ioctl argument

    Return Value:
        None

    Note:
        Usage:
        		1.) UI needs to wait 4 seconds after issue a site survey command
        		2.) iwpriv ra0 get_site_survey
        		3.) UI needs to prepare at least 4096bytes to get the results
    ==========================================================================
*/
#define	LINE_LEN	(4+33+20+23+9+7+7+3)	/* Channel+SSID+Bssid+Security+Signal+WiressMode+ExtCh+NetworkType*/

#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */
void RTMPCommSiteSurveyData(
	IN  char *	msg,
	IN  PBSS_ENTRY	pBss,
	IN  u32		MsgLen)
{
	INT         Rssi = 0;
	UINT        Rssi_Quality = 0;
	NDIS_802_11_NETWORK_TYPE    wireless_mode;
	CHAR		Ssid[MAX_LEN_OF_SSID +1];
	STRING		SecurityStr[32] = {0};
	NDIS_802_11_ENCRYPTION_STATUS	ap_cipher = Ndis802_11EncryptionDisabled;
	NDIS_802_11_AUTHENTICATION_MODE	ap_auth_mode = Ndis802_11AuthModeOpen;

		/*Channel*/
		sprintf(msg+strlen(msg),"%-4d", pBss->Channel);


		/*SSID*/
	memset(Ssid, 0, (MAX_LEN_OF_SSID +1));
	if (RTMPCheckStrPrintAble((char *)pBss->Ssid, pBss->SsidLen))
		memmove(Ssid, pBss->Ssid, pBss->SsidLen);
	else
	{
		INT idx = 0;
		sprintf(Ssid, "0x");
		for (idx = 0; (idx < 14) && (idx < pBss->SsidLen); idx++)
			sprintf(Ssid + 2 + (idx*2), "%02X", (u8)pBss->Ssid[idx]);
	}
		sprintf(msg+strlen(msg),"%-33s", Ssid);

		/*BSSID*/
		sprintf(msg+strlen(msg),"%02x:%02x:%02x:%02x:%02x:%02x   ",
			pBss->Bssid[0],
			pBss->Bssid[1],
			pBss->Bssid[2],
			pBss->Bssid[3],
			pBss->Bssid[4],
			pBss->Bssid[5]);

	/*Security*/
	memset(SecurityStr, 0, 32);
	if ((Ndis802_11AuthModeWPA <= pBss->AuthMode) &&
		(pBss->AuthMode <= Ndis802_11AuthModeWPA1PSKWPA2PSK))
	{
		if (pBss->AuthMode == Ndis802_11AuthModeWPANone)
		{
			ap_auth_mode = pBss->AuthMode;
				ap_cipher = pBss->WPA.PairCipher;
		}
		else if (pBss->AuthModeAux == Ndis802_11AuthModeOpen)
		{
			ap_auth_mode = pBss->AuthMode;
			if ((ap_auth_mode == Ndis802_11AuthModeWPA) ||
				(ap_auth_mode == Ndis802_11AuthModeWPAPSK))
			{
				if (pBss->WPA.PairCipherAux == Ndis802_11WEPDisabled)
					ap_cipher = pBss->WPA.PairCipher;
				else
					ap_cipher = Ndis802_11Encryption4Enabled;
			}
			else if ((ap_auth_mode == Ndis802_11AuthModeWPA2) ||
					 (ap_auth_mode == Ndis802_11AuthModeWPA2PSK))
			{
				if (pBss->WPA2.PairCipherAux == Ndis802_11WEPDisabled)
					ap_cipher = pBss->WPA2.PairCipher;
				else
					ap_cipher = Ndis802_11Encryption4Enabled;
			}
		}
		else if ((pBss->AuthMode == Ndis802_11AuthModeWPAPSK) ||
				 (pBss->AuthMode == Ndis802_11AuthModeWPA2PSK))
		{
			if ((pBss->AuthModeAux == Ndis802_11AuthModeWPAPSK) ||
				(pBss->AuthModeAux == Ndis802_11AuthModeWPA2PSK))
				ap_auth_mode = Ndis802_11AuthModeWPA1PSKWPA2PSK;
			else
				ap_auth_mode = pBss->AuthMode;

			if (pBss->WPA.PairCipher != pBss->WPA2.PairCipher)
				ap_cipher = Ndis802_11Encryption4Enabled;
			else if ((pBss->WPA.PairCipher == pBss->WPA2.PairCipher) &&
					 (pBss->WPA.PairCipherAux != pBss->WPA2.PairCipherAux))
				ap_cipher = Ndis802_11Encryption4Enabled;
			else if ((pBss->WPA.PairCipher == pBss->WPA2.PairCipher) &&
					 (pBss->WPA.PairCipherAux == pBss->WPA2.PairCipherAux) &&
					 (pBss->WPA.PairCipherAux != Ndis802_11WEPDisabled))
				ap_cipher = Ndis802_11Encryption4Enabled;
			else if ((pBss->WPA.PairCipher == pBss->WPA2.PairCipher) &&
					 (pBss->WPA.PairCipherAux == pBss->WPA2.PairCipherAux) &&
					 (pBss->WPA.PairCipherAux == Ndis802_11WEPDisabled))
				ap_cipher = pBss->WPA.PairCipher;
		}
		else if ((pBss->AuthMode == Ndis802_11AuthModeWPA) ||
				 (pBss->AuthMode == Ndis802_11AuthModeWPA2))
		{
			if ((pBss->AuthModeAux == Ndis802_11AuthModeWPA) ||
				(pBss->AuthModeAux == Ndis802_11AuthModeWPA2))
				ap_auth_mode = Ndis802_11AuthModeWPA1WPA2;
			else
				ap_auth_mode = pBss->AuthMode;

			if (pBss->WPA.PairCipher != pBss->WPA2.PairCipher)
				ap_cipher = Ndis802_11Encryption4Enabled;
			else if ((pBss->WPA.PairCipher == pBss->WPA2.PairCipher) &&
					 (pBss->WPA.PairCipherAux != pBss->WPA2.PairCipherAux))
				ap_cipher = Ndis802_11Encryption4Enabled;
			else if ((pBss->WPA.PairCipher == pBss->WPA2.PairCipher) &&
					 (pBss->WPA.PairCipherAux == pBss->WPA2.PairCipherAux) &&
					 (pBss->WPA.PairCipherAux != Ndis802_11WEPDisabled))
				ap_cipher = Ndis802_11Encryption4Enabled;
			else if ((pBss->WPA.PairCipher == pBss->WPA2.PairCipher) &&
					 (pBss->WPA.PairCipherAux == pBss->WPA2.PairCipherAux) &&
					 (pBss->WPA.PairCipherAux == Ndis802_11WEPDisabled))
				ap_cipher = pBss->WPA.PairCipher;
		}

		sprintf(SecurityStr, "%s/%s", GetAuthMode((CHAR)ap_auth_mode), GetEncryptType((CHAR)ap_cipher));
	}
	else
	{
		ap_auth_mode = pBss->AuthMode;
		ap_cipher = pBss->WepStatus;
		if (ap_cipher == Ndis802_11WEPDisabled)
			sprintf(SecurityStr, "NONE");
		else if (ap_cipher == Ndis802_11WEPEnabled)
			sprintf(SecurityStr, "WEP");
		else
			sprintf(SecurityStr, "%s/%s", GetAuthMode((CHAR)ap_auth_mode), GetEncryptType((CHAR)ap_cipher));
	}

	sprintf(msg+strlen(msg), "%-23s", SecurityStr);

		/* Rssi*/
		Rssi = (INT)pBss->Rssi;
		if (Rssi >= -50)
			Rssi_Quality = 100;
		else if (Rssi >= -80)    /* between -50 ~ -80dbm*/
			Rssi_Quality = (UINT)(24 + ((Rssi + 80) * 26)/10);
		else if (Rssi >= -90)   /* between -80 ~ -90dbm*/
			Rssi_Quality = (UINT)(((Rssi + 90) * 26)/10);
		else    /* < -84 dbm*/
			Rssi_Quality = 0;
		sprintf(msg+strlen(msg),"%-9d", Rssi_Quality);

		/* Wireless Mode*/
		wireless_mode = NetworkTypeInUseSanity(pBss);
		if (wireless_mode == Ndis802_11FH ||
			wireless_mode == Ndis802_11DS)
			sprintf(msg+strlen(msg),"%-7s", "11b");
		else if (wireless_mode == Ndis802_11OFDM5)
			sprintf(msg+strlen(msg),"%-7s", "11a");
		else if (wireless_mode == Ndis802_11OFDM5_N)
			sprintf(msg+strlen(msg),"%-7s", "11a/n");
		else if (wireless_mode == Ndis802_11OFDM24)
			sprintf(msg+strlen(msg),"%-7s", "11b/g");
		else if (wireless_mode == Ndis802_11OFDM24_N)
			sprintf(msg+strlen(msg),"%-7s", "11b/g/n");
		else
			sprintf(msg+strlen(msg),"%-7s", "unknow");

		/* Ext Channel*/
		if (pBss->AddHtInfoLen > 0)
		{
			if (pBss->AddHtInfo.AddHtInfo.ExtChanOffset == EXTCHA_ABOVE)
				sprintf(msg+strlen(msg),"%-7s", " ABOVE");
			else if (pBss->AddHtInfo.AddHtInfo.ExtChanOffset == EXTCHA_BELOW)
				sprintf(msg+strlen(msg),"%-7s", " BELOW");
			else
				sprintf(msg+strlen(msg),"%-7s", " NONE");
		}
		else
		{
			sprintf(msg+strlen(msg),"%-7s", " NONE");
		}

		/*Network Type		*/
		if (pBss->BssType == BSS_ADHOC)
			sprintf(msg+strlen(msg),"%-3s", " Ad");
		else
			sprintf(msg+strlen(msg),"%-3s", " In");

        sprintf(msg+strlen(msg),"\n");

	return;
}

#define	MAC_LINE_LEN	(1+14+4+4+4+4+10+10+10+6+6)	/* "\n"+Addr+aid+psm+datatime+rxbyte+txbyte+current tx rate+last tx rate+"\n" */
void RTMPIoctlGetMacTable(
	IN struct rtmp_adapter *pAd,
	IN RTMP_IOCTL_INPUT_STRUCT *wrq)
{
	INT i;
/*	RT_802_11_MAC_TABLE MacTab;*/
	RT_802_11_MAC_TABLE *pMacTab = NULL;
	RT_802_11_MAC_ENTRY *pDst;
	MAC_TABLE_ENTRY *pEntry;
	char *msg;

	/* allocate memory */
	pMacTab = kmalloc(sizeof(RT_802_11_MAC_TABLE), GFP_ATOMIC);
	if (!pMacTab) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Allocate memory fail!!!\n", __FUNCTION__));
		return;
	}

	memset(pMacTab, 0, sizeof(RT_802_11_MAC_TABLE));
	pMacTab->Num = 0;
	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
	{
		pEntry = &(pAd->MacTab.Content[i]);

		if (IS_ENTRY_CLIENT(pEntry) && (pEntry->Sst == SST_ASSOC))
		{
			pDst = &pMacTab->Entry[pMacTab->Num];


			pDst->ApIdx = (u8)pEntry->apidx;
			memcpy(pDst->Addr, &pEntry->Addr, ETH_ALEN);
			pDst->Aid = (u8)pEntry->Aid;
			pDst->Psm = pEntry->PsMode;
			pDst->MimoPs = pEntry->MmpsMode;

			/* Fill in RSSI per entry*/
			pDst->AvgRssi0 = pEntry->RssiSample.AvgRssi0;
			pDst->AvgRssi1 = pEntry->RssiSample.AvgRssi1;
			pDst->AvgRssi2 = pEntry->RssiSample.AvgRssi2;

			/* the connected time per entry*/
			pDst->ConnectedTime = pEntry->StaConnectTime;
			pDst->TxRate.word = pEntry->HTPhyMode.word;


			pMacTab->Num += 1;
		}
	}

	wrq->u.data.length = sizeof(RT_802_11_MAC_TABLE);
	if (copy_to_user(wrq->u.data.pointer, pMacTab, wrq->u.data.length))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s: copy_to_user() fail\n", __FUNCTION__));
	}


/*	msg = kmalloc(sizeof(CHAR)*(MAX_LEN_OF_MAC_TABLE*MAC_LINE_LEN), MEM_ALLOC_FLAG);*/
	msg = kmalloc(sizeof(CHAR)*(MAX_LEN_OF_MAC_TABLE*MAC_LINE_LEN), GFP_ATOMIC);
	if (!msg) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s():Alloc memory failed\n", __FUNCTION__));
		goto LabelOK;
	}
	memset(msg, 0 ,MAX_LEN_OF_MAC_TABLE*MAC_LINE_LEN );
	sprintf(msg,"%s","\n");
	sprintf(msg+strlen(msg),"%-14s%-4s%-4s%-4s%-4s%-6s%-6s%-10s%-10s%-10s\n",
		"MAC", "AP",  "AID", "PSM", "AUTH", "CTxR", "LTxR","LDT", "RxB", "TxB");

	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
	{
		PMAC_TABLE_ENTRY pEntry = &pAd->MacTab.Content[i];

		if (IS_ENTRY_CLIENT(pEntry) && (pEntry->Sst == SST_ASSOC))
		{
			if((strlen(msg)+MAC_LINE_LEN ) >= (MAX_LEN_OF_MAC_TABLE*MAC_LINE_LEN) )
				break;
			sprintf(msg+strlen(msg),"%02x%02x%02x%02x%02x%02x  ",
				pEntry->Addr[0], pEntry->Addr[1], pEntry->Addr[2],
				pEntry->Addr[3], pEntry->Addr[4], pEntry->Addr[5]);
			sprintf(msg+strlen(msg),"%-4d", (int)pEntry->apidx);
			sprintf(msg+strlen(msg),"%-4d", (int)pEntry->Aid);
			sprintf(msg+strlen(msg),"%-4d", (int)pEntry->PsMode);
			sprintf(msg+strlen(msg),"%-4d", (int)pEntry->AuthState);
			sprintf(msg+strlen(msg),"%-6d",RateIdToMbps[pAd->MacTab.Content[i].CurrTxRate]);
			sprintf(msg+strlen(msg),"%-6d",0/*RateIdToMbps[pAd->MacTab.Content[i].HTPhyMode.word]*/); /* ToDo*/
			sprintf(msg+strlen(msg),"%-10d",0/*pAd->MacTab.Content[i].HSCounter.LastDataPacketTime*/); /* ToDo*/
			sprintf(msg+strlen(msg),"%-10d",0/*pAd->MacTab.Content[i].HSCounter.TotalRxByteCount*/); /* ToDo*/
			sprintf(msg+strlen(msg),"%-10d\n",0/*pAd->MacTab.Content[i].HSCounter.TotalTxByteCount*/); /* ToDo*/

		}
	}
	/* for compatible with old API just do the printk to console*/

	DBGPRINT(RT_DEBUG_TRACE, ("%s", msg));
/*	kfree(msg);*/
	kfree(msg);

LabelOK:
	if (pMacTab != NULL)
		kfree(pMacTab);
}

#ifdef INF_AR9
#ifdef AR9_MAPI_SUPPORT
#endif/*AR9_MAPI_SUPPORT*/
#endif/* INF_AR9 */

INT	Set_BASetup_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
    u8 mac[6], tid;
	char *token;
	STRING sepValue[] = ":", DASH = '-';
	INT i;
    MAC_TABLE_ENTRY *pEntry;

/*
	The BASetup inupt string format should be xx:xx:xx:xx:xx:xx-d,
		=>The six 2 digit hex-decimal number previous are the Mac address,
		=>The seventh decimal number is the tid value.
*/
	/*DBGPRINT(RT_DEBUG_TRACE,("\n%s\n", arg));*/

	if(strlen(arg) < 19)  /*Mac address acceptable format 01:02:03:04:05:06 length 17 plus the "-" and tid value in decimal format.*/
		return false;

	token = strchr(arg, DASH);
	if ((token != NULL) && (strlen(token)>1))
	{
		tid = (u8) simple_strtol((token+1), 0, 10);
		/* tid is 0 ~ 7; Or kernel will crash in BAOriSessionSetUp() */
		if (tid > (NUM_OF_TID-1))
			return false;

		*token = '\0';
		for (i = 0, token = rstrtok(arg, &sepValue[0]); token; token = rstrtok(NULL, &sepValue[0]), i++)
		{
			if((strlen(token) != 2) || (!isxdigit(*token)) || (!isxdigit(*(token+1))))
				return false;
			AtoH(token, (&mac[i]), 1);
		}
		if(i != 6)
			return false;

		DBGPRINT(RT_DEBUG_OFF, ("\n%02x:%02x:%02x:%02x:%02x:%02x-%02x\n",
								mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], tid));

	    pEntry = MacTableLookup(pAd, (u8 *) mac);

    	if (pEntry) {
        	DBGPRINT(RT_DEBUG_OFF, ("\nSetup BA Session: Tid = %d\n", tid));
	        BAOriSessionSetUp(pAd, pEntry, tid, 0, 100, true);
    	}

		return true;
	}

	return false;

}

INT	Set_BADecline_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	ULONG bBADecline;

	bBADecline = simple_strtol(arg, 0, 10);

	if (bBADecline == 0)
	{
		pAd->CommonCfg.bBADecline = false;
	}
	else if (bBADecline == 1)
	{
		pAd->CommonCfg.bBADecline = true;
	}
	else
	{
		return false; /*Invalid argument*/
	}

	DBGPRINT(RT_DEBUG_TRACE, ("Set_BADecline_Proc::(BADecline=%d)\n", pAd->CommonCfg.bBADecline));

	return true;
}

INT	Set_BAOriTearDown_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
    u8 mac[6], tid;
	char *token;
	STRING sepValue[] = ":", DASH = '-';
	INT i;
    MAC_TABLE_ENTRY *pEntry;

    /*DBGPRINT(RT_DEBUG_TRACE,("\n%s\n", arg));*/
/*
	The BAOriTearDown inupt string format should be xx:xx:xx:xx:xx:xx-d,
		=>The six 2 digit hex-decimal number previous are the Mac address,
		=>The seventh decimal number is the tid value.
*/
    if(strlen(arg) < 19)  /*Mac address acceptable format 01:02:03:04:05:06 length 17 plus the "-" and tid value in decimal format.*/
		return false;

	token = strchr(arg, DASH);
	if ((token != NULL) && (strlen(token)>1))
	{
		tid = simple_strtol((token+1), 0, 10);
		/* tid will be 0 ~ 7; Or kernel will crash in BAOriSessionTearDown() */
		if (tid > (NUM_OF_TID-1))
			return false;

		*token = '\0';
		for (i = 0, token = rstrtok(arg, &sepValue[0]); token; token = rstrtok(NULL, &sepValue[0]), i++)
		{
			if((strlen(token) != 2) || (!isxdigit(*token)) || (!isxdigit(*(token+1))))
				return false;
			AtoH(token, (&mac[i]), 1);
		}
		if(i != 6)
			return false;

	    DBGPRINT(RT_DEBUG_OFF, ("\n%02x:%02x:%02x:%02x:%02x:%02x-%02x",
								mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], tid));

	    pEntry = MacTableLookup(pAd, (u8 *) mac);

	    if (pEntry) {
	        DBGPRINT(RT_DEBUG_OFF, ("\nTear down Ori BA Session: Tid = %d\n", tid));
	        BAOriSessionTearDown(pAd, pEntry->Aid, tid, false, true);
	    }

		return true;
	}

	return false;

}

INT	Set_BARecTearDown_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
    u8 mac[6], tid;
	char *token;
	STRING sepValue[] = ":", DASH = '-';
	INT i;
    MAC_TABLE_ENTRY *pEntry;

    /*DBGPRINT(RT_DEBUG_TRACE,("\n%s\n", arg));*/
/*
	The BARecTearDown inupt string format should be xx:xx:xx:xx:xx:xx-d,
		=>The six 2 digit hex-decimal number previous are the Mac address,
		=>The seventh decimal number is the tid value.
*/
    if(strlen(arg) < 19)  /*Mac address acceptable format 01:02:03:04:05:06 length 17 plus the "-" and tid value in decimal format.*/
		return false;

	token = strchr(arg, DASH);
	if ((token != NULL) && (strlen(token)>1))
	{
		tid = simple_strtol((token+1), 0, 10);
		/* tid will be 0 ~ 7; Or kernel will crash in BARecSessionTearDown() */
		if (tid > (NUM_OF_TID-1))
			return false;

		*token = '\0';
		for (i = 0, token = rstrtok(arg, &sepValue[0]); token; token = rstrtok(NULL, &sepValue[0]), i++)
		{
			if((strlen(token) != 2) || (!isxdigit(*token)) || (!isxdigit(*(token+1))))
				return false;
			AtoH(token, (&mac[i]), 1);
		}
		if(i != 6)
			return false;

		DBGPRINT(RT_DEBUG_OFF, ("\n%02x:%02x:%02x:%02x:%02x:%02x-%02x",
								mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], tid));

		pEntry = MacTableLookup(pAd, (u8 *) mac);

		if (pEntry) {
		    DBGPRINT(RT_DEBUG_OFF, ("\nTear down Rec BA Session: Tid = %d\n", tid));
		    BARecSessionTearDown(pAd, pEntry->Aid, tid, false);
		}

		return true;
	}

	return false;

}

INT	Set_HtBw_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	ULONG HtBw;

	HtBw = simple_strtol(arg, 0, 10);

	if (HtBw == BW_40)
		pAd->CommonCfg.RegTransmitSetting.field.BW  = BW_40;
	else if (HtBw == BW_20)
		pAd->CommonCfg.RegTransmitSetting.field.BW  = BW_20;
	else
		return false;  /*Invalid argument */

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtBw_Proc::(HtBw=%d)\n", pAd->CommonCfg.RegTransmitSetting.field.BW));

	return true;
}


INT	Set_HtMcs_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	ULONG HtMcs, Mcs_tmp, ValidMcs = 15;
#ifdef CONFIG_STA_SUPPORT
    bool bAutoRate = false;
#endif /* CONFIG_STA_SUPPORT */

	Mcs_tmp = simple_strtol(arg, 0, 10);

	if (Mcs_tmp <= ValidMcs || Mcs_tmp == 32)
		HtMcs = Mcs_tmp;
	else
		HtMcs = MCS_AUTO;

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		pAd->StaCfg.DesiredTransmitSetting.field.MCS = HtMcs;
		pAd->StaCfg.bAutoTxRateSwitch = (HtMcs == MCS_AUTO) ? true:false;
		DBGPRINT(RT_DEBUG_TRACE, ("Set_HtMcs_Proc::(HtMcs=%d, bAutoTxRateSwitch = %d)\n",
						pAd->StaCfg.DesiredTransmitSetting.field.MCS, pAd->StaCfg.bAutoTxRateSwitch));

		if ((!WMODE_CAP_N(pAd->CommonCfg.PhyMode)) ||
			(pAd->MacTab.Content[BSSID_WCID].HTPhyMode.field.MODE < MODE_HTMIX))
		{
			if ((pAd->StaCfg.DesiredTransmitSetting.field.MCS != MCS_AUTO) &&
				(HtMcs <= 3) &&
				(pAd->StaCfg.DesiredTransmitSetting.field.FixedTxMode == FIXED_TXMODE_CCK))
			{
				RTMPSetDesiredRates(pAd, (LONG) (RateIdToMbps[HtMcs] * 1000000));
			}
			else if ((pAd->StaCfg.DesiredTransmitSetting.field.MCS != MCS_AUTO) &&
					(HtMcs <= 7) &&
	            			(pAd->StaCfg.DesiredTransmitSetting.field.FixedTxMode == FIXED_TXMODE_OFDM))
			{
				RTMPSetDesiredRates(pAd, (LONG) (RateIdToMbps[HtMcs+4] * 1000000));
			}
			else
				bAutoRate = true;

			if (bAutoRate)
			{
				pAd->StaCfg.DesiredTransmitSetting.field.MCS = MCS_AUTO;
				RTMPSetDesiredRates(pAd, -1);
			}
	        DBGPRINT(RT_DEBUG_TRACE, ("Set_HtMcs_Proc::(FixedTxMode=%d)\n",pAd->StaCfg.DesiredTransmitSetting.field.FixedTxMode));
		}
        if (ADHOC_ON(pAd))
            return true;
	}
#endif /* CONFIG_STA_SUPPORT */

	SetCommonHT(pAd);

	return true;
}

INT	Set_HtGi_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	ULONG HtGi;

	HtGi = simple_strtol(arg, 0, 10);

	if ( HtGi == GI_400)
		pAd->CommonCfg.RegTransmitSetting.field.ShortGI = GI_400;
	else if ( HtGi == GI_800 )
		pAd->CommonCfg.RegTransmitSetting.field.ShortGI = GI_800;
	else
		return false; /* Invalid argument */

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtGi_Proc::(ShortGI=%d)\n",pAd->CommonCfg.RegTransmitSetting.field.ShortGI));

	return true;
}


INT	Set_HtTxBASize_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	u8 Size;

	Size = simple_strtol(arg, 0, 10);

	if (Size <=0 || Size >=64)
	{
		Size = 8;
	}
	pAd->CommonCfg.TxBASize = Size-1;
	DBGPRINT(RT_DEBUG_ERROR, ("Set_HtTxBASize ::(TxBASize= %d)\n", Size));

	return true;
}

INT	Set_HtDisallowTKIP_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);

	if (Value == 1)
	{
		pAd->CommonCfg.HT_DisallowTKIP = true;
	}
	else
	{
		pAd->CommonCfg.HT_DisallowTKIP = false;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtDisallowTKIP_Proc ::%s\n",
				(pAd->CommonCfg.HT_DisallowTKIP == true) ? "enabled" : "disabled"));

	return true;
}

INT	Set_HtOpMode_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{

	ULONG Value;

	Value = simple_strtol(arg, 0, 10);

	if (Value == HTMODE_GF)
		pAd->CommonCfg.RegTransmitSetting.field.HTMODE  = HTMODE_GF;
	else if ( Value == HTMODE_MM )
		pAd->CommonCfg.RegTransmitSetting.field.HTMODE  = HTMODE_MM;
	else
		return false; /*Invalid argument */

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtOpMode_Proc::(HtOpMode=%d)\n",pAd->CommonCfg.RegTransmitSetting.field.HTMODE));

	return true;

}

INT	Set_HtStbc_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{

	ULONG Value;

	Value = simple_strtol(arg, 0, 10);

	if (Value == STBC_USE)
		pAd->CommonCfg.RegTransmitSetting.field.STBC = STBC_USE;
	else if ( Value == STBC_NONE )
		pAd->CommonCfg.RegTransmitSetting.field.STBC = STBC_NONE;
	else
		return false; /*Invalid argument */

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_Stbc_Proc::(HtStbc=%d)\n",pAd->CommonCfg.RegTransmitSetting.field.STBC));

	return true;
}

INT	Set_HtHtc_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{

	ULONG Value;

	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
		pAd->HTCEnable = false;
	else if ( Value ==1 )
        pAd->HTCEnable = true;
	else
		return false; /*Invalid argument 	*/

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtHtc_Proc::(HtHtc=%d)\n",pAd->HTCEnable));

	return true;
}

INT	Set_HtExtcha_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{

	ULONG Value;

	Value = simple_strtol(arg, 0, 10);

	if (Value == 0)
		pAd->CommonCfg.RegTransmitSetting.field.EXTCHA  = EXTCHA_BELOW;
	else if ( Value ==1 )
		pAd->CommonCfg.RegTransmitSetting.field.EXTCHA = EXTCHA_ABOVE;
	else
		return false; /*Invalid argument 	*/

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtExtcha_Proc::(HtExtcha=%d)\n",pAd->CommonCfg.RegTransmitSetting.field.EXTCHA));

	return true;
}

INT	Set_HtMpduDensity_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);

	if (Value <=7)
		pAd->CommonCfg.BACapability.field.MpduDensity = Value;
	else
		pAd->CommonCfg.BACapability.field.MpduDensity = 4;

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtMpduDensity_Proc::(HtMpduDensity=%d)\n",pAd->CommonCfg.BACapability.field.MpduDensity));

	return true;
}

INT	Set_HtBaWinSize_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);


	if (Value >=1 && Value <= 64)
	{
		pAd->CommonCfg.REGBACapability.field.RxBAWinLimit = Value;
		pAd->CommonCfg.BACapability.field.RxBAWinLimit = Value;
	}
	else
	{
        pAd->CommonCfg.REGBACapability.field.RxBAWinLimit = 64;
		pAd->CommonCfg.BACapability.field.RxBAWinLimit = 64;
	}

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtBaWinSize_Proc::(HtBaWinSize=%d)\n",pAd->CommonCfg.BACapability.field.RxBAWinLimit));

	return true;
}

INT	Set_HtRdg_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);

	if (Value == 0)
		pAd->CommonCfg.bRdg = false;
	else if ( Value ==1 )
	{
		pAd->HTCEnable = true;
        pAd->CommonCfg.bRdg = true;
	}
	else
		return false; /*Invalid argument*/

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtRdg_Proc::(HtRdg=%d)\n",pAd->CommonCfg.bRdg));

	return true;
}

INT	Set_HtLinkAdapt_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
		pAd->bLinkAdapt = false;
	else if ( Value ==1 )
	{
			pAd->HTCEnable = true;
			pAd->bLinkAdapt = true;
	}
	else
		return false; /*Invalid argument*/

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtLinkAdapt_Proc::(HtLinkAdapt=%d)\n",pAd->bLinkAdapt));

	return true;
}

INT	Set_HtAmsdu_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
		pAd->CommonCfg.BACapability.field.AmsduEnable = false;
	else if ( Value == 1 )
        pAd->CommonCfg.BACapability.field.AmsduEnable = true;
	else
		return false; /*Invalid argument*/

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtAmsdu_Proc::(HtAmsdu=%d)\n",pAd->CommonCfg.BACapability.field.AmsduEnable));

	return true;
}

INT	Set_HtAutoBa_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
	{
		pAd->CommonCfg.BACapability.field.AutoBA = false;
		pAd->CommonCfg.BACapability.field.Policy = BA_NOTUSE;
	}
    else if (Value == 1)
    {
		pAd->CommonCfg.BACapability.field.AutoBA = true;
		pAd->CommonCfg.BACapability.field.Policy = IMMED_BA;
    }
	else
		return false; /*Invalid argument*/

    pAd->CommonCfg.REGBACapability.field.AutoBA = pAd->CommonCfg.BACapability.field.AutoBA;
	pAd->CommonCfg.REGBACapability.field.Policy = pAd->CommonCfg.BACapability.field.Policy;
	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtAutoBa_Proc::(HtAutoBa=%d)\n",pAd->CommonCfg.BACapability.field.AutoBA));

	return true;

}

INT	Set_HtProtect_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
		pAd->CommonCfg.bHTProtect = false;
    else if (Value == 1)
		pAd->CommonCfg.bHTProtect = true;
	else
		return false; /*Invalid argument*/

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtProtect_Proc::(HtProtect=%d)\n",pAd->CommonCfg.bHTProtect));

	return true;
}

INT	Set_SendPSMPAction_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
    u8 mac[6], mode;
	char *token;
	STRING sepValue[] = ":", DASH = '-';
	INT i;
    MAC_TABLE_ENTRY *pEntry;

    /*DBGPRINT(RT_DEBUG_TRACE,("\n%s\n", arg));*/
/*
	The BARecTearDown inupt string format should be xx:xx:xx:xx:xx:xx-d,
		=>The six 2 digit hex-decimal number previous are the Mac address,
		=>The seventh decimal number is the mode value.
*/
    if(strlen(arg) < 19)  /*Mac address acceptable format 01:02:03:04:05:06 length 17 plus the "-" and mode value in decimal format.*/
		return false;

   	token = strchr(arg, DASH);
	if ((token != NULL) && (strlen(token)>1))
	{
		mode = simple_strtol((token+1), 0, 10);
		if (mode > MMPS_ENABLE)
			return false;

		*token = '\0';
		for (i = 0, token = rstrtok(arg, &sepValue[0]); token; token = rstrtok(NULL, &sepValue[0]), i++)
		{
			if((strlen(token) != 2) || (!isxdigit(*token)) || (!isxdigit(*(token+1))))
				return false;
			AtoH(token, (&mac[i]), 1);
		}
		if(i != 6)
			return false;

		DBGPRINT(RT_DEBUG_OFF, ("\n%02x:%02x:%02x:%02x:%02x:%02x-%02x",
								mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], mode));

		pEntry = MacTableLookup(pAd, mac);

		if (pEntry) {
		    DBGPRINT(RT_DEBUG_OFF, ("\nSendPSMPAction MIPS mode = %d\n", mode));
		    SendPSMPAction(pAd, pEntry->Aid, mode);
		}

		return true;
	}

	return false;


}

INT	Set_HtMIMOPSmode_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);

	if (Value <=3)
		pAd->CommonCfg.BACapability.field.MMPSmode = Value;
	else
		pAd->CommonCfg.BACapability.field.MMPSmode = 3;

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtMIMOPSmode_Proc::(MIMOPS mode=%d)\n",pAd->CommonCfg.BACapability.field.MMPSmode));

	return true;
}


INT	Set_ForceShortGI_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
		pAd->WIFItestbed.bShortGI = false;
	else if (Value == 1)
		pAd->WIFItestbed.bShortGI = true;
	else
		return false; /*Invalid argument*/

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ForceShortGI_Proc::(ForceShortGI=%d)\n", pAd->WIFItestbed.bShortGI));

	return true;
}



INT	Set_ForceGF_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
		pAd->WIFItestbed.bGreenField = false;
	else if (Value == 1)
		pAd->WIFItestbed.bGreenField = true;
	else
		return false; /*Invalid argument*/

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ForceGF_Proc::(ForceGF=%d)\n", pAd->WIFItestbed.bGreenField));

	return true;
}

INT	Set_HtMimoPs_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
		pAd->CommonCfg.bMIMOPSEnable = false;
	else if (Value == 1)
		pAd->CommonCfg.bMIMOPSEnable = true;
	else
		return false; /*Invalid argument*/

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtMimoPs_Proc::(HtMimoPs=%d)\n",pAd->CommonCfg.bMIMOPSEnable));

	return true;
}


INT Set_HT_BssCoex_Proc(
	IN	struct rtmp_adapter *	pAd,
	IN	char *			pParam)
{
	u8 bBssCoexEnable = simple_strtol(pParam, 0, 10);

	pAd->CommonCfg.bBssCoexEnable = ((bBssCoexEnable == 1) ? true: false);

	DBGPRINT(RT_DEBUG_TRACE, ("Set bBssCoexEnable=%d!\n", pAd->CommonCfg.bBssCoexEnable));

	return true;
}


INT Set_HT_BssCoexApCntThr_Proc(
	IN	struct rtmp_adapter *	pAd,
	IN	char *			pParam)
{
	pAd->CommonCfg.BssCoexApCntThr = simple_strtol(pParam, 0, 10);

	DBGPRINT(RT_DEBUG_TRACE, ("Set BssCoexApCntThr=%d!\n", pAd->CommonCfg.BssCoexApCntThr));

	return true;
}



INT	Set_VhtBw_Proc(
	IN struct rtmp_adapter*pAd,
	IN char *arg)
{
	ULONG vht_cw;
	u8 cent_ch;
	vht_cw = simple_strtol(arg, 0, 10);


	if (vht_cw == VHT_BW_80)
		pAd->CommonCfg.vht_bw = VHT_BW_80;
	else
		pAd->CommonCfg.vht_bw = VHT_BW_2040;

	if (!WMODE_CAP_AC(pAd->CommonCfg.PhyMode))
		goto direct_done;

	SetCommonHT(pAd);
	if(pAd->CommonCfg.BBPCurrentBW == BW_80)
		cent_ch = pAd->CommonCfg.vht_cent_ch;
	else
		cent_ch = pAd->CommonCfg.CentralChannel;

	AsicSwitchChannel(pAd, cent_ch, false);
	AsicLockChannel(pAd, cent_ch);

	DBGPRINT(RT_DEBUG_TRACE, ("BW_%s, PrimaryChannel(%d), %s CentralChannel = %d, apply it immediately\n",
						(pAd->CommonCfg.BBPCurrentBW == BW_80 ? "80":
							(pAd->CommonCfg.BBPCurrentBW == BW_40 ? "40" :
							"20")),
						pAd->CommonCfg.Channel,
						(pAd->CommonCfg.BBPCurrentBW == BW_80 ? "VHT" : "HT"), cent_ch));

direct_done:

	DBGPRINT(RT_DEBUG_TRACE, ("Set_VhtBw_Proc::(VHT_BW=%d)\n", pAd->CommonCfg.vht_bw));

	return true;
}


INT Set_VhtBwSignal_Proc(struct rtmp_adapter*pAd, char *arg)
{
	ULONG bw_signal = simple_strtol(arg, 0, 10);

	if (bw_signal <= 2)
		pAd->CommonCfg.vht_bw_signal = bw_signal;
	else
		pAd->CommonCfg.vht_bw_signal = 0;
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): vht_bw_signal=%d(%s)\n",
				__FUNCTION__, pAd->CommonCfg.vht_bw_signal,
				(pAd->CommonCfg.vht_bw_signal == 2 ? "Dynamic" :
				(pAd->CommonCfg.vht_bw_signal == 1 ? "Static" : "Disable"))));

	return true;
}


INT	Set_VhtStbc_Proc(
	IN struct rtmp_adapter*pAd,
	IN char *arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);

	if (Value == STBC_USE)
		pAd->CommonCfg.vht_stbc = STBC_USE;
	else if ( Value == STBC_NONE )
		pAd->CommonCfg.vht_stbc = STBC_NONE;
	else
		return false; /*Invalid argument */

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_VhtStbc_Proc::(VhtStbc=%d)\n", pAd->CommonCfg.vht_stbc));

	return true;
}



INT	Set_FixedTxMode_Proc(struct rtmp_adapter*pAd, char *arg)
{
	INT	fix_tx_mode = RT_CfgSetFixedTxPhyMode(arg);


#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		pAd->StaCfg.DesiredTransmitSetting.field.FixedTxMode = fix_tx_mode;
#endif /* CONFIG_STA_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("%s():(FixedTxMode=%d)\n",
								__FUNCTION__, fix_tx_mode));

	return true;
}

#ifdef DBG_CTRL_SUPPORT
#ifdef INCLUDE_DEBUG_QUEUE
/* ---------------------- DEBUG QUEUE ------------------------*/

#define DBQ_LENGTH	512
#define DBQ_DATA_LENGTH	8

/* Define to include TX and RX HT Control field in log */
/* #define DBQ_INCLUDE_HTC */

typedef
struct {
	u8 type;					/* type of data*/
	ULONG timestamp;			/* sec/usec timestamp from gettimeofday*/
	u8 data[DBQ_DATA_LENGTH];	/* data*/
} DBQUEUE_ENTRY;

/* Type field definitions */
#define DBQ_TYPE_EMPTY	0
#define DBQ_TYPE_TXWI	0x70		/* TXWI*/
#define DBQ_TYPE_TXHDR	0x72		/* TX Header*/
#define DBQ_TYPE_TXFIFO	0x73		/* TX Stat FIFO*/
#define DBQ_TYPE_RXWI	0x78		/* RXWI uses 0x78 to 0x7A for 5 longs*/
#define DBQ_TYPE_RXHDR	0x7B		/* RX Header*/
#define DBQ_TYPE_TXQHTC	0x7c		/* RX Qos+HT Control field*/
#define DBQ_TYPE_RXQHTC	0x7d		/* RX Qos+HT Control field */
#define DBQ_TYPE_RALOG	0x7e		/* RA Log */

#define DBQ_INIT_SIG	0x4442484E	/* 'DBIN' - dbqInit initialized flag*/
#define DBQ_ENA_SIG		0x4442454E	/* 'DBEN' - dbqEnable enabled flag*/

static DBQUEUE_ENTRY dbQueue[DBQ_LENGTH];
static ULONG dbqTail=0;
static ULONG dbqEnable=0;
static ULONG dbqInit=0;

/* dbQueueInit - initialize Debug Queue variables and clear the queue*/
void dbQueueInit(void)
{
	int i;

	for (i=0; i<DBQ_LENGTH; i++)
		dbQueue[i].type = DBQ_TYPE_EMPTY;
	dbqTail = 0;
	dbqInit = DBQ_INIT_SIG;
}

/* dbQueueEnqueue - enqueue data*/
void dbQueueEnqueue(u8 type, u8 *data)
{
	DBQUEUE_ENTRY *oldTail;
	struct timeval tval;

	if (dbqEnable!=DBQ_ENA_SIG || data==NULL)
		return;

	if (dbqInit!=DBQ_INIT_SIG || dbqTail>=DBQ_LENGTH)
		dbQueueInit();

	oldTail = &dbQueue[dbqTail];

	/* Advance tail and mark as empty*/
	if (dbqTail >= DBQ_LENGTH-1)
		dbqTail = 0;
	else
		dbqTail++;
	dbQueue[dbqTail].type = DBQ_TYPE_EMPTY;

	/* Enqueue data*/
	oldTail->type = type;
	do_gettimeofday(&tval);
	oldTail->timestamp = tval.tv_sec*1000000L + tval.tv_usec;
	memcpy(oldTail->data, data, DBQ_DATA_LENGTH);
}

void dbQueueEnqueueTxFrame(u8 *txwi, u8 *pHeader_802_11)
{
	dbQueueEnqueue(DBQ_TYPE_TXWI, txwi);

	/* 802.11 Header */
	if (pHeader_802_11 != NULL) {
		dbQueueEnqueue(DBQ_TYPE_TXHDR, pHeader_802_11);
#ifdef DBQ_INCLUDE_HTC
		/* Qos+HT Control field */
		if ((pHeader_802_11[0] & 0x08) && (pHeader_802_11[1] & 0x80))
			dbQueueEnqueue(DBQ_TYPE_TXQHTC, pHeader_802_11+24);
#endif /* DBQ_INCLUDE_HTC */
	}
}

void dbQueueEnqueueRxFrame(u8 *pRxWI, u8 *pHeader_802_11, ULONG flags)
{
	/* Ignore Beacons if disabled */
	if ((flags & DBF_DBQ_NO_BCN) && (pHeader_802_11[0] & 0xfc)==0x80)
		return;

	/* RXWI */
	dbQueueEnqueue(DBQ_TYPE_RXWI, pRxWI);
	if (flags & DBF_DBQ_RXWI_FULL) {
		dbQueueEnqueue(DBQ_TYPE_RXWI+1, pRxWI+8);
		dbQueueEnqueue(DBQ_TYPE_RXWI+2, pRxWI+16);
	}

	/* 802.11 Header */
	dbQueueEnqueue(DBQ_TYPE_RXHDR, (u8 *)pHeader_802_11);

#ifdef DBQ_INCLUDE_HTC
	/* Qos+HT Control field */
	if ((pHeader_802_11[0] & 0x08) &&
		(pHeader_802_11[1] & 0x80))
		dbQueueEnqueue(DBQ_TYPE_RXQHTC, pHeader_802_11+24);
#endif /* DBQ_INCLUDE_HTC */
}


/* dbQueueDisplayPhy - Display PHY rate */
static void dbQueueDisplayPHY(USHORT phyRate)
{
	static CHAR *mode[4] = {" C", "oM","mM", "gM"};

	DBGPRINT(RT_DEBUG_OFF, ("%2s%02d %c%c%c%c",
		//(phyRate>>8) & 0xFF, phyRate & 0xFF,
		mode[(phyRate>>14) & 0x3],							// Mode: c, o, m, g
		phyRate & 0x7F,										// MCS
		(phyRate & 0x0100)? 'S': 'L',						// Guard Int: S or L
		(phyRate & 0x0080)? '4': '2',						// BW: 4 or 2
		(phyRate & 0x0200)? 'S': 's',						// STBC:  S or s
		(phyRate & 0x2000)? 'I': ((phyRate & 0x800)? 'E': '_')	// Beamforming:  E or I or _
		) );
}

/* dbQueueDump - dump contents of debug queue*/
static void dbQueueDump(
	IN  struct rtmp_adapter *  pAd,
	bool decode)
{
	DBQUEUE_ENTRY *oldTail;
	int i, origMCS, succMCS;
	ULONG lastTimestamp=0;
	bool showTimestamp;
	USHORT phyRate;

	if (dbqInit!=DBQ_INIT_SIG || dbqTail>=DBQ_LENGTH)
		return;

	oldTail = &dbQueue[dbqTail];

	for (i=0; i<DBQ_LENGTH; i++) {
		if (++oldTail >= &dbQueue[DBQ_LENGTH])
			oldTail = dbQueue;

		/* Skip empty entries*/
		if (oldTail->type == DBQ_TYPE_EMPTY)
			continue;

		showTimestamp = false;

		switch (oldTail->type) {
		case 0x70:	/* TXWI - 2 longs, MSB to LSB */
		case 0x78:	/* RXWI - 2 longs, MSB to LSB */
			showTimestamp = true;

			if (decode && oldTail->type==0x70) {
				DBGPRINT(RT_DEBUG_OFF, ("\nTxWI ") );
				dbQueueDisplayPHY(oldTail->data[3]*256 + oldTail->data[2]);
				DBGPRINT(RT_DEBUG_OFF, ("%c s=%03X %02X %s-",
						(oldTail->data[0] & 0x10)? 'A': '_',				// AMPDU
						(oldTail->data[7]*256 + oldTail->data[6]) & 0xFFF,	// Size
						oldTail->data[5],									// WCID
						(oldTail->data[4] & 0x01)? "AK": "NA" ));			// ACK/NACK
			}
			else if (decode && oldTail->type==0x78) {
				DBGPRINT(RT_DEBUG_OFF, ("\nRxWI ") );
				dbQueueDisplayPHY(oldTail->data[7]*256 + oldTail->data[6]);
				DBGPRINT(RT_DEBUG_OFF, (" s=%03X %02X %02X%01X-",
						(oldTail->data[3]*256 + oldTail->data[2]) & 0xFFF,	// Size
						oldTail->data[0],									// WCID
						oldTail->data[5], oldTail->data[4]>>4 ));			// Seq Number
			}
			else
				DBGPRINT(RT_DEBUG_OFF, ("\n%cxWI %02X%02X %02X%02X-%02X%02X %02X%02X----",
					oldTail->type==0x70? 'T': 'R',
					oldTail->data[3], oldTail->data[2], oldTail->data[1], oldTail->data[0],
					oldTail->data[7], oldTail->data[6], oldTail->data[5], oldTail->data[4]) );
			break;

		case 0x79:	/* RXWI - next 2 longs, MSB to LSB */
			if (decode) {
				DBGPRINT(RT_DEBUG_OFF, ("Rx2  %2d %2d %2d S:%d %d %d ",
						ConvertToRssi(pAd, (CHAR)oldTail->data[0], RSSI_0),
						ConvertToRssi(pAd, (CHAR)oldTail->data[1], RSSI_1),
						ConvertToRssi(pAd, (CHAR)oldTail->data[2], RSSI_2),
						(oldTail->data[4]*3 + 8)/16,
						(oldTail->data[5]*3 + 8)/16,
						(oldTail->data[6]*3 + 8)/16) );
			}
			else
				DBGPRINT(RT_DEBUG_OFF, ("Rx2  %02X%02X %02X%02X-%02X%02X %02X%02X    ",
						oldTail->data[3], oldTail->data[2], oldTail->data[1], oldTail->data[0],
						oldTail->data[7], oldTail->data[6], oldTail->data[5], oldTail->data[4]) );
			break;


		case 0x7c:	/* TX HTC+QoS, 6 bytes, MSB to LSB */
		case 0x7d:	/* RX HTC+QoS, 6 bytes, MSB to LSB */
			DBGPRINT(RT_DEBUG_OFF, ("%cxHTC  H:%02X%02X%02X%02X Q:%02X%02X   ",
					oldTail->type==0x7c? 'T': 'R',
					oldTail->data[5], oldTail->data[4], oldTail->data[3], oldTail->data[2],
					oldTail->data[1], oldTail->data[0]) );
			break;

		case 0x72:	/* Tx 802.11 header, MSB to LSB, translate type/subtype*/
		case 0x7b:	/* Rx*/
			{
			u8 tCode;
			struct _typeTableEntry {
				u8 code;	/* Type/subtype*/
				CHAR  str[4];
			} *pTab;
			static struct _typeTableEntry typeTable[] = {
				{0x00, "mARq"}, {0x01, "mARp"}, {0x02, "mRRq"}, {0x03, "mRRp"},
				{0x04, "mPRq"}, {0x05, "mPRp"}, {0x08, "mBcn"}, {0x09, "mATI"},
				{0x0a, "mDis"}, {0x0b, "mAut"}, {0x0c, "mDAu"}, {0x0d, "mAct"},
				{0x0e, "mANA"},
				{0x17, "cCWr"}, {0x18, "cBAR"}, {0x19, "cBAc"}, {0x1a, "cPSP"},
				{0x1b, "cRTS"}, {0x1c, "cCTS"}, {0x1d, "cACK"}, {0x1e, "cCFE"},
				{0x1f, "cCEA"},
				{0x20, "dDat"}, {0x21, "dDCA"}, {0x22, "dDCP"}, {0x23, "dDAP"},
				{0x24, "dNul"}, {0x25, "dCFA"}, {0x26, "dCFP"}, {0x27, "dCAP"},
				{0x28, "dQDa"}, {0x29, "dQCA"}, {0x2a, "dQCP"}, {0x2b, "dQAP"},
				{0x2c, "dQNu"}, {0x2e, "dQNP"}, {0x2f, "dQNA"},
				{0xFF, "RESV"}};

			tCode = ((oldTail->data[0]<<2) & 0x30) | ((oldTail->data[0]>>4) & 0xF);
			for (pTab=typeTable; pTab->code!=0xFF; pTab++) {
				if (pTab->code == tCode)
					break;
			}

			DBGPRINT(RT_DEBUG_OFF, ("%cxH  %c%c%c%c [%02X%02X %02X%02X]       \n",
					oldTail->type==0x72? 'T': 'R',
					pTab->str[0], pTab->str[1], pTab->str[2], pTab->str[3],
					oldTail->data[3], oldTail->data[2], oldTail->data[1], oldTail->data[0]) );
			}
			break;

		case 0x73:	/* TX STAT FIFO*/
			showTimestamp = true;

			/* origMCS is limited to 4 bits. Check for case of MCS16 to 23*/
			origMCS = (oldTail->data[0]>>1) & 0xF;
			succMCS = (oldTail->data[2] & 0x7F);
			if (succMCS>origMCS && origMCS<8)
				origMCS += 16;
			phyRate = (oldTail->data[3]<<8) + oldTail->data[2];

			DBGPRINT(RT_DEBUG_OFF, ("TxFI %02X%02X%02X%02X=%c%c%c%c%c M%02d/%02d%c%c",
					oldTail->data[3], oldTail->data[2],
					oldTail->data[1], oldTail->data[0],
					(phyRate & 0x0100)? 'S': 'L',				/* Guard Int:    S or L */
					(phyRate & 0x0080)? '4': '2',				/* BW: 			 4 or 2 */
					(phyRate & 0x0200)? 'S': 's',				/* STBC:         S or s */
					(phyRate & 0x2000)? 'I': ((phyRate & 0x0800)? 'E': '_'), /* Beamforming:  E or I or _ */
					(oldTail->data[0] & 0x40)? 'A': '_',		/* Aggregated:   A or _ */
					succMCS, origMCS,							/* MCS:          <Final>/<orig> */
					succMCS==origMCS? ' ': '*',					/* Retry:        '*' if MCS doesn't match */
					(oldTail->data[0] & 0x20)? ' ': 'F') );		/* Success/Fail  _ or F */
			break;
		case 0x7E:	/* RA Log info */
			{
				struct {USHORT phy; USHORT per; USHORT tp; USHORT bfPer;} *p = (void*)(oldTail->data);
				DBGPRINT(RT_DEBUG_OFF, ("RALog %02X%02X %d %d %d    ",
											(p->phy>>8) & 0xFF, p->phy & 0xFF, p->per, p->tp, p->bfPer) );
			}
			break;

		default:
			DBGPRINT(RT_DEBUG_OFF, ("%02X   %02X%02X %02X%02X %02X%02X %02X%02X   ", oldTail->type,
					oldTail->data[0], oldTail->data[1], oldTail->data[2], oldTail->data[3],
					oldTail->data[4], oldTail->data[5], oldTail->data[6], oldTail->data[7]) );
			break;
		}

		if (showTimestamp)
		{
			ULONG t = oldTail->timestamp;
			ULONG dt = oldTail->timestamp-lastTimestamp;

			DBGPRINT(RT_DEBUG_OFF, ("%lu.%06lu ", t/1000000L, t % 1000000L) );

			if (dt>999999L)
			DBGPRINT(RT_DEBUG_OFF, ("+%lu.%06lu s\n", dt/1000000L, dt % 1000000L) );
			else
			DBGPRINT(RT_DEBUG_OFF, ("+%lu us\n", dt) );
			lastTimestamp = oldTail->timestamp;
		}
	}
}

/*
	Set_DebugQueue_Proc - Control DBQueue
		iwpriv ra0 set DBQueue=dd.
			dd: 0=>disable, 1=>enable, 2=>dump, 3=>clear, 4=>dump & decode
*/
INT Set_DebugQueue_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *        arg)
{
    ULONG argValue = simple_strtol(arg, 0, 10);

	switch (argValue) {
	case 0:
		dbqEnable = 0;
		break;
	case 1:
		dbqEnable = DBQ_ENA_SIG;
		break;
	case 2:
		dbQueueDump(pAd, false);
		break;
	case 3:
		dbQueueInit();
		break;
	case 4:
		dbQueueDump(pAd, true);
		break;
	default:
		return false;
	}

	return true;
}
#endif /* INCLUDE_DEBUG_QUEUE */
#endif /* DBG_CTRL_SUPPORT */

#ifdef CFO_TRACK
/*
	Set_CFOTrack_Proc - enable/disable CFOTrack
		usage: iwpriv ra0 set CFOTrack=[0..8]
*/
INT Set_CFOTrack_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *        arg)
{
    pAd->CommonCfg.CFOTrack = simple_strtol(arg, 0, 10);
    DBGPRINT(RT_DEBUG_TRACE, ("%s():(CFOTrack=%d)\n",
				__FUNCTION__, pAd->CommonCfg.CFOTrack));
	return true;
}
#endif /* CFO_TRACK */


#ifdef DBG_CTRL_SUPPORT
INT Set_DebugFlags_Proc(
    IN  struct rtmp_adapter *  pAd,
    IN  char *        arg)
{
    pAd->CommonCfg.DebugFlags = simple_strtol(arg, 0, 16);

    DBGPRINT(RT_DEBUG_TRACE, ("Set_DebugFlags_Proc::(DebugFlags=%02lX)\n", pAd->CommonCfg.DebugFlags));
	return true;
}
#endif /* DBG_CTRL_SUPPORT */






INT Set_LongRetryLimit_Proc(
	IN	struct rtmp_adapter *pAdapter,
	IN	char *		arg)
{
	TX_RTY_CFG_STRUC	tx_rty_cfg;
	u8 			LongRetryLimit = (u8)simple_strtol(arg, 0, 10);

	tx_rty_cfg.word = mt7610u_read32(pAdapter, TX_RTY_CFG);
	tx_rty_cfg.field.LongRtyLimit = LongRetryLimit;
	mt7610u_write32(pAdapter, TX_RTY_CFG, tx_rty_cfg.word);
	DBGPRINT(RT_DEBUG_TRACE, ("IF Set_LongRetryLimit_Proc::(tx_rty_cfg=0x%x)\n", tx_rty_cfg.word));
	return true;
}

INT Set_ShortRetryLimit_Proc(
	IN	struct rtmp_adapter *pAdapter,
	IN	char *		arg)
{
	TX_RTY_CFG_STRUC	tx_rty_cfg;
	u8 			ShortRetryLimit = (u8)simple_strtol(arg, 0, 10);

	tx_rty_cfg.word = mt7610u_read32(pAdapter, TX_RTY_CFG);
	tx_rty_cfg.field.ShortRtyLimit = ShortRetryLimit;
	mt7610u_write32(pAdapter, TX_RTY_CFG, tx_rty_cfg.word);
	DBGPRINT(RT_DEBUG_TRACE, ("IF Set_ShortRetryLimit_Proc::(tx_rty_cfg=0x%x)\n", tx_rty_cfg.word));
	return true;
}

INT Set_AutoFallBack_Proc(
	IN	struct rtmp_adapter *pAdapter,
	IN	char *		arg)
{
	return RT_CfgSetAutoFallBack(pAdapter, arg);
}



char *RTMPGetRalinkAuthModeStr(
    IN  NDIS_802_11_AUTHENTICATION_MODE authMode)
{
	switch(authMode)
	{
		case Ndis802_11AuthModeOpen:
			return "OPEN";
		case Ndis802_11AuthModeWPAPSK:
			return "WPAPSK";
		case Ndis802_11AuthModeShared:
			return "SHARED";
		case Ndis802_11AuthModeAutoSwitch:
			return "WEPAUTO";
		case Ndis802_11AuthModeWPA:
			return "WPA";
		case Ndis802_11AuthModeWPA2:
			return "WPA2";
		case Ndis802_11AuthModeWPA2PSK:
			return "WPA2PSK";
        case Ndis802_11AuthModeWPA1PSKWPA2PSK:
			return "WPAPSKWPA2PSK";
        case Ndis802_11AuthModeWPA1WPA2:
			return "WPA1WPA2";
		case Ndis802_11AuthModeWPANone:
			return "WPANONE";
		default:
			return "UNKNOW";
	}
}

char *RTMPGetRalinkEncryModeStr(
    IN  USHORT encryMode)
{
	switch(encryMode)
	{
		case Ndis802_11WEPDisabled:
			return "NONE";
		case Ndis802_11WEPEnabled:
			return "WEP";
		case Ndis802_11Encryption2Enabled:
			return "TKIP";
		case Ndis802_11Encryption3Enabled:
			return "AES";
        case Ndis802_11Encryption4Enabled:
			return "TKIPAES";
		default:
			return "UNKNOW";
	}
}

#ifdef APCLI_SUPPORT
 INT RTMPIoctlConnStatus(
	IN	struct rtmp_adapter *pAd,
 	IN	char *		arg)
{

 	INT i=0;
 	struct os_cookie *pObj;
 	u8 ifIndex;
	bool bConnect=false;

 	pObj = pAd->OS_Cookie;



 	DBGPRINT(RT_DEBUG_TRACE, ("==>RTMPIoctlConnStatus\n"));

 	if (pObj->ioctl_if_type != INT_APCLI)
 		return false;

 	ifIndex = pObj->ioctl_if;


 	DBGPRINT(RT_DEBUG_OFF, ("=============================================================\n"));
 	if((pAd->ApCfg.ApCliTab[ifIndex].CtrlCurrState == APCLI_CTRL_CONNECTED)
 		&& (pAd->ApCfg.ApCliTab[ifIndex].SsidLen != 0))
 	{
 		for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
 		{
 			PMAC_TABLE_ENTRY pEntry = &pAd->MacTab.Content[i];

 			if ( IS_ENTRY_APCLI(pEntry)
				&& (pEntry->Sst == SST_ASSOC)
				&& (pEntry->PortSecured == WPA_802_1X_PORT_SECURED))
 				{
 					DBGPRINT(RT_DEBUG_OFF, ("ApCli%d Connected AP : %02X:%02X:%02X:%02X:%02X:%02X   SSID:%s\n",ifIndex,
 						pEntry->Addr[0], pEntry->Addr[1], pEntry->Addr[2],
 						pEntry->Addr[3], pEntry->Addr[4], pEntry->Addr[5],
 						pAd->ApCfg.ApCliTab[ifIndex].Ssid));
					bConnect=true;
 				}
 		}

		if (!bConnect)
			DBGPRINT(RT_DEBUG_OFF, ("ApCli%d Connected AP : Disconnect\n",ifIndex));
 	}
 	else
 	{
 		DBGPRINT(RT_DEBUG_OFF, ("ApCli%d Connected AP : Disconnect\n",ifIndex));
 	}
 	DBGPRINT(RT_DEBUG_OFF, ("=============================================================\n"));
     	DBGPRINT(RT_DEBUG_TRACE, ("<==RTMPIoctlConnStatus\n"));
 	return true;
}
#endif/*APCLI_SUPPORT*/

void  getRate(HTTRANSMIT_SETTING HTSetting, ULONG* fLastTxRxRate)

{
	 INT MCSMappingRateTable[] =
	{2,  4,   11,  22, /* CCK*/
	12, 18,   24,  36, 48, 72, 96, 108, /* OFDM*/
	13, 26,   39,  52,  78, 104, 117, 130, 26,  52,  78, 104, 156, 208, 234, 260, /* 20MHz, 800ns GI, MCS: 0 ~ 15*/
	39, 78,  117, 156, 234, 312, 351, 390,										  /* 20MHz, 800ns GI, MCS: 16 ~ 23*/
	27, 54,   81, 108, 162, 216, 243, 270, 54, 108, 162, 216, 324, 432, 486, 540, /* 40MHz, 800ns GI, MCS: 0 ~ 15*/
	81, 162, 243, 324, 486, 648, 729, 810,										  /* 40MHz, 800ns GI, MCS: 16 ~ 23*/
	14, 29,   43,  57,  87, 115, 130, 144, 29, 59,   87, 115, 173, 230, 260, 288, /* 20MHz, 400ns GI, MCS: 0 ~ 15*/
	43, 87,  130, 173, 260, 317, 390, 433,										  /* 20MHz, 400ns GI, MCS: 16 ~ 23*/
	30, 60,   90, 120, 180, 240, 270, 300, 60, 120, 180, 240, 360, 480, 540, 600, /* 40MHz, 400ns GI, MCS: 0 ~ 15*/
	90, 180, 270, 360, 540, 720, 810, 900};

	int rate_count = sizeof(MCSMappingRateTable)/sizeof(int);
	int rate_index = 0;
	int value = 0;

    if (HTSetting.field.MODE >= MODE_HTMIX)
    {
/*    	rate_index = 12 + ((u8)ht_setting.field.BW *16) + ((u8)ht_setting.field.ShortGI *32) + ((u8)ht_setting.field.MCS);*/
    	rate_index = 12 + ((u8)HTSetting.field.BW *24) + ((u8)HTSetting.field.ShortGI *48) + ((u8)HTSetting.field.MCS);
    }
    else
    if (HTSetting.field.MODE == MODE_OFDM)
    	rate_index = (u8)(HTSetting.field.MCS) + 4;
    else if (HTSetting.field.MODE == MODE_CCK)
    	rate_index = (u8)(HTSetting.field.MCS);

    if (rate_index < 0)
        rate_index = 0;

    if (rate_index >= rate_count)
        rate_index = rate_count-1;

    value = (MCSMappingRateTable[rate_index] * 5)/10;
	*fLastTxRxRate=(ULONG)value;
	return;
}

void assoc_ht_info_debugshow(
	IN struct rtmp_adapter *pAd,
	IN MAC_TABLE_ENTRY *pEntry,
	IN u8 ht_cap_len,
	IN HT_CAPABILITY_IE *pHTCapability)
{
	HT_CAP_INFO			*pHTCap;
	HT_CAP_PARM		*pHTCapParm;
	EXT_HT_CAP_INFO		*pExtHT;

	if (pHTCapability && (ht_cap_len > 0))
	{
		pHTCap = &pHTCapability->HtCapInfo;
		pHTCapParm = &pHTCapability->HtCapParm;
		pExtHT = &pHTCapability->ExtHtCapInfo;

		DBGPRINT(RT_DEBUG_TRACE, ("Peer - 11n HT Info\n"));
		DBGPRINT(RT_DEBUG_TRACE, ("\tHT Cap Info: \n"));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\t AdvCode(%d), BW(%d), MIMOPS(%d), GF(%d), ShortGI_20(%d), ShortGI_40(%d)\n",
			pHTCap->AdvCoding, pHTCap->ChannelWidth, pHTCap->MimoPs, pHTCap->GF,
			pHTCap->ShortGIfor20, pHTCap->ShortGIfor40));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\t TxSTBC(%d), RxSTBC(%d), DelayedBA(%d), A-MSDU(%d), CCK_40(%d)\n",
			pHTCap->TxSTBC, pHTCap->RxSTBC, pHTCap->DelayedBA, pHTCap->AMsduSize, pHTCap->CCKmodein40));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\t PSMP(%d), Forty_Mhz_Intolerant(%d), L-SIG(%d)\n",
			pHTCap->PSMP, pHTCap->Forty_Mhz_Intolerant, pHTCap->LSIGTxopProSup));

		DBGPRINT(RT_DEBUG_TRACE, ("\tHT Parm Info: \n"));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\t MaxRx A-MPDU Factor(%d), MPDU Density(%d)\n",
			pHTCapParm->MaxRAmpduFactor, pHTCapParm->MpduDensity));

		DBGPRINT(RT_DEBUG_TRACE, ("\tHT MCS set: \n"));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\t RxMCS(%02x %02x %02x %02x %02x) MaxRxMbps(%d) TxMCSSetDef(%02x)\n",
			pHTCapability->MCSSet[0], pHTCapability->MCSSet[1], pHTCapability->MCSSet[2],
			pHTCapability->MCSSet[3], pHTCapability->MCSSet[4],
			(pHTCapability->MCSSet[11]<<8) + pHTCapability->MCSSet[10],
			pHTCapability->MCSSet[12]));

		DBGPRINT(RT_DEBUG_TRACE, ("\tExt HT Cap Info: \n"));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\t PCO(%d), TransTime(%d), MCSFeedback(%d), +HTC(%d), RDG(%d)\n",
			pExtHT->Pco, pExtHT->TranTime, pExtHT->MCSFeedback, pExtHT->PlusHTC, pExtHT->RDGSupport));

		DBGPRINT(RT_DEBUG_TRACE, ("\nPeer - MODE=%d, BW=%d, MCS=%d, ShortGI=%d, MaxRxFactor=%d, MpduDensity=%d, MIMOPS=%d, AMSDU=%d\n",
			pEntry->HTPhyMode.field.MODE, pEntry->HTPhyMode.field.BW,
			pEntry->HTPhyMode.field.MCS, pEntry->HTPhyMode.field.ShortGI,
			pEntry->MaxRAmpduFactor, pEntry->MpduDensity,
			pEntry->MmpsMode, pEntry->AMsduSize));

		DBGPRINT(RT_DEBUG_TRACE, ("\tExt Cap Info: \n"));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\tBss2040CoexistMgmt=%d\n", pEntry->BSS2040CoexistenceMgmtSupport));
	}
}


INT	Set_BurstMode_Proc(
	IN	struct rtmp_adapter *pAd,
	IN	char *		arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);

	if (Value == 1)
	{
		pAd->CommonCfg.bRalinkBurstMode= true;
		RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RALINK_BURST_MODE);
		AsicEnableRalinkBurstMode(pAd);
	}
	else
	{
		pAd->CommonCfg.bRalinkBurstMode = false;
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_RALINK_BURST_MODE);
		AsicDisableRalinkBurstMode(pAd);
	}

	DBGPRINT(RT_DEBUG_TRACE, ("Set_BurstMode_Proc ::%s\n",
				(pAd->CommonCfg.bRalinkBurstMode == true) ? "enabled" : "disabled"));

	return true;
}


void assoc_vht_info_debugshow(
	IN struct rtmp_adapter*pAd,
	IN MAC_TABLE_ENTRY *pEntry,
	IN VHT_CAP_IE *vht_cap,
	IN VHT_OP_IE *vht_op)
{
	VHT_CAP_INFO *cap_info;
	VHT_MCS_SET *mcs_set;
	VHT_OP_INFO *op_info;
	VHT_MCS_MAP *mcs_map;


	if (!WMODE_CAP_AC(pAd->CommonCfg.PhyMode))
		return;

	DBGPRINT(RT_DEBUG_TRACE, ("Peer - 11AC VHT Info\n"));
	if (vht_cap)
	{
		cap_info = &vht_cap->vht_cap;
		mcs_set = &vht_cap->mcs_set;

		DBGPRINT(RT_DEBUG_TRACE, ("\tVHT Cap Info: \n"));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\tMaxMpduLen(%d), BW(%d), SGI_80M(%d), TxSTBC(%d), RxSTBC(%d), +HTC-VHT(%d)\n",
				cap_info->max_mpdu_len, cap_info->ch_width, cap_info->sgi_80M, cap_info->tx_stbc,
				cap_info->rx_stbc, cap_info->htc_vht_cap));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\tMaxAmpduExp(%d), VhtLinkAdapt(%d), RxAntConsist(%d), TxAntConsist(%d)\n",
				cap_info->max_ampdu_exp, cap_info->vht_link_adapt, cap_info->rx_ant_consistency, cap_info->tx_ant_consistency));
		mcs_map = &mcs_set->rx_mcs_map;
		DBGPRINT(RT_DEBUG_TRACE, ("\t\tRxMcsSet: HighRate(%d), RxMCSMap(%d,%d,%d,%d,%d,%d,%d)\n",
			mcs_set->rx_high_rate, mcs_map->mcs_ss1, mcs_map->mcs_ss2, mcs_map->mcs_ss3,
			mcs_map->mcs_ss4, mcs_map->mcs_ss5, mcs_map->mcs_ss6, mcs_map->mcs_ss7));
		mcs_map = &mcs_set->tx_mcs_map;
		DBGPRINT(RT_DEBUG_TRACE, ("\t\tTxMcsSet: HighRate(%d), TxMcsMap(%d,%d,%d,%d,%d,%d,%d)\n",
			mcs_set->tx_high_rate, mcs_map->mcs_ss1, mcs_map->mcs_ss2, mcs_map->mcs_ss3,
			mcs_map->mcs_ss4, mcs_map->mcs_ss5, mcs_map->mcs_ss6, mcs_map->mcs_ss7));
	}

	if (vht_op)
	{
		op_info = &vht_op->vht_op_info;
		mcs_map = &vht_op->basic_mcs_set;
		DBGPRINT(RT_DEBUG_TRACE, ("\tHT OP Info: \n"));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\tChannel Width(%d), CenteralFreq1(%d), CenteralFreq2(%d)\n",
			op_info->ch_width, op_info->center_freq_1, op_info->center_freq_2));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\tBasicMCSSet(SS1:%d, SS2:%d, SS3:%d, SS4:%d, SS5:%d, SS6:%d, SS7:%d)\n",
			mcs_map->mcs_ss1, mcs_map->mcs_ss2, mcs_map->mcs_ss3,
			mcs_map->mcs_ss4, mcs_map->mcs_ss5, mcs_map->mcs_ss6,
			mcs_map->mcs_ss7));
	}
	DBGPRINT(RT_DEBUG_TRACE, ("\n"));

}


INT Set_RateAdaptInterval(
	IN struct rtmp_adapter*pAd,
	IN char *arg)
{
	u32 ra_time, ra_qtime;
	char *token;
	char sep = ':';
	ULONG irqFlags;

/*
	The ra_interval inupt string format should be d:d, in units of ms.
		=>The first decimal number indicates the rate adaptation checking period,
		=>The second decimal number indicates the rate adaptation quick response checking period.
*/
	DBGPRINT(RT_DEBUG_TRACE,("%s():%s\n", __FUNCTION__, arg));

	token = strchr(arg, sep);
	if (token != NULL)
	{
		*token = '\0';

		if (strlen(arg) && strlen(token+1))
		{
			ra_time = simple_strtol(arg, 0, 10);
			ra_qtime = simple_strtol(token+1, 0, 10);
			DBGPRINT(RT_DEBUG_OFF, ("%s():Set RateAdaptation TimeInterval as(%d:%d) ms\n",
						__FUNCTION__, ra_time, ra_qtime));

			spin_lock_bh(&pAd->irq_lock);
			pAd->ra_interval = ra_time;
			pAd->ra_fast_interval = ra_qtime;
			spin_unlock_bh(&pAd->irq_lock);
			return true;
		}
	}

	return false;

}


INT Set_VcoPeriod_Proc(
	IN struct rtmp_adapter	*pAd,
	IN char *		arg)
{
	pAd->chipCap.VcoPeriod = simple_strtol(arg, 0, 10);

	DBGPRINT(RT_DEBUG_TRACE,
			("VCO Period = %d seconds\n", pAd->chipCap.VcoPeriod));
	return true;
}

#ifdef WFA_VHT_PF
INT set_force_noack(struct rtmp_adapter*pAd, char *arg)
{
	pAd->force_noack = (simple_strtol(arg, 0, 10) > 0 ? true : false);
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): force_noack=%d\n",
				__FUNCTION__, pAd->force_noack));

	return true;
}


INT set_force_amsdu(struct rtmp_adapter*pAd, char *arg)
{
	pAd->force_amsdu = (simple_strtol(arg, 0, 10) > 0 ? true : false);
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): force_amsdu=%d\n",
				__FUNCTION__, pAd->force_amsdu));
	return true;
}


INT set_force_vht_sgi(struct rtmp_adapter*pAd, char *arg)
{
	pAd->vht_force_sgi = (simple_strtol(arg, 0, 10) > 0 ? true : false);
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): vht_force_sgi=%d\n",
				__FUNCTION__, pAd->vht_force_sgi));

	return true;
}


INT set_force_vht_tx_stbc(struct rtmp_adapter*pAd, char *arg)
{
	pAd->vht_force_tx_stbc = (simple_strtol(arg, 0, 10) > 0 ? true : false);
	if (pAd->CommonCfg.TxStream < 2)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s(): TxStream=%d is not enough for TxSTBC!\n",
				__FUNCTION__, pAd->CommonCfg.TxStream));
		pAd->vht_force_tx_stbc = 0;
	}
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): vht_force_tx_stbc=%d\n",
				__FUNCTION__, pAd->vht_force_tx_stbc));

	return true;
}


INT set_force_ext_cca(struct rtmp_adapter*pAd, char *arg)
{
	ULONG cca_cfg;
	u32 mac_val;

	cca_cfg = (simple_strtol(arg, 0, 10) > 0 ? true : false);
	if (cca_cfg)
		mac_val = 0x0410243f;
	else
		mac_val = 0x583f;
	mt7610u_write32(pAd, TXOP_CTRL_CFG, mac_val);

	return true;
}


#ifdef IP_ASSEMBLY
INT set_force_ip_assemble(struct rtmp_adapter*pAd, char *arg)
{
	pAd->ip_assemble = (simple_strtol(arg, 0, 10) > 0 ? true : false);
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): ip_assemble = %d\n",
				__FUNCTION__, pAd->ip_assemble));

	return true;
}
#endif /* IP_ASSEMBLY */
#endif /* WFA_VHT_PF */
