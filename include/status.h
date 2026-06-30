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
 *  MODULE  : Global definition                                                  *
 *  DATE    : 2026                                                               *
 *  PURPOSE : NT-style STATUS codes                                              *
 ********************************************************************************/
#pragma once

#include <types.h>

typedef LONG CBKSTATUS;

// clang-format off
#define NT_SUCCESS(Status)           (((CBKSTATUS)(Status)) >= 0)
#define NT_INFORMATION(Status)       ((((ULONG)(Status)) >> 30) == 1)
#define NT_WARNING(Status)           ((((ULONG)(Status)) >> 30) == 2)
#define NT_ERROR(Status)             ((((ULONG)(Status)) >> 30) == 3)
#define MAKE_STATUS(x)               static_cast<CBKSTATUS>(x##U)
// clang-format on

// SUCCESS CODES (0x00000000 - 0x3FFFFFFF)
constexpr CBKSTATUS STATUS_SUCCESS      = MAKE_STATUS(0x00000000);
constexpr CBKSTATUS STATUS_WAIT_0       = MAKE_STATUS(0x00000000);
constexpr CBKSTATUS STATUS_WAIT_TIMEOUT = MAKE_STATUS(0x00000102);
constexpr CBKSTATUS STATUS_PENDING      = MAKE_STATUS(0x00000103);
constexpr CBKSTATUS STATUS_REPARSE      = MAKE_STATUS(0x00000104);
constexpr CBKSTATUS STATUS_MORE_ENTRIES = MAKE_STATUS(0x00000105);
constexpr CBKSTATUS STATUS_LARGE_PAGE   = MAKE_STATUS(0x00000106);

// INFORMATIONAL CODES (0x40000000 - 0x7FFFFFFF)
constexpr CBKSTATUS STATUS_OBJECT_NAME_EXISTS   = MAKE_STATUS(0x40000000);
constexpr CBKSTATUS STATUS_THREAD_WAS_SUSPENDED = MAKE_STATUS(0x40000001);

// WARNING CODES (0x80000000 - 0xBFFFFFFF)
constexpr CBKSTATUS STATUS_GUARD_PAGE_VIOLATION  = MAKE_STATUS(0x80000001);
constexpr CBKSTATUS STATUS_DATATYPE_MISALIGNMENT = MAKE_STATUS(0x80000002);
constexpr CBKSTATUS STATUS_BREAKPOINT            = MAKE_STATUS(0x80000003);
constexpr CBKSTATUS STATUS_SINGLE_STEP           = MAKE_STATUS(0x80000004);
constexpr CBKSTATUS STATUS_BUFFER_OVERFLOW       = MAKE_STATUS(0x80000005);

// ERROR CODES (0xC0000000 - 0xFFFFFFFF)

// -- General System Errors --
constexpr CBKSTATUS STATUS_UNSUCCESSFUL             = MAKE_STATUS(0xC0000001);
constexpr CBKSTATUS STATUS_NOT_IMPLEMENTED          = MAKE_STATUS(0xC0000002);
constexpr CBKSTATUS STATUS_INVALID_INFO_CLASS       = MAKE_STATUS(0xC0000003);
constexpr CBKSTATUS STATUS_INFO_LENGTH_MISMATCH     = MAKE_STATUS(0xC0000004);
constexpr CBKSTATUS STATUS_ACCESS_VIOLATION         = MAKE_STATUS(0xC0000005);
constexpr CBKSTATUS STATUS_INVALID_HANDLE           = MAKE_STATUS(0xC0000008);
constexpr CBKSTATUS STATUS_INVALID_PARAMETER        = MAKE_STATUS(0xC000000D);
constexpr CBKSTATUS STATUS_NO_SUCH_DEVICE           = MAKE_STATUS(0xC000000E);
constexpr CBKSTATUS STATUS_NO_SUCH_FILE             = MAKE_STATUS(0xC000000F);
constexpr CBKSTATUS STATUS_MORE_PROCESSING_REQUIRED = MAKE_STATUS(0xC0000016);

