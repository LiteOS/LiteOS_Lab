/*----------------------------------------------------------------------------
 * Copyright (c) <2016-2018>, <Huawei Technologies Co., Ltd>
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

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "hmac.h"
#include "mbedtls/md.h"
#include "test_oc_mqtt_tiny.h"

#include "cpp_stub.h"

/* Defines ------------------------------------------------------------------*/
/* Typedefs -----------------------------------------------------------------*/
/* Macros -------------------------------------------------------------------*/
/* Local variables ----------------------------------------------------------*/
/* Extern variables ---------------------------------------------------------*/
extern "C"
{
    #include "osal.h"
    #include "oc_mqtt_tiny.h"

    #include <queue.h>
    #include <oc_mqtt_al.h>
    #include <oc_mqtt_assistant.h>

    /* stubs */

    static queue_t* queue_create_stub(const char *name,int len,int syncmode)
    {
        return NULL;
    }

    static void* osal_task_create_stub(const char *name,int (*task_entry)(void *args),
                      void *args,int stack_size,void *stack,int prior)
    {
        return NULL;
    }

    static cJSON *cJSON_CreateObject_stub(void)
    {
        return NULL;
    }

    static cJSON *cJSON_CreateObject_stub2(double num)
    {
        cJSON *item = NULL;
        static int index = 0;

        if (++index == 2) {
            return NULL;
        }
        item = (cJSON *)malloc(sizeof(cJSON));
        if (item != NULL) {
            memset(item, 0, sizeof(cJSON));
        }
        return item;
    }

    static cJSON *cJSON_CreateString_stub(const char *string)
    {
        return NULL;
    }

    static cJSON *cJSON_CreateNumber_stub(double num)
    {
        return NULL;
    }

    static cJSON *cJSON_CreateArray_stub(void)
    {
        return NULL;
    }
}

#define DEFAULT_LIFETIME            60               ///< the platform need more
#define DEFAULT_SERVER_IPV4         "49.4.93.24"     ///<  server ip address
#define DEFAULT_SERVER_PORT         "8883"           ///<  server mqtt service port
#define CN_MQTT_EP_NOTEID           "mqtt_sdk01"
#define CN_MQTT_EP_DEVICEID         "6e4a90d5-8e31-48c0-920a-d03c6e91d923"
#define CN_MQTT_EP_PASSWD           "c18f10422c93548e6fef"

typedef struct
{
    int qos;
    int dup;
    int retain;
    int msg_len;
    char *topic;
    uint8_t *msg;
    uint8_t *buf;
} demo_msg_t;

static int app_msg_deal(void *arg, mqtt_al_msgrcv_t *msg)
{
    return 0;
}

static int oc_cmd_normal(demo_msg_t *demo_msg)
{
    int ret = 0;
    char *buf = NULL; ///< used for the mqtt

    cJSON *mid_json;
    cJSON *cmd_json;
    cJSON *response_msg;
    tag_oc_mqtt_response response;
    tag_key_value_list list;
    int mid_int = 0;
    int err_int = 0;
    //////////////HANDLE YOUR MESSAGE HERE WITH YOUR DEVICE PROFILE///////////
    cmd_json = cJSON_Parse((const char *)demo_msg->msg);
    mid_json = cJSON_GetObjectItem(cmd_json, "mid");
    if (NULL != mid_json)
    {
        mid_int = mid_json->valueint;
    }
    cJSON_Delete(cmd_json);
    //////////////DO THE RESPONSE FOR THE COMMAND/////////////////////////////
    list.item.name = (char *)"body_para";
    list.item.buf = (char *)"body_para";
    list.item.type = en_key_value_type_string;
    list.next = NULL;

    response.hasmore = en_oc_mqtt_has_more_no;
    response.errcode = err_int;
    response.mid = mid_int;
    response.bodylst = &list;

    response_msg = oc_mqtt_json_fmt_response(&response);
    if (NULL != response_msg)
    {
        buf = cJSON_PrintUnformatted(response_msg);
        if (NULL != buf)
        {
            ret = oc_mqtt_report((uint8_t *)buf, strlen(buf), en_mqtt_al_qos_1);
            printf("%s:RESPONSE:mid:%d err_int:%d retcode:%d \r\n", __FUNCTION__,
                   mid_int, err_int, ret);

            osal_free(buf);
        }
        cJSON_Delete(response_msg);
    }

    return 0;
}

