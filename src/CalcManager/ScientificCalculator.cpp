// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"
#include "ScientificCalculator.h"
#include "EpsilonEngine/EpsilonEngine.h"
#include "CalculatorHistory.h"
#include "ExpressionCommand.h"
#include "CalculatorResource.h"
#include "Header Files/EngineStrings.h"
#include "Header Files/ICalcDisplay.h"
#include "Ratpack/CalcErr.h"
#include <algorithm>
#include <optional>
#include <random>
#include <variant>
#include <iomanip>
#include <sstream>
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::string_view;
using std::vector;
using std::wstring;
namespace CalculationManager
{
    namespace
    {
        constexpr size_t MaxInputCharacters = 256, MaxTokens = 8192;
        constexpr unsigned MaxParseDepth = 32;
        constexpr int MaxDisplayPrecision = 100;
        using SourceSpan = ExpressionSpan;
        struct ParseError : ExpressionException
        {
            explicit ParseError(const char* m)
                : ExpressionException(ExpressionError::Syntax, m)
            {
            }
        };
        struct ResourceLimitError : ExpressionException
        {
            explicit ResourceLimitError(const char* m)
                : ExpressionException(ExpressionError::ResourceLimit, m)
            {
            }
        };
        struct NumberToken
        {
            shared_ptr<EpsilonValue> value;
            string lexeme;
            wstring display;
            SourceSpan span;
        };
        struct PlusToken
        {
            SourceSpan span;
        };
        struct MinusToken
        {
            SourceSpan span;
        };
        struct MultiplyToken
        {
            SourceSpan span;
        };
        struct DivideToken
        {
            SourceSpan span;
        };
        struct PowerToken
        {
            SourceSpan span;
        };
        struct RootToken
        {
            SourceSpan span;
        };
        struct ModToken
        {
            SourceSpan span;
        };
        struct LogBaseToken
        {
            SourceSpan span;
        };
        struct LeftParenToken
        {
            SourceSpan span;
        };
        struct RightParenToken
        {
            SourceSpan span;
        };
        using Token = std::variant<
            NumberToken,
            PlusToken,
            MinusToken,
            MultiplyToken,
            DivideToken,
            PowerToken,
            RootToken,
            ModToken,
            LogBaseToken,
            LeftParenToken,
            RightParenToken>;
        struct ParseResult
        {
            EpsilonValue value;
            string source;
        };
        EpsilonValue Evaluate(const string& source)
        {
            auto result = EpsilonEngine::Evaluate(source);
            if (auto* error = std::get_if<ExpressionException>(&result))
                throw *error;
            return std::move(std::get<EpsilonValue>(result));
        }
        wstring widen(string_view value)
        {
            return wstring(value.begin(), value.end());
        }
        string narrowNumber(const wstring& input, wchar_t decimal)
        {
            string result;
            for (auto c : input)
            {
                if (c == decimal)
                    result.push_back('.');
                else if ((c >= L'0' && c <= L'9') || c == L'+' || c == L'-' || c == L'e' || c == L'E' || c == L'.')
                    result.push_back(static_cast<char>(c));
                else
                    throw ParseError("Invalid localized number");
            }
            return result;
        }
        bool endsWithOperand(const vector<Token>& t)
        {
            return !t.empty() && (std::holds_alternative<NumberToken>(t.back()) || std::holds_alternative<RightParenToken>(t.back()));
        }
        bool isOperator(const Token& t)
        {
            return !std::holds_alternative<NumberToken>(t) && !std::holds_alternative<LeftParenToken>(t) && !std::holds_alternative<RightParenToken>(t);
        }
        int Precedence(const Token& t)
        {
            if (std::holds_alternative<PlusToken>(t) || std::holds_alternative<MinusToken>(t))
                return 10;
            if (std::holds_alternative<MultiplyToken>(t) || std::holds_alternative<DivideToken>(t) || std::holds_alternative<ModToken>(t))
                return 20;
            return 40;
        }
        int CommandPrecedence(Command command)
        {
            if (command == Command::CommandADD || command == Command::CommandSUB)
                return 10;
            if (command == Command::CommandMUL || command == Command::CommandDIV || command == Command::CommandMOD)
                return 20;
            return 40;
        }
        string OperatorSource(const Token& t)
        {
            if (std::holds_alternative<PlusToken>(t))
                return "+";
            if (std::holds_alternative<MinusToken>(t))
                return "-";
            if (std::holds_alternative<MultiplyToken>(t))
                return "*";
            if (std::holds_alternative<DivideToken>(t))
                return "/";
            if (std::holds_alternative<PowerToken>(t))
                return "^";
            if (std::holds_alternative<RootToken>(t))
                return " root ";
            if (std::holds_alternative<ModToken>(t))
                return " mod ";
            return " logbase ";
        }
        string FunctionName(Command command)
        {
            switch (command)
            {
            case Command::CommandSQR:
                return "sqr";
            case Command::CommandSQRT:
                return "sqrt";
            case Command::CommandREC:
                return "recip";
            case Command::CommandSIN:
                return "sin";
            case Command::CommandCOS:
                return "cos";
            case Command::CommandTAN:
                return "tan";
            case Command::CommandLN:
                return "ln";
            case Command::CommandLOG:
                return "log";
            case Command::CommandPOWE:
                return "exp";
            case Command::CommandCUB:
                return "cube";
            case Command::CommandCUBEROOT:
                return "cbrt";
            case Command::CommandFAC:
                return "fact";
            case Command::CommandPOW10:
                return "pow10";
            case Command::CommandPOW2:
                return "pow2";
            case Command::CommandPERCENT:
                return "percent";
            case Command::CommandASIN:
                return "asin";
            case Command::CommandACOS:
                return "acos";
            case Command::CommandATAN:
                return "atan";
            case Command::CommandSINH:
                return "sinh";
            case Command::CommandCOSH:
                return "cosh";
            case Command::CommandTANH:
                return "tanh";
            case Command::CommandASINH:
                return "asinh";
            case Command::CommandACOSH:
                return "acosh";
            case Command::CommandATANH:
                return "atanh";
            case Command::CommandSEC:
                return "sec";
            case Command::CommandCSC:
                return "csc";
            case Command::CommandCOT:
                return "cot";
            case Command::CommandASEC:
                return "asec";
            case Command::CommandACSC:
                return "acsc";
            case Command::CommandACOT:
                return "acot";
            case Command::CommandSECH:
                return "sech";
            case Command::CommandCSCH:
                return "csch";
            case Command::CommandCOTH:
                return "coth";
            case Command::CommandASECH:
                return "asech";
            case Command::CommandACSCH:
                return "acsch";
            case Command::CommandACOTH:
                return "acoth";
            case Command::CommandAbs:
                return "abs";
            case Command::CommandFloor:
                return "floor";
            case Command::CommandCeil:
                return "ceil";
            case Command::CommandCHOP:
                return "trunc";
            case Command::CommandDMS:
                return "dms";
            case Command::CommandDegrees:
                return "degrees";
            default:
                return {};
            }
        }
        bool IsAngleFunction(const string& name)
        {
            return string("|sin|cos|tan|sec|csc|cot|asin|acos|atan|asec|acsc|acot|").find("|" + name + "|") != string::npos;
        }
    }
    class ScientificCalculator::Impl
    {
    public:
        Impl(IResourceProvider* resourceProvider, ICalcDisplay* displayCallback, shared_ptr<CalculatorHistory> history)
            : m_resourceProvider(resourceProvider)
            , m_display(displayCallback)
            , m_history(std::move(history))
        {
            if (m_resourceProvider == nullptr)
            {
                throw std::invalid_argument("ScientificCalculator requires a resource provider");
            }
        }

        void Reset()
        {
            m_tokens.clear();
            m_input.clear();
            m_result.reset();
            m_resultSource.clear();
            m_repeatSource.clear();

            m_exponentBase.reset();
            m_exponentBaseDisplay.clear();
            m_errorExpressionCandidate.clear();

            m_resultText = L"0";
            m_error = false;
            m_completed = false;
            m_replaceOperandOnInput = false;
            m_scientificFormat = false;
            m_inverse = false;
            m_hyperbolic = false;
            m_angle = Command::CommandDEG;
            m_openParentheses = 0;
            PublishAll();
        }

        bool IsInputEmpty() const
        {
            return !m_error && m_input.empty() && !m_replaceOperandOnInput && !m_completed && !endsWithOperand(m_tokens);
        }

        bool IsRecording() const
        {
            return !m_completed && (!m_input.empty() || !m_tokens.empty());
        }

        void SetPrecision(int32_t precision)
        {
            m_precision = std::clamp(precision, int32_t{ 1 }, int32_t{ MaxDisplayPrecision });
            if (m_result)
            {
                m_resultText = Format(*m_result);
                PublishPrimary();
            }
        }

