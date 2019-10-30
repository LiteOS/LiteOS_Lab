src-prefix              := src/
src-y                   += heap.c malloc.c memtry.c
src-$(CONFIG_PAGE_POOL) += page.c
src-$(CONFIG_MEMPOOL)   += mempool.c
inc-g-y                 += h/