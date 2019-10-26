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

/*
high level description
----------------------

0) cmder shell struct

   0.0 cmder shell is assumed to support more than one entity, more than way:

       > it can be openned in two uart consoles at the same time
       > it can be openned through eth and uart at the same time

       for each entity, these is a <cmder_t> struct for it

   0.1 cmder support customer added command, as this kernel is designed as a
       real-time embedded system, it only support register command statically,
       meaning at build time

       commands are descripted by the struct of <cmder_cmd_t>

1) cli editing

the customer input is held in a buffer in cmder_t::input_buff, and this buffer
is divided into three parts, they are refered by:

    * cmder_t::input_idx
    * cmder_t::compl_idx
    * cmder_t::shift_idx

all of them begin from zero and the sum of them is not greater than the size of
the buffer subtract one for a ending '\0'. and so there are three Implicit buffers:

    * input buffer, it is just cmder_t::input_buff
    * compl buffer, it is just cmder_t::input_buff + cmder_t::input_idx it is
                    used when <tab> or <ctrl-i> keys hit and process the command
                    completion
    * shift buffer, it is special, it is used to save the characters when <left>,
                    <home> or <ctrl-b> keys hit, its base address is the end of
                    cmder_t::input_buff and increase from high to low just like
                    stacks

these three buff/index shown as:

 .------------------------ cmder->input_buff -------------------------.
/                                                                      \
+---------------------+---------------------+-----+---------------------+
|/////////////////////|\\\\\\\\\\\\\\\\\\\\\|     |/////////////////////|
+---------------------+---------------------+-----+---------------------+
|0           input_idx|0           compl_idx|empty|shift_idx           0|
|increase direction ->|increase direction ->|empty|<- increase direction|
+---------------------+---------------------+-----+---------------------+
|characters before the|completion buff, size|     |characters behind the|
|cursor               |is usually 0         |     |cursor               |
                                            ^
                                          cursor

for example, if there is a command with the name of "test-cmd", and then you
just typed in the word "tes foo", and then 4 <left> keys, and then the command
line should be:

    cmder> tes foo
    $$$$$$$iiissss

> the characters above '$' is the command line prompt
> the characters above 'i' is the "input" area, the first part in the tab
> the characters above 's' is the "shift" area, the last part in the tab
> and the cursor now is just between "s "

And then type the "tab" key, and then the command line should be:

    cmder> test-cmd foo
    $$$$$$$iiicccccssss

> the characters above 'c' is the "compl" area, the second part in the tab
> and the cursor now is just between "d "

NOTE: even you see the string in command line is *one* continuous string, it
may be three parts in the <input_buff>:

     .-- input_buff (with the content of "rt-")
    /
    +-------------------+
    |test-cmd~~~~~~~oof |
    +-------------------+
        |    \     /   \
        |     `-+-'     `-- shift_buff growth down (with the content of "oof ")
        \       '-- scratch, not used (hold garbage content)
         `-- compl_buff (with the content of "t-cmd")

when processing the "tab", when any other key except "tab" hit, the current
command will be selected, the "compl" area will be appended to the "input"
area, the "compl" area will be 0 again

before processing the command, these three area will be join to one continuous
string.

2) history processing

the cmder shell support command history, but the number of history commands is
limited, for saving memory, the buffer hold the history is fixed. so when the
buffer is full, the oldest command is removed. this is just a typical fifo
behavior, so the history commands will a ending NUL are saved in a ring buffer,
implemented in <ring.c>, just like this:

cmder_t::his_cmd:

     .-- the ring buffer start of <cmder_t::his_cmd>
    /
    | arg1 arg2\0cmd0 0\0cmd1 arg0 arg1\0cmd2 arg0 arg1 arg2\0cmd3 arg0|
     333333333333000000001111111111111111222222222222222222222333333333
                 ^       ^               ^                    ^
              idx0(11) idx1(18)        idx2(33)             idx2(54)

as all history commands are saved in the <his_cmd> ring, and every records have
different length, so the index (the ring tail when inserting) must be saved as
a reference so the record can be get back. as the indexes should also be a fifo
behavior, so the indexes are also saved in a ring (cmder_t::his_idx).

so the previous example of <cmder_t::his_cmd> may have following index ring:

     .-- the ring buffer start of <cmder_t::his_idx>
    /
    | 0x000b | 0x0012 | 0x0021 | 0x0036 |
     idx0(11) idx1(18) idx2(33) idx3(64)

in storage, the two rings looks like this:

    +------+------+---+------+
    | idx0 | idx1 | ~ | idxn |                (his_idx ring)
    +------+------+---+------+
      |      \          \
      |       `---+      `--------+
      |           |               |
    +-----------+-----------+---+-----------+
    | cmd0 args | cmd1 args | ~ | cmdn args | (his_cmd ring)
    +-----------+-----------+---+-----------+

when a record in <cmder_t::his_cmd> is overwrote, the corresponding index saved
in <cmder_t::his_idx> must be removed. as the overwrote only happen when the ring
is full:

    +----+
    |    |`\
    |    | |
    |    | +- history ring buff, and is full now
    |    | |
    |    | /
    +----+'
    |    | <- a index for history (refered as his_idx)
    +----+

    so there must be: (ring->tail - his_idx) > (ring->size)

every time a new history is inserted, the overwrote indexes is removed.

3) command registeration and management

commands are managed in a static array

4) cmder shell front-end

this library is designed for different shells (as descripted in section 0),
different front-end should implement there own support routines and then invoke
this library.
a front-end should implement three function pointers:
    int  (* getchar) (uintptr_t);
    void (* putchar) (uintptr_t, char);
a front-end should define there own cmder_t control struct

to add a new cmder front-end please refer to <cmder_uart.c>

5) special keys and shortcuts

ascii code from 0x00 ~ 0x1f are for NUL, ctrl-x, ESC and other invisable keys
ascii code from 0x20 ~ 0x7e are visiable keys
ascii code 0x7f are for del
code from 0x80 ~ 0xff is not really standard and this shell will not support them
functional keys like <home>, <end>, <up>, <down>, <left>, <right> usually generate
more than one key by the terminal, and different terminal have different behavior
so the unsupported codes from 0x80 ~ 0x85 are used to stand for these special
keys, a cmder front-end should convert these keys to the codes this library can
recognize (0x80 ~ 0x85), please refer to <cmder_uart.c> as an example.

cmder shell support some shortcut keys (have similar behovior has them in other
shells):

    * ctrl-a, move cursor to the front of line, just like <home>
    * ctrl-b, move cursor backward, just like <left>
    * ctrl-c, cancel the input, and start a new prompt
    * ctrl-d, kill a character behind the cursor, just like <del>
    * ctrl-e, move cursor to the end of line, just like <end>
    * ctrl-f, move cursor forward, just like <right>
    * ctrl-h, kill a character before the cursor, just like <baskcpase>
    * ctrl-i, command completion, just like <tab>
    * ctlr-l, clean the screen
    * ctrl-j, executive the command have been typed, behavior same as <enter>
    * ctrl-k, kill all characters until to the end of line
    * ctrl-m, executive the command have been typed, behavior same as <enter>
    * ctrl-n, iterate history backward, just like <down>
    * ctrl-o, executive the command have been typed, behavior same as <enter>
    * ctrl-p, iterate history forward, just like <up>
    * ctrl-u, kill all characters until to the begin of line
    * ctrl-w, kill a word before cursor
    * <home>, move cursor to the front of line
    * <end>,  move cursor to the end of line
    * <up>,   iterate history forward
    * <down>, iterate history backward
    * <del>,  kill a character behind the cursor
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#include <config.h>
#include <common.h>
#include <cmder.h>
#include <list.h>
#include <xprintf.h>

/* externs */

