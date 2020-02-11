/********************************************************************************
* 文件名称 ：BEEP.c
* 作 者：Bossay IoT 开发组
* 版 本：V1.0
* 编写日期 ：2019-11-01
* 功 能：IoTBox 扬声器驱动
*********************************************************************************
* 说 明 ：本案例配套Bossay IoT Box开发板使用
* 网 站 ：edu.ibossay.com
* 电 话 ：4006916913 
*********************************************************************************/
#include "Actuators.h"
#include "stm32l4xx.h"
#include "stm32l4xx_hal.h"
#include "gpio.h"
#include <stdio.h>

TIM_HandleTypeDef htim1;

/***************************************************************
* 函数名称: Init_Beep
* 说    明: 初始化IoTBox的蜂鸣器
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_Beep(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  BEEP_GPIO_CLK_ENABLE();

	 /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);
	
	 /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = BEEP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BEEP_GPIO_Port, &GPIO_InitStruct);
}

/***************************************************************
* 函数名称: MX_TIM1_Init
* 说    明: 初始化定时器16
* 参    数: 无
* 返 回 值: 无
***************************************************************/
/* TIM16 init function */
void MX_TIM1_Init(void)
{
  TIM_OC_InitTypeDef sConfigOC;
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 79;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 499;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  HAL_TIM_MspPostInit(&htim1);

}
/***************************************************************
* 函数名称: HAL_TIM_Base_MspInit
* 说    明: 使能定时器16时钟,设置时钟源
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

	if(tim_baseHandle->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM16_MspInit 0 */

  /* USER CODE END TIM16_MspInit 0 */
    /* TIM16 clock enable */
    __HAL_RCC_TIM1_CLK_ENABLE();
  /* USER CODE BEGIN TIM16_MspInit 1 */

  /* USER CODE END TIM16_MspInit 1 */
  }
}

/***************************************************************
* 函数名称: HAL_TIM_MspPostInit
* 说    明: 使能定时器16硬件初始化
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(htim->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM16_MspPostInit 0 */

  /* USER CODE END TIM16_MspPostInit 0 */
  
    /**TIM16 GPIO Configuration    
    PA8     ------> TIM16_CH1 
    */
    GPIO_InitStruct.Pin = BEEP_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(BEEP_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM16_MspPostInit 1 */

  /* USER CODE END TIM16_MspPostInit 1 */
  }

}



/***************************************************************
* 函数名称: IoTBox_Beep_StatusSet
* 说    明: IoTBox蜂鸣器报警与否
* 参    数: status,ENUM枚举的数据
*									OFF,蜂鸣器
*									ON,开蜂鸣器
* 返 回 值: 无
***************************************************************/
void IoTBox_Beep_StatusSet(Actuators_Status_ENUM status)
{
	if(status == ON)
		HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	if(status == OFF)
		HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_1);
}


/***************************************************************
* 函数名称: IoTBox_Beep_duration
* 说    明: IoTBox蜂鸣器报警时间
* 参    数: duration 报警维持时间
* 返 回 值: 无
***************************************************************/
void IoTBox_Beep_duration(int duration)
{
  printf("beep!!\r\n");
  printf("duration is %d!\r\n",duration);
  HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
   printf("beep!!\r\n");
  HAL_Delay(duration);
  HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_1);
}
