# Calculator
The Windows Calculator app is a modern Windows app written in C++ and C# that ships pre-installed with Windows.
The app provides standard, scientific, and programmer calculator functionality, as well as a set of converters between various units of measurement and currencies.

Calculator ships regularly with new features and bug fixes. You can get the latest version of Calculator in the [Microsoft Store](https://www.microsoft.com/store/apps/9WZDNCRFHVN5).

[![Build Status](https://dev.azure.com/ms/calculator/_apis/build/status/Calculator-CI?branchName=main)](https://dev.azure.com/ms/calculator/_build/latest?definitionId=57&branchName=main)

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
- Your computer must be running Windows 11, build 22000 or newer.
- Install the latest version of [Visual Studio](https://developer.microsoft.com/en-us/windows/downloads) (the free community edition is sufficient).
  - Install the "Universal Windows Platform Development" workload.
  - Install the optional "C++ Universal Windows Platform tools" component.
  - Install the latest Windows 11 SDK.

  ![Visual Studio Installation Screenshot](docs/Images/VSInstallationScreenshot.png)
- Install the [XAML Styler](https://marketplace.visualstudio.com/items?itemName=TeamXavalon.XAMLStyler) Visual Studio extension.

- Get the code:
    ```
    git clone https://github.com/Microsoft/calculator.git
    ```

- Open [src\Calculator.sln](/src/Calculator.sln) in Visual Studio to build and run the Calculator app.
- For a general description of the Calculator project architecture see [ApplicationArchitecture.md](docs/ApplicationArchitecture.md).
- To run the UI Tests, you need to make sure that
  [Windows Application Driver (WinAppDriver)](https://github.com/microsoft/WinAppDriver/releases/latest)
  is installed.

## Contributing
Want to contribute? The team encourages community feedback and contributions. Please follow our [contributing guidelines](CONTRIBUTING.md).

If Calculator is not working properly, please file a report in the [Feedback Hub](https://insider.windows.com/en-us/fb/?contextid=130).
We also welcome [issues submitted on GitHub](https://github.com/Microsoft/calculator/issues).

## Roadmap
For information regarding Windows Calculator plans and release schedule, please see the [Windows Calculator Roadmap](docs/Roadmap.md).

### Graphing Mode
Adding graphing calculator functionality [is on the project roadmap](https://github.com/Microsoft/calculator/issues/338) and we hope that this project can create a great end-user experience around graphing. To that end, the UI from the official in-box Windows Calculator is currently part of this repository, although the proprietary Microsoft-built graphing engine, which also drives graphing in Microsoft Mathematics and OneNote, is not. Community members can still be involved in the creation of the UI, however developer builds will not have graphing functionality due to the use of a [mock implementation of the engine](/src/GraphingImpl/Mocks) built on top of a
[common graphing API](/src/GraphingInterfaces).

## Diagnostic Data
This project collects usage data and sends it to Microsoft to help improve our products and services.
Read our [privacy statement](https://go.microsoft.com/fwlink/?LinkId=521839) to learn more.
Diagnostic data is disabled in development builds by default, and can be enabled with the `SEND_DIAGNOSTICS`
build flag.

## Currency Converter
Windows Calculator includes a currency converter feature that uses mock data in developer builds. The data that
Microsoft uses for the currency converter feature (e.g., in the retail version of the application) is not licensed
for your use. The mock data will be clearly identifiable as it references planets instead of countries,
and remains static regardless of selected inputs.

## Hotkeys
With the following list of hotkeys, you will be quickly convert from different modes and will be able to fully take advantage of
the powerful features offered by the Windows Calculator App.

| Key   | Functionality   |
|---|---|
| Alt + 1  | Switch to Standard Mode   
| Alt + 2  | Switch to Scientific Mode 
| Alt + 3	 | Switch to Graphing mode   
| Alt + 4	 | Switch to Programmer mode
| Alt + 5	 | Switch to Date Calculation mode
| Ctrl + M | Store in memory, in Standard mode, Scientific mode, and Programmer mode
| Ctrl + P| 	Add to memory, in Standard mode, Scientific mode, and Programmer mode
| Ctrl + Q	| Subtract from memory, in Standard mode, Scientific mode, and Programmer mode
| Ctrl + R	| Recall from memory, in Standard mode, Scientific mode, and Programmer mode
| Ctrl + L	| Clear memory
| Delete	| Clear current input (select CE)
| Esc	| Fully clear input (select C)
| Tab	| Navigate to the next UI item and give it focus
| Spacebar	| Selects UI item that has focus
| Enter	| Selects = in Standard mode, Scientific mode, and Programmer mode
| F9	| Select +/- in Standard mode, Scientific mode, and Programmer mode
| R	| Select 1/x in Standard mode and Scientific mode
| @	| Select 2√x in Standard mode and Scientific mode
| %	| Select % in Standard mode and Programmer mode
| Ctrl + H| 	When history button is visible, selects the history button in Standard mode and Scientific mode
| Up arrow	| Move up in history list, memory list, and menu items
| Down arrow	| Move down in history list, memory list, and menu items
| Ctrl + Shift + D	| Clear history
| F3	| Select DEG in Scientific mode
| F4	| Select RAD in Scientific mode
| F5	| Select GRAD in Scientific mode
| G	| Select 2<sup>x</sup> in Scientific mode
| Ctrl + G	| Select 10<sup>x</sup> in Scientific mode
| Shift + S	| Select sin<sup>-1</sup> in Scientific mode
| Ctrl + S	| Select sinh in Scientific mode
| Ctrl + Shift + S	| Select sinh<sup>-1</sup> in Scientific mode
| T	| Select tan in Scientific mode
| Shift + T| 	Select tan<sup>-1</sup> in Scientific mode
| Ctrl + T| 	Select tanh in Scientific mode
| Ctrl + Shift + T| 	Select tanh<sup>-1</sup> in Scientific mode
| O| 	Select cos in Scientific mode
| Shift + O| 	Select cos<sup>-1</sup> in Scientific mode
| Ctrl + O| 	Select cosh in Scientific mode
| Ctrl + Shift + O	| Select cosh<sup>-1</sup> in Scientific mode
| U	| Select sec in Scientific mode
| Shift + U	| Select sec<sup>-1</sup> in Scientific mode
| Ctrl + U	| Select sech in Scientific mode
| Ctrl + Shift + U| 	Select sech<sup>-1</sup> in Scientific mode
| I	| Select csc in Scientific mode
| Shift + I	| Select csc<sup>-1</sup> in Scientific mode
| Ctrl + I	| Select csch in Scientific mode
| Ctrl + Shift + I| 	Select csch<sup>-1</sup> in Scientific mode
| J	| Select cot in Scientific mode
| Shift + J	| Select cot<sup>-1</sup> in Scientific mode
| Ctrl + J	| Select coth in Scientific mode
| Ctrl + Shift + J| 	Select coth<sup>-1</sup> in Scientific mode
| Ctrl + Y| Select <sup>y</sup>√x in Scientific mode
| Shift + \	| Select |x| in Scientific mode
| [	| Select ⌊x⌋ in Scientific mode
| ]	| Select ⌈x⌉ in Scientific mode
| L	| Select log in Scientific mode
| Shift + L	| Select log<sub>y</sub><sup>x</sup> in Scientific mode
| M	| Select dms in Scientific mode
| N	| Select ln in Scientific mode
| Ctrl + N| Select e<sup>x</sup> in Scientific mode
| P	| Select Pi in Scientific mode
| Q	| Select x<sup>2</sup> in Standard mode and Scientific mode
| V	| Toggles on/off F-E button in Scientific mode
| X	| Select exp in Scientific mode
| Y, ^| 	Select x<sup>y</sup> in Scientific mode
| #	| Select x<sup>3</sup> in Scientific mode
| !	| Select n! in Scientific mode
| %	| Select mod in Scientific mode
| Ctrl ++ on numpad| 	Graph zooms in while in Graphing mode
| Ctrl +- on numpad	| Graph zooms out while in Graphing mode
| F5	| Select HEX in Programmer mode
| F6	| Select DEC in Programmer mode
| F7	| Select OCT in Programmer mode
| A-F	| Select letters A-F in Programmer mode while HEX is selected
| Shift + ,	| Select RoL in Programmer mode when Bit Shift is set to either "Circular" shift
| Shift + .	| Select RoR in Programmer mode when Bit Shift is set to either "Circular" shift
| Shift + ,	| Select Lsh in Programmer mode when Bit Shift is set to "Arithmetic" or "Logical" shift
| Shift + .	| Select Rsh in Programmer mode when Bit Shift is set to "Arithmetic" or "Logical" shift
| %	Select % | in Programmer mode

## Reporting Security Issues
Please refer to [SECURITY.md](./SECURITY.md).

## License
Copyright (c) Microsoft Corporation. All rights reserved.

Licensed under the [MIT License](./LICENSE).
