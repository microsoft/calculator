// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.ComponentModel;
using CalculatorApp.ViewModel.Common;

namespace CalculatorApp.ViewModel
{
    [Windows.UI.Xaml.Data.Bindable]
    public sealed class GraphingSettingsViewModel : INotifyPropertyChanged
    {
        private bool _yMinError;
        private bool _xMinError;
        private bool _xMaxError;
        private bool _yMaxError;
        private GraphControl.Grapher _graph;

        private string _xMin;
        private string _xMax;
        private string _yMin;
        private string _yMax;
        private double _xMinValue;
        private double _xMaxValue;
        private double _yMinValue;
        private double _yMaxValue;
        private bool _dontUpdateDisplayRange;
        private bool _xIsMinLastChanged;
        private bool _yIsMinLastChanged;

        public event PropertyChangedEventHandler PropertyChanged;

        public GraphingSettingsViewModel()
        {
            _xMin = string.Empty;
            _xMax = string.Empty;
            _yMin = string.Empty;
            _yMax = string.Empty;
        }

        #region Observable Properties

        public bool YMinError
        {
            get => _yMinError;
            private set { if (_yMinError != value) { _yMinError = value; RaisePropertyChanged(nameof(YMinError)); } }
        }

        public bool XMinError
        {
            get => _xMinError;
            private set { if (_xMinError != value) { _xMinError = value; RaisePropertyChanged(nameof(XMinError)); } }
        }

        public bool XMaxError
        {
            get => _xMaxError;
            private set { if (_xMaxError != value) { _xMaxError = value; RaisePropertyChanged(nameof(XMaxError)); } }
        }

        public bool YMaxError
        {
            get => _yMaxError;
            private set { if (_yMaxError != value) { _yMaxError = value; RaisePropertyChanged(nameof(YMaxError)); } }
        }

        public GraphControl.Grapher Graph
        {
            get => _graph;
            private set { if (_graph != value) { _graph = value; RaisePropertyChanged(nameof(Graph)); } }
        }

        #endregion

        #region Computed Properties

        public bool XError => !_xMinError && !_xMaxError && _xMinValue >= _xMaxValue;
        public bool YError => !_yMinError && !_yMaxError && _yMinValue >= _yMaxValue;

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

                RaisePropertyChanged(nameof(XError));
                RaisePropertyChanged(nameof(XMin));
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

                RaisePropertyChanged(nameof(XError));
                RaisePropertyChanged(nameof(XMax));
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

                RaisePropertyChanged(nameof(YError));
                RaisePropertyChanged(nameof(YMin));
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

                RaisePropertyChanged(nameof(YError));
                RaisePropertyChanged(nameof(YMax));
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
                RaisePropertyChanged(nameof(TrigUnit));
            }
        }

        public bool TrigModeRadians
        {
            get => false; // Would check _graph.TrigUnitMode == Radians
            set
            {
                if (value)
                {
                    RaisePropertyChanged(nameof(TrigModeRadians));
                    RaisePropertyChanged(nameof(TrigModeDegrees));
                    RaisePropertyChanged(nameof(TrigModeGradians));
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
                    RaisePropertyChanged(nameof(TrigModeDegrees));
                    RaisePropertyChanged(nameof(TrigModeRadians));
                    RaisePropertyChanged(nameof(TrigModeGradians));
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
                    RaisePropertyChanged(nameof(TrigModeGradians));
                    RaisePropertyChanged(nameof(TrigModeDegrees));
                    RaisePropertyChanged(nameof(TrigModeRadians));
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
            _graph = grapher;
            RaisePropertyChanged(nameof(Graph));
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

        private void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
