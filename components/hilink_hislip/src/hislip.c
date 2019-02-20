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
#include <stdlib.h>
#include "hislip.h"
#include "hislip_uart.h"
#include "hilink_module_op.h"
#include "hilink.h"

static hislip_s32 hislip_build_frame(hislip_sendcb_t* send_cb, hislip_u8** fbuf);
static hislip_s32 hislip_send_frame(hislip_u8* data, hislip_u16 len);
static hislip_s32 hislip_io_send_data(hislip_u8* buf, hislip_u16 len);
static hislip_s32 hislip_io_rcv_data(hislip_u8* buf, hislip_u16 len);
static hislip_s32 hislip_crc_check(hislip_u8* data, hislip_u8 len);
static hislip_s32 hislip_process_pkg_data(hislip_u8* data, hislip_u16 len);
static hislip_s32 hislip_deal_ack(hislip_u8 seq, 
	hislip_u16 cmd, 
	hislip_u8* data, 
	hislip_u16 len);

static const hislip_u8 hislip_crc_table[HISLIP_CRC_TABLE_LEN] =
{
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15, 0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d,
    0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65, 0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d,
    0xe0, 0xe7, 0xee, 0xe9, 0xfc, 0xfb, 0xf2, 0xf5, 0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
    0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85, 0xa8, 0xaf, 0xa6, 0xa1, 0xb4, 0xb3, 0xba, 0xbd,
    0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2, 0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea,
    0xb7, 0xb0, 0xb9, 0xbe, 0xab, 0xac, 0xa5, 0xa2, 0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
    0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32, 0x1f, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0d, 0x0a,
    0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42, 0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a,
    0x89, 0x8e, 0x87, 0x80, 0x95, 0x92, 0x9b, 0x9c, 0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
    0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec, 0xc1, 0xc6, 0xcf, 0xc8, 0xdd, 0xda, 0xd3, 0xd4,
    0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c, 0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44,
    0x19, 0x1e, 0x17, 0x10, 0x05, 0x02, 0x0b, 0x0c, 0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
    0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b, 0x76, 0x71, 0x78, 0x7f, 0x6a, 0x6d, 0x64, 0x63,
    0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b, 0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13,
    0xae, 0xa9, 0xa0, 0xa7, 0xb2, 0xb5, 0xbc, 0xbb, 0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8d, 0x84, 0x83,
    0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb, 0xe6, 0xe1, 0xe8, 0xef, 0xfa, 0xfd, 0xf4, 0xf3
};
static last_ack_info_t hislip_last_ack;
static hislip_handler_t hislip_handler;
static hislip_u8 hislip_send_buf[HISLIP_MAX_FRM_LEN] = {0};
/*****************************************************************************
Function     : hislip_init
Description  : init hislip moudle
Input        : None
Output       : None
Return       : None
*****************************************************************************/
void hislip_init(void)
{
    hislip_handler.io_send = hislip_io_send_data;
    hislip_handler.io_rcv = hislip_io_rcv_data;
    hislip_handler.rcvcb.cmd_func = hlk_cmd_process;
    hislip_handler.rcvcb.ack_func = hlk_ack_process;
    hislip_handler.rcvcb.ack_sem = 0;
    hislip_handler.sendcb.seq = 0;
    hislip_handler.sendcb.auto_seq = 0;
	
    hislip_uart_res_init();
    
    return;
}

/*****************************************************************************
Function     : hislip_build_frame
Description  : build hislip frame data
Input        : hislip_sendcb_t* send_cb
Output       : hislip_u8** fbuf
Return       : HISLIP_OK/HISLIP_SIZE_ERR
*****************************************************************************/
static hislip_s32 hislip_build_frame(hislip_sendcb_t* send_cb, hislip_u8** fbuf)
{
    int i = 0;
    hislip_u8 fcs = 0x0;
    hislip_u8* frame = NULL;
    hislip_u8* fp = NULL;

    if (send_cb->len > HISLIP_MAX_DATA_LEN)
    {
        return HISLIP_SIZE_ERR;
    }
    
    (void)memset(hislip_send_buf, 0, sizeof(hislip_send_buf));
    frame = fp = hislip_send_buf;
    /* Flag */
    *fp = HISLIP_FRAME_FLAG;
	
	/* Pkg num */
	*++fp = send_cb->pkg_num;
	fcs = hislip_crc_table[fcs ^ *fp];
	
	/* Seq */
	if (HISLIP_FIRST_PKG_NUM == (send_cb->pkg_num & (~HISLIP_EA_BIT)))
	{
		*++fp = send_cb->seq;
		fcs = hislip_crc_table[fcs ^ *fp];
		send_cb->frame_size = send_cb->len + HISLIP_FRAME_FIELD_LEN;
	}
	else
	{
		send_cb->frame_size = send_cb->len + HISLIP_FRAME_FIELD_LEN - HISLIP_SEQ_NUM_LEN;
	}
	
    /* Information */
    (void)memcpy(++fp, send_cb->data, send_cb->len);
    for (i = 0; i < send_cb->len; i++)
    {
        fcs = hislip_crc_table[fcs ^ *fp++]; 
    }

    *fp = fcs;
        
    *++fp = HISLIP_FRAME_FLAG;
    *fbuf = frame;

    return HISLIP_OK;
}

