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
 *  2019-08-05 17:38  zhangqianfu  The first version
 *
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>


#include <osal.h>
#include <oc_mqtt_al.h>


#include <shell.h>



/* brief : the oceanconnect platform only support the ca_crt up tills now*/
/** the address product_id device_id password crt is only for the test  */

#define DEFAULT_LIFETIME            10
#define DEFAULT_SERVER_IPV4         "49.4.93.24"     ///<  server ip address
#define DEFAULT_SERVER_PORT         "8883"           ///<  server mqtt service port
#define CN_MQTT_EP_NOTEID           "mqtt_sdk01"
#define CN_MQTT_EP_DEVICEID         "6e4a90d5-8e31-48c0-920a-d03c6e91d923"
#define CN_MQTT_EP_PASSWD           "c18f10422c93548e6fef"


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

//if your command is very fast,please use a queue here--TODO
#define cn_app_rcv_buf_len 256
static char            s_rcv_buffer[cn_app_rcv_buf_len];
static int             s_rcv_datalen;
static osal_semp_t     s_rcv_sync;
static void           *s_mqtt_handle;
//use this function to push all the message to the buffer
static int app_msg_deal(void *handle,mqtt_al_msgrcv_t *msg)
{
    int ret = -1;
    printf("topic:%s qos:%d\n\r",msg->topic.data,msg->qos);

    if(msg->msg.len < cn_app_rcv_buf_len)
    {
        memcpy(s_rcv_buffer,msg->msg.data,msg->msg.len );
        s_rcv_buffer[msg->msg.len] = '\0'; ///< the json need it
        s_rcv_datalen = msg->msg.len;

        printf("msg:%s\n\r",s_rcv_buffer);

        osal_semp_post(s_rcv_sync);
        ret = 0;

    }
    return ret;
}


static int oc_mqtt_cmd_entry( void *args)
{
    cJSON  *msg = NULL;
    cJSON  *mid = NULL;
    cJSON  *ioswitch = NULL;
    cJSON  *msgType = NULL;
    cJSON  *paras = NULL;
    cJSON  *serviceId = NULL;
    cJSON  *cmd = NULL;
    char   *buf = NULL;

    tag_oc_mqtt_response response;
    tag_key_value_list   list;

    int mid_int;
    int err_int;
    while(1)
    {
        if(osal_semp_pend(s_rcv_sync,cn_osal_timeout_forever))
        {
            err_int = 1;
            mid_int = 1;
            msg = cJSON_Parse(s_rcv_buffer);

            if(NULL != msg)
            {
                serviceId = cJSON_GetObjectItem(msg,"serviceId");
                if(NULL != serviceId)
                {
                    printf("serviceId:%s\n\r",serviceId->valuestring);
                }

                mid = cJSON_GetObjectItem(msg,"mid");
                if(NULL != mid)
                {
                    mid_int = mid->valueint;
                    printf("mid:%d\n\r",mid_int);
                }
                msgType = cJSON_GetObjectItem(msg,"msgType");
                if(NULL != msgType)
                {
                    printf("msgType:%s\n\r",msgType->valuestring);
                }
                cmd =  cJSON_GetObjectItem(msg,"cmd");
                if(NULL != cmd)
                {
                    printf("cmd:%s\n\r",cmd->valuestring);
                }

                paras = cJSON_GetObjectItem(msg,"paras");
                if(NULL != paras)
                {
                    ioswitch = cJSON_GetObjectItem(paras,"ioswitch");
                    if(NULL != ioswitch)
                    {
                        printf("ioswitch:%d\n\r",ioswitch->valueint);
                        err_int = en_oc_mqtt_err_code_ok;
                    }
                    else
                    {
                        printf("handle the json data as your specific profile\r\n");
                        err_int = en_oc_mqtt_err_code_err;
                    }
                }
                cJSON_Delete(msg);

                list.item.name = "body_para";
                list.item.buf = "body_para";
                list.item.type = en_key_value_type_string;
                list.next = NULL;

                response.hasmore = 0;
                response.errcode = err_int;
                response.mid = mid_int;
                response.bodylst = &list;

                msg = oc_mqtt_json_fmt_response(&response);
                if(NULL != msg)
                {
                    buf = cJSON_Print(msg);
                    if(NULL != buf)
                    {
                        if(0 == oc_mqtt_report(s_mqtt_handle,buf,strlen(buf),en_mqtt_al_qos_1))
                        {
                           // printf("SNDMSG:%s\n\r",buf);
                        }
                        osal_free(buf);
                    }
                    cJSON_Delete(msg);
                }
            }
        }
    }

    return 0;
}



