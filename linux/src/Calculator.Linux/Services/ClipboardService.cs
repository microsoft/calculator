// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Linux clipboard abstraction. Upstream CopyPasteManager calls WinRT
// Clipboard/DataPackage directly (3 call sites); ported ViewModels (Task 4)
// depend on this interface instead. Avalonia wiring lands with the shell
// (Task 9); ViewModels and tests use the in-memory fake until then.
using Avalonia.Input.Platform;

namespace Calculator.Linux.Services;

public interface IClipboardService
{
    void Copy(string text);
    Task<string?> GetTextAsync();
    bool HasText { get; }
}

public sealed class InMemoryClipboardService : IClipboardService
{
    private string? _text;

    public void Copy(string text) => _text = text;

    public Task<string?> GetTextAsync() => Task.FromResult(_text);

    public bool HasText => !string.IsNullOrEmpty(_text);
}

public sealed class AvaloniaClipboardService : IClipboardService
{
    private readonly IClipboard _clipboard;

    public AvaloniaClipboardService(IClipboard clipboard)
    {
        _clipboard = clipboard;
    }

    public void Copy(string text) => _clipboard.SetTextAsync(text);

    public async Task<string?> GetTextAsync() => await _clipboard.GetTextAsync();

    public bool HasText => true; // Avalonia cannot peek; VMs validate pasted text anyway
}
