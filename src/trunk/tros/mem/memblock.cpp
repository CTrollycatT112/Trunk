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
 *  MODULE  : Memory management system                                           *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Memory allocator for early boot stage.                             *
 ********************************************************************************/

#include <trunk/tros/mem/memblock.h>

extern "C" char __kernel_phys_start[];
extern "C" char __kernel_phys_end[];

namespace trunk::mem
{
    static MemoryRegion s_memory_regions[MAX_MEMBLOCK_REGIONS];
    static MemoryRegion s_reserved_regions[MAX_MEMBLOCK_REGIONS];

    static usize s_memory_count = 0;
    static usize s_reserved_count = 0;

    namespace
    {
        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : align_up                                                           *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Alignment utility function                                         *
         ********************************************************************************/
        [[nodiscard]] constexpr u64 align_up(u64 address, u64 alignment) noexcept
        {
            if (alignment == 0 || (address % alignment) == 0) [[likely]]
                return address;
            return address + (alignment - (address % alignment));
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : sort_regions                                                       *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Sort the memory regions so no full space is left behind            *
         ********************************************************************************/
        void sort_regions(MemoryRegion *regions, usize count) noexcept
        {
            if (count < 2)
                return;

            for (usize i = 0; i < count - 1; ++i)
            {
                for (usize j = 0; j < count - i - 1; ++j)
                {
                    if (regions[j].base > regions[j + 1].base)
                    {
                        MemoryRegion temp = regions[j];
                        regions[j] = regions[j + 1];
                        regions[j + 1] = temp;
                    }
                }
            }
        }
    } // namespace

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : memblock_init                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialization function for memblock                               *
     ********************************************************************************/
    void memblock_init(const boot::BootInfo &boot_info) noexcept
    {
        s_memory_count = 0;
        s_reserved_count = 0;

        for (usize i = 0; i < boot_info.mmap_count; ++i)
        {
            s_memory_count = 0;
            s_reserved_count = 0;

            for (usize i = 0; i < boot_info.mmap_count; ++i)
            {
                const auto &entry = boot_info.mmap[i];

                if (entry.available())
                {
                    ASSERT(s_memory_count < MAX_MEMBLOCK_REGIONS, "EXCEEDED MAX_MEMBLOCK_REGIONS IN MEMORY TRACKER");
                    s_memory_regions[s_memory_count++] = {entry.base, entry.length};
                }
                else
                {
                    memblock_reserve(entry.base, entry.length);
                }
            }

            u64 k_start = reinterpret_cast<u64>(__kernel_phys_start);
            u64 k_end = reinterpret_cast<u64>(__kernel_phys_end);
            memblock_reserve(k_start, k_end - k_start);

            sort_regions(s_memory_regions, s_memory_count);
            sort_regions(s_reserved_regions, s_reserved_count);
        }
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : memblock_alloc                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Allocate a new chunk inside the memblock region                    *
     ********************************************************************************/
    [[nodiscard]] u64 memblock_alloc(u64 size, u64 alignment) noexcept
    {
        if (size == 0 || alignment == 0) [[unlikely]]
            return 0;

        for (usize i = 0; i < s_memory_count; ++i)
        {
            const u64 region_start = s_memory_regions[i].base;
            const u64 region_end = region_start + s_memory_regions[i].size;

            u64 candidate = align_up(region_start, alignment);

            while (candidate + size <= region_end)
            {
                bool overlapped = false;

                for (usize j = 0; j < s_reserved_count; ++j)
                {
                    const u64 res_start = s_reserved_regions[j].base;
                    const u64 res_end = res_start + s_reserved_regions[j].size;

                    if (candidate < res_end && (candidate + size) > res_start)
                    {
                        overlapped = true;
                        candidate = align_up(res_end, alignment);
                        break;
                    }
                }

                if (!overlapped)
                {
                    memblock_reserve(candidate, size);
                    return candidate;
                }
            }
        }

        return 0;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : memblock_reserve                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Reserve a region inside the memblock                               *
     ********************************************************************************/
    void memblock_reserve(u64 base, u64 size) noexcept
    {
        if (size == 0)
            return;
        ASSERT(s_reserved_count < MAX_MEMBLOCK_REGIONS, "Reserved region count exceeds MAX_MEMBLOCK_REGIONS");

        s_reserved_regions[s_reserved_count++] = {base, size};

        sort_regions(s_reserved_regions, s_reserved_count);
    }

} // namespace trunk::mem