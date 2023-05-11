// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.Common;
using CalculatorApp.Controls;
using CalculatorApp.Utils;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;
using CalculatorApp.ViewModel.Common.Automation;

using GraphControl;

using System;

using Windows.ApplicationModel.DataTransfer;
using Windows.ApplicationModel.Resources;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage;
using Windows.System;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Input;

namespace CalculatorApp
{
    public sealed partial class GraphingCalculator : System.ComponentModel.INotifyPropertyChanged
    {
        public GraphingCalculator()
        {
            InitializeComponent();

            m_accessibilitySettings = new AccessibilitySettings();
            DataTransferManager dataTransferManager = DataTransferManager.GetForCurrentView();

            // Register the current control as a share source.
            dataTransferManager.DataRequested += OnDataRequested;

            // Request notifications when we should be showing the trace values
            GraphingControl.TracingChangedEvent += OnShowTracePopupChanged;

            // And when the actual trace value changes
            GraphingControl.TracingValueChangedEvent += OnTracePointChanged;

            // Update where the pointer value is (ie: where the user cursor from keyboard inputs moves the point to)
            GraphingControl.PointerValueChangedEvent += OnPointerPointChanged;

            GraphingControl.Loaded += OnGraphingCalculatorLoaded;

            GraphingControl.UseCommaDecimalSeperator = LocalizationSettings.GetInstance().GetDecimalSeparator() == ',';

            // OemMinus and OemAdd aren't declared in the VirtualKey enum, we can't add this accelerator XAML-side
            var virtualKey = new KeyboardAccelerator
            {
                Key = (VirtualKey)189, // OemPlus key
                Modifiers = VirtualKeyModifiers.Control
            };
            ZoomOutButton.KeyboardAccelerators.Add(virtualKey);

            virtualKey = new KeyboardAccelerator
            {
                Key = (VirtualKey)187, // OemAdd key
                Modifiers = VirtualKeyModifiers.Control
            };
            ZoomInButton.KeyboardAccelerators.Add(virtualKey);

            if (Windows.Foundation.Metadata.ApiInformation.IsTypePresent("Windows.UI.Xaml.Media.ThemeShadow"))
            {
                SharedShadow.Receivers.Add(GraphingControl);
            }

            m_accessibilitySettings.HighContrastChanged += OnHighContrastChanged;

            m_uiSettings = new UISettings();
            m_uiSettings.ColorValuesChanged += OnColorValuesChanged;

            ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;

            if (localSettings != null && localSettings.Values.ContainsKey(sc_IsGraphThemeMatchApp))
            {
                var isMatchAppLocalSetting = (bool)(localSettings.Values[sc_IsGraphThemeMatchApp]);
                if (isMatchAppLocalSetting)
                {
                    IsMatchAppTheme = true;
                    CalculatorApp.ViewModel.Common.TraceLogger.GetInstance().LogGraphTheme("IsMatchAppTheme");
                }
            }
            else
            {
                IsMatchAppTheme = false;
                CalculatorApp.ViewModel.Common.TraceLogger.GetInstance().LogGraphTheme("IsAlwaysLightTheme");
            }
        }

        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
        internal void RaisePropertyChanged(string p)
        {
#if !UNIT_TESTS
            PropertyChanged?.Invoke(this, new System.ComponentModel.PropertyChangedEventArgs(p));
#endif
        }

        public System.Windows.Input.ICommand ZoomOutButtonPressed
        {
            get
            {
                if (donotuse_ZoomOutButtonPressed == null)
                {
                    donotuse_ZoomOutButtonPressed = DelegateCommandUtils.MakeDelegateCommand(this,
                        (that, param) =>
                        {
                            that.OnZoomOutCommand(param);
                        });
                }
                return donotuse_ZoomOutButtonPressed;
            }
        }
        private System.Windows.Input.ICommand donotuse_ZoomOutButtonPressed;

