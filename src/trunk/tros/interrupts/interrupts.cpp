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
#include <trunk/tros/interrupts/interrupts.h>

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

} // namespace trunk::interrupts