// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using CalcManager.Interop;
using CalculatorApp.ViewModel.Common;

namespace CalculatorApp.ViewModel.DataLoaders
{
    /// <summary>
    /// Loads unit converter data from resource files and provides conversion data
    /// for all non-currency unit categories.
    /// Extends ConverterDataLoaderBase to feed data to the native CalcManager engine.
    /// </summary>
    public sealed class UnitConverterDataLoader : ConverterDataLoaderBase
    {
        private CategoryWrapper[] _categories;
        private Dictionary<int, UnitWrapper[]> _unitsByCategory;
        private Dictionary<int, UnitConversionEntry[]> _ratiosByUnit;

        // Region-dependent flags (default to US)
        private readonly string _regionCode;

        public UnitConverterDataLoader()
            : this("US")
        {
        }

        public UnitConverterDataLoader(Windows.Globalization.GeographicRegion region)
            : this(region?.CodeTwoLetter ?? "US")
        {
        }

        public UnitConverterDataLoader(string regionCode)
        {
            _regionCode = regionCode ?? "US";
        }

        #region Unit ID Constants

        // These match UnitConverterUnits in UnitConverterDataConstants.h
        private const int Area_Acre = 1;
        private const int Area_Hectare = 2;
        private const int Area_SquareCentimeter = 3;
        private const int Area_SquareFoot = 4;
        private const int Area_SquareInch = 5;
        private const int Area_SquareKilometer = 6;
        private const int Area_SquareMeter = 7;
        private const int Area_SquareMile = 8;
        private const int Area_SquareMillimeter = 9;
        private const int Area_SquareYard = 10;
        private const int Data_Bit = 11;
        private const int Data_Byte = 12;
        private const int Data_Gigabit = 13;
        private const int Data_Gigabyte = 14;
        private const int Data_Kilobit = 15;
        private const int Data_Kilobyte = 16;
        private const int Data_Megabit = 17;
        private const int Data_Megabyte = 18;
        private const int Data_Petabit = 19;
        private const int Data_Petabyte = 20;
        private const int Data_Terabit = 21;
        private const int Data_Terabyte = 22;
        private const int Energy_BritishThermalUnit = 23;
        private const int Energy_Calorie = 24;
        private const int Energy_ElectronVolt = 25;
        private const int Energy_FootPound = 26;
        private const int Energy_Joule = 27;
        private const int Energy_Kilocalorie = 28;
        private const int Energy_Kilojoule = 29;
        private const int Length_Centimeter = 30;
        private const int Length_Foot = 31;
        private const int Length_Inch = 32;
        private const int Length_Kilometer = 33;
        private const int Length_Meter = 34;
        private const int Length_Micron = 35;
        private const int Length_Mile = 36;
        private const int Length_Millimeter = 37;
        private const int Length_Nanometer = 38;
        private const int Length_NauticalMile = 39;
        private const int Length_Yard = 40;
        private const int Power_BritishThermalUnitPerMinute = 41;
        private const int Power_FootPoundPerMinute = 42;
        private const int Power_Horsepower = 43;
        private const int Power_Kilowatt = 44;
        private const int Power_Watt = 45;
        private const int Temperature_DegreesCelsius = 46;
        private const int Temperature_DegreesFahrenheit = 47;
        private const int Temperature_Kelvin = 48;
        private const int Time_Day = 49;
        private const int Time_Hour = 50;
        private const int Time_Microsecond = 51;
        private const int Time_Millisecond = 52;
        private const int Time_Minute = 53;
        private const int Time_Second = 54;
        private const int Time_Week = 55;
        private const int Time_Year = 56;
        private const int Speed_CentimetersPerSecond = 57;
        private const int Speed_FeetPerSecond = 58;
        private const int Speed_KilometersPerHour = 59;
        private const int Speed_Knot = 60;
        private const int Speed_Mach = 61;
        private const int Speed_MetersPerSecond = 62;
        private const int Speed_MilesPerHour = 63;
        private const int Volume_CubicCentimeter = 64;
        private const int Volume_CubicFoot = 65;
        private const int Volume_CubicInch = 66;
        private const int Volume_CubicMeter = 67;
        private const int Volume_CubicYard = 68;
        private const int Volume_CupUS = 69;
        private const int Volume_FluidOunceUK = 70;
        private const int Volume_FluidOunceUS = 71;
        private const int Volume_GallonUK = 72;
        private const int Volume_GallonUS = 73;
        private const int Volume_Liter = 74;
        private const int Volume_Milliliter = 75;
        private const int Volume_PintUK = 76;
        private const int Volume_PintUS = 77;
        private const int Volume_TablespoonUS = 78;
        private const int Volume_TeaspoonUS = 79;
        private const int Volume_QuartUK = 80;
        private const int Volume_QuartUS = 81;
        private const int Weight_Carat = 82;
        private const int Weight_Centigram = 83;
        private const int Weight_Decigram = 84;
        private const int Weight_Decagram = 85;
        private const int Weight_Gram = 86;
        private const int Weight_Hectogram = 87;
        private const int Weight_Kilogram = 88;
        private const int Weight_LongTon = 89;
        private const int Weight_Milligram = 90;
        private const int Weight_Ounce = 91;
        private const int Weight_Pound = 92;
        private const int Weight_ShortTon = 93;
        private const int Weight_Stone = 94;
        private const int Weight_Tonne = 95;
        private const int Area_SoccerField = 99;
        private const int Data_FloppyDisk = 100;
        private const int Data_CD = 101;
        private const int Data_DVD = 102;
        private const int Energy_Battery = 103;
        private const int Length_Paperclip = 105;
        private const int Length_JumboJet = 107;
        private const int Power_LightBulb = 108;
        private const int Power_Horse = 109;
        private const int Volume_Bathtub = 111;
        private const int Weight_Snowflake = 113;
        private const int Weight_Elephant = 114;
        private const int Volume_TeaspoonUK = 115;
        private const int Volume_TablespoonUK = 116;
        private const int Area_Hand = 118;
        private const int Speed_Turtle = 121;
        private const int Speed_Jet = 122;
        private const int Volume_CoffeeCup = 124;
        private const int Weight_Whale = 123;
        private const int Volume_SwimmingPool = 125;
        private const int Speed_Horse = 126;
        private const int Area_Paper = 127;
        private const int Area_Castle = 128;
        private const int Energy_Banana = 129;
        private const int Energy_SliceOfCake = 130;
        private const int Length_Hand = 131;
        private const int Power_TrainEngine = 132;
        private const int Weight_SoccerBall = 133;
        private const int Angle_Degree = 134;
        private const int Angle_Radian = 135;
        private const int Angle_Gradian = 136;
        private const int Pressure_Atmosphere = 137;
        private const int Pressure_Bar = 138;
        private const int Pressure_KiloPascal = 139;
        private const int Pressure_MillimeterOfMercury = 140;
        private const int Pressure_Pascal = 141;
        private const int Pressure_PSI = 142;
        private const int Data_Exabits = 143;
        private const int Data_Exabytes = 144;
        private const int Data_Exbibits = 145;
        private const int Data_Exbibytes = 146;
        private const int Data_Gibibits = 147;
        private const int Data_Gibibytes = 148;
        private const int Data_Kibibits = 149;
        private const int Data_Kibibytes = 150;
        private const int Data_Mebibits = 151;
        private const int Data_Mebibytes = 152;
        private const int Data_Pebibits = 153;
        private const int Data_Pebibytes = 154;
        private const int Data_Tebibits = 155;
        private const int Data_Tebibytes = 156;
        private const int Data_Yobibits = 157;
        private const int Data_Yobibytes = 158;
        private const int Data_Yottabit = 159;
        private const int Data_Yottabyte = 160;
        private const int Data_Zebibits = 161;
        private const int Data_Zebibytes = 162;
        private const int Data_Zetabits = 163;
        private const int Data_Zetabytes = 164;
        private const int Area_Pyeong = 165;
        private const int Energy_Kilowatthour = 166;
        private const int Data_Nibble = 167;
        private const int Length_Angstrom = 168;