        public System.Windows.Input.ICommand ZoomInButtonPressed
        {
            get
            {
                if (donotuse_ZoomInButtonPressed == null)
                {
                    donotuse_ZoomInButtonPressed = DelegateCommandUtils.MakeDelegateCommand(this,
                        (that, param) =>
                        {
                            that.OnZoomInCommand(param);
                        });
                }
                return donotuse_ZoomInButtonPressed;
            }
        }
        private System.Windows.Input.ICommand donotuse_ZoomInButtonPressed;

        public bool IsKeyGraphFeaturesVisible
        {
            get => m_IsKeyGraphFeaturesVisible;
            private set
            {
                if (m_IsKeyGraphFeaturesVisible != value)
                {
                    m_IsKeyGraphFeaturesVisible = value;
                    RaisePropertyChanged(nameof(IsKeyGraphFeaturesVisible));
                }
            }
        }
        private bool m_IsKeyGraphFeaturesVisible;

        public bool IsSmallState
        {
            get => (bool)GetValue(IsSmallStateProperty);
            set => SetValue(IsSmallStateProperty, value);
        }

        // Using a DependencyProperty as the backing store for IsSmallState.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IsSmallStateProperty =
            DependencyProperty.Register(nameof(IsSmallState), typeof(bool), typeof(GraphingCalculator), new PropertyMetadata(default(bool)));

        public string GraphControlAutomationName
        {
            get => (string)GetValue(GraphControlAutomationNameProperty);
            set => SetValue(GraphControlAutomationNameProperty, value);
        }

        // Using a DependencyProperty as the backing store for GraphControlAutomationName.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty GraphControlAutomationNameProperty =
            DependencyProperty.Register(nameof(GraphControlAutomationName), typeof(string), typeof(GraphingCalculator), new PropertyMetadata(string.Empty));

        public bool IsMatchAppTheme
        {
            get => m_IsMatchAppTheme;
            private set
            {
                if (m_IsMatchAppTheme != value)
                {
                    m_IsMatchAppTheme = value;
                    RaisePropertyChanged(nameof(IsMatchAppTheme));
                }
            }
        }
        private bool m_IsMatchAppTheme;

        public bool IsManualAdjustment
        {
            get => m_IsManualAdjustment;
            set
            {
                if (m_IsManualAdjustment != value)
                {
                    m_IsManualAdjustment = value;
                    RaisePropertyChanged(nameof(IsManualAdjustment));
                }
            }
        }
        private bool m_IsManualAdjustment;

        public CalculatorApp.ViewModel.GraphingCalculatorViewModel ViewModel
        {
            get => m_viewModel;
            set
            {
                if (m_viewModel != value)
                {
                    m_viewModel = value;
                    RaisePropertyChanged(sc_ViewModelPropertyName);
                }
            }
        }

        public static Visibility ShouldDisplayPanel(bool isSmallState, bool isEquationModeActivated, bool isGraphPanel)
        {
            return (!isSmallState || isEquationModeActivated ^ isGraphPanel) ? Visibility.Visible : Visibility.Collapsed;
        }

        public static string GetInfoForSwitchModeToggleButton(bool isChecked)
        {
            if (isChecked)
            {
                return AppResourceProvider.GetInstance().GetResourceString("GraphSwitchToGraphMode");
            }
            else
            {
                return AppResourceProvider.GetInstance().GetResourceString("GraphSwitchToEquationMode");
            }
        }

        public static Windows.UI.Xaml.Visibility ManageEditVariablesButtonVisibility(uint numberOfVariables)
        {
            return numberOfVariables == 0 ? Visibility.Collapsed : Visibility.Visible;
        }

        public static string GetTracingLegend(bool? isTracing)
        {
            var resProvider = AppResourceProvider.GetInstance();
            return isTracing != null && isTracing.Value ? resProvider.GetResourceString("disableTracingButtonToolTip")
                                                        : resProvider.GetResourceString("enableTracingButtonToolTip");
        }

        public void SetDefaultFocus()
        {
            if (IsSmallState)
            {
                SwitchModeToggleButton.Focus(FocusState.Programmatic);
            }
            else
            {
                EquationInputAreaControl.Focus(FocusState.Programmatic);
            }
        }

