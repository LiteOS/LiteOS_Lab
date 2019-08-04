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
#define CN_BS_RCV_BUFLENTH 256


typedef enum
{
    en_oc_agent_mqtt_status_idle = 0,       ///< not config yet;
    en_oc_agent_mqtt_status_config,         ///< has been configured, have not connected yet
    en_oc_agent_mqtt_status_bs_connecting,  ///< doing the connecting
    en_oc_agent_mqtt_status_bs_connected,   ///< has connected to the bs server
    en_oc_agent_mqtt_status_bs_success,     ///< has get the address
    en_oc_agent_mqtt_status_bs_failed,      ///< has get the address
    en_oc_agent_mqtt_status_dmp_connecting, ///< doing the connnecting
    en_oc_agent_mqtt_status_dmp_connected,  ///< has connected to the dmp server
}en_oc_agent_mqtt_status_t;

typedef enum
{
    en_oc_boot_strap_status_bs = 0,
    en_oc_boot_strap_status_hub,
    en_oc_boot_strap_status_max,
}en_oc_boot_strap_status_t;

typedef struct
{
    void         *mqtt_handle;
    char         *mqtt_client_id;
    char         *mqtt_user;
    char         *mqtt_passwd;
    char         *mqtt_subtopic;
    char         *mqtt_pubtopic;
}oc_mqtt_t;

typedef struct
{
    oc_mqtt_t  mqtt;
}oc_dmp_mqtt_cb_t; ///< when we used

typedef struct
{
    char         dmp_ip[16];
    char         dmp_port[6];  ///< i think max is 65536, so string is at most 6 bytes
    char        rcv_buf[CN_BS_RCV_BUFLENTH];
    int         rcv_len;
}oc_bs_mqtt_cb_t; ///< when we used

typedef struct
{
    tag_oc_mqtt_config  config;
    en_oc_boot_strap_status_t boot_status;

    unsigned int        b_flag_update:1;          ///< when get device id and passwd should set it
    unsigned int        b_flag_get_passswd:1;     ///< this bit to be set when dynamic and received secret
    unsigned int        b_flag_connect_dynamic:1; ///< this bit to be set when dynamic and not get secret yet
    unsigned int        b_flag_rcv_snd:1;         ///< this bit to be set when could receive and send
    unsigned int        b_flag_stop:1;            ///< this bit to be set when stop the engine
    char               *get_device_id;            ///< when dynamic mode,this is received from the server
    char               *get_device_passwd;        ///< when dynamic mode,this is received from the server
    ///< the following used for the mqtt connect
    void         *mqtt_handle;
    char         *mqtt_client_id;
    char         *mqtt_user;
    char         *mqtt_passwd;
    char         *mqtt_subtopic;
    char         *mqtt_pubtopic;
    char         server_address[16];
    char         server_port[6];
    ///< we have two mode, bs mode or dmp mode
    ///< we have the agent engine
    void              *engine;  ///< it is a task here
    ///< reflect the connect status
    en_oc_agent_mqtt_status_t     constatus;
}oc_agent_mqtt_cb_t;   ///< i think we may only got one mqtt
static oc_agent_mqtt_cb_t *s_oc_agent_mqtt_cb;

/**
 * @brief: this is used to storage the dynamic message to the rom
 *
 * the fomat is following:
 *
 * 4Byte    4Byte  32Byte   JsonString
 *
 * Magic    Length  hmac    JsonString(the string length is Length)
 *
 * */

#define cn_secret_magic  0xaa55aa55
typedef struct
{
    uint32_t magic;
    uint32_t length;
    uint8_t  hmac[32];
}tag_oc_secret_rom;

#define cn_secret_index_product_id_name     "productid"
#define cn_secret_index_product_pass_name   "productname"
#define cn_secret_index_note_id_name        "noteid"
#define cn_secret_index_time_name           "time"
#define cn_secret_index_device_id_name      "deviceid"
#define cn_secret_index_device_pass_name    "secret"

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

    if(en_oc_mqtt_code_mode_json != config->code_mode )
    {
        goto EXIT_ERR;
    }

    if(en_mqtt_sign_type_hmacsha256_check_time_no != config->sign_type )
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

    cb->config = *config;
    ret = 0;
    return ret;