        wchar_t DecimalSeparator() const
        {
            wstring separator = m_resourceProvider->GetCEngineString(L"sDecimal");
            return separator.empty() ? L'.' : separator.front();
        }

        void Process(Command command)
        {
            if (m_error && command != Command::CommandCLEAR && command != Command::CommandCENTR)
            {
                return;
            }

            try
            {
                if (command >= Command::Command0 && command <= Command::Command9)
                {
                    Digit(static_cast<int>(command) - static_cast<int>(Command::Command0));
                    return;
                }
                switch (command)
                {
                case Command::CommandPNT:
                    Decimal();
                    break;
                case Command::CommandEXP:
                    Exponent();
                    break;
                case Command::CommandSIGN:
                    Sign();
                    break;
                case Command::CommandBACK:
                    Backspace();
                    break;
                case Command::CommandCENTR:
                    ClearEntry();
                    break;
                case Command::CommandCLEAR:
                    ClearCalculation();
                    break;
                case Command::CommandOPENP:
                    OpenParenthesis();
                    break;
                case Command::CommandCLOSEP:
                    CloseParenthesis();
                    break;
                case Command::CommandADD:
                case Command::CommandSUB:
                case Command::CommandMUL:
                case Command::CommandDIV:
                case Command::CommandPWR:
                case Command::CommandROOT:
                case Command::CommandMOD:
                case Command::CommandLogBaseY:
                    Binary(command);
                    break;
                case Command::CommandSQR:
                case Command::CommandSQRT:
                case Command::CommandREC:
                case Command::CommandSIN:
                case Command::CommandCOS:
                case Command::CommandTAN:
                case Command::CommandLN:
                case Command::CommandLOG:
                case Command::CommandPOWE:
                    Unary(command);
                    break;
                case Command::CommandPI:
                case Command::CommandEuler:
                case Command::CommandRand:
                    Constant(command);
                    break;
                case Command::CommandDEG:
                case Command::CommandRAD:
                case Command::CommandGRAD:
                    m_angle = command;
                    PublishExpression();
                    break;
                case Command::CommandFE:
                    ToggleFormat();
                    break;
                case Command::CommandINV:
                    m_inverse = !m_inverse;
                    break;
                case Command::CommandHYP:
                    m_hyperbolic = !m_hyperbolic;
                    break;
                case Command::CommandSTORE:
                case Command::CommandRECALL:
                case Command::CommandMPLUS:
                case Command::CommandMMINUS:
                case Command::CommandMCLEAR:
                    Memory(command);
                    break;
                case Command::CommandEQU:
                    Equals();
                    break;
                default:
                    if (!FunctionName(command).empty())
                    {
                        Unary(command);
                        break;
                    }
                    throw std::invalid_argument("unsupported ScientificCalculator command");
                }
            }
            catch (const ExpressionException& error)
            {
                DisplayError(
                    error.code == ExpressionError::DivideByZero    ? CALC_E_DIVIDEBYZERO
                    : error.code == ExpressionError::Undefined     ? CALC_E_INDEFINITE
                    : error.code == ExpressionError::ResourceLimit ? CALC_E_OVERFLOW
                                                                   : CALC_E_DOMAIN);
            }
            catch (const std::bad_alloc&)
            {
                DisplayError(CALC_E_OVERFLOW);
            }
            catch (const std::length_error&)
            {
                DisplayError(CALC_E_OVERFLOW);
            }
        }

        void DisplayError(int32_t errorCode)
        {
            uint32_t code = static_cast<uint32_t>(errorCode);
            int resourceId;
            switch (code)
            {
            case CALC_E_DIVIDEBYZERO:
                resourceId = IDS_DIVBYZERO;
                break;
            case CALC_E_INDEFINITE:
                resourceId = IDS_UNDEFINED;
                break;
            case CALC_E_OVERFLOW:
                resourceId = IDS_OVERFLOW;
                break;
            case CALC_E_DOMAIN:
            default:
                resourceId = IDS_DOMAIN;
                break;
            }
            wstring message = m_resourceProvider->GetCEngineString(std::to_wstring(resourceId));
            if (message.empty())
            {
                message = resourceId == IDS_DIVBYZERO   ? L"Cannot divide by zero"
                          : resourceId == IDS_UNDEFINED ? L"Result is undefined"
                          : resourceId == IDS_OVERFLOW  ? L"Overflow"
                                                        : L"Invalid input";
            }
            m_resultText = std::move(message);
            m_error = true;
            if (m_display)
            {
                m_display->SetPrimaryDisplay(m_resultText, true);
                m_display->SetIsInError(true);
            }
            if (!m_errorExpressionCandidate.empty())
            {
                PublishExpressionText(m_errorExpressionCandidate);
                m_errorExpressionCandidate.clear();
            }
        }

        const wstring& Result() const
        {
            return m_resultText;
        }
        Command Angle() const
        {
            return m_angle;
        }
        bool ScientificFormat() const
        {
            return m_scientificFormat;
        }

        string CurrentSource() const
        {
            if (!m_input.empty())
            {
                if (m_exponentBase)
                {
                    auto marker = m_input.find_last_of(L"eE");
                    if (marker == wstring::npos)
                        throw ParseError("Missing exponent");
                    return "(" + m_exponentBase->source + ")*1e" + narrowNumber(m_input.substr(marker + 1), DecimalSeparator());
                }
                return narrowNumber(m_input, DecimalSeparator());
            }
            if (!m_completed && !m_replaceOperandOnInput && !endsWithOperand(m_tokens) && m_resultText == L"0")
                return "0";
            return m_resultSource.empty() ? "0" : m_resultSource;
        }

        wstring SaveState() const
        {
            std::wostringstream output;
            output << L"Scientific/1 " << m_precision << L' ' << static_cast<int>(m_angle) << L' ' << m_openParentheses << L' ' << m_error << L' '
                   << m_completed << L' ' << m_replaceOperandOnInput << L' ' << m_scientificFormat << L' ' << m_inverse << L' ' << m_hyperbolic << L' '
                   << std::quoted(widen(narrowNumber(m_input, DecimalSeparator()))) << L' ' << std::quoted(widen(m_resultSource)) << L' '
                   << std::quoted(m_resultText) << L' ' << std::quoted(widen(m_repeatSource)) << L' ' << std::quoted(widen(m_repeatOperator)) << L' '
                   << std::quoted(m_repeatDisplay) << L' ' << std::quoted(m_exponentBase ? widen(m_exponentBase->source) : L"") << L' '
                   << m_exponentOperandStart << L' ' << std::quoted(m_exponentBaseDisplay) << L' ' << m_tokens.size();
            for (const auto& token : m_tokens)
            {
                output << L' ' << token.index() << L' ';
                const auto* number = std::get_if<NumberToken>(&token);
                output << std::quoted(number ? widen(number->lexeme) : L"") << L' ' << std::quoted(number ? number->display : L"");
            }
            auto state = output.str();
            if (state.size() > 1048576)
                throw ResourceLimitError("Scientific state size limit");
            return state;
        }

