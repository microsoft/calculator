// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "Common/Automation/NarratorAnnouncement.h"
#include "Common/DisplayExpressionToken.h"
#include "Common/CalculatorDisplay.h"
#include "Common/EngineResourceProvider.h"
#include "Common/CalculatorButtonUser.h"
#include "HistoryViewModel.h"
#include "MemoryItemViewModel.h"
#include "Common/BitLength.h"
#include "Common/NumberBase.h"

namespace CalculatorUnitTests
{
    class MultiWindowUnitTests;
}

namespace CalculatorApp
{
    namespace WS = Windows::System;
    namespace CM = CalculationManager;

    namespace ViewModel
    {
#define ASCII_0 48
        public delegate void HideMemoryClickedHandler();

        public value struct ButtonInfo
        {
            NumbersAndOperatorsEnum buttonId;
            bool canSendNegate;
        };

        [Windows::UI::Xaml::Data::Bindable] public ref class StandardCalculatorViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
        {
        public:
            StandardCalculatorViewModel();
            void UpdateOperand(int pos, Platform::String ^ text);

            OBSERVABLE_OBJECT_CALLBACK(OnPropertyChanged);
            OBSERVABLE_PROPERTY_RW(Platform::String ^, DisplayValue);
            OBSERVABLE_PROPERTY_R(HistoryViewModel ^, HistoryVM);
            OBSERVABLE_PROPERTY_RW(bool, IsAlwaysOnTop);
            OBSERVABLE_PROPERTY_R(bool, IsBinaryBitFlippingEnabled);
            PROPERTY_R(bool, IsOperandUpdatedUsingViewModel);
            PROPERTY_R(int, TokenPosition);
            PROPERTY_R(bool, IsOperandTextCompletelySelected);
            PROPERTY_R(bool, KeyPressed);
            PROPERTY_R(Platform::String ^, SelectedExpressionLastData);
            OBSERVABLE_NAMED_PROPERTY_R(bool, IsInError);
            OBSERVABLE_PROPERTY_R(bool, IsOperatorCommand);
            OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IObservableVector<Common::DisplayExpressionToken ^> ^, ExpressionTokens);
            OBSERVABLE_PROPERTY_R(Platform::String ^, DecimalDisplayValue);
            OBSERVABLE_PROPERTY_R(Platform::String ^, HexDisplayValue);
            OBSERVABLE_PROPERTY_R(Platform::String ^, OctalDisplayValue);
            OBSERVABLE_NAMED_PROPERTY_R(Platform::String ^, BinaryDisplayValue);
            OBSERVABLE_NAMED_PROPERTY_R(Windows::Foundation::Collections::IVector<bool> ^, BinaryDigits);
            OBSERVABLE_PROPERTY_R(Platform::String ^, HexDisplayValue_AutomationName);
            OBSERVABLE_PROPERTY_R(Platform::String ^, DecDisplayValue_AutomationName);
            OBSERVABLE_PROPERTY_R(Platform::String ^, OctDisplayValue_AutomationName);
            OBSERVABLE_PROPERTY_R(Platform::String ^, BinDisplayValue_AutomationName);
            OBSERVABLE_PROPERTY_R(bool, IsBinaryOperatorEnabled);
            OBSERVABLE_PROPERTY_R(bool, IsUnaryOperatorEnabled);
            OBSERVABLE_PROPERTY_R(bool, IsNegateEnabled);
            OBSERVABLE_PROPERTY_RW(bool, IsDecimalEnabled);
            OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IVector<MemoryItemViewModel ^> ^, MemorizedNumbers);
            OBSERVABLE_NAMED_PROPERTY_RW(bool, IsMemoryEmpty);
            OBSERVABLE_PROPERTY_R(bool, IsFToEChecked);
            OBSERVABLE_PROPERTY_R(bool, IsFToEEnabled);
            OBSERVABLE_PROPERTY_R(bool, AreHEXButtonsEnabled);
            OBSERVABLE_PROPERTY_R(Platform::String ^, CalculationResultAutomationName);
            OBSERVABLE_PROPERTY_R(Platform::String ^, CalculationExpressionAutomationName);
            OBSERVABLE_PROPERTY_R(bool, IsShiftProgrammerChecked);
            OBSERVABLE_PROPERTY_R(CalculatorApp::Common::NumberBase, CurrentRadixType);
            OBSERVABLE_PROPERTY_R(bool, AreTokensUpdated);
            OBSERVABLE_PROPERTY_R(bool, AreAlwaysOnTopResultsUpdated);
            OBSERVABLE_PROPERTY_R(bool, AreProgrammerRadixOperatorsVisible);
            OBSERVABLE_PROPERTY_R(bool, IsInputEmpty);
            OBSERVABLE_PROPERTY_R(CalculatorApp::Common::Automation::NarratorAnnouncement ^, Announcement);
            OBSERVABLE_PROPERTY_R(unsigned int, OpenParenthesisCount);

