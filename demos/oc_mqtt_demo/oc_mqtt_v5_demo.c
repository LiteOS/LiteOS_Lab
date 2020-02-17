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
 *  2020-02-07 09:06  zhangqianfu  The first version
 *
 */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <queue.h>

#include <osal.h>
#include <oc_mqtt_al.h>
#include <oc_mqtt_profile.h>

///< ANYWAY, YOU COULD CONFIG IT TO THE ONE MODE,ALL THE INFORMATION IS JUST FOR THE TEST
#if CONFIG_OC_MQTT_DEMO_BS

///< the device bootstrap center
#define CN_SERVER_IPV4         "119.3.251.30"
#define CN_SERVER_PORT         "8883"
#define CN_EP_DEVICEID         "54f107da-f251-436c-af4c-624f33b7d7b6"
#define CN_EP_PASSWD           "f62fcf47d62c4ed18913"
#define CN_BOOT_MODE           1

#else

#define CN_BOOT_MODE              0

///< the iot device access center
#define CN_SERVER_IPV4         "119.3.248.253"
#define CN_SERVER_PORT         "8883"
#define CN_EP_DEVICEID         "54f107da-f251-436c-af4c-624f33b7d7b6"
#define CN_EP_PASSWD           "f62fcf47d62c4ed18913"

/////< the iot develop center
//#define CN_SERVER_IPV4         "49.4.93.24"
//#define CN_SERVER_PORT         "8883"
//#define CN_EP_DEVICEID         "mqtt_sdk01"
//#define CN_EP_PASSWD           "c18f10422c93548e6fef"

#endif

#define CN_LIFE_TIME            60                 ///< the platform need more

//if your command is very fast,please use a queue here--TODO
static queue_t                   *s_queue_rcvmsg = NULL;   ///< this is used to cached the message
static oc_mqtt_profile_service_t  s_device_service;

//use this function to push all the message to the buffer
static int app_msg_deal(oc_mqtt_profile_msgrcv_t *msg)
{
    int    ret = -1;
    char  *buf;
    int    buflen;
    int    datalen = 0;
    oc_mqtt_profile_msgrcv_t *demo_msg;

    ///< clone the message
    buflen = sizeof(oc_mqtt_profile_msgrcv_t) + msg->msg_len + 1;///< we copy with '\0' endings
    if(NULL != msg->request_id)
    {
        buflen += strlen(msg->request_id) +1; ///< ending with '\0'
    }

    buf = osal_malloc(buflen);
    if(NULL != buf)
    {
        demo_msg = (oc_mqtt_profile_msgrcv_t *)buf;
        buf += sizeof(oc_mqtt_profile_msgrcv_t);
        ///< copy the message and push it to the queue
        demo_msg->type = msg->type;

        if(NULL != msg->request_id)
        {
            demo_msg->request_id = buf;
            datalen = strlen(msg->request_id);
            memcpy(buf,msg->request_id,datalen);
            buf[datalen] = '\0';
            buf += (datalen+1);
        }
        else
        {
            demo_msg->request_id = NULL;
        }

        demo_msg->msg = buf;
        demo_msg->msg_len = msg->msg_len;
        datalen = msg->msg_len;
        memcpy(buf,msg->msg,datalen);
        buf[datalen] = '\0';

        printf("RCVMSG:type:%d reuqestID:%s payloadlen:%d payload:%s\n\r",\
                demo_msg->type,demo_msg->request_id==NULL?"NULL":demo_msg->request_id,\
                demo_msg->msg_len,(char *)demo_msg->msg);

        ret = queue_push(s_queue_rcvmsg,demo_msg,10);
        if(ret != 0)
        {
            osal_free(demo_msg);
        }
    }

    return ret;
}

