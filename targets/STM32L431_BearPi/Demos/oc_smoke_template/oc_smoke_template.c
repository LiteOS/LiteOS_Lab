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
 *  2019-05-14 17:21  zhangqianfu  The first version
 *
 */
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <osal.h>
#include <oc_lwm2m_al.h>
#include <link_endian.h>

#include <boudica150_oc.h>
#include "E53_SF1.h"
#include "lcd.h"

#include <gpio.h>
#include <stm32l4xx_it.h>

#define cn_endpoint_id        "SDK_LWM2M_NODTLS"
#define cn_app_server         "49.4.85.232"
#define cn_app_port           "5683"

typedef unsigned char int8u;
typedef char int8s;
typedef unsigned short int16u;
typedef short int16s;
typedef unsigned char int24u;
typedef char int24s;
typedef int int32s;
typedef char string;
typedef char array;
typedef char varstring;
typedef char variant;

#define cn_app_Smoke 0x8
#define cn_app_response_Smoke_Control_Beep 0xa
#define cn_app_Smoke_Control_Beep 0x9

#pragma pack(1)
typedef struct
{
    int8u messageId;
    int16u Smoke_Value;
} tag_app_Smoke;

typedef struct
{
    int8u messageId;
    int16u mid;
    int8u errcode;
    int8u Beep_State;
} tag_app_Response_Smoke_Control_Beep;

typedef struct
{
    int8u messageId;
    int16u mid;
    string Beep[3];
} tag_app_Smoke_Control_Beep;
#pragma pack()

void *context;
int8_t qr_code = 1;
extern const unsigned char gImage_Huawei_IoT_QR_Code[114720];
E53_SF1_Data_TypeDef E53_SF1_Data;


//if your command is very fast,please use a queue here--TODO
#define cn_app_rcv_buf_len 128
static int             s_rcv_buffer[cn_app_rcv_buf_len];
static int             s_rcv_datalen;
static osal_semp_t     s_rcv_sync;

static void timer1_callback(void *arg)
{
	qr_code = !qr_code;
	LCD_Clear(WHITE);
	if (qr_code == 1)
		LCD_Show_Image(0,0,240,239,gImage_Huawei_IoT_QR_Code);
	else
	{
		POINT_COLOR = RED;
		LCD_ShowString(40, 10, 200, 16, 24, "IoTCluB BearPi");
		LCD_ShowString(15, 50, 210, 16, 24, "LiteOS NB-IoT Demo");
		LCD_ShowString(10, 100, 200, 16, 16, "NCDP_IP:");
		LCD_ShowString(80, 100, 200, 16, 16, cn_app_server);
		LCD_ShowString(10, 150, 200, 16, 16, "NCDP_PORT:");
		LCD_ShowString(100, 150, 200, 16, 16, cn_app_port);
	}
}

//use this function to push all the message to the buffer
static int app_msg_deal(void *usr_data, en_oc_lwm2m_msg_t type, void *data, int len)
{
    unsigned char *msg;
    msg = data;
    int ret = -1;

    if(len <= cn_app_rcv_buf_len)
    {
    	if (msg[0] == 0xaa && msg[1] == 0xaa)
    	{
    		printf("OC respond message received! \n\r");
    		return ret;
    	}
        memcpy(s_rcv_buffer,msg,len);
        s_rcv_datalen = len;

        osal_semp_post(s_rcv_sync);

        ret = 0;

    }
    return ret;
}


