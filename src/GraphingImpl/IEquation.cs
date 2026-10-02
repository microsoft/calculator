// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace Graphing
{
    public interface IEquation
    {
        IEquationOptions GetGraphEquationOptions();
        uint GetGraphEquationID();
        bool TrySelectEquation();
        bool IsEquationSelected();
    }
}
