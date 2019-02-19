#ifndef _HILINK_EXAMPLE_H
#define _HILINK_EXAMPLE_H

#include "stdint.h"
#include "hislip.h"

#define HISLIP_NO_MSG_TIMEOUT                  (10000)

#define HLK_RUN_STATE_INIT			0x00
#define HLK_RUN_STATE_REGPROFILE	(HLK_RUN_STATE_INIT + 1)
#define HLK_RUN_STATE_SETWKMODE     (HLK_RUN_STATE_REGPROFILE + 1)
#define HLK_RUN_STATE_RPTINIVAL     (HLK_RUN_STATE_SETWKMODE + 1)
#define HLK_RUN_STATE_IDLE			(HLK_RUN_STATE_RPTINIVAL + 1)
#define HLK_RUN_STATE_MAX			(HLK_RUN_STATE_IDLE + 1)

#define HLK_WAIT_ACK 0x01
#define HLK_NWAIT_ACK 0x00


#define FUNC_PROC_STATE_SNDCMD      (0)
#define FUNC_PROC_STATE_WACK        (FUNC_PROC_STATE_SNDCMD + 1)
#define FUNC_PROC_STATE_MAX         (FUNC_PROC_STATE_WACK + 1)

#define ACK_STATE_WAITTING      0
#define ACK_STATE_ACKED         (ACK_STATE_WAITTING + 1)
#define ACK_STATE_ACKTIMOUT     (ACK_STATE_ACKED + 1)
#define ACK_STATE_MAX           (ACK_STATE_ACKTIMOUT + 1)
#define ACK_TIMEOUT_VALUE       (500)

#define HLK_RET_ERR        -2 /* unknown err */
#define HLK_RET_SFAIL      -1 /* send data failed */
#define HLK_RET_OK          0 /* function process ok*/
#define HLK_RET_WACK        1 /* waiting ack */ 
#define HLK_RET_ACK_TMOUT   2 /* waiting ack timed out */ 
#define HLK_RET_NEXTSTAGE   3 /* function process ok*/

#define HLK_RPT_PROCESSING  0 /* now is reporting init values */
#define HLK_RPT_DONE        1 /* now report init values finished */

#define DEV_REPORT_LATER 0
#define DEV_REPORT_NOW 1


/* error codes for debugging this code */
#define HLK_ECODE_RCV_TAG_ERR               0x01 /* recive tlv's tag is invalid */
#define HLK_ECODE_RCV_LEN_ERR               0x02 /* recive tlv's len is invalid */
#define HLK_ECODE_PROFILE_TAG_ERR           0x03 /* profile's tag(key id or svc id ) is invalid */
#define HLK_ECODE_PROFILE_LEN_ERR           0x04 /* profile's length data is error */
#define HLK_ECODE_TLVARRAY_SIZE_SVCSMALL    0x05 /* global var(g_tlvs)'s size is smaller than profile need */
#define HLK_ECODE_PROFILE_SVCINST_ERR       0x06 /* profile's svc instance's id error */
#define HLK_ECODE_TLVARRAY_SIZE_KEYSMALL    0x07 /* global var(g_tlvs)'s size is smaller than svc tlv need */
#define HLK_ECODE_PROFILE_KEYMAP_ERR        0x08 /* global var(g_tlvs)'s size is smaller than key's tlv need */
#define HLK_ECODE_EXT_BUF_SIZE_SMALL        0x09 /* g_ext_tmp_buf's size is small than needed */
#define HLK_ECODE_KEYMAP_DATA_TYPE_ERR      0x0A /* profile's key's data type init error */
#define HLK_ECODE_RCV_WKMODE_ERR            0x10 /* recive work mode can't find tag_workmode */
#define HLK_ECODE_RCV_NETSTA_ERR            0x11 /* recive net status can't find tag_netstatus */
#define HLK_ECODE_RCV_UNKNOWN_CMD           0x12 /* recive an unknown cmd from wifi module */
#define HLK_ECODE_NO_BUF_TO_STORE_STR       0x13 /* in hilink_msg_table[] str data's buf not setted */

