################################################################################
# this is used for compile the dtls
################################################################################

DTLS_AL_SOURCE  = ${wildcard $(TOP_DIR)/iot_link/network/dtls/dtls_al/*.c}
C_SOURCES += $(DTLS_AL_SOURCE)
		
DTLS_AL_INC = -I $(TOP_DIR)/iot_link/network/dtls/dtls_al
C_INCLUDES += $(DTLS_AL_INC)

C_DEFS += -DWITH_DTLS

#only support it mbedtls now
DTLS_AL_CONFIG_TYPE = mbedtls

ifeq ($(DTLS_AL_CONFIG_TYPE), mbedtls)
	include $(TOP_DIR)/iot_link/network/dtls/mbedtls/mbedtls.mk
endif
