// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"
#include "TwoPaneViewCX.h"
#include "Utils/VisualTree.h"
#include <math.h>

using namespace std;
using namespace Platform;
using namespace CalculatorApp::Controls;
using namespace Calculator::Utils;
using namespace Windows::Foundation;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using Windows::Foundation::Metadata::ApiInformation;

StringReference c_pane1ScrollViewerName(L"PART_Pane1ScrollViewer");
StringReference c_pane2ScrollViewerName(L"PART_Pane2ScrollViewer");
StringReference c_columnLeftName(L"PART_ColumnLeft");
StringReference c_columnMiddleName(L"PART_ColumnMiddle");
StringReference c_columnRightName(L"PART_ColumnRight");
StringReference c_rowTopName(L"PART_RowTop");
StringReference c_rowMiddleName(L"PART_RowMiddle");
StringReference c_rowBottomName(L"PART_RowBottom");

DEPENDENCY_PROPERTY_INITIALIZATION(TwoPaneViewCX, Pane1);
DEPENDENCY_PROPERTY_INITIALIZATION(TwoPaneViewCX, Pane2);
DEPENDENCY_PROPERTY_INITIALIZATION(TwoPaneViewCX, Pane1Length);
DEPENDENCY_PROPERTY_INITIALIZATION(TwoPaneViewCX, Pane2Length);
DEPENDENCY_PROPERTY_INITIALIZATION(TwoPaneViewCX, Pane1MinLength);
DEPENDENCY_PROPERTY_INITIALIZATION(TwoPaneViewCX, Pane2MinLength);
DEPENDENCY_PROPERTY_INITIALIZATION(TwoPaneViewCX, Pane1MaxLength);
DEPENDENCY_PROPERTY_INITIALIZATION(TwoPaneViewCX, Pane2MaxLength);
DEPENDENCY_PROPERTY_INITIALIZATION(TwoPaneViewCX, PanePriority);
DEPENDENCY_PROPERTY_INITIALIZATION(TwoPaneViewCX, Mode);
DEPENDENCY_PROPERTY_INITIALIZATION(TwoPaneViewCX, WideModeConfiguration);
DEPENDENCY_PROPERTY_INITIALIZATION(TwoPaneViewCX, TallModeConfiguration);
DEPENDENCY_PROPERTY_INITIALIZATION(TwoPaneViewCX, MinWideModeWidth);
DEPENDENCY_PROPERTY_INITIALIZATION(TwoPaneViewCX, MinTallModeHeight);

TwoPaneViewCX::TwoPaneViewCX()
{
    this->DefaultStyleKey = TwoPaneViewCX::typeid->FullName;
    this->SizeChanged += ref new Windows::UI::Xaml::SizeChangedEventHandler(this, &TwoPaneViewCX::OnSizeChanged);
    m_windowSizeChangedToken = Window::Current->SizeChanged +=
        ref new Windows::UI::Xaml::WindowSizeChangedEventHandler(this, &TwoPaneViewCX::OnWindowSizeChanged);
}

TwoPaneViewCX::~TwoPaneViewCX()
{
    if (Window::Current != nullptr)
    {
        Window::Current->SizeChanged -= m_windowSizeChangedToken;
    }
}

void TwoPaneViewCX::OnApplyTemplate()
{
    m_loaded = true;
    this->SetScrollViewerProperties(c_pane1ScrollViewerName);
    this->SetScrollViewerProperties(c_pane2ScrollViewerName);

    if (auto column = dynamic_cast<ColumnDefinition ^>(this->GetTemplateChild(c_columnLeftName)))
    {
        m_columnLeft = column;
    }
    if (auto column = dynamic_cast<ColumnDefinition ^>(this->GetTemplateChild(c_columnMiddleName)))
    {
        m_columnMiddle = column;
    }
    if (auto column = dynamic_cast<ColumnDefinition ^>(this->GetTemplateChild(c_columnRightName)))
    {
        m_columnRight = column;
    }
    if (auto row = dynamic_cast<RowDefinition ^>(this->GetTemplateChild(c_rowTopName)))
    {
        m_rowTop = row;
    }
    if (auto row = dynamic_cast<RowDefinition ^>(this->GetTemplateChild(c_rowMiddleName)))
    {
        m_rowMiddle = row;
    }
    if (auto row = dynamic_cast<RowDefinition ^>(this->GetTemplateChild(c_rowBottomName)))
    {
        m_rowBottom = row;
    }
}

