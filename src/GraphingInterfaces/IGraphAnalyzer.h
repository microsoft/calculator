#pragma once

#include "Common.h"
#include "GraphingEnums.h"
#include <string>
#include <IMathSolver.h>

namespace Graphing::Analyzer
{

	struct IParityItem : public NonCopyable, public NonMoveable
	{
		virtual ~IParityItem() = default;
		virtual FunctionParityType GetParity() const = 0;
		virtual HRESULT Serialize(std::wstring& msgOut) const = 0;
	};

	struct ISingleExpressionItem : public NonCopyable, public NonMoveable
	{
		virtual ~ISingleExpressionItem() = default;
		virtual const std::shared_ptr<MathSolverEngine::IExpression>& GetExpression() const = 0;
		virtual HRESULT Serialize(std::wstring& msgOut) const = 0;
	};

	struct IHorizontalObliqueAsymptoteItem : public NonCopyable, public NonMoveable
	{
		virtual ~IHorizontalObliqueAsymptoteItem() = default;
		virtual const std::shared_ptr<MathSolverEngine::IExpression>& GetExpression() const = 0;
		virtual AsymptoteType GetAsymptoteDescription() const = 0;
		virtual HRESULT Serialize(std::wstring& msgOut, std::vector<std::wstring>& dataOut) const = 0;
	};

	struct IMonotonicityItem : public NonCopyable, public NonMoveable
	{
		virtual ~IMonotonicityItem() = default;
		virtual const std::shared_ptr<MathSolverEngine::IExpression>& GetIntervalExpression() const = 0;
		virtual FunctionMonotonicityType GetMonotonicityDirection() const = 0;
		virtual HRESULT Serialize(std::wstring& msgOut, std::vector<std::wstring>& dataOut) const = 0;
	};

	struct IPeriodicityItem : public NonCopyable, public NonMoveable
	{
		virtual ~IPeriodicityItem() = default;
		virtual FunctionPeriodicityType GetPeriodicity() const = 0;
		virtual const std::shared_ptr<MathSolverEngine::IExpression>& GetExpression() const = 0;
		virtual HRESULT Serialize(std::wstring& msgOut) const = 0;
		virtual bool Supported() const = 0;
	};

	struct IGraphFunctionAnalysisData : public NonCopyable, public NonMoveable
	{
		virtual ~IGraphFunctionAnalysisData() = default;
		virtual HRESULT GetDomain(std::shared_ptr<Analyzer::ISingleExpressionItem>& domainItemsOut) = 0;
		virtual HRESULT GetRange(std::shared_ptr<Analyzer::ISingleExpressionItem>& rangeItemsOut) = 0;
		virtual HRESULT GetParity(std::shared_ptr<Analyzer::IParityItem>& parityItemsOut) = 0;
		virtual HRESULT GetPeriodicity(std::shared_ptr<Analyzer::IPeriodicityItem>& periodItemOut) = 0;
		virtual HRESULT GetZeros(std::shared_ptr<Analyzer::ISingleExpressionItem>& zerosItemsOut) = 0;
		virtual HRESULT GetYIntercept(std::shared_ptr<Analyzer::ISingleExpressionItem>& yInterceptItemsOut) = 0;
		virtual HRESULT GetMinima(std::vector<std::shared_ptr<Analyzer::ISingleExpressionItem>>& minimaItemsOut) = 0;
		virtual HRESULT GetMaxima(std::vector<std::shared_ptr<Analyzer::ISingleExpressionItem>>& maximaItemsOut) = 0;
		virtual HRESULT GetInflectionPoints(std::vector<std::shared_ptr<Analyzer::ISingleExpressionItem>>& inflectionPointItemsOut) = 0;
		virtual HRESULT GetVerticalAsymptotes(std::vector<std::shared_ptr<Analyzer::ISingleExpressionItem>>& verticalAsymptoteItemsOut) = 0;
		virtual HRESULT GetHorizontalAsymptotes(std::vector<std::shared_ptr<Analyzer::IHorizontalObliqueAsymptoteItem>>& horizontalAsymptoteItemsOut) = 0;
		virtual HRESULT GetObliqueAsymptotes(std::vector<std::shared_ptr<Analyzer::IHorizontalObliqueAsymptoteItem>>& obliqueAsymptoteItemsOut) = 0;
		virtual HRESULT GetMonotoneIntervals(std::vector<std::shared_ptr<Analyzer::IMonotonicityItem>>& monotoneIntervalItemsOut) = 0;
	};

	typedef unsigned int NativeAnalysisType; // PerformAnalysisType

	struct IGraphAnalyzer : public NonCopyable, public NonMoveable
	{
		virtual ~IGraphAnalyzer() = default;
		virtual HRESULT CanFunctionAnalysisBePerformed(bool& result) = 0;
		virtual HRESULT PerformFunctionAnalysis(NativeAnalysisType analysisType) = 0;
		virtual HRESULT GetFunctionAnalysisData(std::vector<std::shared_ptr<Analyzer::IGraphFunctionAnalysisData>>& functionAnalysisDataOut) = 0;
		virtual HRESULT GetAnalysisTypeCaption(const AnalysisType type, std::wstring& captionOut) const = 0;
		virtual HRESULT GetMessage(const GraphAnalyzerMessage msg, std::wstring& msgOut) const = 0;
	};

}