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


#include <string.h>
#include <stdio.h>

#include "oc_mqtt_profile.h"
#include "link_misc.h"
#include "osal.h"
#include "oc_mqtt_al.h"
#include "oc_mqtt_profile_package.h"

typedef struct
{
    char                        *device_id;
    FnOcMqttRcvDeal   rcvfunc;
}oc_mqtt_profile_cb_t;

static oc_mqtt_profile_cb_t s_oc_mqtt_profile_cb;


/**
 * @brief: we use this function to deal all the received message and passed to the user
 *
 *
 * @return: 0 success while -1 failed, not used yet
 * */

//use this function to push all the message to the buffer

#define CN_OC_MQTT_PROFILE_REQUESTID_INDEX             "request_id="
#define CN_OC_MQTT_PROFILE_MSGDOWN_INDEX               "/sys/messages/down"
#define CN_OC_MQTT_PROFILE_SETPROPERTY_INDEX           "/sys/properties/set/"
#define CN_OC_MQTT_PROFILE_GETPROPERTY_INDEX           "/sys/properties/get/"
#define CN_OC_MQTT_PROFILE_CMD_INDEX                   "/sys/commands/"
#define CN_OC_MQTT_PROFILE_EVENTDOWN_INDEX             "/sys/events/down"
#define CN_OC_MQTT_PROFILE_BS_INDEX                    "/sys/bootstrap/down"
#define CN_OC_MQTT_PROFILE_GETSHADOW_INDEX             "/sys/shadow/get"

static int app_msg_deal(void *arg,mqtt_al_msgrcv_t *msg)
{
    int ret = -1;

    char *request_id = NULL;
    char *topicbuf;
    OcMqttMsgRcvT  message;

    topicbuf = osal_malloc(msg->topic.len + 1);
    if(NULL == topicbuf)
    {
        return ret;
    }
    memcpy(topicbuf, msg->topic.data, msg->topic.len);
    topicbuf[msg->topic.len] = '\0';
    if(NULL != s_oc_mqtt_profile_cb.rcvfunc)
    {
        message.msgContent = msg->msg.data;
        message.msgLen = msg->msg.len;

        request_id = strstr(topicbuf,CN_OC_MQTT_PROFILE_REQUESTID_INDEX);
        if(NULL != request_id)
        {
            request_id  += strlen(CN_OC_MQTT_PROFILE_REQUESTID_INDEX);
            message.requestId = request_id;
        }
        else
        {
            message.requestId = NULL;
        }

        if(NULL != strstr(topicbuf,CN_OC_MQTT_PROFILE_MSGDOWN_INDEX))
        {
            message.type = EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_MSGDOWN;
        }
        else if(NULL != strstr(topicbuf,CN_OC_MQTT_PROFILE_SETPROPERTY_INDEX))
        {
            message.type = EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_PROPERTYSET;
        }
        else if(NULL != strstr(topicbuf,CN_OC_MQTT_PROFILE_GETPROPERTY_INDEX))
        {
            message.type = EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_PROPERTYGET;
        }
        else if(NULL != strstr(topicbuf,CN_OC_MQTT_PROFILE_CMD_INDEX))
        {
            message.type = EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_COMMANDS;
        }
        else if(NULL != strstr(topicbuf,CN_OC_MQTT_PROFILE_EVENTDOWN_INDEX))
        {
            message.type = EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_EVENT;
        }
        else if(NULL != strstr(topicbuf,CN_OC_MQTT_PROFILE_BS_INDEX))
        {
            message.type = EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_BS;
        }
        else if(NULL != strstr(topicbuf,CN_OC_MQTT_PROFILE_GETSHADOW_INDEX))
        {
            message.type = EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_SHADOWGET;
        }
        else
        {
            message.type = EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_LAST;
        }
        if(NULL !=  s_oc_mqtt_profile_cb.rcvfunc)
        {
            s_oc_mqtt_profile_cb.rcvfunc(&message);
        }
    }

    osal_free(topicbuf);
    return ret;
}


