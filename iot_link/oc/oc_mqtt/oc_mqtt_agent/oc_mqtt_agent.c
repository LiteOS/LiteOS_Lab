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

#include <osal.h>
#include <mqtt_al.h>
#include <oc_mqtt_al.h>

#include <cJSON.h>           //json mode
#include <hmac.h>            //used to generate the user passwd

#define VARIABLE_SIZE (4 + 1)
#define cn_cmd_topic_fmt            "/huawei/v1/devices/%s/command/%s"
#define cn_data_topic_fmt           "/huawei/v1/devices/%s/data/%s"
#define SECRET_NOTIFY_TOPIC_FMT     "/huawei/v1/products/%s/sn/%s/secretNotify"
#define SECRET_ACK_TOPIC_FMT        "/huawei/v1/products/%s/sn/%s/secretACK"

#define MQTT_TIME_BUF_LEN 11


#define cn_string_max_len  127

#define is_valid_string(name) (strnlen((name), cn_string_max_len + 1) <= cn_string_max_len)

typedef enum
{
    en_dynamic_connect_with_product_id = 0,
    en_dynamic_connect_with_device_id
}en_dynamic_connect_state;

typedef struct
{
    tag_oc_mqtt_config  config;

    int                 b_flag_get_passswd:1;     ///< this bit to be set when dynamic and received secret
    int                 b_flag_connect_dynamic:1; ///< this bit to be set when dynamic and not get secret yet
    int                 b_flag_rcv_snd:1;         ///< this bit to be set when could receive and send
    int                 b_flag_stop;              ///< this bit to be set when stop the engine
    const char         *get_device_id;            ///< when dynamic mode,this is received from the server
    const char         *get_device_passwd;        ///< when dynamic mode,this is received from the server
}tag_oc_mqtt_agent_cb;


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
#define cn_secret_index_device_pass_name    "devicepass"

#define cn_check_time_value                 "2018111517"


static int  *secret_get_devceid_passwd(tag_oc_mqtt_agent_cb  *cb)
{
    int ret = -1;

    cJSON *root = NULL;
    cJSON *tmp;
    char  *buf;
    int    len;
    tag_oc_secret_rom  rom;

    memset(&rom,0,sizeof(rom));

    cb->config.device_info.d_device.read_secret(0,&rom, sizeof(rom));

    if(rom.magic != cn_secret_magic)
    {
        goto ERR_MAGIC;
    }

    len = rom.length;
    buf = osal_malloc(len);
    if(NULL == buf)
    {
        goto ERR_BUF_MALLOC;
    }

    cb->config.device_info.d_device.read_secret(sizeof(rom),buf,len);
    root = cJSON_Parse(buf);
    if(NULL == root)
    {
        goto EXIT_JSON_PARSE;
    }

    tmp = cJSON_GetObjectItem(root, cn_secret_index_product_id_name);
    if((NULL == tmp) || (!cJSON_IsString(tmp))||\
       (0 != strcmp(cb->config.device_info.d_device.productid,tmp->valuestring)))
    {
        goto EXIT_PRODUCT_ID_ERR;
    }

    tmp = cJSON_GetObjectItem(root, cn_secret_index_product_pass_name);
    if((NULL == tmp) || (!cJSON_IsString(tmp))||\
       (0 != strcmp(cb->config.device_info.d_device.productpasswd,tmp->valuestring)))
    {
        goto EXIT_PRODUCT_PASS_ERR;
    }

    tmp = cJSON_GetObjectItem(root, cn_secret_index_note_id_name);
    if((NULL == tmp) || (!cJSON_IsString(tmp))||\
       (0 != strcmp(cb->config.device_info.d_device.notetid,tmp->valuestring)))
    {
        goto EXIT_NOTEID_ERR;
    }

    ///< ok now, we get it
    tmp = cJSON_GetObjectItem(root, cn_secret_index_device_id_name);
    if((NULL == tmp) || (!cJSON_IsString(tmp)))
    {
        goto EXIT_JSON_DEVICE_ID_ERR;
    }
    cb->get_device_id = osal_strdup(tmp->valuestring);
    if(NULL == cb->get_device_id)
    {
        goto EXIT_COPY_DEVICE_ID_ERR;
    }

    tmp = cJSON_GetObjectItem(root, cn_secret_index_device_pass_name);
    if((NULL == tmp) || (!cJSON_IsString(tmp)))
    {
        goto EXIT_JSON_DEVICE_PASS_ERR;
    }
    cb->get_device_passwd = osal_strdup(tmp->valuestring);
    if(NULL == cb->get_device_passwd)
    {
        goto EXIT_COPY_DEVICE_PASS_ERR;
    }
    cb->b_flag_get_passswd =1;

    //release the resouce
    osal_free(buf);
    cJSON_Delete(root);

    ret = 0;
    return ret;

EXIT_COPY_DEVICE_PASS_ERR:
EXIT_JSON_DEVICE_PASS_ERR:
    osal_free(cb->get_device_id);
    cb->get_device_id = NULL;
EXIT_COPY_DEVICE_ID_ERR:
EXIT_JSON_DEVICE_ID_ERR:
EXIT_NOTEID_ERR:
EXIT_PRODUCT_PASS_ERR:
EXIT_PRODUCT_ID_ERR:
EXIT_JSON_PARSE:
    osal_free(buf);
ERR_BUF_MALLOC:
ERR_MAGIC:
    return ret;
}


