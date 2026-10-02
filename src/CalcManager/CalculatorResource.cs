// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace CalcManager.CalculationManager
{
    public interface IResourceProvider
    {
        // Should return a string from the resource table for strings used
        // by the calculation engine. The strings that must be defined
        // and the ids to define them with can be seen in EngineStrings.cs
        // with SIDS prefix. Additionally it must provide values for string
        // ids "sDecimal", "sThousand" and "sGrouping". See
        // https://technet.microsoft.com/en-us/library/cc782655(v=ws.10).aspx
        // for what these values refer to.
        string GetCEngineString(string id);
    }
}
