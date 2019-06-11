/*----------------------------------------------------------------------------
 * Copyright (c) <2019>, <Huawei Technologies Co., Ltd>
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
 *  2019-05-27 18:00    huerjia     The first version  
 *
 */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <osal.h>
#include <oc_mqtt_al.h>

#include "oc_mqtt_bootstrap.h"

#define cn_bs_request_topic_fmt               "/huawei/v1/devices/%s/iodpsData"
#define cn_bs_message_topic_fmt               "/huawei/v1/devices/%s/iodpsCommand"

#define cn_check_time_value                   "2018111517"
#define cn_string_max_len                     127

/*The unit is millisecond*/
#ifndef cn_mqtt_conn_failed_base_delay
#define cn_mqtt_conn_failed_base_delay 1000
#endif

#ifndef cn_mqtt_keepalive_interval_s
#define cn_mqtt_keepalive_interval_s (100)
#endif


typedef struct
{
    tag_bs_mqtt_config  config;

    unsigned int        b_flag_update:1;          ///< when get device id and passwd should set it
    unsigned int        b_flag_connect_dynamic:1; ///< this bit to be set when dynamic and not get secret yet
    unsigned int        b_flag_rcv_snd:1;         ///< this bit to be set when could receive and send
    unsigned int        b_flag_stop:1;            ///< this bit to be set when stop the engine
    ///< the following used for the mqtt connect
    void         *mqtt_handle;
    char         *mqtt_client_id;
    char         *mqtt_user;
    char         *mqtt_passwd;
    char         *mqtt_subtopic;
    char         *mqtt_pubtopic;

}tag_oc_mqtt_bs_cb;

static tag_oc_mqtt_bs_cb *s_bs_agent_cb;

