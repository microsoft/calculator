// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ApplicationViewModel.h"
#include "Common/TraceLogger.h"
#include "Common/AppResourceProvider.h"
#include "StandardCalculatorViewModel.h"
#include "DateCalculatorViewModel.h"
#include "DataLoaders/CurrencyHttpClient.h"
#include "DataLoaders/CurrencyDataLoader.h"
#include "DataLoaders/UnitConverterDataLoader.h"

using namespace CalculatorApp;
using namespace CalculatorApp::ViewModel::Common;
using namespace CalculatorApp::ViewModel::DataLoaders;
using namespace CalculatorApp::ViewModel;
using namespace CalculationManager;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::System;
using namespace Windows::Storage;
using namespace Utils;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::Foundation;
using namespace Concurrency;

namespace
{
    StringReference CategoriesPropertyName(L"Categories");
    StringReference ClearMemoryVisibilityPropertyName(L"ClearMemoryVisibility");

    struct SnapshotHelper
    {
        static constexpr int SnapshotVersion = 0;

        static Windows::Data::Json::JsonObject ^ SaveSnapshotToJson(const ApplicationSnapshot& value)
        {
            auto jsonObject = ref new Windows::Data::Json::JsonObject();
            jsonObject->SetNamedValue(L"SnapshotVersion", Windows::Data::Json::JsonValue::CreateNumberValue(value.SnapshotVersion));
            jsonObject->SetNamedValue(L"Mode", Windows::Data::Json::JsonValue::CreateNumberValue(value.Mode));
            if (value.StandardCalc.has_value())
            {
                jsonObject->SetNamedValue(L"StandardCalculatorSnapshot", SaveSnapshotToJson(*value.StandardCalc));
            }
            return jsonObject;
        }

        static Windows::Data::Json::JsonObject ^ SaveSnapshotToJson(const StandardCalculatorSnapshot& value)
        {
            auto jsonObject = ref new Windows::Data::Json::JsonObject();
            jsonObject->SetNamedValue(L"CalculatorManagerSnapshot", SaveSnapshotToJson(value.CalcManager));
            jsonObject->SetNamedValue(L"PrimaryDisplay", SaveSnapshotToJson(value.PrimaryDisplay));
            if (value.ExpressionDisplay.has_value())
            {
                jsonObject->SetNamedValue(L"ExpressionDisplay", SaveSnapshotToJson(*value.ExpressionDisplay));
            }
            auto commandsJsonArray = ref new Windows::Data::Json::JsonArray();
            for (const auto& command : value.DisplayCommands)
            {
                commandsJsonArray->Append(SaveSnapshotToJson(command));
            }
            jsonObject->SetNamedValue(L"DisplayCommands", commandsJsonArray);
            return jsonObject;
        }

        static Windows::Data::Json::JsonObject ^ SaveSnapshotToJson(const PrimaryDisplaySnapshot& value)
        {
            auto jsonObject = ref new Windows::Data::Json::JsonObject();
            jsonObject->SetNamedValue(L"DisplayValue", Windows::Data::Json::JsonValue::CreateStringValue(value.DisplayValue));
            jsonObject->SetNamedValue(L"IsError", Windows::Data::Json::JsonValue::CreateBooleanValue(value.IsError));
            return jsonObject;
        }

        static Windows::Data::Json::JsonObject ^ SaveSnapshotToJson(const ExpressionDisplaySnapshot& value)
        {
            auto jsonObject = ref new Windows::Data::Json::JsonObject();
            auto tokensJsonArray = ref new Windows::Data::Json::JsonArray();
            for (const auto& token : value.Tokens)
            {
                auto tokenJsonArray = ref new Windows::Data::Json::JsonArray();
                tokenJsonArray->Append(Windows::Data::Json::JsonValue::CreateStringValue(ref new Platform::String(token.first.c_str())));
                tokenJsonArray->Append(Windows::Data::Json::JsonValue::CreateNumberValue(token.second));
                tokensJsonArray->Append(tokenJsonArray);
            }
            jsonObject->SetNamedValue(L"Tokens", tokensJsonArray);

            auto commandsJsonArray = ref new Windows::Data::Json::JsonArray();
            for (const auto& command : value.Commands)
            {
                commandsJsonArray->Append(SaveSnapshotToJson(command));
            }
            jsonObject->SetNamedValue(L"Commands", commandsJsonArray);

            return jsonObject;
        }

