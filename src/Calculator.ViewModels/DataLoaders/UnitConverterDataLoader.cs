// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;
using CalculatorApp.ViewModel.Common;

namespace CalculatorApp.ViewModel.DataLoaders
{
    /// <summary>
    /// Represents a unit with ordering information for display.
    /// </summary>
    public class OrderedUnit
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public string Abbreviation { get; set; }
        public int Order { get; set; }
        public bool IsConversionSource { get; set; }
        public bool IsConversionTarget { get; set; }
        public bool IsWhimsical { get; set; }

        public OrderedUnit()
        {
        }

        public OrderedUnit(int id, string name, string abbreviation, int order,
            bool isConversionSource = false, bool isConversionTarget = false, bool isWhimsical = false)
        {
            Id = id;
            Name = name;
            Abbreviation = abbreviation;
            Order = order;
            IsConversionSource = isConversionSource;
            IsConversionTarget = isConversionTarget;
            IsWhimsical = isWhimsical;
        }
    }

    /// <summary>
    /// Represents unit conversion data for a specific unit.
    /// </summary>
    public class UnitConversionData
    {
        public ViewMode CategoryId { get; set; }
        public int UnitId { get; set; }
        public double Factor { get; set; }
    }

    /// <summary>
    /// Represents explicit unit-to-unit conversion data.
    /// </summary>
    public class ExplicitUnitConversionData
    {
        public ViewMode CategoryId { get; set; }
        public int ParentUnitId { get; set; }
        public int UnitId { get; set; }
        public double Ratio { get; set; }
        public double Offset { get; set; }
        public bool OffsetFirst { get; set; }

        public ExplicitUnitConversionData()
        {
        }

        public ExplicitUnitConversionData(ViewMode categoryId, int parentUnitId, int unitId,
            double ratio, double offset, bool offsetFirst = false)
        {
            CategoryId = categoryId;
            ParentUnitId = parentUnitId;
            UnitId = unitId;
            Ratio = ratio;
            Offset = offset;
            OffsetFirst = offsetFirst;
        }
    }

    /// <summary>
    /// Loads unit converter data from resource files and provides conversion data
    /// for all non-currency unit categories.
    /// </summary>
    public sealed class UnitConverterDataLoader
    {
        private readonly string _currentRegionCode;
        private List<CategoryData> _categoryList;
        private Dictionary<int, List<OrderedUnit>> _categoryIdToUnitsMap;
        private Dictionary<int, Dictionary<int, ConversionData>> _ratioMap;

        public UnitConverterDataLoader(string regionCode)
        {
            _currentRegionCode = regionCode ?? "US";
        }

        public UnitConverterDataLoader(Windows.Globalization.GeographicRegion region)
            : this(region?.CodeTwoLetter ?? "US")
        {
        }

        public void LoadData()
        {
            _categoryList = new List<CategoryData>();
            _categoryIdToUnitsMap = new Dictionary<int, List<OrderedUnit>>();
            _ratioMap = new Dictionary<int, Dictionary<int, ConversionData>>();

            GetCategories(_categoryList);
            GetUnits(_categoryIdToUnitsMap);
            var conversionMap = new Dictionary<ViewMode, Dictionary<int, double>>();
            GetConversionData(conversionMap);
            var explicitMap = new Dictionary<int, Dictionary<int, ConversionData>>();
            GetExplicitConversionData(explicitMap);

            // Build the ratio map from conversion data
            foreach (var catEntry in _categoryIdToUnitsMap)
            {
                _ratioMap[catEntry.Key] = new Dictionary<int, ConversionData>();
            }
        }

        public List<CategoryData> GetOrderedCategories()
        {
            return _categoryList ?? new List<CategoryData>();
        }

        public List<OrderedUnit> GetOrderedUnits(int categoryId)
        {
            if (_categoryIdToUnitsMap != null && _categoryIdToUnitsMap.TryGetValue(categoryId, out var units))
            {
                return units;
            }
            return new List<OrderedUnit>();
        }

        public Dictionary<int, ConversionData> LoadOrderedRatios(int unitId)
        {
            if (_ratioMap != null && _ratioMap.TryGetValue(unitId, out var ratios))
            {
                return ratios;
            }
            return new Dictionary<int, ConversionData>();
        }

        public bool SupportsCategory(int categoryId)
        {
            // Currency is handled by CurrencyDataLoader, not this loader
            return categoryId != NavCategoryStates.Serialize(ViewMode.Currency);
        }

        private void GetCategories(List<CategoryData> categoriesList)
        {
            // Populate categories from NavCategoryStates
            // Each supported converter mode becomes a category
            var converterModes = new[]
            {
                ViewMode.Volume, ViewMode.Length, ViewMode.Weight,
                ViewMode.Temperature, ViewMode.Energy, ViewMode.Area,
                ViewMode.Speed, ViewMode.Time, ViewMode.Power,
                ViewMode.Data, ViewMode.Pressure, ViewMode.Angle
            };

            foreach (var mode in converterModes)
            {
                categoriesList.Add(new CategoryData
                {
                    Id = NavCategoryStates.Serialize(mode),
                    Name = NavCategoryStates.GetNameResourceKey(mode),
                    SupportsNegative = mode == ViewMode.Temperature
                });
            }
        }

        private void GetUnits(Dictionary<int, List<OrderedUnit>> unitMap)
        {
            // Populate unit lists for each category from resource data
            // This would normally read from compiled resources
        }

        private void GetConversionData(Dictionary<ViewMode, Dictionary<int, double>> categoryToUnitConversionMap)
        {
            // Load conversion factors for each unit
        }

        private void GetExplicitConversionData(Dictionary<int, Dictionary<int, ConversionData>> unitToUnitConversionList)
        {
            // Load explicit unit-to-unit conversions (e.g., temperature)
        }

        private string GetLocalizedStringName(string stringId)
        {
            return AppResourceProvider.GetInstance().GetResourceString(stringId);
        }
    }
}
