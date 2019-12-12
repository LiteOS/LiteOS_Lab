/********************************************************************************
    * 文件名称 ：E53_IA1.C
    * 作     者：物联网俱乐部
    * 版     本：V1.0
    * 编写日期 ：2019-5-31
    * 功     能：E53_IA1扩展板驱动
*********************************************************************************
    * 说    明 ：本例程配套小熊派-IoT(GD)开发板使用
    *
    * 淘     宝：https://shop336827451.taobao.com/
    * 论     坛：https://bbs.huaweicloud.com/forum/forum-734-1.html
*********************************************************************************/

#include "E53_IA1.h"
#include "gd32f30x.h"
#include "i2c.h"

const int16_t POLYNOMIAL = 0x131;

void delay_us(uint32_t n)
{
    uint8_t j;
    while (n--)
        for (j = 0; j < 10; j++)
            ;
}
void delay_ms(uint32_t n)
{
    while (n--)
        delay_us(1000);
}

void E53_IA1_I2C_Master_Transmit(uint8_t slaveAddr, uint8_t *pBuffer, uint8_t NumByteToRead)
{
    /* wait until I2C bus is idle */
    while (i2c_flag_get(I2C0, I2C_FLAG_I2CBSY))
        ;
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    /* wait until SBSEND bit is set */
    while (!i2c_flag_get(I2C0, I2C_FLAG_SBSEND))
        ;
    /* send slave address to I2C bus */
    i2c_master_addressing(I2C0, slaveAddr, I2C_TRANSMITTER);
    /* wait until ADDSEND bit is set */
    while (!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))
        ;
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
    /* wait until the transmit data buffer is empty */
    while (!i2c_flag_get(I2C0, I2C_FLAG_TBE))
        ;

    for (int i = 0; i < NumByteToRead; i++)
    {
        /* data transmission */
        i2c_data_transmit(I2C0, pBuffer[i]);
        /* wait until the TBE bit is set */
        while (!i2c_flag_get(I2C0, I2C_FLAG_TBE))
            ;
    }
    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C0);
    while (I2C_CTL0(I2C0) & 0x0200)
        ;
}

void E53_IA1_I2C_Master_Receive(uint8_t slaveAddr, uint8_t *pBuffer, uint8_t NumByteToRead)
{
    /* wait until I2C bus is idle */
    while (i2c_flag_get(I2C0, I2C_FLAG_I2CBSY))
        ;
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    /* wait until SBSEND bit is set */
    while (!i2c_flag_get(I2C0, I2C_FLAG_SBSEND))
        ;
    /* send slave address to I2C bus */
    i2c_master_addressing(I2C0, slaveAddr, I2C_TRANSMITTER);
    /* wait until ADDSEND bit is set */
    while (!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))
        ;
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
    /* wait until the transmit data buffer is empty */
    while (!i2c_flag_get(I2C0, I2C_FLAG_TBE))
        ;
    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C0);
    while (I2C_CTL0(I2C0) & 0x0200)
        ;

    /* wait until I2C bus is idle */
    while (i2c_flag_get(I2C0, I2C_FLAG_I2CBSY))
        ;
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    /* wait until SBSEND bit is set */
    while (!i2c_flag_get(I2C0, I2C_FLAG_SBSEND))
        ;
    /* send slave address to I2C bus */
    i2c_master_addressing(I2C0, slaveAddr, I2C_RECEIVER);
    /* wait until ADDSEND bit is set */
    while (!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))
        ;
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);

    /* While there is data to be read */
    while (NumByteToRead)
    {
        if (NumByteToRead == 1)
        {
            /* Disable Acknowledgement */
            i2c_ack_config(I2C0, I2C_ACK_DISABLE);

            /* Send STOP Condition */
            i2c_stop_on_bus(I2C0);
        }
        /* wait until the RBNE bit is set */
        while (!i2c_flag_get(I2C0, I2C_FLAG_RBNE))
            ;
        /* Read a byte from the MPU6050 */
        *pBuffer = i2c_data_receive(I2C0);
        /* Point to the next location where the byte read will be saved */
        pBuffer++;
        /* Decrement the read bytes counter */
        NumByteToRead--;
    }
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
}

/***************************************************************
 * 函数名称: Init_BH1750
 * 说    明: 写命令初始化BH1750
 * 参    数: 无
 * 返 回 值: 无
 ***************************************************************/
void Init_BH1750(void)
{
    uint8_t t_Data = 0x01;
    E53_IA1_I2C_Master_Transmit(BH1750_Addr, &t_Data, 1);
}

