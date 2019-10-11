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
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_iwdg.c  \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rng.c
C_SOURCES += $(HAL_DRIVER_SRC)
        
HAL_DRIVER_INC = -I $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Inc   \
				 -I $(SDK_DIR)/drivers/third_party/ST/STM32L4xx_HAL_Driver/Inc/Legacy
C_INCLUDES += $(HAL_DRIVER_INC)

OS_CONFIG_INC = -I $(SDK_DIR)/targets/STM32L431_EVBM1/OS_CONFIG
C_INCLUDES += $(OS_CONFIG_INC)
        
USER_SRC = ${wildcard $(SDK_DIR)/targets/STM32L431_EVBM1/Usr/*.c}
C_SOURCES += $(USER_SRC)

USER_INC = -I $(SDK_DIR)/targets/STM32L431_EVBM1/Usr
C_INCLUDES += $(USER_INC)
        
# C defines
C_DEFS +=  -D USE_HAL_DRIVER -D STM32L431xx -D NDEBUG

include $(SDK_DIR)/iot_link/iot.mk

                 