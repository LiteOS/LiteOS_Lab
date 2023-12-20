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

#include <stdio.h>
#include "link_main.h"
#include "iot_link_config.h"
#include "link_version.h"
#include "cmsis_os2.h"
#include "osal.h"

#ifdef CONFIG_MACOS_ENABLE
    #include <sys/select.h>
#endif

#define  CN_LINK_VERSION_MAJOR      2
#define  CN_LINK_VERSION_MINOR      2
#define  CN_LINK_VERSION_FEATURE    2

static char s_link_mainversion[64];
const char *linkmain_version(void)
{
    (void) snprintf(s_link_mainversion,64,"V%d.%d.%d AT %s ON %s",CN_LINK_VERSION_MAJOR,\
            CN_LINK_VERSION_MINOR,CN_LINK_VERSION_FEATURE,__TIME__,__DATE__);
    return s_link_mainversion;
}

static int s_link_start = 0;
int link_main(void *args)
{
    // install the RTOS kernel for the link
    if(s_link_start)
    {
       return -1;
    }
    s_link_start =1;

    (void)osal_init();
    LINK_LOG_DEBUG("linkmain:%s",linkmain_version());

// install the cJSON, for the oc mqtt agent need the cJSON
#ifdef CONFIG_CJSON_ENABLE
    #include "cJSON.h"

    cJSON_Hooks  hook;
    hook.free_fn = osal_free;
    hook.malloc_fn = osal_malloc;
    cJSON_InitHooks(&hook);
#endif

//////////////////////////  TCPIP PROTOCOL /////////////////////////////////////
#ifdef CONFIG_TCPIP_AL_ENABLE
    #include "sal.h"
    (void)link_tcpip_init();
#endif

//////////////////////////  DTLS PROTOCOL  /////////////////////////////////////
#ifdef CONFIG_DTLS_AL_ENABLE
    #include "dtls_al.h"
    (void)dtls_al_init();
#endif

//////////////////////////  MQTT PROTOCOL  /////////////////////////////////////
#ifdef CONFIG_MQTT_AL_ENABLE
    #include "mqtt_al.h"
    mqtt_al_init();
#endif

//////////////////////////  OC MQTT  //////////////////////////////////
#ifdef CONFIG_OCMQTT_ENABLE
    #include "oc_mqtt_al.h"
    (void)oc_mqtt_init();
#endif

////////////////////////////  LINK DEMO ////////////////////////////////////////
#ifdef CONFIG_LINKDEMO_ENABLE
    extern int standard_app_demo_main(void);
    (void) standard_app_demo_main();
#endif

    return 0;
}

void link_main_task_entry(void) 
{
    osThreadAttr_t attr;
    attr.name = "link_main_task_entry";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 4;
    attr.priority = 30;
    if (osThreadNew((osThreadFunc_t)link_main, NULL, &attr) == NULL) {
        printf("Failed to create link_main_task_entry!\n");
    } else {
         printf("create link_main_task_entry successful!\n");
    }
}