/*****************************************************************************
Function     : hislip_send_frame
Description  : hislip send frame data
Input        : hislip_u8* data, 
			   hislip_u16 len
Output       : None
Return       : HISLIP_OK/HISLIP_ERR
*****************************************************************************/
static hislip_s32 hislip_send_frame(hislip_u8* data, 
    hislip_u16 len)
{
    hislip_handler_t* handler = &hislip_handler;
    hislip_u8* build_frame = NULL;

    if (len > HISLIP_MAX_DATA_LEN)
    {
        return HISLIP_SIZE_ERR;
    }
    
    handler->sendcb.data = data;
	handler->sendcb.len = len;
    handler->sendcb.frame_size = 0;

    (void)hislip_build_frame(&handler->sendcb, &build_frame);
    if (0 == handler->io_send(build_frame, handler->sendcb.frame_size))
    {
        return HISLIP_ERR;
    }

    return HISLIP_OK;
}

/*****************************************************************************
Function     : hislip_get_valseq
Description  : get current valid seq number
Input        : None
Output       : None
Return       : seq number
*****************************************************************************/
static hislip_u8 hislip_get_valseq(void)
{
	hislip_handler.sendcb.auto_seq++;
	hislip_handler.sendcb.auto_seq %= HISLIP_MAX_SEQ_NUM;
	return hislip_handler.sendcb.auto_seq;
}

/*****************************************************************************
Function     : hislip_send_data
Description  : hislip create frame data and send
Input        : u8* data, u16 len, hislip_u8 cr
Output       : None
Return       : HISLIP_OK/HISLIP_ERR
*****************************************************************************/
hislip_s32 hislip_send_data(hislip_u8* data, hislip_u16 len, hislip_u8 cr)
{
    int ret = HISLIP_OK;
    hislip_u8 pkg_num = 0x00;
    hislip_u8* tmp_data = data;
    hislip_u8 seq = 0;
    hislip_u16 left = len;

	
    if ((0 == len) || (len > HISLIP_SEND_LEN_MAX))
    {
        return HISLIP_ERR;
    }

    if (cr & HISLIP_ACK_TYPE)
    {
        seq = cr;
    }
    else
    {
        seq = hislip_get_valseq();
        seq |= cr;
    }
    hislip_handler.sendcb.seq = seq;
    if (left > HISLIP_MAX_FFRM_DATA_LEN)
    {
        hislip_handler.sendcb.pkg_num = pkg_num;	
        (void)hislip_send_frame(tmp_data, HISLIP_MAX_FFRM_DATA_LEN);
        pkg_num++;
        tmp_data += HISLIP_MAX_FFRM_DATA_LEN;
        left -= HISLIP_MAX_FFRM_DATA_LEN;
		
        while (left > HISLIP_MAX_DATA_LEN)
        {
            hislip_handler.sendcb.pkg_num = pkg_num;
            (void)hislip_send_frame(tmp_data, HISLIP_MAX_DATA_LEN);
            pkg_num++;
            tmp_data += HISLIP_MAX_DATA_LEN;
            left -= HISLIP_MAX_DATA_LEN;
        }
    }
    hislip_handler.sendcb.pkg_num = HISLIP_EA_BIT | pkg_num;
    ret = hislip_send_frame(tmp_data, left);

    if (!(cr & HISLIP_ACK_TYPE))
    {   
        /* store the seq value which we need wait its ack */
        hlk_store_cur_wack_seq(seq);
    }

    return ret;
}

