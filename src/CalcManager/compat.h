#pragma once

#if defined(__WEBASSEMBLY__)

#define HRESULT long
#define _In_opt_
#define _Out_
#define _In_
#define _Inout_
#define __in_opt

#define S_OK ((HRESULT)0L)
#define S_FALSE ((HRESULT)1L)
#define _HRESULT_TYPEDEF_(_sc) ((HRESULT)_sc)
#define E_OUTOFMEMORY _HRESULT_TYPEDEF_(0x8007000EL)
#define E_BOUNDS _HRESULT_TYPEDEF_(0x8000000BL)
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#define FAILED(hr) (((HRESULT)(hr)) < 0)
//
#define LOWORD(_dw) ((WORD)(((DWORD_PTR)(_dw)) & 0xffff))
#define HIWORD(_dw) ((WORD)((((DWORD_PTR)(_dw)) >> 16) & 0xffff))
#define LODWORD(_qw) ((DWORD)(_qw))
#define HIDWORD(_qw) ((DWORD)(((_qw) >> 32) & 0xffffffff))

typedef unsigned long DWORD;

#define HRESULT_CODE(hr) ((hr)&0xFFFF)
#define SCODE_CODE(sc) ((sc)&0xFFFF)

#endif