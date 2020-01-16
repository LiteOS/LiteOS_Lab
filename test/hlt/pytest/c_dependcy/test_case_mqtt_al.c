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

#include <mqtt_al.h>

#include "test_case.h"

static char s_mqtt_ca_crt[] =
#if 0
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
#endif
"-----BEGIN CERTIFICATE-----\r\n"
"MIIDpDCCAowCCQCe+yNczGt4yDANBgkqhkiG9w0BAQsFADCBkzELMAkGA1UEBhMC\r\n"
"Y24xEDAOBgNVBAgMB2ppYW5nc3UxEDAOBgNVBAcMB25hbmppbmcxEDAOBgNVBAoM\r\n"
"B2hvcGVydW4xETAPBgNVBAsMCHl1aHVhdGFpMRYwFAYDVQQDDA0xOTIuMTY4LjEu\r\n"
"MTAzMSMwIQYJKoZIhvcNAQkBFhR4dV9saXF1bkBob3BlcnVuLmNvbTAeFw0xOTEy\r\n"
"MTEwMjU1MDNaFw0yMDAzMjAwMjU1MDNaMIGTMQswCQYDVQQGEwJjbjEQMA4GA1UE\r\n"
"CAwHamlhbmdzdTEQMA4GA1UEBwwHbmFuamluZzEQMA4GA1UECgwHaG9wZXJ1bjER\r\n"
"MA8GA1UECwwIeXVodWF0YWkxFjAUBgNVBAMMDTE5Mi4xNjguMS4xMDMxIzAhBgkq\r\n"
"hkiG9w0BCQEWFHh1X2xpcXVuQGhvcGVydW4uY29tMIIBIjANBgkqhkiG9w0BAQEF\r\n"
"AAOCAQ8AMIIBCgKCAQEA1l2V0Nf69DwLicsVaY/QgIrFhn/kerjnWQN1pvt6FpzC\r\n"
"TUel6dZUFS6aoA6TZzSQQ/qqymcZt6X4PSCEzWJ2msiFzXKB87hHkvqgVfsdA/9F\r\n"
"qpqZjavpSYId9dwSGRgkymWj9zZ+DA3BkPv5JwogV3QbGG++RQldZXfvhlbRQedq\r\n"
"CTffmvvx08zs/yi9MkN8MimXkiz6QpGZojbDG2aWfLsmpmhQZXmtx6ucJ5jN8Dn3\r\n"
"EBj/VFnMmoUuIr3PbnN9cIC0oofsIztPEgXjXe3JH1cOAFkQUrlw+RPJsu/v1+RE\r\n"
"Y3Tg/0SfJuIrEQiuHsLd0q3TGs5KzaQYPQtFgQf3BwIDAQABMA0GCSqGSIb3DQEB\r\n"
"CwUAA4IBAQA4N0L9iz1FRXno78pwMyBiORzkBgJfDtphmZJ9exsgnF4P9WeisQPo\r\n"
"HYEDBGuDbxk+1wEWdQ1qlLr+OcQ+mXm3Wm2OayG5dKbrA/WnhGVR/38GG6MQQDZO\r\n"
"6ghrN8Jm6w8koDIhYT8wG9gbRDtGdxGsYbbwW76sTkOFCq8JjYM3iH7rlWowamaG\r\n"
"shkb3d/TvgWZ5WIbDhuw2GVU0+Tmq4Rq2LSHDFR07GF2kRWM0vdik/I7nYMB4+O7\r\n"
"WnhRK8uDhlIL9BefI7nrF8Z3E5jFU6YpVb9W53+Z86D7AnGlt6jqIqFjsRfAKGjf\r\n"
"OQHIVpyGfDqi4kIiKB1wPhw6PLHnCSO7\r\n"
"-----END CERTIFICATE-----\r\n";



//#define DEFAULT_SERVER_IPV4         "49.4.93.24"     ///<  server ip address
//#define DEFAULT_SERVER_IPV4         "192.168.1.103"     ///<  server ip address
//#define DEFAULT_SERVER_PORT         "8883"           ///<  server mqtt service port
#define CN_OC_MQTT_CLIENTID_STATIC_FMT     "%s_%d_%d_%s"
#define cn_check_time_value                 "2018111517"
static void *g_mqtt_al_handle = NULL;

typedef struct mqtt_al_paras_s{
    char *client;
    char *passwd;
    char *clientdata;
    char *server_ip4;
    char *server_port;
    char *cavalid;
    char *subtopic;
    char *unsubtopic;
    char *pubtopic;
    char *pubmsg;
}mqtt_al_paras;

static mqtt_al_paras  g_mqtt_al_paras = {0};


