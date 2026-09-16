// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>
#include <future>
#include <type_traits>
#include "CalcManager/EpsilonEngine/EpsilonEngine.h"
#include "CalcManager/EpsilonEngine/Syntax.h"

using namespace CalculationManager;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EpsilonExpressionTests
{
    TEST_CLASS(ExpressionEngineTest){ public: static std::string Value(const std::string& source){ auto result = EpsilonEngine::Evaluate(source);
    if (auto* error = std::get_if<ExpressionException>(&result))
        throw *error;
    return std::get<EpsilonValue>(result).Format();
}
static void Error(const std::string& source, ExpressionError code)
{
    auto result = EpsilonEngine::Evaluate(source);
    Assert::IsTrue(std::holds_alternative<ExpressionException>(result));
    Assert::AreEqual(static_cast<int>(code), static_cast<int>(std::get<ExpressionException>(result).code));
}
TEST_METHOD(StatelessTextPipelineAndOwnedValues)
{
    static_assert(std::is_empty_v<EpsilonEngine>);
    auto value = EpsilonEngine::Evaluate(std::string("sqrt(2)"));
    Error("1/0", ExpressionError::DivideByZero);
    Assert::AreEqual(std::string("1.4142135623730950488016887242097"), std::get<EpsilonValue>(value).Format());
    auto copy = std::get<EpsilonValue>(value);
    Assert::AreEqual(copy.Format(), std::get<EpsilonValue>(value).Format());
    Assert::AreEqual(std::string("1"), Value("(1/3)*3"));
    Assert::AreEqual(std::string("0"), Value("sin(1)-sin(1)"));
}
TEST_METHOD(PrattAssociativityAndFunctionSyntax)
{
    Assert::AreEqual(std::string("14"), Value("2+3*4"));
    Assert::AreEqual(std::string("512"), Value("2^3^2"));
    Assert::AreEqual(std::string("-4"), Value("-2^2"));
    Assert::AreEqual(std::string("6"), Value("3!"));
    Assert::AreEqual(std::string("0.501"), Value(".5+1.e-3"));
    for (auto source : { "", "4+", "()", "(2+3", "2 3", "1e+", "2xyz", ".", "sin()", "sqrt(1,deg)", "deg", "sin(1,2)", "unknown(1)" })
        Error(source, ExpressionError::Syntax);
    Error(std::string("1\0+2", 4), ExpressionError::Syntax);
    auto result = EpsilonEngine::Evaluate("1+@");
    const auto& error = std::get<ExpressionException>(result);
    Assert::AreEqual(size_t(2), error.span.begin);
    Assert::AreEqual(size_t(3), error.span.end);
}
TEST_METHOD(ExplicitAnglesAndCertifiedPoles)
{
    Assert::AreEqual(std::string("0.5"), Value("sin(30,deg)"));
    Assert::AreEqual(std::string("1"), Value("sin(100,grad)"));
    Assert::AreEqual(std::string("90"), Value("asin(1,deg)"));
    Assert::AreEqual(std::string("180"), Value("acos(-1,deg)"));
    Assert::AreEqual(std::string("0"), Value("sin(pi,rad)"));
    Error("tan(90,deg)", ExpressionError::DivideByZero);
    Error("csc(0,rad)", ExpressionError::DivideByZero);
    Error("asin(2)", ExpressionError::Domain);
    Error("acosh(0)", ExpressionError::Domain);
    Error("atanh(1)", ExpressionError::Domain);
}
TEST_METHOD(PowersRootsAndDiscreteOperations)
{
    Assert::AreEqual(std::string("-8"), Value("pow(-2,3)"));
    Assert::AreEqual(std::string("-2"), Value("root(-8,3)"));
    Assert::AreEqual(std::string("0.25"), Value("pow(2,-2)"));
    Assert::AreEqual(std::string("2"), Value("-7 mod 3"));
    Assert::AreEqual(std::string("-3"), Value("floor(-2.1)"));
    Assert::AreEqual(std::string("-2"), Value("ceil(-2.1)"));
    Assert::AreEqual(std::string("1"), Value("floor(1+1e-100)"));
    Assert::AreEqual(std::string("0"), Value("floor(1-1e-100)"));
    Assert::AreEqual(std::string("12.3"), Value("dms(12.5)"));
    Assert::AreEqual(std::string("12.5"), Value("degrees(12.3)"));
    Error("0^0", ExpressionError::Undefined);
    Error("root(-8,2)", ExpressionError::Domain);
    Error("0^-1", ExpressionError::DivideByZero);
    Error("logbase(2,1)", ExpressionError::DivideByZero);
}
TEST_METHOD(RealFactorialAndFunctionReferences)
{
    Assert::AreEqual(std::string("120"), Value("fact(5)"));
    Assert::AreEqual(std::string("0.88622692545275801364908374167057"), Value("fact(0.5)"));
    Assert::AreEqual(std::string("1.7724538509055160272981674833411"), Value("fact(-0.5)"));
    Assert::AreEqual(std::string("1.3293403881791370204736256125059"), Value("fact(1.5)"));
    Assert::AreEqual(std::string("0"), Value("asinh(0)"));
    Assert::AreEqual(std::string("0"), Value("acosh(1)"));
    Assert::AreEqual(std::string("1"), Value("sech(0)"));
    Assert::AreEqual(std::string("0"), Value("tanh(0)"));
    Error("fact(-1)", ExpressionError::Domain);
}
TEST_METHOD(BoundedSourceTokensDepthAndWork)
{
    EvaluationLimits limits;
    limits.sourceCharacters = 3;
    Assert::IsTrue(std::holds_alternative<EpsilonValue>(EpsilonEngine::Evaluate("1+2", limits)));
    Assert::IsTrue(std::holds_alternative<ExpressionException>(EpsilonEngine::Evaluate("1+22", limits)));
    limits = {};
    limits.tokens = 2;
    Assert::IsTrue(std::holds_alternative<ExpressionException>(EpsilonEngine::Evaluate("1+2", limits)));
    Error(std::string(33, '(') + "1" + std::string(33, ')'), ExpressionError::ResourceLimit);
    std::string source = "1";
    for (int i = 0; i < 40; ++i)
        source = "sqr(" + source + ")";
    Error(source, ExpressionError::ResourceLimit);
    Assert::AreEqual(std::string("5"), Value("2+3"));
}
TEST_METHOD(IndependentConcurrentRequests)
{
    auto left = std::async(std::launch::async, [] { return Value("sin(30,deg)"); });
    auto right = std::async(std::launch::async, [] { return Value("(1/7)*7"); });
    Assert::AreEqual(std::string("0.5"), left.get());
    Assert::AreEqual(std::string("1"), right.get());
}
TEST_METHOD(LexerSpansAndParserOwnTheirInput)
{
    std::string source = " .5 + sin(1,deg)";
    Expression::Lexer lexer(source);
    auto token = lexer.Next();
    Assert::AreEqual(static_cast<int>(Expression::Kind::Number), static_cast<int>(token.kind));
    Assert::AreEqual(size_t(1), token.span.begin);
    Assert::AreEqual(size_t(3), token.span.end);
    auto ast = Expression::Parse(source, {});
    source.clear();
    Assert::AreEqual(std::string("+"), ast.nodes[ast.root].name);
    Assert::AreEqual(std::string("sin"), ast.nodes[ast.nodes[ast.root].children[1]].name);
    source = "1/0";
    Assert::AreEqual(std::string("/"), Expression::Parse(source, {}).nodes.back().name);
}
TEST_METHOD(UnrelatedFunctionsNeverShareExactIdentities)
{
    Assert::AreEqual(std::string("3.742447587916421222150280909772"), Value("tan(1)-tan(2)"));
    Assert::AreEqual(std::string("-18.434948822922010648427806279547"), Value("atan(1,deg)-atan(2,deg)"));
    Assert::AreEqual(std::string("1"), Value("sqr(sqrt(2))/2"));
    Assert::AreEqual(std::string("2"), Value("floor(root(8,3))"));
    Assert::AreEqual(std::string("0"), Value("ln(sqrt(2*2)/2)"));
    Assert::AreEqual(std::string("0"), Value("ln(-(-1))"));
    Assert::AreEqual(std::string("3"), Value("floor(log(100*10))"));
    Error("0*(1/0)", ExpressionError::DivideByZero);
    Error("sqrt(-1)^2", ExpressionError::Domain);
}
TEST_METHOD(NonHalfIntegerFactorialHasABoundedEnclosure)
{
    Assert::AreEqual(std::string("0.90640247705547707798267128896692"), Value("fact(0.25)"));
    Assert::AreEqual(std::string("-4.9016668098607105805163932134516"), Value("fact(-1.25)"));
}
}
;
}
