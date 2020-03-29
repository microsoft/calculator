#include "pch.h"

#include "GraphingSettings.xaml.h"
#include "CalcViewModel\Common\AppResourceProvider.cpp"

using namespace std;
using namespace Graphing;
using namespace GraphControl;

using namespace CalculatorApp;
using namespace CalculatorApp::ViewModel;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::System;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

GraphingSettings::GraphingSettings()
    : m_ViewModel(ref new GraphingSettingsViewModel())
    , m_IsMatchAppTheme(false)
{
    InitializeComponent();
}

void GraphingSettings::SetGrapher(Grapher ^ grapher)
{
    m_ViewModel->SetGrapher(grapher);
}

Style ^ GraphingSettings::SelectTextBoxStyle(bool incorrectRange, bool error)
{
    if (incorrectRange || error)
    {
        return static_cast<::Style ^>(this->Resources->Lookup(L"ErrorTextBoxStyle"));
    }
    else
    {
        return nullptr;
    }
}

void GraphingSettings::GridSettingsTextBox_PreviewKeyDown(Platform::Object ^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e)
{
    if (e->Key == VirtualKey::Enter)
    {
        if (!FocusManager::TryMoveFocusAsync(FocusNavigationDirection::Next))
        {
            FocusManager::TryMoveFocusAsync(FocusNavigationDirection::Previous);
        }
        e->Handled = true;
    }
}

void GraphingSettings::RefreshRanges()
{
    ViewModel->InitRanges();
}

void GraphingSettings::ResetViewButton_Clicked(Object ^ sender, RoutedEventArgs ^ e)
{
    ViewModel->ResetView();
}

String ^ GraphingSettings::GetLineWidthAutomationName(double width)
{
    auto resourceLoader = AppResourceProvider::GetInstance();

    if (width == 1.0)
    {
        return resourceLoader->GetResourceString("SmallLineWidthAutomationName");
    }
    else if(width == 2.0)
    {
        return resourceLoader->GetResourceString("MediumLineWidthAutomationName");
    }
    else if (width == 3.0)
    {
        return resourceLoader->GetResourceString("LargeLineWidthAutomationName");
    }
    else
    {
        return resourceLoader->GetResourceString("ExtraLargeLineWidthAutomationName");
    }
}

void GraphingSettings::SetGraphTheme(bool isMatchAppTheme)
{
    String ^ propertyName = isMatchAppTheme ? L"IsMatchAppTheme" : L"IsAlwaysLightTheme";
    ApplicationDataContainer ^ localSettings = ApplicationData::Current->LocalSettings;
    localSettings->Values->Insert(L"IsGraphThemeMatchApp", isMatchAppTheme);
    GraphThemeSettingChanged(this, isMatchAppTheme);
    TraceLogger::GetInstance()->LogGraphSettingsChanged(GraphSettingsType::Theme, propertyName);
}
