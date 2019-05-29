################################################################################
# this is used for compile the iotlink
################################################################################

iot_link_root = $(TOP_DIR)/iot_link

#configure the misc
include $(iot_link_root)/link_misc/link_misc.mk

#configure the log
include $(iot_link_root)/link_log/link_log.mk

#configure the operation system for the iot_link
include $(iot_link_root)/os/os.mk

#configure the shell for the iot_link
include $(iot_link_root)/shell/shell.mk

#configure the libc
include $(iot_link_root)/libc/libc.mk

#configure the loader	
include $(iot_link_root)/loader/loader.mk


iot_link_src  = ${wildcard $(iot_link_root)/*.c} 
C_SOURCES += $(iot_link_src)	
		
iot_link_inc = -I $(iot_link_root)
C_INCLUDES += $(iot_link_inc)

