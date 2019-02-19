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
#include <stdint.h>
#include <stdlib.h>
#include "hilink_module_op.h"
#include "dev_com_struct.h"
#include "hislip.h"
#include "hislip_uart.h"
#include "hilink.h"

/* Private types -------------------------------------------------------------*/ 
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
#define HLK_REPORT_RESULT_TME  (30000)

/* Private function declare --------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

hislip_u8 ota_status = 0;

static HILINK_NET_STATUS mod_netsta = NET_UNKONWN;
static HILINK_WORK_MODE mod_workmode = WKMD_UNKONWN;

static hislip_s32 g_hlk_run_state = HLK_RUN_STATE_INIT;
static hislip_s32 g_hlk_func_state = FUNC_PROC_STATE_SNDCMD;

hislip_s32 g_profile_send = 0;

static hislip_u8 hlk_get_key_datatype(hislip_u16 mapid);

static hislip_u8 g_snd_tmp_buf[HLK_SND_BUF_SIZE] = {0};
static hislip_u8 g_ext_tmp_buf[HLK_EXT_BUF_SIZE] = {0};
static hlk_tlv_t g_tlvs[HLK_MAX_TLVS_CNT];
static hislip_u32 g_init_val_idx = 0;
static hislip_s32 g_rpt_initval_state = HLK_RPT_PROCESSING;
static hislip_u8 g_wack_seq[HLK_WACK_MAX_CNT] = {0};

static hislip_s32 hlk_req_qwork_mode(void);
static hislip_s32 hlk_req_qnet_status(void);
static hislip_s32 hlk_wait_ack_common(hislip_u16 cmd, hislip_u64 starttick);

static hislip_s32 hlk_req_reg_start(void);
static hislip_s32 hlk_req_reg_ac_rsa(void);
static hislip_s32 hlk_req_reg_devinfo(void);
static hislip_s32 hlk_req_reg_svcinfo(void);
static hislip_s32 hlk_req_reg_keymapinfo(void);
static hislip_s32 hlk_req_reg_end(void);

static hislip_s32 hlk_req_set_workmode(void);
static hislip_s32 hlk_req_report_init_values(void);
static hislip_s32 hlk_report_profile_value(void);

static hislip_s32 hlk_process_stage_func(hlk_func_par_t *stagefunc, hislip_u32 idx);
static hislip_s32 hlk_process_idle_state(hlk_func_matrix_t *funcmatrix, hislip_s32 rcvval);

hlk_func_par_t g_init_funcs[2] = 
{
    {HLK_CMD_DEVCTL_QWM, hlk_req_qwork_mode, hlk_wait_ack_common},
    {HLK_CMD_DEVCTL_QNS, hlk_req_qnet_status, hlk_wait_ack_common}
};

hlk_func_par_t g_reg_funcs[7] = 
{
    {HLK_CMD_DEVREG_SETSTA, hlk_req_reg_start, hlk_wait_ack_common},      /* reg start */
    {HLK_CMD_DEVREG_DEVINFO, hlk_req_reg_ac_rsa, hlk_wait_ack_common},    /* reg device ac&rsa */
    {HLK_CMD_DEVREG_DEVINFO, hlk_req_reg_devinfo, hlk_wait_ack_common},   /* reg device info */
    {HLK_CMD_DEVREG_SVCINFO, hlk_req_reg_svcinfo, hlk_wait_ack_common},   /* reg service info */
    {HLK_CMD_DEVREG_PROT, hlk_req_reg_keymapinfo, hlk_wait_ack_common},   /* reg keys info */
    {HLK_CMD_DEVREG_SETSTA, hlk_req_reg_end, hlk_wait_ack_common},        /* reg end */
    {HLK_CMD_DEVCTL_QNS, hlk_req_qnet_status, hlk_wait_ack_common}        /* query net status */
};

hlk_func_par_t g_swm_funcs[1] = 
{
    {HLK_CMD_DEVCTL_WM, hlk_req_set_workmode, hlk_wait_ack_common},    /* set workmode */
};
hlk_func_par_t g_rptv_funcs[1] = 
{
    {HLK_CMD_DEVUPD_STA, hlk_req_report_init_values, hlk_wait_ack_common},    /* set workmode */
};

hlk_func_par_t g_idle_funcs[2] = 
{
    {HLK_CMD_DEVUPD_STA, hlk_report_profile_value, hlk_wait_ack_common},
    {HLK_CMD_DEVCTL_QNS, hlk_req_qnet_status, hlk_wait_ack_common}
};

hlk_func_matrix_t g_func_matrix[HLK_RUN_STATE_MAX] = 
{
    {g_init_funcs, 0, 2},
    {g_reg_funcs, 0, 7},
    {g_swm_funcs, 0, 1},
    {g_rptv_funcs, 0, 1},
    {g_idle_funcs, 0, 2}
};

static void hilk_func_matrix_reinit()
{
    hislip_s32 i = 0;
    for (i = 0; i < HLK_RUN_STATE_MAX; i++)
    {
        g_func_matrix[i].curused = 0;
    }
    return ;
}

static hislip_s32 hlk_chg_run_state(hislip_s32 state)
{
    hislip_s32 ret = -1;
	if (state >= HLK_RUN_STATE_INIT 
		&& state < HLK_RUN_STATE_MAX)
    {
        g_hlk_run_state = state;
        ret = 0;
    }

	return ret;
}
static hislip_s32 hlk_get_run_state(void)
{
    return g_hlk_run_state;
}


void hlk_store_cur_wack_seq(hislip_u8 seq)
{
    g_wack_seq[0] = seq;
    
}

hislip_u8 hlk_get_cur_wack_seq(void)
{
    return g_wack_seq[0];
}

static hislip_u8 * hlk_get_data_buf(void)
{
    return g_snd_tmp_buf;
}
static hislip_u8 * hlk_get_ext_tmp_buf(void)
{
    return g_ext_tmp_buf;
}

static hislip_s32 hlk_get_cur_init_vals_index(void)
{
    return g_init_val_idx;
}

static void hlk_set_cur_init_vals_index(hislip_u32 index)
{
    g_init_val_idx = index;
}

static hlk_tlv_t * hlk_get_tmp_tlv_array(void)
{
    return g_tlvs;
}
static hislip_s32 hlk_get_rptvals_state(void)
{
    /* should get report svc's count and record report index */
    return g_rpt_initval_state;
}

static void hlk_set_rptvals_state(hislip_s32 state)
{
    g_rpt_initval_state = state;
}

static hislip_s32 hlk_get_func_state(void)
{
    return g_hlk_func_state;
}
static void hlk_set_func_state(hislip_u32 state)
{
    if (state <= FUNC_PROC_STATE_MAX)
    {
        g_hlk_func_state = (int)state;
    }
    return ;
}

static hislip_s32 hlk_add_maxtrix_funcindex(hlk_func_matrix_t *funmatrix)
{
    hislip_s32 ret = HLK_RET_OK;
    if (funmatrix)
    {
        funmatrix->curused ++;
        if (funmatrix->curused == funmatrix->funcscnt)
        {
            funmatrix->curused = 0;
            ret = HLK_RET_NEXTSTAGE;
        }
    }
    return ret;
}

static void hlk_jump_reg_func_index(void)
{
    g_func_matrix[HLK_RUN_STATE_REGPROFILE].curused = g_func_matrix[HLK_RUN_STATE_REGPROFILE].funcscnt - 1;
}

