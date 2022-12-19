// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using OpenQA.Selenium;
using OpenQA.Selenium.Appium.Windows;

namespace CalculatorUITestFramework
{
    /// <summary>
    /// UI elements unique to programmer mode
    /// </summary>
    public class ProgrammerOperatorsPanel
    {
        private WindowsDriver<WindowsElement> session => CalculatorDriver.Instance.CalculatorSession;
        public NumberPad NumberPad = new NumberPad();

        public WindowsElement HexButton => this.session.TryFindElementByAccessibilityId("hexButton");
        public WindowsElement DecButton => this.session.TryFindElementByAccessibilityId("decimalButton");
        public WindowsElement OctButton => this.session.TryFindElementByAccessibilityId("octolButton");
        public WindowsElement BinButton => this.session.TryFindElementByAccessibilityId("binaryButton");
        public WindowsElement FullKeypad => this.session.TryFindElementByAccessibilityId("fullKeypad");
        public WindowsElement BitFlip => this.session.TryFindElementByAccessibilityId("bitFlip");
        public WindowsElement WordButton => this.session.TryFindElementByAccessibilityId("wordButton");
        public WindowsElement QWordButton => this.session.TryFindElementByAccessibilityId("qwordButton");
        public WindowsElement DWordButton => this.session.TryFindElementByAccessibilityId("dwordButton");
        public WindowsElement ByteButton => this.session.TryFindElementByAccessibilityId("byteButton");
        public WindowsElement BitwiseButton => this.session.TryFindElementByAccessibilityId("bitwiseButton");
        public WindowsElement BitShiftButton => this.session.TryFindElementByAccessibilityId("bitShiftButton");
        public WindowsElement AndButton => this.session.TryFindElementByAccessibilityId("andButton");
        public WindowsElement NandButton => this.session.TryFindElementByAccessibilityId("nandButton");
        public WindowsElement OrButton => this.session.TryFindElementByAccessibilityId("orButton");
        public WindowsElement NorButton => this.session.TryFindElementByAccessibilityId("norButton");
        public WindowsElement NotButton => this.session.TryFindElementByAccessibilityId("notButton");
        public WindowsElement XorButton => this.session.TryFindElementByAccessibilityId("xorButton");
        public WindowsElement ArithmeticShiftButton => this.session.TryFindElementByAccessibilityId("arithmeticShiftButton");
        public WindowsElement LogicalShiftButton => this.session.TryFindElementByAccessibilityId("logicalShiftButton");
        public WindowsElement RotateCircularButton => this.session.TryFindElementByAccessibilityId("rotateCircularButton");
        public WindowsElement RotateCarryShiftButton => this.session.TryFindElementByAccessibilityId("rotateCarryShiftButton");
        public WindowsElement AButton => this.session.TryFindElementByAccessibilityId("aButton");
        public WindowsElement BButton => this.session.TryFindElementByAccessibilityId("bButton");
        public WindowsElement CButton => this.session.TryFindElementByAccessibilityId("cButton");
        public WindowsElement DButton => this.session.TryFindElementByAccessibilityId("dButton");
        public WindowsElement EButton => this.session.TryFindElementByAccessibilityId("eButton");
        public WindowsElement FButton => this.session.TryFindElementByAccessibilityId("fButton");
        public WindowsElement LeftShiftButton => this.session.TryFindElementByAccessibilityId("lshButton");
        public WindowsElement RightShiftButton => this.session.TryFindElementByAccessibilityId("rshButton");
        public WindowsElement LeftShiftLogicalButton => this.session.TryFindElementByAccessibilityId("lshLogicalButton");
        public WindowsElement RightShiftLogicalButton => this.session.TryFindElementByAccessibilityId("rshLogicalButton");
        public WindowsElement RoLButton => this.session.TryFindElementByAccessibilityId("rolButton");
        public WindowsElement RoRButton => this.session.TryFindElementByAccessibilityId("rorButton");
        public WindowsElement RoRCarryButton => this.session.TryFindElementByAccessibilityId("rorCarryButton");
        public WindowsElement Bit0 => this.session.TryFindElementByAccessibilityId("Bit0");
        public WindowsElement Bit1 => this.session.TryFindElementByAccessibilityId("Bit1");
        public WindowsElement Bit2 => this.session.TryFindElementByAccessibilityId("Bit2");
        public WindowsElement Bit3 => this.session.TryFindElementByAccessibilityId("Bit3");
        public WindowsElement Bit4 => this.session.TryFindElementByAccessibilityId("Bit4");
        public WindowsElement Bit5 => this.session.TryFindElementByAccessibilityId("Bit5");
        public WindowsElement Bit6 => this.session.TryFindElementByAccessibilityId("Bit6");
        public WindowsElement Bit7 => this.session.TryFindElementByAccessibilityId("Bit7");
        public WindowsElement Bit8 => this.session.TryFindElementByAccessibilityId("Bit8");
        public WindowsElement Bit9 => this.session.TryFindElementByAccessibilityId("Bit9");
        public WindowsElement Bit10 => this.session.TryFindElementByAccessibilityId("Bit10");
        public WindowsElement Bit11 => this.session.TryFindElementByAccessibilityId("Bit11");
        public WindowsElement Bit12 => this.session.TryFindElementByAccessibilityId("Bit12");
        public WindowsElement Bit13 => this.session.TryFindElementByAccessibilityId("Bit13");
        public WindowsElement Bit14 => this.session.TryFindElementByAccessibilityId("Bit14");
        public WindowsElement Bit15 => this.session.TryFindElementByAccessibilityId("Bit15");
        public WindowsElement Bit16 => this.session.TryFindElementByAccessibilityId("Bit16");
        public WindowsElement Bit17 => this.session.TryFindElementByAccessibilityId("Bit17");
        public WindowsElement Bit18 => this.session.TryFindElementByAccessibilityId("Bit18");
        public WindowsElement Bit19 => this.session.TryFindElementByAccessibilityId("Bit19");
        public WindowsElement Bit20 => this.session.TryFindElementByAccessibilityId("Bit20");
        public WindowsElement Bit21 => this.session.TryFindElementByAccessibilityId("Bit21");
        public WindowsElement Bit22 => this.session.TryFindElementByAccessibilityId("Bit22");
        public WindowsElement Bit23 => this.session.TryFindElementByAccessibilityId("Bit23");
        public WindowsElement Bit24 => this.session.TryFindElementByAccessibilityId("Bit24");
        public WindowsElement Bit25 => this.session.TryFindElementByAccessibilityId("Bit25");
        public WindowsElement Bit26 => this.session.TryFindElementByAccessibilityId("Bit26");
        public WindowsElement Bit27 => this.session.TryFindElementByAccessibilityId("Bit27");
        public WindowsElement Bit28 => this.session.TryFindElementByAccessibilityId("Bit28");
        public WindowsElement Bit29 => this.session.TryFindElementByAccessibilityId("Bit29");
        public WindowsElement Bit30 => this.session.TryFindElementByAccessibilityId("Bit30");
        public WindowsElement Bit31 => this.session.TryFindElementByAccessibilityId("Bit31");
        public WindowsElement Bit32 => this.session.TryFindElementByAccessibilityId("Bit32");
        public WindowsElement Bit33 => this.session.TryFindElementByAccessibilityId("Bit33");
        public WindowsElement Bit34 => this.session.TryFindElementByAccessibilityId("Bit34");
        public WindowsElement Bit35 => this.session.TryFindElementByAccessibilityId("Bit35");
        public WindowsElement Bit36 => this.session.TryFindElementByAccessibilityId("Bit36");
        public WindowsElement Bit37 => this.session.TryFindElementByAccessibilityId("Bit37");
        public WindowsElement Bit38 => this.session.TryFindElementByAccessibilityId("Bit38");
        public WindowsElement Bit39 => this.session.TryFindElementByAccessibilityId("Bit39");
        public WindowsElement Bit40 => this.session.TryFindElementByAccessibilityId("Bit40");
        public WindowsElement Bit41 => this.session.TryFindElementByAccessibilityId("Bit41");
        public WindowsElement Bit42 => this.session.TryFindElementByAccessibilityId("Bit42");
        public WindowsElement Bit43 => this.session.TryFindElementByAccessibilityId("Bit43");
        public WindowsElement Bit44 => this.session.TryFindElementByAccessibilityId("Bit44");
        public WindowsElement Bit45 => this.session.TryFindElementByAccessibilityId("Bit45");
        public WindowsElement Bit46 => this.session.TryFindElementByAccessibilityId("Bit46");
        public WindowsElement Bit47 => this.session.TryFindElementByAccessibilityId("Bit47");
        public WindowsElement Bit48 => this.session.TryFindElementByAccessibilityId("Bit48");
        public WindowsElement Bit49 => this.session.TryFindElementByAccessibilityId("Bit49");
        public WindowsElement Bit50 => this.session.TryFindElementByAccessibilityId("Bit50");
        public WindowsElement Bit51 => this.session.TryFindElementByAccessibilityId("Bit51");
        public WindowsElement Bit52 => this.session.TryFindElementByAccessibilityId("Bit52");
        public WindowsElement Bit53 => this.session.TryFindElementByAccessibilityId("Bit53");
        public WindowsElement Bit54 => this.session.TryFindElementByAccessibilityId("Bit54");
        public WindowsElement Bit55 => this.session.TryFindElementByAccessibilityId("Bit55");
        public WindowsElement Bit56 => this.session.TryFindElementByAccessibilityId("Bit56");
        public WindowsElement Bit57 => this.session.TryFindElementByAccessibilityId("Bit57");
        public WindowsElement Bit58 => this.session.TryFindElementByAccessibilityId("Bit58");
        public WindowsElement Bit59 => this.session.TryFindElementByAccessibilityId("Bit59");
        public WindowsElement Bit60 => this.session.TryFindElementByAccessibilityId("Bit60");
        public WindowsElement Bit61 => this.session.TryFindElementByAccessibilityId("Bit61");
        public WindowsElement Bit62 => this.session.TryFindElementByAccessibilityId("Bit62");
        public WindowsElement Bit63 => this.session.TryFindElementByAccessibilityId("Bit63");

