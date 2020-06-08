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
 *  2020-05-15 10:18  zhangqianfu  The first version
 *
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <link_log.h>
#include <queue.h>
#include <oc_mqtt_al.h>
#include <oc_mqtt_profile.h>
#include <ota_flag.h>

const char* g_signature_public=
"-----BEGIN RSA PUBLIC KEY-----\r\n"
"MIIBCgKCAQEAz9u5vpDoov9DDrkWkwdWQnLjiYXO3RuwXmcSCu/N1Wrv55b3w/BJ\r\n"
"iXl7mTv1zWrU9gL+jdMXrxP6BK5nOh3wa8tiPGqPnM2tNCUVEt2dmDasroh8VVv1\r\n"
"9yOUiSlNGSZ+UrnuUlAzMLt0GJrCPHesapOQ7OkAQd2SNbQfv/vFmXzUcNUAZxr4\r\n"
"zJmSoZT9aXTO/RfShlsgrPtpz+8sejcRR5s4LKn5KsJwjqJ+sHmnEKlcGiciNGIx\r\n"
"Ajf2nFigo7QrZ+4o6kvweNA05Ptg29j/0JPr0WbyLsCWDVaAneelh8Sl3TPdZYOM\r\n"
"6iHE2k1sLBeP7X3lymWh3BZ9rU+xngi9lQIDAQAB\r\n"
"-----END RSA PUBLIC KEY-----\r\n";

//#define CN_SERVER_IPV4       "iot-mqtts.cn-north-4.myhuaweicloud.com"
#define CN_SERVER_IPV4                 "121.36.42.100"
#define CN_SERVER_PORT                 "1883"
#define CN_SECURITY_TYPE               EN_DTLS_AL_SECURITY_TYPE_NONE

#ifndef CONFIG_OCMQTTV5_DEMO_DEVICEID
//#define CONFIG_OCMQTTV5_DEMO_DEVICEID  "5e12ea0a334dd4f337902dc3_iotlink005"
#define CONFIG_OCMQTTV5_DEMO_DEVICEID  "5ec3f516cce62b02c56524a9_otamqtt003"
#endif

#ifndef CONFIG_OCMQTTV5_DEMO_DEVPWD
#define CONFIG_OCMQTTV5_DEMO_DEVPWD     "f62fcf47d62c4ed18913"
#endif

#ifndef CONFIG_OCMQTTV5_DEMO_REPORTCYCLE
#define CONFIG_OCMQTTV5_DEMO_REPORTCYCLE   (10*1000)
#endif

#define CN_EP_DEVICEID        CONFIG_OCMQTTV5_DEMO_DEVICEID
#define CN_EP_PASSWD          CONFIG_OCMQTTV5_DEMO_DEVPWD


#define CN_BOOT_MODE           0
#define CN_LIFE_TIME           40                         ///< the platform need more
#define CN_OTA_SOTA_VERSION    "SOTAV1"
#define CN_OTA_FOTA_VERSION    "FOTAV1"
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
        demo_msg = (oc_mqtt_profile_msgrcv_t *)(uintptr_t)buf;
        buf += sizeof(oc_mqtt_profile_msgrcv_t);
        ///< copy the message and push it to the queue
        demo_msg->type = msg->type;

        if(NULL != msg->request_id)
        {
            demo_msg->request_id = buf;
            datalen = strlen(msg->request_id);
            (void) memcpy(buf,msg->request_id,datalen);
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
        (void) memcpy(buf,msg->msg,datalen);
        buf[datalen] = '\0';

        LINK_LOG_DEBUG("RCVMSG:type:%d reuqestID:%s payloadlen:%d payload:%s\n\r",\
                (int) demo_msg->type,demo_msg->request_id==NULL?"NULL":demo_msg->request_id,\
                demo_msg->msg_len,(char *)demo_msg->msg);

        ret = queue_push(s_queue_rcvmsg,demo_msg,10);
        if(ret != 0)
        {
            osal_free(demo_msg);
        }
    }

    return ret;
}

