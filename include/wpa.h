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


#ifndef	__WPA_H__
#define	__WPA_H__

#ifndef ROUND_UP
#define ROUND_UP(__x, __y) \
	(((ULONG)((__x)+((__y)-1))) & ((ULONG)~((__y)-1)))
#endif

#define	SET_UINT16_TO_ARRARY(_V, _LEN)		\
{											\
	_V[0] = ((UINT16)_LEN) >> 8;			\
	_V[1] = ((UINT16)_LEN & 0xFF);					\
}

#define	INC_UINT16_TO_ARRARY(_V, _LEN)			\
{												\
	UINT16	var_len;							\
												\
	var_len = (_V[0]<<8) | (_V[1]);				\
	var_len += _LEN;							\
												\
	_V[0] = (var_len & 0xFF00) >> 8;			\
	_V[1] = (var_len & 0xFF);					\
}

#define	CONV_ARRARY_TO_UINT16(_V)	((_V[0]<<8) | (_V[1]))

#define	ADD_ONE_To_64BIT_VAR(_V)		\
{										\
	u8 cnt = LEN_KEY_DESC_REPLAY;	\
	do									\
	{									\
		cnt--;							\
		_V[cnt]++;						\
		if (cnt == 0)					\
			break;						\
	}while (_V[cnt] == 0);				\
}

#define INC_TX_TSC(_tsc, _cnt)                          \
{                                                       \
    INT i=0;                                            \
	while (++_tsc[i] == 0x0)                            \
    {                                                   \
        i++;                                            \
		if (i == (_cnt))                                \
			break;                                      \
	}                                                   \
}

#define IS_WPA_CAPABILITY(a)       (((a) >= Ndis802_11AuthModeWPA) && ((a) <= Ndis802_11AuthModeWPA1PSKWPA2PSK))

/*
	WFA recommend to restrict the encryption type in 11n-HT mode.
 	So, the WEP and TKIP shall not be allowed to use HT rate.
 */
#define IS_INVALID_HT_SECURITY(_mode)		\
	(((_mode) == Ndis802_11Encryption1Enabled) || \
	 ((_mode) == Ndis802_11Encryption2Enabled))

#define MIX_CIPHER_WPA_TKIP_ON(x)       (((x) & 0x08) != 0)
#define MIX_CIPHER_WPA_AES_ON(x)        (((x) & 0x04) != 0)
#define MIX_CIPHER_WPA2_TKIP_ON(x)      (((x) & 0x02) != 0)
#define MIX_CIPHER_WPA2_AES_ON(x)       (((x) & 0x01) != 0)

/* Some definition are different between Keneral mode and Daemon mode */
#ifdef WPA_DAEMON_MODE
/* The definition for Daemon mode */
#define WPA_GET_BSS_NUM(_pAd)		(_pAd)->mbss_num

#define WPA_GET_PMK(_pAd, _pEntry, _pmk)					\
{															\
	_pmk = _pAd->MBSS[_pEntry->apidx].PMK;					\
}

#define WPA_GET_GTK(_pAd, _pEntry, _gtk)					\
{															\
	_gtk = _pAd->MBSS[_pEntry->apidx].GTK;					\
}

#define WPA_GET_GROUP_CIPHER(_pAd, _pEntry, _cipher)		\
{															\
	_cipher = (_pAd)->MBSS[_pEntry->apidx].GroupEncrypType;	\
}

#define WPA_GET_DEFAULT_KEY_ID(_pAd, _pEntry, _idx)			\
{															\
	_idx = (_pAd)->MBSS[_pEntry->apidx].DefaultKeyId;		\
}

#define WPA_GET_BMCST_TSC(_pAd, _pEntry, _tsc)				\
{															\
	_tsc = 1;												\
}

#define WPA_BSSID(_pAd, _apidx)		(_pAd)->MBSS[_apidx].wlan_addr

#define WPA_OS_MALLOC(_p, _s)		\
{									\
	_p = os_malloc(_s);			\
}

#define WPA_OS_FREE(_p)		\
{								\
	os_free(_p);				\
}

#define WPA_GET_CURRENT_TIME(_time)		\
{										\
	struct timeval tv;					\
	gettimeofday(&tv, NULL);			\
	*(_time) = tv.tv_sec;					\
}

#else
/* The definition for Driver mode */

#define WPA_GET_BSS_NUM(_pAd)		1
#define WPA_GET_GROUP_CIPHER(_pAd, _pEntry, _cipher)				\
	{																\
		_cipher = (_pAd)->StaCfg.GroupCipher;						\
	}
#define WPA_BSSID(_pAd, _apidx) 	(_pAd)->CommonCfg.Bssid

