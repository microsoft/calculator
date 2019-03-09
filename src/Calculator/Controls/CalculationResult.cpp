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
DEPENDENCY_PROPERTY_INITIALIZATION(CalculationResult, AccentColor);
DEPENDENCY_PROPERTY_INITIALIZATION(CalculationResult, MinFontSize);
DEPENDENCY_PROPERTY_INITIALIZATION(CalculationResult, DisplayMargin);
DEPENDENCY_PROPERTY_INITIALIZATION(CalculationResult, MaxExpressionHistoryCharacters);
DEPENDENCY_PROPERTY_INITIALIZATION(CalculationResult, ExpressionVisibility);
DEPENDENCY_PROPERTY_INITIALIZATION(CalculationResult, DisplayValue);
DEPENDENCY_PROPERTY_INITIALIZATION(CalculationResult, IsInError);
DEPENDENCY_PROPERTY_INITIALIZATION(CalculationResult, IsOperatorCommand);
DEPENDENCY_PROPERTY_INITIALIZATION(CalculationResult, DisplayStringExpression);

#define SCALEFACTOR 0.357143
#define SMALLHEIGHTSCALEFACTOR 0
#define HEIGHTCUTOFF 100
#define INCREMENTOFFSET 1
#define MAXFONTINCREMENT 5
#define WIDTHTOFONTSCALAR 0.0556513
#define WIDTHTOFONTOFFSET 3
#define WIDTHCUTOFF 50
#define FONTTOLERANCE 0.001

StringReference CalculationResult::s_FocusedState(L"Focused");
StringReference CalculationResult::s_UnfocusedState(L"Unfocused");

CalculationResult::CalculationResult():
    m_startingFontSize(0.0),
    m_isScalingText(false),
    m_haveCalculatedMax(false)
{
}

Platform::String^ CalculationResult::GetRawDisplayValue()
{
    std::wstring rawValue;

    LocalizationSettings::GetInstance().RemoveGroupSeparators(DisplayValue->Data(), DisplayValue->Length(), &rawValue);

    return ref new Platform::String(rawValue.c_str());
}

void CalculationResult::OnApplyTemplate()
{
    assert((m_scrollLeft == nullptr && m_scrollRight == nullptr) || (m_scrollLeft != nullptr && m_scrollRight != nullptr));
    if (m_textContainer)
    {
        m_textContainer->LayoutUpdated -= m_textContainerLayoutChangedToken;
    }
    m_textContainer = dynamic_cast<ScrollViewer^>(GetTemplateChild("textContainer"));
    if (m_textContainer)
    {
        m_textContainer->SizeChanged += ref new SizeChangedEventHandler(this, &CalculationResult::TextContainerSizeChanged);
        // We want to know when the size of the container changes so 
        // we can rescale the textbox
        m_textContainerLayoutChangedToken = m_textContainer->LayoutUpdated += ref new EventHandler<Object^>(this, &CalculationResult::OnTextContainerLayoutUpdated);

        m_textContainer->ChangeView(m_textContainer->ExtentWidth - m_textContainer->ViewportWidth,nullptr,nullptr);
        m_scrollLeft = dynamic_cast<HyperlinkButton^>(GetTemplateChild("scrollLeft"));
        m_scrollRight = dynamic_cast<HyperlinkButton^>(GetTemplateChild("scrollRight"));
        auto borderContainer = dynamic_cast<UIElement^>(GetTemplateChild("border"));
        if (m_scrollLeft && m_scrollRight)
        {
            m_scrollLeft->Click += ref new RoutedEventHandler(this, &CalculationResult::OnScrollClick);
            m_scrollRight->Click += ref new RoutedEventHandler(this, &CalculationResult::OnScrollClick);
            borderContainer->PointerEntered += ref new PointerEventHandler(this, &CalculationResult::OnPointerEntered);
            borderContainer->PointerExited += ref new PointerEventHandler(this, &CalculationResult::OnPointerExited);
        }
        m_textBlock = dynamic_cast<TextBlock^>(m_textContainer->FindName("normalOutput"));
        if (m_textBlock)
        {
            m_textBlock->Visibility = ::Visibility::Visible;
            m_startingFontSize = m_textBlock->FontSize;
        }
    }
    UpdateAllState();
    VisualStateManager::GoToState(this, s_UnfocusedState, false);
}

