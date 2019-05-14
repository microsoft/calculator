// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.Common;
using Windows.ApplicationModel.Resources;

namespace CalculatorApp
{
    public interface IResourceProvider
    {
        // Should return a string from the resource table for strings used
        // by the calculation engine. The strings that must be defined
        // and the ids to define them with can be seen in EngineStrings.h
        // with SIDS prefix. Additionally it must provide values for string
        // ids "sDecimal", "sThousand" and "sGrouping". See
        // https://technet.microsoft.com/en-us/library/cc782655(v=ws.10).aspx
        // for what these values refer to.
        string GetCEngineString(string id);
    };


    public class EngineResourceProvider : IResourceProvider
    {
        public EngineResourceProvider()
        {
            m_resLoader = ResourceLoader.GetForViewIndependentUse("CEngineStrings");
        }

        public string GetCEngineString(string id)
        {
            var localizationSettings = LocalizationSettings.GetInstance();

            if (id.CompareTo("sDecimal") == 0)
            {
                return localizationSettings.GetDecimalSeparatorStr();
            }

            if (id.CompareTo("sThousand") == 0)
            {
                return localizationSettings.GetNumberGroupingSeparatorStr();
            }

            if (id.CompareTo("sGrouping") == 0)
            {
                // The following groupings are the onces that CalcEngine supports.
                //   0;0             0x000          - no grouping
                //   3;0             0x003          - group every 3 digits
                //   3;2;0           0x023          - group 1st 3 and then every 2 digits
                //   4;0             0x004          - group every 4 digits
                //   5;3;2;0         0x235          - group 5, then 3, then every 2
                string numberGroupingString = localizationSettings.GetNumberGroupingStr();
                return numberGroupingString;
            }

            return m_resLoader.GetString(id);
        }

        Windows.ApplicationModel.Resources.ResourceLoader m_resLoader;
    };
}
