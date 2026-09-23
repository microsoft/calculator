// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Text.Json;

using CalculatorApp.JsonUtils;
using CalculatorApp.ViewModel.Common;
using CalculatorApp.ViewModel.Snapshot;

namespace CalculatorApp
{
    internal sealed class SnapshotLaunchArguments
    {
        public bool HasError { get; private set; }
        public ApplicationSnapshot Snapshot { get; private set; }

        internal static SnapshotLaunchArguments Error()
        {
            return new SnapshotLaunchArguments { HasError = true };
        }

        internal static SnapshotLaunchArguments FromJson(string json)
        {
            try
            {
                var alias = JsonSerializer.Deserialize<ApplicationSnapshotAlias>(json)
                    ?? throw new JsonException("Snapshot payload is null.");
                SnapshotValidator.ValidateProtocol(alias.Value);
                return new SnapshotLaunchArguments { Snapshot = alias.Value };
            }
            catch (Exception ex)
            {
                TraceLogger.GetInstance().LogRecallError(
                    $"Error occurs during the deserialization of Snapshot. Exception: {ex}");
                return Error();
            }
        }
    }
}
