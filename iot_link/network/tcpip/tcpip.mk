################################################################################
# this is used for compile the iotlink
################################################################################

ifneq ($(cfg_tcpip_type),none)

    SAL_SOURCE  = ${wildcard $(iot_link_root)/network/tcpip/sal/*.c}
    C_SOURCES += $(SAL_SOURCE)
    		
    SAL_INC = -I $(iot_link_root)/network/tcpip/sal
    C_INCLUDES += $(SAL_INC)
    
    #now suport lwip and linux  macos socket
    
    ifeq ($(cfg_tcpip_type), lwip)
    	include $(iot_link_root)/network/tcpip/lwip/lwip_imp.mk
    else ifeq ($(cfg_tcpip_type), linux_socket)
    	include $(iot_link_root)/network/tcpip/linux_socket/linux_socket_imp.mk
    else ifeq ($(cfg_tcpip_type), macos_socket)
    	include $(iot_link_root)/network/tcpip/macos_socket/macos_socket_imp.mk	
    else
    	
    endif

    C_DEFS += -D cfg_tcpip_enable=1
    
endif