static void hlk_set_idle_func_index(hislip_s32 idx)
{
    g_func_matrix[HLK_RUN_STATE_IDLE].curused = idx;
}

static hislip_u32 hlk_get_maxtrix_funcindex(hlk_func_matrix_t *funmatrix)
{
    if (funmatrix)
    {
        return (unsigned int)funmatrix->curused;
    }
    return 0xffffU;
}

hislip_u8* hlk_general_func_parse_tlv(hislip_u8 *inbuf, hlk_tlv_t *tlv)
{
    hislip_u8 *pseg = NULL;
    hislip_u16 rlen = 0;
    hislip_s16 itag = 0;
    hislip_s16 ilen = 0;
    hislip_u8 *ival = NULL;
    
    if (NULL == inbuf)
    {
        return NULL;
    }
    pseg = inbuf;
    itag = hislip_parse_data_eabit(pseg, &rlen);
    if (-1 == itag)
    {
        hlk_notify_error_info(HLK_ECODE_RCV_TAG_ERR);
        return NULL;
    }
    pseg = pseg + rlen;
    rlen = 0;
    ilen = hislip_parse_data_eabit(pseg, &rlen);
    if (-1 == ilen)
    {
        hlk_notify_error_info(HLK_ECODE_RCV_LEN_ERR);
        return NULL;
    }
    pseg = pseg + rlen;
    if (ilen > 0)
    {
        ival = pseg;
        pseg = pseg + ilen;
    }
    if (NULL != tlv)
    {
        tlv->tag = (hislip_u16)itag;
        tlv->len = (hislip_u16)ilen;
        tlv->val = ival;
    }
    
    /* return next tlv start position */
    return pseg;
}

hislip_s32 hlk_general_func_prepare_data(hislip_u16 cmd, hlk_tlv_t *tlvs, 
                                                hislip_u32 tlvcnt, hislip_s8 *outdata,
                                                hislip_u16 *outlen)
{
    hislip_s32 i = 0;
	hislip_s32 ret = HLK_RET_OK;
	hislip_u8* ptmp = NULL;
	hislip_u16 slen = 0;
	hislip_u8* msg = NULL;
	hislip_u16 rlen = 0;
	hislip_u8 filltmp[HISLIP_CTL_MAX_SIZE] = {0};

    if (NULL == outdata
        || (NULL == tlvs && 0 != tlvcnt)
        || (NULL != tlvs && 0 == tlvcnt))
    {
        return HLK_RET_ERR;
    }
    msg = (hislip_u8*)outdata;
	slen = 0;

    /* fill cmd data */
	hislip_fill_data_eabit(cmd, &rlen, filltmp);
	slen += rlen;
	ptmp = hislip_fill_buff_info(msg, filltmp, rlen);

    /* fill tlv data */
    for (i = 0; i < tlvcnt; i++)
    {
        /* fill tag data */
        if (tlvs[i].tag >= HLK_INVALID_TAG)
        {
            hlk_notify_error_info(HLK_ECODE_PROFILE_TAG_ERR);
            ret = HLK_RET_ERR;
            break;
        }
        hislip_fill_data_eabit(tlvs[i].tag, &rlen, filltmp);
        slen += rlen;
        ptmp = hislip_fill_buff_info(ptmp, filltmp, rlen);
        
        /* fill len data */
        if (tlvs[i].len >= HLK_INVALID_LEN)
        {
            hlk_notify_error_info(HLK_ECODE_PROFILE_LEN_ERR);
            ret = HLK_RET_ERR;
            break;
        }
        hislip_fill_data_eabit(tlvs[i].len, &rlen, filltmp);
        slen += rlen;
        ptmp = hislip_fill_buff_info(ptmp, filltmp, rlen);
        
        /* fill val data */
        if ((tlvs[i].len > 0) && (NULL != tlvs[i].val))
        {
            ptmp = hislip_fill_buff_info(ptmp, tlvs[i].val, tlvs[i].len);
            slen += tlvs[i].len;
        }
    }
    if ((HLK_RET_OK == ret) && (outlen))
    {
        *outlen = slen;
    }
    return ret;
}

hislip_s32 hlk_general_func_send_cmd(hislip_u16 cmd, hlk_tlv_t *tlvs, 
                                            hislip_u16 tlvcnt, hislip_u8 ackseq)
{
    hislip_s32 ret = 0;
    hislip_u8 *tmpbuf = NULL;
    hislip_u16 datalen = 0;
    
    tmpbuf = hlk_get_data_buf();
    if (NULL == tmpbuf)
    {
        return HLK_RET_ERR;
    }
    
    ret = hlk_general_func_prepare_data(cmd, tlvs, tlvcnt, (char *)tmpbuf, &datalen);
    if (HLK_RET_OK == ret)
    {
        if (!ackseq)
        {
            ret = hislip_send_data(tmpbuf, datalen, HISLIP_CMD_TYPE);
        }
        else
        {
            ret = hislip_send_data(tmpbuf, datalen, ackseq);
        }
        if (ret == HISLIP_OK)
        {
            ret = HLK_RET_OK;
        }
        else
        {
            ret = HLK_RET_SFAIL;
        }
    }
	return ret;
}


HILINK_WORK_MODE hlk_get_loacal_work_mode(void)
{
    return mod_workmode;
}
HILINK_NET_STATUS hlk_get_loacal_net_status(void)
{
    return mod_netsta;
}

static hislip_s32 hlk_req_qwork_mode(void)
{
    hislip_s32 ret = HLK_RET_OK;
    /* query work mode, no payload */
    ret = hlk_general_func_send_cmd(HLK_CMD_DEVCTL_QWM, NULL, 0, 0);
    
    return ret;
}

static hislip_s32 hlk_req_qnet_status(void)
{
    hislip_s32 ret = HLK_RET_OK;

    /* query network status, no payload */
    ret = hlk_general_func_send_cmd(HLK_CMD_DEVCTL_QNS, NULL, 0, 0);

    return ret;
}

static hislip_s32 hlk_wait_ack_common(hislip_u16 cmd, hislip_u64 starttick)
{
    hislip_u8 seq = 0xff;
    hislip_u64 cur_tick = 0;
    last_ack_info_t lastack;
    
    seq = hlk_get_cur_wack_seq();
    cur_tick = hislip_get_cur_time();
    
    hislip_get_last_acked_info(&lastack);

    if ((cur_tick - starttick) >= ACK_TIMEOUT_VALUE)
    {
        return ACK_STATE_ACKTIMOUT;
    }
    
    if (lastack.cmd == cmd && seq == lastack.seq)
    {
        return ACK_STATE_ACKED;
    }

    return ACK_STATE_WAITTING;
}

static hislip_s32 hlk_req_reg_start(void)
{
    hislip_s32 ret = HLK_RET_OK;
    hlk_tlv_t tlvs[2];
    hislip_u8 *profilever = NULL;
    hislip_u8 regstatus = 0x0;

    regstatus = (unsigned char )HLK_REG_START;
    profilever = (unsigned char *)dev_get_dev_profilever();

    tlvs[0].tag = (unsigned short)HLK_TAG_REGSTA;
    tlvs[0].len = 1;
    tlvs[0].val = &regstatus;
    tlvs[1].tag = HLK_TAG_PROFILE_VER;
    tlvs[1].len = strlen((const char *)profilever);
    tlvs[1].val = profilever;

    /* send reg start cmd, payload is reg status and profile version */
    ret = hlk_general_func_send_cmd(HLK_CMD_DEVREG_SETSTA, tlvs, 2, 0);

    return ret;
}

