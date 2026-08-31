// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;

namespace CalculatorApp.ViewModel
{
    /// <summary>
    /// Ranks unit names, abbreviations, and categories using ordinal-ignore-case semantics.
    /// </summary>
    public sealed class FuzzyUnitSearchMatcher : IUnitSearchMatcher
    {
        // Bands keep within-strategy adjustments from changing strategy priority.
        private const int ExactScore = 1000;
        private const int PrefixScore = 900;
        private const int WordPrefixScore = 800;
        private const int ContainsScore = 700;
        private const int SubsequenceScore = 500;
        private const int TypoScore = 400;

        // Category matches rank below equivalent unit-field matches.
        private const int CategoryPenalty = 250;

        // Short queries skip loose matching to avoid noisy results.
        private const int MinSubsequenceQueryLength = 3;
        private const int MinTypoQueryLength = 4;

        private const int TwoEditQueryLength = 7;

        private static readonly char[] WordSeparators = { ' ', '-', '/', '(', ')', ',', '.' };
        // One-to-one lowercase mappings above U+FFFF.
        private static readonly (int Start, int End, int Delta)[] SupplementaryLowercaseRanges =
        {
            (0x10428, 0x1044F, -0x28),
            (0x104D8, 0x104FB, -0x28),
            (0x10597, 0x105A1, -0x27),
            (0x105A3, 0x105B1, -0x27),
            (0x105B3, 0x105B9, -0x27),
            (0x105BB, 0x105BC, -0x27),
            (0x10CC0, 0x10CF2, -0x40),
            (0x118C0, 0x118DF, -0x20),
            (0x16E60, 0x16E7F, -0x20),
            (0x1E922, 0x1E943, -0x22),
        };

        public int Rank(string query, UnitPickerItem item)
        {
            if (item?.Unit == null)
            {
                return 0;
            }

            if (string.IsNullOrEmpty(query))
            {
                return ExactScore;
            }

            int best = Math.Max(
                ScoreField(query, item.Unit.Name),
                ScoreField(query, item.Unit.Abbreviation));

            int categoryScore = ScoreField(query, item.CategoryName);
            if (categoryScore > 0)
            {
                best = Math.Max(best, categoryScore - CategoryPenalty);
            }

            return best;
        }

        private static int ScoreField(string query, string value)
        {
            if (string.IsNullOrEmpty(value))
            {
                return 0;
            }

            if (string.Equals(value, query, StringComparison.OrdinalIgnoreCase))
            {
                return ExactScore;
            }

            if (value.StartsWith(query, StringComparison.OrdinalIgnoreCase))
            {
                return PrefixScore;
            }

            int index = value.IndexOf(query, StringComparison.OrdinalIgnoreCase);
            if (index >= 0)
            {
                // Earlier and word-start matches rank higher within this band.
                int band = StartsWord(value, index) ? WordPrefixScore : ContainsScore;
                return band - Math.Min(index, 50);
            }

            string[] queryScalars = SplitScalars(query);
            string[] valueScalars = null;

            if (queryScalars.Length >= MinSubsequenceQueryLength)
            {
                valueScalars = SplitScalars(value);
                if (IsSubsequence(queryScalars, valueScalars))
                {
                    return SubsequenceScore;
                }
            }

            if (queryScalars.Length >= MinTypoQueryLength)
            {
                valueScalars = valueScalars ?? SplitScalars(value);
                int distance = BestEditDistance(queryScalars, valueScalars, value);
                if (distance >= 0)
                {
                    return TypoScore - (distance * 10);
                }
            }

            return 0;
        }

        private static bool StartsWord(string value, int index)
        {
            return index == 0 || Array.IndexOf(WordSeparators, value[index - 1]) >= 0;
        }

        private static bool IsSubsequence(string[] query, string[] value)
        {
            int queryIndex = 0;
            for (int i = 0; i < value.Length && queryIndex < query.Length; i++)
            {
                if (ScalarsEqualOrdinalIgnoreCase(value[i], query[queryIndex]))
                {
                    queryIndex++;
                }
            }

            return queryIndex == query.Length;
        }

        private static int BestEditDistance(string[] query, string[] value, string rawValue)
        {
            int allowed = query.Length >= TwoEditQueryLength ? 2 : 1;
            int best = BoundedEditDistance(query, value, allowed);

            // A misspelling in one word should still match a multi-word value.
            foreach (var word in rawValue.Split(WordSeparators, StringSplitOptions.RemoveEmptyEntries))
            {
                int distance = BoundedEditDistance(query, SplitScalars(word), allowed);
                if (distance >= 0 && (best < 0 || distance < best))
                {
                    best = distance;
                }
            }

            return best;
        }

        private static int BoundedEditDistance(string[] query, string[] value, int allowed)
        {
            if (Math.Abs(query.Length - value.Length) > allowed)
            {
                return -1;
            }

            var previous = new int[value.Length + 1];
            var current = new int[value.Length + 1];
            for (int j = 0; j <= value.Length; j++)
            {
                previous[j] = j;
            }

            for (int i = 1; i <= query.Length; i++)
            {
                current[0] = i;
                int rowBest = current[0];

                for (int j = 1; j <= value.Length; j++)
                {
                    int substitution =
                        ScalarsEqualOrdinalIgnoreCase(query[i - 1], value[j - 1]) ? 0 : 1;
                    current[j] = Math.Min(
                        Math.Min(current[j - 1] + 1, previous[j] + 1),
                        previous[j - 1] + substitution);

                    if (current[j] < rowBest)
                    {
                        rowBest = current[j];
                    }
                }

                // Row minima cannot recover once every candidate exceeds the bound.
                if (rowBest > allowed)
                {
                    return -1;
                }

                var swap = previous;
                previous = current;
                current = swap;
            }

            int result = previous[value.Length];
            return result <= allowed ? result : -1;
        }

        private static bool ScalarsEqualOrdinalIgnoreCase(string left, string right)
        {
            return string.Equals(left, right, StringComparison.OrdinalIgnoreCase)
                || (left.Length == 2
                    && right.Length == 2
                    && FoldSupplementaryCase(left) == FoldSupplementaryCase(right));
        }

        private static int FoldSupplementaryCase(string scalar)
        {
            int codePoint = char.ConvertToUtf32(scalar, 0);
            foreach (var range in SupplementaryLowercaseRanges)
            {
                if (codePoint >= range.Start && codePoint <= range.End)
                {
                    return codePoint + range.Delta;
                }
            }

            return codePoint;
        }

        private static string[] SplitScalars(string value)
        {
            var scalars = new List<string>(value.Length);
            for (int index = 0; index < value.Length;)
            {
                int length = char.IsHighSurrogate(value[index])
                    && index + 1 < value.Length
                    && char.IsLowSurrogate(value[index + 1])
                        ? 2
                        : 1;
                scalars.Add(value.Substring(index, length));
                index += length;
            }

            return scalars.ToArray();
        }
    }
}
