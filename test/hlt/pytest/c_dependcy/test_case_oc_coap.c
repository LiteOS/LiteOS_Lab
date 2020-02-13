/*----------------------------------------------------------------------------
 * Copyright (c) <2019>, <Huawei Technologies Co., Ltd>
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
 *  2019-12-10          majun       The first version  
 *
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <osal.h>
#include <oc_coap_al.h>
#include <link_endian.h>

#include <stdio.h>
#include <stdlib.h>
#include <queue.h>
#include <atiny_coap.h>
#include "test_case.h"


#define cn_app_connectivity    0
#define cn_app_lightstats      1
#define cn_app_light           2
#define cn_app_ledcmd          3
#define cn_app_cmdreply        4

typedef struct
{
    int8_t msgid;
    int16_t intensity;
}app_light_intensity_t;

typedef struct
{
    int8_t msgid;
    uint16_t mid;
    char led[3];
}app_led_cmd_t;

typedef struct
{
    int8_t msgid;
    uint16_t mid;
    int8_t errorcode;
    char curstats[3];
}app_cmdreply_t;

static char led_current_state[3];

typedef struct oc_coap_paras_s{
    en_oc_boot_strap_mode_t  boot_mode;
    char *address;                ///< server address,maybe domain name
    char *port;                   ///< server port
    char *ep_id;                  ///< endpoint identifier, which could be recognized by the server
    char *psk;
    int  psk_len;
    char *cb_name;
}oc_coap_paras;
static oc_coap_paras g_coap_param = {0};

//if your command is very fast,please use a queue here--TODO
#define cn_app_rcv_buf_len 128
static int8_t          s_rcv_buffer[cn_app_rcv_buf_len];
static int             s_rcv_datalen;
static osal_semp_t     s_rcv_sync;
static void           *s_coap_handle = NULL;
static queue_t        *s_queue_rcvmsg = NULL;   		 ///< this is used to cached the message //used
static void           *task_handle = NULL; 			     //used


/*-------------------OC COAP----------------------------*/
static int ts_oc_coap_init(char *message, int len);
static int ts_oc_coap_register(char *message, int len);
static int ts_oc_coap_config(char *message, int len);
static int ts_oc_coap_report(char *message, int len);
static int ts_oc_coap_getvalue(char *message, int len);
static int ts_oc_coap_deconfig(char *message, int len);
static int ts_oc_coap_deinit(char *message, int len);

test_entry ts_oc_coap_entry_flist[]= {
    ts_oc_coap_init,
    ts_oc_coap_register,
    ts_oc_coap_config,
    ts_oc_coap_report,
    ts_oc_coap_getvalue,
    ts_oc_coap_deconfig,
    ts_oc_coap_deinit,
};

int ts_sort_oc_coap_al(int entry_id, char *message, int len)
{
    if(entry_id >= TEST_OC_COAP_DEINIT)
    {
        printf("invalid entry_id(%d)\n",entry_id);
        return TS_FAILED;
    }
    return (ts_oc_coap_entry_flist[entry_id])(message, len);
}
/*---------use this function to push all the message to the buffer--------------------*/
static int app_msg_deal(void *msg, int len)
{
    int ret = -1;

    if(len <= cn_app_rcv_buf_len)
    {
        memcpy(s_rcv_buffer,msg,len);
        s_rcv_datalen = len;
        printf("receive msgid = %d\n", s_rcv_buffer[0]);
        printf("receive on = %c\n", s_rcv_buffer[3]);
        printf("receive on1 = %c\n", s_rcv_buffer[4]);
        printf("receive on2 = %c\n", s_rcv_buffer[5]);
        osal_semp_post(s_rcv_sync);
        ret = 0;
    }
    return ret;
}

