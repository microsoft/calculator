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
    public class MemoryPanel
    {
        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        public WindowsElement MemoryClear => session.TryFindElementByAccessibilityId("ClearMemoryButton");
        public WindowsElement MemRecall => session.TryFindElementByAccessibilityId("MemRecall");
        public WindowsElement MemPlus => session.TryFindElementByAccessibilityId("MemPlus");
        public WindowsElement MemMinus => session.TryFindElementByAccessibilityId("MemMinus");
        public WindowsElement MemButton => session.TryFindElementByAccessibilityId("memButton");
        public WindowsElement MemoryPane => session.TryFindElementByAccessibilityId("MemoryPanel");
        public WindowsElement MemoryLabel => session.TryFindElementByAccessibilityId("MemoryLabel");
        public WindowsElement MemoryListView => session.TryFindElementByAccessibilityId("MemoryListView");
        public WindowsElement MemoryPaneEmptyLabel => session.TryFindElementByAccessibilityId("MemoryPaneEmpty");

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
