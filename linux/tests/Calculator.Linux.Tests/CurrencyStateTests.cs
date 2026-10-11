// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Manual-check coverage, automated: JPY (0 digits) / JOD (3 digits)
// formatting, week-old flag, stale reload, failed-load states.
using System.Net;
using System.Net.Http;
using Calculator.Linux.Common;
using Calculator.Linux.DataLoaders;
using Calculator.Linux.Services;
using Calculator.Linux.ViewModels;
using Xunit;

namespace Calculator.Linux.Tests;

public sealed class CurrencyStateTests : IDisposable
{
    private const string MockFeed = """
        {"amount":1.0,"base":"USD","date":"2026-10-09","rates":{"EUR":0.89238,"JPY":158.25,"JOD":0.709}}
        """;

    private readonly string _root = Path.Combine(Path.GetTempPath(), Path.GetRandomFileName());

    public void Dispose()
    {
        FrankfurterCurrencyClient.ForceWebFailure = false;
        try
        {
            Directory.Delete(_root, recursive: true);
        }
        catch
        {
        }
    }

    private CurrencyDataLoader NewLoader(string sub, FrankfurterCurrencyClient client) =>
        new("en-US",
            new LinuxSettings(Path.Combine(_root, sub, "settings")),
            new LinuxFileCache(Path.Combine(_root, sub, "cache")),
            client);

    private static FrankfurterCurrencyClient MockClient()
    {
        var client = new FrankfurterCurrencyClient(new HttpClient(new StubHandler(MockFeed)));
        client.Initialize("USD", "en-US");
        return client;
    }

    private static async Task WaitLoaded(CurrencyDataLoader loader)
    {
        var exited = new TaskCompletionSource(TaskCreationOptions.RunContinuationsAsynchronously);
        loader.LoadGateExitedForTest = () => exited.TrySetResult();
        loader.LoadData();
        await exited.Task.WaitAsync(TimeSpan.FromSeconds(30));
        Assert.True(loader.LoadFinished());
    }

    [Fact]
    public async Task WeekOldCache_FlagsStale()
    {
        var loader = NewLoader("a", MockClient());
        await WaitLoaded(loader);
        Assert.True(loader.LoadedFromWeb());

        // Backdate the cache timestamp 8 days; kill the web; reload.
        var settings = new LinuxSettings(Path.Combine(_root, "a", "settings"));
        settings.Set("CURRENCY_CONVERTER_TIMESTAMP", DateTimeOffset.UtcNow.AddDays(-8));
        var recorder = new Recorder();
        FrankfurterCurrencyClient.ForceWebFailure = true;
        var loader2 = NewLoader("a", MockClient());
        loader2.SetViewModelCallback(recorder);
        await WaitLoaded(loader2);

        Assert.True(loader2.LoadedFromCache());
        await recorder.StampSeen.Task.WaitAsync(TimeSpan.FromSeconds(10));
        Assert.True(recorder.WeekOld);
        Assert.False(string.IsNullOrEmpty(recorder.Timestamp));
    }

    [Fact]
    public async Task StaleCache_ReloadsFromWeb()
    {
        var loader = NewLoader("b", MockClient());
        await WaitLoaded(loader);

        var settings = new LinuxSettings(Path.Combine(_root, "b", "settings"));
        settings.Set("CURRENCY_CONVERTER_TIMESTAMP", DateTimeOffset.UtcNow.AddDays(-2));
        var loader2 = NewLoader("b", MockClient());
        await WaitLoaded(loader2);

        Assert.True(loader2.LoadedFromWeb());
    }

    [Fact]
    public async Task NoCacheWebFailure_FailsClosed()
    {
        FrankfurterCurrencyClient.ForceWebFailure = true;
        var loader = NewLoader("c", MockClient());
        await WaitLoaded(loader);

        Assert.False(loader.LoadedFromWeb());
        Assert.False(loader.LoadedFromCache());
        Assert.Empty(loader.GetOrderedUnits());
    }

    [Fact]
    public async Task ViewModel_JpyShowsNoDecimals_JodShowsThree()
    {
        var vmSettings = new LinuxSettings(Path.Combine(_root, "vm", "settings"));
        var vm = new UnitConverterViewModel(vmSettings, NewLoader("vm", MockClient()));
        try
        {
            vm.CurrentCategory = vm.Categories.First(c => c.Name.Contains("Currency"));
            SpinWait.SpinUntil(() => vm.Units.Count > 1, TimeSpan.FromSeconds(10));
            vm.Unit1 = vm.Units.First(u => u.Abbreviation == "USD");
            vm.Unit2 = vm.Units.First(u => u.Abbreviation == "JPY");
            vm.ButtonPressedCommand.Execute(Common.NumbersAndOperatorsEnum.One);
            vm.ButtonPressedCommand.Execute(Common.NumbersAndOperatorsEnum.Zero);
            vm.ButtonPressedCommand.Execute(Common.NumbersAndOperatorsEnum.Zero);
            Assert.Equal("15,825", vm.Value2); // grouped, zero fraction digits

            vm.Unit2 = vm.Units.First(u => u.Abbreviation == "JOD");
            Assert.Equal("70.900", vm.Value2);
        }
        finally
        {
            vm.Dispose();
        }
    }

    [Fact]
    public async Task ViewModel_FailedLoad_SurfacesFlag()
    {
        FrankfurterCurrencyClient.ForceWebFailure = true;
        var vm = new UnitConverterViewModel(
            new LinuxSettings(Path.Combine(_root, "vmfail", "settings")),
            NewLoader("vmfail", MockClient()));
        try
        {
            vm.CurrentCategory = vm.Categories.First(c => c.Name.Contains("Currency"));
            SpinWait.SpinUntil(() => !vm.IsCurrencyLoadingVisible, TimeSpan.FromSeconds(10));
            Assert.True(vm.CurrencyDataLoadFailed);
        }
        finally
        {
            vm.Dispose();
        }
    }

    private sealed class Recorder : IViewModelCurrencyCallback
    {
        public volatile bool WeekOld;
        public volatile string Timestamp = string.Empty;
        public readonly TaskCompletionSource StampSeen = new();

        public void CurrencyDataLoadFinished(bool didLoad)
        {
        }

        public void CurrencyTimestampUpdated(string timestamp, bool isWeekOld)
        {
            Timestamp = timestamp;
            WeekOld = isWeekOld;
            StampSeen.TrySetResult();
        }

        public void NetworkBehaviorChanged(NetworkAccessBehavior newBehavior)
        {
        }
    }

    private sealed class StubHandler : HttpMessageHandler
    {
        private readonly string _body;
        public StubHandler(string body) => _body = body;
        protected override Task<HttpResponseMessage> SendAsync(HttpRequestMessage request, CancellationToken cancellationToken) =>
            Task.FromResult(new HttpResponseMessage(HttpStatusCode.OK) { Content = new StringContent(_body) });
    }
}
