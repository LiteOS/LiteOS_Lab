/*----------------------------------------------------------------------------
 * Copyright (c) <2017-2018>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 *---------------------------------------------------------------------------*/
#include <string.h>
#include "hislip_uart.h"

static hislip_u8 uart_buf[HISLIP_UART_BUF_SIZE] = {0};
static uart_handler_t gst_uart_handler;
static hislip_u8 gskb_write = 0;
static hislip_u8 gskb_read = 0;
static hislip_u16 gframe_len = 0;

extern hislip_s32 hislip_uart_send_one_byte(hislip_u8 data);

hislip_s32 hislip_uart_res_init(void)
{
	gst_uart_handler.write_pos = 0;
	gst_uart_handler.rx_overlap = 0;
	gst_uart_handler.is_esc_state = 0;
	gst_uart_handler.buf = uart_buf;
	gst_uart_handler.buf_size = sizeof(uart_buf);
    
    gskb_write = 0;
    gskb_read = 0;

	(void)memset(gst_uart_handler.skb, 0x0, sizeof(uart_skb_t) * HISLIP_UART_SKB_NUM);
	return HISLIP_SUCC;
}

/*****************************************************************************
 Function    : hislip_uart_rcv_one_byte
 Description : uart write buf
 Input       : hislip_u8 data
 Output      : None
 Return      : HISLIP_SUCC
 *****************************************************************************/
hislip_s32 hislip_uart_rcv_one_byte(hislip_u8 data)
{	
	if (gst_uart_handler.is_esc_state)
	{
		if (ESC_END != data && ESC_ESC != data)
		{
			gst_uart_handler.skb[gskb_write].len = 0;
			gst_uart_handler.skb[gskb_write].start_pos = gst_uart_handler.write_pos;
			gframe_len = 0;
			gst_uart_handler.is_esc_state = 0;
		}
	}
	switch(data)
	{
    	case END:
    		if (gframe_len)
    		{
    			gst_uart_handler.skb[gskb_write].len = gframe_len;
    			gskb_write = (gskb_write + 1) % HISLIP_UART_SKB_NUM;
    			if (gskb_write == gskb_read)
                {
                    /* here skb buf write through , read should go next */
                    gst_uart_handler.skb[gskb_read].len = 0;
                    gskb_read = (gskb_read + 1) % HISLIP_UART_SKB_NUM;
                }
    			gst_uart_handler.skb[gskb_write].start_pos = gst_uart_handler.write_pos;
    			gframe_len = 0;
    		}
    		return HISLIP_SUCC;
    	case ESC:
    		gst_uart_handler.is_esc_state = 1;
    	    break;
    	case ESC_END:
    		if(gst_uart_handler.is_esc_state){
    			data = END;
    			gst_uart_handler.is_esc_state = 0;
    		}
    	case ESC_ESC:
    		if(gst_uart_handler.is_esc_state){
    			data = ESC;
    			gst_uart_handler.is_esc_state = 0;
    		}	
    	default:
    		gframe_len++;
    		gst_uart_handler.buf[gst_uart_handler.write_pos++] = data;
    		break;
	}

	if (gst_uart_handler.buf_size == gst_uart_handler.write_pos)
    {
		gst_uart_handler.write_pos = 0;
		gst_uart_handler.rx_overlap = 1;
	}

	if (gst_uart_handler.rx_overlap && (gst_uart_handler.write_pos == gst_uart_handler.skb[gskb_read].start_pos))
	{
		gst_uart_handler.skb[gskb_read].len = 0;
		gskb_read = (gskb_read + 1) % HISLIP_UART_SKB_NUM;
	}
	
	return HISLIP_SUCC;
}

/*****************************************************************************
 Function    : hislip_uart_write
 Description : uart write buf
 Input       : char *inbuf, 
               uint32_t wsize
 Output      : None
 Return      : write len
 *****************************************************************************/
hislip_s32 hislip_uart_write(const hislip_u8* inbuf, hislip_u16 wsize)
{
	hislip_u32 len = 0;
	
	if ((wsize < HISLIP_FRAME_MIN_LEN) || (!inbuf))
    {
		return 0;
	}

	hislip_uart_send_one_byte(*inbuf++);

	while (len < (wsize - HISLIP_FLAG_LEN))
	{
		switch (*inbuf)
		{
    		case END:
    			hislip_uart_send_one_byte(ESC);
    			hislip_uart_send_one_byte(ESC_END);
    			break;
    		case ESC:
    			hislip_uart_send_one_byte(ESC);
    			hislip_uart_send_one_byte(ESC_ESC);
    			break;
    		default:
    			hislip_uart_send_one_byte(*inbuf);
    			break;
		}
		
		inbuf++;
		len++;
	}

	hislip_uart_send_one_byte(*inbuf);

	return wsize;
}

/*****************************************************************************
 Function    : hislip_uart_read
 Description : uart read buf
 Input       : uint32_t rsize
 Output      : char *outbuf
 Return      : read len
 *****************************************************************************/
hislip_s32 hislip_uart_read(hislip_u8* outbuf, hislip_u16 rsize)
{
	hislip_s32 i = 0;
	hislip_u16 len = 0;
	hislip_s32 read_byte = 0;

	if (NULL == outbuf || rsize <= 0)
    {
        return 0;
    }
	
	for (i = gskb_read; i < HISLIP_UART_SKB_NUM; i++)
	{
		if (gst_uart_handler.skb[i].len)
		{
			read_byte = gst_uart_handler.skb[i].len;
			gskb_read = i;
			break;
		}
	}
		
	if (!read_byte)
    {
        return 0;
    }
	else
    {
		if (gst_uart_handler.skb[i].len < (HISLIP_FRAME_MIN_LEN) 
			|| gst_uart_handler.skb[i].len > rsize)
		{
			gst_uart_handler.skb[gskb_read].len = 0;
		    gskb_read = (gskb_read + 1) % HISLIP_UART_SKB_NUM;
			return 0;
		}
	}

	if(gst_uart_handler.skb[gskb_read].start_pos + read_byte <= gst_uart_handler.buf_size)
    {
		memcpy((hislip_u8*)outbuf, gst_uart_handler.buf + gst_uart_handler.skb[gskb_read].start_pos, read_byte);
	}
	else
    { //overlap
		len = gst_uart_handler.buf_size - gst_uart_handler.skb[gskb_read].start_pos;
		memcpy((hislip_u8*)outbuf, gst_uart_handler.buf + gst_uart_handler.skb[gskb_read].start_pos, len);
		memcpy((hislip_u8*)(outbuf + len), gst_uart_handler.buf, read_byte - len);
		gst_uart_handler.rx_overlap = 0;
	}

	gst_uart_handler.skb[gskb_read].len = 0;
	gskb_read = (gskb_read + 1) % HISLIP_UART_SKB_NUM;
    
	return read_byte;
}
