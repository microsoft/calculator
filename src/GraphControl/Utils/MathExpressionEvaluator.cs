// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Text.RegularExpressions;
using System.Xml.Linq;

namespace GraphControl
{
    public static class MathExpressionEvaluator
    {
        public static string ExtractExpressionFromMathML(string input)
        {
            if (string.IsNullOrWhiteSpace(input))
            {
                return string.Empty;
            }

            input = input.Trim();
            if (!input.StartsWith("<", StringComparison.Ordinal))
            {
                return input;
            }

            try
            {
                var doc = XDocument.Parse(input);
                return ExtractFromXml(doc.Root);
            }
            catch
            {
                // Regex fallback for stripping MathML tags
                string cleaned = Regex.Replace(input, @"<[^>]+>", " ");
                cleaned = Regex.Replace(cleaned, @"\s+", " ").Trim();
                return cleaned;
            }
        }

        private static string ExtractFromXml(XElement element)
        {
            if (element == null) return string.Empty;

            string name = element.Name.LocalName.ToLowerInvariant();
            if (name == "mfrac")
            {
                var children = new List<XElement>(element.Elements());
                if (children.Count == 2)
                {
                    return $"({ExtractFromXml(children[0])})/({ExtractFromXml(children[1])})";
                }
            }
            else if (name == "msup")
            {
                var children = new List<XElement>(element.Elements());
                if (children.Count == 2)
                {
                    return $"({ExtractFromXml(children[0])})^({ExtractFromXml(children[1])})";
                }
            }
            else if (name == "msqrt")
            {
                var children = new List<XElement>(element.Elements());
                var inner = string.Join("", children.ConvertAll(ExtractFromXml));
                return $"sqrt({inner})";
            }
            else if (name == "mroot")
            {
                var children = new List<XElement>(element.Elements());
                if (children.Count == 2)
                {
                    return $"({ExtractFromXml(children[0])})^(1/({ExtractFromXml(children[1])}))";
                }
            }

            if (!element.HasElements)
            {
                return element.Value;
            }

            var sb = new System.Text.StringBuilder();
            foreach (var child in element.Elements())
            {
                sb.Append(ExtractFromXml(child));
            }
            return sb.ToString();
        }

        public static Func<double, double> Compile(string rawExpr, IDictionary<string, double> variables = null)
        {
            if (string.IsNullOrWhiteSpace(rawExpr))
            {
                return null;
            }

            string expr = ExtractExpressionFromMathML(rawExpr);

            // Handle "y = ...", "f(x) = ...", "y(x) = ..."
            int eqIdx = expr.IndexOf('=');
            if (eqIdx >= 0)
            {
                string left = expr.Substring(0, eqIdx).Trim().ToLowerInvariant();
                string right = expr.Substring(eqIdx + 1).Trim();
                if (left == "y" || left == "f(x)" || left == "g(x)" || left == "h(x)" || left == "y(x)")
                {
                    expr = right;
                }
                else
                {
                    // e.g. "x + y = 2" -> "2 - (x)"
                    expr = $"({right}) - ({left.Replace("y", "0")})";
                }
            }

            expr = NormalizeExpression(expr);

            try
            {
                var tokens = Tokenize(expr);
                var rpn = ShuntingYard(tokens);
                return x => EvaluateRpn(rpn, x, variables);
            }
            catch
            {
                return null;
            }
        }

        private static string NormalizeExpression(string s)
        {
            s = s.Replace("×", "*").Replace("÷", "/").Replace("−", "-").Replace("π", "pi");
            
            // Insert implicit multiplication: e.g. 2x -> 2*x, 2sin -> 2*sin, x(x) -> x*(x), )( -> )*(
            s = Regex.Replace(s, @"(\d)([a-zA-Z\(])", "$1*$2");
            s = Regex.Replace(s, @"([a-zA-Z\)])(\d)", "$1*$2");
            s = Regex.Replace(s, @"(\))(\()", "$1*$2");
            s = Regex.Replace(s, @"(\))([a-zA-Z])", "$1*$2");
            s = Regex.Replace(s, @"(x)([a-zA-Z])", "$1*$2"); // x sin(x) -> x*sin(x)

            return s;
        }

