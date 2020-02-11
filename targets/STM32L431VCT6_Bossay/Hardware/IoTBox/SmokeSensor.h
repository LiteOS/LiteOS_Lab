/********************************************************************************
* 文件名称 ：SmokeSensor.h
* 作 者：Bossay IoT 开发组
* 版 本：V1.0
* 编写日期 ：2019-11-01
* 功 能：IoTBox 烟感驱动
*********************************************************************************
* 说 明 ：本案例配套Bossay IoT Box开发板使用
* 网 站 ：edu.ibossay.com
* 电 话 ：4006916913 
*********************************************************************************/
#ifndef __SmokeSensor_H__
#define __SmokeSensor_H__
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/*烟雾传感器数据类型定义 ------------------------------------------------------------*/
typedef struct
{
	int Smoke_Value;
} IoTBox_Smoke_Data_TypeDef;

extern IoTBox_Smoke_Data_TypeDef IoTBox_Smoke_Data;
extern ADC_HandleTypeDef hadc1;
void IoTBox_Smoke_Read_Data(void);


extern IoTBox_Smoke_Data_TypeDef IoTBox_Smoke_Data;
extern ADC_HandleTypeDef hadc1;
void IoTBox_Smoke_Read_Data(void);


#endif