void TwoPaneViewCX::UpdateMode()
{
    // Don't bother running this logic until after we hit OnApplyTemplate.
    if (!m_loaded)
        return;

    double controlWidth = this->ActualWidth;
    double controlHeight = this->ActualHeight;

    ViewMode newMode = (this->PanePriority == TwoPaneViewCXPriority::Pane1) ? ViewMode::Pane1Only : ViewMode::Pane2Only;

    // Calculate new mode
    Rect rcControl = GetControlRect();
    auto info = this->GetDisplayRegionHelperInfo();
    bool isInMultipleRegions = IsInMultipleRegions(info, rcControl);

    if (isInMultipleRegions)
    {
        if (info.Mode == TwoPaneViewCXMode::Wide)
        {
            // Regions are laid out horizontally
            if (this->WideModeConfiguration != TwoPaneViewCXWideModeConfiguration::SinglePane)
            {
                newMode = (this->WideModeConfiguration == TwoPaneViewCXWideModeConfiguration::LeftRight) ? ViewMode::LeftRight : ViewMode::RightLeft;
            }
        }
        else if (info.Mode == TwoPaneViewCXMode::Tall)
        {
            // Regions are laid out vertically
            if (this->TallModeConfiguration != TwoPaneViewCXTallModeConfiguration::SinglePane)
            {
                newMode = (this->TallModeConfiguration == TwoPaneViewCXTallModeConfiguration::TopBottom) ? ViewMode::TopBottom : ViewMode::BottomTop;
            }
        }
    }
    else
    {
        // One region
        if (controlWidth > this->MinWideModeWidth && this->WideModeConfiguration != TwoPaneViewCXWideModeConfiguration::SinglePane)
        {
            // Split horizontally
            newMode = (this->WideModeConfiguration == TwoPaneViewCXWideModeConfiguration::LeftRight) ? ViewMode::LeftRight : ViewMode::RightLeft;
        }
        else if (controlHeight > this->MinTallModeHeight && this->TallModeConfiguration != TwoPaneViewCXTallModeConfiguration::SinglePane)
        {
            // Split vertically
            newMode = (this->TallModeConfiguration == TwoPaneViewCXTallModeConfiguration::TopBottom) ? ViewMode::TopBottom : ViewMode::BottomTop;
        }
    }

    // Update row/column sizes (this may need to happen even if the mode doesn't change)
    UpdateRowsColumns(newMode, info, rcControl);

    // Update mode if necessary
    if (newMode != m_currentMode)
    {
        m_currentMode = newMode;

        auto newViewMode = TwoPaneViewCXMode::SinglePane;

        switch (m_currentMode)
        {
        case ViewMode::Pane1Only:
            VisualStateManager::GoToState(this, L"ViewMode_OneOnly", true);
            break;
        case ViewMode::Pane2Only:
            VisualStateManager::GoToState(this, L"ViewMode_TwoOnly", true);
            break;
        case ViewMode::LeftRight:
            VisualStateManager::GoToState(this, L"ViewMode_LeftRight", true);
            newViewMode = TwoPaneViewCXMode::Wide;
            break;
        case ViewMode::RightLeft:
            VisualStateManager::GoToState(this, L"ViewMode_RightLeft", true);
            newViewMode = TwoPaneViewCXMode::Wide;
            break;
        case ViewMode::TopBottom:
            VisualStateManager::GoToState(this, L"ViewMode_TopBottom", true);
            newViewMode = TwoPaneViewCXMode::Tall;
            break;
        case ViewMode::BottomTop:
            VisualStateManager::GoToState(this, L"ViewMode_BottomTop", true);
            newViewMode = TwoPaneViewCXMode::Tall;
            break;
        }

        if (newViewMode != this->Mode)
        {
            SetValue(s_ModeProperty, newViewMode);
            this->ModeChanged(this, this);
        }
    }
}

