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
 *  2019-11-13       xuliqun  The first version  
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sal.h>
#include <osal.h>
#include <queue.h>
#include <oc_mqtt_al.h>
#include <mqtt_al.h>

#include "test_case.h"

//install atiny mqtt
#include <oc_mqtt_tiny.h>


static char s_mqtt_ca_crt[] =
"-----BEGIN CERTIFICATE-----\r\n"
"MIID4DCCAsigAwIBAgIJAK97nNS67HRvMA0GCSqGSIb3DQEBCwUAMFMxCzAJBgNV\r\n"
"BAYTAkNOMQswCQYDVQQIEwJHRDELMAkGA1UEBxMCU1oxDzANBgNVBAoTBkh1YXdl\r\n"
"aTELMAkGA1UECxMCQ04xDDAKBgNVBAMTA0lPVDAeFw0xNjA1MDQxMjE3MjdaFw0y\r\n"
"NjA1MDIxMjE3MjdaMFMxCzAJBgNVBAYTAkNOMQswCQYDVQQIEwJHRDELMAkGA1UE\r\n"
"BxMCU1oxDzANBgNVBAoTBkh1YXdlaTELMAkGA1UECxMCQ04xDDAKBgNVBAMTA0lP\r\n"
"VDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAJxM9fwkwvxeILpkvoAM\r\n"
"Gdqq3x0G9o445F6Shg3I0xmmzu9Of8wYuW3c4jtQ/6zscuIGyWf06ke1z//AVZ/o\r\n"
"dp8LkuFbBbDXR5swjUJ6z15b6yaYH614Ty/d6DrCM+RaU+FWmxmOon9W/VELu2BB\r\n"
"NXDQHJBSbWrLNGnZA2erk4JSMp7RhHrZ0QaNtT4HhIczFYtQ2lYF+sQJpQMrjoRn\r\n"
"dSV9WB872Ja4DgcISU1+wuWLmS/NKjIvOWW1upS79yu2I4Rxos2mFy9xxz311rGC\r\n"
"Z3X65ejFNzCUrNgf6NEP1N7wB9hUu7u50aA+/56D7EgjeI0gpFytC+a4f6JCPVWI\r\n"
"Lr0CAwEAAaOBtjCBszAdBgNVHQ4EFgQUcGqy59oawLEgMl21//7F5RyABpwwgYMG\r\n"
"A1UdIwR8MHqAFHBqsufaGsCxIDJdtf/+xeUcgAacoVekVTBTMQswCQYDVQQGEwJD\r\n"
"TjELMAkGA1UECBMCR0QxCzAJBgNVBAcTAlNaMQ8wDQYDVQQKEwZIdWF3ZWkxCzAJ\r\n"
"BgNVBAsTAkNOMQwwCgYDVQQDEwNJT1SCCQCve5zUuux0bzAMBgNVHRMEBTADAQH/\r\n"
"MA0GCSqGSIb3DQEBCwUAA4IBAQBgv2PQn66gRMbGJMSYS48GIFqpCo783TUTePNS\r\n"
"tV8G1MIiQCpYNdk2wNw/iFjoLRkdx4va6jgceht5iX6SdjpoQF7y5qVDVrScQmsP\r\n"
"U95IFcOkZJCNtOpUXdT+a3N+NlpxiScyIOtSrQnDFixWMCJQwEfg8j74qO96UvDA\r\n"
"FuTCocOouER3ZZjQ8MEsMMquNEvMHJkMRX11L5Rxo1pc6J/EMWW5scK2rC0Hg91a\r\n"
"Lod6aezh2K7KleC0V5ZlIuEvFoBc7bCwcBSAKA3BnQveJ8nEu9pbuBsVAjHOroVb\r\n"
"8/bL5retJigmAN2GIyFv39TFXIySw+lW0wlp+iSPxO9s9J+t\r\n"
"-----END CERTIFICATE-----\r\n";

