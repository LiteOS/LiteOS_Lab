################################################################################
# this is used for compile the mqtt lib
################################################################################
ifeq ($(CONFIG_MQTT_ENABLE), y)
    mqtt_src  = ${wildcard $(iot_link_root)/network/mqtt/mqtt_al/*.c}
    C_SOURCES += $(mqtt_src)

    mqtt_inc = -I $(iot_link_root)/network/mqtt/mqtt_al
    C_INCLUDES += $(mqtt_inc)
    
    C_DEFS += -D CONFIG_MQTT_ENABLE=1
    
    ifeq ($(CONFIG_MQTT_TYPE), "paho_mqtt")
    	include $(iot_link_root)/network/mqtt/paho_mqtt/paho_mqtt.mk
    else ifeq ($(CONFIG_MQTT_TYPE), "lite_mqtt")
    	include $(iot_link_root)/network/mqtt/lite_mqtt/lite_mqtt.mk
    else
    	#you could add your mqtt here   	
    endif
       
endif

