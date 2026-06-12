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
 *  MODULE  : Interrupt subsystem                                                *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Implements registration array logic and exception translation      *
 ********************************************************************************/
#include <trunk/tros/kern/interrupts/interrupts.h>

#include <trunk/drivers/serial/serial.h>

#include <trunk/tros/kern/kabort.h>

namespace trunk::interrupts
{
    static InterruptHandler g_InterruptHandlers[256] = {nullptr};

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : register_interrupt_handler                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Assigns a custom C++ driver function to an IDT slot                *
     ********************************************************************************/
    void register_interrupt_handler(u8 vector, InterruptHandler handler) noexcept
    {
        g_InterruptHandlers[vector] = handler;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : execute_interrupt_handler                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Invoked to route traffic or detect unhandled traps                 *
     ********************************************************************************/
    void execute_interrupt_handler(u8 vector, InterruptFrame *frame) noexcept
    {
        InterruptHandler handler = g_InterruptHandlers[vector];

        if (handler != nullptr)
            handler(frame);
        else
        {
            if (vector < 32)
                kernel::kabort("Unhandled architectural processor exception trapped by dispatcher.");
        }
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : vector_to_status                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Translates raw CPU exceptions (0-31) to the STATUS enum            *
     ********************************************************************************/
    STATUS vector_to_status(u8 vector) noexcept
    {
        // clang-format off
        switch (vector)
        {
            case 0:  return STATUS_INTEGER_DIVIDE_BY_ZERO;
            case 1:  return STATUS_SINGLE_STEP_DEBUG;
            case 2:  return STATUS_NMI_HARDWARE_FAILURE;
            case 3:  return STATUS_BREAKPOINT;
            case 4:  return STATUS_INTEGER_OVERFLOW;
            case 5:  return STATUS_ARRAY_BOUNDS_EXCEEDED;
            case 6:  return STATUS_ILLEGAL_INSTRUCTION;
            case 7:  return STATUS_FLOAT_INVALID_OPERATION;
            case 8:  return STATUS_DOUBLE_FAULT;
            case 10: return STATUS_INVALID_DISPOSITION;
            case 11: return STATUS_IN_PAGE_ERROR;
            case 12: return STATUS_STACK_OVERFLOW;
            case 13: return STATUS_GENERAL_PROTECTION_FAULT;
            case 14: return STATUS_ACCESS_VIOLATION;
            case 16: return STATUS_FLOAT_STACK_CHECK;
            case 17: return STATUS_DATATYPE_MISALIGNMENT;
            case 18: return STATUS_MACHINE_CHECK;
            case 19: return STATUS_SIMD_FAULT;
            default: return STATUS_UNSUCCESSFUL;
        }
        // clang-format on
    }

} // namespace trunk::interrupts