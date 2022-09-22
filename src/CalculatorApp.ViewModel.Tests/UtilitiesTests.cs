using CalculatorApp.ViewModel.Common;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace CalculatorApp.ViewModel.Tests
{
    [TestClass]
    public class UtilitiesTests
    {
        [TestMethod]
        public void NoCoreWindowOnThread_WindowIdIsNegative1()
        {
            Assert.AreEqual(-1, Utilities.GetWindowId());
        }
    }
}
