

#include "DHT11_BUS.h"

#define Delay_ms(x)   HAL_Delay(x)

static void    DHT11_Mode_IPU(void);
static void    DHT11_Mode_Out_PP(void);
static uint8_t DHT11_ReadByte(void);


///< this is used for the dht11 delay
static void DHT11_Delay(uint16_t time)
{
    uint8_t i;

    while(time)
    {
        for (i = 0; i < 10; i++)
        {

        }
        time--;
    }
}

///< used for  the dhtll initialize
void DHT11_Init ( void )
{
    DHT11_Dout_GPIO_CLK_ENABLE();

    DHT11_Mode_Out_PP();

    DHT11_Dout_HIGH();
}

static void DHT11_Mode_IPU(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin   = DHT11_Dout_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    HAL_GPIO_Init(DHT11_Dout_PORT, &GPIO_InitStruct);
}


static void DHT11_Mode_Out_PP(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = DHT11_Dout_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT11_Dout_PORT, &GPIO_InitStruct);
}


static uint8_t DHT11_ReadByte ( void )
{
    uint8_t i, temp=0;


    for(i=0;i<8;i++)
    {
        while(DHT11_Data_IN()==GPIO_PIN_RESET);


        DHT11_Delay(40);

        if(DHT11_Data_IN()==GPIO_PIN_SET)
        {
            while(DHT11_Data_IN()==GPIO_PIN_SET);

            temp|=(uint8_t)(0x01<<(7-i));
        }
        else
        {
            temp&=(uint8_t)~(0x01<<(7-i));
        }
    }
    return temp;
}


uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef *DHT11_Data)
{  
    uint8_t temp;
    uint16_t humi_temp;

    DHT11_Mode_Out_PP();
    
    DHT11_Dout_LOW();

    Delay_ms(18);

    DHT11_Dout_HIGH();

    DHT11_Delay(30);
    
    DHT11_Mode_IPU();
    
    if(DHT11_Data_IN()==GPIO_PIN_RESET)
    {
        while(DHT11_Data_IN()==GPIO_PIN_RESET);

        while(DHT11_Data_IN()==GPIO_PIN_SET);

        DHT11_Data->humi_high8bit= DHT11_ReadByte();
        DHT11_Data->humi_low8bit = DHT11_ReadByte();
        DHT11_Data->temp_high8bit= DHT11_ReadByte();
        DHT11_Data->temp_low8bit = DHT11_ReadByte();
        DHT11_Data->check_sum    = DHT11_ReadByte();

        DHT11_Mode_Out_PP();

        DHT11_Dout_HIGH();

        humi_temp=DHT11_Data->humi_high8bit*100+DHT11_Data->humi_low8bit;
        DHT11_Data->humidity =(float)humi_temp/100;

        humi_temp=DHT11_Data->temp_high8bit*100+DHT11_Data->temp_low8bit;
        DHT11_Data->temperature=(float)humi_temp/100;

        temp = DHT11_Data->humi_high8bit + DHT11_Data->humi_low8bit +
               DHT11_Data->temp_high8bit+ DHT11_Data->temp_low8bit;
        if(DHT11_Data->check_sum==temp)
        {
            return SUCCESS;
        }
        else
          return ERROR;
    }
    else
        return ERROR;
}