        void RestoreState(const wstring& state)
        {
            if (state.size() > 1048576)
                throw ResourceLimitError("Scientific state size limit");
            Impl candidate(m_resourceProvider, nullptr, nullptr);
            std::wistringstream input(state);
            wstring version, resultSource, repeatSource, repeatOperator, exponentSource;
            int angle = 0;
            size_t count = 0;
            input >> version >> candidate.m_precision >> angle >> candidate.m_openParentheses >> candidate.m_error >> candidate.m_completed
                >> candidate.m_replaceOperandOnInput >> candidate.m_scientificFormat >> candidate.m_inverse >> candidate.m_hyperbolic
                >> std::quoted(candidate.m_input) >> std::quoted(resultSource) >> std::quoted(candidate.m_resultText) >> std::quoted(repeatSource)
                >> std::quoted(repeatOperator) >> std::quoted(candidate.m_repeatDisplay) >> std::quoted(exponentSource) >> candidate.m_exponentOperandStart
                >> std::quoted(candidate.m_exponentBaseDisplay) >> count;
            if (!input || version != L"Scientific/1" || count > MaxTokens || candidate.m_precision < 1 || candidate.m_precision > 100
                || candidate.m_openParentheses > MaxParseDepth
                || (angle != static_cast<int>(Command::CommandDEG) && angle != static_cast<int>(Command::CommandRAD)
                    && angle != static_cast<int>(Command::CommandGRAD))
                || candidate.m_input.size() > MaxInputCharacters)
                throw ParseError("Invalid Scientific state");
            candidate.m_angle = static_cast<Command>(angle);
            std::replace(candidate.m_input.begin(), candidate.m_input.end(), L'.', candidate.DecimalSeparator());
            auto ascii = [](const wstring& text)
            {
                string result;
                for (auto character : text)
                {
                    if (character < 0 || character > 127)
                        throw ParseError("Non-ASCII canonical expression");
                    result.push_back(static_cast<char>(character));
                }
                return result;
            };
            candidate.m_resultSource = ascii(resultSource);
            if (!resultSource.empty())
                candidate.m_result = Evaluate(candidate.m_resultSource);
            candidate.m_repeatSource = ascii(repeatSource);
            if (!repeatSource.empty())
                Evaluate(candidate.m_repeatSource);
            candidate.m_repeatOperator = ascii(repeatOperator);
            if (!repeatSource.empty() && candidate.m_repeatOperator != "+" && candidate.m_repeatOperator != "-" && candidate.m_repeatOperator != "*"
                && candidate.m_repeatOperator != "/" && candidate.m_repeatOperator != "^" && candidate.m_repeatOperator != " root "
                && candidate.m_repeatOperator != " mod " && candidate.m_repeatOperator != " logbase ")
                throw ParseError("Invalid repeat operator");
            if (!exponentSource.empty())
            {
                auto source = ascii(exponentSource);
                candidate.m_exponentBase = ParseResult{ Evaluate(source), source };
                candidate.m_exponentBaseDisplay = candidate.Format(candidate.m_exponentBase->value);
            }
            for (size_t i = 0; i < count; ++i)
            {
                size_t kind = 0;
                wstring source, label;
                input >> kind >> std::quoted(source) >> std::quoted(label);
                if (!input || kind > 10 || source.size() > 65536 || label.size() > 65536)
                    throw ParseError("Invalid Scientific token");
                SourceSpan span{ i, i + 1 };
                if (kind == 0)
                {
                    auto canonical = ascii(source);
                    candidate.m_tokens.emplace_back(NumberToken{ make_shared<EpsilonValue>(Evaluate(canonical)), canonical, label, span });
                }
                else
                {
                    if (!source.empty() || !label.empty())
                        throw ParseError("Invalid operator payload");
                    switch (kind)
                    {
                    case 1:
                        candidate.m_tokens.emplace_back(PlusToken{ span });
                        break;
                    case 2:
                        candidate.m_tokens.emplace_back(MinusToken{ span });
                        break;
                    case 3:
                        candidate.m_tokens.emplace_back(MultiplyToken{ span });
                        break;
                    case 4:
                        candidate.m_tokens.emplace_back(DivideToken{ span });
                        break;
                    case 5:
                        candidate.m_tokens.emplace_back(PowerToken{ span });
                        break;
                    case 6:
                        candidate.m_tokens.emplace_back(RootToken{ span });
                        break;
                    case 7:
                        candidate.m_tokens.emplace_back(ModToken{ span });
                        break;
                    case 8:
                        candidate.m_tokens.emplace_back(LogBaseToken{ span });
                        break;
                    case 9:
                        candidate.m_tokens.emplace_back(LeftParenToken{ span });
                        break;
                    case 10:
                        candidate.m_tokens.emplace_back(RightParenToken{ span });
                        break;
                    }
                }
            }
            input >> std::ws;
            if (!input.eof() || (candidate.m_exponentBase && candidate.m_exponentOperandStart >= count))
                throw ParseError("Invalid trailing Scientific state");
            unsigned open = 0;
            bool expectingOperand = true;
            for (const auto& token : candidate.m_tokens)
            {
                if (std::holds_alternative<NumberToken>(token))
                {
                    if (!expectingOperand)
                        throw ParseError("Adjacent Scientific operands");
                    expectingOperand = false;
                }
                else if (std::holds_alternative<LeftParenToken>(token))
                {
                    if (!expectingOperand || ++open > MaxParseDepth)
                        throw ParseError("Invalid Scientific group");
                }
                else if (std::holds_alternative<RightParenToken>(token))
                {
                    if (expectingOperand || open == 0)
                        throw ParseError("Invalid Scientific closing group");
                    --open;
                }
                else
                {
                    if (expectingOperand)
                        throw ParseError("Invalid Scientific operator");
                    expectingOperand = true;
                }
            }
            if (open != candidate.m_openParentheses || (candidate.m_completed && (open != 0 || expectingOperand))
                || (!candidate.m_input.empty() && !expectingOperand && !candidate.m_exponentBase))
                throw ParseError("Inconsistent Scientific editor state");
            if (!candidate.m_input.empty())
            {
                auto numeric = narrowNumber(candidate.m_input, candidate.DecimalSeparator());
                if (numeric == "+" || numeric == "-")
                    numeric += "0";
                if (numeric.ends_with("e+") || numeric.ends_with("e-") || numeric.ends_with("e"))
                    numeric += "0";
                if (!candidate.m_error)
                    Evaluate(numeric);
            }
            if (candidate.m_completed)
            {
                auto evaluated = candidate.ParseRange(0, candidate.m_tokens.size());
                candidate.m_resultSource = evaluated.source;
                candidate.m_result = std::move(evaluated.value);
            }
            if (!candidate.m_error)
            {
                if (!candidate.m_input.empty())
                    candidate.PublishInput();
                else
                    candidate.m_resultText = candidate.m_result ? candidate.Format(*candidate.m_result) : L"0";
            }
            candidate.m_memory = std::move(m_memory);
            candidate.m_history = m_history;
            candidate.m_display = m_display;
            candidate.m_random = m_random;
            *this = std::move(candidate);
            PublishAll();
        }

        void EditToken(unsigned index, Command command, bool append)
        {
            if (index >= m_tokens.size())
                throw std::out_of_range("Scientific token index");
            auto saved = m_tokens;
            try
            {
                if (auto* number = std::get_if<NumberToken>(&m_tokens[index]))
                {
                    Impl operand(m_resourceProvider, nullptr, nullptr);
                    operand.m_angle = m_angle;
                    operand.Load(number->lexeme);
                    if (append || command == Command::CommandBACK)
                    {
                        if (number->lexeme.find_first_not_of("0123456789.eE+-") != string::npos)
                            throw ParseError("Only numeric input can be appended while editing");
                        operand.m_input = widen(number->lexeme);
                        std::replace(operand.m_input.begin(), operand.m_input.end(), L'.', DecimalSeparator());
                    }
                    operand.Process(command);
                    if (operand.m_error)
                        throw ParseError("Invalid edited operand");
                    auto source = command == Command::CommandBACK && operand.m_input.empty() ? string("0") : operand.CurrentSource();
                    auto value = Evaluate(source);
                    m_tokens[index] = NumberToken{ make_shared<EpsilonValue>(value), source, operand.Result(), { index, index + 1 } };
                }
                else if (isOperator(m_tokens[index]))
                {
                    auto prefix = std::move(m_tokens);
                    m_tokens.clear();
                    AddOperator(command);
                    auto replacement = m_tokens.back();
                    m_tokens = std::move(prefix);
                    m_tokens[index] = std::move(replacement);
                }
                else
                    throw ParseError("Parentheses cannot be edited as an operand");
                auto result = ParseRange(0, m_tokens.size());
                auto text = Format(result.value);
                m_resultSource = std::move(result.source);
                m_result = std::move(result.value);
                m_resultText = std::move(text);
                m_completed = true;
                m_repeatSource.clear();
                PublishAll();
            }
            catch (const ExpressionException& error)
            {
                m_tokens = std::move(saved);
                DisplayError(error.code == ExpressionError::ResourceLimit ? CALC_E_OVERFLOW : CALC_E_DOMAIN);
            }
        }

        void Load(const string& source)
        {
            auto value = Evaluate(source);
            auto text = Format(value);
            m_tokens.clear();
            m_input.clear();
            m_exponentBase.reset();
            m_exponentBaseDisplay.clear();
            m_repeatSource.clear();
            m_errorExpressionCandidate.clear();
            m_resultSource = source;
            m_result = std::move(value);
            m_resultText = std::move(text);
            m_openParentheses = 0;
            m_error = false;
            m_completed = false;
            m_replaceOperandOnInput = true;
            m_tokens.emplace_back(NumberToken{ make_shared<EpsilonValue>(*m_result), source, widen(source), { 0, 1 } });
            PublishAll();
        }

        void PublishMemory()
        {
            vector<wstring> values;
            for (const auto& source : m_memory)
                values.push_back(Format(Evaluate(source)));
            if (m_display)
                m_display->SetMemorizedNumbers(values);
        }

