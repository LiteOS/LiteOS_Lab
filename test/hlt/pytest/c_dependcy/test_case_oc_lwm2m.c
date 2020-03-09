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


#include <stdio.h>
#include <stdlib.h>
#include <sal.h>
#include <osal.h>
#include <queue.h>
#include <oc_lwm2m_al.h>
#include <link_endian.h>

#include "test_case.h"

#define cn_endpoint_id        "lwm2m_autotest_cy"
#define cn_app_server         "49.4.85.232"
#define cn_app_port           "5683"

#define cn_app_connectivity    0
#define cn_app_lightstats      1
#define cn_app_light           2
#define cn_app_ledcmd          3
#define cn_app_cmdreply        4

#define cn_endpoint_manufacturer "Agent_Tiny"
#define cn_endpoint_device_type  "Lwm2mFota"

#pragma pack(1)

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

#pragma pack()

typedef struct{
    char *server_ip4;
    char *server_port;
	char *endpoint_name;	
    char *passwd;
    en_oc_boot_strap_mode_t boot_mode;
    char *cb_name;
}oc_lwm2m_paras;
static oc_lwm2m_paras g_lwm2m_paras = {0};


#define cn_app_rcv_buf_len 128
static int8_t          s_rcv_buffer[cn_app_rcv_buf_len];
static int             s_rcv_datalen;
static osal_semp_t     s_rcv_sync;
static queue_t        *s_queue_rcvmsg = NULL;   ///< this is used to cached the message
static void           *recv_task_handle = NULL;
static void           *send_task_handle = NULL;  


/*OC LWM2M*/
static int ts_oc_lwm2m_init(char *message, int len);
static int ts_oc_lwm2m_register(char *message, int len);
static int ts_oc_lwm2m_config(char *message, int len);
static int ts_oc_lwm2m_report(char *message, int len);
static int ts_oc_lwm2m_getvalue(char *message, int len);
static int ts_oc_lwm2m_deconfig(char *message, int len);
static int ts_oc_lwm2m_deinit(char *message, int len);
static int ts_oc_lwm2m_deregister(char *message, int len);

test_entry ts_oc_lwm2m_entry_flist[]= {
    ts_oc_lwm2m_init,
    ts_oc_lwm2m_register,
    ts_oc_lwm2m_config,
    ts_oc_lwm2m_report,
    ts_oc_lwm2m_getvalue,
    ts_oc_lwm2m_deconfig,
    ts_oc_lwm2m_deinit,
    ts_oc_lwm2m_deregister
};

int ts_sort_oc_lwm2m_al(int entry_id, char *message, int len)
{
    if(entry_id >= TEST_OC_LWM2M_INVALID)
    {
        printf("invalid entry_id(%d)\n",entry_id);
        return TS_FAILED;
    }

    return (ts_oc_lwm2m_entry_flist[entry_id])(message, len);
}