        private void GraphingCalculator_DataContextChanged(FrameworkElement sender, DataContextChangedEventArgs args)
        {
            if (ViewModel != null)
            {
                ViewModel.Equations.VectorChanged -= OnEquationsVectorChanged;
                ViewModel.VariableUpdated -= OnVariableChanged;
            }

            ViewModel = args.NewValue as GraphingCalculatorViewModel;
            ViewModel.Equations.VectorChanged += OnEquationsVectorChanged;
            ViewModel.VariableUpdated += OnVariableChanged;

            UpdateGraphAutomationName();
        }

        private void OnVariableChanged(object sender, VariableChangedEventArgs args)
        {
            GraphingControl.SetVariable(args.variableName, args.newValue);
        }

        private void OnEquationsVectorChanged(IObservableVector<EquationViewModel> sender, IVectorChangedEventArgs e)
        {
            // If an item is already added to the graph, changing it should automatically trigger a graph update
            if (e.CollectionChange == CollectionChange.ItemChanged)
            {
                return;
            }

            // Do not plot the graph if we are removing an empty equation, just remove it
            if (e.CollectionChange == CollectionChange.ItemRemoved)
            {
                var itemToRemove = GraphingControl.Equations[(int)e.Index];

                if (string.IsNullOrEmpty(itemToRemove.Expression))
                {
                    GraphingControl.Equations.RemoveAt((int)e.Index);

                    if (GraphingControl.Equations.Count == 1 && string.IsNullOrEmpty(GraphingControl.Equations[0].Expression))
                    {
                        IsManualAdjustment = false;
                    }

                    return;
                }
            }

            // Do not plot the graph if we are adding an empty equation, just add it
            if (e.CollectionChange == CollectionChange.ItemInserted)
            {
                var itemToAdd = sender[(int)e.Index];

                if (string.IsNullOrEmpty(itemToAdd.Expression))
                {
                    GraphingControl.Equations.Add(itemToAdd.GraphEquation);

                    return;
                }
            }

            // We are either adding or removing a valid equation, or resetting the collection. We will need to plot the graph
            GraphingControl.Equations.Clear();

            foreach (var equationViewModel in ViewModel.Equations)
            {
                GraphingControl.Equations.Add(equationViewModel.GraphEquation);
            }

            GraphingControl.PlotGraph(false);
        }

        private void OnZoomInCommand(object parameter)
        {
            GraphingControl.ZoomFromCenter(zoomInScale);
            CalculatorApp.ViewModel.Common.TraceLogger.GetInstance().LogGraphButtonClicked(GraphButton.ZoomIn, GraphButtonValue.None);
        }

        private void OnZoomOutCommand(object parameter)
        {
            GraphingControl.ZoomFromCenter(zoomOutScale);
            CalculatorApp.ViewModel.Common.TraceLogger.GetInstance().LogGraphButtonClicked(GraphButton.ZoomOut, GraphButtonValue.None);
        }

        private void OnShareClick(object sender, RoutedEventArgs e)
        {
            // Ask the OS to start a share action.
            try
            {
                DataTransferManager.ShowShareUI();
                CalculatorApp.ViewModel.Common.TraceLogger.GetInstance().LogGraphButtonClicked(GraphButton.Share, GraphButtonValue.None);
            }
            catch (System.Runtime.InteropServices.COMException ex)
            {
                // COMException and HResult, long RPC_E_SERVERCALL_RETRYLATER is out of range of int
                long rpc_e_servercall_retrylater = 0x8001010A;
                if (ex.HResult == unchecked(rpc_e_servercall_retrylater))
                {
                    ShowShareError();
                    CalculatorApp.ViewModel.Common.TraceLogger.GetInstance().LogPlatformExceptionInfo(ViewMode.Graphing, System.Reflection.MethodBase.GetCurrentMethod().Name, ex.Message, ex.HResult);
                }
                else
                {
                    throw;
                }
            }
        }

