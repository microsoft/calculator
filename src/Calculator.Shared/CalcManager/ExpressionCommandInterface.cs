// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp;
using System;
using System.Runtime.InteropServices;

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
		public CalculationManager.CommandType GetCommandType() => CommandType.Parentheses;
		public void Accept(ISerializeCommandVisitor commandVisitor) => throw new NotImplementedException();
    }

    public class CUnaryCommand : IUnaryCommand
    {
		private IntPtr m_pExpressionCommand;

		public CUnaryCommand(IntPtr pExpressionCommand) => this.m_pExpressionCommand = pExpressionCommand;

        public CUnaryCommand(int command) => throw new NotImplementedException();
        public CUnaryCommand(int command1, int command2) => throw new NotImplementedException();

		public CalculatorList<int> GetCommands()
		{
			var pResult = NativeDispatch.CUnaryCommand_GetCommands(m_pExpressionCommand);
			var result = Marshal.PtrToStructure<CUnaryCommand_GetCommandsResult>(pResult);

			int[] commandsArray = new int[result.CommandCount];
			Marshal.Copy(result.Commands, commandsArray, 0, commandsArray.Length);

			NativeDispatch.Free(result.Commands);
			NativeDispatch.Free(pResult);

			return new CalculatorList<int>(commandsArray);
		}

		public CalculationManager.CommandType GetCommandType() => CommandType.UnaryCommand;
		public void SetCommand(int command) => throw new NotImplementedException();
        public void SetCommands(int command1, int command2) => throw new NotImplementedException();
        public void Accept(ISerializeCommandVisitor commandVisitor) => throw new NotImplementedException();
    }

	public class CBinaryCommand : IBinaryCommand
	{
		private IntPtr m_pExpressionCommand;

		public CBinaryCommand(IntPtr pExpressionCommand) => this.m_pExpressionCommand = pExpressionCommand;

		public CBinaryCommand(int command) => throw new NotImplementedException();
		public void SetCommand(int command) => throw new NotImplementedException();
		public int GetCommand() => NativeDispatch.CBinaryCommand_GetCommand(m_pExpressionCommand);
		public CalculationManager.CommandType GetCommandType() => CommandType.BinaryCommand;
		public void Accept(ISerializeCommandVisitor commandVisitor) => throw new NotImplementedException();
	}

	public class COpndCommand : IOpndCommand
    {
		private IntPtr m_pExpressionCommand;

		public COpndCommand(CalculatorList<int> commands, bool fNegative, bool fDecimal, bool fSciFmt) => throw new NotImplementedException();
		public COpndCommand(IntPtr pExpressionCommand) => this.m_pExpressionCommand = pExpressionCommand;

		// public void Initialize(CalcEngine.Rational rat) => throw new NotImplementedException();

		public CalculatorList<int> GetCommands()
		{
			var pResult = NativeDispatch.COpndCommand_GetCommands(m_pExpressionCommand);
			var result = Marshal.PtrToStructure<COpndCommand_GetCommandsResult>(pResult);

			int[] commandsArray = new int[result.CommandCount];
			Marshal.Copy(result.Commands, commandsArray, 0, commandsArray.Length);

			NativeDispatch.Free(result.Commands);
			NativeDispatch.Free(pResult);

			return new CalculatorList<int>(commandsArray);
		}

		public void SetCommands(CalculatorList<int> commands) => throw new NotImplementedException();
        public void AppendCommand(int command) => throw new NotImplementedException();
        public void ToggleSign() => throw new NotImplementedException();
        public void RemoveFromEnd() => throw new NotImplementedException();
        public bool IsNegative() => NativeDispatch.COpndCommand_IsNegative(m_pExpressionCommand);
        public bool IsSciFmt() => throw new NotImplementedException();
        public bool IsDecimalPresent() => throw new NotImplementedException();
        public string GetToken(char decimalSymbol) => throw new NotImplementedException();
        public CalculationManager.CommandType GetCommandType() => CommandType.OperandCommand;
        public void Accept(ISerializeCommandVisitor commandVisitor) => throw new NotImplementedException();
        public string GetString(uint radix, int precision) => throw new NotImplementedException();
    }
}
