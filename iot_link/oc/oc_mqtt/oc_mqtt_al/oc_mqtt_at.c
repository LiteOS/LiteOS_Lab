/*----------------------------------------------------------------------------
 * Copyright (c) <2016-2019>, <Huawei Technologies Co., Ltd>
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <osal.h>
#include <oc_mqtt_al.h>

extern int link_main(void *args);


///< when receive any information from hw, then it call this function,THIS IS A URC imformation
///< ATCOMMAND:    +HWMQTTRECVPUB:1,0,"HELLOTOPC",2,0010
void __attribute__((weak)) hwoc_mqtt_recvpub(int qos,int dup,const char *topic,uint8_t *payload, int len)
{
    printf("%s:specified topic message\n\r",__FUNCTION__);
    ///< PLEASE USE THE AT PIPE TO OUTPUT THE INFORMATION
    return;
}

///< when receive any information from hw, then it call this function,THIS IS A URC imformation
///< received the default topic message
///< ATCOMMAND:    +HWMQTTRECEIVED:1,2,0010
void __attribute__((weak)) hwoc_mqtt_received(int qos,uint8_t *payload, int len)
{
    printf("%s:default topic message\n\r",__FUNCTION__);
    ///< PLEASE USE THE AT PIPE TO OUTPUT THE INFORMATION
    return;
}


//use this function to push all the message to the buffer
static int app_msg_deal(void *arg,mqtt_al_msgrcv_t *msg)
{
    int ret = -1;
    char *topic;

    printf("%s:qos:%d dup:%d topiclen:%d msglen:%d\n\r",__FUNCTION__,msg->qos,msg->dup,\
            msg->topic.len,msg->msg.len);
    if((NULL != msg->topic.data) && (msg->topic.len > 0))
    {
        topic = osal_malloc(msg->topic.len +1);
        if(NULL != topic)
        {
            memcpy(topic, msg->topic.data,msg->topic.len);
            topic[msg->topic.len] = '\0';
            hwoc_mqtt_recvpub(msg->qos,msg->dup,topic,(uint8_t *)msg->msg.data,msg->msg.len);
        }
    }
    else
    {
        hwoc_mqtt_received(msg->qos,(uint8_t *)msg->msg.data,msg->msg.len);
    }

    return ret;
}


///< when the at pipe want to send some message to the hw,then it call this function,
///< if the topic is NULL, then it will send to the default topic
///< ATCOMMAND:    AT+HWOCMQTTSEND=1,3,001020
///< ATRESPONSE:   +SEND OK when success
///<               +SEND ERR:code
///<               code:reference to en_oc_mqtt_err_code_t defines

int hwoc_mqtt_send(int qos,uint8_t *payload,int len)
{
    link_main(NULL);
    return oc_mqtt_publish(NULL,payload, len, qos);
}

///< when the at pipe want to send some message to the hw,then it call this function,
///< if the topic is NULL, then it will send to the default topic
///< ATCOMMAND:    AT+HWOCMQTTPUBLISH=1,"PUBTOPIC",3,001020
///< ATRESPONSE:   +PUBLISH OK when success
//<                +PUBLISH ERR:code
///<               code:reference to en_oc_mqtt_err_code_t defines
int hwoc_mqtt_publish(int qos,char *topic,uint8_t *payload,int len)
{
    link_main(NULL);
    return oc_mqtt_publish(topic,payload, len, qos);
}


///< when you want to connect to the hw,then call this function
///< ATCOMMAND:    AT+HWOCMQTTCONNECT=1,10,"192.168.1.20","8883","DEVEID","DEVEPASSWD"
///< ATRESPONSE:   +CONNECTED OK when success
///<               +CONNECTED ERR:code
///<               code:reference to en_oc_mqtt_err_code_t defines

int hwoc_mqtt_connect(int bsmode, unsigned short lifetime, const char *ip, const char *port,
                               const char *deviceid, const char *devicepasswd)
{
    int ret;
    oc_mqtt_config_t config;

    link_main(NULL);
    if(bsmode)
    {
        config.boot_mode = en_oc_mqtt_mode_bs_static_nodeid_hmacsha256_notimecheck_json;
    }
    else
    {
        config.boot_mode = en_oc_mqtt_mode_nobs_static_nodeid_hmacsha256_notimecheck_json;
    }

    config.server_addr = (char *)ip;
    config.server_port =(char *) port;
    config.lifetime = lifetime;
    config.msg_deal = app_msg_deal;
    config.msg_deal_arg = NULL;
    config.id = (char *)deviceid;
    config.pwd = (char *)devicepasswd;
    config.security.type = EN_DTLS_AL_SECURITY_TYPE_CERT;

    ret = oc_mqtt_config(&config);


    return ret;
}

///< WHEN YOU WANT TO DISCONNECT FROM THE HW,THEN YOU CALL THIS FUNCTION
///< ATCOMMAND:    AT+HWOCMQTTDISCONNECT
///< ATRESPONSE:   +DISCONNECT OK  when succes
///                +DISCONNECT ERR:code  when succes
///<               code:reference to en_oc_mqtt_err_code_t defines

int hwoc_mqtt_disconnect()
{
    int ret = -1;

    link_main(NULL);
    ret = oc_mqtt_deconfig();

    return ret;
}


///< WHEN YOU WANT TO KNOW WHICH VERSION YOU USE
///< ATCOMMAND: AT+HWOCMQTTVERSION
///< ATRESPONSE: +HWOCMQTTVERSION:vx.x.x AT XXXXXX ON XXXXX

char *hwoc_mqtt_version()
{
    extern char *linkmain_version();

    return linkmain_version();
}






