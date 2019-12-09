/*!
    \file  gd32f30x_it.c
    \brief interrupt service routines

    \version 2019-10-30, V1.0.0, demo for BearPi-IoT_GD32F30x
*/

/*
    Copyright (c) 2019, BearPi LIMITED.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32f30x_it.h"
#include "systick.h"
// #include "E53_SC1.h"


extern uint8_t   rx_buffer[];
extern volatile uint16_t   rx_counter;
int8_t key1 = 0;
int8_t key2 = 0;
int16_t toggle = 0;

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
__attribute__((weak)) void PendSV_Handler(void)
{
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
__attribute__((weak)) void SysTick_Handler(void)
{
    delay_decrement();
}
// /*!
//     \brief      this function handles external lines 1 interrupt request
//     \param[in]  none
//     \param[out] none
//     \retval     none
// */
// void EXTI1_IRQHandler(void)
// {
//     if(RESET != exti_interrupt_flag_get(EXTI_1)){
//         key1 = 1;
//         printf("proceed to get ue_status!\r\n");
//         exti_interrupt_flag_clear(EXTI_1);
//     }
// }

// /*!
//     \brief      this function handles external lines 0 interrupt request
//     \param[in]  none
//     \param[out] none
//     \retval     none
// */
// void EXTI0_IRQHandler(void)
// {
//     if(RESET != exti_interrupt_flag_get(EXTI_0)){
//         key2 = 1;
//         printf("toggle LED and report!\r\n");
//         toggle = !toggle;
//         // gpio_bit_write(SC1_Light_GPIO_Port, SC1_Light_Pin, (bit_status)(1 - gpio_input_bit_get(SC1_Light_GPIO_Port, SC1_Light_Pin)));
//         exti_interrupt_flag_clear(EXTI_0);
//     }
// }

void UART3_IRQHandler(void)
{
    printf("this is uart3_irq\r\n");
    if(RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_RBNE)){
        /* read one byte from the receive data register */
        rx_buffer[rx_counter++] = (uint8_t)usart_data_receive(UART3);
        if(rx_counter >= 1000)
        {
            /* disable the USART0 receive interrupt */
            usart_interrupt_disable(UART3, USART_INT_RBNE);
        }
    }
       
}
void USART0_IRQHandler(void)
{
    printf("this is uart1_irq\r\n");
    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)){
        /* read one byte from the receive data register */
        rx_buffer[rx_counter++] = (uint8_t)usart_data_receive(USART0);
                if(rx_counter >= 1000)
        {
            /* disable the USART0 receive interrupt */
            usart_interrupt_disable(USART0, USART_INT_RBNE);
        }
    }
       
}
