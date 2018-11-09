#ifndef __OLED_H
#define __OLED_H			  	 
#include "stm32l4xx_hal.h"
#include "stdlib.h"	    	
#define OLED_MODE 0
#define OLED_SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    						  
//-----------------OLED IIC�˿ڶ���----------------  					   
//HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
#define OLED_SCLK_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10, GPIO_PIN_RESET);//SDA IIC�ӿڵ�ʱ���ź�
#define OLED_SCLK_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10, GPIO_PIN_SET);

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11, GPIO_PIN_RESET);//SCL IIC�ӿڵ������ź�
#define OLED_SDIN_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11, GPIO_PIN_SET);

 		     
#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����


//OLED�ӿڿ����ú���  
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void OLED_ShowString(uint8_t x,uint8_t y, uint8_t *p,uint8_t Char_Size);	 
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);



//OLED�������� 
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Set_Pos(unsigned char x, unsigned char y);
#endif  
	 



