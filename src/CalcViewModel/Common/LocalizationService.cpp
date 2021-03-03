// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "LocalizationService.h"
#include "LocalizationSettings.h"
#include "AppResourceProvider.h"

using namespace CalculatorApp::Common;
using namespace CalculatorApp::Common::LocalizationServiceProperties;
using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::ApplicationModel::Resources::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::Globalization::Fonts;
using namespace Windows::Globalization::NumberFormatting;
using namespace Windows::System::UserProfile;
using namespace Windows::UI::Text;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Media;

DEPENDENCY_PROPERTY_INITIALIZATION(LocalizationService, FontType);
DEPENDENCY_PROPERTY_INITIALIZATION(LocalizationService, FontSize);

static reader_writer_lock s_locServiceInstanceLock;

LocalizationService ^ LocalizationService::s_singletonInstance = nullptr;

// Resources for the engine use numbers as keys. It's inconvenient, but also difficult to
// change given that the engine heavily relies on perfect ordering of certain elements.
// The key for open parenthesis, '(', is "48".
static constexpr auto s_openParenResourceKey = L"48";

LocalizationService ^ LocalizationService::GetInstance()
{
    if (s_singletonInstance == nullptr)
    {
        // Writer lock for the static maps
        reader_writer_lock::scoped_lock lock(s_locServiceInstanceLock);

        if (s_singletonInstance == nullptr)
        {
            s_singletonInstance = ref new LocalizationService(nullptr);
        }
    }
    return s_singletonInstance;
}

/// <summary>
/// Replace (or create) the single instance of this singleton class by one with the language passed as parameter
/// </summary>
/// <param name="language">RFC-5646 identifier of the language to use</param>
/// <remarks>
/// Should only be used for test purpose
/// </remarks>
void LocalizationService::OverrideWithLanguage(_In_ const wchar_t* const language)
{
    s_singletonInstance = ref new LocalizationService(language);
}

/// <summary>
/// Constructor
/// </summary>
/// <param name="overridedLanguage">RFC-5646 identifier of the language to use, if null, will use the current language of the system</param>
LocalizationService::LocalizationService(_In_ const wchar_t * const overridedLanguage)
{
    m_isLanguageOverrided = overridedLanguage != nullptr;
    m_language = m_isLanguageOverrided ? ref new Platform::String(overridedLanguage) : ApplicationLanguages::Languages->GetAt(0);
    m_flowDirection = ResourceContext::GetForViewIndependentUse()->QualifierValues->Lookup(L"LayoutDirection")
        != L"LTR" ? FlowDirection::RightToLeft : FlowDirection::LeftToRight;
    wstring localeName = wstring(m_language->Data());
    localeName += L".UTF8";

    try
    {
        // Convert wstring to string for locale
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &localeName[0], (int)localeName.size(), NULL, 0, NULL, NULL);
        string localeNameStr(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &localeName[0], (int)localeName.size(), &localeNameStr[0], size_needed, NULL, NULL);

        m_locale = locale(localeNameStr.data());
    }
    catch (...)
    {
        m_locale = locale("");
    }
    auto resourceLoader = AppResourceProvider::GetInstance();
    m_fontFamilyOverride = resourceLoader->GetResourceString(L"LocalizedFontFamilyOverride");

    String ^ reserved = L"RESERVED_FOR_FONTLOC";

    m_overrideFontApiValues = ((m_fontFamilyOverride != nullptr) && (m_fontFamilyOverride != reserved));
    if (m_overrideFontApiValues)
    {
        String ^ localizedUICaptionFontSizeFactorOverride = resourceLoader->GetResourceString(L"LocalizedUICaptionFontSizeFactorOverride");
        String ^ localizedUITextFontSizeFactorOverride = resourceLoader->GetResourceString(L"LocalizedUITextFontSizeFactorOverride");
        String ^ localizedFontWeightOverride = resourceLoader->GetResourceString(L"LocalizedFontWeightOverride");

        // If any of the font overrides are modified then all of them need to be modified
        assert(localizedFontWeightOverride != reserved);
        assert(localizedUITextFontSizeFactorOverride != reserved);
        assert(localizedUICaptionFontSizeFactorOverride != reserved);

        m_fontWeightOverride = ParseFontWeight(localizedFontWeightOverride);
        m_uiTextFontScaleFactorOverride = _wtof(localizedUITextFontSizeFactorOverride->Data());
        m_uiCaptionFontScaleFactorOverride = _wtof(localizedUICaptionFontSizeFactorOverride->Data());
    }

    m_fontGroup = ref new LanguageFontGroup(m_language);
}