///< use this function to connect to the cloud platform
int OcMqttConnect(OcMqttConnectT *connectPara)
{
    int ret = (int)EN_OC_MQTT_ERR_PARAFMT;
    oc_mqtt_config_t config;

    if(NULL == connectPara)
    {
        return ret;
    }

    (void) memset(&config,0, sizeof(config));
    config.boot_mode =connectPara->boostrap;
    config.id = connectPara->deviceId;
    config.pwd = connectPara->devicePasswd;
    config.scope_id = connectPara->scopeId;
    config.server_addr = connectPara->serverAddr;
    config.server_port = connectPara->serverPort;
    config.lifetime = connectPara->lifeTime;
    config.msg_deal = app_msg_deal;
    config.msg_deal_arg = NULL;
    config.security = connectPara->security;
    config.log_dealer = connectPara->logFunc;

    if(connectPara->boostrap)
    {
        config.boot_mode  = en_oc_mqtt_mode_bs_static_nodeid_hmacsha256_notimecheck_json;
    }
    else
    {
        config.boot_mode = en_oc_mqtt_mode_nobs_static_nodeid_hmacsha256_notimecheck_json;
    }

    if(NULL != s_oc_mqtt_profile_cb.device_id)
    {
        osal_free(s_oc_mqtt_profile_cb.device_id);
    }
    s_oc_mqtt_profile_cb.device_id = osal_strdup(connectPara->deviceId);
    s_oc_mqtt_profile_cb.rcvfunc = connectPara->rcvFunc;
    ret = oc_mqtt_config(&config);

    return ret;
}

///< use this function to disconnect from the cloudplatform
int OcMqttDisConnect(void)
{
    int ret;

    ret = oc_mqtt_deconfig();
    if(ret == (int)EN_OC_MQTT_ERR_OK)
    {
        osal_free(s_oc_mqtt_profile_cb.device_id);
        s_oc_mqtt_profile_cb.device_id = NULL;
        s_oc_mqtt_profile_cb.rcvfunc = NULL;
    }

    return ret;
}


///< use this function to make a topic to publish
///< if request_id  is needed depends on the fmt
static char *topic_make(char *fmt, char *device_id, char *request_id)
{
    int len;
    char *ret = NULL;

    if(NULL == device_id)
    {
        return ret;
    }
    len = strlen(fmt) + strlen(device_id);
    if(NULL != request_id)
    {
        len += strlen(request_id);
    }

    ret = osal_malloc(len);
    if(NULL != ret)
    {
        (void) snprintf(ret,len,fmt,device_id,request_id);
    }
    return ret;
}


///< use this function to report the messsage
#define CN_OC_MQTT_PROFILE_MSGUP_TOPICFMT   "$oc/devices/%s/sys/messages/up"
int OcMqttMessageReport(char *deviceId,OcMqttMessageReportT *message)
{
    int ret = (int)EN_OC_MQTT_ERR_PARAFMT;
    char *topic;
    char *msg;

    if(NULL == deviceId)
    {
        if(NULL == s_oc_mqtt_profile_cb.device_id)
        {
            return ret;
        }
        else
        {
            deviceId = s_oc_mqtt_profile_cb.device_id;
        }
    }

    if((NULL == message) || (NULL == message->msgContent))
    {
        return ret;
    }

    topic = topic_make(CN_OC_MQTT_PROFILE_MSGUP_TOPICFMT, deviceId,NULL);
    msg = oc_mqtt_profile_package_msgup(message);

    if((NULL != topic) && (NULL != msg))
    {
        ret = oc_mqtt_publish(topic,(uint8_t *)msg,strlen(msg),(int)en_mqtt_al_qos_1);
    }
    else
    {
        ret = (int)EN_OC_MQTT_ERR_SYSMEM;
    }

    osal_free(topic);
    osal_free(msg);

    return ret;
}

#define CN_OC_MQTT_PROFILE_PROPERTYREPORT_TOPICFMT   "$oc/devices/%s/sys/properties/report"
int OcMqttPropertiesReport(char *deviceid,OcMqttServiceT *property)
{
    int ret = (int)EN_OC_MQTT_ERR_PARAFMT;
    char *topic;
    char *msg;

    if(NULL == deviceid)
    {
        if(NULL == s_oc_mqtt_profile_cb.device_id)
        {
            return ret;
        }
        else
        {
            deviceid = s_oc_mqtt_profile_cb.device_id;
        }
    }

    if((NULL== property) || (NULL== property->serviceId) || (NULL == property->serviceProperty))
    {
        return ret;
    }

    topic = topic_make(CN_OC_MQTT_PROFILE_PROPERTYREPORT_TOPICFMT, deviceid,NULL);
    msg = oc_mqtt_profile_package_propertyreport(property);

    if((NULL != topic) && (NULL != msg))
    {
        ret = oc_mqtt_publish(topic,(uint8_t *)msg,strlen(msg),(int)en_mqtt_al_qos_1);
    }
    else
    {
        ret = (int)EN_OC_MQTT_ERR_SYSMEM;
    }

    osal_free(topic);
    osal_free(msg);

    return ret;
}

