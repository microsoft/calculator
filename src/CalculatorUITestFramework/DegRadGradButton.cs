using OpenQA.Selenium;
using OpenQA.Selenium.Appium;
using OpenQA.Selenium.Appium.Enums;
using OpenQA.Selenium.Appium.Windows;
using System;
using System.Collections.Generic;
using System.Text;

namespace CalculatorUITestFramework
{
    /// <summary>
    /// Possible states of the DegRadGradButton
    /// </summary>
    public enum DegRadGradState
    {
        Degrees,
        Radians,
        Gradians
    };


    /// <summary>
    /// A wrapper class that controls the state of the degrees, radians and gradians buttons.
    /// </summary>
    public class SetAngleOperatorButtonState
    {
        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        public WindowsElement Element => GetDegRadGradButton();

        private WindowsElement GetDegRadGradButton()
        {
            string source = this.session.PageSource;
            if (source.Contains("degButton"))
            {
                return this.session.TryFindElementByAccessibilityId("degButton");
            }
            else if (source.Contains("gradButton"))
            {
                return this.session.TryFindElementByAccessibilityId("gradButton");
            }
            else if (source.Contains("radButton"))
            {
                return this.session.TryFindElementByAccessibilityId("radButton");
            }

            throw new NotFoundException("Could not find deg, rad or grad button in page source");
        }

        /// <summary>
        /// Set the state of the degrees, radians and gradients buttons.
        /// </summary>
        public DegRadGradState State
        {
            //Get the current status of the button and return the enum for that state
            get
            {
                var id = Element.GetAttribute("AutomationId");
                switch (id)
                {
                    case "degButton":
                        return DegRadGradState.Degrees;
                    case "radButton":
                        return DegRadGradState.Radians;
                    case "gradButton":
                        return DegRadGradState.Gradians;
                    default:
                        throw new NotImplementedException();
                }
            }

            //set the desired string value for the button
            set
            {
                string desiredId;
                switch (value)
                {
                    case DegRadGradState.Degrees:
                        desiredId = "degButton";
                        break;
                    case DegRadGradState.Gradians:
                        desiredId = "gradButton";
                        break;
                    case DegRadGradState.Radians:
                        desiredId = "radButton";
                        break;
                    default:
                        throw new NotImplementedException();
                }
                while (this.Element.GetAttribute("AutomationId") != desiredId)
                {
                    var foo = this.Element.GetAttribute("AutomationId");
                    this.Element.Click();
                }
            }
        }
    }
}
