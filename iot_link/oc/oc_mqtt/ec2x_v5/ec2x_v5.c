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
 *  DATE                INSTRUCTION
 *  2020-09-08 19:52    The first version
 *
 */

#include <string.h>
#include <at.h>
#include <oc_mqtt_al.h>
#include <osal.h>
#include <stdlib.h>
#include <iot_link_config.h>

#ifndef CONFIG_EC2X_RCVLEN
#define CONFIG_EC2X_RCVLEN 1024
#endif

#ifndef CONFIG_EC2X_CMD_BASETIME
#define CONFIG_EC2X_CMD_BASETIME (5*1000)
#endif


#ifndef CONFIG_EC2X_CMDRESPLEN
#define CONFIG_EC2X_CMDRESPLEN  64
#endif

#ifndef CONFIG_EC2X_CMDLEN
#define CONFIG_EC2X_CMDLEN     128
#endif


typedef struct
{
    //initialized by the applicaiton
    fn_oc_mqtt_msg_deal  msg_dealer;
    void                *msg_dealer_args;
    osal_mutex_t         lock;    ///< this is the at lock here
} ec2xv5_cb_t;
static ec2xv5_cb_t   g_ec2xv5_cb;

//at command  with response:0 success while -1 failed
static int atcmd_response(const char *cmd,const char *index,uint8_t *buf, int len ,int timeout)
{
    int ret = -1;

    if(osal_mutex_lock(g_ec2xv5_cb.lock)){
        ret = at_command((unsigned char *)cmd,strlen(cmd),index,(char *)buf,len,timeout);
        if(ret >= 0){
            ret = 0;
        }
        (void) osal_mutex_unlock(g_ec2xv5_cb.lock);
    }
    return ret;
}


///< format:\r\n+HMREC="topic",len,payload
#define CN_HMREC_INDEX  "\r\n\r\n+HMREC="
static int ec2x_oc_msg_deal(void *args,void *msg,size_t len)
{
    char *str_start;
    mqtt_al_msgrcv_t ocmsg;
    int payload_len;

    if(NULL != g_ec2xv5_cb.msg_dealer){
        memset(&ocmsg,0, sizeof(ocmsg));
        str_start = (char *)msg + strlen(CN_HMREC_INDEX);
        str_start ++;
        ocmsg.topic.data = str_start;
        str_start = strstr(str_start,"\"");
        ocmsg.topic.len = str_start - ocmsg.topic.data;
        str_start++;
        sscanf(str_start,",%d,", &payload_len);
        str_start = strstr(str_start,"\"");
        ocmsg.msg.data = str_start+1;
        ocmsg.msg.len = payload_len;
        g_ec2xv5_cb.msg_dealer(g_ec2xv5_cb.msg_dealer_args,&ocmsg);
    }

    return 0;
}

#define CN_HMDIS_INDEX "\r\n+HMDIS OK"
static int iot_deconfig(void)
{
    int ret = en_oc_mqtt_err_system;
    char cmd[CONFIG_EC2X_CMDLEN];

    snprintf(cmd, CONFIG_EC2X_CMDLEN, "%s\r\n","AT+HMDIS");
    if(0 == atcmd_response(cmd,CN_HMDIS_INDEX,NULL, 0, CONFIG_EC2X_CMD_BASETIME*5)){
        ret = en_oc_mqtt_err_ok;
    }
    return ret;
}

#define CN_HMCON_INDEX    "\r\n+HMCON "
static int iot_config(oc_mqtt_config_t *param)
{
    int ret = en_oc_mqtt_err_system;
    char cmd[CONFIG_EC2X_CMDLEN];
    uint8_t cmd_resp[CONFIG_EC2X_CMDRESPLEN];

    int boot_mode = 0;
    if(param->boot_mode == en_oc_mqtt_mode_bs_static_nodeid_hmacsha256_notimecheck_json){
        boot_mode = 1;
    }

    if(NULL == param->pwd){
        snprintf(cmd, CONFIG_EC2X_CMDLEN,"AT+HMCON=%d,%d,\"%s\",\"%s\",\"%s\",,0\r\n",\
                boot_mode, param->lifetime,param->server_addr,param->server_port,param->id);
    }
    else{
        snprintf(cmd, CONFIG_EC2X_CMDLEN,"AT+HMCON=%d,%d,\"%s\",\"%s\",\"%s\",\"%s\",0\r\n",\
                boot_mode, param->lifetime,param->server_addr,param->server_port,param->id,param->pwd);
    }
    if(0 == atcmd_response(cmd,CN_HMCON_INDEX,cmd_resp, CONFIG_EC2X_CMDRESPLEN, CONFIG_EC2X_CMD_BASETIME*5)){
        if(NULL != strstr((char *)cmd_resp, "+HMCON OK")){
            ret = en_oc_mqtt_err_ok;
            g_ec2xv5_cb.msg_dealer = param->msg_deal;
            g_ec2xv5_cb.msg_dealer_args = param->msg_deal_arg;
        }
        else{
            sscanf((char *)cmd_resp,"\r\n+HMCON ERR:%d",&ret);
        }
    }
    return ret;
}