#define DEFAULT_SERVER_IPV4         "49.4.93.24"     ///<  server ip address
#define DEFAULT_SERVER_PORT         "8883"           ///<  server mqtt service port
#define CN_OC_MQTT_CLIENTID_STATIC_FMT     "%s_%d_%d_%s"
#define cn_check_time_value                 "2018111517"
static void *g_mqtt_al_handle = NULL;
#define DEFAULT_LIFETIME            10  ///<  server mqtt service port

static int g_ioswitch = -1;

typedef struct oc_mqtt_paras_s{
    en_oc_mqtt_mode  boot_mode;
    unsigned short life_time;
    char *server_ip4;
    char *server_port;
    unsigned short sec_type;
    char *id;
    char *passwd;
    char *cbname;
    unsigned short ca_is_valid;
}oc_mqtt_paras;

oc_mqtt_paras g_mqtt_paras = {0};

extern char g_acRecvBuf[TS_RECV_BUFFER_LEN+128];
/*OC MQTT*/
static int ts_oc_mqtt_init(char *message, int len);
static int ts_oc_mqtt_register(char *message, int len);
static int ts_oc_mqtt_config(char *message, int len);
static int ts_oc_mqtt_json_fmt_req(char *message, int len);
static int ts_oc_mqtt_json_fmt_res(char *message, int len);
static int ts_oc_mqtt_publish(char *message, int len);
static int ts_oc_mqtt_deconfig(char *message, int len);
static int ts_oc_mqtt_deinit(char *message, int len);
static int ts_oc_mqtt_getvalue(char *message, int len);


test_entry ts_oc_mqtt_entry_flist[]= {
    ts_oc_mqtt_init,
    ts_oc_mqtt_register,
    ts_oc_mqtt_config,
    ts_oc_mqtt_json_fmt_req,
    ts_oc_mqtt_json_fmt_res,
    ts_oc_mqtt_publish,
    ts_oc_mqtt_deconfig,
    ts_oc_mqtt_deinit,
    ts_oc_mqtt_getvalue,
};

int ts_sort_oc_mqtt_al(int entry_id, char *message, int len)
{
    if(entry_id >= TEST_OC_MQTT_INVALID)
    {
        printf("invalid entry_id(%d)\n",entry_id);
        return TS_FAILED;
    }

    return (ts_oc_mqtt_entry_flist[entry_id])(message, len);
}

/*--------------oc mqtt test---------------------------*/

static queue_t *s_queue_rcvmsg = NULL;
static void    *task_handle = NULL;

typedef struct
{
    int        qos;
    int        dup;
    int        retain;
    int        msg_len;
    char      *topic;
    uint8_t   *msg;
    uint8_t   *buf;
}demo_msg_t;

static int pp_oc_app_msg_deal(void *arg,mqtt_al_msgrcv_t *msg)
{
    int ret = -1;
    uint8_t  *buf;
    uint32_t  buflen;
    demo_msg_t *demo_msg;

    buflen =msg->msg.len + msg->topic.len + sizeof(demo_msg_t) + 1 +1;
    buf = osal_malloc(buflen);
    if(NULL != buf)
    {
        ///< copy the message and push it to the queue
        demo_msg = (demo_msg_t *)buf;
        demo_msg->buf = buf + sizeof(demo_msg_t);
        demo_msg->dup = msg->dup;
        demo_msg->qos = msg->qos;
        demo_msg->retain = msg->retain;

        demo_msg->topic = (char *) demo_msg->buf;
        buf = (uint8_t *)demo_msg->topic;
        memcpy(buf,msg->topic.data,msg->topic.len);
        buf[msg->topic.len] = '\0';

        demo_msg->msg = demo_msg->buf + msg->topic.len +1;
        demo_msg->msg_len = msg->msg.len;
        buf = demo_msg->msg;
        memcpy(buf,msg->msg.data,msg->msg.len);
        buf[msg->msg.len] = '\0';

        printf("RCVMSG:qos:%d dup:%d retain:%d topic:%s msg:len:%d payload:%s\n\r",\
                demo_msg->qos,demo_msg->dup,demo_msg->retain,\
                demo_msg->topic,demo_msg->msg_len,demo_msg->msg);

        ret = queue_push(s_queue_rcvmsg,demo_msg,10);
        if(ret != 0)
        {
            osal_free(demo_msg);
        }
    }

    return ret;
}

