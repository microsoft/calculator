#pragma once

#include <cstdint>

#ifndef LODWORD
#define LODWORD(qw) ((uint32_t)(qw))
#endif

#ifndef HIDWORD
#define HIDWORD(qw) ((uint32_t)(((qw) >> 32) & 0xffffffff))
#endif
