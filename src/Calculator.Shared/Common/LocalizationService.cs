// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;
using Windows.ApplicationModel.Resources.Core;
using Windows.Globalization;
using Windows.Globalization.DateTimeFormatting;
using Windows.Globalization.Fonts;
using Windows.Globalization.NumberFormatting;
using Windows.System.UserProfile;
using Windows.UI.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Media;

namespace CalculatorApp.Common
{

    public enum LanguageFontType
    {
        UIText,
        UICaption,
    };

    public sealed class LocalizationService
    {
        static string DefaultCurrencyCode = "USD";



        public static LanguageFontType GetFontType(DependencyObject obj) => (LanguageFontType)obj.GetValue(FontTypeProperty);

        public static void SetFontType(DependencyObject obj, LanguageFontType value) => obj.SetValue(FontTypeProperty, value);

        public static readonly DependencyProperty FontTypeProperty =
            DependencyProperty.RegisterAttached("FontType", typeof(LanguageFontType), typeof(LocalizationService), new PropertyMetadata(LanguageFontType.UIText));


        public static double GetFontSize(DependencyObject obj) => (double)obj.GetValue(FontSizeProperty);

        public static void SetFontSize(DependencyObject obj, double value) => obj.SetValue(FontSizeProperty, value);

        public static readonly DependencyProperty FontSizeProperty =
            DependencyProperty.RegisterAttached("FontSize", typeof(double), typeof(LocalizationService), new PropertyMetadata(0));

        static LocalizationService s_singletonInstance;

        Windows.Globalization.Fonts.LanguageFontGroup m_fontGroup;
        string m_language;
        Windows.UI.Xaml.FlowDirection m_flowDirection;
        bool m_overrideFontApiValues;
        string m_fontFamilyOverride;
        Windows.UI.Text.FontWeight m_fontWeightOverride;
        double m_uiTextFontScaleFactorOverride;
        double m_uiCaptionFontScaleFactorOverride;


        // Resources for the engine use numbers as keys. It's inconvenient, but also difficult to
        // change given that the engine heavily relies on perfect ordering of certain elements.
        // The key for open parenthesis, '(', is "48".
        static  string s_openParenResourceKey = "48";

        public static LocalizationService GetInstance()
        {
            if (s_singletonInstance == null)
            {
                if (s_singletonInstance == null)
                {
                    s_singletonInstance =  new LocalizationService();
                }
            }

            return s_singletonInstance;
        }

        LocalizationService()
        {
#if HAS_UNO
            m_language = "en-US";
            m_flowDirection = FlowDirection.LeftToRight;
#else
            m_language = ApplicationLanguages.Languages[0];
            m_flowDirection =
                ResourceContext.GetForCurrentView().QualifierValues["LayoutDirection"] != "LTR" ? FlowDirection.RightToLeft : FlowDirection.LeftToRight;
#endif

            var resourceLoader = AppResourceProvider.GetInstance();
            m_fontFamilyOverride = resourceLoader.GetResourceString("LocalizedFontFamilyOverride");

            string reserved = "RESERVED_FOR_FONTLOC";

            m_overrideFontApiValues = ((m_fontFamilyOverride != null) && (m_fontFamilyOverride != reserved));
            if (m_overrideFontApiValues)
            {
                string localizedUICaptionFontSizeFactorOverride = resourceLoader.GetResourceString("LocalizedUICaptionFontSizeFactorOverride");
                string localizedUITextFontSizeFactorOverride = resourceLoader.GetResourceString("LocalizedUITextFontSizeFactorOverride");
                string localizedFontWeightOverride = resourceLoader.GetResourceString("LocalizedFontWeightOverride");

                // If any of the font overrides are modified then all of them need to be modified
                Debug.Assert(localizedFontWeightOverride != reserved);
                Debug.Assert(localizedUITextFontSizeFactorOverride != reserved);
                Debug.Assert(localizedUICaptionFontSizeFactorOverride != reserved);

                m_fontWeightOverride = ParseFontWeight(localizedFontWeightOverride);
                m_uiTextFontScaleFactorOverride = double.Parse(localizedUITextFontSizeFactorOverride);
                m_uiCaptionFontScaleFactorOverride = double.Parse(localizedUICaptionFontSizeFactorOverride);
            }

            m_fontGroup =  new LanguageFontGroup(m_language);
        }

