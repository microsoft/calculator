// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Windows.Input;
using Windows.Foundation.Metadata;
using Windows.Globalization.NumberFormatting;
using Windows.Storage;
using Windows.System.Threading;
using Windows.UI.Core;
using Windows.UI.Xaml;
using CalculationManager;
using Calculator;
using CalculatorApp;
using CalculatorApp.Common;
using CalculatorApp.Common.Automation;
using UnitConversionManager;
using UCM = UnitConversionManager;
using CategorySelectionInitializer = System.Tuple<CalculatorApp.CalculatorList<UnitConversionManager.Unit>, UnitConversionManager.Unit, UnitConversionManager.Unit>;
using UnitToUnitToConversionDataMap = System.Collections.Generic.Dictionary<UnitConversionManager.Unit, System.Collections.Generic.Dictionary<UnitConversionManager.Unit, UnitConversionManager.ConversionData>>;
using CategoryToUnitVectorMap = System.Collections.Generic.Dictionary<UnitConversionManager.Category, CalculatorApp.CalculatorList<UnitConversionManager.Unit>>;
using System.Globalization;
using Windows.System;

namespace CalculatorApp.ViewModel
{


	[Windows.UI.Xaml.Data.Bindable]
	public sealed class Category : INotifyPropertyChanged
	{
		internal Category(UnitConversionManager.Category category)
		{
			m_original = category;

		}

		public event PropertyChangedEventHandler PropertyChanged;

		private void RaisePropertyChanged(
			[System.Runtime.CompilerServices.CallerMemberName]
			string p = null) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(p));

		public string Name
		{
			get { return m_original.name; }
		}

		public Windows.UI.Xaml.Visibility NegateVisibility
		{
			get { return m_original.supportsNegative ? Windows.UI.Xaml.Visibility.Visible : Windows.UI.Xaml.Visibility.Collapsed; }
		}

		internal UnitConversionManager.Category GetModelCategory()
		{
			return m_original;
		}

