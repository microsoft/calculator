# Calculator
The Windows Calculator app is a modern Windows app written in C++ that ships pre-installed with Windows.
The app provides standard, scientific, and programmer calculator functionality, as well as a set of converters between various units of measurement and currencies.

Calculator ships regularly with new features and bug fixes. You can get the latest version of Calculator in the [Windows Store.](https://www.microsoft.com/store/apps/9WZDNCRFHVN5)

<!-- TODO: Replace with something else or remove if builds are not public -->
[![Build Status](https://microsoft.visualstudio.com/Apps/_apis/build/status/Utility%20Apps/Calculator-Daily)](https://microsoft.visualstudio.com/Apps/_build?definitionId=3539)

  ![Calculator Screenshot](\docs\Images\CalculatorScreenshot.png)

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

  ![Visual Studio Installation Screenshot](\docs\Images\VSInstallationScreenshot.png)
- Install the [XamlStyler](https://marketplace.visualstudio.com/items?itemName=TeamXavalon.XAMLStyler) Visual Studio extension

<!-- TODO: Replace with external link -->
- Get the code:
    ```
    git clone https://microsoft.visualstudio.com/DefaultCollection/Apps/_git/calculator.app
    ```

- Open [src\Calculator.sln](\src\Calculator.sln) in Visual Studio to build and run the Calculator app.
- For a general description of the Calculator project architecture see [ApplicationArchitecture.md](docs\ApplicationArchitecture.md).

## Running Tests
To run tests in Visual Studio, install [TDP](https://osgwiki.com/wiki/TDP) <!-- TODO 19570630: Replace with internal docs or remove once we know what we are doing with tests --> and use the TAEF explorer pane.

Calculator has two primary sets of tests:
- [CalculatorUnitTests.vcxproj](\src\CalculatorUnitTests\CalculatorUnitTests.vcxproj) - Unit Tests
- [UIAutomationTests.csproj](\src\UIAutomationTests\UIAutomationTests.csproj) - UI Automation using [MitaLite](http://osgwiki.com/mitalite) <!-- TODO: Replace with link to the MitaLite open source repository -->

## Exploring the repo
- [Build](\build) - Scripts which run during the build
- [Docs](\Docs) - Documentation for developers
- [Loc](\Loc) - Localization settings and files
- [PDP](\PDP) - Information about the app for the Store's Product Description Page
- [SpkgDefs](\SpkgDefs) - Definitions for building [test packages](https://osgwiki.com/wiki/Test_Package) <!-- TODO: Replace with external link or remove once we know what we are doing with tests -->
- [Src](\src) - Main folder for source code
- [Tools](\tools) - Scripts and tools to aid in development
- [TRexDefs](\TRexDefs) - Definitions for running tests using [TReX](https://osgwiki.com/wiki/TReX) <!-- TODO: Replace with external link or remove once we know what we are doing with tests -->

## Contributing
Want to contribute? The team encourages community feedback and contributions. Please follow our [contributing guidelines](\CONTRIBUTING.md).

If Calculator is not working properly, please file a report in the [Feedback Hub](https://insider.windows.com/en-us/fb/?contextid=130).
If you want to submit an issue to this repository, please read the
[issue reporting guidelines](\IssueTracking.md).

<!-- Uncomment when we finalize licensing
# License
The Calculator project is released under the *Insert license info and link here*
-->

<!-- TODO: Remove or replace with the decided on external mechanism to contact the team -->
## Contact
Questions? Reach out to the PAX Essential Experiences App email alias: <paxeeapps@microsoft.com>