void CalculationResult::OnPointerPressed(PointerRoutedEventArgs^ e)
{
    if (m_scrollLeft && m_scrollRight && e->Pointer->PointerDeviceType == PointerDeviceType::Touch)
    {
        ShowHideScrollButtons(::Visibility::Collapsed, ::Visibility::Collapsed);
    }
}

void CalculationResult::OnTextContainerLayoutUpdated(Object^ /*sender*/, Object^ /*e*/)
{
    if (m_isScalingText)
    {
        UpdateTextState();
    }
}

void CalculationResult::TextContainerSizeChanged(Object^ /*sender*/, SizeChangedEventArgs^ /*e*/)
{
    UpdateTextState();
}

void CalculationResult::OnIsActivePropertyChanged(bool /*oldValue*/, bool /*newValue */)
{
    UpdateVisualState();
}

void CalculationResult::OnAccentColorPropertyChanged(Brush^ /*oldValue*/, Brush^ /*newValue*/)
{
    // Force the "Active" transition to happen again
    if (IsActive)
    {
        VisualStateManager::GoToState(this, "Normal", true);
        VisualStateManager::GoToState(this, "Active", true);
    }
}

void CalculationResult::OnDisplayValuePropertyChanged(String^ /*oldValue*/, String^ /*newValue*/)
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
    String^ oldText = m_textBlock->Text;
    String^ newText =  Utils::LRO + DisplayValue + Utils::PDF;

    //Initiate the scaling operation
    //UpdateLayout will keep calling us until we make it through the below 2 if-statements
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
            fontSizeChange = min(max(floor(WIDTHTOFONTSCALAR * widthDiff) - WIDTHTOFONTOFFSET, INCREMENTOFFSET), MAXFONTINCREMENT);
        }
        if (m_textBlock->ActualWidth < containerSize && abs(m_textBlock->FontSize - m_startingFontSize) > FONTTOLERANCE && !m_haveCalculatedMax)
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
        assert(m_textBlock->FontSize >= MinFontSize && m_textBlock->FontSize <= m_startingFontSize);
        m_isScalingText = false;
        if (IsOperatorCommand)
        {
            m_textContainer->ChangeView(0.0,nullptr,nullptr);
        }
        else
        {
            m_textContainer->ChangeView(m_textContainer->ExtentWidth - m_textContainer->ViewportWidth,nullptr,nullptr);
        }

        if (m_scrollLeft && m_scrollRight)
        {
            if (m_textBlock->ActualWidth < containerSize)
            {
                ShowHideScrollButtons(::Visibility::Collapsed, ::Visibility::Collapsed);
            }
            else
            {
                if (IsOperatorCommand)
                {
                    ShowHideScrollButtons(::Visibility::Collapsed, ::Visibility::Visible);
                }
                else
                {
                    ShowHideScrollButtons(::Visibility::Visible, ::Visibility::Collapsed);
                }
            }
        }
        m_textBlock->InvalidateArrange();
    }
}

void CalculationResult::ScrollLeft()
{
    if (m_textContainer->HorizontalOffset > 0)
    {
        double offset = m_textContainer->HorizontalOffset - (scrollRatio * m_textContainer->ViewportWidth);
        m_textContainer->ChangeView(offset, nullptr, nullptr);
        m_textContainer->UpdateLayout();
        UpdateScrollButtons();
    }
}

void CalculationResult::ScrollRight()
{
    if (m_textContainer->HorizontalOffset < m_textContainer->ExtentWidth - m_textContainer->ViewportWidth)
    {
        double offset = m_textContainer->HorizontalOffset + (scrollRatio * m_textContainer->ViewportWidth);
        m_textContainer->ChangeView(offset, nullptr, nullptr);
        m_textContainer->UpdateLayout();
        UpdateScrollButtons();
    }
}

void CalculationResult::OnKeyDown(KeyRoutedEventArgs^ e)
{
    if (m_scrollLeft != nullptr && m_scrollRight != nullptr)
    {
        auto key = e->Key;
        if (key == Windows::System::VirtualKey::Left)
        {
            this->ScrollLeft();
        }
        else if (key == Windows::System::VirtualKey::Right)
        {
            this->ScrollRight();
        }
    }
}

