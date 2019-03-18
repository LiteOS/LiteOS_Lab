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
#include <string.h>
#include <stdlib.h>
#include <osport.h>
#include <shell.h>
#include <app_main.h>

#include <nb.h>

typedef struct
{
    const char         *appname;    //inderection the app name
    u8_t                direction;  //read or write
    s32_t               cycle;      //if read ,the cycle to read
    s32_t               deadtime;   //if read, how much time left to report
    app_msg_function    func;       //used to get a message or deal with the received message
}tagAppItem;

typedef struct
{
    tagNbConfig         config;
    u16_t               status;     //create or not, report or not
    u32_t               totalsnt;
    u32_t               totalrcv;
    u32_t               timeout;    //hou much time wait to report
    tagAppItem          apps[en_app_msgid_last];
}tagAppManager;

static tagAppManager  s_app_manager;

#define cn_app_task_status_create      (1<<0)   //which means the task created or not
#define cn_app_task_status_running     (1<<1)   //which means report or not
#define cn_app_report_timeout          (2*1000) //at most we could wait 2 seconds

//use this function to register a app funciton to report data or deal with the command
bool_t  app_register(const char *appname, en_app_msg_direction direction, en_app_msgid msgid,\
                     app_msg_function func, s32_t cycle)
{
    bool_t ret = false;
    if ((NULL != appname) && (msgid < en_app_msgid_last) && (NULL != func) &&\
        (!((0 == cycle) && (direction == en_app_direction_report))))  //when report,not permit cycle zero
    {
        s_app_manager.apps[msgid].appname = appname;
        s_app_manager.apps[msgid].func = func;
        s_app_manager.apps[msgid].direction = direction;
        s_app_manager.apps[msgid].deadtime = 0;
        s_app_manager.apps[msgid].cycle = cycle;

        ret = true;
    }
    return ret;
}

//this function used to deal all the received message here
//message format:msgid+content
static void  app_rcv_deal(u8_t *data, s32_t len)
{
    u8_t msgid;
    app_msg_function  func;
    printf("msg:len:%d msgid:%d data:%s \r\n", len, *data, (char *)(data+1));
    msgid = *data;
    s_app_manager.totalsnt += len;

    if ((msgid < en_app_msgid_last) && (s_app_manager.apps[msgid].direction == en_app_direction_command) &&\
        (NULL != s_app_manager.apps[msgid].func))
    {
        func = s_app_manager.apps[msgid].func;
        func(data+1, len-1);
    }
    return;
}

//here we supply a method for the app send raw data
bool_t app_send_raw(u8_t *msg, s32_t len)
{
    bool_t ret = false;

    if ((NULL != msg) && (len > 0))
    {
        ret = nb_send(msg, len, cn_app_report_timeout);
    }

    return ret;
}


//this is app main function:loop
#define cn_report_buf_len    256   //may be bigger enough
static  u8_t  s_report_buf[cn_report_buf_len];


#define LIGHT_SERSOR_EN   1   //need to insert the bh1750 mini board
#define CSQ_SENSOR_EN     1   //on the main board :NB MODULE support
#define LED_SWITCH_EN     1   //on the main board :D1
#define DHT11_SENSOR_EN   0   //need to insert the dht11 mini board
#define SMOKE_SENSOR_EN   0   //need to insert the smoke mini board
#define BEEP_SWICTH_EN    0   //need to insert the smoke mini board