#define WPA_OS_MALLOC(_p, _s)		\
{									\
	kmalloc((u8 **)&_p, _s);		\
}

#define WPA_OS_FREE(_p)		\
{							\
	kfree(_p);	\
}

#define WPA_GET_CURRENT_TIME(_time)		NdisGetSystemUpTime(_time);

#endif /* End of Driver Mode */


/*========================================
	The prototype is defined in cmm_wpa.c
  ========================================*/
void inc_iv_byte(
	u8 *iv,
	UINT len,
	UINT cnt);

BOOLEAN WpaMsgTypeSubst(
	IN u8 EAPType,
	OUT INT *MsgType);

void PRF(
	IN u8 *key,
	IN INT key_len,
	IN u8 *prefix,
	IN INT prefix_len,
	IN u8 *data,
	IN INT data_len,
	OUT u8 *output,
	IN INT len);

int RtmpPasswordHash(
	char *password,
	unsigned char *ssid,
	int ssidlength,
	unsigned char *output);

	void KDF(
	IN u8 * key,
	IN INT key_len,
	IN u8 * label,
	IN INT label_len,
	IN u8 * data,
	IN INT data_len,
	OUT u8 * output,
	IN USHORT len);

u8 * WPA_ExtractSuiteFromRSNIE(
	IN u8 * rsnie,
	IN UINT rsnie_len,
	IN u8 type,
	OUT u8 *count);

void WpaShowAllsuite(
	IN u8 * rsnie,
	IN UINT rsnie_len);

void RTMPInsertRSNIE(
	IN u8 *pFrameBuf,
	OUT PULONG pFrameLen,
	IN u8 * rsnie_ptr,
	IN u8 rsnie_len,
	IN u8 * pmkid_ptr,
	IN u8 pmkid_len);

/*
 =====================================
 	function prototype in cmm_wpa.c
 =====================================
*/
void RTMPToWirelessSta(
	IN struct rtmp_adapter *pAd,
	IN PMAC_TABLE_ENTRY pEntry,
	IN u8 *pHeader802_3,
	IN UINT HdrLen,
	IN u8 *pData,
	IN UINT DataLen,
	IN BOOLEAN bClearFrame);

void WpaDerivePTK(
	IN struct rtmp_adapter *pAd,
	IN u8 *PMK,
	IN u8 *ANonce,
	IN u8 *AA,
	IN u8 *SNonce,
	IN u8 *SA,
	OUT u8 *output,
	IN UINT len);

void WpaDeriveGTK(
	IN u8 *PMK,
	IN u8 *GNonce,
	IN u8 *AA,
	OUT u8 *output,
	IN UINT len);

void GenRandom(
	IN struct rtmp_adapter *pAd,
	IN u8 *macAddr,
	OUT u8 *random);

BOOLEAN RTMPCheckWPAframe(
	IN struct rtmp_adapter *pAd,
	IN PMAC_TABLE_ENTRY pEntry,
	IN u8 *pData,
	IN ULONG DataByteCount,
	IN u8 FromWhichBSSID);

#ifdef HDR_TRANS_SUPPORT
BOOLEAN RTMPCheckWPAframe_Hdr_Trns(
	IN struct rtmp_adapter *pAd,
	IN PMAC_TABLE_ENTRY pEntry,
	IN u8 *pData,
	IN ULONG DataByteCount,
	IN u8 FromWhichBSSID);
#endif /* HDR_TRANS_SUPPORT */

BOOLEAN RTMPParseEapolKeyData(
	IN struct rtmp_adapter *pAd,
	IN u8 *pKeyData,
	IN u8 KeyDataLen,
	IN u8 GroupKeyIndex,
	IN u8 MsgType,
	IN BOOLEAN bWPA2,
	IN MAC_TABLE_ENTRY *pEntry);

void WPA_ConstructKdeHdr(
	IN u8 data_type,
	IN u8 data_len,
	OUT u8 *pBuf);

void ConstructEapolMsg(
	IN PMAC_TABLE_ENTRY pEntry,
	IN u8 GroupKeyWepStatus,
	IN u8 MsgType,
	IN u8 DefaultKeyIdx,
	IN u8 *KeyNonce,
	IN u8 *TxRSC,
	IN u8 *GTK,
	IN u8 *RSNIE,
	IN u8 RSNIE_Len,
	OUT PEAPOL_PACKET pMsg);

PCIPHER_KEY RTMPSwCipherKeySelection(
	IN struct rtmp_adapter *pAd,
	IN u8 *pIV,
	IN RX_BLK *pRxBlk,
	IN PMAC_TABLE_ENTRY pEntry);

