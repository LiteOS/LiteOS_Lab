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
#include <link_log.h>
#include <queue.h>
#include <oc_lwm2m_al.h>
#include <link_endian.h>
#include <lcd.h>
#include <E53_SC1.h>
#include "BearPi-IoT_gd32f303.h"

#ifdef CONFIG_OCLWM2M_APP_DTLSMODE
#define CN_APP_SERVERIP       "iot-coaps.cn-north-4.myhuaweicloud.com"
#define CN_APP_SERVERPORT     "5684"
#define CN_APP_DEVICEID       "Lwm2mBoard001"
static uint8_t  g_device_psk[]={0x01,0x02,0x03,0x04,0x05,0x06};

#else

#define CN_APP_SERVERIP       "iot-coaps.cn-north-4.myhuaweicloud.com"
#define CN_APP_SERVERPORT     "5683"
#define CN_APP_DEVICEID       "Lwm2mBoard002"

#endif


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
    LCD_ShowString(10, 50, 240, 24, 24, " LWM2M ScoreBoard");
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

#pragma pack(1)
typedef struct
{
    uint8_t    msgid;
    uint32_t   f1;
    uint32_t   f2;
    uint32_t   light_status;
}appmsg_up_t;

typedef struct
{
    uint8_t   msgid;
    uint16_t  mid;
    uint32_t  para;
}appmsg_down_scoreclear_t;

typedef struct
{
    uint8_t   msgid;
    uint16_t  mid;
    uint32_t  light_cmd;
}appmsg_down_lightcmd_t;

typedef struct
{
    uint8_t msgid;
    uint16_t mid;
    uint8_t  cmdcode;
}appmsg_cmdreply_t;

#pragma pack()

typedef struct
{
    int               msg_len;
    en_oc_lwm2m_msg_t msg_type;
    uint8_t *msg;
}appmsg_down_t;

typedef enum
{
    EN_APPMSG_ID_UP = 0,
    EN_APPMSG_ID_DOWNSCORECLEAR,
    EN_APPMSG_ID_DOWNLIGHTCTRL,
    EN_APPMSG_ID_REPLYSCORECLEAR,
    EN_APPMSG_ID_REPLYLIGHTCTRL,
}en_appmsg_id_t;


