// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CalculationResult.h"
#include "CalculationResultAutomationPeer.h"
#include "CalcViewModel/Common/LocalizationSettings.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Controls;
using namespace CalculatorApp::Common;

using namespace Platform;
using namespace Windows::Devices::Input;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Automation::Peers;
using namespace std;

DEPENDENCY_PROPERTY_INITIALIZATION(CalculationResult, IsActive);
DEPENDENCY_PROPERTY_INITIALIZATION(CalculationResult, MinFontSize);
DEPENDENCY_PROPERTY_INITIALIZATION(CalculationResult, MaxFontSize);
DEPENDENCY_PROPERTY_INITIALIZATION(CalculationResult, DisplayMargin);
DEPENDENCY_PROPERTY_INITIALIZATION(CalculationResult, DisplayValue);
DEPENDENCY_PROPERTY_INITIALIZATION(CalculationResult, IsInError);
DEPENDENCY_PROPERTY_INITIALIZATION(CalculationResult, IsOperatorCommand);

#define SCALEFACTOR 0.357143
#define SMALLHEIGHTSCALEFACTOR 0
#define HEIGHTCUTOFF 100
#define INCREMENTOFFSET 1
#define MAXFONTINCREMENT 5
#define WIDTHTOFONTSCALAR 0.0556513
#define WIDTHTOFONTOFFSET 3
#define WIDTHCUTOFF 50
#define FONTTOLERANCE 0.001
#define SCROLL_RATIO 0.7

// We need a safety margin to guarantee we correctly always show/hide ScrollLeft and ScrollRight buttons when necessary.
// In rare cases, ScrollViewer::HorizontalOffset is a little low by a few (sub)pixels when users scroll to one of the extremity
// and no events are launched when they scroll again in the same direction
#define SCROLL_BUTTONS_APPROXIMATION_RANGE 4

CalculationResult::CalculationResult()
    : m_isScalingText(false)
    , m_haveCalculatedMax(false)
{
}

Platform::String ^ CalculationResult::GetRawDisplayValue()
{
    return LocalizationSettings::GetInstance().RemoveGroupSeparators(DisplayValue);
}

void CalculationResult::OnApplyTemplate()
{
    if (m_textContainer)
    {
        if (m_textContainerLayoutChangedToken.Value != 0)
        {
            m_textContainer->LayoutUpdated -= m_textContainerLayoutChangedToken;
            m_textContainerLayoutChangedToken.Value = 0;
        }
        if (m_textContainerSizeChangedToken.Value != 0)
        {
            m_textContainer->SizeChanged -= m_textContainerSizeChangedToken;
            m_textContainerSizeChangedToken.Value = 0;
        }
        if (m_textContainerViewChangedToken.Value != 0)
        {
            m_textContainer->ViewChanged -= m_textContainerViewChangedToken;
            m_textContainerViewChangedToken.Value = 0;
        }
    }

    if (m_textBlock != nullptr && m_textBlockSizeChangedToken.Value != 0)
    {
        m_textBlock->SizeChanged -= m_textBlockSizeChangedToken;
        m_textBlockSizeChangedToken.Value = 0;
    }

    if (m_scrollLeft != nullptr && m_scrollLeftClickToken.Value != 0)
    {
        m_scrollLeft->Click -= m_scrollLeftClickToken;
        m_scrollLeftClickToken.Value = 0;
    }

    if (m_scrollRight != nullptr && m_scrollRightClickToken.Value != 0)
    {
        m_scrollRight->Click -= m_scrollRightClickToken;
        m_scrollRightClickToken.Value = 0;
    }

    m_textContainer = dynamic_cast<ScrollViewer ^>(GetTemplateChild("TextContainer"));
    if (m_textContainer)
    {
        // We want to know when the size of the container changes so
        // we can rescale the textbox
        m_textContainerSizeChangedToken = m_textContainer->SizeChanged += ref new SizeChangedEventHandler(this, &CalculationResult::OnTextContainerSizeChanged);

        m_textContainerViewChangedToken = m_textContainer->ViewChanged +=
            ref new Windows::Foundation::EventHandler<Windows::UI::Xaml::Controls::ScrollViewerViewChangedEventArgs ^>(
                this, &CalculatorApp::Controls::CalculationResult::OnTextContainerOnViewChanged);

        m_textContainerLayoutChangedToken = m_textContainer->LayoutUpdated +=
            ref new EventHandler<Object ^>(this, &CalculationResult::OnTextContainerLayoutUpdated);

        m_textContainer->ChangeView(m_textContainer->ExtentWidth - m_textContainer->ViewportWidth, nullptr, nullptr);
        m_scrollLeft = dynamic_cast<HyperlinkButton ^>(GetTemplateChild("ScrollLeft"));
        if (m_scrollLeft)
        {
            m_scrollLeftClickToken = m_scrollLeft->Click += ref new RoutedEventHandler(this, &CalculationResult::OnScrollLeftClick);
        }
        m_scrollRight = dynamic_cast<HyperlinkButton ^>(GetTemplateChild("ScrollRight"));
        if (m_scrollRight)
        {
            m_scrollRightClickToken = m_scrollRight->Click += ref new RoutedEventHandler(this, &CalculationResult::OnScrollRightClick);
        }
        m_textBlock = dynamic_cast<TextBlock ^>(GetTemplateChild("NormalOutput"));
        if (m_textBlock)
        {
            m_textBlock->Visibility = ::Visibility::Visible;
            m_textBlockSizeChangedToken = m_textBlock->SizeChanged += ref new SizeChangedEventHandler(this, &CalculationResult::OnTextBlockSizeChanged);
        }
    }
    UpdateVisualState();
    UpdateTextState();
}