#include <cJSON.h>
#include <ota_https.h>
#define CN_EVENT_SERVICES_ID         "services"
#define CN_EVENT_TYPE_INDEX          "event_type"
#define CN_EVENT_TYPE_VERSIONQUERY   "version_query"
#define CN_EVENT_TYPE_FIRMUPDATE     "firmware_upgrade"

static int oc_cmd_event_versionquery(cJSON *event)
{
    int ret;
    char *topic = "$oc/devices/"CN_EP_DEVICEID"/sys/events/up";
    char *fmt = "{ \"services\": [{ \"service_id\": \"$ota\", \"event_type\": \"version_report\", \"paras\": { \"sw_version\":\"v1.0\",\"fw_version\":\"%s\" } }]}";
    char *data;
    int   len;

    len = strlen(fmt) + strlen(CN_OTA_FOTA_VERSION) + 1;
    data = osal_malloc(len);
    if(NULL != data)
    {
        snprintf(data, len,fmt, CN_OTA_FOTA_VERSION);
        LINK_LOG_DEBUG("REPLY:CURVERSION:%s",data);
        ret = oc_mqtt_publish(topic,(uint8_t *)data, strlen(data),0);
        osal_free(data);
    }
    return ret;
}

//Topic: $oc/devices/{device_id}/sys/events/up
//数据格式：
//{
//    "object_device_id": "{object_device_id}",
//    "services": [{
//        "service_id": "$ota",
//        "event_type": "upgrade_progress_report",
//        "event_time": "20151212T121212Z",
//        "paras": {
//            "result_code": 0,
//            "progress": 80,
//            "version": "V2.0",
//            "description": "upgrade processing"
//        }
//    }]
//}

static int oc_report_upgraderet(int upgraderet, const char *version)
{
    int ret;
    char *topic = "$oc/devices/"CN_EP_DEVICEID"/sys/events/up";
    char *fmt = "{ \"services\": [{ \"service_id\": \"$ota\", \"event_type\": \"upgrade_progress_report\", \"paras\": { \"result_code\":%d,\"version\":\"%s\" } }]}";
    char *data;
    int   len;

    len = strlen(fmt) + strlen(version) + sizeof(upgraderet);
    data = osal_malloc(len);
    if(NULL != data)
    {
        snprintf(data, len, fmt,upgraderet, version);
        LINK_LOG_DEBUG("REPORT:UPGRADERET:%s",data);
        ret = oc_mqtt_publish(topic,(uint8_t *)data, strlen(data),0);
        osal_free(data);
    }
    return ret;
}


static int oc_cmd_event_firmupdate(cJSON *event)
{
    int ret = -1;
    cJSON *obj_paras;
    cJSON *obj_version;
    cJSON *obj_url;
    cJSON *obj_filesize;
    cJSON *obj_accesstoken;
    cJSON *obj_sign;
    ota_https_para_t *otapara;

    otapara = osal_malloc(sizeof(ota_https_para_t));
    memset(otapara, 0, sizeof(ota_https_para_t));
    if(NULL == otapara)
    {
        return ret;
    }

    obj_paras = cJSON_GetObjectItem(event, "paras");
    if(NULL != obj_paras)
    {
        obj_version = cJSON_GetObjectItem(obj_paras, "version");
        obj_url = cJSON_GetObjectItem(obj_paras, "url");
        obj_filesize = cJSON_GetObjectItem(obj_paras, "file_size");
        obj_accesstoken = cJSON_GetObjectItem(obj_paras, "access_token");
        obj_sign = cJSON_GetObjectItem(obj_paras, "sign");

        if((NULL != obj_version) && (NULL != obj_url) && (NULL != obj_filesize) && \
                (NULL != obj_accesstoken) && (NULL != obj_sign))
        {
            otapara->authorize = cJSON_GetStringValue(obj_accesstoken);
            otapara->url = cJSON_GetStringValue(obj_url);
            otapara->signature = cJSON_GetStringValue(obj_sign);
            otapara->file_size = obj_filesize->valueint;
            otapara->version = cJSON_GetStringValue(obj_version);
            otapara->signature_public = g_signature_public;
            ///< here we do the firmware download
            ret =  ota_https_download(otapara);
            if(ret != 0)
            {
                oc_report_upgraderet(6,otapara->version);
                LINK_LOG_ERROR("DOWNLOADING ERR");
            }
            else
            {
                LINK_LOG_DEBUG("DOWNLOADING SUCCESS");
                osal_task_sleep(5*1000);
                osal_reboot();
            }
            ret = 0;
        }
    }

    osal_free(otapara);

    return ret;
}


