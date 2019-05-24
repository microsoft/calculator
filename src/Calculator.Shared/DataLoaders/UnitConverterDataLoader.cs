// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.Linq;
using System.Text;
using Windows.Globalization;
using CalculatorApp;
using CalculatorApp.Common;
using CalculatorApp.ViewModel;
using CategorySelectionInitializer = System.Tuple<CalculatorApp.CalculatorList<UnitConversionManager.Unit>, UnitConversionManager.Unit, UnitConversionManager.Unit>;
using UnitToUnitToConversionDataMap = System.Collections.Generic.Dictionary<UnitConversionManager.Unit, System.Collections.Generic.Dictionary<UnitConversionManager.Unit, UnitConversionManager.ConversionData>>;
using CategoryToUnitVectorMap = System.Collections.Generic.Dictionary<UnitConversionManager.Category, CalculatorApp.CalculatorList<UnitConversionManager.Unit>>;
using UCM = UnitConversionManager;

namespace CalculatorApp.ViewModel
{
	public class OrderedUnit : UnitConversionManager.Unit
	{
		public OrderedUnit()
		{
		}

		public OrderedUnit(
			UnitConverterUnits id,
			string name,
			string abbreviation,
			int order,
			bool isConversionSource = false,
			bool isConversionTarget = false,
			bool isWhimsical = false)
			: this((int)id, name, abbreviation, order, isConversionSource, isConversionTarget, isWhimsical)
		{
		}

		public OrderedUnit(
			int id,
			string name,
			string abbreviation,
			int order,
			bool isConversionSource = false,
			bool isConversionTarget = false,
			bool isWhimsical = false)
			: base(id, name, abbreviation, isConversionSource, isConversionTarget, isWhimsical)
		{
			this.order = order;
		}

		public int order;
	};

	public struct UnitData
	{
		public UnitData(ViewMode categoryId, UnitConverterUnits unit, double factor)
		{
			this.categoryId = categoryId;
			this.unitId = (int)unit;
			this.factor = factor;
		}

		public CalculatorApp.Common.ViewMode categoryId;
		public int unitId;
		public double factor;
	};

	class ExplicitUnitConversionData : UnitConversionManager.ConversionData
	{
		//public ExplicitUnitConversionData()
		//{
		//}
		public ExplicitUnitConversionData(
			CalculatorApp.Common.ViewMode categoryId,
			UnitConverterUnits parentUnitId,
			UnitConverterUnits unitId,
			double ratio,
			double offset,
			bool offsetFirst = false)
			: this(categoryId, (int)parentUnitId, (int)unitId, ratio, offset, offsetFirst)
		{

		}


		public ExplicitUnitConversionData(
			CalculatorApp.Common.ViewMode categoryId,
			int parentUnitId,
			int unitId,
			double ratio,
			double offset,
			bool offsetFirst = false)
			: base(ratio, offset, offsetFirst)
		{
			this.categoryId = categoryId;
			this.parentUnitId = parentUnitId;
			this.unitId = unitId;
		}

		public CalculatorApp.Common.ViewMode categoryId;
		public int parentUnitId;
		public int unitId;
	};

	class UnitConverterDataLoader : UnitConversionManager.IConverterDataLoader
	{
		CalculatorList<UnitConversionManager.Category> m_categoryList;
		CategoryToUnitVectorMap m_categoryToUnits;
		UnitToUnitToConversionDataMap m_ratioMap;
		string m_currentRegionCode;


		static bool CONVERT_WITH_OFFSET_FIRST = true;

		public UnitConverterDataLoader(GeographicRegion region)
		{
			// TODO UNO
			//m_currentRegionCode = region.CodeTwoLetter;
			m_currentRegionCode = new RegionInfo(CultureInfo.CurrentCulture.LCID).TwoLetterISORegionName;

			m_categoryList = new CalculatorList<UCM.Category>();
			m_categoryToUnits = new CategoryToUnitVectorMap(EqualityComparer<UCM.Category>.Default);
			m_ratioMap = new UnitToUnitToConversionDataMap(EqualityComparer<UCM.Unit>.Default);
		}

		public CalculatorList<UCM.Category> LoadOrderedCategories()
		{
			return new CalculatorList<UCM.Category>(m_categoryList);
		}

		public CalculatorList<UCM.Unit> LoadOrderedUnits(UCM.Category category)
		{
			return m_categoryToUnits[category];
		}

		public Dictionary<UCM.Unit, UCM.ConversionData> LoadOrderedRatios(UCM.Unit unit)
		{
			return m_ratioMap[unit];
		}

		public bool SupportsCategory(UCM.Category target)
		{
			CalculatorList<UCM.Category> supportedCategories = null;
			if (!m_categoryList.IsEmpty())
			{
				supportedCategories = m_categoryList;
			}
			else
			{
				GetCategories(supportedCategories);
			}

			int currencyId = NavCategory.Serialize(ViewMode.Currency);
			var itr = !supportedCategories.Any(category => currencyId != category.id && target.id == category.id);
			return itr;
		}

		public void LoadData()
		{
			Dictionary<int, OrderedUnit> idToUnit = new Dictionary<int, OrderedUnit>();

			Dictionary<ViewMode, CalculatorList<OrderedUnit>> orderedUnitMap = new Dictionary<ViewMode, CalculatorList<OrderedUnit>>();
			Dictionary<ViewMode, Dictionary<int, double>> categoryToUnitConversionDataMap = new Dictionary<ViewMode, Dictionary<int, double>>();
			Dictionary<int, Dictionary<int, UCM.ConversionData>> explicitConversionData = new Dictionary<int, Dictionary<int, UCM.ConversionData>>();

			// Load categories, units and conversion data into data structures. This will be then used to populate hashmaps used by CalcEngine and UI layer
			GetCategories(m_categoryList);
			GetUnits(orderedUnitMap);
			GetConversionData(categoryToUnitConversionDataMap);
			GetExplicitConversionData(explicitConversionData); // This is needed for temperature conversions

			m_categoryToUnits.Clear();
			m_ratioMap.Clear();
			foreach (UCM.Category objectCategory in m_categoryList)
			{
				ViewMode categoryViewMode = NavCategory.Deserialize(objectCategory.id);
				Debug.Assert(NavCategory.IsConverterViewMode(categoryViewMode));
				if (categoryViewMode == ViewMode.Currency)
				{
					// Currency is an ordered category but we do not want to process it here
					// because this function is not thread-safe and currency data is asynchronously
					// loaded.
					m_categoryToUnits.Add(objectCategory, new CalculatorList<UCM.Unit>());
					continue;
				}

				CalculatorList<OrderedUnit> orderedUnits = orderedUnitMap[categoryViewMode];
				CalculatorList<UCM.Unit> unitList = new CalculatorList<UCM.Unit>();

				// Sort the units by order
				orderedUnits.Sort((OrderedUnit first, OrderedUnit second) => { return first.order < second.order; });

				foreach (OrderedUnit u in orderedUnits)
				{
					unitList.PushBack((UCM.Unit)(u));
					idToUnit.Add(u.id, u);
				}

				// Save units per category
				m_categoryToUnits.Add(objectCategory, unitList);

				// For each unit, populate the conversion data
				foreach (UCM.Unit unit in unitList)
				{
					Dictionary<UCM.Unit, UCM.ConversionData> conversions = new Dictionary<UCM.Unit, UCM.ConversionData>(EqualityComparer<UCM.Unit>.Default);

					//if (explicitConversionData.find(unit.id) == explicitConversionData.end())
					if (!explicitConversionData.TryGetValue(unit.id, out var unitConversions))
					{
						// Get the associated units for a category id
						Dictionary<int, double> unitConversions2 = categoryToUnitConversionDataMap[categoryViewMode];
						double unitFactor = unitConversions2[unit.id];

						foreach (var kvp in unitConversions2)
						{
							var id = kvp.Key;
							var conversionFactor = kvp.Value;
							if (!idToUnit.ContainsKey(id))
							{
								// Optional units will not be in idToUnit but can be in unitConversions.
								// For optional units that did not make it to the current set of units, just continue.
								continue;
							}

							UCM.ConversionData parsedData = new UCM.ConversionData(1.0, 0.0, false);
							Debug.Assert(conversionFactor > 0); // divide by zero assert
							parsedData.ratio = unitFactor / conversionFactor;
							conversions.Add(idToUnit[id], parsedData);
						}
					}
					else
					{
						//Dictionary<int, UCM.ConversionData> unitConversions = explicitConversionData.at(unit.id);
						foreach (var itr in unitConversions)
						{
							conversions.Add(idToUnit[itr.Key], itr.Value);
						}
					}

					m_ratioMap.Add(unit, conversions);
				}
			}
		}

