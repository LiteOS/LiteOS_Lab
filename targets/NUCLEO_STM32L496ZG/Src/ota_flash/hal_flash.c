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

#include <string.h>
#include "hal_flash.h"
#include "stm32l4xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_flash.h"
#include "stm32l4xx_hal_flash_ex.h"

#ifdef HAL_FLASH_MODULE_ENABLED

#define FLASH_PAGE_SIZE_MASK        (FLASH_PAGE_SIZE - 1)

int hal_flash_read(void* buf, int32_t len, uint32_t location)
{
    memcpy(buf, (uint8_t*)location, len);
    return 0;
}

static uint32_t __get_bank(uint32_t addr)
{
    uint32_t bank = 0;

    if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0)
    {
        /* No Bank swap */
        if (addr < (FLASH_BASE + FLASH_BANK_SIZE))
        {
            bank = FLASH_BANK_1;
        }
        else
        {
            bank = FLASH_BANK_2;
        }
    }
    else
    {
        /* Bank swap */
        if (addr < (FLASH_BASE + FLASH_BANK_SIZE))
        {
            bank = FLASH_BANK_2;
        }
        else
        {
            bank = FLASH_BANK_1;
        }
    }

    return bank;
}

static uint32_t __get_page(uint32_t addr)
{
    uint32_t page = 0;

    if (addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
        /* Bank 1 */
        page = (addr - FLASH_BASE) / FLASH_PAGE_SIZE;
    }
    else
    {
        /* Bank 2 */
        page = (addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
    }

    return page;
}

static int hal_flash_erase_one_page (uint32_t addr)
{
    uint32_t PageError = 0;
    HAL_StatusTypeDef status;
    FLASH_EraseInitTypeDef eraseinitstruct;

    HAL_FLASH_Unlock ();

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR);

    eraseinitstruct.Banks = __get_bank(addr);
    eraseinitstruct.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseinitstruct.Page = __get_page(addr);
    eraseinitstruct.NbPages = 1;
    status = HAL_FLASHEx_Erase(&eraseinitstruct, &PageError);

    HAL_FLASH_Lock ();

    if (status != HAL_OK)
    {
        return -1;
    }

    return 0;
}

int hal_flash_erase(uint32_t addr, int32_t len)
{
    int32_t i;

    if (((addr & FLASH_PAGE_SIZE_MASK) != 0) || (len < 0) || ((len & FLASH_PAGE_SIZE_MASK) != 0))
    {
        return -1;
    }

    for (i = 0; i < len / (int32_t) FLASH_PAGE_SIZE; i++)
    {
        if (hal_flash_erase_one_page(addr) != 0)
        {
            return -1;
        }

        addr += FLASH_PAGE_SIZE;
    }

    return 0;
}

int hal_flash_write(const void *buf, int32_t len, uint32_t* location)
{
    int32_t i = 0;
    char *dst = (char *)*location;
    const char *src = (const char *)buf;
    int ret = 0;

    if ((((uint32_t)src | len) & 7) != 0)
    {
        return -1;
    }

    HAL_FLASH_Unlock ();

    /* Clear OPTVERR bit set on virgin samples */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR);

    for(i = 0; i < len; i += 8)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)(dst+i), *(const uint64_t*)(src+i)) != HAL_OK ||
            *(const uint64_t *)(src + i) != *(const uint64_t*)(dst+i))
        {
            ret = -1;
            break;
        }
    }

    (void)HAL_FLASH_Lock();

    return ret;
}

#endif /* HAL_FLASH_MODULE_ENABLED */
