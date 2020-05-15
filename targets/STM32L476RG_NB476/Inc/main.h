/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
/* Includes LiteOS------------------------------------------------------------------*/

#include "los_base.h"
#include "los_config.h"
#include "los_sys.h"
#include "los_typedef.h"
#include "los_event.ph"
#include "los_event.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DHT11_DATA_Pin GPIO_PIN_13
#define DHT11_DATA_GPIO_Port GPIOC
#define KEY4_Pin GPIO_PIN_0
#define KEY4_GPIO_Port GPIOC
#define KEY3_Pin GPIO_PIN_1
#define KEY3_GPIO_Port GPIOC
#define KEY2_Pin GPIO_PIN_2
#define KEY2_GPIO_Port GPIOC
#define KEY1_Pin GPIO_PIN_3
#define KEY1_GPIO_Port GPIOC
#define LED_Pin GPIO_PIN_2
#define LED_GPIO_Port GPIOB
#define NB_RI_Pin GPIO_PIN_15
#define NB_RI_GPIO_Port GPIOB
#define NB_EN_Pin GPIO_PIN_6
#define NB_EN_GPIO_Port GPIOC
#define OLED_SCL_Pin GPIO_PIN_6
#define OLED_SCL_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_7
#define OLED_SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
