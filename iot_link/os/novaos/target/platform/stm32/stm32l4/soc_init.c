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

#include <stm32l4xx_hal.h>
#include <system_stm32l4xx.h>

#include <stm32l4xx_hal_conf.h>
#include <stm32l4xx_hal_rcc.h>

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
    RCC_OscInitTypeDef       RCC_OscInitStruct;
    RCC_ClkInitTypeDef       RCC_ClkInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /* Configure LSE Drive Capability */

    HAL_PWR_EnableBkUpAccess ();

    __HAL_RCC_LSEDRIVE_CONFIG (RCC_LSEDRIVE_LOW);

    /* Initializes the CPU, AHB and APB busses clocks */
    
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI |
                                            RCC_OSCILLATORTYPE_LSE |
                                            RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.LSEState            = RCC_LSE_ON;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = 16;
    RCC_OscInitStruct.MSIState            = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = 0;
    RCC_OscInitStruct.MSIClockRange       = RCC_MSIRANGE_6;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM            = 1;
    RCC_OscInitStruct.PLL.PLLN            = 40;
    RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ            = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR            = RCC_PLLR_DIV2;
    
    if (HAL_RCC_OscConfig (&RCC_OscInitStruct) != HAL_OK)
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
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig (&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
        {
        _Error_Handler (__FILE__, __LINE__);
        }

    PeriphClkInit.PeriphClockSelection    = RCC_PERIPHCLK_USART1  |
                                            RCC_PERIPHCLK_USART2  |
                                            RCC_PERIPHCLK_USART3  |
                                            RCC_PERIPHCLK_LPUART1 |
                                            RCC_PERIPHCLK_I2C1    |
                                            RCC_PERIPHCLK_I2C2    |
                                            RCC_PERIPHCLK_I2C3    |
                                            RCC_PERIPHCLK_SDMMC1  |
                                            RCC_PERIPHCLK_ADC;
    PeriphClkInit.Usart1ClockSelection    = RCC_USART1CLKSOURCE_PCLK2;
    PeriphClkInit.Usart2ClockSelection    = RCC_USART2CLKSOURCE_PCLK1;
    PeriphClkInit.Usart3ClockSelection    = RCC_USART3CLKSOURCE_PCLK1;
    PeriphClkInit.Lpuart1ClockSelection   = RCC_LPUART1CLKSOURCE_HSI;
    PeriphClkInit.I2c1ClockSelection      = RCC_I2C1CLKSOURCE_PCLK1;
    PeriphClkInit.I2c2ClockSelection      = RCC_I2C2CLKSOURCE_PCLK1;
    PeriphClkInit.I2c3ClockSelection      = RCC_I2C3CLKSOURCE_PCLK1;
    PeriphClkInit.AdcClockSelection       = RCC_ADCCLKSOURCE_PLLSAI1;
    PeriphClkInit.Sdmmc1ClockSelection    = RCC_SDMMC1CLKSOURCE_PLLSAI1;
    PeriphClkInit.PLLSAI1.PLLSAI1Source   = RCC_PLLSOURCE_MSI;
    PeriphClkInit.PLLSAI1.PLLSAI1M        = 1;
    PeriphClkInit.PLLSAI1.PLLSAI1N        = 16;
    PeriphClkInit.PLLSAI1.PLLSAI1P        = RCC_PLLP_DIV7;
    PeriphClkInit.PLLSAI1.PLLSAI1Q        = RCC_PLLQ_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1R        = RCC_PLLR_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK|RCC_PLLSAI1_ADC1CLK;
    
    if (HAL_RCCEx_PeriphCLKConfig (&PeriphClkInit) != HAL_OK)
        {
        _Error_Handler (__FILE__, __LINE__);
        }

    /* Configure the main internal regulator output voltage */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
        {
        _Error_Handler (__FILE__, __LINE__);
        }

    /* Configure the Systick interrupt time */
    //HAL_SYSTICK_Config (HAL_RCC_GetHCLKFreq()/1000);

    /* Configure the Systick */
    //HAL_SYSTICK_CLKSourceConfig (SYSTICK_CLKSOURCE_HCLK);

    /* Enable MSI Auto calibration */
    //HAL_RCCEx_EnableMSIPLLMode ();

    /* SysTick_IRQn interrupt configuration */    
    //HAL_NVIC_SetPriority (SysTick_IRQn, 0, 0);
    }

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed (uint8_t* file, uint32_t line)
    { 
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
    }
#endif /* USE_FULL_ASSERT */

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
