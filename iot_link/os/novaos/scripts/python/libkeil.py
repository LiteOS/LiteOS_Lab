#
# Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
#
# LiteOS NOVA is licensed under the Mulan PSL v1.
# You can use this software according to the terms and conditions of the Mulan PSL v1.
# You may obtain a copy of Mulan PSL v1 at:
#
# 	http://license.coscl.org.cn/MulanPSL
#
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
# FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v1 for more details.
#

import sys, os, argparse, shutil, json, tempfile, re
from . import libconf

prjname = os.getcwd ().split (sep = "/") [-1]

keil_header = '''\
<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<Project xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="project_projx.xsd">

  <SchemaVersion>2.1</SchemaVersion>

  <Header>### uVision Project, (C) Keil Software</Header>

  <Targets>
    <Target>
      <TargetName>''' + prjname + '''</TargetName>
      <ToolsetNumber>0x4</ToolsetNumber>
      <uAC6>1</uAC6>
      <TargetOption>
        <TargetCommonOption>
          <Device>{device}</Device>
          <Vendor>{vendor}</Vendor>
          <Cpu>{cpu}</Cpu>
          <OutputDirectory>keil\\</OutputDirectory>
          <OutputName>''' + prjname + '''</OutputName>
          <CreateExecutable>1</CreateExecutable>
          <ListingPath>keil\\</ListingPath>
        </TargetCommonOption>
        <TargetArmAds>
          <Cads>
            <Optim>2</Optim>
            <OneElfS>1</OneElfS>
            <wLevel>2</wLevel>
            <uC99>1</uC99>
            <uGnu>1</uGnu>
            <v6Lang>6</v6Lang>
            <v6WtE>1</v6WtE>
            <VariousControls>
              <MiscControls>-Wno-bad-function-cast -Wno-date-time -Wno-missing-braces -Wno-missing-field-initializers -Wno-gnu-designator -Wno-dollar-in-identifier-extension -Wno-unused-parameter -Wno-overlength-strings -Wno-missing-prototypes -Wno-missing-variable-declarations -Wno-padded -Wno-cast-align -Wno-used-but-marked-unused -Wno-zero-length-array {flags}</MiscControls>
              <Define>__KEIL__=1</Define>
              <Undefine></Undefine>
              <IncludePath>{include}</IncludePath>
            </VariousControls>
          </Cads>
          <LDads>
            <ScatterFile>.\\''' + prjname + '''.sct</ScatterFile>
            <Misc>{keep}</Misc>
            <DisabledWarnings>6319,6314</DisabledWarnings>
          </LDads>
        </TargetArmAds>
        <DllOption>
          <TargetDllName>{tdn}</TargetDllName>
          <TargetDllArguments>{tda}</TargetDllArguments>
          <TargetDlgDll>{tddn}</TargetDlgDll>
          <TargetDlgDllArguments>{tdda}</TargetDlgDllArguments>
        </DllOption>
      </TargetOption>
      <Groups>
'''

keil_tail = '''\
      </Groups>
    </Target>
  </Targets>
</Project>
'''

group_head = '''\
        <Group>
          <GroupName>{0}</GroupName>
          <Files>
'''

group_tail = '''\
          </Files>
        </Group>
'''

file_entry_bdf_n = '''\
            <File>
              <FileName>{0}</FileName>
              <FileType>1</FileType>
              <FilePath>{1}</FilePath>
            </File>
'''

file_entry_bdf_y = '''\
            <File>
              <FileName>{0}</FileName>
              <FileType>1</FileType>
              <FilePath>{1}</FilePath>
              <FileOption>
                <FileArmAds>
                  <Cads>
                    <VariousControls>
                      <MiscControls>{2}</MiscControls>
                      <Define></Define>
                      <Undefine></Undefine>
                      <IncludePath>{3}</IncludePath>
                    </VariousControls>
                  </Cads>
                </FileArmAds>
              </FileOption>
            </File>
'''

sct_content = '''\
; *************************************************************
; *** created by project maintenance tool, do not edit it!
; *************************************************************

LR_IROM1 {rom_start} {rom_size}
    {{
    ER_IROM1 {rom_start}
        {{
        *.o (vectors, +FIRST)
        *(InRoot$$Sections)
        .ANY (+RO)
        }}

    INIT_CPU        +0 ALIGN 4 {{ *.o (init_cpu) }}
    INIT_BOARD      +0 ALIGN 4 {{ *.o (init_board) }}
    INIT_PREKERNEL  +0 ALIGN 4 {{ *.o (init_prekernel) }}
    INIT_KERNEL     +0 ALIGN 4 {{ *.o (init_kernel) }}
    INIT_POSTKERNEL +0 ALIGN 4 {{ *.o (init_postkernel) }}
    INIT_HAL        +0 ALIGN 4 {{ *.o (init_hal) }}
    INIT_BUS        +0 ALIGN 4 {{ *.o (init_bus) }}
    INIT_DRIVER     +0 ALIGN 4 {{ *.o (init_driver) }}
    INIT_POSTDRIVER +0 ALIGN 4 {{ *.o (init_postdriver) }}
    INIT_USER       +0 ALIGN 4 {{ *.o (init_user) }}

    RW_IRAM1 {ram_start} {ram_size}
        {{
        .ANY (+RW +ZI)
        }}

    ARM_LIB_STACKHEAP +0 ALIGN 8 EMPTY {irq_stack_size}
        {{
        }}
    }}
'''

