#ifndef __DELAY_H
#define __DELAY_H

#define  DWT_CR      *(volatile unsigned int *)0xE0001000
#define  DWT_CYCCNT  *(volatile unsigned int *)0xE0001004
#define  DEM_CR      *(volatile unsigned int *)0xE000EDFC
#define  DEM_CR_TRCENA                   (1 << 24)
#define  DWT_CR_CYCCNTENA                (1 <<  0)
void DelayInit(void);
void Delayus(unsigned int usec);
#define Delayms(msec)         Delayus(msec*1000) 
void delay_us(unsigned int us);
void delay_ms(unsigned int ms);
#endif
