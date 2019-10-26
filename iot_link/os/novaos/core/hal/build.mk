src-prefix             := src
src-y                  += hal_int.c hal_timer.c
src-$(CONFIG_HAL_UART) += hal_uart.c
src-$(CONFIG_HAL_I2C)  += hal_i2c.c
inc-g-y                += h