///<format:AT+HMPUB=QOS,"TOPIC",LEN,"PAYLOAD"
#define CN_PUB_DEFAULTTOPIC_FMT   "AT+HMPUB=%d,,%d,\"%s\"\r\n"
#define CN_PUB_SPECTOPIC_FMT      "AT+HMPUB=%d,\"%s\",%d,\"%s\"\r\n"
#define CN_HMPUB_INDEX  "\r\n+HMPUB OK"
static int iot_publish(char *topic,uint8_t *msg,int len,int qos)
{
    int ret = en_oc_mqtt_err_system;
    char *cmd;
    int  cmd_len;

    if(topic == NULL){
        cmd_len = len + strlen(CN_PUB_DEFAULTTOPIC_FMT);
        cmd = osal_malloc(cmd_len);
        if(cmd != NULL){
            snprintf(cmd, cmd_len,CN_PUB_DEFAULTTOPIC_FMT,qos,len,msg);
        }
    }
    else{
        cmd_len = len + strlen(CN_PUB_SPECTOPIC_FMT)+strlen(topic);
        cmd = osal_malloc(cmd_len);
        if(cmd != NULL){
            snprintf(cmd, cmd_len,CN_PUB_SPECTOPIC_FMT,qos,topic,len,msg);
        }
    }
    if(cmd != NULL){
        if(0 ==  atcmd_response(cmd,CN_HMPUB_INDEX,NULL,0,CONFIG_EC2X_CMD_BASETIME*5)){
            ret = en_oc_mqtt_err_ok;
        }
        osal_free(cmd);
    }
    return ret;
}

///< subscribe a user topic
#define CN_SUB_USERTOPICMDT  "AT+HMSUB=%d,\"%s\"\r\n"
#define CN_HMSUB_INDEX  "\r\n+HMSUB OK"
static int iot_subscribe(char *topic,int qos)
{
    int ret = en_oc_mqtt_err_system;
    char *cmd;
    int  cmd_len;

    if(NULL == topic){
        return ret;
    }
    cmd_len = strlen(topic) + strlen(CN_SUB_USERTOPICMDT);
    cmd = osal_malloc(cmd_len);
    if(cmd != NULL){
        snprintf(cmd, cmd_len,CN_SUB_USERTOPICMDT,qos,topic);
        if(0 ==  atcmd_response(cmd,CN_HMSUB_INDEX,NULL,0,CONFIG_EC2X_CMD_BASETIME*5)){
            ret = en_oc_mqtt_err_ok;
        }
        osal_free(cmd);
    }
    return ret;
}

#define CN_UNSUB_USERTOPICMDT  "AT+HMUNS=\"%s\"\r\n"
#define CN_HMUNS_INDEX "\r\n+HMUNS OK"
static int iot_unsubscribe(char *topic)
{
    int ret = en_oc_mqtt_err_system;
    char *cmd;
    int  cmd_len;

    if(NULL == topic){
        return ret;
    }

    cmd_len = strlen(topic) + strlen(CN_UNSUB_USERTOPICMDT);
    cmd = osal_malloc(cmd_len);
    if(cmd != NULL){
        snprintf(cmd, cmd_len,CN_UNSUB_USERTOPICMDT,topic);
        if(0 ==  atcmd_response(cmd,CN_HMUNS_INDEX,NULL,0,CONFIG_EC2X_CMD_BASETIME*5)){
            ret = en_oc_mqtt_err_ok;
        }
        osal_free(cmd);
    }
    return ret;
}

static const oc_mqtt_t g_iot_opt =
{
    .name = "ec2xv5",
    .op = {
        .config = iot_config,
        .deconfig = iot_deconfig,
        .publish =  iot_publish,
        .subscribe = iot_subscribe,
        .unsubscribe = iot_unsubscribe,
    },
};

static int module_checkat(void)
{
    int ret;
    ret = atcmd_response("AT\r\n","OK", NULL, 0, CONFIG_EC2X_CMD_BASETIME);
    if(0 == ret){
        atcmd_response("ATE0\r\n","OK", NULL, 0, CONFIG_EC2X_CMD_BASETIME);
    }
    return ret;
}

