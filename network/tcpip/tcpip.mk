################################################################################
# this is used for compile the iotlink
################################################################################

ifeq ($(CONFIG_TCPIP_AL_ENABLE), y)

    C_SOURCES += ${wildcard $(iot_link_root)/network/tcpip/sal/*.c}    		
    C_INCLUDES += -I $(iot_link_root)/network/tcpip/sal
    C_DEFS += -D TCPIP_AL_ENABLE=1
   
    #now suport "lwip" and "linux"  "macos" socket
    ifeq ($(CONFIG_LWIP_ENABLE), y)
        include $(iot_link_root)/network/tcpip/lwip_socket/lwip_socket_imp.mk
    endif
        
    ifeq ($(CONFIG_LINUXSOCKET_ENABLE), y)
        include $(iot_link_root)/network/tcpip/linux_socket/linux_socket_imp.mk
    endif
    
    ifeq ($(CONFIG_MACOSSOCKET_ENABLE), y)
        include $(iot_link_root)/network/tcpip/macos_socket/macos_socket_imp.mk
    endif
    
    ifeq ($(CONFIG_ESP8266_ENABLE), y)
        include $(iot_link_root)/network/tcpip/esp8266/build.mk
    endif
    
    ifeq ($(CONFIG_RTK8710_ENABLE), y)
        include $(iot_link_root)/network/tcpip/rtk8710/build.mk
    endif
    
    ifeq ($(CONFIG_FIBOCOML716_ENABLE), y)
        include $(iot_link_root)/network/tcpip/fibocom_l716/build.mk        
    endif

    
endif
