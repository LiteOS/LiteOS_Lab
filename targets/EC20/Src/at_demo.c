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
#include <stdio.h>
#include <pthread.h>
#include <osal.h>
#if cfg_liteos_enable
#include <liteos_imp.h>
#elif cfg_linux_enable
#include <linux_imp.h>
#elif cfg_macos_enable
#include <macos_imp.h>
#else
#error("no os supplied yet");
#endif


#ifdef WITH_DTLS
#include <dtls_interface.h>

#endif

#include <at_interface_mqtt.h>


int main(void)
{
        ///< install the RTOS kernel for the link
        osal_init();
    
    
#if cfg_liteos_enable
        osal_install_liteos();
#elif cfg_linux_enable
        osal_install_linux();
#elif  cfg_macos_enable
        osal_install_macos();
#endif
    
#if cfg_shell_enable
    #include <shell.h>
        ///< install the shell for the link
        extern void shell_uart_init(int baud);
        shell_uart_init(115200);
        shell_init();
#endif
    
        ///< install the cJSON, for the oc mqtt agent need the cJSON
#if cfg_json_enable
    #include <cJSON.h>
    
        cJSON_Hooks  hook;
        hook.free_fn = osal_free;
        hook.malloc_fn = osal_malloc;
        cJSON_InitHooks(&hook);
#endif
        ///< install the tcpip stack and net driver for the link
#if cfg_tcpip_enable
    #include <sal.h>
        tcpipstack_init(10);
    
    #if cfg_lwip_enable
        #include <lwip_imp.h>
            tcpipstack_install_lwip(netdriver_install);
    #elif cfg_linux_socket_enable
        #include <linux_socket_imp.h>
            tcpipstack_install_linux_socket();
    #elif cfg_macos_socket_enable
        #include <macos_socket_imp.h>
            tcpipstack_install_macos_socket();
    #else
    
    #endif
    
#endif
    
#ifdef WITH_DTLS
    
        dtls_init();
#endif
    
    //////////////////////////    MQTT PROTOCOL  /////////////////////////////////////
#if cfg_mqtt_enable
    #include <mqtt_al.h>
    #include <paho_mqtt_port.h>
    
        mqtt_init();
        mqtt_install_pahomqtt();
#endif
    
    //////////////////////////    OC MQTT && DEMOS  //////////////////////////////////
    
#if cfg_oc_mqtt_enable
    #include <oc_mqtt_al.h>
    //#include <oc_mqtt_demo.h>
        oc_mqtt_init();
    
#if cfg_atiny_mqtt_enable
    #include <atiny_mqtt.h>
        oc_mqtt_install_atiny_mqtt();
#endif
        //oc_mqtt_demo_main();
     hwoc_mqtt_connect(1,"119.3.184.255","8883","31415926", "9f825c0ed3e95ea3d459");
     printf("conned\r\n");
    tag_oc_mqtt_report  report;
    tag_key_value_list   list;
    int leftpower = 0;
    int err_int = 1;
    int mid_int = 1;
    char   *buf;
    int times= 0;

    while(1)
    {
        leftpower = (leftpower + 7 )%100;
        list.item.name = "batteryLevel";
        list.item.buf = (char *)&leftpower;
        list.item.len = sizeof(leftpower);
        list.item.type = en_key_value_type_int;
        list.next = NULL;

        report.hasmore = en_oc_mqtt_has_more_no;
        report.paralst= &list;
        report.serviceid = "Battery";
        report.eventtime = "20190508T112020Z";

        buf = cJSON_Print(oc_mqtt_json_fmt_report(&report));

        if(NULL != buf)
        {
            hwoc_mqtt_send(en_mqtt_al_qos_1, buf,strlen(buf));
            osal_task_sleep(1000);
            printf("times:%d power:%d\r\n",times++,leftpower);
        }
    }

#endif
    
    ////////////////////////////  OC LWM2M && DEMOS     /////////////////////////////
    
#if cfg_oc_lwm2m_enable
    #include <oc_lwm2m_al.h>
    #include <oc_lwm2m_demo.h>
        oc_lwm2m_init();
    
#if cfg_oc_lwm2m_agent_enable
    #include <agent_lwm2m.h>
        oc_lwm2m_install_agent();
#endif
    
        oc_lwm2m_demo_main();
    
    
#endif
    while(1)
    {
        sleep(100); ///< should hung the process
    }
    return 0;
}