void TwoPaneViewCX::UpdateRowsColumns(ViewMode newMode, DisplayRegionHelperInfo info, Rect rcControl)
{
    if (m_columnLeft && m_columnMiddle && m_columnRight && m_rowTop && m_rowMiddle && m_rowBottom)
    {
        // Reset split lengths
        this->m_columnMiddle->Width = GridLengthHelper::FromPixels(0);
        this->m_rowMiddle->Height = GridLengthHelper::FromPixels(0);

        // Set columns lengths
        if (newMode == ViewMode::LeftRight || newMode == ViewMode::RightLeft)
        {
            if (newMode == ViewMode::LeftRight)
            {
                this->m_columnLeft->MinWidth = this->Pane1MinLength;
                this->m_columnLeft->MaxWidth = this->Pane1MaxLength;
                this->m_columnLeft->Width = this->Pane1Length;
                this->m_columnRight->MinWidth = this->Pane2MinLength;
                this->m_columnRight->MaxWidth = this->Pane2MaxLength;
                this->m_columnRight->Width = this->Pane2Length;
            }
            else
            {
                this->m_columnLeft->MinWidth = this->Pane2MinLength;
                this->m_columnLeft->MaxWidth = this->Pane2MaxLength;
                this->m_columnLeft->Width = this->Pane2Length;
                this->m_columnRight->MinWidth = this->Pane1MinLength;
                this->m_columnRight->MaxWidth = this->Pane1MaxLength;
                this->m_columnRight->Width = this->Pane1Length;
            }
        }
        else
        {
            this->m_columnLeft->MinWidth = 0.0;
            this->m_columnRight->MinWidth = 0.0; 
            this->m_columnLeft->MaxWidth = std::numeric_limits<double>::max();
            this->m_columnRight->MaxWidth = std::numeric_limits<double>::max();
            this->m_columnLeft->Width = GridLengthHelper::FromValueAndType(1, GridUnitType::Star);
            this->m_columnRight->Width = GridLengthHelper::FromPixels(0);
        }

        // Set row lengths
        if (newMode == ViewMode::TopBottom || newMode == ViewMode::BottomTop)
        {
            if (newMode == ViewMode::TopBottom)
            {
                this->m_rowTop->MinHeight = this->Pane1MinLength;
                this->m_rowTop->MaxHeight = this->Pane1MaxLength;
                this->m_rowTop->Height = this->Pane1Length;
                this->m_rowBottom->MinHeight = this->Pane2MinLength;
                this->m_rowBottom->MaxHeight = this->Pane2MaxLength;
                this->m_rowBottom->Height = this->Pane2Length;
            }
            else
            {
                this->m_rowTop->MinHeight = this->Pane2MinLength;
                this->m_rowTop->MaxHeight = this->Pane2MaxLength;
                this->m_rowTop->Height = this->Pane2Length;
                this->m_rowBottom->MinHeight = this->Pane1MinLength;
                this->m_rowBottom->MaxHeight = this->Pane1MaxLength;
                this->m_rowBottom->Height = this->Pane1Length;
            }
        }
        else
        {
            this->m_rowTop->MinHeight = 0.0;
            this->m_rowBottom->MinHeight = 0.0;
            this->m_rowTop->MaxHeight = std::numeric_limits<double>::max();
            this->m_rowBottom->MaxHeight = std::numeric_limits<double>::max();
            this->m_rowTop->Height = GridLengthHelper::FromValueAndType(1, GridUnitType::Star);
            this->m_rowBottom->Height = GridLengthHelper::FromPixels(0);
        }

        // Handle regions
        if (IsInMultipleRegions(info, rcControl) && newMode != ViewMode::Pane1Only && newMode != ViewMode::Pane2Only)
        {
            Rect rc1 = info.Regions[0];
            Rect rc2 = info.Regions[1];
            Rect rcWindow = Window::Current->Bounds;

            if (info.Mode == TwoPaneViewCXMode::Wide)
            {
                this->m_columnMiddle->Width = GridLengthHelper::FromPixels(rc2.X - rc1.Width);

                this->m_columnLeft->MinWidth = 0.0;
                this->m_columnRight->MinWidth = 0.0;
                this->m_columnLeft->MaxWidth = std::numeric_limits<double>::max();
                this->m_columnRight->MaxWidth = std::numeric_limits<double>::max();
                this->m_columnLeft->Width = GridLengthHelper::FromPixels(rc1.Width - rcControl.X);
                this->m_columnRight->Width = GridLengthHelper::FromPixels(rc2.Width - ((rcWindow.Width - rcControl.Width) - rcControl.X));
            }
            else
            {
                this->m_rowMiddle->Height = GridLengthHelper::FromPixels(rc2.Y - rc1.Height);

                this->m_rowTop->MinHeight = 0.0;
                this->m_rowBottom->MinHeight = 0.0;
                this->m_rowTop->MaxHeight = std::numeric_limits<double>::max();
                this->m_rowBottom->MaxHeight = std::numeric_limits<double>::max();
                this->m_rowTop->Height = GridLengthHelper::FromPixels(rc1.Height - rcControl.Y);
                this->m_rowBottom->Height = GridLengthHelper::FromPixels(rc2.Height - ((rcWindow.Height - rcControl.Height) - rcControl.Y));
            }
        }
    }
}