        void Memory(Command command, unsigned index = 0)
        {
            if (m_error)
                return;
            try
            {
                if (command == Command::CommandSTORE)
                {
                    auto source = CurrentSource();
                    Format(Evaluate(source));
                    m_memory.insert(m_memory.begin(), std::move(source));
                    if (m_memory.size() > 100)
                        m_memory.resize(100);
                }
                else if (command == Command::CommandMCLEAR)
                    m_memory.clear();
                else if (command == Command::CommandRECALL)
                {
                    if (index >= m_memory.size())
                        throw std::out_of_range("Scientific memory index");
                    auto source = m_memory[index];
                    auto value = Evaluate(source);
                    auto text = Format(value);
                    SeedFromResult();
                    if (endsWithOperand(m_tokens))
                        m_tokens.erase(m_tokens.begin() + CurrentOperandStart(), m_tokens.end());
                    m_input.clear();
                    m_exponentBase.reset();
                    m_resultSource = source;
                    m_result = std::move(value);
                    m_resultText = std::move(text);
                    auto offset = m_tokens.size();
                    m_tokens.emplace_back(NumberToken{ make_shared<EpsilonValue>(*m_result), source, m_resultText, { offset, offset + 1 } });
                    m_replaceOperandOnInput = true;
                    PublishAll();
                }
                else if (command == Command::CommandMPLUS || command == Command::CommandMMINUS)
                {
                    if (!m_memory.empty() && index >= m_memory.size())
                        throw std::out_of_range("Scientific memory index");
                    string source = "(" + (m_memory.empty() ? string("0") : m_memory[index]) + ")" + (command == Command::CommandMPLUS ? "+" : "-") + "("
                                    + CurrentSource() + ")";
                    Format(Evaluate(source));
                    if (m_memory.empty())
                        m_memory.push_back(std::move(source));
                    else
                        m_memory[index] = std::move(source);
                    if (m_display)
                        m_display->MemoryItemChanged(index);
                }
                else
                {
                    if (index >= m_memory.size())
                        throw std::out_of_range("Scientific memory index");
                    m_memory.erase(m_memory.begin() + index);
                }
                PublishMemory();
            }
            catch (const ExpressionException& error)
            {
                DisplayError(
                    error.code == ExpressionError::ResourceLimit  ? CALC_E_OVERFLOW
                    : error.code == ExpressionError::DivideByZero ? CALC_E_DIVIDEBYZERO
                                                                  : CALC_E_DOMAIN);
            }
        }

    private:
        string AngleUnit() const
        {
            return m_angle == Command::CommandDEG ? "deg" : m_angle == Command::CommandGRAD ? "grad" : "rad";
        }
        void StartFreshInputIfCompleted()
        {
            if (m_completed)
            {
                m_tokens.clear();
                m_result.reset();
                m_resultSource.clear();
                m_repeatSource.clear();

                m_exponentBase.reset();
                m_exponentBaseDisplay.clear();
                m_errorExpressionCandidate.clear();

                m_input.clear();
                m_completed = false;
                m_replaceOperandOnInput = false;
                m_openParentheses = 0;
            }
        }

        void ReplaceEvaluatedOperandForInput()
        {
            if (!m_replaceOperandOnInput || !m_input.empty())
            {
                return;
            }
            size_t begin = CurrentOperandStart();
            m_tokens.erase(m_tokens.begin() + begin, m_tokens.end());
            m_result.reset();
            m_resultSource.clear();
            m_repeatSource.clear();

            m_replaceOperandOnInput = false;
        }

        void CheckInputLimit() const
        {
            if (m_input.size() >= MaxInputCharacters)
            {
                throw ResourceLimitError("input length limit");
            }
        }

        void CheckTokenLimit() const
        {
            if (m_tokens.size() >= MaxTokens)
            {
                throw ResourceLimitError("token limit");
            }
        }

        void BeginNumericInput()
        {
            StartFreshInputIfCompleted();
            if (m_exponentBase)
            {
                return;
            }
            ReplaceEvaluatedOperandForInput();
            if (m_input.empty() && !m_tokens.empty() && std::holds_alternative<RightParenToken>(m_tokens.back()))
            {
                AddOperator(Command::CommandMUL);
                if (m_display)
                {
                    m_display->BinaryOperatorReceived();
                }
            }
        }

        void Digit(int digit)
        {
            BeginNumericInput();
            CheckInputLimit();
            if (m_input == L"0")
            {
                m_input.clear();
            }
            m_input.push_back(static_cast<wchar_t>(L'0' + digit));
            PublishInput();
        }

        void Decimal()
        {
            BeginNumericInput();
            CheckInputLimit();
            if (m_input.find_first_of(L"eE") != wstring::npos)
            {
                return;
            }
            wchar_t separator = DecimalSeparator();
            if (m_input.find(separator) != wstring::npos || m_input.find(L'.') != wstring::npos)
            {
                return;
            }
            if (m_input.empty() || m_input == L"-" || m_input == L"+")
            {
                m_input += L'0';
            }
            m_input.push_back(separator);
            PublishInput();
        }

        std::optional<wstring> ExponentInput(const wstring& base) const
        {
            if (base.find_first_of(L"eE") != wstring::npos)
            {
                return std::nullopt;
            }
            wstring input = base;
            wchar_t separator = DecimalSeparator();
            const wstring groupingSeparator = m_resourceProvider->GetCEngineString(L"sThousand");
            if (!groupingSeparator.empty() && groupingSeparator != wstring(1, separator))
            {
                size_t position = input.find(groupingSeparator);
                while (position != wstring::npos)
                {
                    input.erase(position, groupingSeparator.size());
                    position = input.find(groupingSeparator, position);
                }
            }
            if (input.find(separator) == wstring::npos && input.find(L'.') == wstring::npos)
            {
                input.push_back(separator);
            }
            input += L"e+";
            return input;
        }

        void BeginRetainedExponent(wstring input)
        {
            size_t begin = CurrentOperandStart();
            m_exponentBase = ParseRange(begin, m_tokens.size());
            m_exponentOperandStart = begin;
            m_exponentBaseDisplay = m_resultText;
            m_input = std::move(input);
            PublishInput();
        }

        void Exponent()
        {
            if (!m_input.empty())
            {
                if (m_input == L"-" || m_input == L"+" || m_input.find_first_of(L"eE") != wstring::npos)
                {
                    return;
                }
                auto input = ExponentInput(m_input);
                if (!input)
                {
                    return;
                }
                if (input->size() > MaxInputCharacters)
                {
                    throw ResourceLimitError("input length limit");
                }
                m_input = std::move(*input);
                PublishInput();
                return;
            }

            if (m_completed && m_result)
            {
                auto input = ExponentInput(m_resultText);
                if (!input)
                {
                    return;
                }
                if (input->size() > MaxInputCharacters)
                {
                    throw ResourceLimitError("input length limit");
                }
                SeedFromResult();
                m_replaceOperandOnInput = true;
                BeginRetainedExponent(std::move(*input));
                return;
            }

            if (!m_replaceOperandOnInput || m_tokens.empty() || !endsWithOperand(m_tokens))
            {
                return;
            }
            auto input = ExponentInput(m_resultText);
            if (!input)
            {
                return;
            }
            if (input->size() > MaxInputCharacters)
            {
                throw ResourceLimitError("input length limit");
            }
            BeginRetainedExponent(std::move(*input));
        }

        void Sign()
        {
            if (!m_input.empty())
            {
                size_t exponent = m_input.find_first_of(L"eE");
                if (exponent != wstring::npos)
                {
                    size_t signPosition = exponent + 1;
                    if (signPosition < m_input.size() && m_input[signPosition] == L'-')
                    {
                        m_input[signPosition] = L'+';
                    }
                    else if (signPosition < m_input.size() && m_input[signPosition] == L'+')
                    {
                        m_input[signPosition] = L'-';
                    }
                    else
                    {
                        m_input.insert(signPosition, 1, L'+');
                    }
                }
                else if (m_input.front() == L'-')
                {
                    m_input.erase(0, 1);
                }
                else
                {
                    m_input.insert(0, 1, L'-');
                }
                PublishInput();
                return;
            }
            UnarySign();
        }

        void Backspace()
        {
            if (m_completed)
            {
                return;
            }
            if (!m_input.empty())
            {
                size_t exponent = m_input.find_first_of(L"eE");
                if (exponent != wstring::npos && m_input.size() == exponent + 2)
                {
                    m_input.erase(exponent);
                    if (m_exponentBase)
                    {
                        m_input.clear();
                        m_resultText = m_exponentBaseDisplay;
                        m_exponentBase.reset();
                        m_exponentBaseDisplay.clear();
                        PublishAll();
                        return;
                    }
                }
                else
                {
                    m_input.pop_back();
                }
                PublishInput();
            }
        }

        void CancelIncompleteRetainedExponent()
        {
            if (!m_exponentBase)
            {
                return;
            }
            size_t exponent = m_input.find_last_of(L"eE");
            if (exponent == wstring::npos || m_input.size() != exponent + 2 || (m_input.back() != L'+' && m_input.back() != L'-'))
            {
                return;
            }
            m_input.clear();
            m_resultText = m_exponentBaseDisplay;
            m_exponentBase.reset();
            m_exponentBaseDisplay.clear();
        }

