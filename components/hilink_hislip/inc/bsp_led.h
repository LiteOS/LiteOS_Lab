#ifndef __LED_H
#define	__LED_H

#include "stm32f4xx.h"



//引脚定义
/*******************************************************/
//R 红色灯
#define LED1_PIN                  GPIO_PIN_5                 
#define LED1_GPIO_PORT            GPIOI                      

//G 绿色灯
#define LED2_PIN                  GPIO_PIN_6                 
#define LED2_GPIO_PORT            GPIOI                      

//B 蓝色灯
#define LED3_PIN                  GPIO_PIN_7                 
#define LED3_GPIO_PORT            GPIOI                       
/************************************************************/


/** 控制LED灯亮灭的宏，
	* LED低电平亮，设置ON=0，OFF=1
	* 若LED高电平亮，把宏设置成ON=1 ，OFF=0 即可
	*/
#define ON  0
#define OFF 1


/* 定义控制IO的宏 */
#define LED1_TOGGLE		 HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN)
#define LED1_OFF			 HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_SET)
#define LED1_ON				HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_RESET)

#define LED2_TOGGLE		HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED2_PIN)
#define LED2_OFF			HAL_GPIO_WritePin(LED1_GPIO_PORT, LED2_PIN, GPIO_PIN_SET)
#define LED2_ON				HAL_GPIO_WritePin(LED1_GPIO_PORT, LED2_PIN, GPIO_PIN_RESET)

#define LED3_TOGGLE		HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED3_PIN)
#define LED3_OFF			HAL_GPIO_WritePin(LED1_GPIO_PORT, LED3_PIN, GPIO_PIN_SET)
#define LED3_ON				HAL_GPIO_WritePin(LED1_GPIO_PORT, LED3_PIN, GPIO_PIN_RESET)

/* 基本混色，后面高级用法使用PWM可混出全彩颜色,且效果更好 */

//红
#define LED_RED  \
					LED1_ON;\
					LED2_OFF;\
					LED3_OFF

//绿
#define LED_GREEN		\
					LED1_OFF;\
					LED2_ON;\
					LED3_OFF

//蓝
#define LED_BLUE	\
					LED1_OFF;\
					LED2_OFF;\
					LED3_ON

					
//黄(红+绿)					
#define LED_YELLOW	\
					LED1_ON;\
					LED2_ON;\
					LED3_OFF
//紫(红+蓝)
#define LED_PURPLE	\
					LED1_ON;\
					LED2_OFF;\
					LED3_ON

//青(绿+蓝)
#define LED_CYAN \
					LED1_OFF;\
					LED2_ON;\
					LED3_ON
					
//白(红+绿+蓝)
#define LED_WHITE	\
					LED1_ON;\
					LED2_ON;\
					LED3_ON
					
//黑(全部关闭)
#define LED_RGBOFF	\
					LED1_OFF;\
					LED2_OFF;\
					LED3_OFF		




void LED_GPIO_Config(void);

#endif /* __LED_H */
