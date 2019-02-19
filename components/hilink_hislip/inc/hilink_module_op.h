#ifndef _HILINK_MODULE_OP_H
#define _HILINK_MODULE_OP_H

#ifdef __cplusplus
extern "C" {
#endif
	
#define HISLIP_ACK_CMD	 (0x3F)
	
/* cmds */
#define HLK_CMD_DEVCTL_WM           0x01  /* device control wifi module change work mode */
#define HLK_CMD_DEVCTL_RB           0x02  /* device control wifi module reboot */
#define HLK_CMD_DEVCTL_RST          0x03 /* device control wifi module factory reset */
#define HLK_CMD_DEVCTL_QWM          0x04 /* device query wifi module work mode */
#define HLK_CMD_DEVCTL_QNS          0x05 /* device query wifi module network status */
//#define HLK_CMD_DEVCTL_NTP        0x06 /* device config ntp server address */
#define HLK_CMD_DEVCTL_QRT          0x06 /* device query wifi module current utc time */
#define HLK_CMD_MDLRSP_WM           0x07  /* wifi module tell device work model changed */
#define HLK_CMD_MDLRSP_RST          0x08 /* wifi module tell device factory reset result */
#define HLK_CMD_MDLRSP_RNS          0x09 /* wifi module tell device current network status */
#define HLK_CMD_MDLRSP_UGD          0x0A /* wifi module tell device upgrade information */
#define HLK_CMD_DEVUPD_STA          0x0B/* device update device profile's value */
#define HLK_CMD_DEVCTL_FRLT         0x0C  /* device change property failed report */
#define HLK_CMD_MDCTL_DEV_PROT      0x0D  /* wifi module tell device to change property */
#define HLK_CMD_MDCTL_DEV_QPROT     0x0E  /* wifi module query device's property */

#define HLK_CMD_DEV_QUPG            0x10 /*check if device need upgrade */
#define HLK_CMD_DEV_SUPG            0x11 /* start device upgrade */
#define HLK_CMD_DEV_TRANSING        0x12 /* transmit device's ota bin's data to device */
#define HLK_CMD_DEV_EUPG            0x13 /* stop device's ota bin data transmitting */

#define HLK_CMD_DEVREG_SETSTA       0x2000  /* device regist status set , now delete this cmd */
#define HLK_CMD_DEVREG_DEVINFO      0x2001  /* device regist device info  */
#define HLK_CMD_DEVREG_PROT         0x2002  /* device regist device profile  */
#define HLK_CMD_DEVREG_SVCINFO      0x2003  /* device regist service info  */
#define HLK_CMD_DEVQRY_FINFOS       0x2004  /* device query dev infos(mac rssi...)*/

/* tags */
#define HLK_TAG_OP_RLT              0x00 /* operation result tag */
#define HLK_TAG_WK_MD               0x01 /* work mode tag */
#define HLK_TAG_RB_RSN              0x02 /* reboot reason tag */
#define HLK_TAG_MD_NST              0x03 /* wifi module network status tag */
#define HLK_TAG_UGDST               0x04 /* wifi module upgrade status tag */
#define HLK_TAG_UGDPGS              0x05 /* wifi module upgrade progress tag */
#define HLK_TAG_UTC                 0x06 /* UTC time tag */
#define HLK_TAG_PROT_ITEM           0x07 /* device profile property item tag */
#define HLK_TAG_PROTKIND            0x08 /* device profile property kind tag */
#define HLK_TAG_PROT_SVCINF         0x09 /* device svc_info data tag */
#define HLK_TAG_PROFILE_STR         0x0A  /* profile's string tag in flash */
#define HLK_TAG_PROFILE_TREE        0x0B  /* profile's tag in flash */
#define HLK_TAG_PROFILE_TIME        0x0C  /* profile's tag in flash */
#define HLK_TAG_DEV_SVER            0x10  /* device's software version */
#define HLK_TAG_DEV_BIN_SIZE        0x11  /* device's software ota bin's size */
#define HLK_TAG_DEV_BIN_DATA        0x12  /* device's software ota bin payload*/
#define HLK_TAG_DEV_BIN_CHK         0x13  /* device's software ota bin's checksum */

#define HLK_TAG_PROFILE_VER         0x2000  /* profile's store position, it can get from wifi module or from device */
#define HLK_TAG_WIFI_SSID           0x2001
#define HLK_TAG_WIFI_PWD            0x2002
#define HLK_TAG_WIFI_RAND           0x2003
#define HLK_TAG_NTP                 0x2004 /* ntp server tag */
#define HLK_TAG_REGSTA              0x2005 /* register profile status tag */
#define HLK_TAG_PROT_PRDTID         0x2006 /* device dev_info product id tag */
#define HLK_TAG_PROT_DEVSN          0x2007 /* device dev_info sn tag */
#define HLK_TAG_PROT_PRDTMD         0x2008 /* device dev_info model tag */
#define HLK_TAG_PROT_PRDTTP         0x2009 /* device dev_info type tag */
#define HLK_TAG_PROT_DTENN          0x200A /* device DeviceTypeNameEn tag */
#define HLK_TAG_PROT_MANU           0x200B /* device dev_info manufactory tag */
#define HLK_TAG_PROT_MAUNENN        0x200C /* device ManufacturerFullNameEn tag */

#define HLK_TAG_PROT_CA             0x200D /* CA certificate tag */
#define HLK_TAG_PROT_AC             0x200E /* device AC data  tag */
#define HLK_TAG_PROT_BIRSA          0x200F /* device BI and RSA data tag */

#define HLK_TAG_FINFO_MAC           0x2010 /* device factory info mac data */
#define HLK_TAG_FINFO_HW            0x2011 /* device factory info hardware ver data */
#define HLK_TAG_FINFO_FW            0x2012 /* device factory info software ver data */
#define HLK_TAG_FINFO_APRSSI        0x2013 /* device factory info ap's rssi data */
#define HLK_TAG_FINFO_AP            0x2014 /* device factory info ssid data */

#define HLK_TAG_SDK_TMPD            0x2015 /* hilink sdk flash data tag */

#define HLK_TLV_MIN_LEN 3 /*tlv data's tag and length and data filed data's min length*/
#define HLK_TLV_TL_LEN 2 /*tlv data's tag and length filed data's length*/
#define HLK_INFO_FILED_MIN_LEN 4 /* cmux data's information filed min buffer length */

#define HLK_WKMD_DATA_LEN           1 /* tag work mode's data length */
#define HLK_NSTA_DATA_LEN           1 /* tag wifi module network status's data length */
#define HLK_RST_DATA_LEN            1 /* factory reset result data length */

#define HLK_DEV_INFO_ITEMS_CNT      10 /*device info struct's item count */

#define HLK_DEV_USR_TAG_MIN         0x40
#define HLK_DEV_USR_TAG_MAX         0x7F

#define HLK_DATA_TYPE_STR           0x00
#define HLK_DATA_TYPE_BOOL          0x01
#define HLK_DATA_TYPE_INT           0x02
#define HLK_DATA_TYPE_UNKNOWN       0x7f

#define HLK_TAG_MUL_BYTE_FLAG       0x80
#define HLK_TAG_MUL_BYTE_LEN        2
#define HLK_TAG_MUL_BYTE_LEN        2

#define HLK_INVALID_CMD 0x4000U
#define HLK_INVALID_TAG 0x4000U
#define HLK_INVALID_LEN 0x4000U

#define HLK_DT_ARRAY  0x01  /* tlv's value's type is bytes array */
#define HLK_DT_EATP   0x02  /* tlv's value's type is EA format short */
#define HLK_DT_INT    0x04  /* tlv's value's type is nomal int  */
#define HLK_DT_TLV    0x08  /* tlv's value's type is tlv format arrays  */

#define HLK_MAX_SVCINST_CNT 16
#define HLK_MAX_KEYMAP_CNT 32

#define HLK_MAX_TLVS_CNT 80
#define HLK_SND_BUF_SIZE 640
#define HLK_EXT_BUF_SIZE 64
#define HLK_WACK_MAX_CNT 2

typedef enum _HILINK_REBOOT_REASON
{
    HLK_RB_MDERR = 0x01, /* module return some errors, forced reboot */
    HLK_RB_DTERR = 0x02, /* hilink data error, forced reboot */

}HLK_RB_RSN;

typedef enum _HILINK_UGD_STATUS
{
    HLK_UGD_NONE     = 0x00, /* module return some errors, forced reboot */ 
    HLK_UGD_DOWNLOAD = 0x01, /* downloading upgrade file */
    HLK_UGD_FLASHOK  = 0x02, /* write the upgrade filed finished, waiting for reboot */    

}HLK_UGD_STA;

typedef enum _HILINK_REG_STATUS
{
    HLK_REG_START = 0x00, /* device regist property status start */ 
    HLK_REG_END   = 0x01, /* device regist property status end */
    HLK_REG_MAX   = 0x02, /* device regist property status unknown */    
}HLK_REG_STAT;


typedef enum _HILINK_OPS_RESULT
{
    HLK_RSLT_OK  = 0x00, /* op result , op ok  */
    HLK_RSLT_NOK = 0x01, /* op result , op not ok  */
}HLK_OPS_RSLT;


typedef enum _HILINK_HWCON_STATUS
{
    HLK_HW_NOTCON = 0x00, /* wifi module not connected device yet */
    HLK_HW_CONOK  = 0x01, /* wifi module connected device ok */
    HLK_HW_CONMAX
}HILINK_HWCON_STATUS;



#ifdef __cplusplus
}
#endif

#endif  /* _HILINK_MODULE_OP_H */

