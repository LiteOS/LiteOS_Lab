# global variables
VPATH           = $(los_root)
MAGIC           = $(los_root)/scripts/make/magic.mk

outdir          = out

# default values for VERBOSE, LIST, DEBUG
V              ?= 0
L              ?= 0
D              ?= 0

# variables used for iterate sub dirs must be defined as ':='
# parse from $(los_root)
subs           := $(abspath $(los_root))
curdir         :=
proj           := $(abspath ./)

# all dir (not abspath) being built will be added to dirs
dirs           :=

cflags-global  :=
inc-global     :=

include        kconfig.mk
include        $(los_root)/scripts/make/$(shell echo $(LOSCFG_ARCH)).mk
include        $(los_root)/scripts/make/common.mk
include        $(MAGIC)

cflags-global  += $(cflags-arch)

# srcs-$(dir) holds the filename with absolutely path
srcs           := $(patsubst $(los_root)%,$(outdir)%,$(foreach d,$(dirs),$(src-$(d))))

objs           := $(srcs:.c=.o)
objs           := $(objs:.s=.o)
objs           := $(objs:.S=.o)

outdirs        := $(sort $(dir $(objs)))

ifeq ($(V),1)
    $(foreach v, $(sort $(filter src-% inc-% par-% cflags-%, $(.VARIABLES))), $(info $(v) = $($(v))))
    $(info objects = $(objs))
    $(info outdir = $(strip $(outdir)))
    $(info outdirs = $(outdirs))
endif

.PHONY: all
ifdef LOSCFG_GNUC
all: $(outdir)/Huawei_LiteOS.out
	$(SIZE) $<
else
ifdef LOSCFG_KEIL
defines        := $(filter -D%, $(cflags-global))
all:
	@rm out -rf
	@$(los_root)/scripts/keilgen $(foreach d,$(dirs),$(src-$(d))) $(inc-global) $(defines)
else
all:
	@echo "error, do nothing"
endif
endif

$(outdir)/Huawei_LiteOS.out : $(objs)
	$(CC) $^ $(cflags-arch) $(lflags) -o $@

$(outdir)/%.o : %.c
	$(strip $(CC) $(cflags-common) $(cflags-global) $(cflags-$<) -I$(proj)/OS_CONFIG $(inc-global) $< -c -o $@)

$(outdir)/%.o : %.s
	$(strip $(CC) $(cflags-common) $(cflags-global) $(cflags-$<) -I$(proj)/OS_CONFIG $(inc-global) $< -c -o $@)

$(outdir)/%.o : %.S
	$(strip $(CC) $(cflags-common) $(cflags-global) $(cflags-$<) -I$(proj)/OS_CONFIG $(inc-global) $< -c -o $@)

.PHONY: clean
ifdef LOSCFG_GNUC
clean:
	@rm -rf $(objs) $(objs:.o=.d) $(objs:.o=.list)
else
ifdef LOSCFG_KEIL
clean:
	@rm -rf Huawei_LiteOS.uvguix* Huawei_LiteOS.uvprojx out
else
clean:
	@echo "error, do nothing"
endif
endif

# define rules for creating out dirs
define dir-rule
    ifeq ($(V),1)
        $$(info define rule for $(1))
    endif
    $(1) :
		@mkdir -p $(1)
endef

$(foreach d,$(outdirs),$(eval $(call dir-rule,$(d))))

# define depend rules for objs to dirs
define obj-rule
    ifeq ($(V),1)
        $$(info define rule for $(1) : $$(dir $(1)))
    endif
    $(1) : | $$(dir $(1))
endef

$(foreach o,$(objs),$(eval $(call obj-rule,$(o))))

-include $(objs:.o=.d)
