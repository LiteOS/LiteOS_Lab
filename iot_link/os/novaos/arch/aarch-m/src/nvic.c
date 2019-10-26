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

#include <common.h>
#include <init.h>
#include <hal_int.h>
#include <bug.h>

#include <arch/sync.h>              /* for dsb, isb */
#include <arch/config.h>

/* macros */

#define NVIC_PRIO_BITS              CONFIG_NVIC_PRIO_BITS

static struct
    {
    volatile uint32_t iser [16];    /* offset 0x000 */
             uint32_t pad0 [16];
    volatile uint32_t icer [16];    /* offset 0x080 */
             uint32_t pad1 [16];
    volatile uint32_t ispr [16];    /* offset 0x100 */
             uint32_t pad2 [16];
    volatile uint32_t icpr [16];    /* offset 0x180 */
             uint32_t pad3 [24];
    volatile uint32_t ibar [16];    /* offset 0x200 */
             uint32_t pad4 [48];
    volatile uint8_t  ipr  [496];   /* offset 0x300 */
             uint32_t pad5 [580];
    volatile uint32_t stir;         /* offset 0xe00 */
    } * const nvic = (void *) 0xe000e100;

static int nvic_setprio (unsigned int irq, unsigned int prio)
    {
    nvic->ipr [irq] = (uint8_t) ((prio << (8u - NVIC_PRIO_BITS)) & (uint32_t) 0xffu);

    return 0;
    }

static int nvic_enable (unsigned int irq)
    {
    nvic->iser [irq >> 5] = (1 << (irq & 0x1f));

    return 0;
    }

static int nvic_disable (unsigned int irq)
    {
    nvic->icer [irq >> 5] = (1 << (irq & 0x1f));

    dsb ();
    isb ();

    return 0;
    }

/**
 * nvic_init - nvic initialization routine
 *
 * return: 0 on success, negtive value on error
 */

static int nvic_init (void)
    {
    static const hal_int_methods_t nvic_methods =
        {
        .enable  = nvic_enable,
        .disable = nvic_disable,
        .setprio = nvic_setprio
        };

    BUG_ON (hal_int_register (&nvic_methods) != 0, "Fail to register NVIC!");

    return 0;
    }

MODULE_INIT (driver, nvic_init);

