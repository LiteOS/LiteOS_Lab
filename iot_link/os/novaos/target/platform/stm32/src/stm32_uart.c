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

#include <kconfig.h>

#include CONFIG_STM32_HAL_H

#include <config.h>
#include <hal_uart.h>
#include <hal_int.h>
#include <init.h>
#include <warn.h>
#include <defer.h>
#include <symn2c.h>

/* macros */

/* typedefs */

struct stm32_uart
    {
    hal_uart_t         hal_uart;
    UART_HandleTypeDef handle;
    deferred_job_t     rxjob;
    deferred_job_t     txjob;
    /* more members if needed */
    };

/* locals */

#ifdef CONFIG_STM32_UART1
static struct stm32_uart stm32_uart1;
#endif

#ifdef CONFIG_STM32_UART2
static struct stm32_uart stm32_uart2;
#endif

#ifdef CONFIG_STM32_UART3
static struct stm32_uart stm32_uart3;
#endif

#ifdef CONFIG_STM32_LPUART1
static struct stm32_uart stm32_lpuart1;
#endif

static void __stm32_deferred_rx (uintptr_t arg)
    {
    struct stm32_uart  * uart   = (struct stm32_uart *) arg;
    UART_HandleTypeDef * handle = &uart->handle;

    while (__HAL_UART_GET_FLAG (handle, UART_FLAG_RXNE) != RESET)
        {
#ifdef CONFIG_STM32_UART_USING_DR
        unsigned char ch = (uint8_t) (handle->Instance->DR  & (uint8_t) 0x00ff);
#else
        unsigned char ch = (uint8_t) (handle->Instance->RDR & (uint8_t) 0x00ff);
#endif

        hal_rx_putc (&uart->hal_uart, ch);
        }

    /* re-enable RX interrupt */

    __HAL_UART_ENABLE_IT (handle, UART_IT_RXNE);
    }

static void __stm32_deferred_tx (uintptr_t arg)
    {
    struct stm32_uart  * uart   = (struct stm32_uart *) arg;
    UART_HandleTypeDef * handle = &uart->handle;

    while (__HAL_UART_GET_FLAG (handle, UART_FLAG_TXE)  != RESET)
        {
        unsigned char ch;

        if (hal_tx_getc (&uart->hal_uart, &ch) == 0)
            {
            return;
            }
#ifdef  CONFIG_STM32_UART_USING_DR
        handle->Instance->DR  = ch;
#else
        handle->Instance->TDR = ch;
#endif
        }

    /* re-enable TX interrupt as there maybe more bytes in the ring */

    __HAL_UART_ENABLE_IT (handle, UART_IT_TXE);
    }

static void __stm32_uart_handler (uintptr_t arg)
    {
    struct stm32_uart * uart = (struct stm32_uart *) arg;

    if (__HAL_UART_GET_IT_SOURCE (&uart->handle, UART_IT_RXNE) != RESET)
        {
        __HAL_UART_DISABLE_IT (&uart->handle, UART_IT_RXNE);
        deferred_job_sched (&uart->rxjob);
        }

    if (__HAL_UART_GET_IT_SOURCE (&uart->handle, UART_IT_TXE)  != RESET)
        {
        __HAL_UART_DISABLE_IT (&uart->handle, UART_IT_TXE);
        deferred_job_sched (&uart->txjob);
        }
    }

static int __stm32_uart_ioctl (hal_uart_t * uart, int cmd, va_list valist)
    {
    return -1;  // TODO:
    }

static int __stm32_uart_poll_getc (hal_uart_t * hal_uart)
    {
    uint8_t             ch   = 0;
    struct stm32_uart * uart = container_of (hal_uart, struct stm32_uart,
                                             hal_uart);

    (void) HAL_UART_Receive  (&uart->handle, (uint8_t *) &ch, 1, 0xffffffff);

    return ch;
    }

