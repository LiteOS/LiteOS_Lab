HAL_DRIVER_SRC =  \
        $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/system_gd32vf103.c        \
        $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Source/gd32vf103_eclic.c  \
        $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Source/gd32vf103_exmc.c   \
        $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Source/gd32vf103_exti.c   \
        $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Source/gd32vf103_gpio.c   \
        $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Source/gd32vf103_timer.c  \
        $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Source/gd32vf103_usart.c  \
        $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Source/gd32vf103_rcu.c    \
        $(TOP_DIR)/drivers/third_party/GigaDevice/n22/drivers/n22_func.c
        C_SOURCES += $(HAL_DRIVER_SRC)

#        $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Source/gd32vf103_i2c.c    \
         $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Source/gd32vf103_fwdgt.c  \
        $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Source/gd32vf103_pmu.c    \
        $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Source/gd32vf103_rtc.c    \
        $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Source/gd32vf103_spi.c    \
        $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Source/gd32vf103_dma.c    \
        $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Source/gd32vf103_adc.c    \
        $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Source/gd32vf103_bkp.c    \
        $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Source/gd32vf103_can.c    \
        $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Source/gd32vf103_crc.c    \
        $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Source/gd32vf103_dbg.c    \
                $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Source/gd32vf103_fmc.c    \
        $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Source/gd32vf103_dac.c    



TARGET_LIB_SRC = \
        $(TOP_DIR)/targets/GD32VF103V_EVAL/Lib/close.c      \
        $(TOP_DIR)/targets/GD32VF103V_EVAL/Lib/fstat.c      \
        $(TOP_DIR)/targets/GD32VF103V_EVAL/Lib/isatty.c     \
        $(TOP_DIR)/targets/GD32VF103V_EVAL/Lib/lseek.c      \
        $(TOP_DIR)/targets/GD32VF103V_EVAL/Lib/read.c       \
        $(TOP_DIR)/targets/GD32VF103V_EVAL/Lib/sbrk.c       \
        $(TOP_DIR)/targets/GD32VF103V_EVAL/Lib/write.c      \
        $(TOP_DIR)/targets/GD32VF103V_EVAL/Lib/write_hex.c
        C_SOURCES += $(TARGET_LIB_SRC)

        #$(TOP_DIR)/targets/GD32VF103V_EVAL/Lib/_exit.c      \

USER_SRC =  \
        $(TOP_DIR)/targets/GD32VF103V_EVAL/Src/gd32v103v_eval.c \
        $(TOP_DIR)/targets/GD32VF103V_EVAL/Src/gd32v103v_lcd_eval.c \
        $(TOP_DIR)/targets/GD32VF103V_EVAL/Src/picture.c \
        $(TOP_DIR)/targets/GD32VF103V_EVAL/Src/systick.c \
        $(TOP_DIR)/targets/GD32VF103V_EVAL/Src/touch_panel.c \
        $(TOP_DIR)/targets/GD32VF103V_EVAL/Src/lcd_font.c \
        $(TOP_DIR)/targets/GD32VF103V_EVAL/Src/uart_at.c \
        $(TOP_DIR)/targets/GD32VF103V_EVAL/Src/main.c
        C_SOURCES += $(USER_SRC)
        
HANDLERS_SRC =  \
        $(TOP_DIR)/targets/GD32VF103V_EVAL/GCC/handlers.c \
        $(TOP_DIR)/targets/GD32VF103V_EVAL/GCC/init.c
        C_SOURCES += $(HANDLERS_SRC)
        
OS_CONFIG_INC = \
        -I $(TOP_DIR)/targets/GD32VF103V_EVAL/OS_CONFIG
        C_INCLUDES += $(OS_CONFIG_INC)       
# C includes
HAL_DRIVER_INC = \
        -I $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral \
        -I $(TOP_DIR)/drivers/third_party/GigaDevice/GD32VF103_standard_peripheral/Include \
        -I $(TOP_DIR)/drivers/third_party/GigaDevice/n22/drivers
        C_INCLUDES += $(HAL_DRIVER_INC)
        
RISCV_ASM_INC = \
        -I $(TOP_DIR)/drivers/third_party/GigaDevice/n22/drivers
        AS_INCLUDES += $(RISCV_ASM_INC)

USER_INC = \
        -I $(TOP_DIR)/targets/GD32VF103V_EVAL/Inc
        C_INCLUDES += $(USER_INC)

ASM_SOURCES_S += $(TOP_DIR)/targets/GD32VF103V_EVAL/GCC/start.S \
                 $(TOP_DIR)/targets/GD32VF103V_EVAL/GCC/entry.S
        
# C defines
C_DEFS += -D NDEBUG -DUSE_STDPERIPH_DRIVER -DGD32V103V_EVAL
                 
