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
 *  2019-05-05 19:15  zhangqianfu  The first version:
 *
 *  1, only support CRT tls mode
 *  2, the data encode only support the json mode
 *  3, the passwd only support the no check time mode
 *
 */

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <link_misc.h>

#include <osal.h>
#include <mqtt_al.h>
#include <oc_mqtt_al.h>
#include <sal.h>

#include <cJSON.h>           //json mode
#include "hmac.h"            //used to generate the user passwd
#include <atiny_mqtt.h>

#define VARIABLE_SIZE (4 + 1)
#define CN_DMP_DATA_COMMAND_FMT             "/huawei/v1/devices/%s/command/%s"
#define CN_DMP_DATA_REPORT_FMT              "/huawei/v1/devices/%s/data/%s"

#define cn_secret_notify_topic_fmt          "/huawei/v1/products/%s/sn/%s/secretNotify"
#define cn_secret_ack_topic_fmt             "/huawei/v1/products/%s/sn/%s/secretACK"

#define CN_BS_DATA_REPORT_FMT               "/huawei/v1/devices/%s/iodpsData"
#define CN_BS_DATA_COMMAND_FMT              "/huawei/v1/devices/%s/iodpsCommand"


/*The unit is millisecond*/
#define CN_CON_BACKOFF_TIME     (1000)   ///< UNIT:ms
#define CN_CON_BACKOFF_MAXTIMES  6       ///< WHEN WAIT OR GET ADDRESS ,WE COULD TRY MAX TIMES

#define CN_STRING_MAXLEN    127
#define IS_VALID_STRING(name) (strnlen((name), CN_STRING_MAXLEN + 1) <= CN_STRING_MAXLEN)

///< NOW WE ONLY SUPPORT IPV4,THE LENGTH IS 16, NEXT TIME WE SUPPORT IPV6, AND MODIFY IT HERE--TODO
#define CN_SEVRER_IP_LEN   16
#define CN_SEVRER_PROT_LEN 6

typedef struct
{
    char       *dmp_address;  ///< we get from the bs server
    char       *dmp_port;     ///< we get from the bs server
}oc_bs_mqtt_cb_t; ///< when we used

typedef struct
{
    tag_oc_mqtt_config  config;
    oc_bs_mqtt_cb_t     bs_cb;
    unsigned int        b_flag_run:1;             ///< set when we begin if any configure start
    unsigned int        b_flag_stop:1;            ///< set when the engine really stoped
    unsigned int        b_flag_bs_run:1;          ///< set by the configure if we client initialized
    unsigned int        b_flag_bs_getaddr:1;      ///< set by the bs message dealer
    unsigned int        b_flag_dmp_run:1;         ///< which means we are in the keep stage, we will try all the time
    unsigned int        b_flag_dmp_connected:1;   ///< which means we have connected to the server,could only set once

    ///< the following used for the mqtt connect
    void         *mqtt_handle;
    char         *mqtt_clientid;
    char         *mqtt_user;
    char         *mqtt_passwd;
    char         *mqtt_subtopic;
    char         *mqtt_pubtopic;
    const char   *server_address;    ///< it maybe used as dmp or bs server ip address
    const char   *server_port;       ///< it maybe used as dmp or bs server port address
    fn_msg_dealer msg_dealer;        ///< call this function to deal with the message
    ///< we have the agent engine
    void        *engine;  ///< it is a task here
}oc_agent_mqtt_cb_t;   ///< i think we may only got one mqtt
static oc_agent_mqtt_cb_t *s_oc_agent_mqtt_cb;