static hislip_s32 hlk_req_reg_ac_rsa(void)
{
    hislip_s32 ret = HLK_RET_OK;
    hlk_tlv_t tlvs[2];
    hislip_u8 *birsa = NULL;
    hislip_u8 *ac = NULL;
    
    birsa = (unsigned char *)dev_get_dev_birsa();
    ac = (unsigned char *)dev_get_dev_ac();

    tlvs[0].tag = (unsigned short)HLK_TAG_PROT_BIRSA;
    tlvs[0].len = strlen((const char *)birsa);
    tlvs[0].val = birsa;
    tlvs[1].tag = HLK_TAG_PROT_AC;
    tlvs[1].len = HLK_AC_NUM_LEN;
    tlvs[1].val = ac;

    /* send reg devinfo's ac and birsa */
    ret = hlk_general_func_send_cmd(HLK_CMD_DEVREG_DEVINFO, tlvs, 2, 0);

    return ret;
}

static hislip_s32 hlk_req_reg_devinfo(void)
{
    hislip_s32 ret = HLK_RET_OK;
    hlk_tlv_t tlvs[7];
    dev_info_t *devinfo = NULL;
    dev_nmen_t *devennam = NULL;
    
    devinfo = (dev_info_t *)dev_get_dev_info();
    devennam = (dev_nmen_t *)dev_get_dev_enname();

    tlvs[0].tag = (unsigned short)HLK_TAG_PROT_DEVSN;
    tlvs[0].len = strlen(devinfo->sn);
    tlvs[0].val = (unsigned char *)devinfo->sn;
    
    tlvs[1].tag = HLK_TAG_PROT_PRDTID;
    tlvs[1].len = strlen(devinfo->prodId);
    tlvs[1].val = (unsigned char *)devinfo->prodId;

    tlvs[2].tag = HLK_TAG_PROT_PRDTMD;
    tlvs[2].len = strlen(devinfo->model);
    tlvs[2].val = (unsigned char *)devinfo->model;
    
    tlvs[3].tag = (unsigned short)HLK_TAG_PROT_PRDTTP;
    tlvs[3].len = strlen(devinfo->dev_t);
    tlvs[3].val = (unsigned char *)devinfo->dev_t;
    
    tlvs[4].tag = HLK_TAG_PROT_MANU;
    tlvs[4].len = strlen(devinfo->manu);
    tlvs[4].val = (unsigned char *)devinfo->manu;

    tlvs[5].tag = HLK_TAG_PROT_DTENN;
    tlvs[5].len = strlen(devennam->devt_nm);
    tlvs[5].val = (unsigned char *)devennam->devt_nm;

    tlvs[6].tag = HLK_TAG_PROT_MAUNENN;
    tlvs[6].len = strlen(devennam->manu_nm);
    tlvs[6].val = (unsigned char *)devennam->manu_nm;

    /* send reg devinfo's ac and birsa */
    ret = hlk_general_func_send_cmd(HLK_CMD_DEVREG_DEVINFO, tlvs, 7, 0);

    return ret;
}

/* this function is only called by hlk_req_reg_svcinfo() */
static hislip_s32 hlk_local_fill_svc_tlv(hlk_tlv_t *tlvs, hislip_u32 *cnt)
{
    hislip_s32 i = 0;
    hislip_s32 itemcnt = 0;
    svc_info_t *svcs = NULL;
    hislip_u8 *valbuf = NULL;
    hislip_s32 offset = 0;
    hislip_u16 taglen = 0;
    
    svcs = ( svc_info_t *)dev_get_profinfo_and_itssize(DEV_PROF_T_SVCS, &itemcnt);
    valbuf = hlk_get_ext_tmp_buf();
    memset(valbuf, 0, HLK_EXT_BUF_SIZE);
    
    if ((itemcnt > HLK_MAX_SVCINST_CNT)
        || ((itemcnt * 3) > HLK_MAX_TLVS_CNT))
    {
        /* todo: here need call to a global error function to tell user 
           that config size is smaller than real svc data's size */
        hlk_notify_error_info(HLK_ECODE_TLVARRAY_SIZE_SVCSMALL);
        return HLK_RET_ERR;
    }
    for (i = 0; i < itemcnt; i++)
    {
        if (svcs[i].svc_inst >= HLK_INVALID_TAG
            || NULL == svcs[i].st
            || NULL == svcs[i].svc_id)
        {
            hlk_notify_error_info(HLK_ECODE_PROFILE_SVCINST_ERR);
            return HLK_RET_ERR;
        }

        /* process svc map tag data */
        tlvs[i*3].tag = HLK_TAG_PROT_SVCINF;
        hislip_fill_data_eabit(svcs[i].svc_inst, &taglen, valbuf + offset);
        tlvs[i*3].len = taglen;
        tlvs[i*3].val = valbuf + offset;
        offset += taglen;

        /* process svc service type data */
        tlvs[i*3 + 1].tag = HLK_TAG_PROT_SVCINF;
        tlvs[i*3 + 1].len = strlen(svcs[i].st);
        tlvs[i*3 + 1].val = (unsigned char *)svcs[i].st;
        
        /* process svc service instance name data */
        tlvs[i*3 + 2].tag = HLK_TAG_PROT_SVCINF;
        tlvs[i*3 + 2].len = strlen(svcs[i].svc_id);
        tlvs[i*3 + 2].val = (unsigned char *)svcs[i].svc_id;
    }
    if (cnt)
    {
        *cnt = itemcnt * 3;
    }
    return HLK_RET_OK;
}

static hislip_s32 hlk_req_reg_svcinfo(void)
{
    hislip_s32 ret = HLK_RET_OK;    
    hislip_u32 itemcnt = 0;
    hlk_tlv_t *svctlvs = NULL;
    
    svctlvs = hlk_get_tmp_tlv_array();
    ret = hlk_local_fill_svc_tlv(svctlvs, &itemcnt);
    /* send reg service info */
    if (HLK_RET_OK == ret)
    {
        ret = hlk_general_func_send_cmd(HLK_CMD_DEVREG_SVCINFO, svctlvs, itemcnt, 0);
    }

    return ret;
}

