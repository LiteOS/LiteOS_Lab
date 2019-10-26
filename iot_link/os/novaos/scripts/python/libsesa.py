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

sesa_header = '''\
<!DOCTYPE CrossStudio_Project_File>
<solution Name="''' + prjname + '''" target="8" version="2">
  <project Name="''' + prjname + '''">
    <configuration
      Name="Common"
      arm_architecture="{arch}"
      arm_assembler_variant="clang"
      arm_compiler_variant="clang"
      arm_core_type="{core}"
      arm_endian="{endian}"
      arm_fp_abi="{fp_abi}"
      arm_fpu_type="{fpu}"
      arm_linker_heap_size="1024"
      arm_linker_process_stack_size="0"
      arm_linker_stack_size="1024"
      arm_linker_variant="SEGGER"
      arm_simulator_memory_simulation_parameter="{simu_para}"
      arm_target_device_name="{device}"
      arm_target_interface_type="{interface}"
      build_intermediate_directory="sesa/$(Configuration)/obj"
      build_output_directory="sesa/$(Configuration)/"
      build_treat_warnings_as_errors="Yes"
      c_additional_options="-Wno-unused-parameter;-Wno-implicit-fallthrough;-Wno-missing-braces;-Wno-missing-field-initializers;{flags}"
      c_preprocessor_definitions="__SESA__=1;"
      c_user_include_directories="{include}"
      debug_target_connection="J-Link"
      gcc_c_language_standard="c11"
      gcc_enable_all_warnings="Yes"
      link_linker_script_file="{root}/scripts/link/rom.icf"
      link_symbol_definitions="ROM_START={rom_start};ROM_SIZE={rom_size};RAM_START={ram_start};RAM_SIZE={ram_size}"
      link_use_linker_script_file="Yes"
      linker_additional_options="--auto-es-block-symbols"
      linker_output_format="hex"
      linker_section_placements_segments="FLASH RX {rom_start} {rom_size};RAM RWX {ram_start} {ram_size}"
      project_directory=""
      project_type="Executable" />
'''

sesa_tail = '''\
  </project>
  <configuration
    Name="Debug"
    c_preprocessor_definitions="DEBUG"
    gcc_debugging_level="Level 3"
    gcc_optimization_level="None" />
  <configuration
    Name="Release"
    c_preprocessor_definitions="NDEBUG"
    gcc_debugging_level="None"
    gcc_omit_frame_pointer="Yes"
    gcc_optimization_level="Level 1" />
</solution>
'''

group_head = '''\
    <folder Name="{0}">
'''

group_tail = '''\
    </folder>
'''

file_entry = '''\
      <file file_name="{0}">
        <configuration
          Name="Common"
          c_user_include_directories="{1}"
          c_additional_options="{2}"
          build_object_file_name="$(IntDir)/{3}/$(InputName)$(OBJ)"
          build_dependency_file_name="$(IntDir)/{3}/$(InputName).d" />
      </file>
'''

def dprint (str):
    pass

def get_root_relative (root, proj):
    rel = ".."

    while os.path.abspath (proj + "/" + rel) != root:
        rel += "/.."

    return rel

# as this script is assumed running under MSYS2, all path handled are unix-like
# we should convert these to windows relative path like "..\..\...\xx.c"
def get_win_path (p):
    root         = libconf.get_project () ["root"]
    proj_dir_rel = get_root_relative (root, os.path.abspath ("."))
    return p.replace (root, proj_dir_rel)

def get_inc (inc):
    return ";".join ([get_win_path (x [2:]) for x in inc.split ()])

def get_flags (flags):
    return flags.replace (" ", ";")

def get_group (file):
    return file.replace ("../", "").split (sep = "/") [0]

def add_files (f, bdf_ini):
    last_group = None

    for src in sorted (bdf_ini.sections () [1:]):
        flags = bdf_ini.get (src, "flags")
        inc   = bdf_ini.get (src, "inc")

        dprint ("dealing with src = " + src)
        dprint ("src = " + src)
        dprint ("flags = " + flags)
        dprint ("inc   = " + inc)

        src = get_win_path (src)

        group = get_group (src)

        if last_group != group:
            if last_group:
                f.write (group_tail)
            f.write (group_head.format (group))

        f.write (file_entry.format (src, get_inc (inc), get_flags (flags), \
                                    os.path.dirname (src.replace ("../", ""))))

        last_group = group

    if last_group:
        f.write (group_tail)

def main ():
    kconf   = libconf.get_kconfig ()

    project = libconf.get_project ()
    bdf_ini = libconf.get_bdf_ini (sys.argv [1], prjname + ".emProject")

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
    global_flags = get_flags (bdf_ini.get ("global", "flags"))
    dprint ("global_flags = " + global_flags)

    arch      = kconf.syms ["SESA_ARCH"].str_value
    core      = kconf.syms ["SESA_CORE"].str_value
    endian    = kconf.syms ["SESA_ENDIAN"].str_value
    fp_abi    = kconf.syms ["SESA_FP_ABI"].str_value
    fpu       = kconf.syms ["SESA_FPU"].str_value
    device    = kconf.syms ["SESA_DEVICE"].str_value
    interface = kconf.syms ["SESA_INTERFACE"].str_value
    simu_para = kconf.syms ["SESA_SIMU_MEM_PARA"].str_value

    if (not arch) or (not core) or (not endian) or (not fp_abi) or (not fpu) or \
       (not device) or (not interface):
        print ("sesa not supported!")
        quit ()

    with open (prjname + ".emProject", "w", encoding = "utf-8") as f:
        f.write (sesa_header.format (                       \
            arch      = arch,                               \
            core      = core,                               \
            endian    = endian,                             \
            fp_abi    = fp_abi,                             \
            fpu       = fpu,                                \
            device    = device,                             \
            interface = interface,                          \
            simu_para = simu_para,                          \
            root      = get_win_path (project ["root"]),    \
            flags     = global_flags,                       \
            rom_start = kconf.syms ["ROM_START"].str_value, \
            rom_size  = kconf.syms ["ROM_SIZE"].str_value,  \
            ram_start = kconf.syms ["RAM_START"].str_value, \
            ram_size  = kconf.syms ["RAM_SIZE"].str_value,  \
            include   = global_inc))

        add_files (f, bdf_ini)

        f.write (sesa_tail)

    print ("created " + prjname + ".emProject, happy coding ;-)")
