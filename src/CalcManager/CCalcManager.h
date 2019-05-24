#pragma once

#include "CalculatorHistory.h"
#include "CalculatorManager.h"
#include "headers/CalcEngine.h"
#include "headers/Rational.h"
#include "headers/ICalcDisplay.h"

struct GetHistoryItemResult;
struct GetHistoryItemsResult;

typedef void (*SetPrimaryDisplayFunc)(void* state, const wchar_t* text, bool isError);
typedef void (*SetIsInErrorFunc)(void* state, bool isInError);
typedef void (*SetExpressionDisplayFunc)(void* state, GetHistoryItemResult* data);
typedef void (*SetParenthesisNumberFunc)(void* state, unsigned int count);
typedef void (*OnNoRightParenAddedFunc)(void* state);
typedef void (*MaxDigitsReachedFunc)(void* state);
typedef void (*BinaryOperatorReceivedFunc)(void* state);
typedef void (*OnHistoryItemAddedFunc)(void* state, unsigned int addedItemIndex);
typedef void (*SetMemorizedNumbersFunc)(void* state, unsigned int count, const wchar_t** memorizedNumbers);
typedef void (*MemoryItemChangedFunc)(void* state, unsigned int indexOfMemory);

typedef const wchar_t* (*GetCEngineStringFunc)(void* state, const wchar_t* id);

struct CalculatorManager_CreateParams
{
    void* CalculatorState;

    SetPrimaryDisplayFunc SetPrimaryDisplay;
    SetIsInErrorFunc SetIsInError;
    SetExpressionDisplayFunc SetExpressionDisplay;
    SetParenthesisNumberFunc SetParenthesisNumber;
    OnNoRightParenAddedFunc OnNoRightParenAdded;
    MaxDigitsReachedFunc MaxDigitsReached;
    BinaryOperatorReceivedFunc BinaryOperatorReceived;
    OnHistoryItemAddedFunc OnHistoryItemAdded;
    SetMemorizedNumbersFunc SetMemorizedNumbers;
    MemoryItemChangedFunc MemoryItemChanged;

    void* ResourceState;
    GetCEngineStringFunc GetCEngineString;
};

#if defined(__EMSCRIPTEN__) || defined(__APPLE__) || defined(__ANDROID__)
#define DLL_EXPORT 
#else
#define DLL_EXPORT __declspec(dllexport)
#endif

GetHistoryItemResult* MarshalHistoryItem(std::shared_ptr<CalculationManager::HISTORYITEM>& historyItem);
void* MarshalHistoryItems(std::vector<std::shared_ptr<CalculationManager::HISTORYITEM>>& historyItems);

extern "C"
{
    struct GetHistoryItemsResult
    {
        int32_t ItemsCount;
        void* HistoryItems;
    };

    struct GetHistoryItemResult
    {
        const wchar_t* expression;
        const wchar_t* result;

        int32_t TokenCount;
        const wchar_t** TokenStrings;
        int32_t* TokenValues;

        int32_t CommandCount;
        void** Commands;
    };

    struct COpndCommand_GetCommandsResult
    {
        int32_t CommandCount;
        int32_t* pCommands;
    };

    struct CUnaryCommand_GetCommandsResult
    {
        int32_t CommandCount;
        int32_t* pCommands;
    };

    DLL_EXPORT void* CalculatorManager_Create(CalculatorManager_CreateParams* params);
    DLL_EXPORT void CalculatorManager_SendCommand(void* manager, int command);
    DLL_EXPORT void CalculatorManager_SetRadix(void* manager, RADIX_TYPE iRadixType);
    DLL_EXPORT void CalculatorManager_Reset(void* manager, bool clearMemory);
    DLL_EXPORT void CalculatorManager_SetStandardMode(void* manager);
    DLL_EXPORT void CalculatorManager_SetScientificMode(void* manager);
    DLL_EXPORT void CalculatorManager_SetProgrammerMode(void* manager);
    DLL_EXPORT void CalculatorManager_MemorizeNumber(void* manager);
    DLL_EXPORT void CalculatorManager_MemorizedNumberLoad(void* manager, int value);
    DLL_EXPORT void CalculatorManager_MemorizedNumberAdd(void* manager, int value);
    DLL_EXPORT void CalculatorManager_MemorizedNumberSubtract(void* manager, int value);
    DLL_EXPORT void CalculatorManager_MemorizedNumberClear(void* manager, int value);
    DLL_EXPORT void CalculatorManager_MemorizedNumberClearAll(void* manager);
    DLL_EXPORT bool CalculatorManager_IsEngineRecording(void* manager);
    DLL_EXPORT void CalculatorManager_SetMemorizedNumbersString(void* manager);
    DLL_EXPORT const wchar_t* CalculatorManager_GetResultForRadix(void* manager, int radix, int precision);
    DLL_EXPORT void CalculatorManager_SetPrecision(void* manager, int precision);
    DLL_EXPORT void CalculatorManager_UpdateMaxIntDigits(void* manager);
    DLL_EXPORT const wchar_t* CalculatorManager_DecimalSeparator(void* manager);
    DLL_EXPORT bool CalculatorManager_RemoveHistoryItem(void* manager, int uIdx);
    DLL_EXPORT void CalculatorManager_ClearHistory(void* manager);
    DLL_EXPORT size_t CalculatorManager_MaxHistorySize(void* manager);
    DLL_EXPORT int CalculatorManager_GetCurrentDegreeMode(void* manager);
    DLL_EXPORT void CalculatorManager_SetInHistoryItemLoadMode(void* manager, bool isHistoryItemLoadMode);
    DLL_EXPORT void* CalculatorManager_GetHistoryItemsWithMode(void* manager, int mode);
    DLL_EXPORT void* CalculatorManager_GetHistoryItems(void* manager);
    DLL_EXPORT void* CalculatorManager_GetHistoryItem(void* manager, int index);

    DLL_EXPORT void Free(void* ptr);

    DLL_EXPORT int IExpressionCommand_GetCommandType(void* pExpressionCommand);

    DLL_EXPORT void* COpndCommand_GetCommands(void* pExpressionCommand);
    DLL_EXPORT bool COpndCommand_IsNegative(void* pExpressionCommand);

    DLL_EXPORT void* CUnaryCommand_GetCommands(void* pExpressionCommand);

	DLL_EXPORT int CBinaryCommand_GetCommand(void* pExpressionCommand);
}
