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

#include <link_version.h>
//RTOS KERNEL
#include <osal.h>

#if CONFIG_MACOS_ENABLE
    #include <sys/select.h>
#endif


#ifdef WITH_DTLS
#include <dtls_interface.h>

#endif



#define  CN_LINK_VERSION_MAJOR      1
#define  CN_LINK_VERSION_MINOR      3
#define  CN_LINK_VERSION_FEATURE    1


static char s_link_mainversion[64];
const char *linkmain_version()
{
    snprintf(s_link_mainversion,64,"V%d.%d.%d AT %s ON %s",CN_LINK_VERSION_MAJOR,\
            CN_LINK_VERSION_MINOR,CN_LINK_VERSION_FEATURE,__TIME__,__DATE__);
    return s_link_mainversion;
}


static int s_link_start = 0;
int link_main(void *args)
{
    ///< install the RTOS kernel for the link
    if(s_link_start)
    {
       return -1;
    }
    s_link_start =1;

    osal_init();
    printf("linkmain:%s \n\r",linkmain_version());

#if CONFIG_STIMER_ENABLE
    #include <stimer.h>
    stimer_init();
#endif

#if CONFIG_SHELL_ENABLE
    #include <shell.h>
    shell_init();
#endif

    /* add loader code here */
#if CONFIG_OTA_ENABLE
    extern void hal_init_ota(void);
    hal_init_ota();
#endif

#if CONFIG_LOADER_ENABLE
    printf("loader main!\n");
    extern int ota_detection();
    ota_detection();
    loader_main();
    return;
#endif
    /* add loader code here end */

///< install the driver framework
#if CONFIG_DRIVER_ENABLE
    #include <driver.h>
    ///< install the driver framework for the link
    los_driv_init();
#endif


///< install the at framework
#if CONFIG_AT_ENABLE
    #include <at.h>
    #include <iot_link_config.h>
    extern bool_t uart_at_init(int baud);


    #ifndef CONFIG_AT_BAUDRATE
    #define CONFIG_AT_BAUDRATE  9600
    #endif

    #ifndef CONFIG_AT_DEVICENAME
    #define CONFIG_AT_DEVICENAME  "atdev"
    #endif


    ///< install the at framework for the link
    uart_at_init(CONFIG_AT_BAUDRATE);
    at_init(CONFIG_AT_DEVICENAME);
#endif

///< initialize the service
#if CONFIG_SERVICE_ENABLE
    void service_init (void);
    service_init ();
#endif

///< install the cJSON, for the oc mqtt agent need the cJSON
#if CONFIG_JSON_ENABLE
    #include <cJSON.h>

    cJSON_Hooks  hook;
    hook.free_fn = osal_free;
    hook.malloc_fn = osal_malloc;
    cJSON_InitHooks(&hook);
#endif


//////////////////////////  TCPIP PROTOCOL  ////////////////////////////////////

#if CONFIG_TCPIP_ENABLE
    #include <sal.h>
    link_tcpip_init();
#endif

//////////////////////////  DTLS PROTOCOL  /////////////////////////////////////
#ifdef WITH_DTLS
    dtls_init();
#endif

//////////////////////////  MQTT PROTOCOL  /////////////////////////////////////
#if CONFIG_MQTT_ENABLE
    #include <mqtt_al.h>
    mqtt_al_init();
#if CONFIG_MQTT_PAHO_ENABLE
    #include <paho_mqtt_port.h>
    mqtt_install_pahomqtt();
#elif CONFIG_MQTT_SINN_ENABLE
    #include <mqtt_sinn_port.h>
    mqtt_install_sinnmqtt();
#endif

#endif


//////////////////////////  COAP PROTOCOL  /////////////////////////////////
#if CONFIG_LITE_COAP_ENABLE
    #include <litecoap_port.h>
    coap_install_litecoap();
#elif CONFIG_LIBCOAP_ENABLE
    #include <libcoap_port.h>
    coap_install_libcoap();
#endif

//////////////////////////  LWM2M PROTOCOL  /////////////////////////////////
#if CONFIG_WAKAAMA_ENABLE
    #include <lwm2m_port.h>
    lwm2m_install();
#endif

//////////////////////////  OC MQTT  //////////////////////////////////

#if CONFIG_OC_MQTT_ENABLE
    #include <oc_mqtt_al.h>
    oc_mqtt_init();

    #if CONFIG_ATINY_MQTT_ENABLE
        #include <atiny_mqtt.h>
        oc_mqtt_install_atiny_mqtt();
    #endif


    #if CONFIG_OC_MQTT_TINY_ENABLE
        #include <oc_mqtt_tiny.h>
        oc_mqtt_tiny_install();
    #endif


    #if CONFIG_OC_MQTT_EC2X_ENABLE
        #include <ec2x_oc.h>
        ec2x_oc_init();
    #endif


#endif

////////////////////////////  OC LWM2M ///////     /////////////////////////////

#if CONFIG_OC_LWM2M_ENABLE
    #include <oc_lwm2m_al.h>
    oc_lwm2m_init();

    #if CONFIG_OC_LWM2M_AGENT_ENABLE
        #include <agent_lwm2m.h>
        oc_lwm2m_install_agent();
    #endif

    #if CONFIG_OC_LWM2M_BOUDICA150_ENABLE
        #include <boudica150_oc.h>
        #define cn_app_bands    "5,8,20"
        boudica150_init(NULL,NULL,cn_app_bands);
    #endif

#endif

////////////////////////////  OC COAP ////////     /////////////////////////////
#if CONFIG_OC_COAP_ENABLE
	#include <oc_coap_al.h>
    oc_coap_init();

    #if CONFIG_ATINY_COAP_ENABLE
        #include <atiny_coap.h>
        oc_coap_install_agent();
    #endif

#endif

#if CONFIG_DEMOS_ENABLE
    extern int standard_app_demo_main();
    standard_app_demo_main();
#endif

#if CONFIG_AUTO_TEST
    #include <test_case.h>
    autotest_start();
#endif

    return 0;
}