        static Windows::Data::Json::JsonObject ^ SaveSnapshotToJson(const CalculatorManagerSnapshot& value)
        {
            auto jsonObject = ref new Windows::Data::Json::JsonObject();
            if (value.HistoryItems.has_value())
            {
                auto historyJsonArray = ref new Windows::Data::Json::JsonArray();
                for (const auto& item : *value.HistoryItems)
                {
                    historyJsonArray->Append(SaveSnapshotToJson(*item));
                }
                jsonObject->SetNamedValue(L"HistoryItems", historyJsonArray);
            }
            return jsonObject;
        }

        static Windows::Data::Json::JsonObject ^ SaveSnapshotToJson(const CalculationManager::HISTORYITEM& value)
        {
            auto jsonObject = ref new Windows::Data::Json::JsonObject();
            jsonObject->SetNamedValue(L"Expression", Windows::Data::Json::JsonValue::CreateStringValue(ref new Platform::String(value.historyItemVector.expression.c_str())));
            jsonObject->SetNamedValue(L"Result", Windows::Data::Json::JsonValue::CreateStringValue(ref new Platform::String(value.historyItemVector.result.c_str())));

            auto tokensJsonArray = ref new Windows::Data::Json::JsonArray();
            for (const auto& token : *value.historyItemVector.spTokens)
            {
                auto tokenJsonArray = ref new Windows::Data::Json::JsonArray();
                tokenJsonArray->Append(Windows::Data::Json::JsonValue::CreateStringValue(ref new Platform::String(token.first.c_str())));
                tokenJsonArray->Append(Windows::Data::Json::JsonValue::CreateNumberValue(token.second));
                tokensJsonArray->Append(tokenJsonArray);
            }
            jsonObject->SetNamedValue(L"Tokens", tokensJsonArray);

            auto commandsJsonArray = ref new Windows::Data::Json::JsonArray();
            for (const auto& command : *value.historyItemVector.spCommands)
            {
                commandsJsonArray->Append(SaveSnapshotToJson(command));
            }
            jsonObject->SetNamedValue(L"Commands", commandsJsonArray);

            return jsonObject;
        }

        static Windows::Data::Json::JsonObject ^ SaveSnapshotToJson(const std::shared_ptr<IExpressionCommand>& value)
        {
            auto jsonObject = ref new Windows::Data::Json::JsonObject();
            auto opndCommand = dynamic_cast<COpndCommand*>(value.get());
            if (opndCommand != nullptr)
            {
                jsonObject = SaveSnapshotToJson(*opndCommand);
            }
            auto unaryCommand = dynamic_cast<CUnaryCommand*>(value.get());
            if (unaryCommand != nullptr)
            {
                jsonObject = SaveSnapshotToJson(*unaryCommand);
            }
            auto binaryCommand = dynamic_cast<CBinaryCommand*>(value.get());
            if (binaryCommand != nullptr)
            {
                jsonObject = SaveSnapshotToJson(*binaryCommand);
            }
            auto parenthesesCommand = dynamic_cast<CParentheses*>(value.get());
            if (parenthesesCommand != nullptr)
            {
                jsonObject = SaveSnapshotToJson(*parenthesesCommand);
            }
            return jsonObject;
        }

        static Windows::Data::Json::JsonObject ^ SaveSnapshotToJson(const COpndCommand& value)
        {
            auto jsonObject = ref new Windows::Data::Json::JsonObject();
            jsonObject->SetNamedValue(L"CommandType", Windows::Data::Json::JsonValue::CreateNumberValue(static_cast<double>(value.GetCommandType())));
            jsonObject->SetNamedValue(L"IsNegative", Windows::Data::Json::JsonValue::CreateBooleanValue(value.IsNegative()));
            jsonObject->SetNamedValue(L"IsDecimalPresent", Windows::Data::Json::JsonValue::CreateBooleanValue(value.IsDecimalPresent()));
            jsonObject->SetNamedValue(L"IsSciFmt", Windows::Data::Json::JsonValue::CreateBooleanValue(value.IsSciFmt()));
            auto commandsJsonArray = ref new Windows::Data::Json::JsonArray();
            for (const auto& command : *value.GetCommands())
            {
                commandsJsonArray->Append(Windows::Data::Json::JsonValue::CreateNumberValue(command));
            }
            jsonObject->SetNamedValue(L"Commands", commandsJsonArray);
            return jsonObject;
        }

