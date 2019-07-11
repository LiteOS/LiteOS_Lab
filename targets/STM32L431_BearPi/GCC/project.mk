HAL_DRIVER_SRC =  \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_iwdg.c
        C_SOURCES += $(HAL_DRIVER_SRC)

HARDWARE_SRC =  \
        ${wildcard $(TOP_DIR)/targets/STM32L431_BearPi/Hardware/Src/*.c} \
        ${wildcard $(TOP_DIR)/targets/STM32L431_BearPi/Hardware/BH1750/*.c} \
        ${wildcard $(TOP_DIR)/targets/STM32L431_BearPi/Hardware/LCD/*.c}
        C_SOURCES += $(HARDWARE_SRC)
HAL_DRIVER_SRC_NO_BOOTLOADER =  \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rng.c
        C_SOURCES += $(HAL_DRIVER_SRC_NO_BOOTLOADER) 
        
USER_SRC =  \
        $(TOP_DIR)/targets/STM32L431_BearPi/Src/main.c \
        $(TOP_DIR)/targets/STM32L431_BearPi/Src/system_stm32l4xx.c \
        $(TOP_DIR)/targets/STM32L431_BearPi/Src/stm32l4xx_it.c \
        $(TOP_DIR)/targets/STM32L431_BearPi/Src/sys_init.c \
        $(TOP_DIR)/targets/STM32L431_BearPi/Src/usart.c \
        $(TOP_DIR)/targets/STM32L431_BearPi/Src/uart_debug.c \
        $(TOP_DIR)/targets/STM32L431_BearPi/Src/uart_at.c \
        $(TOP_DIR)/targets/STM32L431_BearPi/Src/dwt.c \
        $(TOP_DIR)/targets/STM32L431_BearPi/Src/i2c.c \
        $(TOP_DIR)/targets/STM32L431_BearPi/Src/spi.c \
        $(TOP_DIR)/targets/STM32L431_BearPi/Src/gpio.c \
        $(TOP_DIR)/targets/STM32L431_BearPi/Src/net_driver.c \
        $(TOP_DIR)/targets/STM32L431_BearPi/Src/Huawei_IoT_QR_Code.c
        C_SOURCES += $(USER_SRC)  
        
 OS_CONFIG_INC = \
        -I $(TOP_DIR)/targets/STM32L431_BearPi/OS_CONFIG
        C_INCLUDES += $(OS_CONFIG_INC)       
# C includes
HAL_DRIVER_INC = \
        -I $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Inc \
        -I $(TOP_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Inc/Legacy
        C_INCLUDES += $(HAL_DRIVER_INC)        
HARDWARE_INC = \
        -I ${wildcard $(TOP_DIR)/targets/STM32L431_BearPi/Hardware/Inc} \
        -I ${wildcard $(TOP_DIR)/targets/STM32L431_BearPi/Hardware/BH1750} \
        -I ${wildcard $(TOP_DIR)/targets/STM32L431_BearPi/Hardware/LCD}
        C_INCLUDES += $(HARDWARE_INC)

USER_INC = \
        -I $(TOP_DIR)/targets/STM32L431_BearPi/Inc
        C_INCLUDES += $(USER_INC)
        
# C defines
C_DEFS +=  -D USE_HAL_DRIVER -D STM32L431xx -D NDEBUG
                 