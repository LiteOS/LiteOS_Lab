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

#include <hal_int.h>
#include <hal_timer.h>
#include <init.h>

#define NRF_RTC_MAX_COUNT       0xffffff

struct nrf_rtc_regset
    {
    volatile uint32_t tasks_start;
    volatile uint32_t tasks_stop;
    volatile uint32_t tasks_clear;
    volatile uint32_t tasks_trigovrflw;
    volatile uint32_t pad0 [60];
    volatile uint32_t events_tick;
    volatile uint32_t events_ovrflw;
    volatile uint32_t pad1 [14];
    volatile uint32_t events_compare [4];
    volatile uint32_t pad2 [109];
    volatile uint32_t intenset;
    volatile uint32_t intenclr;
    volatile uint32_t pad3 [13];
    volatile uint32_t evten;
    volatile uint32_t evtenset;
    volatile uint32_t evtenclr;
    volatile uint32_t pad4 [110];
    volatile uint32_t counter;
    volatile uint32_t prescaler;
    volatile uint32_t pad5 [13];
    volatile uint32_t cc [4];
    volatile uint32_t pad6 [683];
    volatile uint32_t power;
    };

struct nrf_rtc
    {
    hal_timer_t             hal_timer;
    struct nrf_rtc_regset * regset;
    unsigned int            irqn;
    unsigned long           step;
    /* more members if needed */
    };

/* locals */

#ifdef CONFIG_NRF_RTC0
struct nrf_rtc nrf_rtc0;
#endif

#ifdef CONFIG_NRF_RTC1
struct nrf_rtc nrf_rtc1;
#endif

/* TODO: clock driver */

static struct
    {
    volatile uint32_t tasks_hfclkstart;
    volatile uint32_t tasks_hfclkstop;
    volatile uint32_t tasks_lfclkstart;
    volatile uint32_t tasks_lfclkstop;
    volatile uint32_t tasks_cal;
    volatile uint32_t tasks_ctstart;
    volatile uint32_t tasks_ctstop;
    volatile uint32_t pad0 [57];
    volatile uint32_t events_hfclkstarted;
    volatile uint32_t events_lfclkstarted;
    volatile uint32_t pad1;
    volatile uint32_t events_done;
    volatile uint32_t events_ctto;
    volatile uint32_t pad2 [124];
    volatile uint32_t intenset;
    volatile uint32_t intenclr;
    volatile uint32_t pad3 [63];
    volatile uint32_t hfclkrun;
    volatile uint32_t hfclkstat;
    volatile uint32_t pad4 [1];
    volatile uint32_t lfclkrun;
    volatile uint32_t lfclkstat;
    volatile uint32_t lfclksrccopy;

    volatile uint32_t pad5[62];
    volatile uint32_t lfclksrc;
    volatile uint32_t pad6 [7];
    volatile uint32_t ctiv;
    volatile uint32_t pad7 [5];
    volatile uint32_t xtalfreq;
    } * const nrf_clock = (void *) 0x40000000;

static int rtc_enable (hal_timer_t * timer, unsigned long count)
    {
    struct nrf_rtc        * nrf_rtc = container_of (timer, struct nrf_rtc, hal_timer);
    struct nrf_rtc_regset * regset  = nrf_rtc->regset;

    nrf_rtc->step    = count;

    regset->cc [0]   = count;

    regset->evtenset = 1 << 16;  /* compare0 event enable */
    regset->intenset = 1 << 16;  /* compare0 int enable */

    hal_int_setprio (nrf_rtc->irqn, 3);

    hal_int_enable (nrf_rtc->irqn);

    regset->tasks_clear = 1;

    regset->tasks_start = 1;

    return 0;
    }

static int rtc_disable (hal_timer_t * timer)
    {
    struct nrf_rtc        * nrf_rtc = container_of (timer, struct nrf_rtc, hal_timer);
    struct nrf_rtc_regset * regset  = nrf_rtc->regset;

    regset->tasks_stop = 1;

    regset->evtenclr = 1 << 16;  /* compare0 event clear */
    regset->intenclr = 1 << 16;  /* compare0 int clear */

    hal_int_disable (nrf_rtc->irqn);

    return 0;
    }

