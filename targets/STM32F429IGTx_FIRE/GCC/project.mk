HAL_DRIVER_SRC =  \
        $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_iwdg.c
        C_SOURCES += $(HAL_DRIVER_SRC)

HARDWARE_SRC =  \
        ${wildcard $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Hardware/Src/*.c}
        C_SOURCES += $(HARDWARE_SRC)
HAL_DRIVER_SRC_NO_BOOTLOADER =  \
        $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_eth.c \
        $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rng.c
        C_SOURCES += $(HAL_DRIVER_SRC_NO_BOOTLOADER) 
        
USER_SRC =  \
        $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/main.c \
        $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/system_stm32f4xx.c \
        $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/usart.c \
        $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/uart_debug.c \
        $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/eth.c \
        $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/net_driver.c 
        C_SOURCES += $(USER_SRC)  
        
 OS_CONFIG_INC = \
        -I $(TOP_DIR)/targets/STM32F429IGTx_FIRE/OS_CONFIG
        C_INCLUDES += $(OS_CONFIG_INC)       
# C includes
HAL_DRIVER_INC = \
        -I $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Inc \
        -I $(TOP_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Inc/Legacy
        C_INCLUDES += $(HAL_DRIVER_INC)        
HARDWARE_INC = \
        -I $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Hardware/Inc
        C_INCLUDES += $(HARDWARE_INC)

USER_INC = \
        -I $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Inc
        C_INCLUDES += $(USER_INC)
        
# C defines
C_DEFS +=  -D USE_HAL_DRIVER -D STM32F429xx -D NDEBUG
                 