            COMMAND_FOR_METHOD(CopyCommand, StandardCalculatorViewModel::OnCopyCommand);
            COMMAND_FOR_METHOD(PasteCommand, StandardCalculatorViewModel::OnPasteCommand);
            COMMAND_FOR_METHOD(ButtonPressed, StandardCalculatorViewModel::OnButtonPressed);
            COMMAND_FOR_METHOD(ClearMemoryCommand, StandardCalculatorViewModel::OnClearMemoryCommand);
            COMMAND_FOR_METHOD(MemoryItemPressed, StandardCalculatorViewModel::OnMemoryItemPressed);
            COMMAND_FOR_METHOD(MemoryAdd, StandardCalculatorViewModel::OnMemoryAdd);
            COMMAND_FOR_METHOD(MemorySubtract, StandardCalculatorViewModel::OnMemorySubtract);

            event HideMemoryClickedHandler ^ HideMemoryClicked;

            property bool IsBitFlipChecked
            {
                bool get()
                {
                    return m_isBitFlipChecked;
                }
                void set(bool value)
                {
                    if (m_isBitFlipChecked != value)
                    {
                        m_isBitFlipChecked = value;
                        IsBinaryBitFlippingEnabled = IsProgrammer && m_isBitFlipChecked;
                        AreProgrammerRadixOperatorsVisible = IsProgrammer && !m_isBitFlipChecked;
                        RaisePropertyChanged(L"IsBitFlipChecked");
                    }
                }
            }
            static property Platform::String
                ^ IsBitFlipCheckedPropertyName { Platform::String ^ get() { return Platform::StringReference(L"IsBitFlipChecked"); } }

            property CalculatorApp::Common::BitLength ValueBitLength
            {
                CalculatorApp::Common::BitLength get()
                {
                    return m_valueBitLength;
                }
                void set(CalculatorApp::Common::BitLength value);
            }

            property bool IsStandard
            {
                bool get()
                {
                    return m_isStandard;
                }
                void set(bool value)
                {
                    if (m_isStandard != value)
                    {
                        m_isStandard = value;
                        if (value)
                        {
                            IsScientific = false;
                            IsProgrammer = false;
                        }
                        RaisePropertyChanged(L"IsStandard");
                    }
                }
            }

            property bool IsScientific
            {
                bool get()
                {
                    return m_isScientific;
                }
                void set(bool value)
                {
                    if (m_isScientific != value)
                    {
                        m_isScientific = value;
                        if (value)
                        {
                            IsStandard = false;
                            IsProgrammer = false;
                        }
                        RaisePropertyChanged(L"IsScientific");
                    }
                }
            }

            property bool IsProgrammer
            {
                bool get()
                {
                    return m_isProgrammer;
                }
                void set(bool value)
                {
                    if (m_isProgrammer != value)
                    {
                        m_isProgrammer = value;
                        if (!m_isProgrammer)
                        {
                            IsBitFlipChecked = false;
                        }
                        IsBinaryBitFlippingEnabled = m_isProgrammer && IsBitFlipChecked;
                        AreProgrammerRadixOperatorsVisible = m_isProgrammer && !IsBitFlipChecked;
                        if (value)
                        {
                            IsStandard = false;
                            IsScientific = false;
                        }
                        RaisePropertyChanged(L"IsProgrammer");
                    }
                }
            }
            static property Platform::String
                ^ IsProgrammerPropertyName { Platform::String ^ get() { return Platform::StringReference(L"IsProgrammer"); } }

