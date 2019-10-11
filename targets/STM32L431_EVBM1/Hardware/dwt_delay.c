

///< this is the delay function,we use the dwt counter to do the delay, which
///< will not reference the system task
#include "stm32l4xx_hal.h"
#include "dwt_delay.h"

extern uint32_t SystemCoreClock;      ///< the system frequency which define and update by the SystemCoreClockUpdate function

#define  DWT_CR                          *(volatile unsigned int *)0xE0001000
#define  DWT_CYCCNT                      *(volatile unsigned int *)0xE0001004
#define  DEM_CR                          *(volatile unsigned int *)0xE000EDFC
#define  DEM_CR_TRCENA                   (1 << 24)
#define  DWT_CR_CYCCNTENA                (1 <<  0)


///< initialize the dwt function
void dwt_delay_init(void)
{
    DEM_CR   |=  DEM_CR_TRCENA;
    DWT_CR   |=  DWT_CR_CYCCNTENA;
}

///< delay for the us
void dwt_delay_us(unsigned int us)
{
    unsigned int temp;
    temp= DWT_CYCCNT;
    us *=  SystemCoreClock/1000000;
    while((unsigned int)( DWT_CYCCNT - temp)< us);
}


///< delay for the ms, which call the us function, which avoid the integer overlap for dwt
void dwt_delay_ms(unsigned int ms)
{
    while(ms > 0)
    {
        dwt_delay_us(1000);
    }
}