/* this function is only called by hlk_req_reg_keymapinfo() */
static hislip_s32 hlk_local_fill_keymap_tlv(hlk_tlv_t *tlvs, hislip_u32 *cnt)
{
    hislip_s32 i = 0;
    hislip_s32 itemcnt = 0;
    dev_key_map_t *keys = NULL;
    hislip_u8 *valbuf = NULL;
    hislip_s32 offset = 0;
    hislip_u16 taglen = 0;
    
    keys = ( dev_key_map_t *)dev_get_profinfo_and_itssize(DEV_PROF_T_KEYS, &itemcnt);
    valbuf = hlk_get_ext_tmp_buf();
    memset(valbuf, 0, HLK_EXT_BUF_SIZE);
    
    if ((itemcnt > HLK_MAX_KEYMAP_CNT)
        || ((itemcnt * 4) > HLK_MAX_TLVS_CNT))
    {
        /* todo: here need call to a global error function to tell user 
           that config size is smaller than real svc data's size */
        hlk_notify_error_info(HLK_ECODE_TLVARRAY_SIZE_KEYSMALL);
        return HLK_RET_ERR;
    }
    
    for (i = 0; i < itemcnt; i++)
    {
        if (keys[i].map_id >= HLK_INVALID_TAG
            || NULL == keys[i].svc_name
            || NULL == keys[i].charact_name)
        {
            hlk_notify_error_info(HLK_ECODE_PROFILE_KEYMAP_ERR);
            return HLK_RET_ERR;
        }

        /* process key map tag data */
        tlvs[i*4].tag = HLK_TAG_PROT_ITEM;
        hislip_fill_data_eabit(keys[i].map_id, &taglen, valbuf + offset);
        tlvs[i*4].len = taglen;
        tlvs[i*4].val = valbuf + offset;
        offset += taglen;

        /* process svc instance name data */
        tlvs[i*4 + 1].tag = HLK_TAG_PROT_ITEM;
        tlvs[i*4 + 1].len = strlen(keys[i].svc_name);
        tlvs[i*4 + 1].val = (unsigned char *)keys[i].svc_name;
        
        /* process key name data */
        tlvs[i*4 + 2].tag = HLK_TAG_PROT_ITEM;
        tlvs[i*4 + 2].len = strlen(keys[i].charact_name);
        tlvs[i*4 + 2].val = (unsigned char *)keys[i].charact_name;

        /* process key data type */
        tlvs[i*4 + 3].tag = HLK_TAG_PROTKIND;
        tlvs[i*4 + 3].len = 1;
        tlvs[i*4 + 3].val = (unsigned char *)&keys[i].pro_type;

    }
    if (cnt)
    {
        *cnt = itemcnt * 4;
    }
    return HLK_RET_OK;

}

static hislip_s32 hlk_req_reg_keymapinfo(void)
{
    hislip_s32 ret = HLK_RET_OK;    
    hislip_u32 itemcnt = 0;
    hlk_tlv_t *keytlvs = NULL;
    
    keytlvs = hlk_get_tmp_tlv_array();
    ret = hlk_local_fill_keymap_tlv(keytlvs, &itemcnt);
    
    /* send reg keymap info */
    if (HLK_RET_OK == ret)
    {
        ret = hlk_general_func_send_cmd(HLK_CMD_DEVREG_PROT, keytlvs, itemcnt, 0);
    }

    return ret;

}

static hislip_s32 hlk_req_reg_end(void)
{
    hislip_s32 ret = HLK_RET_OK;
    hlk_tlv_t tlvs[1];
    hislip_u8 regstatus = 0x0;

    regstatus = (hislip_u8)HLK_REG_END;

    tlvs[0].tag = (hislip_u16)HLK_TAG_REGSTA;
    tlvs[0].len = 1;
    tlvs[0].val = &regstatus;
    
    /* send reg end cmd, payload is reg status */
    ret = hlk_general_func_send_cmd(HLK_CMD_DEVREG_SETSTA, tlvs, 1, 0);
    
    return ret;
}

static hislip_s32 hlk_req_set_workmode(void)
{
    hislip_s32 ret = HLK_RET_OK;
    hlk_tlv_t tlvs[1];
    HILINK_NET_STATUS netstatus = NET_UNKONWN;
    hislip_u8 wkmode = 0x0;
    
    netstatus = hlk_get_loacal_net_status();
    switch (netstatus)
    {
        case NET_NOAP:
            wkmode = (unsigned char)WKMD_AUTOAP;
            break;
        case NET_NOTCONN:
            wkmode = (unsigned char)WKMD_ONLINE;
            break;
        default:
            wkmode = (unsigned char)hlk_get_loacal_work_mode();
            //return HLK_RET_ERR;
            break;
    }

    tlvs[0].tag = (unsigned short)HLK_TAG_WK_MD;
    tlvs[0].len = 1;
    tlvs[0].val = &wkmode;
    
    /* send set work mode cmd, payload is work mode */
    ret = hlk_general_func_send_cmd(HLK_CMD_DEVCTL_WM, tlvs, 1, 0);
    
    return ret;
}

static hislip_s32 hlk_fill_int_to_arrays(hislip_u8 *buf, hislip_s32 val)
{
    buf[0] = (unsigned char)((val >> 24) & 0xFF);
    buf[1] = (unsigned char)((val >> 16) & 0xFF);
    buf[2] = (unsigned char)((val >> 8) & 0xFF);
    buf[3] = (unsigned char)(val & 0xFF);
    return 0;
}

static hislip_s32 hlk_fill_svc_tlvs(hislip_u16 svcmapid, hislip_u16 keyid, 
                                hlk_tlv_t *tlvs, hislip_u32 *keycnt, 
                                hislip_u32 *datalen)
{
    hilink_msg_t *keyvals = NULL;
    hislip_s32 cnt = 0;
    hislip_s32 i = 0;
    hislip_s32 tlvcnt = 0;
    hislip_u8 datatype;
    hislip_u8 *valbuf = NULL;
    hislip_s32 offset = 0;
    hislip_u32 tmpdlen = 0;
    hislip_u16 rlen = 0;
    
    valbuf = hlk_get_ext_tmp_buf();
    keyvals = (hilink_msg_t *)dev_get_profinfo_and_itssize(DEV_PROF_T_VALS, &cnt);
    for (i = 0; i < cnt; i++)
    {
        if ((keyvals[i].msg_info.svc_inst == svcmapid)
            && ((HLK_INVALID_TAG == keyid) || (keyid == keyvals[i].msg_info.map_id)))
        {
            tlvs[tlvcnt].tag = keyvals[i].msg_info.map_id;
            /* get tag data store len */
            hislip_fill_data_eabit(tlvs[tlvcnt].tag, &rlen, NULL);
            tmpdlen += rlen;
            
            datatype = hlk_get_key_datatype(keyvals[i].msg_info.map_id);
            switch(datatype)
            {
                case HLK_DATA_TYPE_INT:
                case HLK_DATA_TYPE_BOOL:
                    if (offset >= HLK_EXT_BUF_SIZE)
                    {
                        hlk_notify_error_info(HLK_ECODE_EXT_BUF_SIZE_SMALL);
                        return HLK_RET_ERR;
                    }
                    tlvs[tlvcnt].len = sizeof(int);
                    (void)hlk_fill_int_to_arrays(&valbuf[offset], keyvals[i].msg_info.value);
                    tlvs[tlvcnt].val = valbuf + offset;
                    offset += 4;
                    break;
                case HLK_DATA_TYPE_STR:
                    tlvs[tlvcnt].len = strlen((char *)((unsigned int)keyvals[i].msg_info.value));
                    tlvs[tlvcnt].val = (unsigned char *)((unsigned int)keyvals[i].msg_info.value);
                    break;
                default:
                    hlk_notify_error_info(HLK_ECODE_KEYMAP_DATA_TYPE_ERR);
                    return HLK_RET_ERR;
            }
            /* get tlv's length data store len */
            hislip_fill_data_eabit(tlvs[tlvcnt].len, &rlen, NULL);
            tmpdlen += rlen;
            /* get tlv's length data store len */
            tmpdlen += tlvs[tlvcnt].len;
            tlvcnt++;
            if (HLK_INVALID_TAG != keyid)
            {
                break;
            }
        }
    }
    if (keycnt)
    {
        *keycnt = tlvcnt;
    }
    if (datalen)
    {
        *datalen = tmpdlen;
    }
    return HLK_RET_OK;
}

