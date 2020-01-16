HOME_DIR = $(abspath $(PROJECTBASE)/..)

USER_SRC = \
            ${wildcard $(HOME_DIR)/src/*.c} \
            ${wildcard $(HOME_DIR)/../case/src/*.c} \
            ${wildcard $(HOME_DIR)/../case/src/fs/*.c} \
            ${wildcard $(HOME_DIR)/../case/src/hal/*.c}
C_SOURCES += $(USER_SRC)

USER_INC = -I $(HOME_DIR)/inc  \
           -I $(HOME_DIR)/inc/config \
           -I $(HOME_DIR)/../case/h
C_INCLUDES += $(USER_INC)

TARGET_SRC =$(NOVA_ROOT)/target/platform/stm32/src/stm32_common.c  \
            $(NOVA_ROOT)/target/platform/stm32/src/stm32_uart.c  \
            $(NOVA_ROOT)/target/platform/stm32/stm32f4/soc_init.c  \
            $(NOVA_ROOT)/target/platform/stm32/stm32f4/library/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c  \
            $(NOVA_ROOT)/target/platform/stm32/stm32f4/library/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c  \
            $(NOVA_ROOT)/target/platform/stm32/stm32f4/library/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c  \
            $(NOVA_ROOT)/target/platform/stm32/stm32f4/library/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c  \
            $(NOVA_ROOT)/target/platform/stm32/stm32f4/library/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c  \
            $(NOVA_ROOT)/target/platform/stm32/stm32f4/library/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c  \
            $(NOVA_ROOT)/target/platform/stm32/stm32f4/library/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c  \
            $(NOVA_ROOT)/target/platform/stm32/stm32f4/library/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c  \
            $(NOVA_ROOT)/target/platform/stm32/stm32f4/library/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c  \
            $(NOVA_ROOT)/target/platform/stm32/stm32f4/library/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c  \
            $(NOVA_ROOT)/target/platform/stm32/stm32f4/library/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_eth.c    \
            $(NOVA_ROOT)/target/platform/stm32/stm32f4/library/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rng.c

TARGET_INC = -I $(NOVA_ROOT)/target/platform/stm32/stm32f4/library \
             -I $(NOVA_ROOT)/target/platform/stm32/stm32f4/library/CMSIS/Device/ST/STM32F4xx/Include \
             -I $(NOVA_ROOT)/target/platform/stm32/stm32f4/library/STM32F4xx_HAL_Driver/Inc

C_SOURCES += $(TARGET_SRC)
C_INCLUDES += $(TARGET_INC)
