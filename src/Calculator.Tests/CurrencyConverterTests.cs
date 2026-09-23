// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Globalization;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;
using CalculatorApp.ViewModel.DataLoaders;
using Windows.Storage;

namespace Calculator.Tests
{
    [TestClass]
    public class CurrencyConverterLoadTests
    {
        [TestMethod]
        public async Task LoadFromCache_Fail_NoCacheKey()
        {
            var localSettings = ApplicationData.Current.LocalSettings;
            localSettings.Values.Remove(CurrencyDataLoaderConstants.CacheTimestampKey);

            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();

            Assert.IsFalse(didLoad, "Loading from cache must fail when the cache timestamp key is absent");
            Assert.IsFalse(loader.LoadFinished());
            Assert.IsFalse(loader.LoadedFromCache());
        }

        [TestMethod]
        public async Task InitialLoadAndRefreshSerializeCurrencyMutation()
        {
            var firstEntry = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);
            var releaseFirstEntry = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);
            var initialLoadFinished = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);
            int entryCount = 0;
            int activeEntries = 0;
            int overlapDetected = 0;

            var loader = new CurrencyDataLoader("en-US");
            loader.LoadGateEnteredForTest = async () =>
            {
                if (Interlocked.Increment(ref activeEntries) > 1)
                {
                    Interlocked.Exchange(ref overlapDetected, 1);
                }

                int entry = Interlocked.Increment(ref entryCount);
                if (entry == 1)
                {
                    firstEntry.SetResult(true);
                    await releaseFirstEntry.Task;
                }
            };
            loader.LoadGateExitedForTest = () => Interlocked.Decrement(ref activeEntries);
            loader.SetViewModelCallback(new LoadCompletionCallback(initialLoadFinished));

            loader.LoadData();
            await firstEntry.Task;
            Task<bool> refresh = loader.TryLoadDataFromWebOverrideAsync();
            releaseFirstEntry.SetResult(true);

            await initialLoadFinished.Task;
            await refresh;

            Assert.AreEqual(0, overlapDetected);
            Assert.AreEqual(2, entryCount);
        }

        private static async Task<DateTimeOffset> PrimeCacheAsync(
            string languageCode = "en-US",
            DateTimeOffset? timestamp = null,
            bool writeStaticData = true,
            bool writeRatios = true,
            string staticDataOverride = null,
            string ratiosOverride = null)
        {
            var stamp = timestamp ?? DateTimeOffset.UtcNow;
            var localSettings = ApplicationData.Current.LocalSettings;
            localSettings.Values[CurrencyDataLoaderConstants.CacheTimestampKey] = stamp;
            localSettings.Values[CurrencyDataLoaderConstants.CacheLangcodeKey] = languageCode;

            var folder = ApplicationData.Current.LocalCacheFolder;
            var staticData = staticDataOverride ?? await new CurrencyHttpClient().GetCurrencyMetadataAsync();
            var ratios = ratiosOverride ?? await new CurrencyHttpClient().GetCurrencyRatiosAsync();

            await WriteOrDeleteAsync(folder, CurrencyDataLoaderConstants.StaticDataFilename, writeStaticData ? staticData : null);
            await WriteOrDeleteAsync(folder, CurrencyDataLoaderConstants.AllRatiosDataFilename, writeRatios ? ratios : null);

            return stamp;
        }

        private static async Task WriteOrDeleteAsync(StorageFolder folder, string name, string contents)
        {
            if (contents == null)
            {
                var existing = await folder.TryGetItemAsync(name);
                if (existing != null)
                {
                    await existing.DeleteAsync();
                }
                return;
            }

            var file = await folder.CreateFileAsync(name, CreationCollisionOption.ReplaceExisting);
            await FileIO.WriteTextAsync(file, contents);
        }

        private static async Task<CurrencyDataLoader> LoadedLoaderAsync()
        {
            await PrimeCacheAsync();
            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();
            Assert.IsTrue(didLoad, "Cache load failed while setting up the test.");
            return loader;
        }

        [TestMethod]
        public async Task LoadFromCache_Success()
        {
            await PrimeCacheAsync();

            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();

            Assert.IsTrue(didLoad);
            Assert.IsTrue(loader.LoadFinished());
            Assert.IsTrue(loader.LoadedFromCache());
        }

        [TestMethod]
        public async Task LoadFromCache_Fail_StaticDataFileDoesNotExist()
        {
            await PrimeCacheAsync(writeStaticData: false);

            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();

            Assert.IsFalse(didLoad, "A cache load with no static data file must fail.");
            Assert.IsFalse(loader.LoadedFromCache());
        }

        [TestMethod]
        public async Task LoadFromCache_Fail_AllRatiosDataFileDoesNotExist()
        {
            await PrimeCacheAsync(writeRatios: false);

            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();

            Assert.IsFalse(didLoad, "A cache load with no ratios file must fail.");
            Assert.IsFalse(loader.LoadedFromCache());
        }

        [TestMethod]
        public async Task LoadFromCache_Fail_ResponseLanguageChanged()
        {
            // The cached text is localized, so a cache written for another language cannot be used.
            await PrimeCacheAsync(languageCode: "ar-SA");

            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();

            Assert.IsFalse(didLoad, "A cache written for a different language must not be reused.");
            Assert.IsFalse(loader.LoadedFromCache());
        }

        [TestMethod]
        public async Task LoadFromCache_SortsCurrenciesByCountryName()
        {
            const string staticData = @"[
                {""CountryCode"":""AA"",""CountryName"":""Zebra"",""CurrencyCode"":""AAA"",""CurrencyName"":""Alpha Coin"",""CurrencySymbol"":""A""},
                {""CountryCode"":""ZZ"",""CountryName"":""Alpha"",""CurrencyCode"":""ZZZ"",""CurrencyName"":""Zulu Coin"",""CurrencySymbol"":""Z""},
                {""CountryCode"":""MM"",""CountryName"":""Éclair"",""CurrencyCode"":""MMM"",""CurrencyName"":""Mike Coin"",""CurrencySymbol"":""M""}
            ]";
            const string ratios = @"[
                {""Rt"":1.0,""An"":""AAA""},
                {""Rt"":2.0,""An"":""ZZZ""},
                {""Rt"":3.0,""An"":""MMM""}
            ]";

            var originalCulture = CultureInfo.CurrentCulture;
            try
            {
                CultureInfo.CurrentCulture = new CultureInfo("en-US");
                await PrimeCacheAsync(staticDataOverride: staticData, ratiosOverride: ratios);
                var loader = new CurrencyDataLoader("en-US");

                Assert.IsTrue(await loader.TryLoadDataFromCacheAsync());

                CollectionAssert.AreEqual(
                    new[] { "Alpha", "Éclair", "Zebra" },
                    loader.GetOrderedUnits(0).ConvertAll(unit => unit.CountryName));
            }
            finally
            {
                CultureInfo.CurrentCulture = originalCulture;
                await PrimeCacheAsync();
            }
        }

        [TestMethod]
        public async Task Loaded_LoadOrderedUnits()
        {
            var loader = await LoadedLoaderAsync();

            var units = loader.GetOrderedUnits(0);

            Assert.IsTrue(units.Count > 0, "No currency units were loaded.");
            foreach (var unit in units)
            {
                Assert.IsFalse(string.IsNullOrEmpty(unit.Abbreviation), "A currency unit had no abbreviation.");
                Assert.IsFalse(string.IsNullOrEmpty(unit.CountryName), "A currency unit had no country name.");
            }
        }

        [TestMethod]
        public async Task Loaded_LoadOrderedRatios()
        {
            var loader = await LoadedLoaderAsync();
            var units = loader.GetOrderedUnits(0);

            var ratios = loader.LoadOrderedRatios(units[0].Id);

            Assert.IsTrue(ratios.Count > 0, "The first currency unit had no ratios.");
            Assert.IsTrue(
                ratios.ContainsKey(units[0].Id),
                "A currency should always convert to itself.");
        }

        [TestMethod]
        public async Task Loaded_GetCurrencySymbols_Valid()
        {
            var loader = await LoadedLoaderAsync();
            var units = loader.GetOrderedUnits(0);

            var symbols = loader.GetCurrencySymbols(units[0].Id, units[1].Id);

            Assert.IsNotNull(symbols.Symbol1);
            Assert.IsNotNull(symbols.Symbol2);
        }

        [TestMethod]
        public async Task Loaded_GetCurrencySymbols_Invalid()
        {
            var loader = await LoadedLoaderAsync();

            var symbols = loader.GetCurrencySymbols(-1, -2);

            Assert.AreEqual(string.Empty, symbols.Symbol1, "An unknown unit must not yield a symbol.");
            Assert.AreEqual(string.Empty, symbols.Symbol2);
        }

        [TestMethod]
        public async Task Loaded_GetCurrencyRatioEquality_Valid()
        {
            var loader = await LoadedLoaderAsync();
            var units = loader.GetOrderedUnits(0);

            var equality = loader.GetCurrencyRatioEquality(units[0].Id, units[1].Id);

            Assert.IsFalse(string.IsNullOrEmpty(equality.Ratio1), "The ratio line was empty.");
            Assert.IsFalse(string.IsNullOrEmpty(equality.Ratio2), "The accessible ratio line was empty.");

            // The accessible form spells the currencies out rather than abbreviating them.
            StringAssert.Contains(equality.Ratio2, units[0].CountryName);
            StringAssert.Contains(equality.Ratio2, units[0].Name);
        }

        [TestMethod]
        public async Task Loaded_GetCurrencyRatioEquality_Invalid()
        {
            var loader = await LoadedLoaderAsync();

            var equality = loader.GetCurrencyRatioEquality(-1, -2);

            Assert.AreEqual(string.Empty, equality.Ratio1);
            Assert.AreEqual(string.Empty, equality.Ratio2);
        }

        [TestMethod]
        public async Task LoadFromWeb_Success()
        {
            var loader = new CurrencyDataLoader("en-US");

            bool didLoad = await loader.TryLoadDataFromWebAsync();

            Assert.IsTrue(didLoad);
            Assert.IsTrue(loader.LoadFinished());
            Assert.IsTrue(loader.LoadedFromWeb());
        }

        [TestMethod]
        public async Task Load_Success_LoadedFromCache()
        {
            // A cache written moments ago is still fresh, so the loader uses it and never goes out.
            await PrimeCacheAsync();

            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();

            Assert.IsTrue(didLoad);
            Assert.IsTrue(loader.LoadedFromCache());
            Assert.IsFalse(loader.LoadedFromWeb());
        }

        [TestMethod]
        public async Task Load_Success_LoadedFromWeb()
        {
            // A cache older than a day is refreshed from the web rather than used as-is.
            await PrimeCacheAsync(timestamp: DateTimeOffset.UtcNow.AddDays(-2));

            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();

            Assert.IsTrue(didLoad);
            Assert.IsTrue(loader.LoadedFromWeb(), "A stale cache should have been refreshed from the web.");
        }


        [TestMethod]
        public void Test_RoundCurrencyRatio()
        {
            (double Ratio, double Expected)[] cases =
            {
                (1234567, 1234567),
                (0, 0),
                (9999.999, 9999.999),
                (8765.4321, 8765.4321),
                (4815.162342, 4815.1623),
                (4815.162358, 4815.1624),
                (4815.162388934723, 4815.1624),
                (0.12, 0.12),
                (0.123, 0.123),
                (0.1234, 0.1234),
                (0.12343, 0.1234),
                (0.0321, 0.0321),
                (0.03211, 0.03211),
                (0.032119, 0.03212),
                (0.00322119, 0.003221),
                (0.00123269, 0.001233),
                (0.00076269, 0.0007627),
                (0.000069, 0.000069),
                (0.000061, 0.000061),
                (0.000054612, 0.00005461),
                (0.000054616, 0.00005462),
                (0.000005416, 0.000005416),
                (0.0000016134324, 0.000001613),
                (0.0000096134324, 0.000009613),
                (0.0000032169348392, 0.000003217),
                (0.000000002134987218, 0.000000002135),
                (0.000000000000087231445, 0.00000000000008723),
            };

            foreach (var (ratio, expected) in cases)
            {
                Assert.AreEqual(expected, CurrencyDataLoader.RoundCurrencyRatio(ratio), 0d,
                    $"RoundCurrencyRatio({ratio})");
            }
        }

        private sealed class LoadCompletionCallback : IViewModelCurrencyCallback
        {
            private readonly TaskCompletionSource<bool> _completion;

            public LoadCompletionCallback(TaskCompletionSource<bool> completion)
            {
                _completion = completion;
            }

            public void CurrencyDataLoadFinished(bool didLoad)
            {
                _completion.TrySetResult(didLoad);
            }

            public void CurrencyTimestampUpdated(string timestamp, bool isWeekOld)
            {
            }

            public void NetworkBehaviorChanged(NetworkAccessBehavior newBehavior)
            {
            }
        }
    }
}
