/*!
    \file  gd32f3x0_eval.h
    \brief definitions for GD32f3x0_EVAL's leds, keys and COM ports hardware resources
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#ifndef GD32F3X0_EVAL_H
#define GD32F3X0_EVAL_H

#ifdef cplusplus
 extern "C" {
#endif

#include "gd32f3x0.h"

/* exported types */
typedef enum 
{
    LED1 = 0,
    LED2 = 1,
    LED3 = 2,
    LED4 = 3
}led_typedef_enum;

typedef enum 
{
    KEY_WAKEUP = 0,
    KEY_TAMPER = 1,
    KEY_USER1  = 2,
    KEY_USER2  = 3
}key_typedef_enum;

typedef enum 
{
    KEY_MODE_GPIO = 0,
    KEY_MODE_EXTI = 1
}keymode_typedef_enum;


/* GD323x0 eval low layer led */
#define LEDn                             4U

#define LED1_PIN                         GPIO_PIN_8
#define LED1_GPIO_PORT                   GPIOB
#define LED1_GPIO_CLK                    RCU_GPIOB

#define LED2_PIN                         GPIO_PIN_9
#define LED2_GPIO_PORT                   GPIOB
#define LED2_GPIO_CLK                    RCU_GPIOB

#define LED3_PIN                         GPIO_PIN_10
#define LED3_GPIO_PORT                   GPIOB
#define LED3_GPIO_CLK                    RCU_GPIOB

#define LED4_PIN                         GPIO_PIN_9
#define LED4_GPIO_PORT                   GPIOC
#define LED4_GPIO_CLK                    RCU_GPIOC

/* GD323x0 eval low layer button */  
#define KEYn                             4U

/* wakeup push-button */
#define WAKEUP_KEY_PIN                   GPIO_PIN_0
#define WAKEUP_KEY_GPIO_PORT             GPIOA
#define WAKEUP_KEY_GPIO_CLK              RCU_GPIOA
#define WAKEUP_KEY_EXTI_LINE             EXTI_0
#define WAKEUP_KEY_EXTI_PORT_SOURCE      EXTI_SOURCE_GPIOA
#define WAKEUP_KEY_EXTI_PIN_SOURCE       EXTI_SOURCE_PIN0
#define WAKEUP_KEY_EXTI_IRQn             EXTI0_1_IRQn

/* tamper push-button */
#define TAMPER_KEY_PIN                   GPIO_PIN_13
#define TAMPER_KEY_GPIO_PORT             GPIOC
#define TAMPER_KEY_GPIO_CLK              RCU_GPIOC
#define TAMPER_KEY_EXTI_LINE             EXTI_13
#define TAMPER_KEY_EXTI_PORT_SOURCE      EXTI_SOURCE_GPIOC
#define TAMPER_KEY_EXTI_PIN_SOURCE       EXTI_SOURCE_PIN13
#define TAMPER_KEY_EXTI_IRQn             EXTI4_15_IRQn

/* user1 push-button */
#define USER1_KEY_PIN                     GPIO_PIN_5
#define USER1_KEY_GPIO_PORT               GPIOC
#define USER1_KEY_GPIO_CLK                RCU_GPIOC
#define USER1_KEY_EXTI_LINE               EXTI_5
#define USER1_KEY_EXTI_PORT_SOURCE        EXTI_SOURCE_GPIOC
#define USER1_KEY_EXTI_PIN_SOURCE         EXTI_SOURCE_PIN5
#define USER1_KEY_EXTI_IRQn               EXTI4_15_IRQn

/* user2 push-button */
#define USER2_KEY_PIN                     GPIO_PIN_4
#define USER2_KEY_GPIO_PORT               GPIOC
#define USER2_KEY_GPIO_CLK                RCU_GPIOC
#define USER2_KEY_EXTI_LINE               EXTI_4
#define USER2_KEY_EXTI_PORT_SOURCE        EXTI_SOURCE_GPIOC
#define USER2_KEY_EXTI_PIN_SOURCE         EXTI_SOURCE_PIN4
#define USER2_KEY_EXTI_IRQn               EXTI4_15_IRQn

/* GD323x0 eval low layer COM */
#define COMn                             2U

/* definition for COM1, connected to USART0 */
#define EVAL_COM1                        USART0
#define EVAL_COM1_CLK                    RCU_USART0

#define EVAL_COM1_TX_PIN                 GPIO_PIN_9
#define EVAL_COM1_RX_PIN                 GPIO_PIN_10

#define EVAL_COM_GPIO_PORT               GPIOA
#define EVAL_COM_GPIO_CLK                RCU_GPIOA
#define EVAL_COM_AF                      GPIO_AF_1

/* definition for COM2, connected to USART1 */
#define EVAL_COM2                        USART1
#define EVAL_COM2_CLK                    RCU_USART1

#define EVAL_COM2_TX_PIN                 GPIO_PIN_2
#define EVAL_COM2_RX_PIN                 GPIO_PIN_3

#define EVAL_COM_GPIO_PORT               GPIOA
#define EVAL_COM_GPIO_CLK                RCU_GPIOA
#define EVAL_COM_AF                      GPIO_AF_1


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
void gd_eval_key_init(key_typedef_enum keynum, keymode_typedef_enum keymode);
/* return the selected key state */
uint8_t gd_eval_key_state_get(key_typedef_enum keynum);
/* configure COM port */
void gd_eval_com_init(uint32_t com);

#ifdef cplusplus
}
#endif

#endif /* GD32F3x0_EVAL_H */