static int oc_report_normal(void)
{
    int ret = 0;
    cJSON *root = NULL;
    char *buf = NULL;
    tag_oc_mqtt_report report;
    tag_key_value_list lst;
    static int leftpower = 1;
    static int times = 1;

    leftpower = (leftpower + 7) % 100;

    lst.item.name = (char *)"batteryLevel";
    lst.item.buf = (char *)&leftpower;
    lst.item.len = sizeof(leftpower);
    lst.item.type = en_key_value_type_int;
    lst.next = NULL;

    report.hasmore = en_oc_mqtt_has_more_no;
    report.paralst = &lst;
    report.serviceid = (char *)"Battery";
    report.eventtime = (char *)"20200102T163120Z";

    root = oc_mqtt_json_fmt_report(&report);
    if (NULL != root)
    {
        buf = cJSON_PrintUnformatted(root);
        if (NULL != buf)
        {
            ret = oc_mqtt_report((uint8_t *)buf, strlen(buf), en_mqtt_al_qos_1);
            printf("%s:REPORT:times:%d:power:%d retcode:%d \r\n", __FUNCTION__, times++, leftpower, ret);
            osal_free(buf);
        }

        cJSON_Delete(root);
    }

    return 0;
}

/* Global variables ---------------------------------------------------------*/
/* Private function prototypes ----------------------------------------------*/

/* Public functions ---------------------------------------------------------*/
TestOcMqttTiny::TestOcMqttTiny()
{
    TEST_ADD(TestOcMqttTiny::test_oc_mqtt_al);
    TEST_ADD(TestOcMqttTiny::test_start);
    TEST_ADD(TestOcMqttTiny::test_hmac_generate_passwd);
    TEST_ADD(TestOcMqttTiny::test_oc_mqtt_tiny);
    TEST_ADD(TestOcMqttTiny::test_oc_mqtt_tiny_bs);
    TEST_ADD(TestOcMqttTiny::test_oc_mqtt_tiny_connect_err);
    TEST_ADD(TestOcMqttTiny::test_oc_mqtt_tiny_config_err);
    TEST_ADD(TestOcMqttTiny::test_oc_mqtt_stub);
}

TestOcMqttTiny::~TestOcMqttTiny()
{
}

void TestOcMqttTiny::test_oc_mqtt_al(void)
{
    int ret;

    ret = oc_mqtt_init();
    TEST_ASSERT(ret == 0);

    ret = oc_mqtt_subscribe(NULL, 0);
    TEST_ASSERT(ret == en_oc_mqtt_err_system);

    const char *err_str = oc_mqtt_err(-1);
    TEST_ASSERT(0 == strcmp(err_str, "UNKNOWN"));

    err_str = oc_mqtt_err(1);
    TEST_ASSERT(0 == strcmp(err_str, "parameter_err"));
}

void TestOcMqttTiny::test_hmac_generate_passwd(void)
{
    unsigned char buf[128];
    int ret;

    ret = hmac_generate_passwd(NULL, 0, NULL, 0, NULL, 0);
    TEST_ASSERT(ret == MBEDTLS_ERR_MD_BAD_INPUT_DATA);

    ret = hmac_generate_passwd((char *)"content", strlen("content"), (char *)"key", strlen("key"), buf, 10);
    TEST_ASSERT(ret == MBEDTLS_ERR_MD_BAD_INPUT_DATA);

    ret = hmac_generate_passwd((char *)"content", strlen("content"), (char *)"key", strlen("key"), buf, sizeof(buf));
    TEST_ASSERT(ret == 0);
}

void TestOcMqttTiny::test_oc_mqtt_tiny(void)
{
    int ret;

    ret = oc_mqtt_tiny_install();
    TEST_ASSERT(ret == 0);

    oc_mqtt_config_t config;
    config.boot_mode = en_oc_mqtt_mode_nobs_static_nodeid_hmacsha256_notimecheck_json;
    config.msg_deal = app_msg_deal;
    config.msg_deal_arg = NULL;
    config.lifetime = DEFAULT_LIFETIME;
    config.server_addr = (char *)DEFAULT_SERVER_IPV4;
    config.server_port = (char *)DEFAULT_SERVER_PORT;
    config.id = (char *)CN_MQTT_EP_NOTEID;
    config.pwd= (char *)CN_MQTT_EP_PASSWD;
    config.sec_type = en_mqtt_al_security_cas;

    ret = oc_mqtt_config(&config);
    TEST_ASSERT(ret == en_oc_mqtt_err_ok);

    ret = oc_report_normal();
    TEST_ASSERT(ret == 0);

    ret = oc_mqtt_deconfig();
    TEST_ASSERT(ret == en_oc_mqtt_err_ok);

    sleep(1);
    oc_mqtt_tiny_uninstall();
}

