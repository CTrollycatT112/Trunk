/* ******************************************************************************
 *                                                                              *
 *  Copyright 2026 Trollycat                                                    *
 *                                                                              *
 *  Licensed under the Apache License, Version 2.0 (the "License");             *
 *  you may not use this file except in compliance with the License.            *
 *  You may obtain a copy of the License at                                     *
 *                                                                              *
 *      http://www.apache.org/licenses/LICENSE-2.0                              *
 *                                                                              *
 *  Unless required by applicable law or agreed to in writing, software         *
 *  distributed under the License is distributed on an "AS IS" BASIS,           *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.    *
 *  See the License for the specific language governing permissions and         *
 *  limitations under the License.                                              *
 *                                                                              *
 ********************************************************************************
 *                                                                              *
 *  AUTHOR  : Trollycat                                                         *
 *  FILE    : boot.ld                                                           *
 *  DATE    : 2026                                                              *
 *  PURPOSE : Linker script for trboot.elf — the Trunk boot stage.              *
 *            All symbols resolve to physical addresses. No higher-half mapping.*
 *            GRUB loads trboot.elf at 0x100000 (1MB mark) and jumps to _start. *
 *            Boot.cpp runs the ELF loader, maps troskern.elf to higher-half,   *
 *            then jumps into virtual space. This script is purely physical.    *
 *                                                                              *
 * *****************************************************************************/

OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)

BOOT_BASE = 0x0000000000100000;

SECTIONS
{
    . = BOOT_BASE;

    .multiboot2 : {
        KEEP(*(.multiboot2))
    }

    .boot.text ALIGN(16) : {
        *(.boot.text)
        *(.boot.text.*)
    }

    .boot.rodata ALIGN(16) : {
        *(.boot.rodata)
        *(.boot.rodata.*)
    }

    .text ALIGN(0x1000) : {
        *(.text)
        *(.text.*)
    }

    .rodata ALIGN(0x1000) : {
        *(.rodata)
        *(.rodata.*)
    }

    .init_array ALIGN(8) : {
        __init_array_start = .;
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        __init_array_end = .;
    }

    .fini_array ALIGN(8) : {
        __fini_array_start = .;
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        __fini_array_end = .;
    }

    .data ALIGN(0x1000) : {
        *(.data)
        *(.data.*)
    }

    .bss ALIGN(0x1000) : {
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        __bss_end   = .;
    }

    .boot.stack ALIGN(0x1000) : {
        __boot_stack_bottom = .;
        . += 0x4000;
        __boot_stack_top    = .;
    }

    __boot_phys_start = BOOT_BASE;
    __boot_phys_end   = .;

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note.GNU-stack)
    }
}