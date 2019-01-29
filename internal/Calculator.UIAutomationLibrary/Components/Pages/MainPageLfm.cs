// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Etw.Managed;
using MS.Internal.Mita.Foundation.Waiters;

namespace Calculator.UIAutomationLibrary.Components
{
    public class MainPageLfm : ICanFocusWithClicks
    {
        public MainPageLfm(MainPagePom objectModel)
        {
            this.ObjectModel = objectModel;
        }

        public MainPagePom ObjectModel { get; }

        public NavBarLfm OpenNavBar()
        {
            using (EtwWaiter waiter = this.ObjectModel.GetNavBarOpenedWaiter())
            {
                this.ObjectModel.NavButton.Invoke();
                waiter.Wait();
            }

            return new NavBarLfm(this.ObjectModel.NavBarPom);
        }

        public void CloseNavBar()
        {
            this.ObjectModel.NavBarPom.CloseButton.Invoke();
        }

        public void FocusWithClicks()
        {
            this.ObjectModel.Header.DoubleClick();
        }

        public StandardCalculatorLfm NavigateToStandardCalculator()
        {
            var navBar = this.OpenNavBar();
            using (var waiter = this.ObjectModel.GetModeChangedWaiter())
            {
                navBar.SelectStandard();
                waiter.TryWait();
            }

            return new StandardCalculatorLfm(this.ObjectModel.StandardCalculatorPom);
        }

        public ScientificCalculatorLfm NavigateToScientificCalculator()
        {
            var navBar = this.OpenNavBar();
            using (var waiter = this.ObjectModel.GetModeChangedWaiter())
            {
                navBar.SelectScientific();
                waiter.TryWait();
            }

            return new ScientificCalculatorLfm(this.ObjectModel.ScientificCalculatorPom);
        }

        public ProgrammerCalculatorLfm NavigateToProgrammerCalculator()
        {
            var navBar = this.OpenNavBar();
            using (var waiter = this.ObjectModel.GetModeChangedWaiter())
            {
                navBar.SelectProgrammer();
                waiter.TryWait();
            }

            return new ProgrammerCalculatorLfm(this.ObjectModel.ProgrammerCalculatorPom);
        }

        public DateCalculatorLfm NavigateToDateCalculator()
        {
            var navBar = this.OpenNavBar();
            using (var waiter = this.ObjectModel.GetModeChangedWaiter())
            {
                navBar.SelectDate();
                waiter.TryWait();
            }

            return new DateCalculatorLfm(this.ObjectModel.DateCalculatorPom);
        }

        public UnitConverterLfm NavigateToCurrencyConverter()
        {
            var navBar = this.OpenNavBar();
            using (var waiter = this.ObjectModel.GetModeChangedWaiter())
            {
                navBar.SelectCurrency();
                waiter.TryWait();
            }

            return new UnitConverterLfm(this.ObjectModel.UnitConverterPom);
        }

        public UnitConverterLfm NavigateToVolumeConverter()
        {
            var navBar = this.OpenNavBar();
            using (var waiter = this.ObjectModel.GetModeChangedWaiter())
            {
                navBar.SelectVolume();
                waiter.TryWait();
            }

            return new UnitConverterLfm(this.ObjectModel.UnitConverterPom);
        }

        public UnitConverterLfm NavigateToLengthConverter()
        {
            var navBar = this.OpenNavBar();
            using (var waiter = this.ObjectModel.GetModeChangedWaiter())
            {
                navBar.SelectLength();
                waiter.TryWait();
            }

            return new UnitConverterLfm(this.ObjectModel.UnitConverterPom);
        }

        public UnitConverterLfm NavigateToWeightConverter()
        {
            var navBar = this.OpenNavBar();
            using (var waiter = this.ObjectModel.GetModeChangedWaiter())
            {
                navBar.SelectWeight();
                waiter.TryWait();
            }

            return new UnitConverterLfm(this.ObjectModel.UnitConverterPom);
        }

        public UnitConverterLfm NavigateToTemperatureConverter()
        {
            var navBar = this.OpenNavBar();
            using (var waiter = this.ObjectModel.GetModeChangedWaiter())
            {
                navBar.SelectTemperature();
                waiter.TryWait();
            }

            return new UnitConverterLfm(this.ObjectModel.UnitConverterPom);
        }

        public UnitConverterLfm NavigateToEnergyConverter()
        {
            var navBar = this.OpenNavBar();
            using (var waiter = this.ObjectModel.GetModeChangedWaiter())
            {
                navBar.SelectEnergy();
                waiter.TryWait();
            }

            return new UnitConverterLfm(this.ObjectModel.UnitConverterPom);
        }

        public UnitConverterLfm NavigateToAreaConverter()
        {
            var navBar = this.OpenNavBar();
            using (var waiter = this.ObjectModel.GetModeChangedWaiter())
            {
                navBar.SelectArea();
                waiter.TryWait();
            }

            return new UnitConverterLfm(this.ObjectModel.UnitConverterPom);
        }

        public UnitConverterLfm NavigateToSpeedConverter()
        {
            var navBar = this.OpenNavBar();
            using (var waiter = this.ObjectModel.GetModeChangedWaiter())
            {
                navBar.SelectSpeed();
                waiter.TryWait();
            }

            return new UnitConverterLfm(this.ObjectModel.UnitConverterPom);
        }

        public UnitConverterLfm NavigateToTimeConverter()
        {
            var navBar = this.OpenNavBar();
            using (var waiter = this.ObjectModel.GetModeChangedWaiter())
            {
                navBar.SelectTime();
                waiter.TryWait();
            }

            return new UnitConverterLfm(this.ObjectModel.UnitConverterPom);
        }

        public UnitConverterLfm NavigateToPowerConverter()
        {
            var navBar = this.OpenNavBar();
            using (var waiter = this.ObjectModel.GetModeChangedWaiter())
            {
                navBar.SelectPower();
                waiter.TryWait();
            }

            return new UnitConverterLfm(this.ObjectModel.UnitConverterPom);
        }

        public UnitConverterLfm NavigateToDataConverter()
        {
            var navBar = this.OpenNavBar();
            using (var waiter = this.ObjectModel.GetModeChangedWaiter())
            {
                navBar.SelectData();
                waiter.TryWait();
            }

            return new UnitConverterLfm(this.ObjectModel.UnitConverterPom);
        }

        public UnitConverterLfm NavigateToPressureConverter()
        {
            var navBar = this.OpenNavBar();
            using (var waiter = this.ObjectModel.GetModeChangedWaiter())
            {
                navBar.SelectPressure();
                waiter.TryWait();
            }

            return new UnitConverterLfm(this.ObjectModel.UnitConverterPom);
        }

        public UnitConverterLfm NavigateToAngleConverter()
        {
            var navBar = this.OpenNavBar();
            using (var waiter = this.ObjectModel.GetModeChangedWaiter())
            {
                navBar.SelectAngle();
                waiter.TryWait();
            }

            return new UnitConverterLfm(this.ObjectModel.UnitConverterPom);
        }

        public AboutFlyoutLfm OpenAboutFlyout()
        {
            var navBar = this.OpenNavBar();
            using (EtwWaiter waiter = new EtwWaiter(Constants.CalculatorETWProviderGUID, Constants.AboutFlyoutOpenedETWEventName))
            {
                navBar.SelectAbout();
                waiter.Wait();
            }

            return new AboutFlyoutLfm(this.ObjectModel.AboutFlyoutPom);
        }
    }
}
