################################################################################
# this is used for compile the OC_MQTT
# please add the corresponding file to C_SOURCES C_INCLUDES C_DEFS
################################################################################
#NEXT TIME WE SHOULD MOVE THE JSON OUT

ifeq ($(CONFIG_OCMQTT_ENABLE), y)

    C_SOURCES += $(iot_link_root)/oc/oc_mqtt/oc_mqtt_al/oc_mqtt_al.c
    C_INCLUDES +=  -I $(iot_link_root)/oc/oc_mqtt/oc_mqtt_al
    
#if we enable the at command here
	ifeq ($(CONFIG_OCMQTT_ATCMD), y)
        C_SOURCES += $(iot_link_root)/oc/oc_mqtt/oc_mqtt_al/oc_mqtt_at.c
    endif
    
#   add the profile  function tools 
    ifeq ($(CONFIG_OC_MQTTV5_PROFILE), y)
    
    	C_SOURCES += $(iot_link_root)/oc/oc_mqtt/oc_mqtt_profile_v5/oc_mqtt_profile.c \
    	             $(iot_link_root)/oc/oc_mqtt/oc_mqtt_profile_v5/oc_mqtt_profile_package.c  \
    	             $(iot_link_root)/oc/oc_mqtt/oc_mqtt_profile_v5/oc_mqtt_event.c
    	             
    	
    	ifeq ($(CONFIG_OC_MQTTV5_DEMO), y)
   		
   			C_SOURCES += $(iot_link_root)/oc/oc_mqtt/oc_mqtt_profile_v5/oc_mqtt_demo.c
    		
    	endif
    	
       
        C_INCLUDES += -I $(iot_link_root)/oc/oc_mqtt/oc_mqtt_profile_v5
        
    endif
         	    	
    ifeq ($(CONFIG_OC_MQTTV1_PROFILE), y)
    	
    	C_SOURCES  += $(iot_link_root)/oc/oc_mqtt/oc_mqtt_profile/oc_mqtt_assistant.c
    	
    	ifeq ($(CONFIG_OC_MQTTV1_DEMO), y)
   		
   			C_SOURCES += $(iot_link_root)/oc/oc_mqtt/oc_mqtt_profile/oc_mqtt_demo.c
    		
    	endif
    	
        C_INCLUDES += -I $(iot_link_root)/oc/oc_mqtt/oc_mqtt_profile
    endif
	

	            	
#   add the implement component 
    ifeq ($(CONFIG_OC_TINYMQTTV5_ENABLE),y)
    	include $(iot_link_root)/oc/oc_mqtt/oc_mqtt_tiny_v5/oc_mqtt_tiny.mk	
    endif
    
    ifeq ($(CONFIG_OC_TINYMQTTV1_ENABLE),y)
    	include $(iot_link_root)/oc/oc_mqtt/oc_mqtt_tiny/oc_mqtt_tiny.mk	
    endif
    
    ifeq ($(CONFIG_OC_EC2XV5_ENABLE),y)
    	include $(iot_link_root)/oc/oc_mqtt/ec2x_v5/build.mk
   	endif
   	
   	 ifeq ($(CONFIG_EC2XV1_ENABLE),y)
    	include $(iot_link_root)/oc/oc_mqtt/ec2x_oc/ec2x_oc.mk
   	endif 	 	
  
endif