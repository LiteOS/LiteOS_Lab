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

#ifndef LITEOS_LAB_IOT_LINK_OC_OC_MQTT_OC_MQTT_PROFILE_OC_MQTT_PROFILE_H_
#define LITEOS_LAB_IOT_LINK_OC_OC_MQTT_OC_MQTT_PROFILE_OC_MQTT_PROFILE_H_

#include <stdint.h>
#include "oc_mqtt_al.h"

///< define all the json key will be used in the profile
#define CN_OC_JSON_KEY_SERVICEID        "service_id"
#define CN_OC_JSON_KEY_OBJECTDEVICEID   "object_device_id"
#define CN_OC_JSON_KEY_NAME             "name"
#define CN_OC_JSON_KEY_ID               "id"
#define CN_OC_JSON_KEY_CONTENT          "content"
#define CN_OC_JSON_KEY_PROPERTIES       "properties"
#define CN_OC_JSON_KEY_EVENTTIME        "event_time"
#define CN_OC_JSON_KEY_EVENTTYPE        "event_type"
#define CN_OC_JSON_KEY_DEVICES          "devices"
#define CN_OC_JSON_KEY_DEVICEID         "device_id"
#define CN_OC_JSON_KEY_RESULTCODE       "result_code"
#define CN_OC_JSON_KEY_RESPONSENAME     "response_name"
#define CN_OC_JSON_KEY_PARAS            "paras"
#define CN_OC_JSON_KEY_RESULTDESC       "result_desc"
#define CN_OC_JSON_KEY_SERVICES         "services"
#define CN_OC_JSON_KEY_SWVERSION        "sw_version"
#define CN_OC_JSON_KEY_FWVERSION        "fw_version"
#define CN_OC_JSON_KEY_VERSION          "version"
#define CN_OC_JSON_KEY_PROGRESS         "progress"


///< UP means the device send data to the cloud
typedef enum
{
    EN_OC_MQTT_PROFILE_MSG_TYPE_UP_MSGUP = 0,            ///< DEVICE REPORT MESSAG DIRECTLY
    EN_OC_MQTT_PROFILE_MSG_TYPE_UP_PROPERTYREPORT,       ///< DEVICE REPORT PROPERTY
    EN_OC_MQTT_PROFILE_MSG_TYPE_UP_SUBPROPERTYREPORT,    ///< GATWAY REPORT SUBDEVICEPROPERTY
    EN_OC_MQTT_PROFILE_MSG_TYPE_UP_PROPERTYSETRESPONSE,  ///< RESPONSE TO THE PROPERTY SET
    EN_OC_MQTT_PROFILE_MSG_TYPE_UP_PROPERTYGETRESPONSE,  ///< RESPONSE TO THE PROPERTY GET
    EN_OC_MQTT_PROFILE_MSG_TYPE_UP_CMDRESPONSE,          ///< RESPONSE TO THE COMMAND
    EN_OC_MQTT_PROFILE_MSG_TYPE_UP_LAST,
}en_oc_mqtt_profile_msg_type_up_t;


///< DOWN means the cloud send data to the device
typedef enum
{
    EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_MSGDOWN = 0,        ///< THIS IS THE MESSAGE DOWN
    EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_COMMANDS,           ///< THIS IS THE PLATFORM COMMANDS
    EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_PROPERTYSET,        ///< THIS IS THE PLATFORM SET PROPERTY
    EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_PROPERTYGET,        ///< THIS IS THE PLATFORM GET PROPERTY
    EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_EVENT,              ///< THIS IS THE EVENT
    EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_BS,                 ///< THIS IS THE BOOTSTRAP
    EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_SHADOWGET,          ///< THIS IS THE GET SHADOW
    EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_LAST = 255,         ///< NOT DEFINED
}EnOcMqttMsgTypeDownT;


