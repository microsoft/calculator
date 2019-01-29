// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

bool IsOpInRange(WPARAM op, uint32_t x, uint32_t y);
bool IsBinOpCode(WPARAM opCode);

// WARNING: IDC_SIGN is a special unary op but still this doesnt catch this. Caller has to be aware
// of it and catch it themself or not needing this
bool IsUnaryOpCode(WPARAM opCode);
bool IsDigitOpCode(WPARAM opCode);
bool IsGuiSettingOpCode(WPARAM opCode);
