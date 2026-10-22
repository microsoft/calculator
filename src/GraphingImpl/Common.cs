// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace Graphing
{
    public interface IExpression
    {
        uint GetExpressionID();
        bool IsEmptySet();
    }

    public interface IVariable
    {
        int GetVariableID();
        string GetVariableName();
    }

    public interface IExpressible
    {
        IExpression GetExpression();
    }

    public struct Color : IEquatable<Color>
    {
        private const int RedChannelShift = 24;
        private const int GreenChannelShift = 16;
        private const int BlueChannelShift = 8;
        private const int AlphaChannelShift = 0;

        public byte R { get; set; }
        public byte G { get; set; }
        public byte B { get; set; }
        public byte A { get; set; }

        public Color(byte r, byte g, byte b, byte a = 0xFF)
        {
            R = r;
            G = g;
            B = b;
            A = a;
        }

        public Color(uint value)
        {
            R = (byte)(value >> RedChannelShift);
            G = (byte)(value >> GreenChannelShift);
            B = (byte)(value >> BlueChannelShift);
            A = (byte)(value >> AlphaChannelShift);
        }

        public uint ToUInt32()
        {
            return ((uint)A << AlphaChannelShift)
                 | ((uint)R << RedChannelShift)
                 | ((uint)G << GreenChannelShift)
                 | ((uint)B << BlueChannelShift);
        }

        public bool Equals(Color other)
        {
            return R == other.R && G == other.G && B == other.B && A == other.A;
        }

        public override bool Equals(object obj)
        {
            return obj is Color other && Equals(other);
        }

        public override int GetHashCode()
        {
            return (int)ToUInt32();
        }

        public static bool operator ==(Color left, Color right) => left.Equals(right);
        public static bool operator !=(Color left, Color right) => !left.Equals(right);
    }
}
