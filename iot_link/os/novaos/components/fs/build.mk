src-prefix          := src/
src-$(CONFIG_VFS)   += vfs.c
src-$(CONFIG_DEVFS) += devfs.c
inc-g-$(CONFIG_VFS) += h/