// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Utils.h"

namespace CalculatorApp { namespace Common
{
    namespace LocalizationServiceProperties
    {
        static constexpr std::wstring_view DefaultCurrencyCode{ L"USD" };
    }

    public enum class LanguageFontType
    {
        UIText,
        UICaption,
    };

    public ref class LocalizationService sealed
    {
    public:

        DEPENDENCY_PROPERTY_OWNER(LocalizationService);

        DEPENDENCY_PROPERTY_ATTACHED_WITH_DEFAULT_AND_CALLBACK(LanguageFontType, FontType, LanguageFontType::UIText);
        DEPENDENCY_PROPERTY_ATTACHED_WITH_CALLBACK(double, FontSize);

    internal:
        static LocalizationService^ GetInstance();

        Windows::UI::Xaml::FlowDirection GetFlowDirection();
        bool IsRtlLayout();
        bool GetOverrideFontApiValues();
        Platform::String^ GetLanguage();
        Windows::UI::Xaml::Media::FontFamily^ GetLanguageFontFamilyForType(LanguageFontType fontType);
        Platform::String^ GetFontFamilyOverride();
        Windows::UI::Text::FontWeight GetFontWeightOverride();
        double GetFontScaleFactorOverride(LanguageFontType fontType);

        static Windows::Globalization::NumberFormatting::DecimalFormatter^ GetRegionalSettingsAwareDecimalFormatter();
        static Windows::Globalization::DateTimeFormatting::DateTimeFormatter^ GetRegionalSettingsAwareDateTimeFormatter(_In_ Platform::String^ format);
        static Windows::Globalization::DateTimeFormatting::DateTimeFormatter^ GetRegionalSettingsAwareDateTimeFormatter(
            _In_ Platform::String^ format,
            _In_ Platform::String^ calendarIdentifier,
            _In_ Platform::String^ clockIdentifier);

        static Windows::Globalization::NumberFormatting::CurrencyFormatter^ GetRegionalSettingsAwareCurrencyFormatter();

        static Platform::String^ GetNarratorReadableToken(Platform::String^ rawToken);
        static Platform::String^ GetNarratorReadableString(Platform::String^ rawString);

    private:
        Windows::Globalization::Fonts::LanguageFont^ GetLanguageFont(LanguageFontType fontType);
        Windows::UI::Text::FontWeight ParseFontWeight(Platform::String^ fontWeight);

        static Windows::Foundation::Collections::IIterable<Platform::String^>^ GetLanguageIdentifiers();

        // Attached property callbacks
        static void OnFontTypePropertyChanged(Windows::UI::Xaml::DependencyObject^ target, LanguageFontType oldValue, LanguageFontType newValue);
        static void OnFontWeightPropertyChanged(Windows::UI::Xaml::DependencyObject^ target, Windows::UI::Text::FontWeight oldValue, Windows::UI::Text::FontWeight newValue);
        static void OnFontSizePropertyChanged(Windows::UI::Xaml::DependencyObject^ target, double oldValue, double newValue);

        static void UpdateFontFamilyAndSize(Windows::UI::Xaml::DependencyObject^ target);

        static std::unordered_map<std::wstring, std::wstring> GetTokenToReadableNameMap();

    private:
        LocalizationService();

        static LocalizationService^ s_singletonInstance;

        Windows::Globalization::Fonts::LanguageFontGroup^ m_fontGroup;
        Platform::String^ m_language;
        Windows::UI::Xaml::FlowDirection m_flowDirection;
        bool m_overrideFontApiValues;
        Platform::String^ m_fontFamilyOverride;
        Windows::UI::Text::FontWeight m_fontWeightOverride;
        double m_uiTextFontScaleFactorOverride;
        double m_uiCaptionFontScaleFactorOverride;
    };

}}