            property bool IsEditingEnabled
            {
                bool get()
                {
                    return m_isEditingEnabled;
                }
                void set(bool value)
                {
                    if (m_isEditingEnabled != value)
                    {
                        m_isEditingEnabled = value;
                        bool currentEditToggleValue = !m_isEditingEnabled;
                        IsBinaryOperatorEnabled = currentEditToggleValue;
                        IsUnaryOperatorEnabled = currentEditToggleValue;
                        IsOperandEnabled = currentEditToggleValue;
                        IsNegateEnabled = currentEditToggleValue;
                        IsDecimalEnabled = currentEditToggleValue;
                        RaisePropertyChanged(L"IsEditingEnabled");
                    }
                }
            }

            property bool IsEngineRecording
            {
                bool get()
                {
                    return m_standardCalculatorManager.IsEngineRecording();
                }
            }

            property bool IsOperandEnabled
            {
                bool get()
                {
                    return m_isOperandEnabled;
                }
                void set(bool value)
                {
                    if (m_isOperandEnabled != value)
                    {
                        m_isOperandEnabled = value;
                        IsDecimalEnabled = value;
                        AreHEXButtonsEnabled = IsProgrammer;
                        IsFToEEnabled = value;
                        RaisePropertyChanged(L"IsOperandEnabled");
                    }
                }
            }

            // Used by unit tests
            void ResetCalcManager(bool clearMemory);
            void SendCommandToCalcManager(int command);

        internal:
            void OnPaste(Platform::String ^ pastedString);
            void OnCopyCommand(Platform::Object ^ parameter);
            void OnPasteCommand(Platform::Object ^ parameter);

            ButtonInfo MapCharacterToButtonId(char16 ch);

            // Memory feature related methods. They are internal because they need to called from the MainPage code-behind
            void OnMemoryButtonPressed();
            void OnMemoryItemPressed(Platform::Object ^ memoryItemPosition);
            void OnMemoryAdd(Platform::Object ^ memoryItemPosition);
            void OnMemorySubtract(Platform::Object ^ memoryItemPosition);
            void OnMemoryClear(_In_ Platform::Object ^ memoryItemPosition);

            void OnInputChanged();
            void DisplayPasteError();
            void SetParenthesisCount(_In_ unsigned int parenthesisCount);
            void SetOpenParenthesisCountNarratorAnnouncement();
            void OnNoRightParenAdded();
            void SetNoParenAddedNarratorAnnouncement();
            void OnMaxDigitsReached();
            void OnBinaryOperatorReceived();
            void OnMemoryItemChanged(unsigned int indexOfMemory);
            void SetBitshiftRadioButtonCheckedAnnouncement(Platform::String ^ announcement);

            Platform::String ^ GetLocalizedStringFormat(Platform::String ^ format, Platform::String ^ displayValue);
            void OnPropertyChanged(Platform::String ^ propertyname);
            void SetCalculatorType(CalculatorApp::Common::ViewMode targetState);

            Platform::String ^ GetRawDisplayValue();
            void Recalculate(bool fromHistory = false);
            bool IsOperator(CalculationManager::Command cmdenum);
            void FtoEButtonToggled();
            void SwitchProgrammerModeBase(CalculatorApp::Common::NumberBase calculatorBase);
            void SetMemorizedNumbersString();
            void SwitchAngleType(NumbersAndOperatorsEnum num);
            void ResetDisplay();
          
