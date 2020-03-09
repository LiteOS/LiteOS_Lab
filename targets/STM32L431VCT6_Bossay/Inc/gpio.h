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
/* Private defines -----------------------------------------------------------*/
/* IoTBox IO宏定义 */
#define LORA_RESET_Pin GPIO_PIN_2
#define LORA_RESET_GPIO_Port GPIOE
#define LORA_DIO_3_Pin GPIO_PIN_3
#define LORA_DIO_3_GPIO_Port GPIOE
#define LORA_DIO_2_Pin GPIO_PIN_4
#define LORA_DIO_2_GPIO_Port GPIOE
#define LORA_DIO_1_Pin GPIO_PIN_5
#define LORA_DIO_1_GPIO_Port GPIOE
#define LORA_DIO_0_Pin GPIO_PIN_6
#define LORA_DIO_0_GPIO_Port GPIOE
#define LOAR_SPI_NSS_Pin GPIO_PIN_15
#define LOAR_SPI_NSS_GPIO_Port GPIOA
#define LORA_SPI_SCK_Pin GPIO_PIN_10
#define LORA_SPI_SCK_GPIO_Port GPIOC
#define LORA_SPI_MISO_Pin GPIO_PIN_11
#define LORA_SPI_MISO_GPIO_Port GPIOC
#define LORA_SPI_MOSI_Pin GPIO_PIN_5
#define LORA_SPI_MOSI_GPIO_Port GPIOB

#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC

#define WAN_AT_RX_Pin GPIO_PIN_0
#define WAN_AT_RX_GPIO_Port GPIOC
#define WAN_AT_TX_Pin GPIO_PIN_1
#define WAN_AT_TX_GPIO_Port GPIOC
#define WAN_RESET_Pin GPIO_PIN_0
#define WAN_RESET_GPIO_Port GPIOA

#define LCD_SPI_SCK_Pin GPIO_PIN_1
#define LCD_SPI_SCK_GPIO_Port GPIOA

#define DEBUG_TX_Pin GPIO_PIN_2
#define DEBUG_TX_GPIO_Port GPIOA
#define DEBUG_RX_Pin GPIO_PIN_3
#define DEBUG_RX_GPIO_Port GPIOA


#define MOTOR_Pin GPIO_PIN_5
#define MOTOR_GPIO_Port GPIOA

#define MQ_ADC_Pin GPIO_PIN_6
#define MQ_ADC_GPIO_Port GPIOA


#define GPS_UART_TX_Pin GPIO_PIN_4
#define GPS_UART_TX_GPIO_Port GPIOC
#define GPS_UART_RX_Pin GPIO_PIN_5
#define GPS_UART_RX_GPIO_Port GPIOC
#define GPS_PWR_EN_Pin GPIO_PIN_7
#define GPS_PWR_EN_GPIO_Port GPIOE
#define GPS_RESET_Pin GPIO_PIN_8
#define GPS_RESET_GPIO_Port GPIOE

/*******SPI_FLASH**********/
#define QSPI_BK1_IO1_Pin GPIO_PIN_0
#define QSPI_BK1_IO1_GPIO_Port GPIOB
#define QSPI_BK1_IO0_Pin GPIO_PIN_1
#define QSPI_BK1_IO0_GPIO_Port GPIOB
#define QSPI_CLK_Pin GPIO_PIN_10
#define QSPI_CLK_GPIO_Port GPIOE
#define QSPI_BK1_NCS_Pin GPIO_PIN_11
#define QSPI_BK1_NCS_GPIO_Port GPIOE

#define SHT30_INT_Pin GPIO_PIN_9
#define SHT30_INT_GPIO_Port GPIOE
#define SHT30_IIC_SCL_Pin GPIO_PIN_10
#define SHT30_IIC_SCL_GPIO_Port GPIOB
#define SHT30_IIC_SDA_Pin GPIO_PIN_11
#define SHT30_IIC_SDA_GPIO_Port GPIOB

