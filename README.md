# Calculator
The Windows Calculator app is a modern Windows app written in C++ that ships pre-installed with Windows.
The app provides standard, scientific, and programmer calculator functionality, as well as a set of converters between various units of measurement and currencies.

Calculator ships regularly with new features and bug fixes. You can get the latest version of Calculator in the [Windows Store.](https://www.microsoft.com/store/apps/9WZDNCRFHVN5)

<!-- TODO: Replace with badge from public pipeline -->
[![Build Status](https://microsoft.visualstudio.com/Apps/_apis/build/status/Utility%20Apps/Calculator-Daily)](https://microsoft.visualstudio.com/Apps/_build?definitionId=3539)

  ![Calculator Screenshot](docs/Images/CalculatorScreenshot.png)

## Features
- Standard Calculator functionality which offers basic operations and evaluates commands immediately as they are entered.
- Scientific Calculator functionality which offers expanded operations and evaluates commands using order of operations.
- Programmer Calculator functionality which offers common mathematical operations for developers including conversion between common bases.
- Calculation history and memory capabilities.
- Conversion between many units of measurement.
- Currency conversion based on data retrieved from [Bing](https://www.bing.com).

## Getting started
Prerequisites:
- Your computer must be running Windows 10, version 1803 or newer
- Install the latest version of [Visual Studio](https://developer.microsoft.com/en-us/windows/downloads) (the free community edition is sufficient)
  - Install the "Universal Windows Platform Development" workload
  - Install the optional "C++ Universal Windows Platform tools" component
  - Install the latest Windows 10 SDK

  ![Visual Studio Installation Screenshot](docs/Images/VSInstallationScreenshot.png)
- Install the [XAML Styler](https://marketplace.visualstudio.com/items?itemName=TeamXavalon.XAMLStyler) Visual Studio extension

- Get the code:
    ```
    git clone https://github.com/Microsoft/calculator.git
    ```

- Open [src\Calculator.sln](/src/Calculator.sln) in Visual Studio to build and run the Calculator app.
- For a general description of the Calculator project architecture see [ApplicationArchitecture.md](docs/ApplicationArchitecture.md).

## Contributing
Want to contribute? The team encourages community feedback and contributions. Please follow our [contributing guidelines](CONTRIBUTING.md).

If Calculator is not working properly, please file a report in the [Feedback Hub](https://insider.windows.com/en-us/fb/?contextid=130).
We also welcome [issues submitted on GitHub](https://github.com/Microsoft/calculator/issues).
