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

#include <cbk/mm/mappag.h>

namespace cbk::mem
{
    ArchAspace *krnl_space = nullptr;

    namespace
    {

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MmuMapIdentityPhysmap                                              *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Internal helper, map direct identity/physmap window                *
         ********************************************************************************/
        VOID
        MmuMapIdentityPhysmap() noexcept
        {
            PFN_NUM pg_hi        = MmGetHighestPhysicalPage();
            SIZE_T phys_map_size = pg_hi * PAGE_SIZE;

            CBKSTATUS status = MapRange4K(PHYSMAP_BASE,
                                          0x0,
                                          phys_map_size,
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
        VOID
        IMmuMapSection(PVOID section_start, PVOID section_end, QWORD hw_flags) noexcept
        {
            QWORD vstart = reinterpret_cast<QWORD>(section_start);
            QWORD pstart = vstart - KERNEL_VMA;

            SIZE_T size = reinterpret_cast<QWORD>(section_end) - vstart;

            CBKSTATUS status = MapRange4K(vstart, pstart, size, hw_flags);
            ASSERT(status == STATUS_SUCCESS, "IMmuMapSection: Failed to map kernel section...?");
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MmuMapTextSection                                                  *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Protect .text section (EXECUTABLE CODE)                            *
         ********************************************************************************/
        VOID
        MmuMapTextSection() noexcept
        {
            IMmuMapSection(__text_start, __text_end, TEXT_SECTION_HW_FLAGS);
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MmuMapRoDataSection                                                *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Protect .rodata section (CONSTANT DATA)                            *
         ********************************************************************************/
        VOID
        MmuMapRoDataSection() noexcept
        {
            IMmuMapSection(__rodata_start, __rodata_end, RODATA_SECTION_HW_FLAGS);
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MmuMapDataBssSection                                               *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Protect .bss section (UNINITIALIZED DATA)                          *
         ********************************************************************************/
        VOID
        MmuMapDataBssSection() noexcept
        {
            IMmuMapSection(__bss_start, __stack_top, BSS_SECTION_HW_FLAGS);
        }
    } // namespace

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuGetTableIndex                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Extract 9-bit table index for a given level                        *
     ********************************************************************************/
    NO_DISCARD ULONG
    MmuGetTableIndex(QWORD virt, PAGING_LEVEL lvl) noexcept
    {
        return static_cast<ULONG>((virt >> static_cast<ULONG>(lvl)) & IDX_BITSHIFT);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuGetTablePointer                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Convert a physical frame back into a virtual page table pointer    *
     ********************************************************************************/
    NO_DISCARD PPAGE_TABLE
    MmuGetTablePointer(PAGE_TABLE_ENTRY entry) noexcept
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
    NO_DISCARD CBKSTATUS
    MmuExecuteOnPte(QWORD virt,
                    PAGING_LEVEL target_level,
                    BOOL alloc_if_missing,
                    MmuPteAction action,
                    PTE_CONTEXT &ctx) noexcept
    {
        QWORD alignment_mask =
            (target_level == PAGING_LEVEL::PD) ? (HUGE_PAGE_SIZE - 1) : (PAGE_SIZE - 1);
        if ((virt & alignment_mask) != 0)
            return STATUS_DATATYPE_MISALIGNMENT;

        PPAGE_TABLE_ENTRY target_entry = nullptr;
        PAGING_LEVEL resolved_level    = PAGING_LEVEL::PML4;

        CBKSTATUS status = MmuWalkToTable(krnl_space->pml4_phys,
                                          virt,
                                          target_level,
                                          alloc_if_missing,
                                          target_entry,
                                          resolved_level);
        if (status != STATUS_SUCCESS)
            return status;

        QWORD old_val = target_entry->val;
        action(target_entry, ctx);

        if (target_entry->val != old_val)
            hal::InvLpg(virt);
        if (target_entry->Bits.large_page && resolved_level != target_level)
            return STATUS_LARGE_PAGE;

        return STATUS_SUCCESS;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuWalkToTable                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Walks down the page tables to a specific level                     *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmuWalkToTable(QWORD pml4_phys,
                   QWORD virt,
                   PAGING_LEVEL target_level,
                   BOOL alloc_if_missing,
                   PPAGE_TABLE_ENTRY &out_entry,
                   PAGING_LEVEL &out_resolved_level) noexcept
    {
        PPAGE_TABLE working_table      = reinterpret_cast<PPAGE_TABLE>(PaddrToKvaddr(pml4_phys));
        constexpr PAGING_LEVEL steps[] = {PAGING_LEVEL::PML4, PAGING_LEVEL::PDPT, PAGING_LEVEL::PD};

        for (PAGING_LEVEL level : steps) {

            ULONG idx          = MmuGetTableIndex(virt, level);
            out_resolved_level = level;
            out_entry          = &working_table->entries[idx];

            if (level == target_level || out_entry->Bits.large_page)
                return STATUS_SUCCESS;

            if (!out_entry->Bits.present) {
                if (!alloc_if_missing)
                    return STATUS_NOT_FOUND;

                PFN_NUM new_pfn = MmAllocPage(static_cast<ULONG>(MC_TYPE::SYSTEM));
                if (new_pfn == 0)
                    return STATUS_NO_MEMORY;

                QWORD new_paddr          = PfnToAddr(new_pfn);
                PPAGE_TABLE new_tbl_virt = reinterpret_cast<PPAGE_TABLE>(PaddrToKvaddr(new_paddr));

                tklib::memset(new_tbl_virt, 0, PAGE_SIZE);
                out_entry->val = new_paddr | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
            }

            working_table = MmuGetTablePointer(*out_entry);
        }

        if (target_level == PAGING_LEVEL::PT) {
            ULONG pt_idx       = MmuGetTableIndex(virt, PAGING_LEVEL::PT);
            out_resolved_level = PAGING_LEVEL::PT;
            out_entry          = &working_table->entries[pt_idx];
            return STATUS_SUCCESS;
        }

        return STATUS_NOT_FOUND;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuProtectRange                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Update protection flags across a range                             *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmuProtectRange(QWORD start_addr, SIZE_T size, ULONG new_protection) noexcept
    {
        QWORD hw_bits_to_set   = 0;
        QWORD hw_bits_to_clear = PAGE_WRITABLE | PAGE_NX;

        if (new_protection & PAGE_READWRITE)
            hw_bits_to_set |= PAGE_WRITABLE;
        if (!(new_protection & PAGE_EXECUTE))
            hw_bits_to_set |= PAGE_NX;

        PTE_CONTEXT ctx{};
        ctx.extra   = hw_bits_to_clear;
        ctx.payload = hw_bits_to_set;

        auto protect_action = [](PPAGE_TABLE_ENTRY pte, PTE_CONTEXT &context) noexcept -> VOID {
            pte->val = (pte->val & ~context.extra) | context.payload;
        };

        for (QWORD addr = start_addr; addr < start_addr + size; addr += PAGE_SIZE) {
            CBKSTATUS status = MmuExecuteOnPte(addr, PAGING_LEVEL::PT, FALSE, protect_action, ctx);
            if (status != STATUS_SUCCESS)
                return status;
        }

        return STATUS_SUCCESS;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuWriteCr3                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Load a new root page table address into the CPU                    *
     ********************************************************************************/
    VOID
    MmuWriteCr3(QWORD pml4_phys) noexcept
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
    VOID
    MmuInitPerCpu() noexcept
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
    VOID
    MmuInitialize(ArchAspace *space) noexcept
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
} // namespace cbk::mem