ifeq ($(CONFIG_ATUART_ENABLE), y)	
	C_SOURCES += ${wildcard $(TARGET_DIR)/Hardware/UART_AT/*.c}
    C_INCLUDES += -I $(TARGET_DIR)/Hardware/UART_AT
endif

ifeq ($(CONFIG_LCD_ENABLE), y)	
	C_SOURCES += ${wildcard $(TARGET_DIR)/Hardware/LCD/*.c}
    C_INCLUDES += -I $(TARGET_DIR)/Hardware/LCD
endif
	
ifeq ($(CONFIG_E53_IA1), y)	
	C_SOURCES += ${wildcard $(TARGET_DIR)/Hardware/E53_IA1/*.c}
    C_INCLUDES += -I $(TARGET_DIR)/Hardware/E53_IA1
endif	


ifeq ($(CONFIG_E53_SC1), y)	
	C_SOURCES += ${wildcard $(TARGET_DIR)/Hardware/E53_SC1/*.c}
    C_INCLUDES += -I $(TARGET_DIR)/Hardware/E53_SC1
endif	


ifeq ($(CONFIG_E53_SF1), y)	
	C_SOURCES += ${wildcard $(TARGET_DIR)/Hardware/E53_SF1/*.c}
    C_INCLUDES += -I $(TARGET_DIR)/Hardware/E53_SF1
endif	


ifeq ($(CONFIG_E53_ST1), y)	
	C_SOURCES += ${wildcard $(TARGET_DIR)/Hardware/E53_ST1/*.c}
    C_INCLUDES += -I $(TARGET_DIR)/Hardware/E53_ST1
endif		
	
