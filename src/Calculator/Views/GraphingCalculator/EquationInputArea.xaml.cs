// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Calculator.Utils;

using CalculatorApp.Controls;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;
using CalculatorApp.ViewModel.Common.Automation;

using GraphControl;

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;

using Windows.System;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;

namespace CalculatorApp
{
    public sealed partial class EquationInputArea : System.ComponentModel.INotifyPropertyChanged
    {
        public EquationInputArea()
        {
            m_lastLineColorIndex = -1;
            m_AvailableColors = new ObservableCollection<SolidColorBrush>();
            m_accessibilitySettings = new AccessibilitySettings();
            m_equationToFocus = null;

            m_accessibilitySettings.HighContrastChanged += OnHighContrastChanged;
            m_isHighContrast = m_accessibilitySettings.HighContrast;

            m_uiSettings = new UISettings();
            m_uiSettings.ColorValuesChanged += OnColorValuesChanged;

            ReloadAvailableColors(m_accessibilitySettings.HighContrast, true);

            InitializeComponent();
        }

        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
        internal void RaisePropertyChanged(string p)
        {
            PropertyChanged?.Invoke(this, new System.ComponentModel.PropertyChangedEventArgs(p));
            OnPropertyChanged(p);
        }

        public Windows.Foundation.Collections.IObservableVector<ViewModel.EquationViewModel> Equations
        {
            get => m_Equations;
            set
            {
                if (m_Equations != value)
                {
                    m_Equations = value;
                    RaisePropertyChanged(nameof(Equations));
                }
            }
        }
        private Windows.Foundation.Collections.IObservableVector<ViewModel.EquationViewModel> m_Equations;

        public Windows.Foundation.Collections.IObservableVector<ViewModel.VariableViewModel> Variables
        {
            get => m_Variables;
            set
            {
                if (m_Variables != value)
                {
                    m_Variables = value;
                    RaisePropertyChanged(nameof(Variables));
                }
            }
        }
        private Windows.Foundation.Collections.IObservableVector<ViewModel.VariableViewModel> m_Variables;

        public ObservableCollection<SolidColorBrush> AvailableColors
        {
            get => m_AvailableColors;
            set
            {
                if (m_AvailableColors != value)
                {
                    m_AvailableColors = value;
                    RaisePropertyChanged(nameof(AvailableColors));
                }
            }
        }
        private ObservableCollection<SolidColorBrush> m_AvailableColors;


        public bool IsMatchAppTheme
        {
            get => m_IsMatchAppTheme;
            set
            {
                if (m_IsMatchAppTheme != value)
                {
                    m_IsMatchAppTheme = value;
                    RaisePropertyChanged(nameof(IsMatchAppTheme));
                }
            }
        }
        private bool m_IsMatchAppTheme;

        public event System.EventHandler<ViewModel.EquationViewModel> KeyGraphFeaturesRequested;
        public event System.EventHandler<CalculatorApp.Controls.MathRichEditBoxFormatRequest> EquationFormatRequested;

        public static Visibility ManageEditVariablesButtonVisibility(uint numberOfVariables)
        {
            return numberOfVariables == 0 ? Visibility.Collapsed : Visibility.Visible;
        }

        public static bool ManageEditVariablesButtonLoaded(int numberOfVariables)
        {
            return numberOfVariables != 0;
        }

        public static string GetChevronIcon(bool isCollapsed)
        {
            return isCollapsed ? "\uE70E" : "\uE70D";
        }

        public static SolidColorBrush ToSolidColorBrush(Color color)
        {
            return new SolidColorBrush(color);
        }

        public static SolidColorBrush GetForegroundColor(Color lineColor)
        {
            return Utilities.GetContrastColor(lineColor);
        }

        public void FocusEquationTextBox(EquationViewModel equation)
        {
            int index = Equations.IndexOf(equation);
            if (index < 0)
            {
                return;
            }
            var container = (UIElement)EquationInputList.ContainerFromIndex(index);
            if (container != null)
            {
                container.StartBringIntoView();

                var equationInput = VisualTree.FindDescendantByName(container, "EquationInputButton");
                if (equationInput == null)
                {
                    return;
                }
                if (equationInput is EquationTextBox equationTextBox)
                {
                    equationTextBox.FocusTextBox();
                }
            }
        }

