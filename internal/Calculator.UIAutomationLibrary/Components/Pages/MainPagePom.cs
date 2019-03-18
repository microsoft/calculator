// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Etw.Managed;
using MS.Internal.Mita.Foundation;
using MS.Internal.Mita.Foundation.Controls;
using MS.Internal.Mita.Foundation.Waiters;

namespace Calculator.UIAutomationLibrary.Components
{
    /// <summary>
    /// Physical Object Model for the app window.
    /// POM is the implementation model of the app.
    /// See https://en.wikipedia.org/wiki/Model-based_testing for model-based testing.
    /// </summary>
    public class MainPagePom : UIObject
    {
        private const string NavButtonId = "TogglePaneButton";
        private const string SplitViewPaneRootId = "PaneRoot";
        private const string NavBarFlyoutId = "FlyoutNav";
        private const string HeaderId = "Header";
        private const string AboutPageFlyoutId = "AboutPageFlyout";

        public MainPagePom(UIObject uiObject)
            : base(uiObject)
        {
        }

        public Button NavButton => new Button(this.Descendants.Find(UICondition.CreateFromId(NavButtonId)));

        public UIObject Header => new UIObject(this.Descendants.Find(HeaderId));

        public NavBarPom NavBarPom => new NavBarPom(this.Children.Find(SplitViewPaneRootId));

        public EtwWaiter GetNavBarOpenedWaiter() => new EtwWaiter(Constants.CalculatorETWProviderGUID, Constants.NavBarOpenedETWEventName);

        public StandardCalculatorPom StandardCalculatorPom => new StandardCalculatorPom(this);

        public ScientificCalculatorPom ScientificCalculatorPom => new ScientificCalculatorPom(this);

        public ProgrammerCalculatorPom ProgrammerCalculatorPom => new ProgrammerCalculatorPom(this);

        public DateCalculatorPom DateCalculatorPom => new DateCalculatorPom(this);

        public UnitConverterPom UnitConverterPom => new UnitConverterPom(this);

        public AboutFlyoutPom AboutFlyoutPom => new AboutFlyoutPom(this.Descendants.Find(AboutPageFlyoutId));

        public EtwWaiter GetModeChangedWaiter() => new EtwWaiter(Constants.CalculatorETWProviderGUID, Constants.AppModeChangeEndETWEventName);
    }
}