		void GetCategories(CalculatorList<UCM.Category> categoriesList)
		{
			categoriesList.Clear();
			var converterCategory = NavCategoryGroup.CreateConverterCategory();
			foreach (var category in converterCategory.Categories)
			{
				/* Id, CategoryName, SupportsNegative */
				categoriesList.EmplaceBack(new UCM.Category(NavCategory.Serialize(category.Mode), category.Name, category.SupportsNegative));
			}
		}

		void GetUnits(Dictionary<ViewMode, CalculatorList<OrderedUnit>> unitMap)
		{
			// US + Federated States of Micronesia, Marshall Islands, Palau
			bool useUSCustomaryAndFahrenheit =
				m_currentRegionCode == "US" || m_currentRegionCode == "FM" || m_currentRegionCode == "MH" || m_currentRegionCode == "PW";

			// useUSCustomaryAndFahrenheit + Liberia
			// Source: https://en.wikipedia.org/wiki/Metrication
			bool useUSCustomary = useUSCustomaryAndFahrenheit || m_currentRegionCode == "LR";

			// Use 'Système International' (International System of Units - Metrics)
			bool useSI = !useUSCustomary;

			// useUSCustomaryAndFahrenheit + the Bahamas, the Cayman Islands and Liberia
			// Source: http://en.wikipedia.org/wiki/Fahrenheit
			bool useFahrenheit = useUSCustomaryAndFahrenheit || m_currentRegionCode == "BS" || m_currentRegionCode == "KY" || m_currentRegionCode == "LR";

			bool useWattInsteadOfKilowatt = m_currentRegionCode == "GB";

			// Use Pyeong, a Korean floorspace unit.
			// https://en.wikipedia.org/wiki/Korean_units_of_measurement#Area
			bool usePyeong = m_currentRegionCode == "KP" || m_currentRegionCode == "KR";

			CalculatorList<OrderedUnit> areaUnits = new CalculatorList<OrderedUnit>();
			areaUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Area_Acre, GetLocalizedStringName("UnitName_Acre"), GetLocalizedStringName("UnitAbbreviation_Acre"), 9));
			areaUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Area_Hectare, GetLocalizedStringName("UnitName_Hectare"), GetLocalizedStringName("UnitAbbreviation_Hectare"), 4));
			areaUnits.PushBack(new OrderedUnit(UnitConverterUnits.Area_SquareCentimeter,
				GetLocalizedStringName("UnitName_SquareCentimeter"),
				GetLocalizedStringName("UnitAbbreviation_SquareCentimeter"),
				2));
			areaUnits.PushBack(new OrderedUnit(UnitConverterUnits.Area_SquareFoot,
				GetLocalizedStringName("UnitName_SquareFoot"),
				GetLocalizedStringName("UnitAbbreviation_SquareFoot"),
				7,
				useSI,
				useUSCustomary,
				false));
			areaUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Area_SquareInch,
				GetLocalizedStringName("UnitName_SquareInch"),
				GetLocalizedStringName("UnitAbbreviation_SquareInch"),
				6));
			areaUnits.PushBack(new OrderedUnit(UnitConverterUnits.Area_SquareKilometer,
				GetLocalizedStringName("UnitName_SquareKilometer"),
				GetLocalizedStringName("UnitAbbreviation_SquareKilometer"),
				5));
			areaUnits.PushBack(new OrderedUnit(UnitConverterUnits.Area_SquareMeter,
				GetLocalizedStringName("UnitName_SquareMeter"),
				GetLocalizedStringName("UnitAbbreviation_SquareMeter"),
				3,
				useUSCustomary,
				useSI,
				false));
			areaUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Area_SquareMile,
				GetLocalizedStringName("UnitName_SquareMile"),
				GetLocalizedStringName("UnitAbbreviation_SquareMile"),
				10));
			areaUnits.PushBack(new OrderedUnit(UnitConverterUnits.Area_SquareMillimeter,
				GetLocalizedStringName("UnitName_SquareMillimeter"),
				GetLocalizedStringName("UnitAbbreviation_SquareMillimeter"),
				1));
			areaUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Area_SquareYard,
				GetLocalizedStringName("UnitName_SquareYard"),
				GetLocalizedStringName("UnitAbbreviation_SquareYard"),
				8));
			areaUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Area_Hand,
				GetLocalizedStringName("UnitName_Hand"),
				GetLocalizedStringName("UnitAbbreviation_Hand"),
				11,
				false,
				false,
				true));
			areaUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Area_Paper,
				GetLocalizedStringName("UnitName_Paper"),
				GetLocalizedStringName("UnitAbbreviation_Paper"),
				12,
				false,
				false,
				true));
			areaUnits.PushBack(new OrderedUnit(UnitConverterUnits.Area_SoccerField,
				GetLocalizedStringName("UnitName_SoccerField"),
				GetLocalizedStringName("UnitAbbreviation_SoccerField"),
				13,
				false,
				false,
				true));
			areaUnits.PushBack(new OrderedUnit(UnitConverterUnits.Area_Castle,
				GetLocalizedStringName("UnitName_Castle"),
				GetLocalizedStringName("UnitAbbreviation_Castle"),
				14,
				false,
				false,
				true));
			if (usePyeong)
			{
				areaUnits.PushBack(new OrderedUnit(UnitConverterUnits.Area_Pyeong,
					GetLocalizedStringName("UnitName_Pyeong"),
					GetLocalizedStringName("UnitAbbreviation_Pyeong"),
					15,
					false,
					false,
					false));
			}

			unitMap.Add(ViewMode.Area, areaUnits);

			CalculatorList<OrderedUnit> dataUnits = new CalculatorList<OrderedUnit>();
			dataUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Data_Bit, GetLocalizedStringName("UnitName_Bit"), GetLocalizedStringName("UnitAbbreviation_Bit"), 1));
			dataUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Data_Byte, GetLocalizedStringName("UnitName_Byte"), GetLocalizedStringName("UnitAbbreviation_Byte"), 2));
			dataUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Data_Exabits, GetLocalizedStringName("UnitName_Exabits"), GetLocalizedStringName("UnitAbbreviation_Exabits"), 23));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Exabytes,
				GetLocalizedStringName("UnitName_Exabytes"),
				GetLocalizedStringName("UnitAbbreviation_Exabytes"),
				25));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Exbibits,
				GetLocalizedStringName("UnitName_Exbibits"),
				GetLocalizedStringName("UnitAbbreviation_Exbibits"),
				24));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Exbibytes,
				GetLocalizedStringName("UnitName_Exbibytes"),
				GetLocalizedStringName("UnitAbbreviation_Exbibytes"),
				26));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Gibibits,
				GetLocalizedStringName("UnitName_Gibibits"),
				GetLocalizedStringName("UnitAbbreviation_Gibibits"),
				12));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Gibibytes,
				GetLocalizedStringName("UnitName_Gibibytes"),
				GetLocalizedStringName("UnitAbbreviation_Gibibytes"),
				14));
			dataUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Data_Gigabit, GetLocalizedStringName("UnitName_Gigabit"), GetLocalizedStringName("UnitAbbreviation_Gigabit"), 11));
			dataUnits.PushBack(new OrderedUnit(UnitConverterUnits.Data_Gigabyte,
				GetLocalizedStringName("UnitName_Gigabyte"),
				GetLocalizedStringName("UnitAbbreviation_Gigabyte"),
				13,
				true,
				false,
				false));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Kibibits,
				GetLocalizedStringName("UnitName_Kibibits"),
				GetLocalizedStringName("UnitAbbreviation_Kibibits"),
				4));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Kibibytes,
				GetLocalizedStringName("UnitName_Kibibytes"),
				GetLocalizedStringName("UnitAbbreviation_Kibibytes"),
				6));
			dataUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Data_Kilobit, GetLocalizedStringName("UnitName_Kilobit"), GetLocalizedStringName("UnitAbbreviation_Kilobit"), 3));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Kilobyte,
				GetLocalizedStringName("UnitName_Kilobyte"),
				GetLocalizedStringName("UnitAbbreviation_Kilobyte"),
				5));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Mebibits,
				GetLocalizedStringName("UnitName_Mebibits"),
				GetLocalizedStringName("UnitAbbreviation_Mebibits"),
				8));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Mebibytes,
				GetLocalizedStringName("UnitName_Mebibytes"),
				GetLocalizedStringName("UnitAbbreviation_Mebibytes"),
				10));
			dataUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Data_Megabit, GetLocalizedStringName("UnitName_Megabit"), GetLocalizedStringName("UnitAbbreviation_Megabit"), 7));
			dataUnits.PushBack(new OrderedUnit(UnitConverterUnits.Data_Megabyte,
				GetLocalizedStringName("UnitName_Megabyte"),
				GetLocalizedStringName("UnitAbbreviation_Megabyte"),
				9,
				false,
				true,
				false));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Pebibits,
				GetLocalizedStringName("UnitName_Pebibits"),
				GetLocalizedStringName("UnitAbbreviation_Pebibits"),
				20));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Pebibytes,
				GetLocalizedStringName("UnitName_Pebibytes"),
				GetLocalizedStringName("UnitAbbreviation_Pebibytes"),
				22));
			dataUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Data_Petabit, GetLocalizedStringName("UnitName_Petabit"), GetLocalizedStringName("UnitAbbreviation_Petabit"), 19));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Petabyte,
				GetLocalizedStringName("UnitName_Petabyte"),
				GetLocalizedStringName("UnitAbbreviation_Petabyte"),
				21));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Tebibits,
				GetLocalizedStringName("UnitName_Tebibits"),
				GetLocalizedStringName("UnitAbbreviation_Tebibits"),
				16));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Tebibytes,
				GetLocalizedStringName("UnitName_Tebibytes"),
				GetLocalizedStringName("UnitAbbreviation_Tebibytes"),
				18));
			dataUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Data_Terabit, GetLocalizedStringName("UnitName_Terabit"), GetLocalizedStringName("UnitAbbreviation_Terabit"), 15));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Terabyte,
				GetLocalizedStringName("UnitName_Terabyte"),
				GetLocalizedStringName("UnitAbbreviation_Terabyte"),
				17));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Yobibits,
				GetLocalizedStringName("UnitName_Yobibits"),
				GetLocalizedStringName("UnitAbbreviation_Yobibits"),
				32));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Yobibytes,
				GetLocalizedStringName("UnitName_Yobibytes"),
				GetLocalizedStringName("UnitAbbreviation_Yobibytes"),
				34));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Yottabit,
				GetLocalizedStringName("UnitName_Yottabit"),
				GetLocalizedStringName("UnitAbbreviation_Yottabit"),
				31));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Yottabyte,
				GetLocalizedStringName("UnitName_Yottabyte"),
				GetLocalizedStringName("UnitAbbreviation_Yottabyte"),
				33));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Zebibits,
				GetLocalizedStringName("UnitName_Zebibits"),
				GetLocalizedStringName("UnitAbbreviation_Zebibits"),
				28));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Zebibytes,
				GetLocalizedStringName("UnitName_Zebibytes"),
				GetLocalizedStringName("UnitAbbreviation_Zebibytes"),
				30));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Zetabits,
				GetLocalizedStringName("UnitName_Zetabits"),
				GetLocalizedStringName("UnitAbbreviation_Zetabits"),
				27));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_Zetabytes,
				GetLocalizedStringName("UnitName_Zetabytes"),
				GetLocalizedStringName("UnitAbbreviation_Zetabytes"),
				29));
			dataUnits.PushBack(new OrderedUnit(UnitConverterUnits.Data_FloppyDisk,
				GetLocalizedStringName("UnitName_FloppyDisk"),
				GetLocalizedStringName("UnitAbbreviation_FloppyDisk"),
				13,
				false,
				false,
				true));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_CD,
				GetLocalizedStringName("UnitName_CD"),
				GetLocalizedStringName("UnitAbbreviation_CD"),
				14,
				false,
				false,
				true));
			dataUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Data_DVD,
				GetLocalizedStringName("UnitName_DVD"),
				GetLocalizedStringName("UnitAbbreviation_DVD"),
				15,
				false,
				false,
				true));
			unitMap.Add(ViewMode.Data, dataUnits);

			CalculatorList<OrderedUnit> energyUnits = new CalculatorList<OrderedUnit>();
			energyUnits.PushBack(new OrderedUnit(UnitConverterUnits.Energy_BritishThermalUnit,
				GetLocalizedStringName("UnitName_BritishThermalUnit"),
				GetLocalizedStringName("UnitAbbreviation_BritishThermalUnit"),
				7));
			energyUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Energy_Calorie, GetLocalizedStringName("UnitName_Calorie"), GetLocalizedStringName("UnitAbbreviation_Calorie"), 4));
			energyUnits.PushBack(new OrderedUnit(UnitConverterUnits.Energy_ElectronVolt,
				GetLocalizedStringName("UnitName_Electron-Volt"),
				GetLocalizedStringName("UnitAbbreviation_Electron-Volt"),
				1));
			energyUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Energy_FootPound,
				GetLocalizedStringName("UnitName_Foot-Pound"),
				GetLocalizedStringName("UnitAbbreviation_Foot-Pound"),
				6));
			energyUnits.PushBack(new OrderedUnit(UnitConverterUnits.Energy_Joule,
				GetLocalizedStringName("UnitName_Joule"),
				GetLocalizedStringName("UnitAbbreviation_Joule"),
				2,
				true,
				false,
				false));
			energyUnits.PushBack(new OrderedUnit(UnitConverterUnits.Energy_Kilocalorie,
				GetLocalizedStringName("UnitName_Kilocalorie"),
				GetLocalizedStringName("UnitAbbreviation_Kilocalorie"),
				5,
				false,
				true,
				false));
			energyUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Energy_Kilojoule,
				GetLocalizedStringName("UnitName_Kilojoule"),
				GetLocalizedStringName("UnitAbbreviation_Kilojoule"),
				3));
			energyUnits.PushBack(new OrderedUnit(UnitConverterUnits.Energy_Battery,
				GetLocalizedStringName("UnitName_Battery"),
				GetLocalizedStringName("UnitAbbreviation_Battery"),
				8,
				false,
				false,
				true));
			energyUnits.PushBack(new OrderedUnit(UnitConverterUnits.Energy_Banana,
				GetLocalizedStringName("UnitName_Banana"),
				GetLocalizedStringName("UnitAbbreviation_Banana"),
				9,
				false,
				false,
				true));
			energyUnits.PushBack(new OrderedUnit(UnitConverterUnits.Energy_SliceOfCake,
				GetLocalizedStringName("UnitName_SliceOfCake"),
				GetLocalizedStringName("UnitAbbreviation_SliceOfCake"),
				10,
				false,
				false,
				true));
			unitMap.Add(ViewMode.Energy, energyUnits);

			CalculatorList<OrderedUnit> lengthUnits = new CalculatorList<OrderedUnit>();
			lengthUnits.PushBack(new OrderedUnit(UnitConverterUnits.Length_Centimeter,
				GetLocalizedStringName("UnitName_Centimeter"),
				GetLocalizedStringName("UnitAbbreviation_Centimeter"),
				4,
				useUSCustomary,
				useSI,
				false));
			lengthUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Length_Foot, GetLocalizedStringName("UnitName_Foot"), GetLocalizedStringName("UnitAbbreviation_Foot"), 8));
			lengthUnits.PushBack(new OrderedUnit(UnitConverterUnits.Length_Inch,
				GetLocalizedStringName("UnitName_Inch"),
				GetLocalizedStringName("UnitAbbreviation_Inch"),
				7,
				useSI,
				useUSCustomary,
				false));
			lengthUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Length_Kilometer,
				GetLocalizedStringName("UnitName_Kilometer"),
				GetLocalizedStringName("UnitAbbreviation_Kilometer"),
				6));
			lengthUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Length_Meter, GetLocalizedStringName("UnitName_Meter"), GetLocalizedStringName("UnitAbbreviation_Meter"), 5));
			lengthUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Length_Micron, GetLocalizedStringName("UnitName_Micron"), GetLocalizedStringName("UnitAbbreviation_Micron"), 2));
			lengthUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Length_Mile, GetLocalizedStringName("UnitName_Mile"), GetLocalizedStringName("UnitAbbreviation_Mile"), 10));
			lengthUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Length_Millimeter,
				GetLocalizedStringName("UnitName_Millimeter"),
				GetLocalizedStringName("UnitAbbreviation_Millimeter"),
				3));
			lengthUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Length_Nanometer,
				GetLocalizedStringName("UnitName_Nanometer"),
				GetLocalizedStringName("UnitAbbreviation_Nanometer"),
				1));
			lengthUnits.PushBack(new OrderedUnit(UnitConverterUnits.Length_NauticalMile,
				GetLocalizedStringName("UnitName_NauticalMile"),
				GetLocalizedStringName("UnitAbbreviation_NauticalMile"),
				11));
			lengthUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Length_Yard, GetLocalizedStringName("UnitName_Yard"), GetLocalizedStringName("UnitAbbreviation_Yard"), 9));
			lengthUnits.PushBack(new OrderedUnit(UnitConverterUnits.Length_Paperclip,
				GetLocalizedStringName("UnitName_Paperclip"),
				GetLocalizedStringName("UnitAbbreviation_Paperclip"),
				12,
				false,
				false,
				true));
			lengthUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Length_Hand,
				GetLocalizedStringName("UnitName_Hand"),
				GetLocalizedStringName("UnitAbbreviation_Hand"),
				13,
				false,
				false,
				true));
			lengthUnits.PushBack(new OrderedUnit(UnitConverterUnits.Length_JumboJet,
				GetLocalizedStringName("UnitName_JumboJet"),
				GetLocalizedStringName("UnitAbbreviation_JumboJet"),
				14,
				false,
				false,
				true));
			unitMap.Add(ViewMode.Length, lengthUnits);

			CalculatorList<OrderedUnit> powerUnits = new CalculatorList<OrderedUnit>();
			powerUnits.PushBack(new OrderedUnit(UnitConverterUnits.Power_BritishThermalUnitPerMinute,
				GetLocalizedStringName("UnitName_BTUPerMinute"),
				GetLocalizedStringName("UnitAbbreviation_BTUPerMinute"),
				5));
			powerUnits.PushBack(new OrderedUnit(UnitConverterUnits.Power_FootPoundPerMinute,
				GetLocalizedStringName("UnitName_Foot-PoundPerMinute"),
				GetLocalizedStringName("UnitAbbreviation_Foot-PoundPerMinute"),
				4));
			powerUnits.PushBack(new OrderedUnit(UnitConverterUnits.Power_Horsepower,
				GetLocalizedStringName("UnitName_Horsepower"),
				GetLocalizedStringName("UnitAbbreviation_Horsepower"),
				3,
				false,
				true,
				false));
			powerUnits.PushBack(new OrderedUnit(UnitConverterUnits.Power_Kilowatt,
				GetLocalizedStringName("UnitName_Kilowatt"),
				GetLocalizedStringName("UnitAbbreviation_Kilowatt"),
				2,
				!useWattInsteadOfKilowatt));
			powerUnits.PushBack(new OrderedUnit(UnitConverterUnits.Power_Watt,
				GetLocalizedStringName("UnitName_Watt"),
				GetLocalizedStringName("UnitAbbreviation_Watt"),
				1,
				useWattInsteadOfKilowatt));
			powerUnits.PushBack(new OrderedUnit(UnitConverterUnits.Power_LightBulb,
				GetLocalizedStringName("UnitName_LightBulb"),
				GetLocalizedStringName("UnitAbbreviation_LightBulb"),
				6,
				false,
				false,
				true));
			powerUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Power_Horse,
				GetLocalizedStringName("UnitName_Horse"),
				GetLocalizedStringName("UnitAbbreviation_Horse"),
				7,
				false,
				false,
				true));
			powerUnits.PushBack(new OrderedUnit(UnitConverterUnits.Power_TrainEngine,
				GetLocalizedStringName("UnitName_TrainEngine"),
				GetLocalizedStringName("UnitAbbreviation_TrainEngine"),
				8,
				false,
				false,
				true));
			unitMap.Add(ViewMode.Power, powerUnits);

			CalculatorList<OrderedUnit> tempUnits = new CalculatorList<OrderedUnit>();
			tempUnits.PushBack(new OrderedUnit(UnitConverterUnits.Temperature_DegreesCelsius,
				GetLocalizedStringName("UnitName_DegreesCelsius"),
				GetLocalizedStringName("UnitAbbreviation_DegreesCelsius"),
				1,
				useFahrenheit,
				!useFahrenheit,
				false));
			tempUnits.PushBack(new OrderedUnit(UnitConverterUnits.Temperature_DegreesFahrenheit,
				GetLocalizedStringName("UnitName_DegreesFahrenheit"),
				GetLocalizedStringName("UnitAbbreviation_DegreesFahrenheit"),
				2,
				!useFahrenheit,
				useFahrenheit,
				false));
			tempUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Temperature_Kelvin,
				GetLocalizedStringName("UnitName_Kelvin"),
				GetLocalizedStringName("UnitAbbreviation_Kelvin"),
				3));
			unitMap.Add(ViewMode.Temperature, tempUnits);

			CalculatorList<OrderedUnit> timeUnits = new CalculatorList<OrderedUnit>();
			timeUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Time_Day, GetLocalizedStringName("UnitName_Day"), GetLocalizedStringName("UnitAbbreviation_Day"), 6));
			timeUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Time_Hour,
				GetLocalizedStringName("UnitName_Hour"),
				GetLocalizedStringName("UnitAbbreviation_Hour"),
				5,
				true,
				false,
				false));
			timeUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Time_Microsecond,
				GetLocalizedStringName("UnitName_Microsecond"),
				GetLocalizedStringName("UnitAbbreviation_Microsecond"),
				1));
			timeUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Time_Millisecond,
				GetLocalizedStringName("UnitName_Millisecond"),
				GetLocalizedStringName("UnitAbbreviation_Millisecond"),
				2));
			timeUnits.PushBack(new OrderedUnit(UnitConverterUnits.Time_Minute,
				GetLocalizedStringName("UnitName_Minute"),
				GetLocalizedStringName("UnitAbbreviation_Minute"),
				4,
				false,
				true,
				false));
			timeUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Time_Second, GetLocalizedStringName("UnitName_Second"), GetLocalizedStringName("UnitAbbreviation_Second"), 3));
			timeUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Time_Week, GetLocalizedStringName("UnitName_Week"), GetLocalizedStringName("UnitAbbreviation_Week"), 7));
			timeUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Time_Year, GetLocalizedStringName("UnitName_Year"), GetLocalizedStringName("UnitAbbreviation_Year"), 8));
			unitMap.Add(ViewMode.Time, timeUnits);

			CalculatorList<OrderedUnit> speedUnits = new CalculatorList<OrderedUnit>();
			speedUnits.PushBack(new OrderedUnit(UnitConverterUnits.Speed_CentimetersPerSecond,
				GetLocalizedStringName("UnitName_CentimetersPerSecond"),
				GetLocalizedStringName("UnitAbbreviation_CentimetersPerSecond"),
				1));
			speedUnits.PushBack(new OrderedUnit(UnitConverterUnits.Speed_FeetPerSecond,
				GetLocalizedStringName("UnitName_FeetPerSecond"),
				GetLocalizedStringName("UnitAbbreviation_FeetPerSecond"),
				4));
			speedUnits.PushBack(new OrderedUnit(UnitConverterUnits.Speed_KilometersPerHour,
				GetLocalizedStringName("UnitName_KilometersPerHour"),
				GetLocalizedStringName("UnitAbbreviation_KilometersPerHour"),
				3,
				useUSCustomary,
				useSI,
				false));
			speedUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Speed_Knot, GetLocalizedStringName("UnitName_Knot"), GetLocalizedStringName("UnitAbbreviation_Knot"), 6));
			speedUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Speed_Mach, GetLocalizedStringName("UnitName_Mach"), GetLocalizedStringName("UnitAbbreviation_Mach"), 7));
			speedUnits.PushBack(new OrderedUnit(UnitConverterUnits.Speed_MetersPerSecond,
				GetLocalizedStringName("UnitName_MetersPerSecond"),
				GetLocalizedStringName("UnitAbbreviation_MetersPerSecond"),
				2));
			speedUnits.PushBack(new OrderedUnit(UnitConverterUnits.Speed_MilesPerHour,
				GetLocalizedStringName("UnitName_MilesPerHour"),
				GetLocalizedStringName("UnitAbbreviation_MilesPerHour"),
				5,
				useSI,
				useUSCustomary,
				false));
			speedUnits.PushBack(new OrderedUnit(UnitConverterUnits.Speed_Turtle,
				GetLocalizedStringName("UnitName_Turtle"),
				GetLocalizedStringName("UnitAbbreviation_Turtle"),
				8,
				false,
				false,
				true));
			speedUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Speed_Horse,
				GetLocalizedStringName("UnitName_Horse"),
				GetLocalizedStringName("UnitAbbreviation_Horse"),
				9,
				false,
				false,
				true));
			speedUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Speed_Jet,
				GetLocalizedStringName("UnitName_Jet"),
				GetLocalizedStringName("UnitAbbreviation_Jet"),
				10,
				false,
				false,
				true));
			unitMap.Add(ViewMode.Speed, speedUnits);

			CalculatorList<OrderedUnit> volumeUnits = new CalculatorList<OrderedUnit>();
			volumeUnits.PushBack(new OrderedUnit(UnitConverterUnits.Volume_CubicCentimeter,
				GetLocalizedStringName("UnitName_CubicCentimeter"),
				GetLocalizedStringName("UnitAbbreviation_CubicCentimeter"),
				2));
			volumeUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Volume_CubicFoot,
				GetLocalizedStringName("UnitName_CubicFoot"),
				GetLocalizedStringName("UnitAbbreviation_CubicFoot"),
				13));
			volumeUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Volume_CubicInch,
				GetLocalizedStringName("UnitName_CubicInch"),
				GetLocalizedStringName("UnitAbbreviation_CubicInch"),
				12));
			volumeUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Volume_CubicMeter,
				GetLocalizedStringName("UnitName_CubicMeter"),
				GetLocalizedStringName("UnitAbbreviation_CubicMeter"),
				4));
			volumeUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Volume_CubicYard,
				GetLocalizedStringName("UnitName_CubicYard"),
				GetLocalizedStringName("UnitAbbreviation_CubicYard"),
				14));
			volumeUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Volume_CupUS, GetLocalizedStringName("UnitName_CupUS"), GetLocalizedStringName("UnitAbbreviation_CupUS"), 8));
			volumeUnits.PushBack(new OrderedUnit(UnitConverterUnits.Volume_FluidOunceUK,
				GetLocalizedStringName("UnitName_FluidOunceUK"),
				GetLocalizedStringName("UnitAbbreviation_FluidOunceUK"),
				17));
			volumeUnits.PushBack(new OrderedUnit(UnitConverterUnits.Volume_FluidOunceUS,
				GetLocalizedStringName("UnitName_FluidOunceUS"),
				GetLocalizedStringName("UnitAbbreviation_FluidOunceUS"),
				7));
			volumeUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Volume_GallonUK,
				GetLocalizedStringName("UnitName_GallonUK"),
				GetLocalizedStringName("UnitAbbreviation_GallonUK"),
				20));
			volumeUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Volume_GallonUS,
				GetLocalizedStringName("UnitName_GallonUS"),
				GetLocalizedStringName("UnitAbbreviation_GallonUS"),
				11));
			volumeUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Volume_Liter, GetLocalizedStringName("UnitName_Liter"), GetLocalizedStringName("UnitAbbreviation_Liter"), 3));
			volumeUnits.PushBack(new OrderedUnit(UnitConverterUnits.Volume_Milliliter,
				GetLocalizedStringName("UnitName_Milliliter"),
				GetLocalizedStringName("UnitAbbreviation_Milliliter"),
				1,
				useUSCustomary,
				useSI));
			volumeUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Volume_PintUK, GetLocalizedStringName("UnitName_PintUK"), GetLocalizedStringName("UnitAbbreviation_PintUK"), 18));
			volumeUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Volume_PintUS, GetLocalizedStringName("UnitName_PintUS"), GetLocalizedStringName("UnitAbbreviation_PintUS"), 9));
			volumeUnits.PushBack(new OrderedUnit(UnitConverterUnits.Volume_TablespoonUS,
				GetLocalizedStringName("UnitName_TablespoonUS"),
				GetLocalizedStringName("UnitAbbreviation_TablespoonUS"),
				6));
			volumeUnits.PushBack(new OrderedUnit(UnitConverterUnits.Volume_TeaspoonUS,
				GetLocalizedStringName("UnitName_TeaspoonUS"),
				GetLocalizedStringName("UnitAbbreviation_TeaspoonUS"),
				5,
				useSI,
				useUSCustomary && m_currentRegionCode != "GB"));
			volumeUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Volume_QuartUK,
				GetLocalizedStringName("UnitName_QuartUK"),
				GetLocalizedStringName("UnitAbbreviation_QuartUK"),
				19));
			volumeUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Volume_QuartUS,
				GetLocalizedStringName("UnitName_QuartUS"),
				GetLocalizedStringName("UnitAbbreviation_QuartUS"),
				10));
			volumeUnits.PushBack(new OrderedUnit(UnitConverterUnits.Volume_TeaspoonUK,
				GetLocalizedStringName("UnitName_TeaspoonUK"),
				GetLocalizedStringName("UnitAbbreviation_TeaspoonUK"),
				15,
				false,
				useUSCustomary && m_currentRegionCode == "GB"));
			volumeUnits.PushBack(new OrderedUnit(UnitConverterUnits.Volume_TablespoonUK,
				GetLocalizedStringName("UnitName_TablespoonUK"),
				GetLocalizedStringName("UnitAbbreviation_TablespoonUK"),
				16));
			volumeUnits.PushBack(new OrderedUnit(UnitConverterUnits.Volume_CoffeeCup,
				GetLocalizedStringName("UnitName_CoffeeCup"),
				GetLocalizedStringName("UnitAbbreviation_CoffeeCup"),
				22,
				false,
				false,
				true));
			volumeUnits.PushBack(new OrderedUnit(UnitConverterUnits.Volume_Bathtub,
				GetLocalizedStringName("UnitName_Bathtub"),
				GetLocalizedStringName("UnitAbbreviation_Bathtub"),
				23,
				false,
				false,
				true));
			volumeUnits.PushBack(new OrderedUnit(UnitConverterUnits.Volume_SwimmingPool,
				GetLocalizedStringName("UnitName_SwimmingPool"),
				GetLocalizedStringName("UnitAbbreviation_SwimmingPool"),
				24,
				false,
				false,
				true));
			unitMap.Add(ViewMode.Volume, volumeUnits);

			CalculatorList<OrderedUnit> weightUnits = new CalculatorList<OrderedUnit>();
			weightUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Weight_Carat, GetLocalizedStringName("UnitName_Carat"), GetLocalizedStringName("UnitAbbreviation_Carat"), 1));
			weightUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Weight_Centigram,
				GetLocalizedStringName("UnitName_Centigram"),
				GetLocalizedStringName("UnitAbbreviation_Centigram"),
				3));
			weightUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Weight_Decigram,
				GetLocalizedStringName("UnitName_Decigram"),
				GetLocalizedStringName("UnitAbbreviation_Decigram"),
				4));
			weightUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Weight_Decagram,
				GetLocalizedStringName("UnitName_Decagram"),
				GetLocalizedStringName("UnitAbbreviation_Decagram"),
				6));
			weightUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Weight_Gram, GetLocalizedStringName("UnitName_Gram"), GetLocalizedStringName("UnitAbbreviation_Gram"), 5));
			weightUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Weight_Hectogram,
				GetLocalizedStringName("UnitName_Hectogram"),
				GetLocalizedStringName("UnitAbbreviation_Hectogram"),
				7));
			weightUnits.PushBack(new OrderedUnit(UnitConverterUnits.Weight_Kilogram,
				GetLocalizedStringName("UnitName_Kilogram"),
				GetLocalizedStringName("UnitAbbreviation_Kilogram"),
				8,
				useUSCustomary,
				useSI));
			weightUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Weight_LongTon,
				GetLocalizedStringName("UnitName_LongTon"),
				GetLocalizedStringName("UnitAbbreviation_LongTon"),
				14));
			weightUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Weight_Milligram,
				GetLocalizedStringName("UnitName_Milligram"),
				GetLocalizedStringName("UnitAbbreviation_Milligram"),
				2));
			weightUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Weight_Ounce, GetLocalizedStringName("UnitName_Ounce"), GetLocalizedStringName("UnitAbbreviation_Ounce"), 10));
			weightUnits.PushBack(new OrderedUnit(UnitConverterUnits.Weight_Pound,
				GetLocalizedStringName("UnitName_Pound"),
				GetLocalizedStringName("UnitAbbreviation_Pound"),
				11,
				useSI,
				useUSCustomary));
			weightUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Weight_ShortTon,
				GetLocalizedStringName("UnitName_ShortTon"),
				GetLocalizedStringName("UnitAbbreviation_ShortTon"),
				13));
			weightUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Weight_Stone, GetLocalizedStringName("UnitName_Stone"), GetLocalizedStringName("UnitAbbreviation_Stone"), 12));
			weightUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Weight_Tonne, GetLocalizedStringName("UnitName_Tonne"), GetLocalizedStringName("UnitAbbreviation_Tonne"), 9));
			weightUnits.PushBack(new OrderedUnit(UnitConverterUnits.Weight_Snowflake,
				GetLocalizedStringName("UnitName_Snowflake"),
				GetLocalizedStringName("UnitAbbreviation_Snowflake"),
				15,
				false,
				false,
				true));
			weightUnits.PushBack(new OrderedUnit(UnitConverterUnits.Weight_SoccerBall,
				GetLocalizedStringName("UnitName_SoccerBall"),
				GetLocalizedStringName("UnitAbbreviation_SoccerBall"),
				16,
				false,
				false,
				true));
			weightUnits.PushBack(new OrderedUnit(UnitConverterUnits.Weight_Elephant,
				GetLocalizedStringName("UnitName_Elephant"),
				GetLocalizedStringName("UnitAbbreviation_Elephant"),
				17,
				false,
				false,
				true));
			weightUnits.PushBack(new OrderedUnit(UnitConverterUnits.Weight_Whale,
				GetLocalizedStringName("UnitName_Whale"),
				GetLocalizedStringName("UnitAbbreviation_Whale"),
				18,
				false,
				false,
				true));
			unitMap.Add(ViewMode.Weight, weightUnits);

			CalculatorList<OrderedUnit> pressureUnits = new CalculatorList<OrderedUnit>();
			pressureUnits.PushBack(new OrderedUnit(UnitConverterUnits.Pressure_Atmosphere,
				GetLocalizedStringName("UnitName_Atmosphere"),
				GetLocalizedStringName("UnitAbbreviation_Atmosphere"),
				1,
				true,
				false,
				false));
			pressureUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Pressure_Bar,
				GetLocalizedStringName("UnitName_Bar"),
				GetLocalizedStringName("UnitAbbreviation_Bar"),
				2,
				false,
				true,
				false));
			pressureUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Pressure_KiloPascal,
				GetLocalizedStringName("UnitName_KiloPascal"),
				GetLocalizedStringName("UnitAbbreviation_KiloPascal"),
				3));
			pressureUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Pressure_MillimeterOfMercury,
				GetLocalizedStringName("UnitName_MillimeterOfMercury"),
				GetLocalizedStringName("UnitAbbreviation_MillimeterOfMercury"),
				4));
			pressureUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Pressure_Pascal, GetLocalizedStringName("UnitName_Pascal"), GetLocalizedStringName("UnitAbbreviation_Pascal"), 5));
			pressureUnits.PushBack(new OrderedUnit(
				UnitConverterUnits.Pressure_PSI,
				GetLocalizedStringName("UnitName_PSI"),
				GetLocalizedStringName("UnitAbbreviation_PSI"),
				6,
				false,
				false,
				false));
			unitMap.Add(ViewMode.Pressure, pressureUnits);

			CalculatorList<OrderedUnit> angleUnits = new CalculatorList<OrderedUnit>();
			angleUnits.PushBack(new OrderedUnit(UnitConverterUnits.Angle_Degree,
				GetLocalizedStringName("UnitName_Degree"),
				GetLocalizedStringName("UnitAbbreviation_Degree"),
				1,
				true,
				false,
				false));
			angleUnits.PushBack(new OrderedUnit(UnitConverterUnits.Angle_Radian,
				GetLocalizedStringName("UnitName_Radian"),
				GetLocalizedStringName("UnitAbbreviation_Radian"),
				2,
				false,
				true,
				false));
			angleUnits.PushBack(
				new OrderedUnit(UnitConverterUnits.Angle_Gradian, GetLocalizedStringName("UnitName_Gradian"), GetLocalizedStringName("UnitAbbreviation_Gradian"), 3));
			unitMap.Add(ViewMode.Angle, angleUnits);
		}

		void GetConversionData(Dictionary<ViewMode, Dictionary<int, double>> categoryToUnitConversionMap)
		{
			/*categoryId, UnitId, factor*/
			CalculatorList<UnitData> unitDataList = new CalculatorList<UnitData>
			{
				new UnitData(ViewMode.Area, UnitConverterUnits.Area_Acre, 4046.8564224),
				new UnitData(ViewMode.Area, UnitConverterUnits.Area_SquareMeter, 1),
				new UnitData(ViewMode.Area, UnitConverterUnits.Area_SquareFoot, 0.09290304),
				new UnitData(ViewMode.Area, UnitConverterUnits.Area_SquareYard, 0.83612736),
				new UnitData(ViewMode.Area, UnitConverterUnits.Area_SquareMillimeter, 0.000001),
				new UnitData(ViewMode.Area, UnitConverterUnits.Area_SquareCentimeter, 0.0001),
				new UnitData(ViewMode.Area, UnitConverterUnits.Area_SquareInch, 0.00064516),
				new UnitData(ViewMode.Area, UnitConverterUnits.Area_SquareMile, 2589988.110336),
				new UnitData(ViewMode.Area, UnitConverterUnits.Area_SquareKilometer, 1000000),
				new UnitData(ViewMode.Area, UnitConverterUnits.Area_Hectare, 10000),
				new UnitData(ViewMode.Area, UnitConverterUnits.Area_Hand, 0.012516104),
				new UnitData(ViewMode.Area, UnitConverterUnits.Area_Paper, 0.06032246),
				new UnitData(ViewMode.Area, UnitConverterUnits.Area_SoccerField, 10869.66),
				new UnitData(ViewMode.Area, UnitConverterUnits.Area_Castle, 100000),
				new UnitData(ViewMode.Area, UnitConverterUnits.Area_Pyeong, 400.0 / 121.0),

				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Bit, 0.000000125),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Byte, 0.000001),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Kilobyte, 0.001),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Megabyte, 1),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Gigabyte, 1000),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Terabyte, 1000000),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Petabyte, 1000000000),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Exabytes, 1000000000000),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Zetabytes, 1000000000000000),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Yottabyte, 1000000000000000000),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Kilobit, 0.000125),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Megabit, 0.125),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Gigabit, 125),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Terabit, 125000),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Petabit, 125000000),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Exabits, 125000000000),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Zetabits, 125000000000000),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Yottabit, 125000000000000000),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Gibibits, 134.217728),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Gibibytes, 1073.741824),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Kibibits, 0.000128),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Kibibytes, 0.001024),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Mebibits, 0.131072),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Mebibytes, 1.048576),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Pebibits, 140737488.355328),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Pebibytes, 1125899906.842624),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Tebibits, 137438.953472),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Tebibytes, 1099511.627776),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Exbibits, 144115188075.855872),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Exbibytes, 1152921504606.846976),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Zebibits, 147573952589676.412928),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Zebibytes, 1180591620717411.303424),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Yobibits, 151115727451828646.838272),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_Yobibytes, 1208925819614629174.706176),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_FloppyDisk, 1.509949),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_CD, 734.003200),
				new UnitData(ViewMode.Data, UnitConverterUnits.Data_DVD, 5046.586573),

				new UnitData(ViewMode.Energy, UnitConverterUnits.Energy_Calorie, 4.184),
				new UnitData(ViewMode.Energy, UnitConverterUnits.Energy_Kilocalorie, 4184),
				new UnitData(ViewMode.Energy, UnitConverterUnits.Energy_BritishThermalUnit, 1055.056),
				new UnitData(ViewMode.Energy, UnitConverterUnits.Energy_Kilojoule, 1000),
				new UnitData(ViewMode.Energy, UnitConverterUnits.Energy_ElectronVolt, 0.0000000000000000001602176565),
				new UnitData(ViewMode.Energy, UnitConverterUnits.Energy_Joule, 1),
				new UnitData(ViewMode.Energy, UnitConverterUnits.Energy_FootPound, 1.3558179483314),
				new UnitData(ViewMode.Energy, UnitConverterUnits.Energy_Battery, 9000),
				new UnitData(ViewMode.Energy, UnitConverterUnits.Energy_Banana, 439614),
				new UnitData(ViewMode.Energy, UnitConverterUnits.Energy_SliceOfCake, 1046700),

				new UnitData(ViewMode.Length, UnitConverterUnits.Length_Inch, 0.0254),
				new UnitData(ViewMode.Length, UnitConverterUnits.Length_Foot, 0.3048),
				new UnitData(ViewMode.Length, UnitConverterUnits.Length_Yard, 0.9144),
				new UnitData(ViewMode.Length, UnitConverterUnits.Length_Mile, 1609.344),
				new UnitData(ViewMode.Length, UnitConverterUnits.Length_Micron, 0.000001),
				new UnitData(ViewMode.Length, UnitConverterUnits.Length_Millimeter, 0.001),
				new UnitData(ViewMode.Length, UnitConverterUnits.Length_Nanometer, 0.000000001),
				new UnitData(ViewMode.Length, UnitConverterUnits.Length_Centimeter, 0.01),
				new UnitData(ViewMode.Length, UnitConverterUnits.Length_Meter, 1),
				new UnitData(ViewMode.Length, UnitConverterUnits.Length_Kilometer, 1000),
				new UnitData(ViewMode.Length, UnitConverterUnits.Length_NauticalMile, 1852),
				new UnitData(ViewMode.Length, UnitConverterUnits.Length_Paperclip, 0.035052),
				new UnitData(ViewMode.Length, UnitConverterUnits.Length_Hand, 0.18669),
				new UnitData(ViewMode.Length, UnitConverterUnits.Length_JumboJet, 76),

				new UnitData(ViewMode.Power, UnitConverterUnits.Power_BritishThermalUnitPerMinute, 17.58426666666667),
				new UnitData(ViewMode.Power, UnitConverterUnits.Power_FootPoundPerMinute, 0.0225969658055233),
				new UnitData(ViewMode.Power, UnitConverterUnits.Power_Watt, 1),
				new UnitData(ViewMode.Power, UnitConverterUnits.Power_Kilowatt, 1000),
				new UnitData(ViewMode.Power, UnitConverterUnits.Power_Horsepower, 745.69987158227022),
				new UnitData(ViewMode.Power, UnitConverterUnits.Power_LightBulb, 60),
				new UnitData(ViewMode.Power, UnitConverterUnits.Power_Horse, 745.7),
				new UnitData(ViewMode.Power, UnitConverterUnits.Power_TrainEngine, 2982799.486329081),

				new UnitData(ViewMode.Time, UnitConverterUnits.Time_Day, 86400),
				new UnitData(ViewMode.Time, UnitConverterUnits.Time_Second, 1),
				new UnitData(ViewMode.Time, UnitConverterUnits.Time_Week, 604800),
				new UnitData(ViewMode.Time, UnitConverterUnits.Time_Year, 31557600),
				new UnitData(ViewMode.Time, UnitConverterUnits.Time_Millisecond, 0.001),
				new UnitData(ViewMode.Time, UnitConverterUnits.Time_Microsecond, 0.000001),
				new UnitData(ViewMode.Time, UnitConverterUnits.Time_Minute, 60),
				new UnitData(ViewMode.Time, UnitConverterUnits.Time_Hour, 3600),

				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_CupUS, 236.588237),
				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_PintUS, 473.176473),
				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_PintUK, 568.26125),
				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_QuartUS, 946.352946),
				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_QuartUK, 1136.5225),
				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_GallonUS, 3785.411784),
				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_GallonUK, 4546.09),
				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_Liter, 1000),
				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_TeaspoonUS, 4.92892159375),
				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_TablespoonUS, 14.78676478125),
				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_CubicCentimeter, 1),
				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_CubicYard, 764554.857984),
				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_CubicMeter, 1000000),
				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_Milliliter, 1),
				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_CubicInch, 16.387064),
				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_CubicFoot, 28316.846592),
				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_FluidOunceUS, 29.5735295625),
				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_FluidOunceUK, 28.4130625),
				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_TeaspoonUK, 5.91938802083333333333),
				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_TablespoonUK, 17.7581640625),
				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_CoffeeCup, 236.5882),
				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_Bathtub, 378541.2),
				new UnitData(ViewMode.Volume, UnitConverterUnits.Volume_SwimmingPool, 3750000000),

				new UnitData(ViewMode.Weight, UnitConverterUnits.Weight_Kilogram, 1),
				new UnitData(ViewMode.Weight, UnitConverterUnits.Weight_Hectogram, 0.1),
				new UnitData(ViewMode.Weight, UnitConverterUnits.Weight_Decagram, 0.01),
				new UnitData(ViewMode.Weight, UnitConverterUnits.Weight_Gram, 0.001),
				new UnitData(ViewMode.Weight, UnitConverterUnits.Weight_Pound, 0.45359237),
				new UnitData(ViewMode.Weight, UnitConverterUnits.Weight_Ounce, 0.028349523125),
				new UnitData(ViewMode.Weight, UnitConverterUnits.Weight_Milligram, 0.000001),
				new UnitData(ViewMode.Weight, UnitConverterUnits.Weight_Centigram, 0.00001),
				new UnitData(ViewMode.Weight, UnitConverterUnits.Weight_Decigram, 0.0001),
				new UnitData(ViewMode.Weight, UnitConverterUnits.Weight_LongTon, 1016.0469088),
				new UnitData(ViewMode.Weight, UnitConverterUnits.Weight_Tonne, 1000),
				new UnitData(ViewMode.Weight, UnitConverterUnits.Weight_Stone, 6.35029318),
				new UnitData(ViewMode.Weight, UnitConverterUnits.Weight_Carat, 0.0002),
				new UnitData(ViewMode.Weight, UnitConverterUnits.Weight_ShortTon, 907.18474),
				new UnitData(ViewMode.Weight, UnitConverterUnits.Weight_Snowflake, 0.000002),
				new UnitData(ViewMode.Weight, UnitConverterUnits.Weight_SoccerBall, 0.4325),
				new UnitData(ViewMode.Weight, UnitConverterUnits.Weight_Elephant, 4000),
				new UnitData(ViewMode.Weight, UnitConverterUnits.Weight_Whale, 90000),

				new UnitData(ViewMode.Speed, UnitConverterUnits.Speed_CentimetersPerSecond, 1),
				new UnitData(ViewMode.Speed, UnitConverterUnits.Speed_FeetPerSecond, 30.48),
				new UnitData(ViewMode.Speed, UnitConverterUnits.Speed_KilometersPerHour, 27.777777777777777777778),
				new UnitData(ViewMode.Speed, UnitConverterUnits.Speed_Knot, 51.44),
				new UnitData(ViewMode.Speed, UnitConverterUnits.Speed_Mach, 34030),
				new UnitData(ViewMode.Speed, UnitConverterUnits.Speed_MetersPerSecond, 100),
				new UnitData(ViewMode.Speed, UnitConverterUnits.Speed_MilesPerHour, 44.7),
				new UnitData(ViewMode.Speed, UnitConverterUnits.Speed_Turtle, 8.94),
				new UnitData(ViewMode.Speed, UnitConverterUnits.Speed_Horse, 2011.5),
				new UnitData(ViewMode.Speed, UnitConverterUnits.Speed_Jet, 24585),

				new UnitData(ViewMode.Angle, UnitConverterUnits.Angle_Degree, 1),
				new UnitData(ViewMode.Angle, UnitConverterUnits.Angle_Radian, 57.29577951308233),
				new UnitData(ViewMode.Angle, UnitConverterUnits.Angle_Gradian, 0.9),

				new UnitData(ViewMode.Pressure, UnitConverterUnits.Pressure_Atmosphere, 1),
				new UnitData(ViewMode.Pressure, UnitConverterUnits.Pressure_Bar, 0.9869232667160128),
				new UnitData(ViewMode.Pressure, UnitConverterUnits.Pressure_KiloPascal, 0.0098692326671601),
				new UnitData(ViewMode.Pressure, UnitConverterUnits.Pressure_MillimeterOfMercury, 0.0013155687145324),
				new UnitData(ViewMode.Pressure, UnitConverterUnits.Pressure_Pascal, 9.869232667160128e-6),
				new UnitData(ViewMode.Pressure, UnitConverterUnits.Pressure_PSI, 0.068045961016531)
			};

			// Populate the hash map and return;
			foreach (UnitData unitdata in unitDataList)
			{
				if (!categoryToUnitConversionMap.TryGetValue(unitdata.categoryId, out var conversionData))
				{
					conversionData = new Dictionary<int, double> {{unitdata.unitId, unitdata.factor}};
					categoryToUnitConversionMap.Add(unitdata.categoryId, conversionData);
				}
				else
				{
					conversionData.Add(unitdata.unitId, unitdata.factor);
				}
			}
		}

		string GetLocalizedStringName(String stringId)
		{
			return AppResourceProvider.GetInstance().GetResourceString(stringId);
		}

		void GetExplicitConversionData(Dictionary<int, Dictionary<int, UCM.ConversionData>> unitToUnitConversionList)
		{
			/* categoryId, ParentUnitId, UnitId, ratio, offset, offsetfirst*/
			ExplicitUnitConversionData[] conversionDataList = new[]
			{
				new ExplicitUnitConversionData(ViewMode.Temperature, UnitConverterUnits.Temperature_DegreesCelsius, UnitConverterUnits.Temperature_DegreesCelsius, 1, 0),
				new ExplicitUnitConversionData(ViewMode.Temperature, UnitConverterUnits.Temperature_DegreesCelsius, UnitConverterUnits.Temperature_DegreesFahrenheit, 1.8, 32),
				new ExplicitUnitConversionData(ViewMode.Temperature, UnitConverterUnits.Temperature_DegreesCelsius, UnitConverterUnits.Temperature_Kelvin, 1, 273.15),
				new ExplicitUnitConversionData(ViewMode.Temperature,
					UnitConverterUnits.Temperature_DegreesFahrenheit,
					UnitConverterUnits.Temperature_DegreesCelsius,
					0.55555555555555555555555555555556,
					-32,
					CONVERT_WITH_OFFSET_FIRST),
				new ExplicitUnitConversionData(ViewMode.Temperature, UnitConverterUnits.Temperature_DegreesFahrenheit, UnitConverterUnits.Temperature_DegreesFahrenheit, 1, 0),
				new ExplicitUnitConversionData(ViewMode.Temperature,
					UnitConverterUnits.Temperature_DegreesFahrenheit,
					UnitConverterUnits.Temperature_Kelvin,
					0.55555555555555555555555555555556,
					459.67,
					CONVERT_WITH_OFFSET_FIRST),
				new ExplicitUnitConversionData(ViewMode.Temperature,
					UnitConverterUnits.Temperature_Kelvin,
					UnitConverterUnits.Temperature_DegreesCelsius,
					1,
					-273.15,
					CONVERT_WITH_OFFSET_FIRST),
				new ExplicitUnitConversionData(ViewMode.Temperature, UnitConverterUnits.Temperature_Kelvin, UnitConverterUnits.Temperature_DegreesFahrenheit, 1.8, -459.67),
				new ExplicitUnitConversionData(ViewMode.Temperature, UnitConverterUnits.Temperature_Kelvin, UnitConverterUnits.Temperature_Kelvin, 1, 0)
			};

			// Populate the hash map and return;
			foreach (ExplicitUnitConversionData data in conversionDataList)
			{
				if (!unitToUnitConversionList.TryGetValue(data.parentUnitId, out var conversionData))
				{
					conversionData = new Dictionary<int, UCM.ConversionData> {{data.unitId, (UCM.ConversionData)(data)}};
					unitToUnitConversionList.Add(data.parentUnitId, conversionData);
				}
				else
				{
					conversionData.Add(data.unitId, (UCM.ConversionData)(data));
				}
			}
		}
	}
}