hislip_s32 hislip_send_data_noack(hislip_u8* data, hislip_u16 len, hislip_u8 cr)
{
    hislip_s32 ret = HISLIP_OK;
    hislip_u8 pkg_num = 0x00;
    hislip_u8* tmp_data = data;
    hislip_u8 seq = 0;
    hislip_u16 left = len;
    
    if ((0 == len) || (len > HISLIP_SEND_LEN_MAX))
    {
        return HISLIP_ERR;
    }

    if (cr & HISLIP_ACK_TYPE)
    {
        seq = cr;
    }
    else
    {
        seq = hislip_get_valseq();
        seq |= cr;
    }
    hislip_handler.sendcb.seq = seq;

    while (left > HISLIP_MAX_DATA_LEN)
    {
        hislip_handler.sendcb.pkg_num = pkg_num;
    	
        (void)hislip_send_frame(tmp_data, HISLIP_MAX_DATA_LEN);
        pkg_num++;
        tmp_data += HISLIP_MAX_DATA_LEN;
        left -= HISLIP_MAX_DATA_LEN;
    }

    hislip_handler.sendcb.pkg_num = HISLIP_EA_BIT | pkg_num;
    ret = hislip_send_frame(tmp_data, left);

    return ret;
}
hislip_u8* hislip_fill_buff_info(hislip_u8* buf, const hislip_u8* data, hislip_u16 wlen)
{
    (void)memcpy(buf, data, wlen);
    return (hislip_u8*)(buf + wlen);
}

hislip_u8* hislip_read_buf_info(hislip_u8* buf, hislip_u8* data, hislip_u16 rlen)
{
    (void)memcpy(data, buf, rlen);
    return (hislip_u8*)(buf + rlen);
}

void hislip_fill_data_eabit(hislip_s16 data, hislip_u16 *len, hislip_u8 *odata)
{
    hislip_s32 dlen = 0;
    hislip_u8 tmp = 0x00;
    hislip_u8 rdat[HISLIP_CTL_MAX_SIZE]; 
    
    if (data < HISLIP_SINGLE_BIT_LEN_MAX)
    {
        dlen = 1;
        tmp = (hislip_u8)data;
        tmp = tmp | HISLIP_EA_BIT;
        rdat[0] = tmp;
    }
    else
    {
        tmp = (hislip_u8)(data >> 7);
        if (tmp > 0x7f)
        {
            if (len)
            {
                *len = 0;
            }
            if (odata)
            {
                odata[0] = 0x0;
            }
            /* the data's value over max value that in hislip protocol */
            return ;
        }
        tmp = tmp & (~HISLIP_EA_BIT);
        rdat[0] = tmp;
        tmp = (hislip_u8)(data & 0x7f);
        tmp = tmp | HISLIP_EA_BIT;
        rdat[1] = tmp;
        dlen = HISLIP_CTL_MAX_SIZE;
    }
    if (len)
    {
        *len = dlen;
    }
    if (odata)
    {
        (void)memcpy(odata, rdat, dlen);
    }
    return ;
}

hislip_s16 hislip_parse_data_eabit(hislip_u8* data, hislip_u16 *len)
{
    hislip_s16 rdat = -1;
    hislip_u8 tmp;
    
    if (NULL == data)
    {
        return rdat;
    }
    tmp = data[0];
    if (tmp & HISLIP_EA_BIT)
    {
        tmp = (tmp & (~HISLIP_EA_BIT));
        rdat = tmp;
        if (len)
        {
            *len = 1;
        }
    }
    else
    {
        rdat = tmp << 7;
        tmp = data[1];
        if (!(tmp & HISLIP_EA_BIT))
        {
            /* error happend, so return -1 */
            rdat = -1;
            return rdat;
        }
        tmp = (tmp & (~HISLIP_EA_BIT));
        rdat += tmp;
        if (len)
        {
            *len = HISLIP_CTL_MAX_SIZE;
        }
    }
    return rdat;
}

static hislip_s32 hislip_crc_check(hislip_u8* data, hislip_u8 len)
{
	hislip_s32 i = 0;
	hislip_u8 fcs = 0x0;
	
    for (i = 0; i < (len - HISLIP_FCS_LEN); i++)
    {
        fcs = hislip_crc_table[(hislip_u8)fcs ^ *data];
        data++;
    }
	
	if(fcs == (*data))
    {
		return HISLIP_SUCC;
	}
	
    return HISLIP_ERR;
}

void hislip_set_last_acked_data(hislip_u16 cmd, hislip_u8 seq_num)
{
    hislip_last_ack.cmd = cmd;
    hislip_last_ack.seq = seq_num;
}

void hislip_get_last_acked_info(last_ack_info_t *ackinfo)
{
    if (ackinfo)
    {
        ackinfo->cmd = hislip_last_ack.cmd;
        ackinfo->seq = hislip_last_ack.seq;
    }
    return ;
}

