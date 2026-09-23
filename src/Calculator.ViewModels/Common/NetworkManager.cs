// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using Windows.Networking.Connectivity;

namespace CalculatorApp.ViewModel.Common
{
    public enum NetworkAccessBehavior
    {
        Normal = 0,
        OptIn = 1,
        Offline = 2
    }

    public delegate void NetworkBehaviorChangedHandler(NetworkAccessBehavior behavior);

    public sealed class NetworkManager
    {
        public event NetworkBehaviorChangedHandler NetworkBehaviorChanged;

        private NetworkStatusChangedEventHandler _networkStatusChangedHandler;
        private readonly Action<NetworkStatusChangedEventHandler> _unsubscribe;

        public NetworkManager()
            : this(
                handler => NetworkInformation.NetworkStatusChanged += handler,
                handler => NetworkInformation.NetworkStatusChanged -= handler)
        {
        }

        internal NetworkManager(
            Action<NetworkStatusChangedEventHandler> subscribe,
            Action<NetworkStatusChangedEventHandler> unsubscribe)
        {
            if (subscribe == null) throw new ArgumentNullException(nameof(subscribe));
            _unsubscribe = unsubscribe ?? throw new ArgumentNullException(nameof(unsubscribe));

            // A static event must not strongly root its manager.
            var weakThis = new WeakReference<NetworkManager>(this);
            NetworkStatusChangedEventHandler handler = null;
            handler = sender =>
            {
                if (weakThis.TryGetTarget(out NetworkManager self))
                {
                    self.OnNetworkStatusChange(sender);
                }
                else
                {
                    unsubscribe(handler);
                }
            };

            _networkStatusChangedHandler = handler;
            subscribe(handler);
        }

        ~NetworkManager()
        {
            // Finalizer exceptions terminate the process.
            try
            {
                NetworkStatusChangedEventHandler handler = _networkStatusChangedHandler;
                _networkStatusChangedHandler = null;
                if (handler != null)
                {
                    _unsubscribe(handler);
                }
            }
            catch (Exception)
            {
            }
        }

        public static NetworkAccessBehavior GetNetworkAccessBehavior()
        {
            NetworkAccessBehavior behavior = NetworkAccessBehavior.Offline;
            ConnectionProfile connectionProfile = NetworkInformation.GetInternetConnectionProfile();
            if (connectionProfile != null)
            {
                NetworkConnectivityLevel connectivityLevel = connectionProfile.GetNetworkConnectivityLevel();
                if (connectivityLevel == NetworkConnectivityLevel.InternetAccess
                    || connectivityLevel == NetworkConnectivityLevel.ConstrainedInternetAccess)
                {
                    ConnectionCost connectionCost = connectionProfile.GetConnectionCost();
                    behavior = ConvertCostInfoToBehavior(connectionCost);
                }
            }

            return behavior;
        }

        private void OnNetworkStatusChange(object sender)
        {
            NetworkBehaviorChanged?.Invoke(GetNetworkAccessBehavior());
        }

        // See app behavior guidelines at https://msdn.microsoft.com/en-us/library/windows/apps/xaml/jj835821(v=win.10).aspx
        private static NetworkAccessBehavior ConvertCostInfoToBehavior(ConnectionCost connectionCost)
        {
            if (connectionCost.Roaming || connectionCost.OverDataLimit
                || connectionCost.NetworkCostType == NetworkCostType.Variable
                || connectionCost.NetworkCostType == NetworkCostType.Fixed)
            {
                return NetworkAccessBehavior.OptIn;
            }

            return NetworkAccessBehavior.Normal;
        }
    }
}
