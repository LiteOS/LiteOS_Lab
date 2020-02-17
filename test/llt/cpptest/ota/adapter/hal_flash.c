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

#if 0
#include "stm32f4xx.h"
#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_flash_ex.h"

//#ifdef HAL_FLASH_MODULE_ENABLED
#endif

//static char flash[FLASH_SECTOR_ILEGAL] = {0};
char mem_flash[FLASH_SECTOR_ILEGAL] = {0};


#define ADDR_FLASH_SECTOR_0      ((uint32_t)0x00000000) /* Base address of Sector 0, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_1      ((uint32_t)0x00004000) /* Base address of Sector 1, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_2      ((uint32_t)0x00008000) /* Base address of Sector 2, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_3      ((uint32_t)0x0000C000) /* Base address of Sector 3, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_4      ((uint32_t)0x00010000) /* Base address of Sector 4, 64 Kbytes   */
#define ADDR_FLASH_SECTOR_5      ((uint32_t)0x00020000) /* Base address of Sector 5, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_6      ((uint32_t)0x00040000) /* Base address of Sector 6, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_7      ((uint32_t)0x00060000) /* Base address of Sector 7, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_8      ((uint32_t)0x00080000) /* Base address of Sector 8, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_9      ((uint32_t)0x000A0000) /* Base address of Sector 9, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_10     ((uint32_t)0x000C0000) /* Base address of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11     ((uint32_t)0x000E0000) /* Base address of Sector 11, 128 Kbytes */
#define ADDR_FLASH_SECTOR_12     ((uint32_t)0x00100000) /* Base address of Sector 12, 16 Kbytes  */
#define ADDR_FLASH_SECTOR_13     ((uint32_t)0x00104000) /* Base address of Sector 13, 16 Kbytes  */
#define ADDR_FLASH_SECTOR_14     ((uint32_t)0x00108000) /* Base address of Sector 14, 16 Kbytes  */
#define ADDR_FLASH_SECTOR_15     ((uint32_t)0x0010C000) /* Base address of Sector 15, 16 Kbytes  */
#define ADDR_FLASH_SECTOR_16     ((uint32_t)0x00110000) /* Base address of Sector 16, 64 Kbytes  */
#define ADDR_FLASH_SECTOR_17     ((uint32_t)0x00120000) /* Base address of Sector 17, 128 Kbytes */
#define ADDR_FLASH_SECTOR_18     ((uint32_t)0x00140000) /* Base address of Sector 18, 128 Kbytes */
#define ADDR_FLASH_SECTOR_19     ((uint32_t)0x00160000) /* Base address of Sector 19, 128 Kbytes */
#define ADDR_FLASH_SECTOR_20     ((uint32_t)0x00180000) /* Base address of Sector 20, 128 Kbytes */
#define ADDR_FLASH_SECTOR_21     ((uint32_t)0x001A0000) /* Base address of Sector 21, 128 Kbytes */
#define ADDR_FLASH_SECTOR_22     ((uint32_t)0x001C0000) /* Base address of Sector 22, 128 Kbytes */
#define ADDR_FLASH_SECTOR_23     ((uint32_t)0x001E0000) /* Base address of Sector 23, 128 Kbytes */
#define ADDR_FLASH_SECTOR_END    ((uint32_t)0x00200000) /* End address of Sector 23 */


#define FLASH_SECTOR_0     0U  /*!< Sector Number 0   */
#define FLASH_SECTOR_1     1U  /*!< Sector Number 1   */
#define FLASH_SECTOR_2     2U  /*!< Sector Number 2   */
#define FLASH_SECTOR_3     3U  /*!< Sector Number 3   */
#define FLASH_SECTOR_4     4U  /*!< Sector Number 4   */
#define FLASH_SECTOR_5     5U  /*!< Sector Number 5   */
#define FLASH_SECTOR_6     6U  /*!< Sector Number 6   */
#define FLASH_SECTOR_7     7U  /*!< Sector Number 7   */
#define FLASH_SECTOR_8     8U  /*!< Sector Number 8   */
#define FLASH_SECTOR_9     9U  /*!< Sector Number 9   */
#define FLASH_SECTOR_10    10U /*!< Sector Number 10  */
#define FLASH_SECTOR_11    11U /*!< Sector Number 11  */
#define FLASH_SECTOR_12    12U /*!< Sector Number 12  */
#define FLASH_SECTOR_13    13U /*!< Sector Number 13  */
#define FLASH_SECTOR_14    14U /*!< Sector Number 14  */
#define FLASH_SECTOR_15    15U /*!< Sector Number 15  */
#define FLASH_SECTOR_16    16U /*!< Sector Number 16  */
#define FLASH_SECTOR_17    17U /*!< Sector Number 17  */
#define FLASH_SECTOR_18    18U /*!< Sector Number 18  */
#define FLASH_SECTOR_19    19U /*!< Sector Number 19  */
#define FLASH_SECTOR_20    20U /*!< Sector Number 20  */
#define FLASH_SECTOR_21    21U /*!< Sector Number 21  */
#define FLASH_SECTOR_22    22U /*!< Sector Number 22  */
#define FLASH_SECTOR_23    23U /*!< Sector Number 23  */