        private void OnShowTracePopupChanged(bool newValue)
        {
            if ((TraceValuePopup.Visibility == Visibility.Visible) != newValue)
            {
                TraceValuePopup.Visibility = newValue ? Visibility.Visible : Visibility.Collapsed;
            }
        }

        private void OnTracePointChanged(double xPointValue, double yPointValue)
        {
            GraphingControl.GetDisplayRanges(out double xAxisMin, out double xAxisMax, out double yAxisMin, out double yAxisMax);

            TraceValue.Text = "(" + xPointValue.ToString("R") + ", " + yPointValue.ToString("N15") + ")";

            var peer = FrameworkElementAutomationPeer.FromElement(TraceValue);

            if (peer != null)
            {
                peer.RaiseAutomationEvent(AutomationEvents.LiveRegionChanged);
            }

            PositionGraphPopup();
        }

        private void OnPointerPointChanged(Point newPoint)
        {
            if (TracePointer != null)
            {
                // Move the pointer glyph to where it is supposed to be.
                Canvas.SetLeft(TracePointer, newPoint.X);
                Canvas.SetTop(TracePointer, newPoint.Y);
            }
        }

        // When share is invoked (by the user or programmatically) the event handler we registered will be called to populate the data package with the
        // data to be shared. We will request the current graph image from the grapher as a stream that will pass to the share request.
        private void OnDataRequested(DataTransferManager sender, DataRequestedEventArgs args)
        {
            var resourceLoader = ResourceLoader.GetForCurrentView();

            try
            {
                string rawHtml;
                string equationHtml;

                rawHtml = "<p><img src='graph.png' width='600' alt='" + resourceLoader.GetString("GraphImageAltText") + "'></p>";

                var equations = ViewModel.Equations;
                bool hasEquations = false;

                if (equations.Count > 0)
                {
                    equationHtml = "<span style=\"color: rgb(68, 114, 196); font-style: bold; font-size : 13pt;\">"
                                   + resourceLoader.GetString("EquationsShareHeader") + "</span>"
                                   + "<table cellpadding=\"0\" cellspacing=\"0\" >";

                    foreach (var equation in equations)
                    {
                        var expression = equation.Expression;
                        if (string.IsNullOrEmpty(expression))
                        {
                            continue;
                        }

                        var color = equation.LineColor;
                        hasEquations = true;

                        expression = GraphingControl.ConvertToLinear(expression);

                        string equationColorHtml;
                        equationColorHtml = "color:rgb(" + color.R.ToString() + "," + color.G.ToString() + "," + color.B.ToString() + ");";

                        equationHtml += "<tr style=\"margin: 0pt 0pt 0pt 0pt; padding: 0pt 0pt 0pt 0pt; \"><td><span style=\"font-size: 22pt; line-height: 0;"
                                        + equationColorHtml + "\">&#x25A0;</span></td><td><div style=\"margin: 4pt 0pt 0pt 6pt;\">"
                                        + Utilities.EscapeHtmlSpecialCharacters(expression) + "</div></td>";
                    }
                    equationHtml += "</table>";

                    if (hasEquations)
                    {
                        rawHtml += equationHtml;
                    }
                }

                var variables = ViewModel.Variables;

                if (variables.Count > 0)
                {
                    var localizedSeperator = LocalizationSettings.GetInstance().GetListSeparatorWinRT() + " ";

                    rawHtml += "<br><span style=\"color: rgb(68, 114, 196); font-style: bold; font-size: 13pt;\">"
                               + resourceLoader.GetString("VariablesShareHeader")
                               + "</span><br><div style=\"margin: 4pt 0pt 0pt 0pt;\">";

                    for (int i = 0; i < variables.Count; i++)
                    {
                        var name = variables[i].Name;
                        var value = variables[i].Value;

                        rawHtml += name + "=";
                        var formattedValue = value.ToString();
                        rawHtml += formattedValue;

                        if (variables.Count - 1 != i)
                        {
                            rawHtml += localizedSeperator;
                        }
                    }

                    rawHtml += "</div>";
                }

                rawHtml += "<br><br>";

                // Shortcut to the request data
                var requestData = args.Request.Data;

                DataPackage dataPackage = new DataPackage();
                var html = HtmlFormatHelper.CreateHtmlFormat(rawHtml);

                requestData.Properties.Title = resourceLoader.GetString("ShareActionTitle");

                requestData.SetHtmlFormat(html);

                var bitmapStream = GraphingControl.GetGraphBitmapStream();

                requestData.ResourceMap.Add("graph.png", bitmapStream);
                requestData.SetBitmap(bitmapStream);

                // Set the thumbnail image (in case the share target can't handle HTML)
                requestData.Properties.Thumbnail = bitmapStream;
            }
            catch (Exception ex)
            {
                ShowShareError();
                CalculatorApp.ViewModel.Common.TraceLogger.GetInstance().LogPlatformExceptionInfo(ViewMode.Graphing, System.Reflection.MethodBase.GetCurrentMethod().Name, ex.Message, ex.HResult);
            }
        }

