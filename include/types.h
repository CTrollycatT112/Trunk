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
 *  MODULE  : Global definitions                                                 *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Type aliases                                                       *
 ********************************************************************************/
#pragma once

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

#ifdef __cplusplus
#define INLINE inline
#define CONSTEXPR constexpr
#define CONST const
#define INLINE_CONST INLINE CONSTEXPR
#else
#define INLINE
#define CONSTEXPR
#define CONST
#define INLINE_CONST
#endif

using BYTE  = uint8_t;
using WORD  = uint16_t;
using DWORD = uint32_t;
using QWORD = uint64_t;

using CHAR     = char;
using SHORT    = int16_t;
using LONG     = int32_t;
using LONGLONG = int64_t;

using UCHAR     = uint8_t;
using USHORT    = uint16_t;
using ULONG     = uint32_t;
using ULONGLONG = uint64_t;

using SIZE_T    = size_t;
using PSIZE_T   = size_t *;
using ULONG_PTR = uintptr_t;
using LONG_PTR  = intptr_t;

using BOOL  = bool;
using PBOOL = bool *;

using VOID    = void;
using PVOID   = void *;
using LPCVOID = CONST void *;

using PBYTE = BYTE *;

using PWORD      = WORD *;
using PDWORD     = DWORD *;
using PQWORD     = QWORD *;
using PCHAR      = CHAR *;
using PLONG      = LONG *;
using PULONG     = ULONG *;
using PULONG_PTR = ULONG **;
using PCSTR      = CONST char *;
using PWSTR      = wchar_t *;
using PCWSTR     = CONST wchar_t *;

using HANDLE  = void *;
using PHANDLE = HANDLE *;

INLINE_CONST ULONG_PTR INVALID_HANDLE_VALUE = ~ULONG_PTR{0};

struct EProcess;
using PEPROCESS = EProcess *;

struct ExceptionRecord;
using PEXCEPTION_RECORD = ExceptionRecord *;

struct ExceptionPointers;
using PEXCEPTION_POINTERS = ExceptionPointers *;

using PFN_NUM  = QWORD;
using PPFN_NUM = QWORD *;

using PFN_COUNT = QWORD;

namespace limits
{
    INLINE_CONST BYTE BYTE_min   = 0;
    INLINE_CONST BYTE BYTE_max   = UINT8_MAX;
    INLINE_CONST WORD WORD_min   = 0;
    INLINE_CONST WORD WORD_max   = UINT16_MAX;
    INLINE_CONST DWORD DWORD_min = 0;
    INLINE_CONST DWORD DWORD_max = UINT32_MAX;
    INLINE_CONST QWORD QWORD_min = 0;
    INLINE_CONST QWORD QWORD_max = UINT64_MAX;

    INLINE_CONST CHAR CHAR_min = CHAR_MIN;
    INLINE_CONST CHAR CHAR_max = CHAR_MAX;

    INLINE_CONST SHORT SHORT_min       = INT16_MIN;
    INLINE_CONST SHORT SHORT_max       = INT16_MAX;
    INLINE_CONST LONG LONG_min         = INT32_MIN;
    INLINE_CONST LONG LONG_max         = INT32_MAX;
    INLINE_CONST LONGLONG LONGLONG_min = INT64_MIN;
    INLINE_CONST LONGLONG LONGLONG_max = INT64_MAX;

    INLINE_CONST SIZE_T SIZE_T_max       = SIZE_MAX;
    INLINE_CONST ULONG_PTR ULONG_PTR_max = UINTPTR_MAX;
    INLINE_CONST LONG_PTR LONG_PTR_min   = INTPTR_MIN;
    INLINE_CONST LONG_PTR LONG_PTR_max   = INTPTR_MAX;
} // namespace limits

namespace cbk::mem
{
    struct ListEntry;
    using LIST_ENTRY  = ListEntry;
    using PLIST_ENTRY = ListEntry *;

    struct MmRmapEntry;
    using PMM_RMAP_ENTRY = MmRmapEntry *;

    struct MmPfn;
    using MMPFN   = MmPfn;
    using PMMPFN  = MmPfn *;
    using PPMMPFN = MmPfn **;

    struct MmVad;
    using MMVAD  = MmVad;
    using PMMVAD = MmVad *;

    struct MmPte;
    using MMPTE  = MmPte;
    using PMMPTE = MmPte *;
} // namespace cbk::mem

INLINE_CONST BOOL TRUE  = true;
INLINE_CONST BOOL FALSE = false;

#pragma GCC diagnostic pop