// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.ComponentModel;
using System.Globalization;
using CalculatorApp.ViewModel.Common;
using CommunityToolkit.Mvvm.ComponentModel;

namespace CalculatorApp.ViewModel
{
    [Windows.UI.Xaml.Data.Bindable]
    public sealed partial class GraphingSettingsViewModel : ObservableObject
    {
        private const int InvalidTrigUnit = 0;
        private const int RadiansTrigUnit = 1;
        private const int DegreesTrigUnit = 2;
        private const int GradiansTrigUnit = 3;

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

        public bool XError => !XMinError && !XMaxError && _xMinValue >= _xMaxValue;
        public bool YError => !YMinError && !YMaxError && _yMinValue >= _yMaxValue;

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
                    if (Graph != null)
                    {
                        Graph.XAxisMin = number;
                    }
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
                    if (Graph != null)
                    {
                        Graph.XAxisMax = number;
                    }
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
                    if (Graph != null)
                    {
                        Graph.YAxisMin = number;
                    }
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
                    if (Graph != null)
                    {
                        Graph.YAxisMax = number;
                    }
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
            get => Graph == null ? InvalidTrigUnit : Graph.TrigUnitMode;
            set
            {
                if (Graph == null)
                {
                    return;
                }

                Graph.TrigUnitMode = value;
                OnPropertyChanged(nameof(TrigUnit));
            }
        }

        public bool TrigModeRadians
        {
            get => Graph != null && Graph.TrigUnitMode == RadiansTrigUnit;
            set
            {
                if (value && Graph != null && Graph.TrigUnitMode != RadiansTrigUnit)
                {
                    Graph.TrigUnitMode = RadiansTrigUnit;
                    OnPropertyChanged(nameof(TrigModeRadians));
                    OnPropertyChanged(nameof(TrigModeDegrees));
                    OnPropertyChanged(nameof(TrigModeGradians));
                    TraceLogger.GetInstance().LogGraphSettingsChanged(Common.GraphSettingsType.TrigUnits, "Radians");
                }
            }
        }

        public bool TrigModeDegrees
        {
            get => Graph != null && Graph.TrigUnitMode == DegreesTrigUnit;
            set
            {
                if (value && Graph != null && Graph.TrigUnitMode != DegreesTrigUnit)
                {
                    Graph.TrigUnitMode = DegreesTrigUnit;
                    OnPropertyChanged(nameof(TrigModeDegrees));
                    OnPropertyChanged(nameof(TrigModeRadians));
                    OnPropertyChanged(nameof(TrigModeGradians));
                    TraceLogger.GetInstance().LogGraphSettingsChanged(Common.GraphSettingsType.TrigUnits, "Degrees");
                }
            }
        }

        public bool TrigModeGradians
        {
            get => Graph != null && Graph.TrigUnitMode == GradiansTrigUnit;
            set
            {
                if (value && Graph != null && Graph.TrigUnitMode != GradiansTrigUnit)
                {
                    Graph.TrigUnitMode = GradiansTrigUnit;
                    OnPropertyChanged(nameof(TrigModeGradians));
                    OnPropertyChanged(nameof(TrigModeDegrees));
                    OnPropertyChanged(nameof(TrigModeRadians));
                    TraceLogger.GetInstance().LogGraphSettingsChanged(Common.GraphSettingsType.TrigUnits, "Gradians");
                }
            }
        }

        #endregion

        public void UpdateDisplayRange()
        {
            if (Graph == null || _dontUpdateDisplayRange || HasError())
            {
                return;
            }

            Graph.SetDisplayRanges(_xMinValue, _xMaxValue, _yMinValue, _yMaxValue);
            TraceLogger.GetInstance().LogGraphSettingsChanged(Common.GraphSettingsType.Grid, string.Empty);
        }

        public void SetGrapher(GraphControl.Grapher grapher)
        {
            if (grapher != null && grapher.TrigUnitMode == InvalidTrigUnit)
            {
                grapher.TrigUnitMode = RadiansTrigUnit;
            }

            Graph = grapher;
            InitRanges();
            OnPropertyChanged(nameof(TrigUnit));
            OnPropertyChanged(nameof(TrigModeRadians));
            OnPropertyChanged(nameof(TrigModeDegrees));
            OnPropertyChanged(nameof(TrigModeGradians));
        }

        public void InitRanges()
        {
            double xMin = 0;
            double xMax = 0;
            double yMin = 0;
            double yMax = 0;
            if (Graph != null)
            {
                Graph.GetDisplayRanges(out xMin, out xMax, out yMin, out yMax);
            }

            _dontUpdateDisplayRange = true;
            _xMinValue = xMin;
            _xMaxValue = xMax;
            _yMinValue = yMin;
            _yMaxValue = yMax;

            XMin = xMin.ToString(CultureInfo.CurrentCulture);
            XMax = xMax.ToString(CultureInfo.CurrentCulture);
            YMin = yMin.ToString(CultureInfo.CurrentCulture);
            YMax = yMax.ToString(CultureInfo.CurrentCulture);
            _dontUpdateDisplayRange = false;
        }

        public void ResetView()
        {
            if (Graph == null)
            {
                return;
            }

            _dontUpdateDisplayRange = true;
            Graph.ResetGrid();
            InitRanges();
            XMinError = false;
            XMaxError = false;
            YMinError = false;
            YMaxError = false;
            _dontUpdateDisplayRange = false;

            OnPropertyChanged(nameof(XError));
            OnPropertyChanged(nameof(XMin));
            OnPropertyChanged(nameof(XMax));
            OnPropertyChanged(nameof(YError));
            OnPropertyChanged(nameof(YMin));
            OnPropertyChanged(nameof(YMax));
        }

        public bool HasError()
        {
            return XMinError || XMaxError || YMinError || YMaxError || XError || YError;
        }

    }
}
