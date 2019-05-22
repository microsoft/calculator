using System;
using System.Linq;

namespace Calculator
{
	public static class StringExtensions
	{
		private const int _npos = -1;

		public static char back(this string str) => str.Last();
		public static char front(this string str) => str.First();
		public static int length(this string str) => str.Length;
		public static char at(this string str, int index) => str[index];
		public static bool empty(this string str) => string.IsNullOrEmpty(str);
		public static bool IsEmpty(this string str) => string.IsNullOrEmpty(str);
		public static int npos(this string str) => _npos;
		public static int size(this string str) => str.Length;
		public static int find(this string str, char c) => str.IndexOf(c);
		public static int find(this string str, char c, int startIndex) => str.IndexOf(c, startIndex);
		public static int find(this string str, string c) => str.IndexOf(c);
		public static int find(this string str, string c, int startIndex) => str.IndexOf(c, startIndex);

		public static string substr(this string str, int pos = 0, int len = _npos)
		{
			if (pos == str.Length)
			{
				return string.Empty;
			}

			if (pos > str.Length)
			{
				throw new ArgumentOutOfRangeException(nameof(pos));
			}

			if (len == _npos)
			{
				return str.Substring(pos);
			}
			else
			{
				return str.Substring(pos, Math.Min(len, str.Length - pos));
			}
		}
	}
}