#define cn_check_time_value                 "2018111517"
///< check the config parameters
static int check_clone_config_params(oc_agent_mqtt_cb_t *cb,tag_oc_mqtt_config *config)
{
    int ret = -1;
    if(NULL == config->server)
    {
        goto EXIT_ERR;
    }
    if(NULL == config->port)
    {
        goto EXIT_ERR;
    }

    if(en_mqtt_al_security_cas != config->security.type)
    {
        goto EXIT_ERR;
    }

    if(en_oc_mqtt_code_mode_json != config->code_mode)
    {
        goto EXIT_ERR;
    }

    if(en_mqtt_sign_type_hmacsha256_check_time_no != config->sign_type)
    {
        goto EXIT_ERR;
    }

    if(en_oc_mqtt_device_type_static == config->device_type)
    {
        if((NULL == config->device_info.s_device.deviceid) ||\
           (NULL == config->device_info.s_device.devicepasswd))
        {
            goto EXIT_ERR;
        }
    }
    else
    {
        if((NULL == config->device_info.d_device.productid) ||\
           (NULL == config->device_info.d_device.productpasswd) ||\
           (NULL == config->device_info.d_device.notetid) ||\
           (NULL == config->device_info.d_device.read_secret) ||\
           (NULL == config->device_info.d_device.write_secret))
        {
            goto EXIT_ERR;
        }
    }

    if((en_oc_mqtt_device_type_static == config->device_type) &&\
        (en_mqtt_auth_type_model == config->auth_type))
    {
        goto EXIT_ERR;
    }

    if((en_oc_mqtt_device_type_dynamic == config->device_type) &&\
        (en_mqtt_auth_type_model != config->auth_type))
    {
        goto EXIT_ERR;
    }

    memcpy(&cb->config,config,sizeof(tag_oc_mqtt_config));

    ret = 0;
    return ret;


EXIT_ERR:
    return ret;
}


#define CN_OC_MQTT_CLIENTID_STATIC_FMT     "%s_%d_%d_%s"      ///<  deviceid  connect_type check_time time
#define CN_OC_MQTT_CLIENTID_DYNAMIC_FMT    "%s_%s_%d_%d_%s"   ///<  deviceid  noteid connect_type check_time time

static const char *s_codec_mode[en_oc_mqtt_code_mode_max] = {"binary", "json"};
///< generate the client_id user passwd for the mqtt need
static int free_mqtt_para(oc_agent_mqtt_cb_t *cb)
{
    if(NULL != cb->mqtt_handle)
    {
        mqtt_al_disconnect(cb->mqtt_handle);
        cb->mqtt_handle = NULL;
    }
    if(NULL != cb->mqtt_clientid)
    {
        osal_free(cb->mqtt_clientid);
        cb->mqtt_clientid = NULL;
    }

    if(NULL != cb->mqtt_user)
    {
        osal_free(cb->mqtt_user);
        cb->mqtt_user = NULL;
    }

    if(NULL != cb->mqtt_passwd)
    {
        osal_free(cb->mqtt_passwd);
        cb->mqtt_passwd = NULL;
    }

    if(NULL != cb->mqtt_subtopic)
    {
        osal_free(cb->mqtt_subtopic);
        cb->mqtt_subtopic = NULL;
    }

    if(NULL != cb->mqtt_pubtopic)
    {
        osal_free(cb->mqtt_pubtopic);
        cb->mqtt_pubtopic = NULL;
    }

    cb->server_address = NULL;
    cb->server_port = NULL;


    return 0;
}

static void __deal_dmp_msg(void *arg,mqtt_al_msgrcv_t  *msg)
{
    cJSON  *root = NULL;
    cJSON  *cmd_item = NULL;
    cJSON  *serviceid_item = NULL;
    char   *json_buf;
    int     reboot_servceid_match = 0;
    int     reboot_cmd_match = 0;
    int     mqtt_rebootstrap = 0;

    //for we must add the '/0' to the end to make sure the json parse correct
    if ((msg == NULL) || ( msg->msg.data == NULL) || (NULL == s_oc_agent_mqtt_cb->config.msgdealer))
    {
        return;
    }

    json_buf = osal_malloc(msg->msg.len + 1);
    if(NULL != json_buf)
    {
        memcpy(json_buf,msg->msg.data,msg->msg.len);
        json_buf[msg->msg.len] = '\0';
        root = cJSON_Parse(json_buf);
        if(NULL != root)
        {
            cmd_item = cJSON_GetObjectItem(root,"cmd");
            if((NULL != cmd_item) && (strncmp(cmd_item->valuestring, "BootstrapRequestTrigger", strlen(cmd_item->valuestring)) == 0))
            {
                reboot_cmd_match =1;
            }
            serviceid_item = cJSON_GetObjectItem(root,"serviceid");
            if((NULL != serviceid_item) && (strncmp(serviceid_item->valuestring, "IOTHUB.BS", strlen(serviceid_item->valuestring)) == 0))
            {
                reboot_servceid_match = 1;
            }
            if(reboot_cmd_match && reboot_servceid_match)
            {
                mqtt_rebootstrap = 1;
            }

            cJSON_Delete(root);
        }
        osal_free(json_buf);
    }

    if(mqtt_rebootstrap) ///< we reboot it now
    {
        printf("THIS IS THE OC MQTT REBOOTSTRAP COMMAND\n\r");
        s_oc_agent_mqtt_cb->b_flag_dmp_run = 0;
        return;
    }
	
    if(NULL != s_oc_agent_mqtt_cb->config.msgdealer)
    {
        s_oc_agent_mqtt_cb->config.msgdealer(s_oc_agent_mqtt_cb,msg);
    }

    return;
}