////< enum all the data type for the oc profile
typedef enum
{
    EN_OC_MQTT_PROFILE_VALUE_INT = 0,
    EN_OC_MQTT_PROFILE_VALUE_LONG,
    EN_OC_MQTT_PROFILE_VALUE_FLOAT,
    EN_OC_MQTT_PROFILE_VALUE_DOUBLE,
    EN_OC_MQTT_PROFILE_VALUE_STRING,           ///< must be ended with '\0'
    EN_OC_MQTT_PROFILE_VALUE_INT_ARRAY,
    EN_OC_MQTT_PROFILE_VALUE_FLOAT_ARRAY,
    EN_OC_MQTT_PROFILE_VALUE_DOUBLE_ARRAY,
    EN_OC_MQTT_PROFILE_VALUE_STRING_ARRAY,
    EN_OC_MQTT_PROFILE_VALUE_KV_T,
    EN_OC_MQTT_PROFILE_VALUE_LAST = 255,   // not defined
}EnOcDataT;


typedef struct
{
    void                 *nxt;   ///< ponit to the next key
    char                 *key;
    EnOcDataT             type;
    void                 *value;
    int                   arraySize;
}OcMqttKvT;


typedef struct
{
    EnOcMqttMsgTypeDownT type;       ///< defined as EnOcMqttMsgTypeDownT
    char *requestId;                              ///< get from the topic, NULL if not supplied
    void *msgContent;                                     ///< the send from the cloud platform
    int   msgLen;                                 ///< the message length
}OcMqttMsgRcvT;

/**
 * @brief: this the message from the cloud call back function
 *
 * @param[in] message:defined as OcMqttMsgRcvT
 *
 * @return: not used yet
 *
 * */
typedef int (*FnOcMqttRcvDeal)(OcMqttMsgRcvT *message);

typedef struct
{
    int boostrap;              ///< we use the bootstrap mode or not

    int lifeTime;             ///< ping echo cycle
    char             *serverAddr;
    char             *serverPort;
    char             *deviceId;
    char             *devicePasswd;
    char             *scopeId;
    TlsAlSecurityT    security;

    FnOcMqttRcvDeal   rcvFunc;
    FnOcMqttLog       logFunc;

}OcMqttConnectT;

/**
 * @brief: use this function to connect to the cloud
 *
 * @param[in] connectPara: defined as OcMqttConnectT
 *
 * @return :defined as EnOcMqttErrCodeT
 *
 * */
int OcMqttConnect(OcMqttConnectT *connectPara);

/**
 * @brief: use this function to disconnect to the cloud
 *
 * @return :defined as EnOcMqttErrCodeT
 *
 * */
int OcMqttDisconnect(void);

typedef struct
{
    char *deviceId;   ///< you could specify it,if NULL,then use the device_id which used to connect
    char *msgName;        ///< message name, could be NULL
    char *msgId;          ///< message id, could be NULL
    void *msgContent;         ///< message to send
    int   msgLen;     ///< message length
}OcMqttMessageReportT;


/**
 * @brief: use this function to send data to the platform without profile decode
 *
 * @param[in] deviceId: the cloud message receiver, if NULL then send to the connected one
 *
 * @param[in] message: defined as OcMqttMessageReportT
 *
 * @return :defined as EnOcMqttErrCodeT
 *
 * */
int OcMqttMessageReport(char *deviceId,OcMqttMessageReportT *message);


typedef struct
{
   void *nxt;
   char *serviceId;                         ///< the service id in the profile, which could not be NULL
   char *eventTime;                         ///< eventtime, which could be NULL means use the platform time
   OcMqttKvT *serviceProperty;   ///< the property in the profile, which could not be NULL
}OcMqttServiceT;

/**
 * @brief: use this function to send data to the platform with profile decode
 *
 * @param[in] deviceid: the cloud message receiver, if NULL then send to the connected one
 *
 * @param[in] payload: properties list to send to the platform
 *
 * @return :defined as EnOcMqttErrCodeT
 *
 * */
int OcMqttPropertiesReport(char *deviceId, OcMqttServiceT *property);

typedef struct
{
    void *nxt;                                                  ///< maybe much more
    char                                *subdevice_id;          ///< the specified device, which could not be NULL
    OcMqttServiceT           *subdevice_property;    ///< the property of the specified device, which could not be NULL
}oc_mqtt_profile_device_t;