static int  pp_oc_cmd_normal(demo_msg_t *demo_msg)
{

    int ret = 0;
    char   *buf = NULL;  ///< used for the mqtt

    cJSON               *mid_json;
    cJSON               *cmd_json;
    cJSON  *ioswitch = NULL;
    cJSON  *paras = NULL;
    cJSON               *response_msg;
    tag_oc_mqtt_response response;
    tag_key_value_list   list;
    int mid_int = 0;
    int err_int = 0;
    //////////////HANDLE YOUR MESSAGE HERE WITH YOUR DEVICE PROFILE///////////
    cmd_json = cJSON_Parse((const char *)demo_msg->msg);   
    if(NULL != cmd_json)
    {
        paras = cJSON_GetObjectItem(cmd_json,"paras");
        if(NULL != paras)
        {
            ioswitch = cJSON_GetObjectItem(paras,"ioswitch");
            if(NULL != ioswitch)
            {
                printf("last ioswitch is %d, newer ioswitch:%d\n\r",g_ioswitch, ioswitch->valueint);
                g_ioswitch = ioswitch->valueint;
            }
            else
            {
                printf("handle the json data as your specific profile\r\n");
                err_int = en_oc_mqtt_err_parafmt;
            }
        }
    }
    
    mid_json = cJSON_GetObjectItem(cmd_json,"mid");
    if(NULL != mid_json)
    {
        mid_int = mid_json->valueint;
    }
    cJSON_Delete(cmd_json);
    //////////////DO THE RESPONSE FOR THE COMMAND/////////////////////////////
    list.item.name = "body_para";
    list.item.buf = "body_para";
    list.item.type = en_key_value_type_string;
    list.next = NULL;

    response.hasmore = 0;
    response.errcode = err_int;
    response.mid = mid_int;
    response.bodylst = &list;

    response_msg = oc_mqtt_json_fmt_response(&response);
    if(NULL != response_msg)
    {
        buf = cJSON_PrintUnformatted(response_msg);
        if(NULL != buf)
        {
            ret = oc_mqtt_publish(NULL, (uint8_t *)buf,strlen(buf),en_mqtt_al_qos_1);
            printf("%s:RESPONSE:mid:%d err_int:%d retcode:%d \r\n",__FUNCTION__,\
                    mid_int,err_int,ret);

            osal_free(buf);
        }
        cJSON_Delete(response_msg);
    }

    return 0;
}

static int task_rcvmsg_entry( void *args)
{

    demo_msg_t *demo_msg;

    while(1)
    {
        demo_msg = NULL;
        queue_pop(s_queue_rcvmsg,(void **)&demo_msg,cn_osal_timeout_forever);

        if(NULL != demo_msg)
        {
            pp_oc_cmd_normal(demo_msg);  ///< this is the old model
            osal_free(demo_msg);
        }
    }

    return 0;
}


static int pp_oc_report(void)
{
    int ret = -1;
    tag_oc_mqtt_report  report;
    tag_key_value_list  lst;
    cJSON *root = NULL;
    char  *buf = NULL;
    static int leftpower = 0;

    leftpower = (leftpower + 7 )%100;

    lst.item.name = "batteryLevel";
    lst.item.buf = (char *)&leftpower;
    lst.item.len = sizeof(leftpower);
    lst.item.type = en_key_value_type_int;
    lst.next = NULL;

    report.hasmore = en_oc_mqtt_has_more_no;
    report.paralst= &lst;
    report.serviceid = "Battery";
    report.eventtime = NULL;

    root = oc_mqtt_json_fmt_report(&report);
    if(NULL != root)
    {
        buf = cJSON_Print(root);
        if(NULL != buf)
        {
            ret = oc_mqtt_publish(NULL,buf,strlen(buf),en_mqtt_al_qos_1);
            osal_free(buf);
        }

        cJSON_Delete(root);
    }

    return ret;
}