Rect TwoPaneViewCX::GetControlRect()
{
    // Find out where this control is in the window
    auto transform = TransformToVisual(Window::Current->Content);
    return transform->TransformBounds(RectHelper::FromCoordinatesAndDimensions(0, 0, (float)this->ActualWidth, (float)this->ActualHeight));
}

bool TwoPaneViewCX::IsInMultipleRegions(DisplayRegionHelperInfo info, Rect rcControl)
{
    bool isInMultipleRegions = false;

    if (info.Mode != TwoPaneViewCXMode::SinglePane)
    {
        Rect rc1 = info.Regions[0];
        Rect rc2 = info.Regions[1];

        if (info.Mode == TwoPaneViewCXMode::Wide)
        {
            // Check that the control is over the split
            if (rcControl.X < rc1.Width && rcControl.X + rcControl.Width > rc2.X)
            {
                isInMultipleRegions = true;
            }
        }
        else if (info.Mode == TwoPaneViewCXMode::Tall)
        {
            // Check that the control is over the split
            if (rcControl.Y < rc1.Height && rcControl.Y + rcControl.Height > rc2.Y)
            {
                isInMultipleRegions = true;
            }
        }
    }

    return isInMultipleRegions;
}

void TwoPaneViewCX::OnSizeChanged(Platform::Object ^ /*sender*/, Windows::UI::Xaml::SizeChangedEventArgs ^ /*e*/)
{
    this->UpdateMode();
}

void TwoPaneViewCX::OnWindowSizeChanged(Platform::Object ^ /*sender*/, Windows::UI::Core::WindowSizeChangedEventArgs ^ /*e*/)
{
    this->UpdateMode();
}

void TwoPaneViewCX::OnPane1LengthPropertyChanged(GridLength /*oldValue*/, GridLength /*newValue*/)
{
    this->UpdateMode();
}

void TwoPaneViewCX::OnPane2LengthPropertyChanged(GridLength /*oldValue*/, GridLength /*newValue*/)
{
    this->UpdateMode();
}

void TwoPaneViewCX::OnPane1MinLengthPropertyChanged(double /*oldValue*/, double /*newValue*/)
{
    this->UpdateMode();
}

void TwoPaneViewCX::OnPane2MinLengthPropertyChanged(double /*oldValue*/, double /*newValue*/)
{
    this->UpdateMode();
}

void TwoPaneViewCX::OnPane1MaxLengthPropertyChanged(double /*oldValue*/, double /*newValue*/)
{
    this->UpdateMode();
}

void TwoPaneViewCX::OnPane2MaxLengthPropertyChanged(double /*oldValue*/, double /*newValue*/)
{
    this->UpdateMode();
}

void TwoPaneViewCX::OnMinTallModeHeightPropertyChanged(double /*oldValue*/, double newValue)
{
    auto clampedValue = max(0.0, newValue);
    if (clampedValue != newValue)
    {
        this->MinTallModeHeight = clampedValue;
        return;
    }
    this->UpdateMode();
}

void TwoPaneViewCX::OnMinWideModeWidthPropertyChanged(double /*oldValue*/, double newValue)
{
    auto clampedValue = max(0.0, newValue);
    if (clampedValue != newValue)
    {
        this->MinWideModeWidth = clampedValue;
        return;
    }
    this->UpdateMode();
}

