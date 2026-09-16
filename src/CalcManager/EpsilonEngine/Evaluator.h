// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "EpsilonEngine.h"

namespace CalculationManager::Expression
{
    class Evaluator
    {
    public:
        static EvaluationResult Evaluate(std::string_view source, EvaluationLimits limits);
    };
}
