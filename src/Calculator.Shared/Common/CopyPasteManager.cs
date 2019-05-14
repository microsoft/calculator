using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using CalculatorApp.Common;

namespace CalculatorApp
{
    public class CopyPasteManager
    {
        public const string PasteErrorString = "NoOp";

        public const int QwordType = 1;
        public const int DwordType = 2;
        public const int WordType = 3;
        public const int ByteType = 4;
        public const int HexBase = 5;
        public const int DecBase = 6;
        public const int OctBase = 7;
        public const int BinBase = 8;

        internal static bool HasStringToPaste() => throw new NotImplementedException();
        internal static Task<string> GetStringToPaste(ViewMode mode, CategoryGroupType categoryGroupType, int numberBase, int bitLengthType) => throw new NotImplementedException();
        internal static void CopyToClipboard(string v) => throw new NotImplementedException();
    }
}