/* locals */

static size_t         cmder_max_len = 0;
static cmder_cmd_t  * cmder_cmds;
static unsigned int   cmder_nr_cmds;

static inline bool __is_buff_full (cmder_t * cmder)
    {
    int len = cmder->input_idx + cmder->compl_idx + cmder->shift_idx;

    /* reserve one byte for the ending '\0' */

    return len == (CMDER_BUFFER_SIZE - 1);
    }

static inline void __input_buff_put (cmder_t * cmder, char ch)
    {
    if (__is_buff_full (cmder))
        {
        return;
        }

    cmder->input_buff [cmder->input_idx++] = ch;
    }

static inline char __input_buff_get (cmder_t * cmder, int idx)
    {
    return cmder->input_buff [idx];
    }

static inline void __compl_buff_put (cmder_t * cmder, char ch)
    {
    if (__is_buff_full (cmder))
        {
        return;
        }

    cmder->input_buff [cmder->input_idx + cmder->compl_idx++] = ch;
    }

static inline char __compl_buff_get (cmder_t * cmder, int idx)
    {
    return cmder->input_buff [cmder->input_idx + idx];
    }

static inline void __shift_buff_put (cmder_t * cmder, char ch)
    {
    if (__is_buff_full (cmder))
        {
        return;
        }

    cmder->input_buff [CMDER_BUFFER_SIZE - (++cmder->shift_idx)] = ch;
    }