void TwoPaneViewCX::OnWideModeConfigurationPropertyChanged(
    TwoPaneViewCXWideModeConfiguration /*oldValue*/,
    TwoPaneViewCXWideModeConfiguration /*newValue*/)
{
    this->UpdateMode();
}

void TwoPaneViewCX::OnTallModeConfigurationPropertyChanged(
    TwoPaneViewCXTallModeConfiguration /*oldValue*/,
    TwoPaneViewCXTallModeConfiguration /*newValue*/)
{
    this->UpdateMode();
}

void TwoPaneViewCX::OnPanePriorityPropertyChanged(TwoPaneViewCXPriority /*oldValue*/, TwoPaneViewCXPriority /*newValue*/)
{
    this->UpdateMode();
}

TwoPaneViewCX::DisplayRegionHelperInfo TwoPaneViewCX::GetDisplayRegionHelperInfo()
{
    DisplayRegionHelperInfo info;
    info.Mode = TwoPaneViewCXMode::SinglePane;

    if (!Windows::Foundation::Metadata::ApiInformation::IsMethodPresent(L"Windows.UI.ViewManagement.ApplicationView", L"GetDisplayRegions"))
    {
        return info;
    }

    // ApplicationView::GetForCurrentView throws on failure; in that case we just won't do anything.
    ApplicationView ^ view;
    try
    {
        view = ApplicationView::GetForCurrentView();
    }
    catch (...)
    {
    }

    if (view && (int)view->ViewMode == 2 /*ApplicationViewMode::Spanning*/)
    {
        auto regions = view->GetDisplayRegions();
        if (regions->Size == 2)
        {
            auto region1 = regions->GetAt(0);
            auto region2 = regions->GetAt(1);
            info.Regions = ref new Array<Rect>(2);
            info.Regions[0] = RectHelper::FromCoordinatesAndDimensions(
                region1->WorkAreaOffset.X, region1->WorkAreaOffset.Y, region1->WorkAreaSize.Width, region1->WorkAreaSize.Height);
            info.Regions[1] = RectHelper::FromCoordinatesAndDimensions(
                region2->WorkAreaOffset.X, region2->WorkAreaOffset.Y, region2->WorkAreaSize.Width, region2->WorkAreaSize.Height);

            // Determine orientation. If neither of these are true, default to doing nothing.
            if (info.Regions[0].X < info.Regions[1].X && info.Regions[0].Y == info.Regions[1].Y)
            {
                // Double portrait
                info.Mode = TwoPaneViewCXMode::Wide;
            }
            else if (info.Regions[0].X == info.Regions[1].X && info.Regions[0].Y < info.Regions[1].Y)
            {
                // Double landscape
                info.Mode = TwoPaneViewCXMode::Tall;
            }
        }
    }
    return info;
}

void TwoPaneViewCX::SetScrollViewerProperties(String ^ scrollViewerName)
{
    if (ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 7))
    {
        if (auto scrollViewer = dynamic_cast<ScrollViewer ^>(this->GetTemplateChild(scrollViewerName)))
        {
            if (ApiInformation::IsPropertyPresent(L"Windows.UI.Xaml.Controls.ScrollContentPresenter", L"SizesContentToTemplatedParent"))
            {
                scrollViewer->Loaded += ref new RoutedEventHandler(this, &TwoPaneViewCX::OnScrollViewerLoaded);
            }

            if (ApiInformation::IsPropertyPresent(L"Windows.UI.Xaml.Controls.ScrollViewer", L"ReduceViewportForCoreInputViewOcclusions"))
            {
                scrollViewer->ReduceViewportForCoreInputViewOcclusions = true;
            }
        }
    }
}

void TwoPaneViewCX::OnScrollViewerLoaded(Object ^ sender, RoutedEventArgs ^ e)
{
    if (auto scrollViewer = dynamic_cast<FrameworkElement ^>(sender))
    {
        auto scrollContentPresenterFE = VisualTree::FindDescendantByName(scrollViewer, L"ScrollContentPresenter");
        if (scrollContentPresenterFE)
        {
            if (auto scrollContentPresenter = dynamic_cast<ScrollContentPresenter ^>(scrollContentPresenterFE))
            {
                scrollContentPresenter->SizesContentToTemplatedParent = true;
            }
        }
    }
}