static void __app_main_taskentry(void *args)
{
    en_app_msgid   appmsgid;
    tagAppItem    *appitem;
    s32_t          msglen;

#if LIGHT_SERSOR_EN
    extern bool_t app_intensity_report();
    app_intensity_report();  //used to report the light sensor,if not need, just comment it
#endif

#if  LED_SWITCH_EN
    extern bool_t app_led_switch();
    app_led_switch();          //used to control the led, if not need, just comment it
#endif

#if CSQ_SENSOR_EN
    extern bool_t app_csq_report();
    app_csq_report();         //used to report the net csq, if not need, just comment it
#endif

#if DHT11_SENSOR_EN
    extern bool_t app_dht11_report();
    app_dht11_report();
#endif

#if SMOKE_SENSOR_EN
    extern bool_t app_smoke_report();
    app_smoke_report();
#endif

    nb_init(&s_app_manager.config);
    nb_regester_receivehandle(app_rcv_deal);

    while (1)
    {
        if (s_app_manager.status & cn_app_task_status_running)
        {
            for (appmsgid = en_app_msgid_start;appmsgid< en_app_msgid_last;appmsgid++ )
            {
                appitem = &s_app_manager.apps[appmsgid];
                if ((NULL != appitem->func) && (en_app_direction_report == appitem->direction))
                {
                   appitem->deadtime --;
                   if (appitem->deadtime <= 0)
                   {
                       msglen = appitem->func(s_report_buf + 1, cn_report_buf_len);
                       if (msglen > 0)
                       {
                           s_report_buf[0] = appmsgid;
                           appitem->deadtime = appitem->cycle;
                           if (nb_send(s_report_buf, msglen + 1, cn_app_report_timeout))
                           {
                                printf("sendmsg OK \n\r");
                                s_app_manager.totalsnt += msglen;
                           }
                           else
                           {
                                printf("sendmsg ERR \n\r");  //you should reset the module,maybe.
                           }
                       }

                   }
                }
            }
        }
        task_sleepms(s_app_manager.timeout);
    }
}

bool_t app_main_init(tagNbConfig *config)
{
    bool_t ret = false;

    if (0 == (s_app_manager.status & cn_app_task_status_create))
    {
        s_app_manager.config = *config;
        s_app_manager.status |= cn_app_task_status_create;
        s_app_manager.status |= cn_app_task_status_running;  //make it running
        s_app_manager.timeout = cn_app_report_cyle;

        task_create("appmain", (fnTaskEntry)__app_main_taskentry, 0x800, NULL, NULL, 12);

        ret = true;
    }

    return ret;
}

//use this function to show the app status
static void __app_main_status(void)
{
    u32_t appnum = 0;
    printf("create:%s  running:%s\n\r",\
            s_app_manager.status&cn_app_task_status_create?"YES":"NO ",\
            s_app_manager.status&cn_app_task_status_running?"YES":"NO ");
    if (s_app_manager.status&cn_app_task_status_create)
    {
        printf("SERVER:%s   BAND:%s\n\r",\
                s_app_manager.config.server,s_app_manager.config.bands);

        printf("CYCLE:%d (ms) SNT:%d (bytes) RCV:%d (bytes)\n\r",\
                s_app_manager.timeout, s_app_manager.totalsnt, s_app_manager.totalrcv);
        //show the registerd apps

        printf("%8s %-16s %-9s %-8s %8s %8s\n\r",\
        "msgid", "appname", "direction", "funcaddr", "looptime", "lifetime");
        en_app_msgid   appmsgid;
        tagAppItem    *appitem;
        for (appmsgid = en_app_msgid_start;appmsgid< en_app_msgid_last;appmsgid++ )
        {
            appitem = &s_app_manager.apps[appmsgid];
            if (NULL != appitem->func)
            {
                printf("%08d %-16s %-9s %-8x %-8d %-8d\n\r",\
                        appmsgid, appitem->appname,\
                        appitem->direction == en_app_direction_report?"report":"command",\
                       (u32_t)appitem->func, (u32_t)appitem->cycle, (u32_t)appitem->deadtime);
                appnum++;
            }
        }
        printf("appmain:appnum:%d\n\r", appnum);
    }
}


//create a command to control the report
static s32_t shell_appreport(s32_t argc, const char *argv[])
{
    if (argc <2)
    {
        __app_main_status();
        return 0;
    }

    if ((0 == strcmp(argv[1], "timeout"))&&(argc == 3))
    {
        s_app_manager.timeout = strtoul(argv[2], NULL, 0);
    }
    else if (0 == strcmp(argv[1], "stop"))
    {
        s_app_manager.status &= (~cn_app_task_status_running);
    }
    else if (0 == strcmp(argv[1], "start"))
    {
         s_app_manager.status |= cn_app_task_status_running;
    }
    else  //show the app status
    {
        __app_main_status();
    }
    return 0;

}
OSSHELL_EXPORT_CMD(shell_appreport, "appreport", "appreport [stop/start/timeout] [timeout]");

