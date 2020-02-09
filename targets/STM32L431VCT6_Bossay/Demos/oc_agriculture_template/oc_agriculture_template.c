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
#include "DEMO.h"
#include "BH1750.h"
#include "SHT30.h"
#include "Actuators.h"
#include "lcd.h"

#include <gpio.h>
#include <stm32l4xx_it.h>

#define cn_endpoint_id        "SDK_LWM2M_NODTLS"
#define cn_app_server         "49.4.85.232"
#define cn_app_port           "5683"

#define cn_app_Agriculture 0x0
#define cn_app_Agriculture_Control_Light 0x1
#define cn_app_response_Agriculture_Control_Light 0x2
#define cn_app_Agriculture_Control_Motor 0x3
#define cn_app_response_Agriculture_Control_Motor 0x4

#pragma pack(1)
typedef struct
{
    int8_t messageId;
    int8_t Temperature;
    int8_t Humidity;
    uint16_t Luminance;
} tag_app_Agriculture;

typedef struct
{
    int8_t messageId;
    uint16_t mid;
    int8_t errcode;
    int8_t Light_State;
} tag_app_Response_Agriculture_Control_Light;

typedef struct
{
    int8_t messageId;
    uint16_t mid;
    int8_t errcode;
    int8_t Motor_State;
} tag_app_Response_Agriculture_Control_Motor;

typedef struct
{
    int8_t messageId;
    uint16_t mid;
    char Light[3];
} tag_app_Agriculture_Control_Light;

typedef struct
{
    uint8_t messageId;
    uint16_t mid;
    char Motor[3];
} tag_app_Agriculture_Control_Motor;
#pragma pack()