		private UnitConversionManager.Category m_original;
	};

	[Windows.UI.Xaml.Data.Bindable]
	public sealed class Unit : INotifyPropertyChanged
	{
		internal Unit(UnitConversionManager.Unit unit)
		{
			m_original = unit;
		}

		public event PropertyChangedEventHandler PropertyChanged;

		private void RaisePropertyChanged(
			[System.Runtime.CompilerServices.CallerMemberName]
			string p = null) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(p));

		public string Name
		{
			get { return m_original.name; }
		}

		public string AccessibleName
		{
			get { return m_original.accessibleName; }
		}

		public string Abbreviation
		{
			get { return m_original.abbreviation; }
		}

		// This method is used to return the desired automation name for default unit in UnitConverter combo box.
		public override string ToString()
		{
			return AccessibleName;
		}

		internal UnitConversionManager.Unit GetModelUnit()
		{
			return m_original;
		}

		private UnitConversionManager.Unit m_original;
	}

	[Windows.UI.Xaml.Data.Bindable]
	public sealed class SupplementaryResult : INotifyPropertyChanged
	{
		internal SupplementaryResult(string value, Unit unit)
		{
			m_Value = value;
			m_Unit = unit;

		}

		public bool IsWhimsical()
		{
			return m_Unit.GetModelUnit().isWhimsical;
		}

		public string GetLocalizedAutomationName()
		{
			// TODO UNO
			return "TODO UNO";
		}

		public event PropertyChangedEventHandler PropertyChanged;

		private void RaisePropertyChanged(
			[System.Runtime.CompilerServices.CallerMemberName]
			string p = null) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(p));

		private string m_Value;

		public string Value
		{
			get => m_Value;
			private set
			{
				m_Value = value;
				RaisePropertyChanged("Value");
			}
		}


		private CalculatorApp.ViewModel.Unit m_Unit;

		public CalculatorApp.ViewModel.Unit Unit
		{
			get => m_Unit;
			private set
			{
				m_Unit = value;
				RaisePropertyChanged("Unit");
			}
		}
	}

	public interface IActivatable
	{
		bool IsActive { get; set; }
	};

	public sealed class Activatable<TActivatable> : IActivatable
		where TActivatable : IActivatable
	{
		TActivatable m_activatable;

		public Activatable(TActivatable activatable)
		{
			m_activatable = activatable;
		}

		public bool IsActive
		{
			get { return m_activatable.IsActive; }
			set { m_activatable.IsActive = value; }
		}
	};


	[Windows.UI.Xaml.Data.Bindable]
	public sealed partial class UnitConverterViewModel : INotifyPropertyChanged
	{


		public event PropertyChangedEventHandler PropertyChanged;

		private void RaisePropertyChanged(
			[System.Runtime.CompilerServices.CallerMemberName]
			string p = null)
		{
			PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(p));
			OnPropertyChanged(p);
		}

		private CalculatorObservableCollection<Category> m_Categories;

		public CalculatorObservableCollection<Category> Categories
		{
			get => m_Categories;
			private set
			{
				if (m_Categories != value)
				{
					m_Categories = value;
					RaisePropertyChanged("Categories");
				}
			}
		}


		private Category m_CurrentCategory;

		public Category CurrentCategory
		{
			get => m_CurrentCategory;
			set
			{
				if (m_CurrentCategory != value)
				{
					m_CurrentCategory = value;
					RaisePropertyChanged("CurrentCategory");
				}
			}
		}


		private CalculatorApp.Common.ViewMode m_Mode;

		public CalculatorApp.Common.ViewMode Mode
		{
			get => m_Mode;
			set
			{
				if (m_Mode != value)
				{
					m_Mode = value;
					RaisePropertyChanged("Mode");
				}
			}
		}


		private CalculatorObservableCollection<Unit> m_Units;

		public CalculatorObservableCollection<Unit> Units
		{
			get => m_Units;
			private set
			{
				if (m_Units != value)
				{
					m_Units = value;
					RaisePropertyChanged("Units");
				}
			}
		}


		private string m_CurrencySymbol1;

		public string CurrencySymbol1
		{
			get => m_CurrencySymbol1;
			set
			{
				if (m_CurrencySymbol1 != value)
				{
					m_CurrencySymbol1 = value;
					RaisePropertyChanged("CurrencySymbol1"); 
				}
			}
		}


		private Unit m_Unit1;

		public Unit Unit1
		{
			get => m_Unit1;
			set
			{
				if (m_Unit1 != value)
				{
					m_Unit1 = value;
					RaisePropertyChanged("Unit1"); 
				}
			}
		}


		private string m_Value1;

		public string Value1
		{
			get => m_Value1;
			set
			{
				if (m_Value1 != value)
				{
					m_Value1 = value;
					RaisePropertyChanged("Value1"); 
				}
			}
		}


		private string m_CurrencySymbol2;

		public string CurrencySymbol2
		{
			get => m_CurrencySymbol2;
			set
			{
				if (m_CurrencySymbol2 != value)
				{
					m_CurrencySymbol2 = value;
					RaisePropertyChanged("CurrencySymbol2"); 
				}
			}
		}


		private Unit m_Unit2;

		public Unit Unit2
		{
			get => m_Unit2;
			set
			{
				if (m_Unit2 != value)
				{
					m_Unit2 = value;
					RaisePropertyChanged("Unit2"); 
				}
			}
		}


		private string m_Value2;

		public string Value2
		{
			get => m_Value2;
			set
			{
				if (m_Value2 != value)
				{
					m_Value2 = value;
					RaisePropertyChanged("Value2"); 
				}
			}
		}


		private CalculatorObservableCollection<SupplementaryResult> m_SupplementaryResults;

		public CalculatorObservableCollection<SupplementaryResult> SupplementaryResults
		{
			get => m_SupplementaryResults;
			set
			{
				if (m_SupplementaryResults != value)
				{
					m_SupplementaryResults = value;
					RaisePropertyChanged("SupplementaryResults"); 
				}
			}
		}

		internal static string SupplementaryResultsPropertyName => "SupplementaryResults";

		private bool m_Value1Active;

		public bool Value1Active
		{
			get => m_Value1Active;
			set
			{
				if (m_Value1Active != value)
				{
					m_Value1Active = value;
					RaisePropertyChanged("Value1Active"); 
				}
			}
		}


		private bool m_Value2Active;

		public bool Value2Active
		{
			get => m_Value2Active;
			set
			{
				if (m_Value2Active != value)
				{
					m_Value2Active = value;
					RaisePropertyChanged("Value2Active"); 
				}
			}
		}


		private string m_Value1AutomationName;

		public string Value1AutomationName
		{
			get => m_Value1AutomationName;
			set
			{
				if (m_Value1AutomationName != value)
				{
					m_Value1AutomationName = value;
					RaisePropertyChanged("Value1AutomationName"); 
				}
			}
		}


		private string m_Value2AutomationName;

		public string Value2AutomationName
		{
			get => m_Value2AutomationName;
			set
			{
				if (m_Value2AutomationName != value)
				{
					m_Value2AutomationName = value;
					RaisePropertyChanged("Value2AutomationName"); 
				}
			}
		}


		private string m_Unit1AutomationName;

		public string Unit1AutomationName
		{
			get => m_Unit1AutomationName;
			set
			{
				if (m_Unit1AutomationName != value)
				{
					m_Unit1AutomationName = value;
					RaisePropertyChanged("Unit1AutomationName"); 
				}
			}
		}


		private string m_Unit2AutomationName;

		public string Unit2AutomationName
		{
			get => m_Unit2AutomationName;
			set
			{
				if (m_Unit2AutomationName != value)
				{
					m_Unit2AutomationName = value;
					RaisePropertyChanged("Unit2AutomationName"); 
				}
			}
		}


		private CalculatorApp.Common.Automation.NarratorAnnouncement m_Announcement;

		public CalculatorApp.Common.Automation.NarratorAnnouncement Announcement
		{
			get => m_Announcement;
			set
			{
				if (m_Announcement != value)
				{
					m_Announcement = value;
					RaisePropertyChanged("Announcement"); 
				}
			}
		}


		private bool m_IsDecimalEnabled;

		public bool IsDecimalEnabled
		{
			get => m_IsDecimalEnabled;
			set
			{
				if (m_IsDecimalEnabled != value)
				{
					m_IsDecimalEnabled = value;
					RaisePropertyChanged("IsDecimalEnabled"); 
				}
			}
		}


		private bool m_IsDropDownOpen;

		public bool IsDropDownOpen
		{
			get => m_IsDropDownOpen;
			set
			{
				if (m_IsDropDownOpen != value)
				{
					m_IsDropDownOpen = value;
					RaisePropertyChanged("IsDropDownOpen"); 
				}
			}
		}


		private bool m_IsDropDownEnabled;

		public bool IsDropDownEnabled
		{
			get => m_IsDropDownEnabled;
			set
			{
				if (m_IsDropDownEnabled != value)
				{
					m_IsDropDownEnabled = value;
					RaisePropertyChanged("IsDropDownEnabled"); 
				}
			}
		}


		internal const string IsCurrencyLoadingVisiblePropertyName = "IsCurrencyLoadingVisible";
		private bool m_IsCurrencyLoadingVisible;

		public bool IsCurrencyLoadingVisible
		{
			get => m_IsCurrencyLoadingVisible;
			set
			{
				if (m_IsCurrencyLoadingVisible != value)
				{
					m_IsCurrencyLoadingVisible = value;
					RaisePropertyChanged("IsCurrencyLoadingVisible"); 
				}
			}
		}


		private bool m_IsCurrencyCurrentCategory;

		public bool IsCurrencyCurrentCategory
		{
			get => m_IsCurrencyCurrentCategory;
			set
			{
				if (m_IsCurrencyCurrentCategory != value)
				{
					m_IsCurrencyCurrentCategory = value;
					RaisePropertyChanged("IsCurrencyCurrentCategory"); 
				}
			}
		}


		private string m_CurrencyRatioEquality;

		public string CurrencyRatioEquality
		{
			get => m_CurrencyRatioEquality;
			set
			{
				if (m_CurrencyRatioEquality != value)
				{
					m_CurrencyRatioEquality = value;
					RaisePropertyChanged("CurrencyRatioEquality"); 
				}
			}
		}


		private string m_CurrencyRatioEqualityAutomationName = "TODO UNO";

		public string CurrencyRatioEqualityAutomationName
		{
			get => m_CurrencyRatioEqualityAutomationName;
			set
			{
				if (m_CurrencyRatioEqualityAutomationName != value)
				{
					m_CurrencyRatioEqualityAutomationName = value;
					RaisePropertyChanged("CurrencyRatioEqualityAutomationName"); 
				}
			}
		}


		private string m_CurrencyTimestamp;

		public string CurrencyTimestamp
		{
			get => m_CurrencyTimestamp;
			set
			{
				if (m_CurrencyTimestamp != value)
				{
					m_CurrencyTimestamp = value;
					RaisePropertyChanged("CurrencyTimestamp"); 
				}
			}
		}


		internal const string NetworkBehaviorPropertyName = "NetworkBehavior";
		private CalculatorApp.NetworkAccessBehavior m_NetworkBehavior;

		public CalculatorApp.NetworkAccessBehavior NetworkBehavior
		{
			get => m_NetworkBehavior;
			set
			{
				if (m_NetworkBehavior != value)
				{
					m_NetworkBehavior = value;
					RaisePropertyChanged("NetworkBehavior"); 
				}
			}
		}


		internal const string CurrencyDataLoadFailedPropertyName = "CurrencyDataLoadFailed";
		private bool m_CurrencyDataLoadFailed;

		public bool CurrencyDataLoadFailed
		{
			get => m_CurrencyDataLoadFailed;
			set
			{
				if (m_CurrencyDataLoadFailed != value)
				{
					m_CurrencyDataLoadFailed = value;
					RaisePropertyChanged("CurrencyDataLoadFailed"); 
				}
			}
		}


		internal const string CurrencyDataIsWeekOldPropertyName = "CurrencyDataIsWeekOld";
		private bool m_CurrencyDataIsWeekOld;

		public bool CurrencyDataIsWeekOld
		{
			get => m_CurrencyDataIsWeekOld;
			set
			{
				if (m_CurrencyDataIsWeekOld != value)
				{
					m_CurrencyDataIsWeekOld = value;
					RaisePropertyChanged("CurrencyDataIsWeekOld"); 
				}
			}
		}



		public Windows.UI.Xaml.Visibility SupplementaryVisibility
		{
			get { return SupplementaryResults.Count > 0 ? Windows.UI.Xaml.Visibility.Visible : Windows.UI.Xaml.Visibility.Collapsed; }
		}

		public Windows.UI.Xaml.Visibility CurrencySymbolVisibility
		{
			get
			{
				return (CurrencySymbol1.IsEmpty() || CurrencySymbol2.IsEmpty())
					? Windows.UI.Xaml.Visibility.Collapsed
					: Windows.UI.Xaml.Visibility.Visible;
			}
		}

		public ICommand CategoryChanged { get; }
		public ICommand UnitChanged { get; }
		public ICommand SwitchActive { get; }
		public ICommand ButtonPressed { get; }
		public ICommand CopyCommand { get; }
		public ICommand PasteCommand { get; }


		string GetValueFromUnlocalized()
		{
			return m_valueFromUnlocalized;
		}

		string GetValueToUnlocalized()
		{
			return m_valueToUnlocalized;
		}

		UnitConversionManager.IUnitConverter m_model;
		char m_decimalSeparator;

		enum ConversionParameter
		{
			Source,
			Target
		}

		ConversionParameter m_value1cp;

		private string ValueFrom
		{
			get { return m_value1cp == ConversionParameter.Source ? Value1 : Value2; }
			set
			{
				if (m_value1cp == ConversionParameter.Source) Value1 = value;
				else Value2 = value;
			}
		}

		private Unit UnitFrom
		{
			get { return m_value1cp == ConversionParameter.Source ? Unit1 : Unit2; }
			set
			{
				if (m_value1cp == ConversionParameter.Source) Unit1 = value;
				else Unit2 = value;
			}
		}

		private string ValueTo
		{
			get { return m_value1cp == ConversionParameter.Target ? Value1 : Value2; }
			set
			{
				if (m_value1cp == ConversionParameter.Target) Value1 = value;
				else Value2 = value;
			}
		}

		Unit UnitTo
		{
			get { return m_value1cp == ConversionParameter.Target ? Unit1 : Unit2; }
			set
			{
				if (m_value1cp == ConversionParameter.Target) Unit1 = value;
				else Unit2 = value;
			}
		}

		void SwitchConversionParameters()
		{
			m_value1cp = m_value1cp == ConversionParameter.Source ? ConversionParameter.Target : ConversionParameter.Source;
		}

		bool m_isInputBlocked;
		Windows.System.Threading.ThreadPoolTimer m_supplementaryResultsTimer;
		bool m_resettingTimer;

		CalculatorList<Tuple<string, UnitConversionManager.Unit>> m_cachedSuggestedValues;

		//Mutex m_cacheMutex;
		object m_cacheMutex = new object();
		Windows.Globalization.NumberFormatting.DecimalFormatter m_decimalFormatter;
		Windows.Globalization.NumberFormatting.CurrencyFormatter m_currencyFormatter;
		int m_currencyMaxFractionDigits;
		string m_valueFromUnlocalized;
		string m_valueToUnlocalized;

		bool m_relocalizeStringOnSwitch;

		// For Saving the User Preferences only if the Unit converter ViewModel is initialised for the first time
		bool m_IsFirstTime;

		string m_localizedValueFromFormat;
		string m_localizedValueFromDecimalFormat;
		string m_localizedValueToFormat;
		string m_localizedConversionResultFormat;
		string m_localizedInputUnitName;
		string m_localizedOutputUnitName;

		bool m_isValue1Updating;
		bool m_isValue2Updating;
		string m_lastAnnouncedFrom;
		string m_lastAnnouncedTo;
		string m_lastAnnouncedConversionResult;

		bool m_isCurrencyDataLoaded;

		CalculatorApp.Common.ConversionResultTaskHelper m_conversionResultTaskHelper;
	}

	internal class UnitConverterVMCallback : UnitConversionManager.IUnitConverterVMCallback
	{
		public UnitConverterVMCallback(UnitConverterViewModel viewModel)
		{
			m_viewModel = viewModel;
		}

		public void DisplayCallback(string from, string to)
		{
			m_viewModel.UpdateDisplay(from, to);
		}

		public void SuggestedValueCallback(CalculatorList<Tuple<string, UnitConversionManager.Unit>> suggestedValues)
		{
			m_viewModel.UpdateSupplementaryResults(suggestedValues);
		}

		public void MaxDigitsReached()
		{
			m_viewModel.OnMaxDigitsReached();
		}

		UnitConverterViewModel m_viewModel;
	}

	internal class ViewModelCurrencyCallback : UnitConversionManager.IViewModelCurrencyCallback
	{
		public ViewModelCurrencyCallback(UnitConverterViewModel viewModel)
		{
			m_viewModel = viewModel;
		}

		public void CurrencyDataLoadFinished(bool didLoad)
		{
			m_viewModel.OnCurrencyDataLoadFinished(didLoad);
		}

		public void CurrencySymbolsCallback(string symbol1, string symbol2)
		{
			string sym1 = symbol1;
			string sym2 = symbol2;

			bool value1Active = m_viewModel.Value1Active;
			m_viewModel.CurrencySymbol1 = value1Active ? sym1 : sym2;
			m_viewModel.CurrencySymbol2 = value1Active ? sym2 : sym1;
		}

		public void CurrencyRatiosCallback(string ratioEquality, string accRatioEquality)
		{
			m_viewModel.CurrencyRatioEquality = ratioEquality;
			m_viewModel.CurrencyRatioEqualityAutomationName = accRatioEquality;
		}

		public void CurrencyTimestampCallback(string timestamp, bool isWeekOld)
		{
			m_viewModel.OnCurrencyTimestampUpdated(timestamp, isWeekOld);
		}

		public void NetworkBehaviorChanged(int newBehavior)
		{
			m_viewModel.OnNetworkBehaviorChanged((CalculatorApp.NetworkAccessBehavior)(newBehavior));
		}

		UnitConverterViewModel m_viewModel;
	};

	public sealed partial class UnitConverterViewModel
	{
		const int EXPECTEDVIEWMODELDATATOKENS = 8;

		// interval is in 100 nanosecond units
		const uint TIMER_INTERVAL_IN_MS = 10000;

		//#if UNIT_TESTS
		//#define TIMER_CALLBACK_CONTEXT // CallbackContext.Any ==> UNO cf. _supplementaryResultsTimerContext
		//#else
		//#define TIMER_CALLBACK_CONTEXT // CallbackContext.Same ==> UNO cf. _supplementaryResultsTimerContext
		//#endif

		private static readonly TimeSpan SUPPLEMENTARY_VALUES_INTERVAL = TimeSpan.FromTicks(10 * TIMER_INTERVAL_IN_MS);

		static Unit EMPTY_UNIT = new Unit(UCM.Unit.EMPTY_UNIT);

		private static readonly Func<CategorySelectionInitializer, CalculatorApp.CalculatorList<UnitConversionManager.Unit>> UNIT_LIST = csi => csi.Item1;
		private static readonly Func<CategorySelectionInitializer, UnitConversionManager.Unit> SELECTED_SOURCE_UNIT = csi => csi.Item2;
		private static readonly Func<CategorySelectionInitializer, UnitConversionManager.Unit> SELECTED_TARGET_UNIT = csi => csi.Item3;


		// x millisecond delay before we consider conversion to be final
		uint CONVERSION_FINALIZED_DELAY_IN_MS = 1000;
		static readonly Regex regexTrimSpacesStart = new Regex("^\\s+");
		static readonly Regex regexTrimSpacesEnd = new Regex("\\s+$");

		const string CurrentCategoryPropertyName = "CurrentCategory";
		const string Unit1AutomationNamePropertyName = "Unit1AutomationName";
		const string Unit2AutomationNamePropertyName = "Unit2AutomationName";
		const string Unit1PropertyName = "Unit1";
		const string Unit2PropertyName = "Unit2";
		const string Value1PropertyName = "Value1";
		const string Value2PropertyName = "Value2";
		const string Value1ActivePropertyName = "Value1Active";
		const string Value2ActivePropertyName = "Value2Active";
		const string Value1AutomationNamePropertyName = "Value1AutomationName";
		const string Value2AutomationNamePropertyName = "Value2AutomationName";
		const string CurrencySymbol1PropertyName = "CurrencySymbol1";
		const string CurrencySymbol2PropertyName = "CurrencySymbol2";
		const string CurrencySymbolVisibilityPropertyName = "CurrencySymbolVisibility";
		const string SupplementaryVisibilityPropertyName = "SupplementaryVisibility";

		public static class UnitConverterResourceKeys
		{
			public const string ValueFromFormat = "Format_ValueFrom";
			public const string ValueFromDecimalFormat = "Format_ValueFrom_Decimal";
			public const string ValueToFormat = "Format_ValueTo";
			public const string ConversionResultFormat = "Format_ConversionResult";
			public const string InputUnit_Name = "InputUnit_Name";
			public const string OutputUnit_Name = "OutputUnit_Name";
			public const string MaxDigitsReachedFormat = "Format_MaxDigitsReached";
			public const string UpdatingCurrencyRates = "UpdatingCurrencyRates";
			public const string CurrencyRatesUpdated = "CurrencyRatesUpdated";
			public const string CurrencyRatesUpdateFailed = "CurrencyRatesUpdateFailed";
		}

		public UnitConverterViewModel(IUnitConverter model)
		{
			CategoryChanged = new DelegateCommand(OnCategoryChanged);
			UnitChanged = new DelegateCommand(OnUnitChanged);
			SwitchActive = new DelegateCommand(OnSwitchActive);
			ButtonPressed = new DelegateCommand(OnButtonPressed);
			CopyCommand = new DelegateCommand(OnCopyCommand);
			PasteCommand = new DelegateCommand(OnPasteCommand);

			m_model = model;
			m_resettingTimer = false;
			m_value1cp = ConversionParameter.Source;
			m_Value1Active = true;
			m_Value2Active = false;
			m_Value1 = "0";
			m_Value2 = "0";
			m_valueToUnlocalized = "0";
			m_valueFromUnlocalized = "0";
			m_relocalizeStringOnSwitch = false;
			m_Categories = new CalculatorObservableCollection<Category>();
			m_Units = new CalculatorObservableCollection<Unit>();
			m_SupplementaryResults = new CalculatorObservableCollection<SupplementaryResult>();
			m_IsDropDownOpen = false;
			m_IsDropDownEnabled = true;
			m_IsCurrencyLoadingVisible = false;
			m_isCurrencyDataLoaded = false;
			m_lastAnnouncedFrom = "";
			m_lastAnnouncedTo = "";
			m_lastAnnouncedConversionResult = "";
			m_isValue1Updating = false;
			m_isValue2Updating = false;
			m_Announcement = null;
			m_Mode = ViewMode.None;
			m_CurrencySymbol1 = "";
			m_CurrencySymbol2 = "";
			m_IsCurrencyCurrentCategory = false;
			m_CurrencyRatioEquality = "";
			m_CurrencyRatioEqualityAutomationName = "";
			m_isInputBlocked = false;
			m_CurrencyDataLoadFailed = false;

			m_model.SetViewModelCallback(new UnitConverterVMCallback(this));
			m_model.SetViewModelCurrencyCallback(new ViewModelCurrencyCallback(this));
			m_decimalFormatter = LocalizationService.GetRegionalSettingsAwareDecimalFormatter();
			m_decimalFormatter.FractionDigits = 0;
			m_decimalFormatter.IsGrouped = true;
			m_decimalSeparator = LocalizationSettings.GetInstance().GetDecimalSeparator();

			m_currencyFormatter = LocalizationService.GetRegionalSettingsAwareCurrencyFormatter();
			m_currencyFormatter.IsGrouped = true;
			m_currencyFormatter.Mode = CurrencyFormatterMode.UseCurrencyCode;
			m_currencyFormatter.ApplyRoundingForCurrency(RoundingAlgorithm.RoundHalfDown);
			//TODO UNO: m_currencyMaxFractionDigits = m_currencyFormatter.FractionDigits;
			m_currencyMaxFractionDigits = 2;

			var resourceLoader = AppResourceProvider.GetInstance();
			m_localizedValueFromFormat = resourceLoader.GetResourceString(UnitConverterResourceKeys.ValueFromFormat);
			m_localizedValueToFormat = resourceLoader.GetResourceString(UnitConverterResourceKeys.ValueToFormat);
			m_localizedConversionResultFormat = resourceLoader.GetResourceString(UnitConverterResourceKeys.ConversionResultFormat);
			m_localizedValueFromDecimalFormat = resourceLoader.GetResourceString(UnitConverterResourceKeys.ValueFromDecimalFormat);
			m_localizedInputUnitName = resourceLoader.GetResourceString(UnitConverterResourceKeys.InputUnit_Name);
			m_localizedOutputUnitName = resourceLoader.GetResourceString(UnitConverterResourceKeys.OutputUnit_Name);

			Unit1AutomationName = m_localizedInputUnitName;
			Unit2AutomationName = m_localizedOutputUnitName;
			IsDecimalEnabled = true;

			m_IsFirstTime = true;
			m_model.Initialize();
			PopulateData();
		}

		void ResetView()
		{
			m_model.SendCommand(UCM.Command.Reset);
			m_IsFirstTime = true;
			OnCategoryChanged(null);
		}

		void PopulateData()
		{
			InitializeView();
		}

		void OnCategoryChanged(object parameter)
		{
			m_model.SendCommand(UCM.Command.Clear);
			ResetCategory();
		}

		void ResetCategory()
		{
			UCM.Category currentCategory = CurrentCategory.GetModelCategory();
			IsCurrencyCurrentCategory = currentCategory.id == NavCategory.Serialize(ViewMode.Currency);

			m_isInputBlocked = false;
			SetSelectedUnits();

			IsCurrencyLoadingVisible = m_IsCurrencyCurrentCategory && !m_isCurrencyDataLoaded;
			IsDropDownEnabled = m_Units[0] != EMPTY_UNIT;

			UnitChanged.Execute(null);
		}

		void SetSelectedUnits()
		{
			CategorySelectionInitializer categoryInitializer = m_model.SetCurrentCategory(CurrentCategory.GetModelCategory());
			BuildUnitList(UNIT_LIST(categoryInitializer));

			UnitFrom = FindUnitInList(SELECTED_SOURCE_UNIT(categoryInitializer));
			UnitTo = FindUnitInList(SELECTED_TARGET_UNIT(categoryInitializer));
		}

		void BuildUnitList(CalculatorList<UCM.Unit> modelUnitList)
		{
			using (m_Units.BatchUpdate())
			{
				m_Units.Clear();
				foreach (UCM.Unit modelUnit in modelUnitList)
				{
					if (!modelUnit.isWhimsical)
					{
						m_Units.Append(new Unit(modelUnit));
					}
				}

				if (m_Units.Count == 0)
				{
					m_Units.Append(EMPTY_UNIT);
				}
			}
		}

		Unit FindUnitInList(UCM.Unit target)
		{
			foreach (Unit vmUnit in m_Units)
			{
				UCM.Unit modelUnit = vmUnit.GetModelUnit();
				if (modelUnit.id == target.id)
				{
					return vmUnit;
				}
			}

			return EMPTY_UNIT;
		}

		void OnUnitChanged(object parameter)
		{
			if ((m_Unit1 == null) || (m_Unit2 == null))
			{
				// Return if both Unit1 & Unit2 are not set
				return;
			}

			m_model.SetCurrentUnitTypes(UnitFrom.GetModelUnit(), UnitTo.GetModelUnit());
			if (m_supplementaryResultsTimer != null)
			{
				// End timer to show results immediately
				m_supplementaryResultsTimer.Cancel();
			}

			if (!m_IsFirstTime)
			{
				SaveUserPreferences();
			}
			else
			{
				RestoreUserPreferences();
				m_IsFirstTime = false;
			}
		}

		void OnSwitchActive(object unused)
		{
			// this can be false if this switch occurs without the user having explicitly updated any strings
			// (for example, during deserialization). We only want to try this cleanup if there's actually
			// something to clean up.
			if (m_relocalizeStringOnSwitch)
			{
				// clean up any ill-formed strings that were in progress before the switch
				ValueFrom = ConvertToLocalizedString(m_valueFromUnlocalized, false);
			}

			SwitchConversionParameters();
			// Now deactivate the other
			if (m_value1cp == ConversionParameter.Source)
			{
				Value2Active = false;
			}
			else
			{
				Value1Active = false;
			}

			Utils.Swap(ref m_valueFromUnlocalized, ref m_valueToUnlocalized);
			Utils.Swap(ref m_localizedValueFromFormat, ref m_localizedValueToFormat);

			Utils.Swap(ref m_Unit1AutomationName, ref m_Unit2AutomationName);
			RaisePropertyChanged(Unit1AutomationNamePropertyName);
			RaisePropertyChanged(Unit2AutomationNamePropertyName);

			m_isInputBlocked = false;
			m_model.SwitchActive(m_valueFromUnlocalized);
		}

		string ConvertToLocalizedString(string stringToLocalize, bool allowPartialStrings)
		{
			string result = null;

			if (stringToLocalize.empty())
			{
				return result;
			}

			m_decimalFormatter.IsDecimalPointAlwaysDisplayed = false;
			m_decimalFormatter.FractionDigits = 0;
			m_currencyFormatter.IsDecimalPointAlwaysDisplayed = false;
			m_currencyFormatter.FractionDigits = 0;

			var posOfE = stringToLocalize.find('e');
			if (posOfE != "".npos())
			{
				int posOfSign = posOfE + 1;
				char signOfE = stringToLocalize.at(posOfSign);
				string significandStr = stringToLocalize.substr(0, posOfE);
				string exponentStr = stringToLocalize.substr(posOfSign + 1, stringToLocalize.length() - posOfSign);

				result += ConvertToLocalizedString(significandStr, allowPartialStrings) + "e" + signOfE + ConvertToLocalizedString(exponentStr, allowPartialStrings);
			}
			else
			{
				// stringToLocalize is in en-US and has the default decimal separator, so this is safe to do.
				int posOfDecimal = stringToLocalize.find('.');

				bool hasDecimal = "".npos() != posOfDecimal;
				
				// TODO UNO - BEGIN
				var value = double.Parse(stringToLocalize, NumberStyles.Any, CultureInfo.InvariantCulture);
				if (IsCurrencyCurrentCategory)
				{
					if (hasDecimal)
					{
						result = value.ToString("N" + m_currencyMaxFractionDigits, CultureInfo.CurrentCulture);
					}
					else
					{
						result = value.ToString("N0", CultureInfo.CurrentCulture);
					}
				}
				else
				{
					var decimalDigitCount = stringToLocalize.length() - (posOfDecimal + 1);
					if (hasDecimal && decimalDigitCount == 0 && allowPartialStrings)
					{
						result = value.ToString("N1", CultureInfo.CurrentCulture).TrimEnd('0');
					}
					else if (hasDecimal)
					{
						result = value.ToString("N" + decimalDigitCount, CultureInfo.CurrentCulture);
					}
					else
					{
						result = value.ToString("N0", CultureInfo.CurrentCulture);
					}
				}

				/* TODO UNO
				if (hasDecimal)
				{
					if (allowPartialStrings)
					{
						// allow "in progress" strings, like "3." that occur during the composition of
						// a final number. Without this, when typing the three characters in "3.2"
						// you don't see the decimal point when typing it, you only see it once you've finally
						// typed a post-decimal digit.

						m_decimalFormatter.IsDecimalPointAlwaysDisplayed = true;
						m_currencyFormatter.IsDecimalPointAlwaysDisplayed = true;
					}

					// force post-decimal digits so that trailing zeroes entered by the user aren't suddenly cut off.
					m_decimalFormatter.FractionDigits = (int)(stringToLocalize.length() - (posOfDecimal + 1));
					m_currencyFormatter.FractionDigits = m_currencyMaxFractionDigits;
				}

				if (IsCurrencyCurrentCategory)
				{
					string currencyResult = m_currencyFormatter.Format(System.Convert.ToDouble(stringToLocalize, CultureInfo.InvariantCulture));
					string currencyCode = m_currencyFormatter.Currency;

					// CurrencyFormatter always includes LangCode or Symbol. Make it include LangCode
					// because this includes a non-breaking space. Remove the LangCode.
					var pos = currencyResult.find(currencyCode);
					if (pos != "".npos())
					{
						currencyResult = currencyResult.Remove(pos, currencyCode.length());
						currencyResult = regexTrimSpacesStart.Replace(currencyResult, "");
						currencyResult = regexTrimSpacesEnd.Replace(currencyResult, "");
					}

					result = currencyResult;
				}
				else
				{
					// Convert the input string to double using stod
					// Then use the decimalFormatter to reformat the double to Platform String
					result = m_decimalFormatter.Format(System.Convert.ToDouble(stringToLocalize));
				}

				if (hasDecimal)
				{
					// Since the output from GetLocaleInfoEx() and DecimalFormatter are differing for decimal string
					// we are adding the below work-around of editing the string returned by DecimalFormatter
					// and replacing the decimal separator with the one returned by GetLocaleInfoEx()
					String formattedSampleString = m_decimalFormatter.Format(System.Convert.ToDouble("1.1"));
					string formattedSampleWString = formattedSampleString;

					string resultWithDecimal = result;
					resultWithDecimal = resultWithDecimal.Replace(formattedSampleWString[1], m_decimalSeparator);

					// Copy back the edited string to the result
					result = resultWithDecimal;
				} TODO UNO - END */
			}

			string resultHolder = result;
			if ((stringToLocalize.front() == '-' && System.Convert.ToDouble(stringToLocalize, CultureInfo.InvariantCulture) == 0) || resultHolder.back() == '-')
			{
				if (resultHolder.back() == '-')
				{
					result = resultHolder.Remove(resultHolder.size() - 1, 1);
				}

				result = "-" + result;
			}

			result = Utils.LRE + result + Utils.PDF;
			return result;
		}

		void DisplayPasteError()
		{
			String errorMsg = AppResourceProvider.GetInstance().GetCEngineString(EngineStrings.SIDS_DOMAIN); /*SIDS_DOMAIN is for "invalid input"*/
			Value1 = errorMsg;
			Value2 = errorMsg;
			m_relocalizeStringOnSwitch = false;
		}

		internal void UpdateDisplay(string from, string to)
		{
			String fromStr = this.ConvertToLocalizedString(from, true);
			UpdateInputBlocked(from);
			String toStr = this.ConvertToLocalizedString(to, true);

			bool updatedValueFrom = ValueFrom != fromStr;
			bool updatedValueTo = ValueTo != toStr;
			if (updatedValueFrom)
			{
				m_valueFromUnlocalized = from;
				// once we've updated the unlocalized from string, we'll potentially need to clean it back up when switching between fields
				// to eliminate dangling decimal points.
				m_relocalizeStringOnSwitch = true;
			}

			if (updatedValueTo)
			{
				// This is supposed to use trimming logic, but that's highly dependent
				// on the auto-scaling textbox control which we dont have yet. For now,
				// not doing anything. It will have to be integrated once that control is
				// created.
				m_valueToUnlocalized = to;
			}

			m_isValue1Updating = m_Value1Active ? updatedValueFrom : updatedValueTo;
			m_isValue2Updating = m_Value2Active ? updatedValueFrom : updatedValueTo;

			// Setting these properties before setting the member variables above causes
			// a chain of properties that can result in the wrong result being announced
			// to Narrator. We need to know which values are updating before setting the
			// below properties, so that we know when to announce the result.
			if (updatedValueFrom)
			{
				ValueFrom = fromStr;
			}

			if (updatedValueTo)
			{
				ValueTo = toStr;
			}
		}

		internal void UpdateSupplementaryResults(CalculatorList<Tuple<string, UnitConversionManager.Unit>> suggestedValues)
		{
			lock (m_cacheMutex)
			{
				m_cachedSuggestedValues = suggestedValues;
			}

			// If we're already "ticking", reset the timer
			if (m_supplementaryResultsTimer != null)
			{
				m_resettingTimer = true;
				m_supplementaryResultsTimer.Cancel();
				m_resettingTimer = false;
			}

			// Schedule the timer
			m_supplementaryResultsTimer = ThreadPoolTimer.CreateTimer(
				new TimerElapsedHandler(SupplementaryResultsTimerTick),
				SUPPLEMENTARY_VALUES_INTERVAL,
				new TimerDestroyedHandler(SupplementaryResultsTimerCancel)); // UNO: TIMER_CALLBACK_CONTEXT: cf. _supplementaryResultsTimerContext
		}

		public void OnValueActivated(IActivatable control)
		{
			control.IsActive = true;
		}

		void OnButtonPressed(object parameter)
		{
			NumbersAndOperatorsEnum numOpEnum = CalculatorButtonPressedEventArgs.GetOperationFromCommandParameter(parameter);
			UCM.Command command = CommandFromButtonId(numOpEnum);

			// Don't clear the display if combo box is open and escape is pressed
			if (command == UCM.Command.Clear && IsDropDownOpen)
			{
				return;
			}

			CalculatorList<UCM.Command> OPERANDS = new CalculatorList<UCM.Command>
			{
				UCM.Command.Zero, UCM.Command.One, UCM.Command.Two, UCM.Command.Three, UCM.Command.Four,
				UCM.Command.Five, UCM.Command.Six, UCM.Command.Seven, UCM.Command.Eight, UCM.Command.Nine
			};

			if (OPERANDS.IndexOf(command) != -1)
			{
				if (m_isInputBlocked)
				{
					return;
				}

				if (m_IsCurrencyCurrentCategory)
				{
					StartConversionResultTimer();
				}
			}

			m_model.SendCommand(command);
		}

		public void OnCopyCommand(object parameter)
		{
			// EventWriteClipboardCopy_Start();
			CopyPasteManager.CopyToClipboard(m_valueFromUnlocalized);
			// EventWriteClipboardCopy_Stop();
		}

		public async void OnPasteCommand(object parameter)
		{
			// if there's nothing to copy early out
			if (!CopyPasteManager.HasStringToPaste())
			{
				return;
			}

			// Ensure that the paste happens on the UI thread
			// EventWriteClipboardPaste_Start();
			// Any converter ViewMode is fine here.
			var pastedString = await CopyPasteManager.GetStringToPaste(m_Mode, NavCategory.GetGroupType(m_Mode), default(int), default(int));
			OnPaste(pastedString, m_Mode);
		}

		void InitializeView()
		{
			using (m_Categories.BatchUpdate())
			{
				CalculatorList<UCM.Category> categories = m_model.GetCategories();
				for (uint i = 0; i < categories.Size(); i++)
				{
					Category category = new Category(categories[i]);
					m_Categories.Append(category);
				}
			}

			RestoreUserPreferences();
			CurrentCategory = new Category(m_model.GetCurrentCategory());
		}

		static bool isCategoryChanging = false;

		void OnPropertyChanged(string prop)
		{
			if (prop == CurrentCategoryPropertyName)
			{
				isCategoryChanging = true;
				CategoryChanged.Execute(null);
				isCategoryChanging = false;
			}
			else if (prop == Unit1PropertyName || prop == Unit2PropertyName)
			{
				// Category changes will handle updating units after they've both been updated.
				// This event should only be used to update units from explicit user interaction.
				if (!isCategoryChanging)
				{
					UnitChanged.Execute(null);
				}

				// Get the localized automation name for each CalculationResults field
				if (prop == Unit1PropertyName)
				{
					UpdateValue1AutomationName();
				}
				else
				{
					UpdateValue2AutomationName();
				}
			}
			else if (prop == Value1PropertyName)
			{
				UpdateValue1AutomationName();
			}
			else if (prop == Value2PropertyName)
			{
				UpdateValue2AutomationName();
			}
			else if (prop == Value1ActivePropertyName || prop == Value2ActivePropertyName)
			{
				// if one of the values is activated, and as a result both are true, it means
				// that we're trying to switch.
				if (Value1Active && Value2Active)
				{
					SwitchActive.Execute(null);
				}

				UpdateValue1AutomationName();
				UpdateValue2AutomationName();
			}
			else if (prop == SupplementaryResultsPropertyName)
			{
				RaisePropertyChanged(SupplementaryVisibilityPropertyName);
			}
			else if (prop == Value1AutomationNamePropertyName)
			{
				m_isValue1Updating = false;
				if (!m_isValue2Updating)
				{
					AnnounceConversionResult();
				}
			}
			else if (prop == Value2AutomationNamePropertyName)
			{
				m_isValue2Updating = false;
				if (!m_isValue1Updating)
				{
					AnnounceConversionResult();
				}
			}
			else if (prop == CurrencySymbol1PropertyName || prop == CurrencySymbol2PropertyName)
			{
				RaisePropertyChanged(CurrencySymbolVisibilityPropertyName);
			}
		}

		String Serialize()
		{
			using (var @out = new StringWriter())
			{
				const string delimiter = "[;;;]";
				@out.Write(m_resettingTimer);
				@out.Write(delimiter);
				@out.Write((int)(m_value1cp));
				@out.Write(delimiter);
				@out.Write(m_Value1Active);
				@out.Write(delimiter);
				@out.Write(m_Value2Active);
				@out.Write(delimiter);
				@out.Write(m_Value1);
				@out.Write(delimiter);
				@out.Write(m_Value2);
				@out.Write(delimiter);
				@out.Write(m_valueFromUnlocalized);
				@out.Write(delimiter);
				@out.Write(m_valueToUnlocalized);
				@out.Write(delimiter);
				@out.Write("[###]");
				string unitConverterSerializedData = m_model.Serialize();

				if (!unitConverterSerializedData.empty())
				{
					@out.Write(m_model.Serialize());
					@out.Write("[###]");
					String serializedData = @out.ToString();
					return serializedData;
				}

				return null;
			}
		}

		void Deserialize(string state)
		{
			string serializedData = state;
			CalculatorList<string> tokens = UCM.UnitConverter.StringToVector(serializedData, "[###]");
			Debug.Assert(tokens.Size() >= 2);
			CalculatorList<string> viewModelData = UCM.UnitConverter.StringToVector(tokens[0], "[;;;]");
			Debug.Assert(viewModelData.Size() == EXPECTEDVIEWMODELDATATOKENS);
			m_resettingTimer = (viewModelData[0].CompareTo("1") == 0);
			m_value1cp = (ConversionParameter)System.Convert.ToInt32(viewModelData[1]);
			m_Value1Active = (viewModelData[2].CompareTo("1") == 0);
			m_Value2Active = (viewModelData[3].CompareTo("1") == 0);
			m_Value1 = viewModelData[4];
			m_Value2 = viewModelData[5];
			m_valueFromUnlocalized = viewModelData[6];
			m_valueToUnlocalized = viewModelData[7];
			string modelData = string.Empty;
			for (uint i = 1; i < tokens.Size(); i++)
			{
				modelData += tokens[i] + "[###]";
			}

			m_model.DeSerialize(modelData);
			InitializeView();
			RaisePropertyChanged(null); // Update since all props have been updated.
		}

		// Saving User Preferences of Category and Associated-Units across Sessions.
		void SaveUserPreferences()
		{
			if (UnitsAreValid())
			{
				ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;
				if (!m_IsCurrencyCurrentCategory)
				{
					var userPreferences = m_model.SaveUserPreferences();
					localSettings.Values["UnitConverterPreferences"] = userPreferences;
				}
				else
				{
					// Currency preferences shouldn't be saved in the same way as standard converter modes because
					// the delay loading creates a big mess of issues that are better to avoid.
					localSettings.Values[CalculatorApp.ViewModel.UnitConverterResourceKeys.CurrencyUnitFromKey] = UnitFrom.Abbreviation;
					localSettings.Values[CalculatorApp.ViewModel.UnitConverterResourceKeys.CurrencyUnitToKey] = UnitTo.Abbreviation;
				}
			}
		}

		// Restoring User Preferences of Category and Associated-Units.
		void RestoreUserPreferences()
		{
			if (!IsCurrencyCurrentCategory)
			{
				ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;
				if (localSettings.Values.ContainsKey("UnitConverterPreferences"))
				{
					var userPreferences = (string)localSettings.Values["UnitConverterPreferences"];
					m_model.RestoreUserPreferences(userPreferences);
				}
			}
		}

		internal void OnCurrencyDataLoadFinished(bool didLoad)
		{
			m_isCurrencyDataLoaded = true;
			CurrencyDataLoadFailed = !didLoad;
			m_model.ResetCategoriesAndRatios();
			m_model.Calculate();
			ResetCategory();

			string key = didLoad ? UnitConverterResourceKeys.CurrencyRatesUpdated : UnitConverterResourceKeys.CurrencyRatesUpdateFailed;
			String announcement = AppResourceProvider.GetInstance().GetResourceString(key);
			Announcement = CalculatorAnnouncement.GetUpdateCurrencyRatesAnnouncement(announcement);
		}

		internal void OnCurrencyTimestampUpdated(string timestamp, bool isWeekOld)
		{
			CurrencyDataIsWeekOld = isWeekOld;
			CurrencyTimestamp = timestamp;
		}

		public async void RefreshCurrencyRatios()
		{
			m_isCurrencyDataLoaded = false;
			IsCurrencyLoadingVisible = true;

			String announcement = AppResourceProvider.GetInstance().GetResourceString(UnitConverterResourceKeys.UpdatingCurrencyRates);
			Announcement = CalculatorAnnouncement.GetUpdateCurrencyRatesAnnouncement(announcement);

			var refreshTask = m_model.RefreshCurrencyRatios();
			var refreshResult = await refreshTask;
			bool didLoad = refreshResult.Key;
			string timestamp = refreshResult.Value;

			OnCurrencyTimestampUpdated(timestamp, false /*isWeekOldData*/);
			OnCurrencyDataLoadFinished(didLoad);
		}

		internal void OnNetworkBehaviorChanged(NetworkAccessBehavior newBehavior)
		{
			CurrencyDataLoadFailed = false;
			NetworkBehavior = newBehavior;
		}

		UnitConversionManager.Command CommandFromButtonId(NumbersAndOperatorsEnum button)
		{
			UCM.Command command;

			switch (button)
			{
				case NumbersAndOperatorsEnum.Zero:
					command = UCM.Command.Zero;
					break;
				case NumbersAndOperatorsEnum.One:
					command = UCM.Command.One;
					break;
				case NumbersAndOperatorsEnum.Two:
					command = UCM.Command.Two;
					break;
				case NumbersAndOperatorsEnum.Three:
					command = UCM.Command.Three;
					break;
				case NumbersAndOperatorsEnum.Four:
					command = UCM.Command.Four;
					break;
				case NumbersAndOperatorsEnum.Five:
					command = UCM.Command.Five;
					break;
				case NumbersAndOperatorsEnum.Six:
					command = UCM.Command.Six;
					break;
				case NumbersAndOperatorsEnum.Seven:
					command = UCM.Command.Seven;
					break;
				case NumbersAndOperatorsEnum.Eight:
					command = UCM.Command.Eight;
					break;
				case NumbersAndOperatorsEnum.Nine:
					command = UCM.Command.Nine;
					break;
				case NumbersAndOperatorsEnum.Decimal:
					command = UCM.Command.Decimal;
					break;
				case NumbersAndOperatorsEnum.Negate:
					command = UCM.Command.Negate;
					break;
				case NumbersAndOperatorsEnum.Backspace:
					command = UCM.Command.Backspace;
					break;
				case NumbersAndOperatorsEnum.Clear:
					command = UCM.Command.Clear;
					break;
				default:
					command = UCM.Command.None;
					break;
			}

			return command;
		}

		void SupplementaryResultsTimerTick(ThreadPoolTimer timer)
		{
			timer.Cancel();
		}

		// UNO: We cannot specify the resturning context of ThreadPoolTime in C#.
		//		Instead we capture the dispatcher on which this VM has been created, and send back the UI update on it
		private readonly CoreDispatcher _supplementaryResultsTimerContext = Window.Current.Dispatcher;

		void SupplementaryResultsTimerCancel(ThreadPoolTimer timer)
		{
			if (!m_resettingTimer)
			{
				_supplementaryResultsTimerContext.RunAsync(CoreDispatcherPriority.Normal, RefreshSupplementaryResults);
			}
		}

		void RefreshSupplementaryResults()
		{
			lock (m_cacheMutex)
			using(m_SupplementaryResults.BatchUpdate())
			{
				m_SupplementaryResults.Clear();

				CalculatorList<SupplementaryResult> whimsicals = new CalculatorList<SupplementaryResult>();

				foreach (Tuple<string, UCM.Unit> suggestedValue in m_cachedSuggestedValues)
				{
					SupplementaryResult result = new SupplementaryResult(this.ConvertToLocalizedString(suggestedValue.Item1, false), new Unit(suggestedValue.Item2));
					if (result.IsWhimsical())
					{
						whimsicals.PushBack(result);
					}
					else
					{
						m_SupplementaryResults.Append(result);
					}
				}

				if (whimsicals.Size() > 0)
				{
					m_SupplementaryResults.Append(whimsicals[0]);
				}
			}

			RaisePropertyChanged(SupplementaryResultsPropertyName);
			// EventWriteConverterSupplementaryResultsUpdated();
		}

		// When UpdateDisplay is called, the ViewModel will remember the From/To unlocalized display values
		// This function will announce the conversion result after the ValueTo/ValueFrom automation names update,
		// only if the new unlocalized display values are different from the last announced values, and if the
		// values are not both zero.
		void AnnounceConversionResult()
		{
			if ((m_valueFromUnlocalized != m_lastAnnouncedFrom || m_valueToUnlocalized != m_lastAnnouncedTo) && Unit1 != null && Unit2 != null)
			{
				m_lastAnnouncedFrom = m_valueFromUnlocalized;
				m_lastAnnouncedTo = m_valueToUnlocalized;

				Unit unitFrom = Value1Active ? Unit1 : Unit2;
				Unit unitTo = (unitFrom == Unit1) ? Unit2 : Unit1;
				m_lastAnnouncedConversionResult = GetLocalizedConversionResultStringFormat(ValueFrom, unitFrom.Name, ValueTo, unitTo.Name);

				Announcement = CalculatorAnnouncement.GetDisplayUpdatedAnnouncement(m_lastAnnouncedConversionResult);
			}
		}

		void UpdateInputBlocked(string currencyInput)
		{
			// currencyInput is in en-US and has the default decimal separator, so this is safe to do.
			var posOfDecimal = currencyInput.find('.');
			m_isInputBlocked = false;
			if (posOfDecimal != "".npos() && IsCurrencyCurrentCategory)
			{
				m_isInputBlocked = (posOfDecimal + (int)(m_currencyMaxFractionDigits) + 1 == currencyInput.length());
			}
		}

		NumbersAndOperatorsEnum MapCharacterToButtonId(char ch, bool canSendNegate)
		{
			Debug.Assert(NumbersAndOperatorsEnum.Zero < NumbersAndOperatorsEnum.One, "NumbersAndOperatorsEnum order is invalid");
			Debug.Assert(NumbersAndOperatorsEnum.One < NumbersAndOperatorsEnum.Two, "NumbersAndOperatorsEnum order is invalid");
			Debug.Assert(NumbersAndOperatorsEnum.Two < NumbersAndOperatorsEnum.Three, "NumbersAndOperatorsEnum order is invalid");
			Debug.Assert(NumbersAndOperatorsEnum.Three < NumbersAndOperatorsEnum.Four, "NumbersAndOperatorsEnum order is invalid");
			Debug.Assert(NumbersAndOperatorsEnum.Four < NumbersAndOperatorsEnum.Five, "NumbersAndOperatorsEnum order is invalid");
			Debug.Assert(NumbersAndOperatorsEnum.Five < NumbersAndOperatorsEnum.Six, "NumbersAndOperatorsEnum order is invalid");
			Debug.Assert(NumbersAndOperatorsEnum.Six < NumbersAndOperatorsEnum.Seven, "NumbersAndOperatorsEnum order is invalid");
			Debug.Assert(NumbersAndOperatorsEnum.Seven < NumbersAndOperatorsEnum.Eight, "NumbersAndOperatorsEnum order is invalid");
			Debug.Assert(NumbersAndOperatorsEnum.Eight < NumbersAndOperatorsEnum.Nine, "NumbersAndOperatorsEnum order is invalid");
			Debug.Assert(NumbersAndOperatorsEnum.Zero < NumbersAndOperatorsEnum.Nine, "NumbersAndOperatorsEnum order is invalid");

			NumbersAndOperatorsEnum mappedValue = NumbersAndOperatorsEnum.None;
			canSendNegate = false;

			switch (ch)
			{
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					mappedValue = (int)NumbersAndOperatorsEnum.Zero + (NumbersAndOperatorsEnum)(ch - '0');
					canSendNegate = true;
					break;

				case '-':
					mappedValue = NumbersAndOperatorsEnum.Negate;
					break;

				default:
					// Respect the user setting for decimal separator
					if (ch == m_decimalSeparator)
					{
						mappedValue = NumbersAndOperatorsEnum.Decimal;
						canSendNegate = true;
						break;
					}

					break;
			}

			if (mappedValue == NumbersAndOperatorsEnum.None)
			{
				if (LocalizationSettings.GetInstance().IsLocalizedDigit(ch))
				{
					mappedValue = (int)NumbersAndOperatorsEnum.Zero
						+ (NumbersAndOperatorsEnum)(ch - LocalizationSettings.GetInstance().GetDigitSymbolFromEnUsDigit('0'));
					canSendNegate = true;
				}
			}

			return mappedValue;
		}

		private static readonly IDictionary<VirtualKey, UCM.Command> _keyToCommandMap = new Dictionary<VirtualKey, UCM.Command>
		{
			{VirtualKey.Number0, UCM.Command.Zero},
			{VirtualKey.Number1, UCM.Command.One},
			{VirtualKey.Number2, UCM.Command.Two},
			{VirtualKey.Number3, UCM.Command.Three},
			{VirtualKey.Number4, UCM.Command.Four},
			{VirtualKey.Number5, UCM.Command.Five},
			{VirtualKey.Number6, UCM.Command.Six},
			{VirtualKey.Number7, UCM.Command.Seven},
			{VirtualKey.Number8, UCM.Command.Eight},
			{VirtualKey.Number9, UCM.Command.Nine},
			{VirtualKey.Decimal, UCM.Command.Decimal},
			{VirtualKey.Subtract, UCM.Command.Negate},
			{VirtualKey.Back, UCM.Command.Backspace},
			{VirtualKey.Escape, UCM.Command.Clear},
			{VirtualKey.Delete, UCM.Command.Reset},
		};

		// TODO UNO: KeyboardShortcutManager
		public void OnKeyPress(VirtualKey key)
		{
			if (_keyToCommandMap.TryGetValue(key, out var cmd))
			{
				m_model.SendCommand(cmd);
			}
		}

		public void OnPaste(String stringToPaste, ViewMode mode)
		{
			// If pastedString is invalid("NoOp") then display pasteError else process the string
			if (stringToPaste == CopyPasteManager.PasteErrorString)
			{
				this.DisplayPasteError();
				return;
			}

			TraceLogger.GetInstance().LogValidInputPasted(mode);
			bool isFirstLegalChar = true;
			bool sendNegate = false;
			string accumulation = "";

			foreach (char it in stringToPaste)
			{
				bool canSendNegate = false;

				NumbersAndOperatorsEnum op = MapCharacterToButtonId(it, canSendNegate);

				if (NumbersAndOperatorsEnum.None != op)
				{
					if (isFirstLegalChar)
					{
						// Send Clear before sending something that will actually apply
						// to the field.
						m_model.SendCommand(UCM.Command.Clear);
						isFirstLegalChar = false;

						// If the very first legal character is a - sign, send negate
						// after sending the next legal character.  Send nothing now, or
						// it will be ignored.
						if (NumbersAndOperatorsEnum.Negate == op)
						{
							sendNegate = true;
						}
					}

					// Negate is only allowed if it's the first legal character, which is handled above.
					if (NumbersAndOperatorsEnum.Negate != op)
					{
						UCM.Command cmd = CommandFromButtonId(op);
						m_model.SendCommand(cmd);

						if (sendNegate)
						{
							if (canSendNegate)
							{
								m_model.SendCommand(UCM.Command.Negate);
							}

							sendNegate = false;
						}
					}

					accumulation += it;
					UpdateInputBlocked(accumulation);
					if (m_isInputBlocked)
					{
						break;
					}
				}
				else
				{
					sendNegate = false;
				}
			}
		}

		String GetLocalizedAutomationName(String displayvalue, String unitname, String format)
		{
			String valueToLocalize = displayvalue;
			if (displayvalue == ValueFrom && Utils.IsLastCharacterTarget(m_valueFromUnlocalized, m_decimalSeparator))
			{
				// Need to compute a second localized value for the automation
				// name that does not include the decimal separator.
				displayvalue = ConvertToLocalizedString(m_valueFromUnlocalized, false /*allowTrailingDecimal*/);
				format = m_localizedValueFromDecimalFormat;
			}

			string localizedResult = LocalizationStringUtil.GetLocalizedString(format, displayvalue, unitname);
			return localizedResult;
		}

		String GetLocalizedConversionResultStringFormat(
			String fromValue,
			String fromUnit,
			String toValue,
			String toUnit)
		{
			String localizedString = LocalizationStringUtil.GetLocalizedString(m_localizedConversionResultFormat, fromValue, fromUnit, toValue, toUnit);
			return localizedString;
		}

		void UpdateValue1AutomationName()
		{
			if (Unit1 != null)
			{
				Value1AutomationName = GetLocalizedAutomationName(Value1, Unit1.AccessibleName, m_localizedValueFromFormat);
			}
		}

		void UpdateValue2AutomationName()
		{
			if (Unit2 != null)
			{
				Value2AutomationName = GetLocalizedAutomationName(Value2, Unit2.AccessibleName, m_localizedValueToFormat);
			}
		}

		internal void OnMaxDigitsReached()
		{
			String format = AppResourceProvider.GetInstance().GetResourceString(UnitConverterResourceKeys.MaxDigitsReachedFormat);
			string announcement = LocalizationStringUtil.GetLocalizedString(format, m_lastAnnouncedConversionResult);
			Announcement = CalculatorAnnouncement.GetMaxDigitsReachedAnnouncement(announcement);
		}

		bool UnitsAreValid()
		{
			return UnitFrom != null && !UnitFrom.Abbreviation.IsEmpty() && UnitTo != null && !UnitTo.Abbreviation.IsEmpty();
		}

		void StartConversionResultTimer()
		{
			m_conversionResultTaskHelper = new ConversionResultTaskHelper(CONVERSION_FINALIZED_DELAY_IN_MS,
				() =>
				{
					if (UnitsAreValid())
					{
						String valueFrom = m_Value1Active ? m_Value1 : m_Value2;
						String valueTo = m_Value1Active ? m_Value2 : m_Value1;
						TraceLogger.GetInstance().LogConversionResult(valueFrom, UnitFrom.ToString(), valueTo, UnitTo.ToString());
					}
				});
		}

		String GetLocalizedAutomationName()
		{
			// TODO UNO
			//var format = AppResourceProvider.GetInstance().GetResourceString("SupplementaryUnit_AutomationName");
			//return new String(LocalizationStringUtil.GetLocalizedString(format, this.Value, this.Unit.Name));

			return string.Empty;
		}
	}
}
