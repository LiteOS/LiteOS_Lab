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
 *  2019-08-07 14:56  zhangqianfu  The first version
 *
 */
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <sal.h>
#include <link_endian.h>
#include <shell.h>
#include <time.h>
#include <osal.h>

///< return 0 success while -1 failed
static int __test_sal_tcp(const char *host,const char *port)
{
    int fd = -1;
    struct sockaddr_in addr;
    struct timeval timedelay;


    ///< first we try use the gethostbyname to get the ip address, the host maybe a domain name
    struct hostent* entry = NULL;
    entry = sal_gethostbyname(host);
    if( !(entry && entry->h_addr_list[0] && (entry->h_addrtype == AF_INET)))
    {
       printf("gethostbyname:%s failed\n\r",host);
       goto EXIT_GETIP;
    }

    fd = sal_socket(AF_INET,SOCK_STREAM,0);
    if(fd == -1)
    {
        printf("socket:failed\n\r");
        goto EXIT_SOCKET;
    }

    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    memcpy(&addr.sin_addr.s_addr,entry->h_addr_list[0],sizeof(addr.sin_addr.s_addr));
    addr.sin_port = htons(atoi(port));
    if( 0 != sal_connect(fd,(struct sockaddr *)&addr,sizeof(addr)))
    {
        printf("connect:%s failed\n\r",host);
        goto EXIT_CONNECT;
    }

    timedelay.tv_sec = 10;
    timedelay.tv_usec = 1000;
    ///< set the recv timeout
    if(0 != sal_setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&timedelay,sizeof(timedelay)))
    {
        printf("setsockopt:%d %d failed\n\r",SOL_SOCKET,SO_RCVTIMEO);
        goto EXIT_SETSOCKOPT;
    }

    if(0 != sal_closesocket(fd))
    {
        printf("socketclose:failed\n\r");
        goto EXIT_CLOSE;
    }
    return 0;

EXIT_CLOSE:
EXIT_SETSOCKOPT:
EXIT_CONNECT:
    sal_closesocket( fd );
EXIT_SOCKET:
EXIT_GETIP:
    return -1;
}


static int sal_test_main(int argc, const char *argv[])
{
    int test_counter = 0;
    int test_ret = 0;
    int test_success = 0;
    const char *host;
    const char *port;
    int         test_times_limit;


    if(argc != 4)
    {
        return -1;
    }

    host = argv[1];
    port = argv[2];
    test_times_limit = atoi(argv[3]);


    for(test_counter = 1; test_counter <= test_times_limit; test_counter ++)
    {
        test_ret = __test_sal_tcp(host,port);
        if(test_ret  == 0)
        {
            test_success++;
        }
        printf("TEST:%d SUCCESS:%d\n\r", test_counter,test_success);

        osal_task_sleep(1000);
    }

    return 0;
}
OSSHELL_EXPORT_CMD(sal_test_main,"saltest","saltest host port times");

