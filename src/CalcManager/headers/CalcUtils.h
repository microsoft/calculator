// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

using OpCode = uintptr_t;

bool IsOpInRange(OpCode op, uint32_t x, uint32_t y);
bool IsBinOpCode(OpCode opCode);

// WARNING: IDC_SIGN is a special unary op but still this doesn't catch this. Caller has to be aware
// of it and catch it themselves or not needing this
bool IsUnaryOpCode(OpCode opCode);
bool IsDigitOpCode(OpCode opCode);
bool IsGuiSettingOpCode(OpCode opCode);