static inline char __shift_buff_get (cmder_t * cmder, int idx)
    {
    return cmder->input_buff [CMDER_BUFFER_SIZE - idx - 1];
    }

static inline void __print_banner (cmder_t * cmder)
    {
    cmder_printf (cmder, "\nWellcome to LiteOS NOVA!\n");
    cmder_printf (cmder, "build time: %s %s\n", __DATE__, __TIME__);
    cmder_printf (cmder, "build tool: %s\n",
#if   defined (__KEIL__)
                         "ARM KEIL"
#endif
#if   defined (__SESA__)
                         "Segger Embedded Studio for ARM"
#endif

#if   defined (__clang__)
                         "+LLVM"
#elif defined (__GNUC__)
                         "+GNUC"
#elif defined (_MSC_VER)
                         "+MSVC"
#endif

                 );
    }

static inline void __print_prompt (cmder_t * cmder)
    {
    int i;

    cmder_printf (cmder, "\rcmder> ");

    for (i = 0; i < cmder->line_end; i++)
        {
        cmder->putc (cmder->arg, ' ');
        }

    while (cmder->line_end)
        {
        cmder->putc (cmder->arg, '\b');
        cmder->line_end--;
        }

    cmder->position = 0;
    }

/**
 * refresh the command line interface show
 */

static inline void __print_cli (cmder_t * cmder)
    {
    int i, kill;

    if (cmder->input_idx > cmder->position)
        {
        while (cmder->input_idx > cmder->position)
            {
            cmder->putc (cmder->arg, __input_buff_get (cmder, cmder->position));
            cmder->position++;
            }
        }
    else
        {

        /* must processing tab, go to the end of input_buff */

        while (cmder->position > cmder->input_idx)
            {
            cmder->putc (cmder->arg, '\b');
            cmder->position--;
            }
        }

    for (i = 0; i < cmder->compl_idx; i++)
        {
        cmder->putc (cmder->arg, __compl_buff_get (cmder, i));
        }

    cmder->position += cmder->compl_idx;

    kill = cmder->line_end - (cmder->position + cmder->shift_idx);

    cmder->line_end = cmder->position + cmder->shift_idx;

    if (kill == 0)
        {
        return;
        }

    i = cmder->shift_idx;

    while (i)
        {
        cmder->putc (cmder->arg, __shift_buff_get (cmder, --i));
        }

    kill = kill < 0 ? 0 : kill;

    /* kill garbage left by previous command in the end */

    for (i = 0; i < kill; i++)
        {
        cmder->putc (cmder->arg, ' ');
        }

    /* move cursor back to the front of "shift" position */

    for (i = 0; i < kill + cmder->shift_idx; i++)
        {
        cmder->putc (cmder->arg, '\b');
        }
    }

static int __tab_loop_pfn (cmder_t * cmder, const cmder_cmd_t * cmd)
    {
    int          ret;
    const char * name = cmd->name;
    char *       buf  = cmder->input_buff;
    size_t       len  = cmder->input_idx;

    while ((*buf == ' ') && len)
        {
        buf++;
        len--;
        }

    if (strncmp (buf, name, len))
        {
        return 0;
        }

    if (name [len] == '\0')
        {
        return 0;
        }

    name += len;

    cmder->compl_idx = 0;

    while (*name)
        {
        __compl_buff_put (cmder, *name++);
        }

    __print_cli (cmder);

    ret = cmder->getc (cmder->arg);

    return ret == '\t' ? 0 : ret;
    }

static inline int __cmd_foreach (cmder_t * cmder,
                                 int (* pfn) (cmder_t *, const cmder_cmd_t *))
    {
    unsigned int i;
    int          ret;

    for (i = 0; i < cmder_nr_cmds; i++)
        {
        ret = pfn (cmder, &cmder_cmds [i]);

        /* if pfn return non-zero value means stop the iteration */

        if (ret != 0)
            {
            return ret;
            }
        }

    return 0;
    }

