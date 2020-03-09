/********************************************************************************
* 文件名称 ：GPS.h
* 作 者：Bossay IoT 开发组
* 版 本：V1.0
* 编写日期 ：2019-11-01
* 功 能：IoTBox GPS驱动
*********************************************************************************
* 说 明 ：本案例配套Bossay IoT Box开发板使用
* 网 站 ：edu.ibossay.com
* 电 话 ：4006916913 
*********************************************************************************/
#ifndef __GPS_H__
#define __GPS_H__
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32l4xx_hal.h"


/***************************************************\
*GPS NMEA-0183协议重要参数结构体定义
*卫星信息
\***************************************************/
typedef struct
{
	uint32_t latitude_bd;					//纬度   分扩大100000倍，实际要除以100000
	uint8_t nshemi_bd;						//北纬/南纬,N:北纬;S:南纬	
	uint32_t longitude_bd;			  //经度 分扩大100000倍,实际要除以100000
	uint8_t ewhemi_bd;						//东经/西经,E:东经;W:西经
}gps_msg;

/* GPS传感器数据类型定义 ------------------------------------------------------------*/
typedef struct
{
		float    Longitude;				//经度
		float    Latitude;        //纬度
} IoTBox_GPS_Data_TypeDef;

typedef struct
{
		char    Longitude[9];				//经度
		char    Latitude[8];        //纬度
} IoTBox_GPS_Data_Send_TypeDef;

extern IoTBox_GPS_Data_TypeDef IoTBox_GPS_Data;
extern IoTBox_GPS_Data_Send_TypeDef IoTBox_GPS_Data_Send;

void Init_GPS_PWR();	
void GPS_Init();
void IoTBox_GPS_Read_Data(void);

#endif

