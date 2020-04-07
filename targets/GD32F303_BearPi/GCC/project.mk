HAL_DRIVER_SRC =  \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_adc.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_bkp.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_can.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_crc.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_ctc.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_dac.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_dbg.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_dma.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_enet.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_exmc.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_exti.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_fmc.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_fwdgt.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_gpio.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_i2c.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_misc.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_pmu.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_rcu.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_rtc.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_sdio.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_spi.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_timer.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_usart.c \
        $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_wwdgt.c 
        C_SOURCES += $(HAL_DRIVER_SRC)

HARDWARE_SRC =  \
        ${wildcard $(TOP_DIR)/targets/GD32F303_BearPi/Hardware/Src/*.c} 

        C_SOURCES += $(HARDWARE_SRC)
        
USER_SRC =  \
		$(TOP_DIR)/targets/GD32F303_BearPi/Src/write.c \
        $(TOP_DIR)/targets/GD32F303_BearPi/Src/main.c \
        $(TOP_DIR)/targets/GD32F303_BearPi/Src/system_gd32f30x.c \
        $(TOP_DIR)/targets/GD32F303_BearPi/Src/gd32f30x_it.c \
        $(TOP_DIR)/targets/GD32F303_BearPi/Src/systick.c \
        $(TOP_DIR)/targets/GD32F303_BearPi/Src/BearPi-IoT_gd32f303.c \
        $(TOP_DIR)/targets/GD32F303_BearPi/Src/uart_debug.c \
        $(TOP_DIR)/targets/GD32F303_BearPi/Src/Huawei_IoT_QR_Code.c
        C_SOURCES += $(USER_SRC)  
        
ifeq ($(CONFIG_AT_ENABLE),y)
    UART_AT_SRC = $(TOP_DIR)/targets/GD32F303_BearPi/uart_at/uart_at.c
    C_SOURCES += $(UART_AT_SRC)
endif

 OS_CONFIG_INC = \
        -I $(TOP_DIR)/targets/GD32F303_BearPi/OS_CONFIG
        C_INCLUDES += $(OS_CONFIG_INC)       
# C includes
HAL_DRIVER_INC = \
        -I $(SDK_DIR)/drivers/third_party/GigaDevice/GD32F30x_standard_peripheral/Include 
        C_INCLUDES += $(HAL_DRIVER_INC)        
HARDWARE_INC = \
        -I $(TOP_DIR)/targets/GD32F303_BearPi/Hardware/Inc
        C_INCLUDES += $(HARDWARE_INC)

USER_INC = \
        -I $(TOP_DIR)/targets/GD32F303_BearPi/Inc
        C_INCLUDES += $(USER_INC)
        
# C defines
C_DEFS +=  -D GD32F30X_XD -D NDEBUG

C_INCLUDES += -I $(TOP_DIR)/targets/GD32F303_BearPi



                 