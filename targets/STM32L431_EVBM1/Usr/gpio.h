/**
  ******************************************************************************
  * File Name          : gpio.h
  * Description        : This file contains all the functions prototypes for 
  *                      the gpio  
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */


 /* Private define ------------------------------------------------------------*/
 #define Beep_Pin         GPIO_PIN_5
 #define Beep_GPIO_Port   GPIOB
 #define LED_Pin GPIO_PIN_13
 #define LED_GPIO_Port GPIOC
 #define Light_Pin GPIO_PIN_5
 #define Light_GPIO_Port GPIOA
 #define KEY1_Pin GPIO_PIN_0
 #define KEY1_GPIO_Port GPIOC
 #define KEY1_EXTI_IRQn EXTI0_IRQn
 #define KEY2_Pin GPIO_PIN_1
 #define KEY2_GPIO_Port GPIOC
 #define KEY2_EXTI_IRQn EXTI1_IRQn
 #define KEY3_Pin GPIO_PIN_2
 #define KEY3_GPIO_Port GPIOC
 #define KEY3_EXTI_IRQn EXTI2_IRQn
 #define KEY4_Pin GPIO_PIN_3
 #define KEY4_GPIO_Port GPIOC
 #define KEY4_EXTI_IRQn EXTI3_IRQn
 #define MQ2_DO_Pin GPIO_PIN_6
 #define MQ2_DO_GPIO_Port GPIOA
 #define OLED_SCL_Pin GPIO_PIN_10
 #define OLED_SCL_GPIO_Port GPIOB
 #define OLED_SDA_Pin GPIO_PIN_11
 #define OLED_SDA_GPIO_Port GPIOB

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
