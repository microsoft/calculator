// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcManager/UnitConverter.h"
#include "Common/Utils.h"
#include "Common/NetworkManager.h"
#include "Common/Automation/NarratorAnnouncement.h"
#include "Common/CalculatorButtonUser.h"
#include "Common/NavCategory.h"

namespace CalculatorApp
{
    namespace ViewModel
    {
        [Windows::UI::Xaml::Data::Bindable] public ref class Category sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
        {
            internal : Category(const UnitConversionManager::Category& category)
                : m_original(category)
            {
            }

        public:
            OBSERVABLE_OBJECT();

            property Platform::String
                ^ Name { Platform::String ^ get() { return ref new Platform::String(m_original.name.c_str()); } }

                property Windows::UI::Xaml::Visibility NegateVisibility
            {
                Windows::UI::Xaml::Visibility get()
                {
                    return m_original.supportsNegative ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
                }
            }

            internal : const UnitConversionManager::Category& GetModelCategory() const
            {
                return m_original;
            }

        private:
            const UnitConversionManager::Category m_original;
        };

        [Windows::UI::Xaml::Data::Bindable] public ref class Unit sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
        {
            internal : Unit(const UnitConversionManager::Unit& unit)
                : m_original(unit)
            {
            }

        public:
            OBSERVABLE_OBJECT();

            property Platform::String
                ^ Name { Platform::String ^ get() { return ref new Platform::String(m_original.name.c_str()); } }

                property Platform::String
                ^ AccessibleName { Platform::String ^ get() { return ref new Platform::String(m_original.accessibleName.c_str()); } }

                property Platform::String
                ^ Abbreviation { Platform::String ^ get() { return ref new Platform::String(m_original.abbreviation.c_str()); } }

                // This method is used to return the desired automation name for default unit in UnitConverter combo box.
                Platform::String
                ^ ToString() override
            {
                return AccessibleName;
            }

            internal : const UnitConversionManager::Unit& GetModelUnit() const
            {
                return m_original;
            }

        private:
            const UnitConversionManager::Unit m_original;
        };

        [Windows::UI::Xaml::Data::Bindable] public ref class SupplementaryResult sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
        {
            internal : SupplementaryResult(Platform::String ^ value, Unit ^ unit)
                : m_Value(value)
                , m_Unit(unit)
            {
            }

            bool IsWhimsical() const
            {
                return m_Unit->GetModelUnit().isWhimsical;
            }

            Platform::String ^ GetLocalizedAutomationName();

        public:
            OBSERVABLE_OBJECT();

            OBSERVABLE_PROPERTY_R(Platform::String ^, Value);
            OBSERVABLE_PROPERTY_R(CalculatorApp::ViewModel::Unit ^, Unit);
        };

        interface class IActivatable
        {
            virtual property bool IsActive;
        };

        template <typename TActivatable>
        ref class Activatable sealed : public IActivatable
        {
        private:
            TActivatable m_activatable;

        public:
            Activatable(TActivatable activatable)
                : m_activatable(activatable)
            {
            }

            virtual property bool IsActive
            {
                bool get()
                {
                    return m_activatable->IsActive;
                }
                void set(bool value)
                {
                    m_activatable->IsActive = value;
                }
            }
        };

        template <typename TActivatable>
            IActivatable
            ^ AsActivatable(TActivatable activatable) { return ref new Activatable<TActivatable>(activatable); }

