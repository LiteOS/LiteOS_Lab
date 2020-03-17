################################################################################
# this is used for compile the service module
################################################################################

ifeq ($(CONFIG_CRYPTO_ENABLE),y)
	C_SOURCES  +=  ${wildcard $(iot_link_root)/crypto/*.c}

	C_INCLUDES += -I$(iot_link_root)/crypto

	C_DEFS     += -D CONFIG_CRYPTO_ENABLE=1
endif