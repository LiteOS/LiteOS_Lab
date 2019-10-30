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

#include <config.h>

#include <hal_uart.h>
#include <hal_int.h>
#include <init.h>
#include <defer.h>

#include <arch/sync.h>

/* macros */

#define INTENT_RXDRDY       (1u << 2)
#define INTENT_TXDRDY       (1u << 7)

/* typedefs */

struct nrf_uart_regset
    {
    volatile uint32_t tasks_startrx;
    volatile uint32_t tasks_stoprx;
    volatile uint32_t tasks_starttx;
    volatile uint32_t tasks_stoptx;
    volatile uint32_t pad0 [3];
    volatile uint32_t tasks_suspend;
    volatile uint32_t pad1 [56];
    volatile uint32_t events_cts;
    volatile uint32_t events_ncts;
    volatile uint32_t events_rxdrdy;
    volatile uint32_t pad2 [4];
    volatile uint32_t events_txdrdy;
    volatile uint32_t pad3;
    volatile uint32_t events_error;
    volatile uint32_t pad4 [7];
    volatile uint32_t events_rxto;
    volatile uint32_t pad5 [46];
    volatile uint32_t shorts;
    volatile uint32_t pad6 [64];
    volatile uint32_t intenset;
    volatile uint32_t intenclr;
    volatile uint32_t pad7 [93];
    volatile uint32_t errorsrc;
    volatile uint32_t pad8 [31];
    volatile uint32_t enable;
    volatile uint32_t pad9;
    volatile uint32_t pselrts;
    volatile uint32_t pseltxd;
    volatile uint32_t pselcts;
    volatile uint32_t pselrxd;
    volatile uint32_t rxd;
    volatile uint32_t txd;
    volatile uint32_t pad10;
    volatile uint32_t baudrate;
    volatile uint32_t pad11 [17];
    volatile uint32_t config;
    volatile uint32_t pad12 [675];
    volatile uint32_t power;
    };

struct nrf_uart
    {
    hal_uart_t               hal_uart;
    struct nrf_uart_regset * regset;
    deferred_job_t           job;
    /* more members if needed */
    };

/* locals */

#ifdef CONFIG_NRF_UART0
static struct nrf_uart nrf_uart0;
#endif

#ifdef CONFIG_NRF_UART1
static struct nrf_uart nrf_uart1;
#endif

static struct
    {
    volatile uint32_t pad0 [321];
    volatile uint32_t out;
    volatile uint32_t outset;
    volatile uint32_t outclr;
    volatile uint32_t in;
    volatile uint32_t dir;
    volatile uint32_t dirset;
    volatile uint32_t dirclr;
    volatile uint32_t pad1 [120];
    volatile uint32_t pin_cnf [32];
    } * const nrf_gpio = (void *) 0x50000000;

static void __nrf_handler (uintptr_t arg)
    {
    struct nrf_uart        * nrf_uart = (struct nrf_uart *) arg;
    struct nrf_uart_regset * regset   = nrf_uart->regset;
    hal_uart_t             * uart     = &nrf_uart->hal_uart;

    while (regset->events_rxdrdy != 0)
        {
        regset->events_rxdrdy = 0;

        hal_rx_putc (uart, (unsigned char) regset->rxd);
        }

    regset->intenset = INTENT_RXDRDY;

    while (regset->events_txdrdy != 0)
        {
        unsigned char ch;

        if (hal_tx_getc (uart, &ch) == 0)
            {
            return;
            }

        regset->events_txdrdy = 0;
        regset->txd           = ch;
        }

    regset->intenset = INTENT_TXDRDY;
    }

static void nrf_uart_handler (uintptr_t arg)
    {
    struct nrf_uart        * nrf_uart = (struct nrf_uart *) arg;
    struct nrf_uart_regset * regset   = nrf_uart->regset;

    deferred_job_sched (&nrf_uart->job);

    regset->intenclr = INTENT_RXDRDY | INTENT_TXDRDY;
    }

static int nrf_uart_ioctl (hal_uart_t * uart, int cmd, va_list valist)
    {
    return -1;  // TODO:
    }

