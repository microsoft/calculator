// Placeholder entry point (Task 1). Proves the .NET toolchain restores and
// runs alongside libCalcManager.so. Replaced by the Avalonia App in Task 4.
Console.WriteLine($"Calculator.Linux placeholder — Avalonia {typeof(Avalonia.Application).Assembly.GetName().Version} loads OK.");
Console.WriteLine($"Runtime: {System.Runtime.InteropServices.RuntimeInformation.FrameworkDescription}");
return 0;
