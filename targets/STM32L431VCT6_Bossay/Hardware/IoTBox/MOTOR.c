/********************************************************************************
* 文件名称 ：MOTOR.c
* 作 者：Bossay IoT 开发组
* 版 本：V1.0
* 编写日期 ：2019-11-01
* 功 能：IoTBox 马达驱动
*********************************************************************************
* 说 明 ：本案例配套Bossay IoT Box开发板使用
* 网 站 ：edu.ibossay.com
* 电 话 ：4006916913 
*********************************************************************************/

#include "Actuators.h"
#include "stm32l4xx.h"
#include "stm32l4xx_it.h"
#include "gpio.h"


/***************************************************************
* 函数名称: Init_Motor
* 说    明: 初始化IoTBox的马达
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_Motor(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO Ports Clock Enable */
    MOTOR_GPIO_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(MOTOR_GPIO_Port, MOTOR_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.Pin = MOTOR_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(MOTOR_GPIO_Port, &GPIO_InitStruct);
}


/***************************************************************
* 函数名称: IoTBox_Motor_StatusSet
* 说    明: IoTBox开发板上的MOTOR的开关控制
* 参    数: status,MOTOR的状态
*									OFF,马达通电
*									ON,关闭马达
* 返 回 值: 无
***************************************************************/
void IoTBox_Motor_StatusSet(Actuators_Status_ENUM status)
{
	HAL_GPIO_WritePin(MOTOR_GPIO_Port, MOTOR_Pin, status != ON ? GPIO_PIN_RESET : GPIO_PIN_SET );	
}

