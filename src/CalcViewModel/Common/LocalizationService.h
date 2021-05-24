// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Utils.h"

namespace CalculatorApp::ViewModel
{
    namespace Common
    {
        namespace LocalizationServiceProperties
        {
            static constexpr std::wstring_view DefaultCurrencyCode{ L"USD" };
        }

    public
        enum class LanguageFontType
        {
            UIText,
            UICaption,
        };

    public
        ref class LocalizationService sealed
        {
        public:
            DEPENDENCY_PROPERTY_OWNER(LocalizationService);

            DEPENDENCY_PROPERTY_ATTACHED_WITH_DEFAULT_AND_CALLBACK(LanguageFontType, FontType, LanguageFontType::UIText);
            DEPENDENCY_PROPERTY_ATTACHED_WITH_CALLBACK(double, FontSize);

            static LocalizationService ^ GetInstance();
            Windows::UI::Xaml::FlowDirection GetFlowDirection();
            bool IsRtlLayout();
            bool GetOverrideFontApiValues();
            Platform::String ^ GetLanguage();
            Windows::UI::Xaml::Media::FontFamily ^ GetLanguageFontFamilyForType(LanguageFontType fontType);
            Platform::String ^ GetFontFamilyOverride();
            Windows::UI::Text::FontWeight GetFontWeightOverride();
            double GetFontScaleFactorOverride(LanguageFontType fontType);
            Windows::Globalization::NumberFormatting::DecimalFormatter ^ GetRegionalSettingsAwareDecimalFormatter();
            Windows::Globalization::DateTimeFormatting::DateTimeFormatter ^ GetRegionalSettingsAwareDateTimeFormatter(_In_ Platform::String ^ format);
            Windows::Globalization::DateTimeFormatting::DateTimeFormatter
                ^ GetRegionalSettingsAwareDateTimeFormatter(
                    _In_ Platform::String ^ format,
                    _In_ Platform::String ^ calendarIdentifier,
                    _In_ Platform::String ^ clockIdentifier);
            Windows::Globalization::NumberFormatting::CurrencyFormatter ^ GetRegionalSettingsAwareCurrencyFormatter();

        internal:
            static void OverrideWithLanguage(_In_ const wchar_t* const language);
            void Sort(std::vector<Platform::String ^>& source);

            template <typename T>
            void Sort(std::vector<T>& source, std::function<Platform::String ^ (T)> func)
            {
                const collate<wchar_t>& coll = use_facet<collate<wchar_t>>(m_locale);
                sort(source.begin(), source.end(), [&coll, &func](T obj1, T obj2) {
                    Platform::String ^ str1 = func(obj1);
                    Platform::String ^ str2 = func(obj2);
                    return coll.compare(str1->Begin(), str1->End(), str2->Begin(), str2->End()) < 0;
                });
            }

            static Platform::String ^ GetNarratorReadableToken(Platform::String ^ rawToken);
            static Platform::String ^ GetNarratorReadableString(Platform::String ^ rawString);

        private:
            LocalizationService(_In_ const wchar_t* const overridedLanguage);
            Windows::Globalization::Fonts::LanguageFont ^ GetLanguageFont(LanguageFontType fontType);
            Windows::UI::Text::FontWeight ParseFontWeight(Platform::String ^ fontWeight);

            Windows::Foundation::Collections::IIterable<Platform::String ^> ^ GetLanguageIdentifiers() const;

            // Attached property callbacks
            static void OnFontTypePropertyChanged(Windows::UI::Xaml::DependencyObject ^ target, LanguageFontType oldValue, LanguageFontType newValue);
            static void OnFontWeightPropertyChanged(
                Windows::UI::Xaml::DependencyObject ^ target,
                Windows::UI::Text::FontWeight oldValue,
                Windows::UI::Text::FontWeight newValue);
            static void OnFontSizePropertyChanged(Windows::UI::Xaml::DependencyObject ^ target, double oldValue, double newValue);

            static void UpdateFontFamilyAndSize(Windows::UI::Xaml::DependencyObject ^ target);

            static std::unordered_map<std::wstring, std::wstring> GetTokenToReadableNameMap();

            static LocalizationService ^ s_singletonInstance;

            Windows::Globalization::Fonts::LanguageFontGroup ^ m_fontGroup;
            Platform::String ^ m_language;
            Windows::UI::Xaml::FlowDirection m_flowDirection;
            bool m_overrideFontApiValues;
            Platform::String ^ m_fontFamilyOverride;
            bool m_isLanguageOverrided;
            Windows::UI::Text::FontWeight m_fontWeightOverride;
            double m_uiTextFontScaleFactorOverride;
            double m_uiCaptionFontScaleFactorOverride;
            std::locale m_locale;
        };
    }
}