                [Windows::UI::Xaml::Data::Bindable] public ref class UnitConverterViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
        {
            internal : UnitConverterViewModel(const std::shared_ptr<UnitConversionManager::IUnitConverter>& model);

        public:
            OBSERVABLE_OBJECT_CALLBACK(OnPropertyChanged);

            OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IObservableVector<Category ^> ^, Categories);
            OBSERVABLE_PROPERTY_RW(CalculatorApp::Common::ViewMode, Mode);
            OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IObservableVector<Unit ^> ^, Units);
            OBSERVABLE_PROPERTY_RW(Platform::String ^, CurrencySymbol1);
            OBSERVABLE_PROPERTY_RW(Unit ^, Unit1);
            OBSERVABLE_PROPERTY_RW(Platform::String ^, Value1);
            OBSERVABLE_PROPERTY_RW(Platform::String ^, CurrencySymbol2);
            OBSERVABLE_PROPERTY_RW(Unit ^, Unit2);
            OBSERVABLE_PROPERTY_RW(Platform::String ^, Value2);
            OBSERVABLE_NAMED_PROPERTY_R(Windows::Foundation::Collections::IObservableVector<SupplementaryResult ^> ^, SupplementaryResults);
            OBSERVABLE_PROPERTY_RW(bool, Value1Active);
            OBSERVABLE_PROPERTY_RW(bool, Value2Active);
            OBSERVABLE_PROPERTY_RW(Platform::String ^, Value1AutomationName);
            OBSERVABLE_PROPERTY_RW(Platform::String ^, Value2AutomationName);
            OBSERVABLE_PROPERTY_RW(Platform::String ^, Unit1AutomationName);
            OBSERVABLE_PROPERTY_RW(Platform::String ^, Unit2AutomationName);
            OBSERVABLE_PROPERTY_RW(CalculatorApp::Common::Automation::NarratorAnnouncement ^, Announcement);
            OBSERVABLE_PROPERTY_RW(bool, IsDecimalEnabled);
            OBSERVABLE_PROPERTY_RW(bool, IsDropDownOpen);
            OBSERVABLE_PROPERTY_RW(bool, IsDropDownEnabled);
            OBSERVABLE_NAMED_PROPERTY_RW(bool, IsCurrencyLoadingVisible);
            OBSERVABLE_NAMED_PROPERTY_R(bool, IsCurrencyCurrentCategory);
            OBSERVABLE_PROPERTY_RW(Platform::String ^, CurrencyRatioEquality);
            OBSERVABLE_PROPERTY_RW(Platform::String ^, CurrencyRatioEqualityAutomationName);
            OBSERVABLE_PROPERTY_RW(Platform::String ^, CurrencyTimestamp);
            OBSERVABLE_NAMED_PROPERTY_RW(CalculatorApp::NetworkAccessBehavior, NetworkBehavior);
            OBSERVABLE_NAMED_PROPERTY_RW(bool, CurrencyDataLoadFailed);
            OBSERVABLE_NAMED_PROPERTY_RW(bool, CurrencyDataIsWeekOld);

        public:
            property Category ^ CurrentCategory
            {
                Category ^ get() { return m_CurrentCategory; }
                void set(Category ^ value)
                {
                    if (m_CurrentCategory == value)
                    {
                        return;
                    }
                    m_CurrentCategory = value;
                    if (value != nullptr)
                    {
                        auto currentCategory = value->GetModelCategory();
                        IsCurrencyCurrentCategory = currentCategory.id == CalculatorApp::Common::NavCategory::Serialize(CalculatorApp::Common::ViewMode::Currency);
                    }
                    RaisePropertyChanged("CurrentCategory");
                }
            }

            property Windows::UI::Xaml::Visibility SupplementaryVisibility
            {
                Windows::UI::Xaml::Visibility get()
                {
                    return SupplementaryResults->Size > 0 ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
                }
            }

            property Windows::UI::Xaml::Visibility CurrencySymbolVisibility
            {
                Windows::UI::Xaml::Visibility get()
                {
                    return (CurrencySymbol1->IsEmpty() || CurrencySymbol2->IsEmpty()) ? Windows::UI::Xaml::Visibility::Collapsed
                                                                                      : Windows::UI::Xaml::Visibility::Visible;
                }
            }

            COMMAND_FOR_METHOD(CategoryChanged, UnitConverterViewModel::OnCategoryChanged);
            COMMAND_FOR_METHOD(UnitChanged, UnitConverterViewModel::OnUnitChanged);
            COMMAND_FOR_METHOD(SwitchActive, UnitConverterViewModel::OnSwitchActive);
            COMMAND_FOR_METHOD(ButtonPressed, UnitConverterViewModel::OnButtonPressed);
            COMMAND_FOR_METHOD(CopyCommand, UnitConverterViewModel::OnCopyCommand);
            COMMAND_FOR_METHOD(PasteCommand, UnitConverterViewModel::OnPasteCommand);

            void AnnounceConversionResult();

            internal : void ResetView();
            void PopulateData();
            NumbersAndOperatorsEnum MapCharacterToButtonId(const wchar_t ch, bool& canSendNegate);
            void DisplayPasteError();
            void OnValueActivated(IActivatable ^ control);
            void OnPaste(Platform::String ^ stringToPaste);

            void OnCopyCommand(Platform::Object ^ parameter);
            void OnPasteCommand(Platform::Object ^ parameter);

            enum class CurrencyFormatterParameter
            {
                Default,
                ForValue1,
                ForValue2,
            };

            Platform::String
                ^ GetLocalizedAutomationName(
                    _In_ Platform::String ^ displayvalue,
                    _In_ Platform::String ^ unitname,
                    _In_ Platform::String ^ format,
                    _In_ CurrencyFormatterParameter cfp);
            Platform::String
                ^ GetLocalizedConversionResultStringFormat(
                    _In_ Platform::String ^ fromValue,
                    _In_ Platform::String ^ fromUnit,
                    _In_ Platform::String ^ toValue,
                    _In_ Platform::String ^ toUnit);
            void UpdateValue1AutomationName();
            void UpdateValue2AutomationName();