        FontWeight ParseFontWeight(string fontWeight)
        {
            fontWeight = fontWeight.ToLower();

            if (fontWeight == "black")
            {
                return FontWeights.Black;
            }
            else if (fontWeight == "bold")
            {
                return FontWeights.Bold;
            }
            else if (fontWeight == "extrablack")
            {
                return FontWeights.ExtraBlack;
            }
            else if (fontWeight == "extrabold")
            {
                return FontWeights.ExtraBold;
            }
            else if (fontWeight == "extralight")
            {
                return FontWeights.ExtraLight;
            }
            else if (fontWeight == "light")
            {
                return FontWeights.Light;
            }
            else if (fontWeight == "medium")
            {
                return FontWeights.Medium;
            }
            else if (fontWeight == "normal")
            {
                return FontWeights.Normal;
            }
            else if (fontWeight == "semibold")
            {
                return FontWeights.SemiBold;
            }
            else if (fontWeight == "semilight")
            {
                return FontWeights.SemiLight;
            }
            else if (fontWeight == "thin")
            {
                return FontWeights.Thin;
            }
            else
            {
                throw new ArgumentException("Invalid argument: fontWeight");
            }
        }

        FlowDirection GetFlowDirection()
        {
            return m_flowDirection;
        }

        public bool IsRtlLayout()
        {
            return m_flowDirection == FlowDirection.RightToLeft;
        }

        string GetLanguage()
        {
            return m_language;
        }

        bool GetOverrideFontApiValues()
        {
            return m_overrideFontApiValues;
        }

        public FontFamily GetLanguageFontFamilyForType(LanguageFontType fontType)
        {
            if (m_overrideFontApiValues)
            {
                return  new FontFamily(m_fontFamilyOverride);
            }
            else
            {
                return  new FontFamily(GetLanguageFont(fontType).FontFamily);
            }
        }

        LanguageFont GetLanguageFont(LanguageFontType fontType)
        {
            Debug.Assert(!m_overrideFontApiValues);
            Debug.Assert(m_fontGroup != null);

            switch (fontType)
            {
            case LanguageFontType.UIText:
                return m_fontGroup.UITextFont;
            case LanguageFontType.UICaption:
                return m_fontGroup.UICaptionFont;
            default:
                throw new ArgumentException("fontType");
            }
        }

        string GetFontFamilyOverride()
        {
            Debug.Assert(m_overrideFontApiValues);
            return m_fontFamilyOverride;
        }

        FontWeight GetFontWeightOverride()
        {
            Debug.Assert(m_overrideFontApiValues);
            return m_fontWeightOverride;
        }

        double GetFontScaleFactorOverride(LanguageFontType fontType)
        {
            Debug.Assert(m_overrideFontApiValues);

            switch (fontType)
            {
            case LanguageFontType.UIText:
                return m_uiTextFontScaleFactorOverride;
            case LanguageFontType.UICaption:
                return m_uiCaptionFontScaleFactorOverride;
            default:
                throw new ArgumentException("Invalid argument: fontType");
            }
        }

        void OnFontTypePropertyChanged(DependencyObject target, LanguageFontType oldValue, LanguageFontType newValue)
        {
            UpdateFontFamilyAndSize(target);
        }

        void OnFontWeightPropertyChanged(DependencyObject target, FontWeight oldValue, FontWeight newValue)
        {
            UpdateFontFamilyAndSize(target);
        }

        void OnFontSizePropertyChanged(DependencyObject target, double oldValue, double newValue)
        {
            UpdateFontFamilyAndSize(target);
        }

        void UpdateFontFamilyAndSize(DependencyObject target)
        {
            FontFamily fontFamily;
            FontWeight fontWeight = FontWeights.Normal;
            bool fOverrideFontWeight = false;
            double scaleFactor;

            var service = GetInstance();
            var fontType = GetFontType(target);

            if (service.GetOverrideFontApiValues())
            {
                fontFamily =  new FontFamily(service.GetFontFamilyOverride());
                scaleFactor = service.GetFontScaleFactorOverride(fontType) / 100.0;
                fontWeight = service.GetFontWeightOverride();
                fOverrideFontWeight = true;
            }
            else
            {
                var languageFont = service.GetLanguageFont(fontType);
                fontFamily =  new FontFamily(languageFont.FontFamily);
                scaleFactor = languageFont.ScaleFactor / 100.0;
            }

            double sizeToUse = GetFontSize(target) * scaleFactor;

            var control = target as Control;
            if (control != null)
            {
                control.FontFamily = fontFamily;
                if (fOverrideFontWeight)
                {
                    control.FontWeight = fontWeight;
                }
                if (sizeToUse != 0.0)
                {
                    control.FontSize = sizeToUse;
                }
                else
                {
                    control.ClearValue(Control.FontSizeProperty);
                }
            }
            else
            {
                var textBlock = target as TextBlock;
                if (textBlock != null)
                {
                    textBlock.FontFamily = fontFamily;
                    if (fOverrideFontWeight)
                    {
                        textBlock.FontWeight = fontWeight;
                    }
                    if (sizeToUse != 0.0)
                    {
                        textBlock.FontSize = sizeToUse;
                    }
                    else
                    {
                        textBlock.ClearValue(TextBlock.FontSizeProperty);
                    }
                }
                else
                {
                    RichTextBlock richTextBlock = (target as RichTextBlock);
                    if (richTextBlock != null)
                    {
                        richTextBlock.FontFamily = fontFamily;
                        if (fOverrideFontWeight)
                        {
                            richTextBlock.FontWeight = fontWeight;
                        }
                        if (sizeToUse != 0.0)
                        {
                            richTextBlock.FontSize = sizeToUse;
                        }
                        else
                        {
                            richTextBlock.ClearValue(RichTextBlock.FontSizeProperty);
                        }
                    }
                    else
                    {
                        TextElement textElement = (target as TextElement);
                        if (textElement != null)
                        {
                            textElement.FontFamily = fontFamily;
                            if (fOverrideFontWeight)
                            {
                                textElement.FontWeight = fontWeight;
                            }
                            if (sizeToUse != 0.0)
                            {
                                textElement.FontSize = sizeToUse;
                            }
                            else
                            {
                                textElement.ClearValue(TextElement.FontSizeProperty);
                            }
                        }
                    }
                }
            }
        }