static int pp_oc_config_test(oc_mqtt_config_t *config)
{
    printf("this is at_oc_config\n");
    return 0;
}

static int pp_oc_deconfig_test()
{
    printf("this is at_oc_deconfig\n");
    return 0;
}

static int pp_oc_publish_test(char *topic,uint8_t *msg,int msg_len,int qos)
{
    printf("this is at_oc_report_test\n");
    return 0;
}

int ts_oc_mqtt_init(char *message, int len)
{
    s_queue_rcvmsg = queue_create("queue_rcvmsg",2,1);
    task_handle = osal_task_create("task_rcvmsg",task_rcvmsg_entry,NULL,0x1000,NULL,8);
    
    return oc_mqtt_init();
}

/*ts_oc_mqtt_config must be before ts_oc_mqtt_config*/
static oc_mqtt_t s_oc_mqtt_ops_test = \
{
    .name = "oc_mqtt_tiny",
    .op =
    {
        .config = pp_oc_config_test,
        .deconfig = pp_oc_deconfig_test,
        .publish = pp_oc_publish_test,
    },
};

static int ts_oc_mqtt_register(char *message, int len)
{
    int ret = 0;
    int retCode;
    
    
    retCode = oc_mqtt_register(&s_oc_mqtt_ops_test);
    ret -= (!(retCode == en_oc_mqtt_err_ok));
    /*register again, will sucess*/
    retCode = oc_mqtt_register(&s_oc_mqtt_ops_test);
    ret -= (!(retCode == en_oc_mqtt_err_ok));

    retCode = oc_mqtt_register(NULL);
    ret -= (!(retCode == en_oc_mqtt_err_system));

    
    oc_mqtt_tiny_install();

    osal_task_sleep(500);
    

    printf("exit from %s\n", __FUNCTION__);
    
    return ret;
}

