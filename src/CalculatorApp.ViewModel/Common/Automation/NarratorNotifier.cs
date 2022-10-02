// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.ViewModelNative.Common.Automation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;

namespace CalculatorApp.ViewModel.Common.Automation
{
    public class NarratorNotifier : DependencyObject
    {
        public static readonly DependencyProperty AnnouncementProperty =
            DependencyProperty.Register(nameof(Announcement), typeof(NarratorAnnouncement), typeof(NarratorNotifier), new PropertyMetadata(null, (sender, args) =>
            {
                var self = (NarratorNotifier)sender;
                self.Announce((NarratorAnnouncement)args.NewValue);
            }));

        public NarratorAnnouncement Announcement
        {
            get => (NarratorAnnouncement)GetValue(AnnouncementProperty);
            set => SetValue(AnnouncementProperty, value);
        }

        public NarratorNotifier()
        {
        }

        public void Announce(NarratorAnnouncement announcement)
        {
            if (NarratorAnnouncement.IsValid(announcement))
            {
                if (this.announcementElement == null)
                {
                    this.announcementElement = new TextBlock();
                }

                var peer = FrameworkElementAutomationPeer.FromElement(this.announcementElement);
                if (peer != null)
                {
                    peer.RaiseNotificationEvent(announcement.Kind, announcement.Processing, announcement.Announcement, announcement.ActivityId);
                }
            }
        }

        private UIElement announcementElement;
    }
}
