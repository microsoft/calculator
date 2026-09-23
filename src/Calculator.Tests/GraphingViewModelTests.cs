// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;

using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

namespace Calculator.Tests
{
    [TestClass]
    public class GraphingViewModelTests
    {
        [TestMethod]
        public void VariableViewModelProxiesTheGraphVariable()
        {
            var variable = new GraphControl.Variable(2)
            {
                Min = -5,
                Max = 5,
                Step = 0.25
            };
            var viewModel = new VariableViewModel("a", variable);

            Assert.AreEqual(-5, viewModel.Min);
            Assert.AreEqual(5, viewModel.Max);
            Assert.AreEqual(0.25, viewModel.Step);
            Assert.AreEqual(2, viewModel.Value);

            viewModel.Step = 0.5;
            viewModel.Value = 8;

            Assert.AreEqual(0.5, variable.Step);
            Assert.AreEqual(8, variable.Value);
            Assert.AreEqual(8, variable.Max);
        }

        [TestMethod]
        public void EquationErrorsUseExistingLocalizedResources()
        {
            var resources = AppResourceProvider.GetInstance();

            Assert.AreEqual(
                resources.GetResourceString("Overflow"),
                EquationViewModel.EquationErrorText(
                    GraphControl.ErrorType.Evaluation,
                    (int)GraphControl.EvaluationErrorCode.Overflow));
            Assert.AreEqual(
                resources.GetResourceString("ParenthesisMismatch"),
                EquationViewModel.EquationErrorText(
                    GraphControl.ErrorType.Syntax,
                    (int)GraphControl.SyntaxErrorCode.ParenthesisMismatch));
        }
    }
}
