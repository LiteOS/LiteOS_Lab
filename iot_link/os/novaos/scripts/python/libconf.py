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

import os, sys, pickle, configparser, tempfile, re, shutil, time

sys.path.append (os.path.abspath (os.path.dirname (__file__) + "/../../scripts/kconfig/"))

from kconfiglib import Kconfig

os_root = os.path.abspath (os.path.dirname (__file__) + "/../..")

project = None

# build flag ini file
bdf_ini = None

# kconfig handle
kconfig = None

prjname = os.getcwd ().split (sep = "/") [-1] + ".prj"

bdfname = "flags.ini"

def get_project ():
    global project

    if project:
        return project

    if os.path.exists (prjname):
        with open (prjname, "rb") as f:
            project = pickle.load (f)

    return project

def create_dir (path, force):
    if os.path.isfile (path):
        print ("cannot create dir " + path + ", it is a file!")
        quit ()

    if not os.path.isdir (path):
        os.makedirs (path)

    if not os.path.isdir (path):
        print ("failed to create dir " + path)
        quit ()

    if len (os.listdir (path)) != 0:
        if force:
            shutil.rmtree (path)
        else:
            print ("dir " + path + " not empty!")
            quit ()

def __get_socs (dir, socs):
    for name in os.listdir (dir):
        path = os.path.join (dir, name)
        if os.path.isdir (path):
            __get_socs (path, socs)
            continue
        match = re.match ("(.+)\.config", name)
        if not match:
            continue
        soc = match.groups () [0]
        if soc == "soc":
            continue
        if soc in socs:
            print ("redefined SOC " + soc + " at:")
            print (os.path.join (dir, name))
            print ("and\n" + socs [soc])
            quit ()
        socs [match.groups () [0]] = os.path.join (dir, name)

def get_socs (root):
    socs = {}
    __get_socs (os.path.join (root, "target/platform"), socs)
    return socs

def get_bsps (root):
    bsps = {}
    bsp_root = os.path.join (root, "target/board")
    for name in os.listdir (bsp_root):
        path = os.path.join (bsp_root, name)
        if os.path.isfile (path):
            continue
        if os.path.isfile (os.path.join (path, "kconfig")):
            bsps [name] = path
    return bsps

prj_kconfig = '''\
# created by project maintenance tool, do not edit it!

# add timestamp in the name to avoid potential name conflict
config PROJECT_{0}
    def_bool y
    select {1}

source "{2}/kconfig"

menu "LiteOS NOVA target (platform and board) configuration"

source "{3}/kconfig"
{4}
endmenu

'''

def parse_workspace (workspace, force):
    global project
    global prjname

    if workspace:
        proj, bsp = workspace

        prjname = proj

        bsps = get_bsps (os_root)

        if not bsp in bsps:
            print ("bsp <" + bsp + "> not supported! Supported bsps are: " + str (list (bsps)))
            quit ()

        bsp_dir = bsps [bsp]

        socs = get_socs (os_root)

        soc  = get_soc (bsp_dir)

        if not soc in socs:
            print ("soc <" + soc + "> not supported! Supported socs are: " + str (list (socs)))
            quit ()

        soc_cfg = socs [get_soc (bsps [bsp])]
        soc_dir = os.path.dirname (soc_cfg)

        create_dir (proj, force)
        create_dir (proj + "/config", force)

        project = {"root"     : os_root,            \
                   "bsp"      : bsp,                \
                   "bsp_dir"  : bsp_dir,            \
                   "soc"      : soc,                \
                   "soc_dir"  : soc_dir,            \
                   "soc_cfg"  : soc_cfg,            \
                   "src"      : [],                 \
                   "inc-l"    : [],                 \
                   "inc-g"    : [],                 \
                   "cflags-l" : [],                 \
                   "cflags-g" : []}

        # now, goto the project dir
        os.chdir (proj)

        # copy main.c
        shutil.copyfile (os.path.join (os_root, "target/main.c"), "main.c")

        project ["src"].append ("main.c")

        # create kconfig
        with open ("kconfig", "w", encoding = "utf-8") as f:
            kconfig_list = []
            kconfig_dir  = soc_dir
            while kconfig_dir != os_root:
                if os.path.isfile (os.path.join (kconfig_dir, "kconfig")):
                    kconfig_list.insert (0, os.path.join (kconfig_dir, "kconfig"))
                kconfig_dir = os.path.dirname (kconfig_dir)

            f.write (prj_kconfig.format (int (time.time ()), bsp.upper (), os_root, \
                                         bsp_dir, \
                                         "".join (["source \"" + x + "\"\n" for x in kconfig_list])))
        kconf = get_kconfig ()

        # create Makefile
        with open ("Makefile", "w", encoding = "utf-8") as f:
            f.write ("# created by project maintenance tool, do not edit it!\n")
            f.write ("os_root   := %s\n" % (os_root))
            f.write ("ld_script := $(os_root)/scripts/link/%s.ld\n" % kconf.syms ["LINK_TYPE"].str_value)

            f.write ("lflags    += -Wl,--defsym,RAM_START=$(CONFIG_RAM_START)\n")
            f.write ("lflags    += -Wl,--defsym,RAM_SIZE=$(CONFIG_RAM_SIZE)\n")

            # write ROM info if bsp link type is "rom"
            if kconf.syms ["LINK_TYPE"].str_value == "rom":
                f.write ("lflags    += -Wl,--defsym,ROM_START=$(CONFIG_ROM_START)\n")
                f.write ("lflags    += -Wl,--defsym,ROM_SIZE=$(CONFIG_ROM_SIZE)\n")

            f.write ("include $(os_root)/scripts/make/entry.mk")
    else:
        project = get_project ()

        if not project:
            print ("Error: not in a project workspace!")
            quit ()

    return project