static int oc_cmd_event(oc_mqtt_profile_msgrcv_t *msg)
{
    int ret = -1;
    cJSON *obj_root;
    cJSON *obj_servicearry;
    cJSON *obj_service;
    cJSON *obj_eventtype;

    obj_root = cJSON_Parse(msg->msg);
    if(NULL == obj_root)
    {
        goto EXIT_JSONFMT;
    }

    obj_servicearry = cJSON_GetObjectItem(obj_root,CN_EVENT_SERVICES_ID);
    if((NULL == obj_servicearry) || (!cJSON_IsArray(obj_servicearry)))
    {
        goto EXIT_JSONSERVICEARRY;
    }

    cJSON_ArrayForEach(obj_service,obj_servicearry)
    {
        obj_eventtype = cJSON_GetObjectItem(obj_service,CN_EVENT_TYPE_INDEX);
        if(NULL == obj_eventtype)
        {
            continue;
        }
        if(0 == strcmp(cJSON_GetStringValue(obj_eventtype),CN_EVENT_TYPE_VERSIONQUERY))
        {
            oc_cmd_event_versionquery(obj_service);
        }
        else if(0 == strcmp(cJSON_GetStringValue(obj_eventtype),CN_EVENT_TYPE_FIRMUPDATE))
        {
            oc_cmd_event_firmupdate(obj_service);
        }
    }
    cJSON_Delete(obj_root);
    return 0;

EXIT_JSONSERVICEARRY:
    cJSON_Delete(obj_root);
EXIT_JSONFMT:
    return ret;
}

///< now we deal the message here
static int  oc_cmd_normal(oc_mqtt_profile_msgrcv_t *demo_msg)
{
    static int value = 0;
    oc_mqtt_profile_cmdresp_t  cmdresp;
    oc_mqtt_profile_propertysetresp_t propertysetresp;
    oc_mqtt_profile_propertygetresp_t propertygetresp;

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
            (void)oc_mqtt_profile_cmdresp(NULL,&cmdresp);
            break;

        case EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_PROPERTYSET:
            ///< add your own deal here

            ///< do the response
            propertysetresp.request_id = demo_msg->request_id;
            propertysetresp.ret_code = 0;
            propertysetresp.ret_description = NULL;
            (void)oc_mqtt_profile_propertysetresp(NULL,&propertysetresp);
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
            (void)oc_mqtt_profile_propertygetresp(NULL,&propertygetresp);
            break;
        case EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_EVENT:
            oc_cmd_event(demo_msg);
            break;

        default:
            break;

    }
    return 0;
}

static int demo_pubdefault(void)
{
    int ret ;
    static int radio = 0;
    const char *fmt = "{\"services\":[{\"service_id\":\"DeviceStatus\",\"properties\":{\"radioValue\":%d}}]}";
    char *data;
    int   len;

    len = strlen(fmt) + 16;
    data = osal_malloc(len);
    if(NULL != data)
    {
        snprintf(data, len, fmt,radio++);
        ret = oc_mqtt_publish(NULL,(uint8_t *)data, strlen(data),0);
        osal_free(data);
    }
    return ret;
}



static int task_rcvmsg_entry( void *args)
{
    oc_mqtt_profile_msgrcv_t *demo_msg;
    while(1)
    {
        demo_msg = NULL;
        (void)queue_pop(s_queue_rcvmsg,(void **)&demo_msg,(int)cn_osal_timeout_forever);
        if(NULL != demo_msg)
        {
            (void) oc_cmd_normal(demo_msg);
            osal_free(demo_msg);
        }
    }
    return 0;
}

void  hwoc_mqtt_log(en_oc_mqtt_log_t  logtype)
{

    if(logtype == en_oc_mqtt_log_connected)
    {
        (void)printf("%s:connected %d\n\r",__FUNCTION__,(int)logtype);
    }
    else
    {
        (void)printf("%s:disconnected %d\n\r",__FUNCTION__,(int)logtype);
    }

    return;
}

