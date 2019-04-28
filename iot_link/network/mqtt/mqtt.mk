################################################################################
# this is used for compile the mqtt lib
################################################################################

MQTT_AL_SOURCE  = \
		${wildcard $(TOP_DIR)/iot_link/network/mqtt/mqtt_al/*.c} 
C_SOURCES += $(MQTT_AL_SOURCE)	
		
MQTT_AL_INC = \
	    -I $(TOP_DIR)/iot_link/network/mqtt/mqtt_al
	    
C_INCLUDES += $(MQTT_AL_INC)

C_DEFS += -DCFG_MQTT_EN
##uptils now , we only support the paho mqtt
MQTT_AL_CFG_TYPE = paho_mqtt
				
ifeq ($(MQTT_AL_CFG_TYPE), paho_mqtt)

include $(TOP_DIR)/iot_link/network/mqtt/mqtt_paho/mqtt_paho.mk
 	        
endif