        static Windows::Data::Json::JsonObject ^ SaveSnapshotToJson(const CUnaryCommand& value)
        {
            auto jsonObject = ref new Windows::Data::Json::JsonObject();
            jsonObject->SetNamedValue(L"CommandType", Windows::Data::Json::JsonValue::CreateNumberValue(static_cast<double>(value.GetCommandType())));
            auto commandsJsonArray = ref new Windows::Data::Json::JsonArray();
            for (const auto& command : *value.GetCommands())
            {
                commandsJsonArray->Append(Windows::Data::Json::JsonValue::CreateNumberValue(command));
            }
            jsonObject->SetNamedValue(L"Commands", commandsJsonArray);
            return jsonObject;
        }

        static Windows::Data::Json::JsonObject ^ SaveSnapshotToJson(const CBinaryCommand& value)
        {
            auto jsonObject = ref new Windows::Data::Json::JsonObject();
            jsonObject->SetNamedValue(L"CommandType", Windows::Data::Json::JsonValue::CreateNumberValue(static_cast<double>(value.GetCommandType())));
            jsonObject->SetNamedValue(L"Command", Windows::Data::Json::JsonValue::CreateNumberValue(value.GetCommand()));
            return jsonObject;
        }

        static Windows::Data::Json::JsonObject ^ SaveSnapshotToJson(const CParentheses& value)
        {
            auto jsonObject = ref new Windows::Data::Json::JsonObject();
            jsonObject->SetNamedValue(L"CommandType", Windows::Data::Json::JsonValue::CreateNumberValue(static_cast<double>(value.GetCommandType())));
            jsonObject->SetNamedValue(L"Command", Windows::Data::Json::JsonValue::CreateNumberValue(value.GetCommand()));
            return jsonObject;
        }

        static void RestoreJsonToSnapshot(Windows::Data::Json::JsonObject ^ jsonObject, std::optional<ApplicationSnapshot>& value)
        {
            ApplicationSnapshot applicationSnapshot;
            applicationSnapshot.SnapshotVersion = static_cast<int>(jsonObject->GetNamedNumber(L"SnapshotVersion"));
            if (applicationSnapshot.SnapshotVersion > SnapshotVersion)
            {
                return;
            }
            applicationSnapshot.Mode = static_cast<int>(jsonObject->GetNamedNumber(L"Mode"));
            if (jsonObject->HasKey(L"StandardCalculatorSnapshot"))
            {
                std::optional<StandardCalculatorSnapshot> standardCalculatorSnapshot;
                RestoreJsonToSnapshot(jsonObject->GetNamedObject(L"StandardCalculatorSnapshot"), standardCalculatorSnapshot);
                if (standardCalculatorSnapshot.has_value())
                {
                    applicationSnapshot.StandardCalc = std::move(*standardCalculatorSnapshot);
                }
                else
                {
                    return;
                }
            }
            value = std::move(applicationSnapshot);
        }