        #endregion

        protected override void LoadData()
        {
            var resProvider = AppResourceProvider.GetInstance();

            // Determine region-dependent flags
            bool useUSCustomaryAndFahrenheit =
                _regionCode == "US" || _regionCode == "FM" || _regionCode == "MH" || _regionCode == "PW";
            bool useUSCustomary = useUSCustomaryAndFahrenheit || _regionCode == "LR";
            bool useSI = !useUSCustomary;
            bool useFahrenheit = useUSCustomaryAndFahrenheit || _regionCode == "BS" || _regionCode == "KY" || _regionCode == "LR";
            bool useWattInsteadOfKilowatt = _regionCode == "GB";
            bool usePyeong = _regionCode == "JP" || _regionCode == "TW" || _regionCode == "KP" || _regionCode == "KR";

            // Build categories — include Currency placeholder at position 0 to match NavCategoryManifest order
            // (MainPage.GetIndexInGroup counts Currency when computing the category index)
            var converterModes = new[]
            {
                ViewMode.Currency,
                ViewMode.Volume, ViewMode.Length, ViewMode.Weight,
                ViewMode.Temperature, ViewMode.Energy, ViewMode.Area,
                ViewMode.Speed, ViewMode.Time, ViewMode.Power,
                ViewMode.Data, ViewMode.Pressure, ViewMode.Angle
            };

            var categoryList = new List<CategoryWrapper>();
            foreach (var mode in converterModes)
            {
                string nameResourceKey = NavCategoryStates.GetNameResourceKey(mode);
                string categoryName = nameResourceKey != null ? resProvider.GetResourceString(nameResourceKey) : mode.ToString();
                bool supportsNegative = mode == ViewMode.Temperature || mode == ViewMode.Power || mode == ViewMode.Angle;

                categoryList.Add(new CategoryWrapper
                {
                    Id = NavCategoryStates.Serialize(mode),
                    Name = categoryName,
                    SupportsNegative = supportsNegative
                });
            }
            _categories = categoryList.ToArray();

            // Helper to get localized string
            string Loc(string key) => resProvider.GetResourceString(key);

            // Build units per category
            var unitsByCategory = new Dictionary<int, List<(UnitWrapper unit, int order)>>();

            void AddUnit(ViewMode mode, int id, string nameKey, string abbrKey, int order,
                bool isSource = false, bool isTarget = false, bool isWhimsical = false)
            {
                int catId = NavCategoryStates.Serialize(mode);
                if (!unitsByCategory.ContainsKey(catId))
                    unitsByCategory[catId] = new List<(UnitWrapper, int)>();

                unitsByCategory[catId].Add((new UnitWrapper
                {
                    Id = id,
                    Name = Loc(nameKey),
                    AccessibleName = Loc(nameKey),
                    Abbreviation = Loc(abbrKey),
                    IsConversionSource = isSource,
                    IsConversionTarget = isTarget,
                    IsWhimsical = isWhimsical
                }, order));
            }

            // ---- Area ----
            AddUnit(ViewMode.Area, Area_Acre, "UnitName_Acre", "UnitAbbreviation_Acre", 9);
            AddUnit(ViewMode.Area, Area_Hectare, "UnitName_Hectare", "UnitAbbreviation_Hectare", 4);
            AddUnit(ViewMode.Area, Area_SquareCentimeter, "UnitName_SquareCentimeter", "UnitAbbreviation_SquareCentimeter", 2);
            AddUnit(ViewMode.Area, Area_SquareFoot, "UnitName_SquareFoot", "UnitAbbreviation_SquareFoot", 7, useSI, useUSCustomary);
            AddUnit(ViewMode.Area, Area_SquareInch, "UnitName_SquareInch", "UnitAbbreviation_SquareInch", 6);
            AddUnit(ViewMode.Area, Area_SquareKilometer, "UnitName_SquareKilometer", "UnitAbbreviation_SquareKilometer", 5);
            AddUnit(ViewMode.Area, Area_SquareMeter, "UnitName_SquareMeter", "UnitAbbreviation_SquareMeter", 3, useUSCustomary, useSI);
            AddUnit(ViewMode.Area, Area_SquareMile, "UnitName_SquareMile", "UnitAbbreviation_SquareMile", 10);
            AddUnit(ViewMode.Area, Area_SquareMillimeter, "UnitName_SquareMillimeter", "UnitAbbreviation_SquareMillimeter", 1);
            AddUnit(ViewMode.Area, Area_SquareYard, "UnitName_SquareYard", "UnitAbbreviation_SquareYard", 8);
            AddUnit(ViewMode.Area, Area_Hand, "UnitName_Hand", "UnitAbbreviation_Hand", 11, false, false, true);
            AddUnit(ViewMode.Area, Area_Paper, "UnitName_Paper", "UnitAbbreviation_Paper", 12, false, false, true);
            AddUnit(ViewMode.Area, Area_SoccerField, "UnitName_SoccerField", "UnitAbbreviation_SoccerField", 13, false, false, true);
            AddUnit(ViewMode.Area, Area_Castle, "UnitName_Castle", "UnitAbbreviation_Castle", 14, false, false, true);
            if (usePyeong)
            {
                AddUnit(ViewMode.Area, Area_Pyeong, "UnitName_Pyeong", "UnitAbbreviation_Pyeong", 15);
            }

            // ---- Data ----
            AddUnit(ViewMode.Data, Data_Bit, "UnitName_Bit", "UnitAbbreviation_Bit", 1);
            AddUnit(ViewMode.Data, Data_Byte, "UnitName_Byte", "UnitAbbreviation_Byte", 3);
            AddUnit(ViewMode.Data, Data_Exabits, "UnitName_Exabits", "UnitAbbreviation_Exabits", 24);
            AddUnit(ViewMode.Data, Data_Exabytes, "UnitName_Exabytes", "UnitAbbreviation_Exabytes", 26);
            AddUnit(ViewMode.Data, Data_Exbibits, "UnitName_Exbibits", "UnitAbbreviation_Exbibits", 25);
            AddUnit(ViewMode.Data, Data_Exbibytes, "UnitName_Exbibytes", "UnitAbbreviation_Exbibytes", 27);
            AddUnit(ViewMode.Data, Data_Gibibits, "UnitName_Gibibits", "UnitAbbreviation_Gibibits", 13);
            AddUnit(ViewMode.Data, Data_Gibibytes, "UnitName_Gibibytes", "UnitAbbreviation_Gibibytes", 15);
            AddUnit(ViewMode.Data, Data_Gigabit, "UnitName_Gigabit", "UnitAbbreviation_Gigabit", 12);
            AddUnit(ViewMode.Data, Data_Gigabyte, "UnitName_Gigabyte", "UnitAbbreviation_Gigabyte", 14, true, false);
            AddUnit(ViewMode.Data, Data_Kibibits, "UnitName_Kibibits", "UnitAbbreviation_Kibibits", 5);
            AddUnit(ViewMode.Data, Data_Kibibytes, "UnitName_Kibibytes", "UnitAbbreviation_Kibibytes", 7);
            AddUnit(ViewMode.Data, Data_Kilobit, "UnitName_Kilobit", "UnitAbbreviation_Kilobit", 4);
            AddUnit(ViewMode.Data, Data_Kilobyte, "UnitName_Kilobyte", "UnitAbbreviation_Kilobyte", 6);
            AddUnit(ViewMode.Data, Data_Mebibits, "UnitName_Mebibits", "UnitAbbreviation_Mebibits", 9);
            AddUnit(ViewMode.Data, Data_Mebibytes, "UnitName_Mebibytes", "UnitAbbreviation_Mebibytes", 11);
            AddUnit(ViewMode.Data, Data_Megabit, "UnitName_Megabit", "UnitAbbreviation_Megabit", 8);
            AddUnit(ViewMode.Data, Data_Megabyte, "UnitName_Megabyte", "UnitAbbreviation_Megabyte", 10, false, true);
            AddUnit(ViewMode.Data, Data_Nibble, "UnitName_Nibble", "UnitAbbreviation_Nibble", 2);
            AddUnit(ViewMode.Data, Data_Pebibits, "UnitName_Pebibits", "UnitAbbreviation_Pebibits", 21);
            AddUnit(ViewMode.Data, Data_Pebibytes, "UnitName_Pebibytes", "UnitAbbreviation_Pebibytes", 23);
            AddUnit(ViewMode.Data, Data_Petabit, "UnitName_Petabit", "UnitAbbreviation_Petabit", 20);
            AddUnit(ViewMode.Data, Data_Petabyte, "UnitName_Petabyte", "UnitAbbreviation_Petabyte", 22);
            AddUnit(ViewMode.Data, Data_Tebibits, "UnitName_Tebibits", "UnitAbbreviation_Tebibits", 17);
            AddUnit(ViewMode.Data, Data_Tebibytes, "UnitName_Tebibytes", "UnitAbbreviation_Tebibytes", 19);
            AddUnit(ViewMode.Data, Data_Terabit, "UnitName_Terabit", "UnitAbbreviation_Terabit", 16);
            AddUnit(ViewMode.Data, Data_Terabyte, "UnitName_Terabyte", "UnitAbbreviation_Terabyte", 18);
            AddUnit(ViewMode.Data, Data_Yobibits, "UnitName_Yobibits", "UnitAbbreviation_Yobibits", 33);
            AddUnit(ViewMode.Data, Data_Yobibytes, "UnitName_Yobibytes", "UnitAbbreviation_Yobibytes", 35);
            AddUnit(ViewMode.Data, Data_Yottabit, "UnitName_Yottabit", "UnitAbbreviation_Yottabit", 32);
            AddUnit(ViewMode.Data, Data_Yottabyte, "UnitName_Yottabyte", "UnitAbbreviation_Yottabyte", 34);
            AddUnit(ViewMode.Data, Data_Zebibits, "UnitName_Zebibits", "UnitAbbreviation_Zebibits", 29);
            AddUnit(ViewMode.Data, Data_Zebibytes, "UnitName_Zebibytes", "UnitAbbreviation_Zebibytes", 31);
            AddUnit(ViewMode.Data, Data_Zetabits, "UnitName_Zetabits", "UnitAbbreviation_Zetabits", 28);
            AddUnit(ViewMode.Data, Data_Zetabytes, "UnitName_Zetabytes", "UnitAbbreviation_Zetabytes", 30);
            AddUnit(ViewMode.Data, Data_FloppyDisk, "UnitName_FloppyDisk", "UnitAbbreviation_FloppyDisk", 13, false, false, true);
            AddUnit(ViewMode.Data, Data_CD, "UnitName_CD", "UnitAbbreviation_CD", 14, false, false, true);
            AddUnit(ViewMode.Data, Data_DVD, "UnitName_DVD", "UnitAbbreviation_DVD", 15, false, false, true);

            // ---- Energy ----
            AddUnit(ViewMode.Energy, Energy_BritishThermalUnit, "UnitName_BritishThermalUnit", "UnitAbbreviation_BritishThermalUnit", 7);
            AddUnit(ViewMode.Energy, Energy_Calorie, "UnitName_Calorie", "UnitAbbreviation_Calorie", 4);
            AddUnit(ViewMode.Energy, Energy_ElectronVolt, "UnitName_Electron-Volt", "UnitAbbreviation_Electron-Volt", 1);
            AddUnit(ViewMode.Energy, Energy_FootPound, "UnitName_Foot-Pound", "UnitAbbreviation_Foot-Pound", 6);
            AddUnit(ViewMode.Energy, Energy_Joule, "UnitName_Joule", "UnitAbbreviation_Joule", 2, true, false);
            AddUnit(ViewMode.Energy, Energy_Kilowatthour, "UnitName_Kilowatthour", "UnitAbbreviation_Kilowatthour", 166, true, false);
            AddUnit(ViewMode.Energy, Energy_Kilocalorie, "UnitName_Kilocalorie", "UnitAbbreviation_Kilocalorie", 5, false, true);
            AddUnit(ViewMode.Energy, Energy_Kilojoule, "UnitName_Kilojoule", "UnitAbbreviation_Kilojoule", 3);
            AddUnit(ViewMode.Energy, Energy_Battery, "UnitName_Battery", "UnitAbbreviation_Battery", 8, false, false, true);
            AddUnit(ViewMode.Energy, Energy_Banana, "UnitName_Banana", "UnitAbbreviation_Banana", 9, false, false, true);
            AddUnit(ViewMode.Energy, Energy_SliceOfCake, "UnitName_SliceOfCake", "UnitAbbreviation_SliceOfCake", 10, false, false, true);

            // ---- Length ----
            AddUnit(ViewMode.Length, Length_Angstrom, "UnitName_Angstrom", "UnitAbbreviation_Angstrom", 1);
            AddUnit(ViewMode.Length, Length_Centimeter, "UnitName_Centimeter", "UnitAbbreviation_Centimeter", 5, useUSCustomary, useSI);
            AddUnit(ViewMode.Length, Length_Foot, "UnitName_Foot", "UnitAbbreviation_Foot", 9);
            AddUnit(ViewMode.Length, Length_Inch, "UnitName_Inch", "UnitAbbreviation_Inch", 8, useSI, useUSCustomary);
            AddUnit(ViewMode.Length, Length_Kilometer, "UnitName_Kilometer", "UnitAbbreviation_Kilometer", 7);
            AddUnit(ViewMode.Length, Length_Meter, "UnitName_Meter", "UnitAbbreviation_Meter", 6);
            AddUnit(ViewMode.Length, Length_Micron, "UnitName_Micron", "UnitAbbreviation_Micron", 3);
            AddUnit(ViewMode.Length, Length_Mile, "UnitName_Mile", "UnitAbbreviation_Mile", 11);
            AddUnit(ViewMode.Length, Length_Millimeter, "UnitName_Millimeter", "UnitAbbreviation_Millimeter", 4);
            AddUnit(ViewMode.Length, Length_Nanometer, "UnitName_Nanometer", "UnitAbbreviation_Nanometer", 2);
            AddUnit(ViewMode.Length, Length_NauticalMile, "UnitName_NauticalMile", "UnitAbbreviation_NauticalMile", 12);
            AddUnit(ViewMode.Length, Length_Yard, "UnitName_Yard", "UnitAbbreviation_Yard", 10);
            AddUnit(ViewMode.Length, Length_Paperclip, "UnitName_Paperclip", "UnitAbbreviation_Paperclip", 13, false, false, true);
            AddUnit(ViewMode.Length, Length_Hand, "UnitName_Hand", "UnitAbbreviation_Hand", 14, false, false, true);
            AddUnit(ViewMode.Length, Length_JumboJet, "UnitName_JumboJet", "UnitAbbreviation_JumboJet", 15, false, false, true);

            // ---- Power ----
            AddUnit(ViewMode.Power, Power_BritishThermalUnitPerMinute, "UnitName_BTUPerMinute", "UnitAbbreviation_BTUPerMinute", 5);
            AddUnit(ViewMode.Power, Power_FootPoundPerMinute, "UnitName_Foot-PoundPerMinute", "UnitAbbreviation_Foot-PoundPerMinute", 4);
            AddUnit(ViewMode.Power, Power_Horsepower, "UnitName_Horsepower", "UnitAbbreviation_Horsepower", 3, false, true);
            AddUnit(ViewMode.Power, Power_Kilowatt, "UnitName_Kilowatt", "UnitAbbreviation_Kilowatt", 2, !useWattInsteadOfKilowatt);
            AddUnit(ViewMode.Power, Power_Watt, "UnitName_Watt", "UnitAbbreviation_Watt", 1, useWattInsteadOfKilowatt);
            AddUnit(ViewMode.Power, Power_LightBulb, "UnitName_LightBulb", "UnitAbbreviation_LightBulb", 6, false, false, true);
            AddUnit(ViewMode.Power, Power_Horse, "UnitName_Horse", "UnitAbbreviation_Horse", 7, false, false, true);
            AddUnit(ViewMode.Power, Power_TrainEngine, "UnitName_TrainEngine", "UnitAbbreviation_TrainEngine", 8, false, false, true);

            // ---- Temperature ----
            AddUnit(ViewMode.Temperature, Temperature_DegreesCelsius, "UnitName_DegreesCelsius", "UnitAbbreviation_DegreesCelsius", 1, useFahrenheit, !useFahrenheit);
            AddUnit(ViewMode.Temperature, Temperature_DegreesFahrenheit, "UnitName_DegreesFahrenheit", "UnitAbbreviation_DegreesFahrenheit", 2, !useFahrenheit, useFahrenheit);
            AddUnit(ViewMode.Temperature, Temperature_Kelvin, "UnitName_Kelvin", "UnitAbbreviation_Kelvin", 3);

            // ---- Time ----
            AddUnit(ViewMode.Time, Time_Day, "UnitName_Day", "UnitAbbreviation_Day", 6);
            AddUnit(ViewMode.Time, Time_Hour, "UnitName_Hour", "UnitAbbreviation_Hour", 5, true, false);
            AddUnit(ViewMode.Time, Time_Microsecond, "UnitName_Microsecond", "UnitAbbreviation_Microsecond", 1);
            AddUnit(ViewMode.Time, Time_Millisecond, "UnitName_Millisecond", "UnitAbbreviation_Millisecond", 2);
            AddUnit(ViewMode.Time, Time_Minute, "UnitName_Minute", "UnitAbbreviation_Minute", 4, false, true);
            AddUnit(ViewMode.Time, Time_Second, "UnitName_Second", "UnitAbbreviation_Second", 3);
            AddUnit(ViewMode.Time, Time_Week, "UnitName_Week", "UnitAbbreviation_Week", 7);
            AddUnit(ViewMode.Time, Time_Year, "UnitName_Year", "UnitAbbreviation_Year", 8);

            // ---- Speed ----
            AddUnit(ViewMode.Speed, Speed_CentimetersPerSecond, "UnitName_CentimetersPerSecond", "UnitAbbreviation_CentimetersPerSecond", 1);
            AddUnit(ViewMode.Speed, Speed_FeetPerSecond, "UnitName_FeetPerSecond", "UnitAbbreviation_FeetPerSecond", 4);
            AddUnit(ViewMode.Speed, Speed_KilometersPerHour, "UnitName_KilometersPerHour", "UnitAbbreviation_KilometersPerHour", 3, useUSCustomary, useSI);
            AddUnit(ViewMode.Speed, Speed_Knot, "UnitName_Knot", "UnitAbbreviation_Knot", 6);
            AddUnit(ViewMode.Speed, Speed_Mach, "UnitName_Mach", "UnitAbbreviation_Mach", 7);
            AddUnit(ViewMode.Speed, Speed_MetersPerSecond, "UnitName_MetersPerSecond", "UnitAbbreviation_MetersPerSecond", 2);
            AddUnit(ViewMode.Speed, Speed_MilesPerHour, "UnitName_MilesPerHour", "UnitAbbreviation_MilesPerHour", 5, useSI, useUSCustomary);
            AddUnit(ViewMode.Speed, Speed_Turtle, "UnitName_Turtle", "UnitAbbreviation_Turtle", 8, false, false, true);
            AddUnit(ViewMode.Speed, Speed_Horse, "UnitName_Horse", "UnitAbbreviation_Horse", 9, false, false, true);
            AddUnit(ViewMode.Speed, Speed_Jet, "UnitName_Jet", "UnitAbbreviation_Jet", 10, false, false, true);

            // ---- Volume ----
            AddUnit(ViewMode.Volume, Volume_CubicCentimeter, "UnitName_CubicCentimeter", "UnitAbbreviation_CubicCentimeter", 2);
            AddUnit(ViewMode.Volume, Volume_CubicFoot, "UnitName_CubicFoot", "UnitAbbreviation_CubicFoot", 13);
            AddUnit(ViewMode.Volume, Volume_CubicInch, "UnitName_CubicInch", "UnitAbbreviation_CubicInch", 12);
            AddUnit(ViewMode.Volume, Volume_CubicMeter, "UnitName_CubicMeter", "UnitAbbreviation_CubicMeter", 4);
            AddUnit(ViewMode.Volume, Volume_CubicYard, "UnitName_CubicYard", "UnitAbbreviation_CubicYard", 14);
            AddUnit(ViewMode.Volume, Volume_CupUS, "UnitName_CupUS", "UnitAbbreviation_CupUS", 8);
            AddUnit(ViewMode.Volume, Volume_FluidOunceUK, "UnitName_FluidOunceUK", "UnitAbbreviation_FluidOunceUK", 17);
            AddUnit(ViewMode.Volume, Volume_FluidOunceUS, "UnitName_FluidOunceUS", "UnitAbbreviation_FluidOunceUS", 7);
            AddUnit(ViewMode.Volume, Volume_GallonUK, "UnitName_GallonUK", "UnitAbbreviation_GallonUK", 20);
            AddUnit(ViewMode.Volume, Volume_GallonUS, "UnitName_GallonUS", "UnitAbbreviation_GallonUS", 11);
            AddUnit(ViewMode.Volume, Volume_Liter, "UnitName_Liter", "UnitAbbreviation_Liter", 3);
            AddUnit(ViewMode.Volume, Volume_Milliliter, "UnitName_Milliliter", "UnitAbbreviation_Milliliter", 1, useUSCustomary, useSI);
            AddUnit(ViewMode.Volume, Volume_PintUK, "UnitName_PintUK", "UnitAbbreviation_PintUK", 18);
            AddUnit(ViewMode.Volume, Volume_PintUS, "UnitName_PintUS", "UnitAbbreviation_PintUS", 9);
            AddUnit(ViewMode.Volume, Volume_TablespoonUS, "UnitName_TablespoonUS", "UnitAbbreviation_TablespoonUS", 6);
            AddUnit(ViewMode.Volume, Volume_TeaspoonUS, "UnitName_TeaspoonUS", "UnitAbbreviation_TeaspoonUS", 5, useSI, useUSCustomary && _regionCode != "GB");
            AddUnit(ViewMode.Volume, Volume_QuartUK, "UnitName_QuartUK", "UnitAbbreviation_QuartUK", 19);
            AddUnit(ViewMode.Volume, Volume_QuartUS, "UnitName_QuartUS", "UnitAbbreviation_QuartUS", 10);
            AddUnit(ViewMode.Volume, Volume_TeaspoonUK, "UnitName_TeaspoonUK", "UnitAbbreviation_TeaspoonUK", 15, false, useUSCustomary && _regionCode == "GB");
            AddUnit(ViewMode.Volume, Volume_TablespoonUK, "UnitName_TablespoonUK", "UnitAbbreviation_TablespoonUK", 16);
            AddUnit(ViewMode.Volume, Volume_CoffeeCup, "UnitName_CoffeeCup", "UnitAbbreviation_CoffeeCup", 22, false, false, true);
            AddUnit(ViewMode.Volume, Volume_Bathtub, "UnitName_Bathtub", "UnitAbbreviation_Bathtub", 23, false, false, true);
            AddUnit(ViewMode.Volume, Volume_SwimmingPool, "UnitName_SwimmingPool", "UnitAbbreviation_SwimmingPool", 24, false, false, true);

            // ---- Weight ----
            AddUnit(ViewMode.Weight, Weight_Carat, "UnitName_Carat", "UnitAbbreviation_Carat", 1);
            AddUnit(ViewMode.Weight, Weight_Centigram, "UnitName_Centigram", "UnitAbbreviation_Centigram", 3);
            AddUnit(ViewMode.Weight, Weight_Decigram, "UnitName_Decigram", "UnitAbbreviation_Decigram", 4);
            AddUnit(ViewMode.Weight, Weight_Decagram, "UnitName_Decagram", "UnitAbbreviation_Decagram", 6);
            AddUnit(ViewMode.Weight, Weight_Gram, "UnitName_Gram", "UnitAbbreviation_Gram", 5);
            AddUnit(ViewMode.Weight, Weight_Hectogram, "UnitName_Hectogram", "UnitAbbreviation_Hectogram", 7);
            AddUnit(ViewMode.Weight, Weight_Kilogram, "UnitName_Kilogram", "UnitAbbreviation_Kilogram", 8, useUSCustomary, useSI);
            AddUnit(ViewMode.Weight, Weight_LongTon, "UnitName_LongTon", "UnitAbbreviation_LongTon", 14);
            AddUnit(ViewMode.Weight, Weight_Milligram, "UnitName_Milligram", "UnitAbbreviation_Milligram", 2);
            AddUnit(ViewMode.Weight, Weight_Ounce, "UnitName_Ounce", "UnitAbbreviation_Ounce", 10);
            AddUnit(ViewMode.Weight, Weight_Pound, "UnitName_Pound", "UnitAbbreviation_Pound", 11, useSI, useUSCustomary);
            AddUnit(ViewMode.Weight, Weight_ShortTon, "UnitName_ShortTon", "UnitAbbreviation_ShortTon", 13);
            AddUnit(ViewMode.Weight, Weight_Stone, "UnitName_Stone", "UnitAbbreviation_Stone", 12);
            AddUnit(ViewMode.Weight, Weight_Tonne, "UnitName_Tonne", "UnitAbbreviation_Tonne", 9);
            AddUnit(ViewMode.Weight, Weight_Snowflake, "UnitName_Snowflake", "UnitAbbreviation_Snowflake", 15, false, false, true);
            AddUnit(ViewMode.Weight, Weight_SoccerBall, "UnitName_SoccerBall", "UnitAbbreviation_SoccerBall", 16, false, false, true);
            AddUnit(ViewMode.Weight, Weight_Elephant, "UnitName_Elephant", "UnitAbbreviation_Elephant", 17, false, false, true);
            AddUnit(ViewMode.Weight, Weight_Whale, "UnitName_Whale", "UnitAbbreviation_Whale", 18, false, false, true);

            // ---- Pressure ----
            AddUnit(ViewMode.Pressure, Pressure_Atmosphere, "UnitName_Atmosphere", "UnitAbbreviation_Atmosphere", 1, true, false);
            AddUnit(ViewMode.Pressure, Pressure_Bar, "UnitName_Bar", "UnitAbbreviation_Bar", 2, false, true);
            AddUnit(ViewMode.Pressure, Pressure_KiloPascal, "UnitName_KiloPascal", "UnitAbbreviation_KiloPascal", 3);
            AddUnit(ViewMode.Pressure, Pressure_MillimeterOfMercury, "UnitName_MillimeterOfMercury ", "UnitAbbreviation_MillimeterOfMercury ", 4);
            AddUnit(ViewMode.Pressure, Pressure_Pascal, "UnitName_Pascal", "UnitAbbreviation_Pascal", 5);
            AddUnit(ViewMode.Pressure, Pressure_PSI, "UnitName_PSI", "UnitAbbreviation_PSI", 6);

            // ---- Angle ----
            AddUnit(ViewMode.Angle, Angle_Degree, "UnitName_Degree", "UnitAbbreviation_Degree", 1, true, false);
            AddUnit(ViewMode.Angle, Angle_Radian, "UnitName_Radian", "UnitAbbreviation_Radian", 2, false, true);
            AddUnit(ViewMode.Angle, Angle_Gradian, "UnitName_Gradian", "UnitAbbreviation_Gradian", 3);

            // Sort units by order and store
            _unitsByCategory = new Dictionary<int, UnitWrapper[]>();
            foreach (var kvp in unitsByCategory)
            {
                var sorted = kvp.Value.OrderBy(x => x.order).Select(x => x.unit).ToArray();
                _unitsByCategory[kvp.Key] = sorted;
            }

            // Ensure all categories have an entry (Currency gets empty)
            foreach (var cat in _categories)
            {
                if (!_unitsByCategory.ContainsKey(cat.Id))
                    _unitsByCategory[cat.Id] = Array.Empty<UnitWrapper>();
            }

            // Build conversion factors per category (matches GetConversionData in C++)
            var conversionFactors = new Dictionary<ViewMode, Dictionary<int, double>>
            {
                [ViewMode.Area] = new Dictionary<int, double>
                {
                    { Area_Acre, 4046.8564224 },
                    { Area_SquareMeter, 1 },
                    { Area_SquareFoot, 0.09290304 },
                    { Area_SquareYard, 0.83612736 },
                    { Area_SquareMillimeter, 0.000001 },
                    { Area_SquareCentimeter, 0.0001 },
                    { Area_SquareInch, 0.00064516 },
                    { Area_SquareMile, 2589988.110336 },
                    { Area_SquareKilometer, 1000000 },
                    { Area_Hectare, 10000 },
                    { Area_Hand, 0.012516104 },
                    { Area_Paper, 0.06032246 },
                    { Area_SoccerField, 10869.66 },
                    { Area_Castle, 100000 },
                    { Area_Pyeong, 400.0 / 121.0 },
                },
                [ViewMode.Data] = new Dictionary<int, double>
                {
                    { Data_Bit, 0.000000125 },
                    { Data_Nibble, 0.0000005 },
                    { Data_Byte, 0.000001 },
                    { Data_Kilobyte, 0.001 },
                    { Data_Megabyte, 1 },
                    { Data_Gigabyte, 1000 },
                    { Data_Terabyte, 1000000 },
                    { Data_Petabyte, 1000000000 },
                    { Data_Exabytes, 1000000000000 },
                    { Data_Zetabytes, 1000000000000000 },
                    { Data_Yottabyte, 1000000000000000000 },
                    { Data_Kilobit, 0.000125 },
                    { Data_Megabit, 0.125 },
                    { Data_Gigabit, 125 },
                    { Data_Terabit, 125000 },
                    { Data_Petabit, 125000000 },
                    { Data_Exabits, 125000000000 },
                    { Data_Zetabits, 125000000000000 },
                    { Data_Yottabit, 125000000000000000 },
                    { Data_Gibibits, 134.217728 },
                    { Data_Gibibytes, 1073.741824 },
                    { Data_Kibibits, 0.000128 },
                    { Data_Kibibytes, 0.001024 },
                    { Data_Mebibits, 0.131072 },
                    { Data_Mebibytes, 1.048576 },
                    { Data_Pebibits, 140737488.355328 },
                    { Data_Pebibytes, 1125899906.842624 },
                    { Data_Tebibits, 137438.953472 },
                    { Data_Tebibytes, 1099511.627776 },
                    { Data_Exbibits, 144115188075.855872 },
                    { Data_Exbibytes, 1152921504606.846976 },
                    { Data_Zebibits, 147573952589676.412928 },
                    { Data_Zebibytes, 1180591620717411.303424 },
                    { Data_Yobibits, 151115727451828646.838272 },
                    { Data_Yobibytes, 1208925819614629174.706176 },
                    { Data_FloppyDisk, 1.474560 },
                    { Data_CD, 700 },
                    { Data_DVD, 4700 },
                },
                [ViewMode.Energy] = new Dictionary<int, double>
                {
                    { Energy_Calorie, 4.184 },
                    { Energy_Kilocalorie, 4184 },
                    { Energy_BritishThermalUnit, 1055.056 },
                    { Energy_Kilojoule, 1000 },
                    { Energy_Kilowatthour, 3600000 },
                    { Energy_ElectronVolt, 0.0000000000000000001602176565 },
                    { Energy_Joule, 1 },
                    { Energy_FootPound, 1.3558179483314 },
                    { Energy_Battery, 9000 },
                    { Energy_Banana, 439614 },
                    { Energy_SliceOfCake, 1046700 },
                },
                [ViewMode.Length] = new Dictionary<int, double>
                {
                    { Length_Inch, 0.0254 },
                    { Length_Foot, 0.3048 },
                    { Length_Yard, 0.9144 },
                    { Length_Mile, 1609.344 },
                    { Length_Micron, 0.000001 },
                    { Length_Millimeter, 0.001 },
                    { Length_Nanometer, 0.000000001 },
                    { Length_Angstrom, 0.0000000001 },
                    { Length_Centimeter, 0.01 },
                    { Length_Meter, 1 },
                    { Length_Kilometer, 1000 },
                    { Length_NauticalMile, 1852 },
                    { Length_Paperclip, 0.035052 },
                    { Length_Hand, 0.18669 },
                    { Length_JumboJet, 76 },
                },
                [ViewMode.Power] = new Dictionary<int, double>
                {
                    { Power_BritishThermalUnitPerMinute, 17.58426666666667 },
                    { Power_FootPoundPerMinute, 0.0225969658055233 },
                    { Power_Watt, 1 },
                    { Power_Kilowatt, 1000 },
                    { Power_Horsepower, 745.69987158227022 },
                    { Power_LightBulb, 60 },
                    { Power_Horse, 745.7 },
                    { Power_TrainEngine, 2982799.486329081 },
                },
                [ViewMode.Time] = new Dictionary<int, double>
                {
                    { Time_Day, 86400 },
                    { Time_Second, 1 },
                    { Time_Week, 604800 },
                    { Time_Year, 31557600 },
                    { Time_Millisecond, 0.001 },
                    { Time_Microsecond, 0.000001 },
                    { Time_Minute, 60 },
                    { Time_Hour, 3600 },
                },
                [ViewMode.Volume] = new Dictionary<int, double>
                {
                    { Volume_CupUS, 236.588237 },
                    { Volume_PintUS, 473.176473 },
                    { Volume_PintUK, 568.26125 },
                    { Volume_QuartUS, 946.352946 },
                    { Volume_QuartUK, 1136.5225 },
                    { Volume_GallonUS, 3785.411784 },
                    { Volume_GallonUK, 4546.09 },
                    { Volume_Liter, 1000 },
                    { Volume_TeaspoonUS, 4.92892159375 },
                    { Volume_TablespoonUS, 14.78676478125 },
                    { Volume_CubicCentimeter, 1 },
                    { Volume_CubicYard, 764554.857984 },
                    { Volume_CubicMeter, 1000000 },
                    { Volume_Milliliter, 1 },
                    { Volume_CubicInch, 16.387064 },
                    { Volume_CubicFoot, 28316.846592 },
                    { Volume_FluidOunceUS, 29.5735295625 },
                    { Volume_FluidOunceUK, 28.4130625 },
                    { Volume_TeaspoonUK, 5.91938802083333333333 },
                    { Volume_TablespoonUK, 17.7581640625 },
                    { Volume_CoffeeCup, 236.5882 },
                    { Volume_Bathtub, 378541.2 },
                    { Volume_SwimmingPool, 3750000000 },
                },
                [ViewMode.Weight] = new Dictionary<int, double>
                {
                    { Weight_Kilogram, 1 },
                    { Weight_Hectogram, 0.1 },
                    { Weight_Decagram, 0.01 },
                    { Weight_Gram, 0.001 },
                    { Weight_Pound, 0.45359237 },
                    { Weight_Ounce, 0.028349523125 },
                    { Weight_Milligram, 0.000001 },
                    { Weight_Centigram, 0.00001 },
                    { Weight_Decigram, 0.0001 },
                    { Weight_LongTon, 1016.0469088 },
                    { Weight_Tonne, 1000 },
                    { Weight_Stone, 6.35029318 },
                    { Weight_Carat, 0.0002 },
                    { Weight_ShortTon, 907.18474 },
                    { Weight_Snowflake, 0.000002 },
                    { Weight_SoccerBall, 0.4325 },
                    { Weight_Elephant, 4000 },
                    { Weight_Whale, 90000 },
                },
                [ViewMode.Speed] = new Dictionary<int, double>
                {
                    { Speed_CentimetersPerSecond, 1 },
                    { Speed_FeetPerSecond, 30.48 },
                    { Speed_KilometersPerHour, 27.777777777777777777778 },
                    { Speed_Knot, 51.44 },
                    { Speed_Mach, 34030 },
                    { Speed_MetersPerSecond, 100 },
                    { Speed_MilesPerHour, 44.7 },
                    { Speed_Turtle, 8.94 },
                    { Speed_Horse, 2011.5 },
                    { Speed_Jet, 24585 },
                },
                [ViewMode.Angle] = new Dictionary<int, double>
                {
                    { Angle_Degree, 1 },
                    { Angle_Radian, 57.29577951308233 },
                    { Angle_Gradian, 0.9 },
                },
                [ViewMode.Pressure] = new Dictionary<int, double>
                {
                    { Pressure_Atmosphere, 1 },
                    { Pressure_Bar, 0.9869232667160128 },
                    { Pressure_KiloPascal, 0.0098692326671601 },
                    { Pressure_MillimeterOfMercury, 0.0013155687145324 },
                    { Pressure_Pascal, 9.869232667160128e-6 },
                    { Pressure_PSI, 0.068045961016531 },
                },
            };

            // Build explicit conversion data (Temperature)
            var explicitConversions = new Dictionary<int, Dictionary<int, (double ratio, double offset, bool offsetFirst)>>
            {
                [Temperature_DegreesCelsius] = new Dictionary<int, (double, double, bool)>
                {
                    { Temperature_DegreesCelsius, (1, 0, false) },
                    { Temperature_DegreesFahrenheit, (1.8, 32, false) },
                    { Temperature_Kelvin, (1, 273.15, false) },
                },
                [Temperature_DegreesFahrenheit] = new Dictionary<int, (double, double, bool)>
                {
                    { Temperature_DegreesCelsius, (0.55555555555555555555555555555556, -32, true) },
                    { Temperature_DegreesFahrenheit, (1, 0, false) },
                    { Temperature_Kelvin, (0.55555555555555555555555555555556, 459.67, true) },
                },
                [Temperature_Kelvin] = new Dictionary<int, (double, double, bool)>
                {
                    { Temperature_DegreesCelsius, (1, -273.15, true) },
                    { Temperature_DegreesFahrenheit, (1.8, -459.67, false) },
                    { Temperature_Kelvin, (1, 0, false) },
                },
            };

            // Build unit lookup by ID
            var allUnitsById = new Dictionary<int, UnitWrapper>();
            foreach (var kvp in _unitsByCategory)
            {
                foreach (var unit in kvp.Value)
                {
                    allUnitsById[unit.Id] = unit;
                }
            }

            // Build the ratio map: for each unit, compute conversions to all other units in the same category
            _ratiosByUnit = new Dictionary<int, UnitConversionEntry[]>();

            foreach (var cat in _categories)
            {
                ViewMode categoryViewMode = NavCategoryStates.Deserialize(cat.Id);
                if (categoryViewMode == ViewMode.Currency || categoryViewMode == ViewMode.None)
                {
                    continue;
                }

                if (!_unitsByCategory.TryGetValue(cat.Id, out var units) || units.Length == 0)
                {
                    continue;
                }

                foreach (var sourceUnit in units)
                {
                    var entries = new List<UnitConversionEntry>();

                    if (explicitConversions.TryGetValue(sourceUnit.Id, out var explicitMap))
                    {
                        // Temperature: use explicit conversion data
                        foreach (var kvp in explicitMap)
                        {
                            if (allUnitsById.TryGetValue(kvp.Key, out var targetUnit))
                            {
                                entries.Add(new UnitConversionEntry
                                {
                                    Unit = targetUnit,
                                    Ratio = kvp.Value.ratio,
                                    Offset = kvp.Value.offset,
                                    OffsetFirst = kvp.Value.offsetFirst
                                });
                            }
                        }
                    }
                    else if (conversionFactors.TryGetValue(categoryViewMode, out var categoryFactors))
                    {
                        // Standard factor-based conversion: ratio = sourceFactor / targetFactor
                        if (!categoryFactors.TryGetValue(sourceUnit.Id, out double sourceFactor))
                        {
                            continue;
                        }

                        foreach (var targetUnit in units)
                        {
                            if (categoryFactors.TryGetValue(targetUnit.Id, out double targetFactor) && targetFactor > 0)
                            {
                                entries.Add(new UnitConversionEntry
                                {
                                    Unit = targetUnit,
                                    Ratio = sourceFactor / targetFactor,
                                    Offset = 0,
                                    OffsetFirst = false
                                });
                            }
                        }
                    }

                    _ratiosByUnit[sourceUnit.Id] = entries.ToArray();
                }
            }
        }

        protected override CategoryWrapper[] GetOrderedCategories()
        {
            return _categories ?? Array.Empty<CategoryWrapper>();
        }

        protected override UnitWrapper[] GetOrderedUnits(CategoryWrapper category)
        {
            if (_unitsByCategory != null && _unitsByCategory.TryGetValue(category.Id, out var units))
            {
                return units;
            }
            return Array.Empty<UnitWrapper>();
        }

        protected override UnitConversionEntry[] LoadOrderedRatios(UnitWrapper unit)
        {
            if (_ratiosByUnit != null && _ratiosByUnit.TryGetValue(unit.Id, out var ratios))
            {
                return ratios;
            }
            return Array.Empty<UnitConversionEntry>();
        }

        protected override bool SupportsCategory(CategoryWrapper target)
        {
            return target.Id != NavCategoryStates.Serialize(ViewMode.Currency);
        }
    }
}
