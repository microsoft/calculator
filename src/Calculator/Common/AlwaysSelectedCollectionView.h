// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp { namespace Common 
{
    ref class AlwaysSelectedCollectionView sealed: 
        public Windows::UI::Xaml::DependencyObject,
        public Windows::UI::Xaml::Data::ICollectionView
    {
    internal:
        AlwaysSelectedCollectionView(Windows::UI::Xaml::Interop::IBindableVector^ source):
            m_currentPosition(-1)
        {
            m_source = source;
            
            Windows::UI::Xaml::Interop::IBindableObservableVector^ observable = dynamic_cast<Windows::UI::Xaml::Interop::IBindableObservableVector^>(source);
            if (observable)
            {
                observable->VectorChanged += 
                    ref new Windows::UI::Xaml::Interop::BindableVectorChangedEventHandler(this, &AlwaysSelectedCollectionView::OnSourceBindableVectorChanged);
            }
        }

    private:
        // ICollectionView
          // Not implemented methods
        virtual Windows::Foundation::IAsyncOperation<Windows::UI::Xaml::Data::LoadMoreItemsResult>^ LoadMoreItemsAsync(unsigned int) = Windows::UI::Xaml::Data::ICollectionView::LoadMoreItemsAsync
        {
            throw ref new Platform::NotImplementedException();
        }
        virtual bool MoveCurrentToFirst() = Windows::UI::Xaml::Data::ICollectionView::MoveCurrentToFirst
        {
            throw ref new Platform::NotImplementedException();
        }
        virtual bool MoveCurrentToLast() = Windows::UI::Xaml::Data::ICollectionView::MoveCurrentToLast
        {
            throw ref new Platform::NotImplementedException();
        }
        virtual bool MoveCurrentToNext() = Windows::UI::Xaml::Data::ICollectionView::MoveCurrentToNext
        {
            throw ref new Platform::NotImplementedException();
        }
        virtual bool MoveCurrentToPrevious() = Windows::UI::Xaml::Data::ICollectionView::MoveCurrentToPrevious
        {
            throw ref new Platform::NotImplementedException();
        }
        property Windows::Foundation::Collections::IObservableVector<Platform::Object^>^ CollectionGroups
        {
            virtual Windows::Foundation::Collections::IObservableVector<Platform::Object^>^ get() = Windows::UI::Xaml::Data::ICollectionView::CollectionGroups::get
            {
                return ref new Platform::Collections::Vector<Platform::Object^>();
            }
        }
        property bool HasMoreItems 
        {
            virtual bool get() = Windows::UI::Xaml::Data::ICollectionView::HasMoreItems::get
            {
                return false;
            }
        }

          // Implementented methods
        virtual bool MoveCurrentTo(Platform::Object^ item) = Windows::UI::Xaml::Data::ICollectionView::MoveCurrentTo
        {
            if (item)
            {
                unsigned int newCurrentPosition = 0;
                bool result = m_source->IndexOf(item, &newCurrentPosition);
                if (result)
                {
                    m_currentPosition = newCurrentPosition;
                    m_currentChanged(this, nullptr);
                    return true;
                }
            }

            // The item is not in the collection
            // We're going to schedule a call back later so we 
            // restore the selection to the way we wanted it to begin with
            if (m_currentPosition >= 0 && m_currentPosition < static_cast<int>(m_source->Size))
            {
                this->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, 
                    ref new Windows::UI::Core::DispatchedHandler(
                        [this]()
                        {
                            m_currentChanged(this, nullptr);
                        }));
            }
            return false;
        }

        virtual bool MoveCurrentToPosition(int index) = Windows::UI::Xaml::Data::ICollectionView::MoveCurrentToPosition
        {
            if (index < 0 || index >= static_cast<int>(m_source->Size))
            {
                return false;
            }

            m_currentPosition = index;
            m_currentChanged(this, nullptr);
            return true;
        }

        property Platform::Object^ CurrentItem
        {
            virtual Platform::Object^ get() = Windows::UI::Xaml::Data::ICollectionView::CurrentItem::get
            {
                if (m_currentPosition >= 0 && m_currentPosition < static_cast<int>(m_source->Size))
                {
                    return m_source->GetAt(m_currentPosition);
                }
                return nullptr;
            }
        }

        property int CurrentPosition
        {
            virtual int get() = Windows::UI::Xaml::Data::ICollectionView::CurrentPosition::get
            {
                return m_currentPosition;
            }
        }

        property bool IsCurrentAfterLast
        {
            virtual bool get() = Windows::UI::Xaml::Data::ICollectionView::IsCurrentAfterLast::get
            {
                return m_currentPosition >= static_cast<int>(m_source->Size);
            }
        }

        property bool IsCurrentBeforeFirst
        {
            virtual bool get() = Windows::UI::Xaml::Data::ICollectionView::IsCurrentBeforeFirst::get
            {
                return m_currentPosition < 0;
            }
        }

        event Windows::Foundation::EventHandler<Platform::Object^>^ CurrentChanged
        {
            virtual Windows::Foundation::EventRegistrationToken add(Windows::Foundation::EventHandler<Platform::Object^>^ handler) = Windows::UI::Xaml::Data::ICollectionView::CurrentChanged::add
            {
                return m_currentChanged += handler;
            }
            virtual void remove(Windows::Foundation::EventRegistrationToken token) = Windows::UI::Xaml::Data::ICollectionView::CurrentChanged::remove
            {
                m_currentChanged -= token;
            }
        }
        event Windows::UI::Xaml::Data::CurrentChangingEventHandler^ CurrentChanging
        {
            virtual Windows::Foundation::EventRegistrationToken add(Windows::UI::Xaml::Data::CurrentChangingEventHandler^ handler) = Windows::UI::Xaml::Data::ICollectionView::CurrentChanging::add
            {
                return m_currentChanging += handler;
            }
            virtual void remove(Windows::Foundation::EventRegistrationToken token) = Windows::UI::Xaml::Data::ICollectionView::CurrentChanging::remove
            {
                m_currentChanging -= token;
            }
        }
        
        // IVector<Object^>
          // Not implemented methods
        virtual void Append(Platform::Object^ /*item*/) = Windows::Foundation::Collections::IVector<Platform::Object^>::Append
        {
            throw ref new Platform::NotImplementedException();
        }
        virtual void Clear() = Windows::Foundation::Collections::IVector<Platform::Object^>::Clear
        {
            throw ref new Platform::NotImplementedException();
        }
        virtual unsigned int GetMany(unsigned int /*startIndex*/, Platform::WriteOnlyArray<Platform::Object^>^ /*items*/) = Windows::Foundation::Collections::IVector<Platform::Object^>::GetMany
        {
            throw ref new Platform::NotImplementedException();
        }
        virtual Windows::Foundation::Collections::IVectorView<Platform::Object^>^ GetView() = Windows::Foundation::Collections::IVector<Platform::Object^>::GetView
        {
            throw ref new Platform::NotImplementedException();
        }
        virtual void InsertAt(unsigned int /*index*/, Platform::Object^ /*item*/) = Windows::Foundation::Collections::IVector<Platform::Object^>::InsertAt
        {
            throw ref new Platform::NotImplementedException();
        }
        virtual void RemoveAt(unsigned int /*index*/) = Windows::Foundation::Collections::IVector<Platform::Object^>::RemoveAt
        {
            throw ref new Platform::NotImplementedException();
        }
        virtual void RemoveAtEnd() = Windows::Foundation::Collections::IVector<Platform::Object^>::RemoveAtEnd
        {
            throw ref new Platform::NotImplementedException();
        }
        virtual void ReplaceAll(const Platform::Array<Platform::Object^>^ /*items*/) = Windows::Foundation::Collections::IVector<Platform::Object^>::ReplaceAll
        {
            throw ref new Platform::NotImplementedException();
        }
        virtual void SetAt(unsigned int /*index*/, Platform::Object^ /*item*/) = Windows::Foundation::Collections::IVector<Platform::Object^>::SetAt
        {
            throw ref new Platform::NotImplementedException();
        }

          // Implemented methods
        virtual Platform::Object^ GetAt(unsigned int index) = Windows::Foundation::Collections::IVector<Platform::Object^>::GetAt
        {
            return m_source->GetAt(index);
        }

        virtual bool IndexOf(Platform::Object^ item, unsigned int* index) = Windows::Foundation::Collections::IVector<Platform::Object^>::IndexOf
        {
            return m_source->IndexOf(item, index);
        }

        property unsigned int Size
        {
            virtual unsigned int get() = Windows::Foundation::Collections::IVector<Platform::Object^>::Size::get
            {
                return m_source->Size;
            }
        }
    
        // IObservableVector<Object^>
        event Windows::Foundation::Collections::VectorChangedEventHandler<Platform::Object^>^ VectorChanged
        {
            virtual Windows::Foundation::EventRegistrationToken add(Windows::Foundation::Collections::VectorChangedEventHandler<Platform::Object^>^ handler) = Windows::Foundation::Collections::IObservableVector<Platform::Object^>::VectorChanged::add
            {
                return m_vectorChanged += handler;
            }
            virtual void remove(Windows::Foundation::EventRegistrationToken token) = Windows::Foundation::Collections::IObservableVector<Platform::Object^>::VectorChanged::remove
            {
                m_vectorChanged -= token;
            }
        }

        // IIterable<Object^>
         // Not implemented
        virtual Windows::Foundation::Collections::IIterator<Platform::Object^>^ First() = Windows::Foundation::Collections::IIterable<Platform::Object^>::First
        {
            throw ref new Platform::NotImplementedException();
        }

        // Event handlers
        void OnSourceBindableVectorChanged(Windows::UI::Xaml::Interop::IBindableObservableVector^ source, Platform::Object^ e)
        {
            Windows::Foundation::Collections::IVectorChangedEventArgs^ args = safe_cast<Windows::Foundation::Collections::IVectorChangedEventArgs^>(e);
            m_vectorChanged(this, args);
        }

        Windows::UI::Xaml::Interop::IBindableVector^ m_source;
        int m_currentPosition;
        event Windows::Foundation::EventHandler<Platform::Object^>^ m_currentChanged;
        event Windows::UI::Xaml::Data::CurrentChangingEventHandler^ m_currentChanging;
        event Windows::Foundation::Collections::VectorChangedEventHandler<Platform::Object^>^ m_vectorChanged;
    };

    public ref class AlwaysSelectedCollectionViewConverter sealed: public Windows::UI::Xaml::Data::IValueConverter
    {
    public:
        AlwaysSelectedCollectionViewConverter()
        { }

    private:
        virtual Platform::Object^ Convert(
            Platform::Object^ value, 
            Windows::UI::Xaml::Interop::TypeName /*targetType*/, 
            Platform::Object^ /*parameter*/, 
            Platform::String^ /*language*/) = Windows::UI::Xaml::Data::IValueConverter::Convert
        {
            auto result = dynamic_cast<Windows::UI::Xaml::Interop::IBindableVector^>(value);
            if (result)
            {
                return ref new AlwaysSelectedCollectionView(result);
            }
            return Windows::UI::Xaml::DependencyProperty::UnsetValue;  // Can't convert
        }

        virtual Platform::Object^ ConvertBack(
            Platform::Object^ /*value*/, 
            Windows::UI::Xaml::Interop::TypeName /*targetType*/, 
            Platform::Object^ /*parameter*/, 
            Platform::String^ /*language*/) = Windows::UI::Xaml::Data::IValueConverter::ConvertBack
        {
            return Windows::UI::Xaml::DependencyProperty::UnsetValue;
        }
    };
}}