typedef struct __sector_map {
    uint32_t offset_addr;
    uint32_t sector_size;
} sector_map_t;


sector_map_t sector_map[24] = {
    {ADDR_FLASH_SECTOR_0, 16},  {ADDR_FLASH_SECTOR_1, 16},
    {ADDR_FLASH_SECTOR_2, 16},  {ADDR_FLASH_SECTOR_3, 16},
    {ADDR_FLASH_SECTOR_4, 64},  {ADDR_FLASH_SECTOR_5, 128},
    {ADDR_FLASH_SECTOR_6, 128},  {ADDR_FLASH_SECTOR_7, 128},
    {ADDR_FLASH_SECTOR_8, 128},  {ADDR_FLASH_SECTOR_9, 128},
    {ADDR_FLASH_SECTOR_10, 128},  {ADDR_FLASH_SECTOR_11, 128},
    {ADDR_FLASH_SECTOR_12, 16},  {ADDR_FLASH_SECTOR_13, 16},
    {ADDR_FLASH_SECTOR_14, 16},  {ADDR_FLASH_SECTOR_15, 16},
    {ADDR_FLASH_SECTOR_16, 64},  {ADDR_FLASH_SECTOR_17, 128},
    {ADDR_FLASH_SECTOR_18, 128},  {ADDR_FLASH_SECTOR_19, 128},
    {ADDR_FLASH_SECTOR_20, 128},  {ADDR_FLASH_SECTOR_21, 128},
    {ADDR_FLASH_SECTOR_22, 128},  {ADDR_FLASH_SECTOR_23, 128},
};


static uint32_t prv_flash_get_sector(uint32_t addr)
{
    uint32_t sector = 0;
    if ((addr < ADDR_FLASH_SECTOR_1) && (addr >= ADDR_FLASH_SECTOR_0))
    {
        sector = FLASH_SECTOR_0;
    }
    else if ((addr < ADDR_FLASH_SECTOR_2) && (addr >= ADDR_FLASH_SECTOR_1))
    {
        sector = FLASH_SECTOR_1;
    }
    else if ((addr < ADDR_FLASH_SECTOR_3) && (addr >= ADDR_FLASH_SECTOR_2))
    {
        sector = FLASH_SECTOR_2;
    }
    else if ((addr < ADDR_FLASH_SECTOR_4) && (addr >= ADDR_FLASH_SECTOR_3))
    {
        sector = FLASH_SECTOR_3;
    }
    else if ((addr < ADDR_FLASH_SECTOR_5) && (addr >= ADDR_FLASH_SECTOR_4))
    {
        sector = FLASH_SECTOR_4;
    }
    else if ((addr < ADDR_FLASH_SECTOR_6) && (addr >= ADDR_FLASH_SECTOR_5))
    {
        sector = FLASH_SECTOR_5;
    }
    else if ((addr < ADDR_FLASH_SECTOR_7) && (addr >= ADDR_FLASH_SECTOR_6))
    {
        sector = FLASH_SECTOR_6;
    }
    else if ((addr < ADDR_FLASH_SECTOR_8) && (addr >= ADDR_FLASH_SECTOR_7))
    {
        sector = FLASH_SECTOR_7;
    }
    else if ((addr < ADDR_FLASH_SECTOR_9) && (addr >= ADDR_FLASH_SECTOR_8))
    {
        sector = FLASH_SECTOR_8;
    }
    else if ((addr < ADDR_FLASH_SECTOR_10) && (addr >= ADDR_FLASH_SECTOR_9))
    {
        sector = FLASH_SECTOR_9;
    }
    else if ((addr < ADDR_FLASH_SECTOR_11) && (addr >= ADDR_FLASH_SECTOR_10))
    {
        sector = FLASH_SECTOR_10;
    }
    else if ((addr < ADDR_FLASH_SECTOR_12) && (addr >= ADDR_FLASH_SECTOR_11))
    {
        sector = FLASH_SECTOR_11;
    }

    else if ((addr < ADDR_FLASH_SECTOR_13) && (addr >= ADDR_FLASH_SECTOR_12))
    {
        sector = FLASH_SECTOR_12;
    }
    else if ((addr < ADDR_FLASH_SECTOR_14) && (addr >= ADDR_FLASH_SECTOR_13))
    {
        sector = FLASH_SECTOR_13;
    }
    else if ((addr < ADDR_FLASH_SECTOR_15) && (addr >= ADDR_FLASH_SECTOR_14))
    {
        sector = FLASH_SECTOR_14;
    }
    else if ((addr < ADDR_FLASH_SECTOR_16) && (addr >= ADDR_FLASH_SECTOR_15))
    {
        sector = FLASH_SECTOR_15;
    }
    else if ((addr < ADDR_FLASH_SECTOR_17) && (addr >= ADDR_FLASH_SECTOR_16))
    {
        sector = FLASH_SECTOR_16;
    }
    else if ((addr < ADDR_FLASH_SECTOR_18) && (addr >= ADDR_FLASH_SECTOR_17))
    {
        sector = FLASH_SECTOR_17;
    }
    else if ((addr < ADDR_FLASH_SECTOR_19) && (addr >= ADDR_FLASH_SECTOR_18))
    {
        sector = FLASH_SECTOR_18;
    }
    else if ((addr < ADDR_FLASH_SECTOR_20) && (addr >= ADDR_FLASH_SECTOR_19))
    {
        sector = FLASH_SECTOR_19;
    }
    else if ((addr < ADDR_FLASH_SECTOR_21) && (addr >= ADDR_FLASH_SECTOR_20))
    {
        sector = FLASH_SECTOR_20;
    }
    else if ((addr < ADDR_FLASH_SECTOR_22) && (addr >= ADDR_FLASH_SECTOR_21))
    {
        sector = FLASH_SECTOR_21;
    }
    else if ((addr < ADDR_FLASH_SECTOR_23) && (addr >= ADDR_FLASH_SECTOR_22))
    {
        sector = FLASH_SECTOR_22;
    }
    else if ((addr < ADDR_FLASH_SECTOR_END) && (addr >= ADDR_FLASH_SECTOR_23))
    {
        sector = FLASH_SECTOR_23;
    }
    else
    {
        sector = FLASH_SECTOR_ILEGAL;
    }

    return sector;
}