kept_sections = ["init_cpu",        \
                 "init_board",      \
                 "init_prekernel",  \
                 "init_kernel",     \
                 "init_postkernel", \
                 "init_hal",        \
                 "init_bus",        \
                 "init_driver",     \
                 "init_postdriver", \
                 "init_user",       \
                 "cmder"]

def dprint (str):
    pass

def get_root_relative (root, proj):
    rel = ".."

    while os.path.abspath (proj + "/" + rel) != root:
        rel += "/.."

    return rel.replace ("/", "\\")

# as this script is assumed running under MSYS2, all path handled are unix-like
# we should convert these to windows relative path like "..\..\...\xx.c"
def get_win_path (p):
    root         = libconf.get_project () ["root"]
    proj_dir_rel = get_root_relative (root, os.path.abspath ("."))
    return p.replace (root, proj_dir_rel).replace ("/", "\\")

def get_inc (d):
    return ";".join ([get_win_path (x [2:]) for x in d.split ()])

def get_group (file):
    return file.replace ("..\\", "").split (sep = "\\") [0]

def add_files (f, bdf_ini):
    last_group = None

    for src in sorted (bdf_ini.sections () [1:]):
        flags = bdf_ini.get (src, "flags")
        inc   = bdf_ini.get (src, "inc")

        dprint ("dealing with src = " + src)
        dprint ("src   = " + src)
        dprint ("inc   = " + inc)
        dprint ("flags = " + flags)

        src = get_win_path (src)

        group = get_group (src)

        if last_group != group:
            if last_group:
                f.write (group_tail)
            f.write (group_head.format (group))

        if not flags and not inc:
            f.write (file_entry_bdf_n.format (src.split (sep = "\\") [-1], src))
        else:
            f.write (file_entry_bdf_y.format (src.split (sep = "\\") [-1], src, \
                                              flags, get_inc (inc)))

        last_group = group

    if last_group:
        f.write (group_tail)

def main ():
    project = libconf.get_project ()
    bdf_ini = libconf.get_bdf_ini (sys.argv [1], prjname + ".uvprojx")

    kconf   = libconf.get_kconfig ()

    device  = kconf.syms ["KEIL_DEVICE"].str_value
    vendor  = kconf.syms ["KEIL_VENDOR"].str_value
    cpu     = kconf.syms ["KEIL_CPU"].str_value
    tdn     = kconf.syms ["KEIL_TARGET_DLL_NAME"].str_value
    tda     = kconf.syms ["KEIL_TARGET_DLL_ARG"].str_value
    tddn    = kconf.syms ["KEIL_TARGET_DLG_DLL_NAME"].str_value
    tdda    = kconf.syms ["KEIL_TARGET_DLG_DLL_ARG"].str_value

    if (not device) or (not vendor) or (not cpu):
        print ("keil not supported!")
        quit ()

    if not project:
        print ("project file not exist!")
        quit ()
    elif not bdf_ini:
        print ("build flag file not exit!")
        quit ()

    # get global include
    global_inc = get_inc (bdf_ini.get ("global", "inc"))
    dprint ("global_inc = " + global_inc)

    # get global define
    global_flags = bdf_ini.get ("global", "flags")
    dprint ("global_flags = " + global_flags)

    # create prjname.sct if not exist
    if not os.path.isfile (prjname + ".sct"):
        with open (prjname + ".sct", "w", encoding = "utf-8") as f:
            # TODO: fix me when supported ram link type with keil

            f.write (sct_content.format (                               \
                rom_start      = kconf.syms ["ROM_START"].str_value,    \
                rom_size       = kconf.syms ["ROM_SIZE"].str_value,     \
                ram_start      = kconf.syms ["RAM_START"].str_value,    \
                ram_size       = kconf.syms ["RAM_SIZE"].str_value,     \
                irq_stack_size = kconf.syms ["IRQ_STACK_SIZE"].str_value))

    if kconf.syms ["USER_APP"].str_value != "y":
        keep = ''.join (["--keep *.o(" + x + ") " for x in kept_sections]).strip ()
    else:
        keep = ""

    with open (prjname + ".uvprojx", "w", encoding = "utf-8") as f:
        f.write (keil_header.format (device  = device,          \
                                     vendor  = vendor,          \
                                     cpu     = cpu,             \
                                     tdn     = tdn,             \
                                     tda     = tda,             \
                                     tddn    = tddn,            \
                                     tdda    = tdda,            \
                                     flags   = global_flags,    \
                                     keep    = keep,            \
                                     include = global_inc))

        add_files (f, bdf_ini)

        f.write (keil_tail)

    print ("created " + prjname + ".uvprojx, happy coding ;-)")

