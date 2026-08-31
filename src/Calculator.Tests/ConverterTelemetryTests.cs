// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;
using System.Linq;

using CalculatorApp.ViewModel.Common;

using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Calculator.Tests
{
    [TestClass]
    public class ConverterTelemetryTests
    {
        [TestMethod]
        public void PickerOpened_LogsExpectedFieldsOnly()
        {
            var sink = new RecordingConverterTelemetrySink();
            var logger = new TraceLogger(sink);

            logger.LogConverterPickerOpened(ViewMode.Volume, isFromUnit: true);

            Assert.AreEqual("ConverterPickerOpened", sink.EventName);
            CollectionAssert.AreEquivalent(
                new[] { "CalcMode", "IsFromUnit" },
                sink.Fields.Keys.ToArray());
            Assert.AreEqual("Volume", sink.Fields["CalcMode"]);
            Assert.AreEqual(true, sink.Fields["IsFromUnit"]);
        }

        [TestMethod]
        public void SearchUsed_LogsCountsWithoutRawInput()
        {
            var sink = new RecordingConverterTelemetrySink();
            var logger = new TraceLogger(sink);

            logger.LogConverterSearchUsed(ViewMode.Volume, queryLength: 2, resultCount: 3);

            Assert.AreEqual("ConverterSearchUsed", sink.EventName);
            CollectionAssert.AreEquivalent(
                new[] { "CalcMode", "QueryLength", "ResultCount" },
                sink.Fields.Keys.ToArray());
            Assert.AreEqual("Volume", sink.Fields["CalcMode"]);
            Assert.AreEqual(2, sink.Fields["QueryLength"]);
            Assert.AreEqual(3, sink.Fields["ResultCount"]);
        }

        [TestMethod]
        public void CategorySelected_LogsExpectedFieldsOnly()
        {
            var sink = new RecordingConverterTelemetrySink();
            var logger = new TraceLogger(sink);

            logger.LogConverterCategorySelected(ViewMode.Volume);

            Assert.AreEqual("ConverterCategorySelected", sink.EventName);
            CollectionAssert.AreEquivalent(
                new[] { "CalcMode" },
                sink.Fields.Keys.ToArray());
            Assert.AreEqual("Volume", sink.Fields["CalcMode"]);
        }

        [TestMethod]
        public void UnitSelected_LogsExpectedFieldsOnly()
        {
            var sink = new RecordingConverterTelemetrySink();
            var logger = new TraceLogger(sink);

            logger.LogConverterUnitSelected(ViewMode.Volume, unitId: 74, isFromUnit: true);

            Assert.AreEqual("ConverterUnitSelected", sink.EventName);
            CollectionAssert.AreEquivalent(
                new[] { "CalcMode", "UnitId", "IsFromUnit" },
                sink.Fields.Keys.ToArray());
            Assert.AreEqual("Volume", sink.Fields["CalcMode"]);
            Assert.AreEqual(74, sink.Fields["UnitId"]);
            Assert.AreEqual(true, sink.Fields["IsFromUnit"]);
        }

        [TestMethod]
        public void UnitsSwapped_LogsExpectedFieldsOnly()
        {
            var sink = new RecordingConverterTelemetrySink();
            var logger = new TraceLogger(sink);

            logger.LogConverterUnitsSwapped(ViewMode.Volume);

            Assert.AreEqual("ConverterUnitsSwapped", sink.EventName);
            CollectionAssert.AreEquivalent(
                new[] { "CalcMode" },
                sink.Fields.Keys.ToArray());
            Assert.AreEqual("Volume", sink.Fields["CalcMode"]);
        }

        [TestMethod]
        public void InputReceived_LogsExpectedFieldsOnly()
        {
            var sink = new RecordingConverterTelemetrySink();
            var logger = new TraceLogger(sink);

            logger.LogConverterInputReceived(ViewMode.Volume);

            Assert.AreEqual("ConverterInputReceived", sink.EventName);
            CollectionAssert.AreEquivalent(
                new[] { "CalcMode" },
                sink.Fields.Keys.ToArray());
            Assert.AreEqual("Volume", sink.Fields["CalcMode"]);
        }

        private sealed class RecordingConverterTelemetrySink : IConverterTelemetrySink
        {
            public string EventName { get; private set; }

            public IReadOnlyDictionary<string, object> Fields { get; private set; }

            public void Log(string eventName, IReadOnlyDictionary<string, object> fields)
            {
                EventName = eventName;
                Fields = new Dictionary<string, object>(fields);
            }
        }
    }
}
