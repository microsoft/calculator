// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Managed side of the unit-converter bridge (see
// linux/src/CalcManager.Linux/unit_converter_bridge.h). The native
// UnitConversionManager::UnitConverter owns the input state machine;
// categories/units/ratios come from UnitConverterDataLoader and display
// sinks surface as C# events. Same UTF-8 + GCHandle discipline as
// CalcManagerNative. Currency arrives in Task 7 (loader flag + source).
using System.Runtime.InteropServices;
using Calculator.Linux.DataLoaders;

namespace Calculator.Linux.Interop;

public enum UnitConverterCommand
{
    Zero = 0,
    One,
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,
    Decimal,
    Negate,
    Backspace,
    Clear,
    Reset,
    None,
}

[StructLayout(LayoutKind.Sequential)]
internal struct UCCategoryNative
{
    public int Id;
    public IntPtr Name;
    public int SupportsNegative;
}

[StructLayout(LayoutKind.Sequential)]
internal struct UCUnitNative
{
    public int Id;
    public IntPtr Name;
    public IntPtr Abbreviation;
    public IntPtr AccessibleName;
    public int IsSource;
    public int IsTarget;
    public int IsWhimsical;
}

[StructLayout(LayoutKind.Sequential)]
internal struct UCRatioNative
{
    public UCUnitNative Target;
    public double Ratio;
    public double Offset;
    public int OffsetFirst;
}

[StructLayout(LayoutKind.Sequential)]
internal struct UCSuggestedNative
{
    public IntPtr Value;
    public UCUnitNative Unit;
}