static inline int __tab_loop (cmder_t * cmder)
    {
    return __cmd_foreach (cmder, __tab_loop_pfn);
    }

static inline cmder_cmd_t * __find_cmd (char * name)
    {
    unsigned int i;

    for (i = 0; i < cmder_nr_cmds; i++)
        {
        if (strcmp (cmder_cmds [i].name, name) == 0)
            {
            return &cmder_cmds [i];
            }
        }

    return NULL;
    }

static inline void __cursor_raw_left (cmder_t * cmder)
    {
    __shift_buff_put (cmder, __input_buff_get (cmder, --cmder->input_idx));
    cmder->putc (cmder->arg, '\b');
    cmder->position--;
    }

static inline void __cursor_left (cmder_t * cmder)
    {
    if (cmder->input_idx != 0)
        {
        __cursor_raw_left (cmder);
        }
    }

static inline void __cursor_home (cmder_t * cmder)
    {
    while (cmder->input_idx != 0)
        {
        __cursor_raw_left (cmder);
        }
    }

static inline void __cursor_raw_right (cmder_t * cmder)
    {
    __input_buff_put (cmder, __shift_buff_get (cmder, --cmder->shift_idx));
    cmder->putc (cmder->arg, __input_buff_get (cmder, cmder->input_idx - 1));
    cmder->position++;
    }

static inline void __cursor_right (cmder_t * cmder)
    {
    if (cmder->shift_idx != 0)
        {
        __cursor_raw_right (cmder);
        }
    }

static inline void __cursor_end (cmder_t * cmder)
    {
    while (cmder->shift_idx != 0)
        {
        __cursor_raw_right (cmder);
        }
    }

static inline void __kill_forward (cmder_t * cmder)
    {
    if (cmder->shift_idx != 0)
        {
        cmder->shift_idx--;
        }
    }

static inline void __kill_to_end (cmder_t * cmder)
    {
    while (cmder->shift_idx != 0)
        {
        cmder->shift_idx--;
        }
    }

static inline void __kill_backward (cmder_t * cmder)
    {
    if (cmder->input_idx)
        {
        cmder->input_idx--;
        }
    }

static inline void __kill_to_home (cmder_t * cmder)
    {
    while (cmder->input_idx)
        {
        cmder->input_idx--;
        }
    }

static inline void __kill_word (cmder_t * cmder)
    {
    while (cmder->input_idx != 0)
        {
        if (__input_buff_get (cmder, cmder->input_idx - 1) != ' ')
            {
            break;
            }

        __kill_backward (cmder);
        }

    while (cmder->input_idx != 0)
        {
        if (__input_buff_get (cmder, cmder->input_idx - 1) == ' ')
            {
            break;
            }

        __kill_backward (cmder);
        }
    }

static inline int __str2arg (char * argstr, char * argv [])
    {
    int argc = 0;

    while (*argstr)
        {
        if (*argstr == ' ')
            {
            *argstr = '\0';
            }
        else
            {
            if (argc == CMDER_NR_ARGUMENTS)
                {
                break;
                }

            if ((argc == 0) || (argstr [-1] == '\0'))
                {
                argv [argc++] = argstr;
                }
            }

        argstr++;
        }

    return argc;
    }

static inline uint16_t __get_his_idx (cmder_t * cmder, size_t meta_idx)
    {
    union
        {
        uint16_t      u16;
        unsigned char ch [2];
        } u_idx;

    u_idx.ch [0] = ring_peek (&cmder->his_idx, (meta_idx << 1) + 0);
    u_idx.ch [1] = ring_peek (&cmder->his_idx, (meta_idx << 1) + 1);

    return u_idx.u16;
    }

static inline void __put_his_idx (cmder_t * cmder, uint16_t idx)
    {
    union
        {
        uint16_t      u16;
        unsigned char ch [2];
        } u_idx;

    u_idx.u16 = idx;

    ring_putc_force (&cmder->his_idx, u_idx.ch [0]);
    ring_putc_force (&cmder->his_idx, u_idx.ch [1]);
    }

/**
 * check if the command inserting is the same with the last one
 */

