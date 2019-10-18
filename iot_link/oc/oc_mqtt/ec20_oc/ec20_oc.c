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

#include <string.h>
#include <at.h>
#include <ec20_oc.h>
#include <oc_mqtt_al.h>
#include <osal.h>

#define cn_ec20_config_timeout  (10*1000)
#define cn_ec20_cmd_timeout  (2*1000)
#define cn_ec20_rcvindex  "\r\n\r\n+HWOCMQTTRECEIVED="

typedef struct
{
    //initialized by the applicaiton
    oc_mqtt_msgdealer  msgdealer;

    //this is the control block
    bool_t            sndenable;
} ec20_cb_t;

static void  *s_oc_handle = NULL;
static ec20_cb_t   s_ec20_oc_cb;
static osal_mutex_t s_report_mutex;


static int ec20_rcvdeal(void *args,void *msg,size_t len)
{
    char *data;
    char *str;
    int datalen = 0;
    int qos = 0;
    mqtt_al_msgrcv_t mqtt_msg;

    data = msg;

    //printf("recv:%s len:%d\r\n", data, len);
#if 1
    str = strstr((char *)data,"=");
    str++;

    for (; *str <= '9' && *str >= '0' ;str++)
    {
        qos = (qos * 10 + (*str - '0'));
    }
    printf("qos:%d\r\n", qos);
    str++;
    mqtt_msg.qos = qos;
    for (; *str <= '9' && *str >= '0' ;str++)
    {
        datalen = (datalen * 10 + (*str - '0'));
    }
	printf("datalen:%d\r\n", datalen);

    str = strstr((char *)data,"{");
    mqtt_msg.msg.data = str;
    mqtt_msg.msg.len = datalen;
    mqtt_msg.topic.data = NULL;
    mqtt_msg.topic.len = 0;
    if(NULL != s_ec20_oc_cb.msgdealer)
    {
        s_ec20_oc_cb.msgdealer(NULL, &mqtt_msg);
    }
#endif
    return 0;
}


static bool_t ec20_atcmd(const char *cmd,const char *index)
{
    int ret = 0;
    unsigned int timeout;

    if(strstr(cmd, "CONNECT"))
        timeout = cn_ec20_config_timeout;
    else
        timeout = cn_ec20_cmd_timeout;
    ret = at_command((unsigned char *)cmd,strlen(cmd),index,NULL,0,timeout);
    if(ret >= 0)
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
    int ret = 0;
    char cmd[256];

    at_oobregister("ec20rcv",cn_ec20_rcvindex,strlen(cn_ec20_rcvindex),ec20_rcvdeal,NULL);
    s_ec20_oc_cb.msgdealer = param->msgdealer;

    if(NULL != param)
    {
        memset(cmd,0,256);
        snprintf(cmd,256,"at+HWOCMQTTCONNECT=%d,%d,\"%s\",\"%s\",\"%s\",\"%s\"\r", param->boot_mode, param->lifetime, param->server, param->port, param->device_info.s_device.deviceid, param->device_info.s_device.devicepasswd);
        printf("cmd:%s\r\n", cmd);
        ret = ec20_atcmd(cmd,"OK");
        if(ret)
        {
            s_ec20_oc_cb.sndenable = true;
            s_oc_handle = &s_ec20_oc_cb;
        }
    }
    return s_oc_handle;
}


static int ec20_oc_deconfig(void *handle)
{
    osal_mutex_del(s_report_mutex);
    s_oc_handle = NULL;
    return 0;
}

static int ec20_oc_report(void *handle,char *msg,int len,en_mqtt_al_qos_t qos)
{
    int ret = -1;

    char cmd[256];
    if ((NULL == msg) || (false == s_ec20_oc_cb.sndenable))
    {
        return ret;
    }

    memset(cmd,0,256);

    osal_mutex_lock(s_report_mutex);
    snprintf(cmd,256,"AT+HWOCMQTTSEND=%d,%d,\"%s\"\r", qos, len, msg);
    ret = ec20_atcmd(cmd,"OK");
    osal_mutex_lock(s_report_mutex);

    if(ret > 0)
    {
        ret = 0;
    }
    else
    {
        ret = -1;
    }

    return ret;
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

    osal_mutex_create(&s_report_mutex);
    ret = oc_mqtt_register(&g_ec20_oc_opt);

    return ret;
}
