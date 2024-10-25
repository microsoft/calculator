using Windows.ApplicationModel.Activation;
using CalculatorApp.ViewModel.Snapshot;

namespace CalculatorApp
{
    internal class SnapshotLaunchArguments
    {
        public bool HasError { get; set; }
        public ApplicationSnapshot Snapshot { get; set; }
    }

    internal static class LaunchExtensions
    {
        public static bool TryGetSnapshotProtocol(this IActivatedEventArgs args, out IProtocolActivatedEventArgs result)
        {
            result = null;
            var protoArgs = args as IProtocolActivatedEventArgs;
            if (protoArgs == null ||
                protoArgs.Uri == null ||
                protoArgs.Uri.Segments == null ||
                protoArgs.Uri.Segments.Length < 2 ||
                protoArgs.Uri.Segments[0] != "snapshot/")
            {
                return false;
            }
            result = protoArgs;
            return true;
        }

        public static SnapshotLaunchArguments GetSnapshotLaunchArgs(this IProtocolActivatedEventArgs args)
        {
            return null;
        }
    }
}
