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

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <iot_link_config.h>
#include <queue.h>
#include <oc_mqtt_al.h>
#include <oc_mqtt_profile.h>

#ifndef CONFIG_APP_SERVERIP
#define CONFIG_APP_SERVERIP       "121.36.42.100"
#endif
#ifndef CONFIG_APP_SERVERPORT
#define CONFIG_APP_SERVERPORT     "1883"
#endif

#ifndef CONFIG_APP_DEVICEID
#define CONFIG_APP_DEVICEID       "5f58768785edc002bc69cbf2_hcdemo123456"
#endif

#ifndef CONFIG_APP_DEVICEPWD
#define CONFIG_APP_DEVICEPWD      "123456789"
#endif

#ifndef CONFIG_APP_LIFETIME
#define CONFIG_APP_LIFETIME       60     ///< seconds
#endif

#ifndef CONFIG_QUEUE_TIMEOUT
#define CONFIG_QUEUE_TIMEOUT      (5*1000)
#endif

typedef enum
{
    en_msg_cmd = 0,
    en_msg_report,
    en_msg_conn,
    en_msg_disconn,
}en_msg_type_t;

typedef struct
{
    char *request_id;
    char *payload;
}cmd_t;

typedef struct
{
    int lum;
    int temp;
    int hum;
}report_t;

typedef struct
{
    en_msg_type_t msg_type;
    union{
        cmd_t cmd;
        report_t report;
    }msg;
}app_msg_t;

typedef struct
{
    queue_t                     *app_msg;
    int                          connected;
    int                          led;
    int                          motor;
}app_cb_t;
static app_cb_t  g_app_cb;

static void deal_report_msg(report_t *report)
{
    oc_mqtt_profile_service_t    service;
    oc_mqtt_profile_kv_t         temperature;
    oc_mqtt_profile_kv_t         humidity;
    oc_mqtt_profile_kv_t         luminance;
    oc_mqtt_profile_kv_t         led;
    oc_mqtt_profile_kv_t         motor;

    if(g_app_cb.connected != 1){
        return;
    }

    service.event_time = NULL;
    service.service_id = "Agriculture";
    service.service_property = &temperature;
    service.nxt = NULL;

    temperature.key = "Temperature";
    temperature.value = &report->temp;
    temperature.type = EN_OC_MQTT_PROFILE_VALUE_INT;
    temperature.nxt = &humidity;

    humidity.key = "Humidity";
    humidity.value = &report->hum;
    humidity.type = EN_OC_MQTT_PROFILE_VALUE_INT;
    humidity.nxt = &luminance;

    luminance.key = "Luminance";
    luminance.value = &report->lum;
    luminance.type = EN_OC_MQTT_PROFILE_VALUE_INT;
    luminance.nxt = &led;

    led.key = "LightStatus";
    led.value = g_app_cb.led?"ON":"OFF";
    led.type = EN_OC_MQTT_PROFILE_VALUE_STRING;
    led.nxt = &motor;

    motor.key = "MotorStatus";
    motor.value = g_app_cb.motor?"ON":"OFF";
    motor.type = EN_OC_MQTT_PROFILE_VALUE_STRING;
    motor.nxt = NULL;

    oc_mqtt_profile_propertyreport(NULL,&service);
    return;
}
//use this function to push all the message to the buffer
static int msg_rcv_callback(oc_mqtt_profile_msgrcv_t *msg)
{
    int    ret = 0;
    char  *buf;
    int    buf_len;
    app_msg_t *app_msg;

    if((NULL == msg)|| (msg->request_id == NULL) || (msg->type != EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_COMMANDS)){
        return ret;
    }

    buf_len = sizeof(app_msg_t) + strlen(msg->request_id) + 1 + msg->msg_len + 1;
    buf = osal_malloc(buf_len);
    if(NULL == buf){
        return ret;
    }
    app_msg = (app_msg_t *)buf;
    buf += sizeof(app_msg_t);

    app_msg->msg_type = en_msg_cmd;
    app_msg->msg.cmd.request_id = buf;
    buf_len = strlen(msg->request_id);
    buf += buf_len + 1;
    memcpy(app_msg->msg.cmd.request_id, msg->request_id, buf_len);
    app_msg->msg.cmd.request_id[buf_len] = '\0';

    buf_len = msg->msg_len;
    app_msg->msg.cmd.payload = buf;
    memcpy(app_msg->msg.cmd.payload, msg->msg, buf_len);
    app_msg->msg.cmd.payload[buf_len] = '\0';

    ret = queue_push(g_app_cb.app_msg,app_msg,10);
    if(ret != 0){
        osal_free(app_msg);
    }

    return ret;
}

