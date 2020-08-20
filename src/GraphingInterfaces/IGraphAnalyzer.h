// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Common.h"
#include "GraphingEnums.h"
#include <string>
#include <vector>
#include <map>

namespace Graphing::Analyzer
{
	typedef unsigned int NativeAnalysisType; // PerformAnalysisType

	struct IGraphAnalyzer : public NonCopyable, public NonMoveable
	{
        virtual ~IGraphAnalyzer() = default;
        virtual bool CanFunctionAnalysisBePerformed(bool& variableIsNotX) = 0;
        virtual HRESULT PerformFunctionAnalysis(NativeAnalysisType analysisType) = 0;
        virtual HRESULT GetAnalysisTypeCaption(const AnalysisType type, std::wstring& captionOut) const = 0;
        virtual HRESULT GetMessage(const GraphAnalyzerMessage msg, std::wstring& msgOut) const = 0;
	};
}
