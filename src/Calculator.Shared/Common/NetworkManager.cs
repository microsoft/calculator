// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Text;
using Windows.Networking.Connectivity;

namespace CalculatorApp
{
	public enum NetworkAccessBehavior
    {
        Normal = 0,
        OptIn = 1,
        Offline = 2
    };

	public delegate void NetworkBehaviorChangedHandler(NetworkAccessBehavior behavior);

	public sealed class NetworkManager
	{
		public event NetworkBehaviorChangedHandler NetworkBehaviorChanged;

		public NetworkManager()
		{
			// TODO: UNO (Not implemented)
			//NetworkInformation.NetworkStatusChanged += new NetworkStatusChangedEventHandler(OnNetworkStatusChange);
		}

		~NetworkManager()
		{
			// TODO: UNO (Not implemented)
			//NetworkInformation.NetworkStatusChanged -= OnNetworkStatusChange;
		}

		public static NetworkAccessBehavior GetNetworkAccessBehavior()
		{
			// TODO: UNO (Not implemented)
			return NetworkAccessBehavior.Normal;

			//NetworkAccessBehavior behavior = NetworkAccessBehavior.Offline;
			//ConnectionProfile connectionProfile = NetworkInformation.GetInternetConnectionProfile();
			//if (connectionProfile != null)
			//{
			//	NetworkConnectivityLevel connectivityLevel = connectionProfile.GetNetworkConnectivityLevel();
			//	if (connectivityLevel == NetworkConnectivityLevel.InternetAccess || connectivityLevel == NetworkConnectivityLevel.ConstrainedInternetAccess)
			//	{
			//		ConnectionCost connectionCost = connectionProfile.GetConnectionCost();
			//		behavior = ConvertCostInfoToBehavior(connectionCost);
			//	}
			//}

			//return behavior;
		}

		void OnNetworkStatusChange(object sender)
		{
			NetworkBehaviorChanged?.Invoke(GetNetworkAccessBehavior());
		}

		// See app behavior guidelines at https://msdn.microsoft.com/en-us/library/windows/apps/xaml/jj835821(v=win.10).aspx
		static NetworkAccessBehavior ConvertCostInfoToBehavior(ConnectionCost connectionCost)
		{
			if (connectionCost.Roaming || connectionCost.OverDataLimit || connectionCost.NetworkCostType == NetworkCostType.Variable
				|| connectionCost.NetworkCostType == NetworkCostType.Fixed)
			{
				return NetworkAccessBehavior.OptIn;
			}

			return NetworkAccessBehavior.Normal;
		}
	}
}
