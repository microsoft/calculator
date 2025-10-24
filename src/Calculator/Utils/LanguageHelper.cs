// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using Windows.Storage;

namespace CalculatorApp.Utils
{
    /// <summary>
    /// Helper class for managing application language settings
    /// </summary>
    public static class LanguageHelper
    {
        private const string SelectedLanguageKey = "SelectedLanguage"; // "system" or BCP-47 tag

        /// <summary>
        /// Initialize the application to use the System Display Language instead of Regional Settings
        /// </summary>
        public static void InitializeDisplayLanguage()
        {
            try
            {
                var displayLanguage = GetSystemDisplayLanguage();
                if (!string.IsNullOrEmpty(displayLanguage))
                {
                    Debug.WriteLine($"Setting Calculator to use Display Language: {displayLanguage}");
                    SetApplicationLanguage(displayLanguage);
                }
                else
                {
                    Debug.WriteLine("Could not detect system display language, using default behavior");
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Error initializing display language: {ex.Message}");
                // Don't throw - let the app continue with default behavior
            }
        }

        /// <summary>
        /// Persisted selected language code. "system" means follow system (Display Language)
        /// </summary>
        public static string SelectedLanguage
        {
            get => ApplicationData.Current.LocalSettings.Values[SelectedLanguageKey]?.ToString() ?? "system";
            set => ApplicationData.Current.LocalSettings.Values[SelectedLanguageKey] = value;
        }

        /// <summary>
        /// Return available UI languages for selection (System Default + supported app languages)
        /// </summary>
        public static List<LanguageInfo> GetAvailableLanguages()
        {
            var list = new List<LanguageInfo>();
            list.Add(new LanguageInfo { Code = "system", NativeName = "System Default" });

            foreach (var code in GetSupportedLanguageCodes())
            {
                try
                {
                    var lang = new Windows.Globalization.Language(code);
                    list.Add(new LanguageInfo { Code = code, NativeName = lang.NativeName });
                }
                catch
                {
                    list.Add(new LanguageInfo { Code = code, NativeName = code });
                }
            }

            return list;
        }

        private static IEnumerable<string> GetSupportedLanguageCodes()
        {
            // Full list based on src/Calculator/Resources/* folders
            return new[]
            {
                "af-ZA","am-ET","ar-SA","az-Latn-AZ","bg-BG","ca-ES","cs-CZ","da-DK","de-DE","el-GR",
                "en-GB","en-US","es-ES","es-MX","et-EE","eu-ES","fa-IR","fi-FI","fil-PH","fr-CA","fr-FR",
                "gl-ES","he-IL","hi-IN","hr-HR","hu-HU","id-ID","is-IS","it-IT","ja-JP","kk-KZ","km-KH",
                "kn-IN","ko-KR","lo-LA","lt-LT","lv-LV","mk-MK","ml-IN","ms-MY","nb-NO","nl-NL","pl-PL",
                "pt-BR","pt-PT","ro-RO","ru-RU","sk-SK","sl-SI","sq-AL","sr-Latn-RS","sv-SE","ta-IN",
                "te-IN","th-TH","tr-TR","uk-UA","vi-VN","zh-CN","zh-TW"
            };
        }

        /// <summary>
        /// Apply a user-selected language code (or "system") immediately and persist preference
        /// </summary>
        public static void ApplyUserLanguageSelection(string code)
        {
            SelectedLanguage = code;

            string languageToUse = code == "system" ? GetSystemDisplayLanguage() : code;
            if (string.IsNullOrEmpty(languageToUse))
            {
                Debug.WriteLine("No language resolved; skipping override");
                return;
            }

            SetApplicationLanguage(languageToUse);
            ApplyViewResourceLanguages(languageToUse);
            ApplyGlobalResourceLanguages(languageToUse);
        }

        /// <summary>
        /// Gets the system's Display Language (not Regional Settings)
        /// </summary>
        /// <returns>The primary display language code (e.g., "en-US", "ja-JP")</returns>
        public static string GetSystemDisplayLanguage()
        {
            try
            {
                // Primary method: Get the user's preferred UI languages from GlobalizationPreferences
                // This returns the Display Language, not Regional Settings
                var userLanguages = Windows.System.UserProfile.GlobalizationPreferences.Languages;
                if (userLanguages.Count > 0)
                {
                    var primaryLanguage = userLanguages[0];
                    Debug.WriteLine($"Display Language from GlobalizationPreferences: {primaryLanguage}");
                    return primaryLanguage;
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Error getting GlobalizationPreferences: {ex.Message}");
                
                // Fallback method: Try ResourceContext
                try
                {
                    var resourceContext = Windows.ApplicationModel.Resources.Core.ResourceContext.GetForCurrentView();
                    if (resourceContext.Languages.Count > 0)
                    {
                        var primaryLanguage = resourceContext.Languages[0];
                        Debug.WriteLine($"Display Language from ResourceContext: {primaryLanguage}");
                        return primaryLanguage;
                    }
                }
                catch (Exception ex2)
                {
                    Debug.WriteLine($"Error getting ResourceContext languages: {ex2.Message}");
                }
            }
            
            return null;
        }

        /// <summary>
        /// Sets the application language using ApplicationLanguages.PrimaryLanguageOverride
        /// </summary>
        /// <param name="languageCode">Language code (e.g., "en-US", "ja-JP")</param>
        public static void SetApplicationLanguage(string languageCode)
        {
            try
            {
                // Set the primary language override for this application
                // This overrides the default language resolution and forces the app to use the specified language
                Windows.Globalization.ApplicationLanguages.PrimaryLanguageOverride = languageCode;
                Debug.WriteLine($"ApplicationLanguages.PrimaryLanguageOverride set to: {languageCode}");
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Error setting application language to {languageCode}: {ex.Message}");
                throw;
            }
        }

        /// <summary>
        /// Apply the specified language to ResourceContext used outside of any view
        /// </summary>
        public static void ApplyGlobalResourceLanguages(string languageCode)
        {
            try
            {
                // Set process-wide qualifier for language
                Windows.ApplicationModel.Resources.Core.ResourceContext.SetGlobalQualifierValue("Language", languageCode);
                Debug.WriteLine($"Global qualifier 'Language' set to: {languageCode}");
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Error applying global resource language: {ex.Message}");
            }
        }

        /// <summary>
        /// Apply the specified language to ResourceContext for the current view
        /// </summary>
        public static void ApplyViewResourceLanguages(string languageCode)
        {
            try
            {
                var ctx = Windows.ApplicationModel.Resources.Core.ResourceContext.GetForCurrentView();
                // Set qualifier value for this view
                ctx.QualifierValues["Language"] = languageCode;
                Debug.WriteLine($"View qualifier 'Language' set to: {languageCode}");
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Error applying view resource language: {ex.Message}");
            }
        }

        /// <summary>
        /// Clears the application language override (for debugging/testing)
        /// </summary>
        public static void ClearApplicationLanguageOverride()
        {
            try
            {
                Windows.Globalization.ApplicationLanguages.PrimaryLanguageOverride = "";
                Debug.WriteLine("ApplicationLanguages.PrimaryLanguageOverride cleared");
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Error clearing application language override: {ex.Message}");
            }
        }

        /// <summary>
        /// Gets debug information about current language settings
        /// </summary>
        public static void LogLanguageDebugInfo()
        {
            try
            {
                var currentOverride = Windows.Globalization.ApplicationLanguages.PrimaryLanguageOverride;
                var systemLanguages = Windows.System.UserProfile.GlobalizationPreferences.Languages;
                var appLanguages = Windows.Globalization.ApplicationLanguages.Languages;

                Debug.WriteLine("=== Language Debug Info ===");
                Debug.WriteLine($"PrimaryLanguageOverride: '{currentOverride}'");
                Debug.WriteLine($"System Display Languages: {string.Join(", ", systemLanguages)}");
                Debug.WriteLine($"Application Languages: {string.Join(", ", appLanguages)}");
                Debug.WriteLine("===========================");
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Error getting language debug info: {ex.Message}");
            }
        }
    }

    public class LanguageInfo
    {
        public string Code { get; set; }
        public string NativeName { get; set; }

        public override string ToString()
        {
            return string.IsNullOrEmpty(NativeName) ? Code : NativeName;
        }
    }
}
