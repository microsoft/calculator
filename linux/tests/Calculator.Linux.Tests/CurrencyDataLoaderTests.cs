// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Task 7, slice 7b: CurrencyDataLoader cache/web/stale/offline behavior with
// a recorded FX feed (no network) plus ratio math vectors.
using System.Net;
using System.Net.Http;
using Calculator.Linux.DataLoaders;
using Calculator.Linux.Services;
using Xunit;

namespace Calculator.Linux.Tests;

public sealed class CurrencyDataLoaderTests : IDisposable
{
    private const string RecordedLatest = """
        {"amount":1.0,"base":"USD","date":"2026-10-09","rates":{"EUR":0.89238,"JPY":158.25}}
        """;

    private readonly string _root = Path.Combine(Path.GetTempPath(), Path.GetRandomFileName());
    private string SettingsDir => Path.Combine(_root, "settings");
    private string CacheDir => Path.Combine(_root, "cache");

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

    private CurrencyDataLoader CreateLoader(int webCallsRefCount = 0)
    {
        var handler = new CountingHandler(RecordedLatest);
        var client = new FrankfurterCurrencyClient(new HttpClient(handler));
        client.Initialize("USD", "en-US");
        var loader = new CurrencyDataLoader(
            "en-US",
            new LinuxSettings(SettingsDir),
            new LinuxFileCache(CacheDir),
            client);
        loader.LoadGateExitedForTest = () => { };
        return loader;
    }

    private static async Task WaitLoaded(CurrencyDataLoader loader)
    {
        // Gate on the exited hook (fires in LoadData's finally, strictly after
        // Notify): true completion signal, immune to flag-polling races.
        var exited = new TaskCompletionSource(TaskCreationOptions.RunContinuationsAsynchronously);
        loader.LoadGateExitedForTest = () => exited.TrySetResult();
        loader.LoadData();
        await exited.Task.WaitAsync(TimeSpan.FromSeconds(30));
        Assert.True(loader.LoadFinished());
    }

    [Fact]
    public async Task WebLoad_PopulatesUnitsAndRatios()
    {
        var loader = CreateLoader();
        await WaitLoaded(loader);

        Assert.True(loader.LoadedFromWeb());
        var units = loader.GetOrderedUnits();
        Assert.Contains(units, u => u.Abbreviation == "USD");
        Assert.Contains(units, u => u.Abbreviation == "EUR");
        Assert.Contains(units, u => u.Abbreviation == "JPY");

        var usd = units.First(u => u.Abbreviation == "USD");
        var ratios = loader.LoadOrderedRatios(usd.Id);
        Assert.True(ratios.ContainsKey(units.First(u => u.Abbreviation == "EUR").Id));
    }

    [Fact]
    public async Task SecondInstance_LoadsFromCacheWithoutWeb()
    {
        var first = CreateLoader();
        await WaitLoaded(first);
        Assert.True(first.LoadedFromWeb());

        FrankfurterCurrencyClient.ForceWebFailure = true;
        var second = new CurrencyDataLoader(
            "en-US",
            new LinuxSettings(SettingsDir),
            new LinuxFileCache(CacheDir),
            new FrankfurterCurrencyClient(new HttpClient(new CountingHandler(RecordedLatest))));
        await WaitLoaded(second);

        Assert.True(second.LoadedFromCache());
        Assert.Contains(second.GetOrderedUnits(), u => u.Abbreviation == "EUR");
    }

    [Fact]
    public async Task RatioEquality_FormatsBothNames()
    {
        var loader = CreateLoader();
        await WaitLoaded(loader);

        var units = loader.GetOrderedUnits();
        int usd = units.First(u => u.Abbreviation == "USD").Id;
        int eur = units.First(u => u.Abbreviation == "EUR").Id;
        var (ratio, _) = loader.GetCurrencyRatioEquality(usd, eur);
        Assert.Contains("USD", ratio);
        Assert.Contains("EUR", ratio);
    }

    [Fact]
    public async Task Symbols_ReturnDollarAndEuro()
    {
        var loader = CreateLoader();
        await WaitLoaded(loader);

        var units = loader.GetOrderedUnits();
        int usd = units.First(u => u.Abbreviation == "USD").Id;
        int eur = units.First(u => u.Abbreviation == "EUR").Id;
        var (s1, s2) = loader.GetCurrencySymbols(usd, eur);
        Assert.Equal("$", s1);
        Assert.NotEqual(string.Empty, s2);
    }

    [Fact]
    public void RoundCurrencyRatio_KeepsSignificantDigits()
    {
        Assert.Equal(0.8924, CurrencyDataLoader.RoundCurrencyRatio(0.89238), precision: 4);
        Assert.Equal(158.25, CurrencyDataLoader.RoundCurrencyRatio(158.25), precision: 2);
    }

    [Fact]
    public async Task Timestamp_ProvidedAfterLoad()
    {
        var loader = CreateLoader();
        await WaitLoaded(loader);
        Assert.False(string.IsNullOrEmpty(loader.GetCurrencyTimestamp()));
    }

    private sealed class CountingHandler : HttpMessageHandler
    {
        private readonly string _body;

        public CountingHandler(string body)
        {
            _body = body;
        }

        protected override Task<HttpResponseMessage> SendAsync(HttpRequestMessage request, CancellationToken cancellationToken) =>
            Task.FromResult(new HttpResponseMessage(HttpStatusCode.OK) { Content = new StringContent(_body) });
    }
}
