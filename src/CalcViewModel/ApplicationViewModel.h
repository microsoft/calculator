// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "StandardCalculatorViewModel.h"
#include "DateCalculatorViewModel.h"
#include "UnitConverterViewModel.h"

namespace CalculatorApp
{
    namespace ViewModel
    {
        [Windows::UI::Xaml::Data::Bindable] public ref class ApplicationViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
        {
        public:
            ApplicationViewModel();

            void Initialize(CalculatorApp::Common::ViewMode mode); // Use for first init, use deserialize for rehydration

            OBSERVABLE_OBJECT();
            OBSERVABLE_PROPERTY_RW(StandardCalculatorViewModel ^, CalculatorViewModel);
            OBSERVABLE_PROPERTY_RW(DateCalculatorViewModel ^, DateCalcViewModel);
            OBSERVABLE_PROPERTY_RW(UnitConverterViewModel ^, ConverterViewModel);
            OBSERVABLE_PROPERTY_RW(CalculatorApp::Common::ViewMode, PreviousMode);
            OBSERVABLE_NAMED_PROPERTY_RW(Platform::String ^, CategoryName);

            COMMAND_FOR_METHOD(CopyCommand, ApplicationViewModel::OnCopyCommand);
            COMMAND_FOR_METHOD(PasteCommand, ApplicationViewModel::OnPasteCommand);

            property CalculatorApp::Common::ViewMode Mode
            {
                CalculatorApp::Common::ViewMode get()
                {
                    return m_mode;
                }

                void set(CalculatorApp::Common::ViewMode value);
            }
            static property Platform::String^ ModePropertyName
            {
                Platform::String^ get()
                {
                    return Platform::StringReference(L"Mode");
                }
            }

            property Windows::Foundation::Collections::IObservableVector<CalculatorApp::Common::NavCategoryGroup^>^ Categories
            {
                Windows::Foundation::Collections::IObservableVector<CalculatorApp::Common::NavCategoryGroup^>^ get()
                {
                    return m_categories;
                }

                void set(Windows::Foundation::Collections::IObservableVector<CalculatorApp::Common::NavCategoryGroup^>^ value);
            }

            property Windows::UI::Xaml::Visibility ClearMemoryVisibility
            {
                Windows::UI::Xaml::Visibility get()
                {
                    return CalculatorApp::Common::NavCategory::IsCalculatorViewMode(Mode) ? Windows::UI::Xaml::Visibility::Visible
                                                                                          : Windows::UI::Xaml::Visibility::Collapsed;
                }
            }

        private:
            bool TryRecoverFromNavigationModeFailure();

            void OnModeChanged();

            void OnCopyCommand(Platform::Object ^ parameter);
            void OnPasteCommand(Platform::Object ^ parameter);

            void SetMenuCategories();

            CalculatorApp::Common::ViewMode m_mode;
            Windows::Foundation::Collections::IObservableVector<CalculatorApp::Common::NavCategoryGroup ^> ^ m_categories;
        };
    }
}
