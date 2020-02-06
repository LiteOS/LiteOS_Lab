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
 *  2019-10-10 13:14   YuHeng-P  The first version
 *
 */
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <osal.h>
#include <oc_coap_al.h>
#include <link_endian.h>

#define cn_endpoint_id        "coap_001"
#define cn_app_server         "127.0.0.1"
#define cn_app_port           "5683"

unsigned char libcoap_res[]=".well-known";
unsigned char libcoap_res1[]="core";
static void *ctx = NULL;


#define cn_app_connectivity    0
#define cn_app_lightstats      1
#define cn_app_light           2
#define cn_app_ledcmd          3
#define cn_app_cmdreply        4

#pragma pack(1)
typedef struct
{
    int8_t msgid;
    int16_t rsrp;
    int16_t ecl;
    int16_t snr;
    int32_t cellid;
}app_connectivity_t;

typedef struct
{
    int8_t msgid;
    int16_t tog;
}app_toggle_t;

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

#pragma pack()

static coap_al_initpara_t *initpara = NULL;
static coap_al_optpara_t *optpara = NULL;
static coap_al_reqpara_t *reqpara = NULL;
static coap_al_sndpara_t *sndpara = NULL;
static coap_al_rcvpara_t *rcvpara = NULL;



//if your command is very fast,please use a queue here--TODO
#define cn_app_rcv_buf_len 128
static int8_t          s_rcv_buffer[cn_app_rcv_buf_len];
static int             s_rcv_datalen;
static osal_semp_t     s_rcv_sync;

static void           *s_coap_handle = NULL;



//use this function to push all the message to the buffer
static int app_msg_deal(void *msg, int len)
{
    int ret = -1;

    if(len <= cn_app_rcv_buf_len)
    {
        memcpy(s_rcv_buffer,msg,len);
        s_rcv_datalen = len;

        osal_semp_post(s_rcv_sync);

        ret = 0;

    }
    return ret;
}






static int app_report_task_entry()
{
    int ret = -1;
    int lux = 0;
    void *opts = NULL;
    void *msg = NULL;

    initpara = (coap_al_initpara_t *)osal_malloc(sizeof(coap_al_initpara_t));
    optpara = (coap_al_optpara_t *)osal_malloc(sizeof(coap_al_optpara_t));
    reqpara = (coap_al_reqpara_t *)osal_malloc(sizeof(coap_al_reqpara_t));
    sndpara = (coap_al_sndpara_t *)osal_malloc(sizeof(coap_al_sndpara_t));
    rcvpara = (coap_al_rcvpara_t *)osal_malloc(sizeof(coap_al_rcvpara_t));



    app_light_intensity_t  light;

    memset(initpara,0,sizeof(coap_al_initpara_t));
    initpara->address = cn_app_server;
    initpara->port = atoi(cn_app_port);
    initpara->dealer = app_msg_deal;
    initpara->psk = NULL;
    initpara->psklen = 0;
    initpara->pskid = NULL;
    initpara->proto = COAP_PROTO_TCP;
    coap_al_init(initpara);
    ctx = initpara->ctx;

#if 0
    //send CSM
    char csm_data[] = {0x80, 0x01, 0x00};
    memset(optpara,0,sizeof(coap_al_optpara_t));
    optpara->head = opts;
    optpara->opt_num = 2;
    optpara->data = csm_data;
    optpara->len = 3;
    opts = coap_al_add_option(optpara);
    
    memset(reqpara,0,sizeof(coap_al_reqpara_t));
    reqpara->ctx = ctx;
    reqpara->msgtype = COAP_AL_MESSAGE_CON;
    reqpara->code = (7<<5) | 1;
    reqpara->optlst = opts;

    msg = coap_al_new_request(reqpara);
    memset(sndpara,0,sizeof(coap_al_sndpara_t));
    sndpara->handle = ctx;
    sndpara->msg = msg;
    printf("!~~~~~~~~~~\r\n");
    ret = coap_al_send(sndpara);
    printf("!~~~~~~~~~~ret:%d\r\n", ret);

    memset(rcvpara,0,sizeof(coap_al_rcvpara_t));
    rcvpara->ctx = ctx;
    ret = coap_al_recv(rcvpara);

    //if (ret < 0)
    {
        //atiny_set_errcode(ATINY_ERR_CODE_NETWORK);
        osal_task_sleep(100);
    }
#endif

    opts = NULL;
    memset(optpara,0,sizeof(coap_al_optpara_t));
    optpara->head = opts;
    optpara->opt_num = COAP_AL_OPTION_URI_PATH;
    optpara->data = (char *)libcoap_res;
    optpara->len = strlen(libcoap_res);
    opts = coap_al_add_option(optpara);

    optpara->head = opts;
    optpara->opt_num = COAP_AL_OPTION_URI_PATH;
    optpara->data = (char *)libcoap_res1;
    optpara->len = strlen(libcoap_res1);
    opts = coap_al_add_option(optpara);

    memset(reqpara,0,sizeof(coap_al_reqpara_t));
    reqpara->ctx = ctx;
    reqpara->msgtype = COAP_AL_MESSAGE_CON;
    reqpara->code = COAP_AL_REQUEST_GET;
    reqpara->optlst = opts;

    msg = coap_al_new_request(reqpara);
    memset(sndpara,0,sizeof(coap_al_sndpara_t));
    sndpara->handle = ctx;
    sndpara->msg = msg;
    printf("~~~~~~~~~~\r\n");
    ret = coap_al_send(sndpara);
    printf("~~~~~~~~~~ret:%d\r\n", ret);

    while (1)
    {
        memset(rcvpara,0,sizeof(coap_al_rcvpara_t));
        rcvpara->ctx = ctx;
        ret = coap_al_recv(rcvpara);

        if (ret < 0)
        {
            //atiny_set_errcode(ATINY_ERR_CODE_NETWORK);
            osal_task_sleep(100);
        }
    }


    if(NULL != s_coap_handle)   //success ,so we could receive and send
    {
        //install a dealer for the led message received
        while(1) //--TODO ,you could add your own code here
        {
            lux++;
            lux= lux%10000;
            printf("lux is %d!\r\n",lux);

            light.msgid = cn_app_light;
            light.intensity = htons(lux);
            oc_coap_report(s_coap_handle,(char *)&light,sizeof(light)); ///< report the light message
            osal_task_sleep(60*1000);
        }
    }

    return ret;
}

int standard_app_demo_main()
{
    osal_semp_create(&s_rcv_sync,1,0);

    osal_task_create("app_report",app_report_task_entry,NULL,0x1000,NULL,2);

    return 0;
}
