// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.ApplicationModel.Resources;

namespace CalculatorApp.ViewModel.Common
{
    /// <summary>
    /// Provides localized engine resource strings. No longer inherits from a WinRT
    /// base class; the GetCEngineString method is called via a delegate passed to
    /// CalculatorManagerWrapper.
    /// </summary>
    internal class EngineResourceProvider
    {
        private ResourceLoader _resLoader;

        public EngineResourceProvider()
        {
            try
            {
                _resLoader = ResourceLoader.GetForViewIndependentUse("CEngineStrings");
            }
            catch
            {
                _resLoader = null;
            }
        }

        public string GetCEngineString(string id)
        {
            try
            {
                var localizationSettings = LocalizationSettings.GetInstance();

                if (id == "sDecimal")
                {
                    return localizationSettings.GetDecimalSeparatorStr();
                }

                if (id == "sThousand")
                {
                    return localizationSettings.GetNumberGroupingSeparatorStr();
                }

                if (id == "sGrouping")
                {
                    return localizationSettings.GetNumberGroupingStr();
                }

                return _resLoader?.GetString(id) ?? "";
            }
            catch
            {
                // Return reasonable fallbacks to prevent native engine crash
                if (id == "sDecimal") return ".";
                if (id == "sThousand") return ",";
                if (id == "sGrouping") return "3;0";
                return "";
            }
        }
    }
}