static int msg_convert(mqtt_al_msgrcv_t *msg)
{
    int ret = -1;
    uint8_t  *buf;
    uint32_t  buflen;
    demo_msg_t *demo_msg;

    buflen =msg->msg.len + msg->topic.len + sizeof(demo_msg_t) + 1 +1;
    buf = (uint8_t *)osal_malloc(buflen);
    if(NULL != buf)
    {
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

        oc_cmd_normal(demo_msg);
        osal_free(demo_msg);
    }

    return ret;
}

void TestOcMqttTiny::test_oc_mqtt_tiny_bs(void)
{
    int ret;

    ret = oc_mqtt_tiny_install();
    TEST_ASSERT(ret == 0);

    oc_mqtt_config_t config;
    config.boot_mode = en_oc_mqtt_mode_bs_static_nodeid_hmacsha256_notimecheck_json;
    config.msg_deal = app_msg_deal;
    config.msg_deal_arg = NULL;
    config.lifetime = DEFAULT_LIFETIME;
    config.server_addr = (char *)DEFAULT_SERVER_IPV4;
    config.server_port = (char *)DEFAULT_SERVER_PORT;
    config.id = (char *)CN_MQTT_EP_NOTEID;
    config.pwd= (char *)CN_MQTT_EP_PASSWD;
    config.sec_type = en_mqtt_al_security_none;

    ret = oc_mqtt_config(&config);
    TEST_ASSERT(ret == en_oc_mqtt_err_ok);

    mqtt_al_msgrcv_t msg = {0};
    msg.topic.data = (char *)"hello";
    msg.topic.len = strlen(msg.topic.data) + 1;
    msg.msg.data = (char *)"{\"address\": \"10.0.0.1:8883\",\"dnsFlag\": 1}";
    msg.msg.len = strlen(msg.msg.data) + 1;
    msg.qos = en_mqtt_al_qos_0;
    msg.dup = 0;
    msg.retain = 0;
    msg_convert(&msg);

    ret = oc_mqtt_deconfig();
    TEST_ASSERT(ret == en_oc_mqtt_err_ok);

    sleep(1);
    oc_mqtt_tiny_uninstall();
}

extern char g_conret;

void TestOcMqttTiny::test_oc_mqtt_tiny_connect_err(void)
{
    int ret;

    ret = oc_mqtt_tiny_install();
    TEST_ASSERT(ret == 0);

    oc_mqtt_config_t config;
    config.boot_mode = en_oc_mqtt_mode_nobs_static_nodeid_hmacsha256_notimecheck_json;
    config.msg_deal = app_msg_deal;
    config.msg_deal_arg = NULL;
    config.lifetime = DEFAULT_LIFETIME;
    config.server_addr = (char *)DEFAULT_SERVER_IPV4;
    config.server_port = (char *)DEFAULT_SERVER_PORT;
    config.id = (char *)CN_MQTT_EP_NOTEID;
    config.pwd= (char *)CN_MQTT_EP_PASSWD;
    config.sec_type = en_mqtt_al_security_cas;

    for (int i = 2; i < 7; i++)
    {
        g_conret = i;
        ret = oc_mqtt_config(&config);
        TEST_ASSERT(ret != en_oc_mqtt_err_ok);
    }

    ret = oc_mqtt_deconfig();
    TEST_ASSERT(ret == en_oc_mqtt_err_noconfigured);

    oc_mqtt_tiny_uninstall();
}