        void ClearEntry()
        {
            if (m_error || m_completed)
            {
                ClearCalculation();
                return;
            }
            bool removeCommittedOperand = m_exponentBase.has_value() || (m_input.empty() && endsWithOperand(m_tokens));
            if (removeCommittedOperand)
            {
                size_t begin = CurrentOperandStart();
                m_tokens.erase(m_tokens.begin() + begin, m_tokens.end());
            }
            m_input.clear();
            m_result.reset();
            m_resultSource.clear();
            m_repeatSource.clear();

            m_exponentBase.reset();
            m_exponentBaseDisplay.clear();
            m_errorExpressionCandidate.clear();
            m_error = false;
            m_replaceOperandOnInput = false;
            m_resultText = L"0";
            if (m_display)
            {
                m_display->SetIsInError(false);
            }
            PublishAll();
        }

        void ClearCalculation()
        {
            bool retainMode = m_scientificFormat;
            Command retainAngle = m_angle;
            m_tokens.clear();
            m_input.clear();
            m_result.reset();
            m_resultSource.clear();
            m_repeatSource.clear();

            m_exponentBase.reset();
            m_exponentBaseDisplay.clear();
            m_errorExpressionCandidate.clear();

            m_resultText = L"0";
            m_error = false;
            m_completed = false;
            m_replaceOperandOnInput = false;
            m_openParentheses = 0;
            m_scientificFormat = retainMode;
            m_angle = retainAngle;
            PublishAll();
        }

        void CommitInput()
        {
            if (m_input.empty())
                return;
            CheckTokenLimit();
            string source;
            if (m_exponentBase)
            {
                auto marker = m_input.find_last_of(L"eE");
                if (marker == wstring::npos)
                    throw ParseError("Missing exponent");
                source = "(" + m_exponentBase->source + ")*1e" + narrowNumber(m_input.substr(marker + 1), DecimalSeparator());
            }
            else
                source = narrowNumber(m_input, DecimalSeparator());
            auto value = Evaluate(source);
            if (m_exponentBase)
                m_tokens.erase(m_tokens.begin() + m_exponentOperandStart, m_tokens.end());
            size_t offset = m_tokens.size();
            m_tokens.emplace_back(NumberToken{ make_shared<EpsilonValue>(std::move(value)), source, m_input, { offset, offset + 1 } });
            m_input.clear();
            m_exponentBase.reset();
            m_exponentBaseDisplay.clear();
            m_replaceOperandOnInput = false;
        }

        void SeedFromResult()
        {
            if (!m_completed || !m_result)
                return;
            m_tokens.clear();
            m_tokens.emplace_back(NumberToken{ make_shared<EpsilonValue>(*m_result), m_resultSource, m_resultText, { 0, 1 } });
            m_completed = false;
            m_replaceOperandOnInput = false;
            m_openParentheses = 0;
        }

        wstring BinaryDisplay(Command command) const
        {
            switch (command)
            {
            case Command::CommandADD:
                return L"+";
            case Command::CommandSUB:
                return L"-";
            case Command::CommandMUL:
                return L"\u00d7";
            case Command::CommandDIV:
                return L"\u00f7";
            case Command::CommandPWR:
                return L"^";
            case Command::CommandROOT:
                return L"root";
            case Command::CommandMOD:
                return L"mod";
            case Command::CommandLogBaseY:
                return L"logbase";
            default:
                throw ParseError("invalid binary operator");
            }
        }

        void UpdatePrefixResult(Command nextOperator)
        {
            // Reduce only the current group and operators that bind at least as tightly
            // as the incoming operator. Pending additions must not run before a multiply.
            size_t begin = 0;
            unsigned nesting = 0;
            for (size_t position = m_tokens.size(); position-- > 0;)
            {
                if (std::holds_alternative<RightParenToken>(m_tokens[position]))
                {
                    ++nesting;
                }
                else if (std::holds_alternative<LeftParenToken>(m_tokens[position]))
                {
                    if (nesting == 0)
                    {
                        begin = position + 1;
                        break;
                    }
                    --nesting;
                }
            }

            if (CommandPrecedence(nextOperator) > 10)
            {
                nesting = 0;
                for (size_t position = m_tokens.size(); position-- > begin;)
                {
                    if (std::holds_alternative<RightParenToken>(m_tokens[position]))
                    {
                        ++nesting;
                    }
                    else if (std::holds_alternative<LeftParenToken>(m_tokens[position]))
                    {
                        --nesting;
                    }
                    else if (
                        nesting == 0 && position > begin && isOperator(m_tokens[position]) && Precedence(m_tokens[position]) < CommandPrecedence(nextOperator)
                        && !isOperator(m_tokens[position - 1]))
                    {
                        begin = position + 1;
                        break;
                    }
                }
            }

            ParseResult result = ParseRange(begin, m_tokens.size());
            m_result = std::move(result.value);
            m_resultSource = result.source;

            m_resultText = Format(*m_result);
            if (CommandPrecedence(nextOperator) == 40 && m_tokens.size() > begin + 1)
            {
                wstring label = L"(" + ExpressionText(begin, m_tokens.size(), false) + L")";
                m_tokens.erase(m_tokens.begin() + begin, m_tokens.end());
                m_tokens.emplace_back(NumberToken{ make_shared<EpsilonValue>(*m_result), m_resultSource, std::move(label), { begin, begin + 1 } });
            }
        }

        void Binary(Command command)
        {
            CancelIncompleteRetainedExponent();
            SeedFromResult();
            if (m_tokens.empty() && !m_input.empty())
            {
                m_errorExpressionCandidate = m_input + L" " + BinaryDisplay(command) + L" ";
            }
            CommitInput();
            m_errorExpressionCandidate.clear();
            if (m_tokens.empty())
            {
                if (command == Command::CommandADD || command == Command::CommandSUB)
                {
                    AddZeroOperand();
                    AddOperator(command);
                    PublishAll();
                }
                return;
            }
            if (isOperator(m_tokens.back()))
            {
                const int previousPrecedence = Precedence(m_tokens.back());
                bool groupPrevious = previousPrecedence < CommandPrecedence(command) && m_result.has_value();
                m_tokens.pop_back();
                if (groupPrevious)
                {
                    size_t begin = 0;
                    unsigned depth = 0;
                    for (size_t i = m_tokens.size(); i-- > 0;)
                    {
                        if (std::holds_alternative<RightParenToken>(m_tokens[i]))
                            ++depth;
                        else if (std::holds_alternative<LeftParenToken>(m_tokens[i]))
                        {
                            if (depth == 0)
                            {
                                begin = i + 1;
                                break;
                            }
                            --depth;
                        }
                        else if (depth == 0 && isOperator(m_tokens[i]) && Precedence(m_tokens[i]) < previousPrecedence)
                        {
                            begin = i + 1;
                            break;
                        }
                    }
                    if (m_tokens.size() > begin + 1)
                    {
                        auto value = ParseRange(begin, m_tokens.size());
                        wstring label = L"(" + ExpressionText(begin, m_tokens.size(), false) + L")";
                        m_tokens.erase(m_tokens.begin() + begin, m_tokens.end());
                        m_tokens.emplace_back(NumberToken{ make_shared<EpsilonValue>(value.value), value.source, std::move(label), { begin, begin + 1 } });
                    }
                }
            }
            else if (std::holds_alternative<LeftParenToken>(m_tokens.back()) && (command == Command::CommandADD || command == Command::CommandSUB))
            {
                AddZeroOperand();
            }
            else if (!endsWithOperand(m_tokens))
            {
                throw ParseError("operator requires operand");
            }
            else
            {
                UpdatePrefixResult(command);
            }
            AddOperator(command);
            m_replaceOperandOnInput = false;
            if (m_display)
            {
                m_display->BinaryOperatorReceived();
            }
            PublishAll();
        }

        void AddZeroOperand()
        {
            CheckTokenLimit();
            auto offset = m_tokens.size();
            m_tokens.emplace_back(NumberToken{ make_shared<EpsilonValue>(Evaluate("0")), "0", L"0", { offset, offset + 1 } });
        }

        void AddOperator(Command command)
        {
            CheckTokenLimit();
            SourceSpan span{ m_tokens.size(), m_tokens.size() + 1 };
            switch (command)
            {
            case Command::CommandADD:
                m_tokens.emplace_back(PlusToken{ span });
                break;
            case Command::CommandSUB:
                m_tokens.emplace_back(MinusToken{ span });
                break;
            case Command::CommandMUL:
                m_tokens.emplace_back(MultiplyToken{ span });
                break;
            case Command::CommandDIV:
                m_tokens.emplace_back(DivideToken{ span });
                break;
            case Command::CommandPWR:
                m_tokens.emplace_back(PowerToken{ span });
                break;
            case Command::CommandROOT:
                m_tokens.emplace_back(RootToken{ span });
                break;
            case Command::CommandMOD:
                m_tokens.emplace_back(ModToken{ span });
                break;
            case Command::CommandLogBaseY:
                m_tokens.emplace_back(LogBaseToken{ span });
                break;
            default:
                throw ParseError("invalid binary operator");
            }
        }

