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
#include "Actuators.h"
#include "lcd.h"

#include <gpio.h>
#include <stm32l4xx_it.h>



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

void *context;
int8_t qr_code = 1;
const unsigned char gImage_Huawei_IoT_QR_Code[114720];
const unsigned char gImage_Bossaylogo[45128];
IotBox_Lux_Data_TypeDef IotBox_Lux_Data;
IotBox_HT_Data_TypeDef IotBox_HT_Data;

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
		//LCD_ShowString(40, 10, 200, 16, 24, "");
		//LCD_ShowString(15, 50, 210, 16, 24, "");
        LCD_Show_Image(0,0,240,93,gImage_Bossaylogo);
        LCD_ShowString(180, 110, 200, 16, 16, "IA Demo");
        LCD_ShowString(10, 180, 200, 16, 16, "Lux Value is:");
		LCD_ShowNum(140,180,(int)IotBox_Lux_Data.Lux,5,16);
		LCD_ShowString(10, 200, 200, 16, 16, "Humidity is:");
		LCD_ShowNum(140,200,(int)IotBox_HT_Data.Humidity,5,16);
		LCD_ShowString(10, 220, 200, 16, 16, "Temperature is:");
		LCD_ShowNum(140,220,(int)IotBox_HT_Data.Temperature,5,16);

	}
}


static int app_collect_task_entry()
{
    Test_Board_DEMO();	
    while (1)
    {
        IotBox_Lux_Read_Data();
        IotBox_Hum_Temp_Read_Data();

        printf("\r\n******************************Lux Value is  %d\r\n", (int)IotBox_Lux_Data.Lux);
		printf("\r\n******************************Humidity is  %d\r\n", (int)IotBox_HT_Data.Humidity);
		printf("\r\n******************************Temperature is  %d\r\n", (int)IotBox_HT_Data.Temperature);
        if (qr_code == 0)
        {
            // LCD_ShowString(10, 200, 200, 16, 16, "BH1750 Value is:");
            // LCD_ShowNum(140, 200, lux, 5, 16);

        }
        osal_task_sleep(2*1000);
    }

    return 0;
}








