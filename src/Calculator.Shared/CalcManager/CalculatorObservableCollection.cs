using System;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Linq;
using Windows.UI.Xaml.Input;
using Uno.Extensions;

namespace CalculatorApp
{
	public class CalculatorObservableCollection<TType> : ObservableCollection<TType>
	{
		private int _batchUpdateCount;

		public IDisposable BatchUpdate()
		{
			++_batchUpdateCount;

			return Uno.Disposables.Disposable.Create(Release);

			void Release()
			{
				if (--_batchUpdateCount <= 0)
				{
					OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
				}
			}
		}


		/// <inheritdoc />
		protected override void OnCollectionChanged(NotifyCollectionChangedEventArgs e)
		{
			if (_batchUpdateCount > 0)
			{
				return;
			}

			base.OnCollectionChanged(e);
		}

		public void Append(TType item)
		{
			Add(item);
		}

		public TType GetAt(int index)
		{
			return this[index];
		}
	}
}
