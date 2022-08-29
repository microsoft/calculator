// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "GraphingInterfaces/IGraphingOptions.h"

namespace MockGraphingImpl
{
    class GraphingOptions : public Graphing::IGraphingOptions
    {
    public:
        GraphingOptions()
            : m_markZeros(true)
            , m_markYIntercept(false)
            , m_markMinima(false)
            , m_markMaxima(false)
            , m_markInflectionPoints(false)
            , m_markVerticalAsymptotes(false)
            , m_markHorizontalAsymptotes(false)
            , m_markObliqueAsymptotes(false)
            , m_maxExecutionTime(0)
            , m_allowKeyGraphFeaturesForFunctionsWithParameters(false)
            , m_showAxis(true)
            , m_showGrid(true)
            , m_showBox(true)
            , m_forceProportional(false)
            , m_aliasX(L"x")
            , m_aliasY(L"y")
            , m_lineStyle(Graphing::Renderer::LineStyle::Solid)
            , m_XRange{ -10, 10 }
            , m_YRange{ -10, 10 }
        {
        }

        void ResetMarkKeyGraphFeaturesData() override
        {
        }

        bool GetMarkZeros() const override
        {
            return m_markZeros;
        }

        void SetMarkZeros(bool value) override
        {
            m_markZeros = value;
        }

        bool GetMarkYIntercept() const override
        {
            return m_markYIntercept;
        }

        void SetMarkYIntercept(bool value) override
        {
            m_markYIntercept = value;
        }

        bool GetMarkMinima() const override
        {
            return m_markMinima;
        }

        void SetMarkMinima(bool value) override
        {
            m_markMinima = value;
        }

        bool GetMarkMaxima() const override
        {
            return m_markMaxima;
        }

        void SetMarkMaxima(bool value) override
        {
            m_markMaxima = value;
        }

        bool GetMarkInflectionPoints() const override
        {
            return m_markInflectionPoints;
        }

        void SetMarkInflectionPoints(bool value) override
        {
            m_markInflectionPoints = value;
        }

        bool GetMarkVerticalAsymptotes() const override
        {
            return m_markVerticalAsymptotes;
        }

        void SetMarkVerticalAsymptotes(bool value) override
        {
            m_markVerticalAsymptotes = value;
        }

        bool GetMarkHorizontalAsymptotes() const override
        {
            return m_markHorizontalAsymptotes;
        }

        void SetMarkHorizontalAsymptotes(bool value) override
        {
            m_markHorizontalAsymptotes = value;
        }

        bool GetMarkObliqueAsymptotes() const override
        {
            return m_markObliqueAsymptotes;
        }

        void SetMarkObliqueAsymptotes(bool value) override
        {
            m_markObliqueAsymptotes = value;
        }

        unsigned long long GetMaxExecutionTime() const override
        {
            return m_maxExecutionTime;
        }

        void SetMaxExecutionTime(unsigned long long value) override
        {
            m_maxExecutionTime = value;
        }

        void ResetMaxExecutionTime() override
        {
            m_maxExecutionTime = 0;
        }

        std::vector<Graphing::Color> GetGraphColors() const override
        {
            return m_colors;
        }

        bool SetGraphColors(const std::vector<Graphing::Color>& colors) override
        {
            m_colors = colors;
            return true;
        }

        void ResetGraphColors() override
        {
            m_colors.clear();
        }

        Graphing::Color GetBackColor() const override
        {
            return m_backColor;
        }

        void SetBackColor(const Graphing::Color& value) override
        {
            m_backColor = value;
        }

        void ResetBackColor() override
        {
            m_backColor = Graphing::Color();
        }

        void SetAllowKeyGraphFeaturesForFunctionsWithParameters(bool kgf) override
        {
            m_allowKeyGraphFeaturesForFunctionsWithParameters = kgf;
        }

        bool GetAllowKeyGraphFeaturesForFunctionsWithParameters() const override
        {
            return m_allowKeyGraphFeaturesForFunctionsWithParameters;
        }

        void ResetAllowKeyGraphFeaturesForFunctionsWithParameters() override
        {
            m_allowKeyGraphFeaturesForFunctionsWithParameters = true;
        }

        Graphing::Color GetZerosColor() const override
        {
            return m_zerosColor;
        }

        void SetZerosColor(const Graphing::Color& value) override
        {
            m_zerosColor = value;
        }

        void ResetZerosColor() override
        {
            m_zerosColor = Graphing::Color();
        }

        Graphing::Color GetExtremaColor() const override
        {
            return m_extremaColor;
        }

        void SetExtremaColor(const Graphing::Color& value) override
        {
            m_extremaColor = value;
        }

        void ResetExtremaColor() override
        {
            m_extremaColor = Graphing::Color();
        }

        Graphing::Color GetInflectionPointsColor() const override
        {
            return m_inflectionPointsColor;
        }

        void SetInflectionPointsColor(const Graphing::Color& value) override
        {
            m_inflectionPointsColor = value;
        }

        void ResetInflectionPointsColor() override
        {
            m_inflectionPointsColor = Graphing::Color();
        }

