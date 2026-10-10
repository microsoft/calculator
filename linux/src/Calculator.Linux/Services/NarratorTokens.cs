// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Narrator-readable token names, ported from LocalizationSettings. Maps engine
// token strings (e.g. "sin(") to accessible resource names for screen readers.
// Resource lookup goes through AppResources (generated en-US tables).
namespace Calculator.Linux.Services;

public static class NarratorTokens
{
    private static Dictionary<string, string>? s_map;
    private static string s_openParen = "(";
    private static readonly object s_lock = new();

    public static string GetReadableToken(string token)
    {
        EnsureMap();
        if (s_map!.TryGetValue(token, out string? readable))
        {
            return readable + " " + s_openParen;
        }
        return token;
    }

    public static string GetReadableString(string value)
    {
        var sb = new System.Text.StringBuilder();
        foreach (char c in value)
        {
            sb.Append(GetReadableToken(c.ToString()));
        }
        return sb.ToString();
    }

    private static void EnsureMap()
    {
        if (s_map != null)
        {
            return;
        }
        lock (s_lock)
        {
            if (s_map != null)
            {
                return;
            }
            try
            {
                var map = new Dictionary<string, string>();
                string openParen = AppResources.GetCEngineString("48"); // "("
                AddParen(map, openParen, "67", "SineDegrees");
                AddParen(map, openParen, "73", "SineRadians");
                AddParen(map, openParen, "79", "SineGradians");
                AddParen(map, openParen, "70", "InverseSineDegrees");
                AddParen(map, openParen, "76", "InverseSineRadians");
                AddParen(map, openParen, "82", "InverseSineGradians");
                AddParen(map, openParen, "25", "HyperbolicSine");
                AddParen(map, openParen, "85", "InverseHyperbolicSine");
                AddParen(map, openParen, "68", "CosineDegrees");
                AddParen(map, openParen, "74", "CosineRadians");
                AddParen(map, openParen, "80", "CosineGradians");
                AddParen(map, openParen, "71", "InverseCosineDegrees");
                AddParen(map, openParen, "77", "InverseCosineRadians");
                AddParen(map, openParen, "83", "InverseCosineGradians");
                AddParen(map, openParen, "26", "HyperbolicCosine");
                AddParen(map, openParen, "86", "InverseHyperbolicCosine");
                AddParen(map, openParen, "69", "TangentDegrees");
                AddParen(map, openParen, "75", "TangentRadians");
                AddParen(map, openParen, "81", "TangentGradians");
                AddParen(map, openParen, "72", "InverseTangentDegrees");
                AddParen(map, openParen, "78", "InverseTangentRadians");
                AddParen(map, openParen, "84", "InverseTangentGradians");
                AddParen(map, openParen, "27", "HyperbolicTangent");
                AddParen(map, openParen, "87", "InverseHyperbolicTangent");
                AddParen(map, openParen, "SecDeg", "SecantDegrees");
                AddParen(map, openParen, "SecRad", "SecantRadians");
                AddParen(map, openParen, "SecGrad", "SecantGradians");
                AddParen(map, openParen, "InverseSecDeg", "InverseSecantDegrees");
                AddParen(map, openParen, "InverseSecRad", "InverseSecantRadians");
                AddParen(map, openParen, "InverseSecGrad", "InverseSecantGradians");
                AddParen(map, openParen, "Sech", "HyperbolicSecant");
                AddParen(map, openParen, "InverseSech", "InverseHyperbolicSecant");
                AddParen(map, openParen, "CscDeg", "CosecantDegrees");
                AddParen(map, openParen, "CscRad", "CosecantRadians");
                AddParen(map, openParen, "CscGrad", "CosecantGradians");
                AddParen(map, openParen, "InverseCscDeg", "InverseCosecantDegrees");
                AddParen(map, openParen, "InverseCscRad", "InverseCosecantRadians");
                AddParen(map, openParen, "InverseCscGrad", "InverseCosecantGradians");
                AddParen(map, openParen, "Csch", "HyperbolicCosecant");
                AddParen(map, openParen, "InverseCsch", "InverseHyperbolicCosecant");
                AddParen(map, openParen, "CotDeg", "CotangentDegrees");
                AddParen(map, openParen, "CotRad", "CotangentRadians");
                AddParen(map, openParen, "CotGrad", "CotangentGradians");
                AddParen(map, openParen, "InverseCotDeg", "InverseCotangentDegrees");
                AddParen(map, openParen, "InverseCotRad", "InverseCotangentRadians");
                AddParen(map, openParen, "InverseCotGrad", "InverseCotangentGradians");
                AddParen(map, openParen, "Coth", "HyperbolicCotangent");
                AddParen(map, openParen, "InverseCoth", "InverseHyperbolicCotangent");
                AddParen(map, openParen, "94", "Factorial");
                AddParen(map, openParen, "35", "DegreeMinuteSecond");
                AddParen(map, openParen, "28", "NaturalLog");
                AddParen(map, openParen, "91", "Square");
                AddParen(map, openParen, "92", "Cube");
                AddParen(map, openParen, "23", "TenPowerX");
                AddParen(map, openParen, "97", "LogBase2");
                AddParen(map, openParen, "98", "AbsoluteValue");
                AddParen(map, openParen, "24", "PowerOfE");
                AddParen(map, openParen, "29", "CommonLog");
                AddParen(map, openParen, "93", "CubeRoot");
                AddPlain(map, "9", "LeftShift");
                AddPlain(map, "10", "RightShift");
                AddPlain(map, "LogBaseY", "Logy");
                AddPlain(map, "16", "YRoot");

                string minusText = AppResources.GetString("minus");
                if (!string.IsNullOrEmpty(minusText))
                {
                    map["-"] = minusText;
                }

                s_openParen = AppResources.GetCEngineString("48");
                s_map = map;
            }
            catch
            {
                s_openParen = "(";
                s_map = new Dictionary<string, string>();
            }
        }
    }

    private static void AddParen(Dictionary<string, string> map, string openParen, string engineKey, string resourceName)
    {
        string engineStr = AppResources.GetCEngineString(engineKey);
        string automationName = AppResources.GetString(resourceName);
        if (!string.IsNullOrEmpty(engineStr) && !string.IsNullOrEmpty(automationName) && automationName != resourceName)
        {
            map[engineStr + openParen] = automationName;
        }
    }

    private static void AddPlain(Dictionary<string, string> map, string engineKey, string resourceName)
    {
        string engineStr = AppResources.GetCEngineString(engineKey);
        string automationName = AppResources.GetString(resourceName);
        if (!string.IsNullOrEmpty(engineStr) && !string.IsNullOrEmpty(automationName) && automationName != resourceName)
        {
            map[engineStr] = automationName;
        }
    }
}