static hislip_s32 hlk_req_report_svc_vals(hislip_u16 svcmapid, hislip_u16 keyid)
{
    hislip_s32 ret = 0;
    hlk_tlv_t *keytlvs = NULL;
    hislip_u32 keycnt = 0;
    hislip_u32 datalen = 0;
    
    keytlvs = hlk_get_tmp_tlv_array();
    ret = hlk_fill_svc_tlvs(svcmapid, keyid, &keytlvs[1], &keycnt, &datalen);
    if (HLK_RET_OK == ret)
    {
        keytlvs[0].tag = svcmapid;
        keytlvs[0].len = datalen;
        keytlvs[0].val = NULL;
        ret = hlk_general_func_send_cmd(HLK_CMD_DEVUPD_STA, keytlvs, keycnt + 1, 0);
    }
    return ret;
}

static hislip_s32 hlk_req_report_init_values(void)
{
    hislip_s32 ret = 0;
    hislip_s32 itemcnt = 0;
    svc_info_t *svcs = NULL;
    hislip_u32 idx = 0;
    
    idx = hlk_get_cur_init_vals_index();
    svcs = ( svc_info_t *)dev_get_profinfo_and_itssize(DEV_PROF_T_SVCS, &itemcnt);
    if (idx == itemcnt)
    {
        hlk_set_rptvals_state(HLK_RPT_DONE);
        return HLK_RET_NEXTSTAGE;
    }

    /* report svc's all key values */
    ret = hlk_req_report_svc_vals(svcs[idx].svc_inst, HLK_INVALID_TAG);
    hlk_set_cur_init_vals_index((idx + 1));

    return ret;

}

hislip_s32 hlk_req_report_changed_keyvals(hislip_s32 idx)
{
    hislip_s32 ret = HLK_RET_OK;
    hislip_s32 cnt = 0;
    hilink_msg_t *keyvals = NULL;
    keyvals = (hilink_msg_t *)dev_get_profinfo_and_itssize(DEV_PROF_T_VALS, &cnt);
    if (idx < 0 || idx >= cnt || NULL == keyvals)
    {
        return -1;
    }
    if (dev_chk_keyv_chg_by_index(idx, DEV_KEY_MASK_USET))
    {
        /* report svc's keyvals[idx].msg_info.map_id key values */
        ret = hlk_req_report_svc_vals(keyvals[idx].msg_info.svc_inst, keyvals[idx].msg_info.map_id);
    }

    return ret;
}

static hislip_s32 hlk_process_stage_func(hlk_func_par_t *stagefunc, hislip_u32 idx)
{
    hislip_s32 ret = HLK_RET_OK;
    hislip_u32 funcstate;
    static hislip_u64 curtick = 0;
    
    funcstate = hlk_get_func_state();
    if (FUNC_PROC_STATE_SNDCMD == funcstate)
    {
        ret = stagefunc[idx].send_request();
        if (HLK_RET_NEXTSTAGE == ret)
        {
            ret = HLK_RET_OK;
        }
        else if (HLK_RET_OK == ret)
        {
            hlk_set_func_state(FUNC_PROC_STATE_WACK);
            ret = HLK_RET_WACK;
            curtick = hislip_get_cur_time();
        }
        else
        {
            ret = HLK_RET_SFAIL;
        }
    }
    else if (FUNC_PROC_STATE_WACK == funcstate)
    {
        ret = stagefunc[idx].process_ack_state_chg(stagefunc[idx].cmd, curtick);
        switch(ret)
        {
            case ACK_STATE_WAITTING:
                ret = HLK_RET_WACK;
                break;
            case ACK_STATE_ACKED:
                ret = HLK_RET_OK;
                hlk_set_func_state(FUNC_PROC_STATE_SNDCMD);
                break;
            case ACK_STATE_ACKTIMOUT:
                ret = HLK_RET_ACK_TMOUT;
                hlk_set_func_state(FUNC_PROC_STATE_SNDCMD);
                break;
            default:
                ret = HLK_RET_ERR;
                hlk_set_func_state(FUNC_PROC_STATE_SNDCMD);
                break;
        }
    }
    return ret;
}

static hislip_s32 hlk_process_run_state_chg(HILINK_WORK_MODE wkmod, HILINK_NET_STATUS netsta)
{
    hislip_s32 ret = HLK_RET_OK;
    hislip_s32 runstate = 0;
    hislip_s32 rptstate = HLK_RPT_PROCESSING;
    
    runstate = hlk_get_run_state();
    switch(runstate)
    {
        case HLK_RUN_STATE_INIT:
            if ((WKMD_OFFLINE == wkmod) && (NET_UNKONWN == netsta))
            {
                /* change to reg profile state */
                hlk_chg_run_state(HLK_RUN_STATE_REGPROFILE);
            }
            else if((WKMD_OFFLINE == wkmod) && ((NET_NOAP == netsta) || (NET_NOTCONN == netsta)))
            {
                //hlk_chg_run_state(HLK_RUN_STATE_SETWKMODE);
                hlk_chg_run_state(HLK_RUN_STATE_RPTINIVAL);
            }
            else if ((WKMD_ONLINE == wkmod) || (WKMD_AUTOAP == wkmod) )
            {
                /* wifi module is online , but deivce reboot , so should report init data */
                hlk_chg_run_state(HLK_RUN_STATE_RPTINIVAL);
            }
            break;
        case HLK_RUN_STATE_REGPROFILE:
            if((WKMD_OFFLINE == wkmod) && ((NET_NOAP == netsta) || (NET_NOTCONN == netsta)))
            {
                //hlk_chg_run_state(HLK_RUN_STATE_SETWKMODE);
                hlk_chg_run_state(HLK_RUN_STATE_RPTINIVAL);
            }
            break;
        case HLK_RUN_STATE_SETWKMODE:
            //hlk_chg_run_state(HLK_RUN_STATE_RPTINIVAL);
            hlk_chg_run_state(HLK_RUN_STATE_IDLE);
            break;
        case HLK_RUN_STATE_RPTINIVAL:
            /* if report init vals finished , change to idle tate */
            rptstate = hlk_get_rptvals_state();
            if (HLK_RPT_DONE == rptstate)
            {
                /*set report init value state to processing state */
                hlk_set_rptvals_state(HLK_RPT_PROCESSING);
                //hlk_chg_run_state(HLK_RUN_STATE_IDLE);
                hlk_chg_run_state(HLK_RUN_STATE_SETWKMODE);
            }
            break;
        case HLK_RUN_STATE_IDLE:
            hlk_chg_run_state(HLK_RUN_STATE_INIT);
            break;
        default:
            ret = HLK_RET_ERR;
            break;
    }

    return ret;
}

