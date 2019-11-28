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

/**@defgroup atiny_adapter Agenttiny Adapter
 * @ingroup agent
 */

#ifndef OTA_ADAPTOR_H
#define OTA_ADAPTOR_H

#include "hal_spi_flash.h"
#include "ota_flag.h"
#include "board.h"
#include "hal_flash.h" 

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum {
	PACKAGE_TYPE_FULL = 0,
	PACKAGE_TYPE_DIFF,
}EN_PACKAGE_TYPE;

typedef enum {
	UPGRADE_RESULT_SUCC = 0x00,
	UPGRADE_RESULT_DEV_BUSY = 0x01,
	UPGRADE_RESULT_NOSPACE = 0x05,
	UPGRADE_RESULT_FIRMWARE_NOTMATCH = 0x8,
	UPGRADE_RESULT_MEMEXHAUSTED = 0x09,
	UPGRADE_RESULT_INNERERROR = 0x7f,
}EN_UPGRADE_RESULT;

#pragma pack(1)
typedef struct {
	uint8_t pack_type;
	uint8_t reserved[31];
}ota_binary_info;
#pragma pack()

#define OTA_BINARY_OFFSET (OTA_SIGNATURE_LEN + sizeof(ota_binary_info))

void hal_init_ota(void);
void ota_update_upgrade_result(ota_flag_t *flag, uint32_t result);
EN_PACKAGE_TYPE get_upgrade_type();   /* 0: full upgrade, 1: diff upgrade */

#if defined(__cplusplus)
}
#endif

#endif //OTA_ADAPTOR_H
