/********************************************************************************
 * 文件名称 ：E53_ST1.c
 * 作     者：物联网俱乐部
 * 版     本：V1.0
 * 编写日期 ：2019-5-27
 * 功     能：E53_ST1扩展板驱动
 *********************************************************************************
 * 说    明 ：本例程配套物联网俱乐部开发板使用
 *
 * 淘     宝：https://shop128001708.taobao.com/
 * 论     坛：bbs.iot-club.cn
 *********************************************************************************/

#include "E53_ST1.h"
#include "gd32f30x.h"
#include <gd32f30x_it.h>
#include "BearPi-IoT_gd32f303.h"
// #include "usart.h"

gps_msg gpsmsg;
static unsigned char gps_uart[1000];
uint8_t rx_buffer[1000];
volatile uint16_t  rx_counter = 0;

/**
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
  */
void beep_gpio_config(void)
{
    rcu_periph_clock_enable(ST1_Beep_GPIO_CLK);

    /*Configure PB0(TIMER3_CH2) as alternate function*/
    gpio_init(ST1_Beep_GPIO_Port, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, ST1_Beep_Pin);
}
/**
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void timer_config(void)
{
    /* TIMER3 configuration: generate PWM signals with different duty cycles:
       TIMER3CLK = SystemCoreClock / 120 = 1MHz */
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER3);
    timer_deinit(TIMER3);

    /* TIMER3 configuration */
    timer_initpara.prescaler = 119;
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = 500;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER3, &timer_initpara);

    /* CH0 configuration in PWM mode */
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER3, TIMER_CH_2, &timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_2, 0);
    timer_channel_output_mode_config(TIMER3, TIMER_CH_2, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER3, TIMER_CH_2, TIMER_OC_SHADOW_DISABLE);

    timer_primary_output_config(TIMER3, ENABLE);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER3);
    timer_enable(TIMER3);
}

/***************************************************************
 * 函数名称: Init_Beep
 * 说    明: 初始化E53_ST1的蜂鸣器
 * 参    数: 无
 * 返 回 值: 无
 ***************************************************************/
void Init_Beep(void)
{
    /* configure the GPIO ports */
    beep_gpio_config();

    /* configure the TIMER peripheral */
    timer_config();
}

/***************************************************************
 * 函数名称: Init_LED
 * 说    明: 初始化E53_ST1的LED灯
 * 参    数: 无
 * 返 回 值: 无
 ***************************************************************/
void Init_LED(void)
{
    /* enable the led clock */
    rcu_periph_clock_enable(ST1_LED_GPIO_CLK);
    /* configure led GPIO port */
    gpio_init(ST1_LED_GPIO_Port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, ST1_LED_Pin);
    /*configure GPIO pin Output Level */
    gpio_bit_set(ST1_LED_GPIO_Port, ST1_LED_Pin);
}
/***************************************************************
 * 函数名称: Init_GPS_POW
 * 说    明: 初始化E53_ST1的GPS使能引脚
 * 参    数: 无
 * 返 回 值: 无
 ***************************************************************/
void Init_GPS_POW(void)
{
    /* enable the led clock */
    rcu_periph_clock_enable(ST1_GPS_POW_GPIO_CLK);
    /* configure led GPIO port */
    gpio_init(ST1_GPS_POW_GPIO_Port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, ST1_GPS_POW_Pin);
    /*configure GPIO pin Output Level */
    gpio_bit_reset(ST1_GPS_POW_GPIO_Port, ST1_GPS_POW_Pin);
}
/***************************************************************
 * 函数名称: GPS_Init
 * 说    明: 初始化GPS模块
 * 参    数: 无
 * 返 回 值: 无
 ***************************************************************/