        static void RestoreJsonToSnapshot(Windows::Data::Json::JsonObject ^ jsonObject, std::optional<StandardCalculatorSnapshot>& value)
        {
            StandardCalculatorSnapshot standardCalculatorSnapshot;
            std::optional<CalculatorManagerSnapshot> calcManagerSnapshot;
            RestoreJsonToSnapshot(jsonObject->GetNamedObject(L"CalculatorManagerSnapshot"), calcManagerSnapshot);
            if (calcManagerSnapshot.has_value())
            {
                standardCalculatorSnapshot.CalcManager = std::move(*calcManagerSnapshot);
            }
            else
            {
                return;
            }
            std::optional<PrimaryDisplaySnapshot> primaryDisplaySnapshot;
            RestoreJsonToSnapshot(jsonObject->GetNamedObject(L"PrimaryDisplay"), primaryDisplaySnapshot);
            if (primaryDisplaySnapshot.has_value())
            {
                standardCalculatorSnapshot.PrimaryDisplay = std::move(*primaryDisplaySnapshot);
            }
            else
            {
                return;
            }
            if (jsonObject->HasKey(L"ExpressionDisplay"))
            {
                std::optional<ExpressionDisplaySnapshot> expressionDisplaySnapshot;
                RestoreJsonToSnapshot(jsonObject->GetNamedObject(L"ExpressionDisplay"), expressionDisplaySnapshot);
                if (expressionDisplaySnapshot.has_value())
                {
                    standardCalculatorSnapshot.ExpressionDisplay = std::move(*expressionDisplaySnapshot);
                }
                else
                {
                    return;
                }
            }
            standardCalculatorSnapshot.DisplayCommands = RestoreExpressionCommandsFromJsonArray(jsonObject->GetNamedArray(L"DisplayCommands"));
            value = std::move(standardCalculatorSnapshot);
        }

        static void RestoreJsonToSnapshot(Windows::Data::Json::JsonObject ^ jsonObject, std::optional<PrimaryDisplaySnapshot>& value)
        {
            value = PrimaryDisplaySnapshot{ jsonObject->GetNamedString(L"DisplayValue"), jsonObject->GetNamedBoolean(L"IsError") };
        }

        static void RestoreJsonToSnapshot(Windows::Data::Json::JsonObject ^ jsonObject, std::optional<ExpressionDisplaySnapshot>& value)
        {
            ExpressionDisplaySnapshot expressionDisplaySnapshot;
            expressionDisplaySnapshot.Tokens = RestoreExpressionTokensFromJsonArray(jsonObject->GetNamedArray(L"Tokens"));
            if (expressionDisplaySnapshot.Tokens.empty())
            {
                return;
            }
            expressionDisplaySnapshot.Commands = RestoreExpressionCommandsFromJsonArray(jsonObject->GetNamedArray(L"Commands"));
            if (expressionDisplaySnapshot.Commands.empty())
            {
                return;
            }
            value = std::move(expressionDisplaySnapshot);
        }

        static void RestoreJsonToSnapshot(Windows::Data::Json::JsonObject ^ jsonObject, std::optional<CalculatorManagerSnapshot>& value)
        {
            CalculatorManagerSnapshot calcManagerSnapshot;
            if (jsonObject->HasKey(L"HistoryItems"))
            {
                std::vector<std::shared_ptr<CalculationManager::HISTORYITEM>> historyItems;
                auto historyJsonArray = jsonObject->GetNamedArray(L"HistoryItems");
                for (uint32_t i = 0; i < historyJsonArray->Size; ++i)
                {
                    std::optional<CalculationManager::HISTORYITEM> historyItem;
                    RestoreJsonToSnapshot(historyJsonArray->GetObjectAt(i), historyItem);
                    if (historyItem.has_value())
                    {
                        historyItems.push_back(std::make_shared<CalculationManager::HISTORYITEM>(*historyItem));
                    }
                    else
                    {
                        return;
                    }
                }
                calcManagerSnapshot.HistoryItems = std::move(historyItems);
            }
            value = std::move(calcManagerSnapshot);
        }

        static void RestoreJsonToSnapshot(Windows::Data::Json::JsonObject ^ jsonObject, std::optional<CalculationManager::HISTORYITEM>& value)
        {
            CalculationManager::HISTORYITEM historyItem;
            historyItem.historyItemVector.expression = std::wstring(jsonObject->GetNamedString(L"Expression")->Data());
            historyItem.historyItemVector.result = std::wstring(jsonObject->GetNamedString(L"Result")->Data());
            historyItem.historyItemVector.spTokens =
                std::make_shared<std::vector<std::pair<std::wstring, int>>>(RestoreExpressionTokensFromJsonArray(jsonObject->GetNamedArray(L"Tokens")));
            if (historyItem.historyItemVector.spTokens->empty())
            {
                return;
            }
            historyItem.historyItemVector.spCommands = std::make_shared<std::vector<std::shared_ptr<IExpressionCommand>>>(
                RestoreExpressionCommandsFromJsonArray(jsonObject->GetNamedArray(L"Commands")));
            if (historyItem.historyItemVector.spCommands->empty())
            {
                return;
            }
            value = std::move(historyItem);
        }

