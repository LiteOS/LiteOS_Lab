HAL_DRIVER_SRC =  \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_iwdg.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc.c
        C_SOURCES += $(HAL_DRIVER_SRC)

HARDWARE_SRC =  \
        ${wildcard $(TARGET_DIR)/Hardware/Src/*.c} \
        ${wildcard $(TARGET_DIR)/Hardware/OLED/*.c} \
        ${wildcard $(TARGET_DIR)/Hardware/DHT11/*.c}
        C_SOURCES += $(HARDWARE_SRC)

HAL_DRIVER_SRC_NO_BOOTLOADER =  \
        $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c \
        $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c \
        $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rng.c
        C_SOURCES += $(HAL_DRIVER_SRC_NO_BOOTLOADER) 
        
USER_SRC =  \
        $(TARGET_DIR)/Src/main.c \
        $(TARGET_DIR)/Src/system_stm32l4xx.c \
        $(TARGET_DIR)/Src/stm32l4xx_it.c \
        $(TARGET_DIR)/Src/sys_init.c \
        $(TARGET_DIR)/Src/usart.c \
        $(TARGET_DIR)/Src/uart_debug.c \
        $(TARGET_DIR)/Src/dwt.c \
        $(TARGET_DIR)/Src/i2c.c \
        $(TARGET_DIR)/Src/gpio.c\
        $(TARGET_DIR)/Src/rtc.c
        C_SOURCES += $(USER_SRC)
        
ifeq ($(CONFIG_AT_ENABLE),y)
    UART_AT_SRC = $(TARGET_DIR)/uart_at/uart_at.c
    C_SOURCES += $(UART_AT_SRC)
endif

 OS_CONFIG_INC = \
        -I $(TARGET_DIR)/OS_CONFIG
        C_INCLUDES += $(OS_CONFIG_INC)
# C includes
HAL_DRIVER_INC = \
        -I $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Inc \
        -I $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Inc/Legacy
        C_INCLUDES += $(HAL_DRIVER_INC)        
HARDWARE_INC = \
        -I ${wildcard $(TARGET_DIR)/Hardware/Inc} \
        -I ${wildcard $(TARGET_DIR)/Hardware/OLED} \
        -I ${wildcard $(TARGET_DIR)/Hardware/DHT11}
        C_INCLUDES += $(HARDWARE_INC)

USER_INC = \
        -I $(TARGET_DIR)/Inc \
        -I $(TARGET_DIR)
        C_INCLUDES += $(USER_INC)
        
# C defines
C_DEFS +=  -D USE_HAL_DRIVER -D STM32L476xx -D NDEBUG

include $(TARGET_DIR)/Demos/user_demo.mk


