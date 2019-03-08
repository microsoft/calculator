// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

bool IsOpInRange(uintptr_t op, uint32_t x, uint32_t y);
bool IsBinOpCode(uintptr_t opCode);

// WARNING: IDC_SIGN is a special unary op but still this doesn't catch this. Caller has to be aware
// of it and catch it themselves or not needing this
bool IsUnaryOpCode(uintptr_t opCode);
bool IsDigitOpCode(uintptr_t opCode);
bool IsGuiSettingOpCode(uintptr_t opCode);
