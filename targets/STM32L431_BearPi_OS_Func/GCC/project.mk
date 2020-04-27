HAL_DRIVER_SRC =  \
        $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c \
        $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c \
        $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c \
        $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c \
        $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c \
        $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c \
        $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c \
        $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c \
        $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c \
        $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c \
        $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c \
        $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c \
        $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c \
        $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c \
        $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.c \
        $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c \
        $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c \
        $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_iwdg.c \
        $(DRIVERLIB_DIR)/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc.c
        C_SOURCES += $(HAL_DRIVER_SRC)

HARDWARE_SRC =  \
        ${wildcard $(TARGET_DIR)/Hardware/Src/*.c} \
        ${wildcard $(TARGET_DIR)/Hardware/LCD/*.c}
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
        $(TARGET_DIR)/Src/spi.c \
        $(TARGET_DIR)/Src/gpio.c \
        $(TARGET_DIR)/Src/Huawei_IoT_QR_Code.c
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
        -I ${wildcard $(TARGET_DIR)/Hardware/LCD}
        C_INCLUDES += $(HARDWARE_INC)

USER_INC = -I $(TARGET_DIR)   \
		   -I $(TARGET_DIR)/Inc
		   
C_INCLUDES += $(USER_INC)
        
# C defines
C_DEFS +=  -D USE_HAL_DRIVER -D STM32L431xx -D NDEBUG

include $(TARGET_DIR)/Demos/user_demo.mk

                 
