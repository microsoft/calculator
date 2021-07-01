// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "NetworkManager.h"

using namespace CalculatorApp;
using namespace CalculatorApp::ViewModel;
using namespace CalculatorApp::ViewModel::Common;
using namespace Platform;
using namespace Windows::Networking::Connectivity;

NetworkManager::NetworkManager()
{
    m_NetworkStatusChangedToken = NetworkInformation::NetworkStatusChanged +=
        ref new NetworkStatusChangedEventHandler(this, &NetworkManager::OnNetworkStatusChange, CallbackContext::Same);
}

NetworkManager::~NetworkManager()
{
    NetworkInformation::NetworkStatusChanged -= m_NetworkStatusChangedToken;
}

NetworkAccessBehavior NetworkManager::GetNetworkAccessBehavior()
{
    NetworkAccessBehavior behavior = NetworkAccessBehavior::Offline;
    ConnectionProfile ^ connectionProfile = NetworkInformation::GetInternetConnectionProfile();
    if (connectionProfile != nullptr)
    {
        NetworkConnectivityLevel connectivityLevel = connectionProfile->GetNetworkConnectivityLevel();
        if (connectivityLevel == NetworkConnectivityLevel::InternetAccess || connectivityLevel == NetworkConnectivityLevel::ConstrainedInternetAccess)
        {
            ConnectionCost ^ connectionCost = connectionProfile->GetConnectionCost();
            behavior = ConvertCostInfoToBehavior(connectionCost);
        }
    }

    return behavior;
}

void NetworkManager::OnNetworkStatusChange(_In_ Object ^ /*sender*/)
{
    NetworkBehaviorChanged(GetNetworkAccessBehavior());
}

// See app behavior guidelines at https://msdn.microsoft.com/en-us/library/windows/apps/xaml/jj835821(v=win.10).aspx
NetworkAccessBehavior NetworkManager::ConvertCostInfoToBehavior(_In_ ConnectionCost ^ connectionCost)
{
    if (connectionCost->Roaming || connectionCost->OverDataLimit || connectionCost->NetworkCostType == NetworkCostType::Variable
        || connectionCost->NetworkCostType == NetworkCostType::Fixed)
    {
        return NetworkAccessBehavior::OptIn;
    }

    return NetworkAccessBehavior::Normal;
}
