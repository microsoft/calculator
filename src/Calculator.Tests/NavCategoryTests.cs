// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel.Common;
using System.Collections.Generic;

namespace Calculator.Tests
{
    [TestClass]
    public class NavCategoryTests
    {
        [TestMethod]
        public void IsCalculatorViewMode()
        {
            Assert.IsTrue(NavCategory.IsCalculatorViewMode(ViewMode.Standard));
            Assert.IsTrue(NavCategory.IsCalculatorViewMode(ViewMode.Scientific));
            Assert.IsTrue(NavCategory.IsCalculatorViewMode(ViewMode.Programmer));

            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Date));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Graphing));

            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Currency));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Volume));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Length));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Weight));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Temperature));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Energy));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Area));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Speed));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Time));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Power));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Data));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Pressure));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Angle));
        }

        [TestMethod]
        public void IsDateCalculatorViewMode()
        {
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Standard));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Scientific));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Programmer));

            Assert.IsTrue(NavCategory.IsDateCalculatorViewMode(ViewMode.Date));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Graphing));

            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Currency));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Volume));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Length));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Weight));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Temperature));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Energy));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Area));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Speed));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Time));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Power));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Data));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Pressure));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Angle));
        }

        [TestMethod]
        public void IsConverterViewMode()
        {
            Assert.IsFalse(NavCategory.IsConverterViewMode(ViewMode.Standard));
            Assert.IsFalse(NavCategory.IsConverterViewMode(ViewMode.Scientific));
            Assert.IsFalse(NavCategory.IsConverterViewMode(ViewMode.Programmer));
            Assert.IsFalse(NavCategory.IsConverterViewMode(ViewMode.Date));
            Assert.IsFalse(NavCategory.IsConverterViewMode(ViewMode.Graphing));

            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Currency));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Volume));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Length));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Weight));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Temperature));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Energy));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Area));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Speed));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Time));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Power));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Data));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Pressure));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Angle));
        }
    }

    [TestClass]
    public class NavCategoryGroupTests
    {
        [TestMethod]
        public void CreateNavCategoryGroup()
        {
            var menuOptions = NavCategoryStates.CreateMenuOptions();

            Assert.AreEqual(2, menuOptions.Count);

            var calculatorGroup = (NavCategoryGroup)menuOptions[0];
            Assert.AreEqual(CategoryGroupType.Calculator, calculatorGroup.GroupType);

            var calculatorCategories = calculatorGroup.Categories;
            ValidateNavCategory(calculatorCategories, 0, ViewMode.Standard);
            ValidateNavCategory(calculatorCategories, 1, ViewMode.Scientific);
            ValidateNavCategory(calculatorCategories, 2, ViewMode.Graphing);
            ValidateNavCategory(calculatorCategories, 3, ViewMode.Programmer);
            ValidateNavCategory(calculatorCategories, 4, ViewMode.Date);
            Assert.AreEqual(5, calculatorCategories.Count);

            var converterGroup = (NavCategoryGroup)menuOptions[1];
            Assert.AreEqual(CategoryGroupType.Converter, converterGroup.GroupType);

            var converterCategories = converterGroup.Categories;
            Assert.AreEqual(13, converterCategories.Count);
            ValidateNavCategory(converterCategories, 0, ViewMode.Currency);
            ValidateNavCategory(converterCategories, 1, ViewMode.Volume);
            ValidateNavCategory(converterCategories, 2, ViewMode.Length);
            ValidateNavCategory(converterCategories, 3, ViewMode.Weight);
            ValidateNavCategory(converterCategories, 4, ViewMode.Temperature);
            ValidateNavCategory(converterCategories, 5, ViewMode.Energy);
            ValidateNavCategory(converterCategories, 6, ViewMode.Area);
            ValidateNavCategory(converterCategories, 7, ViewMode.Speed);
            ValidateNavCategory(converterCategories, 8, ViewMode.Time);
            ValidateNavCategory(converterCategories, 9, ViewMode.Power);
            ValidateNavCategory(converterCategories, 10, ViewMode.Data);
            ValidateNavCategory(converterCategories, 11, ViewMode.Pressure);
            ValidateNavCategory(converterCategories, 12, ViewMode.Angle);
        }

        private void ValidateNavCategory(IList<NavCategory> categories, int index, ViewMode expectedMode)
        {
            Assert.IsTrue(categories.Count > 0);
            Assert.IsTrue(categories.Count > index);

            var category = categories[index];
            Assert.AreEqual(expectedMode, category.ViewMode);
            Assert.AreEqual(expectedMode != ViewMode.Graphing, category.IsEnabled);
        }
    }

    [TestClass]
    public class NavCategoryStatesTests
    {
        private static readonly ViewMode[] OrderedModes =
        {
            ViewMode.Standard, ViewMode.Scientific, ViewMode.Graphing, ViewMode.Programmer, ViewMode.Date, ViewMode.Currency,
            ViewMode.Volume, ViewMode.Length, ViewMode.Weight, ViewMode.Temperature, ViewMode.Energy, ViewMode.Area,
            ViewMode.Speed, ViewMode.Time, ViewMode.Power, ViewMode.Data, ViewMode.Pressure, ViewMode.Angle
        };

        [TestMethod]
        public void Serialize()
        {
            // These values should NEVER change. We are validating the unique ID for each mode.
            Assert.AreEqual(0, NavCategoryStates.Serialize(ViewMode.Standard));
            Assert.AreEqual(1, NavCategoryStates.Serialize(ViewMode.Scientific));
            Assert.AreEqual(2, NavCategoryStates.Serialize(ViewMode.Programmer));
            Assert.AreEqual(3, NavCategoryStates.Serialize(ViewMode.Date));
            Assert.AreEqual(16, NavCategoryStates.Serialize(ViewMode.Currency));
            Assert.AreEqual(4, NavCategoryStates.Serialize(ViewMode.Volume));
            Assert.AreEqual(5, NavCategoryStates.Serialize(ViewMode.Length));
            Assert.AreEqual(6, NavCategoryStates.Serialize(ViewMode.Weight));
            Assert.AreEqual(7, NavCategoryStates.Serialize(ViewMode.Temperature));
            Assert.AreEqual(8, NavCategoryStates.Serialize(ViewMode.Energy));
            Assert.AreEqual(9, NavCategoryStates.Serialize(ViewMode.Area));
            Assert.AreEqual(10, NavCategoryStates.Serialize(ViewMode.Speed));
            Assert.AreEqual(11, NavCategoryStates.Serialize(ViewMode.Time));
            Assert.AreEqual(12, NavCategoryStates.Serialize(ViewMode.Power));
            Assert.AreEqual(13, NavCategoryStates.Serialize(ViewMode.Data));
            Assert.AreEqual(14, NavCategoryStates.Serialize(ViewMode.Pressure));
            Assert.AreEqual(15, NavCategoryStates.Serialize(ViewMode.Angle));

            Assert.AreEqual(-1, NavCategoryStates.Serialize(ViewMode.None));
        }

        [TestMethod]
        public void Deserialize_AllValid()
        {
            // These values should NEVER change.
            Assert.AreEqual(ViewMode.Standard, NavCategoryStates.Deserialize(0));
            Assert.AreEqual(ViewMode.Scientific, NavCategoryStates.Deserialize(1));
            Assert.AreEqual(ViewMode.Programmer, NavCategoryStates.Deserialize(2));
            Assert.AreEqual(ViewMode.Date, NavCategoryStates.Deserialize(3));
            Assert.AreEqual(ViewMode.Currency, NavCategoryStates.Deserialize(16));
            Assert.AreEqual(ViewMode.Volume, NavCategoryStates.Deserialize(4));
            Assert.AreEqual(ViewMode.Length, NavCategoryStates.Deserialize(5));
            Assert.AreEqual(ViewMode.Weight, NavCategoryStates.Deserialize(6));
            Assert.AreEqual(ViewMode.Temperature, NavCategoryStates.Deserialize(7));
            Assert.AreEqual(ViewMode.Energy, NavCategoryStates.Deserialize(8));
            Assert.AreEqual(ViewMode.Area, NavCategoryStates.Deserialize(9));
            Assert.AreEqual(ViewMode.Speed, NavCategoryStates.Deserialize(10));
            Assert.AreEqual(ViewMode.Time, NavCategoryStates.Deserialize(11));
            Assert.AreEqual(ViewMode.Power, NavCategoryStates.Deserialize(12));
            Assert.AreEqual(ViewMode.Data, NavCategoryStates.Deserialize(13));
            Assert.AreEqual(ViewMode.Pressure, NavCategoryStates.Deserialize(14));
            Assert.AreEqual(ViewMode.Angle, NavCategoryStates.Deserialize(15));
        }

        [TestMethod]
        public void Deserialize_AllInvalid()
        {
            Assert.AreEqual(ViewMode.None, NavCategoryStates.Deserialize(null));
            Assert.AreEqual(ViewMode.None, NavCategoryStates.Deserialize("fail"));

            // Boundary testing
            Assert.AreEqual(ViewMode.None, NavCategoryStates.Deserialize(-1));
            Assert.AreEqual(ViewMode.None, NavCategoryStates.Deserialize(18));
        }

        [TestMethod]
        public void IsValidViewMode_AllValid()
        {
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Standard));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Scientific));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Programmer));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Date));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Graphing));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Currency));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Volume));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Length));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Weight));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Temperature));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Energy));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Area));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Speed));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Time));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Power));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Data));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Pressure));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Angle));
        }

        [TestMethod]
        public void IsValidViewMode_AllInvalid()
        {
            Assert.IsFalse(NavCategoryStates.IsValidViewMode(ViewMode.None));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode((ViewMode)17));
            Assert.IsFalse(NavCategoryStates.IsValidViewMode((ViewMode)18));

            // Also verify the lower bound
            Assert.IsTrue(NavCategoryStates.IsValidViewMode((ViewMode)0));
            Assert.IsFalse(NavCategoryStates.IsValidViewMode((ViewMode)(-1)));
        }

        [TestMethod]
        public void GetFriendlyName()
        {
            Assert.AreEqual("Standard", NavCategoryStates.GetFriendlyName(ViewMode.Standard));
            Assert.AreEqual("Scientific", NavCategoryStates.GetFriendlyName(ViewMode.Scientific));
            Assert.AreEqual("Programmer", NavCategoryStates.GetFriendlyName(ViewMode.Programmer));
            Assert.AreEqual("Date", NavCategoryStates.GetFriendlyName(ViewMode.Date));
            Assert.AreEqual("Graphing", NavCategoryStates.GetFriendlyName(ViewMode.Graphing));
            Assert.AreEqual("Currency", NavCategoryStates.GetFriendlyName(ViewMode.Currency));
            Assert.AreEqual("Volume", NavCategoryStates.GetFriendlyName(ViewMode.Volume));
            Assert.AreEqual("Length", NavCategoryStates.GetFriendlyName(ViewMode.Length));
            Assert.AreEqual("Weight and Mass", NavCategoryStates.GetFriendlyName(ViewMode.Weight));
            Assert.AreEqual("Temperature", NavCategoryStates.GetFriendlyName(ViewMode.Temperature));
            Assert.AreEqual("Energy", NavCategoryStates.GetFriendlyName(ViewMode.Energy));
            Assert.AreEqual("Area", NavCategoryStates.GetFriendlyName(ViewMode.Area));
            Assert.AreEqual("Speed", NavCategoryStates.GetFriendlyName(ViewMode.Speed));
            Assert.AreEqual("Time", NavCategoryStates.GetFriendlyName(ViewMode.Time));
            Assert.AreEqual("Power", NavCategoryStates.GetFriendlyName(ViewMode.Power));
            Assert.AreEqual("Data", NavCategoryStates.GetFriendlyName(ViewMode.Data));
            Assert.AreEqual("Pressure", NavCategoryStates.GetFriendlyName(ViewMode.Pressure));
            Assert.AreEqual("Angle", NavCategoryStates.GetFriendlyName(ViewMode.Angle));

            Assert.AreEqual("None", NavCategoryStates.GetFriendlyName(ViewMode.None));
        }

        [TestMethod]
        public void GetGroupType()
        {
            Assert.AreEqual(CategoryGroupType.Calculator, NavCategoryStates.GetGroupType(ViewMode.Standard));
            Assert.AreEqual(CategoryGroupType.Calculator, NavCategoryStates.GetGroupType(ViewMode.Scientific));
            Assert.AreEqual(CategoryGroupType.Calculator, NavCategoryStates.GetGroupType(ViewMode.Programmer));
            Assert.AreEqual(CategoryGroupType.Calculator, NavCategoryStates.GetGroupType(ViewMode.Date));
            Assert.AreEqual(CategoryGroupType.Calculator, NavCategoryStates.GetGroupType(ViewMode.Graphing));

            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Currency));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Volume));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Length));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Weight));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Temperature));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Energy));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Area));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Speed));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Time));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Power));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Data));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Pressure));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Angle));
        }

        [TestMethod]
        public void GetIndex()
        {
            // Index is the 0-based ordering of modes
            for (int index = 0; index < OrderedModes.Length; index++)
            {
                ViewMode mode = OrderedModes[index];
                Assert.AreEqual(index, NavCategoryStates.GetIndex(mode));
            }

            Assert.AreEqual(-1, NavCategoryStates.GetIndex(ViewMode.None));
        }

        [TestMethod]
        public void GetPosition()
        {
            // Position is the 1-based ordering of modes
            for (int pos = 1; pos <= OrderedModes.Length; pos++)
            {
                ViewMode mode = OrderedModes[pos - 1];
                Assert.AreEqual(pos, NavCategoryStates.GetPosition(mode));
            }

            Assert.AreEqual(-1, NavCategoryStates.GetPosition(ViewMode.None));
        }

        [TestMethod]
        public void GetIndex_GetPosition_Relationship()
        {
            // Index should be 1 less than Position.
            Assert.AreEqual(NavCategoryStates.GetIndex(ViewMode.Standard) + 1, NavCategoryStates.GetPosition(ViewMode.Standard));
            Assert.AreEqual(NavCategoryStates.GetPosition(ViewMode.Volume) - 1, NavCategoryStates.GetIndex(ViewMode.Volume));
        }

        [TestMethod]
        public void GetIndexInGroup()
        {
            Assert.AreEqual(0, NavCategoryStates.GetIndexInGroup(ViewMode.Standard, CategoryGroupType.Calculator));
            Assert.AreEqual(1, NavCategoryStates.GetIndexInGroup(ViewMode.Scientific, CategoryGroupType.Calculator));
            Assert.AreEqual(2, NavCategoryStates.GetIndexInGroup(ViewMode.Graphing, CategoryGroupType.Calculator));
            Assert.AreEqual(3, NavCategoryStates.GetIndexInGroup(ViewMode.Programmer, CategoryGroupType.Calculator));
            Assert.AreEqual(4, NavCategoryStates.GetIndexInGroup(ViewMode.Date, CategoryGroupType.Calculator));
            Assert.AreEqual(0, NavCategoryStates.GetIndexInGroup(ViewMode.Currency, CategoryGroupType.Converter));
            Assert.AreEqual(1, NavCategoryStates.GetIndexInGroup(ViewMode.Volume, CategoryGroupType.Converter));
            Assert.AreEqual(2, NavCategoryStates.GetIndexInGroup(ViewMode.Length, CategoryGroupType.Converter));
            Assert.AreEqual(3, NavCategoryStates.GetIndexInGroup(ViewMode.Weight, CategoryGroupType.Converter));
            Assert.AreEqual(4, NavCategoryStates.GetIndexInGroup(ViewMode.Temperature, CategoryGroupType.Converter));
            Assert.AreEqual(5, NavCategoryStates.GetIndexInGroup(ViewMode.Energy, CategoryGroupType.Converter));
            Assert.AreEqual(6, NavCategoryStates.GetIndexInGroup(ViewMode.Area, CategoryGroupType.Converter));
            Assert.AreEqual(7, NavCategoryStates.GetIndexInGroup(ViewMode.Speed, CategoryGroupType.Converter));
            Assert.AreEqual(8, NavCategoryStates.GetIndexInGroup(ViewMode.Time, CategoryGroupType.Converter));
            Assert.AreEqual(9, NavCategoryStates.GetIndexInGroup(ViewMode.Power, CategoryGroupType.Converter));
            Assert.AreEqual(10, NavCategoryStates.GetIndexInGroup(ViewMode.Data, CategoryGroupType.Converter));
            Assert.AreEqual(11, NavCategoryStates.GetIndexInGroup(ViewMode.Pressure, CategoryGroupType.Converter));
            Assert.AreEqual(12, NavCategoryStates.GetIndexInGroup(ViewMode.Angle, CategoryGroupType.Converter));

            Assert.AreEqual(-1, NavCategoryStates.GetIndexInGroup(ViewMode.None, CategoryGroupType.Calculator));
            Assert.AreEqual(-1, NavCategoryStates.GetIndexInGroup(ViewMode.None, CategoryGroupType.Converter));
        }

        [TestMethod]
        public void GetViewModeForVirtualKey()
        {
            Assert.AreEqual(ViewMode.Standard, NavCategoryStates.GetViewModeForVirtualKey(MyVirtualKey.Number1));
            Assert.AreEqual(ViewMode.Scientific, NavCategoryStates.GetViewModeForVirtualKey(MyVirtualKey.Number2));
            Assert.AreEqual(ViewMode.Graphing, NavCategoryStates.GetViewModeForVirtualKey(MyVirtualKey.Number3));
            Assert.AreEqual(ViewMode.Programmer, NavCategoryStates.GetViewModeForVirtualKey(MyVirtualKey.Number4));
            Assert.AreEqual(ViewMode.Date, NavCategoryStates.GetViewModeForVirtualKey(MyVirtualKey.Number5));
        }

        [TestMethod]
        public void IsViewModeEnabled_ShouldBeTrue_ExceptGraphingMode()
        {
            foreach (var mode in OrderedModes)
            {
                if (mode != ViewMode.Graphing)
                {
                    Assert.IsTrue(NavCategoryStates.IsViewModeEnabled(mode));
                }
            }
        }

        [TestMethod]
        public void GraphingModeIsEnabled_ShouldBeTrue_WhenNullUserAssigned()
        {
            NavCategoryStates.SetCurrentUser("null-user"); // make sure User.GetFromId() returns null
            Assert.IsTrue(NavCategoryStates.IsViewModeEnabled(ViewMode.Graphing));
        }
    }
}
