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

#if CONFIG_CNCS_SERVICE_ENABLE

#include <stddef.h>
#include <osal.h>
#include "cncs_service.h"

static service_id svid; // cncs service ID
cncs_para_t para; // cncs para
cncs_msg_t msg = {NULL, 0, 0};

void cncs_callback(void *msg, int ret)
{
	if (0 == ret)
	{
		printf("service ok, cncs para data is updated!\n");
        // here add code to deal with cncs para data
	}
	else
	{
		printf("sorry, something is wrong!\n");
        // here add code to deal with wrong situation
	}
	return;
}

int service_cncs_demo_main()
{
    int cmd_list = 0;
    // init and open service
    if (!cncs_service_init("cncs service")) 
    {
        return -1;
    }
    svid = service_open("cncs service");
    if (!svid) 
    {
        printf("cncs service open failed!\n");
        return -1;
    }
    msg.buf = &((void *)para);
    msg.len = sizeof(cncs_para_t):
    memset(msg.buf, 0, msg.len);

    // service demo 
    while (1)
    {
        switch (cmd_list)
        {
        case CNCS_CMD_SNR:
            msg.cmd = CNCS_CMD_SNR;
            service_send(svid, &msg, cncs_callback);
            break;
        case CNCS_CMD_RSRP:
            msg.cmd = CNCS_CMD_RSRP;
            service_send(svid, &msg, cncs_callback);
            break;
        case CNCS_CMD_ECL:
            msg.cmd = CNCS_CMD_ECL;
            service_send(svid, &msg, cncs_callback);
            break;
        case CNCS_CMD_CELLID:
            msg.cmd = CNCS_CMD_CELLID;
            service_send(svid, &msg, cncs_callback);
            break;
        case CNCS_CMD_PCI:
            msg.cmd = CNCS_CMD_PCI;
            service_send(svid, &msg, cncs_callback);
            break;
        case CNCS_CMD_IMSI:
            msg.cmd = CNCS_CMD_IMSI;
            service_send(svid, &msg, cncs_callback);
            break;
        case CNCS_CMD_IMEI:
            msg.cmd = CNCS_CMD_IMEI;
            service_send(svid, &msg, cncs_callback);
            break;
        
        default:
            break;
        }

        osal_task_sleep(1000);
        if (cmd_list++ > 10)
        {
            break;
        }        
    }
    
    // when exit, close service 
    if (!service_close(svid)) 
    {
        printf("cncs service close failed!\n");
        return -1;
    }

    return 0;
} /* service_cncs_demo_main */

#endif