// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Graphing expression parser + evaluator (Task 8). Replaces the proprietary
// Microsoft math solver for rendering: equations are parsed once into an AST
// and sampled over the viewport. Plotting convention: non-finite samples
// (NaN/Infinity from e.g. division by zero) are skipped, not errors.
namespace Calculator.Linux.Services.Graphing;

public enum AngleMode
{
    Radians,
    Degrees,
}

public sealed class GraphExpression
{
    private readonly Node? _root;
    private readonly string? _error;

    private GraphExpression(Node? root, string? error)
    {
        _root = root;
        _error = error;
    }

    public bool IsValid => _root != null;
    public string Error => _error ?? string.Empty;

    public static GraphExpression Parse(string text) => new Parser(text).Parse();

    public double Evaluate(double x, IDictionary<string, double>? parameters, AngleMode mode)
    {
        if (_root == null)
        {
            throw new InvalidOperationException("Cannot evaluate an invalid expression: " + Error);
        }
        bool degrees = mode == AngleMode.Degrees;
        double result = _root.Eval(x, parameters, degrees);
        if (double.IsNaN(result) || double.IsInfinity(result))
        {
            return double.NaN;
        }
        return result;
    }

    private abstract class Node
    {
        public abstract double Eval(double x, IDictionary<string, double>? parameters, bool degrees);
    }

    private sealed class ConstNode(double value) : Node
    {
        public override double Eval(double x, IDictionary<string, double>? parameters, bool degrees) => value;
    }

    private sealed class VarNode : Node
    {
        public override double Eval(double x, IDictionary<string, double>? parameters, bool degrees) => x;
    }

    private sealed class ParamNode(string name) : Node
    {
        public override double Eval(double x, IDictionary<string, double>? parameters, bool degrees) =>
            parameters != null && parameters.TryGetValue(name, out double value) ? value : double.NaN;
    }

    private sealed class NegateNode(Node inner) : Node
    {
        public override double Eval(double x, IDictionary<string, double>? parameters, bool degrees) =>
            -inner.Eval(x, parameters, degrees);
    }

    private sealed class BinaryNode(char op, Node left, Node right) : Node
    {
        public override double Eval(double x, IDictionary<string, double>? parameters, bool degrees)
        {
            double a = left.Eval(x, parameters, degrees);
            double b = right.Eval(x, parameters, degrees);
            return op switch
            {
                '+' => a + b,
                '-' => a - b,
                '*' => a * b,
                '/' => b == 0 ? double.NaN : a / b,
                '%' => b == 0 ? double.NaN : a % b,
                '^' => Math.Pow(a, b),
                _ => double.NaN,
            };
        }
    }

    private sealed class FuncNode(string name, Node arg) : Node
    {
        public override double Eval(double x, IDictionary<string, double>? parameters, bool degrees)
        {
            double a = arg.Eval(x, parameters, degrees);
            return Apply(name, a, degrees);
        }

        private static double Apply(string name, double a, bool degrees)
        {
            double toRad = degrees ? Math.PI / 180.0 : 1.0;
            double fromRad = degrees ? 180.0 / Math.PI : 1.0;
            return name switch
            {
                "sin" => Math.Sin(a * toRad),
                "cos" => Math.Cos(a * toRad),
                "tan" => Math.Tan(a * toRad),
                "asin" => Math.Asin(a) * fromRad,
                "acos" => Math.Acos(a) * fromRad,
                "atan" => Math.Atan(a) * fromRad,
                "sinh" => Math.Sinh(a),
                "cosh" => Math.Cosh(a),
                "tanh" => Math.Tanh(a),
                "ln" => Math.Log(a),
                "log" => Math.Log10(a),
                "sqrt" => Math.Sqrt(a),
                "cbrt" => Math.Cbrt(a),
                "abs" => Math.Abs(a),
                "exp" => Math.Exp(a),
                "floor" => Math.Floor(a),
                "ceil" => Math.Ceiling(a),
                "round" => Math.Round(a),
                _ => double.NaN,
            };
        }
    }

    private enum TokenKind
    {
        End,
        Number,
        Ident,
        Plus,
        Minus,
        Star,
        Slash,
        Caret,
        Percent,
        LParen,
        RParen,
    }

