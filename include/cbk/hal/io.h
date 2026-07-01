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
 *  MODULE  : Assembly Instructions                                              *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Assembly instruction C++ wrappers()                                *
 ********************************************************************************/
#pragma once

#include <attributes.h>
#include <types.h>

namespace cbk::hal
{

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalOutB                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Write a byte to an I/O port.                                       *
     ********************************************************************************/
    INLINE VOID
    HalOutB(WORD port, BYTE value) noexcept
    {
        asm volatile("outb %0, %1" : : "a"(value), "Nd"(port) : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalOutW                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Write a word (2 bytes) to an I/O port.                             *
     ********************************************************************************/
    INLINE VOID
    HalOutW(WORD port, WORD value) noexcept
    {
        asm volatile("outw %0, %1" : : "a"(value), "Nd"(port) : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalOutL                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Write a dword (4 bytes) to an I/O port.                            *
     ********************************************************************************/
    INLINE VOID
    HalOutL(WORD port, DWORD value) noexcept
    {
        asm volatile("outl %0, %1" : : "a"(value), "Nd"(port) : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalInB                                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read a byte from an I/O port.                                      *
     ********************************************************************************/
    NO_DISCARD INLINE BYTE
    HalInB(WORD port) noexcept
    {
        BYTE value;
        asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port) : "memory");
        return value;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalInW                                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read a word (2 bytes) from an I/O port.                            *
     ********************************************************************************/
    NO_DISCARD INLINE WORD
    HalInW(WORD port) noexcept
    {
        WORD value;
        asm volatile("inw %1, %0" : "=a"(value) : "Nd"(port) : "memory");
        return value;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalInL                                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read a dword (4 bytes) from an I/O port.                           *
     ********************************************************************************/
    NO_DISCARD INLINE DWORD
    HalInL(WORD port) noexcept
    {
        DWORD value;
        asm volatile("inl %1, %0" : "=a"(value) : "Nd"(port) : "memory");
        return value;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalIoWait                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Brief I/O delay by writing to an unused port.                      *
     ********************************************************************************/
    INLINE VOID
    HalIoWait() noexcept
    {
        HalOutB(0x80, 0x00);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalRdMsr                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read a Model Specific Register.                                    *
     ********************************************************************************/
    NO_DISCARD INLINE QWORD
    HalRdMsr(DWORD msr) noexcept
    {
        DWORD low, high;
        asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
        return (static_cast<QWORD>(high) << 32) | low;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalWrMsr                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Write a Model Specific Register.                                   *
     ********************************************************************************/
    INLINE VOID
    HalWrMsr(DWORD msr, QWORD value) noexcept
    {
        DWORD low  = static_cast<DWORD>(value);
        DWORD high = static_cast<DWORD>(value >> 32);
        asm volatile("wrmsr" : : "c"(msr), "a"(low), "d"(high));
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalReadCr0                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read control register CR0.                                         *
     ********************************************************************************/
    NO_DISCARD INLINE QWORD
    HalReadCr0() noexcept
    {
        QWORD value;
        asm volatile("mov %%cr0, %0" : "=r"(value));
        return value;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalWriteCr0                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Write control register CR0.                                        *
     ********************************************************************************/
    INLINE VOID
    HalWriteCr0(QWORD value) noexcept
    {
        asm volatile("mov %0, %%cr0" : : "r"(value) : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalReadCr2                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read control register CR2.                                         *
     ********************************************************************************/
    NO_DISCARD INLINE QWORD
    HalReadCr2() noexcept
    {
        QWORD value;
        asm volatile("mov %%cr2, %0" : "=r"(value));
        return value;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalReadCr3                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read control register CR3.                                         *
     ********************************************************************************/
    NO_DISCARD INLINE QWORD
    HalReadCr3() noexcept
    {
        QWORD value;
        asm volatile("mov %%cr3, %0" : "=r"(value));
        return value;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalWriteCr3                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Write control register CR3.                                        *
     ********************************************************************************/
    INLINE VOID
    HalWriteCr3(QWORD value) noexcept
    {
        asm volatile("mov %0, %%cr3" : : "r"(value) : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalReadCr4                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read control register CR4.                                         *
     ********************************************************************************/
    NO_DISCARD INLINE QWORD
    HalReadCr4() noexcept
    {
        QWORD value;
        asm volatile("mov %%cr4, %0" : "=r"(value));
        return value;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalWriteCr4                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Write control register CR4.                                        *
     ********************************************************************************/
    INLINE VOID
    HalWriteCr4(QWORD value) noexcept
    {
        asm volatile("mov %0, %%cr4" : : "r"(value) : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalInvLpg                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Invalidate a single TLB entry for the given virtual address.       *
     ********************************************************************************/
    INLINE VOID
    HalInvLpg(ULONG_PTR vaddr) noexcept
    {
        asm volatile("invlpg (%0)" : : "r"(vaddr) : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalHalt                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Halt the CPU until the next interrupt.                             *
     ********************************************************************************/
    INLINE VOID
    HalHalt() noexcept
    {
        asm volatile("hlt");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalCli                                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Disable hardware interrupts.                                       *
     ********************************************************************************/
    INLINE VOID
    HalCli() noexcept
    {
        asm volatile("cli" : : : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalSti                                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Enable hardware interrupts.                                        *
     ********************************************************************************/
    INLINE VOID
    HalSti() noexcept
    {
        asm volatile("sti" : : : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalPause                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Hint to the CPU that this is a spin-wait loop.                     *
     ********************************************************************************/
    INLINE VOID
    HalPause() noexcept
    {
        asm volatile("pause");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalCpuid                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Execute HalCpuid instruction.                                      *
     ********************************************************************************/
    INLINE VOID
    HalCpuid(DWORD leaf, DWORD &eax, DWORD &ebx, DWORD &ecx, DWORD &edx) noexcept
    {
        asm volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(leaf), "c"(0));
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalRdtSc                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read the Time Stamp Counter.                                       *
     ********************************************************************************/
    NO_DISCARD INLINE QWORD
    HalRdtSc() noexcept
    {
        DWORD low, high;
        asm volatile("rdtsc" : "=a"(low), "=d"(high));
        return (static_cast<QWORD>(high) << 32) | low;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalMFence                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Execute the mfence assembly instruction                            *
     ********************************************************************************/
    INLINE VOID
    HalMFence() noexcept
    {
        __asm__ __volatile__("mfence" ::: "memory");
    }

} // namespace cbk::hal