        private enum TokenType
        {
            Number,
            Variable,
            Identifier,
            Operator,
            LeftParen,
            RightParen,
            Comma
        }

        private class Token
        {
            public TokenType Type;
            public string Text;
            public double NumberValue;
        }

        private static List<Token> Tokenize(string expr)
        {
            var tokens = new List<Token>();
            int i = 0;

            while (i < expr.Length)
            {
                char c = expr[i];

                if (char.IsWhiteSpace(c))
                {
                    i++;
                    continue;
                }

                if (char.IsDigit(c) || (c == '.' && i + 1 < expr.Length && char.IsDigit(expr[i + 1])))
                {
                    int start = i;
                    bool hasDot = false;
                    while (i < expr.Length && (char.IsDigit(expr[i]) || (!hasDot && expr[i] == '.')))
                    {
                        if (expr[i] == '.') hasDot = true;
                        i++;
                    }
                    string numStr = expr.Substring(start, i - start);
                    double val = double.Parse(numStr, CultureInfo.InvariantCulture);
                    tokens.Add(new Token { Type = TokenType.Number, Text = numStr, NumberValue = val });
                    continue;
                }

                if (char.IsLetter(c))
                {
                    int start = i;
                    while (i < expr.Length && (char.IsLetterOrDigit(expr[i]) || expr[i] == '_'))
                    {
                        i++;
                    }
                    string id = expr.Substring(start, i - start).ToLowerInvariant();
                    if (id == "x")
                    {
                        tokens.Add(new Token { Type = TokenType.Variable, Text = "x" });
                    }
                    else if (id == "pi")
                    {
                        tokens.Add(new Token { Type = TokenType.Number, Text = "pi", NumberValue = Math.PI });
                    }
                    else if (id == "e" && (tokens.Count == 0 || tokens[tokens.Count - 1].Type == TokenType.Operator || tokens[tokens.Count - 1].Type == TokenType.LeftParen))
                    {
                        tokens.Add(new Token { Type = TokenType.Number, Text = "e", NumberValue = Math.E });
                    }
                    else
                    {
                        tokens.Add(new Token { Type = TokenType.Identifier, Text = id });
                    }
                    continue;
                }

                if (c == '(')
                {
                    tokens.Add(new Token { Type = TokenType.LeftParen, Text = "(" });
                    i++;
                    continue;
                }

                if (c == ')')
                {
                    tokens.Add(new Token { Type = TokenType.RightParen, Text = ")" });
                    i++;
                    continue;
                }

                if (c == ',')
                {
                    tokens.Add(new Token { Type = TokenType.Comma, Text = "," });
                    i++;
                    continue;
                }

                if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '%')
                {
                    // Check for unary minus
                    if (c == '-' && (tokens.Count == 0 || tokens[tokens.Count - 1].Type == TokenType.Operator || tokens[tokens.Count - 1].Type == TokenType.LeftParen))
                    {
                        tokens.Add(new Token { Type = TokenType.Operator, Text = "u-" });
                    }
                    else if (c == '+' && (tokens.Count == 0 || tokens[tokens.Count - 1].Type == TokenType.Operator || tokens[tokens.Count - 1].Type == TokenType.LeftParen))
                    {
                        // Unary plus, ignore
                    }
                    else
                    {
                        tokens.Add(new Token { Type = TokenType.Operator, Text = c.ToString() });
                    }
                    i++;
                    continue;
                }

                i++;
            }

            return tokens;
        }

        private static int GetPrecedence(string op)
        {
            switch (op)
            {
                case "u-": return 4;
                case "^": return 3;
                case "*":
                case "/":
                case "%": return 2;
                case "+":
                case "-": return 1;
                default: return 0;
            }
        }

