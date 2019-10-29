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

#include <stm32h7xx_hal.h>
#include <system_stm32h7xx.h>

#include <stm32h7xx_hal_conf.h>
#include <stm32h7xx_hal_rcc.h>

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
  
	MODIFY_REG (PWR->CR3,PWR_CR3_SCUEN, 0);

	__HAL_PWR_VOLTAGESCALING_CONFIG (PWR_REGULATOR_VOLTAGE_SCALE1);

	while ((PWR->D3CR & (PWR_D3CR_VOSRDY)) != PWR_D3CR_VOSRDY);
  
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
	RCC_OscInitStruct.HSIState       = RCC_HSI_OFF;
	RCC_OscInitStruct.CSIState       = RCC_CSI_OFF;
	RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;

	RCC_OscInitStruct.PLL.PLLN = 160;
	RCC_OscInitStruct.PLL.PLLM = 5;
	RCC_OscInitStruct.PLL.PLLP = 2;
	RCC_OscInitStruct.PLL.PLLQ = 4;

	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLRGE    = RCC_PLL1VCIRANGE_2;
	if (HAL_RCC_OscConfig (&RCC_OscInitStruct) != HAL_OK)
        {
        _Error_Handler (__FILE__, __LINE__);
        }
  
	RCC_ClkInitStruct.ClockType = ( RCC_CLOCKTYPE_SYSCLK  |
                                    RCC_CLOCKTYPE_HCLK    |
                                    RCC_CLOCKTYPE_D1PCLK1 |
                                    RCC_CLOCKTYPE_PCLK1   |
                                    RCC_CLOCKTYPE_PCLK2   |
                                    RCC_CLOCKTYPE_D3PCLK1);

	RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.SYSCLKDivider  = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider  = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2; 
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2; 
	RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;  
	RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV4; 
    
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
        {
        _Error_Handler (__FILE__, __LINE__);
        }

	__HAL_RCC_CSI_ENABLE () ;
	__HAL_RCC_SYSCFG_CLK_ENABLE () ;  
	HAL_EnableCompensationCell ();
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
