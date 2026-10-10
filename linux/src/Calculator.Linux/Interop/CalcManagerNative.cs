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

    public string PrimaryDisplay { get; private set; } = "0";
    public bool IsInError { get; private set; }
    public int HistoryAddedCount { get; private set; }
    public uint HistoryCount => _handle != IntPtr.Zero ? Native.calc_manager_history_count(_handle) : 0;

    public CalcEngine(string? decimalSep = null, string? thousandSep = null, string? grouping = null)
    {
        Native.EnsureLoaded();
        _onPrimary = OnPrimaryDisplay;
        _onError = OnIsInError;
        _onHistory = OnHistoryAdded;
        _self = GCHandle.Alloc(this);

        var cb = new DisplayCallbacks
        {
            UserData = GCHandle.ToIntPtr(_self),
            OnPrimaryDisplay = _onPrimary,
            OnIsInError = _onError,
            OnHistoryItemAdded = _onHistory,
        };
        _handle = Native.calc_manager_create(ref cb, decimalSep, thousandSep, grouping);
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
    public bool IsInputEmpty => Native.calc_manager_is_input_empty(_handle) != 0;

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
    }

    private static void OnIsInError(int isError, IntPtr userData) => Target(userData).IsInError = isError != 0;
    private static void OnHistoryAdded(uint index, IntPtr userData) => Target(userData).HistoryAddedCount++;

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
