#pragma once

#include <cstdint>

#define LODWORD(qw) ((uint32_t)(qw))
#define HIDWORD(qw) ((uint32_t)(((qw) >> 32) & 0xffffffff))
