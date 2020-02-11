/********************************************************************************
* 文件名称 ：SmokeSenser.c
* 作 者：Bossay IoT 开发组
* 版 本：V1.0
* 编写日期 ：2019-11-01
* 功 能：IoTBox 烟感驱动
*********************************************************************************
* 说 明 ：本案例配套Bossay IoT Box开发板使用
* 网 站 ：edu.ibossay.com
* 电 话 ：4006916913 
*********************************************************************************/

#include "SmokeSensor.h"
#include "stm32l4xx.h"
#include "stm32l4xx_it.h"
#include "gpio.h"

ADC_HandleTypeDef hadc1;

/***************************************************************
* 函数名称: MX_ADC1_Init
* 说    明: 初始化ADC1电压采集通道
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void MX_ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig;

    /**Common config 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.NbrOfDiscConversion = 1;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/***************************************************************
* 函数名称: HAL_ADC_MspInit
* 说    明: 使能ADC时钟,设置时钟源
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* ADC1 clock enable */
    __HAL_RCC_ADC_CLK_ENABLE();
  
    /**ADC1 GPIO Configuration    
    PA6     ------> MQ_ADC
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
}

/***************************************************************
* 函数名称: IoTBox_Smoke_Read_Data
* 说    明: 获取烟雾传感器的数据
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void IoTBox_Smoke_Read_Data(void)
{
	HAL_ADC_Start(&hadc1);	
	HAL_ADC_PollForConversion(&hadc1, 50);
	IoTBox_Smoke_Data.Smoke_Value = HAL_ADC_GetValue(&hadc1);
}