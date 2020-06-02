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
#include <iot_link_config.h>
#include <queue.h>
#include <oc_mqtt_al.h>
#include <oc_mqtt_profile.h>

#include <lcd.h>
#include <E53_SC1.h>
#include <BearPi-IoT_gd32f303.h>
///< and i think the platform will make the timeout much longer and fix this problem
///< for the MCU has weak compute ability, and the new CERT FILE of platform is very big, so should not use tls for the MCUS
///< 1.设备接入服务重新更新了证书以及加密套件，椭圆加密算法算法需要大算力去链接服务器，因此对月MCU而言，请选择非加密方案
///< 2.设备发放平台目前本身在和设备接入做对接，还不ready.

#define CONFIG_OCMQTTV5_DEMO_SERVERIP       "121.36.42.100"
#define CONFIG_OCMQTTV5_DEMO_SERVERPORT     "1883"
#define CONFIG_OCMQTTV5_DEMO_DEVICEID       "5ed5f2c039d04f02c7d8fe80_MqttBoard001"
#define CONFIG_OCMQTTV5_DEMO_DEVPWD         "f62fcf47d62c4ed18913"
#define CONFIG_OCMQTTV5_DEMO_LIFE           60     ///< seconds

typedef struct
{
    queue_t                     *rcvmsgqueue;
    osal_semp_t                  reportsync;
    int                          f1_value;
    int                          f2_value;
    int                          light_status;
}app_cb_t;
static app_cb_t  g_app_cb;

static void indexinfo_display()
{
    LCD_ShowString(10, 50, 240, 24, 24, " MQTT ScoreBoard");
    LCD_ShowString(20, 74, 240, 16, 16, "BearPi IoT Develop Board");
    LCD_ShowString(20, 90, 240, 16, 16, "Powerd by Huawei LiteOS!");
    LCD_ShowString(10, 120, 240, 24, 24, "      SCORE      ");

}

#define CN_SCORE_STRING  20
static void score_display(void)
{
    char  score_string[CN_SCORE_STRING];  ///< MAY NOT BE OVERRIDE
    score_string[CN_SCORE_STRING-1] = '\0';
    snprintf(score_string,CN_SCORE_STRING-1," F1 %04d:%04d F2",g_app_cb.f1_value, g_app_cb.f2_value);
    LCD_ShowString(10, 150, 240, 24, 24, score_string);
}


static int app_propertyreport(void)
{
    int ret;
    oc_mqtt_profile_service_t    reportproperty;
    oc_mqtt_profile_kv_t         f1_property;
    oc_mqtt_profile_kv_t         f2_property;
    oc_mqtt_profile_kv_t         light_property;


    reportproperty.event_time = NULL;
    reportproperty.service_id = "ScoreBoard";
    reportproperty.service_property = &f1_property;
    reportproperty.nxt = NULL;

    f1_property.key = "f1";
    f1_property.value = &g_app_cb.f1_value;
    f1_property.type = EN_OC_MQTT_PROFILE_VALUE_INT;
    f1_property.nxt = &f2_property;

    f2_property.key = "f2";
    f2_property.value = &g_app_cb.f2_value;
    f2_property.type = EN_OC_MQTT_PROFILE_VALUE_INT;
    f2_property.nxt = &light_property;

    light_property.nxt = NULL;
    light_property.key = "LightStatus";
    light_property.type = EN_OC_MQTT_PROFILE_VALUE_INT;
    light_property.value = &g_app_cb.light_status;

    ret = oc_mqtt_profile_propertyreport(NULL,&reportproperty);
    LINK_LOG_DEBUG("REPORT:F1:%d F2:%d  Light:%d RET:%d",\
            g_app_cb.f1_value,g_app_cb.f2_value,g_app_cb.light_status,ret);


    return ret;
}
//use this function to push all the message to the buffer
static int app_msg_rcvhook(oc_mqtt_profile_msgrcv_t *msg)
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
        demo_msg = (oc_mqtt_profile_msgrcv_t *)(uintptr_t)buf;
        buf += sizeof(oc_mqtt_profile_msgrcv_t);
        ///< copy the message and push it to the queue
        demo_msg->type = msg->type;

        if(NULL != msg->request_id)
        {
            demo_msg->request_id = buf;
            datalen = strlen(msg->request_id);
            (void) memcpy(buf,msg->request_id,datalen);
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
        (void) memcpy(buf,msg->msg,datalen);
        buf[datalen] = '\0';
        ret = queue_push(g_app_cb.rcvmsgqueue,demo_msg,10);
        if(ret != 0)
        {
            osal_free(demo_msg);
        }
    }

    return ret;
}