static int ts_oc_mqtt_config(char *message, int len)
{

    oc_mqtt_config_t config;

    char *pchTmp, *pchStrTmpIn;
    oc_mqtt_paras *pparas = &g_mqtt_paras;
    int retcode;
    

    
    printf("[%s] g_tempbuf is %s\n", __FUNCTION__, message);
    pchTmp      = message;
    pchStrTmpIn = NULL;
    
    pchTmp = strtok_r(pchTmp, "|", &pchStrTmpIn);
    printf("[%s]id is %d\n",__FUNCTION__,atoi(pchTmp));
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    printf("[%s]call name is %s\n",__FUNCTION__,pchTmp);


    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    pparas->boot_mode = atoi(pchTmp);
    printf("165--boot_mode is %d\n",pparas->boot_mode);

    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    pparas->life_time = atoi(pchTmp);
    printf("165--life_time is %d\n",pparas->life_time);

    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    printf("168--oc server ipv4 is %s\n",pchTmp);
    if(pparas->server_ip4 != NULL) 
    {
        osal_free(pparas->server_ip4);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->server_ip4 = NULL;
    }
    else
    {
        pparas->server_ip4 = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->server_ip4, pchTmp, strlen(pchTmp));
        pparas->server_ip4[strlen(pchTmp)] = '\0';
    }

    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    printf("168--oc server port is %s\n",pchTmp);
    if(pparas->server_port != NULL) 
    {
        osal_free(pparas->server_port);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->server_port = NULL;
    }
    else
    {
        pparas->server_port = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->server_port, pchTmp, strlen(pchTmp));
        pparas->server_port[strlen(pchTmp)] = '\0';
    }
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    pparas->sec_type = atoi(pchTmp);
    printf("165--sec_type is %d\n",pparas->sec_type);
    
    
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    printf("[%s]ep name is %s\n", __FUNCTION__, pchTmp);
    if(pparas->id != NULL) 
    {
        osal_free(pparas->id);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->id = NULL;
    }
    else
    {
        pparas->id = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->id, pchTmp, strlen(pchTmp));
        pparas->id[strlen(pchTmp)] = '\0';
        printf("120--id is %s\n",pparas->id);
    }
    
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    if(pparas->passwd != NULL) 
    {
        osal_free(pparas->passwd);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->passwd = NULL;
    }
    else
    {
        pparas->passwd = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->passwd, pchTmp, strlen(pchTmp));
        pparas->passwd[strlen(pchTmp)] = '\0';
        printf("140--passwd is %s\n",pparas->passwd);
    }

    

    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    
    if(pparas->cbname != NULL) 
    {
        osal_free(pparas->cbname);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->cbname = NULL;
    }
    else
    {
        pparas->cbname = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->cbname, pchTmp, strlen(pchTmp));
        pparas->cbname[strlen(pchTmp)] = '\0';
        printf("125--cbname is %s\n",pparas->cbname);
    }
    

    config.boot_mode    = pparas->boot_mode;
    config.lifetime     = pparas->life_time;
    config.server_addr  = pparas->server_ip4;
    config.server_port  = pparas->server_port;
    config.sec_type     = pparas->sec_type;
    config.id           = pparas->id;
    config.pwd          = pparas->passwd;
    
    config.msg_deal     = (pparas->cbname ? pp_oc_app_msg_deal : NULL);
    config.msg_deal_arg = NULL;
    
    if(pparas->cbname)printf("[ts_oc_mqtt_config]pparas->cbname is %s\n",pparas->cbname);
    

    retcode = oc_mqtt_config(&config);
    printf("[call oc_mqtt_config] retcode is %d\n",retcode);
    return retcode;
}

