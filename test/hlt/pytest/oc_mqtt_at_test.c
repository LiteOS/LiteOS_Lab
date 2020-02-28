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
 *  2019-11-19 15:43  zhangqianfu  The first version
 *
 */
////////////////MAKE SOME COMMAND FOR THE SHELL      ///////////////////////////

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <shell.h>
#include <osal.h>
#include <oc_mqtt_at.h>
#include <oc_mqtt_al.h>

static int shell_connect(int argc, const char *argv[])
{
    int ret;

    int  mode;
    char *ip;
    char *id;
    char *pwd;

    if(argc != 2)
    {
        printf("parameter err:connect mode(bs/bserr/hub/huberr)");
        return -1;
    }
    else if(0 == strcmp(argv[1],"bs"))
    {
        mode = 1;
        ip = "119.3.251.30";
        id = "mqtt_sdk03";
        pwd = "f62fcf47d62c4ed18913";
    }
    else if(0 == strcmp(argv[1],"bserr"))
    {
        mode = 1;
        ip = "119.3.251.30";
        id = "mqtt_sdk11";
        pwd = "f62fcf47d62c4ed18913";

    }
    else if(0 == strcmp(argv[1],"hub"))
    {
        mode = 0;
        ip = "49.4.93.24";
        id = "mqtt_sdk01";
        pwd = "c18f10422c93548e6fef";

    }
    else if(0 == strcmp(argv[1],"huberr"))
    {
        mode = 0;
        ip = "49.4.93.24";
        id = "mqtt_sdk01";
        pwd = "c18f10422c93548e6fe1";

    }
    else
    {
        printf("parameter err:connect mode(bs/bserr/hub/huberr)");
        return -1;
    }

    ret = hwoc_mqtt_connect(mode,10,ip,"8883",id,pwd);

    printf("%s:connectcode:%d:%s\n\r",__FUNCTION__,ret,oc_mqtt_err(ret));

    return 0;
}

OSSHELL_EXPORT_CMD(shell_connect,"connect","connect (bs/bserr/hub/huberr)");

static int shell_disconnect(int argc, const char *argv[])
{
    int ret;

    ret = hwoc_mqtt_disconnect();

    printf("%s:disconnectcode:%d:%s\n\r",__FUNCTION__,ret,oc_mqtt_err(ret));

    return 0;
}

OSSHELL_EXPORT_CMD(shell_disconnect,"disconnect","disconnect");

static int shell_version(int argc, const char *argv[])
{
    char * ret;

    ret = hwoc_mqtt_version();

    printf("%s:version:%s\n\r",__FUNCTION__,ret);

    return 0;
}

OSSHELL_EXPORT_CMD(shell_version,"version","version");

static int shell_send(int argc, const char *argv[])
{
    int  ret;

    if(argc != 3)
    {
        return 0;
    }

    ret = hwoc_mqtt_send(atoi(argv[1]),(uint8_t*)argv[2],strlen(argv[2]));

    printf("%s:send:%d:%s\n\r",__FUNCTION__,ret,oc_mqtt_err(ret));

    return 0;
}

OSSHELL_EXPORT_CMD(shell_send,"send","send qos data");

static int shell_publish(int argc, const char *argv[])
{
    int  ret;

    if(argc != 4)
    {
        return 0;
    }

    ret = hwoc_mqtt_publish(atoi(argv[1]),(char *)argv[2],(uint8_t *)argv[3],strlen(argv[3]));

    printf("%s:send:%d:%s\n\r",__FUNCTION__,ret,oc_mqtt_err(ret));

    return 0;
}

OSSHELL_EXPORT_CMD(shell_publish,"publish","publish qos topic data");


#define CN_LOOP_TIMES  1000

#define CN_BS_ID       "mqtt_sdk03"
#define CN_BS_PWD      "f62fcf47d62c4ed18913"
#define CN_BS_SERVER   "119.3.251.30"
#define CN_BS_PORT     "8883"

#define CN_HUB_ID       "mqtt_sdk01"
#define CN_HUB_PWD      "c18f10422c93548e6fef"
#define CN_HUB_SERVER   "49.4.93.24"
#define CN_HUB_PORT     "8883"

#define CN_PUB_DATA     "{\"msgType\":\"deviceReq\",\"hasMore\":0,\"data\":[{\"serviceId\":\"Battery\",\"serviceData\":{\"batteryLevel\":68}}]}"

static int demo_bsconnect()
{
    int ret =0;
    int loop = 0;

    hwoc_mqtt_disconnect();


    for(loop = 0; loop<CN_LOOP_TIMES;loop++ )
    {
        printf("***************%s:LOOP:%d************************\n\r",__FUNCTION__,loop);

        if(0 != hwoc_mqtt_connect(1,10,CN_BS_SERVER,CN_BS_PORT,CN_BS_ID,CN_BS_PWD))
        {
            break;
        }
        hwoc_mqtt_disconnect();
        osal_task_sleep(100);

    }
    if(loop == CN_LOOP_TIMES)
    {
        printf("OC MQTT BS CONNECT TEST SUCCESS------------------\n\r");
        ret = 0;
    }
    else
    {
        printf("OC MQTT BS CONNECT TEST FAILED ++++++++++++++++++++\n\r");
        ret =-1;
    }
    return ret;
}

