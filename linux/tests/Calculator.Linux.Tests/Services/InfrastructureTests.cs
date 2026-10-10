// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Task 3, slice E (RED): clipboard abstraction, network behavior,
// dispatcher abstraction, and no-op trace logger.
using Calculator.Linux.Common;
using Calculator.Linux.Services;
using Xunit;

namespace Calculator.Linux.Tests.Services;

public sealed class InfrastructureTests
{
    [Fact]
    public async Task InMemoryClipboard_RoundTripsText()
    {
        IClipboardService clipboard = new InMemoryClipboardService();
        Assert.False(clipboard.HasText);

        clipboard.Copy("12+34");
        Assert.True(clipboard.HasText);
        Assert.Equal("12+34", await clipboard.GetTextAsync());
    }

    [Fact]
    public void NetworkBehavior_ReturnsKnownValueWithoutThrowing()
    {
        NetworkAccessBehavior behavior = LinuxNetworkManager.GetBehavior();
        Assert.True(behavior is NetworkAccessBehavior.Normal or NetworkAccessBehavior.Offline);
    }

    [Fact]
    public void NetworkManager_ConstructsAndDisposes()
    {
        using var manager = new LinuxNetworkManager();
        NetworkAccessBehavior? seen = null;
        manager.BehaviorChanged += b => seen = b;
        Assert.Null(seen); // no synthetic event on subscribe
    }

    [Fact]
    public void ImmediateDispatcher_RunsInline()
    {
        IDispatcher dispatcher = ImmediateDispatcher.Instance;
        Assert.True(dispatcher.HasThreadAccess);
        bool ran = false;
        dispatcher.Post(() => ran = true);
        Assert.True(ran);
    }

    [Fact]
    public void TraceLogger_NoOpsNeverThrow()
    {
        var log = LinuxTraceLogger.Instance;
        log.LogError(ViewMode.Standard, "Fn", "Err");
        log.LogPlatformException(ViewMode.Currency, "Fn", new InvalidOperationException("x"));
        log.UpdateButtonUsage(NumbersAndOperatorsEnum.Add, ViewMode.Standard);
        log.LogMemoryItemLoad(ViewMode.Standard, 3, 0);
        log.LogHistoryItemLoad(ViewMode.Scientific, 3, 0);
        log.LogModeChange(ViewMode.Programmer);
        log.LogWindowCreated(ViewMode.Standard, 1);
        log.LogRecallError("boom");
        log.LogInputPasted(ViewMode.Standard);
        log.LogGraphSettingsChanged(GraphSettingsType.Grid, "v");
    }
}
