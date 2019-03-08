// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

template <typename TType>
class CalculatorVector 
{
public:
    HRESULT GetAt(_In_opt_ unsigned int index, _Out_ TType *item)
    {
        HRESULT hr = S_OK;
        try
        {
            *item = m_vector.at(index);
        }
        catch (const std::out_of_range& /*ex*/)
        {
            hr = E_BOUNDS;
        }
        return hr;
    }

    HRESULT GetSize(_Out_ unsigned int *size)
    {
        *size = static_cast<unsigned>(m_vector.size());
        return S_OK;
    }

    HRESULT SetAt(_In_ unsigned int index, _In_opt_ TType item)
    {
        HRESULT hr = S_OK;
        try
        {
            m_vector[index] = item;
        }
        catch (const std::out_of_range& /*ex*/)
        {
            hr = E_BOUNDS;
        }
        return hr;
    }

    HRESULT RemoveAt(_In_ unsigned int index)
    {
        HRESULT hr = S_OK;
        if (index < m_vector.size())
        {
            m_vector.erase(m_vector.begin() + index);
        }
        else
        {
            hr = E_BOUNDS;
        }
        return hr;
    }

    HRESULT InsertAt(_In_ unsigned int index, _In_ TType item)
    {
        HRESULT hr = S_OK;
        try
        {
            auto iter = m_vector.begin() + index;
            m_vector.insert(iter, item);
        }
        catch (const std::bad_alloc& /*ex*/)
        {
            hr = E_OUTOFMEMORY;
        }
        return hr;
    }

    HRESULT Truncate(_In_ unsigned int index)
    {
        HRESULT hr = S_OK;
        if (index < m_vector.size())
        {
            auto startIter = m_vector.begin() + index;
            m_vector.erase(startIter, m_vector.end());
        }
        else
        {
            hr = E_BOUNDS;
        }
        return hr;
    }

    HRESULT Append(_In_opt_ TType item)
    {
        HRESULT hr = S_OK;
        try
        {
            m_vector.push_back(item);
        }
        catch (const std::bad_alloc& /*ex*/)
        {
            hr = E_OUTOFMEMORY;
        }
        return hr;
    }

    HRESULT RemoveAtEnd()
    {
        m_vector.erase(--(m_vector.end()));
        return S_OK;
    }

    HRESULT Clear()
    {
        m_vector.clear();
        return S_OK;
    }

    HRESULT GetString(_Out_ std::wstring* expression)
    {
        HRESULT hr = S_OK;
        unsigned int nTokens = 0;
        std::pair <std::wstring, int> currentPair;
        hr = this->GetSize(&nTokens);
        if (SUCCEEDED(hr))
        {
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

    HRESULT GetExpressionSuffix(_Out_ std::wstring* suffix)
    {
        *suffix = L" =";
        return S_OK;
    }

private:
    std::vector<TType> m_vector;
};