        private void OnPropertyChanged(string propertyName)
        {
            if (propertyName == EquationsPropertyName)
            {
                OnEquationsPropertyChanged();
            }

            else if (propertyName == IsMatchAppThemePropertyName)
            {
                ReloadAvailableColors(m_accessibilitySettings.HighContrast, false);
            }

        }

        private void OnEquationsPropertyChanged()
        {
            if (Equations != null && Equations.Count == 0)
            {
                AddNewEquation();
            }
        }

        private void AddNewEquation()
        {
            if (Equations.Count > 0)
            {
                Equations[Equations.Count - 1].IsLastItemInList = false;
            }

            // Cap equations at 14
            if (Equations.Count >= maxEquationSize)
            {
                return;
            }

            int colorIndex;

            if (m_accessibilitySettings.HighContrast)
            {
                m_lastLineColorIndex = (m_lastLineColorIndex + 1) % AvailableColors.Count;
                colorIndex = m_lastLineColorIndex;
            }
            else
            {
                bool[] colorAssignmentUsed = new bool[colorCount];
                foreach (var equation in Equations)
                {
                    colorAssignmentUsed[equation.LineColorIndex] = true;
                }

                colorIndex = 0;
                // If for some reason all of the values in colorAssignmentUsed are true, the check for colorIndex < colorCount - 1 will
                // set it to the last color in the list
                while (colorIndex < colorCount - 1 && colorAssignmentUsed[colorAssignmentMapping[colorIndex]])
                {
                    colorIndex++;
                }

                colorIndex = colorAssignmentMapping[colorIndex];
            }

            var eq = new EquationViewModel(new Equation(), ++m_lastFunctionLabelIndex, AvailableColors[colorIndex].Color, colorIndex)
            {
                IsLastItemInList = true
            };
            m_equationToFocus = eq;
            Equations.Add(eq);
        }

        private void EquationTextBox_GotFocus(object sender, RoutedEventArgs e)
        {
            var eq = GetViewModelFromEquationTextBox(sender);
            if (eq != null)
            {
                eq.GraphEquation.IsSelected = true;
            }
        }

        private void EquationTextBox_LostFocus(object sender, RoutedEventArgs e)
        {
            var eq = GetViewModelFromEquationTextBox(sender);
            if (eq != null)
            {
                eq.GraphEquation.IsSelected = false;
            }
        }

        private void EquationTextBox_Submitted(object sender, MathRichEditBoxSubmission submission)
        {
            var eq = GetViewModelFromEquationTextBox(sender);
            if (eq == null)
            {
                return;
            }

            if (submission.Source == EquationSubmissionSource.ENTER_KEY
                || (submission.Source == EquationSubmissionSource.FOCUS_LOST && submission.HasTextChanged && !string.IsNullOrEmpty(eq.Expression)))
            {
                if (submission.Source == EquationSubmissionSource.ENTER_KEY)
                {
                    eq.IsLineEnabled = true;
                }

                int index = Equations.IndexOf(eq);
                if (index >= 0)
                {
                    if (index == Equations.Count - 1)
                    {
                        // If it's the last equation of the list
                        AddNewEquation();
                    }
                    else
                    {
                        if (submission.Source == EquationSubmissionSource.ENTER_KEY)
                        {
                            var nextEquation = Equations[index + 1];
                            FocusEquationTextBox(nextEquation);
                        }
                    }
                }
            }
        }

        private void OnHighContrastChanged(AccessibilitySettings sender, object args)
        {
            ReloadAvailableColors(sender.HighContrast, true);
            m_isHighContrast = sender.HighContrast;
        }