static int __generate_dmp_para(oc_agent_mqtt_cb_t *cb)
{
    int ret = -1;
    int len;
    int i;
    unsigned char hmac[32];
    char  *time_value;
    char  *passwd;

    time_value = cn_check_time_value;
    free_mqtt_para(cb);  ///< try to free all the resource we have built

    if(cb->config.device_type == en_oc_mqtt_device_type_static)   ///< static device mode
    {
        len = strlen(cb->config.device_info.s_device.deviceid)  + \
              strlen(time_value) + strlen(CN_OC_MQTT_CLIENTID_STATIC_FMT) + 1;
        cb->mqtt_clientid = osal_malloc(len);
        if (cb->mqtt_clientid != NULL)
        {
            snprintf(cb->mqtt_clientid,len,CN_OC_MQTT_CLIENTID_STATIC_FMT,cb->config.device_info.s_device.deviceid,\
                    cb->config.auth_type,cb->config.sign_type,time_value);
        }
        else
        {
            goto EXIT_MALLOC;
        }

        len = strlen(cb->config.device_info.s_device.deviceid) +1;
        cb->mqtt_user = osal_malloc(len);
        if (cb->mqtt_user != NULL)
        {
            memcpy(cb->mqtt_user,cb->config.device_info.s_device.deviceid,len);
        }
        else
        {
            goto EXIT_MALLOC;
        }

        len = sizeof(hmac) * 2 + 1;
        passwd = (char *)cb->config.device_info.s_device.devicepasswd;
        cb->mqtt_passwd = osal_malloc(len);
        if(NULL != cb->mqtt_passwd)
        {
            hmac_generate_passwd(passwd,strlen(passwd),time_value,strlen(time_value),\
                                 hmac,sizeof(hmac));

            for(i = 0; i < sizeof(hmac); i++)
            {
                snprintf(cb->mqtt_passwd + i * 2, 3, "%02x", hmac[i]);
            }
        }
        else
        {
            goto EXIT_MALLOC;
        }

        len = strnlen(cb->config.device_info.s_device.deviceid, CN_STRING_MAXLEN) +\
                strnlen(s_codec_mode[cb->config.code_mode], CN_STRING_MAXLEN) +\
                strlen(CN_DMP_DATA_COMMAND_FMT) + 1;
        cb->mqtt_subtopic = osal_malloc(len);
        if(NULL != cb->mqtt_subtopic)
        {
            snprintf(cb->mqtt_subtopic, len, CN_DMP_DATA_COMMAND_FMT, \
                    cb->config.device_info.s_device.deviceid, s_codec_mode[cb->config.code_mode]);
        }
        else
        {
            goto EXIT_MALLOC;
        }


        len = strnlen(cb->config.device_info.s_device.deviceid, CN_STRING_MAXLEN) +\
                strnlen(s_codec_mode[cb->config.code_mode], CN_STRING_MAXLEN) +\
                strlen(CN_DMP_DATA_REPORT_FMT) + 1;
        cb->mqtt_pubtopic = osal_malloc(len);
        if(NULL != cb->mqtt_pubtopic)
        {
            snprintf(cb->mqtt_pubtopic, len, CN_DMP_DATA_REPORT_FMT, \
                    cb->config.device_info.s_device.deviceid, s_codec_mode[cb->config.code_mode]);
        }
        else
        {
            goto EXIT_MALLOC;
        }

        if(cb->config.boot_mode == en_oc_boot_strap_mode_client_initialize)
        {
            cb->server_address = cb->bs_cb.dmp_address;
            cb->server_port = cb->bs_cb.dmp_port;
        }
        else
        {
            cb->server_address = cb->config.server;
            cb->server_port = cb->config.port;
        }

        cb->msg_dealer = __deal_dmp_msg;
        ret = 0;

    }
    else
    {
        ret = -1;
    }

    return ret;

EXIT_MALLOC:
    free_mqtt_para(cb);
    return ret;
}


