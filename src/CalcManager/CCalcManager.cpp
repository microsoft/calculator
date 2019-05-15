#define __STDC_WANT_LIB_EXT1__ 1
#include "pch.h"
#include "CCalcManager.h"
#include "CalculatorManager.h"
#include "CalculatorResource.h"
#include <codecvt>
#include <locale>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace CalculationManager;

class CalcDisplay : public ICalcDisplay {

private:
	CalculatorManager_CreateParams _params;

public:
	CalcDisplay(CalculatorManager_CreateParams params) {
		_params = params;
	}

	// Inherited via ICalcDisplay
	virtual void SetPrimaryDisplay(const std::wstring& pszText, bool isError) override
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
		auto str = convert.to_bytes(pszText);

		_params.SetPrimaryDisplay(_params.CalculatorState, str.data(), isError);
	}

	virtual void SetIsInError(bool isInError) override
	{
		_params.SetIsInError(_params.CalculatorState, isInError);
	}

	virtual void SetExpressionDisplay(
		std::shared_ptr<CalculatorVector<std::pair<std::wstring, int>>> const& /*tokens*/,
		std::shared_ptr<CalculatorVector<std::shared_ptr<IExpressionCommand>>> const& /*commands*/) override
	{
	}

	virtual void SetParenthesisNumber(unsigned int count) override
	{
		_params.SetParenthesisNumber(_params.CalculatorState, count);
	}

	virtual void OnNoRightParenAdded() override
	{
		_params.OnNoRightParenAdded(_params.CalculatorState);
	}

	virtual void MaxDigitsReached() override
	{
		_params.MaxDigitsReached(_params.CalculatorState);
	}

	virtual void BinaryOperatorReceived() override
	{
		_params.BinaryOperatorReceived(_params.CalculatorState);
	}

	virtual void OnHistoryItemAdded(unsigned int addedItemIndex) override
	{
		_params.OnHistoryItemAdded(_params.CalculatorState, addedItemIndex);
	}

	virtual void SetMemorizedNumbers(const std::vector<std::wstring>& memorizedNumbers) override
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;

		const char** numbers = new const char* [memorizedNumbers.size()];

		for (size_t i = 0; i < memorizedNumbers.size(); i++)
		{
			auto str = convert.to_bytes(memorizedNumbers[i]);
			auto pData = new char[str.size() + 1];

#if !defined(__EMSCRIPTEN__)
			strcpy_s(pData, str.size()+1, str.data());
#else
            strcpy(pData, str.data());
#endif
			numbers[i] = pData;
		}

		_params.SetMemorizedNumbers(_params.CalculatorState, (unsigned int)memorizedNumbers.size(), numbers);

		for (size_t i = 0; i < memorizedNumbers.size(); i++)
		{
			delete[] numbers[i];
		}

		delete[] numbers;
	}

	virtual void MemoryItemChanged(unsigned int indexOfMemory) override
	{
		_params.MemoryItemChanged(_params.CalculatorState, indexOfMemory);
	}
};

class ResourceProvider : public CalculationManager::IResourceProvider {
private:
	CalculatorManager_CreateParams _params;

public:
	ResourceProvider(CalculatorManager_CreateParams params)
	{
		_params = params;
	}

	virtual std::wstring GetCEngineString(const std::wstring& id) override {

		std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
		auto str = convert.to_bytes(id);

		auto res = _params.GetCEngineString(_params.ResourceState, str.data());

		return convert.from_bytes(res);
	}
};

CalculatorManager* AsManager(void* manager)
{
    return static_cast<CalculatorManager*>(manager);
}

void* CalculatorManager_Create(CalculatorManager_CreateParams* pParams)
{
	auto calcDisplay = new CalcDisplay(*pParams);
	auto resProvider = new ResourceProvider(*pParams);

	auto cm = new CalculatorManager(calcDisplay, resProvider);
	return cm;
}

void CalculatorManager_SendCommand(void* manager, int command)
{
    AsManager(manager)->SendCommand((Command)command);
}

void CalculatorManager_SetRadix(void* manager, RADIX_TYPE iRadixType)
{
    AsManager(manager)->SetRadix(iRadixType);
}

void CalculatorManager_Reset(void* manager, bool clearMemory)
{
    AsManager(manager)->Reset(clearMemory);
}

void CalculatorManager_SetStandardMode(void* manager)
{
    AsManager(manager)->SetStandardMode();
}

void CalculatorManager_SetScientificMode(void* manager)
{
    AsManager(manager)->SetScientificMode();
}

void CalculatorManager_SetProgrammerMode(void* manager)
{
    AsManager(manager)->SetProgrammerMode();
}

void CalculatorManager_MemorizeNumber(void* manager)
{
    AsManager(manager)->MemorizeNumber();
}

void CalculatorManager_MemorizedNumberLoad(void* manager, int value)
{
    AsManager(manager)->MemorizedNumberLoad(value);
}

void CalculatorManager_MemorizedNumberAdd(void* manager, int value)
{
    AsManager(manager)->MemorizedNumberAdd(value);
}

void CalculatorManager_MemorizedNumberSubtract(void* manager, int value)
{
    AsManager(manager)->MemorizedNumberSubtract(value);
}

void CalculatorManager_MemorizedNumberClear(void* manager, int value)
{
    AsManager(manager)->MemorizedNumberClear(value);
}

void CalculatorManager_MemorizedNumberClearAll(void* manager)
{
    AsManager(manager)->MemorizedNumberClearAll();
}

bool CalculatorManager_IsEngineRecording(void* manager)
{
    return AsManager(manager)->IsEngineRecording();
}

void CalculatorManager_SetMemorizedNumbersString(void* manager)
{
    AsManager(manager)->SetMemorizedNumbersString();
}

const char* CalculatorManager_GetResultForRadix(void* manager, int radix, int precision)
{
    auto res = AsManager(manager)->GetResultForRadix(radix, precision);

	std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    auto str = convert.to_bytes(res);

	return str.data();
}

void CalculatorManager_SetPrecision(void* manager, int precision)
{
    AsManager(manager)->SetPrecision(precision);
}

void CalculatorManager_UpdateMaxIntDigits(void* manager)
{
    AsManager(manager)->UpdateMaxIntDigits();
}

const char* CalculatorManager_DecimalSeparator(void* manager)
{
    auto res = AsManager(manager)->DecimalSeparator();

    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    auto str = convert.to_bytes(res);

	return str.data();
}

bool CalculatorManager_RemoveHistoryItem(void* manager, int uIdx)
{
    return AsManager(manager)->RemoveHistoryItem(uIdx);
}

void CalculatorManager_ClearHistory(void* manager)
{
    AsManager(manager)->ClearHistory();
}

size_t CalculatorManager_MaxHistorySize(void* manager)
{
    return AsManager(manager)->MaxHistorySize();
}

int CalculatorManager_GetCurrentDegreeMode(void* manager)
{
    return (int)AsManager(manager)->GetCurrentDegreeMode();
}

void CalculatorManager_SetInHistoryItemLoadMode(void* manager, bool isHistoryItemLoadMode)
{
    AsManager(manager)->SetInHistoryItemLoadMode(isHistoryItemLoadMode);
}
