// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Frankfurter-backed currency web client (Task 7). Replaces the mock
// CurrencyHttpClient (fictional planet data) and the proprietary Bing feed.
//
// Endpoints (api.frankfurter.dev/v1 — the .app host now redirects here):
//   GET /v1/latest?base=USD  -> {"base":"USD","date":"...","rates":{"EUR":n}}
// Metadata (names, symbols, countries) is NOT in the FX feed, so it is
// derived at runtime from .NET region data (RegionInfo per specific culture):
// country = region display name, symbol = currency symbol, currency name =
// CurrencyEnglishName. Only currencies present in the FX feed are listed.
//
// Shapes returned match the loader's historic JSON contracts:
//   metadata: [{"CountryCode","CountryName","CurrencyCode","CurrencyName","CurrencySymbol"}]
//   ratios:   [{"An": code, "Rt": ratio-vs-USD}] (base USD included as 1.0)
using System.Globalization;
using System.Net.Http;
using System.Text.Json;

namespace Calculator.Linux.DataLoaders;

public sealed class FrankfurterCurrencyClient
{
    public const string EndpointLatest = "https://api.frankfurter.dev/v1/latest?base=USD";

    // Test seam (replaces upstream VIEWMODEL_FOR_UT): forced failure.
    public static bool ForceWebFailure { get; set; }

    private readonly HttpClient _http;
    private string _sourceCurrencyCode = "USD";

    public FrankfurterCurrencyClient(HttpClient? http = null)
    {
        _http = http ?? new HttpClient { Timeout = TimeSpan.FromSeconds(15) };
    }

    public void Initialize(string sourceCurrencyCode, string responseLanguage)
    {
        _sourceCurrencyCode = string.IsNullOrEmpty(sourceCurrencyCode) ? "USD" : sourceCurrencyCode;
    }

    public async Task<string?> GetCurrencyMetadataAsync()
    {
        if (ForceWebFailure)
        {
            return null;
        }
        try
        {
            // Metadata needs no network; build from region data. Kept async to
            // preserve the loader's cache/web symmetry.
            await Task.Yield();
            return BuildMetadataJson();
        }
        catch
        {
            return null;
        }
    }

    public async Task<string?> GetCurrencyRatiosAsync()
    {
        if (ForceWebFailure)
        {
            return null;
        }
        try
        {
            using var response = await _http.GetAsync(EndpointLatest);
            response.EnsureSuccessStatusCode();
            using var doc = await JsonDocument.ParseAsync(await response.Content.ReadAsStreamAsync());
            if (!doc.RootElement.TryGetProperty("rates", out JsonElement rates)
                || rates.ValueKind != JsonValueKind.Object)
            {
                return null;
            }

            var entries = new List<string>(rates.EnumerateObject().Count() + 1)
            {
                $"{{\"An\": \"{_sourceCurrencyCode}\", \"Rt\": 1.0 }}",
            };
            foreach (JsonProperty rate in rates.EnumerateObject())
            {
                if (rate.Value.ValueKind == JsonValueKind.Number && rate.Value.TryGetDouble(out double ratio) && ratio > 0)
                {
                    entries.Add($"{{\"An\": \"{rate.Name}\", \"Rt\": {ratio:R} }}");
                }
            }
            return "[ " + string.Join(", ", entries) + " ]";
        }
        catch
        {
            return null;
        }
    }

    internal static string BuildMetadataJson()
    {
        // One entry per ISO currency present in .NET region data. The issuing
        // country is preferred by matching the region code against the first
        // two letters of the currency code (ISO 4217 convention: USD -> US);
        // otherwise the current region (if it uses the currency), else first.
        var byCurrency = new Dictionary<string, List<RegionInfo>>(StringComparer.OrdinalIgnoreCase);
        foreach (CultureInfo culture in CultureInfo.GetCultures(CultureTypes.SpecificCultures))
        {
            RegionInfo region;
            try
            {
                region = new RegionInfo(culture.Name);
            }
            catch (ArgumentException)
            {
                continue;
            }
            string code = region.ISOCurrencySymbol;
            if (string.IsNullOrEmpty(code) || code.Length != 3)
            {
                continue;
            }
            if (!byCurrency.TryGetValue(code, out var list))
            {
                list = new List<RegionInfo>();
                byCurrency[code] = list;
            }
            if (list.Exists(r => r.Name == region.Name))
            {
                continue;
            }
            list.Add(region);
        }

        string currentRegion = string.Empty;
        try
        {
            currentRegion = RegionInfo.CurrentRegion.Name;
        }
        catch
        {
        }

        var entries = new List<string>(byCurrency.Count);
        foreach (var pair in byCurrency)
        {
            RegionInfo? best = null;
            foreach (RegionInfo region in pair.Value)
            {
                if (region.Name.Equals(pair.Key.Substring(0, 2), StringComparison.OrdinalIgnoreCase))
                {
                    best = region;
                    break;
                }
            }
            best ??= pair.Value.Find(r => r.Name == currentRegion) ?? pair.Value[0];

            string symbol = best.CurrencySymbol;
            if (string.IsNullOrEmpty(symbol))
            {
                continue;
            }
            entries.Add(
                "{ \"CountryCode\": \"" + JsonEncoded(best.TwoLetterISORegionName) + "\"" +
                ", \"CountryName\": \"" + JsonEncoded(best.DisplayName) + "\"" +
                ", \"CurrencyCode\": \"" + JsonEncoded(pair.Key) + "\"" +
                ", \"CurrencyName\": \"" + JsonEncoded(CurrencyEnglishName(best)) + "\"" +
                ", \"CurrencySymbol\": \"" + JsonEncoded(symbol) + "\" }");
        }
        return "[ " + string.Join(", ", entries) + " ]";
    }

    private static string CurrencyEnglishName(RegionInfo region)
    {
        try
        {
            string name = region.CurrencyEnglishName;
            if (!string.IsNullOrEmpty(name))
            {
                return name;
            }
        }
        catch
        {
        }
        return region.ISOCurrencySymbol;
    }

    private static string JsonEncoded(string value) =>
        value.Replace("\\", "\\\\").Replace("\"", "\\\"").Replace("\n", " ");
}
