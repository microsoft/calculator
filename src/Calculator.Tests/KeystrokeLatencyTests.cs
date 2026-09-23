// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Diagnostics;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

namespace Calculator.Tests
{
    // The budget sits an order of magnitude above the measured 0.16 ms cost on x64 Debug.
    // It catches a change of order, not small movements that would make the suite flaky.
    [TestClass]
    public class KeystrokeLatencyTests
    {
        // A frame is about 16 ms, so anything in this range is imperceptible; the budget is set an
        // order of magnitude above what the code costs today.
        private const double KeystrokeBudgetMs = 5.0;

        // Enough repetitions to average out scheduling noise, and no more. Driving the view models
        // harder than a person could type destabilizes the UWP test host, which shows up as the run
        // aborting with "Unable to communicate with test host process" rather than as a failure.
        private const int KeystrokeRounds = 5;

        [TestMethod]
        public void TypingIntoTheConverterStaysWellInsideAFrame()
        {
            var viewModel = new UnitConverterViewModel();
            var digits = new[]
            {
                NumbersAndOperatorsEnum.One, NumbersAndOperatorsEnum.Two, NumbersAndOperatorsEnum.Three,
                NumbersAndOperatorsEnum.Four, NumbersAndOperatorsEnum.Five, NumbersAndOperatorsEnum.Six,
                NumbersAndOperatorsEnum.Seven,
            };

            for (int warmup = 0; warmup < 1; warmup++)
            {
                viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Clear);
                foreach (var digit in digits)
                {
                    viewModel.ButtonPressedCommand.Execute(digit);
                }
            }

            var stopwatch = Stopwatch.StartNew();
            for (int round = 0; round < KeystrokeRounds; round++)
            {
                viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Clear);
                foreach (var digit in digits)
                {
                    viewModel.ButtonPressedCommand.Execute(digit);
                }
            }
            stopwatch.Stop();

            double perKeystroke = stopwatch.Elapsed.TotalMilliseconds / (KeystrokeRounds * (digits.Length + 1));
            Assert.IsTrue(
                perKeystroke < KeystrokeBudgetMs,
                $"A converter keystroke took {perKeystroke:F3} ms, over the {KeystrokeBudgetMs} ms budget.");
        }
    }
}
