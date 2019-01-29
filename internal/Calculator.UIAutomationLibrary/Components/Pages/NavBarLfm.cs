// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Windows.Automation;
using MS.Internal.Mita.Foundation;
using MS.Internal.Mita.Foundation.Controls;
using MS.Internal.Mita.Foundation.Patterns;

namespace Calculator.UIAutomationLibrary.Components
{
    /// <summary>
    /// Represents the NavBar of the Calculator app.
    /// </summary>
    public class NavBarLfm : ICanFocusWithClicks
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="NavBarLfm" /> class.
        /// </summary>
        /// <param name="objectModel">The NavBarPom that represents the NavBar.</param>
        public NavBarLfm(NavBarPom objectModel)
        {
            this.ObjectModel = objectModel;
        }

        public NavBarPom ObjectModel { get; }

        public void SelectStandard()
        {
            SelectItem(this.ObjectModel.StandardMenuItem);
        }

        public void SelectScientific()
        {
            SelectItem(this.ObjectModel.ScientificMenuItem);
        }

        public void SelectProgrammer()
        {
            SelectItem(this.ObjectModel.ProgrammerMenuItem);
        }

        public void SelectDate()
        {
            SelectItem(this.ObjectModel.DateMenuItem);
        }

        public void SelectCurrency()
        {
            SelectItem(this.ObjectModel.CurrencyMenuItem);
        }

        public void SelectVolume()
        {
            SelectItem(this.ObjectModel.VolumeMenuItem);
        }

        public void SelectLength()
        {
            SelectItem(this.ObjectModel.LengthMenuItem);
        }

        public void SelectWeight()
        {
            SelectItem(this.ObjectModel.WeightMenuItem);
        }

        public void SelectTemperature()
        {
            SelectItem(this.ObjectModel.TemperatureMenuItem);
        }

        public void SelectEnergy()
        {
            SelectItem(this.ObjectModel.EnergyMenuItem);
        }

        public void SelectArea()
        {
            SelectItem(this.ObjectModel.AreaMenuItem);
        }

        public void SelectSpeed()
        {
            SelectItem(this.ObjectModel.SpeedMenuItem);
        }

        public void SelectTime()
        {
            SelectItem(this.ObjectModel.TimeMenuItem);
        }

        public void SelectPower()
        {
            SelectItem(this.ObjectModel.PowerMenuItem);
        }

        public void SelectData()
        {
            SelectItem(this.ObjectModel.DataMenuItem);
        }

        public void SelectPressure()
        {
            SelectItem(this.ObjectModel.PressureMenuItem);
        }

        public void SelectAngle()
        {
            SelectItem(this.ObjectModel.AngleMenuItem);
        }

        public void SelectAbout()
        {
            this.ObjectModel.AboutButton.Invoke();
        }

        public void Close()
        {
            this.ObjectModel.CloseButton.Invoke();
        }

        public void FocusWithClicks()
        {
            // To focus (for AccSpot) without changing anything, click to the right of the close button.
            Button button = this.ObjectModel.CloseButton;
            int xPos = button.BoundingRectangle.Width + Constants.ClickMargin;
            int yPos = button.BoundingRectangle.Height / 2;
            button.DoubleClick(PointerButtons.Primary, xPos, yPos);
        }

        private void SelectItem(ListViewItem item)
        {
            if (item.IsSelected)
            {
                this.Close();
            }
            else
            {
                item.Select();
            }
        }
    }
}
