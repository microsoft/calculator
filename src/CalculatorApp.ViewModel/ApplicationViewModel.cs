// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.ViewModelNative;
using CalculatorApp.ViewModelNative.Common;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Windows.Input;
using Windows.Foundation;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;

namespace CalculatorApp.ViewModel
{
    public class ApplicationViewModel : INotifyPropertyChanged
    {
        private class CommandWrapper : ICommand
        {
            private readonly Action action;

            public CommandWrapper(Action action)
            {
                this.action = action;
            }

            public event EventHandler CanExecuteChanged
            {
                add { }
                remove { }
            }

            public bool CanExecute(object parameter)
            {
                return true;
            }

            public void Execute(object parameter)
            {
                this.action();
            }
        }

        public const string ModeLocalSettings = "Mode";
        public const string AlwaysOnTopLaunchedLocalSettings = "calculatorAlwaysOnTopLaunched";
        public const string WidthLocalSettings = "calculatorAlwaysOnTopLastWidth";
        public const string HeightLocalSettings = "calculatorAlwaysOnTopLastHeight";

        public StandardCalculatorViewModel CalculatorViewModel { get; private set; }
        public GraphingCalculatorViewModel GraphingCalcViewModel { get; private set; }
        public DateCalculatorViewModel DateCalcViewModel { get; private set; }
        public UnitConverterViewModel ConverterViewModel { get; private set; }

        public event PropertyChangedEventHandler PropertyChanged;

        private ViewMode mode = ViewMode.None;
        public ViewMode Mode
        {
            get => this.mode;
            set
            {
                if (this.mode != value)
                {
                    this.PreviousMode = this.mode;
                    this.mode = value;
                    SetDisplayNormalAlwaysOnTopOption();
                    OnModeChanged();
                    RaisePropertyChanged();
                }
            }
        }

        private ViewMode previousMode = ViewMode.None;
        public ViewMode PreviousMode
        {
            get => this.previousMode;
            set
            {
                if (this.previousMode != value)
                {
                    this.previousMode = value;
                    RaisePropertyChanged();
                }
            }
        }

        public IList<NavCategoryGroup> Categories => NavCategoryStates.CreateMenuOptions();

        private string categoryName;
        public string CategoryName
        {
            get => this.categoryName;
            private set
            {
                if (this.categoryName != value)
                {
                    this.categoryName = value;
                    RaisePropertyChanged();
                }
            }
        }

        public Visibility ClearMemoryVisibility
        {
            get => NavCategory.IsCalculatorViewMode(Mode) ? Visibility.Visible : Visibility.Collapsed;
        }

        private bool isAlwaysOnTop;
        public bool IsAlwaysOnTop
        {
            get => this.isAlwaysOnTop;
            private set
            {
                if (this.isAlwaysOnTop != value)
                {
                    this.isAlwaysOnTop = value;
                    RaisePropertyChanged();
                }
            }
        }

        private bool displayNormalAlwaysOnTopOption;
        // Indicates whether calculator is currently in standard mode _and_ supports CompactOverlay _and_ is not in Always-on-Top mode
        public bool DisplayNormalAlwaysOnTopOption
        {
            get => this.displayNormalAlwaysOnTopOption;
            private set
            {
                if (this.displayNormalAlwaysOnTopOption != value)
                {
                    this.displayNormalAlwaysOnTopOption = value;
                    RaisePropertyChanged();
                }
            }
        }

        public void Initialize(ViewMode mode)
        {
            if (!NavCategoryStates.IsValidViewMode(mode) || !NavCategoryStates.IsViewModeEnabled(mode))
            {
                mode = ViewMode.Standard;
            }

            try
            {
                this.Mode = mode;
            }
            catch (Exception e)
            {
                TraceLogger.GetInstance().LogError(mode, "ApplicationViewModel::Initialize", e.Message);
                if (!TryRecoverFromNavigationModeFailure())
                {
                    // Could not navigate to standard mode either.
                    // Throw the original exception so we have a good stack to debug.
                    throw;
                }
            }
        }

        public ICommand CopyCommand => new CommandWrapper(this.Copy);
        public ICommand PasteCommand => new CommandWrapper(this.Paste);

        public void Copy()
        {
            if (NavCategory.IsConverterViewMode(this.mode))
            {
                ConverterViewModel.OnCopyCommand(null);
            }
            else if (NavCategory.IsDateCalculatorViewMode(this.mode))
            {
                DateCalcViewModel.OnCopyCommand(null);
            }
            else if (NavCategory.IsCalculatorViewMode(this.mode))
            {
                CalculatorViewModel.OnCopyCommand(null);
            }
        }

        public void Paste()
        {
            if (NavCategory.IsConverterViewMode(this.mode))
            {
                ConverterViewModel.OnPasteCommand(null);
            }
            else if (NavCategory.IsCalculatorViewMode(this.mode))
            {
                CalculatorViewModel.OnPasteCommand(null);
            }
        }

