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

        public WindowsElement HexButton => session.TryFindElementByAccessibilityId("hexButton");
        public WindowsElement DecButton => session.TryFindElementByAccessibilityId("decimalButton");
        public WindowsElement OctButton => session.TryFindElementByAccessibilityId("octolButton");
        public WindowsElement BinButton => session.TryFindElementByAccessibilityId("binaryButton");
        public WindowsElement FullKeypad => session.TryFindElementByAccessibilityId("fullKeypad");
        public WindowsElement BitFlip => session.TryFindElementByAccessibilityId("bitFlip");
        public WindowsElement WordButton => session.TryFindElementByAccessibilityId("wordButton");
        public WindowsElement QWordButton => session.TryFindElementByAccessibilityId("qwordButton");
        public WindowsElement DWordButton => session.TryFindElementByAccessibilityId("dwordButton");
        public WindowsElement ByteButton => session.TryFindElementByAccessibilityId("byteButton");
        public WindowsElement BitwiseButton => session.TryFindElementByAccessibilityId("bitwiseButton");
        public WindowsElement BitShiftButton => session.TryFindElementByAccessibilityId("bitShiftButton");
        public WindowsElement AndButton => session.TryFindElementByAccessibilityId("andButton");
        public WindowsElement NandButton => session.TryFindElementByAccessibilityId("nandButton");
        public WindowsElement OrButton => session.TryFindElementByAccessibilityId("orButton");
        public WindowsElement NorButton => session.TryFindElementByAccessibilityId("norButton");
        public WindowsElement NotButton => session.TryFindElementByAccessibilityId("notButton");
        public WindowsElement XorButton => session.TryFindElementByAccessibilityId("xorButton");
        public WindowsElement ArithmeticShiftButton => session.TryFindElementByAccessibilityId("arithmeticShiftButton");
        public WindowsElement LogicalShiftButton => session.TryFindElementByAccessibilityId("logicalShiftButton");
        public WindowsElement RotateCircularButton => session.TryFindElementByAccessibilityId("rotateCircularButton");
        public WindowsElement RotateCarryShiftButton => session.TryFindElementByAccessibilityId("rotateCarryShiftButton");
        public WindowsElement AButton => session.TryFindElementByAccessibilityId("aButton");
        public WindowsElement BButton => session.TryFindElementByAccessibilityId("bButton");
        public WindowsElement CButton => session.TryFindElementByAccessibilityId("cButton");
        public WindowsElement DButton => session.TryFindElementByAccessibilityId("dButton");
        public WindowsElement EButton => session.TryFindElementByAccessibilityId("eButton");
        public WindowsElement FButton => session.TryFindElementByAccessibilityId("fButton");
        public WindowsElement LeftShiftButton => session.TryFindElementByAccessibilityId("lshButton");
        public WindowsElement RightShiftButton => session.TryFindElementByAccessibilityId("rshButton");
        public WindowsElement LeftShiftLogicalButton => session.TryFindElementByAccessibilityId("lshLogicalButton");
        public WindowsElement RightShiftLogicalButton => session.TryFindElementByAccessibilityId("rshLogicalButton");
        public WindowsElement RoLButton => session.TryFindElementByAccessibilityId("rolButton");
        public WindowsElement RoRButton => session.TryFindElementByAccessibilityId("rorButton");
        public WindowsElement RoRCarryButton => session.TryFindElementByAccessibilityId("rorCarryButton");
        public WindowsElement Bit0 => session.TryFindElementByAccessibilityId("Bit0");
        public WindowsElement Bit1 => session.TryFindElementByAccessibilityId("Bit1");
        public WindowsElement Bit2 => session.TryFindElementByAccessibilityId("Bit2");
        public WindowsElement Bit3 => session.TryFindElementByAccessibilityId("Bit3");
        public WindowsElement Bit4 => session.TryFindElementByAccessibilityId("Bit4");
        public WindowsElement Bit5 => session.TryFindElementByAccessibilityId("Bit5");
        public WindowsElement Bit6 => session.TryFindElementByAccessibilityId("Bit6");
        public WindowsElement Bit7 => session.TryFindElementByAccessibilityId("Bit7");
        public WindowsElement Bit8 => session.TryFindElementByAccessibilityId("Bit8");
        public WindowsElement Bit9 => session.TryFindElementByAccessibilityId("Bit9");
        public WindowsElement Bit10 => session.TryFindElementByAccessibilityId("Bit10");
        public WindowsElement Bit11 => session.TryFindElementByAccessibilityId("Bit11");
        public WindowsElement Bit12 => session.TryFindElementByAccessibilityId("Bit12");
        public WindowsElement Bit13 => session.TryFindElementByAccessibilityId("Bit13");
        public WindowsElement Bit14 => session.TryFindElementByAccessibilityId("Bit14");
        public WindowsElement Bit15 => session.TryFindElementByAccessibilityId("Bit15");
        public WindowsElement Bit16 => session.TryFindElementByAccessibilityId("Bit16");
        public WindowsElement Bit17 => session.TryFindElementByAccessibilityId("Bit17");
        public WindowsElement Bit18 => session.TryFindElementByAccessibilityId("Bit18");
        public WindowsElement Bit19 => session.TryFindElementByAccessibilityId("Bit19");
        public WindowsElement Bit20 => session.TryFindElementByAccessibilityId("Bit20");
        public WindowsElement Bit21 => session.TryFindElementByAccessibilityId("Bit21");
        public WindowsElement Bit22 => session.TryFindElementByAccessibilityId("Bit22");
        public WindowsElement Bit23 => session.TryFindElementByAccessibilityId("Bit23");
        public WindowsElement Bit24 => session.TryFindElementByAccessibilityId("Bit24");
        public WindowsElement Bit25 => session.TryFindElementByAccessibilityId("Bit25");
        public WindowsElement Bit26 => session.TryFindElementByAccessibilityId("Bit26");
        public WindowsElement Bit27 => session.TryFindElementByAccessibilityId("Bit27");
        public WindowsElement Bit28 => session.TryFindElementByAccessibilityId("Bit28");
        public WindowsElement Bit29 => session.TryFindElementByAccessibilityId("Bit29");
        public WindowsElement Bit30 => session.TryFindElementByAccessibilityId("Bit30");
        public WindowsElement Bit31 => session.TryFindElementByAccessibilityId("Bit31");
        public WindowsElement Bit32 => session.TryFindElementByAccessibilityId("Bit32");
        public WindowsElement Bit33 => session.TryFindElementByAccessibilityId("Bit33");
        public WindowsElement Bit34 => session.TryFindElementByAccessibilityId("Bit34");
        public WindowsElement Bit35 => session.TryFindElementByAccessibilityId("Bit35");
        public WindowsElement Bit36 => session.TryFindElementByAccessibilityId("Bit36");
        public WindowsElement Bit37 => session.TryFindElementByAccessibilityId("Bit37");
        public WindowsElement Bit38 => session.TryFindElementByAccessibilityId("Bit38");
        public WindowsElement Bit39 => session.TryFindElementByAccessibilityId("Bit39");
        public WindowsElement Bit40 => session.TryFindElementByAccessibilityId("Bit40");
        public WindowsElement Bit41 => session.TryFindElementByAccessibilityId("Bit41");
        public WindowsElement Bit42 => session.TryFindElementByAccessibilityId("Bit42");
        public WindowsElement Bit43 => session.TryFindElementByAccessibilityId("Bit43");
        public WindowsElement Bit44 => session.TryFindElementByAccessibilityId("Bit44");
        public WindowsElement Bit45 => session.TryFindElementByAccessibilityId("Bit45");
        public WindowsElement Bit46 => session.TryFindElementByAccessibilityId("Bit46");
        public WindowsElement Bit47 => session.TryFindElementByAccessibilityId("Bit47");
        public WindowsElement Bit48 => session.TryFindElementByAccessibilityId("Bit48");
        public WindowsElement Bit49 => session.TryFindElementByAccessibilityId("Bit49");
        public WindowsElement Bit50 => session.TryFindElementByAccessibilityId("Bit50");
        public WindowsElement Bit51 => session.TryFindElementByAccessibilityId("Bit51");
        public WindowsElement Bit52 => session.TryFindElementByAccessibilityId("Bit52");
        public WindowsElement Bit53 => session.TryFindElementByAccessibilityId("Bit53");
        public WindowsElement Bit54 => session.TryFindElementByAccessibilityId("Bit54");
        public WindowsElement Bit55 => session.TryFindElementByAccessibilityId("Bit55");
        public WindowsElement Bit56 => session.TryFindElementByAccessibilityId("Bit56");
        public WindowsElement Bit57 => session.TryFindElementByAccessibilityId("Bit57");
        public WindowsElement Bit58 => session.TryFindElementByAccessibilityId("Bit58");
        public WindowsElement Bit59 => session.TryFindElementByAccessibilityId("Bit59");
        public WindowsElement Bit60 => session.TryFindElementByAccessibilityId("Bit60");
        public WindowsElement Bit61 => session.TryFindElementByAccessibilityId("Bit61");
        public WindowsElement Bit62 => session.TryFindElementByAccessibilityId("Bit62");
        public WindowsElement Bit63 => session.TryFindElementByAccessibilityId("Bit63");

        public void SetArithmeticShift()
        {
            BitShiftButton.Click();
            if (ArithmeticShiftButton.GetAttribute("isEnabled") != "True")
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
            if (LogicalShiftButton.GetAttribute("isEnabled") != "True")
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
            if (RotateCircularButton.GetAttribute("isEnabled") != "True")
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
            if (RotateCarryShiftButton.GetAttribute("isEnabled") != "True")
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
            string source = session.PageSource;
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
