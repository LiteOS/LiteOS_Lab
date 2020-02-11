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
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_iwdg.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc.c
        C_SOURCES += $(HAL_DRIVER_SRC)

HARDWARE_SRC =  \
        ${wildcard $(TOP_DIR)/targets/STM32L431VCT6_Bossay/Hardware/Src/*.c} \
        ${wildcard $(TOP_DIR)/targets/STM32L431VCT6_Bossay/Hardware/LCD/*.c}
        C_SOURCES += $(HARDWARE_SRC)

HAL_DRIVER_SRC_NO_BOOTLOADER =  \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rng.c
        C_SOURCES += $(HAL_DRIVER_SRC_NO_BOOTLOADER) 
        
USER_SRC =  \
        $(TOP_DIR)/targets/STM32L431VCT6_Bossay/Src/main.c \
        $(TOP_DIR)/targets/STM32L431VCT6_Bossay/Src/system_stm32l4xx.c \
        $(TOP_DIR)/targets/STM32L431VCT6_Bossay/Src/stm32l4xx_it.c \
        $(TOP_DIR)/targets/STM32L431VCT6_Bossay/Src/sys_init.c \
        $(TOP_DIR)/targets/STM32L431VCT6_Bossay/Src/usart.c \
        $(TOP_DIR)/targets/STM32L431VCT6_Bossay/Src/uart_debug.c \
        $(TOP_DIR)/targets/STM32L431VCT6_Bossay/Src/dwt.c \
        $(TOP_DIR)/targets/STM32L431VCT6_Bossay/Src/i2c.c \
        $(TOP_DIR)/targets/STM32L431VCT6_Bossay/Src/spi.c \
        $(TOP_DIR)/targets/STM32L431VCT6_Bossay/Src/gpio.c \
        $(TOP_DIR)/targets/STM32L431VCT6_Bossay/Src/Bossaylogo.c \
        $(TOP_DIR)/targets/STM32L431VCT6_Bossay/Src/Huawei_IoT_QR_Code.c
        C_SOURCES += $(USER_SRC)  
        
ifeq ($(CONFIG_AT_ENABLE),y)
    UART_AT_SRC = $(TOP_DIR)/targets/STM32L431VCT6_Bossay/uart_at/uart_at.c
    C_SOURCES += $(UART_AT_SRC)
endif

 OS_CONFIG_INC = \
        -I $(TOP_DIR)/targets/STM32L431VCT6_Bossay/OS_CONFIG
        C_INCLUDES += $(OS_CONFIG_INC)       
# C includes
HAL_DRIVER_INC = \
        -I $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Inc \
        -I $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Inc/Legacy
        C_INCLUDES += $(HAL_DRIVER_INC)        
HARDWARE_INC = \
        -I ${wildcard $(TOP_DIR)/targets/STM32L431VCT6_Bossay/Hardware/Inc} \
        -I ${wildcard $(TOP_DIR)/targets/STM32L431VCT6_Bossay/Hardware/IoTBox} \
        -I ${wildcard $(TOP_DIR)/targets/STM32L431VCT6_Bossay/Hardware/LCD}
        C_INCLUDES += $(HARDWARE_INC)

USER_INC = \
        -I $(TOP_DIR)/targets/STM32L431VCT6_Bossay/Inc
        C_INCLUDES += $(USER_INC)
        
# C defines
C_DEFS +=  -D USE_HAL_DRIVER -D STM32L431xx -D NDEBUG

include $(SDK_DIR)/iot_link/iot.mk
include $(TOP_DIR)/targets/STM32L431VCT6_Bossay/Demos/user_demo.mk

                 