void GPS_Init(void)
{
    // gd_eval_com_init(EVAL_COM3, 9600);
    // usart_flag_clear(EVAL_COM3,USART_FLAG_TC);
    // // nvic_irq_enable(UART3_IRQn, 0, 0);
    // LOS_HwiCreate(UART3_IRQn, 7,0,UART3_IRQHandler,NULL);	//创建中断
    //     /* enable USART0 receive interrupt */
    // usart_interrupt_enable(UART3, USART_INT_RBNE);

    gd_eval_com_init(EVAL_COM0, 115200);
    // nvic_irq_enable(USART0_IRQn, 0, 0);
    LOS_HwiCreate(USART0_IRQn, 7,0,USART0_IRQHandler,NULL);	//创建中断
        /* enable USART0 receive interrupt */
    usart_interrupt_enable(USART0, USART_INT_RBNE);
}

/***************************************************************
 * 函数名称: Init_E53_ST1
 * 说    明: 初始化E53_ST1扩展板
 * 参    数: 无
 * 返 回 值: 无
 ***************************************************************/
void Init_E53_ST1(void)
{
    GPS_Init();
    Init_Beep();
    Init_LED();
    Init_GPS_POW();
}

/***************************************************\
* 函数名称: NMEA_Comma_Pos
*	函数功能：从buf里面得到第cx个逗号所在的位置
*	输入值：
*	返回值：0~0xFE，代表逗号所在位置的便宜
*				 	0xFF，代表不存在第cx个逗号
\***************************************************/

uint8_t NMEA_Comma_Pos(uint8_t *buf, uint8_t cx)
{
    uint8_t *p = buf;
    while (cx)
    {
        if (*buf == '*' || *buf < ' ' || *buf > 'z')
            return 0xFF;
        if (*buf == ',')
            cx--;
        buf++;
    }
    return buf - p;
}
/***************************************************\
* 函数名称: NMEA_Pow
*	函数功能：返回m的n次方值
*	输入值：底数m和指数n
*	返回值：m^n
\***************************************************/
uint32_t NMEA_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--)
        result *= m;
    return result;
}
/***************************************************\
* 函数名称: NMEA_Str2num
*	函数功能：str数字转换为（int）数字，以','或者'*'结束
*	输入值：buf，数字存储区
*				 	dx，小数点位数，返回给调用函数
*	返回值：转换后的数值
\***************************************************/
int NMEA_Str2num(uint8_t *buf, uint8_t *dx)
{
    uint8_t *p = buf;
    uint32_t ires = 0, fres = 0;
    uint8_t ilen = 0, flen = 0, i;
    uint8_t mask = 0;
    int res;
    while (1)
    {
        if (*p == '-')
        {
            mask |= 0x02;
            p++;
        } //说明有负数
        if (*p == ',' || *p == '*')
            break; //遇到结束符
        if (*p == '.')
        {
            mask |= 0x01;
            p++;
        }                                //遇到小数点
        else if (*p > '9' || (*p < '0')) //数字不在0和9之内，说明有非法字符
        {
            ilen = 0;
            flen = 0;
            break;
        }
        if (mask & 0x01)
            flen++; //小数点的位数
        else
            ilen++; // str长度加一
        p++;        //下一个字符
    }
    if (mask & 0x02)
        buf++;                 //移到下一位，除去负号
    for (i = 0; i < ilen; i++) //得到整数部分数据
    {
        ires += NMEA_Pow(10, ilen - 1 - i) * (buf[i] - '0');
    }
    if (flen > 5)
        flen = 5; //最多取五位小数
    *dx = flen;
    for (i = 0; i < flen; i++) //得到小数部分数据
    {
        fres += NMEA_Pow(10, flen - 1 - i) * (buf[ilen + 1 + i] - '0');
    }
    res = ires * NMEA_Pow(10, flen) + fres;
    if (mask & 0x02)
        res = -res;
    return res;
}
/***************************************************\
* 函数名称: NMEA_BDS_GPRMC_Analysis
*	函数功能：解析GPRMC信息
*	输入值：gpsx,NMEA信息结构体
*				 buf：接收到的GPS数据缓冲区首地址
\***************************************************/
void NMEA_BDS_GPRMC_Analysis(gps_msg *gpsmsg, uint8_t *buf)
{
    uint8_t *p4, dx;
    uint8_t posx;
    uint32_t temp;
    float rs;
    p4 = (uint8_t *)strstr((const char *)buf, "$GPRMC"); //"$GPRMC",经常有&和GPRMC分开的情况,故只判断GPRMC.
    posx = NMEA_Comma_Pos(p4, 3);                        //得到纬度
    if (posx != 0XFF)
    {
        temp = NMEA_Str2num(p4 + posx, &dx);
        gpsmsg->latitude_bd = temp / NMEA_Pow(10, dx + 2);                                              //得到°
        rs = temp % NMEA_Pow(10, dx + 2);                                                               //得到'
        gpsmsg->latitude_bd = gpsmsg->latitude_bd * NMEA_Pow(10, 5) + (rs * NMEA_Pow(10, 5 - dx)) / 60; //转换为°
    }
    posx = NMEA_Comma_Pos(p4, 4); //南纬还是北纬
    if (posx != 0XFF)
        gpsmsg->nshemi_bd = *(p4 + posx);
    posx = NMEA_Comma_Pos(p4, 5); //得到经度
    if (posx != 0XFF)
    {
        temp = NMEA_Str2num(p4 + posx, &dx);
        gpsmsg->longitude_bd = temp / NMEA_Pow(10, dx + 2);                                               //得到°
        rs = temp % NMEA_Pow(10, dx + 2);                                                                 //得到'
        gpsmsg->longitude_bd = gpsmsg->longitude_bd * NMEA_Pow(10, 5) + (rs * NMEA_Pow(10, 5 - dx)) / 60; //转换为°
    }
    posx = NMEA_Comma_Pos(p4, 6); //东经还是西经
    if (posx != 0XFF)
        gpsmsg->ewhemi_bd = *(p4 + posx);
}

