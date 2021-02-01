// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <memory> // for std::shared_ptr
#include <vector>
#include "Command.h"
#include "sal_cross_platform.h"

class ISerializeCommandVisitor;

class IExpressionCommand
{
public:
    virtual CalculationManager::CommandType GetCommandType() const = 0;
    virtual void Accept(_In_ ISerializeCommandVisitor& commandVisitor) = 0;
};

class IOperatorCommand : public IExpressionCommand
{
public:
    virtual void SetCommand(int command) = 0;
};

class IUnaryCommand : public IOperatorCommand
{
public:
    virtual const std::shared_ptr<std::vector<int>>& GetCommands() const = 0;
    virtual void SetCommands(int command1, int command2) = 0;
};

class IBinaryCommand : public IOperatorCommand
{
public:
    virtual void SetCommand(int command) override = 0;
    virtual int GetCommand() const = 0;
};

class IOpndCommand : public IExpressionCommand
{
public:
    virtual const std::shared_ptr<std::vector<int>>& GetCommands() const = 0;
    virtual void AppendCommand(int command) = 0;
    virtual void ToggleSign() = 0;
    virtual void RemoveFromEnd() = 0;
    virtual bool IsNegative() const = 0;
    virtual bool IsSciFmt() const = 0;
    virtual bool IsDecimalPresent() const = 0;
    virtual const std::wstring& GetToken(wchar_t decimalSymbol) = 0;
    virtual void SetCommands(std::shared_ptr<std::vector<int>> const& commands) = 0;
};

class IParenthesisCommand : public IExpressionCommand
{
public:
    virtual int GetCommand() const = 0;
};
