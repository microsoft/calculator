// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "../Common/Utils.h"

namespace GraphControl
{
    ref class Equation;
    ref class KeyGraphFeaturesInfo;
}

namespace CalculatorApp::ViewModel
{
public
    ref class GridDisplayItems sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        GridDisplayItems();

        OBSERVABLE_OBJECT();
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Expression);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Direction);
    };

public
    ref class KeyGraphFeaturesItem sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        KeyGraphFeaturesItem();

        OBSERVABLE_OBJECT();
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Title);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector<Platform::String ^> ^, DisplayItems);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector<GridDisplayItems ^> ^, GridItems);
        OBSERVABLE_PROPERTY_RW(bool, IsText);
    };

public
    ref class EquationViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        EquationViewModel(GraphControl::Equation ^ equation, int functionLabelIndex, Windows::UI::Color color, int colorIndex);

        OBSERVABLE_OBJECT();
        OBSERVABLE_PROPERTY_R(GraphControl::Equation ^, GraphEquation);
        OBSERVABLE_PROPERTY_RW(int, FunctionLabelIndex);
        OBSERVABLE_PROPERTY_RW(bool, IsLastItemInList);
        PROPERTY_RW(int, LineColorIndex);

        property Platform::String ^ Expression
        {
            Platform::String ^ get()
            {
                return GraphEquation->Expression;
            }
            void set(Platform::String ^ value)
            {
                if (GraphEquation->Expression != value)
                {
                    GraphEquation->Expression = value;
                    RaisePropertyChanged("Expression");
                }
            }
        }

        property Windows::UI::Color LineColor
        {
            Windows::UI::Color get()
            {
                return GraphEquation->LineColor;
            }
            void set(Windows::UI::Color value)
            {
                if (!Utils::AreColorsEqual(GraphEquation->LineColor, value))
                {
                    GraphEquation->LineColor = value;
                    RaisePropertyChanged("LineColor");
                }
            }
        }

        property bool IsLineEnabled
        {
            bool get()
            {
                return GraphEquation->IsLineEnabled;
            }
            void set(bool value)
            {
                if (GraphEquation->IsLineEnabled != value)
                {
                    GraphEquation->IsLineEnabled = value;
                    RaisePropertyChanged("IsLineEnabled");
                }
            }
        }

        // Key Graph Features
        OBSERVABLE_PROPERTY_R(Platform::String ^, AnalysisErrorString);
        OBSERVABLE_PROPERTY_R(bool, AnalysisErrorVisible);
        OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IObservableVector<CalculatorApp::ViewModel::KeyGraphFeaturesItem ^> ^, KeyGraphFeaturesItems)

        void PopulateKeyGraphFeatures(GraphControl::KeyGraphFeaturesInfo ^ info);

        static Platform::String ^ EquationErrorText(GraphControl::ErrorType errorType, int errorCode);

    private:
        void AddKeyGraphFeature(Platform::String ^ title, Platform::String ^ expression, Platform::String ^ errorString);
        void AddKeyGraphFeature(
            Platform::String ^ title,
            Windows::Foundation::Collections::IVector<Platform::String ^> ^ expressionVector,
            Platform::String ^ errorString);
        void AddParityKeyGraphFeature(GraphControl::KeyGraphFeaturesInfo ^ info);
        void AddPeriodicityKeyGraphFeature(GraphControl::KeyGraphFeaturesInfo ^ info);
        void AddMonotoncityKeyGraphFeature(GraphControl::KeyGraphFeaturesInfo ^ info);
        void AddTooComplexKeyGraphFeature(GraphControl::KeyGraphFeaturesInfo ^ info);

        Windows::Foundation::Collections::IObservableMap<Platform::String ^, Platform::String ^> ^ m_Monotonicity;
        Windows::ApplicationModel::Resources::ResourceLoader ^ m_resourceLoader;
    };
}