        Graphing::Color GetAsymptotesColor() const override
        {
            return m_asymptotesColor;
        }

        void SetAsymptotesColor(const Graphing::Color& value) override
        {
            m_asymptotesColor = value;
        }

        void ResetAsymptotesColor() override
        {
            m_asymptotesColor = Graphing::Color();
        }

        Graphing::Color GetAxisColor() const override
        {
            return m_axisColor;
        }

        void SetAxisColor(const Graphing::Color& value) override
        {
            m_axisColor = value;
        }

        void ResetAxisColor() override
        {
            m_axisColor = Graphing::Color();
        }

        Graphing::Color GetBoxColor() const override
        {
            return m_boxColor;
        }

        void SetBoxColor(const Graphing::Color& value) override
        {
            m_boxColor = value;
        }

        void ResetBoxColor() override
        {
            m_boxColor = Graphing::Color();
        }

        Graphing::Color GetGridColor() const override
        {
            return m_gridColor;
        }

        void SetGridColor(const Graphing::Color& value) override
        {
            m_gridColor = value;
        }

        void ResetGridColor() override
        {
            m_gridColor = Graphing::Color();
        }

        Graphing::Color GetFontColor() const override
        {
            return m_fontColor;
        }

        void SetFontColor(const Graphing::Color& value) override
        {
            m_fontColor = value;
        }

        void ResetFontColor() override
        {
            m_fontColor = Graphing::Color();
        }

        bool GetShowAxis() const override
        {
            return m_showAxis;
        }

        void SetShowAxis(bool value) override
        {
            m_showAxis = value;
        }

        void ResetShowAxis() override
        {
            m_showAxis = true;
        }

        bool GetShowGrid() const override
        {
            return m_showGrid;
        }

        void SetShowGrid(bool value) override
        {
            m_showGrid = value;
        }

        void ResetShowGrid() override
        {
            m_showGrid = true;
        }

        bool GetShowBox() const override
        {
            return m_showBox;
        }

        void SetShowBox(bool value) override
        {
            m_showBox = value;
        }

        void ResetShowBox() override
        {
            m_showBox = true;
        }

        bool GetForceProportional() const override
        {
            return m_forceProportional;
        }

        void SetForceProportional(bool value) override
        {
            m_forceProportional = value;
        }

        void ResetForceProportional() override
        {
            m_forceProportional = false;
        }

        std::wstring GetAliasX() const override
        {
            return m_aliasX;
        }

        void SetAliasX(const std::wstring& value) override
        {
            m_aliasX = value;
        }

        void ResetAliasX() override
        {
            m_aliasX = L"";
        }

        std::wstring GetAliasY() const override
        {
            return m_aliasY;
        }

        void SetAliasY(const std::wstring& value) override
        {
            m_aliasY = value;
        }

        void ResetAliasY() override
        {
            m_aliasY = L"";
        }

        Graphing::Renderer::LineStyle GetLineStyle() const override
        {
            return m_lineStyle;
        }

        void SetLineStyle(Graphing::Renderer::LineStyle value) override
        {
            m_lineStyle = value;
        }

        void ResetLineStyle() override
        {
            m_lineStyle = Graphing::Renderer::LineStyle::Solid;
        }

        std::pair<double, double> GetDefaultXRange() const override
        {
            return m_XRange;
        }

        bool SetDefaultXRange(const std::pair<double, double>& minmax) override
        {
            m_XRange = minmax;
            return true;
        }

        void ResetDefaultXRange() override
        {
            m_XRange = { 0, 0 };
        }

        std::pair<double, double> GetDefaultYRange() const override
        {
            return m_YRange;
        }

        bool SetDefaultYRange(const std::pair<double, double>& minmax) override
        {
            m_YRange = minmax;
            return true;
        }

        void ResetDefaultYRange() override
        {
            m_YRange = { 0, 0 };
        }

    private:
        bool m_markZeros;
        bool m_markYIntercept;
        bool m_markMinima;
        bool m_markMaxima;
        bool m_markInflectionPoints;
        bool m_markVerticalAsymptotes;
        bool m_markHorizontalAsymptotes;
        bool m_markObliqueAsymptotes;
        unsigned long long m_maxExecutionTime;
        std::vector<Graphing::Color> m_colors;
        Graphing::Color m_backColor;
        bool m_allowKeyGraphFeaturesForFunctionsWithParameters;
        Graphing::Color m_zerosColor;
        Graphing::Color m_extremaColor;
        Graphing::Color m_inflectionPointsColor;
        Graphing::Color m_asymptotesColor;
        Graphing::Color m_axisColor;
        Graphing::Color m_boxColor;
        Graphing::Color m_gridColor;
        Graphing::Color m_fontColor;
        bool m_showAxis;
        bool m_showGrid;
        bool m_showBox;
        bool m_forceProportional;
        std::wstring m_aliasX;
        std::wstring m_aliasY;
        Graphing::Renderer::LineStyle m_lineStyle;
        std::pair<double, double> m_XRange;
        std::pair<double, double> m_YRange;
    };
}