static int app_cmd_task_entry()
{
    int ret = -1;
    app_led_cmd_t *led_cmd;
    app_cmdreply_t replymsg;
    int8_t msgid;

    while(1)
    {
        if(osal_semp_pend(s_rcv_sync,cn_osal_timeout_forever))
        {
            msgid = s_rcv_buffer[0];
            switch (msgid)
            {
                case cn_app_ledcmd:
                    led_cmd = (app_led_cmd_t *)s_rcv_buffer;
                    printf("LEDCMD:msgid:%d mid:%d msg:%s \n\r",led_cmd->msgid,ntohs(led_cmd->mid),led_cmd->led);
                    //add command action--TODO
                    //if (led_cmd->led[0] == 'O' && led_cmd->led[1] == 'N')
                    if (s_rcv_buffer[3] == 'O' && s_rcv_buffer[4] == 'N')
                    {
                        //if you need response message,do it here--TODO
                        replymsg.msgid = cn_app_cmdreply;
                        replymsg.mid = led_cmd->mid;
                        printf("reply mid is %d. \n\r",ntohs(replymsg.mid));
                        replymsg.errorcode = 0;
                        replymsg.curstats[0] = 'O';
                        replymsg.curstats[1] = 'N';
                        replymsg.curstats[2] = ' ';
                        led_current_state[0] = 'O';
                        led_current_state[1] = 'N';
                        led_current_state[2] = NULL;
                        oc_coap_report(s_coap_handle,(char *)&replymsg,sizeof(replymsg));    ///< report cmd reply message
                    }
                    //else if (led_cmd->led[0] == 'O' && led_cmd->led[1] == 'F' && led_cmd->led[2] == 'F')
                    else if (s_rcv_buffer[3] == 'O' && s_rcv_buffer[4] == 'F' && s_rcv_buffer[5] == 'F')
                    {

                        //if you need response message,do it here--TODO
                        replymsg.msgid = cn_app_cmdreply;
                        replymsg.mid = led_cmd->mid;
                        printf("reply mid is %d. \n\r",ntohs(replymsg.mid));
                        replymsg.errorcode = 0;
                        replymsg.curstats[0] = 'O';
                        replymsg.curstats[1] = 'F';
                        replymsg.curstats[2] = 'F';
                        led_current_state[0] = 'O';
                        led_current_state[1] = 'F';
                        led_current_state[2] = 'F';
                        oc_coap_report(s_coap_handle,(char *)&replymsg,sizeof(replymsg));    ///< report cmd reply message
                    }
                    else
                    {

                    }
                    break;
                default:
                    break;
            }
        }
    }

    return ret;
}

static int pp_oc_report()
{
    int retCode = 0;
    int lux = 0;	
    app_light_intensity_t  light;	

    printf("Calling app_report_task_entry(), start to report data.\n");
    lux++;
    lux= lux%10000;

    light.msgid = cn_app_light;
    light.intensity = htons(lux);
    retCode = oc_coap_report(s_coap_handle,(char *)&light,sizeof(light)); ///< report the light message

    return retCode;
}

static int pp_oc_config_test(oc_config_param_t *param)
{
    printf("this is coap at_oc_config\n");
    return 0;
}

static int pp_oc_deconfig_test(void *handle)
{
    printf("this is coap at_oc_deconfig\n");
    return 0;
}

static int pp_oc_report_test(void *handle,char *msg,int len)
{
    printf("this is coap at_oc_report_test\n");
    return 0;
}

static oc_coap_opt_t s_oc_coap_ops_test = \
{
        .config = pp_oc_config_test,
        .deconfig = pp_oc_deconfig_test,
        .report = pp_oc_report_test,
};

/*-----------------ts_oc_coap_init------------------------------------------------------*/
int ts_oc_coap_init(char *message, int len)
{
    s_queue_rcvmsg = queue_create("queue_rcvmsg",2,1);
    task_handle = osal_task_create("app_command",app_cmd_task_entry,NULL,0x1000,NULL,3);
    return oc_coap_init();
}

/*-----------------ts_oc_coap_register--------------------------------------------------*/
extern int oc_coap_unregister(const char *name);

