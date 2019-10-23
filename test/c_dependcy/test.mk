################################################################################
# this is used for compile the test
################################################################################
test_dependcy = $(SDK_DIR)/test/c_dependcy

test_dependcy_src  = ${wildcard $(test_dependcy)/*.c} 
C_SOURCES += $(test_dependcy_src)	

test_dependcy_inc = -I $(test_dependcy)
C_INCLUDES += $(test_dependcy_inc)

