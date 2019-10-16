

///< this is the light intensity sensor
#include  <stm32l4xx_hal.h>
#include  <dwt_delay.h>

#include "bh1750.h"


#define I2C_OWN_ADDRESS                            0x0A

#define I2C_WR          0
#define I2C_RD          1

#define I2C_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOC_CLK_ENABLE()
#define I2C_GPIO_PORT                       GPIOC
#define I2C_SCL_PIN                         GPIO_PIN_11
#define I2C_SDA_PIN                         GPIO_PIN_10

#define HIGH    1
#define LOW     0


#define I2C_SCL(n) (n?HAL_GPIO_WritePin(I2C_GPIO_PORT,I2C_SCL_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(I2C_GPIO_PORT,I2C_SCL_PIN,GPIO_PIN_RESET)) //SCL
#define I2C_SDA(n) (n?HAL_GPIO_WritePin(I2C_GPIO_PORT,I2C_SDA_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(I2C_GPIO_PORT,I2C_SDA_PIN,GPIO_PIN_RESET)) //SDA
#define I2C_SDA_READ()               HAL_GPIO_ReadPin(I2C_GPIO_PORT,I2C_SDA_PIN)

#define BH1750_Addr 0x46
#define BH1750_ON   0x01
#define BH1750_CON  0x10
#define BH1750_ONE  0x20
#define BH1750_RSET 0x07

#define SDA_IN()  {GPIOC->MODER&=~(3<<(10*2));GPIOC->MODER|=0<<10*2;}
#define SDA_OUT() {GPIOC->MODER&=~(3<<(10*2));GPIOC->MODER|=1<<10*2;}


void    I2C_Start(void);
void    I2C_Stop(void);
uint8_t I2C_WaitAck(void);
void    I2C_Ack(void);
void    I2C_NAck(void);
void    I2C_SendByte(uint8_t Byte);
uint8_t I2C_ReadByte(uint8_t  ack);
uint8_t I2C_CheckDevice(uint8_t _Address);


static void I2C_InitGPIO(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    I2C_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin = I2C_SCL_PIN|I2C_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH ;
    HAL_GPIO_Init(I2C_GPIO_PORT, &GPIO_InitStruct);

    I2C_SCL(HIGH);
    I2C_SDA(HIGH);
}


static void I2C_Start(void)
{
	SDA_OUT();
	I2C_SDA(HIGH); 
	I2C_SCL(HIGH); 
	dwt_delay_us(4);
	I2C_SDA(LOW); 
	dwt_delay_us(4);
	I2C_SCL(LOW); 
}


static void I2C_Stop(void)
{
	SDA_OUT();
	I2C_SCL(LOW); 
	I2C_SDA(LOW); 
	dwt_delay_us(4);
	I2C_SCL(HIGH); 	
	I2C_SDA(HIGH); 
	dwt_delay_us(4);
}


static uint8_t I2C_WaitAck(void)
{
    uint8_t ucErrTime=0;
    SDA_IN();
    I2C_SDA(HIGH);
    dwt_delay_us(4);
    I2C_SCL(HIGH);
    dwt_delay_us(4);
    while (I2C_SDA_READ())
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            I2C_Stop();
            return 1;
        }
    }

    I2C_SCL(LOW);
    return 0;
}


static void I2C_Ack(void)
{

	I2C_SCL(LOW); 
	SDA_OUT(); 
	I2C_SDA(LOW);
	dwt_delay_us(2);
	I2C_SCL(HIGH);
	dwt_delay_us(2);
	I2C_SCL(LOW); 
}


static void I2C_NAck(void)
{
	I2C_SCL(LOW); 
	SDA_OUT();
	I2C_SDA(HIGH);
	dwt_delay_us(2);
	I2C_SCL(HIGH);
	dwt_delay_us(2);
	I2C_SCL(LOW); 
}


static void I2C_SendByte(uint8_t Byte)
{
    uint8_t t;
      
    SDA_OUT();
    I2C_SCL(LOW);
    for (t = 0; t < 8; t++)  
    {  
        I2C_SDA((Byte & 0x80) >> 7);  
        Byte <<= 1;  
        dwt_delay_us(2);
        I2C_SCL(HIGH);  
        dwt_delay_us(2);
        I2C_SCL(LOW);  
        dwt_delay_us(2);
    } 
}


uint8_t I2C_ReadByte(uint8_t ack)
{
	uint8_t i;
	uint8_t value;
    SDA_IN();
    value = 0;

    for (i = 0; i < 8; i++)
    {  
        I2C_SCL(LOW);  
        dwt_delay_us(2);
        I2C_SCL(HIGH);  
        value <<= 1;  
        if (I2C_SDA_READ() ) value++;  
        dwt_delay_us(1);
    }  
    if (!ack)
    {
        I2C_NAck();
    }
    else
	{
        I2C_Ack();
	}
	return value;
}

uint8_t I2C_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;

	I2C_InitGPIO();
	I2C_Start();

	I2C_SendByte(_Address | I2C_WR);
	ucAck = I2C_WaitAck();
	I2C_Stop();
	return ucAck;
}

static void Cmd_Write_BH1750(uint8_t cmd)
{
    I2C_Start();
    I2C_SendByte(BH1750_Addr+0);
	while(I2C_WaitAck());
	dwt_delay_us(100);
    I2C_SendByte(cmd);
	while(I2C_WaitAck());
	dwt_delay_us(100);
    I2C_Stop();
	dwt_delay_ms(5);
}


static void Start_BH1750(void)
{
	Cmd_Write_BH1750(BH1750_ON);	 //power on
	Cmd_Write_BH1750(BH1750_CON);
}


static int Read_BH1750(void)
{
    int ret = 0;
    uint8_t i;
    uint8_t buf[2]={0};
    I2C_Start();
    I2C_SendByte(BH1750_Addr+1);
	while(I2C_WaitAck());
	for (i=0; i<3; i++)
    {     
        if (i ==3)
        {
            buf[i]=I2C_ReadByte(0);
        }
        else
        {
            buf[i]=I2C_ReadByte(1);
        }
    }

    I2C_Stop();
    dwt_delay_ms(5);

    ret = buf[0];
    ret = ret<<8 +buf[1];

    return ret;
}

void bh1750_init(void)
{
    I2C_InitGPIO();
    Cmd_Write_BH1750(0x01);
}

int bh1750_read(void)
{
    int ret = 0;
	Start_BH1750();
	dwt_delay_ms(180);
	ret = Read_BH1750();
	ret =(ret*5)/6;           ///< value should divide by 1.2
    return ret;
}


