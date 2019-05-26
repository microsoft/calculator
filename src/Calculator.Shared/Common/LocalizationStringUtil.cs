// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace CalculatorApp
{
	namespace Common
	{
		public static class LocalizationStringUtil
		{
			public static string GetLocalizedString(string pMessage, params object[] args)
			{
				if (pMessage == null)
				{
					return string.Empty;
				}

				var pattern = Regex.Replace(pMessage, @"%(?<number>\d+)", m => "{" + (int.Parse(m.Groups["number"].Value) - 1) + "}");
				return string.Format(CultureInfo.CurrentCulture, pattern, args);
			}

			public static string GetLocalizedNarratorAnnouncement(string resourceKey, string formatVariable, params object[] args)
			{
				EnsureInitialization(resourceKey, formatVariable);
				return GetLocalizedString(formatVariable, args);
			}

			static void EnsureInitialization(string resourceKey, string formatVariable)
			{
				if (resourceKey == null || string.IsNullOrEmpty(resourceKey))
				{
					return;
				}

				// If the formatVariable already has a value, we don't need to set it again. Simply return.
				if (formatVariable != null && !string.IsNullOrEmpty(formatVariable))
				{
					return;
				}

				formatVariable = AppResourceProvider.GetInstance().GetResourceString(resourceKey);
			}

			static IEnumerable<Match> AsEnumerable(this MatchCollection source)
			{
				var enumerator = source.GetEnumerator();
				var returnedEnum = new List<Match>();

				while (enumerator.MoveNext())
				{
					returnedEnum.Add((Match)enumerator.Current);
				}

				return returnedEnum;
			}
		}
	}
}
