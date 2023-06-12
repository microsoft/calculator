# Application Architecture

Windows Calculator is a [C++/CX][C++/CX] application, built for the Universal Windows Platform ([UWP][What is UWP?]).
Calculator uses the [XAML][XAML Overview] UI framework, and the project follows the Model-View-ViewModel ([MVVM][MVVM])
design pattern. This document discusses each of the layers and how they are related to the three Visual Studio projects
that build into the final Calculator application.

--------------------
## Table of Contents

* [View](#view)
    * [VisualStates](#visualstates)
    * [Data-Binding](#data-binding)
* [ViewModel](#viewmodel)
    * [PropertyChanged Events](#propertychanged-events)
* [Model](#model)

--------------------

## View

The View layer is contained in the [Calculator project][Calculator folder]. This project contains mostly XAML files
and various custom controls that support the UI. [App.xaml][App.xaml] contains many of the [static][StaticResource] and
[theme][ThemeResource] resources that the other XAML files will reference. Its code-behind file, [App.xaml.cs][App.xaml.cs],
contains the main entry point to the application. On startup, it navigates to the main page.

```C#
rootFrame.Navigate(typeof(MainPage), argument)
```

In Calculator, there is only one concrete [Page][Page] class: [MainPage.xaml][MainPage.xaml]. `MainPage` is the root
container for all the other application UI elements.  As you can see, there's not much content. `MainPage` uses a
`NavigationView` control to display the toggleable navigation menu, and empty containers for delay-loaded UI elements.
Of the many modes that Calculator shows in its menu, there are actually only three XAML files that `MainPage` needs to
manage in order to support all modes. They are:

* [Calculator.xaml][Calculator.xaml]: This [UserControl] is itself a container for the [Standard][CalculatorStandardOperators.xaml],
  [Scientific][CalculatorScientificOperators.xaml], and [Programmer][CalculatorProgrammerOperators.xaml] modes.
* [DateCalculator.xaml][DateCalculator.xaml]: Everything needed for the DateCalculator mode.
* [UnitConverter.xaml][UnitConverter.xaml]: One `UserControl` to support every Converter mode.

### VisualStates

[VisualStates][VisualState] are used to change the size, position, and appearance ([Style][Style]) of UI elements
in order to create an adaptive, responsive UI. A transition to a new `VisualState` is often triggered by specific
window sizes. Here are a few important examples of `VisualStates` in Calculator. Note that it is not a
complete list. When making UI changes, make sure you are considering the various `VisualStates` and layouts that
Calculator defines.

#### History/Memory Dock Panel expansion

In the Standard, Scientific, and Programmer modes, the History/Memory panel is exposed as a flyout in small window sizes.
Once the window is resized to have enough space, the panel becomes docked along the edge of the window.

<img src="Images\VisualStates\Standard1.gif" height="400" />

#### Scientific mode, inverse function button presence

In the Scientific mode, for small window sizes there is not enough room to show all the function buttons. The mode
hides some of the buttons and provides a Shift (↑) button to toggle the visibility of the collapsed rows. When the
window size is large enough, the buttons are re-arranged to display all function buttons at the same time.

<img src="Images\VisualStates\Scientific1.gif" height="400" />

#### Unit Converter aspect ratio adjustment

In the Unit Converter mode, the converter inputs and the numberpad will re-arrange depending on if the window is in
a Portrait or Landscape aspect ratio.

<img src="Images\VisualStates\Converter1.gif" height="400" />

### Data-Binding

Calculator uses [data binding][Data Binding] to dynamically update the properties of UI elements. If this concept
is new for you, it's also worth reading about [data binding in depth][Data binding in depth].

The [x:Bind][x:Bind] markup extension is a newer replacement for the older [Binding][Binding] style. You may see both
styles in the Calculator codebase. Prefer `x:Bind` in new contributions because it has better performance. If you need
to add or modify an existing `Binding`, updating to `x:Bind` is a great first step. Make sure to read and understand
the difference between the two styles, as there are some subtle behavioral changes. Refer to the
[binding feature comparison][BindingComparison] to learn more.

------------
## ViewModel

The ViewModel layer is contained in the [CalcViewModel][CalcViewModel folder] project. ViewModels provide a source of
data for the UI to bind against and act as the intermediary separating pure business logic from UI components that
should not care about the model's implementation. Just as the View layer consists of a hierarchy of XAML files, the
ViewModel consists of a hierarchy of ViewModel files. The relationship between XAML and ViewModel files is often 1:1.
Here are the notable ViewModel files to start exploring with:

* [ApplicationViewModel.h][ApplicationViewModel.h]: The ViewModel for [MainPage.xaml][MainPage.xaml]. This ViewModel
  is the root of the other mode-specific ViewModels. The application changes between modes by updating the `Mode` property
  of the `ApplicationViewModel`. The ViewModel will make sure the appropriate ViewModel for the new mode is initialized.
* [StandardCalculatorViewModel.h][StandardCalculatorViewModel.h]: The ViewModel for [Calculator.xaml][Calculator.xaml].
  This ViewModel exposes functionality for the main three Calculator modes: Standard, Scientific, and Programmer.
* [DateCalculatorViewModel.h][DateCalculatorViewModel.h]: The ViewModel for [DateCalculator.xaml][DateCalculator.xaml].
* [UnitConverterViewModel.h][UnitConverterViewModel.h]: The ViewModel for [UnitConverter.xaml][UnitConverter.xaml].
  This ViewModel implements the logic to support every converter mode, including Currency Converter.

### PropertyChanged Events

In order for [data binding](#data-binding) to work, ViewModels need a way to inform the XAML framework about
updates to their member properties. Most ViewModels in the project do so by implementing the
[INotifyPropertyChanged][INotifyPropertyChanged] interface. The interface requires that the class provides a
[PropertyChanged event][PropertyChanged]. Clients of the ViewModel (such as the UI), can register for the
`PropertyChanged` event from the ViewModel, then re-evaluate bindings or handle the event in code-behind when the
ViewModel decides to raise the event. ViewModels in the Calculator codebase generally uses a macro, defined in the
[Utils.h][Utils.h] utility file, to implement the `INotifyPropertyChanged` interface. Here is a standard
implementation, taken from [ApplicationViewModel.h][ApplicationViewModel.h].

```C++
[Windows::UI::Xaml::Data::Bindable]
public ref class ApplicationViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
{
public:
    ApplicationViewModel();

    OBSERVABLE_OBJECT();
```

The `OBSERVABLE_OBJECT()` macro defines the required `PropertyChanged` event. It also defines a private
`RaisePropertyChanged` helper function for the class. The function takes a property name and raises a
`PropertyChanged` event for that property.

Properties that are intended to be the source for a data binding are also typically implemented with a macro. Here is
one such property from `ApplicationViewModel`:

```C++
OBSERVABLE_PROPERTY_RW(Platform::String^, CategoryName);
```

The `OBSERVABLE_PROPERTY_RW` macro defines a Read/Write property that will raise a `PropertyChanged` event if its value
changes. Read/Write means the property exposes both a public getter and setter. For efficiency and to avoid raising
unnecessary `PropertyChanged` events, the setter for these types of properties will check if the new value is
different from the previous value before raising the event.

From this example, either `ApplicationViewModel` or clients of the class can simply assign to the `CategoryName`
property and a `PropertyChanged` event will be raised, allowing the UI to respond to the new `CategoryName` value.

--------
## Model

The Model for the Calculator modes is contained in the [CalcManager][CalcManager folder] project. It consists of three layers: a `CalculatorManager`, which relies on a `CalcEngine`, which relies on the `Ratpack`.

### CalculatorManager

The CalculatorManager contains the logic for managing the overall Calculator app's data such as the History and Memory lists, as well as maintaining the instances of calculator engines used for the various modes. The interface to this layer is defined in [CalculatorManager.h][CalculatorManager.h].

### CalcEngine

The CalcEngine contains the logic for interpreting and performing operations according to the commands passed to it. It maintains the current state of calculations and relies on the RatPack for performing mathematical operations. The interface to this layer is defined in [CalcEngine.h][CalcEngine.h].

### RatPack

The RatPack (short for Rational Pack) is the core of the Calculator model and contains the logic for
performing its mathematical operations (using [infinite precision][Infinite Precision] arithmetic
instead of regular floating point arithmetic). The interface to this layer is defined in [ratpak.h][ratpak.h].

[References]:####################################################################################################

[C++/CX]:                             https://docs.microsoft.com/en-us/cpp/cppcx/visual-c-language-reference-c-cx
[What is UWP?]:                       https://docs.microsoft.com/en-us/windows/uwp/get-started/universal-application-platform-guide
[XAML Overview]:                      https://docs.microsoft.com/en-us/windows/uwp/xaml-platform/xaml-overview
[MVVM]:                               https://docs.microsoft.com/en-us/windows/uwp/data-binding/data-binding-and-mvvm

[Calculator folder]:                  ../src/Calculator
[App.xaml]:                           ../src/Calculator/App.xaml
[App.xaml.cs]:                       ../src/Calculator/App.xaml.cs
[StaticResource]:                     https://docs.microsoft.com/en-us/windows/uwp/xaml-platform/staticresource-markup-extension
[ThemeResource]:                      https://docs.microsoft.com/en-us/windows/uwp/xaml-platform/themeresource-markup-extension
[Page]:                               https://docs.microsoft.com/en-us/uwp/api/Windows.UI.Xaml.Controls.Page
[UserControl]:                        https://docs.microsoft.com/en-us/uwp/api/Windows.UI.Xaml.Controls.UserControl
[MainPage.xaml]:                      ../src/Calculator/Views/MainPage.xaml
[Calculator.xaml]:                    ../src/Calculator/Views/Calculator.xaml
[CalculatorStandardOperators.xaml]:   ../src/Calculator/Views/CalculatorStandardOperators.xaml
[CalculatorScientificOperators.xaml]: ../src/Calculator/Views/CalculatorScientificOperators.xaml
[CalculatorProgrammerOperators.xaml]: ../src/Calculator/Views/CalculatorProgrammerOperators.xaml
[DateCalculator.xaml]:                ../src/Calculator/Views/DateCalculator.xaml
[UnitConverter.xaml]:                 ../src/Calculator/Views/UnitConverter.xaml

[VisualState]:                        https://docs.microsoft.com/en-us/windows/uwp/design/layout/layouts-with-xaml#adaptive-layouts-with-visual-states-and-state-triggers
[Style]:                              https://docs.microsoft.com/en-us/windows/uwp/design/controls-and-patterns/xaml-styles

[Data Binding]:                       https://docs.microsoft.com/en-us/windows/uwp/data-binding/data-binding-quickstart
[Data binding in depth]:              https://docs.microsoft.com/en-us/windows/uwp/data-binding/data-binding-in-depth
[x:Bind]:                             https://docs.microsoft.com/en-us/windows/uwp/xaml-platform/x-bind-markup-extension
[Binding]:                            https://docs.microsoft.com/en-us/windows/uwp/xaml-platform/binding-markup-extension
[BindingComparison]:                  https://docs.microsoft.com/en-us/windows/uwp/data-binding/data-binding-in-depth#xbind-and-binding-feature-comparison

[CalcViewModel folder]:               ../src/CalcViewModel
[ApplicationViewModel.h]:             ../src/CalcViewModel/ApplicationViewModel.h
[StandardCalculatorViewModel.h]:      ../src/CalcViewModel/StandardCalculatorViewModel.h
[DateCalculatorViewModel.h]:          ../src/CalcViewModel/DateCalculatorViewModel.h
[UnitConverterViewModel.h]:           ../src/CalcViewModel/UnitConverterViewModel.h

[INotifyPropertyChanged]:             https://docs.microsoft.com/en-us/uwp/api/windows.ui.xaml.data.inotifypropertychanged
[PropertyChanged]:                    https://docs.microsoft.com/en-us/uwp/api/windows.ui.xaml.data.inotifypropertychanged.propertychanged
[Utils.h]:                            ../src/CalcViewModel/Common/Utils.h

[CalcManager folder]:                 ../src/CalcManager
[CalculatorManager.h]:                ../src/CalcManager/CalculatorManager.h
[CalcEngine.h]:                       ../src/CalcManager/Header&#32;Files/CalcEngine.h
[Infinite Precision]:                 https://en.wikipedia.org/wiki/Arbitrary-precision_arithmetic
[ratpak.h]:                           ../src/CalcManager/Ratpack/ratpak.h
