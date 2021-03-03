// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <vector>
#include "winerror_cross_platform.h"
#include "Ratpack/CalcErr.h"
#include <stdexcept>            // for std::out_of_range
#include "sal_cross_platform.h" // for SAL

template <typename TType>
class CalculatorVector
{
public:
    ResultCode GetAt(_In_opt_ unsigned int index, _Out_ TType* item)
    {
        try
        {
            *item = m_vector.at(index);
        }
        catch (const std::out_of_range& /*ex*/)
        {
            return E_BOUNDS;
        }
        return S_OK;
    }

    ResultCode GetSize(_Out_ unsigned int* size)
    {
        *size = static_cast<unsigned>(m_vector.size());
        return S_OK;
    }

    ResultCode SetAt(_In_ unsigned int index, _In_opt_ TType item)
    {
        try
        {
            m_vector[index] = item;
        }
        catch (const std::out_of_range& /*ex*/)
        {
            return E_BOUNDS;
        }
        return S_OK;
    }

    ResultCode RemoveAt(_In_ unsigned int index)
    {
        if (index < m_vector.size())
        {
            m_vector.erase(m_vector.begin() + index);
        }
        else
        {
            return E_BOUNDS;
        }
        return S_OK;
    }

    ResultCode InsertAt(_In_ unsigned int index, _In_ TType item)
    {
        try
        {
            auto iter = m_vector.begin() + index;
            m_vector.insert(iter, item);
        }
        catch (const std::bad_alloc& /*ex*/)
        {
            return E_OUTOFMEMORY;
        }
        return S_OK;
    }

    ResultCode Truncate(_In_ unsigned int index)
    {
        if (index < m_vector.size())
        {
            auto startIter = m_vector.begin() + index;
            m_vector.erase(startIter, m_vector.end());
        }
        else
        {
            return E_BOUNDS;
        }
        return S_OK;
    }

    ResultCode Append(_In_opt_ TType item)
    {
        try
        {
            m_vector.push_back(item);
        }
        catch (const std::bad_alloc& /*ex*/)
        {
            return E_OUTOFMEMORY;
        }
        return S_OK;
    }

    ResultCode RemoveAtEnd()
    {
        m_vector.erase(--(m_vector.end()));
        return S_OK;
    }

    ResultCode Clear()
    {
        m_vector.clear();
        return S_OK;
    }

    ResultCode GetString(_Out_ std::wstring* expression)
    {
        unsigned int nTokens = 0;
        ResultCode hr = this->GetSize(&nTokens);
        if (SUCCEEDED(hr))
        {
            std::pair<std::wstring, int> currentPair;
            for (unsigned int i = 0; i < nTokens; i++)
            {
                hr = this->GetAt(i, &currentPair);
                if (SUCCEEDED(hr))
                {
                    expression->append(currentPair.first);

                    if (i != (nTokens - 1))
                    {
                        expression->append(L" ");
                    }
                }
            }

            std::wstring expressionSuffix{};
            hr = GetExpressionSuffix(&expressionSuffix);
            if (SUCCEEDED(hr))
            {
                expression->append(expressionSuffix);
            }
        }

        return hr;
    }

    ResultCode GetExpressionSuffix(_Out_ std::wstring* suffix)
    {
        *suffix = L" =";
        return S_OK;
    }

private:
    std::vector<TType> m_vector;
};