typedef enum
{
    EN_SCORELIGHT_CMD_ON = 1,
    EN_SCORELIGHT_CMD_OFF
}en_scorelight_cmd_t;

///< COMMAND DEAL
#include <cJSON.h>
static void oc_cmd_deal(oc_mqtt_profile_msgrcv_t *demo_msg)
{
    cJSON *obj_root;
    cJSON *obj_cmdname;
    cJSON *obj_paras;
    cJSON *obj_para;

    int cmdret = 1;
    oc_mqtt_profile_cmdresp_t  cmdresp;
    obj_root = cJSON_Parse(demo_msg->msg);
    if(NULL == obj_root)
    {
        goto EXIT_JSONPARSE;
    }

    obj_cmdname = cJSON_GetObjectItem(obj_root,"command_name");
    if(NULL == obj_cmdname)
    {
        goto EXIT_CMDOBJ;
    }
    if(0 == strcmp(cJSON_GetStringValue(obj_cmdname),"LightCtrl"))
    {
        obj_paras = cJSON_GetObjectItem(obj_root,"Paras");
        if(NULL == obj_paras)
        {
            goto EXIT_OBJPARAS;
        }

        obj_para = cJSON_GetObjectItem(obj_paras,"Light");
        if(NULL == obj_para)
        {
            goto EXIT_OBJPARA;
        }
        if(obj_para->valueint == (int)EN_SCORELIGHT_CMD_ON)
        {

            LINK_LOG_DEBUG("LIGHT ON");
            E53_SC1_LED_StatusSet(ON);
        }
        else if(obj_para->valueint == (int)EN_SCORELIGHT_CMD_OFF)
        {
            LINK_LOG_DEBUG("LIGHT OFF");
            E53_SC1_LED_StatusSet(OFF);
        }
        else
        {
            goto EXIT_LIGHTPARA;
        }
        g_app_cb.light_status = obj_para->valueint;
        cmdret = 0;
    }
    else if(0 == strcmp(cJSON_GetStringValue(obj_cmdname),"ScoreClear"))
    {
        g_app_cb.f1_value = 0;
        g_app_cb.f2_value = 0;
        score_display();
        cmdret = 0;
    }
    app_propertyreport();

EXIT_LIGHTPARA:
EXIT_OBJPARA:
EXIT_OBJPARAS:
EXIT_CMDOBJ:
    cJSON_Delete(obj_root);
EXIT_JSONPARSE:
    ///< do the response
    cmdresp.paras = NULL;
    cmdresp.request_id = demo_msg->request_id;
    cmdresp.ret_code = cmdret;
    cmdresp.ret_name = NULL;
    (void)oc_mqtt_profile_cmdresp(NULL,&cmdresp);

    return;
}



///< now we deal the message here
static int  oc_msg_deal(oc_mqtt_profile_msgrcv_t *demo_msg)
{
    oc_mqtt_profile_propertysetresp_t propertysetresp;
    LINK_LOG_DEBUG("RCVMSG:type:%d reuqestID:%s payloadlen:%d payload:%s\n\r",\
             (int) demo_msg->type,demo_msg->request_id==NULL?"NULL":demo_msg->request_id,\
             demo_msg->msg_len,(char *)demo_msg->msg);

    switch(demo_msg->type)
    {
        case EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_MSGDOWN:
            ///< add your own deal here
            break;
        case EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_COMMANDS:
            ///< add your own deal here
            oc_cmd_deal(demo_msg);
            break;

        case EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_PROPERTYSET:
            ///< add your own deal here
            ///< do the response
            propertysetresp.request_id = demo_msg->request_id;
            propertysetresp.ret_code = 0;
            propertysetresp.ret_description = NULL;
            (void)oc_mqtt_profile_propertysetresp(NULL,&propertysetresp);
            break;

        case  EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_PROPERTYGET:
            ///< add your own deal here
            ///< do the response
            break;
        case EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_EVENT:
            break;

        default:
            break;

    }
    return 0;
}

