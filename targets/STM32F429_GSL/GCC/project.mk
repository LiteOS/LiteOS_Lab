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
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_iwdg.c         \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma2d.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_fmc.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sdram.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_ltdc.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_ltdc_ex.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_eth.c \
        $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rng.c
        C_SOURCES += $(HAL_DRIVER_SRC)
        
# C includes
HAL_DRIVER_INC = \
        -I $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Inc \
        -I $(SDK_DIR)/drivers/third_party/ST/STM32F4xx_HAL_Driver/Inc/Legacy
        C_INCLUDES += $(HAL_DRIVER_INC)          
        

HARDWARE_SRC =  ${wildcard $(TOP_DIR)/targets/STM32F429_GSL/Hardware/Src/*.c}
C_SOURCES += $(HARDWARE_SRC)
        
HARDWARE_INC = -I $(TOP_DIR)/targets/STM32F429_GSL/Hardware/Inc
C_INCLUDES += $(HARDWARE_INC)        
        
        
USER_SRC =  ${wildcard $(TOP_DIR)/targets/STM32F429_GSL/Src/*.c}
C_SOURCES += $(USER_SRC) 
        
USER_INC = -I $(TOP_DIR)/targets/STM32F429_GSL/Inc
C_INCLUDES += $(USER_INC)        
         
OS_CONFIG_INC = -I $(TOP_DIR)/targets/STM32F429_GSL/OS_CONFIG
C_INCLUDES += $(OS_CONFIG_INC)       
              
        
#img 
IMG_SRC =  ${wildcard $(TOP_DIR)/targets/STM32F429_GSL/IMG/*.c}
C_SOURCES += $(IMG_SRC)
       
IMG_INC = -I $(TOP_DIR)/targets/STM32F429_GSL/IMG
C_INCLUDES += $(IMG_INC)

#FONT 
FONT_SRC =  ${wildcard $(TOP_DIR)/targets/STM32F429_GSL/FONT/*.c}
C_SOURCES += $(FONT_SRC)   
    
FONT_INC = -I $(TOP_DIR)/targets/STM32F429_GSL/FONT
C_INCLUDES += $(FONT_INC)           
        
# C defines
C_DEFS +=  -D USE_HAL_DRIVER -D STM32F429xx -D NDEBUG


                 