        // If successful, returns a formatter that respects the user's regional format settings,
        // as configured by running intl.cpl.
        public static DecimalFormatter GetRegionalSettingsAwareDecimalFormatter()
        {
            IEnumerable<String> languageIdentifiers = GetLanguageIdentifiers();
            if (languageIdentifiers != null)
            {
                return  new DecimalFormatter(languageIdentifiers, GlobalizationPreferences.HomeGeographicRegion);
            }

            return  new DecimalFormatter();
        }

        // If successful, returns a formatter that respects the user's regional format settings,
        // as configured by running intl.cpl.
        //
        // This helper function creates a DateTimeFormatter with a TwentyFour hour clock
        DateTimeFormatter GetRegionalSettingsAwareDateTimeFormatter( string format)
        {
            IEnumerable<String> languageIdentifiers = GetLanguageIdentifiers();
            if (languageIdentifiers == null)
            {
                languageIdentifiers = ApplicationLanguages.Languages;
            }

            return  new DateTimeFormatter(format, languageIdentifiers);
        }

        // If successful, returns a formatter that respects the user's regional format settings,
        // as configured by running intl.cpl.
        DateTimeFormatter GetRegionalSettingsAwareDateTimeFormatter( string format,  string calendarIdentifier,  string clockIdentifier)
        {
            // UNO TODO
            //IIterable<String> languageIdentifiers = GetLanguageIdentifiers();
            //if (languageIdentifiers == null)
            //{
            //    languageIdentifiers = ApplicationLanguages.Languages;
            //}

            //return  new DateTimeFormatter(format, languageIdentifiers, GlobalizationPerences.HomeGeographicRegion, calendarIdentifier, clockIdentifier);

            throw new NotImplementedException();
        }

        public CurrencyFormatter GetRegionalSettingsAwareCurrencyFormatter()
        {
            // UNO TODO
            //string userCurrency =
            //    (GlobalizationPreferences.Currencies.Size > 0) ? GlobalizationPreferences.Currencies.GetAt(0) : string(DefaultCurrencyCode.data());

            //IIterable<String> languageIdentifiers = GetLanguageIdentifiers();
            //if (languageIdentifiers == null)
            //{
            //    languageIdentifiers = ApplicationLanguages.Languages;
            //}

            //var currencyFormatter =  new CurrencyFormatter(userCurrency, languageIdentifiers, GlobalizationPerences.HomeGeographicRegion);

            //int fractionDigits = LocalizationSettings.GetInstance().GetCurrencyTrailingDigits();
            //currencyFormatter.FractionDigits = fractionDigits;

            //return currencyFormatter;

            throw new NotImplementedException();
        }

        static IEnumerable<String> GetLanguageIdentifiers()
        {
#if !HAS_UNO
            //char currentLocale[LOCALE_NAME_MAX_LENGTH] = {};
            //int result = GetUserDefaultLocaleName(currentLocale, LOCALE_NAME_MAX_LENGTH);
            //if (result != 0)
            //{
            //    // GetUserDefaultLocaleName may return an invalid bcp47 language tag with trailing non-BCP47 friendly characters,
            //    // which if present would start with an underscore, for example sort order
            //    // (see https://msdn.microsoft.com/en-us/library/windows/desktop/dd373814(v=vs.85).aspx).
            //    // Therefore, if there is an underscore in the locale name, trim all characters from the underscore onwards.
            //    WCHAR* underscore = wcschr(currentLocale, '_');
            //    if (underscore != null)
            //    {
            //        *underscore = '\0';
            //    }

            //    string localestring =  new String(currentLocale);
            //    // validate if the locale we have is valid
            //    // otherwise we fallback to the default.
            //    if (Language.IsWellFormed(localeString))
            //    {
            //        var languageList =  new Vector<String>();
            //        languageList.Append(localeString);
            //        return languageList;
            //    }
            //}

            //return null;
            yield break;
#else
            yield break;
#endif
        }

