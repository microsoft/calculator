using System;
using System.Collections.ObjectModel;
using System.Linq;

namespace CalculatorApp
{
	public class CalculatorObservableCollection<TType> : ObservableCollection<TType>
	{
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