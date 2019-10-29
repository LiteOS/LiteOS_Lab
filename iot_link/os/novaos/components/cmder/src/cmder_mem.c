/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
 *
 * LiteOS NOVA is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *
 * 	http://license.coscl.org.cn/MulanPSL
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
 * FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v1 for more details.
 */

#include <string.h>
#include <stdlib.h>

#include <memtry.h>
#include <cmder.h>

static void __cmd_d_help (cmder_t * cmder)
    {
    cmder_printf (cmder, "usage: d address [units] [unit_size]\n");
    cmder_printf (cmder, "display memory content at address\n\n");
    cmder_printf (cmder, "if units is omited, 16 is picked\n");
    cmder_printf (cmder, "if unit_size is omited, 1 is picked\n");
    }

static void __dump_mem (cmder_t * cmder, char * addr, int order, int units)
    {
    int      i;
    uint8_t  u8  = 0;
    uint16_t u16 = 0;
    uint32_t u32 = 0;
    uint64_t u64 = 0;
    int      ret;

    for (i = 0; i < units; i++, addr += (1 << order))
        {

        /* display 16 bytes per line */

        if (((i & ((16 >> order) - 1)) == 0))
            {
            if (i != 0)
                {
                cmder_printf (cmder, "\n");
                }

            cmder_printf (cmder, "%p: ", addr);
            }

        /* this code snippet will works fine for both little and big endian */

        switch (order)
            {
            case 0: ret = mem_try (&u8,  addr, order); break;
            case 1: ret = mem_try (&u16, addr, order); break;
            case 2: ret = mem_try (&u32, addr, order); break;
            case 3: ret = mem_try (&u64, addr, order); break;
            default: return;    /* should never be here */
            }

        if (ret == -1)
            {
            cmder_printf (cmder, &"???????????????? " [16 - (2 << order)]);

            continue;
            }

        switch (order)
            {
            case 0: cmder_printf (cmder, "%02x ",    u8);  break;
            case 1: cmder_printf (cmder, "%04x ",    u16); break;
            case 2: cmder_printf (cmder, "%08x ",    u32); break;
            case 3: cmder_printf (cmder, "%016llx ", u64); break;
            default: return;    /* should never be here */
            }
        }
    }

static int __cmd_d (cmder_t * cmder, int argc, char * argv [])
    {
    static char * addr = NULL;

    /* save last commander info so these feilds can be omitted */

    static int    units = 0;
    static int    unit_size = 0;

    if (argc == 1)
        {
        if (addr == NULL)
            {
            cmder_printf (cmder, "error: no address specified!\n");
            __cmd_d_help (cmder);
            return -1;
            }
        }
    else
        {
        if ((strcmp (argv [1], "--help") == 0) || (strcmp (argv [1], "/?") == 0))
            {
            addr = NULL;
            __cmd_d_help (cmder);
            return 0;
            }

        addr      = (char *) strtoul (argv [1], NULL, 16);

        units     = argc > 2 ? atoi (argv [2]) : 0;

        unit_size = argc > 3 ? atoi (argv [3]) : 0;
        }

    /*
     * default:
     * if unit_size is not specified, set it to 1
     * if units     is not specified, show 64 bytes
     */

    unit_size = unit_size == 0 ? 1 : unit_size;
    units     = units == 0 ? 64 / unit_size : units;

    switch (unit_size)
        {
        case 1: __dump_mem (cmder, addr, 0, units); break;
        case 2: __dump_mem (cmder, addr, 1, units); break;
        case 4: __dump_mem (cmder, addr, 2, units); break;
        case 8: __dump_mem (cmder, addr, 3, units); break;
        default:
            cmder_printf (cmder, "invalid length!");
            break;
        }

    addr += units * unit_size;

    cmder_printf (cmder, "\n");

    return 0;
    }

CMDER_CMD_DEF ("d", "display memory", __cmd_d);

static void __cmd_m_help (cmder_t * cmder)
    {
    cmder_printf (cmder, "usage: m address [width]\n");
    cmder_printf (cmder, "modify memory content at address\n\n");
    cmder_printf (cmder, "if width is omited, 1 is picked\n");
    }

