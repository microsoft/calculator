# Reference Golden Vectors — Microsoft's Windows Calculator (`microsoft/calculator`)

Machine-extracted, **verbatim** reproduction of the reference repository's *own* test suites,
intended to be used as golden vectors for auditing a Linux port.

## 0. Provenance

| Item | Value |
|---|---|
| Reference repo | `/home/elshad-guliyev/ref-calculator` |
| Commit | `d125246a4e19842ce1332e6c7839cf0e110027d8` |
| Commit date | `2026-09-28 13:48:24 -0700` |
| Extraction method | `python3` + `grep`, brace-matched, **no paraphrasing** — every test body below is a byte-for-byte copy of the reference source |
| Test frameworks present | C++ `Microsoft::VisualStudio::CppUnitTestFramework` (native UWP unit test project) and C# `MSTest` 2.2.10 (`MSTest.TestAdapter`/`MSTest.TestFramework`) for unit tests; `MSTest` 4.2.2 + `Appium.WebDriver` 4.4.5 for UI tests |
| Referenced UIElements | All UI tests drive the real WinUI 3 app via WinAppDriver/Appium |

### How to use this document

* Section **3** is the C++ engine test suite (`src/CalculatorUnitTests/`), grouped by `TEST_CLASS`.
* Section **4** is the C# unit test suite (`src/Calculator.Tests/`), grouped by `[TestClass]`.
* Section **5** is a cross-cutting index that re-prints the individual test bodies grouped by *vector category*
  (date maths, scientific/precision, number formatting, unit conversion, memory/history, max-digits).
  These are duplicated from sections 3–4 **on purpose**, so a vector category can be lifted out without
  having to read the whole suite.
* Line numbers in headings are 1-based and refer to the original reference files.
* Escapes such as `\x221A` (√), `\x00D7` (×), `\x00F7` (÷) are present **as written in the reference**;
  they are C# `\uXXXX` / C++ universal-character-name style escapes used inside string literals.

---

## 1. Test project inventory

### 1.1 What exists

| # | Path | Exists? | Kind | Framework | Test-bearing sources |
|---|---|---|---|---|---|
| 1 | `src/CalculatorUnitTests/CalculatorUnitTests.vcxproj` | **YES** | Native C++/WinRT UWP unit-test application (`ProjectSubType=NativeUnitTestProject`) targeting the `CalcManager` engine | `CppUnitTestFramework.Universal` 15.0 | 86 `TEST_METHOD`s across 5 .cpp files |
| 2 | `src/Calculator.Tests/Calculator.Tests.csproj` | **YES** | C# UWP unit-test app against `Calculator.ViewModels` + `CalcManager.Interop` + `GraphControl` | MSTest 2.2.10 | 288 `[TestMethod]`/`[DataTestMethod]` across 16 .cs files |
| 3 | `src/CalculatorUITests/CalculatorUITests.csproj` | **YES** | .NET 10 UI functional-test project (WinAppDriver / Appium) | MSTest 4.2.2 + Appium.WebDriver 4.4.5 | 160 `[TestMethod]`s across 6 .cs files |
| 4 | `src/CalculatorUITestFramework/CalculatorUITestFramework.csproj` | **YES** | *Support library* for the UI tests (page objects: `StandardCalculatorPage`, `ScientificCalculatorPage`, `ProgrammerCalculatorPage`, `UnitConverterPage`, `CalculatorResults`, `HistoryPanel`, `MemoryPanel`, …). **No test methods.** | — | 0 |

### 1.2 What does NOT exist

| Path probed | Exists? | Note |
|---|---|---|
| `src/CalculatorUnitTests/` as a **C#** unit-test project | **NO** | Despite the name, `src/CalculatorUnitTests/` is a **native C++/WinRT** project (`.vcxproj`, `CppUnitTestFramework`). It contains **zero `.cs` files**. |
| A C++ test project for `CalcManager` under a different name (e.g. `CalcManagerTests`, `CalcManager.Test`, `*Test*.vcxproj` separate from `CalculatorUnitTests`) | **NO** | `find src -name '*.vcxproj'` returns exactly 6 projects; the only test `.vcxproj` is `CalculatorUnitTests.vcxproj`. `src/CalcManager/` itself contains **no** test files (only production sources). |
| `src/CalcManager/CEngine/**` unit tests (parser/`Command`/`RadPack` tests inside the engine folder) | **NO** | `src/CalcManager/CEngine/` has no `*test*` files. |
| gtest / Catch2 / doctest / Boost.Test | **NO** | No C++ test framework other than the MSVC `CppUnitTestFramework` is referenced. |
| xUnit / NUnit | **NO** | Only MSTest. |
| Property-based / fuzz / randomized testing | **NO** | No hypothesis/FuzzIt/QuickCheck anywhere. All expectations are hand-written literals. |
| Golden-file / approval-test baselines for engine output | **NO** | `Snapshot*Tests.cs` test *JSON serialisation of view-model state*, not engine output text. |
| Tests for `CalcManager.Interop` or `GraphingImpl` or `GraphControl` in isolation | **NO** | Those libraries have no dedicated test project. |
| Date-calculation tests in the **C++ engine** project | **NO** | `DateCalculationEngine` is C# (`Calculator.ViewModels`); all date vectors live in `src/Calculator.Tests/DateCalculatorTests.cs`. |

### 1.3 Per-file test counts (unit tests)

| File | `TEST_METHOD` / `[TestMethod]` count |
|---|---|
| `src/CalculatorUnitTests/CalcEngineTests.cpp` | 4 |
| `src/CalculatorUnitTests/CalcInputTest.cpp` | 37 |
| `src/CalculatorUnitTests/CalculatorManagerTest.cpp` | 21 |
| `src/CalculatorUnitTests/RationalTest.cpp` | 8 |
| `src/CalculatorUnitTests/UnitConverterTest.cpp` | 16 |
| `src/Calculator.Tests/CopyPasteManagerTests.cs` | 23 |
| `src/Calculator.Tests/CurrencyConverterTests.cs` | 17 |
| `src/Calculator.Tests/DateCalculatorTests.cs` | 28 |
| `src/Calculator.Tests/GraphingViewModelTests.cs` | 2 |
| `src/Calculator.Tests/HistoryTests.cs` | 36 |
| `src/Calculator.Tests/KeystrokeLatencyTests.cs` | 1 |
| `src/Calculator.Tests/LocalizationSettingsTests.cs` | 8 |
| `src/Calculator.Tests/MultiWindowTests.cs` | 16 |
| `src/Calculator.Tests/NarratorAnnouncementTests.cs` | 19 |
| `src/Calculator.Tests/NavCategoryTests.cs` | 18 |
| `src/Calculator.Tests/SnapshotJsonTests.cs` | 13 |
| `src/Calculator.Tests/SnapshotRoundTripTests.cs` | 17 |
| `src/Calculator.Tests/StandardCalculatorViewModelTests.cs` | 50 |
| `src/Calculator.Tests/UnitConverterViewModelTests.cs` | 31 |
| `src/Calculator.Tests/UtilsTests.cs` | 6 |
| `src/Calculator.Tests/ViewModelLifetimeTests.cs` | 3 |
| **TOTAL (unit tests)** | **374** |

### 1.4 Per-file test counts (UI tests)

| File | `[TestMethod]` count |
|---|---|
| `src/CalculatorUITests/CurrencyConverterFunctionalTests.cs` | 8 |
| `src/CalculatorUITests/HistoryFunctionalTests.cs` | 4 |
| `src/CalculatorUITests/MemoryFunctionalTests.cs` | 5 |
| `src/CalculatorUITests/ProgrammerModeFunctionalTests.cs` | 60 |
| `src/CalculatorUITests/ScientificModeFunctionalTests.cs` | 45 |
| `src/CalculatorUITests/StandardModeFunctionalTests.cs` | 38 |
| **TOTAL (UI tests)** | **160** |

---

## 2. Shared test infrastructure (verbatim)

These files define the assertion macros, precision constants and fixture helpers that the
test bodies in sections 3–5 depend on. They are part of the golden-vector contract: e.g.
`StandardModePrecision = 16`, `ScientificModePrecision = 32`, `ProgrammerModePrecision = 64`
are the reference's own declared digit budgets.

### 2.1 `src/CalculatorUnitTests/Helpers.h` — assertion macros + precision constants

**`src/CalculatorUnitTests/Helpers.h`** — 99 lines

```cpp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#pragma once
#include <CppUnitTest.h>

namespace CalculatorUnitTests
{
#define StandardModePrecision 16
#define ScientificModePrecision 32
#define ProgrammerModePrecision 64

    namespace UtfUtils
    {
        constexpr wchar_t MUL = 0x00d7; // Multiplication Symbol
    }
}

#define VERIFY_THROWS_EXPECTEDEXCEPTION(__operation, __exception)                                                                                              \
    {                                                                                                                                                          \
        bool __exceptionHit = false;                                                                                                                           \
        try                                                                                                                                                    \
        {                                                                                                                                                      \
            __operation;                                                                                                                                       \
        }                                                                                                                                                      \
        catch (__exception)                                                                                                                                    \
        {                                                                                                                                                      \
            __exceptionHit = true;                                                                                                                             \
        }                                                                                                                                                      \
                                                                                                                                                               \
        if (!__exceptionHit)                                                                                                                                   \
        {                                                                                                                                                      \
            Assert::Fail(L"Expected exception was not caught");                                                                                                \
        }                                                                                                                                                      \
    }

#define VERIFY_IS_NULL(__operation, ...)                                                                                                                       \
    {                                                                                                                                                          \
        Assert::IsTrue(nullptr == __operation, __VA_ARGS__);                                                                                                   \
    }

#define VERIFY_IS_NOT_NULL(__operation, ...)                                                                                                                   \
    {                                                                                                                                                          \
        Assert::IsTrue(nullptr != __operation, __VA_ARGS__);                                                                                                   \
    }

#define VERIFY_IS_LESS_THAN(__expectedLess, __expectedGreater, ...)                                                                                            \
    {                                                                                                                                                          \
        Assert::IsTrue(__expectedLess < __expectedGreater, __VA_ARGS__);                                                                                       \
    }

#define VERIFY_IS_GREATER_THAN(__expectedGreater, __expectedLess, ...)                                                                                         \
    {                                                                                                                                                          \
        Assert::IsTrue(__expectedGreater > __expectedLess, __VA_ARGS__);                                                                                       \
    }

#define VERIFY_ARE_EQUAL(__f1, __f2, ...)                                                                                                                      \
    {                                                                                                                                                          \
        Assert::IsTrue(__f1 == __f2, __VA_ARGS__);                                                                                                             \
    }

#define VERIFY_ARE_NOT_EQUAL(__f1, __f2, ...)                                                                                                                  \
    {                                                                                                                                                          \
        Assert::IsTrue(__f1 != __f2, __VA_ARGS__);                                                                                                             \
    }

#define VERIFY_IS_TRUE(__operation, ...)                                                                                                                       \
    {                                                                                                                                                          \
        Assert::IsTrue(__operation, __VA_ARGS__);                                                                                                              \
    }

#define VERIFY_IS_FALSE(__operation, ...)                                                                                                                      \
    {                                                                                                                                                          \
        Assert::IsFalse(__operation, __VA_ARGS__);                                                                                                             \
    }

#define VERIFY_IS_LESS_THAN_OR_EQUAL(__expectedLess, __expectedGreater, ...)                                                                                   \
    {                                                                                                                                                          \
        Assert::IsTrue(__expectedLess <= __expectedGreater, __VA_ARGS__);                                                                                      \
    }

#define VERIFY_IS_GREATER_THAN_OR_EQUAL(__expectedGreater, __expectedLess, ...)                                                                                \
    {                                                                                                                                                          \
        Assert::IsTrue(__expectedGreater >= __expectedLess, __VA_ARGS__);                                                                                      \
    }

template <typename T>
void VERIFY_VECTORS_ARE_EQUAL(Windows::Foundation::Collections::IVector<T> ^ vecA, Windows::Foundation::Collections::IVector<T> ^ vecB, ...)
{
    if (vecA->Size != vecB->Size)
        Assert::Fail();

    for (unsigned int i = 0; i < vecA->Size; ++i)
    {
        VERIFY_ARE_EQUAL(vecA->GetAt(i), vecB->GetAt(i), __VA_ARGS__);
    }
};
```

### 2.2 `src/CalculatorUnitTests/pch.h` — precompiled header

**`src/CalculatorUnitTests/pch.h`** — 58 lines

```cpp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// pch.h
// Header for standard system include files.
//

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Windows headers define min/max macros.
// Disable it for project code.
#define NOMINMAX

#define UNIT_TESTS

#include <windows.h>
#include <collection.h>
#include <ppltasks.h>
#include <cassert>
#include <concrt.h>
#include <string>
#include <bitset>
#include <memory>
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <deque>
#include <regex>
#include <concurrent_vector.h>
#include <pplawait.h>
#include <unordered_map>
#include <mutex>
#include <locale>
#include <sal.h>
#include <sstream>

// C++\WinRT Headers
#include "winrt/base.h"
#include "winrt/Windows.Foundation.Diagnostics.h"
#include "winrt/Windows.Globalization.h"
#include "winrt/Windows.Globalization.DateTimeFormatting.h"
#include "winrt/Windows.System.UserProfile.h"

// CalcManager Headers
#include "CalcManager/ExpressionCommand.h"
#include "CalcManager/CalculatorResource.h"
#include "CalcManager/CalculatorManager.h"
#include "CalcManager/UnitConverter.h"

#include "Helpers.h"

#include "UnitTestApp.xaml.h"
```

### 2.3 `src/CalculatorUnitTests/EngineResourceProvider.h` / `.cpp` — engine string provider used by every C++ fixture

**`src/CalculatorUnitTests/EngineResourceProvider.h`** — 23 lines

```cpp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcManager/CalculatorResource.h"

namespace CalculatorApp::ViewModel::Common
{
    // Minimal engine resource provider for the native CalcManager unit tests.
    // Self-contained (no CalcViewModel dependency): engine strings come from the
    // CEngineStrings resources, and the number separators are read from the
    // current user locale.
    class EngineResourceProvider : public CalculationManager::IResourceProvider
    {
    public:
        EngineResourceProvider();
        std::wstring GetCEngineString(std::wstring_view id) override;

    private:
        Windows::ApplicationModel::Resources::ResourceLoader ^ m_resLoader;
    };
}
```

**`src/CalculatorUnitTests/EngineResourceProvider.cpp`** — 40 lines

```cpp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "EngineResourceProvider.h"

using namespace CalculatorApp::ViewModel::Common;
using namespace Platform;
using namespace Windows::ApplicationModel::Resources;
using namespace std;

EngineResourceProvider::EngineResourceProvider()
{
    m_resLoader = ResourceLoader::GetForViewIndependentUse("CEngineStrings");
}

wstring EngineResourceProvider::GetCEngineString(wstring_view id)
{
    // The unit tests force the en-US locale (see UnitTestApp), so the engine
    // number separators are fixed to their en-US values here.
    if (id.compare(L"sDecimal") == 0)
    {
        return L".";
    }

    if (id.compare(L"sThousand") == 0)
    {
        return L",";
    }

    if (id.compare(L"sGrouping") == 0)
    {
        // CalcEngine consumes the Win32 grouping format; "3;0" groups every 3 digits.
        return L"3;0";
    }

    StringReference idRef(id.data(), id.length());
    String ^ str = m_resLoader->GetString(idRef);
    return wstring(str->Data());
}
```

### 2.4 `src/CalculatorUnitTests/AsyncHelper.h` / `.cpp` — UI-thread marshalling helper

**`src/CalculatorUnitTests/AsyncHelper.h`** — 74 lines, UTF-8 BOM

```cpp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <wrl/wrappers/corewrappers.h>

namespace CalculatorApp
{
    class AsyncHelper
    {
    public:
        static concurrency::task<void> RunOnUIThreadAsync(std::function<void()>&& action);
        static void RunOnUIThread(std::function<void()>&& action, DWORD timeout = INFINITE);
        static void Delay(DWORD milliseconds);

        template <typename T>
        static void RunOnUIThread(std::function<concurrency::task<T>()>&& action, DWORD timeout = INFINITE)
        {
            concurrency::task<T> t;
            concurrency::task<void> uiTask =
                RunOnUIThreadAsync([&t, action]() { t = action(); }).then([&t]() { t.wait(); }, concurrency::task_continuation_context::use_arbitrary());

            WaitForTask<void>(uiTask, timeout);
        }

        template <typename T>
        static bool WaitForTask(concurrency::task<T>& t, DWORD timeout = INFINITE)
        {
            Microsoft::WRL::Wrappers::Event event(CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));
            if (!event.IsValid())
            {
                throw std::bad_alloc();
            }

            Platform::Exception ^ ex;
            t.then(
                [&event, &ex](concurrency::task<T> prevTask) {
                    try
                    {
                        prevTask.get();
                    }
                    catch (Platform::Exception ^ e)
                    {
                        ex = e;
                    }

                    if (event.IsValid())
                    {
                        SetEvent(event.Get());
                    }
                },
                concurrency::task_continuation_context::use_arbitrary());

            DWORD waitResult; // = STATUS_PENDING;
            waitResult = WaitForSingleObjectEx(event.Get(), timeout, true);
            event.Close();

            if (ex != nullptr)
            {
                throw ex;
            }

            if (waitResult == WAIT_FAILED)
            {
                throw ref new Platform::Exception(-1, L"Error in waiting for task completion: " + waitResult.ToString());
            }

            return waitResult == WAIT_OBJECT_0;
        }

    private:
        static void Sleep(DWORD milliseconds);
    };
}
```

**`src/CalculatorUnitTests/AsyncHelper.cpp`** — 39 lines, UTF-8 BOM

```cpp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "AsyncHelper.h"
#include <chrono>
#include <thread>

using namespace std;
using namespace concurrency;
using namespace Platform;
using namespace CalculatorApp;
using namespace Windows::UI::Core;
using namespace Windows::ApplicationModel::Core;

task<void> AsyncHelper::RunOnUIThreadAsync(function<void()>&& action)
{
    auto callback = ref new DispatchedHandler([action]() { action(); });

    return create_task(CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, callback));
}

void AsyncHelper::RunOnUIThread(function<void()>&& action, DWORD timeout)
{
    task<void> waitTask = RunOnUIThreadAsync([action]() { action(); });

    WaitForTask<void>(waitTask, timeout);
}

void AsyncHelper::Delay(DWORD milliseconds)
{
    thread timer(bind(CalculatorApp::AsyncHelper::Sleep, milliseconds));
    timer.join();
}

void AsyncHelper::Sleep(DWORD milliseconds)
{
    this_thread::sleep_for(chrono::milliseconds(milliseconds));
}
```

### 2.5 `src/CalculatorUnitTests/Module.cpp` — MSTest/native test-module bootstrap

**`src/CalculatorUnitTests/Module.cpp`** — 22 lines, UTF-8 BOM

```cpp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CalculatorUnitTests
{
    BEGIN_TEST_MODULE_ATTRIBUTE()
    TEST_MODULE_ATTRIBUTE(L"APPX:CertificateFileName", L"CalculatorUnitTests.cer:TrustedPeople")
    END_TEST_MODULE_ATTRIBUTE()

    TEST_MODULE_INITIALIZE(ModuleSetup)
    {
    }

    TEST_MODULE_CLEANUP(ModuleCleanup)
    {
    }
}
```

### 2.6 `src/CalculatorUnitTests/UnitTestApp.xaml.h` / `.xaml.cpp` — UWP unit-test host

**`src/CalculatorUnitTests/UnitTestApp.xaml.h`** — 29 lines, UTF-8 BOM

```cpp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// App.xaml.h
// Declaration of the App class.
//

#pragma once

#include "UnitTestApp.g.h"

namespace CalculatorUnitTests
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    ref class App sealed
    {
    protected:
        virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs ^ e) override;

        internal : App();

    private:
        void OnSuspending(Platform::Object ^ sender, Windows::ApplicationModel::SuspendingEventArgs ^ e);
        void OnNavigationFailed(Platform::Object ^ sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^ e);
    };
}
```

**`src/CalculatorUnitTests/UnitTestApp.xaml.cpp`** — 111 lines, UTF-8 BOM

```cpp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// App.xaml.cpp
// Implementation of the App class.
//

#include "pch.h"
#include "UnitTestApp.xaml.h"

using namespace CalculatorUnitTests;

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Application template is documented at https://go.microsoft.com/fwlink/?LinkId=402347&clcid=0x409

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
{
    InitializeComponent();
    Suspending += ref new SuspendingEventHandler(this, &App::OnSuspending);
}

/// <summary>
/// Invoked when the application is launched normally by the end user. Other entry points
/// will be used such as when the application is launched to open a specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs ^ e)
{
#if _DEBUG
    // Show graphics profiling information while debugging.
    if (IsDebuggerPresent())
    {
        // Display the current frame rate counters
        DebugSettings->EnableFrameRateCounter = true;
    }
#endif

    auto rootFrame = dynamic_cast<Frame ^>(Window::Current->Content);

    // Do not repeat app initialization when the Window already has content,
    // just ensure that the window is active
    if (rootFrame == nullptr)
    {
        // Create a Frame to act as the navigation context and associate it with
        // a SuspensionManager key
        rootFrame = ref new Frame();

        rootFrame->NavigationFailed += ref new Windows::UI::Xaml::Navigation::NavigationFailedEventHandler(this, &App::OnNavigationFailed);

        if (e->PreviousExecutionState == ApplicationExecutionState::Terminated)
        {
            // TODO: Restore the saved session state only when appropriate, scheduling the
            // final launch steps after the restore is complete
        }

        // Place the frame in the current Window
        Window::Current->Content = rootFrame;
    }

    Microsoft::VisualStudio::TestPlatform::TestExecutor::WinRTCore::UnitTestClient::CreateDefaultUI();

    Window::Current->Activate();

    // Override the current locale to use English (US) to be compatible with all tests based on formatting
    Windows::Globalization::ApplicationLanguages::PrimaryLanguageOverride = ref new Platform::String(L"en-US");

    Microsoft::VisualStudio::TestPlatform::TestExecutor::WinRTCore::UnitTestClient::Run(e->Arguments);
}

/// <summary>
/// Invoked when application execution is being suspended. Application state is saved
/// without knowing whether the application will be terminated or resumed with the contents
/// of memory still intact.
/// </summary>
/// <param name="sender">The source of the suspend request.</param>
/// <param name="e">Details about the suspend request.</param>
void App::OnSuspending(Object ^ sender, SuspendingEventArgs ^ e)
{
    (void)sender; // Unused parameter
    (void)e;      // Unused parameter

    // TODO: Save application state and stop any background activity
}

/// <summary>
/// Invoked when Navigation to a certain page fails
/// </summary>
/// <param name="sender">The Frame which failed navigation</param>
/// <param name="e">Details about the navigation failure</param>
void App::OnNavigationFailed(Platform::Object ^ sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^ e)
{
    throw ref new FailureException("Failed to load Page " + e->SourcePageType.Name);
}
```

### 2.7 `src/Calculator.Tests/UnitTestApp.xaml.cs` — UWP unit-test host (C#)

**`src/Calculator.Tests/UnitTestApp.xaml.cs`** — 24 lines

```csharp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestPlatform.TestExecutor;
using Windows.ApplicationModel.Activation;
using Windows.UI.Xaml;

namespace Calculator.Tests
{
    sealed partial class App : Application
    {
        public App()
        {
            this.InitializeComponent();
        }

        protected override void OnLaunched(LaunchActivatedEventArgs e)
        {
            UnitTestClient.CreateDefaultUI();
            Window.Current.Activate();
            UnitTestClient.Run(e.Arguments);
        }
    }
}
```

---

## 3. C++ engine unit tests — `src/CalculatorUnitTests/` (verbatim, grouped by `TEST_CLASS`)

Native WinRT/C++ unit tests against the **real `CalcManager` engine**
(`CCalcEngine`, `CalcInput`, `RationalMath`, `UnitConverter`, `NumberFormattingUtils`,
`CalculatorHistory`). 5 `TEST_CLASS`es, **86 `TEST_METHOD`s**.

Each subsection contains: an index table of the class's test methods with line ranges, the
fixture/helper code the class depends on, the `TEST_CLASS` block itself, and all out-of-line
`TEST_METHOD` definitions — all verbatim and non-overlapping.

### 3.1 `src/CalculatorUnitTests/CalcEngineTests.cpp` — `TEST_CLASS(CalcEngineTests)`

4 test methods.

| # | `TEST_METHOD` | Kind | Source lines |
|---|---|---|---|
| 1 | `TestGroupDigitsPerRadix` | inline in class | `CalcEngineTests.cpp:36-59` |
| 2 | `TestIsNumberInvalid` | inline in class | `CalcEngineTests.cpp:61-157` |
| 3 | `TestDigitGroupingStringToGroupingVector` | inline in class | `CalcEngineTests.cpp:159-186` |
| 4 | `TestGroupDigits` | inline in class | `CalcEngineTests.cpp:188-236` |

#### 3.1.0 File preamble, `using` directives and fixture helpers (`CalcEngineTests.cpp:1-18`)

```cpp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>

#include "EngineResourceProvider.h"

using namespace std;
using namespace CalculatorApp;
using namespace CalculatorApp::ViewModel::Common;
using namespace CalculationManager;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static constexpr size_t MAX_HISTORY_SIZE = 20;

namespace CalculatorEngineTests
{
```

#### 3.1.1 `TEST_CLASS` block verbatim (`CalcEngineTests.cpp:19-242`)

```cpp
    TEST_CLASS(CalcEngineTests)
    {
        TEST_METHOD_INITIALIZE(CommonSetup)
        {
            m_resourceProvider = make_shared<EngineResourceProvider>();
            m_history = make_shared<CalculatorHistory>(MAX_HISTORY_SIZE);
            CCalcEngine::InitialOneTimeOnlySetup(*(m_resourceProvider.get()));
            m_calcEngine = make_unique<CCalcEngine>(
                false /* Respect Order of Operations */, false /* Set to Integer Mode */, m_resourceProvider.get(), nullptr, m_history);
        }
        TEST_METHOD_CLEANUP(Cleanup)
        {
            m_resourceProvider = nullptr;
            m_history = nullptr;
            m_calcEngine = nullptr;
        }

        TEST_METHOD(TestGroupDigitsPerRadix)
        {
            // Empty/Error cases
            VERIFY_IS_TRUE(m_calcEngine->GroupDigitsPerRadix(L"", 10).empty(), L"Verify grouping empty string returns empty string.");
            VERIFY_ARE_EQUAL(L"12345678", m_calcEngine->GroupDigitsPerRadix(L"12345678", 9), L"Verify grouping on invalid base returns original string");

            // Octal
            VERIFY_ARE_EQUAL(L"1 234 567", m_calcEngine->GroupDigitsPerRadix(L"1234567", 8), L"Verify grouping in octal.");
            VERIFY_ARE_EQUAL(L"123", m_calcEngine->GroupDigitsPerRadix(L"123", 8), L"Verify minimum grouping in octal.");

            // Binary/Hexadecimal
            VERIFY_ARE_EQUAL(L"12 3456 7890", m_calcEngine->GroupDigitsPerRadix(L"1234567890", 2), L"Verify grouping in binary.");
            VERIFY_ARE_EQUAL(L"1234", m_calcEngine->GroupDigitsPerRadix(L"1234", 2), L"Verify minimum grouping in binary.");
            VERIFY_ARE_EQUAL(L"12 3456 7890", m_calcEngine->GroupDigitsPerRadix(L"1234567890", 16), L"Verify grouping in hexadecimal.");
            VERIFY_ARE_EQUAL(L"1234", m_calcEngine->GroupDigitsPerRadix(L"1234", 16), L"Verify minimum grouping in hexadecimal.");

            // Decimal
            VERIFY_ARE_EQUAL(L"1,234,567,890", m_calcEngine->GroupDigitsPerRadix(L"1234567890", 10), L"Verify grouping in base10.");
            VERIFY_ARE_EQUAL(L"1,234,567.89", m_calcEngine->GroupDigitsPerRadix(L"1234567.89", 10), L"Verify grouping in base10 with decimal.");
            VERIFY_ARE_EQUAL(L"1,234,567e89", m_calcEngine->GroupDigitsPerRadix(L"1234567e89", 10), L"Verify grouping in base10 with exponent.");
            VERIFY_ARE_EQUAL(
                L"1,234,567.89e5", m_calcEngine->GroupDigitsPerRadix(L"1234567.89e5", 10), L"Verify grouping in base10 with decimal and exponent.");
            VERIFY_ARE_EQUAL(L"-123,456,789", m_calcEngine->GroupDigitsPerRadix(L"-123456789", 10), L"Verify grouping in base10 with negative.");
        }

        TEST_METHOD(TestIsNumberInvalid)
        {
            // Binary Number Checks
            vector<wstring> validBinStrs{ L"0", L"1", L"0011", L"1100" };
            vector<wstring> invalidBinStrs{ L"2", L"A", L"0.1" };
            for (wstring const& str : validBinStrs)
            {
                VERIFY_ARE_EQUAL(0, m_calcEngine->IsNumberInvalid(str, 0, 0, 2 /* Binary */));
            }
            for (wstring const& str : invalidBinStrs)
            {
                VERIFY_ARE_EQUAL(IDS_ERR_UNK_CH, m_calcEngine->IsNumberInvalid(str, 0, 0, 2 /* Binary */));
            }

            // Octal Number Checks
            vector<wstring> validOctStrs{ L"0", L"7", L"01234567", L"76543210" };
            vector<wstring> invalidOctStrs{ L"8", L"A", L"0.7" };
            for (wstring const& str : validOctStrs)
            {
                VERIFY_ARE_EQUAL(0, m_calcEngine->IsNumberInvalid(str, 0, 0, 8 /* Octal */));
            }
            for (wstring const& str : invalidOctStrs)
            {
                VERIFY_ARE_EQUAL(IDS_ERR_UNK_CH, m_calcEngine->IsNumberInvalid(str, 0, 0, 8 /* Octal */));
            }

            // Hexadecimal Number Checks
            vector<wstring> validHexStrs{ L"0", L"F", L"0123456789ABCDEF", L"FEDCBA9876543210" };
            vector<wstring> invalidHexStrs{ L"G", L"abcdef", L"x", L"0.1" };
            for (wstring const& str : validHexStrs)
            {
                VERIFY_ARE_EQUAL(0, m_calcEngine->IsNumberInvalid(str, 0, 0, 16 /* HEx */));
            }
            for (wstring const& str : invalidHexStrs)
            {
                VERIFY_ARE_EQUAL(IDS_ERR_UNK_CH, m_calcEngine->IsNumberInvalid(str, 0, 0, 16 /* Hex */));
            }

            // Decimal Number Checks

            // Special case errors: long exponent, long mantissa
            wstring longExp(L"1e12345");
            VERIFY_ARE_EQUAL(0, m_calcEngine->IsNumberInvalid(longExp, 5 /* Max exp length */, 100, 10 /* Decimal */));
            VERIFY_ARE_EQUAL(IDS_ERR_INPUT_OVERFLOW, m_calcEngine->IsNumberInvalid(longExp, 4 /* Max exp length */, 100, 10 /* Decimal */));
            // Mantissa length is sum of:
            //  - digits before decimal separator, minus leading zeroes
            //  - digits after decimal separator, including trailing zeroes
            // Each of these mantissa values should calculate as a length of 5
            vector<wstring> longMantStrs{ L"10000", L"10.000", L"0000012345", L"123.45", L"0.00123", L"0.12345", L"-123.45e678" };
            for (wstring const& str : longMantStrs)
            {
                VERIFY_ARE_EQUAL(0, m_calcEngine->IsNumberInvalid(str, 100, 5 /* Max mantissa length */, 10 /* Decimal */));
            }
            for (wstring const& str : longMantStrs)
            {
                VERIFY_ARE_EQUAL(IDS_ERR_INPUT_OVERFLOW, m_calcEngine->IsNumberInvalid(str, 100, 4 /* Max mantissa length */, 10 /* Decimal */));
            }

            // Regex matching (descriptions taken from CalcUtils.cpp)
            // Use 100 for exp/mantissa length as they are tested above
            vector<wstring> validDecStrs{ // Start with an optional + or -
                                          L"+1",
                                          L"-1",
                                          L"1",
                                          // Followed by zero or more digits
                                          L"-",
                                          L"",
                                          L"1234567890",
                                          // Followed by an optional decimal point
                                          L"1.0",
                                          L"-.",
                                          L"1.",
                                          // Followed by zero or more digits
                                          L"0.0",
                                          L"0.123456",
                                          // Followed by an optional exponent ('e')
                                          L"1e",
                                          L"1.e",
                                          L"-e",
                                          // If there's an exponent, its optionally followed by + or -
                                          // and followed by zero or more digits
                                          L"1e+12345",
                                          L"1e-12345",
                                          L"1e123",
                                          // All together
                                          L"-123.456e+789"
            };
            vector<wstring> invalidDecStrs{ L"x123", L"123-", L"1e1.2", L"1-e2" };
            for (wstring const& str : validDecStrs)
            {
                VERIFY_ARE_EQUAL(0, m_calcEngine->IsNumberInvalid(str, 100, 100, 10 /* Dec */));
            }
            for (wstring const& str : invalidDecStrs)
            {
                VERIFY_ARE_EQUAL(IDS_ERR_UNK_CH, m_calcEngine->IsNumberInvalid(str, 100, 100, 10 /* Dec */));
            }
        }

        TEST_METHOD(TestDigitGroupingStringToGroupingVector)
        {
            vector<uint32_t> groupingVector{};
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(L""), L"Verify empty grouping");

            groupingVector = { 1 };
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(L"1"), L"Verify simple grouping");

            groupingVector = { 3, 0 };
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(L"3;0"), L"Verify standard grouping");

            groupingVector = { 3, 0, 0 };
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(L"3;0;0"), L"Verify expanded non-repeating grouping");

            groupingVector = { 5, 3, 2, 4, 6 };
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(L"5;3;2;4;6"), L"Verify long grouping");

            groupingVector = { 15, 15, 15, 0 };
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(L"15;15;15;0"), L"Verify large grouping");

            groupingVector = { 4, 7, 0 };
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(L"4;16;7;25;0"), L"Verify we ignore oversize grouping");

            groupingVector = { 3, 0 };
            constexpr wstring_view nonRepeatingGrouping = L"3;0;0";
            constexpr wstring_view repeatingGrouping = nonRepeatingGrouping.substr(0, 3);
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(repeatingGrouping), L"Verify we don't go past the end of wstring_view range");
        }

        TEST_METHOD(TestGroupDigits)
        {
            wstring result{ L"1234567" };
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L"", { 3, 0 }, L"1234567", false), L"Verify handling of empty delimiter.");
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", {}, L"1234567", false), L"Verify handling of empty grouping.");

            result = L"1,234,567";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 3, 0 }, L"1234567", false), L"Verify standard digit grouping.");

            result = L"1 234 567";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L" ", { 3, 0 }, L"1234567", false), L"Verify delimiter change.");

            result = L"1|||234|||567";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L"|||", { 3, 0 }, L"1234567", false), L"Verify long delimiter.");

            result = L"12,345e67";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 3, 0 }, L"12345e67", false), L"Verify respect of exponent.");

            result = L"12,345.67";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 3, 0 }, L"12345.67", false), L"Verify respect of decimal.");

            result = L"1,234.56e7";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 3, 0 }, L"1234.56e7", false), L"Verify respect of exponent and decimal.");

            result = L"-1,234,567";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 3, 0 }, L"-1234567", true), L"Verify negative number grouping.");

            // Test various groupings
            result = L"1234567890123456";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 0, 0 }, L"1234567890123456", false), L"Verify no grouping.");

            result = L"1234567890123,456";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 3 }, L"1234567890123456", false), L"Verify non-repeating grouping.");
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 3, 0, 0 }, L"1234567890123456", false), L"Verify expanded form non-repeating grouping.");

            result = L"12,34,56,78,901,23456";
            VERIFY_ARE_EQUAL(
                result, m_calcEngine->GroupDigits(L",", { 5, 3, 2, 0 }, L"1234567890123456", false), L"Verify multigroup with repeating grouping.");

            result = L"1234,5678,9012,3456";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 4, 0 }, L"1234567890123456", false), L"Verify repeating non-standard grouping.");

            result = L"123456,78,901,23456";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 5, 3, 2 }, L"1234567890123456", false), L"Verify multigroup non-repeating grouping.");
            VERIFY_ARE_EQUAL(
                result,
                m_calcEngine->GroupDigits(L",", { 5, 3, 2, 0, 0 }, L"1234567890123456", false),
                L"Verify expanded form multigroup non-repeating grouping.");
        }

    private:
        unique_ptr<CCalcEngine> m_calcEngine;
        shared_ptr<IResourceProvider> m_resourceProvider;
        shared_ptr<CalculatorHistory> m_history;
    };
```

#### 3.1.Z Remaining non-blank source not inside a `TEST_CLASS` or `TEST_METHOD` (fixture glue)

<!-- CalcEngineTests.cpp:243-243 -->
```cpp
}
```

### 3.2 `src/CalculatorUnitTests/CalcInputTest.cpp` — `TEST_CLASS(CalcInputTest)`

37 test methods.

| # | `TEST_METHOD` | Kind | Source lines |
|---|---|---|---|
| 1 | `Clear` | inline in class | `CalcInputTest.cpp:25-40` |
| 2 | `TryToggleSignZero` | inline in class | `CalcInputTest.cpp:42-46` |
| 3 | `TryToggleSignExponent` | inline in class | `CalcInputTest.cpp:47-56` |
| 4 | `TryToggleSignBase` | inline in class | `CalcInputTest.cpp:57-64` |
| 5 | `TryToggleSignBaseIntegerMode` | inline in class | `CalcInputTest.cpp:65-70` |
| 6 | `TryToggleSignRollover` | inline in class | `CalcInputTest.cpp:71-79` |
| 7 | `TryAddDigitLeadingZeroes` | inline in class | `CalcInputTest.cpp:81-87` |
| 8 | `TryAddDigitMaxCount` | inline in class | `CalcInputTest.cpp:88-106` |
| 9 | `TryAddDigitValues` | inline in class | `CalcInputTest.cpp:107-116` |
| 10 | `TryAddDigitRolloverBaseCheck` | inline in class | `CalcInputTest.cpp:117-122` |
| 11 | `TryAddDigitRolloverOctalByte` | inline in class | `CalcInputTest.cpp:123-131` |
| 12 | `TryAddDigitRolloverOctalWord` | inline in class | `CalcInputTest.cpp:132-140` |
| 13 | `TryAddDigitRolloverOctalDword` | inline in class | `CalcInputTest.cpp:141-149` |
| 14 | `TryAddDigitRolloverOctalQword` | inline in class | `CalcInputTest.cpp:150-158` |
| 15 | `TryAddDigitRolloverDecimal` | inline in class | `CalcInputTest.cpp:159-178` |
| 16 | `TryAddDecimalPtEmpty` | inline in class | `CalcInputTest.cpp:180-186` |
| 17 | `TryAddDecimalPointTwice` | inline in class | `CalcInputTest.cpp:187-193` |
| 18 | `TryAddDecimalPointExponent` | inline in class | `CalcInputTest.cpp:194-200` |
| 19 | `TryBeginExponentNoExponent` | inline in class | `CalcInputTest.cpp:202-207` |
| 20 | `TryBeginExponentWithExponent` | inline in class | `CalcInputTest.cpp:208-213` |
| 21 | `BackspaceZero` | inline in class | `CalcInputTest.cpp:215-219` |
| 22 | `BackspaceSingleChar` | inline in class | `CalcInputTest.cpp:220-226` |
| 23 | `BackspaceMultiChar` | inline in class | `CalcInputTest.cpp:227-234` |
| 24 | `BackspaceDecimal` | inline in class | `CalcInputTest.cpp:235-244` |
| 25 | `BackspaceMultiCharDecimal` | inline in class | `CalcInputTest.cpp:245-254` |
| 26 | `BackspaceZeroDecimalWithoutPrefixZeros` | inline in class | `CalcInputTest.cpp:256-264` |
| 27 | `SetDecimalSymbol` | inline in class | `CalcInputTest.cpp:266-272` |
| 28 | `ToStringEmpty` | inline in class | `CalcInputTest.cpp:274-277` |
| 29 | `ToStringNegative` | inline in class | `CalcInputTest.cpp:278-283` |
| 30 | `ToStringExponentBase10` | inline in class | `CalcInputTest.cpp:284-289` |
| 31 | `ToStringExponentBase8` | inline in class | `CalcInputTest.cpp:290-295` |
| 32 | `ToStringExponentNegative` | inline in class | `CalcInputTest.cpp:296-302` |
| 33 | `ToStringExponentPositive` | inline in class | `CalcInputTest.cpp:303-311` |
| 34 | `ToStringInteger` | inline in class | `CalcInputTest.cpp:312-316` |
| 35 | `ToStringBaseTooLong` | inline in class | `CalcInputTest.cpp:317-327` |
| 36 | `ToStringExponentTooLong` | inline in class | `CalcInputTest.cpp:328-354` |
| 37 | `ToRational` | inline in class | `CalcInputTest.cpp:356-366` |

#### 3.2.0 File preamble, `using` directives and fixture helpers (`CalcInputTest.cpp:1-12`)

```cpp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>

using namespace std;
using namespace CalculationManager;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CalculatorEngineTests
{
```

#### 3.2.1 `TEST_CLASS` block verbatim (`CalcInputTest.cpp:13-370`)

```cpp
    TEST_CLASS(CalcInputTest)
    {
        TEST_METHOD_INITIALIZE(CommonSetup)
        {
            m_calcInput = CalcEngine::CalcInput(L'.');
        }
        TEST_METHOD_CLEANUP(Cleanup)
        {
            m_calcInput.Clear();
            m_calcInput.SetDecimalSymbol(L'.');
        }

        TEST_METHOD(Clear)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryToggleSign(false, L"999");
            m_calcInput.TryAddDecimalPt();
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            m_calcInput.TryAddDigit(3, 10, false, L"999", 64, 32);

            VERIFY_ARE_EQUAL(L"-1.2e+3", m_calcInput.ToString(10), L"Verify input is correct.");

            m_calcInput.Clear();

            ::Logger::WriteMessage(m_calcInput.ToString(10).c_str());
            VERIFY_ARE_EQUAL(L"0", m_calcInput.ToString(10), L"Verify input is 0 after clear.");
        }

        TEST_METHOD(TryToggleSignZero)
        {
            VERIFY_IS_TRUE(m_calcInput.TryToggleSign(false, L"999"), L"Verify toggling 0 succeeds.");
            VERIFY_ARE_EQUAL(L"0", m_calcInput.ToString(10), L"Verify toggling 0 does not create -0.");
        }
        TEST_METHOD(TryToggleSignExponent)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryToggleSign(false, L"999"), L"Verify toggling exponent sign succeeds.");
            VERIFY_ARE_EQUAL(L"1.e-2", m_calcInput.ToString(10), L"Verify toggling exponent sign does not toggle base sign.");
            VERIFY_IS_TRUE(m_calcInput.TryToggleSign(false, L"999"), L"Verify toggling exponent sign succeeds.");
            VERIFY_ARE_EQUAL(L"1.e+2", m_calcInput.ToString(10), L"Verify toggling negative exponent sign does not toggle base sign.");
        }
        TEST_METHOD(TryToggleSignBase)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryToggleSign(false, L"999"), L"Verify toggling base sign succeeds.");
            VERIFY_ARE_EQUAL(L"-1", m_calcInput.ToString(10), L"Verify toggling base sign creates negative base.");
            VERIFY_IS_TRUE(m_calcInput.TryToggleSign(false, L"999"), L"Verify toggling base sign succeeds.");
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify toggling negative base sign creates positive base.");
        }
        TEST_METHOD(TryToggleSignBaseIntegerMode)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryToggleSign(true, L"999"), L"Verify toggling base sign in integer mode succeeds.");
            VERIFY_ARE_EQUAL(L"-1", m_calcInput.ToString(10), L"Verify toggling base sign creates negative base.");
        }
        TEST_METHOD(TryToggleSignRollover)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryToggleSign(true, L"127"), L"Verify toggling base sign in integer mode succeeds.");
            m_calcInput.TryAddDigit(8, 10, false, L"999", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryToggleSign(true, L"127"), L"Verify toggling base sign in integer mode fails on rollover.");
            VERIFY_ARE_EQUAL(L"-128", m_calcInput.ToString(10), L"Verify toggling base sign on rollover does not change value.");
        }

        TEST_METHOD(TryAddDigitLeadingZeroes)
        {
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(0, 10, false, L"999", 64, 32), L"Verify TryAddDigit succeeds.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(0, 10, false, L"999", 64, 32), L"Verify TryAddDigit succeeds.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(0, 10, false, L"999", 64, 32), L"Verify TryAddDigit succeeds.");
            VERIFY_ARE_EQUAL(L"0", m_calcInput.ToString(10), L"Verify leading zeros are ignored.");
        }
        TEST_METHOD(TryAddDigitMaxCount)
        {
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32), L"Verify TryAddDigit for base with length < maxDigits succeeds.");
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify adding digit for base with length < maxDigits succeeded.");
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 1), L"Verify TryAddDigit for base with length > maxDigits fails.");
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify digit for base was not added.");
            m_calcInput.TryBeginExponent();
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32), L"Verify TryAddDigit for exponent with length < maxDigits succeeds.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32), L"Verify TryAddDigit for exponent with length < maxDigits succeeds.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(3, 10, false, L"999", 64, 32), L"Verify TryAddDigit for exponent with length < maxDigits succeeds.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(4, 10, false, L"999", 64, 32), L"Verify TryAddDigit for exponent with length < maxDigits succeeds.");
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(5, 10, false, L"999", 64, 32), L"Verify TryAddDigit for exponent with length > maxDigits fails.");
            VERIFY_ARE_EQUAL(L"1.e+1234", m_calcInput.ToString(10), L"Verify adding digits for exponent with length < maxDigits succeeded.");

            m_calcInput.Clear();
            m_calcInput.TryAddDecimalPt();
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 1), L"Verify decimal point and leading zero does not count toward maxDigits.");
            VERIFY_ARE_EQUAL(L"0.1", m_calcInput.ToString(10), L"Verify input value checking dec pt and leading zero impact on maxDigits.");
        }
        TEST_METHOD(TryAddDigitValues)
        {
            // Use an arbitrary value > 16 to test that input accepts digits > hexadecimal 0xF.
            // TryAddDigit does not validate whether the digit fits within the current radix.
            for (unsigned int i = 0; i < 25; i++)
            {
                VERIFY_IS_TRUE(m_calcInput.TryAddDigit(i, 10, false, L"999", 64, 32), (L"Verify TryAddDigit succeeds for " + to_wstring(i)).c_str());
                m_calcInput.Clear();
            }
        }
        TEST_METHOD(TryAddDigitRolloverBaseCheck)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(2, 16, true, L"999", 64, 1), L"Verify TryAddDigit rollover fails for bases other than 8,10.");
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(1, 2, true, L"999", 64, 1), L"Verify TryAddDigit rollover fails for bases other than 8,10.");
        }
        TEST_METHOD(TryAddDigitRolloverOctalByte)
        {
            m_calcInput.TryAddDigit(1, 8, true, L"777", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(2, 8, true, L"377", 8, 1), L"Verify we can add an extra digit in OctalByte if first digit <= 3.");

            m_calcInput.Clear();
            m_calcInput.TryAddDigit(4, 8, true, L"777", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(2, 8, true, L"377", 8, 1), L"Verify we cannot add an extra digit in OctalByte if first digit > 3.");
        }
        TEST_METHOD(TryAddDigitRolloverOctalWord)
        {
            m_calcInput.TryAddDigit(1, 8, true, L"777", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(2, 8, true, L"377", 16, 1), L"Verify we can add an extra digit in OctalByte if first digit == 1.");

            m_calcInput.Clear();
            m_calcInput.TryAddDigit(2, 8, true, L"777", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(2, 8, true, L"377", 16, 1), L"Verify we cannot add an extra digit in OctalByte if first digit > 1.");
        }
        TEST_METHOD(TryAddDigitRolloverOctalDword)
        {
            m_calcInput.TryAddDigit(1, 8, true, L"777", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(2, 8, true, L"377", 32, 1), L"Verify we can add an extra digit in OctalByte if first digit <= 3.");

            m_calcInput.Clear();
            m_calcInput.TryAddDigit(4, 8, true, L"777", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(2, 8, true, L"377", 32, 1), L"Verify we cannot add an extra digit in OctalByte if first digit > 3.");
        }
        TEST_METHOD(TryAddDigitRolloverOctalQword)
        {
            m_calcInput.TryAddDigit(1, 8, true, L"777", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(2, 8, true, L"377", 64, 1), L"Verify we can add an extra digit in OctalByte if first digit == 1.");

            m_calcInput.Clear();
            m_calcInput.TryAddDigit(2, 8, true, L"777", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(2, 8, true, L"377", 64, 1), L"Verify we cannot add an extra digit in OctalByte if first digit > 1.");
        }
        TEST_METHOD(TryAddDigitRolloverDecimal)
        {
            m_calcInput.TryAddDigit(1, 10, true, L"127", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(0, 10, true, L"1", 8, 1), L"Verify we cannot add a digit if input size matches maxStr size.");
            m_calcInput.TryAddDigit(2, 10, true, L"127", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(2, 10, true, L"110", 8, 2), L"Verify we cannot add a digit if n char comparison > 0.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(7, 10, true, L"130", 8, 2), L"Verify we can add a digit if n char comparison < 0.");

            m_calcInput.Clear();
            m_calcInput.TryAddDigit(1, 10, true, L"127", 64, 32);
            m_calcInput.TryAddDigit(2, 10, true, L"127", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(8, 10, true, L"127", 8, 2), L"Verify we cannot add a digit if digit exceeds max value.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(7, 10, true, L"127", 8, 2), L"Verify we can add a digit if digit does not exceed max value.");

            m_calcInput.Backspace();
            m_calcInput.TryToggleSign(true, L"127");
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(9, 10, true, L"127", 8, 2), L"Negative value: verify we cannot add a digit if digit exceeds max value.");
            VERIFY_IS_TRUE(
                m_calcInput.TryAddDigit(8, 10, true, L"127", 8, 2), L"Negative value: verify we can add a digit if digit does not exceed max value.");
        }

        TEST_METHOD(TryAddDecimalPtEmpty)
        {
            VERIFY_IS_FALSE(m_calcInput.HasDecimalPt(), L"Verify input has no decimal point.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDecimalPt(), L"Verify adding decimal to empty input.");
            VERIFY_IS_TRUE(m_calcInput.HasDecimalPt(), L"Verify input has decimal point.");
            VERIFY_ARE_EQUAL(L"0.", m_calcInput.ToString(10), L"Verify decimal on empty input.");
        }
        TEST_METHOD(TryAddDecimalPointTwice)
        {
            VERIFY_IS_FALSE(m_calcInput.HasDecimalPt(), L"Verify input has no decimal point.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDecimalPt(), L"Verify adding decimal to empty input.");
            VERIFY_IS_TRUE(m_calcInput.HasDecimalPt(), L"Verify input has decimal point.");
            VERIFY_IS_FALSE(m_calcInput.TryAddDecimalPt(), L"Verify adding decimal point fails if input has decimal point.");
        }
        TEST_METHOD(TryAddDecimalPointExponent)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDecimalPt(), L"Verify adding decimal point fails if input has exponent.");
        }

        TEST_METHOD(TryBeginExponentNoExponent)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryBeginExponent(), L"Verify adding exponent succeeds on input without exponent.");
            VERIFY_ARE_EQUAL(L"1.e+0", m_calcInput.ToString(10), L"Verify exponent present.");
        }
        TEST_METHOD(TryBeginExponentWithExponent)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryBeginExponent(), L"Verify adding exponent succeeds on input without exponent.");
            VERIFY_IS_FALSE(m_calcInput.TryBeginExponent(), L"Verify cannot add exponent if input already has exponent.");
        }

        TEST_METHOD(BackspaceZero)
        {
            m_calcInput.Backspace();
            VERIFY_ARE_EQUAL(L"0", m_calcInput.ToString(10), L"Verify backspace on 0 is still 0.");
        }
        TEST_METHOD(BackspaceSingleChar)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify input before backspace.");
            m_calcInput.Backspace();
            VERIFY_ARE_EQUAL(L"0", m_calcInput.ToString(10), L"Verify input after backspace.");
        }
        TEST_METHOD(BackspaceMultiChar)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            VERIFY_ARE_EQUAL(L"12", m_calcInput.ToString(10), L"Verify input before backspace.");
            m_calcInput.Backspace();
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify input after backspace.");
        }
        TEST_METHOD(BackspaceDecimal)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDecimalPt();
            VERIFY_ARE_EQUAL(L"1.", m_calcInput.ToString(10), L"Verify input before backspace.");
            VERIFY_IS_TRUE(m_calcInput.HasDecimalPt(), L"Verify input has decimal point.");
            m_calcInput.Backspace();
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify input after backspace.");
            VERIFY_IS_FALSE(m_calcInput.HasDecimalPt(), L"Verify decimal point was removed.");
        }
        TEST_METHOD(BackspaceMultiCharDecimal)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDecimalPt();
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDigit(3, 10, false, L"999", 64, 32);
            VERIFY_ARE_EQUAL(L"1.23", m_calcInput.ToString(10), L"Verify input before backspace.");
            m_calcInput.Backspace();
            VERIFY_ARE_EQUAL(L"1.2", m_calcInput.ToString(10), L"Verify input after backspace.");
        }
        // Issue #817: Prefixed multiple zeros
        TEST_METHOD(BackspaceZeroDecimalWithoutPrefixZeros)
        {
            m_calcInput.TryAddDigit(0, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDecimalPt();
            VERIFY_ARE_EQUAL(L"0.", m_calcInput.ToString(10), L"Verify input before backspace.")
            m_calcInput.Backspace();
            m_calcInput.TryAddDigit(0, 10, false, L"999", 64, 32);
            VERIFY_ARE_EQUAL(L"0", m_calcInput.ToString(10), L"Verify input after backspace.")
        }

        TEST_METHOD(SetDecimalSymbol)
        {
            m_calcInput.TryAddDecimalPt();
            VERIFY_ARE_EQUAL(L"0.", m_calcInput.ToString(10), L"Verify default decimal point.");
            m_calcInput.SetDecimalSymbol(L',');
            VERIFY_ARE_EQUAL(L"0,", m_calcInput.ToString(10), L"Verify new decimal point.");
        }

        TEST_METHOD(ToStringEmpty)
        {
            VERIFY_ARE_EQUAL(L"0", m_calcInput.ToString(10), L"Verify ToString of empty value.");
        }
        TEST_METHOD(ToStringNegative)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryToggleSign(false, L"999");
            VERIFY_ARE_EQUAL(L"-1", m_calcInput.ToString(10), L"Verify ToString of negative value.");
        }
        TEST_METHOD(ToStringExponentBase10)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            VERIFY_ARE_EQUAL(L"1.e+0", m_calcInput.ToString(10), L"Verify ToString of empty base10 exponent.");
        }
        TEST_METHOD(ToStringExponentBase8)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            VERIFY_ARE_EQUAL(L"1.^+0", m_calcInput.ToString(8), L"Verify ToString of empty base8 exponent.");
        }
        TEST_METHOD(ToStringExponentNegative)
        {
            m_calcInput.TryAddDigit(1, 8, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            m_calcInput.TryToggleSign(false, L"999");
            VERIFY_ARE_EQUAL(L"1.e-0", m_calcInput.ToString(10), L"Verify ToString of empty negative exponent.");
        }
        TEST_METHOD(ToStringExponentPositive)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDigit(3, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDigit(4, 10, false, L"999", 64, 32);
            VERIFY_ARE_EQUAL(L"1.e+234", m_calcInput.ToString(10), L"Verify ToString of exponent with value.");
        }
        TEST_METHOD(ToStringInteger)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify ToString of integer value hides decimal.");
        }
        TEST_METHOD(ToStringBaseTooLong)
        {
            wstring maxStr{};
            for (size_t i = 0; i < MAX_STRLEN + 1; i++)
            {
                maxStr += L'1';
                m_calcInput.TryAddDigit(1, 10, false, maxStr, 64, 100);
            }
            auto result = m_calcInput.ToString(10);
            VERIFY_IS_TRUE(result.empty(), L"Verify ToString of base value that is too large yields empty string.");
        }
        TEST_METHOD(ToStringExponentTooLong)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            wstring maxStr{ L"11" };
            bool exponentCapped = false;
            for (size_t i = 0; i < MAX_STRLEN + 1; i++)
            {
                maxStr += L'1';
                if (!m_calcInput.TryAddDigit(1, 10, false, maxStr, 64, MAX_STRLEN + 25))
                {
                    exponentCapped = true;
                }
            }
            auto result = m_calcInput.ToString(10);

            // TryAddDigit caps the exponent length to C_EXP_MAX_DIGITS = 4, so ToString() succeeds.
            // If that cap is removed, ToString() should return an empty string.
            if (exponentCapped)
            {
                VERIFY_ARE_EQUAL(L"1.e+1111", result, L"Verify ToString succeeds; exponent length is capped at C_EXP_MAX_DIGITS.");
            }
            else
            {
                VERIFY_IS_TRUE(result.empty(), L"Verify ToString of exponent value that is too large yields empty string.");
            }
        }

        TEST_METHOD(ToRational)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDigit(3, 10, false, L"999", 64, 32);
            VERIFY_ARE_EQUAL(L"123", m_calcInput.ToString(10), L"Verify input before conversion to rational.");

            auto rat = m_calcInput.ToRational(10, false);
            VERIFY_ARE_EQUAL(1, rat.P().Mantissa().size(), L"Verify digit count of rational.");
            VERIFY_ARE_EQUAL(123, rat.P().Mantissa().front(), L"Verify first digit of mantissa.");
        }

    private:
        CalcEngine::CalcInput m_calcInput;
    };
```

#### 3.2.Z Remaining non-blank source not inside a `TEST_CLASS` or `TEST_METHOD` (fixture glue)

<!-- CalcInputTest.cpp:371-371 -->
```cpp
}
```

### 3.3 `src/CalculatorUnitTests/CalculatorManagerTest.cpp` — `TEST_CLASS(CalculatorManagerTest)`

21 test methods.

| # | `TEST_METHOD` | Kind | Source lines |
|---|---|---|---|
| 1 | `CalculatorManagerTestStandard` | out-of-line definition | `CalculatorManagerTest.cpp:320-402` |
| 2 | `CalculatorManagerTestScientific` | out-of-line definition | `CalculatorManagerTest.cpp:404-498` |
| 3 | `CalculatorManagerTestScientific2` | out-of-line definition | `CalculatorManagerTest.cpp:501-634` |
| 4 | `CalculatorManagerTestScientificParenthesis` | out-of-line definition | `CalculatorManagerTest.cpp:636-679` |
| 5 | `CalculatorManagerTestScientificError` | out-of-line definition | `CalculatorManagerTest.cpp:681-702` |
| 6 | `CalculatorManagerTestScientificModeChange` | out-of-line definition | `CalculatorManagerTest.cpp:705-724` |
| 7 | `CalculatorManagerTestModeChange` | out-of-line definition | `CalculatorManagerTest.cpp:726-751` |
| 8 | `CalculatorManagerTestProgrammer` | out-of-line definition | `CalculatorManagerTest.cpp:753-813` |
| 9 | `CalculatorManagerTestMemory` | out-of-line definition | `CalculatorManagerTest.cpp:815-960` |
| 10 | `CalculatorManagerTestMaxDigitsReached` | out-of-line definition | `CalculatorManagerTest.cpp:963-966` |
| 11 | `CalculatorManagerTestMaxDigitsReached_LeadingDecimal` | out-of-line definition | `CalculatorManagerTest.cpp:968-971` |
| 12 | `CalculatorManagerTestMaxDigitsReached_TrailingDecimal` | out-of-line definition | `CalculatorManagerTest.cpp:973-976` |
| 13 | `UnitConversionManagerNumberFormattingUtils_TrimTrailingZeros` | out-of-line definition | `CalculatorManagerTest.cpp:978-1001` |
| 14 | `UnitConversionManagerNumberFormattingUtils_GetNumberDigits` | out-of-line definition | `CalculatorManagerTest.cpp:1003-1020` |
| 15 | `UnitConversionManagerNumberFormattingUtils_GetNumberDigitsWholeNumberPart` | out-of-line definition | `CalculatorManagerTest.cpp:1022-1040` |
| 16 | `UnitConversionManagerNumberFormattingUtils_RoundSignificantDigits` | out-of-line definition | `CalculatorManagerTest.cpp:1042-1056` |
| 17 | `UnitConversionManagerNumberFormattingUtils_ToScientificNumber` | out-of-line definition | `CalculatorManagerTest.cpp:1058-1076` |
| 18 | `CalculatorManagerTestBinaryOperatorReceived` | out-of-line definition | `CalculatorManagerTest.cpp:1078-1092` |
| 19 | `CalculatorManagerTestBinaryOperatorReceived_Multiple` | out-of-line definition | `CalculatorManagerTest.cpp:1094-1108` |
| 20 | `CalculatorManagerTestBinaryOperatorReceived_LongInput` | out-of-line definition | `CalculatorManagerTest.cpp:1110-1134` |
| 21 | `CalculatorManagerTestStandardOrderOfOperations` | out-of-line definition | `CalculatorManagerTest.cpp:1136-1211` |

#### 3.3.0 File preamble, `using` directives and fixture helpers (`CalculatorManagerTest.cpp:1-170`)

```cpp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <CppUnitTest.h>

#include "CalcManager/CalculatorHistory.h"
#include "EngineResourceProvider.h"
#include "CalcManager/NumberFormattingUtils.h"

using namespace CalculatorApp;
using namespace CalculatorApp::ViewModel::Common;
using namespace CalculationManager;
using namespace UnitConversionManager::NumberFormattingUtils;
using namespace Platform;
using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CalculatorManagerTest
{
    class CalculatorManagerDisplayTester final : public ICalcDisplay
    {
    public:
        CalculatorManagerDisplayTester()
        {
            Reset();
        }

        void Reset()
        {
            m_isError = false;
            m_maxDigitsCalledCount = 0;
            m_binaryOperatorReceivedCallCount = 0;
        }

        void SetPrimaryDisplay(const wstring& text, bool isError) override
        {
            m_primaryDisplay = text;
            m_isError = isError;
        }
        void SetIsInError(bool isError) override
        {
            m_isError = isError;
        }
        void SetExpressionDisplay(
            _Inout_ std::shared_ptr<std::vector<std::pair<std::wstring, int>>> const& tokens,
            _Inout_ std::shared_ptr<std::vector<std::shared_ptr<IExpressionCommand>>> const& /*commands*/) override
        {
            m_expression.clear();

            for (const auto& currentPair : *tokens)
            {
                m_expression += currentPair.first;
            }
        }
        void SetMemorizedNumbers(const vector<wstring>& numbers) override
        {
            m_memorizedNumberStrings = numbers;
        }

        void SetParenthesisNumber(unsigned int parenthesisCount) override
        {
            m_parenDisplay = parenthesisCount;
        }

        void OnNoRightParenAdded() override
        {
            // This method is used to create a narrator announcement when a close parenthesis cannot be added because there are no open parentheses
        }

        const wstring& GetPrimaryDisplay() const
        {
            return m_primaryDisplay;
        }
        const wstring& GetExpression() const
        {
            return m_expression;
        }
        const vector<wstring>& GetMemorizedNumbers() const
        {
            return m_memorizedNumberStrings;
        }
        bool GetIsError() const
        {
            return m_isError;
        }

        void OnHistoryItemAdded(_In_ unsigned int /*addedItemIndex */) override
        {
        }

        void MaxDigitsReached() override
        {
            m_maxDigitsCalledCount++;
        }

        void InputChanged() override
        {
        }

        int GetMaxDigitsCalledCount()
        {
            return m_maxDigitsCalledCount;
        }

        void BinaryOperatorReceived() override
        {
            m_binaryOperatorReceivedCallCount++;
        }

        void MemoryItemChanged(unsigned int /*indexOfMemory*/) override
        {
        }

        int GetBinaryOperatorReceivedCallCount()
        {
            return m_binaryOperatorReceivedCallCount;
        }

    private:
        wstring m_primaryDisplay;
        wstring m_expression;
        unsigned int m_parenDisplay;
        bool m_isError;
        vector<wstring> m_memorizedNumberStrings;
        int m_maxDigitsCalledCount;
        int m_binaryOperatorReceivedCallCount;
    };

    class TestDriver
    {
    private:
        static shared_ptr<CalculatorManagerDisplayTester> m_displayTester;
        static shared_ptr<CalculationManager::CalculatorManager> m_calculatorManager;

    public:
        static void Initialize(shared_ptr<CalculatorManagerDisplayTester> displayTester, shared_ptr<CalculatorManager> calculatorManager)
        {
            m_displayTester = displayTester;
            m_calculatorManager = calculatorManager;
        }

        static void Test(wstring expectedPrimary, wstring expectedExpression, Command testCommands[], bool cleanup = true, bool isScientific = false)
        {
            if (cleanup)
            {
                m_calculatorManager->Reset();
            }

            if (isScientific)
            {
                m_calculatorManager->SendCommand(Command::ModeScientific);
            }

            Command* currentCommand = testCommands;
            while (*currentCommand != Command::CommandNULL)
            {
                m_calculatorManager->SendCommand(*currentCommand++);
            }

            VERIFY_ARE_EQUAL(expectedPrimary, m_displayTester->GetPrimaryDisplay());
            if (expectedExpression != L"N/A")
            {
                VERIFY_ARE_EQUAL(expectedExpression, m_displayTester->GetExpression());
            }
        }
    };

    // Declare this class as a TestClass, and supply metadata if necessary.
```

#### 3.3.1 `TEST_CLASS` block verbatim (`CalculatorManagerTest.cpp:171-273`)

```cpp
    TEST_CLASS(CalculatorManagerTest)
    {
    public:
        TEST_METHOD_INITIALIZE(CommonSetup);

        TEST_METHOD(CalculatorManagerTestStandard);

        TEST_METHOD(CalculatorManagerTestScientific);
        TEST_METHOD(CalculatorManagerTestScientific2);
        TEST_METHOD(CalculatorManagerTestScientificParenthesis);
        TEST_METHOD(CalculatorManagerTestScientificError);
        TEST_METHOD(CalculatorManagerTestScientificModeChange);

        TEST_METHOD(CalculatorManagerTestProgrammer);

        TEST_METHOD(CalculatorManagerTestModeChange);

        TEST_METHOD(CalculatorManagerTestMemory);

        TEST_METHOD(CalculatorManagerTestMaxDigitsReached);
        TEST_METHOD(CalculatorManagerTestMaxDigitsReached_LeadingDecimal);
        TEST_METHOD(CalculatorManagerTestMaxDigitsReached_TrailingDecimal);

        TEST_METHOD(UnitConversionManagerNumberFormattingUtils_TrimTrailingZeros);
        TEST_METHOD(UnitConversionManagerNumberFormattingUtils_GetNumberDigits);
        TEST_METHOD(UnitConversionManagerNumberFormattingUtils_GetNumberDigitsWholeNumberPart);
        TEST_METHOD(UnitConversionManagerNumberFormattingUtils_RoundSignificantDigits);
        TEST_METHOD(UnitConversionManagerNumberFormattingUtils_ToScientificNumber);

        TEST_METHOD(CalculatorManagerTestBinaryOperatorReceived);
        TEST_METHOD(CalculatorManagerTestBinaryOperatorReceived_Multiple);
        TEST_METHOD(CalculatorManagerTestBinaryOperatorReceived_LongInput);

        TEST_METHOD(CalculatorManagerTestStandardOrderOfOperations);

        TEST_METHOD_CLEANUP(Cleanup);

    private:
        static std::shared_ptr<CalculatorManager> m_calculatorManager;
        static std::shared_ptr<IResourceProvider> m_resourceProvider;
        static std::shared_ptr<CalculatorManagerDisplayTester> m_calculatorDisplayTester;
        void ExecuteCommands(Command commands[]);
        void ExecuteCommands(const vector<Command>& commands);

        vector<Command> CommandListFromStringInput(const wstring& input)
        {
            vector<Command> result{};
            for (auto iter = input.begin(); iter != input.end(); iter++)
            {
                wchar_t ch = *iter;
                Command asCommand = Command::CommandNULL;
                if (ch == L'.')
                {
                    asCommand = Command::CommandPNT;
                }
                else if (L'0' <= ch && ch <= L'9')
                {
                    int diff = static_cast<int>(ch) - static_cast<int>(L'0');
                    asCommand = static_cast<Command>(diff + static_cast<int>(Command::Command0));
                }

                if (asCommand != Command::CommandNULL)
                {
                    result.push_back(asCommand);
                }
            }

            return result;
        }

        void TestMaxDigitsReachedScenario(const wstring& constInput)
        {
            CalculatorManagerDisplayTester* pCalculatorDisplay = (CalculatorManagerDisplayTester*)m_calculatorDisplayTester.get();

            // Make sure we're in a clean state.
            VERIFY_ARE_EQUAL(0, pCalculatorDisplay->GetMaxDigitsCalledCount());

            vector<Command> commands = CommandListFromStringInput(constInput);
            VERIFY_IS_FALSE(commands.empty());

            // The last element in the list should always cause MaxDigitsReached
            // Remember the command but remove from the actual input that is sent
            Command finalInput = commands[commands.size() - 1];
            commands.pop_back();
            wstring input = constInput.substr(0, constInput.length() - 1);

            m_calculatorManager->SetStandardMode();
            ExecuteCommands(commands);

            wstring expectedDisplay = input;
            wstring display = pCalculatorDisplay->GetPrimaryDisplay();
            VERIFY_ARE_EQUAL(expectedDisplay, display);

            m_calculatorManager->SendCommand(finalInput);

            // Verify MaxDigitsReached
            display = pCalculatorDisplay->GetPrimaryDisplay();
            VERIFY_ARE_EQUAL(expectedDisplay, display);

            // MaxDigitsReached should have been called once
            VERIFY_IS_LESS_THAN(0, pCalculatorDisplay->GetMaxDigitsCalledCount());
        }
    };
```

#### 3.3.1.1 Out-of-line `TEST_METHOD` definitions, verbatim

##### `CalculatorManagerTestStandard` — `CalculatorManagerTest.cpp:320-402`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestStandard()
    {
        Command commands1[] = { Command::Command1, Command::Command2, Command::Command3, Command::CommandPNT,
                                Command::Command4, Command::Command5, Command::Command6, Command::CommandNULL };
        TestDriver::Test(L"123.456", L"", commands1);

        Command commands2[] = { Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"0 + ", commands2);

        Command commands3[] = { Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(0)", commands3);

        Command commands4[] = { Command::Command2, Command::CommandADD, Command::Command3,   Command::CommandEQU,
                                Command::Command4, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"7", L"4 + 3=", commands4);

        Command commands5[] = { Command::Command4, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"4", L"4=", commands5);

        Command commands6[] = { Command::Command2,    Command::Command5,    Command::Command6,   Command::CommandSQRT,
                                Command::CommandSQRT, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"2", L"\x221A(\x221A(\x221A(256)))", commands6);

        Command commands7[] = { Command::Command3,   Command::CommandSUB, Command::Command6,   Command::CommandEQU,
                                Command::CommandMUL, Command::Command3,   Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"-9", L"-3 \x00D7 3=", commands7);

        Command commands8[] = { Command::Command9,     Command::CommandMUL, Command::Command6,   Command::CommandSUB,
                                Command::CommandCENTR, Command::Command8,   Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"46", L"54 - 8=", commands8);

        Command commands9[] = { Command::Command6, Command::CommandMUL, Command::Command6, Command::CommandPERCENT, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"0.36", L"6 \x00D7 0.06=", commands9);

        Command commands10[] = { Command::Command5, Command::Command0,       Command::CommandADD, Command::Command2,
                                 Command::Command0, Command::CommandPERCENT, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"60", L"50 + 10=", commands10);

        Command commands11[] = { Command::Command4, Command::CommandADD, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"8", L"4 + 4=", commands11);

        Command commands12[] = { Command::Command5, Command::CommandADD, Command::CommandMUL, Command::Command3, Command::CommandNULL };
        TestDriver::Test(L"3", L"5 \x00D7 ", commands12);

        Command commands13[] = { Command::Command1, Command::CommandEXP, Command::CommandSIGN, Command::Command9, Command::Command9,   Command::Command9,
                                 Command::Command9, Command::CommandDIV, Command::Command1,    Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Overflow", L"1.e-9999 \x00F7 ", commands13);

        Command commands14[] = { Command::Command5, Command::Command0,       Command::CommandADD, Command::Command2,
                                 Command::Command0, Command::CommandPERCENT, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"60", L"50 + 10=", commands14);

        Command commands15[] = { Command::Command0, Command::CommandDIV, Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Result is undefined", L"0 \x00F7 ", commands15);

        Command commands16[] = { Command::Command1, Command::CommandDIV, Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Cannot divide by zero", L"1 \x00F7 ", commands16);

        Command commands17[] = { Command::Command1,     Command::Command2, Command::CommandADD, Command::Command5,
                                 Command::CommandCENTR, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"14", L"14 + ", commands17);

        Command commands18[] = { Command::Command1, Command::Command0, Command::Command0, Command::CommandSIGN, Command::CommandREC, Command::CommandNULL };
        TestDriver::Test(L"-0.01", L"1/(-100)", commands18);

        Command commands19[] = { Command::Command1, Command::Command2, Command::Command3, Command::CommandBACK, Command::CommandBACK, Command::CommandNULL };
        TestDriver::Test(L"1", L"", commands19);

        Command commands20[] = { Command::Command1,    Command::Command2,    Command::Command3,   Command::CommandBACK,
                                 Command::CommandBACK, Command::CommandBACK, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands20);

        Command commands21[] = { Command::Command4, Command::CommandSQRT, Command::CommandSUB, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"0 + ", commands21);

        Command commands22[] = { Command::Command1,   Command::Command0, Command::Command2, Command::Command4,   Command::CommandSQRT,
                                 Command::CommandSUB, Command::Command3, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"0 + ", commands22);

        Command commands23[] = { Command::Command2, Command::CommandPNT, Command::Command2, Command::Command5,   Command::CommandSQRT, Command::CommandSUB,
                                 Command::Command1, Command::CommandPNT, Command::Command5, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(2.25) - 1.5=", commands23);
    }
```

##### `CalculatorManagerTestScientific` — `CalculatorManagerTest.cpp:404-498`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestScientific()
    {
        Command commands1[] = { Command::Command1, Command::Command2, Command::Command3, Command::CommandPNT,
                                Command::Command4, Command::Command5, Command::Command6, Command::CommandNULL };
        TestDriver::Test(L"123.456", L"", commands1, true, true);

        Command commands2[] = { Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"0 + ", commands2, true, true);

        Command commands3[] = { Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(0)", commands3, true, true);

        Command commands4[] = { Command::Command1, Command::CommandADD, Command::Command0,   Command::CommandMUL,
                                Command::Command2, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"1", L"1 + 0 \x00D7 2=", commands4, true, true);

        Command commands5[] = { Command::Command4, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"4", L"4=", commands5, true, true);

        Command commands6[] = { Command::Command2,    Command::Command5,    Command::Command6,   Command::CommandSQRT,
                                Command::CommandSQRT, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"2", L"\x221A(\x221A(\x221A(256)))", commands6, true, true);

        Command commands7[] = { Command::Command3,   Command::CommandSUB, Command::Command6,   Command::CommandEQU,
                                Command::CommandMUL, Command::Command3,   Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"-9", L"-3 \x00D7 3 + ", commands7, true, true);

        Command commands8[] = { Command::Command9, Command::CommandMUL, Command::Command6, Command::CommandSUB, Command::CommandCENTR,
                                Command::Command8, Command::CommandMUL, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"38", L"9 \x00D7 6 - 8 \x00D7 2 + ", commands8, true, true);

        Command commands9[] = { Command::Command6, Command::CommandMUL, Command::Command6, Command::CommandSIGN, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"Invalid input", L"6 \x00D7 \x221A(-6)", commands9, true, true);

        Command commands10[] = { Command::Command5, Command::Command0,   Command::CommandADD, Command::Command2,
                                 Command::Command0, Command::CommandREC, Command::CommandSUB, Command::CommandNULL };
        TestDriver::Test(L"50.05", L"50 + 1/(20) - ", commands10, true, true);

        Command commands11[] = { Command::Command4, Command::CommandADD, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"8", L"4 + 4=", commands11, true, true);

        Command commands12[] = { Command::Command5, Command::CommandADD, Command::CommandMUL, Command::Command3, Command::CommandNULL };
        TestDriver::Test(L"3", L"5 \x00D7 ", commands12, true, true);

        Command commands13[] = { Command::Command1, Command::CommandEXP, Command::CommandSIGN, Command::Command9, Command::Command9,   Command::Command9,
                                 Command::Command9, Command::CommandDIV, Command::Command1,    Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Overflow", L"1.e-9999 \x00F7 ", commands13, true, true);

        Command commands14[] = { Command::Command5, Command::Command0,       Command::CommandADD, Command::Command2,
                                 Command::Command0, Command::CommandPERCENT, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"60", L"50 + 10=", commands14, true, true);

        Command commands15[] = { Command::Command0, Command::CommandDIV, Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Result is undefined", L"0 \x00F7 ", commands15, true, true);

        Command commands16[] = { Command::Command1, Command::CommandDIV, Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Cannot divide by zero", L"1 \x00F7 ", commands16, true, true);

        Command commands17[] = { Command::Command1,     Command::Command2, Command::CommandADD, Command::Command5,
                                 Command::CommandCENTR, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"14", L"12 + 2 + ", commands17, true, true);

        Command commands18[] = { Command::Command1, Command::Command0, Command::Command0, Command::CommandSIGN, Command::CommandREC, Command::CommandNULL };
        TestDriver::Test(L"-0.01", L"1/(-100)", commands18, true, true);

        Command commands19[] = { Command::Command1, Command::Command2, Command::Command3, Command::CommandBACK, Command::CommandBACK, Command::CommandNULL };
        TestDriver::Test(L"1", L"", commands19, true, true);

        Command commands20[] = { Command::Command1,    Command::Command2,    Command::Command3,   Command::CommandBACK,
                                 Command::CommandBACK, Command::CommandBACK, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands20, true, true);

        Command commands21[] = { Command::Command4, Command::CommandSQRT, Command::CommandSUB, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(4) - 2 + ", commands21, true, true);

        Command commands22[] = { Command::Command0, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(0)", commands22, true, true);

        Command commands23[] = { Command::Command1,   Command::Command0, Command::Command2, Command::Command4,   Command::CommandSQRT,
                                 Command::CommandSUB, Command::Command3, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(1024) - 32 + ", commands23, true, true);

        Command commands24[] = { Command::Command2,    Command::Command5,    Command::Command7,   Command::CommandSQRT,
                                 Command::CommandSQRT, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"2.0009748976330773374220277351385", L"\x221A(\x221A(\x221A(257)))", commands24, true, true);

        Command commands25[] = { Command::Command2, Command::CommandPNT, Command::Command2, Command::Command5,   Command::CommandSQRT, Command::CommandSUB,
                                 Command::Command1, Command::CommandPNT, Command::Command5, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(2.25) - 1.5=", commands25, true, true);

        Command commands26[] = { Command::CommandOPENP, Command::Command2,   Command::CommandPNT, Command::Command2,   Command::Command5,
                                 Command::CommandSQRT,  Command::CommandDIV, Command::Command1,   Command::CommandPNT, Command::Command5,
                                 Command::CommandCLOSEP, Command::CommandLOG,  Command::CommandNULL };
        TestDriver::Test(L"0", L"log(\x221A(2.25) \x00F7 1.5)", commands26, true, true);
    }
```

##### `CalculatorManagerTestScientific2` — `CalculatorManagerTest.cpp:501-634`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestScientific2()
    {
        Command commands1[] = { Command::Command1, Command::Command2, Command::CommandSQR, Command::CommandNULL };
        TestDriver::Test(L"144", L"sqr(12)", commands1, true, true);

        Command commands2[] = { Command::Command5, Command::CommandFAC, Command::CommandNULL };
        TestDriver::Test(L"120", L"fact(5)", commands2, true, true);

        Command commands3[] = { Command::Command5, Command::CommandPWR, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"25", L"5 ^ 2 + ", commands3, true, true);

        Command commands4[] = { Command::Command8, Command::CommandROOT, Command::Command3, Command::CommandMUL, Command::CommandNULL };
        TestDriver::Test(L"2", L"8 yroot 3 \x00D7 ", commands4, true, true);

        Command commands5[] = { Command::Command8, Command::CommandCUB, Command::CommandNULL };
        TestDriver::Test(L"512", L"cube(8)", commands5, true, true);

        Command commands6[] = { Command::Command8, Command::CommandCUB, Command::CommandCUBEROOT, Command::CommandNULL };
        TestDriver::Test(L"8", L"cuberoot(cube(8))", commands6, true, true);

        Command commands7[] = { Command::Command1, Command::Command0, Command::CommandLOG, Command::CommandNULL };
        TestDriver::Test(L"1", L"log(10)", commands7, true, true);

        Command commands8[] = { Command::Command5, Command::CommandPOW10, Command::CommandNULL };
        TestDriver::Test(L"100,000", L"10^(5)", commands8, true, true);

        Command commands9[] = { Command::Command1, Command::Command0, Command::CommandLN, Command::CommandNULL };
        TestDriver::Test(L"2.3025850929940456840179914546844", L"ln(10)", commands9, true, true);

        Command commands10[] = { Command::Command1, Command::CommandSIN, Command::CommandNULL };
        TestDriver::Test(L"0.01745240643728351281941897851632", L"sin\x2080(1)", commands10, true, true);

        Command commands11[] = { Command::Command1, Command::CommandCOS, Command::CommandNULL };
        TestDriver::Test(L"0.99984769515639123915701155881391", L"cos\x2080(1)", commands11, true, true);

        Command commands12[] = { Command::Command1, Command::CommandTAN, Command::CommandNULL };
        TestDriver::Test(L"0.01745506492821758576512889521973", L"tan\x2080(1)", commands12, true, true);

        Command commands13[] = { Command::Command1, Command::CommandASIN, Command::CommandNULL };
        TestDriver::Test(L"90", L"sin\x2080\x207B\x00B9(1)", commands13, true, true);

        Command commands14[] = { Command::Command1, Command::CommandACOS, Command::CommandNULL };
        TestDriver::Test(L"0", L"cos\x2080\x207B\x00B9(1)", commands14, true, true);

        Command commands15[] = { Command::Command1, Command::CommandATAN, Command::CommandNULL };
        TestDriver::Test(L"45", L"tan\x2080\x207B\x00B9(1)", commands15, true, true);

        Command commands16[] = { Command::Command2, Command::CommandPOWE, Command::CommandNULL };
        TestDriver::Test(L"7.389056098930650227230427460575", L"e^(2)", commands16, true, true);

        Command commands17[] = { Command::Command5, Command::CommandPWR, Command::Command0, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"1", L"5 ^ 0 + ", commands17, true, true);

        Command commands18[] = { Command::Command0, Command::CommandPWR, Command::Command0, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"1", L"0 ^ 0 + ", commands18, true, true);

        Command commands19[] = { Command::Command2, Command::Command7,   Command::CommandSIGN, Command::CommandROOT,
                                 Command::Command3, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"-3", L"-27 yroot 3 + ", commands19, true, true);

        Command commands20[] = { Command::Command8,   Command::CommandPWR, Command::CommandOPENP,  Command::Command2,
                                 Command::CommandDIV, Command::Command3,   Command::CommandCLOSEP, Command::CommandSUB,
                                 Command::Command4,   Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"8 ^ (2 \x00F7 3) - 4 + ", commands20, true, true);

        Command commands21[] = { Command::Command4,   Command::CommandPWR, Command::CommandOPENP,  Command::Command3,
                                 Command::CommandDIV, Command::Command2,   Command::CommandCLOSEP, Command::CommandSUB,
                                 Command::Command8,   Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"4 ^ (3 \x00F7 2) - 8 + ", commands21, true, true);

        Command commands22[] = { Command::Command1, Command::Command0, Command::CommandPWR, Command::Command1, Command::CommandPNT, Command::Command2,
                                 Command::Command3, Command::Command4, Command::Command5,   Command::Command6, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"17.161687912241792074207286679393", L"10 ^ 1.23456 + ", commands22, true, true);

        Command commands23[] = { Command::Command1, Command::CommandSEC, Command::CommandNULL };
        TestDriver::Test(L"1.0001523280439076654284264342126", L"sec\x2080(1)", commands23, true, true);

        Command commands24[] = { Command::Command1, Command::CommandCSC, Command::CommandNULL };
        TestDriver::Test(L"57.298688498550183476612683735174", L"csc\x2080(1)", commands24, true, true);

        Command commands25[] = { Command::Command1, Command::CommandCOT, Command::CommandNULL };
        TestDriver::Test(L"57.289961630759424687278147537113", L"cot\x2080(1)", commands25, true, true);

        Command commands26[] = { Command::Command1, Command::CommandASEC, Command::CommandNULL };
        TestDriver::Test(L"0", L"sec\x2080\x207B\x00B9(1)", commands26, true, true);

        Command commands27[] = { Command::Command1, Command::CommandACSC, Command::CommandNULL };
        TestDriver::Test(L"90", L"csc\x2080\x207B\x00B9(1)", commands27, true, true);

        Command commands28[] = { Command::Command1, Command::CommandACOT, Command::CommandNULL };
        TestDriver::Test(L"45", L"cot\x2080\x207B\x00B9(1)", commands28, true, true);

        Command commands29[] = { Command::Command1, Command::CommandSECH, Command::CommandNULL };
        TestDriver::Test(L"0.64805427366388539957497735322615", L"sech(1)", commands29, true, true);

        Command commands30[] = { Command::Command1, Command::CommandCSCH, Command::CommandNULL };
        TestDriver::Test(L"0.85091812823932154513384276328718", L"csch(1)", commands30, true, true);

        Command commands31[] = { Command::Command1, Command::CommandCOTH, Command::CommandNULL };
        TestDriver::Test(L"1.3130352854993313036361612469308", L"coth(1)", commands31, true, true);

        Command commands32[] = { Command::Command1, Command::CommandASECH, Command::CommandNULL };
        TestDriver::Test(L"0", L"sech\x207B\x00B9(1)", commands32, true, true);

        Command commands33[] = { Command::Command1, Command::CommandACSCH, Command::CommandNULL };
        TestDriver::Test(L"0.88137358701954302523260932497979", L"csch\x207B\x00B9(1)", commands33, true, true);

        Command commands34[] = { Command::Command2, Command::CommandACOTH, Command::CommandNULL };
        TestDriver::Test(L"0.54930614433405484569762261846126", L"coth\x207B\x00B9(2)", commands34, true, true);

        Command commands35[] = { Command::Command8, Command::CommandPOW2, Command::CommandNULL };
        TestDriver::Test(L"256", L"2^(8)", commands35, true, true);

        Command commands36[] = { Command::CommandRand, Command::CommandCeil, Command::CommandNULL };
        TestDriver::Test(L"1", L"N/A", commands36, true, true);

        Command commands37[] = { Command::CommandRand, Command::CommandFloor, Command::CommandNULL };
        TestDriver::Test(L"0", L"N/A", commands37, true, true);

        Command commands38[] = { Command::CommandRand, Command::CommandSIGN, Command::CommandCeil, Command::CommandNULL };
        TestDriver::Test(L"0", L"N/A", commands38, true, true);

        Command commands39[] = { Command::CommandRand, Command::CommandSIGN, Command::CommandFloor, Command::CommandNULL };
        TestDriver::Test(L"-1", L"N/A", commands39, true, true);

        Command commands40[] = { Command::Command3, Command::CommandPNT, Command::Command8, Command::CommandFloor, Command::CommandNULL };
        TestDriver::Test(L"3", L"floor(3.8)", commands40, true, true);

        Command commands41[] = { Command::Command3, Command::CommandPNT, Command::Command8, Command::CommandCeil, Command::CommandNULL };
        TestDriver::Test(L"4", L"ceil(3.8)", commands41, true, true);

        Command commands42[] = { Command::Command5, Command::CommandLogBaseY, Command::Command3, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"1.4649735207179271671970404076786", L"5 log base 3 + ", commands42, true, true);
    }
```

##### `CalculatorManagerTestScientificParenthesis` — `CalculatorManagerTest.cpp:636-679`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestScientificParenthesis()
    {
        Command commands1[] = { Command::Command1, Command::CommandADD,    Command::CommandOPENP, Command::CommandADD,
                                Command::Command3, Command::CommandCLOSEP, Command::CommandNULL };
        TestDriver::Test(L"3", L"1 + (0 + 3)", commands1, true, true);

        Command commands2[] = {
            Command::CommandOPENP, Command::CommandOPENP, Command::Command1, Command::Command2, Command::CommandCLOSEP, Command::CommandNULL
        };
        TestDriver::Test(L"12", L"((12)", commands2, true, true);

        Command commands3[] = { Command::Command1,      Command::Command2,     Command::CommandCLOSEP,
                                Command::CommandCLOSEP, Command::CommandOPENP, Command::CommandNULL };
        TestDriver::Test(L"12", L"12 \x00D7 (", commands3, true, true);

        Command commands4[] = {
            Command::Command2, Command::CommandOPENP, Command::Command2, Command::CommandCLOSEP, Command::CommandADD, Command::CommandNULL
        };
        TestDriver::Test(L"4", L"2 \x00D7 (2) + ", commands4, true, true);

        Command commands5[] = { Command::Command2,   Command::CommandOPENP, Command::Command2,   Command::CommandCLOSEP,
                                Command::CommandADD, Command::CommandEQU,   Command::CommandNULL };
        TestDriver::Test(L"8", L"2 \x00D7 (2) + 4=", commands5, true, true);

        Command commands6[] = { Command::CommandOPENP, Command::Command8, Command::CommandCLOSEP, Command::Command2, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"16", L"(8) \x00D7 2=", commands6, true, true);

        Command commands7[] = { Command::CommandOPENP,  Command::Command7, Command::CommandMUL, Command::Command2,
                                Command::CommandCLOSEP, Command::Command2, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"28", L"(7 \x00D7 2) \x00D7 2=", commands7, true, true);

        Command commands8[] = { Command::CommandOPENP,  Command::Command7, Command::CommandMUL, Command::Command2,
                                Command::CommandCLOSEP, Command::Command2, Command::CommandEQU,
                                Command::CommandOPENP, Command::Command1, Command::Command4, Command::CommandCLOSEP, Command::Command2, Command::CommandEQU, Command::CommandNULL};
        TestDriver::Test(L"28", L"(14) \x00D7 2=", commands8, true, true);

        Command commands9[] = { Command::CommandOPENP, Command::Command8, Command::CommandCLOSEP,
                                Command::Command0, Command::CommandPNT, Command::Command5, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"4", L"(8) \x00D7 0.5=", commands9, true, true);

        Command commands10[] = { Command::CommandOPENP, Command::Command8, Command::CommandCLOSEP,
                                 Command::CommandPNT, Command::Command5, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"4", L"(8) \x00D7 0.5=", commands10, true, true);
    }
```

##### `CalculatorManagerTestScientificError` — `CalculatorManagerTest.cpp:681-702`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestScientificError()
    {
        Command commands1[] = { Command::Command1, Command::CommandDIV, Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Cannot divide by zero", L"1 \x00F7 ", commands1, true, true);
        VERIFY_IS_TRUE(m_calculatorDisplayTester->GetIsError());

        Command commands2[] = { Command::Command2, Command::CommandSIGN, Command::CommandLOG, Command::CommandNULL };
        TestDriver::Test(L"Invalid input", L"log(-2)", commands2, true, true);
        VERIFY_IS_TRUE(m_calculatorDisplayTester->GetIsError());

        Command commands3[] = { Command::Command0, Command::CommandDIV, Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Result is undefined", L"0 \x00F7 ", commands3, true, true);
        VERIFY_IS_TRUE(m_calculatorDisplayTester->GetIsError());

        // Do the same tests for the basic calculator
        TestDriver::Test(L"Cannot divide by zero", L"1 \x00F7 ", commands1);
        VERIFY_IS_TRUE(m_calculatorDisplayTester->GetIsError());
        TestDriver::Test(L"Invalid input", L"log(-2)", commands2);
        VERIFY_IS_TRUE(m_calculatorDisplayTester->GetIsError());
        TestDriver::Test(L"Result is undefined", L"0 \x00F7 ", commands3);
        VERIFY_IS_TRUE(m_calculatorDisplayTester->GetIsError());
    }
```

##### `CalculatorManagerTestScientificModeChange` — `CalculatorManagerTest.cpp:705-724`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestScientificModeChange()
    {
        Command commands1[] = { Command::CommandRAD, Command::CommandPI, Command::CommandSIN, Command::CommandNULL };
        TestDriver::Test(L"0", L"N/A", commands1, true, true);

        Command commands2[] = { Command::CommandRAD, Command::CommandPI, Command::CommandCOS, Command::CommandNULL };
        TestDriver::Test(L"-1", L"N/A", commands2, true, true);

        Command commands3[] = { Command::CommandRAD, Command::CommandPI, Command::CommandTAN, Command::CommandNULL };
        TestDriver::Test(L"0", L"N/A", commands3, true, true);

        Command commands4[] = { Command::CommandGRAD, Command::Command4, Command::Command0, Command::Command0, Command::CommandSIN, Command::CommandNULL };
        TestDriver::Test(L"0", L"N/A", commands4, true, true);

        Command commands5[] = { Command::CommandGRAD, Command::Command4, Command::Command0, Command::Command0, Command::CommandCOS, Command::CommandNULL };
        TestDriver::Test(L"1", L"N/A", commands5, true, true);

        Command commands6[] = { Command::CommandGRAD, Command::Command4, Command::Command0, Command::Command0, Command::CommandTAN, Command::CommandNULL };
        TestDriver::Test(L"0", L"N/A", commands6, true, true);
    }
```

##### `CalculatorManagerTestModeChange` — `CalculatorManagerTest.cpp:726-751`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestModeChange()
    {
        Command commands1[] = { Command::Command1, Command::Command2, Command::Command3, Command::CommandNULL };
        TestDriver::Test(L"123", L"", commands1, true, false);

        Command commands2[] = { Command::ModeScientific, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands2, true, false);

        Command commands3[] = { Command::Command1, Command::Command2, Command::Command3, Command::CommandNULL };
        TestDriver::Test(L"123", L"", commands3, true, false);

        Command commands4[] = { Command::ModeProgrammer, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands4, true, false);

        Command commands5[] = { Command::Command1, Command::Command2, Command::Command3, Command::CommandNULL };
        TestDriver::Test(L"123", L"", commands5, true, false);

        Command commands6[] = { Command::ModeScientific, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands6, true, false);

        Command commands7[] = { Command::Command6, Command::Command7, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"67", L"67 + ", commands7, true, false);

        Command commands8[] = { Command::ModeBasic, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands8, true, false);
    }
```

##### `CalculatorManagerTestProgrammer` — `CalculatorManagerTest.cpp:753-813`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestProgrammer()
    {
        Command commands1[] = { Command::ModeProgrammer, Command::Command5, Command::Command3,   Command::CommandNand,
                                Command::Command8,       Command::Command3, Command::CommandAnd, Command::CommandNULL };
        TestDriver::Test(L"-18", L"53 NAND 83 AND ", commands1, true, false);

        Command commands2[] = { Command::ModeProgrammer, Command::Command5, Command::Command3,   Command::CommandNor,
                                Command::Command8,       Command::Command3, Command::CommandAnd, Command::CommandNULL };
        TestDriver::Test(L"-120", L"53 NOR 83 AND ", commands2, true, false);

        Command commands3[] = {
            Command::ModeProgrammer, Command::Command5, Command::CommandLSHF, Command::Command1, Command::CommandAnd, Command::CommandNULL
        };
        TestDriver::Test(L"10", L"5 Lsh 1 AND ", commands3, true, false);

        Command commands5[] = {
            Command::ModeProgrammer, Command::Command5, Command::CommandRSHFL, Command::Command1, Command::CommandAnd, Command::CommandNULL
        };
        TestDriver::Test(L"2", L"5 Rsh 1 AND ", commands5, true, false);

        Command commands6[] = { Command::ModeProgrammer, Command::CommandBINPOS63, Command::CommandRSHF, Command::Command5,
                                Command::Command6,       Command::CommandAnd,      Command::CommandNULL };
        TestDriver::Test(L"-128", L"-9223372036854775808 Rsh 56 AND ", commands6, true, false);

        Command commands7[] = { Command::ModeProgrammer, Command::Command1, Command::CommandROL, Command::CommandNULL };
        TestDriver::Test(L"2", L"RoL(1)", commands7, true, false);

        Command commands8[] = { Command::ModeProgrammer, Command::Command1, Command::CommandROR, Command::CommandNULL };
        TestDriver::Test(L"-9,223,372,036,854,775,808", L"RoR(1)", commands8, true, false);

        Command commands9[] = { Command::ModeProgrammer, Command::Command1, Command::CommandRORC, Command::CommandNULL };
        TestDriver::Test(L"0", L"RoR(1)", commands9, true, false);

        Command commands10[] = { Command::ModeProgrammer, Command::Command1, Command::CommandRORC, Command::CommandRORC, Command::CommandNULL };
        TestDriver::Test(L"-9,223,372,036,854,775,808", L"RoR(RoR(1))", commands10, true, false);

        Command commands11[] = { Command::ModeProgrammer, Command::CommandDec, Command::Command4, Command::Command2, Command::Command9,   Command::Command4,
                                Command::Command9,       Command::Command6,   Command::Command7, Command::Command2, Command::Command9,   Command::Command6,
                                Command::CommandDIV,     Command::Command2,   Command::Command5, Command::Command5, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"16,843,009", L"4294967296 \x00F7 255=", commands11, true, false);

        Command commands12[] = {
            Command::ModeProgrammer, Command::CommandDec, Command::Command4, Command::Command2, Command::Command9,   Command::Command4,
            Command::Command9,       Command::Command6,   Command::Command7, Command::Command3, Command::Command0,   Command::Command3,
            Command::CommandDIV,     Command::Command2,   Command::Command5, Command::Command5, Command::CommandEQU, Command::CommandNULL
        };
        TestDriver::Test(L"16,843,009", L"4294967303 \x00F7 255=", commands12, true, false);

        Command commands13[] = {
            Command::ModeProgrammer, Command::CommandDec, Command::Command1, Command::Command0, Command::Command0, Command::Command0,
            Command::Command0, Command::Command0, Command::Command0, Command::Command0, Command::Command0, Command::Command0, Command::CommandDIV,
            Command::Command6, Command::Command4, Command::Command4, Command::Command8, Command::Command7, Command::CommandEQU, Command::CommandNULL
        };
        TestDriver::Test(L"15,507", L"1000000000 \x00F7 64487=", commands13, true, false);

        Command commands14[] = { Command::ModeProgrammer, Command::CommandDec, Command::Command1,   Command::Command0,   Command::Command0,
                                 Command::Command0,       Command::Command0,   Command::Command0,   Command::Command0,   Command::Command0,
                                 Command::Command0,       Command::Command0,   Command::CommandDIV, Command::Command6,   Command::Command4,
                                 Command::Command4,       Command::Command8,   Command::Command8,   Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"15,506", L"1000000000 \x00F7 64488=", commands14, true, false);
    }
```

##### `CalculatorManagerTestMemory` — `CalculatorManagerTest.cpp:815-960`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestMemory()
    {
        Command scientificCalculatorTest52[] = { Command::Command1, Command::CommandSTORE, Command::CommandNULL };
        wstring expectedPrimaryDisplayTestScientific52(L"1");
        wstring expectedExpressionDisplayTestScientific52(L"");

        Command scientificCalculatorTest53[] = { Command::Command1, Command::CommandNULL };
        wstring expectedPrimaryDisplayTestScientific53(L"1");
        wstring expectedExpressionDisplayTestScientific53(L"");

        CalculatorManagerDisplayTester* pCalculatorDisplay = (CalculatorManagerDisplayTester*)m_calculatorDisplayTester.get();
        wstring resultPrimary = L"";
        wstring resultExpression = L"";

        Cleanup();
        ExecuteCommands(scientificCalculatorTest52);
        resultPrimary = pCalculatorDisplay->GetPrimaryDisplay();
        resultExpression = pCalculatorDisplay->GetExpression();
        VERIFY_ARE_EQUAL(expectedPrimaryDisplayTestScientific52, resultPrimary);

        Cleanup();
        ExecuteCommands(scientificCalculatorTest53);
        m_calculatorManager->MemorizeNumber();
        m_calculatorManager->SendCommand(Command::CommandCLEAR);
        m_calculatorManager->MemorizedNumberLoad(0);
        resultPrimary = pCalculatorDisplay->GetPrimaryDisplay();
        resultExpression = pCalculatorDisplay->GetExpression();
        VERIFY_ARE_EQUAL(expectedPrimaryDisplayTestScientific52, resultPrimary);

        Cleanup();
        m_calculatorManager->SendCommand(Command::Command1);
        m_calculatorManager->MemorizeNumber();
        m_calculatorManager->SendCommand(Command::CommandCLEAR);
        m_calculatorManager->SendCommand(Command::Command2);
        m_calculatorManager->MemorizeNumber();
        m_calculatorManager->SendCommand(Command::CommandCLEAR);
        m_calculatorManager->MemorizedNumberLoad(1);
        resultPrimary = pCalculatorDisplay->GetPrimaryDisplay();
        VERIFY_ARE_EQUAL(wstring(L"1"), resultPrimary);

        m_calculatorManager->MemorizedNumberLoad(0);
        resultPrimary = pCalculatorDisplay->GetPrimaryDisplay();
        VERIFY_ARE_EQUAL(wstring(L"2"), resultPrimary);

        Cleanup();
        m_calculatorManager->SendCommand(Command::Command1);
        m_calculatorManager->SendCommand(Command::CommandSIGN);
        m_calculatorManager->MemorizeNumber();
        m_calculatorManager->SendCommand(Command::CommandADD);
        m_calculatorManager->SendCommand(Command::Command2);
        m_calculatorManager->SendCommand(Command::CommandEQU);
        m_calculatorManager->MemorizeNumber();
        m_calculatorManager->SendCommand(Command::CommandMUL);
        m_calculatorManager->SendCommand(Command::Command2);
        m_calculatorManager->MemorizeNumber();

        vector<wstring> memorizedNumbers = pCalculatorDisplay->GetMemorizedNumbers();

        vector<wstring> expectedMemorizedNumbers;
        expectedMemorizedNumbers.push_back(L"2");
        expectedMemorizedNumbers.push_back(L"1");
        expectedMemorizedNumbers.push_back(L"-1");

        bool isEqual = false;
        if (memorizedNumbers.size() < expectedMemorizedNumbers.size())
        {
            isEqual = std::equal(memorizedNumbers.begin(), memorizedNumbers.end(), expectedMemorizedNumbers.begin());
        }
        else
        {
            isEqual = std::equal(expectedMemorizedNumbers.begin(), expectedMemorizedNumbers.end(), memorizedNumbers.begin());
        }
        VERIFY_IS_TRUE(isEqual);

        m_calculatorManager->SendCommand(Command::CommandCLEAR);
        m_calculatorManager->SendCommand(Command::Command2);
        m_calculatorManager->MemorizedNumberAdd(0);
        m_calculatorManager->MemorizedNumberAdd(1);
        m_calculatorManager->MemorizedNumberAdd(2);

        memorizedNumbers = pCalculatorDisplay->GetMemorizedNumbers();

        expectedMemorizedNumbers.clear();
        expectedMemorizedNumbers.push_back(L"4");
        expectedMemorizedNumbers.push_back(L"3");
        expectedMemorizedNumbers.push_back(L"1");

        if (memorizedNumbers.size() < expectedMemorizedNumbers.size())
        {
            isEqual = std::equal(memorizedNumbers.begin(), memorizedNumbers.end(), expectedMemorizedNumbers.begin());
        }
        else
        {
            isEqual = std::equal(expectedMemorizedNumbers.begin(), expectedMemorizedNumbers.end(), memorizedNumbers.begin());
        }
        VERIFY_IS_TRUE(isEqual);

        m_calculatorManager->SendCommand(Command::CommandCLEAR);
        m_calculatorManager->SendCommand(Command::Command1);
        m_calculatorManager->SendCommand(Command::CommandPNT);
        m_calculatorManager->SendCommand(Command::Command5);

        m_calculatorManager->MemorizedNumberSubtract(0);
        m_calculatorManager->MemorizedNumberSubtract(1);
        m_calculatorManager->MemorizedNumberSubtract(2);

        memorizedNumbers = pCalculatorDisplay->GetMemorizedNumbers();

        expectedMemorizedNumbers.clear();
        expectedMemorizedNumbers.push_back(L"2.5");
        expectedMemorizedNumbers.push_back(L"1.5");
        expectedMemorizedNumbers.push_back(L"-0.5");

        if (memorizedNumbers.size() < expectedMemorizedNumbers.size())
        {
            isEqual = std::equal(memorizedNumbers.begin(), memorizedNumbers.end(), expectedMemorizedNumbers.begin());
        }
        else
        {
            isEqual = std::equal(expectedMemorizedNumbers.begin(), expectedMemorizedNumbers.end(), memorizedNumbers.begin());
        }
        VERIFY_IS_TRUE(isEqual);

        // Memorizing 101 numbers, which exceeds the limit.
        Cleanup();
        for (int i = 0; i < 101; i++)
        {
            m_calculatorManager->SendCommand(Command::Command1);
            m_calculatorManager->MemorizeNumber();
        }

        memorizedNumbers = pCalculatorDisplay->GetMemorizedNumbers();
        VERIFY_ARE_EQUAL((size_t)100, memorizedNumbers.size());

        // Memorizing new number, which should show up at the top of the memory
        m_calculatorManager->SendCommand(Command::Command2);
        m_calculatorManager->MemorizeNumber();
        memorizedNumbers = pCalculatorDisplay->GetMemorizedNumbers();
        VERIFY_ARE_EQUAL(wstring(L"2"), memorizedNumbers.at(0));

        // Test for trying to memorize invalid value
        m_calculatorManager->SendCommand(Command::Command2);
        m_calculatorManager->SendCommand(Command::CommandSIGN);
        m_calculatorManager->SendCommand(Command::CommandSQRT);
        m_calculatorManager->MemorizeNumber();
    }
```

##### `CalculatorManagerTestMaxDigitsReached` — `CalculatorManagerTest.cpp:963-966`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestMaxDigitsReached()
    {
        TestMaxDigitsReachedScenario(L"1,234,567,891,011,1213");
    }
```

##### `CalculatorManagerTestMaxDigitsReached_LeadingDecimal` — `CalculatorManagerTest.cpp:968-971`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestMaxDigitsReached_LeadingDecimal()
    {
        TestMaxDigitsReachedScenario(L"0.12345678910111213");
    }
```

##### `CalculatorManagerTestMaxDigitsReached_TrailingDecimal` — `CalculatorManagerTest.cpp:973-976`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestMaxDigitsReached_TrailingDecimal()
    {
        TestMaxDigitsReachedScenario(L"123,456,789,101,112.13");
    }
```

##### `UnitConversionManagerNumberFormattingUtils_TrimTrailingZeros` — `CalculatorManagerTest.cpp:978-1001`

```cpp
    void CalculatorManagerTest::UnitConversionManagerNumberFormattingUtils_TrimTrailingZeros()
    {
        wstring number = L"2.1032100000000";
        TrimTrailingZeros(number);
        VERIFY_ARE_EQUAL(number, L"2.10321");
        number = L"-122.123200";
        TrimTrailingZeros(number);
        VERIFY_ARE_EQUAL(number, L"-122.1232");
        number = L"0.0001200";
        TrimTrailingZeros(number);
        VERIFY_ARE_EQUAL(number, L"0.00012");
        number = L"12.000";
        TrimTrailingZeros(number);
        VERIFY_ARE_EQUAL(number, L"12");
        number = L"-12.00000";
        TrimTrailingZeros(number);
        VERIFY_ARE_EQUAL(number, L"-12");
        number = L"0.000";
        TrimTrailingZeros(number);
        VERIFY_ARE_EQUAL(number, L"0");
        number = L"322423";
        TrimTrailingZeros(number);
        VERIFY_ARE_EQUAL(number, L"322423");
    }
```

##### `UnitConversionManagerNumberFormattingUtils_GetNumberDigits` — `CalculatorManagerTest.cpp:1003-1020`

```cpp
    void CalculatorManagerTest::UnitConversionManagerNumberFormattingUtils_GetNumberDigits()
    {
        wstring number = L"2.10321";
        unsigned int digitsCount = GetNumberDigits(number);
        VERIFY_ARE_EQUAL(digitsCount, 6);
        number = L"-122.1232";
        digitsCount = GetNumberDigits(number);
        VERIFY_ARE_EQUAL(digitsCount, 7);
        number = L"-3432";
        digitsCount = GetNumberDigits(number);
        VERIFY_ARE_EQUAL(digitsCount, 4);
        number = L"0";
        digitsCount = GetNumberDigits(number);
        VERIFY_ARE_EQUAL(digitsCount, 1);
        number = L"0.0001223";
        digitsCount = GetNumberDigits(number);
        VERIFY_ARE_EQUAL(digitsCount, 8);
    }
```

##### `UnitConversionManagerNumberFormattingUtils_GetNumberDigitsWholeNumberPart` — `CalculatorManagerTest.cpp:1022-1040`

```cpp
    void CalculatorManagerTest::UnitConversionManagerNumberFormattingUtils_GetNumberDigitsWholeNumberPart()
    {
        unsigned int digitsCount = GetNumberDigitsWholeNumberPart(2.10321);
        VERIFY_ARE_EQUAL(digitsCount, 1);
        digitsCount = GetNumberDigitsWholeNumberPart(-122.1232);
        VERIFY_ARE_EQUAL(digitsCount, 3);
        digitsCount = GetNumberDigitsWholeNumberPart(-3432);
        VERIFY_ARE_EQUAL(digitsCount, 4);
        digitsCount = GetNumberDigitsWholeNumberPart(0);
        VERIFY_ARE_EQUAL(digitsCount, 1);
        digitsCount = GetNumberDigitsWholeNumberPart(324328412837382);
        VERIFY_ARE_EQUAL(digitsCount, 15);
        digitsCount = GetNumberDigitsWholeNumberPart(324328412837382.232213214324234);
        VERIFY_ARE_EQUAL(digitsCount, 15);
        digitsCount = GetNumberDigitsWholeNumberPart(0.032);
        VERIFY_ARE_EQUAL(digitsCount, 1);
        digitsCount = GetNumberDigitsWholeNumberPart(0.00000000000000000001);
        VERIFY_ARE_EQUAL(digitsCount, 1);
    }
```

##### `UnitConversionManagerNumberFormattingUtils_RoundSignificantDigits` — `CalculatorManagerTest.cpp:1042-1056`

```cpp
    void CalculatorManagerTest::UnitConversionManagerNumberFormattingUtils_RoundSignificantDigits()
    {
        wstring result = RoundSignificantDigits(12.342343242, 3);
        VERIFY_ARE_EQUAL(result, L"12.342");
        result = RoundSignificantDigits(12.3429999, 3);
        VERIFY_ARE_EQUAL(result, L"12.343");
        result = RoundSignificantDigits(12.342500001, 3);
        VERIFY_ARE_EQUAL(result, L"12.343");
        result = RoundSignificantDigits(-2312.1244243346454345, 5);
        VERIFY_ARE_EQUAL(result, L"-2312.12442");
        result = RoundSignificantDigits(0.3423432423, 5);
        VERIFY_ARE_EQUAL(result, L"0.34234");
        result = RoundSignificantDigits(0.3423, 7);
        VERIFY_ARE_EQUAL(result, L"0.3423000");
    }
```

##### `UnitConversionManagerNumberFormattingUtils_ToScientificNumber` — `CalculatorManagerTest.cpp:1058-1076`

```cpp
    void CalculatorManagerTest::UnitConversionManagerNumberFormattingUtils_ToScientificNumber()
    {
        wstring result = ToScientificNumber(3423);
        VERIFY_ARE_EQUAL(result, L"3.423000e+03");
        result = ToScientificNumber(-21);
        VERIFY_ARE_EQUAL(result, L"-2.100000e+01");
        result = ToScientificNumber(0.0232);
        VERIFY_ARE_EQUAL(result, L"2.320000e-02");
        result = ToScientificNumber(-0.00921);
        VERIFY_ARE_EQUAL(result, L"-9.210000e-03");
        result = ToScientificNumber(2343243345677);
        VERIFY_ARE_EQUAL(result, L"2.343243e+12");
        result = ToScientificNumber(-3432474247332942);
        VERIFY_ARE_EQUAL(result, L"-3.432474e+15");
        result = ToScientificNumber(0.000000003432432);
        VERIFY_ARE_EQUAL(result, L"3.432432e-09");
        result = ToScientificNumber(-0.000000003432432);
        VERIFY_ARE_EQUAL(result, L"-3.432432e-09");
    }
```

##### `CalculatorManagerTestBinaryOperatorReceived` — `CalculatorManagerTest.cpp:1078-1092`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestBinaryOperatorReceived()
    {
        CalculatorManagerDisplayTester* pCalculatorDisplay = (CalculatorManagerDisplayTester*)m_calculatorDisplayTester.get();

        VERIFY_ARE_EQUAL(0, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());

        m_calculatorManager->SetStandardMode();
        ExecuteCommands({ Command::Command1, Command::CommandADD });

        wstring display = pCalculatorDisplay->GetPrimaryDisplay();
        VERIFY_ARE_EQUAL(L"1", display);

        // Verify BinaryOperatorReceived
        VERIFY_ARE_EQUAL(1, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());
    }
```

##### `CalculatorManagerTestBinaryOperatorReceived_Multiple` — `CalculatorManagerTest.cpp:1094-1108`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestBinaryOperatorReceived_Multiple()
    {
        CalculatorManagerDisplayTester* pCalculatorDisplay = (CalculatorManagerDisplayTester*)m_calculatorDisplayTester.get();

        VERIFY_ARE_EQUAL(0, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());

        m_calculatorManager->SetStandardMode();
        ExecuteCommands({ Command::Command1, Command::CommandADD, Command::CommandSUB, Command::CommandMUL });

        wstring display = pCalculatorDisplay->GetPrimaryDisplay();
        VERIFY_ARE_EQUAL(L"1", display);

        // Verify BinaryOperatorReceived
        VERIFY_ARE_EQUAL(3, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());
    }
```

##### `CalculatorManagerTestBinaryOperatorReceived_LongInput` — `CalculatorManagerTest.cpp:1110-1134`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestBinaryOperatorReceived_LongInput()
    {
        CalculatorManagerDisplayTester* pCalculatorDisplay = (CalculatorManagerDisplayTester*)m_calculatorDisplayTester.get();

        VERIFY_ARE_EQUAL(0, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());

        m_calculatorManager->SetStandardMode();
        ExecuteCommands({ Command::Command1,
                          Command::CommandADD,
                          Command::Command2,
                          Command::CommandMUL,
                          Command::Command1,
                          Command::Command0,
                          Command::CommandSUB,
                          Command::Command5,
                          Command::CommandDIV,
                          Command::Command5,
                          Command::CommandEQU });

        wstring display = pCalculatorDisplay->GetPrimaryDisplay();
        VERIFY_ARE_EQUAL(L"5", display);

        // Verify BinaryOperatorReceived
        VERIFY_ARE_EQUAL(4, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());
    }
```

##### `CalculatorManagerTestStandardOrderOfOperations` — `CalculatorManagerTest.cpp:1136-1211`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestStandardOrderOfOperations()
    {
        Command commands1[] = { Command::Command1, Command::CommandREC, Command::CommandNULL };
        TestDriver::Test(L"1", L"1/(1)", commands1);

        Command commands2[] = { Command::Command4, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"2", L"\x221A(4)", commands2);

        Command commands3[] = { Command::Command1, Command::CommandADD, Command::Command4, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"2", L"1 + \x221A(4)", commands3);

        Command commands4[] = { Command::Command1, Command::CommandADD, Command::Command4, Command::CommandSQRT, Command::CommandSUB, Command::CommandNULL };
        TestDriver::Test(L"3", L"3 - ", commands4);

        Command commands5[] = { Command::Command2, Command::CommandMUL, Command::Command4, Command::CommandREC, Command::CommandNULL };
        TestDriver::Test(L"0.25", L"2 \x00D7 1/(4)", commands5);

        Command commands6[] = { Command::Command5, Command::CommandDIV, Command::Command6, Command::CommandPERCENT, Command::CommandNULL };
        TestDriver::Test(L"0.06", L"5 \x00F7 0.06", commands6);

        Command commands7[] = { Command::Command4, Command::CommandSQRT, Command::CommandSUB, Command::CommandNULL };
        TestDriver::Test(L"2", L"\x221A(4) - ", commands7);

        Command commands8[] = { Command::Command7, Command::CommandSQR, Command::CommandDIV, Command::CommandNULL };
        TestDriver::Test(L"49", L"sqr(7) \x00F7 ", commands8);

        Command commands9[] = { Command::Command8, Command::CommandSQR, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"8", L"\x221A(sqr(8))", commands9);

        Command commands10[] = { Command::Command1, Command::Command0, Command::CommandADD, Command::Command2, Command::CommandSUB, Command::CommandNULL };
        TestDriver::Test(L"12", L"12 - ", commands10);

        Command commands11[] = { Command::Command3, Command::CommandMUL, Command::Command4, Command::CommandDIV, Command::CommandNULL };
        TestDriver::Test(L"12", L"12 \x00F7 ", commands11);

        Command commands12[] = { Command::Command6, Command::CommandDIV, Command::Command3, Command::CommandSUB, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"2", L"2 + ", commands12);

        Command commands13[] = { Command::Command7, Command::CommandSUB, Command::Command4, Command::CommandDIV, Command::CommandMUL, Command::CommandNULL };
        TestDriver::Test(L"3", L"3 \x00D7 ", commands13);

        Command commands14[] = { Command::Command8, Command::CommandMUL, Command::Command2, Command::CommandADD, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"4", L"16 + \x221A(16)", commands14);

        Command commands15[] = { Command::Command9, Command::CommandADD, Command::Command0, Command::CommandMUL, Command::CommandSIGN, Command::CommandNULL };
        TestDriver::Test(L"-9", L"9 \x00D7 negate(9)", commands15);

        Command commands16[] = { Command::Command9, Command::CommandSIGN, Command::Command0, Command::CommandADD, Command::CommandMUL, Command::CommandNULL };
        TestDriver::Test(L"-90", L"-90 \x00D7 ", commands16);

        Command commands17[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"3", L"1 + 2=", commands17);

        Command commands18[] = { Command::Command2, Command::Command0,   Command::CommandMUL, Command::Command0,
                                 Command::Command2, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"40", L"20 \x00D7 2=", commands18);

        Command commands19[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandADD, Command::CommandBACK, Command::CommandNULL };
        TestDriver::Test(L"3", L"3 + ", commands19);

        Command commands20[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandADD, Command::CommandCLEAR, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands20);

        Command commands21[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandADD, Command::CommandCENTR, Command::CommandNULL };
        TestDriver::Test(L"0", L"3 + ", commands21);

        Command commands22[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandCLEAR, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands22);

        Command commands23[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandCENTR, Command::CommandNULL };
        TestDriver::Test(L"0", L"1 + ", commands23);

        Command commands24[] = { Command::Command1, Command::CommandMUL, Command::Command2, Command::CommandMUL, Command::Command3,   Command::CommandMUL,
                                 Command::Command4, Command::CommandMUL, Command::Command5, Command::CommandMUL, Command::CommandNULL };
        TestDriver::Test(L"120", L"120 \x00D7 ", commands24);
    }
```

#### 3.3.Z Remaining non-blank source not inside a `TEST_CLASS` or `TEST_METHOD` (fixture glue)

<!-- CalculatorManagerTest.cpp:275-279 -->
```cpp
    std::shared_ptr<CalculatorManager> CalculatorManagerTest::m_calculatorManager;
    std::shared_ptr<CalculatorManagerDisplayTester> CalculatorManagerTest::m_calculatorDisplayTester;
    std::shared_ptr<IResourceProvider> CalculatorManagerTest::m_resourceProvider;
    std::shared_ptr<CalculatorManagerDisplayTester> TestDriver::m_displayTester;
    std::shared_ptr<CalculatorManager> TestDriver::m_calculatorManager;
```

<!-- CalculatorManagerTest.cpp:281-288 -->
```cpp
    // Creates instance of CalculationManager before running tests
    void CalculatorManagerTest::CommonSetup()
    {
        m_calculatorDisplayTester = std::make_shared<CalculatorManagerDisplayTester>();
        m_resourceProvider = std::make_shared<EngineResourceProvider>();
        m_calculatorManager = std::make_shared<CalculatorManager>(m_calculatorDisplayTester.get(), m_resourceProvider.get());
        TestDriver::Initialize(m_calculatorDisplayTester, m_calculatorManager);
    }
```

<!-- CalculatorManagerTest.cpp:290-295 -->
```cpp
    // Resets calculator state to start state after each test
    void CalculatorManagerTest::Cleanup()
    {
        m_calculatorManager->Reset();
        m_calculatorDisplayTester->Reset();
    }
```

<!-- CalculatorManagerTest.cpp:297-305 -->
```cpp
    void CalculatorManagerTest::ExecuteCommands(Command commands[])
    {
        Command* itr = commands;
        while (*itr != Command::CommandNULL)
        {
            m_calculatorManager->SendCommand(*itr);
            itr++;
        }
    }
```

<!-- CalculatorManagerTest.cpp:307-314 -->
```cpp
    void CalculatorManagerTest::ExecuteCommands(const vector<Command>& commands)
    {
        for (const Command& command : commands)
        {
            if (command == Command::CommandNULL)
            {
                break;
            }
```

<!-- CalculatorManagerTest.cpp:316-318 -->
```cpp
            m_calculatorManager->SendCommand(command);
        }
    }
```

<!-- CalculatorManagerTest.cpp:500-500 -->
```cpp
    // Scientific functions from the scientific calculator
```

<!-- CalculatorManagerTest.cpp:704-704 -->
```cpp
    // Radians and Grads Test
```

<!-- CalculatorManagerTest.cpp:962-962 -->
```cpp
    // Send 12345678910111213 and verify MaxDigitsReached
```

<!-- CalculatorManagerTest.cpp:1212-1212 -->
```cpp
} /* namespace CalculationManagerUnitTests */
```

### 3.4 `src/CalculatorUnitTests/RationalTest.cpp` — `TEST_CLASS(RationalTest)`

8 test methods.

| # | `TEST_METHOD` | Kind | Source lines |
|---|---|---|---|
| 1 | `TestModuloOperandsNotModified` | inline in class | `RationalTest.cpp:18-41` |
| 2 | `TestModuloInteger` | inline in class | `RationalTest.cpp:43-60` |
| 3 | `TestModuloZero` | inline in class | `RationalTest.cpp:62-75` |
| 4 | `TestModuloRational` | inline in class | `RationalTest.cpp:77-100` |
| 5 | `TestRemainderOperandsNotModified` | inline in class | `RationalTest.cpp:102-125` |
| 6 | `TestRemainderInteger` | inline in class | `RationalTest.cpp:127-144` |
| 7 | `TestRemainderZero` | inline in class | `RationalTest.cpp:146-189` |
| 8 | `TestRemainderRational` | inline in class | `RationalTest.cpp:191-218` |

#### 3.4.0 File preamble, `using` directives and fixture helpers (`RationalTest.cpp:1-14`)

```cpp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>
#include "Header Files/Rational.h"
#include "Header Files/RationalMath.h"

using namespace CalcEngine;
using namespace CalcEngine::RationalMath;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CalculatorEngineTests
{
```

#### 3.4.1 `TEST_CLASS` block verbatim (`RationalTest.cpp:15-219`)

```cpp
    TEST_CLASS(RationalTest){ public: TEST_CLASS_INITIALIZE(CommonSetup){ ChangeConstants(10, 128);
}

TEST_METHOD(TestModuloOperandsNotModified)
{
    // Verify results but also check that operands are not modified
    Rational rat25(25);
    Rational ratminus25(-25);
    Rational rat4(4);
    Rational ratminus4(-4);
    Rational res = Mod(rat25, rat4);
    VERIFY_ARE_EQUAL(res, 1);
    VERIFY_ARE_EQUAL(rat25, 25);
    VERIFY_ARE_EQUAL(rat4, 4);
    res = Mod(rat25, ratminus4);
    VERIFY_ARE_EQUAL(res, -3);
    VERIFY_ARE_EQUAL(rat25, 25);
    VERIFY_ARE_EQUAL(ratminus4, -4);
    res = Mod(ratminus25, ratminus4);
    VERIFY_ARE_EQUAL(res, -1);
    VERIFY_ARE_EQUAL(ratminus25, -25);
    VERIFY_ARE_EQUAL(ratminus4, -4);
    res = Mod(ratminus25, rat4);
    VERIFY_ARE_EQUAL(res, 3);
    VERIFY_ARE_EQUAL(ratminus25, -25);
    VERIFY_ARE_EQUAL(rat4, 4);
}

TEST_METHOD(TestModuloInteger)
{
    // Check with integers
    auto res = Mod(Rational(426), Rational(56478));
    VERIFY_ARE_EQUAL(res, 426);
    res = Mod(Rational(56478), Rational(426));
    VERIFY_ARE_EQUAL(res, 246);
    res = Mod(Rational(-643), Rational(8756));
    VERIFY_ARE_EQUAL(res, 8113);
    res = Mod(Rational(643), Rational(-8756));
    VERIFY_ARE_EQUAL(res, -8113);
    res = Mod(Rational(-643), Rational(-8756));
    VERIFY_ARE_EQUAL(res, -643);
    res = Mod(Rational(1000), Rational(250));
    VERIFY_ARE_EQUAL(res, 0);
    res = Mod(Rational(1000), Rational(-250));
    VERIFY_ARE_EQUAL(res, 0);
}

TEST_METHOD(TestModuloZero)
{
    // Test with Zero
    auto res = Mod(Rational(343654332), Rational(0));
    VERIFY_ARE_EQUAL(res, 343654332);
    res = Mod(Rational(0), Rational(8756));
    VERIFY_ARE_EQUAL(res, 0);
    res = Mod(Rational(0), Rational(-242));
    VERIFY_ARE_EQUAL(res, 0);
    res = Mod(Rational(0), Rational(0));
    VERIFY_ARE_EQUAL(res, 0);
    res = Mod(Rational(Number(1, 0, { 23242 }), Number(1, 0, { 2 })), Rational(Number(1, 0, { 0 }), Number(1, 0, { 23 })));
    VERIFY_ARE_EQUAL(res, 11621);
}

TEST_METHOD(TestModuloRational)
{
    // Test with rational numbers
    auto res = Mod(Rational(Number(1, 0, { 250 }), Number(1, 0, { 100 })), Rational(89));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"2.5");
    res = Mod(Rational(Number(1, 0, { 3330 }), Number(1, 0, { 1332 })), Rational(1));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"0.5");
    res = Mod(Rational(Number(1, 0, { 12250 }), Number(1, 0, { 100 })), Rational(10));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"2.5");
    res = Mod(Rational(Number(-1, 0, { 12250 }), Number(1, 0, { 100 })), Rational(10));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"7.5");
    res = Mod(Rational(Number(-1, 0, { 12250 }), Number(1, 0, { 100 })), Rational(-10));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-2.5");
    res = Mod(Rational(Number(1, 0, { 12250 }), Number(1, 0, { 100 })), Rational(-10));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-7.5");
    res = Mod(Rational(Number(1, 0, { 1000 }), Number(1, 0, { 3 })), Rational(1));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"0.33333333");
    res = Mod(Rational(Number(1, 0, { 1000 }), Number(1, 0, { 3 })), Rational(-10));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-6.6666667");
    res = Mod(Rational(834345), Rational(Number(1, 0, { 103 }), Number(1, 0, { 100 })));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"0.71");
    res = Mod(Rational(834345), Rational(Number(-1, 0, { 103 }), Number(1, 0, { 100 })));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-0.32");
}

TEST_METHOD(TestRemainderOperandsNotModified)
{
    // Verify results but also check that operands are not modified
    Rational rat25(25);
    Rational ratminus25(-25);
    Rational rat4(4);
    Rational ratminus4(-4);
    Rational res = rat25 % rat4;
    VERIFY_ARE_EQUAL(res, 1);
    VERIFY_ARE_EQUAL(rat25, 25);
    VERIFY_ARE_EQUAL(rat4, 4);
    res = rat25 % ratminus4;
    VERIFY_ARE_EQUAL(res, 1);
    VERIFY_ARE_EQUAL(rat25, 25);
    VERIFY_ARE_EQUAL(ratminus4, -4);
    res = ratminus25 % ratminus4;
    VERIFY_ARE_EQUAL(res, -1);
    VERIFY_ARE_EQUAL(ratminus25, -25);
    VERIFY_ARE_EQUAL(ratminus4, -4);
    res = ratminus25 % rat4;
    VERIFY_ARE_EQUAL(res, -1);
    VERIFY_ARE_EQUAL(ratminus25, -25);
    VERIFY_ARE_EQUAL(rat4, 4);
}

TEST_METHOD(TestRemainderInteger)
{
    // Check with integers
    auto res = Rational(426) % Rational(56478);
    VERIFY_ARE_EQUAL(res, 426);
    res = Rational(56478) % Rational(426);
    VERIFY_ARE_EQUAL(res, 246);
    res = Rational(-643) % Rational(8756);
    VERIFY_ARE_EQUAL(res, -643);
    res = Rational(643) % Rational(-8756);
    VERIFY_ARE_EQUAL(res, 643);
    res = Rational(-643) % Rational(-8756);
    VERIFY_ARE_EQUAL(res, -643);
    res = Rational(-124) % Rational(-124);
    VERIFY_ARE_EQUAL(res, 0);
    res = Rational(24) % Rational(24);
    VERIFY_ARE_EQUAL(res, 0);
}

TEST_METHOD(TestRemainderZero)
{
    // Test with Zero
    auto res = Rational(0) % Rational(3654);
    VERIFY_ARE_EQUAL(res, 0);
    res = Rational(0) % Rational(-242);
    VERIFY_ARE_EQUAL(res, 0);
    for (auto number : { 343654332, 0, -23423 })
    {
        try
        {
            res = Rational(number) % Rational(0);
            Assert::Fail();
        }
        catch (uint32_t t)
        {
            if (t != CALC_E_INDEFINITE)
            {
                Assert::Fail();
            }
        }
        catch (...)
        {
            Assert::Fail();
        }

        try
        {
            res = Rational(Number(1, number, { 0 }), Number(1, 0, { 2 })) % Rational(Number(1, 0, { 0 }), Number(1, 0, { 23 }));
            Assert::Fail();
        }
        catch (uint32_t t)
        {
            if (t != CALC_E_INDEFINITE)
            {
                Assert::Fail();
            }
        }
        catch (...)
        {
            Assert::Fail();
        }
    }
}

TEST_METHOD(TestRemainderRational)
{
    // Test with rational numbers
    auto res = Rational(Number(1, 0, { 250 }), Number(1, 0, { 100 })) % Rational(89);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"2.5");
    res = Rational(Number(1, 0, { 3330 }), Number(1, 0, { 1332 })) % Rational(1);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"0.5");
    res = Rational(Number(1, 0, { 12250 }), Number(1, 0, { 100 })) % Rational(10);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"2.5");
    res = Rational(Number(-1, 0, { 12250 }), Number(1, 0, { 100 })) % Rational(10);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-2.5");
    res = Rational(Number(-1, 0, { 12250 }), Number(1, 0, { 100 })) % Rational(-10);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-2.5");
    res = Rational(Number(1, 0, { 12250 }), Number(1, 0, { 100 })) % Rational(-10);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"2.5");
    res = Rational(Number(1, 0, { 1000 }), Number(1, 0, { 3 })) % Rational(1);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"0.33333333");
    res = Rational(Number(1, 0, { 1000 }), Number(1, 0, { 3 })) % Rational(-10);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"3.3333333");
    res = Rational(Number(-1, 0, { 1000 }), Number(1, 0, { 3 })) % Rational(-10);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-3.3333333");
    res = Rational(834345) % Rational(Number(1, 0, { 103 }), Number(1, 0, { 100 }));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"0.71");
    res = Rational(834345) % Rational(Number(-1, 0, { 103 }), Number(1, 0, { 100 }));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"0.71");
    res = Rational(-834345) % Rational(Number(1, 0, { 103 }), Number(1, 0, { 100 }));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-0.71");
}
}
```

#### 3.4.Z Remaining non-blank source not inside a `TEST_CLASS` or `TEST_METHOD` (fixture glue)

<!-- RationalTest.cpp:220-221 -->
```cpp
;
}
```

### 3.5 `src/CalculatorUnitTests/UnitConverterTest.cpp` — `TEST_CLASS(UnitConverterTest)`

16 test methods.

| # | `TEST_METHOD` | Kind | Source lines |
|---|---|---|---|
| 1 | `UnitConverterTestInit` | out-of-line definition | `UnitConverterTest.cpp:269-274` |
| 2 | `UnitConverterTestBasic` | out-of-line definition | `UnitConverterTest.cpp:277-294` |
| 3 | `UnitConverterTestBackspaceBasic` | out-of-line definition | `UnitConverterTest.cpp:297-315` |
| 4 | `UnitConverterTestClear` | out-of-line definition | `UnitConverterTest.cpp:318-328` |
| 5 | `UnitConverterTestGetters` | out-of-line definition | `UnitConverterTest.cpp:331-338` |
| 6 | `UnitConverterTestGetCategory` | out-of-line definition | `UnitConverterTest.cpp:341-345` |
| 7 | `UnitConverterTestUnitTypeSwitching` | out-of-line definition | `UnitConverterTest.cpp:348-360` |
| 8 | `UnitConverterTestQuote` | out-of-line definition | `UnitConverterTest.cpp:363-374` |
| 9 | `UnitConverterTestUnquote` | out-of-line definition | `UnitConverterTest.cpp:377-386` |
| 10 | `UnitConverterTestBackspace` | out-of-line definition | `UnitConverterTest.cpp:389-420` |
| 11 | `UnitConverterTestScientificInputs` | out-of-line definition | `UnitConverterTest.cpp:423-480` |
| 12 | `UnitConverterTestSupplementaryResultRounding` | out-of-line definition | `UnitConverterTest.cpp:483-496` |
| 13 | `UnitConverterTestMaxDigitsReached` | out-of-line definition | `UnitConverterTest.cpp:498-521` |
| 14 | `UnitConverterTestMaxDigitsReached_LeadingDecimal` | out-of-line definition | `UnitConverterTest.cpp:523-547` |
| 15 | `UnitConverterTestMaxDigitsReached_TrailingDecimal` | out-of-line definition | `UnitConverterTest.cpp:549-573` |
| 16 | `UnitConverterTestMaxDigitsReached_MultipleTimes` | out-of-line definition | `UnitConverterTest.cpp:575-601` |

#### 3.5.0 File preamble, `using` directives and fixture helpers (`UnitConverterTest.cpp:1-188`)

```cpp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <CppUnitTest.h>

using namespace UnitConversionManager;
using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitConverterUnitTests
{
    void SetUnitParams(Unit* type, int id, wstring name, wstring abbreviation, bool conversionSource, bool conversionTarget, bool isWhimsical)
    {
        type->id = id;
        type->name = name;
        type->abbreviation = abbreviation;
        type->isConversionSource = conversionSource;
        type->isConversionTarget = conversionTarget;
        type->isWhimsical = isWhimsical;
    }

    void SetCategoryParams(Category* type, int id, wstring name, bool supportsNegative)
    {
        type->id = id;
        type->name = name;
        type->supportsNegative = supportsNegative;
    }

    void SetConversionDataParams(ConversionData* type, double ratio, double offset, bool offsetFirst)
    {
        type->ratio = ratio;
        type->offset = offset;
        type->offsetFirst = offsetFirst;
    }

    class TestUnitConverterConfigLoader : public IConverterDataLoader
    {
    public:
        TestUnitConverterConfigLoader()
            : m_loadDataCallCount(0)
        {
            Category c1, c2;
            SetCategoryParams(&c1, 1, L"Length", true);
            SetCategoryParams(&c2, 2, L"Weight", false);
            m_categories.push_back(c1);
            m_categories.push_back(c2);

            Unit u1, u2, u3, u4;
            SetUnitParams(&u1, 1, L"Inches", L"In", true, true, false);
            SetUnitParams(&u2, 2, L"Feet", L"Ft", false, false, false);
            SetUnitParams(&u3, 3, L"Pounds", L"Lb", true, true, false);
            SetUnitParams(&u4, 4, L"Kilograms", L"Kg", false, false, false);

            vector<Unit> c1units = vector<Unit>();
            vector<Unit> c2units = vector<Unit>();
            c1units.push_back(u1);
            c1units.push_back(u2);
            c2units.push_back(u3);
            c2units.push_back(u4);

            m_units[c1.id] = c1units;
            m_units[c2.id] = c2units;

            unordered_map<Unit, ConversionData, UnitHash> unit1Map = unordered_map<Unit, ConversionData, UnitHash>();
            unordered_map<Unit, ConversionData, UnitHash> unit2Map = unordered_map<Unit, ConversionData, UnitHash>();
            unordered_map<Unit, ConversionData, UnitHash> unit3Map = unordered_map<Unit, ConversionData, UnitHash>();
            unordered_map<Unit, ConversionData, UnitHash> unit4Map = unordered_map<Unit, ConversionData, UnitHash>();

            ConversionData conversion1, conversion2, conversion3, conversion4, conversion5;
            SetConversionDataParams(&conversion1, 1.0, 0, false);
            SetConversionDataParams(&conversion2, 0.08333333333333333333333333333333, 0, false);
            SetConversionDataParams(&conversion3, 12.0, 0, false);
            SetConversionDataParams(&conversion4, 0.453592, 0, false);
            SetConversionDataParams(&conversion5, 2.20462, 0, false);

            // Setting the conversion ratios for testing
            unit1Map[u1] = conversion1;
            unit1Map[u2] = conversion2;

            unit2Map[u1] = conversion3;
            unit2Map[u2] = conversion1;

            unit3Map[u3] = conversion1;
            unit3Map[u4] = conversion4;

            unit4Map[u3] = conversion5;
            unit4Map[u4] = conversion1;

            m_ratioMaps[u1] = unit1Map;
            m_ratioMaps[u2] = unit2Map;
            m_ratioMaps[u3] = unit3Map;
            m_ratioMaps[u4] = unit4Map;
        }

        void LoadData()
        {
            m_loadDataCallCount++;
        }

        vector<Category> GetOrderedCategories()
        {
            return m_categories;
        }

        vector<Unit> GetOrderedUnits(const Category& category)
        {
            return m_units[category.id];
        }

        unordered_map<Unit, ConversionData, UnitHash> LoadOrderedRatios(const Unit& u)
        {
            return m_ratioMaps[u];
        }

        bool SupportsCategory(const Category& /*target*/)
        {
            return true;
        }

        UINT m_loadDataCallCount;

    private:
        vector<Category> m_categories;
        CategoryToUnitVectorMap m_units;
        UnitToUnitToConversionDataMap m_ratioMaps;
    };

    class TestUnitConverterVMCallback : public IUnitConverterVMCallback
    {
    public:
        void Reset()
        {
            m_maxDigitsReachedCallCount = 0;
        }

        void DisplayCallback(const wstring& from, const wstring& to) override
        {
            m_lastFrom = from;
            m_lastTo = to;
        }

        void SuggestedValueCallback(const vector<tuple<wstring, Unit>>& suggestedValues) override
        {
            m_lastSuggested = suggestedValues;
        }

        void MaxDigitsReached() override
        {
            m_maxDigitsReachedCallCount++;
        }

        int GetMaxDigitsReachedCallCount()
        {
            return m_maxDigitsReachedCallCount;
        }

        bool CheckDisplayValues(wstring from, wstring to)
        {
            return (from == m_lastFrom && to == m_lastTo);
        }

        bool CheckSuggestedValues(vector<tuple<wstring, Unit>> suggested)
        {
            if (suggested.size() != m_lastSuggested.size())
            {
                return false;
            }
            bool returnValue = true;
            for (unsigned int i = 0; i < suggested.size(); i++)
            {
                if (suggested[i] != m_lastSuggested[i])
                {
                    returnValue = false;
                    break;
                }
            }
            return returnValue;
        }

    private:
        wstring m_lastFrom;
        wstring m_lastTo;
        vector<tuple<wstring, Unit>> m_lastSuggested;
        int m_maxDigitsReachedCallCount = 0;
    };
```

#### 3.5.1 `TEST_CLASS` block verbatim (`UnitConverterTest.cpp:189-226`)

```cpp
    TEST_CLASS(UnitConverterTest)
    {
    public:
        // Declare this class as a TestClass, and supply metadata if necessary.
        TEST_CLASS_INITIALIZE(CommonSetup);

        TEST_METHOD_CLEANUP(Cleanup);

        TEST_METHOD(UnitConverterTestInit);
        TEST_METHOD(UnitConverterTestBasic);
        TEST_METHOD(UnitConverterTestGetters);
        TEST_METHOD(UnitConverterTestGetCategory);
        TEST_METHOD(UnitConverterTestUnitTypeSwitching);
        TEST_METHOD(UnitConverterTestQuote);
        TEST_METHOD(UnitConverterTestUnquote);
        TEST_METHOD(UnitConverterTestBackspace);
        TEST_METHOD(UnitConverterTestBackspaceBasic);
        TEST_METHOD(UnitConverterTestClear);
        TEST_METHOD(UnitConverterTestScientificInputs);
        TEST_METHOD(UnitConverterTestSupplementaryResultRounding);
        TEST_METHOD(UnitConverterTestMaxDigitsReached);
        TEST_METHOD(UnitConverterTestMaxDigitsReached_LeadingDecimal);
        TEST_METHOD(UnitConverterTestMaxDigitsReached_TrailingDecimal);
        TEST_METHOD(UnitConverterTestMaxDigitsReached_MultipleTimes);

    private:
        static void ExecuteCommands(vector<Command> commands);

        static shared_ptr<UnitConverter> s_unitConverter;
        static shared_ptr<TestUnitConverterConfigLoader> s_xmlLoader;
        static shared_ptr<TestUnitConverterVMCallback> s_testVMCallback;
        static Category s_testLength;
        static Category s_testWeight;
        static Unit s_testInches;
        static Unit s_testFeet;
        static Unit s_testPounds;
        static Unit s_testKilograms;
    };
```

#### 3.5.1.1 Out-of-line `TEST_METHOD` definitions, verbatim

##### `UnitConverterTestInit` — `UnitConverterTest.cpp:269-274`

```cpp
    void UnitConverterTest::UnitConverterTestInit()
    {
        VERIFY_ARE_EQUAL((UINT)0, s_xmlLoader->m_loadDataCallCount); // shouldn't have initialized the loader yet
        s_unitConverter->Initialize();
        VERIFY_ARE_EQUAL((UINT)1, s_xmlLoader->m_loadDataCallCount); // now we should have loaded
    }
```

##### `UnitConverterTestBasic` — `UnitConverterTest.cpp:277-294`

```cpp
    void UnitConverterTest::UnitConverterTestBasic()
    {
        tuple<wstring, Unit> test1[] = { tuple<wstring, Unit>(wstring(L"0.25"), s_testFeet) };
        tuple<wstring, Unit> test2[] = { tuple<wstring, Unit>(wstring(L"2.5"), s_testFeet) };

        s_unitConverter->SendCommand(Command::Three);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"3"), wstring(L"3")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test1), end(test1))));
        s_unitConverter->SendCommand(Command::Zero);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"30"), wstring(L"30")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test2), end(test2))));
        s_unitConverter->SendCommand(Command::Decimal);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"30."), wstring(L"30")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test2), end(test2))));
        s_unitConverter->SendCommand(Command::Zero);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"30.0"), wstring(L"30")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test2), end(test2))));
    }
```

##### `UnitConverterTestBackspaceBasic` — `UnitConverterTest.cpp:297-315`

```cpp
    void UnitConverterTest::UnitConverterTestBackspaceBasic()
    {
        s_unitConverter->SendCommand(Command::Two);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Decimal);
        s_unitConverter->SendCommand(Command::Four);
        s_unitConverter->SendCommand(Command::Three);
        s_unitConverter->SendCommand(Command::Backspace);

        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"20.4"), wstring(L"20.4")));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"20."), wstring(L"20")));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"20"), wstring(L"20")));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"2"), wstring(L"2")));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"0"), wstring(L"0")));
    }
```

##### `UnitConverterTestClear` — `UnitConverterTest.cpp:318-328`

```cpp
    void UnitConverterTest::UnitConverterTestClear()
    {
        s_unitConverter->SendCommand(Command::Two);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Decimal);
        s_unitConverter->SendCommand(Command::Four);
        s_unitConverter->SendCommand(Command::Three);
        s_unitConverter->SendCommand(Command::Clear);

        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"0"), wstring(L"0")));
    }
```

##### `UnitConverterTestGetters` — `UnitConverterTest.cpp:331-338`

```cpp
    void UnitConverterTest::UnitConverterTestGetters()
    {
        Category test1[] = { s_testLength, s_testWeight };
        Unit test2[] = { s_testInches, s_testFeet };

        VERIFY_IS_TRUE(s_unitConverter->GetCategories() == vector<Category>(begin(test1), end(test1)));
        VERIFY_IS_TRUE(get<0>(s_unitConverter->SetCurrentCategory(test1[0])) == vector<Unit>(begin(test2), end(test2)));
    }
```

##### `UnitConverterTestGetCategory` — `UnitConverterTest.cpp:341-345`

```cpp
    void UnitConverterTest::UnitConverterTestGetCategory()
    {
        s_unitConverter->SetCurrentCategory(s_testWeight);
        VERIFY_IS_TRUE(s_unitConverter->GetCurrentCategory() == s_testWeight);
    }
```

##### `UnitConverterTestUnitTypeSwitching` — `UnitConverterTest.cpp:348-360`

```cpp
    void UnitConverterTest::UnitConverterTestUnitTypeSwitching()
    {
        // Enter 57 into the from field, then switch focus to the to field (making it the new from field)
        s_unitConverter->SendCommand(Command::Five);
        s_unitConverter->SendCommand(Command::Seven);
        s_unitConverter->SwitchActive(wstring(L"57"));
        // Now set unit conversion to go from kilograms to pounds
        s_unitConverter->SetCurrentCategory(s_testWeight);
        s_unitConverter->SetCurrentUnitTypes(s_testKilograms, s_testPounds);
        s_unitConverter->SendCommand(Command::Five);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"5"), wstring(L"11.0231")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>()));
    }
```

##### `UnitConverterTestQuote` — `UnitConverterTest.cpp:363-374`

```cpp
    void UnitConverterTest::UnitConverterTestQuote()
    {
        constexpr wstring_view input1 = L"Weight";
        constexpr wstring_view output1 = L"Weight";
        constexpr wstring_view input2 = L"{p}Weig;[ht|";
        constexpr wstring_view output2 = L"{lb}p{rb}Weig{sc}{lc}ht{p}";
        constexpr wstring_view input3 = L"{{{t;s}}},:]";
        constexpr wstring_view output3 = L"{lb}{lb}{lb}t{sc}s{rb}{rb}{rb}{cm}{co}{rc}";
        VERIFY_IS_TRUE(UnitConverter::Quote(input1) == output1);
        VERIFY_IS_TRUE(UnitConverter::Quote(input2) == output2);
        VERIFY_IS_TRUE(UnitConverter::Quote(input3) == output3);
    }
```

##### `UnitConverterTestUnquote` — `UnitConverterTest.cpp:377-386`

```cpp
    void UnitConverterTest::UnitConverterTestUnquote()
    {
        constexpr wstring_view input1 = L"Weight";
        constexpr wstring_view input2 = L"{p}Weig;[ht|";
        constexpr wstring_view input3 = L"{{{t;s}}},:]";
        VERIFY_IS_TRUE(UnitConverter::Unquote(input1) == input1);
        VERIFY_IS_TRUE(UnitConverter::Unquote(UnitConverter::Quote(input1)) == input1);
        VERIFY_IS_TRUE(UnitConverter::Unquote(UnitConverter::Quote(input2)) == input2);
        VERIFY_IS_TRUE(UnitConverter::Unquote(UnitConverter::Quote(input3)) == input3);
    }
```

##### `UnitConverterTestBackspace` — `UnitConverterTest.cpp:389-420`

```cpp
    void UnitConverterTest::UnitConverterTestBackspace()
    {
        tuple<wstring, Unit> test1[] = { tuple<wstring, Unit>(wstring(L"13.66"), s_testKilograms) };
        tuple<wstring, Unit> test2[] = { tuple<wstring, Unit>(wstring(L"13.65"), s_testKilograms) };
        tuple<wstring, Unit> test3[] = { tuple<wstring, Unit>(wstring(L"13.61"), s_testKilograms) };
        tuple<wstring, Unit> test4[] = { tuple<wstring, Unit>(wstring(L"1.36"), s_testKilograms) };

        s_unitConverter->SetCurrentCategory(s_testWeight);
        s_unitConverter->SetCurrentUnitTypes(s_testPounds, s_testPounds);
        s_unitConverter->SendCommand(Command::Three);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Decimal);
        s_unitConverter->SendCommand(Command::One);
        s_unitConverter->SendCommand(Command::Two);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"30.12"), wstring(L"30.12")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test1), end(test1))));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"30.1"), wstring(L"30.1")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test2), end(test2))));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"30."), wstring(L"30")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test3), end(test3))));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"30"), wstring(L"30")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test3), end(test3))));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"3"), wstring(L"3")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test4), end(test4))));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"0"), wstring(L"0")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>()));
    }
```

##### `UnitConverterTestScientificInputs` — `UnitConverterTest.cpp:423-480`

```cpp
    void UnitConverterTest::UnitConverterTestScientificInputs()
    {
        s_unitConverter->SetCurrentCategory(s_testWeight);
        s_unitConverter->SetCurrentUnitTypes(s_testPounds, s_testKilograms);
        s_unitConverter->SendCommand(Command::Decimal);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::One);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"0.00000000000001"), wstring(L"4.535920e-15")));
        s_unitConverter->SwitchActive(wstring(L"4.535920e-15"));
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"999999999999999"), wstring(L"2.204620e+15")));
        s_unitConverter->SwitchActive(wstring(L"2.20463e+15"));
        s_unitConverter->SendCommand(Command::One);
        s_unitConverter->SendCommand(Command::Two);
        s_unitConverter->SendCommand(Command::Three);
        s_unitConverter->SendCommand(Command::Four);
        s_unitConverter->SendCommand(Command::Five);
        s_unitConverter->SendCommand(Command::Six);
        s_unitConverter->SendCommand(Command::Seven);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"1234567"), wstring(L"559989.7")));
        s_unitConverter->SwitchActive(wstring(L"559989.7"));
        s_unitConverter->SendCommand(Command::One);
        s_unitConverter->SendCommand(Command::Two);
        s_unitConverter->SendCommand(Command::Three);
        s_unitConverter->SendCommand(Command::Four);
        s_unitConverter->SendCommand(Command::Five);
        s_unitConverter->SendCommand(Command::Six);
        s_unitConverter->SendCommand(Command::Seven);
        s_unitConverter->SendCommand(Command::Eight);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"12345678"), wstring(L"27217529")));
    }
```

##### `UnitConverterTestSupplementaryResultRounding` — `UnitConverterTest.cpp:483-496`

```cpp
    void UnitConverterTest::UnitConverterTestSupplementaryResultRounding()
    {
        tuple<wstring, Unit> test1[] = { tuple<wstring, Unit>(wstring(L"27.75"), s_testFeet) };
        tuple<wstring, Unit> test2[] = { tuple<wstring, Unit>(wstring(L"277.8"), s_testFeet) };
        tuple<wstring, Unit> test3[] = { tuple<wstring, Unit>(wstring(L"2778"), s_testFeet) };
        s_unitConverter->SendCommand(Command::Three);
        s_unitConverter->SendCommand(Command::Three);
        s_unitConverter->SendCommand(Command::Three);
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test1), end(test1))));
        s_unitConverter->SendCommand(Command::Three);
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test2), end(test2))));
        s_unitConverter->SendCommand(Command::Three);
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test3), end(test3))));
    }
```

##### `UnitConverterTestMaxDigitsReached` — `UnitConverterTest.cpp:498-521`

```cpp
    void UnitConverterTest::UnitConverterTestMaxDigitsReached()
    {
        ExecuteCommands({ Command::One,
                          Command::Two,
                          Command::Three,
                          Command::Four,
                          Command::Five,
                          Command::Six,
                          Command::Seven,
                          Command::Eight,
                          Command::Nine,
                          Command::One,
                          Command::Zero,
                          Command::One,
                          Command::One,
                          Command::One,
                          Command::Two });

        VERIFY_ARE_EQUAL(0, s_testVMCallback->GetMaxDigitsReachedCallCount());

        ExecuteCommands({ Command::One });

        VERIFY_ARE_EQUAL(1, s_testVMCallback->GetMaxDigitsReachedCallCount());
    }
```

##### `UnitConverterTestMaxDigitsReached_LeadingDecimal` — `UnitConverterTest.cpp:523-547`

```cpp
    void UnitConverterTest::UnitConverterTestMaxDigitsReached_LeadingDecimal()
    {
        ExecuteCommands({ Command::Zero,
                          Command::Decimal,
                          Command::One,
                          Command::Two,
                          Command::Three,
                          Command::Four,
                          Command::Five,
                          Command::Six,
                          Command::Seven,
                          Command::Eight,
                          Command::Nine,
                          Command::One,
                          Command::Zero,
                          Command::One,
                          Command::One,
                          Command::One });

        VERIFY_ARE_EQUAL(0, s_testVMCallback->GetMaxDigitsReachedCallCount());

        ExecuteCommands({ Command::Two });

        VERIFY_ARE_EQUAL(1, s_testVMCallback->GetMaxDigitsReachedCallCount());
    }
```

##### `UnitConverterTestMaxDigitsReached_TrailingDecimal` — `UnitConverterTest.cpp:549-573`

```cpp
    void UnitConverterTest::UnitConverterTestMaxDigitsReached_TrailingDecimal()
    {
        ExecuteCommands({ Command::One,
                          Command::Two,
                          Command::Three,
                          Command::Four,
                          Command::Five,
                          Command::Six,
                          Command::Seven,
                          Command::Eight,
                          Command::Nine,
                          Command::One,
                          Command::Zero,
                          Command::One,
                          Command::One,
                          Command::One,
                          Command::Two,
                          Command::Decimal });

        VERIFY_ARE_EQUAL(0, s_testVMCallback->GetMaxDigitsReachedCallCount());

        ExecuteCommands({ Command::One });

        VERIFY_ARE_EQUAL(1, s_testVMCallback->GetMaxDigitsReachedCallCount());
    }
```

##### `UnitConverterTestMaxDigitsReached_MultipleTimes` — `UnitConverterTest.cpp:575-601`

```cpp
    void UnitConverterTest::UnitConverterTestMaxDigitsReached_MultipleTimes()
    {
        ExecuteCommands({ Command::One,
                          Command::Two,
                          Command::Three,
                          Command::Four,
                          Command::Five,
                          Command::Six,
                          Command::Seven,
                          Command::Eight,
                          Command::Nine,
                          Command::One,
                          Command::Zero,
                          Command::One,
                          Command::One,
                          Command::One,
                          Command::Two });

        VERIFY_ARE_EQUAL(0, s_testVMCallback->GetMaxDigitsReachedCallCount());

        for (auto count = 1; count <= 10; count++)
        {
            ExecuteCommands({ Command::Three });

            VERIFY_ARE_EQUAL(count, s_testVMCallback->GetMaxDigitsReachedCallCount(), to_wstring(count).c_str());
        }
    }
```

#### 3.5.Z Remaining non-blank source not inside a `TEST_CLASS` or `TEST_METHOD` (fixture glue)

<!-- UnitConverterTest.cpp:228-236 -->
```cpp
    shared_ptr<UnitConverter> UnitConverterTest::s_unitConverter;
    shared_ptr<TestUnitConverterConfigLoader> UnitConverterTest::s_xmlLoader;
    shared_ptr<TestUnitConverterVMCallback> UnitConverterTest::s_testVMCallback;
    Category UnitConverterTest::s_testLength;
    Category UnitConverterTest::s_testWeight;
    Unit UnitConverterTest::s_testInches;
    Unit UnitConverterTest::s_testFeet;
    Unit UnitConverterTest::s_testPounds;
    Unit UnitConverterTest::s_testKilograms;
```

<!-- UnitConverterTest.cpp:238-251 -->
```cpp
    // Creates instance of UnitConverter before running tests
    void UnitConverterTest::CommonSetup()
    {
        s_testVMCallback = make_shared<TestUnitConverterVMCallback>();
        s_xmlLoader = make_shared<TestUnitConverterConfigLoader>();
        s_unitConverter = make_shared<UnitConverter>(s_xmlLoader);
        s_unitConverter->SetViewModelCallback(s_testVMCallback);
        SetCategoryParams(&s_testLength, 1, L"Length", true);
        SetCategoryParams(&s_testWeight, 2, L"Weight", false);
        SetUnitParams(&s_testInches, 1, L"Inches", L"In", true, true, false);
        SetUnitParams(&s_testFeet, 2, L"Feet", L"Ft", false, false, false);
        SetUnitParams(&s_testPounds, 3, L"Pounds", L"Lb", true, true, false);
        SetUnitParams(&s_testKilograms, 4, L"Kilograms", L"Kg", false, false, false);
    }
```

<!-- UnitConverterTest.cpp:253-258 -->
```cpp
    // Resets calculator state to start state after each test
    void UnitConverterTest::Cleanup()
    {
        s_unitConverter->SendCommand(Command::Reset);
        s_testVMCallback->Reset();
    }
```

<!-- UnitConverterTest.cpp:260-266 -->
```cpp
    void UnitConverterTest::ExecuteCommands(vector<Command> commands)
    {
        for (size_t i = 0; i < commands.size() && commands[i] != Command::None; i++)
        {
            s_unitConverter->SendCommand(commands[i]);
        }
    }
```

<!-- UnitConverterTest.cpp:268-268 -->
```cpp
    // Test ctor/initialization states
```

<!-- UnitConverterTest.cpp:276-276 -->
```cpp
    // Verify a basic input command stream.'3', '2', '.', '0'
```

<!-- UnitConverterTest.cpp:296-296 -->
```cpp
    // Verify a basic copy paste steam. '20.43' with backspace button pressed
```

<!-- UnitConverterTest.cpp:317-317 -->
```cpp
    // Verify a basic copy paste steam. '20.43' with backspace button pressed
```

<!-- UnitConverterTest.cpp:330-330 -->
```cpp
    // Check the getter functions
```

<!-- UnitConverterTest.cpp:340-340 -->
```cpp
    // Test getting category after it has been set.
```

<!-- UnitConverterTest.cpp:347-347 -->
```cpp
    // Test switching of unit types
```

<!-- UnitConverterTest.cpp:362-362 -->
```cpp
    // Test input escaping
```

<!-- UnitConverterTest.cpp:376-376 -->
```cpp
    // Test output unescaping
```

<!-- UnitConverterTest.cpp:388-388 -->
```cpp
    // Test backspace commands
```

<!-- UnitConverterTest.cpp:422-422 -->
```cpp
    // Test large values
```

<!-- UnitConverterTest.cpp:482-482 -->
```cpp
    // Test large values
```

<!-- UnitConverterTest.cpp:602-602 -->
```cpp
}
```

**C++ `TEST_METHOD` total: 86**

---

## 4. C# unit tests — `src/Calculator.Tests/` (verbatim, grouped by `[TestClass]`)

MSTest 2.2.10 UWP unit tests against `Calculator.ViewModels` (+ `CalcManager.Interop`, `GraphControl`).
**23 `[TestClass]`es, 288 `[TestMethod]`/`[DataTestMethod]` methods** across 16 files.
Sections are ordered by file, then by declaration order within the file.

Each subsection contains: an index table of test methods with line ranges (including `[DataRow]`
values), the file preamble (`using` directives / file-scoped helpers), and each `[TestClass]`
block verbatim — which contains the `[ClassInitialize]`, `[TestInitialize]`, helper methods and
every `[TestMethod]` body exactly as written.

### 4.1 `src/Calculator.Tests/CopyPasteManagerTests.cs`

1 `[TestClass]`(es), 23 test method(s).

| # | Test method | Attributes | Source lines |
|---|---|---|---|
| 1 | `FunctionalCopyPasteTest` | `[TestMethod]` | `CopyPasteManagerTests.cs:15-29` |
| 2 | `ValidateStandardPasteExpressionTest` | `[TestMethod]` | `CopyPasteManagerTests.cs:31-83` |
| 3 | `ValidateScientificPasteExpressionTest` | `[TestMethod]` | `CopyPasteManagerTests.cs:85-144` |
| 4 | `ScientificPasteRequiresANumericOperand` | `[TestMethod]` | `CopyPasteManagerTests.cs:146-153` |
| 5 | `ScientificPasteAllowsTerminalOperators` | `[TestMethod]` | `CopyPasteManagerTests.cs:155-162` |
| 6 | `ScientificPasteRejectsDigitsUnsupportedByCalculator` | `[TestMethod]` | `CopyPasteManagerTests.cs:164-171` |
| 7 | `ValidateProgrammerDecPasteExpressionTest` | `[TestMethod]` | `CopyPasteManagerTests.cs:173-268` |
| 8 | `ValidateProgrammerOctPasteExpressionTest` | `[TestMethod]` | `CopyPasteManagerTests.cs:270-368` |
| 9 | `ValidateProgrammerHexPasteExpressionTest` | `[TestMethod]` | `CopyPasteManagerTests.cs:370-486` |
| 10 | `ValidateProgrammerBinPasteExpressionTest` | `[TestMethod]` | `CopyPasteManagerTests.cs:488-601` |
| 11 | `ProgrammerPrefixedValuesRespectByteAndWordRanges` | `[TestMethod]` | `CopyPasteManagerTests.cs:603-637` |
| 12 | `BinaryZeroWithSuffixRemainsValid` | `[TestMethod]` | `CopyPasteManagerTests.cs:639-643` |
| 13 | `ValidateConverterPasteExpressionTest` | `[TestMethod]` | `CopyPasteManagerTests.cs:645-666` |
| 14 | `ValidatePasteExpressionErrorStates` | `[TestMethod]` | `CopyPasteManagerTests.cs:668-700` |
| 15 | `ValidateExtractOperands` | `[TestMethod]` | `CopyPasteManagerTests.cs:702-726` |
| 16 | `ValidateExtractOperandsErrors` | `[TestMethod]` | `CopyPasteManagerTests.cs:728-751` |
| 17 | `ValidateExpressionRegExMatch` | `[TestMethod]` | `CopyPasteManagerTests.cs:753-833` |
| 18 | `ValidateGetMaxOperandLengthAndValue` | `[TestMethod]` | `CopyPasteManagerTests.cs:835-918` |
| 19 | `ValidateSanitizeOperand` | `[TestMethod]` | `CopyPasteManagerTests.cs:920-936` |
| 20 | `ValidatePrefixCurrencySymbols` | `[TestMethod]` | `CopyPasteManagerTests.cs:938-952` |
| 21 | `ValidateTryOperandToULL` | `[TestMethod]` | `CopyPasteManagerTests.cs:954-1026` |
| 22 | `ValidateStandardScientificOperandLength` | `[TestMethod]` | `CopyPasteManagerTests.cs:1028-1037` |
| 23 | `ValidateProgrammerOperandLength` | `[TestMethod]` | `CopyPasteManagerTests.cs:1039-1069` |

#### 4.1.0 File preamble — `using` directives, file-scoped structs/helpers, namespace open (`CopyPasteManagerTests.cs:1-11`)

```csharp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

namespace Calculator.Tests
{
```

#### 4.1.1 `[TestClass] CopyPasteManagerTests` — verbatim (`CopyPasteManagerTests.cs:12-1178`)

```csharp
    [TestClass]
    public class CopyPasteManagerTests
    {
        [TestMethod]
        public void FunctionalCopyPasteTest()
        {
            var scvm = new StandardCalculatorViewModel();
            scvm.IsStandard = true;
            string[] inputs = { "123", "12345", "123+456", "1,234", "1 2 3", "\n\r1,234\n", "\n 1+\n2 ", "1\"2" };

            foreach (var input in inputs)
            {
                scvm.OnPaste(input);
                Assert.AreEqual(ValidateStandardPasteExpression(scvm.DisplayValue), scvm.DisplayValue);
                Assert.AreEqual(ValidateScientificPasteExpression(scvm.DisplayValue), scvm.DisplayValue);
                Assert.AreEqual(ValidateProgrammerHexQwordPasteExpression(scvm.DisplayValue), scvm.DisplayValue);
            }
        }

        [TestMethod]
        public void ValidateStandardPasteExpressionTest()
        {
            string[] positiveInput =
            {
                "123",
                "+123",
                "-133",
                "12345.",
                "+12.34",
                "12.345",
                "012.034",
                "-23.032",
                "-.123",
                ".1234",
                "012.012",
                "123+456",
                "123+-234",
                "123*-345",
                "123*4*-3",
                "123*+4*-3",
                "1,234",
                "1 2 3",
                "\n\r1,234\n",
                "\f\n1+2\t\r\v\x85",
                "\n 1+\n2 ",
                "1\"2",
                "1234567891234567",
                "2+2=",
                "2+2=   ",
                "1.2e23",
                "12345e-23",
            };
            string[] negativeInput =
            {
                "(123)+(456)",
                "abcdef",
                "xyz",
                "ABab",
                "e+234",
                "12345678912345678",
                "SIN(2)",
                "2+2==",
                "2=+2",
                "2%2",
                "10^2",
            };

            foreach (var data in positiveInput)
                Assert.AreEqual(ValidateStandardPasteExpression(data), data);
            foreach (var data in negativeInput)
                Assert.AreEqual(ValidateStandardPasteExpression(data), "NoOp");
        }

        [TestMethod]
        public void ValidateScientificPasteExpressionTest()
        {
            string[] positiveInput =
            {
                "123",
                "+123",
                "-133",
                "123+456",
                "12345e+023",
                "1,234",
                "1.23",
                "-.123",
                ".1234",
                "012.012",
                "123+-234",
                "123*-345",
                "123*4*-3",
                "123*+4*-3",
                "1 2 3",
                "\n\r1,234\n",
                "\f\n1+2\t\r\v\x85",
                "\n 1+\n2 ",
                "1\"2",
                "1.2e+023",
                "12345e-23",
                "(123)+(456)",
                "12345678912345678123456789012345",
                "(123)+(456)=",
                "2+2=   ",
                "-(43)",
                "+(41213)",
                "-(432+3232)",
                "-(+(-3213)+(-2312))",
                "-(-(432+3232))",
                "1.2e23",
                "12^2",
                "-12.12^-2",
                "61%99" + "-6.1%99",
                "1.1111111111111111111111111111111e+1142",
            };
            string[] negativeInput =
            {
                "abcdef",
                "xyz",
                "ABab",
                "e+234",
                "123456789123456781234567890123456",
                "11.1111111111111111111111111111111e+1142",
                "1.1e+10001",
                "0.11111111111111111111111111111111111e+111111" + "SIN(2)",
                "2+2==",
                "2=+2",
            };

            foreach (var data in positiveInput)
                Assert.AreEqual(ValidateScientificPasteExpression(data), data);
            foreach (var data in negativeInput)
                Assert.AreEqual(ValidateScientificPasteExpression(data), "NoOp");
        }

        [TestMethod]
        public void ScientificPasteRequiresANumericOperand()
        {
            foreach (string candidate in new[] { "", "   ", "+", "-" })
            {
                Assert.AreEqual("NoOp", ValidateScientificPasteExpression(candidate), candidate);
            }
        }

        [TestMethod]
        public void ScientificPasteAllowsTerminalOperators()
        {
            foreach (string candidate in new[] { "50%", "1+" })
            {
                Assert.AreEqual(candidate, ValidateScientificPasteExpression(candidate), candidate);
            }
        }

        [TestMethod]
        public void ScientificPasteRejectsDigitsUnsupportedByCalculator()
        {
            foreach (string candidate in new[] { "１", "1+１", "1e+１" })
            {
                Assert.AreEqual("NoOp", ValidateScientificPasteExpression(candidate), candidate);
            }
        }

        [TestMethod]
        public void ValidateProgrammerDecPasteExpressionTest()
        {
            // QWord
            string[] qwordPositiveInput =
            {
                "123", "+123", "-133", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "\n\r1,234\n", "\f\n1+2\t\r\v\x85", "\n 1+\n2 ", "1\"2",
                "(123)+(456)", "123+-234", "123*-345", "123*4*-3", "123*+4*-3",
                "9223372036854775807", "-9223372036854775808",
                "0n1234", "0N1234", "1234u", "1234ul", "1234ULL",
                "2+2=", "2+2=   ", "823%21",
            };
            string[] qwordNegativeInput =
            {
                "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "1.2e23", "1.2e+023", "12345e-23",
                "abcdef", "xyz", "ABab", "e+234",
                "9223372036854775808", "9223372036854775809",
                "SIN(2)", "-0n123", "0nn1234", "1234uu", "1234ulll",
                "2+2==", "2=+2",
            };

            foreach (var data in qwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerDecQwordPasteExpression(data), data);
            foreach (var data in qwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerDecQwordPasteExpression(data), "NoOp");

            // DWord
            string[] dwordPositiveInput =
            {
                "123", "+123", "-133", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "\n\r1,234\n", "\f\n1+2\t\r\v\x85", "\n 1+\n2 ", "1\"2",
                "(123)+(456)", "123+-234", "123*-345", "123*4*-3", "123*+4*-3",
                "2147483647", "-2147483647",
                "0n1234", "0N1234", "1234u", "1234ul", "1234ULL",
            };
            string[] dwordNegativeInput =
            {
                "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "1.2e23", "1.2e+023", "12345e-23",
                "abcdef", "xyz", "ABab", "e+234",
                "2147483649",
                "SIN(2)", "-0n123", "0nn1234", "1234uu", "1234ulll",
            };

            foreach (var data in dwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerDecDwordPasteExpression(data), data);
            foreach (var data in dwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerDecDwordPasteExpression(data), "NoOp");

            // Word
            string[] wordPositiveInput =
            {
                "123", "+123", "-133", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "\f\n1+2\t\r\v\x85", "1\"2",
                "(123)+(456)", "123+-234", "123*-345", "123*4*-3", "123*+4*-3",
                "32767", "-32767", "-32768",
                "0n1234", "0N1234", "1234u", "1234ul", "1234ULL",
            };
            string[] wordNegativeInput =
            {
                "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "1.2e23", "1.2e+023", "12345e-23",
                "abcdef", "xyz", "ABab", "e+234",
                "32769",
                "SIN(2)", "-0n123", "0nn1234", "1234uu", "1234ulll",
            };

            foreach (var data in wordPositiveInput)
                Assert.AreEqual(ValidateProgrammerDecWordPasteExpression(data), data);
            foreach (var data in wordNegativeInput)
                Assert.AreEqual(ValidateProgrammerDecWordPasteExpression(data), "NoOp");

            // Byte
            string[] bytePositiveInput =
            {
                "13", "+13", "-13", "13+46", "13+-34",
                "13*-3", "3*4*-3", "3*+4*-3", "1,3", "1 3",
                "1'2'3", "1_2_3", "1\"2", "127", "-127",
                "0n123", "0N123", "123u", "123ul", "123ULL",
            };
            string[] byteNegativeInput =
            {
                "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "1.2e23", "1.2e+023", "15e-23",
                "abcdef", "xyz", "ABab", "e+24",
                "129",
                "SIN(2)", "-0n123", "0nn1234", "123uu", "123ulll",
            };

            foreach (var data in bytePositiveInput)
                Assert.AreEqual(ValidateProgrammerDecBytePasteExpression(data), data);
            foreach (var data in byteNegativeInput)
                Assert.AreEqual(ValidateProgrammerDecBytePasteExpression(data), "NoOp");
        }

        [TestMethod]
        public void ValidateProgrammerOctPasteExpressionTest()
        {
            // QWord
            string[] qwordPositiveInput =
            {
                "123", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "\n\r1,234\n", "\f\n1+2\t\r\v\x85", "\n 1+\n2 ", "1\"2",
                "(123)+(456)", "0t1234", "0T1234", "0o1234", "0O1234",
                "1234u", "1234ul", "1234ULL",
                "2+2=", "2+2=   ", "127%71",
                "1777777777777777777777",
            };
            string[] qwordNegativeInput =
            {
                "+123", "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "-133", "1.2e23", "1.2e+023", "12345e-23",
                "abcdef", "xyz", "ABab", "e+234",
                "12345678901234567890123",
                "2000000000000000000000",
                "SIN(2)", "123+-234", "0ot1234", "1234uu", "1234ulll",
                "2+2==", "2=+2", "89%12",
            };

            foreach (var data in qwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerOctQwordPasteExpression(data), data);
            foreach (var data in qwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerOctQwordPasteExpression(data), "NoOp");

            // DWord
            string[] dwordPositiveInput =
            {
                "123", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "\n\r1,234\n", "\f\n1+2\t\r\v\x85", "\n 1+\n2 ", "1\"2",
                "(123)+(456)", "37777777777",
                "0t1234", "0T1234", "0o1234", "0O1234",
                "1234u", "1234ul", "1234ULL",
            };
            string[] dwordNegativeInput =
            {
                "+123", "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "-133", "1.2e23", "1.2e+023", "12345e-23",
                "abcdef", "xyz", "ABab", "e+234",
                "377777777771",
                "40000000000",
                "SIN(2)", "123+-234", "0ot1234", "1234uu", "1234ulll",
            };

            foreach (var data in dwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerOctDwordPasteExpression(data), data);
            foreach (var data in dwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerOctDwordPasteExpression(data), "NoOp");

            // Word
            string[] wordPositiveInput =
            {
                "123", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "\f\n1+2\t\r\v\x85", "1\"2",
                "(123)+(456)", "177777",
                "0t1234", "0T1234", "0o1234", "0O1234",
                "1234u", "1234ul", "1234ULL",
            };
            string[] wordNegativeInput =
            {
                "+123", "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "-133", "1.2e23", "1.2e+023", "12345e-23",
                "abcdef", "xyz", "ABab", "e+234",
                "1777771",
                "200000",
                "SIN(2)", "123+-234", "0ot1234", "1234uu", "1234ulll",
            };

            foreach (var data in wordPositiveInput)
                Assert.AreEqual(ValidateProgrammerOctWordPasteExpression(data), data);
            foreach (var data in wordNegativeInput)
                Assert.AreEqual(ValidateProgrammerOctWordPasteExpression(data), "NoOp");

            // Byte
            string[] bytePositiveInput =
            {
                "13", "13+46", "1,3", "1 3", "1'2'3", "1_2_3", "1\"2",
                "377",
                "0t123", "0T123", "0o123", "0O123", "123u", "123ul", "123ULL",
            };
            string[] byteNegativeInput =
            {
                "+123", "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "-13", "1.2e23", "1.2e+023", "15e-23",
                "abcdef", "xyz", "ABab", "e+24",
                "477",
                "400",
                "SIN(2)", "123+-34", "0ot123", "123uu", "123ulll",
            };

            foreach (var data in bytePositiveInput)
                Assert.AreEqual(ValidateProgrammerOctBytePasteExpression(data), data);
            foreach (var data in byteNegativeInput)
                Assert.AreEqual(ValidateProgrammerOctBytePasteExpression(data), "NoOp");
        }

        [TestMethod]
        public void ValidateProgrammerHexPasteExpressionTest()
        {
            // QWord
            string[] qwordPositiveInput =
            {
                "123", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "12345e-23",
                "\n\r1,234\n", "\f\n1+2\t\r\v\x85", "\f\n1+2\t\r\v\x85",
                "\n 1+\n2 ", "e+234", "1\"2",
                "(123)+(456)", "abcdef", "ABab",
                "ABCDF21abc41a",
                "0x1234", "0xab12", "0X1234",
                "AB12h", "BC34H",
                "1234u", "1234ul", "1234ULL",
                "2+2=", "2+2=   ",
                "A4C3%12", "1233%AB", "FFC1%F2",
            };
            string[] qwordNegativeInput =
            {
                "+123", "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "-133", "1.2e+023", "1.2e23",
                "xyz",
                "ABCDEF21abc41abc7",
                "SIN(2)", "123+-234", "1234x", "A0x1234", "0xx1234",
                "1234uu", "1234ulll",
                "2+2==", "2=+2",
            };

            foreach (var data in qwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerHexQwordPasteExpression(data), data);
            foreach (var data in qwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerHexQwordPasteExpression(data), "NoOp");

            // DWord
            string[] dwordPositiveInput =
            {
                "123", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "12345e-23",
                "\n\r1,234\n", "\f\n1+2\t\r\v\x85", "\n 1+\n2 ",
                "e+234", "1\"2",
                "(123)+(456)", "abcdef", "ABab",
                "ABCD123a",
                "0x1234", "0xab12", "0X1234",
                "AB12h", "BC34H",
                "1234u", "1234ul", "1234ULL",
            };
            string[] dwordNegativeInput =
            {
                "+123", "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "-133", "1.2e+023", "1.2e23",
                "xyz",
                "ABCD123ab",
                "SIN(2)", "123+-234", "1234x", "A0x1234", "0xx1234",
                "1234uu", "1234ulll",
            };

            foreach (var data in dwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerHexDwordPasteExpression(data), data);
            foreach (var data in dwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerHexDwordPasteExpression(data), "NoOp");

            // Word
            string[] wordPositiveInput =
            {
                "123", "13+456", "1,34", "12 3", "1'2'3'4", "1_2_3_4",
                "15e-23",
                "\r1", "\n\r1,4", "\n1,4\n",
                "\f\n1+2\t\r\v", "\n 1+\n2 ",
                "e+24", "1\"2",
                "(23)+(4)", "aef", "ABab",
                "A1a3",
                "FFFF",
                "0x1234", "0xab12", "0X1234",
                "AB12h", "BC34H",
                "1234u", "1234ul", "1234ULL",
            };
            string[] wordNegativeInput =
            {
                "+123", "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "-133", "1.2e+023", "1.2e23",
                "xyz",
                "A1a3b",
                "SIN(2)", "123+-234", "1234x", "A0x1234", "0xx1234",
                "1234uu", "1234ulll",
            };

            foreach (var data in wordPositiveInput)
                Assert.AreEqual(ValidateProgrammerHexWordPasteExpression(data), data);
            foreach (var data in wordNegativeInput)
                Assert.AreEqual(ValidateProgrammerHexWordPasteExpression(data), "NoOp");

            // Byte
            string[] bytePositiveInput =
            {
                "13", "13+6", "1,4", "2 3", "1'2", "1_2",
                "5e-3",
                "\r1", "a", "ab", "A1",
                "0x12", "0xab", "0X12",
                "A9h", "B8H",
                "12u", "12ul", "12ULL",
            };
            string[] byteNegativeInput =
            {
                "+3", "1.2", "1''2", "'12", "12'", "1__2", "_12", "12_",
                "-3", "1.1e+02", "1.2e3",
                "xz",
                "A3a",
                "SIN(2)", "13+-23", "12x", "A0x1", "0xx12",
                "12uu", "12ulll",
            };

            foreach (var data in bytePositiveInput)
                Assert.AreEqual(ValidateProgrammerHexBytePasteExpression(data), data);
            foreach (var data in byteNegativeInput)
                Assert.AreEqual(ValidateProgrammerHexBytePasteExpression(data), "NoOp");
        }

        [TestMethod]
        public void ValidateProgrammerBinPasteExpressionTest()
        {
            // QWord
            string[] qwordPositiveInput =
            {
                "100", "100+101", "1,001", "1 0 1", "1'0'0'1", "1_0_0_1",
                "\n\r1,010\n", "\f\n1+11\t\r\v\x85", "\n 1+\n1 ", "1\"1",
                "(101)+(10)",
                "0b1001", "0B1111", "0y1001", "0Y1001",
                "1100b", "1101B",
                "1111u", "1111ul", "1111ULL",
                "1010101010101010101010101011110110100100101010101001010101001010",
                "1+10=", "1+10=   ", "1001%10",
            };
            string[] qwordNegativeInput =
            {
                "+10101", "1.01", "1''0", "'101", "101'", "1__0", "_101", "101_",
                "-10101001", "123", "1.2e23", "1.2e+023", "101010e-1010",
                "abcdef", "xyz", "ABab", "e+10101",
                "b1001", "10b01", "0x10", "1001x", "1001h",
                "0bb1111", "1111uu", "1111ulll",
                "10101010101010101010101010111101101001001010101010010101010010100",
                "SIN(01010)", "10+-10101010101",
                "1+10==", "1=+10",
            };

            foreach (var data in qwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerBinQwordPasteExpression(data), data);
            foreach (var data in qwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerBinQwordPasteExpression(data), "NoOp");

            // DWord
            string[] dwordPositiveInput =
            {
                "100", "100+101", "1,001", "1 0 1", "1'0'0'1", "1_0_0_1",
                "\n\r1,010\n", "\f\n1+11\t\r\v\x85", "\n 1+\n1 ", "1\"1",
                "(101)+(10)",
                "0b1001", "0B1111", "0y1001", "0Y1001",
                "1100b", "1101B",
                "1111u", "1111ul", "1111ULL",
                "10101001001010101101010111111100",
            };
            string[] dwordNegativeInput =
            {
                "+10101", "1.01", "1''0", "'101", "101'", "1__0", "_101", "101_",
                "-10101001", "123", "1.2e23", "1.2e+023", "101010e-1010",
                "abcdef", "xyz", "ABab", "e+10101",
                "b1001", "10b01", "0x10", "1001x", "1001h",
                "0bb1111", "1111uu", "1111ulll",
                "101010010010101011010101111111001",
                "SIN(01010)", "10+-10101010101",
            };

            foreach (var data in dwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerBinDwordPasteExpression(data), data);
            foreach (var data in dwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerBinDwordPasteExpression(data), "NoOp");

            // Word
            string[] wordPositiveInput =
            {
                "100", "100+101", "1,001", "1 0 1", "1'0'0'1", "1_0_0_1",
                "\n\r1,010\n", "\f\n1+11\t\r\v\x85", "\n 1+\n1 ", "1\"1",
                "(101)+(10)",
                "0b1001", "0B1111", "0y1001", "0Y1001",
                "1100b", "1101B",
                "1111u", "1111ul", "1111ULL",
                "1010101010010010",
            };
            string[] wordNegativeInput =
            {
                "+10101", "1.01", "1''0", "'101", "101'", "1__0", "_101", "101_",
                "-10101001", "123", "1.2e23", "1.2e+023", "101010e-1010",
                "abcdef", "xyz", "ABab", "e+10101",
                "b1001", "10b01", "0x10", "1001x", "1001h",
                "0bb1111", "1111uu", "1111ulll",
                "10101010100100101",
                "SIN(01010)", "10+-10101010101",
            };

            foreach (var data in wordPositiveInput)
                Assert.AreEqual(ValidateProgrammerBinWordPasteExpression(data), data);
            foreach (var data in wordNegativeInput)
                Assert.AreEqual(ValidateProgrammerBinWordPasteExpression(data), "NoOp");

            // Byte
            string[] bytePositiveInput =
            {
                "100", "100+101", "1,001", "1 0 1", "1'0'0'1", "1_0_0_1",
                "\n\r1,010\n", "\n 1+\n1 ", "1\"1",
                "(101)+(10)",
                "0b1001", "0B1111", "0y1001", "0Y1001",
                "1100b", "1101B",
                "1111u", "1111ul", "1111ULL",
                "10100010", "11111111",
            };
            string[] byteNegativeInput =
            {
                "+10101", "1.01", "1''0", "'101", "101'", "1__0", "_101", "101_",
                "-10101001", "123", "1.2e23", "1.2e+023", "101010e-1010",
                "abcdef", "xyz", "ABab", "e+10101",
                "b1001", "10b01", "0x10", "1001x", "1001h",
                "0bb1111", "1111uu", "1111ulll",
                "101000101",
                "100000000",
                "SIN(01010)", "10+-1010101",
            };

            foreach (var data in bytePositiveInput)
                Assert.AreEqual(ValidateProgrammerBinBytePasteExpression(data), data);
            foreach (var data in byteNegativeInput)
                Assert.AreEqual(ValidateProgrammerBinBytePasteExpression(data), "NoOp");
        }

        [TestMethod]
        public void ProgrammerPrefixedValuesRespectByteAndWordRanges()
        {
            var cases = new[]
            {
                (Value: "0n127", Base: NumberBase.DecBase, Length: BitLength.BitLengthByte, Expected: "0n127"),
                (Value: "0n128", Base: NumberBase.DecBase, Length: BitLength.BitLengthByte, Expected: "NoOp"),
                (Value: "0n32767", Base: NumberBase.DecBase, Length: BitLength.BitLengthWord, Expected: "0n32767"),
                (Value: "0n32768", Base: NumberBase.DecBase, Length: BitLength.BitLengthWord, Expected: "NoOp"),
                (Value: "0o377", Base: NumberBase.OctBase, Length: BitLength.BitLengthByte, Expected: "0o377"),
                (Value: "0o400", Base: NumberBase.OctBase, Length: BitLength.BitLengthByte, Expected: "NoOp"),
                (Value: "0t177777", Base: NumberBase.OctBase, Length: BitLength.BitLengthWord, Expected: "0t177777"),
                (Value: "0t200000", Base: NumberBase.OctBase, Length: BitLength.BitLengthWord, Expected: "NoOp"),
                (Value: "0b11111111", Base: NumberBase.BinBase, Length: BitLength.BitLengthByte, Expected: "0b11111111"),
                (Value: "0b100000000", Base: NumberBase.BinBase, Length: BitLength.BitLengthByte, Expected: "NoOp"),
                (Value: "0y1111111111111111", Base: NumberBase.BinBase, Length: BitLength.BitLengthWord, Expected: "0y1111111111111111"),
                (Value: "0y10000000000000000", Base: NumberBase.BinBase, Length: BitLength.BitLengthWord, Expected: "NoOp"),
                (Value: "0xFF", Base: NumberBase.HexBase, Length: BitLength.BitLengthByte, Expected: "0xFF"),
                (Value: "0x100", Base: NumberBase.HexBase, Length: BitLength.BitLengthByte, Expected: "NoOp"),
                (Value: "0xFFFF", Base: NumberBase.HexBase, Length: BitLength.BitLengthWord, Expected: "0xFFFF"),
                (Value: "0x10000", Base: NumberBase.HexBase, Length: BitLength.BitLengthWord, Expected: "NoOp"),
            };

            foreach (var testCase in cases)
            {
                Assert.AreEqual(
                    testCase.Expected,
                    CopyPasteManager.ValidatePasteExpression(
                        testCase.Value,
                        ViewMode.Programmer,
                        testCase.Base,
                        testCase.Length),
                    testCase.Value);
            }
        }

        [TestMethod]
        public void BinaryZeroWithSuffixRemainsValid()
        {
            Assert.AreEqual("0b", ValidateProgrammerBinBytePasteExpression("0b"));
        }

        [TestMethod]
        public void ValidateConverterPasteExpressionTest()
        {
            string[] positiveInput =
            {
                "123", "+123", "-133", "12345.", "012.012", "1,234", "1 2 3",
                "\n\r1,234\n", "\f\n12\t\r\v\x85", "1\"2", "100=", "100=   ",
            };
            string[] negativeInput =
            {
                "(123)+(456)", "1.2e23", "12345e-23",
                "\n 1+\n2 ", "123+456", "abcdef", "\n 1+\n2 ",
                "xyz", "ABab", "e+234",
                "12345678912345678",
                "SIN(2)", "123+-234", "100==", "=100",
            };

            foreach (var data in positiveInput)
                Assert.AreEqual(ValidateConverterPasteExpression(data), data);
            foreach (var data in negativeInput)
                Assert.AreEqual(ValidateConverterPasteExpression(data), "NoOp");
        }

        [TestMethod]
        public void ValidatePasteExpressionErrorStates()
        {
            string exp_TooLong = "";
            for (uint i = 0; i < CopyPasteManager.MaxPasteableLength / 8; i++)
            {
                exp_TooLong += "-1234567";
            }
            Assert.AreEqual(
                CopyPasteManager.ValidatePasteExpression(
                    exp_TooLong, ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown),
                exp_TooLong);
            exp_TooLong += "1";
            Assert.AreEqual(
                CopyPasteManager.ValidatePasteExpression(
                    exp_TooLong, ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown),
                "NoOp");

            Assert.AreEqual(
                CopyPasteManager.ValidatePasteExpression(
                    "", ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown),
                "NoOp");

            Assert.AreEqual(
                CopyPasteManager.ValidatePasteExpression(
                    "1a23f456", ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown),
                "NoOp");

            Assert.AreEqual(
                CopyPasteManager.ValidatePasteExpression(
                    "123", ViewMode.None, CategoryGroupType.None, NumberBase.Unknown, BitLength.BitLengthUnknown),
                "NoOp");
        }

        [TestMethod]
        public void ValidateExtractOperands()
        {
            var oneOperand = new List<string> { "123456" };
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123456", ViewMode.Standard), oneOperand);

            oneOperand.Clear();
            oneOperand.Add("123^456");
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123^456", ViewMode.Standard), oneOperand);

            var twoOperands = new List<string> { "123", "456" };
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123+456", ViewMode.Standard), twoOperands);
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123-456", ViewMode.Standard), twoOperands);
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123*456", ViewMode.Standard), twoOperands);
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123/456", ViewMode.Standard), twoOperands);

            var expOperand = new List<string> { "123e456" };
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123e456", ViewMode.Standard), expOperand);
            expOperand.Clear();
            expOperand.Add("123e4567");
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123e4567", ViewMode.Standard), expOperand);

            var twoOperandsParens = new List<string> { "((45)", "(-30))" };
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("((45)+(-30))", ViewMode.Scientific), twoOperandsParens);
        }

        [TestMethod]
        public void ValidateExtractOperandsErrors()
        {
            string exp_OperandLimit = "";
            for (uint i = 0; i < CopyPasteManager.MaxOperandCount; i++)
            {
                exp_OperandLimit += "+1";
            }
            Assert.AreEqual(
                (uint)CopyPasteManager.ExtractOperands(exp_OperandLimit, ViewMode.Standard).Count,
                (uint)100);

            exp_OperandLimit += "+1";
            Assert.AreEqual(
                (uint)CopyPasteManager.ExtractOperands(exp_OperandLimit, ViewMode.Standard).Count,
                (uint)0);

            Assert.AreEqual(
                (uint)CopyPasteManager.ExtractOperands("12e9999", ViewMode.Standard).Count,
                (uint)1);
            Assert.AreEqual(
                (uint)CopyPasteManager.ExtractOperands("12e10000", ViewMode.Standard).Count,
                (uint)0);
        }

        [TestMethod]
        public void ValidateExpressionRegExMatch()
        {
            Assert.IsFalse(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string>(), ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsFalse(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "123" },
                    ViewMode.None, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsFalse(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "123" },
                    ViewMode.Currency, CategoryGroupType.None, NumberBase.Unknown, BitLength.BitLengthUnknown));

            // Verify operand lengths > max return false
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "12345678901234567" },
                ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "123456789012345678901234567890123" },
                ViewMode.Scientific, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "12345678901234567" },
                ViewMode.None, CategoryGroupType.Converter, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "11111111111111111" },
                ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.HexBase, BitLength.BitLengthQWord));
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "12345678901234567890" },
                ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.DecBase, BitLength.BitLengthQWord));
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "11111111111111111111111" },
                ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.OctBase, BitLength.BitLengthQWord));
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "10000000000000000000000000000000000000000000000000000000000000000" },
                ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.BinBase, BitLength.BitLengthQWord));

            Assert.IsFalse(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "9223372036854775808" },
                    ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.DecBase, BitLength.BitLengthQWord));

            Assert.IsTrue(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "((((((((((((((((((((123))))))))))))))))))))" },
                    ViewMode.Scientific, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsTrue(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "9223372036854775807" },
                    ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.DecBase, BitLength.BitLengthQWord));
            Assert.IsTrue(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "-9223372036854775808" },
                    ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.DecBase, BitLength.BitLengthQWord));

            // Verify all operands must match patterns
            Assert.IsTrue(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "123", "456" },
                ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsTrue(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "123", "1e23" },
                ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "123", "fab10" },
                ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));

            Assert.IsTrue(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "1.23e+456", "1.23e456", ".23e+456", "123e-456", "12e2", "12e+2", "12e-2", "-12e2", "-12e+2", "-12e-2" },
                    ViewMode.Scientific, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));

            Assert.IsFalse(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "123", "12345678901234567" },
                    ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsFalse(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "123", "9223372036854775808" },
                    ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.DecBase, BitLength.BitLengthQWord));
        }

        [TestMethod]
        public void ValidateGetMaxOperandLengthAndValue()
        {
            var result = CopyPasteManager.GetMaxOperandLengthAndValue(
                ViewMode.Standard, CategoryGroupType.None, NumberBase.Unknown, BitLength.BitLengthUnknown);
            Assert.IsTrue(result.MaxLength == CopyPasteManager.MaxStandardOperandLength);
            Assert.IsTrue(result.MaxValue == 0);

            result = CopyPasteManager.GetMaxOperandLengthAndValue(
                ViewMode.Scientific, CategoryGroupType.None, NumberBase.Unknown, BitLength.BitLengthUnknown);
            Assert.IsTrue(result.MaxLength == CopyPasteManager.MaxScientificOperandLength);
            Assert.IsTrue(result.MaxValue == 0);

            result = CopyPasteManager.GetMaxOperandLengthAndValue(
                ViewMode.None, CategoryGroupType.Converter, NumberBase.Unknown, BitLength.BitLengthUnknown);
            Assert.IsTrue(result.MaxLength == CopyPasteManager.MaxConverterInputLength);
            Assert.IsTrue(result.MaxValue == 0);

            ulong ullQwordMax = ulong.MaxValue;
            ulong ullDwordMax = uint.MaxValue;
            ulong ullWordMax = ushort.MaxValue;
            ulong ullByteMax = byte.MaxValue;

            // Hex
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.HexBase, BitLength.BitLengthQWord);
            Assert.IsTrue(result.MaxLength == 16u);
            Assert.IsTrue(result.MaxValue == ullQwordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.HexBase, BitLength.BitLengthDWord);
            Assert.IsTrue(result.MaxLength == 8u);
            Assert.IsTrue(result.MaxValue == ullDwordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.HexBase, BitLength.BitLengthWord);
            Assert.IsTrue(result.MaxLength == 4u);
            Assert.IsTrue(result.MaxValue == ullWordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.HexBase, BitLength.BitLengthByte);
            Assert.IsTrue(result.MaxLength == 2u);
            Assert.IsTrue(result.MaxValue == ullByteMax);

            // Dec
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.DecBase, BitLength.BitLengthQWord);
            Assert.IsTrue(result.MaxLength == 19u);
            Assert.IsTrue(result.MaxValue == ullQwordMax >> 1);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.DecBase, BitLength.BitLengthDWord);
            Assert.IsTrue(result.MaxLength == 10u);
            Assert.IsTrue(result.MaxValue == ullDwordMax >> 1);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.DecBase, BitLength.BitLengthWord);
            Assert.IsTrue(result.MaxLength == 5u);
            Assert.IsTrue(result.MaxValue == ullWordMax >> 1);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.DecBase, BitLength.BitLengthByte);
            Assert.IsTrue(result.MaxLength == 3u);
            Assert.IsTrue(result.MaxValue == ullByteMax >> 1);

            // Oct
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.OctBase, BitLength.BitLengthQWord);
            Assert.IsTrue(result.MaxLength == 22u);
            Assert.IsTrue(result.MaxValue == ullQwordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.OctBase, BitLength.BitLengthDWord);
            Assert.IsTrue(result.MaxLength == 11u);
            Assert.IsTrue(result.MaxValue == ullDwordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.OctBase, BitLength.BitLengthWord);
            Assert.IsTrue(result.MaxLength == 6u);
            Assert.IsTrue(result.MaxValue == ullWordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.OctBase, BitLength.BitLengthByte);
            Assert.IsTrue(result.MaxLength == 3u);
            Assert.IsTrue(result.MaxValue == ullByteMax);

            // Bin
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.BinBase, BitLength.BitLengthQWord);
            Assert.IsTrue(result.MaxLength == 64u);
            Assert.IsTrue(result.MaxValue == ullQwordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.BinBase, BitLength.BitLengthDWord);
            Assert.IsTrue(result.MaxLength == 32u);
            Assert.IsTrue(result.MaxValue == ullDwordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.BinBase, BitLength.BitLengthWord);
            Assert.IsTrue(result.MaxLength == 16u);
            Assert.IsTrue(result.MaxValue == ullWordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.BinBase, BitLength.BitLengthByte);
            Assert.IsTrue(result.MaxLength == 8u);
            Assert.IsTrue(result.MaxValue == ullByteMax);

            // Invalid
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.None, CategoryGroupType.None, NumberBase.Unknown, BitLength.BitLengthUnknown);
            Assert.IsTrue(result.MaxLength == 0u);
            Assert.IsTrue(result.MaxValue == 0UL);
        }

        [TestMethod]
        public void ValidateSanitizeOperand()
        {
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("((1234"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("1234))"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("1234))"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("-1234"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("+1234"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("-(1234)"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("+(1234)"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("12-34"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("((((1234))))"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("1'2'3'4"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("'''''1234''''"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("1_2_3_4"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("______1234___"), "1234");
        }

        [TestMethod]
        public void ValidatePrefixCurrencySymbols()
        {
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u00A5\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u00A4\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u20B5\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u0024\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u20A1\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u20A9\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u20AA\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u20A6\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u20B9\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u00A3\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u20AC\u0035"), "5");
        }

        [TestMethod]
        public void ValidateTryOperandToULL()
        {
            ulong? result = null;

            // Hex
            result = CopyPasteManager.TryOperandToULL("1234", NumberBase.HexBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0x1234UL);
            result = CopyPasteManager.TryOperandToULL("FF", NumberBase.HexBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFUL);
            result = CopyPasteManager.TryOperandToULL("FFFFFFFFFFFFFFFF", NumberBase.HexBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);
            result = CopyPasteManager.TryOperandToULL("0xFFFFFFFFFFFFFFFF", NumberBase.HexBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);
            result = CopyPasteManager.TryOperandToULL("0XFFFFFFFFFFFFFFFF", NumberBase.HexBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);
            result = CopyPasteManager.TryOperandToULL("0X0FFFFFFFFFFFFFFFF", NumberBase.HexBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);

            // Dec
            result = CopyPasteManager.TryOperandToULL("1234", NumberBase.DecBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 1234UL);
            result = CopyPasteManager.TryOperandToULL("18446744073709551615", NumberBase.DecBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);
            result = CopyPasteManager.TryOperandToULL("018446744073709551615", NumberBase.DecBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);

            // Oct
            result = CopyPasteManager.TryOperandToULL("777", NumberBase.OctBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 511UL); // 0777 octal
            result = CopyPasteManager.TryOperandToULL("0777", NumberBase.OctBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 511UL);
            result = CopyPasteManager.TryOperandToULL("1777777777777777777777", NumberBase.OctBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);
            result = CopyPasteManager.TryOperandToULL("01777777777777777777777", NumberBase.OctBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);

            // Bin
            result = CopyPasteManager.TryOperandToULL("1111", NumberBase.BinBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0b1111UL);
            result = CopyPasteManager.TryOperandToULL("0010", NumberBase.BinBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0b10UL);
            result = CopyPasteManager.TryOperandToULL("1111111111111111111111111111111111111111111111111111111111111111", NumberBase.BinBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);
            result = CopyPasteManager.TryOperandToULL("01111111111111111111111111111111111111111111111111111111111111111", NumberBase.BinBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);

            // Invalid / overflow
            Assert.IsNull(CopyPasteManager.TryOperandToULL("0xFFFFFFFFFFFFFFFFF1", NumberBase.HexBase));
            Assert.IsNull(CopyPasteManager.TryOperandToULL("18446744073709551616", NumberBase.DecBase));
            Assert.IsNull(CopyPasteManager.TryOperandToULL("2000000000000000000000", NumberBase.OctBase));
            Assert.IsNull(CopyPasteManager.TryOperandToULL("11111111111111111111111111111111111111111111111111111111111111111", NumberBase.BinBase));
            Assert.IsNull(CopyPasteManager.TryOperandToULL("-1", NumberBase.DecBase));
            Assert.IsNull(CopyPasteManager.TryOperandToULL("5555", NumberBase.BinBase));
            Assert.IsNull(CopyPasteManager.TryOperandToULL("xyz", NumberBase.BinBase));
        }

        [TestMethod]
        public void ValidateStandardScientificOperandLength()
        {
            Assert.AreEqual(CopyPasteManager.StandardScientificOperandLength(""), (uint)0);
            Assert.AreEqual(CopyPasteManager.StandardScientificOperandLength("0.2"), (uint)1);
            Assert.AreEqual(CopyPasteManager.StandardScientificOperandLength("1.2"), (uint)2);
            Assert.AreEqual(CopyPasteManager.StandardScientificOperandLength("0."), (uint)0);
            Assert.AreEqual(CopyPasteManager.StandardScientificOperandLength("12345"), (uint)5);
            Assert.AreEqual(CopyPasteManager.StandardScientificOperandLength("-12345"), (uint)6);
        }

        [TestMethod]
        public void ValidateProgrammerOperandLength()
        {
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("1001", NumberBase.BinBase), (uint)4);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("1001b", NumberBase.BinBase), (uint)4);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("1001B", NumberBase.BinBase), (uint)4);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0b1001", NumberBase.BinBase), (uint)4);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0B1001", NumberBase.BinBase), (uint)4);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0y1001", NumberBase.BinBase), (uint)4);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0Y1001", NumberBase.BinBase), (uint)4);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0b", NumberBase.BinBase), (uint)1);

            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("123456", NumberBase.OctBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0t123456", NumberBase.OctBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0T123456", NumberBase.OctBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0o123456", NumberBase.OctBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0O123456", NumberBase.OctBase), (uint)6);

            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("", NumberBase.DecBase), (uint)0);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("-", NumberBase.DecBase), (uint)0);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("12345", NumberBase.DecBase), (uint)5);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("-12345", NumberBase.DecBase), (uint)5);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0n12345", NumberBase.DecBase), (uint)5);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0N12345", NumberBase.DecBase), (uint)5);

            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("123ABC", NumberBase.HexBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0x123ABC", NumberBase.HexBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0X123ABC", NumberBase.HexBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("123ABCh", NumberBase.HexBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("123ABCH", NumberBase.HexBase), (uint)6);
        }

        #region Helper Methods

        private static void AssertListsAreEqual(IList<string> actual, IList<string> expected)
        {
            Assert.AreEqual(expected.Count, actual.Count);
            for (int i = 0; i < expected.Count; i++)
            {
                Assert.AreEqual(expected[i], actual[i]);
            }
        }

        private static string ValidateStandardPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Standard, NumberBase.Unknown, BitLength.BitLengthUnknown);
        }

        private static string ValidateScientificPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Scientific, NumberBase.Unknown, BitLength.BitLengthUnknown);
        }

        private static string ValidateConverterPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.None, CategoryGroupType.Converter, NumberBase.Unknown, BitLength.BitLengthUnknown);
        }

        private static string ValidateProgrammerHexQwordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.HexBase, BitLength.BitLengthQWord);
        }

        private static string ValidateProgrammerHexDwordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.HexBase, BitLength.BitLengthDWord);
        }

        private static string ValidateProgrammerHexWordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.HexBase, BitLength.BitLengthWord);
        }

        private static string ValidateProgrammerHexBytePasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.HexBase, BitLength.BitLengthByte);
        }

        private static string ValidateProgrammerDecQwordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.DecBase, BitLength.BitLengthQWord);
        }

        private static string ValidateProgrammerDecDwordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.DecBase, BitLength.BitLengthDWord);
        }

        private static string ValidateProgrammerDecWordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.DecBase, BitLength.BitLengthWord);
        }

        private static string ValidateProgrammerDecBytePasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.DecBase, BitLength.BitLengthByte);
        }

        private static string ValidateProgrammerOctQwordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.OctBase, BitLength.BitLengthQWord);
        }

        private static string ValidateProgrammerOctDwordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.OctBase, BitLength.BitLengthDWord);
        }

        private static string ValidateProgrammerOctWordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.OctBase, BitLength.BitLengthWord);
        }

        private static string ValidateProgrammerOctBytePasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.OctBase, BitLength.BitLengthByte);
        }

        private static string ValidateProgrammerBinQwordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.BinBase, BitLength.BitLengthQWord);
        }

        private static string ValidateProgrammerBinDwordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.BinBase, BitLength.BitLengthDWord);
        }

        private static string ValidateProgrammerBinWordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.BinBase, BitLength.BitLengthWord);
        }

        private static string ValidateProgrammerBinBytePasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.BinBase, BitLength.BitLengthByte);
        }

        #endregion
    }
```

#### 4.1.Z Remaining non-blank source in this file

<!-- CopyPasteManagerTests.cs:1179-1179 -->
```csharp
}
```

### 4.2 `src/Calculator.Tests/CurrencyConverterTests.cs`

1 `[TestClass]`(es), 17 test method(s).

| # | Test method | Attributes | Source lines |
|---|---|---|---|
| 1 | `LoadFromCache_Fail_NoCacheKey` | `[TestMethod]` | `CurrencyConverterTests.cs:19-31` |
| 2 | `InitialLoadAndRefreshSerializeCurrencyMutation` | `[TestMethod]` | `CurrencyConverterTests.cs:33-71` |
| 3 | `LoadFromCache_Success` | `[TestMethod]` | `CurrencyConverterTests.cs:121-132` |
| 4 | `LoadFromCache_Fail_StaticDataFileDoesNotExist` | `[TestMethod]` | `CurrencyConverterTests.cs:134-144` |
| 5 | `LoadFromCache_Fail_AllRatiosDataFileDoesNotExist` | `[TestMethod]` | `CurrencyConverterTests.cs:146-156` |
| 6 | `LoadFromCache_Fail_ResponseLanguageChanged` | `[TestMethod]` | `CurrencyConverterTests.cs:158-169` |
| 7 | `LoadFromCache_SortsCurrenciesByCountryName` | `[TestMethod]` | `CurrencyConverterTests.cs:171-203` |
| 8 | `Loaded_LoadOrderedUnits` | `[TestMethod]` | `CurrencyConverterTests.cs:205-218` |
| 9 | `Loaded_LoadOrderedRatios` | `[TestMethod]` | `CurrencyConverterTests.cs:220-232` |
| 10 | `Loaded_GetCurrencySymbols_Valid` | `[TestMethod]` | `CurrencyConverterTests.cs:234-244` |
| 11 | `Loaded_GetCurrencySymbols_Invalid` | `[TestMethod]` | `CurrencyConverterTests.cs:246-255` |
| 12 | `Loaded_GetCurrencyRatioEquality_Valid` | `[TestMethod]` | `CurrencyConverterTests.cs:257-271` |
| 13 | `Loaded_GetCurrencyRatioEquality_Invalid` | `[TestMethod]` | `CurrencyConverterTests.cs:273-282` |
| 14 | `LoadFromWeb_Success` | `[TestMethod]` | `CurrencyConverterTests.cs:284-294` |
| 15 | `Load_Success_LoadedFromCache` | `[TestMethod]` | `CurrencyConverterTests.cs:296-308` |
| 16 | `Load_Success_LoadedFromWeb` | `[TestMethod]` | `CurrencyConverterTests.cs:310-321` |
| 17 | `Test_RoundCurrencyRatio` | `[TestMethod]` | `CurrencyConverterTests.cs:324-363` |

#### 4.2.0 File preamble — `using` directives, file-scoped structs/helpers, namespace open (`CurrencyConverterTests.cs:1-15`)

```csharp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Globalization;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;
using CalculatorApp.ViewModel.DataLoaders;
using Windows.Storage;

namespace Calculator.Tests
{
```

#### 4.2.1 `[TestClass] CurrencyConverterLoadTests` — verbatim (`CurrencyConverterTests.cs:16-387`)

```csharp
    [TestClass]
    public class CurrencyConverterLoadTests
    {
        [TestMethod]
        public async Task LoadFromCache_Fail_NoCacheKey()
        {
            var localSettings = ApplicationData.Current.LocalSettings;
            localSettings.Values.Remove(CurrencyDataLoaderConstants.CacheTimestampKey);

            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();

            Assert.IsFalse(didLoad, "Loading from cache must fail when the cache timestamp key is absent");
            Assert.IsFalse(loader.LoadFinished());
            Assert.IsFalse(loader.LoadedFromCache());
        }

        [TestMethod]
        public async Task InitialLoadAndRefreshSerializeCurrencyMutation()
        {
            var firstEntry = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);
            var releaseFirstEntry = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);
            var initialLoadFinished = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);
            int entryCount = 0;
            int activeEntries = 0;
            int overlapDetected = 0;

            var loader = new CurrencyDataLoader("en-US");
            loader.LoadGateEnteredForTest = async () =>
            {
                if (Interlocked.Increment(ref activeEntries) > 1)
                {
                    Interlocked.Exchange(ref overlapDetected, 1);
                }

                int entry = Interlocked.Increment(ref entryCount);
                if (entry == 1)
                {
                    firstEntry.SetResult(true);
                    await releaseFirstEntry.Task;
                }
            };
            loader.LoadGateExitedForTest = () => Interlocked.Decrement(ref activeEntries);
            loader.SetViewModelCallback(new LoadCompletionCallback(initialLoadFinished));

            loader.LoadData();
            await firstEntry.Task;
            Task<bool> refresh = loader.TryLoadDataFromWebOverrideAsync();
            releaseFirstEntry.SetResult(true);

            await initialLoadFinished.Task;
            await refresh;

            Assert.AreEqual(0, overlapDetected);
            Assert.AreEqual(2, entryCount);
        }

        private static async Task<DateTimeOffset> PrimeCacheAsync(
            string languageCode = "en-US",
            DateTimeOffset? timestamp = null,
            bool writeStaticData = true,
            bool writeRatios = true,
            string staticDataOverride = null,
            string ratiosOverride = null)
        {
            var stamp = timestamp ?? DateTimeOffset.UtcNow;
            var localSettings = ApplicationData.Current.LocalSettings;
            localSettings.Values[CurrencyDataLoaderConstants.CacheTimestampKey] = stamp;
            localSettings.Values[CurrencyDataLoaderConstants.CacheLangcodeKey] = languageCode;

            var folder = ApplicationData.Current.LocalCacheFolder;
            var staticData = staticDataOverride ?? await new CurrencyHttpClient().GetCurrencyMetadataAsync();
            var ratios = ratiosOverride ?? await new CurrencyHttpClient().GetCurrencyRatiosAsync();

            await WriteOrDeleteAsync(folder, CurrencyDataLoaderConstants.StaticDataFilename, writeStaticData ? staticData : null);
            await WriteOrDeleteAsync(folder, CurrencyDataLoaderConstants.AllRatiosDataFilename, writeRatios ? ratios : null);

            return stamp;
        }

        private static async Task WriteOrDeleteAsync(StorageFolder folder, string name, string contents)
        {
            if (contents == null)
            {
                var existing = await folder.TryGetItemAsync(name);
                if (existing != null)
                {
                    await existing.DeleteAsync();
                }
                return;
            }

            var file = await folder.CreateFileAsync(name, CreationCollisionOption.ReplaceExisting);
            await FileIO.WriteTextAsync(file, contents);
        }

        private static async Task<CurrencyDataLoader> LoadedLoaderAsync()
        {
            await PrimeCacheAsync();
            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();
            Assert.IsTrue(didLoad, "Cache load failed while setting up the test.");
            return loader;
        }

        [TestMethod]
        public async Task LoadFromCache_Success()
        {
            await PrimeCacheAsync();

            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();

            Assert.IsTrue(didLoad);
            Assert.IsTrue(loader.LoadFinished());
            Assert.IsTrue(loader.LoadedFromCache());
        }

        [TestMethod]
        public async Task LoadFromCache_Fail_StaticDataFileDoesNotExist()
        {
            await PrimeCacheAsync(writeStaticData: false);

            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();

            Assert.IsFalse(didLoad, "A cache load with no static data file must fail.");
            Assert.IsFalse(loader.LoadedFromCache());
        }

        [TestMethod]
        public async Task LoadFromCache_Fail_AllRatiosDataFileDoesNotExist()
        {
            await PrimeCacheAsync(writeRatios: false);

            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();

            Assert.IsFalse(didLoad, "A cache load with no ratios file must fail.");
            Assert.IsFalse(loader.LoadedFromCache());
        }

        [TestMethod]
        public async Task LoadFromCache_Fail_ResponseLanguageChanged()
        {
            // The cached text is localized, so a cache written for another language cannot be used.
            await PrimeCacheAsync(languageCode: "ar-SA");

            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();

            Assert.IsFalse(didLoad, "A cache written for a different language must not be reused.");
            Assert.IsFalse(loader.LoadedFromCache());
        }

        [TestMethod]
        public async Task LoadFromCache_SortsCurrenciesByCountryName()
        {
            const string staticData = @"[
                {""CountryCode"":""AA"",""CountryName"":""Zebra"",""CurrencyCode"":""AAA"",""CurrencyName"":""Alpha Coin"",""CurrencySymbol"":""A""},
                {""CountryCode"":""ZZ"",""CountryName"":""Alpha"",""CurrencyCode"":""ZZZ"",""CurrencyName"":""Zulu Coin"",""CurrencySymbol"":""Z""},
                {""CountryCode"":""MM"",""CountryName"":""Éclair"",""CurrencyCode"":""MMM"",""CurrencyName"":""Mike Coin"",""CurrencySymbol"":""M""}
            ]";
            const string ratios = @"[
                {""Rt"":1.0,""An"":""AAA""},
                {""Rt"":2.0,""An"":""ZZZ""},
                {""Rt"":3.0,""An"":""MMM""}
            ]";

            var originalCulture = CultureInfo.CurrentCulture;
            try
            {
                CultureInfo.CurrentCulture = new CultureInfo("en-US");
                await PrimeCacheAsync(staticDataOverride: staticData, ratiosOverride: ratios);
                var loader = new CurrencyDataLoader("en-US");

                Assert.IsTrue(await loader.TryLoadDataFromCacheAsync());

                CollectionAssert.AreEqual(
                    new[] { "Alpha", "Éclair", "Zebra" },
                    loader.GetOrderedUnits(0).ConvertAll(unit => unit.CountryName));
            }
            finally
            {
                CultureInfo.CurrentCulture = originalCulture;
                await PrimeCacheAsync();
            }
        }

        [TestMethod]
        public async Task Loaded_LoadOrderedUnits()
        {
            var loader = await LoadedLoaderAsync();

            var units = loader.GetOrderedUnits(0);

            Assert.IsTrue(units.Count > 0, "No currency units were loaded.");
            foreach (var unit in units)
            {
                Assert.IsFalse(string.IsNullOrEmpty(unit.Abbreviation), "A currency unit had no abbreviation.");
                Assert.IsFalse(string.IsNullOrEmpty(unit.CountryName), "A currency unit had no country name.");
            }
        }

        [TestMethod]
        public async Task Loaded_LoadOrderedRatios()
        {
            var loader = await LoadedLoaderAsync();
            var units = loader.GetOrderedUnits(0);

            var ratios = loader.LoadOrderedRatios(units[0].Id);

            Assert.IsTrue(ratios.Count > 0, "The first currency unit had no ratios.");
            Assert.IsTrue(
                ratios.ContainsKey(units[0].Id),
                "A currency should always convert to itself.");
        }

        [TestMethod]
        public async Task Loaded_GetCurrencySymbols_Valid()
        {
            var loader = await LoadedLoaderAsync();
            var units = loader.GetOrderedUnits(0);

            var symbols = loader.GetCurrencySymbols(units[0].Id, units[1].Id);

            Assert.IsNotNull(symbols.Symbol1);
            Assert.IsNotNull(symbols.Symbol2);
        }

        [TestMethod]
        public async Task Loaded_GetCurrencySymbols_Invalid()
        {
            var loader = await LoadedLoaderAsync();

            var symbols = loader.GetCurrencySymbols(-1, -2);

            Assert.AreEqual(string.Empty, symbols.Symbol1, "An unknown unit must not yield a symbol.");
            Assert.AreEqual(string.Empty, symbols.Symbol2);
        }

        [TestMethod]
        public async Task Loaded_GetCurrencyRatioEquality_Valid()
        {
            var loader = await LoadedLoaderAsync();
            var units = loader.GetOrderedUnits(0);

            var equality = loader.GetCurrencyRatioEquality(units[0].Id, units[1].Id);

            Assert.IsFalse(string.IsNullOrEmpty(equality.Ratio1), "The ratio line was empty.");
            Assert.IsFalse(string.IsNullOrEmpty(equality.Ratio2), "The accessible ratio line was empty.");

            // The accessible form spells the currencies out rather than abbreviating them.
            StringAssert.Contains(equality.Ratio2, units[0].CountryName);
            StringAssert.Contains(equality.Ratio2, units[0].Name);
        }

        [TestMethod]
        public async Task Loaded_GetCurrencyRatioEquality_Invalid()
        {
            var loader = await LoadedLoaderAsync();

            var equality = loader.GetCurrencyRatioEquality(-1, -2);

            Assert.AreEqual(string.Empty, equality.Ratio1);
            Assert.AreEqual(string.Empty, equality.Ratio2);
        }

        [TestMethod]
        public async Task LoadFromWeb_Success()
        {
            var loader = new CurrencyDataLoader("en-US");

            bool didLoad = await loader.TryLoadDataFromWebAsync();

            Assert.IsTrue(didLoad);
            Assert.IsTrue(loader.LoadFinished());
            Assert.IsTrue(loader.LoadedFromWeb());
        }

        [TestMethod]
        public async Task Load_Success_LoadedFromCache()
        {
            // A cache written moments ago is still fresh, so the loader uses it and never goes out.
            await PrimeCacheAsync();

            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();

            Assert.IsTrue(didLoad);
            Assert.IsTrue(loader.LoadedFromCache());
            Assert.IsFalse(loader.LoadedFromWeb());
        }

        [TestMethod]
        public async Task Load_Success_LoadedFromWeb()
        {
            // A cache older than a day is refreshed from the web rather than used as-is.
            await PrimeCacheAsync(timestamp: DateTimeOffset.UtcNow.AddDays(-2));

            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();

            Assert.IsTrue(didLoad);
            Assert.IsTrue(loader.LoadedFromWeb(), "A stale cache should have been refreshed from the web.");
        }


        [TestMethod]
        public void Test_RoundCurrencyRatio()
        {
            (double Ratio, double Expected)[] cases =
            {
                (1234567, 1234567),
                (0, 0),
                (9999.999, 9999.999),
                (8765.4321, 8765.4321),
                (4815.162342, 4815.1623),
                (4815.162358, 4815.1624),
                (4815.162388934723, 4815.1624),
                (0.12, 0.12),
                (0.123, 0.123),
                (0.1234, 0.1234),
                (0.12343, 0.1234),
                (0.0321, 0.0321),
                (0.03211, 0.03211),
                (0.032119, 0.03212),
                (0.00322119, 0.003221),
                (0.00123269, 0.001233),
                (0.00076269, 0.0007627),
                (0.000069, 0.000069),
                (0.000061, 0.000061),
                (0.000054612, 0.00005461),
                (0.000054616, 0.00005462),
                (0.000005416, 0.000005416),
                (0.0000016134324, 0.000001613),
                (0.0000096134324, 0.000009613),
                (0.0000032169348392, 0.000003217),
                (0.000000002134987218, 0.000000002135),
                (0.000000000000087231445, 0.00000000000008723),
            };

            foreach (var (ratio, expected) in cases)
            {
                Assert.AreEqual(expected, CurrencyDataLoader.RoundCurrencyRatio(ratio), 0d,
                    $"RoundCurrencyRatio({ratio})");
            }
        }

        private sealed class LoadCompletionCallback : IViewModelCurrencyCallback
        {
            private readonly TaskCompletionSource<bool> _completion;

            public LoadCompletionCallback(TaskCompletionSource<bool> completion)
            {
                _completion = completion;
            }

            public void CurrencyDataLoadFinished(bool didLoad)
            {
                _completion.TrySetResult(didLoad);
            }

            public void CurrencyTimestampUpdated(string timestamp, bool isWeekOld)
            {
            }

            public void NetworkBehaviorChanged(NetworkAccessBehavior newBehavior)
            {
            }
        }
    }
```

#### 4.2.Z Remaining non-blank source in this file

<!-- CurrencyConverterTests.cs:388-388 -->
```csharp
}
```

### 4.3 `src/Calculator.Tests/DateCalculatorTests.cs`

2 `[TestClass]`(es), 28 test method(s).

| # | Test method | Attributes | Source lines |
|---|---|---|---|
| 1 | `TestDateDiff` | `[TestMethod]` | `DateCalculatorTests.cs:107-128` |
| 2 | `OneYearDifferenceEndingAtTheTopOfRangeIsCalculated` | `[TestMethod]` | `DateCalculatorTests.cs:130-141` |
| 3 | `FullSupportedRangeReturnsYearMonthDayDifference` | `[TestMethod]` | `DateCalculatorTests.cs:143-155` |
| 4 | `RangeEndingInYear9998ReturnsYearMonthDayDifference` | `[TestMethod]` | `DateCalculatorTests.cs:157-169` |
| 5 | `TestAddOob` | `[TestMethod]` | `DateCalculatorTests.cs:183-194` |
| 6 | `TestSubtractOob` | `[TestMethod]` | `DateCalculatorTests.cs:196-209` |
| 7 | `TestAddition` | `[TestMethod]` | `DateCalculatorTests.cs:211-226` |
| 8 | `TestSubtraction` | `[TestMethod]` | `DateCalculatorTests.cs:228-243` |
| 9 | `DateCalcViewModelInitializationTest` | `[TestMethod]` | `DateCalculatorTests.cs:333-353` |
| 10 | `DateCalcViewModelAddSubtractInitTest` | `[TestMethod]` | `DateCalculatorTests.cs:355-378` |
| 11 | `DateCalcViewModelDateDiffDaylightSavingTimeTest` | `[TestMethod]` | `DateCalculatorTests.cs:380-414` |
| 12 | `DateCalcViewModelAddTest` | `[TestMethod]` | `DateCalculatorTests.cs:416-436` |
| 13 | `DateCalcViewModelSubtractTest` | `[TestMethod]` | `DateCalculatorTests.cs:438-458` |
| 14 | `DateCalcViewModelAddOobTest` | `[TestMethod]` | `DateCalculatorTests.cs:460-480` |
| 15 | `DateCalcViewModelSubtractOobTest` | `[TestMethod]` | `DateCalculatorTests.cs:491-511` |
| 16 | `DateCalcViewModelDateDiffIgnoreSignTest` | `[TestMethod]` | `DateCalculatorTests.cs:513-534` |
| 17 | `DateCalcViewModelRangeEndingAtMaximumShowsYearsMonthsAndTotalDays` | `[TestMethod]` | `DateCalculatorTests.cs:536-554` |
| 18 | `DateCalcViewModelDateDiffResultInPositiveDaysTest` | `[TestMethod]` | `DateCalculatorTests.cs:556-570` |
| 19 | `DateCalcViewModelDateDiffAutomationNameUsesLocalizedFormat` | `[TestMethod]` | `DateCalculatorTests.cs:572-585` |
| 20 | `DateCalcViewModelDateDiffFromDateHigherThanToDate` | `[TestMethod]` | `DateCalculatorTests.cs:587-601` |
| 21 | `DateCalcViewModelPreservesCalendarDateForOffsetPickerValues` | `[TestMethod]` | `DateCalculatorTests.cs:603-615` |
| 22 | `DateCalcViewModelAddSubtractResultAutomationNameTest` | `[TestMethod]` | `DateCalculatorTests.cs:617-631` |
| 23 | `JaEraTransitionAddition` | `[TestMethod]` | `DateCalculatorTests.cs:633-643` |
| 24 | `JaEraTransitionSubtraction` | `[TestMethod]` | `DateCalculatorTests.cs:645-654` |
| 25 | `JaEraTransitionDifference` | `[TestMethod]` | `DateCalculatorTests.cs:656-671` |
| 26 | `JapaneseCalendarSubtractionBelowSupportedRangeReturnsNullAndRecovers` | `[TestMethod]` | `DateCalculatorTests.cs:673-687` |
| 27 | `UmAlQuraRangeAtCalendarLimitReturnsYearMonthDayDifference` | `[TestMethod]` | `DateCalculatorTests.cs:689-703` |
| 28 | `JapaneseDifferenceRecoveryPreservesTheCalendarSystem` | `[TestMethod]` | `DateCalculatorTests.cs:705-718` |

#### 4.3.0 File preamble — `using` directives, file-scoped structs/helpers, namespace open (`DateCalculatorTests.cs:1-20`)

```csharp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;
using CalculatorApp.ViewModel.Common.DateCalculation;

namespace Calculator.Tests
{
    #region Test Data

    internal struct DateTimeTestCase
    {
        public DateTimeOffset StartDate;
        public DateTimeOffset EndDate;
        public DateDifference DateDiff;
    }
```

#### 4.3.1 `[TestClass] DateCalculatorUnitTests` — verbatim (`DateCalculatorTests.cs:21-244`)

```csharp
    #endregion

    [TestClass]
    public class DateCalculatorUnitTests
    {
        private static DateCalculationEngine s_dateCalcEngine;

        private static readonly DateTimeOffset[] s_date = new DateTimeOffset[15];
        private static readonly DateDifference[] s_dateDifference = new DateDifference[14];
        private static readonly DateTimeTestCase[] s_datetimeDifftest = new DateTimeTestCase[9];
        private static readonly DateTimeTestCase[] s_datetimeBoundAdd = new DateTimeTestCase[2];
        private static readonly DateTimeTestCase[] s_datetimeBoundSubtract = new DateTimeTestCase[2];
        private static readonly DateTimeTestCase[] s_datetimeAddCase = new DateTimeTestCase[3];
        private static readonly DateTimeTestCase[] s_datetimeSubtractCase = new DateTimeTestCase[3];

        private static DateTimeOffset MakeDate(int year, int month, int day) =>
            new DateTimeOffset(year, month, day, 0, 0, 0, TimeSpan.Zero);

        [ClassInitialize]
        public static void TestClassSetup(TestContext context)
        {
            s_dateCalcEngine = new DateCalculationEngine("GregorianCalendar");

            // Dates - DD.MM.YYYY
            s_date[0]  = MakeDate(9999, 12, 31);
            s_date[1]  = MakeDate(9999, 12, 30);
            s_date[2]  = MakeDate(9998, 12, 31);
            s_date[3]  = MakeDate(1601,  1,  1);
            s_date[4]  = MakeDate(1601,  1,  2);
            s_date[5]  = MakeDate(2008,  5, 10);
            s_date[6]  = MakeDate(2008,  3, 10);
            s_date[7]  = MakeDate(2008,  2, 29);
            s_date[8]  = MakeDate(2007,  2, 28);
            s_date[9]  = MakeDate(2007,  3, 10);
            s_date[10] = MakeDate(2007,  5, 10);
            s_date[11] = MakeDate(2008,  1, 29);
            s_date[12] = MakeDate(2007,  1, 28);
            s_date[13] = MakeDate(2008,  1, 31);
            s_date[14] = MakeDate(2008,  3, 31);

            // Date Differences
            s_dateDifference[0]  = new DateDifference { Year = 1, Month = 1 };
            s_dateDifference[1]  = new DateDifference { Month = 1, Day = 10 };
            s_dateDifference[2]  = new DateDifference { Day = 2 };
            s_dateDifference[3]  = new DateDifference { Week = 52, Day = 1 };
            s_dateDifference[4]  = new DateDifference { Year = 1 };
            s_dateDifference[5]  = new DateDifference { Day = 365 };
            s_dateDifference[6]  = new DateDifference { Month = 1 };
            s_dateDifference[7]  = new DateDifference { Month = 1, Day = 2 };
            s_dateDifference[8]  = new DateDifference { Day = 31 };
            s_dateDifference[9]  = new DateDifference { Month = 11, Day = 1 };
            s_dateDifference[10] = new DateDifference { Year = 8398, Month = 11, Day = 30 };
            s_dateDifference[11] = new DateDifference { Year = 2008 };
            s_dateDifference[12] = new DateDifference { Year = 7991, Month = 11 };
            s_dateDifference[13] = new DateDifference { Week = 416998, Day = 1 };

            // Date Difference test cases
            s_datetimeDifftest[0] = new DateTimeTestCase { StartDate = s_date[0],  EndDate = s_date[3],  DateDiff = s_dateDifference[10] };
            s_datetimeDifftest[1] = new DateTimeTestCase { StartDate = s_date[0],  EndDate = s_date[2],  DateDiff = s_dateDifference[5] };
            s_datetimeDifftest[2] = new DateTimeTestCase { StartDate = s_date[0],  EndDate = s_date[2],  DateDiff = s_dateDifference[4] };
            s_datetimeDifftest[3] = new DateTimeTestCase { StartDate = s_date[0],  EndDate = s_date[2],  DateDiff = s_dateDifference[3] };
            s_datetimeDifftest[4] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[7],  DateDiff = s_dateDifference[7] };
            s_datetimeDifftest[5] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[7],  DateDiff = s_dateDifference[8] };
            s_datetimeDifftest[6] = new DateTimeTestCase { StartDate = s_date[11], EndDate = s_date[8],  DateDiff = s_dateDifference[9] };
            s_datetimeDifftest[7] = new DateTimeTestCase { StartDate = s_date[13], EndDate = s_date[0],  DateDiff = s_dateDifference[12] };
            s_datetimeDifftest[8] = new DateTimeTestCase { StartDate = s_date[13], EndDate = s_date[0],  DateDiff = s_dateDifference[13] };

            // Date Add Out of Bound test cases
            s_datetimeBoundAdd[0] = new DateTimeTestCase { StartDate = s_date[1], EndDate = s_date[0], DateDiff = s_dateDifference[2] };
            s_datetimeBoundAdd[1] = new DateTimeTestCase { StartDate = s_date[2], EndDate = s_date[0], DateDiff = s_dateDifference[11] };

            // Date Subtract Out of Bound test cases
            s_datetimeBoundSubtract[0] = new DateTimeTestCase { StartDate = s_date[3],  EndDate = s_date[0], DateDiff = s_dateDifference[2] };
            s_datetimeBoundSubtract[1] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[0], DateDiff = s_dateDifference[11] };

            // Date Add test cases
            s_datetimeAddCase[0] = new DateTimeTestCase { StartDate = s_date[13], EndDate = s_date[7], DateDiff = s_dateDifference[6] };
            s_datetimeAddCase[1] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[5], DateDiff = s_dateDifference[1] };
            s_datetimeAddCase[2] = new DateTimeTestCase { StartDate = s_date[13], EndDate = s_date[6], DateDiff = s_dateDifference[1] };

            // Date Subtract test cases
            s_datetimeSubtractCase[0] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[7], DateDiff = s_dateDifference[6] };
            s_datetimeSubtractCase[1] = new DateTimeTestCase { StartDate = s_date[6],  EndDate = s_date[11], DateDiff = s_dateDifference[1] };
            s_datetimeSubtractCase[2] = new DateTimeTestCase { StartDate = s_date[9],  EndDate = s_date[12], DateDiff = s_dateDifference[1] };
        }

        [TestMethod]
        public void TestDateDiff()
        {
            for (int testIndex = 0; testIndex < s_datetimeDifftest.Length; testIndex++)
            {
                var testCase = s_datetimeDifftest[testIndex];
                var requested = UnitsPresentIn(testCase.DateDiff);

                // The engine takes the dates in order; putting them the other way round is the view
                // model's job to normalize, which DateCalcViewModelDateDiffIgnoreSignTest covers.
                var earlier = testCase.StartDate <= testCase.EndDate ? testCase.StartDate : testCase.EndDate;
                var later = testCase.StartDate <= testCase.EndDate ? testCase.EndDate : testCase.StartDate;

                var difference = s_dateCalcEngine.TryGetDateDifference(earlier, later, requested);

                Assert.IsNotNull(difference, $"TryGetDateDifference returned null for case {testIndex}");
                Assert.AreEqual(testCase.DateDiff.Year, difference.Value.Year, $"year, case {testIndex}");
                Assert.AreEqual(testCase.DateDiff.Month, difference.Value.Month, $"month, case {testIndex}");
                Assert.AreEqual(testCase.DateDiff.Week, difference.Value.Week, $"week, case {testIndex}");
                Assert.AreEqual(testCase.DateDiff.Day, difference.Value.Day, $"day, case {testIndex}");
            }
        }

        [TestMethod]
        public void OneYearDifferenceEndingAtTheTopOfRangeIsCalculated()
        {
            var difference = s_dateCalcEngine.TryGetDateDifference(
                MakeDate(9998, 12, 31),
                MakeDate(9999, 12, 31),
                DateUnit.Year);

            Assert.IsNotNull(difference);
            Assert.AreEqual(1, difference.Value.Year);
            Assert.AreEqual(0, difference.Value.Day);
        }

        [TestMethod]
        public void FullSupportedRangeReturnsYearMonthDayDifference()
        {
            var difference = s_dateCalcEngine.TryGetDateDifference(
                MakeDate(1601, 1, 1),
                MakeDate(9999, 12, 31),
                DateUnit.Year | DateUnit.Month | DateUnit.Day);

            Assert.IsNotNull(difference);
            Assert.AreEqual(8398, difference.Value.Year);
            Assert.AreEqual(11, difference.Value.Month);
            Assert.AreEqual(30, difference.Value.Day);
        }

        [TestMethod]
        public void RangeEndingInYear9998ReturnsYearMonthDayDifference()
        {
            var difference = s_dateCalcEngine.TryGetDateDifference(
                MakeDate(1601, 1, 1),
                MakeDate(9998, 12, 31),
                DateUnit.Year | DateUnit.Month | DateUnit.Day);

            Assert.IsNotNull(difference);
            Assert.AreEqual(8397, difference.Value.Year);
            Assert.AreEqual(11, difference.Value.Month);
            Assert.AreEqual(30, difference.Value.Day);
        }

        // The request has to name the same units the expectation uses, or the engine reduces the
        // answer into ones the case never set.
        private static DateUnit UnitsPresentIn(DateDifference difference)
        {
            DateUnit units = 0;
            if (difference.Year != 0) units |= DateUnit.Year;
            if (difference.Month != 0) units |= DateUnit.Month;
            if (difference.Week != 0) units |= DateUnit.Week;
            if (difference.Day != 0) units |= DateUnit.Day;
            return units;
        }

        [TestMethod]
        public void TestAddOob()
        {
            for (int testIndex = 0; testIndex < s_datetimeBoundAdd.Length; testIndex++)
            {
                var endDate = s_dateCalcEngine.AddDuration(
                    s_datetimeBoundAdd[testIndex].StartDate,
                    s_datetimeBoundAdd[testIndex].DateDiff);

                Assert.IsNull(endDate, $"AddDuration should return null for out-of-bound case {testIndex}");
            }
        }

        [TestMethod]
        public void TestSubtractOob()
        {
            for (int testIndex = 0; testIndex < s_datetimeBoundSubtract.Length; testIndex++)
            {
                // Subtract Duration
                var endDate = s_dateCalcEngine.SubtractDuration(
                    s_datetimeBoundSubtract[testIndex].StartDate,
                    s_datetimeBoundSubtract[testIndex].DateDiff);

                // Assert for the result
                Assert.IsNull(endDate, $"SubtractDuration should return null for out-of-bound case {testIndex}");
            }
        }

        [TestMethod]
        public void TestAddition()
        {
            for (int testIndex = 0; testIndex < s_datetimeAddCase.Length; testIndex++)
            {
                var endDate = s_dateCalcEngine.AddDuration(
                    s_datetimeAddCase[testIndex].StartDate,
                    s_datetimeAddCase[testIndex].DateDiff);

                Assert.IsNotNull(endDate, $"AddDuration returned null for case {testIndex}");
                Assert.AreEqual(
                    s_datetimeAddCase[testIndex].EndDate,
                    endDate.Value,
                    $"AddDuration produced the wrong date for case {testIndex}");
            }
        }

        [TestMethod]
        public void TestSubtraction()
        {
            for (int testIndex = 0; testIndex < s_datetimeSubtractCase.Length; testIndex++)
            {
                var endDate = s_dateCalcEngine.SubtractDuration(
                    s_datetimeSubtractCase[testIndex].StartDate,
                    s_datetimeSubtractCase[testIndex].DateDiff);

                Assert.IsNotNull(endDate, $"SubtractDuration returned null for case {testIndex}");
                Assert.AreEqual(
                    s_datetimeSubtractCase[testIndex].EndDate,
                    endDate.Value,
                    $"SubtractDuration produced the wrong date for case {testIndex}");
            }
        }
    }
```

#### 4.3.2 `[TestClass] DateCalculatorViewModelTests` — verbatim (`DateCalculatorTests.cs:246-719`)

```csharp
    [TestClass]
    public class DateCalculatorViewModelTests
    {
        private static readonly DateTimeOffset[] s_date = new DateTimeOffset[15];
        private static readonly DateDifference[] s_dateDifference = new DateDifference[14];
        private static readonly DateTimeTestCase[] s_datetimeDifftest = new DateTimeTestCase[9];
        private static readonly DateTimeTestCase[] s_datetimeBoundAdd = new DateTimeTestCase[2];
        private static readonly DateTimeTestCase[] s_datetimeBoundSubtract = new DateTimeTestCase[2];
        private static readonly DateTimeTestCase[] s_datetimeAddCase = new DateTimeTestCase[3];
        private static readonly DateTimeTestCase[] s_datetimeSubtractCase = new DateTimeTestCase[3];

        private static DateTimeOffset MakeDate(int year, int month, int day) =>
            new DateTimeOffset(year, month, day, 0, 0, 0, TimeSpan.Zero);

        private static string LocalizeNumber(int value)
        {
            string result = value.ToString();
            LocalizationSettings.GetInstance().LocalizeDisplayValue(ref result);
            return result;
        }

        [ClassInitialize]
        public static void TestClassSetup(TestContext context)
        {
            // Dates - DD.MM.YYYY
            s_date[0]  = MakeDate(9999, 12, 31);
            s_date[1]  = MakeDate(9999, 12, 30);
            s_date[2]  = MakeDate(9998, 12, 31);
            s_date[3]  = MakeDate(1601,  1,  1);
            s_date[4]  = MakeDate(1601,  1,  2);
            s_date[5]  = MakeDate(2008,  5, 10);
            s_date[6]  = MakeDate(2008,  3, 10);
            s_date[7]  = MakeDate(2008,  2, 29);
            s_date[8]  = MakeDate(2007,  2, 28);
            s_date[9]  = MakeDate(2007,  3, 10);
            s_date[10] = MakeDate(2007,  5, 10);
            s_date[11] = MakeDate(2008,  1, 29);
            s_date[12] = MakeDate(2007,  1, 28);
            s_date[13] = MakeDate(2008,  1, 31);
            s_date[14] = MakeDate(2008,  3, 31);

            // Date Differences
            s_dateDifference[0]  = new DateDifference { Year = 1, Month = 1 };
            s_dateDifference[1]  = new DateDifference { Month = 1, Day = 10 };
            s_dateDifference[2]  = new DateDifference { Day = 2 };
            s_dateDifference[3]  = new DateDifference { Week = 52, Day = 1 };
            s_dateDifference[4]  = new DateDifference { Year = 1 };
            s_dateDifference[5]  = new DateDifference { Day = 365 };
            s_dateDifference[6]  = new DateDifference { Month = 1 };
            s_dateDifference[7]  = new DateDifference { Month = 1, Day = 2 };
            s_dateDifference[8]  = new DateDifference { Day = 31 };
            s_dateDifference[9]  = new DateDifference { Month = 11, Day = 1 };
            s_dateDifference[10] = new DateDifference { Year = 8398, Month = 11, Day = 30 };
            s_dateDifference[11] = new DateDifference { Year = 2008 };
            s_dateDifference[12] = new DateDifference { Year = 7991, Month = 11 };
            s_dateDifference[13] = new DateDifference { Week = 416998, Day = 1 };

            // Date Difference test cases
            s_datetimeDifftest[0] = new DateTimeTestCase { StartDate = s_date[0],  EndDate = s_date[3],  DateDiff = s_dateDifference[10] };
            s_datetimeDifftest[1] = new DateTimeTestCase { StartDate = s_date[0],  EndDate = s_date[2],  DateDiff = s_dateDifference[5] };
            s_datetimeDifftest[2] = new DateTimeTestCase { StartDate = s_date[0],  EndDate = s_date[2],  DateDiff = s_dateDifference[4] };
            s_datetimeDifftest[3] = new DateTimeTestCase { StartDate = s_date[0],  EndDate = s_date[2],  DateDiff = s_dateDifference[3] };
            s_datetimeDifftest[4] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[7],  DateDiff = s_dateDifference[7] };
            s_datetimeDifftest[5] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[7],  DateDiff = s_dateDifference[8] };
            s_datetimeDifftest[6] = new DateTimeTestCase { StartDate = s_date[11], EndDate = s_date[8],  DateDiff = s_dateDifference[9] };
            s_datetimeDifftest[7] = new DateTimeTestCase { StartDate = s_date[13], EndDate = s_date[0],  DateDiff = s_dateDifference[12] };
            s_datetimeDifftest[8] = new DateTimeTestCase { StartDate = s_date[13], EndDate = s_date[0],  DateDiff = s_dateDifference[13] };

            // Date Add Out of Bound test cases
            s_datetimeBoundAdd[0] = new DateTimeTestCase { StartDate = s_date[1], EndDate = s_date[0], DateDiff = s_dateDifference[2] };
            s_datetimeBoundAdd[1] = new DateTimeTestCase { StartDate = s_date[2], EndDate = s_date[0], DateDiff = s_dateDifference[11] };

            // Date Subtract Out of Bound test cases
            s_datetimeBoundSubtract[0] = new DateTimeTestCase { StartDate = s_date[3],  EndDate = s_date[0], DateDiff = s_dateDifference[2] };
            s_datetimeBoundSubtract[1] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[0], DateDiff = s_dateDifference[11] };

            // Date Add test cases
            s_datetimeAddCase[0] = new DateTimeTestCase { StartDate = s_date[13], EndDate = s_date[7], DateDiff = s_dateDifference[6] };
            s_datetimeAddCase[1] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[5], DateDiff = s_dateDifference[1] };
            s_datetimeAddCase[2] = new DateTimeTestCase { StartDate = s_date[13], EndDate = s_date[6], DateDiff = s_dateDifference[1] };

            // Date Subtract test cases
            s_datetimeSubtractCase[0] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[7], DateDiff = s_dateDifference[6] };
            s_datetimeSubtractCase[1] = new DateTimeTestCase { StartDate = s_date[6],  EndDate = s_date[11], DateDiff = s_dateDifference[1] };
            s_datetimeSubtractCase[2] = new DateTimeTestCase { StartDate = s_date[9],  EndDate = s_date[12], DateDiff = s_dateDifference[1] };
        }

        [TestMethod]
        public void DateCalcViewModelInitializationTest()
        {
            var viewModel = new DateCalculatorViewModel();

            Assert.IsTrue(viewModel.IsDateDiffMode);
            Assert.IsTrue(viewModel.IsAddMode);

            Assert.AreNotEqual(default(DateTimeOffset), viewModel.FromDate);
            Assert.AreNotEqual(default(DateTimeOffset), viewModel.ToDate);
            Assert.AreNotEqual(default(DateTimeOffset), viewModel.StartDate);

            Assert.AreEqual(0, viewModel.DaysOffset);
            Assert.AreEqual(0, viewModel.MonthsOffset);
            Assert.AreEqual(0, viewModel.YearsOffset);

            Assert.IsTrue(viewModel.IsDiffInDays);
            Assert.AreEqual("Same dates", viewModel.StrDateDiffResult);
            Assert.AreEqual(string.Empty, viewModel.StrDateDiffResultInDays);
            Assert.AreEqual(string.Empty, viewModel.StrDateResult);
        }

        [TestMethod]
        public void DateCalcViewModelAddSubtractInitTest()
        {
            var viewModel = new DateCalculatorViewModel();
            viewModel.IsDateDiffMode = false;

            Assert.IsFalse(viewModel.IsDateDiffMode);
            Assert.IsTrue(viewModel.IsAddMode);

            Assert.AreNotEqual(default(DateTimeOffset), viewModel.FromDate);
            Assert.AreNotEqual(default(DateTimeOffset), viewModel.ToDate);
            Assert.AreNotEqual(default(DateTimeOffset), viewModel.StartDate);

            Assert.AreEqual(0, viewModel.DaysOffset);
            Assert.AreEqual(0, viewModel.MonthsOffset);
            Assert.AreEqual(0, viewModel.YearsOffset);

            Assert.IsTrue(viewModel.IsDiffInDays);
            Assert.AreEqual("Same dates", viewModel.StrDateDiffResult);
            Assert.AreEqual(string.Empty, viewModel.StrDateDiffResultInDays);

            Assert.IsNotNull(viewModel.StrDateResult);
            Assert.AreNotEqual("", viewModel.StrDateResult);
        }

        [TestMethod]
        public void DateCalcViewModelDateDiffDaylightSavingTimeTest()
        {
            var viewModel = new DateCalculatorViewModel();
            viewModel.IsDateDiffMode = true;
            Assert.IsTrue(viewModel.IsDateDiffMode);

            // 31.03.2008 -> 29.02.2008
            viewModel.FromDate = s_datetimeDifftest[5].StartDate;
            viewModel.ToDate = s_datetimeDifftest[5].EndDate;

            // Assert for the result
            Assert.IsFalse(viewModel.IsDiffInDays);
            Assert.AreEqual("31 days", viewModel.StrDateDiffResultInDays);
            Assert.AreEqual("1 month, 2 days", viewModel.StrDateDiffResult);

            // Daylight Saving Time - Clock Forward
            // 10.03.2019 -> 11.03.2019
            viewModel.FromDate = MakeDate(2019, 3, 10);
            viewModel.ToDate = MakeDate(2019, 3, 11);
            Assert.IsTrue(viewModel.IsDiffInDays);
            Assert.AreEqual("1 day", viewModel.StrDateDiffResult);

            // 10.03.2019 -> 17.03.2019
            viewModel.ToDate = MakeDate(2019, 3, 17);
            Assert.IsFalse(viewModel.IsDiffInDays);
            Assert.AreEqual("1 week", viewModel.StrDateDiffResult);

            // Daylight Saving Time - Clock Backward
            // 03.11.2019 -> 04.11.2019
            viewModel.FromDate = MakeDate(2019, 11, 3);
            viewModel.ToDate = MakeDate(2019, 11, 4);
            Assert.IsTrue(viewModel.IsDiffInDays);
            Assert.AreEqual("1 day", viewModel.StrDateDiffResult);
        }

        [TestMethod]
        public void DateCalcViewModelAddTest()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = false;
            viewModel.IsAddMode = true;

            for (int testIndex = 0; testIndex < s_datetimeAddCase.Length; testIndex++)
            {
                viewModel.StartDate = s_datetimeAddCase[testIndex].StartDate;
                viewModel.DaysOffset = s_datetimeAddCase[testIndex].DateDiff.Day;
                viewModel.MonthsOffset = s_datetimeAddCase[testIndex].DateDiff.Month;
                viewModel.YearsOffset = s_datetimeAddCase[testIndex].DateDiff.Year;

                Assert.AreEqual(
                    ExpectedDateString(s_datetimeAddCase[testIndex].EndDate),
                    viewModel.StrDateResult,
                    $"Add mode produced the wrong date for case {testIndex}");
            }
        }

        [TestMethod]
        public void DateCalcViewModelSubtractTest()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = false;
            viewModel.IsAddMode = false;

            for (int testIndex = 0; testIndex < s_datetimeSubtractCase.Length; testIndex++)
            {
                viewModel.StartDate = s_datetimeSubtractCase[testIndex].StartDate;
                viewModel.DaysOffset = s_datetimeSubtractCase[testIndex].DateDiff.Day;
                viewModel.MonthsOffset = s_datetimeSubtractCase[testIndex].DateDiff.Month;
                viewModel.YearsOffset = s_datetimeSubtractCase[testIndex].DateDiff.Year;

                Assert.AreEqual(
                    ExpectedDateString(s_datetimeSubtractCase[testIndex].EndDate),
                    viewModel.StrDateResult,
                    $"Subtract mode produced the wrong date for case {testIndex}");
            }
        }

        [TestMethod]
        public void DateCalcViewModelAddOobTest()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = false;
            viewModel.IsAddMode = true;
            Assert.IsFalse(viewModel.IsDateDiffMode);
            Assert.IsTrue(viewModel.IsAddMode);

            for (int testIndex = 0; testIndex < s_datetimeBoundAdd.Length; testIndex++)
            {
                viewModel.StartDate = s_datetimeBoundAdd[testIndex].StartDate;
                viewModel.DaysOffset = s_datetimeBoundAdd[testIndex].DateDiff.Day;
                viewModel.MonthsOffset = s_datetimeBoundAdd[testIndex].DateDiff.Month;
                viewModel.YearsOffset = s_datetimeBoundAdd[testIndex].DateDiff.Year;

                // Assert for the result
                Assert.AreEqual("Date out of Bound", viewModel.StrDateResult);
            }
        }

        // Built through the regional long-date formatter, the same way the view model renders it,
        // rather than hard-coded to one locale.
        private static string ExpectedDateString(DateTimeOffset date)
        {
            return LocalizationSettings.GetInstance()
                .GetRegionalSettingsAwareDateTimeFormatter("longdate")
                .Format(date);
        }

        [TestMethod]
        public void DateCalcViewModelSubtractOobTest()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = false;
            viewModel.IsAddMode = false;
            Assert.IsFalse(viewModel.IsDateDiffMode);
            Assert.IsFalse(viewModel.IsAddMode);

            for (int testIndex = 0; testIndex < s_datetimeBoundSubtract.Length; testIndex++)
            {
                viewModel.StartDate = s_datetimeBoundSubtract[testIndex].StartDate;
                viewModel.DaysOffset = s_datetimeBoundSubtract[testIndex].DateDiff.Day;
                viewModel.MonthsOffset = s_datetimeBoundSubtract[testIndex].DateDiff.Month;
                viewModel.YearsOffset = s_datetimeBoundSubtract[testIndex].DateDiff.Year;

                // Assert for the result
                Assert.AreEqual("Date out of Bound", viewModel.StrDateResult);
            }
        }

        [TestMethod]
        public void DateCalcViewModelDateDiffIgnoreSignTest()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = true;
            Assert.IsTrue(viewModel.IsDateDiffMode);

            viewModel.FromDate = s_date[10]; // 10.05.2007
            viewModel.ToDate = s_date[6];    // 10.03.2008

            Assert.IsFalse(viewModel.IsDiffInDays);
            Assert.AreEqual("305 days", viewModel.StrDateDiffResultInDays);
            Assert.AreEqual("10 months", viewModel.StrDateDiffResult);

            viewModel.FromDate = s_date[6];  // 10.03.2008
            viewModel.ToDate = s_date[10];   // 10.05.2007

            Assert.IsFalse(viewModel.IsDiffInDays);
            Assert.AreEqual("305 days", viewModel.StrDateDiffResultInDays);
            Assert.AreEqual("10 months", viewModel.StrDateDiffResult);
        }

        [TestMethod]
        public void DateCalcViewModelRangeEndingAtMaximumShowsYearsMonthsAndTotalDays()
        {
            var viewModel = new DateCalculatorViewModel();
            viewModel.IsDateDiffMode = true;

            viewModel.FromDate = s_date[13];  // 31.01.2008
            viewModel.ToDate = s_date[0];     // 31.12.9999

            var resources = AppResourceProvider.GetInstance();
            string expectedBreakdown = LocalizeNumber(7991) + " " + resources.GetResourceString("Date_Years")
                + LocalizationSettings.GetInstance().GetListSeparator() + " "
                + LocalizeNumber(11) + " " + resources.GetResourceString("Date_Months");
            string expectedDays = LocalizeNumber(2918987) + " " + resources.GetResourceString("Date_Days");

            Assert.IsFalse(viewModel.IsDiffInDays);
            Assert.AreEqual(expectedBreakdown, viewModel.StrDateDiffResult);
            Assert.AreEqual(expectedDays, viewModel.StrDateDiffResultInDays);
        }

        [TestMethod]
        public void DateCalcViewModelDateDiffResultInPositiveDaysTest()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = true;
            Assert.IsTrue(viewModel.IsDateDiffMode);

            viewModel.FromDate = s_date[1]; // 30.12.9999
            viewModel.ToDate = s_date[0];   // 31.12.9999

            Assert.IsTrue(viewModel.IsDiffInDays);
            Assert.AreEqual("1 day", viewModel.StrDateDiffResult);
            Assert.AreEqual(string.Empty, viewModel.StrDateDiffResultInDays);
        }

        [TestMethod]
        public void DateCalcViewModelDateDiffAutomationNameUsesLocalizedFormat()
        {
            var viewModel = new DateCalculatorViewModel
            {
                IsDateDiffMode = true,
                FromDate = s_date[1],
                ToDate = s_date[0]
            };

            Assert.AreEqual(
                "Difference " + viewModel.StrDateDiffResult,
                viewModel.StrDateDiffResultAutomationName);
        }

        [TestMethod]
        public void DateCalcViewModelDateDiffFromDateHigherThanToDate()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = true;
            Assert.IsTrue(viewModel.IsDateDiffMode);

            viewModel.FromDate = s_date[0]; // 31.12.9999
            viewModel.ToDate = s_date[1];   // 30.12.9999

            Assert.IsTrue(viewModel.IsDiffInDays);
            Assert.AreEqual("1 day", viewModel.StrDateDiffResult);
            Assert.AreEqual(string.Empty, viewModel.StrDateDiffResultInDays);
        }

        [TestMethod]
        public void DateCalcViewModelPreservesCalendarDateForOffsetPickerValues()
        {
            var viewModel = new DateCalculatorViewModel
            {
                IsDateDiffMode = true,
                FromDate = new DateTimeOffset(2024, 3, 1, 0, 0, 0, TimeSpan.FromHours(2)),
                ToDate = new DateTimeOffset(2024, 4, 1, 0, 0, 0, TimeSpan.FromHours(2))
            };

            Assert.IsFalse(viewModel.IsDiffInDays);
            Assert.AreEqual("1 month", viewModel.StrDateDiffResult);
        }

        [TestMethod]
        public void DateCalcViewModelAddSubtractResultAutomationNameTest()
        {
            var viewModel = new DateCalculatorViewModel();
            viewModel.IsDateDiffMode = false;
            viewModel.IsAddMode = true;
            viewModel.StartDate = s_date[13];
            viewModel.DaysOffset = 1;
            viewModel.MonthsOffset = 0;
            viewModel.YearsOffset = 0;

            var automationName = viewModel.StrDateResultAutomationName;

            Assert.AreEqual("Resulting date " + viewModel.StrDateResult, automationName);
        }

        [TestMethod]
        public void JaEraTransitionAddition()
        {
            // Showa ends 1989-01-07 and Heisei begins on the 8th. Date arithmetic must not notice.
            var engine = new DateCalculationEngine("JapaneseCalendar");

            var result = engine.AddDuration(MakeDate(1989, 1, 7), new DateDifference { Day = 1 });

            Assert.IsNotNull(result, "AddDuration returned null across the Showa/Heisei boundary");
            Assert.AreEqual(MakeDate(1989, 1, 8), result.Value);
        }

        [TestMethod]
        public void JaEraTransitionSubtraction()
        {
            var engine = new DateCalculationEngine("JapaneseCalendar");

            var result = engine.SubtractDuration(MakeDate(1989, 1, 8), new DateDifference { Day = 1 });

            Assert.IsNotNull(result, "SubtractDuration returned null across the Showa/Heisei boundary");
            Assert.AreEqual(MakeDate(1989, 1, 7), result.Value);
        }

        [TestMethod]
        public void JaEraTransitionDifference()
        {
            var engine = new DateCalculationEngine("JapaneseCalendar");

            var showaToHeisei = engine.TryGetDateDifference(
                MakeDate(1989, 1, 7), MakeDate(1989, 1, 8), DateUnit.Day);
            Assert.IsNotNull(showaToHeisei, "TryGetDateDifference returned null across Showa/Heisei");
            Assert.AreEqual(1, showaToHeisei.Value.Day);

            // Heisei ends 2019-04-30 and Reiwa begins on the 1st; the same must hold there.
            var heiseiToReiwa = engine.TryGetDateDifference(
                MakeDate(2019, 4, 30), MakeDate(2019, 5, 1), DateUnit.Day);
            Assert.IsNotNull(heiseiToReiwa, "TryGetDateDifference returned null across Heisei/Reiwa");
            Assert.AreEqual(1, heiseiToReiwa.Value.Day);
        }

        [TestMethod]
        public void JapaneseCalendarSubtractionBelowSupportedRangeReturnsNullAndRecovers()
        {
            var engine = new DateCalculationEngine("JapaneseCalendar");

            var outOfRange = engine.SubtractDuration(
                MakeDate(1900, 1, 1),
                new DateDifference { Year = 100 });

            Assert.IsNull(outOfRange);

            Assert.IsNull(engine.SubtractDuration(
                MakeDate(1900, 1, 1),
                new DateDifference { Year = 100 }));
        }

        [TestMethod]
        public void UmAlQuraRangeAtCalendarLimitReturnsYearMonthDayDifference()
        {
            var engine = new DateCalculationEngine("UmAlQuraCalendar");

            var difference = engine.TryGetDateDifference(
                MakeDate(2075, 1, 1),
                MakeDate(2077, 11, 16),
                DateUnit.Year | DateUnit.Month | DateUnit.Day);

            Assert.IsNotNull(difference);
            Assert.AreEqual(2, difference.Value.Year);
            Assert.AreEqual(11, difference.Value.Month);
            Assert.AreEqual(16, difference.Value.Day);
        }

        [TestMethod]
        public void JapaneseDifferenceRecoveryPreservesTheCalendarSystem()
        {
            var engine = new DateCalculationEngine("JapaneseCalendar");

            Assert.IsNotNull(engine.TryGetDateDifference(
                MakeDate(1900, 1, 1),
                MakeDate(9998, 12, 31),
                DateUnit.Year | DateUnit.Month | DateUnit.Day));

            Assert.IsNull(engine.SubtractDuration(
                MakeDate(1900, 1, 1),
                new DateDifference { Year = 100 }));
        }
    }
```

#### 4.3.Z Remaining non-blank source in this file

<!-- DateCalculatorTests.cs:720-720 -->
```csharp
}
```

### 4.4 `src/Calculator.Tests/GraphingViewModelTests.cs`

1 `[TestClass]`(es), 2 test method(s).

| # | Test method | Attributes | Source lines |
|---|---|---|---|
| 1 | `VariableViewModelProxiesTheGraphVariable` | `[TestMethod]` | `GraphingViewModelTests.cs:14-36` |
| 2 | `EquationErrorsUseExistingLocalizedResources` | `[TestMethod]` | `GraphingViewModelTests.cs:38-53` |

#### 4.4.0 File preamble — `using` directives, file-scoped structs/helpers, namespace open (`GraphingViewModelTests.cs:1-10`)

```csharp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;

using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

namespace Calculator.Tests
{
```

#### 4.4.1 `[TestClass] GraphingViewModelTests` — verbatim (`GraphingViewModelTests.cs:11-54`)

```csharp
    [TestClass]
    public class GraphingViewModelTests
    {
        [TestMethod]
        public void VariableViewModelProxiesTheGraphVariable()
        {
            var variable = new GraphControl.Variable(2)
            {
                Min = -5,
                Max = 5,
                Step = 0.25
            };
            var viewModel = new VariableViewModel("a", variable);

            Assert.AreEqual(-5, viewModel.Min);
            Assert.AreEqual(5, viewModel.Max);
            Assert.AreEqual(0.25, viewModel.Step);
            Assert.AreEqual(2, viewModel.Value);

            viewModel.Step = 0.5;
            viewModel.Value = 8;

            Assert.AreEqual(0.5, variable.Step);
            Assert.AreEqual(8, variable.Value);
            Assert.AreEqual(8, variable.Max);
        }

        [TestMethod]
        public void EquationErrorsUseExistingLocalizedResources()
        {
            var resources = AppResourceProvider.GetInstance();

            Assert.AreEqual(
                resources.GetResourceString("Overflow"),
                EquationViewModel.EquationErrorText(
                    GraphControl.ErrorType.Evaluation,
                    (int)GraphControl.EvaluationErrorCode.Overflow));
            Assert.AreEqual(
                resources.GetResourceString("ParenthesisMismatch"),
                EquationViewModel.EquationErrorText(
                    GraphControl.ErrorType.Syntax,
                    (int)GraphControl.SyntaxErrorCode.ParenthesisMismatch));
        }
    }
```

#### 4.4.Z Remaining non-blank source in this file

<!-- GraphingViewModelTests.cs:55-55 -->
```csharp
}
```

### 4.5 `src/Calculator.Tests/HistoryTests.cs`

1 `[TestClass]`(es), 36 test method(s).

| # | Test method | Attributes | Source lines |
|---|---|---|---|
| 1 | `TestHistoryItemClicked` | `[TestMethod]` | `HistoryTests.cs:95-118` |
| 2 | `TestHistoryItemAddSingleItem` | `[TestMethod]` | `HistoryTests.cs:120-136` |
| 3 | `TestHistoryItemAddMaxItems` | `[TestMethod]` | `HistoryTests.cs:138-171` |
| 4 | `TestHistoryClearCommand` | `[TestMethod]` | `HistoryTests.cs:173-190` |
| 5 | `TestHistoryClearCommandWithEmptyHistory` | `[TestMethod]` | `HistoryTests.cs:192-201` |
| 6 | `TestReLoadHistory` | `[TestMethod]` | `HistoryTests.cs:203-257` |
| 7 | `TestHistoryItemWithPrettyExpressions` | `[TestMethod]` | `HistoryTests.cs:259-277` |
| 8 | `TestHistoryItemWithPrettyExpressionsAcrossAngleModes` | `[TestMethod]` | `HistoryTests.cs:279-305` |
| 9 | `TestHistoryItemLoadAndContinueCalculation` | `[TestMethod]` | `HistoryTests.cs:307-334` |
| 10 | `TestDisplayValueAutomationNames` | `[TestMethod]` | `HistoryTests.cs:336-361` |
| 11 | `TestRadixAutomationName` | `[TestMethod]` | `HistoryTests.cs:363-377` |
| 12 | `TestHistoryEmpty` | `[TestMethod]` | `HistoryTests.cs:379-387` |
| 13 | `TestHistoryStandardOrderOfOperations_1` | `[TestMethod]` | `HistoryTests.cs:389-394` |
| 14 | `TestHistoryStandardOrderOfOperations_2` | `[TestMethod]` | `HistoryTests.cs:396-401` |
| 15 | `TestHistoryStandardOrderOfOperations_3` | `[TestMethod]` | `HistoryTests.cs:403-408` |
| 16 | `TestHistoryStandardOrderOfOperations_4` | `[TestMethod]` | `HistoryTests.cs:410-415` |
| 17 | `TestHistoryStandardOrderOfOperations_5` | `[TestMethod]` | `HistoryTests.cs:417-422` |
| 18 | `TestHistoryStandardOrderOfOperations_6` | `[TestMethod]` | `HistoryTests.cs:424-429` |
| 19 | `TestHistoryStandardOrderOfOperations_7` | `[TestMethod]` | `HistoryTests.cs:431-436` |
| 20 | `TestHistoryStandardOrderOfOperations_8` | `[TestMethod]` | `HistoryTests.cs:438-443` |
| 21 | `TestHistoryStandardOrderOfOperations_9` | `[TestMethod]` | `HistoryTests.cs:445-450` |
| 22 | `TestHistoryStandardOrderOfOperations_10` | `[TestMethod]` | `HistoryTests.cs:452-457` |
| 23 | `TestHistoryStandardOrderOfOperations_11` | `[TestMethod]` | `HistoryTests.cs:459-464` |
| 24 | `TestHistoryStandardOrderOfOperations_12` | `[TestMethod]` | `HistoryTests.cs:466-471` |
| 25 | `TestHistoryStandardOrderOfOperations_13` | `[TestMethod]` | `HistoryTests.cs:473-478` |
| 26 | `TestHistoryStandardOrderOfOperations_14` | `[TestMethod]` | `HistoryTests.cs:480-485` |
| 27 | `TestHistoryStandardOrderOfOperations_15` | `[TestMethod]` | `HistoryTests.cs:487-492` |
| 28 | `TestHistoryStandardOrderOfOperations_16` | `[TestMethod]` | `HistoryTests.cs:494-499` |
| 29 | `TestHistoryStandardOrderOfOperations_17` | `[TestMethod]` | `HistoryTests.cs:501-506` |
| 30 | `TestHistoryStandardOrderOfOperations_18` | `[TestMethod]` | `HistoryTests.cs:508-513` |
| 31 | `TestHistoryStandardOrderOfOperations_19` | `[TestMethod]` | `HistoryTests.cs:515-520` |
| 32 | `TestHistoryStandardOrderOfOperations_20` | `[TestMethod]` | `HistoryTests.cs:522-527` |
| 33 | `TestHistoryStandardOrderOfOperations_21` | `[TestMethod]` | `HistoryTests.cs:529-534` |
| 34 | `TestHistoryStandardOrderOfOperations_22` | `[TestMethod]` | `HistoryTests.cs:536-541` |
| 35 | `TestHistoryStandardOrderOfOperations_23` | `[TestMethod]` | `HistoryTests.cs:543-548` |
| 36 | `TestHistoryStandardOrderOfOperationsMultiple` | `[TestMethod]` | `HistoryTests.cs:550-577` |

#### 4.5.0 File preamble — `using` directives, file-scoped structs/helpers, namespace open (`HistoryTests.cs:1-9`)

```csharp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

namespace Calculator.Tests
{
```

#### 4.5.1 `[TestClass] HistoryTests` — verbatim (`HistoryTests.cs:10-578`)

```csharp
    [TestClass]
    public class HistoryTests
    {
        // CalcManager command IDs used by the interop boundary.
        private const int CommandNULL = 0;
        private const int CommandSIGN = 80;
        private const int CommandCLEAR = 81;
        private const int CommandCENTR = 82;
        private const int CommandBACK = 83;
        private const int CommandDIV = 91;
        private const int CommandMUL = 92;
        private const int CommandADD = 93;
        private const int CommandSUB = 94;
        private const int CommandSIN = 102;
        private const int CommandCOS = 103;
        private const int CommandTAN = 104;
        private const int CommandSQRT = 110;
        private const int CommandSQR = 111;
        private const int CommandREC = 114;
        private const int CommandPERCENT = 118;
        private const int CommandEQU = 121;
        private const int Command0 = 130;
        private const int Command1 = 131;
        private const int Command2 = 132;
        private const int Command3 = 133;
        private const int Command4 = 134;
        private const int Command5 = 135;
        private const int Command6 = 136;
        private const int Command7 = 137;
        private const int Command8 = 138;
        private const int Command9 = 139;
        private const int ModeBasic = 200;
        private const int ModeScientific = 201;
        private const int CommandASIN = 202;
        private const int CommandACOS = 203;
        private const int CommandATAN = 204;
        private const int ModeProgrammer = 209;
        private const int CommandDEG = 321;
        private const int CommandRAD = 322;
        private const int CommandGRAD = 323;

        private StandardCalculatorViewModel m_standardViewModel;
        private HistoryViewModel m_historyViewModel;

        private void Initialize()
        {
            m_standardViewModel = new StandardCalculatorViewModel();
            m_standardViewModel.IsStandard = true;
            m_historyViewModel = m_standardViewModel.HistoryVM;
        }

        private void Cleanup()
        {
            m_standardViewModel.SendCommandToCalcManager(ModeBasic);
            m_historyViewModel.ClearCommand.Execute(null);
            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            m_historyViewModel.ClearCommand.Execute(null);
            m_standardViewModel.ResetCalcManager(false);
        }

        private void HistoryStandardOrderOfOperationsHelper(string expectedResult, string expectedExpression, int[] testCommands)
        {
            Initialize();
            int initialSize = m_historyViewModel.ItemsCount;
            foreach (int command in testCommands)
            {
                if (command == CommandNULL)
                    break;
                m_standardViewModel.SendCommandToCalcManager(command);
            }
            int sizeAfterCommandsAdd = m_historyViewModel.ItemsCount;
            if (string.IsNullOrEmpty(expectedResult))
            {
                Assert.AreEqual(initialSize, sizeAfterCommandsAdd);
            }
            else
            {
                var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[0];
                Assert.AreEqual(initialSize + 1, sizeAfterCommandsAdd);
                Assert.AreEqual(expectedExpression, historyItem.Expression);
                Assert.AreEqual(expectedResult, historyItem.Result);
            }
            Cleanup();
        }

        [TestMethod]
        public void TestHistoryItemClicked()
        {
            Initialize();
            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command5);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command3);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1];
            m_standardViewModel.SelectHistoryItem(historyItem);
            Assert.AreEqual("9", m_standardViewModel.DisplayValue);
            Assert.AreEqual("1", m_standardViewModel.ExpressionTokens[0].Token);
            Assert.AreEqual(" ", m_standardViewModel.ExpressionTokens[1].Token);
            Assert.AreEqual("+", m_standardViewModel.ExpressionTokens[2].Token);
            Assert.AreEqual(" ", m_standardViewModel.ExpressionTokens[3].Token);
            Assert.AreEqual("5", m_standardViewModel.ExpressionTokens[4].Token);
            Assert.AreEqual(" ", m_standardViewModel.ExpressionTokens[5].Token);
            Assert.AreEqual("+", m_standardViewModel.ExpressionTokens[6].Token);
            Assert.AreEqual(" ", m_standardViewModel.ExpressionTokens[7].Token);
            Cleanup();
        }

        [TestMethod]
        public void TestHistoryItemAddSingleItem()
        {
            Initialize();
            int initialSize = m_historyViewModel.ItemsCount;
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command8);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            int sizeAfterItemAdd = m_historyViewModel.ItemsCount;
            var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[0];
            string expression = "1   +   8 =";
            Assert.AreEqual(initialSize + 1, sizeAfterItemAdd);
            Assert.AreEqual(expression, historyItem.Expression);
            Assert.AreEqual("9", historyItem.Result);
            Cleanup();
        }

        [TestMethod]
        public void TestHistoryItemAddMaxItems()
        {
            Initialize();
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            for (ulong i = 1; i < m_historyViewModel.GetMaxItemSize(); i++)
            {
                m_standardViewModel.SendCommandToCalcManager(Command1);
                m_standardViewModel.SendCommandToCalcManager(CommandADD);
                m_standardViewModel.SendCommandToCalcManager(Command2);
                m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            }
            Assert.AreEqual((int)m_historyViewModel.GetMaxItemSize(), m_historyViewModel.ItemsCount);
            string expression = "1   +   1 =";
            int output = 2;
            string result = output.ToString();
            var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1];
            Assert.AreEqual(expression, historyItem.Expression);
            Assert.AreEqual(result, historyItem.Result);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command5);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            expression = "1   +   2 =";
            output = 3;
            result = output.ToString();
            historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1];
            Assert.AreEqual(expression, historyItem.Expression);
            Assert.AreEqual(result, historyItem.Result);
            Cleanup();
        }

        [TestMethod]
        public void TestHistoryClearCommand()
        {
            Initialize();
            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command2);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            m_standardViewModel.SendCommandToCalcManager(ModeBasic);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command2);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            m_historyViewModel.ClearCommand.Execute(null);
            Assert.AreEqual(0, m_historyViewModel.ItemsCount);
            Cleanup();
        }

        [TestMethod]
        public void TestHistoryClearCommandWithEmptyHistory()
        {
            Initialize();
            Assert.AreEqual(0, m_historyViewModel.ItemsCount);
            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            m_historyViewModel.ClearCommand.Execute(null);
            Assert.AreEqual(0, m_historyViewModel.ItemsCount);
            Cleanup();
        }

        [TestMethod]
        public void TestReLoadHistory()
        {
            Initialize();

            m_standardViewModel.ResetCalcManager(false);
            int scientificItems = 5;
            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            for (int i = 0; i < scientificItems; i++)
            {
                int nextCommand = 130 + i;
                m_standardViewModel.SendCommandToCalcManager(Command1);
                m_standardViewModel.SendCommandToCalcManager(CommandADD);
                m_standardViewModel.SendCommandToCalcManager(nextCommand);
                m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            }

            m_standardViewModel.SendCommandToCalcManager(ModeBasic);
            int standardItems = 2;
            for (int i = 0; i < standardItems; i++)
            {
                int nextCommand = 130 + i;
                m_standardViewModel.SendCommandToCalcManager(Command1);
                m_standardViewModel.SendCommandToCalcManager(CommandADD);
                m_standardViewModel.SendCommandToCalcManager(nextCommand);
                m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            }

            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            m_historyViewModel.ReloadHistory(ViewMode.Scientific);
            Assert.AreEqual(scientificItems, m_historyViewModel.ItemsCount);
            for (int i = 0; i < scientificItems; i++)
            {
                string expr = "1   +   " + i.ToString() + " =";
                int output = 1 + i;
                string result = output.ToString();
                var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1 - i];
                Assert.AreEqual(expr, historyItem.Expression);
                Assert.AreEqual(result, historyItem.Result);
            }

            m_historyViewModel.ReloadHistory(ViewMode.Standard);
            m_standardViewModel.SendCommandToCalcManager(ModeBasic);
            Assert.AreEqual(standardItems, m_historyViewModel.ItemsCount);
            for (int i = 0; i < standardItems; i++)
            {
                string expr = "1   +   " + i.ToString() + " =";
                int output = 1 + i;
                string result = output.ToString();
                var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1 - i];
                Assert.AreEqual(expr, historyItem.Expression);
                Assert.AreEqual(result, historyItem.Result);
            }
            Cleanup();
        }

        [TestMethod]
        public void TestHistoryItemWithPrettyExpressions()
        {
            Initialize();
            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            m_standardViewModel.SendCommandToCalcManager(Command2);
            m_standardViewModel.SendCommandToCalcManager(CommandSQRT);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);

            var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1];

            // The stored expression is the readable form, not the raw command name.
            Assert.IsFalse(string.IsNullOrEmpty(historyItem.Expression));
            Assert.IsFalse(
                historyItem.Expression.Contains("CommandSQRT"),
                $"The expression shows a command name rather than a symbol: '{historyItem.Expression}'.");
            Assert.AreEqual("1.4142135623730950488016887242097", historyItem.Result);
            Cleanup();
        }

        [TestMethod]
        public void TestHistoryItemWithPrettyExpressionsAcrossAngleModes()
        {
            Initialize();
            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            m_standardViewModel.SendCommandToCalcManager(CommandDEG);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandSIN);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(CommandRAD);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandSIN);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(CommandGRAD);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandSIN);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);

            var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1];
            var resources = AppResourceProvider.GetInstance();
            string expected = resources.GetCEngineString("67") + "( 1 )   +   "
                + resources.GetCEngineString("73") + "( 1 )   +   "
                + resources.GetCEngineString("79") + "( 1 ) =";

            Assert.AreEqual(expected, historyItem.Expression);
            Cleanup();
        }

        [TestMethod]
        public void TestHistoryItemLoadAndContinueCalculation()
        {
            Initialize();
            m_standardViewModel.SendCommandToCalcManager(ModeBasic);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command5);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command3);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);

            var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1];
            m_standardViewModel.SelectHistoryItem(historyItem);

            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command5);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            Assert.AreEqual("11", m_standardViewModel.DisplayValue);
            historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1];
            m_standardViewModel.SelectHistoryItem(historyItem);
            Assert.AreEqual("6", m_standardViewModel.DisplayValue);

            historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 2];
            m_standardViewModel.SelectHistoryItem(historyItem);
            Assert.AreEqual("9", m_standardViewModel.DisplayValue);
            Cleanup();
        }

        [TestMethod]
        public void TestDisplayValueAutomationNames()
        {
            Initialize();
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command8);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            Assert.AreEqual("Display is 9", m_standardViewModel.CalculationResultAutomationName);

            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command5);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            Assert.AreEqual("Display is 6", m_standardViewModel.CalculationResultAutomationName);

            m_standardViewModel.SendCommandToCalcManager(ModeProgrammer);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command2);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            Assert.AreEqual("Display is 3", m_standardViewModel.CalculationResultAutomationName);

            Cleanup();
        }

        [TestMethod]
        public void TestRadixAutomationName()
        {
            Initialize();
            m_standardViewModel.SendCommandToCalcManager(ModeProgrammer);
            m_standardViewModel.IsProgrammer = true;
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command7);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            Assert.AreEqual("HexaDecimal 8", "HexaDecimal " + m_standardViewModel.HexDisplayValue);
            Assert.AreEqual("Octal 10", "Octal " + m_standardViewModel.OctalDisplayValue);
            Assert.AreEqual("Binary 1000", "Binary " + m_standardViewModel.BinaryDisplayValue);
            Cleanup();
        }

        [TestMethod]
        public void TestHistoryEmpty()
        {
            Initialize();
            Assert.AreEqual(0, m_historyViewModel.ItemsCount);
            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            Assert.AreEqual(0, m_historyViewModel.ItemsCount);
            Cleanup();
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_1()
        {
            int[] commands = { Command1, CommandREC, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_2()
        {
            int[] commands = { Command4, CommandSQRT, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_3()
        {
            int[] commands = { Command1, CommandADD, Command4, CommandSQRT, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_4()
        {
            int[] commands = { Command1, CommandADD, Command4, CommandSQRT, CommandSUB, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("3", "1   +   \x221A( 4 ) =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_5()
        {
            int[] commands = { Command2, CommandMUL, Command4, CommandREC, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_6()
        {
            int[] commands = { Command5, CommandDIV, Command6, CommandPERCENT, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_7()
        {
            int[] commands = { Command4, CommandSQRT, CommandSUB, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_8()
        {
            int[] commands = { Command7, CommandSQR, CommandDIV, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_9()
        {
            int[] commands = { Command8, CommandSQR, CommandSQRT, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_10()
        {
            int[] commands = { Command1, Command0, CommandADD, Command2, CommandSUB, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("12", "10   +   2 =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_11()
        {
            int[] commands = { Command3, CommandMUL, Command4, CommandDIV, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("12", "3   \x00D7   4 =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_12()
        {
            int[] commands = { Command6, CommandDIV, Command3, CommandSUB, CommandADD, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("2", "6   \x00F7   3 =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_13()
        {
            int[] commands = { Command7, CommandSUB, Command4, CommandDIV, CommandMUL, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("3", "7   -   4 =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_14()
        {
            int[] commands = { Command8, CommandMUL, Command2, CommandADD, CommandSQRT, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("16", "8   \x00D7   2 =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_15()
        {
            int[] commands = { Command9, CommandADD, Command0, CommandMUL, CommandSIGN, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("9", "9   +   0 =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_16()
        {
            int[] commands = { Command9, CommandSIGN, Command0, CommandADD, CommandMUL, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_17()
        {
            int[] commands = { Command1, CommandADD, Command2, CommandEQU, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("3", "1   +   2 =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_18()
        {
            int[] commands = { Command2, Command0, CommandMUL, Command0, Command2, CommandEQU, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("40", "20   \x00D7   2 =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_19()
        {
            int[] commands = { Command1, CommandADD, Command2, CommandADD, CommandBACK, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("3", "1   +   2 =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_20()
        {
            int[] commands = { Command1, CommandADD, Command2, CommandADD, CommandCLEAR, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("3", "1   +   2 =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_21()
        {
            int[] commands = { Command1, CommandADD, Command2, CommandADD, CommandCENTR, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("3", "1   +   2 =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_22()
        {
            int[] commands = { Command1, CommandADD, Command2, CommandCLEAR, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_23()
        {
            int[] commands = { Command1, CommandADD, Command2, CommandCENTR, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperationsMultiple()
        {
            Initialize();
            int[] commands = { Command1, CommandMUL, Command2, CommandMUL, Command3, CommandMUL, Command4, CommandMUL, Command5, CommandMUL, CommandNULL };
            int initialSize = m_historyViewModel.ItemsCount;
            foreach (int command in commands)
            {
                if (command == CommandNULL)
                    break;
                m_standardViewModel.SendCommandToCalcManager(command);
            }
            int sizeAfterCommandsAdd = m_historyViewModel.ItemsCount;
            Assert.AreEqual(initialSize + 4, sizeAfterCommandsAdd);
            var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[0];
            Assert.AreEqual("24   \x00D7   5 =", historyItem.Expression);
            Assert.AreEqual("120", historyItem.Result);
            historyItem = (HistoryItemViewModel)m_historyViewModel.Items[1];
            Assert.AreEqual("6   \x00D7   4 =", historyItem.Expression);
            Assert.AreEqual("24", historyItem.Result);
            historyItem = (HistoryItemViewModel)m_historyViewModel.Items[2];
            Assert.AreEqual("2   \x00D7   3 =", historyItem.Expression);
            Assert.AreEqual("6", historyItem.Result);
            historyItem = (HistoryItemViewModel)m_historyViewModel.Items[3];
            Assert.AreEqual("1   \x00D7   2 =", historyItem.Expression);
            Assert.AreEqual("2", historyItem.Result);
            Cleanup();
        }
    }
```

#### 4.5.Z Remaining non-blank source in this file

<!-- HistoryTests.cs:579-579 -->
```csharp
}
```

### 4.6 `src/Calculator.Tests/KeystrokeLatencyTests.cs`

1 `[TestClass]`(es), 1 test method(s).

| # | Test method | Attributes | Source lines |
|---|---|---|---|
| 1 | `TypingIntoTheConverterStaysWellInsideAFrame` | `[TestMethod]` | `KeystrokeLatencyTests.cs:25-60` |

#### 4.6.0 File preamble — `using` directives, file-scoped structs/helpers, namespace open (`KeystrokeLatencyTests.cs:1-12`)

```csharp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Diagnostics;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

namespace Calculator.Tests
{
    // The budget sits an order of magnitude above the measured 0.16 ms cost on x64 Debug.
    // It catches a change of order, not small movements that would make the suite flaky.
```

#### 4.6.1 `[TestClass] KeystrokeLatencyTests` — verbatim (`KeystrokeLatencyTests.cs:13-61`)

```csharp
    [TestClass]
    public class KeystrokeLatencyTests
    {
        // A frame is about 16 ms, so anything in this range is imperceptible; the budget is set an
        // order of magnitude above what the code costs today.
        private const double KeystrokeBudgetMs = 5.0;

        // Enough repetitions to average out scheduling noise, and no more. Driving the view models
        // harder than a person could type destabilizes the UWP test host, which shows up as the run
        // aborting with "Unable to communicate with test host process" rather than as a failure.
        private const int KeystrokeRounds = 5;

        [TestMethod]
        public void TypingIntoTheConverterStaysWellInsideAFrame()
        {
            var viewModel = new UnitConverterViewModel();
            var digits = new[]
            {
                NumbersAndOperatorsEnum.One, NumbersAndOperatorsEnum.Two, NumbersAndOperatorsEnum.Three,
                NumbersAndOperatorsEnum.Four, NumbersAndOperatorsEnum.Five, NumbersAndOperatorsEnum.Six,
                NumbersAndOperatorsEnum.Seven,
            };

            for (int warmup = 0; warmup < 1; warmup++)
            {
                viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Clear);
                foreach (var digit in digits)
                {
                    viewModel.ButtonPressedCommand.Execute(digit);
                }
            }

            var stopwatch = Stopwatch.StartNew();
            for (int round = 0; round < KeystrokeRounds; round++)
            {
                viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Clear);
                foreach (var digit in digits)
                {
                    viewModel.ButtonPressedCommand.Execute(digit);
                }
            }
            stopwatch.Stop();

            double perKeystroke = stopwatch.Elapsed.TotalMilliseconds / (KeystrokeRounds * (digits.Length + 1));
            Assert.IsTrue(
                perKeystroke < KeystrokeBudgetMs,
                $"A converter keystroke took {perKeystroke:F3} ms, over the {KeystrokeBudgetMs} ms budget.");
        }
    }
```

#### 4.6.Z Remaining non-blank source in this file

<!-- KeystrokeLatencyTests.cs:62-62 -->
```csharp
}
```

### 4.7 `src/Calculator.Tests/LocalizationSettingsTests.cs`

1 `[TestClass]`(es), 8 test method(s).

| # | Test method | Attributes | Source lines |
|---|---|---|---|
| 1 | `TestLocaleName` | `[TestMethod]` | `LocalizationSettingsTests.cs:20-26` |
| 2 | `TestIsDigitEnUsSetting` | `[TestMethod]` | `LocalizationSettingsTests.cs:28-38` |
| 3 | `TestLocalizeDisplayValue` | `[TestMethod]` | `LocalizationSettingsTests.cs:40-48` |
| 4 | `TestGetEnglishValueFromLocalizedDigits` | `[TestMethod]` | `LocalizationSettingsTests.cs:50-56` |
| 5 | `TestIsEnUsDigit` | `[TestMethod]` | `LocalizationSettingsTests.cs:58-68` |
| 6 | `TestIsLocalizedDigit` | `[TestMethod]` | `LocalizationSettingsTests.cs:70-77` |
| 7 | `TestIsLocalizedHexDigit` | `[TestMethod]` | `LocalizationSettingsTests.cs:79-87` |
| 8 | `TestRemoveGroupSeparators` | `[TestMethod]` | `LocalizationSettingsTests.cs:89-95` |

#### 4.7.0 File preamble — `using` directives, file-scoped structs/helpers, namespace open (`LocalizationSettingsTests.cs:1-10`)

```csharp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel.Common;
using Windows.Globalization.NumberFormatting;
using System.Collections.Generic;

namespace Calculator.Tests
{
```

#### 4.7.1 `[TestClass] LocalizationSettingsTests` — verbatim (`LocalizationSettingsTests.cs:11-96`)

```csharp
    [TestClass]
    public class LocalizationSettingsTests
    {
        private static DecimalFormatter CreateDecimalFormatter(string language, string geographicRegion)
        {
            var languages = new List<string> { language };
            return new DecimalFormatter(languages, geographicRegion);
        }

        [TestMethod]
        public void TestLocaleName()
        {
            var formatter = CreateDecimalFormatter("en-US", "US");
            var settings = new LocalizationSettings(formatter);
            Assert.AreEqual("en-US", settings.GetLocaleName());
        }

        [TestMethod]
        public void TestIsDigitEnUsSetting()
        {
            var arabicFormatter = CreateDecimalFormatter("ar-AE", "AE");
            var arabicSettings = new LocalizationSettings(arabicFormatter);
            Assert.IsFalse(arabicSettings.IsDigitEnUsSetting());

            var englishFormatter = CreateDecimalFormatter("en-US", "US");
            var englishSettings = new LocalizationSettings(englishFormatter);
            Assert.IsTrue(englishSettings.IsDigitEnUsSetting());
        }

        [TestMethod]
        public void TestLocalizeDisplayValue()
        {
            var formatter = CreateDecimalFormatter("ar-AE", "AE");
            var settings = new LocalizationSettings(formatter);
            string input = "A123";
            settings.LocalizeDisplayValue(ref input);
            Assert.AreEqual("A\u0661\u0662\u0663", input);
        }

        [TestMethod]
        public void TestGetEnglishValueFromLocalizedDigits()
        {
            var formatter = CreateDecimalFormatter("ar-AE", "AE");
            var settings = new LocalizationSettings(formatter);
            Assert.AreEqual("A123", settings.GetEnglishValueFromLocalizedDigits("A\u0661\u0662\u0663"));
        }

        [TestMethod]
        public void TestIsEnUsDigit()
        {
            var settings = LocalizationSettings.GetInstance();
            Assert.IsFalse(settings.IsEnUsDigit('/'));
            Assert.IsTrue(settings.IsEnUsDigit('0'));
            Assert.IsTrue(settings.IsEnUsDigit('1'));
            Assert.IsTrue(settings.IsEnUsDigit('8'));
            Assert.IsTrue(settings.IsEnUsDigit('9'));
            Assert.IsFalse(settings.IsEnUsDigit(':'));
        }

        [TestMethod]
        public void TestIsLocalizedDigit()
        {
            var formatter = CreateDecimalFormatter("en-US", "US");
            var settings = new LocalizationSettings(formatter);
            Assert.IsTrue(settings.IsLocalizedDigit('0'));
            Assert.IsFalse(settings.IsLocalizedDigit('A'));
        }

        [TestMethod]
        public void TestIsLocalizedHexDigit()
        {
            var formatter = CreateDecimalFormatter("en-US", "US");
            var settings = new LocalizationSettings(formatter);
            Assert.IsTrue(settings.IsLocalizedHexDigit('0'));
            Assert.IsTrue(settings.IsLocalizedHexDigit('A'));
            Assert.IsFalse(settings.IsLocalizedHexDigit('G'));
        }

        [TestMethod]
        public void TestRemoveGroupSeparators()
        {
            var formatter = CreateDecimalFormatter("en-US", "US");
            var settings = new LocalizationSettings(formatter);
            Assert.AreEqual("1000000", settings.RemoveGroupSeparators("1,000 000"));
        }
    }
```

#### 4.7.Z Remaining non-blank source in this file

<!-- LocalizationSettingsTests.cs:97-97 -->
```csharp
}
```

### 4.8 `src/Calculator.Tests/MultiWindowTests.cs`

1 `[TestClass]`(es), 16 test method(s).

| # | Test method | Attributes | Source lines |
|---|---|---|---|
| 1 | `InitializeMultipleInstancesTest` | `[TestMethod]` | `MultiWindowTests.cs:111-126` |
| 2 | `InitializeMultipleModeInstancesTest` | `[TestMethod]` | `MultiWindowTests.cs:128-144` |
| 3 | `MultipleModesCalculationTest` | `[TestMethod]` | `MultiWindowTests.cs:146-198` |
| 4 | `MultipleStandardModeCalculationTest` | `[TestMethod]` | `MultiWindowTests.cs:200-235` |
| 5 | `MultipleScientificModeCalculationTest` | `[TestMethod]` | `MultiWindowTests.cs:237-272` |
| 6 | `MultipleScientificModeWithDifferentSettingsTest` | `[TestMethod]` | `MultiWindowTests.cs:274-321` |
| 7 | `MultipleProgrammerModeCalculationTest` | `[TestMethod]` | `MultiWindowTests.cs:323-367` |
| 8 | `MultipleProgrammerModeWithDifferentSettingsTest` | `[TestMethod]` | `MultiWindowTests.cs:369-431` |
| 9 | `MultipleModesHistoryAddItemTest` | `[TestMethod]` | `MultiWindowTests.cs:433-481` |
| 10 | `MultipleStandardModesHistoryAddItemTest` | `[TestMethod]` | `MultiWindowTests.cs:483-544` |
| 11 | `MultipleScientificModesHistoryAddItemTest` | `[TestMethod]` | `MultiWindowTests.cs:546-594` |
| 12 | `MultipleModesMemoryAddItemTest` | `[TestMethod]` | `MultiWindowTests.cs:596-656` |
| 13 | `DateCalculatorInstancesKeepIndependentState` | `[TestMethod]` | `MultiWindowTests.cs:658-680` |
| 14 | `ConverterInstancesKeepIndependentState` | `[TestMethod]` | `MultiWindowTests.cs:682-718` |
| 15 | `CalculatorModeViewModelsKeepIndependentState` | `[TestMethod]` | `MultiWindowTests.cs:720-747` |
| 16 | `MultipleModesWithChangeInProgrammerRadix` | `[TestMethod]` | `MultiWindowTests.cs:749-811` |

#### 4.8.0 File preamble — `using` directives, file-scoped structs/helpers, namespace open (`MultiWindowTests.cs:1-11`)

```csharp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

namespace Calculator.Tests
{
```

#### 4.8.1 `[TestClass] MultiWindowUnitTests` — verbatim (`MultiWindowTests.cs:12-812`)

```csharp
    [TestClass]
    public class MultiWindowUnitTests
    {
        #region Helper Methods

        private static void ChangeMode(StandardCalculatorViewModel viewModel, int mode)
        {
            if (mode == 0)
            {
                viewModel.IsStandard = true;
                viewModel.IsScientific = false;
                viewModel.IsProgrammer = false;
                viewModel.SetCalculatorType(ViewMode.Standard);
            }
            else if (mode == 1)
            {
                viewModel.IsScientific = true;
                viewModel.IsProgrammer = false;
                viewModel.IsStandard = false;
                viewModel.SetCalculatorType(ViewMode.Scientific);
            }
            else if (mode == 2)
            {
                viewModel.IsProgrammer = true;
                viewModel.IsScientific = false;
                viewModel.IsStandard = false;
                viewModel.SetCalculatorType(ViewMode.Programmer);
            }
        }

        private static void ValidateViewModelMode(StandardCalculatorViewModel viewModel, int mode)
        {
            if (mode == 0)
            {
                Assert.IsTrue(viewModel.IsStandard);
                Assert.IsFalse(viewModel.IsScientific);
                Assert.IsFalse(viewModel.IsProgrammer);
            }
            else if (mode == 1)
            {
                Assert.IsFalse(viewModel.IsStandard);
                Assert.IsTrue(viewModel.IsScientific);
                Assert.IsFalse(viewModel.IsProgrammer);
            }
            else if (mode == 2)
            {
                Assert.IsFalse(viewModel.IsStandard);
                Assert.IsFalse(viewModel.IsScientific);
                Assert.IsTrue(viewModel.IsProgrammer);
            }
        }

        private static object CommandParameterFromTestItem(TestItem item)
        {
            var binOps = new (NumbersAndOperatorsEnum op, string feedback)[]
            {
                (NumbersAndOperatorsEnum.Add, "plus"),
                (NumbersAndOperatorsEnum.Subtract, "minus"),
                (NumbersAndOperatorsEnum.Multiply, "times"),
                (NumbersAndOperatorsEnum.Divide, "divided by")
            };

            foreach (var binOp in binOps)
            {
                if (item.Command == binOp.op)
                {
                    return new CalculatorButtonPressedEventArgs(binOp.feedback, binOp.op);
                }
            }

            return item.Command;
        }

        private static void ValidateViewModelByCommands(StandardCalculatorViewModel viewModel, TestItem[] items, bool doReset = false)
        {
            if (doReset)
            {
                viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Clear);
                viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.ClearEntry);
                viewModel.ClearMemoryCommand.Execute(null);
            }

            foreach (var item in items)
            {
                if (item.Command == NumbersAndOperatorsEnum.None)
                    break;

                object commandParam = CommandParameterFromTestItem(item);
                viewModel.ButtonPressed.Execute(commandParam);

                if (item.ExpectedPrimaryDisplay != "N/A")
                {
                    Assert.AreEqual(item.ExpectedPrimaryDisplay, viewModel.DisplayValue);
                }
            }
        }

        #endregion

        [TestMethod]
        public void InitializeMultipleInstancesTest()
        {
            var viewModels = new StandardCalculatorViewModel[3];

            for (int i = 0; i < 3; i++)
            {
                viewModels[i] = new StandardCalculatorViewModel();
                viewModels[i].IsStandard = true;
            }

            for (int i = 0; i < 3; i++)
            {
                Assert.AreEqual("0", viewModels[i].DisplayValue);
            }
        }

        [TestMethod]
        public void InitializeMultipleModeInstancesTest()
        {
            var viewModels = new StandardCalculatorViewModel[3];

            for (int i = 0; i < 3; i++)
            {
                viewModels[i] = new StandardCalculatorViewModel();
                ChangeMode(viewModels[i], i);
            }

            for (int i = 0; i < 3; i++)
            {
                ValidateViewModelMode(viewModels[i], i);
                Assert.AreEqual("0", viewModels[i].DisplayValue);
            }
        }

        [TestMethod]
        public void MultipleModesCalculationTest()
        {
            var viewModels = new StandardCalculatorViewModel[3];

            for (int i = 0; i < 3; i++)
            {
                viewModels[i] = new StandardCalculatorViewModel();
                ChangeMode(viewModels[i], i);
            }

            // Standard Mode: 1+2=3
            var standardItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Equals, "3", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[0], standardItems, true);

            // Scientific Mode: 1+2*3=7
            var scientificItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Multiply, "2", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Equals, "7", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[1], scientificItems, true);

            // Programmer Mode: Hex F = 15
            var programmerItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[2], programmerItems, false);
            Assert.AreEqual("F", viewModels[2].HexDisplayValue);
            Assert.AreEqual("15", viewModels[2].DecimalDisplayValue);
            Assert.AreEqual("17", viewModels[2].OctalDisplayValue);
            Assert.AreEqual("1111", viewModels[2].BinaryDisplayValue);

            // Assert Standard and Scientific display values unchanged
            Assert.AreEqual("3", viewModels[0].DisplayValue);
            Assert.AreEqual("7", viewModels[1].DisplayValue);
        }

        [TestMethod]
        public void MultipleStandardModeCalculationTest()
        {
            var standardViewModel1 = new StandardCalculatorViewModel();
            var standardViewModel2 = new StandardCalculatorViewModel();

            ChangeMode(standardViewModel1, 0);
            ChangeMode(standardViewModel2, 0);

            ValidateViewModelMode(standardViewModel1, 0);
            ValidateViewModelMode(standardViewModel2, 0);

            // Standard 1: 3-2=1
            var items1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Three, "3", ""),
                new(NumbersAndOperatorsEnum.Subtract, "3", "3 - "),
                new(NumbersAndOperatorsEnum.Two, "2", "3 - "),
                new(NumbersAndOperatorsEnum.Equals, "1", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(standardViewModel1, items1, true);

            // Standard 2: 4*5=20
            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Four, "4", ""),
                new(NumbersAndOperatorsEnum.Multiply, "4", "4 * "),
                new(NumbersAndOperatorsEnum.Five, "5", "4 * "),
                new(NumbersAndOperatorsEnum.Equals, "20", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(standardViewModel2, items2, true);

            Assert.AreEqual("1", standardViewModel1.DisplayValue);
        }

        [TestMethod]
        public void MultipleScientificModeCalculationTest()
        {
            var scientificViewModel1 = new StandardCalculatorViewModel();
            var scientificViewModel2 = new StandardCalculatorViewModel();

            ChangeMode(scientificViewModel1, 1);
            ChangeMode(scientificViewModel2, 1);

            ValidateViewModelMode(scientificViewModel1, 1);
            ValidateViewModelMode(scientificViewModel2, 1);

            // Scientific 1: 3-2=1
            var items1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Three, "3", ""),
                new(NumbersAndOperatorsEnum.Subtract, "3", "3 - "),
                new(NumbersAndOperatorsEnum.Two, "2", "3 - "),
                new(NumbersAndOperatorsEnum.Equals, "1", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(scientificViewModel1, items1, true);

            // Scientific 2: 4*5=20
            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Four, "4", ""),
                new(NumbersAndOperatorsEnum.Multiply, "4", "4 * "),
                new(NumbersAndOperatorsEnum.Five, "5", "4 * "),
                new(NumbersAndOperatorsEnum.Equals, "20", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(scientificViewModel2, items2, true);

            Assert.AreEqual("1", scientificViewModel1.DisplayValue);
        }

        [TestMethod]
        public void MultipleScientificModeWithDifferentSettingsTest()
        {
            var scientificViewModel1 = new StandardCalculatorViewModel();
            var scientificViewModel2 = new StandardCalculatorViewModel();

            ChangeMode(scientificViewModel1, 1);
            ChangeMode(scientificViewModel2, 1);

            ValidateViewModelMode(scientificViewModel1, 1);
            ValidateViewModelMode(scientificViewModel2, 1);

            // Scientific 1: Degrees with HYP checked
            var initItems1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Degree, "0", ""),
                new(NumbersAndOperatorsEnum.Hyp, "0", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(scientificViewModel1, initItems1, true);

            // Scientific 2: Radians with F-E checked
            var initItems2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Radians, "0", ""),
                new(NumbersAndOperatorsEnum.FToE, "0.e+0", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(scientificViewModel2, initItems2, true);

            // Scientific 1: CosH(0 degrees)
            var testItems1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Zero, "0", ""),
                new(NumbersAndOperatorsEnum.Cosh, "1", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(scientificViewModel1, testItems1, true);

            // Scientific 2: Cos(pi radians)
            var testItems2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Pi, "3.1415926535897932384626433832795e+0", ""),
                new(NumbersAndOperatorsEnum.Cos, "-1.e+0", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(scientificViewModel2, testItems2, true);
        }

        [TestMethod]
        public void MultipleProgrammerModeCalculationTest()
        {
            var programmerViewModel1 = new StandardCalculatorViewModel();
            var programmerViewModel2 = new StandardCalculatorViewModel();

            ChangeMode(programmerViewModel1, 2);
            ChangeMode(programmerViewModel2, 2);

            ValidateViewModelMode(programmerViewModel1, 2);
            ValidateViewModelMode(programmerViewModel2, 2);

            // Programmer 1: Hex F
            var items1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(programmerViewModel1, items1, false);
            Assert.AreEqual("F", programmerViewModel1.HexDisplayValue);
            Assert.AreEqual("15", programmerViewModel1.DecimalDisplayValue);
            Assert.AreEqual("17", programmerViewModel1.OctalDisplayValue);
            Assert.AreEqual("1111", programmerViewModel1.BinaryDisplayValue);

            // Programmer 2: Oct 7
            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.OctButton, "0", ""),
                new(NumbersAndOperatorsEnum.Seven, "7", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(programmerViewModel2, items2, false);
            Assert.AreEqual("7", programmerViewModel2.HexDisplayValue);
            Assert.AreEqual("7", programmerViewModel2.DecimalDisplayValue);
            Assert.AreEqual("7", programmerViewModel2.OctalDisplayValue);
            Assert.AreEqual("0111", programmerViewModel2.BinaryDisplayValue);

            // Assert 1st instance unchanged
            Assert.AreEqual("F", programmerViewModel1.DisplayValue);
            Assert.AreEqual("F", programmerViewModel1.HexDisplayValue);
            Assert.AreEqual("15", programmerViewModel1.DecimalDisplayValue);
            Assert.AreEqual("17", programmerViewModel1.OctalDisplayValue);
            Assert.AreEqual("1111", programmerViewModel1.BinaryDisplayValue);
        }

        [TestMethod]
        public void MultipleProgrammerModeWithDifferentSettingsTest()
        {
            var programmerViewModel1 = new StandardCalculatorViewModel();
            var programmerViewModel2 = new StandardCalculatorViewModel();

            ChangeMode(programmerViewModel1, 2);
            ChangeMode(programmerViewModel2, 2);

            ValidateViewModelMode(programmerViewModel1, 2);
            ValidateViewModelMode(programmerViewModel2, 2);

            // Programmer 1: Byte & Hex
            var initItems1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Byte, "0", ""),
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(programmerViewModel1, initItems1, false);

            // Programmer 2: Word & Oct
            var initItems2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Word, "0", ""),
                new(NumbersAndOperatorsEnum.OctButton, "0", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(programmerViewModel2, initItems2, false);

            // Programmer 1: FF (byte max)
            var testItems1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.F, "FF", ""),
                // One more F shouldn't have any effect, testing for precision
                new(NumbersAndOperatorsEnum.F, "FF", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(programmerViewModel1, testItems1, false);
            Assert.AreEqual("FF", programmerViewModel1.HexDisplayValue);
            Assert.AreEqual("-1", programmerViewModel1.DecimalDisplayValue);
            Assert.AreEqual("377", programmerViewModel1.OctalDisplayValue);
            Assert.AreEqual("1111 1111", programmerViewModel1.BinaryDisplayValue);

            // Programmer 2: 77777 (word, octal)
            var testItems2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Seven, "7", ""),
                new(NumbersAndOperatorsEnum.Seven, "77", ""),
                new(NumbersAndOperatorsEnum.Seven, "777", ""),
                new(NumbersAndOperatorsEnum.Seven, "7 777", ""),
                new(NumbersAndOperatorsEnum.Seven, "77 777", ""),
                // One more '7' shouldn't have any effect, testing for precision
                new(NumbersAndOperatorsEnum.Seven, "77 777", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(programmerViewModel2, testItems2, false);
            Assert.AreEqual("7FFF", programmerViewModel2.HexDisplayValue);
            Assert.AreEqual("32,767", programmerViewModel2.DecimalDisplayValue);
            Assert.AreEqual("77 777", programmerViewModel2.OctalDisplayValue);
            Assert.AreEqual("0111 1111 1111 1111", programmerViewModel2.BinaryDisplayValue);
        }

        [TestMethod]
        public void MultipleModesHistoryAddItemTest()
        {
            var viewModels = new StandardCalculatorViewModel[2];

            for (int i = 0; i < 2; i++)
            {
                viewModels[i] = new StandardCalculatorViewModel();
                ChangeMode(viewModels[i], i);
                Assert.AreEqual(0, viewModels[i].HistoryVM.ItemsCount);
            }

            // Standard Mode: 1+2=3
            var standardItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Equals, "3", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[0], standardItems, true);

            // Scientific Mode: 1+2*3=7
            var scientificItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Multiply, "2", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Equals, "7", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[1], scientificItems, true);

            // Assert history for Standard instance
            Assert.AreEqual(1, viewModels[0].HistoryVM.ItemsCount);
            var item1 = (HistoryItemViewModel)viewModels[0].HistoryVM.Items[0];
            Assert.AreEqual("1   +   2 =", item1.Expression);
            Assert.AreEqual("3", item1.Result);

            // Assert history for Scientific instance
            Assert.AreEqual(1, viewModels[1].HistoryVM.ItemsCount);
            var item2 = (HistoryItemViewModel)viewModels[1].HistoryVM.Items[0];
            Assert.AreEqual("1   +   2   \u00D7   3 =", item2.Expression);
            Assert.AreEqual("7", item2.Result);
        }

        [TestMethod]
        public void MultipleStandardModesHistoryAddItemTest()
        {
            var viewModels = new StandardCalculatorViewModel[2];

            for (int i = 0; i < 2; i++)
            {
                viewModels[i] = new StandardCalculatorViewModel();
                ChangeMode(viewModels[i], 0);
                Assert.AreEqual(0, viewModels[i].HistoryVM.ItemsCount);
            }

            // Instance 0: 3+2=5, then 5*1=5
            var items0 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsStandardMode, "0", ""),
                new(NumbersAndOperatorsEnum.Three, "3", ""),
                new(NumbersAndOperatorsEnum.Add, "3", "3 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "3 + "),
                new(NumbersAndOperatorsEnum.Multiply, "5", "5 * "),
                new(NumbersAndOperatorsEnum.One, "1", "5 * "),
                new(NumbersAndOperatorsEnum.Equals, "5", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[0], items0, true);

            // Instance 1: 1+2=3, then 3*3=9
            var items1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsStandardMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Multiply, "3", "3 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "3 * "),
                new(NumbersAndOperatorsEnum.Equals, "9", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[1], items1, true);

            // Verify history items
            string[,] expressions = {
                { "5   \u00D7   1 =", "3   +   2 =" },
                { "3   \u00D7   3 =", "1   +   2 =" }
            };
            string[,] results = {
                { "5", "5" },
                { "9", "3" }
            };

            for (int i = 0; i < 2; i++)
            {
                Assert.AreEqual(2, viewModels[i].HistoryVM.ItemsCount);

                for (int j = 0; j < 2; j++)
                {
                    var item = (HistoryItemViewModel)viewModels[i].HistoryVM.Items[j];
                    Assert.AreEqual(expressions[i, j], item.Expression);
                    Assert.AreEqual(results[i, j], item.Result);
                }
            }
        }

        [TestMethod]
        public void MultipleScientificModesHistoryAddItemTest()
        {
            var viewModels = new StandardCalculatorViewModel[2];

            for (int i = 0; i < 2; i++)
            {
                viewModels[i] = new StandardCalculatorViewModel();
                ChangeMode(viewModels[i], 1);
                Assert.AreEqual(0, viewModels[i].HistoryVM.ItemsCount);
            }

            // Instance 0: 1+2=3
            var items0 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Equals, "3", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[0], items0, true);

            // Instance 1: 1+2*3=7
            var items1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Multiply, "2", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Equals, "7", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[1], items1, true);

            string[] expectedExpressions = { "1   +   2 =", "1   +   2   \u00D7   3 =" };
            string[] expectedResults = { "3", "7" };

            for (int i = 0; i < 2; i++)
            {
                Assert.AreEqual(1, viewModels[i].HistoryVM.ItemsCount);
                var item = (HistoryItemViewModel)viewModels[i].HistoryVM.Items[0];
                Assert.AreEqual(expectedExpressions[i], item.Expression);
                Assert.AreEqual(expectedResults[i], item.Result);
            }
        }

        [TestMethod]
        public void MultipleModesMemoryAddItemTest()
        {
            var viewModels = new StandardCalculatorViewModel[3];

            for (int i = 0; i < 3; i++)
            {
                viewModels[i] = new StandardCalculatorViewModel();
                ChangeMode(viewModels[i], i);
                Assert.AreEqual(0, viewModels[i].MemorizedNumbers.Count);
            }

            // Standard Mode: 1+2=3
            var standardItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Equals, "3", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[0], standardItems, true);

            // Scientific Mode: 1+2*3=7
            var scientificItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Multiply, "2", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Equals, "7", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[1], scientificItems, true);

            // Programmer Mode: Hex F
            var programmerItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[2], programmerItems, false);

            // Press Memory button to save values
            for (int i = 0; i < 3; i++)
            {
                viewModels[i].ButtonPressed.Execute(NumbersAndOperatorsEnum.Memory);
            }

            string[] expectedMemoryValues = { "3", "7", "F" };

            for (int i = 0; i < 3; i++)
            {
                Assert.AreEqual(1, viewModels[i].MemorizedNumbers.Count);
                var memorySlot = viewModels[i].MemorizedNumbers[0];
                Assert.AreEqual(expectedMemoryValues[i], memorySlot.Value);
            }
        }

        [TestMethod]
        public void DateCalculatorInstancesKeepIndependentState()
        {
            var first = new DateCalculatorViewModel { IsDateDiffMode = true };
            var second = new DateCalculatorViewModel { IsDateDiffMode = true };
            var third = new DateCalculatorViewModel();

            first.FromDate = new DateTimeOffset(2008, 1, 31, 0, 0, 0, TimeSpan.Zero);
            first.ToDate = new DateTimeOffset(2008, 3, 31, 0, 0, 0, TimeSpan.Zero);

            second.FromDate = new DateTimeOffset(2007, 2, 28, 0, 0, 0, TimeSpan.Zero);
            second.ToDate = new DateTimeOffset(2007, 3, 10, 0, 0, 0, TimeSpan.Zero);

            third.IsDateDiffMode = false;
            third.IsAddMode = true;
            third.StartDate = new DateTimeOffset(2008, 1, 31, 0, 0, 0, TimeSpan.Zero);
            third.DaysOffset = 1;

            Assert.AreNotEqual(first.StrDateDiffResult, second.StrDateDiffResult,
                "Two date calculators shared a result.");
            Assert.IsFalse(string.IsNullOrEmpty(third.StrDateResult));
            Assert.IsTrue(first.IsDateDiffMode, "One date calculator's mode changed another's.");
        }

        [TestMethod]
        public void ConverterInstancesKeepIndependentState()
        {
            var converters = new[]
            {
                new UnitConverterViewModel(),
                new UnitConverterViewModel(),
                new UnitConverterViewModel(),
            };

            foreach (var converter in converters)
            {
                Assert.IsTrue(converter.Categories.Count > 0);
                Assert.IsNotNull(converter.Unit1);
                Assert.IsNotNull(converter.Unit2);
            }

            var other = converters[0].Categories.First(
                category => category.GetModelCategoryId() != converters[0].CurrentCategory.GetModelCategoryId());
            converters[0].CurrentCategory = other;
            converters[2].CurrentCategory = other;

            Assert.AreEqual(other.GetModelCategoryId(), converters[0].CurrentCategory.GetModelCategoryId());
            Assert.AreEqual(other.GetModelCategoryId(), converters[2].CurrentCategory.GetModelCategoryId());
            Assert.AreNotEqual(
                other.GetModelCategoryId(),
                converters[1].CurrentCategory.GetModelCategoryId(),
                "Changing one converter's category changed another's.");

            converters[0].ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.One);
            converters[1].ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Two);
            converters[2].ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Three);

            Assert.AreEqual("1", converters[0].Value1);
            Assert.AreEqual("2", converters[1].Value1);
            Assert.AreEqual("3", converters[2].Value1);
        }

        [TestMethod]
        public void CalculatorModeViewModelsKeepIndependentState()
        {
            var standard = new StandardCalculatorViewModel { IsStandard = true };
            var converter = new UnitConverterViewModel();
            var dateCalc = new DateCalculatorViewModel { IsDateDiffMode = false, IsAddMode = true };

            standard.ButtonPressed.Execute(NumbersAndOperatorsEnum.Three);
            standard.ButtonPressed.Execute(NumbersAndOperatorsEnum.Subtract);
            standard.ButtonPressed.Execute(NumbersAndOperatorsEnum.Two);
            standard.ButtonPressed.Execute(NumbersAndOperatorsEnum.Equals);
            Assert.AreEqual("1", standard.DisplayValue);

            dateCalc.DaysOffset = 1;
            converter.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Two);

            Assert.AreEqual("2", converter.Value1);
            Assert.AreEqual("1", standard.DisplayValue, "The converter disturbed the standard calculator.");
            Assert.IsFalse(string.IsNullOrEmpty(dateCalc.StrDateResult));

            standard.ButtonPressed.Execute(NumbersAndOperatorsEnum.One);
            standard.ButtonPressed.Execute(NumbersAndOperatorsEnum.Add);
            standard.ButtonPressed.Execute(NumbersAndOperatorsEnum.Two);
            standard.ButtonPressed.Execute(NumbersAndOperatorsEnum.Equals);

            Assert.AreEqual("3", standard.DisplayValue);
            Assert.AreEqual("2", converter.Value1, "The standard calculator disturbed the converter.");
        }

        [TestMethod]
        public void MultipleModesWithChangeInProgrammerRadix()
        {
            var viewModels = new StandardCalculatorViewModel[3];

            // Standard mode
            viewModels[0] = new StandardCalculatorViewModel();
            ChangeMode(viewModels[0], 0);

            // Programmer mode
            viewModels[1] = new StandardCalculatorViewModel();
            ChangeMode(viewModels[1], 2);

            // Change programmer radix to hex and enter F
            var programmerItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[1], programmerItems, false);
            Assert.AreEqual("F", viewModels[1].HexDisplayValue);

            // Standard Mode: 10+2=12 (radix should be decimal, not hex)
            var standardItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Add, "10", "10 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "10 + "),
                new(NumbersAndOperatorsEnum.Equals, "12", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[0], standardItems, false);

            // Launch a new Standard instance
            viewModels[2] = new StandardCalculatorViewModel();
            ChangeMode(viewModels[2], 0);

            // New Standard: 10+2=12
            var standardItemsNew = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Add, "10", "10 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "10 + "),
                new(NumbersAndOperatorsEnum.Equals, "12", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[2], standardItemsNew, false);

            // Programmer radix should still be hex: A+1=B
            var programmerItemsNew = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.A, "A", ""),
                new(NumbersAndOperatorsEnum.Add, "A", "A + "),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Equals, "B", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[1], programmerItemsNew, true);
            Assert.AreEqual("B", viewModels[1].HexDisplayValue);
        }
    }
```

#### 4.8.Z Remaining non-blank source in this file

<!-- MultiWindowTests.cs:813-813 -->
```csharp
}
```

### 4.9 `src/Calculator.Tests/NarratorAnnouncementTests.cs`

1 `[TestClass]`(es), 19 test method(s).

| # | Test method | Attributes | Source lines |
|---|---|---|---|
| 1 | `TestGetDisplayUpdatedAnnouncement` | `[TestMethod]` | `NarratorAnnouncementTests.cs:15-23` |
| 2 | `TestGetMaxDigitsReachedAnnouncement` | `[TestMethod]` | `NarratorAnnouncementTests.cs:25-33` |
| 3 | `TestGetMemoryClearedAnnouncement` | `[TestMethod]` | `NarratorAnnouncementTests.cs:35-43` |
| 4 | `TestGetMemoryItemChangedAnnouncement` | `[TestMethod]` | `NarratorAnnouncementTests.cs:45-53` |
| 5 | `TestGetMemoryItemAddedAnnouncement` | `[TestMethod]` | `NarratorAnnouncementTests.cs:55-63` |
| 6 | `TestGetHistoryClearedAnnouncement` | `[TestMethod]` | `NarratorAnnouncementTests.cs:65-73` |
| 7 | `TestGetHistorySlotClearedAnnouncement` | `[TestMethod]` | `NarratorAnnouncementTests.cs:75-83` |
| 8 | `TestGetCategoryNameChangedAnnouncement` | `[TestMethod]` | `NarratorAnnouncementTests.cs:85-93` |
| 9 | `TestGetUpdateCurrencyRatesAnnouncement` | `[TestMethod]` | `NarratorAnnouncementTests.cs:95-103` |
| 10 | `TestGetDisplayCopiedAnnouncement` | `[TestMethod]` | `NarratorAnnouncementTests.cs:105-113` |
| 11 | `TestGetOpenParenthesisCountChangedAnnouncement` | `[TestMethod]` | `NarratorAnnouncementTests.cs:115-123` |
| 12 | `TestGetNoRightParenthesisAddedAnnouncement` | `[TestMethod]` | `NarratorAnnouncementTests.cs:125-133` |
| 13 | `TestGetGraphModeChangedAnnouncement` | `[TestMethod]` | `NarratorAnnouncementTests.cs:135-143` |
| 14 | `TestGetGraphViewChangedAnnouncement` | `[TestMethod]` | `NarratorAnnouncementTests.cs:145-153` |
| 15 | `TestGetFunctionRemovedAnnouncement` | `[TestMethod]` | `NarratorAnnouncementTests.cs:155-163` |
| 16 | `TestGetGraphViewBestFitChangedAnnouncement` | `[TestMethod]` | `NarratorAnnouncementTests.cs:165-173` |
| 17 | `TestGetAlwaysOnTopChangedAnnouncement` | `[TestMethod]` | `NarratorAnnouncementTests.cs:175-183` |
| 18 | `TestGetBitShiftRadioButtonCheckedAnnouncement` | `[TestMethod]` | `NarratorAnnouncementTests.cs:185-193` |
| 19 | `TestGetSettingsPageOpenedAnnouncement` | `[TestMethod]` | `NarratorAnnouncementTests.cs:195-203` |

#### 4.9.0 File preamble — `using` directives, file-scoped structs/helpers, namespace open (`NarratorAnnouncementTests.cs:1-9`)

```csharp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel.Common.Automation;
using Windows.UI.Xaml.Automation.Peers;

namespace Calculator.Tests
{
```

#### 4.9.1 `[TestClass] NarratorAnnouncementTests` — verbatim (`NarratorAnnouncementTests.cs:10-204`)

```csharp
    [TestClass]
    public class NarratorAnnouncementTests
    {
        private const string TestAnnouncement = "TestAnnouncement";

        [TestMethod]
        public void TestGetDisplayUpdatedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetDisplayUpdatedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("DisplayUpdated", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.Other, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetMaxDigitsReachedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetMaxDigitsReachedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("MaxDigitsReached", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.Other, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetMemoryClearedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetMemoryClearedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("MemoryCleared", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ItemRemoved, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetMemoryItemChangedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetMemoryItemChangedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("MemorySlotChanged", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.MostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetMemoryItemAddedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetMemoryItemAddedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("MemorySlotAdded", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ItemAdded, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.MostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetHistoryClearedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetHistoryClearedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("HistoryCleared", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ItemRemoved, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.MostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetHistorySlotClearedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetHistorySlotClearedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("HistorySlotCleared", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ItemRemoved, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetCategoryNameChangedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetCategoryNameChangedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("CategoryNameChanged", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetUpdateCurrencyRatesAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetUpdateCurrencyRatesAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("UpdateCurrencyRates", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetDisplayCopiedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetDisplayCopiedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("DisplayCopied", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetOpenParenthesisCountChangedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetOpenParenthesisCountChangedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("OpenParenthesisCountChanged", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetNoRightParenthesisAddedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetNoRightParenthesisAddedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("NoParenthesisAdded", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetGraphModeChangedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetGraphModeChangedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("GraphModeChanged", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetGraphViewChangedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetGraphViewChangedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("GraphViewChanged", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.CurrentThenMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetFunctionRemovedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetFunctionRemovedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("FunctionRemoved", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ItemRemoved, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetGraphViewBestFitChangedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetGraphViewBestFitChangedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("GraphViewBestFitChanged", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.MostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetAlwaysOnTopChangedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetAlwaysOnTopChangedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("AlwaysOnTop", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetBitShiftRadioButtonCheckedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetBitShiftRadioButtonCheckedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("BitShiftRadioButtonContent", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetSettingsPageOpenedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetSettingsPageOpenedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("SettingsPageOpened", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }
    }
```

#### 4.9.Z Remaining non-blank source in this file

<!-- NarratorAnnouncementTests.cs:205-205 -->
```csharp
}
```

### 4.10 `src/Calculator.Tests/NavCategoryTests.cs`

3 `[TestClass]`(es), 18 test method(s).

| # | Test method | Attributes | Source lines |
|---|---|---|---|
| 1 | `IsCalculatorViewMode` | `[TestMethod]` | `NavCategoryTests.cs:13-36` |
| 2 | `IsDateCalculatorViewMode` | `[TestMethod]` | `NavCategoryTests.cs:38-61` |
| 3 | `IsConverterViewMode` | `[TestMethod]` | `NavCategoryTests.cs:63-85` |
| 4 | `CreateNavCategoryGroup` | `[TestMethod]` | `NavCategoryTests.cs:91-127` |
| 5 | `Serialize` | `[TestMethod]` | `NavCategoryTests.cs:150-173` |
| 6 | `Deserialize_AllValid` | `[TestMethod]` | `NavCategoryTests.cs:175-196` |
| 7 | `Deserialize_AllInvalid` | `[TestMethod]` | `NavCategoryTests.cs:198-207` |
| 8 | `IsValidViewMode_AllValid` | `[TestMethod]` | `NavCategoryTests.cs:209-230` |
| 9 | `IsValidViewMode_AllInvalid` | `[TestMethod]` | `NavCategoryTests.cs:232-242` |
| 10 | `GetFriendlyName` | `[TestMethod]` | `NavCategoryTests.cs:244-267` |
| 11 | `GetGroupType` | `[TestMethod]` | `NavCategoryTests.cs:269-291` |
| 12 | `GetIndex` | `[TestMethod]` | `NavCategoryTests.cs:293-304` |
| 13 | `GetPosition` | `[TestMethod]` | `NavCategoryTests.cs:306-317` |
| 14 | `GetIndex_GetPosition_Relationship` | `[TestMethod]` | `NavCategoryTests.cs:319-325` |
| 15 | `GetIndexInGroup` | `[TestMethod]` | `NavCategoryTests.cs:327-351` |
| 16 | `GetViewModeForVirtualKey` | `[TestMethod]` | `NavCategoryTests.cs:353-361` |
| 17 | `IsViewModeEnabled_ShouldBeTrue_ExceptGraphingMode` | `[TestMethod]` | `NavCategoryTests.cs:363-373` |
| 18 | `GraphingModeIsEnabled_ShouldBeTrue_WhenNullUserAssigned` | `[TestMethod]` | `NavCategoryTests.cs:375-380` |

#### 4.10.0 File preamble — `using` directives, file-scoped structs/helpers, namespace open (`NavCategoryTests.cs:1-9`)

```csharp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel.Common;
using System.Collections.Generic;

namespace Calculator.Tests
{
```

#### 4.10.1 `[TestClass] NavCategoryTests` — verbatim (`NavCategoryTests.cs:10-86`)

```csharp
    [TestClass]
    public class NavCategoryTests
    {
        [TestMethod]
        public void IsCalculatorViewMode()
        {
            Assert.IsTrue(NavCategory.IsCalculatorViewMode(ViewMode.Standard));
            Assert.IsTrue(NavCategory.IsCalculatorViewMode(ViewMode.Scientific));
            Assert.IsTrue(NavCategory.IsCalculatorViewMode(ViewMode.Programmer));

            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Date));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Graphing));

            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Currency));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Volume));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Length));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Weight));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Temperature));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Energy));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Area));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Speed));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Time));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Power));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Data));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Pressure));
            Assert.IsFalse(NavCategory.IsCalculatorViewMode(ViewMode.Angle));
        }

        [TestMethod]
        public void IsDateCalculatorViewMode()
        {
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Standard));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Scientific));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Programmer));

            Assert.IsTrue(NavCategory.IsDateCalculatorViewMode(ViewMode.Date));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Graphing));

            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Currency));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Volume));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Length));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Weight));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Temperature));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Energy));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Area));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Speed));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Time));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Power));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Data));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Pressure));
            Assert.IsFalse(NavCategory.IsDateCalculatorViewMode(ViewMode.Angle));
        }

        [TestMethod]
        public void IsConverterViewMode()
        {
            Assert.IsFalse(NavCategory.IsConverterViewMode(ViewMode.Standard));
            Assert.IsFalse(NavCategory.IsConverterViewMode(ViewMode.Scientific));
            Assert.IsFalse(NavCategory.IsConverterViewMode(ViewMode.Programmer));
            Assert.IsFalse(NavCategory.IsConverterViewMode(ViewMode.Date));
            Assert.IsFalse(NavCategory.IsConverterViewMode(ViewMode.Graphing));

            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Currency));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Volume));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Length));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Weight));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Temperature));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Energy));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Area));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Speed));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Time));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Power));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Data));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Pressure));
            Assert.IsTrue(NavCategory.IsConverterViewMode(ViewMode.Angle));
        }
    }
```

#### 4.10.2 `[TestClass] NavCategoryGroupTests` — verbatim (`NavCategoryTests.cs:88-138`)

```csharp
    [TestClass]
    public class NavCategoryGroupTests
    {
        [TestMethod]
        public void CreateNavCategoryGroup()
        {
            var menuOptions = NavCategoryStates.CreateMenuOptions();

            Assert.AreEqual(2, menuOptions.Count);

            var calculatorGroup = (NavCategoryGroup)menuOptions[0];
            Assert.AreEqual(CategoryGroupType.Calculator, calculatorGroup.GroupType);

            var calculatorCategories = calculatorGroup.Categories;
            ValidateNavCategory(calculatorCategories, 0, ViewMode.Standard);
            ValidateNavCategory(calculatorCategories, 1, ViewMode.Scientific);
            ValidateNavCategory(calculatorCategories, 2, ViewMode.Graphing);
            ValidateNavCategory(calculatorCategories, 3, ViewMode.Programmer);
            ValidateNavCategory(calculatorCategories, 4, ViewMode.Date);
            Assert.AreEqual(5, calculatorCategories.Count);

            var converterGroup = (NavCategoryGroup)menuOptions[1];
            Assert.AreEqual(CategoryGroupType.Converter, converterGroup.GroupType);

            var converterCategories = converterGroup.Categories;
            Assert.AreEqual(13, converterCategories.Count);
            ValidateNavCategory(converterCategories, 0, ViewMode.Currency);
            ValidateNavCategory(converterCategories, 1, ViewMode.Volume);
            ValidateNavCategory(converterCategories, 2, ViewMode.Length);
            ValidateNavCategory(converterCategories, 3, ViewMode.Weight);
            ValidateNavCategory(converterCategories, 4, ViewMode.Temperature);
            ValidateNavCategory(converterCategories, 5, ViewMode.Energy);
            ValidateNavCategory(converterCategories, 6, ViewMode.Area);
            ValidateNavCategory(converterCategories, 7, ViewMode.Speed);
            ValidateNavCategory(converterCategories, 8, ViewMode.Time);
            ValidateNavCategory(converterCategories, 9, ViewMode.Power);
            ValidateNavCategory(converterCategories, 10, ViewMode.Data);
            ValidateNavCategory(converterCategories, 11, ViewMode.Pressure);
            ValidateNavCategory(converterCategories, 12, ViewMode.Angle);
        }

        private void ValidateNavCategory(IList<NavCategory> categories, int index, ViewMode expectedMode)
        {
            Assert.IsTrue(categories.Count > 0);
            Assert.IsTrue(categories.Count > index);

            var category = categories[index];
            Assert.AreEqual(expectedMode, category.ViewMode);
            Assert.AreEqual(expectedMode != ViewMode.Graphing, category.IsEnabled);
        }
    }
```

#### 4.10.3 `[TestClass] NavCategoryStatesTests` — verbatim (`NavCategoryTests.cs:140-381`)

```csharp
    [TestClass]
    public class NavCategoryStatesTests
    {
        private static readonly ViewMode[] OrderedModes =
        {
            ViewMode.Standard, ViewMode.Scientific, ViewMode.Graphing, ViewMode.Programmer, ViewMode.Date, ViewMode.Currency,
            ViewMode.Volume, ViewMode.Length, ViewMode.Weight, ViewMode.Temperature, ViewMode.Energy, ViewMode.Area,
            ViewMode.Speed, ViewMode.Time, ViewMode.Power, ViewMode.Data, ViewMode.Pressure, ViewMode.Angle
        };

        [TestMethod]
        public void Serialize()
        {
            // These values should NEVER change. We are validating the unique ID for each mode.
            Assert.AreEqual(0, NavCategoryStates.Serialize(ViewMode.Standard));
            Assert.AreEqual(1, NavCategoryStates.Serialize(ViewMode.Scientific));
            Assert.AreEqual(2, NavCategoryStates.Serialize(ViewMode.Programmer));
            Assert.AreEqual(3, NavCategoryStates.Serialize(ViewMode.Date));
            Assert.AreEqual(16, NavCategoryStates.Serialize(ViewMode.Currency));
            Assert.AreEqual(4, NavCategoryStates.Serialize(ViewMode.Volume));
            Assert.AreEqual(5, NavCategoryStates.Serialize(ViewMode.Length));
            Assert.AreEqual(6, NavCategoryStates.Serialize(ViewMode.Weight));
            Assert.AreEqual(7, NavCategoryStates.Serialize(ViewMode.Temperature));
            Assert.AreEqual(8, NavCategoryStates.Serialize(ViewMode.Energy));
            Assert.AreEqual(9, NavCategoryStates.Serialize(ViewMode.Area));
            Assert.AreEqual(10, NavCategoryStates.Serialize(ViewMode.Speed));
            Assert.AreEqual(11, NavCategoryStates.Serialize(ViewMode.Time));
            Assert.AreEqual(12, NavCategoryStates.Serialize(ViewMode.Power));
            Assert.AreEqual(13, NavCategoryStates.Serialize(ViewMode.Data));
            Assert.AreEqual(14, NavCategoryStates.Serialize(ViewMode.Pressure));
            Assert.AreEqual(15, NavCategoryStates.Serialize(ViewMode.Angle));

            Assert.AreEqual(-1, NavCategoryStates.Serialize(ViewMode.None));
        }

        [TestMethod]
        public void Deserialize_AllValid()
        {
            // These values should NEVER change.
            Assert.AreEqual(ViewMode.Standard, NavCategoryStates.Deserialize(0));
            Assert.AreEqual(ViewMode.Scientific, NavCategoryStates.Deserialize(1));
            Assert.AreEqual(ViewMode.Programmer, NavCategoryStates.Deserialize(2));
            Assert.AreEqual(ViewMode.Date, NavCategoryStates.Deserialize(3));
            Assert.AreEqual(ViewMode.Currency, NavCategoryStates.Deserialize(16));
            Assert.AreEqual(ViewMode.Volume, NavCategoryStates.Deserialize(4));
            Assert.AreEqual(ViewMode.Length, NavCategoryStates.Deserialize(5));
            Assert.AreEqual(ViewMode.Weight, NavCategoryStates.Deserialize(6));
            Assert.AreEqual(ViewMode.Temperature, NavCategoryStates.Deserialize(7));
            Assert.AreEqual(ViewMode.Energy, NavCategoryStates.Deserialize(8));
            Assert.AreEqual(ViewMode.Area, NavCategoryStates.Deserialize(9));
            Assert.AreEqual(ViewMode.Speed, NavCategoryStates.Deserialize(10));
            Assert.AreEqual(ViewMode.Time, NavCategoryStates.Deserialize(11));
            Assert.AreEqual(ViewMode.Power, NavCategoryStates.Deserialize(12));
            Assert.AreEqual(ViewMode.Data, NavCategoryStates.Deserialize(13));
            Assert.AreEqual(ViewMode.Pressure, NavCategoryStates.Deserialize(14));
            Assert.AreEqual(ViewMode.Angle, NavCategoryStates.Deserialize(15));
        }

        [TestMethod]
        public void Deserialize_AllInvalid()
        {
            Assert.AreEqual(ViewMode.None, NavCategoryStates.Deserialize(null));
            Assert.AreEqual(ViewMode.None, NavCategoryStates.Deserialize("fail"));

            // Boundary testing
            Assert.AreEqual(ViewMode.None, NavCategoryStates.Deserialize(-1));
            Assert.AreEqual(ViewMode.None, NavCategoryStates.Deserialize(18));
        }

        [TestMethod]
        public void IsValidViewMode_AllValid()
        {
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Standard));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Scientific));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Programmer));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Date));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Graphing));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Currency));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Volume));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Length));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Weight));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Temperature));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Energy));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Area));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Speed));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Time));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Power));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Data));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Pressure));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode(ViewMode.Angle));
        }

        [TestMethod]
        public void IsValidViewMode_AllInvalid()
        {
            Assert.IsFalse(NavCategoryStates.IsValidViewMode(ViewMode.None));
            Assert.IsTrue(NavCategoryStates.IsValidViewMode((ViewMode)17));
            Assert.IsFalse(NavCategoryStates.IsValidViewMode((ViewMode)18));

            // Also verify the lower bound
            Assert.IsTrue(NavCategoryStates.IsValidViewMode((ViewMode)0));
            Assert.IsFalse(NavCategoryStates.IsValidViewMode((ViewMode)(-1)));
        }

        [TestMethod]
        public void GetFriendlyName()
        {
            Assert.AreEqual("Standard", NavCategoryStates.GetFriendlyName(ViewMode.Standard));
            Assert.AreEqual("Scientific", NavCategoryStates.GetFriendlyName(ViewMode.Scientific));
            Assert.AreEqual("Programmer", NavCategoryStates.GetFriendlyName(ViewMode.Programmer));
            Assert.AreEqual("Date", NavCategoryStates.GetFriendlyName(ViewMode.Date));
            Assert.AreEqual("Graphing", NavCategoryStates.GetFriendlyName(ViewMode.Graphing));
            Assert.AreEqual("Currency", NavCategoryStates.GetFriendlyName(ViewMode.Currency));
            Assert.AreEqual("Volume", NavCategoryStates.GetFriendlyName(ViewMode.Volume));
            Assert.AreEqual("Length", NavCategoryStates.GetFriendlyName(ViewMode.Length));
            Assert.AreEqual("Weight and Mass", NavCategoryStates.GetFriendlyName(ViewMode.Weight));
            Assert.AreEqual("Temperature", NavCategoryStates.GetFriendlyName(ViewMode.Temperature));
            Assert.AreEqual("Energy", NavCategoryStates.GetFriendlyName(ViewMode.Energy));
            Assert.AreEqual("Area", NavCategoryStates.GetFriendlyName(ViewMode.Area));
            Assert.AreEqual("Speed", NavCategoryStates.GetFriendlyName(ViewMode.Speed));
            Assert.AreEqual("Time", NavCategoryStates.GetFriendlyName(ViewMode.Time));
            Assert.AreEqual("Power", NavCategoryStates.GetFriendlyName(ViewMode.Power));
            Assert.AreEqual("Data", NavCategoryStates.GetFriendlyName(ViewMode.Data));
            Assert.AreEqual("Pressure", NavCategoryStates.GetFriendlyName(ViewMode.Pressure));
            Assert.AreEqual("Angle", NavCategoryStates.GetFriendlyName(ViewMode.Angle));

            Assert.AreEqual("None", NavCategoryStates.GetFriendlyName(ViewMode.None));
        }

        [TestMethod]
        public void GetGroupType()
        {
            Assert.AreEqual(CategoryGroupType.Calculator, NavCategoryStates.GetGroupType(ViewMode.Standard));
            Assert.AreEqual(CategoryGroupType.Calculator, NavCategoryStates.GetGroupType(ViewMode.Scientific));
            Assert.AreEqual(CategoryGroupType.Calculator, NavCategoryStates.GetGroupType(ViewMode.Programmer));
            Assert.AreEqual(CategoryGroupType.Calculator, NavCategoryStates.GetGroupType(ViewMode.Date));
            Assert.AreEqual(CategoryGroupType.Calculator, NavCategoryStates.GetGroupType(ViewMode.Graphing));

            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Currency));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Volume));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Length));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Weight));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Temperature));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Energy));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Area));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Speed));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Time));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Power));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Data));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Pressure));
            Assert.AreEqual(CategoryGroupType.Converter, NavCategoryStates.GetGroupType(ViewMode.Angle));
        }

        [TestMethod]
        public void GetIndex()
        {
            // Index is the 0-based ordering of modes
            for (int index = 0; index < OrderedModes.Length; index++)
            {
                ViewMode mode = OrderedModes[index];
                Assert.AreEqual(index, NavCategoryStates.GetIndex(mode));
            }

            Assert.AreEqual(-1, NavCategoryStates.GetIndex(ViewMode.None));
        }

        [TestMethod]
        public void GetPosition()
        {
            // Position is the 1-based ordering of modes
            for (int pos = 1; pos <= OrderedModes.Length; pos++)
            {
                ViewMode mode = OrderedModes[pos - 1];
                Assert.AreEqual(pos, NavCategoryStates.GetPosition(mode));
            }

            Assert.AreEqual(-1, NavCategoryStates.GetPosition(ViewMode.None));
        }

        [TestMethod]
        public void GetIndex_GetPosition_Relationship()
        {
            // Index should be 1 less than Position.
            Assert.AreEqual(NavCategoryStates.GetIndex(ViewMode.Standard) + 1, NavCategoryStates.GetPosition(ViewMode.Standard));
            Assert.AreEqual(NavCategoryStates.GetPosition(ViewMode.Volume) - 1, NavCategoryStates.GetIndex(ViewMode.Volume));
        }

        [TestMethod]
        public void GetIndexInGroup()
        {
            Assert.AreEqual(0, NavCategoryStates.GetIndexInGroup(ViewMode.Standard, CategoryGroupType.Calculator));
            Assert.AreEqual(1, NavCategoryStates.GetIndexInGroup(ViewMode.Scientific, CategoryGroupType.Calculator));
            Assert.AreEqual(2, NavCategoryStates.GetIndexInGroup(ViewMode.Graphing, CategoryGroupType.Calculator));
            Assert.AreEqual(3, NavCategoryStates.GetIndexInGroup(ViewMode.Programmer, CategoryGroupType.Calculator));
            Assert.AreEqual(4, NavCategoryStates.GetIndexInGroup(ViewMode.Date, CategoryGroupType.Calculator));
            Assert.AreEqual(0, NavCategoryStates.GetIndexInGroup(ViewMode.Currency, CategoryGroupType.Converter));
            Assert.AreEqual(1, NavCategoryStates.GetIndexInGroup(ViewMode.Volume, CategoryGroupType.Converter));
            Assert.AreEqual(2, NavCategoryStates.GetIndexInGroup(ViewMode.Length, CategoryGroupType.Converter));
            Assert.AreEqual(3, NavCategoryStates.GetIndexInGroup(ViewMode.Weight, CategoryGroupType.Converter));
            Assert.AreEqual(4, NavCategoryStates.GetIndexInGroup(ViewMode.Temperature, CategoryGroupType.Converter));
            Assert.AreEqual(5, NavCategoryStates.GetIndexInGroup(ViewMode.Energy, CategoryGroupType.Converter));
            Assert.AreEqual(6, NavCategoryStates.GetIndexInGroup(ViewMode.Area, CategoryGroupType.Converter));
            Assert.AreEqual(7, NavCategoryStates.GetIndexInGroup(ViewMode.Speed, CategoryGroupType.Converter));
            Assert.AreEqual(8, NavCategoryStates.GetIndexInGroup(ViewMode.Time, CategoryGroupType.Converter));
            Assert.AreEqual(9, NavCategoryStates.GetIndexInGroup(ViewMode.Power, CategoryGroupType.Converter));
            Assert.AreEqual(10, NavCategoryStates.GetIndexInGroup(ViewMode.Data, CategoryGroupType.Converter));
            Assert.AreEqual(11, NavCategoryStates.GetIndexInGroup(ViewMode.Pressure, CategoryGroupType.Converter));
            Assert.AreEqual(12, NavCategoryStates.GetIndexInGroup(ViewMode.Angle, CategoryGroupType.Converter));

            Assert.AreEqual(-1, NavCategoryStates.GetIndexInGroup(ViewMode.None, CategoryGroupType.Calculator));
            Assert.AreEqual(-1, NavCategoryStates.GetIndexInGroup(ViewMode.None, CategoryGroupType.Converter));
        }

        [TestMethod]
        public void GetViewModeForVirtualKey()
        {
            Assert.AreEqual(ViewMode.Standard, NavCategoryStates.GetViewModeForVirtualKey(MyVirtualKey.Number1));
            Assert.AreEqual(ViewMode.Scientific, NavCategoryStates.GetViewModeForVirtualKey(MyVirtualKey.Number2));
            Assert.AreEqual(ViewMode.Graphing, NavCategoryStates.GetViewModeForVirtualKey(MyVirtualKey.Number3));
            Assert.AreEqual(ViewMode.Programmer, NavCategoryStates.GetViewModeForVirtualKey(MyVirtualKey.Number4));
            Assert.AreEqual(ViewMode.Date, NavCategoryStates.GetViewModeForVirtualKey(MyVirtualKey.Number5));
        }

        [TestMethod]
        public void IsViewModeEnabled_ShouldBeTrue_ExceptGraphingMode()
        {
            foreach (var mode in OrderedModes)
            {
                if (mode != ViewMode.Graphing)
                {
                    Assert.IsTrue(NavCategoryStates.IsViewModeEnabled(mode));
                }
            }
        }

        [TestMethod]
        public void GraphingModeIsEnabled_ShouldBeTrue_WhenNullUserAssigned()
        {
            NavCategoryStates.SetCurrentUser("null-user"); // make sure User.GetFromId() returns null
            Assert.IsTrue(NavCategoryStates.IsViewModeEnabled(ViewMode.Graphing));
        }
    }
```

#### 4.10.Z Remaining non-blank source in this file

<!-- NavCategoryTests.cs:382-382 -->
```csharp
}
```

### 4.11 `src/Calculator.Tests/SnapshotJsonTests.cs`

1 `[TestClass]`(es), 13 test method(s).

| # | Test method | Attributes | Source lines |
|---|---|---|---|
| 1 | `InvalidModeSetsLaunchError` | `[DataTestMethod] [DataRow(-1)] [DataRow(int.MaxValue)]` | `SnapshotJsonTests.cs:22-34` |
| 2 | `CalculatorModeWithoutStateSetsLaunchError` | `[DataTestMethod] [DataRow((int)ViewMode.Standard)] [DataRow((int)ViewMode.Scientific)] [DataRow((int)ViewMode.Programmer)]` | `SnapshotJsonTests.cs:36-49` |
| 3 | `InvalidHistoryTokenIndexSetsLaunchError` | `[DataTestMethod] [DataRow(-2)] [DataRow(1)]` | `SnapshotJsonTests.cs:51-67` |
| 4 | `InvalidExpressionTokenIndexSetsLaunchError` | `[DataTestMethod] [DataRow(-2)] [DataRow(1)]` | `SnapshotJsonTests.cs:69-84` |
| 5 | `NullPrimaryDisplayValueSetsLaunchError` | `[TestMethod]` | `SnapshotJsonTests.cs:86-96` |
| 6 | `InvalidDisplayCommandSetsLaunchError` | `[DataTestMethod] [DataRow((int)CalculatorCommand.ModeProgrammer)] [DataRow(int.MaxValue)]` | `SnapshotJsonTests.cs:98-116` |
| 7 | `ValidDisplayCommandsAreAccepted` | `[TestMethod]` | `SnapshotJsonTests.cs:118-160` |
| 8 | `UnaryCommandSurvivesTheRoundTrip` | `[TestMethod]` | `SnapshotJsonTests.cs:162-170` |
| 9 | `BinaryCommandSurvivesTheRoundTrip` | `[TestMethod]` | `SnapshotJsonTests.cs:172-180` |
| 10 | `ParenthesesCommandSurvivesTheRoundTrip` | `[TestMethod]` | `SnapshotJsonTests.cs:182-190` |
| 11 | `OperandCommandCarriesItsFlagsThroughTheRoundTrip` | `[TestMethod]` | `SnapshotJsonTests.cs:192-208` |
| 12 | `MalformedUnaryCommandIsRejectedDuringDeserialization` | `[TestMethod]` | `SnapshotJsonTests.cs:210-216` |
| 13 | `TokenSurvivesTheRoundTrip` | `[TestMethod]` | `SnapshotJsonTests.cs:218-228` |

**All `[DataRow]` values in this file (complete, in declaration order):**

```
-1
int.MaxValue
(int)ViewMode.Standard
(int)ViewMode.Scientific
(int)ViewMode.Programmer
-2
1
-2
1
(int)CalculatorCommand.ModeProgrammer
int.MaxValue
```

#### 4.11.0 File preamble — `using` directives, file-scoped structs/helpers, namespace open (`SnapshotJsonTests.cs:1-18`)

```csharp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;
using System.Linq;
using System.Text.Json;

using Microsoft.VisualStudio.TestTools.UnitTesting;

using CalcManager.Interop;
using CalculatorApp;
using CalculatorApp.JsonUtils;
using CalculatorApp.ViewModel.Common;
using CalculatorApp.ViewModel.Snapshot;

namespace Calculator.Tests
{
    // Snapshot JSON is the untrusted Recall activation boundary.
```

#### 4.11.1 `[TestClass] SnapshotJsonTests` — verbatim (`SnapshotJsonTests.cs:19-253`)

```csharp
    [TestClass]
    public class SnapshotJsonTests
    {
        [DataTestMethod]
        [DataRow(-1)]
        [DataRow(int.MaxValue)]
        public void InvalidModeSetsLaunchError(int mode)
        {
            var alias = new ApplicationSnapshotAlias();
            alias.Mode = mode;

            var result = SnapshotLaunchArguments.FromJson(JsonSerializer.Serialize(alias));

            Assert.IsTrue(result.HasError);
            Assert.IsNull(result.Snapshot);
        }

        [DataTestMethod]
        [DataRow((int)ViewMode.Standard)]
        [DataRow((int)ViewMode.Scientific)]
        [DataRow((int)ViewMode.Programmer)]
        public void CalculatorModeWithoutStateSetsLaunchError(int mode)
        {
            var alias = new ApplicationSnapshotAlias();
            alias.Mode = mode;

            var result = SnapshotLaunchArguments.FromJson(JsonSerializer.Serialize(alias));

            Assert.IsTrue(result.HasError);
            Assert.IsNull(result.Snapshot);
        }

        [DataTestMethod]
        [DataRow(-2)]
        [DataRow(1)]
        public void InvalidHistoryTokenIndexSetsLaunchError(int commandIndex)
        {
            var snapshot = CreateApplicationSnapshot();
            var item = new CalcManagerHistoryItem();
            item.Commands.Add(new ExpressionCommandWrapper(
                CommandType.BinaryCommand, 93, System.Array.Empty<int>(), false, false, false));
            item.Tokens.Add(new CalcManagerToken("+", commandIndex));
            snapshot.StandardCalculator.CalcManager.HistoryItems = new List<CalcManagerHistoryItem> { item };

            var result = ParseSnapshot(snapshot);

            Assert.IsTrue(result.HasError);
            Assert.IsNull(result.Snapshot);
        }

        [DataTestMethod]
        [DataRow(-2)]
        [DataRow(1)]
        public void InvalidExpressionTokenIndexSetsLaunchError(int commandIndex)
        {
            var snapshot = CreateApplicationSnapshot();
            snapshot.StandardCalculator.ExpressionDisplay = new ExpressionDisplaySnapshot();
            snapshot.StandardCalculator.ExpressionDisplay.Commands.Add(new ExpressionCommandWrapper(
                CommandType.BinaryCommand, 93, System.Array.Empty<int>(), false, false, false));
            snapshot.StandardCalculator.ExpressionDisplay.Tokens.Add(new CalcManagerToken("+", commandIndex));

            var result = ParseSnapshot(snapshot);

            Assert.IsTrue(result.HasError);
            Assert.IsNull(result.Snapshot);
        }

        [TestMethod]
        public void NullPrimaryDisplayValueSetsLaunchError()
        {
            var snapshot = CreateApplicationSnapshot();
            snapshot.StandardCalculator.PrimaryDisplay.DisplayValue = null;

            var result = ParseSnapshot(snapshot);

            Assert.IsTrue(result.HasError);
            Assert.IsNull(result.Snapshot);
        }

        [DataTestMethod]
        [DataRow((int)CalculatorCommand.ModeProgrammer)]
        [DataRow(int.MaxValue)]
        public void InvalidDisplayCommandSetsLaunchError(int command)
        {
            var snapshot = CreateApplicationSnapshot();
            snapshot.StandardCalculator.DisplayCommands.Add(new ExpressionCommandWrapper(
                CommandType.BinaryCommand,
                command,
                System.Array.Empty<int>(),
                false,
                false,
                false));

            var result = ParseSnapshot(snapshot);

            Assert.IsTrue(result.HasError);
            Assert.IsNull(result.Snapshot);
        }

        [TestMethod]
        public void ValidDisplayCommandsAreAccepted()
        {
            var snapshot = CreateApplicationSnapshot();
            snapshot.StandardCalculator.DisplayCommands.Add(new ExpressionCommandWrapper(
                CommandType.UnaryCommand,
                0,
                new[] { (int)NumbersAndOperatorsEnum.Degree, (int)CalculatorCommand.CommandSIN },
                false,
                false,
                false));
            snapshot.StandardCalculator.DisplayCommands.Add(new ExpressionCommandWrapper(
                CommandType.BinaryCommand,
                (int)CalculatorCommand.CommandADD,
                System.Array.Empty<int>(),
                false,
                false,
                false));
            snapshot.StandardCalculator.DisplayCommands.Add(new ExpressionCommandWrapper(
                CommandType.OperandCommand,
                0,
                new[]
                {
                    (int)CalculatorCommand.Command1,
                    (int)CalculatorCommand.CommandPNT,
                    (int)CalculatorCommand.Command2
                },
                false,
                true,
                false));
            snapshot.StandardCalculator.DisplayCommands.Add(new ExpressionCommandWrapper(
                CommandType.Parentheses,
                (int)CalculatorCommand.CommandOPENP,
                System.Array.Empty<int>(),
                false,
                false,
                false));

            var result = ParseSnapshot(snapshot);

            Assert.IsFalse(result.HasError);
            Assert.IsNotNull(result.Snapshot);
        }

        [TestMethod]
        public void UnaryCommandSurvivesTheRoundTrip()
        {
            var restored = RoundTrip(new ExpressionCommandWrapper(
                CommandType.UnaryCommand, 0, new[] { 91, 92 }, false, false, false));

            Assert.AreEqual(CommandType.UnaryCommand, restored.Type);
            CollectionAssert.AreEqual(new[] { 91, 92 }, restored.Commands);
        }

        [TestMethod]
        public void BinaryCommandSurvivesTheRoundTrip()
        {
            var restored = RoundTrip(new ExpressionCommandWrapper(
                CommandType.BinaryCommand, 93, System.Array.Empty<int>(), false, false, false));

            Assert.AreEqual(CommandType.BinaryCommand, restored.Type);
            Assert.AreEqual(93, restored.Command);
        }

        [TestMethod]
        public void ParenthesesCommandSurvivesTheRoundTrip()
        {
            var restored = RoundTrip(new ExpressionCommandWrapper(
                CommandType.Parentheses, 106, System.Array.Empty<int>(), false, false, false));

            Assert.AreEqual(CommandType.Parentheses, restored.Type);
            Assert.AreEqual(106, restored.Command);
        }

        [TestMethod]
        public void OperandCommandCarriesItsFlagsThroughTheRoundTrip()
        {
            var restored = RoundTrip(new ExpressionCommandWrapper(
                CommandType.OperandCommand,
                0,
                new[] { 131, 132 },
                isNegative: true,
                isDecimalPresent: true,
                isSciFmt: true));

            Assert.AreEqual(CommandType.OperandCommand, restored.Type);
            CollectionAssert.AreEqual(new[] { 131, 132 }, restored.Commands);
            Assert.IsTrue(restored.IsNegative, "IsNegative was lost in the round trip.");
            Assert.IsTrue(restored.IsDecimalPresent, "IsDecimalPresent was lost in the round trip.");
            Assert.IsTrue(restored.IsSciFmt, "IsSciFmt was lost in the round trip.");
        }

        [TestMethod]
        public void MalformedUnaryCommandIsRejectedDuringDeserialization()
        {
            Assert.ThrowsException<System.ArgumentOutOfRangeException>(
                () => Helpers.MapCommandAlias(new UnaryCommandAlias()),
                "A unary command with no sub-commands has to be rejected at the JSON boundary.");
        }

        [TestMethod]
        public void TokenSurvivesTheRoundTrip()
        {
            var token = new CalculatorApp.ViewModel.Snapshot.CalcManagerToken("+", 3);

            var json = JsonSerializer.Serialize(new CalcManagerTokenAlias(token));
            var restored = Helpers.MapToken(JsonSerializer.Deserialize<CalcManagerTokenAlias>(json));

            Assert.AreEqual("+", restored.OpCodeName);
            Assert.AreEqual(3, restored.CommandIndex);
        }

        // Serializes through the alias exactly as the activation URI does, then maps back.
        private static ExpressionCommandWrapper RoundTrip(ExpressionCommandWrapper command)
        {
            ICalcManagerIExprCommandAlias alias = Helpers.MapCommandAlias(command);
            string json = JsonSerializer.Serialize(alias);
            var deserialized = JsonSerializer.Deserialize<ICalcManagerIExprCommandAlias>(json);
            return Helpers.MapCommandAlias(deserialized);
        }

        private static ApplicationSnapshot CreateApplicationSnapshot()
        {
            return new ApplicationSnapshot
            {
                Mode = (int)ViewMode.Standard,
                StandardCalculator = new StandardCalculatorSnapshot()
            };
        }

        private static SnapshotLaunchArguments ParseSnapshot(ApplicationSnapshot snapshot)
        {
            return SnapshotLaunchArguments.FromJson(
                JsonSerializer.Serialize(new ApplicationSnapshotAlias(snapshot)));
        }
    }
```

#### 4.11.Z Remaining non-blank source in this file

<!-- SnapshotJsonTests.cs:254-254 -->
```csharp
}
```

### 4.12 `src/Calculator.Tests/SnapshotRoundTripTests.cs`

1 `[TestClass]`(es), 17 test method(s).

| # | Test method | Attributes | Source lines |
|---|---|---|---|
| 1 | `InvalidSnapshotModeDoesNotReplaceCurrentMode` | `[TestMethod]` | `SnapshotRoundTripTests.cs:38-49` |
| 2 | `NonCalculatorSnapshotIgnoresStaleCalculatorState` | `[TestMethod]` | `SnapshotRoundTripTests.cs:51-66` |
| 3 | `NonCalculatorSnapshotDoesNotCaptureStaleCalculatorState` | `[TestMethod]` | `SnapshotRoundTripTests.cs:68-81` |
| 4 | `SnapshotRestoresHistory` | `[TestMethod]` | `SnapshotRoundTripTests.cs:83-118` |
| 5 | `CapturedHistoryCarriesItsTokensAndCommands` | `[TestMethod]` | `SnapshotRoundTripTests.cs:120-130` |
| 6 | `ExpressionCommandWrapperCarriesTheOperandFlags` | `[TestMethod]` | `SnapshotRoundTripTests.cs:132-148` |
| 7 | `HistoryItemWrapperIsConstructibleFromManagedData` | `[TestMethod]` | `SnapshotRoundTripTests.cs:150-165` |
| 8 | `RestoringASnapshotClearsMemory` | `[TestMethod]` | `SnapshotRoundTripTests.cs:168-183` |
| 9 | `RestoringAMalformedHistoryCommandThrowsInsteadOfCrashing` | `[TestMethod]` | `SnapshotRoundTripTests.cs:185-205` |
| 10 | `FailedRestoreResetsTheBoundCalculator` | `[TestMethod]` | `SnapshotRoundTripTests.cs:207-240` |
| 11 | `FailedCrossModeRestoreClearsSnapshotModeHistory` | `[TestMethod]` | `SnapshotRoundTripTests.cs:242-263` |
| 12 | `FailedScientificRestoreKeepsTheScientificEngine` | `[TestMethod]` | `SnapshotRoundTripTests.cs:265-279` |
| 13 | `FailedScientificRestoreResetsErroredEngineAngleMode` | `[TestMethod]` | `SnapshotRoundTripTests.cs:281-295` |
| 14 | `FailedProgrammerRestoreKeepsTheProgrammerEngine` | `[TestMethod]` | `SnapshotRoundTripTests.cs:297-311` |
| 15 | `FailedProgrammerRestoreResetsBitLength` | `[TestMethod]` | `SnapshotRoundTripTests.cs:313-328` |
| 16 | `SuccessfulScientificRestoreKeepsTheScientificEngine` | `[TestMethod]` | `SnapshotRoundTripTests.cs:330-346` |
| 17 | `SuccessfulProgrammerRestoreKeepsTheProgrammerEngine` | `[TestMethod]` | `SnapshotRoundTripTests.cs:348-364` |

#### 4.12.0 File preamble — `using` directives, file-scoped structs/helpers, namespace open (`SnapshotRoundTripTests.cs:1-17`)

```csharp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

using Microsoft.VisualStudio.TestTools.UnitTesting;

using CalcManager.Interop;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;
using CalculatorApp.ViewModel.Snapshot;

namespace Calculator.Tests
{
```

#### 4.12.1 `[TestClass] SnapshotRoundTripTests` — verbatim (`SnapshotRoundTripTests.cs:18-444`)

```csharp
    [TestClass]
    public class SnapshotRoundTripTests
    {
        private const int Command0 = 130;
        private const int Command1 = 131;
        private const int Command2 = 132;
        private const int Command3 = 133;
        private const int Command5 = 135;
        private const int Command9 = 139;
        private const int CommandCLEAR = 81;
        private const int CommandPNT = 84;
        private const int CommandDIV = 91;
        private const int CommandADD = 93;
        private const int CommandMUL = 92;
        private const int CommandSIN = 102;
        private const int CommandEQU = 121;
        private const int ModeBasic = 200;
        private const int ModeScientific = 201;
        private const int ModeProgrammer = 209;

        [TestMethod]
        public void InvalidSnapshotModeDoesNotReplaceCurrentMode()
        {
            var viewModel = new ApplicationViewModel();
            viewModel.Initialize(ViewMode.Standard);
            var snapshot = new ApplicationSnapshot { Mode = int.MaxValue };

            Assert.ThrowsException<ArgumentOutOfRangeException>(
                () => viewModel.RestoreFromSnapshot(snapshot));

            Assert.AreEqual(ViewMode.Standard, viewModel.Mode);
        }

        [TestMethod]
        public void NonCalculatorSnapshotIgnoresStaleCalculatorState()
        {
            var viewModel = new ApplicationViewModel();
            SetModeWithoutNavigation(viewModel, ViewMode.Date);
            var snapshot = new ApplicationSnapshot
            {
                Mode = (int)ViewMode.Date,
                StandardCalculator = CreateStandardViewModel().Snapshot
            };

            viewModel.RestoreFromSnapshot(snapshot);

            Assert.AreEqual(ViewMode.Date, viewModel.Mode);
            Assert.IsNull(viewModel.CalculatorViewModel);
        }

        [TestMethod]
        public void NonCalculatorSnapshotDoesNotCaptureStaleCalculatorState()
        {
            var viewModel = new ApplicationViewModel
            {
                CalculatorViewModel = CreateStandardViewModel()
            };
            SetModeWithoutNavigation(viewModel, ViewMode.Date);

            var snapshot = viewModel.Snapshot;

            Assert.AreEqual((int)ViewMode.Date, snapshot.Mode);
            Assert.IsNull(snapshot.StandardCalculator);
        }

        [TestMethod]
        public void SnapshotRestoresHistory()
        {
            StandardCalculatorViewModel source = CreateStandardViewModel();
            Evaluate(source, Command1, CommandADD, Command2);
            Evaluate(source, Command3, CommandMUL, Command3);

            var captured = source.Snapshot;

            Assert.IsNotNull(
                captured.CalcManager.HistoryItems,
                "No history was captured, so resuming from Recall would come back with an empty history list.");
            Assert.AreEqual(2, captured.CalcManager.HistoryItems.Count);

            StandardCalculatorViewModel restored = CreateStandardViewModel();
            restored.Snapshot = captured;

            var recaptured = restored.Snapshot;
            Assert.IsNotNull(recaptured.CalcManager.HistoryItems, "The restored calculator captured no history.");
            CollectionAssert.AreEqual(
                captured.CalcManager.HistoryItems.Select(item => item.Expression).ToArray(),
                recaptured.CalcManager.HistoryItems.Select(item => item.Expression).ToArray(),
                "Expressions did not survive the round trip.");
            CollectionAssert.AreEqual(
                captured.CalcManager.HistoryItems.Select(item => item.Result).ToArray(),
                recaptured.CalcManager.HistoryItems.Select(item => item.Result).ToArray(),
                "Results did not survive the round trip.");

            // Display history is newest-first while the snapshot is oldest-first.
            var items = restored.HistoryVM.Items;
            Assert.AreEqual(2, items.Count, "The restored calculator did not get its history back.");
            CollectionAssert.AreEqual(
                captured.CalcManager.HistoryItems.Select(item => item.Result).Reverse().ToArray(),
                items.Select(item => item.Result).ToArray(),
                "The restored history list does not show what was captured.");
        }

        [TestMethod]
        public void CapturedHistoryCarriesItsTokensAndCommands()
        {
            StandardCalculatorViewModel source = CreateStandardViewModel();
            Evaluate(source, Command1, CommandADD, Command2);

            var item = source.Snapshot.CalcManager.HistoryItems.Single();

            Assert.IsTrue(item.Tokens.Count > 0, "The captured history item carried no tokens.");
            Assert.IsTrue(item.Commands.Count > 0, "The captured history item carried no commands.");
        }

        [TestMethod]
        public void ExpressionCommandWrapperCarriesTheOperandFlags()
        {
            var command = new ExpressionCommandWrapper(
                CommandType.OperandCommand,
                0,
                new[] { Command1, Command2 },
                isNegative: true,
                isDecimalPresent: true,
                isSciFmt: true);

            Assert.AreEqual(CommandType.OperandCommand, command.Type);
            CollectionAssert.AreEqual(new[] { Command1, Command2 }, command.Commands);
            Assert.IsTrue(command.IsNegative, "IsNegative did not survive.");
            Assert.IsTrue(command.IsDecimalPresent, "IsDecimalPresent did not survive.");
            Assert.IsTrue(command.IsSciFmt, "IsSciFmt did not survive.");
        }

        [TestMethod]
        public void HistoryItemWrapperIsConstructibleFromManagedData()
        {
            var token = new HistoryToken { Value = "1", CommandIndex = 0 };
            var command = new ExpressionCommandWrapper(
                CommandType.BinaryCommand, CommandADD, System.Array.Empty<int>(), false, false, false);

            var item = new HistoryItemWrapper(new[] { token }, new[] { command }, "1 + 2 =", "3");

            Assert.AreEqual("1 + 2 =", item.Expression);
            Assert.AreEqual("3", item.Result);
            Assert.AreEqual(1, item.Tokens.Length);
            Assert.AreEqual("1", item.Tokens[0].Value);
            Assert.AreEqual(1, item.Commands.Length);
            Assert.AreEqual(CommandADD, item.Commands[0].Command);
        }

        // A recalled session must not inherit memory from the current one.
        [TestMethod]
        public void RestoringASnapshotClearsMemory()
        {
            StandardCalculatorViewModel source = CreateStandardViewModel();
            Evaluate(source, Command1, CommandADD, Command2);
            var captured = source.Snapshot;

            StandardCalculatorViewModel restored = CreateStandardViewModel();
            Evaluate(restored, Command3);
            restored.OnMemoryButtonPressed();
            Assert.IsTrue(restored.IsMemoryEmpty == false, "Memory was not set up, so the test proves nothing.");

            restored.Snapshot = captured;

            Assert.IsTrue(restored.IsMemoryEmpty, "Restoring a snapshot left the previous session's memory behind.");
        }

        [TestMethod]
        public void RestoringAMalformedHistoryCommandThrowsInsteadOfCrashing()
        {
            var malformed = new ExpressionCommandWrapper(
                CommandType.UnaryCommand, 0, System.Array.Empty<int>(), false, false, false);
            var item = new CalculatorApp.ViewModel.Snapshot.CalcManagerHistoryItem
            {
                Expression = "1 + 2 =",
                Result = "3"
            };
            item.Commands.Add(malformed);

            var snapshot = CreateStandardViewModel().Snapshot;
            snapshot.CalcManager.HistoryItems = new List<CalculatorApp.ViewModel.Snapshot.CalcManagerHistoryItem> { item };

            StandardCalculatorViewModel target = CreateStandardViewModel();

            Assert.ThrowsException<ArgumentException>(
                () => target.Snapshot = snapshot,
                "A malformed unary command has to be rejected with a catchable exception.");
        }

        [TestMethod]
        public void FailedRestoreResetsTheBoundCalculator()
        {
            var viewModel = new ApplicationViewModel();
            viewModel.Initialize(ViewMode.Standard);
            var originalCalculator = CreateStandardViewModel();
            viewModel.CalculatorViewModel = originalCalculator;
            Evaluate(originalCalculator, Command3);
            originalCalculator.OnMemoryButtonPressed();

            StandardCalculatorViewModel source = CreateStandardViewModel();
            Evaluate(source, Command1, CommandADD, Command2);
            var standardSnapshot = source.Snapshot;
            var malformed = new CalculatorApp.ViewModel.Snapshot.CalcManagerHistoryItem();
            malformed.Commands.Add(new ExpressionCommandWrapper(
                CommandType.UnaryCommand, 0, System.Array.Empty<int>(), false, false, false));
            standardSnapshot.CalcManager.HistoryItems.Add(malformed);
            var snapshot = new ApplicationSnapshot
            {
                Mode = (int)ViewMode.Standard,
                StandardCalculator = standardSnapshot
            };

            Assert.ThrowsException<ArgumentException>(
                () => viewModel.RestoreFromSnapshot(snapshot));

            Assert.AreEqual(ViewMode.Standard, viewModel.Mode);
            Assert.AreSame(originalCalculator, viewModel.CalculatorViewModel);
            Assert.AreEqual("0", viewModel.CalculatorViewModel.DisplayValue);
            Assert.IsTrue(viewModel.CalculatorViewModel.IsMemoryEmpty);
            Assert.AreEqual(0, viewModel.CalculatorViewModel.HistoryVM.Items.Count);
            Assert.IsNull(viewModel.CalculatorViewModel.Snapshot.ExpressionDisplay);
            Assert.AreEqual(0, viewModel.CalculatorViewModel.Snapshot.DisplayCommands.Count);
        }

        [TestMethod]
        public void FailedCrossModeRestoreClearsSnapshotModeHistory()
        {
            StandardCalculatorViewModel calculator = CreateCalculatorViewModel(ViewMode.Standard);
            var viewModel = CreateApplicationViewModel(ViewMode.Standard, calculator);
            StandardCalculatorViewModel source = CreateCalculatorViewModel(ViewMode.Scientific);
            Evaluate(source, Command1, CommandADD, Command2);
            var standardSnapshot = source.Snapshot;
            standardSnapshot.DisplayCommands = new List<ExpressionCommandWrapper> { null };
            var snapshot = new ApplicationSnapshot
            {
                Mode = (int)ViewMode.Scientific,
                StandardCalculator = standardSnapshot
            };

            Assert.ThrowsException<NullReferenceException>(
                () => viewModel.RestoreFromSnapshot(snapshot));

            calculator.SendCommandToCalcManager(ModeScientific);
            calculator.HistoryVM.ReloadHistory(ViewMode.Scientific);
            Assert.AreEqual(0, calculator.HistoryVM.Items.Count);
        }

        [TestMethod]
        public void FailedScientificRestoreKeepsTheScientificEngine()
        {
            StandardCalculatorViewModel calculator = CreateCalculatorViewModel(ViewMode.Scientific);
            AssertScientificOrderOfOperations(calculator);
            calculator.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Radians);
            Assert.AreEqual(NumbersAndOperatorsEnum.Radians, calculator.GetCurrentAngleType());
            var viewModel = CreateApplicationViewModel(ViewMode.Scientific, calculator);

            Assert.ThrowsException<ArgumentException>(
                () => viewModel.RestoreFromSnapshot(CreateMalformedSnapshot(ViewMode.Scientific)));

            Assert.AreEqual(NumbersAndOperatorsEnum.Degree, calculator.GetCurrentAngleType());
            AssertScientificOrderOfOperations(calculator);
        }

        [TestMethod]
        public void FailedScientificRestoreResetsErroredEngineAngleMode()
        {
            StandardCalculatorViewModel calculator = CreateCalculatorViewModel(ViewMode.Scientific);
            calculator.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Radians);
            Evaluate(calculator, Command1, CommandDIV, Command0);
            Assert.IsTrue(calculator.IsInError);
            var viewModel = CreateApplicationViewModel(ViewMode.Scientific, calculator);

            Assert.ThrowsException<ArgumentException>(
                () => viewModel.RestoreFromSnapshot(CreateMalformedSnapshot(ViewMode.Scientific)));

            Evaluate(calculator, Command9, Command0, CommandSIN);
            Assert.AreEqual("1", calculator.DisplayValue);
        }

        [TestMethod]
        public void FailedProgrammerRestoreKeepsTheProgrammerEngine()
        {
            StandardCalculatorViewModel calculator = CreateCalculatorViewModel(ViewMode.Programmer);
            AssertProgrammerIgnoresDecimalPoint(calculator);
            calculator.SwitchProgrammerModeBase(NumberBase.HexBase);
            Assert.AreEqual(NumberBase.HexBase, calculator.CurrentRadixType);
            var viewModel = CreateApplicationViewModel(ViewMode.Programmer, calculator);

            Assert.ThrowsException<ArgumentException>(
                () => viewModel.RestoreFromSnapshot(CreateMalformedSnapshot(ViewMode.Programmer)));

            Assert.AreEqual(NumberBase.DecBase, calculator.CurrentRadixType);
            AssertProgrammerIgnoresDecimalPoint(calculator);
        }

        [TestMethod]
        public void FailedProgrammerRestoreResetsBitLength()
        {
            StandardCalculatorViewModel calculator = CreateCalculatorViewModel(ViewMode.Programmer);
            calculator.ValueBitLength = BitLength.BitLengthByte;
            Evaluate(calculator, Command1, CommandDIV, Command0);
            Assert.IsTrue(calculator.IsInError);
            var viewModel = CreateApplicationViewModel(ViewMode.Programmer, calculator);

            Assert.ThrowsException<ArgumentException>(
                () => viewModel.RestoreFromSnapshot(CreateMalformedSnapshot(ViewMode.Programmer)));

            Assert.AreEqual(BitLength.BitLengthQWord, calculator.ValueBitLength);
            Evaluate(calculator, Command2, Command5, Command5, CommandADD, Command1);
            Assert.AreEqual("256", calculator.DisplayValue);
        }

        [TestMethod]
        public void SuccessfulScientificRestoreKeepsTheScientificEngine()
        {
            StandardCalculatorViewModel calculator = CreateCalculatorViewModel(ViewMode.Scientific);
            calculator.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Radians);
            Assert.AreEqual(NumbersAndOperatorsEnum.Radians, calculator.GetCurrentAngleType());
            var viewModel = CreateApplicationViewModel(ViewMode.Scientific, calculator);

            viewModel.RestoreFromSnapshot(new ApplicationSnapshot
            {
                Mode = (int)ViewMode.Scientific,
                StandardCalculator = CreateCalculatorViewModel(ViewMode.Scientific).Snapshot
            });

            Assert.AreEqual(NumbersAndOperatorsEnum.Degree, calculator.GetCurrentAngleType());
            AssertScientificOrderOfOperations(calculator);
        }

        [TestMethod]
        public void SuccessfulProgrammerRestoreKeepsTheProgrammerEngine()
        {
            StandardCalculatorViewModel calculator = CreateCalculatorViewModel(ViewMode.Programmer);
            calculator.SwitchProgrammerModeBase(NumberBase.HexBase);
            Assert.AreEqual(NumberBase.HexBase, calculator.CurrentRadixType);
            var viewModel = CreateApplicationViewModel(ViewMode.Programmer, calculator);

            viewModel.RestoreFromSnapshot(new ApplicationSnapshot
            {
                Mode = (int)ViewMode.Programmer,
                StandardCalculator = CreateCalculatorViewModel(ViewMode.Programmer).Snapshot
            });

            Assert.AreEqual(NumberBase.DecBase, calculator.CurrentRadixType);
            AssertProgrammerIgnoresDecimalPoint(calculator);
        }

        private static StandardCalculatorViewModel CreateStandardViewModel()
        {
            var viewModel = new StandardCalculatorViewModel();
            viewModel.IsStandard = true;
            viewModel.SendCommandToCalcManager(ModeBasic);
            viewModel.HistoryVM.ClearCommand.Execute(null);
            return viewModel;
        }

        private static StandardCalculatorViewModel CreateCalculatorViewModel(ViewMode mode)
        {
            StandardCalculatorViewModel viewModel = CreateStandardViewModel();
            viewModel.SetCalculatorType(mode);
            viewModel.SendCommandToCalcManager(
                mode == ViewMode.Scientific ? ModeScientific : ModeProgrammer);
            return viewModel;
        }

        private static ApplicationViewModel CreateApplicationViewModel(
            ViewMode mode,
            StandardCalculatorViewModel calculator)
        {
            var viewModel = new ApplicationViewModel
            {
                CalculatorViewModel = calculator
            };
            SetModeWithoutNavigation(viewModel, mode);
            return viewModel;
        }

        private static ApplicationSnapshot CreateMalformedSnapshot(ViewMode mode)
        {
            StandardCalculatorViewModel source = CreateStandardViewModel();
            Evaluate(source, Command1, CommandADD, Command2);
            var snapshot = source.Snapshot;
            var malformed = new CalculatorApp.ViewModel.Snapshot.CalcManagerHistoryItem();
            malformed.Commands.Add(new ExpressionCommandWrapper(
                CommandType.UnaryCommand, 0, System.Array.Empty<int>(), false, false, false));
            snapshot.CalcManager.HistoryItems.Add(malformed);
            return new ApplicationSnapshot
            {
                Mode = (int)mode,
                StandardCalculator = snapshot
            };
        }

        private static void AssertScientificOrderOfOperations(StandardCalculatorViewModel viewModel)
        {
            viewModel.SendCommandToCalcManager(CommandCLEAR);
            Evaluate(viewModel, Command1, CommandADD, Command2, CommandMUL, Command3);
            Assert.AreEqual("7", viewModel.DisplayValue);
        }

        private static void AssertProgrammerIgnoresDecimalPoint(StandardCalculatorViewModel viewModel)
        {
            viewModel.SendCommandToCalcManager(CommandCLEAR);
            viewModel.SendCommandToCalcManager(Command1);
            viewModel.SendCommandToCalcManager(CommandPNT);
            viewModel.SendCommandToCalcManager(Command5);
            Assert.AreEqual("15", viewModel.DisplayValue);
        }

        private static void SetModeWithoutNavigation(ApplicationViewModel viewModel, ViewMode mode)
        {
            typeof(ApplicationViewModel)
                .GetField("_mode", BindingFlags.Instance | BindingFlags.NonPublic)
                .SetValue(viewModel, mode);
        }

        private static void Evaluate(StandardCalculatorViewModel viewModel, params int[] commands)
        {
            foreach (int command in commands)
            {
                viewModel.SendCommandToCalcManager(command);
            }

            viewModel.SendCommandToCalcManager(CommandEQU);
        }
    }
```

#### 4.12.Z Remaining non-blank source in this file

<!-- SnapshotRoundTripTests.cs:445-445 -->
```csharp
}
```

### 4.13 `src/Calculator.Tests/StandardCalculatorViewModelTests.cs`

1 `[TestClass]`(es), 50 test method(s).

| # | Test method | Attributes | Source lines |
|---|---|---|---|
| 1 | `ViewModelConstructorDisplayValueAndExpressionInitializedTest` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:159-166` |
| 2 | `ViewModelConstructorButtonPressedInitializedTest` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:168-174` |
| 3 | `ButtonPressedLeftHandOperandEnteredTest` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:181-192` |
| 4 | `ButtonPressedLeftHandOperandAndDecimalEnteredTest` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:195-206` |
| 5 | `ButtonPressedLeftHandOperandAndEqualsEnteredTest` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:209-221` |
| 6 | `ButtonPressedLeftHandOperandAndOperationEnteredTest` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:224-235` |
| 7 | `ButtonPressedRightHandOperandEnteredTest` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:238-252` |
| 8 | `ButtonPressedAdditionWithEqualsTest` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:255-267` |
| 9 | `ButtonPressedSubtractionWithEqualsTest` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:270-282` |
| 10 | `ButtonPressedMultiplyWithEqualsTest` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:285-297` |
| 11 | `ButtonPressedDivideTest` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:300-312` |
| 12 | `ButtonPressedDecimalOperationTest` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:315-331` |
| 13 | `ButtonPressedDivideByZeroNegativeTest` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:334-346` |
| 14 | `ButtonPressedExpressionWithMultipleOperatorsTest` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:349-361` |
| 15 | `ButtonPressedExpressionWithMultipleOperatorsInSuccessionTest` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:364-378` |
| 16 | `ButtonPressedExpressionWithMultipleEqualsAfterEvaluateTest` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:381-394` |
| 17 | `ButtonPressedExpressionWithBackSpaceTest` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:397-410` |
| 18 | `ButtonPressedExpressionWithClearTest` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:413-427` |
| 19 | `VerifyCorrectCharacterMapping` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:434-457` |
| 20 | `PasteExpressions` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:460-512` |
| 21 | `CalculationResultAutomationNameVerification` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:518-556` |
| 22 | `ButtonPressedCalculatorModeSwitch` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:562-592` |
| 23 | `ProgrammerModeAutoConvertedValue` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:598-623` |
| 24 | `ProgrammerModeButtonsDisable` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:625-643` |
| 25 | `ProgrammerModeRadixGrouping` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:645-689` |
| 26 | `ProgrammerModeNot` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:691-714` |
| 27 | `ProgrammerModeAndOr` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:716-742` |
| 28 | `ProgrammerModeClear` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:744-770` |
| 29 | `ButtonPressedUnaryOperatorTest` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:776-825` |
| 30 | `IsMemoryEmptyTest` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:831-841` |
| 31 | `IsOperatorCommandTest` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:843-876` |
| 32 | `OnMemoryButtonPressed` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:878-887` |
| 33 | `OnMemoryAddWhenMemoryEmpty` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:889-905` |
| 34 | `OnMemorySubtractWhenMemoryEmpty` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:907-923` |
| 35 | `OnNegativeEntryInMemory` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:925-953` |
| 36 | `OnDecimalEntryInMemory` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:955-984` |
| 37 | `OnNegativeDecimalInMemory` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:986-1005` |
| 38 | `OnDecimalAddedToMemory` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:1007-1038` |
| 39 | `OnMemorySavedInHexRadixAndSwitchedToStandardMode` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:1040-1059` |
| 40 | `OnMemorySavedInHexRadixAndRadixChanges` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:1061-1086` |
| 41 | `OnMemoryButtonPressedMaxTimes` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:1088-1099` |
| 42 | `OnMemoryItemPressed` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:1101-1128` |
| 43 | `OnMemoryItemPressedNoMemory` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:1130-1143` |
| 44 | `OnMemoryItemAddAndSubtract` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:1145-1174` |
| 45 | `VerifyRawFormatting` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:1180-1198` |
| 46 | `VerifyAnnouncementAfterBinaryOperatorReceived` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:1204-1218` |
| 47 | `VerifyAnnouncementAfterMultipleBinaryOperatorsReceived` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:1220-1258` |
| 48 | `VerifyAnnouncementOmitsFeedbackWhenButtonSuppliesNone` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:1260-1269` |
| 49 | `VerifyAnnouncementUsesLocalizedFeedbackNotTheCommandName` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:1271-1279` |
| 50 | `CalculatorButtonPressedEventArgsAcceptsSupportedShapesAndRejectsOthers` | `[TestMethod]` | `StandardCalculatorViewModelTests.cs:1281-1304` |

#### 4.13.0 File preamble — `using` directives, file-scoped structs/helpers, namespace open (`StandardCalculatorViewModelTests.cs:1-27`)

```csharp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

namespace Calculator.Tests
{
    // A test command with its expected display and expression results.
    internal struct TestItem
    {
        public NumbersAndOperatorsEnum Command;
        public string ExpectedPrimaryDisplay;
        public string ExpectedExpression;

        public TestItem(NumbersAndOperatorsEnum command, string expectedPrimaryDisplay, string expectedExpression)
        {
            Command = command;
            ExpectedPrimaryDisplay = expectedPrimaryDisplay;
            ExpectedExpression = expectedExpression;
        }
    }
```

#### 4.13.1 `[TestClass] StandardCalculatorViewModelTests` — verbatim (`StandardCalculatorViewModelTests.cs:28-1307`)

```csharp
    [TestClass]
    public class StandardCalculatorViewModelTests
    {
        private StandardCalculatorViewModel _viewModel;
        private string _decimalSeparator;

        [TestInitialize]
        public void InitializeViewModel()
        {
            _viewModel = new StandardCalculatorViewModel();
            _viewModel.IsStandard = true;
            var engineResourceProvider = new EngineResourceProvider();
            _decimalSeparator = engineResourceProvider.GetCEngineString("sDecimal");
        }

        #region Helper Methods

        private static void CompareVector(IList<bool> vecA, IList<bool> vecB)
        {
            Assert.AreEqual(vecA.Count, vecB.Count, "Vector sizes differ");
            for (int i = 0; i < vecA.Count; i++)
            {
                Assert.AreEqual(vecA[i], vecB[i], $"Mismatch at index {i}");
            }
        }

        private static void ChangeMode(StandardCalculatorViewModel viewModel, int mode)
        {
            if (mode == 0)
            {
                viewModel.IsStandard = true;
                viewModel.IsScientific = false;
                viewModel.IsProgrammer = false;
                viewModel.SetCalculatorType(ViewMode.Standard);
            }
            else if (mode == 1)
            {
                viewModel.IsScientific = true;
                viewModel.IsProgrammer = false;
                viewModel.IsStandard = false;
                viewModel.SetCalculatorType(ViewMode.Scientific);
            }
            else if (mode == 2)
            {
                viewModel.IsProgrammer = true;
                viewModel.IsScientific = false;
                viewModel.IsStandard = false;
                viewModel.SetCalculatorType(ViewMode.Programmer);
            }
        }

        private static object CommandParameterFromTestItem(TestItem item)
        {
            // Binary operators need to be wrapped with their auditory feedback
            var binOps = new (NumbersAndOperatorsEnum op, string feedback)[]
            {
                (NumbersAndOperatorsEnum.Add, "plus"),
                (NumbersAndOperatorsEnum.Subtract, "minus"),
                (NumbersAndOperatorsEnum.Multiply, "times"),
                (NumbersAndOperatorsEnum.Divide, "divided by")
            };

            foreach (var binOp in binOps)
            {
                if (item.Command == binOp.op)
                {
                    return new CalculatorButtonPressedEventArgs(binOp.feedback, binOp.op);
                }
            }

            return item.Command;
        }

        private static void ValidateViewModelByCommands(StandardCalculatorViewModel viewModel, TestItem[] items, bool doReset = false)
        {
            if (doReset)
            {
                viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Clear);
                viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.ClearEntry);
                viewModel.ClearMemoryCommand.Execute(null);
            }

            foreach (var item in items)
            {
                if (item.Command == NumbersAndOperatorsEnum.None)
                    break;

                object commandParam = CommandParameterFromTestItem(item);
                viewModel.ButtonPressed.Execute(commandParam);

                if (item.ExpectedPrimaryDisplay != "N/A")
                {
                    Assert.AreEqual(item.ExpectedPrimaryDisplay, viewModel.DisplayValue);
                }
            }
        }

        private void ValidateViewModelValue(string value)
        {
            if (value != null)
            {
                Assert.AreEqual(value, _viewModel.DisplayValue);
            }
        }

        private void ValidateViewModelValueAndSecondaryExpression(string value, string expression = null)
        {
            int nTokens = _viewModel.ExpressionTokens.Count;
            string displaySecondaryExpression = string.Empty;

            for (int i = 0; i < nTokens; i++)
            {
                var currentToken = _viewModel.ExpressionTokens[i];
                displaySecondaryExpression += currentToken.Token;
            }

            if (value != null)
            {
                Assert.AreEqual(value, _viewModel.DisplayValue);
            }

            if (expression != null)
            {
                Assert.AreEqual(expression, displaySecondaryExpression);
            }
        }

        #endregion

        #region Constructor Tests

        [TestMethod]
        public void ViewModelConstructorDisplayValueAndExpressionInitializedTest()
        {
            var vm = new StandardCalculatorViewModel();
            vm.IsStandard = true;
            Assert.AreEqual("0", vm.DisplayValue);
            Assert.AreEqual("Display is 0", vm.CalculationResultAutomationName);
        }

        [TestMethod]
        public void ViewModelConstructorButtonPressedInitializedTest()
        {
            var vm = new StandardCalculatorViewModel();
            vm.IsStandard = true;
            Assert.IsNotNull(vm.ButtonPressed);
        }

        #endregion

        #region Basic Arithmetic Tests

        // Expression: 135
        [TestMethod]
        public void ButtonPressedLeftHandOperandEnteredTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Three, "13", ""),
                new(NumbersAndOperatorsEnum.Five, "135", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        // Expression: 13.
        [TestMethod]
        public void ButtonPressedLeftHandOperandAndDecimalEnteredTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Three, "13", ""),
                new(NumbersAndOperatorsEnum.Decimal, "13" + _decimalSeparator, ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        // Expression: 13==
        [TestMethod]
        public void ButtonPressedLeftHandOperandAndEqualsEnteredTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Three, "13", ""),
                new(NumbersAndOperatorsEnum.Equals, "13", ""),
                new(NumbersAndOperatorsEnum.Equals, "13", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        // Expression: 13+
        [TestMethod]
        public void ButtonPressedLeftHandOperandAndOperationEnteredTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Three, "13", ""),
                new(NumbersAndOperatorsEnum.Add, "13", "13 + "),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        // Expression: 13+801
        [TestMethod]
        public void ButtonPressedRightHandOperandEnteredTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Three, "13", ""),
                new(NumbersAndOperatorsEnum.Add, "13", "13 + "),
                new(NumbersAndOperatorsEnum.Eight, "8", "13 + "),
                new(NumbersAndOperatorsEnum.Zero, "80", "13 + "),
                new(NumbersAndOperatorsEnum.One, "801", "13 + "),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        // Expression: 1+2=
        [TestMethod]
        public void ButtonPressedAdditionWithEqualsTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Equals, "3", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        // Expression: 1-2=
        [TestMethod]
        public void ButtonPressedSubtractionWithEqualsTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Subtract, "1", "1 - "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 - "),
                new(NumbersAndOperatorsEnum.Equals, "-1", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        // Expression: 3*5=
        [TestMethod]
        public void ButtonPressedMultiplyWithEqualsTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Three, "3", ""),
                new(NumbersAndOperatorsEnum.Multiply, "3", "3 * "),
                new(NumbersAndOperatorsEnum.Five, "5", "3 * "),
                new(NumbersAndOperatorsEnum.Equals, "15", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        // Expression: 9/3=
        [TestMethod]
        public void ButtonPressedDivideTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Nine, "9", ""),
                new(NumbersAndOperatorsEnum.Divide, "9", "9 / "),
                new(NumbersAndOperatorsEnum.Three, "3", "9 / "),
                new(NumbersAndOperatorsEnum.Equals, "3", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        // Expression: 7.555*3=
        [TestMethod]
        public void ButtonPressedDecimalOperationTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Seven, "7", ""),
                new(NumbersAndOperatorsEnum.Decimal, "7" + _decimalSeparator, ""),
                new(NumbersAndOperatorsEnum.Five, "7" + _decimalSeparator + "5", ""),
                new(NumbersAndOperatorsEnum.Five, "7" + _decimalSeparator + "55", ""),
                new(NumbersAndOperatorsEnum.Five, "7" + _decimalSeparator + "555", ""),
                new(NumbersAndOperatorsEnum.Multiply, "7" + _decimalSeparator + "555", "7" + _decimalSeparator + "555 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "7" + _decimalSeparator + "555 * "),
                new(NumbersAndOperatorsEnum.Equals, "22" + _decimalSeparator + "665", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        // Expression: 7/0
        [TestMethod]
        public void ButtonPressedDivideByZeroNegativeTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Seven, "7", ""),
                new(NumbersAndOperatorsEnum.Divide, "7", "7 / "),
                new(NumbersAndOperatorsEnum.Zero, "0", "7 / "),
                new(NumbersAndOperatorsEnum.Equals, "Cannot divide by zero", "7 / "),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        // Expression: 8/2*
        [TestMethod]
        public void ButtonPressedExpressionWithMultipleOperatorsTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Eight, "8", ""),
                new(NumbersAndOperatorsEnum.Divide, "8", "8 / "),
                new(NumbersAndOperatorsEnum.Two, "2", "8 / "),
                new(NumbersAndOperatorsEnum.Multiply, "4", "8 / 2 * "),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        // Expression: 8/+*2*
        [TestMethod]
        public void ButtonPressedExpressionWithMultipleOperatorsInSuccessionTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Eight, "8", ""),
                new(NumbersAndOperatorsEnum.Divide, "8", "8 / "),
                new(NumbersAndOperatorsEnum.Add, "8", "8 + "),
                new(NumbersAndOperatorsEnum.Multiply, "8", "8 * "),
                new(NumbersAndOperatorsEnum.Two, "2", "8 * "),
                new(NumbersAndOperatorsEnum.Multiply, "16", "8 * 2 * "),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        // Expression: 8*2==
        [TestMethod]
        public void ButtonPressedExpressionWithMultipleEqualsAfterEvaluateTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Eight, "8", ""),
                new(NumbersAndOperatorsEnum.Multiply, "8", "8 * "),
                new(NumbersAndOperatorsEnum.Two, "2", "8 * "),
                new(NumbersAndOperatorsEnum.Equals, "16", ""),
                new(NumbersAndOperatorsEnum.Equals, "32", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        // Expression: 7-6 and Backspace
        [TestMethod]
        public void ButtonPressedExpressionWithBackSpaceTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Seven, "7", ""),
                new(NumbersAndOperatorsEnum.Subtract, "7", "7 - "),
                new(NumbersAndOperatorsEnum.Six, "6", "7 - "),
                new(NumbersAndOperatorsEnum.Backspace, "0", "7 - "),
                new(NumbersAndOperatorsEnum.Backspace, "0", "7 - "),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        // Expression: 91-68 and Clear
        [TestMethod]
        public void ButtonPressedExpressionWithClearTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Nine, "9", ""),
                new(NumbersAndOperatorsEnum.One, "91", ""),
                new(NumbersAndOperatorsEnum.Subtract, "91", "91 - "),
                new(NumbersAndOperatorsEnum.Six, "6", "91 - "),
                new(NumbersAndOperatorsEnum.Eight, "68", "91 - "),
                new(NumbersAndOperatorsEnum.Clear, "0", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        #endregion

        #region Paste Tests

        // Low-level test of character mapping
        [TestMethod]
        public void VerifyCorrectCharacterMapping()
        {
            // Valid numbers
            var n = _viewModel.MapCharacterToButtonId('0').ButtonId;
            Assert.AreEqual(NumbersAndOperatorsEnum.Zero, n);

            n = _viewModel.MapCharacterToButtonId('1').ButtonId;
            Assert.AreEqual(NumbersAndOperatorsEnum.One, n);

            // Valid operators
            n = _viewModel.MapCharacterToButtonId('+').ButtonId;
            Assert.AreEqual(NumbersAndOperatorsEnum.Add, n);

            n = _viewModel.MapCharacterToButtonId('=').ButtonId;
            Assert.AreEqual(NumbersAndOperatorsEnum.Equals, n);

            n = _viewModel.MapCharacterToButtonId('a').ButtonId;
            Assert.AreEqual(NumbersAndOperatorsEnum.A, n);

            // Invalid character
            n = _viewModel.MapCharacterToButtonId('$').ButtonId;
            Assert.AreEqual(NumbersAndOperatorsEnum.None, n);
        }

        // Various strings get pasted
        [TestMethod]
        public void PasteExpressions()
        {
            _viewModel.IsScientific = false;

            _viewModel.OnPaste("-0.99");
            ValidateViewModelValue("-0" + _decimalSeparator + "99");

            _viewModel.OnPaste("1+1=");
            ValidateViewModelValue("2");

            // This result is not obvious: it's the result of the previous operation
            _viewModel.OnPaste("0=");
            ValidateViewModelValue("1");

            // Negative value
            _viewModel.OnPaste("-1");
            ValidateViewModelValue("-1");

            // Negated expression
            _viewModel.OnPaste("-(1+1)");
            ValidateViewModelValueAndSecondaryExpression("-2", "negate(1 + 1)");

            // More complicated Negated expression
            _viewModel.OnPaste("-(-(-1))");
            ValidateViewModelValueAndSecondaryExpression("-1", "negate(0 - (0 - 1))");

            // Switch to scientific mode
            _viewModel.IsScientific = true;

            Assert.IsFalse(_viewModel.IsFToEChecked);

            // Positive exponent
            _viewModel.OnPaste("1.23e+10");
            ValidateViewModelValue("1" + _decimalSeparator + "23e+10");

            _viewModel.OnPaste("1.23e10");
            ValidateViewModelValue("1" + _decimalSeparator + "23e+10");

            _viewModel.OnPaste("135e10");
            ValidateViewModelValue("135" + _decimalSeparator + "e+10");

            // Negative exponent
            _viewModel.OnPaste("1.23e-10");
            ValidateViewModelValue("1" + _decimalSeparator + "23e-10");

            // Uppercase E (for exponent)
            _viewModel.OnPaste("1.23E-10");
            ValidateViewModelValue("1" + _decimalSeparator + "23e-10");

            _viewModel.OnPaste("135E10");
            ValidateViewModelValue("135" + _decimalSeparator + "e+10");
        }

        #endregion

        #region Automation Name Tests

        [TestMethod]
        public void CalculationResultAutomationNameVerification()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsStandardMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Two, "12", ""),
                new(NumbersAndOperatorsEnum.Three, "123", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            Assert.AreEqual("Display is 123", _viewModel.CalculationResultAutomationName);

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Multiply, "2", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Equals, "7", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, true);
            Assert.AreEqual("Display is 7", _viewModel.CalculationResultAutomationName);

            var items3 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Clear, "0", ""),
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.Five, "5", ""),
                new(NumbersAndOperatorsEnum.InvSin, "Invalid input", "asind(5)"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items3, false);
            Assert.AreEqual("Display is Invalid input", _viewModel.CalculationResultAutomationName);
        }

        #endregion

        #region Mode Switch Tests

        [TestMethod]
        public void ButtonPressedCalculatorModeSwitch()
        {
            // Standard mode: 1+2*3 = 9 (left-to-right evaluation)
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsStandardMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Multiply, "3", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Equals, "9", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);

            // Scientific mode: 1+2*3 = 7 (operator precedence)
            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Multiply, "2", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Equals, "7", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, true);
        }

        #endregion

        #region Programmer Mode Tests

        [TestMethod]
        public void ProgrammerModeAutoConvertedValue()
        {
            var none = new TestItem[] { new(NumbersAndOperatorsEnum.None, "", "") };
            ValidateViewModelByCommands(_viewModel, none, true);
            _viewModel.IsProgrammer = true;

            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, false);
            Assert.AreEqual("F", _viewModel.HexDisplayValue);
            Assert.AreEqual("15", _viewModel.DecimalDisplayValue);
            Assert.AreEqual("17", _viewModel.OctalDisplayValue);
            Assert.AreEqual("1111", _viewModel.BinaryDisplayValue);

            var val = new bool[64];
            val[0] = true;
            val[1] = true;
            val[2] = true;
            val[3] = true;
            CompareVector(_viewModel.BinaryDigits, val);
        }

        [TestMethod]
        public void ProgrammerModeButtonsDisable()
        {
            var viewModel = new StandardCalculatorViewModel();
            ChangeMode(viewModel, 2);

            // Hex accepts A-F; the other radices do not, and the keypad reflects that.
            viewModel.SwitchProgrammerModeBase(NumberBase.HexBase);
            Assert.IsTrue(viewModel.AreHEXButtonsEnabled, "Hex digits should be available in hex.");

            viewModel.SwitchProgrammerModeBase(NumberBase.DecBase);
            Assert.IsFalse(viewModel.AreHEXButtonsEnabled, "Hex digits should be unavailable in decimal.");

            viewModel.SwitchProgrammerModeBase(NumberBase.OctBase);
            Assert.IsFalse(viewModel.AreHEXButtonsEnabled, "Hex digits should be unavailable in octal.");

            viewModel.SwitchProgrammerModeBase(NumberBase.BinBase);
            Assert.IsFalse(viewModel.AreHEXButtonsEnabled, "Hex digits should be unavailable in binary.");
        }

        [TestMethod]
        public void ProgrammerModeRadixGrouping()
        {
            var none = new TestItem[] { new(NumbersAndOperatorsEnum.None, "", "") };
            ValidateViewModelByCommands(_viewModel, none, true);
            _viewModel.IsProgrammer = true;

            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Two, "12", ""),
                new(NumbersAndOperatorsEnum.Three, "123", ""),
                new(NumbersAndOperatorsEnum.Four, "1,234", ""),
                new(NumbersAndOperatorsEnum.Five, "12,345", ""),
                new(NumbersAndOperatorsEnum.Six, "123,456", ""),
                new(NumbersAndOperatorsEnum.Seven, "1,234,567", ""),
                new(NumbersAndOperatorsEnum.Eight, "12,345,678", ""),
                new(NumbersAndOperatorsEnum.Nine, "123,456,789", ""),
                new(NumbersAndOperatorsEnum.None, "1", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            Assert.AreEqual("75B CD15", _viewModel.HexDisplayValue);
            Assert.AreEqual("123,456,789", _viewModel.DecimalDisplayValue);
            Assert.AreEqual("726 746 425", _viewModel.OctalDisplayValue);
            Assert.AreEqual("0111 0101 1011 1100 1101 0001 0101", _viewModel.BinaryDisplayValue);

            var val = new bool[64];
            val[0] = true;
            val[2] = true;
            val[4] = true;
            val[8] = true;
            val[10] = true;
            val[11] = true;
            val[14] = true;
            val[15] = true;
            val[16] = true;
            val[17] = true;
            val[19] = true;
            val[20] = true;
            val[22] = true;
            val[24] = true;
            val[25] = true;
            val[26] = true;
            CompareVector(_viewModel.BinaryDigits, val);
        }

        [TestMethod]
        public void ProgrammerModeNot()
        {
            var none = new TestItem[] { new(NumbersAndOperatorsEnum.None, "", "") };
            ValidateViewModelByCommands(_viewModel, none, true);
            _viewModel.IsProgrammer = true;

            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Not, "-2", "~(1)"),
                new(NumbersAndOperatorsEnum.None, "N/A", "N/A")
            };
            ValidateViewModelByCommands(_viewModel, items, false);
            Assert.AreEqual("FFFF FFFF FFFF FFFE", _viewModel.HexDisplayValue);
            Assert.AreEqual("-2", _viewModel.DecimalDisplayValue);
            Assert.AreEqual("1 777 777 777 777 777 777 776", _viewModel.OctalDisplayValue);
            Assert.AreEqual("1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1110", _viewModel.BinaryDisplayValue);
            Assert.AreEqual("-2", _viewModel.DisplayValue);

            var val = Enumerable.Repeat(true, 64).ToArray();
            val[0] = false;
            CompareVector(_viewModel.BinaryDigits, val);
        }

        [TestMethod]
        public void ProgrammerModeAndOr()
        {
            var none = new TestItem[] { new(NumbersAndOperatorsEnum.None, "", "") };
            ValidateViewModelByCommands(_viewModel, none, true);
            _viewModel.IsProgrammer = true;

            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Or, "1", "1 ||"),
                new(NumbersAndOperatorsEnum.Two, "2", "1 ||"),
                new(NumbersAndOperatorsEnum.Equals, "3", ""),
                new(NumbersAndOperatorsEnum.None, "3", "")
            };
            ValidateViewModelByCommands(_viewModel, items, false);

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.And, "1", "1 &"),
                new(NumbersAndOperatorsEnum.Two, "2", "1 &"),
                new(NumbersAndOperatorsEnum.Equals, "0", ""),
                new(NumbersAndOperatorsEnum.None, "0", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, false);
        }

        [TestMethod]
        public void ProgrammerModeClear()
        {
            var none = new TestItem[] { new(NumbersAndOperatorsEnum.None, "", "") };
            ValidateViewModelByCommands(_viewModel, none, true);
            _viewModel.IsProgrammer = true;

            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Or, "1", "1 ||"),
                new(NumbersAndOperatorsEnum.Two, "2", "1 ||"),
                new(NumbersAndOperatorsEnum.ClearEntry, "0", "1 ||"),
                new(NumbersAndOperatorsEnum.None, "", "1 ||")
            };
            ValidateViewModelByCommands(_viewModel, items, false);

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.And, "1", "1 &"),
                new(NumbersAndOperatorsEnum.Two, "2", "1 &"),
                new(NumbersAndOperatorsEnum.Clear, "0", ""),
                new(NumbersAndOperatorsEnum.None, "0", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, false);
        }

        #endregion

        #region Unary Operator Tests

        [TestMethod]
        public void ButtonPressedUnaryOperatorTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsStandardMode, "0", ""),
                new(NumbersAndOperatorsEnum.Five, "5", ""),
                new(NumbersAndOperatorsEnum.Invert, "0" + _decimalSeparator + "2", "reciproc(5)"),
                new(NumbersAndOperatorsEnum.Equals, "0" + _decimalSeparator + "2", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Six, "16", ""),
                new(NumbersAndOperatorsEnum.Sqrt, "4", "sqrt(16)"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, false);

            var items3 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Six, "6", ""),
                new(NumbersAndOperatorsEnum.Negate, "-6", ""),
                new(NumbersAndOperatorsEnum.Nine, "-69", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items3, false);

            var items4 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Clear, "0", ""),
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.Five, "5", ""),
                new(NumbersAndOperatorsEnum.InvSin, "Invalid input", "asind(5)"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items4, false);

            var items5 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Clear, "0", ""),
                new(NumbersAndOperatorsEnum.Four, "4", ""),
                new(NumbersAndOperatorsEnum.Factorial, "24", "fact(4)"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items5, false);
        }

        #endregion

        #region Memory Tests

        [TestMethod]
        public void IsMemoryEmptyTest()
        {
            var viewModel = new StandardCalculatorViewModel();
            viewModel.IsStandard = true;
            Assert.AreEqual(0, viewModel.MemorizedNumbers.Count);
            viewModel.OnMemoryButtonPressed();
            Assert.AreEqual(1, viewModel.MemorizedNumbers.Count);
            viewModel.ClearMemoryCommand.Execute(null);
            Assert.AreEqual(0, viewModel.MemorizedNumbers.Count);
        }

        [TestMethod]
        public void IsOperatorCommandTest()
        {
            var viewModel = new StandardCalculatorViewModel();
            viewModel.IsStandard = true;
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.One);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Two);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Three);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Four);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Five);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Six);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Seven);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Eight);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Nine);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Decimal);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Zero);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Multiply);
            Assert.AreEqual(true, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Add);
            Assert.AreEqual(true, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Zero);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
        }

        [TestMethod]
        public void OnMemoryButtonPressed()
        {
            var viewModel = new StandardCalculatorViewModel();
            viewModel.IsStandard = true;
            viewModel.DisplayValue = "1001";
            viewModel.OnMemoryButtonPressed();
            viewModel.OnMemoryButtonPressed();
            Assert.AreEqual(2, viewModel.MemorizedNumbers.Count);
        }

        [TestMethod]
        public void OnMemoryAddWhenMemoryEmpty()
        {
            _viewModel.IsStandard = true;
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryAdd(0);
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("1,001", _viewModel.DisplayValue);
        }

        [TestMethod]
        public void OnMemorySubtractWhenMemoryEmpty()
        {
            _viewModel.IsStandard = true;
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemorySubtract(0);
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("-1,001", _viewModel.DisplayValue);
        }

        [TestMethod]
        public void OnNegativeEntryInMemory()
        {
            ChangeMode(_viewModel, 0 /*Standard*/);
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.Negate, "N/A", "N/A"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("-1,001", _viewModel.DisplayValue);

            var memorySlotStandard = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("-1,001", memorySlotStandard.Value);

            ChangeMode(_viewModel, 1 /*Scientific*/);
            var memorySlotScientific = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("-1,001", memorySlotScientific.Value);

            ChangeMode(_viewModel, 2 /*Programmer*/);
            var memorySlotProgrammer = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("-1,001", memorySlotProgrammer.Value);
        }

        [TestMethod]
        public void OnDecimalEntryInMemory()
        {
            ChangeMode(_viewModel, 0 /*Standard*/);
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.Decimal, "1,001.", ""),
                new(NumbersAndOperatorsEnum.One, "1,001.1", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("1,001.1", _viewModel.DisplayValue);

            var memorySlotStandard = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("1,001.1", memorySlotStandard.Value);

            ChangeMode(_viewModel, 1 /*Scientific*/);
            var memorySlotScientific = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("1,001.1", memorySlotScientific.Value);

            ChangeMode(_viewModel, 2 /*Programmer*/);
            var memorySlotProgrammer = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("1,001", memorySlotProgrammer.Value);
        }

        [TestMethod]
        public void OnNegativeDecimalInMemory()
        {
            _viewModel.IsStandard = true;
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.Decimal, "1,001.", ""),
                new(NumbersAndOperatorsEnum.One, "1,001.1", ""),
                new(NumbersAndOperatorsEnum.Negate, "N/A", "N/A"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("-1,001.1", _viewModel.DisplayValue);
        }

        [TestMethod]
        public void OnDecimalAddedToMemory()
        {
            _viewModel.IsStandard = true;
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.Decimal, "1,001.", ""),
                new(NumbersAndOperatorsEnum.One, "1,001.1", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.Decimal, "1,001.", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, false);
            _viewModel.OnMemoryButtonPressed();
            _viewModel.OnMemoryAdd(1);
            _viewModel.OnMemoryItemPressed(1);
            Assert.AreEqual("2,002.1", _viewModel.DisplayValue);
        }

        [TestMethod]
        public void OnMemorySavedInHexRadixAndSwitchedToStandardMode()
        {
            ChangeMode(_viewModel, 2 /*Programmer*/);
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.F, "FF", ""),
                new(NumbersAndOperatorsEnum.None, "FF", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();
            ChangeMode(_viewModel, 1 /*Scientific*/);
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("255", _viewModel.DisplayValue);

            var memorySlot = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("255", memorySlot.Value);
        }

        [TestMethod]
        public void OnMemorySavedInHexRadixAndRadixChanges()
        {
            ChangeMode(_viewModel, 2 /*Programmer*/);
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.F, "FF", ""),
                new(NumbersAndOperatorsEnum.None, "FF", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();

            _viewModel.SwitchProgrammerModeBase(NumberBase.OctBase);
            var memorySlotOct = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("377", memorySlotOct.Value);

            _viewModel.SwitchProgrammerModeBase(NumberBase.DecBase);
            var memorySlotDec = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("255", memorySlotDec.Value);

            _viewModel.SwitchProgrammerModeBase(NumberBase.BinBase);
            var memorySlotBin = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("1111 1111", memorySlotBin.Value);
        }

        [TestMethod]
        public void OnMemoryButtonPressedMaxTimes()
        {
            var viewModel = new StandardCalculatorViewModel();
            viewModel.IsStandard = true;
            viewModel.DisplayValue = "1001";
            for (int i = 0; i < 110; i++)
            {
                viewModel.OnMemoryButtonPressed();
            }
            Assert.AreEqual(100, viewModel.MemorizedNumbers.Count);
        }

        [TestMethod]
        public void OnMemoryItemPressed()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Two, "2", ""),
                new(NumbersAndOperatorsEnum.Zero, "20", ""),
                new(NumbersAndOperatorsEnum.Zero, "200", ""),
                new(NumbersAndOperatorsEnum.Two, "2,002", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, false);
            _viewModel.OnMemoryButtonPressed();
            _viewModel.OnMemoryItemPressed(1);

            Assert.AreEqual("1,001", _viewModel.DisplayValue);
        }

        [TestMethod]
        public void OnMemoryItemPressedNoMemory()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Two, "12", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("12", _viewModel.DisplayValue);
            Assert.AreEqual(0, _viewModel.MemorizedNumbers.Count);
        }

        [TestMethod]
        public void OnMemoryItemAddAndSubtract()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Two, "2", ""),
                new(NumbersAndOperatorsEnum.Zero, "20", ""),
                new(NumbersAndOperatorsEnum.Zero, "200", ""),
                new(NumbersAndOperatorsEnum.Two, "2,002", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, false);
            _viewModel.OnMemoryButtonPressed();
            _viewModel.OnMemoryItemPressed(1);
            _viewModel.OnMemoryAdd(0);

            var memorySlot = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("3,003", memorySlot.Value);
        }

        #endregion

        #region Raw Formatting Tests

        [TestMethod]
        public void VerifyRawFormatting()
        {
            // GetRawDisplayValue is private, so we test via the LocalizationSettings
            // approach used internally: remove group separators from display value.
            var settings = LocalizationSettings.GetInstance();

            _viewModel.DisplayValue = "1,001";
            Assert.AreEqual("1001", settings.RemoveGroupSeparators(_viewModel.DisplayValue));

            _viewModel.DisplayValue = "999";
            Assert.AreEqual("999", settings.RemoveGroupSeparators(_viewModel.DisplayValue));

            _viewModel.DisplayValue = "1,001,001";
            Assert.AreEqual("1001001", settings.RemoveGroupSeparators(_viewModel.DisplayValue));

            _viewModel.DisplayValue = "1,001, 001";
            Assert.AreEqual("1001001", settings.RemoveGroupSeparators(_viewModel.DisplayValue));
        }

        #endregion

        #region Announcement Tests

        [TestMethod]
        public void VerifyAnnouncementAfterBinaryOperatorReceived()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.Multiply, "1,001", "1001 x"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            Assert.AreEqual("Display is 1,001 times", _viewModel.Announcement?.Announcement);
        }

        [TestMethod]
        public void VerifyAnnouncementAfterMultipleBinaryOperatorsReceived()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.Multiply, "1,001", "1001 x"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            Assert.AreEqual("Display is 1,001 times", _viewModel.Announcement?.Announcement);

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Divide, "1,001", "1001 \u00f7"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, false);
            Assert.AreEqual("Display is 1,001 divided by", _viewModel.Announcement?.Announcement);

            var items3 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Add, "1,001", "1001 +"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items3, false);
            Assert.AreEqual("Display is 1,001 plus", _viewModel.Announcement?.Announcement);

            var items4 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Subtract, "1,001", "1001 x"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items4, false);
            Assert.AreEqual("Display is 1,001 minus", _viewModel.Announcement?.Announcement);
        }

        [TestMethod]
        public void VerifyAnnouncementOmitsFeedbackWhenButtonSuppliesNone()
        {
            // Only the operator buttons carry an AuditoryFeedback resource. Every other button sends
            // a bare NumbersAndOperatorsEnum, and its announcement must be the display value alone.
            _viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Clear);
            _viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.One);

            Assert.AreEqual("Display is 1", _viewModel.Announcement?.Announcement);
        }

        [TestMethod]
        public void VerifyAnnouncementUsesLocalizedFeedbackNotTheCommandName()
        {
            _viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Clear);
            _viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.One);
            _viewModel.ButtonPressed.Execute(new CalculatorButtonPressedEventArgs("mal", NumbersAndOperatorsEnum.Multiply));

            Assert.AreEqual("Display is 1 mal", _viewModel.Announcement?.Announcement);
        }

        [TestMethod]
        public void CalculatorButtonPressedEventArgsAcceptsSupportedShapesAndRejectsOthers()
        {
            Assert.AreEqual(
                NumbersAndOperatorsEnum.Add,
                CalculatorButtonPressedEventArgs.GetOperationFromCommandParameter(
                    new CalculatorButtonPressedEventArgs("plus", NumbersAndOperatorsEnum.Add)));
            Assert.AreEqual(
                NumbersAndOperatorsEnum.Subtract,
                CalculatorButtonPressedEventArgs.GetOperationFromCommandParameter((int)NumbersAndOperatorsEnum.Subtract));
            Assert.AreEqual(
                NumbersAndOperatorsEnum.Multiply,
                CalculatorButtonPressedEventArgs.GetOperationFromCommandParameter(NumbersAndOperatorsEnum.Multiply));

            try
            {
                CalculatorButtonPressedEventArgs.GetOperationFromCommandParameter(new object());
                Assert.Fail("Unsupported command parameters must be rejected.");
            }
            catch (ArgumentException exception)
            {
                Assert.AreEqual("commandParameter", exception.ParamName);
            }
        }

        #endregion
    }
```

#### 4.13.Z Remaining non-blank source in this file

<!-- StandardCalculatorViewModelTests.cs:1308-1308 -->
```csharp
}
```

### 4.14 `src/Calculator.Tests/UnitConverterViewModelTests.cs`

5 `[TestClass]`(es), 31 test method(s).

| # | Test method | Attributes | Source lines |
|---|---|---|---|
| 1 | `TestGetNameReturnsCorrectName` | `[TestMethod]` | `UnitConverterViewModelTests.cs:19-26` |
| 2 | `TestGetVisibilityReturnsVisible` | `[TestMethod]` | `UnitConverterViewModelTests.cs:28-34` |
| 3 | `TestGetVisibilityReturnsCollapsed` | `[TestMethod]` | `UnitConverterViewModelTests.cs:36-42` |
| 4 | `TestGetNameReturnsCorrectName` | `[TestMethod]` | `UnitConverterViewModelTests.cs:48-55` |
| 5 | `TestGetAbbreviationReturnsCorrectAbbreviation` | `[TestMethod]` | `UnitConverterViewModelTests.cs:57-65` |
| 6 | `TestGetValueReturnsCorrectValue` | `[TestMethod]` | `UnitConverterViewModelTests.cs:71-78` |
| 7 | `TestGetUnitNameReturnsCorrectValue` | `[TestMethod]` | `UnitConverterViewModelTests.cs:80-88` |
| 8 | `TestGetIsWhimsicalReturnsCorrectValue` | `[TestMethod]` | `UnitConverterViewModelTests.cs:90-100` |
| 9 | `AllStaticUnitsProduceFiniteConversionsInBothDirections` | `[TestMethod]` | `UnitConverterViewModelTests.cs:106-138` |
| 10 | `EnteringValueAfterSwitchingActiveUpdatesSecondValue` | `[TestMethod]` | `UnitConverterViewModelTests.cs:165-174` |
| 11 | `MaxDigitsAnnouncementIncludesTheConversionResult` | `[TestMethod]` | `UnitConverterViewModelTests.cs:176-189` |
| 12 | `ConversionResultNarrationSubstitutesAllPlaceholders` | `[TestMethod]` | `UnitConverterViewModelTests.cs:191-205` |
| 13 | `SwitchingActiveValueSwapsTheFromAndToAutomationFormats` | `[TestMethod]` | `UnitConverterViewModelTests.cs:207-226` |
| 14 | `PastingAMinusAfterDigitsDoesNotNegateTheValue` | `[TestMethod]` | `UnitConverterViewModelTests.cs:233-242` |
| 15 | `PastingALeadingMinusNegatesTheValue` | `[TestMethod]` | `UnitConverterViewModelTests.cs:244-253` |
| 16 | `TextWithNoUsableNumberIsRejectedBeforeItReachesTheConverter` | `[TestMethod]` | `UnitConverterViewModelTests.cs:261-276` |
| 17 | `PartialDisplayValuesDoNotThrowDuringFormatting` | `[TestMethod]` | `UnitConverterViewModelTests.cs:278-287` |
| 18 | `RejectedPasteSaysWhyInsteadOfBlankingTheDisplay` | `[TestMethod]` | `UnitConverterViewModelTests.cs:289-302` |
| 19 | `LargeValuesAreDisplayedWithGroupSeparators` | `[TestMethod]` | `UnitConverterViewModelTests.cs:304-325` |
| 20 | `LengthSuggestionsPreserveWhimsicalUnitMetadata` | `[TestMethod]` | `UnitConverterViewModelTests.cs:327-349` |
| 21 | `EnteringDigitAfterCurrencyUnitChangeReplacesValue` | `[TestMethod]` | `UnitConverterViewModelTests.cs:351-368` |
| 22 | `EnteringCurrencyAfterBackgroundLoadUsesLoadedRatios` | `[TestMethod]` | `UnitConverterViewModelTests.cs:370-394` |
| 23 | `CurrencyLoadFinishingInsideCurrencyUsesLoadedRatios` | `[TestMethod]` | `UnitConverterViewModelTests.cs:396-418` |
| 24 | `LeavingCurrencyClearsTheCurrencySymbolsAndRatio` | `[TestMethod]` | `UnitConverterViewModelTests.cs:420-439` |
| 25 | `CurrencyRefreshCompletesAfterInitialLoad` | `[TestMethod]` | `UnitConverterViewModelTests.cs:441-454` |
| 26 | `LocaleDefaultCurrencyMapIsPackaged` | `[TestMethod]` | `UnitConverterViewModelTests.cs:456-467` |
| 27 | `ConverterCommandsKeepTheirIdentityAcrossReads` | `[TestMethod]` | `UnitConverterViewModelTests.cs:469-481` |
| 28 | `OtherViewModelCommandsKeepTheirIdentityAcrossReads` | `[TestMethod]` | `UnitConverterViewModelTests.cs:483-497` |
| 29 | `ConstructionAndCategorySwitchingKeepTheConverterConsistent` | `[TestMethod]` | `UnitConverterViewModelTests.cs:499-527` |
| 30 | `CategorySwitchPublishesNewUnitsBeforeSelectedUnits` | `[TestMethod]` | `UnitConverterViewModelTests.cs:529-571` |
| 31 | `InputFollowsTheActiveValueAndIsFormattedForDisplay` | `[TestMethod]` | `UnitConverterViewModelTests.cs:573-600` |

#### 4.14.0 File preamble — `using` directives, file-scoped structs/helpers, namespace open (`UnitConverterViewModelTests.cs:1-15`)

```csharp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Globalization;
using System.Linq;
using System.Threading.Tasks;

using Microsoft.VisualStudio.TestTools.UnitTesting;
using Windows.UI.Xaml;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

namespace Calculator.Tests
{
```

#### 4.14.1 `[TestClass] CategoryViewModelTests` — verbatim (`UnitConverterViewModelTests.cs:16-43`)

```csharp
    [TestClass]
    public class CategoryViewModelTests
    {
        [TestMethod]
        public void TestGetNameReturnsCorrectName()
        {
            var category = new Category(3, "Length", supportsNegative: false);

            Assert.AreEqual("Length", category.Name);
            Assert.AreEqual(3, category.GetModelCategoryId());
        }

        [TestMethod]
        public void TestGetVisibilityReturnsVisible()
        {
            var category = new Category(7, "Temperature", supportsNegative: true);

            Assert.AreEqual(Visibility.Visible, category.NegateVisibility);
        }

        [TestMethod]
        public void TestGetVisibilityReturnsCollapsed()
        {
            var category = new Category(3, "Length", supportsNegative: false);

            Assert.AreEqual(Visibility.Collapsed, category.NegateVisibility);
        }
    }
```

#### 4.14.2 `[TestClass] UnitViewModelTests` — verbatim (`UnitConverterViewModelTests.cs:45-66`)

```csharp
    [TestClass]
    public class UnitViewModelTests
    {
        [TestMethod]
        public void TestGetNameReturnsCorrectName()
        {
            var unit = new Unit(11, "Centimeters", "cm", "Centimeters");

            Assert.AreEqual("Centimeters", unit.Name);
            Assert.AreEqual(11, unit.ModelUnitID());
        }

        [TestMethod]
        public void TestGetAbbreviationReturnsCorrectAbbreviation()
        {
            var unit = new Unit(11, "Centimeters", "cm", "centimeters");

            Assert.AreEqual("cm", unit.Abbreviation);
            Assert.AreEqual("centimeters", unit.AccessibleName);
            Assert.AreEqual("centimeters", unit.ToString());
        }
    }
```

#### 4.14.3 `[TestClass] SupplementaryResultsViewModelTests` — verbatim (`UnitConverterViewModelTests.cs:68-101`)

```csharp
    [TestClass]
    public class SupplementaryResultsViewModelTests
    {
        [TestMethod]
        public void TestGetValueReturnsCorrectValue()
        {
            var result = new SupplementaryResult(
                "3.5", new Unit(11, "Centimeters", "cm", "centimeters"));

            Assert.AreEqual("3.5", result.Value);
        }

        [TestMethod]
        public void TestGetUnitNameReturnsCorrectValue()
        {
            var unit = new Unit(11, "Centimeters", "cm", "centimeters");
            var result = new SupplementaryResult("3.5", unit);

            Assert.AreSame(unit, result.Unit);
            Assert.AreEqual("3.5 Centimeters", result.GetLocalizedAutomationName());
        }

        [TestMethod]
        public void TestGetIsWhimsicalReturnsCorrectValue()
        {
            var plain = new SupplementaryResult(
                "3.5", new Unit(11, "Centimeters", "cm", "centimeters"));
            var whimsical = new SupplementaryResult(
                "2", new Unit(90, "Jumbo Jets", "jj", "jumbo jets", isWhimsical: true));

            Assert.IsFalse(plain.IsWhimsical());
            Assert.IsTrue(whimsical.IsWhimsical());
        }
    }
```

#### 4.14.4 `[TestClass] UnitConverterDataLoaderTests` — verbatim (`UnitConverterViewModelTests.cs:103-160`)

```csharp
    [TestClass]
    public class UnitConverterDataLoaderTests
    {
        [TestMethod]
        public void AllStaticUnitsProduceFiniteConversionsInBothDirections()
        {
            var viewModel = new UnitConverterViewModel();
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            int unitsChecked = 0;

            foreach (var category in viewModel.Categories.Where(
                category => category.GetModelCategoryId() != currencyId))
            {
                viewModel.CurrentCategory = category;
                var units = viewModel.Units.ToList();
                Assert.IsTrue(units.Count > 0, $"Category '{category.Name}' exposed no units.");

                var reference = units[0];
                foreach (var unit in units)
                {
                    viewModel.Unit1 = unit;
                    viewModel.Unit2 = reference;
                    viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Clear);
                    viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.One);

                    AssertIsRealNumber(viewModel.Value2, category.Name, unit.Name, reference.Name);

                    viewModel.Unit1 = reference;
                    viewModel.Unit2 = unit;
                    AssertIsRealNumber(viewModel.Value2, category.Name, reference.Name, unit.Name);
                    unitsChecked++;
                }
            }

            Assert.IsTrue(unitsChecked > 100, $"Only {unitsChecked} units were checked.");
        }

        private static void AssertIsRealNumber(string displayed, string category, string from, string to)
        {
            string location = $"{category}: {from} -> {to} displayed '{displayed}'";
            Assert.IsFalse(string.IsNullOrWhiteSpace(displayed), $"{location} (empty)");

            var settings = LocalizationSettings.GetInstance();
            string bare = displayed
                .Replace(settings.GetNumberGroupingSeparatorStr(), string.Empty)
                .Replace("\u00A0", string.Empty)
                .Replace(settings.GetDecimalSeparatorStr(), ".");

            Assert.IsTrue(
                double.TryParse(
                    bare,
                    NumberStyles.Float,
                    CultureInfo.InvariantCulture,
                    out double value),
                $"{location} (not a number)");
            Assert.IsFalse(double.IsNaN(value) || double.IsInfinity(value), $"{location} (not finite)");
        }
    }
```

#### 4.14.5 `[TestClass] UnitConverterViewModelTests` — verbatim (`UnitConverterViewModelTests.cs:162-659`)

```csharp
    [TestClass]
    public class UnitConverterViewModelTests
    {
        [TestMethod]
        public void EnteringValueAfterSwitchingActiveUpdatesSecondValue()
        {
            var viewModel = new UnitConverterViewModel();
            viewModel.SwitchActiveCommand.Execute(null);

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Seven);

            Assert.AreEqual("7", viewModel.Value2);
        }

        [TestMethod]
        public void MaxDigitsAnnouncementIncludesTheConversionResult()
        {
            var viewModel = new UnitConverterViewModel();
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Five);
            viewModel.OnMaxDigitsReached();

            var announcement = viewModel.Announcement?.Announcement;

            Assert.IsFalse(string.IsNullOrEmpty(announcement));
            Assert.IsFalse(
                announcement.Contains("%1"),
                $"The format placeholder was never substituted: '{announcement}'.");
        }

        [TestMethod]
        public void ConversionResultNarrationSubstitutesAllPlaceholders()
        {
            var viewModel = new UnitConverterViewModel();

            string result = viewModel.GetLocalizedConversionResultStringFormat(
                "1", "meter", "3.28", "feet");

            Assert.IsFalse(result.Contains("%1"));
            Assert.IsFalse(result.Contains("%2"));
            Assert.IsFalse(result.Contains("%3"));
            Assert.IsFalse(result.Contains("%4"));
            StringAssert.Contains(result, "meter");
            StringAssert.Contains(result, "feet");
        }

        [TestMethod]
        public void SwitchingActiveValueSwapsTheFromAndToAutomationFormats()
        {
            var viewModel = new UnitConverterViewModel();
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Five);

            var value1NameBefore = viewModel.Value1AutomationName;
            var value2NameBefore = viewModel.Value2AutomationName;

            viewModel.SwitchActiveCommand.Execute(null);
            viewModel.UpdateValue1AutomationName();
            viewModel.UpdateValue2AutomationName();

            Assert.AreNotEqual(
                StripDigits(value1NameBefore),
                StripDigits(viewModel.Value1AutomationName));
            Assert.AreNotEqual(
                StripDigits(value2NameBefore),
                StripDigits(viewModel.Value2AutomationName));
        }

        private static string StripDigits(string value)
        {
            return value == null ? null : new string(value.Where(c => !char.IsDigit(c)).ToArray());
        }

        [TestMethod]
        public void PastingAMinusAfterDigitsDoesNotNegateTheValue()
        {
            var viewModel = new UnitConverterViewModel();
            SelectNegatableCategory(viewModel);

            viewModel.OnPaste("5-3");

            Assert.AreEqual("53", viewModel.Value1);
        }

        [TestMethod]
        public void PastingALeadingMinusNegatesTheValue()
        {
            var viewModel = new UnitConverterViewModel();
            SelectNegatableCategory(viewModel);

            viewModel.OnPaste("-53");

            Assert.AreEqual("-53", viewModel.Value1);
        }

        private static void SelectNegatableCategory(UnitConverterViewModel viewModel)
        {
            viewModel.CurrentCategory = viewModel.Categories.First(
                category => category.NegateVisibility == Visibility.Visible);
        }

        [TestMethod]
        public void TextWithNoUsableNumberIsRejectedBeforeItReachesTheConverter()
        {
            foreach (string candidate in new[] { "-", "-abc", ".", "abc" })
            {
                Assert.AreEqual(
                    "NoOp",
                    CopyPasteManager.ValidatePasteExpression(
                        candidate,
                        ViewMode.Length,
                        CategoryGroupType.Converter,
                        NumberBase.Unknown,
                        BitLength.BitLengthUnknown),
                    $"'{candidate}' should be rejected as a paste for a converter.");
            }
        }

        [TestMethod]
        public void PartialDisplayValuesDoNotThrowDuringFormatting()
        {
            var viewModel = new UnitConverterViewModel();

            viewModel.UpdateDisplay("-", ".");

            Assert.AreEqual("-", viewModel.Value1);
            Assert.AreEqual(".", viewModel.Value2);
        }

        [TestMethod]
        public void RejectedPasteSaysWhyInsteadOfBlankingTheDisplay()
        {
            var viewModel = new UnitConverterViewModel();
            SelectNegatableCategory(viewModel);
            viewModel.OnPaste("53");
            Assert.AreEqual("53", viewModel.Value1);

            viewModel.OnPaste("NoOp");

            Assert.IsFalse(string.IsNullOrEmpty(viewModel.Value1));
            Assert.AreEqual(viewModel.Value1, viewModel.Value2);
            Assert.AreNotEqual("53", viewModel.Value1);
        }

        [TestMethod]
        public void LargeValuesAreDisplayedWithGroupSeparators()
        {
            var viewModel = new UnitConverterViewModel();
            foreach (var digit in new[]
            {
                NumbersAndOperatorsEnum.One,
                NumbersAndOperatorsEnum.Two,
                NumbersAndOperatorsEnum.Three,
                NumbersAndOperatorsEnum.Four,
                NumbersAndOperatorsEnum.Five,
                NumbersAndOperatorsEnum.Six,
                NumbersAndOperatorsEnum.Seven
            })
            {
                viewModel.ButtonPressedCommand.Execute(digit);
            }

            var separator = LocalizationSettings.GetInstance().GetNumberGroupingSeparatorStr();

            StringAssert.Contains(viewModel.Value1, separator);
        }

        [TestMethod]
        public void LengthSuggestionsPreserveWhimsicalUnitMetadata()
        {
            var viewModel = CreateLengthViewModel();
            var resources = AppResourceProvider.GetInstance();
            Unit centimeters = viewModel.Units.Single(
                unit => unit.Name == resources.GetResourceString("UnitName_Centimeter"));
            Unit inches = viewModel.Units.Single(
                unit => unit.Name == resources.GetResourceString("UnitName_Inch"));
            SelectUnit(viewModel, centimeters, isFromUnit: true);
            SelectUnit(viewModel, inches, isFromUnit: false);

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Clear);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Four);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Seven);

            SupplementaryResult result = viewModel.SupplementaryResults.Last();
            Assert.AreEqual(resources.GetResourceString("UnitName_Hand"), result.Unit.Name);
            Assert.AreEqual(
                resources.GetResourceString("UnitAbbreviation_Hand"),
                result.Unit.Abbreviation);
            Assert.IsTrue(result.IsWhimsical());
        }

        [TestMethod]
        public async Task EnteringDigitAfterCurrencyUnitChangeReplacesValue()
        {
            var viewModel = new UnitConverterViewModel();
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == currencyId);
            await WaitForCurrencyUnitsAsync(viewModel);

            viewModel.OnPaste("1.23");
            Unit replacement = viewModel.Units.First(
                unit => unit.ModelUnitID() != viewModel.Unit1.ModelUnitID());
            SelectUnit(viewModel, replacement, isFromUnit: true);

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Seven);

            Assert.AreEqual("7", viewModel.Value1);
        }

        [TestMethod]
        public async Task EnteringCurrencyAfterBackgroundLoadUsesLoadedRatios()
        {
            var viewModel = new UnitConverterViewModel();
            Assert.IsFalse(viewModel.IsCurrencyCurrentCategory);

            await WaitForCurrencyLoadAsync(viewModel);

            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == currencyId);
            await WaitForCurrencyUnitsAsync(viewModel);

            Unit mars = viewModel.Units.First(unit => unit.Abbreviation == "MAR");
            Unit moon = viewModel.Units.First(unit => unit.Abbreviation == "MON");
            SelectUnit(viewModel, mars, isFromUnit: true);
            SelectUnit(viewModel, moon, isFromUnit: false);

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.One);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Zero);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Zero);

            Assert.AreEqual("100", viewModel.Value1);
            Assert.AreEqual("50", viewModel.Value2);
        }

        [TestMethod]
        public async Task CurrencyLoadFinishingInsideCurrencyUsesLoadedRatios()
        {
            var viewModel = new UnitConverterViewModel();
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == currencyId);
            Assert.IsFalse(viewModel.IsCurrencyDataLoaded);

            await WaitForCurrencyUnitsAsync(viewModel);

            Unit mars = viewModel.Units.First(unit => unit.Abbreviation == "MAR");
            Unit moon = viewModel.Units.First(unit => unit.Abbreviation == "MON");
            SelectUnit(viewModel, mars, isFromUnit: true);
            SelectUnit(viewModel, moon, isFromUnit: false);

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.One);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Zero);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Zero);

            Assert.AreEqual("100", viewModel.Value1);
            Assert.AreEqual("50", viewModel.Value2);
        }

        [TestMethod]
        public async Task LeavingCurrencyClearsTheCurrencySymbolsAndRatio()
        {
            var viewModel = new UnitConverterViewModel();
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == currencyId);
            await WaitForCurrencyUnitsAsync(viewModel);

            Assert.IsFalse(string.IsNullOrEmpty(viewModel.CurrencySymbol1));

            int lengthId = NavCategoryStates.Serialize(ViewMode.Length);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == lengthId);

            Assert.AreEqual(string.Empty, viewModel.CurrencySymbol1);
            Assert.AreEqual(string.Empty, viewModel.CurrencySymbol2);
            Assert.AreEqual(Visibility.Collapsed, viewModel.CurrencySymbolVisibility);
            Assert.AreEqual(string.Empty, viewModel.CurrencyRatioEquality);
        }

        [TestMethod]
        public async Task CurrencyRefreshCompletesAfterInitialLoad()
        {
            var viewModel = new UnitConverterViewModel();
            await WaitForCurrencyLoadAsync(viewModel);
            viewModel.OnCurrencyTimestampUpdated("stale timestamp", isWeekOld: true);

            await viewModel.RefreshCurrencyRatiosAsync();

            Assert.IsTrue(viewModel.IsCurrencyDataLoaded);
            Assert.IsFalse(viewModel.IsCurrencyLoadingVisible);
            Assert.IsFalse(viewModel.CurrencyDataLoadFailed);
            Assert.AreNotEqual("stale timestamp", viewModel.CurrencyTimestamp);
        }

        [TestMethod]
        public async Task LocaleDefaultCurrencyMapIsPackaged()
        {
            var file = await Windows.Storage.StorageFile.GetFileFromApplicationUriAsync(
                new Uri("ms-appx:///DataLoaders/DefaultFromToCurrency.json"));
            string json = await Windows.Storage.FileIO.ReadTextAsync(file);

            StringAssert.Contains(json, "\"en-GB\"");
            StringAssert.Contains(json, "\"GBP\"");
            StringAssert.Contains(json, "\"en-CA\"");
            StringAssert.Contains(json, "\"CAD\"");
        }

        [TestMethod]
        public void ConverterCommandsKeepTheirIdentityAcrossReads()
        {
            var viewModel = new UnitConverterViewModel();

            Assert.AreSame(viewModel.CategoryChangedCommand, viewModel.CategoryChangedCommand);
            Assert.AreSame(viewModel.UnitChangedCommand, viewModel.UnitChangedCommand);
            Assert.AreSame(viewModel.SwitchActiveCommand, viewModel.SwitchActiveCommand);
            Assert.AreSame(viewModel.ButtonPressedCommand, viewModel.ButtonPressedCommand);
            Assert.AreSame(viewModel.CopyCommand, viewModel.CopyCommand);
            Assert.AreSame(viewModel.PasteCommand, viewModel.PasteCommand);
            Assert.AreSame(viewModel.ButtonPressedCommand, viewModel.ButtonPressed);
        }

        [TestMethod]
        public void OtherViewModelCommandsKeepTheirIdentityAcrossReads()
        {
            var standard = new StandardCalculatorViewModel();
            HistoryViewModel history = standard.HistoryVM;
            Assert.AreSame(history.ClearCommand, history.ClearCommand);
            Assert.AreSame(history.HideCommand, history.HideCommand);

            var dateCalculator = new DateCalculatorViewModel();
            Assert.AreSame(dateCalculator.CopyCommand, dateCalculator.CopyCommand);

            var application = new ApplicationViewModel();
            Assert.AreSame(application.CopyCommand, application.CopyCommand);
            Assert.AreSame(application.PasteCommand, application.PasteCommand);
        }

        [TestMethod]
        public void ConstructionAndCategorySwitchingKeepTheConverterConsistent()
        {
            var viewModel = new UnitConverterViewModel();

            Assert.IsTrue(viewModel.Categories.Count > 0);
            Assert.IsTrue(viewModel.Units.Count > 0);
            Assert.IsNotNull(viewModel.CurrentCategory);
            Assert.IsNotNull(viewModel.Unit1);
            Assert.IsNotNull(viewModel.Unit2);
            Assert.IsTrue(viewModel.Value1Active ^ viewModel.Value2Active);

            var original = viewModel.CurrentCategory;
            var originalUnits = viewModel.Units.Select(unit => unit.ModelUnitID()).ToList();
            var other = viewModel.Categories.First(
                category => category.GetModelCategoryId() != original.GetModelCategoryId());

            viewModel.CurrentCategory = other;
            CollectionAssert.AreNotEqual(
                originalUnits,
                viewModel.Units.Select(unit => unit.ModelUnitID()).ToList());
            Assert.IsTrue(viewModel.Units.Contains(viewModel.Unit1));
            Assert.IsTrue(viewModel.Units.Contains(viewModel.Unit2));

            viewModel.CurrentCategory = original;
            CollectionAssert.AreEqual(
                originalUnits,
                viewModel.Units.Select(unit => unit.ModelUnitID()).ToList());
        }

        [TestMethod]
        public void CategorySwitchPublishesNewUnitsBeforeSelectedUnits()
        {
            var viewModel = new UnitConverterViewModel();
            var originalCollection = viewModel.Units;
            var originalUnits = originalCollection.Select(unit => unit.ModelUnitID()).ToList();
            int sequence = 0;
            int unitsChanged = -1;
            int unit1Changed = -1;
            int unit2Changed = -1;

            viewModel.PropertyChanged += (sender, args) =>
            {
                sequence++;
                if (args.PropertyName == nameof(UnitConverterViewModel.Units))
                {
                    unitsChanged = sequence;
                }
                else if (args.PropertyName == nameof(UnitConverterViewModel.Unit1))
                {
                    unit1Changed = sequence;
                }
                else if (args.PropertyName == nameof(UnitConverterViewModel.Unit2))
                {
                    unit2Changed = sequence;
                }
            };

            var other = viewModel.Categories.First(
                category => category.GetModelCategoryId() != viewModel.CurrentCategory.GetModelCategoryId()
                    && category.GetModelCategoryId() != NavCategoryStates.Serialize(ViewMode.Currency));
            viewModel.CurrentCategory = other;

            Assert.AreNotSame(originalCollection, viewModel.Units);
            CollectionAssert.AreEqual(
                originalUnits,
                originalCollection.Select(unit => unit.ModelUnitID()).ToList());
            Assert.IsTrue(unitsChanged > 0);
            Assert.IsTrue(unit1Changed > unitsChanged);
            Assert.IsTrue(unit2Changed > unitsChanged);
            Assert.IsTrue(viewModel.Units.Contains(viewModel.Unit1));
            Assert.IsTrue(viewModel.Units.Contains(viewModel.Unit2));
        }

        [TestMethod]
        public void InputFollowsTheActiveValueAndIsFormattedForDisplay()
        {
            var viewModel = new UnitConverterViewModel();
            var separator = LocalizationSettings.GetInstance().GetDecimalSeparatorStr();
            bool firstWasActive = viewModel.Value1Active;

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.One);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Decimal);
            StringAssert.EndsWith(viewModel.Value1, separator);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Five);
            Assert.AreEqual($"1{separator}5", viewModel.Value1);

            viewModel.SwitchActiveCommand.Execute(null);
            Assert.AreNotEqual(firstWasActive, viewModel.Value1Active);
            Assert.IsTrue(viewModel.Value1Active ^ viewModel.Value2Active);

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Eight);
            Assert.AreEqual("8", viewModel.Value2);

            viewModel.SwitchActiveCommand.Execute(null);
            Assert.AreEqual(firstWasActive, viewModel.Value1Active);
            Assert.IsTrue(viewModel.Value1Active ^ viewModel.Value2Active);

            viewModel.UpdateValue1AutomationName();
            Assert.IsFalse(string.IsNullOrEmpty(viewModel.Value1AutomationName));
            StringAssert.Contains(viewModel.Value1AutomationName, viewModel.Unit1.AccessibleName);
        }

        private static async Task WaitForCurrencyLoadAsync(UnitConverterViewModel viewModel)
        {
            for (int attempt = 0; attempt < 250; attempt++)
            {
                if (viewModel.IsCurrencyDataLoaded)
                {
                    return;
                }

                await Task.Delay(20);
            }

            Assert.Fail("The background currency load did not finish.");
        }

        private static async Task WaitForCurrencyUnitsAsync(UnitConverterViewModel viewModel)
        {
            await WaitForCurrencyLoadAsync(viewModel);

            for (int attempt = 0; attempt < 100; attempt++)
            {
                if (viewModel.Units.Count > 1
                    && viewModel.Units[0].ModelUnitID() != -1
                    && viewModel.Unit1 != null
                    && viewModel.Unit2 != null
                    && viewModel.Units.Contains(viewModel.Unit1)
                    && viewModel.Units.Contains(viewModel.Unit2))
                {
                    return;
                }

                await Task.Delay(20);
            }

            Assert.Fail("Currency units did not load.");
        }

        private static UnitConverterViewModel CreateLengthViewModel()
        {
            var viewModel = new UnitConverterViewModel();
            int lengthId = NavCategoryStates.Serialize(ViewMode.Length);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == lengthId);
            return viewModel;
        }

        private static void SelectUnit(UnitConverterViewModel viewModel, Unit unit, bool isFromUnit)
        {
            if (isFromUnit)
            {
                viewModel.Unit1 = unit;
            }
            else
            {
                viewModel.Unit2 = unit;
            }
        }
    }
```

#### 4.14.Z Remaining non-blank source in this file

<!-- UnitConverterViewModelTests.cs:660-660 -->
```csharp
}
```

### 4.15 `src/Calculator.Tests/UtilsTests.cs`

1 `[TestClass]`(es), 6 test method(s).

| # | Test method | Attributes | Source lines |
|---|---|---|---|
| 1 | `IsLastCharacterSuccess` | `[TestMethod]` | `UtilsTests.cs:12-16` |
| 2 | `IsLastCharacterSuccessMultipleSuffices` | `[TestMethod]` | `UtilsTests.cs:18-22` |
| 3 | `IsLastCharacterFailure` | `[TestMethod]` | `UtilsTests.cs:24-28` |
| 4 | `IsLastCharacterFailureAllButLastMatch` | `[TestMethod]` | `UtilsTests.cs:30-34` |
| 5 | `IsLastCharacterFailureEmptyInput` | `[TestMethod]` | `UtilsTests.cs:36-40` |
| 6 | `IsLastCharacterFailureNullTarget` | `[TestMethod]` | `UtilsTests.cs:42-46` |

#### 4.15.0 File preamble — `using` directives, file-scoped structs/helpers, namespace open (`UtilsTests.cs:1-8`)

```csharp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel.Common;

namespace Calculator.Tests
{
```

#### 4.15.1 `[TestClass] UtilsTests` — verbatim (`UtilsTests.cs:9-47`)

```csharp
    [TestClass]
    public class UtilsTests
    {
        [TestMethod]
        public void IsLastCharacterSuccess()
        {
            Assert.IsTrue(Utils.IsLastCharacterTarget("Test.", '.'));
        }

        [TestMethod]
        public void IsLastCharacterSuccessMultipleSuffices()
        {
            Assert.IsTrue(Utils.IsLastCharacterTarget("Test..", '.'));
        }

        [TestMethod]
        public void IsLastCharacterFailure()
        {
            Assert.IsFalse(Utils.IsLastCharacterTarget("Test", '.'));
        }

        [TestMethod]
        public void IsLastCharacterFailureAllButLastMatch()
        {
            Assert.IsFalse(Utils.IsLastCharacterTarget(".....T", '.'));
        }

        [TestMethod]
        public void IsLastCharacterFailureEmptyInput()
        {
            Assert.IsFalse(Utils.IsLastCharacterTarget(string.Empty, '.'));
        }

        [TestMethod]
        public void IsLastCharacterFailureNullTarget()
        {
            Assert.IsFalse(Utils.IsLastCharacterTarget(string.Empty, '\0'));
        }
    }
```

#### 4.15.Z Remaining non-blank source in this file

<!-- UtilsTests.cs:48-48 -->
```csharp
}
```

### 4.16 `src/Calculator.Tests/ViewModelLifetimeTests.cs`

1 `[TestClass]`(es), 3 test method(s).

| # | Test method | Attributes | Source lines |
|---|---|---|---|
| 1 | `ConverterViewModelsAreReleasedWhenDropped` | `[TestMethod]` | `ViewModelLifetimeTests.cs:20-32` |
| 2 | `NetworkManagersAreReleasedWhenDropped` | `[TestMethod]` | `ViewModelLifetimeTests.cs:34-47` |
| 3 | `NetworkManagerRevokesItsSubscriptionWhenCollected` | `[TestMethod]` | `ViewModelLifetimeTests.cs:49-60` |

#### 4.16.0 File preamble — `using` directives, file-scoped structs/helpers, namespace open (`ViewModelLifetimeTests.cs:1-16`)

```csharp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

using Microsoft.VisualStudio.TestTools.UnitTesting;

using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;
using Windows.Networking.Connectivity;

namespace Calculator.Tests
{
    // Converter view models own native and network resources.
```

#### 4.16.1 `[TestClass] ViewModelLifetimeTests` — verbatim (`ViewModelLifetimeTests.cs:17-135`)

```csharp
    [TestClass]
    public class ViewModelLifetimeTests
    {
        [TestMethod]
        public void ConverterViewModelsAreReleasedWhenDropped()
        {
            List<WeakReference> viewModels = CreateAndAbandonConverterViewModels(5);

            ForceFullCollection();

            int survivors = CountAlive(viewModels);
            Assert.AreEqual(
                0,
                survivors,
                $"{survivors} of {viewModels.Count} converter view models survived a full collection.");
        }

        [TestMethod]
        public void NetworkManagersAreReleasedWhenDropped()
        {
            List<WeakReference> managers = CreateAndAbandonNetworkManagers(5);

            ForceFullCollection();

            int survivors = CountAlive(managers);
            Assert.AreEqual(
                0,
                survivors,
                $"{survivors} of {managers.Count} NetworkManager instances survived a full collection. "
                + "A static WinRT event cannot hold its subscriber strongly.");
        }

        [TestMethod]
        public void NetworkManagerRevokesItsSubscriptionWhenCollected()
        {
            var source = new RecordingNetworkStatusSource();
            WeakReference manager = CreateAndAbandonNetworkManager(source);

            ForceFullCollection();

            Assert.IsFalse(manager.IsAlive);
            Assert.AreEqual(1, source.RemoveCount);
            Assert.IsNull(source.Handler);
        }

        // Keep strong locals out of the asserting stack frame.
        [MethodImpl(MethodImplOptions.NoInlining)]
        private static List<WeakReference> CreateAndAbandonConverterViewModels(int count)
        {
            var references = new List<WeakReference>(count);
            for (int i = 0; i < count; i++)
            {
                references.Add(new WeakReference(new UnitConverterViewModel()));
            }

            return references;
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        private static List<WeakReference> CreateAndAbandonNetworkManagers(int count)
        {
            var references = new List<WeakReference>(count);
            for (int i = 0; i < count; i++)
            {
                references.Add(new WeakReference(new NetworkManager()));
            }

            return references;
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        private static WeakReference CreateAndAbandonNetworkManager(RecordingNetworkStatusSource source)
        {
            return new WeakReference(new NetworkManager(source.Add, source.Remove));
        }

        private static int CountAlive(List<WeakReference> references)
        {
            int alive = 0;
            foreach (WeakReference reference in references)
            {
                if (reference.IsAlive)
                {
                    alive++;
                }
            }

            return alive;
        }

        // Finalizable objects can require more than one collection.
        private static void ForceFullCollection()
        {
            for (int pass = 0; pass < 3; pass++)
            {
                GC.Collect(GC.MaxGeneration, GCCollectionMode.Forced, blocking: true);
                GC.WaitForPendingFinalizers();
            }

            GC.Collect(GC.MaxGeneration, GCCollectionMode.Forced, blocking: true);
        }

        private sealed class RecordingNetworkStatusSource
        {
            public NetworkStatusChangedEventHandler Handler { get; private set; }
            public int RemoveCount { get; private set; }

            public void Add(NetworkStatusChangedEventHandler handler)
            {
                Handler += handler;
            }

            public void Remove(NetworkStatusChangedEventHandler handler)
            {
                Handler -= handler;
                RemoveCount++;
            }
        }
    }
```

#### 4.16.Z Remaining non-blank source in this file

<!-- ViewModelLifetimeTests.cs:136-136 -->
```csharp
}
```

**C# totals: 23 `[TestClass]`es, 288 test methods, 11 `[DataRow]` values.**

---

## 5. Golden-vector catalogue by category (test bodies re-printed verbatim)

The same test bodies appear here again, re-grouped by *what they assert*, so a category can be
lifted straight out as a golden-vector set. Every block below is a byte-for-byte copy of the
reference source at the stated line range.

| § | Category | Tests | Where they come from |
|---|---|---|---|
| 5.1 | Date calculation | 29 | `Calculator.Tests/DateCalculatorTests.cs` |
| 5.2 | Scientific / precision / rational arithmetic | 36 | `CalculatorUnitTests/CalculatorManagerTest.cpp`, `RationalTest.cpp`, `Calculator.Tests/*` |
| 5.3 | Number formatting & string conversion | 120 | `CalcEngineTests.cpp`, `CalcInputTest.cpp`, `CalculatorManagerTest.cpp`, `Calculator.Tests/*` |
| 5.4 | Unit converter (incl. currency) | 64 | `CalculatorUnitTests/UnitConverterTest.cpp`, `Calculator.Tests/UnitConverterViewModelTests.cs`, `CurrencyConverterTests.cs` |
| 5.5 | Memory & history | 55 | `CalculatorManagerTest.cpp`, `Calculator.Tests/HistoryTests.cs`, `StandardCalculatorViewModelTests.cs`, `MultiWindowTests.cs` |
| 5.6 | Max-digits / precision thresholds | 10 | `CalculatorManagerTest.cpp`, `UnitConverterTest.cpp`, `CalcInputTest.cpp`, `NarratorAnnouncementTests.cs`, `UnitConverterViewModelTests.cs` |

---

### 5.1 Date calculation vectors

Date add / subtract / difference vectors. All live in the C# `Calculator.Tests` project — the
C++ engine project has **no** date tests (`DateCalculationEngine` is managed code).
The `DateTimeTestCase` struct, the `s_date[]`, `s_dateDifference[]`, `s_datetimeDifftest[]`,
`s_datetimeBoundAdd[]`, `s_datetimeBoundSubtract[]`, `s_datetimeAddCase[]`, `s_datetimeSubtractCase[]`
tables and the `[ClassInitialize] TestClassSetup` that fills them are inside the class blocks
reproduced verbatim below (see also §4.3 and §4.9).

#### 5.1.1 `DateCalculatorUnitTests` + `DateCalculatorViewModelTests` (whole file, all data tables in situ)

The two class blocks below include every `MakeDate(...)`, `DateDifference(...)` and `DateTimeTestCase(...)`
literal that the assertions depend on. **These literals are the date golden vectors.**

##### `TestDateDiff` — `DateCalculatorUnitTests` — `DateCalculatorTests.cs:107-128`

```csharp
        [TestMethod]
        public void TestDateDiff()
        {
            for (int testIndex = 0; testIndex < s_datetimeDifftest.Length; testIndex++)
            {
                var testCase = s_datetimeDifftest[testIndex];
                var requested = UnitsPresentIn(testCase.DateDiff);

                // The engine takes the dates in order; putting them the other way round is the view
                // model's job to normalize, which DateCalcViewModelDateDiffIgnoreSignTest covers.
                var earlier = testCase.StartDate <= testCase.EndDate ? testCase.StartDate : testCase.EndDate;
                var later = testCase.StartDate <= testCase.EndDate ? testCase.EndDate : testCase.StartDate;

                var difference = s_dateCalcEngine.TryGetDateDifference(earlier, later, requested);

                Assert.IsNotNull(difference, $"TryGetDateDifference returned null for case {testIndex}");
                Assert.AreEqual(testCase.DateDiff.Year, difference.Value.Year, $"year, case {testIndex}");
                Assert.AreEqual(testCase.DateDiff.Month, difference.Value.Month, $"month, case {testIndex}");
                Assert.AreEqual(testCase.DateDiff.Week, difference.Value.Week, $"week, case {testIndex}");
                Assert.AreEqual(testCase.DateDiff.Day, difference.Value.Day, $"day, case {testIndex}");
            }
        }
```

##### `OneYearDifferenceEndingAtTheTopOfRangeIsCalculated` — `DateCalculatorUnitTests` — `DateCalculatorTests.cs:130-141`

```csharp
        [TestMethod]
        public void OneYearDifferenceEndingAtTheTopOfRangeIsCalculated()
        {
            var difference = s_dateCalcEngine.TryGetDateDifference(
                MakeDate(9998, 12, 31),
                MakeDate(9999, 12, 31),
                DateUnit.Year);

            Assert.IsNotNull(difference);
            Assert.AreEqual(1, difference.Value.Year);
            Assert.AreEqual(0, difference.Value.Day);
        }
```

##### `FullSupportedRangeReturnsYearMonthDayDifference` — `DateCalculatorUnitTests` — `DateCalculatorTests.cs:143-155`

```csharp
        [TestMethod]
        public void FullSupportedRangeReturnsYearMonthDayDifference()
        {
            var difference = s_dateCalcEngine.TryGetDateDifference(
                MakeDate(1601, 1, 1),
                MakeDate(9999, 12, 31),
                DateUnit.Year | DateUnit.Month | DateUnit.Day);

            Assert.IsNotNull(difference);
            Assert.AreEqual(8398, difference.Value.Year);
            Assert.AreEqual(11, difference.Value.Month);
            Assert.AreEqual(30, difference.Value.Day);
        }
```

##### `RangeEndingInYear9998ReturnsYearMonthDayDifference` — `DateCalculatorUnitTests` — `DateCalculatorTests.cs:157-169`

```csharp
        [TestMethod]
        public void RangeEndingInYear9998ReturnsYearMonthDayDifference()
        {
            var difference = s_dateCalcEngine.TryGetDateDifference(
                MakeDate(1601, 1, 1),
                MakeDate(9998, 12, 31),
                DateUnit.Year | DateUnit.Month | DateUnit.Day);

            Assert.IsNotNull(difference);
            Assert.AreEqual(8397, difference.Value.Year);
            Assert.AreEqual(11, difference.Value.Month);
            Assert.AreEqual(30, difference.Value.Day);
        }
```

##### `TestAddOob` — `DateCalculatorUnitTests` — `DateCalculatorTests.cs:183-194`

```csharp
        [TestMethod]
        public void TestAddOob()
        {
            for (int testIndex = 0; testIndex < s_datetimeBoundAdd.Length; testIndex++)
            {
                var endDate = s_dateCalcEngine.AddDuration(
                    s_datetimeBoundAdd[testIndex].StartDate,
                    s_datetimeBoundAdd[testIndex].DateDiff);

                Assert.IsNull(endDate, $"AddDuration should return null for out-of-bound case {testIndex}");
            }
        }
```

##### `TestSubtractOob` — `DateCalculatorUnitTests` — `DateCalculatorTests.cs:196-209`

```csharp
        [TestMethod]
        public void TestSubtractOob()
        {
            for (int testIndex = 0; testIndex < s_datetimeBoundSubtract.Length; testIndex++)
            {
                // Subtract Duration
                var endDate = s_dateCalcEngine.SubtractDuration(
                    s_datetimeBoundSubtract[testIndex].StartDate,
                    s_datetimeBoundSubtract[testIndex].DateDiff);

                // Assert for the result
                Assert.IsNull(endDate, $"SubtractDuration should return null for out-of-bound case {testIndex}");
            }
        }
```

##### `TestAddition` — `DateCalculatorUnitTests` — `DateCalculatorTests.cs:211-226`

```csharp
        [TestMethod]
        public void TestAddition()
        {
            for (int testIndex = 0; testIndex < s_datetimeAddCase.Length; testIndex++)
            {
                var endDate = s_dateCalcEngine.AddDuration(
                    s_datetimeAddCase[testIndex].StartDate,
                    s_datetimeAddCase[testIndex].DateDiff);

                Assert.IsNotNull(endDate, $"AddDuration returned null for case {testIndex}");
                Assert.AreEqual(
                    s_datetimeAddCase[testIndex].EndDate,
                    endDate.Value,
                    $"AddDuration produced the wrong date for case {testIndex}");
            }
        }
```

##### `TestSubtraction` — `DateCalculatorUnitTests` — `DateCalculatorTests.cs:228-243`

```csharp
        [TestMethod]
        public void TestSubtraction()
        {
            for (int testIndex = 0; testIndex < s_datetimeSubtractCase.Length; testIndex++)
            {
                var endDate = s_dateCalcEngine.SubtractDuration(
                    s_datetimeSubtractCase[testIndex].StartDate,
                    s_datetimeSubtractCase[testIndex].DateDiff);

                Assert.IsNotNull(endDate, $"SubtractDuration returned null for case {testIndex}");
                Assert.AreEqual(
                    s_datetimeSubtractCase[testIndex].EndDate,
                    endDate.Value,
                    $"SubtractDuration produced the wrong date for case {testIndex}");
            }
        }
```

##### `DateCalcViewModelInitializationTest` — `DateCalculatorViewModelTests` — `DateCalculatorTests.cs:333-353`

```csharp
        [TestMethod]
        public void DateCalcViewModelInitializationTest()
        {
            var viewModel = new DateCalculatorViewModel();

            Assert.IsTrue(viewModel.IsDateDiffMode);
            Assert.IsTrue(viewModel.IsAddMode);

            Assert.AreNotEqual(default(DateTimeOffset), viewModel.FromDate);
            Assert.AreNotEqual(default(DateTimeOffset), viewModel.ToDate);
            Assert.AreNotEqual(default(DateTimeOffset), viewModel.StartDate);

            Assert.AreEqual(0, viewModel.DaysOffset);
            Assert.AreEqual(0, viewModel.MonthsOffset);
            Assert.AreEqual(0, viewModel.YearsOffset);

            Assert.IsTrue(viewModel.IsDiffInDays);
            Assert.AreEqual("Same dates", viewModel.StrDateDiffResult);
            Assert.AreEqual(string.Empty, viewModel.StrDateDiffResultInDays);
            Assert.AreEqual(string.Empty, viewModel.StrDateResult);
        }
```

##### `DateCalcViewModelAddSubtractInitTest` — `DateCalculatorViewModelTests` — `DateCalculatorTests.cs:355-378`

```csharp
        [TestMethod]
        public void DateCalcViewModelAddSubtractInitTest()
        {
            var viewModel = new DateCalculatorViewModel();
            viewModel.IsDateDiffMode = false;

            Assert.IsFalse(viewModel.IsDateDiffMode);
            Assert.IsTrue(viewModel.IsAddMode);

            Assert.AreNotEqual(default(DateTimeOffset), viewModel.FromDate);
            Assert.AreNotEqual(default(DateTimeOffset), viewModel.ToDate);
            Assert.AreNotEqual(default(DateTimeOffset), viewModel.StartDate);

            Assert.AreEqual(0, viewModel.DaysOffset);
            Assert.AreEqual(0, viewModel.MonthsOffset);
            Assert.AreEqual(0, viewModel.YearsOffset);

            Assert.IsTrue(viewModel.IsDiffInDays);
            Assert.AreEqual("Same dates", viewModel.StrDateDiffResult);
            Assert.AreEqual(string.Empty, viewModel.StrDateDiffResultInDays);

            Assert.IsNotNull(viewModel.StrDateResult);
            Assert.AreNotEqual("", viewModel.StrDateResult);
        }
```

##### `DateCalcViewModelDateDiffDaylightSavingTimeTest` — `DateCalculatorViewModelTests` — `DateCalculatorTests.cs:380-414`

```csharp
        [TestMethod]
        public void DateCalcViewModelDateDiffDaylightSavingTimeTest()
        {
            var viewModel = new DateCalculatorViewModel();
            viewModel.IsDateDiffMode = true;
            Assert.IsTrue(viewModel.IsDateDiffMode);

            // 31.03.2008 -> 29.02.2008
            viewModel.FromDate = s_datetimeDifftest[5].StartDate;
            viewModel.ToDate = s_datetimeDifftest[5].EndDate;

            // Assert for the result
            Assert.IsFalse(viewModel.IsDiffInDays);
            Assert.AreEqual("31 days", viewModel.StrDateDiffResultInDays);
            Assert.AreEqual("1 month, 2 days", viewModel.StrDateDiffResult);

            // Daylight Saving Time - Clock Forward
            // 10.03.2019 -> 11.03.2019
            viewModel.FromDate = MakeDate(2019, 3, 10);
            viewModel.ToDate = MakeDate(2019, 3, 11);
            Assert.IsTrue(viewModel.IsDiffInDays);
            Assert.AreEqual("1 day", viewModel.StrDateDiffResult);

            // 10.03.2019 -> 17.03.2019
            viewModel.ToDate = MakeDate(2019, 3, 17);
            Assert.IsFalse(viewModel.IsDiffInDays);
            Assert.AreEqual("1 week", viewModel.StrDateDiffResult);

            // Daylight Saving Time - Clock Backward
            // 03.11.2019 -> 04.11.2019
            viewModel.FromDate = MakeDate(2019, 11, 3);
            viewModel.ToDate = MakeDate(2019, 11, 4);
            Assert.IsTrue(viewModel.IsDiffInDays);
            Assert.AreEqual("1 day", viewModel.StrDateDiffResult);
        }
```

##### `DateCalcViewModelAddTest` — `DateCalculatorViewModelTests` — `DateCalculatorTests.cs:416-436`

```csharp
        [TestMethod]
        public void DateCalcViewModelAddTest()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = false;
            viewModel.IsAddMode = true;

            for (int testIndex = 0; testIndex < s_datetimeAddCase.Length; testIndex++)
            {
                viewModel.StartDate = s_datetimeAddCase[testIndex].StartDate;
                viewModel.DaysOffset = s_datetimeAddCase[testIndex].DateDiff.Day;
                viewModel.MonthsOffset = s_datetimeAddCase[testIndex].DateDiff.Month;
                viewModel.YearsOffset = s_datetimeAddCase[testIndex].DateDiff.Year;

                Assert.AreEqual(
                    ExpectedDateString(s_datetimeAddCase[testIndex].EndDate),
                    viewModel.StrDateResult,
                    $"Add mode produced the wrong date for case {testIndex}");
            }
        }
```

##### `DateCalcViewModelSubtractTest` — `DateCalculatorViewModelTests` — `DateCalculatorTests.cs:438-458`

```csharp
        [TestMethod]
        public void DateCalcViewModelSubtractTest()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = false;
            viewModel.IsAddMode = false;

            for (int testIndex = 0; testIndex < s_datetimeSubtractCase.Length; testIndex++)
            {
                viewModel.StartDate = s_datetimeSubtractCase[testIndex].StartDate;
                viewModel.DaysOffset = s_datetimeSubtractCase[testIndex].DateDiff.Day;
                viewModel.MonthsOffset = s_datetimeSubtractCase[testIndex].DateDiff.Month;
                viewModel.YearsOffset = s_datetimeSubtractCase[testIndex].DateDiff.Year;

                Assert.AreEqual(
                    ExpectedDateString(s_datetimeSubtractCase[testIndex].EndDate),
                    viewModel.StrDateResult,
                    $"Subtract mode produced the wrong date for case {testIndex}");
            }
        }
```

##### `DateCalcViewModelAddOobTest` — `DateCalculatorViewModelTests` — `DateCalculatorTests.cs:460-480`

```csharp
        [TestMethod]
        public void DateCalcViewModelAddOobTest()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = false;
            viewModel.IsAddMode = true;
            Assert.IsFalse(viewModel.IsDateDiffMode);
            Assert.IsTrue(viewModel.IsAddMode);

            for (int testIndex = 0; testIndex < s_datetimeBoundAdd.Length; testIndex++)
            {
                viewModel.StartDate = s_datetimeBoundAdd[testIndex].StartDate;
                viewModel.DaysOffset = s_datetimeBoundAdd[testIndex].DateDiff.Day;
                viewModel.MonthsOffset = s_datetimeBoundAdd[testIndex].DateDiff.Month;
                viewModel.YearsOffset = s_datetimeBoundAdd[testIndex].DateDiff.Year;

                // Assert for the result
                Assert.AreEqual("Date out of Bound", viewModel.StrDateResult);
            }
        }
```

##### `DateCalcViewModelSubtractOobTest` — `DateCalculatorViewModelTests` — `DateCalculatorTests.cs:491-511`

```csharp
        [TestMethod]
        public void DateCalcViewModelSubtractOobTest()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = false;
            viewModel.IsAddMode = false;
            Assert.IsFalse(viewModel.IsDateDiffMode);
            Assert.IsFalse(viewModel.IsAddMode);

            for (int testIndex = 0; testIndex < s_datetimeBoundSubtract.Length; testIndex++)
            {
                viewModel.StartDate = s_datetimeBoundSubtract[testIndex].StartDate;
                viewModel.DaysOffset = s_datetimeBoundSubtract[testIndex].DateDiff.Day;
                viewModel.MonthsOffset = s_datetimeBoundSubtract[testIndex].DateDiff.Month;
                viewModel.YearsOffset = s_datetimeBoundSubtract[testIndex].DateDiff.Year;

                // Assert for the result
                Assert.AreEqual("Date out of Bound", viewModel.StrDateResult);
            }
        }
```

##### `DateCalcViewModelDateDiffIgnoreSignTest` — `DateCalculatorViewModelTests` — `DateCalculatorTests.cs:513-534`

```csharp
        [TestMethod]
        public void DateCalcViewModelDateDiffIgnoreSignTest()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = true;
            Assert.IsTrue(viewModel.IsDateDiffMode);

            viewModel.FromDate = s_date[10]; // 10.05.2007
            viewModel.ToDate = s_date[6];    // 10.03.2008

            Assert.IsFalse(viewModel.IsDiffInDays);
            Assert.AreEqual("305 days", viewModel.StrDateDiffResultInDays);
            Assert.AreEqual("10 months", viewModel.StrDateDiffResult);

            viewModel.FromDate = s_date[6];  // 10.03.2008
            viewModel.ToDate = s_date[10];   // 10.05.2007

            Assert.IsFalse(viewModel.IsDiffInDays);
            Assert.AreEqual("305 days", viewModel.StrDateDiffResultInDays);
            Assert.AreEqual("10 months", viewModel.StrDateDiffResult);
        }
```

##### `DateCalcViewModelRangeEndingAtMaximumShowsYearsMonthsAndTotalDays` — `DateCalculatorViewModelTests` — `DateCalculatorTests.cs:536-554`

```csharp
        [TestMethod]
        public void DateCalcViewModelRangeEndingAtMaximumShowsYearsMonthsAndTotalDays()
        {
            var viewModel = new DateCalculatorViewModel();
            viewModel.IsDateDiffMode = true;

            viewModel.FromDate = s_date[13];  // 31.01.2008
            viewModel.ToDate = s_date[0];     // 31.12.9999

            var resources = AppResourceProvider.GetInstance();
            string expectedBreakdown = LocalizeNumber(7991) + " " + resources.GetResourceString("Date_Years")
                + LocalizationSettings.GetInstance().GetListSeparator() + " "
                + LocalizeNumber(11) + " " + resources.GetResourceString("Date_Months");
            string expectedDays = LocalizeNumber(2918987) + " " + resources.GetResourceString("Date_Days");

            Assert.IsFalse(viewModel.IsDiffInDays);
            Assert.AreEqual(expectedBreakdown, viewModel.StrDateDiffResult);
            Assert.AreEqual(expectedDays, viewModel.StrDateDiffResultInDays);
        }
```

##### `DateCalcViewModelDateDiffResultInPositiveDaysTest` — `DateCalculatorViewModelTests` — `DateCalculatorTests.cs:556-570`

```csharp
        [TestMethod]
        public void DateCalcViewModelDateDiffResultInPositiveDaysTest()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = true;
            Assert.IsTrue(viewModel.IsDateDiffMode);

            viewModel.FromDate = s_date[1]; // 30.12.9999
            viewModel.ToDate = s_date[0];   // 31.12.9999

            Assert.IsTrue(viewModel.IsDiffInDays);
            Assert.AreEqual("1 day", viewModel.StrDateDiffResult);
            Assert.AreEqual(string.Empty, viewModel.StrDateDiffResultInDays);
        }
```

##### `DateCalcViewModelDateDiffAutomationNameUsesLocalizedFormat` — `DateCalculatorViewModelTests` — `DateCalculatorTests.cs:572-585`

```csharp
        [TestMethod]
        public void DateCalcViewModelDateDiffAutomationNameUsesLocalizedFormat()
        {
            var viewModel = new DateCalculatorViewModel
            {
                IsDateDiffMode = true,
                FromDate = s_date[1],
                ToDate = s_date[0]
            };

            Assert.AreEqual(
                "Difference " + viewModel.StrDateDiffResult,
                viewModel.StrDateDiffResultAutomationName);
        }
```

##### `DateCalcViewModelDateDiffFromDateHigherThanToDate` — `DateCalculatorViewModelTests` — `DateCalculatorTests.cs:587-601`

```csharp
        [TestMethod]
        public void DateCalcViewModelDateDiffFromDateHigherThanToDate()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = true;
            Assert.IsTrue(viewModel.IsDateDiffMode);

            viewModel.FromDate = s_date[0]; // 31.12.9999
            viewModel.ToDate = s_date[1];   // 30.12.9999

            Assert.IsTrue(viewModel.IsDiffInDays);
            Assert.AreEqual("1 day", viewModel.StrDateDiffResult);
            Assert.AreEqual(string.Empty, viewModel.StrDateDiffResultInDays);
        }
```

##### `DateCalcViewModelPreservesCalendarDateForOffsetPickerValues` — `DateCalculatorViewModelTests` — `DateCalculatorTests.cs:603-615`

```csharp
        [TestMethod]
        public void DateCalcViewModelPreservesCalendarDateForOffsetPickerValues()
        {
            var viewModel = new DateCalculatorViewModel
            {
                IsDateDiffMode = true,
                FromDate = new DateTimeOffset(2024, 3, 1, 0, 0, 0, TimeSpan.FromHours(2)),
                ToDate = new DateTimeOffset(2024, 4, 1, 0, 0, 0, TimeSpan.FromHours(2))
            };

            Assert.IsFalse(viewModel.IsDiffInDays);
            Assert.AreEqual("1 month", viewModel.StrDateDiffResult);
        }
```

##### `DateCalcViewModelAddSubtractResultAutomationNameTest` — `DateCalculatorViewModelTests` — `DateCalculatorTests.cs:617-631`

```csharp
        [TestMethod]
        public void DateCalcViewModelAddSubtractResultAutomationNameTest()
        {
            var viewModel = new DateCalculatorViewModel();
            viewModel.IsDateDiffMode = false;
            viewModel.IsAddMode = true;
            viewModel.StartDate = s_date[13];
            viewModel.DaysOffset = 1;
            viewModel.MonthsOffset = 0;
            viewModel.YearsOffset = 0;

            var automationName = viewModel.StrDateResultAutomationName;

            Assert.AreEqual("Resulting date " + viewModel.StrDateResult, automationName);
        }
```

##### `JaEraTransitionAddition` — `DateCalculatorViewModelTests` — `DateCalculatorTests.cs:633-643`

```csharp
        [TestMethod]
        public void JaEraTransitionAddition()
        {
            // Showa ends 1989-01-07 and Heisei begins on the 8th. Date arithmetic must not notice.
            var engine = new DateCalculationEngine("JapaneseCalendar");

            var result = engine.AddDuration(MakeDate(1989, 1, 7), new DateDifference { Day = 1 });

            Assert.IsNotNull(result, "AddDuration returned null across the Showa/Heisei boundary");
            Assert.AreEqual(MakeDate(1989, 1, 8), result.Value);
        }
```

##### `JaEraTransitionSubtraction` — `DateCalculatorViewModelTests` — `DateCalculatorTests.cs:645-654`

```csharp
        [TestMethod]
        public void JaEraTransitionSubtraction()
        {
            var engine = new DateCalculationEngine("JapaneseCalendar");

            var result = engine.SubtractDuration(MakeDate(1989, 1, 8), new DateDifference { Day = 1 });

            Assert.IsNotNull(result, "SubtractDuration returned null across the Showa/Heisei boundary");
            Assert.AreEqual(MakeDate(1989, 1, 7), result.Value);
        }
```

##### `JaEraTransitionDifference` — `DateCalculatorViewModelTests` — `DateCalculatorTests.cs:656-671`

```csharp
        [TestMethod]
        public void JaEraTransitionDifference()
        {
            var engine = new DateCalculationEngine("JapaneseCalendar");

            var showaToHeisei = engine.TryGetDateDifference(
                MakeDate(1989, 1, 7), MakeDate(1989, 1, 8), DateUnit.Day);
            Assert.IsNotNull(showaToHeisei, "TryGetDateDifference returned null across Showa/Heisei");
            Assert.AreEqual(1, showaToHeisei.Value.Day);

            // Heisei ends 2019-04-30 and Reiwa begins on the 1st; the same must hold there.
            var heiseiToReiwa = engine.TryGetDateDifference(
                MakeDate(2019, 4, 30), MakeDate(2019, 5, 1), DateUnit.Day);
            Assert.IsNotNull(heiseiToReiwa, "TryGetDateDifference returned null across Heisei/Reiwa");
            Assert.AreEqual(1, heiseiToReiwa.Value.Day);
        }
```

##### `JapaneseCalendarSubtractionBelowSupportedRangeReturnsNullAndRecovers` — `DateCalculatorViewModelTests` — `DateCalculatorTests.cs:673-687`

```csharp
        [TestMethod]
        public void JapaneseCalendarSubtractionBelowSupportedRangeReturnsNullAndRecovers()
        {
            var engine = new DateCalculationEngine("JapaneseCalendar");

            var outOfRange = engine.SubtractDuration(
                MakeDate(1900, 1, 1),
                new DateDifference { Year = 100 });

            Assert.IsNull(outOfRange);

            Assert.IsNull(engine.SubtractDuration(
                MakeDate(1900, 1, 1),
                new DateDifference { Year = 100 }));
        }
```

##### `UmAlQuraRangeAtCalendarLimitReturnsYearMonthDayDifference` — `DateCalculatorViewModelTests` — `DateCalculatorTests.cs:689-703`

```csharp
        [TestMethod]
        public void UmAlQuraRangeAtCalendarLimitReturnsYearMonthDayDifference()
        {
            var engine = new DateCalculationEngine("UmAlQuraCalendar");

            var difference = engine.TryGetDateDifference(
                MakeDate(2075, 1, 1),
                MakeDate(2077, 11, 16),
                DateUnit.Year | DateUnit.Month | DateUnit.Day);

            Assert.IsNotNull(difference);
            Assert.AreEqual(2, difference.Value.Year);
            Assert.AreEqual(11, difference.Value.Month);
            Assert.AreEqual(16, difference.Value.Day);
        }
```

##### `JapaneseDifferenceRecoveryPreservesTheCalendarSystem` — `DateCalculatorViewModelTests` — `DateCalculatorTests.cs:705-718`

```csharp
        [TestMethod]
        public void JapaneseDifferenceRecoveryPreservesTheCalendarSystem()
        {
            var engine = new DateCalculationEngine("JapaneseCalendar");

            Assert.IsNotNull(engine.TryGetDateDifference(
                MakeDate(1900, 1, 1),
                MakeDate(9998, 12, 31),
                DateUnit.Year | DateUnit.Month | DateUnit.Day));

            Assert.IsNull(engine.SubtractDuration(
                MakeDate(1900, 1, 1),
                new DateDifference { Year = 100 }));
        }
```

##### `DateCalculatorInstancesKeepIndependentState` — `MultiWindowUnitTests` — `MultiWindowTests.cs:658-680`

```csharp
        [TestMethod]
        public void DateCalculatorInstancesKeepIndependentState()
        {
            var first = new DateCalculatorViewModel { IsDateDiffMode = true };
            var second = new DateCalculatorViewModel { IsDateDiffMode = true };
            var third = new DateCalculatorViewModel();

            first.FromDate = new DateTimeOffset(2008, 1, 31, 0, 0, 0, TimeSpan.Zero);
            first.ToDate = new DateTimeOffset(2008, 3, 31, 0, 0, 0, TimeSpan.Zero);

            second.FromDate = new DateTimeOffset(2007, 2, 28, 0, 0, 0, TimeSpan.Zero);
            second.ToDate = new DateTimeOffset(2007, 3, 10, 0, 0, 0, TimeSpan.Zero);

            third.IsDateDiffMode = false;
            third.IsAddMode = true;
            third.StartDate = new DateTimeOffset(2008, 1, 31, 0, 0, 0, TimeSpan.Zero);
            third.DaysOffset = 1;

            Assert.AreNotEqual(first.StrDateDiffResult, second.StrDateDiffResult,
                "Two date calculators shared a result.");
            Assert.IsFalse(string.IsNullOrEmpty(third.StrDateResult));
            Assert.IsTrue(first.IsDateDiffMode, "One date calculator's mode changed another's.");
        }
```

### 5.2 Scientific / precision / rational-arithmetic vectors

Scientific-mode evaluation, order-of-operations, programmer-mode (binary/octal/hex), and the
`Ratpack` rational modulo/remainder arithmetic used for exact big-integer results.

#### 5.2.1 `TEST_CLASS(CalculatorManagerTest)` — scientific / standard / programmer evaluation vectors

`TestDriver::Test(expectedPrimaryDisplay, expectedExpression, commands, cleanup, isScientific)` asserts both
the primary display string and the expression string after feeding a `Command[]` stream. These are the
richest display-formatting vectors in the repository.

##### `CalculatorManagerTestScientific` — `CalculatorManagerTest` — `CalculatorManagerTest.cpp:404-498`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestScientific()
    {
        Command commands1[] = { Command::Command1, Command::Command2, Command::Command3, Command::CommandPNT,
                                Command::Command4, Command::Command5, Command::Command6, Command::CommandNULL };
        TestDriver::Test(L"123.456", L"", commands1, true, true);

        Command commands2[] = { Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"0 + ", commands2, true, true);

        Command commands3[] = { Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(0)", commands3, true, true);

        Command commands4[] = { Command::Command1, Command::CommandADD, Command::Command0,   Command::CommandMUL,
                                Command::Command2, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"1", L"1 + 0 \x00D7 2=", commands4, true, true);

        Command commands5[] = { Command::Command4, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"4", L"4=", commands5, true, true);

        Command commands6[] = { Command::Command2,    Command::Command5,    Command::Command6,   Command::CommandSQRT,
                                Command::CommandSQRT, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"2", L"\x221A(\x221A(\x221A(256)))", commands6, true, true);

        Command commands7[] = { Command::Command3,   Command::CommandSUB, Command::Command6,   Command::CommandEQU,
                                Command::CommandMUL, Command::Command3,   Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"-9", L"-3 \x00D7 3 + ", commands7, true, true);

        Command commands8[] = { Command::Command9, Command::CommandMUL, Command::Command6, Command::CommandSUB, Command::CommandCENTR,
                                Command::Command8, Command::CommandMUL, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"38", L"9 \x00D7 6 - 8 \x00D7 2 + ", commands8, true, true);

        Command commands9[] = { Command::Command6, Command::CommandMUL, Command::Command6, Command::CommandSIGN, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"Invalid input", L"6 \x00D7 \x221A(-6)", commands9, true, true);

        Command commands10[] = { Command::Command5, Command::Command0,   Command::CommandADD, Command::Command2,
                                 Command::Command0, Command::CommandREC, Command::CommandSUB, Command::CommandNULL };
        TestDriver::Test(L"50.05", L"50 + 1/(20) - ", commands10, true, true);

        Command commands11[] = { Command::Command4, Command::CommandADD, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"8", L"4 + 4=", commands11, true, true);

        Command commands12[] = { Command::Command5, Command::CommandADD, Command::CommandMUL, Command::Command3, Command::CommandNULL };
        TestDriver::Test(L"3", L"5 \x00D7 ", commands12, true, true);

        Command commands13[] = { Command::Command1, Command::CommandEXP, Command::CommandSIGN, Command::Command9, Command::Command9,   Command::Command9,
                                 Command::Command9, Command::CommandDIV, Command::Command1,    Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Overflow", L"1.e-9999 \x00F7 ", commands13, true, true);

        Command commands14[] = { Command::Command5, Command::Command0,       Command::CommandADD, Command::Command2,
                                 Command::Command0, Command::CommandPERCENT, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"60", L"50 + 10=", commands14, true, true);

        Command commands15[] = { Command::Command0, Command::CommandDIV, Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Result is undefined", L"0 \x00F7 ", commands15, true, true);

        Command commands16[] = { Command::Command1, Command::CommandDIV, Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Cannot divide by zero", L"1 \x00F7 ", commands16, true, true);

        Command commands17[] = { Command::Command1,     Command::Command2, Command::CommandADD, Command::Command5,
                                 Command::CommandCENTR, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"14", L"12 + 2 + ", commands17, true, true);

        Command commands18[] = { Command::Command1, Command::Command0, Command::Command0, Command::CommandSIGN, Command::CommandREC, Command::CommandNULL };
        TestDriver::Test(L"-0.01", L"1/(-100)", commands18, true, true);

        Command commands19[] = { Command::Command1, Command::Command2, Command::Command3, Command::CommandBACK, Command::CommandBACK, Command::CommandNULL };
        TestDriver::Test(L"1", L"", commands19, true, true);

        Command commands20[] = { Command::Command1,    Command::Command2,    Command::Command3,   Command::CommandBACK,
                                 Command::CommandBACK, Command::CommandBACK, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands20, true, true);

        Command commands21[] = { Command::Command4, Command::CommandSQRT, Command::CommandSUB, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(4) - 2 + ", commands21, true, true);

        Command commands22[] = { Command::Command0, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(0)", commands22, true, true);

        Command commands23[] = { Command::Command1,   Command::Command0, Command::Command2, Command::Command4,   Command::CommandSQRT,
                                 Command::CommandSUB, Command::Command3, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(1024) - 32 + ", commands23, true, true);

        Command commands24[] = { Command::Command2,    Command::Command5,    Command::Command7,   Command::CommandSQRT,
                                 Command::CommandSQRT, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"2.0009748976330773374220277351385", L"\x221A(\x221A(\x221A(257)))", commands24, true, true);

        Command commands25[] = { Command::Command2, Command::CommandPNT, Command::Command2, Command::Command5,   Command::CommandSQRT, Command::CommandSUB,
                                 Command::Command1, Command::CommandPNT, Command::Command5, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(2.25) - 1.5=", commands25, true, true);

        Command commands26[] = { Command::CommandOPENP, Command::Command2,   Command::CommandPNT, Command::Command2,   Command::Command5,
                                 Command::CommandSQRT,  Command::CommandDIV, Command::Command1,   Command::CommandPNT, Command::Command5,
                                 Command::CommandCLOSEP, Command::CommandLOG,  Command::CommandNULL };
        TestDriver::Test(L"0", L"log(\x221A(2.25) \x00F7 1.5)", commands26, true, true);
    }
```

##### `CalculatorManagerTestScientific2` — `CalculatorManagerTest` — `CalculatorManagerTest.cpp:501-634`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestScientific2()
    {
        Command commands1[] = { Command::Command1, Command::Command2, Command::CommandSQR, Command::CommandNULL };
        TestDriver::Test(L"144", L"sqr(12)", commands1, true, true);

        Command commands2[] = { Command::Command5, Command::CommandFAC, Command::CommandNULL };
        TestDriver::Test(L"120", L"fact(5)", commands2, true, true);

        Command commands3[] = { Command::Command5, Command::CommandPWR, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"25", L"5 ^ 2 + ", commands3, true, true);

        Command commands4[] = { Command::Command8, Command::CommandROOT, Command::Command3, Command::CommandMUL, Command::CommandNULL };
        TestDriver::Test(L"2", L"8 yroot 3 \x00D7 ", commands4, true, true);

        Command commands5[] = { Command::Command8, Command::CommandCUB, Command::CommandNULL };
        TestDriver::Test(L"512", L"cube(8)", commands5, true, true);

        Command commands6[] = { Command::Command8, Command::CommandCUB, Command::CommandCUBEROOT, Command::CommandNULL };
        TestDriver::Test(L"8", L"cuberoot(cube(8))", commands6, true, true);

        Command commands7[] = { Command::Command1, Command::Command0, Command::CommandLOG, Command::CommandNULL };
        TestDriver::Test(L"1", L"log(10)", commands7, true, true);

        Command commands8[] = { Command::Command5, Command::CommandPOW10, Command::CommandNULL };
        TestDriver::Test(L"100,000", L"10^(5)", commands8, true, true);

        Command commands9[] = { Command::Command1, Command::Command0, Command::CommandLN, Command::CommandNULL };
        TestDriver::Test(L"2.3025850929940456840179914546844", L"ln(10)", commands9, true, true);

        Command commands10[] = { Command::Command1, Command::CommandSIN, Command::CommandNULL };
        TestDriver::Test(L"0.01745240643728351281941897851632", L"sin\x2080(1)", commands10, true, true);

        Command commands11[] = { Command::Command1, Command::CommandCOS, Command::CommandNULL };
        TestDriver::Test(L"0.99984769515639123915701155881391", L"cos\x2080(1)", commands11, true, true);

        Command commands12[] = { Command::Command1, Command::CommandTAN, Command::CommandNULL };
        TestDriver::Test(L"0.01745506492821758576512889521973", L"tan\x2080(1)", commands12, true, true);

        Command commands13[] = { Command::Command1, Command::CommandASIN, Command::CommandNULL };
        TestDriver::Test(L"90", L"sin\x2080\x207B\x00B9(1)", commands13, true, true);

        Command commands14[] = { Command::Command1, Command::CommandACOS, Command::CommandNULL };
        TestDriver::Test(L"0", L"cos\x2080\x207B\x00B9(1)", commands14, true, true);

        Command commands15[] = { Command::Command1, Command::CommandATAN, Command::CommandNULL };
        TestDriver::Test(L"45", L"tan\x2080\x207B\x00B9(1)", commands15, true, true);

        Command commands16[] = { Command::Command2, Command::CommandPOWE, Command::CommandNULL };
        TestDriver::Test(L"7.389056098930650227230427460575", L"e^(2)", commands16, true, true);

        Command commands17[] = { Command::Command5, Command::CommandPWR, Command::Command0, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"1", L"5 ^ 0 + ", commands17, true, true);

        Command commands18[] = { Command::Command0, Command::CommandPWR, Command::Command0, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"1", L"0 ^ 0 + ", commands18, true, true);

        Command commands19[] = { Command::Command2, Command::Command7,   Command::CommandSIGN, Command::CommandROOT,
                                 Command::Command3, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"-3", L"-27 yroot 3 + ", commands19, true, true);

        Command commands20[] = { Command::Command8,   Command::CommandPWR, Command::CommandOPENP,  Command::Command2,
                                 Command::CommandDIV, Command::Command3,   Command::CommandCLOSEP, Command::CommandSUB,
                                 Command::Command4,   Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"8 ^ (2 \x00F7 3) - 4 + ", commands20, true, true);

        Command commands21[] = { Command::Command4,   Command::CommandPWR, Command::CommandOPENP,  Command::Command3,
                                 Command::CommandDIV, Command::Command2,   Command::CommandCLOSEP, Command::CommandSUB,
                                 Command::Command8,   Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"4 ^ (3 \x00F7 2) - 8 + ", commands21, true, true);

        Command commands22[] = { Command::Command1, Command::Command0, Command::CommandPWR, Command::Command1, Command::CommandPNT, Command::Command2,
                                 Command::Command3, Command::Command4, Command::Command5,   Command::Command6, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"17.161687912241792074207286679393", L"10 ^ 1.23456 + ", commands22, true, true);

        Command commands23[] = { Command::Command1, Command::CommandSEC, Command::CommandNULL };
        TestDriver::Test(L"1.0001523280439076654284264342126", L"sec\x2080(1)", commands23, true, true);

        Command commands24[] = { Command::Command1, Command::CommandCSC, Command::CommandNULL };
        TestDriver::Test(L"57.298688498550183476612683735174", L"csc\x2080(1)", commands24, true, true);

        Command commands25[] = { Command::Command1, Command::CommandCOT, Command::CommandNULL };
        TestDriver::Test(L"57.289961630759424687278147537113", L"cot\x2080(1)", commands25, true, true);

        Command commands26[] = { Command::Command1, Command::CommandASEC, Command::CommandNULL };
        TestDriver::Test(L"0", L"sec\x2080\x207B\x00B9(1)", commands26, true, true);

        Command commands27[] = { Command::Command1, Command::CommandACSC, Command::CommandNULL };
        TestDriver::Test(L"90", L"csc\x2080\x207B\x00B9(1)", commands27, true, true);

        Command commands28[] = { Command::Command1, Command::CommandACOT, Command::CommandNULL };
        TestDriver::Test(L"45", L"cot\x2080\x207B\x00B9(1)", commands28, true, true);

        Command commands29[] = { Command::Command1, Command::CommandSECH, Command::CommandNULL };
        TestDriver::Test(L"0.64805427366388539957497735322615", L"sech(1)", commands29, true, true);

        Command commands30[] = { Command::Command1, Command::CommandCSCH, Command::CommandNULL };
        TestDriver::Test(L"0.85091812823932154513384276328718", L"csch(1)", commands30, true, true);

        Command commands31[] = { Command::Command1, Command::CommandCOTH, Command::CommandNULL };
        TestDriver::Test(L"1.3130352854993313036361612469308", L"coth(1)", commands31, true, true);

        Command commands32[] = { Command::Command1, Command::CommandASECH, Command::CommandNULL };
        TestDriver::Test(L"0", L"sech\x207B\x00B9(1)", commands32, true, true);

        Command commands33[] = { Command::Command1, Command::CommandACSCH, Command::CommandNULL };
        TestDriver::Test(L"0.88137358701954302523260932497979", L"csch\x207B\x00B9(1)", commands33, true, true);

        Command commands34[] = { Command::Command2, Command::CommandACOTH, Command::CommandNULL };
        TestDriver::Test(L"0.54930614433405484569762261846126", L"coth\x207B\x00B9(2)", commands34, true, true);

        Command commands35[] = { Command::Command8, Command::CommandPOW2, Command::CommandNULL };
        TestDriver::Test(L"256", L"2^(8)", commands35, true, true);

        Command commands36[] = { Command::CommandRand, Command::CommandCeil, Command::CommandNULL };
        TestDriver::Test(L"1", L"N/A", commands36, true, true);

        Command commands37[] = { Command::CommandRand, Command::CommandFloor, Command::CommandNULL };
        TestDriver::Test(L"0", L"N/A", commands37, true, true);

        Command commands38[] = { Command::CommandRand, Command::CommandSIGN, Command::CommandCeil, Command::CommandNULL };
        TestDriver::Test(L"0", L"N/A", commands38, true, true);

        Command commands39[] = { Command::CommandRand, Command::CommandSIGN, Command::CommandFloor, Command::CommandNULL };
        TestDriver::Test(L"-1", L"N/A", commands39, true, true);

        Command commands40[] = { Command::Command3, Command::CommandPNT, Command::Command8, Command::CommandFloor, Command::CommandNULL };
        TestDriver::Test(L"3", L"floor(3.8)", commands40, true, true);

        Command commands41[] = { Command::Command3, Command::CommandPNT, Command::Command8, Command::CommandCeil, Command::CommandNULL };
        TestDriver::Test(L"4", L"ceil(3.8)", commands41, true, true);

        Command commands42[] = { Command::Command5, Command::CommandLogBaseY, Command::Command3, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"1.4649735207179271671970404076786", L"5 log base 3 + ", commands42, true, true);
    }
```

##### `CalculatorManagerTestScientificParenthesis` — `CalculatorManagerTest` — `CalculatorManagerTest.cpp:636-679`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestScientificParenthesis()
    {
        Command commands1[] = { Command::Command1, Command::CommandADD,    Command::CommandOPENP, Command::CommandADD,
                                Command::Command3, Command::CommandCLOSEP, Command::CommandNULL };
        TestDriver::Test(L"3", L"1 + (0 + 3)", commands1, true, true);

        Command commands2[] = {
            Command::CommandOPENP, Command::CommandOPENP, Command::Command1, Command::Command2, Command::CommandCLOSEP, Command::CommandNULL
        };
        TestDriver::Test(L"12", L"((12)", commands2, true, true);

        Command commands3[] = { Command::Command1,      Command::Command2,     Command::CommandCLOSEP,
                                Command::CommandCLOSEP, Command::CommandOPENP, Command::CommandNULL };
        TestDriver::Test(L"12", L"12 \x00D7 (", commands3, true, true);

        Command commands4[] = {
            Command::Command2, Command::CommandOPENP, Command::Command2, Command::CommandCLOSEP, Command::CommandADD, Command::CommandNULL
        };
        TestDriver::Test(L"4", L"2 \x00D7 (2) + ", commands4, true, true);

        Command commands5[] = { Command::Command2,   Command::CommandOPENP, Command::Command2,   Command::CommandCLOSEP,
                                Command::CommandADD, Command::CommandEQU,   Command::CommandNULL };
        TestDriver::Test(L"8", L"2 \x00D7 (2) + 4=", commands5, true, true);

        Command commands6[] = { Command::CommandOPENP, Command::Command8, Command::CommandCLOSEP, Command::Command2, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"16", L"(8) \x00D7 2=", commands6, true, true);

        Command commands7[] = { Command::CommandOPENP,  Command::Command7, Command::CommandMUL, Command::Command2,
                                Command::CommandCLOSEP, Command::Command2, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"28", L"(7 \x00D7 2) \x00D7 2=", commands7, true, true);

        Command commands8[] = { Command::CommandOPENP,  Command::Command7, Command::CommandMUL, Command::Command2,
                                Command::CommandCLOSEP, Command::Command2, Command::CommandEQU,
                                Command::CommandOPENP, Command::Command1, Command::Command4, Command::CommandCLOSEP, Command::Command2, Command::CommandEQU, Command::CommandNULL};
        TestDriver::Test(L"28", L"(14) \x00D7 2=", commands8, true, true);

        Command commands9[] = { Command::CommandOPENP, Command::Command8, Command::CommandCLOSEP,
                                Command::Command0, Command::CommandPNT, Command::Command5, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"4", L"(8) \x00D7 0.5=", commands9, true, true);

        Command commands10[] = { Command::CommandOPENP, Command::Command8, Command::CommandCLOSEP,
                                 Command::CommandPNT, Command::Command5, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"4", L"(8) \x00D7 0.5=", commands10, true, true);
    }
```

##### `CalculatorManagerTestScientificError` — `CalculatorManagerTest` — `CalculatorManagerTest.cpp:681-702`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestScientificError()
    {
        Command commands1[] = { Command::Command1, Command::CommandDIV, Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Cannot divide by zero", L"1 \x00F7 ", commands1, true, true);
        VERIFY_IS_TRUE(m_calculatorDisplayTester->GetIsError());

        Command commands2[] = { Command::Command2, Command::CommandSIGN, Command::CommandLOG, Command::CommandNULL };
        TestDriver::Test(L"Invalid input", L"log(-2)", commands2, true, true);
        VERIFY_IS_TRUE(m_calculatorDisplayTester->GetIsError());

        Command commands3[] = { Command::Command0, Command::CommandDIV, Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Result is undefined", L"0 \x00F7 ", commands3, true, true);
        VERIFY_IS_TRUE(m_calculatorDisplayTester->GetIsError());

        // Do the same tests for the basic calculator
        TestDriver::Test(L"Cannot divide by zero", L"1 \x00F7 ", commands1);
        VERIFY_IS_TRUE(m_calculatorDisplayTester->GetIsError());
        TestDriver::Test(L"Invalid input", L"log(-2)", commands2);
        VERIFY_IS_TRUE(m_calculatorDisplayTester->GetIsError());
        TestDriver::Test(L"Result is undefined", L"0 \x00F7 ", commands3);
        VERIFY_IS_TRUE(m_calculatorDisplayTester->GetIsError());
    }
```

##### `CalculatorManagerTestScientificModeChange` — `CalculatorManagerTest` — `CalculatorManagerTest.cpp:705-724`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestScientificModeChange()
    {
        Command commands1[] = { Command::CommandRAD, Command::CommandPI, Command::CommandSIN, Command::CommandNULL };
        TestDriver::Test(L"0", L"N/A", commands1, true, true);

        Command commands2[] = { Command::CommandRAD, Command::CommandPI, Command::CommandCOS, Command::CommandNULL };
        TestDriver::Test(L"-1", L"N/A", commands2, true, true);

        Command commands3[] = { Command::CommandRAD, Command::CommandPI, Command::CommandTAN, Command::CommandNULL };
        TestDriver::Test(L"0", L"N/A", commands3, true, true);

        Command commands4[] = { Command::CommandGRAD, Command::Command4, Command::Command0, Command::Command0, Command::CommandSIN, Command::CommandNULL };
        TestDriver::Test(L"0", L"N/A", commands4, true, true);

        Command commands5[] = { Command::CommandGRAD, Command::Command4, Command::Command0, Command::Command0, Command::CommandCOS, Command::CommandNULL };
        TestDriver::Test(L"1", L"N/A", commands5, true, true);

        Command commands6[] = { Command::CommandGRAD, Command::Command4, Command::Command0, Command::Command0, Command::CommandTAN, Command::CommandNULL };
        TestDriver::Test(L"0", L"N/A", commands6, true, true);
    }
```

##### `CalculatorManagerTestStandard` — `CalculatorManagerTest` — `CalculatorManagerTest.cpp:320-402`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestStandard()
    {
        Command commands1[] = { Command::Command1, Command::Command2, Command::Command3, Command::CommandPNT,
                                Command::Command4, Command::Command5, Command::Command6, Command::CommandNULL };
        TestDriver::Test(L"123.456", L"", commands1);

        Command commands2[] = { Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"0 + ", commands2);

        Command commands3[] = { Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(0)", commands3);

        Command commands4[] = { Command::Command2, Command::CommandADD, Command::Command3,   Command::CommandEQU,
                                Command::Command4, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"7", L"4 + 3=", commands4);

        Command commands5[] = { Command::Command4, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"4", L"4=", commands5);

        Command commands6[] = { Command::Command2,    Command::Command5,    Command::Command6,   Command::CommandSQRT,
                                Command::CommandSQRT, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"2", L"\x221A(\x221A(\x221A(256)))", commands6);

        Command commands7[] = { Command::Command3,   Command::CommandSUB, Command::Command6,   Command::CommandEQU,
                                Command::CommandMUL, Command::Command3,   Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"-9", L"-3 \x00D7 3=", commands7);

        Command commands8[] = { Command::Command9,     Command::CommandMUL, Command::Command6,   Command::CommandSUB,
                                Command::CommandCENTR, Command::Command8,   Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"46", L"54 - 8=", commands8);

        Command commands9[] = { Command::Command6, Command::CommandMUL, Command::Command6, Command::CommandPERCENT, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"0.36", L"6 \x00D7 0.06=", commands9);

        Command commands10[] = { Command::Command5, Command::Command0,       Command::CommandADD, Command::Command2,
                                 Command::Command0, Command::CommandPERCENT, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"60", L"50 + 10=", commands10);

        Command commands11[] = { Command::Command4, Command::CommandADD, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"8", L"4 + 4=", commands11);

        Command commands12[] = { Command::Command5, Command::CommandADD, Command::CommandMUL, Command::Command3, Command::CommandNULL };
        TestDriver::Test(L"3", L"5 \x00D7 ", commands12);

        Command commands13[] = { Command::Command1, Command::CommandEXP, Command::CommandSIGN, Command::Command9, Command::Command9,   Command::Command9,
                                 Command::Command9, Command::CommandDIV, Command::Command1,    Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Overflow", L"1.e-9999 \x00F7 ", commands13);

        Command commands14[] = { Command::Command5, Command::Command0,       Command::CommandADD, Command::Command2,
                                 Command::Command0, Command::CommandPERCENT, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"60", L"50 + 10=", commands14);

        Command commands15[] = { Command::Command0, Command::CommandDIV, Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Result is undefined", L"0 \x00F7 ", commands15);

        Command commands16[] = { Command::Command1, Command::CommandDIV, Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Cannot divide by zero", L"1 \x00F7 ", commands16);

        Command commands17[] = { Command::Command1,     Command::Command2, Command::CommandADD, Command::Command5,
                                 Command::CommandCENTR, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"14", L"14 + ", commands17);

        Command commands18[] = { Command::Command1, Command::Command0, Command::Command0, Command::CommandSIGN, Command::CommandREC, Command::CommandNULL };
        TestDriver::Test(L"-0.01", L"1/(-100)", commands18);

        Command commands19[] = { Command::Command1, Command::Command2, Command::Command3, Command::CommandBACK, Command::CommandBACK, Command::CommandNULL };
        TestDriver::Test(L"1", L"", commands19);

        Command commands20[] = { Command::Command1,    Command::Command2,    Command::Command3,   Command::CommandBACK,
                                 Command::CommandBACK, Command::CommandBACK, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands20);

        Command commands21[] = { Command::Command4, Command::CommandSQRT, Command::CommandSUB, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"0 + ", commands21);

        Command commands22[] = { Command::Command1,   Command::Command0, Command::Command2, Command::Command4,   Command::CommandSQRT,
                                 Command::CommandSUB, Command::Command3, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"0 + ", commands22);

        Command commands23[] = { Command::Command2, Command::CommandPNT, Command::Command2, Command::Command5,   Command::CommandSQRT, Command::CommandSUB,
                                 Command::Command1, Command::CommandPNT, Command::Command5, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(2.25) - 1.5=", commands23);
    }
```

##### `CalculatorManagerTestStandardOrderOfOperations` — `CalculatorManagerTest` — `CalculatorManagerTest.cpp:1136-1211`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestStandardOrderOfOperations()
    {
        Command commands1[] = { Command::Command1, Command::CommandREC, Command::CommandNULL };
        TestDriver::Test(L"1", L"1/(1)", commands1);

        Command commands2[] = { Command::Command4, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"2", L"\x221A(4)", commands2);

        Command commands3[] = { Command::Command1, Command::CommandADD, Command::Command4, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"2", L"1 + \x221A(4)", commands3);

        Command commands4[] = { Command::Command1, Command::CommandADD, Command::Command4, Command::CommandSQRT, Command::CommandSUB, Command::CommandNULL };
        TestDriver::Test(L"3", L"3 - ", commands4);

        Command commands5[] = { Command::Command2, Command::CommandMUL, Command::Command4, Command::CommandREC, Command::CommandNULL };
        TestDriver::Test(L"0.25", L"2 \x00D7 1/(4)", commands5);

        Command commands6[] = { Command::Command5, Command::CommandDIV, Command::Command6, Command::CommandPERCENT, Command::CommandNULL };
        TestDriver::Test(L"0.06", L"5 \x00F7 0.06", commands6);

        Command commands7[] = { Command::Command4, Command::CommandSQRT, Command::CommandSUB, Command::CommandNULL };
        TestDriver::Test(L"2", L"\x221A(4) - ", commands7);

        Command commands8[] = { Command::Command7, Command::CommandSQR, Command::CommandDIV, Command::CommandNULL };
        TestDriver::Test(L"49", L"sqr(7) \x00F7 ", commands8);

        Command commands9[] = { Command::Command8, Command::CommandSQR, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"8", L"\x221A(sqr(8))", commands9);

        Command commands10[] = { Command::Command1, Command::Command0, Command::CommandADD, Command::Command2, Command::CommandSUB, Command::CommandNULL };
        TestDriver::Test(L"12", L"12 - ", commands10);

        Command commands11[] = { Command::Command3, Command::CommandMUL, Command::Command4, Command::CommandDIV, Command::CommandNULL };
        TestDriver::Test(L"12", L"12 \x00F7 ", commands11);

        Command commands12[] = { Command::Command6, Command::CommandDIV, Command::Command3, Command::CommandSUB, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"2", L"2 + ", commands12);

        Command commands13[] = { Command::Command7, Command::CommandSUB, Command::Command4, Command::CommandDIV, Command::CommandMUL, Command::CommandNULL };
        TestDriver::Test(L"3", L"3 \x00D7 ", commands13);

        Command commands14[] = { Command::Command8, Command::CommandMUL, Command::Command2, Command::CommandADD, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"4", L"16 + \x221A(16)", commands14);

        Command commands15[] = { Command::Command9, Command::CommandADD, Command::Command0, Command::CommandMUL, Command::CommandSIGN, Command::CommandNULL };
        TestDriver::Test(L"-9", L"9 \x00D7 negate(9)", commands15);

        Command commands16[] = { Command::Command9, Command::CommandSIGN, Command::Command0, Command::CommandADD, Command::CommandMUL, Command::CommandNULL };
        TestDriver::Test(L"-90", L"-90 \x00D7 ", commands16);

        Command commands17[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"3", L"1 + 2=", commands17);

        Command commands18[] = { Command::Command2, Command::Command0,   Command::CommandMUL, Command::Command0,
                                 Command::Command2, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"40", L"20 \x00D7 2=", commands18);

        Command commands19[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandADD, Command::CommandBACK, Command::CommandNULL };
        TestDriver::Test(L"3", L"3 + ", commands19);

        Command commands20[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandADD, Command::CommandCLEAR, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands20);

        Command commands21[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandADD, Command::CommandCENTR, Command::CommandNULL };
        TestDriver::Test(L"0", L"3 + ", commands21);

        Command commands22[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandCLEAR, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands22);

        Command commands23[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandCENTR, Command::CommandNULL };
        TestDriver::Test(L"0", L"1 + ", commands23);

        Command commands24[] = { Command::Command1, Command::CommandMUL, Command::Command2, Command::CommandMUL, Command::Command3,   Command::CommandMUL,
                                 Command::Command4, Command::CommandMUL, Command::Command5, Command::CommandMUL, Command::CommandNULL };
        TestDriver::Test(L"120", L"120 \x00D7 ", commands24);
    }
```

##### `CalculatorManagerTestModeChange` — `CalculatorManagerTest` — `CalculatorManagerTest.cpp:726-751`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestModeChange()
    {
        Command commands1[] = { Command::Command1, Command::Command2, Command::Command3, Command::CommandNULL };
        TestDriver::Test(L"123", L"", commands1, true, false);

        Command commands2[] = { Command::ModeScientific, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands2, true, false);

        Command commands3[] = { Command::Command1, Command::Command2, Command::Command3, Command::CommandNULL };
        TestDriver::Test(L"123", L"", commands3, true, false);

        Command commands4[] = { Command::ModeProgrammer, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands4, true, false);

        Command commands5[] = { Command::Command1, Command::Command2, Command::Command3, Command::CommandNULL };
        TestDriver::Test(L"123", L"", commands5, true, false);

        Command commands6[] = { Command::ModeScientific, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands6, true, false);

        Command commands7[] = { Command::Command6, Command::Command7, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"67", L"67 + ", commands7, true, false);

        Command commands8[] = { Command::ModeBasic, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands8, true, false);
    }
```

##### `CalculatorManagerTestProgrammer` — `CalculatorManagerTest` — `CalculatorManagerTest.cpp:753-813`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestProgrammer()
    {
        Command commands1[] = { Command::ModeProgrammer, Command::Command5, Command::Command3,   Command::CommandNand,
                                Command::Command8,       Command::Command3, Command::CommandAnd, Command::CommandNULL };
        TestDriver::Test(L"-18", L"53 NAND 83 AND ", commands1, true, false);

        Command commands2[] = { Command::ModeProgrammer, Command::Command5, Command::Command3,   Command::CommandNor,
                                Command::Command8,       Command::Command3, Command::CommandAnd, Command::CommandNULL };
        TestDriver::Test(L"-120", L"53 NOR 83 AND ", commands2, true, false);

        Command commands3[] = {
            Command::ModeProgrammer, Command::Command5, Command::CommandLSHF, Command::Command1, Command::CommandAnd, Command::CommandNULL
        };
        TestDriver::Test(L"10", L"5 Lsh 1 AND ", commands3, true, false);

        Command commands5[] = {
            Command::ModeProgrammer, Command::Command5, Command::CommandRSHFL, Command::Command1, Command::CommandAnd, Command::CommandNULL
        };
        TestDriver::Test(L"2", L"5 Rsh 1 AND ", commands5, true, false);

        Command commands6[] = { Command::ModeProgrammer, Command::CommandBINPOS63, Command::CommandRSHF, Command::Command5,
                                Command::Command6,       Command::CommandAnd,      Command::CommandNULL };
        TestDriver::Test(L"-128", L"-9223372036854775808 Rsh 56 AND ", commands6, true, false);

        Command commands7[] = { Command::ModeProgrammer, Command::Command1, Command::CommandROL, Command::CommandNULL };
        TestDriver::Test(L"2", L"RoL(1)", commands7, true, false);

        Command commands8[] = { Command::ModeProgrammer, Command::Command1, Command::CommandROR, Command::CommandNULL };
        TestDriver::Test(L"-9,223,372,036,854,775,808", L"RoR(1)", commands8, true, false);

        Command commands9[] = { Command::ModeProgrammer, Command::Command1, Command::CommandRORC, Command::CommandNULL };
        TestDriver::Test(L"0", L"RoR(1)", commands9, true, false);

        Command commands10[] = { Command::ModeProgrammer, Command::Command1, Command::CommandRORC, Command::CommandRORC, Command::CommandNULL };
        TestDriver::Test(L"-9,223,372,036,854,775,808", L"RoR(RoR(1))", commands10, true, false);

        Command commands11[] = { Command::ModeProgrammer, Command::CommandDec, Command::Command4, Command::Command2, Command::Command9,   Command::Command4,
                                Command::Command9,       Command::Command6,   Command::Command7, Command::Command2, Command::Command9,   Command::Command6,
                                Command::CommandDIV,     Command::Command2,   Command::Command5, Command::Command5, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"16,843,009", L"4294967296 \x00F7 255=", commands11, true, false);

        Command commands12[] = {
            Command::ModeProgrammer, Command::CommandDec, Command::Command4, Command::Command2, Command::Command9,   Command::Command4,
            Command::Command9,       Command::Command6,   Command::Command7, Command::Command3, Command::Command0,   Command::Command3,
            Command::CommandDIV,     Command::Command2,   Command::Command5, Command::Command5, Command::CommandEQU, Command::CommandNULL
        };
        TestDriver::Test(L"16,843,009", L"4294967303 \x00F7 255=", commands12, true, false);

        Command commands13[] = {
            Command::ModeProgrammer, Command::CommandDec, Command::Command1, Command::Command0, Command::Command0, Command::Command0,
            Command::Command0, Command::Command0, Command::Command0, Command::Command0, Command::Command0, Command::Command0, Command::CommandDIV,
            Command::Command6, Command::Command4, Command::Command4, Command::Command8, Command::Command7, Command::CommandEQU, Command::CommandNULL
        };
        TestDriver::Test(L"15,507", L"1000000000 \x00F7 64487=", commands13, true, false);

        Command commands14[] = { Command::ModeProgrammer, Command::CommandDec, Command::Command1,   Command::Command0,   Command::Command0,
                                 Command::Command0,       Command::Command0,   Command::Command0,   Command::Command0,   Command::Command0,
                                 Command::Command0,       Command::Command0,   Command::CommandDIV, Command::Command6,   Command::Command4,
                                 Command::Command4,       Command::Command8,   Command::Command8,   Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"15,506", L"1000000000 \x00F7 64488=", commands14, true, false);
    }
```

#### 5.2.2 `TEST_CLASS(RationalTest)` — exact rational modulo / remainder

Note `TEST_CLASS_INITIALIZE(CommonSetup){ ChangeConstants(10, 128); }` — base 10, word size 128 bits.

##### `TestModuloOperandsNotModified` — `RationalTest` — `RationalTest.cpp:18-41`

```cpp
TEST_METHOD(TestModuloOperandsNotModified)
{
    // Verify results but also check that operands are not modified
    Rational rat25(25);
    Rational ratminus25(-25);
    Rational rat4(4);
    Rational ratminus4(-4);
    Rational res = Mod(rat25, rat4);
    VERIFY_ARE_EQUAL(res, 1);
    VERIFY_ARE_EQUAL(rat25, 25);
    VERIFY_ARE_EQUAL(rat4, 4);
    res = Mod(rat25, ratminus4);
    VERIFY_ARE_EQUAL(res, -3);
    VERIFY_ARE_EQUAL(rat25, 25);
    VERIFY_ARE_EQUAL(ratminus4, -4);
    res = Mod(ratminus25, ratminus4);
    VERIFY_ARE_EQUAL(res, -1);
    VERIFY_ARE_EQUAL(ratminus25, -25);
    VERIFY_ARE_EQUAL(ratminus4, -4);
    res = Mod(ratminus25, rat4);
    VERIFY_ARE_EQUAL(res, 3);
    VERIFY_ARE_EQUAL(ratminus25, -25);
    VERIFY_ARE_EQUAL(rat4, 4);
}
```

##### `TestModuloInteger` — `RationalTest` — `RationalTest.cpp:43-60`

```cpp
TEST_METHOD(TestModuloInteger)
{
    // Check with integers
    auto res = Mod(Rational(426), Rational(56478));
    VERIFY_ARE_EQUAL(res, 426);
    res = Mod(Rational(56478), Rational(426));
    VERIFY_ARE_EQUAL(res, 246);
    res = Mod(Rational(-643), Rational(8756));
    VERIFY_ARE_EQUAL(res, 8113);
    res = Mod(Rational(643), Rational(-8756));
    VERIFY_ARE_EQUAL(res, -8113);
    res = Mod(Rational(-643), Rational(-8756));
    VERIFY_ARE_EQUAL(res, -643);
    res = Mod(Rational(1000), Rational(250));
    VERIFY_ARE_EQUAL(res, 0);
    res = Mod(Rational(1000), Rational(-250));
    VERIFY_ARE_EQUAL(res, 0);
}
```

##### `TestModuloZero` — `RationalTest` — `RationalTest.cpp:62-75`

```cpp
TEST_METHOD(TestModuloZero)
{
    // Test with Zero
    auto res = Mod(Rational(343654332), Rational(0));
    VERIFY_ARE_EQUAL(res, 343654332);
    res = Mod(Rational(0), Rational(8756));
    VERIFY_ARE_EQUAL(res, 0);
    res = Mod(Rational(0), Rational(-242));
    VERIFY_ARE_EQUAL(res, 0);
    res = Mod(Rational(0), Rational(0));
    VERIFY_ARE_EQUAL(res, 0);
    res = Mod(Rational(Number(1, 0, { 23242 }), Number(1, 0, { 2 })), Rational(Number(1, 0, { 0 }), Number(1, 0, { 23 })));
    VERIFY_ARE_EQUAL(res, 11621);
}
```

##### `TestModuloRational` — `RationalTest` — `RationalTest.cpp:77-100`

```cpp
TEST_METHOD(TestModuloRational)
{
    // Test with rational numbers
    auto res = Mod(Rational(Number(1, 0, { 250 }), Number(1, 0, { 100 })), Rational(89));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"2.5");
    res = Mod(Rational(Number(1, 0, { 3330 }), Number(1, 0, { 1332 })), Rational(1));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"0.5");
    res = Mod(Rational(Number(1, 0, { 12250 }), Number(1, 0, { 100 })), Rational(10));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"2.5");
    res = Mod(Rational(Number(-1, 0, { 12250 }), Number(1, 0, { 100 })), Rational(10));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"7.5");
    res = Mod(Rational(Number(-1, 0, { 12250 }), Number(1, 0, { 100 })), Rational(-10));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-2.5");
    res = Mod(Rational(Number(1, 0, { 12250 }), Number(1, 0, { 100 })), Rational(-10));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-7.5");
    res = Mod(Rational(Number(1, 0, { 1000 }), Number(1, 0, { 3 })), Rational(1));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"0.33333333");
    res = Mod(Rational(Number(1, 0, { 1000 }), Number(1, 0, { 3 })), Rational(-10));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-6.6666667");
    res = Mod(Rational(834345), Rational(Number(1, 0, { 103 }), Number(1, 0, { 100 })));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"0.71");
    res = Mod(Rational(834345), Rational(Number(-1, 0, { 103 }), Number(1, 0, { 100 })));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-0.32");
}
```

##### `TestRemainderOperandsNotModified` — `RationalTest` — `RationalTest.cpp:102-125`

```cpp
TEST_METHOD(TestRemainderOperandsNotModified)
{
    // Verify results but also check that operands are not modified
    Rational rat25(25);
    Rational ratminus25(-25);
    Rational rat4(4);
    Rational ratminus4(-4);
    Rational res = rat25 % rat4;
    VERIFY_ARE_EQUAL(res, 1);
    VERIFY_ARE_EQUAL(rat25, 25);
    VERIFY_ARE_EQUAL(rat4, 4);
    res = rat25 % ratminus4;
    VERIFY_ARE_EQUAL(res, 1);
    VERIFY_ARE_EQUAL(rat25, 25);
    VERIFY_ARE_EQUAL(ratminus4, -4);
    res = ratminus25 % ratminus4;
    VERIFY_ARE_EQUAL(res, -1);
    VERIFY_ARE_EQUAL(ratminus25, -25);
    VERIFY_ARE_EQUAL(ratminus4, -4);
    res = ratminus25 % rat4;
    VERIFY_ARE_EQUAL(res, -1);
    VERIFY_ARE_EQUAL(ratminus25, -25);
    VERIFY_ARE_EQUAL(rat4, 4);
}
```

##### `TestRemainderInteger` — `RationalTest` — `RationalTest.cpp:127-144`

```cpp
TEST_METHOD(TestRemainderInteger)
{
    // Check with integers
    auto res = Rational(426) % Rational(56478);
    VERIFY_ARE_EQUAL(res, 426);
    res = Rational(56478) % Rational(426);
    VERIFY_ARE_EQUAL(res, 246);
    res = Rational(-643) % Rational(8756);
    VERIFY_ARE_EQUAL(res, -643);
    res = Rational(643) % Rational(-8756);
    VERIFY_ARE_EQUAL(res, 643);
    res = Rational(-643) % Rational(-8756);
    VERIFY_ARE_EQUAL(res, -643);
    res = Rational(-124) % Rational(-124);
    VERIFY_ARE_EQUAL(res, 0);
    res = Rational(24) % Rational(24);
    VERIFY_ARE_EQUAL(res, 0);
}
```

##### `TestRemainderZero` — `RationalTest` — `RationalTest.cpp:146-189`

```cpp
TEST_METHOD(TestRemainderZero)
{
    // Test with Zero
    auto res = Rational(0) % Rational(3654);
    VERIFY_ARE_EQUAL(res, 0);
    res = Rational(0) % Rational(-242);
    VERIFY_ARE_EQUAL(res, 0);
    for (auto number : { 343654332, 0, -23423 })
    {
        try
        {
            res = Rational(number) % Rational(0);
            Assert::Fail();
        }
        catch (uint32_t t)
        {
            if (t != CALC_E_INDEFINITE)
            {
                Assert::Fail();
            }
        }
        catch (...)
        {
            Assert::Fail();
        }

        try
        {
            res = Rational(Number(1, number, { 0 }), Number(1, 0, { 2 })) % Rational(Number(1, 0, { 0 }), Number(1, 0, { 23 }));
            Assert::Fail();
        }
        catch (uint32_t t)
        {
            if (t != CALC_E_INDEFINITE)
            {
                Assert::Fail();
            }
        }
        catch (...)
        {
            Assert::Fail();
        }
    }
}
```

##### `TestRemainderRational` — `RationalTest` — `RationalTest.cpp:191-218`

```cpp
TEST_METHOD(TestRemainderRational)
{
    // Test with rational numbers
    auto res = Rational(Number(1, 0, { 250 }), Number(1, 0, { 100 })) % Rational(89);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"2.5");
    res = Rational(Number(1, 0, { 3330 }), Number(1, 0, { 1332 })) % Rational(1);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"0.5");
    res = Rational(Number(1, 0, { 12250 }), Number(1, 0, { 100 })) % Rational(10);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"2.5");
    res = Rational(Number(-1, 0, { 12250 }), Number(1, 0, { 100 })) % Rational(10);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-2.5");
    res = Rational(Number(-1, 0, { 12250 }), Number(1, 0, { 100 })) % Rational(-10);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-2.5");
    res = Rational(Number(1, 0, { 12250 }), Number(1, 0, { 100 })) % Rational(-10);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"2.5");
    res = Rational(Number(1, 0, { 1000 }), Number(1, 0, { 3 })) % Rational(1);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"0.33333333");
    res = Rational(Number(1, 0, { 1000 }), Number(1, 0, { 3 })) % Rational(-10);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"3.3333333");
    res = Rational(Number(-1, 0, { 1000 }), Number(1, 0, { 3 })) % Rational(-10);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-3.3333333");
    res = Rational(834345) % Rational(Number(1, 0, { 103 }), Number(1, 0, { 100 }));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"0.71");
    res = Rational(834345) % Rational(Number(-1, 0, { 103 }), Number(1, 0, { 100 }));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"0.71");
    res = Rational(-834345) % Rational(Number(1, 0, { 103 }), Number(1, 0, { 100 }));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-0.71");
}
```

#### 5.2.3 Scientific / programmer vectors from the managed unit tests

##### `MultipleModesCalculationTest` — `MultiWindowUnitTests` — `MultiWindowTests.cs:146-198`

```csharp
        [TestMethod]
        public void MultipleModesCalculationTest()
        {
            var viewModels = new StandardCalculatorViewModel[3];

            for (int i = 0; i < 3; i++)
            {
                viewModels[i] = new StandardCalculatorViewModel();
                ChangeMode(viewModels[i], i);
            }

            // Standard Mode: 1+2=3
            var standardItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Equals, "3", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[0], standardItems, true);

            // Scientific Mode: 1+2*3=7
            var scientificItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Multiply, "2", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Equals, "7", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[1], scientificItems, true);

            // Programmer Mode: Hex F = 15
            var programmerItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[2], programmerItems, false);
            Assert.AreEqual("F", viewModels[2].HexDisplayValue);
            Assert.AreEqual("15", viewModels[2].DecimalDisplayValue);
            Assert.AreEqual("17", viewModels[2].OctalDisplayValue);
            Assert.AreEqual("1111", viewModels[2].BinaryDisplayValue);

            // Assert Standard and Scientific display values unchanged
            Assert.AreEqual("3", viewModels[0].DisplayValue);
            Assert.AreEqual("7", viewModels[1].DisplayValue);
        }
```

##### `MultipleScientificModeCalculationTest` — `MultiWindowUnitTests` — `MultiWindowTests.cs:237-272`

```csharp
        [TestMethod]
        public void MultipleScientificModeCalculationTest()
        {
            var scientificViewModel1 = new StandardCalculatorViewModel();
            var scientificViewModel2 = new StandardCalculatorViewModel();

            ChangeMode(scientificViewModel1, 1);
            ChangeMode(scientificViewModel2, 1);

            ValidateViewModelMode(scientificViewModel1, 1);
            ValidateViewModelMode(scientificViewModel2, 1);

            // Scientific 1: 3-2=1
            var items1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Three, "3", ""),
                new(NumbersAndOperatorsEnum.Subtract, "3", "3 - "),
                new(NumbersAndOperatorsEnum.Two, "2", "3 - "),
                new(NumbersAndOperatorsEnum.Equals, "1", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(scientificViewModel1, items1, true);

            // Scientific 2: 4*5=20
            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Four, "4", ""),
                new(NumbersAndOperatorsEnum.Multiply, "4", "4 * "),
                new(NumbersAndOperatorsEnum.Five, "5", "4 * "),
                new(NumbersAndOperatorsEnum.Equals, "20", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(scientificViewModel2, items2, true);

            Assert.AreEqual("1", scientificViewModel1.DisplayValue);
        }
```

##### `MultipleScientificModeWithDifferentSettingsTest` — `MultiWindowUnitTests` — `MultiWindowTests.cs:274-321`

```csharp
        [TestMethod]
        public void MultipleScientificModeWithDifferentSettingsTest()
        {
            var scientificViewModel1 = new StandardCalculatorViewModel();
            var scientificViewModel2 = new StandardCalculatorViewModel();

            ChangeMode(scientificViewModel1, 1);
            ChangeMode(scientificViewModel2, 1);

            ValidateViewModelMode(scientificViewModel1, 1);
            ValidateViewModelMode(scientificViewModel2, 1);

            // Scientific 1: Degrees with HYP checked
            var initItems1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Degree, "0", ""),
                new(NumbersAndOperatorsEnum.Hyp, "0", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(scientificViewModel1, initItems1, true);

            // Scientific 2: Radians with F-E checked
            var initItems2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Radians, "0", ""),
                new(NumbersAndOperatorsEnum.FToE, "0.e+0", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(scientificViewModel2, initItems2, true);

            // Scientific 1: CosH(0 degrees)
            var testItems1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Zero, "0", ""),
                new(NumbersAndOperatorsEnum.Cosh, "1", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(scientificViewModel1, testItems1, true);

            // Scientific 2: Cos(pi radians)
            var testItems2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Pi, "3.1415926535897932384626433832795e+0", ""),
                new(NumbersAndOperatorsEnum.Cos, "-1.e+0", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(scientificViewModel2, testItems2, true);
        }
```

##### `MultipleProgrammerModeCalculationTest` — `MultiWindowUnitTests` — `MultiWindowTests.cs:323-367`

```csharp
        [TestMethod]
        public void MultipleProgrammerModeCalculationTest()
        {
            var programmerViewModel1 = new StandardCalculatorViewModel();
            var programmerViewModel2 = new StandardCalculatorViewModel();

            ChangeMode(programmerViewModel1, 2);
            ChangeMode(programmerViewModel2, 2);

            ValidateViewModelMode(programmerViewModel1, 2);
            ValidateViewModelMode(programmerViewModel2, 2);

            // Programmer 1: Hex F
            var items1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(programmerViewModel1, items1, false);
            Assert.AreEqual("F", programmerViewModel1.HexDisplayValue);
            Assert.AreEqual("15", programmerViewModel1.DecimalDisplayValue);
            Assert.AreEqual("17", programmerViewModel1.OctalDisplayValue);
            Assert.AreEqual("1111", programmerViewModel1.BinaryDisplayValue);

            // Programmer 2: Oct 7
            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.OctButton, "0", ""),
                new(NumbersAndOperatorsEnum.Seven, "7", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(programmerViewModel2, items2, false);
            Assert.AreEqual("7", programmerViewModel2.HexDisplayValue);
            Assert.AreEqual("7", programmerViewModel2.DecimalDisplayValue);
            Assert.AreEqual("7", programmerViewModel2.OctalDisplayValue);
            Assert.AreEqual("0111", programmerViewModel2.BinaryDisplayValue);

            // Assert 1st instance unchanged
            Assert.AreEqual("F", programmerViewModel1.DisplayValue);
            Assert.AreEqual("F", programmerViewModel1.HexDisplayValue);
            Assert.AreEqual("15", programmerViewModel1.DecimalDisplayValue);
            Assert.AreEqual("17", programmerViewModel1.OctalDisplayValue);
            Assert.AreEqual("1111", programmerViewModel1.BinaryDisplayValue);
        }
```

##### `MultipleProgrammerModeWithDifferentSettingsTest` — `MultiWindowUnitTests` — `MultiWindowTests.cs:369-431`

```csharp
        [TestMethod]
        public void MultipleProgrammerModeWithDifferentSettingsTest()
        {
            var programmerViewModel1 = new StandardCalculatorViewModel();
            var programmerViewModel2 = new StandardCalculatorViewModel();

            ChangeMode(programmerViewModel1, 2);
            ChangeMode(programmerViewModel2, 2);

            ValidateViewModelMode(programmerViewModel1, 2);
            ValidateViewModelMode(programmerViewModel2, 2);

            // Programmer 1: Byte & Hex
            var initItems1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Byte, "0", ""),
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(programmerViewModel1, initItems1, false);

            // Programmer 2: Word & Oct
            var initItems2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Word, "0", ""),
                new(NumbersAndOperatorsEnum.OctButton, "0", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(programmerViewModel2, initItems2, false);

            // Programmer 1: FF (byte max)
            var testItems1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.F, "FF", ""),
                // One more F shouldn't have any effect, testing for precision
                new(NumbersAndOperatorsEnum.F, "FF", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(programmerViewModel1, testItems1, false);
            Assert.AreEqual("FF", programmerViewModel1.HexDisplayValue);
            Assert.AreEqual("-1", programmerViewModel1.DecimalDisplayValue);
            Assert.AreEqual("377", programmerViewModel1.OctalDisplayValue);
            Assert.AreEqual("1111 1111", programmerViewModel1.BinaryDisplayValue);

            // Programmer 2: 77777 (word, octal)
            var testItems2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Seven, "7", ""),
                new(NumbersAndOperatorsEnum.Seven, "77", ""),
                new(NumbersAndOperatorsEnum.Seven, "777", ""),
                new(NumbersAndOperatorsEnum.Seven, "7 777", ""),
                new(NumbersAndOperatorsEnum.Seven, "77 777", ""),
                // One more '7' shouldn't have any effect, testing for precision
                new(NumbersAndOperatorsEnum.Seven, "77 777", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(programmerViewModel2, testItems2, false);
            Assert.AreEqual("7FFF", programmerViewModel2.HexDisplayValue);
            Assert.AreEqual("32,767", programmerViewModel2.DecimalDisplayValue);
            Assert.AreEqual("77 777", programmerViewModel2.OctalDisplayValue);
            Assert.AreEqual("0111 1111 1111 1111", programmerViewModel2.BinaryDisplayValue);
        }
```

##### `MultipleModesHistoryAddItemTest` — `MultiWindowUnitTests` — `MultiWindowTests.cs:433-481`

```csharp
        [TestMethod]
        public void MultipleModesHistoryAddItemTest()
        {
            var viewModels = new StandardCalculatorViewModel[2];

            for (int i = 0; i < 2; i++)
            {
                viewModels[i] = new StandardCalculatorViewModel();
                ChangeMode(viewModels[i], i);
                Assert.AreEqual(0, viewModels[i].HistoryVM.ItemsCount);
            }

            // Standard Mode: 1+2=3
            var standardItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Equals, "3", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[0], standardItems, true);

            // Scientific Mode: 1+2*3=7
            var scientificItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Multiply, "2", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Equals, "7", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[1], scientificItems, true);

            // Assert history for Standard instance
            Assert.AreEqual(1, viewModels[0].HistoryVM.ItemsCount);
            var item1 = (HistoryItemViewModel)viewModels[0].HistoryVM.Items[0];
            Assert.AreEqual("1   +   2 =", item1.Expression);
            Assert.AreEqual("3", item1.Result);

            // Assert history for Scientific instance
            Assert.AreEqual(1, viewModels[1].HistoryVM.ItemsCount);
            var item2 = (HistoryItemViewModel)viewModels[1].HistoryVM.Items[0];
            Assert.AreEqual("1   +   2   \u00D7   3 =", item2.Expression);
            Assert.AreEqual("7", item2.Result);
        }
```

##### `MultipleScientificModesHistoryAddItemTest` — `MultiWindowUnitTests` — `MultiWindowTests.cs:546-594`

```csharp
        [TestMethod]
        public void MultipleScientificModesHistoryAddItemTest()
        {
            var viewModels = new StandardCalculatorViewModel[2];

            for (int i = 0; i < 2; i++)
            {
                viewModels[i] = new StandardCalculatorViewModel();
                ChangeMode(viewModels[i], 1);
                Assert.AreEqual(0, viewModels[i].HistoryVM.ItemsCount);
            }

            // Instance 0: 1+2=3
            var items0 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Equals, "3", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[0], items0, true);

            // Instance 1: 1+2*3=7
            var items1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Multiply, "2", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Equals, "7", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[1], items1, true);

            string[] expectedExpressions = { "1   +   2 =", "1   +   2   \u00D7   3 =" };
            string[] expectedResults = { "3", "7" };

            for (int i = 0; i < 2; i++)
            {
                Assert.AreEqual(1, viewModels[i].HistoryVM.ItemsCount);
                var item = (HistoryItemViewModel)viewModels[i].HistoryVM.Items[0];
                Assert.AreEqual(expectedExpressions[i], item.Expression);
                Assert.AreEqual(expectedResults[i], item.Result);
            }
        }
```

##### `MultipleModesMemoryAddItemTest` — `MultiWindowUnitTests` — `MultiWindowTests.cs:596-656`

```csharp
        [TestMethod]
        public void MultipleModesMemoryAddItemTest()
        {
            var viewModels = new StandardCalculatorViewModel[3];

            for (int i = 0; i < 3; i++)
            {
                viewModels[i] = new StandardCalculatorViewModel();
                ChangeMode(viewModels[i], i);
                Assert.AreEqual(0, viewModels[i].MemorizedNumbers.Count);
            }

            // Standard Mode: 1+2=3
            var standardItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Equals, "3", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[0], standardItems, true);

            // Scientific Mode: 1+2*3=7
            var scientificItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Multiply, "2", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Equals, "7", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[1], scientificItems, true);

            // Programmer Mode: Hex F
            var programmerItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[2], programmerItems, false);

            // Press Memory button to save values
            for (int i = 0; i < 3; i++)
            {
                viewModels[i].ButtonPressed.Execute(NumbersAndOperatorsEnum.Memory);
            }

            string[] expectedMemoryValues = { "3", "7", "F" };

            for (int i = 0; i < 3; i++)
            {
                Assert.AreEqual(1, viewModels[i].MemorizedNumbers.Count);
                var memorySlot = viewModels[i].MemorizedNumbers[0];
                Assert.AreEqual(expectedMemoryValues[i], memorySlot.Value);
            }
        }
```

##### `MultipleModesWithChangeInProgrammerRadix` — `MultiWindowUnitTests` — `MultiWindowTests.cs:749-811`

```csharp
        [TestMethod]
        public void MultipleModesWithChangeInProgrammerRadix()
        {
            var viewModels = new StandardCalculatorViewModel[3];

            // Standard mode
            viewModels[0] = new StandardCalculatorViewModel();
            ChangeMode(viewModels[0], 0);

            // Programmer mode
            viewModels[1] = new StandardCalculatorViewModel();
            ChangeMode(viewModels[1], 2);

            // Change programmer radix to hex and enter F
            var programmerItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[1], programmerItems, false);
            Assert.AreEqual("F", viewModels[1].HexDisplayValue);

            // Standard Mode: 10+2=12 (radix should be decimal, not hex)
            var standardItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Add, "10", "10 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "10 + "),
                new(NumbersAndOperatorsEnum.Equals, "12", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[0], standardItems, false);

            // Launch a new Standard instance
            viewModels[2] = new StandardCalculatorViewModel();
            ChangeMode(viewModels[2], 0);

            // New Standard: 10+2=12
            var standardItemsNew = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Add, "10", "10 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "10 + "),
                new(NumbersAndOperatorsEnum.Equals, "12", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[2], standardItemsNew, false);

            // Programmer radix should still be hex: A+1=B
            var programmerItemsNew = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.A, "A", ""),
                new(NumbersAndOperatorsEnum.Add, "A", "A + "),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Equals, "B", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[1], programmerItemsNew, true);
            Assert.AreEqual("B", viewModels[1].HexDisplayValue);
        }
```

##### `ProgrammerModeAutoConvertedValue` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:598-623`

```csharp
        [TestMethod]
        public void ProgrammerModeAutoConvertedValue()
        {
            var none = new TestItem[] { new(NumbersAndOperatorsEnum.None, "", "") };
            ValidateViewModelByCommands(_viewModel, none, true);
            _viewModel.IsProgrammer = true;

            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, false);
            Assert.AreEqual("F", _viewModel.HexDisplayValue);
            Assert.AreEqual("15", _viewModel.DecimalDisplayValue);
            Assert.AreEqual("17", _viewModel.OctalDisplayValue);
            Assert.AreEqual("1111", _viewModel.BinaryDisplayValue);

            var val = new bool[64];
            val[0] = true;
            val[1] = true;
            val[2] = true;
            val[3] = true;
            CompareVector(_viewModel.BinaryDigits, val);
        }
```

##### `ProgrammerModeButtonsDisable` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:625-643`

```csharp
        [TestMethod]
        public void ProgrammerModeButtonsDisable()
        {
            var viewModel = new StandardCalculatorViewModel();
            ChangeMode(viewModel, 2);

            // Hex accepts A-F; the other radices do not, and the keypad reflects that.
            viewModel.SwitchProgrammerModeBase(NumberBase.HexBase);
            Assert.IsTrue(viewModel.AreHEXButtonsEnabled, "Hex digits should be available in hex.");

            viewModel.SwitchProgrammerModeBase(NumberBase.DecBase);
            Assert.IsFalse(viewModel.AreHEXButtonsEnabled, "Hex digits should be unavailable in decimal.");

            viewModel.SwitchProgrammerModeBase(NumberBase.OctBase);
            Assert.IsFalse(viewModel.AreHEXButtonsEnabled, "Hex digits should be unavailable in octal.");

            viewModel.SwitchProgrammerModeBase(NumberBase.BinBase);
            Assert.IsFalse(viewModel.AreHEXButtonsEnabled, "Hex digits should be unavailable in binary.");
        }
```

##### `ProgrammerModeRadixGrouping` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:645-689`

```csharp
        [TestMethod]
        public void ProgrammerModeRadixGrouping()
        {
            var none = new TestItem[] { new(NumbersAndOperatorsEnum.None, "", "") };
            ValidateViewModelByCommands(_viewModel, none, true);
            _viewModel.IsProgrammer = true;

            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Two, "12", ""),
                new(NumbersAndOperatorsEnum.Three, "123", ""),
                new(NumbersAndOperatorsEnum.Four, "1,234", ""),
                new(NumbersAndOperatorsEnum.Five, "12,345", ""),
                new(NumbersAndOperatorsEnum.Six, "123,456", ""),
                new(NumbersAndOperatorsEnum.Seven, "1,234,567", ""),
                new(NumbersAndOperatorsEnum.Eight, "12,345,678", ""),
                new(NumbersAndOperatorsEnum.Nine, "123,456,789", ""),
                new(NumbersAndOperatorsEnum.None, "1", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            Assert.AreEqual("75B CD15", _viewModel.HexDisplayValue);
            Assert.AreEqual("123,456,789", _viewModel.DecimalDisplayValue);
            Assert.AreEqual("726 746 425", _viewModel.OctalDisplayValue);
            Assert.AreEqual("0111 0101 1011 1100 1101 0001 0101", _viewModel.BinaryDisplayValue);

            var val = new bool[64];
            val[0] = true;
            val[2] = true;
            val[4] = true;
            val[8] = true;
            val[10] = true;
            val[11] = true;
            val[14] = true;
            val[15] = true;
            val[16] = true;
            val[17] = true;
            val[19] = true;
            val[20] = true;
            val[22] = true;
            val[24] = true;
            val[25] = true;
            val[26] = true;
            CompareVector(_viewModel.BinaryDigits, val);
        }
```

##### `ProgrammerModeNot` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:691-714`

```csharp
        [TestMethod]
        public void ProgrammerModeNot()
        {
            var none = new TestItem[] { new(NumbersAndOperatorsEnum.None, "", "") };
            ValidateViewModelByCommands(_viewModel, none, true);
            _viewModel.IsProgrammer = true;

            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Not, "-2", "~(1)"),
                new(NumbersAndOperatorsEnum.None, "N/A", "N/A")
            };
            ValidateViewModelByCommands(_viewModel, items, false);
            Assert.AreEqual("FFFF FFFF FFFF FFFE", _viewModel.HexDisplayValue);
            Assert.AreEqual("-2", _viewModel.DecimalDisplayValue);
            Assert.AreEqual("1 777 777 777 777 777 777 776", _viewModel.OctalDisplayValue);
            Assert.AreEqual("1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1110", _viewModel.BinaryDisplayValue);
            Assert.AreEqual("-2", _viewModel.DisplayValue);

            var val = Enumerable.Repeat(true, 64).ToArray();
            val[0] = false;
            CompareVector(_viewModel.BinaryDigits, val);
        }
```

##### `ProgrammerModeAndOr` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:716-742`

```csharp
        [TestMethod]
        public void ProgrammerModeAndOr()
        {
            var none = new TestItem[] { new(NumbersAndOperatorsEnum.None, "", "") };
            ValidateViewModelByCommands(_viewModel, none, true);
            _viewModel.IsProgrammer = true;

            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Or, "1", "1 ||"),
                new(NumbersAndOperatorsEnum.Two, "2", "1 ||"),
                new(NumbersAndOperatorsEnum.Equals, "3", ""),
                new(NumbersAndOperatorsEnum.None, "3", "")
            };
            ValidateViewModelByCommands(_viewModel, items, false);

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.And, "1", "1 &"),
                new(NumbersAndOperatorsEnum.Two, "2", "1 &"),
                new(NumbersAndOperatorsEnum.Equals, "0", ""),
                new(NumbersAndOperatorsEnum.None, "0", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, false);
        }
```

##### `ProgrammerModeClear` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:744-770`

```csharp
        [TestMethod]
        public void ProgrammerModeClear()
        {
            var none = new TestItem[] { new(NumbersAndOperatorsEnum.None, "", "") };
            ValidateViewModelByCommands(_viewModel, none, true);
            _viewModel.IsProgrammer = true;

            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Or, "1", "1 ||"),
                new(NumbersAndOperatorsEnum.Two, "2", "1 ||"),
                new(NumbersAndOperatorsEnum.ClearEntry, "0", "1 ||"),
                new(NumbersAndOperatorsEnum.None, "", "1 ||")
            };
            ValidateViewModelByCommands(_viewModel, items, false);

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.And, "1", "1 &"),
                new(NumbersAndOperatorsEnum.Two, "2", "1 &"),
                new(NumbersAndOperatorsEnum.Clear, "0", ""),
                new(NumbersAndOperatorsEnum.None, "0", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, false);
        }
```

##### `ButtonPressedUnaryOperatorTest` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:776-825`

```csharp
        [TestMethod]
        public void ButtonPressedUnaryOperatorTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsStandardMode, "0", ""),
                new(NumbersAndOperatorsEnum.Five, "5", ""),
                new(NumbersAndOperatorsEnum.Invert, "0" + _decimalSeparator + "2", "reciproc(5)"),
                new(NumbersAndOperatorsEnum.Equals, "0" + _decimalSeparator + "2", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Six, "16", ""),
                new(NumbersAndOperatorsEnum.Sqrt, "4", "sqrt(16)"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, false);

            var items3 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Six, "6", ""),
                new(NumbersAndOperatorsEnum.Negate, "-6", ""),
                new(NumbersAndOperatorsEnum.Nine, "-69", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items3, false);

            var items4 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Clear, "0", ""),
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.Five, "5", ""),
                new(NumbersAndOperatorsEnum.InvSin, "Invalid input", "asind(5)"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items4, false);

            var items5 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Clear, "0", ""),
                new(NumbersAndOperatorsEnum.Four, "4", ""),
                new(NumbersAndOperatorsEnum.Factorial, "24", "fact(4)"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items5, false);
        }
```

##### `OnMemorySavedInHexRadixAndSwitchedToStandardMode` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:1040-1059`

```csharp
        [TestMethod]
        public void OnMemorySavedInHexRadixAndSwitchedToStandardMode()
        {
            ChangeMode(_viewModel, 2 /*Programmer*/);
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.F, "FF", ""),
                new(NumbersAndOperatorsEnum.None, "FF", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();
            ChangeMode(_viewModel, 1 /*Scientific*/);
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("255", _viewModel.DisplayValue);

            var memorySlot = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("255", memorySlot.Value);
        }
```

##### `OnMemorySavedInHexRadixAndRadixChanges` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:1061-1086`

```csharp
        [TestMethod]
        public void OnMemorySavedInHexRadixAndRadixChanges()
        {
            ChangeMode(_viewModel, 2 /*Programmer*/);
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.F, "FF", ""),
                new(NumbersAndOperatorsEnum.None, "FF", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();

            _viewModel.SwitchProgrammerModeBase(NumberBase.OctBase);
            var memorySlotOct = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("377", memorySlotOct.Value);

            _viewModel.SwitchProgrammerModeBase(NumberBase.DecBase);
            var memorySlotDec = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("255", memorySlotDec.Value);

            _viewModel.SwitchProgrammerModeBase(NumberBase.BinBase);
            var memorySlotBin = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("1111 1111", memorySlotBin.Value);
        }
```

##### `VerifyRawFormatting` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:1180-1198`

```csharp
        [TestMethod]
        public void VerifyRawFormatting()
        {
            // GetRawDisplayValue is private, so we test via the LocalizationSettings
            // approach used internally: remove group separators from display value.
            var settings = LocalizationSettings.GetInstance();

            _viewModel.DisplayValue = "1,001";
            Assert.AreEqual("1001", settings.RemoveGroupSeparators(_viewModel.DisplayValue));

            _viewModel.DisplayValue = "999";
            Assert.AreEqual("999", settings.RemoveGroupSeparators(_viewModel.DisplayValue));

            _viewModel.DisplayValue = "1,001,001";
            Assert.AreEqual("1001001", settings.RemoveGroupSeparators(_viewModel.DisplayValue));

            _viewModel.DisplayValue = "1,001, 001";
            Assert.AreEqual("1001001", settings.RemoveGroupSeparators(_viewModel.DisplayValue));
        }
```

### 5.3 Number formatting & string-conversion vectors

`GroupDigitsPerRadix`, `CalcInput::ToString` (exponent/base formatting), `TrimTrailingZeros`,
`GetNumberDigits`, `GetNumberDigitsWholeNumberPart`, `RoundSignificantDigits`, `ToScientificNumber`,
digit grouping, backspace/decimal-point editing rules, localised digit & group-separator handling,
clipboard string→expression parsing, and view-model state (de)serialisation.

#### 5.3.1 `TEST_CLASS(CalcEngineTests)` — digit grouping per radix (`GroupDigitsPerRadix`, `IsNumberInvalid`, `GroupDigits`)

##### `TestGroupDigitsPerRadix` — `CalcEngineTests` — `CalcEngineTests.cpp:36-59`

```cpp
        TEST_METHOD(TestGroupDigitsPerRadix)
        {
            // Empty/Error cases
            VERIFY_IS_TRUE(m_calcEngine->GroupDigitsPerRadix(L"", 10).empty(), L"Verify grouping empty string returns empty string.");
            VERIFY_ARE_EQUAL(L"12345678", m_calcEngine->GroupDigitsPerRadix(L"12345678", 9), L"Verify grouping on invalid base returns original string");

            // Octal
            VERIFY_ARE_EQUAL(L"1 234 567", m_calcEngine->GroupDigitsPerRadix(L"1234567", 8), L"Verify grouping in octal.");
            VERIFY_ARE_EQUAL(L"123", m_calcEngine->GroupDigitsPerRadix(L"123", 8), L"Verify minimum grouping in octal.");

            // Binary/Hexadecimal
            VERIFY_ARE_EQUAL(L"12 3456 7890", m_calcEngine->GroupDigitsPerRadix(L"1234567890", 2), L"Verify grouping in binary.");
            VERIFY_ARE_EQUAL(L"1234", m_calcEngine->GroupDigitsPerRadix(L"1234", 2), L"Verify minimum grouping in binary.");
            VERIFY_ARE_EQUAL(L"12 3456 7890", m_calcEngine->GroupDigitsPerRadix(L"1234567890", 16), L"Verify grouping in hexadecimal.");
            VERIFY_ARE_EQUAL(L"1234", m_calcEngine->GroupDigitsPerRadix(L"1234", 16), L"Verify minimum grouping in hexadecimal.");

            // Decimal
            VERIFY_ARE_EQUAL(L"1,234,567,890", m_calcEngine->GroupDigitsPerRadix(L"1234567890", 10), L"Verify grouping in base10.");
            VERIFY_ARE_EQUAL(L"1,234,567.89", m_calcEngine->GroupDigitsPerRadix(L"1234567.89", 10), L"Verify grouping in base10 with decimal.");
            VERIFY_ARE_EQUAL(L"1,234,567e89", m_calcEngine->GroupDigitsPerRadix(L"1234567e89", 10), L"Verify grouping in base10 with exponent.");
            VERIFY_ARE_EQUAL(
                L"1,234,567.89e5", m_calcEngine->GroupDigitsPerRadix(L"1234567.89e5", 10), L"Verify grouping in base10 with decimal and exponent.");
            VERIFY_ARE_EQUAL(L"-123,456,789", m_calcEngine->GroupDigitsPerRadix(L"-123456789", 10), L"Verify grouping in base10 with negative.");
        }
```

##### `TestIsNumberInvalid` — `CalcEngineTests` — `CalcEngineTests.cpp:61-157`

```cpp
        TEST_METHOD(TestIsNumberInvalid)
        {
            // Binary Number Checks
            vector<wstring> validBinStrs{ L"0", L"1", L"0011", L"1100" };
            vector<wstring> invalidBinStrs{ L"2", L"A", L"0.1" };
            for (wstring const& str : validBinStrs)
            {
                VERIFY_ARE_EQUAL(0, m_calcEngine->IsNumberInvalid(str, 0, 0, 2 /* Binary */));
            }
            for (wstring const& str : invalidBinStrs)
            {
                VERIFY_ARE_EQUAL(IDS_ERR_UNK_CH, m_calcEngine->IsNumberInvalid(str, 0, 0, 2 /* Binary */));
            }

            // Octal Number Checks
            vector<wstring> validOctStrs{ L"0", L"7", L"01234567", L"76543210" };
            vector<wstring> invalidOctStrs{ L"8", L"A", L"0.7" };
            for (wstring const& str : validOctStrs)
            {
                VERIFY_ARE_EQUAL(0, m_calcEngine->IsNumberInvalid(str, 0, 0, 8 /* Octal */));
            }
            for (wstring const& str : invalidOctStrs)
            {
                VERIFY_ARE_EQUAL(IDS_ERR_UNK_CH, m_calcEngine->IsNumberInvalid(str, 0, 0, 8 /* Octal */));
            }

            // Hexadecimal Number Checks
            vector<wstring> validHexStrs{ L"0", L"F", L"0123456789ABCDEF", L"FEDCBA9876543210" };
            vector<wstring> invalidHexStrs{ L"G", L"abcdef", L"x", L"0.1" };
            for (wstring const& str : validHexStrs)
            {
                VERIFY_ARE_EQUAL(0, m_calcEngine->IsNumberInvalid(str, 0, 0, 16 /* HEx */));
            }
            for (wstring const& str : invalidHexStrs)
            {
                VERIFY_ARE_EQUAL(IDS_ERR_UNK_CH, m_calcEngine->IsNumberInvalid(str, 0, 0, 16 /* Hex */));
            }

            // Decimal Number Checks

            // Special case errors: long exponent, long mantissa
            wstring longExp(L"1e12345");
            VERIFY_ARE_EQUAL(0, m_calcEngine->IsNumberInvalid(longExp, 5 /* Max exp length */, 100, 10 /* Decimal */));
            VERIFY_ARE_EQUAL(IDS_ERR_INPUT_OVERFLOW, m_calcEngine->IsNumberInvalid(longExp, 4 /* Max exp length */, 100, 10 /* Decimal */));
            // Mantissa length is sum of:
            //  - digits before decimal separator, minus leading zeroes
            //  - digits after decimal separator, including trailing zeroes
            // Each of these mantissa values should calculate as a length of 5
            vector<wstring> longMantStrs{ L"10000", L"10.000", L"0000012345", L"123.45", L"0.00123", L"0.12345", L"-123.45e678" };
            for (wstring const& str : longMantStrs)
            {
                VERIFY_ARE_EQUAL(0, m_calcEngine->IsNumberInvalid(str, 100, 5 /* Max mantissa length */, 10 /* Decimal */));
            }
            for (wstring const& str : longMantStrs)
            {
                VERIFY_ARE_EQUAL(IDS_ERR_INPUT_OVERFLOW, m_calcEngine->IsNumberInvalid(str, 100, 4 /* Max mantissa length */, 10 /* Decimal */));
            }

            // Regex matching (descriptions taken from CalcUtils.cpp)
            // Use 100 for exp/mantissa length as they are tested above
            vector<wstring> validDecStrs{ // Start with an optional + or -
                                          L"+1",
                                          L"-1",
                                          L"1",
                                          // Followed by zero or more digits
                                          L"-",
                                          L"",
                                          L"1234567890",
                                          // Followed by an optional decimal point
                                          L"1.0",
                                          L"-.",
                                          L"1.",
                                          // Followed by zero or more digits
                                          L"0.0",
                                          L"0.123456",
                                          // Followed by an optional exponent ('e')
                                          L"1e",
                                          L"1.e",
                                          L"-e",
                                          // If there's an exponent, its optionally followed by + or -
                                          // and followed by zero or more digits
                                          L"1e+12345",
                                          L"1e-12345",
                                          L"1e123",
                                          // All together
                                          L"-123.456e+789"
            };
            vector<wstring> invalidDecStrs{ L"x123", L"123-", L"1e1.2", L"1-e2" };
            for (wstring const& str : validDecStrs)
            {
                VERIFY_ARE_EQUAL(0, m_calcEngine->IsNumberInvalid(str, 100, 100, 10 /* Dec */));
            }
            for (wstring const& str : invalidDecStrs)
            {
                VERIFY_ARE_EQUAL(IDS_ERR_UNK_CH, m_calcEngine->IsNumberInvalid(str, 100, 100, 10 /* Dec */));
            }
        }
```

##### `TestDigitGroupingStringToGroupingVector` — `CalcEngineTests` — `CalcEngineTests.cpp:159-186`

```cpp
        TEST_METHOD(TestDigitGroupingStringToGroupingVector)
        {
            vector<uint32_t> groupingVector{};
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(L""), L"Verify empty grouping");

            groupingVector = { 1 };
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(L"1"), L"Verify simple grouping");

            groupingVector = { 3, 0 };
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(L"3;0"), L"Verify standard grouping");

            groupingVector = { 3, 0, 0 };
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(L"3;0;0"), L"Verify expanded non-repeating grouping");

            groupingVector = { 5, 3, 2, 4, 6 };
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(L"5;3;2;4;6"), L"Verify long grouping");

            groupingVector = { 15, 15, 15, 0 };
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(L"15;15;15;0"), L"Verify large grouping");

            groupingVector = { 4, 7, 0 };
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(L"4;16;7;25;0"), L"Verify we ignore oversize grouping");

            groupingVector = { 3, 0 };
            constexpr wstring_view nonRepeatingGrouping = L"3;0;0";
            constexpr wstring_view repeatingGrouping = nonRepeatingGrouping.substr(0, 3);
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(repeatingGrouping), L"Verify we don't go past the end of wstring_view range");
        }
```

##### `TestGroupDigits` — `CalcEngineTests` — `CalcEngineTests.cpp:188-236`

```cpp
        TEST_METHOD(TestGroupDigits)
        {
            wstring result{ L"1234567" };
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L"", { 3, 0 }, L"1234567", false), L"Verify handling of empty delimiter.");
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", {}, L"1234567", false), L"Verify handling of empty grouping.");

            result = L"1,234,567";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 3, 0 }, L"1234567", false), L"Verify standard digit grouping.");

            result = L"1 234 567";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L" ", { 3, 0 }, L"1234567", false), L"Verify delimiter change.");

            result = L"1|||234|||567";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L"|||", { 3, 0 }, L"1234567", false), L"Verify long delimiter.");

            result = L"12,345e67";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 3, 0 }, L"12345e67", false), L"Verify respect of exponent.");

            result = L"12,345.67";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 3, 0 }, L"12345.67", false), L"Verify respect of decimal.");

            result = L"1,234.56e7";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 3, 0 }, L"1234.56e7", false), L"Verify respect of exponent and decimal.");

            result = L"-1,234,567";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 3, 0 }, L"-1234567", true), L"Verify negative number grouping.");

            // Test various groupings
            result = L"1234567890123456";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 0, 0 }, L"1234567890123456", false), L"Verify no grouping.");

            result = L"1234567890123,456";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 3 }, L"1234567890123456", false), L"Verify non-repeating grouping.");
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 3, 0, 0 }, L"1234567890123456", false), L"Verify expanded form non-repeating grouping.");

            result = L"12,34,56,78,901,23456";
            VERIFY_ARE_EQUAL(
                result, m_calcEngine->GroupDigits(L",", { 5, 3, 2, 0 }, L"1234567890123456", false), L"Verify multigroup with repeating grouping.");

            result = L"1234,5678,9012,3456";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 4, 0 }, L"1234567890123456", false), L"Verify repeating non-standard grouping.");

            result = L"123456,78,901,23456";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 5, 3, 2 }, L"1234567890123456", false), L"Verify multigroup non-repeating grouping.");
            VERIFY_ARE_EQUAL(
                result,
                m_calcEngine->GroupDigits(L",", { 5, 3, 2, 0, 0 }, L"1234567890123456", false),
                L"Verify expanded form multigroup non-repeating grouping.");
        }
```

#### 5.3.2 `TEST_CLASS(CalcInputTest)` — input editing rules (`TryAddDigit`, `TryToggleSign`, `Backspace*`, decimal point, exponent entry)

Includes `TryAddDigitMaxCount` and `ToRational`, which also appear in section 5.6.

##### `Clear` — `CalcInputTest` — `CalcInputTest.cpp:25-40`

```cpp
        TEST_METHOD(Clear)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryToggleSign(false, L"999");
            m_calcInput.TryAddDecimalPt();
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            m_calcInput.TryAddDigit(3, 10, false, L"999", 64, 32);

            VERIFY_ARE_EQUAL(L"-1.2e+3", m_calcInput.ToString(10), L"Verify input is correct.");

            m_calcInput.Clear();

            ::Logger::WriteMessage(m_calcInput.ToString(10).c_str());
            VERIFY_ARE_EQUAL(L"0", m_calcInput.ToString(10), L"Verify input is 0 after clear.");
        }
```

##### `TryToggleSignZero` — `CalcInputTest` — `CalcInputTest.cpp:42-46`

```cpp
        TEST_METHOD(TryToggleSignZero)
        {
            VERIFY_IS_TRUE(m_calcInput.TryToggleSign(false, L"999"), L"Verify toggling 0 succeeds.");
            VERIFY_ARE_EQUAL(L"0", m_calcInput.ToString(10), L"Verify toggling 0 does not create -0.");
        }
```

##### `TryToggleSignExponent` — `CalcInputTest` — `CalcInputTest.cpp:47-56`

```cpp
        TEST_METHOD(TryToggleSignExponent)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryToggleSign(false, L"999"), L"Verify toggling exponent sign succeeds.");
            VERIFY_ARE_EQUAL(L"1.e-2", m_calcInput.ToString(10), L"Verify toggling exponent sign does not toggle base sign.");
            VERIFY_IS_TRUE(m_calcInput.TryToggleSign(false, L"999"), L"Verify toggling exponent sign succeeds.");
            VERIFY_ARE_EQUAL(L"1.e+2", m_calcInput.ToString(10), L"Verify toggling negative exponent sign does not toggle base sign.");
        }
```

##### `TryToggleSignBase` — `CalcInputTest` — `CalcInputTest.cpp:57-64`

```cpp
        TEST_METHOD(TryToggleSignBase)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryToggleSign(false, L"999"), L"Verify toggling base sign succeeds.");
            VERIFY_ARE_EQUAL(L"-1", m_calcInput.ToString(10), L"Verify toggling base sign creates negative base.");
            VERIFY_IS_TRUE(m_calcInput.TryToggleSign(false, L"999"), L"Verify toggling base sign succeeds.");
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify toggling negative base sign creates positive base.");
        }
```

##### `TryToggleSignBaseIntegerMode` — `CalcInputTest` — `CalcInputTest.cpp:65-70`

```cpp
        TEST_METHOD(TryToggleSignBaseIntegerMode)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryToggleSign(true, L"999"), L"Verify toggling base sign in integer mode succeeds.");
            VERIFY_ARE_EQUAL(L"-1", m_calcInput.ToString(10), L"Verify toggling base sign creates negative base.");
        }
```

##### `TryToggleSignRollover` — `CalcInputTest` — `CalcInputTest.cpp:71-79`

```cpp
        TEST_METHOD(TryToggleSignRollover)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryToggleSign(true, L"127"), L"Verify toggling base sign in integer mode succeeds.");
            m_calcInput.TryAddDigit(8, 10, false, L"999", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryToggleSign(true, L"127"), L"Verify toggling base sign in integer mode fails on rollover.");
            VERIFY_ARE_EQUAL(L"-128", m_calcInput.ToString(10), L"Verify toggling base sign on rollover does not change value.");
        }
```

##### `TryAddDigitLeadingZeroes` — `CalcInputTest` — `CalcInputTest.cpp:81-87`

```cpp
        TEST_METHOD(TryAddDigitLeadingZeroes)
        {
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(0, 10, false, L"999", 64, 32), L"Verify TryAddDigit succeeds.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(0, 10, false, L"999", 64, 32), L"Verify TryAddDigit succeeds.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(0, 10, false, L"999", 64, 32), L"Verify TryAddDigit succeeds.");
            VERIFY_ARE_EQUAL(L"0", m_calcInput.ToString(10), L"Verify leading zeros are ignored.");
        }
```

##### `TryAddDigitMaxCount` — `CalcInputTest` — `CalcInputTest.cpp:88-106`

```cpp
        TEST_METHOD(TryAddDigitMaxCount)
        {
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32), L"Verify TryAddDigit for base with length < maxDigits succeeds.");
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify adding digit for base with length < maxDigits succeeded.");
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 1), L"Verify TryAddDigit for base with length > maxDigits fails.");
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify digit for base was not added.");
            m_calcInput.TryBeginExponent();
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32), L"Verify TryAddDigit for exponent with length < maxDigits succeeds.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32), L"Verify TryAddDigit for exponent with length < maxDigits succeeds.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(3, 10, false, L"999", 64, 32), L"Verify TryAddDigit for exponent with length < maxDigits succeeds.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(4, 10, false, L"999", 64, 32), L"Verify TryAddDigit for exponent with length < maxDigits succeeds.");
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(5, 10, false, L"999", 64, 32), L"Verify TryAddDigit for exponent with length > maxDigits fails.");
            VERIFY_ARE_EQUAL(L"1.e+1234", m_calcInput.ToString(10), L"Verify adding digits for exponent with length < maxDigits succeeded.");

            m_calcInput.Clear();
            m_calcInput.TryAddDecimalPt();
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 1), L"Verify decimal point and leading zero does not count toward maxDigits.");
            VERIFY_ARE_EQUAL(L"0.1", m_calcInput.ToString(10), L"Verify input value checking dec pt and leading zero impact on maxDigits.");
        }
```

##### `TryAddDigitValues` — `CalcInputTest` — `CalcInputTest.cpp:107-116`

```cpp
        TEST_METHOD(TryAddDigitValues)
        {
            // Use an arbitrary value > 16 to test that input accepts digits > hexadecimal 0xF.
            // TryAddDigit does not validate whether the digit fits within the current radix.
            for (unsigned int i = 0; i < 25; i++)
            {
                VERIFY_IS_TRUE(m_calcInput.TryAddDigit(i, 10, false, L"999", 64, 32), (L"Verify TryAddDigit succeeds for " + to_wstring(i)).c_str());
                m_calcInput.Clear();
            }
        }
```

##### `TryAddDigitRolloverBaseCheck` — `CalcInputTest` — `CalcInputTest.cpp:117-122`

```cpp
        TEST_METHOD(TryAddDigitRolloverBaseCheck)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(2, 16, true, L"999", 64, 1), L"Verify TryAddDigit rollover fails for bases other than 8,10.");
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(1, 2, true, L"999", 64, 1), L"Verify TryAddDigit rollover fails for bases other than 8,10.");
        }
```

##### `TryAddDigitRolloverOctalByte` — `CalcInputTest` — `CalcInputTest.cpp:123-131`

```cpp
        TEST_METHOD(TryAddDigitRolloverOctalByte)
        {
            m_calcInput.TryAddDigit(1, 8, true, L"777", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(2, 8, true, L"377", 8, 1), L"Verify we can add an extra digit in OctalByte if first digit <= 3.");

            m_calcInput.Clear();
            m_calcInput.TryAddDigit(4, 8, true, L"777", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(2, 8, true, L"377", 8, 1), L"Verify we cannot add an extra digit in OctalByte if first digit > 3.");
        }
```

##### `TryAddDigitRolloverOctalWord` — `CalcInputTest` — `CalcInputTest.cpp:132-140`

```cpp
        TEST_METHOD(TryAddDigitRolloverOctalWord)
        {
            m_calcInput.TryAddDigit(1, 8, true, L"777", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(2, 8, true, L"377", 16, 1), L"Verify we can add an extra digit in OctalByte if first digit == 1.");

            m_calcInput.Clear();
            m_calcInput.TryAddDigit(2, 8, true, L"777", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(2, 8, true, L"377", 16, 1), L"Verify we cannot add an extra digit in OctalByte if first digit > 1.");
        }
```

##### `TryAddDigitRolloverOctalDword` — `CalcInputTest` — `CalcInputTest.cpp:141-149`

```cpp
        TEST_METHOD(TryAddDigitRolloverOctalDword)
        {
            m_calcInput.TryAddDigit(1, 8, true, L"777", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(2, 8, true, L"377", 32, 1), L"Verify we can add an extra digit in OctalByte if first digit <= 3.");

            m_calcInput.Clear();
            m_calcInput.TryAddDigit(4, 8, true, L"777", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(2, 8, true, L"377", 32, 1), L"Verify we cannot add an extra digit in OctalByte if first digit > 3.");
        }
```

##### `TryAddDigitRolloverOctalQword` — `CalcInputTest` — `CalcInputTest.cpp:150-158`

```cpp
        TEST_METHOD(TryAddDigitRolloverOctalQword)
        {
            m_calcInput.TryAddDigit(1, 8, true, L"777", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(2, 8, true, L"377", 64, 1), L"Verify we can add an extra digit in OctalByte if first digit == 1.");

            m_calcInput.Clear();
            m_calcInput.TryAddDigit(2, 8, true, L"777", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(2, 8, true, L"377", 64, 1), L"Verify we cannot add an extra digit in OctalByte if first digit > 1.");
        }
```

##### `TryAddDigitRolloverDecimal` — `CalcInputTest` — `CalcInputTest.cpp:159-178`

```cpp
        TEST_METHOD(TryAddDigitRolloverDecimal)
        {
            m_calcInput.TryAddDigit(1, 10, true, L"127", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(0, 10, true, L"1", 8, 1), L"Verify we cannot add a digit if input size matches maxStr size.");
            m_calcInput.TryAddDigit(2, 10, true, L"127", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(2, 10, true, L"110", 8, 2), L"Verify we cannot add a digit if n char comparison > 0.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(7, 10, true, L"130", 8, 2), L"Verify we can add a digit if n char comparison < 0.");

            m_calcInput.Clear();
            m_calcInput.TryAddDigit(1, 10, true, L"127", 64, 32);
            m_calcInput.TryAddDigit(2, 10, true, L"127", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(8, 10, true, L"127", 8, 2), L"Verify we cannot add a digit if digit exceeds max value.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(7, 10, true, L"127", 8, 2), L"Verify we can add a digit if digit does not exceed max value.");

            m_calcInput.Backspace();
            m_calcInput.TryToggleSign(true, L"127");
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(9, 10, true, L"127", 8, 2), L"Negative value: verify we cannot add a digit if digit exceeds max value.");
            VERIFY_IS_TRUE(
                m_calcInput.TryAddDigit(8, 10, true, L"127", 8, 2), L"Negative value: verify we can add a digit if digit does not exceed max value.");
        }
```

##### `TryAddDecimalPtEmpty` — `CalcInputTest` — `CalcInputTest.cpp:180-186`

```cpp
        TEST_METHOD(TryAddDecimalPtEmpty)
        {
            VERIFY_IS_FALSE(m_calcInput.HasDecimalPt(), L"Verify input has no decimal point.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDecimalPt(), L"Verify adding decimal to empty input.");
            VERIFY_IS_TRUE(m_calcInput.HasDecimalPt(), L"Verify input has decimal point.");
            VERIFY_ARE_EQUAL(L"0.", m_calcInput.ToString(10), L"Verify decimal on empty input.");
        }
```

##### `TryAddDecimalPointTwice` — `CalcInputTest` — `CalcInputTest.cpp:187-193`

```cpp
        TEST_METHOD(TryAddDecimalPointTwice)
        {
            VERIFY_IS_FALSE(m_calcInput.HasDecimalPt(), L"Verify input has no decimal point.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDecimalPt(), L"Verify adding decimal to empty input.");
            VERIFY_IS_TRUE(m_calcInput.HasDecimalPt(), L"Verify input has decimal point.");
            VERIFY_IS_FALSE(m_calcInput.TryAddDecimalPt(), L"Verify adding decimal point fails if input has decimal point.");
        }
```

##### `TryAddDecimalPointExponent` — `CalcInputTest` — `CalcInputTest.cpp:194-200`

```cpp
        TEST_METHOD(TryAddDecimalPointExponent)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDecimalPt(), L"Verify adding decimal point fails if input has exponent.");
        }
```

##### `TryBeginExponentNoExponent` — `CalcInputTest` — `CalcInputTest.cpp:202-207`

```cpp
        TEST_METHOD(TryBeginExponentNoExponent)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryBeginExponent(), L"Verify adding exponent succeeds on input without exponent.");
            VERIFY_ARE_EQUAL(L"1.e+0", m_calcInput.ToString(10), L"Verify exponent present.");
        }
```

##### `TryBeginExponentWithExponent` — `CalcInputTest` — `CalcInputTest.cpp:208-213`

```cpp
        TEST_METHOD(TryBeginExponentWithExponent)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryBeginExponent(), L"Verify adding exponent succeeds on input without exponent.");
            VERIFY_IS_FALSE(m_calcInput.TryBeginExponent(), L"Verify cannot add exponent if input already has exponent.");
        }
```

##### `BackspaceZero` — `CalcInputTest` — `CalcInputTest.cpp:215-219`

```cpp
        TEST_METHOD(BackspaceZero)
        {
            m_calcInput.Backspace();
            VERIFY_ARE_EQUAL(L"0", m_calcInput.ToString(10), L"Verify backspace on 0 is still 0.");
        }
```

##### `BackspaceSingleChar` — `CalcInputTest` — `CalcInputTest.cpp:220-226`

```cpp
        TEST_METHOD(BackspaceSingleChar)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify input before backspace.");
            m_calcInput.Backspace();
            VERIFY_ARE_EQUAL(L"0", m_calcInput.ToString(10), L"Verify input after backspace.");
        }
```

##### `BackspaceMultiChar` — `CalcInputTest` — `CalcInputTest.cpp:227-234`

```cpp
        TEST_METHOD(BackspaceMultiChar)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            VERIFY_ARE_EQUAL(L"12", m_calcInput.ToString(10), L"Verify input before backspace.");
            m_calcInput.Backspace();
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify input after backspace.");
        }
```

##### `BackspaceDecimal` — `CalcInputTest` — `CalcInputTest.cpp:235-244`

```cpp
        TEST_METHOD(BackspaceDecimal)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDecimalPt();
            VERIFY_ARE_EQUAL(L"1.", m_calcInput.ToString(10), L"Verify input before backspace.");
            VERIFY_IS_TRUE(m_calcInput.HasDecimalPt(), L"Verify input has decimal point.");
            m_calcInput.Backspace();
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify input after backspace.");
            VERIFY_IS_FALSE(m_calcInput.HasDecimalPt(), L"Verify decimal point was removed.");
        }
```

##### `BackspaceMultiCharDecimal` — `CalcInputTest` — `CalcInputTest.cpp:245-254`

```cpp
        TEST_METHOD(BackspaceMultiCharDecimal)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDecimalPt();
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDigit(3, 10, false, L"999", 64, 32);
            VERIFY_ARE_EQUAL(L"1.23", m_calcInput.ToString(10), L"Verify input before backspace.");
            m_calcInput.Backspace();
            VERIFY_ARE_EQUAL(L"1.2", m_calcInput.ToString(10), L"Verify input after backspace.");
        }
```

##### `BackspaceZeroDecimalWithoutPrefixZeros` — `CalcInputTest` — `CalcInputTest.cpp:256-264`

```cpp
        TEST_METHOD(BackspaceZeroDecimalWithoutPrefixZeros)
        {
            m_calcInput.TryAddDigit(0, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDecimalPt();
            VERIFY_ARE_EQUAL(L"0.", m_calcInput.ToString(10), L"Verify input before backspace.")
            m_calcInput.Backspace();
            m_calcInput.TryAddDigit(0, 10, false, L"999", 64, 32);
            VERIFY_ARE_EQUAL(L"0", m_calcInput.ToString(10), L"Verify input after backspace.")
        }
```

##### `SetDecimalSymbol` — `CalcInputTest` — `CalcInputTest.cpp:266-272`

```cpp
        TEST_METHOD(SetDecimalSymbol)
        {
            m_calcInput.TryAddDecimalPt();
            VERIFY_ARE_EQUAL(L"0.", m_calcInput.ToString(10), L"Verify default decimal point.");
            m_calcInput.SetDecimalSymbol(L',');
            VERIFY_ARE_EQUAL(L"0,", m_calcInput.ToString(10), L"Verify new decimal point.");
        }
```

##### `ToStringEmpty` — `CalcInputTest` — `CalcInputTest.cpp:274-277`

```cpp
        TEST_METHOD(ToStringEmpty)
        {
            VERIFY_ARE_EQUAL(L"0", m_calcInput.ToString(10), L"Verify ToString of empty value.");
        }
```

##### `ToStringNegative` — `CalcInputTest` — `CalcInputTest.cpp:278-283`

```cpp
        TEST_METHOD(ToStringNegative)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryToggleSign(false, L"999");
            VERIFY_ARE_EQUAL(L"-1", m_calcInput.ToString(10), L"Verify ToString of negative value.");
        }
```

##### `ToStringExponentBase10` — `CalcInputTest` — `CalcInputTest.cpp:284-289`

```cpp
        TEST_METHOD(ToStringExponentBase10)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            VERIFY_ARE_EQUAL(L"1.e+0", m_calcInput.ToString(10), L"Verify ToString of empty base10 exponent.");
        }
```

##### `ToStringExponentBase8` — `CalcInputTest` — `CalcInputTest.cpp:290-295`

```cpp
        TEST_METHOD(ToStringExponentBase8)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            VERIFY_ARE_EQUAL(L"1.^+0", m_calcInput.ToString(8), L"Verify ToString of empty base8 exponent.");
        }
```

##### `ToStringExponentNegative` — `CalcInputTest` — `CalcInputTest.cpp:296-302`

```cpp
        TEST_METHOD(ToStringExponentNegative)
        {
            m_calcInput.TryAddDigit(1, 8, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            m_calcInput.TryToggleSign(false, L"999");
            VERIFY_ARE_EQUAL(L"1.e-0", m_calcInput.ToString(10), L"Verify ToString of empty negative exponent.");
        }
```

##### `ToStringExponentPositive` — `CalcInputTest` — `CalcInputTest.cpp:303-311`

```cpp
        TEST_METHOD(ToStringExponentPositive)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDigit(3, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDigit(4, 10, false, L"999", 64, 32);
            VERIFY_ARE_EQUAL(L"1.e+234", m_calcInput.ToString(10), L"Verify ToString of exponent with value.");
        }
```

##### `ToStringInteger` — `CalcInputTest` — `CalcInputTest.cpp:312-316`

```cpp
        TEST_METHOD(ToStringInteger)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify ToString of integer value hides decimal.");
        }
```

##### `ToStringBaseTooLong` — `CalcInputTest` — `CalcInputTest.cpp:317-327`

```cpp
        TEST_METHOD(ToStringBaseTooLong)
        {
            wstring maxStr{};
            for (size_t i = 0; i < MAX_STRLEN + 1; i++)
            {
                maxStr += L'1';
                m_calcInput.TryAddDigit(1, 10, false, maxStr, 64, 100);
            }
            auto result = m_calcInput.ToString(10);
            VERIFY_IS_TRUE(result.empty(), L"Verify ToString of base value that is too large yields empty string.");
        }
```

##### `ToStringExponentTooLong` — `CalcInputTest` — `CalcInputTest.cpp:328-354`

```cpp
        TEST_METHOD(ToStringExponentTooLong)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            wstring maxStr{ L"11" };
            bool exponentCapped = false;
            for (size_t i = 0; i < MAX_STRLEN + 1; i++)
            {
                maxStr += L'1';
                if (!m_calcInput.TryAddDigit(1, 10, false, maxStr, 64, MAX_STRLEN + 25))
                {
                    exponentCapped = true;
                }
            }
            auto result = m_calcInput.ToString(10);

            // TryAddDigit caps the exponent length to C_EXP_MAX_DIGITS = 4, so ToString() succeeds.
            // If that cap is removed, ToString() should return an empty string.
            if (exponentCapped)
            {
                VERIFY_ARE_EQUAL(L"1.e+1111", result, L"Verify ToString succeeds; exponent length is capped at C_EXP_MAX_DIGITS.");
            }
            else
            {
                VERIFY_IS_TRUE(result.empty(), L"Verify ToString of exponent value that is too large yields empty string.");
            }
        }
```

##### `ToRational` — `CalcInputTest` — `CalcInputTest.cpp:356-366`

```cpp
        TEST_METHOD(ToRational)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDigit(3, 10, false, L"999", 64, 32);
            VERIFY_ARE_EQUAL(L"123", m_calcInput.ToString(10), L"Verify input before conversion to rational.");

            auto rat = m_calcInput.ToRational(10, false);
            VERIFY_ARE_EQUAL(1, rat.P().Mantissa().size(), L"Verify digit count of rational.");
            VERIFY_ARE_EQUAL(123, rat.P().Mantissa().front(), L"Verify first digit of mantissa.");
        }
```

#### 5.3.3 `TEST_CLASS(CalculatorManagerTest)` — `NumberFormattingUtils` + binary-operator display reset

##### `UnitConversionManagerNumberFormattingUtils_TrimTrailingZeros` — `CalculatorManagerTest` — `CalculatorManagerTest.cpp:978-1001`

```cpp
    void CalculatorManagerTest::UnitConversionManagerNumberFormattingUtils_TrimTrailingZeros()
    {
        wstring number = L"2.1032100000000";
        TrimTrailingZeros(number);
        VERIFY_ARE_EQUAL(number, L"2.10321");
        number = L"-122.123200";
        TrimTrailingZeros(number);
        VERIFY_ARE_EQUAL(number, L"-122.1232");
        number = L"0.0001200";
        TrimTrailingZeros(number);
        VERIFY_ARE_EQUAL(number, L"0.00012");
        number = L"12.000";
        TrimTrailingZeros(number);
        VERIFY_ARE_EQUAL(number, L"12");
        number = L"-12.00000";
        TrimTrailingZeros(number);
        VERIFY_ARE_EQUAL(number, L"-12");
        number = L"0.000";
        TrimTrailingZeros(number);
        VERIFY_ARE_EQUAL(number, L"0");
        number = L"322423";
        TrimTrailingZeros(number);
        VERIFY_ARE_EQUAL(number, L"322423");
    }
```

##### `UnitConversionManagerNumberFormattingUtils_GetNumberDigits` — `CalculatorManagerTest` — `CalculatorManagerTest.cpp:1003-1020`

```cpp
    void CalculatorManagerTest::UnitConversionManagerNumberFormattingUtils_GetNumberDigits()
    {
        wstring number = L"2.10321";
        unsigned int digitsCount = GetNumberDigits(number);
        VERIFY_ARE_EQUAL(digitsCount, 6);
        number = L"-122.1232";
        digitsCount = GetNumberDigits(number);
        VERIFY_ARE_EQUAL(digitsCount, 7);
        number = L"-3432";
        digitsCount = GetNumberDigits(number);
        VERIFY_ARE_EQUAL(digitsCount, 4);
        number = L"0";
        digitsCount = GetNumberDigits(number);
        VERIFY_ARE_EQUAL(digitsCount, 1);
        number = L"0.0001223";
        digitsCount = GetNumberDigits(number);
        VERIFY_ARE_EQUAL(digitsCount, 8);
    }
```

##### `UnitConversionManagerNumberFormattingUtils_GetNumberDigitsWholeNumberPart` — `CalculatorManagerTest` — `CalculatorManagerTest.cpp:1022-1040`

```cpp
    void CalculatorManagerTest::UnitConversionManagerNumberFormattingUtils_GetNumberDigitsWholeNumberPart()
    {
        unsigned int digitsCount = GetNumberDigitsWholeNumberPart(2.10321);
        VERIFY_ARE_EQUAL(digitsCount, 1);
        digitsCount = GetNumberDigitsWholeNumberPart(-122.1232);
        VERIFY_ARE_EQUAL(digitsCount, 3);
        digitsCount = GetNumberDigitsWholeNumberPart(-3432);
        VERIFY_ARE_EQUAL(digitsCount, 4);
        digitsCount = GetNumberDigitsWholeNumberPart(0);
        VERIFY_ARE_EQUAL(digitsCount, 1);
        digitsCount = GetNumberDigitsWholeNumberPart(324328412837382);
        VERIFY_ARE_EQUAL(digitsCount, 15);
        digitsCount = GetNumberDigitsWholeNumberPart(324328412837382.232213214324234);
        VERIFY_ARE_EQUAL(digitsCount, 15);
        digitsCount = GetNumberDigitsWholeNumberPart(0.032);
        VERIFY_ARE_EQUAL(digitsCount, 1);
        digitsCount = GetNumberDigitsWholeNumberPart(0.00000000000000000001);
        VERIFY_ARE_EQUAL(digitsCount, 1);
    }
```

##### `UnitConversionManagerNumberFormattingUtils_RoundSignificantDigits` — `CalculatorManagerTest` — `CalculatorManagerTest.cpp:1042-1056`

```cpp
    void CalculatorManagerTest::UnitConversionManagerNumberFormattingUtils_RoundSignificantDigits()
    {
        wstring result = RoundSignificantDigits(12.342343242, 3);
        VERIFY_ARE_EQUAL(result, L"12.342");
        result = RoundSignificantDigits(12.3429999, 3);
        VERIFY_ARE_EQUAL(result, L"12.343");
        result = RoundSignificantDigits(12.342500001, 3);
        VERIFY_ARE_EQUAL(result, L"12.343");
        result = RoundSignificantDigits(-2312.1244243346454345, 5);
        VERIFY_ARE_EQUAL(result, L"-2312.12442");
        result = RoundSignificantDigits(0.3423432423, 5);
        VERIFY_ARE_EQUAL(result, L"0.34234");
        result = RoundSignificantDigits(0.3423, 7);
        VERIFY_ARE_EQUAL(result, L"0.3423000");
    }
```

##### `UnitConversionManagerNumberFormattingUtils_ToScientificNumber` — `CalculatorManagerTest` — `CalculatorManagerTest.cpp:1058-1076`

```cpp
    void CalculatorManagerTest::UnitConversionManagerNumberFormattingUtils_ToScientificNumber()
    {
        wstring result = ToScientificNumber(3423);
        VERIFY_ARE_EQUAL(result, L"3.423000e+03");
        result = ToScientificNumber(-21);
        VERIFY_ARE_EQUAL(result, L"-2.100000e+01");
        result = ToScientificNumber(0.0232);
        VERIFY_ARE_EQUAL(result, L"2.320000e-02");
        result = ToScientificNumber(-0.00921);
        VERIFY_ARE_EQUAL(result, L"-9.210000e-03");
        result = ToScientificNumber(2343243345677);
        VERIFY_ARE_EQUAL(result, L"2.343243e+12");
        result = ToScientificNumber(-3432474247332942);
        VERIFY_ARE_EQUAL(result, L"-3.432474e+15");
        result = ToScientificNumber(0.000000003432432);
        VERIFY_ARE_EQUAL(result, L"3.432432e-09");
        result = ToScientificNumber(-0.000000003432432);
        VERIFY_ARE_EQUAL(result, L"-3.432432e-09");
    }
```

##### `CalculatorManagerTestBinaryOperatorReceived` — `CalculatorManagerTest` — `CalculatorManagerTest.cpp:1078-1092`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestBinaryOperatorReceived()
    {
        CalculatorManagerDisplayTester* pCalculatorDisplay = (CalculatorManagerDisplayTester*)m_calculatorDisplayTester.get();

        VERIFY_ARE_EQUAL(0, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());

        m_calculatorManager->SetStandardMode();
        ExecuteCommands({ Command::Command1, Command::CommandADD });

        wstring display = pCalculatorDisplay->GetPrimaryDisplay();
        VERIFY_ARE_EQUAL(L"1", display);

        // Verify BinaryOperatorReceived
        VERIFY_ARE_EQUAL(1, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());
    }
```

##### `CalculatorManagerTestBinaryOperatorReceived_Multiple` — `CalculatorManagerTest` — `CalculatorManagerTest.cpp:1094-1108`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestBinaryOperatorReceived_Multiple()
    {
        CalculatorManagerDisplayTester* pCalculatorDisplay = (CalculatorManagerDisplayTester*)m_calculatorDisplayTester.get();

        VERIFY_ARE_EQUAL(0, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());

        m_calculatorManager->SetStandardMode();
        ExecuteCommands({ Command::Command1, Command::CommandADD, Command::CommandSUB, Command::CommandMUL });

        wstring display = pCalculatorDisplay->GetPrimaryDisplay();
        VERIFY_ARE_EQUAL(L"1", display);

        // Verify BinaryOperatorReceived
        VERIFY_ARE_EQUAL(3, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());
    }
```

##### `CalculatorManagerTestBinaryOperatorReceived_LongInput` — `CalculatorManagerTest` — `CalculatorManagerTest.cpp:1110-1134`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestBinaryOperatorReceived_LongInput()
    {
        CalculatorManagerDisplayTester* pCalculatorDisplay = (CalculatorManagerDisplayTester*)m_calculatorDisplayTester.get();

        VERIFY_ARE_EQUAL(0, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());

        m_calculatorManager->SetStandardMode();
        ExecuteCommands({ Command::Command1,
                          Command::CommandADD,
                          Command::Command2,
                          Command::CommandMUL,
                          Command::Command1,
                          Command::Command0,
                          Command::CommandSUB,
                          Command::Command5,
                          Command::CommandDIV,
                          Command::Command5,
                          Command::CommandEQU });

        wstring display = pCalculatorDisplay->GetPrimaryDisplay();
        VERIFY_ARE_EQUAL(L"5", display);

        // Verify BinaryOperatorReceived
        VERIFY_ARE_EQUAL(4, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());
    }
```

#### 5.3.4 Managed formatting / parsing / serialisation

`CopyPasteManagerTests` (clipboard to expression), `LocalizationSettingsTests` (localised digits and group
separators), `UtilsTests`, `SnapshotJsonTests`, `SnapshotRoundTripTests`, and the
`StandardCalculatorViewModelTests` formatting/automation-name tests.

##### `FunctionalCopyPasteTest` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:15-29`

```csharp
        [TestMethod]
        public void FunctionalCopyPasteTest()
        {
            var scvm = new StandardCalculatorViewModel();
            scvm.IsStandard = true;
            string[] inputs = { "123", "12345", "123+456", "1,234", "1 2 3", "\n\r1,234\n", "\n 1+\n2 ", "1\"2" };

            foreach (var input in inputs)
            {
                scvm.OnPaste(input);
                Assert.AreEqual(ValidateStandardPasteExpression(scvm.DisplayValue), scvm.DisplayValue);
                Assert.AreEqual(ValidateScientificPasteExpression(scvm.DisplayValue), scvm.DisplayValue);
                Assert.AreEqual(ValidateProgrammerHexQwordPasteExpression(scvm.DisplayValue), scvm.DisplayValue);
            }
        }
```

##### `ValidateStandardPasteExpressionTest` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:31-83`

```csharp
        [TestMethod]
        public void ValidateStandardPasteExpressionTest()
        {
            string[] positiveInput =
            {
                "123",
                "+123",
                "-133",
                "12345.",
                "+12.34",
                "12.345",
                "012.034",
                "-23.032",
                "-.123",
                ".1234",
                "012.012",
                "123+456",
                "123+-234",
                "123*-345",
                "123*4*-3",
                "123*+4*-3",
                "1,234",
                "1 2 3",
                "\n\r1,234\n",
                "\f\n1+2\t\r\v\x85",
                "\n 1+\n2 ",
                "1\"2",
                "1234567891234567",
                "2+2=",
                "2+2=   ",
                "1.2e23",
                "12345e-23",
            };
            string[] negativeInput =
            {
                "(123)+(456)",
                "abcdef",
                "xyz",
                "ABab",
                "e+234",
                "12345678912345678",
                "SIN(2)",
                "2+2==",
                "2=+2",
                "2%2",
                "10^2",
            };

            foreach (var data in positiveInput)
                Assert.AreEqual(ValidateStandardPasteExpression(data), data);
            foreach (var data in negativeInput)
                Assert.AreEqual(ValidateStandardPasteExpression(data), "NoOp");
        }
```

##### `ValidateScientificPasteExpressionTest` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:85-144`

```csharp
        [TestMethod]
        public void ValidateScientificPasteExpressionTest()
        {
            string[] positiveInput =
            {
                "123",
                "+123",
                "-133",
                "123+456",
                "12345e+023",
                "1,234",
                "1.23",
                "-.123",
                ".1234",
                "012.012",
                "123+-234",
                "123*-345",
                "123*4*-3",
                "123*+4*-3",
                "1 2 3",
                "\n\r1,234\n",
                "\f\n1+2\t\r\v\x85",
                "\n 1+\n2 ",
                "1\"2",
                "1.2e+023",
                "12345e-23",
                "(123)+(456)",
                "12345678912345678123456789012345",
                "(123)+(456)=",
                "2+2=   ",
                "-(43)",
                "+(41213)",
                "-(432+3232)",
                "-(+(-3213)+(-2312))",
                "-(-(432+3232))",
                "1.2e23",
                "12^2",
                "-12.12^-2",
                "61%99" + "-6.1%99",
                "1.1111111111111111111111111111111e+1142",
            };
            string[] negativeInput =
            {
                "abcdef",
                "xyz",
                "ABab",
                "e+234",
                "123456789123456781234567890123456",
                "11.1111111111111111111111111111111e+1142",
                "1.1e+10001",
                "0.11111111111111111111111111111111111e+111111" + "SIN(2)",
                "2+2==",
                "2=+2",
            };

            foreach (var data in positiveInput)
                Assert.AreEqual(ValidateScientificPasteExpression(data), data);
            foreach (var data in negativeInput)
                Assert.AreEqual(ValidateScientificPasteExpression(data), "NoOp");
        }
```

##### `ScientificPasteRequiresANumericOperand` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:146-153`

```csharp
        [TestMethod]
        public void ScientificPasteRequiresANumericOperand()
        {
            foreach (string candidate in new[] { "", "   ", "+", "-" })
            {
                Assert.AreEqual("NoOp", ValidateScientificPasteExpression(candidate), candidate);
            }
        }
```

##### `ScientificPasteAllowsTerminalOperators` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:155-162`

```csharp
        [TestMethod]
        public void ScientificPasteAllowsTerminalOperators()
        {
            foreach (string candidate in new[] { "50%", "1+" })
            {
                Assert.AreEqual(candidate, ValidateScientificPasteExpression(candidate), candidate);
            }
        }
```

##### `ScientificPasteRejectsDigitsUnsupportedByCalculator` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:164-171`

```csharp
        [TestMethod]
        public void ScientificPasteRejectsDigitsUnsupportedByCalculator()
        {
            foreach (string candidate in new[] { "１", "1+１", "1e+１" })
            {
                Assert.AreEqual("NoOp", ValidateScientificPasteExpression(candidate), candidate);
            }
        }
```

##### `ValidateProgrammerDecPasteExpressionTest` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:173-268`

```csharp
        [TestMethod]
        public void ValidateProgrammerDecPasteExpressionTest()
        {
            // QWord
            string[] qwordPositiveInput =
            {
                "123", "+123", "-133", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "\n\r1,234\n", "\f\n1+2\t\r\v\x85", "\n 1+\n2 ", "1\"2",
                "(123)+(456)", "123+-234", "123*-345", "123*4*-3", "123*+4*-3",
                "9223372036854775807", "-9223372036854775808",
                "0n1234", "0N1234", "1234u", "1234ul", "1234ULL",
                "2+2=", "2+2=   ", "823%21",
            };
            string[] qwordNegativeInput =
            {
                "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "1.2e23", "1.2e+023", "12345e-23",
                "abcdef", "xyz", "ABab", "e+234",
                "9223372036854775808", "9223372036854775809",
                "SIN(2)", "-0n123", "0nn1234", "1234uu", "1234ulll",
                "2+2==", "2=+2",
            };

            foreach (var data in qwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerDecQwordPasteExpression(data), data);
            foreach (var data in qwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerDecQwordPasteExpression(data), "NoOp");

            // DWord
            string[] dwordPositiveInput =
            {
                "123", "+123", "-133", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "\n\r1,234\n", "\f\n1+2\t\r\v\x85", "\n 1+\n2 ", "1\"2",
                "(123)+(456)", "123+-234", "123*-345", "123*4*-3", "123*+4*-3",
                "2147483647", "-2147483647",
                "0n1234", "0N1234", "1234u", "1234ul", "1234ULL",
            };
            string[] dwordNegativeInput =
            {
                "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "1.2e23", "1.2e+023", "12345e-23",
                "abcdef", "xyz", "ABab", "e+234",
                "2147483649",
                "SIN(2)", "-0n123", "0nn1234", "1234uu", "1234ulll",
            };

            foreach (var data in dwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerDecDwordPasteExpression(data), data);
            foreach (var data in dwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerDecDwordPasteExpression(data), "NoOp");

            // Word
            string[] wordPositiveInput =
            {
                "123", "+123", "-133", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "\f\n1+2\t\r\v\x85", "1\"2",
                "(123)+(456)", "123+-234", "123*-345", "123*4*-3", "123*+4*-3",
                "32767", "-32767", "-32768",
                "0n1234", "0N1234", "1234u", "1234ul", "1234ULL",
            };
            string[] wordNegativeInput =
            {
                "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "1.2e23", "1.2e+023", "12345e-23",
                "abcdef", "xyz", "ABab", "e+234",
                "32769",
                "SIN(2)", "-0n123", "0nn1234", "1234uu", "1234ulll",
            };

            foreach (var data in wordPositiveInput)
                Assert.AreEqual(ValidateProgrammerDecWordPasteExpression(data), data);
            foreach (var data in wordNegativeInput)
                Assert.AreEqual(ValidateProgrammerDecWordPasteExpression(data), "NoOp");

            // Byte
            string[] bytePositiveInput =
            {
                "13", "+13", "-13", "13+46", "13+-34",
                "13*-3", "3*4*-3", "3*+4*-3", "1,3", "1 3",
                "1'2'3", "1_2_3", "1\"2", "127", "-127",
                "0n123", "0N123", "123u", "123ul", "123ULL",
            };
            string[] byteNegativeInput =
            {
                "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "1.2e23", "1.2e+023", "15e-23",
                "abcdef", "xyz", "ABab", "e+24",
                "129",
                "SIN(2)", "-0n123", "0nn1234", "123uu", "123ulll",
            };

            foreach (var data in bytePositiveInput)
                Assert.AreEqual(ValidateProgrammerDecBytePasteExpression(data), data);
            foreach (var data in byteNegativeInput)
                Assert.AreEqual(ValidateProgrammerDecBytePasteExpression(data), "NoOp");
        }
```

##### `ValidateProgrammerOctPasteExpressionTest` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:270-368`

```csharp
        [TestMethod]
        public void ValidateProgrammerOctPasteExpressionTest()
        {
            // QWord
            string[] qwordPositiveInput =
            {
                "123", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "\n\r1,234\n", "\f\n1+2\t\r\v\x85", "\n 1+\n2 ", "1\"2",
                "(123)+(456)", "0t1234", "0T1234", "0o1234", "0O1234",
                "1234u", "1234ul", "1234ULL",
                "2+2=", "2+2=   ", "127%71",
                "1777777777777777777777",
            };
            string[] qwordNegativeInput =
            {
                "+123", "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "-133", "1.2e23", "1.2e+023", "12345e-23",
                "abcdef", "xyz", "ABab", "e+234",
                "12345678901234567890123",
                "2000000000000000000000",
                "SIN(2)", "123+-234", "0ot1234", "1234uu", "1234ulll",
                "2+2==", "2=+2", "89%12",
            };

            foreach (var data in qwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerOctQwordPasteExpression(data), data);
            foreach (var data in qwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerOctQwordPasteExpression(data), "NoOp");

            // DWord
            string[] dwordPositiveInput =
            {
                "123", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "\n\r1,234\n", "\f\n1+2\t\r\v\x85", "\n 1+\n2 ", "1\"2",
                "(123)+(456)", "37777777777",
                "0t1234", "0T1234", "0o1234", "0O1234",
                "1234u", "1234ul", "1234ULL",
            };
            string[] dwordNegativeInput =
            {
                "+123", "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "-133", "1.2e23", "1.2e+023", "12345e-23",
                "abcdef", "xyz", "ABab", "e+234",
                "377777777771",
                "40000000000",
                "SIN(2)", "123+-234", "0ot1234", "1234uu", "1234ulll",
            };

            foreach (var data in dwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerOctDwordPasteExpression(data), data);
            foreach (var data in dwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerOctDwordPasteExpression(data), "NoOp");

            // Word
            string[] wordPositiveInput =
            {
                "123", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "\f\n1+2\t\r\v\x85", "1\"2",
                "(123)+(456)", "177777",
                "0t1234", "0T1234", "0o1234", "0O1234",
                "1234u", "1234ul", "1234ULL",
            };
            string[] wordNegativeInput =
            {
                "+123", "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "-133", "1.2e23", "1.2e+023", "12345e-23",
                "abcdef", "xyz", "ABab", "e+234",
                "1777771",
                "200000",
                "SIN(2)", "123+-234", "0ot1234", "1234uu", "1234ulll",
            };

            foreach (var data in wordPositiveInput)
                Assert.AreEqual(ValidateProgrammerOctWordPasteExpression(data), data);
            foreach (var data in wordNegativeInput)
                Assert.AreEqual(ValidateProgrammerOctWordPasteExpression(data), "NoOp");

            // Byte
            string[] bytePositiveInput =
            {
                "13", "13+46", "1,3", "1 3", "1'2'3", "1_2_3", "1\"2",
                "377",
                "0t123", "0T123", "0o123", "0O123", "123u", "123ul", "123ULL",
            };
            string[] byteNegativeInput =
            {
                "+123", "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "-13", "1.2e23", "1.2e+023", "15e-23",
                "abcdef", "xyz", "ABab", "e+24",
                "477",
                "400",
                "SIN(2)", "123+-34", "0ot123", "123uu", "123ulll",
            };

            foreach (var data in bytePositiveInput)
                Assert.AreEqual(ValidateProgrammerOctBytePasteExpression(data), data);
            foreach (var data in byteNegativeInput)
                Assert.AreEqual(ValidateProgrammerOctBytePasteExpression(data), "NoOp");
        }
```

##### `ValidateProgrammerHexPasteExpressionTest` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:370-486`

```csharp
        [TestMethod]
        public void ValidateProgrammerHexPasteExpressionTest()
        {
            // QWord
            string[] qwordPositiveInput =
            {
                "123", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "12345e-23",
                "\n\r1,234\n", "\f\n1+2\t\r\v\x85", "\f\n1+2\t\r\v\x85",
                "\n 1+\n2 ", "e+234", "1\"2",
                "(123)+(456)", "abcdef", "ABab",
                "ABCDF21abc41a",
                "0x1234", "0xab12", "0X1234",
                "AB12h", "BC34H",
                "1234u", "1234ul", "1234ULL",
                "2+2=", "2+2=   ",
                "A4C3%12", "1233%AB", "FFC1%F2",
            };
            string[] qwordNegativeInput =
            {
                "+123", "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "-133", "1.2e+023", "1.2e23",
                "xyz",
                "ABCDEF21abc41abc7",
                "SIN(2)", "123+-234", "1234x", "A0x1234", "0xx1234",
                "1234uu", "1234ulll",
                "2+2==", "2=+2",
            };

            foreach (var data in qwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerHexQwordPasteExpression(data), data);
            foreach (var data in qwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerHexQwordPasteExpression(data), "NoOp");

            // DWord
            string[] dwordPositiveInput =
            {
                "123", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "12345e-23",
                "\n\r1,234\n", "\f\n1+2\t\r\v\x85", "\n 1+\n2 ",
                "e+234", "1\"2",
                "(123)+(456)", "abcdef", "ABab",
                "ABCD123a",
                "0x1234", "0xab12", "0X1234",
                "AB12h", "BC34H",
                "1234u", "1234ul", "1234ULL",
            };
            string[] dwordNegativeInput =
            {
                "+123", "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "-133", "1.2e+023", "1.2e23",
                "xyz",
                "ABCD123ab",
                "SIN(2)", "123+-234", "1234x", "A0x1234", "0xx1234",
                "1234uu", "1234ulll",
            };

            foreach (var data in dwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerHexDwordPasteExpression(data), data);
            foreach (var data in dwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerHexDwordPasteExpression(data), "NoOp");

            // Word
            string[] wordPositiveInput =
            {
                "123", "13+456", "1,34", "12 3", "1'2'3'4", "1_2_3_4",
                "15e-23",
                "\r1", "\n\r1,4", "\n1,4\n",
                "\f\n1+2\t\r\v", "\n 1+\n2 ",
                "e+24", "1\"2",
                "(23)+(4)", "aef", "ABab",
                "A1a3",
                "FFFF",
                "0x1234", "0xab12", "0X1234",
                "AB12h", "BC34H",
                "1234u", "1234ul", "1234ULL",
            };
            string[] wordNegativeInput =
            {
                "+123", "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "-133", "1.2e+023", "1.2e23",
                "xyz",
                "A1a3b",
                "SIN(2)", "123+-234", "1234x", "A0x1234", "0xx1234",
                "1234uu", "1234ulll",
            };

            foreach (var data in wordPositiveInput)
                Assert.AreEqual(ValidateProgrammerHexWordPasteExpression(data), data);
            foreach (var data in wordNegativeInput)
                Assert.AreEqual(ValidateProgrammerHexWordPasteExpression(data), "NoOp");

            // Byte
            string[] bytePositiveInput =
            {
                "13", "13+6", "1,4", "2 3", "1'2", "1_2",
                "5e-3",
                "\r1", "a", "ab", "A1",
                "0x12", "0xab", "0X12",
                "A9h", "B8H",
                "12u", "12ul", "12ULL",
            };
            string[] byteNegativeInput =
            {
                "+3", "1.2", "1''2", "'12", "12'", "1__2", "_12", "12_",
                "-3", "1.1e+02", "1.2e3",
                "xz",
                "A3a",
                "SIN(2)", "13+-23", "12x", "A0x1", "0xx12",
                "12uu", "12ulll",
            };

            foreach (var data in bytePositiveInput)
                Assert.AreEqual(ValidateProgrammerHexBytePasteExpression(data), data);
            foreach (var data in byteNegativeInput)
                Assert.AreEqual(ValidateProgrammerHexBytePasteExpression(data), "NoOp");
        }
```

##### `ValidateProgrammerBinPasteExpressionTest` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:488-601`

```csharp
        [TestMethod]
        public void ValidateProgrammerBinPasteExpressionTest()
        {
            // QWord
            string[] qwordPositiveInput =
            {
                "100", "100+101", "1,001", "1 0 1", "1'0'0'1", "1_0_0_1",
                "\n\r1,010\n", "\f\n1+11\t\r\v\x85", "\n 1+\n1 ", "1\"1",
                "(101)+(10)",
                "0b1001", "0B1111", "0y1001", "0Y1001",
                "1100b", "1101B",
                "1111u", "1111ul", "1111ULL",
                "1010101010101010101010101011110110100100101010101001010101001010",
                "1+10=", "1+10=   ", "1001%10",
            };
            string[] qwordNegativeInput =
            {
                "+10101", "1.01", "1''0", "'101", "101'", "1__0", "_101", "101_",
                "-10101001", "123", "1.2e23", "1.2e+023", "101010e-1010",
                "abcdef", "xyz", "ABab", "e+10101",
                "b1001", "10b01", "0x10", "1001x", "1001h",
                "0bb1111", "1111uu", "1111ulll",
                "10101010101010101010101010111101101001001010101010010101010010100",
                "SIN(01010)", "10+-10101010101",
                "1+10==", "1=+10",
            };

            foreach (var data in qwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerBinQwordPasteExpression(data), data);
            foreach (var data in qwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerBinQwordPasteExpression(data), "NoOp");

            // DWord
            string[] dwordPositiveInput =
            {
                "100", "100+101", "1,001", "1 0 1", "1'0'0'1", "1_0_0_1",
                "\n\r1,010\n", "\f\n1+11\t\r\v\x85", "\n 1+\n1 ", "1\"1",
                "(101)+(10)",
                "0b1001", "0B1111", "0y1001", "0Y1001",
                "1100b", "1101B",
                "1111u", "1111ul", "1111ULL",
                "10101001001010101101010111111100",
            };
            string[] dwordNegativeInput =
            {
                "+10101", "1.01", "1''0", "'101", "101'", "1__0", "_101", "101_",
                "-10101001", "123", "1.2e23", "1.2e+023", "101010e-1010",
                "abcdef", "xyz", "ABab", "e+10101",
                "b1001", "10b01", "0x10", "1001x", "1001h",
                "0bb1111", "1111uu", "1111ulll",
                "101010010010101011010101111111001",
                "SIN(01010)", "10+-10101010101",
            };

            foreach (var data in dwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerBinDwordPasteExpression(data), data);
            foreach (var data in dwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerBinDwordPasteExpression(data), "NoOp");

            // Word
            string[] wordPositiveInput =
            {
                "100", "100+101", "1,001", "1 0 1", "1'0'0'1", "1_0_0_1",
                "\n\r1,010\n", "\f\n1+11\t\r\v\x85", "\n 1+\n1 ", "1\"1",
                "(101)+(10)",
                "0b1001", "0B1111", "0y1001", "0Y1001",
                "1100b", "1101B",
                "1111u", "1111ul", "1111ULL",
                "1010101010010010",
            };
            string[] wordNegativeInput =
            {
                "+10101", "1.01", "1''0", "'101", "101'", "1__0", "_101", "101_",
                "-10101001", "123", "1.2e23", "1.2e+023", "101010e-1010",
                "abcdef", "xyz", "ABab", "e+10101",
                "b1001", "10b01", "0x10", "1001x", "1001h",
                "0bb1111", "1111uu", "1111ulll",
                "10101010100100101",
                "SIN(01010)", "10+-10101010101",
            };

            foreach (var data in wordPositiveInput)
                Assert.AreEqual(ValidateProgrammerBinWordPasteExpression(data), data);
            foreach (var data in wordNegativeInput)
                Assert.AreEqual(ValidateProgrammerBinWordPasteExpression(data), "NoOp");

            // Byte
            string[] bytePositiveInput =
            {
                "100", "100+101", "1,001", "1 0 1", "1'0'0'1", "1_0_0_1",
                "\n\r1,010\n", "\n 1+\n1 ", "1\"1",
                "(101)+(10)",
                "0b1001", "0B1111", "0y1001", "0Y1001",
                "1100b", "1101B",
                "1111u", "1111ul", "1111ULL",
                "10100010", "11111111",
            };
            string[] byteNegativeInput =
            {
                "+10101", "1.01", "1''0", "'101", "101'", "1__0", "_101", "101_",
                "-10101001", "123", "1.2e23", "1.2e+023", "101010e-1010",
                "abcdef", "xyz", "ABab", "e+10101",
                "b1001", "10b01", "0x10", "1001x", "1001h",
                "0bb1111", "1111uu", "1111ulll",
                "101000101",
                "100000000",
                "SIN(01010)", "10+-1010101",
            };

            foreach (var data in bytePositiveInput)
                Assert.AreEqual(ValidateProgrammerBinBytePasteExpression(data), data);
            foreach (var data in byteNegativeInput)
                Assert.AreEqual(ValidateProgrammerBinBytePasteExpression(data), "NoOp");
        }
```

##### `ProgrammerPrefixedValuesRespectByteAndWordRanges` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:603-637`

```csharp
        [TestMethod]
        public void ProgrammerPrefixedValuesRespectByteAndWordRanges()
        {
            var cases = new[]
            {
                (Value: "0n127", Base: NumberBase.DecBase, Length: BitLength.BitLengthByte, Expected: "0n127"),
                (Value: "0n128", Base: NumberBase.DecBase, Length: BitLength.BitLengthByte, Expected: "NoOp"),
                (Value: "0n32767", Base: NumberBase.DecBase, Length: BitLength.BitLengthWord, Expected: "0n32767"),
                (Value: "0n32768", Base: NumberBase.DecBase, Length: BitLength.BitLengthWord, Expected: "NoOp"),
                (Value: "0o377", Base: NumberBase.OctBase, Length: BitLength.BitLengthByte, Expected: "0o377"),
                (Value: "0o400", Base: NumberBase.OctBase, Length: BitLength.BitLengthByte, Expected: "NoOp"),
                (Value: "0t177777", Base: NumberBase.OctBase, Length: BitLength.BitLengthWord, Expected: "0t177777"),
                (Value: "0t200000", Base: NumberBase.OctBase, Length: BitLength.BitLengthWord, Expected: "NoOp"),
                (Value: "0b11111111", Base: NumberBase.BinBase, Length: BitLength.BitLengthByte, Expected: "0b11111111"),
                (Value: "0b100000000", Base: NumberBase.BinBase, Length: BitLength.BitLengthByte, Expected: "NoOp"),
                (Value: "0y1111111111111111", Base: NumberBase.BinBase, Length: BitLength.BitLengthWord, Expected: "0y1111111111111111"),
                (Value: "0y10000000000000000", Base: NumberBase.BinBase, Length: BitLength.BitLengthWord, Expected: "NoOp"),
                (Value: "0xFF", Base: NumberBase.HexBase, Length: BitLength.BitLengthByte, Expected: "0xFF"),
                (Value: "0x100", Base: NumberBase.HexBase, Length: BitLength.BitLengthByte, Expected: "NoOp"),
                (Value: "0xFFFF", Base: NumberBase.HexBase, Length: BitLength.BitLengthWord, Expected: "0xFFFF"),
                (Value: "0x10000", Base: NumberBase.HexBase, Length: BitLength.BitLengthWord, Expected: "NoOp"),
            };

            foreach (var testCase in cases)
            {
                Assert.AreEqual(
                    testCase.Expected,
                    CopyPasteManager.ValidatePasteExpression(
                        testCase.Value,
                        ViewMode.Programmer,
                        testCase.Base,
                        testCase.Length),
                    testCase.Value);
            }
        }
```

##### `BinaryZeroWithSuffixRemainsValid` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:639-643`

```csharp
        [TestMethod]
        public void BinaryZeroWithSuffixRemainsValid()
        {
            Assert.AreEqual("0b", ValidateProgrammerBinBytePasteExpression("0b"));
        }
```

##### `ValidateConverterPasteExpressionTest` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:645-666`

```csharp
        [TestMethod]
        public void ValidateConverterPasteExpressionTest()
        {
            string[] positiveInput =
            {
                "123", "+123", "-133", "12345.", "012.012", "1,234", "1 2 3",
                "\n\r1,234\n", "\f\n12\t\r\v\x85", "1\"2", "100=", "100=   ",
            };
            string[] negativeInput =
            {
                "(123)+(456)", "1.2e23", "12345e-23",
                "\n 1+\n2 ", "123+456", "abcdef", "\n 1+\n2 ",
                "xyz", "ABab", "e+234",
                "12345678912345678",
                "SIN(2)", "123+-234", "100==", "=100",
            };

            foreach (var data in positiveInput)
                Assert.AreEqual(ValidateConverterPasteExpression(data), data);
            foreach (var data in negativeInput)
                Assert.AreEqual(ValidateConverterPasteExpression(data), "NoOp");
        }
```

##### `ValidatePasteExpressionErrorStates` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:668-700`

```csharp
        [TestMethod]
        public void ValidatePasteExpressionErrorStates()
        {
            string exp_TooLong = "";
            for (uint i = 0; i < CopyPasteManager.MaxPasteableLength / 8; i++)
            {
                exp_TooLong += "-1234567";
            }
            Assert.AreEqual(
                CopyPasteManager.ValidatePasteExpression(
                    exp_TooLong, ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown),
                exp_TooLong);
            exp_TooLong += "1";
            Assert.AreEqual(
                CopyPasteManager.ValidatePasteExpression(
                    exp_TooLong, ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown),
                "NoOp");

            Assert.AreEqual(
                CopyPasteManager.ValidatePasteExpression(
                    "", ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown),
                "NoOp");

            Assert.AreEqual(
                CopyPasteManager.ValidatePasteExpression(
                    "1a23f456", ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown),
                "NoOp");

            Assert.AreEqual(
                CopyPasteManager.ValidatePasteExpression(
                    "123", ViewMode.None, CategoryGroupType.None, NumberBase.Unknown, BitLength.BitLengthUnknown),
                "NoOp");
        }
```

##### `ValidateExtractOperands` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:702-726`

```csharp
        [TestMethod]
        public void ValidateExtractOperands()
        {
            var oneOperand = new List<string> { "123456" };
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123456", ViewMode.Standard), oneOperand);

            oneOperand.Clear();
            oneOperand.Add("123^456");
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123^456", ViewMode.Standard), oneOperand);

            var twoOperands = new List<string> { "123", "456" };
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123+456", ViewMode.Standard), twoOperands);
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123-456", ViewMode.Standard), twoOperands);
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123*456", ViewMode.Standard), twoOperands);
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123/456", ViewMode.Standard), twoOperands);

            var expOperand = new List<string> { "123e456" };
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123e456", ViewMode.Standard), expOperand);
            expOperand.Clear();
            expOperand.Add("123e4567");
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123e4567", ViewMode.Standard), expOperand);

            var twoOperandsParens = new List<string> { "((45)", "(-30))" };
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("((45)+(-30))", ViewMode.Scientific), twoOperandsParens);
        }
```

##### `ValidateExtractOperandsErrors` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:728-751`

```csharp
        [TestMethod]
        public void ValidateExtractOperandsErrors()
        {
            string exp_OperandLimit = "";
            for (uint i = 0; i < CopyPasteManager.MaxOperandCount; i++)
            {
                exp_OperandLimit += "+1";
            }
            Assert.AreEqual(
                (uint)CopyPasteManager.ExtractOperands(exp_OperandLimit, ViewMode.Standard).Count,
                (uint)100);

            exp_OperandLimit += "+1";
            Assert.AreEqual(
                (uint)CopyPasteManager.ExtractOperands(exp_OperandLimit, ViewMode.Standard).Count,
                (uint)0);

            Assert.AreEqual(
                (uint)CopyPasteManager.ExtractOperands("12e9999", ViewMode.Standard).Count,
                (uint)1);
            Assert.AreEqual(
                (uint)CopyPasteManager.ExtractOperands("12e10000", ViewMode.Standard).Count,
                (uint)0);
        }
```

##### `ValidateExpressionRegExMatch` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:753-833`

```csharp
        [TestMethod]
        public void ValidateExpressionRegExMatch()
        {
            Assert.IsFalse(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string>(), ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsFalse(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "123" },
                    ViewMode.None, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsFalse(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "123" },
                    ViewMode.Currency, CategoryGroupType.None, NumberBase.Unknown, BitLength.BitLengthUnknown));

            // Verify operand lengths > max return false
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "12345678901234567" },
                ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "123456789012345678901234567890123" },
                ViewMode.Scientific, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "12345678901234567" },
                ViewMode.None, CategoryGroupType.Converter, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "11111111111111111" },
                ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.HexBase, BitLength.BitLengthQWord));
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "12345678901234567890" },
                ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.DecBase, BitLength.BitLengthQWord));
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "11111111111111111111111" },
                ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.OctBase, BitLength.BitLengthQWord));
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "10000000000000000000000000000000000000000000000000000000000000000" },
                ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.BinBase, BitLength.BitLengthQWord));

            Assert.IsFalse(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "9223372036854775808" },
                    ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.DecBase, BitLength.BitLengthQWord));

            Assert.IsTrue(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "((((((((((((((((((((123))))))))))))))))))))" },
                    ViewMode.Scientific, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsTrue(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "9223372036854775807" },
                    ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.DecBase, BitLength.BitLengthQWord));
            Assert.IsTrue(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "-9223372036854775808" },
                    ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.DecBase, BitLength.BitLengthQWord));

            // Verify all operands must match patterns
            Assert.IsTrue(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "123", "456" },
                ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsTrue(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "123", "1e23" },
                ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "123", "fab10" },
                ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));

            Assert.IsTrue(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "1.23e+456", "1.23e456", ".23e+456", "123e-456", "12e2", "12e+2", "12e-2", "-12e2", "-12e+2", "-12e-2" },
                    ViewMode.Scientific, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));

            Assert.IsFalse(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "123", "12345678901234567" },
                    ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsFalse(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "123", "9223372036854775808" },
                    ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.DecBase, BitLength.BitLengthQWord));
        }
```

##### `ValidateGetMaxOperandLengthAndValue` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:835-918`

```csharp
        [TestMethod]
        public void ValidateGetMaxOperandLengthAndValue()
        {
            var result = CopyPasteManager.GetMaxOperandLengthAndValue(
                ViewMode.Standard, CategoryGroupType.None, NumberBase.Unknown, BitLength.BitLengthUnknown);
            Assert.IsTrue(result.MaxLength == CopyPasteManager.MaxStandardOperandLength);
            Assert.IsTrue(result.MaxValue == 0);

            result = CopyPasteManager.GetMaxOperandLengthAndValue(
                ViewMode.Scientific, CategoryGroupType.None, NumberBase.Unknown, BitLength.BitLengthUnknown);
            Assert.IsTrue(result.MaxLength == CopyPasteManager.MaxScientificOperandLength);
            Assert.IsTrue(result.MaxValue == 0);

            result = CopyPasteManager.GetMaxOperandLengthAndValue(
                ViewMode.None, CategoryGroupType.Converter, NumberBase.Unknown, BitLength.BitLengthUnknown);
            Assert.IsTrue(result.MaxLength == CopyPasteManager.MaxConverterInputLength);
            Assert.IsTrue(result.MaxValue == 0);

            ulong ullQwordMax = ulong.MaxValue;
            ulong ullDwordMax = uint.MaxValue;
            ulong ullWordMax = ushort.MaxValue;
            ulong ullByteMax = byte.MaxValue;

            // Hex
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.HexBase, BitLength.BitLengthQWord);
            Assert.IsTrue(result.MaxLength == 16u);
            Assert.IsTrue(result.MaxValue == ullQwordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.HexBase, BitLength.BitLengthDWord);
            Assert.IsTrue(result.MaxLength == 8u);
            Assert.IsTrue(result.MaxValue == ullDwordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.HexBase, BitLength.BitLengthWord);
            Assert.IsTrue(result.MaxLength == 4u);
            Assert.IsTrue(result.MaxValue == ullWordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.HexBase, BitLength.BitLengthByte);
            Assert.IsTrue(result.MaxLength == 2u);
            Assert.IsTrue(result.MaxValue == ullByteMax);

            // Dec
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.DecBase, BitLength.BitLengthQWord);
            Assert.IsTrue(result.MaxLength == 19u);
            Assert.IsTrue(result.MaxValue == ullQwordMax >> 1);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.DecBase, BitLength.BitLengthDWord);
            Assert.IsTrue(result.MaxLength == 10u);
            Assert.IsTrue(result.MaxValue == ullDwordMax >> 1);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.DecBase, BitLength.BitLengthWord);
            Assert.IsTrue(result.MaxLength == 5u);
            Assert.IsTrue(result.MaxValue == ullWordMax >> 1);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.DecBase, BitLength.BitLengthByte);
            Assert.IsTrue(result.MaxLength == 3u);
            Assert.IsTrue(result.MaxValue == ullByteMax >> 1);

            // Oct
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.OctBase, BitLength.BitLengthQWord);
            Assert.IsTrue(result.MaxLength == 22u);
            Assert.IsTrue(result.MaxValue == ullQwordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.OctBase, BitLength.BitLengthDWord);
            Assert.IsTrue(result.MaxLength == 11u);
            Assert.IsTrue(result.MaxValue == ullDwordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.OctBase, BitLength.BitLengthWord);
            Assert.IsTrue(result.MaxLength == 6u);
            Assert.IsTrue(result.MaxValue == ullWordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.OctBase, BitLength.BitLengthByte);
            Assert.IsTrue(result.MaxLength == 3u);
            Assert.IsTrue(result.MaxValue == ullByteMax);

            // Bin
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.BinBase, BitLength.BitLengthQWord);
            Assert.IsTrue(result.MaxLength == 64u);
            Assert.IsTrue(result.MaxValue == ullQwordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.BinBase, BitLength.BitLengthDWord);
            Assert.IsTrue(result.MaxLength == 32u);
            Assert.IsTrue(result.MaxValue == ullDwordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.BinBase, BitLength.BitLengthWord);
            Assert.IsTrue(result.MaxLength == 16u);
            Assert.IsTrue(result.MaxValue == ullWordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.BinBase, BitLength.BitLengthByte);
            Assert.IsTrue(result.MaxLength == 8u);
            Assert.IsTrue(result.MaxValue == ullByteMax);

            // Invalid
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.None, CategoryGroupType.None, NumberBase.Unknown, BitLength.BitLengthUnknown);
            Assert.IsTrue(result.MaxLength == 0u);
            Assert.IsTrue(result.MaxValue == 0UL);
        }
```

##### `ValidateSanitizeOperand` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:920-936`

```csharp
        [TestMethod]
        public void ValidateSanitizeOperand()
        {
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("((1234"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("1234))"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("1234))"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("-1234"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("+1234"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("-(1234)"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("+(1234)"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("12-34"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("((((1234))))"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("1'2'3'4"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("'''''1234''''"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("1_2_3_4"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("______1234___"), "1234");
        }
```

##### `ValidatePrefixCurrencySymbols` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:938-952`

```csharp
        [TestMethod]
        public void ValidatePrefixCurrencySymbols()
        {
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u00A5\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u00A4\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u20B5\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u0024\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u20A1\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u20A9\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u20AA\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u20A6\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u20B9\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u00A3\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u20AC\u0035"), "5");
        }
```

##### `ValidateTryOperandToULL` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:954-1026`

```csharp
        [TestMethod]
        public void ValidateTryOperandToULL()
        {
            ulong? result = null;

            // Hex
            result = CopyPasteManager.TryOperandToULL("1234", NumberBase.HexBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0x1234UL);
            result = CopyPasteManager.TryOperandToULL("FF", NumberBase.HexBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFUL);
            result = CopyPasteManager.TryOperandToULL("FFFFFFFFFFFFFFFF", NumberBase.HexBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);
            result = CopyPasteManager.TryOperandToULL("0xFFFFFFFFFFFFFFFF", NumberBase.HexBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);
            result = CopyPasteManager.TryOperandToULL("0XFFFFFFFFFFFFFFFF", NumberBase.HexBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);
            result = CopyPasteManager.TryOperandToULL("0X0FFFFFFFFFFFFFFFF", NumberBase.HexBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);

            // Dec
            result = CopyPasteManager.TryOperandToULL("1234", NumberBase.DecBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 1234UL);
            result = CopyPasteManager.TryOperandToULL("18446744073709551615", NumberBase.DecBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);
            result = CopyPasteManager.TryOperandToULL("018446744073709551615", NumberBase.DecBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);

            // Oct
            result = CopyPasteManager.TryOperandToULL("777", NumberBase.OctBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 511UL); // 0777 octal
            result = CopyPasteManager.TryOperandToULL("0777", NumberBase.OctBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 511UL);
            result = CopyPasteManager.TryOperandToULL("1777777777777777777777", NumberBase.OctBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);
            result = CopyPasteManager.TryOperandToULL("01777777777777777777777", NumberBase.OctBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);

            // Bin
            result = CopyPasteManager.TryOperandToULL("1111", NumberBase.BinBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0b1111UL);
            result = CopyPasteManager.TryOperandToULL("0010", NumberBase.BinBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0b10UL);
            result = CopyPasteManager.TryOperandToULL("1111111111111111111111111111111111111111111111111111111111111111", NumberBase.BinBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);
            result = CopyPasteManager.TryOperandToULL("01111111111111111111111111111111111111111111111111111111111111111", NumberBase.BinBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);

            // Invalid / overflow
            Assert.IsNull(CopyPasteManager.TryOperandToULL("0xFFFFFFFFFFFFFFFFF1", NumberBase.HexBase));
            Assert.IsNull(CopyPasteManager.TryOperandToULL("18446744073709551616", NumberBase.DecBase));
            Assert.IsNull(CopyPasteManager.TryOperandToULL("2000000000000000000000", NumberBase.OctBase));
            Assert.IsNull(CopyPasteManager.TryOperandToULL("11111111111111111111111111111111111111111111111111111111111111111", NumberBase.BinBase));
            Assert.IsNull(CopyPasteManager.TryOperandToULL("-1", NumberBase.DecBase));
            Assert.IsNull(CopyPasteManager.TryOperandToULL("5555", NumberBase.BinBase));
            Assert.IsNull(CopyPasteManager.TryOperandToULL("xyz", NumberBase.BinBase));
        }
```

##### `ValidateStandardScientificOperandLength` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:1028-1037`

```csharp
        [TestMethod]
        public void ValidateStandardScientificOperandLength()
        {
            Assert.AreEqual(CopyPasteManager.StandardScientificOperandLength(""), (uint)0);
            Assert.AreEqual(CopyPasteManager.StandardScientificOperandLength("0.2"), (uint)1);
            Assert.AreEqual(CopyPasteManager.StandardScientificOperandLength("1.2"), (uint)2);
            Assert.AreEqual(CopyPasteManager.StandardScientificOperandLength("0."), (uint)0);
            Assert.AreEqual(CopyPasteManager.StandardScientificOperandLength("12345"), (uint)5);
            Assert.AreEqual(CopyPasteManager.StandardScientificOperandLength("-12345"), (uint)6);
        }
```

##### `ValidateProgrammerOperandLength` — `CopyPasteManagerTests` — `CopyPasteManagerTests.cs:1039-1069`

```csharp
        [TestMethod]
        public void ValidateProgrammerOperandLength()
        {
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("1001", NumberBase.BinBase), (uint)4);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("1001b", NumberBase.BinBase), (uint)4);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("1001B", NumberBase.BinBase), (uint)4);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0b1001", NumberBase.BinBase), (uint)4);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0B1001", NumberBase.BinBase), (uint)4);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0y1001", NumberBase.BinBase), (uint)4);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0Y1001", NumberBase.BinBase), (uint)4);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0b", NumberBase.BinBase), (uint)1);

            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("123456", NumberBase.OctBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0t123456", NumberBase.OctBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0T123456", NumberBase.OctBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0o123456", NumberBase.OctBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0O123456", NumberBase.OctBase), (uint)6);

            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("", NumberBase.DecBase), (uint)0);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("-", NumberBase.DecBase), (uint)0);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("12345", NumberBase.DecBase), (uint)5);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("-12345", NumberBase.DecBase), (uint)5);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0n12345", NumberBase.DecBase), (uint)5);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0N12345", NumberBase.DecBase), (uint)5);

            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("123ABC", NumberBase.HexBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0x123ABC", NumberBase.HexBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0X123ABC", NumberBase.HexBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("123ABCh", NumberBase.HexBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("123ABCH", NumberBase.HexBase), (uint)6);
        }
```

##### `TestLocaleName` — `LocalizationSettingsTests` — `LocalizationSettingsTests.cs:20-26`

```csharp
        [TestMethod]
        public void TestLocaleName()
        {
            var formatter = CreateDecimalFormatter("en-US", "US");
            var settings = new LocalizationSettings(formatter);
            Assert.AreEqual("en-US", settings.GetLocaleName());
        }
```

##### `TestIsDigitEnUsSetting` — `LocalizationSettingsTests` — `LocalizationSettingsTests.cs:28-38`

```csharp
        [TestMethod]
        public void TestIsDigitEnUsSetting()
        {
            var arabicFormatter = CreateDecimalFormatter("ar-AE", "AE");
            var arabicSettings = new LocalizationSettings(arabicFormatter);
            Assert.IsFalse(arabicSettings.IsDigitEnUsSetting());

            var englishFormatter = CreateDecimalFormatter("en-US", "US");
            var englishSettings = new LocalizationSettings(englishFormatter);
            Assert.IsTrue(englishSettings.IsDigitEnUsSetting());
        }
```

##### `TestLocalizeDisplayValue` — `LocalizationSettingsTests` — `LocalizationSettingsTests.cs:40-48`

```csharp
        [TestMethod]
        public void TestLocalizeDisplayValue()
        {
            var formatter = CreateDecimalFormatter("ar-AE", "AE");
            var settings = new LocalizationSettings(formatter);
            string input = "A123";
            settings.LocalizeDisplayValue(ref input);
            Assert.AreEqual("A\u0661\u0662\u0663", input);
        }
```

##### `TestGetEnglishValueFromLocalizedDigits` — `LocalizationSettingsTests` — `LocalizationSettingsTests.cs:50-56`

```csharp
        [TestMethod]
        public void TestGetEnglishValueFromLocalizedDigits()
        {
            var formatter = CreateDecimalFormatter("ar-AE", "AE");
            var settings = new LocalizationSettings(formatter);
            Assert.AreEqual("A123", settings.GetEnglishValueFromLocalizedDigits("A\u0661\u0662\u0663"));
        }
```

##### `TestIsEnUsDigit` — `LocalizationSettingsTests` — `LocalizationSettingsTests.cs:58-68`

```csharp
        [TestMethod]
        public void TestIsEnUsDigit()
        {
            var settings = LocalizationSettings.GetInstance();
            Assert.IsFalse(settings.IsEnUsDigit('/'));
            Assert.IsTrue(settings.IsEnUsDigit('0'));
            Assert.IsTrue(settings.IsEnUsDigit('1'));
            Assert.IsTrue(settings.IsEnUsDigit('8'));
            Assert.IsTrue(settings.IsEnUsDigit('9'));
            Assert.IsFalse(settings.IsEnUsDigit(':'));
        }
```

##### `TestIsLocalizedDigit` — `LocalizationSettingsTests` — `LocalizationSettingsTests.cs:70-77`

```csharp
        [TestMethod]
        public void TestIsLocalizedDigit()
        {
            var formatter = CreateDecimalFormatter("en-US", "US");
            var settings = new LocalizationSettings(formatter);
            Assert.IsTrue(settings.IsLocalizedDigit('0'));
            Assert.IsFalse(settings.IsLocalizedDigit('A'));
        }
```

##### `TestIsLocalizedHexDigit` — `LocalizationSettingsTests` — `LocalizationSettingsTests.cs:79-87`

```csharp
        [TestMethod]
        public void TestIsLocalizedHexDigit()
        {
            var formatter = CreateDecimalFormatter("en-US", "US");
            var settings = new LocalizationSettings(formatter);
            Assert.IsTrue(settings.IsLocalizedHexDigit('0'));
            Assert.IsTrue(settings.IsLocalizedHexDigit('A'));
            Assert.IsFalse(settings.IsLocalizedHexDigit('G'));
        }
```

##### `TestRemoveGroupSeparators` — `LocalizationSettingsTests` — `LocalizationSettingsTests.cs:89-95`

```csharp
        [TestMethod]
        public void TestRemoveGroupSeparators()
        {
            var formatter = CreateDecimalFormatter("en-US", "US");
            var settings = new LocalizationSettings(formatter);
            Assert.AreEqual("1000000", settings.RemoveGroupSeparators("1,000 000"));
        }
```

##### `InvalidModeSetsLaunchError` — `SnapshotJsonTests` — `SnapshotJsonTests.cs:22-34`

```csharp
        [DataTestMethod]
        [DataRow(-1)]
        [DataRow(int.MaxValue)]
        public void InvalidModeSetsLaunchError(int mode)
        {
            var alias = new ApplicationSnapshotAlias();
            alias.Mode = mode;

            var result = SnapshotLaunchArguments.FromJson(JsonSerializer.Serialize(alias));

            Assert.IsTrue(result.HasError);
            Assert.IsNull(result.Snapshot);
        }
```

##### `CalculatorModeWithoutStateSetsLaunchError` — `SnapshotJsonTests` — `SnapshotJsonTests.cs:36-49`

```csharp
        [DataTestMethod]
        [DataRow((int)ViewMode.Standard)]
        [DataRow((int)ViewMode.Scientific)]
        [DataRow((int)ViewMode.Programmer)]
        public void CalculatorModeWithoutStateSetsLaunchError(int mode)
        {
            var alias = new ApplicationSnapshotAlias();
            alias.Mode = mode;

            var result = SnapshotLaunchArguments.FromJson(JsonSerializer.Serialize(alias));

            Assert.IsTrue(result.HasError);
            Assert.IsNull(result.Snapshot);
        }
```

##### `InvalidHistoryTokenIndexSetsLaunchError` — `SnapshotJsonTests` — `SnapshotJsonTests.cs:51-67`

```csharp
        [DataTestMethod]
        [DataRow(-2)]
        [DataRow(1)]
        public void InvalidHistoryTokenIndexSetsLaunchError(int commandIndex)
        {
            var snapshot = CreateApplicationSnapshot();
            var item = new CalcManagerHistoryItem();
            item.Commands.Add(new ExpressionCommandWrapper(
                CommandType.BinaryCommand, 93, System.Array.Empty<int>(), false, false, false));
            item.Tokens.Add(new CalcManagerToken("+", commandIndex));
            snapshot.StandardCalculator.CalcManager.HistoryItems = new List<CalcManagerHistoryItem> { item };

            var result = ParseSnapshot(snapshot);

            Assert.IsTrue(result.HasError);
            Assert.IsNull(result.Snapshot);
        }
```

##### `InvalidExpressionTokenIndexSetsLaunchError` — `SnapshotJsonTests` — `SnapshotJsonTests.cs:69-84`

```csharp
        [DataTestMethod]
        [DataRow(-2)]
        [DataRow(1)]
        public void InvalidExpressionTokenIndexSetsLaunchError(int commandIndex)
        {
            var snapshot = CreateApplicationSnapshot();
            snapshot.StandardCalculator.ExpressionDisplay = new ExpressionDisplaySnapshot();
            snapshot.StandardCalculator.ExpressionDisplay.Commands.Add(new ExpressionCommandWrapper(
                CommandType.BinaryCommand, 93, System.Array.Empty<int>(), false, false, false));
            snapshot.StandardCalculator.ExpressionDisplay.Tokens.Add(new CalcManagerToken("+", commandIndex));

            var result = ParseSnapshot(snapshot);

            Assert.IsTrue(result.HasError);
            Assert.IsNull(result.Snapshot);
        }
```

##### `NullPrimaryDisplayValueSetsLaunchError` — `SnapshotJsonTests` — `SnapshotJsonTests.cs:86-96`

```csharp
        [TestMethod]
        public void NullPrimaryDisplayValueSetsLaunchError()
        {
            var snapshot = CreateApplicationSnapshot();
            snapshot.StandardCalculator.PrimaryDisplay.DisplayValue = null;

            var result = ParseSnapshot(snapshot);

            Assert.IsTrue(result.HasError);
            Assert.IsNull(result.Snapshot);
        }
```

##### `InvalidDisplayCommandSetsLaunchError` — `SnapshotJsonTests` — `SnapshotJsonTests.cs:98-116`

```csharp
        [DataTestMethod]
        [DataRow((int)CalculatorCommand.ModeProgrammer)]
        [DataRow(int.MaxValue)]
        public void InvalidDisplayCommandSetsLaunchError(int command)
        {
            var snapshot = CreateApplicationSnapshot();
            snapshot.StandardCalculator.DisplayCommands.Add(new ExpressionCommandWrapper(
                CommandType.BinaryCommand,
                command,
                System.Array.Empty<int>(),
                false,
                false,
                false));

            var result = ParseSnapshot(snapshot);

            Assert.IsTrue(result.HasError);
            Assert.IsNull(result.Snapshot);
        }
```

##### `ValidDisplayCommandsAreAccepted` — `SnapshotJsonTests` — `SnapshotJsonTests.cs:118-160`

```csharp
        [TestMethod]
        public void ValidDisplayCommandsAreAccepted()
        {
            var snapshot = CreateApplicationSnapshot();
            snapshot.StandardCalculator.DisplayCommands.Add(new ExpressionCommandWrapper(
                CommandType.UnaryCommand,
                0,
                new[] { (int)NumbersAndOperatorsEnum.Degree, (int)CalculatorCommand.CommandSIN },
                false,
                false,
                false));
            snapshot.StandardCalculator.DisplayCommands.Add(new ExpressionCommandWrapper(
                CommandType.BinaryCommand,
                (int)CalculatorCommand.CommandADD,
                System.Array.Empty<int>(),
                false,
                false,
                false));
            snapshot.StandardCalculator.DisplayCommands.Add(new ExpressionCommandWrapper(
                CommandType.OperandCommand,
                0,
                new[]
                {
                    (int)CalculatorCommand.Command1,
                    (int)CalculatorCommand.CommandPNT,
                    (int)CalculatorCommand.Command2
                },
                false,
                true,
                false));
            snapshot.StandardCalculator.DisplayCommands.Add(new ExpressionCommandWrapper(
                CommandType.Parentheses,
                (int)CalculatorCommand.CommandOPENP,
                System.Array.Empty<int>(),
                false,
                false,
                false));

            var result = ParseSnapshot(snapshot);

            Assert.IsFalse(result.HasError);
            Assert.IsNotNull(result.Snapshot);
        }
```

##### `UnaryCommandSurvivesTheRoundTrip` — `SnapshotJsonTests` — `SnapshotJsonTests.cs:162-170`

```csharp
        [TestMethod]
        public void UnaryCommandSurvivesTheRoundTrip()
        {
            var restored = RoundTrip(new ExpressionCommandWrapper(
                CommandType.UnaryCommand, 0, new[] { 91, 92 }, false, false, false));

            Assert.AreEqual(CommandType.UnaryCommand, restored.Type);
            CollectionAssert.AreEqual(new[] { 91, 92 }, restored.Commands);
        }
```

##### `BinaryCommandSurvivesTheRoundTrip` — `SnapshotJsonTests` — `SnapshotJsonTests.cs:172-180`

```csharp
        [TestMethod]
        public void BinaryCommandSurvivesTheRoundTrip()
        {
            var restored = RoundTrip(new ExpressionCommandWrapper(
                CommandType.BinaryCommand, 93, System.Array.Empty<int>(), false, false, false));

            Assert.AreEqual(CommandType.BinaryCommand, restored.Type);
            Assert.AreEqual(93, restored.Command);
        }
```

##### `ParenthesesCommandSurvivesTheRoundTrip` — `SnapshotJsonTests` — `SnapshotJsonTests.cs:182-190`

```csharp
        [TestMethod]
        public void ParenthesesCommandSurvivesTheRoundTrip()
        {
            var restored = RoundTrip(new ExpressionCommandWrapper(
                CommandType.Parentheses, 106, System.Array.Empty<int>(), false, false, false));

            Assert.AreEqual(CommandType.Parentheses, restored.Type);
            Assert.AreEqual(106, restored.Command);
        }
```

##### `OperandCommandCarriesItsFlagsThroughTheRoundTrip` — `SnapshotJsonTests` — `SnapshotJsonTests.cs:192-208`

```csharp
        [TestMethod]
        public void OperandCommandCarriesItsFlagsThroughTheRoundTrip()
        {
            var restored = RoundTrip(new ExpressionCommandWrapper(
                CommandType.OperandCommand,
                0,
                new[] { 131, 132 },
                isNegative: true,
                isDecimalPresent: true,
                isSciFmt: true));

            Assert.AreEqual(CommandType.OperandCommand, restored.Type);
            CollectionAssert.AreEqual(new[] { 131, 132 }, restored.Commands);
            Assert.IsTrue(restored.IsNegative, "IsNegative was lost in the round trip.");
            Assert.IsTrue(restored.IsDecimalPresent, "IsDecimalPresent was lost in the round trip.");
            Assert.IsTrue(restored.IsSciFmt, "IsSciFmt was lost in the round trip.");
        }
```

##### `MalformedUnaryCommandIsRejectedDuringDeserialization` — `SnapshotJsonTests` — `SnapshotJsonTests.cs:210-216`

```csharp
        [TestMethod]
        public void MalformedUnaryCommandIsRejectedDuringDeserialization()
        {
            Assert.ThrowsException<System.ArgumentOutOfRangeException>(
                () => Helpers.MapCommandAlias(new UnaryCommandAlias()),
                "A unary command with no sub-commands has to be rejected at the JSON boundary.");
        }
```

##### `TokenSurvivesTheRoundTrip` — `SnapshotJsonTests` — `SnapshotJsonTests.cs:218-228`

```csharp
        [TestMethod]
        public void TokenSurvivesTheRoundTrip()
        {
            var token = new CalculatorApp.ViewModel.Snapshot.CalcManagerToken("+", 3);

            var json = JsonSerializer.Serialize(new CalcManagerTokenAlias(token));
            var restored = Helpers.MapToken(JsonSerializer.Deserialize<CalcManagerTokenAlias>(json));

            Assert.AreEqual("+", restored.OpCodeName);
            Assert.AreEqual(3, restored.CommandIndex);
        }
```

##### `InvalidSnapshotModeDoesNotReplaceCurrentMode` — `SnapshotRoundTripTests` — `SnapshotRoundTripTests.cs:38-49`

```csharp
        [TestMethod]
        public void InvalidSnapshotModeDoesNotReplaceCurrentMode()
        {
            var viewModel = new ApplicationViewModel();
            viewModel.Initialize(ViewMode.Standard);
            var snapshot = new ApplicationSnapshot { Mode = int.MaxValue };

            Assert.ThrowsException<ArgumentOutOfRangeException>(
                () => viewModel.RestoreFromSnapshot(snapshot));

            Assert.AreEqual(ViewMode.Standard, viewModel.Mode);
        }
```

##### `NonCalculatorSnapshotIgnoresStaleCalculatorState` — `SnapshotRoundTripTests` — `SnapshotRoundTripTests.cs:51-66`

```csharp
        [TestMethod]
        public void NonCalculatorSnapshotIgnoresStaleCalculatorState()
        {
            var viewModel = new ApplicationViewModel();
            SetModeWithoutNavigation(viewModel, ViewMode.Date);
            var snapshot = new ApplicationSnapshot
            {
                Mode = (int)ViewMode.Date,
                StandardCalculator = CreateStandardViewModel().Snapshot
            };

            viewModel.RestoreFromSnapshot(snapshot);

            Assert.AreEqual(ViewMode.Date, viewModel.Mode);
            Assert.IsNull(viewModel.CalculatorViewModel);
        }
```

##### `NonCalculatorSnapshotDoesNotCaptureStaleCalculatorState` — `SnapshotRoundTripTests` — `SnapshotRoundTripTests.cs:68-81`

```csharp
        [TestMethod]
        public void NonCalculatorSnapshotDoesNotCaptureStaleCalculatorState()
        {
            var viewModel = new ApplicationViewModel
            {
                CalculatorViewModel = CreateStandardViewModel()
            };
            SetModeWithoutNavigation(viewModel, ViewMode.Date);

            var snapshot = viewModel.Snapshot;

            Assert.AreEqual((int)ViewMode.Date, snapshot.Mode);
            Assert.IsNull(snapshot.StandardCalculator);
        }
```

##### `SnapshotRestoresHistory` — `SnapshotRoundTripTests` — `SnapshotRoundTripTests.cs:83-118`

```csharp
        [TestMethod]
        public void SnapshotRestoresHistory()
        {
            StandardCalculatorViewModel source = CreateStandardViewModel();
            Evaluate(source, Command1, CommandADD, Command2);
            Evaluate(source, Command3, CommandMUL, Command3);

            var captured = source.Snapshot;

            Assert.IsNotNull(
                captured.CalcManager.HistoryItems,
                "No history was captured, so resuming from Recall would come back with an empty history list.");
            Assert.AreEqual(2, captured.CalcManager.HistoryItems.Count);

            StandardCalculatorViewModel restored = CreateStandardViewModel();
            restored.Snapshot = captured;

            var recaptured = restored.Snapshot;
            Assert.IsNotNull(recaptured.CalcManager.HistoryItems, "The restored calculator captured no history.");
            CollectionAssert.AreEqual(
                captured.CalcManager.HistoryItems.Select(item => item.Expression).ToArray(),
                recaptured.CalcManager.HistoryItems.Select(item => item.Expression).ToArray(),
                "Expressions did not survive the round trip.");
            CollectionAssert.AreEqual(
                captured.CalcManager.HistoryItems.Select(item => item.Result).ToArray(),
                recaptured.CalcManager.HistoryItems.Select(item => item.Result).ToArray(),
                "Results did not survive the round trip.");

            // Display history is newest-first while the snapshot is oldest-first.
            var items = restored.HistoryVM.Items;
            Assert.AreEqual(2, items.Count, "The restored calculator did not get its history back.");
            CollectionAssert.AreEqual(
                captured.CalcManager.HistoryItems.Select(item => item.Result).Reverse().ToArray(),
                items.Select(item => item.Result).ToArray(),
                "The restored history list does not show what was captured.");
        }
```

##### `CapturedHistoryCarriesItsTokensAndCommands` — `SnapshotRoundTripTests` — `SnapshotRoundTripTests.cs:120-130`

```csharp
        [TestMethod]
        public void CapturedHistoryCarriesItsTokensAndCommands()
        {
            StandardCalculatorViewModel source = CreateStandardViewModel();
            Evaluate(source, Command1, CommandADD, Command2);

            var item = source.Snapshot.CalcManager.HistoryItems.Single();

            Assert.IsTrue(item.Tokens.Count > 0, "The captured history item carried no tokens.");
            Assert.IsTrue(item.Commands.Count > 0, "The captured history item carried no commands.");
        }
```

##### `ExpressionCommandWrapperCarriesTheOperandFlags` — `SnapshotRoundTripTests` — `SnapshotRoundTripTests.cs:132-148`

```csharp
        [TestMethod]
        public void ExpressionCommandWrapperCarriesTheOperandFlags()
        {
            var command = new ExpressionCommandWrapper(
                CommandType.OperandCommand,
                0,
                new[] { Command1, Command2 },
                isNegative: true,
                isDecimalPresent: true,
                isSciFmt: true);

            Assert.AreEqual(CommandType.OperandCommand, command.Type);
            CollectionAssert.AreEqual(new[] { Command1, Command2 }, command.Commands);
            Assert.IsTrue(command.IsNegative, "IsNegative did not survive.");
            Assert.IsTrue(command.IsDecimalPresent, "IsDecimalPresent did not survive.");
            Assert.IsTrue(command.IsSciFmt, "IsSciFmt did not survive.");
        }
```

##### `HistoryItemWrapperIsConstructibleFromManagedData` — `SnapshotRoundTripTests` — `SnapshotRoundTripTests.cs:150-165`

```csharp
        [TestMethod]
        public void HistoryItemWrapperIsConstructibleFromManagedData()
        {
            var token = new HistoryToken { Value = "1", CommandIndex = 0 };
            var command = new ExpressionCommandWrapper(
                CommandType.BinaryCommand, CommandADD, System.Array.Empty<int>(), false, false, false);

            var item = new HistoryItemWrapper(new[] { token }, new[] { command }, "1 + 2 =", "3");

            Assert.AreEqual("1 + 2 =", item.Expression);
            Assert.AreEqual("3", item.Result);
            Assert.AreEqual(1, item.Tokens.Length);
            Assert.AreEqual("1", item.Tokens[0].Value);
            Assert.AreEqual(1, item.Commands.Length);
            Assert.AreEqual(CommandADD, item.Commands[0].Command);
        }
```

##### `RestoringASnapshotClearsMemory` — `SnapshotRoundTripTests` — `SnapshotRoundTripTests.cs:168-183`

```csharp
        [TestMethod]
        public void RestoringASnapshotClearsMemory()
        {
            StandardCalculatorViewModel source = CreateStandardViewModel();
            Evaluate(source, Command1, CommandADD, Command2);
            var captured = source.Snapshot;

            StandardCalculatorViewModel restored = CreateStandardViewModel();
            Evaluate(restored, Command3);
            restored.OnMemoryButtonPressed();
            Assert.IsTrue(restored.IsMemoryEmpty == false, "Memory was not set up, so the test proves nothing.");

            restored.Snapshot = captured;

            Assert.IsTrue(restored.IsMemoryEmpty, "Restoring a snapshot left the previous session's memory behind.");
        }
```

##### `RestoringAMalformedHistoryCommandThrowsInsteadOfCrashing` — `SnapshotRoundTripTests` — `SnapshotRoundTripTests.cs:185-205`

```csharp
        [TestMethod]
        public void RestoringAMalformedHistoryCommandThrowsInsteadOfCrashing()
        {
            var malformed = new ExpressionCommandWrapper(
                CommandType.UnaryCommand, 0, System.Array.Empty<int>(), false, false, false);
            var item = new CalculatorApp.ViewModel.Snapshot.CalcManagerHistoryItem
            {
                Expression = "1 + 2 =",
                Result = "3"
            };
            item.Commands.Add(malformed);

            var snapshot = CreateStandardViewModel().Snapshot;
            snapshot.CalcManager.HistoryItems = new List<CalculatorApp.ViewModel.Snapshot.CalcManagerHistoryItem> { item };

            StandardCalculatorViewModel target = CreateStandardViewModel();

            Assert.ThrowsException<ArgumentException>(
                () => target.Snapshot = snapshot,
                "A malformed unary command has to be rejected with a catchable exception.");
        }
```

##### `FailedRestoreResetsTheBoundCalculator` — `SnapshotRoundTripTests` — `SnapshotRoundTripTests.cs:207-240`

```csharp
        [TestMethod]
        public void FailedRestoreResetsTheBoundCalculator()
        {
            var viewModel = new ApplicationViewModel();
            viewModel.Initialize(ViewMode.Standard);
            var originalCalculator = CreateStandardViewModel();
            viewModel.CalculatorViewModel = originalCalculator;
            Evaluate(originalCalculator, Command3);
            originalCalculator.OnMemoryButtonPressed();

            StandardCalculatorViewModel source = CreateStandardViewModel();
            Evaluate(source, Command1, CommandADD, Command2);
            var standardSnapshot = source.Snapshot;
            var malformed = new CalculatorApp.ViewModel.Snapshot.CalcManagerHistoryItem();
            malformed.Commands.Add(new ExpressionCommandWrapper(
                CommandType.UnaryCommand, 0, System.Array.Empty<int>(), false, false, false));
            standardSnapshot.CalcManager.HistoryItems.Add(malformed);
            var snapshot = new ApplicationSnapshot
            {
                Mode = (int)ViewMode.Standard,
                StandardCalculator = standardSnapshot
            };

            Assert.ThrowsException<ArgumentException>(
                () => viewModel.RestoreFromSnapshot(snapshot));

            Assert.AreEqual(ViewMode.Standard, viewModel.Mode);
            Assert.AreSame(originalCalculator, viewModel.CalculatorViewModel);
            Assert.AreEqual("0", viewModel.CalculatorViewModel.DisplayValue);
            Assert.IsTrue(viewModel.CalculatorViewModel.IsMemoryEmpty);
            Assert.AreEqual(0, viewModel.CalculatorViewModel.HistoryVM.Items.Count);
            Assert.IsNull(viewModel.CalculatorViewModel.Snapshot.ExpressionDisplay);
            Assert.AreEqual(0, viewModel.CalculatorViewModel.Snapshot.DisplayCommands.Count);
        }
```

##### `FailedCrossModeRestoreClearsSnapshotModeHistory` — `SnapshotRoundTripTests` — `SnapshotRoundTripTests.cs:242-263`

```csharp
        [TestMethod]
        public void FailedCrossModeRestoreClearsSnapshotModeHistory()
        {
            StandardCalculatorViewModel calculator = CreateCalculatorViewModel(ViewMode.Standard);
            var viewModel = CreateApplicationViewModel(ViewMode.Standard, calculator);
            StandardCalculatorViewModel source = CreateCalculatorViewModel(ViewMode.Scientific);
            Evaluate(source, Command1, CommandADD, Command2);
            var standardSnapshot = source.Snapshot;
            standardSnapshot.DisplayCommands = new List<ExpressionCommandWrapper> { null };
            var snapshot = new ApplicationSnapshot
            {
                Mode = (int)ViewMode.Scientific,
                StandardCalculator = standardSnapshot
            };

            Assert.ThrowsException<NullReferenceException>(
                () => viewModel.RestoreFromSnapshot(snapshot));

            calculator.SendCommandToCalcManager(ModeScientific);
            calculator.HistoryVM.ReloadHistory(ViewMode.Scientific);
            Assert.AreEqual(0, calculator.HistoryVM.Items.Count);
        }
```

##### `FailedScientificRestoreKeepsTheScientificEngine` — `SnapshotRoundTripTests` — `SnapshotRoundTripTests.cs:265-279`

```csharp
        [TestMethod]
        public void FailedScientificRestoreKeepsTheScientificEngine()
        {
            StandardCalculatorViewModel calculator = CreateCalculatorViewModel(ViewMode.Scientific);
            AssertScientificOrderOfOperations(calculator);
            calculator.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Radians);
            Assert.AreEqual(NumbersAndOperatorsEnum.Radians, calculator.GetCurrentAngleType());
            var viewModel = CreateApplicationViewModel(ViewMode.Scientific, calculator);

            Assert.ThrowsException<ArgumentException>(
                () => viewModel.RestoreFromSnapshot(CreateMalformedSnapshot(ViewMode.Scientific)));

            Assert.AreEqual(NumbersAndOperatorsEnum.Degree, calculator.GetCurrentAngleType());
            AssertScientificOrderOfOperations(calculator);
        }
```

##### `FailedScientificRestoreResetsErroredEngineAngleMode` — `SnapshotRoundTripTests` — `SnapshotRoundTripTests.cs:281-295`

```csharp
        [TestMethod]
        public void FailedScientificRestoreResetsErroredEngineAngleMode()
        {
            StandardCalculatorViewModel calculator = CreateCalculatorViewModel(ViewMode.Scientific);
            calculator.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Radians);
            Evaluate(calculator, Command1, CommandDIV, Command0);
            Assert.IsTrue(calculator.IsInError);
            var viewModel = CreateApplicationViewModel(ViewMode.Scientific, calculator);

            Assert.ThrowsException<ArgumentException>(
                () => viewModel.RestoreFromSnapshot(CreateMalformedSnapshot(ViewMode.Scientific)));

            Evaluate(calculator, Command9, Command0, CommandSIN);
            Assert.AreEqual("1", calculator.DisplayValue);
        }
```

##### `FailedProgrammerRestoreKeepsTheProgrammerEngine` — `SnapshotRoundTripTests` — `SnapshotRoundTripTests.cs:297-311`

```csharp
        [TestMethod]
        public void FailedProgrammerRestoreKeepsTheProgrammerEngine()
        {
            StandardCalculatorViewModel calculator = CreateCalculatorViewModel(ViewMode.Programmer);
            AssertProgrammerIgnoresDecimalPoint(calculator);
            calculator.SwitchProgrammerModeBase(NumberBase.HexBase);
            Assert.AreEqual(NumberBase.HexBase, calculator.CurrentRadixType);
            var viewModel = CreateApplicationViewModel(ViewMode.Programmer, calculator);

            Assert.ThrowsException<ArgumentException>(
                () => viewModel.RestoreFromSnapshot(CreateMalformedSnapshot(ViewMode.Programmer)));

            Assert.AreEqual(NumberBase.DecBase, calculator.CurrentRadixType);
            AssertProgrammerIgnoresDecimalPoint(calculator);
        }
```

##### `FailedProgrammerRestoreResetsBitLength` — `SnapshotRoundTripTests` — `SnapshotRoundTripTests.cs:313-328`

```csharp
        [TestMethod]
        public void FailedProgrammerRestoreResetsBitLength()
        {
            StandardCalculatorViewModel calculator = CreateCalculatorViewModel(ViewMode.Programmer);
            calculator.ValueBitLength = BitLength.BitLengthByte;
            Evaluate(calculator, Command1, CommandDIV, Command0);
            Assert.IsTrue(calculator.IsInError);
            var viewModel = CreateApplicationViewModel(ViewMode.Programmer, calculator);

            Assert.ThrowsException<ArgumentException>(
                () => viewModel.RestoreFromSnapshot(CreateMalformedSnapshot(ViewMode.Programmer)));

            Assert.AreEqual(BitLength.BitLengthQWord, calculator.ValueBitLength);
            Evaluate(calculator, Command2, Command5, Command5, CommandADD, Command1);
            Assert.AreEqual("256", calculator.DisplayValue);
        }
```

##### `SuccessfulScientificRestoreKeepsTheScientificEngine` — `SnapshotRoundTripTests` — `SnapshotRoundTripTests.cs:330-346`

```csharp
        [TestMethod]
        public void SuccessfulScientificRestoreKeepsTheScientificEngine()
        {
            StandardCalculatorViewModel calculator = CreateCalculatorViewModel(ViewMode.Scientific);
            calculator.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Radians);
            Assert.AreEqual(NumbersAndOperatorsEnum.Radians, calculator.GetCurrentAngleType());
            var viewModel = CreateApplicationViewModel(ViewMode.Scientific, calculator);

            viewModel.RestoreFromSnapshot(new ApplicationSnapshot
            {
                Mode = (int)ViewMode.Scientific,
                StandardCalculator = CreateCalculatorViewModel(ViewMode.Scientific).Snapshot
            });

            Assert.AreEqual(NumbersAndOperatorsEnum.Degree, calculator.GetCurrentAngleType());
            AssertScientificOrderOfOperations(calculator);
        }
```

##### `SuccessfulProgrammerRestoreKeepsTheProgrammerEngine` — `SnapshotRoundTripTests` — `SnapshotRoundTripTests.cs:348-364`

```csharp
        [TestMethod]
        public void SuccessfulProgrammerRestoreKeepsTheProgrammerEngine()
        {
            StandardCalculatorViewModel calculator = CreateCalculatorViewModel(ViewMode.Programmer);
            calculator.SwitchProgrammerModeBase(NumberBase.HexBase);
            Assert.AreEqual(NumberBase.HexBase, calculator.CurrentRadixType);
            var viewModel = CreateApplicationViewModel(ViewMode.Programmer, calculator);

            viewModel.RestoreFromSnapshot(new ApplicationSnapshot
            {
                Mode = (int)ViewMode.Programmer,
                StandardCalculator = CreateCalculatorViewModel(ViewMode.Programmer).Snapshot
            });

            Assert.AreEqual(NumberBase.DecBase, calculator.CurrentRadixType);
            AssertProgrammerIgnoresDecimalPoint(calculator);
        }
```

##### `VerifyCorrectCharacterMapping` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:434-457`

```csharp
        [TestMethod]
        public void VerifyCorrectCharacterMapping()
        {
            // Valid numbers
            var n = _viewModel.MapCharacterToButtonId('0').ButtonId;
            Assert.AreEqual(NumbersAndOperatorsEnum.Zero, n);

            n = _viewModel.MapCharacterToButtonId('1').ButtonId;
            Assert.AreEqual(NumbersAndOperatorsEnum.One, n);

            // Valid operators
            n = _viewModel.MapCharacterToButtonId('+').ButtonId;
            Assert.AreEqual(NumbersAndOperatorsEnum.Add, n);

            n = _viewModel.MapCharacterToButtonId('=').ButtonId;
            Assert.AreEqual(NumbersAndOperatorsEnum.Equals, n);

            n = _viewModel.MapCharacterToButtonId('a').ButtonId;
            Assert.AreEqual(NumbersAndOperatorsEnum.A, n);

            // Invalid character
            n = _viewModel.MapCharacterToButtonId('$').ButtonId;
            Assert.AreEqual(NumbersAndOperatorsEnum.None, n);
        }
```

##### `PasteExpressions` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:460-512`

```csharp
        [TestMethod]
        public void PasteExpressions()
        {
            _viewModel.IsScientific = false;

            _viewModel.OnPaste("-0.99");
            ValidateViewModelValue("-0" + _decimalSeparator + "99");

            _viewModel.OnPaste("1+1=");
            ValidateViewModelValue("2");

            // This result is not obvious: it's the result of the previous operation
            _viewModel.OnPaste("0=");
            ValidateViewModelValue("1");

            // Negative value
            _viewModel.OnPaste("-1");
            ValidateViewModelValue("-1");

            // Negated expression
            _viewModel.OnPaste("-(1+1)");
            ValidateViewModelValueAndSecondaryExpression("-2", "negate(1 + 1)");

            // More complicated Negated expression
            _viewModel.OnPaste("-(-(-1))");
            ValidateViewModelValueAndSecondaryExpression("-1", "negate(0 - (0 - 1))");

            // Switch to scientific mode
            _viewModel.IsScientific = true;

            Assert.IsFalse(_viewModel.IsFToEChecked);

            // Positive exponent
            _viewModel.OnPaste("1.23e+10");
            ValidateViewModelValue("1" + _decimalSeparator + "23e+10");

            _viewModel.OnPaste("1.23e10");
            ValidateViewModelValue("1" + _decimalSeparator + "23e+10");

            _viewModel.OnPaste("135e10");
            ValidateViewModelValue("135" + _decimalSeparator + "e+10");

            // Negative exponent
            _viewModel.OnPaste("1.23e-10");
            ValidateViewModelValue("1" + _decimalSeparator + "23e-10");

            // Uppercase E (for exponent)
            _viewModel.OnPaste("1.23E-10");
            ValidateViewModelValue("1" + _decimalSeparator + "23e-10");

            _viewModel.OnPaste("135E10");
            ValidateViewModelValue("135" + _decimalSeparator + "e+10");
        }
```

##### `CalculationResultAutomationNameVerification` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:518-556`

```csharp
        [TestMethod]
        public void CalculationResultAutomationNameVerification()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsStandardMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Two, "12", ""),
                new(NumbersAndOperatorsEnum.Three, "123", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            Assert.AreEqual("Display is 123", _viewModel.CalculationResultAutomationName);

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Multiply, "2", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Equals, "7", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, true);
            Assert.AreEqual("Display is 7", _viewModel.CalculationResultAutomationName);

            var items3 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Clear, "0", ""),
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.Five, "5", ""),
                new(NumbersAndOperatorsEnum.InvSin, "Invalid input", "asind(5)"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items3, false);
            Assert.AreEqual("Display is Invalid input", _viewModel.CalculationResultAutomationName);
        }
```

##### `VerifyRawFormatting` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:1180-1198`

```csharp
        [TestMethod]
        public void VerifyRawFormatting()
        {
            // GetRawDisplayValue is private, so we test via the LocalizationSettings
            // approach used internally: remove group separators from display value.
            var settings = LocalizationSettings.GetInstance();

            _viewModel.DisplayValue = "1,001";
            Assert.AreEqual("1001", settings.RemoveGroupSeparators(_viewModel.DisplayValue));

            _viewModel.DisplayValue = "999";
            Assert.AreEqual("999", settings.RemoveGroupSeparators(_viewModel.DisplayValue));

            _viewModel.DisplayValue = "1,001,001";
            Assert.AreEqual("1001001", settings.RemoveGroupSeparators(_viewModel.DisplayValue));

            _viewModel.DisplayValue = "1,001, 001";
            Assert.AreEqual("1001001", settings.RemoveGroupSeparators(_viewModel.DisplayValue));
        }
```

##### `IsLastCharacterSuccess` — `UtilsTests` — `UtilsTests.cs:12-16`

```csharp
        [TestMethod]
        public void IsLastCharacterSuccess()
        {
            Assert.IsTrue(Utils.IsLastCharacterTarget("Test.", '.'));
        }
```

##### `IsLastCharacterSuccessMultipleSuffices` — `UtilsTests` — `UtilsTests.cs:18-22`

```csharp
        [TestMethod]
        public void IsLastCharacterSuccessMultipleSuffices()
        {
            Assert.IsTrue(Utils.IsLastCharacterTarget("Test..", '.'));
        }
```

##### `IsLastCharacterFailure` — `UtilsTests` — `UtilsTests.cs:24-28`

```csharp
        [TestMethod]
        public void IsLastCharacterFailure()
        {
            Assert.IsFalse(Utils.IsLastCharacterTarget("Test", '.'));
        }
```

##### `IsLastCharacterFailureAllButLastMatch` — `UtilsTests` — `UtilsTests.cs:30-34`

```csharp
        [TestMethod]
        public void IsLastCharacterFailureAllButLastMatch()
        {
            Assert.IsFalse(Utils.IsLastCharacterTarget(".....T", '.'));
        }
```

##### `IsLastCharacterFailureEmptyInput` — `UtilsTests` — `UtilsTests.cs:36-40`

```csharp
        [TestMethod]
        public void IsLastCharacterFailureEmptyInput()
        {
            Assert.IsFalse(Utils.IsLastCharacterTarget(string.Empty, '.'));
        }
```

##### `IsLastCharacterFailureNullTarget` — `UtilsTests` — `UtilsTests.cs:42-46`

```csharp
        [TestMethod]
        public void IsLastCharacterFailureNullTarget()
        {
            Assert.IsFalse(Utils.IsLastCharacterTarget(string.Empty, '\0'));
        }
```

### 5.4 Unit converter vectors (incl. currency)

`UnitConverter` (C++) plus `CategoryViewModel`, `UnitViewModel`, `SupplementaryResultsViewModel`,
`UnitConverterDataLoader`, `UnitConverterViewModel` and `CurrencyConverterLoadTests` (C#).

#### 5.4.1 `TEST_CLASS(UnitConverterTest)` (C++ engine) — synthetic units with explicit ratios/offsets

The file-local `TestUnitConverterConfigLoader`, `SetUnitParams`, `SetCategoryParams`, `SetConversionDataParams`
and the static `s_testFeet`/`s_testMeters`-style unit fixtures are in §3.5.0 and §3.5.1.

##### `LoadFromCache_Fail_NoCacheKey` — `CurrencyConverterLoadTests` — `CurrencyConverterTests.cs:19-31`

```csharp
        [TestMethod]
        public async Task LoadFromCache_Fail_NoCacheKey()
        {
            var localSettings = ApplicationData.Current.LocalSettings;
            localSettings.Values.Remove(CurrencyDataLoaderConstants.CacheTimestampKey);

            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();

            Assert.IsFalse(didLoad, "Loading from cache must fail when the cache timestamp key is absent");
            Assert.IsFalse(loader.LoadFinished());
            Assert.IsFalse(loader.LoadedFromCache());
        }
```

##### `InitialLoadAndRefreshSerializeCurrencyMutation` — `CurrencyConverterLoadTests` — `CurrencyConverterTests.cs:33-71`

```csharp
        [TestMethod]
        public async Task InitialLoadAndRefreshSerializeCurrencyMutation()
        {
            var firstEntry = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);
            var releaseFirstEntry = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);
            var initialLoadFinished = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);
            int entryCount = 0;
            int activeEntries = 0;
            int overlapDetected = 0;

            var loader = new CurrencyDataLoader("en-US");
            loader.LoadGateEnteredForTest = async () =>
            {
                if (Interlocked.Increment(ref activeEntries) > 1)
                {
                    Interlocked.Exchange(ref overlapDetected, 1);
                }

                int entry = Interlocked.Increment(ref entryCount);
                if (entry == 1)
                {
                    firstEntry.SetResult(true);
                    await releaseFirstEntry.Task;
                }
            };
            loader.LoadGateExitedForTest = () => Interlocked.Decrement(ref activeEntries);
            loader.SetViewModelCallback(new LoadCompletionCallback(initialLoadFinished));

            loader.LoadData();
            await firstEntry.Task;
            Task<bool> refresh = loader.TryLoadDataFromWebOverrideAsync();
            releaseFirstEntry.SetResult(true);

            await initialLoadFinished.Task;
            await refresh;

            Assert.AreEqual(0, overlapDetected);
            Assert.AreEqual(2, entryCount);
        }
```

##### `LoadFromCache_Success` — `CurrencyConverterLoadTests` — `CurrencyConverterTests.cs:121-132`

```csharp
        [TestMethod]
        public async Task LoadFromCache_Success()
        {
            await PrimeCacheAsync();

            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();

            Assert.IsTrue(didLoad);
            Assert.IsTrue(loader.LoadFinished());
            Assert.IsTrue(loader.LoadedFromCache());
        }
```

##### `LoadFromCache_Fail_StaticDataFileDoesNotExist` — `CurrencyConverterLoadTests` — `CurrencyConverterTests.cs:134-144`

```csharp
        [TestMethod]
        public async Task LoadFromCache_Fail_StaticDataFileDoesNotExist()
        {
            await PrimeCacheAsync(writeStaticData: false);

            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();

            Assert.IsFalse(didLoad, "A cache load with no static data file must fail.");
            Assert.IsFalse(loader.LoadedFromCache());
        }
```

##### `LoadFromCache_Fail_AllRatiosDataFileDoesNotExist` — `CurrencyConverterLoadTests` — `CurrencyConverterTests.cs:146-156`

```csharp
        [TestMethod]
        public async Task LoadFromCache_Fail_AllRatiosDataFileDoesNotExist()
        {
            await PrimeCacheAsync(writeRatios: false);

            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();

            Assert.IsFalse(didLoad, "A cache load with no ratios file must fail.");
            Assert.IsFalse(loader.LoadedFromCache());
        }
```

##### `LoadFromCache_Fail_ResponseLanguageChanged` — `CurrencyConverterLoadTests` — `CurrencyConverterTests.cs:158-169`

```csharp
        [TestMethod]
        public async Task LoadFromCache_Fail_ResponseLanguageChanged()
        {
            // The cached text is localized, so a cache written for another language cannot be used.
            await PrimeCacheAsync(languageCode: "ar-SA");

            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();

            Assert.IsFalse(didLoad, "A cache written for a different language must not be reused.");
            Assert.IsFalse(loader.LoadedFromCache());
        }
```

##### `LoadFromCache_SortsCurrenciesByCountryName` — `CurrencyConverterLoadTests` — `CurrencyConverterTests.cs:171-203`

```csharp
        [TestMethod]
        public async Task LoadFromCache_SortsCurrenciesByCountryName()
        {
            const string staticData = @"[
                {""CountryCode"":""AA"",""CountryName"":""Zebra"",""CurrencyCode"":""AAA"",""CurrencyName"":""Alpha Coin"",""CurrencySymbol"":""A""},
                {""CountryCode"":""ZZ"",""CountryName"":""Alpha"",""CurrencyCode"":""ZZZ"",""CurrencyName"":""Zulu Coin"",""CurrencySymbol"":""Z""},
                {""CountryCode"":""MM"",""CountryName"":""Éclair"",""CurrencyCode"":""MMM"",""CurrencyName"":""Mike Coin"",""CurrencySymbol"":""M""}
            ]";
            const string ratios = @"[
                {""Rt"":1.0,""An"":""AAA""},
                {""Rt"":2.0,""An"":""ZZZ""},
                {""Rt"":3.0,""An"":""MMM""}
            ]";

            var originalCulture = CultureInfo.CurrentCulture;
            try
            {
                CultureInfo.CurrentCulture = new CultureInfo("en-US");
                await PrimeCacheAsync(staticDataOverride: staticData, ratiosOverride: ratios);
                var loader = new CurrencyDataLoader("en-US");

                Assert.IsTrue(await loader.TryLoadDataFromCacheAsync());

                CollectionAssert.AreEqual(
                    new[] { "Alpha", "Éclair", "Zebra" },
                    loader.GetOrderedUnits(0).ConvertAll(unit => unit.CountryName));
            }
            finally
            {
                CultureInfo.CurrentCulture = originalCulture;
                await PrimeCacheAsync();
            }
        }
```

##### `Loaded_LoadOrderedUnits` — `CurrencyConverterLoadTests` — `CurrencyConverterTests.cs:205-218`

```csharp
        [TestMethod]
        public async Task Loaded_LoadOrderedUnits()
        {
            var loader = await LoadedLoaderAsync();

            var units = loader.GetOrderedUnits(0);

            Assert.IsTrue(units.Count > 0, "No currency units were loaded.");
            foreach (var unit in units)
            {
                Assert.IsFalse(string.IsNullOrEmpty(unit.Abbreviation), "A currency unit had no abbreviation.");
                Assert.IsFalse(string.IsNullOrEmpty(unit.CountryName), "A currency unit had no country name.");
            }
        }
```

##### `Loaded_LoadOrderedRatios` — `CurrencyConverterLoadTests` — `CurrencyConverterTests.cs:220-232`

```csharp
        [TestMethod]
        public async Task Loaded_LoadOrderedRatios()
        {
            var loader = await LoadedLoaderAsync();
            var units = loader.GetOrderedUnits(0);

            var ratios = loader.LoadOrderedRatios(units[0].Id);

            Assert.IsTrue(ratios.Count > 0, "The first currency unit had no ratios.");
            Assert.IsTrue(
                ratios.ContainsKey(units[0].Id),
                "A currency should always convert to itself.");
        }
```

##### `Loaded_GetCurrencySymbols_Valid` — `CurrencyConverterLoadTests` — `CurrencyConverterTests.cs:234-244`

```csharp
        [TestMethod]
        public async Task Loaded_GetCurrencySymbols_Valid()
        {
            var loader = await LoadedLoaderAsync();
            var units = loader.GetOrderedUnits(0);

            var symbols = loader.GetCurrencySymbols(units[0].Id, units[1].Id);

            Assert.IsNotNull(symbols.Symbol1);
            Assert.IsNotNull(symbols.Symbol2);
        }
```

##### `Loaded_GetCurrencySymbols_Invalid` — `CurrencyConverterLoadTests` — `CurrencyConverterTests.cs:246-255`

```csharp
        [TestMethod]
        public async Task Loaded_GetCurrencySymbols_Invalid()
        {
            var loader = await LoadedLoaderAsync();

            var symbols = loader.GetCurrencySymbols(-1, -2);

            Assert.AreEqual(string.Empty, symbols.Symbol1, "An unknown unit must not yield a symbol.");
            Assert.AreEqual(string.Empty, symbols.Symbol2);
        }
```

##### `Loaded_GetCurrencyRatioEquality_Valid` — `CurrencyConverterLoadTests` — `CurrencyConverterTests.cs:257-271`

```csharp
        [TestMethod]
        public async Task Loaded_GetCurrencyRatioEquality_Valid()
        {
            var loader = await LoadedLoaderAsync();
            var units = loader.GetOrderedUnits(0);

            var equality = loader.GetCurrencyRatioEquality(units[0].Id, units[1].Id);

            Assert.IsFalse(string.IsNullOrEmpty(equality.Ratio1), "The ratio line was empty.");
            Assert.IsFalse(string.IsNullOrEmpty(equality.Ratio2), "The accessible ratio line was empty.");

            // The accessible form spells the currencies out rather than abbreviating them.
            StringAssert.Contains(equality.Ratio2, units[0].CountryName);
            StringAssert.Contains(equality.Ratio2, units[0].Name);
        }
```

##### `Loaded_GetCurrencyRatioEquality_Invalid` — `CurrencyConverterLoadTests` — `CurrencyConverterTests.cs:273-282`

```csharp
        [TestMethod]
        public async Task Loaded_GetCurrencyRatioEquality_Invalid()
        {
            var loader = await LoadedLoaderAsync();

            var equality = loader.GetCurrencyRatioEquality(-1, -2);

            Assert.AreEqual(string.Empty, equality.Ratio1);
            Assert.AreEqual(string.Empty, equality.Ratio2);
        }
```

##### `LoadFromWeb_Success` — `CurrencyConverterLoadTests` — `CurrencyConverterTests.cs:284-294`

```csharp
        [TestMethod]
        public async Task LoadFromWeb_Success()
        {
            var loader = new CurrencyDataLoader("en-US");

            bool didLoad = await loader.TryLoadDataFromWebAsync();

            Assert.IsTrue(didLoad);
            Assert.IsTrue(loader.LoadFinished());
            Assert.IsTrue(loader.LoadedFromWeb());
        }
```

##### `Load_Success_LoadedFromCache` — `CurrencyConverterLoadTests` — `CurrencyConverterTests.cs:296-308`

```csharp
        [TestMethod]
        public async Task Load_Success_LoadedFromCache()
        {
            // A cache written moments ago is still fresh, so the loader uses it and never goes out.
            await PrimeCacheAsync();

            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();

            Assert.IsTrue(didLoad);
            Assert.IsTrue(loader.LoadedFromCache());
            Assert.IsFalse(loader.LoadedFromWeb());
        }
```

##### `Load_Success_LoadedFromWeb` — `CurrencyConverterLoadTests` — `CurrencyConverterTests.cs:310-321`

```csharp
        [TestMethod]
        public async Task Load_Success_LoadedFromWeb()
        {
            // A cache older than a day is refreshed from the web rather than used as-is.
            await PrimeCacheAsync(timestamp: DateTimeOffset.UtcNow.AddDays(-2));

            var loader = new CurrencyDataLoader("en-US");
            bool didLoad = await loader.TryLoadDataFromCacheAsync();

            Assert.IsTrue(didLoad);
            Assert.IsTrue(loader.LoadedFromWeb(), "A stale cache should have been refreshed from the web.");
        }
```

#### 5.4.2 Managed unit-converter view-model and data-loader tests

##### `Test_RoundCurrencyRatio` — `CurrencyConverterLoadTests` — `CurrencyConverterTests.cs:324-363`

```csharp
        [TestMethod]
        public void Test_RoundCurrencyRatio()
        {
            (double Ratio, double Expected)[] cases =
            {
                (1234567, 1234567),
                (0, 0),
                (9999.999, 9999.999),
                (8765.4321, 8765.4321),
                (4815.162342, 4815.1623),
                (4815.162358, 4815.1624),
                (4815.162388934723, 4815.1624),
                (0.12, 0.12),
                (0.123, 0.123),
                (0.1234, 0.1234),
                (0.12343, 0.1234),
                (0.0321, 0.0321),
                (0.03211, 0.03211),
                (0.032119, 0.03212),
                (0.00322119, 0.003221),
                (0.00123269, 0.001233),
                (0.00076269, 0.0007627),
                (0.000069, 0.000069),
                (0.000061, 0.000061),
                (0.000054612, 0.00005461),
                (0.000054616, 0.00005462),
                (0.000005416, 0.000005416),
                (0.0000016134324, 0.000001613),
                (0.0000096134324, 0.000009613),
                (0.0000032169348392, 0.000003217),
                (0.000000002134987218, 0.000000002135),
                (0.000000000000087231445, 0.00000000000008723),
            };

            foreach (var (ratio, expected) in cases)
            {
                Assert.AreEqual(expected, CurrencyDataLoader.RoundCurrencyRatio(ratio), 0d,
                    $"RoundCurrencyRatio({ratio})");
            }
        }
```

##### `UnitConverterTestInit` — `UnitConverterTest` — `UnitConverterTest.cpp:269-274`

```cpp
    void UnitConverterTest::UnitConverterTestInit()
    {
        VERIFY_ARE_EQUAL((UINT)0, s_xmlLoader->m_loadDataCallCount); // shouldn't have initialized the loader yet
        s_unitConverter->Initialize();
        VERIFY_ARE_EQUAL((UINT)1, s_xmlLoader->m_loadDataCallCount); // now we should have loaded
    }
```

##### `UnitConverterTestBasic` — `UnitConverterTest` — `UnitConverterTest.cpp:277-294`

```cpp
    void UnitConverterTest::UnitConverterTestBasic()
    {
        tuple<wstring, Unit> test1[] = { tuple<wstring, Unit>(wstring(L"0.25"), s_testFeet) };
        tuple<wstring, Unit> test2[] = { tuple<wstring, Unit>(wstring(L"2.5"), s_testFeet) };

        s_unitConverter->SendCommand(Command::Three);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"3"), wstring(L"3")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test1), end(test1))));
        s_unitConverter->SendCommand(Command::Zero);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"30"), wstring(L"30")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test2), end(test2))));
        s_unitConverter->SendCommand(Command::Decimal);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"30."), wstring(L"30")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test2), end(test2))));
        s_unitConverter->SendCommand(Command::Zero);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"30.0"), wstring(L"30")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test2), end(test2))));
    }
```

##### `UnitConverterTestBackspaceBasic` — `UnitConverterTest` — `UnitConverterTest.cpp:297-315`

```cpp
    void UnitConverterTest::UnitConverterTestBackspaceBasic()
    {
        s_unitConverter->SendCommand(Command::Two);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Decimal);
        s_unitConverter->SendCommand(Command::Four);
        s_unitConverter->SendCommand(Command::Three);
        s_unitConverter->SendCommand(Command::Backspace);

        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"20.4"), wstring(L"20.4")));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"20."), wstring(L"20")));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"20"), wstring(L"20")));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"2"), wstring(L"2")));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"0"), wstring(L"0")));
    }
```

##### `UnitConverterTestClear` — `UnitConverterTest` — `UnitConverterTest.cpp:318-328`

```cpp
    void UnitConverterTest::UnitConverterTestClear()
    {
        s_unitConverter->SendCommand(Command::Two);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Decimal);
        s_unitConverter->SendCommand(Command::Four);
        s_unitConverter->SendCommand(Command::Three);
        s_unitConverter->SendCommand(Command::Clear);

        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"0"), wstring(L"0")));
    }
```

##### `UnitConverterTestGetters` — `UnitConverterTest` — `UnitConverterTest.cpp:331-338`

```cpp
    void UnitConverterTest::UnitConverterTestGetters()
    {
        Category test1[] = { s_testLength, s_testWeight };
        Unit test2[] = { s_testInches, s_testFeet };

        VERIFY_IS_TRUE(s_unitConverter->GetCategories() == vector<Category>(begin(test1), end(test1)));
        VERIFY_IS_TRUE(get<0>(s_unitConverter->SetCurrentCategory(test1[0])) == vector<Unit>(begin(test2), end(test2)));
    }
```

##### `UnitConverterTestGetCategory` — `UnitConverterTest` — `UnitConverterTest.cpp:341-345`

```cpp
    void UnitConverterTest::UnitConverterTestGetCategory()
    {
        s_unitConverter->SetCurrentCategory(s_testWeight);
        VERIFY_IS_TRUE(s_unitConverter->GetCurrentCategory() == s_testWeight);
    }
```

##### `UnitConverterTestUnitTypeSwitching` — `UnitConverterTest` — `UnitConverterTest.cpp:348-360`

```cpp
    void UnitConverterTest::UnitConverterTestUnitTypeSwitching()
    {
        // Enter 57 into the from field, then switch focus to the to field (making it the new from field)
        s_unitConverter->SendCommand(Command::Five);
        s_unitConverter->SendCommand(Command::Seven);
        s_unitConverter->SwitchActive(wstring(L"57"));
        // Now set unit conversion to go from kilograms to pounds
        s_unitConverter->SetCurrentCategory(s_testWeight);
        s_unitConverter->SetCurrentUnitTypes(s_testKilograms, s_testPounds);
        s_unitConverter->SendCommand(Command::Five);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"5"), wstring(L"11.0231")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>()));
    }
```

##### `UnitConverterTestQuote` — `UnitConverterTest` — `UnitConverterTest.cpp:363-374`

```cpp
    void UnitConverterTest::UnitConverterTestQuote()
    {
        constexpr wstring_view input1 = L"Weight";
        constexpr wstring_view output1 = L"Weight";
        constexpr wstring_view input2 = L"{p}Weig;[ht|";
        constexpr wstring_view output2 = L"{lb}p{rb}Weig{sc}{lc}ht{p}";
        constexpr wstring_view input3 = L"{{{t;s}}},:]";
        constexpr wstring_view output3 = L"{lb}{lb}{lb}t{sc}s{rb}{rb}{rb}{cm}{co}{rc}";
        VERIFY_IS_TRUE(UnitConverter::Quote(input1) == output1);
        VERIFY_IS_TRUE(UnitConverter::Quote(input2) == output2);
        VERIFY_IS_TRUE(UnitConverter::Quote(input3) == output3);
    }
```

##### `UnitConverterTestUnquote` — `UnitConverterTest` — `UnitConverterTest.cpp:377-386`

```cpp
    void UnitConverterTest::UnitConverterTestUnquote()
    {
        constexpr wstring_view input1 = L"Weight";
        constexpr wstring_view input2 = L"{p}Weig;[ht|";
        constexpr wstring_view input3 = L"{{{t;s}}},:]";
        VERIFY_IS_TRUE(UnitConverter::Unquote(input1) == input1);
        VERIFY_IS_TRUE(UnitConverter::Unquote(UnitConverter::Quote(input1)) == input1);
        VERIFY_IS_TRUE(UnitConverter::Unquote(UnitConverter::Quote(input2)) == input2);
        VERIFY_IS_TRUE(UnitConverter::Unquote(UnitConverter::Quote(input3)) == input3);
    }
```

##### `UnitConverterTestBackspace` — `UnitConverterTest` — `UnitConverterTest.cpp:389-420`

```cpp
    void UnitConverterTest::UnitConverterTestBackspace()
    {
        tuple<wstring, Unit> test1[] = { tuple<wstring, Unit>(wstring(L"13.66"), s_testKilograms) };
        tuple<wstring, Unit> test2[] = { tuple<wstring, Unit>(wstring(L"13.65"), s_testKilograms) };
        tuple<wstring, Unit> test3[] = { tuple<wstring, Unit>(wstring(L"13.61"), s_testKilograms) };
        tuple<wstring, Unit> test4[] = { tuple<wstring, Unit>(wstring(L"1.36"), s_testKilograms) };

        s_unitConverter->SetCurrentCategory(s_testWeight);
        s_unitConverter->SetCurrentUnitTypes(s_testPounds, s_testPounds);
        s_unitConverter->SendCommand(Command::Three);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Decimal);
        s_unitConverter->SendCommand(Command::One);
        s_unitConverter->SendCommand(Command::Two);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"30.12"), wstring(L"30.12")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test1), end(test1))));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"30.1"), wstring(L"30.1")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test2), end(test2))));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"30."), wstring(L"30")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test3), end(test3))));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"30"), wstring(L"30")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test3), end(test3))));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"3"), wstring(L"3")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test4), end(test4))));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"0"), wstring(L"0")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>()));
    }
```

##### `UnitConverterTestScientificInputs` — `UnitConverterTest` — `UnitConverterTest.cpp:423-480`

```cpp
    void UnitConverterTest::UnitConverterTestScientificInputs()
    {
        s_unitConverter->SetCurrentCategory(s_testWeight);
        s_unitConverter->SetCurrentUnitTypes(s_testPounds, s_testKilograms);
        s_unitConverter->SendCommand(Command::Decimal);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::One);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"0.00000000000001"), wstring(L"4.535920e-15")));
        s_unitConverter->SwitchActive(wstring(L"4.535920e-15"));
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"999999999999999"), wstring(L"2.204620e+15")));
        s_unitConverter->SwitchActive(wstring(L"2.20463e+15"));
        s_unitConverter->SendCommand(Command::One);
        s_unitConverter->SendCommand(Command::Two);
        s_unitConverter->SendCommand(Command::Three);
        s_unitConverter->SendCommand(Command::Four);
        s_unitConverter->SendCommand(Command::Five);
        s_unitConverter->SendCommand(Command::Six);
        s_unitConverter->SendCommand(Command::Seven);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"1234567"), wstring(L"559989.7")));
        s_unitConverter->SwitchActive(wstring(L"559989.7"));
        s_unitConverter->SendCommand(Command::One);
        s_unitConverter->SendCommand(Command::Two);
        s_unitConverter->SendCommand(Command::Three);
        s_unitConverter->SendCommand(Command::Four);
        s_unitConverter->SendCommand(Command::Five);
        s_unitConverter->SendCommand(Command::Six);
        s_unitConverter->SendCommand(Command::Seven);
        s_unitConverter->SendCommand(Command::Eight);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"12345678"), wstring(L"27217529")));
    }
```

##### `UnitConverterTestSupplementaryResultRounding` — `UnitConverterTest` — `UnitConverterTest.cpp:483-496`

```cpp
    void UnitConverterTest::UnitConverterTestSupplementaryResultRounding()
    {
        tuple<wstring, Unit> test1[] = { tuple<wstring, Unit>(wstring(L"27.75"), s_testFeet) };
        tuple<wstring, Unit> test2[] = { tuple<wstring, Unit>(wstring(L"277.8"), s_testFeet) };
        tuple<wstring, Unit> test3[] = { tuple<wstring, Unit>(wstring(L"2778"), s_testFeet) };
        s_unitConverter->SendCommand(Command::Three);
        s_unitConverter->SendCommand(Command::Three);
        s_unitConverter->SendCommand(Command::Three);
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test1), end(test1))));
        s_unitConverter->SendCommand(Command::Three);
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test2), end(test2))));
        s_unitConverter->SendCommand(Command::Three);
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test3), end(test3))));
    }
```

##### `UnitConverterTestMaxDigitsReached` — `UnitConverterTest` — `UnitConverterTest.cpp:498-521`

```cpp
    void UnitConverterTest::UnitConverterTestMaxDigitsReached()
    {
        ExecuteCommands({ Command::One,
                          Command::Two,
                          Command::Three,
                          Command::Four,
                          Command::Five,
                          Command::Six,
                          Command::Seven,
                          Command::Eight,
                          Command::Nine,
                          Command::One,
                          Command::Zero,
                          Command::One,
                          Command::One,
                          Command::One,
                          Command::Two });

        VERIFY_ARE_EQUAL(0, s_testVMCallback->GetMaxDigitsReachedCallCount());

        ExecuteCommands({ Command::One });

        VERIFY_ARE_EQUAL(1, s_testVMCallback->GetMaxDigitsReachedCallCount());
    }
```

##### `UnitConverterTestMaxDigitsReached_LeadingDecimal` — `UnitConverterTest` — `UnitConverterTest.cpp:523-547`

```cpp
    void UnitConverterTest::UnitConverterTestMaxDigitsReached_LeadingDecimal()
    {
        ExecuteCommands({ Command::Zero,
                          Command::Decimal,
                          Command::One,
                          Command::Two,
                          Command::Three,
                          Command::Four,
                          Command::Five,
                          Command::Six,
                          Command::Seven,
                          Command::Eight,
                          Command::Nine,
                          Command::One,
                          Command::Zero,
                          Command::One,
                          Command::One,
                          Command::One });

        VERIFY_ARE_EQUAL(0, s_testVMCallback->GetMaxDigitsReachedCallCount());

        ExecuteCommands({ Command::Two });

        VERIFY_ARE_EQUAL(1, s_testVMCallback->GetMaxDigitsReachedCallCount());
    }
```

##### `UnitConverterTestMaxDigitsReached_TrailingDecimal` — `UnitConverterTest` — `UnitConverterTest.cpp:549-573`

```cpp
    void UnitConverterTest::UnitConverterTestMaxDigitsReached_TrailingDecimal()
    {
        ExecuteCommands({ Command::One,
                          Command::Two,
                          Command::Three,
                          Command::Four,
                          Command::Five,
                          Command::Six,
                          Command::Seven,
                          Command::Eight,
                          Command::Nine,
                          Command::One,
                          Command::Zero,
                          Command::One,
                          Command::One,
                          Command::One,
                          Command::Two,
                          Command::Decimal });

        VERIFY_ARE_EQUAL(0, s_testVMCallback->GetMaxDigitsReachedCallCount());

        ExecuteCommands({ Command::One });

        VERIFY_ARE_EQUAL(1, s_testVMCallback->GetMaxDigitsReachedCallCount());
    }
```

##### `UnitConverterTestMaxDigitsReached_MultipleTimes` — `UnitConverterTest` — `UnitConverterTest.cpp:575-601`

```cpp
    void UnitConverterTest::UnitConverterTestMaxDigitsReached_MultipleTimes()
    {
        ExecuteCommands({ Command::One,
                          Command::Two,
                          Command::Three,
                          Command::Four,
                          Command::Five,
                          Command::Six,
                          Command::Seven,
                          Command::Eight,
                          Command::Nine,
                          Command::One,
                          Command::Zero,
                          Command::One,
                          Command::One,
                          Command::One,
                          Command::Two });

        VERIFY_ARE_EQUAL(0, s_testVMCallback->GetMaxDigitsReachedCallCount());

        for (auto count = 1; count <= 10; count++)
        {
            ExecuteCommands({ Command::Three });

            VERIFY_ARE_EQUAL(count, s_testVMCallback->GetMaxDigitsReachedCallCount(), to_wstring(count).c_str());
        }
    }
```

##### `TestGetNameReturnsCorrectName` — `CategoryViewModelTests` — `UnitConverterViewModelTests.cs:19-26`

```csharp
        [TestMethod]
        public void TestGetNameReturnsCorrectName()
        {
            var category = new Category(3, "Length", supportsNegative: false);

            Assert.AreEqual("Length", category.Name);
            Assert.AreEqual(3, category.GetModelCategoryId());
        }
```

##### `TestGetVisibilityReturnsVisible` — `CategoryViewModelTests` — `UnitConverterViewModelTests.cs:28-34`

```csharp
        [TestMethod]
        public void TestGetVisibilityReturnsVisible()
        {
            var category = new Category(7, "Temperature", supportsNegative: true);

            Assert.AreEqual(Visibility.Visible, category.NegateVisibility);
        }
```

##### `TestGetVisibilityReturnsCollapsed` — `CategoryViewModelTests` — `UnitConverterViewModelTests.cs:36-42`

```csharp
        [TestMethod]
        public void TestGetVisibilityReturnsCollapsed()
        {
            var category = new Category(3, "Length", supportsNegative: false);

            Assert.AreEqual(Visibility.Collapsed, category.NegateVisibility);
        }
```

##### `TestGetNameReturnsCorrectName` — `UnitViewModelTests` — `UnitConverterViewModelTests.cs:48-55`

```csharp
        [TestMethod]
        public void TestGetNameReturnsCorrectName()
        {
            var unit = new Unit(11, "Centimeters", "cm", "Centimeters");

            Assert.AreEqual("Centimeters", unit.Name);
            Assert.AreEqual(11, unit.ModelUnitID());
        }
```

##### `TestGetAbbreviationReturnsCorrectAbbreviation` — `UnitViewModelTests` — `UnitConverterViewModelTests.cs:57-65`

```csharp
        [TestMethod]
        public void TestGetAbbreviationReturnsCorrectAbbreviation()
        {
            var unit = new Unit(11, "Centimeters", "cm", "centimeters");

            Assert.AreEqual("cm", unit.Abbreviation);
            Assert.AreEqual("centimeters", unit.AccessibleName);
            Assert.AreEqual("centimeters", unit.ToString());
        }
```

##### `TestGetValueReturnsCorrectValue` — `SupplementaryResultsViewModelTests` — `UnitConverterViewModelTests.cs:71-78`

```csharp
        [TestMethod]
        public void TestGetValueReturnsCorrectValue()
        {
            var result = new SupplementaryResult(
                "3.5", new Unit(11, "Centimeters", "cm", "centimeters"));

            Assert.AreEqual("3.5", result.Value);
        }
```

##### `TestGetUnitNameReturnsCorrectValue` — `SupplementaryResultsViewModelTests` — `UnitConverterViewModelTests.cs:80-88`

```csharp
        [TestMethod]
        public void TestGetUnitNameReturnsCorrectValue()
        {
            var unit = new Unit(11, "Centimeters", "cm", "centimeters");
            var result = new SupplementaryResult("3.5", unit);

            Assert.AreSame(unit, result.Unit);
            Assert.AreEqual("3.5 Centimeters", result.GetLocalizedAutomationName());
        }
```

##### `TestGetIsWhimsicalReturnsCorrectValue` — `SupplementaryResultsViewModelTests` — `UnitConverterViewModelTests.cs:90-100`

```csharp
        [TestMethod]
        public void TestGetIsWhimsicalReturnsCorrectValue()
        {
            var plain = new SupplementaryResult(
                "3.5", new Unit(11, "Centimeters", "cm", "centimeters"));
            var whimsical = new SupplementaryResult(
                "2", new Unit(90, "Jumbo Jets", "jj", "jumbo jets", isWhimsical: true));

            Assert.IsFalse(plain.IsWhimsical());
            Assert.IsTrue(whimsical.IsWhimsical());
        }
```

##### `AllStaticUnitsProduceFiniteConversionsInBothDirections` — `UnitConverterDataLoaderTests` — `UnitConverterViewModelTests.cs:106-138`

```csharp
        [TestMethod]
        public void AllStaticUnitsProduceFiniteConversionsInBothDirections()
        {
            var viewModel = new UnitConverterViewModel();
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            int unitsChecked = 0;

            foreach (var category in viewModel.Categories.Where(
                category => category.GetModelCategoryId() != currencyId))
            {
                viewModel.CurrentCategory = category;
                var units = viewModel.Units.ToList();
                Assert.IsTrue(units.Count > 0, $"Category '{category.Name}' exposed no units.");

                var reference = units[0];
                foreach (var unit in units)
                {
                    viewModel.Unit1 = unit;
                    viewModel.Unit2 = reference;
                    viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Clear);
                    viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.One);

                    AssertIsRealNumber(viewModel.Value2, category.Name, unit.Name, reference.Name);

                    viewModel.Unit1 = reference;
                    viewModel.Unit2 = unit;
                    AssertIsRealNumber(viewModel.Value2, category.Name, reference.Name, unit.Name);
                    unitsChecked++;
                }
            }

            Assert.IsTrue(unitsChecked > 100, $"Only {unitsChecked} units were checked.");
        }
```

##### `EnteringValueAfterSwitchingActiveUpdatesSecondValue` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:165-174`

```csharp
        [TestMethod]
        public void EnteringValueAfterSwitchingActiveUpdatesSecondValue()
        {
            var viewModel = new UnitConverterViewModel();
            viewModel.SwitchActiveCommand.Execute(null);

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Seven);

            Assert.AreEqual("7", viewModel.Value2);
        }
```

##### `MaxDigitsAnnouncementIncludesTheConversionResult` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:176-189`

```csharp
        [TestMethod]
        public void MaxDigitsAnnouncementIncludesTheConversionResult()
        {
            var viewModel = new UnitConverterViewModel();
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Five);
            viewModel.OnMaxDigitsReached();

            var announcement = viewModel.Announcement?.Announcement;

            Assert.IsFalse(string.IsNullOrEmpty(announcement));
            Assert.IsFalse(
                announcement.Contains("%1"),
                $"The format placeholder was never substituted: '{announcement}'.");
        }
```

##### `ConversionResultNarrationSubstitutesAllPlaceholders` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:191-205`

```csharp
        [TestMethod]
        public void ConversionResultNarrationSubstitutesAllPlaceholders()
        {
            var viewModel = new UnitConverterViewModel();

            string result = viewModel.GetLocalizedConversionResultStringFormat(
                "1", "meter", "3.28", "feet");

            Assert.IsFalse(result.Contains("%1"));
            Assert.IsFalse(result.Contains("%2"));
            Assert.IsFalse(result.Contains("%3"));
            Assert.IsFalse(result.Contains("%4"));
            StringAssert.Contains(result, "meter");
            StringAssert.Contains(result, "feet");
        }
```

##### `SwitchingActiveValueSwapsTheFromAndToAutomationFormats` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:207-226`

```csharp
        [TestMethod]
        public void SwitchingActiveValueSwapsTheFromAndToAutomationFormats()
        {
            var viewModel = new UnitConverterViewModel();
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Five);

            var value1NameBefore = viewModel.Value1AutomationName;
            var value2NameBefore = viewModel.Value2AutomationName;

            viewModel.SwitchActiveCommand.Execute(null);
            viewModel.UpdateValue1AutomationName();
            viewModel.UpdateValue2AutomationName();

            Assert.AreNotEqual(
                StripDigits(value1NameBefore),
                StripDigits(viewModel.Value1AutomationName));
            Assert.AreNotEqual(
                StripDigits(value2NameBefore),
                StripDigits(viewModel.Value2AutomationName));
        }
```

##### `PastingAMinusAfterDigitsDoesNotNegateTheValue` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:233-242`

```csharp
        [TestMethod]
        public void PastingAMinusAfterDigitsDoesNotNegateTheValue()
        {
            var viewModel = new UnitConverterViewModel();
            SelectNegatableCategory(viewModel);

            viewModel.OnPaste("5-3");

            Assert.AreEqual("53", viewModel.Value1);
        }
```

##### `PastingALeadingMinusNegatesTheValue` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:244-253`

```csharp
        [TestMethod]
        public void PastingALeadingMinusNegatesTheValue()
        {
            var viewModel = new UnitConverterViewModel();
            SelectNegatableCategory(viewModel);

            viewModel.OnPaste("-53");

            Assert.AreEqual("-53", viewModel.Value1);
        }
```

##### `TextWithNoUsableNumberIsRejectedBeforeItReachesTheConverter` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:261-276`

```csharp
        [TestMethod]
        public void TextWithNoUsableNumberIsRejectedBeforeItReachesTheConverter()
        {
            foreach (string candidate in new[] { "-", "-abc", ".", "abc" })
            {
                Assert.AreEqual(
                    "NoOp",
                    CopyPasteManager.ValidatePasteExpression(
                        candidate,
                        ViewMode.Length,
                        CategoryGroupType.Converter,
                        NumberBase.Unknown,
                        BitLength.BitLengthUnknown),
                    $"'{candidate}' should be rejected as a paste for a converter.");
            }
        }
```

##### `PartialDisplayValuesDoNotThrowDuringFormatting` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:278-287`

```csharp
        [TestMethod]
        public void PartialDisplayValuesDoNotThrowDuringFormatting()
        {
            var viewModel = new UnitConverterViewModel();

            viewModel.UpdateDisplay("-", ".");

            Assert.AreEqual("-", viewModel.Value1);
            Assert.AreEqual(".", viewModel.Value2);
        }
```

##### `RejectedPasteSaysWhyInsteadOfBlankingTheDisplay` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:289-302`

```csharp
        [TestMethod]
        public void RejectedPasteSaysWhyInsteadOfBlankingTheDisplay()
        {
            var viewModel = new UnitConverterViewModel();
            SelectNegatableCategory(viewModel);
            viewModel.OnPaste("53");
            Assert.AreEqual("53", viewModel.Value1);

            viewModel.OnPaste("NoOp");

            Assert.IsFalse(string.IsNullOrEmpty(viewModel.Value1));
            Assert.AreEqual(viewModel.Value1, viewModel.Value2);
            Assert.AreNotEqual("53", viewModel.Value1);
        }
```

##### `LargeValuesAreDisplayedWithGroupSeparators` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:304-325`

```csharp
        [TestMethod]
        public void LargeValuesAreDisplayedWithGroupSeparators()
        {
            var viewModel = new UnitConverterViewModel();
            foreach (var digit in new[]
            {
                NumbersAndOperatorsEnum.One,
                NumbersAndOperatorsEnum.Two,
                NumbersAndOperatorsEnum.Three,
                NumbersAndOperatorsEnum.Four,
                NumbersAndOperatorsEnum.Five,
                NumbersAndOperatorsEnum.Six,
                NumbersAndOperatorsEnum.Seven
            })
            {
                viewModel.ButtonPressedCommand.Execute(digit);
            }

            var separator = LocalizationSettings.GetInstance().GetNumberGroupingSeparatorStr();

            StringAssert.Contains(viewModel.Value1, separator);
        }
```

##### `LengthSuggestionsPreserveWhimsicalUnitMetadata` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:327-349`

```csharp
        [TestMethod]
        public void LengthSuggestionsPreserveWhimsicalUnitMetadata()
        {
            var viewModel = CreateLengthViewModel();
            var resources = AppResourceProvider.GetInstance();
            Unit centimeters = viewModel.Units.Single(
                unit => unit.Name == resources.GetResourceString("UnitName_Centimeter"));
            Unit inches = viewModel.Units.Single(
                unit => unit.Name == resources.GetResourceString("UnitName_Inch"));
            SelectUnit(viewModel, centimeters, isFromUnit: true);
            SelectUnit(viewModel, inches, isFromUnit: false);

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Clear);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Four);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Seven);

            SupplementaryResult result = viewModel.SupplementaryResults.Last();
            Assert.AreEqual(resources.GetResourceString("UnitName_Hand"), result.Unit.Name);
            Assert.AreEqual(
                resources.GetResourceString("UnitAbbreviation_Hand"),
                result.Unit.Abbreviation);
            Assert.IsTrue(result.IsWhimsical());
        }
```

##### `EnteringDigitAfterCurrencyUnitChangeReplacesValue` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:351-368`

```csharp
        [TestMethod]
        public async Task EnteringDigitAfterCurrencyUnitChangeReplacesValue()
        {
            var viewModel = new UnitConverterViewModel();
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == currencyId);
            await WaitForCurrencyUnitsAsync(viewModel);

            viewModel.OnPaste("1.23");
            Unit replacement = viewModel.Units.First(
                unit => unit.ModelUnitID() != viewModel.Unit1.ModelUnitID());
            SelectUnit(viewModel, replacement, isFromUnit: true);

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Seven);

            Assert.AreEqual("7", viewModel.Value1);
        }
```

##### `EnteringCurrencyAfterBackgroundLoadUsesLoadedRatios` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:370-394`

```csharp
        [TestMethod]
        public async Task EnteringCurrencyAfterBackgroundLoadUsesLoadedRatios()
        {
            var viewModel = new UnitConverterViewModel();
            Assert.IsFalse(viewModel.IsCurrencyCurrentCategory);

            await WaitForCurrencyLoadAsync(viewModel);

            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == currencyId);
            await WaitForCurrencyUnitsAsync(viewModel);

            Unit mars = viewModel.Units.First(unit => unit.Abbreviation == "MAR");
            Unit moon = viewModel.Units.First(unit => unit.Abbreviation == "MON");
            SelectUnit(viewModel, mars, isFromUnit: true);
            SelectUnit(viewModel, moon, isFromUnit: false);

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.One);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Zero);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Zero);

            Assert.AreEqual("100", viewModel.Value1);
            Assert.AreEqual("50", viewModel.Value2);
        }
```

##### `CurrencyLoadFinishingInsideCurrencyUsesLoadedRatios` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:396-418`

```csharp
        [TestMethod]
        public async Task CurrencyLoadFinishingInsideCurrencyUsesLoadedRatios()
        {
            var viewModel = new UnitConverterViewModel();
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == currencyId);
            Assert.IsFalse(viewModel.IsCurrencyDataLoaded);

            await WaitForCurrencyUnitsAsync(viewModel);

            Unit mars = viewModel.Units.First(unit => unit.Abbreviation == "MAR");
            Unit moon = viewModel.Units.First(unit => unit.Abbreviation == "MON");
            SelectUnit(viewModel, mars, isFromUnit: true);
            SelectUnit(viewModel, moon, isFromUnit: false);

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.One);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Zero);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Zero);

            Assert.AreEqual("100", viewModel.Value1);
            Assert.AreEqual("50", viewModel.Value2);
        }
```

##### `LeavingCurrencyClearsTheCurrencySymbolsAndRatio` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:420-439`

```csharp
        [TestMethod]
        public async Task LeavingCurrencyClearsTheCurrencySymbolsAndRatio()
        {
            var viewModel = new UnitConverterViewModel();
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == currencyId);
            await WaitForCurrencyUnitsAsync(viewModel);

            Assert.IsFalse(string.IsNullOrEmpty(viewModel.CurrencySymbol1));

            int lengthId = NavCategoryStates.Serialize(ViewMode.Length);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == lengthId);

            Assert.AreEqual(string.Empty, viewModel.CurrencySymbol1);
            Assert.AreEqual(string.Empty, viewModel.CurrencySymbol2);
            Assert.AreEqual(Visibility.Collapsed, viewModel.CurrencySymbolVisibility);
            Assert.AreEqual(string.Empty, viewModel.CurrencyRatioEquality);
        }
```

##### `CurrencyRefreshCompletesAfterInitialLoad` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:441-454`

```csharp
        [TestMethod]
        public async Task CurrencyRefreshCompletesAfterInitialLoad()
        {
            var viewModel = new UnitConverterViewModel();
            await WaitForCurrencyLoadAsync(viewModel);
            viewModel.OnCurrencyTimestampUpdated("stale timestamp", isWeekOld: true);

            await viewModel.RefreshCurrencyRatiosAsync();

            Assert.IsTrue(viewModel.IsCurrencyDataLoaded);
            Assert.IsFalse(viewModel.IsCurrencyLoadingVisible);
            Assert.IsFalse(viewModel.CurrencyDataLoadFailed);
            Assert.AreNotEqual("stale timestamp", viewModel.CurrencyTimestamp);
        }
```

##### `LocaleDefaultCurrencyMapIsPackaged` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:456-467`

```csharp
        [TestMethod]
        public async Task LocaleDefaultCurrencyMapIsPackaged()
        {
            var file = await Windows.Storage.StorageFile.GetFileFromApplicationUriAsync(
                new Uri("ms-appx:///DataLoaders/DefaultFromToCurrency.json"));
            string json = await Windows.Storage.FileIO.ReadTextAsync(file);

            StringAssert.Contains(json, "\"en-GB\"");
            StringAssert.Contains(json, "\"GBP\"");
            StringAssert.Contains(json, "\"en-CA\"");
            StringAssert.Contains(json, "\"CAD\"");
        }
```

##### `ConverterCommandsKeepTheirIdentityAcrossReads` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:469-481`

```csharp
        [TestMethod]
        public void ConverterCommandsKeepTheirIdentityAcrossReads()
        {
            var viewModel = new UnitConverterViewModel();

            Assert.AreSame(viewModel.CategoryChangedCommand, viewModel.CategoryChangedCommand);
            Assert.AreSame(viewModel.UnitChangedCommand, viewModel.UnitChangedCommand);
            Assert.AreSame(viewModel.SwitchActiveCommand, viewModel.SwitchActiveCommand);
            Assert.AreSame(viewModel.ButtonPressedCommand, viewModel.ButtonPressedCommand);
            Assert.AreSame(viewModel.CopyCommand, viewModel.CopyCommand);
            Assert.AreSame(viewModel.PasteCommand, viewModel.PasteCommand);
            Assert.AreSame(viewModel.ButtonPressedCommand, viewModel.ButtonPressed);
        }
```

##### `OtherViewModelCommandsKeepTheirIdentityAcrossReads` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:483-497`

```csharp
        [TestMethod]
        public void OtherViewModelCommandsKeepTheirIdentityAcrossReads()
        {
            var standard = new StandardCalculatorViewModel();
            HistoryViewModel history = standard.HistoryVM;
            Assert.AreSame(history.ClearCommand, history.ClearCommand);
            Assert.AreSame(history.HideCommand, history.HideCommand);

            var dateCalculator = new DateCalculatorViewModel();
            Assert.AreSame(dateCalculator.CopyCommand, dateCalculator.CopyCommand);

            var application = new ApplicationViewModel();
            Assert.AreSame(application.CopyCommand, application.CopyCommand);
            Assert.AreSame(application.PasteCommand, application.PasteCommand);
        }
```

##### `ConstructionAndCategorySwitchingKeepTheConverterConsistent` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:499-527`

```csharp
        [TestMethod]
        public void ConstructionAndCategorySwitchingKeepTheConverterConsistent()
        {
            var viewModel = new UnitConverterViewModel();

            Assert.IsTrue(viewModel.Categories.Count > 0);
            Assert.IsTrue(viewModel.Units.Count > 0);
            Assert.IsNotNull(viewModel.CurrentCategory);
            Assert.IsNotNull(viewModel.Unit1);
            Assert.IsNotNull(viewModel.Unit2);
            Assert.IsTrue(viewModel.Value1Active ^ viewModel.Value2Active);

            var original = viewModel.CurrentCategory;
            var originalUnits = viewModel.Units.Select(unit => unit.ModelUnitID()).ToList();
            var other = viewModel.Categories.First(
                category => category.GetModelCategoryId() != original.GetModelCategoryId());

            viewModel.CurrentCategory = other;
            CollectionAssert.AreNotEqual(
                originalUnits,
                viewModel.Units.Select(unit => unit.ModelUnitID()).ToList());
            Assert.IsTrue(viewModel.Units.Contains(viewModel.Unit1));
            Assert.IsTrue(viewModel.Units.Contains(viewModel.Unit2));

            viewModel.CurrentCategory = original;
            CollectionAssert.AreEqual(
                originalUnits,
                viewModel.Units.Select(unit => unit.ModelUnitID()).ToList());
        }
```

##### `CategorySwitchPublishesNewUnitsBeforeSelectedUnits` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:529-571`

```csharp
        [TestMethod]
        public void CategorySwitchPublishesNewUnitsBeforeSelectedUnits()
        {
            var viewModel = new UnitConverterViewModel();
            var originalCollection = viewModel.Units;
            var originalUnits = originalCollection.Select(unit => unit.ModelUnitID()).ToList();
            int sequence = 0;
            int unitsChanged = -1;
            int unit1Changed = -1;
            int unit2Changed = -1;

            viewModel.PropertyChanged += (sender, args) =>
            {
                sequence++;
                if (args.PropertyName == nameof(UnitConverterViewModel.Units))
                {
                    unitsChanged = sequence;
                }
                else if (args.PropertyName == nameof(UnitConverterViewModel.Unit1))
                {
                    unit1Changed = sequence;
                }
                else if (args.PropertyName == nameof(UnitConverterViewModel.Unit2))
                {
                    unit2Changed = sequence;
                }
            };

            var other = viewModel.Categories.First(
                category => category.GetModelCategoryId() != viewModel.CurrentCategory.GetModelCategoryId()
                    && category.GetModelCategoryId() != NavCategoryStates.Serialize(ViewMode.Currency));
            viewModel.CurrentCategory = other;

            Assert.AreNotSame(originalCollection, viewModel.Units);
            CollectionAssert.AreEqual(
                originalUnits,
                originalCollection.Select(unit => unit.ModelUnitID()).ToList());
            Assert.IsTrue(unitsChanged > 0);
            Assert.IsTrue(unit1Changed > unitsChanged);
            Assert.IsTrue(unit2Changed > unitsChanged);
            Assert.IsTrue(viewModel.Units.Contains(viewModel.Unit1));
            Assert.IsTrue(viewModel.Units.Contains(viewModel.Unit2));
        }
```

##### `InputFollowsTheActiveValueAndIsFormattedForDisplay` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:573-600`

```csharp
        [TestMethod]
        public void InputFollowsTheActiveValueAndIsFormattedForDisplay()
        {
            var viewModel = new UnitConverterViewModel();
            var separator = LocalizationSettings.GetInstance().GetDecimalSeparatorStr();
            bool firstWasActive = viewModel.Value1Active;

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.One);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Decimal);
            StringAssert.EndsWith(viewModel.Value1, separator);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Five);
            Assert.AreEqual($"1{separator}5", viewModel.Value1);

            viewModel.SwitchActiveCommand.Execute(null);
            Assert.AreNotEqual(firstWasActive, viewModel.Value1Active);
            Assert.IsTrue(viewModel.Value1Active ^ viewModel.Value2Active);

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Eight);
            Assert.AreEqual("8", viewModel.Value2);

            viewModel.SwitchActiveCommand.Execute(null);
            Assert.AreEqual(firstWasActive, viewModel.Value1Active);
            Assert.IsTrue(viewModel.Value1Active ^ viewModel.Value2Active);

            viewModel.UpdateValue1AutomationName();
            Assert.IsFalse(string.IsNullOrEmpty(viewModel.Value1AutomationName));
            StringAssert.Contains(viewModel.Value1AutomationName, viewModel.Unit1.AccessibleName);
        }
```

#### 5.4.3 Currency converter

##### `Test_RoundCurrencyRatio` — `CurrencyConverterLoadTests` — `CurrencyConverterTests.cs:324-363`

```csharp
        [TestMethod]
        public void Test_RoundCurrencyRatio()
        {
            (double Ratio, double Expected)[] cases =
            {
                (1234567, 1234567),
                (0, 0),
                (9999.999, 9999.999),
                (8765.4321, 8765.4321),
                (4815.162342, 4815.1623),
                (4815.162358, 4815.1624),
                (4815.162388934723, 4815.1624),
                (0.12, 0.12),
                (0.123, 0.123),
                (0.1234, 0.1234),
                (0.12343, 0.1234),
                (0.0321, 0.0321),
                (0.03211, 0.03211),
                (0.032119, 0.03212),
                (0.00322119, 0.003221),
                (0.00123269, 0.001233),
                (0.00076269, 0.0007627),
                (0.000069, 0.000069),
                (0.000061, 0.000061),
                (0.000054612, 0.00005461),
                (0.000054616, 0.00005462),
                (0.000005416, 0.000005416),
                (0.0000016134324, 0.000001613),
                (0.0000096134324, 0.000009613),
                (0.0000032169348392, 0.000003217),
                (0.000000002134987218, 0.000000002135),
                (0.000000000000087231445, 0.00000000000008723),
            };

            foreach (var (ratio, expected) in cases)
            {
                Assert.AreEqual(expected, CurrencyDataLoader.RoundCurrencyRatio(ratio), 0d,
                    $"RoundCurrencyRatio({ratio})");
            }
        }
```

### 5.5 Memory & history vectors

`CalculatorHistory` cap behaviour, history item expression pretty-printing, memory add/subtract/
recall/stack semantics, and per-instance isolation across windows.

#### 5.5.1 `TEST_CLASS(CalculatorManagerTest)` — memory (engine `ICalcDisplay::SetMemorizedNumbers`)

##### `CalculatorManagerTestMemory` — `CalculatorManagerTest` — `CalculatorManagerTest.cpp:815-960`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestMemory()
    {
        Command scientificCalculatorTest52[] = { Command::Command1, Command::CommandSTORE, Command::CommandNULL };
        wstring expectedPrimaryDisplayTestScientific52(L"1");
        wstring expectedExpressionDisplayTestScientific52(L"");

        Command scientificCalculatorTest53[] = { Command::Command1, Command::CommandNULL };
        wstring expectedPrimaryDisplayTestScientific53(L"1");
        wstring expectedExpressionDisplayTestScientific53(L"");

        CalculatorManagerDisplayTester* pCalculatorDisplay = (CalculatorManagerDisplayTester*)m_calculatorDisplayTester.get();
        wstring resultPrimary = L"";
        wstring resultExpression = L"";

        Cleanup();
        ExecuteCommands(scientificCalculatorTest52);
        resultPrimary = pCalculatorDisplay->GetPrimaryDisplay();
        resultExpression = pCalculatorDisplay->GetExpression();
        VERIFY_ARE_EQUAL(expectedPrimaryDisplayTestScientific52, resultPrimary);

        Cleanup();
        ExecuteCommands(scientificCalculatorTest53);
        m_calculatorManager->MemorizeNumber();
        m_calculatorManager->SendCommand(Command::CommandCLEAR);
        m_calculatorManager->MemorizedNumberLoad(0);
        resultPrimary = pCalculatorDisplay->GetPrimaryDisplay();
        resultExpression = pCalculatorDisplay->GetExpression();
        VERIFY_ARE_EQUAL(expectedPrimaryDisplayTestScientific52, resultPrimary);

        Cleanup();
        m_calculatorManager->SendCommand(Command::Command1);
        m_calculatorManager->MemorizeNumber();
        m_calculatorManager->SendCommand(Command::CommandCLEAR);
        m_calculatorManager->SendCommand(Command::Command2);
        m_calculatorManager->MemorizeNumber();
        m_calculatorManager->SendCommand(Command::CommandCLEAR);
        m_calculatorManager->MemorizedNumberLoad(1);
        resultPrimary = pCalculatorDisplay->GetPrimaryDisplay();
        VERIFY_ARE_EQUAL(wstring(L"1"), resultPrimary);

        m_calculatorManager->MemorizedNumberLoad(0);
        resultPrimary = pCalculatorDisplay->GetPrimaryDisplay();
        VERIFY_ARE_EQUAL(wstring(L"2"), resultPrimary);

        Cleanup();
        m_calculatorManager->SendCommand(Command::Command1);
        m_calculatorManager->SendCommand(Command::CommandSIGN);
        m_calculatorManager->MemorizeNumber();
        m_calculatorManager->SendCommand(Command::CommandADD);
        m_calculatorManager->SendCommand(Command::Command2);
        m_calculatorManager->SendCommand(Command::CommandEQU);
        m_calculatorManager->MemorizeNumber();
        m_calculatorManager->SendCommand(Command::CommandMUL);
        m_calculatorManager->SendCommand(Command::Command2);
        m_calculatorManager->MemorizeNumber();

        vector<wstring> memorizedNumbers = pCalculatorDisplay->GetMemorizedNumbers();

        vector<wstring> expectedMemorizedNumbers;
        expectedMemorizedNumbers.push_back(L"2");
        expectedMemorizedNumbers.push_back(L"1");
        expectedMemorizedNumbers.push_back(L"-1");

        bool isEqual = false;
        if (memorizedNumbers.size() < expectedMemorizedNumbers.size())
        {
            isEqual = std::equal(memorizedNumbers.begin(), memorizedNumbers.end(), expectedMemorizedNumbers.begin());
        }
        else
        {
            isEqual = std::equal(expectedMemorizedNumbers.begin(), expectedMemorizedNumbers.end(), memorizedNumbers.begin());
        }
        VERIFY_IS_TRUE(isEqual);

        m_calculatorManager->SendCommand(Command::CommandCLEAR);
        m_calculatorManager->SendCommand(Command::Command2);
        m_calculatorManager->MemorizedNumberAdd(0);
        m_calculatorManager->MemorizedNumberAdd(1);
        m_calculatorManager->MemorizedNumberAdd(2);

        memorizedNumbers = pCalculatorDisplay->GetMemorizedNumbers();

        expectedMemorizedNumbers.clear();
        expectedMemorizedNumbers.push_back(L"4");
        expectedMemorizedNumbers.push_back(L"3");
        expectedMemorizedNumbers.push_back(L"1");

        if (memorizedNumbers.size() < expectedMemorizedNumbers.size())
        {
            isEqual = std::equal(memorizedNumbers.begin(), memorizedNumbers.end(), expectedMemorizedNumbers.begin());
        }
        else
        {
            isEqual = std::equal(expectedMemorizedNumbers.begin(), expectedMemorizedNumbers.end(), memorizedNumbers.begin());
        }
        VERIFY_IS_TRUE(isEqual);

        m_calculatorManager->SendCommand(Command::CommandCLEAR);
        m_calculatorManager->SendCommand(Command::Command1);
        m_calculatorManager->SendCommand(Command::CommandPNT);
        m_calculatorManager->SendCommand(Command::Command5);

        m_calculatorManager->MemorizedNumberSubtract(0);
        m_calculatorManager->MemorizedNumberSubtract(1);
        m_calculatorManager->MemorizedNumberSubtract(2);

        memorizedNumbers = pCalculatorDisplay->GetMemorizedNumbers();

        expectedMemorizedNumbers.clear();
        expectedMemorizedNumbers.push_back(L"2.5");
        expectedMemorizedNumbers.push_back(L"1.5");
        expectedMemorizedNumbers.push_back(L"-0.5");

        if (memorizedNumbers.size() < expectedMemorizedNumbers.size())
        {
            isEqual = std::equal(memorizedNumbers.begin(), memorizedNumbers.end(), expectedMemorizedNumbers.begin());
        }
        else
        {
            isEqual = std::equal(expectedMemorizedNumbers.begin(), expectedMemorizedNumbers.end(), memorizedNumbers.begin());
        }
        VERIFY_IS_TRUE(isEqual);

        // Memorizing 101 numbers, which exceeds the limit.
        Cleanup();
        for (int i = 0; i < 101; i++)
        {
            m_calculatorManager->SendCommand(Command::Command1);
            m_calculatorManager->MemorizeNumber();
        }

        memorizedNumbers = pCalculatorDisplay->GetMemorizedNumbers();
        VERIFY_ARE_EQUAL((size_t)100, memorizedNumbers.size());

        // Memorizing new number, which should show up at the top of the memory
        m_calculatorManager->SendCommand(Command::Command2);
        m_calculatorManager->MemorizeNumber();
        memorizedNumbers = pCalculatorDisplay->GetMemorizedNumbers();
        VERIFY_ARE_EQUAL(wstring(L"2"), memorizedNumbers.at(0));

        // Test for trying to memorize invalid value
        m_calculatorManager->SendCommand(Command::Command2);
        m_calculatorManager->SendCommand(Command::CommandSIGN);
        m_calculatorManager->SendCommand(Command::CommandSQRT);
        m_calculatorManager->MemorizeNumber();
    }
```

#### 5.5.2 `HistoryTests` (36 tests)

##### `TestHistoryItemClicked` — `HistoryTests` — `HistoryTests.cs:95-118`

```csharp
        [TestMethod]
        public void TestHistoryItemClicked()
        {
            Initialize();
            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command5);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command3);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1];
            m_standardViewModel.SelectHistoryItem(historyItem);
            Assert.AreEqual("9", m_standardViewModel.DisplayValue);
            Assert.AreEqual("1", m_standardViewModel.ExpressionTokens[0].Token);
            Assert.AreEqual(" ", m_standardViewModel.ExpressionTokens[1].Token);
            Assert.AreEqual("+", m_standardViewModel.ExpressionTokens[2].Token);
            Assert.AreEqual(" ", m_standardViewModel.ExpressionTokens[3].Token);
            Assert.AreEqual("5", m_standardViewModel.ExpressionTokens[4].Token);
            Assert.AreEqual(" ", m_standardViewModel.ExpressionTokens[5].Token);
            Assert.AreEqual("+", m_standardViewModel.ExpressionTokens[6].Token);
            Assert.AreEqual(" ", m_standardViewModel.ExpressionTokens[7].Token);
            Cleanup();
        }
```

##### `TestHistoryItemAddSingleItem` — `HistoryTests` — `HistoryTests.cs:120-136`

```csharp
        [TestMethod]
        public void TestHistoryItemAddSingleItem()
        {
            Initialize();
            int initialSize = m_historyViewModel.ItemsCount;
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command8);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            int sizeAfterItemAdd = m_historyViewModel.ItemsCount;
            var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[0];
            string expression = "1   +   8 =";
            Assert.AreEqual(initialSize + 1, sizeAfterItemAdd);
            Assert.AreEqual(expression, historyItem.Expression);
            Assert.AreEqual("9", historyItem.Result);
            Cleanup();
        }
```

##### `TestHistoryItemAddMaxItems` — `HistoryTests` — `HistoryTests.cs:138-171`

```csharp
        [TestMethod]
        public void TestHistoryItemAddMaxItems()
        {
            Initialize();
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            for (ulong i = 1; i < m_historyViewModel.GetMaxItemSize(); i++)
            {
                m_standardViewModel.SendCommandToCalcManager(Command1);
                m_standardViewModel.SendCommandToCalcManager(CommandADD);
                m_standardViewModel.SendCommandToCalcManager(Command2);
                m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            }
            Assert.AreEqual((int)m_historyViewModel.GetMaxItemSize(), m_historyViewModel.ItemsCount);
            string expression = "1   +   1 =";
            int output = 2;
            string result = output.ToString();
            var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1];
            Assert.AreEqual(expression, historyItem.Expression);
            Assert.AreEqual(result, historyItem.Result);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command5);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            expression = "1   +   2 =";
            output = 3;
            result = output.ToString();
            historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1];
            Assert.AreEqual(expression, historyItem.Expression);
            Assert.AreEqual(result, historyItem.Result);
            Cleanup();
        }
```

##### `TestHistoryClearCommand` — `HistoryTests` — `HistoryTests.cs:173-190`

```csharp
        [TestMethod]
        public void TestHistoryClearCommand()
        {
            Initialize();
            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command2);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            m_standardViewModel.SendCommandToCalcManager(ModeBasic);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command2);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            m_historyViewModel.ClearCommand.Execute(null);
            Assert.AreEqual(0, m_historyViewModel.ItemsCount);
            Cleanup();
        }
```

##### `TestHistoryClearCommandWithEmptyHistory` — `HistoryTests` — `HistoryTests.cs:192-201`

```csharp
        [TestMethod]
        public void TestHistoryClearCommandWithEmptyHistory()
        {
            Initialize();
            Assert.AreEqual(0, m_historyViewModel.ItemsCount);
            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            m_historyViewModel.ClearCommand.Execute(null);
            Assert.AreEqual(0, m_historyViewModel.ItemsCount);
            Cleanup();
        }
```

##### `TestReLoadHistory` — `HistoryTests` — `HistoryTests.cs:203-257`

```csharp
        [TestMethod]
        public void TestReLoadHistory()
        {
            Initialize();

            m_standardViewModel.ResetCalcManager(false);
            int scientificItems = 5;
            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            for (int i = 0; i < scientificItems; i++)
            {
                int nextCommand = 130 + i;
                m_standardViewModel.SendCommandToCalcManager(Command1);
                m_standardViewModel.SendCommandToCalcManager(CommandADD);
                m_standardViewModel.SendCommandToCalcManager(nextCommand);
                m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            }

            m_standardViewModel.SendCommandToCalcManager(ModeBasic);
            int standardItems = 2;
            for (int i = 0; i < standardItems; i++)
            {
                int nextCommand = 130 + i;
                m_standardViewModel.SendCommandToCalcManager(Command1);
                m_standardViewModel.SendCommandToCalcManager(CommandADD);
                m_standardViewModel.SendCommandToCalcManager(nextCommand);
                m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            }

            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            m_historyViewModel.ReloadHistory(ViewMode.Scientific);
            Assert.AreEqual(scientificItems, m_historyViewModel.ItemsCount);
            for (int i = 0; i < scientificItems; i++)
            {
                string expr = "1   +   " + i.ToString() + " =";
                int output = 1 + i;
                string result = output.ToString();
                var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1 - i];
                Assert.AreEqual(expr, historyItem.Expression);
                Assert.AreEqual(result, historyItem.Result);
            }

            m_historyViewModel.ReloadHistory(ViewMode.Standard);
            m_standardViewModel.SendCommandToCalcManager(ModeBasic);
            Assert.AreEqual(standardItems, m_historyViewModel.ItemsCount);
            for (int i = 0; i < standardItems; i++)
            {
                string expr = "1   +   " + i.ToString() + " =";
                int output = 1 + i;
                string result = output.ToString();
                var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1 - i];
                Assert.AreEqual(expr, historyItem.Expression);
                Assert.AreEqual(result, historyItem.Result);
            }
            Cleanup();
        }
```

##### `TestHistoryItemWithPrettyExpressions` — `HistoryTests` — `HistoryTests.cs:259-277`

```csharp
        [TestMethod]
        public void TestHistoryItemWithPrettyExpressions()
        {
            Initialize();
            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            m_standardViewModel.SendCommandToCalcManager(Command2);
            m_standardViewModel.SendCommandToCalcManager(CommandSQRT);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);

            var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1];

            // The stored expression is the readable form, not the raw command name.
            Assert.IsFalse(string.IsNullOrEmpty(historyItem.Expression));
            Assert.IsFalse(
                historyItem.Expression.Contains("CommandSQRT"),
                $"The expression shows a command name rather than a symbol: '{historyItem.Expression}'.");
            Assert.AreEqual("1.4142135623730950488016887242097", historyItem.Result);
            Cleanup();
        }
```

##### `TestHistoryItemWithPrettyExpressionsAcrossAngleModes` — `HistoryTests` — `HistoryTests.cs:279-305`

```csharp
        [TestMethod]
        public void TestHistoryItemWithPrettyExpressionsAcrossAngleModes()
        {
            Initialize();
            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            m_standardViewModel.SendCommandToCalcManager(CommandDEG);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandSIN);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(CommandRAD);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandSIN);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(CommandGRAD);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandSIN);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);

            var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1];
            var resources = AppResourceProvider.GetInstance();
            string expected = resources.GetCEngineString("67") + "( 1 )   +   "
                + resources.GetCEngineString("73") + "( 1 )   +   "
                + resources.GetCEngineString("79") + "( 1 ) =";

            Assert.AreEqual(expected, historyItem.Expression);
            Cleanup();
        }
```

##### `TestHistoryItemLoadAndContinueCalculation` — `HistoryTests` — `HistoryTests.cs:307-334`

```csharp
        [TestMethod]
        public void TestHistoryItemLoadAndContinueCalculation()
        {
            Initialize();
            m_standardViewModel.SendCommandToCalcManager(ModeBasic);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command5);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command3);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);

            var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1];
            m_standardViewModel.SelectHistoryItem(historyItem);

            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command5);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            Assert.AreEqual("11", m_standardViewModel.DisplayValue);
            historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1];
            m_standardViewModel.SelectHistoryItem(historyItem);
            Assert.AreEqual("6", m_standardViewModel.DisplayValue);

            historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 2];
            m_standardViewModel.SelectHistoryItem(historyItem);
            Assert.AreEqual("9", m_standardViewModel.DisplayValue);
            Cleanup();
        }
```

##### `TestDisplayValueAutomationNames` — `HistoryTests` — `HistoryTests.cs:336-361`

```csharp
        [TestMethod]
        public void TestDisplayValueAutomationNames()
        {
            Initialize();
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command8);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            Assert.AreEqual("Display is 9", m_standardViewModel.CalculationResultAutomationName);

            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command5);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            Assert.AreEqual("Display is 6", m_standardViewModel.CalculationResultAutomationName);

            m_standardViewModel.SendCommandToCalcManager(ModeProgrammer);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command2);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            Assert.AreEqual("Display is 3", m_standardViewModel.CalculationResultAutomationName);

            Cleanup();
        }
```

##### `TestRadixAutomationName` — `HistoryTests` — `HistoryTests.cs:363-377`

```csharp
        [TestMethod]
        public void TestRadixAutomationName()
        {
            Initialize();
            m_standardViewModel.SendCommandToCalcManager(ModeProgrammer);
            m_standardViewModel.IsProgrammer = true;
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command7);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            Assert.AreEqual("HexaDecimal 8", "HexaDecimal " + m_standardViewModel.HexDisplayValue);
            Assert.AreEqual("Octal 10", "Octal " + m_standardViewModel.OctalDisplayValue);
            Assert.AreEqual("Binary 1000", "Binary " + m_standardViewModel.BinaryDisplayValue);
            Cleanup();
        }
```

##### `TestHistoryEmpty` — `HistoryTests` — `HistoryTests.cs:379-387`

```csharp
        [TestMethod]
        public void TestHistoryEmpty()
        {
            Initialize();
            Assert.AreEqual(0, m_historyViewModel.ItemsCount);
            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            Assert.AreEqual(0, m_historyViewModel.ItemsCount);
            Cleanup();
        }
```

##### `TestHistoryStandardOrderOfOperations_1` — `HistoryTests` — `HistoryTests.cs:389-394`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_1()
        {
            int[] commands = { Command1, CommandREC, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }
```

##### `TestHistoryStandardOrderOfOperations_2` — `HistoryTests` — `HistoryTests.cs:396-401`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_2()
        {
            int[] commands = { Command4, CommandSQRT, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }
```

##### `TestHistoryStandardOrderOfOperations_3` — `HistoryTests` — `HistoryTests.cs:403-408`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_3()
        {
            int[] commands = { Command1, CommandADD, Command4, CommandSQRT, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }
```

##### `TestHistoryStandardOrderOfOperations_4` — `HistoryTests` — `HistoryTests.cs:410-415`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_4()
        {
            int[] commands = { Command1, CommandADD, Command4, CommandSQRT, CommandSUB, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("3", "1   +   \x221A( 4 ) =", commands);
        }
```

##### `TestHistoryStandardOrderOfOperations_5` — `HistoryTests` — `HistoryTests.cs:417-422`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_5()
        {
            int[] commands = { Command2, CommandMUL, Command4, CommandREC, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }
```

##### `TestHistoryStandardOrderOfOperations_6` — `HistoryTests` — `HistoryTests.cs:424-429`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_6()
        {
            int[] commands = { Command5, CommandDIV, Command6, CommandPERCENT, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }
```

##### `TestHistoryStandardOrderOfOperations_7` — `HistoryTests` — `HistoryTests.cs:431-436`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_7()
        {
            int[] commands = { Command4, CommandSQRT, CommandSUB, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }
```

##### `TestHistoryStandardOrderOfOperations_8` — `HistoryTests` — `HistoryTests.cs:438-443`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_8()
        {
            int[] commands = { Command7, CommandSQR, CommandDIV, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }
```

##### `TestHistoryStandardOrderOfOperations_9` — `HistoryTests` — `HistoryTests.cs:445-450`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_9()
        {
            int[] commands = { Command8, CommandSQR, CommandSQRT, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }
```

##### `TestHistoryStandardOrderOfOperations_10` — `HistoryTests` — `HistoryTests.cs:452-457`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_10()
        {
            int[] commands = { Command1, Command0, CommandADD, Command2, CommandSUB, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("12", "10   +   2 =", commands);
        }
```

##### `TestHistoryStandardOrderOfOperations_11` — `HistoryTests` — `HistoryTests.cs:459-464`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_11()
        {
            int[] commands = { Command3, CommandMUL, Command4, CommandDIV, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("12", "3   \x00D7   4 =", commands);
        }
```

##### `TestHistoryStandardOrderOfOperations_12` — `HistoryTests` — `HistoryTests.cs:466-471`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_12()
        {
            int[] commands = { Command6, CommandDIV, Command3, CommandSUB, CommandADD, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("2", "6   \x00F7   3 =", commands);
        }
```

##### `TestHistoryStandardOrderOfOperations_13` — `HistoryTests` — `HistoryTests.cs:473-478`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_13()
        {
            int[] commands = { Command7, CommandSUB, Command4, CommandDIV, CommandMUL, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("3", "7   -   4 =", commands);
        }
```

##### `TestHistoryStandardOrderOfOperations_14` — `HistoryTests` — `HistoryTests.cs:480-485`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_14()
        {
            int[] commands = { Command8, CommandMUL, Command2, CommandADD, CommandSQRT, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("16", "8   \x00D7   2 =", commands);
        }
```

##### `TestHistoryStandardOrderOfOperations_15` — `HistoryTests` — `HistoryTests.cs:487-492`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_15()
        {
            int[] commands = { Command9, CommandADD, Command0, CommandMUL, CommandSIGN, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("9", "9   +   0 =", commands);
        }
```

##### `TestHistoryStandardOrderOfOperations_16` — `HistoryTests` — `HistoryTests.cs:494-499`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_16()
        {
            int[] commands = { Command9, CommandSIGN, Command0, CommandADD, CommandMUL, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }
```

##### `TestHistoryStandardOrderOfOperations_17` — `HistoryTests` — `HistoryTests.cs:501-506`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_17()
        {
            int[] commands = { Command1, CommandADD, Command2, CommandEQU, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("3", "1   +   2 =", commands);
        }
```

##### `TestHistoryStandardOrderOfOperations_18` — `HistoryTests` — `HistoryTests.cs:508-513`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_18()
        {
            int[] commands = { Command2, Command0, CommandMUL, Command0, Command2, CommandEQU, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("40", "20   \x00D7   2 =", commands);
        }
```

##### `TestHistoryStandardOrderOfOperations_19` — `HistoryTests` — `HistoryTests.cs:515-520`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_19()
        {
            int[] commands = { Command1, CommandADD, Command2, CommandADD, CommandBACK, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("3", "1   +   2 =", commands);
        }
```

##### `TestHistoryStandardOrderOfOperations_20` — `HistoryTests` — `HistoryTests.cs:522-527`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_20()
        {
            int[] commands = { Command1, CommandADD, Command2, CommandADD, CommandCLEAR, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("3", "1   +   2 =", commands);
        }
```

##### `TestHistoryStandardOrderOfOperations_21` — `HistoryTests` — `HistoryTests.cs:529-534`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_21()
        {
            int[] commands = { Command1, CommandADD, Command2, CommandADD, CommandCENTR, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("3", "1   +   2 =", commands);
        }
```

##### `TestHistoryStandardOrderOfOperations_22` — `HistoryTests` — `HistoryTests.cs:536-541`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_22()
        {
            int[] commands = { Command1, CommandADD, Command2, CommandCLEAR, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }
```

##### `TestHistoryStandardOrderOfOperations_23` — `HistoryTests` — `HistoryTests.cs:543-548`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_23()
        {
            int[] commands = { Command1, CommandADD, Command2, CommandCENTR, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }
```

##### `TestHistoryStandardOrderOfOperationsMultiple` — `HistoryTests` — `HistoryTests.cs:550-577`

```csharp
        [TestMethod]
        public void TestHistoryStandardOrderOfOperationsMultiple()
        {
            Initialize();
            int[] commands = { Command1, CommandMUL, Command2, CommandMUL, Command3, CommandMUL, Command4, CommandMUL, Command5, CommandMUL, CommandNULL };
            int initialSize = m_historyViewModel.ItemsCount;
            foreach (int command in commands)
            {
                if (command == CommandNULL)
                    break;
                m_standardViewModel.SendCommandToCalcManager(command);
            }
            int sizeAfterCommandsAdd = m_historyViewModel.ItemsCount;
            Assert.AreEqual(initialSize + 4, sizeAfterCommandsAdd);
            var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[0];
            Assert.AreEqual("24   \x00D7   5 =", historyItem.Expression);
            Assert.AreEqual("120", historyItem.Result);
            historyItem = (HistoryItemViewModel)m_historyViewModel.Items[1];
            Assert.AreEqual("6   \x00D7   4 =", historyItem.Expression);
            Assert.AreEqual("24", historyItem.Result);
            historyItem = (HistoryItemViewModel)m_historyViewModel.Items[2];
            Assert.AreEqual("2   \x00D7   3 =", historyItem.Expression);
            Assert.AreEqual("6", historyItem.Result);
            historyItem = (HistoryItemViewModel)m_historyViewModel.Items[3];
            Assert.AreEqual("1   \x00D7   2 =", historyItem.Expression);
            Assert.AreEqual("2", historyItem.Result);
            Cleanup();
        }
```

#### 5.5.3 `StandardCalculatorViewModelTests` — memory semantics

##### `IsMemoryEmptyTest` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:831-841`

```csharp
        [TestMethod]
        public void IsMemoryEmptyTest()
        {
            var viewModel = new StandardCalculatorViewModel();
            viewModel.IsStandard = true;
            Assert.AreEqual(0, viewModel.MemorizedNumbers.Count);
            viewModel.OnMemoryButtonPressed();
            Assert.AreEqual(1, viewModel.MemorizedNumbers.Count);
            viewModel.ClearMemoryCommand.Execute(null);
            Assert.AreEqual(0, viewModel.MemorizedNumbers.Count);
        }
```

##### `OnMemoryButtonPressed` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:878-887`

```csharp
        [TestMethod]
        public void OnMemoryButtonPressed()
        {
            var viewModel = new StandardCalculatorViewModel();
            viewModel.IsStandard = true;
            viewModel.DisplayValue = "1001";
            viewModel.OnMemoryButtonPressed();
            viewModel.OnMemoryButtonPressed();
            Assert.AreEqual(2, viewModel.MemorizedNumbers.Count);
        }
```

##### `OnMemoryAddWhenMemoryEmpty` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:889-905`

```csharp
        [TestMethod]
        public void OnMemoryAddWhenMemoryEmpty()
        {
            _viewModel.IsStandard = true;
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryAdd(0);
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("1,001", _viewModel.DisplayValue);
        }
```

##### `OnMemorySubtractWhenMemoryEmpty` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:907-923`

```csharp
        [TestMethod]
        public void OnMemorySubtractWhenMemoryEmpty()
        {
            _viewModel.IsStandard = true;
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemorySubtract(0);
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("-1,001", _viewModel.DisplayValue);
        }
```

##### `OnNegativeEntryInMemory` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:925-953`

```csharp
        [TestMethod]
        public void OnNegativeEntryInMemory()
        {
            ChangeMode(_viewModel, 0 /*Standard*/);
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.Negate, "N/A", "N/A"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("-1,001", _viewModel.DisplayValue);

            var memorySlotStandard = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("-1,001", memorySlotStandard.Value);

            ChangeMode(_viewModel, 1 /*Scientific*/);
            var memorySlotScientific = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("-1,001", memorySlotScientific.Value);

            ChangeMode(_viewModel, 2 /*Programmer*/);
            var memorySlotProgrammer = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("-1,001", memorySlotProgrammer.Value);
        }
```

##### `OnDecimalEntryInMemory` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:955-984`

```csharp
        [TestMethod]
        public void OnDecimalEntryInMemory()
        {
            ChangeMode(_viewModel, 0 /*Standard*/);
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.Decimal, "1,001.", ""),
                new(NumbersAndOperatorsEnum.One, "1,001.1", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("1,001.1", _viewModel.DisplayValue);

            var memorySlotStandard = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("1,001.1", memorySlotStandard.Value);

            ChangeMode(_viewModel, 1 /*Scientific*/);
            var memorySlotScientific = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("1,001.1", memorySlotScientific.Value);

            ChangeMode(_viewModel, 2 /*Programmer*/);
            var memorySlotProgrammer = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("1,001", memorySlotProgrammer.Value);
        }
```

##### `OnNegativeDecimalInMemory` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:986-1005`

```csharp
        [TestMethod]
        public void OnNegativeDecimalInMemory()
        {
            _viewModel.IsStandard = true;
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.Decimal, "1,001.", ""),
                new(NumbersAndOperatorsEnum.One, "1,001.1", ""),
                new(NumbersAndOperatorsEnum.Negate, "N/A", "N/A"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("-1,001.1", _viewModel.DisplayValue);
        }
```

##### `OnDecimalAddedToMemory` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:1007-1038`

```csharp
        [TestMethod]
        public void OnDecimalAddedToMemory()
        {
            _viewModel.IsStandard = true;
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.Decimal, "1,001.", ""),
                new(NumbersAndOperatorsEnum.One, "1,001.1", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.Decimal, "1,001.", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, false);
            _viewModel.OnMemoryButtonPressed();
            _viewModel.OnMemoryAdd(1);
            _viewModel.OnMemoryItemPressed(1);
            Assert.AreEqual("2,002.1", _viewModel.DisplayValue);
        }
```

##### `OnMemorySavedInHexRadixAndSwitchedToStandardMode` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:1040-1059`

```csharp
        [TestMethod]
        public void OnMemorySavedInHexRadixAndSwitchedToStandardMode()
        {
            ChangeMode(_viewModel, 2 /*Programmer*/);
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.F, "FF", ""),
                new(NumbersAndOperatorsEnum.None, "FF", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();
            ChangeMode(_viewModel, 1 /*Scientific*/);
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("255", _viewModel.DisplayValue);

            var memorySlot = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("255", memorySlot.Value);
        }
```

##### `OnMemorySavedInHexRadixAndRadixChanges` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:1061-1086`

```csharp
        [TestMethod]
        public void OnMemorySavedInHexRadixAndRadixChanges()
        {
            ChangeMode(_viewModel, 2 /*Programmer*/);
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.F, "FF", ""),
                new(NumbersAndOperatorsEnum.None, "FF", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();

            _viewModel.SwitchProgrammerModeBase(NumberBase.OctBase);
            var memorySlotOct = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("377", memorySlotOct.Value);

            _viewModel.SwitchProgrammerModeBase(NumberBase.DecBase);
            var memorySlotDec = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("255", memorySlotDec.Value);

            _viewModel.SwitchProgrammerModeBase(NumberBase.BinBase);
            var memorySlotBin = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("1111 1111", memorySlotBin.Value);
        }
```

##### `OnMemoryButtonPressedMaxTimes` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:1088-1099`

```csharp
        [TestMethod]
        public void OnMemoryButtonPressedMaxTimes()
        {
            var viewModel = new StandardCalculatorViewModel();
            viewModel.IsStandard = true;
            viewModel.DisplayValue = "1001";
            for (int i = 0; i < 110; i++)
            {
                viewModel.OnMemoryButtonPressed();
            }
            Assert.AreEqual(100, viewModel.MemorizedNumbers.Count);
        }
```

##### `OnMemoryItemPressed` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:1101-1128`

```csharp
        [TestMethod]
        public void OnMemoryItemPressed()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Two, "2", ""),
                new(NumbersAndOperatorsEnum.Zero, "20", ""),
                new(NumbersAndOperatorsEnum.Zero, "200", ""),
                new(NumbersAndOperatorsEnum.Two, "2,002", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, false);
            _viewModel.OnMemoryButtonPressed();
            _viewModel.OnMemoryItemPressed(1);

            Assert.AreEqual("1,001", _viewModel.DisplayValue);
        }
```

##### `OnMemoryItemPressedNoMemory` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:1130-1143`

```csharp
        [TestMethod]
        public void OnMemoryItemPressedNoMemory()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Two, "12", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("12", _viewModel.DisplayValue);
            Assert.AreEqual(0, _viewModel.MemorizedNumbers.Count);
        }
```

##### `OnMemoryItemAddAndSubtract` — `StandardCalculatorViewModelTests` — `StandardCalculatorViewModelTests.cs:1145-1174`

```csharp
        [TestMethod]
        public void OnMemoryItemAddAndSubtract()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Two, "2", ""),
                new(NumbersAndOperatorsEnum.Zero, "20", ""),
                new(NumbersAndOperatorsEnum.Zero, "200", ""),
                new(NumbersAndOperatorsEnum.Two, "2,002", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, false);
            _viewModel.OnMemoryButtonPressed();
            _viewModel.OnMemoryItemPressed(1);
            _viewModel.OnMemoryAdd(0);

            var memorySlot = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("3,003", memorySlot.Value);
        }
```

#### 5.5.4 `MultiWindowTests` — history/memory isolation

##### `MultipleModesHistoryAddItemTest` — `MultiWindowUnitTests` — `MultiWindowTests.cs:433-481`

```csharp
        [TestMethod]
        public void MultipleModesHistoryAddItemTest()
        {
            var viewModels = new StandardCalculatorViewModel[2];

            for (int i = 0; i < 2; i++)
            {
                viewModels[i] = new StandardCalculatorViewModel();
                ChangeMode(viewModels[i], i);
                Assert.AreEqual(0, viewModels[i].HistoryVM.ItemsCount);
            }

            // Standard Mode: 1+2=3
            var standardItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Equals, "3", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[0], standardItems, true);

            // Scientific Mode: 1+2*3=7
            var scientificItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Multiply, "2", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Equals, "7", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[1], scientificItems, true);

            // Assert history for Standard instance
            Assert.AreEqual(1, viewModels[0].HistoryVM.ItemsCount);
            var item1 = (HistoryItemViewModel)viewModels[0].HistoryVM.Items[0];
            Assert.AreEqual("1   +   2 =", item1.Expression);
            Assert.AreEqual("3", item1.Result);

            // Assert history for Scientific instance
            Assert.AreEqual(1, viewModels[1].HistoryVM.ItemsCount);
            var item2 = (HistoryItemViewModel)viewModels[1].HistoryVM.Items[0];
            Assert.AreEqual("1   +   2   \u00D7   3 =", item2.Expression);
            Assert.AreEqual("7", item2.Result);
        }
```

##### `MultipleStandardModesHistoryAddItemTest` — `MultiWindowUnitTests` — `MultiWindowTests.cs:483-544`

```csharp
        [TestMethod]
        public void MultipleStandardModesHistoryAddItemTest()
        {
            var viewModels = new StandardCalculatorViewModel[2];

            for (int i = 0; i < 2; i++)
            {
                viewModels[i] = new StandardCalculatorViewModel();
                ChangeMode(viewModels[i], 0);
                Assert.AreEqual(0, viewModels[i].HistoryVM.ItemsCount);
            }

            // Instance 0: 3+2=5, then 5*1=5
            var items0 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsStandardMode, "0", ""),
                new(NumbersAndOperatorsEnum.Three, "3", ""),
                new(NumbersAndOperatorsEnum.Add, "3", "3 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "3 + "),
                new(NumbersAndOperatorsEnum.Multiply, "5", "5 * "),
                new(NumbersAndOperatorsEnum.One, "1", "5 * "),
                new(NumbersAndOperatorsEnum.Equals, "5", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[0], items0, true);

            // Instance 1: 1+2=3, then 3*3=9
            var items1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsStandardMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Multiply, "3", "3 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "3 * "),
                new(NumbersAndOperatorsEnum.Equals, "9", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[1], items1, true);

            // Verify history items
            string[,] expressions = {
                { "5   \u00D7   1 =", "3   +   2 =" },
                { "3   \u00D7   3 =", "1   +   2 =" }
            };
            string[,] results = {
                { "5", "5" },
                { "9", "3" }
            };

            for (int i = 0; i < 2; i++)
            {
                Assert.AreEqual(2, viewModels[i].HistoryVM.ItemsCount);

                for (int j = 0; j < 2; j++)
                {
                    var item = (HistoryItemViewModel)viewModels[i].HistoryVM.Items[j];
                    Assert.AreEqual(expressions[i, j], item.Expression);
                    Assert.AreEqual(results[i, j], item.Result);
                }
            }
        }
```

##### `MultipleScientificModesHistoryAddItemTest` — `MultiWindowUnitTests` — `MultiWindowTests.cs:546-594`

```csharp
        [TestMethod]
        public void MultipleScientificModesHistoryAddItemTest()
        {
            var viewModels = new StandardCalculatorViewModel[2];

            for (int i = 0; i < 2; i++)
            {
                viewModels[i] = new StandardCalculatorViewModel();
                ChangeMode(viewModels[i], 1);
                Assert.AreEqual(0, viewModels[i].HistoryVM.ItemsCount);
            }

            // Instance 0: 1+2=3
            var items0 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Equals, "3", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[0], items0, true);

            // Instance 1: 1+2*3=7
            var items1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Multiply, "2", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Equals, "7", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[1], items1, true);

            string[] expectedExpressions = { "1   +   2 =", "1   +   2   \u00D7   3 =" };
            string[] expectedResults = { "3", "7" };

            for (int i = 0; i < 2; i++)
            {
                Assert.AreEqual(1, viewModels[i].HistoryVM.ItemsCount);
                var item = (HistoryItemViewModel)viewModels[i].HistoryVM.Items[0];
                Assert.AreEqual(expectedExpressions[i], item.Expression);
                Assert.AreEqual(expectedResults[i], item.Result);
            }
        }
```

##### `MultipleModesMemoryAddItemTest` — `MultiWindowUnitTests` — `MultiWindowTests.cs:596-656`

```csharp
        [TestMethod]
        public void MultipleModesMemoryAddItemTest()
        {
            var viewModels = new StandardCalculatorViewModel[3];

            for (int i = 0; i < 3; i++)
            {
                viewModels[i] = new StandardCalculatorViewModel();
                ChangeMode(viewModels[i], i);
                Assert.AreEqual(0, viewModels[i].MemorizedNumbers.Count);
            }

            // Standard Mode: 1+2=3
            var standardItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Equals, "3", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[0], standardItems, true);

            // Scientific Mode: 1+2*3=7
            var scientificItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Multiply, "2", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Equals, "7", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[1], scientificItems, true);

            // Programmer Mode: Hex F
            var programmerItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[2], programmerItems, false);

            // Press Memory button to save values
            for (int i = 0; i < 3; i++)
            {
                viewModels[i].ButtonPressed.Execute(NumbersAndOperatorsEnum.Memory);
            }

            string[] expectedMemoryValues = { "3", "7", "F" };

            for (int i = 0; i < 3; i++)
            {
                Assert.AreEqual(1, viewModels[i].MemorizedNumbers.Count);
                var memorySlot = viewModels[i].MemorizedNumbers[0];
                Assert.AreEqual(expectedMemoryValues[i], memorySlot.Value);
            }
        }
```

### 5.6 Max-digits / precision-threshold vectors

The reference's own digit budgets are declared in `src/CalculatorUnitTests/Helpers.h`:
`StandardModePrecision 16`, `ScientificModePrecision 32`, `ProgrammerModePrecision 64`.
These tests pin the exact inputs at which `MaxDigitsReached()` must fire, and must not fire.

#### 5.6.1 `CalculatorManagerTest` — `MaxDigitsReached` (standard mode)

##### `CalculatorManagerTestMaxDigitsReached` — `CalculatorManagerTest` — `CalculatorManagerTest.cpp:963-966`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestMaxDigitsReached()
    {
        TestMaxDigitsReachedScenario(L"1,234,567,891,011,1213");
    }
```

##### `CalculatorManagerTestMaxDigitsReached_LeadingDecimal` — `CalculatorManagerTest` — `CalculatorManagerTest.cpp:968-971`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestMaxDigitsReached_LeadingDecimal()
    {
        TestMaxDigitsReachedScenario(L"0.12345678910111213");
    }
```

##### `CalculatorManagerTestMaxDigitsReached_TrailingDecimal` — `CalculatorManagerTest` — `CalculatorManagerTest.cpp:973-976`

```cpp
    void CalculatorManagerTest::CalculatorManagerTestMaxDigitsReached_TrailingDecimal()
    {
        TestMaxDigitsReachedScenario(L"123,456,789,101,112.13");
    }
```

#### 5.6.2 `UnitConverterTest` — `MaxDigitsReached` (converter)

##### `UnitConverterTestMaxDigitsReached` — `UnitConverterTest` — `UnitConverterTest.cpp:498-521`

```cpp
    void UnitConverterTest::UnitConverterTestMaxDigitsReached()
    {
        ExecuteCommands({ Command::One,
                          Command::Two,
                          Command::Three,
                          Command::Four,
                          Command::Five,
                          Command::Six,
                          Command::Seven,
                          Command::Eight,
                          Command::Nine,
                          Command::One,
                          Command::Zero,
                          Command::One,
                          Command::One,
                          Command::One,
                          Command::Two });

        VERIFY_ARE_EQUAL(0, s_testVMCallback->GetMaxDigitsReachedCallCount());

        ExecuteCommands({ Command::One });

        VERIFY_ARE_EQUAL(1, s_testVMCallback->GetMaxDigitsReachedCallCount());
    }
```

##### `UnitConverterTestMaxDigitsReached_LeadingDecimal` — `UnitConverterTest` — `UnitConverterTest.cpp:523-547`

```cpp
    void UnitConverterTest::UnitConverterTestMaxDigitsReached_LeadingDecimal()
    {
        ExecuteCommands({ Command::Zero,
                          Command::Decimal,
                          Command::One,
                          Command::Two,
                          Command::Three,
                          Command::Four,
                          Command::Five,
                          Command::Six,
                          Command::Seven,
                          Command::Eight,
                          Command::Nine,
                          Command::One,
                          Command::Zero,
                          Command::One,
                          Command::One,
                          Command::One });

        VERIFY_ARE_EQUAL(0, s_testVMCallback->GetMaxDigitsReachedCallCount());

        ExecuteCommands({ Command::Two });

        VERIFY_ARE_EQUAL(1, s_testVMCallback->GetMaxDigitsReachedCallCount());
    }
```

##### `UnitConverterTestMaxDigitsReached_TrailingDecimal` — `UnitConverterTest` — `UnitConverterTest.cpp:549-573`

```cpp
    void UnitConverterTest::UnitConverterTestMaxDigitsReached_TrailingDecimal()
    {
        ExecuteCommands({ Command::One,
                          Command::Two,
                          Command::Three,
                          Command::Four,
                          Command::Five,
                          Command::Six,
                          Command::Seven,
                          Command::Eight,
                          Command::Nine,
                          Command::One,
                          Command::Zero,
                          Command::One,
                          Command::One,
                          Command::One,
                          Command::Two,
                          Command::Decimal });

        VERIFY_ARE_EQUAL(0, s_testVMCallback->GetMaxDigitsReachedCallCount());

        ExecuteCommands({ Command::One });

        VERIFY_ARE_EQUAL(1, s_testVMCallback->GetMaxDigitsReachedCallCount());
    }
```

##### `UnitConverterTestMaxDigitsReached_MultipleTimes` — `UnitConverterTest` — `UnitConverterTest.cpp:575-601`

```cpp
    void UnitConverterTest::UnitConverterTestMaxDigitsReached_MultipleTimes()
    {
        ExecuteCommands({ Command::One,
                          Command::Two,
                          Command::Three,
                          Command::Four,
                          Command::Five,
                          Command::Six,
                          Command::Seven,
                          Command::Eight,
                          Command::Nine,
                          Command::One,
                          Command::Zero,
                          Command::One,
                          Command::One,
                          Command::One,
                          Command::Two });

        VERIFY_ARE_EQUAL(0, s_testVMCallback->GetMaxDigitsReachedCallCount());

        for (auto count = 1; count <= 10; count++)
        {
            ExecuteCommands({ Command::Three });

            VERIFY_ARE_EQUAL(count, s_testVMCallback->GetMaxDigitsReachedCallCount(), to_wstring(count).c_str());
        }
    }
```

#### 5.6.3 `CalcInputTest::TryAddDigitMaxCount` — explicit `maxDigits` argument

##### `TryAddDigitMaxCount` — `CalcInputTest` — `CalcInputTest.cpp:88-106`

```cpp
        TEST_METHOD(TryAddDigitMaxCount)
        {
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32), L"Verify TryAddDigit for base with length < maxDigits succeeds.");
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify adding digit for base with length < maxDigits succeeded.");
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 1), L"Verify TryAddDigit for base with length > maxDigits fails.");
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify digit for base was not added.");
            m_calcInput.TryBeginExponent();
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32), L"Verify TryAddDigit for exponent with length < maxDigits succeeds.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32), L"Verify TryAddDigit for exponent with length < maxDigits succeeds.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(3, 10, false, L"999", 64, 32), L"Verify TryAddDigit for exponent with length < maxDigits succeeds.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(4, 10, false, L"999", 64, 32), L"Verify TryAddDigit for exponent with length < maxDigits succeeds.");
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(5, 10, false, L"999", 64, 32), L"Verify TryAddDigit for exponent with length > maxDigits fails.");
            VERIFY_ARE_EQUAL(L"1.e+1234", m_calcInput.ToString(10), L"Verify adding digits for exponent with length < maxDigits succeeded.");

            m_calcInput.Clear();
            m_calcInput.TryAddDecimalPt();
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 1), L"Verify decimal point and leading zero does not count toward maxDigits.");
            VERIFY_ARE_EQUAL(L"0.1", m_calcInput.ToString(10), L"Verify input value checking dec pt and leading zero impact on maxDigits.");
        }
```

#### 5.6.4 Managed announcement / view-model surface

##### `TestGetMaxDigitsReachedAnnouncement` — `NarratorAnnouncementTests` — `NarratorAnnouncementTests.cs:25-33`

```csharp
        [TestMethod]
        public void TestGetMaxDigitsReachedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetMaxDigitsReachedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("MaxDigitsReached", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.Other, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }
```

##### `MaxDigitsAnnouncementIncludesTheConversionResult` — `UnitConverterViewModelTests` — `UnitConverterViewModelTests.cs:176-189`

```csharp
        [TestMethod]
        public void MaxDigitsAnnouncementIncludesTheConversionResult()
        {
            var viewModel = new UnitConverterViewModel();
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Five);
            viewModel.OnMaxDigitsReached();

            var announcement = viewModel.Announcement?.Announcement;

            Assert.IsFalse(string.IsNullOrEmpty(announcement));
            Assert.IsFalse(
                announcement.Contains("%1"),
                $"The format placeholder was never substituted: '{announcement}'.");
        }
```

---

## 6. UI functional tests — `src/CalculatorUITests/` (test class & method index)

MSTest 4.2.2 + Appium/WebDriver 4.4.5, `net10.0`. These drive the **shipped WinUI 3 app** through
WinAppDriver, so they depend on a running, installed, focused Calculator window and a real UI thread.
They are the only tests that exercise rendering, focus, narrator text, window/AoT chrome and clipboard
at the OS level. Method **names only** here, per the brief; the vectors that matter for a headless port
are the literal display/expression strings asserted inside them, which are reproduced in §5 and §3/§4.

### 6.1 `[TestClass] StandardModeFunctionalTests` — `StandardModeFunctionalTests.cs` (38 tests)

Standard-mode keypad/operator UI driven end-to-end via WinAppDriver.
* **Mouse input**: digit + operator sequences, `+/-`, `C`/`CE`, `1/x`, `x^2`, `%`, `sqrt`, backspace, decimal, negate.
* **Keyboard input**: same sequences via hardware keys, plus hotkeys to switch modes and memory/history hotkeys.
* **Mixed input**: each operator exercised by mouse+keyboard+paste combinations (addition, subtraction, multiplication,
  division, reciprocal, square, square root, percent add/sub, percent mul/div, +/-, decimal, `=`, delete,
  `CE`/`C`, and deleting a decimal point without prefixing zeros).
* **Always-on-top (AoT)**: enter/exit, tooltip, no memory function, history function, standard-only button,
  error messages (`Result is undefined`, `Cannot divide by zero`, message retention on exit).
* **Error states**: divide-by-zero, square root of a negative number.
* These assert both the **result text** and the **expression text** (e.g. `"20 ÷ 6="`), so they double as
  display-format golden vectors.

| # | Test method | Source lines |
|---|---|---|
| 1 | `MouseInput_AddSubtractClearClearEntryTwoThree` | `StandardModeFunctionalTests.cs:72-93` |
| 2 | `MouseInput_MultiplyDivideEqualFourFiveSix` | `StandardModeFunctionalTests.cs:95-114` |
| 3 | `MouseInput_InvertSquaredSevenEightNine` | `StandardModeFunctionalTests.cs:116-135` |
| 4 | `MouseInput_PercentSquareRootBackspaceDecimalNegateOneZero` | `StandardModeFunctionalTests.cs:137-162` |
| 5 | `MouseInput_HistoryButtons` | `StandardModeFunctionalTests.cs:164-190` |
| 6 | `MouseInput_MemoryButtons` | `StandardModeFunctionalTests.cs:192-213` |
| 7 | `KeyboardInput_HotkeysToChangeToBasicModes` | `StandardModeFunctionalTests.cs:226-246` |
| 8 | `KeyboardInput_AddSubtractClearClearEntryTwoThree` | `StandardModeFunctionalTests.cs:248-272` |
| 9 | `KeyboardInput_MultiplyDivideEqualFourFiveSix` | `StandardModeFunctionalTests.cs:274-294` |
| 10 | `KeyboardInput_InvertSquaredSevenEightNine` | `StandardModeFunctionalTests.cs:296-317` |
| 11 | `KeyboardInput_PercentSquareRootBackspaceDecimalNegateOneZero` | `StandardModeFunctionalTests.cs:319-346` |
| 12 | `KeyboardInput_MemoryHotkeys` | `StandardModeFunctionalTests.cs:348-371` |
| 13 | `KeyboardInput_HistoryHotkeys` | `StandardModeFunctionalTests.cs:373-402` |
| 14 | `MixedInput_Operators_Addition` | `StandardModeFunctionalTests.cs:415-428` |
| 15 | `MixedInput_Operators_Subtraction` | `StandardModeFunctionalTests.cs:430-442` |
| 16 | `MixedInput_Operators_Multiplication` | `StandardModeFunctionalTests.cs:444-456` |
| 17 | `MixedInput_Operators_Division` | `StandardModeFunctionalTests.cs:458-470` |
| 18 | `MixedInput_Operators_Reciprocal` | `StandardModeFunctionalTests.cs:472-482` |
| 19 | `MixedInput_Operators_Square` | `StandardModeFunctionalTests.cs:484-495` |
| 20 | `MixedInput_Operators_SquareRoot` | `StandardModeFunctionalTests.cs:497-508` |
| 21 | `MixedInput_Operators_PercentAdditionSubtraction` | `StandardModeFunctionalTests.cs:510-523` |
| 22 | `MixedInput_Operators_PercentMultiplicationDivision` | `StandardModeFunctionalTests.cs:525-538` |
| 23 | `MixedInput_Operators_PositiveNegative` | `StandardModeFunctionalTests.cs:540-553` |
| 24 | `MixedInput_Operators_Decimal` | `StandardModeFunctionalTests.cs:555-567` |
| 25 | `MixedInput_Operators_Equal` | `StandardModeFunctionalTests.cs:569-585` |
| 26 | `MixedInput_Operators_Delete` | `StandardModeFunctionalTests.cs:587-599` |
| 27 | `MixedInput_Operators_ClearEntryClear` | `StandardModeFunctionalTests.cs:601-627` |
| 28 | `MixedInput_Operators_DeletingDecimalDoesNotPrefixZeros` | `StandardModeFunctionalTests.cs:630-642` |
| 29 | `AoT_EnterExitKeepOnTop` | `StandardModeFunctionalTests.cs:663-669` |
| 30 | `AoT_Tootip` | `StandardModeFunctionalTests.cs:671-678` |
| 31 | `AoT_NoMemoryFunction` | `StandardModeFunctionalTests.cs:680-694` |
| 32 | `AoT_HistoryFunction` | `StandardModeFunctionalTests.cs:696-718` |
| 33 | `AoT_ButtonOnlyInStandard` | `StandardModeFunctionalTests.cs:720-742` |
| 34 | `AoT_ErrorMessage_ResultUndefined` | `StandardModeFunctionalTests.cs:744-755` |
| 35 | `AoT_ErrorMessage_CannotDivideByZero` | `StandardModeFunctionalTests.cs:757-768` |
| 36 | `AoT_ErrorMessage_MessageRetentionUponExitingAoT` | `StandardModeFunctionalTests.cs:770-785` |
| 37 | `Error_DivideByZero` | `StandardModeFunctionalTests.cs:795-801` |
| 38 | `Error_ErrorSquareRootNegativeNumber` | `StandardModeFunctionalTests.cs:803-811` |

Lifecycle: `[ClassInitialize] ClassInitialize` / `[ClassCleanup] ClassCleanup` / `[TestInitialize] TestInit` / `[TestCleanup] TestCleanup` — each resets the calculator to a known state before every test.

### 6.2 `[TestClass] ScientificModeFunctionalTests` — `ScientificModeFunctionalTests.cs` (45 tests)

Scientific-mode UI.
* **Smoke tests**: cube (`x^3`), `sin`, `tanh`, `inv cos`, `floor`, parentheses (4 sub-cases incl. `0.5` and `.5`
  both yielding `(8) × 0.5=`), close-parenthesis, radian/gradian angle operators, fixed→exponential toggle.
* **Operators**: `x^y`, `10^x`, `log`, `ln`, `|x|`, `e^x`, `mod`, `n!` (factorial), `ceil`, `rand`, `dms`, `deg`.
* **Trig**: `cos`, `tan`, `sec`, `csc`, `cot`, all six inverses, all six hyperbolic and all six inverse-hyperbolic.
* **Display**: `FixedToExponentialResetsOnClear` and `FixedToExponentialResetsOnClearEntry`.
* Every operator test asserts the resulting **expression string** and the **history item value/expression**,
  making these strong formatting + rounding vectors.

| # | Test method | Source lines |
|---|---|---|
| 1 | `SmokeTest_Cube` | `ScientificModeFunctionalTests.cs:66-75` |
| 2 | `SmokeTest_Sin` | `ScientificModeFunctionalTests.cs:77-86` |
| 3 | `SmokeTest_Tanh` | `ScientificModeFunctionalTests.cs:88-98` |
| 4 | `SmokeTest_InvCos` | `ScientificModeFunctionalTests.cs:100-110` |
| 5 | `SmokeTest_Floor` | `ScientificModeFunctionalTests.cs:112-121` |
| 6 | `SmokeTest_Parentheses` | `ScientificModeFunctionalTests.cs:123-136` |
| 7 | `SmokeTest_CloseParenthesis` | `ScientificModeFunctionalTests.cs:138-211` |
| 8 | `SmokeTest_RadianAngleOperator` | `ScientificModeFunctionalTests.cs:213-224` |
| 9 | `SmokeTest_GradianAngleOperator` | `ScientificModeFunctionalTests.cs:226-237` |
| 10 | `SmokeTest_FixedToExponential` | `ScientificModeFunctionalTests.cs:239-246` |
| 11 | `Operator_XPowerY` | `ScientificModeFunctionalTests.cs:250-259` |
| 12 | `Operator_PowerOf10Button` | `ScientificModeFunctionalTests.cs:261-269` |
| 13 | `Operator_LogButton` | `ScientificModeFunctionalTests.cs:271-279` |
| 14 | `Operator_LnButton` | `ScientificModeFunctionalTests.cs:281-289` |
| 15 | `Operator_AbsButton` | `ScientificModeFunctionalTests.cs:291-300` |
| 16 | `Operator_ExpButton` | `ScientificModeFunctionalTests.cs:302-311` |
| 17 | `Operator_ModButton` | `ScientificModeFunctionalTests.cs:313-322` |
| 18 | `Operator_FactorialButton` | `ScientificModeFunctionalTests.cs:324-332` |
| 19 | `Operator_CeilingButton` | `ScientificModeFunctionalTests.cs:334-343` |
| 20 | `Operator_RandomButton` | `ScientificModeFunctionalTests.cs:345-353` |
| 21 | `Operator_DmsButton` | `ScientificModeFunctionalTests.cs:355-364` |
| 22 | `Operator_DegreesButton` | `ScientificModeFunctionalTests.cs:366-375` |
| 23 | `Trig_CosButton` | `ScientificModeFunctionalTests.cs:379-390` |
| 24 | `Trig_TanButton` | `ScientificModeFunctionalTests.cs:392-404` |
| 25 | `Trig_SecButton` | `ScientificModeFunctionalTests.cs:406-418` |
| 26 | `Trig_CscButton` | `ScientificModeFunctionalTests.cs:420-432` |
| 27 | `Trig_CotButton` | `ScientificModeFunctionalTests.cs:434-446` |
| 28 | `Trig_InvSinButton` | `ScientificModeFunctionalTests.cs:448-461` |
| 29 | `Trig_InvTanButton` | `ScientificModeFunctionalTests.cs:463-476` |
| 30 | `Trig_InvSecButton` | `ScientificModeFunctionalTests.cs:478-492` |
| 31 | `Trig_InvCscButton` | `ScientificModeFunctionalTests.cs:494-507` |
| 32 | `Trig_InvCotButton` | `ScientificModeFunctionalTests.cs:509-522` |
| 33 | `Trig_SinhButton` | `ScientificModeFunctionalTests.cs:524-537` |
| 34 | `Trig_CoshButton` | `ScientificModeFunctionalTests.cs:539-552` |
| 35 | `Trig_SechButton` | `ScientificModeFunctionalTests.cs:554-567` |
| 36 | `Trig_CschButton` | `ScientificModeFunctionalTests.cs:569-582` |
| 37 | `Trig_CothButton` | `ScientificModeFunctionalTests.cs:584-597` |
| 38 | `Trig_InvSinhButton` | `ScientificModeFunctionalTests.cs:599-613` |
| 39 | `Trig_InvCoshButton` | `ScientificModeFunctionalTests.cs:615-629` |
| 40 | `Trig_InvTanhButton` | `ScientificModeFunctionalTests.cs:631-645` |
| 41 | `Trig_InvSechButton` | `ScientificModeFunctionalTests.cs:647-661` |
| 42 | `Trig_InvCschButton` | `ScientificModeFunctionalTests.cs:663-677` |
| 43 | `Trig_InvCothButton` | `ScientificModeFunctionalTests.cs:679-693` |
| 44 | `FixedToExponentialResetsOnClear` | `ScientificModeFunctionalTests.cs:703-730` |
| 45 | `FixedToExponentialResetsOnClearEntry` | `ScientificModeFunctionalTests.cs:737-756` |

Lifecycle: `[ClassInitialize] ClassInitialize` / `[ClassCleanup] ClassCleanup` / `[TestInitialize] TestInit` / `[TestCleanup] TestCleanup` — each resets the calculator to a known state before every test.

### 6.3 `[TestClass] ProgrammerModeFunctionalTests` — `ProgrammerModeFunctionalTests.cs` (60 tests)

Programmer-mode UI (binary/octal/decimal/hex).
* **Smoke tests**: decimal left/right shift, `&`, `nand`, `|`, `nor`, `~` (not), `^` (xor); word size (QWORD/DWORD/WORD/BYTE);
  bit-flip keypad.
* **Arithmetic operators** (`<<`, `>>`, `&`, `nand`, `|`, `nor`, `~`, `^`) repeated across all four radices:
  Octal, Binary, Hex, Decimal.
* **Logical operators** left/right shift across all four radices.
* **Circular** left/right shift across all four radices.
* **Through-carry** left/right shift across all four radices.
* **Clipboard**: copy a simple number, copy an invalid number.
* These assert the displayed result **per radix**, so they pin the decimal→bin/oct/hex conversion and
  64/32/16-bit truncation semantics.

| # | Test method | Source lines |
|---|---|---|
| 1 | `SmokeTest_Arithmetic_Decimal_LeftShift` | `ProgrammerModeFunctionalTests.cs:72-81` |
| 2 | `SmokeTest_Arithmetic_Decimal_RightShift` | `ProgrammerModeFunctionalTests.cs:83-93` |
| 3 | `SmokeTest_Arithmetic_Decimal_And` | `ProgrammerModeFunctionalTests.cs:95-105` |
| 4 | `SmokeTest_Arithmetic_Decimal_Nand` | `ProgrammerModeFunctionalTests.cs:107-117` |
| 5 | `SmokeTest_Arithmetic_Decimal_Or` | `ProgrammerModeFunctionalTests.cs:119-129` |
| 6 | `SmokeTest_Arithmetic_Decimal_Nor` | `ProgrammerModeFunctionalTests.cs:131-141` |
| 7 | `SmokeTest_Arithmetic_Decimal_Not` | `ProgrammerModeFunctionalTests.cs:143-152` |
| 8 | `SmokeTest_Arithmetic_Decimal_Xor` | `ProgrammerModeFunctionalTests.cs:154-164` |
| 9 | `SmokeTest_WordSize` | `ProgrammerModeFunctionalTests.cs:169-211` |
| 10 | `SmokeTest_BitFlipKeypad` | `ProgrammerModeFunctionalTests.cs:217-225` |
| 11 | `Arithmetic_Operator_Octal_LeftShift` | `ProgrammerModeFunctionalTests.cs:232-242` |
| 12 | `Arithmetic_Operator_Octal_RightShift` | `ProgrammerModeFunctionalTests.cs:244-255` |
| 13 | `Arithmetic_Operator_Octal_And` | `ProgrammerModeFunctionalTests.cs:257-268` |
| 14 | `Arithmetic_Operator_Octal_Nand` | `ProgrammerModeFunctionalTests.cs:270-281` |
| 15 | `Arithmetic_Operator_Octal_Or` | `ProgrammerModeFunctionalTests.cs:283-294` |
| 16 | `Arithmetic_Operator_Octal_Nor` | `ProgrammerModeFunctionalTests.cs:296-307` |
| 17 | `Arithmetic_Operator_Octal_Not` | `ProgrammerModeFunctionalTests.cs:309-319` |
| 18 | `Arithmetic_Operator_Octal_Xor` | `ProgrammerModeFunctionalTests.cs:321-332` |
| 19 | `Arithmetic_Operator_Binary_LeftShift` | `ProgrammerModeFunctionalTests.cs:337-347` |
| 20 | `Arithmetic_Operator_Binary_RightShift` | `ProgrammerModeFunctionalTests.cs:349-359` |
| 21 | `Arithmetic_Operator_Binary_And` | `ProgrammerModeFunctionalTests.cs:361-372` |
| 22 | `Arithmetic_Operator_Binary_Nand` | `ProgrammerModeFunctionalTests.cs:374-385` |
| 23 | `Arithmetic_Operator_Binary_Or` | `ProgrammerModeFunctionalTests.cs:387-398` |
| 24 | `Arithmetic_Operator_Binary_Nor` | `ProgrammerModeFunctionalTests.cs:400-411` |
| 25 | `Arithmetic_Operator_Binary_Not` | `ProgrammerModeFunctionalTests.cs:413-423` |
| 26 | `Arithmetic_Operator_Binary_Xor` | `ProgrammerModeFunctionalTests.cs:425-436` |
| 27 | `Arithmetic_Operator_Hex_LeftShift` | `ProgrammerModeFunctionalTests.cs:441-451` |
| 28 | `Arithmetic_Operator_Hex_RightShift` | `ProgrammerModeFunctionalTests.cs:453-463` |
| 29 | `Arithmetic_Operator_Hex_And` | `ProgrammerModeFunctionalTests.cs:465-478` |
| 30 | `Arithmetic_Operator_Hex_Nand` | `ProgrammerModeFunctionalTests.cs:480-493` |
| 31 | `Arithmetic_Operator_Hex_Or` | `ProgrammerModeFunctionalTests.cs:495-508` |
| 32 | `Arithmetic_Operator_Hex_Nor` | `ProgrammerModeFunctionalTests.cs:510-523` |
| 33 | `Arithmetic_Operator_Hex_Not` | `ProgrammerModeFunctionalTests.cs:525-537` |
| 34 | `Arithmetic_Operator_Hex_Xor` | `ProgrammerModeFunctionalTests.cs:539-552` |
| 35 | `Logical_Operator_Decimal_LeftShift` | `ProgrammerModeFunctionalTests.cs:559-570` |
| 36 | `Logical_Operator_Decimal_RightShift` | `ProgrammerModeFunctionalTests.cs:572-584` |
| 37 | `Logical_Operator_Octal_LeftShift` | `ProgrammerModeFunctionalTests.cs:589-600` |
| 38 | `Logical_Operator_Octal_RightShift` | `ProgrammerModeFunctionalTests.cs:602-614` |
| 39 | `Logical_Operator_Binary_LeftShift` | `ProgrammerModeFunctionalTests.cs:619-630` |
| 40 | `Logical_Operator_Binary_RightShift` | `ProgrammerModeFunctionalTests.cs:632-644` |
| 41 | `Logical_Operator_Hex_LeftShift` | `ProgrammerModeFunctionalTests.cs:649-660` |
| 42 | `Logical_Operator_Hex_RightShift` | `ProgrammerModeFunctionalTests.cs:662-674` |
| 43 | `Circular_Operator_Decimal_LeftShift` | `ProgrammerModeFunctionalTests.cs:681-690` |
| 44 | `Circular_Operator_Decimal_RightShift` | `ProgrammerModeFunctionalTests.cs:692-702` |
| 45 | `Circular_Operator_Octal_LeftShift` | `ProgrammerModeFunctionalTests.cs:707-717` |
| 46 | `Circular_Operator_Octal_RightShift` | `ProgrammerModeFunctionalTests.cs:719-729` |
| 47 | `Circular_Operator_Binary_LeftShift` | `ProgrammerModeFunctionalTests.cs:734-744` |
| 48 | `Circular_Operator_Binary_RightShift` | `ProgrammerModeFunctionalTests.cs:746-756` |
| 49 | `Circular_Operator_Hex_LeftShift` | `ProgrammerModeFunctionalTests.cs:761-771` |
| 50 | `Circular_Operator_Hex_RightShift` | `ProgrammerModeFunctionalTests.cs:773-783` |
| 51 | `ThroughCarry_Operator_Decimal_LeftShift` | `ProgrammerModeFunctionalTests.cs:790-800` |
| 52 | `ThroughCarry_Operator_Decimal_RightShift` | `ProgrammerModeFunctionalTests.cs:802-811` |
| 53 | `ThroughCarry_Operator_Octal_LeftShift` | `ProgrammerModeFunctionalTests.cs:816-826` |
| 54 | `ThroughCarry_Operator_Octal_RightShift` | `ProgrammerModeFunctionalTests.cs:828-838` |
| 55 | `ThroughCarry_Operator_Binary_LeftShift` | `ProgrammerModeFunctionalTests.cs:843-853` |
| 56 | `ThroughCarry_Operator_Binary_RightShift` | `ProgrammerModeFunctionalTests.cs:855-865` |
| 57 | `ThroughCarry_Operator_Hex_LeftShift` | `ProgrammerModeFunctionalTests.cs:870-880` |
| 58 | `ThroughCarry_Operator_Hex_RightShift` | `ProgrammerModeFunctionalTests.cs:882-892` |
| 59 | `Copy_And_Paste_Simple_Number` | `ProgrammerModeFunctionalTests.cs:899-910` |
| 60 | `Copy_And_Paste_Invalid_Number` | `ProgrammerModeFunctionalTests.cs:912-924` |

Lifecycle: `[ClassInitialize] ClassInitialize` / `[ClassCleanup] ClassCleanup` / `[TestInitialize] TestInit` / `[TestCleanup] TestCleanup` — each resets the calculator to a known state before every test.

### 6.4 `[TestClass] MemoryFunctionalTests` — `MemoryFunctionalTests.cs` (5 tests)

Memory panel / memory flyout in standard mode: list contents after `M+`, `MR`, `M-`; item ordering;
and **focus management after clearing the only item** (`Memory_` panel) and after clearing the first of
several (focus moves to the next item).

| # | Test method | Source lines |
|---|---|---|
| 1 | `StandardMemory_Panel` | `MemoryFunctionalTests.cs:71-100` |
| 2 | `StandardMemory_Flyout` | `MemoryFunctionalTests.cs:102-137` |
| 3 | `StandardMemory_Panel_FocusMovesToMemoryPlusAfterClearingOnlyItem` | `MemoryFunctionalTests.cs:144-165` |
| 4 | `StandardMemory_Panel_FocusMovesToNextItemAfterClearingFirstOfMultiple` | `MemoryFunctionalTests.cs:171-196` |
| 5 | `StandardMemory_Flyout_FocusMovesToMemoryPlusAfterClearingOnlyItem` | `MemoryFunctionalTests.cs:202-223` |

Lifecycle: `[ClassInitialize] ClassInitialize` / `[ClassCleanup] ClassCleanup` / `[TestInitialize] TestInit` / `[TestCleanup] TestCleanup` — each resets the calculator to a known state before every test.

### 6.5 `[TestClass] HistoryFunctionalTests` — `HistoryFunctionalTests.cs` (4 tests)

History panel / history flyout in standard mode: list contents and ordering after operations;
and **focus management after deleting the only item** (panel focus must not fall back to `Clear all`;
flyout focus moves to the history button).

| # | Test method | Source lines |
|---|---|---|
| 1 | `StandardHistory_Panel` | `HistoryFunctionalTests.cs:75-116` |
| 2 | `StandardHistory_Flyout` | `HistoryFunctionalTests.cs:118-171` |
| 3 | `StandardHistory_Panel_FocusDoesNotFallBackToClearAllAfterDeletingOnlyItem` | `HistoryFunctionalTests.cs:181-207` |
| 4 | `StandardHistory_Flyout_FocusMovesToHistoryButtonAfterDeletingOnlyItem` | `HistoryFunctionalTests.cs:214-236` |

Lifecycle: `[ClassInitialize] ClassInitialize` / `[ClassCleanup] ClassCleanup` / `[TestInitialize] TestInit` / `[TestCleanup] TestCleanup` — each resets the calculator to a known state before every test.

### 6.6 `[TestClass] CurrencyConverterFunctionalTests` — `CurrencyConverterFunctionalTests.cs` (8 tests)

Currency converter page.
* Enter a value and check the converted result; enter the full decimal fraction and clear.
* Enter the full decimal fraction and backspace.
* **Currency fraction-digit formatting**: a 3-fractional-digit currency, a 0-fractional-digit currency,
  switching between currencies with different fractional digits, switching to fewer fractional digits then
  entering input, and result consistency after switching.

| # | Test method | Source lines |
|---|---|---|
| 1 | `MouseInput_EnterInputAndCheckTheResult` | `CurrencyConverterFunctionalTests.cs:77-85` |
| 2 | `MouseInput_EnterInputWithFullDecimalAndClear` | `CurrencyConverterFunctionalTests.cs:91-115` |
| 3 | `MouseInput_EnterInputWithFullDecimalAndClearWithBackspace` | `CurrencyConverterFunctionalTests.cs:121-149` |
| 4 | `MouseInput_SelectCurrencyWith3FractionalDigitsEnterInputAndCheckTheFormat` | `CurrencyConverterFunctionalTests.cs:155-177` |
| 5 | `MouseInput_SelectCurrencyWithoutFractionalDigitEnterInputAndCheckTheFormat` | `CurrencyConverterFunctionalTests.cs:183-199` |
| 6 | `MouseInput_SwitchCurrencyWithDifferentFractionalDigitsAndCheckTheFormat` | `CurrencyConverterFunctionalTests.cs:205-227` |
| 7 | `MouseInput_SwitchCurrencyWithLessFractionalDigitsAndEnterInput` | `CurrencyConverterFunctionalTests.cs:233-257` |
| 8 | `MouseInput_SwitchCurrencyWithLessFractionalDigitsAndCheckIfTheResultIsConsistent` | `CurrencyConverterFunctionalTests.cs:263-296` |

Lifecycle: `[ClassInitialize] ClassInitialize` / `[ClassCleanup] ClassCleanup` / `[TestInitialize] TestInit` / `[TestCleanup] TestCleanup` — each resets the calculator to a known state before every test.

**UI test total: 6 classes, 160 test methods.**

### 6.7 UI test support library — `src/CalculatorUITestFramework/` (no test methods)

Page objects used by the classes above. Useful for understanding what the UI tests actually assert
(e.g. `CalculatorResults.GetCalculatorResultText()` strips the narrator prefix `"Display is"`, and
`GetCalculatorExpressionText()` strips `"Expression is"` — so the asserted strings are the raw
calculator display / expression strings).

| File | Role |
|---|---|
| `src/CalculatorUITestFramework/CalculatorApp.cs` | App-level helpers (focus, resize for memory/history dock, always-on-top window handle). |
| `src/CalculatorUITestFramework/CalculatorDriver.cs` | WinAppDriver session lifetime (`SetupCalculatorSession` / `TearDownCalculatorSession`). |
| `src/CalculatorUITestFramework/CalculatorResults.cs` | Reads `CalculatorResults` / `CalculatorExpression` / `CalculatorAlwaysOnTopResults` automation ids and strips narrator prefixes. |
| `src/CalculatorUITestFramework/CalculatorUITestFramework.csproj` | Project file for the support library. |
| `src/CalculatorUITestFramework/HistoryItem.cs` | `HistoryItem` page object (value + expression). |
| `src/CalculatorUITestFramework/HistoryPanel.cs` | History panel/flyout page object. |
| `src/CalculatorUITestFramework/MemoryItem.cs` | `MemoryItem` page object. |
| `src/CalculatorUITestFramework/MemoryPanel.cs` | Memory panel/flyout page object. |
| `src/CalculatorUITestFramework/NavigationMenu.cs` | Navigation menu page object (mode switching). |
| `src/CalculatorUITestFramework/NumberPad.cs` | Number-pad page object (0-9, A-F, decimal, +/-). |
| `src/CalculatorUITestFramework/ProgrammerCalculatorPage.cs` | Programmer page (navigate, operator panel, memory/history). |
| `src/CalculatorUITestFramework/ProgrammerOperatorsPanel.cs` | Programmer operators panel. |
| `src/CalculatorUITestFramework/ScientificCalculatorPage.cs` | Scientific page (navigate, operator panel, memory/history). |
| `src/CalculatorUITestFramework/ScientificOperatorsPanel.cs` | Scientific operators panel (trig, log, functions, angle mode). |
| `src/CalculatorUITestFramework/StandardAoTCalculatorPage.cs` | Always-on-top window page object. |
| `src/CalculatorUITestFramework/StandardCalculatorPage.cs` | Standard page (navigate, operator panel, memory/history). |
| `src/CalculatorUITestFramework/StandardOperatorsPanel.cs` | Standard operators panel (+,-,×,÷,%,√,1/x,x²,=). |
| `src/CalculatorUITestFramework/UnitConverterOperatorsPanel.cs` | Converter operators panel (copy, swap, digits). |
| `src/CalculatorUITestFramework/UnitConverterPage.cs` | Converter page (category/unit selectors, result panel). |
| `src/CalculatorUITestFramework/UnitConverterResults.cs` | Converter results reader (primary + secondary results). |
| `src/CalculatorUITestFramework/WinAppDriverLocalServer.cs` | Manages the local WinAppDriver.exe process. |
| `src/CalculatorUITestFramework/WindowsDriverExtensions.cs` | `TryFindElementByAccessibilityId` and friends. |
| `src/CalculatorUITestFramework/WindowsElementExtensions.cs` | Element helpers (click, text, focus). |

---

## 7. Skipped / ignored tests

**Result: there are NONE.** Exhaustive grep across all three test projects for
`[Fact(Skip=`, `[Ignore`, `[Ignore(`, `Skip=`, `Ignore=`, `Assert.Inconclusive`, `Assert.Ignore`,
`#if`/`#if !DEBUG` guards around test bodies, and early-`return` bail-outs returns:

```
grep -rn "Ignore\]\|\[Ignore\|Skip *=\|Ignore *=" \
    src/CalculatorUnitTests/ src/Calculator.Tests/ src/CalculatorUITests/
# (no output)
```

The only conditional-compilation in the native test project is unrelated to skipping:

| Location | Directive | Effect |
|---|---|---|
| `src/CalculatorUnitTests/UnitTestApp.xaml.h` / `.cpp` | `#ifdef DEBUG` (generated UWP host scaffolding) | not a test skip |

**What *does* affect which tests run (not skips, but filters):**

| Mechanism | Where | Notes |
|---|---|---|
| `[Priority(n)]` | `src/CalculatorUITests/*.cs` (35× P0, 77× P1, 48× P2) | MSTest priority; used by CI to order/stage the Appium suite. Not a skip. |
| `.runsettings` | `CalculatorUITests.ci.runsettings`, `.ci-internal.runsettings`, `.release.runsettings` | supply `AppId` and the two synthetic test currencies `Test Fractional Digits - Test Fractional Digits` / `Test No Fractional Digits - Test No Fractional Digits` consumed by `CurrencyConverterFunctionalTests`. |
| `<Build Solution="*|ARM64" Project="false" />` | `src/Calculator.slnx` | `CalculatorUnitTests` and `Calculator.Tests` are **not built** for the ARM64 solution configuration. |
| UI tests need a live window | `CalculatorDriver.SetupCalculatorSession` | On a headless Linux port these cannot run at all — they are the only coverage for rendering/focus/AoT. |

---

## 8. Coverage gaps relevant to a Linux port (things the reference does NOT test)

Derived from the inventory in §1. These are places where the reference offers **no golden vector**,
so a port cannot be validated against it and must define its own expectations.

| # | Gap | Evidence |
|---|---|---|
| 1 | **No C++ engine test project other than `CalculatorUnitTests`.** The whole of `src/CalcManager/` (including `CEngine/`, `Ratpack/`, `Command.h`, `ExpressionCommand.cpp`, `CalculatorHistory.cpp`) has no in-tree test file; it is only exercised indirectly through the 86 `TEST_METHOD`s in `CalculatorUnitTests`. | `find src/CalcManager -iname '*test*'` -> 0 results |
| 2 | **No tests inside `CalcManager/CEngine`** — no parser tests, no operator-precedence tests, no `Command` enum tests, no `CCommandArgs`/tokenizer tests, no trig-function accuracy tests. | `ls src/CalcManager/CEngine` -> 12 production .cpp, 0 test files |
| 3 | **No factorial tests in the unit-test suites.** `n!` is only exercised by the Appium UI test `Operator_FactorialButton` (`ScientificModeFunctionalTests`), which needs a live Windows app. | `grep factorial -> UI test only` |
| 4 | **No explicit overflow / underflow / infinity / NaN vectors.** The only error/overflow display strings the reference pins are `L"Overflow"` (for `1.e-9999 ÷ 1`), `L"Result is undefined"` (0÷0), `L"Cannot divide by zero"` (1÷0) and `L"Invalid input"` (`log(-2)`) — asserted in `CalculatorManagerTestStandard` (:366,:373,:376), `CalculatorManagerTestScientific` (:450,:457,:460) and `CalculatorManagerTestScientificError` (:684,:687,:692,:696,:699,:700). There are **no** vectors for ±Infinity, NaN, or decimal over/underflow. | `CalculatorManagerTest.cpp:349-376, 433-460, 681-702` |
| 5 | **No rounding-mode vectors** (no banker's/half-away/half-even tie cases) beyond `RoundSignificantDigits` and `UnitConverterTestSupplementaryResultRounding`. | `grep 'Round' -> only those two` |
| 6 | **No negative-zero vectors.** `TryToggleSignZero` exists in `CalcInputTest` but there is no `-0` display-string assertion anywhere. | `CalcInputTest.cpp:42` |
| 7 | **No time-zone / DST date vectors.** `DateCalcViewModelDateDiffDaylightSavingTimeTest` exists, but it uses fixed `DateTimeOffset` values, not real zone transitions. | `DateCalculatorTests.cs:379` |
| 8 | **No leap-year-century / proleptic-Gregorian edge vectors** beyond 1601-01-01, 9998-12-31, 9999-12-31 and 2008-02-29 already in the `s_date[]` table. | `DateCalculatorTests.cs:44-58` (`s_date[]` literals) |
| 9 | **No currency *rate* vectors.** `CurrencyConverterTests` and `CurrencyConverterFunctionalTests` test caching, ordering, symbol handling and *formatting* (fraction digits) only — never a real conversion result such as `1 USD = X EUR`. | `grep Assert -> no rate literals` |
| 10 | **No graphing/equation-solving numeric vectors.** `GraphingViewModelTests` has 2 tests, both about view-model plumbing and localized error strings, not about plot values. | `GraphingViewModelTests.cs` (2 tests) |
| 11 | **No tests for `Calculator.ViewModels` `ProgrammerCalculatorViewModel` arithmetic** in isolation — programmer-mode arithmetic is only covered via `CalculatorManagerTestProgrammer` (engine) and the 60 Appium UI tests. | `no ProgrammerCalculatorViewModelTests.cs` |
| 12 | **No property-based / fuzz testing** anywhere, so overflow/invalid-input coverage is only as good as the hand-written literals. | `no fuzz/hypothesis package` |
| 13 | **No performance regression tests.** `KeystrokeLatencyTests` (1 test) is a single converter-typing timing assertion, not a benchmark suite. | `KeystrokeLatencyTests.cs` (1 test) |
| 14 | **No accessibility / high-contrast / RTL layout vectors.** Screen-reader *strings* are tested in `NarratorAnnouncementTests` (19 tests), but no visual/layout assertions exist. | `grep -> NarratorAnnouncementTests only` |
| 15 | **No settings-persistence or update-feed vectors** (`SettingsFragment`, `UpdateManager`). | `no SettingsFragment tests` |

---

## 9. Quick reference — full test counts

| Suite | Classes | Test methods |
|---|---|---|
| `src/CalculatorUnitTests/` (C++/WinRT, engine) | 5 `TEST_CLASS` | 86 `TEST_METHOD` |
| `src/Calculator.Tests/` (C#/MSTest, view models) | 23 `[TestClass]` | 288 `[TestMethod]`/`[DataTestMethod]` |
| `src/CalculatorUITests/` (C#/MSTest + Appium) | 6 `[TestClass]` | 160 `[TestMethod]` |
| **Total** | **34** | **534** |

| Extra | Count |
|---|---|
| `[DataRow]` parameterised cases | 11 (all in `SnapshotJsonTests.cs`) |
| `[DataTestMethod]` methods | 5 (all in `SnapshotJsonTests.cs`) |
| `[ClassInitialize]` / `[TestInitialize]` / `[ClassCleanup]` / `[TestCleanup]` hooks | present in every unit-test class; `ClassInitialize`+`TestInitialize`+`TestCleanup`+`ClassCleanup` in every UI class |
| Tests skipped/ignored | **0** |

## 10. Reproduction

Everything above was produced from the reference tree with a single `python3` pass that:
reads each source file as bytes (preserving UTF-8 and BOM state), blanking out string/char
literals and comments before brace-counting so that block boundaries are exact, then writes the
resulting line ranges into this document **verbatim**. No test body has been reformatted,
truncated, summarised or paraphrased; line-range comments (`<!-- file:start-end -->`) give the
exact provenance of every fragment.