/***************************************************************
 * 函数名称: E53_ST1_Read_Data
 * 说    明: 获取GPS经纬度信息
 * 参    数: 无
 * 返 回 值: 无
 ***************************************************************/
void E53_ST1_Read_Data(void)
{
    // HAL_UART_Receive_IT(&huart3,gps_uart,1000);
    printf("GPS data:%s",&rx_buffer);
    NMEA_BDS_GPRMC_Analysis(&gpsmsg, (uint8_t *)rx_buffer); //分析字符串
    E53_ST1_Data.Longitude = (float)((float)gpsmsg.longitude_bd / 100000);
    E53_ST1_Data.Latitude = (float)((float)gpsmsg.latitude_bd / 100000);
}

/***************************************************************
 * 函数名称: E53_ST1_LED_StatusSet
 * 说    明: E53_ST1开发板上的LED灯的亮灭控制
 * 参    数: status,LED灯的状态
 *									OFF,关灯
 *									ON,开灯
 * 返 回 值: 无
 ***************************************************************/
void E53_ST1_LED_StatusSet(E53_ST1_Status_ENUM status)
{
    if (status == ON)
        gpio_bit_set(ST1_LED_GPIO_Port, ST1_LED_Pin);
    else
        gpio_bit_reset(ST1_LED_GPIO_Port, ST1_LED_Pin);
}

/***************************************************************
 * 函数名称: E53_ST1_Beep_StatusSet
 * 说    明: E53_ST1蜂鸣器报警与否
 * 参    数: status,枚举的数据
 *									OFF,关蜂鸣器
 *									ON,开蜂鸣器
 * 返 回 值: 无
 ***************************************************************/
void E53_ST1_Beep_StatusSet(E53_ST1_Status_ENUM status)
{
    if (status == ON)
        timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_2, 400);
    if (status == OFF)
        timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_2, 0);
}
