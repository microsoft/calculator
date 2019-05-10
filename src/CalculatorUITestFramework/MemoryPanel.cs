using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium.Appium;
using OpenQA.Selenium.Appium.Windows;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CalculatorUITestFramework
{
    public class MemoryPanel : ApplicationBase
    {
        public WindowsElement MemoryClear => TryFindElementByAccessibilityId("ClearMemoryButton");
        public WindowsElement MemRecall => TryFindElementByAccessibilityId("MemRecall");
        public WindowsElement MemPlus => TryFindElementByAccessibilityId("MemPlus");
        public WindowsElement MemMinus => TryFindElementByAccessibilityId("MemMinus");
        public WindowsElement MemButton => TryFindElementByAccessibilityId("memButton");
        public WindowsElement MemoryPane => TryFindElementByAccessibilityId("MemoryPanel");
        public WindowsElement MemoryLabel => TryFindElementByAccessibilityId("MemoryLabel");
        public WindowsElement MemoryListView => TryFindElementByAccessibilityId("MemoryListView");

        /// <summary>
        /// Opens the Memory Pane by clicking the Memory pivot label.
        /// </summary>
        public void OpenMemoryPanel()
        {
            MemoryLabel.Click();
            MemoryPane.WaitForDisplayed();
        }

        /// <summary>
        /// Gets all of the memory items listed in the Memory Pane.
        /// </summary>
        /// <returns>A readonly collection of memory items.</returns>
        public ReadOnlyCollection<AppiumWebElement> GetAllMemoryListViewItems()
        {
            OpenMemoryPanel();
            return MemoryListView.FindElementsByClassName("ListViewItem");
        }
    }
}
