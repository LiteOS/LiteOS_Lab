#ifndef __DHT11_H__
#define __DHT11_H__

#include "main.h"
#include "dwt.h"
/************************ DHT11 数据类型定义******************************/
typedef struct
{
	uint8_t  humi_high8bit;                //原始数据：湿度高8位
	uint8_t  humi_low8bit;                 //原始数据：湿度低8位
	uint8_t  temp_high8bit;                 //原始数据：温度高8位
	uint8_t  temp_low8bit;                 //原始数据：温度高8位
	uint8_t  check_sum;                     //校验和
	float    humidity;        //实际湿度
  float    temperature;     //实际温度  
} DHT11_Data_TypeDef;

#define DHT11_Dout_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOC_CLK_ENABLE()
#define DHT11_Dout_PORT                           GPIOC
#define DHT11_Dout_PIN                            GPIO_PIN_13

#define DHT11_Dout_LOW()                          HAL_GPIO_WritePin(DHT11_Dout_PORT,DHT11_Dout_PIN,GPIO_PIN_RESET) 
#define DHT11_Dout_HIGH()                         HAL_GPIO_WritePin(DHT11_Dout_PORT,DHT11_Dout_PIN,GPIO_PIN_SET)
#define DHT11_Data_IN()                           HAL_GPIO_ReadPin(DHT11_Dout_PORT,DHT11_Dout_PIN)

void DHT11_Init( void );
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef * DHT11_Data);
void DHT11_Delay(uint32_t time);
#endif /* __DHT11_H__ */
