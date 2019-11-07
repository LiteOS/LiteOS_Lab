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
 *  2019-11-06 18:09  zhangqianfu  The first version
 *
 */


/* brief : the oceanconnect platform only support the ca_crt up tills now*/
/** the address product_id device_id password crt is only for the test  */

#define DEFAULT_LIFETIME            10
#define DEFAULT_SERVER_IPV4         "49.4.93.24"     ///<  server ip address
#define DEFAULT_SERVER_PORT         "8883"           ///<  server mqtt service port
#define CN_MQTT_EP_NOTEID           "mqtt_sdk01"
#define CN_MQTT_EP_PASSWD           "c18f10422c93548e6fef"

#define CN_MQTT_EP_MSG              "{HELLO NEW WORLD}"

#include <osal.h>

#include <at_interface_mqtt.h>

int standard_app_demo_main()
{
    void *handle;

    handle = hwoc_mqtt_connect(0,DEFAULT_LIFETIME,DEFAULT_SERVER_IPV4,DEFAULT_SERVER_PORT,CN_MQTT_EP_NOTEID,CN_MQTT_EP_PASSWD);

    if(NULL == handle)
    {
        printf("connect failed\n\r");

        return -1;
    }

    while(1)
    {
        hwoc_mqtt_send(0,CN_MQTT_EP_MSG,strlen(CN_MQTT_EP_MSG));
        osal_task_sleep(10000);
    }

    hwoc_mqtt_disconnect();

    return 0;

}
