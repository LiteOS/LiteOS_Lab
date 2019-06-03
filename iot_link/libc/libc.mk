################################################################################
# this is used for compile the libc
################################################################################

ifeq ($(cfg_libc_enable),yes)
    libc_src  = ${wildcard $(iot_link_root)/libc/*.c} 
    C_SOURCES += $(libc_src)	
    		
    libc_inc = -I $(iot_link_root)/libc
    C_INCLUDES += $(libc_inc)
    
    C_DEFS += -D cfg_libc_enable=1
    
endif







