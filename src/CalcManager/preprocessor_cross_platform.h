#pragma once

#if !defined(_WIN32) || !defined(_MSC_VER)

// Empty macro definitions for Windows-specific preprocessor directives

#ifndef __pragma
#define __pragma(x)
#endif

#endif
