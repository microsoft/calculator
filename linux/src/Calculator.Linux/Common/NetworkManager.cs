// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using Calculator.Linux.Services;

namespace Calculator.Linux.Common
{
    public enum NetworkAccessBehavior
    {
        Normal = 0,
        OptIn = 1,
        Offline = 2
    }

    public delegate void NetworkBehaviorChangedHandler(NetworkAccessBehavior behavior);

    public sealed class NetworkManager : IDisposable
    {
        public event NetworkBehaviorChangedHandler? NetworkBehaviorChanged;

        private readonly LinuxNetworkManager _inner;

        public NetworkManager()
        {
            _inner = new LinuxNetworkManager();
            _inner.BehaviorChanged += b => NetworkBehaviorChanged?.Invoke(b);
        }

        public static NetworkAccessBehavior GetNetworkAccessBehavior() => LinuxNetworkManager.GetBehavior();

        public void Dispose()
        {
            _inner.Dispose();
            GC.SuppressFinalize(this);
        }
    }
}
