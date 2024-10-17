using System;
using Windows.ApplicationModel.Activation;
using Windows.ApplicationModel.UserActivities;
using CalculatorApp.ViewModel;

namespace CalculatorApp
{
    internal class SnapshotLaunchArguments
    {
        public ApplicationSnapshot Snapshot;
    }

    internal static class LaunchExtensions
    {
        public static bool IsSnapshotProtocol(this IActivatedEventArgs args) =>
            args is IProtocolActivatedEventArgs protoArgs &&
            protoArgs.Uri != null &&
            protoArgs.Uri.Segments != null &&
            protoArgs.Uri.Segments.Length == 2 &&
            protoArgs.Uri.Segments[0] == "snapshot/";
    }
}
