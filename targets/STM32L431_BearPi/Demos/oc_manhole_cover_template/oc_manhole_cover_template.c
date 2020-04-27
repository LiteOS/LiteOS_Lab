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
 *  2019-11-27 10:12  zhangzhongfeng  The first version  
 *
 */
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <osal.h>
#include <oc_lwm2m_al.h>
#include <link_endian.h>

#include "E53_SC2.h"
#include "lcd.h"

#include <gpio.h>
#include <stm32l4xx_it.h>

#define cn_endpoint_id        "BearPi_0001"
#define cn_app_server         "119.3.250.80"
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

#define cn_app_Manhole_Cover 0x0

#pragma pack(1)
typedef struct
{
    int8u messageId;
    int8_t Temperature;
    int16_t Accel_x;
    int16_t Accel_y;
    int16_t Accel_z;
    string Status[5];
} tag_app_Manhole_Cover;
#pragma pack()


E53_SC2_Data_TypeDef E53_SC2_Data;
tag_app_Manhole_Cover Manhole_Cover;


//if your command is very fast,please use a queue here--TODO
#define cn_app_rcv_buf_len 128
static int             s_rcv_buffer[cn_app_rcv_buf_len];
static int             s_rcv_datalen;
static osal_semp_t     s_rcv_sync;


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

static int app_report_task_entry()
{
    int ret = -1;

    oc_config_param_t      oc_param;
    
    memset(&oc_param,0,sizeof(oc_param));

    oc_param.app_server.address = cn_app_server;
    oc_param.app_server.port = cn_app_port;
    oc_param.app_server.ep_id = cn_endpoint_id;
    oc_param.boot_mode = en_oc_boot_strap_mode_factory;
    oc_param.rcv_func = app_msg_deal;

    ret = oc_lwm2m_config( &oc_param);
    if (0 != ret)
    {
    	return ret;
    }
    //install a dealer for the led message received
    while(1) //--TODO ,you could add your own code here
    {
        Manhole_Cover.messageId = cn_app_Manhole_Cover;
        Manhole_Cover.Temperature = (int)E53_SC2_Data.Temperature;
        Manhole_Cover.Accel_x = htons(E53_SC2_Data.Accel[0] & 0x0000FFFF);
        Manhole_Cover.Accel_y = htons(E53_SC2_Data.Accel[1] & 0x0000FFFF);
        Manhole_Cover.Accel_z = htons(E53_SC2_Data.Accel[2] & 0x0000FFFF);
        oc_lwm2m_report((char *)&Manhole_Cover, sizeof(Manhole_Cover), 1000);
        osal_task_sleep(2*1000);
    }

    return ret;
}

int X = 0,Y = 0,Z = 0;
static int app_collect_task_entry()
{
    Init_E53_SC2();
    while (1)
    {
        E53_SC2_Read_Data();
        printf("\r\n******************************Temperature      is  %d\r\n", (int)E53_SC2_Data.Temperature);
        printf("\r\n******************************Accel[0]         is  %d\r\n", (int)E53_SC2_Data.Accel[0]);
        printf("\r\n******************************Accel[1]         is  %d\r\n", (int)E53_SC2_Data.Accel[1]);
        printf("\r\n******************************Accel[2]         is  %d\r\n", (int)E53_SC2_Data.Accel[2]);
        if( X == 0 && Y == 0 && Z == 0)
        {
            X = (int)E53_SC2_Data.Accel[0];
            Y = (int)E53_SC2_Data.Accel[1];
            Z = (int)E53_SC2_Data.Accel[2];
        }
        else
        {
            if(X+100<E53_SC2_Data.Accel[0]||X-100>E53_SC2_Data.Accel[0]||Y+100<E53_SC2_Data.Accel[1]||Y-100>E53_SC2_Data.Accel[1]||Z+100<E53_SC2_Data.Accel[2]||Z-100>E53_SC2_Data.Accel[2])
            {
                HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);
                HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);
                Manhole_Cover.Status[0] = ' ';
                Manhole_Cover.Status[1] = 'T';
                Manhole_Cover.Status[2] = 'i';
                Manhole_Cover.Status[3] = 'l';
                Manhole_Cover.Status[4] = 't';

            }
            else
            {
                HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
                Manhole_Cover.Status[0] = 'L';
                Manhole_Cover.Status[1] = 'e';
                Manhole_Cover.Status[2] = 'v';
                Manhole_Cover.Status[3] = 'e';
                Manhole_Cover.Status[4] = 'l';
            }
        }
		LCD_ShowString(10, 135, 200, 16, 16, "Temperature:");
		LCD_ShowNum(140, 135, (int)E53_SC2_Data.Temperature, 5, 16);
		LCD_ShowString(10, 160, 200, 16, 16, "Acce_X:");
		LCD_ShowNum(140, 160, (int)E53_SC2_Data.Accel[0], 5, 16);
		LCD_ShowString(10, 185, 200, 16, 16, "Acce_Y:");
		LCD_ShowNum(140, 185, (int)E53_SC2_Data.Accel[1], 5, 16);
		LCD_ShowString(10, 210, 200, 16, 16, "Acce_Z:");
		LCD_ShowNum(140, 210, (int)E53_SC2_Data.Accel[2], 5, 16);
        osal_task_sleep(2*1000);
    }
    return 0;
}


#include <stimer.h>

int standard_app_demo_main()
{
	LCD_Clear(BLACK);
	POINT_COLOR = GREEN;
	LCD_ShowString(10, 10, 200, 16, 24, "Welcome to BearPi");
	LCD_ShowString(30, 40, 200, 16, 24, "Manhole Demo");
	LCD_ShowString(10, 80, 200, 16, 16, "NCDP_IP:");
	LCD_ShowString(80, 80, 200, 16, 16, cn_app_server);
	LCD_ShowString(10, 110, 200, 16, 16, "NCDP_PORT:");
	LCD_ShowString(100, 110, 200, 16, 16, cn_app_port);

	osal_semp_create(&s_rcv_sync,1,0);

    osal_task_create("app_collect",app_collect_task_entry,NULL,0x400,NULL,3);
    osal_task_create("app_report",app_report_task_entry,NULL,0x1000,NULL,2);



    return 0;
}





