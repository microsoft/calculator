// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.LogicalTree;
using Avalonia.Markup.Xaml;

namespace Calculator.Linux;

public sealed partial class AboutWindow : Window
{
    public AboutWindow()
    {
        InitializeComponent();
        string version = GetType().Assembly.GetName().Version?.ToString(3) ?? "1.0.0";
        var versionText = this.GetLogicalDescendants().OfType<TextBlock>().FirstOrDefault(t => t.Name == "VersionText");
        if (versionText != null)
        {
            versionText.Text = $"Version {version} — MIT License";
        }
    }

    private void InitializeComponent() => AvaloniaXamlLoader.Load(this);

    private void Close_Click(object? sender, RoutedEventArgs e) => Close();
}