            // Saving And Restoring User Preferences of Category and Associated-Units across Sessions.
            void SaveUserPreferences();
            void RestoreUserPreferences();

            void OnCurrencyDataLoadFinished(bool didLoad);
            void OnCurrencyTimestampUpdated(_In_ const std::wstring& timestamp, bool isWeekOld);
            void RefreshCurrencyRatios();
            void OnNetworkBehaviorChanged(_In_ CalculatorApp::NetworkAccessBehavior newBehavior);

            const std::wstring& GetValueFromUnlocalized() const
            {
                return m_valueFromUnlocalized;
            }
            const std::wstring& GetValueToUnlocalized() const
            {
                return m_valueToUnlocalized;
            }

            // used by UnitConverterVMCallback
            void UpdateDisplay(const std::wstring& from, const std::wstring& to);
            void UpdateSupplementaryResults(const std::vector<std::tuple<std::wstring, UnitConversionManager::Unit>>& suggestedValues);
            void OnMaxDigitsReached();

            void BuildUnitList(const std::vector<UnitConversionManager::Unit>& modelUnitList);
            Unit ^ FindUnitInList(UnitConversionManager::Unit target);
            void SetSelectedUnits();

        private:
            void InitializeView();
            void OnPropertyChanged(Platform::String ^ prop);
            void OnCategoryChanged(Platform::Object ^ unused);
            void OnUnitChanged(Platform::Object ^ unused);
            void OnSwitchActive(Platform::Object ^ unused);
            UnitConversionManager::Command CommandFromButtonId(CalculatorApp::NumbersAndOperatorsEnum button);
            void SupplementaryResultsTimerTick(Windows::System::Threading::ThreadPoolTimer ^ timer);
            void SupplementaryResultsTimerCancel(Windows::System::Threading::ThreadPoolTimer ^ timer);
            void RefreshSupplementaryResults();
            void UpdateInputBlocked(_In_ const std::wstring& currencyInput);
            void UpdateCurrencyFormatter();
            void UpdateIsDecimalEnabled();
            bool UnitsAreValid();
            void ResetCategory();

            void OnButtonPressed(Platform::Object ^ parameter);
            Platform::String ^ ConvertToLocalizedString(const std::wstring& stringToLocalize, bool allowPartialStrings, CurrencyFormatterParameter cfp);

            std::shared_ptr<UnitConversionManager::IUnitConverter> m_model;
            wchar_t m_decimalSeparator;

            enum class ConversionParameter
            {
                Source,
                Target
            } m_value1cp;
            property CurrencyFormatterParameter CurrencyFormatterParameterFrom
            {
                CurrencyFormatterParameter get()
                {
                    return m_value1cp == ConversionParameter::Source ? CurrencyFormatterParameter::ForValue1 : CurrencyFormatterParameter::ForValue2;
                }
            }
            property CurrencyFormatterParameter CurrencyFormatterParameterTo
            {
                CurrencyFormatterParameter get()
                {
                    return m_value1cp == ConversionParameter::Target ? CurrencyFormatterParameter::ForValue1 : CurrencyFormatterParameter::ForValue2;
                }
            }
            property Windows::Globalization::NumberFormatting::CurrencyFormatter^ CurrencyFormatterFrom
            {
                Windows::Globalization::NumberFormatting::CurrencyFormatter^ get()
                {
                    return m_value1cp == ConversionParameter::Source ? m_currencyFormatter1 : m_currencyFormatter2;
                }
            }
            property Windows::Globalization::NumberFormatting::CurrencyFormatter^ CurrencyFormatterTo
            {
                Windows::Globalization::NumberFormatting::CurrencyFormatter^ get()
                {
                    return m_value1cp == ConversionParameter::Target ? m_currencyFormatter1 : m_currencyFormatter2;
                }
            }
            property Platform::String^ ValueFrom
            {
                Platform::String^ get() { return m_value1cp == ConversionParameter::Source ? Value1 : Value2; }
                void set(Platform::String^ value) { m_value1cp == ConversionParameter::Source ? Value1 = value : Value2 = value; }
            }
            property Unit^ UnitFrom
            {
                Unit^ get() { return m_value1cp == ConversionParameter::Source ? Unit1 : Unit2; }
                void set(Unit^ value) { m_value1cp == ConversionParameter::Source ? Unit1 = value : Unit2 = value; }
            }
            property Platform::String^ ValueTo
            {
                Platform::String^ get() { return m_value1cp == ConversionParameter::Target ? Value1 : Value2; }
                void set(Platform::String^ value) { m_value1cp == ConversionParameter::Target ? Value1 = value : Value2 = value; }
            }
            property Unit^ UnitTo
            {
                Unit^ get() { return m_value1cp == ConversionParameter::Target ? Unit1 : Unit2; }
                void set(Unit^ value) { m_value1cp == ConversionParameter::Target ? Unit1 = value : Unit2 = value; }
            }
            void SwitchConversionParameters()
            {
                m_value1cp = m_value1cp == ConversionParameter::Source ? ConversionParameter::Target : ConversionParameter::Source;
            }

