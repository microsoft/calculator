// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace Numbers
{
    namespace DesignData
    {
#ifdef _DEBUG

        public ref class MemorySlot sealed: public Windows::UI::Xaml::Data::INotifyPropertyChanged
        {
        public:
            MemorySlot(int slotPosition, Platform::String^ value) :
                m_SlotPosition(slotPosition),
                m_SlotValue(value)
            {}

            OBSERVABLE_OBJECT();
            OBSERVABLE_PROPERTY_RW(int, SlotPosition);
            OBSERVABLE_PROPERTY_RW(Platform::String^, SlotValue);
        };

        // This class is to be consumed exclusively by Blend and the VS designer
        // with this class Blend will be able to populate the controls, the CalculationResults control for example,
        // with the hardcoded strings so whoever is working on the UI can actually see how the app would look like 
        // with semi-realistic data.
        // This data is to only be compiled in the debug build and it will not affect app functionality at all 
        // so it does not need to be tested. It will have to be kept in sync with StandardCalculatorViewModel though
        // to ensure that the design experience is correct.
        // This class's code is run in the designer process so the less code it has the better. 
        public ref class StandardCalculatorViewModel sealed: public Windows::UI::Xaml::Data::INotifyPropertyChanged
        {
        public:

            StandardCalculatorViewModel():
                m_DisplayValue("1234569"),
                m_DisplayStringExpression("14560 x 1890"),
                m_DegreeButtonContent("Deg"),
                m_IsMemoryEmpty(false)
            {
                m_MemorizedNumbers = ref new Platform::Collections::Vector<MemorySlot^>();
                for (int i = 1000; i < 1100; i++)
                {
                    wchar_t wzi[5];
                    _itow_s(i, wzi, 10);
                    m_MemorizedNumbers->Append(ref new MemorySlot(i, ref new Platform::String(wzi)));
                }
            }

            typedef Windows::Foundation::Collections::IMap<NumbersAndOperatorsEnum, bool> KeyboardButtonStates;

            OBSERVABLE_OBJECT();

            OBSERVABLE_PROPERTY_RW(Platform::String^, DisplayValue);
            OBSERVABLE_PROPERTY_RW(Platform::String^, DisplayStringExpression);
            OBSERVABLE_PROPERTY_RW(Platform::String^, DegreeButtonContent);
            OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IVector<MemorySlot^>^, MemorizedNumbers);
            OBSERVABLE_PROPERTY_RW(bool, IsMemoryEmpty);

            OBSERVABLE_PROPERTY_RW(KeyboardButtonStates^, PressedButtons);

            COMMAND_FOR_METHOD(ButtonPressed, StandardCalculatorViewModel::OnButtonPressed);

        private:

            void OnButtonPressed(Platform::Object^ parameter)
            { }

        };

#endif
    }
}

