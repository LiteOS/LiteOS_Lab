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
 *  2019-12-11 19:52  zhangqianfu  The first version  
 *
 */


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
#include <at.h>
#include <oc_mqtt_al.h>
#include <osal.h>
#include <stdlib.h>


#define cn_ec20_cmd_timeout  (32*1000)
#define cn_urc_deact           "\r\n+QIURC:"

static void  *s_oc_handle = NULL;
static int resetpdp = 1;


static bool_t ec20_atcmd(const char *cmd,const char *index,int timeout)
{
    int ret = 0;
    ret = at_command((unsigned char *)cmd,strlen(cmd),index,NULL,0,timeout);
    if(ret >= 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//at command  with response
static bool_t ec2x_atcmd_response(const char *cmd,const char *index,char *buf, int len)
{
    int ret = 0;
    ret = at_command((unsigned char *)cmd,strlen(cmd),index,(char *)buf,len,cn_ec20_cmd_timeout);
    if(ret >= 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static void urc_deact(void *args,void *msg,size_t len)
{

//    Paint_DrawImage(gImage_OFF ,290, 130, 140, 140);
//    //Paint_ClearWindows(60,400,400,430,WHITE);
//    //Paint_ClearWindows(40,440,470,466,WHITE);
//    //Paint_ClearWindows(120,360,400,390,WHITE);
//    Paint_ClearWindows(40,360,470,466,WHITE);
//    Paint_DrawString_EN(60, 400, "Connecting OceanLink", &Font24, WHITE, BLACK);
//    reconn = 1;
//    resetpdp = 1;




}
//
//static void* ec20_oc_config(tag_oc_mqtt_config *param)
//{
//    int ret = 0;
//    char cmd[256];
//    int retry = 0;
//
//    if(reconn == 1 && NULL != param )
//    {
//        while (1)
//        {
//        if(ec20_atcmd("AT\r\n","OK",2000))
//        {
//
//            break;
//        }
//        }
//        osal_task_sleep(5*1000);
//
//        while (1)
//        {
//        memset(cmd,0,256);
//        snprintf(cmd,256,"AT+HWOCMQTTCONNECT=%d,%d,\"%s\",\"%s\",\"%s\",\"%s\"\r", param->boot_mode, param->lifetime, param->server, param->port, param->device_info.s_device.deviceid, param->device_info.s_device.devicepasswd);
//        printf("cmd:%s\r\n", cmd);
//        //ec20_atcmd("at+HWOCMQTTCONNECT=1,30,\"iot-bs.cn-north-4.myhuaweicloud.com\",\"8883\",\"sdk_0040\",\"f62fcf47d62c4ed18913\"","OK");
//
//        char resp[64];
//        memset(resp,0,64);
//        ret = ec2x_atcmd_response(cmd,"+CONNECTED",resp,64);
//        if(ret)
//        {
//            if (NULL != strstr(resp,"CONNECTED OK"))
//            {
//            s_oc_handle = (void *)0xffffffff;
//            reconn = 0;
//            break;
//            }
//            else
//                {
//                s_oc_handle = NULL;
//                continue;
//                }
//        }
//        }
//
//    }
//    return s_oc_handle;
//}

void ec20_oc_setpdp(void)
{
    if (resetpdp)
    {
    char cmd[256];
        int retry = 0;

    while (1)
            {
                int id = 1;
                memset(cmd,0,256);
                snprintf(cmd,256,"AT+QIACT=%d\r\n",id);
            if(ec20_atcmd(cmd,"OK",2000))
            {
                resetpdp = 0;
                break;
            }
            else
            {
                retry++;
            }
            if (retry >= 10)
            {
                if (id <= 16)
                {
                id++;
                retry = 0;
                }
                else break;
            }
            }
    }
}

static int ec20_oc_deconfig(void *handle)
{
    s_oc_handle = NULL;
    return 0;
}

static int ec20_oc_report(void *handle,char *msg,int len,en_mqtt_al_qos_t qos)
{
    int ret = -1;

    char cmd[256];

    memset(cmd,0,256);
    snprintf(cmd,256,"AT+HWOCMQTTSEND=%d,%d,\"%s\"\r", qos, len, msg);

    //ec20_atcmd("at+HWOCMQTTCONNECT=1,30,\"iot-bs.cn-north-4.myhuaweicloud.com\",\"8883\",\"sdk_0040\",\"f62fcf47d62c4ed18913\"","OK");
    //ec20_atcmd("AT+HWOCMQTTSEND=0,116,\"{\"msgType\":\"deviceReq\",\"hasMore\":0,\"data\":[{\"serviceId\":\"LED\",\"serviceData\":{\"LED1\":1,\"LED2\":1,\"LED3\":1,\"LED4\":1}}]}\"","OK");
    printf("cmd:%s\r\n", cmd);

    ret = ec20_atcmd(cmd,"SEND OK",5000);

    if(ret > 0)
    {
        ret = 0;
    }
    else
    {
        ret = -1;
    }

    return ret;
}



//+QSPN: "CHINA MOBILE","CMCC","CMCC",0,"46000"
int ec2x_get_operator(char *buf, int len)
{
    int ret = -1;
    char resp[128];
    char *str;
    char *str_s;
    char *str_e;

    memset(resp,0,128);
    if (ec2x_atcmd_response("AT+QSPN\r\n","+QSPN",resp,128))
    {
        str = strstr(resp,":");
        if(NULL != str)
        {
            str_s = str;
            str_s++;
            str_s++;
            str_s++;
            str_e = strstr(str,",");
            if(NULL != str_e)
            {
                str_e--;
                *str_e = '\0';
                strncpy(buf,str_s,len);
                ret = 0;
            }
        }
     }

    return ret;
}



//AT+QLTS=2
//+QLTS: "2019/12/12,10:49:42+32,0"
int ec2x_gettime(char *timebuffer)
{
    int ret = -1;
    char resp[128];
    char *str;
    int zone;

    char *str_s;
    char *str_e;
    char *str_z;


    memset(resp,0,128);
    if (ec2x_atcmd_response("AT+QLTS=2\r\n","+QLTS",resp,128))
    {
        str = strstr(resp,"\"");
        if(NULL == str)
        {
            goto EXIT_ERR;
        }
        str ++;
        str_s = str;


        str = strstr(str,"+");
        if(NULL == str)
        {
            goto EXIT_ERR;
        }
        str_e = str;

        str++;
        str_z = str;


        str = strstr(str,",");
        if(NULL == str)
        {
            goto EXIT_ERR;
        }

        *str = '\0';
        zone = atoi(str_z)/4;
        sprintf(str_e," GTM+%d",zone);

        strcpy(timebuffer,str_s);

        ret = 0;
    }
    return ret;

EXIT_ERR:
    return ret;

}

//+CSQ: 29,99
int ec2x_get_csq(int *csq)
{
    char resp[64];
    char *str_s;
    int ret = -1;


    memset(resp,0,64);
    if(ec2x_atcmd_response("AT+CSQ\r\n","+CSQ",resp,64))
    {
        str_s = strstr(resp,"+CSQ");
        if(NULL == str_s)
        {
            goto EXIT_ERR;
        }
        ret = sscanf(str_s,"+CSQ: %d,",csq);
        if(ret > 0)
        {
            ret = 0;
        }
        else
        {
            ret = -1;
        }
    }
    return ret;

 EXIT_ERR:
    return ret;
}



static char *find_nchar(char *str,char c,int n)
{
    char *ret = NULL;
    char *p = str;
    int count = 0;

    while(*p != '\0')
    {
        if(*p == c)
        {
            count ++;
            if(count == n)
            {
                ret = p;
                break;
            }
        }
        p++;
    }
    return ret;
}
//+QENG: "servingcell","NOCONN","LTE","TDD",460,00,2C5D704,367,38950,40,5,5,27B7,-78,-7,-50,22,49
int ec2x_get_rssi(int *rssi)
{
    int  ret = -1;
    char resp[128];
    char *str_s;
    char *str_e;

    memset(resp,0,128);

    if(ec2x_atcmd_response("AT+QENG=\"servingcell\"\r\n","+QENG:",resp,128))
    {
        str_s = find_nchar(resp,',',15);
        str_e = find_nchar(resp,',',16);
        if((NULL != str_s) && (NULL != str_e))
        {
            ret = sscanf(str_s,",%d,",rssi);
            if(ret > 0)
            {
                ret = 0;
            }
            else
            {
                ret = -1;
            }
        }
    }
    return ret;
}



