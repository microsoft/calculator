// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// Utils.cpp
//

#include "pch.h"
#include "Utils.h"
#include "Common/AppResourceProvider.h"
#include "Common/ExpressionCommandSerializer.h"
#include "Common/ExpressionCommandDeserializer.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace concurrency;
using namespace Platform;
using namespace std;
using namespace Utils;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::Foundation;
using namespace Windows::Storage;

void Utils::IFTPlatformException(HRESULT hr)
{
    if (FAILED(hr))
    {
        Platform::Exception ^ exception = ref new Platform::Exception(hr);
        throw(exception);
    }
}

String ^ Utils::GetStringValue(String ^ input)
{
    // Remove first and last " characters
    if (input->Length() >= 3)
    {
        wstring out(input->Begin() + 1, input->End() - 1);
        return ref new String(out.c_str());
    }
    return input;
}

double Utils::GetDoubleFromWstring(wstring input)
{
    wchar_t unWantedChars[] = { L' ', L',', 8234, 8235, 8236, 8237 };
    wstring ws = RemoveUnwantedCharsFromWstring(input, unWantedChars, 6);
    return stod(ws);
}

// Returns windowId for the current view
int Utils::GetWindowId()
{
    int windowId = -1;

    auto window = CoreWindow::GetForCurrentThread();
    if (window != nullptr)
    {
        windowId = ApplicationView::GetApplicationViewIdForWindow(window);
    }

    return windowId;
}

void Utils::RunOnUIThreadNonblocking(std::function<void()>&& function, _In_ CoreDispatcher ^ currentDispatcher)
{
    if (currentDispatcher != nullptr)
    {
        auto task = create_task(currentDispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([function]() { function(); })));
    }
}

// Returns if the last character of a wstring is the target wchar_t
bool Utils::IsLastCharacterTarget(_In_ wstring const& input, _In_ wchar_t target)
{
    return !input.empty() && input.back() == target;
}

// Return wstring after removing characters specified by unwantedChars array
wstring Utils::RemoveUnwantedCharsFromWstring(wstring input, wchar_t* unwantedChars, unsigned int size)
{
    for (unsigned int i = 0; i < size; ++i)
    {
        input.erase(std::remove(input.begin(), input.end(), unwantedChars[i]), input.end());
    }
    return input;
}

void Utils::SerializeCommandsAndTokens(
    _In_ shared_ptr<vector<pair<wstring, int>>> const& tokens,
    _In_ shared_ptr<vector<shared_ptr<IExpressionCommand>>> const& commands,
    DataWriter ^ writer)
{
    // Save the size of the commands vector
    writer->WriteUInt32(static_cast<unsigned int>(commands->size()));

    SerializeCommandVisitor cmdVisitor(writer);
    for (const auto& exprCmd : *commands)
    {
        CalculationManager::CommandType commandType = exprCmd->GetCommandType();
        writer->WriteInt32(static_cast<int>(commandType));
        exprCmd->Accept(cmdVisitor);
    }

    writer->WriteUInt32(static_cast<unsigned int>(tokens->size()));

    for (const auto& eachToken : *tokens)
    {
        auto stringData = ref new Platform::String(eachToken.first.c_str());
        auto intData = eachToken.second;
        writer->WriteUInt32(writer->MeasureString(stringData));
        writer->WriteString(stringData);
        writer->WriteInt32(intData);
    }
}

const shared_ptr<vector<shared_ptr<IExpressionCommand>>> Utils::DeserializeCommands(DataReader ^ reader)
{
    auto commandVector = make_shared<vector<shared_ptr<IExpressionCommand>>>();
    auto commandVectorSize = reader->ReadUInt32();

    CommandDeserializer cmdDeserializer(reader);
    for (unsigned int i = 0; i < commandVectorSize; ++i)
    {
        auto commandTypeInt = reader->ReadInt32();
        CalculationManager::CommandType commandType = static_cast<CalculationManager::CommandType>(commandTypeInt);
        shared_ptr<IExpressionCommand> exprCmd = cmdDeserializer.Deserialize(commandType);
        commandVector->push_back(exprCmd);
    }

    return commandVector;
}

const shared_ptr<vector<pair<wstring, int>>> Utils::DeserializeTokens(DataReader ^ reader)
{
    auto tokenVector = make_shared<vector<pair<wstring, int>>>();
    auto tokensSize = reader->ReadUInt32();

    for (unsigned int i = 0; i < tokensSize; ++i)
    {
        auto stringDataLen = reader->ReadUInt32();
        auto stringData = reader->ReadString(stringDataLen);
        auto intData = reader->ReadInt32();
        tokenVector->emplace_back(stringData->Data(), intData);
    }

    return tokenVector;
}

DateTime Utils::GetUniversalSystemTime()
{
    SYSTEMTIME sysTime = {};
    GetSystemTime(&sysTime);

    FILETIME sysTimeAsFileTime = {};
    SystemTimeToFileTime(&sysTime, &sysTimeAsFileTime);

    ULARGE_INTEGER ularge;
    ularge.HighPart = sysTimeAsFileTime.dwHighDateTime;
    ularge.LowPart = sysTimeAsFileTime.dwLowDateTime;

    DateTime result;
    result.UniversalTime = ularge.QuadPart;
    return result;
}

bool Utils::IsDateTimeOlderThan(DateTime dateTime, const long long duration)
{
    DateTime now = Utils::GetUniversalSystemTime();
    return dateTime.UniversalTime + duration < now.UniversalTime;
}

task<void> Utils::WriteFileToFolder(IStorageFolder ^ folder, String ^ fileName, String ^ contents, CreationCollisionOption collisionOption)
{
    if (folder == nullptr)
    {
        co_return;
    }

    StorageFile ^ file = co_await folder->CreateFileAsync(fileName, collisionOption);
    if (file == nullptr)
    {
        co_return;
    }

    co_await FileIO::WriteTextAsync(file, contents);
}

task<String ^> Utils::ReadFileFromFolder(IStorageFolder ^ folder, String ^ fileName)
{
    if (folder == nullptr)
    {
        co_return nullptr;
    }

    StorageFile ^ file = co_await folder->GetFileAsync(fileName);
    if (file == nullptr)
    {
        co_return nullptr;
    }

    String ^ contents = co_await FileIO::ReadTextAsync(file);
    co_return contents;
}