static size_t __stm32_uart_poll_putc (hal_uart_t * hal_uart, unsigned char ch)
    {
    struct stm32_uart * uart = container_of (hal_uart, struct stm32_uart,
                                             hal_uart);

    (void) HAL_UART_Transmit (&uart->handle, (uint8_t *) &ch, 1, 0xffffffff);

    return 1;
    }

static int __stm32_uart_tx_start (hal_uart_t * hal_uart)
    {
    struct stm32_uart * uart = container_of (hal_uart, struct stm32_uart,
                                             hal_uart);

    __HAL_UART_ENABLE_IT (&uart->handle, UART_IT_TXE);

    return 0;
    }

static const hal_uart_methods_t stm32_uart_methods =
    {
    __stm32_uart_ioctl,
    __stm32_uart_poll_getc,
    __stm32_uart_poll_putc,
    __stm32_uart_tx_start,
    };

static int __stm32_uart_chan_init (struct stm32_uart * uart, USART_TypeDef * base,
                                   uint32_t baud,
                                   GPIO_TypeDef * txgpio, uint32_t txpin,
                                   GPIO_TypeDef * rxgpio, uint32_t rxpin,
                                   uint32_t alter, bool lpuart)
    {
    UART_HandleTypeDef * handle = &uart->handle;
    GPIO_InitTypeDef     io;

    handle->Instance            = base;
    handle->Init.BaudRate       = baud;
    handle->Init.WordLength     = UART_WORDLENGTH_8B;
    handle->Init.StopBits       = UART_STOPBITS_1;
    handle->Init.Parity         = UART_PARITY_NONE;
    handle->Init.Mode           = UART_MODE_TX_RX;
    handle->Init.HwFlowCtl      = UART_HWCONTROL_NONE;
    handle->Init.OverSampling   = lpuart ? 0 :UART_OVERSAMPLING_16;

    if (HAL_UART_Init (handle) != HAL_OK)
        {
        WARN ("HAL_UART_Init fail!");
        return -1;
        }

    io.Pin       = 1 << txpin;
    io.Mode      = GPIO_MODE_AF_PP;
    io.Pull      = GPIO_PULLUP;
    io.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    io.Alternate = alter;
    HAL_GPIO_Init (txgpio, &io);

    io.Pin       = 1 << rxpin;
    io.Mode      = GPIO_MODE_AF_PP;
    io.Pull      = GPIO_PULLUP;
    io.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    io.Alternate = alter;
    HAL_GPIO_Init (rxgpio, &io);

    return 0;
    }

static int __stm32_uart_init (struct stm32_uart * uart,
                              unsigned int irqn, const char * name)
    {
    UART_HandleTypeDef * handle   = &uart->handle;
    hal_uart_t         * hal_uart = &uart->hal_uart;

    deferred_job_init (&uart->rxjob, __stm32_deferred_rx, (uintptr_t) uart);
    deferred_job_init (&uart->txjob, __stm32_deferred_tx, (uintptr_t) uart);

    if (hal_int_connect (irqn, __stm32_uart_handler, (uintptr_t) uart))
        {
        return -1;
        }

    if (hal_uart_register (hal_uart, name, &stm32_uart_methods,
                           HAL_UART_MODE_INT) != 0)
        {
        hal_int_disconnect (irqn);
        return -1;
        }

    hal_int_enable  (irqn);
    hal_int_setprio (irqn, 0);

    __HAL_UART_ENABLE_IT  (handle, UART_IT_RXNE);
    __HAL_UART_DISABLE_IT (handle, UART_IT_TXE);

    return 0;
    }