static int task_reportmsg_entry(void *args)
{
    int ret = -1;
    oc_mqtt_profile_connect_t  connect_para;
    static ota_flag_t  otaflag;


    if(0 != ota_flag_get(EN_OTA_TYPE_FOTA,&otaflag))
    {
        LINK_LOG_ERROR("GET FOTA FLAG ERR AND SHOULD DO THE INITIALIZE");
        memset(&otaflag,0,sizeof(otaflag));
        otaflag.info.curstatus = EN_OTA_STATUS_IDLE;
        ret = ota_flag_save(EN_OTA_TYPE_FOTA,&otaflag);
        if(0 != ret)
        {
            LINK_LOG_ERROR("FLAG SAVE FAILED AND WE SHOULD QUIT");
            return ret;
        }
    }

    (void) memset( &connect_para, 0, sizeof(connect_para));
    connect_para.boostrap =      CN_BOOT_MODE;
    connect_para.device_id =     CN_EP_DEVICEID;
    connect_para.device_passwd = CN_EP_PASSWD;
    connect_para.server_addr =   CN_SERVER_IPV4;
    connect_para.sevver_port =   CN_SERVER_PORT;
    connect_para.life_time =     CN_LIFE_TIME;
    connect_para.rcvfunc =       app_msg_deal;
    connect_para.logfunc  = hwoc_mqtt_log;
    connect_para.security.type = CN_SECURITY_TYPE;
    ret = oc_mqtt_profile_connect(&connect_para);
    if((ret != (int)en_oc_mqtt_err_ok))
    {
        LINK_LOG_ERROR("CONNECT ERR");
        return ret;
    }

    if(otaflag.info.curstatus == EN_OTA_STATUS_UPGRADED_SUCCESS)
    {
        otaflag.info.curstatus = EN_OTA_STATUS_IDLE;
        ota_flag_save(EN_OTA_TYPE_FOTA,&otaflag);
        oc_report_upgraderet(0,CN_OTA_FOTA_VERSION);
    }
    else if(otaflag.info.curstatus == EN_OTA_STATUS_UPGRADED_FAILED)
    {
        otaflag.info.curstatus = EN_OTA_STATUS_IDLE;
        ota_flag_save(EN_OTA_TYPE_FOTA,&otaflag);
        oc_report_upgraderet(1,CN_OTA_FOTA_VERSION);
    }

    while(1)  //do the loop here
    {
        demo_pubdefault();
        osal_task_sleep(CONFIG_OCMQTTV5_DEMO_REPORTCYCLE);
    }
    return 0;
}



int standard_app_demo_main(void)
{
    static oc_mqtt_profile_kv_t  property;

    LINK_LOG_DEBUG("This is STM32F429IGTX MQTT OTA application--VERSION:%s",CN_OTA_FOTA_VERSION);
    s_queue_rcvmsg = queue_create("queue_rcvmsg",2,1);
    ///< initialize the service
    property.nxt   = NULL;
    s_device_service.event_time = NULL;
    s_device_service.service_id = "DeviceStatus";
    s_device_service.service_property = &property;
    s_device_service.nxt = NULL;

    (void) osal_task_create("demo_reportmsg",task_reportmsg_entry,NULL,0x800,NULL,8);
    (void) osal_task_create("demo_rcvmsg",task_rcvmsg_entry,NULL,0x1800,NULL,8);

    return 0;
}


static int version_shell(int argc, const char *argv[])
{
    LINK_LOG_DEBUG("S_VERSION:%s F_VERSION:%s",CN_OTA_SOTA_VERSION,CN_OTA_FOTA_VERSION);
    return 0;
}

#if CONFIG_LITEOS_ENABLE

#include <shell.h>
OSSHELL_EXPORT_CMD(version_shell,"version","version");
#endif

#if CONFIG_NOVAOS_ENABLE

#include <cmder.h>
static int nova_command( cmder_t *cer,int argc,char *argv[])
{

    return version_shell(argc, (const char **)argv);
}

CMDER_CMD_DEF ("version","version",nova_command);
#endif


