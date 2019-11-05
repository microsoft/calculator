#pragma once
#include <CppUnitTest.h>

template <typename T>
void VERIFY_VECTORS_ARE_EQUAL(Windows::Foundation::Collections::IVector<T> ^ vecA, Windows::Foundation::Collections::IVector<T> ^ vecB, ...)
{
    if (vecA->Size != vecB->Size)
        Assert::Fail();

    for (unsigned int i = 0; i < vecA->Size; ++i)
    {
        VERIFY_ARE_EQUAL(vecA->GetAt(i), vecB->GetAt(i), __VA_ARGS__);
    }
};