//use this function to push all the message to the buffer
static int app_msg_deal(void *usr_data,en_oc_lwm2m_msg_t type, void *msg, int len)
{
    int ret = -1;

    if(len <= cn_app_rcv_buf_len)
    {
        memcpy(s_rcv_buffer,msg,len);
        s_rcv_datalen = len;
		printf("receive msgid = %d. \n", s_rcv_buffer[0]);
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
                    if (led_cmd->led[0] == 'O' && led_cmd->led[1] == 'N')
                    {
                        //if you need response message,do it here--TODO
                        replymsg.msgid = cn_app_cmdreply;
                        replymsg.mid = led_cmd->mid;
                        printf("reply mid is %d. \n\r",ntohs(replymsg.mid));
                        replymsg.errorcode = 0;
                        replymsg.curstats[0] = 'O';
                        replymsg.curstats[1] = 'N';
                        replymsg.curstats[2] = ' ';
                        for(int i = 0; i<3; i++)
                        {
                            led_current_state[i] = replymsg.curstats[i];
                        }
						printf("receive cmd = ON.\n ");
                        oc_lwm2m_report(s_lwm2m_handle,(char *)&replymsg,sizeof(replymsg),1000, OC_APP_DATA);    ///< report cmd reply message
                    }

                    else if (led_cmd->led[0] == 'O' && led_cmd->led[1] == 'F' && led_cmd->led[2] == 'F')
                    {

                        //if you need response message,do it here--TODO
                        replymsg.msgid = cn_app_cmdreply;
                        replymsg.mid = led_cmd->mid;
                        printf("reply mid is %d. \n\r",ntohs(replymsg.mid));
                        replymsg.errorcode = 0;
                        replymsg.curstats[0] = 'O';
                        replymsg.curstats[1] = 'F';
                        replymsg.curstats[2] = 'F';
                        for(int i = 0; i<3; i++)
                        {
                            led_current_state[i] = replymsg.curstats[i];
                        }
						printf("receive cmd = OFF.\n");
                        oc_lwm2m_report(s_lwm2m_handle,(char *)&replymsg,sizeof(replymsg),1000, OC_APP_DATA);    ///< report cmd reply message
                    }
                    else
                    {

                    }
                    break;
                default:
					led_cmd = (app_led_cmd_t *)s_rcv_buffer;
					printf("default_cy: LEDCMD:msgid:%d mid:%d msg:%s \n\r",led_cmd->msgid,ntohs(led_cmd->mid),led_cmd->led);
                    printf("default_cy: led cmd: %d %d %d.\n",led_cmd->led[0],led_cmd->led[1],led_cmd->led[2]);
					break;
            }
        }
    }

    return ret;
}


static int app_report_task_entry()
{
    static int lux = 0;	
    app_light_intensity_t  light;	

    printf("Calling app_report_task_entry(), start to report data.\n");
    //
    lux++;
    lux= lux%10000;

    light.msgid = cn_app_light;
    light.intensity = htons(lux);
    oc_lwm2m_report((char *)&light,sizeof(light),1000, OC_APP_DATA); ///< report the light message
    osal_task_sleep(10*1000);

    return 0;
}

static int pp_oc_config_test(oc_config_param_t *param)
{
    printf("this is lwm2m at_oc_config\n");
    return 0;
}

static int pp_oc_deconfig_test(void *context)
{
    printf("this is lwm2m at_oc_deconfig\n");
    return 0;
}

static int pp_oc_report_test(char *message, int len)
{
    printf("this is lwm2m at_oc_report_test\n");
    return 0;
}

static oc_lwm2m_opt_t s_oc_lwm2m_ops_test = \
{
	.config = pp_oc_config_test,
	.deconfig = pp_oc_deconfig_test,
	.report = pp_oc_report_test,
};


static int ts_oc_lwm2m_init(char *message, int len)
{
    #include <oc_lwm2m_al.h>
	
//    osal_semp_create(&s_rcv_sync,1,0);
//    osal_task_create("app_command",app_cmd_task_entry,NULL,0x1000,NULL,3);

	return oc_lwm2m_init();
}

static int ts_oc_lwm2m_register(char *message, int len)
{
    int retCode = -1;
	int ret = -1;
	char *name = "test_name";

	retCode = oc_lwm2m_register(name, &s_oc_lwm2m_ops_test);
    if (retCode == 0)
    {
        retCode = oc_lwm2m_unregister(name);
        if (retCode == -1)
        {
            printf("oc lwm2m unregister stubcode failed, exit from %s\n", __FUNCTION__);
            return retCode;
        }
    }
    else
    {
        printf("oc lwm2m has already registered.\n");
        return retCode;
    }

    //install agent lwm2m
    #include <agent_lwm2m.h>
	ret = oc_lwm2m_install_agent();

	printf("oc_lwm2m_register retCode: %d.\n", ret);
    osal_task_sleep(500);
    printf("exit from %s\n", __FUNCTION__);

    return ret;	
}

