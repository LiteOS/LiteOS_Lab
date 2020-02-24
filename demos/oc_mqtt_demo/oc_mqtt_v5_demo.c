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
 *  2020-02-07 09:06  zhangqianfu  The first version
 *
 */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <queue.h>

#include <osal.h>
#include <oc_mqtt_al.h>
#include <oc_mqtt_profile.h>

///< ANYWAY, YOU COULD CONFIG IT TO THE ONE MODE,ALL THE INFORMATION IS JUST FOR THE TEST
#if CONFIG_OC_MQTT_DEMO_BS

///< the device bootstrap center
#define CN_SERVER_IPV4         "119.3.251.30"
#define CN_SERVER_PORT         "8883"
#define CN_EP_DEVICEID         "54f107da-f251-436c-af4c-624f33b7d7b6"
#define CN_EP_PASSWD           "f62fcf47d62c4ed18913"
#define CN_BOOT_MODE           1

#else

#define CN_BOOT_MODE              0

///< the iot device access center
#define CN_SERVER_IPV4         "119.3.248.253"
#define CN_SERVER_PORT         "8883"
#define CN_EP_DEVICEID         "54f107da-f251-436c-af4c-624f33b7d7b6"
#define CN_EP_PASSWD           "f62fcf47d62c4ed18913"
//#define CN_EP_DEVICEID         "5d0c76788a48f95ac41bcb9c_sdk1002"


/////< the iot develop center
//#define CN_SERVER_IPV4         "49.4.93.24"
//#define CN_SERVER_PORT         "8883"
//#define CN_EP_DEVICEID         "mqtt_sdk01"
//#define CN_EP_PASSWD           "c18f10422c93548e6fef"
#endif

#define CN_LIFE_TIME            60                 ///< the platform need more

//if your command is very fast,please use a queue here--TODO
static queue_t                   *s_queue_rcvmsg = NULL;   ///< this is used to cached the message
static oc_mqtt_profile_service_t  s_device_service;

