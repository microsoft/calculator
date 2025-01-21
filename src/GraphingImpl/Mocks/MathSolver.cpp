// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "MathSolver.h"
#include "Mocks/Graph.h"

using namespace std;

namespace Graphing
{
    unique_ptr<IMathSolver> IMathSolver::CreateMathSolver()
    {
        return make_unique<MockGraphingImpl::MathSolver>();
    }
}

shared_ptr<Graphing::IGraph> MockGraphingImpl::MathSolver::CreateGrapher()
{
    return make_shared<MockGraphingImpl::Graph>();
}

shared_ptr<Graphing::IGraph> MockGraphingImpl::MathSolver::CreateGrapher(const Graphing::IExpression* /*expression*/)
{
    return make_shared<MockGraphingImpl::Graph>();
}