static int ts_oc_lwm2m_config(char *message, int len)
{
    char *pchTmp, *pchStrTmpIn;
    oc_config_param_t oc_param;
    memset(&oc_param,0,sizeof(oc_param));
    oc_lwm2m_paras *pparas = &g_lwm2m_paras;
    int retcode  = -1;

    printf("[%s] g_tempbuf is %s\n", __FUNCTION__, message);
    pchTmp	   = message;
    pchStrTmpIn = NULL;

    /** message format: test_id|testcase_name|ip|port|endpoint|passwd|bootstrap_mode|cb_name **/
    pchTmp = strtok_r(pchTmp, "|", &pchStrTmpIn); 
    printf("[%s]test id is %d\n",__FUNCTION__,atoi(pchTmp));
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn); 
    printf("[%s]testcase name is %s\n",__FUNCTION__,pchTmp);


    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    if(pparas->server_ip4 != NULL) 
    {
        osal_free(pparas->server_ip4);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) || (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->server_ip4 = NULL;
    }
    else
    {
        pparas->server_ip4 = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->server_ip4, pchTmp, strlen(pchTmp));
        pparas->server_ip4[strlen(pchTmp)] = '\0';
    }
	printf("server ipv4 is %s\n",pparas->server_ip4);


    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    if(pparas->server_port != NULL) 
    {
        osal_free(pparas->server_port);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->server_port = NULL;
    }
    else
    {
        pparas->server_port = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->server_port, pchTmp, strlen(pchTmp));
        pparas->server_port[strlen(pchTmp)] = '\0';
    }
    printf("oc server port is %s\n",pparas->server_port);


    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    if(pparas->endpoint_name != NULL) 
    {
        osal_free(pparas->endpoint_name);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->endpoint_name = NULL;
    }
    else
    {
        pparas->endpoint_name = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->endpoint_name, pchTmp, strlen(pchTmp));
        pparas->endpoint_name[strlen(pchTmp)] = '\0';
    }
    printf("device endpoint name is %s\n", pparas->endpoint_name);


    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    if(pparas->passwd != NULL) 
    {
        osal_free(pparas->passwd);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->passwd = NULL;
    }
    else
    {
        pparas->passwd = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->passwd, pchTmp, strlen(pchTmp));
        pparas->passwd[strlen(pchTmp)] = '\0';
    }
    printf("password is %s\n",pparas->passwd);


    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
	pparas->boot_mode = atoi(pchTmp);
    printf("boot_mode is %d\n",pparas->boot_mode);

    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
	if(pparas->cb_name != NULL) 
    {
        osal_free(pparas->cb_name);
    }
	pparas->cb_name = osal_malloc(strlen(pchTmp)+1);
	memcpy(pparas->cb_name, pchTmp, strlen(pchTmp));
	pparas->cb_name[strlen(pchTmp)] = '\0';
    printf("cb_name is %s\n",pparas->cb_name);

    oc_param.app_server.address = pparas->server_ip4;
    oc_param.app_server.port = pparas->server_port;
    oc_param.app_server.ep_id = pparas->endpoint_name;
    oc_param.boot_mode = pparas->boot_mode;
    oc_param.rcv_func = (pparas->cb_name ? app_msg_deal : NULL);

    if (0 != oc_lwm2m_config(&oc_param))
    {
        osal_semp_create(&s_rcv_sync,1,0);
        recv_task_handle = osal_task_create("app_command",app_cmd_task_entry,NULL,0x1000,NULL,3);
		retcode = 0;
    }
    return retcode;
}

static int ts_oc_lwm2m_report(char *message, int len)
{ 
    osal_task_sleep(500);
    //send_task_handle = osal_task_create("app_report",app_report_task_entry,NULL,0x1000,NULL,2);
    app_report_task_entry();
	return 0;
}

static int ts_oc_lwm2m_getvalue(char *message, int len)
{
    printf("[ts_oc_lwm2m_getvalue] LED state is %s \n", led_current_state);
    memset(message, 0, 32);/*32 is more large than one int32*/
    snprintf(message,sizeof(led_current_state)+1,"%s",led_current_state);
    
    return TS_OK_HAS_DATA;
}

