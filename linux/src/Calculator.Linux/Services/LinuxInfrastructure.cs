// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Linux replacement for NetworkManager (WinRT NetworkInformation).
// Behavior delta (PORTING-NOTES.md): Linux has no metered/roaming cost API
// in .NET, so OptIn (metered consent) is never reported — the static check
// returns Normal when any interface is up, Offline otherwise. Change events
// come from System.Net.NetworkInformation.NetworkChange.
using System.Net.NetworkInformation;
using Calculator.Linux.Common;

namespace Calculator.Linux.Services;

public sealed class LinuxNetworkManager : IDisposable
{
    public event Action<NetworkAccessBehavior>? BehaviorChanged;

    public LinuxNetworkManager()
    {
        NetworkChange.NetworkAvailabilityChanged += OnAvailabilityChanged;
    }

    public static NetworkAccessBehavior GetBehavior() =>
        NetworkInterface.GetIsNetworkAvailable() ? NetworkAccessBehavior.Normal : NetworkAccessBehavior.Offline;

    public void Dispose()
    {
        NetworkChange.NetworkAvailabilityChanged -= OnAvailabilityChanged;
        GC.SuppressFinalize(this);
    }

    private void OnAvailabilityChanged(object? sender, NetworkAvailabilityEventArgs e) =>
        BehaviorChanged?.Invoke(e.IsAvailable ? NetworkAccessBehavior.Normal : NetworkAccessBehavior.Offline);
}

public interface IDispatcher
{
    void Post(Action action);
    bool HasThreadAccess { get; }
}

/// <summary>Synchronous dispatcher for tests and non-UI contexts.</summary>
public sealed class ImmediateDispatcher : IDispatcher
{
    public static readonly ImmediateDispatcher Instance = new();

    private ImmediateDispatcher()
    {
    }

    public void Post(Action action) => action();
    public bool HasThreadAccess => true;
}

/// <summary>Avalonia UI-thread dispatcher (installed by the shell).</summary>
public sealed class AvaloniaDispatcher : IDispatcher
{
    public static readonly AvaloniaDispatcher Instance = new();

    private AvaloniaDispatcher()
    {
    }

    public void Post(Action action) => Avalonia.Threading.Dispatcher.UIThread.Post(action);
    public bool HasThreadAccess => Avalonia.Threading.Dispatcher.UIThread.CheckAccess();
}