        static void RestoreJsonToSnapshot(Windows::Data::Json::JsonObject ^ jsonObject, std::optional<std::shared_ptr<IExpressionCommand>>& value)
        {
            auto commandType = static_cast<CalculationManager::CommandType>(jsonObject->GetNamedNumber(L"CommandType"));
            switch (commandType)
            {
            case CalculationManager::CommandType::OperandCommand:
            {
                std::optional<COpndCommand> opndCommand;
                RestoreJsonToSnapshot(jsonObject, opndCommand);
                if (opndCommand.has_value())
                {
                    value = std::make_shared<COpndCommand>(*opndCommand);
                }
                break;
            }
            case CalculationManager::CommandType::UnaryCommand:
            {
                std::optional<CUnaryCommand> unaryCommand;
                RestoreJsonToSnapshot(jsonObject, unaryCommand);
                if (unaryCommand.has_value())
                {
                    value = std::make_shared<CUnaryCommand>(*unaryCommand);
                }
                break;
            }
            case CalculationManager::CommandType::BinaryCommand:
            {
                std::optional<CBinaryCommand> binaryCommand;
                RestoreJsonToSnapshot(jsonObject, binaryCommand);
                if (binaryCommand.has_value())
                {
                    value = std::make_shared<CBinaryCommand>(*binaryCommand);
                }
                break;
            }
            case CalculationManager::CommandType::Parentheses:
            {
                std::optional<CParentheses> parenthesesCommand;
                RestoreJsonToSnapshot(jsonObject, parenthesesCommand);
                if (parenthesesCommand.has_value())
                {
                    value = std::make_shared<CParentheses>(*parenthesesCommand);
                }
                break;
            }
            default:
                throw std::logic_error{ "c8cba597-dfec-447a-bd1c-e78a9ffaad95" };
            }
        }

        static void RestoreJsonToSnapshot(Windows::Data::Json::JsonObject ^ jsonObject, std::optional<COpndCommand>& value)
        {
            auto isNegative = jsonObject->GetNamedBoolean(L"IsNegative");
            auto isDecimalPresent = jsonObject->GetNamedBoolean(L"IsDecimalPresent");
            auto isSciFmt = jsonObject->GetNamedBoolean(L"IsSciFmt");
            std::vector<int> commands;
            auto commandsJsonArray = jsonObject->GetNamedArray(L"Commands");
            for (uint32_t i = 0; i < commandsJsonArray->Size; ++i)
            {
                commands.push_back(static_cast<int>(commandsJsonArray->GetNumberAt(i)));
            }
            value = COpndCommand(std::make_shared<std::vector<int>>(std::move(commands)), isNegative, isDecimalPresent, isSciFmt);
        }

        static void RestoreJsonToSnapshot(Windows::Data::Json::JsonObject ^ jsonObject, std::optional<CUnaryCommand>& value)
        {
            std::vector<int> commands;
            auto commandsJsonArray = jsonObject->GetNamedArray(L"Commands");
            if (commandsJsonArray->Size == 1)
            {
                value = CUnaryCommand(static_cast<int>(commandsJsonArray->GetNumberAt(0)));
            }
            else if (commandsJsonArray->Size == 2)
            {
                value = CUnaryCommand(static_cast<int>(commandsJsonArray->GetNumberAt(0)), static_cast<int>(commandsJsonArray->GetNumberAt(1)));
            }
        }

        static void RestoreJsonToSnapshot(Windows::Data::Json::JsonObject ^ jsonObject, std::optional<CBinaryCommand>& value)
        {
            value = CBinaryCommand(static_cast<int>(jsonObject->GetNamedNumber(L"Command")));
        }

