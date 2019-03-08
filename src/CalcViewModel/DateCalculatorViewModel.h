// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Common/Utils.h"
#include "Common/DateCalculator.h"

const int c_maxOffsetValue = 999;

namespace CalculatorApp
{
    namespace ViewModel
    {
        [Windows::UI::Xaml::Data::Bindable]
        public ref class DateCalculatorViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
        {
        public:
            DateCalculatorViewModel();

            OBSERVABLE_OBJECT_CALLBACK(OnPropertyChanged);

            // Input Properties
            OBSERVABLE_PROPERTY_RW(bool, IsDateDiffMode);
            OBSERVABLE_PROPERTY_RW(bool, IsAddMode);
            OBSERVABLE_PROPERTY_RW(bool, IsDiffInDays);     // If diff is only in days or the dates are the same,
                                                            // then show only one result and avoid redundancy

            OBSERVABLE_PROPERTY_RW(int, DaysOffset);
            OBSERVABLE_PROPERTY_RW(int, MonthsOffset);
            OBSERVABLE_PROPERTY_RW(int, YearsOffset);

            // Read only property for offset values
            property Windows::Foundation::Collections::IVector<Platform::String^>^ OffsetValues
            {
                Windows::Foundation::Collections::IVector<Platform::String^>^ get() { return m_offsetValues; }
            }

            // From date for Date Diff
            property Windows::Foundation::DateTime FromDate
            {
                Windows::Foundation::DateTime get() { return m_fromDate; }

                void set(Windows::Foundation::DateTime value)
                {
                    if (m_fromDate.UniversalTime != value.UniversalTime)
                    {
                        m_fromDate = value;
                        RaisePropertyChanged("FromDate");
                    }
                }
            }

            // To date for Date Diff
            property Windows::Foundation::DateTime ToDate
            {
                Windows::Foundation::DateTime get() { return m_toDate; }

                void set(Windows::Foundation::DateTime value)
                {
                    if (m_toDate.UniversalTime != value.UniversalTime)
                    {
                        m_toDate = value;
                        RaisePropertyChanged("ToDate");
                    }
                }
            }

            // Start date for Add/Subtract date
            property Windows::Foundation::DateTime StartDate
            {
                Windows::Foundation::DateTime get() { return m_startDate; }

                void set(Windows::Foundation::DateTime value)
                {
                    if (m_startDate.UniversalTime != value.UniversalTime)
                    {
                        m_startDate = value;
                        RaisePropertyChanged("StartDate");
                    }
                }
            }

            // Output Properties
            OBSERVABLE_PROPERTY_RW(Platform::String^, StrDateDiffResult);
            OBSERVABLE_PROPERTY_RW(Platform::String^, StrDateDiffResultAutomationName);
            OBSERVABLE_PROPERTY_RW(Platform::String^, StrDateDiffResultInDays);
            OBSERVABLE_PROPERTY_RW(Platform::String^, StrDateResult);
            OBSERVABLE_PROPERTY_RW(Platform::String^, StrDateResultAutomationName);

            COMMAND_FOR_METHOD(CopyCommand, DateCalculatorViewModel::OnCopyCommand);

            void OnCopyCommand(Platform::Object^ parameter);

        private:
            void OnPropertyChanged(_In_ Platform::String^ prop);
            void OnInputsChanged();
            void UpdateDisplayResult();
            void UpdateStrDateDiffResultAutomationName();
            void UpdateStrDateResultAutomationName();
            void InitializeDateOutputFormats(Platform::String^ calendarIdentifier);
            Platform::String^ GetDateDiffString() const;
            Platform::String^ GetDateDiffStringInDays() const;
            Platform::String^ GetLocalizedNumberString(int value) const;
            static Windows::Foundation::DateTime ClipTime(Windows::Foundation::DateTime dateTime);

            static void CheckClipTimeSameDay(Windows::Globalization::Calendar^ reference);

            property bool IsOutOfBound
            {
                bool get() { return m_isOutOfBound; }
                void set(bool value) { m_isOutOfBound = value; UpdateDisplayResult(); }
            }

            property CalculatorApp::Common::DateCalculation::DateDifference DateDiffResult
            {
                CalculatorApp::Common::DateCalculation::DateDifference get() { return m_dateDiffResult; }
                void set(CalculatorApp::Common::DateCalculation::DateDifference value) { m_dateDiffResult = value; UpdateDisplayResult(); }
            }

            property CalculatorApp::Common::DateCalculation::DateDifference DateDiffResultInDays
            {
                CalculatorApp::Common::DateCalculation::DateDifference get() { return m_dateDiffResultInDays; }
                void set(CalculatorApp::Common::DateCalculation::DateDifference value) { m_dateDiffResultInDays = value; UpdateDisplayResult(); }
            }

            property Windows::Foundation::DateTime DateResult
            {
                Windows::Foundation::DateTime get() { return m_dateResult; }
                void set(Windows::Foundation::DateTime value) { m_dateResult = value; UpdateDisplayResult();}
            }

        private:
            // Property variables
            bool m_isOutOfBound;
            Platform::Collections::Vector<Platform::String^>^ m_offsetValues;
            Windows::Foundation::DateTime m_fromDate;
            Windows::Foundation::DateTime m_toDate;
            Windows::Foundation::DateTime m_startDate;
            Windows::Foundation::DateTime m_dateResult;
            CalculatorApp::Common::DateCalculation::DateDifference m_dateDiffResult;
            CalculatorApp::Common::DateCalculation::DateDifference m_dateDiffResultInDays;

            // Private members
            std::shared_ptr<CalculatorApp::Common::DateCalculation::DateCalculationEngine> m_dateCalcEngine;
            CalculatorApp::Common::DateCalculation::DateUnit m_daysOutputFormat;
            CalculatorApp::Common::DateCalculation::DateUnit m_allDateUnitsOutputFormat;
            Windows::Globalization::DateTimeFormatting::DateTimeFormatter^ m_dateTimeFormatter;
            Platform::String^ m_listSeparator;
        };
    }
}