/***************************************************************
 * 函数名称: Start_BH1750
 * 说    明: 启动BH1750
 * 参    数: 无
 * 返 回 值: 无
 ***************************************************************/
void Start_BH1750(void)
{
    uint8_t t_Data = 0x10;
    E53_IA1_I2C_Master_Transmit(BH1750_Addr, &t_Data, 1);
}

/***************************************************************
 * 函数名称: Convert_BH1750
 * 说    明: 数值转换
 * 参    数: 无
 * 返 回 值: 光强值
 ***************************************************************/
float Convert_BH1750(void)
{
    float result_lx;
    uint8_t BUF[2];
    int result;
    Start_BH1750();
    delay_ms(180);
    E53_IA1_I2C_Master_Receive(BH1750_Addr + 1, BUF, 2);
    result = BUF[0];
    result = (result << 8) + BUF[1]; // Synthetic Digital Illumination Intensity Data
    result_lx = (float)(result / 1.2);
    return result_lx;
}

/***************************************************************
 * 函数名称: SHT30_reset
 * 说    明: SHT30复位
 * 参    数: 无
 * 返 回 值: 无
 ***************************************************************/
void SHT30_reset(void)
{
    uint8_t SHT3X_Resetcommand_Buffer[2] = {0x30, 0xA2}; // soft reset
    E53_IA1_I2C_Master_Transmit(SHT30_Addr << 1, SHT3X_Resetcommand_Buffer, 2);
    delay_ms(15);
}

/***************************************************************
 * 函数名称: Init_SHT30
 * 说    明: 初始化SHT30，设置测量周期
 * 参    数: 无
 * 返 回 值: 无
 ***************************************************************/
void Init_SHT30(void)
{
    uint8_t SHT3X_Modecommand_Buffer[2] = {0x22, 0x36};                        // periodic mode commands
    E53_IA1_I2C_Master_Transmit(SHT30_Addr << 1, SHT3X_Modecommand_Buffer, 2); // send periodic mode commands
}

/***************************************************************
* 函数名称: SHT3x_CheckCrc
* 说    明: 检查数据正确性
* 参    数: data：读取到的数据
            nbrOfBytes：需要校验的数量
            checksum：读取到的校对比验值
* 返 回 值: 校验结果，0-成功		1-失败
***************************************************************/
uint8_t SHT3x_CheckCrc(char data[], char nbrOfBytes, char checksum)
{

    char crc = 0xFF;
    char bit = 0;
    char byteCtr;

    // calculates 8-Bit checksum with given polynomial
    for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
    {
        crc ^= (data[byteCtr]);
        for (bit = 8; bit > 0; --bit)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ POLYNOMIAL;
            else
                crc = (crc << 1);
        }
    }

    if (crc != checksum)
        return 1;
    else
        return 0;
}

/***************************************************************
 * 函数名称: SHT3x_CalcTemperatureC
 * 说    明: 温度计算
 * 参    数: u16sT：读取到的温度原始数据
 * 返 回 值: 计算后的温度数据
 ***************************************************************/
float SHT3x_CalcTemperatureC(unsigned short u16sT)
{

    float temperatureC = 0; // variable for result

    u16sT &= ~0x0003; // clear bits [1..0] (status bits)
    //-- calculate temperature [℃] --
    temperatureC = (175 * (float)u16sT / 65535 - 45); // T = -45 + 175 * rawValue / (2^16-1)

    return temperatureC;
}

/***************************************************************
 * 函数名称: SHT3x_CalcRH
 * 说    明: 湿度计算
 * 参    数: u16sRH：读取到的湿度原始数据
 * 返 回 值: 计算后的湿度数据
 ***************************************************************/
float SHT3x_CalcRH(unsigned short u16sRH)
{

    float humidityRH = 0; // variable for result

    u16sRH &= ~0x0003; // clear bits [1..0] (status bits)
    //-- calculate relative humidity [%RH] --
    humidityRH = (100 * (float)u16sRH / 65535); // RH = rawValue / (2^16-1) * 10

    return humidityRH;
}

/***************************************************************
 * 函数名称: Init_Motor
 * 说    明: 初始化Init_E53_IA1的马达
 * 参    数: 无
 * 返 回 值: 无
 ***************************************************************/
void Init_Motor(void)
{
    /* enable the led clock */
    rcu_periph_clock_enable(IA1_Motor_GPIO_CLK);
    /* configure led GPIO port */
    gpio_init(IA1_Motor_GPIO_Port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, IA1_Motor_Pin);
    /*configure GPIO pin Output Level */
    gpio_bit_reset(IA1_Motor_GPIO_Port, IA1_Motor_Pin);
}