        private void ReloadAvailableColors(bool isHighContrast, bool reassignColors)
        {
            m_AvailableColors.Clear();
            if (isHighContrast)
            {
                m_AvailableColors.Add((SolidColorBrush)Application.Current.Resources["EquationBrush1"]);
                m_AvailableColors.Add((SolidColorBrush)Application.Current.Resources["EquationBrush2"]);
                m_AvailableColors.Add((SolidColorBrush)Application.Current.Resources["EquationBrush3"]);
                m_AvailableColors.Add((SolidColorBrush)Application.Current.Resources["EquationBrush4"]);
            }

            // If this is not high contrast, we have all 16 colors, otherwise we will restrict this to a subset of high contrast colors
            else
            {
                object themeDictionaryName = "Light";
                if (IsMatchAppTheme && Application.Current.RequestedTheme == ApplicationTheme.Dark)
                {
                    themeDictionaryName = "Default";
                }
                var themeDictionary = (ResourceDictionary)Application.Current.Resources.ThemeDictionaries[themeDictionaryName];
                m_AvailableColors.Add((SolidColorBrush)themeDictionary["EquationBrush1"]);
                m_AvailableColors.Add((SolidColorBrush)themeDictionary["EquationBrush2"]);
                m_AvailableColors.Add((SolidColorBrush)themeDictionary["EquationBrush3"]);
                m_AvailableColors.Add((SolidColorBrush)themeDictionary["EquationBrush4"]);
                m_AvailableColors.Add((SolidColorBrush)themeDictionary["EquationBrush5"]);
                m_AvailableColors.Add((SolidColorBrush)themeDictionary["EquationBrush6"]);
                m_AvailableColors.Add((SolidColorBrush)themeDictionary["EquationBrush7"]);
                m_AvailableColors.Add((SolidColorBrush)themeDictionary["EquationBrush8"]);
                m_AvailableColors.Add((SolidColorBrush)themeDictionary["EquationBrush9"]);
                m_AvailableColors.Add((SolidColorBrush)themeDictionary["EquationBrush10"]);
                m_AvailableColors.Add((SolidColorBrush)themeDictionary["EquationBrush11"]);
                m_AvailableColors.Add((SolidColorBrush)themeDictionary["EquationBrush12"]);
                m_AvailableColors.Add((SolidColorBrush)themeDictionary["EquationBrush13"]);
                m_AvailableColors.Add((SolidColorBrush)themeDictionary["EquationBrush14"]);
            }

            // If there are no equations to reload, quit early
            if (Equations == null || Equations.Count == 0)
            {
                return;
            }

            // Reassign colors for each equation
            if (reassignColors)
            {
                m_lastLineColorIndex = -1;
            }

            foreach (var equationViewModel in Equations)
            {
                if (reassignColors)
                {
                    m_lastLineColorIndex = (m_lastLineColorIndex + 1) % AvailableColors.Count;
                    equationViewModel.LineColorIndex = m_lastLineColorIndex;
                }
                equationViewModel.LineColor = AvailableColors[equationViewModel.LineColorIndex].Color;
            }
        }

