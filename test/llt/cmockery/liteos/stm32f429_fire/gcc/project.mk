HOME_DIR = $(abspath $(PROJECTBASE)/..)

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

HAL_DRIVER_SRC_NO_BOOTLOADER =  \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_eth.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rng.c
        C_SOURCES += $(HAL_DRIVER_SRC_NO_BOOTLOADER)

USER_SRC =  \
        ${wildcard $(HOME_DIR)/src/*.c}
        C_SOURCES += $(USER_SRC)

CASE_SRC = \
        ${wildcard $(HOME_DIR)/../case/src/*.c}
        C_SOURCES += $(CASE_SRC)

# C includes
HAL_DRIVER_INC = \
        -I $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Inc \
        -I $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Inc/Legacy
        C_INCLUDES += $(HAL_DRIVER_INC)

USER_INC = \
        -I $(HOME_DIR)/inc \
        -I $(HOME_DIR)/inc/config \
        -I $(HOME_DIR)/../case/inc
        C_INCLUDES += $(USER_INC)

# C defines
C_DEFS +=  -D USE_HAL_DRIVER -D STM32F429xx -D NDEBUG