        private static bool IsRightAssociative(string op)
        {
            return op == "^" || op == "u-";
        }

        private static List<Token> ShuntingYard(List<Token> tokens)
        {
            var output = new List<Token>();
            var stack = new Stack<Token>();

            foreach (var token in tokens)
            {
                switch (token.Type)
                {
                    case TokenType.Number:
                    case TokenType.Variable:
                        output.Add(token);
                        break;

                    case TokenType.Identifier:
                        stack.Push(token);
                        break;

                    case TokenType.Comma:
                        while (stack.Count > 0 && stack.Peek().Type != TokenType.LeftParen)
                        {
                            output.Add(stack.Pop());
                        }
                        break;

                    case TokenType.Operator:
                        while (stack.Count > 0 && stack.Peek().Type == TokenType.Operator)
                        {
                            string topOp = stack.Peek().Text;
                            int topPrec = GetPrecedence(topOp);
                            int currPrec = GetPrecedence(token.Text);

                            if ((!IsRightAssociative(token.Text) && currPrec <= topPrec) ||
                                (IsRightAssociative(token.Text) && currPrec < topPrec))
                            {
                                output.Add(stack.Pop());
                            }
                            else
                            {
                                break;
                            }
                        }
                        stack.Push(token);
                        break;

                    case TokenType.LeftParen:
                        stack.Push(token);
                        break;

                    case TokenType.RightParen:
                        while (stack.Count > 0 && stack.Peek().Type != TokenType.LeftParen)
                        {
                            output.Add(stack.Pop());
                        }
                        if (stack.Count > 0 && stack.Peek().Type == TokenType.LeftParen)
                        {
                            stack.Pop(); // Pop '('
                        }
                        if (stack.Count > 0 && stack.Peek().Type == TokenType.Identifier)
                        {
                            output.Add(stack.Pop()); // Function call
                        }
                        break;
                }
            }

            while (stack.Count > 0)
            {
                var top = stack.Pop();
                if (top.Type != TokenType.LeftParen && top.Type != TokenType.RightParen)
                {
                    output.Add(top);
                }
            }

            return output;
        }

        private static double EvaluateRpn(List<Token> rpn, double x, IDictionary<string, double> variables)
        {
            var evalStack = new Stack<double>();

            foreach (var token in rpn)
            {
                if (token.Type == TokenType.Number)
                {
                    evalStack.Push(token.NumberValue);
                }
                else if (token.Type == TokenType.Variable)
                {
                    evalStack.Push(x);
                }
                else if (token.Type == TokenType.Operator)
                {
                    if (token.Text == "u-")
                    {
                        double a = evalStack.Count > 0 ? evalStack.Pop() : 0;
                        evalStack.Push(-a);
                    }
                    else
                    {
                        double b = evalStack.Count > 0 ? evalStack.Pop() : 0;
                        double a = evalStack.Count > 0 ? evalStack.Pop() : 0;
                        switch (token.Text)
                        {
                            case "+": evalStack.Push(a + b); break;
                            case "-": evalStack.Push(a - b); break;
                            case "*": evalStack.Push(a * b); break;
                            case "/": evalStack.Push(Math.Abs(b) > 1e-12 ? a / b : double.NaN); break;
                            case "%": evalStack.Push(a % b); break;
                            case "^": evalStack.Push(Math.Pow(a, b)); break;
                        }
                    }
                }
                else if (token.Type == TokenType.Identifier)
                {
                    string func = token.Text.ToLowerInvariant();
                    if (variables != null && variables.TryGetValue(func, out double varVal))
                    {
                        evalStack.Push(varVal);
                    }
                    else
                    {
                        double a = evalStack.Count > 0 ? evalStack.Pop() : 0;
                        evalStack.Push(ApplyFunction(func, a));
                    }
                }
            }

            return evalStack.Count > 0 ? evalStack.Pop() : double.NaN;
        }

