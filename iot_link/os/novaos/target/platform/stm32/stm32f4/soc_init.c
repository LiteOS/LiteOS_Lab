/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
 *
 * LiteOS NOVA is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *
 * 	http://license.coscl.org.cn/MulanPSL
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
 * FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v1 for more details.
 */

#include <stdint.h>

#include <stm32f4xx_hal.h>
#include <system_stm32f4xx.h>

#include <stm32f4xx_hal_conf.h>
#include <stm32f4xx_hal_rcc.h>

#include <config.h>
#include <init.h>
#include <tick.h>

/* externs */

extern void SystemClock_Config (void);

uint32_t HAL_GetTick (void)
    {
    return tick_count_get ();
    }

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void __noreturn _Error_Handler (char * file, int line)
    {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    while(1)
        {
        }
    /* USER CODE END Error_Handler_Debug */
    }

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config (void)
    {
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage
    */
    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Initializes the CPU, AHB and APB busses clocks */

    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = 16;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM            = 8;
    RCC_OscInitStruct.PLL.PLLN            = 180;
    RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ            = 4;
    if (HAL_RCC_OscConfig (&RCC_OscInitStruct) != HAL_OK)
        {
        _Error_Handler (__FILE__, __LINE__);
        }

    /* Initializes the CPU, AHB and APB busses clocks */

    if (HAL_PWREx_EnableOverDrive () != HAL_OK)
        {
        _Error_Handler (__FILE__, __LINE__);
        }

    /* Initializes the CPU, AHB and APB busses clocks */

    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK   |
                                       RCC_CLOCKTYPE_SYSCLK |
                                       RCC_CLOCKTYPE_PCLK1  |
                                       RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig (&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
        {
        _Error_Handler (__FILE__, __LINE__);
        }

    SystemCoreClockUpdate ();
    }

static int soc_init (void)
    {
    SystemInit ();
    HAL_Init ();
    SystemClock_Config ();

    __HAL_RCC_GPIOA_CLK_ENABLE ();
    __HAL_RCC_GPIOB_CLK_ENABLE ();
    __HAL_RCC_GPIOC_CLK_ENABLE ();
    __HAL_RCC_GPIOH_CLK_ENABLE ();

    return 0;
    }

MODULE_INIT (board, soc_init);
