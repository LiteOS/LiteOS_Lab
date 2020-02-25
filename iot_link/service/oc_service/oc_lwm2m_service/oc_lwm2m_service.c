/*----------------------------------------------------------------------------
 * Copyright (c) <2020>, <Huawei Technologies Co., Ltd>
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

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <osal.h>
#include <oc_lwm2m_al.h>
//
#include "oc_service.h"

/* macros */


/* typedefs */


/* locals */


static int __oc_service_handler (void * arg)
{
    oc_message* msg = (oc_message*) arg;
    //printf("message is %s\r\n", msg->buf);
    int ret;
    switch (msg->type)
    {
        case oc_config:
        {
        	printf("this is oc_config service!\r\n");

        	oc_config_param_t* config = (oc_config_param_t *)(msg->buf);
        	ret = oc_lwm2m_config(config);
        	if((ret != en_oc_lwm2m_err_ok))
        	{
        	    printf("config:err :code:%d\r\n",ret);
        	    return -1;
        	}
        }
        break;
        case oc_report:
        {
            printf("this is oc_report service!\r\n");

            char* data = (char *)(msg->buf);
            ret = oc_lwm2m_report((char *)data, msg->len, 1000);

            if((ret != en_oc_lwm2m_err_ok))
            {
                printf("report:err :code:%d\r\n",ret);
                return -1;
            }
        }
        break;
        case oc_deconfig:
        {
            printf("this is oc_deconfig service!\r\n");
            oc_lwm2m_deconfig();
        }
        break;
    }

    return 0;
}

int oc_service_init (const char* name)
{
	if (service_create (name, __oc_service_handler, 0x1000, 6) == INVALID_SID)
	{
	    printf ("Fail to create oc service!\n");

	    return -1;
	}

	return 0;
}
