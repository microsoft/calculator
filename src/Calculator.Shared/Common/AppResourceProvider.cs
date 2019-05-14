// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.ApplicationModel.Resources;

namespace CalculatorApp
{
    public class AppResourceProvider
    {
        Windows.ApplicationModel.Resources.ResourceLoader m_stringResLoader;
        Windows.ApplicationModel.Resources.ResourceLoader m_cEnginestringResLoader;
        private static AppResourceProvider s_instance;

        public AppResourceProvider()
        {
            m_stringResLoader = ResourceLoader.GetForViewIndependentUse();
            m_cEnginestringResLoader = ResourceLoader.GetForViewIndependentUse("CEnginestrings");
        }

        public static AppResourceProvider GetInstance()
        {
            if(s_instance == null)
            {
                s_instance = new AppResourceProvider();
            }

            return s_instance;
        }

        public string GetResourceString(string key)
        {
            return m_stringResLoader.GetString(key);
        }

        public string GetCEngineString(string key)
        {
            return m_cEnginestringResLoader.GetString(key);
        }
    }
}
