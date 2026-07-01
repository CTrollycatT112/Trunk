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
 *  MODULE  : Programmable Interrupt Controller                                  *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Driver for the 8259 PIC chips                                      *
 ********************************************************************************/
#pragma once

#include <status.h>
#include <types.h>

namespace cbk::drivers::pic
{
    INLINE_CONST BYTE PIC1 = 0x20;
    INLINE_CONST BYTE PIC2 = 0xA0;

    INLINE_CONST BYTE PIC1_COMMAND = PIC1;
    INLINE_CONST BYTE PIC1_DATA    = PIC1 + 1;
    INLINE_CONST BYTE PIC2_COMMAND = PIC2;
    INLINE_CONST BYTE PIC2_DATA    = PIC2 + 1;

    INLINE_CONST BYTE PIC_EOI = 0x20;

    INLINE_CONST BYTE PIC1_OFFSET = 0x20;
    INLINE_CONST BYTE PIC2_OFFSET = 0x28;

    INLINE_CONST BYTE ICW1_INIT = 0x11;
    INLINE_CONST BYTE ICW4_8086 = 0x01;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalInitializePic                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialize the PIC driver                                          *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    HalInitializePic() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalPicProcessInterruptRequest                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Signals that an interrupt is being processed                       *
     ********************************************************************************/
    VOID
    HalPicProcessInterruptRequest(BYTE irq) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalPicMaskInterruptRequest                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Mask an IRQ (interrupt request)                                    *
     ********************************************************************************/
    VOID
    HalPicMaskInterruptRequest(BYTE irq) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalPicUnmaskInterruptRequest                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Unmask an IRQ (interrupt request)                                  *
     ********************************************************************************/
    VOID
    HalPicUnmaskInterruptRequest(BYTE irq) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalPicDisableDriver                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Disable the PIC driver                                             *
     ********************************************************************************/
    VOID
    HalPicDisableDriver() noexcept;
} // namespace cbk::drivers::pic