        // Resources for the engine use numbers as keys. It's inconvenient, but also difficult to
        // change given that the engine heavily relies on perfect ordering of certain elements.
        // To compromise, we'll declare a map from engine resource key to automation name from the
        // standard project resources.
        static (string, string)[] s_parenEngineKeyResourceMap = { // Sine permutations
                                                                                  ("67", "SineDegrees"),
                                                                                  ("73", "SineRadians"),
                                                                                  ("79", "SineGradians"),
                                                                                  ("70", "InverseSineDegrees"),
                                                                                  ("76", "InverseSineRadians"),
                                                                                  ("82", "InverseSineGradians"),
                                                                                  ("25", "HyperbolicSine"),
                                                                                  ("85", "InverseHyperbolicSine"),

                                                                                  // Cosine permutations
                                                                                  ("68", "CosineDegrees"),
                                                                                  ("74", "CosineRadians"),
                                                                                  ("80", "CosineGradians"),
                                                                                  ("71", "InverseCosineDegrees"),
                                                                                  ("77", "InverseCosineRadians"),
                                                                                  ("83", "InverseCosineGradians"),
                                                                                  ("26", "HyperbolicCosine"),
                                                                                  ("86", "InverseHyperbolicCosine"),

                                                                                  // Tangent permutations
                                                                                  ("69", "TangentDegrees"),
                                                                                  ("75", "TangentRadians"),
                                                                                  ("81", "TangentGradians"),
                                                                                  ("72", "InverseTangentDegrees"),
                                                                                  ("78", "InverseTangentRadians"),
                                                                                  ("84", "InverseTangentGradians"),
                                                                                  ("27", "HyperbolicTangent"),
                                                                                  ("87", "InverseHyperbolicTangent"),

                                                                                  // Miscellaneous Scientific functions
                                                                                  ("94", "Factorial"),
                                                                                  ("35", "DegreeMinuteSecond"),
                                                                                  ("28", "NaturalLog"),
                                                                                  ("91", "Square")
            };

        static (string, string)[] s_noParenEngineKeyResourceMap = { // Programmer mode functions
                                                                                    ("9", "LeftShift"),
                                                                                    ("10", "RightShift"),

                                                                                    // Y Root scientific function
                                                                                    ("16", "YRoot")
        };

        static Dictionary<string, string> GetTokenToReadableNameMap()
        {
            Dictionary<string, string> tokenToReadableNameMap = new Dictionary<string, string>();
            var resProvider = AppResourceProvider.GetInstance();

            string openParen = resProvider.GetCEngineString(s_openParenResourceKey);

            foreach (var keyPair in s_parenEngineKeyResourceMap)
            {
                string engineStr = resProvider.GetCEngineString(keyPair.Item1);
                string automationName = resProvider.GetResourceString(keyPair.Item2);

                if (!tokenToReadableNameMap.ContainsKey(engineStr + openParen))
                {
                    tokenToReadableNameMap.Add(engineStr + openParen, automationName);
                }
            }
            // s_parenEngineKeyResourceMap.clear();

            foreach (var keyPair in s_noParenEngineKeyResourceMap)
            {
                string engineStr = resProvider.GetCEngineString(keyPair.Item1);
                string automationName = resProvider.GetResourceString(keyPair.Item2);

                if (!tokenToReadableNameMap.ContainsKey(engineStr))
                {
                    tokenToReadableNameMap[engineStr] = automationName;
                }
            }
            // s_noParenEngineKeyResourceMap.clear();

            // Also replace hyphens with "minus"
            string minusText = resProvider.GetResourceString("minus");

            if (!tokenToReadableNameMap.ContainsKey("-"))
            {
                tokenToReadableNameMap["-"] = minusText;
            }

            return tokenToReadableNameMap;
        }

        static Dictionary<string, string> s_tokenToReadableNameMap = GetTokenToReadableNameMap();

        static readonly string openParen = AppResourceProvider.GetInstance().GetCEngineString(s_openParenResourceKey);

        public static string GetNarratorReadableToken(string rawToken)
        {
            if(s_tokenToReadableNameMap.TryGetValue(rawToken, out var itr))
            {
                return itr + " " + openParen;
            }
            else
            {
                return rawToken;
            }
        }

        public static string GetNarratorReadableString(string rawString)
        {
            StringBuilder readableString = new StringBuilder();

            string asWstring = rawString;
            foreach (var c in asWstring)
            {
                readableString.Append(GetNarratorReadableToken("" + c));
            }

            return readableString.ToString();
        }
    };

}
