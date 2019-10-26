src-y                    += stm32_common.c
src-$(CONFIG_STM32_ETH)  += stm32_eth.c
src-$(CONFIG_STM32_UART) += stm32_uart.c
src-$(CONFIG_STM32_I2C)  += stm32_i2c.c

cflags-stm32_i2c.c       += -Wno-cast-qual