        public void SetArithmeticShift()
        {
            BitShiftButton.Click();
            if (this.ArithmeticShiftButton.GetAttribute("isEnabled") != "True")
            {
                ArithmeticShiftButton.Click();
            }
            else
            {
                BitShiftButton.Click();
            }
        }
        public void SetLogicalShift()
        {
            BitShiftButton.Click();
            if (this.LogicalShiftButton.GetAttribute("isEnabled") != "True")
            {
                LogicalShiftButton.Click();
            }
            else
            {
                BitShiftButton.Click();
            }
        }
        public void SetRotateCircularShift()
        {
            BitShiftButton.Click();
            if (this.RotateCircularButton.GetAttribute("isEnabled") != "True")
            {
                RotateCircularButton.Click();
            }
            else
            {
                BitShiftButton.Click();
            }
        }
        public void SetRotateThroughCarryCircularShift()
        {
            BitShiftButton.Click();
            if (this.RotateCarryShiftButton.GetAttribute("isEnabled") != "True")
            {
                RotateCarryShiftButton.Click();
            }
            else
            {
                BitShiftButton.Click();
            }
        }
        public void ResetWordSize()
        {
            string source = this.session.PageSource;
            if (source.Contains("qwordButton"))
            {
                return;
            }
            else if (source.Contains("dwordButton"))
            {
                DWordButton.Click();
                ResetWordSize();
            }
            else if (source.Contains("wordButton"))
            {
                WordButton.Click();
                ResetWordSize();
            }
            else if (source.Contains("byteButton"))
            {
                ByteButton.Click();
                ResetWordSize();
            }
            else
            {
                throw new NotFoundException("Could not find word size buttons in page source");
            }
        }
        public void ResetNumberSystem()
        {
            DecButton.Click();
        }
    }
}
