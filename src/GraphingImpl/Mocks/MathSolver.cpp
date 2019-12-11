// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "MathSolver.h"

using namespace std;

namespace Graphing
{
    unique_ptr<IMathSolver> IMathSolver::CreateMathSolver()
    {
        return make_unique<MockGraphingImpl::MathSolver>();
    }
}
