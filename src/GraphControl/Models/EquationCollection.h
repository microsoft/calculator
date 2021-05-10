// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Equation.h"

namespace GraphControl
{
    delegate void EquationChangedEventHandler(Equation ^ sender);
    delegate void VisibilityChangedEventHandler(Equation ^ sender);

public
    ref class EquationCollection sealed : public Windows::Foundation::Collections::IObservableVector<GraphControl::Equation ^>
    {
    public:
        virtual ~EquationCollection()
        {
        }

#pragma region IIterable
        virtual Windows::Foundation::Collections::IIterator< GraphControl::Equation^ >^ First()
        {
            return m_vector->First();
        }
#pragma endregion

#pragma region IVector
        virtual property unsigned int Size
        {
            unsigned int get()
            {
                return m_vector->Size;
            }
        }

        virtual void Append(GraphControl::Equation ^ value)
        {
            m_vector->Append(value);
            m_tokens.emplace_back(
                value->PropertyChanged += ref new Windows::UI::Xaml::Data::PropertyChangedEventHandler(this, &EquationCollection::OnEquationPropertyChanged));
        }

        virtual void Clear()
        {
            auto numEqs = m_vector->Size;
            for (auto i = 0u; i < numEqs; i++)
            {
                m_vector->GetAt(i)->PropertyChanged -= m_tokens[i];
            }

            m_vector->Clear();
            m_tokens.clear();
        }

        virtual GraphControl::Equation
            ^ GetAt(unsigned int index) { return m_vector->GetAt(index); }

            virtual unsigned int GetMany(unsigned int startIndex, Platform::WriteOnlyArray<GraphControl::Equation ^> ^ items)
        {
            return m_vector->GetMany(startIndex, items);
        }

        virtual Windows::Foundation::Collections::IVectorView< GraphControl::Equation^ >^ GetView()
        {
            return m_vector->GetView();
        }

        virtual Platform::Boolean IndexOf(GraphControl::Equation^ value, unsigned int *index)
        {
            return m_vector->IndexOf(value, index);
        }

        virtual void InsertAt(unsigned int index, GraphControl::Equation ^ value)
        {
            m_vector->InsertAt(index, value);
            m_tokens.insert(
                m_tokens.begin() + index,
                value->PropertyChanged += ref new Windows::UI::Xaml::Data::PropertyChangedEventHandler(this, &EquationCollection::OnEquationPropertyChanged));
        }

        virtual void RemoveAt(unsigned int index)
        {
            m_vector->GetAt(index)->PropertyChanged -= m_tokens[index];

            m_vector->RemoveAt(index);
            m_tokens.erase(m_tokens.begin() + index);
        }

        virtual void RemoveAtEnd()
        {
            auto size = m_vector->Size;
            if (size > 0)
            {
                m_vector->GetAt(size - 1)->PropertyChanged -= *m_tokens.rbegin();
                m_tokens.erase(m_tokens.end() - 1);
            }

            m_vector->RemoveAtEnd();
        }

        virtual void ReplaceAll(const Platform::Array<GraphControl::Equation ^> ^ items)
        {
            auto size = m_vector->Size;
            for (auto i = 0u; i < size; i++)
            {
                m_vector->GetAt(i)->PropertyChanged -= m_tokens[i];
            }

            size = items->Length;
            m_tokens.resize(size);
            for (auto i = 0u; i < size; i++)
            {
                m_tokens[i] = items[i]->PropertyChanged +=
                    ref new Windows::UI::Xaml::Data::PropertyChangedEventHandler(this, &EquationCollection::OnEquationPropertyChanged);
            }

            m_vector->ReplaceAll(items);
        }

        virtual void SetAt(unsigned int index, GraphControl::Equation ^ value)
        {
            m_vector->GetAt(index)->PropertyChanged -= m_tokens[index];

            m_vector->SetAt(index, value);
            m_tokens[index] = value->PropertyChanged +=
                ref new Windows::UI::Xaml::Data::PropertyChangedEventHandler(this, &EquationCollection::OnEquationPropertyChanged);
        }
#pragma endregion

#pragma region IObservableVector
        virtual event Windows::Foundation::Collections::VectorChangedEventHandler< GraphControl::Equation^ >^ VectorChanged
        {
            Windows::Foundation::EventRegistrationToken add(Windows::Foundation::Collections::VectorChangedEventHandler< GraphControl::Equation^ >^ handler)
            {
                return m_vector->VectorChanged += handler;
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                m_vector->VectorChanged -= token;
            }
        }
#pragma endregion

    internal:
        EquationCollection() :
            m_vector(ref new Platform::Collections::Vector< GraphControl::Equation^ >())
        {
        }

        event EquationChangedEventHandler ^ EquationChanged;
        event EquationChangedEventHandler ^ EquationStyleChanged;
        event EquationChangedEventHandler ^ EquationLineEnabledChanged;

    private:
        void OnEquationPropertyChanged(Object ^ sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs ^ args)
        {
            auto equation = static_cast<Equation ^>(sender);
            auto propertyName = args->PropertyName;
            if (propertyName == GraphControl::Equation::LineColorPropertyName || propertyName == GraphControl::Equation::IsSelectedPropertyName
                || propertyName == GraphControl::Equation::EquationStylePropertyName)
            {
                EquationStyleChanged(equation);
            }
            else if (propertyName == GraphControl::Equation::ExpressionPropertyName)
            {
                EquationChanged(equation);
            }
            else if (propertyName == GraphControl::Equation::IsLineEnabledPropertyName)
            {
                EquationLineEnabledChanged(equation);
            }
        }

    private:
        Platform::Collections::Vector<GraphControl::Equation ^> ^ m_vector;
        std::vector<Windows::Foundation::EventRegistrationToken> m_tokens;
    };
}