static void rtc_postpone (hal_timer_t * timer, unsigned long count)
    {
    struct nrf_rtc        * nrf_rtc = container_of (timer, struct nrf_rtc, hal_timer);
    struct nrf_rtc_regset * regset  = nrf_rtc->regset;

    regset->cc [0] += count;
    // only usefull for debug
    regset->events_compare [0] = 0;  /* clear event */
    }

static void rtc_prepone (hal_timer_t * timer, unsigned long count)
    {
    struct nrf_rtc        * nrf_rtc = container_of (timer, struct nrf_rtc, hal_timer);
    struct nrf_rtc_regset * regset  = nrf_rtc->regset;

    regset->cc [0] -= count;
    }

static void rtc_handler (uintptr_t arg)
    {
    hal_timer_t           * timer   = (hal_timer_t *) arg;
    struct nrf_rtc        * nrf_rtc = container_of (timer, struct nrf_rtc, hal_timer);
    struct nrf_rtc_regset * regset  = nrf_rtc->regset;
    uint32_t                count, cc;

    if (!regset->events_compare [0])
        {
        return;
        }

    hal_timer_handler (timer);

    count = nrf_rtc->regset->counter;
    cc    = regset->cc [0];

    regset->events_compare [0] = 0;  /* clear event */

    if (timer->mode == HAL_TIMER_MODE_ONE_SHOT)
        {
        rtc_disable (timer);
        return;
        }

    if (unlikely (count - cc >= nrf_rtc->step))
        {
        regset->cc [0] = count + nrf_rtc->step;
        }
    else
        {
        regset->cc [0] = cc    + nrf_rtc->step;
        }
    }

static unsigned long rtc_counter (hal_timer_t * timer)
    {
    struct nrf_rtc * nrf_rtc = container_of (timer, struct nrf_rtc, hal_timer);

    return nrf_rtc->regset->counter;
    }

static unsigned long rtc_period (hal_timer_t * timer)
    {
    return NRF_RTC_MAX_COUNT;
    }

static const hal_timer_methods_t rtc_methods =
    {
    .enable        = rtc_enable,
    .disable       = rtc_disable,
    .postpone      = rtc_postpone,
    .prepone       = rtc_prepone,
    .counter       = rtc_counter,
    .period        = rtc_period
    };

static int __nrf_rtc_init (struct nrf_rtc * nrf_rtc, uintptr_t base,
                           unsigned int irqn, const char * name)
    {
    struct nrf_rtc_regset * regset    = (struct nrf_rtc_regset *) base;
    hal_timer_t           * hal_timer = &nrf_rtc->hal_timer;

    regset->prescaler    = 0;

    hal_timer->name      = name;
    hal_timer->freq      = 32768;
    hal_timer->max_count = NRF_RTC_MAX_COUNT;
    hal_timer->methods   = &rtc_methods;

    if (hal_int_connect (irqn, rtc_handler, (uintptr_t) nrf_rtc))
        {
        return -1;
        }

    nrf_rtc->irqn   = irqn;
	nrf_rtc->regset = regset;

    return hal_timer_register (&nrf_rtc->hal_timer);
    }

static int nrf_rtc_init (void)
    {
    int ret;

    nrf_clock->lfclksrc            = 1;         /* use xtal 32768hz */
    nrf_clock->events_lfclkstarted = 0;
    nrf_clock->tasks_lfclkstart    = 1;

    while (!nrf_clock->events_lfclkstarted);

    nrf_clock->events_lfclkstarted = 0;

#ifdef CONFIG_NRF_RTC0
    ret = __nrf_rtc_init (&nrf_rtc0, CONFIG_NRF_RTC0_REGBASE,
                          CONFIG_NRF_RTC0_IRQN, "rtc0");

    if (ret != 0)
        {
        return ret;
        }
#endif

#ifdef CONFIG_NRF_RTC1
    ret = __nrf_rtc_init (&nrf_rtc1, CONFIG_NRF_RTC1_REGBASE,
                          CONFIG_NRF_RTC1_IRQN, "rtc1");

    if (ret != 0)
        {
        return ret;
        }
#endif

    return 0;
    }

MODULE_INIT (driver, nrf_rtc_init);

