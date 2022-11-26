################################################################################
# this is used for compile the dtls
################################################################################

ifeq ($(CONFIG_TLS_AL_ENABLE), y)

    C_SOURCES += ${wildcard $(iot_link_root)/network/tls/tls_al/*.c}
    C_INCLUDES += -I $(iot_link_root)/network/tls/tls_al
    
    ifeq ($(CONFIG_MBEDTLS_ENABLE), y)
      include $(iot_link_root)/network/tls/mbedtls/mbedtls.mk
    endif

endif

