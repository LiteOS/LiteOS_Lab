/*----------------------------------------------------------------------------
 * Copyright (c) <2016-2018>, <Huawei Technologies Co., Ltd>
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

#include <at.h>
#include <ec20_oc.h>
#include <oc_mqtt_al.h>
#include <osal.h>

#define cn_ec20_cmd_timeout  (2*1000)


static bool_t ec20_atcmd(const char *cmd,const char *index)
{
    int ret = 0;
    ret = at_command((unsigned char *)cmd,strlen(cmd),index,NULL,0,cn_ec20_cmd_timeout);
    if(ret > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}


static void* ec20_oc_config(tag_oc_mqtt_config *param)
{
    char cmd[128];
    char resp[64];

    memset(cmd,0,128);
    //snprintf(cmd,128,"AT+CGDCONT=%s\r",apn);

    ec20_atcmd("aat+HWOCMQTTCONNECT=1,30,\"iot-bs.cn-north-4.myhuaweicloud.com\",\"8883\",\"sdk_0040\",\"f62fcf47d62c4ed18913\"","OK");

}


static int ec20_oc_deconfig(void *handle)
{

}

static int ec20_oc_report(void *handle,char *msg,int len,en_mqtt_al_qos_t qos)
{



}



const tag_oc_mqtt_ops  g_ec20_oc_opt = \
{
    .config = ec20_oc_config,
    .deconfig = ec20_oc_deconfig,
    .report = ec20_oc_report,
};


int ec20_init(void)
{
    int ret = -1;

    ret = oc_mqtt_register(&g_ec20_oc_opt);

    return ret;
}

