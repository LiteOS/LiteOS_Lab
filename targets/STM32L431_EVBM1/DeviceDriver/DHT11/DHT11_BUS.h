#ifndef __ONEWIRE_DHT11_H__
#define __ONEWIRE_DHT11_H__

#include "stm32l4xx_hal.h"

typedef struct
{
	uint8_t  humi_high8bit;
	uint8_t  humi_low8bit;
	uint8_t  temp_high8bit;
	uint8_t  temp_low8bit;
	uint8_t  check_sum;
    float    humidity;
    float    temperature;
} DHT11_Data_TypeDef;


#define DHT11_Dout_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()
#define DHT11_Dout_PORT                           GPIOA
#define DHT11_Dout_PIN                            GPIO_PIN_11

#define DHT11_Dout_LOW()                          HAL_GPIO_WritePin(DHT11_Dout_PORT,DHT11_Dout_PIN,GPIO_PIN_RESET) 
#define DHT11_Dout_HIGH()                         HAL_GPIO_WritePin(DHT11_Dout_PORT,DHT11_Dout_PIN,GPIO_PIN_SET)
#define DHT11_Data_IN()	                          HAL_GPIO_ReadPin(DHT11_Dout_PORT,DHT11_Dout_PIN)

void DHT11_Init( void );
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef * DHT11_Data);

#endif /* __ONEWIRE_DHT11_H__ */

