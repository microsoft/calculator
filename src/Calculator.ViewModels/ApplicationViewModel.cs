// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Threading.Tasks;
using System.Windows.Input;

using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;

using Windows.Foundation;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;

using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;
using CalculatorApp.ViewModel.Snapshot;

namespace CalculatorApp.ViewModel
{
    public partial class ApplicationViewModel : ObservableObject
    {
        [ObservableProperty]
        private StandardCalculatorViewModel _calculatorViewModel;

        [ObservableProperty]
        private DateCalculatorViewModel _dateCalcViewModel;

        [ObservableProperty]
        private GraphingCalculatorViewModel _graphingCalcViewModel;

        [ObservableProperty]
        private UnitConverterViewModel _converterViewModel;

        private ViewMode _mode = ViewMode.None;

        [ObservableProperty]
        private ViewMode _previousMode = ViewMode.None;

        private bool _isAlwaysOnTop = false;
        private bool _displayNormalAlwaysOnTopOption;

        [ObservableProperty]
        private string _categoryName;

        [ObservableProperty]
        private IList<NavCategoryGroup> _categories = NavCategoryStates.CreateMenuOptions();

        public const string WidthLocalSettingsKey = "calculatorAlwaysOnTopLastWidth";
        public const string HeightLocalSettingsKey = "calculatorAlwaysOnTopLastHeight";

        public ViewMode Mode
        {
            get => _mode;
            set
            {
                if (_mode != value)
                {
                    PreviousMode = _mode;
                    _mode = value;
                    SetDisplayNormalAlwaysOnTopOption();
                    OnModeChanged();
                    OnPropertyChanged(nameof(Mode));
                }
            }
        }

        public ICommand CopyCommand => new RelayCommand<object>(OnCopyCommand);

        public ICommand PasteCommand => new RelayCommand<object>(OnPasteCommand);

        public Visibility ClearMemoryVisibility
        {
            get => NavCategory.IsCalculatorViewMode(_mode) ?
                Visibility.Visible : Visibility.Collapsed;
        }

        public ApplicationSnapshot Snapshot
        {
            get
            {
                var snapshot = new ApplicationSnapshot();
                snapshot.Mode = (int)_mode;
                if (CalculatorViewModel != null)
                {
                    snapshot.StandardCalculator = CalculatorViewModel.Snapshot;
                }
                return snapshot;
            }
        }

        public bool IsAlwaysOnTop
        {
            get => _isAlwaysOnTop;
            private set => SetProperty(ref _isAlwaysOnTop, value);
        }

        // Indicates whether calculator is currently in standard mode _and_ supports CompactOverlay _and_ is not in Always-on-Top mode
        public bool DisplayNormalAlwaysOnTopOption
        {
            get => _displayNormalAlwaysOnTopOption;
            private set => SetProperty(ref _displayNormalAlwaysOnTopOption, value);
        }

        public async Task ToggleAlwaysOnTop(double width, double height)
        {
            var DefaultSize = new Size(320, 394);
            const string LaunchedSettingsKey = "calculatorAlwaysOnTopLaunched";
            var settings = ApplicationData.Current.LocalSettings;
            if (ApplicationView.GetForCurrentView().ViewMode == ApplicationViewMode.CompactOverlay)
            {
                settings.Values[WidthLocalSettingsKey] = width;
                settings.Values[HeightLocalSettingsKey] = height;
                bool success = await ApplicationView.GetForCurrentView().TryEnterViewModeAsync(ApplicationViewMode.Default);
                CalculatorViewModel.HistoryVM.AreHistoryShortcutsEnabled = success;
                CalculatorViewModel.IsAlwaysOnTop = !success;
                IsAlwaysOnTop = !success;
            }
            else
            {
                var compactOptions = ViewModePreferences.CreateDefault(ApplicationViewMode.CompactOverlay);
                if (!settings.Values.ContainsKey(LaunchedSettingsKey))
                {
                    compactOptions.CustomSize = DefaultSize;
                    settings.Values[LaunchedSettingsKey] = true;
                }
                else
                {
                    try
                    {
                        if (settings.Values.TryGetValue(WidthLocalSettingsKey, out var oldWidth) &&
                            settings.Values.TryGetValue(HeightLocalSettingsKey, out var oldHeight))
                        {
                            compactOptions.CustomSize = new Size((double)oldWidth, (double)oldHeight);
                        }
                        else
                        {
                            compactOptions.CustomSize = DefaultSize;
                        }
                    }
                    catch (InvalidCastException)
                    {
                        compactOptions.CustomSize = DefaultSize;
                    }
                }
                bool success = await ApplicationView.GetForCurrentView().TryEnterViewModeAsync(ApplicationViewMode.CompactOverlay, compactOptions);
                CalculatorViewModel.HistoryVM.AreHistoryShortcutsEnabled = !success;
                CalculatorViewModel.IsAlwaysOnTop = success;
                IsAlwaysOnTop = success;
            }
            SetDisplayNormalAlwaysOnTopOption();
        }

