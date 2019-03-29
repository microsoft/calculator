# CalculatorTest

CalculatorTest is a sample test project that runs and validates basic UI scenarios on the **Microsoft Open Sourced Calculator** application. This sample is created as the most basic test project to quickly try out Windows Application Driver.

This test project highlights the following basic interactions to demonstrate how UI testing using Windows Application Driver work.
- Creating a modern UWP app session
- Finding element using name
- Finding element using accessibility id
- Finding element using XPath
- Sending click action to an element
- Retrieving element value
- Navigating using SplitViewPane


## Requirements

- Windows 10 PC with the latest Windows 10 version (Version 1809 or later)
- Microsoft Visual Studio 2017 or later


## Getting Started

1. [Run](../../../README.md#installing-and-running-windows-application-driver) `WinAppDriver.exe` on the test device
2. Open `CalculatorTest.sln` in Visual Studio
3. Select **Build** > **Rebuild Solution**
4. Select **Test** > **Windows** > **Test Explorer**
5. Select **Run All** on the test pane or through menu **Test** > **Run** > **All Tests**

> Once the project is successfully built, you can use the **TestExplorer** to pick and choose the test scenario(s) to run

> If Visual Studio fail to discover and run the test scenarios:
> 1. Select **Tools** > **Options...** > **Test**
> 2. Under *Active Solution*, uncheck *For improved performance, only use test adapters in test assembly folder or as specified in runsettings file*
