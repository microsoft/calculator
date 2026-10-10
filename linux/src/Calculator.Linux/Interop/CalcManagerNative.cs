// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// P/Invoke layer over libCalcManager.so (see
// linux/src/CalcManager.Linux/calc_manager_c_api.h — the frozen C ABI).
//
// This replaces src/CalcManager.Interop (C++/CX + WinRT .idl), which cannot
// compile on Linux. Design rules:
//   - UTF-8 (IntPtr + PtrToStringUTF8) everywhere; never CharSet.Unicode,
//     because wchar_t is 4 bytes on Linux but 2 bytes in C#.
//   - Callbacks are static methods routed via the GCHandle passed as
//     userData; delegate instances are rooted as fields (never collected).
//   - No `Windows.*` namespaces anywhere in this tree (build gate in Task 10).
using System.Reflection;
using System.Runtime.InteropServices;

namespace Calculator.Linux.Interop;

[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate void PrimaryDisplayCallback(IntPtr utf8Text, int isError, IntPtr userData);

[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate void IsInErrorCallback(int isError, IntPtr userData);

[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate void ExpressionCallback(IntPtr tokens, IntPtr indices, int count, IntPtr userData);

[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate void ParenthesisCallback(uint count, IntPtr userData);

[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate void SimpleCallback(IntPtr userData);

[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate void HistoryAddedCallback(uint index, IntPtr userData);

[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate void MemoryCallback(int changedIndex, IntPtr userData);

[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate void ExpressionExCallback(IntPtr tokens, IntPtr indices, IntPtr cmdTypes, int count, IntPtr userData);

[StructLayout(LayoutKind.Sequential)]
internal struct DisplayCallbacks
{
    public IntPtr UserData;
    public PrimaryDisplayCallback? OnPrimaryDisplay;
    public IsInErrorCallback? OnIsInError;
    public ExpressionCallback? OnExpression;
    public ParenthesisCallback? OnParenthesis;
    public SimpleCallback? OnNoRightParenAdded;
    public SimpleCallback? OnMaxDigitsReached;
    public SimpleCallback? OnBinaryOperatorReceived;
    public HistoryAddedCallback? OnHistoryItemAdded;
    public MemoryCallback? OnMemoryChanged;
    public SimpleCallback? OnInputChanged;
}

[StructLayout(LayoutKind.Sequential)]
internal struct DisplayCallbacksEx
{
    public DisplayCallbacks Base;
    public ExpressionExCallback? OnExpressionEx;
}

internal static class Native
{
    private const string LibraryName = "CalcManager";

    static Native()
    {
        NativeLibrary.SetDllImportResolver(typeof(Native).Assembly, Resolve);
    }

    private static IntPtr Resolve(string libraryName, Assembly assembly, DllImportSearchPath? searchPath)
    {
        if (libraryName != LibraryName)
        {
            return IntPtr.Zero;
        }

        // 1. Explicit override (CI / custom engine builds).
        string? env = Environment.GetEnvironmentVariable("CALCMANAGER_PATH");
        if (!string.IsNullOrEmpty(env) && NativeLibrary.TryLoad(env, out IntPtr envHandle))
        {
            return envHandle;
        }

        // 2. In-tree build output: <repo>/linux/build/calc-manager/.
        // Test/placeholder binaries run from linux/{src,tests}/.../bin/<cfg>/<tfm>/.
        string? dir = AppContext.BaseDirectory;
        for (int i = 0; i < 8 && dir != null; i++)
        {
            string candidate = Path.Combine(dir, "build", "calc-manager", "libCalcManager.so");
            if (File.Exists(candidate) && NativeLibrary.TryLoad(candidate, out IntPtr treeHandle))
            {
                return treeHandle;
            }
            // Also match the repo layout where base is linux/src/... (walk up to linux/).
            string linuxRoot = Path.Combine(dir, "linux", "build", "calc-manager", "libCalcManager.so");
            if (File.Exists(linuxRoot) && NativeLibrary.TryLoad(linuxRoot, out IntPtr rootHandle))
            {
                return rootHandle;
            }
            dir = Directory.GetParent(dir)?.FullName;
        }

        // 3. System library paths (future .deb installs to /usr/lib/...).
        if (NativeLibrary.TryLoad("libCalcManager.so", out IntPtr sysHandle))
        {
            return sysHandle;
        }

        return IntPtr.Zero;
    }

    public static void EnsureLoaded() { } // triggers static ctor

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr calc_manager_create(ref DisplayCallbacks callbacks, string? decimalSep, string? thousandSep, string? grouping);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr calc_manager_create_ex(ref DisplayCallbacksEx callbacks, string? decimalSep, string? thousandSep, string? grouping);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void calc_manager_destroy(IntPtr h);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void calc_manager_reset(IntPtr h, int clearMemory);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void calc_manager_set_standard_mode(IntPtr h);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void calc_manager_set_scientific_mode(IntPtr h);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void calc_manager_set_programmer_mode(IntPtr h);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void calc_manager_send_command(IntPtr h, int commandId);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void calc_manager_memorize(IntPtr h);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void calc_manager_memory_load(IntPtr h, uint index);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void calc_manager_memory_add(IntPtr h, uint index);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void calc_manager_memory_subtract(IntPtr h, uint index);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void calc_manager_memory_clear(IntPtr h, uint index);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void calc_manager_memory_clear_all(IntPtr h);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int calc_manager_is_engine_recording(IntPtr h);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int calc_manager_is_input_empty(IntPtr h);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int calc_manager_is_in_error(IntPtr h);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int calc_manager_get_primary_display(IntPtr h, IntPtr buffer, int capacity);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void calc_manager_set_radix(IntPtr h, int radixType);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void calc_manager_set_precision(IntPtr h, int precision);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern uint calc_manager_history_count(IntPtr h);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void calc_manager_history_clear(IntPtr h);

    // --- Task 4 additive imports ---

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int calc_manager_history_item(
        IntPtr h, int mode, uint index, IntPtr expression, int expressionCapacity, IntPtr result, int resultCapacity);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern uint calc_manager_history_count_for_mode(IntPtr h, int mode);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern uint calc_manager_max_history_size(IntPtr h);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int calc_manager_remove_history_item(IntPtr h, uint index);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern uint calc_manager_history_item_token_count(IntPtr h, int mode, uint index);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int calc_manager_history_item_token(
        IntPtr h, int mode, uint item, uint token, IntPtr buffer, int capacity, out int cmdIndex, out int cmdType);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern uint calc_manager_history_item_commands(
        IntPtr h, int mode, uint index, [Out] int[]? output, uint capacity);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern uint calc_manager_display_commands(IntPtr h, [Out] int[]? output, uint capacity);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern uint calc_manager_memorized_count(IntPtr h);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int calc_manager_memorized_at(IntPtr h, uint index, IntPtr buffer, int capacity);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void calc_manager_update_max_int_digits(IntPtr h);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int calc_manager_get_result_for_radix(
        IntPtr h, uint radix, int precision, int groupDigits, IntPtr buffer, int capacity);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int calc_manager_get_current_degree_mode(IntPtr h);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void calc_manager_set_in_history_load_mode(IntPtr h, int enabled);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void calc_manager_refresh_memorized_numbers(IntPtr h);
}

/// <summary>
/// Managed owner of one engine instance. Push state (callbacks) and pull
/// accessors mirror the C ABI; Task 4 binds ViewModels to this class.
/// </summary>
public sealed class CalcEngine : IDisposable
{
    private IntPtr _handle;
    private readonly GCHandle _self;
    private bool _disposed;

    // Rooted delegates: the native side holds these function pointers for the
    // lifetime of the engine. If they were collected, callbacks would crash.
    private readonly PrimaryDisplayCallback _onPrimary;
    private readonly IsInErrorCallback _onError;
    private readonly HistoryAddedCallback _onHistory;
    private readonly ExpressionExCallback _onExpressionEx;
    private readonly MemoryCallback _onMemory;
    private readonly SimpleCallback _onInputChanged;
    private readonly ParenthesisCallback _onParenthesis;
    private readonly SimpleCallback _onNoRightParen;
    private readonly SimpleCallback _onMaxDigits;
    private readonly SimpleCallback _onBinaryOp;

    public string PrimaryDisplay { get; private set; } = "0";
    public bool IsInError { get; private set; }
    public int HistoryAddedCount { get; private set; }
    public uint OpenParenthesisCount { get; private set; }
    public bool IsInputEmpty => _handle != IntPtr.Zero && Native.calc_manager_is_input_empty(_handle) != 0;
    public bool IsEngineRecording => _handle != IntPtr.Zero && Native.calc_manager_is_engine_recording(_handle) != 0;
    public uint HistoryCount => _handle != IntPtr.Zero ? Native.calc_manager_history_count(_handle) : 0;

    public event Action? ExpressionChanged;
    public event Action<int>? MemoryChanged;
    public event Action? InputChanged;
    public event Action<string, bool>? PrimaryDisplayChanged;
    public event Action<bool>? IsInErrorChanged;
    public event Action<uint>? ParenthesesChanged;
    public event Action? NoRightParenAdded;
    public event Action? MaxDigitsReached;
    public event Action? BinaryOperatorReceived;
    public event Action<uint>? HistoryItemAdded;

    public List<(string Token, int CommandIndex, int CommandType)> ExpressionTokens { get; } = new();

    public CalcEngine(string? decimalSep = null, string? thousandSep = null, string? grouping = null)
    {
        Native.EnsureLoaded();
        _onPrimary = OnPrimaryDisplay;
        _onError = OnIsInError;
        _onHistory = OnHistoryAdded;
        _onExpressionEx = OnExpressionEx;
        _onMemory = OnMemoryChanged;
        _onInputChanged = OnInputChanged;
        _onParenthesis = OnParenthesis;
        _onNoRightParen = OnNoRightParenAdded;
        _onMaxDigits = OnMaxDigitsReached;
        _onBinaryOp = OnBinaryOperatorReceived;
        _self = GCHandle.Alloc(this);

        var cb = new DisplayCallbacksEx
        {
            Base = new DisplayCallbacks
            {
                UserData = GCHandle.ToIntPtr(_self),
                OnPrimaryDisplay = _onPrimary,
                OnIsInError = _onError,
                OnHistoryItemAdded = _onHistory,
                OnMemoryChanged = _onMemory,
                OnInputChanged = _onInputChanged,
                OnParenthesis = _onParenthesis,
                OnNoRightParenAdded = _onNoRightParen,
                OnMaxDigitsReached = _onMaxDigits,
                OnBinaryOperatorReceived = _onBinaryOp,
            },
            OnExpressionEx = _onExpressionEx,
        };
        _handle = Native.calc_manager_create_ex(ref cb, decimalSep, thousandSep, grouping);
        if (_handle == IntPtr.Zero)
        {
            _self.Free();
            throw new InvalidOperationException("calc_manager_create failed (native engine did not initialize).");
        }
    }

    public void SetStandardMode() => Native.calc_manager_set_standard_mode(_handle);
    public void SetScientificMode() => Native.calc_manager_set_scientific_mode(_handle);
    public void SetProgrammerMode() => Native.calc_manager_set_programmer_mode(_handle);
    public void Send(int commandId) => Native.calc_manager_send_command(_handle, commandId);
    public void Reset(bool clearMemory = true) => Native.calc_manager_reset(_handle, clearMemory ? 1 : 0);
    public void Memorize() => Native.calc_manager_memorize(_handle);
    public void MemoryLoad(uint index) => Native.calc_manager_memory_load(_handle, index);
    public void MemoryAdd(uint index) => Native.calc_manager_memory_add(_handle, index);
    public void MemorySubtract(uint index) => Native.calc_manager_memory_subtract(_handle, index);
    public void MemoryClear(uint index) => Native.calc_manager_memory_clear(_handle, index);
    public void MemoryClearAll() => Native.calc_manager_memory_clear_all(_handle);
    public void ClearHistory() => Native.calc_manager_history_clear(_handle);
    public void SetRadix(int radixType) => Native.calc_manager_set_radix(_handle, radixType);
    public void SetPrecision(int precision) => Native.calc_manager_set_precision(_handle, precision);
    public void UpdateMaxIntDigits() => Native.calc_manager_update_max_int_digits(_handle);
    public void SetHistoryLoadMode(bool enabled) => Native.calc_manager_set_in_history_load_mode(_handle, enabled ? 1 : 0);
    public void RefreshMemorizedNumbers() => Native.calc_manager_refresh_memorized_numbers(_handle);
    public int CurrentDegreeMode => Native.calc_manager_get_current_degree_mode(_handle);
    public uint MaxHistorySize => Native.calc_manager_max_history_size(_handle);
    public bool RemoveHistoryItem(uint index) => Native.calc_manager_remove_history_item(_handle, index) != 0;
    public uint HistoryCountForMode(int mode) => Native.calc_manager_history_count_for_mode(_handle, mode);

    public (string Expression, string Result)? HistoryItem(int mode, uint index)
    {
        const int cap = 1024;
        IntPtr expr = Marshal.AllocHGlobal(cap);
        IntPtr result = Marshal.AllocHGlobal(cap);
        try
        {
            int rc = Native.calc_manager_history_item(_handle, mode, index, expr, cap, result, cap);
            if (rc != 0)
            {
                return null;
            }
            return (Marshal.PtrToStringUTF8(expr) ?? string.Empty, Marshal.PtrToStringUTF8(result) ?? string.Empty);
        }
        finally
        {
            Marshal.FreeHGlobal(expr);
            Marshal.FreeHGlobal(result);
        }
    }

    public List<(string Token, int CommandIndex, int CommandType)> HistoryItemTokens(int mode, uint index)
    {
        var tokens = new List<(string, int, int)>();
        uint count = Native.calc_manager_history_item_token_count(_handle, mode, index);
        const int cap = 512;
        IntPtr buf = Marshal.AllocHGlobal(cap);
        try
        {
            for (uint t = 0; t < count; t++)
            {
                int rc = Native.calc_manager_history_item_token(_handle, mode, index, t, buf, cap, out int cmdIndex, out int cmdType);
                if (rc == 0)
                {
                    tokens.Add((Marshal.PtrToStringUTF8(buf) ?? string.Empty, cmdIndex, cmdType));
                }
            }
        }
        finally
        {
            Marshal.FreeHGlobal(buf);
        }
        return tokens;
    }

    public List<int> HistoryItemCommands(int mode, uint index)
    {
        uint needed = Native.calc_manager_history_item_commands(_handle, mode, index, null, 0);
        if (needed == 0)
        {
            return new List<int>();
        }
        var output = new int[needed];
        Native.calc_manager_history_item_commands(_handle, mode, index, output, needed);
        return new List<int>(output);
    }

    public List<int> DisplayCommands()
    {
        uint needed = Native.calc_manager_display_commands(_handle, null, 0);
        if (needed == 0)
        {
            return new List<int>();
        }
        var output = new int[needed];
        Native.calc_manager_display_commands(_handle, output, needed);
        return new List<int>(output);
    }

    public List<string> MemorizedNumbers()
    {
        var numbers = new List<string>();
        uint count = Native.calc_manager_memorized_count(_handle);
        const int cap = 256;
        IntPtr buf = Marshal.AllocHGlobal(cap);
        try
        {
            for (uint i = 0; i < count; i++)
            {
                if (Native.calc_manager_memorized_at(_handle, i, buf, cap) >= 0)
                {
                    numbers.Add(Marshal.PtrToStringUTF8(buf) ?? string.Empty);
                }
            }
        }
        finally
        {
            Marshal.FreeHGlobal(buf);
        }
        return numbers;
    }

    public string ResultForRadix(uint radix, int precision, bool groupDigits)
    {
        const int cap = 256;
        IntPtr buf = Marshal.AllocHGlobal(cap);
        try
        {
            Native.calc_manager_get_result_for_radix(_handle, radix, precision, groupDigits ? 1 : 0, buf, cap);
            return Marshal.PtrToStringUTF8(buf) ?? string.Empty;
        }
        finally
        {
            Marshal.FreeHGlobal(buf);
        }
    }

    public string PullPrimaryDisplay()
    {
        const int cap = 256;
        IntPtr buf = Marshal.AllocHGlobal(cap);
        try
        {
            Native.calc_manager_get_primary_display(_handle, buf, cap);
            return Marshal.PtrToStringUTF8(buf) ?? string.Empty;
        }
        finally
        {
            Marshal.FreeHGlobal(buf);
        }
    }

    private static CalcEngine Target(IntPtr userData) =>
        (CalcEngine)GCHandle.FromIntPtr(userData).Target!;

    private static void OnPrimaryDisplay(IntPtr utf8Text, int isError, IntPtr userData)
    {
        var self = Target(userData);
        self.PrimaryDisplay = Marshal.PtrToStringUTF8(utf8Text) ?? string.Empty;
        self.IsInError = isError != 0;
        self.PrimaryDisplayChanged?.Invoke(self.PrimaryDisplay, self.IsInError);
    }

    private static void OnIsInError(int isError, IntPtr userData)
    {
        var self = Target(userData);
        self.IsInError = isError != 0;
        self.IsInErrorChanged?.Invoke(self.IsInError);
    }

    private static void OnHistoryAdded(uint index, IntPtr userData)
    {
        var self = Target(userData);
        self.HistoryAddedCount++;
        self.HistoryItemAdded?.Invoke(index);
    }

    private static void OnExpressionEx(IntPtr tokens, IntPtr indices, IntPtr cmdTypes, int count, IntPtr userData)
    {
        var self = Target(userData);
        self.ExpressionTokens.Clear();
        if (count > 0)
        {
            var tokenPtrs = new IntPtr[count];
            var indexVals = new int[count];
            var typeVals = new int[count];
            Marshal.Copy(tokens, tokenPtrs, 0, count);
            Marshal.Copy(indices, indexVals, 0, count);
            Marshal.Copy(cmdTypes, typeVals, 0, count);
            for (int i = 0; i < count; i++)
            {
                self.ExpressionTokens.Add((Marshal.PtrToStringUTF8(tokenPtrs[i]) ?? string.Empty, indexVals[i], typeVals[i]));
            }
        }
        self.ExpressionChanged?.Invoke();
    }

    private static void OnMemoryChanged(int changedIndex, IntPtr userData) =>
        Target(userData).MemoryChanged?.Invoke(changedIndex);
    private static void OnInputChanged(IntPtr userData) => Target(userData).InputChanged?.Invoke();
    private static void OnParenthesis(uint count, IntPtr userData)
    {
        var self = Target(userData);
        self.OpenParenthesisCount = count;
        self.ParenthesesChanged?.Invoke(count);
    }
    private static void OnNoRightParenAdded(IntPtr userData) => Target(userData).NoRightParenAdded?.Invoke();
    private static void OnMaxDigitsReached(IntPtr userData) => Target(userData).MaxDigitsReached?.Invoke();
    private static void OnBinaryOperatorReceived(IntPtr userData) => Target(userData).BinaryOperatorReceived?.Invoke();

    public void Dispose()
    {
        if (_disposed)
        {
            return;
        }
        _disposed = true;
        if (_handle != IntPtr.Zero)
        {
            Native.calc_manager_destroy(_handle);
            _handle = IntPtr.Zero;
        }
        if (_self.IsAllocated)
        {
            _self.Free();
        }
        GC.SuppressFinalize(this);
    }

    ~CalcEngine() => Dispose();
}
