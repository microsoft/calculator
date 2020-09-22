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
            , m_colors()
            , m_backColor()
            , m_allowKeyGraphFeaturesForFunctionsWithParameters(false)
            , m_zerosColor()
            , m_extremaColor()
            , m_inflectionPointsColor()
            , m_asymptotesColor()
            , m_axisColor()
            , m_boxColor()
            , m_gridColor()
            , m_fontColor()
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

        virtual void ResetMarkKeyGraphFeaturesData()
        {
        }

        virtual bool GetMarkZeros() const
        {
            return m_markZeros;
        }
        virtual void SetMarkZeros(bool value)
        {
            m_markZeros = value;
        }

        virtual bool GetMarkYIntercept() const
        {
            return m_markYIntercept;
        }
        virtual void SetMarkYIntercept(bool value)
        {
            m_markYIntercept = value;
        }

        virtual bool GetMarkMinima() const
        {
            return m_markMinima;
        }
        virtual void SetMarkMinima(bool value)
        {
            m_markMinima = value;
        }

        virtual bool GetMarkMaxima() const
        {
            return m_markMaxima;
        }
        virtual void SetMarkMaxima(bool value)
        {
            m_markMaxima = value;
        }

        virtual bool GetMarkInflectionPoints() const
        {
            return m_markInflectionPoints;
        }
        virtual void SetMarkInflectionPoints(bool value)
        {
            m_markInflectionPoints = value;
        }

        virtual bool GetMarkVerticalAsymptotes() const
        {
            return m_markVerticalAsymptotes;
        }
        virtual void SetMarkVerticalAsymptotes(bool value)
        {
            m_markVerticalAsymptotes = value;
        }

        virtual bool GetMarkHorizontalAsymptotes() const
        {
            return m_markHorizontalAsymptotes;
        }
        virtual void SetMarkHorizontalAsymptotes(bool value)
        {
            m_markHorizontalAsymptotes = value;
        }

        virtual bool GetMarkObliqueAsymptotes() const
        {
            return m_markObliqueAsymptotes;
        }
        virtual void SetMarkObliqueAsymptotes(bool value)
        {
            m_markObliqueAsymptotes = value;
        }

        virtual unsigned long long GetMaxExecutionTime() const
        {
            return m_maxExecutionTime;
        }
        virtual void SetMaxExecutionTime(unsigned long long value)
        {
            m_maxExecutionTime = value;
        }

        virtual void ResetMaxExecutionTime()
        {
            m_maxExecutionTime = 0;
        };

        virtual std::vector<Graphing::Color> GetGraphColors() const
        {
            return m_colors;
        }
        virtual bool SetGraphColors(const std::vector<Graphing::Color>& colors)
        {
            m_colors = colors;
            return true;
        }
        virtual void ResetGraphColors()
        {
            m_colors.clear();
        }

        virtual Graphing::Color GetBackColor() const
        {
            return m_backColor;
        }
        virtual void SetBackColor(const Graphing::Color& value)
        {
            m_backColor = value;
        }

        virtual void ResetBackColor()
        {
            m_backColor = Graphing::Color();
        }

        virtual void SetAllowKeyGraphFeaturesForFunctionsWithParameters(bool kgf)
        {
            m_allowKeyGraphFeaturesForFunctionsWithParameters = kgf;
        }
        virtual bool GetAllowKeyGraphFeaturesForFunctionsWithParameters() const
        {
            return m_allowKeyGraphFeaturesForFunctionsWithParameters;
        }
        virtual void ResetAllowKeyGraphFeaturesForFunctionsWithParameters()
        {
            m_allowKeyGraphFeaturesForFunctionsWithParameters = true;
        }

        virtual Graphing::Color GetZerosColor() const
        {
            return m_zerosColor;
        }
        virtual void SetZerosColor(const Graphing::Color& value)
        {
            m_zerosColor = value;
        }
        virtual void ResetZerosColor()
        {
            m_zerosColor = Graphing::Color();
        }

        virtual Graphing::Color GetExtremaColor() const
        {
            return m_extremaColor;
        }
        virtual void SetExtremaColor(const Graphing::Color& value)
        {
            m_extremaColor = value;
        }
        virtual void ResetExtremaColor()
        {
            m_extremaColor = Graphing::Color();
        }

        virtual Graphing::Color GetInflectionPointsColor() const
        {
            return m_inflectionPointsColor;
        }
        virtual void SetInflectionPointsColor(const Graphing::Color& value)
        {
            m_inflectionPointsColor = value;
        }
        virtual void ResetInflectionPointsColor()
        {
            m_inflectionPointsColor = Graphing::Color();
        }

        virtual Graphing::Color GetAsymptotesColor() const
        {
            return m_asymptotesColor;
        }
        virtual void SetAsymptotesColor(const Graphing::Color& value)
        {
            m_asymptotesColor = value;
        }
        virtual void ResetAsymptotesColor()
        {
            m_asymptotesColor = Graphing::Color();
        }

        virtual Graphing::Color GetAxisColor() const
        {
            return m_axisColor;
        }
        virtual void SetAxisColor(const Graphing::Color& value)
        {
            m_axisColor = value;
        }
        virtual void ResetAxisColor()
        {
            m_axisColor = Graphing::Color();
        }

        virtual Graphing::Color GetBoxColor() const
        {
            return m_boxColor;
        }
        virtual void SetBoxColor(const Graphing::Color& value)
        {
            m_boxColor = value;
        }
        virtual void ResetBoxColor()
        {
            m_boxColor = Graphing::Color();
        }

        virtual Graphing::Color GetGridColor() const
        {
            return m_gridColor;
        }
        virtual void SetGridColor(const Graphing::Color& value)
        {
            m_gridColor = value;
        }
        virtual void ResetGridColor()
        {
            m_gridColor = Graphing::Color();
        }

        virtual Graphing::Color GetFontColor() const
        {
            return m_fontColor;
        }
        virtual void SetFontColor(const Graphing::Color& value)
        {
            m_fontColor = value;
        }
        virtual void ResetFontColor()
        {
            m_fontColor = Graphing::Color();
        }

        virtual bool GetShowAxis() const
        {
            return m_showAxis;
        }
        virtual void SetShowAxis(bool value)
        {
            m_showAxis = value;
        }
        virtual void ResetShowAxis()
        {
            m_showAxis = true;
        }

        virtual bool GetShowGrid() const
        {
            return m_showGrid;
        }
        virtual void SetShowGrid(bool value)
        {
            m_showGrid = value;
        }
        virtual void ResetShowGrid()
        {
            m_showGrid = true;
        }

        virtual bool GetShowBox() const
        {
            return m_showBox;
        }
        virtual void SetShowBox(bool value)
        {
            m_showBox = value;
        }
        virtual void ResetShowBox()
        {
            m_showBox = true;
        }

        virtual bool GetForceProportional() const
        {
            return m_forceProportional;
        }
        virtual void SetForceProportional(bool value)
        {
            m_forceProportional = value;
        }
        virtual void ResetForceProportional()
        {
            m_forceProportional = false;
        }

        virtual std::wstring GetAliasX() const
        {
            return m_aliasX;
        }
        virtual void SetAliasX(const std::wstring& value)
        {
            m_aliasX = value;
        }
        virtual void ResetAliasX()
        {
            m_aliasX = L"";
        }

        virtual std::wstring GetAliasY() const
        {
            return m_aliasY;
        }
        virtual void SetAliasY(const std::wstring& value)
        {
            m_aliasY = value;
        }
        virtual void ResetAliasY()
        {
            m_aliasY = L"";
        }

        virtual Graphing::Renderer::LineStyle GetLineStyle() const
        {
            return m_lineStyle;
        }
        virtual void SetLineStyle(Graphing::Renderer::LineStyle value)
        {
            m_lineStyle = value;
        }
        virtual void ResetLineStyle()
        {
            m_lineStyle = Graphing::Renderer::LineStyle::Solid;
        }

        virtual std::pair<double, double> GetDefaultXRange() const
        {
            return m_XRange;
        }

        virtual bool SetDefaultXRange(const std::pair<double, double>& minmax)
        {
            m_XRange = minmax;
            return true;
        }
        virtual void ResetDefaultXRange()
        {
            m_XRange = { 0, 0 };
        }

        virtual std::pair<double, double> GetDefaultYRange() const
        {
            return m_YRange;
        }

        virtual bool SetDefaultYRange(const std::pair<double, double>& minmax)
        {
            m_YRange = minmax;
            return true;
        }
        virtual void ResetDefaultYRange()
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