int hal_flash_read(void* buf, int32_t len, uint32_t location)
{
    if (NULL == buf
        || len < 0
        || len >= ADDR_FLASH_SECTOR_END - ADDR_FLASH_SECTOR_0)
    {
        return -1;
    }

    if (FLASH_SECTOR_ILEGAL != prv_flash_get_sector(location)
        && FLASH_SECTOR_ILEGAL != prv_flash_get_sector(location + len))
    {
        memcpy(buf, (uint8_t*)location, len);
        return 0;
    }
    else
    {
        return -1;
    }

}

void FLASH_Erase_Sector(uint32_t sector)
{
    memset(mem_flash + sector_map[sector].offset_addr, 0xff, sector_map[sector].sector_size << 10);
}

int hal_flash_erase(uint32_t addr, int32_t len)
{
    uint32_t begin_sector;
    uint32_t end_sector;
    uint32_t i;

    if (len < 0 || len >= ADDR_FLASH_SECTOR_END - ADDR_FLASH_SECTOR_0)
    {
        return -1;
    }
/*
    if (HAL_FLASH_Unlock() != HAL_OK)
    {
        return -1;
    }
*/
    begin_sector = prv_flash_get_sector(addr);
    end_sector = prv_flash_get_sector(addr + len);

    if (FLASH_SECTOR_ILEGAL == begin_sector
        || FLASH_SECTOR_ILEGAL == end_sector)
    {
        return -1;
    }

//    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
//                           FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    for (i = begin_sector; i <= end_sector; ++i)
    {
        FLASH_Erase_Sector(i);
    }

    return 0;

}

int HAL_FLASH_Program(uint32_t TypeProgram, uint32_t FlashAddress, uint32_t DataAddress)
{
    mem_flash[FlashAddress] = DataAddress;
    return 0;
}

int hal_flash_write(const void* buf, int32_t len, uint32_t* location)
{
    int i;
    uint8_t* pbuf;
    uint32_t location_cur;

    if (NULL == buf
        || NULL == location
        || len < 0
        || len >= ADDR_FLASH_SECTOR_END - ADDR_FLASH_SECTOR_0)
    {
        return -1;
    }

    location_cur = *location;
    pbuf = (uint8_t*)buf;

    if (FLASH_SECTOR_ILEGAL == prv_flash_get_sector(location_cur)
        || FLASH_SECTOR_ILEGAL == prv_flash_get_sector(location_cur + len))
    {
        return -1;
    }

    for (i = 0; i < len; ++i)
    {
        if (HAL_FLASH_Program(0,
                              location_cur + i, pbuf[i]) != 0)
        {
            return -1;
        }
    }
    *location += len;

    return 0;
}

int hal_flash_erase_write(const void* buf, int32_t len, uint32_t location)
{
    if (NULL == buf)
    {
        return -1;
    }

    if (hal_flash_erase(location, len) != 0)
    {
//        (void)HAL_FLASH_Lock();
        return -1;
    }

    if (hal_flash_write(buf, len, &location) != 0)
    {
//        (void)HAL_FLASH_Lock();
        return -1;
    }

    return 0;
}

void hal_flash_lock(void)
{
//    (void)HAL_FLASH_Lock();
}

//#endif /* HAL_FLASH_MODULE_ENABLED */
