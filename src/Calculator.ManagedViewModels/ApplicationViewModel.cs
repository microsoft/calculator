// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;
using System.Windows.Input;

using Windows.Foundation;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;

using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;
using CalculatorApp.ViewModel.Snapshot;

namespace CalculatorApp.ManagedViewModels
{
    public class ApplicationViewModel : INotifyPropertyChanged
    {
        private StandardCalculatorViewModel _calcVm;
        private DateCalculatorViewModel _dateVm;
        private GraphingCalculatorViewModel _graphVm;
        private UnitConverterViewModel _unitVm;
        private ViewMode _mode = ViewMode.None;
        private ViewMode _previousMode = ViewMode.None;
        private bool _isAlwaysOnTop = false;
        private bool _displayNormalAlwaysOnTopOption;
        private string _categoryName;
        private IList<NavCategoryGroup> _categories = NavCategoryStates.CreateMenuOptions();

        public const string WidthLocalSettingsKey = "calculatorAlwaysOnTopLastWidth";
        public const string HeightLocalSettingsKey = "calculatorAlwaysOnTopLastHeight";

        public event PropertyChangedEventHandler PropertyChanged;

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
                    RaisePropertyChanged();
                }
            }
        }

        public ICommand CopyCommand => new RelayCommand(OnCopyCommand);

        public ICommand PasteCommand => new RelayCommand(OnPasteCommand);

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
                if (_calcVm != null)
                {
                    snapshot.StandardCalculator = _calcVm.Snapshot;
                }
                return snapshot;
            }
        }

        public StandardCalculatorViewModel CalculatorViewModel
        {
            get => _calcVm;
            set
            {
                if (_calcVm != value)
                {
                    _calcVm = value;
                    RaisePropertyChanged();
                }
            }
        }

        public DateCalculatorViewModel DateCalcViewModel
        {
            get => _dateVm;
            set
            {
                if (_dateVm != value)
                {
                    _dateVm = value;
                    RaisePropertyChanged();
                }
            }
        }

        public GraphingCalculatorViewModel GraphingCalcViewModel
        {
            get => _graphVm;
            set
            {
                if (_graphVm != value)
                {
                    _graphVm = value;
                    RaisePropertyChanged();
                }
            }
        }

        public UnitConverterViewModel ConverterViewModel
        {
            get => _unitVm;
            set
            {
                if (_unitVm != value)
                {
                    _unitVm = value;
                    RaisePropertyChanged();
                }
            }
        }

        public ViewMode PreviousMode
        {
            get => _previousMode;
            set
            {
                if (_previousMode != value)
                {
                    _previousMode = value;
                    RaisePropertyChanged();
                }
            }
        }

        public bool IsAlwaysOnTop
        {
            get => _isAlwaysOnTop;
            private set
            {
                if (_isAlwaysOnTop != value)
                {
                    _isAlwaysOnTop = value;
                    RaisePropertyChanged();
                }
            }
        }

        // Indicates whether calculator is currently in standard mode _and_ supports CompactOverlay _and_ is not in Always-on-Top mode
        public bool DisplayNormalAlwaysOnTopOption
        {
            get => _displayNormalAlwaysOnTopOption;
            private set
            {
                if (_displayNormalAlwaysOnTopOption != value)
                {
                    _displayNormalAlwaysOnTopOption = value;
                    RaisePropertyChanged();
                }
            }
        }

        public string CategoryName
        {
            get => _categoryName;
            set
            {
                if (_categoryName != value)
                {
                    _categoryName = value;
                    RaisePropertyChanged();
                }
            }
        }

        public IList<NavCategoryGroup> Categories
        {
            get => _categories;
            set
            {
                if (_categories != value)
                {
                    _categories = value;
                    RaisePropertyChanged();
                }
            }
        }

        public async Task ToggleAlwaysOnTop(float width, float height)
        {
            var DefaultSize = new Size(320, 394);
            const string LaunchedSettingsKey = "calculatorAlwaysOnTopLaunched";
            var settings = ApplicationData.Current.LocalSettings;
            if (ApplicationView.GetForCurrentView().ViewMode == ApplicationViewMode.CompactOverlay)
            {
                settings.Values[WidthLocalSettingsKey] = width;
                settings.Values[HeightLocalSettingsKey] = height;
                bool success = await ApplicationView.GetForCurrentView().TryEnterViewModeAsync(ApplicationViewMode.Default);
                _calcVm.HistoryVM.AreHistoryShortcutsEnabled = success;
                _calcVm.IsAlwaysOnTop = !success;
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
                    if (settings.Values.TryGetValue(WidthLocalSettingsKey, out var oldWidth) &&
                        settings.Values.TryGetValue(HeightLocalSettingsKey, out var oldHeight))
                    {
                        compactOptions.CustomSize = new Size((float)oldWidth, (float)oldHeight);
                    }
                    else
                    {
                        compactOptions.CustomSize = DefaultSize;
                    }
                }
                bool success = await ApplicationView.GetForCurrentView().TryEnterViewModeAsync(ApplicationViewMode.CompactOverlay, compactOptions);
                _calcVm.HistoryVM.AreHistoryShortcutsEnabled = !success;
                _calcVm.IsAlwaysOnTop = success;
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
                _calcVm.Snapshot = snapshot.StandardCalculator;
            }
        }

        private void OnModeChanged()
        {
            Debug.Assert(NavCategoryStates.IsValidViewMode(_mode));
            if (NavCategory.IsCalculatorViewMode(_mode))
            {
                if (_calcVm == null)
                {
                    _calcVm = new StandardCalculatorViewModel();
                }
                _calcVm.SetCalculatorType(_mode);
            }
            else if (NavCategory.IsGraphingCalculatorViewMode(_mode))
            {
                if (_graphVm == null)
                {
                    _graphVm = new GraphingCalculatorViewModel();
                }
            }
            else if (NavCategory.IsDateCalculatorViewMode(_mode))
            {
                if (_dateVm == null)
                {
                    _dateVm = new DateCalculatorViewModel();
                }
            }
            else if (NavCategory.IsConverterViewMode(_mode))
            {
                if (_unitVm == null)
                {
                    _unitVm = new UnitConverterViewModel();
                }
                _unitVm.Mode = _mode;
            }

            var resProvider = AppResourceProvider.GetInstance();
            CategoryName = resProvider.GetResourceString(NavCategoryStates.GetNameResourceKey(_mode));

            // Cast mode to an int in order to save it to app data.
            // Save the changed mode, so that the new window launches in this mode.
            // Don't save until after we have adjusted to the new mode, so we don't save a mode that fails to load.
            ApplicationData.Current.LocalSettings.Values[nameof(Mode)] = NavCategoryStates.Serialize(_mode);

            // Log ModeChange event when not first launch, log WindowCreated on first launch
            if (NavCategoryStates.IsValidViewMode(_previousMode))
            {
                TraceLogger.GetInstance().LogModeChange(_mode);
            }
            else
            {
                TraceLogger.GetInstance().LogWindowCreated(
                    _mode,
                    ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread()));
            }
            RaisePropertyChanged(nameof(ClearMemoryVisibility));
        }

        private void OnCopyCommand(object param)
        {
            if (NavCategory.IsConverterViewMode(_mode))
            {
                _unitVm.OnCopyCommand(param);
            }
            else if (NavCategory.IsDateCalculatorViewMode(_mode))
            {
                _dateVm.OnCopyCommand(param);
            }
            else if (NavCategory.IsCalculatorViewMode(_mode))
            {
                _calcVm.OnCopyCommand(param);
            }
        }

        private void OnPasteCommand(object param)
        {
            if (NavCategory.IsConverterViewMode(_mode))
            {
                _unitVm.OnPasteCommand(param);
            }
            else if (NavCategory.IsCalculatorViewMode(_mode))
            {
                _calcVm.OnPasteCommand(param);
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

        private void RaisePropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
