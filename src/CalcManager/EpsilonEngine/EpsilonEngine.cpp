// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "EpsilonEngine.h"
#include "Evaluator.h"

namespace CalculationManager
{
    EvaluationResult EpsilonEngine::Evaluate(std::string_view source, EvaluationLimits limits)
    {
        return Expression::Evaluator::Evaluate(source, limits);
    }
}