FontWeight LocalizationService::ParseFontWeight(String ^ fontWeight)
{
    wstring weight = fontWeight->Data();
    transform(weight.begin(), weight.end(), weight.begin(), towlower);
    fontWeight = ref new String(weight.c_str());

    if (fontWeight == "black")
    {
        return FontWeights::Black;
    }
    else if (fontWeight == "bold")
    {
        return FontWeights::Bold;
    }
    else if (fontWeight == "extrablack")
    {
        return FontWeights::ExtraBlack;
    }
    else if (fontWeight == "extrabold")
    {
        return FontWeights::ExtraBold;
    }
    else if (fontWeight == "extralight")
    {
        return FontWeights::ExtraLight;
    }
    else if (fontWeight == "light")
    {
        return FontWeights::Light;
    }
    else if (fontWeight == "medium")
    {
        return FontWeights::Medium;
    }
    else if (fontWeight == "normal")
    {
        return FontWeights::Normal;
    }
    else if (fontWeight == "semibold")
    {
        return FontWeights::SemiBold;
    }
    else if (fontWeight == "semilight")
    {
        return FontWeights::SemiLight;
    }
    else if (fontWeight == "thin")
    {
        return FontWeights::Thin;
    }
    else
    {
        throw invalid_argument("Invalid argument: fontWeight");
    }
}

FlowDirection LocalizationService::GetFlowDirection()
{
    return m_flowDirection;
}

bool LocalizationService::IsRtlLayout()
{
    return m_flowDirection == FlowDirection::RightToLeft;
}

String ^ LocalizationService::GetLanguage()
{
    return m_language;
}

bool LocalizationService::GetOverrideFontApiValues()
{
    return m_overrideFontApiValues;
}

FontFamily ^ LocalizationService::GetLanguageFontFamilyForType(LanguageFontType fontType)
{
    if (m_overrideFontApiValues)
    {
        return ref new FontFamily(m_fontFamilyOverride);
    }
    else
    {
        return ref new FontFamily(GetLanguageFont(fontType)->FontFamily);
    }
}

LanguageFont ^ LocalizationService::GetLanguageFont(LanguageFontType fontType)
{
    assert(!m_overrideFontApiValues);
    assert(m_fontGroup);

    switch (fontType)
    {
    case LanguageFontType::UIText:
        return m_fontGroup->UITextFont;
    case LanguageFontType::UICaption:
        return m_fontGroup->UICaptionFont;
    default:
        throw std::invalid_argument("fontType");
    }
}

String ^ LocalizationService::GetFontFamilyOverride()
{
    assert(m_overrideFontApiValues);
    return m_fontFamilyOverride;
}

FontWeight LocalizationService::GetFontWeightOverride()
{
    assert(m_overrideFontApiValues);
    return m_fontWeightOverride;
}

double LocalizationService::GetFontScaleFactorOverride(LanguageFontType fontType)
{
    assert(m_overrideFontApiValues);

    switch (fontType)
    {
    case LanguageFontType::UIText:
        return m_uiTextFontScaleFactorOverride;
    case LanguageFontType::UICaption:
        return m_uiCaptionFontScaleFactorOverride;
    default:
        throw invalid_argument("Invalid argument: fontType");
    }
}

void LocalizationService::OnFontTypePropertyChanged(DependencyObject ^ target, LanguageFontType /*oldValue*/, LanguageFontType /*newValue*/)
{
    UpdateFontFamilyAndSize(target);
}

void LocalizationService::OnFontWeightPropertyChanged(DependencyObject ^ target, FontWeight /*oldValue*/, FontWeight /*newValue*/)
{
    UpdateFontFamilyAndSize(target);
}

