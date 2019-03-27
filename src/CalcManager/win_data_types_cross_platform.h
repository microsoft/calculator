#pragma once

#define LODWORD(qw) ((DWORD)(qw))
#define HIDWORD(qw) ((DWORD)(((qw) >> 32) & 0xffffffff))
