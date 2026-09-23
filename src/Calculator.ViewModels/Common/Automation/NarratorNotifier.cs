// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;

namespace CalculatorApp.ViewModel.Common.Automation
{
    public sealed class NarratorNotifier : DependencyObject
    {
        private UIElement _announcementElement;

        private static DependencyProperty s_announcementProperty;

        public NarratorNotifier()
        {
        }

        public NarratorAnnouncement Announcement
        {
            get => GetAnnouncement(this);
            set => SetAnnouncement(this, value);
        }

        public static DependencyProperty AnnouncementProperty => s_announcementProperty;

        public static NarratorAnnouncement GetAnnouncement(DependencyObject element)
        {
            return (NarratorAnnouncement)element.GetValue(s_announcementProperty);
        }

        public static void SetAnnouncement(DependencyObject element, NarratorAnnouncement value)
        {
            element.SetValue(s_announcementProperty, value);
        }

        public static void RegisterDependencyProperties()
        {
            s_announcementProperty = DependencyProperty.Register(
                "Announcement",
                typeof(NarratorAnnouncement),
                typeof(NarratorNotifier),
                new PropertyMetadata(null, OnAnnouncementChanged));
        }

        public void Announce(NarratorAnnouncement announcement)
        {
            if (NarratorAnnouncement.IsValid(announcement))
            {
                if (_announcementElement == null)
                {
                    _announcementElement = new TextBlock();
                }

                var peer = FrameworkElementAutomationPeer.FromElement(_announcementElement);
                if (peer != null)
                {
                    peer.RaiseNotificationEvent(
                        announcement.Kind,
                        announcement.Processing,
                        announcement.Announcement,
                        announcement.ActivityId);
                }
            }
        }

        private static void OnAnnouncementChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs e)
        {
            if (dependencyObject is NarratorNotifier instance)
            {
                instance.Announce(e.NewValue as NarratorAnnouncement);
            }
        }
    }
}