static int  *secret_save_devceid_passwd(tag_oc_mqtt_agent_cb  *cb)
{
    int ret = -1;

    cJSON *root;
    cJSON *tmp;
    char  *jsonbuf;
    char  *buf;
    int    len;
    tag_oc_secret_rom rom;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        goto EXIT_JSON_ROOT;
    }

    tmp = cJSON_CreateString(cb->config.device_info.d_device.productid);
    if(NULL == tmp)
    {
        goto EXIT_JSON_TMP;
    }
    cJSON_AddItemToObject(root,cn_secret_index_product_id_name,tmp);

    tmp = cJSON_CreateString(cb->config.device_info.d_device.productpasswd);
    if(NULL == tmp)
    {
        goto EXIT_JSON_TMP;
    }
    cJSON_AddItemToObject(root,cn_secret_index_product_pass_name,tmp);

    tmp = cJSON_CreateString(cb->config.device_info.d_device.notetid);
    if(NULL == tmp)
    {
        goto EXIT_JSON_TMP;
    }
    cJSON_AddItemToObject(root,cn_secret_index_note_id_name,tmp);

    tmp = cJSON_CreateString(cb->get_device_id);
    if(NULL == tmp)
    {
        goto EXIT_JSON_TMP;
    }
    cJSON_AddItemToObject(root,cn_secret_index_device_id_name,tmp);

    tmp = cJSON_CreateString(cb->get_device_passwd);
    if(NULL == tmp)
    {
        goto EXIT_JSON_TMP;
    }
    cJSON_AddItemToObject(root,cn_secret_index_device_pass_name,tmp);

    jsonbuf = cJSON_Print(root);
    if(NULL== jsonbuf)
    {
        goto EXIT_JSON_BUF;
    }

    len = strlen(jsonbuf);
    buf = osal_malloc(len + sizeof(rom));
    if(NULL == buf)
    {
        goto EXIT_SECRET_BUF;
    }
    rom.magic = cn_secret_magic;
    rom.length = len;
    hmac_generate_passwd(jsonbuf,len,&rom.magic,sizeof(rom.magic),rom.hmac,sizeof(rom.hmac));
    memcpy(buf,&rom,sizeof(rom));
    memcpy(buf+ sizeof(rom),jsonbuf,len);
    cb->config.device_info.d_device.write_secret(0,buf,len +sizeof(rom));

    osal_free(jsonbuf);
    osal_free(buf);
    cJSON_Delete(root);
    ret = 0;
    return ret;


EXIT_SECRET_BUF:
    osal_free(jsonbuf);
EXIT_JSON_BUF:
EXIT_JSON_TMP:
    cJSON_Delete(root);
EXIT_JSON_ROOT:
    return ret;
}


///< check the config parameters
static int check_clone_config_params(tag_oc_mqtt_agent_cb *cb,tag_oc_mqtt_config *config)
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

    if(NULL == config->crt)
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

    if(en_oc_mqtt_type_static == config->device_type)
    {
        if((NULL == config->device_info.s_device.deviceid) ||\
           (NULL == config->device_info.s_device.deviceid))
        {
            goto EXIT_ERR;
        }
    }
    else
    {
        if((NULL == config->device_info.d_device.productid) ||\
           (NULL == config->device_info.d_device.productid) ||\
           (NULL == config->device_info.d_device.notetid) ||\
           (NULL == config->device_info.d_device.read_secret) ||\
           (NULL == config->device_info.d_device.write_secret))
        {
            goto EXIT_ERR;
        }
    }
    memset(cb,0,sizeof(tag_oc_mqtt_agent_cb));
    cb->config = *config;

    ret = 0;
    return ret;


EXIT_ERR:
    return ret;
}


typedef struct
{
    const char *client_id;
    const char *user_id;
    const char *passwd;
    const char *topic;
}tag_gen_mqtt_con_para;


#define cn_client_id_fmt_static     "%s_%d_%d_%s"      ///<  deviceid  connect_type check_time time
#define cn_client_id_fmt_dynamic    "%s_%s_%d_%d_%s"   ///<  deviceid  noteid connect_type check_time time

