/*!
    \file  BearPi-IoT_gd32f303.h
    \brief definitions for BearPi-IoT_gd32f303's leds, keys and COM ports hardware resources

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

#ifndef BEARPI_IOT_GD32F303_H
#define BEARPI_IOT_GD32F303_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "gd32f30x.h"
     
/* exported types */
typedef enum 
{
    LED = 0,

} led_typedef_enum;

typedef enum 
{
    F1_KEY = 0,
    F2_KEY = 1,
} key_typedef_enum;

typedef enum 
{
    KEY_MODE_GPIO = 0,
    KEY_MODE_EXTI = 1
} keymode_typedef_enum;

/* eval board low layer led */
#define LEDn                             1U

#define LED_PIN                         GPIO_PIN_13
#define LED_GPIO_PORT                   GPIOC
#define LED_GPIO_CLK                    RCU_GPIOC
  


#define COMn                             4U

#define EVAL_COM0                        USART0
#define EVAL_COM0_CLK                    RCU_USART0
#define EVAL_COM0_TX_PIN                 GPIO_PIN_9
#define EVAL_COM0_RX_PIN                 GPIO_PIN_10
#define EVAL_COM0_GPIO_PORT              GPIOA
#define EVAL_COM0_GPIO_CLK               RCU_GPIOA

#define EVAL_COM1                        USART1
#define EVAL_COM1_CLK                    RCU_USART1
#define EVAL_COM1_TX_PIN                 GPIO_PIN_2
#define EVAL_COM1_RX_PIN                 GPIO_PIN_3
#define EVAL_COM1_GPIO_PORT              GPIOA
#define EVAL_COM1_GPIO_CLK               RCU_GPIOA

#define EVAL_COM2                        USART2
#define EVAL_COM2_CLK                    RCU_USART2
#define EVAL_COM2_TX_PIN                 GPIO_PIN_10
#define EVAL_COM2_RX_PIN                 GPIO_PIN_11
#define EVAL_COM2_GPIO_PORT              GPIOB
#define EVAL_COM2_GPIO_CLK               RCU_GPIOB

#define EVAL_COM3                        UART3
#define EVAL_COM3_CLK                    RCU_UART3
#define EVAL_COM3_TX_PIN                 GPIO_PIN_10
#define EVAL_COM3_RX_PIN                 GPIO_PIN_11
#define EVAL_COM3_GPIO_PORT              GPIOC
#define EVAL_COM3_GPIO_CLK               RCU_GPIOC

#define KEYn                             2U

/* F1 push-button */
#define F1_KEY_PIN                   GPIO_PIN_1
#define F1_KEY_GPIO_PORT             GPIOA
#define F1_KEY_GPIO_CLK              RCU_GPIOA
#define F1_KEY_EXTI_LINE             EXTI_1
#define F1_KEY_EXTI_PORT_SOURCE      GPIO_PORT_SOURCE_GPIOA
#define F1_KEY_EXTI_PIN_SOURCE       GPIO_PIN_SOURCE_1
#define F1_KEY_EXTI_IRQn             EXTI1_IRQn

/* F2 push-button */
#define F2_KEY_PIN                   GPIO_PIN_0
#define F2_KEY_GPIO_PORT             GPIOA
#define F2_KEY_GPIO_CLK              RCU_GPIOA
#define F2_KEY_EXTI_LINE             EXTI_0
#define F2_KEY_EXTI_PORT_SOURCE      GPIO_PORT_SOURCE_GPIOA
#define F2_KEY_EXTI_PIN_SOURCE       GPIO_PIN_SOURCE_0
#define F2_KEY_EXTI_IRQn             EXTI0_IRQn  





/* function declarations */
/* configure led GPIO */
void gd_eval_led_init(led_typedef_enum lednum);
/* turn on selected led */
void gd_eval_led_on(led_typedef_enum lednum);
/* turn off selected led */
void gd_eval_led_off(led_typedef_enum lednum);
/* toggle the selected led */
void gd_eval_led_toggle(led_typedef_enum lednum);
/* configure key */
void gd_eval_key_init(key_typedef_enum key_num, keymode_typedef_enum key_mode);
/* return the selected key state */
uint8_t gd_eval_key_state_get(key_typedef_enum key);
/* configure COM port */
void gd_eval_com_init(uint32_t com, uint32_t baud);

#ifdef __cplusplus
}
#endif

#endif /* BEARPI_IOT_GD32F303_H */
