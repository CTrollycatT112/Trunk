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
 *  AUTHOR  : Trollycat                                                          *
 *  MODULE  : Memory management unit                                             *
 *  DATE    : 2026                                                               *
 *  PURPOSE : CPU MMU driver                                                     *
 ********************************************************************************/
#include <cbk/mm/mmunit.h>

namespace cbk::mem
{
    ArchAspace *krnl_space = nullptr;

    namespace
    {

        using MmuPteAction = VOID (*)(PPAGE_TABLE_ENTRY target_pte, PTE_CONTEXT &ctx) noexcept;

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MmuMapIdentityPhysmap                                              *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Internal helper, map direct identity/physmap window                *
         ********************************************************************************/
        VOID MmuMapIdentityPhysmap() noexcept
        {
            PFN_NUM pg_hi        = MmGetHighestPhysicalPage();
            SIZE_T phys_map_size = pg_hi * PAGE_SIZE;

            CBKSTATUS status = MmuMapRange4K(PHYSMAP_BASE, 0x0, phys_map_size,
                                             PAGE_PRESENT | PAGE_WRITABLE | PAGE_GLOBAL);
            ASSERT(status == STATUS_SUCCESS,
                   "MmuMapIdentityPhysmap: Failed to map direct phys mem window");
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : IMmuMapSection                                                     *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Map a section                                                      *
         ********************************************************************************/
        VOID IMmuMapSection(PVOID section_start, PVOID section_end, QWORD hw_flags) noexcept
        {
            QWORD vstart = reinterpret_cast<QWORD>(section_start);
            QWORD pstart = vstart - KERNEL_VMA;

            SIZE_T size = reinterpret_cast<QWORD>(section_end) - vstart;

            CBKSTATUS status = MmuMapRange4K(vstart, pstart, size, hw_flags);
            ASSERT(status == STATUS_SUCCESS, "IMmuMapSection: Failed to map kernel section...?");
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MmuMapTextSection                                                  *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Protect .text section (EXECUTABLE CODE)                            *
         ********************************************************************************/
        VOID MmuMapTextSection() noexcept
        {
            IMmuMapSection(__text_start, __text_end, TEXT_SECTION_HW_FLAGS);
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MmuMapRoDataSection                                                *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Protect .rodata section (CONSTANT DATA)                            *
         ********************************************************************************/
        VOID MmuMapRoDataSection() noexcept
        {
            IMmuMapSection(__rodata_start, __rodata_end, RODATA_SECTION_HW_FLAGS);
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MmuMapDataBssSection                                               *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Protect .bss section (UNINITIALIZED DATA)                          *
         ********************************************************************************/
        VOID MmuMapDataBssSection() noexcept
        {
            IMmuMapSection(__bss_start, __stack_top, BSS_SECTION_HW_FLAGS);
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MmuGetTableIndex                                                   *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Extract 9-bit table index for a given level                        *
         ********************************************************************************/
        NO_DISCARD ULONG MmuGetTableIndex(QWORD virt, PAGING_LEVEL lvl) noexcept
        {
            return (virt >> static_cast<ULONG>(lvl)) & IDX_BITSHIFT;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MmuGetTablePointer                                                 *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Convert a physical frame back into a virtual page table pointer    *
         ********************************************************************************/
        NO_DISCARD PPAGE_TABLE MmuGetTablePointer(PAGE_TABLE_ENTRY entry) noexcept
        {
            QWORD phys_addr = static_cast<QWORD>(entry.Bits.page_frame) << PAGE_SHIFT;
            return reinterpret_cast<PPAGE_TABLE>(PaddrToKvaddr(phys_addr));
        }

        /* *******************************************************************************
         * AUTHOR  : Trollycat                                                           *
         * FUNC    : MmuExecuteOnPte                                                     *
         * DATE    : 2026                                                                *
         * PURPOSE : Walks down any tiers to leaf                                        *
         ********************************************************************************/
        NO_DISCARD CBKSTATUS MmuExecuteOnPte(QWORD virt, BOOL alloc_if_missing, MmuPteAction action,
                                             PTE_CONTEXT &ctx) noexcept
        {
            if ((virt & (PAGE_SIZE - 1)) != 0)
                return STATUS_DATATYPE_MISALIGNMENT;

            PPAGE_TABLE_ENTRY working_table =
                reinterpret_cast<PPAGE_TABLE_ENTRY>(PaddrToKvaddr(krnl_space->pml4_phys));

            constexpr PAGING_LEVEL steps[] = {PAGING_LEVEL::PML4, PAGING_LEVEL::PDPT,
                                              PAGING_LEVEL::PD};

            for (PAGING_LEVEL level : steps) {
                ULONG idx              = MmuGetTableIndex(virt, level);
                PAGE_TABLE_ENTRY entry = working_table[idx];

                if (!entry.Bits.present) {
                    if (!alloc_if_missing)
                        return STATUS_NOT_FOUND;

                    PFN_NUM new_pfn = MmAllocPage(static_cast<ULONG>(MC_TYPE::SYSTEM));
                    if (new_pfn == 0)
                        return STATUS_NO_MEMORY;

                    QWORD new_paddr = PfnToAddr(new_pfn);

                    PPAGE_TABLE_ENTRY new_tbl_virt =
                        reinterpret_cast<PPAGE_TABLE_ENTRY>(PaddrToKvaddr(new_paddr));

                    tklib::memset(new_tbl_virt, 0, PAGE_SIZE);

                    working_table[idx].val = new_paddr | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
                    entry                  = working_table[idx];
                }

                if (entry.Bits.large_page) {
                    if (!alloc_if_missing && ctx.extra == PHYS_ADDR_MAX)
                        action(&working_table[idx], ctx);
                    return STATUS_LARGE_PAGE;
                }

                working_table = reinterpret_cast<PPAGE_TABLE_ENTRY>(MmuGetTablePointer(entry));
            }

            ULONG pt_idx                 = MmuGetTableIndex(virt, PAGING_LEVEL::PT);
            PPAGE_TABLE_ENTRY target_pte = &working_table[pt_idx];
            QWORD old_val                = target_pte->val;

            action(target_pte, ctx);

            if (target_pte->val != old_val)
                hal::InvLpg(virt);

            return STATUS_SUCCESS;
        }

        /* *******************************************************************************
         * AUTHOR  : Trollycat                                                           *
         * FUNC    : MmuIterateRange                                                     *
         * DATE    : 2026                                                                *
         * PURPOSE : Range loop helper                                                   *
         ********************************************************************************/
        template <typename F>
        NO_DISCARD CBKSTATUS MmuIterateRange(QWORD start, SIZE_T size, F action) noexcept
        {
            if ((start & (PAGE_SIZE - 1)) != 0)
                return STATUS_DATATYPE_MISALIGNMENT;

            SIZE_T page_count = (size + PAGE_SIZE - 1) / PAGE_SIZE;

            for (SIZE_T i = 0; i < page_count; ++i) {
                CBKSTATUS status = action(start + (i * PAGE_SIZE), i);
                if (status != STATUS_SUCCESS)
                    return status;
            }

            return STATUS_SUCCESS;
        }

    } // namespace

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuWriteCr3                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Load a new root page table address into the CPU                    *
     ********************************************************************************/
    VOID MmuWriteCr3(QWORD pml4_phys) noexcept
    {
        ASSERT((pml4_phys & 0xFFF) == 0, "PML4 physical address must be page-aligned!");
        hal::WriteCr3(pml4_phys);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuInitializePerCpu                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialize the MMU driver on every CPU core                        *
     ********************************************************************************/
    VOID MmuInitPerCpu() noexcept
    {
        // MULTI-CORE NOT ADDED
        // TODO: INIT PER CPU
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuInitialize                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Runs once on Core 0, wraps ArchAspace                              *
     ********************************************************************************/
    VOID MmuInitialize(ArchAspace *space) noexcept
    {
        ASSERT(space != nullptr, "MmuInitialize: Kernel address space cannot be nullptr");
        krnl_space = space;

        // Map the massive direct physmap (window)
        MmuMapIdentityPhysmap();

        // PROTECTION: SECTIONS
        // THIS APPLIES SPECIFIC HW_FLAGS TO EACH SECTION
        // THIS PROTECTS THEM WHEN IT COMES TO WRITING AND READING
        MmuMapTextSection();
        MmuMapRoDataSection();
        MmuMapDataBssSection();

        // Throw hw switch to verified tables
        MmuWriteCr3(krnl_space->pml4_phys);

        // DOES NOTHING
        MmuInitPerCpu();
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuMapPage4K                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Walks down the 4 levels, allocates missing sub-tables              *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MmuMapPage4K(QWORD virt, QWORD phys, QWORD flags) noexcept
    {
        ASSERT((phys & (PAGE_SIZE - 1)) == 0, "MmuMapPage4K: Physical frame is not aligned");

        PTE_CONTEXT ctx{(phys & PAGE_MASK) | flags | PAGE_PRESENT, 0};

        CBKSTATUS status = MmuExecuteOnPte(
            virt, TRUE,

            [](PPAGE_TABLE_ENTRY pte, PTE_CONTEXT &c) noexcept {
                if (pte->Bits.present) {
                    c.extra = 1;
                    return;
                }
                pte->val = c.payload;
            },
            ctx);

        if (status == STATUS_SUCCESS && ctx.extra == 1)
            return STATUS_CONFLICTING_ADDRESSES;
        return status;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuUnmapPage4K                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Walks down the final PTE, clears the entry, executes TLB flush     *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MmuUnmapPage4K(QWORD virt) noexcept
    {
        PTE_CONTEXT ctx{0, 0};
        return MmuExecuteOnPte(
            virt, FALSE,

            [](PPAGE_TABLE_ENTRY pte, PTE_CONTEXT &) noexcept {
                pte->val = 0;
            },

            ctx);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmuMapRange4K                                                       *
     * DATE    : 2026                                                                *
     * PURPOSE : Maps a continuous virtual range to a continuous physical range      *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MmuMapRange4K(QWORD vstart, QWORD pstart, SIZE_T size,
                                       QWORD flags) noexcept
    {
        ASSERT((pstart & (PAGE_SIZE - 1)) == 0, "MmuMapRange4K: pstart must be page-aligned");
        return MmuIterateRange(vstart, size, [=](QWORD vaddr, SIZE_T index) noexcept {
            return MmuMapPage4K(vaddr, pstart + (index * PAGE_SIZE), flags);
        });
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmuUnmapRange4K                                                     *
     * DATE    : 2026                                                                *
     * PURPOSE : Unmaps a continuous block of 4KB pages in one call                  *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MmuUnmapRange4K(QWORD start, SIZE_T size) noexcept
    {
        return MmuIterateRange(start, size, [](QWORD vaddr, SIZE_T) noexcept {
            return MmuUnmapPage4K(vaddr);
        });
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuProtectPage4K                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Modify an existing PTE's attribute bits                            *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MmuProtectPage4K(QWORD virt, QWORD flags) noexcept
    {
        PTE_CONTEXT ctx{flags, 0};
        return MmuExecuteOnPte(
            virt, FALSE,

            [](PPAGE_TABLE_ENTRY pte, PTE_CONTEXT &c) noexcept {
                if (!pte->Bits.present)
                    return;
                QWORD original_phys = static_cast<QWORD>(pte->Bits.page_frame) << PAGE_SHIFT;
                pte->val            = (original_phys & PAGE_MASK) | c.payload | PAGE_PRESENT;
            },

            ctx);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuIsRangeFree                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks if a range of virtual addresses has any existing mappings   *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MmuIsRangeFree(QWORD start, SIZE_T size) noexcept
    {
        return MmuIterateRange(start, size, [](QWORD vaddr, SIZE_T) noexcept {
            if (MmuTranslateVirtualToPhysical(vaddr) != PHYS_ADDR_MAX)
                return STATUS_CONFLICTING_ADDRESSES;
            return STATUS_SUCCESS;
        });
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuIsPagePresent                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks if a page is present                                        *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MmuIsPagePresent(QWORD virt) noexcept
    {
        CBKSTATUS status = MmuIsRangeFree(virt, PAGE_SIZE);
        if (status == STATUS_CONFLICTING_ADDRESSES)
            return STATUS_SUCCESS;
        return (status == STATUS_SUCCESS) ? STATUS_NOT_FOUND : status;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuTranslateVirtualToPhysical                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Finds what physical frame address is mapped to a virtual pointer   *
     ********************************************************************************/
    NO_DISCARD QWORD MmuTranslateVirtualToPhysical(QWORD virt) noexcept
    {
        PTE_CONTEXT ctx{virt, PHYS_ADDR_MAX};

        CBKSTATUS status = MmuExecuteOnPte(
            virt, FALSE,
            [](PPAGE_TABLE_ENTRY pte, PTE_CONTEXT &c) noexcept {
                if (!pte->Bits.present)
                    return;
                QWORD mask = pte->Bits.large_page ? HUGE_MASK : (PAGE_SIZE - 1);
                c.extra =
                    (static_cast<QWORD>(pte->Bits.page_frame) << PAGE_SHIFT) + (c.payload & mask);
            },
            ctx);

        return (status != STATUS_SUCCESS && status != STATUS_LARGE_PAGE) ? PHYS_ADDR_MAX
                                                                         : ctx.extra;
    }

} // namespace cbk::mem