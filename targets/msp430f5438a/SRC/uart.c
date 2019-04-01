/*----------------------------------------------------------------------------
 * Copyright (c) <2019>, <Huawei Technologies Co., Ltd>
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

#include <stdbool.h>

#include <msp430.h>

#include <los_typedef.h>
#include <los_hwi.h>
#include <los_task.h>
#include <los_sem.h>

#define RING_SIZE   64

static UINT32 s_rxsem = LOS_ERRNO_SEM_PTR_NULL;
static UINT32 s_txsem = LOS_ERRNO_SEM_PTR_NULL;

struct ring
{
    size_t          head;
    size_t          tail;
    unsigned char   buff [RING_SIZE];                   // ring buff
};

static struct ring tx_ring;

static struct ring rx_ring;

static inline size_t ring_len (struct ring * ring)
{
    return ring->tail - ring->head;
}

static bool ring_empty (struct ring * ring)
{
    return ring->tail == ring->head;
}

static bool ring_full (struct ring * ring)
{
    return ring_len (ring) == RING_SIZE;
}

static unsigned char ring_get (struct ring * ring)
    {
    return ring->buff [ring->head++ & (RING_SIZE - 1)];
    }

static void ring_put (struct ring * ring, unsigned char ch)
    {
    if (ring_full (ring))
        {
        ring->head++;
        }

    ring->buff [ring->tail++ & (RING_SIZE - 1)] = ch;
    }

unsigned char uart_getc (void)
{
    uintptr_t     gie;
    unsigned char ch;

    LOS_SemPend (s_rxsem, LOS_WAIT_FOREVER);
    gie = LOS_IntLock ();       /* for SMP, spinlock is needed */
    ch  = ring_get (&rx_ring);
    LOS_IntRestore (gie);

    return ch;
}

void uart_putc (unsigned char ch)
{
    BOOL      empty;
    uintptr_t gie;

    LOS_SemPend (s_txsem, LOS_WAIT_FOREVER);

    gie = LOS_IntLock ();       /* for SMP, spinlock is needed */

    empty = ring_empty (&tx_ring);

    ring_put (&tx_ring, ch);

    if (empty)
    {
        UCA0IE |= UCTXIE;
    }

    LOS_IntRestore (gie);
}

void uart_puts (unsigned char * str)
{
    unsigned char ch;

    while ((ch = *str++) != '\0')
    {
        uart_putc (ch);

        if (ch == '\n')
        {
            uart_putc ('\r');
        }
    }
}

static void uart_isr (uintptr_t arg)
{
    BOOL full;

    (void) arg;

    switch (__even_in_range (UCA0IV, 4))
    {
    case 0:
        break;                                  // Vector 0 - no interrupt
    case 2:                                     // Vector 2 - RXIFG
        full = ring_full (&rx_ring);
        ring_put (&rx_ring, UCA0RXBUF);
        if (!full)
        {
            LOS_SemPost (s_rxsem);
        }
        break;
    case 4:                                     // Vector 4 - TXIFG
        if (ring_empty (&tx_ring))
        {
            UCA0IFG |= UCTXIFG;
            UCA0IE  &= ~UCTXIE;
            break;
        }

        UCA0TXBUF = ring_get (&tx_ring);

        LOS_SemPost (s_txsem);

        break;
    default:
        break;
    }
}

UINT32 uart_init (void)
{
    UINT32 ret;

    ret = LOS_HwiCreate (USCI_A0_VECTOR, 0, 0, uart_isr, 0);

    if (ret != LOS_OK)
    {
        return ret;
    }

    ret = LOS_SemCreate (0, &s_rxsem);

    if (ret != LOS_OK)
    {
        goto err_hwi_del;
    }

    ret = LOS_SemCreate (RING_SIZE, &s_txsem);

    if (ret != LOS_OK)
    {
        goto err_rxsem_del;
    }

    P3SEL     = 0x30;   /* set P3.4 = UCA0TXD/UCA0SIMO, P3.5 = UCA0RXD/UCA0SOMI */

    /*
     * To avoid unpredictable behavior, configure or reconfigure the USCI_A module
     * only when UCSWRST is set.
     *
     * The recommended USCI initialization/reconfiguration process is:
     * 1. Set UCSWRST.
     * 2. Initialize all USCI registers with UCSWRST = 1
     * 3. Configure ports.
     * 4. Clear UCSWRST via software.
     * 5. Enable interrupts (optional) via UCRXIE and/or UCTXIE.
     */

    UCA0CTL1  = UCSWRST;

    /*
     * (UCAxCTL1[7:6], UCSSELx) USCI clock source select:
     * 00b = UCAxCLK (external USCI clock)
     * 01b = ACLK
     * 10b = SMCLK
     * 11b = SMCLK
     */

    UCA0CTL1 |=  UCSSEL__ACLK;

    /*
     * set baudrate to 9600
     *
     * BRCLK UCBRx UCBRSx UCBRFx BAUDRATE
     * 32768    27      2      0     1200
     * 32768    13      6      0     2400
     * 32768    6       7      0     4800
     * 32768    3       3      0     9600
     */

    UCA0BR0  = 0x03;
    UCA0BR1  = 0x00;
    UCA0MCTL = UCBRS_3 + UCBRF_0;

    UCA0CTL1 &= ~UCSWRST;
    UCA0IE   |= UCRXIE;

    return LOS_OK;

err_hwi_del:
    LOS_HwiDelete (USCI_A0_VECTOR);
err_rxsem_del:
    LOS_SemDelete (s_rxsem);
    s_rxsem  = LOS_ERRNO_SEM_PTR_NULL;

    return ret;
}

