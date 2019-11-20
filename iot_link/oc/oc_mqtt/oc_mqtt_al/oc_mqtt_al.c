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

#include <oc_mqtt_al.h>

static oc_mqtt_t *s_oc_mqtt = NULL;
///////////////////////OC AGENT INSTALL INTERFACE///////////////////////////////
int oc_mqtt_register(const oc_mqtt_t *opt)
{
    int ret = en_oc_mqtt_err_system;

    if(NULL != opt)
    {
        s_oc_mqtt = (oc_mqtt_t *) opt;
        ret = en_oc_mqtt_err_ok;
    }

    return ret;
}

//////////////////////////APPLICATION INTERFACE/////////////////////////////////
int oc_mqtt_config(oc_mqtt_config_t *param)
{
    int ret = en_oc_mqtt_err_system ;
    if((NULL != s_oc_mqtt) &&(NULL != s_oc_mqtt->op.config))
    {
       ret = s_oc_mqtt->op.config(param);
    }

    return ret;
}

int oc_mqtt_deconfig()
{
    int ret = en_oc_mqtt_err_system;

    if((NULL != s_oc_mqtt) \
       &&(NULL != s_oc_mqtt->op.config))
    {
       ret = s_oc_mqtt->op.deconfig();
    }

    return ret;
}

int oc_mqtt_publish(char  *topic,uint8_t *msg,int msg_len,int qos)
{
    int ret = en_oc_mqtt_err_system;

    if((NULL != s_oc_mqtt) &&(NULL != s_oc_mqtt->op.publish))
    {
       ret = s_oc_mqtt->op.publish(topic,msg,msg_len,qos);
    }

    return ret;
}

int oc_mqtt_report(uint8_t *msg, int len, int qos)
{
    int ret = en_oc_mqtt_err_system;

    ret = oc_mqtt_publish(NULL,msg,len,qos);

    return ret;
}

int oc_mqtt_subscribe(char *topic,int qos)
{
    int ret = en_oc_mqtt_err_system;

    if((NULL != s_oc_mqtt) &&(NULL != s_oc_mqtt->op.subscribe))
    {
       ret = s_oc_mqtt->op.subscribe(topic,qos);
    }

    return ret;
}


///////////////////////OC LWM2M AGENT INITIALIZE////////////////////////////////
int oc_mqtt_init()
{
    return 0;
}


static const char *s_oc_mqtt_err_tab[en_oc_mqtt_err_last] =
{
    "success",
    "parameter_err",
    "network_err",
    "version_err",
    "clientid_err",
    "server_err",
    "userpwd_err",
    "clientauth_err",
    "subscribe_err",
    "publish_err",
    "reconfigure_err",
    "nonconfigure_err",
    "mqttconnect_err",
    "bsaddrtimeout_err",
    "sysmemory_err",
    "system_err",
};

const char *oc_mqtt_err(int code)
{
    const char *ret = NULL;
    if((code >= en_oc_mqtt_err_last) || (code < 0))
    {
        ret =  "UNKNOWN";
    }
    else
    {
        ret = s_oc_mqtt_err_tab[code];
    }
    return ret;
}




