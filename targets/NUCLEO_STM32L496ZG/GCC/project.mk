

TARGET_DRIVERLIB_DIR := $(DRIVERLIB_DIR)/third_party/ST

HAL_DRIVER_SRC =  \
        $(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Src/stm32l4xx_hal_flash.c \
        $(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Src/stm32l4xx_hal_tim_ex.c \
        $(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Src/stm32l4xx_hal_rcc.c \
        $(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Src/stm32l4xx_hal_pwr_ex.c \
        $(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Src/stm32l4xx_hal_pwr.c \
        $(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Src/stm32l4xx_hal_gpio.c \
        $(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Src/stm32l4xx_hal_rcc_ex.c \
        $(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Src/stm32l4xx_hal_flash_ex.c \
        $(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Src/stm32l4xx_hal_flash_ramfunc.c \
        $(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Src/stm32l4xx_hal_cortex.c \
        $(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Src/stm32l4xx_hal_uart.c \
        $(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Src/stm32l4xx_hal_uart_ex.c \
        $(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Src/stm32l4xx_hal.c \
        $(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Src/stm32l4xx_hal_tim.c \
        $(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Src/stm32l4xx_hal_spi.c \
        $(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Src/stm32l4xx_hal_i2c.c \
        $(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Src/stm32l4xx_hal_i2c_ex.c \
        $(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Src/stm32l4xx_hal_iwdg.c \
        $(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Src/stm32l4xx_hal_adc.c
        C_SOURCES += $(HAL_DRIVER_SRC)


HAL_DRIVER_SRC_NO_BOOTLOADER =  \
        $(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Src/stm32l4xx_hal_dma.c \
        $(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Src/stm32l4xx_hal_dma_ex.c \
        $(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Src/stm32l4xx_hal_rng.c
        C_SOURCES += $(HAL_DRIVER_SRC_NO_BOOTLOADER)

# C includes
HAL_DRIVER_INC = \
        -I$(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Inc \
        -I$(TARGET_DRIVERLIB_DIR)/STM32L4xx_HAL_Driver_V1_11_1/Inc/Legacy \
        -I$(TARGET_DRIVERLIB_DIR)/CMSIS_V1_11_1/Device/ST/STM32L4xx/Include
        C_INCLUDES += $(HAL_DRIVER_INC)
        
        
#INCLUDE THE USER SOURCES

USER_SRC = $(TARGET_DIR)/Src/board.c \
           $(TARGET_DIR)/Src/iot_main.c  \
           $(TARGET_DIR)/Src/uart_debug.c   \
           $(TARGET_DIR)/Src/stm32l4xx_hal_msp.c   \
           $(TARGET_DIR)/Src/stm32l4xx_it.c   \
           $(TARGET_DIR)/Src/system_stm32l4xx.c                   
C_SOURCES += $(USER_SRC)

USER_INC = -I $(TARGET_DIR)   \
           -I $(TARGET_DIR)/Inc
C_INCLUDES += $(USER_INC)

C_INCLUDES += -I $(TARGET_DIR)/OS_CONFIG

                                        
ifeq ($(CONFIG_AT_ENABLE),y)
    C_SOURCES += $(TARGET_DIR)/Src/uart_at.c
endif

ifeq ($(CONFIG_OTA_ENABLE),y)
    C_SOURCES += ${wildcard $(TARGET_DIR)/Src/ota_flash/*.c}
    C_INCLUDES += -I $(TARGET_DIR)/Src/ota_flash
endif

ifeq ($(CONFIG_OTA_LOADER_ENABLE), y)
    C_SOURCES += $(TARGET_DIR)/Src/loader.c
endif 


ifeq ($(CONFIG_OTA_APP_ENABLE), y)
    C_SOURCES += $(TARGET_DIR)/Src/app_mqttota.c
endif 

C_DEFS +=  -D USE_HAL_DRIVER -D STM32L496xx -D NDEBUG




                 
