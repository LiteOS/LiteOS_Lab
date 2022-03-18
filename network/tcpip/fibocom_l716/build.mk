################################################################################
# this is used for compile the macos socket
################################################################################

#configure the macos socket itself
FIBOCOML716  = ${wildcard $(iot_link_root)/network/tcpip/fibocom_l716/*.c}
C_SOURCES += $(FIBOCOML716)