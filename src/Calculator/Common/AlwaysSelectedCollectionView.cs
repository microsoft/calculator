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
        internal sealed class AlwaysSelectedCollectionView : Windows.UI.Xaml.DependencyObject, Windows.UI.Xaml.Data.ICollectionView
        {
            internal AlwaysSelectedCollectionView(IList source)
            {
                CurrentPosition = -1;
                m_source = source;

                if (source is Windows.UI.Xaml.Interop.IBindableObservableVector observable)
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
                        CurrentPosition = newCurrentPosition;
                        CurrentChanged?.Invoke(this, null);
                        return true;
                    }
                }

                // The item is not in the collection
                // We're going to schedule a call back later so we
                // restore the selection to the way we wanted it to begin with
                if (CurrentPosition >= 0 && CurrentPosition < m_source.Count)
                {
                    Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
                    {
                        CurrentChanged?.Invoke(this, null);
                    }).AsTask().Wait();
                }
                return false;
            }

            public bool MoveCurrentToPosition(int index)
            {
                if (index < 0 || index >= m_source.Count)
                {
                    return false;
                }

                CurrentPosition = index;
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
                get => m_source[index];

                set => throw new NotImplementedException();
            }

            public int Count => m_source.Count;

            public IObservableVector<object> CollectionGroups => (IObservableVector<object>)new List<object>();

            public object CurrentItem
            {
                get
                {
                    if (CurrentPosition >= 0 && CurrentPosition < m_source.Count)
                    {
                        return m_source[CurrentPosition];
                    }
                    return null;
                }
            }

            public int CurrentPosition { get; private set; }

            public bool HasMoreItems => false;

            public bool IsCurrentAfterLast => CurrentPosition >= m_source.Count;

            public bool IsCurrentBeforeFirst => CurrentPosition < 0;

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
            private void OnSourceBindableVectorChanged(Windows.UI.Xaml.Interop.IBindableObservableVector source, object e)
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

            private readonly IList m_source;
        }

        public sealed class AlwaysSelectedCollectionViewConverter : Windows.UI.Xaml.Data.IValueConverter
        {
            public AlwaysSelectedCollectionViewConverter()
            {
            }

            public object Convert(object value, Type targetType, object parameter, string language)
            {
                if (value is IList result)
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

