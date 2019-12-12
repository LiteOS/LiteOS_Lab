/********************************************************************************
    * 文件名称 ：E53_SF1.h
    * 作     者：物联网俱乐部
    * 版     本：V1.0
    * 编写日期 ：2019-5-31
    * 功     能：E53_SF1扩展板驱动
*********************************************************************************
    * 说    明 ：本例程配套小熊派-IoT(GD)开发板使用
    *
    * 淘     宝：https://shop336827451.taobao.com/
    * 论     坛：https://bbs.huaweicloud.com/forum/forum-734-1.html
*********************************************************************************/

#include "E53_SF1.h"
#include "gd32f30x.h"
#include "gd32f30x_it.h"



void delay_us(uint32_t n)
{
	uint8_t j;
	while(n--)
	for(j=0;j<10;j++);
}
void delay_ms(uint32_t n)
{
	while(n--)
	delay_us(1000);
}

/**
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
  */
void beep_gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    
    /*Configure PB0(TIMER3_CH2) as alternate function*/
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
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
    timer_initpara.prescaler         = 119;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 500;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER3,&timer_initpara);

     /* CH0 configuration in PWM mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER3,TIMER_CH_2,&timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER3,TIMER_CH_2,0);
    timer_channel_output_mode_config(TIMER3,TIMER_CH_2,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER3,TIMER_CH_2,TIMER_OC_SHADOW_DISABLE);

    timer_primary_output_config(TIMER3,ENABLE);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER3);
    timer_enable(TIMER3);
}

/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{    
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV2);
    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0,ADC_DATAALIGN_RIGHT);
    /* ADC SCAN function enable */
    adc_special_function_config(ADC0,ADC_SCAN_MODE,ENABLE);  

    /* ADC channel length config */
    adc_channel_length_config(ADC0,ADC_INSERTED_CHANNEL,2);
    /* ADC temperature sensor channel config */
    adc_inserted_channel_config(ADC0, 0, ADC_CHANNEL_12, ADC_SAMPLETIME_7POINT5);

    /* ADC external trigger enable */
    adc_external_trigger_config(ADC0,ADC_INSERTED_CHANNEL,ENABLE);
    /* ADC trigger config */
    adc_external_trigger_source_config(ADC0, ADC_INSERTED_CHANNEL, ADC0_1_2_EXTTRIG_INSERTED_NONE);
    
    /* enable ADC interface */
    adc_enable(ADC0);
    delay_ms(1);    
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
}


/***************************************************************
* 函数名称: Init_LED
* 说    明: 初始化E53_SF1的LED灯
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_LED(void)
{
  /* enable the led clock */
  rcu_periph_clock_enable(SF1_LED_GPIO_CLK);
  /* configure led GPIO port */
  gpio_init(SF1_LED_GPIO_Port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SF1_LED_Pin);
	/*configure GPIO pin Output Level */
  gpio_bit_reset(SF1_LED_GPIO_Port, SF1_LED_Pin);

}
/***************************************************************
* 函数名称: Init_E53_SF1
* 说    明: 初始化E53_SF1扩展板
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_E53_SF1(void)
{
  /* configure the GPIO ports */
  beep_gpio_config();

  /* configure the TIMER peripheral */
  timer_config();

  /* configure the ADCperipheral */
  adc_config();

  Init_LED();

}
/***************************************************************
* 函数名称: E53_SF1_Read_Data
* 说    明: 获取烟雾传感器的数据
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void E53_SF1_Read_Data(void)
{
  /* ADC software trigger enable */
  adc_software_trigger_enable(ADC0, ADC_INSERTED_CHANNEL);
  /* delay a time in milliseconds */
  delay_ms(2000);
	E53_SF1_Data.Smoke_Value = ADC_IDATA1(ADC0);
}
/***************************************************************
* 函数名称: E53_SF1_LED_StatusSet
* 说    明: E53_SF1开发板上的LED灯的亮灭控制
* 参    数: status,LED灯的状态
*									OFF,关灯
*									ON,开灯
* 返 回 值: 无
***************************************************************/
void E53_SF1_LED_StatusSet(E53_SF1_Status_ENUM status)
{
  if(status == ON)
    gpio_bit_set(SF1_LED_GPIO_Port, SF1_LED_Pin);
  else
    gpio_bit_reset(SF1_LED_GPIO_Port, SF1_LED_Pin);
}

/***************************************************************
* 函数名称: E53_SF1_Beep_StatusSet
* 说    明: E53_SF1蜂鸣器报警与否
* 参    数: status,ENUM枚举的数据
*									OFF,蜂鸣器
*									ON,开蜂鸣器
* 返 回 值: 无
***************************************************************/
void E53_SF1_Beep_StatusSet(E53_SF1_Status_ENUM status)
{
	if(status == ON)
		timer_channel_output_pulse_value_config(TIMER3,TIMER_CH_2,400);
	if(status == OFF)
		timer_channel_output_pulse_value_config(TIMER3,TIMER_CH_2,0);
}


