// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;
using Graphing;
using Graphing.Analyzer;
using Graphing.Renderer;

namespace MockGraphingImpl
{
    public class Graph : IGraph
    {
        private readonly List<IVariable> _variables = new List<IVariable>();
        private readonly GraphingOptions _graphingOptions = new GraphingOptions();
        private readonly IGraphRenderer _graphRenderer = new GraphRenderer();

        public IReadOnlyList<IEquation> TryInitialize(IExpression graphingExp = null)
        {
            if (graphingExp != null)
            {
                var equations = new List<IEquation> { null };
                _variables.Add(new MockVariable());
                return equations;
            }
            return null;
        }

        public int GetInitializationError() => 0; // S_OK

        public IGraphingOptions GetOptions() => _graphingOptions;

        public IReadOnlyList<IVariable> GetVariables() => _variables;

        public void SetArgValue(string variableName, double value) { }

        public IGraphRenderer GetRenderer() => _graphRenderer;

        public bool TryResetSelection() => true;

        public IGraphAnalyzer GetAnalyzer() => null;
    }
}