static int ts_oc_mqtt_json_fmt_req(char *message, int len)
{
    int ret = 0;
    tag_oc_mqtt_report  report;
    tag_key_value_list  lst1;
	tag_key_value_list  lst2;
	tag_key_value_list  lst3;
	tag_key_value_list  lst4;
    cJSON *root;
    char  *buf;
    char *pchTmp, *pchStrTmpIn = NULL;

    printf("[ts_oc_mqtt_json_fmt_req]recve msg: %s,%d\n",message,len);
    
    pchTmp = message;

    /*id*/
    pchTmp      = strtok_r(pchTmp, "|", &pchStrTmpIn);
    /*func name*/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    /*service id*/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    char *serviceId = osal_malloc(strlen(pchTmp)+1);
    memcpy(serviceId, pchTmp, strlen(pchTmp));
    serviceId[strlen(pchTmp)] = 0;
    printf("serviceId is %s\n", serviceId);
    /*hasMore*/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    int hasMore = atoi(pchTmp);
    printf("hasMore is %d\n", hasMore);
    
    /*item1**/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    char *item1 = osal_malloc(strlen(pchTmp)+1);
    memcpy(item1, pchTmp, strlen(pchTmp));
    item1[strlen(pchTmp)] = 0;
    printf("item1 is %s\n", item1);
    
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    int valuetype1 = atoi(pchTmp);
    printf("valuetype1 is %d\n", valuetype1);

    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    char *svalue1 = NULL;
    int value1;
    lst1.item.name = item1;//"LED1";
    lst1.item.type = valuetype1; //en_key_value_type_int;
    
    if(valuetype1 == en_key_value_type_int)
    {
        value1 = atoi(pchTmp);
        lst1.item.buf = (char *)&value1;
        lst1.item.len = sizeof(int);
    }
    else if(valuetype1 == en_key_value_type_string)
    {
        svalue1 = osal_malloc(strlen(pchTmp)+1);
        memcpy(svalue1, pchTmp, strlen(pchTmp));
        svalue1[strlen(pchTmp)] = 0;
        lst1.item.buf = svalue1;
        lst1.item.len = strlen(pchTmp);
    }
    else if(valuetype1 == en_key_value_type_array)/* not test now*/
    {
        printf("not test now !!\n");
    }
    else
    {
        printf("unsupport now !!\n");
    }
    lst1.next = &lst2;

    /*item2**/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    char *item2 = osal_malloc(strlen(pchTmp)+1);
    memcpy(item2, pchTmp, strlen(pchTmp));
    item2[strlen(pchTmp)] = 0;
    printf("item2 is %s\n", item2);
    
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    int valuetype2 = atoi(pchTmp);
    printf("valuetype2 is %d\n", valuetype2);

    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    char *svalue2 = NULL;
    int value2;
    lst2.item.name = item2;//"LED1";
    lst2.item.type = valuetype2; //en_key_value_type_int;
    
    if(valuetype2 == en_key_value_type_int)
    {
        value2 = atoi(pchTmp);
        lst2.item.buf = (char *)&value2;
        lst2.item.len = sizeof(int);
    }
    else if(valuetype2 == en_key_value_type_string)
    {
        svalue2 = osal_malloc(strlen(pchTmp)+1);
        memcpy(svalue2, pchTmp, strlen(pchTmp));
        svalue2[strlen(pchTmp)] = 0;
        lst2.item.buf = svalue2;
        lst2.item.len = strlen(pchTmp);
    }
    else if(valuetype2 == en_key_value_type_array)/* not test now*/
    {
        printf("not test now !!\n");
    }
    else
    {
        printf("unsupport now !!\n");
    }
    lst2.next = &lst3;

    /*item3**/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    char *item3 = osal_malloc(strlen(pchTmp)+1);
    memcpy(item3, pchTmp, strlen(pchTmp));
    item3[strlen(pchTmp)] = 0;
    printf("item3 is %s\n", item3);
    
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    int valuetype3 = atoi(pchTmp);
    printf("valuetype3 is %d\n", valuetype3);

    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    char *svalue3 = NULL;
    int value3;
    lst3.item.name = item3;//"LED1";
    lst3.item.type = valuetype3; //en_key_value_type_int;
    
    if(valuetype3 == en_key_value_type_int)
    {
        value3 = atoi(pchTmp);
        lst3.item.buf = (char *)&value3;
        lst3.item.len = sizeof(int);
    }
    else if(valuetype3 == en_key_value_type_string)
    {
        svalue3 = osal_malloc(strlen(pchTmp)+1);
        memcpy(svalue3, pchTmp, strlen(pchTmp));
        svalue3[strlen(pchTmp)] = 0;
        lst3.item.buf = svalue3;
        lst3.item.len = strlen(pchTmp);
    }
    else if(valuetype3 == en_key_value_type_array)/* not test now*/
    {
        printf("not test now !!\n");
    }
    else
    {
        printf("unsupport now !!\n");
    }
    lst3.next = &lst4;

    /*item4**/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    char *item4 = osal_malloc(strlen(pchTmp)+1);
    memcpy(item4, pchTmp, strlen(pchTmp));
    item4[strlen(pchTmp)] = 0;
    printf("item4 is %s\n", item4);
    
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    int valuetype4 = atoi(pchTmp);
    printf("valuetype4 is %d\n", valuetype1);

    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    char *svalue4 = NULL;
    int value4;
    lst4.item.name = item4;//"LED1";
    lst4.item.type = valuetype4; //en_key_value_type_int;
    
    if(valuetype4 == en_key_value_type_int)
    {
        value4 = atoi(pchTmp);
        lst4.item.buf = (char *)&value4;
        lst4.item.len = sizeof(int);
    }
    else if(valuetype4 == en_key_value_type_string)
    {
        svalue4 = osal_malloc(strlen(pchTmp)+1);
        memcpy(svalue4, pchTmp, strlen(pchTmp));
        svalue4[strlen(pchTmp)] = 0;
        lst4.item.buf = svalue4;
        lst4.item.len = strlen(pchTmp);
    }
    else if(valuetype4 == en_key_value_type_array)/* not test now*/
    {
        printf("not test now !!\n");
    }
    else
    {
        printf("unsupport now !!\n");
    }
    lst4.next = NULL;


    report.hasmore = (en_oc_mqtt_has_more)hasMore;//en_oc_mqtt_has_more_no;
    report.paralst= &lst1;
    report.serviceid = serviceId;
    report.eventtime = NULL;
    root = oc_mqtt_json_fmt_report(&report);
    ret -= (!(root != NULL));
    if(root != NULL)
    {
        buf = cJSON_Print(root);
        ret -= (!(buf != NULL));

        cJSON_Delete(root);
        if(buf != NULL)
        {
            
            if(strlen(buf) < TS_RECV_BUFFER_LEN)
            {
                memcpy(message,buf,strlen(buf));
                message[strlen(buf)] = '\0';
            }
            printf("req buf(len %d) is %s\n", (int)strlen(message),message);
            osal_free(buf);
        }
        
    }

    if(serviceId != NULL)osal_free(serviceId);
    if(svalue1 != NULL)osal_free(svalue1);
    if(svalue2 != NULL)osal_free(svalue2);
    if(svalue3 != NULL)osal_free(svalue3);
    if(svalue4 != NULL)osal_free(svalue4);
    
    if(ret == 0)
        return TS_OK_HAS_DATA;
    else
        return ret;
}