        private void GraphingControl_LostFocus(object sender, RoutedEventArgs e)
        {
            // If the graph is losing focus while we are in active tracing we need to turn it off so we don't try to eat keys in other controls.
            if (GraphingControl.ActiveTracing)
            {
                if (ActiveTracing.Equals(FocusManager.GetFocusedElement()) && ActiveTracing.IsPressed)
                {
                    ActiveTracing.PointerCaptureLost += ActiveTracing_PointerCaptureLost;
                }
                else
                {
                    GraphingControl.ActiveTracing = false;
                    OnShowTracePopupChanged(false);
                }
            }
        }

        private void GraphingControl_LosingFocus(UIElement sender, LosingFocusEventArgs args)
        {
            if (!(args.NewFocusedElement is FrameworkElement newFocusElement) || newFocusElement.Name == null)
            {
                // Because clicking on the swap chain panel will try to move focus to a control that can't actually take focus
                // we will get a null destination.  So we are going to try and cancel that request.
                // If the destination is not in our application we will also get a null destination but the cancel will fail so it doesn't hurt to try.
                args.TryCancel();
            }
        }

        private void GraphingControl_VariablesUpdated(object sender, object args)
        {
            m_viewModel.UpdateVariables(GraphingControl.Variables);
        }

        private void GraphingControl_GraphViewChangedEvent(object sender, GraphViewChangedReason reason)
        {
            if (reason == GraphViewChangedReason.Manipulation)
            {
                IsManualAdjustment = true;
            }
            else
            {
                IsManualAdjustment = false;
            }

            UpdateGraphAutomationName();

            var announcement = CalculatorAnnouncement.GetGraphViewChangedAnnouncement(GraphControlAutomationName);
            var peer = FrameworkElementAutomationPeer.FromElement(GraphingControl);
            if (peer != null)
            {
                peer.RaiseNotificationEvent(announcement.Kind, announcement.Processing, announcement.Announcement, announcement.ActivityId);
            }
        }

        private void GraphingControl_GraphPlottedEvent(object sender, RoutedEventArgs e)
        {
            UpdateGraphAutomationName();
        }

        private void OnEquationKeyGraphFeaturesRequested(object sender, EquationViewModel equationViewModel)
        {
            ViewModel.SetSelectedEquation(equationViewModel);
            if (equationViewModel != null)
            {
                var keyGraphFeatureInfo = GraphingControl.AnalyzeEquation(equationViewModel.GraphEquation);
                equationViewModel.PopulateKeyGraphFeatures(keyGraphFeatureInfo);
                IsKeyGraphFeaturesVisible = true;
                equationViewModel.GraphEquation.IsSelected = true;
            }
        }

        private void OnKeyGraphFeaturesClosed(object sender, RoutedEventArgs e)
        {
            IsKeyGraphFeaturesVisible = false;
            EquationInputAreaControl.FocusEquationTextBox(ViewModel.SelectedEquation);
        }

