/*!
    \file  i2c.c
    \brief I2C configuration file
    
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

#include "gd32f30x.h"
#include "i2c.h"
#include <stdio.h>

/*!
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable I2C0 clock */
    rcu_periph_clock_enable(RCU_I2C0);

    /* connect PB6 to I2C0_SCL */
    /* connect PB7 to I2C0_SDA */
    gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);
}

/*!
    \brief      configure the I2C0 interfaces
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_config(void)
{
    /* enable I2C clock */
    rcu_periph_clock_enable(RCU_I2C0);
    /* configure I2C clock */
    i2c_clock_config(I2C0,I2C0_SPEED,I2C_DTCY_2);
    /* configure I2C address */
    i2c_mode_addr_config(I2C0,I2C_I2CMODE_ENABLE,I2C_ADDFORMAT_7BITS,I2C0_SLAVE_ADDRESS7);
    /* enable I2C0 */
    i2c_enable(I2C0);
    /* enable acknowledge */
    i2c_ack_config(I2C0,I2C_ACK_ENABLE);
}