def get_bdf_ini (name, prj):
    global bdf_ini

    if not project:
        return None

    if bdf_ini:
        return bdf_ini

    if not os.path.exists (name):
        return None

    # refresh IDE project only if the build flags file changed
    if os.path.exists (bdfname):
        with open (bdfname,  "r") as old:
            with open (name, "r") as new:
                if old.readlines () == new.readlines () and os.path.exists (prj):
                    os.remove (name)
                    quit ()

        os.remove (bdfname)

    os.rename (name, bdfname)

    bdf_ini = configparser.ConfigParser ()
    bdf_ini.read_file (open (bdfname))

    return bdf_ini

def get_soc (bsp):
    kconf = Kconfig (os.path.join (bsp, "kconfig"))
    if ("SOC" not in kconf.syms) or (not kconf.syms ["SOC"].str_value):
        print ("invalid bsp, SOC is not defined!")
        quit
    return kconf.syms ["SOC"].str_value

def get_kconfig ():
    global kconfig

    if kconfig:
        return kconfig

    project = get_project ()

    if not project:
        return None

    os.environ.setdefault ("KCONFIG_CONFIG", "config/kconfig.mk")

    kconfig = Kconfig ("kconfig")

    kconfig.disable_redun_warnings ()
    kconfig.disable_override_warnings ()

    # load project kconfig.mk
    if os.path.isfile ("config/kconfig.mk"):
        kconfig.load_config ("config/kconfig.mk", False)
    else:
        soc_common = os.path.join (project ["soc_dir"], "soc.config")
        if os.path.isfile (soc_common):
            kconfig.load_config (soc_common, False)

        # load soc.config
        soc_config = project ["soc_cfg"]
        kconfig.load_config (soc_config, False)

        # load bsp.config
        bsp_config = os.path.join (project ["bsp_dir"], "bsp.config")
        if os.path.isfile (bsp_config):
            kconfig.load_config (bsp_config, False)

    return kconfig

def kconfig_set (content):
    if not content:
        return

    kconf = get_kconfig ()

    tmphandle, tmppath = tempfile.mkstemp (prefix = "rtcfg.", text=True)
    tmpfile = os.fdopen (tmphandle, "w")

    for s in content:
        match = re.match ("CONFIG_([^=]+)=(.*)", s)
        if not match:
            print (s + " not match the 'CONFIG_XX=x' format, assignment ignored.")
            continue
        name, val = match.groups ()
        if name not in kconf.syms:
            print ("kconfig symbol CONFIG_" + name + " not exist, assignment ignored.")
        else:
            tmpfile.write (s + "\n")

    tmpfile.close ()
    warn_to_stderr = kconf.warn_to_stderr
    # do not put warnings to console directly, we will handle it (strip the temp file name)
    kconf.warn_to_stderr = False
    kconf.load_config (tmppath, False)
    kconf.warn_to_stderr = warn_to_stderr
    os.remove (tmppath)

    # handle warnings and ignore the temp file name in it
    for e in kconf.warnings:
        print (re.match (".*(warning:.*)", e).groups () [0])

    kconf.write_config ("config/kconfig.mk")
    kconf.write_autoconf ("config/kconfig.h")

