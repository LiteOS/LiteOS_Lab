#ifndef __DELAY_H
#define __DELAY_H


void DelayInit(void);
void Delayus(unsigned int usec);
void delay_us(unsigned int us);
void delay_ms(unsigned int ms);


///< you must call the init function first
void dwt_delay_init();
void dwt_delay_us(unsigned int us);
void dwt_delay_ms(unsigned int ms);




#endif
