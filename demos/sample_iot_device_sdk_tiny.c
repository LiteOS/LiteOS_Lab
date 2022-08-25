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

// Initialize iotlink with the original create task, here are some demos of creating task by os
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "iot_config.h"

extern int link_main(void *args); 

#ifdef CONFIG_LITEOS_ENABLE
    #include "los_base.h"
    #include "los_task.ph"
    #include "los_typedef.h"
    #include "los_sys.h"
    void create_task_by_liteos(void)
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
#endif

#ifdef CONFIG_FREERTOS_ENABLE
    #include "FreeRTOS.h"
    #include "task.h"
    void create_task_by_freertos(void)
    {
        xTaskCreate(link_main,	/* The function that implements the task. */
                "link_main",			/* Just a text name for the task to aid debugging. */
                1000,	/* The stack size is defined in FreeRTOSIPConfig.h. */
                (void *)x,		/* The task parameter, not used in this case. */
                16,		/* The priority assigned to the task is defined in FreeRTOSConfig.h. */
                NULL);				/* The task handle is not used. */   
    }
#endif

#ifdef CONFIG_OPENHARMONYOS_ENABLE
#include "cmsis_os2.h"
    void create_task_by_openharmonyos(void)
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
#endif

void link_main_task_entry(void) 
{
#ifdef CONFIG_LITEOS_ENABLE
    create_task_by_liteos(void);
#endif
#ifdef CONFIG_FREERTOS_ENABLE
    create_task_by_freertos(void);
#endif
#ifdef CONFIG_OPENHARMONYOS_ENABLE
    create_task_by_openharmonyos(void);
#endif

}