static int app_cmd_task_entry()
{
    int ret = -1;
    tag_app_Response_Smoke_Control_Beep Response_Smoke_Control_Beep;
    tag_app_Smoke_Control_Beep *Smoke_Control_Beep;
    int8_t msgid;

    while(1)
    {
        if(osal_semp_pend(s_rcv_sync,cn_osal_timeout_forever))
        {
            msgid = s_rcv_buffer[0] & 0x000000FF;
            switch (msgid)
            {
                 case cn_app_Smoke_Control_Beep:
                    Smoke_Control_Beep = (tag_app_Smoke_Control_Beep *)s_rcv_buffer;
                    printf("Smoke_Control_Beep:msgid:%d mid:%d", Smoke_Control_Beep->messageId, ntohs(Smoke_Control_Beep->mid));
                    /********** code area for cmd from IoT cloud  **********/
                    if (Smoke_Control_Beep->Beep[0] == 'O' && Smoke_Control_Beep->Beep[1] == 'N')
                    {
                        E53_SF1_Beep_StatusSet(ON);
                        Response_Smoke_Control_Beep.messageId = cn_app_response_Smoke_Control_Beep;
                    	Response_Smoke_Control_Beep.mid = Smoke_Control_Beep->mid;
                        Response_Smoke_Control_Beep.errcode = 0;
                		Response_Smoke_Control_Beep.Beep_State = 1;
                        oc_lwm2m_report(context,(char *)&Response_Smoke_Control_Beep,sizeof(Response_Smoke_Control_Beep),1000);    ///< report cmd reply message
                    }
                    if (Smoke_Control_Beep->Beep[0] == 'O' && Smoke_Control_Beep->Beep[1] == 'F' && Smoke_Control_Beep->Beep[2] == 'F')
                    {
                        E53_SF1_Beep_StatusSet(OFF);
                        Response_Smoke_Control_Beep.messageId = cn_app_response_Smoke_Control_Beep;
                    	Response_Smoke_Control_Beep.mid = Smoke_Control_Beep->mid;
                        Response_Smoke_Control_Beep.errcode = 0;
                		Response_Smoke_Control_Beep.Beep_State = 0;
                        oc_lwm2m_report(context,(char *)&Response_Smoke_Control_Beep,sizeof(Response_Smoke_Control_Beep),1000);    ///< report cmd reply message
                    }
                    /********** code area end  **********/
                    break;
                default:
                    break;
            }
        }
    }

    return ret;
}

static int app_report_task_entry()
{
    int ret = -1;

    oc_config_param_t      oc_param;
    tag_app_Smoke Smoke;

    memset(&oc_param,0,sizeof(oc_param));

    oc_param.app_server.address = cn_app_server;
    oc_param.app_server.port = cn_app_port;
    oc_param.app_server.ep_id = cn_endpoint_id;
    oc_param.boot_mode = en_oc_boot_strap_mode_factory;
    oc_param.rcv_func = app_msg_deal;

    // context = oc_lwm2m_config(&oc_param);
    ret = oc_lwm2m_config(&context, &oc_param);
    if (0 != ret)
    {
    	return ret;
    }

    if(NULL != context)   //success ,so we could receive and send
    {
        //install a dealer for the led message received
        while(1) //--TODO ,you could add your own code here
        {
            Smoke.messageId = cn_app_Smoke;
            Smoke.Smoke_Value = htons((int)E53_SF1_Data.Smoke_Value);
            oc_lwm2m_report(context, (char *)&Smoke, sizeof(Smoke), 1000);
            osal_task_sleep(2*1000);
        }
    }

    return ret;
}

static int app_collect_task_entry()
{
    Init_E53_SF1();
    while (1)
    {
        E53_SF1_Read_Data();
        printf("\r\n******************************Smoke Value is  %d\r\n", (int)E53_SF1_Data.Smoke_Value);
        if (qr_code == 0)
        {
            // LCD_ShowString(10, 200, 200, 16, 16, "BH1750 Value is:");
            // LCD_ShowNum(140, 200, lux, 5, 16);
        }
        osal_task_sleep(2*1000);
    }

    return 0;
}


#include <stimer.h>

int standard_app_demo_main()
{
    osal_semp_create(&s_rcv_sync,1,0);

    osal_task_create("app_collect",app_collect_task_entry,NULL,0x400,NULL,3);
    osal_task_create("app_report",app_report_task_entry,NULL,0x1000,NULL,2);
    osal_task_create("app_command",app_cmd_task_entry,NULL,0x1000,NULL,3);


    stimer_create("lcdtimer",timer1_callback,NULL,8*1000,cn_stimer_flag_start);

    return 0;
}





