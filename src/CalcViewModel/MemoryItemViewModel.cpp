// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "MemoryItemViewModel.h"
#include "StandardCalculatorViewModel.h"

using namespace CalculatorApp;
using namespace CalculatorApp::ViewModel::Common;
using namespace CalculatorApp::ViewModel::Common::Automation;
using namespace CalculatorApp::ViewModel;
using namespace Platform;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Security::Cryptography;
using namespace Windows::Foundation::Collections;

void MemoryItemViewModel::Clear()
{
    m_calcVM->OnMemoryClear(Position);
};

void MemoryItemViewModel::MemoryAdd()
{
    m_calcVM->OnMemoryAdd(Position);
};

void MemoryItemViewModel::MemorySubtract()
{
    m_calcVM->OnMemorySubtract(Position);
};