static void __deal_bs_msg(void *handle,mqtt_al_msgrcv_t *msg)
{
    cJSON  *root = NULL;
    cJSON  *addr_item = NULL;
    char   *port = NULL;
    int     port_len;
    int     server_len;
    char   *server = NULL;

    char   *json_buf;

    printf("bs topic:%s qos:%d\n\r",msg->topic.data,msg->qos);

    json_buf = osal_malloc(msg->msg.len + 1);
    if(NULL == json_buf)
    {
        return;
    }

    memcpy(json_buf,msg->msg.data,msg->msg.len);
    json_buf[msg->msg.len] = '\0';

    printf("msg:%s\n\r",json_buf);
    root = cJSON_Parse(json_buf);
    if(NULL != root)
    {
        addr_item = cJSON_GetObjectItem(root,"address");
        if(NULL != addr_item)
        {
            printf("address:%s\n\r", addr_item->valuestring);
            port = strrchr(addr_item->valuestring, ':');
            if(NULL != port)
            {
                server = addr_item->valuestring;
                server_len = port - addr_item->valuestring;
                port++;
                port_len = strlen(port);
                if(NULL != s_oc_agent_mqtt_cb->bs_cb.dmp_port)
                {
                    osal_free (s_oc_agent_mqtt_cb->bs_cb.dmp_port);
                    s_oc_agent_mqtt_cb->bs_cb.dmp_port = NULL;
                }

                if(NULL != s_oc_agent_mqtt_cb->bs_cb.dmp_address)
                {
                    osal_free (s_oc_agent_mqtt_cb->bs_cb.dmp_address);
                    s_oc_agent_mqtt_cb->bs_cb.dmp_address = NULL;
                }


                s_oc_agent_mqtt_cb->bs_cb.dmp_address = osal_malloc(server_len +1);
                s_oc_agent_mqtt_cb->bs_cb.dmp_port = osal_malloc(port_len + 1);

                if((NULL != s_oc_agent_mqtt_cb->bs_cb.dmp_address) && \
                   (NULL != s_oc_agent_mqtt_cb->bs_cb.dmp_port))
                {

                    memcpy(s_oc_agent_mqtt_cb->bs_cb.dmp_port, port, port_len);
                    memcpy(s_oc_agent_mqtt_cb->bs_cb.dmp_address, server, server_len);
                    s_oc_agent_mqtt_cb->bs_cb.dmp_port[port_len] = '\0';
                    s_oc_agent_mqtt_cb->bs_cb.dmp_address[server_len] = '\0';

                    ///< set the flag to make the flag
                    s_oc_agent_mqtt_cb->b_flag_bs_getaddr = 1;
                    s_oc_agent_mqtt_cb->b_flag_bs_run = 0; ///< we could stop it now

                }
                else
                {
                    osal_free (s_oc_agent_mqtt_cb->bs_cb.dmp_address);
                    s_oc_agent_mqtt_cb->bs_cb.dmp_address = NULL;
                    osal_free (s_oc_agent_mqtt_cb->bs_cb.dmp_port);
                    s_oc_agent_mqtt_cb->bs_cb.dmp_port = NULL;
                }
            }
        }
        cJSON_Delete(root);
    }
    osal_free(json_buf);

    return;
}

