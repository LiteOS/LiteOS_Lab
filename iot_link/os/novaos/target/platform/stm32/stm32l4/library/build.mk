src-y                    += STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c            \
                            STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c     \
                            STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c        \
                            STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c     \
                            CMSIS/Device/ST/STM32L4xx/Source/Templates/system_stm32l4xx.c

src-$(CONFIG_STM32_UART) += STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c       \
                            STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c

src-$(CONFIG_STM32_I2C)  += STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c        \
                            STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c

src-$(CONFIG_STM32_DMA)  += STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c

src-$(CONFIG_STM32_RCC)  += STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c        \
                            STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c

src-$(CONFIG_STM32_GPIO) += STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c

inc-g-y                  += CMSIS/Device/ST/STM32L4xx/Include
inc-g-y                  += STM32L4xx_HAL_Driver/Inc
inc-g-y                  += .

cflags-l-$(CONFIG_KEIL)  += -Wno-switch-enum -Wno-covered-switch-default -Wno-missing-noreturn -Wno-conversion
cflags-STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c +=  -Wno-embedded-directive