#ifndef CONFIG_OC_MQTT_DEMO_BS
///< two way mqtt mode
static const char s_client_ca[] = \
"-----BEGIN CERTIFICATE-----\r\n"
"MIICuDCCAaACCQDAnCoP3W+otTANBgkqhkiG9w0BAQsFADAdMQswCQYDVQQGEwJD\r\n"
"QTEOMAwGA1UEAwwFTVkgQ0EwHhcNMjAwMjE3MTEyNTA2WhcNMjEwMjExMTEyNTA2\r\n"
"WjAfMQswCQYDVQQGEwJDTjEQMA4GA1UEAwwHQ0xJRU5UMjCCASIwDQYJKoZIhvcN\r\n"
"AQEBBQADggEPADCCAQoCggEBALKOOfeq1QQnwLNsMboqr47ClO24sTd6oLiSjGYe\r\n"
"zb7IECnjiEg+sILEH5avi6uKdCR8QZHskIWdQbNqi2h0E01MTxuneTCnFaAdYh2z\r\n"
"B8n4IUsGr1gyCayfKCQG8ElDElBkB2JMce/IEuxJvnFdhXy6c9zhj8iU+fusD9hm\r\n"
"jvx34zd6Sn2uRl8wdI89pVwkESKyx6J5za/dBWCS0hxvOtDEY2gJA9av8BNYjTxQ\r\n"
"tRlwnxGzgaza8aA9LMbnRmv1bML98w4NgNHkVGUtw1eQso0unHnyEF1WhqEu0pLk\r\n"
"BJQbdO4s4ajQ1rOr7niVnCT99KtzJuWm3oymNdDT6BHsnn0CAwEAATANBgkqhkiG\r\n"
"9w0BAQsFAAOCAQEAPayFtS8du8D+mPdRU96vFqVpDx6oksGhrUIO+3GZ4ruuHjtf\r\n"
"A764y6zD1/LxQVw15xN44G+AqUwEi/BZszQ+5rSitwqoZFciKMblaqd0ZJSG6G2d\r\n"
"Cqyl87cTL6Tlgpa5cBka4ksaMs2OVU9x1aaNnejrTbX2PvZO7o/pVVimQE8Yh+1J\r\n"
"k2D26hg9UoWN4Z3/9LdGC3y4dsmtuQS+Cye61ZfHtT3/XjTRjl9/c+xKtVsQpE6g\r\n"
"dpaGVxWpFUCpGe4POrNHjOOAXaFiCHP7o9Kab1UnxuFVKHCtNT9wdJ7v8puCliwB\r\n"
"t40h9oegMyUYL/VG+14wOYgEPuCeGqqilht+4w==\r\n"
"-----END CERTIFICATE-----\r\n";
static const char s_client_pk[]= \
"-----BEGIN PRIVATE KEY-----\r\n"
"MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQCyjjn3qtUEJ8Cz\r\n"
"bDG6Kq+OwpTtuLE3eqC4koxmHs2+yBAp44hIPrCCxB+Wr4urinQkfEGR7JCFnUGz\r\n"
"aotodBNNTE8bp3kwpxWgHWIdswfJ+CFLBq9YMgmsnygkBvBJQxJQZAdiTHHvyBLs\r\n"
"Sb5xXYV8unPc4Y/IlPn7rA/YZo78d+M3ekp9rkZfMHSPPaVcJBEisseiec2v3QVg\r\n"
"ktIcbzrQxGNoCQPWr/ATWI08ULUZcJ8Rs4Gs2vGgPSzG50Zr9WzC/fMODYDR5FRl\r\n"
"LcNXkLKNLpx58hBdVoahLtKS5ASUG3TuLOGo0Nazq+54lZwk/fSrcyblpt6MpjXQ\r\n"
"0+gR7J59AgMBAAECggEAFbjx2myhD6laazmdbx2kUATiI22Vub8kcG/7vqy+sRgb\r\n"
"x0zUFp3x0WbUxPtETbFBYfLv8zXJe0ffZ1lt7l44vu2qrh2Yw89/9QO+HQjbyLl0\r\n"
"K3F7wYoM3UBbnM5vSypqvvb8YvwT3G0FyFNMZPRQuhTg5Z1XZbqiEvki0Fvhx1sG\r\n"
"bksCKK85dPaH/7kXxMTPZ67/2Z3TlwUERo1YSPRnrQXGnDowudLqQjn8ptiPfO39\r\n"
"bIMQhM/HI1bTw4F5EevL7sS4rcnhpmQNU6ZeINPvYuel+hP36uPwTHMm1hkZbMgP\r\n"
"a69Yk8/meENffl9fQ+T8woYMGgKq0DM+O5ip7QzrLQKBgQDdvwI/9npuwpYkdjWT\r\n"
"tLj5BKFNINjn+TjyvN1QQmOhXqzTy1y9HVm7EYh1l0H5PY/WgYadBWNYabgNSIe0\r\n"
"vxl7tdDxDX4DBQqDIIEY45sPa3glq6flCgtPzkZxcYTvZpqmdX8/iMYGXHpdOMdm\r\n"
"S8jngobFDEgLxFS1lLLs+LtTTwKBgQDOIz08d2zMRX5AW0SkQ+VbdwcO7G5qlTJV\r\n"
"42Rf51Dmr0as/bX5eIWAAfcsEi/kG2e5Ozupe4BVZXarw7AL1bzE4mLvfcwZEUiJ\r\n"
"Qj56H6QTOiVm/DTZdBz415z60zjCui+59luev6txZBL51ZhoifzfdAfxd8ztucV+\r\n"
"mj3kytIucwKBgBSdporl0norsyPOrap+2dek+44dKK1geU/wCS9mDLc7AxxpwsR+\r\n"
"TxJaTfKIzoRqmc3amvZMQvE8WvU5Mgy225qpy7QtifKj7FdGlVoIBymG11PBQsnD\r\n"
"hku0JM51PGO5iVVChUMV5zs+K84w8jDriNElTHxTaPtXFH0RRxVLBrd3AoGAIYX1\r\n"
"lXnOhcHfcFPna9dm22MWEMU+h7LPxsv+LOKPc5SZp9Ry3alN/K1VJyOmCjCyw5rw\r\n"
"bvWYv0qK7BAc9y/BTTuL32rT4FfBNiFBy9aNdlsEbiUh6yNI+ucFrnzfRDvHOzTn\r\n"
"XX/KP9yIgXzNFmMqbqSODiTvfNGzXo1y91xMpiUCgYAfO+Xb5msOMABiBNGD85qr\r\n"
"C+0xpuWoPe4pB5ccgniv1zhPlUJ+8l1vWz/ZWnH/6oVxGGrq5xLtoBgFWM4uSseI\r\n"
"Je0fSqZp0XAbaFI7wkEV/uP6VMjjFT5TfJz4idgI+EaE72va2jbFBgg1MyfR3gx4\r\n"
"deaXc4ZpE64iV+eAtZ8VVQ==\r\n"
"-----END PRIVATE KEY-----\r\n";
static const char *s_client_pk_pwd = "123456";

