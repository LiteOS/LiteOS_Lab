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
 *  2020-04-21    	  FanXinhao    The first version  
 *
 */
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <los_typedef.h>
#include <los_task.h>
#include <boudica120_oc.h>
#include <osal.h>
#include <oc_coap_al.h>

#include "dht11.h"
#include "oled.h"

//DHT11 Data Struct
DHT11_Data_TypeDef dht11_data;
unsigned char DHT11_str[10] = {0};


/* LWM2M server information */
#define cn_endpoint_id        "coap_001"
#define cn_app_server         "119.3.250.80"
#define cn_app_port           "5683"

//if your command is very fast,please use a queue here--TODO
#define cn_app_rcv_buf_len 128
static int8_t          s_rcv_buffer[cn_app_rcv_buf_len];
static int             s_rcv_datalen;
static osal_semp_t     s_rcv_sync;

//use this function to push all the message to the buffer
static int app_msg_deal(void *msg, int len)
{
    int ret = -1;

    if(len <= cn_app_rcv_buf_len)
    {
        memcpy(s_rcv_buffer,msg,len);
        s_rcv_datalen = len;

        osal_semp_post(s_rcv_sync);

        ret = 0;

    }
    return ret;
}

int coap_report_task(void *args)
{
    int* handle = NULL;
    oc_config_param_t      oc_param;
    char Temp[10] = {00, 22};
	oc_coap_imp_init();

    memset(&oc_param,0,sizeof(oc_param));

    oc_param.app_server.address = cn_app_server;
    oc_param.app_server.port = cn_app_port;
    oc_param.app_server.ep_id = cn_endpoint_id;
    oc_param.boot_mode = en_oc_boot_strap_mode_factory;
    oc_param.rcv_func = app_msg_deal;

    handle = oc_coap_config(&oc_param);
    if (NULL == handle)
    {
		printf("config_init fail!\r\n");
    }
	else
	{
		printf("start send message to CDP server!\r\n");
	}
	while(1)
	{ 
        memset(DHT11_str, 0, 6);
        sprintf(Temp,"%02d",(int)dht11_data.temperature);
		oc_coap_report(handle, Temp, 2);
		osal_task_sleep(10*1000);
	}
}

int Read_DHT11_task(void *args)
{

	while(1)
	{
		DHT11_Read_TempAndHumidity(&dht11_data);//read DHT11
        /* oled display */
        OLED_Clear();
        OLED_ShowString(9, 0, "DHT11 Data", 10);
        memset(DHT11_str, 0, 6);
        sprintf(DHT11_str,"temp %5.2f",dht11_data.temperature);
        OLED_ShowString(9, 2, DHT11_str, 5);
        memset(DHT11_str, 0, 6);
        sprintf(DHT11_str,"humid %5.2f",dht11_data.humidity);
        OLED_ShowString(9, 4, DHT11_str, 5);

		osal_task_sleep(5*1000);

	}
}

int standard_app_demo_main()
{
    osal_task_create("coap_report",coap_report_task, NULL, 0x1000, NULL, 3);
	osal_task_create("Read_DHT11",Read_DHT11_task, NULL, 0x500, NULL, 3);
    return 0;
}





