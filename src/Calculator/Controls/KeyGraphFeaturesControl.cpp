// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "KeyGraphFeaturesControl.h"

using namespace std;
using namespace Platform;
using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Controls;
using namespace CalculatorApp::ViewModel;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Text;

void KeyGraphFeaturesControl::OnApplyTemplate()
{
    m_equationButton = dynamic_cast<Button ^>(GetTemplateChild("EquationButton"));
    m_equationEditBox = dynamic_cast<RichEditBox ^>(GetTemplateChild("EquationTextBox"));
    m_keyGraphFeaturesBox = dynamic_cast<RichEditBox ^>(GetTemplateChild("KeyGraphFeaturesBox"));

    auto access = Windows::ApplicationModel::LimitedAccessFeatures::TryUnlockFeature(
        "com.microsoft.windows.richeditmath",
        "BeDD/jxKhz/yfVNA11t4uA==",
        "8wekyb3d8bbwe has registered their use of com.microsoft.windows.richeditmath with Microsoft and agrees to the terms of use.");

}

void KeyGraphFeaturesControl::SetRichEditText(Windows::ApplicationModel::LimitedAccessFeatureStatus lafStatus, RichEditBox ^ richEditBox, String ^ mathMLString)
{
    if (lafStatus == Windows::ApplicationModel::LimitedAccessFeatureStatus::Available || lafStatus == Windows::ApplicationModel::LimitedAccessFeatureStatus::AvailableWithoutToken)
    {
        m_keyGraphFeaturesBox->TextDocument->SetMathMode(Windows::UI::Text::RichEditMathMode::MathOnly);

        auto vm = static_cast<EquationViewModel ^>(m_keyGraphFeaturesBox->DataContext);

        m_keyGraphFeaturesBox->TextDocument->SetMath("");
    }
}
