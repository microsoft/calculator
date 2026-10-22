// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CommunityToolkit.Mvvm.ComponentModel;

namespace CalculatorApp.DesignData
{
    public sealed class AppViewModel : ObservableObject
    {
        private StandardCalculatorViewModel _calculatorViewModel;
        private UnitConverterViewModel _converterViewModel;
        private bool _isStandardMode;
        private bool _isScientificMode;
        private bool _isConverterMode;

        public AppViewModel()
        {
            _isStandardMode = true;
            _isScientificMode = false;
            _isConverterMode = false;
            _calculatorViewModel = new StandardCalculatorViewModel();
            _converterViewModel = new UnitConverterViewModel();
        }

        public StandardCalculatorViewModel CalculatorViewModel
        {
            get => _calculatorViewModel;
            set => SetProperty(ref _calculatorViewModel, value);
        }

        public UnitConverterViewModel ConverterViewModel
        {
            get => _converterViewModel;
            set => SetProperty(ref _converterViewModel, value);
        }

        public bool IsStandardMode
        {
            get => _isStandardMode;
            set => SetProperty(ref _isStandardMode, value);
        }

        public bool IsScientificMode
        {
            get => _isScientificMode;
            set => SetProperty(ref _isScientificMode, value);
        }

        public bool IsConverterMode
        {
            get => _isConverterMode;
            set => SetProperty(ref _isConverterMode, value);
        }
    }
}

namespace Numbers.DesignData
{
    public sealed class AppViewModel : ObservableObject
    {
        private StandardCalculatorViewModel _calculatorViewModel;
        private UnitConverterViewModel _converterViewModel;
        private bool _isStandardMode;
        private bool _isScientificMode;
        private bool _isConverterMode;

        public AppViewModel()
        {
            _isStandardMode = true;
            _isScientificMode = false;
            _isConverterMode = false;
            _calculatorViewModel = new StandardCalculatorViewModel();
            _converterViewModel = new UnitConverterViewModel();
        }

        public StandardCalculatorViewModel CalculatorViewModel
        {
            get => _calculatorViewModel;
            set => SetProperty(ref _calculatorViewModel, value);
        }

        public UnitConverterViewModel ConverterViewModel
        {
            get => _converterViewModel;
            set => SetProperty(ref _converterViewModel, value);
        }

        public bool IsStandardMode
        {
            get => _isStandardMode;
            set => SetProperty(ref _isStandardMode, value);
        }

        public bool IsScientificMode
        {
            get => _isScientificMode;
            set => SetProperty(ref _isScientificMode, value);
        }

        public bool IsConverterMode
        {
            get => _isConverterMode;
            set => SetProperty(ref _isConverterMode, value);
        }
    }
}