static int gen_mqtt_para(tag_oc_mqtt_agent_cb *cb,tag_gen_mqtt_con_para *genpara)
{
    int ret = -1;
    int len;
    char *buf;

    if(cb->config.device_type == en_oc_mqtt_type_static)
    {
        len = strlen(cb->config.device_info.s_device.deviceid)  + strlen(cn_check_time_value) + \
                strlen(cn_client_id_fmt_static) + 1;
        genpara->client_id = osal_malloc(len);
        if (genpara->client_id != buf)
        {
            snprintf(buf,len,cn_client_id_fmt_static,cb->config.device_info.s_device.deviceid,\
                    0,cb->config.sign_type,cn_check_time_value);
        }
        else
        {
            goto EXIT_MALLOC;
        }


    }
    else if(cb->b_flag_get_passswd)
    {

    }
    else
    {

    }
EXIT_MALLOC:
    if()


}



static int connect_server(mqtt_client_s* handle)
{
    bool  connect_static;
    bool  check_time;
    char time[MQTT_TIME_BUF_LEN];
    char *passwd   = NULL;
    char *deviceid = NULL;
    char *productid= NULL;
    char *noteid   = NULL;
    char *user;
    mqtt_al_conpara_t conpara;
    int   ret = ATINY_ERR;

    connect_static = mqtt_is_connectting_with_deviceid(handle);
    check_time = handle->device_info.sign_type;

    if (connect_static)
    {
        if (handle->device_info.connection_type == MQTT_STATIC_CONNECT)
        {
            deviceid = handle->device_info.u.s_info.deviceid;
            passwd = handle->device_info.password;
        }
        else
        {
            deviceid = handle->dynamic_info.save_info.deviceid;
            passwd = handle->dynamic_info.got_password;
        }
        user = deviceid;
        ATINY_LOG(LOG_INFO, "try static connect");
    }
    else
    {
        productid = handle->device_info.u.d_info.productid;
        noteid = handle->device_info.u.d_info.nodeid;
        passwd = handle->device_info.password;
        user = productid;
        ATINY_LOG(LOG_INFO, "try dynamic connect");
    }

    if (mqtt_cmd_ioctl(MQTT_GET_TIME, time, sizeof(time)) != ATINY_OK)
    {
        ATINY_LOG(LOG_FATAL, "mqtt_cmd_ioctl fail");
        return ret;
    }
    time[sizeof(time) - 1] = '\0';
    //GENERATE THE DATA THE CLOUD PLATFORM NEED
    memset(&conpara,0,sizeof(conpara));
    conpara.clientid.data = generate_client_id(deviceid,productid,noteid,time,connect_static,check_time);
    conpara.clientid.len = strlen(conpara.clientid.data);

    conpara.user.data = generate_client_user(user);
    conpara.user.len = strlen(conpara.user.data);

    conpara.passwd.data = generate_client_passwd(passwd,time);
    conpara.passwd.len = strlen(conpara.passwd.data);
    if((NULL ==conpara.clientid.data ) || (NULL == conpara.user.data) || (NULL == conpara.passwd.data))
    {
        atiny_free(conpara.clientid.data);
        atiny_free(conpara.user.data);
        atiny_free(conpara.passwd.data);

        return ret;
    }

    conpara.cleansession = 1;
    conpara.keepalivetime = MQTT_KEEPALIVE_INTERVAL_S;
    conpara.security = &handle->params.info;
    conpara.serveraddr.data = handle->params.server_ip;
    conpara.serveraddr.len = strlen(handle->params.server_ip);
    conpara.serverport = atoi (handle->params.server_port);
    conpara.timeout = 10000;
    conpara.version = en_mqtt_al_version_3_1_1;
    conpara.willmsg = NULL;
    ATINY_LOG(LOG_DEBUG, "CLIENTID:%s USER:%s PASSWD:%s \r\n",\
            conpara.clientid.data,conpara.user.data,conpara.passwd.data);

    handle->mqtthandle = mqtt_al_connect(&conpara);
    if(NULL != handle->mqtthandle)
    {
        ret = ATINY_OK;
    }

    atiny_free(conpara.clientid.data);
    atiny_free(conpara.user.data);
    atiny_free(conpara.passwd.data);

    return ret;
}


static int __oc_agent_engine(void *args)
{
    int ret = -1;


    return ret;
}


static void *__oc_config(tag_oc_config_param *config)
{
    tag_oc_mqtt_agent_cb *ret = NULL;

    ret = osal_malloc(sizeof(tag_oc_mqtt_agent_cb));
    if(NULL == ret)
    {
        goto EXIT_CB_MEM;
    }

    if(0 == check_clone_config_params(ret,config))
    {
        goto EXIT_CHECK_CLONE;
    }
    if(NULL == osal_task_create(ret->config.server,__oc_agent_engine,ret,0x800,NULL,6))
    {
        goto EXIT_ENGINE_CREATE;
    }

    return ret;


EXIT_ENGINE_CREATE:
EXIT_CHECK_CLONE:
    osal_free(ret);
    ret = NULL;
EXIT_CB_MEM:
    return ret;
}






