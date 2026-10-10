// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Task 7, slice 7c: currency end-to-end through the VM with a recorded feed.
using System.Globalization;
using System.Net;
using System.Net.Http;
using Calculator.Linux.Common;
using Calculator.Linux.DataLoaders;
using Calculator.Linux.Services;
using Calculator.Linux.ViewModels;
using Xunit;

namespace Calculator.Linux.Tests;

public sealed class CurrencyViewModelTests : IDisposable
{
    private const string RecordedLatest = """
        {"amount":1.0,"base":"USD","date":"2026-10-09","rates":{"EUR":0.89238,"JPY":158.25}}
        """;

    private readonly string _root = Path.Combine(Path.GetTempPath(), Path.GetRandomFileName());

    public void Dispose()
    {
        try
        {
            Directory.Delete(_root, recursive: true);
        }
        catch
        {
        }
    }

    private UnitConverterViewModel CreateVm()
    {
        var handler = new StubHandler(RecordedLatest);
        var client = new FrankfurterCurrencyClient(new HttpClient(handler));
        client.Initialize("USD", "en-US");
        var currencyLoader = new CurrencyDataLoader(
            "en-US",
            new LinuxSettings(Path.Combine(_root, "fx-settings")),
            new LinuxFileCache(Path.Combine(_root, "fx-cache")),
            client);
        return new UnitConverterViewModel(new LinuxSettings(Path.Combine(_root, "vm-settings")), currencyLoader);
    }

    [Fact]
    public void Currency_CategoryAppearsAndConverts()
    {
        using var vm = CreateVm();
        Assert.Contains(vm.Categories, c => c.Name.Contains("Currency"));

        vm.CurrentCategory = vm.Categories.First(c => c.Name.Contains("Currency"));
        SpinWait.SpinUntil(() => vm.Units.Count > 1, TimeSpan.FromSeconds(10));
        Assert.Contains(vm.Units, u => u.Abbreviation == "USD");
        Assert.Contains(vm.Units, u => u.Abbreviation == "EUR");

        vm.Unit1 = vm.Units.First(u => u.Abbreviation == "USD");
        vm.Unit2 = vm.Units.First(u => u.Abbreviation == "EUR");
        vm.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.One);
        vm.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Zero);
        vm.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Zero);

        Assert.Equal("100", vm.Value1);
        Assert.Equal("89.24", vm.Value2); // 100 x 0.89238, EUR rounds to 2 digits
        Assert.False(string.IsNullOrEmpty(vm.CurrencyRatioEquality));
        Assert.Equal("$", vm.CurrencySymbol1);
        Assert.False(string.IsNullOrEmpty(vm.CurrencyTimestamp));
    }

    private sealed class StubHandler : HttpMessageHandler
    {
        private readonly string _body;
        public StubHandler(string body) => _body = body;
        protected override Task<HttpResponseMessage> SendAsync(HttpRequestMessage request, CancellationToken cancellationToken) =>
            Task.FromResult(new HttpResponseMessage(HttpStatusCode.OK) { Content = new StringContent(_body) });
    }
}