static int nrf_uart_poll_getc (hal_uart_t * uart)
    {
    struct nrf_uart        * nrf_uart = container_of (uart, struct nrf_uart, hal_uart);
    struct nrf_uart_regset * regset   = nrf_uart->regset;

    while (regset->events_rxdrdy == 0);

    regset->events_rxdrdy = 0;

    return (int) regset->rxd;
    }

static size_t nrf_uart_poll_putc (hal_uart_t * uart, unsigned char ch)
    {
    struct nrf_uart        * nrf_uart = container_of (uart, struct nrf_uart, hal_uart);
    struct nrf_uart_regset * regset   = nrf_uart->regset;

    regset->events_txdrdy = 0;

    regset->txd = ch;

    while (regset->events_txdrdy == 0);

    return 1;
    }

int early_putc (unsigned char ch)
    {
    struct nrf_uart_regset * regset = (struct nrf_uart_regset *) CONFIG_NRF_UART0_REGBASE;

    regset->events_txdrdy = 0;

    regset->txd = ch;

    while (regset->events_txdrdy == 0);

    return 1;
    }

static int nrf_uart_tx_start (hal_uart_t * uart)
    {
    container_of (uart, struct nrf_uart, hal_uart)->regset->intenset = INTENT_TXDRDY;

    return 0;
    }

static int __nrf_uart_init (struct nrf_uart * nrf_uart, uintptr_t base,
                            const char * name, uint32_t tx_pin, uint32_t rx_pin,
                            unsigned int irqn)
    {
    struct nrf_uart_regset * regset = (struct nrf_uart_regset *) base;
    hal_uart_t             * uart   = &nrf_uart->hal_uart;

    static const hal_uart_methods_t nrf_uart_methods =
        {
        nrf_uart_ioctl,
        nrf_uart_poll_getc,
        nrf_uart_poll_putc,
        nrf_uart_tx_start,
        };

    // TODO: gpio driver frame
    nrf_gpio->pin_cnf [tx_pin] = 1;
    nrf_gpio->pin_cnf [rx_pin] = 0;

    regset->pseltxd       = tx_pin;
    regset->pselrxd       = rx_pin;

    regset->baudrate      = 0x01d7e000;
    regset->enable        = 4;

    regset->tasks_startrx = 1;
    regset->tasks_starttx = 1;
    regset->events_rxdrdy = 0;
    regset->events_txdrdy = 0;
    regset->intenclr      = 0xffffffff;
    regset->intenset      = 1 << 2;      // enable rx int

    nrf_uart->regset      =  regset;

    deferred_job_init (&nrf_uart->job, __nrf_handler, (uintptr_t) nrf_uart);

    if (hal_int_connect (irqn, nrf_uart_handler, (uintptr_t) nrf_uart))
        {
        return -1;
        }

    hal_int_setprio (irqn, 3);

    hal_int_enable  (irqn);

    regset->txd = 0;

    return hal_uart_register (uart, name, &nrf_uart_methods, HAL_UART_MODE_INT);
    }

static int nrf_uart_init (void)
    {
    int ret;

#ifdef CONFIG_NRF_UART0
    ret = __nrf_uart_init (&nrf_uart0, CONFIG_NRF_UART0_REGBASE, "uart0",
                           CONFIG_NRF_UART0_TX_PIN, CONFIG_NRF_UART0_RX_PIN,
                           CONFIG_NRF_UART0_IRQN);

    if (ret != 0)
        {
        return ret;
        }
#endif

#ifdef CONFIG_NRF_UART1
    ret = __nrf_uart_init (&nrf_uart1, CONFIG_NRF_UART1_REGBASE, "uart1",
                           CONFIG_NRF_UART1_TX_PIN, CONFIG_NRF_UART1_RX_PIN,
                           CONFIG_NRF_UART1_IRQN);

    if (ret != 0)
        {
        return ret;
        }
#endif

    return 0;
    }

MODULE_INIT (driver, nrf_uart_init);

