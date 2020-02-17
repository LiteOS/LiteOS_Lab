/********************************************************************************
* 文件名称 ：BS_ST_DEMO.c
* 作 者：Bossay IoT 开发组
* 版 本：V1.0
* 编写日期 ：2019-11-01
* 功 能：ST_Demo驱动
*********************************************************************************
* 说 明 ：本案例配套Bossay IoT Box开发板使用
* 网 站 ：edu.ibossay.com
* 电 话 ：4006916913 
*********************************************************************************/

#include "Actuators.h"
#include "GPS.h"
#include "stm32l4xx.h"
#include "stm32l4xx_it.h"
#include "usart.h"
#include "gpio.h"
#include "DEMO.h"



void Init_BS_ST_DEMO(void)
{
	GPS_Init();
	Init_Beep();
	Init_LED();
	Init_GPS_PWR();	
	MX_TIM1_Init();	
}