static int ts_oc_lwm2m_deconfig(char *message, int len)
{
    int retCode = 0;
    oc_lwm2m_paras *pparas = &g_lwm2m_paras;

//    osal_task_sleep(3000);
//    osal_task_kill(send_task_handle);

    printf("start to call oc_lem2m_deconfig. \n");
	retCode = oc_lwm2m_deconfig(s_lwm2m_handle);

    printf("exit from %s, retcode is %d\n", __FUNCTION__, retCode);
    if(pparas->server_ip4 != NULL) 
    {
        osal_free(pparas->server_ip4);
        pparas->server_ip4 = NULL;
    }
    if(pparas->server_port != NULL) 
    {
        osal_free(pparas->server_port);
        pparas->server_port = NULL;
    }
    if(pparas->endpoint_name != NULL) 
    {
        osal_free(pparas->endpoint_name);
        pparas->endpoint_name = NULL;
    }
    if(pparas->passwd != NULL) 
    {
        osal_free(pparas->passwd);
        pparas->passwd = NULL;
    }
    if(pparas->cb_name != NULL) 
    {
        osal_free(pparas->cb_name);
        pparas->cb_name = NULL;
    }

    return retCode;
}

static int ts_oc_lwm2m_deinit(char *message, int len)
{
    osal_task_sleep(500);
	s_lwm2m_handle = NULL;
    if(s_queue_rcvmsg && recv_task_handle) 
    {
        queue_delete(s_queue_rcvmsg);
        osal_task_sleep(3000);
        osal_task_kill(recv_task_handle);
    }
    printf("resource released!");
    return 0;
}

static int ts_oc_lwm2m_deregister(char *message, int len)
{
    return oc_lwm2m_unregister("oc_lwm2m_agent");
}



=======
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


#include <stdio.h>
#include <stdlib.h>
#include <sal.h>
#include <osal.h>
#include <queue.h>
#include <oc_lwm2m_al.h>
#include <link_endian.h>

#include "test_case.h"

#define cn_endpoint_id        "lwm2m_autotest_cy"
#define cn_app_server         "49.4.85.232"
#define cn_app_port           "5683"

#define cn_app_connectivity    0
#define cn_app_lightstats      1
#define cn_app_light           2
#define cn_app_ledcmd          3
#define cn_app_cmdreply        4

#define cn_endpoint_manufacturer "Agent_Tiny"
#define cn_endpoint_device_type  "Lwm2mFota"

#pragma pack(1)

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

#pragma pack()

typedef struct{
    char *server_ip4;
    char *server_port;
	char *endpoint_name;	
    char *passwd;
    en_oc_boot_strap_mode_t boot_mode;
    char *cb_name;
}oc_lwm2m_paras;
static oc_lwm2m_paras g_lwm2m_paras = {0};


#define cn_app_rcv_buf_len 128
static int8_t          s_rcv_buffer[cn_app_rcv_buf_len];
static int             s_rcv_datalen;
static osal_semp_t     s_rcv_sync;
static void           *s_lwm2m_handle = NULL;
static queue_t        *s_queue_rcvmsg = NULL;   ///< this is used to cached the message
static void           *recv_task_handle = NULL;
static void           *send_task_handle = NULL;  


/*OC LWM2M*/
static int ts_oc_lwm2m_init(char *message, int len);
static int ts_oc_lwm2m_register(char *message, int len);
static int ts_oc_lwm2m_config(char *message, int len);
static int ts_oc_lwm2m_report(char *message, int len);
static int ts_oc_lwm2m_getvalue(char *message, int len);
static int ts_oc_lwm2m_deconfig(char *message, int len);
static int ts_oc_lwm2m_deinit(char *message, int len);
static int ts_oc_lwm2m_deregister(char *message, int len);

