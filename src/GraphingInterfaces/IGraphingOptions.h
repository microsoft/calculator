// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <vector>
#include <string>
#include "Common.h"
#include "GraphingEnums.h"

namespace Graphing
{
    struct IGraphingOptions : public NonCopyable, public NonMoveable
    {
        virtual ~IGraphingOptions() = default;

        virtual void ResetMarkKeyGraphFeaturesData() = 0;

        virtual bool GetMarkZeros() const = 0;
        virtual void SetMarkZeros(bool value) = 0;

        virtual bool GetMarkYIntercept() const = 0;
        virtual void SetMarkYIntercept(bool value) = 0;

        virtual bool GetMarkMinima() const = 0;
        virtual void SetMarkMinima(bool value) = 0;

        virtual bool GetMarkMaxima() const = 0;
        virtual void SetMarkMaxima(bool value) = 0;

        virtual bool GetMarkInflectionPoints() const = 0;
        virtual void SetMarkInflectionPoints(bool value) = 0;

        virtual bool GetMarkVerticalAsymptotes() const = 0;
        virtual void SetMarkVerticalAsymptotes(bool value) = 0;

        virtual bool GetMarkHorizontalAsymptotes() const = 0;
        virtual void SetMarkHorizontalAsymptotes(bool value) = 0;

        virtual bool GetMarkObliqueAsymptotes() const = 0;
        virtual void SetMarkObliqueAsymptotes(bool value) = 0;

        virtual unsigned long long GetMaxExecutionTime() const = 0;
        virtual void SetMaxExecutionTime(unsigned long long value) = 0;
        virtual void ResetMaxExecutionTime() = 0;

        virtual std::vector<Graphing::Color> GetGraphColors() const = 0;
        virtual bool SetGraphColors(const std::vector<Graphing::Color>& colors) = 0;
        virtual void ResetGraphColors() = 0;

        virtual Graphing::Color GetBackColor() const = 0;
        virtual void SetBackColor(const Graphing::Color& value) = 0;
        virtual void ResetBackColor() = 0;

        virtual void SetAllowKeyGraphFeaturesForFunctionsWithParameters(bool kgf) = 0;
        virtual bool GetAllowKeyGraphFeaturesForFunctionsWithParameters() const = 0;
        virtual void ResetAllowKeyGraphFeaturesForFunctionsWithParameters() = 0;

        virtual Graphing::Color GetZerosColor() const = 0;
        virtual void SetZerosColor(const Graphing::Color& value) = 0;
        virtual void ResetZerosColor() = 0;

        virtual Graphing::Color GetExtremaColor() const = 0;
        virtual void SetExtremaColor(const Graphing::Color& value) = 0;
        virtual void ResetExtremaColor() = 0;

        virtual Graphing::Color GetInflectionPointsColor() const = 0;
        virtual void SetInflectionPointsColor(const Graphing::Color& value) = 0;
        virtual void ResetInflectionPointsColor() = 0;

        virtual Graphing::Color GetAsymptotesColor() const = 0;
        virtual void SetAsymptotesColor(const Graphing::Color& value) = 0;
        virtual void ResetAsymptotesColor() = 0;

        virtual Graphing::Color GetAxisColor() const = 0;
        virtual void SetAxisColor(const Graphing::Color& value) = 0;
        virtual void ResetAxisColor() = 0;

        virtual Graphing::Color GetBoxColor() const = 0;
        virtual void SetBoxColor(const Graphing::Color& value) = 0;
        virtual void ResetBoxColor() = 0;

        virtual Graphing::Color GetGridColor() const = 0;
        virtual void SetGridColor(const Graphing::Color& value) = 0;
        virtual void ResetGridColor() = 0;

        virtual Graphing::Color GetFontColor() const = 0;
        virtual void SetFontColor(const Graphing::Color& value) = 0;
        virtual void ResetFontColor() = 0;

        virtual bool GetShowAxis() const = 0;
        virtual void SetShowAxis(bool value) = 0;
        virtual void ResetShowAxis() = 0;

        virtual bool GetShowGrid() const = 0;
        virtual void SetShowGrid(bool value) = 0;
        virtual void ResetShowGrid() = 0;

        virtual bool GetShowBox() const = 0;
        virtual void SetShowBox(bool value) = 0;
        virtual void ResetShowBox() = 0;

        virtual bool GetForceProportional() const = 0;
        virtual void SetForceProportional(bool value) = 0;
        virtual void ResetForceProportional() = 0;

        virtual std::wstring GetAliasX() const = 0;
        virtual void SetAliasX(const std::wstring& value) = 0;
        virtual void ResetAliasX() = 0;

        virtual std::wstring GetAliasY() const = 0;
        virtual void SetAliasY(const std::wstring& value) = 0;
        virtual void ResetAliasY() = 0;

        virtual Graphing::Renderer::LineStyle GetLineStyle() const = 0;
        virtual void SetLineStyle(Graphing::Renderer::LineStyle value) = 0;
        virtual void ResetLineStyle() = 0;

        virtual std::pair<double, double> GetDefaultXRange() const = 0;
        virtual bool SetDefaultXRange(const std::pair<double, double>& minmax) = 0;
        virtual void ResetDefaultXRange() = 0;

        virtual std::pair<double, double> GetDefaultYRange() const = 0;
        virtual bool SetDefaultYRange(const std::pair<double, double>& minmax) = 0;
        virtual void ResetDefaultYRange() = 0;
    };
}
