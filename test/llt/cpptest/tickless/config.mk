##
# note: how to configure iot_link? you can see targets/LINUX/GCC/config.mk.
#		Keep it simple here
##

CONFIG_OS_TYPE          := "linux"

CONFIG_ARCH_CPU_TYPE    := "x86-64"

include $(TOP_DIR)/iot_link/iot.mk
