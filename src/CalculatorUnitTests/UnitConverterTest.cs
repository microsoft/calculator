// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using CalcManager.UnitConversionManager;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace UnitConverterUnitTests
{
    public class TestUnitConverterConfigLoader : IConverterDataLoader
    {
        private List<Category> m_categories = new List<Category>();
        private Dictionary<int, List<Unit>> m_units = new Dictionary<int, List<Unit>>();
        private Dictionary<Unit, Dictionary<Unit, ConversionData>> m_ratioMaps = new Dictionary<Unit, Dictionary<Unit, ConversionData>>();
        public uint m_loadDataCallCount;

        public TestUnitConverterConfigLoader()
        {
            var c1 = new Category(1, "Length", true);
            var c2 = new Category(2, "Weight", false);
            m_categories.Add(c1);
            m_categories.Add(c2);

            var u1 = new Unit(1, "Inches", "In", true, true, false);
            var u2 = new Unit(2, "Feet", "Ft", false, false, false);
            var u3 = new Unit(3, "Pounds", "Lb", true, true, false);
            var u4 = new Unit(4, "Kilograms", "Kg", false, false, false);

            var c1units = new List<Unit> { u1, u2 };
            var c2units = new List<Unit> { u3, u4 };

            m_units[c1.Id] = c1units;
            m_units[c2.Id] = c2units;

            var unit1Map = new Dictionary<Unit, ConversionData>();
            var unit2Map = new Dictionary<Unit, ConversionData>();
            var unit3Map = new Dictionary<Unit, ConversionData>();
            var unit4Map = new Dictionary<Unit, ConversionData>();

            var conversion1 = new ConversionData(1.0, 0, false);
            var conversion2 = new ConversionData(0.08333333333333333333333333333333, 0, false);
            var conversion3 = new ConversionData(12.0, 0, false);
            var conversion4 = new ConversionData(0.453592, 0, false);
            var conversion5 = new ConversionData(2.20462, 0, false);

            unit1Map[u1] = conversion1;
            unit1Map[u2] = conversion2;

            unit2Map[u1] = conversion3;
            unit2Map[u2] = conversion1;

            unit3Map[u3] = conversion1;
            unit3Map[u4] = conversion4;

            unit4Map[u3] = conversion5;
            unit4Map[u4] = conversion1;

            m_ratioMaps[u1] = unit1Map;
            m_ratioMaps[u2] = unit2Map;
            m_ratioMaps[u3] = unit3Map;
            m_ratioMaps[u4] = unit4Map;
        }

        public void LoadData()
        {
            m_loadDataCallCount++;
        }

        public List<Category> GetOrderedCategories()
        {
            return m_categories;
        }

        public List<Unit> GetOrderedUnits(Category category)
        {
            return m_units.TryGetValue(category.Id, out var list) ? list : new List<Unit>();
        }

        public Dictionary<Unit, ConversionData> LoadOrderedRatios(Unit u)
        {
            return m_ratioMaps.TryGetValue(u, out var map) ? map : new Dictionary<Unit, ConversionData>();
        }

        public bool SupportsCategory(Category target)
        {
            return true;
        }
    }

    public class TestUnitConverterVMCallback : IUnitConverterVMCallback
    {
        private string m_lastFrom;
        private string m_lastTo;
        private List<Tuple<string, Unit>> m_lastSuggested = new List<Tuple<string, Unit>>();
        private int m_maxDigitsReachedCallCount;

        public void Reset()
        {
            m_maxDigitsReachedCallCount = 0;
        }

        public void DisplayCallback(string from, string to)
        {
            m_lastFrom = from;
            m_lastTo = to;
        }

        public void SuggestedValueCallback(List<Tuple<string, Unit>> suggestedValues)
        {
            m_lastSuggested = suggestedValues ?? new List<Tuple<string, Unit>>();
        }

        public void MaxDigitsReached()
        {
            m_maxDigitsReachedCallCount++;
        }

        public int GetMaxDigitsReachedCallCount() => m_maxDigitsReachedCallCount;

        public bool CheckDisplayValues(string from, string to)
        {
            return from == m_lastFrom && to == m_lastTo;
        }

        public bool CheckSuggestedValues(List<Tuple<string, Unit>> suggested)
        {
            if (suggested.Count != m_lastSuggested.Count)
            {
                return false;
            }
            for (int i = 0; i < suggested.Count; i++)
            {
                if (!suggested[i].Equals(m_lastSuggested[i]))
                {
                    return false;
                }
            }
            return true;
        }
    }

    [TestClass]
    public class UnitConverterTest
    {
        private static UnitConverter s_unitConverter;
        private static TestUnitConverterConfigLoader s_xmlLoader;
        private static TestUnitConverterVMCallback s_testVMCallback;
        private static Category s_testLength;
        private static Category s_testWeight;
        private static Unit s_testInches;
        private static Unit s_testFeet;
        private static Unit s_testPounds;
        private static Unit s_testKilograms;

        [ClassInitialize]
        public static void CommonSetup(TestContext context)
        {
            s_testVMCallback = new TestUnitConverterVMCallback();
            s_xmlLoader = new TestUnitConverterConfigLoader();
            s_unitConverter = new UnitConverter(s_xmlLoader);
            s_unitConverter.SetViewModelCallback(s_testVMCallback);
            s_testLength = new Category(1, "Length", true);
            s_testWeight = new Category(2, "Weight", false);
            s_testInches = new Unit(1, "Inches", "In", true, true, false);
            s_testFeet = new Unit(2, "Feet", "Ft", false, false, false);
            s_testPounds = new Unit(3, "Pounds", "Lb", true, true, false);
            s_testKilograms = new Unit(4, "Kilograms", "Kg", false, false, false);
        }

        [TestCleanup]
        public void Cleanup()
        {
            s_unitConverter.SendCommand(Command.Reset);
            s_testVMCallback.Reset();
        }

        private static void ExecuteCommands(IEnumerable<Command> commands)
        {
            foreach (var cmd in commands)
            {
                if (cmd == Command.None) break;
                s_unitConverter.SendCommand(cmd);
            }
        }

        [TestMethod]
        public void UnitConverterTestInit()
        {
            Assert.AreEqual((uint)0, s_xmlLoader.m_loadDataCallCount);
            s_unitConverter.Initialize();
            Assert.AreEqual((uint)1, s_xmlLoader.m_loadDataCallCount);
        }

        [TestMethod]
        public void UnitConverterTestBasic()
        {
            var test1 = new List<Tuple<string, Unit>> { Tuple.Create("0.25", s_testFeet) };
            var test2 = new List<Tuple<string, Unit>> { Tuple.Create("2.5", s_testFeet) };

            s_unitConverter.SendCommand(Command.Three);
            Assert.IsTrue(s_testVMCallback.CheckDisplayValues("3", "3"));
            Assert.IsTrue(s_testVMCallback.CheckSuggestedValues(test1));
            s_unitConverter.SendCommand(Command.Zero);
            Assert.IsTrue(s_testVMCallback.CheckDisplayValues("30", "30"));
            Assert.IsTrue(s_testVMCallback.CheckSuggestedValues(test2));
            s_unitConverter.SendCommand(Command.Decimal);
            Assert.IsTrue(s_testVMCallback.CheckDisplayValues("30.", "30"));
            Assert.IsTrue(s_testVMCallback.CheckSuggestedValues(test2));
            s_unitConverter.SendCommand(Command.Zero);
            Assert.IsTrue(s_testVMCallback.CheckDisplayValues("30.0", "30"));
            Assert.IsTrue(s_testVMCallback.CheckSuggestedValues(test2));
        }

        [TestMethod]
        public void UnitConverterTestBackspaceBasic()
        {
            s_unitConverter.SendCommand(Command.Two);
            s_unitConverter.SendCommand(Command.Zero);
            s_unitConverter.SendCommand(Command.Decimal);
            s_unitConverter.SendCommand(Command.Four);
            s_unitConverter.SendCommand(Command.Three);
            s_unitConverter.SendCommand(Command.Backspace);

            Assert.IsTrue(s_testVMCallback.CheckDisplayValues("20.4", "20.4"));
            s_unitConverter.SendCommand(Command.Backspace);
            Assert.IsTrue(s_testVMCallback.CheckDisplayValues("20.", "20"));
            s_unitConverter.SendCommand(Command.Backspace);
            Assert.IsTrue(s_testVMCallback.CheckDisplayValues("20", "20"));
            s_unitConverter.SendCommand(Command.Backspace);
            Assert.IsTrue(s_testVMCallback.CheckDisplayValues("2", "2"));
            s_unitConverter.SendCommand(Command.Backspace);
            Assert.IsTrue(s_testVMCallback.CheckDisplayValues("0", "0"));
        }

        [TestMethod]
        public void UnitConverterTestClear()
        {
            s_unitConverter.SendCommand(Command.Two);
            s_unitConverter.SendCommand(Command.Zero);
            s_unitConverter.SendCommand(Command.Decimal);
            s_unitConverter.SendCommand(Command.Four);
            s_unitConverter.SendCommand(Command.Three);
            s_unitConverter.SendCommand(Command.Clear);

            Assert.IsTrue(s_testVMCallback.CheckDisplayValues("0", "0"));
        }

        [TestMethod]
        public void UnitConverterTestGetters()
        {
            var test1 = new List<Category> { s_testLength, s_testWeight };
            var test2 = new List<Unit> { s_testInches, s_testFeet };

            CollectionAssert.AreEqual(test1, s_unitConverter.GetCategories());
            CollectionAssert.AreEqual(test2, s_unitConverter.SetCurrentCategory(test1[0]).Item1);
        }

        [TestMethod]
        public void UnitConverterTestGetCategory()
        {
            s_unitConverter.SetCurrentCategory(s_testWeight);
            Assert.AreEqual(s_testWeight, s_unitConverter.GetCurrentCategory());
        }

        [TestMethod]
        public void UnitConverterTestUnitTypeSwitching()
        {
            s_unitConverter.SendCommand(Command.Five);
            s_unitConverter.SendCommand(Command.Seven);
            s_unitConverter.SwitchActive("57");
            s_unitConverter.SetCurrentCategory(s_testWeight);
            s_unitConverter.SetCurrentUnitTypes(s_testKilograms, s_testPounds);
            s_unitConverter.SendCommand(Command.Five);
            Assert.IsTrue(s_testVMCallback.CheckDisplayValues("5", "11.0231"));
            Assert.IsTrue(s_testVMCallback.CheckSuggestedValues(new List<Tuple<string, Unit>>()));
        }

        [TestMethod]
        public void UnitConverterTestQuote()
        {
            string input1 = "Weight";
            string output1 = "Weight";
            string input2 = "{p}Weig;[ht|";
            string output2 = "{lb}p{rb}Weig{sc}{lc}ht{p}";
            string input3 = "{{{t;s}}},:]";
            string output3 = "{lb}{lb}{lb}t{sc}s{rb}{rb}{rb}{cm}{co}{rc}";
            Assert.AreEqual(output1, UnitConverter.Quote(input1));
            Assert.AreEqual(output2, UnitConverter.Quote(input2));
            Assert.AreEqual(output3, UnitConverter.Quote(input3));
        }

        [TestMethod]
        public void UnitConverterTestUnquote()
        {
            string input1 = "Weight";
            string input2 = "{p}Weig;[ht|";
            string input3 = "{{{t;s}}},:]";
            Assert.AreEqual(input1, UnitConverter.Unquote(input1));
            Assert.AreEqual(input1, UnitConverter.Unquote(UnitConverter.Quote(input1)));
            Assert.AreEqual(input2, UnitConverter.Unquote(UnitConverter.Quote(input2)));
            Assert.AreEqual(input3, UnitConverter.Unquote(UnitConverter.Quote(input3)));
        }

        [TestMethod]
        public void UnitConverterTestBackspace()
        {
            var test1 = new List<Tuple<string, Unit>> { Tuple.Create("13.66", s_testKilograms) };
            var test2 = new List<Tuple<string, Unit>> { Tuple.Create("13.65", s_testKilograms) };
            var test3 = new List<Tuple<string, Unit>> { Tuple.Create("13.61", s_testKilograms) };
            var test4 = new List<Tuple<string, Unit>> { Tuple.Create("1.36", s_testKilograms) };

            s_unitConverter.SetCurrentCategory(s_testWeight);
            s_unitConverter.SetCurrentUnitTypes(s_testPounds, s_testPounds);
            s_unitConverter.SendCommand(Command.Three);
            s_unitConverter.SendCommand(Command.Zero);
            s_unitConverter.SendCommand(Command.Decimal);
            s_unitConverter.SendCommand(Command.One);
            s_unitConverter.SendCommand(Command.Two);
            Assert.IsTrue(s_testVMCallback.CheckDisplayValues("30.12", "30.12"));
            Assert.IsTrue(s_testVMCallback.CheckSuggestedValues(test1));
            s_unitConverter.SendCommand(Command.Backspace);
            Assert.IsTrue(s_testVMCallback.CheckDisplayValues("30.1", "30.1"));
            Assert.IsTrue(s_testVMCallback.CheckSuggestedValues(test2));
            s_unitConverter.SendCommand(Command.Backspace);
            Assert.IsTrue(s_testVMCallback.CheckDisplayValues("30.", "30"));
            Assert.IsTrue(s_testVMCallback.CheckSuggestedValues(test3));
            s_unitConverter.SendCommand(Command.Backspace);
            Assert.IsTrue(s_testVMCallback.CheckDisplayValues("30", "30"));
            Assert.IsTrue(s_testVMCallback.CheckSuggestedValues(test3));
            s_unitConverter.SendCommand(Command.Backspace);
            Assert.IsTrue(s_testVMCallback.CheckDisplayValues("3", "3"));
            Assert.IsTrue(s_testVMCallback.CheckSuggestedValues(test4));
            s_unitConverter.SendCommand(Command.Backspace);
            Assert.IsTrue(s_testVMCallback.CheckDisplayValues("0", "0"));
            Assert.IsTrue(s_testVMCallback.CheckSuggestedValues(new List<Tuple<string, Unit>>()));
        }

        [TestMethod]
        public void UnitConverterTestScientificInputs()
        {
            s_unitConverter.SetCurrentCategory(s_testWeight);
            s_unitConverter.SetCurrentUnitTypes(s_testPounds, s_testKilograms);
            s_unitConverter.SendCommand(Command.Decimal);
            for (int i = 0; i < 13; i++)
            {
                s_unitConverter.SendCommand(Command.Zero);
            }
            s_unitConverter.SendCommand(Command.One);
            Assert.IsTrue(s_testVMCallback.CheckDisplayValues("0.00000000000001", "4.535920e-15"));

            s_unitConverter.SwitchActive("4.535920e-15");
            for (int i = 0; i < 15; i++)
            {
                s_unitConverter.SendCommand(Command.Nine);
            }
            Assert.IsTrue(s_testVMCallback.CheckDisplayValues("999999999999999", "2.204620e+15"));

            s_unitConverter.SwitchActive("2.20463e+15");
            s_unitConverter.SendCommand(Command.One);
            s_unitConverter.SendCommand(Command.Two);
            s_unitConverter.SendCommand(Command.Three);
            s_unitConverter.SendCommand(Command.Four);
            s_unitConverter.SendCommand(Command.Five);
            s_unitConverter.SendCommand(Command.Six);
            s_unitConverter.SendCommand(Command.Seven);
            Assert.IsTrue(s_testVMCallback.CheckDisplayValues("1234567", "559989.7"));

            s_unitConverter.SwitchActive("559989.7");
            s_unitConverter.SendCommand(Command.One);
            s_unitConverter.SendCommand(Command.Two);
            s_unitConverter.SendCommand(Command.Three);
            s_unitConverter.SendCommand(Command.Four);
            s_unitConverter.SendCommand(Command.Five);
            s_unitConverter.SendCommand(Command.Six);
            s_unitConverter.SendCommand(Command.Seven);
            s_unitConverter.SendCommand(Command.Eight);
            Assert.IsTrue(s_testVMCallback.CheckDisplayValues("12345678", "27217529"));
        }

        [TestMethod]
        public void UnitConverterTestSupplementaryResultRounding()
        {
            var test1 = new List<Tuple<string, Unit>> { Tuple.Create("27.75", s_testFeet) };
            var test2 = new List<Tuple<string, Unit>> { Tuple.Create("277.8", s_testFeet) };
            var test3 = new List<Tuple<string, Unit>> { Tuple.Create("2778", s_testFeet) };

            s_unitConverter.SendCommand(Command.Three);
            s_unitConverter.SendCommand(Command.Three);
            s_unitConverter.SendCommand(Command.Three);
            Assert.IsTrue(s_testVMCallback.CheckSuggestedValues(test1));

            s_unitConverter.SendCommand(Command.Three);
            Assert.IsTrue(s_testVMCallback.CheckSuggestedValues(test2));

            s_unitConverter.SendCommand(Command.Three);
            Assert.IsTrue(s_testVMCallback.CheckSuggestedValues(test3));
        }

        [TestMethod]
        public void UnitConverterTestMaxDigitsReached()
        {
            ExecuteCommands(new[] {
                Command.One, Command.Two, Command.Three, Command.Four, Command.Five,
                Command.Six, Command.Seven, Command.Eight, Command.Nine, Command.One,
                Command.Zero, Command.One, Command.One, Command.One, Command.Two
            });

            Assert.AreEqual(0, s_testVMCallback.GetMaxDigitsReachedCallCount());

            ExecuteCommands(new[] { Command.One });

            Assert.AreEqual(1, s_testVMCallback.GetMaxDigitsReachedCallCount());
        }

        [TestMethod]
        public void UnitConverterTestMaxDigitsReached_LeadingDecimal()
        {
            ExecuteCommands(new[] {
                Command.Zero, Command.Decimal, Command.One, Command.Two, Command.Three,
                Command.Four, Command.Five, Command.Six, Command.Seven, Command.Eight,
                Command.Nine, Command.One, Command.Zero, Command.One, Command.One, Command.One
            });

            Assert.AreEqual(0, s_testVMCallback.GetMaxDigitsReachedCallCount());

            ExecuteCommands(new[] { Command.Two });

            Assert.AreEqual(1, s_testVMCallback.GetMaxDigitsReachedCallCount());
        }

        [TestMethod]
        public void UnitConverterTestMaxDigitsReached_TrailingDecimal()
        {
            ExecuteCommands(new[] {
                Command.One, Command.Two, Command.Three, Command.Four, Command.Five,
                Command.Six, Command.Seven, Command.Eight, Command.Nine, Command.One,
                Command.Zero, Command.One, Command.One, Command.One, Command.Two, Command.Decimal
            });

            Assert.AreEqual(0, s_testVMCallback.GetMaxDigitsReachedCallCount());

            ExecuteCommands(new[] { Command.One });

            Assert.AreEqual(1, s_testVMCallback.GetMaxDigitsReachedCallCount());
        }

        [TestMethod]
        public void UnitConverterTestMaxDigitsReached_MultipleTimes()
        {
            ExecuteCommands(new[] {
                Command.One, Command.Two, Command.Three, Command.Four, Command.Five,
                Command.Six, Command.Seven, Command.Eight, Command.Nine, Command.One,
                Command.Zero, Command.One, Command.One, Command.One, Command.Two
            });

            Assert.AreEqual(0, s_testVMCallback.GetMaxDigitsReachedCallCount());

            for (int count = 1; count <= 10; count++)
            {
                ExecuteCommands(new[] { Command.Three });
                Assert.AreEqual(count, s_testVMCallback.GetMaxDigitsReachedCallCount(), count.ToString());
            }
        }
    }
}