static inline bool __his_is_last (cmder_t * cmder, int argc, char * argv [])
    {
    int    i = 0;
    size_t idx;

    /* check if the same with the last command */

    idx = __get_his_idx (cmder, ring_len (&cmder->his_idx) / sizeof (uint16_t) - 1);
    idx = idx - cmder->his_cmd.head;

    do
        {
        char * part = argv [i];
        char   ch;

        do
            {
            if (*part != ring_peek (&cmder->his_cmd, idx++))
                {
                return false;
                }
            } while (*++part);

        ch = ring_peek (&cmder->his_cmd, idx++);

        switch (ch)
            {
            case '\0':
                return i == (argc - 1);
            case ' ':
                continue;
            default:
                return false;
            }
        } while (++i != argc);

    return false;
    }

static inline void __insert_his (cmder_t * cmder, int argc, char * argv [])
    {
    int    i = 0;
    size_t idx;

    /* check if the same with the last command */

    if (__his_is_last (cmder, argc, argv))
        {
        return;
        }

    /* push the insert point of <his_cmd> ring to <his_idx> ring */

    __put_his_idx (cmder, (uint16_t) cmder->his_cmd.tail);

    /* put argv [0 ~ n - 1] to <his_cmd> ring with an extra '\0' in the end */

    while (1)
        {
        ring_put_force (&cmder->his_cmd, (unsigned char *) argv [i],
                        strlen (argv [i]));

        if (++i == argc)
            {
            ring_putc_force (&cmder->his_cmd, '\0');
            break;
            }

        ring_putc_force (&cmder->his_cmd, ' ');
        }

    /* remove the indexes in <his_idx> ring for those overwrote command records */

    while (1)
        {
        idx = __get_his_idx (cmder, 0);

        if (cmder->his_cmd.tail - idx <= cmder->his_cmd.size)
            {
            break;
            }

        ring_get (&cmder->his_idx, (unsigned char *) &idx, sizeof (uint16_t));
        }
    }

static inline void __print_his (cmder_t * cmder, size_t idx)
    {
    char ch;
    int  i;

    while (cmder->position)
        {
        cmder->putc (cmder->arg, '\b');
        cmder->position--;
        }

    while ((ch = ring_peek (&cmder->his_cmd, idx++)) != '\0')
        {
        cmder->putc (cmder->arg, ch);
        cmder->position++;
        }

    for (i = cmder->position; i < cmder->line_end; i++)
        {
        cmder->putc (cmder->arg, ' ');
        }

    while (i-- != cmder->position)
        {
        cmder->putc (cmder->arg, '\b');
        }

    cmder->line_end = cmder->position;
    }

/**
 * select (copy to cmder_t::input_buff) the history record referenced by the
 * index in <his_cmd> ring
 */

static inline void __peak_his (cmder_t * cmder, size_t idx)
    {
    int  i;

    cmder->shift_idx = 0;
    cmder->input_idx = 0;

    for (i = 0; i < cmder->position; i++)
        {
        __input_buff_put (cmder, ring_peek (&cmder->his_cmd, idx++));
        }

    idx += cmder->line_end - cmder->position;

    while (i++ < cmder->line_end)
        {
        __shift_buff_put (cmder, ring_peek (&cmder->his_cmd, --idx));
        }
    }

static inline int __his_loop (cmder_t * cmder)
    {
    size_t            idxes = ring_len (&cmder->his_idx) / sizeof (uint16_t);
    size_t            itr;
    bool              refresh = true;
    size_t            idx = 0;
    unsigned char     ch;

    if (idxes == 0)
        {
        return '\0';
        }

    itr = idxes - 1;

    /*
     *                 itr-.
     *                     |
     * +------+------+---+------+
     * | idx0 | idx1 | ~ | idxn |                (his_idx ring)
     * +------+------+---+------+
     *   |      \          \
     *   |       `---+      `--------+
     *   |           |               |
     * +-----------+-----------+---+-----------+
     * | cmd0 args | cmd1 args | ~ | cmdn args | (his_cmd ring)
     * +-----------+-----------+---+-----------+
     *
     * assume "cmdn" is the last history command record, and <itr> now is just
     * the index of its corresponding index in <his_idx>, itr is index's index
     */

    while (1)
        {
        if (refresh)
            {
            idx = __get_his_idx (cmder, itr) - cmder->his_cmd.head;
            __print_his (cmder, idx);
            refresh = false;
            }

        ch = cmder->getc (cmder->arg);

        switch (ch)
            {
            case cmder_vk_ctrla:
            case cmder_vk_home:
                while (cmder->position != 0)
                    {
                    cmder->putc (cmder->arg, '\b');
                    cmder->position--;
                    }
                break;
            case cmder_vk_ctrlb:
            case cmder_vk_left:
                if (cmder->position != 0)
                    {
                    cmder->putc (cmder->arg, '\b');
                    cmder->position--;
                    }
                break;
            case cmder_vk_ctrlf:
            case cmder_vk_right:
                if (cmder->position != cmder->line_end)
                    {
                    cmder->putc (cmder->arg, ring_peek (&cmder->his_cmd,
                                                           idx + cmder->position));
                    cmder->position++;
                    }
                break;
            case cmder_vk_ctrle:
            case cmder_vk_end:
                while (cmder->position != cmder->line_end)
                    {
                    cmder->putc (cmder->arg, ring_peek (&cmder->his_cmd,
                                                           idx + cmder->position));
                    cmder->position++;
                    }
                break;
            case cmder_vk_ctrlp:
            case cmder_vk_up:
                if (itr != 0)
                    {
                    itr--;
                    refresh = true;
                    }

                break;
            case cmder_vk_ctrln:
            case cmder_vk_down:
                if (++itr == idxes)     /* back to the input buffer */
                    {
                    __print_prompt (cmder);
                    __print_cli  (cmder);
                    return 0;
                    }

                refresh = true;

                break;
            default:
                __peak_his (cmder, idx);

                return ch;
            }
        }
    }

