//******************************************************************************
//
// Copyright (c) 2017 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//******************************************************************************

using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium.Appium.Windows;
using System.Threading;
using System;

namespace CalculatorTest
{
    [TestClass]
    public class ScenarioStandard : CalculatorSession
    {
        private static WindowsElement header;
        private static WindowsElement calculatorResult;

        [TestMethod]
        public void Addition()
        {
            Thread.Sleep(5000);
            // Find the buttons by their names and click them in sequence to peform 1 + 7 = 8
            session.FindElementByName("One").Click();
            session.FindElementByName("Plus").Click();
            session.FindElementByName("Seven").Click();
            session.FindElementByName("Equals").Click();
            Assert.AreEqual("8", GetCalculatorResultText());
        }

        [TestMethod]
        public void Division()
        {
            Thread.Sleep(5000);
            // Find the buttons by their accessibility ids and click them in sequence to perform 88 / 11 = 8
            session.FindElementByAccessibilityId("num8Button").Click();
            session.FindElementByAccessibilityId("num8Button").Click();
            session.FindElementByAccessibilityId("divideButton").Click();
            session.FindElementByAccessibilityId("num1Button").Click();
            session.FindElementByAccessibilityId("num1Button").Click();
            session.FindElementByAccessibilityId("equalButton").Click();
            Assert.AreEqual("8", GetCalculatorResultText());
        }

        [TestMethod]
        public void Multiplication()
        {
            Thread.Sleep(5000);
            // Find the buttons by their names using XPath and click them in sequence to perform 9 x 9 = 81
            session.FindElementByXPath("//Button[@Name='Nine']").Click();
            session.FindElementByXPath("//Button[@Name='Multiply by']").Click();
            session.FindElementByXPath("//Button[@Name='Nine']").Click();
            session.FindElementByXPath("//Button[@Name='Equals']").Click();
            Assert.AreEqual("81", GetCalculatorResultText());
        }

        [TestMethod]
        public void Subtraction()
        {
            Thread.Sleep(5000);
            // Find the buttons by their accessibility ids using XPath and click them in sequence to perform 9 - 1 = 8
            session.FindElementByXPath("//Button[@AutomationId=\"num9Button\"]").Click();
            session.FindElementByXPath("//Button[@AutomationId=\"minusButton\"]").Click();
            session.FindElementByXPath("//Button[@AutomationId=\"num1Button\"]").Click();
            session.FindElementByXPath("//Button[@AutomationId=\"equalButton\"]").Click();
            Assert.AreEqual("8", GetCalculatorResultText());
        }

        /*
        [TestMethod]
        public void Templatized()
        {
            Templatized("One", "Plus", "Seven", "8");
            Templatized("Nine", "Minus", "One", "8");
            Templatized("Eight", "Divide by", "Eight", "1");
        }
        */
        [TestMethod]
        [DataRow("One", "Plus", "Seven", "8")]
        [DataRow("Nine", "Minus", "One", "8")]
        [DataRow("Eight", "Divide by", "Eight", "1")]
        public void Templatized(string input1, string operation, string input2, string expectedResult)
        {
            Thread.Sleep(5000);
            // Run sequence of button presses specified above and validate the results
            session.FindElementByName(input1).Click();
            session.FindElementByName(operation).Click();
            session.FindElementByName(input2).Click();
            session.FindElementByName("Equals").Click();
            Assert.AreEqual(expectedResult, GetCalculatorResultText());
        }
        
        [ClassInitialize]
        public static void ClassInitialize(TestContext context)
        {
            // Create session to launch a Calculator window
            Setup(context);

            Thread.Sleep(5000);

            // Identify calculator mode by locating the header
            try
            {
                header = session.FindElementByAccessibilityId("Header");
            }
            catch
            {
                header = session.FindElementByAccessibilityId("ContentPresenter");
            }            

            // Ensure that calculator is in standard mode
            if (!header.Text.Equals("Standard", StringComparison.OrdinalIgnoreCase))
            {
                session.FindElementByAccessibilityId("TogglePaneButton").Click();
                Thread.Sleep(TimeSpan.FromSeconds(1));
                var splitViewPane = session.FindElementByClassName("SplitViewPane");
                splitViewPane.FindElementByName("Standard Calculator").Click();
                Thread.Sleep(TimeSpan.FromSeconds(1));
                Assert.IsTrue(header.Text.Equals("Standard", StringComparison.OrdinalIgnoreCase));
            }

            Thread.Sleep(5000);

            // Locate the calculatorResult element
            calculatorResult = session.FindElementByAccessibilityId("CalculatorResults");
            Assert.IsNotNull(calculatorResult);
        }

        [ClassCleanup]
        public static void ClassCleanup()
        {
            TearDown();
        }

        [TestInitialize]
        public void Clear()
        {
            Thread.Sleep(5000);
            session.FindElementByName("Clear").Click();
            Assert.AreEqual("0", GetCalculatorResultText());
            Thread.Sleep(10000);
        }

        private string GetCalculatorResultText()
        {
            return calculatorResult.Text.Replace("Display is", string.Empty).Trim();
        }
    }
}
