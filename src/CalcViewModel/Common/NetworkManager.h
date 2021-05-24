// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp::ViewModel::Common
{
public
    enum class NetworkAccessBehavior
    {
        Normal = 0,
        OptIn = 1,
        Offline = 2
    };

public
    delegate void NetworkBehaviorChangedHandler(NetworkAccessBehavior behavior);

public
    ref class NetworkManager sealed
    {
    public:
        NetworkManager();

        static NetworkAccessBehavior GetNetworkAccessBehavior();

        event NetworkBehaviorChangedHandler ^ NetworkBehaviorChanged;

    private:
        ~NetworkManager();

        void OnNetworkStatusChange(_In_ Platform::Object ^ sender);
        static NetworkAccessBehavior ConvertCostInfoToBehavior(_In_ Windows::Networking::Connectivity::ConnectionCost ^ connectionCost);

    private:
        Windows::Foundation::EventRegistrationToken m_NetworkStatusChangedToken;
    };
}