static int __cmd_history (cmder_t * cmder, int argc, char * argv [])
    {
    size_t            idxes = ring_len (&cmder->his_idx) / sizeof (uint16_t);
    size_t            itr;

    for (itr = 0; itr < idxes; itr++)
        {
        size_t        idx;
        unsigned char ch;

        idx = __get_his_idx (cmder, itr) - cmder->his_cmd.head;

        do
            {
            ch = ring_peek (&cmder->his_cmd, idx++);
            cmder->putc (cmder->arg, ch);
            } while (ch != '\0');

        cmder_printf (cmder, "\n");
        }

    return 0;
    }

CMDER_CMD_DEF ("history",
               "show command history, continuous same commands will only save once",
               __cmd_history);

static int __show_cmd_pfn (cmder_t * cmder, const cmder_cmd_t * cmd)
    {
    cmder_printf (cmder, "%-*s - %s\n", cmder_max_len, cmd->name, cmd->desc);
    return 0;
    }

static int __cmd_help (cmder_t * cmder, int argc, char * argv [])
    {
    __cmd_foreach (cmder, __show_cmd_pfn);
    return 0;
    }

CMDER_CMD_DEF ("?",    "show help content (which you are seeing)", __cmd_help);
CMDER_CMD_DEF ("help", "show help content (which you are seeing)", __cmd_help);

static inline int __do_cmd_arg (cmder_t * cmder, int argc, char * argv [])
    {
    const cmder_cmd_t * cmd = __find_cmd (argv [0]);

    cmder_printf (cmder, "\n");

    if (cmd != NULL)
        {
        return cmd->cmd (cmder, argc, argv);
        }

    cmder_printf (cmder, "\"%s\" command not found!\n", argv [0]);

    return -1;
    }

static inline int __do_cmd (cmder_t * cmder)
    {
    int    argc;
    char * argv [CMDER_NR_ARGUMENTS];
    int    ret = -1;

    while (cmder->shift_idx)
        {
        __input_buff_put (cmder, __shift_buff_get (cmder, --cmder->shift_idx));
        }

    /* make sure there is an ending NUL */

    cmder->input_buff [cmder->input_idx] = '\0';

    argc = __str2arg (cmder->input_buff, argv);

    if (argc != 0)
        {
        __insert_his (cmder, argc, argv);

        ret = __do_cmd_arg (cmder, argc, argv);
        }
    else
        {
        cmder->putc (cmder->arg, '\n');
        }

    return ret;
    }

static inline void __join_compl (cmder_t * cmder)
    {
    cmder->input_idx += cmder->compl_idx;
    cmder->compl_idx  = 0;
    }

static inline int __do_tab (cmder_t * cmder)
    {
    int ch = __tab_loop (cmder);

    if (ch == '\0')
        {

        /*
         * done a tab loop circle and then another tab here, just
         * reset the compl buffer
         */

        cmder->compl_idx = 0;

        __print_cli (cmder);
        }
    else
        {

        /*
         * any other key, means current completion is selected, just append to
         * the input buffer
         */

        __join_compl (cmder);
        }

    return ch;
    }