static int ts_oc_mqtt_json_fmt_res(char *message, int len)
{
    int ret = 0;
    //int retCode;
    tag_oc_mqtt_response response;
    tag_key_value_list   list;

    cJSON  *root;
    char   *buf;
    
    char *pchTmp, *pchStrTmpIn = NULL;

    printf("[ts_oc_mqtt_json_fmt_req]recve msg: %s,%d\n",message,len);
    
    pchTmp = message;
    /*id*/
    pchTmp      = strtok_r(pchTmp, "|", &pchStrTmpIn);
    /*func name*/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    /*hasmore*/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    en_oc_mqtt_has_more hasMore = (en_oc_mqtt_has_more)atoi(pchTmp);

    /*errorcode*/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    int errCode = (int)atoi(pchTmp);
    /*mid*/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    int mid = (int)atoi(pchTmp);
    /*item*/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    char *item = osal_malloc(strlen(pchTmp)+1);
    memcpy(item, pchTmp, strlen(pchTmp));
    item[strlen(pchTmp)] = 0;
    printf("item is %s\n", item);
    /*value type*/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    int valuetype = atoi(pchTmp);
    printf("valuetype is %d\n", valuetype);
    
    list.item.name = item;
    list.item.type = valuetype;
    
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    int value;
    char *savlue = NULL;
    
    if(valuetype == en_key_value_type_int)
    {
        value = atoi(pchTmp);
        list.item.buf = (char *)&value;
        list.item.len = sizeof(int);
    }
    else if(valuetype == en_key_value_type_string)
    {
        savlue = osal_malloc(strlen(pchTmp)+1);
        memcpy(savlue, pchTmp, strlen(pchTmp));
        savlue[strlen(pchTmp)] = 0;
        list.item.buf = savlue;
        list.item.len = strlen(pchTmp);
    }
    else if(valuetype == en_key_value_type_array)/* not test now*/
    {
        printf("not test now !!\n");
    }
    else
    {
        printf("unsupport now !!\n");
    }
    list.next = NULL;
    /*
    typedef enum
    {
        en_oc_mqtt_has_more_no = 0,
        en_oc_mqtt_has_more_yes =1,
    }en_oc_mqtt_has_more;
    typedef enum
{
    en_oc_mqtt_err_code_ok = 0,
    en_oc_mqtt_err_code_err =1,
}en_oc_mqtt_err_code;
    typedef enum
    {
        en_key_value_type_int = 0,
        en_key_value_type_string,
        en_key_value_type_array,
    }en_value_type;

    */

    response.hasmore = hasMore;
    response.errcode = errCode;
    response.mid = mid;//1;
    response.bodylst = &list;
    
    root = oc_mqtt_json_fmt_response(&response);
    
    ret -= (!(root != NULL));
    if(root != NULL)
    {
        buf = cJSON_Print(root);
        ret -= (!(buf != NULL));

        cJSON_Delete(root);
        if(buf != NULL)
        {
            
            if(strlen(buf) < TS_RECV_BUFFER_LEN)
            {
                memcpy(message,buf,strlen(buf));
                message[strlen(buf)] = '\0';
            }
            printf("res buf(len %d) is %s\n", (int)strlen(message),message);
            osal_free(buf);
        }
        
    }
    if(ret == 0)
        return TS_OK_HAS_DATA;
    else
        return ret;
}



