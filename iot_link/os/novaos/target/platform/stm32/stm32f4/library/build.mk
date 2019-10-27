src-y                    += STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c            \
                            STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c     \
                            STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c        \
                            STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c     \
                            CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c

src-$(CONFIG_STM32_UART) += STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c       \
                            STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart_ex.c

src-$(CONFIG_STM32_I2C)  += STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c        \
                            STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c

src-$(CONFIG_STM32_DMA)  += STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c

src-$(CONFIG_STM32_RCC)  += STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c        \
                            STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c

src-$(CONFIG_STM32_GPIO) += STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c

src-$(CONFIG_STM32_ETH)  += STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_eth.c

inc-g-y                  += CMSIS/Device/ST/STM32F4xx/Include
inc-g-y                  += STM32F4xx_HAL_Driver/Inc
inc-g-y                  += .

cflags-l-$(CONFIG_KEIL)  += -Wno-switch-enum -Wno-covered-switch-default -Wno-missing-noreturn -Wno-conversion
cflags-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c += -Wno-embedded-directive