        static void RestoreJsonToSnapshot(Windows::Data::Json::JsonObject ^ jsonObject, std::optional<CParentheses>& value)
        {
            value = CParentheses(static_cast<int>(jsonObject->GetNamedNumber(L"Command")));
        }

        static std::vector<std::pair<std::wstring, int>> RestoreExpressionTokensFromJsonArray(Windows::Data::Json::JsonArray ^ jsonArray)
        {
            std::vector<std::pair<std::wstring, int>> tokens;
            for (uint32_t i = 0; i < jsonArray->Size; ++i)
            {
                auto tokenJsonArray = jsonArray->GetArrayAt(i);
                if (tokenJsonArray->Size == 2 && tokenJsonArray->GetAt(0)->ValueType == Windows::Data::Json::JsonValueType::String
                    && tokenJsonArray->GetAt(1)->ValueType == Windows::Data::Json::JsonValueType::Number)
                {
                    tokens.emplace_back(std::wstring(tokenJsonArray->GetAt(0)->GetString()->Data()), static_cast<int>(tokenJsonArray->GetAt(1)->GetNumber()));
                }
                else
                {
                    return {};
                }
            }
            return tokens;
        }

        static std::vector<std::shared_ptr<IExpressionCommand>> RestoreExpressionCommandsFromJsonArray(Windows::Data::Json::JsonArray ^ jsonArray)
        {
            std::vector<std::shared_ptr<IExpressionCommand>> commands;
            for (uint32_t i = 0; i < jsonArray->Size; ++i)
            {
                std::optional<std::shared_ptr<IExpressionCommand>> command;
                RestoreJsonToSnapshot(jsonArray->GetObjectAt(i), command);
                if (command.has_value())
                {
                    commands.push_back(*command);
                }
                else
                {
                    return {};
                }
            }
            return commands;
        }

        static bool IsJsonParsingException(Platform::COMException ^ e)
        {
            return e->HResult == WEB_E_JSON_VALUE_NOT_FOUND || e->HResult == E_ILLEGAL_METHOD_CALL;
        }
    };
}

ApplicationViewModel::ApplicationViewModel()
    : m_CalculatorViewModel(nullptr)
    , m_DateCalcViewModel(nullptr)
    , m_GraphingCalcViewModel(nullptr)
    , m_ConverterViewModel(nullptr)
    , m_PreviousMode(ViewMode::None)
    , m_mode(ViewMode::None)
    , m_categories(nullptr)
{
    SetMenuCategories();
}

void ApplicationViewModel::Mode::set(ViewMode value)
{
    if (m_mode != value)
    {
        PreviousMode = m_mode;
        m_mode = value;
        SetDisplayNormalAlwaysOnTopOption();
        OnModeChanged();
        RaisePropertyChanged(ModePropertyName);
    }
}

void ApplicationViewModel::Categories::set(IObservableVector<NavCategoryGroup ^> ^ value)
{
    if (m_categories != value)
    {
        m_categories = value;
        RaisePropertyChanged(CategoriesPropertyName);
    }
}

void ApplicationViewModel::Initialize(ViewMode mode)
{
    if (!NavCategoryStates::IsValidViewMode(mode) || !NavCategoryStates::IsViewModeEnabled(mode))
    {
        mode = ViewMode::Standard;
    }

    try
    {
        Mode = mode;
    }
    catch (const std::exception& e)
    {
        TraceLogger::GetInstance()->LogStandardException(mode, __FUNCTIONW__, e);
        if (!TryRecoverFromNavigationModeFailure())
        {
            // Could not navigate to standard mode either.
            // Throw the original exception so we have a good stack to debug.
            throw;
        }
    }
    catch (Exception ^ e)
    {
        TraceLogger::GetInstance()->LogPlatformException(mode, __FUNCTIONW__, e);
        if (!TryRecoverFromNavigationModeFailure())
        {
            // Could not navigate to standard mode either.
            // Throw the original exception so we have a good stack to debug.
            throw;
        }
    }
}

