// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Text;
using Windows.UI.Xaml;

namespace CalculatorApp.Common.Automation
{
	public sealed partial class NarratorNotifier : DependencyObject
	{
		public static DependencyProperty AnnouncementProperty { get; } = DependencyProperty.RegisterAttached(
			"Announcement",
			typeof(NarratorAnnouncement),
			typeof(NarratorNotifier),
			new PropertyMetadata(default(NarratorAnnouncement), OnAnnouncementChanged));

		public static NarratorAnnouncement GetAnnouncement(Windows.UI.Xaml.DependencyObject element)
		{
			return element.GetValue(AnnouncementProperty) as NarratorAnnouncement;
		}

		public static void SetAnnouncement(Windows.UI.Xaml.DependencyObject element, NarratorAnnouncement value)
		{
			element.SetValue(AnnouncementProperty, value);
		}

		public NarratorAnnouncement Announcement
		{
			get { return GetAnnouncement(this); }
			set { SetAnnouncement(this, value); }
		}


		//TODO UNO INarratorAnnouncementHost m_announcementHost;

		public NarratorNotifier()
		{
			// TODO UNO
			// m_announcementHost = NarratorAnnouncementHostFactory.MakeHost();
		}

		public void Announce(NarratorAnnouncement announcement)
		{
			// TODO UNO:
			//if (NarratorAnnouncement.IsValid(announcement) && m_announcementHost != null)
			//{
			//	m_announcementHost.Announce(announcement);
			//}
		}

		static void OnAnnouncementChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs e)
		{
			var instance = dependencyObject as NarratorNotifier;
			if (instance != null)
			{
				instance.Announce((NarratorAnnouncement)(e.NewValue));
			}
		}
	}
}