void LocalizationService::OnFontSizePropertyChanged(DependencyObject ^ target, double /*oldValue*/, double /*newValue*/)
{
    UpdateFontFamilyAndSize(target);
}

void LocalizationService::UpdateFontFamilyAndSize(DependencyObject ^ target)
{
    FontFamily ^ fontFamily;
    FontWeight fontWeight;
    bool fOverrideFontWeight = false;
    double scaleFactor;

    auto service = LocalizationService::GetInstance();
    auto fontType = LocalizationService::GetFontType(target);

    if (service->GetOverrideFontApiValues())
    {
        fontFamily = ref new FontFamily(service->GetFontFamilyOverride());
        scaleFactor = service->GetFontScaleFactorOverride(fontType) / 100.0;
        fontWeight = service->GetFontWeightOverride();
        fOverrideFontWeight = true;
    }
    else
    {
        auto languageFont = service->GetLanguageFont(fontType);
        fontFamily = ref new FontFamily(languageFont->FontFamily);
        scaleFactor = languageFont->ScaleFactor / 100.0;
    }

    double sizeToUse = LocalizationService::GetFontSize(target) * scaleFactor;

    auto control = dynamic_cast<Control ^>(target);
    if (control)
    {
        control->FontFamily = fontFamily;
        if (fOverrideFontWeight)
        {
            control->FontWeight = fontWeight;
        }
        if (sizeToUse != 0.0)
        {
            control->FontSize = sizeToUse;
        }
        else
        {
            control->ClearValue(Control::FontSizeProperty);
        }
    }
    else
    {
        auto textBlock = dynamic_cast<TextBlock ^>(target);
        if (textBlock)
        {
            textBlock->FontFamily = fontFamily;
            if (fOverrideFontWeight)
            {
                textBlock->FontWeight = fontWeight;
            }
            if (sizeToUse != 0.0)
            {
                textBlock->FontSize = sizeToUse;
            }
            else
            {
                textBlock->ClearValue(TextBlock::FontSizeProperty);
            }
        }
        else
        {
            RichTextBlock ^ richTextBlock = dynamic_cast<RichTextBlock ^>(target);
            if (richTextBlock)
            {
                richTextBlock->FontFamily = fontFamily;
                if (fOverrideFontWeight)
                {
                    richTextBlock->FontWeight = fontWeight;
                }
                if (sizeToUse != 0.0)
                {
                    richTextBlock->FontSize = sizeToUse;
                }
                else
                {
                    richTextBlock->ClearValue(RichTextBlock::FontSizeProperty);
                }
            }
            else
            {
                TextElement ^ textElement = dynamic_cast<TextElement ^>(target);
                if (textElement)
                {
                    textElement->FontFamily = fontFamily;
                    if (fOverrideFontWeight)
                    {
                        textElement->FontWeight = fontWeight;
                    }
                    if (sizeToUse != 0.0)
                    {
                        textElement->FontSize = sizeToUse;
                    }
                    else
                    {
                        textElement->ClearValue(TextElement::FontSizeProperty);
                    }
                }
            }
        }
    }
}

// If successful, returns a formatter that respects the user's regional format settings,
// as configured by running intl.cpl.
DecimalFormatter ^ LocalizationService::GetRegionalSettingsAwareDecimalFormatter() const
{
    IIterable<String ^> ^ languageIdentifiers = LocalizationService::GetLanguageIdentifiers();
    if (languageIdentifiers != nullptr)
    {
        return ref new DecimalFormatter(languageIdentifiers, GlobalizationPreferences::HomeGeographicRegion);
    }

    return ref new DecimalFormatter();
}

// If successful, returns a formatter that respects the user's regional format settings,
// as configured by running intl.cpl.
//
// This helper function creates a DateTimeFormatter with a TwentyFour hour clock
DateTimeFormatter ^ LocalizationService::GetRegionalSettingsAwareDateTimeFormatter(_In_ String ^ format) const
{
    IIterable<String ^> ^ languageIdentifiers = LocalizationService::GetLanguageIdentifiers();
    if (languageIdentifiers == nullptr)
    {
        languageIdentifiers = ApplicationLanguages::Languages;
    }

    return ref new DateTimeFormatter(format, languageIdentifiers);
}