int RTMPSoftDecryptionAction(
	IN struct rtmp_adapter *pAd,
	IN u8 *pHdr,
	IN u8 UserPriority,
	IN PCIPHER_KEY pKey,
	INOUT u8 *pData,
	INOUT UINT16 *DataByteCnt);

void RTMPSoftConstructIVHdr(
	IN u8 CipherAlg,
	IN u8 key_id,
	IN u8 *pTxIv,
	OUT u8 *pHdrIv,
	OUT u8 *hdr_iv_len);

void RTMPSoftEncryptionAction(
	IN struct rtmp_adapter *pAd,
	IN u8 CipherAlg,
	IN u8 *pHdr,
	IN u8 *pSrcBufData,
	IN u32 SrcBufLen,
	IN u8 KeyIdx,
	IN PCIPHER_KEY pKey,
	OUT u8 *ext_len);

void RTMPMakeRSNIE(
	IN struct rtmp_adapter *pAd,
	IN UINT AuthMode,
	IN UINT WepStatus,
	IN u8 apidx);

void WPAInstallPairwiseKey(
	struct rtmp_adapter *pAd,
	u8 BssIdx,
	PMAC_TABLE_ENTRY pEntry,
	BOOLEAN bAE);

void WPAInstallSharedKey(
	struct rtmp_adapter *pAd,
	u8 GroupCipher,
	u8 BssIdx,
	u8 KeyIdx,
	u8 Wcid,
	BOOLEAN bAE,
	u8 * pGtk,
	u8 GtkLen);

void RTMPSetWcidSecurityInfo(
	struct rtmp_adapter *pAd,
	u8 BssIdx,
	u8 KeyIdx,
	u8 CipherAlg,
	u8 Wcid,
	u8 KeyTabFlag);

void CalculateMIC(
	IN u8 KeyDescVer,
	IN u8 *PTK,
	OUT PEAPOL_PACKET pMsg);

char *GetEapolMsgType(
	CHAR msg);

#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */

/*
 =====================================
 	function prototype in cmm_wep.c
 =====================================
*/
UINT RTMP_CALC_FCS32(
	IN UINT Fcs,
	IN u8 *Cp,
	IN INT Len);

void RTMPConstructWEPIVHdr(
	IN u8 key_idx,
	IN u8 *pn,
	OUT u8 *iv_hdr);

BOOLEAN RTMPSoftEncryptWEP(
	IN struct rtmp_adapter *pAd,
	IN u8 *pIvHdr,
	IN PCIPHER_KEY pKey,
	INOUT u8 *pData,
	IN ULONG DataByteCnt);

BOOLEAN RTMPSoftDecryptWEP(
	IN struct rtmp_adapter *pAd,
	IN PCIPHER_KEY pKey,
	INOUT u8 *pData,
	INOUT UINT16 *DataByteCnt);

/*
 =====================================
 	function prototype in cmm_tkip.c
 =====================================
*/
BOOLEAN RTMPSoftDecryptTKIP(
	IN struct rtmp_adapter *pAd,
	IN u8 *pHdr,
	IN u8 UserPriority,
	IN PCIPHER_KEY pKey,
	INOUT u8 *pData,
	IN UINT16 *DataByteCnt);

void TKIP_GTK_KEY_WRAP(
	IN u8 *key,
	IN u8 *iv,
	IN u8 *input_text,
	IN u32 input_len,
	OUT u8 *output_text);

void TKIP_GTK_KEY_UNWRAP(
	IN u8 *key,
	IN u8 *iv,
	IN u8 *input_text,
	IN u32 input_len,
	OUT u8 *output_text);

/*
 =====================================
 	function prototype in cmm_aes.c
 =====================================
*/
BOOLEAN RTMPSoftDecryptAES(
	IN struct rtmp_adapter *pAd,
	IN u8 *pData,
	IN ULONG DataByteCnt,
	IN PCIPHER_KEY pWpaKey);

void RTMPConstructCCMPHdr(
	IN u8 key_idx,
	IN u8 *pn,
	OUT u8 *ccmp_hdr);

BOOLEAN RTMPSoftEncryptCCMP(
	IN struct rtmp_adapter *pAd,
	IN u8 *pHdr,
	IN u8 *pIV,
	IN u8 *pKey,
	INOUT u8 *pData,
	IN u32 DataLen);

BOOLEAN RTMPSoftDecryptCCMP(
	IN struct rtmp_adapter *pAd,
	IN u8 *pHdr,
	IN PCIPHER_KEY pKey,
	INOUT u8 *pData,
	INOUT UINT16 *DataLen);

void CCMP_test_vector(
	IN struct rtmp_adapter *pAd,
	IN INT input);

#endif