        private static double ApplyFunction(string func, double a)
        {
            switch (func)
            {
                case "sin": return Math.Sin(a);
                case "cos": return Math.Cos(a);
                case "tan": return Math.Tan(a);
                case "cot": return 1.0 / Math.Tan(a);
                case "sec": return 1.0 / Math.Cos(a);
                case "csc": return 1.0 / Math.Sin(a);
                case "asin":
                case "arcsin": return Math.Asin(a);
                case "acos":
                case "arccos": return Math.Acos(a);
                case "atan":
                case "arctan": return Math.Atan(a);
                case "sinh": return Math.Sinh(a);
                case "cosh": return Math.Cosh(a);
                case "tanh": return Math.Tanh(a);
                case "sqrt": return a >= 0 ? Math.Sqrt(a) : double.NaN;
                case "cbrt": return Math.Sign(a) * Math.Pow(Math.Abs(a), 1.0 / 3.0);
                case "abs": return Math.Abs(a);
                case "exp": return Math.Exp(a);
                case "ln": return a > 0 ? Math.Log(a) : double.NaN;
                case "log":
                case "log10": return a > 0 ? Math.Log10(a) : double.NaN;
                case "log2": return a > 0 ? Math.Log(a, 2.0) : double.NaN;
                case "floor": return Math.Floor(a);
                case "ceil":
                case "ceiling": return Math.Ceiling(a);
                case "round": return Math.Round(a);
                case "sign": return Math.Sign(a);
                default: return a;
            }
        }

