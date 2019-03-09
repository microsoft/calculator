// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

class CalcException : public std::exception
{
public:
    CalcException(HRESULT hr)
    {
        m_hr = hr;
    }
    HRESULT GetException()
    {
        return m_hr;
    }
private:
    HRESULT m_hr;
};

void IFT(HRESULT hr)
{
    if (FAILED(hr))
    {
        CalcException exception(hr);
        throw(exception);
    }
}
