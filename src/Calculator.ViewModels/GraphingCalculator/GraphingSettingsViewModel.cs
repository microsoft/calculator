// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.ComponentModel;
using CalculatorApp.ViewModel.Common;
using CommunityToolkit.Mvvm.ComponentModel;

namespace CalculatorApp.ViewModel
{
    [Windows.UI.Xaml.Data.Bindable]
    public sealed partial class GraphingSettingsViewModel : ObservableObject
    {
        private string _xMin;
        private string _xMax;
        private string _yMin;
        private string _yMax;
        private double _xMinValue;
        private double _xMaxValue;
        private double _yMinValue;
        private double _yMaxValue;
        private bool _dontUpdateDisplayRange;
#pragma warning disable CS0414 // Field is assigned but its value is never used
        private bool _xIsMinLastChanged;
        private bool _yIsMinLastChanged;
#pragma warning restore CS0414

        [ObservableProperty]
        private bool _yMinError;

        [ObservableProperty]
        private bool _xMinError;

        [ObservableProperty]
        private bool _xMaxError;

        [ObservableProperty]
        private bool _yMaxError;

        [ObservableProperty]
        private GraphControl.Grapher _graph;

        public GraphingSettingsViewModel()
        {
            _xMin = string.Empty;
            _xMax = string.Empty;
            _yMin = string.Empty;
            _yMax = string.Empty;
        }

        #region Computed Properties

        public bool XError => !XMinError && !XMaxError && _xMinValue >= _xMaxValue;
        public bool YError => !YMinError && !YMaxError && _yMinValue >= _yMaxValue;

        #endregion

        #region Range Properties

        public string XMin
        {
            get => _xMin;
            set
            {
                if (_xMin == value) return;
                _xMin = value;
                _xIsMinLastChanged = true;

                if (double.TryParse(value, out double number))
                {
                    _xMinValue = number;
                    XMinError = false;
                    // _graph?.SetXAxisMin(number);
                }
                else
                {
                    XMinError = true;
                }

                OnPropertyChanged(nameof(XError));
                OnPropertyChanged(nameof(XMin));
                UpdateDisplayRange();
            }
        }

        public string XMax
        {
            get => _xMax;
            set
            {
                if (_xMax == value) return;
                _xMax = value;
                _xIsMinLastChanged = false;

                if (double.TryParse(value, out double number))
                {
                    _xMaxValue = number;
                    XMaxError = false;
                }
                else
                {
                    XMaxError = true;
                }

                OnPropertyChanged(nameof(XError));
                OnPropertyChanged(nameof(XMax));
                UpdateDisplayRange();
            }
        }

        public string YMin
        {
            get => _yMin;
            set
            {
                if (_yMin == value) return;
                _yMin = value;
                _yIsMinLastChanged = true;

                if (double.TryParse(value, out double number))
                {
                    _yMinValue = number;
                    YMinError = false;
                }
                else
                {
                    YMinError = true;
                }

                OnPropertyChanged(nameof(YError));
                OnPropertyChanged(nameof(YMin));
                UpdateDisplayRange();
            }
        }

        public string YMax
        {
            get => _yMax;
            set
            {
                if (_yMax == value) return;
                _yMax = value;
                _yIsMinLastChanged = false;

                if (double.TryParse(value, out double number))
                {
                    _yMaxValue = number;
                    YMaxError = false;
                }
                else
                {
                    YMaxError = true;
                }

                OnPropertyChanged(nameof(YError));
                OnPropertyChanged(nameof(YMax));
                UpdateDisplayRange();
            }
        }

        #endregion

        #region Trig Mode Properties

        public int TrigUnit
        {
            get => 0; // Default: would query _graph.TrigUnitMode
            set
            {
                // Would set _graph.TrigUnitMode = value
                OnPropertyChanged(nameof(TrigUnit));
            }
        }

        public bool TrigModeRadians
        {
            get => false; // Would check _graph.TrigUnitMode == Radians
            set
            {
                if (value)
                {
                    OnPropertyChanged(nameof(TrigModeRadians));
                    OnPropertyChanged(nameof(TrigModeDegrees));
                    OnPropertyChanged(nameof(TrigModeGradians));
                    TraceLogger.GetInstance().LogGraphSettingsChanged(Common.GraphSettingsType.TrigUnits, "Radians");
                }
            }
        }

        public bool TrigModeDegrees
        {
            get => false;
            set
            {
                if (value)
                {
                    OnPropertyChanged(nameof(TrigModeDegrees));
                    OnPropertyChanged(nameof(TrigModeRadians));
                    OnPropertyChanged(nameof(TrigModeGradians));
                    TraceLogger.GetInstance().LogGraphSettingsChanged(Common.GraphSettingsType.TrigUnits, "Degrees");
                }
            }
        }

        public bool TrigModeGradians
        {
            get => false;
            set
            {
                if (value)
                {
                    OnPropertyChanged(nameof(TrigModeGradians));
                    OnPropertyChanged(nameof(TrigModeDegrees));
                    OnPropertyChanged(nameof(TrigModeRadians));
                    TraceLogger.GetInstance().LogGraphSettingsChanged(Common.GraphSettingsType.TrigUnits, "Gradians");
                }
            }
        }

        #endregion

        #region Public Methods

        public void UpdateDisplayRange()
        {
            if (_dontUpdateDisplayRange) return;
            // Update the graph display range
        }

        public void SetGrapher(GraphControl.Grapher grapher)
        {
            Graph = grapher;
            InitRanges();
        }

        public void InitRanges()
        {
            _dontUpdateDisplayRange = true;
            // Initialize ranges from graph
            // XMin = ...; XMax = ...; YMin = ...; YMax = ...;
            _dontUpdateDisplayRange = false;
        }

        public void ResetView()
        {
            _dontUpdateDisplayRange = true;
            // Reset to default ranges
            _dontUpdateDisplayRange = false;
            UpdateDisplayRange();
        }

        public bool HasError()
        {
            return XMinError || XMaxError || YMinError || YMaxError || XError || YError;
        }

        #endregion

    }
}
