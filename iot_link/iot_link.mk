################################################################################
# this is used for compile the iotlink
################################################################################

#configure the misc
include $(TOP_DIR)/iot_link/link_misc/link_misc.mk

#configure the operation system for the iot_link
include $(TOP_DIR)/iot_link/oskernel/oskernel.mk


#configure the shell for the iot_link
include $(TOP_DIR)/iot_link/shell/shell.mk

#configure the sal for the iot_link
include $(TOP_DIR)/iot_link/network/tcpip/tcpip.mk

#configure the dtls for the iot link
include $(TOP_DIR)/iot_link/network/dtls/dtls.mk

#configure the mqtt
include $(TOP_DIR)/iot_link/network/mqtt/mqtt.mk

#configure the oc mqtt
include $(TOP_DIR)/iot_link/oc/oc_mqtt/oc_mqtt.mk


IOT_LINK_SOURCE  = ${wildcard $(TOP_DIR)/iot_link/*.c} 
C_SOURCES += $(IOT_LINK_SOURCE)	
		
IOT_LINK_INC = -I $(TOP_DIR)/iot_link
C_INCLUDES += $(IOT_LINK_INC)

C_DEFS += -DWITH_IOT_LINK
