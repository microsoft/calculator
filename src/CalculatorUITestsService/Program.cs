using CalculatorUITestFramework;

using var server = new MockedCurrencyServer();
var sem = new SemaphoreSlim(0);
_ = Task.Run(() =>
{
    Console.WriteLine("Press any key to exit:");
    Console.ReadKey();
    sem.Release();
});
sem.Wait();