static int ts_oc_coap_register(char *message, int len)
{
    int ret = 0;
    int retCode = 0;
    retCode = oc_coap_register("test_oc_coap_register", &s_oc_coap_ops_test);
    if (retCode == 0)
    {
        retCode = oc_coap_unregister("test_oc_coap_register");
        if (retCode == -1)
        {
            printf("coap unregister stubcode failed, exit from %s\n", __FUNCTION__);
            return retCode;
        }
    }
    else
    {
        return retCode;
    }

    ret = oc_coap_install_agent();
    printf("oc_coap_register  retCode: %d. \n", ret);
    osal_task_sleep(500);
    printf("exit from %s\n", __FUNCTION__);

    return ret;
}

/*-------------------ts_oc_coap_config--------------------------------------------------*/
static int ts_oc_coap_config(char *message, int len)
{
    int retcode = -1;
    oc_config_param_t      oc_param;
    memset(&oc_param,0,sizeof(oc_param));
    char *pchTmp, *pchStrTmpIn;
    oc_coap_paras  *pparas =&g_coap_param;// use to store usr data

    // message format: test_id|fname|bootstrap_mode|ip|port|endpoint|passwd|cb_name
    printf("[%s]g_tempbuf is %s\n", __FUNCTION__, message);
    pchTmp      = message;
    pchStrTmpIn = NULL;
    


    pchTmp = strtok_r(pchTmp, "|", &pchStrTmpIn); // test_id
    printf("[%s]id is %d\n",__FUNCTION__,atoi(pchTmp));
   

    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    printf("[%s]call name is %s\n",__FUNCTION__,pchTmp); // fname



    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn); // bootstrap_mode
    pparas->boot_mode = atoi(pchTmp);
    printf("[%s]boot_mode is %d\n",__FUNCTION__,pparas->boot_mode);
    


    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn); // ip address
    if(pparas->address != NULL) 
    {
        osal_free(pparas->address);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->address = NULL;
    }
    else
    {
        pparas->address = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->address, pchTmp, strlen(pchTmp));
        pparas->address[strlen(pchTmp)] = '\0';
        printf("[%s]oc server ipv4 address is %s\n",__FUNCTION__,pparas->address);
    }



    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn); // port
    if(pparas->port != NULL) 
    {
        osal_free(pparas->port);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->port = NULL;
    }
    else
    {
        pparas->port = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->port, pchTmp, strlen(pchTmp));
        pparas->port[strlen(pchTmp)] = '\0';
        printf("[%s]oc server port is %s\n",__FUNCTION__,pparas->port);
    }        




    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn); // user id
    if(pparas->ep_id != NULL) 
    {
        osal_free(pparas->ep_id);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->ep_id = NULL;
    }
    else
    {
        pparas->ep_id = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->ep_id, pchTmp, strlen(pchTmp));
        pparas->ep_id[strlen(pchTmp)] = '\0';
        printf("[%s]user id is %s\n", __FUNCTION__, pparas->ep_id);
    }




    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn); // psk
    if(pparas->psk != NULL) 
    {
        osal_free(pparas->psk);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->psk = NULL;
    }
    else
    {
        int len = strlen(pchTmp);
        printf("Password is %s\n",  pchTmp);
        pparas->psk_len = len/2;
        pparas->psk = osal_malloc(pparas->psk_len);
    	///0:48 9:57  A:65 F:70  a:97 f:102
        for(int i = 0; i < len; i++){
        	if(47 < pchTmp[i] && pchTmp[i] < 58)
        	{
        		pchTmp[i] = (pchTmp[i] - 48) * (i % 2 == 0 ? 16 : 1);
        	}else if(64 < pchTmp[i] && pchTmp[i] < 71)
        	{
        		pchTmp[i] = (pchTmp[i] - 55) * (i % 2 == 0 ? 16 : 1);
        	}else if(96 < pchTmp[i] && pchTmp[i] < 103)
        	{
        		pchTmp[i] = (pchTmp[i] - 87) * (i % 2 == 0 ? 16 : 1);
        	}

        	if(i % 2 == 1)
        	{
                pparas->psk[i/2] = pchTmp[i] + pchTmp[i - 1];
        	}
        }
    }
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn); // cb_name
	if(pparas->cb_name != NULL) 
    {
        osal_free(pparas->cb_name);
    }
	pparas->cb_name = osal_malloc(strlen(pchTmp)+1);
	memcpy(pparas->cb_name, pchTmp, strlen(pchTmp));
	pparas->cb_name[strlen(pchTmp)] = '\0';
    printf("[%s]cb_name is %s\n", __FUNCTION__, pparas->cb_name);

    oc_param.app_server.address = pparas->address;
    oc_param.app_server.port = pparas->port;
    oc_param.app_server.ep_id = pparas->ep_id;
    oc_param.app_server.psk = (char *)(pparas->psk);
    oc_param.app_server.psk_len = 6;//sizeof(pparas->psk);
    oc_param.app_server.psk_id = pparas->ep_id;
    oc_param.boot_mode = pparas->boot_mode;
    oc_param.rcv_func = (pparas->cb_name ? app_msg_deal : NULL);
    s_coap_handle = oc_coap_config(&oc_param);
    if(NULL != s_coap_handle)   //success ,so we could receive and send
    {
        retcode = 0;
    }
    return retcode;

}

