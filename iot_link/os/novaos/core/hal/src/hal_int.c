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

#include <stddef.h>

#include <config.h>
#include <compiler.h>
#include <hal_int.h>
#include <errno.h>
#include <warn.h>
#include <bug.h>

/* globals */

unsigned int int_cnt = 0;

__section__ (stack) __unused char irq_stack [CONFIG_IRQ_STACK_SIZE];

/* statics */

static struct
    {
    hal_int_handler_t handler;
    uintptr_t         arg;
    } __irq_table [CONFIG_NR_IRQS];

static const hal_int_methods_t * __intc_methods = NULL;

/**
 * hal_int_dispatch - run interrupt routine for an irq
 * @irq: the irq number
 *
 * return: NA
 */

void hal_int_dispatch (unsigned int irq)
    {
    WARN_ON (irq >= CONFIG_NR_IRQS,
             errno = ERRNO_HAL_INTC_ILLEGAL_IRQN; return,
             "IRQn too large!");

    WARN_ON (__irq_table [irq].handler == NULL,
             return,
             "IRQ (%d) not connected!", irq);

    __irq_table [irq].handler (__irq_table [irq].arg);
    }

/**
 * hal_int_connect - connect a routine to a hardware interrupt
 * @irq:     the irq number to attach to
 * @handler: irq handler to be installed
 * @arg:     argument for the handler
 *
 * return: 0 on success, negtive value on error
 */

int hal_int_connect (unsigned int irq, hal_int_handler_t handler, uintptr_t arg)
    {
    WARN_ON (irq >= CONFIG_NR_IRQS,
             errno = ERRNO_HAL_INTC_ILLEGAL_IRQN; return -1,
             "IRQn too large!");

    WARN_ON (__irq_table [irq].handler != NULL,
             errno = ERRNO_HAL_INTC_ILLEGAL_OPERATION; return -1,
             "IRQ (%d) already connected!", irq);

    __irq_table [irq].arg     = arg;
    __irq_table [irq].handler = handler;

    return 0;
    }

/**
 * hal_int_disconnect - disconnect a routine to a hardware interrupt
 * @irq: the irq number to be attached
 *
 * return: 0 on success, negtive value on error
 */

int hal_int_disconnect (unsigned int irq)
    {
    WARN_ON (irq >= CONFIG_NR_IRQS,
             errno = ERRNO_HAL_INTC_ILLEGAL_IRQN; return -1,
             "IRQn too large!");

    __irq_table [irq].handler = NULL;

    return 0;
    }

/**
 * hal_int_setprio - set the priority of a specific irq
 * @irq:  the irq number
 * @prio: the priority of the irq
 *
 * return: 0 on success, negtive value on error
 */

int hal_int_setprio (unsigned int irq, unsigned int prio)
    {
    BUG_ON (__intc_methods == NULL, "No interrupt controller installed!");

    WARN_ON (__intc_methods->setprio == NULL,
             errno = ERRNO_HAL_INTC_ILLEGAL_OPERATION; return -1,
             "Interrupt controller has no <setprio> method!");

    return __intc_methods->setprio (irq, prio);
    }

/**
 * hal_int_enable - enable a specific irq
 * @irq:  the irq number to be enabled
 *
 * return: 0 on success, negtive value on error
 */

int hal_int_enable (unsigned int irq)
    {
    BUG_ON (__intc_methods == NULL, "No interrupt controller installed!");

    // TODO: devfs register

    return __intc_methods->enable (irq);
    }

/**
 * hal_int_disable - disable a specific irq
 * @irq: the irq number to be disabled
 *
 * return: 0 on success, negtive value on error
 */

int hal_int_disable (unsigned int irq)
    {
    BUG_ON (__intc_methods == NULL, "No interrupt controller installed!");

    WARN_ON (__intc_methods->disable == NULL,
             errno = ERRNO_HAL_INTC_ILLEGAL_OPERATION; return -1,
             "Interrupt controller has no <setprio> disable!");

    return __intc_methods->disable (irq);
    }

/**
 * hal_int_handler - interrupt handler
 * @irq: the irq number to be disabled
 *
 * return: 0 on success, negtive value on error
 */

void hal_int_handler (void)
    {
    BUG_ON (__intc_methods == NULL, "No interrupt controller installed!");

    if (unlikely (__intc_methods->handler == NULL))
        {
        return;
        }

    __intc_methods->handler ();
    }

/**
 * hal_int_register - register an interrupt controler
 * @methods:   the interrupt controler methods
 *
 * return: 0 on success, negtive value on error
 */

int hal_int_register (const hal_int_methods_t * methods)
    {
    BUG_ON (methods == NULL || methods->enable == NULL, "Invalid interrupt methods!");

    __intc_methods = methods;

    // TODO: devfs register

    return 0;
    }

