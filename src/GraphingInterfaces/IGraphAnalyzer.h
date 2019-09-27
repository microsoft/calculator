#pragma once

#include "Common.h"
#include "GraphingEnums.h"
#include <string>
#include <vector>
#include <map>

namespace Graphing::Analyzer
{
	struct IGraphFunctionAnalysisData
	{
		std::wstring Domain;
		std::wstring Range;
		int Parity;
		int Periodicity;
		std::wstring Zeros;
		std::wstring YIntercept;
		std::vector<std::wstring> Minima;
		std::vector<std::wstring> Maxima;
		std::vector<std::wstring> InflectionPoints;
		std::vector<std::wstring> VerticalAsymptotes;
		std::vector<std::wstring> HorizontalAsymptotes;
		std::vector<std::wstring> ObliqueAsymptotes;
		std::map<const Graphing::IExpression*, int> MonotoneIntervals;
	};

	typedef unsigned int NativeAnalysisType; // PerformAnalysisType

	struct IGraphAnalyzer : public NonCopyable, public NonMoveable
	{
		virtual ~IGraphAnalyzer() = default;
		virtual HRESULT CanFunctionAnalysisBePerformed(bool& result) = 0;
		virtual HRESULT PerformFunctionAnalysis(NativeAnalysisType analysisType) = 0;
		virtual HRESULT GetFunctionAnalysisData(std::vector<Analyzer::IGraphFunctionAnalysisData>& functionAnalysisDataOut) = 0;
		virtual HRESULT GetAnalysisTypeCaption(const AnalysisType type, std::wstring& captionOut) const = 0;
		virtual HRESULT GetMessage(const GraphAnalyzerMessage msg, std::wstring& msgOut) const = 0;
	};
}