/* *******************************************************************************
 *                                                                               *
 *  Copyright 2026 Trollycat                                                     *
 *                                                                               *
 *  Licensed under the Apache License, Version 2.0 (the "License");              *
 *  you may not use this file except in compliance with the License.             *
 *  You may obtain a copy of the License at                                      *
 *                                                                               *
 *      http://www.apache.org/licenses/LICENSE-2.0                               *
 *                                                                               *
 *  Unless required by applicable law or agreed to in writing, software          *
 *  distributed under the License is distributed on an "AS IS" BASIS,            *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.     *
 *  See the License for the specific language governing permissions and          *
 *  limitations under the License.                                               *
 *                                                                               *
 *********************************************************************************
 *                                                                               *
 *  AUTHOR  : Trollycat                                                          *
 *  MODULE  : Global Descriptor Table                                            *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Defines and initalizes the permanent 64-bit                        *
 *            Global Descriptor Table.                                           *
 *                                                                               *
 ********************************************************************************/

#pragma once

#include <trunk/tros/gdt/tss.h>

#include <types.h>
#include <macros.h>

namespace trunk::gdt
{
    inline constexpr u8 GDT_PRESENT = 0x80;
    inline constexpr u8 GDT_RING0 = 0x00;
    inline constexpr u8 GDT_RING3 = 0x60;
    inline constexpr u8 GDT_SYSTEM = 0x10;
    inline constexpr u8 GDT_EXECUTABLE = 0x08;
    inline constexpr u8 GDT_READ_WRITE = 0x02;
    inline constexpr u8 GDT_LONG_MODE = 0x20;

    struct GNU_PACKED GdtEntry
    {
        u16 limit_low;
        u16 base_low;
        u8 base_middle;
        u8 access;
        u8 flags_limit_high;
        u8 base_high;

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : create                                                             *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Creates a new GdtEntry with passed in paramaters                   *
         ********************************************************************************/

        static constexpr GdtEntry create(u8 access, u8 flags) noexcept
        {
            return GdtEntry{
                .limit_low = 0,
                .base_low = 0,
                .base_middle = 0,
                .access = access,
                .flags_limit_high = static_cast<u8>((flags & 0xF0)),
                .base_high = 0};
        }
    };

    struct GNU_PACKED TssDescriptor
    {
        u16 limit_low;
        u16 base_low;
        u8 base_middle;

        u8 type : 4;
        u8 zero : 1;
        u8 dpl : 2;
        u8 p : 1;

        u8 limit_high : 4;
        u8 avl : 1;
        u8 l : 1;
        u8 db : 1;
        u8 g : 1;

        u8 base_high;
        u32 base_upper;
        u32 reserved;
    };

    struct GNU_PACKED GdtLayout
    {
        GdtEntry null_desc;
        GdtEntry kernel_code;
        GdtEntry kernel_data;
        GdtEntry user_code;
        GdtEntry user_data;
        TssDescriptor tss_desc;
    };

    struct GNU_PACKED GdtPointer
    {
        u16 limit;
        uptr base;
    };
    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : gdt_init                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initializes the global descriptor table                            *
     ********************************************************************************/
    void gdt_init() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : gdt_flush                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Flushes/Reloads the global descriptor table (external assembly)    *
     ********************************************************************************/
    extern "C" void gdt_flush(uptr gdt_ptr_addr) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : gdt_install_tss                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Installs the TSS                                                   *
     ********************************************************************************/
    [[nodiscard]] u16 gdt_install_tss(const Tss *tss_ptr) noexcept;

} // namespace trunk::gdt
