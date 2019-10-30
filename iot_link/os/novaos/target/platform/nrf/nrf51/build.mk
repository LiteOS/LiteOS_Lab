dir-$(CONFIG_NRF51) = *
src-$(CONFIG_NRF51) += soc_init.c

# source $(soc).mk if any
-include $(soc).mk