test_entry ts_oc_lwm2m_entry_flist[]= {
    ts_oc_lwm2m_init,
    ts_oc_lwm2m_register,
    ts_oc_lwm2m_config,
    ts_oc_lwm2m_report,
    ts_oc_lwm2m_getvalue,
    ts_oc_lwm2m_deconfig,
    ts_oc_lwm2m_deinit,
    ts_oc_lwm2m_deregister
};

int ts_sort_oc_lwm2m_al(int entry_id, char *message, int len)
{
    if(entry_id >= TEST_OC_LWM2M_INVALID)
    {
        printf("invalid entry_id(%d)\n",entry_id);
        return TS_FAILED;
    }

    return (ts_oc_lwm2m_entry_flist[entry_id])(message, len);
}

//use this function to push all the message to the buffer
static int app_msg_deal(void *usr_data,en_oc_lwm2m_msg_t type, void *msg, int len)
{
    int ret = -1;

    if(len <= cn_app_rcv_buf_len)
    {
        memcpy(s_rcv_buffer,msg,len);
        s_rcv_datalen = len;
		printf("receive msgid = %d. \n", s_rcv_buffer[0]);
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
                    if (led_cmd->led[0] == 'O' && led_cmd->led[1] == 'N')
                    {
                        //if you need response message,do it here--TODO
                        replymsg.msgid = cn_app_cmdreply;
                        replymsg.mid = led_cmd->mid;
                        printf("reply mid is %d. \n\r",ntohs(replymsg.mid));
                        replymsg.errorcode = 0;
                        replymsg.curstats[0] = 'O';
                        replymsg.curstats[1] = 'N';
                        replymsg.curstats[2] = ' ';
                        for(int i = 0; i<3; i++)
                        {
                            led_current_state[i] = replymsg.curstats[i];
                        }
						printf("receive cmd = ON.\n ");
                        oc_lwm2m_report(s_lwm2m_handle,(char *)&replymsg,sizeof(replymsg),1000, OC_APP_DATA);    ///< report cmd reply message
                    }

                    else if (led_cmd->led[0] == 'O' && led_cmd->led[1] == 'F' && led_cmd->led[2] == 'F')
                    {

                        //if you need response message,do it here--TODO
                        replymsg.msgid = cn_app_cmdreply;
                        replymsg.mid = led_cmd->mid;
                        printf("reply mid is %d. \n\r",ntohs(replymsg.mid));
                        replymsg.errorcode = 0;
                        replymsg.curstats[0] = 'O';
                        replymsg.curstats[1] = 'F';
                        replymsg.curstats[2] = 'F';
                        for(int i = 0; i<3; i++)
                        {
                            led_current_state[i] = replymsg.curstats[i];
                        }
						printf("receive cmd = OFF.\n");
                        oc_lwm2m_report(s_lwm2m_handle,(char *)&replymsg,sizeof(replymsg),1000, OC_APP_DATA);    ///< report cmd reply message
                    }
                    else
                    {

                    }
                    break;
                default:
					led_cmd = (app_led_cmd_t *)s_rcv_buffer;
					printf("default_cy: LEDCMD:msgid:%d mid:%d msg:%s \n\r",led_cmd->msgid,ntohs(led_cmd->mid),led_cmd->led);
                    printf("default_cy: led cmd: %d %d %d.\n",led_cmd->led[0],led_cmd->led[1],led_cmd->led[2]);
					break;
            }
        }
    }

    return ret;
}


static int app_report_task_entry()
{
    static int lux = 0;	
    app_light_intensity_t  light;	

    printf("Calling app_report_task_entry(), start to report data.\n");
    if(NULL != s_lwm2m_handle)   //success ,so we could receive and send
    {
        //install a dealer for the led message received
        //while(1)
        //{
            lux++;
            lux= lux%10000;

            light.msgid = cn_app_light;
            light.intensity = htons(lux);
            oc_lwm2m_report(s_lwm2m_handle,(char *)&light,sizeof(light),1000, OC_APP_DATA); ///< report the light message
            osal_task_sleep(10*1000);
        //}
    }

    return 0;
}

