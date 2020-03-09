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
#include "main.h"
#include "sys_init.h"
#include "mem.h"
#include "Actuators.h"
#include "los_base.h"
#include "los_task.ph"
#include "los_typedef.h"
#include "los_sys.h"
#include "stm32l4xx_hal.h"
#include "osal.h"


#if defined (__CC_ARM)
extern char Image$$RW_IRAM1$$ZI$$Limit [];
extern char Image$$ARM_LIB_STACKHEAP$$Base [];
#elif defined (__GNUC__)
extern char __los_heap_addr_start__ [];
extern char __los_heap_addr_end__ [];
#else
#error "fix me"
#endif

const struct phys_mem system_phys_mem [] =
    {
#if defined (__CC_ARM)
        { Image$$RW_IRAM1$$ZI$$Limit, Image$$ARM_LIB_STACKHEAP$$Base },
#elif defined (__GNUC__)
        {(unsigned long) __los_heap_addr_start__,(unsigned long) __los_heap_addr_end__ },
#else
#error "fix me"
#endif
        { 0, 0 }
    };
extern const unsigned char gImage_Bossaylogo[45128];
VOID HardWare_Init(VOID)
{
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();
	MX_GPIO_Init();
	MX_I2C1_Init();
    MX_I2C2_Init();
	dwt_delay_init(SystemCoreClock);
	LCD_Init();		
	LCD_Clear(WHITE);		   	
	POINT_COLOR = RED;
    LCD_Show_Image(0,0,240,93,gImage_Bossaylogo);			
	//LCD_ShowString(10, 50, 240, 24, 24, "Welcome to Bossay!");
	//LCD_ShowString(20, 90, 240, 16, 16, "IoT Deveoper Box");
	LCD_ShowString(20, 130, 240, 16, 16, "Powered by Huawei LiteOS!");
	LCD_ShowString(2, 170, 240, 16, 16, "System intializing......");;	
}

void delay_us(uint32_t nus)
{
 uint32_t temp;
 SysTick->LOAD = 9*nus;
 SysTick->VAL=0X00;//清空计数器
 SysTick->CTRL=0X01;//使能，减到零是无动作，采用外部时钟源
 do
 {
  temp=SysTick->CTRL;//读取当前倒计数值
 }while((temp&0x01)&&(!(temp&(1<<16))));//等待时间到达
     SysTick->CTRL=0x00; //关闭计数器
    SysTick->VAL =0X00; //清空计数器
}
void delay_ms(uint16_t nms)
{
 uint16_t temp;
 SysTick->LOAD = 9000*nms;
 SysTick->VAL=0X00;//清空计数器
 SysTick->CTRL=0X01;//使能，减到零是无动作，采用外部时钟源
 do
 {
  temp=SysTick->CTRL;//读取当前倒计数值
 }while((temp&0x01)&&(!(temp&(1<<16))));//等待时间到达
    SysTick->CTRL=0x00; //关闭计数器
    SysTick->VAL =0X00; //清空计数器
}



extern int link_main(void *args);
static int link_test()
{
    int ret = -1;
    UINT32 uwRet = LOS_OK;
    UINT32  handle;
    TSK_INIT_PARAM_S task_init_param;

    memset (&task_init_param, 0, sizeof (TSK_INIT_PARAM_S));
    task_init_param.uwArg = (unsigned int)NULL;
    task_init_param.usTaskPrio = 2;
    task_init_param.pcName =(char *) "link_main";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)link_main;
    task_init_param.uwStackSize = 0x1000;
    uwRet = LOS_TaskCreate(&handle, &task_init_param);
    if(LOS_OK == uwRet){
        ret = 0;
    }
    return ret;
}

int main(void)
{
    UINT32 uwRet = LOS_OK;
    HardWare_Init();
    uwRet = LOS_KernelInit();
    if (uwRet != LOS_OK)
    {
        return LOS_NOK;
    }

    extern void shell_uart_init(int baud);
    shell_uart_init(115200);

    link_test();

    (void)LOS_Start();
    return 0;
}