static hislip_s32 hlk_process_maxtrix_funcs(hlk_func_matrix_t *funcmatrix)
{
    hislip_s32 ret = 0;
    hislip_u32 funcidx = 0;
    HILINK_WORK_MODE wkmod = WKMD_MAX;
    HILINK_NET_STATUS netsta = NET_UNKONWN;
    
    if (NULL == funcmatrix)
    {
        return HLK_RET_ERR;
    }
    
    funcidx = hlk_get_maxtrix_funcindex(funcmatrix);
    if (funcidx > funcmatrix->funcscnt)
    {
        return HLK_RET_ERR;
    }
    ret = hlk_process_stage_func(funcmatrix->params, funcidx);
    if (HLK_RET_OK == ret)
    {
        /* todo: report init vals should do what add */
        ret = hlk_add_maxtrix_funcindex(funcmatrix);
        if (HLK_RET_NEXTSTAGE == ret)
        {
            /* get local workmode and net status */
            wkmod = hlk_get_loacal_work_mode();
            netsta = hlk_get_loacal_net_status();
            
            /* change to next state or idle according to workmode and net status */
            hlk_process_run_state_chg(wkmod, netsta);
        }
    }
    return ret;
}
static void hlk_get_long_time_nodata_flag(hislip_s32 rcvflg, hislip_s32 *nodataflag)
{
    hislip_u32 curtick = 0;
    static hislip_u32 lastrcvtick = 0;
    curtick = hislip_get_cur_time();
    
    /* if long time no message come , hilink should send cmd to wifi module */
    if (HISLIP_RCV_NO_DATA == rcvflg)
    {
        if (0 == lastrcvtick)
        {
            /* update last recive data time value */
            lastrcvtick = curtick;
        }
        if (curtick - lastrcvtick >= HISLIP_NO_MSG_TIMEOUT)
        {
            /* call get netstatus */
            if (nodataflag)
            {
                *nodataflag = 1;
            }
            lastrcvtick = 0;
        }
    }
    else
    {
        lastrcvtick = 0;
    }
}
static hislip_s32 hlk_process_idle_state(hlk_func_matrix_t *funcmatrix, hislip_s32 rcvval)
{
    hislip_s32 ret = HLK_RET_OK;
    hislip_s32 nodataflag = 0;

    hlk_get_long_time_nodata_flag(rcvval, &nodataflag);

    if (nodataflag)
    {
        /* query net status if long time no data come */
        hlk_set_idle_func_index(1);
        ret = hlk_process_maxtrix_funcs(&g_func_matrix[HLK_RUN_STATE_IDLE]);
        if (ret != HLK_RET_WACK)
        {
            nodataflag = 0;
        }
        return 0;
    }
    else
    {
        /* upload key values */
        hlk_set_idle_func_index(0);
        ret = hlk_process_maxtrix_funcs(&g_func_matrix[HLK_RUN_STATE_IDLE]);
    }

    return 0;
}


#define HLK_COMM_FUNC

static hislip_u8 hlk_get_key_datatype(hislip_u16 mapid)
{
	hislip_s32 i = 0;
    hislip_s32 mapcnt = 0;
    dev_key_map_t *keys = NULL;
    
    keys = (dev_key_map_t *)dev_get_profinfo_and_itssize(DEV_PROF_T_KEYS, &mapcnt);
    if (NULL == keys)
    {
        return HLK_DATA_TYPE_UNKNOWN;
    }
	for (i  = 0; i < mapcnt; i++)
	{
        if (keys[i].map_id == mapid)
        {
            return keys[i].pro_type;
		}
	}
	return HLK_DATA_TYPE_UNKNOWN;
}

hislip_s32 hislip_send_ack_msg(hislip_u8 seq, 
    hislip_u16 cmd, 
    hislip_u8 result)
{
    hislip_s32 ret = HLK_RET_OK;
    hlk_tlv_t keytlvs[1];

    keytlvs[0].tag = HLK_TAG_OP_RLT;
    keytlvs[0].len = HISLIP_PARAM_VAL_LEN;
    keytlvs[0].val = &result;
    ret = hlk_general_func_send_cmd(cmd, keytlvs, 1, HISLIP_ACK_TYPE | seq);

    return ret;
}

static hislip_s32 hlk_report_changed_keyvals(hislip_s32 idx)
{
    hislip_s32 cnt = 0;
    hilink_msg_t *keyvals = NULL;
    keyvals = (hilink_msg_t *)dev_get_profinfo_and_itssize(DEV_PROF_T_VALS, &cnt);
    if (idx < 0 || idx >= cnt || NULL == keyvals)
    {
        return -1;
    }

    hlk_req_report_svc_vals(keyvals[idx].msg_info.svc_inst, keyvals[idx].msg_info.map_id);
    return 0;
}


static hislip_s32 hlk_report_profile_value(void)
{
    hislip_s32 keyvalcnt = 0;
    hislip_s32 i = 0;
    hislip_s32 itemcnt = 0;
    svc_info_t *svcs = NULL;
    hislip_u32 idx = 0;

    if (dev_get_keyv_chg_flag())
    {
        (void)dev_get_profinfo_and_itssize(DEV_PROF_T_VALS, &keyvalcnt);
        for (i = 0; i < keyvalcnt; i++)
        {
            if (dev_chk_keyv_chg_by_index(i, DEV_KEY_MASK_USET))
            {
                (void)hlk_report_changed_keyvals(i);
                return HISLIP_SUCC;
            }
        }
    }
    if (dev_get_svc_chg_flag())
    {
        svcs = ( svc_info_t *)dev_get_profinfo_and_itssize(DEV_PROF_T_SVCS, &itemcnt);
        for (idx = 0; idx < itemcnt; idx++)
        {
            /* report svc's all key values */
            if (dev_chk_svc_chg_by_index(idx, DEV_SVC_MASK_USET))
            {
                (void)hlk_req_report_svc_vals(svcs[idx].svc_inst, HLK_INVALID_TAG);
            }
        }
    }
    return HISLIP_ERR;
}

#define HLK_SEND_CMD

hislip_s32 hlk_dev_reboot(void)
{
	hislip_s32 ret = HLK_RET_OK;
    hlk_tlv_t keytlvs[1];
    hislip_u8 result = HLK_RB_MDERR;
    
    keytlvs[0].tag = HLK_TAG_RB_RSN;
    keytlvs[0].len = HISLIP_PARAM_VAL_LEN;
    keytlvs[0].val = &result;

	ret = hlk_general_func_send_cmd(HLK_CMD_DEVCTL_RB, keytlvs, 1, HISLIP_CMD_TYPE);
    return ret;
}

hislip_s32 hlk_dev_reset(void)
{
    hislip_s32 ret = HLK_RET_OK;

	ret = hlk_general_func_send_cmd(HLK_CMD_DEVCTL_RST, NULL, 0, HISLIP_CMD_TYPE);
    return ret;
}

#define HLK_RCV_CMD


