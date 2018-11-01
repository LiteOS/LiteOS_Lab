/*!
    \file  gd32f3x0_eval.c
    \brief firmware functions to manage leds, keys, COM ports
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include "gd32f3x0_eval.h"

/* private variables */
static uint32_t GPIO_PORT[LEDn]       = {LED1_GPIO_PORT,
                                         LED2_GPIO_PORT,
                                         LED3_GPIO_PORT,
                                         LED4_GPIO_PORT};

static uint32_t GPIO_PIN[LEDn]        = {LED1_PIN,
                                         LED2_PIN, 
                                         LED3_PIN, 
                                         LED4_PIN};

static rcu_periph_enum COM_CLK[COMn]  = {EVAL_COM1_CLK,
                                         EVAL_COM2_CLK};

static uint32_t COM_TX_PIN[COMn]      = {EVAL_COM1_TX_PIN,
                                         EVAL_COM2_TX_PIN};

static uint32_t COM_RX_PIN[COMn]      = {EVAL_COM1_RX_PIN,
                                         EVAL_COM2_RX_PIN};

static rcu_periph_enum GPIO_CLK[LEDn] = {LED1_GPIO_CLK,
                                         LED2_GPIO_CLK, 
                                         LED3_GPIO_CLK, 
                                         LED4_GPIO_CLK};

static uint32_t KEY_PORT[KEYn]        = {WAKEUP_KEY_GPIO_PORT, 
                                         TAMPER_KEY_GPIO_PORT,
                                         USER1_KEY_GPIO_PORT,
                                         USER2_KEY_GPIO_PORT};

static uint32_t KEY_PIN[KEYn]         = {WAKEUP_KEY_PIN, 
                                         TAMPER_KEY_PIN,
                                         USER1_KEY_PIN,
                                         USER2_KEY_PIN};

static rcu_periph_enum KEY_CLK[KEYn]      = {WAKEUP_KEY_GPIO_CLK, 
                                            TAMPER_KEY_GPIO_CLK,
                                            USER1_KEY_GPIO_CLK,
                                            USER2_KEY_GPIO_CLK};

static exti_line_enum KEY_EXTI_LINE[KEYn] = {WAKEUP_KEY_EXTI_LINE,
                                            TAMPER_KEY_EXTI_LINE,
                                            USER1_KEY_EXTI_LINE,
                                            USER2_KEY_EXTI_LINE};

static uint8_t KEY_PORT_SOURCE[KEYn]     = {WAKEUP_KEY_EXTI_PORT_SOURCE,
                                            TAMPER_KEY_EXTI_PORT_SOURCE,
                                            USER1_KEY_EXTI_PORT_SOURCE,
                                            USER2_KEY_EXTI_PORT_SOURCE};

static uint8_t KEY_PIN_SOURCE[KEYn]      = {WAKEUP_KEY_EXTI_PIN_SOURCE,
                                            TAMPER_KEY_EXTI_PIN_SOURCE,
                                            USER1_KEY_EXTI_PIN_SOURCE,
                                            USER2_KEY_EXTI_PIN_SOURCE};

static uint8_t KEY_IRQn[KEYn]            = {WAKEUP_KEY_EXTI_IRQn, 
                                            TAMPER_KEY_EXTI_IRQn,
                                            USER1_KEY_EXTI_IRQn,
                                            USER2_KEY_EXTI_IRQn};

