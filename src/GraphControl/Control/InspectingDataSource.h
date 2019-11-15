// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace GraphControl
{
    public enum class DataSourceChangedAction
    {
        Insert,
        Remove,
        Replace,
        Reset
    };

    value struct DataSourceChangedEventArgs sealed
    {
        DataSourceChangedAction Action;
        int OldStartingIndex;
        int OldItemsCount;
        int NewStartingIndex;
        int NewItemsCount;
    };

    ref class InspectingDataSource sealed
    {
    internal:
        InspectingDataSource(Platform::Object^ source);

        event Windows::Foundation::TypedEventHandler<InspectingDataSource^, DataSourceChangedEventArgs>^ DataSourceChanged;

        unsigned int GetSize();
        Platform::Object^ GetAt(unsigned int index);
        std::optional<unsigned int> IndexOf(Platform::Object^ value);

    private:
        ~InspectingDataSource();

        static winrt::Windows::Foundation::Collections::IVector<winrt::Windows::Foundation::IInspectable>
            WrapIterable(const winrt::Windows::Foundation::Collections::IIterable<winrt::Windows::Foundation::IInspectable>& iterable);

        void ListenToCollectionChanges();
        void UnlistenToCollectionChanges();

        void OnCollectionChanged(
            const winrt::Windows::Foundation::IInspectable& sender,
            const winrt::Windows::UI::Xaml::Interop::NotifyCollectionChangedEventArgs& e);

        void OnVectorChanged(
            const winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable>& sender,
            const winrt::Windows::Foundation::Collections::IVectorChangedEventArgs& e);

        void OnBindableVectorChanged(
            winrt::Windows::UI::Xaml::Interop::IBindableObservableVector const& vector,
            winrt::Windows::Foundation::IInspectable const& e);

    private:
        winrt::Windows::Foundation::Collections::IVector<winrt::Windows::Foundation::IInspectable> m_vector;

        winrt::Windows::UI::Xaml::Interop::INotifyCollectionChanged m_notifyCollectionChanged;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> m_observableVector;
        winrt::Windows::UI::Xaml::Interop::IBindableObservableVector m_bindableObservableVector;
        winrt::event_token m_eventToken;
    };
}
