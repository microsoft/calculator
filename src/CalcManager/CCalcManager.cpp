#include "pch.h"
#include "CCalcManager.h"
#include "CalculatorManager.h"
#include "CalculatorResource.h"
#include <codecvt>

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
			strncpy_s(pData, str.size(), str.data(), str.size());
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

void* CalculatorManager_Create(CalculatorManager_CreateParams* pParams) {

	auto calcDisplay = new CalcDisplay(*pParams);
	auto resProvider = new ResourceProvider(*pParams);

	auto cm = new CalculatorManager(calcDisplay, resProvider);
	return cm;
}

void CalculatorManager_SendCommand(void* manager, int command) {
	(static_cast<CalculatorManager*>(manager))->SendCommand((Command)command);
}
