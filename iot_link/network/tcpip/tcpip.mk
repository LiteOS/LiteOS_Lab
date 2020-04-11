################################################################################
# this is used for compile the iotlink
################################################################################

ifeq ($(CONFIG_TCPIP_ENABLE), y)

    SAL_SOURCE  = ${wildcard $(iot_link_root)/network/tcpip/sal/*.c}
    C_SOURCES += $(SAL_SOURCE)
    		
    SAL_INC = -I $(iot_link_root)/network/tcpip/sal
    C_INCLUDES += $(SAL_INC)
   
    #now suport "lwip" and "linux"  "macos" socket
    ifeq ($(CONFIG_TCPIP_TYPE), "lwip_socket")
        include $(iot_link_root)/network/tcpip/lwip_socket/lwip_socket_imp.mk
    else ifeq ($(CONFIG_TCPIP_TYPE), "linux_socket")
        include $(iot_link_root)/network/tcpip/linux_socket/linux_socket_imp.mk
    else ifeq ($(CONFIG_TCPIP_TYPE), "macos_socket")
        include $(iot_link_root)/network/tcpip/macos_socket/macos_socket_imp.mk
    else ifeq ($(CONFIG_TCPIP_TYPE), "esp8266_socket")
        include $(iot_link_root)/network/tcpip/esp8266_socket/esp8266_socket_imp.mk
    else ifeq ($(CONFIG_TCPIP_TYPE), "rtk8710_socket")
        include $(iot_link_root)/network/tcpip/rtk8710_socket/rtk8710_socket_imp.mk
    else
 		#maybe you want to add your own tcpip here
    endif

    C_DEFS += -D CONFIG_TCPIP_ENABLE=1
    
endif
