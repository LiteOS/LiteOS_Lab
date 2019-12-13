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


#define CN_EC2X_CMD_TIMEOUT  (32*1000)
#define cn_urc_deact         "\r\n+QIURC:"


typedef struct
{
    int          connfigured;
    osal_mutex_t mutex;
}ec2x_oc_cb_t;

static bool_t ec2x_atcmd(const char *cmd,const char *index)
{
    int ret = 0;
    ret = at_command((unsigned char *)cmd,strlen(cmd),index,NULL,0,CN_EC2X_CMD_TIMEOUT);
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
    ret = at_command((unsigned char *)cmd,strlen(cmd),index,(char *)buf,len,CN_EC2X_CMD_TIMEOUT);
    if(ret >= 0)
    {
        return true;
    }
    else
    {
        return false;
    }
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
int ec2x_get_time(char *timebuffer)
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
    if(ec2x_atcmd_response("AT+CSQ\r\n","+CSQ: ",resp,64))
    {
        str_s = strstr(resp,"+CSQ: ");
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



typedef struct
{
    //initialized by the applicaiton
    fn_oc_mqtt_msg_deal  msg_dealer;
    void                *msg_dealer_args;
    int                  bsmode;
    int                  configured:1;
    int                  connected:1;

    osal_mutex_t         lock;    ///< this is the at lock here
} ec2x_cb_t;

#define CN_EC2X_OC_MSGINDEX  "\r\n\r\n+HWOCMQTTRECEIVED="
static ec2x_cb_t   s_ec2x_cb;

static int ec2x_oc_msg_deal(void *args,void *msg,size_t len)
{
    char *data;

    data = msg;

    printf("recv:%s len:%d\r\n", data, len);

    return 0;
}

static int ec2x_oc_config(oc_mqtt_config_t *param)
{
    int ret = en_oc_mqtt_err_system;
    char cmd[256];

    if(osal_mutex_lock(s_ec2x_cb.lock))
    {
        if(s_ec2x_cb.configured)
        {
            ret = en_oc_mqtt_err_configured;
        }
        else
        {

            if(param->boot_mode == en_oc_mqtt_mode_bs_static_nodeid_hmacsha256_notimecheck_json)
            {
                s_ec2x_cb.bsmode = 1;
            }
            else
            {
                s_ec2x_cb.bsmode = 0;
            }
            s_ec2x_cb.msg_dealer = param->msg_deal;
            s_ec2x_cb.msg_dealer_args = param->msg_deal_arg;

            memset(cmd,0,256);
            snprintf(cmd,256,"at+HWOCMQTTCONNECT=%d,%d,\"%s\",\"%s\",\"%s\",\"%s\"\r",\
                    s_ec2x_cb.bsmode, param->lifetime, param->server_addr, param->server_port, param->id, param->pwd);
            if(ec2x_atcmd(cmd,"+CONNECTED OK"))
            {
                ret = en_oc_mqtt_err_ok;
                s_ec2x_cb.configured =1;
                s_ec2x_cb.connected =1;
            }
            else
            {
                ret = en_oc_mqtt_err_network;
            }
        }

        osal_mutex_unlock(s_ec2x_cb.lock);
    }

    return ret;
}


static int ec2x_oc_deconfig(void)
{
    int ret = en_oc_mqtt_err_system;
    char cmd[256];
    if(osal_mutex_lock(s_ec2x_cb.lock))
    {
        if(0 == s_ec2x_cb.configured)
        {
            ret = en_oc_mqtt_err_noconfigured;
        }
        else
        {
            memset(cmd,0,256);
            snprintf(cmd,256,"at+HWOCMQTTDISCONNECT\r");
            if(ec2x_atcmd(cmd,"+DISCONNECTED OK"))
            {
                ret = en_oc_mqtt_err_ok;
                s_ec2x_cb.configured = 0;
                s_ec2x_cb.connected = 0;
                s_ec2x_cb.msg_dealer = NULL;
                s_ec2x_cb.msg_dealer_args = NULL;
            }
            else
            {
                ret = en_oc_mqtt_err_network;
            }
        }
        osal_mutex_unlock(s_ec2x_cb.lock);
    }

    return ret;
}

static int ec2x_oc_publish(char *topic,uint8_t *msg,int len,int qos)
{
    int ret = en_oc_mqtt_err_system;
    char cmd[256];
    if(osal_mutex_lock(s_ec2x_cb.lock))
    {
        if(0 == s_ec2x_cb.connected)
        {
            ret = en_oc_mqtt_err_noconfigured;
        }
        else if(NULL != topic)
        {
            ret = en_oc_mqtt_err_system;
        }
        else if((NULL == msg) || (len = 0) ||(qos >= en_mqtt_al_qos_err))
        {
            ret = en_oc_mqtt_err_parafmt;
        }
        else
        {
            memset(cmd,0,256);
            snprintf(cmd,256,"AT+HWOCMQTTSEND=%d,%d,\"%s\"\r", qos, len,(char *) msg);
            if(ec2x_atcmd(cmd,"+DISCONNECTED OK"))
            {
                ret = en_oc_mqtt_err_ok;
            }
            else
            {
                ret = en_oc_mqtt_err_network;
            }
        }
        osal_mutex_unlock(s_ec2x_cb.lock);
    }

    return ret;
}

static const oc_mqtt_t s_ec2x_oc =
{
    .name = "ec2x_oc",
    .op = {
        .config = ec2x_oc_config,
        .deconfig = ec2x_oc_deconfig,
        .publish =  ec2x_oc_publish,
        .subscribe = NULL,
    },
};

int ec2x_oc_init(void)
{
    int ret = -1;
    osal_mutex_create(&s_ec2x_cb.lock);

    at_oobregister("ec2x_ocmsg",CN_EC2X_OC_MSGINDEX,strlen(CN_EC2X_OC_MSGINDEX),ec2x_oc_msg_deal,NULL);

    ret = oc_mqtt_register(&s_ec2x_oc);
    return ret;
}