static int ts_oc_mqtt_publish(char *message, int len)
{
    int ret = 0;
    int retCode = 0;
    
    if(0 == retCode)
    {
        printf("CONNECT:SUCCESS\n\r");
    }
    else
    {
        printf("CONNECT:FAILED\n\r");
        goto DEMO_EXIT;
    }
    ret -= (!(retCode == 0));
    
    retCode = pp_oc_report();
    ret -= (!(retCode == 0));
    if(0 == retCode)
    {
        printf("REPORT:SUCCESS\n\r");
    }
    else
    {
        printf("REPORT:FAILED\n\r");
        goto DEMO_EXIT;
    }
    
    
    
DEMO_EXIT:
    if(0 == ret)
    {
        printf("EXIT test_oc_mqtt_report_static: PASS\n\r");
    }
    else
    {
        printf("EXIT test_oc_mqtt_report_static: FAILED ************\n\r");
    }
    return ret;
}

static int ts_oc_mqtt_deconfig(char *message, int len)
{
    int retCode = 0;
    oc_mqtt_paras *pparas = &g_mqtt_paras;
    
    retCode = oc_mqtt_deconfig();
    
    
    printf("exit from %s, retcode is %d\n", __FUNCTION__, retCode);
    if(pparas->id != NULL) 
    {
        osal_free(pparas->id);
        pparas->id = NULL;
    }
    if(pparas->passwd != NULL) 
    {
        osal_free(pparas->passwd);
        pparas->passwd = NULL;
    }
    
    if(pparas->server_ip4 != NULL) 
    {
        osal_free(pparas->server_ip4);
        pparas->server_ip4 = NULL;
    }
    if(pparas->server_port != NULL) 
    {
        osal_free(pparas->server_port);
        pparas->server_port = NULL;
    }
    if(pparas->cbname != NULL) 
    {
        osal_free(pparas->cbname);
        pparas->cbname = NULL;
    }
    return retCode;
}
static int ts_oc_mqtt_deinit(char *message, int len)
{
    osal_task_sleep(500);
    if(s_queue_rcvmsg && task_handle) 
    {
        osal_task_kill(task_handle);
        osal_task_sleep(500);
        queue_delete(s_queue_rcvmsg);
    }

    oc_mqtt_tiny_uninstall();
    
    printf("resource released!");
    return TS_OK;
}

static int ts_oc_mqtt_getvalue(char *message, int len)
{
    printf("[ts_oc_mqtt_getvalue]g_ioswitch is %d \n", g_ioswitch);
    memset(message, 0, 32);/*32 is more large than one int32*/
    snprintf(message,len,"%d",g_ioswitch);
    
    return TS_OK_HAS_DATA;
}


