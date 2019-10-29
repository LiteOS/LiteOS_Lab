################################################################################
# this is used for compile the mbedtls
################################################################################
#only support the "crt" mode and "psk" mode


USE_MBEDTLS_DEBUG_C := y
USE_MBEDTLS_AES_ROM_TABLES := y

MBEDTLS_INC = \
        -I $(TOP_DIR)/iot_link/network/dtls/mbedtls/mbedtls-2.16.2/include \
        -I $(TOP_DIR)/iot_link/network/dtls/mbedtls/mbedtls_port
         
MBEDTLS_SRC = \
        ${wildcard $(TOP_DIR)/iot_link/network/dtls/mbedtls/mbedtls-2.16.2/library/*.c} \
        ${wildcard $(TOP_DIR)/iot_link/network/dtls/mbedtls/mbedtls_port/*.c}
        
MBEDTLS_DEF = -D WITH_DTLS

ifeq ($(USE_MBEDTLS_DEBUG_C), y)
    MBEDTLS_DEF += -D MBEDTLS_DEBUG_C
endif

ifeq ($(USE_MBEDTLS_AES_ROM_TABLES), y)
    MBEDTLS_DEF += -D MBEDTLS_AES_ROM_TABLES
endif
   
    
ifeq ($(CONFIG_DTLS_TYPE), "mbedtls_cert")    	
    C_DEFS += -D MBEDTLS_CONFIG_FILE=\"los_mbedtls_config_cert.h\"
    C_INCLUDES += $(MBEDTLS_INC)
    C_SOURCES += $(MBEDTLS_SRC)
    C_DEFS += $(MBEDTLS_DEF) -D CFG_MBEDTLS_MODE=CRT
else ifeq ($(CONFIG_DTLS_TYPE), "mbedtls_psk")    	
    C_DEFS += -D MBEDTLS_CONFIG_FILE=\"los_mbedtls_config.h\"
    C_INCLUDES += $(MBEDTLS_INC)
    C_SOURCES += $(MBEDTLS_SRC)
    C_DEFS += $(MBEDTLS_DEF) -D CFG_MBEDTLS_MODE=PSK
else
	C_DEFS += -D NO_DTLS  
endif      