void CalculationResult::OnTextContainerLayoutUpdated(Object ^ /*sender*/, Object ^ /*e*/)
{
    if (m_isScalingText)
    {
        UpdateTextState();
    }
}

void CalculationResult::OnTextContainerSizeChanged(Object ^ /*sender*/, SizeChangedEventArgs ^ /*e*/)
{
    UpdateTextState();
}

void CalculationResult::OnIsActivePropertyChanged(bool /*oldValue*/, bool /*newValue */)
{
    UpdateVisualState();
}

void CalculationResult::OnDisplayValuePropertyChanged(String ^ /*oldValue*/, String ^ /*newValue*/)
{
    UpdateTextState();
}

void CalculationResult::OnMinFontSizePropertyChanged(double /*oldValue*/, double /*newValue*/)
{
    UpdateTextState();
}

void CalculationResult::OnMaxFontSizePropertyChanged(double /*oldValue*/, double /*newValue*/)
{
    UpdateTextState();
}

void CalculationResult::OnIsInErrorPropertyChanged(bool /*oldValue*/, bool newValue)
{
    // We need to have a good template for this to work
    if (m_textBlock == nullptr)
    {
        return;
    }

    if (newValue)
    {
        // If there's an error message we need to override the normal display font
        // with the font appropriate for this language. This is because the error
        // message is localized and therefore can contain characters that are not
        // available in the normal font.
        // We use UIText as the font type because this is the most common font type to use
        m_textBlock->FontFamily = LocalizationService::GetInstance()->GetLanguageFontFamilyForType(LanguageFontType::UIText);
    }
    else
    {
        // The error result is no longer an error so we will restore the
        // value to FontFamily property to the value provided in the style
        // for the TextBlock in the template.
        m_textBlock->ClearValue(TextBlock::FontFamilyProperty);
    }
}

void CalculationResult::UpdateVisualState()
{
    if (IsActive)
    {
        VisualStateManager::GoToState(this, "Active", true);
    }
    else
    {
        VisualStateManager::GoToState(this, "Normal", true);
    }
}

void CalculationResult::UpdateTextState()
{
    if ((m_textContainer == nullptr) || (m_textBlock == nullptr))
    {
        return;
    }

    auto containerSize = m_textContainer->ActualWidth;
    String ^ oldText = m_textBlock->Text;
    String ^ newText = DisplayValue;

    // Initiate the scaling operation
    // UpdateLayout will keep calling us until we make it through the below 2 if-statements
    if (!m_isScalingText || oldText != newText)
    {
        m_textBlock->Text = newText;

        m_isScalingText = true;
        m_haveCalculatedMax = false;
        m_textBlock->InvalidateArrange();
        return;
    }
    if (containerSize > 0)
    {
        double widthDiff = abs(m_textBlock->ActualWidth - containerSize);
        double fontSizeChange = INCREMENTOFFSET;

        if (widthDiff > WIDTHCUTOFF)
        {
            fontSizeChange = min<double>(max<double>(floor(WIDTHTOFONTSCALAR * widthDiff) - WIDTHTOFONTOFFSET, INCREMENTOFFSET), MAXFONTINCREMENT);
        }
        if (m_textBlock->ActualWidth < containerSize && abs(m_textBlock->FontSize - MaxFontSize) > FONTTOLERANCE && !m_haveCalculatedMax)
        {
            ModifyFontAndMargin(m_textBlock, fontSizeChange);
            m_textBlock->InvalidateArrange();
            return;
        }
        if (fontSizeChange < 5)
        {
            m_haveCalculatedMax = true;
        }
        if (m_textBlock->ActualWidth >= containerSize && abs(m_textBlock->FontSize - MinFontSize) > FONTTOLERANCE)
        {
            ModifyFontAndMargin(m_textBlock, -1 * fontSizeChange);
            m_textBlock->InvalidateArrange();
            return;
        }
        assert(m_textBlock->FontSize >= MinFontSize && m_textBlock->FontSize <= MaxFontSize);
        m_isScalingText = false;
        if (IsOperatorCommand)
        {
            m_textContainer->ChangeView(0.0, nullptr, nullptr);
        }
        else
        {
            m_textContainer->ChangeView(m_textContainer->ExtentWidth - m_textContainer->ViewportWidth, nullptr, nullptr);
        }
    }
}