static int __generate_bs_para(oc_agent_mqtt_cb_t *cb)
{
    int ret = -1;
    int len;
    int i;
    unsigned char hmac[32];
    char  *time_value;
    char  *passwd;

    time_value = cn_check_time_value;
    free_mqtt_para(cb);

    len = strlen(cb->config.device_info.s_device.deviceid)  + \
          strlen(time_value) + strlen(CN_OC_MQTT_CLIENTID_STATIC_FMT) + 1;
    cb->mqtt_clientid = osal_malloc(len);
    if (cb->mqtt_clientid != NULL)
    {
        snprintf(cb->mqtt_clientid,len,CN_OC_MQTT_CLIENTID_STATIC_FMT,cb->config.device_info.s_device.deviceid,\
                cb->config.auth_type,cb->config.sign_type,time_value);
    }
    else
    {
        goto EXIT_MALLOC;
    }

    len = strlen(cb->config.device_info.s_device.deviceid) +1;
    cb->mqtt_user = osal_malloc(len);
    if (cb->mqtt_user != NULL)
    {
        memcpy(cb->mqtt_user,cb->config.device_info.s_device.deviceid,len);
    }
    else
    {
        goto EXIT_MALLOC;
    }

    len = sizeof(hmac) * 2 + 1;
    passwd = (char *)cb->config.device_info.s_device.devicepasswd;
    cb->mqtt_passwd = osal_malloc(len);
    if(NULL != cb->mqtt_passwd)
    {
        if(0 == hmac_generate_passwd(passwd,strlen(passwd),time_value,strlen(time_value),\
                             hmac,sizeof(hmac)))
        {
            for(i = 0; i < sizeof(hmac); i++)
            {
                snprintf(cb->mqtt_passwd + i * 2, 3, "%02x", hmac[i]);
            }
        }
    }
    else
    {
        goto EXIT_MALLOC;
    }

    len = strnlen(cb->config.device_info.s_device.deviceid, CN_STRING_MAXLEN) +\
            strlen(CN_BS_DATA_COMMAND_FMT) + 1;
    cb->mqtt_subtopic = osal_malloc(len);
    if(NULL != cb->mqtt_subtopic)
    {
        snprintf(cb->mqtt_subtopic, len, CN_BS_DATA_COMMAND_FMT, \
                cb->config.device_info.s_device.deviceid);
    }
    else
    {
        goto EXIT_MALLOC;
    }


    len = strnlen(cb->config.device_info.s_device.deviceid, CN_STRING_MAXLEN) +\
            strlen(CN_BS_DATA_REPORT_FMT) + 1;
    cb->mqtt_pubtopic = osal_malloc(len);
    if(NULL != cb->mqtt_pubtopic)
    {
        snprintf(cb->mqtt_pubtopic, len, CN_BS_DATA_REPORT_FMT, \
                cb->config.device_info.s_device.deviceid);
    }
    else
    {
        goto EXIT_MALLOC;
    }

    cb->server_address = cb->config.server;
    cb->server_port = cb->config.port;
    cb->msg_dealer = __deal_bs_msg;

    return 0;


EXIT_MALLOC:
    free_mqtt_para(cb);
    return ret;
}

///< return the reason code defined by the mqtt_al.h
static int __oc_mqtt_connect_server(oc_agent_mqtt_cb_t *cb)
{
    mqtt_al_conpara_t conpara;

    //GENERATE THE DATA THE CLOUD PLATFORM NEED
    memset(&conpara,0,sizeof(conpara));

    conpara.clientid.data = cb->mqtt_clientid;
    conpara.clientid.len = strlen(conpara.clientid.data);

    conpara.user.data =  cb->mqtt_user;
    conpara.user.len = strlen(conpara.user.data);

    conpara.passwd.data = cb->mqtt_passwd;
    conpara.passwd.len = strlen(conpara.passwd.data);

    conpara.cleansession = 1;
    conpara.keepalivetime = cb->config.lifetime;
    conpara.security = &cb->config.security;

    conpara.serveraddr.data = (char *)cb->server_address;
    conpara.serveraddr.len = strlen(conpara.serveraddr.data);
    conpara.serverport = atoi(cb->server_port);
    conpara.timeout = 10000;
    conpara.version = en_mqtt_al_version_3_1_1;
    conpara.willmsg = NULL;

    printf("SERVER:%s \r\n PORT:%d \r\n",\
            conpara.serveraddr.data,conpara.serverport);
    printf("CLIENTID:%s \r\n USER:%s \r\n PASSWD:%s \r\n",\
            conpara.clientid.data,conpara.user.data,conpara.passwd.data);

    cb->mqtt_handle = mqtt_al_connect(&conpara);
    return conpara.conret;
}

