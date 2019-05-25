################################################################################
# this is used for compile the iotlink
################################################################################

iot_link_root = $(TOP_DIR)/iot_link

#configure the misc
include $(iot_link_root)/link_misc/link_misc.mk

#configure the operation system for the iot_link
include $(iot_link_root)/oskernel/oskernel.mk

#configure the cJSON 
include $(iot_link_root)/cJSON/cJSON.mk

#configure the shell for the iot_link
include $(iot_link_root)/shell/shell.mk

#configure the sal for the iot_link
include $(iot_link_root)/network/tcpip/tcpip.mk

#configure the dtls for the iot link
include $(iot_link_root)/network/dtls/dtls.mk

###############################OC MQTT EXAMPLE##################################
###configure the mqtt
#include $(iot_link_root)/network/mqtt/mqtt.mk
#
##configure the oc mqtt
#include $(iot_link_root)/oc/oc_mqtt/oc_mqtt.mk
 

###############################OC LWM2M EXAMPLE#################################
#configure the  lwm2m  protocol
include $(iot_link_root)/network/lwm2m/lwm2m.mk

#configure the oc lwm2m models
include $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m.mk


################################################################################
IOT_LINK_SOURCE  = ${wildcard $(iot_link_root)/*.c} 
C_SOURCES += $(IOT_LINK_SOURCE)	
		
IOT_LINK_INC = -I $(iot_link_root)
C_INCLUDES += $(IOT_LINK_INC)

C_DEFS += -DWITH_IOT_LINK