void TestOcMqttTiny::test_oc_mqtt_tiny_config_err(void)
{
    int ret;

    ret = oc_mqtt_tiny_install();
    TEST_ASSERT(ret == 0);

    oc_mqtt_config_t config;
    config.boot_mode = en_oc_mqtt_mode_nobs_static_nodeid_hmacsha256_notimecheck_json;
    config.msg_deal = app_msg_deal;
    config.msg_deal_arg = NULL;
    config.lifetime = DEFAULT_LIFETIME;
    config.server_addr = (char *)DEFAULT_SERVER_IPV4;
    config.server_port = (char *)DEFAULT_SERVER_PORT;
    config.id = (char *)CN_MQTT_EP_NOTEID;
    config.pwd= (char *)CN_MQTT_EP_PASSWD;
    config.sec_type = en_mqtt_al_security_cas;

    config.boot_mode = en_oc_mqtt_mode_last;
    ret = oc_mqtt_config(&config);
    TEST_ASSERT(ret == en_oc_mqtt_err_parafmt);
    config.boot_mode = en_oc_mqtt_mode_nobs_static_nodeid_hmacsha256_notimecheck_json;

    config.id = NULL;
    ret = oc_mqtt_config(&config);
    TEST_ASSERT(ret == en_oc_mqtt_err_parafmt);
    config.id = (char *)CN_MQTT_EP_NOTEID;

    config.sec_type = en_mqtt_al_security_end;
    ret = oc_mqtt_config(&config);
    TEST_ASSERT(ret == en_oc_mqtt_err_parafmt);
    config.sec_type = en_mqtt_al_security_cas;

    ret = oc_mqtt_deconfig();
    TEST_ASSERT(ret == en_oc_mqtt_err_noconfigured);

    oc_mqtt_tiny_uninstall();
}

void TestOcMqttTiny::test_oc_mqtt_stub(void)
{
    Stub sb;

    // osal_task_create failed
    sb.set(osal_task_create, osal_task_create_stub);
    TEST_ASSERT_EQUALS(-1, oc_mqtt_tiny_install());
    sb.reset(osal_task_create);

    // queue_create failed
    sb.set(queue_create, queue_create_stub);
    TEST_ASSERT_EQUALS(-1, oc_mqtt_tiny_install());
    sb.reset(queue_delete);

// oc_mqtt_json_fmt_response failed
    tag_oc_mqtt_response response = {0, };
    // cJSON_CreateObject failed
    sb.set(cJSON_CreateObject, cJSON_CreateObject_stub);
    TEST_ASSERT(NULL == oc_mqtt_json_fmt_response(&response));
    sb.reset(cJSON_CreateObject);

    sb.set(cJSON_CreateObject, cJSON_CreateObject_stub2);
    TEST_ASSERT(NULL == oc_mqtt_json_fmt_response(&response));
    sb.reset(cJSON_CreateObject);

    // cJSON_CreateString failed
    sb.set(cJSON_CreateString, cJSON_CreateString_stub);
    TEST_ASSERT(NULL == oc_mqtt_json_fmt_response(&response));
    sb.reset(cJSON_CreateString);

    // // cJSON_CreateNumber failed
    sb.set(cJSON_CreateNumber, cJSON_CreateNumber_stub);
    TEST_ASSERT(NULL == oc_mqtt_json_fmt_response(&response));
    sb.reset(cJSON_CreateNumber);

// oc_mqtt_json_fmt_report failed
    tag_oc_mqtt_report report = {0, };
    // cJSON_CreateObject failed
    sb.set(cJSON_CreateObject, cJSON_CreateObject_stub);
    TEST_ASSERT(NULL == oc_mqtt_json_fmt_report(&report));
    sb.reset(cJSON_CreateObject);

    sb.set(cJSON_CreateObject, cJSON_CreateObject_stub2);
    TEST_ASSERT(NULL == oc_mqtt_json_fmt_report(&report));
    sb.reset(cJSON_CreateObject);

    // cJSON_CreateString failed
    sb.set(cJSON_CreateString, cJSON_CreateString_stub);
    TEST_ASSERT(NULL == oc_mqtt_json_fmt_report(&report));
    sb.reset(cJSON_CreateString);

    // cJSON_CreateNumber failed
    sb.set(cJSON_CreateNumber, cJSON_CreateNumber_stub);
    TEST_ASSERT(NULL == oc_mqtt_json_fmt_report(&report));
    sb.reset(cJSON_CreateNumber);

    // cJSON_CreateArray failed
    sb.set(cJSON_CreateArray, cJSON_CreateArray_stub);
    TEST_ASSERT(NULL == oc_mqtt_json_fmt_report(&report));
    sb.reset(cJSON_CreateArray);
}

/* Private functions --------------------------------------------------------*/
void TestOcMqttTiny::test_start()
{
    int ret;

    ret = oc_mqtt_tiny_install();
    TEST_ASSERT(ret == -1); // osal_malloc fail

    ret = osal_init();
    TEST_ASSERT(ret == 0);
}

void TestOcMqttTiny::test_end()
{
}

void TestOcMqttTiny::setup()
{
}

void TestOcMqttTiny::tear_down()
{
}