        private void TraceValuePopup_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            PositionGraphPopup();
        }

        private void PositionGraphPopup()
        {
            if (GraphingControl.TraceLocation.X + 15 + TraceValuePopup.ActualWidth >= GraphingControl.ActualWidth)
            {
                TraceValuePopupTransform.X = (int)GraphingControl.TraceLocation.X - 15 - TraceValuePopup.ActualWidth;
            }
            else
            {
                TraceValuePopupTransform.X = (int)GraphingControl.TraceLocation.X + 15;
            }

            if (GraphingControl.TraceLocation.Y >= 30)
            {
                TraceValuePopupTransform.Y = (int)GraphingControl.TraceLocation.Y - 30;
            }
            else
            {
                TraceValuePopupTransform.Y = (int)GraphingControl.TraceLocation.Y;
            }
        }

        private void ActiveTracing_Checked(object sender, RoutedEventArgs e)
        {
            if (!m_cursorShadowInitialized)
            {
                this.FindName("TraceCanvas");

                // add shadow to the trace pointer
                AddTracePointerShadow();

                // hide the shadow in high contrast mode
                CursorShadow.Visibility = m_accessibilitySettings.HighContrast ? Visibility.Collapsed : Visibility.Visible;

                Canvas.SetLeft(TracePointer, TraceCanvas.ActualWidth / 2 + 40);
                Canvas.SetTop(TracePointer, TraceCanvas.ActualHeight / 2 - 40);

                m_cursorShadowInitialized = true;
            }

            _ = FocusManager.TryFocusAsync(GraphingControl, FocusState.Programmatic);

            Window.Current.CoreWindow.KeyUp += ActiveTracing_KeyUp;

            KeyboardShortcutManager.IgnoreEscape(false);

            TracePointer.Visibility = Visibility.Visible;
            CalculatorApp.ViewModel.Common.TraceLogger.GetInstance().LogGraphButtonClicked(GraphButton.ActiveTracingChecked, GraphButtonValue.None);
        }

        private void ActiveTracing_Unchecked(object sender, RoutedEventArgs e)
        {
            ActiveTracing.PointerCaptureLost -= ActiveTracing_PointerCaptureLost;
            Window.Current.CoreWindow.KeyUp -= ActiveTracing_KeyUp;
            KeyboardShortcutManager.HonorEscape();

            TracePointer.Visibility = Visibility.Collapsed;
            CalculatorApp.ViewModel.Common.TraceLogger.GetInstance().LogGraphButtonClicked(GraphButton.ActiveTracingUnchecked, GraphButtonValue.None);
        }

        private void ActiveTracing_KeyUp(CoreWindow sender, KeyEventArgs args)
        {
            if (args.VirtualKey == VirtualKey.Escape)
            {
                GraphingControl.ActiveTracing = false;
                ActiveTracing.Focus(FocusState.Programmatic);
                args.Handled = true;
            }
        }

        private void ActiveTracing_PointerCaptureLost(object sender, PointerRoutedEventArgs e)
        {
            ActiveTracing.PointerCaptureLost -= ActiveTracing_PointerCaptureLost;

            if (GraphingControl.ActiveTracing)
            {
                GraphingControl.ActiveTracing = false;
                OnShowTracePopupChanged(false);
            }
        }

        private void GraphSettingsButton_Click(object sender, RoutedEventArgs e)
        {
            DisplayGraphSettings();
            CalculatorApp.ViewModel.Common.TraceLogger.GetInstance().LogGraphButtonClicked(GraphButton.GraphSettings, GraphButtonValue.None);
        }

        private void SwitchModeToggleButton_Toggled(object sender, RoutedEventArgs e)
        {
            var narratorNotifier = new NarratorNotifier();
            string announcementText;
            if (SwitchModeToggleButton.IsOn)
            {
                announcementText = AppResourceProvider.GetInstance().GetResourceString("GraphSwitchedToEquationModeAnnouncement");
            }
            else
            {
                announcementText = AppResourceProvider.GetInstance().GetResourceString("GraphSwitchedToGraphModeAnnouncement");
            }

            var announcement = CalculatorAnnouncement.GetGraphModeChangedAnnouncement(announcementText);
            narratorNotifier.Announce(announcement);
        }