static int pp_oc_config_test(oc_config_param_t *param)
{
    printf("this is lwm2m at_oc_config\n");
    return 0;
}

static int pp_oc_deconfig_test(void *context)
{
    printf("this is lwm2m at_oc_deconfig\n");
    return 0;
}

static int pp_oc_report_test(char *message, int len)
{
    printf("this is lwm2m at_oc_report_test\n");
    return 0;
}

static oc_lwm2m_opt_t s_oc_lwm2m_ops_test = \
{
	.config = pp_oc_config_test,
	.deconfig = pp_oc_deconfig_test,
	.report = pp_oc_report_test,
};


static int ts_oc_lwm2m_init(char *message, int len)
{
    #include <oc_lwm2m_al.h>
	
//    osal_semp_create(&s_rcv_sync,1,0);
//    osal_task_create("app_command",app_cmd_task_entry,NULL,0x1000,NULL,3);

	return oc_lwm2m_init();
}

static int ts_oc_lwm2m_register(char *message, int len)
{
    int retCode = -1;
	int ret = -1;
	char *name = "test_name";

	retCode = oc_lwm2m_register(name, &s_oc_lwm2m_ops_test);
    if (retCode == 0)
    {
        retCode = oc_lwm2m_unregister(name);
        if (retCode == -1)
        {
            printf("oc lwm2m unregister stubcode failed, exit from %s\n", __FUNCTION__);
            return retCode;
        }
    }
    else
    {
        printf("oc lwm2m has already registered.\n");
        return retCode;
    }

    //install agent lwm2m
    #include <agent_lwm2m.h>
	ret = oc_lwm2m_install_agent();

	printf("oc_lwm2m_register retCode: %d.\n", ret);
    osal_task_sleep(500);
    printf("exit from %s\n", __FUNCTION__);

    return ret;	
}

static int ts_oc_lwm2m_config(char *message, int len)
{
    char *pchTmp, *pchStrTmpIn;
    oc_config_param_t oc_param;
    memset(&oc_param,0,sizeof(oc_param));
    oc_lwm2m_paras *pparas = &g_lwm2m_paras;
    int retcode  = -1;

    printf("[%s] g_tempbuf is %s\n", __FUNCTION__, message);
    pchTmp	   = message;
    pchStrTmpIn = NULL;

    /** message format: test_id|testcase_name|ip|port|endpoint|passwd|bootstrap_mode|cb_name **/
    pchTmp = strtok_r(pchTmp, "|", &pchStrTmpIn); 
    printf("[%s]test id is %d\n",__FUNCTION__,atoi(pchTmp));
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn); 
    printf("[%s]testcase name is %s\n",__FUNCTION__,pchTmp);


    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    if(pparas->server_ip4 != NULL) 
    {
        osal_free(pparas->server_ip4);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) || (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->server_ip4 = NULL;
    }
    else
    {
        pparas->server_ip4 = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->server_ip4, pchTmp, strlen(pchTmp));
        pparas->server_ip4[strlen(pchTmp)] = '\0';
    }
	printf("server ipv4 is %s\n",pparas->server_ip4);


    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    if(pparas->server_port != NULL) 
    {
        osal_free(pparas->server_port);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->server_port = NULL;
    }
    else
    {
        pparas->server_port = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->server_port, pchTmp, strlen(pchTmp));
        pparas->server_port[strlen(pchTmp)] = '\0';
    }
    printf("oc server port is %s\n",pparas->server_port);


    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    if(pparas->endpoint_name != NULL) 
    {
        osal_free(pparas->endpoint_name);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->endpoint_name = NULL;
    }
    else
    {
        pparas->endpoint_name = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->endpoint_name, pchTmp, strlen(pchTmp));
        pparas->endpoint_name[strlen(pchTmp)] = '\0';
    }
    printf("device endpoint name is %s\n", pparas->endpoint_name);


    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    if(pparas->passwd != NULL) 
    {
        osal_free(pparas->passwd);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->passwd = NULL;
    }
    else
    {
        pparas->passwd = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->passwd, pchTmp, strlen(pchTmp));
        pparas->passwd[strlen(pchTmp)] = '\0';
    }
    printf("password is %s\n",pparas->passwd);


    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
	pparas->boot_mode = atoi(pchTmp);
    printf("boot_mode is %d\n",pparas->boot_mode);

    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
	if(pparas->cb_name != NULL) 
    {
        osal_free(pparas->cb_name);
    }
	pparas->cb_name = osal_malloc(strlen(pchTmp)+1);
	memcpy(pparas->cb_name, pchTmp, strlen(pchTmp));
	pparas->cb_name[strlen(pchTmp)] = '\0';
    printf("cb_name is %s\n",pparas->cb_name);

    oc_param.app_server.address = pparas->server_ip4;
    oc_param.app_server.port = pparas->server_port;
    oc_param.app_server.ep_id = pparas->endpoint_name;
    oc_param.boot_mode = pparas->boot_mode;
    oc_param.rcv_func = (pparas->cb_name ? app_msg_deal : NULL);

	s_lwm2m_handle = oc_lwm2m_config(&oc_param);
    if (NULL != s_lwm2m_handle)
    {
        osal_semp_create(&s_rcv_sync,1,0);
        recv_task_handle = osal_task_create("app_command",app_cmd_task_entry,NULL,0x1000,NULL,3);
		retcode = 0;
    }
    return retcode;
}

