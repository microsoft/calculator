// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Task 7, slice 7a: Frankfurter client with recorded responses (no network).
using System.Net;
using System.Net.Http;
using System.Text.Json;
using Calculator.Linux.DataLoaders;
using Xunit;

namespace Calculator.Linux.Tests;

public sealed class FrankfurterClientTests : IDisposable
{
    private const string RecordedLatest = """
        {"amount":1.0,"base":"USD","date":"2026-10-09","rates":{"EUR":0.89238,"JPY":158.25,"GBP":0.75641}}
        """;

    public FrankfurterClientTests() => FrankfurterCurrencyClient.ForceWebFailure = false;
    public void Dispose() => FrankfurterCurrencyClient.ForceWebFailure = false;

    private static FrankfurterCurrencyClient WithResponse(string json, HttpStatusCode code = HttpStatusCode.OK)
    {
        var handler = new RecordedHandler(json, code);
        return new FrankfurterCurrencyClient(new HttpClient(handler));
    }

    [Fact]
    public async Task Ratios_ParseIncludesBaseAtOne()
    {
        var client = WithResponse(RecordedLatest);
        client.Initialize("USD", "en-US");

        string? json = await client.GetCurrencyRatiosAsync();
        Assert.NotNull(json);

        using var doc = JsonDocument.Parse(json);
        var map = new Dictionary<string, double>();
        foreach (JsonElement el in doc.RootElement.EnumerateArray())
        {
            map[el.GetProperty("An").GetString()!] = el.GetProperty("Rt").GetDouble();
        }
        Assert.Equal(1.0, map["USD"]);
        Assert.Equal(0.89238, map["EUR"], precision: 5);
        Assert.Equal(158.25, map["JPY"], precision: 2);
    }

    [Fact]
    public async Task Ratios_HttpError_ReturnsNull()
    {
        var client = WithResponse("oops", HttpStatusCode.InternalServerError);
        Assert.Null(await client.GetCurrencyRatiosAsync());
    }

    [Fact]
    public async Task Ratios_ForcedFailure_ReturnsNull()
    {
        FrankfurterCurrencyClient.ForceWebFailure = true;
        var client = WithResponse(RecordedLatest);
        Assert.Null(await client.GetCurrencyRatiosAsync());
        Assert.Null(await client.GetCurrencyMetadataAsync());
    }

    [Fact]
    public async Task Metadata_ContainsUsdWithDollarSymbol()
    {
        var client = WithResponse(RecordedLatest);
        string? json = await client.GetCurrencyMetadataAsync();
        Assert.NotNull(json);

        using var doc = JsonDocument.Parse(json);
        bool foundUsd = false;
        foreach (JsonElement el in doc.RootElement.EnumerateArray())
        {
            if (el.GetProperty("CurrencyCode").GetString() == "USD")
            {
                foundUsd = true;
                Assert.Equal("$", el.GetProperty("CurrencySymbol").GetString());
                Assert.False(string.IsNullOrEmpty(el.GetProperty("CountryName").GetString()));
                Assert.False(string.IsNullOrEmpty(el.GetProperty("CurrencyName").GetString()));
            }
        }
        Assert.True(foundUsd);
        Assert.True(doc.RootElement.GetArrayLength() >= 20);
    }

    private sealed class RecordedHandler : HttpMessageHandler
    {
        private readonly string _body;
        private readonly HttpStatusCode _code;

        public RecordedHandler(string body, HttpStatusCode code)
        {
            _body = body;
            _code = code;
        }

        protected override Task<HttpResponseMessage> SendAsync(HttpRequestMessage request, CancellationToken cancellationToken)
        {
            Assert.StartsWith("https://api.frankfurter.dev/v1/latest", request.RequestUri!.ToString());
            return Task.FromResult(new HttpResponseMessage(_code) { Content = new StringContent(_body) });
        }
    }

    [Fact]
    public void Metadata_UsesEnglishCountryNames()
    {
        // Regression: the region for USD was picked from CultureInfo's culture
        // list, which contains "chr-US", and the name came from
        // RegionInfo.DisplayName (UI-culture dependent) — so USD rendered as
        // "<Cherokee> - US Dollar" and EUR as "España - Euro".
        string json = FrankfurterCurrencyClient.BuildMetadataJson();

        Assert.Contains("\"CountryCode\": \"US\", \"CountryName\": \"United States\"", json);
        Assert.Contains("\"CountryCode\": \"ES\", \"CountryName\": \"Spain\"", json);
        Assert.Contains("\"CountryCode\": \"GB\", \"CountryName\": \"United Kingdom\"", json);
    }

    [Fact]
    public void Metadata_CountryNamesAreCultureIndependent()
    {
        // Must equal RegionInfo.EnglishName, never DisplayName: the latter
        // follows the UI culture ("España", "Deutschland", Cherokee for chr-US).
        using var document = System.Text.Json.JsonDocument.Parse(
            FrankfurterCurrencyClient.BuildMetadataJson());

        var byCurrency = document.RootElement.EnumerateArray().ToDictionary(
            e => e.GetProperty("CurrencyCode").GetString()!,
            e => e.GetProperty("CountryCode").GetString()!);

        foreach (string currency in new[] { "USD", "EUR", "GBP", "JPY", "TRY", "CHF", "AUD" })
        {
            if (!byCurrency.TryGetValue(currency, out string? countryCode))
            {
                continue;
            }
            string expected = new System.Globalization.RegionInfo(countryCode!).EnglishName;
            string actual = document.RootElement.EnumerateArray()
                .First(e => e.GetProperty("CurrencyCode").GetString() == currency)
                .GetProperty("CountryName").GetString()!;
            Assert.Equal(expected, actual);
        }
    }
}