#endif


//use this function to push all the message to the buffer
static int app_msg_deal(oc_mqtt_profile_msgrcv_t *msg)
{
    int    ret = -1;
    char  *buf;
    int    buflen;
    int    datalen = 0;
    oc_mqtt_profile_msgrcv_t *demo_msg;

    ///< clone the message
    buflen = sizeof(oc_mqtt_profile_msgrcv_t) + msg->msg_len + 1;///< we copy with '\0' endings
    if(NULL != msg->request_id)
    {
        buflen += strlen(msg->request_id) +1; ///< ending with '\0'
    }

    buf = osal_malloc(buflen);
    if(NULL != buf)
    {
        demo_msg = (oc_mqtt_profile_msgrcv_t *)buf;
        buf += sizeof(oc_mqtt_profile_msgrcv_t);
        ///< copy the message and push it to the queue
        demo_msg->type = msg->type;

        if(NULL != msg->request_id)
        {
            demo_msg->request_id = buf;
            datalen = strlen(msg->request_id);
            memcpy(buf,msg->request_id,datalen);
            buf[datalen] = '\0';
            buf += (datalen+1);
        }
        else
        {
            demo_msg->request_id = NULL;
        }

        demo_msg->msg = buf;
        demo_msg->msg_len = msg->msg_len;
        datalen = msg->msg_len;
        memcpy(buf,msg->msg,datalen);
        buf[datalen] = '\0';

        printf("RCVMSG:type:%d reuqestID:%s payloadlen:%d payload:%s\n\r",\
                demo_msg->type,demo_msg->request_id==NULL?"NULL":demo_msg->request_id,\
                demo_msg->msg_len,(char *)demo_msg->msg);

        ret = queue_push(s_queue_rcvmsg,demo_msg,10);
        if(ret != 0)
        {
            osal_free(demo_msg);
        }
    }

    return ret;
}

///< now we deal the message here
static int  oc_cmd_normal(oc_mqtt_profile_msgrcv_t *demo_msg)
{
    static int value = 0;
    oc_mqtt_profile_cmdresp_t  cmdresp;
    oc_mqtt_profile_propertysetresp_t propertysetresp;
    oc_mqtt_profile_propertygetresp_t propertygetresp;

    printf("DEALMSG:type:%d reuqestID:%s payloadlen:%d payload:%s\n\r",\
            demo_msg->type,demo_msg->request_id==NULL?"NULL":demo_msg->request_id,\
            demo_msg->msg_len,(char *)demo_msg->msg);
    switch(demo_msg->type)
    {
        case EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_MSGDOWN:
            ///< add your own deal here
            break;
        case EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_COMMANDS:
            ///< add your own deal here

            ///< do the response
            cmdresp.paras = NULL;
            cmdresp.request_id = demo_msg->request_id;
            cmdresp.ret_code = 0;
            cmdresp.ret_name = NULL;
            oc_mqtt_profile_cmdresp(NULL,&cmdresp);
            break;

        case EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_PROPERTYSET:
            ///< add your own deal here

            ///< do the response
            propertysetresp.request_id = demo_msg->request_id;
            propertysetresp.ret_code = 0;
            propertysetresp.ret_description = NULL;
            oc_mqtt_profile_propertysetresp(NULL,&propertysetresp);
            break;

        case  EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_PROPERTYGET:
            ///< add your own deal here

            ///< do the response
            value  = (value+1)%100;
            s_device_service.service_property->key = "radioValue";
            s_device_service.service_property->value = &value;
            s_device_service.service_property->type = EN_OC_MQTT_PROFILE_VALUE_INT;

            propertygetresp.request_id = demo_msg->request_id;
            propertygetresp.services = &s_device_service;
            oc_mqtt_profile_propertygetresp(NULL,&propertygetresp);
            break;

        default:
            break;

    }
    return 0;
}


