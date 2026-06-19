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
 *  MODULE  : Memory types                                                       *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Stores common memory types and handlers                            *
 ********************************************************************************/
#pragma once

#include <types.h>

namespace trunk::mem
{
    // TODO: THIS IS A FORWARD DECL, NOT DEFINED YET.
    // TO BE DEFINED DURING PS.
    struct EProcess;
    using PEPROCESS = EProcess *;

    struct MmRmapEntry
    {
        MmRmapEntry *next;
        PEPROCESS process;
        PVOID virtual_address;
    };
    using PMM_RMAP_ENTRY = MmRmapEntry *;

    enum class MM_PFN_STATE : BYTE
    {
        ZEROED_PAGE_LIST = 0,
        FREE_PAGE_LIST   = 1,
        ACTIVE_AND_VALID = 6
    };

    struct FreeAreaNode
    {
        FreeAreaNode *next;
    };

    struct MmPfn
    {
        BYTE order;
        MM_PFN_STATE page_location;
        FreeAreaNode node;
        PMM_RMAP_ENTRY rmap_list_head;
        ULONG reference_count;
    };

    using MMPFN   = MmPfn;
    using PMMPFN  = MmPfn *;
    using PPMMPFN = MmPfn **;

    using PFN_NUM = QWORD;

} // namespace trunk::mem