    private readonly struct Token(TokenKind kind, string text, double number)
    {
        public TokenKind Kind { get; } = kind;
        public string Text { get; } = text;
        public double Number { get; } = number;
    }

    private sealed class Parser
    {
        private static readonly HashSet<string> s_functions = new(StringComparer.OrdinalIgnoreCase)
        {
            "sin", "cos", "tan", "asin", "acos", "atan",
            "sinh", "cosh", "tanh", "ln", "log", "sqrt",
            "cbrt", "abs", "exp", "floor", "ceil", "round",
        };

        private readonly string _text;
        private readonly List<Token> _tokens = new();
        private int _position;

        public Parser(string text)
        {
            _text = (text ?? string.Empty).Trim();
            // Accept optional "y =" prefix (equation form).
            if (_text.StartsWith("y=", StringComparison.OrdinalIgnoreCase))
            {
                _text = _text.Substring(2);
            }
            else if (_text.StartsWith("y =", StringComparison.OrdinalIgnoreCase))
            {
                _text = _text.Substring(3);
            }
        }

        public GraphExpression Parse()
        {
            if (!Tokenize(out string? error))
            {
                return new GraphExpression(null, error);
            }
            try
            {
                Node root = ParseExpr();
                Expect(TokenKind.End, "Unexpected token at end of expression.");
                return new GraphExpression(root, null);
            }
            catch (ParseException ex)
            {
                return new GraphExpression(null, ex.Message);
            }
        }

        private bool Tokenize(out string? error)
        {
            error = null;
            int i = 0;
            while (i < _text.Length)
            {
                char c = _text[i];
                if (char.IsWhiteSpace(c))
                {
                    i++;
                    continue;
                }
                if (char.IsDigit(c) || (c == '.' && i + 1 < _text.Length && char.IsDigit(_text[i + 1])))
                {
                    int start = i;
                    while (i < _text.Length && char.IsDigit(_text[i]))
                    {
                        i++;
                    }
                    if (i < _text.Length && _text[i] == '.')
                    {
                        i++;
                        if (i < _text.Length && !char.IsDigit(_text[i]))
                        {
                            error = "Too many decimal points.";
                            return false;
                        }
                        while (i < _text.Length && char.IsDigit(_text[i]))
                        {
                            i++;
                        }
                    }
                    if (i < _text.Length && (_text[i] == 'e' || _text[i] == 'E'))
                    {
                        int mark = i;
                        i++;
                        if (i < _text.Length && (_text[i] == '+' || _text[i] == '-'))
                        {
                            i++;
                        }
                        if (i >= _text.Length || !char.IsDigit(_text[i]))
                        {
                            i = mark; // bare 'e' (constant), not an exponent
                        }
                        else
                        {
                            while (i < _text.Length && char.IsDigit(_text[i]))
                            {
                                i++;
                            }
                        }
                    }
                    string lexeme = _text.Substring(start, i - start);
                    if (!double.TryParse(lexeme, System.Globalization.NumberStyles.Float,
                            System.Globalization.CultureInfo.InvariantCulture, out double value))
                    {
                        error = $"Invalid number '{lexeme}'.";
                        return false;
                    }
                    _tokens.Add(new Token(TokenKind.Number, lexeme, value));
                    continue;
                }
                if (char.IsLetter(c) || c == 'π')
                {
                    int start = i;
                    while (i < _text.Length && (char.IsLetterOrDigit(_text[i]) || _text[i] == 'π'))
                    {
                        i++;
                    }
                    _tokens.Add(new Token(TokenKind.Ident, _text.Substring(start, i - start), 0));
                    continue;
                }
                TokenKind? kind = c switch
                {
                    '+' => TokenKind.Plus,
                    '-' => TokenKind.Minus,
                    '*' => TokenKind.Star,
                    '/' => TokenKind.Slash,
                    '^' => TokenKind.Caret,
                    '%' => TokenKind.Percent,
                    '(' => TokenKind.LParen,
                    ')' => TokenKind.RParen,
                    _ => null,
                };
                if (kind == null)
                {
                    error = $"Unexpected token '{c}'.";
                    return false;
                }
                _tokens.Add(new Token(kind.Value, c.ToString(), 0));
                i++;
            }
            _tokens.Add(new Token(TokenKind.End, string.Empty, 0));
            return true;
        }

