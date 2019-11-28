/*----------------------------------------------------------------------------
 * Copyright (c) <2016-2018>, <Huawei Technologies Co., Ltd>
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

#include "ota_port.h"
#include "common.h"
#include <string.h>
#include <stdlib.h>
#include <board.h>
#include "partition.h"
#include "flash_adaptor.h"

static int hal_check_flash_param(ota_flash_type_e type, int32_t len, uint32_t location)
{
    return OK;
}

static int hal_read_flash(ota_flash_type_e type, void *buf, int32_t len, uint32_t location)
{
    if (hal_check_flash_param(type, len, location) != OK)
    {
        return ERR;
    }

    return storage_partition_read(type, buf, len, location);
}

static int hal_write_flash(ota_flash_type_e type, const void *buf, int32_t len, uint32_t location)
{
    if (hal_check_flash_param(type, len, location) != OK)
    {
        return ERR;
    }

    return storage_partition_write(type, buf, len, location);
}

void hal_get_ota_opt(ota_opt_s *opt)
{
    if (opt == NULL)
    {
        HAL_OTA_LOG("opt NULL");
        return;
    }

    memset(opt, 0, sizeof(*opt));
    opt->read_flash = hal_read_flash;
    opt->write_flash = hal_write_flash;
    opt->flash_block_size = FLASH_BLOCK_SIZE;
}