        private:
            bool m_isInputBlocked;
            Windows::System::Threading::ThreadPoolTimer ^ m_supplementaryResultsTimer;
            bool m_resettingTimer;
            std::vector<std::tuple<std::wstring, UnitConversionManager::Unit>> m_cachedSuggestedValues;
            std::mutex m_cacheMutex;
            Windows::Globalization::NumberFormatting::DecimalFormatter ^ m_decimalFormatter;
            Windows::Globalization::NumberFormatting::CurrencyFormatter ^ m_currencyFormatter;
            Windows::Globalization::NumberFormatting::CurrencyFormatter ^ m_currencyFormatter1;
            Windows::Globalization::NumberFormatting::CurrencyFormatter ^ m_currencyFormatter2;
            std::wstring m_valueFromUnlocalized;
            std::wstring m_valueToUnlocalized;
            bool m_relocalizeStringOnSwitch;

            Platform::String ^ m_localizedValueFromFormat;
            Platform::String ^ m_localizedValueFromDecimalFormat;
            Platform::String ^ m_localizedValueToFormat;
            Platform::String ^ m_localizedConversionResultFormat;
            Platform::String ^ m_localizedInputUnitName;
            Platform::String ^ m_localizedOutputUnitName;

            bool m_isValue1Updating;
            bool m_isValue2Updating;
            std::wstring m_lastAnnouncedFrom;
            std::wstring m_lastAnnouncedTo;
            Platform::String ^ m_lastAnnouncedConversionResult;
            Category ^ m_CurrentCategory;
            bool m_isCurrencyDataLoaded;
        };

        class UnitConverterVMCallback : public UnitConversionManager::IUnitConverterVMCallback
        {
        public:
            UnitConverterVMCallback(UnitConverterViewModel ^ viewModel)
                : m_viewModel(viewModel)
            {
            }

            void DisplayCallback(const std::wstring& from, const std::wstring& to) override
            {
                m_viewModel->UpdateDisplay(from, to);
            }

            void SuggestedValueCallback(const std::vector<std::tuple<std::wstring, UnitConversionManager::Unit>>& suggestedValues) override
            {
                m_viewModel->UpdateSupplementaryResults(suggestedValues);
            }

            void MaxDigitsReached()
            {
                m_viewModel->OnMaxDigitsReached();
            }

        private:
            UnitConverterViewModel ^ m_viewModel;
        };

        class ViewModelCurrencyCallback : public UnitConversionManager::IViewModelCurrencyCallback
        {
        public:
            ViewModelCurrencyCallback(UnitConverterViewModel ^ viewModel)
                : m_viewModel(viewModel)
            {
            }

            void CurrencyDataLoadFinished(bool didLoad) override
            {
                m_viewModel->OnCurrencyDataLoadFinished(didLoad);
            }

            void CurrencySymbolsCallback(const std::wstring& symbol1, const std::wstring& symbol2) override
            {
                Platform::String ^ sym1 = Platform::StringReference(symbol1.c_str());
                Platform::String ^ sym2 = Platform::StringReference(symbol2.c_str());

                bool value1Active = m_viewModel->Value1Active;
                m_viewModel->CurrencySymbol1 = value1Active ? sym1 : sym2;
                m_viewModel->CurrencySymbol2 = value1Active ? sym2 : sym1;
            }

            void CurrencyRatiosCallback(_In_ const std::wstring& ratioEquality, _In_ const std::wstring& accRatioEquality) override
            {
                m_viewModel->CurrencyRatioEquality = ref new Platform::String(ratioEquality.c_str());
                m_viewModel->CurrencyRatioEqualityAutomationName = ref new Platform::String(accRatioEquality.c_str());
            }

            void CurrencyTimestampCallback(_In_ const std::wstring& timestamp, bool isWeekOld) override
            {
                m_viewModel->OnCurrencyTimestampUpdated(timestamp, isWeekOld);
            }

            void NetworkBehaviorChanged(_In_ int newBehavior) override
            {
                m_viewModel->OnNetworkBehaviorChanged(static_cast<CalculatorApp::NetworkAccessBehavior>(newBehavior));
            }

        private:
            UnitConverterViewModel ^ m_viewModel;
        };
    }
}