#define LCD_SPI_MOSI_Pin GPIO_PIN_15
#define LCD_SPI_MOSI_GPIO_Port GPIOE
#define LCD_WR_RS_Pin GPIO_PIN_8
#define LCD_WR_RS_GPIO_Port GPIOD
#define LCD_RESET_Pin GPIO_PIN_9
#define LCD_RESET_GPIO_Port GPIOD
#define LCD_POWER_Pin GPIO_PIN_10
#define LCD_POWER_GPIO_Port GPIOD

#define BEEP_Pin GPIO_PIN_8
#define BEEP_GPIO_Port GPIOA

#define LIGHT_Pin GPIO_PIN_11
#define LIGHT_GPIO_Port GPIOA

#define SDMMC_D0_Pin GPIO_PIN_8
#define SDMMC_D0_GPIO_Port GPIOC
#define SDMMC_CK_Pin GPIO_PIN_12
#define SDMMC_CK_GPIO_Port GPIOC
#define SDMMC_CMD_Pin GPIO_PIN_2
#define SDMMC_CMD_GPIO_Port GPIOD

#define BH1750_IIC_SCL_Pin GPIO_PIN_6
#define BH1750_IIC_SCL_GPIO_Port GPIOB
#define BH1750_IIC_SDA_Pin GPIO_PIN_7
#define BH1750_IIC_SDA_GPIO_Port GPIOB


#define RSRV_ADC_OUT_Pin GPIO_PIN_3
#define RSRV_ADC_OUT_GPIO_Port GPIOC
#define RSRV_GPIO3_Pin GPIO_PIN_8
#define RSRV_GPIO3_GPIO_Port GPIOB
#define RSRV_IIC_SDA_Pin GPIO_PIN_4
#define RSRV_IIC_SDA_GPIO_Port GPIOB
#define RSRV_DAC_IN_Pin GPIO_PIN_4
#define RSRV_DAC_IN_GPIO_Port GPIOA
#define RSRV_IIC_SCL_Pin GPIO_PIN_7
#define RSRV_IIC_SCL_GPIO_Port GPIOA
#define RSRV_UART_TX_Pin GPIO_PIN_9
#define RSRV_UART_TX_GPIO_Port GPIOA
#define RSRV_UART_RX_Pin GPIO_PIN_10
#define RSRV_UART_RX_GPIO_Port GPIOA
#define RSRV_SPI_NSS_Pin GPIO_PIN_12
#define RSRV_SPI_NSS_GPIO_Port GPIOB
#define RSRV_SPI_SCK_Pin GPIO_PIN_13
#define RSRV_SPI_SCK_GPIO_Port GPIOB
#define RSRV_SPI_MOSI_Pin GPIO_PIN_15
#define RSRV_SPI_MOSI_GPIO_Port GPIOB
#define RSRV_SPI_MISO_Pin GPIO_PIN_2
#define RSRV_SPI_MISO_GPIO_Port GPIOC
#define RSRV_GPIO1_Pin GPIO_PIN_11
#define RSRV_GPIO1_GPIO_Port GPIOD
#define RSRV_GPIO2_Pin GPIO_PIN_12
#define RSRV_GPIO2_GPIO_Port GPIOD
#define RSRV_GPIO4_Pin GPIO_PIN_14
#define RSRV_GPIO4_GPIO_Port GPIOD
#define RSRV_GPIO5_Pin GPIO_PIN_15
#define RSRV_GPIO5_GPIO_Port GPIOD

#define KEY1_Pin GPIO_PIN_12
#define KEY1_GPIO_Port GPIOE
#define KEY2_Pin GPIO_PIN_13
#define KEY2_GPIO_Port GPIOE


#define LIGHT_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define BEEP_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define MOTOR_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define GPS_PWR_EN_GPIO_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE()

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

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