///< COMMAND DEAL
#include <cJSON.h>
static void deal_cmd_msg(cmd_t *cmd)
{
    cJSON *obj_root;
    cJSON *obj_cmdname;
    cJSON *obj_paras;
    cJSON *obj_para;

    int cmdret = 1;
    oc_mqtt_profile_cmdresp_t  cmdresp;
    obj_root = cJSON_Parse(cmd->payload);
    if(NULL == obj_root){
        goto EXIT_JSONPARSE;
    }

    obj_cmdname = cJSON_GetObjectItem(obj_root,"command_name");
    if(NULL == obj_cmdname){
        goto EXIT_CMDOBJ;
    }
    if(0 == strcmp(cJSON_GetStringValue(obj_cmdname),"Agriculture_Control_light")){
        obj_paras = cJSON_GetObjectItem(obj_root,"Paras");
        if(NULL == obj_paras){
            goto EXIT_OBJPARAS;
        }
        obj_para = cJSON_GetObjectItem(obj_paras,"Light");
        if(NULL == obj_para){
            goto EXIT_OBJPARA;
        }
        ///< operate the LED here
        if(0 == strcmp(cJSON_GetStringValue(obj_para),"ON")){
            g_app_cb.led = 1;
            // define your own code to make the light on
            LINK_LOG_DEBUG("Light On!");
        }
        else{
            g_app_cb.led = 0;
            // define your own code to make the light on
            LINK_LOG_DEBUG("Light Off!");
        }
        cmdret = 0;
    }
    else if(0 == strcmp(cJSON_GetStringValue(obj_cmdname),"Agriculture_Control_Motor")){
        obj_paras = cJSON_GetObjectItem(obj_root,"Paras");
        if(NULL == obj_paras){
            goto EXIT_OBJPARAS;
        }
        obj_para = cJSON_GetObjectItem(obj_paras,"Motor");
        if(NULL == obj_para){
            goto EXIT_OBJPARA;
        }
        ///< operate the Motor here
        if(0 == strcmp(cJSON_GetStringValue(obj_para),"ON")){
            g_app_cb.motor = 1;
            // define your own code to make the motor on
            LINK_LOG_DEBUG("Motor On!");
        }
        else{
            g_app_cb.motor = 0;
            // define your own code to make the motor off
            LINK_LOG_DEBUG("Motor Off!");
        }
        cmdret = 0;
    }

EXIT_OBJPARA:
EXIT_OBJPARAS:
EXIT_CMDOBJ:
    cJSON_Delete(obj_root);
EXIT_JSONPARSE:
    ///< do the response
    cmdresp.paras = NULL;
    cmdresp.request_id = cmd->request_id;
    cmdresp.ret_code = cmdret;
    cmdresp.ret_name = NULL;
    (void)oc_mqtt_profile_cmdresp(NULL,&cmdresp);
    return;
}

static void deal_conn_msg(void)
{
    int ret;
    oc_mqtt_profile_connect_t  connect_para;
    (void) memset( &connect_para, 0, sizeof(connect_para));

    connect_para.boostrap =      0;
    connect_para.device_id =     CONFIG_APP_DEVICEID;
    connect_para.device_passwd = CONFIG_APP_DEVICEPWD;
    connect_para.server_addr =   CONFIG_APP_SERVERIP;
    connect_para.server_port =   CONFIG_APP_SERVERPORT;
    connect_para.life_time =     CONFIG_APP_LIFETIME;
    connect_para.rcvfunc =       msg_rcv_callback;
    connect_para.security.type = EN_DTLS_AL_SECURITY_TYPE_NONE;
    ret = oc_mqtt_profile_connect(&connect_para);

    // a simple reconnection avoidance algorithm demo
    int min_back_off = 1000;
    int default_back_off = 1000;
    int max_back_off = 30 * 1000;
    int retry_times = 0;
    int max_retry_times = 20;
    while (ret != (int)en_oc_mqtt_err_ok) {
        int low_bound = (int) default_back_off * 0.8;
        int high_bound = (int) default_back_off * 1.0;
        int random_back_off = (int) rand() % (high_bound - low_bound);
        int back_off_with_jitter = (int) pow(2.0, (double) retry_times) * (random_back_off + low_bound);
        int next_retry_time = (int) (min_back_off + back_off_with_jitter) > max_back_off ? max_back_off : (min_back_off + back_off_with_jitter);
        osal_task_sleep(next_retry_time);
        retry_times++;
        if (retry_times > max_retry_times) {
            retry_times = 0;
        }
        ret = oc_mqtt_profile_connect(&connect_para);
    }

    if((ret == (int)en_oc_mqtt_err_ok)){
        g_app_cb.connected = 1;
    }
    return;
}