/*---------------------ts_oc_coap_report------------------------------------------------*/
static int ts_oc_coap_report(char *message, int len)
{   
    int ret = 0;
    int retCode = 0;

    if(NULL != s_coap_handle)
    {
        printf("CONNECT:SUCCESS\n\r");
    }
    else
    {
        ret = -1;
        printf("CONNECT:FAILED\n\r");
        goto DEMO_EXIT;
    }

    retCode = pp_oc_report();
    ret -= (!(retCode == 0));
    if(0 == retCode)
    {
        printf("REPORT:SUCCESS\n\r");
    }
    else
    {
        printf("REPORT:FAILED\n\r");
        goto DEMO_EXIT;
    }


DEMO_EXIT:
    if(0 == ret)
    {
        printf("EXIT test_oc_lwm2m_report: PASS\n\r");
    }
    else
    {
        printf("EXIT test_oc_lwm2m_report: FAILED ************\n\r");
    }
    return ret;
}

/*---------------------ts_oc_coap_getvalue----------------------------------------------*/
static int ts_oc_coap_getvalue(char *message, int len)
{
    printf("[ts_oc_coap_getvalue] LED state is %s \n", led_current_state);
    memset(message, 0, 32);/*32 is more large than one int32*/
    snprintf(message,sizeof(led_current_state)+1,"%s",led_current_state);
    
    return TS_OK_HAS_DATA;
}

/*---------------------ts_oc_coap_deconfig----------------------------------------------*/
static int ts_oc_coap_deconfig(char *message, int len)
{
    int retCode = -1;
    oc_coap_paras *pparas = &g_coap_param;

    printf("start to call oc_coap_deconfig.  \n");
    retCode = oc_coap_deconfig(s_coap_handle);

    printf("exit from %s, retcode is %d\n", __FUNCTION__, retCode);
    if(pparas->address != NULL) 
    {
        osal_free(pparas->address);
        pparas->address = NULL;
    }
    if(pparas->port != NULL) 
    {
        osal_free(pparas->port);
        pparas->port = NULL;
    }
    if(pparas->ep_id != NULL) 
    {
        osal_free(pparas->ep_id);
        pparas->ep_id = NULL;
    }
    if(pparas->psk != NULL) 
    {
        osal_free(pparas->psk);
        pparas->psk = NULL;
    }
    if(pparas->psk_len != 0)
    {
    	pparas->psk_len = 0;
    }
    if(pparas->cb_name != NULL) 
    {
        osal_free(pparas->cb_name);
        pparas->cb_name = NULL;
    }
    return retCode;
}

/*---------------------ts_oc_coap_deinit-------------------------------------------- ----*/
static int ts_oc_coap_deinit(char *message, int len)
{
    int retcode = -1;
    osal_task_sleep(500);
	s_coap_handle = NULL;
    if(s_queue_rcvmsg && task_handle) 
    {
        queue_delete(s_queue_rcvmsg);
        osal_task_sleep(3000);
        osal_task_kill(task_handle);
    }
    printf("resource released!");
    retcode = 0;
    return retcode;
}