        private void DisplayGraphSettings()
        {
            if (m_graphSettings == null)
            {
                m_graphSettings = new GraphingSettings();
                m_graphSettings.GraphThemeSettingChanged += OnGraphThemeSettingChanged;
            }

            if (m_graphFlyout == null)
            {
                m_graphFlyout = new Flyout
                {
                    Content = m_graphSettings
                };
            }

            m_graphSettings.SetGrapher(this.GraphingControl);
            m_graphSettings.IsMatchAppTheme = IsMatchAppTheme;

            var options = new FlyoutShowOptions
            {
                Placement = FlyoutPlacementMode.BottomEdgeAlignedRight
            };
            m_graphFlyout.ShowAt(GraphSettingsButton, options);
        }

        private void AddTracePointerShadow()
        {
            var compositor = Windows.UI.Xaml.Hosting.ElementCompositionPreview.GetElementVisual(CursorPath).Compositor;
            var dropShadow = compositor.CreateDropShadow();
            dropShadow.BlurRadius = 6;
            dropShadow.Opacity = 0.33f;
            dropShadow.Offset = new System.Numerics.Vector3(2, 2, 0);
            dropShadow.Mask = CursorPath.GetAlphaMask();

            var shadowSpriteVisual = compositor.CreateSpriteVisual();
            shadowSpriteVisual.Size = new System.Numerics.Vector2(18, 18);
            shadowSpriteVisual.Shadow = dropShadow;
            Windows.UI.Xaml.Hosting.ElementCompositionPreview.SetElementChildVisual(CursorShadow, shadowSpriteVisual);
        }

        private void UpdateGraphAutomationName()
        {
            int numEquations = 0;

            // Only count equations that are graphed
            foreach (var equation in ViewModel.Equations)
            {
                if (equation.GraphEquation.IsValidated)
                {
                    numEquations++;
                }
            }

            GraphingControl.GetDisplayRanges(out double xAxisMin, out double xAxisMax, out double yAxisMin, out double yAxisMax);

            GraphControlAutomationName = LocalizationStringUtil.GetLocalizedString(
                AppResourceProvider.GetInstance().GetResourceString("graphAutomationName"),
                xAxisMin.ToString(),
                xAxisMax.ToString(),
                yAxisMin.ToString(),
                yAxisMax.ToString(),
                numEquations.ToString());
        }

