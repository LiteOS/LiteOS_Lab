src-y                  += common/*.c membox/*.c
src-$(LOSCFG_NEW_HEAP) += heap/*.c

old-heap-$(LOSCFG_MEMORY_BESTFIT) = bestfit_little/*.c
old-heap-$(LOSCFG_MEMORY_TLSF)    = tlsf/*.c

src-$(LOSCFG_OLD_HEAP) += $(old-heap-y)