/*
 * if there is no valid input and then <enter>, then return 1, and this unit will
 * be skipped
 */

static int __get_new_val (cmder_t * cmder, uint64_t * pval)
    {
    unsigned char ch;
    unsigned char t;
    uint64_t      val   = 0;
    bool          first = true;

    while ((t = ch = cmder->getc (cmder->arg)) != '\r')
        {
        if (ch == 'q')
            {
            return -1;
            }

        if ((t >= '0') && (t <= '9'))
            {
            t -= '0';
            }
        else
            {
            t |= 0x20;

            if ((t < 'a') || (t > 'f'))
                {
                continue;
                }

            t = t - 'a' + 10;
            }

        cmder_printf (cmder, "%c", ch);

        first = false;

        val = (val << 4) + t;
        }

    if (first)
        {
        return 1;
        }

    *pval = val;

    return 0;
    }

static void __mod_mem (cmder_t * cmder, char * addr, int order)
    {
    uint8_t  u8  = 0;
    uint16_t u16 = 0;
    uint32_t u32 = 0;
    uint64_t u64 = 0;
    int      ret;

    while (1)
        {
        switch (order)
            {
            case 0: ret = mem_try (&u8,  addr, order); break;
            case 1: ret = mem_try (&u16, addr, order); break;
            case 2: ret = mem_try (&u32, addr, order); break;
            case 3: ret = mem_try (&u64, addr, order); break;
            default: return;    /* should never be here */
            }

        if (ret == -1)
            {
            cmder_printf (cmder, "can not read from address, abort!\n");
            return;
            }

        switch (order)
            {
            case 0: cmder_printf (cmder, "%08x:%02x-",    addr, u8);  break;
            case 1: cmder_printf (cmder, "%08x:%04x-",    addr, u16); break;
            case 2: cmder_printf (cmder, "%08x:%08x-",    addr, u32); break;
            case 3: cmder_printf (cmder, "%08x:%016llx-", addr, u64); break;
            default: return;    /* should never be here */
            }

        ret = __get_new_val (cmder, &u64);

        cmder_printf (cmder, "\n");

        if (ret == -1)
            {
            return;
            }

        if (ret == 0)
            {
            switch (order)
                {
                case 0: u8  = (uint8_t)  u64; ret = mem_try (addr, &u8,  order); break;
                case 1: u16 = (uint16_t) u64; ret = mem_try (addr, &u16, order); break;
                case 2: u32 = (uint32_t) u64; ret = mem_try (addr, &u32, order); break;
                case 3: u64 = (uint64_t) u64; ret = mem_try (addr, &u64, order); break;
                default: return;    /* should never be here */
                }

            if (ret == -1)
                {
                cmder_printf (cmder, "can not write to address, abort!\n");
                return;
                }
            }

        addr += 1 << order;
        }
    }

static int __cmd_m (cmder_t * cmder, int argc, char * argv [])
    {
    char * addr;
    int    width = 0;

    if (argc == 1)
        {
        cmder_printf (cmder, "error: no address specified!\n");
        __cmd_m_help (cmder);
        return -1;
        }

    if ((strcmp (argv [1], "--help") == 0) || (strcmp (argv [1], "/?") == 0))
        {
        __cmd_m_help (cmder);
        return 0;
        }

    addr = (char *) strtoul (argv [1], NULL, 16);

    if (argc > 2)
        {
        width = atoi (argv [2]);
        }

    width = width == 0 ? 1 : width;

    cmder_printf (cmder, "\nmodify memory in hex format, press 'q' to quit\n\n");

    switch (width)
        {
        case 1: __mod_mem (cmder, addr, 0); break;
        case 2: __mod_mem (cmder, addr, 1); break;
        case 4: __mod_mem (cmder, addr, 2); break;
        case 8: __mod_mem (cmder, addr, 3); break;
        default:
            cmder_printf (cmder, "invalid length!");
            break;
        }

    return 0;
    }

CMDER_CMD_DEF ("m", "modify memory", __cmd_m);