        private void OnColorValuesChanged(UISettings sender, object args)
        {
            WeakReference weakThis = new WeakReference(this);
            _ = this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                if (weakThis.Target is GraphingCalculator refThis && IsMatchAppTheme)
                {
                    refThis.UpdateGraphTheme();
                }
            });
        }

        private void UpdateGraphTheme()
        {
            if (m_accessibilitySettings.HighContrast)
            {
                VisualStateManager.GoToState(this, "GrapherHighContrast", true);
                return;
            }

            if (IsMatchAppTheme && Application.Current.RequestedTheme == ApplicationTheme.Dark)
            {
                VisualStateManager.GoToState(this, "GrapherDarkTheme", true);
            }
            else
            {
                VisualStateManager.GoToState(this, "GrapherLightTheme", true);
            }
        }

        private void OnGraphThemeSettingChanged(object sender, bool isMatchAppTheme)
        {
            if (IsMatchAppTheme == isMatchAppTheme)
            {
                return;
            }

            IsMatchAppTheme = isMatchAppTheme;
            WeakReference weakThis = new WeakReference(this);
            _ = this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                if (weakThis.Target is GraphingCalculator refThis)
                {
                    refThis.UpdateGraphTheme();
                }
            });
        }

        private const double zoomInScale = 1 / 1.0625;
        private const double zoomOutScale = 1.0625;
        private const string sc_ViewModelPropertyName = "ViewModel";
        private const string sc_IsGraphThemeMatchApp = "IsGraphThemeMatchApp";

        private CalculatorApp.ViewModel.GraphingCalculatorViewModel m_viewModel;
        private readonly Windows.UI.ViewManagement.AccessibilitySettings m_accessibilitySettings;
        private bool m_cursorShadowInitialized;
        private readonly Windows.UI.ViewManagement.UISettings m_uiSettings;
        private Windows.UI.Xaml.Controls.Flyout m_graphFlyout;
        private CalculatorApp.GraphingSettings m_graphSettings;

        private void Canvas_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            // Initialize the pointer to the correct location to match initial value in GraphControl\DirectX\RenderMain
            if (TracePointer != null)
            {
                Canvas.SetLeft(TracePointer, e.NewSize.Width / 2 + 40);
                Canvas.SetTop(TracePointer, e.NewSize.Height / 2 - 40);
            }
        }

        private void OnHighContrastChanged(AccessibilitySettings sender, object args)
        {
            if (CursorShadow != null)
            {
                CursorShadow.Visibility = sender.HighContrast ? Visibility.Collapsed : Visibility.Visible;
            }

            UpdateGraphTheme();
        }

        private void OnEquationFormatRequested(object sender, MathRichEditBoxFormatRequest e)
        {
            if (!string.IsNullOrEmpty(e.OriginalText))
            {
                e.FormattedText = GraphingControl.FormatMathML(e.OriginalText);
            }
        }

        private void GraphMenuFlyoutItem_Click(object sender, RoutedEventArgs e)
        {
            var dataPackage = new DataPackage
            {
                RequestedOperation = DataPackageOperation.Copy
            };

            var bitmapStream = GraphingControl.GetGraphBitmapStream();
            dataPackage.SetBitmap(bitmapStream);
            Clipboard.SetContent(dataPackage);
        }

        private void OnVisualStateChanged(object sender, VisualStateChangedEventArgs e)
        {
            CalculatorApp.ViewModel.Common.TraceLogger.GetInstance().LogVisualStateChanged(ViewMode.Graphing, e.NewState.Name, false);
        }

        private void GraphViewButton_Click(object sender, RoutedEventArgs e)
        {
            var narratorNotifier = new NarratorNotifier();
            string announcementText;
            if (IsManualAdjustment)
            {
                announcementText = AppResourceProvider.GetInstance().GetResourceString("GraphViewManualAdjustmentAnnouncement");
            }
            else
            {
                announcementText = AppResourceProvider.GetInstance().GetResourceString("GraphViewAutomaticBestFitAnnouncement");
                announcementText += AppResourceProvider.GetInstance().GetResourceString("GridResetAnnouncement");
                GraphingControl.ResetGrid();
            }

            var announcement = CalculatorAnnouncement.GetGraphViewBestFitChangedAnnouncement(announcementText);
            narratorNotifier.Announce(announcement);

            CalculatorApp.ViewModel.Common.TraceLogger.GetInstance().LogGraphButtonClicked(
                GraphButton.GraphView, IsManualAdjustment ? GraphButtonValue.ManualAdjustment : GraphButtonValue.AutomaticBestFit);
        }

        private void ShowShareError()
        {
            // Something went wrong, notify the user.
            var resourceLoader = ResourceLoader.GetForCurrentView();
            var errDialog = new ContentDialog
            {
                Content = resourceLoader.GetString("ShareActionErrorMessage"),
                CloseButtonText = resourceLoader.GetString("ShareActionErrorOk")
            };
            _ = errDialog.ShowAsync();
        }

        private void OnGraphingCalculatorLoaded(object sender, RoutedEventArgs e)
        {
            GraphingControl.Loaded -= OnGraphingCalculatorLoaded;

            // The control needs to be loaded, else the control will override GridLinesColor and ignore the value passed
            UpdateGraphTheme();
        }
    }
}