/*MQTT AL*/
static int ts_mqtt_al_init(char *message, int len);
static int ts_mqtt_al_install(char *message, int len);
static int ts_mqtt_al_uninstall(char *message, int len);
static int ts_mqtt_al_connect(char *message, int len);
static int ts_mqtt_al_disconnect(char *message, int len);
static int ts_mqtt_al_subscrible(char *message, int len);
static int ts_mqtt_al_unsubscrible(char *message, int len);
static int ts_mqtt_al_publish(char *message, int len);
static int ts_mqtt_al_checkstatus(char *message, int len);
static int ts_mqtt_al_deinit(char *message, int len);


test_entry ts_mqtt_al_entry_flist[]= {
    ts_mqtt_al_init,
    ts_mqtt_al_install,
    ts_mqtt_al_uninstall,
    ts_mqtt_al_connect,
    ts_mqtt_al_disconnect,
    ts_mqtt_al_subscrible,
    ts_mqtt_al_unsubscrible,
    ts_mqtt_al_publish,
    ts_mqtt_al_checkstatus,
    ts_mqtt_al_deinit,
};

int ts_sort_mqtt_al(int entry_id, char *message, int len)
{
    if(entry_id > 9)
    {
        printf("invalid entry_id(%d)\n",entry_id);
        return TS_FAILED;
    }
    return (ts_mqtt_al_entry_flist[entry_id])(message, len);
}

/*--------------mqtt_al test---------------------------*/
static int ts_mqtt_al_init(char *message, int len)
{
    char *pchTmp, *pchStrTmpIn;
    mqtt_al_paras *pparas = &g_mqtt_al_paras;
    
    printf("[%s] message is %s\n", __FUNCTION__, message);
    pchTmp      = message;
    pchStrTmpIn = NULL;
    
    pchTmp = strtok_r(pchTmp, "|", &pchStrTmpIn);
    printf("[%s]id is %d\n",__FUNCTION__,atoi(pchTmp));
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    printf("[%s]call name is %s\n",__FUNCTION__,pchTmp);


    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    printf("[%s]server ip is %s\n", __FUNCTION__, pchTmp);
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
        printf("[%s]server ip is %s\n", __FUNCTION__, pparas->server_ip4);
    }
    

    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    
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
        printf("[%s]server port is %s\n", __FUNCTION__, pparas->server_port);
    }
    
    
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    
    if(pparas->client != NULL) 
    {
        osal_free(pparas->client);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->client = NULL;
    }
    else
    {
        pparas->client = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->client, pchTmp, strlen(pchTmp));
        pparas->client[strlen(pchTmp)] = '\0';
        printf("[%s]ep name is %s\n", __FUNCTION__, pparas->client);
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

        printf("[%s]ep passwd is %s\n", __FUNCTION__, pparas->passwd);
    }
    
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    if(pparas->clientdata != NULL)
    {
        osal_free(pparas->clientdata);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->clientdata = NULL;
    }
    else
    {
        int tlen = strlen(pparas->client) +
            strlen(cn_check_time_value) + strlen(CN_OC_MQTT_CLIENTID_STATIC_FMT) + 1;
    
        pparas->clientdata = osal_malloc(tlen);
    
        snprintf(pparas->clientdata, tlen, CN_OC_MQTT_CLIENTID_STATIC_FMT, pparas->client,\
            2/*en_mqtt_auth_type_nodeid*/, 0/*en_mqtt_sign_type_hmacsha256_check_time_no*/, cn_check_time_value);

        printf("[%s]clientdata is %s\n", __FUNCTION__, pparas->clientdata);
    }
    
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    if(pparas->cavalid != NULL)
    {
        osal_free(pparas->cavalid);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->cavalid = NULL;
    }
    else
    {
        pparas->cavalid = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->cavalid, pchTmp, strlen(pchTmp));
        pparas->cavalid[strlen(pchTmp)] = '\0';

        printf("140--cavalid is %s\n",pparas->cavalid);
    }

    
    
    return mqtt_al_init();
}

static int ts_mqtt_al_install(char *message, int len)
{
    int ret = 0;
    int retCode;
    
    mqtt_al_uninstall();
#if CONFIG_MQTT_PAHO_ENABLE
    #include <paho_mqtt_port.h>
    retCode = mqtt_install_pahomqtt();
#elif CONFIG_MQTT_SINN_ENABLE
    #include <mqtt_sinn_port.h>
    retCode = mqtt_install_sinnmqtt();
#endif

    ret -= (!(retCode == 0));
    /*again install ,will failed*/
#if CONFIG_MQTT_PAHO_ENABLE
    #include <paho_mqtt_port.h>
    retCode = mqtt_install_pahomqtt();
#elif CONFIG_MQTT_SINN_ENABLE
    #include <mqtt_sinn_port.h>
    retCode = mqtt_install_sinnmqtt();
#endif

    ret -= (!(retCode == -1));

    return ret;

}
static int ts_mqtt_al_uninstall(char *message, int len)
{
    int ret = 0;
    int retCode;
    
    mqtt_al_uninstall();
#if CONFIG_MQTT_PAHO_ENABLE
    #include <paho_mqtt_port.h>
    mqtt_install_pahomqtt();
#elif CONFIG_MQTT_SINN_ENABLE
    #include <mqtt_sinn_port.h>
    mqtt_install_sinnmqtt();
#endif

    retCode = mqtt_al_uninstall();
    ret -= (!(retCode == 0));

    retCode = mqtt_al_uninstall();
    ret -= (!(retCode == -1));

    return ret;
    
}


