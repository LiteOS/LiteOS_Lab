HAL_DRIVER_SRC =  \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_iwdg.c
        C_SOURCES += $(HAL_DRIVER_SRC)

HARDWARE_SRC =  \
        ${wildcard $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Hardware/Src/*.c}
        C_SOURCES += $(HARDWARE_SRC)
HAL_DRIVER_SRC_NO_BOOTLOADER =  \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_eth.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rng.c
        C_SOURCES += $(HAL_DRIVER_SRC_NO_BOOTLOADER) 
        
USER_SRC =  \
        $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/main.c \
        $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/system_stm32f4xx.c \
        $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/usart.c \
        $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/uart_debug.c        
C_SOURCES += $(USER_SRC)  

ifeq ($(CONFIG_PB11PA2_UARTAT),y)
    C_SOURCES += $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/uart_at/uart_at.c
endif    

ifeq ($(CONFIG_PB11PA2_MAC),y)

	ifeq ($(CONFIG_LWIP_ENABLE), y)
    	C_SOURCES += $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/ethernet/eth.c \
                 $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/ethernet/net_driver.c
	endif

              
endif 


OS_CONFIG_INC = -I $(TOP_DIR)/targets/STM32F429IGTx_FIRE/OS_CONFIG
C_INCLUDES += $(OS_CONFIG_INC)       
# C includes
HAL_DRIVER_INC = \
        -I $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Inc \
        -I $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Inc/Legacy
        C_INCLUDES += $(HAL_DRIVER_INC)        
HARDWARE_INC = \
        -I $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Hardware/Inc
        C_INCLUDES += $(HARDWARE_INC)

USER_INC = \
        -I $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Inc
        C_INCLUDES += $(USER_INC)
        
# C defines
C_DEFS +=  -D USE_HAL_DRIVER -D STM32F429xx -D NDEBUG
C_INCLUDES += -I $(TOP_DIR)/targets/STM32F429IGTx_FIRE

ifeq ($(CONFIG_OTA_ENABLE),y)
    C_SOURCES += ${wildcard $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/ota_flash/*.c}
    C_INCLUDES += -I $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/ota_flash
endif

ifeq ($(CONFIG_OTA_LOADER_ENABLE), y)
    C_SOURCES += $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/loader.c
endif 


ifeq ($(CONFIG_OTA_APP_ENABLE), y)
    C_SOURCES += $(TOP_DIR)/targets/STM32F429IGTx_FIRE/Src/app_mqttota.c
endif 




                 