static int  oc_report_normal(void)
{
    int ret = en_oc_mqtt_err_ok;
    static int value = 1;
    static int times = 0;

    times = (times + 1)%2;
    if(times == EN_OC_MQTT_PROFILE_MSG_TYPE_UP_MSGUP)
    {
        oc_mqtt_profile_msgup_t msgup;
        msgup.device_id = CN_EP_DEVICEID;
        msgup.id = "12345";
        msgup.name = "MSGUP";
        msgup.msg = "Hello,Message Up";
        msgup.msg_len = strlen(msgup.msg);

        ret = oc_mqtt_profile_msgup(NULL,&msgup);
    }
    else if(times == EN_OC_MQTT_PROFILE_MSG_TYPE_UP_PROPERTYREPORT)
    {
        value  = (value+1)%100;
        s_device_service.service_property->key = "radioValue";
        s_device_service.service_property->value = &value;
        s_device_service.service_property->type = EN_OC_MQTT_PROFILE_VALUE_INT;
        ret = oc_mqtt_profile_propertyreport(NULL,&s_device_service);
    }

    printf("REPORT TIMES:%d RET:%d\n\r",times,ret);
    return ret;
}


static int task_rcvmsg_entry( void *args)
{

    oc_mqtt_profile_msgrcv_t *demo_msg;

    while(1)
    {
        demo_msg = NULL;
        queue_pop(s_queue_rcvmsg,(void **)&demo_msg,cn_osal_timeout_forever);

        if(NULL != demo_msg)
        {
            oc_cmd_normal(demo_msg);
            osal_free(demo_msg);
        }
    }

    return 0;
}

static int task_reportmsg_entry(void *args)
{
    int ret;
    oc_mqtt_profile_connect_t  connect_para;

    memset( &connect_para, 0, sizeof(connect_para));

    connect_para.boostrap =      CN_BOOT_MODE;
    connect_para.device_id =     CN_EP_DEVICEID;
    connect_para.device_passwd = CN_EP_PASSWD;
    connect_para.server_addr =   CN_SERVER_IPV4;
    connect_para.sevver_port =   CN_SERVER_PORT;
    connect_para.life_time =     CN_LIFE_TIME;
    connect_para.rcvfunc =       app_msg_deal;

    connect_para.security.type = EN_DTLS_AL_SECURITY_TYPE_CERT;

#ifndef CONFIG_OC_MQTT_DEMO_BS
    connect_para.security.u.cert.client_ca = (uint8_t *)s_client_ca;
    connect_para.security.u.cert.client_ca_len = sizeof(s_client_ca);
    connect_para.security.u.cert.client_pk = (uint8_t *)s_client_pk;
    connect_para.security.u.cert.client_pk_len = sizeof(s_client_pk);
    connect_para.security.u.cert.client_pk_pwd = (uint8_t *)s_client_pk_pwd;
    connect_para.security.u.cert.client_pk_pwd_len = strlen(s_client_pk_pwd);
#endif

    ret = oc_mqtt_profile_connect(&connect_para);
    if((ret != en_oc_mqtt_err_ok))
    {
        printf("config:err :code:%d\r\n",ret);
        return -1;
    }

    char *topic;
    topic = "user/demo_topic";
    ret = oc_mqtt_subscribe(topic, 0);
    printf("subscribe:topic:%d ret:%d",topic,ret);

    ret = oc_mqtt_unsubscribe(topic);
    printf("unsubscribe:topic:%d ret:%d",topic,ret);

    while(1)  //do the loop here
    {
        oc_report_normal();
        osal_task_sleep(10*1000);
    }
    return 0;
}

int standard_app_demo_main()
{
    static oc_mqtt_profile_kv_t  property;
    s_queue_rcvmsg = queue_create("queue_rcvmsg",2,1);

    ///< initialize the service
    property.nxt   = NULL;
    s_device_service.event_time = NULL;
    s_device_service.service_id = "DeviceStatus";
    s_device_service.service_property = &property;
    s_device_service.nxt = NULL;

    osal_task_create("demo_reportmsg",task_reportmsg_entry,NULL,0x800,NULL,8);
    osal_task_create("demo_rcvmsg",task_rcvmsg_entry,NULL,0x800,NULL,8);

    return 0;
}