static int task_rcvmsg_entry( void *args)
{
    oc_mqtt_profile_msgrcv_t *demo_msg;
    while(1)
    {
        demo_msg = NULL;
        (void)queue_pop(g_app_cb.rcvmsgqueue,(void **)&demo_msg,(int)cn_osal_timeout_forever);
        if(NULL != demo_msg)
        {
            (void) oc_msg_deal(demo_msg);
            osal_free(demo_msg);
        }
    }
    return 0;
}

static int task_reportmsg_entry(void *args)
{
    int ret;
    oc_mqtt_profile_connect_t  connect_para;
    (void) memset( &connect_para, 0, sizeof(connect_para));

    connect_para.boostrap =      0;
    connect_para.device_id =     CONFIG_OCMQTTV5_DEMO_DEVICEID;
    connect_para.device_passwd = CONFIG_OCMQTTV5_DEMO_DEVPWD;
    connect_para.server_addr =   CONFIG_OCMQTTV5_DEMO_SERVERIP;
    connect_para.sevver_port =   CONFIG_OCMQTTV5_DEMO_SERVERPORT;
    connect_para.life_time =     CONFIG_OCMQTTV5_DEMO_LIFE;
    connect_para.rcvfunc =       app_msg_rcvhook;
    connect_para.security.type = EN_DTLS_AL_SECURITY_TYPE_NONE;
    ret = oc_mqtt_profile_connect(&connect_para);
    if((ret != (int)en_oc_mqtt_err_ok))
    {
        (void) printf("config:err :code:%d\r\n",ret);
        return -1;
    }
    score_display();
    while(1)  //do the loop here
    {
        osal_semp_pend(g_app_cb.reportsync, cn_osal_timeout_forever);
        score_display();
        app_propertyreport();
    }
    return 0;
}


void EXTI1_IRQHandler(void *arg)
{
    if(RESET != exti_interrupt_flag_get(EXTI_1)){
        exti_interrupt_flag_clear(EXTI_1);
        g_app_cb.f1_value++;
        osal_semp_post(g_app_cb.reportsync);
    }
}

void EXTI0_IRQHandler(void *arg)
{
    if(RESET != exti_interrupt_flag_get(EXTI_0)){
        exti_interrupt_flag_clear(EXTI_0);
        g_app_cb.f2_value++;
        osal_semp_post(g_app_cb.reportsync);
    }
}

#include <systick.h>
void HardWare_Init(void)
{
    /* configure systick */
    systick_config();

    LCD_Init();
    LCD_Clear(BLACK);
    POINT_COLOR = GREEN;
    Init_E53_SC1();

    indexinfo_display();
    /* enable the F1 key GPIO clock */
    gd_eval_key_init(F1_KEY, KEY_MODE_EXTI);
    /* enable the F2 key GPIO clock */
    gd_eval_key_init(F2_KEY, KEY_MODE_EXTI);
    ///< connect the isr with the interrupt
    osal_int_connect(EXTI0_IRQn, 2,0,EXTI0_IRQHandler,NULL);
    osal_int_connect(EXTI1_IRQn, 3,0,EXTI1_IRQHandler,NULL);
}


static const char *g_user_instruction = "There are two buttons and one light;When push the buttons, we will counter and do the report!";
int standard_app_demo_main()
{
    int ret = -1;
    LINK_LOG_DEBUG("This Is MQTT V5 DEMOS:ScoreBoard");
    LINK_LOG_DEBUG("Usage:%s",g_user_instruction);
    HardWare_Init();
    if(false == osal_semp_create(&g_app_cb.reportsync,1,0))
    {
        LINK_LOG_ERROR("Create report sync failed");
        goto EXIT_SEMPSYNC;
    }
    g_app_cb.rcvmsgqueue = queue_create("queue_rcvmsg",2,1);
    if(NULL ==  g_app_cb.rcvmsgqueue)
    {
        LINK_LOG_ERROR("Create receive msg queue failed");
        goto EXIT_QUEUE;
    }
    (void)osal_task_create("demo_reportmsg",task_reportmsg_entry,NULL,0x800,NULL,8);
    (void) osal_task_create("demo_rcvmsg",task_rcvmsg_entry,NULL,0x800,NULL,8);
    ret = 0;
    return ret;


EXIT_QUEUE:
    osal_semp_del(g_app_cb.reportsync);
EXIT_SEMPSYNC:
    return ret;
}
