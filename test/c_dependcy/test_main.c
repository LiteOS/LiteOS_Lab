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
 *  2019-11-13       xuliqun  The first version  
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sal.h>
#include <osal.h>
#include "test_case.h"

#define LOCAL_PORT  5999



test_sort ts_sort_flist[] = {
    NULL,
    NULL,
    ts_sort_oc_mqtt_al,
    ts_sort_mqtt_al,
};

char g_acRecvBuf[TS_RECV_BUFFER_LEN];
static char g_acSendBuf[TS_RECV_BUFFER_LEN+128];

int g_apitestfin = 0;


int test_main_entry(void *paras)
{
    int ret;
    int server_fd;
    struct sockaddr_in ser_addr;
    struct sockaddr_in cli_addr;
    socklen_t addr_len = sizeof(cli_addr);
    int recv_len,send_len;
    int test_id;
    int prot_id,entry_id;
    int result;
    char *pchTmp, *pchStrTmpIn;
    struct hostent *entry;
    
    server_fd = sal_socket(AF_INET, SOCK_DGRAM, 0);
    if(server_fd < 0)
    {
        printf("create socket fail!\n");
        return -1;
    }

    printf("udp server bind 5999 \n");


    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    ser_addr.sin_port = htons(LOCAL_PORT);
    ret = sal_bind(server_fd, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
    if(ret < 0){
        printf("socket bind fail!\n");
        return -1;
    }

    printf("will come into test_main_entry \n");

    for(;;)
    {
        memset(g_acRecvBuf, 0, sizeof(g_acRecvBuf));
        recv_len = sal_recvfrom(server_fd, g_acRecvBuf, sizeof(g_acRecvBuf), 0, (struct sockaddr *)&cli_addr, &addr_len);
        if (recv_len < 0)
        {
           printf("error(%d) occur while receive\n",recv_len);
           break;
        }
        printf("recve msg: %s,%d\n",g_acRecvBuf,recv_len);
        memcpy(g_acSendBuf, g_acRecvBuf, recv_len);

        pchTmp      = g_acSendBuf;
        pchStrTmpIn = NULL;
        pchTmp      = strtok_r(pchTmp, "|", &pchStrTmpIn);
        test_id     = atoi(pchTmp);
        prot_id     = test_id >> 16;
        entry_id    = test_id & 0xFFFF;

        printf("$$$$$$$$$$,test case will run ,prot_id:%x, entry_id: %x\n",prot_id,entry_id);
        result = TS_FAILED;
        if(prot_id < TEST_SORT_MAX)
        {
            result = ts_sort_flist[prot_id](entry_id, g_acRecvBuf, recv_len);
        }

        memset(g_acSendBuf,0,sizeof(g_acSendBuf));
        if(result != TS_OK_HAS_DATA)
            snprintf(g_acSendBuf,sizeof(g_acSendBuf),"%d,|%d,|",test_id,result);
        else
            snprintf(g_acSendBuf,sizeof(g_acSendBuf),"%d,|%d,|%s",test_id,result,g_acRecvBuf);
        
        printf("testcase result is %d, send ==++==***:%s\n",result, g_acSendBuf);
        send_len = sal_sendto(server_fd, g_acSendBuf, strlen(g_acSendBuf), 0, (struct sockaddr *)&cli_addr, addr_len);
        if (send_len < 0)
        {
            printf("error(%d) occur while send!!!\n",send_len);
        }
    }

    return 0;
}


void autotest_start(void)
{
    osal_task_create("test_main",test_main_entry,NULL,0x1000,NULL,10);
    
}




