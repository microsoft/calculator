// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// UnitConverter.xaml.h
// Declaration of the UnitConverter class

#pragma once

#include "Views/UnitConverter.g.h"
#include "Common/AlwaysSelectedCollectionView.h"
#include "CalcViewModel/Common/ValidatingConverters.h"
#include "Converters/BooleanToVisibilityConverter.h"
#include "Converters/VisibilityNegationConverter.h"
#include "CalcViewModel/UnitConverterViewModel.h"
#include "Views/StateTriggers/AspectRatioTrigger.h"

namespace CalculatorApp
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class UnitConverter sealed
    {
    public:
        UnitConverter();
        DEPENDENCY_PROPERTY_OWNER(UnitConverter);

        void AnimateConverter();

        property CalculatorApp::ViewModel::UnitConverterViewModel^ Model
        {
            CalculatorApp::ViewModel::UnitConverterViewModel^ get() {
                return static_cast<CalculatorApp::ViewModel::UnitConverterViewModel^>(this->DataContext);
            }
        }

        property Windows::UI::Xaml::FlowDirection LayoutDirection
        {
            Windows::UI::Xaml::FlowDirection get()
            {
                return m_layoutDirection;
            }
        }

        void SetDefaultFocus();

    private:
        void OnValueKeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
        void OnContextRequested(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::Input::ContextRequestedEventArgs^ e);
        void OnContextCanceled(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnCopyMenuItemClicked(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnPasteMenuItemClicked(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnValueSelected(_In_ Platform::Object^ sender);
        void UpdateDropDownState(_In_ Platform::Object^ sender, _In_ Platform::Object^ e);
        void OnLoaded(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
        void CurrencyRefreshButton_Click(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);

        void OnPropertyChanged(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::Data::PropertyChangedEventArgs^ e);
        void OnDataContextChanged(_In_ Windows::UI::Xaml::FrameworkElement^ sender, _In_ Windows::UI::Xaml::DataContextChangedEventArgs^ args);
        void OnIsDisplayVisibleChanged();
        void Units1_IsEnabledChanged(Platform::Object^ sender, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ e);

        void OnNetworkBehaviorChanged();
        void OnNormalNetworkAccess();
        void OnOptInNetworkAccess();
        void OnOfflineNetworkAccess();

        void InitializeOfflineStatusTextBlock();

        void SetNormalCurrencyStatus();
        void SetChargesMayApplyStatus();
        void SetFailedToRefreshStatus();

        void SetCurrencyTimestampFontWeight();

        void StartProgressRingWithDelay();
        void OnDelayTimerTick(Platform::Object^ sender, Platform::Object^ e);
        void HideProgressRing();

    private:
        Windows::UI::Xaml::FlowDirection m_layoutDirection;
        Windows::Foundation::EventRegistrationToken m_propertyChangedToken;
        Windows::UI::Xaml::Controls::MenuFlyout^ m_resultsFlyout;
        
        Platform::String^ m_chargesMayApplyText;
        Platform::String^ m_failedToRefreshText;

        bool m_meteredConnectionOverride;

        Windows::UI::Xaml::DispatcherTimer^ m_delayTimer;

        bool m_isAnimationEnabled;
    };
}
