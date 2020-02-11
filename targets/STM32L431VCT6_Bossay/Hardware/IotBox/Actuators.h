/********************************************************************************
* 文件名称 ：Actuators.h
* 作 者：Bossay IoT 开发组
* 版 本：V1.0
* 编写日期 ：2019-11-01
* 功 能：IoTBox 执行单元驱动
*********************************************************************************
* 说 明 ：本案例配套Bossay IoT Box开发板使用
* 网 站 ：edu.ibossay.com
* 电 话 ：4006916913 
*********************************************************************************/
#ifndef __Actuators_H__
#define __Actuators_H__
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

extern TIM_HandleTypeDef htim1;

/***************************************************************
* 名		称: GasStatus_ENUM
* 说    明：枚举状态结构体
***************************************************************/
typedef enum
{
	OFF = 0,
	ON
}
Actuators_Status_ENUM;


void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
void Init_Beep(void);
void MX_TIM1_Init(void);
void IoTBox_Beep_StatusSet(Actuators_Status_ENUM status);
void IoTBox_Beep_duration(int duration);

void Init_LED(void);
void IoTBox_LED_StatusSet(Actuators_Status_ENUM status);
void Led_blink(void);

void Init_Light(void);
void IoTBox_Light_StatusSet(Actuators_Status_ENUM Light_status);
void Light_blink(void);

void Init_Motor(void);
void IoTBox_Motor_StatusSet(Actuators_Status_ENUM Motor_status);

#endif