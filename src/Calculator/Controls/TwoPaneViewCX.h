// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#pragma once
#include "CalcViewModel/Common/Utils.h"

namespace CalculatorApp::Controls
{
    ref class TwoPaneViewCX;
public
    interface class ITwoPaneViewCX
    {
        event Windows::Foundation::TypedEventHandler<TwoPaneViewCX ^, Object ^> ^ ModeChanged;
    };

    // We can't use the TwoPaneView control from the SDK or from Microsoft.UI.Xaml because of a bug with C++ apps.
    // (see this issue: https://github.com/microsoft/microsoft-ui-xaml/pull/2045)
    // This class is a C++/CX port of the C++/WinRT version of Microsoft.UI.Xaml (commit b3a2e45) which include the patch to fix the crash.
    // This fork adds also 4 new properties Pane1MinLength, Pane2MinLength, Pane1MaxLength, Pane2MaxLength
public
    ref class TwoPaneViewCX sealed : public Windows::UI::Xaml::Controls::Control, [Windows::Foundation::Metadata::Default] ITwoPaneViewCX
    {
        enum class ViewMode
        {
            Pane1Only,
            Pane2Only,
            LeftRight,
            RightLeft,
            TopBottom,
            BottomTop,
            None
        };

        struct DisplayRegionHelperInfo
        {
            Microsoft::UI::Xaml::Controls::TwoPaneViewMode Mode = Microsoft::UI::Xaml::Controls::TwoPaneViewMode::SinglePane;
            Platform::Array<Windows::Foundation::Rect> ^ Regions;
        };

    public:
        DEPENDENCY_PROPERTY_OWNER(TwoPaneViewCX);
        DEPENDENCY_PROPERTY(Windows::UI::Xaml::UIElement ^, Pane1);
        DEPENDENCY_PROPERTY(Windows::UI::Xaml::UIElement ^, Pane2);
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(
            Windows::UI::Xaml::GridLength,
            Pane1Length,
            Windows::UI::Xaml::GridLengthHelper::FromValueAndType(1, Windows::UI::Xaml::GridUnitType::Auto));
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(
            Windows::UI::Xaml::GridLength,
            Pane2Length,
            Windows::UI::Xaml::GridLengthHelper::FromValueAndType(1, Windows::UI::Xaml::GridUnitType::Star));
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(double, Pane1MinLength, 0.0);
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(double, Pane2MinLength, 0.0);
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(double, Pane1MaxLength, std::numeric_limits<double>::max());
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(double, Pane2MaxLength, std::numeric_limits<double>::max());

        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(Microsoft::UI::Xaml::Controls::TwoPaneViewPriority,
            PanePriority,
            Microsoft::UI::Xaml::Controls::TwoPaneViewPriority::Pane1);
        DEPENDENCY_PROPERTY(Microsoft::UI::Xaml::Controls::TwoPaneViewMode, Mode);
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(
            Microsoft::UI::Xaml::Controls::TwoPaneViewWideModeConfiguration,
            WideModeConfiguration,
            Microsoft::UI::Xaml::Controls::TwoPaneViewWideModeConfiguration::LeftRight);
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(
            Microsoft::UI::Xaml::Controls::TwoPaneViewTallModeConfiguration,
            TallModeConfiguration,
            Microsoft::UI::Xaml::Controls::TwoPaneViewTallModeConfiguration::TopBottom);
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(double, MinWideModeWidth, 641);
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(double, MinTallModeHeight, 641);

        TwoPaneViewCX();
        virtual ~TwoPaneViewCX();
        void OnApplyTemplate() override;
        virtual event Windows::Foundation::TypedEventHandler<TwoPaneViewCX ^, Object ^> ^ ModeChanged;

    private:
        void UpdateRowsColumns(ViewMode newMode, DisplayRegionHelperInfo info, Windows::Foundation::Rect rcControl);
        void UpdateMode();
        bool IsInMultipleRegions(DisplayRegionHelperInfo info, Windows::Foundation::Rect rcControl);
        void SetScrollViewerProperties(Platform::String ^ scrollViewerName);
        Windows::Foundation::Rect GetControlRect();
        DisplayRegionHelperInfo GetDisplayRegionHelperInfo();

        void OnSizeChanged(Platform::Object ^ sender, Windows::UI::Xaml::SizeChangedEventArgs ^ e);
        void OnWindowSizeChanged(Platform::Object ^ sender, Windows::UI::Core::WindowSizeChangedEventArgs ^ e);
        void OnPane1LengthPropertyChanged(Windows::UI::Xaml::GridLength oldValue, Windows::UI::Xaml::GridLength newValue);
        void OnPane2LengthPropertyChanged(Windows::UI::Xaml::GridLength oldValue, Windows::UI::Xaml::GridLength newValue);
        void OnPane1MinLengthPropertyChanged(double oldValue, double newValue);
        void OnPane2MinLengthPropertyChanged(double oldValue, double newValue);
        void OnPane1MaxLengthPropertyChanged(double oldValue, double newValue);
        void OnPane2MaxLengthPropertyChanged(double oldValue, double newValue);
        void
        OnPanePriorityPropertyChanged(Microsoft::UI::Xaml::Controls::TwoPaneViewPriority oldValue, Microsoft::UI::Xaml::Controls::TwoPaneViewPriority newValue);
        void OnMinTallModeHeightPropertyChanged(double oldValue, double newValue);
        void OnMinWideModeWidthPropertyChanged(double oldValue, double newValue);
        void OnWideModeConfigurationPropertyChanged(
            Microsoft::UI::Xaml::Controls::TwoPaneViewWideModeConfiguration oldValue,
            Microsoft::UI::Xaml::Controls::TwoPaneViewWideModeConfiguration newValue);
        void OnTallModeConfigurationPropertyChanged(
            Microsoft::UI::Xaml::Controls::TwoPaneViewTallModeConfiguration oldValue,
            Microsoft::UI::Xaml::Controls::TwoPaneViewTallModeConfiguration newValue);
        void OnScrollViewerLoaded(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

    private:
        Windows::UI::Xaml::Controls::ColumnDefinition ^ m_columnLeft;
        Windows::UI::Xaml::Controls::ColumnDefinition ^ m_columnMiddle;
        Windows::UI::Xaml::Controls::ColumnDefinition ^ m_columnRight;
        Windows::UI::Xaml::Controls::RowDefinition ^ m_rowTop;
        Windows::UI::Xaml::Controls::RowDefinition ^ m_rowMiddle;
        Windows::UI::Xaml::Controls::RowDefinition ^ m_rowBottom;
        Windows::Foundation::EventRegistrationToken m_windowSizeChangedToken;
        ViewMode m_currentMode{ ViewMode::None };
        bool m_loaded{ false };
    };
}
