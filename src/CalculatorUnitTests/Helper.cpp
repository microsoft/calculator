#include "pch.h"
#include "Helpers.h"
using namespace Platform;
using namespace std;

String ^ GetStringValue(String ^ input) {
    // Remove first and last " characters
    if (input->Length() >= 3)
    {
        wstring out(input->Begin() + 1, input->End() - 1);
        return ref new String(out.c_str());
    }
    return input;
}