static hislip_s32 hislip_deal_ack(hislip_u8 seq, hislip_u16 cmd, hislip_u8* data, hislip_u16 len)
{
    hislip_s32 ret = HISLIP_ERR;
    
	hislip_set_last_acked_data(cmd, (seq & (~HISLIP_CTRL_REQ_BIT)));
    
    if (hislip_handler.rcvcb.ack_func)
    {
        ret = hislip_handler.rcvcb.ack_func(seq & (~HISLIP_CTRL_REQ_BIT), cmd, data, len);
    }		  
    return ret;
}

/*****************************************************************************
Function     : hislip_process_pkg_data
Description  : Call  deal_func process rcv data 
Input        : hislip_u8* data, 
               hislip_u16 len
Output       : None
Return       : HISLIP_SUCC/HISLIP_ERR
*****************************************************************************/
static hislip_s32 hislip_process_pkg_data(hislip_u8* data, hislip_u16 len)
{
    hislip_s32 ret = HISLIP_SUCC;
    hislip_u8* ptmp = NULL;
    hislip_u8 pkg_num;
    hislip_s32 val_len;
    hislip_u16 rlen = 0;
    hislip_u8 seq = 0x0;
    hislip_u16 cmd = 0;
	
    /* we don't meet any multi packages for one information filed data yet */
    ptmp = hislip_read_buf_info(data, &pkg_num, HISLIP_PKG_NUM_LEN);
    if (!(pkg_num & HISLIP_EA_BIT))
    {
        /*
            package number not the first package's number, so, discard it.
        */
        return HISLIP_ERR;
    }

    ptmp = hislip_read_buf_info(ptmp, &seq, HISLIP_SEQ_NUM_LEN);
    cmd = hislip_parse_data_eabit(ptmp, &rlen);

    if (hislip_handler.rcvcb.hisseq == seq)
    {
        /*this if is used to process retrans package */

        if(HISLIP_ACK_TYPE != (seq & HISLIP_CTRL_REQ_BIT))
        {
            hislip_send_ack_msg(seq, cmd, HISLIP_OK);
        }
        return HISLIP_SUCC;
    }

    hislip_handler.rcvcb.hisseq = seq;

    ptmp = ptmp + rlen;
    val_len = len - HISLIP_PKG_NUM_LEN - HISLIP_SEQ_NUM_LEN - rlen;
    if (HISLIP_ACK_TYPE == (seq & HISLIP_CTRL_REQ_BIT))
    {
        (void)hislip_deal_ack(seq & (~HISLIP_CTRL_REQ_BIT), cmd, ptmp, val_len);
    }
    else
    {
        if(hislip_handler.rcvcb.cmd_func)
        {
            ret = hislip_handler.rcvcb.cmd_func(seq & (~HISLIP_CTRL_REQ_BIT), cmd, ptmp, val_len);
        }
    }
    return ret;
}

/*****************************************************************************
Function     : hislip_rcv_data
Description  : hislip recive data and deal 
Input        : u8* data, u16 len
Output       : None
Return       : HISLIP_OK/HISLIP_ERR
*****************************************************************************/
hislip_s32 hislip_rcv_data(hislip_u8* data, hislip_u8 len)
{
    hislip_u16 rcv_len = 0;
    hislip_s32 ret = HISLIP_OK;
    hislip_handler_t* handler = &hislip_handler;

    if (NULL == data || 0 == len)
    {
        return HISLIP_ERR;
    }

    memset(data, 0x0, len);

    rcv_len = handler->io_rcv(data, len);
    if (0 == rcv_len)
    {
        return HISLIP_RCV_NO_DATA;
    }
    else
    { 
        if (HISLIP_ERR == hislip_crc_check(data, rcv_len))
        {
            //PRINT_ERR("hislip crc check err.");
            return HISLIP_ERR;
        }

        ret = hislip_process_pkg_data(data, rcv_len - HISLIP_FCS_LEN);  
        if (ret != HISLIP_SUCC)
        {
            //PRINT_ERR("hislip process rcvd frame err, ret = [%d].", ret);
        }
    }

    return ret;
}

/**************************************************************************************************
 *  功能：通过硬件设备发送数据                                                                    *
 **************************************************************************************************/
static hislip_s32 hislip_io_send_data(hislip_u8* buf, hislip_u16 len)
{
    return hislip_uart_write(buf, len);
}

static hislip_s32 hislip_io_rcv_data(hislip_u8* buf, hislip_u16 len)
{
    return hislip_uart_read(buf, len);
}