static void deal_disconn_msg(void)
{
    int ret;
    ret = oc_mqtt_profile_disconnect();
    if((ret == (int)en_oc_mqtt_err_ok)){
        g_app_cb.connected = 0;
    }
    return;
}

static int task_main_entry( void *args)
{
    app_msg_t *app_msg;
    while(1){
        app_msg = NULL;
        (void)queue_pop(g_app_cb.app_msg,(void **)&app_msg,(int)cn_osal_timeout_forever);
        if(NULL != app_msg){
            switch(app_msg->msg_type){
                case en_msg_conn:
                    deal_conn_msg();
                    break;
                case en_msg_disconn:
                    deal_disconn_msg();
                    break;
                case en_msg_cmd:
                    deal_cmd_msg(&app_msg->msg.cmd);
                    break;
                case en_msg_report:
                    deal_report_msg(&app_msg->msg.report);
                    break;
                default:
                    break;
            }
            osal_free(app_msg);
        }
    }
    return 0;
}

static int task_sensor_entry( void *args)
{
    app_msg_t *app_msg;

    while(1){
        // define your code to read sensor data
        app_msg = osal_malloc(sizeof(app_msg_t));
        LINK_LOG_DEBUG("SENSOR:lum:%f temp:%f hum:%f",data.Lux, data.Temperature,data.Humidity);
        if(NULL != app_msg){
            app_msg->msg_type = en_msg_report;
            app_msg->msg.report.hum = rand() % 25;
            app_msg->msg.report.lum = rand() % 35;
            app_msg->msg.report.temp = rand() % 100;
            if(0 != queue_push(g_app_cb.app_msg,app_msg,CONFIG_QUEUE_TIMEOUT)){
                osal_free(app_msg);
            }
        }
        osal_task_sleep(1000*5);
    }
    return 0;
}

static int shell_conn(int argc, const char *argv[])
{
    app_msg_t *app_msg;
    app_msg = osal_malloc(sizeof(app_msg_t));
    if(NULL != app_msg){
        app_msg->msg_type = en_msg_conn;
        if(0 != queue_push(g_app_cb.app_msg,app_msg,CONFIG_QUEUE_TIMEOUT)){
            osal_free(app_msg);
        }
    }
    return 0;
}

static int shell_disconn(int argc, const char *argv[])
{
    app_msg_t *app_msg;
    app_msg = osal_malloc(sizeof(app_msg_t));
    if(NULL != app_msg){
        app_msg->msg_type = en_msg_disconn;
        if(0 != queue_push(g_app_cb.app_msg,app_msg,CONFIG_QUEUE_TIMEOUT)){
            osal_free(app_msg);
        }
    }
    return 0;
}


int standard_app_demo_main()
{
    int ret = -1;
    LINK_LOG_DEBUG("This Is MQTT V5 DEMOS:HC Display");
    g_app_cb.app_msg = queue_create("queue_rcvmsg",10,1);
    if(NULL ==  g_app_cb.app_msg){
        LINK_LOG_ERROR("Create receive msg queue failed");
        return ret;
    }

    (void)osal_task_create("iot_main",task_main_entry,NULL,0x800,NULL,8);
    (void)osal_task_create("sensor",task_sensor_entry,NULL,0x800,NULL,9);
    shell_conn(0,NULL);
    ret = 0;
    return ret;
}