static int stm32_uart_chan_init (void)
    {
    int         ret;
    static bool inited = false;

    if (inited)
        {
        return 0;
        }

#ifdef CONFIG_STM32_UART1
    __HAL_RCC_USART1_CLK_ENABLE ();

    ret = __stm32_uart_chan_init (&stm32_uart1, USART1,
                                  CONFIG_STM32_UART1_BAUDRATE,
                                  SYM_N2C (GPIO, CONFIG_STM32_UART1_TX_GPIOX),
                                  CONFIG_STM32_UART1_TX_PIN,
                                  SYM_N2C (GPIO, CONFIG_STM32_UART1_RX_GPIOX),
                                  CONFIG_STM32_UART1_RX_PIN,
                                  CONFIG_STM32_UART1_AF, false);

    if (ret != 0)
        {
        return -1;
        }
#endif

#ifdef CONFIG_STM32_UART2
    __HAL_RCC_USART2_CLK_ENABLE ();

    ret = __stm32_uart_chan_init (&stm32_uart2, USART2,
                                  CONFIG_STM32_UART2_BAUDRATE,
                                  SYM_N2C (GPIO, CONFIG_STM32_UART2_TX_GPIOX),
                                  CONFIG_STM32_UART2_TX_PIN,
                                  SYM_N2C (GPIO, CONFIG_STM32_UART2_RX_GPIOX),
                                  CONFIG_STM32_UART2_RX_PIN,
                                  CONFIG_STM32_UART2_AF, false);

    if (ret != 0)
        {
        return -1;
        }
#endif

#ifdef CONFIG_STM32_UART3
    __HAL_RCC_USART3_CLK_ENABLE ();

    ret = __stm32_uart_chan_init (&stm32_uart3, USART3,
                                  CONFIG_STM32_UART3_BAUDRATE,
                                  SYM_N2C (GPIO, CONFIG_STM32_UART3_TX_GPIOX),
                                  CONFIG_STM32_UART3_TX_PIN,
                                  SYM_N2C (GPIO, CONFIG_STM32_UART3_RX_GPIOX),
                                  CONFIG_STM32_UART3_RX_PIN,
                                  CONFIG_STM32_UART3_AF, false);

    if (ret != 0)
        {
        return -1;
        }
#endif

#ifdef CONFIG_STM32_LPUART1
    __HAL_RCC_LPUART1_CLK_ENABLE ();

    ret = __stm32_uart_chan_init (&stm32_lpuart1, LPUART1,
                                  CONFIG_STM32_LPUART1_BAUDRATE,
                                  SYM_N2C (GPIO, CONFIG_STM32_LPUART1_TX_GPIOX),
                                  CONFIG_STM32_LPUART1_TX_PIN,
                                  SYM_N2C (GPIO, CONFIG_STM32_LPUART1_RX_GPIOX),
                                  CONFIG_STM32_LPUART1_RX_PIN,
                                  CONFIG_STM32_LPUART1_AF, true);

    if (ret != 0)
        {
        return -1;
        }
#endif

    inited = true;

    return 0;
    }

static int stm32_uart_init (void)
    {
    if (stm32_uart_chan_init () != 0)
        {
        return -1;
        }

#ifdef CONFIG_STM32_UART1
    if (__stm32_uart_init (&stm32_uart1, USART1_IRQn, "uart1") != 0)
        {
        return -1;
        }
#endif

#ifdef CONFIG_STM32_UART2
    if (__stm32_uart_init (&stm32_uart2, USART2_IRQn, "uart2") != 0)
        {
        return -1;
        }
#endif

#ifdef CONFIG_STM32_UART3
    if (__stm32_uart_init (&stm32_uart3, USART3_IRQn, "uart3") != 0)
        {
        return -1;
        }
#endif

#ifdef CONFIG_STM32_LPUART1
    if (__stm32_uart_init (&stm32_lpuart1, LPUART1_IRQn, "lpuart1") != 0)
        {
        return -1;
        }
#endif

    return 0;
    }

#ifdef CONFIG_STM32_UART1
int early_putc (unsigned char ch)
    {
    if (stm32_uart_chan_init () != 0)
        {
        return 0;
        }

    (void) HAL_UART_Transmit (&stm32_uart1.handle, (uint8_t *) &ch, 1, 0xffffffff);

    return 1;
    }
#endif

MODULE_INIT (driver, stm32_uart_init);

