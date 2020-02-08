/********************************************************************************
    * 文件名称 ：E53_IA1.h
    * 作     者：物联网俱乐部
    * 版     本：V1.0
    * 编写日期 ：2019-5-27
    * 功     能：E53_IA1扩展板驱动
*********************************************************************************
    * 说    明 ：本例程配套物联网俱乐部开发板使用
    *
    * 淘     宝：https://shop128001708.taobao.com/
    * 论     坛：bbs.iot-club.cn
*********************************************************************************/
#ifndef __E53_IA1_H__
#define __E53_IA1_H__
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* 控制设备IO口定义 ------------------------------------------------------------*/

#define IA1_Motor_Pin GPIO_PIN_8
#define IA1_Motor_GPIO_Port GPIOB
#define IA1_Motor_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define IA1_Light_Pin GPIO_PIN_0
#define IA1_Light_GPIO_Port GPIOA
#define IA1_Light_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()

/* E53_IA1传感器数据类型定义 ------------------------------------------------------------*/
typedef struct
{
		float    Lux;							//光照强度
		float    Humidity;        //湿度
    float    Temperature;     //温度
} E53_IA1_Data_TypeDef;

extern E53_IA1_Data_TypeDef E53_IA1_Data;

/* 寄存器宏定义 --------------------------------------------------------------------*/
#define I2C_OWN_ADDRESS                            0x0A

#define SHT30_Addr 0x44

#define BH1750_Addr 0x46
#define BH1750_ON   0x01
#define BH1750_CON  0x10
#define BH1750_ONE  0x20
#define BH1750_RSET 0x07

void Init_E53_IA1(void);
void E53_IA1_Read_Data(void);

#endif





