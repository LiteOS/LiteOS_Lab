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
 *  2019-04-28 15:00  zhangqianfu  The first version  
 *
 */

//RTOS KERNEL
#include <osal.h>
#include <liteos_imp.h>

//debug
#include <osal.h>
#include <shell.h>

//tcpip stack
#include <sal.h>
#include <lwip_imp.h>

//mqtt protocol
#include <mqtt_al.h>
#include <mqtt_port.h>

extern int netdriver_init();
int __attribute__((weak))  netdriver_install()
{
    printf("please remember to supply a netdriver---- please\n\r");

    return -1;
}

int link_main(void *args)
{
    //install the RTOS kernel for the link
    osal_init();
    osal_install_liteos();

    //install the shell for the link
    extern void shell_uart_init(int baud);
    shell_uart_init(115200);
    shell_init();

    //install the tcpip stack and net driver for the link
    tcpip_sal_init(10);
    tcpip_sal_install_lwip(netdriver_init);

    //install the mqtt for the link
    mqtt_al_init();
    mqtt_al_install_pahomqtt();

    //oc mqtt service for the link
    oc_mqtt_init();

    extern int oc_mqtt_demo_main();
    oc_mqtt_demo_main();

    return 0;
}


