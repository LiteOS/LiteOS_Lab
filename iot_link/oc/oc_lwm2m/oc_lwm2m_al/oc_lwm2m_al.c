/*----------------------------------------------------------------------------
 * Copyright (c) <2018>, <Huawei Technologies Co., Ltd>
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
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/
/**
 *  DATE                AUTHOR      INSTRUCTION
 *  2019-05-14 17:22  zhangqianfu  The first version
 *
 */

#define CFG_OC_LWM2M_ENABLE  1  //for test


#if CFG_OC_LWM2M_ENABLE

#include <stdlib.h>
#include <osport.h>
#include <oc_lwm2m_al.h>

static oc_lwm2m_opt_t *s_oc_lwm2m_agent = NULL;

typedef struct
{
    void       *nxt;        // next item
    const char *match_buf;  //used to compare
    int     match_len;  //compare length
    fn_oc_msg_deal dealer;  //the message dealer
}tag_msg_dealer_item;       //item structure

typedef struct
{
    osal_mutex_t               mux;
    tag_msg_dealer_item       *item;
}oc_lwm2m_msgcb_t;

static oc_lwm2m_msgcb_t  s_msg_dealer_cb;

int oc_lwm2m_register(const oc_lwm2m_opt_t *opt)
{
    int ret = -1;

    if(NULL != opt)
    {
        s_oc_lwm2m_agent = (oc_lwm2m_opt_t *) opt;

        ret = 0;
    }

    return ret;
}

int oc_lwm2m_msg_push(uint8_t *msg, int len)
{
    int ret = -1;
    tag_msg_dealer_item *item;

    if(mutex_lock(s_msg_dealer_cb.mux))
    {
        item = s_msg_dealer_cb.item;

        while(NULL != item)
        {
            if((item->match_buf == NULL)||((len >= item->match_len)&&\
               (0 == memcmp(msg,item->match_buf,item->match_len))))
            {
                ret = item->dealer(msg,len);
            }
            item = item->nxt;
        }

        mutex_unlock(s_msg_dealer_cb.mux);
    }

    return ret;
}

//////////////////////////APPLICATION INTERFACE/////////////////////////////////
int oc_lwm2m_install_msgdealer(fn_oc_msg_deal dealer,const char *match_buf,int match_len)
{
    int ret = -1;

    tag_msg_dealer_item *item;

    if(NULL == dealer)
    {
        return ret;
    }

    item = malloc(sizeof(tag_msg_dealer_item));
    if(NULL== item)
    {
        return ret;
    }

    item->dealer = dealer;
    item->match_buf = match_buf;
    item->match_len = match_len;

    if(mutex_lock(s_msg_dealer_cb.mux))
    {
        item->nxt = s_msg_dealer_cb.item;
        s_msg_dealer_cb.item = item;

        mutex_unlock(s_msg_dealer_cb.mux);

        ret = 0;
    }
    else
    {
        free(item);
    }

    return ret;

}


int oc_lwm2m_report(uint8_t *buf, int len,int timeout)
{
    int ret = -1;

    if((NULL != s_oc_lwm2m_agent) &&(NULL != s_oc_lwm2m_agent->report))
    {
       ret = s_oc_lwm2m_agent->report(buf,len,timeout);
    }

    return ret;
}


int oc_lwm2m_config(oc_config_param_t *param)
{
    int ret = -1;

    if((NULL != s_oc_lwm2m_agent) &&(NULL != s_oc_lwm2m_agent->config))
    {
       ret = s_oc_lwm2m_agent->config(param);
    }

    return ret;
}


int oc_lwm2m_deconfig()
{
    int ret = -1;

    if((NULL != s_oc_lwm2m_agent) &&(NULL != s_oc_lwm2m_agent->deconfig))
    {
       ret = s_oc_lwm2m_agent->deconfig();
    }

    return ret;
}


///////////////////////OC LWM2M AGENT INITIALIZE////////////////////////////////
int oc_lwm2m_init()
{
    int ret = -1;

    if (osal_mutex_create(&s_msg_dealer_cb.mux))
    {
        ret = 0;
    }

    return ret;
}

#endif
