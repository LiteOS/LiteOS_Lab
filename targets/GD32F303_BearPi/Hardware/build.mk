ifeq ($(CONFIG_ATUART_ENABLE), y)	
	C_SOURCES += ${wildcard $(TARGET_DIR)/Hardware/UART_AT/*.c}
    C_INCLUDES += -I $(TARGET_DIR)/Hardware/UART_AT
endif

ifeq ($(CONFIG_LCD_ENABLE), y)	
	C_SOURCES += ${wildcard $(TARGET_DIR)/Hardware/LCD/*.c}
    C_INCLUDES += -I $(TARGET_DIR)/Hardware/LCD
endif
	
	
