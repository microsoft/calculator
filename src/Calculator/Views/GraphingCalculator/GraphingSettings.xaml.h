//
// MyUserControl.xaml.h
// Declaration of the MyUserControl class
//

#pragma once

#include "CalcViewModel/Common/Utils.h"
#include "CalcViewModel/GraphingCalculator/GraphingSettingsViewModel.h"
#include "Views\GraphingCalculator\GraphingSettings.g.h"
#include <CalcViewModel\GraphingCalculator\GraphingCalculatorViewModel.h>

namespace CalculatorApp
{
    [Windows::Foundation::Metadata::WebHostHidden] public ref class GraphingSettings sealed
    {
    public:
        GraphingSettings();

        PROPERTY_RW(CalculatorApp::ViewModel::GraphingSettingsViewModel ^, ViewModel);

        
        property bool IsMatchAppTheme
        {
            bool get()
            {
                return m_IsMatchAppTheme;
            }
            void set(bool value)
            {
                if (m_IsMatchAppTheme == value)
                {
                    return;
                }

                m_IsMatchAppTheme = value;
                SetGraphTheme(m_IsMatchAppTheme);
            }
        }

        Windows::UI::Xaml::Style ^ SelectTextBoxStyle(bool incorrectRange, bool error);
        void SetGrapher(GraphControl::Grapher ^ grapher);
        void RefreshRanges();

        static Platform::String ^ GetLineWidthAutomationName(double width);

        // Event sends the if the IsMatchAppTheme is selected
        event Windows::Foundation::EventHandler<bool> ^ GraphThemeSettingChanged;
    private:
        void GridSettingsTextBox_PreviewKeyDown(Platform::Object ^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e);
        void ResetViewButton_Clicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void SetGraphTheme(bool isMatchAppTheme);

        bool m_IsMatchAppTheme;
    };
}
