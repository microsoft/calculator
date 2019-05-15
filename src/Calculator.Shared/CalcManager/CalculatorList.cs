using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;

namespace CalculatorApp
{
    public class CalculatorList<TType> : IEnumerable<TType>, IEnumerable
    {
        List<TType> m_vector;

        public CalculatorList()
        {
            m_vector = new List<TType>();
        }

        public CalculatorList(CalculatorList<TType> source)
        {
            m_vector = new List<TType>(source.m_vector);
        }

        public bool GetAt(int index, out TType item)
        {
            item = m_vector[index];
            return true;
        }

        public bool GetSize(out int size)
        {
            size = m_vector.Count;
            return true;
        }

        public bool SetAt(int index, TType item)
        {
            m_vector[index] = item;
            return true;
        }

        public bool RemoveAt(int index)
        {
            if (index < m_vector.Count)
            {
                m_vector.RemoveAt(index);
            }
            else
            {
                throw new ArgumentOutOfRangeException();
            }

            return true;
        }

        public bool InsertAt(int index, TType item)
        {
            m_vector.Insert(index, item);
            return true;
        }

        public bool Truncate(int index)
        {
            if (index < m_vector.Count)
            {
                m_vector.RemoveRange(index, m_vector.Count - index);
            }
            else
            {
                throw new ArgumentOutOfRangeException();
            }
            return true;
        }

        public bool Append(TType item)
        {
            m_vector.Add(item);
            return true;
        }

        public bool RemoveAtEnd()
        {
            m_vector.RemoveAt(m_vector.Count - 1);
            return true;
        }

        public bool Clear()
        {
            m_vector.Clear();
            return true;
        }

        public bool GetString(out string expression)
        {
            // UNO TODO
            //int nTokens = 0;
            //(string, int) currentPair;
            //var hr = this.GetSize(out nTokens);
            //if (hr)
            //{
            //    for (int i = 0; i < nTokens; i++)
            //    {
            //        hr = this.GetAt(i, out currentPair);
            //        if ((hr))
            //        {
            //            expression.append(currentPair.first);

            //            if (i != (nTokens - 1))
            //            {
            //                expression.append(" ");
            //            }
            //        }
            //    }

            //    string expressionSuffix;
            //    hr = GetExpressionSuffix(&expressionSuffix);
            //    if (hr)
            //    {
            //        expression += expressionSuffix;
            //    }
            //}

            // return true;

            expression = "";
            return false;
        }

        public bool GetExpressionSuffix(out string suffix)
        {
            suffix = " =";
            return true;
        }

		/// <inheritdoc />
		public IEnumerator<TType> GetEnumerator()
			=> m_vector.GetEnumerator();

		/// <inheritdoc />
		IEnumerator IEnumerable.GetEnumerator()
			=> GetEnumerator();
	}
}