        public void Initialize(ViewMode mode)
        {
            if (!NavCategoryStates.IsValidViewMode(mode) || !NavCategoryStates.IsViewModeEnabled(mode))
            {
                mode = ViewMode.Standard;
            }
            try
            {
                Mode = mode;
            }
            catch (Exception e)
            {
                TraceLogger.GetInstance().LogError(mode, nameof(Initialize), e.Message);
                if (!TryRecoverFromNavigationModeFailure())
                {
                    // Could not navigate to standard mode either.
                    // Throw the original exception so we have a good stack to debug.
                    throw;
                }
            }
        }

        public void RestoreFromSnapshot(ApplicationSnapshot snapshot)
        {
            Mode = (ViewMode)snapshot.Mode;
            if (snapshot.StandardCalculator != null)
            {
                CalculatorViewModel.Snapshot = snapshot.StandardCalculator;
            }
        }

        private void OnModeChanged()
        {
            Debug.Assert(NavCategoryStates.IsValidViewMode(_mode));
            if (NavCategory.IsCalculatorViewMode(_mode))
            {
                if (CalculatorViewModel == null)
                {
                    CalculatorViewModel = new StandardCalculatorViewModel();
                }
                CalculatorViewModel.SetCalculatorType(_mode);
            }
            else if (NavCategory.IsGraphingCalculatorViewMode(_mode))
            {
                if (GraphingCalcViewModel == null)
                {
                    GraphingCalcViewModel = new GraphingCalculatorViewModel();
                }
            }
            else if (NavCategory.IsDateCalculatorViewMode(_mode))
            {
                if (DateCalcViewModel == null)
                {
                    DateCalcViewModel = new DateCalculatorViewModel();
                }
            }
            else if (NavCategory.IsConverterViewMode(_mode))
            {
                if (ConverterViewModel == null)
                {
                    ConverterViewModel = new UnitConverterViewModel();
                }
                ConverterViewModel.Mode = _mode;
            }

            var resProvider = AppResourceProvider.GetInstance();
            CategoryName = resProvider.GetResourceString(NavCategoryStates.GetNameResourceKey(_mode));

            // Cast mode to an int in order to save it to app data.
            // Save the changed mode, so that the new window launches in this mode.
            // Don't save until after we have adjusted to the new mode, so we don't save a mode that fails to load.
            ApplicationData.Current.LocalSettings.Values[nameof(Mode)] = NavCategoryStates.Serialize(_mode);

            // Log ModeChange event when not first launch, log WindowCreated on first launch
            if (NavCategoryStates.IsValidViewMode(PreviousMode))
            {
                TraceLogger.GetInstance().LogModeChange(_mode);
            }
            else
            {
                TraceLogger.GetInstance().LogWindowCreated(
                    _mode,
                    ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread()));
            }
            OnPropertyChanged(nameof(ClearMemoryVisibility));
        }

        private void OnCopyCommand(object param)
        {
            if (NavCategory.IsConverterViewMode(_mode))
            {
                ConverterViewModel.OnCopyCommand(param);
            }
            else if (NavCategory.IsDateCalculatorViewMode(_mode))
            {
                DateCalcViewModel.OnCopyCommand(param);
            }
            else if (NavCategory.IsCalculatorViewMode(_mode))
            {
                CalculatorViewModel.OnCopyCommand(param);
            }
        }

        private void OnPasteCommand(object param)
        {
            if (NavCategory.IsConverterViewMode(_mode))
            {
                ConverterViewModel.OnPasteCommand(param);
            }
            else if (NavCategory.IsCalculatorViewMode(_mode))
            {
                CalculatorViewModel.OnPasteCommand(param);
            }
        }

        private void SetDisplayNormalAlwaysOnTopOption()
        {
            DisplayNormalAlwaysOnTopOption = _mode == ViewMode.Standard &&
                ApplicationView.GetForCurrentView().IsViewModeSupported(ApplicationViewMode.CompactOverlay) &&
                !_isAlwaysOnTop;
        }

        private bool TryRecoverFromNavigationModeFailure()
        {
            // Here we are simply trying to recover from being unable to navigate to a mode.
            // Try falling back to standard mode and if there are *any* exceptions, we should
            // fail because something is seriously wrong.
            try
            {
                Mode = ViewMode.Standard;
                return true;
            }
            catch
            {
                return false;
            }
        }
    }
}
