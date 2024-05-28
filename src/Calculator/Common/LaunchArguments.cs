using System;

using Windows.ApplicationModel.Activation;
using Windows.ApplicationModel.UserActivities;

namespace CalculatorApp
{
    internal class SnapshotLaunchArguments
    {
        public string ActivityId { get; set; }
        public Uri LaunchUri { get; set; }
    }

    internal static class LaunchExtensions
    {
        public static bool IsSnapshotProtocol(this IActivatedEventArgs args) =>
            args is IProtocolActivatedEventArgs protoArgs &&
            protoArgs.Uri != null &&
            protoArgs.Uri.Segments != null &&
            protoArgs.Uri.Segments.Length == 2 &&
            protoArgs.Uri.Segments[0] == "snapshots/";

        /// <summary>
        /// GetActivityId() requires the parameter `launchUri` to be a well-formed
        /// snapshot URI.
        /// </summary>
        /// <param name="launchUri">the Uri to launch with a snapshot context.</param>
        /// <returns>Activity ID</returns>
        public static string GetActivityId(this Uri launchUri)
        {
            return launchUri.Segments[1].Trim();
        }

        public static bool VerifyIncomingActivity(this SnapshotLaunchArguments launchArgs, UserActivity activity)
        {
            if (activity.State != UserActivityState.Published ||
                string.IsNullOrEmpty(activity.ActivityId) ||
                activity.ActivationUri == null ||
                activity.ActivationUri.Segments == null ||
                activity.ActivationUri.Segments.Length != 2 ||
                activity.ActivationUri.Segments[0] != "snapshots/")
            {
                return false;
            }
            return activity.ActivityId == GetActivityId(launchArgs.LaunchUri);
        }
    }
}
