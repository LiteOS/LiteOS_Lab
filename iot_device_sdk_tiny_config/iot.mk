################################################################################
# this is used for compile the iotlink
################################################################################
iot_link_root = $(SDK_DIR)/iot_link

#configure the common includes
include $(iot_link_root)/inc/inc.mk

#configure the misc
include $(iot_link_root)/link_misc/link_misc.mk

#configure the log
include $(iot_link_root)/link_log/link_log.mk

#configure the operation system for the iot_link
include $(iot_link_root)/os/os.mk

#configure the shell for the iot_link
include $(iot_link_root)/shell/shell.mk

#configure the soft timer for the iot_link
include $(iot_link_root)/stimer/stimer.mk

#configure the queue
include $(iot_link_root)/queue/queue.mk

#configure the driver famework for the iot_link
include $(iot_link_root)/driver/driver.mk

#configure the AT famework for the iot_link
include $(iot_link_root)/at/at.mk

#configure the cJSON 
include $(iot_link_root)/cJSON/cJSON.mk 

#configure th crc
include $(iot_link_root)/crc/crc.mk 

#configure the network
include $(iot_link_root)/network/network.mk

#configure the oceanconnect
include $(iot_link_root)/oc/oc.mk

#configure the ota	
include $(iot_link_root)/link_ota/build.mk

#configure demos
include $(iot_link_root)/demos/demos.mk

#configure the secure clib
include $(iot_link_root)/secure_c/build.mk
iot_link_src  = ${wildcard $(iot_link_root)/*.c} 
C_SOURCES += $(iot_link_src)	
		
iot_link_inc = -I $(iot_link_root)
C_INCLUDES += $(iot_link_inc)


