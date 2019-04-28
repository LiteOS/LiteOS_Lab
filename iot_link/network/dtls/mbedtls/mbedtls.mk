################################################################################
# this is used for compile the mbedtls
################################################################################
#only support the mqtt test
WITH_MQTT = yes

MBEDTLS_INC = \
        -I $(TOP_DIR)/iot_link/network/dtls/mbedtls/mbedtls-2.6.0/include
        C_INCLUDES += $(MBEDTLS_INC)

MBEDTLS_PORT_INC = \
        -I $(TOP_DIR)/iot_link/network/dtls/mbedtls/mbedtls_port
        C_INCLUDES += $(MBEDTLS_PORT_INC)
MBEDTLS_SRC = \
        ${wildcard $(TOP_DIR)/iot_link/network/dtls/mbedtls/mbedtls-2.6.0/library/*.c}
        C_SOURCES += $(MBEDTLS_SRC)

MBEDTLS_PORT_SRC = \
        ${wildcard $(TOP_DIR)/iot_link/network/dtls/mbedtls/mbedtls_port/*.c}
        C_SOURCES += $(MBEDTLS_PORT_SRC)        
        
ifeq ($(WITH_MQTT), yes)
    C_DEFS += -D MBEDTLS_CONFIG_FILE=\"los_mbedtls_config_cert.h\"
else
    C_DEFS += -D MBEDTLS_CONFIG_FILE=\"los_mbedtls_config.h\"
endif
ifeq ($(USE_MBEDTLS_DEBUG_C), yes)
    C_DEFS += -DMBEDTLS_DEBUG_C
endif

ifeq ($(USE_MBEDTLS_AES_ROM_TABLES), yes)
    C_DEFS += -DMBEDTLS_AES_ROM_TABLES
endif
