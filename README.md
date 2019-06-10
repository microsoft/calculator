# Uno Calculator

The Uno Calculator is a port of the [Windows Calculator](https://github.com/microsoft/calculator) to C# and to the [Uno Platform](https://platform.uno) for iOS, Android and WebAssembly.

The app provides standard, scientific, and programmer calculator functionality, as well as a set of converters between various units of measurement and currencies.

The Uno Calculator will regularly follow with the Windows Calculator updates. You can get it from the [App Store](https://bit.ly/calc-ios), [Play Store](https://play.google.com/store/apps/details?id=uno.platform.calculator), [in your browser](https://calculator.platform.uno), and of course the original calculator on the [Microsoft Store](https://www.microsoft.com/store/apps/9WZDNCRFHVN5).

[![Build Status](https://uno-platform.visualstudio.com/Uno%20Platform/_apis/build/status/Uno%20Platform/Calculator%20CI?branchName=uno)](https://uno-platform.visualstudio.com/Uno%20Platform/_build?definitionId=55&_a=summary)

  ![Calculator Screenshot](docs/Images/CalculatorScreenshot.png)

## Features
- Standard Calculator functionality which offers basic operations and evaluates commands immediately as they are entered.
- Scientific Calculator functionality which offers expanded operations and evaluates commands using order of operations.
- Programmer Calculator functionality which offers common mathematical operations for developers including conversion between common bases.
- Date Calculation functionality which offers the difference between two dates, as well as the ability to add/subtract years, months and/or days to/from a given input date.
- Calculation history and memory capabilities.
- Conversion between many units of measurement.
- Currency conversion based on data retrieved from [Bing](https://www.bing.com).
- [Infinite precision](https://en.wikipedia.org/wiki/Arbitrary-precision_arithmetic) for basic
  arithmetic operations (addition, subtraction, multiplication, division) so that calculations
  never lose precision.

## Getting started
Prerequisites:
- Your computer must be running Windows 10, version 1803 or newer.
- Install the latest version of [Visual Studio](https://developer.microsoft.com/en-us/windows/downloads).
  - Install the "Universal Windows Platform Development" workload.
  - Install the optional "C++ Universal Windows Platform tools" component.
  - Install the Xamarin Development component
  - Install the optional "Mobile C++ development tools" component.
  - Install the latest Windows 10 SDK.

> When using Visual Studio 2019, the `.vsconfig` feature will automatically prompt to install missing components.

- Get the code:
    ```
    git clone https://github.com/nventive/calculator
    ```

- Open [src\Calculator.sln](/src/Calculator.sln) in Visual Studio to build and run the Calculator app.

## Contributing
Want to contribute? The team encourages community feedback and contributions. Please follow our [contributing guidelines](CONTRIBUTING.md).

If Calculator is not working properly, please [file an issue](https://github.com/nventive/calculator/issues).

## Data / Telemetry
This project collects usage data and sends it to App Center to help improve the quality of the calculator.

## Reporting Security Issues
Security issues and bugs should be reported through the [GitHub security tab](https://github.com/nventive/calculator/security).
