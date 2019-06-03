################################################################################
# this is used for compile the shell module
################################################################################

ifeq ($(cfg_shell_enble),yes)
    shell_src  = ${wildcard $(iot_link_root)/shell/*.c} 
    C_SOURCES += $(shell_src)	
    		
    shell_inc = -I $(iot_link_root)/shell
    C_INCLUDES += $(shell_inc)
    
    C_DEFS += -D cfg_shell_enable=1
endif

