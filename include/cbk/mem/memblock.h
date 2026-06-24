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
 *  MODULE  : Memblock                                                           *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Early boot-stage allocator, PFN is not available at this time      *
 ********************************************************************************/
#pragma once

// Memblock is used when we're in early boot-stage, and need to allocate some space to startup the
// advanced memory management system.
// For example, the PFN database needs an allocated spot in memory for it's structures
// After the system is allocated and setup, memblock is discarded and never used again
// Think of it like the 'initramfs' of memory management

// Usually in production operating systems, they manipulate addresses and stuff to actually fully
// get rid of the memblock For Trunk, we're just gonna stop using it Doing that only saves a few
// bits of memory, which isn't important for Trunk

namespace cbk::mem
{

} // namespace cbk::mem