///< now we deal the message here
static int  oc_cmd_normal(oc_mqtt_profile_msgrcv_t *demo_msg)
{
    static int value = 0;
    oc_mqtt_profile_cmdresp_t  cmdresp;
    oc_mqtt_profile_propertysetresp_t propertysetresp;
    oc_mqtt_profile_propertygetresp_t propertygetresp;

    printf("DEALMSG:type:%d reuqestID:%s payloadlen:%d payload:%s\n\r",\
            demo_msg->type,demo_msg->request_id==NULL?"NULL":demo_msg->request_id,\
            demo_msg->msg_len,(char *)demo_msg->msg);
    switch(demo_msg->type)
    {
        case EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_MSGDOWN:
            ///< add your own deal here
            break;
        case EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_COMMANDS:
            ///< add your own deal here

            ///< do the response
            cmdresp.paras = NULL;
            cmdresp.request_id = demo_msg->request_id;
            cmdresp.ret_code = 0;
            cmdresp.ret_name = NULL;
            oc_mqtt_profile_cmdresp(NULL,&cmdresp);
            break;

        case EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_PROPERTYSET:
            ///< add your own deal here

            ///< do the response
            propertysetresp.request_id = demo_msg->request_id;
            propertysetresp.ret_code = 0;
            propertysetresp.ret_description = NULL;
            oc_mqtt_profile_propertysetresp(NULL,&propertysetresp);
            break;

        case  EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_PROPERTYGET:
            ///< add your own deal here

            ///< do the response
            value  = (value+1)%100;
            s_device_service.service_property->key = "radioValue";
            s_device_service.service_property->value = &value;
            s_device_service.service_property->type = EN_OC_MQTT_PROFILE_VALUE_INT;

            propertygetresp.request_id = demo_msg->request_id;
            propertygetresp.services = &s_device_service;
            oc_mqtt_profile_propertygetresp(NULL,&propertygetresp);
            break;

        default:
            break;

    }
    return 0;
}


static int  oc_report_normal(void)
{
    int ret = en_oc_mqtt_err_ok;
    static int value = 1;
    static int times = 0;

    times = (times + 1)%2;
    if(times == EN_OC_MQTT_PROFILE_MSG_TYPE_UP_MSGUP)
    {
        oc_mqtt_profile_msgup_t msgup;
        msgup.device_id = CN_EP_DEVICEID;
        msgup.id = "12345";
        msgup.name = "MSGUP";
        msgup.msg = "Hello,Message Up";
        msgup.msg_len = strlen(msgup.msg);

        ret = oc_mqtt_profile_msgup(NULL,&msgup);
    }
    else if(times == EN_OC_MQTT_PROFILE_MSG_TYPE_UP_PROPERTYREPORT)
    {
        value  = (value+1)%100;
        s_device_service.service_property->key = "radioValue";
        s_device_service.service_property->value = &value;
        s_device_service.service_property->type = EN_OC_MQTT_PROFILE_VALUE_INT;
        ret = oc_mqtt_profile_propertyreport(NULL,&s_device_service);
    }

    printf("REPORT TIMES:%d RET:%d\n\r",times,ret);
    return ret;
}


static int task_rcvmsg_entry( void *args)
{

    oc_mqtt_profile_msgrcv_t *demo_msg;

    while(1)
    {
        demo_msg = NULL;
        queue_pop(s_queue_rcvmsg,(void **)&demo_msg,cn_osal_timeout_forever);

        if(NULL != demo_msg)
        {
            oc_cmd_normal(demo_msg);
            osal_free(demo_msg);
        }
    }

    return 0;
}

static int task_reportmsg_entry(void *args)
{
    int ret;
    oc_mqtt_profile_connect_t  connect_para;

    connect_para.boostrap =      CN_BOOT_MODE;
    connect_para.device_id =     CN_EP_DEVICEID;
    connect_para.device_passwd = CN_EP_PASSWD;
    connect_para.server_addr =   CN_SERVER_IPV4;
    connect_para.sevver_port =   CN_SERVER_PORT;
    connect_para.life_time =     CN_LIFE_TIME;
    connect_para.rcvfunc =       app_msg_deal;

    ret = oc_mqtt_profile_connect(&connect_para);
    if((ret != en_oc_mqtt_err_ok))
    {
        printf("config:err :code:%d\r\n",ret);
        return -1;
    }

    while(1)  //do the loop here
    {
        oc_report_normal();
        osal_task_sleep(1000);
    }
    return 0;
}

int standard_app_demo_main()
{
    static oc_mqtt_profile_kv_t  property;
    s_queue_rcvmsg = queue_create("queue_rcvmsg",2,1);

    ///< initialize the service
    property.nxt   = NULL;
    s_device_service.event_time = NULL;
    s_device_service.service_id = "DeviceStatus";
    s_device_service.service_property = &property;
    s_device_service.nxt = NULL;

    osal_task_create("demo_reportmsg",task_reportmsg_entry,NULL,0x800,NULL,8);
    osal_task_create("demo_rcvmsg",task_rcvmsg_entry,NULL,0x800,NULL,8);

    return 0;
}
