#ifndef _HISLIP_UART_H
#define _HISLIP_UART_H

#include "hislip.h"

#define END              		(0xC0)
#define ESC              		(0xDB)
#define ESC_END          		(0xDC)
#define ESC_ESC          		(0xDD)

#define HISLIP_UART_BUF_SIZE       (512U)
#define HISLIP_UART_SKB_NUM        (16U)
#define HISLIP_FRAME_MIN_LEN       (3U)
#define HISLIP_FLAG_LEN            (2U)

typedef struct uart_skb
{
	hislip_s16 start_pos;
	hislip_u16 len;
}uart_skb_t;

typedef struct uart_handler
{
    hislip_u16 write_pos;
    hislip_u16 rx_overlap;
	hislip_u8 is_esc_state;
    hislip_u8* buf;
    hislip_u16 buf_size;
    uart_skb_t skb[HISLIP_UART_SKB_NUM];
}uart_handler_t;

extern hislip_s32 hislip_uart_res_init(void);
extern hislip_s32 hislip_uart_rcv_one_byte(hislip_u8 data);
extern hislip_s32 hislip_uart_send_one_byte(hislip_u8 data);

#endif /* _HISLIP_UART_H */