static int subscribe_topic(oc_agent_mqtt_cb_t *cb)
{
    int ret = -1;
    mqtt_al_subpara_t   subpara;

    memset(&subpara,0,sizeof(subpara));

    subpara.dealer = cb->msg_dealer;
    subpara.arg = cb;
    subpara.qos = en_mqtt_al_qos_1;
    subpara.topic.data = cb->mqtt_subtopic ;
    subpara.topic.len = strlen(cb->mqtt_subtopic);

    ret = mqtt_al_subscribe(cb->mqtt_handle,&subpara);

    return ret;
}

///< this function used for the ocean mqtt dmp keep alive stage
///< we will try all the time
static int __oc_agent_mqtt_dmp_alive_stage(oc_agent_mqtt_cb_t  *cb)
{
    int conn_failed_cnt = 0;
    int ret = -1;

    cb->b_flag_dmp_connected = 0;
    ///< b_flag_dmp_run set by the bs or by the static config or by the bs mode
    while(cb->b_flag_dmp_run && cb->b_flag_run)
    {
        osal_task_sleep(CN_CON_BACKOFF_TIME << conn_failed_cnt);
        if(-1 == __generate_dmp_para(cb))
        {
            conn_failed_cnt = conn_failed_cnt >= CN_CON_BACKOFF_MAXTIMES?CN_CON_BACKOFF_MAXTIMES:(conn_failed_cnt+1);
            continue; ///< generate all the parameters needed by the ocean connect server
        }

        if(NULL != cb->mqtt_handle)
        {
            mqtt_al_disconnect(cb->mqtt_handle);
            cb->mqtt_handle = NULL;
        }

        if(cn_mqtt_al_con_code_ok != __oc_mqtt_connect_server(cb))
        {
            conn_failed_cnt = conn_failed_cnt >= CN_CON_BACKOFF_MAXTIMES?CN_CON_BACKOFF_MAXTIMES:(conn_failed_cnt+1);
            continue; ///< generate all the parameters needed by the ocean connect server
        }
        ret = subscribe_topic(cb);
        if(ret == -1)
        {
            mqtt_al_disconnect(cb->mqtt_handle);
            cb->mqtt_handle = NULL;
            conn_failed_cnt = conn_failed_cnt >= CN_CON_BACKOFF_MAXTIMES?CN_CON_BACKOFF_MAXTIMES:(conn_failed_cnt+1);
            continue; ///< generate all the parameters needed by the ocean connect server
        }
        cb->b_flag_dmp_connected = 1;
        conn_failed_cnt = 0;
        ///< we begin to loop to monitor the mqtt status or the api command
        while((en_mqtt_al_connect_ok == mqtt_al_check_status(cb->mqtt_handle)) &&\
                cb->b_flag_dmp_run && cb->b_flag_run)
        {
            osal_task_sleep(1000);
        }
        mqtt_al_disconnect(cb->mqtt_handle);
        cb->mqtt_handle = NULL;

        cb->b_flag_dmp_connected = 0;
    }
    ///< release all the resources
    free_mqtt_para(cb);
    return ret;
}



