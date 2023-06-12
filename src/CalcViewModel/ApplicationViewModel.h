// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "StandardCalculatorViewModel.h"
#include "DateCalculatorViewModel.h"
#include "GraphingCalculator/GraphingCalculatorViewModel.h"
#include "UnitConverterViewModel.h"

namespace CalculatorApp
{
    namespace ViewModel
    {
        [Windows::UI::Xaml::Data::Bindable] public ref class ApplicationViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
        {
        public:
            ApplicationViewModel();

            void Initialize(CalculatorApp::ViewModel::Common::ViewMode mode); // Use for first init, use deserialize for rehydration

            OBSERVABLE_OBJECT();
            OBSERVABLE_PROPERTY_RW(StandardCalculatorViewModel ^, CalculatorViewModel);
            OBSERVABLE_PROPERTY_RW(DateCalculatorViewModel ^, DateCalcViewModel);
            OBSERVABLE_PROPERTY_RW(GraphingCalculatorViewModel ^, GraphingCalcViewModel);
            OBSERVABLE_PROPERTY_RW(UnitConverterViewModel ^, ConverterViewModel);
            OBSERVABLE_PROPERTY_RW(CalculatorApp::ViewModel::Common::ViewMode, PreviousMode);
            OBSERVABLE_PROPERTY_R(bool, IsAlwaysOnTop);
            OBSERVABLE_NAMED_PROPERTY_RW(Platform::String ^, CategoryName);

            // Indicates whether calculator is currently in standard mode _and_ supports CompactOverlay _and_ is not in Always-on-Top mode
            OBSERVABLE_PROPERTY_R(bool, DisplayNormalAlwaysOnTopOption);

            COMMAND_FOR_METHOD(CopyCommand, ApplicationViewModel::OnCopyCommand);
            COMMAND_FOR_METHOD(PasteCommand, ApplicationViewModel::OnPasteCommand);

            property CalculatorApp::ViewModel::Common::ViewMode Mode
            {
                CalculatorApp::ViewModel::Common::ViewMode get()
                {
                    return m_mode;
                }

                void set(CalculatorApp::ViewModel::Common::ViewMode value);
            }
            static property Platform::String^ ModePropertyName
            {
                Platform::String^ get()
                {
                    return Platform::StringReference(L"Mode");
                }
            }

            property Windows::Foundation::Collections::IObservableVector<CalculatorApp::ViewModel::Common::NavCategoryGroup^>^ Categories
            {
                Windows::Foundation::Collections::IObservableVector<CalculatorApp::ViewModel::Common::NavCategoryGroup^>^ get()
                {
                    return m_categories;
                }

                void set(Windows::Foundation::Collections::IObservableVector<CalculatorApp::ViewModel::Common::NavCategoryGroup^>^ value);
            }

            property Windows::UI::Xaml::Visibility ClearMemoryVisibility
            {
                Windows::UI::Xaml::Visibility get()
                {
                    return CalculatorApp::ViewModel::Common::NavCategory::IsCalculatorViewMode(Mode) ? Windows::UI::Xaml::Visibility::Visible
                                                                                          : Windows::UI::Xaml::Visibility::Collapsed;
                }
            }

            static property Platform::String ^ LaunchedLocalSettings
            {
                Platform::String ^ get()
                {
                    return Platform::StringReference(L"calculatorAlwaysOnTopLaunched");
                }
            }

            static property Platform::String ^ WidthLocalSettings
            {
                Platform::String ^ get()
                {
                    return Platform::StringReference(L"calculatorAlwaysOnTopLastWidth");
                }
            }

            static property Platform::String ^ HeightLocalSettings
            {
                Platform::String ^ get()
                {
                    return Platform::StringReference(L"calculatorAlwaysOnTopLastHeight");
                }
            }

            void ToggleAlwaysOnTop(float width, float height);

        private:
            bool TryRecoverFromNavigationModeFailure();

            void OnModeChanged();

            void OnCopyCommand(Platform::Object ^ parameter);
            void OnPasteCommand(Platform::Object ^ parameter);

            void SetMenuCategories();

            CalculatorApp::ViewModel::Common::ViewMode m_mode;
            Windows::Foundation::Collections::IObservableVector<CalculatorApp::ViewModel::Common::NavCategoryGroup ^> ^ m_categories;
            Concurrency::task<void> HandleToggleAlwaysOnTop(float width, float height);
            void SetDisplayNormalAlwaysOnTopOption();
        };
    }
}
