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


#include "stm32l4xx.h"
#include <osport.h>

static UART_HandleTypeDef sensor_uart;
static USART_TypeDef*     s_pUSART = USART3;
static uint32_t           s_uwIRQn = USART3_IRQn;

#define CN_RCVBUF_LEN  256  //cache a frame 
#define CN_RCVMEM_LEN  512  //use to cache more frames

struct uartio_cb
{
    u16_t        w_next;    //the next position to be write  
    semp_t       rcvsync;   //if a frame has been written to the ring, then active it
    tagRingBuf   rcvring;
    u8_t         rcvbuf[CN_RCVBUF_LEN];
    u8_t         rcvringmem[CN_RCVMEM_LEN];
    //for the debug here
    u32_t        rframeover; //how many times the frame has been over the max length
    u32_t        rframedrop; //how many frame has been droped for memmory
    u32_t        sndlen;     //how many bytes has been sent
    u32_t        rcvlen;     //how many bytes has been received
    u32_t        sndframe;   //how many frame has been sent
    u32_t        rcvframe;   //how many frame has been received
    u32_t        rcvringrst; //how many times the receive ring has been reset
};
static struct uartio_cb   s_uartio_cb;

/*******************************************************************************
function     :use this function to deal the uart interrupt
parameters   :
instruction  :we cached the data in the temp buffer,when the idle interrupt reached,
              then we write the data and the length to the ring if the ring has enough
              space
*******************************************************************************/
static void uartio_irq(void)
{
    u8_t  value;
    u16_t ringspace;
    if(__HAL_UART_GET_FLAG(&sensor_uart, UART_FLAG_RXNE) != RESET)
    {
       value = (uint8_t)(sensor_uart.Instance->RDR & 0x00FF);
       s_uartio_cb.rcvlen++;
       if(s_uartio_cb.w_next < CN_RCVBUF_LEN)
       {
           s_uartio_cb.rcvbuf[s_uartio_cb.w_next] = value;
           s_uartio_cb.w_next++;
       }
       else
       {
            s_uartio_cb.rframeover++;
       }
    }
    else if (__HAL_UART_GET_FLAG(&sensor_uart,UART_FLAG_IDLE) != RESET)
    {
        __HAL_UART_CLEAR_IDLEFLAG(&sensor_uart);
        ringspace = CN_RCVMEM_LEN - ring_datalen(&s_uartio_cb.rcvring);
        if(ringspace < s_uartio_cb.w_next)  //not enough mem 
        {
            s_uartio_cb.rframedrop++;
        }
        else
        {
            //write data to the ring buffer:len+data format
            ringspace = s_uartio_cb.w_next;
            ring_write(&s_uartio_cb.rcvring,(u8_t *)&ringspace,sizeof(ringspace));
            ring_write(&s_uartio_cb.rcvring,s_uartio_cb.rcvbuf,ringspace);
            semp_post(s_uartio_cb.rcvsync);
            s_uartio_cb.rcvframe++;
        }
        s_uartio_cb.w_next=0; //write from the head
    }
}