//void *context;
int8_t qr_code = 1;
const unsigned char gImage_Huawei_IoT_QR_Code[114720];
unsigned char gImage_Bossaylogo[45128];
IoTBox_Lux_Data_TypeDef IoTBox_Lux_Data;
IoTBox_HT_Data_TypeDef IoTBox_HT_Data;

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
    {
        LCD_Show_Image(0,0,240,239,gImage_Huawei_IoT_QR_Code);
        printf("Not enough space,Huawei QR Code don't display! \n\r");
    }
	
	else
	{
		POINT_COLOR = RED;
		//LCD_ShowString(40, 10, 200, 16, 24, "");
		//LCD_ShowString(15, 50, 210, 16, 24, "");
        LCD_Show_Image(0,0,240,93,gImage_Bossaylogo);
        LCD_ShowString(180, 110, 200, 16, 16, "IA Demo");
		LCD_ShowString(10, 130, 200, 16, 16, "NCDP_IP:");
		LCD_ShowString(80, 130, 200, 16, 16, cn_app_server);
		LCD_ShowString(10, 150, 200, 16, 16, "NCDP_PORT:");
		LCD_ShowString(100, 150, 200, 16, 16, cn_app_port);
        LCD_ShowString(10, 180, 200, 16, 16, "Lux Value is:");
		LCD_ShowNum(140,180,(int)IoTBox_Lux_Data.Lux,5,16);
		LCD_ShowString(10, 200, 200, 16, 16, "Humidity is:");
		LCD_ShowNum(140,200,(int)IoTBox_HT_Data.Humidity,5,16);
		LCD_ShowString(10, 220, 200, 16, 16, "Temperature is:");
		LCD_ShowNum(140,220,(int)IoTBox_HT_Data.Temperature,5,16);

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
    tag_app_Response_Agriculture_Control_Light Response_Agriculture_Control_Light;
    tag_app_Response_Agriculture_Control_Motor Response_Agriculture_Control_Motor;
    tag_app_Agriculture_Control_Light *Agriculture_Control_Light;
    tag_app_Agriculture_Control_Motor *Agriculture_Control_Motor;
    int8_t msgid;

    while(1)
    {
        if(osal_semp_pend(s_rcv_sync,cn_osal_timeout_forever))
        {
            msgid = s_rcv_buffer[0] & 0x000000FF;
            switch (msgid)
            {
                case cn_app_Agriculture_Control_Light:
                    Agriculture_Control_Light = (tag_app_Agriculture_Control_Light *)s_rcv_buffer;
                    printf("Agriculture_Control_Light:msgid:%d mid:%d", Agriculture_Control_Light->messageId, ntohs(Agriculture_Control_Light->mid));
                    /********** code area for cmd from IoT cloud  **********/
                    if (Agriculture_Control_Light->Light[0] == 'O' && Agriculture_Control_Light->Light[1] == 'N')
                    {	
                        HAL_GPIO_WritePin(LIGHT_GPIO_Port, LIGHT_Pin, GPIO_PIN_SET);  				
                        Response_Agriculture_Control_Light.messageId = cn_app_response_Agriculture_Control_Light;
                    	Response_Agriculture_Control_Light.mid = Agriculture_Control_Light->mid;
                        Response_Agriculture_Control_Light.errcode = 0;
                		Response_Agriculture_Control_Light.Light_State = 1;
                        oc_lwm2m_report((char *)&Response_Agriculture_Control_Light,sizeof(Response_Agriculture_Control_Light),1000);    ///< report cmd reply message	
                    }
                    if (Agriculture_Control_Light->Light[0] == 'O' && Agriculture_Control_Light->Light[1] == 'F' && Agriculture_Control_Light->Light[2] == 'F')
                    {	
                        HAL_GPIO_WritePin(LIGHT_GPIO_Port, LIGHT_Pin, GPIO_PIN_RESET); 
                        Response_Agriculture_Control_Light.messageId = cn_app_response_Agriculture_Control_Light;
                    	Response_Agriculture_Control_Light.mid = Agriculture_Control_Light->mid;
                        Response_Agriculture_Control_Light.errcode = 0;
                		Response_Agriculture_Control_Light.Light_State = 0;
                        oc_lwm2m_report((char *)&Response_Agriculture_Control_Light,sizeof(Response_Agriculture_Control_Light),1000);    ///< report cmd reply message	
                    }
                    /********** code area end  **********/
                    break;
                case cn_app_Agriculture_Control_Motor:
                    Agriculture_Control_Motor = (tag_app_Agriculture_Control_Motor *)s_rcv_buffer;
                    printf("Agriculture_Control_Motor:msgid:%d mid:%d", Agriculture_Control_Motor->messageId, ntohs(Agriculture_Control_Motor->mid));
                    /********** code area for cmd from IoT cloud  **********/
                    if (Agriculture_Control_Motor->Motor[0] == 'O' && Agriculture_Control_Motor->Motor[1] == 'N')
                    {	
                        HAL_GPIO_WritePin(MOTOR_GPIO_Port, MOTOR_Pin, GPIO_PIN_SET); 
                        Response_Agriculture_Control_Motor.messageId = cn_app_response_Agriculture_Control_Motor;
                    	Response_Agriculture_Control_Motor.mid = Agriculture_Control_Motor->mid;
                        Response_Agriculture_Control_Motor.errcode = 0;
                		Response_Agriculture_Control_Motor.Motor_State = 1;
                        oc_lwm2m_report((char *)&Response_Agriculture_Control_Motor,sizeof(Response_Agriculture_Control_Motor),1000);    ///< report cmd reply message	
                    }
                    if (Agriculture_Control_Motor->Motor[0] == 'O' && Agriculture_Control_Motor->Motor[1] == 'F' && Agriculture_Control_Motor->Motor[2] == 'F')
                    {	
                        HAL_GPIO_WritePin(MOTOR_GPIO_Port, MOTOR_Pin, GPIO_PIN_RESET); 
                        Response_Agriculture_Control_Motor.messageId = cn_app_response_Agriculture_Control_Motor;
                    	Response_Agriculture_Control_Motor.mid = Agriculture_Control_Motor->mid;
                        Response_Agriculture_Control_Motor.errcode = 0;
                		Response_Agriculture_Control_Motor.Motor_State = 0;
                        oc_lwm2m_report((char *)&Response_Agriculture_Control_Motor,sizeof(Response_Agriculture_Control_Motor),1000);    ///< report cmd reply message	
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
    tag_app_Agriculture Agriculture;

    memset(&oc_param,0,sizeof(oc_param));

    oc_param.app_server.address = cn_app_server;
    oc_param.app_server.port = cn_app_port;
    oc_param.app_server.ep_id = cn_endpoint_id;
    oc_param.boot_mode = en_oc_boot_strap_mode_factory;
    oc_param.rcv_func = app_msg_deal;

    ret = oc_lwm2m_config(&oc_param);

    if (0 != ret)
    {
        return ret;
    }
        //install a dealer for the led message received
        while(1) //--TODO ,you could add your own code here
        {
            Agriculture.messageId = cn_app_Agriculture;
            Agriculture.Temperature = (int8_t)IoTBox_HT_Data.Temperature;
            Agriculture.Humidity = (int8_t)IoTBox_HT_Data.Humidity;
            Agriculture.Luminance = htons((uint16_t)IoTBox_Lux_Data.Lux);
            oc_lwm2m_report( (char *)&Agriculture, sizeof(Agriculture), 1000);
            osal_task_sleep(2*1000);
        }

    return ret;
}

static int app_collect_task_entry()
{
    Init_BS_IA_DEMO();	
    while (1)
    {
        IoTBox_Lux_Read_Data();
        IoTBox_Hum_Temp_Read_Data();

        printf("\r\n******************************Lux Value is  %d\r\n", (int)IoTBox_Lux_Data.Lux);
		printf("\r\n******************************Humidity is  %d\r\n", (int)IoTBox_HT_Data.Humidity);
		printf("\r\n******************************Temperature is  %d\r\n", (int)IoTBox_HT_Data.Temperature);
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
    //osal_task_create("app_led",app_led_task_entry,NULL,0x400,NULL,3);

    stimer_create("lcdtimer",timer1_callback,NULL,8*1000,cn_stimer_flag_start);

    return 0;

}





