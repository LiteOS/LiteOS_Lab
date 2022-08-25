################################################################################
# this is used for compile the ota downloading
################################################################################
ifeq ($(CONFIG_OTA_DOWNLOAD_HTTPS), y)
	C_SOURCES += ${wildcard $(iot_link_root)/link_ota/download/https/*.c}
	C_INCLUDES += -I $(iot_link_root)/link_ota/download/https
endif