hislip_s32  hlk_updlocal_work_mode(hislip_u8* buf, hislip_u16 len)
{
    hislip_u8 work_mode;
    hislip_u16 rlen = 0;
    hislip_s16 data = 0x0;
    hislip_s32 offset = 0;
    
    /* get tag value */
    data = hislip_parse_data_eabit(buf + offset, &rlen);
    offset += rlen;
   
	switch (data)
	{
        case HLK_TAG_WK_MD:
            break;  
        case HLK_TAG_OP_RLT:
            /* tlv(op result) + tlv(work mode)*/
            data = hislip_parse_data_eabit(buf + offset, &rlen);//L
    	    offset += (rlen + data);//jump tlv(op result) 
    	    data = hislip_parse_data_eabit(buf + offset, &rlen);//T
    	    offset += rlen;
    	    if (data != HLK_TAG_WK_MD)
    		{
    		    hlk_notify_error_info(HLK_ECODE_RCV_WKMODE_ERR);
    			return HISLIP_ERR;
    		}
            break;
        default:
            //PRINT_ERR("TAG [0x%0x] err.", data);
            return HISLIP_ERR;
    }
	
    /* get len */
    data = hislip_parse_data_eabit(buf + offset, &rlen);
    offset += rlen;
    if (HISLIP_PARAM_VAL_LEN != data)
    {
        return HISLIP_ERR;
    }
    /*get work mode value , it is one byte */
    memcpy(&work_mode, buf + offset, len - offset);

    switch (work_mode)
    {
        case WKMD_OFFLINE:
        case WKMD_AUTOAP:
        case WKMD_ONLINE:
            mod_workmode = (HILINK_WORK_MODE)work_mode;
            break;
        case WKMD_UNKONWN:
        default:
            //PRINT_ERR("tag [0x%0x] err.", data);
            return HISLIP_ERR;
    }
    /* if wifi module reboot, device should re-process regist processing */
    if (WKMD_OFFLINE == mod_workmode && HLK_RUN_STATE_INIT != hlk_get_run_state())
    {
        hlk_chg_run_state(HLK_RUN_STATE_INIT);
        hlk_set_func_state(FUNC_PROC_STATE_SNDCMD);
        hlk_set_cur_init_vals_index(0);
        hlk_set_rptvals_state(HLK_RPT_PROCESSING);
        hislip_uart_res_init();
        hilk_func_matrix_reinit();
    }
    return HISLIP_SUCC;
}
hislip_s32  hlk_update_work_mode(hislip_u8* buf, hislip_u16 len)
{ 
    hlk_updlocal_work_mode(buf, len);
    
    return HISLIP_SUCC;
}

hislip_s32 hlk_update_net_status(hislip_u8* buf, hislip_u16 len)
{
    HILINK_NET_STATUS net_sta;
    hislip_u16 rlen = 0;
    hislip_s16 data = 0x0;
    hislip_s32 offset = 0;

    /* get tag value */
    data = hislip_parse_data_eabit(buf + offset, &rlen);
    offset += rlen;
    switch (data)
    {
        case HLK_TAG_MD_NST:
            break;  
        case HLK_TAG_OP_RLT:
            /* tlv(op result) + tlv(net status)*/
            data = hislip_parse_data_eabit(buf + offset, &rlen);//L
    	    offset += (rlen + data);//jump tlv(op result)
    	    data = hislip_parse_data_eabit(buf + offset, &rlen);//T
    	    offset += rlen;
    	    if(data != HLK_TAG_MD_NST)
    		{
    		    hlk_notify_error_info(HLK_ECODE_RCV_NETSTA_ERR);
    			return HISLIP_ERR;
    		}
            break; 
        default:
            //PRINT_ERR("Tag [0x%0x] err.", data);
            return HISLIP_ERR;
    }
	
    /*get length */
    data = hislip_parse_data_eabit(buf + offset, &rlen);
    if (HISLIP_PARAM_VAL_LEN != data)
    {
        return HISLIP_ERR;
    }
    offset += rlen;
	
    /*get work mode value , it is one byte */
    memcpy(&net_sta, buf + offset, len - offset);

    //PRINT_INFO("Mod netsta [0x%0x] switch to [0x%x].", mod_netsta, net_sta);
    mod_netsta = net_sta; 
    return HISLIP_SUCC;
}

hislip_s32 hlk_update_one_key_val(hislip_s16 svcid, hislip_s16 keyid, 
                                hislip_s32 val, hislip_s8 *str)
{
    hislip_s32 i = 0;
    hislip_s32 cnt = 0;
    hislip_u8 dtype = 0;
    dev_key_map_t *keys = NULL;
    hilink_msg_t *keyvals = NULL;
    
    keys = (dev_key_map_t *)dev_get_profinfo_and_itssize(DEV_PROF_T_KEYS, &cnt);
    if (NULL == keys)
    {
        return -1;
    }
    for (i = 0; i < cnt; i++)
    {
        if (keys[i].map_id == keyid)
        {
            dtype = keys[i].pro_type;
            break;
        }
    }
    
    keyvals = (hilink_msg_t *)dev_get_profinfo_and_itssize(DEV_PROF_T_VALS, &cnt);
    if (NULL == keyvals)
    {
        return -1;
    }
    for (i = 0; i <cnt ; i++)
    {
        if (keyvals[i].msg_info.svc_inst == svcid
            && keyvals[i].msg_info.map_id == keyid)
        {
            if (HLK_DATA_TYPE_INT == dtype
                || HLK_DATA_TYPE_BOOL == dtype)
            {
                keyvals[i].msg_info.value = val;
            }
            if (HLK_DATA_TYPE_STR == dtype)
            {
                if (NULL == str)
                {
                    return -1;
                }
                if (keyvals[i].msg_info.value == 0)
                {
                    /* no buf to store str data*/
                    hlk_notify_error_info(HLK_ECODE_NO_BUF_TO_STORE_STR);
                    return -1;
                }
                memcpy((hislip_s8 *)((hislip_u32)keyvals[i].msg_info.value), str, strlen(str));
            }
            return i;
        }
    }
    return -1;
}


hislip_s32 hlk_get_factory_infos(hislip_s8 *ssid)
{
	hislip_s32 ret = HLK_RET_OK;
    hlk_tlv_t keytlvs[1];
    
    keytlvs[0].tag = HLK_TAG_FINFO_AP;
    if (NULL == ssid)
    {
        keytlvs[0].len = 0;
        keytlvs[0].val = NULL;
    }
    else
    {
        keytlvs[0].len = strlen(ssid);
        keytlvs[0].val = (unsigned char *)ssid;
    }

	ret = hlk_general_func_send_cmd(HLK_CMD_DEVQRY_FINFOS, keytlvs, 1, HISLIP_CMD_TYPE);
    return ret;
}

/* todo: this function need re-write */
hislip_s32 hlk_process_query_keyvals(hislip_s8 seq, hislip_u8* message, hislip_u16 len)
{
    hislip_s32 ret = 0;
    hislip_u16 svc_inst = 0x0;
    hlk_tlv_t *keytlvs = NULL;
    hislip_u32 keycnt = 0;
    hislip_u32 datalen = 0;
    hlk_tlv_t svctlv;    
    hislip_u8 *nxttlv = NULL;
    
    nxttlv = hlk_general_func_parse_tlv(message, &svctlv);
    if (NULL != nxttlv)
    {
        svc_inst = svctlv.tag;
    }
    
    keytlvs = hlk_get_tmp_tlv_array();
    ret = hlk_fill_svc_tlvs(svc_inst, HLK_INVALID_TAG, &keytlvs[1], &keycnt, &datalen);
    if (HLK_RET_OK == ret)
    {
        keytlvs[0].tag = svc_inst;
        keytlvs[0].len = datalen;
        keytlvs[0].val = NULL;
        ret = hlk_general_func_send_cmd(HLK_CMD_DEVUPD_STA, keytlvs, keycnt + 1, (seq | HISLIP_ACK_TYPE));
    }
    return ret;
}

