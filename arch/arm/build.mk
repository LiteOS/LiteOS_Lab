sub-$(LOSCFG_AARCH_M_BASELINE) += arm-m/armv6-m
sub-$(LOSCFG_AARCH_M_MAINLINE) += arm-m/armv7-m
src-$(LOSCFG_AARCH_M)          += arm-m/src/*.c
inc                            += common/cmsis arm-m/include