[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate int UCGetCategoriesDelegate(IntPtr output, int capacity, IntPtr userData);

[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate int UCGetUnitsDelegate(int categoryId, IntPtr output, int capacity, IntPtr userData);

[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate int UCGetRatiosDelegate(int unitId, IntPtr output, int capacity, IntPtr userData);

[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate int UCSupportsCategoryDelegate(int categoryId, IntPtr userData);

[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate void UCDisplayDelegate(IntPtr from, IntPtr to, IntPtr userData);

[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate void UCSuggestedDelegate(IntPtr items, int count, IntPtr userData);

[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate void UCSimpleDelegate(IntPtr userData);

[StructLayout(LayoutKind.Sequential)]
internal struct UnitConverterCallbacks
{
    public IntPtr UserData;
    public UCGetCategoriesDelegate? GetCategories;
    public UCGetUnitsDelegate? GetUnits;
    public UCGetRatiosDelegate? GetRatios;
    public UCSupportsCategoryDelegate? SupportsCategory;
    public UCDisplayDelegate? OnDisplay;
    public UCSuggestedDelegate? OnSuggested;
    public UCSimpleDelegate? OnMaxDigits;
}

internal static class UnitConverterNative
{
    private const string LibraryName = "CalcManager";

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr unit_converter_create(ref UnitConverterCallbacks callbacks, string? regionCode);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void unit_converter_destroy(IntPtr h);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void unit_converter_initialize(IntPtr h);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int unit_converter_get_categories(IntPtr h, IntPtr output, int capacity);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int unit_converter_set_category(
        IntPtr h, int categoryId, IntPtr unitsOut, int capacity, out int fromId, out int toId);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int unit_converter_get_current_category(IntPtr h, out UCCategoryNative current);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void unit_converter_set_unit_types(IntPtr h, int fromId, int toId);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void unit_converter_switch_active(IntPtr h, string? newValue);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int unit_converter_is_switched_active(IntPtr h);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int unit_converter_save_prefs(IntPtr h, IntPtr buffer, int capacity);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void unit_converter_restore_prefs(IntPtr h, string? prefs);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void unit_converter_send(IntPtr h, int command);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void unit_converter_calculate(IntPtr h);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void unit_converter_reset_ratios(IntPtr h);
}

/// <summary>
/// Managed owner of one native converter instance. Data callbacks serve the
/// loader tables; display callbacks surface as events for the ViewModel.
/// </summary>
public sealed class ManagedUnitConverter : IDisposable
{
    // Lifetime rule for loader-direction strings (C# -> native): native
    // copies callback payloads AFTER the callback returns, so buffers must
    // outlive the call. Each loader callback frees the PREVIOUS batch first
    // (native is done with it by then) and tracks the new one. All converter
    // calls originate on the UI thread; the lock is belt-and-braces.
    private static readonly object s_stringPoolLock = new();
    private static readonly List<IntPtr> s_stringPool = new();

    private static IntPtr PoolString(string value)
    {
        IntPtr ptr = Marshal.StringToHGlobalAnsi(value); // UTF-8 on Unix
        lock (s_stringPoolLock)
        {
            s_stringPool.Add(ptr);
        }
        return ptr;
    }

    private static List<IntPtr> TakePool()
    {
        lock (s_stringPoolLock)
        {
            var batch = new List<IntPtr>(s_stringPool);
            s_stringPool.Clear();
            return batch;
        }
    }
    private IntPtr _handle;
    private readonly GCHandle _self;
    private readonly UnitConverterDataLoader _loader;
    private bool _disposed;

    private readonly UCGetCategoriesDelegate _getCategories;
    private readonly UCGetUnitsDelegate _getUnits;
    private readonly UCGetRatiosDelegate _getRatios;
    private readonly UCSupportsCategoryDelegate _supportsCategory;
    private readonly UCDisplayDelegate _onDisplay;
    private readonly UCSuggestedDelegate _onSuggested;
    private readonly UCSimpleDelegate _onMaxDigits;

    public event Action<string, string>? DisplayChanged;
    public event Action<List<(string Value, UnitData Unit)>>? SuggestedChanged;
    public event Action? MaxDigitsReached;

    public string LastFrom { get; private set; } = "0";
    public string LastTo { get; private set; } = "0";
    public List<(string Value, UnitData Unit)> LastSuggested { get; } = new();

    public ManagedUnitConverter(UnitConverterDataLoader loader, string? regionCode = null)
    {
        Native.EnsureLoaded(); // registers the assembly DllImport resolver (see CalcManagerNative)
        _loader = loader;
        _loader.LoadData();
        _getCategories = GetCategories;
        _getUnits = GetUnits;
        _getRatios = GetRatios;
        _supportsCategory = SupportsCategory;
        _onDisplay = OnDisplay;
        _onSuggested = OnSuggested;
        _onMaxDigits = OnMaxDigits;
        _self = GCHandle.Alloc(this);

        var cbs = new UnitConverterCallbacks
        {
            UserData = GCHandle.ToIntPtr(_self),
            GetCategories = _getCategories,
            GetUnits = _getUnits,
            GetRatios = _getRatios,
            SupportsCategory = _supportsCategory,
            OnDisplay = _onDisplay,
            OnSuggested = _onSuggested,
            OnMaxDigits = _onMaxDigits,
        };
        _handle = UnitConverterNative.unit_converter_create(ref cbs, regionCode);
        if (_handle == IntPtr.Zero)
        {
            _self.Free();
            throw new InvalidOperationException("unit_converter_create failed.");
        }
        UnitConverterNative.unit_converter_initialize(_handle);
    }

    public List<CategoryData> Categories()
    {
        int count = UnitConverterNative.unit_converter_get_categories(_handle, IntPtr.Zero, 0);
        var result = new List<CategoryData>(count);
        if (count == 0)
        {
            return result;
        }
        int size = Marshal.SizeOf<UCCategoryNative>();
        IntPtr buf = Marshal.AllocHGlobal(size * count);
        try
        {
            UnitConverterNative.unit_converter_get_categories(_handle, buf, count);
            for (int i = 0; i < count; i++)
            {
                var native = Marshal.PtrToStructure<UCCategoryNative>(buf + i * size);
                result.Add(new CategoryData
                {
                    Id = native.Id,
                    Name = Marshal.PtrToStringUTF8(native.Name) ?? string.Empty,
                    SupportsNegative = native.SupportsNegative != 0,
                });
            }
        }
        finally
        {
            Marshal.FreeHGlobal(buf);
        }
        return result;
    }

    public (List<UnitData> Units, int FromId, int ToId) SetCategory(int categoryId)
    {
        int count = UnitConverterNative.unit_converter_set_category(_handle, categoryId, IntPtr.Zero, 0, out _, out _);
        var units = new List<UnitData>(count);
        int fromId = 0;
        int toId = 0;
        if (count > 0)
        {
            int size = Marshal.SizeOf<UCUnitNative>();
            IntPtr buf = Marshal.AllocHGlobal(size * count);
            try
            {
                UnitConverterNative.unit_converter_set_category(_handle, categoryId, buf, count, out fromId, out toId);
                for (int i = 0; i < count; i++)
                {
                    units.Add(ReadUnit(buf + i * size));
                }
            }
            finally
            {
                Marshal.FreeHGlobal(buf);
            }
        }
        return (units, fromId, toId);
    }

    public void SetUnitTypes(int fromId, int toId) => UnitConverterNative.unit_converter_set_unit_types(_handle, fromId, toId);
    public void SwitchActive(string value) => UnitConverterNative.unit_converter_switch_active(_handle, value);

    public CategoryData CurrentCategory()
    {
        if (UnitConverterNative.unit_converter_get_current_category(_handle, out UCCategoryNative native) != 0)
        {
            return new CategoryData();
        }
        return new CategoryData
        {
            Id = native.Id,
            Name = Marshal.PtrToStringUTF8(native.Name) ?? string.Empty,
            SupportsNegative = native.SupportsNegative != 0,
        };
    }
    public bool IsSwitchedActive => UnitConverterNative.unit_converter_is_switched_active(_handle) != 0;
    public void Send(UnitConverterCommand command) => UnitConverterNative.unit_converter_send(_handle, (int)command);
    public void Calculate() => UnitConverterNative.unit_converter_calculate(_handle);
    public void ResetRatios() => UnitConverterNative.unit_converter_reset_ratios(_handle);

    public string SavePrefs()
    {
        const int cap = 4096;
        IntPtr buf = Marshal.AllocHGlobal(cap);
        try
        {
            UnitConverterNative.unit_converter_save_prefs(_handle, buf, cap);
            return Marshal.PtrToStringUTF8(buf) ?? string.Empty;
        }
        finally
        {
            Marshal.FreeHGlobal(buf);
        }
    }

    public void RestorePrefs(string prefs) => UnitConverterNative.unit_converter_restore_prefs(_handle, prefs);

    private static ManagedUnitConverter Target(IntPtr userData) =>
        (ManagedUnitConverter)GCHandle.FromIntPtr(userData).Target!;

    private static void ReleasePriorStrings()
    {
        foreach (IntPtr ptr in TakePool())
        {
            Marshal.FreeHGlobal(ptr);
        }
    }

    private static int GetCategories(IntPtr output, int capacity, IntPtr userData)
    {
        ReleasePriorStrings();
        var self = Target(userData);
        var categories = self._loader.GetOrderedCategories();
        if (output == IntPtr.Zero || capacity == 0)
        {
            return categories.Length;
        }
        int size = Marshal.SizeOf<UCCategoryNative>();
        int count = Math.Min(capacity, categories.Length);
        for (int i = 0; i < count; i++)
        {
            Marshal.StructureToPtr(new UCCategoryNative
            {
                Id = categories[i].Id,
                Name = PoolString(categories[i].Name),
                SupportsNegative = categories[i].SupportsNegative ? 1 : 0,
            }, output + i * size, false);
        }
        return categories.Length;
    }

    private static int GetUnits(int categoryId, IntPtr output, int capacity, IntPtr userData)
    {
        ReleasePriorStrings();
        var self = Target(userData);
        var units = self._loader.GetUnitsForCategory(new CategoryData { Id = categoryId });
        if (output == IntPtr.Zero || capacity == 0)
        {
            return units.Length;
        }
        int size = Marshal.SizeOf<UCUnitNative>();
        int count = Math.Min(capacity, units.Length);
        for (int i = 0; i < count; i++)
        {
            WriteUnit(output + i * size, units[i]);
        }
        return units.Length;
    }

    private static void WriteUnit(IntPtr dest, UnitData unit)
    {
        Marshal.StructureToPtr(new UCUnitNative
        {
            Id = unit.Id,
            Name = PoolString(unit.Name),
            Abbreviation = PoolString(unit.Abbreviation),
            AccessibleName = PoolString(unit.AccessibleName),
            IsSource = unit.IsConversionSource ? 1 : 0,
            IsTarget = unit.IsConversionTarget ? 1 : 0,
            IsWhimsical = unit.IsWhimsical ? 1 : 0,
        }, dest, false);
    }

    private static int GetRatios(int unitId, IntPtr output, int capacity, IntPtr userData)
    {
        ReleasePriorStrings();
        var self = Target(userData);
        var ratios = self._loader.LoadOrderedRatios(new UnitData { Id = unitId });
        if (output == IntPtr.Zero || capacity == 0)
        {
            return ratios.Length;
        }
        int size = Marshal.SizeOf<UCRatioNative>();
        int count = Math.Min(capacity, ratios.Length);
        for (int i = 0; i < count; i++)
        {
            IntPtr structPtr = output + i * size;
            WriteUnit(structPtr, ratios[i].Unit);
            Marshal.WriteInt64(structPtr + Marshal.OffsetOf<UCRatioNative>(nameof(UCRatioNative.Ratio)).ToInt32(),
                BitConverter.DoubleToInt64Bits(ratios[i].Ratio));
            Marshal.WriteInt64(structPtr + Marshal.OffsetOf<UCRatioNative>(nameof(UCRatioNative.Offset)).ToInt32(),
                BitConverter.DoubleToInt64Bits(ratios[i].Offset));
            Marshal.WriteInt32(structPtr + Marshal.OffsetOf<UCRatioNative>(nameof(UCRatioNative.OffsetFirst)).ToInt32(),
                ratios[i].OffsetFirst ? 1 : 0);
        }
        return ratios.Length;
    }

    private static int SupportsCategory(int categoryId, IntPtr userData) =>
        Target(userData)._loader.SupportsCategory(new CategoryData { Id = categoryId }) ? 1 : 0;

    private static void OnDisplay(IntPtr from, IntPtr to, IntPtr userData)
    {
        var self = Target(userData);
        self.LastFrom = Marshal.PtrToStringUTF8(from) ?? string.Empty;
        self.LastTo = Marshal.PtrToStringUTF8(to) ?? string.Empty;
        self.DisplayChanged?.Invoke(self.LastFrom, self.LastTo);
    }

    private static void OnSuggested(IntPtr items, int count, IntPtr userData)
    {
        var self = Target(userData);
        self.LastSuggested.Clear();
        if (count > 0)
        {
            int size = Marshal.SizeOf<UCSuggestedNative>();
            for (int i = 0; i < count; i++)
            {
                var native = Marshal.PtrToStructure<UCSuggestedNative>(items + i * size);
                self.LastSuggested.Add((
                    Marshal.PtrToStringUTF8(native.Value) ?? string.Empty,
                    ReadUnitStruct(native.Unit)));
            }
        }
        self.SuggestedChanged?.Invoke(new List<(string, UnitData)>(self.LastSuggested));
    }

    private static void OnMaxDigits(IntPtr userData) => Target(userData).MaxDigitsReached?.Invoke();

    private static UnitData ReadUnit(IntPtr ptr)
    {
        var native = Marshal.PtrToStructure<UCUnitNative>(ptr);
        return ReadUnitStruct(native);
    }

    private static UnitData ReadUnitStruct(UCUnitNative native) =>
        new()
        {
            Id = native.Id,
            Name = Marshal.PtrToStringUTF8(native.Name) ?? string.Empty,
            Abbreviation = Marshal.PtrToStringUTF8(native.Abbreviation) ?? string.Empty,
            AccessibleName = Marshal.PtrToStringUTF8(native.AccessibleName) ?? string.Empty,
            IsConversionSource = native.IsSource != 0,
            IsConversionTarget = native.IsTarget != 0,
            IsWhimsical = native.IsWhimsical != 0,
        };

    public void Dispose()
    {
        if (_disposed)
        {
            return;
        }
        _disposed = true;
        if (_handle != IntPtr.Zero)
        {
            UnitConverterNative.unit_converter_destroy(_handle);
            _handle = IntPtr.Zero;
        }
        if (_self.IsAllocated)
        {
            _self.Free();
        }
        GC.SuppressFinalize(this);
    }

    ~ManagedUnitConverter() => Dispose();
}
