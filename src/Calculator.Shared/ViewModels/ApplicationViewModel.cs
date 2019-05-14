// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp;
using CalculatorApp.Common;
using CalculatorApp.ViewModel;
using CalculationManager;
using Windows.System;
using Windows.Storage;
using Windows.Foundation.Collections;
using Windows.Globalization;
using Windows.UI.ViewManagement;
using Windows.UI.Core;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using System.ComponentModel;
using System.Windows.Input;
using System;
using System.Diagnostics;
using System.Collections.ObjectModel;

namespace CalculatorApp.ViewModel
{
    public class ApplicationViewModel : INotifyPropertyChanged
    {
        string CategoriesPropertyName = "Categories";
        string ClearMemoryVisibilityPropertyName = "ClearMemoryVisibility";


        public event PropertyChangedEventHandler PropertyChanged;
        private void RaisePropertyChanged([System.Runtime.CompilerServices.CallerMemberName]string p = null) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(p));
        private StandardCalculatorViewModel m_CalculatorViewModel;
        public StandardCalculatorViewModel CalculatorViewModel { get => m_CalculatorViewModel; set { m_CalculatorViewModel = value; RaisePropertyChanged("CalculatorViewModel"); } }

        // UNO TODO
        //private DateCalculatorViewModel m_DateCalcViewModel;
        //public DateCalculatorViewModel DateCalcViewModel { get => m_DateCalcViewModel; set { m_DateCalcViewModel = value; RaisePropertyChanged("DateCalcViewModel"); } }


        // UNO TODO
        //private UnitConverterViewModel m_ConverterViewModel;
        //public UnitConverterViewModel ConverterViewModel { get => m_ConverterViewModel; set { m_ConverterViewModel = value; RaisePropertyChanged("ConverterViewModel"); } }


        private CalculatorApp.Common.ViewMode m_PreviousMode;
        public CalculatorApp.Common.ViewMode PreviousMode { get => m_PreviousMode; set { m_PreviousMode = value; RaisePropertyChanged("PreviousMode"); } }


        private const string CategoryNameName = "CategoryName";
        private string m_CategoryName;
        public string CategoryName { get => m_CategoryName; set { m_CategoryName = value; RaisePropertyChanged("CategoryName"); } }


        public ICommand CopyCommand { get; }


        public ICommand PasteCommand { get; }




        ViewMode m_mode = ViewMode.None;
        ObservableCollection<NavCategoryGroup> m_categories;

        public ApplicationViewModel()
        {
            CopyCommand = new DelegateCommand(OnCopyCommand);
            PasteCommand  = new DelegateCommand(OnPasteCommand);

            SetMenuCategories();
        }

        public ViewMode Mode
        {
            get => m_mode;
            set
            {
                if (m_mode != value)
                {
                    PreviousMode = m_mode;
                    m_mode = value;
                    OnModeChanged();
                    RaisePropertyChanged();
                }
            }
        }

        public ObservableCollection<NavCategoryGroup> Categories
        {
            get => m_categories;
            set
            {
                if (m_categories != value)
                {
                    m_categories = value;
                    RaisePropertyChanged(CategoriesPropertyName);
                }
            }
        }

        public void Initialize(ViewMode mode)
        {
            if (!NavCategory.IsValidViewMode(mode))
            {
                mode = ViewMode.Standard;
            }

            try
            {
                Mode = mode;
            }
            catch (Exception e)
            {
                // TraceLogger.GetInstance().LogPlatformException(__FUNCTIONW__, e);
                if (!TryRecoverFromNavigationModeFailure())
                {
                    // Could not navigate to standard mode either.
                    // Throw the original exception so we have a good stack to debug.
                    throw;
                }
            }
        }

        bool TryRecoverFromNavigationModeFailure()
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

        void OnModeChanged()
        {
            Debug.Assert(NavCategory.IsValidViewMode(m_mode));
            // TraceLogger.GetInstance().LogModeChangeBegin(m_PreviousMode, m_mode, ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread()));
            if (NavCategory.IsCalculatorViewMode(m_mode))
            {
                // TraceLogger.GetInstance().LogCalculatorModeViewed(m_mode, ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread()));
                if (m_CalculatorViewModel == null)
                {
                    m_CalculatorViewModel = new StandardCalculatorViewModel();
                }
                m_CalculatorViewModel.SetCalculatorType(m_mode);
            }
            else if (NavCategory.IsDateCalculatorViewMode(m_mode))
            {
                // TraceLogger.GetInstance().LogDateCalculatorModeViewed(m_mode, ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread()));
                // UNO TODO
                //if (m_DateCalcViewModel == null)
                //{
                //    m_DateCalcViewModel = new DateCalculatorViewModel();
                //}
            }
            else if (NavCategory.IsConverterViewMode(m_mode))
            {
                // UNO TODO
                //// TraceLogger.GetInstance().LogConverterModeViewed(m_mode, ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread()));
                //if (!m_ConverterViewModel)
                //{
                //    var dataLoader = new UnitConverterDataLoader(new GeographicRegion());
                //    var currencyDataLoader = new CurrencyDataLoader(new CurrencyHttpClient());
                //    m_ConverterViewModel = new UnitConverterViewModel(new UnitConversionManager.UnitConverter(dataLoader, currencyDataLoader));
                //}

                //m_ConverterViewModel.Mode = m_mode;
            }

            var resProvider = AppResourceProvider.GetInstance();
            CategoryName = resProvider.GetResourceString(NavCategory.GetNameResourceKey(m_mode));

            // This is the only place where a ViewMode enum should be cast to an int.
            //
            // Save the changed mode, so that the new window launches in this mode.
            // Don't save until after we have adjusted to the new mode, so we don't save a mode that fails to load.
            ApplicationData.Current.LocalSettings.Values[nameof(Mode)] = NavCategory.Serialize(m_mode);

            // TraceLogger.GetInstance().LogModeChangeEnd(m_mode, ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread()));
            RaisePropertyChanged(ClearMemoryVisibilityPropertyName);
        }

        void OnCopyCommand(object parameter)
        {
            // UNO TODO
            //if (NavCategory.IsConverterViewMode(m_mode))
            //{
            //    ConverterViewModel.OnCopyCommand(parameter);
            //}
            //else if (NavCategory.IsDateCalculatorViewMode(m_mode))
            //{
            //    DateCalcViewModel.OnCopyCommand(parameter);
            //}
            //else
            {
                CalculatorViewModel.OnCopyCommand(parameter);
            }
        }

        void OnPasteCommand(object parameter)
        {
            // UNO TODO
            //if (NavCategory.IsConverterViewMode(m_mode))
            //{
            //    ConverterViewModel.OnPasteCommand(parameter);
            //}
            //else 
            if (NavCategory.IsCalculatorViewMode(m_mode))
            {
                CalculatorViewModel.OnPasteCommand(parameter);
            }
        }

        void SetMenuCategories()
        {
            // Use the Categories property instead of the backing variable
            // because we want to take advantage of binding updates and
            // property setter logic.
            Categories = NavCategoryGroup.CreateMenuOptions();
        }
    }
}