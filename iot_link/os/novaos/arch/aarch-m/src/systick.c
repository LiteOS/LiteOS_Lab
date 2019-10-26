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
#include <common.h>
#include <irq.h>
#include <hal_timer.h>
#include <init.h>

#ifdef CONFIG_PROFILE
#include <profile.h>
#endif

#include <arch/config.h>
#include <arch/exc.h>

/* macros */

#define SYST_MAX_COUNT      0xffffff

/* SYST_CSR SysTick Control and Status Register */

#define SYST_CSR            (*(volatile uint32_t *) 0xe000e010)

#define SYST_CSR_ENABLE     (1 << 0)
#define SYST_CSR_TICKINT    (1 << 1)
#define SYST_CSR_CLKSOURCE  (1 << 2)
#define SYST_CSR_COUNTFLAG  (1 << 16)

/* SYST_RVR SysTick Reload Value Register */

#define SYST_RVR            (*(volatile uint32_t *) 0xe000e014)

/* SYST_CVR SysTick Current Value Register */

#define SYST_CVR            (*(volatile uint32_t *) 0xe000e018)
#define SYST_CALIB          (*(volatile uint32_t *) 0xe000e01c)

#define SYST_IRQ            15

/* locals */

static unsigned long        __reload_count = 0;

static int systick_enable (hal_timer_t * this, unsigned long count)
    {
    __reload_count = count;

    SYST_RVR = count;
    SYST_CVR = 0;
    SYST_CSR = 7;

    return 0;
    }

static int systick_disable (hal_timer_t * this)
    {
    SYST_CSR = 0;

    return 0;
    }

static unsigned long systick_counter (hal_timer_t * this)
    {
    return __reload_count - (unsigned long) SYST_CVR;
    }

static unsigned long systick_period (hal_timer_t * this)
    {
    return __reload_count;
    }

static const hal_timer_methods_t systick_methods =
    {
    .enable    = systick_enable,
    .disable   = systick_disable,
    .postpone  = NULL,
    .prepone   = NULL,
    .counter   = systick_counter,
    .period    = systick_period
    };

static hal_timer_t systick_timer =
    {
    .name      = "systick",
    .freq      = CONFIG_SYSTEM_CORE_CLOCK,
    .max_count = SYST_MAX_COUNT,
    .methods   = &systick_methods
    };

/**
 * systick_init - systick timer initialization routine
 *
 * return: 0 on success, negtive value on error
 */

static int systick_init (void)
    {

    /* hal_timer_register must not fail */

    return hal_timer_register (&systick_timer);
    }

MODULE_INIT (driver, systick_init);

/**
 * systick_handler - SysTick handler, override the weak one
 *
 * refer to handlers.c
 *
 * return: NA
 */

void systick_handler (void)
    {
#ifdef CONFIG_PROFILE
    profile_sys_enter ();
#endif

    if (systick_timer.handler != NULL)
        {
        systick_timer.handler (systick_timer.arg);
        }

    if (systick_timer.mode == HAL_TIMER_MODE_ONE_SHOT)
        {
        SYST_CSR = 0;
        }
    else
        {
        (void) SYST_CSR;    /* reading CSR clears event */
        }

#ifdef CONFIG_PROFILE
    profile_sys_exit ();
#endif
    }
