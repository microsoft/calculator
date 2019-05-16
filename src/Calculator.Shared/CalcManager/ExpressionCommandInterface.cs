// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp;
using System;

namespace CalculationManager
{
    public interface ISerializeCommandVisitor
    {
        void Visit(COpndCommand opndCmd);
        void Visit(CUnaryCommand unaryCmd);
        void Visit(CBinaryCommand binaryCmd);
        void Visit(CParentheses paraCmd);
    };


    public interface IExpressionCommand
    {
        CalculationManager.CommandType GetCommandType();
        void Accept(ISerializeCommandVisitor commandVisitor);
    }

    public interface IOperatorCommand : IExpressionCommand
    {
        void SetCommand(int command);
    }

    public interface IUnaryCommand : IOperatorCommand
    {
        CalculatorList<int> GetCommands();
        void SetCommands(int command1, int command2);
    }

    public interface IBinaryCommand : IOperatorCommand
    {
        new void SetCommand(int command);
        int GetCommand();
    }

    public interface IOpndCommand : IExpressionCommand
    {
        CalculatorList<int> GetCommands();
        void AppendCommand(int command);
        void ToggleSign();
        void RemoveFromEnd();
        bool IsNegative();
        bool IsSciFmt();
        bool IsDecimalPresent();
        string GetToken(char decimalSymbol);
        void SetCommands(CalculatorList<int> commands);
    }

    public interface IParenthesisCommand : IExpressionCommand
    {
        int GetCommand();
    }




    public class CParentheses : IParenthesisCommand
    {
		private IntPtr pExpressionCommand;

		public CParentheses(int command) => throw new NotImplementedException();
		public CParentheses(IntPtr pExpressionCommand) => this.pExpressionCommand = pExpressionCommand;

		public int GetCommand() => throw new NotImplementedException();
        public CalculationManager.CommandType GetCommandType() => throw new NotImplementedException();
        public void Accept(ISerializeCommandVisitor commandVisitor) => throw new NotImplementedException();
    }

    public class CUnaryCommand : IUnaryCommand
    {
		private IntPtr pExpressionCommand;

		public CUnaryCommand(IntPtr pExpressionCommand) => this.pExpressionCommand = pExpressionCommand;

        public CUnaryCommand(int command) => throw new NotImplementedException();
        public CUnaryCommand(int command1, int command2) => throw new NotImplementedException();
        public CalculatorList<int> GetCommands() => throw new NotImplementedException();
        public CalculationManager.CommandType GetCommandType() => throw new NotImplementedException();
        public void SetCommand(int command) => throw new NotImplementedException();
        public void SetCommands(int command1, int command2) => throw new NotImplementedException();
        public void Accept(ISerializeCommandVisitor commandVisitor) => throw new NotImplementedException();
    }

	public class CBinaryCommand : IBinaryCommand
	{
		private IntPtr pExpressionCommand;

		public CBinaryCommand(IntPtr pExpressionCommand) => this.pExpressionCommand = pExpressionCommand;

		public CBinaryCommand(int command) => throw new NotImplementedException();
		public void SetCommand(int command) => throw new NotImplementedException();
		public int GetCommand() => throw new NotImplementedException();
		public CommandType GetCommandType() => throw new NotImplementedException();
		public void Accept(ISerializeCommandVisitor commandVisitor) => throw new NotImplementedException();
	}

	public class COpndCommand : IOpndCommand
    {
		private IntPtr pExpressionCommand;

		public COpndCommand(CalculatorList<int> commands, bool fNegative, bool fDecimal, bool fSciFmt) => throw new NotImplementedException();
		public COpndCommand(IntPtr pExpressionCommand) => this.pExpressionCommand = pExpressionCommand;

		// public void Initialize(CalcEngine.Rational rat) => throw new NotImplementedException();

		public CalculatorList<int> GetCommands() => throw new NotImplementedException();
        public void SetCommands(CalculatorList<int> commands) => throw new NotImplementedException();
        public void AppendCommand(int command) => throw new NotImplementedException();
        public void ToggleSign() => throw new NotImplementedException();
        public void RemoveFromEnd() => throw new NotImplementedException();
        public bool IsNegative() => throw new NotImplementedException();
        public bool IsSciFmt() => throw new NotImplementedException();
        public bool IsDecimalPresent() => throw new NotImplementedException();
        public string GetToken(char decimalSymbol) => throw new NotImplementedException();
        public CalculationManager.CommandType GetCommandType() => throw new NotImplementedException();
        public void Accept(ISerializeCommandVisitor commandVisitor) => throw new NotImplementedException();
        public string GetString(uint radix, int precision) => throw new NotImplementedException();
    }
}
