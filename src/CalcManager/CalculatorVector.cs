// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Text;
using CalcManager.Ratpack;

namespace CalcManager
{
    public class CalculatorVector<TType>
    {
        private readonly List<TType> m_vector = new List<TType>();

        public int GetAt(uint index, out TType item)
        {
            if (index < m_vector.Count)
            {
                item = m_vector[(int)index];
                return CalcErr.S_OK;
            }
            item = default(TType);
            return CalcErr.E_BOUNDS;
        }

        public int GetSize(out uint size)
        {
            size = (uint)m_vector.Count;
            return CalcErr.S_OK;
        }

        public int SetAt(uint index, TType item)
        {
            if (index < m_vector.Count)
            {
                m_vector[(int)index] = item;
                return CalcErr.S_OK;
            }
            return CalcErr.E_BOUNDS;
        }

        public int RemoveAt(uint index)
        {
            if (index < m_vector.Count)
            {
                m_vector.RemoveAt((int)index);
                return CalcErr.S_OK;
            }
            return CalcErr.E_BOUNDS;
        }

        public int InsertAt(uint index, TType item)
        {
            if (index <= m_vector.Count)
            {
                m_vector.Insert((int)index, item);
                return CalcErr.S_OK;
            }
            return CalcErr.E_BOUNDS;
        }

        public int Truncate(uint index)
        {
            if (index < m_vector.Count)
            {
                m_vector.RemoveRange((int)index, m_vector.Count - (int)index);
                return CalcErr.S_OK;
            }
            return CalcErr.E_BOUNDS;
        }

        public int Append(TType item)
        {
            m_vector.Add(item);
            return CalcErr.S_OK;
        }

        public int RemoveAtEnd()
        {
            if (m_vector.Count > 0)
            {
                m_vector.RemoveAt(m_vector.Count - 1);
                return CalcErr.S_OK;
            }
            return CalcErr.E_BOUNDS;
        }

        public int Clear()
        {
            m_vector.Clear();
            return CalcErr.S_OK;
        }

        public int GetString(out string expression)
        {
            var sb = new StringBuilder();
            uint nTokens = 0;
            int hr = GetSize(out nTokens);
            if (CalcErr.SUCCEEDED(hr))
            {
                for (uint i = 0; i < nTokens; i++)
                {
                    hr = GetAt(i, out TType currentPair);
                    if (CalcErr.SUCCEEDED(hr) && currentPair is Tuple<string, int> pair)
                    {
                        sb.Append(pair.Item1);
                        if (i != (nTokens - 1))
                        {
                            sb.Append(" ");
                        }
                    }
                }

                hr = GetExpressionSuffix(out string expressionSuffix);
                if (CalcErr.SUCCEEDED(hr))
                {
                    sb.Append(expressionSuffix);
                }
            }

            expression = sb.ToString();
            return hr;
        }

        public int GetExpressionSuffix(out string suffix)
        {
            suffix = " =";
            return CalcErr.S_OK;
        }

        public List<TType> RawList => m_vector;
    }
}