static int at_oc_report(int argc,const char *argv[])
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
            ret = oc_mqtt_report(s_mqtt_handle,buf,strlen(buf),en_mqtt_al_qos_1);
            osal_free(buf);
        }

        cJSON_Delete(root);
    }

    return ret;
}
OSSHELL_EXPORT_CMD(at_oc_report,"report","report");



static int at_oc_connect(int argc,const char *argv[])
{
    void *handle = NULL;

    tag_oc_mqtt_config config;

    config.boot_mode = en_oc_boot_strap_mode_factory;
    config.lifetime = DEFAULT_LIFETIME;
    config.server = DEFAULT_SERVER_IPV4;
    config.port = DEFAULT_SERVER_PORT;
    config.msgdealer = app_msg_deal;
    config.code_mode = en_oc_mqtt_code_mode_json;
    config.sign_type = en_mqtt_sign_type_hmacsha256_check_time_no;
    config.device_type = en_oc_mqtt_device_type_static;
    config.auth_type = en_mqtt_auth_type_nodeid;
    config.device_info.s_device.deviceid = CN_MQTT_EP_NOTEID;
    config.device_info.s_device.devicepasswd = CN_MQTT_EP_PASSWD;

    config.security.type = en_mqtt_al_security_cas;
    config.security.u.cas.ca_crt.data = s_mqtt_ca_crt;
    config.security.u.cas.ca_crt.len = sizeof(s_mqtt_ca_crt); ///< must including the end '\0'

    handle = oc_mqtt_config(&config);
    if(NULL == handle)
    {
        printf("config err \r\n");
        return -1;
    }
    else
    {
        printf("config success\r\n");
    }

    s_mqtt_handle = handle;

    return 0;
}
OSSHELL_EXPORT_CMD(at_oc_connect,"connect","connect");



static int at_oc_disconnect()
{
    int ret = 0;
    ret =oc_mqtt_deconfig(s_mqtt_handle);
    if(0 == ret)
    {
        s_mqtt_handle = NULL;
        printf("deconfig success\r\n");

    }
    else
    {
        printf("deconfig failed\r\n");
    }
    return ret;
}
OSSHELL_EXPORT_CMD(at_oc_disconnect,"disconnect","disconnect");

///< all the demo,return 0 success while -1 failed

///< demo1: connect it normally
static int oc_mqtt_demo1()
{
    int ret = 0;

    at_oc_disconnect(0,NULL); ///< make sure we disconnected;
    printf("ENTER TESTDEMO 1:CONNECT WHILE NOT CONNECTED YET\n\r");
    ret = at_oc_connect(0,NULL);
    at_oc_disconnect(0,NULL);
    if(0 == ret)
    {
        printf("EXIT TESTDEMO 1:PASS\n\r");
    }
    else
    {
        printf("EXIT TESTDEMO 1:FAILED ************\n\r");
    }
    return ret;
}

///< demo2: disconnect it normally
static int oc_mqtt_demo2()
{
    int ret = 0;

    at_oc_connect(0,NULL); ///< make sure we connected;
    printf("ENTER TESTDEMO 2:DISCONNECTED NORMALLY\n\r");
    ret = at_oc_disconnect(0,NULL);
    if(0 == ret)
    {
        printf("EXIT TESTDEMO 2:PASS\n\r");
    }
    else
    {
        printf("EXIT TESTDEMO 2:FAILED ************\n\r");
    }

    return ret;
}

