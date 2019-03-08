#pragma once

#include <cstdint>

typedef uint64_t ULONGLONG;
typedef int INT;
typedef char CHAR;
typedef long LONG;
typedef unsigned char BYTE;
typedef unsigned int UINT_PTR;
typedef unsigned long ULONG_PTR;
typedef unsigned int ULONG32;
typedef unsigned long DWORD;
typedef unsigned long ULONG;
typedef unsigned short USHORT;
typedef wchar_t WCHAR;
typedef unsigned short WORD;
typedef UINT_PTR WPARAM;
typedef ULONG_PTR DWORD_PTR;

#define LOWORD(dw) ((WORD)(((DWORD_PTR)(dw)) & 0xffff))
#define HIWORD(dw) ((WORD)((((DWORD_PTR)(dw)) >> 16) & 0xffff))
#define LODWORD(qw) ((DWORD)(qw))
#define HIDWORD(qw) ((DWORD)(((qw) >> 32) & 0xffffffff))

#define ARRAYSIZE(a) (sizeof(a) / sizeof(*a))
