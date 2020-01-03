// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Common.h"
#include "GraphingEnums.h"

namespace Graphing
{
    struct IEquationOptions : public NonCopyable, public NonMoveable
    {
        virtual ~IEquationOptions() = default;

        virtual Graphing::Color GetGraphColor() const = 0;
        virtual void SetGraphColor(const Graphing::Color& color) = 0;
        virtual void ResetGraphColor() = 0;

        virtual Graphing::Renderer::LineStyle GetLineStyle() const = 0;
        virtual void SetLineStyle(Graphing::Renderer::LineStyle value) = 0;
        virtual void ResetLineStyle() = 0;

        virtual float GetLineWidth() const = 0;
        virtual void SetLineWidth(float value) = 0;
        virtual void ResetLineWidth() = 0;

        virtual float GetSelectedEquationLineWidth() const = 0;
        virtual void SetSelectedEquationLineWidth(float value) = 0;
        virtual void ResetSelectedEquationLineWidth() = 0;

        virtual float GetPointRadius() const = 0;
        virtual void SetPointRadius(float value) = 0;
        virtual void ResetPointRadius() = 0;

        virtual float GetSelectedEquationPointRadius() const = 0;
        virtual void SetSelectedEquationPointRadius(float value) = 0;
        virtual void ResetSelectedEquationPointRadius() = 0;
    };
}