void CalculationResult::OnScrollClick(Object^ sender, RoutedEventArgs^ /*e*/)
{
    auto clicked = dynamic_cast<HyperlinkButton^>(sender);
    if (clicked == m_scrollLeft)
    {
        this->ScrollLeft();
    }
    else
    {
        this->ScrollRight();
    }
}

void CalculationResult::OnPointerEntered(Platform::Object^ sender,  PointerRoutedEventArgs^ e)
{
    if (e->Pointer->PointerDeviceType == PointerDeviceType::Mouse && m_textBlock->ActualWidth >= m_textContainer->ActualWidth)
    {
        UpdateScrollButtons();
    }
}

void CalculationResult::ShowHideScrollButtons(::Visibility vLeft, ::Visibility vRight)
{
    m_scrollLeft->Visibility = vLeft;
    m_scrollRight->Visibility = vRight;
}

void CalculationResult::UpdateScrollButtons()
{
    // When the width is smaller than the container, don't show any
    if (m_textBlock->ActualWidth < m_textContainer->ActualWidth) 
    {
        ShowHideScrollButtons(::Visibility::Collapsed, ::Visibility::Collapsed);
    }
    // We have more number on both side. Show both arrows
    else if (m_textContainer->HorizontalOffset > 0 && m_textContainer->HorizontalOffset < (m_textContainer->ExtentWidth - m_textContainer->ViewportWidth)) 
    {
        ShowHideScrollButtons(::Visibility::Visible, ::Visibility::Visible);
    }
    // Width is larger than the container and left most part of the number is shown. Should be able to scroll left.
    else if (m_textContainer->HorizontalOffset == 0) 
    {
        ShowHideScrollButtons(::Visibility::Collapsed, ::Visibility::Visible);
    }
    else // Width is larger than the container and right most part of the number is shown. Should be able to scroll left.
    {
        ShowHideScrollButtons(::Visibility::Visible, ::Visibility::Collapsed);
    }
}

void CalculationResult::OnPointerExited(Platform::Object^ sender, PointerRoutedEventArgs^ e)
{
    if (e->Pointer->PointerDeviceType == PointerDeviceType::Mouse)
    {
        UpdateScrollButtons();
    }
}

void CalculationResult::ModifyFontAndMargin(TextBlock^ textBox, double fontChange)
{
    double cur = textBox->FontSize;
    double newFontSize = 0.0;
    Thickness t = textBox->Margin;
    double scaleFactor = SCALEFACTOR;
    if (m_textContainer->ActualHeight <= HEIGHTCUTOFF)
    {
        scaleFactor = SMALLHEIGHTSCALEFACTOR;
    }
    if (fontChange < 0)
    {
        newFontSize = max(cur + fontChange, MinFontSize);
        t.Bottom += scaleFactor * abs(cur - newFontSize);
    }
    else
    {
        newFontSize = min(cur + fontChange, m_startingFontSize);
        t.Bottom -= scaleFactor * abs(cur - newFontSize);
    }
    textBox->FontSize = newFontSize;
    textBox->Margin = t;
}

void CalculationResult::UpdateAllState()
{
    UpdateVisualState();
    UpdateTextState();
}

void CalculationResult::OnTapped(TappedRoutedEventArgs^ e)
{
    this->Focus(::FocusState::Programmatic);
    RaiseSelectedEvent();
}

void CalculationResult::OnRightTapped(RightTappedRoutedEventArgs^ e)
{
    this->Focus(::FocusState::Programmatic);
}

void CalculationResult::OnGotFocus(RoutedEventArgs^ e)
{
    if (this->FocusState == ::FocusState::Keyboard)
    {
        VisualStateManager::GoToState(this, s_FocusedState, true);
    }
}

void CalculationResult::OnLostFocus(RoutedEventArgs^ e)
{
    VisualStateManager::GoToState(this, s_UnfocusedState, true);
}

AutomationPeer^ CalculationResult::OnCreateAutomationPeer()
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
