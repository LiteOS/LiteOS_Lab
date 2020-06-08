################################################################################
# this is used for compile the atiny_mqtt
################################################################################
C_SOURCES += $(iot_link_root)/oc/oc_mqtt/ota_https/ota_https.c

ifeq ($(CONFIG_OCMQTT_OTASHA256CHECK), y)
	C_SOURCES += $(iot_link_root)/oc/oc_mqtt/ota_https/sha256_check.c
endif

C_INCLUDES += -I $(iot_link_root)/oc/oc_mqtt/ota_https
C_DEFS += -D CONFIG_OC_MQTT_OTA_ENABLE=1