/* GD32F3x0 eval low layer private functions */
/*!
    \brief      configure led GPIO
    \param[in]  lednum: specify the led to be configured
      \arg        LED1
      \arg        LED2
      \arg        LED3
      \arg        LED4
    \param[out] none
    \retval     none
*/
void gd_eval_led_init(led_typedef_enum lednum)
{
    /* enable the led clock */
    rcu_periph_clock_enable(GPIO_CLK[lednum]);

    /* configure led GPIO port */ 
    gpio_mode_set(GPIO_PORT[lednum], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN[lednum]);
    gpio_output_options_set(GPIO_PORT[lednum], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN[lednum]);

    GPIO_BC(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

/*!
    \brief      turn on selected led
    \param[in]  lednum: specify the led to be turned on
      \arg        LED1
      \arg        LED2
      \arg        LED3
      \arg        LED4
    \param[out] none
    \retval     none
*/
void gd_eval_led_on(led_typedef_enum lednum)
{
    GPIO_BOP(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

/*!
    \brief      turn off selected led
    \param[in]  lednum: specify the led to be turned off
      \arg        LED1
      \arg        LED2
      \arg        LED3
      \arg        LED4
    \param[out] none
    \retval     none
*/
void gd_eval_led_off(led_typedef_enum lednum)
{
    GPIO_BC(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

/*!
    \brief      toggle selected led
    \param[in]  lednum: specify the led to be toggled
      \arg        LED1
      \arg        LED2
      \arg        LED3
      \arg        LED4
    \param[out] none
    \retval     none
*/
void gd_eval_led_toggle(led_typedef_enum lednum)
{
    GPIO_TG(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

/*!
    \brief      configure key
    \param[in]  keynum: specify the key to be configured
      \arg        KEY_TAMPER: tamper key
      \arg        KEY_WAKEUP: wakeup key
      \arg        KEY_USER1: user1 key
      \arg        KEY_USER2: user2 key
    \param[in]  keymode: specify button mode
      \arg        KEY_MODE_GPIO: key will be used as simple IO
      \arg        KEY_MODE_EXTI: key will be connected to EXTI line with interrupt
    \param[out] none
    \retval     none
*/
void gd_eval_key_init(key_typedef_enum keynum, keymode_typedef_enum keymode)
{
    /* enable the key clock */
    rcu_periph_clock_enable(KEY_CLK[keynum]);
    rcu_periph_clock_enable(RCU_CFGCMP);

    /* configure button pin as input */
    gpio_mode_set(KEY_PORT[keynum], GPIO_MODE_INPUT, GPIO_PUPD_NONE, KEY_PIN[keynum]);

    if (keymode == KEY_MODE_EXTI) {
        /* enable and set key EXTI interrupt to the lowest priority */
        nvic_irq_enable(KEY_IRQn[keynum], 2U, 0U);

        /* connect key EXTI line to key GPIO pin */
        syscfg_exti_line_config(KEY_PORT_SOURCE[keynum], KEY_PIN_SOURCE[keynum]);

        /* configure key EXTI line */
        exti_init(KEY_EXTI_LINE[keynum], EXTI_INTERRUPT, EXTI_TRIG_FALLING);
        exti_interrupt_flag_clear(KEY_EXTI_LINE[keynum]);
    }
}

/*!
    \brief      return the selected key state
    \param[in]  keynum: specify the key to be checked
      \arg        KEY_TAMPER: tamper key
      \arg        KEY_WAKEUP: wakeup key
      \arg        KEY_USER1: user1 key
      \arg        KEY_USER2: user2 key
    \param[out] none
    \retval     the key's GPIO pin value
*/
uint8_t gd_eval_key_state_get(key_typedef_enum keynum)
{
    return gpio_input_bit_get(KEY_PORT[keynum], KEY_PIN[keynum]);
}

/*!
    \brief      configure COM port
    \param[in]  com: COM on the board
      \arg        EVAL_COM1: COM1 on the board
      \arg        EVAL_COM2: COM2 on the board
    \param[out] none
    \retval     none
*/
void gd_eval_com_init(uint32_t com)
{
    uint32_t COM_ID;
    
    if(EVAL_COM1 == com){
        COM_ID = 0U;
    }else if(EVAL_COM2 == com){
        COM_ID = 1U;
    }else{
    }

    /* enable COM GPIO clock */
    rcu_periph_clock_enable(EVAL_COM_GPIO_CLK);

    /* enable USART clock */
    rcu_periph_clock_enable(COM_CLK[COM_ID]);

    /* connect port to USARTx_Tx */
    gpio_af_set(EVAL_COM_GPIO_PORT, EVAL_COM_AF, COM_TX_PIN[COM_ID]);

    /* connect port to USARTx_Rx */
    gpio_af_set(EVAL_COM_GPIO_PORT, EVAL_COM_AF, COM_RX_PIN[COM_ID]);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(EVAL_COM_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, COM_TX_PIN[COM_ID]);
    gpio_output_options_set(EVAL_COM_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, COM_TX_PIN[COM_ID]);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(EVAL_COM_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, COM_RX_PIN[COM_ID]);
    gpio_output_options_set(EVAL_COM_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, COM_RX_PIN[COM_ID]);

    /* USART configure */
    usart_deinit(com);
    usart_baudrate_set(com, 115200U);
    usart_receive_config(com, USART_RECEIVE_ENABLE);
    usart_transmit_config(com, USART_TRANSMIT_ENABLE);

    usart_enable(com);
}