///< this function used for the ocean mqtt dmp keep alive stage
///< we will try all the time
static int __oc_agent_mqtt_bs_alive_stage(oc_agent_mqtt_cb_t  *cb)
{
    int conn_failed_cnt = 0;
    int ret = -1;

    cb->b_flag_bs_getaddr = 0;  ///< clear the flag

    ///< b_flag_dmp_run set by the bs or by the static config or by the bs mode
    while(cb->b_flag_bs_run && cb->b_flag_run)
    {
        osal_task_sleep(CN_CON_BACKOFF_TIME << conn_failed_cnt);
        if(-1 == __generate_bs_para(cb))
        {
            conn_failed_cnt = conn_failed_cnt >= CN_CON_BACKOFF_MAXTIMES?CN_CON_BACKOFF_MAXTIMES:(conn_failed_cnt+1);
            continue; ///< generate all the parameters needed by the ocean connect server
        }
        if(NULL != cb->mqtt_handle)
        {
            mqtt_al_disconnect(cb->mqtt_handle);
            cb->mqtt_handle = NULL;
        }


        if(cn_mqtt_al_con_code_ok != __oc_mqtt_connect_server(cb))
        {
            conn_failed_cnt = conn_failed_cnt >= CN_CON_BACKOFF_MAXTIMES?CN_CON_BACKOFF_MAXTIMES:(conn_failed_cnt+1);
            continue; ///< generate all the parameters needed by the ocean connect server
        }

        ret = subscribe_topic(cb);
        if(ret == -1)
        {
            mqtt_al_disconnect(cb->mqtt_handle);
            cb->mqtt_handle = NULL;

            conn_failed_cnt = conn_failed_cnt >= CN_CON_BACKOFF_MAXTIMES?CN_CON_BACKOFF_MAXTIMES:(conn_failed_cnt+1);
            continue; ///< generate all the parameters needed by the ocean connect server
        }

        ///< pub the mqtt request
        mqtt_al_pubpara_t pubpara;
        memset(&pubpara, 0, sizeof(pubpara));
        pubpara.qos = 1;
        pubpara.retain = 0;
        pubpara.timeout = 1000;
        pubpara.topic.data = cb->mqtt_pubtopic;
        pubpara.topic.len =strlen(cb->mqtt_pubtopic);
        pubpara.msg.data = '\0';
        pubpara.msg.len = 0;

        ret = mqtt_al_publish(cb->mqtt_handle, &pubpara);
        if(ret == -1)
        {
            mqtt_al_disconnect(cb->mqtt_handle);
            cb->mqtt_handle = NULL;

            conn_failed_cnt = conn_failed_cnt >= CN_CON_BACKOFF_MAXTIMES?CN_CON_BACKOFF_MAXTIMES:(conn_failed_cnt+1);
            continue; ///< generate all the parameters needed by the ocean connect server
        }

        conn_failed_cnt = 0;
        ///< we begin to loop to monitor the mqtt status or the api command
        while((en_mqtt_al_connect_ok == mqtt_al_check_status(cb->mqtt_handle)) &&\
                cb->b_flag_bs_run && cb->b_flag_run)
        {
            osal_task_sleep(1000);
        }
        mqtt_al_disconnect(cb->mqtt_handle);
        cb->mqtt_handle = NULL;

    }
    ///< release all the resources
    free_mqtt_para(cb);
    return ret;
}


static int __oc_agent_mqtt_engine(void  *arg)
{
    oc_agent_mqtt_cb_t  *cb;

    printf("%s:engine start\n\r",__FUNCTION__);

    cb = arg;
    while(1)
    {

        if(cb->b_flag_run)
        {
            cb->b_flag_stop =0;    ///< if we reach here, which means we need to

            if(cb->config.boot_mode == en_oc_boot_strap_mode_client_initialize)
            {
                cb->b_flag_bs_run = 1;
                __oc_agent_mqtt_bs_alive_stage(cb);
            }
            cb->b_flag_dmp_run = 1;
            __oc_agent_mqtt_dmp_alive_stage(cb);
            cb->b_flag_stop =1;

            ///< if we reach here, which means we need to try to
            printf("we need to do get the host addr again\n\r");
        }
        else
        {
            osal_task_sleep(1000);   ///< wait until if any connect request

            printf("oc mqtt is pending now\n\r");

        }

    }

    printf("%s:engine quit\n\r",__FUNCTION__);

    return 0;
}