#define CN_OC_MQTT_PROFILE_GWPROPERTYREPORT_TOPICFMT   "$oc/devices/%s/sys/gateway/sub_devices/properties/report"
int oc_mqtt_profile_gwpropertyreport(char *deviceid,oc_mqtt_profile_device_t *payload)
{
    int ret = (int)EN_OC_MQTT_ERR_PARAFMT;
    char *topic;
    char *msg;

    if(NULL == deviceid)
    {
        if(NULL == s_oc_mqtt_profile_cb.device_id)
        {
            return ret;
        }
        else
        {
            deviceid = s_oc_mqtt_profile_cb.device_id;
        }
    }

    if((NULL== payload) || (NULL == payload->subdevice_id)||(NULL== payload->subdevice_property) ||\
       (NULL== payload->subdevice_property->serviceId)||(NULL== payload->subdevice_property->serviceProperty))
    {
        return ret;
    }

    topic = topic_make(CN_OC_MQTT_PROFILE_GWPROPERTYREPORT_TOPICFMT, deviceid,NULL);
    msg = oc_mqtt_profile_package_gwpropertyreport(payload);

    if((NULL != topic) && (NULL != msg))
    {
        ret = oc_mqtt_publish(topic,(uint8_t *)msg,strlen(msg),(int)en_mqtt_al_qos_1);
    }
    else
    {
        ret = (int)EN_OC_MQTT_ERR_SYSMEM;
    }

    osal_free(topic);
    osal_free(msg);

    return ret;
}


#define CN_OC_MQTT_PROFILE_ROPERTYSETRESP_TOPICFMT   "$oc/devices/%s/sys/properties/set/response/request_id=%s"
int OcMqttPropertiesSetResponse(char *deviceid,OcMqttPropertiesSetResponseT *resp)
{
    int ret = (int)EN_OC_MQTT_ERR_PARAFMT;
    char *topic;
    char *msg;

    if(NULL == deviceid)
    {
        if(NULL == s_oc_mqtt_profile_cb.device_id)
        {
            return ret;
        }
        else
        {
            deviceid = s_oc_mqtt_profile_cb.device_id;
        }
    }

    if((NULL == resp) || (NULL == resp->requestId))
    {
        return ret;
    }
    topic = topic_make(CN_OC_MQTT_PROFILE_ROPERTYSETRESP_TOPICFMT, deviceid,resp->requestId);
    msg = oc_mqtt_profile_package_propertysetresp(resp);

    if((NULL != topic) && (NULL != msg))
    {
        ret = oc_mqtt_publish(topic,(uint8_t *)msg,strlen(msg),(int)en_mqtt_al_qos_1);
    }
    else
    {
        ret = (int)EN_OC_MQTT_ERR_SYSMEM;
    }

    osal_free(topic);
    osal_free(msg);

    return ret;
}


#define CN_OC_MQTT_PROFILE_ROPERTYGETRESP_TOPICFMT   "$oc/devices/%s/sys/properties/get/response/request_id=%s"
int OcMqttPropertiesGetResponse(char *deviceid,OcMqttPropertiesGetResponseT *resp)
{
    int ret = (int)EN_OC_MQTT_ERR_PARAFMT;
    char *topic;
    char *msg;

    if(NULL == deviceid)
    {
        if(NULL == s_oc_mqtt_profile_cb.device_id)
        {
            return ret;
        }
        else
        {
            deviceid = s_oc_mqtt_profile_cb.device_id;
        }
    }

    if((NULL== resp) || (NULL == resp->requestId) || \
       (NULL== resp->services->serviceId) || (NULL == resp->services->serviceProperty))
    {
        return ret;
    }

    topic = topic_make(CN_OC_MQTT_PROFILE_ROPERTYGETRESP_TOPICFMT, deviceid,resp->requestId);
    msg = oc_mqtt_profile_package_propertygetresp(resp);

    if((NULL != topic) && (NULL != msg))
    {
        ret = oc_mqtt_publish(topic,(uint8_t *)msg,strlen(msg),(int)en_mqtt_al_qos_1);
    }
    else
    {
        ret = (int)EN_OC_MQTT_ERR_SYSMEM;
    }

    osal_free(topic);
    osal_free(msg);

    return ret;
}

