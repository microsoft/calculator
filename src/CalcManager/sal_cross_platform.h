#pragma once

#if defined(_WIN32) && defined(_MSC_VER)

#include <sal.h>

#else

// Empty macro definitions for source annotations

#define _In_opt_
#define _Out_opt_
#define _In_
#define _Out_
#define _Inout_
#define __in_opt
#define _Frees_ptr_opt_

#endif
