################################################################################
# this is used for compile the mqtt lib
################################################################################

ifneq ($(cfg_mqtt_type),none)

    mqtt_src  = ${wildcard $(iot_link_root)/network/mqtt/mqtt_al/*.c} 
    C_SOURCES += $(mqtt_src)	
    		
    mqtt_inc = -I $(iot_link_root)/network/mqtt/mqtt_al
    C_INCLUDES += $(mqtt_inc)
    
    C_DEFS += -D cfg_mqtt_enable=1
    
    ##anyway,if you need the mqtt service, you should choose one of the mqtt implement
    include $(iot_link_root)/network/mqtt/mqtt_imp/mqtt_imp.mk

endif