static int module_checksim(void)
{
    int ret;

    ret = atcmd_response("AT+CPIN?\r\n","+CPIN: READY", NULL, 0, CONFIG_EC2X_CMD_BASETIME);

    return ret;
}

static int module_checkcsq(void)
{
    int ret;
    int csq = 0;
    char *str;
    uint8_t resp[CONFIG_EC2X_CMDRESPLEN];

    ret = atcmd_response("AT+CSQ\r\n","+CSQ: ", resp, CONFIG_EC2X_CMDRESPLEN, CONFIG_EC2X_CMD_BASETIME);
    if(ret == 0){
        str = strstr((char *)resp,"+CSQ:");
        sscanf(str,"+CSQ: %d,",&csq);
        if((csq <= 20) &&(csq >= 31)){
            ret = -1;
        }
    }
    return ret;
}

static int module_checkcgatt(void)
{
    int ret;
    int cgatt = 0;
    char *str;
    uint8_t resp[CONFIG_EC2X_CMDRESPLEN];

    ret = atcmd_response("AT+CGATT?\r\n","+CGATT: ", resp, CONFIG_EC2X_CMDRESPLEN, CONFIG_EC2X_CMD_BASETIME);
    if(ret == 0){
        str = strstr((char *)resp,"+CGATT:");
        sscanf(str,"+CGATT: %d,",&cgatt);
        if(cgatt != 1){
            ret = -1;
        }
    }
    return ret;
}

static int module_checkcgreg(void)
{
    int ret;
    int cgreg = 0;
    int para;
    char *str;
    uint8_t resp[CONFIG_EC2X_CMDRESPLEN];

    ret = atcmd_response("AT+CGREG?\r\n","+CGREG: ", resp, CONFIG_EC2X_CMDRESPLEN, CONFIG_EC2X_CMD_BASETIME);
    if(ret == 0){
        str = strstr((char *)resp,"+CGREG:");
        sscanf(str,"+CGREG: %d,%d",&para,&cgreg);
        if((cgreg != 1) && (cgreg != 5)){
            ret = -1;
        }
    }
    return ret;
}

static void module_check(void)
{
    ///< we should do the check
 BEGIN_CHECK:
    osal_task_sleep(1000);
    if( 0 == module_checkat()){
        LINK_LOG_DEBUG("MODULE AT_CHECK SUCCESS");
    }
    else{
        LINK_LOG_DEBUG("MODULE AT_CHECK FAILED");
        goto BEGIN_CHECK;
    }

    if( 0 == module_checksim()){
        LINK_LOG_DEBUG("MODULE SIM_CHECK SUCCESS");
    }
    else{
        LINK_LOG_DEBUG("MODULE SIM_CHECK FAILED");
        goto BEGIN_CHECK;
    }

    if( 0 == module_checkcsq()){
        LINK_LOG_DEBUG("MODULE CSQ_CHECK SUCCESS");
    }
    else{
        LINK_LOG_DEBUG("MODULE CSQ_CHECK FAILED");
        goto BEGIN_CHECK;
    }

    if( 0 == module_checkcgatt()){
        LINK_LOG_DEBUG("MODULE CGATT_CHECK SUCCESS");
    }
    else{
        LINK_LOG_DEBUG("MODULE CGATT_CHECK FAILED");
        goto BEGIN_CHECK;
    }

    if( 0 == module_checkcgreg()){
        LINK_LOG_DEBUG("MODULE CGREG_CHECK SUCCESS");
    }
    else{
        LINK_LOG_DEBUG("MODULE CGREG_CHECK FAILED");
        goto BEGIN_CHECK;
    }

    ///< NOW SET IT TO THE USBNET
    atcmd_response("AT+QCFG=\"usbnet\",1\r\n","+CGREG: ", NULL, 0, CONFIG_EC2X_CMD_BASETIME);
    ///< NOW SET IT TO URC TO ALL PORT
    atcmd_response("AT+QURCCFG=\"urcport\",\"all\"\r\n","+CGREG: ", NULL, 0, CONFIG_EC2X_CMD_BASETIME);

    LINK_LOG_DEBUG("******MODULE CHECKSUCCESS*********");
    return;
}

int oc_mqtt_imp_init(void)
{
    int ret = -1;
    osal_mutex_create(&g_ec2xv5_cb.lock);
    module_check();

    at_oobregister("EC2XV5_HMREC",CN_HMREC_INDEX,strlen(CN_HMREC_INDEX),ec2x_oc_msg_deal,NULL);
    ret = oc_mqtt_register(&g_iot_opt);
    return ret;
}