        void OpenParenthesis()
        {
            StartFreshInputIfCompleted();
            CommitInput();
            if (endsWithOperand(m_tokens))
            {
                AddOperator(Command::CommandMUL);
                if (m_display)
                {
                    m_display->BinaryOperatorReceived();
                }
            }
            if (m_openParentheses >= MaxParseDepth)
            {
                throw ResourceLimitError("parenthesis depth limit");
            }
            CheckTokenLimit();
            SourceSpan span{ m_tokens.size(), m_tokens.size() + 1 };
            m_tokens.emplace_back(LeftParenToken{ span });
            ++m_openParentheses;
            PublishAll();
        }

        void CompleteTrailingOperator()
        {
            if (m_tokens.empty() || !isOperator(m_tokens.back()))
                return;
            if (!m_result)
                throw ParseError("Missing equals operand");
            CheckTokenLimit();
            auto offset = m_tokens.size();
            m_tokens.emplace_back(NumberToken{ make_shared<EpsilonValue>(*m_result), m_resultSource, m_resultText, { offset, offset + 1 } });
        }

        void CloseRemainingParentheses()
        {
            while (m_openParentheses > 0)
            {
                CheckTokenLimit();
                SourceSpan span{ m_tokens.size(), m_tokens.size() + 1 };
                m_tokens.emplace_back(RightParenToken{ span });
                --m_openParentheses;
            }
        }

        void CloseParenthesis()
        {
            CommitInput();
            if (m_openParentheses == 0)
            {
                if (m_display)
                {
                    m_display->OnNoRightParenAdded();
                }
                return;
            }
            if (!endsWithOperand(m_tokens))
            {
                throw ParseError("closing parenthesis requires an operand");
            }
            CheckTokenLimit();
            SourceSpan span{ m_tokens.size(), m_tokens.size() + 1 };
            m_tokens.emplace_back(RightParenToken{ span });
            --m_openParentheses;
            ParseResult result = ParseRange(CurrentOperandStart(), m_tokens.size());
            m_result = std::move(result.value);
            m_resultSource = result.source;

            m_resultText = Format(*m_result);
            PublishAll();
        }

        size_t CurrentOperandStart() const
        {
            if (m_tokens.empty())
            {
                throw ParseError("missing operand");
            }
            if (std::holds_alternative<NumberToken>(m_tokens.back()))
            {
                return m_tokens.size() - 1;
            }
            if (!std::holds_alternative<RightParenToken>(m_tokens.back()))
            {
                throw ParseError("missing operand");
            }
            int nesting = 0;
            for (size_t position = m_tokens.size(); position-- > 0;)
            {
                if (std::holds_alternative<RightParenToken>(m_tokens[position]))
                {
                    ++nesting;
                }
                else if (std::holds_alternative<LeftParenToken>(m_tokens[position]) && --nesting == 0)
                {
                    return position;
                }
            }
            throw ParseError("mismatched parenthesis");
        }

        ParseResult ParseRange(size_t begin, size_t end) const
        {
            string source;
            for (size_t i = begin; i < end; ++i)
            {
                string part;
                if (const auto* number = std::get_if<NumberToken>(&m_tokens[i]))
                    part = "(" + number->lexeme + ")";
                else if (std::holds_alternative<PlusToken>(m_tokens[i]))
                    part = "+";
                else if (std::holds_alternative<MinusToken>(m_tokens[i]))
                    part = "-";
                else if (std::holds_alternative<MultiplyToken>(m_tokens[i]))
                    part = "*";
                else if (std::holds_alternative<DivideToken>(m_tokens[i]))
                    part = "/";
                else if (std::holds_alternative<PowerToken>(m_tokens[i]))
                    part = "^";
                else if (std::holds_alternative<RootToken>(m_tokens[i]))
                    part = " root ";
                else if (std::holds_alternative<ModToken>(m_tokens[i]))
                    part = " mod ";
                else if (std::holds_alternative<LogBaseToken>(m_tokens[i]))
                    part = " logbase ";
                else if (std::holds_alternative<LeftParenToken>(m_tokens[i]))
                    part = "(";
                else
                    part = ")";
                if (part.size() > 65536 - source.size())
                    throw ResourceLimitError("Expression source limit");
                source += part;
            }
            auto value = Evaluate(source);
            return { std::move(value), std::move(source) };
        }

        void ReplaceCurrentOperand(EpsilonValue value, wstring label, string source)
        {
            auto text = Format(value);
            size_t begin = CurrentOperandStart();
            m_tokens.erase(m_tokens.begin() + begin, m_tokens.end());
            m_tokens.emplace_back(NumberToken{ make_shared<EpsilonValue>(value), source, std::move(label), { begin, begin + 1 } });
            m_result = std::move(value);
            m_resultSource = std::move(source);
            m_resultText = std::move(text);
            m_replaceOperandOnInput = true;
            PublishAll();
        }

        void Unary(Command command)
        {
            SeedFromResult();
            CommitInput();
            if (m_tokens.empty())
                AddZeroOperand();
            size_t begin = CurrentOperandStart();
            auto old = ParseRange(begin, m_tokens.size());
            auto function = FunctionName(command);
            if (m_hyperbolic
                && (function == "sin" || function == "cos" || function == "tan" || function == "sec" || function == "csc" || function == "cot"
                    || function == "asin" || function == "acos" || function == "atan" || function == "asec" || function == "acsc" || function == "acot"))
                function += "h";
            if (m_inverse)
            {
                if (function == "ln")
                    function = "exp";
                else if (
                    function == "sin" || function == "cos" || function == "tan" || function == "sec" || function == "csc" || function == "cot"
                    || function == "sinh" || function == "cosh" || function == "tanh" || function == "sech" || function == "csch" || function == "coth")
                    function = "a" + function;
            }
            m_inverse = false;
            string source;
            if (command == Command::CommandPOW10)
                source = "pow(10," + old.source + ")";
            else if (command == Command::CommandPOW2)
                source = "pow(2," + old.source + ")";
            else if (command == Command::CommandPERCENT)
            {
                source = "(" + old.source + ")/100";
                if (begin > 0 && (std::holds_alternative<PlusToken>(m_tokens[begin - 1]) || std::holds_alternative<MinusToken>(m_tokens[begin - 1])))
                    source = "(" + ParseRange(0, begin - 1).source + ")*(" + source + ")";
            }
            else
            {
                source = function + "(" + old.source;
                if (IsAngleFunction(function))
                    source += "," + AngleUnit();
                source += ")";
            }
            const wstring operand =
                std::holds_alternative<LeftParenToken>(m_tokens[begin]) ? ExpressionText(begin + 1, m_tokens.size() - 1, false) : OperandDisplay(begin);
            wstring label = (function == FunctionName(command) ? UnaryName(command) : widen(function)) + L"(" + operand + L")";
            PublishExpressionText(ExpressionText(0, begin, false) + label);
            auto value = Evaluate(source);
            ReplaceCurrentOperand(std::move(value), std::move(label), std::move(source));
        }

        void UnarySign()
        {
            SeedFromResult();
            if (m_tokens.empty() || isOperator(m_tokens.back()) || std::holds_alternative<LeftParenToken>(m_tokens.back()))
            {
                m_input = L"-";
                PublishInput();
                return;
            }
            auto begin = CurrentOperandStart();
            auto old = ParseRange(begin, m_tokens.size());
            string source = "-(" + old.source + ")";
            auto value = Evaluate(source);
            ReplaceCurrentOperand(std::move(value), L"-(" + OperandDisplay(begin) + L")", std::move(source));
        }

        wstring UnaryName(Command command) const
        {
            switch (command)
            {
            case Command::CommandSQR:
                return L"sqr";
            case Command::CommandSQRT:
                return L"\x221A";
            case Command::CommandREC:
                return L"1/";
            case Command::CommandSIN:
                return m_angle == Command::CommandDEG ? L"sin\x2080" : m_angle == Command::CommandRAD ? L"sin\x1D63" : L"sin\x1D4D";
            case Command::CommandCOS:
                return m_angle == Command::CommandDEG ? L"cos\x2080" : m_angle == Command::CommandRAD ? L"cos\x1D63" : L"cos\x1D4D";
            case Command::CommandTAN:
                return m_angle == Command::CommandDEG ? L"tan\x2080" : m_angle == Command::CommandRAD ? L"tan\x1D63" : L"tan\x1D4D";
            case Command::CommandLN:
                return L"ln";
            case Command::CommandLOG:
                return L"log";
            case Command::CommandPOWE:
                return L"e^";
            default:
                return widen(FunctionName(command));
            }
        }

        wstring OperandDisplay(size_t begin) const
        {
            return ExpressionText(begin, m_tokens.size(), false);
        }