        public static KeyGraphFeaturesInfo AnalyzeFunction(string rawExpr, IDictionary<string, double> variables = null)
        {
            if (string.IsNullOrWhiteSpace(rawExpr))
            {
                return KeyGraphFeaturesInfo.Create(CalculatorApp.AnalysisErrorType.AnalysisCouldNotBePerformed);
            }

            var f = Compile(rawExpr, variables);
            if (f == null)
            {
                return KeyGraphFeaturesInfo.Create(CalculatorApp.AnalysisErrorType.AnalysisCouldNotBePerformed);
            }

            var info = new KeyGraphFeaturesInfo();

            try
            {
                // 1. Y-Intercept: f(0)
                double y0 = f(0);
                if (!double.IsNaN(y0) && !double.IsInfinity(y0))
                {
                    info.YIntercept = $"(0, {FormatNumber(y0)})";
                }

                // 2. X-Intercepts (Roots / Zeros)
                var roots = new List<double>();
                double step = 0.25;
                for (double x = -50.0; x <= 50.0; x += step)
                {
                    double y1 = f(x);
                    double y2 = f(x + step);

                    if (!double.IsNaN(y1) && !double.IsNaN(y2) && !double.IsInfinity(y1) && !double.IsInfinity(y2))
                    {
                        if (Math.Abs(y1) < 1e-6)
                        {
                            AddUniqueRoot(roots, x);
                        }
                        else if (y1 * y2 < 0)
                        {
                            // Bisection method for high precision
                            double a = x, b = x + step;
                            for (int iter = 0; iter < 24; iter++)
                            {
                                double mid = (a + b) / 2.0;
                                double yMid = f(mid);
                                if (Math.Abs(yMid) < 1e-9)
                                {
                                    a = mid;
                                    break;
                                }
                                if (y1 * yMid < 0) b = mid;
                                else { a = mid; y1 = yMid; }
                            }
                            AddUniqueRoot(roots, (a + b) / 2.0);
                        }
                    }
                }

                if (roots.Count > 0)
                {
                    var rootStrings = roots.ConvertAll(r => $"({FormatNumber(r)}, 0)");
                    info.XIntercept = string.Join(", ", rootStrings);
                }

                // 3. Extrema (Local Minima and Maxima)
                double h = 1e-4;
                double prevDeriv = double.NaN;
                double prevX = double.NaN;

                for (double x = -30.0; x <= 30.0; x += 0.1)
                {
                    double fPlus = f(x + h);
                    double fMinus = f(x - h);
                    if (!double.IsNaN(fPlus) && !double.IsNaN(fMinus) && !double.IsInfinity(fPlus) && !double.IsInfinity(fMinus))
                    {
                        double deriv = (fPlus - fMinus) / (2.0 * h);
                        if (!double.IsNaN(prevDeriv))
                        {
                            if (prevDeriv < 0 && deriv > 0)
                            {
                                // Local minimum
                                double critX = (prevX + x) / 2.0;
                                double critY = f(critX);
                                if (!double.IsNaN(critY))
                                {
                                    info.Minima.Add($"({FormatNumber(critX)}, {FormatNumber(critY)})");
                                }
                            }
                            else if (prevDeriv > 0 && deriv < 0)
                            {
                                // Local maximum
                                double critX = (prevX + x) / 2.0;
                                double critY = f(critX);
                                if (!double.IsNaN(critY))
                                {
                                    info.Maxima.Add($"({FormatNumber(critX)}, {FormatNumber(critY)})");
                                }
                            }
                        }
                        prevDeriv = deriv;
                        prevX = x;
                    }
                    else
                    {
                        prevDeriv = double.NaN;
                    }
                }

                // 4. Parity
                bool isEven = true;
                bool isOdd = true;
                double[] testPoints = { 0.5, 1.0, 2.0, Math.PI, 4.2 };
                foreach (var tp in testPoints)
                {
                    double pos = f(tp);
                    double neg = f(-tp);
                    if (double.IsNaN(pos) || double.IsNaN(neg) || double.IsInfinity(pos) || double.IsInfinity(neg))
                    {
                        isEven = false;
                        isOdd = false;
                        break;
                    }
                    if (Math.Abs(pos - neg) > 1e-4) isEven = false;
                    if (Math.Abs(pos + neg) > 1e-4) isOdd = false;
                }

                if (isEven && isOdd) info.Parity = 1; // Even (or zero function)
                else if (isEven) info.Parity = 1; // Even
                else if (isOdd) info.Parity = 2; // Odd
                else info.Parity = 3; // None

                // 5. Domain & Range
                string cleanExpr = ExtractExpressionFromMathML(rawExpr).ToLowerInvariant();
                if (cleanExpr.Contains("sqrt(") || cleanExpr.Contains("ln(") || cleanExpr.Contains("log("))
                {
                    info.Domain = "[0, ∞)";
                }
                else
                {
                    info.Domain = "(-∞, ∞)";
                }

                if (info.Minima.Count > 0 && info.Maxima.Count == 0)
                {
                    double minY = info.Minima.Count > 0 ? f(0) : 0;
                    info.Range = $"[{FormatNumber(minY)}, ∞)";
                }
                else if (info.Maxima.Count > 0 && info.Minima.Count == 0)
                {
                    double maxY = info.Maxima.Count > 0 ? f(0) : 0;
                    info.Range = $"(-∞, {FormatNumber(maxY)}]";
                }
                else
                {
                    info.Range = "(-∞, ∞)";
                }

                info.AnalysisError = (int)CalculatorApp.AnalysisErrorType.NoError;
            }
            catch
            {
                info.AnalysisError = (int)CalculatorApp.AnalysisErrorType.AnalysisCouldNotBePerformed;
            }

            return info;
        }

        private static void AddUniqueRoot(List<double> roots, double r)
        {
            r = Math.Round(r, 6);
            if (Math.Abs(r) < 1e-6) r = 0;
            if (!roots.Exists(x => Math.Abs(x - r) < 1e-3))
            {
                roots.Add(r);
            }
        }

        private static string FormatNumber(double val)
        {
            if (Math.Abs(val) < 1e-6) return "0";
            return Math.Round(val, 4).ToString(CultureInfo.InvariantCulture);
        }
    }
}