// If successful, returns a formatter that respects the user's regional format settings,
// as configured by running intl.cpl.
DateTimeFormatter ^ LocalizationService::GetRegionalSettingsAwareDateTimeFormatter(_In_ String ^ format, _In_ String ^ calendarIdentifier, _In_ String ^ clockIdentifier) const
{
    IIterable<String ^> ^ languageIdentifiers = LocalizationService::GetLanguageIdentifiers();
    if (languageIdentifiers == nullptr)
    {
        languageIdentifiers = ApplicationLanguages::Languages;
    }

    return ref new DateTimeFormatter(format, languageIdentifiers, GlobalizationPreferences::HomeGeographicRegion, calendarIdentifier, clockIdentifier);
}

CurrencyFormatter ^ LocalizationService::GetRegionalSettingsAwareCurrencyFormatter() const
{
    String ^ userCurrency =
        (GlobalizationPreferences::Currencies->Size > 0) ? GlobalizationPreferences::Currencies->GetAt(0) : StringReference(DefaultCurrencyCode.data());

    IIterable<String ^> ^ languageIdentifiers = GetLanguageIdentifiers();
    if (languageIdentifiers == nullptr)
    {
        languageIdentifiers = ApplicationLanguages::Languages;
    }

    auto currencyFormatter = ref new CurrencyFormatter(userCurrency, languageIdentifiers, GlobalizationPreferences::HomeGeographicRegion);

    int fractionDigits = LocalizationSettings::GetInstance().GetCurrencyTrailingDigits();
    currencyFormatter->FractionDigits = fractionDigits;

    return currencyFormatter;
}

IIterable<String ^> ^ LocalizationService::GetLanguageIdentifiers() const
{
    WCHAR currentLocale[LOCALE_NAME_MAX_LENGTH] = {};
    int result = GetUserDefaultLocaleName(currentLocale, LOCALE_NAME_MAX_LENGTH);

    if (m_isLanguageOverrided)
    {
        auto overridedLanguageList = ref new Vector<String ^>();
        overridedLanguageList->Append(m_language);
        return overridedLanguageList;
    }

    if (result != 0)
    {
        // GetUserDefaultLocaleName may return an invalid bcp47 language tag with trailing non-BCP47 friendly characters,
        // which if present would start with an underscore, for example sort order
        // (see https://msdn.microsoft.com/en-us/library/windows/desktop/dd373814(v=vs.85).aspx).
        // Therefore, if there is an underscore in the locale name, trim all characters from the underscore onwards.
        WCHAR* underscore = wcschr(currentLocale, L'_');
        if (underscore != nullptr)
        {
            *underscore = L'\0';
        }

        String ^ localeString = ref new String(currentLocale);
        // validate if the locale we have is valid
        // otherwise we fallback to the default.
        if (Language::IsWellFormed(localeString))
        {
            auto languageList = ref new Vector<String ^>();
            languageList->Append(localeString);
            return languageList;
        }
    }

    return nullptr;
}

