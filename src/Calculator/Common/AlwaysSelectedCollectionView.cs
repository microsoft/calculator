// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections;
using System.Collections.Generic;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml.Data;

namespace CalculatorApp
{
    namespace Common
    {
        sealed class AlwaysSelectedCollectionView : Windows.UI.Xaml.DependencyObject, Windows.UI.Xaml.Data.ICollectionView
        {
            internal AlwaysSelectedCollectionView(IList source)
            {
                m_currentPosition = -1;
                m_source = source;

                var observable = source as Windows.UI.Xaml.Interop.IBindableObservableVector;
                if (observable != null)
                {
                    observable.VectorChanged += OnSourceBindableVectorChanged;
                }
            }

            public bool MoveCurrentTo(object item)
            {
                if (item != null)
                {
                    int newCurrentPosition = m_source.IndexOf(item);
                    if (newCurrentPosition != -1)
                    {
                        m_currentPosition = newCurrentPosition;
                        CurrentChanged?.Invoke(this, null);
                        return true;
                    }
                }

                // The item is not in the collection
                // We're going to schedule a call back later so we
                // restore the selection to the way we wanted it to begin with
                if (m_currentPosition >= 0 && m_currentPosition < m_source.Count)
                {
                    Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, new Windows.UI.Core.DispatchedHandler(() =>
                    {
                        CurrentChanged?.Invoke(this, null);
                    })).AsTask().Wait();
                }
                return false;
            }

            public bool MoveCurrentToPosition(int index)
            {
                if (index < 0 || index >= m_source.Count)
                {
                    return false;
                }

                m_currentPosition = index;
                CurrentChanged?.Invoke(this, null);
                return true;
            }

            #region no implementations

            public bool MoveCurrentToFirst()
            {
                throw new NotImplementedException();
            }

            public bool MoveCurrentToLast()
            {
                throw new NotImplementedException();
            }

            public bool MoveCurrentToNext()
            {
                throw new NotImplementedException();
            }

            public bool MoveCurrentToPrevious()
            {
                throw new NotImplementedException();
            }

            public IAsyncOperation<LoadMoreItemsResult> LoadMoreItemsAsync(uint count)
            {
                throw new NotImplementedException();
            }

            public void Insert(int index, object item)
            {
                throw new NotImplementedException();
            }

            public void RemoveAt(int index)
            {
                throw new NotImplementedException();
            }

            public void Add(object item)
            {
                throw new NotImplementedException();
            }

            public void Clear()
            {
                throw new NotImplementedException();
            }

            public bool Contains(object item)
            {
                throw new NotImplementedException();
            }

            public void CopyTo(object[] array, int arrayIndex)
            {
                throw new NotImplementedException();
            }

            public bool Remove(object item)
            {
                throw new NotImplementedException();
            }

            public bool IsReadOnly => throw new NotImplementedException();

            #endregion no implementations

            public object this[int index]
            {
                get
                {
                    return m_source[index];
                }

                set => throw new NotImplementedException();
            }

            public int Count
            {
                get
                {
                    return m_source.Count;
                }
            }

            public IObservableVector<object> CollectionGroups
            {
                get
                {
                    return (IObservableVector<object>)new List<object>();
                }
            }

            public object CurrentItem
            {
                get
                {
                    if (m_currentPosition >= 0 && m_currentPosition < m_source.Count)
                    {
                        return m_source[m_currentPosition];
                    }
                    return null;
                }
            }

            public int CurrentPosition
            {
                get
                {
                    return m_currentPosition;
                }
            }

            public bool HasMoreItems
            {
                get
                {
                    return false;
                }
            }

            public bool IsCurrentAfterLast
            {
                get
                {
                    return m_currentPosition >= m_source.Count;
                }
            }

            public bool IsCurrentBeforeFirst
            {
                get
                {
                    return m_currentPosition < 0;
                }
            }

            public int IndexOf(object item)
            {
                return m_source.IndexOf(item);
            }

            public IEnumerator<object> GetEnumerator()
            {
                throw new NotImplementedException();
            }

            IEnumerator IEnumerable.GetEnumerator()
            {
                throw new NotImplementedException();
            }

            // Event handlers
            void OnSourceBindableVectorChanged(Windows.UI.Xaml.Interop.IBindableObservableVector source, object e)
            {
                Windows.Foundation.Collections.IVectorChangedEventArgs args = (Windows.Foundation.Collections.IVectorChangedEventArgs)e;
                VectorChanged?.Invoke(this, args);
            }

            public event EventHandler<object> CurrentChanged;
            public event VectorChangedEventHandler<object> VectorChanged;
            public event CurrentChangingEventHandler CurrentChanging
            {
                add => throw new NotImplementedException();
                remove => throw new NotImplementedException();
            }

            IList m_source;
            int m_currentPosition;
        }

        public sealed class AlwaysSelectedCollectionViewConverter : Windows.UI.Xaml.Data.IValueConverter
        {
            public AlwaysSelectedCollectionViewConverter()
            {
            }

            public object Convert(object value, Type targetType, object parameter, string language)
            {
                var result = value as IList;
                if (result != null)
                {
                    return new AlwaysSelectedCollectionView(result);
                }
                return Windows.UI.Xaml.DependencyProperty.UnsetValue; // Can't convert
            }

            public object ConvertBack(object value, Type targetType, object parameter, string language)
            {
                return Windows.UI.Xaml.DependencyProperty.UnsetValue;
            }
        }
    }
}

