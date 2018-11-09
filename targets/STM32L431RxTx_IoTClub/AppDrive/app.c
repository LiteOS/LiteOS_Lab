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

//show the led welcome information
extern void led_display(void); 

//bc95 interface
extern bool_t bc95_init(const char *server);
extern bool_t bc95_send(u8_t *buf,s32_t len, u32_t timeout);
extern void bc95_regester_receivehandle(void *handle);
extern bool_t bc95_csq(u32_t *value);

//led control
extern void led_on(void);
extern void led_off(void);

//bh1750 read
extern float Convert_BH1750(void);
extern void Init_BH1750(void);


#define cn_app_report_timeout  (2*1000)


//enum all the message type
typedef enum
{
    en_msgid_light_intensity = 0,
    en_msgid_light_state,
    en_msgid_light_ctrl,
    en_msgid_net_csq,
}en_app_msgid;

struct app_report
{
    u32_t timeout;
    bool_t report;  //if false then pause report
};
static struct app_report  s_app_report;


static bool_t s_led_state = false;

//this function used to deal all the received message here
//message format:msgid+content
static void  deal_rcvmsg(u8_t *data, s32_t len)
{
    u8_t msgid;
    char *str;
    printf("msg:len:%d msgid:%d data:%s \r\n",len,*data,(char *)(data+1));
    msgid = *data;
    switch (msgid)
    {
        case en_msgid_light_ctrl:
            str = (char *)data+1;
            if(0 == strcmp(str,"ON"))
            {
                led_on();
                s_led_state = true;
            }
            else if(0 == strcmp(str,"OFF"))
            {
                led_off();
                s_led_state = false;
            }
            else
            {
                printf("LED CMD ERROR\r\n");
            }
            break;
        default:
            printf("UNKOWN MSGID\r\n");
            break;   
    }
    return;
}

//do the light intensity report
static void report_light_intensity()
{
    u8_t   sendbuf[32];
    u32_t  value;

    //sample light intensity and report
    value = Convert_BH1750();
    printf("%s:sample light:%d\r\n",__FUNCTION__,value);

    memset(sendbuf,' ',32);
    sendbuf[0] = en_msgid_light_intensity;
    sprintf((char *)&sendbuf[1],"%5d",(s32_t)value);

    if(bc95_send(sendbuf,6,cn_app_report_timeout))
    {
        printf("sendmsg OK \n\r");
    }
    else
    {
        printf("sendmsg ERR \n\r");
    }
}
//do the light state report
static void report_light_state()
{
    u8_t   sendbuf[32];
    //report the light state
    memset(sendbuf,' ',32);
    sendbuf[0] = en_msgid_light_state;
    if(s_led_state)
    {
        sprintf((char *)&sendbuf[1],"%s","ON ");
    }
    else
    {
        sprintf((char *)&sendbuf[1],"%s","OFF");
    }
    if(bc95_send(sendbuf,4,2*1000))
    {
        printf("sendmsg OK:%s\n\r",(char *)sendbuf);
    }
    else
    {
        printf("sendmsg ERR:%s\n\r",(char *)sendbuf);
    }

}

//do the net csq report
static void report_net_csq()
{
    u8_t   sendbuf[32];
    u32_t  value;
    //sample the net and report
    if(bc95_csq(&value))
    {
        printf("%s:sample csq:%d\r\n",__FUNCTION__,value);            
    }
    else
    {
        printf("%s:sample csq failed\r\n",__FUNCTION__);
    }
    
    memset(sendbuf,0,32);
    sendbuf[0] = en_msgid_net_csq;
    sprintf((char *)&sendbuf[1],"%3d",(s32_t)value);
    if(bc95_send(sendbuf,4,cn_app_report_timeout))
    {
        printf("sendmsg OK\r\n");
    }
    else
    {
        printf("sendmsg ERR\r\n");
    }
}

//this is app main function:loop
u32_t app_main(void *args)
{   
    s_app_report.timeout = 5*1000;
 
    Init_BH1750();
    led_display();
    bc95_init("139.159.140.34");
    bc95_regester_receivehandle(deal_rcvmsg);
    while(1)
    {        
        if(s_app_report.report)
        {
            report_light_intensity();
            
            report_light_state();

            report_net_csq();
        }

        task_sleepms(s_app_report.timeout);   
    }
}

//export a shell command to send data
#include <shell.h>
static s32_t shell_appsend(s32_t argc, const char *argv[])
{
    u8_t sendbuf[64];
    u8_t msgid;
    
    if(argc != 3)
    {
        printf("paras error\n\r");
        return -1;
    }
    if((argv[1][0] >'9')||(argv[1][0] <'0'))
    {
        printf("msgid error\n\r");
        return -1;
    }
    msgid = argv[1][0]-'0';
    memset(sendbuf,0,64);
    sendbuf[0] = msgid;
    strncpy((char *)&(sendbuf[1]),argv[2],62);
    
    if(bc95_send(sendbuf,1+strlen(argv[2]),2*1000))
    {
        printf("%s: send success\n\r",__FUNCTION__);
    }
    else
    {
        printf("%s: send failed\n\r",__FUNCTION__);
    }
    
    return 0;

}
OSSHELL_EXPORT_CMD(shell_appsend,"appsend","appsend msgid data");

//create a command to control the report 
static s32_t shell_appreport(s32_t argc,const char *argv[])
{
    if(argc <2)
    {
        return -1;
    }
    
    if((0 == strcmp(argv[1],"timeout"))&&(argc == 3))
    {
        s_app_report.timeout = strtoul(argv[2],NULL,0);
    }
    else if(0 == strcmp(argv[1],"stop"))
    {
        s_app_report.report = false;
    }
    else if(0 == strcmp(argv[1],"start"))
    {
        s_app_report.report = true;
    }
    else
    {
    
    }
    return 0;

}
OSSHELL_EXPORT_CMD(shell_appreport,"appreport","appreport stop/start/timeout [timeout]");