typedef enum _HILINK_WORK_MODE
{
    WKMD_UNKONWN  = 0x00, /* wifi module is initializing */
    WKMD_OFFLINE  = 0x01, /* offline mode, means wifi module init ok, waitint for device cmd or data */
    WKMD_ONLINE   = 0x02, /* online mode, means start hilink and connected wifi */
    WKMD_AUTOAP   = 0x03, /* auto config wifi ap information mode, user can find hilink device by app */
    WKMD_MAX
}HILINK_WORK_MODE;

typedef enum _HILINK_NET_STATUS
{
    NET_NOAP     = 0x00, /* hilink module don't have any wifi ap info , it can't connect to the wifi router */
    NET_NOTCONN  = 0x01, /* hilink module has ap info, but not connected to the routor*/
    NET_CONWIFI  = 0x02, /* hilink module has connected to the wifi routor, but hilink not connect server yet */
    NET_ONLINE   = 0x03, /* hilink already connected iot server */
    NET_UNKONWN  = 0x04
}HILINK_NET_STATUS;


typedef hislip_s32 (*request_func)(void);
typedef hislip_s32 (*process_ack_state)(hislip_u16 cmd, hislip_u64 starttick);

typedef struct _hlk_tlv_t
{
    hislip_u16 tag;
    hislip_u16 len;
    hislip_u8 *val;
}hlk_tlv_t;

typedef struct _hlk_func_par_t
{
    hislip_s16 cmd;
    request_func send_request;
    process_ack_state process_ack_state_chg;
}hlk_func_par_t;

typedef hislip_s32 (*pfun)(hlk_func_par_t *stagefun);

typedef struct _hlk_func_matrix_t
{
    hlk_func_par_t *params;
    hislip_u16 curused;
    hislip_u16 funcscnt;
}hlk_func_matrix_t;

extern hislip_u8 ota_status;

void hilink_env_init(void);
extern HILINK_WORK_MODE hlk_get_loacal_work_mode(void);
extern HILINK_NET_STATUS hlk_get_loacal_net_status(void);
hislip_s32 hlk_update_one_key_val(hislip_s16 svcid, hislip_s16 keyid, 
                                hislip_s32 val, hislip_s8 *str);

extern void hilink_main_process(void);
extern hislip_s32 hislip_send_ack_msg(hislip_u8 seq, 
	hislip_u16 cmd, 
    hislip_u8 result);
extern hislip_s32 hlk_cmd_process(hislip_u8 seq, hislip_u16 cmd, 
	hislip_u8* buf, 
	hislip_u16 len);
extern hislip_s32 hlk_ack_process(hislip_u8 seq, 
	hislip_u16 cmd, 
	hislip_u8* buf, 
	hislip_u16 len);
extern hislip_s32 hlk_req_check_upg(hislip_u8 seq, 
	hislip_u16 cmd, 
    hislip_u8* buf, 
    hislip_u16 len);
extern hislip_s32 hlk_start_upg(hislip_u8 seq,
	hislip_u16 cmd,
    hislip_u8* buf, 
    hislip_u16 len);
extern hislip_s32 hlk_rcv_upg_data(hislip_u8 seq, 
	hislip_u16 cmd, 
    hislip_u8* buf, 
    hislip_u16 len);
extern hislip_s32 hlk_end_upg(hislip_u8 seq,
	hislip_u16 cmd,
    hislip_u8* buf, 
    hislip_u16 len);
extern hislip_s32 hlk_tell_upg_info(hislip_u8 seq, 
	hislip_u16 cmd, 
    hislip_u8* buf, 
    hislip_u16 len);
void hlk_notify_error_info(hislip_u32 hlkerrno);
extern hislip_s32 device_get_factory_infors(hislip_s8 *buf, hislip_s32 len);
hislip_s32 hlk_dev_reset(void);
hislip_s32 hlk_dev_reboot(void);

#endif /* _HILINK_EXAMPLE_H */