        private Token Current => _tokens[_position];

        private Token Next()
        {
            Token current = _tokens[_position];
            if (_position < _tokens.Count - 1)
            {
                _position++;
            }
            return current;
        }

        private void Expect(TokenKind kind, string message)
        {
            if (Current.Kind != kind)
            {
                throw new ParseException(message);
            }
        }

        private Node ParseExpr()
        {
            Node left = ParseTerm();
            while (Current.Kind is TokenKind.Plus or TokenKind.Minus)
            {
                char op = Next().Kind == TokenKind.Plus ? '+' : '-';
                left = new BinaryNode(op, left, ParseTerm());
            }
            return left;
        }

        private Node ParseTerm()
        {
            Node left = ParseUnary();
            while (true)
            {
                if (Current.Kind is TokenKind.Star or TokenKind.Slash or TokenKind.Percent)
                {
                    char op = Next().Kind switch
                    {
                        TokenKind.Star => '*',
                        TokenKind.Slash => '/',
                        _ => '%',
                    };
                    left = new BinaryNode(op, left, ParseUnary());
                }
                else if (StartsPrimary(Current))
                {
                    // Implicit multiplication: 2x, 2(x), (a)(b).
                    left = new BinaryNode('*', left, ParseUnary());
                }
                else
                {
                    return left;
                }
            }
        }

        private static bool StartsPrimary(Token token) =>
            token.Kind is TokenKind.Number or TokenKind.Ident or TokenKind.LParen;

        private Node ParseUnary()
        {
            if (Current.Kind == TokenKind.Minus)
            {
                Next();
                return new NegateNode(ParseUnary());
            }
            if (Current.Kind == TokenKind.Plus)
            {
                Next();
                return ParseUnary();
            }
            return ParsePower();
        }

        private Node ParsePower()
        {
            Node baseNode = ParsePrimary();
            if (Current.Kind == TokenKind.Caret)
            {
                Next();
                // Right-associative; unary binds tighter than the next ^ level.
                return new BinaryNode('^', baseNode, ParseUnary());
            }
            return baseNode;
        }

        private Node ParsePrimary()
        {
            Token token = Current;
            switch (token.Kind)
            {
                case TokenKind.Number:
                    Next();
                    return new ConstNode(token.Number);
                case TokenKind.Ident:
                    return ParseIdent();
                case TokenKind.LParen:
                    Next();
                    Node inner = ParseExpr();
                    if (Current.Kind != TokenKind.RParen)
                    {
                        throw new ParseException("Parenthesis mismatch.");
                    }
                    Next();
                    return inner;
                default:
                    throw new ParseException("Unexpected end of expression.");
            }
        }

        private Node ParseIdent()
        {
            string name = Next().Text;
            if (name.Equals("x", StringComparison.OrdinalIgnoreCase))
            {
                return new VarNode();
            }
            if (name.Equals("pi", StringComparison.OrdinalIgnoreCase) || name == "π")
            {
                return new ConstNode(Math.PI);
            }
            if (name.Equals("e", StringComparison.OrdinalIgnoreCase))
            {
                return new ConstNode(Math.E);
            }
            if (s_functions.Contains(name))
            {
                if (Current.Kind != TokenKind.LParen)
                {
                    throw new ParseException($"Expect parenthesis after function name '{name}'.");
                }
                Next();
                Node arg = ParseExpr();
                if (Current.Kind != TokenKind.RParen)
                {
                    throw new ParseException("Parenthesis mismatch.");
                }
                Next();
                return new FuncNode(name.ToLowerInvariant(), arg);
            }
            if (name.Length == 1 && char.IsLetter(name[0]))
            {
                return new ParamNode(name.ToLowerInvariant());
            }
            throw new ParseException($"Invalid token '{name}'.");
        }

        private sealed class ParseException(string message) : Exception(message)
        {
        }
    }
}
