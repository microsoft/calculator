// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "ExpressionCommandInterface.h"
#include "Header Files/CalcEngine.h"
#include "Header Files/Rational.h"

class CParentheses : public IParenthesisCommand
{
public:
    CParentheses(_In_ int command);
    int GetCommand() const;
    CalculationManager::CommandType GetCommandType() const;
    void Accept(_In_ ISerializeCommandVisitor &commandVisitor);

private:
    int m_command;
};

class CUnaryCommand : public IUnaryCommand
{
public:
    CUnaryCommand(int command);
    CUnaryCommand(int command1, int command2);
    const std::shared_ptr<CalculatorVector<int>> & GetCommands() const;
    CalculationManager::CommandType GetCommandType() const;
    void SetCommand(int command);
    void SetCommands(int command1, int command2);
    void Accept(_In_ ISerializeCommandVisitor &commandVisitor);

private:
    std::shared_ptr<CalculatorVector<int>> m_command;
};

class CBinaryCommand : public IBinaryCommand
{
public:
    CBinaryCommand(int command);
    void SetCommand(int command);
    int GetCommand() const;
    CalculationManager::CommandType GetCommandType() const;
    void Accept(_In_ ISerializeCommandVisitor &commandVisitor);

private:
    int m_command;
};

class COpndCommand : public IOpndCommand
{
public:
    COpndCommand(
        std::shared_ptr<CalculatorVector<int>> const &commands,
        bool fNegative,
        bool fDecimal,
        bool fSciFmt);
    void Initialize(CalcEngine::Rational const& rat);

    const std::shared_ptr<CalculatorVector<int>> & GetCommands() const;
    void SetCommands(std::shared_ptr<CalculatorVector<int>> const& commands);
    void AppendCommand(int command);
    void ToggleSign();
    void RemoveFromEnd();
    bool IsNegative() const;
    bool IsSciFmt() const;
    bool IsDecimalPresent() const;
    const std::wstring & GetToken(wchar_t decimalSymbol);
    CalculationManager::CommandType GetCommandType() const;
    void Accept(_In_ ISerializeCommandVisitor &commandVisitor);
    std::wstring GetString(uint32_t radix, int32_t precision, wchar_t decimalSymbol);

private:
    std::shared_ptr<CalculatorVector<int>> m_commands;
    bool m_fNegative;
    bool m_fSciFmt;
    bool m_fDecimal;
    bool m_fInitialized;
    std::wstring m_token;
    CalcEngine::Rational m_value;
    void ClearAllAndAppendCommand(CalculationManager::Command command);
};

class ISerializeCommandVisitor
{
public:
    virtual void Visit(_In_ COpndCommand &opndCmd) = 0;
    virtual void Visit(_In_ CUnaryCommand &unaryCmd) = 0;
    virtual void Visit(_In_ CBinaryCommand &binaryCmd) = 0;
    virtual void Visit(_In_ CParentheses &paraCmd) = 0;
};
