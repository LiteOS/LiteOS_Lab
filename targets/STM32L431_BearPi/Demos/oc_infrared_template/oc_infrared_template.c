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

#include "E53_IS1.h"
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

#define cn_app_infrared 0x0

#pragma pack(1)
typedef struct
{
    int8u messageId;
    string Status[4];
} tag_app_infrared;
#pragma pack()


tag_app_infrared infrared;


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
        infrared.messageId = cn_app_infrared;
        oc_lwm2m_report((char *)&infrared, sizeof(infrared), 1000);
        osal_task_sleep(2*1000);
    }


    return 0;
}

static int app_collect_task_entry()
{
    Init_E53_IS1();
    while (1)
    {
        if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9)==SET)
        {
            
            E53_IS1_Beep_StatusSet(ON);
            infrared.Status[0] = 'H';
            infrared.Status[1] = 'a';
            infrared.Status[2] = 'v';
            infrared.Status[3] = 'e';

        }
        else
        {
            E53_IS1_Beep_StatusSet(OFF);
            infrared.Status[0] = ' ';
            infrared.Status[1] = ' ';
            infrared.Status[2] = 'N';
            infrared.Status[3] = 'O';
        }
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
	LCD_ShowString(40, 50, 200, 16, 24, "Infrared Demo");
	LCD_ShowString(10, 90, 200, 16, 16, "NCDP_IP:");
	LCD_ShowString(80, 90, 200, 16, 16, cn_app_server);
	LCD_ShowString(10, 130, 200, 16, 16, "NCDP_PORT:");
	LCD_ShowString(100, 130, 200, 16, 16, cn_app_port);

	osal_semp_create(&s_rcv_sync,1,0);

    osal_task_create("app_collect",app_collect_task_entry,NULL,0x400,NULL,3);
    osal_task_create("app_report",app_report_task_entry,NULL,0x1000,NULL,2);


    return 0;
}