static int app_propertyreport(void)
{
    int ret;
    appmsg_up_t  reportmsg;

    reportmsg.msgid = (uint8_t )EN_APPMSG_ID_UP;
    reportmsg.f1 = htonl(g_app_cb.f1_value);
    reportmsg.f2 = htonl(g_app_cb.f2_value);
    reportmsg.light_status = htonl(g_app_cb.light_status);
    ret = oc_lwm2m_report((char *)&reportmsg,sizeof(reportmsg),1000);

    LINK_LOG_DEBUG("REPORT:F1:%d F2:%d  Light:%d RET:%d",\
            g_app_cb.f1_value,g_app_cb.f2_value,g_app_cb.light_status,ret);
    return ret;
}
//use this function to push all the message to the buffer
static int app_msg_rcvhook(void *usr_data, en_oc_lwm2m_msg_t type, void *data, int len)
{
    int    ret = -1;
    char  *buf;
    int    buflen;
    appmsg_down_t *demo_msg;
    ///< clone the message
    buflen = sizeof(appmsg_down_t) + len ;
    buf = osal_malloc(buflen);
    if(NULL != buf)
    {
        demo_msg = (appmsg_down_t *)(uintptr_t)buf;
        buf += sizeof(appmsg_down_t);
        ///< copy the message and push it to the queue
        demo_msg->msg_len = len;
        demo_msg->msg_type = type;
        demo_msg->msg = (uint8_t *)buf;
        (void) memcpy(buf,data,len);
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

///< now we deal the message here
static int  oc_msg_deal(appmsg_down_t *demo_msg)
{
    int   ret = -1;
    uint8_t msgid;
    appmsg_down_scoreclear_t  msgscoreclear;
    appmsg_down_lightcmd_t    lighctrl;
    appmsg_cmdreply_t         cmdreply;

    msgid = *demo_msg->msg;

    switch(msgid)
    {
        case EN_APPMSG_ID_DOWNSCORECLEAR:
            if(demo_msg->msg_len == sizeof(appmsg_down_scoreclear_t))
            {
                memcpy(&msgscoreclear,demo_msg->msg,sizeof(msgscoreclear));
                cmdreply.msgid = EN_APPMSG_ID_REPLYSCORECLEAR;
                cmdreply.mid = msgscoreclear.mid;
                cmdreply.cmdcode = 0;
                g_app_cb.f1_value = 0;
                g_app_cb.f2_value = 0;
                (void)oc_lwm2m_report((char *)&cmdreply,sizeof(cmdreply),1000);
                score_display();
                ret = 0;
            }
            break;
        case EN_APPMSG_ID_DOWNLIGHTCTRL:
            if(demo_msg->msg_len == sizeof(lighctrl))
            {
                memcpy(&lighctrl,demo_msg->msg,sizeof(lighctrl));
                g_app_cb.light_status = ntohl(lighctrl.light_cmd);
                cmdreply.cmdcode = 0;

                if(g_app_cb.light_status == EN_SCORELIGHT_CMD_ON)
                {
                    E53_SC1_LED_StatusSet(ON);
                    LINK_LOG_DEBUG("LIGHT ON");
                }
                else if(g_app_cb.light_status == EN_SCORELIGHT_CMD_OFF)
                {
                    E53_SC1_LED_StatusSet(OFF);
                    LINK_LOG_DEBUG("LIGHT OFF");
                }
                else
                {
                    cmdreply.cmdcode = 1;
                }
                cmdreply.msgid = EN_APPMSG_ID_REPLYLIGHTCTRL;
                cmdreply.mid = lighctrl.mid;
                (void)oc_lwm2m_report((char *)&cmdreply,sizeof(cmdreply),1000);
                ret = 0;
            }
            break;
        default:
            break;
    }

//    if(ret == 0)
//    {
//        app_propertyreport();
//    }
    return ret;
}

static int task_rcvmsg_entry( void *args)
{
    appmsg_down_t *demo_msg;
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
    oc_config_param_t      oc_param;
    (void) memset(&oc_param,0,sizeof(oc_param));
    oc_param.app_server.address = CN_APP_SERVERIP;
    oc_param.app_server.port = CN_APP_SERVERPORT;
    oc_param.app_server.ep_id = CN_APP_DEVICEID;

#ifdef CONFIG_OCLWM2M_APP_DTLSMODE
    oc_param.app_server.psk = (char *)&g_device_psk[0];
    oc_param.app_server.psk_len = sizeof(g_device_psk);
    oc_param.app_server.psk_id = CN_APP_DEVICEID;
#endif

    oc_param.boot_mode = en_oc_boot_strap_mode_factory;
    oc_param.rcv_func = app_msg_rcvhook;

    ret = oc_lwm2m_config( &oc_param);
    if (0 != ret)
    {
        return ret;
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

    Init_E53_SC1();
    LCD_Init();
    LCD_Clear(BLACK);
    POINT_COLOR = GREEN;

    indexinfo_display();
    /* enable the F1 key GPIO clock */
    gd_eval_key_init(F1_KEY, KEY_MODE_EXTI);
    /* enable the F2 key GPIO clock */
    gd_eval_key_init(F2_KEY, KEY_MODE_EXTI);
    ///< connect the isr with the interrupt
    osal_int_connect(EXTI0_IRQn, 2,0,EXTI0_IRQHandler,NULL);
    osal_int_connect(EXTI1_IRQn, 3,0,EXTI1_IRQHandler,NULL);
}

int standard_app_demo_main()
{
    int ret = -1;
    LINK_LOG_DEBUG("DO THE OC LWM2M DEMOS\n\r");
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
