# put all dir in dirs
dirs             += $(curdir)

src-$(curdir)    := $(sort $(abspath $(wildcard $(addprefix $(curdir)/,$(src-y)))))

cflags-$(curdir) := $(cflags)
cflags-global    += $(cflags-g)

inc-$(curdir)    := $(inc-$(par-$(curdir))) $(addprefix -I, $(abspath $(addprefix $(curdir)/, $(inc))))
inc-global       += $(addprefix -I, $(abspath $(sort $(dir $(wildcard $(addsuffix /./,$(addprefix $(curdir)/,$(inc))))))))

# expand $(sub-y) and append to $(subs)
ifneq ($(words $(sub-y)),0)
    # _subs hold all the result after wildcard in sub-y add suffix /./ with wildcard to force only find dir
    _subs        := $(abspath $(sort $(dir $(wildcard $(addsuffix /./,$(addprefix $(curdir)/,$(sub-y)))))))

    # append $(proj) to the _subs of $(los_root)
    ifeq ($(curdir),$(los_root))
        ifeq ($(V),1)
            $(info appending the project dir [$(proj)] to _subs)
        endif
        _subs    := $(proj) $(_subs)
    endif

    # save the parent dir of xxx in variable par-xxx
    define def-par
        ifeq ($(V),1)
            $$(info setting par-$(1) = [$(2)])
        endif
        par-$(1) := $(2)
    endef

    $(foreach s,$(_subs),$(eval $(call def-par,$(s),$(curdir))))

    subs         += $(_subs)
endif

# cflags, include for each file including per-file specific defines
define def-xxx
    ifeq ($(V),1)
        $$(info appending $(1)-$$(patsubst $(4)%,%,$(2)) = [$$($(1)-$$(patsubst $(4)%,%,$(2)))] to $(1)-$(2))
    endif
    ifeq ($(1),cflags)
        $(1)-$(2)    := $(3) $$($(1)-$$(patsubst $(4)%,%,$(2)))
    endif
    ifeq ($(1),inc)
        $(1)-$(2)    := $(3) $$(addprefix -I,$$(abspath $(4)$$($(1)-$$(patsubst $(4)%,%,$(2)))))
    endif
    # now undefine $(1)-$$(patsubst $(4)%,%,$(2))
    ifeq ($(V),1)
        $$(info undefining $(1)-$$(patsubst $(4)%,%,$(2)))
    endif
    # override undefine $(1)-$$(patsubst $(4)%,%,$(2))
    override $(1)-$$(patsubst $(4)%,%,$(2)) :=
endef

$(foreach s,$(src-$(curdir)),$(eval $(call def-xxx,cflags,$(s),$(cflags),$(curdir)/)))
$(foreach s,$(src-$(curdir)),$(eval $(call def-xxx,inc,$(s),$(inc-$(curdir)),$(curdir)/)))

ifeq ($(V),1)
    # basic info
    $(info curdir = $(strip $(curdir)))
    $(info src-y = $(strip $(src-y)))
    $(info dirs = $(strip $(dirs)))
    $(info sub-y = $(strip $(sub-y)))
    $(info _subs = $(_subs))
    $(info subs = $(strip $(subs)))

    # meta info
    $(info src-curdir: src-$(curdir) = $(strip $(src-$(curdir))))
    $(info par-curdir: par-$(curdir) = $(strip $(par-$(curdir))))
    #$(info inc-curdir: inc-$(curdir) = $(inc-$(curdir)))
    $(info flags-curdir: cflags-$(curdir) = $(cflags-$(curdir)))
    #$(info inc-par-curdir: inc-$(par-$(curdir)) = $(strip $(inc-$(par-$(curdir)))))

    # inc, cflags info
    $(foreach s,$(src-$(curdir)),$(info cflags-$(s) = $(cflags-$(s))))
    #$(foreach s,$(src-$(curdir)),$(info inc-$(s) = $(inc-$(s))))
    $(info inc-global = $(inc-global))
endif

ifneq ($(words $(subs)),0)
    parent       := $(curdir)
    curdir       := $(firstword $(subs))
    subs         := $(wordlist 2,$(words $(subs)),$(subs))
    sub-y        :=
    src-y        :=
    cflags       :=
    cflags-g     :=
    inc          :=
    ifeq ($(V),1)
        $(info ================================================================================)
        $(info parsing $(curdir)/build.mk)
    endif
    include      $(curdir)/build.mk
    include      $(MAGIC)
endif
