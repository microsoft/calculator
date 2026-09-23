using System;
using System.Linq;
using Windows.ApplicationModel.Activation;

using CalculatorApp.ViewModel.Common;

namespace CalculatorApp
{
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
            try
            {
                var rawbase64 = args.Uri.Segments.Skip(1).Aggregate((folded, x) => folded += x);
                var compressed = Convert.FromBase64String(rawbase64);
                var jsonStr = DeflateUtils.Decompress(compressed);
                return SnapshotLaunchArguments.FromJson(jsonStr);
            }
            catch (Exception ex)
            {
                TraceLogger.GetInstance().LogRecallError($"Error occurs during the deserialization of Snapshot. Exception: {ex}");
                return SnapshotLaunchArguments.Error();
            }
        }
    }
}