unordered_map<wstring, wstring> LocalizationService::GetTokenToReadableNameMap()
{
    // Resources for the engine use numbers as keys. It's inconvenient, but also difficult to
    // change given that the engine heavily relies on perfect ordering of certain elements.
    // To compromise, we'll declare a map from engine resource key to automation name from the
    // standard project resources.
    static vector<pair<wstring, wstring>> s_parenEngineKeyResourceMap = { // Sine permutations
                                                                          make_pair<wstring, wstring>(L"67", L"SineDegrees"),
                                                                          make_pair<wstring, wstring>(L"73", L"SineRadians"),
                                                                          make_pair<wstring, wstring>(L"79", L"SineGradians"),
                                                                          make_pair<wstring, wstring>(L"70", L"InverseSineDegrees"),
                                                                          make_pair<wstring, wstring>(L"76", L"InverseSineRadians"),
                                                                          make_pair<wstring, wstring>(L"82", L"InverseSineGradians"),
                                                                          make_pair<wstring, wstring>(L"25", L"HyperbolicSine"),
                                                                          make_pair<wstring, wstring>(L"85", L"InverseHyperbolicSine"),

                                                                          // Cosine permutations
                                                                          make_pair<wstring, wstring>(L"68", L"CosineDegrees"),
                                                                          make_pair<wstring, wstring>(L"74", L"CosineRadians"),
                                                                          make_pair<wstring, wstring>(L"80", L"CosineGradians"),
                                                                          make_pair<wstring, wstring>(L"71", L"InverseCosineDegrees"),
                                                                          make_pair<wstring, wstring>(L"77", L"InverseCosineRadians"),
                                                                          make_pair<wstring, wstring>(L"83", L"InverseCosineGradians"),
                                                                          make_pair<wstring, wstring>(L"26", L"HyperbolicCosine"),
                                                                          make_pair<wstring, wstring>(L"86", L"InverseHyperbolicCosine"),

                                                                          // Tangent permutations
                                                                          make_pair<wstring, wstring>(L"69", L"TangentDegrees"),
                                                                          make_pair<wstring, wstring>(L"75", L"TangentRadians"),
                                                                          make_pair<wstring, wstring>(L"81", L"TangentGradians"),
                                                                          make_pair<wstring, wstring>(L"72", L"InverseTangentDegrees"),
                                                                          make_pair<wstring, wstring>(L"78", L"InverseTangentRadians"),
                                                                          make_pair<wstring, wstring>(L"84", L"InverseTangentGradians"),
                                                                          make_pair<wstring, wstring>(L"27", L"HyperbolicTangent"),
                                                                          make_pair<wstring, wstring>(L"87", L"InverseHyperbolicTangent"),

                                                                          // Secant permutations
                                                                          make_pair<wstring, wstring>(L"SecDeg", L"SecantDegrees"),
                                                                          make_pair<wstring, wstring>(L"SecRad", L"SecantRadians"),
                                                                          make_pair<wstring, wstring>(L"SecGrad", L"SecantGradians"),
                                                                          make_pair<wstring, wstring>(L"InverseSecDeg", L"InverseSecantDegrees"),
                                                                          make_pair<wstring, wstring>(L"InverseSecRad", L"InverseSecantRadians"),
                                                                          make_pair<wstring, wstring>(L"InverseSecGrad", L"InverseSecantGradians"),
                                                                          make_pair<wstring, wstring>(L"Sech", L"HyperbolicSecant"),
                                                                          make_pair<wstring, wstring>(L"InverseSech", L"InverseHyperbolicSecant"),

                                                                          // Cosecant permutations
                                                                          make_pair<wstring, wstring>(L"CscDeg", L"CosecantDegrees"),
                                                                          make_pair<wstring, wstring>(L"CscRad", L"CosecantRadians"),
                                                                          make_pair<wstring, wstring>(L"CscGrad", L"CosecantGradians"),
                                                                          make_pair<wstring, wstring>(L"InverseCscDeg", L"InverseCosecantDegrees"),
                                                                          make_pair<wstring, wstring>(L"InverseCscRad", L"InverseCosecantRadians"),
                                                                          make_pair<wstring, wstring>(L"InverseCscGrad", L"InverseCosecantGradians"),
                                                                          make_pair<wstring, wstring>(L"Csch", L"HyperbolicCosecant"),
                                                                          make_pair<wstring, wstring>(L"InverseCsch", L"InverseHyperbolicCosecant"),

                                                                          // Cotangent permutations
                                                                          make_pair<wstring, wstring>(L"CotDeg", L"CotangentDegrees"),
                                                                          make_pair<wstring, wstring>(L"CotRad", L"CotangentRadians"),
                                                                          make_pair<wstring, wstring>(L"CotGrad", L"CotangentGradians"),
                                                                          make_pair<wstring, wstring>(L"InverseCotDeg", L"InverseCotangentDegrees"),
                                                                          make_pair<wstring, wstring>(L"InverseCotRad", L"InverseCotangentRadians"),
                                                                          make_pair<wstring, wstring>(L"InverseCotGrad", L"InverseCotangentGradians"),
                                                                          make_pair<wstring, wstring>(L"Coth", L"HyperbolicCotangent"),
                                                                          make_pair<wstring, wstring>(L"InverseCoth", L"InverseHyperbolicCotangent"),

                                                                          // Miscellaneous Scientific functions
                                                                          make_pair<wstring, wstring>(L"94", L"Factorial"),
                                                                          make_pair<wstring, wstring>(L"35", L"DegreeMinuteSecond"),
                                                                          make_pair<wstring, wstring>(L"28", L"NaturalLog"),
                                                                          make_pair<wstring, wstring>(L"91", L"Square"),
                                                                          make_pair<wstring, wstring>(L"CubeRoot", L"CubeRoot"),
                                                                          make_pair<wstring, wstring>(L"Abs", L"AbsoluteValue")
    };

    static vector<pair<wstring, wstring>> s_noParenEngineKeyResourceMap = { // Programmer mode functions
                                                                            make_pair<wstring, wstring>(L"9", L"LeftShift"),
                                                                            make_pair<wstring, wstring>(L"10", L"RightShift"),
                                                                            make_pair<wstring, wstring>(L"LogBaseY", L"Logy"),

                                                                            // Y Root scientific function
                                                                            make_pair<wstring, wstring>(L"16", L"YRoot")
    };

    unordered_map<wstring, wstring> tokenToReadableNameMap{};
    auto resProvider = AppResourceProvider::GetInstance();

    static const wstring openParen = resProvider->GetCEngineString(StringReference(s_openParenResourceKey))->Data();

    for (const auto& keyPair : s_parenEngineKeyResourceMap)
    {
        wstring engineStr = resProvider->GetCEngineString(StringReference(keyPair.first.c_str()))->Data();
        wstring automationName = resProvider->GetResourceString(StringReference(keyPair.second.c_str()))->Data();

        tokenToReadableNameMap.emplace(engineStr + openParen, automationName);
    }
    s_parenEngineKeyResourceMap.clear();

    for (const auto& keyPair : s_noParenEngineKeyResourceMap)
    {
        wstring engineStr = resProvider->GetCEngineString(StringReference(keyPair.first.c_str()))->Data();
        wstring automationName = resProvider->GetResourceString(StringReference(keyPair.second.c_str()))->Data();

        tokenToReadableNameMap.emplace(engineStr, automationName);
    }
    s_noParenEngineKeyResourceMap.clear();

    // Also replace hyphens with "minus"
    wstring minusText = resProvider->GetResourceString(L"minus")->Data();
    tokenToReadableNameMap.emplace(L"-", minusText);

    return tokenToReadableNameMap;
}

