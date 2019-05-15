#pragma once

#include "CalculatorHistory.h"
#include "CalculatorManager.h"
#include "headers/CalcEngine.h"
#include "headers/Rational.h"
#include "headers/ICalcDisplay.h"


struct TokenPair {
	char* Item1;
	int Item2;
};

struct ExpressionDisplayData {
	int TokenCount;
	TokenPair* Tokens;

	int CommandCount;
	void* Commands;
};

typedef void (*SetPrimaryDisplayFunc)(void* state, const char* text, bool isError);
typedef void (*SetIsInErrorFunc)(void* state, bool isInError);
typedef void (*SetExpressionDisplayFunc)(void* state, ExpressionDisplayData* data);
typedef void (*SetParenthesisNumberFunc)(void* state, unsigned int count);
typedef void (*OnNoRightParenAddedFunc)(void* state);
typedef void (*MaxDigitsReachedFunc)(void* state);
typedef void (*BinaryOperatorReceivedFunc)(void* state);
typedef void (*OnHistoryItemAddedFunc)(void* state, unsigned int addedItemIndex);
typedef void (*SetMemorizedNumbersFunc)(void* state, unsigned int count, const char** memorizedNumbers);
typedef void (*MemoryItemChangedFunc)(void* state, unsigned int indexOfMemory);

typedef const char* (*GetCEngineStringFunc)(void* state, const char* id);

struct CalculatorManager_CreateParams {
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

#if defined(__EMSCRIPTEN__)
#define DLL_EXPORT 
#else
#define DLL_EXPORT __declspec(dllexport)
#endif

extern "C" {
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
    DLL_EXPORT const char* CalculatorManager_GetResultForRadix(void* manager, int radix, int precision);
    DLL_EXPORT void CalculatorManager_SetPrecision(void* manager, int precision);
    DLL_EXPORT void CalculatorManager_UpdateMaxIntDigits(void* manager);
    DLL_EXPORT const char* CalculatorManager_DecimalSeparator(void* manager);
    DLL_EXPORT bool CalculatorManager_RemoveHistoryItem(void* manager, int uIdx);
    DLL_EXPORT void CalculatorManager_ClearHistory(void* manager);
    DLL_EXPORT size_t CalculatorManager_MaxHistorySize(void* manager);
    DLL_EXPORT int CalculatorManager_GetCurrentDegreeMode(void* manager);
    DLL_EXPORT void CalculatorManager_SetInHistoryItemLoadMode(void* manager, bool isHistoryItemLoadMode);
}

