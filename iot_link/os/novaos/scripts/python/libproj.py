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

import sys, os, time, argparse, re, pickle

#add to kconfig import path
sys.path.append (os.path.abspath (os.path.dirname (__file__) + "/../kconfig/"))

from menuconfig import menuconfig

from . import libconf

prjname = os.getcwd ().split (sep = "/") [-1]

usage_str = '''
prj [-h] [-w <proj_dir> <bsp_name> [-f]] [-m] [-s <CONFIG_AA=x> [<CONFIG_BB=x> ...]]
    [-v <CONFIG_CC> [<CONFIG_DD> ...]] [-f <reltive_path> [<reltive_path> ...]]
    [--inc-l <reltive_path> [<reltive_path> ...]]
    [--inc-g <reltive_path> [<reltive_path> ...]]
    [--cflags-l <flags> [<flags> ...]]
    [--cflags-g <flags> [<flags> ...]]
'''

help_str = '''
optional arguments:
    -h                                          show this help message and exit
    -w <proj_dir> <bsp_name>                    create a project for a bsp
    -f                                          force, if directory exist, delete it first
    -s <CONFIG_AA=x> [<CONFIG_BB=x> ...]        set kconfig components and variables
    -m                                          open menuconfig cui
    -v <CONFIG_CC> [<CONFIG_DD> ...]            get kconfig components and variable values
    --add <reltive_path> [<reltive_path> ...]   add files to project for building
    --inc-l <reltive_path> [<reltive_path> ...] add local include path to project
    --inc-g <reltive_path> [<reltive_path> ...] add global include path to project
    --cflags-l <flags> [<flags> ...]            add local build flags
    --cflags-g <flags> [<flags> ...]            add global build flags
'''

user_main = '''\
#include <kprintf.h>

int main (void)
    {
    kprintf ("hello world\\n");
    }
'''

def main ():
    global prjname

    parser = argparse.ArgumentParser (usage = usage_str, add_help = False)

    parser.add_argument ("-w", "--workspace",  nargs = 2)
    parser.add_argument ("-f", "--force",      action = 'store_true')
    parser.add_argument ('-m', '--menuconfig', action = 'store_true')
    parser.add_argument ("-s", "--set",        nargs = "+")
    parser.add_argument ("-v", "--value",      nargs = "+")
    parser.add_argument (      "--add",        nargs = "+")
    parser.add_argument (      "--inc-l",      nargs = "+")
    parser.add_argument (      "--inc-g",      nargs = "+")
    parser.add_argument (      "--cflags-l",   nargs = "+")
    parser.add_argument (      "--cflags-g",   nargs = "+")
    parser.add_argument ('-h', '--help',       action = 'store_true')

    args = parser.parse_args ()

    if args.help:
        parser.print_usage ()
        print (help_str)
        quit ()

    if args.workspace:
        prjname = args.workspace [0]

    # create project or get project form current dir
    project = libconf.parse_workspace (args.workspace, args.force)

    if args.add:
        project ["src"]      += args.add

    if args.inc_l:
        project ["inc-l"]    += args.inc_l

    if args.inc_g:
        project ["inc-g"]    += args.inc_g

    if args.cflags_l:
        project ["cflags-l"] += ["-" + x for x in args.cflags_l]

    if args.cflags_g:
        project ["cflags-g"] += ["-" + x for x in args.cflags_g]

    # sort options (include paths, flags etc.)
    for i in project:
        if project [i] and isinstance (project [i], list):
            project [i].sort ()

    # set the kconfig configuration file name in system env
    kconf = libconf.get_kconfig ()

    # handle "-s CONFIG_XX=x" or "--set CONFIG_XX=x", component set command
    libconf.kconfig_set (args.set)

    if not os.path.isfile ("config/kconfig.mk"):
        kconf.write_config ("config/kconfig.mk")

    # menu config
    if args.menuconfig:
        menuconfig (kconf)
        # sync settings incase user quited menuconfig without saving, if we do
        # not do this, the  kconfig.mk will be not aligned with kconf which is
        # updated by menuconfig but not saved!
        kconf.load_config ("config/kconfig.mk", False)

    # update/create kconfig.h
    if not os.path.isfile ("config/kconfig.h"):
        kconf.write_autoconf ("config/kconfig.h")
    elif os.path.getmtime ("config/kconfig.mk") > os.path.getmtime ("config/kconfig.h"):
        kconf.write_autoconf ("config/kconfig.h")

    # hande "-v" or "--value", get value
    if args.value:
        for v in args.value:
            match = re.match ("CONFIG_([^=]+)", v)
            if not match:
                print (v + " not match the 'CONFIG_XX' format, value read ignored.")
                continue
            name = match.groups () [0]
            if name not in kconf.syms:
                print ("kconfig symbol CONFIG_" + name + " not exist, value read ignored.")
            else:
                print (v + "=" + kconf.syms [name].str_value)

    if kconf.syms ["USER_APP"].str_value == "y":
        if not os.path.isfile ("syscall_stub.c"):
            project ["src"].append ("syscall_stub.c")

    # update build.mk
    with open ("build.mk", "w", encoding = "utf-8") as f:
        f.write ("# created by project maintenance tool, do not edit it!\n")
        if kconf.syms ["USER_APP"].str_value == "y":
            f.write ("usr-y")
        else:
            f.write ("src-y")
        f.write ("    := " + " ".join (list (set (project ["src"]))) + "\n")
        f.write ("inc-l-y  := " + " ".join (list (set (project ["inc-l"]))) + "\n")
        f.write ("inc-g-y  := " + " ".join (list (set (project ["inc-g"]))) + "\n")
        f.write ("cflags-l := " + " ".join (list (set (project ["cflags-l"]))) + "\n")
        f.write ("cflags-g := " + " ".join (list (set (project ["cflags-g"]))) + "\n")

    # make must be issued after build.mk created
    if kconf.syms ["USER_APP"].str_value == "y":
        if not os.path.isfile ("syscall_stub.c"):
            with open ("main.c", "w", encoding = "utf-8") as f:
                f.write (user_main)

            with open (prjname + ".prj", "wb") as f:
                pickle.dump (project, f)

            os.system ("make syscall_stub.c")

    # save prj
    if not os.path.isfile (prjname + ".prj"):
        with open (prjname + ".prj", "wb") as f:
            pickle.dump (project, f)
    elif args.add or args.inc_l or args.inc_g or args.cflags_l or args.cflags_g:
        with open (prjname + ".prj", "wb") as f:
            pickle.dump (project, f)

    # quit if NOIDE selected
    if kconf.syms ["NOIDE"].str_value == "y":
        return

    # generate/update IDE project if any IDE support is enabled
    if not os.path.isfile (prjname + "." + kconf.syms ["IDE_EXT"].str_value):
        os.system ("make gen-ide GENIDE=1")
        quit ()
    if os.path.getmtime ("config/kconfig.mk") > os.path.getmtime (prjname + "." + kconf.syms ["IDE_EXT"].str_value):
        os.system ("make gen-ide GENIDE=1")
