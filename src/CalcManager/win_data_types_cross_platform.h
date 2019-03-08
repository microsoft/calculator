#pragma once

#include <cstdint>

typedef uint64_t ULONGLONG;
typedef int32_t INT;
typedef uint8_t CHAR;
typedef int32_t LONG;
typedef uint8_t BYTE;
typedef uintptr_t UINT_PTR;
typedef unsigned long ULONG_PTR;
typedef uint32_t ULONG32;
typedef uint32_t DWORD;
typedef uint32_t ULONG;
typedef uint16_t USHORT;
typedef wchar_t WCHAR;
typedef uint16_t WORD;
typedef UINT_PTR WPARAM;
typedef ULONG_PTR DWORD_PTR;

#define LOWORD(dw) ((WORD)(((DWORD_PTR)(dw)) & 0xffff))
#define HIWORD(dw) ((WORD)((((DWORD_PTR)(dw)) >> 16) & 0xffff))
#define LODWORD(qw) ((DWORD)(qw))
#define HIDWORD(qw) ((DWORD)(((qw) >> 32) & 0xffffffff))

#define ARRAYSIZE(a) (sizeof(a) / sizeof(*a))
