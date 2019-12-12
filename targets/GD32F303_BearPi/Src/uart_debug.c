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

//#include "usart.h"

#include "gd32f30x.h"
#include "BearPi-IoT_gd32f303.h"

#include <stdint.h>
#include <stddef.h>

#include <osal.h>
#include <link_misc.h>

#include <los_hwi.h>
#include <los_sem.h>
#include <los_base.h>

#define CN_RCV_RING_BUFLEN  128

static uint32_t s_pUSART = EVAL_COM0;
static uint32_t s_uwIRQn = USART0_IRQn;

static bool_t s_uart_init = false;

/* USART1 init function */

static tag_ring_buffer_t  s_uartdebug_rcv_ring;
static unsigned char      s_uartdebug_rcv_ringmem[CN_RCV_RING_BUFLEN];
static UINT32        s_uartdebug_rcv_sync;
static void uart_debug_irq(void)
{
    unsigned char value;
    if(usart_flag_get(s_pUSART, USART_FLAG_RBNE) != RESET)
    {
        value = (uint8_t)(usart_data_receive(s_pUSART) & 0x00FF);
        ring_buffer_write(&s_uartdebug_rcv_ring,&value,1);
        LOS_SemPost(s_uartdebug_rcv_sync);
    }
    else if (usart_flag_get(s_pUSART,USART_FLAG_IDLE) != RESET)
    {
    	//usart_flag_clear(&uart_debug);
    }
}
void shell_uart_init(int baud)
{
	gd_eval_com_init(s_pUSART, baud);
    LOS_HwiCreate(s_uwIRQn, 3, 0, uart_debug_irq, 0);
    //usart_interrupt_enable(s_pUSART, USART_INT_IDLE);
    usart_interrupt_enable(s_pUSART, USART_INT_RBNE);
    
    //create the receive buffer and receive sync
    extern UINT32 osSemCreate (UINT16 usCount, UINT16 usMaxCount, UINT32 *puwSemHandle);
    osSemCreate(0,CN_RCV_RING_BUFLEN,(UINT32 *)&s_uartdebug_rcv_sync);
    ring_buffer_init(&s_uartdebug_rcv_ring,s_uartdebug_rcv_ringmem,CN_RCV_RING_BUFLEN,0,0);
    s_uart_init = true;

}


/* define fputc */
#if defined ( __CC_ARM ) || defined ( __ICCARM__ )  /* KEIL and IAR: printf will call fputc to print */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(s_pUSART, (uint8_t)ch);
    while (RESET == usart_flag_get(s_pUSART, USART_FLAG_TBE))
        ;
    return ch;
}
int fgetc(FILE *f){
    int ret = 0;
    unsigned char  value;
    do
    {
        if(LOS_OK == LOS_SemPend(s_uartdebug_rcv_sync,LOS_WAIT_FOREVER))
        {
            ret = ring_read(&s_uartdebug_rcv_ring,&value,1);
        }
    }while(ret <=0);
    ret = value;
    return ret;
}
#elif defined ( __GNUC__ )  /* GCC: printf will call _write to print */

int _put_char(int ch)
{
    usart_data_transmit(s_pUSART, (uint8_t) ch );
    while (usart_flag_get(s_pUSART, USART_FLAG_TBE)== RESET){
    }

    return ch;
}

int puts(const char* string) {
	return _write(0, (const void *) string, strlen(string));
}

__attribute__((used)) int _write(int fd, char *ptr, int len)
{
    if(s_uart_init)
    {
    	const uint8_t * current = (const uint8_t *) ptr;

    	for (size_t jj = 0; jj < len; jj++)
    	{
    		_put_char(current[jj]);

    		if (current[jj] == '\n')
    		{
    			_put_char('\r');
    		}
    	}
    }
    return len;
}
__attribute__((used)) int _read(int fd, char *ptr, int len)
{
    int ret = 0;
    unsigned char  value;
    do
    {
        if(LOS_OK == LOS_SemPend(s_uartdebug_rcv_sync,LOS_WAIT_FOREVER))
        {
            ret = ring_buffer_read(&s_uartdebug_rcv_ring,&value,1);
        }
    }while(ret <=0);
    *(unsigned char *)ptr = value;
    return 1;
}

#endif