///< demo3: connect report disconnect
static int oc_mqtt_demo3()
{
    int ret = 0;

    at_oc_disconnect(0,NULL); ///< make sure we connected;
    printf("ENTER TESTDEMO 3:CONNECT->REPORT->DISCONNECTED\n\r");
    ret = at_oc_connect(0,NULL);
    if(0 == ret)
    {
        printf("CONNECT:SUCCESS\n\r");
    }
    else
    {
        printf("CONNECT:FAILED\n\r");
        goto DEMO_EXIT;
    }

    ret = at_oc_report(0,NULL);
    if(0 == ret)
    {
        printf("REPORT:SUCCESS\n\r");
    }
    else
    {
        printf("REPORT:FAILED\n\r");
        goto DEMO_EXIT;
    }

    ret = at_oc_disconnect(0,NULL);
    if(0 == ret)
    {
        printf("DISCONNECT:SUCCESS\n\r");
    }
    else
    {
        printf("DISCONNECT:FAILED\n\r");
        goto DEMO_EXIT;
    }

DEMO_EXIT:
    if(0 == ret)
    {
        printf("EXIT TESTDEMO 3:PASS\n\r");
    }
    else
    {
        printf("EXIT TESTDEMO 3:FAILED ************\n\r");
    }
    return ret;
}


///< demo4: disconnect it while it is not connected yet
static int oc_mqtt_demo4()
{
    int ret = 0;

    at_oc_disconnect(0,NULL); ///< make sure we disconnected;
    printf("ENTER TESTDEMO 4:DISCONNECTED WHILE NOT CONNECTED YET\n\r");
    ret = at_oc_disconnect(0,NULL);
    if(0 != ret)
    {
        printf("EXIT TESTDEMO 4:PASS\n\r");
    }
    else
    {
        printf("EXIT TESTDEMO 4:FAILED ************\n\r");
    }
    return ret;
}

///< demo5: report while it is not connected yet
static int oc_mqtt_demo5()
{
    int ret = 0;

    at_oc_disconnect(0,NULL); ///< make sure we disconnected;
    printf("ENTER TESTDEMO 5:REPORT WHILE NOT CONNECTED YET\n\r");
    ret = at_oc_report(0,NULL);
    if(0 != ret)
    {
        printf("EXIT TESTDEMO 5:PASS\n\r");
    }
    else
    {
        printf("EXIT TESTDEMO 5:FAILED ************\n\r");
    }
    return ret;
}

///< demo6: REPORT 1000 TIMES
static int oc_mqtt_demo6()
{
    int success_times = 0;
    int test_times = 0;
    int ret = 0;

    at_oc_disconnect(0,NULL); ///< make sure we connected;
    printf("ENTER TESTDEMO 6:CONNECT->REPORT 100 TIMES(QOS=1)->DISCONNECTED\n\r");
    ret = at_oc_connect(0,NULL);
    if(0 == ret)
    {
        printf("CONNECT:SUCCESS\n\r");
    }
    else
    {
        printf("CONNECT:FAILED\n\r");
        goto DEMO_EXIT;
    }
    printf("REPORT:000/100");
    while(++test_times  <= 100)
    {
        ret = at_oc_report(0,NULL);
        if(0 == ret)
        {
            success_times++;
        }
        printf("\b\b\b\b\b\b\b%3d/100",test_times);
        osal_task_sleep(1000);
    }
    ret = at_oc_disconnect(0,NULL);
    if(0 == ret)
    {
        printf("DISCONNECT:SUCCESS\n\r");
    }
    else
    {
        printf("DISCONNECT:FAILED\n\r");
        goto DEMO_EXIT;
    }

DEMO_EXIT:
    printf("EXIT TESTDEMO 6:REPORT SUCCESS:%03d/100\n\r",success_times);

    return ret;
}

static int oc_mqtt_static_test_entry(void *args)
{
    int ret = 0;
    oc_mqtt_demo1();
    oc_mqtt_demo2();
    oc_mqtt_demo3();
    oc_mqtt_demo4();
    oc_mqtt_demo5();
    ret = oc_mqtt_demo6();

    if(ret == 0)
    {
        printf("OC MQTT STATTIC TEST SUCCESS------------------\n\r");

    }
    else
    {
        printf("OC MQTT STATTIC TEST FAILED ++++++++++++++++++++\n\r");
    }

    return ret;

}



int test_oc_mqtt_static()
{
    osal_semp_create(&s_rcv_sync,1,0);
    osal_task_create("ocmqtt_cmd",oc_mqtt_cmd_entry,NULL,0x1000,NULL,10);
    osal_task_create("ocmqtt_demotest",oc_mqtt_static_test_entry,NULL,0x1000,NULL,10);
    return 0;
}