void CalculationResult::ScrollLeft()
{
    if (m_textContainer == nullptr)
    {
        return;
    }
    if (m_textContainer->HorizontalOffset > 0)
    {
        double offset = m_textContainer->HorizontalOffset - (SCROLL_RATIO * m_textContainer->ViewportWidth);
        m_textContainer->ChangeView(offset, nullptr, nullptr);
    }
}

void CalculationResult::ScrollRight()
{
    if (m_textContainer == nullptr)
    {
        return;
    }

    if (m_textContainer->HorizontalOffset < m_textContainer->ExtentWidth - m_textContainer->ViewportWidth)
    {
        double offset = m_textContainer->HorizontalOffset + (SCROLL_RATIO * m_textContainer->ViewportWidth);
        m_textContainer->ChangeView(offset, nullptr, nullptr);
    }
}

void CalculationResult::OnKeyDown(KeyRoutedEventArgs ^ e)
{
    switch (e->Key)
    {
    case Windows::System::VirtualKey::Left:
        this->ScrollLeft();
        break;
    case Windows::System::VirtualKey::Right:
        this->ScrollRight();
        break;
    }
}

void CalculationResult::OnScrollLeftClick(Object ^ sender, RoutedEventArgs ^ /*e*/)
{
    ScrollLeft();
}

void CalculationResult::OnScrollRightClick(Object ^ sender, RoutedEventArgs ^ /*e*/)
{
    ScrollRight();
}

void CalculationResult::UpdateScrollButtons()
{
    if (m_textContainer == nullptr)
    {
        return;
    }

    bool shouldTryFocusScrollRight = false;
    if (m_scrollLeft != nullptr)
    {
        auto scrollLeftVisibility = m_textContainer->HorizontalOffset > SCROLL_BUTTONS_APPROXIMATION_RANGE ? ::Visibility::Visible : ::Visibility::Collapsed;

        if (scrollLeftVisibility == ::Visibility::Collapsed)
        {
            shouldTryFocusScrollRight = m_scrollLeft->Equals(FocusManager::GetFocusedElement());
        }

        m_scrollLeft->Visibility = scrollLeftVisibility;
    }

    if (m_scrollRight != nullptr)
    {
        auto scrollRightVisibility =
            m_textContainer->HorizontalOffset + m_textContainer->ViewportWidth + SCROLL_BUTTONS_APPROXIMATION_RANGE < m_textContainer->ExtentWidth
                ? ::Visibility::Visible
                : ::Visibility::Collapsed;

        if (scrollRightVisibility == ::Visibility::Collapsed && m_scrollLeft != nullptr && m_scrollLeft->Visibility == ::Visibility::Visible
            && m_scrollRight->Equals(FocusManager::GetFocusedElement()))
        {
            // ScrollRight had the focus and will be collapsed, ScrollLeft should get the focus
            m_scrollLeft->Focus(::FocusState::Programmatic);
        }
        m_scrollRight->Visibility = scrollRightVisibility;

        if (shouldTryFocusScrollRight && scrollRightVisibility == ::Visibility::Visible)
        {
            m_scrollRight->Focus(::FocusState::Programmatic);
        }
    }
}

void CalculationResult::ModifyFontAndMargin(TextBlock ^ textBox, double fontChange)
{
    double cur = textBox->FontSize;
    double newFontSize = 0.0;
    double scaleFactor = SCALEFACTOR;
    if (m_textContainer->ActualHeight <= HEIGHTCUTOFF)
    {
        scaleFactor = SMALLHEIGHTSCALEFACTOR;
    }

    newFontSize = min(max(cur + fontChange, MinFontSize), MaxFontSize);
    m_textContainer->Padding = Thickness(0, 0, 0, scaleFactor * abs(cur - newFontSize));
    textBox->FontSize = newFontSize;
}

void CalculationResult::OnTapped(TappedRoutedEventArgs ^ e)
{
    this->Focus(::FocusState::Programmatic);
    RaiseSelectedEvent();
}

void CalculationResult::OnRightTapped(RightTappedRoutedEventArgs ^ e)
{
    auto requestedElement = e->OriginalSource;

    if (requestedElement->Equals(dynamic_cast<Object ^>(m_textBlock)))
    {
        m_textBlock->Focus(::FocusState::Programmatic);
    }
    else
    {
        this->Focus(::FocusState::Programmatic);
    }
}

AutomationPeer ^ CalculationResult::OnCreateAutomationPeer()
{
    return ref new CalculationResultAutomationPeer(this);
}

void CalculationResult::ProgrammaticSelect()
{
    RaiseSelectedEvent();
}

void CalculationResult::RaiseSelectedEvent()
{
    Selected(this);
}

void CalculationResult::OnTextContainerOnViewChanged(Object ^ /*sender*/, ScrollViewerViewChangedEventArgs ^ e)
{
    UpdateScrollButtons();
}

void CalculationResult::OnTextBlockSizeChanged(Object ^ /*sender*/, SizeChangedEventArgs ^ /*e*/)
{
    UpdateScrollButtons();
}
