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


#ifndef __EEPROM_H__
#define __EEPROM_H__

/* For ioctl check usage */
#define EEPROM_IS_PROGRAMMED		0x80


#ifdef RTMP_MAC_USB
#define EEPROM_SIZE					0x400
#endif /* RTMP_MAC_USB */


/* ------------------------------------------------------------------- */
/*  E2PROM data layout */
/* ------------------------------------------------------------------- */

/* Board type */

#define BOARD_TYPE_MINI_CARD		0	/* Mini card */
#define BOARD_TYPE_USB_PEN		1	/* USB pen */

/*
	EEPROM antenna select format
*/

#ifdef RT_BIG_ENDIAN
typedef union _EEPROM_NIC_CINFIG2_STRUC {
	struct {
		USHORT DACTestBit:1;	/* control if driver should patch the DAC issue */
		USHORT CoexBit:1;
		USHORT bInternalTxALC:1;	/* Internal Tx ALC */
		USHORT AntOpt:1;	/* Fix Antenna Option: 0:Main; 1: Aux */
		USHORT AntDiversity:1;	/* Antenna diversity */
		USHORT Rsv1:1;	/* must be 0 */
		USHORT BW40MAvailForA:1;	/* 0:enable, 1:disable */
		USHORT BW40MAvailForG:1;	/* 0:enable, 1:disable */
		USHORT EnableWPSPBC:1;	/* WPS PBC Control bit */
		USHORT BW40MSidebandForA:1;
		USHORT BW40MSidebandForG:1;
		USHORT CardbusAcceleration:1;	/* !!! NOTE: 0 - enable, 1 - disable */
		USHORT ExternalLNAForA:1;	/* external LNA enable for 5G */
		USHORT ExternalLNAForG:1;	/* external LNA enable for 2.4G */
		USHORT DynamicTxAgcControl:1;	/* */
		USHORT HardwareRadioControl:1;	/* Whether RF is controlled by driver or HW. 1:enable hw control, 0:disable */
	} field;
	USHORT word;
} EEPROM_NIC_CONFIG2_STRUC, *PEEPROM_NIC_CONFIG2_STRUC;
#else
typedef union _EEPROM_NIC_CINFIG2_STRUC {
	struct {
		USHORT HardwareRadioControl:1;	/* 1:enable, 0:disable */
		USHORT DynamicTxAgcControl:1;	/* */
		USHORT ExternalLNAForG:1;	/* */
		USHORT ExternalLNAForA:1;	/* external LNA enable for 2.4G */
		USHORT CardbusAcceleration:1;	/* !!! NOTE: 0 - enable, 1 - disable */
		USHORT BW40MSidebandForG:1;
		USHORT BW40MSidebandForA:1;
		USHORT EnableWPSPBC:1;	/* WPS PBC Control bit */
		USHORT BW40MAvailForG:1;	/* 0:enable, 1:disable */
		USHORT BW40MAvailForA:1;	/* 0:enable, 1:disable */
		USHORT Rsv1:1;	/* must be 0 */
		USHORT AntDiversity:1;	/* Antenna diversity */
		USHORT AntOpt:1;	/* Fix Antenna Option: 0:Main; 1: Aux */
		USHORT bInternalTxALC:1;	/* Internal Tx ALC */
		USHORT CoexBit:1;
		USHORT DACTestBit:1;	/* control if driver should patch the DAC issue */
	} field;
	USHORT word;
} EEPROM_NIC_CONFIG2_STRUC, *PEEPROM_NIC_CONFIG2_STRUC;
#endif


#if defined(BT_COEXISTENCE_SUPPORT)
#ifdef RTMP_USB_SUPPORT
#ifdef RT_BIG_ENDIAN
typedef union _EEPROM_NIC_CINFIG3_STRUC {
	struct {
		USHORT Rsv1:7;	/* must be 0 */
		USHORT CoexMethod:1;
		USHORT TxStream:4;	/* Number of Tx stream */
		USHORT RxStream:4;	/* Number of rx stream */
	} field;
	USHORT word;
} EEPROM_NIC_CONFIG3_STRUC, *PEEPROM_NIC_CONFIG3_STRUC;
#else
typedef union _EEPROM_NIC_CINFIG3_STRUC {
	struct {
		USHORT RxStream:4;	/* Number of rx stream */
		USHORT TxStream:4;	/* Number of Tx stream */
		USHORT CoexMethod:1;
		USHORT Rsv1:7;	/* must be 0 */
	} field;
	USHORT word;
} EEPROM_NIC_CONFIG3_STRUC, *PEEPROM_NIC_CONFIG3_STRUC;
#endif
#endif /* RTMP_USB_SUPPORT */

#endif /* defined(BT_COEXISTENCE_SUPPORT) */



/*
	TX_PWR Value valid range 0xFA(-6) ~ 0x24(36)
*/
#ifdef RT_BIG_ENDIAN
typedef union _EEPROM_TX_PWR_STRUC {
	struct {
		signed char Byte1;	/* High Byte */
		signed char Byte0;	/* Low Byte */
	} field;
	USHORT word;
} EEPROM_TX_PWR_STRUC, *PEEPROM_TX_PWR_STRUC;
#else
typedef union _EEPROM_TX_PWR_STRUC {
	struct {
		signed char Byte0;	/* Low Byte */
		signed char Byte1;	/* High Byte */
	} field;
	USHORT word;
} EEPROM_TX_PWR_STRUC, *PEEPROM_TX_PWR_STRUC;
#endif

#ifdef RT_BIG_ENDIAN
typedef union _EEPROM_VERSION_STRUC {
	struct {
		u8 Version;	/* High Byte */
		u8 FaeReleaseNumber;	/* Low Byte */
	} field;
	USHORT word;
} EEPROM_VERSION_STRUC, *PEEPROM_VERSION_STRUC;
#else
typedef union _EEPROM_VERSION_STRUC {
	struct {
		u8 FaeReleaseNumber;	/* Low Byte */
		u8 Version;	/* High Byte */
	} field;
	USHORT word;
} EEPROM_VERSION_STRUC, *PEEPROM_VERSION_STRUC;
#endif

struct rtmp_adapter;



#ifdef RTMP_USB_SUPPORT
int RTUSBReadEEPROM16(struct rtmp_adapter *pAd,
	u16 offset, u16 *pData);

int RTUSBWriteEEPROM16(struct rtmp_adapter *pAd,
	u16 offset, u16 value);
#endif /* RTMP_USB_SUPPORT */

#ifdef RTMP_EFUSE_SUPPORT
INT eFuseLoadEEPROM(struct rtmp_adapter *pAd);
void eFuseGetFreeBlockCount(struct rtmp_adapter *pAd, UINT *EfuseFreeBlock);

int rtmp_ee_efuse_read16(struct rtmp_adapter *pAd, USHORT Offset, USHORT *pVal);
int rtmp_ee_efuse_write16(struct rtmp_adapter *pAd, USHORT Offset, USHORT data);


int eFuseRead(struct rtmp_adapter *pAd, USHORT Offset, USHORT *pData, USHORT len);
int eFuseWrite(struct rtmp_adapter *pAd, USHORT Offset, USHORT *pData, USHORT len);

INT eFuse_init(struct rtmp_adapter *pAd);
INT efuse_probe(struct rtmp_adapter *pAd);
#endif /* RTMP_EFUSE_SUPPORT */


/*************************************************************************
  *	Public function declarations for prom operation callback functions setting
  ************************************************************************/

#endif /* __EEPROM_H__ */