String ^ LocalizationService::GetNarratorReadableToken(String ^ rawToken)
{
    static unordered_map<wstring, wstring> s_tokenToReadableNameMap = GetTokenToReadableNameMap();

    auto itr = s_tokenToReadableNameMap.find(rawToken->Data());
    if (itr == s_tokenToReadableNameMap.end())
    {
        return rawToken;
    }
    else
    {
        static const String ^ openParen = AppResourceProvider::GetInstance()->GetCEngineString(StringReference(s_openParenResourceKey));
        return ref new String(itr->second.c_str()) + L" " + openParen;
    }
}

String ^ LocalizationService::GetNarratorReadableString(String ^ rawString)
{
    wstring readableString{};

    wstring asWstring = rawString->Data();
    for (const auto& c : asWstring)
    {
        readableString += LocalizationService::GetNarratorReadableToken(ref new String(&c, 1))->Data();
    }

    return ref new String(readableString.c_str());
}

void LocalizationService::Sort(std::vector<Platform::String ^>& source)
{
    const collate<wchar_t>& coll = use_facet<collate<wchar_t>>(m_locale);
    sort(source.begin(), source.end(), [&coll](Platform::String ^ str1, Platform::String ^ str2) {
        return coll.compare(str1->Begin(), str1->End(), str2->Begin(), str2->End()) < 0;
    });
}
