// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;
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
				var stringBuilder = new StringBuilder(pMessage);

				// This will capture all %<number> strings
				var matches = Regex
					.Matches(pMessage, @"%(\d+)")
					.AsEnumerable()
					.OrderBy(str => str.Value.Replace("%", ""));

				// If our starting index begin at 1, we will decrease all of them in order to use string.Format
				if (matches.FirstOrDefault()?.Value == "%1")
				{
					var addedCharacterFromOriginal = 0;

					foreach (var match in matches)
					{
						if(int.TryParse(match.Value.Replace("%", ""), out var argumentIndex))
						{
							stringBuilder.Remove(match.Index + addedCharacterFromOriginal, match.Length);
							stringBuilder.Insert(match.Index + addedCharacterFromOriginal, "{" + (argumentIndex - 1).ToString() + "}");
							// Since we are replace %<number> by {<number>}, we will add one character for each iteration
							// so we need to consider that
							addedCharacterFromOriginal++;
						}
					}
				}

				return $"{string.Format(stringBuilder.ToString(), args)}";
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
