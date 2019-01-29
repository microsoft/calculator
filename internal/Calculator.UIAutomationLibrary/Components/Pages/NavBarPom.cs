// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Linq;
using MS.Internal.Mita.Foundation;
using MS.Internal.Mita.Foundation.Controls;

namespace Calculator.UIAutomationLibrary.Components
{
    /// <summary>
    /// Represents the navigation menu.
    /// </summary>
    public class NavBarPom : UIObject
    {
        private const string StandardId = "Standard";
        private const string ScientificId = "Scientific";
        private const string ProgrammerId = "Programmer";
        private const string DateId = "Date";
        private const string CurrencyId = "Currency";
        private const string VolumeId = "Volume";
        private const string LengthId = "Length";
        private const string WeightId = "Weight";
        private const string TemperatureId = "Temperature";
        private const string EnergyId = "Energy";
        private const string AreaId = "Area";
        private const string SpeedId = "Speed";
        private const string TimeId = "Time";
        private const string PowerId = "Power";
        private const string DataId = "Data";
        private const string PressureId = "Pressure";
        private const string AngleId = "Angle";
        private const string AboutId = "AboutButton";
        private const string CloseId = "TogglePaneButton";
        private const string FlyoutListViewId = "MenuItemsHost";
        private const string ConverterSectionId = "Converter";
        private const string ConverterTextKey = "ConverterModeText";

        /// <summary>
        /// Initializes a new instance of the <see cref="NavBarPom" /> class.
        /// </summary>
        /// <param name="uiObject">The UIObject that is the root of the navigation menu.</param>
        public NavBarPom(UIObject uiObject) : base(uiObject)
        {
        }

        public ListViewItem StandardMenuItem => ScrollAndGetItem(StandardId);

        public ListViewItem ScientificMenuItem => ScrollAndGetItem(ScientificId);

        public ListViewItem ProgrammerMenuItem => ScrollAndGetItem(ProgrammerId);

        public ListViewItem DateMenuItem => ScrollAndGetItem(DateId);

        public ListViewItem CurrencyMenuItem => ScrollAndGetItem(CurrencyId);

        public ListViewItem VolumeMenuItem => ScrollAndGetItem(VolumeId);

        public ListViewItem LengthMenuItem => ScrollAndGetItem(LengthId);

        public ListViewItem WeightMenuItem => ScrollAndGetItem(WeightId);

        public ListViewItem TemperatureMenuItem => ScrollAndGetItem(TemperatureId);

        public ListViewItem EnergyMenuItem => ScrollAndGetItem(EnergyId);

        public ListViewItem AreaMenuItem => ScrollAndGetItem(AreaId);

        public ListViewItem SpeedMenuItem => ScrollAndGetItem(SpeedId);

        public ListViewItem TimeMenuItem => ScrollAndGetItem(TimeId);

        public ListViewItem PowerMenuItem => ScrollAndGetItem(PowerId);

        public ListViewItem DataMenuItem => ScrollAndGetItem(DataId);

        public ListViewItem PressureMenuItem => ScrollAndGetItem(PressureId);

        public ListViewItem AngleMenuItem => ScrollAndGetItem(AngleId);

        public Button AboutButton => new Button(this.Descendants.Find(AboutId));

        public Button CloseButton => new Button(this.Parent.Children.Find(CloseId));

        public ListView ModeListView => new ListView(this.Descendants.Find(FlyoutListViewId));

        private ListViewItem ScrollAndGetItem(string id)
        {
            ListViewItem item;
            var res = this.ModeListView.AllItems.TryFind(id, out item);

            item.ScrollIntoView();

            return item;
        }
    }
}