/***************************************************************
 * 函数名称: Init_Light
 * 说    明: 初始化Init_E53_IA1的补光灯
 * 参    数: 无
 * 返 回 值: 无
 ***************************************************************/
void Init_Light(void)
{
    /* enable the led clock */
    rcu_periph_clock_enable(IA1_Light_GPIO_CLK);
    /* configure led GPIO port */
    gpio_init(IA1_Light_GPIO_Port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, IA1_Light_Pin);
    /*configure GPIO pin Output Level */
    gpio_bit_reset(IA1_Light_GPIO_Port, IA1_Light_Pin);
}

/***************************************************************
 * 函数名称: E53_IA1_Light_StatusSet
 * 说    明: E53_IA1开发板上的LED灯的亮灭控制
 * 参    数: status,LED灯的状态
 *									OFF,关灯
 *									ON,开灯
 * 返 回 值: 无
 ***************************************************************/
void E53_IA1_Light_StatusSet(E53_IA1_Status_ENUM status)
{
    if (status == ON)
        gpio_bit_set(IA1_Light_GPIO_Port, IA1_Light_Pin);
    else
        gpio_bit_reset(IA1_Light_GPIO_Port, IA1_Light_Pin);
}

/***************************************************************
 * 函数名称: E53_IA1_Motor_StatusSet
 * 说    明: E53_IA1开发板上的电机控制
 * 参    数: status,电机的状态
 *									OFF,关电机
 *									ON,开电机
 * 返 回 值: 无
 ***************************************************************/
void E53_IA1_Motor_StatusSet(E53_IA1_Status_ENUM status)
{
    if (status == ON)
        gpio_bit_set(IA1_Motor_GPIO_Port, IA1_Motor_Pin);
    else
        gpio_bit_reset(IA1_Motor_GPIO_Port, IA1_Motor_Pin);
}
/***************************************************************
 * 函数名称: Init_E53_IA1
 * 说    明: 初始化Init_E53_IA1
 * 参    数: 无
 * 返 回 值: 无
 ***************************************************************/
void Init_E53_IA1(void)
{
    /* configure GPIO */
    gpio_config();
    /* configure I2C */
    i2c_config();
    /* init BH1750 */
    Init_BH1750();
    /* init SHT30 */
    Init_SHT30();
    /* init Motor */
    Init_Motor();
    /* init Light*/
    Init_Light();
}

/***************************************************************
 * 函数名称: E53_IA1_Read_Data
 * 说    明: 测量光照强度、温度、湿度
 * 参    数: 无
 * 返 回 值: 无
 ***************************************************************/
void E53_IA1_Read_Data(void)
{

    char data[3]; // data array for checksum verification
    unsigned char addr = 0;
    unsigned short tmp = 0;
    float t = 0;
    uint16_t dat;
    uint8_t SHT3X_Fetchcommand_Bbuffer[2] = {0xE0, 0x00}; // read the measurement results
    uint8_t SHT3X_Data_Buffer[6];                         // byte 0,1 is temperature byte 4,5 is humidity

    E53_IA1_Data.Lux = Convert_BH1750(); // Read bh1750 sensor data

    E53_IA1_I2C_Master_Transmit(SHT30_Addr << 1, SHT3X_Fetchcommand_Bbuffer, 2); // Read sht30 sensor data
    E53_IA1_I2C_Master_Receive((SHT30_Addr << 1) + 1, SHT3X_Data_Buffer, 6);

        /* check tem */
    data[0] = SHT3X_Data_Buffer[0];
    data[1] = SHT3X_Data_Buffer[1];
    data[2] = SHT3X_Data_Buffer[2];

    tmp = SHT3x_CheckCrc(data, 2, data[2]);
    if (!tmp) /* value is ture */
    {
        dat = ((uint16_t)data[0] << 8) | data[1];
        E53_IA1_Data.Temperature = SHT3x_CalcTemperatureC(dat);
    }

        /* check humidity */
    data[0] = SHT3X_Data_Buffer[3];
    data[1] = SHT3X_Data_Buffer[4];
    data[2] = SHT3X_Data_Buffer[5];

    tmp = SHT3x_CheckCrc(data, 2, data[2]);
    if (!tmp) /* value is ture */
    {
        dat = ((uint16_t)data[0] << 8) | data[1];
        E53_IA1_Data.Humidity = SHT3x_CalcRH(dat);
    }
}
