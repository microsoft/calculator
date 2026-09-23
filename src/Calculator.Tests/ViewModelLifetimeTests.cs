// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

using Microsoft.VisualStudio.TestTools.UnitTesting;

using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;
using Windows.Networking.Connectivity;

namespace Calculator.Tests
{
    // Converter view models own native and network resources.
    [TestClass]
    public class ViewModelLifetimeTests
    {
        [TestMethod]
        public void ConverterViewModelsAreReleasedWhenDropped()
        {
            List<WeakReference> viewModels = CreateAndAbandonConverterViewModels(5);

            ForceFullCollection();

            int survivors = CountAlive(viewModels);
            Assert.AreEqual(
                0,
                survivors,
                $"{survivors} of {viewModels.Count} converter view models survived a full collection.");
        }

        [TestMethod]
        public void NetworkManagersAreReleasedWhenDropped()
        {
            List<WeakReference> managers = CreateAndAbandonNetworkManagers(5);

            ForceFullCollection();

            int survivors = CountAlive(managers);
            Assert.AreEqual(
                0,
                survivors,
                $"{survivors} of {managers.Count} NetworkManager instances survived a full collection. "
                + "A static WinRT event cannot hold its subscriber strongly.");
        }

        [TestMethod]
        public void NetworkManagerRevokesItsSubscriptionWhenCollected()
        {
            var source = new RecordingNetworkStatusSource();
            WeakReference manager = CreateAndAbandonNetworkManager(source);

            ForceFullCollection();

            Assert.IsFalse(manager.IsAlive);
            Assert.AreEqual(1, source.RemoveCount);
            Assert.IsNull(source.Handler);
        }

        // Keep strong locals out of the asserting stack frame.
        [MethodImpl(MethodImplOptions.NoInlining)]
        private static List<WeakReference> CreateAndAbandonConverterViewModels(int count)
        {
            var references = new List<WeakReference>(count);
            for (int i = 0; i < count; i++)
            {
                references.Add(new WeakReference(new UnitConverterViewModel()));
            }

            return references;
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        private static List<WeakReference> CreateAndAbandonNetworkManagers(int count)
        {
            var references = new List<WeakReference>(count);
            for (int i = 0; i < count; i++)
            {
                references.Add(new WeakReference(new NetworkManager()));
            }

            return references;
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        private static WeakReference CreateAndAbandonNetworkManager(RecordingNetworkStatusSource source)
        {
            return new WeakReference(new NetworkManager(source.Add, source.Remove));
        }

        private static int CountAlive(List<WeakReference> references)
        {
            int alive = 0;
            foreach (WeakReference reference in references)
            {
                if (reference.IsAlive)
                {
                    alive++;
                }
            }

            return alive;
        }

        // Finalizable objects can require more than one collection.
        private static void ForceFullCollection()
        {
            for (int pass = 0; pass < 3; pass++)
            {
                GC.Collect(GC.MaxGeneration, GCCollectionMode.Forced, blocking: true);
                GC.WaitForPendingFinalizers();
            }

            GC.Collect(GC.MaxGeneration, GCCollectionMode.Forced, blocking: true);
        }

        private sealed class RecordingNetworkStatusSource
        {
            public NetworkStatusChangedEventHandler Handler { get; private set; }
            public int RemoveCount { get; private set; }

            public void Add(NetworkStatusChangedEventHandler handler)
            {
                Handler += handler;
            }

            public void Remove(NetworkStatusChangedEventHandler handler)
            {
                Handler -= handler;
                RemoveCount++;
            }
        }
    }
}
