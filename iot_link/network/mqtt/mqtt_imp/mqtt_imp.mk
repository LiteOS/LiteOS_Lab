################################################################################
# this is used for compile the mqtt lib
################################################################################
#you should choose one mqtt implement, uptils now, we  only adapter the paho mqtt				
ifeq ($(cfg_mqtt_type), paho)

	include $(iot_link_root)/network/mqtt/mqtt_imp/mqtt_paho/mqtt_paho.mk
 	        
endif