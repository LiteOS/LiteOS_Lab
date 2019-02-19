#ifndef _HISLIP_H
#define _HISLIP_H

#include <stdio.h>
#include <stdint.h>

typedef unsigned char hislip_u8;
typedef unsigned short hislip_u16;
typedef unsigned int hislip_u32;
typedef unsigned long long hislip_u64;
typedef int hislip_s32;
typedef short hislip_s16;
typedef char hislip_s8;


#define HISLIP_IS_PTR_NULL_RET(ptr, ret) \
    if( NULL == (ptr)) \
        return (ret);
    
#define HISLIP_IS_PTR_NULL(ptr) \
    if( NULL == (ptr)) \
        return;


#define HISLIP_PARAM_VAL_LEN                   (1)
#define HISLIP_TLV_LEN_SIZE                    (1)  /* val len size: one byte enough here*/

#define HISLIP_FIRST_PKG_NUM                   (0x00)
#define HISLIP_CRC_TABLE_LEN                   (256)
#define HISLIP_SEND_LEN_MAX                    (15872)//15744?
#define HISLIP_MAX_SEQ_NUM                     (127)

#define HISLIP_PKG_NUM_LEN                     (1)
#define HISLIP_SEQ_NUM_LEN                     (1)
#define HISLIP_FCS_LEN                         (1)
#define HISLIP_FRAME_FIELD_LEN                 (5)
#define HISLIP_MAX_DATA_LEN                    (124)
#define HISLIP_MAX_FRM_INFO_LEN                (126)
#define HISLIP_MAX_FRM_LEN                     (128)
#define HISLIP_MAX_FFRM_DATA_LEN               (123)

#define HISLIP_SEM_WAIT_TIME                   (2000)
#define HISLIP_MUTEX_WAIT_TIME                 (100)

/*information filed, cmd/tag/length max length, now set it to 2 */
#define HISLIP_CTL_MAX_SIZE                    (2)//CTL:CMD TAG LEN MAX SIZE
#define HISLIP_SINGLE_BIT_LEN_MAX              (0x80)

#define HISLIP_FRAME_FLAG                      (0xC0)
#define HISLIP_CTRL_REQ_BIT               	   (0x80)
#define HISLIP_EA_BIT                          (0x80)


#define HISLIP_OK                              (0)
#define HISLIP_NOK                             (1)
#define HISLIP_SUCC                            (0)
#define HISLIP_ERR                             (-1)
#define HISLIP_PTR_NULL                        (-2)
#define HISLIP_RCV_NO_DATA                     (-3)

#define HISLIP_ACK_MAX                         (8)

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

typedef enum
{
    HISLIP_CMD_TYPE = 0x0,
	HISLIP_ACK_TYPE = 0x80,
	HISLIP_CR_MAX
} hislip_cr_type;

typedef enum
{
    HISLIP_SIZE_ERR,
    HISLIP_TYPE_ERR, 
    HISLIP_TIME_OUT,
	HISLIP_ERR_MAX
} hislip_err_code;
    
typedef int (*process_rcvdata_func)(hislip_u8 seq, 
	hislip_u16 cmd, 
	hislip_u8* buf, 
	hislip_u16 len);

typedef struct _last_ack_info_t 
{
	hislip_u16 cmd;
	hislip_u8 seq;
}last_ack_info_t;


typedef struct
{
	hislip_u32 ack_sem;
	hislip_u8 hisseq;
	process_rcvdata_func cmd_func;
    process_rcvdata_func ack_func;
} hislip_rcvcb_t;

typedef struct
{
    void* data;
	hislip_u16 len;
    hislip_u16 frame_size;
	hislip_u8 seq;
	hislip_u8 auto_seq;
	hislip_u8 pkg_num;
} hislip_sendcb_t;

typedef int (*hislip_io_func)(hislip_u8* data, hislip_u16 len);

typedef struct
{
	hislip_sendcb_t sendcb;
    hislip_rcvcb_t rcvcb;
    hislip_io_func io_send;
	hislip_io_func io_rcv;
} hislip_handler_t;

extern void hislip_init(void);
extern hislip_s32 hislip_rcv_data(hislip_u8* data, hislip_u8 len);
extern hislip_s32 hislip_send_data(hislip_u8* data, hislip_u16 len, hislip_u8 cr);
extern hislip_s32 hlk_msg_deal_func(hislip_u8* message, hislip_u16 len);
extern hislip_s32  hlk_update_work_mode(hislip_u8* buf, hislip_u16 len);
extern hislip_s32 hlk_update_net_status(hislip_u8* buf, hislip_u16 len);
extern hislip_u8* hislip_fill_buff_info(hislip_u8* buf, const hislip_u8* data, hislip_u16 wlen);
extern hislip_u8* hislip_read_buf_info(hislip_u8* buf, hislip_u8* data, hislip_u16 rlen);
//extern void hislip_send_ack_msg(hislip_u8 seq, hislip_u16 cmd);
hislip_s32 hislip_send_data_noack(hislip_u8* data, hislip_u16 len, hislip_u8 cr);
void hislip_get_last_acked_info(last_ack_info_t *ackinfo);
void hlk_store_cur_wack_seq(unsigned char seq);

extern void hislip_fill_data_eabit(hislip_s16 data, hislip_u16 *len, hislip_u8 *odata);
extern hislip_s16 hislip_parse_data_eabit(hislip_u8* data, hislip_u16 *len);

extern hislip_u64 hislip_get_cur_time(void);

extern hislip_s32 hislip_uart_write(const hislip_u8* inbuf, hislip_u16 wsize);
extern hislip_s32 hislip_uart_read(hislip_u8* outbuf, hislip_u16 rsize);

#endif /* _HISLIP_H */