static int ts_oc_lwm2m_report(char *message, int len)
{ 
    osal_task_sleep(500);
    //send_task_handle = osal_task_create("app_report",app_report_task_entry,NULL,0x1000,NULL,2);
    app_report_task_entry();
	return 0;
}

static int ts_oc_lwm2m_getvalue(char *message, int len)
{
    printf("[ts_oc_lwm2m_getvalue] LED state is %s \n", led_current_state);
    memset(message, 0, 32);/*32 is more large than one int32*/
    snprintf(message,sizeof(led_current_state)+1,"%s",led_current_state);
    
    return TS_OK_HAS_DATA;
}

static int ts_oc_lwm2m_deconfig(char *message, int len)
{
    int retCode = 0;
    oc_lwm2m_paras *pparas = &g_lwm2m_paras;

//    osal_task_sleep(3000);
//    osal_task_kill(send_task_handle);

    printf("start to call oc_lem2m_deconfig. \n");
	retCode = oc_lwm2m_deconfig(s_lwm2m_handle);

    printf("exit from %s, retcode is %d\n", __FUNCTION__, retCode);
    if(pparas->server_ip4 != NULL) 
    {
        osal_free(pparas->server_ip4);
        pparas->server_ip4 = NULL;
    }
    if(pparas->server_port != NULL) 
    {
        osal_free(pparas->server_port);
        pparas->server_port = NULL;
    }
    if(pparas->endpoint_name != NULL) 
    {
        osal_free(pparas->endpoint_name);
        pparas->endpoint_name = NULL;
    }
    if(pparas->passwd != NULL) 
    {
        osal_free(pparas->passwd);
        pparas->passwd = NULL;
    }
    if(pparas->cb_name != NULL) 
    {
        osal_free(pparas->cb_name);
        pparas->cb_name = NULL;
    }

    return retCode;
}

static int ts_oc_lwm2m_deinit(char *message, int len)
{
    osal_task_sleep(500);
	s_lwm2m_handle = NULL;
    if(s_queue_rcvmsg && recv_task_handle) 
    {
        queue_delete(s_queue_rcvmsg);
        osal_task_sleep(3000);
        osal_task_kill(recv_task_handle);
    }
    printf("resource released!");
    return 0;
}

static int ts_oc_lwm2m_deregister(char *message, int len)
{
    return oc_lwm2m_unregister("oc_lwm2m_agent");
}