        public async void ToggleAlwaysOnTop(float width, float height)
        {
            if (ApplicationView.GetForCurrentView().ViewMode == ApplicationViewMode.CompactOverlay)
            {
                ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;
                localSettings.Values[WidthLocalSettings] = width;
                localSettings.Values[HeightLocalSettings] = height;

                bool success = await ApplicationView.GetForCurrentView().TryEnterViewModeAsync(ApplicationViewMode.Default);
                CalculatorViewModel.HistoryVM.AreHistoryShortcutsEnabled = success;
                CalculatorViewModel.IsAlwaysOnTop = !success;
                this.IsAlwaysOnTop = !success;
            }
            else
            {
                ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;
                ViewModePreferences compactOptions = ViewModePreferences.CreateDefault(ApplicationViewMode.CompactOverlay);
                if (!localSettings.Values.ContainsKey(AlwaysOnTopLaunchedLocalSettings))
                {
                    compactOptions.CustomSize = new Size(320, 394);
                    localSettings.Values[AlwaysOnTopLaunchedLocalSettings] = true;
                }
                else
                {
                    if (localSettings.Values.TryGetValue(WidthLocalSettings, out object widthSetting) && localSettings.Values.TryGetValue(HeightLocalSettings, out object heightSetting))
                    {
                        float oldWidth = (float)widthSetting;
                        float oldHeight = (float)heightSetting;
                        compactOptions.CustomSize = new Size(oldWidth, oldHeight);
                    }
                    else
                    {
                        compactOptions.CustomSize = new Size(320, 394);
                    }
                }

                bool success = await ApplicationView.GetForCurrentView().TryEnterViewModeAsync(ApplicationViewMode.CompactOverlay, compactOptions);
                CalculatorViewModel.HistoryVM.AreHistoryShortcutsEnabled = !success;
                CalculatorViewModel.IsAlwaysOnTop = success;
                this.IsAlwaysOnTop = success;
            }
            SetDisplayNormalAlwaysOnTopOption();
        }

        private void OnModeChanged()
        {
            Debug.Assert(NavCategoryStates.IsValidViewMode(this.mode));
            if (NavCategory.IsCalculatorViewMode(this.mode))
            {
                if (this.CalculatorViewModel is null)
                {
                    this.CalculatorViewModel = new StandardCalculatorViewModel();
                }
                this.CalculatorViewModel.SetCalculatorType(this.mode);
            }
            else if (NavCategory.IsGraphingCalculatorViewMode(this.mode))
            {
                if (this.GraphingCalcViewModel is null)
                {
                    this.GraphingCalcViewModel = new GraphingCalculatorViewModel();
                }
            }
            else if (NavCategory.IsDateCalculatorViewMode(this.mode))
            {
                if (this.DateCalcViewModel is null)
                {
                    this.DateCalcViewModel = new DateCalculatorViewModel();
                }
            }
            else if (NavCategory.IsConverterViewMode(this.mode))
            {
                if (this.ConverterViewModel is null)
                {
                    this.ConverterViewModel = new UnitConverterViewModel();
                }

                this.ConverterViewModel.Mode = this.mode;
            }

            var resProvider = AppResourceProvider.GetInstance();
            this.CategoryName = resProvider.GetResourceString(NavCategoryStates.GetNameResourceKey(this.mode));

            // Cast mode to an int in order to save it to app data.
            // Save the changed mode, so that the new window launches in this mode.
            // Don't save until after we have adjusted to the new mode, so we don't save a mode that fails to load.
            ApplicationData.Current.LocalSettings.Values[ModeLocalSettings] = NavCategoryStates.Serialize(this.mode);

            // Log ModeChange event when not first launch, log WindowCreated on first launch
            if (NavCategoryStates.IsValidViewMode(this.PreviousMode))
            {
                TraceLogger.GetInstance().LogModeChange(this.mode);
            }
            else
            {
                TraceLogger.GetInstance().LogWindowCreated(this.mode, ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread()));
            }

            RaisePropertyChanged(nameof(this.ClearMemoryVisibility));
        }

        private bool TryRecoverFromNavigationModeFailure()
        {
            // Here we are simply trying to recover from being unable to navigate to a mode.
            // Try falling back to standard mode and if there are *any* exceptions, we should
            // fail because something is seriously wrong.
            try
            {
                this.Mode = ViewMode.Standard;
                return true;
            }
            catch
            {
                return false;
            }
        }

        private void SetDisplayNormalAlwaysOnTopOption()
        {
            this.DisplayNormalAlwaysOnTopOption =
                this.mode == ViewMode.Standard && ApplicationView.GetForCurrentView().IsViewModeSupported(ApplicationViewMode.CompactOverlay) && !this.IsAlwaysOnTop;
        }

        private void RaisePropertyChanged([CallerMemberName] string propertyName = null)
        {
            this.PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