static int demo_hubconnect()
{
    int ret =0;
    int loop = 0;

    hwoc_mqtt_disconnect();


    for(loop = 0; loop<CN_LOOP_TIMES;loop++ )
    {
        printf("***************%s:LOOP:%d************************\n\r",__FUNCTION__,loop);

        if(0 != hwoc_mqtt_connect(0,10,CN_HUB_SERVER,CN_HUB_PORT,CN_HUB_ID,CN_HUB_PWD))
        {
            break;
        }
        hwoc_mqtt_disconnect();
        osal_task_sleep(100);

    }
    if(loop == CN_LOOP_TIMES)
    {
        printf("OC MQTT HUB CONNECT TEST SUCCESS------------------\n\r");
        ret = 0;
    }
    else
    {
        printf("OC MQTT HUB CONNECT TEST FAILED ++++++++++++++++++++\n\r");
        ret =-1;
    }
    return ret;

}


static int demo_bshubconnect()
{
    int ret =0;
    int loop = 0;


    hwoc_mqtt_disconnect();

    for(loop = 0; loop<CN_LOOP_TIMES;loop++ )
    {
        printf("***************%s:LOOP:%d************************\n\r",__FUNCTION__,loop);

        if(0 != hwoc_mqtt_connect(1,10,CN_BS_SERVER,CN_BS_PORT,CN_BS_ID,CN_BS_PWD))
        {
            break;
        }
        hwoc_mqtt_disconnect();

        if(0 != hwoc_mqtt_connect(0,10,CN_HUB_SERVER,CN_HUB_PORT,CN_HUB_ID,CN_HUB_PWD))
        {
            break;
        }
        hwoc_mqtt_disconnect();
        osal_task_sleep(100);

    }


    if(loop == CN_LOOP_TIMES)
    {
        printf("OC MQTT BS HUB CONNECT TEST SUCCESS------------------\n\r");
        ret = 0;
    }
    else
    {
        printf("OC MQTT BS HUB CONNECT TEST FAILED ++++++++++++++++++++\n\r");
        ret =-1;
    }
    return ret;
}


static int demo_bspublish()
{

    int ret = -1;
    int loop = 0;


    hwoc_mqtt_disconnect();
    if(0 != hwoc_mqtt_connect(1,10,CN_BS_SERVER,CN_BS_PORT,CN_BS_ID,CN_BS_PWD))
    {
        return ret;
    }

    for(loop = 0; loop<CN_LOOP_TIMES;loop++ )
    {
        printf("***************%s:LOOP:%d************************\n\r",__FUNCTION__,loop);

        if(0 != hwoc_mqtt_send(loop%3,(uint8_t *)CN_PUB_DATA,strlen(CN_PUB_DATA)))
        {
            break;
        }
        osal_task_sleep(100);

    }
    hwoc_mqtt_disconnect();

    if(loop == CN_LOOP_TIMES)
    {
        printf("OC MQTT BS PUB  TEST SUCCESS------------------\n\r");
        ret = 0;
    }
    else
    {
        printf("OC MQTT BS PUB  TEST FAILED ++++++++++++++++++++\n\r");
        ret =-1;
    }
    return ret;

}

static int demo_hubpublish()
{
    int ret = -1;
    int loop = 0;


    hwoc_mqtt_disconnect();
    if(0 != hwoc_mqtt_connect(0,10,CN_HUB_SERVER,CN_HUB_PORT,CN_HUB_ID,CN_HUB_PWD))
    {
        return ret;
    }

    for(loop = 0; loop<CN_LOOP_TIMES;loop++ )
    {
        printf("***************%s:LOOP:%d************************\n\r",__FUNCTION__,loop);

        if(0 != hwoc_mqtt_send(loop%3,(uint8_t *)CN_PUB_DATA,strlen(CN_PUB_DATA)))
        {
            break;
        }
        osal_task_sleep(100);
    }
    hwoc_mqtt_disconnect();

    if(loop == CN_LOOP_TIMES)
    {
        printf("OC MQTT HUB PUBLISH  TEST SUCCESS------------------\n\r");
        ret = 0;
    }
    else
    {
        printf("OC MQTT HUB PUBLISH  TEST FAILED ++++++++++++++++++++\n\r");
        ret =-1;
    }
    return ret;
}


typedef  int (*fn_demo_fun)(void);

static fn_demo_fun s_demo_tab[]=
{
    demo_bspublish,
    demo_hubpublish,
    demo_bsconnect,
    demo_hubconnect,
    demo_bshubconnect,

    NULL,
};



static int entry_ocmqttloop(void *arg)
{
    int ret = 0;
    int i = 0;
    fn_demo_fun demo_fun;

    printf("LOOP TEST START........\n\r");
    for(i =0;;i++)
    {
        demo_fun = s_demo_tab[i];
        if(demo_fun != NULL)
        {
            ret = demo_fun();

            if(0 != ret)
            {
                printf("demo:%d failed, and test exit\n\r ",i);
            }
        }
        else
        {
            break;
        }
    }


    printf("LOOP TEST END..........\n\r");

    return 0;
}

 int shell_ocmqttloop(int argc, const char *argv[])
{
    osal_task_create("ocmqtttest",entry_ocmqttloop,NULL,0x1000,NULL,10);

    return 0;
}


OSSHELL_EXPORT_CMD(shell_ocmqttloop,"ocmqttloop","ocmqttloop test");