        void Constant(Command command)
        {
            StartFreshInputIfCompleted();
            if (!m_input.empty() || endsWithOperand(m_tokens))
                return;
            CheckTokenLimit();
            string source = command == Command::CommandPI ? "pi" : "e";
            if (command == Command::CommandRand)
            {
                auto sample = m_random();
                source = Evaluate(std::to_string(sample) + "/4294967296").Format(100);
            }
            if (command == Command::CommandPI && m_inverse)
                source = "2*pi";
            m_inverse = false;
            auto value = Evaluate(source);
            wstring text = Format(value);
            wstring label = command == Command::CommandRand ? text : widen(source);
            auto offset = m_tokens.size();
            m_tokens.emplace_back(NumberToken{ make_shared<EpsilonValue>(value), source, label, { offset, offset + 1 } });
            m_result = std::move(value);
            m_resultSource = std::move(source);
            m_resultText = std::move(text);
            m_replaceOperandOnInput = true;
            PublishAll();
        }

        void ToggleFormat()
        {
            m_scientificFormat = !m_scientificFormat;
            if (!m_result && m_input.empty())
            {
                m_result = Evaluate("0");
                m_resultSource = "0";
            }
            if (!m_input.empty())
            {
                CommitInput();
                auto result = ParseRange(CurrentOperandStart(), m_tokens.size());
                m_result = std::move(result.value);
                m_resultSource = result.source;
                m_resultSource = std::move(result.source);
                m_replaceOperandOnInput = true;
            }
            if (m_result)
            {
                m_resultText = Format(*m_result);
                PublishPrimary();
                PublishExpression();
            }
        }

        void Equals()
        {
            if (m_completed)
            {
                if (m_repeatSource.empty())
                    return;
                string source = "(" + m_resultSource + ")" + m_repeatOperator + "(" + m_repeatSource + ")";
                auto value = Evaluate(source);
                auto text = Format(value);
                m_tokens.clear();
                m_tokens.emplace_back(
                    NumberToken{ make_shared<EpsilonValue>(value), source, m_resultText + widen(m_repeatOperator) + m_repeatDisplay, { 0, 1 } });
                m_result = std::move(value);
                m_resultSource = std::move(source);
                m_resultText = std::move(text);
                PublishAll();
                AppendHistory(ExpressionText(0, m_tokens.size(), false));
                return;
            }
            CommitInput();
            if (m_tokens.empty())
                return;
            CompleteTrailingOperator();
            if (!endsWithOperand(m_tokens))
                throw ParseError("Incomplete expression");
            CloseRemainingParentheses();
            auto result = ParseRange(0, m_tokens.size());
            auto text = Format(result.value);
            m_repeatSource.clear();
            size_t repeatBegin = 0, repeatEnd = m_tokens.size();
            while (repeatEnd > repeatBegin + 1 && std::holds_alternative<LeftParenToken>(m_tokens[repeatBegin]))
            {
                unsigned nesting = 0;
                size_t close = repeatBegin;
                for (; close < repeatEnd; ++close)
                {
                    if (std::holds_alternative<LeftParenToken>(m_tokens[close]))
                        ++nesting;
                    else if (std::holds_alternative<RightParenToken>(m_tokens[close]) && --nesting == 0)
                        break;
                }
                if (close != repeatEnd - 1)
                    break;
                ++repeatBegin;
                --repeatEnd;
            }
            unsigned nesting = 0;
            int lowest = 100;
            for (size_t i = repeatBegin; i < repeatEnd; ++i)
            {
                if (std::holds_alternative<LeftParenToken>(m_tokens[i]))
                    ++nesting;
                else if (std::holds_alternative<RightParenToken>(m_tokens[i]))
                    --nesting;
                else if (nesting == 0 && isOperator(m_tokens[i]) && Precedence(m_tokens[i]) <= lowest)
                {
                    lowest = Precedence(m_tokens[i]);
                    m_repeatSource = ParseRange(i + 1, repeatEnd).source;
                    m_repeatDisplay = ExpressionText(i + 1, repeatEnd, false);
                    m_repeatOperator = OperatorSource(m_tokens[i]);
                }
            }
            auto expression = ExpressionText(0, m_tokens.size(), false);
            m_result = std::move(result.value);
            m_resultSource = result.source;
            m_resultSource = std::move(result.source);
            m_resultText = std::move(text);
            m_completed = true;
            PublishAll();
            AppendHistory(expression);
        }

        vector<unsigned> DecimalGrouping() const
        {
            wstring specification = m_resourceProvider->GetCEngineString(L"sGrouping");
            if (specification.empty())
            {
                specification = L"3;0";
            }
            vector<unsigned> groups;
            unsigned value = 0;
            bool hasDigit = false;
            for (wchar_t character : specification)
            {
                if (character >= L'0' && character <= L'9')
                {
                    hasDigit = true;
                    value = value * 10 + static_cast<unsigned>(character - L'0');
                    if (value > 16)
                    {
                        return {};
                    }
                }
                else if (hasDigit)
                {
                    groups.push_back(value);
                    value = 0;
                    hasDigit = false;
                }
            }
            if (hasDigit)
            {
                groups.push_back(value);
            }
            return groups;
        }

        wstring LocalizeAndGroup(const string& canonical) const
        {
            if (canonical.find('e') != string::npos)
            {
                wstring scientific = widen(canonical);
                wchar_t decimalSeparator = DecimalSeparator();
                if (decimalSeparator != L'.')
                {
                    std::replace(scientific.begin(), scientific.end(), L'.', decimalSeparator);
                }
                return scientific;
            }

            size_t signLength = !canonical.empty() && canonical.front() == '-' ? 1 : 0;
            size_t decimal = canonical.find('.');
            size_t integerEnd = decimal == string::npos ? canonical.size() : decimal;
            wstring integerPart = widen(string_view(canonical).substr(signLength, integerEnd - signLength));
            vector<unsigned> groups = DecimalGrouping();
            wstring delimiter = m_resourceProvider->GetCEngineString(L"sThousand");
            if (delimiter.empty())
            {
                delimiter = L",";
            }
            if (!groups.empty() && groups.front() != 0)
            {
                size_t remaining = integerPart.size();
                size_t groupIndex = 0;
                unsigned groupSize = groups.front();
                while (groupSize != 0 && remaining > groupSize)
                {
                    remaining -= groupSize;
                    integerPart.insert(remaining, delimiter);
                    if (groupIndex + 1 >= groups.size())
                    {
                        if (groups[groupIndex] == 0)
                        {
                            continue;
                        }
                        break;
                    }
                    unsigned next = groups[++groupIndex];
                    if (next != 0)
                    {
                        groupSize = next;
                    }
                }
            }

            wstring result = signLength ? L"-" : L"";
            result += integerPart;
            if (decimal != string::npos)
            {
                result.push_back(DecimalSeparator());
                result += widen(string_view(canonical).substr(decimal + 1));
            }
            return result;
        }

        wstring Format(const EpsilonValue& value) const
        {
            return LocalizeAndGroup(value.Format(m_precision, m_scientificFormat));
        }

        wstring TokenDisplay(const Token& token) const
        {
            if (const auto* number = std::get_if<NumberToken>(&token))
            {
                return number->display;
            }
            if (std::holds_alternative<PlusToken>(token))
                return L"+";
            if (std::holds_alternative<MinusToken>(token))
                return L"-";
            if (std::holds_alternative<MultiplyToken>(token))
                return L"\u00d7";
            if (std::holds_alternative<DivideToken>(token))
                return L"\u00f7";
            if (std::holds_alternative<LeftParenToken>(token))
                return L"(";
            if (isOperator(token))
                return widen(OperatorSource(token));
            return L")";
        }

        wstring ExpressionText(size_t begin, size_t end, bool includeEquals) const
        {
            wstring result;
            for (size_t position = begin; position < end; ++position)
            {
                const Token& token = m_tokens[position];
                if (isOperator(token))
                {
                    result += L" ";
                    result += TokenDisplay(token);
                    result += L" ";
                }
                else
                {
                    result += TokenDisplay(token);
                }
            }
            if (includeEquals)
                result += L"=";
            return result;
        }

        wstring ExpressionText() const
        {
            return ExpressionText(0, m_tokens.size(), m_completed);
        }

        void AppendHistory(const wstring& expression)
        {
            if (!m_history)
            {
                return;
            }
            auto tokens = make_shared<vector<std::pair<wstring, int>>>();
            tokens->emplace_back(expression + L"=", -1);
            auto commands = make_shared<vector<shared_ptr<IExpressionCommand>>>();
            auto state = SaveState();
            unsigned index = m_history->AddToHistory(tokens, commands, m_resultText);
            m_history->GetHistoryItem(index)->historyItemVector.scientificState = std::move(state);
            if (m_display)
            {
                m_display->OnHistoryItemAdded(index);
            }
        }

