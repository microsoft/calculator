using System;
using System.Linq;

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
            protoArgs.Uri.AbsolutePath == "/snapshot" &&
            !string.IsNullOrEmpty(protoArgs.Uri.Query);

        /// <summary>
        /// GetActivityId() requires the parameter `launchUri` to be a well-formed
        /// snapshot URI.
        /// </summary>
        /// <param name="launchUri"></param>
        /// <returns></returns>
        public static string GetActivityId(this Uri launchUri)
        {
            const string ActivityIdKey = "activityId=";
            var segment = launchUri.Query.Split('?', '&').FirstOrDefault(x => x.StartsWith(ActivityIdKey));
            if (segment != null)
            {
                segment = segment.Trim();
                return segment.Length > ActivityIdKey.Length ?
                    segment.Substring(ActivityIdKey.Length) :
                    string.Empty;
            }
            return string.Empty;
        }

        public static bool VerifyIncomingActivity(this SnapshotLaunchArguments launchArgs, UserActivity activity)
        {
            if (string.IsNullOrEmpty(activity.ActivityId) ||
                activity.ActivationUri == null ||
                activity.ActivationUri.AbsolutePath != "/snapshot" ||
                string.IsNullOrEmpty(activity.ActivationUri.Query) ||
                activity.ContentInfo == null)
            {
                return false;
            }
            return activity.ActivityId == GetActivityId(launchArgs.LaunchUri);
        }
    }
}