static int free_mqtt_para(tag_oc_mqtt_bs_cb *cb)
{
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
        printf("free sub#\r\n");
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

#define cn_client_id_fmt_static     "%s_%d_%d_%s"      ///<  deviceid  connect_type check_time time

static int gen_bs_para(tag_oc_mqtt_bs_cb *cb)
{
    int ret = -1;
    int len;
    int i;
    unsigned char hmac[32];
    char  *time_value;
    char  *passwd;

    time_value = cn_check_time_value;

    len = strlen(cb->config.deviceid)  + \
          strlen(time_value) + strlen(cn_client_id_fmt_static) + 1;
    cb->mqtt_client_id = osal_malloc(len);
    if (cb->mqtt_client_id != NULL)
    {
        snprintf(cb->mqtt_client_id,len,cn_client_id_fmt_static,cb->config.deviceid,\
                0,cb->config.sign_type,time_value);
    }
    else
    {
        goto EXIT_MALLOC;
    }

    len = strlen(cb->config.deviceid) +1;
    cb->mqtt_user = osal_malloc(len);
    if (cb->mqtt_user != NULL)
    {
        memcpy(cb->mqtt_user,cb->config.deviceid,len);
    }
    else
    {
        goto EXIT_MALLOC;
    }

    len = sizeof(hmac) * 2 + 1;
    passwd = (char *)cb->config.devicepasswd;
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

    len = strnlen(cb->config.deviceid, cn_string_max_len) +\
            strlen(cn_bs_message_topic_fmt) + 1;
    cb->mqtt_subtopic = osal_malloc(len);
    if(NULL != cb->mqtt_subtopic)
    {
        snprintf(cb->mqtt_subtopic, len, cn_bs_message_topic_fmt, \
                cb->config.deviceid);
    }
    else
    {
        goto EXIT_MALLOC;
    }


    len = strnlen(cb->config.deviceid, cn_string_max_len) +\
            strlen(cn_bs_request_topic_fmt) + 1;
    cb->mqtt_pubtopic = osal_malloc(len);
    if(NULL != cb->mqtt_pubtopic)
    {
        snprintf(cb->mqtt_pubtopic, len, cn_bs_request_topic_fmt, \
                cb->config.deviceid);
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

static void app_msg_dealer(void *arg,mqtt_al_msgrcv_t  *msg)
{
    //for we must add the '/0' to the end to make sure the json parse correct
    if ((msg == NULL) || ( msg->msg.data == NULL))
    {
        return;
    }
    s_bs_agent_cb->config.msgdealer(s_bs_agent_cb,msg);
}


static int subscribe_topic(tag_oc_mqtt_bs_cb *cb)
{
    int ret = -1;
    mqtt_al_subpara_t   subpara;

    memset(&subpara,0,sizeof(subpara));

    subpara.dealer = app_msg_dealer;

    subpara.arg = cb;
    subpara.qos = en_mqtt_al_qos_1;
    subpara.topic.data = cb->mqtt_subtopic ;
    subpara.topic.len = strlen(cb->mqtt_subtopic);

    ret = mqtt_al_subscribe(cb->mqtt_handle,&subpara);

    return ret;
}

static int connect_server(tag_oc_mqtt_bs_cb *cb)
{
    mqtt_al_conpara_t conpara;
    int   ret = -1;

    //GENERATE THE DATA THE CLOUD PLATFORM NEED
    memset(&conpara,0,sizeof(conpara));

    conpara.clientid.data = cb->mqtt_client_id;
    conpara.clientid.len = strlen(conpara.clientid.data);

    conpara.user.data =  cb->mqtt_user;
    conpara.user.len = strlen(conpara.user.data);

    conpara.passwd.data = cb->mqtt_passwd;
    conpara.passwd.len = strlen(conpara.passwd.data);

    conpara.cleansession = 1;
    conpara.keepalivetime = cn_mqtt_keepalive_interval_s;
    conpara.security = &cb->config.security;
    conpara.serveraddr.data = (char *)cb->config.server;
    conpara.serveraddr.len = strlen(conpara.serveraddr.data);
    conpara.serverport = atoi (cb->config.port);
    conpara.timeout = 10000;
    conpara.version = en_mqtt_al_version_3_1_1;
    conpara.willmsg = NULL;
    printf("CLIENTID:%s \r\n USER:%s \r\n PASSWD:%s \r\n",\
            conpara.clientid.data,conpara.user.data,conpara.passwd.data);

    cb->mqtt_handle = mqtt_al_connect(&conpara);
    if(NULL != cb->mqtt_handle)
        ret = 0;

    return ret;
}

///< check the config parameters
static int check_clone_config_params(tag_oc_mqtt_bs_cb *cb, tag_bs_mqtt_config *config)
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

    if((NULL == config->deviceid) || (NULL == config->devicepasswd))
    {
        goto EXIT_ERR;
    }

    memset(cb,0,sizeof(tag_oc_mqtt_bs_cb));
    cb->config = *config;

    ret = 0;
    return ret;


EXIT_ERR:
    return ret;
}


static int __oc_bs_engine(void *args)
{
    int conn_failed_cnt = 0;
    int ret = -1;

    tag_oc_mqtt_bs_cb  *cb;

    cb = (tag_oc_mqtt_bs_cb *)args;
    if(NULL == cb)
    {
        return ret;
    }

    while(cb->b_flag_stop == 0)
    {
        if(conn_failed_cnt > 0)
        {
            osal_task_sleep(cn_mqtt_conn_failed_base_delay << conn_failed_cnt);
        }

        free_mqtt_para(cb);
        if(-1 == gen_bs_para(cb))
        {
            osal_task_sleep(cn_mqtt_conn_failed_base_delay << conn_failed_cnt);
            continue; ///< generate all the parameters needed by the ocean connect server
        }

        if(0 != connect_server(cb))
        {
            conn_failed_cnt++;
            printf("conn_failed_cnt:%d\r\n", conn_failed_cnt);
            continue;
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

        s_bs_agent_cb = cb;
        cb->b_flag_rcv_snd = 0;
        cb->b_flag_stop = 1;
    }

    ///< release all the resources
    //s_bs_agent_cb = NULL;
    //free_mqtt_para(cb);
    //osal_free(cb);

    return ret;
}

static void *__bs_config(tag_bs_mqtt_config *config)
{
    tag_oc_mqtt_bs_cb *ret = NULL;

    ret = osal_zalloc(sizeof(tag_oc_mqtt_bs_cb));
    if(NULL == ret)
    {
        goto EXIT_CB_MEM;
    }

    if(0 != check_clone_config_params(ret,config))
    {
        goto EXIT_CHECK_CLONE;
    }
    if(NULL == osal_task_create("oc_mqtt_bs",__oc_bs_engine,ret,0x2000,NULL,6))
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


static const tag_bs_mqtt_ops s_oc_mqtt_ops_bs= \
{
    .config = __bs_config,
    //.deconfig =__bs_deconfig,
    //.report = __bs_report,
};

int oc_mqtt_install_bootstrap()
{
    int ret;

    ret = bs_mqtt_register(&s_oc_mqtt_ops_bs);

    return ret;
}