        void PublishInput()
        {
            m_resultText = m_input.empty() ? L"0" : m_exponentBase ? m_input : LocalizeAndGroup(narrowNumber(m_input, DecimalSeparator()));
            PublishPrimary();
            PublishExpression();
            if (m_display)
            {
                m_display->InputChanged();
            }
        }

        void PublishPrimary()
        {
            if (m_display)
            {
                m_display->SetPrimaryDisplay(m_resultText, m_error);
            }
        }

        void PublishExpression()
        {
            if (!m_display)
            {
                return;
            }
            if (!m_completed)
            {
                PublishExpressionText(ExpressionText());
                return;
            }
            auto tokens = make_shared<vector<std::pair<wstring, int>>>();
            auto commands = make_shared<vector<shared_ptr<IExpressionCommand>>>();
            for (size_t i = 0; i < m_tokens.size(); ++i)
            {
                const auto& token = m_tokens[i];
                if (std::holds_alternative<NumberToken>(token))
                {
                    // This describes an editable source fragment, not legacy replay digits.
                    commands->push_back(make_shared<COpndCommand>(make_shared<vector<int>>(), false, false, false));
                    tokens->emplace_back(TokenDisplay(token), static_cast<int>(i));
                }
                else if (isOperator(token))
                {
                    Command command = std::holds_alternative<PlusToken>(token)       ? Command::CommandADD
                                      : std::holds_alternative<MinusToken>(token)    ? Command::CommandSUB
                                      : std::holds_alternative<MultiplyToken>(token) ? Command::CommandMUL
                                      : std::holds_alternative<DivideToken>(token)   ? Command::CommandDIV
                                      : std::holds_alternative<PowerToken>(token)    ? Command::CommandPWR
                                      : std::holds_alternative<RootToken>(token)     ? Command::CommandROOT
                                      : std::holds_alternative<ModToken>(token)      ? Command::CommandMOD
                                                                                     : Command::CommandLogBaseY;
                    commands->push_back(make_shared<CBinaryCommand>(static_cast<int>(command)));
                    tokens->emplace_back(L" " + TokenDisplay(token) + L" ", static_cast<int>(i));
                }
                else
                {
                    commands->push_back(
                        make_shared<CParentheses>(
                            static_cast<int>(std::holds_alternative<LeftParenToken>(token) ? Command::CommandOPENP : Command::CommandCLOSEP)));
                    tokens->emplace_back(TokenDisplay(token), -1);
                }
            }
            tokens->emplace_back(L"=", -1);
            m_display->SetExpressionDisplay(tokens, commands);
            m_display->SetParenthesisNumber(m_openParentheses);
        }

        void PublishExpressionText(wstring expression)
        {
            if (!m_display)
            {
                return;
            }
            auto tokens = make_shared<vector<std::pair<wstring, int>>>();
            if (!expression.empty())
            {
                tokens->emplace_back(std::move(expression), -1);
            }
            auto commands = make_shared<vector<shared_ptr<IExpressionCommand>>>();
            m_display->SetExpressionDisplay(tokens, commands);
            m_display->SetParenthesisNumber(m_openParentheses);
        }

        void PublishAll()
        {
            if (m_display)
            {
                m_display->SetIsInError(m_error);
            }
            PublishPrimary();
            PublishExpression();
            if (m_display)
            {
                m_display->InputChanged();
            }
        }

        IResourceProvider* m_resourceProvider;
        ICalcDisplay* m_display;
        shared_ptr<CalculatorHistory> m_history;
        vector<Token> m_tokens;
        wstring m_input;
        std::optional<EpsilonValue> m_result;

        std::optional<ParseResult> m_exponentBase;
        size_t m_exponentOperandStart = 0;
        wstring m_exponentBaseDisplay;
        wstring m_errorExpressionCandidate;
        string m_resultSource;
        string m_repeatSource;
        string m_repeatOperator;
        wstring m_repeatDisplay;
        std::vector<string> m_memory;
        std::mt19937 m_random{ std::random_device{}() };
        wstring m_resultText = L"0";
        int32_t m_precision = 32;
        Command m_angle = Command::CommandDEG;
        unsigned m_openParentheses = 0;
        bool m_error = false;
        bool m_completed = false;
        bool m_replaceOperandOnInput = false;
        bool m_scientificFormat = false;
        bool m_inverse = false;
        bool m_hyperbolic = false;
    };

    ScientificCalculator::ScientificCalculator(IResourceProvider* resourceProvider, ICalcDisplay* displayCallback, shared_ptr<CalculatorHistory> history)
        : m_impl(std::make_unique<Impl>(resourceProvider, displayCallback, std::move(history)))
    {
    }

    ScientificCalculator::~ScientificCalculator() = default;

    void ScientificCalculator::MemorizeNumber()
    {
        m_impl->Memory(Command::CommandSTORE);
    }
    void ScientificCalculator::MemorizedNumberLoad(unsigned index)
    {
        m_impl->Memory(Command::CommandRECALL, index);
    }
    void ScientificCalculator::MemorizedNumberAdd(unsigned index)
    {
        m_impl->Memory(Command::CommandMPLUS, index);
    }
    void ScientificCalculator::MemorizedNumberSubtract(unsigned index)
    {
        m_impl->Memory(Command::CommandMMINUS, index);
    }
    void ScientificCalculator::MemorizedNumberClear(unsigned index)
    {
        m_impl->Memory(Command::CommandNULL, index);
    }
    void ScientificCalculator::MemorizedNumberClearAll()
    {
        m_impl->Memory(Command::CommandMCLEAR);
    }
    void ScientificCalculator::PublishMemory()
    {
        m_impl->PublishMemory();
    }
    string ScientificCalculator::GetExpression() const
    {
        return m_impl->CurrentSource();
    }
    void ScientificCalculator::LoadExpression(const string& source)
    {
        m_impl->Load(source);
    }
    wstring ScientificCalculator::SaveState() const
    {
        return m_impl->SaveState();
    }
    void ScientificCalculator::RestoreState(const wstring& state)
    {
        m_impl->RestoreState(state);
    }
    void ScientificCalculator::EditToken(unsigned index, Command command, bool append)
    {
        m_impl->EditToken(index, command, append);
    }
    Command ScientificCalculator::Angle() const
    {
        return m_impl->Angle();
    }
    bool ScientificCalculator::ScientificFormat() const
    {
        return m_impl->ScientificFormat();
    }

    bool ScientificCalculator::IsCommandSupported(Command command) noexcept
    {
        if (command >= Command::Command0 && command <= Command::Command9)
        {
            return true;
        }
        switch (command)
        {
        case Command::CommandDEG:
        case Command::CommandINV:
        case Command::CommandHYP:
        case Command::CommandSTORE:
        case Command::CommandRECALL:
        case Command::CommandMPLUS:
        case Command::CommandMMINUS:
        case Command::CommandMCLEAR:
        case Command::CommandRand:
        case Command::CommandPWR:
        case Command::CommandROOT:
        case Command::CommandMOD:
        case Command::CommandLogBaseY:
        case Command::CommandRAD:
        case Command::CommandGRAD:
        case Command::CommandSIGN:
        case Command::CommandCLEAR:
        case Command::CommandCENTR:
        case Command::CommandBACK:
        case Command::CommandPNT:
        case Command::CommandDIV:
        case Command::CommandMUL:
        case Command::CommandADD:
        case Command::CommandSUB:
        case Command::CommandSIN:
        case Command::CommandCOS:
        case Command::CommandTAN:
        case Command::CommandLN:
        case Command::CommandLOG:
        case Command::CommandSQRT:
        case Command::CommandSQR:
        case Command::CommandREC:
        case Command::CommandFE:
        case Command::CommandPI:
        case Command::CommandEQU:
        case Command::CommandEXP:
        case Command::CommandOPENP:
        case Command::CommandCLOSEP:
        case Command::CommandPOWE:
        case Command::CommandEuler:
            return true;
        default:
            return !FunctionName(command).empty();
        }
    }

    void ScientificCalculator::ProcessCommand(Command command)
    {
        if (!IsCommandSupported(command))
        {
            throw std::invalid_argument("unsupported ScientificCalculator command");
        }
        m_impl->Process(command);
    }

    void ScientificCalculator::Reset()
    {
        m_impl->Reset();
    }

    bool ScientificCalculator::IsInputEmpty() const
    {
        return m_impl->IsInputEmpty();
    }

    bool ScientificCalculator::IsEngineRecording() const
    {
        return m_impl->IsRecording();
    }

    void ScientificCalculator::SetPrecision(int32_t precision)
    {
        m_impl->SetPrecision(precision);
    }

    wchar_t ScientificCalculator::DecimalSeparator() const
    {
        return m_impl->DecimalSeparator();
    }

    wstring ScientificCalculator::GetResult() const
    {
        return m_impl->Result();
    }

    void ScientificCalculator::DisplayError(int32_t errorCode)
    {
        m_impl->DisplayError(errorCode);
    }
}