EXIT_ERR:
    return ret;
}


#define cn_client_id_fmt_static     "%s_%d_%d_%s"      ///<  deviceid  connect_type check_time time
#define cn_client_id_fmt_dynamic    "%s_%s_%d_%d_%s"   ///<  deviceid  noteid connect_type check_time time

static const char *s_codec_mode[en_oc_mqtt_code_mode_max] = {"binary", "json"};
///< generate the client_id user passwd for the mqtt need


static int free_mqtt_para(oc_agent_mqtt_cb_t *cb)
{
    if(NULL != cb->mqtt_handle)
    {
        mqtt_al_disconnect(cb->mqtt_handle);
        cb->mqtt_handle = NULL;
    }


    if(NULL != cb->mqtt_client_id)
    {
        osal_free(cb->mqtt_client_id);
        cb->mqtt_client_id = NULL;
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

    return 0;
}

static int gen_dmp_para(oc_agent_mqtt_cb_t *cb)
{
    int ret = -1;
    int len;
    int i;
    unsigned char hmac[32];
    char  *time_value;
    char  *passwd;

    time_value = cn_check_time_value;

    if(cb->config.device_type == en_oc_mqtt_device_type_static)   ///< static device mode
    {
        len = strlen(cb->config.device_info.s_device.deviceid)  + \
              strlen(time_value) + strlen(cn_client_id_fmt_static) + 1;
        cb->mqtt_client_id = osal_malloc(len);
        if (cb->mqtt_client_id != NULL)
        {
            snprintf(cb->mqtt_client_id,len,cn_client_id_fmt_static,cb->config.device_info.s_device.deviceid,\
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

static int gen_bs_para(oc_agent_mqtt_cb_t *cb)
{
    int ret = -1;
    int len;
    int i;
    unsigned char hmac[32];
    char  *time_value;
    char  *passwd;

    time_value = cn_check_time_value;

    len = strlen(cb->config.device_info.s_device.deviceid)  + \
          strlen(time_value) + strlen(cn_client_id_fmt_static) + 1;
    cb->mqtt_client_id = osal_malloc(len);
    if (cb->mqtt_client_id != NULL)
    {
        snprintf(cb->mqtt_client_id,len,cn_client_id_fmt_static,cb->config.device_info.s_device.deviceid,\
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


    return 0;


EXIT_MALLOC:
    free_mqtt_para(cb);
    return ret;
}

///< return the reason code defined by the mqtt_al.h
static int connect_server(oc_agent_mqtt_cb_t *cb)
{
    mqtt_al_conpara_t conpara;
    struct hostent* entry = NULL;

    //GENERATE THE DATA THE CLOUD PLATFORM NEED
    memset(&conpara,0,sizeof(conpara));

    conpara.clientid.data = cb->mqtt_client_id;
    conpara.clientid.len = strlen(conpara.clientid.data);

    conpara.user.data =  cb->mqtt_user;
    conpara.user.len = strlen(conpara.user.data);

    conpara.passwd.data = cb->mqtt_passwd;
    conpara.passwd.len = strlen(conpara.passwd.data);

    conpara.cleansession = 1;
    conpara.keepalivetime = cb->config.lifetime;
    conpara.security = &cb->config.security;

    while(NULL == entry)
    {
        printf("debug:%s\r\n", cb->config.server);
        entry = sal_gethostbyname(cb->config.server);
        if(entry && entry->h_addr_list[0])
        {
            inet_ntop(entry->h_addrtype, entry->h_addr_list[0], iot_server_ip, sizeof(iot_server_ip));
            printf("ip:%s\n", iot_server_ip);
            conpara.serveraddr.data = iot_server_ip;
        }
        else
        {
            printf("LOG ############NO IP ---TRY AGAIN\n\r");
            osal_task_sleep(1000);
        }
    }
    conpara.serveraddr.len = strlen(conpara.serveraddr.data);
    conpara.serverport = atoi(cb->config.port);
    conpara.timeout = 10000;
    conpara.version = en_mqtt_al_version_3_1_1;
    conpara.willmsg = NULL;
    printf("CLIENTID:%s \r\n USER:%s \r\n PASSWD:%s \r\n",\
            conpara.clientid.data,conpara.user.data,conpara.passwd.data);

    cb->mqtt_handle = mqtt_al_connect(&conpara);
    if(NULL != cb->mqtt_handle)
    {
        if(cb->b_flag_get_passswd || (cb->config.device_type == en_oc_mqtt_device_type_static))
        {
            cb->b_flag_rcv_snd =1;
        }
    }

    return conpara.conret;
}



static void app_msg_dealer(void *arg,mqtt_al_msgrcv_t  *msg)
{
    //for we must add the '/0' to the end to make sure the json parse correct
    if ((msg == NULL) || ( msg->msg.data == NULL))
    {
        return;
    }
    
    if(s_oc_agent_mqtt_cb->config.boot_status == en_oc_boot_strap_status_bs)
        s_oc_agent_mqtt_cb->config.bsinfo_dealer(s_oc_agent_mqtt_cb,msg);
    else
        s_oc_agent_mqtt_cb->config.msgdealer(s_oc_agent_mqtt_cb,msg);
}


static int subscribe_topic(oc_agent_mqtt_cb_t *cb)
{
    int ret = -1;
    mqtt_al_subpara_t   subpara;

    memset(&subpara,0,sizeof(subpara));

    if((cb->config.device_type == en_oc_mqtt_device_type_static)||\
       (cb->b_flag_get_passswd))
    {
        subpara.dealer = app_msg_dealer;
    }
    else
    {
        subpara.dealer = secret_msg_dealer;
    }

    subpara.arg = cb;
    subpara.qos = en_mqtt_al_qos_1;
    subpara.topic.data = cb->mqtt_subtopic ;
    subpara.topic.len = strlen(cb->mqtt_subtopic);

    ret = mqtt_al_subscribe(cb->mqtt_handle,&subpara);

    return ret;
}



static int __oc_agent_engine(oc_agent_mqtt_cb_t  *cb)
{
    int conn_failed_cnt = 0;
    osal_loop_timer_t  timer;
    int ret = -1;

    if(cb->config.device_type == en_oc_mqtt_device_type_dynamic)
    {
        secret_get_devceid_passwd(cb);
    }

    while(cb->b_flag_stop == 0)
    {

        if(conn_failed_cnt > 0)
        {
            osal_task_sleep(CN_CON_BACKOFF_TIME << conn_failed_cnt);
        }

        free_mqtt_para(cb);
        if(-1 == gen_dmp_para(cb))
        {
            osal_task_sleep(CN_CON_BACKOFF_TIME << conn_failed_cnt);
            continue; ///< generate all the parameters needed by the ocean connect server
        }

        if(cn_mqtt_al_con_code_ok != connect_server(cb))
        {
            conn_failed_cnt++;
            continue;
        }

        ret = subscribe_topic(cb);
        if(ret == -1)
        {
            mqtt_al_disconnect(cb->mqtt_handle);
            free_mqtt_para(cb);
            cb->b_flag_rcv_snd = 0;
        }

        s_oc_agent_mqtt_cb = cb;

        if((cb->config.device_type == en_oc_mqtt_device_type_dynamic)&&\
                (cb->b_flag_get_passswd == 0))
        {
            osal_loop_timer_init(&timer);
            osal_loop_timer_count_down(&timer, cn_mqtt_write_for_secret_timeout);
        }
        do{
            if (cb->b_flag_update)
            {
                cb->b_flag_update = 0;
                break;
            }
            if((cb->config.device_type == en_oc_mqtt_device_type_dynamic)&&\
                    (cb->b_flag_get_passswd == 0) &&(osal_loop_timer_expired(&timer)))
            {
                break;
            }

            osal_task_sleep(100);
        }while(en_mqtt_al_connect_ok == mqtt_al_check_status(cb->mqtt_handle));

        mqtt_al_disconnect(cb->mqtt_handle);
        free_mqtt_para(cb);
        cb->b_flag_rcv_snd = 0;
    }

    ///< release all the resources
    s_oc_agent_mqtt_cb = NULL;
    free_mqtt_para(cb);
    osal_free(cb->get_device_id);
    osal_free(cb->get_device_passwd);
    osal_free(cb);

    return ret;
}



static int bs_msg_deal(void *handle,mqtt_al_msgrcv_t *msg)
{
    int ret = -1;
    struct hostent* entry = NULL;
    cJSON  *buf = NULL;
    cJSON  *address = NULL;
    char   *port = NULL;
    char   dnsbuf[64];

    memset(dnsbuf,0,sizeof(dnsbuf));
    printf("bs topic:%s qos:%d\n\r",msg->topic.data,msg->qos);

    if(msg->msg.len < CN_BS_RCV_BUFLENTH)
    {
        memcpy(s_rcv_buffer,msg->msg.data,msg->msg.len );
        s_rcv_buffer[msg->msg.len] = '\0'; ///< the json need it
        s_rcv_datalen = msg->msg.len;

        printf("msg:%s\n\r",s_rcv_buffer);
        buf = cJSON_Parse(s_rcv_buffer);
        cJSON_Print(buf);

        if(NULL != buf)
        {
            address = cJSON_GetObjectItem(buf,"address");
            if(NULL != address)
            {
                printf("address:%s\n\r", address->valuestring);
            }
            else
                return ret;

            if(NULL != address)
            {
                port = strrchr(address->valuestring, ':');
                memcpy(iot_server_port, port+1, strlen(port)-1);
                memcpy(dnsbuf, address->valuestring, port - address->valuestring);
                printf("dns:%s len:%d\n\r",dnsbuf,strlen(dnsbuf));

                entry = NULL;
                while(NULL == entry)
                {
                    entry = sal_gethostbyname(dnsbuf);

                    osal_task_sleep(1000);
                    printf("LOG:########## WAIT FOR THE DNS RESOVLED\n\r");
                }

                if(entry && entry->h_addr_list[0])
                {
                    inet_ntop(entry->h_addrtype, entry->h_addr_list[0], iot_server_ip, sizeof(iot_server_ip));
                    printf("iot server ip:%s\r\n", iot_server_ip);
                }
            }

            cJSON_Delete(buf);
        }

        osal_semp_post(s_agent_sync);
        ret = 0;

    }
    return ret;
}

///< 0 success -1 failed
static int __oc_agent_bs_state(oc_agent_mqtt_cb_t  *cb)
{
    int conn_failed_cnt = 0;
    int ret = -1;

    while((cb->runstop) && (cb->bsrunstop))
    {
        if(conn_failed_cnt > 0)
        {
            osal_task_sleep(CN_CON_BACKOFF_TIME << conn_failed_cnt);
        }
        free_mqtt_para(cb);

        ret = gen_bs_para(cb);
        if(-1 == ret)
        {
            break;
        }

        cb->constatus = en_oc_agent_mqtt_status_bs_connecting;
        ret = connect_server(cb);
        if(ret == cn_mqtt_al_con_code_err_network)
        {
            conn_failed_cnt++;
            printf("bs server conn_failed_cnt:%d\r\n", conn_failed_cnt);
            continue;
        }
        else if(ret  != cn_mqtt_al_con_code_ok)
        {
            cb->constatus = en_oc_agent_mqtt_status_bs_failed;
            break;
        }
        else
        {
            cb->constatus = en_oc_agent_mqtt_status_bs_connected;

        }
        ret = subscribe_topic(cb);
        if(ret == -1)
        {
            mqtt_al_disconnect(cb->mqtt_handle);
            free_mqtt_para(cb);
            cb->b_flag_rcv_snd = 0;
            continue;
        }

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
            free_mqtt_para(cb);
            cb->b_flag_rcv_snd = 0;
            continue;
        }

        cb->b_flag_rcv_snd = 0;
        ///< here we wait until the
        while(1)
        {

            if(cb->constatus != en_oc_agent_mqtt_status_bs_connected)
            {
                break;
            }
            if(en_mqtt_al_connect_ok != mqtt_al_check_status(cb->mqtt_handle))
            {

            }

            osal_task_sleep(1000);
        };

        break;
    }

    return ret;
}
static int __oc_agent_dmp_state(oc_agent_mqtt_cb_t  *cb)
{
    int conn_failed_cnt = 0;
    int ret = -1;

    while(cb->runstop)
    {
        if(conn_failed_cnt > 0)
        {
            osal_task_sleep(CN_CON_BACKOFF_TIME << conn_failed_cnt);
        }
        free_mqtt_para(cb);

        ret = gen_dmp_para(cb);
        if(-1 == ret)
        {
            break;
        }

        cb->constatus = en_oc_agent_mqtt_status_dmp_connecting;
        ret = connect_server(cb);
        if(ret == cn_mqtt_al_con_code_err_network)
        {
            conn_failed_cnt++;
            printf("bs server conn_failed_cnt:%d\r\n", conn_failed_cnt);
            continue;
        }
        else if(ret  != cn_mqtt_al_con_code_ok)
        {
            cb->constatus = en_oc_agent_mqtt_status_bs_failed;
            break;
        }
        else
        {
            cb->constatus = en_oc_agent_mqtt_status_bs_connected;

        }
        ret = subscribe_topic(cb);
        if(ret == -1)
        {
            mqtt_al_disconnect(cb->mqtt_handle);
            free_mqtt_para(cb);
            cb->b_flag_rcv_snd = 0;
            continue;
        }

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
            free_mqtt_para(cb);
            cb->b_flag_rcv_snd = 0;
            continue;
        }

        cb->b_flag_rcv_snd = 0;
        cb->b_flag_stop = 1;
        cb->constatus = en_oc_agent_mqtt_status_bs_success;

        break;
    }

    return ret;
}


static int __oc_agent_mqtt_engine(void  *arg)
{
    oc_agent_mqtt_cb_t  *cb;

    cb = arg;
    while(1)
    {
        if(cb->runstop)
        {
            if(cb->config.boot_mode == en_oc_boot_strap_mode_client_initialize)
            {
                __oc_agent_bs_state();
            }
            __oc_agent_dmp_state();
        }
        else
        {
            osal_task_sleep(1000);   ///< wait until if any connect request
        }

    }
    return 0;
}

static void *__oc_config(tag_oc_mqtt_config *config)
{
    oc_agent_mqtt_cb_t *ret = NULL;
    config->config_ret = cn_mqtt_al_con_code_err_unkown;

    if(0 != check_clone_config_params(ret,config))
    {
        goto EXIT_CHECK_CLONE;
    }

    if(config->boot_mode == en_oc_boot_strap_mode_client_initialize)
    {
        ret->config.boot_status = en_oc_boot_strap_status_bs;
        ret->config.bsinfo_dealer = bs_msg_deal;

    }

    return ret;
}
static int __oc_report(void *handle,char *msg, int msg_len,en_mqtt_al_qos_t qos)
{
    int ret = -1;
    size_t payloadlen;
    mqtt_al_pubpara_t  pubpara;

    oc_agent_mqtt_cb_t *cb = handle;

    if ((cb == NULL) || (qos >= en_mqtt_al_qos_err))
    {
        return ret;
    }
    if (msg == NULL || msg_len <= 0)
    {
        return ret;
    }

    if (!(cb->b_flag_rcv_snd &&(en_mqtt_al_connect_ok == mqtt_al_check_status(cb->mqtt_handle))))
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

    if(NULL != cb)
    {
        cb->b_flag_stop =1;
        ret = 0;
    }

    return ret;
}

static const tag_oc_mqtt_ops s_oc_agent_mqtt_ops= \
{
    .config = __oc_config,
    .deconfig =__oc_deconfig,
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
    cb->constatus = en_oc_agent_mqtt_status_idle;

    cb->engine = osal_task_create("oc_mqtt_agent",__oc_agent_mqtt_engine,cb,0x1400,NULL,10);
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