///< the api call this function to configure the engine and wait for it connected and keepalive
static void *__oc_config(tag_oc_mqtt_config *config)
{
    oc_agent_mqtt_cb_t *ret = NULL;
    int try_times = 0;

    ///< check if we have initialized
    if(NULL == s_oc_agent_mqtt_cb)
    {
        return ret;
    }

    ///< check if we have already configured and not de_configured yet
    if(s_oc_agent_mqtt_cb->b_flag_run)
    {
        return ret;
    }

    if(0 != check_clone_config_params(s_oc_agent_mqtt_cb,config))
    {
        return ret;
    }

    s_oc_agent_mqtt_cb->b_flag_stop = 0;
    s_oc_agent_mqtt_cb->b_flag_run = 1;

    ///< here we run wait until we connected to the server
    while((++try_times <= 30) )  ///< we could wait max 30 seconds
    {
        osal_task_sleep(CN_CON_BACKOFF_TIME);  ///< do back off retreat

        if(s_oc_agent_mqtt_cb->b_flag_dmp_connected)
        {
            ret = s_oc_agent_mqtt_cb;
            break;
        }
    }

    if(NULL == ret)
    {
        s_oc_agent_mqtt_cb->b_flag_bs_getaddr = 0;
        s_oc_agent_mqtt_cb->b_flag_bs_run =0;
        s_oc_agent_mqtt_cb->b_flag_dmp_connected = 0;
        s_oc_agent_mqtt_cb->b_flag_dmp_run = 0;
        s_oc_agent_mqtt_cb->b_flag_stop =1;
        s_oc_agent_mqtt_cb->b_flag_run = 0;
        printf("*******we should quit now*************\n\r");
        osal_task_sleep(4*CN_CON_BACKOFF_TIME);  ///< do back off retreat
    }
    else
    {
        printf("*******connect success*************\n\r");
    }

    return ret;
}


///< the api call this function to send message to the dmp server
static int __oc_report(void *handle,char *msg, int msg_len,en_mqtt_al_qos_t qos)
{
    int ret = -1;
    size_t payloadlen;
    mqtt_al_pubpara_t  pubpara;

    oc_agent_mqtt_cb_t *cb = handle;

    if ((cb == NULL) || (cb != s_oc_agent_mqtt_cb)|| (qos >= en_mqtt_al_qos_err)||\
        (msg == NULL) || (msg_len <= 0))
    {
        return ret;
    }

    if (!(cb->b_flag_dmp_connected &&(en_mqtt_al_connect_ok == mqtt_al_check_status(cb->mqtt_handle))))
    {
        return ret;
    }

    payloadlen = msg_len;
    memset(&pubpara, 0, sizeof(pubpara));

    pubpara.qos = qos ;
    pubpara.retain = 0;
    pubpara.timeout = 1000;
    pubpara.topic.data = cb->mqtt_pubtopic;
    pubpara.topic.len =strlen(cb->mqtt_pubtopic);
    pubpara.msg.data =(char *)msg;
    pubpara.msg.len = payloadlen;

    ret = mqtt_al_publish(cb->mqtt_handle,&pubpara);

    return ret;
}

static int __oc_deconfig(void *handle)
{
    int ret =-1;
    oc_agent_mqtt_cb_t *cb = handle;

    if((NULL != cb ) && (cb == s_oc_agent_mqtt_cb) && (cb->b_flag_run))
    {
        cb->b_flag_run = 0;
        ///< wait until we really have stop it
        while(cb->b_flag_stop == 0)  ///< this will be set when we really stop it
        {
            osal_task_sleep(CN_CON_BACKOFF_TIME);
        }
        ret = 0;
    }

    return ret;
}

static const tag_oc_mqtt_ops s_oc_agent_mqtt_ops= \
{
    .config = __oc_config,
    .deconfig = __oc_deconfig,
    .report = __oc_report,
};

int oc_mqtt_install_atiny_mqtt()
{
    int ret = -1;

    oc_agent_mqtt_cb_t *cb;

    cb = osal_malloc(sizeof(oc_agent_mqtt_cb_t));
    if(NULL == cb)
    {
        goto EXIT_MALLOC;
    }
    memset(cb,0,sizeof(oc_agent_mqtt_cb_t));

    cb->b_flag_run = 0;
    cb->engine = osal_task_create("oc_mqtt_agent",__oc_agent_mqtt_engine,cb,0x1800,NULL,10);
    if(NULL == cb->engine)
    {
        goto EXIT_TASK;
    }

    ret = oc_mqtt_register(&s_oc_agent_mqtt_ops);
    if(0 != ret)
    {
        goto EXIT_REGISTER;
    }
    s_oc_agent_mqtt_cb = cb;
    return ret;


EXIT_REGISTER:
    osal_task_kill(cb->engine);
    cb->engine = NULL;

EXIT_TASK:
    osal_free(cb);
    cb = NULL;

EXIT_MALLOC:
    return ret;
}


