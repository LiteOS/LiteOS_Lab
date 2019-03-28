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

#include <shell.h>
#include <los_dev.h>
#include <osport.h>
#include <at.h>

#include <mem.h>

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
        { __los_heap_addr_start__, __los_heap_addr_end__ },
#else
#error "unsupported tool!"
#endif
        { 0, 0 }
    };

VOID HardWare_Init(VOID)
{
	HAL_Init();
	/*configure the system clock*/
	SystemClock_Config();

	/* Initialize all configured peripherals */
	DelayInit();
	MX_GPIO_Init();
	//MX_I2C1_Init();
}

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
    //system frame work initilized here
    los_shell_init();     //which means you could use shell
    los_driv_init();     //which means you could use the driver framwork
    los_at_init("atdev"); //which means you could use the at framework
    //////////////////////APPLICATION INITIALIZE HERE/////////////////////
    //do the shell module initlialize:use uart 1
    extern void uart_debug_init(s32_t baud);
    uart_debug_init(115200);
    //do the at module initialize:use uart 2
    extern bool_t uart_at_init(s32_t baudrate);
    uart_at_init(9600);
     
#if 1 
    //create the main task of the application
    #include <app_main.h>
    //app_main_init("178.15.147.143", 5683,20);
    //app_main_init("testdevice.hw-oc.com", 5683,20);  //Europe openlab use this configure


    //this is used in dus using 1nce card
//    tagNbConfig config;
//    config.server = "178.15.147.143,5683";
//    config.bands = "8,5,20";
//    config.plmn = "26201";
//    config.apn = "1,\"IP\",\"iot.1nce.net\"";
//    app_main_init(&config);


    //this is used in china for China Telecom
//    tagNbConfig config;
//    config.server = "49.4.85.232,5683";
//    config.bands = "5,8,20";
//    config.plmn = "46011";
//    config.apn = "1,\"IP\",\"HUAWEI.COM\"";

    //this is used in china for China  Mobile
    tagNbConfig config;
    config.server = "49.4.85.232,5683";
    config.bands = "8,5,20";
    config.plmn = "46000" ;
    config.apn = "1,\"IP\",\"HUAWEI.COM\"";

    app_main_init(&config);  //China telecom
 #endif
    ////////////////////////APPLICATION INITIALIZE END///////////////////
    //start the system
    LOS_Start();
}
