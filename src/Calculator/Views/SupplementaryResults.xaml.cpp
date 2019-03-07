// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// SupplementaryResults.xaml.cpp
// Implementation of the SupplementaryResults class
//

#include "pch.h"
#include "SupplementaryResults.xaml.h"

using namespace CalculatorApp;

using namespace Platform;
using namespace Platform::Collections;
using namespace ViewModel;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

Object^ DelighterUnitToStyleConverter::Convert(Object^ value, TypeName /*targetType*/, Object^ /*parameter*/, String^ /*language*/)
{
    Unit^ unit = safe_cast<Unit^>(value);

    assert(unit->GetModelUnit().isWhimsical);
    if (!unit->GetModelUnit().isWhimsical)
    {
        return nullptr;
    }

    std::wstring key = L"Unit_";
    key.append(std::to_wstring(unit->GetModelUnit().id));
    return safe_cast<IStyle^>(m_delighters->Lookup(ref new String(key.c_str())));
}

Object^ DelighterUnitToStyleConverter::ConvertBack(Object^ /*value*/, TypeName /*targetType*/, Object^ /*parameter*/, String^ /*language*/)
{
    // We never use convert back, only one way binding supported
    assert(false);
    return nullptr;
}

Windows::UI::Xaml::DataTemplate^ SupplementaryResultDataTemplateSelector::SelectTemplateCore(Object^ item, DependencyObject^ /*container*/)
{
    SupplementaryResult^ result = safe_cast<SupplementaryResult^>(item);
    if (result->IsWhimsical())
    {
        return DelighterTemplate;
    }
    else
    {
        return RegularTemplate;
    }
}

SupplementaryResults::SupplementaryResults() :
    m_data(ref new Vector<SupplementaryResult^>)
{
    InitializeComponent();

    this->Loaded += ref new RoutedEventHandler(this, &SupplementaryResults::OnLoaded);
}

void SupplementaryResults::RefreshData()
{
    // Copy the list so that when we chop stuff off, we don't modify the original
    // complete list.
    m_data->Clear();
    for(SupplementaryResult^ sr : safe_cast<UnitConverterViewModel^>(this->DataContext)->SupplementaryResults)
    {
        m_data->Append(sr);
    }

    // Set as source
    SupplementaryValues->ItemsSource = m_data;
}

void SupplementaryResults::OnLoaded(Object^ sender, RoutedEventArgs^ e)
{
    UnitConverterViewModel^ vm = safe_cast<UnitConverterViewModel^>(this->DataContext);
    vm->PropertyChanged += ref new PropertyChangedEventHandler(this, &SupplementaryResults::OnConverterPropertyChanged);
    Window::Current->SizeChanged += ref new WindowSizeChangedEventHandler(this, &SupplementaryResults::OnWindowSizeChanged);
    // We may be loaded into a state where we need to render (like rehydrate), so prepare data
    RefreshData();
}

void SupplementaryResults::OnConverterPropertyChanged(Object^ /*sender*/, PropertyChangedEventArgs^ e)
{
    if (e->PropertyName == UnitConverterViewModelProperties::SupplementaryResults)
    {
        RefreshData();
    }
}

void SupplementaryResults::OnWindowSizeChanged(Platform::Object^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ e)
{
    // to reload supplementary results every time the window is resized
    RefreshData();
}

void SupplementaryResults::OnSupplementaryValuesLayoutUpdated(Platform::Object^ sender, Platform::Object^ e)
{
    // This means we overflowed and are cutting off, or in a very rare case we fit exactly. Unfortunately
    // the fitting exactly case will still have an item removed, as there is no other way for us to
    // detect that we need to trim.
    Grid^ parentGrid = dynamic_cast<Grid^>(VisualTreeHelper::GetParent(this));
    if (parentGrid != nullptr)
    {
        double parentWidth = parentGrid->ActualWidth;
        if (SupplementaryValues && SupplementaryValues->ActualWidth >= parentWidth)
        {
            if (m_data->Size > 0)
            {
                SupplementaryResult^ last = m_data->GetAt(m_data->Size - 1);
                if (!last->IsWhimsical() || m_data->Size <= 2)
                {
                    m_data->RemoveAtEnd();
                }
                else
                {
                    m_data->RemoveAt(m_data->Size - 2);
                }
            }
        }
    }
}