hislip_s32 hlk_msg_deal_func(hislip_u8* message, hislip_u16 len)
{
    hislip_s32 i;
    hilink_msg_info_t msg_info;
    hislip_s32 iteam = 0;
    hislip_u8 datatype = 0x0;
    hislip_u16 svc_inst = 0x0;
    hislip_u16 map_id = 0x0;
    hilink_msg_t *keyvals = NULL;
    
    hlk_tlv_t comtlv;    
    hislip_u8 *nxttlv = NULL;
    /* note : this data's format is tl(tlv...) [svcid len [keyid keylen keyval]]*/
    /* get svc mapid */
    nxttlv = hlk_general_func_parse_tlv(message, &comtlv);
    if (NULL == nxttlv)
    {
        return HISLIP_ERR;
    }
    svc_inst = comtlv.tag;
    nxttlv = comtlv.val;
    /* get key mapid */
    nxttlv = hlk_general_func_parse_tlv(nxttlv, &comtlv);
    if (NULL == nxttlv)
    {
        return HISLIP_ERR;
    }
    map_id = comtlv.tag;
   
    keyvals = (hilink_msg_t *)dev_get_profinfo_and_itssize(DEV_PROF_T_VALS, &iteam);
    if (NULL == keyvals)
    {
        return HISLIP_ERR;
    }
    for (i = 0; i < iteam; i++)
    {
        if ((map_id == keyvals[i].msg_info.map_id)
			&& (svc_inst == keyvals[i].msg_info.svc_inst))
        {     
            if (keyvals[i].msg_func)
            {	
                msg_info.svc_inst = svc_inst;
                msg_info.map_id = (short)map_id;
                datatype = hlk_get_key_datatype(map_id);
                switch (datatype)
                {
                    case HLK_DATA_TYPE_INT:
                    case HLK_DATA_TYPE_BOOL:
                        msg_info.value = (comtlv.val[0] << 24) + (comtlv.val[1] << 16) + 
                    	(comtlv.val[2] <<8) + comtlv.val[3];
                    	break;
                    case HLK_DATA_TYPE_STR:
                    	if ((0 != msg_info.value) && (0 != comtlv.len))
                        {
                            memcpy((char *)msg_info.value, comtlv.val, comtlv.len);
                        }
                        else
                        {
                            hlk_notify_error_info(HLK_ECODE_RCV_NETSTA_ERR);
                        }
                    	break;
                    default:
                    	return HISLIP_ERR;
                }

                keyvals[i].msg_func(&msg_info);
            }
            break;
        }
    }
    return HISLIP_SUCC;
}

#define HLK_DEAL

static hislip_s32 hlk_process_internal_states(hislip_s32 rcvval)
{
    hislip_s32 ret = HLK_RET_ERR;
    hislip_s32 runstate = 0;

    runstate = hlk_get_run_state();
    switch(runstate)
    {
        case HLK_RUN_STATE_INIT:
        case HLK_RUN_STATE_REGPROFILE:
        case HLK_RUN_STATE_SETWKMODE:
        case HLK_RUN_STATE_RPTINIVAL:
            ret = hlk_process_maxtrix_funcs(&g_func_matrix[runstate]);
            break;
        case HLK_RUN_STATE_IDLE:
            ret = hlk_process_idle_state(&g_func_matrix[runstate], rcvval);
            break;
        default:
            break;
    }
    return ret;
}


hislip_s32 hlk_cmd_process(hislip_u8 seq, hislip_u16 cmd, hislip_u8* buf, hislip_u16 len)
{	
	if (NULL == buf || 0 == len)
	{
		return HISLIP_ERR;
	}
	
    switch (cmd)
    {
    	case HLK_CMD_MDLRSP_WM:
    		hislip_send_ack_msg(seq, cmd, HISLIP_OK);
            hlk_update_work_mode(buf, len);
            break;
        case HLK_CMD_MDLRSP_RNS:
    		hislip_send_ack_msg(seq, cmd, HISLIP_OK);
            hlk_update_net_status(buf, len);
            break;
        case HLK_CMD_MDCTL_DEV_PROT:
    		hislip_send_ack_msg(seq, cmd, HISLIP_OK);
            hlk_msg_deal_func(buf, len);
            break;
        case HLK_CMD_DEV_QUPG:
            /* ecm check whether to upgrade */
            hlk_req_check_upg(seq, cmd, buf, len);
            break;
        case HLK_CMD_MDLRSP_UGD:
            /* mod tell ecm upg status and rate */
            hlk_tell_upg_info(seq, cmd, buf, len);
            break;
        case HLK_CMD_DEV_SUPG:
            /* tell start upgrade */
            hlk_start_upg(seq, cmd, buf, len);
            break;
        case HLK_CMD_DEV_TRANSING:
             /* transmit ota data */
            hlk_rcv_upg_data(seq, cmd, buf, len);
            break;
        case HLK_CMD_DEV_EUPG:
            /* tell end upgrade */
            hlk_end_upg(seq, cmd, buf, len);
    		break;
        case HLK_CMD_MDCTL_DEV_QPROT:
            hlk_process_query_keyvals(seq, buf, len);
    		break;  
        default:
            hlk_notify_error_info(HLK_ECODE_RCV_UNKNOWN_CMD);
            break;
    }
    
    return HISLIP_SUCC;
}

hislip_s32 hlk_process_profile_regflag(hislip_u8* buf, hislip_u16 len)
{
    hislip_u16 rlen = 0;
    hislip_s16 dlen = 0x0;
    hislip_s16 tag = 0x0;
    hislip_s32 offset = 0;
    
    /* get tag value */
    while (offset < len)
    {
        tag = hislip_parse_data_eabit(buf + offset, &rlen);
        if (rlen == 0)
        {
            break;
        }
        offset += rlen;
        /*get length */
        dlen = hislip_parse_data_eabit(buf + offset, &rlen);
        if (rlen == 0)
        {
            break;
        }
        offset += rlen;
        if (tag != HLK_TAG_OP_RLT)
        {
            offset += dlen;
            continue;
        }
        if (buf[offset] != 0x00)
        {
            /* wifi module already stored porofile, don't send profile to wifi module */
            hlk_jump_reg_func_index();
        }
        offset += dlen;
    }
    return 0;
}

hislip_s32 hlk_ack_process(hislip_u8 seq, hislip_u16 cmd, hislip_u8* buf, hislip_u16 len)
{	
	if (NULL == buf || 0 == len)
	{
		return HISLIP_ERR;
	}
	
    switch (cmd)
    {
    	case HLK_CMD_DEVCTL_QWM:
            hlk_update_work_mode(buf, len);
            break;
        case HLK_CMD_DEVCTL_QNS:
            hlk_update_net_status(buf, len);
            break;
        case HLK_CMD_DEVREG_SETSTA:
            hlk_process_profile_regflag((hislip_u8*)buf, len);
            break;
    	case HLK_CMD_DEVQRY_FINFOS:
            device_get_factory_infors((hislip_s8*)buf, len);
            break;
        default:
            break;
    }
    
    return HISLIP_SUCC;
}

void hilink_env_init(void)
{
    hislip_init();
    device_init_devprofile_vals();
    return ;
}

void hilink_main_process(void)
{
    hislip_s32 ret = HISLIP_OK;
    hislip_u8 temp_data[HISLIP_MAX_FRM_INFO_LEN];
    
    ret = hislip_rcv_data(temp_data, sizeof(temp_data));
    hlk_process_internal_states(ret);
    return;
}


