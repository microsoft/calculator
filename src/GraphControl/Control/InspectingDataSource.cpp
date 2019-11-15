// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "InspectingDataSource.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml::Interop;

namespace winrt
{
    using namespace winrt::Windows::Foundation;
    using namespace winrt::Windows::Foundation::Collections;
    using namespace winrt::Windows::UI::Xaml::Interop;
}

namespace GraphControl
{
    InspectingDataSource::InspectingDataSource(Object^ source)
    {
        if (!source)
        {
            throw ref new InvalidArgumentException(L"Argument 'source' is null.");
        }

        auto inspectable = from_cx<winrt::IInspectable>(source);
        if (auto vector = inspectable.try_as<winrt::IVector<winrt::IInspectable>>())
        {
            m_vector = vector;
            ListenToCollectionChanges();
        }
        else if (auto bindableVector = inspectable.try_as<winrt::IBindableVector>())
        {
            // The bindable interop interface are abi compatible with the corresponding
            // WinRT interfaces.

            m_vector = reinterpret_cast<const winrt::IVector<winrt::IInspectable>&>(bindableVector);
            ListenToCollectionChanges();
        }
        else if (auto iterable = inspectable.try_as<winrt::IIterable<winrt::IInspectable>>())
        {
            m_vector = WrapIterable(iterable);
        }
        else if (auto bindableIterable = inspectable.try_as<winrt::IBindableIterable>())
        {
            m_vector = WrapIterable(reinterpret_cast<const winrt::IIterable<winrt::IInspectable> &>(bindableIterable));
        }
        else
        {
            throw ref new InvalidArgumentException(L"Argument 'source' is not a supported vector.");
        }
    }

    InspectingDataSource::~InspectingDataSource()
    {
        UnlistenToCollectionChanges();
    }

    unsigned int InspectingDataSource::GetSize()
    {
        return m_vector.Size();
    }

    Object^ InspectingDataSource::GetAt(unsigned int index)
    {
        return to_cx<Object>(m_vector.GetAt(index));
    }

    optional<unsigned int> InspectingDataSource::IndexOf(Object^ value)
    {
        if ((m_vector != nullptr) && value)
        {
            uint32_t v;
            auto inspectable = from_cx<winrt::IInspectable>(value);
            if (m_vector.IndexOf(inspectable, v))
            {
                return v;
            }
        }
        return nullopt;
    }

    winrt::IVector<winrt::IInspectable> InspectingDataSource::WrapIterable(const winrt::IIterable<winrt::IInspectable>& iterable)
    {
        auto vector = winrt::single_threaded_vector<winrt::IInspectable>();
        auto iterator = iterable.First();
        while (iterator.HasCurrent())
        {
            vector.Append(iterator.Current());
            iterator.MoveNext();
        }

        return vector;
    }

    void InspectingDataSource::UnlistenToCollectionChanges()
    {
        if (m_notifyCollectionChanged)
        {
            m_notifyCollectionChanged.CollectionChanged(m_eventToken);
        }
        else if (m_observableVector)
        {
            m_observableVector.VectorChanged(m_eventToken);
        }
        else if (m_bindableObservableVector)
        {
            m_bindableObservableVector.VectorChanged(m_eventToken);
        }
    }

    void InspectingDataSource::ListenToCollectionChanges()
    {
        assert(m_vector);
        if (auto incc = m_vector.try_as<winrt::INotifyCollectionChanged>())
        {
            m_eventToken = incc.CollectionChanged([this](
                const winrt::IInspectable& sender,
                const winrt::NotifyCollectionChangedEventArgs& e)
            {
                OnCollectionChanged(sender, e);
            });

            m_notifyCollectionChanged = incc;
        }
        else if (auto observableVector = m_vector.try_as<winrt::IObservableVector<winrt::IInspectable>>())
        {
            m_eventToken = observableVector.VectorChanged([this](
                const winrt::IObservableVector<winrt::IInspectable>& sender,
                const winrt::IVectorChangedEventArgs& e)
            {
                OnVectorChanged(sender, e);
            });

            m_observableVector = observableVector;
        }
        else if (auto bindableObservableVector = m_vector.try_as<winrt::IBindableObservableVector>())
        {
            m_eventToken = bindableObservableVector.VectorChanged([this](
                winrt::IBindableObservableVector const& vector,
                winrt::IInspectable const& e)
            {
                OnBindableVectorChanged(vector, e);
            });

            m_bindableObservableVector = bindableObservableVector;
        }
    }

    void InspectingDataSource::OnCollectionChanged(
        const winrt::IInspectable& /*sender*/,
        const winrt::NotifyCollectionChangedEventArgs& e)
    {
        DataSourceChangedAction action;

        switch (e.Action())
        {
        case winrt::NotifyCollectionChangedAction::Add:
            action = DataSourceChangedAction::Insert;
            break;
        case winrt::NotifyCollectionChangedAction::Remove:
            action = DataSourceChangedAction::Remove;
            break;
        case winrt::NotifyCollectionChangedAction::Replace:
            action = DataSourceChangedAction::Replace;
            break;
        case winrt::NotifyCollectionChangedAction::Reset:
            action = DataSourceChangedAction::Reset;
            break;
        case winrt::NotifyCollectionChangedAction::Move:
            throw ref new Exception(E_FAIL, L"Move operations are not supported. Use a combination of Add and Remove instead.");
            break;
        default:
            assert(false);
            break;
        }

        const auto& newItems = e.NewItems();
        const auto& oldItems = e.OldItems();

        DataSourceChanged(this, DataSourceChangedEventArgs{
            action,
            e.OldStartingIndex(),
            oldItems ? static_cast<int>(oldItems.Size()) : 0,
            e.NewStartingIndex(),
            newItems ? static_cast<int>(newItems.Size()) : 0 });
    }

    void InspectingDataSource::OnVectorChanged(
        const winrt::Collections::IObservableVector<winrt::IInspectable>& /*sender*/,
        const winrt::Collections::IVectorChangedEventArgs& e)
    {
        DataSourceChangedAction action;
        int oldStartingIndex = -1;
        int oldItemsCount = 0;
        int newStartingIndex = -1;
        int newItemsCount = 0;

        // Note that the event args' Index property should NOT be accessed
        // in the Reset case, as the property accessor will throw an exception.
        switch (e.CollectionChange())
        {
        case winrt::CollectionChange::ItemInserted:
            action = DataSourceChangedAction::Insert;
            newStartingIndex = e.Index();
            newItemsCount = 1;
            break;
        case winrt::CollectionChange::ItemRemoved:
            action = DataSourceChangedAction::Remove;
            oldStartingIndex = e.Index();
            oldItemsCount = 1;
            break;
        case winrt::CollectionChange::ItemChanged:
            action = DataSourceChangedAction::Replace;
            oldStartingIndex = e.Index();
            oldItemsCount = 1;
            newStartingIndex = e.Index();
            newItemsCount = 1;
            break;
        case winrt::CollectionChange::Reset:
            action = DataSourceChangedAction::Reset;
            break;
        default:
            assert(false);
            break;
        }

        DataSourceChanged(this, DataSourceChangedEventArgs{
            action,
            oldStartingIndex,
            oldItemsCount,
            newStartingIndex,
            newItemsCount });
    }

    void InspectingDataSource::OnBindableVectorChanged(
        winrt::IBindableObservableVector const& vector,
        winrt::IInspectable const& e)
    {
        OnVectorChanged(nullptr, e.as<winrt::IVectorChangedEventArgs>());
    }
}
