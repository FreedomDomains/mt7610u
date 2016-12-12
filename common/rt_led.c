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

INT LED_Array[16][12]={
	{	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1},
	{ 	0, 	2,  	1,	0,	-1,	-1,	0, 	-1, 	5, 	-1, 	-1, 	17},
	{	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1},
	{	-1, 	-1,	-1,	-1,	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1},
	{	-1, 	-1,	-1,	-1,	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1},
	{	-1, 	-1, 	-1,	-1,	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1},
	{	-1, 	-1,	-1,	-1,	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1},
	{	-1, 	-1,	-1,	-1,	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1},
	{	-1, 	-1,	-1,	-1,	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1},
	{	-1, 	-1,	-1,	-1,	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1},
	{  	3,  	2,   	-1,	-1,	-1, 	-1, 	16,	1, 	5,	-1, 	-1, 	17},
	{	-1, 	-1,	-1,	-1,	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1},
	{	-1, 	-1,	-1,	-1,	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1, 	-1},
	{	-1, 	-1,	-1,	-1,	-1, 	-1, 	-1,	-1, 	-1, 	-1, 	-1, 	-1},
	{ 	1,   	2,	1,	-1,	-1, 	-1,	3, 	-1,	6, 	-1, 	-1,	0},
	{ 	1,   	2,	1,   	-1, 	-1, 	-1, 	-1,  	1,   	4, 	-1, 	-1, 	18}
};



/*
	========================================================================

	Routine Description:
		Set LED Status

	Arguments:
		pAd						Pointer to our adapter
		Status					LED Status

	Return Value:
		None

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	Note:

	========================================================================
*/
void RTMPSetLEDStatus(
	IN struct rtmp_adapter *	pAd,
	IN u8 		Status)
{
	/*ULONG			data; */
	u8 		LinkStatus = 0;
	u8 		LedMode;
	u8 		MCUCmd = 0;
	bool 		bIgnored = false;
	INT LED_CMD = -1;

#ifdef STATS_COUNT_SUPPORT
	if(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF))
		return;
#endif /* STATS_COUNT_SUPPORT */

	LedMode = LED_MODE(pAd);

	if (LedMode < 0 || Status < 0 || LedMode > 15 || Status > 11)
		return;

	LED_CMD = LED_Array[LedMode][Status];

	switch (Status) {
		case LED_LINK_DOWN:
			LinkStatus = LINK_STATUS_LINK_DOWN;
			MCUCmd = MCU_SET_LED_MODE;
			break;
		case LED_LINK_UP:
			if (pAd->CommonCfg.Channel > 14)
				LinkStatus = LINK_STATUS_ABAND_LINK_UP;
			else
				LinkStatus = LINK_STATUS_GBAND_LINK_UP;

			MCUCmd = MCU_SET_LED_MODE;
			break;
		case LED_RADIO_ON:
			LinkStatus = LINK_STATUS_RADIO_ON;
			MCUCmd = MCU_SET_LED_MODE;
			break;
		case LED_HALT:
			LedMode = 0; /* Driver sets MAC register and MAC controls LED */
		case LED_RADIO_OFF:
			LinkStatus = LINK_STATUS_RADIO_OFF;
			MCUCmd = MCU_SET_LED_MODE;
			break;
		case LED_WPS:
			LinkStatus = LINK_STATUS_WPS;
			MCUCmd = MCU_SET_LED_MODE;
			break;
		case LED_ON_SITE_SURVEY:
			LinkStatus = LINK_STATUS_ON_SITE_SURVEY;
			MCUCmd = MCU_SET_LED_MODE;
			break;
		case LED_POWER_UP:
			LinkStatus = LINK_STATUS_POWER_UP;
			MCUCmd = MCU_SET_LED_MODE;
			break;
		default:
			DBGPRINT(RT_DEBUG_WARN, ("RTMPSetLED::Unknown Status 0x%x\n", Status));
			break;
	}

	if (LED_CMD != -1)
		andes_led_op(pAd, 0, LED_CMD);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: MCUCmd:0x%x, LED Mode:0x%x, LinkStatus:0x%x\n", __FUNCTION__, MCUCmd, LedMode, LinkStatus));

    /* */
	/* Keep LED status for LED SiteSurvey mode. */
	/* After SiteSurvey, we will set the LED mode to previous status. */
	/* */
	if ((Status != LED_ON_SITE_SURVEY) && (Status != LED_POWER_UP) && (bIgnored == false))
		pAd->LedCntl.LedStatus = Status;

}

void RTMPGetLEDSetting(IN struct rtmp_adapter*pAd)
{
	USHORT Value;
	PLED_CONTROL pLedCntl = &pAd->LedCntl;

	// TODO: wait TC6008 EEPROM format
	RT28xx_EEPROM_READ16(pAd, EEPROM_FREQ_OFFSET, Value);
	pLedCntl->MCULedCntl.word = (Value >> 8);
	RT28xx_EEPROM_READ16(pAd, EEPROM_LEDAG_CONF_OFFSET, Value);
	pLedCntl->LedAGCfg= Value;
	RT28xx_EEPROM_READ16(pAd, EEPROM_LEDACT_CONF_OFFSET, Value);
	pLedCntl->LedACTCfg = Value;
	RT28xx_EEPROM_READ16(pAd, EEPROM_LED_POLARITY_OFFSET, Value);
	pLedCntl->LedPolarity = Value;
}


void RTMPStartLEDMode(IN struct rtmp_adapter*pAd)
{
}


void RTMPInitLEDMode(IN struct rtmp_adapter*pAd)
{
	PLED_CONTROL pLedCntl = &pAd->LedCntl;

	if (pLedCntl->MCULedCntl.word == 0xFF) 	{
		pLedCntl->MCULedCntl.word = 0x01;
		pLedCntl->LedAGCfg = 0x5555;
		pLedCntl->LedACTCfg= 0x2221;
		pLedCntl->LedPolarity = 0x5627;
	}

	AsicSendCommandToMcu(pAd, MCU_SET_LED_AG_CFG, 0xff, (u8)pLedCntl->LedAGCfg, (u8)(pLedCntl->LedAGCfg >> 8));
	AsicSendCommandToMcu(pAd, MCU_SET_LED_ACT_CFG, 0xff, (u8)pLedCntl->LedACTCfg, (u8)(pLedCntl->LedACTCfg >> 8));
	AsicSendCommandToMcu(pAd, MCU_SET_LED_POLARITY, 0xff, (u8)pLedCntl->LedPolarity, (u8)(pLedCntl->LedPolarity >> 8));
	AsicSendCommandToMcu(pAd, MCU_SET_LED_GPIO_SIGNAL_CFG, 0xff, 0, pLedCntl->MCULedCntl.field.Polarity);

	RTMPStartLEDMode(pAd);
}


inline void RTMPExitLEDMode(IN struct rtmp_adapter*pAd)
{

	RTMPSetLED(pAd, LED_RADIO_OFF);

	return;
}

