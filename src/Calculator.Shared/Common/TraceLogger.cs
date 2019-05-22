using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Text;
using Windows.Globalization;
using CalculatorApp.Common;

namespace CalculatorApp
{
	public class TraceLogger
	{
		private static TraceLogger _instance = new TraceLogger();
		public static TraceLogger GetInstance() => _instance;


		public bool GetTraceLoggingProviderEnabled() => false;

		public void LogAppLaunchStart()  {}
		public void LogAppLaunchComplete()  {}
		public void LogAppResumeComplete()  {}
		//public void LogOnAppLaunch(std::wstring_view previousExecutionState)  {}
		public void LogMemoryClearAll(int i)  {}
		public void LogBitFlipPaneClicked()  {}
		public void LogBitFlipUsed()  {}
		public void LogHistoryBodyOpened()  {}
		public void LogHistoryItemLoadBegin()  {}
		public void LogHistoryItemLoadEnd(uint i)  {}
		public void LogHistoryFlyoutOpenBegin(uint i)  {}
		public void LogHistoryFlyoutOpenEnd(int i)  {}
		public void LogCalculatorModeViewed(ViewMode mode, int i)  {}
		public void LogDateCalculatorModeViewed(ViewMode mode, int i)  {}
		public void LogConverterModeViewed(ViewMode mode, int i)  {}
		public void LogModeChangeBegin(ViewMode oldMode, ViewMode newMode, int i)  {}
		public void LogModeChangeEnd(ViewMode mode, int i)  {}
		public void LogClearHistory()  {}
		public void InsertintoMemoryMap(int i, bool b1, bool b2, bool b3)  {}
		public void UpdateMemoryMap(int i1, int i2, bool b1, bool b2, bool b3)  {}
		public void DeleteFromMemoryMap(int i1, int i2)  {}
		public void LogMemoryUsed(int i1, uint i2, bool b1, bool b2, bool b3, uint i4)  {}
		public void LogMultipleMemoryUsed(uint i1, uint i2)  {}
		public void LogSingleMemoryUsed(uint i)  {}
		//public void LogSharedMemoryUsed(std::wstring_view, std::wstring_view, uint i)  {}
		public void LogMemoryBodyOpened()  {}
		public void LogMemoryFlyoutOpenBegin(uint i)  {}
		//public void LogDebug(std::wstring_view debugData)  {}
		public void LogMemoryFlyoutOpenEnd(uint i)  {}
		//public void LogInvalidPastedInputOccurred(std::wstring_view reason, ViewMode mode, int ProgrammerNumberBase, int bitLengthType)  {}
		public void LogValidInputPasted(ViewMode mode)  {}
		public void UpdateFunctionUsage(int func)  {}
		public void LogFunctionUsage(int i)  {}
		public void InitFunctionLogArray()  {}
		public void LogBitLengthButtonUsed(int windowId)  {}
		public void LogRadixButtonUsed(int windowId)  {}
		public void LogAngleButtonUsed(int windowId)  {}
		public void LogHypButtonUsed(int windowId)  {}
		public void LogNewWindowCreationBegin(int windowId)  {}
		public void LogNewWindowCreationEnd(int windowId)  {}
		//public void LogError(std::wstring_view errorString)  {}
		public void LogPrelaunchedAppActivatedByUser()  {}
		public void LogAppPrelaunchedBySystem()  {}
		//public void UpdateWindowCount(size_t windowCount)  {}
		public bool UpdateWindowIdLog(int windowId) => true;
		public void LogMaxWindowCount()  {}
		public void LogWindowActivated()  {}
		public void LogWindowLaunched()  {}
		public void LogUserRequestedRefreshFailed()  {}
		public void LogConversionResult(string fromValue, string fromUnit, string toValue, string toUnit)  {}
		public void LogAboutFlyoutOpened()  {}
		public void LogNavBarOpened()  {}
		public void LogViewClosingTelemetry(int i)  {}
		public void LogCoreWindowWasNull()  {}
		
		// Trace methods for Date Calculator usage
		public void LogDateDifferenceModeUsed(int windowId)  {}
		public void LogDateAddSubtractModeUsed(int windowId, bool isAddMode)  {}
		public void LogDateClippedTimeDifferenceFound(Calendar today, DateTime clippedTime)  {}
		
		public void LogStandardException(Exception e, [CallerMemberName] string functionName = null) { }
		public void LogWinRTException(Exception e, [CallerMemberName] string functionName = null) { }
		public void LogPlatformException(Exception e, [CallerMemberName] string functionName = null)  {}
	}
}
