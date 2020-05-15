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

MEMORY
{
    ROM (rx) : ORIGIN = 0x08000000, LENGTH = 0x00100000
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00030000
}
ENTRY(reset_handler)
SECTIONS
    {
    .text :
        {
        __text_start__ = .;
        __vectors_start__ = .;
        KEEP (*(.vectors))
        __vectors_end__ = .;
        *(.text*)
        KEEP (*(.init))
        KEEP (*(.fini))
        *crtbegin.o (.ctors)
        *crtbegin?.o (.ctors)
        *(EXCLUDE_FILE (*crtend?.o *crtend.o) .ctors)
        *(SORT (.ctors.*))
        *(.ctors)
        *crtbegin.o (.dtors)
        *crtbegin?.o (.dtors)
        *(EXCLUDE_FILE (*crtend?.o *crtend.o) .dtors)
        *(SORT (.dtors.*))
        *(.dtors)
        *(.stub)
        *(.gnu.warning)
        *(.gnu.linkonce.t*)
        *(.glue_7t)
        *(.glue_7)
        *(.rdata)
        *(.rodata)
        *(.rodata.*)
        *(.gnu.linkonce.r*)
        *(.rodata1)
        . = ALIGN (8);
        *(.eh_frame_hdr)
        *(.eh_frame_entry .eh_frame_entry.*)
        *(.eh_frame*)
        *(.gcc_except_table .gcc_except_table.*)
        . = ALIGN (16);
        __init_cpu_start__ = .;
        KEEP (*(.init_cpu))
        KEEP (*(.init_board))
        KEEP (*(.init_prekernel))
        KEEP (*(.init_class))
        KEEP (*(.init_kernel))
        KEEP (*(.init_postkernel))
        KEEP (*(.init_hal))
        KEEP (*(.init_intctr))
        KEEP (*(.init_intmux))
        KEEP (*(.init_bus))
        KEEP (*(.init_driver))
        KEEP (*(.init_postdriver))
        __init_user_start__ = .;
        KEEP (*(.init_user))
        __init_user_end__ = .;
        . = ALIGN (8);
        __cmder_start__ = .;
        KEEP (*(.cmder))
        __cmder_end__    = .;
        
        . = ALIGN(4);
        __oshell_start = .;
        KEEP (*(oshell))
        __oshell_end = .;
		
        . = ALIGN(4);
        __osdriv_start = .;
        KEEP (*(osdriv))
        __osdriv_end = .;

        . = ALIGN (8);
        } > ROM
    .ARM.extab :
        {
        *(.ARM.extab)
        *(.ARM.extab.*)
        *(.gnu.linkonce.armextab.*)
        } > ROM
    .ARM.exidx :
        {
        __exidx_start__ = .;
        *(.ARM.exidx)
        *(.ARM.exidx.*)
        *(.gnu.linkonce.armexidx.*)
        __exidx_end__ = .;
        } > ROM
    __text_end__ = (__exidx_end__ & ~7) + 8;
    .stack :
        {
        __stack_start__ = .;
        . += 0x800;
        __stack_end__ = .;
        __stack = .;
        } > RAM
    .data : AT (__text_end__)
        {
        __data_start__ = .;
        *(.data)
        *(.data*)
        *(.sdata .sdata.*)
        *(.gnu.linkonce.d*)
        SORT (CONSTRUCTORS)
        . = ALIGN (8);
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*)));
        KEEP (*(SORT(.init_array)));
        PROVIDE_HIDDEN (__init_array_end = .);
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*)));
        KEEP (*(SORT(.fini_array)));
        PROVIDE_HIDDEN (__fini_array_end = .);
        *(.jcr)
        *(.got.plt)
        *(.got)
        *(.dynamic)
        *(.got2)
        *(.lit8)
        *(.lit4)
        . = ALIGN (8);
        __tdata_start__ = .;
        *(.tdata*)
        . = ALIGN (8);
        __tdata_end__ = .;
        __data_end__ = .;
        } > RAM
    .bss :
        {
        __bss_start__ = .;
        *(.bss)
        *(.bss*)
        *(.sbss)
        *(.sbss*)
        *(COMMON)
        *(.gnu.linkonce.b*)
        . = ALIGN (8);
        __tbss_start__ = .;
        *(.tbss*)
        . = ALIGN (8);
        __tbss_end__ = .;
        __bss_end__ = .;
        end = .;
        __end__ = end;
        } > RAM
    }