static int ts_mqtt_al_connect(char *message, int len)
{
    mqtt_al_conpara_t conpara;
    mqtt_al_security_para_t security;

    int tlen;
    en_mqtt_al_connect_state state;
    mqtt_al_paras *pparas = &g_mqtt_al_paras;
    
    memset(&conpara,0,sizeof(conpara));
    
    conpara.clientid.data = pparas->clientdata;
    conpara.clientid.len = strlen(conpara.clientid.data);

    conpara.user.data =  pparas->client;
    conpara.user.len = strlen(conpara.user.data);

    conpara.passwd.data = pparas->passwd;
    conpara.passwd.len = strlen(conpara.passwd.data);

    conpara.cleansession = 1;
    conpara.keepalivetime = 10;
    security.type = en_mqtt_al_security_cas;
    security.u.cas.ca_crt.data = s_mqtt_ca_crt;
    security.u.cas.ca_crt.len = (strlen(security.u.cas.ca_crt.data)+1); ///< must including the end '\0'
 
    conpara.security = pparas->cavalid == NULL ? NULL : &security;

    conpara.serveraddr.data = pparas->server_ip4;
    conpara.serveraddr.len = strlen(conpara.serveraddr.data);
    conpara.serverport = atoi(pparas->server_port);
    conpara.timeout = 10000;
    conpara.version = en_mqtt_al_version_3_1_1;
    conpara.willmsg = NULL;

    printf("SERVER:%s \r\n PORT:%d \r\n",\
            conpara.serveraddr.data,conpara.serverport);
    printf("CLIENTID:%s \r\n USER:%s \r\n PASSWD:%s \r\n",\
            conpara.clientid.data,conpara.user.data,conpara.passwd.data);

    g_mqtt_al_handle = mqtt_al_connect(&conpara);
    printf("conpara.conret is %d\n", conpara.conret);
    return conpara.conret;
}

static int ts_mqtt_al_disconnect(char *message, int len)
{
    int ret = 0;
    int retCode;
    retCode = mqtt_al_disconnect(g_mqtt_al_handle);
    if(retCode == 0)
    {
        g_mqtt_al_handle = NULL;
    }
    ret -= (!(retCode==0));

    /*disconnect again*/
    retCode = mqtt_al_disconnect(g_mqtt_al_handle);
    if(retCode == 0)
    {
        g_mqtt_al_handle = NULL;
    }
    ret -= (!(retCode==-1));
    
    return ret;
}
static void test_msg_default_deal(void *arg, mqtt_al_msgrcv_t  *msg)
{
    printf("come into %s, topic is %s\n", __FUNCTION__, msg->topic.data);
}

static int ts_mqtt_al_subscrible(char *message, int len)
{
    
    int ret = -1;
    mqtt_al_subpara_t   subpara;
    char *pchTmp, *pchStrTmpIn;
    mqtt_al_paras *pparas = &g_mqtt_al_paras;

    printf("[%s] message is %s\n", __FUNCTION__, message);
    pchTmp      = message;
    pchStrTmpIn = NULL;
    
    pchTmp = strtok_r(pchTmp, "|", &pchStrTmpIn);
    printf("[%s]id is %d\n",__FUNCTION__,atoi(pchTmp));
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    printf("[%s]call name is %s\n",__FUNCTION__,pchTmp);


    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    
    if(pparas->subtopic != NULL) 
    {
        osal_free(pparas->subtopic);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->subtopic = NULL;
    }
    else
    {
        pparas->subtopic = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->subtopic, pchTmp, strlen(pchTmp));
        pparas->subtopic[strlen(pchTmp)] = '\0';
        printf("[%s]sub topic is %s\n", __FUNCTION__, pparas->subtopic);
    }

    memset(&subpara,0,sizeof(subpara));
    memset(&subpara,0,sizeof(subpara));

    subpara.dealer = test_msg_default_deal;
    subpara.arg = NULL;
    subpara.qos = en_mqtt_al_qos_1;
    subpara.topic.data = pparas->subtopic;
    subpara.topic.len = strlen(subpara.topic.data);
    
    ret = mqtt_al_subscribe(g_mqtt_al_handle,&subpara);
    
    return ret;
}