bool ApplicationViewModel::TryRecoverFromNavigationModeFailure()
{
    // Here we are simply trying to recover from being unable to navigate to a mode.
    // Try falling back to standard mode and if there are *any* exceptions, we should
    // fail because something is seriously wrong.
    try
    {
        Mode = ViewMode::Standard;
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void ApplicationViewModel::OnModeChanged()
{
    assert(NavCategoryStates::IsValidViewMode(m_mode));
    if (NavCategory::IsCalculatorViewMode(m_mode))
    {
        if (!m_CalculatorViewModel)
        {
            m_CalculatorViewModel = ref new StandardCalculatorViewModel();
        }
        m_CalculatorViewModel->SetCalculatorType(m_mode);
    }
    else if (NavCategory::IsGraphingCalculatorViewMode(m_mode))
    {
        if (!m_GraphingCalcViewModel)
        {
            m_GraphingCalcViewModel = ref new GraphingCalculatorViewModel();
        }
    }
    else if (NavCategory::IsDateCalculatorViewMode(m_mode))
    {
        if (!m_DateCalcViewModel)
        {
            m_DateCalcViewModel = ref new DateCalculatorViewModel();
        }
    }
    else if (NavCategory::IsConverterViewMode(m_mode))
    {
        if (!m_ConverterViewModel)
        {
            auto dataLoader = std::make_shared<UnitConverterDataLoader>(ref new GeographicRegion());
            auto currencyDataLoader = std::make_shared<CurrencyDataLoader>(std::make_unique<CurrencyHttpClient>());
            m_ConverterViewModel = ref new UnitConverterViewModel(std::make_shared<UnitConversionManager::UnitConverter>(dataLoader, currencyDataLoader));
        }

        m_ConverterViewModel->Mode = m_mode;
    }

    auto resProvider = AppResourceProvider::GetInstance();
    CategoryName = resProvider->GetResourceString(NavCategoryStates::GetNameResourceKey(m_mode));

    // Cast mode to an int in order to save it to app data.
    // Save the changed mode, so that the new window launches in this mode.
    // Don't save until after we have adjusted to the new mode, so we don't save a mode that fails to load.
    ApplicationData::Current->LocalSettings->Values->Insert(ModePropertyName, NavCategoryStates::Serialize(m_mode));

    // Log ModeChange event when not first launch, log WindowCreated on first launch
    if (NavCategoryStates::IsValidViewMode(m_PreviousMode))
    {
        TraceLogger::GetInstance()->LogModeChange(m_mode);
    }
    else
    {
        TraceLogger::GetInstance()->LogWindowCreated(m_mode, ApplicationView::GetApplicationViewIdForWindow(CoreWindow::GetForCurrentThread()));
    }

    RaisePropertyChanged(ClearMemoryVisibilityPropertyName);
}

void ApplicationViewModel::OnCopyCommand(Object ^ parameter)
{
    if (NavCategory::IsConverterViewMode(m_mode))
    {
        ConverterViewModel->OnCopyCommand(parameter);
    }
    else if (NavCategory::IsDateCalculatorViewMode(m_mode))
    {
        DateCalcViewModel->OnCopyCommand(parameter);
    }
    else if (NavCategory::IsCalculatorViewMode(m_mode))
    {
        CalculatorViewModel->OnCopyCommand(parameter);
    }
}

void ApplicationViewModel::OnPasteCommand(Object ^ parameter)
{
    if (NavCategory::IsConverterViewMode(m_mode))
    {
        ConverterViewModel->OnPasteCommand(parameter);
    }
    else if (NavCategory::IsCalculatorViewMode(m_mode))
    {
        CalculatorViewModel->OnPasteCommand(parameter);
    }
}

void ApplicationViewModel::SetMenuCategories()
{
    // Use the Categories property instead of the backing variable
    // because we want to take advantage of binding updates and
    // property setter logic.
    Categories = NavCategoryStates::CreateMenuOptions();
}

void ApplicationViewModel::ToggleAlwaysOnTop(float width, float height)
{
    HandleToggleAlwaysOnTop(width, height);
}

#pragma optimize("", off)
task<void> ApplicationViewModel::HandleToggleAlwaysOnTop(float width, float height)
{
    if (ApplicationView::GetForCurrentView()->ViewMode == ApplicationViewMode::CompactOverlay)
    {
        ApplicationDataContainer ^ localSettings = ApplicationData::Current->LocalSettings;
        localSettings->Values->Insert(WidthLocalSettings, width);
        localSettings->Values->Insert(HeightLocalSettings, height);

        bool success = co_await ApplicationView::GetForCurrentView()->TryEnterViewModeAsync(ApplicationViewMode::Default);
        CalculatorViewModel->HistoryVM->AreHistoryShortcutsEnabled = success;
        CalculatorViewModel->IsAlwaysOnTop = !success;
        IsAlwaysOnTop = !success;
    }
    else
    {
        ApplicationDataContainer ^ localSettings = ApplicationData::Current->LocalSettings;
        ViewModePreferences ^ compactOptions = ViewModePreferences::CreateDefault(ApplicationViewMode::CompactOverlay);
        if (!localSettings->Values->GetView()->HasKey(LaunchedLocalSettings))
        {
            compactOptions->CustomSize = Size(320, 394);
            localSettings->Values->Insert(LaunchedLocalSettings, true);
        }
        else
        {
            if (localSettings->Values->GetView()->HasKey(WidthLocalSettings) && localSettings->Values->GetView()->HasKey(HeightLocalSettings))
            {
                float oldWidth = safe_cast<IPropertyValue ^>(localSettings->Values->GetView()->Lookup(WidthLocalSettings))->GetSingle();
                float oldHeight = safe_cast<IPropertyValue ^>(localSettings->Values->GetView()->Lookup(HeightLocalSettings))->GetSingle();
                compactOptions->CustomSize = Size(oldWidth, oldHeight);
            }
            else
            {
                compactOptions->CustomSize = Size(320, 394);
            }
        }

        bool success = co_await ApplicationView::GetForCurrentView()->TryEnterViewModeAsync(ApplicationViewMode::CompactOverlay, compactOptions);
        CalculatorViewModel->HistoryVM->AreHistoryShortcutsEnabled = !success;
        CalculatorViewModel->IsAlwaysOnTop = success;
        IsAlwaysOnTop = success;
    }
    SetDisplayNormalAlwaysOnTopOption();
};
#pragma optimize("", on)

void ApplicationViewModel::SetDisplayNormalAlwaysOnTopOption()
{
    DisplayNormalAlwaysOnTopOption =
        m_mode == ViewMode::Standard && ApplicationView::GetForCurrentView()->IsViewModeSupported(ApplicationViewMode::CompactOverlay) && !IsAlwaysOnTop;
}

Windows::Data::Json::JsonObject ^ ApplicationViewModel::SaveApplicationSnapshot()
{
    ApplicationSnapshot applicationSnapshot;
    applicationSnapshot.SnapshotVersion = SnapshotHelper::SnapshotVersion;
    applicationSnapshot.Mode = static_cast<int>(Mode);
    if (m_CalculatorViewModel != nullptr && m_mode == ViewMode::Standard)
    {
        // Standard calculator is the only supported mode so far.
        applicationSnapshot.StandardCalc = m_CalculatorViewModel->GetStandardCalculatorSnapshot();
    }
    return SnapshotHelper::SaveSnapshotToJson(applicationSnapshot);
}

bool ApplicationViewModel::TryRestoreFromSnapshot(Windows::Data::Json::JsonObject ^ jsonObject)
{
    std::optional<ApplicationSnapshot> applicationSnapshot;
    try
    {
        SnapshotHelper::RestoreJsonToSnapshot(jsonObject, applicationSnapshot);
    }
    catch (Platform::COMException ^ e)
    {
        if (SnapshotHelper::IsJsonParsingException(e))
        {
            return false;
        }
        throw;
    }

    if (applicationSnapshot.has_value())
    {
        Mode = static_cast<ViewMode>(applicationSnapshot->Mode);
        if (applicationSnapshot->StandardCalc.has_value())
        {
            if (m_CalculatorViewModel == nullptr)
            {
                m_CalculatorViewModel = ref new StandardCalculatorViewModel();
            }
            m_CalculatorViewModel->SetStandardCalculatorSnapshot(applicationSnapshot->StandardCalc.value());
        }
        return true;
    }
    return false;
}