/*******************************************************************************
function     :use this function to initialize the uart
parameters   :
instruction  :
*******************************************************************************/
bool_t sensor_uart_init(s32_t baudrate)
{
	UART_HandleTypeDef * usart = &sensor_uart;
    //initialize the at controller
    memset(&s_uartio_cb,0,sizeof(s_uartio_cb));
    if(false == semp_create(&s_uartio_cb.rcvsync,CN_RCVMEM_LEN,0))
    {
        printf("%s:semp create error\n\r",__FUNCTION__);
        goto EXIT_SEMP;
    }
    ring_init(&s_uartio_cb.rcvring,s_uartio_cb.rcvringmem,CN_RCVMEM_LEN,0,0);
    
    usart->Instance = s_pUSART;
    usart->Init.BaudRate = baudrate;
    usart->Init.WordLength = UART_WORDLENGTH_8B;
    usart->Init.StopBits = UART_STOPBITS_1;
    usart->Init.Parity = UART_PARITY_NONE;
    usart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    usart->Init.Mode = UART_MODE_RX | UART_MODE_TX;
    if(HAL_UART_Init(usart) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    __HAL_UART_CLEAR_FLAG(usart,UART_FLAG_TC);
    LOS_HwiCreate(s_uwIRQn, 3, 0, uartio_irq, 0);
    __HAL_UART_ENABLE_IT(usart, UART_IT_IDLE);
    __HAL_UART_ENABLE_IT(usart, UART_IT_RXNE);
    return true;

EXIT_SEMP:
    return false;
}

void sensor_uart_deinit(void)
{
    UART_HandleTypeDef * husart = &sensor_uart;
    __HAL_UART_DISABLE(husart);
    __HAL_UART_DISABLE_IT(husart, UART_IT_IDLE);
    __HAL_UART_DISABLE_IT(husart, UART_IT_RXNE);
}

/*******************************************************************************
function     :use this function to send a frame to the uart
parameters   :
instruction  :
*******************************************************************************/
s32_t sensor_uart_snd(u8_t *buf, s32_t len,u32_t timeout)
{
    HAL_UART_Transmit(&sensor_uart,buf,len,timeout);
    s_uartio_cb.sndlen += len;
    s_uartio_cb.sndframe ++;

    return len;
}
/*******************************************************************************
function     :use this function to read a frame from the uart
parameters   :
instruction  :
*******************************************************************************/
s32_t sensor_uart_rcv(u8_t *buf,s32_t len,u32_t timeout)
{
    u16_t cpylen;
    u16_t framelen;
    u16_t readlen;
    int32_t ret = 0;
    UINTPTR lock;
    if(semp_pend(s_uartio_cb.rcvsync,timeout))
    {
        lock = LOS_IntLock();
        readlen = sizeof(framelen);
        cpylen = ring_read(&s_uartio_cb.rcvring,(u8_t *)&framelen,readlen);
        if(cpylen != readlen)
        {
            ring_reset(&s_uartio_cb.rcvring);  //bad ring format here
            s_uartio_cb.rcvringrst++;
        }
        else
        {
            if(framelen > len)
            {
                ring_reset(&s_uartio_cb.rcvring);  //bad ring format here
                s_uartio_cb.rcvringrst++;
            }
            else
            {
                readlen = framelen;
                cpylen = ring_read(&s_uartio_cb.rcvring,buf,readlen);
                if(cpylen != framelen)
                {
                    ring_reset(&s_uartio_cb.rcvring);  //bad ring format here
                    s_uartio_cb.rcvringrst++;
                }
                else
                {
                    ret = cpylen;
                }
            } 
        }
        LOS_IntRestore(lock);
    }
    return ret;
}

//add a debug command here
#include <shell.h>
//use this function to show the atuart status
static s32_t sensoruart_debug(s32_t argc,const char *argv[])
{
    printf("rframeover:0x%08x rframedrop:0x%08x sndlen:0x%08x rcvlen:0x%08x\n\r",\
            s_uartio_cb.rframeover,s_uartio_cb.rframedrop,s_uartio_cb.sndlen,s_uartio_cb.rcvlen);
    printf("sndframe:0x%08x rcvframe:0x%08x rcvringrst:0x%08x\n\r",\
            s_uartio_cb.sndframe,s_uartio_cb.rcvframe,s_uartio_cb.rcvringrst);
    return 0;
}
OSSHELL_EXPORT_CMD(sensoruart_debug,"sensor_uart","sensor_uart");



//make it as the at device here
#include <los_dev.h>

static s32_t  __at_read  (void *pri,u32_t offset,u8_t *buf,s32_t len,u32_t timeout)
{
    return sensor_uart_rcv(buf,len, timeout);

}
static s32_t  __at_write (void *pri,u32_t offset,u8_t *buf,s32_t len,u32_t timeout)
{
    return sensor_uart_snd(buf, len, timeout);

}


static const los_driv_op_t s_sensor_uart_op = {

    .read = __at_read,
    .write = __at_write,
};

OSDRIV_EXPORT(uart_sensor_driv,"sensor_uart",(los_driv_op_t *)&s_sensor_uart_op,NULL,O_RDWR);