            void SetPrecision(int32_t precision);
            void UpdateMaxIntDigits()
            {
                m_standardCalculatorManager.UpdateMaxIntDigits();
            }
            NumbersAndOperatorsEnum GetCurrentAngleType()
            {
                return m_CurrentAngleType;
            }
            void SelectHistoryItem(HistoryItemViewModel ^ item);
        private:
            void SetMemorizedNumbers(const std::vector<std::wstring>& memorizedNumbers);
            void UpdateProgrammerPanelDisplay();
            void HandleUpdatedOperandData(CalculationManager::Command cmdenum);
            void SetPrimaryDisplay(_In_ Platform::String ^ displayStringValue, _In_ bool isError);
            void SetExpressionDisplay(
                _Inout_ std::shared_ptr<std::vector<std::pair<std::wstring, int>>> const& tokens,
                _Inout_ std::shared_ptr<std::vector<std::shared_ptr<IExpressionCommand>>> const& commands);
            void SetHistoryExpressionDisplay(
                _Inout_ std::shared_ptr<std::vector<std::pair<std::wstring, int>>> const& tokens,
                _Inout_ std::shared_ptr<std::vector<std::shared_ptr<IExpressionCommand>>> const& commands);
            void SetTokens(_Inout_ std::shared_ptr<std::vector<std::pair<std::wstring, int>>> const& tokens);
            NumbersAndOperatorsEnum ConvertIntegerToNumbersAndOperatorsEnum(unsigned int parameter);
            static RadixType GetRadixTypeFromNumberBase(CalculatorApp::Common::NumberBase base);
            NumbersAndOperatorsEnum m_CurrentAngleType;
            wchar_t m_decimalSeparator;
            CalculatorDisplay m_calculatorDisplay;
            CalculatorApp::EngineResourceProvider m_resourceProvider;
            CalculationManager::CalculatorManager m_standardCalculatorManager;
            Platform::String ^ m_expressionAutomationNameFormat;
            Platform::String ^ m_localizedCalculationResultAutomationFormat;
            Platform::String ^ m_localizedCalculationResultDecimalAutomationFormat;
            Platform::String ^ m_localizedHexaDecimalAutomationFormat;
            Platform::String ^ m_localizedDecimalAutomationFormat;
            Platform::String ^ m_localizedOctalAutomationFormat;
            Platform::String ^ m_localizedBinaryAutomationFormat;
            Platform::String ^ m_localizedMaxDigitsReachedAutomationFormat;
            Platform::String ^ m_localizedButtonPressFeedbackAutomationFormat;
            Platform::String ^ m_localizedMemorySavedAutomationFormat;
            Platform::String ^ m_localizedMemoryItemChangedAutomationFormat;
            Platform::String ^ m_localizedMemoryItemClearedAutomationFormat;
            Platform::String ^ m_localizedMemoryCleared;
            Platform::String ^ m_localizedOpenParenthesisCountChangedAutomationFormat;
            Platform::String ^ m_localizedNoRightParenthesisAddedFormat;

            bool m_isOperandEnabled;
            bool m_isEditingEnabled;
            bool m_isStandard;
            bool m_isScientific;
            bool m_isProgrammer;
            bool m_isBitFlipChecked;
            bool m_isRtlLanguage;
            bool m_operandUpdated;
            bool m_isLastOperationHistoryLoad;
            CalculatorApp::Common::BitLength m_valueBitLength;
            Platform::String ^ m_selectedExpressionLastData;
            Common::DisplayExpressionToken ^ m_selectedExpressionToken;

            Platform::String ^ LocalizeDisplayValue(_In_ std::wstring const& displayValue);
            Platform::String
                ^ CalculateNarratorDisplayValue(_In_ std::wstring const& displayValue, _In_ Platform::String ^ localizedDisplayValue);
            CalculatorApp::Common::Automation::NarratorAnnouncement ^ GetDisplayUpdatedNarratorAnnouncement();
            Platform::String ^ GetCalculatorExpressionAutomationName();
            Platform::String ^ GetNarratorStringReadRawNumbers(_In_ Platform::String ^ localizedDisplayValue);

            CalculationManager::Command ConvertToOperatorsEnum(NumbersAndOperatorsEnum operation);
            void DisableButtons(CalculationManager::CommandType selectedExpressionCommandType);

            Platform::String ^ m_feedbackForButtonPress;
            void OnButtonPressed(Platform::Object ^ parameter);
            void OnClearMemoryCommand(Platform::Object ^ parameter);
            std::wstring AddPadding(std::wstring);
            size_t LengthWithoutPadding(std::wstring);

            std::shared_ptr<std::vector<std::pair<std::wstring, int>>> m_tokens;
            std::shared_ptr<std::vector<std::shared_ptr<IExpressionCommand>>> m_commands;

            // Token types
            bool IsUnaryOp(CalculationManager::Command command);
            bool IsBinOp(CalculationManager::Command command);
            bool IsTrigOp(CalculationManager::Command command);
            bool IsOpnd(CalculationManager::Command command);
            bool IsRecoverableCommand(CalculationManager::Command command);

            void SaveEditedCommand(_In_ unsigned int index, _In_ CalculationManager::Command command);

            CalculatorApp::Common::ViewMode GetCalculatorMode();

            friend class CalculatorDisplay;
            friend class CalculatorUnitTests::MultiWindowUnitTests;
        };
    }
}