#define CN_OC_MQTT_PROFILE_CMDRESP_TOPICFMT   "$oc/devices/%s/sys/commands/response/request_id=%s"
int OcMqttCommandResponse(char *deviceId,OcMqttCommandResponseT *cmdResp)
{
    int ret = (int)EN_OC_MQTT_ERR_PARAFMT;
    char *topic;
    char *msg;

    if(NULL == deviceId)
    {
        if(NULL == s_oc_mqtt_profile_cb.device_id)
        {
            return ret;
        }
        else
        {
            deviceId = s_oc_mqtt_profile_cb.device_id;
        }
    }

    if((NULL == cmdResp) || (NULL == cmdResp->requestId))
    {
        return ret;
    }

    topic = topic_make(CN_OC_MQTT_PROFILE_CMDRESP_TOPICFMT, deviceId,cmdResp->requestId);
    msg = oc_mqtt_profile_package_cmdresp(cmdResp);

    if((NULL != topic) && (NULL != msg))
    {
        ret = oc_mqtt_publish(topic,(uint8_t *)msg,strlen(msg),(int)en_mqtt_al_qos_1);
    }
    else
    {
        ret = (int)EN_OC_MQTT_ERR_SYSMEM;
    }

    osal_free(topic);
    osal_free(msg);

    return ret;
}

#define CN_OC_MQTT_PROFILE_GETSHADOW_TOPICFMT   "$oc/devices/%s/sys/shadow/get/request_id=%s"
int OcMqttGetDeviceShadow(char *deviceId,OcMqttGetDeviceShadowT *shadow)
{
    int ret = (int)EN_OC_MQTT_ERR_PARAFMT;
    char *topic;
    char *msg;

    if(NULL == deviceId)
    {
        if(NULL == s_oc_mqtt_profile_cb.device_id)
        {
            return ret;
        }
        else
        {
            deviceId = s_oc_mqtt_profile_cb.device_id;
        }
    }

    if((NULL == shadow) || (NULL == shadow->requestId))
    {
        return ret;
    }

    topic = topic_make(CN_OC_MQTT_PROFILE_GETSHADOW_TOPICFMT, deviceId,shadow->requestId);
    msg = oc_mqtt_profile_package_shadowget(shadow);

    if((NULL != topic) && (NULL != msg))
    {
        ret = oc_mqtt_publish(topic,(uint8_t *)msg,strlen(msg),(int)en_mqtt_al_qos_1);
    }
    else
    {
        ret = (int)EN_OC_MQTT_ERR_SYSMEM;
    }

    osal_free(topic);
    osal_free(msg);

    return ret;
}


#define CN_OC_MQTT_PROFILE_EVENT_TOPICFMT   "$oc/devices/%s/sys/events/up"
int OcMqttEventReport(char *deviceid,OcMqttEventT *event)
{
    int ret = (int)EN_OC_MQTT_ERR_PARAFMT;
    char *topic;
    char *msg;

    if(NULL == deviceid){
        if(NULL == s_oc_mqtt_profile_cb.device_id){
            return ret;
        }
        else{
            deviceid = s_oc_mqtt_profile_cb.device_id;
        }
    }

    if((event == NULL) || (event->eventType == NULL)){
        return ret;
    }

    topic = topic_make(CN_OC_MQTT_PROFILE_EVENT_TOPICFMT, deviceid,NULL);
    msg = oc_mqtt_profile_package_event(event);

    if((topic != NULL) && (msg != NULL)){
        ret = oc_mqtt_publish(topic,(uint8_t *)msg,strlen(msg),(int)en_mqtt_al_qos_1);
    }
    else{
        ret = (int)EN_OC_MQTT_ERR_SYSMEM;
    }

    osal_free(topic);
    osal_free(msg);

    return ret;
}





