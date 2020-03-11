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

#ifndef __CNCS_SERVICE_H__
#define __CNCS_SERVICE_H__

#include <stdint.h>
#include "service.h"

/* macros */

/* typedefs */
#pragma pack(1)
typedef struct 
{
    int8_t SNR;
    int16_t RSRP;
    int8_t ECL;
    int32_t CELLID;
    int16_t PCI;
    char IMSI[15];
    char IMEI[15];
}cncs_para_t;
#pragma pack()

typedef enum 
{
    CNCS_CMD_SNR = 0,  // command get snr
    CNCS_CMD_RSRP,     // command get rsrp
    CNCS_CMD_ECL,      // command get ecl  
    CNCS_CMD_CELLID,   // command get cellid         
    CNCS_CMD_PCI,      // command get pci          
    CNCS_CMD_IMSI,     // command get imsi              
    CNCS_CMD_IMEI,     // command get imei       
}cncs_cmd_e;

typedef struct
{
    void *buf;
    int   len;
    cncs_cmd_e   cmd;
}cncs_msg_t;

/* externs */

extern int cncs_service_init(const char *name);


#endif /* __CNCS_SERVICE_H__ */