// -- Memory & VMM Errors --
constexpr CBKSTATUS STATUS_NO_MEMORY                = MAKE_STATUS(0xC0000017);
constexpr CBKSTATUS STATUS_CONFLICTING_ADDRESSES    = MAKE_STATUS(0xC0000018);
constexpr CBKSTATUS STATUS_NOT_MAPPED_VIEW          = MAKE_STATUS(0xC0000019);
constexpr CBKSTATUS STATUS_UNABLE_TO_FREE_VM        = MAKE_STATUS(0xC000001A);
constexpr CBKSTATUS STATUS_UNABLE_TO_DELETE_SECTION = MAKE_STATUS(0xC000001B);
constexpr CBKSTATUS STATUS_ILLEGAL_INSTRUCTION      = MAKE_STATUS(0xC000001D);
constexpr CBKSTATUS STATUS_PAGEFILE_QUOTA           = MAKE_STATUS(0xC000001C);
constexpr CBKSTATUS STATUS_COMMITMENT_LIMIT         = MAKE_STATUS(0xC000012D);
constexpr CBKSTATUS STATUS_INSUFFICIENT_RESOURCES   = MAKE_STATUS(0xC000009A);

// -- Security & Access Errors --
constexpr CBKSTATUS STATUS_ACCESS_DENIED        = MAKE_STATUS(0xC0000022);
constexpr CBKSTATUS STATUS_BUFFER_TOO_SMALL     = MAKE_STATUS(0xC0000023);
constexpr CBKSTATUS STATUS_OBJECT_TYPE_MISMATCH = MAKE_STATUS(0xC0000024);
constexpr CBKSTATUS STATUS_LOGON_FAILURE        = MAKE_STATUS(0xC000006D);
constexpr CBKSTATUS STATUS_PRIVILEGE_NOT_HELD   = MAKE_STATUS(0xC0000061);

// -- Object & File System Errors --
constexpr CBKSTATUS STATUS_OBJECT_NAME_INVALID     = MAKE_STATUS(0xC0000033);
constexpr CBKSTATUS STATUS_OBJECT_NAME_NOT_FOUND   = MAKE_STATUS(0xC0000034);
constexpr CBKSTATUS STATUS_OBJECT_NAME_COLLISION   = MAKE_STATUS(0xC0000035);
constexpr CBKSTATUS STATUS_PORT_DISCONNECTED       = MAKE_STATUS(0xC0000037);
constexpr CBKSTATUS STATUS_DEVICE_ALREADY_ATTACHED = MAKE_STATUS(0xC0000038);
constexpr CBKSTATUS STATUS_OBJECT_PATH_INVALID     = MAKE_STATUS(0xC0000039);
constexpr CBKSTATUS STATUS_OBJECT_PATH_NOT_FOUND   = MAKE_STATUS(0xC000003A);
constexpr CBKSTATUS STATUS_SHARING_VIOLATION       = MAKE_STATUS(0xC0000043);
constexpr CBKSTATUS STATUS_DIRECTORY_NOT_EMPTY     = MAKE_STATUS(0xC0000101);

// -- Hardware & I/O Errors --
constexpr CBKSTATUS STATUS_IO_TIMEOUT           = MAKE_STATUS(0xC00000B5);
constexpr CBKSTATUS STATUS_DEVICE_NOT_CONNECTED = MAKE_STATUS(0xC000009D);
constexpr CBKSTATUS STATUS_DEVICE_POWER_FAILURE = MAKE_STATUS(0xC000009E);
constexpr CBKSTATUS STATUS_NOT_FOUND            = MAKE_STATUS(0xC0000225);
constexpr CBKSTATUS STATUS_DEVICE_NOT_READY     = MAKE_STATUS(0xC00000A3);

// clang-format on