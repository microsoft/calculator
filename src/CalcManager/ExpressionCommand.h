// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "ExpressionCommandInterface.h"
#include "Header Files/CalcEngine.h"
#include "Header Files/Rational.h"

class CParentheses final : public IParenthesisCommand
{
public:
    CParentheses(_In_ int command);
    int GetCommand() const override;
    CalculationManager::CommandType GetCommandType() const override;
    void Accept(_In_ ISerializeCommandVisitor& commandVisitor) override;

private:
    int m_command;
};

class CUnaryCommand final : public IUnaryCommand
{
public:
    CUnaryCommand(int command);
    CUnaryCommand(int command1, int command2);
    const std::shared_ptr<std::vector<int>>& GetCommands() const override;
    CalculationManager::CommandType GetCommandType() const override;
    void SetCommand(int command) override;
    void SetCommands(int command1, int command2) override;
    void Accept(_In_ ISerializeCommandVisitor& commandVisitor) override;

private:
    std::shared_ptr<std::vector<int>> m_command;
};

class CBinaryCommand final : public IBinaryCommand
{
public:
    CBinaryCommand(int command);
    void SetCommand(int command) override;
    int GetCommand() const override;
    CalculationManager::CommandType GetCommandType() const override;
    void Accept(_In_ ISerializeCommandVisitor& commandVisitor) override;

private:
    int m_command;
};

class COpndCommand final : public IOpndCommand
{
public:
    COpndCommand(std::shared_ptr<std::vector<int>> const& commands, bool fNegative, bool fDecimal, bool fSciFmt);
    void Initialize(CalcEngine::Rational const& rat);

    const std::shared_ptr<std::vector<int>>& GetCommands() const override;
    void SetCommands(std::shared_ptr<std::vector<int>> const& commands) override;
    void AppendCommand(int command) override;
    void ToggleSign() override;
    void RemoveFromEnd() override;
    bool IsNegative() const override;
    bool IsSciFmt() const override;
    bool IsDecimalPresent() const override;
    const std::wstring& GetToken(wchar_t decimalSymbol) override;
    CalculationManager::CommandType GetCommandType() const override;
    void Accept(_In_ ISerializeCommandVisitor& commandVisitor) override;
    std::wstring GetString(uint32_t radix, int32_t precision);

private:
    std::shared_ptr<std::vector<int>> m_commands;
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
    virtual void Visit(_In_ COpndCommand& opndCmd) = 0;
    virtual void Visit(_In_ CUnaryCommand& unaryCmd) = 0;
    virtual void Visit(_In_ CBinaryCommand& binaryCmd) = 0;
    virtual void Visit(_In_ CParentheses& paraCmd) = 0;
};