/**
 * @brief: use this function to send data to the platform with profile decode
 *
 * @param[in] deviceid: the cloud message receiver, if NULL then send to the connected one
 *
 * @param[in] payload:defined as oc_mqtt_profile_device_t
 *
 * @return :defined as EnOcMqttErrCodeT
 *
 * */
int oc_mqtt_profile_gwpropertyreport(char *deviceid,oc_mqtt_profile_device_t *payload);


typedef struct
{
    int     retCode;           ///< response code, 0 success while others failed
    char   *retDescription;    ///< response description,maybe used when failed
    char   *requestId;         ///< specified by the setproperty command
}OcMqttPropertiesSetResponseT;
/**
 * @brief: use this function to send the response to the setproperty command
 *
 * @param[in] deviceId: the cloud message receiver, if NULL then send to the connected one
 *
 * @param[in] resp: description as OcMqttPropertiesSetResponseT
 *
 * @return :defined as EnOcMqttErrCodeT
 *
 * */
int OcMqttPropertiesSetResponse(char *deviceId,OcMqttPropertiesSetResponseT *resp);


typedef struct
{
    char *requestId;                              ///< specified by the getproperty command
    OcMqttServiceT  *services;          ///< defined as OcMqttServiceT
}OcMqttPropertiesGetResponseT;

/**
 * @brief: use this function to send the response to the setproperty command

 * @param[in] deviceId: the cloud message receiver, if NULL then send to the connected one

 * @param[in] resp: description as OcMqttPropertiesGetResponseT
 *
 * @return :defined as EnOcMqttErrCodeT
 *
 * */
int OcMqttPropertiesGetResponse(char *deviceId,OcMqttPropertiesGetResponseT *resp);


typedef struct
{
    int     retCode;           ///< response code, 0 success while others failed
    char   *retName;           ///< response description,maybe used when failed
    char   *requestId;         ///< specified by the message command
    OcMqttKvT  *paras;///< the command paras

}OcMqttCommandResponseT;
/**
 * @brief: use this function to send the response to the setproperty command
 *
 * @param[in] deviceId: the cloud message receiver, if NULL then send to the connected one
 *
 * @param[in] cmdResponse: description as OcMqttCommandResponseT
 *
 * @return :defined as EnOcMqttErrCodeT
 *
 * */
int OcMqttCommandResponse(char *deviceId,OcMqttCommandResponseT *cmdResponse);



typedef struct
{

    char   *objectDeviceId;           ///< target deice_id, optional
    char   *serviceId;                 ///< service_id, optional
    char   *requestId;                 ///< the request_id, must be supplied by the service
}OcMqttGetDeviceShadowT;
/**
 * @brief: use this function to send get the platform shadow data
 *
 * @param[in] deviceId: the device we registered
 *
 * @param[in] shadow: description as oc_mqtt_profile_p_t
 *
 * @return :defined as EnOcMqttErrCodeT
 *
 * */
int OcMqttGetDeviceShadow(char *deviceId,OcMqttGetDeviceShadowT *shadow);


#define CN_OC_MQTT_EVENTTYPE_VERSIONQUERY   "version_query"
#define CN_OC_MQTT_EVENTTYPE_VERSIONREPORT  "version_report"
#define CN_OC_MQTT_EVENTTYPE_FWUPGRADE      "firmware_upgrade"
#define CN_OC_MQTT_EVENTTYPE_SWUPGRADE      "software_upgrade"
#define CN_OC_MQTT_EVENTTYPE_UPGRADEPROGRESSREPORT "upgrade_progress_report"

typedef struct
{
    char *objectDeviceId;
    char *serviceId;
    char *eventType;
    char *eventTime;
    OcMqttKvT *paras;
}OcMqttEventT;

/**
 * @brief: use this fucntion to make the event
 *
 * @param:deviceid: the deviceID
 *
 * @param:event:defines as the OcMqttEventT
 *
 * @return: the json string of the event.
 *
 * */
int OcMqttEventReport(char *deviceId,OcMqttEventT *event);



#endif /* LITEOS_LAB_IOT_LINK_OC_OC_MQTT_OC_MQTT_PROFILE_OC_MQTT_PROFILE_H_ */
