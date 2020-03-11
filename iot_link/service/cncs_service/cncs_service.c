/*----------------------------------------------------------------------------
 * Copyright (c) <2020>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
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
/* service */
#include "cncs_service.h"

/* macros */


/* typedefs */


/* locals */

static int __cncs_service_handler(void *arg)
{
    cncs_msg_t *msg = (cncs_msg_t *)arg;
    printf("cncs receive message command is %d\r\n", msg->cmd);
    int ret;
    switch (msg->cmd)
    {
        case CNCS_CMD_SNR:
            printf("please report network para SNR\r\n");
            // umip_al_module(msg);
            break;
        case CNCS_CMD_RSRP:
            printf("please report network para RSRP\r\n");
            // umip_al_module(msg);
            break;
        case CNCS_CMD_ECL:
            printf("please report network para ECL\r\n");
            // umip_al_module(msg);
            break;
        case CNCS_CMD_CELLID:
            printf("please report network para CELLID\r\n");
            // umip_al_module(msg);
            break;
        case CNCS_CMD_PCI:
            printf("please report network para PCI\r\n");
            // umip_al_module(msg);
            break;
        case CNCS_CMD_IMSI:
            printf("please report network para IMSI\r\n");
            // umip_al_module(msg);
            break;
        case CNCS_CMD_IMEI:
            printf("please report network para IMEI\r\n");
            // umip_al_module(msg);
            break;

        default:
            break;
    }

    return ret;
}

int cncs_service_init(const char *name)
{
	if (service_create(SERVICE_DOMAIN_SYSTEM, name, __cncs_service_handler, 6) == INVALID_SID)
	{
	    printf("Fail to create cncs service! \n");
	    return -1;
	}

	return 0;
}
