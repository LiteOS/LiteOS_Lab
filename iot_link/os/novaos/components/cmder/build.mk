src-prefix               := src/
src-$(CONFIG_CMDER)      += cmder.c cmder_mem.c
src-$(CONFIG_CMDER_UART) += cmder_uart.c
src-$(CONFIG_CMDER_TOP)  += cmder_top.c
inc-g-y                  += h/