// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;
using Graphing.Analyzer;
using Graphing.Renderer;

namespace Graphing
{
    public interface IGraph
    {
        IReadOnlyList<IEquation> TryInitialize(IExpression graphingExp = null);
        int GetInitializationError();
        IGraphingOptions GetOptions();
        IReadOnlyList<IVariable> GetVariables();
        void SetArgValue(string variableName, double value);
        IGraphRenderer GetRenderer();
        bool TryResetSelection();
        IGraphAnalyzer GetAnalyzer();
    }
}
