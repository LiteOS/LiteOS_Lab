################################################################################
# this is used for compile the test
################################################################################

ifeq ($(WITH_CMOCKERY_TEST), yes)
ifeq ($(USE_SOTA),yes)
CMOCKERY_TEST_SOTA_TEST = \
         ${wildcard $(TOP_DIR)/tests/test_sota/sota_test.c}
         C_SOURCES += $(CMOCKERY_TEST_SOTA_TEST)
endif
endif

ifeq ($(WITH_CMOCKERY_TEST), yes)
CMOCKERY_TEST_SRC = \
        ${wildcard $(TOP_DIR)/tests/cmockery/src/cmockery.c} \
        ${wildcard $(TOP_DIR)/tests/test_main.c}
        C_SOURCES += $(CMOCKERY_TEST_SRC)
ifeq ($(WITH_LWIP), yes)
CMOCKERY_AGENTTINY_TEST_SRC = \
        ${wildcard $(TOP_DIR)/tests/test_agenttiny/*.c}
        C_SOURCES += $(CMOCKERY_AGENTTINY_TEST_SRC)
endif
endif

ifeq ($(WITH_CMOCKERY_TEST), yes)
CMOCKERY_TEST_INC = \
        -I $(TOP_DIR)/tests/cmockery/src/google
        C_INCLUDES += $(CMOCKERY_TEST_INC)
endif