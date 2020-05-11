HAL_DRIVER_SRC =  \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_adc.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_bkp.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_can.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_crc.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_ctc.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_dac.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_dbg.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_dma.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_enet.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_exmc.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_exti.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_fmc.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_fwdgt.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_gpio.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_i2c.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_misc.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_pmu.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_rcu.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_rtc.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_sdio.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_spi.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_timer.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_usart.c \
        $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Source/gd32f30x_wwdgt.c 
        C_SOURCES += $(HAL_DRIVER_SRC)

HARDWARE_SRC =  \
        ${wildcard $(TARGET_DIR)/Hardware/Src/*.c} 

        C_SOURCES += $(HARDWARE_SRC)
        
USER_SRC =  \
		$(TARGET_DIR)/Src/write.c \
        $(TARGET_DIR)/Src/main.c \
        $(TARGET_DIR)/Src/system_gd32f30x.c \
        $(TARGET_DIR)/Src/gd32f30x_it.c \
        $(TARGET_DIR)/Src/systick.c \
        $(TARGET_DIR)/Src/BearPi-IoT_gd32f303.c \
        $(TARGET_DIR)/Src/uart_debug.c \
        $(TARGET_DIR)/Src/i2c.c \
        $(TARGET_DIR)/Src/Huawei_IoT_QR_Code.c
        C_SOURCES += $(USER_SRC)  
        
OS_CONFIG_INC = \
        -I $(TARGET_DIR)/OS_CONFIG
        C_INCLUDES += $(OS_CONFIG_INC)       
# C includes
HAL_DRIVER_INC = \
        -I $(DRIVERLIB_DIR)/third_party/GigaDevice/GD32F30x_standard_peripheral/Include 
        C_INCLUDES += $(HAL_DRIVER_INC)        
HARDWARE_INC = \
        -I $(TARGET_DIR)/Hardware/Inc
        C_INCLUDES += $(HARDWARE_INC)

USER_INC = \
        -I $(TARGET_DIR)/Inc
        C_INCLUDES += $(USER_INC)
        
# C defines
C_DEFS +=  -D GD32F30X_XD -D NDEBUG
C_INCLUDES += -I $(TARGET_DIR)

include $(TARGET_DIR)/Demos/build.mk
include $(TARGET_DIR)/Hardware/build.mk

 




                 