        private void OnColorValuesChanged(UISettings sender, object args)
        {

            WeakReference weakThis = new WeakReference(this);
            _ = this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                if (weakThis.Target is EquationInputArea refThis && refThis.m_isHighContrast == refThis.m_accessibilitySettings.HighContrast)
                {
                    refThis.ReloadAvailableColors(false, false);
                }
            });
        }

        private void EquationTextBox_RemoveButtonClicked(object sender, RoutedEventArgs e)
        {
            var eq = GetViewModelFromEquationTextBox(sender);
            int index = Equations.IndexOf(eq);

            if (index >= 0)
            {
                // Prevent removing the last equation
                if (index == Equations.Count - 1)
                {
                    return;
                }

                Equations.RemoveAt(index);

                var narratorNotifier = new NarratorNotifier();
                var announcement =
                    CalculatorAnnouncement.GetFunctionRemovedAnnouncement(AppResourceProvider.GetInstance().GetResourceString("FunctionRemovedAnnouncement"));
                narratorNotifier.Announce(announcement);

                int lastIndex = Equations.Count - 1;

                if (Equations.Count <= 1)
                {
                    m_lastFunctionLabelIndex = 1;
                }
                else
                {
                    m_lastFunctionLabelIndex = Equations[lastIndex - 1].FunctionLabelIndex + 1;
                }

                Equations[lastIndex].FunctionLabelIndex = m_lastFunctionLabelIndex;

                // Focus the next equation after the one we just removed. There should always be at least one ghost equation,
                // but check to make sure that there is an equation we can focus in the index where we just removed an equation.
                if (index < Equations.Count)
                {
                    FocusEquationTextBox(Equations[index]);
                }
            }
        }

        private void EquationTextBox_KeyGraphFeaturesButtonClicked(object sender, RoutedEventArgs e)
        {
            KeyGraphFeaturesRequested?.Invoke(this, GetViewModelFromEquationTextBox(sender));
        }

        private void EquationTextBox_EquationButtonClicked(object sender, RoutedEventArgs e)
        {
            var eq = GetViewModelFromEquationTextBox(sender);
            eq.IsLineEnabled = !eq.IsLineEnabled;

            CalculatorApp.ViewModel.Common.TraceLogger.GetInstance().LogShowHideButtonClicked(!eq.IsLineEnabled);
        }

        private void EquationTextBox_Loaded(object sender, RoutedEventArgs e)
        {
            var tb = (EquationTextBox)sender;

            var colorChooser = (EquationStylePanelControl)tb.ColorChooserFlyout.Content;
            colorChooser.AvailableColors = AvailableColors;

            if (m_equationToFocus != null && tb.DataContext == m_equationToFocus)
            {
                var copyEquationToFocus = m_equationToFocus;
                m_equationToFocus = null;
                tb.FocusTextBox();

                int index = Equations.IndexOf(copyEquationToFocus);
                if (index >= 0)
                {
                    var container = (UIElement)EquationInputList.ContainerFromIndex(index);
                    container?.StartBringIntoView();
                }
            }
        }

        private void EquationTextBox_DataContextChanged(FrameworkElement sender, DataContextChangedEventArgs args)
        {
            var tb = (EquationTextBox)sender;
            if (!tb.IsLoaded)
            {
                return;
            }

            FocusEquationIfNecessary(tb);
        }

        private void FocusEquationIfNecessary(EquationTextBox textBox)
        {
            if (m_equationToFocus != null && textBox.DataContext == m_equationToFocus)
            {
                m_equationToFocus = null;
                textBox.FocusTextBox();

                int index = Equations.IndexOf(m_equationToFocus);
                if (index >= 0)
                {
                    var container = (UIElement)EquationInputList.ContainerFromIndex(index);
                    container?.StartBringIntoView();
                }
            }
        }

        private double validateDouble(string value, double defaultValue)
        {
            if (double.TryParse(value, out var resultValue))
            {
                return resultValue;
            }

            return defaultValue;
        }

        private void TextBoxGotFocus(object sender, RoutedEventArgs e)
        {
            ((TextBox)sender).SelectAll();
        }

        private void TextBoxLosingFocus(object sender, LosingFocusEventArgs e)
        {
            SubmitTextbox((TextBox)sender);
        }

        private void TextBoxKeyDown(object sender, KeyRoutedEventArgs e)
        {
            if (e.Key == VirtualKey.Enter)
            {
                SubmitTextbox((TextBox)sender);
            }
        }

        private void SubmitTextbox(TextBox sender)
        {
            var variableViewModel = (VariableViewModel)sender.DataContext;
            double val;
            if (sender.Name == "ValueTextBox")
            {
                val = validateDouble(sender.Text, variableViewModel.Value);
                variableViewModel.Value = val;
                CalculatorApp.ViewModel.Common.TraceLogger.GetInstance().LogVariableChanged("ValueTextBox", variableViewModel.Name);
            }
            else if (sender.Name == "MinTextBox")
            {
                val = validateDouble(sender.Text, variableViewModel.Min);

                variableViewModel.Min = val;
                CalculatorApp.ViewModel.Common.TraceLogger.GetInstance().LogVariableSettingsChanged("MinTextBox");
            }
            else if (sender.Name == "MaxTextBox")
            {
                val = validateDouble(sender.Text, variableViewModel.Max);
                variableViewModel.Max = val;
                CalculatorApp.ViewModel.Common.TraceLogger.GetInstance().LogVariableSettingsChanged("MaxTextBox");
            }
            else if (sender.Name == "StepTextBox")
            {
                val = validateDouble(sender.Text, variableViewModel.Step);

                // Don't allow a value less than or equal to 0 as the step
                if (val <= 0)
                {
                    val = variableViewModel.Step;
                }

                variableViewModel.Step = val;
                CalculatorApp.ViewModel.Common.TraceLogger.GetInstance().LogVariableSettingsChanged("StepTextBox");
            }
            else
            {
                return;
            }

            sender.Text = val.ToString("G6", System.Globalization.CultureInfo.InvariantCulture);
        }

        private void VariableAreaClicked(object sender, RoutedEventArgs e)
        {
            ToggleVariableArea((VariableViewModel)((ToggleButton)sender).DataContext);
        }

        private void VariableAreaButtonTapped(object sender, TappedRoutedEventArgs e)
        {
            e.Handled = true;
        }

        private void VariableAreaTapped(object sender, TappedRoutedEventArgs e)
        {
            ToggleVariableArea((VariableViewModel)((FrameworkElement)sender).DataContext);
        }

        private void EquationTextBox_EquationFormatRequested(object sender, MathRichEditBoxFormatRequest e)
        {
            EquationFormatRequested?.Invoke(sender, e);
        }

        private void Slider_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (variableSliders == null)
            {
                variableSliders = new SortedDictionary<string, DispatcherTimerDelayer>();
            }

            var slider = (Slider)sender;

            // The slider value updates when the user uses the TextBox to change the variable value.
            // Check the focus state so that we don't trigger the event when the user used the textbox to change the variable value.
            if (slider.FocusState == Windows.UI.Xaml.FocusState.Unfocused)
            {
                return;
            }

            var variableVM = (VariableViewModel)slider.DataContext;
            if (variableVM == null)
            {
                return;
            }

            var name = variableVM.Name;

            if (!variableSliders.ContainsKey(name))
            {
                TimeSpan timeSpan = new TimeSpan(10000000); // 1 tick = 100 nanoseconds, and 10000000 ticks = 1 second.
                DispatcherTimerDelayer delayer = new DispatcherTimerDelayer(timeSpan);
                delayer.Action += (s, arg) =>
                {
                    CalculatorApp.ViewModel.Common.TraceLogger.GetInstance().LogVariableChanged("Slider", name);
                    variableSliders.Remove(name);
                };
                delayer.Start();
                variableSliders.Add(name, delayer);
            }
            else
            {
                DispatcherTimerDelayer delayer = variableSliders[name];
                delayer.ResetAndStart();
            }
        }

        private EquationViewModel GetViewModelFromEquationTextBox(object sender)
        {
            var tb = (EquationTextBox)sender;

            var eq = (EquationViewModel)tb?.DataContext;

            return eq;
        }

        private void ToggleVariableArea(VariableViewModel selectedVariableViewModel)
        {
            selectedVariableViewModel.SliderSettingsVisible = !selectedVariableViewModel.SliderSettingsVisible;

            // Collapse all other slider settings that are open
            foreach (var variableViewModel in Variables)
            {
                if (variableViewModel != selectedVariableViewModel)
                {
                    variableViewModel.SliderSettingsVisible = false;
                }
            }
        }

        private const int maxEquationSize = 14;
        private const int colorCount = 14;
        private static readonly int[] colorAssignmentMapping = { 0, 3, 7, 10, 1, 4, 8, 11, 2, 5, 9, 12, 6, 13 };
        private const string EquationsPropertyName = "Equations";
        private const string IsMatchAppThemePropertyName = "IsMatchAppTheme";

        private readonly Windows.UI.ViewManagement.AccessibilitySettings m_accessibilitySettings;
        private readonly Windows.UI.ViewManagement.UISettings m_uiSettings;
        private int m_lastLineColorIndex;
        private int m_lastFunctionLabelIndex;
        private bool m_isHighContrast;
        private ViewModel.EquationViewModel m_equationToFocus;
        private SortedDictionary<string, DispatcherTimerDelayer> variableSliders;
    }
}
