src-y                    += STM32H7xx_HAL_Driver/Src/stm32h7xx_hal.c            \
                            STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cortex.c     \
                            STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr.c        \
                            STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr_ex.c     \
                            CMSIS/Device/ST/STM32H7xx/Source/Templates/system_stm32h7xx.c

src-$(CONFIG_STM32_UART) += STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart.c       \
                            STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart_ex.c

src-$(CONFIG_STM32_I2C)  += STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c.c        \
                            STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c_ex.c

src-$(CONFIG_STM32_DMA)  += STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma.c

src-$(CONFIG_STM32_RCC)  += STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc.c        \
                            STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc_ex.c

src-$(CONFIG_STM32_GPIO) += STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_gpio.c

inc-g-y                  += CMSIS/Device/ST/STM32H7xx/Include
inc-g-y                  += STM32H7xx_HAL_Driver/Inc
inc-g-y                  += .

cflags-l-$(CONFIG_KEIL)  += -Wno-switch-enum -Wno-covered-switch-default -Wno-missing-noreturn -Wno-conversion
cflags-STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc.c += -Wno-embedded-directive