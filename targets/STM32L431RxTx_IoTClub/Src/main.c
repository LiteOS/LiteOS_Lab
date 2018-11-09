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

#include "sys_init.h"

/****** chose a sensor for your application*****/

//led control
void led_on(void)
{
	HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_SET);    // ����ߵ�ƽ
}

void led_off(void)
{
    HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_RESET);  // ����͵�ƽ
}

void led_display(void)
{
	char *str = "PoweredBy LiteOS";
	OLED_Init();
	OLED_Clear();
	OLED_ShowCHinese(0,0,0);
	OLED_ShowCHinese(18,0,1);
	OLED_ShowCHinese(36,0,2);
	OLED_ShowCHinese(54,0,3);
	OLED_ShowCHinese(72,0,4);
	OLED_ShowCHinese(90,0,5);
	OLED_ShowString(0,2,(uint8_t*)str,16);
}

VOID HardWare_Init(VOID)
{
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	DelayInit();	
	MX_GPIO_Init();
	//MX_USART1_UART_Init();
	//MX_USART3_UART_Init();
	//MX_ADC1_Init();
	MX_I2C1_Init();
}


#include <osport.h>
#include <at.h>
#include <shell.h>

int main(void)
{
    UINT32 uwRet = LOS_OK;
    //do the hardware initialize
    HardWare_Init();
    
    //do the liteos kernel initialize
    uwRet = LOS_KernelInit();
    if (uwRet != LOS_OK)
    {
        return LOS_NOK;
    }
    #if 1
    //////////////////////APPLICATION INITIALIZE HERE/////////////////////
    //do the shell module initlialize:use uart 1
    extern void uart_debug_init(s32_t baud);
    uart_debug_init(115200);
    shell_install();
    
    //do the at module initialize:use uart 2
    extern bool_t uart_at_init(s32_t baudrate);
    extern s32_t uart_at_send(u8_t *buf, s32_t len,u32_t timeout);
    extern s32_t uart_at_receive(u8_t *buf,s32_t len,u32_t timeout);
    uart_at_init(9600);
    at_install(uart_at_receive,uart_at_send);
    
    //create the main task of the application
    extern u32_t app_main(void *args);
    task_create("appmain",app_main,0x800,NULL,NULL,12);
    #endif
    ////////////////////////APPLICATION INITIALIZE END///////////////////
    //start the system
    LOS_Start();
}