static int ts_mqtt_al_unsubscrible(char *message, int len)
{
    char *pchTmp, *pchStrTmpIn;
    char *unsubtopic;

    mqtt_al_unsubpara_t unsub;
    int ret;
    mqtt_al_paras *pparas = &g_mqtt_al_paras;

    printf("[%s] message is %s\n", __FUNCTION__, message);
    pchTmp      = message;
    pchStrTmpIn = NULL;
    
    pchTmp = strtok_r(pchTmp, "|", &pchStrTmpIn);
    printf("[%s]id is %d\n",__FUNCTION__,atoi(pchTmp));
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    printf("[%s]call name is %s\n",__FUNCTION__,pchTmp);


    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    
    if(pparas->unsubtopic != NULL) 
    {
        osal_free(pparas->unsubtopic);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->unsubtopic = NULL;
    }
    else
    {
        pparas->unsubtopic = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->unsubtopic, pchTmp, strlen(pchTmp));
        pparas->unsubtopic[strlen(pchTmp)] = '\0';
        printf("[%s]unsub topic is %s\n", __FUNCTION__, pparas->unsubtopic);
    }

    unsub.topic.data = pparas->unsubtopic;
    unsub.topic.len  = strlen(unsub.topic.data);
    
    ret = mqtt_al_unsubscribe(g_mqtt_al_handle,&unsub);
    
    return ret;
}

static int ts_mqtt_al_publish(char *message, int len)
{
    mqtt_al_pubpara_t pubpara;
    int ret;

    char *pchTmp, *pchStrTmpIn;
    mqtt_al_paras *pparas = &g_mqtt_al_paras;

    printf("[%s] message is %s\n", __FUNCTION__, message);
    pchTmp      = message;
    pchStrTmpIn = NULL;
    
    pchTmp = strtok_r(pchTmp, "|", &pchStrTmpIn);
    printf("[%s]id is %d\n",__FUNCTION__,atoi(pchTmp));
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    printf("[%s]call name is %s\n",__FUNCTION__,pchTmp);


    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    
    if(pparas->pubtopic != NULL) 
    {
        osal_free(pparas->pubtopic);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->pubtopic = NULL;
    }
    else
    {
        pparas->pubtopic = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->pubtopic, pchTmp, strlen(pchTmp));
        pparas->pubtopic[strlen(pchTmp)] = '\0';
        printf("[%s]pub topic is %s\n", __FUNCTION__, pparas->pubtopic);
    }
    
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);

    if(pparas->pubmsg != NULL) 
    {
        osal_free(pparas->pubmsg);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->pubmsg = NULL;
    }
    else
    {
        pparas->pubmsg = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->pubmsg, pchTmp, strlen(pchTmp));
        pparas->pubmsg[strlen(pchTmp)] = '\0';
        printf("[%s]pub msg is %s\n", __FUNCTION__, pparas->pubmsg);
    }
    

    ///< pub the mqtt request
    memset(&pubpara, 0, sizeof(pubpara));
    pubpara.qos = en_mqtt_al_qos_1;
    pubpara.retain = 0;
    pubpara.timeout = 1000;


    pubpara.topic.data = pparas->pubtopic;
    pubpara.topic.len =strlen(pubpara.topic.data);
    pubpara.msg.data = (char *)pparas->pubmsg;
    pubpara.msg.len = strlen(pparas->pubmsg);

    printf("oc_mqtt_publish:topic:%s  qos:%d msglen:%d\n\r",pubpara.topic.data,pubpara.qos,pubpara.msg.len);

    ret = mqtt_al_publish(g_mqtt_al_handle, &pubpara);
    
    return ret;
}

static int ts_mqtt_al_checkstatus(char *message, int len)
{
    int ret = 0;
    en_mqtt_al_connect_state state;
    
    state = mqtt_al_check_status(g_mqtt_al_handle);
    
    return (int)state;
}


static int ts_mqtt_al_deinit(char *message, int len)
{
    mqtt_al_paras *pparas = &g_mqtt_al_paras;
    
    if(pparas->client)
        osal_free(pparas->client);
    if(pparas->clientdata)
        osal_free(pparas->clientdata);

    if(pparas->cavalid)
        osal_free(pparas->cavalid);
    
    if(pparas->passwd)
        osal_free(pparas->passwd);

    if(pparas->server_ip4)
        osal_free(pparas->server_ip4);

    if(pparas->server_port)
        osal_free(pparas->server_port);

    if(pparas->subtopic)
        osal_free(pparas->subtopic);
    
    if(pparas->unsubtopic)
        osal_free(pparas->unsubtopic);

    if(pparas->pubtopic)
        osal_free(pparas->pubtopic);

    if(pparas->pubmsg)
        osal_free(pparas->pubmsg);

    return TS_OK;
}






