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
 *  2019-04-28 14:25  zhangqianfu  The first version  
 *
 */
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/errno.h"
#include "lwip/tcpip.h"

#include <sal_imp.h>   ///< register the lwip to sal


static const tag_tcpip_ops s_tcpip_lwip_ops =
{
   .socket = (fn_sal_socket)lwip_socket,
   .bind = (fn_sal_bind)lwip_bind,
   .listen = (fn_sal_listen)lwip_listen,
   .connect = (fn_sal_connect)lwip_connect,
   .accept = (fn_sal_accept)lwip_accept,
   .send = (fn_sal_send)lwip_send,
   .sendto = (fn_sal_sendto)lwip_sendto,
   .recv = (fn_sal_recv)lwip_recv,
   .recvfrom = (fn_sal_recvfrom)lwip_recv,
   .setsockopt = (fn_sal_setsockopt)lwip_setsockopt,
   .getsockopt = (fn_sal_getsockopt)lwip_getsockopt,
   .shutdown =(fn_sal_shutdown) lwip_shutdown,
   .closesocket =(fn_sal_closesocket) lwip_close,
   .getpeername =(fn_sal_getpeername)lwip_getpeername,
   .getsockname = (fn_sal_getsockname)lwip_getsockname,
};

static const tag_tcpip_domain s_tcpip_lwip =
{
    .name = "lwip",
    .domain = AF_INET,
    .ops = &s_tcpip_lwip_ops,
};


int tcpip_sal_install_lwip(fn_lwip_netdriver driver)
{
    int ret = -1;
    /* Initilialize the LwIP stack with RTOS */
    tcpip_init(NULL, NULL);

    if(NULL != driver)
    {
        ret = driver();
        if(ret != 0)
        {
            return ret;
        }
    }


    ret = tcpip_sal_install(&s_tcpip_lwip);

    if(0 == ret)
    {
        printf("sal:install lwip success\r\n");
    }
    else
    {
        printf("sal:install lwip failed\r\n");
    }




    return 0;
}






