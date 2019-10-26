/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
 *
 * LiteOS NOVA is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *
 * 	http://license.coscl.org.cn/MulanPSL
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
 * FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v1 for more details.
 */

#include <stdint.h>

#include <config.h>
#include <init.h>

#define NRF_POWER_RAMON_ADDRESS             0x40000524  /* NRF_POWER->RAMON address             */
#define NRF_POWER_RAMONB_ADDRESS            0x40000554  /* NRF_POWER->RAMONB address            */
#define NRF_POWER_RAMONx_RAMxON_ONMODE_MSK  0x3         /* All RAM blocks on in onmode bit mask */

static int board_init (void)
    {
    volatile uint32_t * nrf_power_ramon  = (uint32_t *) NRF_POWER_RAMON_ADDRESS;
    volatile uint32_t * nrf_power_ramonb = (uint32_t *) NRF_POWER_RAMONB_ADDRESS;

    *nrf_power_ramon  |= NRF_POWER_RAMONx_RAMxON_ONMODE_MSK;
    *nrf_power_ramonb |= NRF_POWER_RAMONx_RAMxON_ONMODE_MSK;

    return 0;
    }

MODULE_INIT (board, board_init);