int cmder_printf (cmder_t * cmder, const char * format, ...)
    {
    va_list valist;
    int     nbytes;

    va_start (valist, format);
    nbytes = xprintf (format, valist, cmder->putc, (uintptr_t) cmder->arg);
    va_end (valist);

    return nbytes;
    }

static inline int __cmder_exec (cmder_t * cmder)
    {
    while (1)
        {
        int ch = cmder->getc (cmder->arg);

reswitch:

        switch (ch)
            {
            case cmder_vk_nul:
                continue;
            case cmder_vk_ctrla:
                __cursor_home (cmder);
                continue;
            case cmder_vk_ctrlb:
                __cursor_left (cmder);
                continue;
            case cmder_vk_ctrlc:
                cmder_printf  (cmder, "^C\n");
                return -1;
            case cmder_vk_ctrld:
                __kill_forward (cmder);
                break;
            case cmder_vk_ctrle:
                __cursor_end (cmder);
                continue;
            case cmder_vk_ctrlf:
                __cursor_right (cmder);
                continue;
            case cmder_vk_ctrlg:
                continue;
            case cmder_vk_ctrlh:        /* backspace */
                __kill_backward (cmder);
                break;
            case cmder_vk_ctrli:
                ch = __do_tab (cmder);
                goto reswitch;
            case cmder_vk_ctrlj:
                return __do_cmd (cmder);
            case cmder_vk_ctrlk:
                __kill_to_end (cmder);
                break;
            case cmder_vk_ctrll:
                cmder->putc (cmder->arg, (unsigned char) ch);
                __print_prompt (cmder);
                __print_cli  (cmder);
                continue;
            case cmder_vk_ctrlm:
                return __do_cmd (cmder);
            case cmder_vk_ctrln:
                continue;
            case cmder_vk_ctrlo:
                return __do_cmd (cmder);
            case cmder_vk_ctrlp:
                ch = __his_loop (cmder);
                goto reswitch;
            case cmder_vk_ctrlq:
            case cmder_vk_ctrlr:
            case cmder_vk_ctrls:
            case cmder_vk_ctrlt:
                continue;
            case cmder_vk_ctrlu:
                __kill_to_home (cmder);
                break;
            case cmder_vk_ctrlv:
                continue;
            case cmder_vk_ctrlw:
                __kill_word (cmder);
                break;
            case cmder_vk_ctrlx:
            case cmder_vk_ctrly:
            case cmder_vk_ctrlz:
            case cmder_vk_esc:
            case cmder_vk_fs:
            case cmder_vk_gs:
            case cmder_vk_rs:
            case cmder_vk_us:
                continue;
            case cmder_vk_del:
                __kill_forward (cmder);
                break;
            case cmder_vk_home:
                __cursor_home (cmder);
                continue;
            case cmder_vk_end:
                __cursor_end (cmder);
                continue;
            case cmder_vk_up:
                ch = __his_loop (cmder);
                goto reswitch;
            case cmder_vk_down:
                continue;               /* do nothing */
            case cmder_vk_left:
                __cursor_left (cmder);
                continue;
            case cmder_vk_right:
                __cursor_right (cmder);
                continue;
            default:
                if (ch & 0x80)
                    {
                    continue;
                    }

                __input_buff_put (cmder, (char) ch);

                break;
            }

        __print_cli (cmder);
        }
    }

static inline void __cmder_init (void)
    {
    unsigned int i;
    size_t       len;

    cmder_cmds     = (cmder_cmd_t *) (__section_start__ (CMDER_SECTION));
    cmder_nr_cmds  = (unsigned int)  (__section_end__   (CMDER_SECTION) -
                                      __section_start__ (CMDER_SECTION));

    cmder_nr_cmds /= sizeof (cmder_cmd_t);

    for (i = 0; i < cmder_nr_cmds; i++)
        {
        len = strlen (cmder_cmds [i].name);

        if (len > cmder_max_len)
            {
            cmder_max_len = len;
            }
        }
    }

__noreturn int cmder_loop (uintptr_t arg)
    {
    cmder_t * cmder = (cmder_t *) arg;

    __cmder_init ();
    __print_banner (cmder);

    while (1)
        {
        cmder->input_idx = 0;
        cmder->compl_idx = 0;
        cmder->shift_idx = 0;
        cmder->line_end  = 0;
        cmder->position  = 0;

        __print_prompt (cmder);

        (void) __cmder_exec (cmder);
        }
    }

