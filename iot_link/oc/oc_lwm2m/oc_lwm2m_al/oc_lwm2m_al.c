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

#include <stdint.h>
#include <stddef.h>

#include <string.h>

#include <stdlib.h>
#include <osal.h>
#include <oc_lwm2m_al.h>

typedef struct
{
    const char            *name;    ///< lwm2m implement name
    const oc_lwm2m_opt_t  *opt;     ///< lwm2m implement method
} oc_lwm2m_t;
static oc_lwm2m_t  s_oc_lwm2m_ops;


int oc_lwm2m_register(const char *name, const oc_lwm2m_opt_t *opt)
{
    int ret = -1;

    if (NULL == s_oc_lwm2m_ops.opt)
    {
        s_oc_lwm2m_ops.name = name;
        s_oc_lwm2m_ops.opt =  opt;
        ret = 0;
    }

    return ret;
}

int oc_lwm2m_unregister(const char *name)
{
    int ret = -1;

    if ((NULL != name) && (NULL != s_oc_lwm2m_ops.name))
    {
        if (0 == strcmp(name, s_oc_lwm2m_ops.name))
        {
            s_oc_lwm2m_ops.opt = NULL;   ///< also think about clear all the ops in the context,
            s_oc_lwm2m_ops.name = NULL;
            ret = 0;
        }
    }

    return ret;
}


//////////////////////////APPLICATION INTERFACE/////////////////////////////////
int oc_lwm2m_report(char  *buf, int len, int timeout)
{
    int ret = en_oc_lwm2m_err_system;

    if ((NULL != s_oc_lwm2m_ops.opt) && (NULL != s_oc_lwm2m_ops.opt->report))
    {
        ret = s_oc_lwm2m_ops.opt->report(buf, len, timeout);
    }

    return ret;
}


int oc_lwm2m_config( oc_config_param_t *param)
{
    int ret = en_oc_lwm2m_err_system;

    if ((NULL != s_oc_lwm2m_ops.opt) && (NULL != s_oc_lwm2m_ops.opt->config))
    {
        if(NULL != param)
        {
            ret = s_oc_lwm2m_ops.opt->config(param);
        }
        else
        {
            ret = en_oc_lwm2m_err_parafmt;
        }
    }

    return ret;
}


int oc_lwm2m_deconfig(void)
{
    int ret = en_oc_lwm2m_err_system;

    if ((NULL != s_oc_lwm2m_ops.opt) && (NULL != s_oc_lwm2m_ops.opt->deconfig))
    {
        ret = s_oc_lwm2m_ops.opt->deconfig();
    }

    return ret;
}

///////////////////////OC LWM2M AGENT INITIALIZE////////////////////////////////
int oc_lwm2m_init()
{
    int ret = -1;
    ret = 0;   ///< uptils now, we should do nothing here
    return ret;
}

