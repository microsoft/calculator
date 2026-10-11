# REFERENCE REPO INVENTORY — RAW EXTRACTION

**Source:** `/home/elshad-guliyev/ref-calculator` (unmodified, `git HEAD = d125246`)
**Extraction date:** 2026-10-11
**Method:** `python3` with `xml.etree.ElementTree` + `re`. No manual reading of large files.
**Scope:** reference repo only. No port code was consulted.

---

## TABLE OF CONTENTS

| § | Section | Rows |
|---|---|---|
| 1 | [MODES](#1-modes) | 18 NavCategory + 19 ViewMode + 2 groups |
| 2 | [BUTTONS](#2-buttons) | 200 controls across 9 files |
| 3 | [SHORTCUTS](#3-shortcuts) | 220 bindings + 6 DPs + enum |
| 4 | [OPTIONS / TOGGLES](#4-options--toggle-inventory-with-defaults) | 13 VM toggles + UI toggles |
| 5 | [RESOURCE STRINGS](#5-resource-strings) | 1166 + 108 = 1274 pairs |
| 6 | [LIMITS](#6-limits) | 12 constants |
| 7 | [CONVERTER DATA](#7-converter-data) | 158 units, 155 factors |
| 8 | [GRAPHING](#8-graphing) | 13 files, 10 interfaces |
| 9 | [CURRENCY](#9-currency) | MOCK — 10 fake currencies |
| 10 | [TEST PROJECTS](#10-test-projects) | 534 test methods |

---


## 1. MODES

### ViewMode enum

| ViewMode | Value | file:line |
|---|---|---|
| None | -1 | NavCategory.cs:21 |
| Standard | 0 | NavCategory.cs:22 |
| Scientific | 1 | NavCategory.cs:23 |
| Programmer | 2 | NavCategory.cs:24 |
| Date | 3 | NavCategory.cs:25 |
| Volume | 4 | NavCategory.cs:26 |
| Length | 5 | NavCategory.cs:27 |
| Weight | 6 | NavCategory.cs:28 |
| Temperature | 7 | NavCategory.cs:29 |
| Energy | 8 | NavCategory.cs:30 |
| Area | 9 | NavCategory.cs:31 |
| Speed | 10 | NavCategory.cs:32 |
| Time | 11 | NavCategory.cs:33 |
| Power | 12 | NavCategory.cs:34 |
| Data | 13 | NavCategory.cs:35 |
| Pressure | 14 | NavCategory.cs:36 |
| Angle | 15 | NavCategory.cs:37 |
| Currency | 16 | NavCategory.cs:38 |

### CategoryGroupType enum

| Name | Value | file:line |
|---|---|---|
| None | -1 | NavCategory.cs:44 |
| Calculator | 0 | NavCategory.cs:45 |

### NavCategoryStates ID constants (frozen)

| Constant | Value | file:line |
|---|---|---|
| StandardId | 0 | NavCategory.cs:223 |
| ScientificId | 1 | NavCategory.cs:224 |
| ProgrammerId | 2 | NavCategory.cs:225 |
| DateId | 3 | NavCategory.cs:226 |
| VolumeId | 4 | NavCategory.cs:227 |
| LengthId | 5 | NavCategory.cs:228 |
| WeightId | 6 | NavCategory.cs:229 |
| TemperatureId | 7 | NavCategory.cs:230 |
| EnergyId | 8 | NavCategory.cs:231 |
| AreaId | 9 | NavCategory.cs:232 |
| SpeedId | 10 | NavCategory.cs:233 |
| TimeId | 11 | NavCategory.cs:234 |
| PowerId | 12 | NavCategory.cs:235 |
| DataId | 13 | NavCategory.cs:236 |
| PressureId | 14 | NavCategory.cs:237 |
| AngleId | 15 | NavCategory.cs:238 |
| CurrencyId | 16 | NavCategory.cs:239 |
| GraphingId | 17 | NavCategory.cs:240 |

### CategoryManifest entries (declared order = nav order)

**Calculator group (5 entries)**

| # | ViewMode | SerId | FriendlyName | NameResourceKey | Glyph escape | Glyph code point | Glyph char | VirtualKey | AccessKey | SupportsNegative | file:line |
|---|---|---|---|---|---|---|---|---|---|---|---|
| 0 | Standard | StandardId | Standard | StandardMode | `\uE8EF` | U+E8EF | `\uE8EF` PUA (Segoe MDL2 Assets private-use) | Number1 | "1" | SupportsAll | NavCategory.cs:247 |
| 1 | Scientific | ScientificId | Scientific | ScientificMode | `\uF196` | U+F196 | `\uF196` PUA (Segoe MDL2 Assets private-use) | Number2 | "2" | SupportsAll | NavCategory.cs:248 |
| 2 | Graphing | GraphingId | Graphing | GraphingCalculatorMode | `\uF770` | U+F770 | `\uF770` PUA (Segoe MDL2 Assets private-use) | Number3 | "3" | SupportsAll | NavCategory.cs:249 |
| 3 | Programmer | ProgrammerId | Programmer | ProgrammerMode | `\uECCE` | U+ECCE | `\uECCE` PUA (Segoe MDL2 Assets private-use) | Number4 | "4" | SupportsAll | NavCategory.cs:250 |
| 4 | Date | DateId | Date | DateCalculationMode | `\uE787` | U+E787 | `\uE787` PUA (Segoe MDL2 Assets private-use) | Number5 | "5" | SupportsAll | NavCategory.cs:251 |

**Converter group (13 entries)**

| # | ViewMode | SerId | FriendlyName | NameResourceKey | Glyph escape | Glyph code point | Glyph char | VirtualKey | AccessKey | SupportsNegative | file:line |
|---|---|---|---|---|---|---|---|---|---|---|---|
| 0 | Currency | CurrencyId | Currency | CategoryName_Currency | `\uEB0D` | U+EB0D | `\uEB0D` PUA (Segoe MDL2 Assets private-use) | None | null | PositiveOnly | NavCategory.cs:252 |
| 1 | Volume | VolumeId | Volume | CategoryName_Volume | `\uF1AA` | U+F1AA | `\uF1AA` PUA (Segoe MDL2 Assets private-use) | None | null | PositiveOnly | NavCategory.cs:253 |
| 2 | Length | LengthId | Length | CategoryName_Length | `\uECC6` | U+ECC6 | `\uECC6` PUA (Segoe MDL2 Assets private-use) | None | null | PositiveOnly | NavCategory.cs:254 |
| 3 | Weight | WeightId | Weight and Mass | CategoryName_Weight | `\uF4C1` | U+F4C1 | `\uF4C1` PUA (Segoe MDL2 Assets private-use) | None | null | PositiveOnly | NavCategory.cs:255 |
| 4 | Temperature | TemperatureId | Temperature | CategoryName_Temperature | `\uE7A3` | U+E7A3 | `\uE7A3` PUA (Segoe MDL2 Assets private-use) | None | null | SupportsNegative | NavCategory.cs:256 |
| 5 | Energy | EnergyId | Energy | CategoryName_Energy | `\uECAD` | U+ECAD | `\uECAD` PUA (Segoe MDL2 Assets private-use) | None | null | PositiveOnly | NavCategory.cs:257 |
| 6 | Area | AreaId | Area | CategoryName_Area | `\uE809` | U+E809 | `\uE809` PUA (Segoe MDL2 Assets private-use) | None | null | PositiveOnly | NavCategory.cs:258 |
| 7 | Speed | SpeedId | Speed | CategoryName_Speed | `\uEADA` | U+EADA | `\uEADA` PUA (Segoe MDL2 Assets private-use) | None | null | PositiveOnly | NavCategory.cs:259 |
| 8 | Time | TimeId | Time | CategoryName_Time | `\uE917` | U+E917 | `\uE917` PUA (Segoe MDL2 Assets private-use) | None | null | PositiveOnly | NavCategory.cs:260 |
| 9 | Power | PowerId | Power | CategoryName_Power | `\uE945` | U+E945 | `\uE945` PUA (Segoe MDL2 Assets private-use) | None | null | SupportsNegative | NavCategory.cs:261 |
| 10 | Data | DataId | Data | CategoryName_Data | `\uF20F` | U+F20F | `\uF20F` PUA (Segoe MDL2 Assets private-use) | None | null | PositiveOnly | NavCategory.cs:262 |
| 11 | Pressure | PressureId | Pressure | CategoryName_Pressure | `\uEC4A` | U+EC4A | `\uEC4A` PUA (Segoe MDL2 Assets private-use) | None | null | PositiveOnly | NavCategory.cs:263 |
| 12 | Angle | AngleId | Angle | CategoryName_Angle | `\uF515` | U+F515 | `\uF515` PUA (Segoe MDL2 Assets private-use) | None | null | SupportsNegative | NavCategory.cs:264 |

TOTAL manifest entries: 18 (Calculator=5, Converter=13)

### Category groups (nav menu sections)

| Group | HeaderResourceKey | ModeResourceKey | AutomationResourceKey | file:line |
|---|---|---|---|---|
| Calculator | CalculatorModeTextCaps | CalculatorModeText | CalculatorModePluralText | NavCategory.cs:285 |
| Converter | ConverterModeTextCaps | ConverterModeText | ConverterModePluralText | NavCategory.cs:296 |

Menu order (CreateMenuOptions): Calculator group then Converter group — NavCategory.cs:272-278

### NavCategory.IsEnabled initial value

`_categories.Add(new NavCategory(..., categoryInitializer.ViewMode != ViewMode.Graphing))` — NavCategory.cs:183
=> IsEnabled=false for Graphing, true for all others (initial state before OnNavigatedTo).


---


## 2. BUTTONS


**Grid-walker:** a recursive flattener (`/tmp/opencode/refinv/buttons.py`) walks the XAML visual tree and converts nested `<Grid>` into **absolute** `(row, col)` by summing the parent grid's own `(row, col)` offset with the child's. `Grid.Row`/`Grid.Column` are read as the literal unprefixed dotted attributes XAML actually uses (`"Grid.Row"`), `RowSpan`/`ColumnSpan` are recorded, and Flyout sub-trees are treated as separate visual trees with their own local `(0,0)` origin.

**WinUI semantics applied:** an omitted `Grid.Row`/`Grid.Column` resolves to `0` (there is no auto-flow stacking in WPF/WinUI Grid).


### CalculatorStandardOperators.xaml

Source: `src/Calculator/Views/CalculatorStandardOperators.xaml`

**Buttons: 13** (main panel: 13, flyout: 0)

| # | x:Name | ButtonId | Glyph (code points) | Style | FontSize | IsEnabled binding | Abs (row,col) | Span | Grid chain | file:line |
|---|---|---|---|---|---|---|---|---|---|---|
| 0 | `PercentButton` | `Percent` |  `U+E94C` | `SymbolOperatorButtonStyle` | `` | `(default true)` | (0,2) | 1x1 | ControlRoot>Root>DisplayControls | CalculatorStandardOperators.xaml:185 |
| 1 | `ClearEntryButton` | `ClearEntry` | `CE` | `OperatorButtonStyle` | `16` | `(default true)` | (0,3) | 1x1 | ControlRoot>Root>DisplayControls | CalculatorStandardOperators.xaml:193 |
| 2 | `ClearButton` | `Clear` | `C` | `OperatorButtonStyle` | `16` | `(default true)` | (0,4) | 1x1 | ControlRoot>Root>DisplayControls | CalculatorStandardOperators.xaml:202 |
| 3 | `BackSpaceButton` | `Backspace` |  `U+E94F` | `SymbolOperatorButtonStyle` | `16` | `(default true)` | (0,5) | 1x1 | ControlRoot>Root>DisplayControls | CalculatorStandardOperators.xaml:211 |
| 4 | `InvertButton` | `Invert` |  `U+F7C9` | `SymbolOperatorButtonStyle` | `18` | `(default true)` | (1,2) | 1x1 | ControlRoot>Root>StandardFunctions | CalculatorStandardOperators.xaml:241 |
| 5 | `XPower2Button` | `XPower2` |  `U+F7C8` | `SymbolOperatorButtonStyle` | `18` | `(default true)` | (1,3) | 1x1 | ControlRoot>Root>StandardFunctions | CalculatorStandardOperators.xaml:249 |
| 6 | `SquareRootButton` | `Sqrt` |  `U+F899` | `SymbolOperatorButtonStyle` | `` | `(default true)` | (1,4) | 1x1 | ControlRoot>Root>StandardFunctions | CalculatorStandardOperators.xaml:258 |
| 7 | `DivideButton` | `Divide` |  `U+E94A` | `SymbolOperatorButtonStyle` | `` | `(default true)` | (1,5) | 1x1 | ControlRoot>Root>StandardOperators | CalculatorStandardOperators.xaml:281 |
| 8 | `MultiplyButton` | `Multiply` |  `U+E947` | `SymbolOperatorButtonStyle` | `` | `(default true)` | (1,6) | 1x1 | ControlRoot>Root>StandardOperators | CalculatorStandardOperators.xaml:290 |
| 9 | `MinusButton` | `Subtract` |  `U+E949` | `SymbolOperatorButtonStyle` | `` | `(default true)` | (1,7) | 1x1 | ControlRoot>Root>StandardOperators | CalculatorStandardOperators.xaml:299 |
| 10 | `PlusButton` | `Add` |  `U+E948` | `SymbolOperatorButtonStyle` | `` | `(default true)` | (1,8) | 1x1 | ControlRoot>Root>StandardOperators | CalculatorStandardOperators.xaml:308 |
| 11 | `EqualButton` | `Equals` |  `U+E94E` | `AccentEmphasizedCalcButtonStyle` | `` | `(default true)` | (1,9) | 1x1 | ControlRoot>Root>StandardOperators | CalculatorStandardOperators.xaml:317 |
| 12 | `NegateButton` | `Negate` |  `U+F898` | `SymbolOperatorKeypadButtonStyle` | `` | `(default true)` | (5,2) | 1x1 | ControlRoot>Root | CalculatorStandardOperators.xaml:337 |

#### Physical layout reconstruction (main panel, 6 rows x 10 cols; cells = x:Name)

| | c0 | c1 | c2 | c3 | c4 | c5 | c6 | c7 | c8 | c9 |
|---|---|---|---|---|---|---|---|---|---|---|
| r0 |  |  | PercentButton | ClearEntryButton | ClearButton | BackSpaceButton |  |  |  |  |
| r1 |  |  | InvertButton | XPower2Button | SquareRootButton | DivideButton | MultiplyButton | MinusButton | PlusButton | EqualButton |
| r2 |  |  |  |  |  |  |  |  |  |  |
| r3 |  |  |  |  |  |  |  |  |  |  |
| r4 |  |  |  |  |  |  |  |  |  |  |
| r5 |  |  | NegateButton |  |  |  |  |  |  |  |

_Absolute coordinates already fold in each parent grid's own `(row,col)` offset._

#### Styles referenced (4 distinct)

| Style key | Defined in file? | BasedOn | TargetType | file:line |
|---|---|---|---|---|
| `AccentEmphasizedCalcButtonStyle` | no (external dictionary) | – | – | – |
| `OperatorButtonStyle` | no (external dictionary) | – | – | – |
| `SymbolOperatorButtonStyle` | no (external dictionary) | – | – | – |
| `SymbolOperatorKeypadButtonStyle` | no (external dictionary) | – | – | – |

### CalculatorScientificOperators.xaml

Source: `src/Calculator/Views/CalculatorScientificOperators.xaml`

**Buttons: 65** (main panel: 33, flyout: 32)

| # | x:Name | ButtonId | Glyph (code points) | Style | FontSize | IsEnabled binding | Abs (row,col) | Span | Grid chain | file:line |
|---|---|---|---|---|---|---|---|---|---|---|
| 0 | `TrigButton` | `` | (none) | `OperatorPanelButtonStyle` | `` | `(default true)` | (0,0) | 1x1 | ControlRoot>ScientificOperators | CalculatorScientificOperators.xaml:379 |
| 1 | `TrigShiftButton` | `` |  `U+F897` _(flyout: TrigButton)_ | `CaptionToggleButtonStyle` | `{StaticResource CalcOperatorCaptionSize}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (0,0) | 1x1 | ControlRoot>ScientificOperators>TrigGrid | CalculatorScientificOperators.xaml:411 |
| 2 | `AbsFlyoutButton` | `Abs` |  `U+F884` _(flyout: FuncButton)_ | `SymbolOperatorButtonStyle` | `` | `{x:Bind FuncButton.IsEnabled, Mode=OneWay}` | (0,0) | 1x1 | ControlRoot>ScientificOperators>FuncGrid | CalculatorScientificOperators.xaml:843 |
| 3 | `HypButton` | `` | `hyp` _(flyout: TrigButton)_ | `CaptionToggleButtonStyle` | `` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (0,1) | 1x1 | ControlRoot>ScientificOperators>TrigGrid | CalculatorScientificOperators.xaml:422 |
| 4 | `FuncButton` | `` | (none) | `OperatorPanelButtonStyle` | `` | `(default true)` | (0,1) | 1x1 | ControlRoot>ScientificOperators | CalculatorScientificOperators.xaml:816 |
| 5 | `FloorButton` | `Floor` |  `U+F885` _(flyout: FuncButton)_ | `SymbolOperatorButtonStyle` | `` | `{x:Bind FuncButton.IsEnabled, Mode=OneWay}` | (0,1) | 1x1 | ControlRoot>ScientificOperators>FuncGrid | CalculatorScientificOperators.xaml:854 |
| 6 | `ShiftButton` | `` |  `U+F897` | `CaptionToggleEmphasizedButtonStyle` | `{StaticResource CalcOperatorCaptionSize}` | `(default true)` | (0,1) | 1x1 | ControlRoot>ScientificOperators | CalculatorScientificOperators.xaml:921 |
| 7 | `SinButton` | `Sin` | `sin` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (0,2) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>TrigFunctions | CalculatorScientificOperators.xaml:448 |
| 8 | `CeilButton` | `Ceil` |  `U+F886` _(flyout: FuncButton)_ | `SymbolOperatorButtonStyle` | `` | `{x:Bind FuncButton.IsEnabled, Mode=OneWay}` | (0,2) | 1x1 | ControlRoot>ScientificOperators>FuncGrid | CalculatorScientificOperators.xaml:866 |
| 9 | `XPower2Button` | `XPower2` |  `U+F7C8` | `SymbolOperatorButtonStyle` | `` | `(default true)` | (0,2) | 1x1 | ControlRoot>ScientificOperators>Row1 | CalculatorScientificOperators.xaml:1017 |
| 10 | `CosButton` | `Cos` | `cos` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (0,3) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>TrigFunctions | CalculatorScientificOperators.xaml:460 |
| 11 | `InvsinButton` | `InvSin` | `sin⁻¹` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (0,3) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>InverseTrigFunctions | CalculatorScientificOperators.xaml:543 |
| 12 | `RandButton` | `Rand` | `rand` _(flyout: FuncButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind FuncButton.IsEnabled, Mode=OneWay}` | (0,3) | 1x1 | ControlRoot>ScientificOperators>FuncGrid | CalculatorScientificOperators.xaml:878 |
| 13 | `SquareRootButton` | `Sqrt` |  `U+F899` | `SymbolOperatorButtonStyle` | `` | `(default true)` | (0,3) | 1x1 | ControlRoot>ScientificOperators>Row1 | CalculatorScientificOperators.xaml:1025 |
| 14 | `XPower3Button` | `Cube` |  `U+F7CB` | `EmphasizedCalcButtonStyle` | `` | `(default true)` | (0,3) | 1x1 | ControlRoot>ScientificOperators>InvRow1 | CalculatorScientificOperators.xaml:1083 |
| 15 | `TanButton` | `Tan` | `tan` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (0,4) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>TrigFunctions | CalculatorScientificOperators.xaml:473 |
| 16 | `InvcosButton` | `InvCos` | `cos⁻¹` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (0,4) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>InverseTrigFunctions | CalculatorScientificOperators.xaml:555 |
| 17 | `SinhButton` | `Sinh` | `sinh` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (0,4) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>HyperbolicTrigFunctions | CalculatorScientificOperators.xaml:638 |
| 18 | `PowerButton` | `XPowerY` |  `U+F7CA` | `SymbolOperatorButtonStyle` | `` | `(default true)` | (0,4) | 1x1 | ControlRoot>ScientificOperators>Row1 | CalculatorScientificOperators.xaml:1034 |
| 19 | `CubeRootButton` | `CubeRoot` |  `U+F881` | `EmphasizedCalcButtonStyle` | `` | `(default true)` | (0,4) | 1x1 | ControlRoot>ScientificOperators>InvRow1 | CalculatorScientificOperators.xaml:1092 |
| 20 | `SecButton` | `Sec` | `sec` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (0,5) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>TrigFunctions | CalculatorScientificOperators.xaml:486 |
| 21 | `InvtanButton` | `InvTan` | `tan⁻¹` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (0,5) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>InverseTrigFunctions | CalculatorScientificOperators.xaml:568 |
| 22 | `CoshButton` | `Cosh` | `cosh` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (0,5) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>HyperbolicTrigFunctions | CalculatorScientificOperators.xaml:650 |
| 23 | `InvsinhButton` | `InvSinh` | `sinh⁻¹` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (0,5) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>InverseHyperbolicTrigFunctions | CalculatorScientificOperators.xaml:732 |
| 24 | `PowerOf10Button` | `TenPowerX` |  `U+F7CC` | `SymbolOperatorButtonStyle` | `` | `(default true)` | (0,5) | 1x1 | ControlRoot>ScientificOperators>Row1 | CalculatorScientificOperators.xaml:1045 |
| 25 | `YSquareRootButton` | `YRootX` |  `U+F7CD` | `EmphasizedCalcButtonStyle` | `` | `(default true)` | (0,5) | 1x1 | ControlRoot>ScientificOperators>InvRow1 | CalculatorScientificOperators.xaml:1102 |
| 26 | `InvsecButton` | `InvSec` | `sec⁻¹` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (0,6) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>InverseTrigFunctions | CalculatorScientificOperators.xaml:581 |
| 27 | `TanhButton` | `Tanh` | `tanh` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (0,6) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>HyperbolicTrigFunctions | CalculatorScientificOperators.xaml:663 |
| 28 | `InvcoshButton` | `InvCosh` | `cosh⁻¹` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (0,6) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>InverseHyperbolicTrigFunctions | CalculatorScientificOperators.xaml:744 |
| 29 | `LogBase10Button` | `LogBase10` | `log` | `OperatorButtonStyle` | `` | `(default true)` | (0,6) | 1x1 | ControlRoot>ScientificOperators>Row1 | CalculatorScientificOperators.xaml:1054 |
| 30 | `TwoPowerXButton` | `TwoPowerX` |  `U+F882` | `EmphasizedCalcButtonStyle` | `` | `(default true)` | (0,6) | 1x1 | ControlRoot>ScientificOperators>InvRow1 | CalculatorScientificOperators.xaml:1113 |
| 31 | `SechButton` | `Sech` | `sech` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (0,7) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>HyperbolicTrigFunctions | CalculatorScientificOperators.xaml:676 |
| 32 | `InvtanhButton` | `InvTanh` | `tanh⁻¹` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (0,7) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>InverseHyperbolicTrigFunctions | CalculatorScientificOperators.xaml:757 |
| 33 | `LogBaseEButton` | `LogBaseE` | `ln` | `OperatorButtonStyle` | `` | `(default true)` | (0,7) | 1x1 | ControlRoot>ScientificOperators>Row1 | CalculatorScientificOperators.xaml:1063 |
| 34 | `LogBaseY` | `LogBaseY` |  `U+F883` | `EmphasizedCalcButtonStyle` | `` | `(default true)` | (0,7) | 1x1 | ControlRoot>ScientificOperators>InvRow1 | CalculatorScientificOperators.xaml:1123 |
| 35 | `InvsechButton` | `InvSech` | `sech⁻¹` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (0,8) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>InverseHyperbolicTrigFunctions | CalculatorScientificOperators.xaml:770 |
| 36 | `PowerOfEButton` | `EPowerX` |  `U+F7CE` | `EmphasizedCalcButtonStyle` | `` | `(default true)` | (0,8) | 1x1 | ControlRoot>ScientificOperators>InvRow1 | CalculatorScientificOperators.xaml:1133 |
| 37 | `DmsButton` | `DMS` |  `U+F890` _(flyout: FuncButton)_ | `SymbolOperatorButtonStyle` | `` | `{x:Bind FuncButton.IsEnabled, Mode=OneWay}` | (1,1) | 1x1 | ControlRoot>ScientificOperators>FuncGrid | CalculatorScientificOperators.xaml:891 |
| 38 | `PiButton` | `Pi` |  `U+F7CF` | `SymbolOperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSize}` | `(default true)` | (1,1) | 1x1 | ControlRoot>ScientificOperators | CalculatorScientificOperators.xaml:934 |
| 39 | `DegreesButton` | `Degrees` |  `U+F891` _(flyout: FuncButton)_ | `SymbolOperatorButtonStyle` | `` | `{x:Bind FuncButton.IsEnabled, Mode=OneWay}` | (1,2) | 1x1 | ControlRoot>ScientificOperators>FuncGrid | CalculatorScientificOperators.xaml:904 |
| 40 | `EulerButton` | `Euler` | `e` | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSize}` | `(default true)` | (1,2) | 1x1 | ControlRoot>ScientificOperators | CalculatorScientificOperators.xaml:945 |
| 41 | `ClearEntryButton` | `ClearEntry` | `CE` | `OperatorButtonStyle` | `16` | `(default true)` | (1,2) | 1x1 | ControlRoot>ScientificOperators>DisplayControls | CalculatorScientificOperators.xaml:966 |
| 42 | `CscButton` | `Csc` | `csc` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (1,3) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>TrigFunctions | CalculatorScientificOperators.xaml:499 |
| 43 | `ClearButton` | `Clear` | `C` | `OperatorButtonStyle` | `16` | `(default true)` | (1,3) | 1x1 | ControlRoot>ScientificOperators>DisplayControls | CalculatorScientificOperators.xaml:978 |
| 44 | `CotButton` | `Cot` | `cot` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (1,4) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>TrigFunctions | CalculatorScientificOperators.xaml:513 |
| 45 | `InvcscButton` | `InvCsc` | `csc⁻¹` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (1,4) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>InverseTrigFunctions | CalculatorScientificOperators.xaml:594 |
| 46 | `BackSpaceButton` | `Backspace` |  `U+E94F` | `SymbolOperatorButtonStyle` | `16` | `(default true)` | (1,4) | 1x1 | ControlRoot>ScientificOperators>DisplayControls | CalculatorScientificOperators.xaml:990 |
| 47 | `InvcotButton` | `InvCot` | `cot⁻¹` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (1,5) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>InverseTrigFunctions | CalculatorScientificOperators.xaml:608 |
| 48 | `CschButton` | `Csch` | `csch` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (1,5) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>HyperbolicTrigFunctions | CalculatorScientificOperators.xaml:689 |
| 49 | `CothButton` | `Coth` | `coth` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (1,6) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>HyperbolicTrigFunctions | CalculatorScientificOperators.xaml:703 |
| 50 | `InvcschButton` | `InvCsch` | `csch⁻¹` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (1,6) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>InverseHyperbolicTrigFunctions | CalculatorScientificOperators.xaml:783 |
| 51 | `InvcothButton` | `InvCoth` | `coth⁻¹` _(flyout: TrigButton)_ | `OperatorButtonStyle` | `{StaticResource CalcOperatorCaptionSizeSmall}` | `{x:Bind TrigButton.IsEnabled, Mode=OneWay}` | (1,7) | 1x1 | ControlRoot>ScientificOperators>TrigGrid>InverseHyperbolicTrigFunctions | CalculatorScientificOperators.xaml:797 |
| 52 | `InvertButton` | `Invert` |  `U+F7C9` | `SymbolOperatorButtonStyle` | `` | `(default true)` | (2,1) | 1x1 | ControlRoot>ScientificOperators>Row2 | CalculatorScientificOperators.xaml:1156 |
| 53 | `AbsButton` | `Abs` |  `U+F884` | `SymbolOperatorButtonStyle` | `` | `(default true)` | (2,2) | 1x1 | ControlRoot>ScientificOperators>Row2 | CalculatorScientificOperators.xaml:1164 |
| 54 | `ExpButton` | `Exp` | `exp` | `OperatorButtonStyle` | `` | `(default true)` | (2,3) | 1x1 | ControlRoot>ScientificOperators>Row2 | CalculatorScientificOperators.xaml:1173 |
| 55 | `ModButton` | `Mod` | `mod` | `OperatorButtonStyle` | `` | `(default true)` | (2,4) | 1x1 | ControlRoot>ScientificOperators>Row2 | CalculatorScientificOperators.xaml:1182 |
| 56 | `OpenParenthesisButton` | `OpenParenthesis` | `(` | `ParenthesisCalcButtonStyle` | `19` | `(default true)` | (3,1) | 1x1 | ControlRoot>ScientificOperators | CalculatorScientificOperators.xaml:1193 |
| 57 | `CloseParenthesisButton` | `CloseParenthesis` | `)` | `OperatorButtonStyle` | `19` | `(default true)` | (3,2) | 1x1 | ControlRoot>ScientificOperators | CalculatorScientificOperators.xaml:1206 |
| 58 | `FactorialButton` | `Factorial` |  `U+F887` | `SymbolOperatorButtonStyle` | `18` | `(default true)` | (3,3) | 1x1 | ControlRoot>ScientificOperators | CalculatorScientificOperators.xaml:1217 |
| 59 | `DivideButton` | `Divide` |  `U+E94A` | `SymbolOperatorButtonStyle` | `` | `(default true)` | (3,4) | 1x1 | ControlRoot>ScientificOperators>StandardOperators | CalculatorScientificOperators.xaml:1241 |
| 60 | `MultiplyButton` | `Multiply` |  `U+E947` | `SymbolOperatorButtonStyle` | `` | `(default true)` | (3,5) | 1x1 | ControlRoot>ScientificOperators>StandardOperators | CalculatorScientificOperators.xaml:1251 |
| 61 | `MinusButton` | `Subtract` |  `U+E949` | `SymbolOperatorButtonStyle` | `` | `(default true)` | (3,6) | 1x1 | ControlRoot>ScientificOperators>StandardOperators | CalculatorScientificOperators.xaml:1261 |
| 62 | `PlusButton` | `Add` |  `U+E948` | `SymbolOperatorButtonStyle` | `` | `(default true)` | (3,7) | 1x1 | ControlRoot>ScientificOperators>StandardOperators | CalculatorScientificOperators.xaml:1271 |
| 63 | `EqualButton` | `Equals` |  `U+E94E` | `AccentEmphasizedCalcButtonStyle` | `` | `(default true)` | (3,8) | 1x1 | ControlRoot>ScientificOperators>StandardOperators | CalculatorScientificOperators.xaml:1281 |
| 64 | `NegateButton` | `Negate` |  `U+F898` | `SymbolOperatorKeypadButtonStyle` | `16` | `{x:Bind ViewModel.IsNegateEnabled, Mode=OneWay}` | (7,1) | 1x1 | ControlRoot>ScientificOperators | CalculatorScientificOperators.xaml:1292 |

#### Physical layout reconstruction (main panel, 8 rows x 9 cols; cells = x:Name)

| | c0 | c1 | c2 | c3 | c4 | c5 | c6 | c7 | c8 |
|---|---|---|---|---|---|---|---|---|---|
| r0 | TrigButton | ShiftButton | XPower2Button | XPower3Button | CubeRootButton | YSquareRootButton | TwoPowerXButton | LogBaseY | PowerOfEButton |
| r1 |  | PiButton | ClearEntryButton | ClearButton | BackSpaceButton |  |  |  |  |
| r2 |  | InvertButton | AbsButton | ExpButton | ModButton |  |  |  |  |
| r3 |  | OpenParenthesisButton | CloseParenthesisButton | FactorialButton | DivideButton | MultiplyButton | MinusButton | PlusButton | EqualButton |
| r4 |  |  |  |  |  |  |  |  |  |
| r5 |  |  |  |  |  |  |  |  |  |
| r6 |  |  |  |  |  |  |  |  |  |
| r7 |  | NegateButton |  |  |  |  |  |  |  |

_Absolute coordinates already fold in each parent grid's own `(row,col)` offset._

#### Styles referenced (9 distinct)

| Style key | Defined in file? | BasedOn | TargetType | file:line |
|---|---|---|---|---|
| `AccentEmphasizedCalcButtonStyle` | no (external dictionary) | – | – | – |
| `CaptionToggleButtonStyle` | no (external dictionary) | – | – | – |
| `CaptionToggleEmphasizedButtonStyle` | no (external dictionary) | – | – | – |
| `EmphasizedCalcButtonStyle` | no (external dictionary) | – | – | – |
| `OperatorButtonStyle` | no (external dictionary) | – | – | – |
| `OperatorPanelButtonStyle` | no (external dictionary) | – | – | – |
| `ParenthesisCalcButtonStyle` | no (external dictionary) | – | – | – |
| `SymbolOperatorButtonStyle` | no (external dictionary) | – | – | – |
| `SymbolOperatorKeypadButtonStyle` | no (external dictionary) | – | – | – |

### CalculatorProgrammerOperators.xaml

Source: `src/Calculator/Views/CalculatorProgrammerOperators.xaml`

**Buttons: 4** (main panel: 4, flyout: 0)

| # | x:Name | ButtonId | Glyph (code points) | Style | FontSize | IsEnabled binding | Abs (row,col) | Span | Grid chain | file:line |
|---|---|---|---|---|---|---|---|---|---|---|
| 0 | `HexButton` | `` | `{x:Bind ViewModel.HexDisplayValue, Mode=OneWay}` | `{ThemeResource ConditionalProgModeRadioButtonStyle}` | `` | `(default true)` | (0,0) | 1x1 | ControlRoot>ProgrammerOperators | CalculatorProgrammerOperators.xaml:207 |
| 1 | `DecimalButton` | `` | `{x:Bind ViewModel.DecimalDisplayValue, Mode=OneWay}` | `{ThemeResource ConditionalProgModeRadioButtonStyle}` | `` | `(default true)` | (0,1) | 1x1 | ControlRoot>ProgrammerOperators | CalculatorProgrammerOperators.xaml:218 |
| 2 | `OctButton` | `` | `{x:Bind ViewModel.OctalDisplayValue, Mode=OneWay}` | `{ThemeResource ConditionalProgModeRadioButtonStyle}` | `` | `(default true)` | (0,2) | 1x1 | ControlRoot>ProgrammerOperators | CalculatorProgrammerOperators.xaml:231 |
| 3 | `BinaryButton` | `` | `{x:Bind ViewModel.BinaryDisplayValue, Mode=OneWay}` | `{ThemeResource ConditionalProgModeRadioButtonStyle}` | `` | `(default true)` | (0,3) | 1x1 | ControlRoot>ProgrammerOperators | CalculatorProgrammerOperators.xaml:243 |

#### Physical layout reconstruction (main panel, 1 rows x 4 cols; cells = x:Name)

| | c0 | c1 | c2 | c3 |
|---|---|---|---|---|
| r0 | HexButton | DecimalButton | OctButton | BinaryButton |

_Absolute coordinates already fold in each parent grid's own `(row,col)` offset._

#### Styles referenced (1 distinct)

| Style key | Defined in file? | BasedOn | TargetType | file:line |
|---|---|---|---|---|
| `{ThemeResource ConditionalProgModeRadioButtonStyle}` | no (external dictionary) | – | – | – |

### CalculatorProgrammerRadixOperators.xaml

Source: `src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml`

**Buttons: 38** (main panel: 28, flyout: 10)

| # | x:Name | ButtonId | Glyph (code points) | Style | FontSize | IsEnabled binding | Abs (row,col) | Span | Grid chain | file:line |
|---|---|---|---|---|---|---|---|---|---|---|
| 0 | `BitwiseButton` | `` | (none) | `OperatorPanelButtonStyle` | `` | `(default true)` | (0,0) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:237 |
| 1 | `AndButton` | `And` | `AND` _(flyout: BitwiseButton)_ | `OperatorButtonStyle` | `12` | `{x:Bind BitwiseButton.IsEnabled, Mode=OneWay}` | (0,0) | 1x1 | ControlRoot>ProgRadixOps>BitwiseGrid | CalculatorProgrammerRadixOperators.xaml:268 |
| 2 | `ArithmeticShiftButton` | `` | `{utils:ResourceString Name=arithmeticShiftButton/Content}` _(flyout: BitShiftButton)_ | `` | `` | `(default true)` | (0,0) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:368 |
| 3 | `OrButton` | `Or` | `OR` _(flyout: BitwiseButton)_ | `OperatorButtonStyle` | `12` | `{x:Bind BitwiseButton.IsEnabled, Mode=OneWay}` | (0,1) | 1x1 | ControlRoot>ProgRadixOps>BitwiseGrid | CalculatorProgrammerRadixOperators.xaml:281 |
| 4 | `BitShiftButton` | `` | `{utils:ResourceString Name=arithmeticShiftButton/Content}` | `OperatorPanelButtonStyle` | `` | `(default true)` | (0,1) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:355 |
| 5 | `LogicalShiftButton` | `` | `{utils:ResourceString Name=logicalShiftButton/Content}` _(flyout: BitShiftButton)_ | `` | `` | `(default true)` | (0,1) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:372 |
| 6 | `NotButton` | `Not` | `NOT` _(flyout: BitwiseButton)_ | `OperatorButtonStyle` | `12` | `{x:Bind BitwiseButton.IsEnabled, Mode=OneWay}` | (0,2) | 1x1 | ControlRoot>ProgRadixOps>BitwiseGrid | CalculatorProgrammerRadixOperators.xaml:295 |
| 7 | `RotateCircularButton` | `` | `{utils:ResourceString Name=rotateCircularButton/Content}` _(flyout: BitShiftButton)_ | `` | `` | `(default true)` | (0,2) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:376 |
| 8 | `NandButton` | `Nand` | `NAND` _(flyout: BitwiseButton)_ | `OperatorButtonStyle` | `12` | `{x:Bind BitwiseButton.IsEnabled, Mode=OneWay}` | (0,3) | 1x1 | ControlRoot>ProgRadixOps>BitwiseGrid | CalculatorProgrammerRadixOperators.xaml:308 |
| 9 | `RotateCarryShiftButton` | `` | `{utils:ResourceString Name=rotateCarryShiftButton/Content}` _(flyout: BitShiftButton)_ | `` | `` | `(default true)` | (0,3) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:380 |
| 10 | `NorButton` | `Nor` | `NOR` _(flyout: BitwiseButton)_ | `OperatorButtonStyle` | `12` | `{x:Bind BitwiseButton.IsEnabled, Mode=OneWay}` | (1,1) | 1x1 | ControlRoot>ProgRadixOps>BitwiseGrid | CalculatorProgrammerRadixOperators.xaml:322 |
| 11 | `AButton` | `A` | `A` | `NumericButtonStyle12` | `` | `{x:Bind ViewModel.AreHEXButtonsEnabled, Mode=OneWay}` | (1,1) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:656 |
| 12 | `XorButton` | `Xor` | `XOR` _(flyout: BitwiseButton)_ | `OperatorButtonStyle` | `12` | `{x:Bind BitwiseButton.IsEnabled, Mode=OneWay}` | (1,2) | 1x1 | ControlRoot>ProgRadixOps>BitwiseGrid | CalculatorProgrammerRadixOperators.xaml:337 |
| 13 | `RolButton` | `Rol` |  `U+F88E` | `SymbolOperatorButtonStyle` | `12` | `(default true)` | (1,2) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:399 |
| 14 | `RorButton` | `Ror` |  `U+F88F` | `SymbolOperatorButtonStyle` | `12` | `(default true)` | (1,3) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:410 |
| 15 | `RolCarryButton` | `RolC` |  `U+F88E` | `SymbolOperatorButtonStyle` | `12` | `(default true)` | (1,4) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:422 |
| 16 | `ClearButton` | `Clear` | `C` | `OperatorButtonStyle` | `12` | `(default true)` | (1,4) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:497 |
| 17 | `RorCarryButton` | `RorC` |  `U+F88F` | `SymbolOperatorButtonStyle` | `12` | `(default true)` | (1,5) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:433 |
| 18 | `ClearEntryButton` | `ClearEntry` | `CE` | `OperatorButtonStyle` | `12` | `(default true)` | (1,5) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:508 |
| 19 | `LshButton` | `Lsh` |  `U+F88E` | `SymbolOperatorButtonStyle` | `12` | `(default true)` | (1,6) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:445 |
| 20 | `BackSpaceButton` | `Backspace` |  `U+E94F` | `SymbolOperatorButtonStyle` | `12` | `(default true)` | (1,6) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:519 |
| 21 | `RshButton` | `Rsh` |  `U+F88F` | `SymbolOperatorButtonStyle` | `12` | `(default true)` | (1,7) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:454 |
| 22 | `LshLogicalButton` | `Lsh` |  `U+F88E` | `SymbolOperatorButtonStyle` | `12` | `(default true)` | (1,8) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:465 |
| 23 | `RshLogicalButton` | `RshL` |  `U+F88F` | `SymbolOperatorButtonStyle` | `12` | `(default true)` | (1,9) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:474 |
| 24 | `BButton` | `B` | `B` | `NumericButtonStyle12` | `` | `{x:Bind ViewModel.AreHEXButtonsEnabled, Mode=OneWay}` | (2,1) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:668 |
| 25 | `OpenParenthesisButton` | `OpenParenthesis` | `(` | `ParenthesisCalcButtonStyle` | `18` | `(default true)` | (2,2) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:530 |
| 26 | `CloseParenthesisButton` | `CloseParenthesis` | `)` | `OperatorButtonStyle` | `18` | `(default true)` | (2,3) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:543 |
| 27 | `ModButton` | `Mod` | `%` | `OperatorButtonStyle` | `12` | `(default true)` | (2,4) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:554 |
| 28 | `DivideButton` | `Divide` |  `U+E94A` | `SymbolOperatorButtonStyle` | `12` | `(default true)` | (2,5) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:578 |
| 29 | `MultiplyButton` | `Multiply` |  `U+E947` | `SymbolOperatorButtonStyle` | `12` | `(default true)` | (2,6) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:589 |
| 30 | `MinusButton` | `Subtract` |  `U+E949` | `SymbolOperatorButtonStyle` | `12` | `(default true)` | (2,7) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:600 |
| 31 | `PlusButton` | `Add` |  `U+E948` | `SymbolOperatorButtonStyle` | `12` | `(default true)` | (2,8) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:611 |
| 32 | `EqualButton` | `Equals` |  `U+E94E` | `AccentEmphasizedCalcButtonStyle` | `12` | `(default true)` | (2,9) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:622 |
| 33 | `CButton` | `C` | `C` | `NumericButtonStyle12` | `` | `{x:Bind ViewModel.AreHEXButtonsEnabled, Mode=OneWay}` | (3,1) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:680 |
| 34 | `DButton` | `D` | `D` | `NumericButtonStyle12` | `` | `{x:Bind ViewModel.AreHEXButtonsEnabled, Mode=OneWay}` | (4,1) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:692 |
| 35 | `EButton` | `E` | `E` | `NumericButtonStyle12` | `` | `{x:Bind ViewModel.AreHEXButtonsEnabled, Mode=OneWay}` | (5,1) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:704 |
| 36 | `FButton` | `F` | `F` | `NumericButtonStyle12` | `` | `{x:Bind ViewModel.AreHEXButtonsEnabled, Mode=OneWay}` | (6,1) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:716 |
| 37 | `NegateButton` | `Negate` |  `U+F898` | `SymbolOperatorKeypadButtonStyle` | `12` | `(default true)` | (6,2) | 1x1 | ControlRoot>ProgRadixOps | CalculatorProgrammerRadixOperators.xaml:737 |

#### Physical layout reconstruction (main panel, 7 rows x 10 cols; cells = x:Name)

| | c0 | c1 | c2 | c3 | c4 | c5 | c6 | c7 | c8 | c9 |
|---|---|---|---|---|---|---|---|---|---|---|
| r0 | BitwiseButton | BitShiftButton |  |  |  |  |  |  |  |  |
| r1 |  | AButton | RolButton | RorButton | ClearButton | ClearEntryButton | BackSpaceButton | RshButton | LshLogicalButton | RshLogicalButton |
| r2 |  | BButton | OpenParenthesisButton | CloseParenthesisButton | ModButton | DivideButton | MultiplyButton | MinusButton | PlusButton | EqualButton |
| r3 |  | CButton |  |  |  |  |  |  |  |  |
| r4 |  | DButton |  |  |  |  |  |  |  |  |
| r5 |  | EButton |  |  |  |  |  |  |  |  |
| r6 |  | FButton | NegateButton |  |  |  |  |  |  |  |

_Absolute coordinates already fold in each parent grid's own `(row,col)` offset._

#### Styles referenced (7 distinct)

| Style key | Defined in file? | BasedOn | TargetType | file:line |
|---|---|---|---|---|
| `AccentEmphasizedCalcButtonStyle` | no (external dictionary) | – | – | – |
| `NumericButtonStyle12` | no (external dictionary) | – | – | – |
| `OperatorButtonStyle` | no (external dictionary) | – | – | – |
| `OperatorPanelButtonStyle` | no (external dictionary) | – | – | – |
| `ParenthesisCalcButtonStyle` | no (external dictionary) | – | – | – |
| `SymbolOperatorButtonStyle` | no (external dictionary) | – | – | – |
| `SymbolOperatorKeypadButtonStyle` | no (external dictionary) | – | – | – |

### NumberPad.xaml

Source: `src/Calculator/Views/NumberPad.xaml`

**Buttons: 11** (main panel: 11, flyout: 0)

| # | x:Name | ButtonId | Glyph (code points) | Style | FontSize | IsEnabled binding | Abs (row,col) | Span | Grid chain | file:line |
|---|---|---|---|---|---|---|---|---|---|---|
| 0 | `Num1Button` | `One` | (none) | `ButtonStyle` | `` | `(default true)` | (0,0) | 1x1 | ControlRoot>Root | NumberPad.xaml:47 |
| 1 | `Num4Button` | `Four` | (none) | `ButtonStyle` | `` | `(default true)` | (0,1) | 1x1 | ControlRoot>Root | NumberPad.xaml:70 |
| 2 | `Num7Button` | `Seven` | (none) | `ButtonStyle` | `` | `(default true)` | (0,2) | 1x1 | ControlRoot>Root | NumberPad.xaml:93 |
| 3 | `Num8Button` | `Eight` | (none) | `ButtonStyle` | `` | `(default true)` | (0,3) | 1x1 | ControlRoot>Root | NumberPad.xaml:99 |
| 4 | `Num9Button` | `Nine` | (none) | `ButtonStyle` | `` | `(default true)` | (0,4) | 1x1 | ControlRoot>Root | NumberPad.xaml:106 |
| 5 | `Num5Button` | `Five` | (none) | `ButtonStyle` | `` | `(default true)` | (1,1) | 1x1 | ControlRoot>Root | NumberPad.xaml:77 |
| 6 | `Num6Button` | `Six` | (none) | `ButtonStyle` | `` | `(default true)` | (1,2) | 1x1 | ControlRoot>Root | NumberPad.xaml:85 |
| 7 | `Num2Button` | `Two` | (none) | `ButtonStyle` | `` | `(default true)` | (2,1) | 1x1 | ControlRoot>Root | NumberPad.xaml:54 |
| 8 | `Num3Button` | `Three` | (none) | `ButtonStyle` | `` | `(default true)` | (2,2) | 1x1 | ControlRoot>Root | NumberPad.xaml:62 |
| 9 | `Num0Button` | `Zero` | (none) | `ButtonStyle` | `` | `(default true)` | (3,1) | 1x1 | ControlRoot>Root | NumberPad.xaml:39 |
| 10 | `DecimalSeparatorButton` | `Decimal` | (none) | `ButtonStyle` | `` | `{Binding IsDecimalEnabled}` | (3,2) | 1x1 | ControlRoot>Root | NumberPad.xaml:114 |

#### Physical layout reconstruction (main panel, 4 rows x 5 cols; cells = x:Name)

| | c0 | c1 | c2 | c3 | c4 |
|---|---|---|---|---|---|
| r0 | Num1Button | Num4Button | Num7Button | Num8Button | Num9Button |
| r1 |  | Num5Button | Num6Button |  |  |
| r2 |  | Num2Button | Num3Button |  |  |
| r3 |  | Num0Button | DecimalSeparatorButton |  |  |

_Absolute coordinates already fold in each parent grid's own `(row,col)` offset._

#### Styles referenced (1 distinct)

| Style key | Defined in file? | BasedOn | TargetType | file:line |
|---|---|---|---|---|
| `ButtonStyle` | no (external dictionary) | – | – | – |

### Memory.xaml

Source: `src/Calculator/Views/Memory.xaml`

**Buttons: 1** (main panel: 1, flyout: 0)

| # | x:Name | ButtonId | Glyph (code points) | Style | FontSize | IsEnabled binding | Abs (row,col) | Span | Grid chain | file:line |
|---|---|---|---|---|---|---|---|---|---|---|
| 0 | `ClearMemory` | `` |  `U+E74D` | `{ThemeResource ClearAllHistoryMemoryButtonStyle}` | `` | `(default true)` | (0,3) | 1x1 | MemoryList>LayoutGrid>MemoryPanel | Memory.xaml:126 |

#### Physical layout reconstruction (main panel, 1 rows x 4 cols; cells = x:Name)

| | c0 | c1 | c2 | c3 |
|---|---|---|---|---|
| r0 |  |  |  | ClearMemory |

_Absolute coordinates already fold in each parent grid's own `(row,col)` offset._

#### Styles referenced (1 distinct)

| Style key | Defined in file? | BasedOn | TargetType | file:line |
|---|---|---|---|---|
| `{ThemeResource ClearAllHistoryMemoryButtonStyle}` | no (external dictionary) | – | – | – |

### MemoryListItem.xaml

Source: `src/Calculator/Views/MemoryListItem.xaml`

**Buttons: 3** (main panel: 3, flyout: 0)

| # | x:Name | ButtonId | Glyph (code points) | Style | FontSize | IsEnabled binding | Abs (row,col) | Span | Grid chain | file:line |
|---|---|---|---|---|---|---|---|---|---|---|
| 0 | `None` | `` | MC `U+004D` `U+0043` | `MemoryHoverButtonStyle` | `` | `(default true)` | (0,1) | 1x1 | MemoryHoverButtons | MemoryListItem.xaml |
| 1 | `None` | `` | M+ `U+004D` `U+002B` | `MemoryHoverButtonStyle` | `` | `(default true)` | (0,2) | 1x1 | MemoryHoverButtons | MemoryListItem.xaml |
| 2 | `None` | `` | M- `U+004D` `U+002D` | `MemoryHoverButtonStyle` | `` | `(default true)` | (0,3) | 1x1 | MemoryHoverButtons | MemoryListItem.xaml |

#### Physical layout reconstruction (main panel, 1 rows x 4 cols; cells = x:Name)

| | c0 | c1 | c2 | c3 |
|---|---|---|---|---|
| r0 |  |  |  |  |

_Absolute coordinates already fold in each parent grid's own `(row,col)` offset._

#### Styles referenced (1 distinct)

| Style key | Defined in file? | BasedOn | TargetType | file:line |
|---|---|---|---|---|
| `MemoryHoverButtonStyle` | no (external dictionary) | – | – | – |

### HistoryList.xaml

Source: `src/Calculator/Views/HistoryList.xaml`

**Buttons: 1** (main panel: 1, flyout: 0)

| # | x:Name | ButtonId | Glyph (code points) | Style | FontSize | IsEnabled binding | Abs (row,col) | Span | Grid chain | file:line |
|---|---|---|---|---|---|---|---|---|---|---|
| 0 | `ClearHistory` | `` | `` | `ClearAllHistoryMemoryButtonStyle` | `` | `(default true)` | (0,3) | 1x1 | LayoutGrid>HistoryListRootGrid | HistoryList.xaml:168 |

#### Physical layout reconstruction (main panel, 1 rows x 4 cols; cells = x:Name)

| | c0 | c1 | c2 | c3 |
|---|---|---|---|---|
| r0 |  |  |  | ClearHistory |

_Absolute coordinates already fold in each parent grid's own `(row,col)` offset._

#### Styles referenced (1 distinct)

| Style key | Defined in file? | BasedOn | TargetType | file:line |
|---|---|---|---|---|
| `ClearAllHistoryMemoryButtonStyle` | no (external dictionary) | – | – | – |

### CalculatorProgrammerBitFlipPanel.xaml

Source: `src/Calculator/Views/CalculatorProgrammerBitFlipPanel.xaml`

**Buttons: 64** (main panel: 64, flyout: 0)

| # | x:Name | ButtonId | Glyph (code points) | Style | FontSize | IsEnabled binding | Abs (row,col) | Span | Grid chain | file:line |
|---|---|---|---|---|---|---|---|---|---|---|
| 0 | `Bit63` | `BINPOS63` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 63), Mode=OneWay}` | (1,1) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:308 |
| 1 | `Bit62` | `BINPOS62` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 62), Mode=OneWay}` | (1,2) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:320 |
| 2 | `Bit61` | `BINPOS61` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 61), Mode=OneWay}` | (1,3) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:332 |
| 3 | `Bit60` | `BINPOS60` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 60), Mode=OneWay}` | (1,4) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:344 |
| 4 | `Bit59` | `BINPOS59` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 59), Mode=OneWay}` | (1,6) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:357 |
| 5 | `Bit58` | `BINPOS58` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 58), Mode=OneWay}` | (1,7) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:369 |
| 6 | `Bit57` | `BINPOS57` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 57), Mode=OneWay}` | (1,8) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:381 |
| 7 | `Bit56` | `BINPOS56` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 56), Mode=OneWay}` | (1,9) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:393 |
| 8 | `Bit55` | `BINPOS55` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 55), Mode=OneWay}` | (1,11) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:406 |
| 9 | `Bit54` | `BINPOS54` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 54), Mode=OneWay}` | (1,12) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:418 |
| 10 | `Bit53` | `BINPOS53` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 53), Mode=OneWay}` | (1,13) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:430 |
| 11 | `Bit52` | `BINPOS52` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 52), Mode=OneWay}` | (1,14) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:442 |
| 12 | `Bit51` | `BINPOS51` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 51), Mode=OneWay}` | (1,16) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:455 |
| 13 | `Bit50` | `BINPOS50` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 50), Mode=OneWay}` | (1,17) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:467 |
| 14 | `Bit49` | `BINPOS49` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 49), Mode=OneWay}` | (1,18) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:479 |
| 15 | `Bit48` | `BINPOS48` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 48), Mode=OneWay}` | (1,19) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:491 |
| 16 | `Bit47` | `BINPOS47` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 47), Mode=OneWay}` | (3,1) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:504 |
| 17 | `Bit46` | `BINPOS46` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 46), Mode=OneWay}` | (3,2) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:516 |
| 18 | `Bit45` | `BINPOS45` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 45), Mode=OneWay}` | (3,3) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:528 |
| 19 | `Bit44` | `BINPOS44` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 44), Mode=OneWay}` | (3,4) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:540 |
| 20 | `Bit43` | `BINPOS43` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 43), Mode=OneWay}` | (3,6) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:553 |
| 21 | `Bit42` | `BINPOS42` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 42), Mode=OneWay}` | (3,7) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:565 |
| 22 | `Bit41` | `BINPOS41` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 41), Mode=OneWay}` | (3,8) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:577 |
| 23 | `Bit40` | `BINPOS40` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 40), Mode=OneWay}` | (3,9) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:589 |
| 24 | `Bit39` | `BINPOS39` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 39), Mode=OneWay}` | (3,11) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:602 |
| 25 | `Bit38` | `BINPOS38` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 38), Mode=OneWay}` | (3,12) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:614 |
| 26 | `Bit37` | `BINPOS37` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 37), Mode=OneWay}` | (3,13) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:626 |
| 27 | `Bit36` | `BINPOS36` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 36), Mode=OneWay}` | (3,14) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:638 |
| 28 | `Bit35` | `BINPOS35` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 35), Mode=OneWay}` | (3,16) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:651 |
| 29 | `Bit34` | `BINPOS34` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 34), Mode=OneWay}` | (3,17) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:663 |
| 30 | `Bit33` | `BINPOS33` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 33), Mode=OneWay}` | (3,18) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:675 |
| 31 | `Bit32` | `BINPOS32` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 32), Mode=OneWay}` | (3,19) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:687 |
| 32 | `Bit31` | `BINPOS31` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 31), Mode=OneWay}` | (5,1) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:700 |
| 33 | `Bit30` | `BINPOS30` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 30), Mode=OneWay}` | (5,2) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:712 |
| 34 | `Bit29` | `BINPOS29` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 29), Mode=OneWay}` | (5,3) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:724 |
| 35 | `Bit28` | `BINPOS28` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 28), Mode=OneWay}` | (5,4) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:736 |
| 36 | `Bit27` | `BINPOS27` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 27), Mode=OneWay}` | (5,6) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:749 |
| 37 | `Bit26` | `BINPOS26` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 26), Mode=OneWay}` | (5,7) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:761 |
| 38 | `Bit25` | `BINPOS25` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 25), Mode=OneWay}` | (5,8) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:773 |
| 39 | `Bit24` | `BINPOS24` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 24), Mode=OneWay}` | (5,9) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:785 |
| 40 | `Bit23` | `BINPOS23` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 23), Mode=OneWay}` | (5,11) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:798 |
| 41 | `Bit22` | `BINPOS22` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 22), Mode=OneWay}` | (5,12) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:810 |
| 42 | `Bit21` | `BINPOS21` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 21), Mode=OneWay}` | (5,13) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:822 |
| 43 | `Bit20` | `BINPOS20` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 20), Mode=OneWay}` | (5,14) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:834 |
| 44 | `Bit19` | `BINPOS19` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 19), Mode=OneWay}` | (5,16) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:847 |
| 45 | `Bit18` | `BINPOS18` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 18), Mode=OneWay}` | (5,17) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:859 |
| 46 | `Bit17` | `BINPOS17` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 17), Mode=OneWay}` | (5,18) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:871 |
| 47 | `Bit16` | `BINPOS16` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 16), Mode=OneWay}` | (5,19) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:883 |
| 48 | `Bit15` | `BINPOS15` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 15), Mode=OneWay}` | (7,1) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:896 |
| 49 | `Bit14` | `BINPOS14` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 14), Mode=OneWay}` | (7,2) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:908 |
| 50 | `Bit13` | `BINPOS13` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 13), Mode=OneWay}` | (7,3) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:920 |
| 51 | `Bit12` | `BINPOS12` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 12), Mode=OneWay}` | (7,4) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:932 |
| 52 | `Bit11` | `BINPOS11` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 11), Mode=OneWay}` | (7,6) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:945 |
| 53 | `Bit10` | `BINPOS10` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 10), Mode=OneWay}` | (7,7) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:957 |
| 54 | `Bit9` | `BINPOS9` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 9), Mode=OneWay}` | (7,8) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:969 |
| 55 | `Bit8` | `BINPOS8` | (none) | `FlippingToggleButtonStyle` | `` | `{x:Bind ShouldEnableBit(ViewModel.ValueBitLength, 8), Mode=OneWay}` | (7,9) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:981 |
| 56 | `Bit7` | `BINPOS7` | (none) | `FlippingToggleButtonStyle` | `` | `(default true)` | (7,11) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:994 |
| 57 | `Bit6` | `BINPOS6` | (none) | `FlippingToggleButtonStyle` | `` | `(default true)` | (7,12) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:1005 |
| 58 | `Bit5` | `BINPOS5` | (none) | `FlippingToggleButtonStyle` | `` | `(default true)` | (7,13) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:1016 |
| 59 | `Bit4` | `BINPOS4` | (none) | `FlippingToggleButtonStyle` | `` | `(default true)` | (7,14) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:1027 |
| 60 | `Bit3` | `BINPOS3` | (none) | `FlippingToggleButtonStyle` | `` | `(default true)` | (7,16) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:1039 |
| 61 | `Bit2` | `BINPOS2` | (none) | `FlippingToggleButtonStyle` | `` | `(default true)` | (7,17) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:1050 |
| 62 | `Bit1` | `BINPOS1` | (none) | `FlippingToggleButtonStyle` | `` | `(default true)` | (7,18) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:1061 |
| 63 | `Bit0` | `BINPOS0` | (none) | `FlippingToggleButtonStyle` | `` | `(default true)` | (7,19) | 1x1 | BitFlip>BitFlipPanel | CalculatorProgrammerBitFlipPanel.xaml:1072 |

#### Physical layout reconstruction (main panel, 8 rows x 20 cols; cells = x:Name)

| | c0 | c1 | c2 | c3 | c4 | c5 | c6 | c7 | c8 | c9 | c10 | c11 | c12 | c13 | c14 | c15 | c16 | c17 | c18 | c19 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| r0 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| r1 |  | Bit63 | Bit62 | Bit61 | Bit60 |  | Bit59 | Bit58 | Bit57 | Bit56 |  | Bit55 | Bit54 | Bit53 | Bit52 |  | Bit51 | Bit50 | Bit49 | Bit48 |
| r2 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| r3 |  | Bit47 | Bit46 | Bit45 | Bit44 |  | Bit43 | Bit42 | Bit41 | Bit40 |  | Bit39 | Bit38 | Bit37 | Bit36 |  | Bit35 | Bit34 | Bit33 | Bit32 |
| r4 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| r5 |  | Bit31 | Bit30 | Bit29 | Bit28 |  | Bit27 | Bit26 | Bit25 | Bit24 |  | Bit23 | Bit22 | Bit21 | Bit20 |  | Bit19 | Bit18 | Bit17 | Bit16 |
| r6 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| r7 |  | Bit15 | Bit14 | Bit13 | Bit12 |  | Bit11 | Bit10 | Bit9 | Bit8 |  | Bit7 | Bit6 | Bit5 | Bit4 |  | Bit3 | Bit2 | Bit1 | Bit0 |

_Absolute coordinates already fold in each parent grid's own `(row,col)` offset._

#### Styles referenced (1 distinct)

| Style key | Defined in file? | BasedOn | TargetType | file:line |
|---|---|---|---|---|
| `FlippingToggleButtonStyle` | yes | `` | ToggleButton | CalculatorProgrammerBitFlipPanel.xaml:17 |


### 2.10 VisualState layout / sizing setters (dynamic layout)


These setters move buttons at runtime, so the static coordinates above are the *base* state. Each is keyed by a `ControlSizeTrigger` / `AdaptiveTrigger`.


#### CalculatorStandardOperators.xaml

| VisualState | Trigger | Setter Target | Value | file:line |
|---|---|---|---|---|
| NoErrorLayout | (none) | `PercentButton.IsEnabled` | `False` | CalculatorStandardOperators.xaml:38 |
| NoErrorLayout | (none) | `SquareRootButton.IsEnabled` | `False` | CalculatorStandardOperators.xaml:39 |
| NoErrorLayout | (none) | `XPower2Button.IsEnabled` | `False` | CalculatorStandardOperators.xaml:40 |
| NoErrorLayout | (none) | `InvertButton.IsEnabled` | `False` | CalculatorStandardOperators.xaml:41 |
| NoErrorLayout | (none) | `DivideButton.IsEnabled` | `False` | CalculatorStandardOperators.xaml:42 |
| NoErrorLayout | (none) | `MultiplyButton.IsEnabled` | `False` | CalculatorStandardOperators.xaml:43 |
| NoErrorLayout | (none) | `MinusButton.IsEnabled` | `False` | CalculatorStandardOperators.xaml:44 |
| NoErrorLayout | (none) | `PlusButton.IsEnabled` | `False` | CalculatorStandardOperators.xaml:45 |
| NoErrorLayout | (none) | `NegateButton.IsEnabled` | `False` | CalculatorStandardOperators.xaml:46 |
| Large | (none) | `PercentButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorStandardOperators.xaml:58 |
| Large | (none) | `SquareRootButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorStandardOperators.xaml:59 |
| Large | (none) | `XPower2Button.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorStandardOperators.xaml:60 |
| Large | (none) | `InvertButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorStandardOperators.xaml:61 |
| Large | (none) | `NegateButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorStandardOperators.xaml:62 |
| Large | (none) | `ClearEntryButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorStandardOperators.xaml:63 |
| Large | (none) | `ClearButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorStandardOperators.xaml:64 |
| Large | (none) | `BackSpaceButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorStandardOperators.xaml:65 |
| Large | (none) | `DivideButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorStandardOperators.xaml:66 |
| Large | (none) | `MultiplyButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorStandardOperators.xaml:67 |
| Large | (none) | `MinusButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorStandardOperators.xaml:68 |
| Large | (none) | `PlusButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorStandardOperators.xaml:69 |
| Large | (none) | `EqualButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorStandardOperators.xaml:70 |
| Large | (none) | `NumberPad.ButtonStyle` | `{StaticResource NumericButtonStyle38}` | CalculatorStandardOperators.xaml:71 |
| Medium | (none) | `PercentButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorStandardOperators.xaml:82 |
| Medium | (none) | `SquareRootButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorStandardOperators.xaml:83 |
| Medium | (none) | `XPower2Button.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorStandardOperators.xaml:84 |
| Medium | (none) | `InvertButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorStandardOperators.xaml:85 |
| Medium | (none) | `NegateButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSizeLarge}` | CalculatorStandardOperators.xaml:86 |
| Medium | (none) | `ClearEntryButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorStandardOperators.xaml:87 |
| Medium | (none) | `ClearButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorStandardOperators.xaml:88 |
| Medium | (none) | `BackSpaceButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorStandardOperators.xaml:89 |
| Medium | (none) | `DivideButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorStandardOperators.xaml:90 |
| Medium | (none) | `MultiplyButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorStandardOperators.xaml:91 |
| Medium | (none) | `MinusButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorStandardOperators.xaml:92 |
| Medium | (none) | `PlusButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorStandardOperators.xaml:93 |
| Medium | (none) | `EqualButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorStandardOperators.xaml:94 |
| Medium | (none) | `NumberPad.ButtonStyle` | `{StaticResource NumericButtonStyle24}` | CalculatorStandardOperators.xaml:95 |
| Small | AdaptiveTrigger[MinWindowHeight="260" MinWindowWidth="0"] | `PercentButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorStandardOperators.xaml:104 |
| Small | AdaptiveTrigger[MinWindowHeight="260" MinWindowWidth="0"] | `SquareRootButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorStandardOperators.xaml:105 |
| Small | AdaptiveTrigger[MinWindowHeight="260" MinWindowWidth="0"] | `XPower2Button.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorStandardOperators.xaml:106 |
| Small | AdaptiveTrigger[MinWindowHeight="260" MinWindowWidth="0"] | `InvertButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorStandardOperators.xaml:107 |
| Small | AdaptiveTrigger[MinWindowHeight="260" MinWindowWidth="0"] | `NegateButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorStandardOperators.xaml:108 |
| Small | AdaptiveTrigger[MinWindowHeight="260" MinWindowWidth="0"] | `ClearEntryButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorStandardOperators.xaml:109 |
| Small | AdaptiveTrigger[MinWindowHeight="260" MinWindowWidth="0"] | `ClearButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorStandardOperators.xaml:110 |
| Small | AdaptiveTrigger[MinWindowHeight="260" MinWindowWidth="0"] | `BackSpaceButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorStandardOperators.xaml:111 |
| Small | AdaptiveTrigger[MinWindowHeight="260" MinWindowWidth="0"] | `DivideButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorStandardOperators.xaml:112 |
| Small | AdaptiveTrigger[MinWindowHeight="260" MinWindowWidth="0"] | `MultiplyButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorStandardOperators.xaml:113 |
| Small | AdaptiveTrigger[MinWindowHeight="260" MinWindowWidth="0"] | `MinusButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorStandardOperators.xaml:114 |
| Small | AdaptiveTrigger[MinWindowHeight="260" MinWindowWidth="0"] | `PlusButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorStandardOperators.xaml:115 |
| Small | AdaptiveTrigger[MinWindowHeight="260" MinWindowWidth="0"] | `EqualButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorStandardOperators.xaml:116 |
| Small | AdaptiveTrigger[MinWindowHeight="260" MinWindowWidth="0"] | `NumberPad.ButtonStyle` | `{StaticResource NumericButtonStyle18}` | CalculatorStandardOperators.xaml:117 |
| Tiny | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `PercentButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSizeTiny}` | CalculatorStandardOperators.xaml:126 |
| Tiny | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `SquareRootButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSizeTiny}` | CalculatorStandardOperators.xaml:127 |
| Tiny | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `XPower2Button.FontSize` | `{StaticResource CalcStandardOperatorCaptionSizeTiny}` | CalculatorStandardOperators.xaml:128 |
| Tiny | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `InvertButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSizeTiny}` | CalculatorStandardOperators.xaml:129 |
| Tiny | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `NegateButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSizeTiny}` | CalculatorStandardOperators.xaml:130 |
| Tiny | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `ClearEntryButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSizeTiny}` | CalculatorStandardOperators.xaml:131 |
| Tiny | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `ClearButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSizeTiny}` | CalculatorStandardOperators.xaml:132 |
| Tiny | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `BackSpaceButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSizeTiny}` | CalculatorStandardOperators.xaml:133 |
| Tiny | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `DivideButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSizeTiny}` | CalculatorStandardOperators.xaml:134 |
| Tiny | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `MultiplyButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSizeTiny}` | CalculatorStandardOperators.xaml:135 |
| Tiny | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `MinusButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSizeTiny}` | CalculatorStandardOperators.xaml:136 |
| Tiny | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `PlusButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSizeTiny}` | CalculatorStandardOperators.xaml:137 |
| Tiny | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `EqualButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSizeTiny}` | CalculatorStandardOperators.xaml:138 |
| Tiny | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `NumberPad.ButtonStyle` | `{StaticResource NumericButtonStyle12}` | CalculatorStandardOperators.xaml:139 |
| ShowStandardFunctions | AdaptiveTrigger[MinWindowHeight="394" MinWindowWidth="0"] | `R1.Height` | `1*` | CalculatorStandardOperators.xaml:150 |
| ShowStandardFunctions | AdaptiveTrigger[MinWindowHeight="394" MinWindowWidth="0"] | `StandardFunctions.Visibility` | `Visible` | CalculatorStandardOperators.xaml:151 |
| HideStandardFunctions | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `R1.Height` | `0*` | CalculatorStandardOperators.xaml:159 |
| HideStandardFunctions | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `StandardFunctions.Visibility` | `Collapsed` | CalculatorStandardOperators.xaml:160 |
| HideStandardFunctions | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `PercentButton.Visibility` | `Collapsed` | CalculatorStandardOperators.xaml:161 |
| HideStandardFunctions | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `PercentColumn.Width` | `0` | CalculatorStandardOperators.xaml:162 |

#### CalculatorScientificOperators.xaml

| VisualState | Trigger | Setter Target | Value | file:line |
|---|---|---|---|---|
| NoErrorLayout | (none) | `XPower2Button.IsEnabled` | `False` | CalculatorScientificOperators.xaml:47 |
| NoErrorLayout | (none) | `PowerButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:48 |
| NoErrorLayout | (none) | `SinButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:49 |
| NoErrorLayout | (none) | `CosButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:50 |
| NoErrorLayout | (none) | `TanButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:51 |
| NoErrorLayout | (none) | `SinhButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:52 |
| NoErrorLayout | (none) | `CoshButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:53 |
| NoErrorLayout | (none) | `TanhButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:54 |
| NoErrorLayout | (none) | `XPower3Button.IsEnabled` | `False` | CalculatorScientificOperators.xaml:55 |
| NoErrorLayout | (none) | `YSquareRootButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:57 |
| NoErrorLayout | (none) | `InvsinButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:58 |
| NoErrorLayout | (none) | `InvcosButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:59 |
| NoErrorLayout | (none) | `InvtanButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:60 |
| NoErrorLayout | (none) | `InvsinhButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:61 |
| NoErrorLayout | (none) | `InvcoshButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:62 |
| NoErrorLayout | (none) | `InvtanhButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:63 |
| NoErrorLayout | (none) | `SquareRootButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:64 |
| NoErrorLayout | (none) | `PowerOf10Button.IsEnabled` | `False` | CalculatorScientificOperators.xaml:66 |
| NoErrorLayout | (none) | `LogBase10Button.IsEnabled` | `False` | CalculatorScientificOperators.xaml:67 |
| NoErrorLayout | (none) | `ExpButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:68 |
| NoErrorLayout | (none) | `ModButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:69 |
| NoErrorLayout | (none) | `InvertButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:70 |
| NoErrorLayout | (none) | `PowerOfEButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:72 |
| NoErrorLayout | (none) | `LogBaseEButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:73 |
| NoErrorLayout | (none) | `DmsButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:74 |
| NoErrorLayout | (none) | `DegreesButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:75 |
| NoErrorLayout | (none) | `ShiftButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:76 |
| NoErrorLayout | (none) | `DivideButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:78 |
| NoErrorLayout | (none) | `MultiplyButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:80 |
| NoErrorLayout | (none) | `MinusButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:81 |
| NoErrorLayout | (none) | `PlusButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:82 |
| NoErrorLayout | (none) | `PiButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:83 |
| NoErrorLayout | (none) | `FactorialButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:85 |
| NoErrorLayout | (none) | `NegateButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:86 |
| NoErrorLayout | (none) | `OpenParenthesisButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:87 |
| NoErrorLayout | (none) | `CloseParenthesisButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:88 |
| NoErrorLayout | (none) | `TrigButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:89 |
| NoErrorLayout | (none) | `FuncButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:91 |
| NoErrorLayout | (none) | `EulerButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:92 |
| NoErrorLayout | (none) | `AbsButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:93 |
| NoErrorLayout | (none) | `LogBaseY.IsEnabled` | `False` | CalculatorScientificOperators.xaml:94 |
| NoErrorLayout | (none) | `TwoPowerXButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:95 |
| NoErrorLayout | (none) | `CubeRootButton.IsEnabled` | `False` | CalculatorScientificOperators.xaml:96 |
| Large | (none) | `NegateButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSizeExtraLarge}` | CalculatorScientificOperators.xaml:108 |
| Large | (none) | `PiButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:109 |
| Large | (none) | `FactorialButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:110 |
| Large | (none) | `OpenParenthesisButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:111 |
| Large | (none) | `CloseParenthesisButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:112 |
| Large | (none) | `SinButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:113 |
| Large | (none) | `CosButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:115 |
| Large | (none) | `TanButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:116 |
| Large | (none) | `SinhButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:117 |
| Large | (none) | `CoshButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:118 |
| Large | (none) | `TanhButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:119 |
| Large | (none) | `InvsinButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:120 |
| Large | (none) | `InvcosButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:121 |
| Large | (none) | `InvtanButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:122 |
| Large | (none) | `InvsinhButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:123 |
| Large | (none) | `InvcoshButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:124 |
| Large | (none) | `InvtanhButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:125 |
| Large | (none) | `PowerButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:126 |
| Large | (none) | `PowerOf10Button.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:128 |
| Large | (none) | `LogBase10Button.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:129 |
| Large | (none) | `ExpButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:130 |
| Large | (none) | `ModButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:131 |
| Large | (none) | `YSquareRootButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:132 |
| Large | (none) | `PowerOfEButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:133 |
| Large | (none) | `LogBaseEButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:134 |
| Large | (none) | `XPower2Button.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:135 |
| Large | (none) | `XPower3Button.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:136 |
| Large | (none) | `SquareRootButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:137 |
| Large | (none) | `InvertButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:138 |
| Large | (none) | `ClearEntryButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:139 |
| Large | (none) | `ClearButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:141 |
| Large | (none) | `BackSpaceButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:142 |
| Large | (none) | `DivideButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:143 |
| Large | (none) | `MultiplyButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:144 |
| Large | (none) | `MinusButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:145 |
| Large | (none) | `PlusButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:146 |
| Large | (none) | `EqualButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:147 |
| Large | (none) | `NumberPad.ButtonStyle` | `{ThemeResource NumericButtonStyle38}` | CalculatorScientificOperators.xaml:148 |
| Large | (none) | `EulerButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:149 |
| Large | (none) | `AbsButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:151 |
| Large | (none) | `LogBaseY.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:152 |
| Large | (none) | `TwoPowerXButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:153 |
| Large | (none) | `CubeRootButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:154 |
| Large | (none) | `ShiftButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:155 |
| Large | (none) | `TrigShiftButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:157 |
| Large | (none) | `HypButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:158 |
| Large | (none) | `SecButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:159 |
| Large | (none) | `CscButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:160 |
| Large | (none) | `CotButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:161 |
| Large | (none) | `InvsecButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:162 |
| Large | (none) | `InvcscButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:163 |
| Large | (none) | `InvcotButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:164 |
| Large | (none) | `SechButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:165 |
| Large | (none) | `CschButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:166 |
| Large | (none) | `CothButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:167 |
| Large | (none) | `InvsechButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:168 |
| Large | (none) | `InvcschButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:169 |
| Large | (none) | `InvcothButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:170 |
| Large | (none) | `AbsFlyoutButton.FontSize` | `{ThemeResource CalcButtonTextIconCaptionSize}` | CalculatorScientificOperators.xaml:171 |
| Large | (none) | `FloorButton.FontSize` | `{ThemeResource CalcButtonTextIconCaptionSize}` | CalculatorScientificOperators.xaml:173 |
| Large | (none) | `CeilButton.FontSize` | `{ThemeResource CalcButtonTextIconCaptionSize}` | CalculatorScientificOperators.xaml:174 |
| Large | (none) | `RandButton.FontSize` | `{ThemeResource CalcButtonCaptionSize}` | CalculatorScientificOperators.xaml:175 |
| Large | (none) | `DmsButton.FontSize` | `{ThemeResource CalcButtonTextIconCaptionSize}` | CalculatorScientificOperators.xaml:176 |
| Large | (none) | `DegreesButton.FontSize` | `{ThemeResource CalcButtonTextIconCaptionSize}` | CalculatorScientificOperators.xaml:177 |
| Large | (none) | `OperatorPanelRow.MinHeight` | `{StaticResource OperatorPanelButtonRowSizeLarge}` | CalculatorScientificOperators.xaml:178 |
| Large | (none) | `TrigButton.Style` | `{StaticResource OperatorPanelButtonLargeStyle}` | CalculatorScientificOperators.xaml:180 |
| Large | (none) | `FuncButton.Style` | `{StaticResource OperatorPanelButtonLargeStyle}` | CalculatorScientificOperators.xaml:182 |
| Large | (none) | `TrigGrid.MinWidth` | `516` | CalculatorScientificOperators.xaml:183 |
| Large | (none) | `TrigGrid.MinHeight` | `192` | CalculatorScientificOperators.xaml:185 |
| Large | (none) | `FuncGrid.MinWidth` | `387` | CalculatorScientificOperators.xaml:186 |
| Large | (none) | `FuncGrid.MinHeight` | `192` | CalculatorScientificOperators.xaml:187 |
| Medium | (none) | `NegateButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSizeLarge}` | CalculatorScientificOperators.xaml:198 |
| Medium | (none) | `PiButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:199 |
| Medium | (none) | `FactorialButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:200 |
| Medium | (none) | `OpenParenthesisButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:201 |
| Medium | (none) | `CloseParenthesisButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:202 |
| Medium | (none) | `SinButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:203 |
| Medium | (none) | `CosButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:205 |
| Medium | (none) | `TanButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:206 |
| Medium | (none) | `SinhButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:207 |
| Medium | (none) | `CoshButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:208 |
| Medium | (none) | `TanhButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:209 |
| Medium | (none) | `InvsinButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:210 |
| Medium | (none) | `InvcosButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:211 |
| Medium | (none) | `InvtanButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:212 |
| Medium | (none) | `InvsinhButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:213 |
| Medium | (none) | `InvcoshButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:214 |
| Medium | (none) | `InvtanhButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:215 |
| Medium | (none) | `PowerButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:216 |
| Medium | (none) | `PowerOf10Button.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:218 |
| Medium | (none) | `LogBase10Button.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:219 |
| Medium | (none) | `ExpButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:220 |
| Medium | (none) | `ModButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:221 |
| Medium | (none) | `YSquareRootButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:222 |
| Medium | (none) | `PowerOfEButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:223 |
| Medium | (none) | `LogBaseEButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:224 |
| Medium | (none) | `XPower2Button.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:225 |
| Medium | (none) | `XPower3Button.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:226 |
| Medium | (none) | `SquareRootButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:227 |
| Medium | (none) | `InvertButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:228 |
| Medium | (none) | `ClearEntryButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:229 |
| Medium | (none) | `ClearButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:231 |
| Medium | (none) | `BackSpaceButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:232 |
| Medium | (none) | `DivideButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:233 |
| Medium | (none) | `MultiplyButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:234 |
| Medium | (none) | `MinusButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:235 |
| Medium | (none) | `PlusButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:236 |
| Medium | (none) | `EqualButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:237 |
| Medium | (none) | `NumberPad.ButtonStyle` | `{ThemeResource NumericButtonStyle24}` | CalculatorScientificOperators.xaml:238 |
| Medium | (none) | `EulerButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:239 |
| Medium | (none) | `AbsButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:241 |
| Medium | (none) | `LogBaseY.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:242 |
| Medium | (none) | `TwoPowerXButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:243 |
| Medium | (none) | `CubeRootButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:244 |
| Medium | (none) | `ShiftButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:245 |
| Medium | (none) | `TrigShiftButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:247 |
| Medium | (none) | `HypButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:248 |
| Medium | (none) | `SecButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:249 |
| Medium | (none) | `CscButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:250 |
| Medium | (none) | `CotButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:251 |
| Medium | (none) | `InvsecButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:252 |
| Medium | (none) | `InvcscButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:253 |
| Medium | (none) | `InvcotButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:254 |
| Medium | (none) | `SechButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:255 |
| Medium | (none) | `CschButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:256 |
| Medium | (none) | `CothButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:257 |
| Medium | (none) | `InvsechButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:258 |
| Medium | (none) | `InvcschButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:259 |
| Medium | (none) | `InvcothButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:260 |
| Medium | (none) | `AbsFlyoutButton.FontSize` | `{ThemeResource CalcStandardOperatorTextIconCaptionSize}` | CalculatorScientificOperators.xaml:261 |
| Medium | (none) | `FloorButton.FontSize` | `{ThemeResource CalcStandardOperatorTextIconCaptionSize}` | CalculatorScientificOperators.xaml:263 |
| Medium | (none) | `CeilButton.FontSize` | `{ThemeResource CalcStandardOperatorTextIconCaptionSize}` | CalculatorScientificOperators.xaml:264 |
| Medium | (none) | `RandButton.FontSize` | `{ThemeResource CalcStandardOperatorCaptionSize}` | CalculatorScientificOperators.xaml:265 |
| Medium | (none) | `DmsButton.FontSize` | `{ThemeResource CalcStandardOperatorTextIconCaptionSize}` | CalculatorScientificOperators.xaml:266 |
| Medium | (none) | `DegreesButton.FontSize` | `{ThemeResource CalcStandardOperatorTextIconCaptionSize}` | CalculatorScientificOperators.xaml:267 |
| Medium | (none) | `OperatorPanelRow.MinHeight` | `{StaticResource OperatorPanelButtonRowSizeMedium}` | CalculatorScientificOperators.xaml:268 |
| Medium | (none) | `TrigButton.Style` | `{StaticResource OperatorPanelButtonMediumStyle}` | CalculatorScientificOperators.xaml:270 |
| Medium | (none) | `FuncButton.Style` | `{StaticResource OperatorPanelButtonMediumStyle}` | CalculatorScientificOperators.xaml:272 |
| Medium | (none) | `TrigGrid.MinWidth` | `480` | CalculatorScientificOperators.xaml:273 |
| Medium | (none) | `TrigGrid.MinHeight` | `144` | CalculatorScientificOperators.xaml:275 |
| Medium | (none) | `FuncGrid.MinWidth` | `360` | CalculatorScientificOperators.xaml:276 |
| Medium | (none) | `FuncGrid.MinHeight` | `144` | CalculatorScientificOperators.xaml:277 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `NegateButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:286 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `PiButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:287 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `FactorialButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:288 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `OpenParenthesisButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:289 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `CloseParenthesisButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:290 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `SinButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:291 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `CosButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:293 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `TanButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:294 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `SinhButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:295 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `CoshButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:296 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `TanhButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:297 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `InvsinButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:298 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `InvcosButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:299 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `InvtanButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:300 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `InvsinhButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:301 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `InvcoshButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:302 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `InvtanhButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:303 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `PowerButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:304 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `PowerOf10Button.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:306 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `LogBase10Button.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:307 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `ExpButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:308 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `ModButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:309 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `YSquareRootButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:310 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `PowerOfEButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:311 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `LogBaseEButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:312 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `XPower2Button.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:313 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `XPower3Button.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:314 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `SquareRootButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:315 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `InvertButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:316 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `ClearEntryButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:317 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `ClearButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:319 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `BackSpaceButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:320 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `DivideButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:321 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `MultiplyButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:322 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `MinusButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:323 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `PlusButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:324 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `EqualButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:325 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `NumberPad.ButtonStyle` | `{ThemeResource NumericButtonStyle18}` | CalculatorScientificOperators.xaml:326 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `EulerButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:327 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `AbsButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:329 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `LogBaseY.FontSize` | `16` | CalculatorScientificOperators.xaml:330 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `TwoPowerXButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:331 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `CubeRootButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:332 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `ShiftButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:333 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `TrigShiftButton.FontSize` | `{ThemeResource CalcOperatorCaptionSize}` | CalculatorScientificOperators.xaml:335 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `HypButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:336 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `SecButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:337 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `CscButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:338 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `CotButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:339 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `InvsecButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:340 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `InvcscButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:341 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `InvcotButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:342 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `SechButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:343 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `CschButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:344 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `CothButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:345 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `InvsechButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:346 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `InvcschButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:347 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `InvcothButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:348 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `AbsFlyoutButton.FontSize` | `{ThemeResource CalcOperatorTextIconCaptionSize}` | CalculatorScientificOperators.xaml:349 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `FloorButton.FontSize` | `{ThemeResource CalcOperatorTextIconCaptionSize}` | CalculatorScientificOperators.xaml:351 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `CeilButton.FontSize` | `{ThemeResource CalcOperatorTextIconCaptionSize}` | CalculatorScientificOperators.xaml:352 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `RandButton.FontSize` | `{ThemeResource CalcOperatorCaptionSizeSmall}` | CalculatorScientificOperators.xaml:353 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `DmsButton.FontSize` | `{ThemeResource CalcOperatorTextIconCaptionSize}` | CalculatorScientificOperators.xaml:354 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `DegreesButton.FontSize` | `{ThemeResource CalcOperatorTextIconCaptionSize}` | CalculatorScientificOperators.xaml:355 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `OperatorPanelRow.MinHeight` | `{ThemeResource OperatorPanelButtonRowSizeSmall}` | CalculatorScientificOperators.xaml:356 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `TrigButton.Style` | `{StaticResource OperatorPanelButtonSmallStyle}` | CalculatorScientificOperators.xaml:358 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `FuncButton.Style` | `{StaticResource OperatorPanelButtonSmallStyle}` | CalculatorScientificOperators.xaml:360 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `TrigGrid.MinWidth` | `258` | CalculatorScientificOperators.xaml:361 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `TrigGrid.MinHeight` | `96` | CalculatorScientificOperators.xaml:363 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `FuncGrid.MinWidth` | `194` | CalculatorScientificOperators.xaml:364 |
| Small | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `FuncGrid.MinHeight` | `96` | CalculatorScientificOperators.xaml:365 |

#### CalculatorProgrammerOperators.xaml

| VisualState | Trigger | Setter Target | Value | file:line |
|---|---|---|---|---|
| Disabled | (none) | `RevealBackplate.Visibility` | `Collapsed` | CalculatorProgrammerOperators.xaml:57 |

#### CalculatorProgrammerRadixOperators.xaml

| VisualState | Trigger | Setter Target | Value | file:line |
|---|---|---|---|---|
| NoErrorLayout | (none) | `RolButton.IsEnabled` | `False` | CalculatorProgrammerRadixOperators.xaml:48 |
| NoErrorLayout | (none) | `RorButton.IsEnabled` | `False` | CalculatorProgrammerRadixOperators.xaml:49 |
| NoErrorLayout | (none) | `LshButton.IsEnabled` | `False` | CalculatorProgrammerRadixOperators.xaml:50 |
| NoErrorLayout | (none) | `RshButton.IsEnabled` | `False` | CalculatorProgrammerRadixOperators.xaml:51 |
| NoErrorLayout | (none) | `OrButton.IsEnabled` | `False` | CalculatorProgrammerRadixOperators.xaml:52 |
| NoErrorLayout | (none) | `XorButton.IsEnabled` | `False` | CalculatorProgrammerRadixOperators.xaml:53 |
| NoErrorLayout | (none) | `NotButton.IsEnabled` | `False` | CalculatorProgrammerRadixOperators.xaml:54 |
| NoErrorLayout | (none) | `AndButton.IsEnabled` | `False` | CalculatorProgrammerRadixOperators.xaml:55 |
| NoErrorLayout | (none) | `ModButton.IsEnabled` | `False` | CalculatorProgrammerRadixOperators.xaml:56 |
| NoErrorLayout | (none) | `DivideButton.IsEnabled` | `False` | CalculatorProgrammerRadixOperators.xaml:58 |
| NoErrorLayout | (none) | `MultiplyButton.IsEnabled` | `False` | CalculatorProgrammerRadixOperators.xaml:60 |
| NoErrorLayout | (none) | `MinusButton.IsEnabled` | `False` | CalculatorProgrammerRadixOperators.xaml:61 |
| NoErrorLayout | (none) | `PlusButton.IsEnabled` | `False` | CalculatorProgrammerRadixOperators.xaml:62 |
| NoErrorLayout | (none) | `OpenParenthesisButton.IsEnabled` | `False` | CalculatorProgrammerRadixOperators.xaml:63 |
| NoErrorLayout | (none) | `CloseParenthesisButton.IsEnabled` | `False` | CalculatorProgrammerRadixOperators.xaml:65 |
| NoErrorLayout | (none) | `NegateButton.IsEnabled` | `False` | CalculatorProgrammerRadixOperators.xaml:66 |
| NoErrorLayout | (none) | `BitShiftButton.IsEnabled` | `False` | CalculatorProgrammerRadixOperators.xaml:67 |
| NoErrorLayout | (none) | `BitwiseButton.IsEnabled` | `False` | CalculatorProgrammerRadixOperators.xaml:68 |
| Large | (none) | `NumberPad.ButtonStyle` | `{StaticResource NumericButtonStyle38}` | CalculatorProgrammerRadixOperators.xaml:80 |
| Large | (none) | `AButton.Style` | `{StaticResource NumericButtonStyle38}` | CalculatorProgrammerRadixOperators.xaml:81 |
| Large | (none) | `BButton.Style` | `{StaticResource NumericButtonStyle38}` | CalculatorProgrammerRadixOperators.xaml:82 |
| Large | (none) | `CButton.Style` | `{StaticResource NumericButtonStyle38}` | CalculatorProgrammerRadixOperators.xaml:83 |
| Large | (none) | `DButton.Style` | `{StaticResource NumericButtonStyle38}` | CalculatorProgrammerRadixOperators.xaml:84 |
| Large | (none) | `EButton.Style` | `{StaticResource NumericButtonStyle38}` | CalculatorProgrammerRadixOperators.xaml:85 |
| Large | (none) | `FButton.Style` | `{StaticResource NumericButtonStyle38}` | CalculatorProgrammerRadixOperators.xaml:86 |
| Large | (none) | `ModButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:87 |
| Large | (none) | `DivideButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:89 |
| Large | (none) | `MultiplyButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:90 |
| Large | (none) | `MinusButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:91 |
| Large | (none) | `PlusButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:92 |
| Large | (none) | `EqualButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:93 |
| Large | (none) | `ClearButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:94 |
| Large | (none) | `ClearEntryButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:95 |
| Large | (none) | `BackSpaceButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:96 |
| Large | (none) | `NegateButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:97 |
| Large | (none) | `OpenParenthesisButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:98 |
| Large | (none) | `CloseParenthesisButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:99 |
| Large | (none) | `AndButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:100 |
| Large | (none) | `OrButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:102 |
| Large | (none) | `XorButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:103 |
| Large | (none) | `NandButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:104 |
| Large | (none) | `NorButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:105 |
| Large | (none) | `RolButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:106 |
| Large | (none) | `RorButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:107 |
| Large | (none) | `RolCarryButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:108 |
| Large | (none) | `RorCarryButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:109 |
| Large | (none) | `LshButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:110 |
| Large | (none) | `RshButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:111 |
| Large | (none) | `LshLogicalButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:112 |
| Large | (none) | `RshLogicalButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:113 |
| Large | (none) | `NotButton.FontSize` | `{StaticResource CalcButtonCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:114 |
| Large | (none) | `OperatorPanelRow.MinHeight` | `{StaticResource OperatorPanelButtonRowSizeLarge}` | CalculatorProgrammerRadixOperators.xaml:115 |
| Large | (none) | `BitwiseButton.Style` | `{StaticResource OperatorPanelButtonLargeStyle}` | CalculatorProgrammerRadixOperators.xaml:117 |
| Large | (none) | `BitShiftButton.Style` | `{StaticResource OperatorPanelButtonLargeStyle}` | CalculatorProgrammerRadixOperators.xaml:119 |
| Large | (none) | `BitwiseGrid.MinWidth` | `387` | CalculatorProgrammerRadixOperators.xaml:120 |
| Large | (none) | `BitwiseGrid.MinHeight` | `192` | CalculatorProgrammerRadixOperators.xaml:122 |
| Medium | (none) | `NumberPad.ButtonStyle` | `{StaticResource NumericButtonStyle24}` | CalculatorProgrammerRadixOperators.xaml:132 |
| Medium | (none) | `AButton.Style` | `{StaticResource NumericButtonStyle24}` | CalculatorProgrammerRadixOperators.xaml:133 |
| Medium | (none) | `BButton.Style` | `{StaticResource NumericButtonStyle24}` | CalculatorProgrammerRadixOperators.xaml:134 |
| Medium | (none) | `CButton.Style` | `{StaticResource NumericButtonStyle24}` | CalculatorProgrammerRadixOperators.xaml:135 |
| Medium | (none) | `DButton.Style` | `{StaticResource NumericButtonStyle24}` | CalculatorProgrammerRadixOperators.xaml:136 |
| Medium | (none) | `EButton.Style` | `{StaticResource NumericButtonStyle24}` | CalculatorProgrammerRadixOperators.xaml:137 |
| Medium | (none) | `FButton.Style` | `{StaticResource NumericButtonStyle24}` | CalculatorProgrammerRadixOperators.xaml:138 |
| Medium | (none) | `ModButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:139 |
| Medium | (none) | `DivideButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:141 |
| Medium | (none) | `MultiplyButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:142 |
| Medium | (none) | `MinusButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:143 |
| Medium | (none) | `PlusButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:144 |
| Medium | (none) | `EqualButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:145 |
| Medium | (none) | `ClearButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:146 |
| Medium | (none) | `ClearEntryButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:147 |
| Medium | (none) | `BackSpaceButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:148 |
| Medium | (none) | `NegateButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSizeLarge}` | CalculatorProgrammerRadixOperators.xaml:149 |
| Medium | (none) | `OpenParenthesisButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:150 |
| Medium | (none) | `CloseParenthesisButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:151 |
| Medium | (none) | `AndButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:152 |
| Medium | (none) | `OrButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:154 |
| Medium | (none) | `XorButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:155 |
| Medium | (none) | `NandButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:156 |
| Medium | (none) | `NorButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:157 |
| Medium | (none) | `RolButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:158 |
| Medium | (none) | `RorButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:159 |
| Medium | (none) | `RolCarryButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:160 |
| Medium | (none) | `RorCarryButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:161 |
| Medium | (none) | `LshButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:162 |
| Medium | (none) | `RshButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:163 |
| Medium | (none) | `LshLogicalButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:164 |
| Medium | (none) | `RshLogicalButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:165 |
| Medium | (none) | `NotButton.FontSize` | `{StaticResource CalcStandardOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:166 |
| Medium | (none) | `OperatorPanelRow.MinHeight` | `{StaticResource OperatorPanelButtonRowSizeMedium}` | CalculatorProgrammerRadixOperators.xaml:167 |
| Medium | (none) | `BitwiseButton.Style` | `{StaticResource OperatorPanelButtonMediumStyle}` | CalculatorProgrammerRadixOperators.xaml:169 |
| Medium | (none) | `BitShiftButton.Style` | `{StaticResource OperatorPanelButtonMediumStyle}` | CalculatorProgrammerRadixOperators.xaml:171 |
| Medium | (none) | `BitwiseGrid.MinWidth` | `416` | CalculatorProgrammerRadixOperators.xaml:172 |
| Medium | (none) | `BitwiseGrid.MinHeight` | `144` | CalculatorProgrammerRadixOperators.xaml:174 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `NumberPad.ButtonStyle` | `{StaticResource NumericButtonStyle18}` | CalculatorProgrammerRadixOperators.xaml:182 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `AButton.Style` | `{StaticResource NumericButtonStyle14}` | CalculatorProgrammerRadixOperators.xaml:183 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `BButton.Style` | `{StaticResource NumericButtonStyle14}` | CalculatorProgrammerRadixOperators.xaml:184 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `CButton.Style` | `{StaticResource NumericButtonStyle14}` | CalculatorProgrammerRadixOperators.xaml:185 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `DButton.Style` | `{StaticResource NumericButtonStyle14}` | CalculatorProgrammerRadixOperators.xaml:186 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `EButton.Style` | `{StaticResource NumericButtonStyle14}` | CalculatorProgrammerRadixOperators.xaml:187 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `FButton.Style` | `{StaticResource NumericButtonStyle14}` | CalculatorProgrammerRadixOperators.xaml:188 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `ModButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:189 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `DivideButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:191 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `MultiplyButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:192 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `MinusButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:193 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `PlusButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:194 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `EqualButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:195 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `ClearButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:196 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `ClearEntryButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:197 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `BackSpaceButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:198 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `NegateButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:199 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `OpenParenthesisButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:200 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `CloseParenthesisButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:201 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `AndButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:202 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `OrButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:204 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `XorButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:205 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `NandButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:206 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `NorButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:207 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `RolButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:208 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `RorButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:209 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `RolCarryButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:210 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `RorCarryButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:211 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `LshButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:212 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `RshButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:213 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `LshLogicalButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:214 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `RshLogicalButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:215 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `NotButton.FontSize` | `{StaticResource CalcOperatorCaptionSize}` | CalculatorProgrammerRadixOperators.xaml:216 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `OperatorPanelRow.MinHeight` | `{StaticResource OperatorPanelButtonRowSizeSmall}` | CalculatorProgrammerRadixOperators.xaml:217 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `BitwiseButton.Style` | `{StaticResource OperatorPanelButtonSmallStyle}` | CalculatorProgrammerRadixOperators.xaml:219 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `BitShiftButton.Style` | `{StaticResource OperatorPanelButtonSmallStyle}` | CalculatorProgrammerRadixOperators.xaml:221 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `BitwiseGrid.MinWidth` | `194` | CalculatorProgrammerRadixOperators.xaml:222 |
| Small | AdaptiveTrigger[MinWindowHeight="{StaticResource AppMinWindowHeight}" MinWindowWidth="{StaticResource AppMinWindowWidth}"] | `BitwiseGrid.MinHeight` | `96` | CalculatorProgrammerRadixOperators.xaml:224 |

#### NumberPad.xaml

| VisualState | Trigger | Setter Target | Value | file:line |
|---|---|---|---|---|
| NoErrorLayout | (none) | `DecimalSeparatorButton.IsEnabled` | `false` | NumberPad.xaml:33 |

#### Memory.xaml

| VisualState | Trigger | Setter Target | Value | file:line |
|---|---|---|---|---|
| NoErrorLayout | (none) | `MemoryListView.IsEnabled` | `False` | Memory.xaml:60 |
| DockedLayout | AdaptiveTrigger[MinWindowWidth="560"] | `MemoryPanel.(Grid.Row)` | `0` | Memory.xaml:70 |
| DockedLayout | AdaptiveTrigger[MinWindowWidth="560"] | `MemoryPanel.(Grid.RowSpan)` | `2` | Memory.xaml:71 |
| DockedLayout | AdaptiveTrigger[MinWindowWidth="560"] | `BackgroundShade.Visibility` | `Collapsed` | Memory.xaml:73 |

#### HistoryList.xaml

| VisualState | Trigger | Setter Target | Value | file:line |
|---|---|---|---|---|
| DockedLayout | AdaptiveTrigger[MinWindowWidth="560"] | `HistoryListRootGrid.(Grid.Row)` | `0` | HistoryList.xaml:109 |
| DockedLayout | AdaptiveTrigger[MinWindowWidth="560"] | `HistoryListRootGrid.(Grid.RowSpan)` | `2` | HistoryList.xaml:110 |
| DockedLayout | AdaptiveTrigger[MinWindowWidth="560"] | `BackgroundShade.Visibility` | `Collapsed` | HistoryList.xaml:112 |

#### CalculatorProgrammerBitFlipPanel.xaml

| VisualState | Trigger | Setter Target | Value | file:line |
|---|---|---|---|---|
| Checked | (none) | `BinaryZero.Visibility` | `Collapsed` | CalculatorProgrammerBitFlipPanel.xaml:57 |
| Checked | (none) | `BinaryOne.Visibility` | `Visible` | CalculatorProgrammerBitFlipPanel.xaml:58 |
| CheckedPointerOver | (none) | `BinaryZero.Visibility` | `Collapsed` | CalculatorProgrammerBitFlipPanel.xaml:68 |
| CheckedPointerOver | (none) | `BinaryOne.Visibility` | `Visible` | CalculatorProgrammerBitFlipPanel.xaml:69 |
| CheckedPressed | (none) | `BinaryZero.Visibility` | `Collapsed` | CalculatorProgrammerBitFlipPanel.xaml:81 |
| CheckedPressed | (none) | `BinaryOne.Visibility` | `Visible` | CalculatorProgrammerBitFlipPanel.xaml:82 |
| DefaultLayout | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `Label60.FontSize` | `10` | CalculatorProgrammerBitFlipPanel.xaml:194 |
| DefaultLayout | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `Label56.FontSize` | `10` | CalculatorProgrammerBitFlipPanel.xaml:195 |
| DefaultLayout | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `Label52.FontSize` | `10` | CalculatorProgrammerBitFlipPanel.xaml:196 |
| DefaultLayout | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `Label48.FontSize` | `10` | CalculatorProgrammerBitFlipPanel.xaml:197 |
| DefaultLayout | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `Label44.FontSize` | `10` | CalculatorProgrammerBitFlipPanel.xaml:198 |
| DefaultLayout | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `Label40.FontSize` | `10` | CalculatorProgrammerBitFlipPanel.xaml:200 |
| DefaultLayout | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `Label36.FontSize` | `10` | CalculatorProgrammerBitFlipPanel.xaml:201 |
| DefaultLayout | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `Label32.FontSize` | `10` | CalculatorProgrammerBitFlipPanel.xaml:202 |
| DefaultLayout | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `Label28.FontSize` | `10` | CalculatorProgrammerBitFlipPanel.xaml:203 |
| DefaultLayout | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `Label24.FontSize` | `10` | CalculatorProgrammerBitFlipPanel.xaml:205 |
| DefaultLayout | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `Label20.FontSize` | `10` | CalculatorProgrammerBitFlipPanel.xaml:206 |
| DefaultLayout | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `Label16.FontSize` | `10` | CalculatorProgrammerBitFlipPanel.xaml:207 |
| DefaultLayout | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `Label12.FontSize` | `10` | CalculatorProgrammerBitFlipPanel.xaml:208 |
| DefaultLayout | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `Label8.FontSize` | `10` | CalculatorProgrammerBitFlipPanel.xaml:210 |
| DefaultLayout | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `Label4.FontSize` | `10` | CalculatorProgrammerBitFlipPanel.xaml:211 |
| DefaultLayout | AdaptiveTrigger[MinWindowHeight="0" MinWindowWidth="0"] | `Label0.FontSize` | `10` | CalculatorProgrammerBitFlipPanel.xaml:212 |


---


## 3. SHORTCUTS

### 3.1 `KeyboardShortcutManager` — class location and attached properties

**Class file:** `src/Calculator/Common/KeyboardShortcutManager.cs` (844 lines) — **NOTE:** it is NOT under `Calculator.ViewModels`; it lives in the `Calculator` (app) project.

`MyVirtualKey` enum: `src/Calculator.ViewModels/Common/MyVirtualKey.cs` (140 lines).

| Attached property | DP name | Type | file:line |
|---|---|---|---|
| `KeyboardShortcutManager.characterProperty` | `characterProperty` | see DP decl | KeyboardShortcutManager.cs:122 |
| `KeyboardShortcutManager.virtualKeyProperty` | `virtualKeyProperty` | see DP decl | KeyboardShortcutManager.cs:142 |
| `KeyboardShortcutManager.virtualKeyControlChordProperty` | `virtualKeyControlChordProperty` | see DP decl | KeyboardShortcutManager.cs:162 |
| `KeyboardShortcutManager.virtualKeyShiftChordProperty` | `virtualKeyShiftChordProperty` | see DP decl | KeyboardShortcutManager.cs:182 |
| `KeyboardShortcutManager.virtualKeyAltChordProperty` | `virtualKeyAltChordProperty` | see DP decl | KeyboardShortcutManager.cs:202 |
| `KeyboardShortcutManager.virtualKeyControlShiftChordProperty` | `virtualKeyControlShiftChordProperty` | see DP decl | KeyboardShortcutManager.cs:222 |

### 3.2 How `Character` maps to a key

`Character` is a `string`. On change it is inserted into a per-view `SortedDictionary<char, List<WeakReference>>` keyed by **`newValue[0]`** — i.e. only the **first character** of the resource string is significant.

The special case is the literal `"."`, which is resolved to the *localized decimal separator*:

```csharp
// KeyboardShortcutManager.cs:414-457
private static void OnCharacterPropertyChanged(DependencyObject target, string oldValue, string newValue)
{
    lock (s_keyboardShortcutMapLockMutex)
    {
        var button = (target as ButtonBase);
        int viewId = Utilities.GetWindowId();
        if (s_characterForButtons.TryGetValue(viewId, out var iterViewMap))
        {
            if (!string.IsNullOrEmpty(oldValue)) { iterViewMap.Remove(oldValue[0]); }
            if (!string.IsNullOrEmpty(newValue))
            {
                if (newValue == ".")
                {
                    char decSep = LocalizationSettings.GetInstance().GetDecimalSeparator();
                    Insert(iterViewMap, decSep, new WeakReference(button));
                }
                else
                {
                    Insert(iterViewMap, newValue[0], new WeakReference(button));
                }
            }
        }
        ... // identical logic for the first-time branch
    }
}
```

Dispatch happens on `CoreWindow.CharacterReceived` and matches the raw char with no case folding:

```csharp
// KeyboardShortcutManager.cs:623-635
private static void OnCharacterReceivedHandler(CoreWindow sender, CharacterReceivedEventArgs args)
{
    int viewId = Utilities.GetWindowId();
    bool hit = s_fHonorShortcuts.TryGetValue(viewId, out var currentHonorShortcuts);
    if (!hit || currentHonorShortcuts)
    {
        char character = ((char)args.KeyCode);
        var buttons = EqualRange(s_characterForButtons[viewId], character);
        KeyboardShortcutManagerLocals.RunFirstEnabledButtonCommand(buttons);
        KeyboardShortcutManagerLocals.LightUpButtons(buttons);
    }
}
```

`VirtualKey` values are `Enum.Parse(typeof(MyVirtualKey), resourceString)` (`src/Calculator/Utils/ResourceVirtualKey.cs:18`) and dispatched in `OnKeyDownHandler` via `GetCurrentKeyDictionary(isControl, isShift, isAlt)` + `EqualRange(lookupMap, (MyVirtualKey)key)` (`KeyboardShortcutManager.cs:680-690`).

### 3.3 Every XAML binding (resolved to its resource value)

**Total bindings: 220** — `Character` × 79, `VirtualKey` × 76, `VirtualKeyControlChord` × 28, `VirtualKeyControlShiftChord` × 13, `VirtualKeyShiftChord` × 24

| # | Property | Resw key | Resolved value | Target x:Name | ButtonId | file:line |
|---|---|---|---|---|---|---|
| 0 | `VirtualKeyControlChord` | `HistoryButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | H | `` | `` | src/Calculator/Views/Calculator.xaml:673 |
| 1 | `VirtualKeyControlChord` | `ClearMemoryButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | L | `` | `` | src/Calculator/Views/Calculator.xaml:741 |
| 2 | `VirtualKeyControlChord` | `MemRecall.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | R | `` | `` | src/Calculator/Views/Calculator.xaml:752 |
| 3 | `VirtualKeyControlChord` | `MemPlus.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | P | `` | `Memory` | src/Calculator/Views/Calculator.xaml:764 |
| 4 | `VirtualKeyControlChord` | `MemMinus.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | Q | `` | `Memory` | src/Calculator/Views/Calculator.xaml:776 |
| 5 | `VirtualKeyControlChord` | `memButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | M | `` | `` | src/Calculator/Views/Calculator.xaml:788 |
| 6 | `VirtualKey` | `qwordButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F2 | `` | `` | src/Calculator/Views/CalculatorProgrammerDisplayPanel.xaml:84 |
| 7 | `VirtualKey` | `dwordButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F3 | `` | `` | src/Calculator/Views/CalculatorProgrammerDisplayPanel.xaml:94 |
| 8 | `VirtualKey` | `wordButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F4 | `` | `` | src/Calculator/Views/CalculatorProgrammerDisplayPanel.xaml:105 |
| 9 | `VirtualKey` | `byteButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F12 | `` | `` | src/Calculator/Views/CalculatorProgrammerDisplayPanel.xaml:116 |
| 10 | `VirtualKey` | `hexButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F5 | `` | `` | src/Calculator/Views/CalculatorProgrammerOperators.xaml:210 |
| 11 | `VirtualKey` | `decimalButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F6 | `` | `` | src/Calculator/Views/CalculatorProgrammerOperators.xaml:222 |
| 12 | `VirtualKey` | `octButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F7 | `` | `` | src/Calculator/Views/CalculatorProgrammerOperators.xaml:235 |
| 13 | `VirtualKey` | `binaryButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F8 | `` | `` | src/Calculator/Views/CalculatorProgrammerOperators.xaml:247 |
| 14 | `Character` | `andButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `&` (U+0026) | `` | `Or` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:271 |
| 15 | `Character` | `orButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `|` (U+007C) | `` | `Not` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:285 |
| 16 | `Character` | `notButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `~` (U+007E) | `` | `Nand` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:299 |
| 17 | `Character` | `nandButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `.` (U+002E) | `` | `Nor` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:312 |
| 18 | `Character` | `norButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `\` (U+005C) | `` | `Xor` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:327 |
| 19 | `Character` | `xorButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `^` (U+005E) | `` | `` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:342 |
| 20 | `Character` | `rolButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `<` (U+003C) | `` | `Ror` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:403 |
| 21 | `Character` | `rorButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `>` (U+003E) | `` | `RolC` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:414 |
| 22 | `Character` | `rolCarryButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `<` (U+003C) | `` | `RorC` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:425 |
| 23 | `Character` | `rorCarryButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `>` (U+003E) | `` | `Lsh` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:437 |
| 24 | `Character` | `lshButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `<` (U+003C) | `` | `Rsh` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:448 |
| 25 | `Character` | `rshButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `>` (U+003E) | `` | `Lsh` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:458 |
| 26 | `Character` | `rshLogicalButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `>` (U+003E) | `` | `Clear` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:478 |
| 27 | `VirtualKey` | `clearButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Escape | `` | `ClearEntry` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:500 |
| 28 | `VirtualKey` | `clearEntryButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Delete | `` | `Backspace` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:511 |
| 29 | `VirtualKey` | `backSpaceButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Back | `` | `OpenParenthesis` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:523 |
| 30 | `Character` | `openParenthesisButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `(` (U+0028) | `` | `CloseParenthesis` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:535 |
| 31 | `Character` | `closeParenthesisButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `)` (U+0029) | `` | `Mod` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:548 |
| 32 | `Character` | `modButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `%` (U+0025) | `` | `Divide` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:559 |
| 33 | `Character` | `divideButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `/` (U+002F) | `` | `Multiply` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:582 |
| 34 | `Character` | `multiplyButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `*` (U+002A) | `` | `Subtract` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:593 |
| 35 | `Character` | `minusButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `-` (U+002D) | `` | `Add` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:604 |
| 36 | `Character` | `plusButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `+` (U+002B) | `` | `Equals` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:615 |
| 37 | `Character` | `equalButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `=` (U+003D) | `` | `A` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:626 |
| 38 | `VirtualKey` | `equalButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Enter | `` | `A` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:627 |
| 39 | `VirtualKey` | `aButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | A | `` | `B` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:660 |
| 40 | `VirtualKeyShiftChord` | `aButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | A | `` | `B` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:661 |
| 41 | `VirtualKey` | `bButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | B | `` | `C` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:672 |
| 42 | `VirtualKeyShiftChord` | `bButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | B | `` | `C` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:673 |
| 43 | `VirtualKey` | `cButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | C | `` | `D` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:684 |
| 44 | `VirtualKeyShiftChord` | `cButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | C | `` | `D` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:685 |
| 45 | `VirtualKey` | `dButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | D | `` | `E` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:696 |
| 46 | `VirtualKeyShiftChord` | `dButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | D | `` | `E` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:697 |
| 47 | `VirtualKey` | `eButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | E | `` | `F` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:708 |
| 48 | `VirtualKeyShiftChord` | `eButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | E | `` | `F` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:709 |
| 49 | `VirtualKey` | `fButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F | `` | `Negate` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:720 |
| 50 | `VirtualKeyShiftChord` | `fButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | F | `` | `Negate` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:721 |
| 51 | `VirtualKey` | `negateButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F9 | `` | `` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml:743 |
| 52 | `VirtualKey` | `degButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F4 | `` | `` | src/Calculator/Views/CalculatorScientificAngleButtons.xaml:77 |
| 53 | `VirtualKey` | `radButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F5 | `` | `` | src/Calculator/Views/CalculatorScientificAngleButtons.xaml:85 |
| 54 | `VirtualKey` | `gradButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F3 | `` | `` | src/Calculator/Views/CalculatorScientificAngleButtons.xaml:94 |
| 55 | `VirtualKey` | `ftoeButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | V | `` | `` | src/Calculator/Views/CalculatorScientificAngleButtons.xaml:105 |
| 56 | `VirtualKey` | `sinButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | S | `` | `Cos` | src/Calculator/Views/CalculatorScientificOperators.xaml:451 |
| 57 | `VirtualKey` | `cosButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | O | `` | `Tan` | src/Calculator/Views/CalculatorScientificOperators.xaml:464 |
| 58 | `VirtualKey` | `tanButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | T | `` | `Sec` | src/Calculator/Views/CalculatorScientificOperators.xaml:477 |
| 59 | `VirtualKey` | `secButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | U | `` | `Csc` | src/Calculator/Views/CalculatorScientificOperators.xaml:490 |
| 60 | `VirtualKey` | `cscButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | I | `` | `Cot` | src/Calculator/Views/CalculatorScientificOperators.xaml:504 |
| 61 | `VirtualKey` | `cotButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | J | `` | `InvSin` | src/Calculator/Views/CalculatorScientificOperators.xaml:518 |
| 62 | `VirtualKeyShiftChord` | `invsinButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | S | `` | `InvCos` | src/Calculator/Views/CalculatorScientificOperators.xaml:546 |
| 63 | `VirtualKeyShiftChord` | `invcosButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | O | `` | `InvTan` | src/Calculator/Views/CalculatorScientificOperators.xaml:559 |
| 64 | `VirtualKeyShiftChord` | `invtanButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | T | `` | `InvSec` | src/Calculator/Views/CalculatorScientificOperators.xaml:572 |
| 65 | `VirtualKeyShiftChord` | `invsecButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | U | `` | `InvCsc` | src/Calculator/Views/CalculatorScientificOperators.xaml:585 |
| 66 | `VirtualKeyShiftChord` | `invcscButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | I | `` | `InvCot` | src/Calculator/Views/CalculatorScientificOperators.xaml:599 |
| 67 | `VirtualKeyShiftChord` | `invcotButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | J | `` | `Sinh` | src/Calculator/Views/CalculatorScientificOperators.xaml:613 |
| 68 | `VirtualKeyControlChord` | `sinhButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | S | `` | `Cosh` | src/Calculator/Views/CalculatorScientificOperators.xaml:641 |
| 69 | `VirtualKeyControlChord` | `coshButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | O | `` | `Tanh` | src/Calculator/Views/CalculatorScientificOperators.xaml:654 |
| 70 | `VirtualKeyControlChord` | `tanhButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | T | `` | `Sech` | src/Calculator/Views/CalculatorScientificOperators.xaml:667 |
| 71 | `VirtualKeyControlChord` | `sechButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | U | `` | `Csch` | src/Calculator/Views/CalculatorScientificOperators.xaml:680 |
| 72 | `VirtualKeyControlChord` | `cschButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | I | `` | `Coth` | src/Calculator/Views/CalculatorScientificOperators.xaml:694 |
| 73 | `VirtualKeyControlChord` | `cothButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | J | `` | `InvSinh` | src/Calculator/Views/CalculatorScientificOperators.xaml:708 |
| 74 | `VirtualKeyControlShiftChord` | `invsinhButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlShiftChord` | S | `` | `InvCosh` | src/Calculator/Views/CalculatorScientificOperators.xaml:735 |
| 75 | `VirtualKeyControlShiftChord` | `invcoshButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlShiftChord` | O | `` | `InvTanh` | src/Calculator/Views/CalculatorScientificOperators.xaml:748 |
| 76 | `VirtualKeyControlShiftChord` | `invtanhButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlShiftChord` | T | `` | `InvSech` | src/Calculator/Views/CalculatorScientificOperators.xaml:761 |
| 77 | `VirtualKeyControlShiftChord` | `invsechButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlShiftChord` | U | `` | `InvCsch` | src/Calculator/Views/CalculatorScientificOperators.xaml:774 |
| 78 | `VirtualKeyControlShiftChord` | `invcschButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlShiftChord` | I | `` | `InvCoth` | src/Calculator/Views/CalculatorScientificOperators.xaml:788 |
| 79 | `VirtualKeyControlShiftChord` | `invcothButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlShiftChord` | J | `` | `` | src/Calculator/Views/CalculatorScientificOperators.xaml:802 |
| 80 | `Character` | `absButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `|` (U+007C) | `` | `Floor` | src/Calculator/Views/CalculatorScientificOperators.xaml:845 |
| 81 | `Character` | `floorButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `[` (U+005B) | `` | `Ceil` | src/Calculator/Views/CalculatorScientificOperators.xaml:857 |
| 82 | `Character` | `ceilButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `]` (U+005D) | `` | `Rand` | src/Calculator/Views/CalculatorScientificOperators.xaml:869 |
| 83 | `VirtualKeyShiftChord` | `randButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | R | `` | `DMS` | src/Calculator/Views/CalculatorScientificOperators.xaml:882 |
| 84 | `VirtualKey` | `dmsButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | M | `` | `Degrees` | src/Calculator/Views/CalculatorScientificOperators.xaml:895 |
| 85 | `VirtualKey` | `piButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | P | `` | `Euler` | src/Calculator/Views/CalculatorScientificOperators.xaml:939 |
| 86 | `VirtualKeyShiftChord` | `eulerButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | E | `` | `ClearEntry` | src/Calculator/Views/CalculatorScientificOperators.xaml:950 |
| 87 | `VirtualKey` | `clearEntryButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Delete | `` | `Clear` | src/Calculator/Views/CalculatorScientificOperators.xaml:970 |
| 88 | `VirtualKey` | `clearButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Escape | `` | `Backspace` | src/Calculator/Views/CalculatorScientificOperators.xaml:982 |
| 89 | `VirtualKey` | `backSpaceButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Back | `` | `XPower2` | src/Calculator/Views/CalculatorScientificOperators.xaml:995 |
| 90 | `VirtualKey` | `xpower2Button.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Q | `` | `Sqrt` | src/Calculator/Views/CalculatorScientificOperators.xaml:1019 |
| 91 | `Character` | `squareRootButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `@` (U+0040) | `` | `XPowerY` | src/Calculator/Views/CalculatorScientificOperators.xaml:1028 |
| 92 | `Character` | `powerButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `^` (U+005E) | `` | `TenPowerX` | src/Calculator/Views/CalculatorScientificOperators.xaml:1037 |
| 93 | `VirtualKey` | `powerButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Y | `` | `TenPowerX` | src/Calculator/Views/CalculatorScientificOperators.xaml:1038 |
| 94 | `VirtualKeyControlChord` | `powerOf10Button.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | G | `` | `LogBase10` | src/Calculator/Views/CalculatorScientificOperators.xaml:1048 |
| 95 | `VirtualKey` | `logBase10Button.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | L | `` | `LogBaseE` | src/Calculator/Views/CalculatorScientificOperators.xaml:1057 |
| 96 | `VirtualKey` | `logBaseEButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | N | `` | `Cube` | src/Calculator/Views/CalculatorScientificOperators.xaml:1066 |
| 97 | `Character` | `xpower3Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `#` (U+0023) | `` | `CubeRoot` | src/Calculator/Views/CalculatorScientificOperators.xaml:1085 |
| 98 | `VirtualKey` | `cubeRootButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | B | `` | `YRootX` | src/Calculator/Views/CalculatorScientificOperators.xaml:1095 |
| 99 | `VirtualKeyControlChord` | `ySquareRootButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | Y | `` | `TwoPowerX` | src/Calculator/Views/CalculatorScientificOperators.xaml:1105 |
| 100 | `VirtualKey` | `twoPowerXButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | G | `` | `LogBaseY` | src/Calculator/Views/CalculatorScientificOperators.xaml:1116 |
| 101 | `VirtualKeyShiftChord` | `logBaseY.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | L | `` | `EPowerX` | src/Calculator/Views/CalculatorScientificOperators.xaml:1126 |
| 102 | `VirtualKeyControlChord` | `powerOfEButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | N | `` | `Invert` | src/Calculator/Views/CalculatorScientificOperators.xaml:1136 |
| 103 | `VirtualKey` | `invertButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | R | `` | `Abs` | src/Calculator/Views/CalculatorScientificOperators.xaml:1158 |
| 104 | `Character` | `absButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `|` (U+007C) | `` | `Exp` | src/Calculator/Views/CalculatorScientificOperators.xaml:1167 |
| 105 | `VirtualKey` | `expButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | X | `` | `Mod` | src/Calculator/Views/CalculatorScientificOperators.xaml:1176 |
| 106 | `Character` | `modButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `%` (U+0025) | `` | `OpenParenthesis` | src/Calculator/Views/CalculatorScientificOperators.xaml:1185 |
| 107 | `Character` | `openParenthesisButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `(` (U+0028) | `` | `CloseParenthesis` | src/Calculator/Views/CalculatorScientificOperators.xaml:1198 |
| 108 | `Character` | `closeParenthesisButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `)` (U+0029) | `` | `Factorial` | src/Calculator/Views/CalculatorScientificOperators.xaml:1211 |
| 109 | `Character` | `factorialButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `!` (U+0021) | `` | `Divide` | src/Calculator/Views/CalculatorScientificOperators.xaml:1222 |
| 110 | `Character` | `divideButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `/` (U+002F) | `` | `Multiply` | src/Calculator/Views/CalculatorScientificOperators.xaml:1244 |
| 111 | `Character` | `multiplyButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `*` (U+002A) | `` | `Subtract` | src/Calculator/Views/CalculatorScientificOperators.xaml:1254 |
| 112 | `Character` | `minusButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `-` (U+002D) | `` | `Add` | src/Calculator/Views/CalculatorScientificOperators.xaml:1264 |
| 113 | `Character` | `plusButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `+` (U+002B) | `` | `Equals` | src/Calculator/Views/CalculatorScientificOperators.xaml:1274 |
| 114 | `Character` | `equalButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `=` (U+003D) | `` | `Negate` | src/Calculator/Views/CalculatorScientificOperators.xaml:1284 |
| 115 | `VirtualKey` | `equalButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Enter | `` | `Negate` | src/Calculator/Views/CalculatorScientificOperators.xaml:1285 |
| 116 | `VirtualKey` | `negateButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F9 | `` | `` | src/Calculator/Views/CalculatorScientificOperators.xaml:1298 |
| 117 | `Character` | `percentButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `%` (U+0025) | `` | `ClearEntry` | src/Calculator/Views/CalculatorStandardOperators.xaml:188 |
| 118 | `VirtualKey` | `clearEntryButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Delete | `` | `Clear` | src/Calculator/Views/CalculatorStandardOperators.xaml:197 |
| 119 | `VirtualKey` | `clearButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Escape | `` | `Backspace` | src/Calculator/Views/CalculatorStandardOperators.xaml:206 |
| 120 | `VirtualKey` | `backSpaceButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Back | `` | `Invert` | src/Calculator/Views/CalculatorStandardOperators.xaml:215 |
| 121 | `VirtualKey` | `invertButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | R | `` | `XPower2` | src/Calculator/Views/CalculatorStandardOperators.xaml:244 |
| 122 | `VirtualKey` | `xpower2Button.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Q | `` | `Sqrt` | src/Calculator/Views/CalculatorStandardOperators.xaml:253 |
| 123 | `Character` | `squareRootButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `@` (U+0040) | `` | `Divide` | src/Calculator/Views/CalculatorStandardOperators.xaml:261 |
| 124 | `Character` | `divideButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `/` (U+002F) | `` | `Multiply` | src/Calculator/Views/CalculatorStandardOperators.xaml:284 |
| 125 | `Character` | `multiplyButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `*` (U+002A) | `` | `Subtract` | src/Calculator/Views/CalculatorStandardOperators.xaml:293 |
| 126 | `Character` | `minusButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `-` (U+002D) | `` | `Add` | src/Calculator/Views/CalculatorStandardOperators.xaml:302 |
| 127 | `Character` | `plusButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `+` (U+002B) | `` | `Equals` | src/Calculator/Views/CalculatorStandardOperators.xaml:311 |
| 128 | `Character` | `equalButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `=` (U+003D) | `` | `Negate` | src/Calculator/Views/CalculatorStandardOperators.xaml:320 |
| 129 | `VirtualKey` | `equalButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Enter | `` | `Negate` | src/Calculator/Views/CalculatorStandardOperators.xaml:321 |
| 130 | `VirtualKey` | `negateButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F9 | `` | `` | src/Calculator/Views/CalculatorStandardOperators.xaml:342 |
| 131 | `VirtualKeyControlChord` | `graphViewButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | Home | `` | `` | src/Calculator/Views/GraphingCalculator/GraphingCalculator.xaml:748 |
| 132 | `VirtualKey` | `sinButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | S | `` | `Cos` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:441 |
| 133 | `VirtualKey` | `cosButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | O | `` | `Tan` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:452 |
| 134 | `VirtualKey` | `tanButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | T | `` | `Sec` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:464 |
| 135 | `VirtualKey` | `secButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | U | `` | `Csc` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:476 |
| 136 | `VirtualKey` | `cscButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | I | `` | `Cot` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:489 |
| 137 | `VirtualKey` | `cotButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | J | `` | `InvSin` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:502 |
| 138 | `VirtualKeyShiftChord` | `invsinButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | S | `` | `InvCos` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:529 |
| 139 | `VirtualKeyShiftChord` | `invcosButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | O | `` | `InvTan` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:541 |
| 140 | `VirtualKeyShiftChord` | `invtanButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | T | `` | `InvSec` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:553 |
| 141 | `VirtualKeyShiftChord` | `invsecButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | U | `` | `InvCsc` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:565 |
| 142 | `VirtualKeyShiftChord` | `invcscButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | I | `` | `InvCot` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:578 |
| 143 | `VirtualKeyShiftChord` | `invcotButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | J | `` | `Sinh` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:591 |
| 144 | `VirtualKeyControlChord` | `sinhButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | S | `` | `Cosh` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:618 |
| 145 | `VirtualKeyControlChord` | `coshButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | O | `` | `Tanh` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:630 |
| 146 | `VirtualKeyControlChord` | `tanhButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | T | `` | `Sech` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:642 |
| 147 | `VirtualKeyControlChord` | `sechButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | U | `` | `Csch` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:654 |
| 148 | `VirtualKeyControlChord` | `cschButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | I | `` | `Coth` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:667 |
| 149 | `VirtualKeyControlChord` | `cothButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | J | `` | `InvSinh` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:680 |
| 150 | `VirtualKeyControlShiftChord` | `invsinhButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlShiftChord` | S | `` | `InvCosh` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:706 |
| 151 | `VirtualKeyControlShiftChord` | `invcoshButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlShiftChord` | O | `` | `InvTanh` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:718 |
| 152 | `VirtualKeyControlShiftChord` | `invtanhButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlShiftChord` | T | `` | `InvSech` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:730 |
| 153 | `VirtualKeyControlShiftChord` | `invsechButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlShiftChord` | U | `` | `InvCsch` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:742 |
| 154 | `VirtualKeyControlShiftChord` | `invcschButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlShiftChord` | I | `` | `InvCoth` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:755 |
| 155 | `VirtualKeyControlShiftChord` | `invcothButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlShiftChord` | J | `` | `` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:768 |
| 156 | `Character` | `absButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `|` (U+007C) | `` | `Floor` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:903 |
| 157 | `Character` | `floorButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `[` (U+005B) | `` | `Ceil` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:915 |
| 158 | `Character` | `ceilButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `]` (U+005D) | `` | `Pi` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:927 |
| 159 | `VirtualKey` | `piButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | P | `` | `Euler` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:960 |
| 160 | `VirtualKeyShiftChord` | `eulerButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | E | `` | `Clear` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:973 |
| 161 | `VirtualKey` | `clearButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Escape | `` | `Backspace` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:996 |
| 162 | `VirtualKey` | `backSpaceButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Back | `` | `XPower2` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1009 |
| 163 | `VirtualKey` | `xpower2Button.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Q | `` | `Sqrt` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1035 |
| 164 | `Character` | `squareRootButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `@` (U+0040) | `` | `XPowerY` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1046 |
| 165 | `Character` | `powerButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `^` (U+005E) | `` | `TenPowerX` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1057 |
| 166 | `VirtualKey` | `powerButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Y | `` | `TenPowerX` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1058 |
| 167 | `VirtualKeyControlChord` | `powerOf10Button.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | G | `` | `LogBase10` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1070 |
| 168 | `VirtualKey` | `logBase10Button.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | L | `` | `LogBaseE` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1081 |
| 169 | `VirtualKey` | `logBaseEButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | N | `` | `Cube` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1092 |
| 170 | `Character` | `xpower3Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `#` (U+0023) | `` | `CubeRoot` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1114 |
| 171 | `VirtualKey` | `cubeRootButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | B | `` | `YRootX` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1125 |
| 172 | `VirtualKeyControlChord` | `ySquareRootButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | Y | `` | `TwoPowerX` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1136 |
| 173 | `VirtualKey` | `twoPowerXButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | G | `` | `LogBaseY` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1148 |
| 174 | `VirtualKeyShiftChord` | `logBaseY.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | L | `` | `EPowerX` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1159 |
| 175 | `VirtualKeyControlChord` | `powerOfEButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | N | `` | `Invert` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1170 |
| 176 | `VirtualKey` | `invertButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | R | `` | `Abs` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1193 |
| 177 | `Character` | `absButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `|` (U+007C) | `` | `X` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1204 |
| 178 | `VirtualKey` | `xButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | X | `` | `Y` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1216 |
| 179 | `VirtualKey` | `yButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Y | `` | `OpenParenthesis` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1228 |
| 180 | `Character` | `openParenthesisButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `(` (U+0028) | `` | `CloseParenthesis` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1242 |
| 181 | `Character` | `closeParenthesisButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `)` (U+0029) | `` | `Equals` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1255 |
| 182 | `Character` | `equalButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `=` (U+003D) | `` | `Divide` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1267 |
| 183 | `VirtualKey` | `equalButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Enter | `` | `Divide` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1268 |
| 184 | `Character` | `divideButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `/` (U+002F) | `` | `Multiply` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1292 |
| 185 | `Character` | `multiplyButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `*` (U+002A) | `` | `Subtract` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1304 |
| 186 | `Character` | `minusButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `-` (U+002D) | `` | `Add` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1316 |
| 187 | `Character` | `plusButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `+` (U+002B) | `` | `Negate` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1328 |
| 188 | `VirtualKey` | `negateButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F9 | `` | `Zero` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1354 |
| 189 | `Character` | `num0Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `0` (U+0030) | `` | `One` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1366 |
| 190 | `Character` | `num1Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `1` (U+0031) | `` | `Two` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1376 |
| 191 | `Character` | `num2Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `2` (U+0032) | `` | `Three` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1386 |
| 192 | `Character` | `num3Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `3` (U+0033) | `` | `Four` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1396 |
| 193 | `Character` | `num4Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `4` (U+0034) | `` | `Five` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1406 |
| 194 | `Character` | `num5Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `5` (U+0035) | `` | `Six` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1416 |
| 195 | `Character` | `num6Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `6` (U+0036) | `` | `Seven` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1426 |
| 196 | `Character` | `num7Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `7` (U+0037) | `` | `Eight` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1436 |
| 197 | `Character` | `num8Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `8` (U+0038) | `` | `Nine` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1446 |
| 198 | `Character` | `num9Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `9` (U+0039) | `` | `Decimal` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1456 |
| 199 | `VirtualKey` | `decimalSeparatorButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Decimal | `` | `` | src/Calculator/Views/GraphingCalculator/GraphingNumPad.xaml:1466 |
| 200 | `VirtualKeyControlShiftChord` | `ClearHistory.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlShiftChord` | D | `` | `` | src/Calculator/Views/HistoryList.xaml:171 |
| 201 | `VirtualKeyControlChord` | `copyButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | C | `` | `` | src/Calculator/Views/MainPage.xaml:94 |
| 202 | `VirtualKeyControlChord` | `pasteButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | V | `` | `` | src/Calculator/Views/MainPage.xaml:97 |
| 203 | `VirtualKeyControlChord` | `copyButtonAlternate.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | Insert | `` | `` | src/Calculator/Views/MainPage.xaml:100 |
| 204 | `VirtualKeyShiftChord` | `pasteButtonAlternate.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | Insert | `` | `` | src/Calculator/Views/MainPage.xaml:103 |
| 205 | `Character` | `num0Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `0` (U+0030) | `` | `One` | src/Calculator/Views/NumberPad.xaml:43 |
| 206 | `Character` | `num1Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `1` (U+0031) | `` | `Two` | src/Calculator/Views/NumberPad.xaml:50 |
| 207 | `Character` | `num2Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `2` (U+0032) | `` | `Three` | src/Calculator/Views/NumberPad.xaml:58 |
| 208 | `Character` | `num3Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `3` (U+0033) | `` | `Four` | src/Calculator/Views/NumberPad.xaml:66 |
| 209 | `Character` | `num4Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `4` (U+0034) | `` | `Five` | src/Calculator/Views/NumberPad.xaml:73 |
| 210 | `Character` | `num5Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `5` (U+0035) | `` | `Six` | src/Calculator/Views/NumberPad.xaml:81 |
| 211 | `Character` | `num6Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `6` (U+0036) | `` | `Seven` | src/Calculator/Views/NumberPad.xaml:89 |
| 212 | `Character` | `num7Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `7` (U+0037) | `` | `Eight` | src/Calculator/Views/NumberPad.xaml:95 |
| 213 | `Character` | `num8Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `8` (U+0038) | `` | `Nine` | src/Calculator/Views/NumberPad.xaml:102 |
| 214 | `Character` | `num9Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `9` (U+0039) | `` | `Decimal` | src/Calculator/Views/NumberPad.xaml:109 |
| 215 | `VirtualKey` | `decimalSeparatorButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Decimal | `` | `` | src/Calculator/Views/NumberPad.xaml:119 |
| 216 | `VirtualKey` | `clearEntryButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Delete | `` | `Backspace` | src/Calculator/Views/UnitConverter.xaml:738 |
| 217 | `VirtualKey` | `backSpaceButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Back | `` | `Negate` | src/Calculator/Views/UnitConverter.xaml:747 |
| 218 | `Character` | `converterNegateButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | `-` (U+002D) | `` | `` | src/Calculator/Views/UnitConverter.xaml:770 |
| 219 | `VirtualKey` | `converterNegateButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F9 | `` | `` | src/Calculator/Views/UnitConverter.xaml:771 |

### 3.4 `MyVirtualKey` enum — full listing

`src/Calculator.ViewModels/Common/MyVirtualKey.cs`, lines 6-139. Values are Win32 `VK_*` codes.

Members with explicit values: **128**

| Line | Name | Value | Line | Name | Value |
|---|---|---|---|---|---|
| 8 | `None` | 0 | 74 | `N` | 78 |
| 9 | `LeftButton` | 1 | 75 | `O` | 79 |
| 10 | `RightButton` | 2 | 76 | `P` | 80 |
| 11 | `Cancel` | 3 | 77 | `Q` | 81 |
| 12 | `MiddleButton` | 4 | 78 | `R` | 82 |
| 13 | `XButton1` | 5 | 79 | `S` | 83 |
| 14 | `XButton2` | 6 | 80 | `T` | 84 |
| 15 | `Back` | 8 | 81 | `U` | 85 |
| 16 | `Tab` | 9 | 82 | `V` | 86 |
| 17 | `Clear` | 12 | 83 | `W` | 87 |
| 18 | `Enter` | 13 | 84 | `X` | 88 |
| 19 | `Shift` | 16 | 85 | `Y` | 89 |
| 20 | `Control` | 17 | 86 | `Z` | 90 |
| 21 | `Menu` | 18 | 87 | `LeftWindows` | 91 |
| 22 | `Pause` | 19 | 88 | `RightWindows` | 92 |
| 23 | `CapitalLock` | 20 | 89 | `Application` | 93 |
| 24 | `Kana` | 21 | 90 | `Sleep` | 95 |
| 26 | `Junja` | 23 | 91 | `NumberPad0` | 96 |
| 27 | `Final` | 24 | 92 | `NumberPad1` | 97 |
| 28 | `Hanja` | 25 | 93 | `NumberPad2` | 98 |
| 30 | `Escape` | 27 | 94 | `NumberPad3` | 99 |
| 31 | `Convert` | 28 | 95 | `NumberPad4` | 100 |
| 32 | `NonConvert` | 29 | 96 | `NumberPad5` | 101 |
| 33 | `Accept` | 30 | 97 | `NumberPad6` | 102 |
| 34 | `ModeChange` | 31 | 98 | `NumberPad7` | 103 |
| 35 | `Space` | 32 | 99 | `NumberPad8` | 104 |
| 36 | `PageUp` | 33 | 100 | `NumberPad9` | 105 |
| 37 | `PageDown` | 34 | 101 | `Multiply` | 106 |
| 38 | `End` | 35 | 102 | `Add` | 107 |
| 39 | `Home` | 36 | 103 | `Separator` | 108 |
| 40 | `Left` | 37 | 104 | `Subtract` | 109 |
| 41 | `Up` | 38 | 105 | `Decimal` | 110 |
| 42 | `Right` | 39 | 106 | `Divide` | 111 |
| 43 | `Down` | 40 | 107 | `F1` | 112 |
| 44 | `Select` | 41 | 108 | `F2` | 113 |
| 45 | `Print` | 42 | 109 | `F3` | 114 |
| 46 | `Execute` | 43 | 110 | `F4` | 115 |
| 47 | `Snapshot` | 44 | 111 | `F5` | 116 |
| 48 | `Insert` | 45 | 112 | `F6` | 117 |
| 49 | `Delete` | 46 | 113 | `F7` | 118 |
| 50 | `Help` | 47 | 114 | `F8` | 119 |
| 51 | `Number0` | 48 | 115 | `F9` | 120 |
| 52 | `Number1` | 49 | 116 | `F10` | 121 |
| 53 | `Number2` | 50 | 117 | `F11` | 122 |
| 54 | `Number3` | 51 | 118 | `F12` | 123 |
| 55 | `Number4` | 52 | 119 | `F13` | 124 |
| 56 | `Number5` | 53 | 120 | `F14` | 125 |
| 57 | `Number6` | 54 | 121 | `F15` | 126 |
| 58 | `Number7` | 55 | 122 | `F16` | 127 |
| 59 | `Number8` | 56 | 123 | `F17` | 128 |
| 60 | `Number9` | 57 | 124 | `F18` | 129 |
| 61 | `A` | 65 | 125 | `F19` | 130 |
| 62 | `B` | 66 | 126 | `F20` | 131 |
| 63 | `C` | 67 | 127 | `F21` | 132 |
| 64 | `D` | 68 | 128 | `F22` | 133 |
| 65 | `E` | 69 | 129 | `F23` | 134 |
| 66 | `F` | 70 | 130 | `F24` | 135 |
| 67 | `G` | 71 | 131 | `NumberKeyLock` | 144 |
| 68 | `H` | 72 | 132 | `Scroll` | 145 |
| 69 | `I` | 73 | 133 | `LeftShift` | 160 |
| 70 | `J` | 74 | 134 | `RightShift` | 161 |
| 71 | `K` | 75 | 135 | `LeftControl` | 162 |
| 72 | `L` | 76 | 136 | `RightControl` | 163 |
| 73 | `M` | 77 | 137 | `LeftMenu` | 164 |

### 3.5 Resource-value provenance

Markup extensions: `src/Calculator/Utils/ResourceString.cs` (returns `string`) and `src/Calculator/Utils/ResourceVirtualKey.cs` (returns `MyVirtualKey` via `Enum.Parse`).

Lookup: `AppResourceProvider.GetResourceString(key)` → `src/Calculator.ViewModels/Common/AppResourceProvider.cs:39`.

| .resw file | Bindings resolved |
|---|---|
| `src/Calculator/Resources/en-US/Resources.resw` | 220 |

## 4. OPTIONS / TOGGLE INVENTORY (with defaults)

Source of truth for initial state: the `StandardCalculatorViewModel` constructor, `src/Calculator.ViewModels/StandardCalculatorViewModel.cs:145-168`.

```csharp
// StandardCalculatorViewModel.cs:146-167 (constructor initialisers)
_displayValue = "0";  _decimalDisplayValue = "0";  _hexDisplayValue = "0";
_binaryDisplayValue = "0";  _octalDisplayValue = "0";
_binaryDigits = new List<bool>(new bool[64]);
_expressionTokens = new ObservableCollection<DisplayExpressionToken>();
_memorizedNumbers = new ObservableCollection<MemoryItemViewModel>();
_isMemoryEmpty = true;
_isFToEChecked = false;                 // F-E toggle OFF
_isShiftProgrammerChecked = false;      // programmer 2nd/shift OFF
_valueBitLength = BitLength.BitLengthQWord;   // 64-bit
_isBitFlipChecked = false;              // bit-flip panel OFF
_isBinaryBitFlippingEnabled = false;
_currentRadixType = NumberBase.DecBase; // radix = DECIMAL
_currentAngleType = NumbersAndOperatorsEnum.Degree;  // angle = DEGREES
_openParenthesisCount = 0;
_tokenPosition = -1;
```

| # | Toggle / Option | Backing field | Default | Property | Type | Default file:line |
|---|---|---|---|---|---|---|
| 0 | Angle unit (DEG/RAD/GRAD) | `_currentAngleType` | `NumbersAndOperatorsEnum.Degree` | ``CurrentAngleType` (internal)` | enum | StandardCalculatorViewModel.cs:161 |
| 1 | F-E (engineering notation) | `_isFToEChecked` | `false` | ``IsFToEChecked` (public get/set)` | bool | StandardCalculatorViewModel.cs:155 |
| 2 | F-E availability | `_isFToEEnabled` | `false` (set false on error) | ``IsFToEEnabled` (private set)` | bool | StandardCalculatorViewModel.cs:933 |
| 3 | Programmer shift (2nd) | `_isShiftProgrammerChecked` | `false` | ``IsShiftProgrammerChecked` (private set)` | bool | StandardCalculatorViewModel.cs:156 |
| 4 | Bit flip (QWORD/DWORD/WORD/BYTE grid) | `_isBitFlipChecked` | `false` | ``IsBitFlipChecked` (public set)` | bool | StandardCalculatorViewModel.cs:158 |
| 5 | Binary bit-flip enabled | `_isBinaryBitFlippingEnabled` | `false` | ``IsBinaryBitFlippingEnabled`` | bool | StandardCalculatorViewModel.cs:159 |
| 6 | Radix (HEX/DEC/OCT/BIN) | `_currentRadixType` | `NumberBase.DecBase` (6) | ``CurrentRadixType`` | enum | StandardCalculatorViewModel.cs:160 |
| 7 | Bit length | `_valueBitLength` | `BitLength.BitLengthQWord` (64) | ``ValueBitLength` (public set)` | enum | StandardCalculatorViewModel.cs:157 |
| 8 | Memory empty | `_isMemoryEmpty` | `true` | `(private)` | bool | StandardCalculatorViewModel.cs:154 |
| 9 | Open parenthesis count | `_openParenthesisCount` | `0` | ``OpenParenthesisCount`` | uint | StandardCalculatorViewModel.cs:162 |
| 10 | HEX buttons enabled | `_areHEXButtonsEnabled` | (set on radix change) | ``AreHEXButtonsEnabled`` | bool | StandardCalculatorViewModel.cs:334 |
| 11 | Always on top | `_isAlwaysOnTop` | `false` | ``ApplicationViewModel.IsAlwaysOnTop`` | bool | ApplicationViewModel.cs:42 |
| 12 | Show AlwaysOnTop in menu | `_displayNormalAlwaysOnTopOption` | `false` (then computed) | ``DisplayNormalAlwaysOnTopOption`` | bool | ApplicationViewModel.cs:43 |

### 4.1 Scientific-mode Toggles (XAML-declared, no VM backing)

These are plain `ToggleButton`s whose `IsChecked` is manipulated in code-behind (`src/Calculator/Views/CalculatorScientificOperators.xaml.cs`).

| # | x:Name | Content | Default IsChecked | Rows toggled | file:line |
|---|---|---|---|---|---|
| 0 | `ShiftButton` | `&#xF897;` | **false** (not declared → ToggleButton default) | `Row1` ⇄ `InvRow1` | CalculatorScientificOperators.xaml:921 |
| 1 | `TrigShiftButton` (in `Trigflyout`) | `&#xF897;` | **false** | `TrigFunctions`/`InverseTrigFunctions`/`HyperbolicTrigFunctions`/`InverseHyperbolicTrigFunctions` | CalculatorScientificOperators.xaml:411 |
| 2 | `HypButton` (in `Trigflyout`) | `hyp` | **false** | same 4 trig rows | CalculatorScientificOperators.xaml:422 |

Resolution logic — `CalculatorScientificOperators.xaml.cs:113-135`:

```csharp
bool isShiftChecked = TrigShiftButton.IsChecked.Value;
bool isHypeChecked   = HypButton.IsChecked.Value;
// inverse+hyp -> InverseHyperbolicTrigFunctions
// inverse      -> InverseTrigFunctions
// hyper        -> HyperbolicTrigFunctions
// neither      -> TrigFunctions
```

### 4.2 Programmer radix toggles

| # | x:Name | GroupName | IsChecked default | file:line |
|---|---|---|---|---|
| 0 | `HexButton` | `BaseConversion` | not set → false | CalculatorProgrammerOperators.xaml:207 |
| 1 | `DecimalButton` | `BaseConversion` | **`IsChecked="true"`** | CalculatorProgrammerOperators.xaml:219-229 |
| 2 | `OctButton` | `BaseConversion` | not set → false | CalculatorProgrammerOperators.xaml:231 |
| 3 | `BinaryButton` | `BaseConversion` | not set → false | CalculatorProgrammerOperators.xaml:244 |

### 4.3 Programmer shift-mode RadioButtons (in `BitShiftButton` flyout)

| # | x:Name | IsChecked default | file:line |
|---|---|---|---|
| 0 | `ArithmeticShiftButton` | **true** (set in code-behind at load) | CalculatorProgrammerRadixOperators.xaml.cs:217 |
| 1 | `LogicalShiftButton` | false | CalculatorProgrammerRadixOperators.xaml.cs:221 |
| 2 | `RotateCircularButton` | false | CalculatorProgrammerRadixOperators.xaml.cs:225 |
| 3 | `RotateCarryShiftButton` | false | CalculatorProgrammerRadixOperators.xaml.cs:229 |

### 4.4 Theme

| # | Option | RadioButton x:Name | resw key | Value | file:line |
|---|---|---|---|---|---|
| 0 | Light | `LightThemeRadioButton` | `LightThemeRadioButton.Content` | `Light` | Settings.xaml:98 |
| 1 | Dark | `DarkThemeRadioButton` | `DarkThemeRadioButton.Content` | `Dark` | Settings.xaml:101 |
| 2 | Use system setting | `SystemThemeRadioButton` | `SystemThemeRadioButton.Content` | `Use system setting` | Settings.xaml:104 |

Selection logic — `Settings.xaml.cs:73-74`:

```csharp
var currentTheme = ThemeHelper.RootTheme.ToString();
(ThemeRadioButtons.Items.Cast<RadioButton>().FirstOrDefault(c => c?.Tag?.ToString() == currentTheme)).IsChecked = true;
```

### 4.5 Bit-length buttons (programmer)

| # | x:Name | CommandParameter | Content | Initial Visibility | file:line |
|---|---|---|---|---|---|
| 0 | `QwordButton` | 0 | `QWORD` | visible | CalculatorProgrammerDisplayPanel.xaml:81 |
| 1 | `DwordButton` | 1 | `DWORD` | Collapsed | CalculatorProgrammerDisplayPanel.xaml:91 |
| 2 | `WordButton` | 2 | `WORD` | Collapsed | CalculatorProgrammerDisplayPanel.xaml:101 |
| 3 | `ByteButton` | 3 | `BYTE` | Collapsed | CalculatorProgrammerDisplayPanel.xaml:111 |

### 4.6 `BitLength` / `NumberBase` / `RadixType` enums

`src/Calculator.ViewModels/Common/BitLength.cs:6-13`

| Name | Value |
|---|---|
| `BitLengthUnknown` | -1 |
| `BitLengthByte` | 8 |
| `BitLengthWord` | 16 |
| `BitLengthDWord` | 32 |
| `BitLengthQWord` | 64 |

`src/Calculator.ViewModels/Common/NumberBase.cs:20-27`

| Name | Value |
|---|---|
| `Unknown` | -1 |
| `HexBase` | 5 |
| `DecBase` | 6 |
| `OctBase` | 7 |
| `BinBase` | 8 |

`src/Calculator.ViewModels/Common/RadixType.cs:34-40`

| Name | Ordinal |
|---|---|
| `Hex` | 0 |
| `Decimal` | 1 |
| `Octal` | 2 |
| `Binary` | 3 |

Angle units — `NumbersAndOperatorsEnum` (`src/Calculator.ViewModels/Common/NumbersAndOperatorsEnum.cs`):

| Name | Value | file:line |
|---|---|---|
| `Degree` | 321 | NumbersAndOperatorsEnum.cs:31 |
| `Radians` | 322 | NumbersAndOperatorsEnum.cs:32 |
| `Grads` | 323 | NumbersAndOperatorsEnum.cs:33 |
| `Degrees` | 324 | NumbersAndOperatorsEnum.cs:34 |
| `Hyp` | 325 | NumbersAndOperatorsEnum.cs:90 |


---


## 5. RESOURCE STRINGS


- `Resources.resw` — exists, **274,840 bytes**
- `CEngineStrings.resw` — **EXISTS**, **21,770 bytes**

### Resources.resw — 1166 entries

| # | Name | Value |
|---|---|---|
| 0 | `AppName` | Calculator |
| 1 | `DevAppName` | Calculator [Dev] |
| 2 | `AppStoreName` | Windows Calculator |
| 3 | `DevAppStoreName` | Windows Calculator [Dev] |
| 4 | `AppDescription` | Calculator |
| 5 | `DevAppDescription` | Calculator [Dev] |
| 6 | `AppUpdateResource` | AppUpdate2 |
| 7 | `decimalButton.Tag` | DEC |
| 8 | `binaryButton.Tag` | BIN |
| 9 | `hexButton.Tag` | HEX |
| 10 | `octButton.Tag` | OCT |
| 11 | `copyMenuItem` | Copy |
| 12 | `pasteMenuItem` | Paste |
| 13 | `SupplementaryResultsHeader.Text` | About equal to |
| 14 | `ClearHistory.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlShiftChord` | D |
| 15 | `HistoryButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | H |
| 16 | `memButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | M |
| 17 | `clearButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Escape |
| 18 | `clearEntryButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Delete |
| 19 | `closeParenthesisButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | ) |
| 20 | `cosButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | O |
| 21 | `coshButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | O |
| 22 | `expButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | X |
| 23 | `dmsButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | M |
| 24 | `decimalSeparatorButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Decimal |
| 25 | `divideButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | / |
| 26 | `equalButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | = |
| 27 | `equalButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Enter |
| 28 | `factorialButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | ! |
| 29 | `ftoeButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | V |
| 30 | `invertButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | R |
| 31 | `logBase10Button.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | L |
| 32 | `logBaseEButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | N |
| 33 | `minusButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | - |
| 34 | `modButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | % |
| 35 | `negateButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F9 |
| 36 | `converterNegateButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F9 |
| 37 | `converterNegateButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | - |
| 38 | `num0Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | 0 |
| 39 | `num1Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | 1 |
| 40 | `num2Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | 2 |
| 41 | `num3Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | 3 |
| 42 | `num4Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | 4 |
| 43 | `num5Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | 5 |
| 44 | `num6Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | 6 |
| 45 | `num7Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | 7 |
| 46 | `num8Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | 8 |
| 47 | `num9Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | 9 |
| 48 | `aButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | A |
| 49 | `bButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | B |
| 50 | `cButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | C |
| 51 | `dButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | D |
| 52 | `eButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | E |
| 53 | `fButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F |
| 54 | `aButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | A |
| 55 | `bButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | B |
| 56 | `cButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | C |
| 57 | `dButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | D |
| 58 | `eButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | E |
| 59 | `fButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | F |
| 60 | `openParenthesisButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | ( |
| 61 | `percentButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | % |
| 62 | `piButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | P |
| 63 | `plusButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | + |
| 64 | `powerButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Y |
| 65 | `powerButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | ^ |
| 66 | `powerOf10Button.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | G |
| 67 | `sinButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | S |
| 68 | `sinhButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | S |
| 69 | `squareRootButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | @ |
| 70 | `tanButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | T |
| 71 | `tanhButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | T |
| 72 | `xpower2Button.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Q |
| 73 | `xpower3Button.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | # |
| 74 | `ySquareRootButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | Y |
| 75 | `backSpaceButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Back |
| 76 | `multiplyButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | * |
| 77 | `orButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | \| |
| 78 | `notButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | ~ |
| 79 | `andButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | & |
| 80 | `lshButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | < |
| 81 | `rshButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | > |
| 82 | `xorButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | ^ |
| 83 | `rolButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | < |
| 84 | `rorButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | > |
| 85 | `decimalButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F6 |
| 86 | `hexButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F5 |
| 87 | `octButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F7 |
| 88 | `binaryButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F8 |
| 89 | `degreeButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | D |
| 90 | `invcosButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | O |
| 91 | `invcoshButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlShiftChord` | O |
| 92 | `invsinButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | S |
| 93 | `invsinhButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlShiftChord` | S |
| 94 | `invtanButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | T |
| 95 | `invtanhButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlShiftChord` | T |
| 96 | `powerOfEButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | N |
| 97 | `copyButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | C |
| 98 | `copyButtonAlternate.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | Insert |
| 99 | `pasteButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | V |
| 100 | `pasteButtonAlternate.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | Insert |
| 101 | `BitFlipItemAutomationName` | %1, value %2 |
| 102 | `BitPosition` | %1 bit |
| 103 | `63` | 63rd |
| 104 | `62` | 62nd |
| 105 | `61` | 61st |
| 106 | `60` | 60th |
| 107 | `59` | 59th |
| 108 | `58` | 58th |
| 109 | `57` | 57th |
| 110 | `56` | 56th |
| 111 | `55` | 55th |
| 112 | `54` | 54th |
| 113 | `53` | 53rd |
| 114 | `52` | 52nd |
| 115 | `51` | 51st |
| 116 | `50` | 50th |
| 117 | `49` | 49th |
| 118 | `48` | 48th |
| 119 | `47` | 47th |
| 120 | `46` | 46th |
| 121 | `45` | 45th |
| 122 | `44` | 44th |
| 123 | `43` | 43rd |
| 124 | `42` | 42nd |
| 125 | `41` | 41st |
| 126 | `40` | 40th |
| 127 | `39` | 39th |
| 128 | `38` | 38th |
| 129 | `37` | 37th |
| 130 | `36` | 36th |
| 131 | `35` | 35th |
| 132 | `34` | 34th |
| 133 | `33` | 33rd |
| 134 | `32` | 32nd |
| 135 | `31` | 31st |
| 136 | `30` | 30th |
| 137 | `29` | 29th |
| 138 | `28` | 28th |
| 139 | `27` | 27th |
| 140 | `26` | 26th |
| 141 | `25` | 25th |
| 142 | `24` | 24th |
| 143 | `23` | 23rd |
| 144 | `22` | 22nd |
| 145 | `21` | 21st |
| 146 | `20` | 20th |
| 147 | `19` | 19th |
| 148 | `18` | 18th |
| 149 | `17` | 17th |
| 150 | `16` | 16th |
| 151 | `15` | 15th |
| 152 | `14` | 14th |
| 153 | `13` | 13th |
| 154 | `12` | 12th |
| 155 | `11` | 11th |
| 156 | `10` | 10th |
| 157 | `9` | 9th |
| 158 | `8` | 8th |
| 159 | `7` | 7th |
| 160 | `6` | 6th |
| 161 | `5` | 5th |
| 162 | `4` | 4th |
| 163 | `3` | 3rd |
| 164 | `2` | 2nd |
| 165 | `1` | 1st |
| 166 | `LeastSignificantBit` | least significant bit |
| 167 | `MemoryButton_Open` | Open memory flyout |
| 168 | `MemoryButton_Close` | Close memory flyout |
| 169 | `AlwaysOnTop_Enter` | Keep on top |
| 170 | `AlwaysOnTop_Exit` | Back to full view |
| 171 | `MemoryButton.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Memory |
| 172 | `HistoryButton.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | History (Ctrl+H) |
| 173 | `bitFlip.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Bit toggling keypad |
| 174 | `fullKeypad.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Full keypad |
| 175 | `ClearMemoryButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | L |
| 176 | `ClearMemoryButton.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Clear all memory (Ctrl+L) |
| 177 | `MemRecall.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | R |
| 178 | `MemPlus.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | P |
| 179 | `MemMinus.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | Q |
| 180 | `MemoryLabel.Text` | Memory |
| 181 | `MemoryPivotItem.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Memory |
| 182 | `HistoryLabel.Text` | History |
| 183 | `HistoryPivotItem.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | History |
| 184 | `LeftChevron.Text` | << |
| 185 | `converterModeButton.Content` | Converter |
| 186 | `scientificModeButton.Content` | Scientific |
| 187 | `standardModeButton.Content` | Standard |
| 188 | `converterModeButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Converter mode |
| 189 | `scientificModeButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Scientific mode |
| 190 | `standardModeButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Standard mode |
| 191 | `ClearHistory.Name` | Clear all history |
| 192 | `ClearHistory.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Clear all history |
| 193 | `HideHistory.Name` | Hide |
| 194 | `StandardModeText` | Standard |
| 195 | `ScientificModeText` | Scientific |
| 196 | `ProgrammerModeText` | Programmer |
| 197 | `ConverterModeText` | Converter |
| 198 | `CalculatorModeText` | Calculator |
| 199 | `ConverterModeTextCaps` | Converter |
| 200 | `CalculatorModeTextCaps` | Calculator |
| 201 | `ConverterModePluralText` | Converters |
| 202 | `CalculatorModePluralText` | Calculators |
| 203 | `Format_CalculatorResults` | Display is %1 |
| 204 | `Format_CalculatorAlwaysOnTopResults` | Expression is %1, Current input is %2 |
| 205 | `Format_CalculatorResults_Decimal` | Display is %1 point |
| 206 | `Format_CalculatorExpression` | Expression is %1 |
| 207 | `Display_Copied` | Display value copied to clipboard |
| 208 | `HistoryPane` | History |
| 209 | `MemoryPane` | Memory |
| 210 | `Format_HexButtonValue` | HexaDecimal %1 |
| 211 | `Format_DecButtonValue` | Decimal %1 |
| 212 | `Format_OctButtonValue` | Octal %1 |
| 213 | `Format_BinButtonValue` | Binary %1 |
| 214 | `ClearHistory.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Clear all history |
| 215 | `HistoryList_Cleared` | History cleared |
| 216 | `HideHistory.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Hide history |
| 217 | `HistoryButton_Open` | Open history flyout |
| 218 | `HistoryButton_Close` | Close history flyout |
| 219 | `memButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Memory store |
| 220 | `memButton.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Memory store (Ctrl+M) |
| 221 | `ClearMemoryButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Clear all memory |
| 222 | `Memory_Cleared` | Memory cleared |
| 223 | `MemRecall.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Memory recall |
| 224 | `MemRecall.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Memory recall (Ctrl+R) |
| 225 | `MemPlus.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Memory add |
| 226 | `MemPlus.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Memory add (Ctrl+P) |
| 227 | `MemMinus.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Memory subtract |
| 228 | `MemMinus.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Memory subtract (Ctrl+Q) |
| 229 | `ClearMemoryItemButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Clear memory item |
| 230 | `ClearMemoryItemButton.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Clear memory item |
| 231 | `MemPlusItem.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Add to memory item |
| 232 | `MemPlusItem.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Add to memory item |
| 233 | `MemMinusItem.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Subtract from memory item |
| 234 | `MemMinusItem.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Subtract from memory item |
| 235 | `ClearMemorySwipeItem.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Clear memory item |
| 236 | `ClearMemoryMenuItem.Text` | Clear memory item |
| 237 | `MemPlusSwipeItem.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Add to memory item |
| 238 | `MemPlusMenuItem.Text` | Add to memory item |
| 239 | `MemMinusSwipeItem.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Subtract from memory item |
| 240 | `MemMinusMenuItem.Text` | Subtract from memory item |
| 241 | `DeleteHistorySwipeItem.Text` | Delete |
| 242 | `CopyHistoryMenuItem.Text` | Copy |
| 243 | `DeleteHistoryMenuItem.Text` | Delete |
| 244 | `DeleteHistorySwipeItem.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Delete history item |
| 245 | `DeleteHistoryMenuItem.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Delete history item |
| 246 | `backSpaceButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Backspace |
| 247 | `BinaryZero.Text` | 0 |
| 248 | `BinaryOne.Text` | 1 |
| 249 | `num0Button.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Zero |
| 250 | `num1Button.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | One |
| 251 | `num2Button.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Two |
| 252 | `num3Button.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Three |
| 253 | `num4Button.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Four |
| 254 | `num5Button.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Five |
| 255 | `num6Button.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Six |
| 256 | `num7Button.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Seven |
| 257 | `num8Button.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Eight |
| 258 | `num9Button.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Nine |
| 259 | `aButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | A |
| 260 | `bButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | B |
| 261 | `cButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | C |
| 262 | `dButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | D |
| 263 | `eButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | E |
| 264 | `fButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | F |
| 265 | `andButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | And |
| 266 | `orButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Or |
| 267 | `notButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Not |
| 268 | `rolButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Rotate on left |
| 269 | `rorButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Rotate on right |
| 270 | `lshButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Left shift |
| 271 | `rshButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Right shift |
| 272 | `xorButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Exclusive or |
| 273 | `qwordButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Quadruple Word toggle |
| 274 | `dwordButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Double Word toggle |
| 275 | `wordButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Word toggle |
| 276 | `byteButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Byte toggle |
| 277 | `bitFlip.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Bit toggling keypad |
| 278 | `fullKeypad.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Full keypad |
| 279 | `decimalSeparatorButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Decimal separator |
| 280 | `clearEntryButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Clear entry |
| 281 | `clearButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Clear |
| 282 | `divideButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Divide by |
| 283 | `multiplyButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Multiply by |
| 284 | `equalButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Equals |
| 285 | `shiftButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Inverse function |
| 286 | `minusButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Minus |
| 287 | `minus` | Minus |
| 288 | `plusButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Plus |
| 289 | `squareRootButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Square root |
| 290 | `percentButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Percent |
| 291 | `negateButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Positive negative |
| 292 | `converterNegateButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Positive negative |
| 293 | `invertButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Reciprocal |
| 294 | `openParenthesisButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Left parenthesis |
| 295 | `Format_OpenParenthesisAutomationNamePrefix` | Left parenthesis, open parenthesis count %1 |
| 296 | `closeParenthesisButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Right parenthesis |
| 297 | `Format_OpenParenthesisCountAutomationNamePrefix` | Open parenthesis count %1 |
| 298 | `NoRightParenthesisAdded_Announcement` | There are no open parentheses to close. |
| 299 | `ftoeButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Scientific notation |
| 300 | `hyperbolicButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Hyperbolic function |
| 301 | `piButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Pi |
| 302 | `sinButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Sine |
| 303 | `cosButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Cosine |
| 304 | `tanButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Tangent |
| 305 | `sinhButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Hyperbolic sine |
| 306 | `coshButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Hyperbolic cosine |
| 307 | `tanhButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Hyperbolic tangent |
| 308 | `xpower2Button.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Square |
| 309 | `xpower3Button.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Cube |
| 310 | `invsinButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Arc sine |
| 311 | `invcosButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Arc cosine |
| 312 | `invtanButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Arc tangent |
| 313 | `invsinhButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Hyperbolic arc sine |
| 314 | `invcoshButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Hyperbolic arc cosine |
| 315 | `invtanhButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Hyperbolic arc tangent |
| 316 | `powerButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | 'X' to the exponent |
| 317 | `powerOf10Button.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Ten to the exponent |
| 318 | `powerOfEButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | 'e' to the exponent |
| 319 | `ySquareRootButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | 'y' root of 'x' |
| 320 | `logBase10Button.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Log |
| 321 | `logBaseEButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Natural log |
| 322 | `modButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Modulo |
| 323 | `expButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Exponential |
| 324 | `dmsButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Degree minute second |
| 325 | `degreesButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Degrees |
| 326 | `intButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Integer part |
| 327 | `fractButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Fractional part |
| 328 | `factorialButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Factorial |
| 329 | `degButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Degrees toggle |
| 330 | `gradButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Gradians toggle |
| 331 | `radButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Radians toggle |
| 332 | `FlyoutNav.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Mode dropdown |
| 333 | `Categories.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Categories dropdown |
| 334 | `EnterAlwaysOnTopButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Keep on top |
| 335 | `ExitAlwaysOnTopButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Back to full view |
| 336 | `EnterAlwaysOnTopButton.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Keep on top (Alt+Up) |
| 337 | `ExitAlwaysOnTopButton.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Back to full view (Alt+Down) |
| 338 | `Format_ValueFrom` | Convert from %1 %2 |
| 339 | `Format_ValueFrom_Decimal` | Convert from %1 point %2 |
| 340 | `Format_ValueTo` | Converts into %1 %2 |
| 341 | `Format_ConversionResult` | %1 %2 is %3 %4 |
| 342 | `InputUnit_Name` | Input unit |
| 343 | `OutputUnit_Name` | Output unit |
| 344 | `CategoryName_AreaText` | Area |
| 345 | `CategoryName_DataText` | Data |
| 346 | `CategoryName_EnergyText` | Energy |
| 347 | `CategoryName_LengthText` | Length |
| 348 | `CategoryName_PowerText` | Power |
| 349 | `CategoryName_SpeedText` | Speed |
| 350 | `CategoryName_TimeText` | Time |
| 351 | `CategoryName_VolumeText` | Volume |
| 352 | `CategoryName_TemperatureText` | Temperature |
| 353 | `CategoryName_WeightText` | Weight and mass |
| 354 | `CategoryName_PressureText` | Pressure |
| 355 | `CategoryName_AngleText` | Angle |
| 356 | `CategoryName_CurrencyText` | Currency |
| 357 | `UnitName_FluidOunceUK` | Fluid ounces (UK) |
| 358 | `UnitAbbreviation_FluidOunceUK` | fl oz (UK) |
| 359 | `UnitName_FluidOunceUS` | Fluid ounces (US) |
| 360 | `UnitAbbreviation_FluidOunceUS` | fl oz (US) |
| 361 | `UnitName_GallonUK` | Gallons (UK) |
| 362 | `UnitAbbreviation_GallonUK` | gal (UK) |
| 363 | `UnitName_GallonUS` | Gallons (US) |
| 364 | `UnitAbbreviation_GallonUS` | gal (US) |
| 365 | `UnitName_Liter` | Liters |
| 366 | `UnitAbbreviation_Liter` | L |
| 367 | `UnitName_Milliliter` | Milliliters |
| 368 | `UnitAbbreviation_Milliliter` | mL |
| 369 | `UnitName_PintUK` | Pints (UK) |
| 370 | `UnitAbbreviation_PintUK` | pt (UK) |
| 371 | `UnitName_PintUS` | Pints (US) |
| 372 | `UnitAbbreviation_PintUS` | pt (US) |
| 373 | `UnitName_TablespoonUS` | Tablespoons (US) |
| 374 | `UnitAbbreviation_TablespoonUS` | tbsp. (US) |
| 375 | `UnitName_TeaspoonUS` | Teaspoons (US) |
| 376 | `UnitAbbreviation_TeaspoonUS` | tsp. (US) |
| 377 | `UnitName_TablespoonUK` | Tablespoons (UK) |
| 378 | `UnitAbbreviation_TablespoonUK` | tbsp. (UK) |
| 379 | `UnitName_TeaspoonUK` | Teaspoons (UK) |
| 380 | `UnitAbbreviation_TeaspoonUK` | tsp. (UK) |
| 381 | `UnitName_QuartUK` | Quarts (UK) |
| 382 | `UnitAbbreviation_QuartUK` | qt (UK) |
| 383 | `UnitName_QuartUS` | Quarts (US) |
| 384 | `UnitAbbreviation_QuartUS` | qt (US) |
| 385 | `UnitName_CupUS` | Cups (US) |
| 386 | `UnitAbbreviation_CupUS` | cup (US) |
| 387 | `UnitAbbreviation_Angstrom` | A |
| 388 | `UnitAbbreviation_Acre` | ac |
| 389 | `UnitAbbreviation_Bit` | b |
| 390 | `UnitAbbreviation_BritishThermalUnit` | BTU |
| 391 | `UnitAbbreviation_BTUPerMinute` | BTU/min |
| 392 | `UnitAbbreviation_Byte` | B |
| 393 | `UnitAbbreviation_Calorie` | cal |
| 394 | `UnitAbbreviation_Centimeter` | cm |
| 395 | `UnitAbbreviation_CentimetersPerSecond` | cm/s |
| 396 | `UnitAbbreviation_CubicCentimeter` | cm³ |
| 397 | `UnitAbbreviation_CubicFoot` | ft³ |
| 398 | `UnitAbbreviation_CubicInch` | in³ |
| 399 | `UnitAbbreviation_CubicMeter` | m³ |
| 400 | `UnitAbbreviation_CubicYard` | yd³ |
| 401 | `UnitAbbreviation_Day` | d |
| 402 | `UnitAbbreviation_DegreesCelsius` | °C |
| 403 | `UnitAbbreviation_DegreesFahrenheit` | °F |
| 404 | `UnitAbbreviation_Electron-Volt` | eV |
| 405 | `UnitAbbreviation_Foot` | ft |
| 406 | `UnitAbbreviation_FeetPerSecond` | ft/s |
| 407 | `UnitAbbreviation_Foot-Pound` | ft•lb |
| 408 | `UnitAbbreviation_Gigabit` | Gb |
| 409 | `UnitAbbreviation_Gigabyte` | GB |
| 410 | `UnitAbbreviation_Hectare` | ha |
| 411 | `UnitAbbreviation_Horsepower` | hp (US) |
| 412 | `UnitAbbreviation_Hour` | hr |
| 413 | `UnitAbbreviation_Inch` | in |
| 414 | `UnitAbbreviation_Joule` | J |
| 415 | `UnitAbbreviation_Kilowatthour` | kWh |
| 416 | `UnitAbbreviation_Kelvin` | K |
| 417 | `UnitAbbreviation_Kilobit` | Kb |
| 418 | `UnitAbbreviation_Kilobyte` | KB |
| 419 | `UnitAbbreviation_Kilocalorie` | kcal |
| 420 | `UnitAbbreviation_Kilojoule` | kJ |
| 421 | `UnitAbbreviation_Kilometer` | km |
| 422 | `UnitAbbreviation_KilometersPerHour` | km/h |
| 423 | `UnitAbbreviation_Kilowatt` | kW |
| 424 | `UnitAbbreviation_Knot` | kn |
| 425 | `UnitAbbreviation_Mach` | M |
| 426 | `UnitAbbreviation_Megabit` | Mb |
| 427 | `UnitAbbreviation_Megabyte` | MB |
| 428 | `UnitAbbreviation_Meter` | m |
| 429 | `UnitAbbreviation_MetersPerSecond` | m/s |
| 430 | `UnitAbbreviation_Micron` | µm |
| 431 | `UnitAbbreviation_Microsecond` | µs |
| 432 | `UnitAbbreviation_Mile` | mi |
| 433 | `UnitAbbreviation_MilesPerHour` | mph |
| 434 | `UnitAbbreviation_Millimeter` | mm |
| 435 | `UnitAbbreviation_Millisecond` | ms |
| 436 | `UnitAbbreviation_Minute` | min |
| 437 | `UnitAbbreviation_Nanometer` | nm |
| 438 | `UnitAbbreviation_NauticalMile` | nmi |
| 439 | `UnitAbbreviation_Petabit` | Pb |
| 440 | `UnitAbbreviation_Petabyte` | PB |
| 441 | `UnitAbbreviation_Foot-PoundPerMinute` | ft•lb/min |
| 442 | `UnitAbbreviation_Second` | s |
| 443 | `UnitAbbreviation_SquareCentimeter` | cm² |
| 444 | `UnitAbbreviation_SquareFoot` | ft² |
| 445 | `UnitAbbreviation_SquareInch` | in² |
| 446 | `UnitAbbreviation_SquareKilometer` | km² |
| 447 | `UnitAbbreviation_SquareMeter` | m² |
| 448 | `UnitAbbreviation_SquareMile` | mi² |
| 449 | `UnitAbbreviation_SquareMillimeter` | mm² |
| 450 | `UnitAbbreviation_SquareYard` | yd² |
| 451 | `UnitAbbreviation_Terabit` | Tb |
| 452 | `UnitAbbreviation_Terabyte` | TB |
| 453 | `UnitAbbreviation_Watt` | W |
| 454 | `UnitAbbreviation_Week` | wk |
| 455 | `UnitAbbreviation_Yard` | yd |
| 456 | `UnitAbbreviation_Year` | yr |
| 457 | `UnitAbbreviation_Gibibits` | Gi |
| 458 | `UnitAbbreviation_Gibibytes` | GiB |
| 459 | `UnitAbbreviation_Kibibits` | Ki |
| 460 | `UnitAbbreviation_Kibibytes` | KiB |
| 461 | `UnitAbbreviation_Mebibits` | Mi |
| 462 | `UnitAbbreviation_Mebibytes` | MiB |
| 463 | `UnitAbbreviation_Nibble` | nybl |
| 464 | `UnitAbbreviation_Pebibits` | Pi |
| 465 | `UnitAbbreviation_Pebibytes` | PiB |
| 466 | `UnitAbbreviation_Tebibits` | Ti |
| 467 | `UnitAbbreviation_Tebibytes` | TiB |
| 468 | `UnitAbbreviation_Exabits` | E |
| 469 | `UnitAbbreviation_Exabytes` | EB |
| 470 | `UnitAbbreviation_Exbibits` | Ei |
| 471 | `UnitAbbreviation_Exbibytes` | EiB |
| 472 | `UnitAbbreviation_Zetabits` | Z |
| 473 | `UnitAbbreviation_Zetabytes` | ZB |
| 474 | `UnitAbbreviation_Zebibits` | Zi |
| 475 | `UnitAbbreviation_Zebibytes` | ZiB |
| 476 | `UnitAbbreviation_Yottabit` | Y |
| 477 | `UnitAbbreviation_Yottabyte` | YB |
| 478 | `UnitAbbreviation_Yobibits` | Yi |
| 479 | `UnitAbbreviation_Yobibytes` | YiB |
| 480 | `UnitName_Acre` | Acres |
| 481 | `UnitName_Bit` | Bits |
| 482 | `UnitName_BritishThermalUnit` | British thermal units |
| 483 | `UnitName_BTUPerMinute` | BTUs/minute |
| 484 | `UnitName_Byte` | Bytes |
| 485 | `UnitName_Calorie` | Thermal calories |
| 486 | `UnitName_Centimeter` | Centimeters |
| 487 | `UnitName_CentimetersPerSecond` | Centimeters per second |
| 488 | `UnitName_CubicCentimeter` | Cubic centimeters |
| 489 | `UnitName_CubicFoot` | Cubic feet |
| 490 | `UnitName_CubicInch` | Cubic inches |
| 491 | `UnitName_CubicMeter` | Cubic meters |
| 492 | `UnitName_CubicYard` | Cubic yards |
| 493 | `UnitName_Day` | Days |
| 494 | `UnitName_DegreesCelsius` | Celsius |
| 495 | `UnitName_DegreesFahrenheit` | Fahrenheit |
| 496 | `UnitName_Electron-Volt` | Electron volts |
| 497 | `UnitName_Foot` | Feet |
| 498 | `UnitName_FeetPerSecond` | Feet per second |
| 499 | `UnitName_Foot-Pound` | Foot-pounds |
| 500 | `UnitName_Foot-PoundPerMinute` | Foot-pounds/minute |
| 501 | `UnitName_Gigabit` | Gigabits |
| 502 | `UnitName_Gigabyte` | Gigabytes |
| 503 | `UnitName_Hectare` | Hectares |
| 504 | `UnitName_Horsepower` | Horsepower (US) |
| 505 | `UnitName_Hour` | Hours |
| 506 | `UnitName_Inch` | Inches |
| 507 | `UnitName_Joule` | Joules |
| 508 | `UnitName_Kilowatthour` | Kilowatt-hours |
| 509 | `UnitName_Kelvin` | Kelvin |
| 510 | `UnitName_Kilobit` | Kilobits |
| 511 | `UnitName_Kilobyte` | Kilobytes |
| 512 | `UnitName_Kilocalorie` | Food calories |
| 513 | `UnitName_Kilojoule` | Kilojoules |
| 514 | `UnitName_Kilometer` | Kilometers |
| 515 | `UnitName_KilometersPerHour` | Kilometers per hour |
| 516 | `UnitName_Kilowatt` | Kilowatts |
| 517 | `UnitName_Knot` | Knots |
| 518 | `UnitName_Mach` | Mach |
| 519 | `UnitName_Megabit` | Megabits |
| 520 | `UnitName_Megabyte` | Megabytes |
| 521 | `UnitName_Meter` | Meters |
| 522 | `UnitName_MetersPerSecond` | Meters per second |
| 523 | `UnitName_Micron` | Microns |
| 524 | `UnitName_Microsecond` | Microseconds |
| 525 | `UnitName_Mile` | Miles |
| 526 | `UnitName_MilesPerHour` | Miles per hour |
| 527 | `UnitName_Millimeter` | Millimeters |
| 528 | `UnitName_Millisecond` | Milliseconds |
| 529 | `UnitName_Minute` | Minutes |
| 530 | `UnitName_Nibble` | Nibble |
| 531 | `UnitName_Nanometer` | Nanometers |
| 532 | `UnitName_Angstrom` | Angstroms |
| 533 | `UnitName_NauticalMile` | Nautical miles |
| 534 | `UnitName_Petabit` | Petabits |
| 535 | `UnitName_Petabyte` | Petabytes |
| 536 | `UnitName_Second` | Seconds |
| 537 | `UnitName_SquareCentimeter` | Square centimeters |
| 538 | `UnitName_SquareFoot` | Square feet |
| 539 | `UnitName_SquareInch` | Square inches |
| 540 | `UnitName_SquareKilometer` | Square kilometers |
| 541 | `UnitName_SquareMeter` | Square meters |
| 542 | `UnitName_SquareMile` | Square miles |
| 543 | `UnitName_SquareMillimeter` | Square millimeters |
| 544 | `UnitName_SquareYard` | Square yards |
| 545 | `UnitName_Terabit` | Terabits |
| 546 | `UnitName_Terabyte` | Terabytes |
| 547 | `UnitName_Watt` | Watts |
| 548 | `UnitName_Week` | Weeks |
| 549 | `UnitName_Yard` | Yards |
| 550 | `UnitName_Year` | Years |
| 551 | `UnitAbbreviation_Carat` | CD |
| 552 | `UnitAbbreviation_Degree` | deg |
| 553 | `UnitAbbreviation_Radian` | rad |
| 554 | `UnitAbbreviation_Gradian` | grad |
| 555 | `UnitAbbreviation_Atmosphere` | atm |
| 556 | `UnitAbbreviation_Bar` | ba |
| 557 | `UnitAbbreviation_KiloPascal` | kPa |
| 558 | `UnitAbbreviation_MillimeterOfMercury` | mmHg |
| 559 | `UnitAbbreviation_Pascal` | Pa |
| 560 | `UnitAbbreviation_PSI` | psi |
| 561 | `UnitAbbreviation_Centigram` | cg |
| 562 | `UnitAbbreviation_Decagram` | dag |
| 563 | `UnitAbbreviation_Decigram` | dg |
| 564 | `UnitAbbreviation_Gram` | g |
| 565 | `UnitAbbreviation_Hectogram` | hg |
| 566 | `UnitAbbreviation_Kilogram` | kg |
| 567 | `UnitAbbreviation_LongTon` | ton (UK) |
| 568 | `UnitAbbreviation_Milligram` | mg |
| 569 | `UnitAbbreviation_Ounce` | oz |
| 570 | `UnitAbbreviation_Pound` | lb |
| 571 | `UnitAbbreviation_ShortTon` | ton (US) |
| 572 | `UnitAbbreviation_Stone` | st |
| 573 | `UnitAbbreviation_Tonne` | t |
| 574 | `UnitName_Carat` | Carats |
| 575 | `UnitName_Degree` | Degrees |
| 576 | `UnitName_Radian` | Radians |
| 577 | `UnitName_Gradian` | Gradians |
| 578 | `UnitName_Atmosphere` | Atmospheres |
| 579 | `UnitName_Bar` | Bars |
| 580 | `UnitName_KiloPascal` | Kilopascals |
| 581 | `UnitName_MillimeterOfMercury` | Millimeters of mercury  |
| 582 | `UnitName_Pascal` | Pascals |
| 583 | `UnitName_PSI` | Pounds per square inch |
| 584 | `UnitName_Centigram` | Centigrams |
| 585 | `UnitName_Decagram` | Dekagrams |
| 586 | `UnitName_Decigram` | Decigrams |
| 587 | `UnitName_Gram` | Grams |
| 588 | `UnitName_Hectogram` | Hectograms |
| 589 | `UnitName_Kilogram` | Kilograms |
| 590 | `UnitName_LongTon` | Long tons (UK) |
| 591 | `UnitName_Milligram` | Milligrams |
| 592 | `UnitName_Ounce` | Ounces |
| 593 | `UnitName_Pound` | Pounds |
| 594 | `UnitName_ShortTon` | Short tons (US) |
| 595 | `UnitName_Stone` | Stone |
| 596 | `UnitName_Tonne` | Metric tonnes |
| 597 | `UnitName_CD` | CDs |
| 598 | `UnitAbbreviation_CD` | CDs |
| 599 | `UnitName_SoccerField` | soccer fields |
| 600 | `UnitAbbreviation_SoccerField` | soccer fields |
| 601 | `UnitName_FloppyDisk` | floppy disks |
| 602 | `UnitAbbreviation_FloppyDisk` | floppy disks |
| 603 | `UnitName_DVD` | DVDs |
| 604 | `UnitAbbreviation_DVD` | DVDs |
| 605 | `UnitName_Battery` | batteries |
| 606 | `UnitAbbreviation_Battery` | batteries |
| 607 | `UnitName_Paperclip` | paperclips |
| 608 | `UnitAbbreviation_Paperclip` | paperclips |
| 609 | `UnitName_JumboJet` | jumbo jets |
| 610 | `UnitAbbreviation_JumboJet` | jumbo jets |
| 611 | `UnitName_LightBulb` | light bulbs |
| 612 | `UnitAbbreviation_LightBulb` | light bulbs |
| 613 | `UnitName_Horse` | horses |
| 614 | `UnitAbbreviation_Horse` | horses |
| 615 | `UnitName_Bathtub` | bathtubs |
| 616 | `UnitAbbreviation_Bathtub` | bathtubs |
| 617 | `UnitName_Snowflake` | snowflakes |
| 618 | `UnitAbbreviation_Snowflake` | snowflakes |
| 619 | `UnitName_Elephant` | elephants |
| 620 | `UnitAbbreviation_Elephant` | elephants |
| 621 | `UnitName_Turtle` | turtles |
| 622 | `UnitAbbreviation_Turtle` | turtles |
| 623 | `UnitName_Jet` | jets |
| 624 | `UnitAbbreviation_Jet` | jets |
| 625 | `UnitName_Whale` | whales |
| 626 | `UnitAbbreviation_Whale` | whales |
| 627 | `UnitName_CoffeeCup` | coffee cups |
| 628 | `UnitAbbreviation_CoffeeCup` | coffee cups |
| 629 | `UnitName_SwimmingPool` | swimming pools |
| 630 | `UnitAbbreviation_SwimmingPool` | swimming pools |
| 631 | `UnitName_Hand` | hands |
| 632 | `UnitAbbreviation_Hand` | hands |
| 633 | `UnitName_Paper` | sheets of paper |
| 634 | `UnitAbbreviation_Paper` | sheets of paper |
| 635 | `UnitName_Castle` | castles |
| 636 | `UnitAbbreviation_Castle` | castles |
| 637 | `UnitName_Banana` | bananas |
| 638 | `UnitAbbreviation_Banana` | bananas |
| 639 | `UnitName_SliceOfCake` | slices of cake |
| 640 | `UnitAbbreviation_SliceOfCake` | slices of cake |
| 641 | `UnitName_TrainEngine` | train engines |
| 642 | `UnitAbbreviation_TrainEngine` | train engines |
| 643 | `UnitName_SoccerBall` | soccer balls |
| 644 | `UnitAbbreviation_SoccerBall` | soccer balls |
| 645 | `degButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F4 |
| 646 | `radButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F5 |
| 647 | `gradButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F3 |
| 648 | `MemoryItemHelpText` | Memory item |
| 649 | `SupplementaryUnit_AutomationName` | %1 %2 |
| 650 | `AboutControlBackButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Back |
| 651 | `AboutControlBackButton.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Back |
| 652 | `AboutEULA.Text` | Microsoft Software License Terms |
| 653 | `PreviewTag.Text` | Preview |
| 654 | `AboutControlPrivacyStatement.Text` | Microsoft Privacy Statement |
| 655 | `AboutControlCopyright` | © %1 Microsoft. All rights reserved. |
| 656 | `AboutControlContribute` | To learn how you can contribute to Windows Calculator, check out the project on %HL%GitHub%HL%. |
| 657 | `AboutGroupTitle.Text` | About |
| 658 | `FeedbackButton.Content` | Send feedback |
| 659 | `LocalizedFontFamilyOverride` | RESERVED_FOR_FONTLOC |
| 660 | `LocalizedFontWeightOverride` | RESERVED_FOR_FONTLOC |
| 661 | `LocalizedUICaptionFontSizeFactorOverride` | RESERVED_FOR_FONTLOC |
| 662 | `LocalizedUITextFontSizeFactorOverride` | RESERVED_FOR_FONTLOC |
| 663 | `Bin` | BIN |
| 664 | `Dec` | DEC |
| 665 | `Hex` | HEX |
| 666 | `Oct` | OCT |
| 667 | `HistoryEmpty.Text` | There’s no history yet. |
| 668 | `MemoryPaneEmpty.Text` | There’s nothing saved in memory. |
| 669 | `MemoryFlyout.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Memory |
| 670 | `CannotPaste` | This expression cannot be pasted |
| 671 | `wordButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F4 |
| 672 | `qwordButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F2 |
| 673 | `byteButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F12 |
| 674 | `dwordButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | F3 |
| 675 | `UnitName_Gibibits` | Gibibits |
| 676 | `UnitName_Gibibytes` | Gibibytes |
| 677 | `UnitName_Kibibits` | Kibibits |
| 678 | `UnitName_Kibibytes` | Kibibytes |
| 679 | `UnitName_Mebibits` | Mebibits |
| 680 | `UnitName_Mebibytes` | Mebibytes |
| 681 | `UnitName_Pebibits` | Pebibits |
| 682 | `UnitName_Pebibytes` | Pebibytes |
| 683 | `UnitName_Tebibits` | Tebibits |
| 684 | `UnitName_Tebibytes` | Tebibytes |
| 685 | `UnitName_Exabits` | Exabits |
| 686 | `UnitName_Exabytes` | Exabytes |
| 687 | `UnitName_Exbibits` | Exbibits |
| 688 | `UnitName_Exbibytes` | Exbibytes |
| 689 | `UnitName_Zetabits` | Zetabits |
| 690 | `UnitName_Zetabytes` | Zetabytes |
| 691 | `UnitName_Zebibits` | Zebibits |
| 692 | `UnitName_Zebibytes` | Zebibytes |
| 693 | `UnitName_Yottabit` | Yottabits |
| 694 | `UnitName_Yottabyte` | Yottabytes |
| 695 | `UnitName_Yobibits` | Yobibits |
| 696 | `UnitName_Yobibytes` | Yobibytes |
| 697 | `DateCalculationModeText` | Date calculation |
| 698 | `DateCalculationOption.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Calculation mode |
| 699 | `AddOption.Content` | Add |
| 700 | `Date_AddSubtractOption.Content` | Add or subtract days |
| 701 | `DateLabel.Text` | Date |
| 702 | `Date_DifferenceOption.Content` | Difference between dates |
| 703 | `DaysLabel.Text` | Days |
| 704 | `Date_DifferenceLabel.Text` | Difference |
| 705 | `DateDiff_FromHeader.Header` | From |
| 706 | `MonthsLabel.Text` | Months |
| 707 | `SubtractOption.Content` | Subtract |
| 708 | `DateDiff_ToHeader.Header` | To |
| 709 | `YearsLabel.Text` | Years |
| 710 | `Date_OutOfBoundMessage` | Date out of Bound |
| 711 | `Date_Day` | day |
| 712 | `Date_Days` | days |
| 713 | `Date_Month` | month |
| 714 | `Date_Months` | months |
| 715 | `Date_SameDates` | Same dates |
| 716 | `Date_Week` | week |
| 717 | `Date_Weeks` | weeks |
| 718 | `Date_Year` | year |
| 719 | `Date_Years` | years |
| 720 | `Date_DifferenceResultAutomationName` | Difference %1 |
| 721 | `Date_ResultingDateAutomationName` | Resulting date %1 |
| 722 | `HeaderAutomationName_Calculator` | %1 Calculator mode |
| 723 | `HeaderAutomationName_Converter` | %1 Converter mode |
| 724 | `HeaderAutomationName_Date` | Date calculation mode |
| 725 | `DockPanel.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | History and Memory lists |
| 726 | `MemoryPanel.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Memory controls |
| 727 | `StandardFunctions.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Standard functions |
| 728 | `DisplayControls.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Display controls |
| 729 | `StandardOperators.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Standard operators |
| 730 | `NumberPad.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Number pad |
| 731 | `ScientificAngleOperators.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Angle operators |
| 732 | `ScientificFunctions.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Scientific functions |
| 733 | `RadixGroup.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Radix selection |
| 734 | `ProgrammerOperators.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Programmer operators |
| 735 | `InputModeSelectionGroup.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Input mode selection |
| 736 | `BitFlipPanel.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Bit toggling keypad |
| 737 | `scrollLeft.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Scroll expression left |
| 738 | `scrollRight.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Scroll expression right |
| 739 | `Format_MaxDigitsReached` | Max digits reached. %1 |
| 740 | `Format_ButtonPressAuditoryFeedback` | %1 %2 |
| 741 | `Format_MemorySave` | %1 saved to memory |
| 742 | `Format_MemorySlotChanged` | Memory slot %1 is %2 |
| 743 | `Format_MemorySlotCleared` | Memory slot %1 cleared |
| 744 | `divideButton.[using:CalculatorApp.Controls]CalculatorButton.AuditoryFeedback` | divided by |
| 745 | `multiplyButton.[using:CalculatorApp.Controls]CalculatorButton.AuditoryFeedback` | times |
| 746 | `minusButton.[using:CalculatorApp.Controls]CalculatorButton.AuditoryFeedback` | minus |
| 747 | `plusButton.[using:CalculatorApp.Controls]CalculatorButton.AuditoryFeedback` | plus |
| 748 | `powerButton.[using:CalculatorApp.Controls]CalculatorButton.AuditoryFeedback` | to the power of |
| 749 | `ySquareRootButton.[using:CalculatorApp.Controls]CalculatorButton.AuditoryFeedback` | y root |
| 750 | `modButton.[using:CalculatorApp.Controls]CalculatorButton.AuditoryFeedback` | mod |
| 751 | `lshButton.[using:CalculatorApp.Controls]CalculatorButton.AuditoryFeedback` | left shift |
| 752 | `rshButton.[using:CalculatorApp.Controls]CalculatorButton.AuditoryFeedback` | right shift |
| 753 | `orButton.[using:CalculatorApp.Controls]CalculatorButton.AuditoryFeedback` | or |
| 754 | `xorButton.[using:CalculatorApp.Controls]CalculatorButton.AuditoryFeedback` | x or |
| 755 | `andButton.[using:CalculatorApp.Controls]CalculatorButton.AuditoryFeedback` | and |
| 756 | `CurrencyFromToRatioFormat` | %1 %2 = %3 %4 |
| 757 | `CurrencyTimestampFormat` | Updated %1 %2 |
| 758 | `RefreshButtonText.Content` | Update rates |
| 759 | `DataChargesMayApply` | Data charges may apply. |
| 760 | `FailedToRefresh` | Couldn’t get new rates. Try again later. |
| 761 | `OfflineStatusHyperlinkText` | Offline. Please check your%HL%Network Settings%HL% |
| 762 | `UpdatingCurrencyRates` | Updating currency rates |
| 763 | `CurrencyRatesUpdated` | Currency rates updated |
| 764 | `CurrencyRatesUpdateFailed` | Could not update rates |
| 765 | `HistoryButton.AccessKey` | I |
| 766 | `MemoryButton.AccessKey` | M |
| 767 | `NavView.AccessKey` | H |
| 768 | `CategoryName_AngleAccessKey` | AN |
| 769 | `CategoryName_AreaAccessKey` | AR |
| 770 | `CategoryName_CurrencyAccessKey` | C |
| 771 | `CategoryName_DataAccessKey` | D |
| 772 | `CategoryName_EnergyAccessKey` | E |
| 773 | `CategoryName_LengthAccessKey` | L |
| 774 | `CategoryName_PowerAccessKey` | PO |
| 775 | `CategoryName_PressureAccessKey` | PR |
| 776 | `CategoryName_SpeedAccessKey` | S |
| 777 | `CategoryName_TimeAccessKey` | TI |
| 778 | `CategoryName_VolumeAccessKey` | V |
| 779 | `CategoryName_WeightAccessKey` | W |
| 780 | `CategoryName_TemperatureAccessKey` | TE |
| 781 | `ClearHistory.AccessKey` | Y |
| 782 | `ClearMemory.AccessKey` | R |
| 783 | `ClearMemory.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Clear all memory (Ctrl+L) |
| 784 | `ClearMemory.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Clear all memory |
| 785 | `HistoryLabel.AccessKey` | I |
| 786 | `MemoryLabel.AccessKey` | M |
| 787 | `SineDegrees` | sine degrees |
| 788 | `SineRadians` | sine radians |
| 789 | `SineGradians` | sine gradians |
| 790 | `InverseSineDegrees` | inverse sine degrees |
| 791 | `InverseSineRadians` | inverse sine radians |
| 792 | `InverseSineGradians` | inverse sine gradians |
| 793 | `HyperbolicSine` | hyperbolic sine |
| 794 | `InverseHyperbolicSine` | inverse hyperbolic sine |
| 795 | `CosineDegrees` | cosine degrees |
| 796 | `CosineRadians` | cosine radians |
| 797 | `CosineGradians` | cosine gradians |
| 798 | `InverseCosineDegrees` | inverse cosine degrees |
| 799 | `InverseCosineRadians` | inverse cosine radians |
| 800 | `InverseCosineGradians` | inverse cosine gradians |
| 801 | `HyperbolicCosine` | hyperbolic cosine |
| 802 | `InverseHyperbolicCosine` | inverse hyperbolic cosine |
| 803 | `TangentDegrees` | tangent degrees |
| 804 | `TangentRadians` | tangent radians |
| 805 | `TangentGradians` | tangent gradians |
| 806 | `InverseTangentDegrees` | inverse tangent degrees |
| 807 | `InverseTangentRadians` | inverse tangent radians |
| 808 | `InverseTangentGradians` | inverse tangent gradians |
| 809 | `HyperbolicTangent` | hyperbolic tangent |
| 810 | `InverseHyperbolicTangent` | inverse hyperbolic tangent |
| 811 | `SecantDegrees` | secant degrees |
| 812 | `SecantRadians` | secant radians |
| 813 | `SecantGradians` | secant gradians |
| 814 | `InverseSecantDegrees` | inverse secant degrees |
| 815 | `InverseSecantRadians` | inverse secant radians |
| 816 | `InverseSecantGradians` | inverse secant gradians |
| 817 | `HyperbolicSecant` | hyperbolic secant |
| 818 | `InverseHyperbolicSecant` | inverse hyperbolic secant |
| 819 | `CosecantDegrees` | cosecant degrees |
| 820 | `CosecantRadians` | cosecant radians |
| 821 | `CosecantGradians` | cosecant gradians |
| 822 | `InverseCosecantDegrees` | inverse cosecant degrees |
| 823 | `InverseCosecantRadians` | inverse cosecant radians |
| 824 | `InverseCosecantGradians` | inverse cosecant gradians |
| 825 | `HyperbolicCosecant` | hyperbolic cosecant |
| 826 | `InverseHyperbolicCosecant` | inverse hyperbolic cosecant |
| 827 | `CotangentDegrees` | cotangent degrees |
| 828 | `CotangentRadians` | Cotangent radians |
| 829 | `CotangentGradians` | cotangent gradians |
| 830 | `InverseCotangentDegrees` | inverse cotangent degrees |
| 831 | `InverseCotangentRadians` | inverse cotangent radians |
| 832 | `InverseCotangentGradians` | inverse cotangent gradians |
| 833 | `HyperbolicCotangent` | hyperbolic cotangent |
| 834 | `InverseHyperbolicCotangent` | inverse hyperbolic cotangent |
| 835 | `CubeRoot` | Cube root |
| 836 | `Logy` | Log base |
| 837 | `AbsoluteValue` | Absolute value |
| 838 | `LeftShift` | left shift |
| 839 | `RightShift` | right shift |
| 840 | `Factorial` | factorial |
| 841 | `DegreeMinuteSecond` | degree minute second |
| 842 | `NaturalLog` | natural log |
| 843 | `Square` | square |
| 844 | `YRoot` | y root |
| 845 | `NavCategoryItem_AutomationNameFormat` | %1 %2 |
| 846 | `NavCategoryHeader_AutomationNameFormat` | %1 category |
| 847 | `AboutControlServicesAgreement.Text` | Microsoft Services Agreement |
| 848 | `UnitAbbreviation_Pyeong` | Pyeong |
| 849 | `UnitName_Pyeong` | Pyeong |
| 850 | `AddSubtract_Date_FromHeader.Header` | From |
| 851 | `CalculationResultScrollLeft.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Scroll calculation result left |
| 852 | `CalculationResultScrollRight.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Scroll calculation result right |
| 853 | `CalculationFailed` | Calculation failed |
| 854 | `logBaseY.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Log base Y |
| 855 | `logBaseY.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | L |
| 856 | `trigButton.Text` | Trigonometry |
| 857 | `funcButton.Text` | Function |
| 858 | `inequalityButton.Text` | Inequalities |
| 859 | `inequalityButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Inequalities |
| 860 | `bitwiseButton.Text` | Bitwise |
| 861 | `bitShiftButton.Text` | Bit shift |
| 862 | `trigShiftButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Inverse function |
| 863 | `hypButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Hyperbolic function |
| 864 | `secButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Secant |
| 865 | `secButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | U |
| 866 | `sechButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Hyperbolic secant |
| 867 | `sechButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | U |
| 868 | `invsecButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Arc secant |
| 869 | `invsecButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | U |
| 870 | `invsechButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Hyperbolic arc secant |
| 871 | `invsechButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlShiftChord` | U |
| 872 | `cscButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Cosecant |
| 873 | `cscButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | I |
| 874 | `cschButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Hyperbolic cosecant |
| 875 | `cschButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | I |
| 876 | `invcscButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Arc cosecant |
| 877 | `invcscButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | I |
| 878 | `invcschButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Hyperbolic arc cosecant |
| 879 | `invcschButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlShiftChord` | I |
| 880 | `cotButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Cotangent |
| 881 | `cotButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | J |
| 882 | `cothButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Hyperbolic cotangent |
| 883 | `cothButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | J |
| 884 | `invcotButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Arc cotangent |
| 885 | `invcotButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | J |
| 886 | `invcothButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Hyperbolic arc cotangent |
| 887 | `invcothButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlShiftChord` | J |
| 888 | `floorButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Floor |
| 889 | `floorButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | [ |
| 890 | `ceilButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Ceiling |
| 891 | `ceilButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | ] |
| 892 | `randButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Random |
| 893 | `randButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | R |
| 894 | `absButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Absolute value |
| 895 | `absButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | \| |
| 896 | `eulerButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Euler's number |
| 897 | `eulerButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyShiftChord` | E |
| 898 | `twoPowerXButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Two to the exponent |
| 899 | `twoPowerXButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | G |
| 900 | `nandButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Nand |
| 901 | `nandButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | . |
| 902 | `nandButton.[using:CalculatorApp.Controls]CalculatorButton.AuditoryFeedback` | Nand |
| 903 | `norButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Nor |
| 904 | `norButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | \\ |
| 905 | `norButton.[using:CalculatorApp.Controls]CalculatorButton.AuditoryFeedback` | Nor |
| 906 | `rolCarryButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Rotate on left with carry |
| 907 | `rolCarryButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | < |
| 908 | `rorCarryButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Rotate on right with carry |
| 909 | `rorCarryButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | > |
| 910 | `lshLogicalButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Left shift |
| 911 | `lshLogicalButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | < |
| 912 | `lshLogicalButton.[using:CalculatorApp.Controls]CalculatorButton.AuditoryFeedback` | Left shift |
| 913 | `rshLogicalButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Right shift |
| 914 | `rshLogicalButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | > |
| 915 | `rshLogicalButton.[using:CalculatorApp.Controls]CalculatorButton.AuditoryFeedback` | Right shift |
| 916 | `arithmeticShiftButton.Content` | Arithmetic shift |
| 917 | `logicalShiftButton.Content` | Logical shift |
| 918 | `rotateCircularButton.Content` | Rotate circular shift |
| 919 | `rotateCarryShiftButton.Content` | Rotate through carry circular shift |
| 920 | `cubeRootButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Cube root |
| 921 | `cubeRootButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | B |
| 922 | `trigButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Trigonometry |
| 923 | `funcButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Functions |
| 924 | `bitwiseButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Bitwise |
| 925 | `bitShiftButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Bitshift |
| 926 | `ScientificOperatorPanel.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Scientific operator panels |
| 927 | `ProgrammerOperatorPanel.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Programmer operator panels |
| 928 | `MostSignificantBit` | most significant bit |
| 929 | `GraphingCalculatorModeText` | Graphing |
| 930 | `plotButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Enter |
| 931 | `plotButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Plot |
| 932 | `xButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | X |
| 933 | `yButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKey` | Y |
| 934 | `graphingPowerButton.[using:CalculatorApp.Common]KeyboardShortcutManager.Character` | ^ |
| 935 | `graphViewButton.[using:CalculatorApp.Common]KeyboardShortcutManager.VirtualKeyControlChord` | Home |
| 936 | `graphViewButton.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Refresh view automatically (Ctrl + 0) |
| 937 | `graphViewButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Graph view |
| 938 | `GraphViewAutomaticBestFitAnnouncement` | Automatic best fit |
| 939 | `GraphViewManualAdjustmentAnnouncement` | Manual adjustment |
| 940 | `GridResetAnnouncement` | Graph view has been reset |
| 941 | `zoomInButton.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Zoom in (Ctrl + plus) |
| 942 | `zoomInButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Zoom in |
| 943 | `zoomOutButton.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Zoom out (Ctrl + minus) |
| 944 | `zoomOutButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Zoom out |
| 945 | `EquationTextBoxAddPanel.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Add equation |
| 946 | `ShareActionErrorMessage` | Unable to share at this time. |
| 947 | `ShareActionErrorOk` | OK |
| 948 | `ShareActionTitle` | Look what I graphed with Windows Calculator |
| 949 | `EquationsShareHeader` | Equations |
| 950 | `VariablesShareHeader` | Variables |
| 951 | `GraphImageAltText` | Image of a graph with equations |
| 952 | `VaiablesHeader.Text` | Variables |
| 953 | `StepTextBlock.Text` | Step |
| 954 | `MinTextBlock.Text` | Min |
| 955 | `MaxTextBlock.Text` | Max |
| 956 | `LineColorText.Text` | Color |
| 957 | `StyleChooserBoxHeading.Text` | Style |
| 958 | `KeyGraphFeaturesLabel.Text` | Function analysis |
| 959 | `KGFHorizontalAsymptotesNone` | The function does not have any horizontal asymptotes. |
| 960 | `KGFInflectionPointsNone` | The function does not have any inflection points. |
| 961 | `KGFMaximaNone` | The function does not have any maxima points. |
| 962 | `KGFMinimaNone` | The function does not have any minima points. |
| 963 | `KGFMonotonicityConstant` | Constant |
| 964 | `KGFMonotonicityDecreasing` | Decreasing |
| 965 | `KGFMonotonicityError` | Unable to determine the monotonicity of the function. |
| 966 | `KGFMonotonicityIncreasing` | Increasing |
| 967 | `KGFMonotonicityUnknown` | The monotonicity of the function is unknown. |
| 968 | `KGFObliqueAsymptotesNone` | The function does not have any oblique aysmptotes. |
| 969 | `KGFParityError` | Unable to determine the parity of the function. |
| 970 | `KGFParityEven` | The function is even. |
| 971 | `KGFParityNeither` | The function is neither even nor odd. |
| 972 | `KGFParityOdd` | The function is odd. |
| 973 | `KGFParityUnknown` | The function parity is unknown. |
| 974 | `KGFPeriodicityError` | Periodicity is not supported for this function. |
| 975 | `KGFPeriodicityNotPeriodic` | The function is not periodic. |
| 976 | `KGFPeriodicityUnknown` | The function periodicity is unknown. |
| 977 | `KGFTooComplexFeaturesError` | These features are too complex for Calculator to calculate: |
| 978 | `KGFVerticalAsymptotesNone` | The function does not have any vertical asymptotes. |
| 979 | `KGFXInterceptNone` | The function does not have any x-intercepts. |
| 980 | `KGFYInterceptNone` | The function does not have any y-intercepts. |
| 981 | `Domain` | Domain |
| 982 | `HorizontalAsymptotes` | Horizontal asymptotes |
| 983 | `InflectionPoints` | Inflection points |
| 984 | `KGFAnalysisNotSupported` | Analysis is not supported for this function. |
| 985 | `KGFVariableIsNotX` | Analysis is only supported for functions in the f(x) format. Example: y=x |
| 986 | `Maxima` | Maxima |
| 987 | `Minima` | Minima |
| 988 | `Monotonicity` | Monotonicity |
| 989 | `ObliqueAsymptotes` | Oblique asymptotes |
| 990 | `Parity` | Parity |
| 991 | `Periodicity` | Period |
| 992 | `Range` | Range |
| 993 | `VerticalAsymptotes` | Vertical asymptotes |
| 994 | `XIntercept` | X-Intercept |
| 995 | `YIntercept` | Y-Intercept |
| 996 | `KGFAnalysisCouldNotBePerformed` | Analysis could not be performed for the function. |
| 997 | `KGFDomainNone` | Unable to calculate the domain for this function. |
| 998 | `KGFRangeNone` | Unable to calculate the range for this function. |
| 999 | `Overflow` | Overflow (the number is too large) |
| 1000 | `RequireRadiansMode` | Radians mode is required to graph this equation. |
| 1001 | `TooComplexToSolve` | This function is too complex to graph |
| 1002 | `RequireDegreesMode` | Degrees mode is required to graph this function |
| 1003 | `FactorialInvalidArgument` | The factorial function has an invalid argument |
| 1004 | `FactorialCannotPerformOnLargeNumber` | The factorial function has an argument that is too large to graph |
| 1005 | `ModuloCannotPerformOnFloat` | Modulo can only be used with whole numbers |
| 1006 | `EquationHasNoSolution` | The equation has no solution |
| 1007 | `DivideByZero` | Cannot divide by zero |
| 1008 | `MutuallyExclusiveConditions` | The equation contains logical conditions that are mutually exclusive |
| 1009 | `OutOfDomain` | Equation is out of domain |
| 1010 | `GE_NotSupported` | Graphing this equation is not supported |
| 1011 | `ParenthesisMismatch` | The equation is missing an opening parenthesis |
| 1012 | `UnmatchedParenthesis` | The equation is missing a closing parenthesis |
| 1013 | `TooManyDecimalPoints` | There are too many decimal points in a number |
| 1014 | `DecimalPointWithoutDigits` | A decimal point is missing digits |
| 1015 | `UnexpectedEndOfExpression` | Unexpected end of expression |
| 1016 | `UnexpectedToken` | Unexpected characters in the expression |
| 1017 | `InvalidToken` | Invalid characters in the expression |
| 1018 | `TooManyEquals` | There are too many equal signs |
| 1019 | `EqualWithoutGraphVariable` | The function must contain at least one x or y variable |
| 1020 | `InvalidEquationSyntax` | Invalid expression |
| 1021 | `EmptyExpression` | The expression is empty |
| 1022 | `EqualWithoutEquation` | Equal was used without an equation |
| 1023 | `ExpectParenthesisAfterFunctionName` | Parenthesis missing after function name |
| 1024 | `IncorrectNumParameter` | A mathematical operation has the incorrect number of parameters |
| 1025 | `InvalidVariableNameFormat` | A variable name is invalid |
| 1026 | `BracketMismatch` | The equation is missing an opening bracket |
| 1027 | `UnmatchedBracket` | The equation is missing a closing bracket |
| 1028 | `CannotUseIInReal` | "i" and "I" cannot be used as variable names |
| 1029 | `GeneralError` | The equation could not be graphed |
| 1030 | `InvalidNumberDigit` | The digit could not be resolved for the given base |
| 1031 | `InvalidNumberBase` | The base must be greater than 2 and less than 36 |
| 1032 | `InvalidVariableSpecification` | A mathematical operation requires one of its parameters to be a variable |
| 1033 | `ExpectingLogicalOperands` | Equation is mixing logical and scalar operands |
| 1034 | `CannotUseIndexVarInOpLimits` | x or y cannot be used in the upper or lower limits |
| 1035 | `CannotUseIndexVarInLimPoint` | x or y cannot be used in the limit point |
| 1036 | `CannotUseComplexInfinityInReal` | Cannot use complex infinity |
| 1037 | `CannotUseIInInequalitySolving` | Cannot use complex numbers in inequalities |
| 1038 | `equationAnalysisBack.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Back to function list |
| 1039 | `equationAnalysisBack.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Back to function list |
| 1040 | `functionAnalysisButton.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Analyze function |
| 1041 | `functionAnalysisButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Analyze function |
| 1042 | `functionAnalysisMenuItem` | Analyze function |
| 1043 | `removeButton.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Remove equation |
| 1044 | `removeButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Remove equation |
| 1045 | `removeMenuItem` | Remove equation |
| 1046 | `shareButton.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Share |
| 1047 | `shareButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Share |
| 1048 | `colorChooserButton.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Change equation style |
| 1049 | `colorChooserButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Change equation style |
| 1050 | `colorChooserMenuItem` | Change equation style |
| 1051 | `showEquationButtonToolTip` | Show equation |
| 1052 | `hideEquationButtonToolTip` | Hide equation |
| 1053 | `showEquationButtonAutomationName` | Show equation %1 |
| 1054 | `hideEquationButtonAutomationName` | Hide equation %1 |
| 1055 | `disableTracingButtonToolTip` | Stop tracing |
| 1056 | `enableTracingButtonToolTip` | Start tracing |
| 1057 | `graphAutomationName` | Graph viewing window, x-axis bounded by %1 and %2, y-axis bounded by %3 and %4, displaying %5 equations |
| 1058 | `sliderOptionsButton.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Configure slider |
| 1059 | `sliderOptionsButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Configure slider |
| 1060 | `GraphSwitchToEquationMode` | Switch to equation mode |
| 1061 | `GraphSwitchToGraphMode` | Switch to graph mode |
| 1062 | `SwitchModeToggleButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Switch to equation mode |
| 1063 | `GraphSwitchedToEquationModeAnnouncement` | Current mode is equation mode |
| 1064 | `GraphSwitchedToGraphModeAnnouncement` | Current mode is graph mode |
| 1065 | `GridHeading.Text` | Window |
| 1066 | `TrigModeDegrees.Content` | Degrees |
| 1067 | `TrigModeGradians.Content` | Gradians |
| 1068 | `TrigModeRadians.Content` | Radians |
| 1069 | `UnitsHeading.Text` | Units |
| 1070 | `ResetViewButton.Content` | Reset view |
| 1071 | `GraphSettingsXMax.Header` | X-Max |
| 1072 | `GraphSettingsXMin.Header` | X-Min |
| 1073 | `GraphSettingsYMax.Header` | Y-Max |
| 1074 | `GraphSettingsYMin.Header` | Y-Min |
| 1075 | `graphSettingsButton.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Graph options |
| 1076 | `graphSettingsButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Graph options |
| 1077 | `GraphOptionsHeading.Text` | Graph options |
| 1078 | `VariableAreaSettings.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Variable options |
| 1079 | `VariableAreaSettings.[using:Windows.UI.Xaml.Controls]ToolTipService.ToolTip` | Toggle variable options |
| 1080 | `LineThicknessBoxHeading.Text` | Line thickness |
| 1081 | `LineOptionsHeading.Text` | Line options |
| 1082 | `SmallLineWidthAutomationName` | Small line width |
| 1083 | `MediumLineWidthAutomationName` | Medium line width |
| 1084 | `LargeLineWidthAutomationName` | Large line width |
| 1085 | `ExtraLargeLineWidthAutomationName` | Extra large line width |
| 1086 | `mathRichEditBox.PlaceholderText` | Enter an expression |
| 1087 | `GraphCopyMenuItem.Text` | Copy |
| 1088 | `cutEquationMenuItem.Text` | Cut |
| 1089 | `copyEquationMenuItem.Text` | Copy |
| 1090 | `pasteEquationMenuItem.Text` | Paste |
| 1091 | `undoEquationMenuItem.Text` | Undo |
| 1092 | `selectAllEquationMenuItem.Text` | Select all |
| 1093 | `EquationInputButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Function input |
| 1094 | `EquationInputList.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Function input |
| 1095 | `EquationInputPanel.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Function input panel |
| 1096 | `VariableStackPanel.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Variable panel |
| 1097 | `VariableListView.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Variable list |
| 1098 | `VariableListViewItem` | Variable %1 list item |
| 1099 | `VariableValueTextBox.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Variable value textbox |
| 1100 | `VariableValueSlider.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Variable value slider |
| 1101 | `VariableMinTextBox.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Variable minimum value textbox |
| 1102 | `VariableStepTextBox.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Variable step value textbox |
| 1103 | `VariableMaxTextBox.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Variable maximum value textbox |
| 1104 | `solidLineStyleAutomationName` | Solid line style |
| 1105 | `dotLineStyleAutomationName` | Dot line style |
| 1106 | `dashLineStyleAutomationName` | Dash line style |
| 1107 | `equationColor1AutomationName` | Navy blue |
| 1108 | `equationColor2AutomationName` | Seafoam |
| 1109 | `equationColor3AutomationName` | Violet |
| 1110 | `equationColor4AutomationName` | Green |
| 1111 | `equationColor5AutomationName` | Mint green |
| 1112 | `equationColor6AutomationName` | Dark green |
| 1113 | `equationColor7AutomationName` | Charcoal |
| 1114 | `equationColor8AutomationName` | Red |
| 1115 | `equationColor9AutomationName` | Plum light |
| 1116 | `equationColor10AutomationName` | Magenta |
| 1117 | `equationColor11AutomationName` | Yellow gold |
| 1118 | `equationColor12AutomationName` | Orange bright |
| 1119 | `equationColor13AutomationName` | Brown |
| 1120 | `equationColor14BlackAutomationName` | Black |
| 1121 | `equationColor14WhiteAutomationName` | White |
| 1122 | `equationHighContrastColor1AutomationName` | Color 1 |
| 1123 | `equationHighContrastColor2AutomationName` | Color 2 |
| 1124 | `equationHighContrastColor3AutomationName` | Color 3 |
| 1125 | `equationHighContrastColor4AutomationName` | Color 4 |
| 1126 | `GraphThemeHeading.Text` | Graph theme |
| 1127 | `AlwaysLightTheme.Content` | Always light |
| 1128 | `MatchAppTheme.Content` | Match app theme |
| 1129 | `GraphThemeHeading.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Theme |
| 1130 | `AlwaysLightTheme.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Always light |
| 1131 | `MatchAppTheme.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Match app theme |
| 1132 | `FunctionRemovedAnnouncement` | Function removed |
| 1133 | `KGFEquationTextBox.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Function analysis equation box |
| 1134 | `graphingEqualButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Equals |
| 1135 | `lessThanFlyoutButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Less than |
| 1136 | `lessThanOrEqualFlyoutButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Less than or equal |
| 1137 | `equalsFlyoutButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Equal |
| 1138 | `greaterThanOrEqualFlyoutButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Greater than or equal |
| 1139 | `greaterThanFlyoutButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Greater than |
| 1140 | `xButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | X |
| 1141 | `yButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Y |
| 1142 | `submitButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Submit |
| 1143 | `FunctionAnalysisGrid.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Function analysis |
| 1144 | `GraphSettingsGrid.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Graph options |
| 1145 | `DockPanel_HistoryMemoryLists` | History and Memory lists |
| 1146 | `DockPanel_MemoryList` | Memory list |
| 1147 | `Format_HistorySlotCleared` | History slot %1 cleared |
| 1148 | `CalcAlwaysOnTop` | Calculator always on top |
| 1149 | `CalcBackToFullView` | Calculator back to full view |
| 1150 | `arithmeticShiftButtonSelected` | Arithmetic shift selected |
| 1151 | `logicalShiftButtonSelected` | Logical shift selected |
| 1152 | `rotateCircularButtonSelected` | Rotate circular shift selected |
| 1153 | `rotateCarryShiftButtonSelected` | Rotate through carry circular shift selected |
| 1154 | `SettingsHeader.Text` | Settings |
| 1155 | `SettingsAppearance.Text` | Appearance |
| 1156 | `AppThemeExpander.Header` | App theme |
| 1157 | `AppThemeExpander.Description` | Select which app theme to display |
| 1158 | `LightThemeRadioButton.Content` | Light |
| 1159 | `DarkThemeRadioButton.Content` | Dark |
| 1160 | `SystemThemeRadioButton.Content` | Use system setting |
| 1161 | `TitleBarBackButton.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Back |
| 1162 | `SettingsPageOpenedAnnouncement` | Settings page |
| 1163 | `MathRichEditBox.[using:Windows.UI.Xaml.Automation]AutomationProperties.Name` | Open the context menu for available actions |
| 1164 | `ErrorButtonOk` | OK |
| 1165 | `SnapshotRestoreError` | Couldn't restore this snapshot. |

### CEngineStrings.resw — 108 entries

| # | Name | Value |
|---|---|---|
| 0 | `10` | Rsh |
| 1 | `100` | Invalid input |
| 2 | `101` | Result is undefined |
| 3 | `105` | Not enough memory |
| 4 | `107` | Overflow |
| 5 | `108` | Result not defined |
| 6 | `11` | ÷ |
| 7 | `118` | Result not defined |
| 8 | `119` | Overflow |
| 9 | `12` | × |
| 10 | `120` | Overflow |
| 11 | `13` | + |
| 12 | `14` | - |
| 13 | `15` | Mod |
| 14 | `16` | yroot |
| 15 | `17` | ^ |
| 16 | `18` | Int |
| 17 | `19` | RoL |
| 18 | `2` | CE |
| 19 | `20` | RoR |
| 20 | `21` | NOT |
| 21 | `22` | sin |
| 22 | `23` | cis |
| 23 | `24` | tan |
| 24 | `25` | sinh |
| 25 | `26` | cosh |
| 26 | `27` | tanh |
| 27 | `28` | ln |
| 28 | `29` | log |
| 29 | `30` | √ |
| 30 | `35` | dms |
| 31 | `37` | 10^ |
| 32 | `38` | % |
| 33 | `4` | . |
| 34 | `40` | Pi |
| 35 | `41` | = |
| 36 | `47` | Exp |
| 37 | `48` | ( |
| 38 | `49` | ) |
| 39 | `6` | AND |
| 40 | `66` | frac |
| 41 | `67` | sin₀ |
| 42 | `68` | cos₀ |
| 43 | `69` | tan₀ |
| 44 | `7` | OR |
| 45 | `70` | sin₀⁻¹ |
| 46 | `71` | cos₀⁻¹ |
| 47 | `72` | tan₀⁻¹ |
| 48 | `73` | sinᵣ |
| 49 | `74` | cosᵣ |
| 50 | `75` | tanᵣ |
| 51 | `76` | sinᵣ⁻¹ |
| 52 | `77` | cosᵣ⁻¹ |
| 53 | `78` | tanᵣ⁻¹ |
| 54 | `79` | sin₉ |
| 55 | `8` | XOR |
| 56 | `80` | cos₉ |
| 57 | `81` | tan₉ |
| 58 | `82` | sin₉⁻¹ |
| 59 | `83` | cos₉⁻¹ |
| 60 | `84` | tan₉⁻¹ |
| 61 | `85` | sinh⁻¹ |
| 62 | `86` | cosh⁻¹ |
| 63 | `87` | tanh⁻¹ |
| 64 | `88` | e^ |
| 65 | `89` | 10^ |
| 66 | `9` | Lsh |
| 67 | `90` | √ |
| 68 | `91` | sqr |
| 69 | `92` | cube |
| 70 | `94` | fact |
| 71 | `95` | 1/ |
| 72 | `96` | degrees |
| 73 | `97` | negate |
| 74 | `99` | Cannot divide by zero |
| 75 | `SecDeg` | sec₀ |
| 76 | `SecRad` | secᵣ |
| 77 | `SecGrad` | sec₉ |
| 78 | `InverseSecDeg` | sec₀⁻¹ |
| 79 | `InverseSecRad` | secᵣ⁻¹ |
| 80 | `InverseSecGrad` | sec₉⁻¹ |
| 81 | `CscDeg` | csc₀ |
| 82 | `CscRad` | cscᵣ |
| 83 | `CscGrad` | csc₉ |
| 84 | `InverseCscDeg` | csc₀⁻¹ |
| 85 | `InverseCscRad` | cscᵣ⁻¹ |
| 86 | `InverseCscGrad` | csc₉⁻¹ |
| 87 | `CotDeg` | cot₀ |
| 88 | `CotRad` | cotᵣ |
| 89 | `CotGrad` | cot₉ |
| 90 | `InverseCotDeg` | cot₀⁻¹ |
| 91 | `InverseCotRad` | cotᵣ⁻¹ |
| 92 | `InverseCotGrad` | cot₉⁻¹ |
| 93 | `Sech` | sech |
| 94 | `InverseSech` | sech⁻¹ |
| 95 | `Csch` | csch |
| 96 | `InverseCsch` | csch⁻¹ |
| 97 | `Coth` | coth |
| 98 | `InverseCoth` | coth⁻¹ |
| 99 | `TwoPowX` | 2^ |
| 100 | `LogBaseY` | log base |
| 101 | `Abs` | abs |
| 102 | `Ceil` | ceil |
| 103 | `Floor` | floor |
| 104 | `Nand` | NAND |
| 105 | `Nor` | NOR |
| 106 | `CubeRoot` | cuberoot |
| 107 | `ProgrammerMod` | % |


---


## 6. LIMITS

### 6.1 Precision constants (managed)

```csharp
        private const int StandardModePrecision = 16;
        private const int ScientificModePrecision = 32;
        private const int ProgrammerModePrecision = 64;
```

Applied at mode switch — `StandardCalculatorViewModel.cs:781-799`:

| Mode | Constant | Value | Set at |
|---|---|---|---|
| Standard | `StandardModePrecision` | 16 | StandardCalculatorViewModel.cs:785 |
| Scientific | `ScientificModePrecision` | 32 | StandardCalculatorViewModel.cs:792 |
| Programmer | `ProgrammerModePrecision` | 64 | StandardCalculatorViewModel.cs:798 |

```csharp
// StandardCalculatorViewModel.cs:1746-1749
        private void SetPrecision(int precision)
        {
            _standardCalculatorManager?.SetPrecision(precision);
        }
```

### 6.2 Engine defaults (C++ CalcManager)

```cpp
// src/CalcManager/CEngine/calc.cpp:15-22
static constexpr int DEFAULT_MAX_DIGITS = 32;
static constexpr int DEFAULT_PRECISION = 32;
static constexpr int32_t DEFAULT_RADIX = 10;

static constexpr wchar_t DEFAULT_DEC_SEPARATOR = L'.';
static constexpr wchar_t DEFAULT_GRP_SEPARATOR = L',';
static constexpr wstring_view DEFAULT_GRP_STR = L"3;0";
static constexpr wstring_view DEFAULT_NUMBER_STR = L"0";
```

| Constant | Value | Meaning | file:line |
|---|---|---|---|
| `DEFAULT_MAX_DIGITS` | 32 | default max significant digits | src/CalcManager/CEngine/calc.cpp:15 |
| `DEFAULT_PRECISION` | 32 | default calculation precision | src/CalcManager/CEngine/calc.cpp:16 |
| `DEFAULT_RADIX` | 10 | default number base | src/CalcManager/CEngine/calc.cpp:17 |
| `DEFAULT_DEC_SEPARATOR` | `L'.'` | decimal separator | src/CalcManager/CEngine/calc.cpp:19 |
| `DEFAULT_GRP_SEPARATOR` | `L','` | group separator | src/CalcManager/CEngine/calc.cpp:20 |
| `DEFAULT_GRP_STR` | `L"3;0"` | grouping (thousands size; increment) | src/CalcManager/CEngine/calc.cpp:21 |
| `DEFAULT_NUMBER_STR` | `L"0"` | zero display | src/CalcManager/CEngine/calc.cpp:22 |

`ChangeBaseConstants(DEFAULT_RADIX, DEFAULT_MAX_DIGITS, DEFAULT_PRECISION)` — `calc.cpp:52`.

### 6.3 Input length limits

```cpp
// src/CalcManager/Header Files/CalcInput.h:9
constexpr int MAX_STRLEN = 84;
```

```cpp
// src/CalcManager/CEngine/CalcInput.cpp:10-11
constexpr int C_NUM_MAX_DIGITS = MAX_STRLEN;
constexpr int C_EXP_MAX_DIGITS = 4;
```

| Constant | Value | Meaning | file:line |
|---|---|---|---|
| `MAX_STRLEN` | 84 | max chars for a mantissa/base string | src/CalcManager/Header Files/CalcInput.h:9 |
| `C_NUM_MAX_DIGITS` | `MAX_STRLEN` = 84 | max digits in the numeric part | src/CalcManager/CEngine/CalcInput.cpp:10 |
| `C_EXP_MAX_DIGITS` | 4 | max exponent digits | src/CalcManager/CEngine/CalcInput.cpp:11 |

```cpp
// src/CalcManager/CEngine/CalcInput.cpp:318-319
    // Base and Exp can each be up to C_NUM_MAX_DIGITS in length, plus 4 characters for sign, dec, exp, and expSign.
    if (result.size() > C_NUM_MAX_DIGITS * 2 + 4)
```

```cpp
// src/CalcManager/CEngine/CalcInput.cpp:276
    if ((m_base.value.size() > MAX_STRLEN) || (m_hasExponent && m_exponent.value.size() > MAX_STRLEN))
```

### 6.4 History cap

```cpp
// src/CalcManager/CalculatorManager.cpp:12
static constexpr size_t MAX_HISTORY_ITEMS = 20;
```

```cpp
// src/CalcManager/CalculatorManager.cpp:27-30
        , m_currentDegreeMode(Command::CommandNULL)
        , m_pStdHistory(new CalculatorHistory(MAX_HISTORY_ITEMS))
        , m_pSciHistory(new CalculatorHistory(MAX_HISTORY_ITEMS))
        , m_pHistory(nullptr)
```

```cpp
// src/CalcManager/CalculatorHistory.cpp:33-36 and :53-57

CalculatorHistory::CalculatorHistory(size_t maxSize)
    : m_maxHistorySize(maxSize)
{
```

**History cap = 20 items per history (standard AND scientific get separate instances).**

| Item | Value | file:line |
|---|---|---|
| `MAX_HISTORY_ITEMS` | 20 | src/CalcManager/CalculatorManager.cpp:12 |
| Managed trim | `if (Items.Count >= (int)(_calculatorManager?.MaxHistorySize ?? 0))` | src/Calculator.ViewModels/HistoryViewModel.cs:75 |
| Managed max | `return (ulong)(_calculatorManager?.MaxHistorySize ?? 0);` | src/Calculator.ViewModels/HistoryViewModel.cs:191 |
| Interop | `uint64_t CalculatorManagerWrapper::MaxHistorySize()` | src/CalcManager.Interop/CalcManagerImpl.cpp:319-321 |

### 6.5 Memory cap

**There is NO memory cap.** `_memorizedNumbers` is an unbounded `ObservableCollection<MemoryItemViewModel>` (`StandardCalculatorViewModel.cs:153`). The C++ side stores a single `std::unique_ptr<CalcEngine::Rational> m_memoryValue` (`src/CalcManager/Header Files/CalcEngine.h:127`), not a stack. No `MAX_MEMORY` / `MemoryItemsCount` constant exists anywhere in `src/`.

| Evidence | Quote | file:line |
|---|---|---|
| Field declaration | `private readonly ObservableCollection<MemoryItemViewModel> _memorizedNumbers;` | StandardCalculatorViewModel.cs:153 |
| Init | `_memorizedNumbers = new ObservableCollection<MemoryItemViewModel>();` | StandardCalculatorViewModel.cs:153 |
| C++ storage | `std::unique_ptr<CalcEngine::Rational> m_memoryValue; // Current memory value.` | src/CalcManager/Header Files/CalcEngine.h:127 |

### 6.6 Parentheses nesting limit

**No numeric cap exists.** `OpenParenthesisCount` is a `uint` that is only ever *reported*; the limit is enforced by button enablement, not by a constant.

```csharp
// StandardCalculatorViewModel.cs:1092-1103
        public void SetParenthesisCount(uint parenthesisCount)
        {
            if (_openParenthesisCount == parenthesisCount)
            {
                return;
            }

            OpenParenthesisCount = parenthesisCount;
            if (IsProgrammer || IsScientific)
            {
                SetOpenParenthesisCountNarratorAnnouncement();
            }
```

| Item | Value / behaviour | file:line |
|---|---|---|
| Field type | `private uint _openParenthesisCount;` | StandardCalculatorViewModel.cs:134 |
| Initial value | `0` | StandardCalculatorViewModel.cs:162 |
| Max value | none (unbounded `uint`) | — |
| Display | `ParenthesisCountToString(uint count)` | src/Calculator/Views/CalculatorScientificOperators.xaml.cs:48 |
| Display | `ParenthesisCountToString(uint count)` | src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml.cs:39 |

### 6.7 Complete threshold-constant sweep

| Constant | Value | file:line |
|---|---|---|
| `StandardModePrecision` | 16 | StandardCalculatorViewModel.cs:36 |
| `ScientificModePrecision` | 32 | StandardCalculatorViewModel.cs:37 |
| `ProgrammerModePrecision` | 64 | StandardCalculatorViewModel.cs:38 |
| `MAX_HISTORY_ITEMS` | 20 | src/CalcManager/CalculatorManager.cpp:12 |
| `DEFAULT_MAX_DIGITS` | 32 | src/CalcManager/CEngine/calc.cpp:15 |
| `DEFAULT_PRECISION` | 32 | src/CalcManager/CEngine/calc.cpp:16 |
| `DEFAULT_RADIX` | 10 | src/CalcManager/CEngine/calc.cpp:17 |
| `MAX_STRLEN` | 84 | src/CalcManager/Header Files/CalcInput.h:9 |
| `C_NUM_MAX_DIGITS` | 84 | src/CalcManager/CEngine/CalcInput.cpp:10 |
| `C_EXP_MAX_DIGITS` | 4 | src/CalcManager/CEngine/CalcInput.cpp:11 |
| Bit-flip digit array | `new bool[64]` | StandardCalculatorViewModel.cs:150 |

## 7. CONVERTER DATA

### 7.1 JSON files in `src/Calculator.ViewModels/DataLoaders/`

| File | Size (bytes) | Notes |
|---|---|---|
| `CurrencyDataLoader.cs` | 36,560 | C# source |
| `CurrencyHttpClient.cs` | 3,739 | C# source |
| `DefaultFromToCurrency.json` | 1,754 | JSON |
| `UnitConverterDataLoader.cs` | 49,625 | C# source |

**Exactly one JSON file:** `DefaultFromToCurrency.json` (1,754 bytes). It is a locale→default-currency-pair map, **not** unit conversion data. Contents are *real* currency codes (USD/EUR/GBP/JPY/INR/…) and match the shipped `en-US` locale.

Non-JSON loaders in the same folder:

| File | Size | Role |
|---|---|---|
| `CurrencyDataLoader.cs` | 36,560 | Currency category loader (consumes the mock HTTP client) |
| `CurrencyHttpClient.cs` | 3,739 | **MOCK** HTTP client (see §9) |
| `UnitConverterDataLoader.cs` | 49,625 | All 12 non-currency unit categories + factors |

### 7.2 Category list (unit converter)

```csharp
// src/Calculator.ViewModels/DataLoaders/UnitConverterDataLoader.cs:225-231
            var converterModes = new[]
            {
                ViewMode.Currency,
                ViewMode.Volume, ViewMode.Length, ViewMode.Weight,
                ViewMode.Temperature, ViewMode.Energy, ViewMode.Area,
                ViewMode.Speed, ViewMode.Time, ViewMode.Power,
                ViewMode.Data, ViewMode.Pressure, ViewMode.Angle
```

| # | ViewMode | file:line (in `converterModes`) | Units | Factor entries |
|---|---|---|---|
| 0 | `Currency` | UnitConverterDataLoader.cs:226 | 0 (delegates to `CurrencyDataLoader`) | 0 |
| 1 | `Volume` | UnitConverterDataLoader.cs:227 | 23 | 23 |
| 2 | `Length` | UnitConverterDataLoader.cs:228 | 15 | 15 |
| 3 | `Weight` | UnitConverterDataLoader.cs:229 | 18 | 18 |
| 4 | `Temperature` | UnitConverterDataLoader.cs:230 | 3 | 0 |
| 5 | `Energy` | UnitConverterDataLoader.cs:231 | 11 | 11 |
| 6 | `Area` | UnitConverterDataLoader.cs:232 | 15 | 15 |
| 7 | `Speed` | UnitConverterDataLoader.cs:233 | 10 | 10 |
| 8 | `Time` | UnitConverterDataLoader.cs:234 | 8 | 8 |
| 9 | `Power` | UnitConverterDataLoader.cs:235 | 8 | 8 |
| 10 | `Data` | UnitConverterDataLoader.cs:236 | 38 | 38 |
| 11 | `Pressure` | UnitConverterDataLoader.cs:237 | 6 | 6 |
| 12 | `Angle` | UnitConverterDataLoader.cs:238 | 3 | 3 |

### 7.3 Region-dependent flags

```csharp
// src/Calculator.ViewModels/DataLoaders/UnitConverterDataLoader.cs:213-221

            // Determine region-dependent flags
            bool useUSCustomaryAndFahrenheit =
                _regionCode == "US" || _regionCode == "FM" || _regionCode == "MH" || _regionCode == "PW";
            bool useUSCustomary = useUSCustomaryAndFahrenheit || _regionCode == "LR";
            bool useSI = !useUSCustomary;
            bool useFahrenheit = useUSCustomaryAndFahrenheit || _regionCode == "BS" || _regionCode == "KY" || _regionCode == "LR";
            bool useWattInsteadOfKilowatt = _regionCode == "GB";
            bool usePyeong = _regionCode == "JP" || _regionCode == "TW" || _regionCode == "KP" || _regionCode == "KR";
```

### 7.4 Every unit + factor — REAL DATA VERDICT

**VERDICT: REAL.** The conversion factors are the authentic published SI/US-customary conversion constants (1 acre = 4046.8564224 m², 1 BTU = 1055.05585262 J, 1 psi = 6894.757293168 Pa, …). They are *not* placeholders: no values repeat as `1.0`/`2.0` filler, they are full double-precision literals with 15-17 significant digits, and they reproduce the exact C++ `GetConversionData` table (`src/CalcManager/`).

#### Volume

| Id | Const | Order | Name (resw → value) | Abbreviation (resw → value) | Factor | Whimsical | file:line |
|---|---|---|---|---|---|---|---|
| 75 | `Volume_Milliliter` | 1 | `UnitName_Milliliter` → Milliliters | `UnitAbbreviation_Milliliter` → mL | `1` <br>`UCD:588` |  | UCD:414 (const at UCD:123) |
| 64 | `Volume_CubicCentimeter` | 2 | `UnitName_CubicCentimeter` → Cubic centimeters | `UnitAbbreviation_CubicCentimeter` → cm³ | `1` <br>`UCD:588` |  | UCD:403 (const at UCD:112) |
| 74 | `Volume_Liter` | 3 | `UnitName_Liter` → Liters | `UnitAbbreviation_Liter` → L | `1000` <br>`UCD:588` |  | UCD:413 (const at UCD:122) |
| 67 | `Volume_CubicMeter` | 4 | `UnitName_CubicMeter` → Cubic meters | `UnitAbbreviation_CubicMeter` → m³ | `1000000` <br>`UCD:588` |  | UCD:406 (const at UCD:115) |
| 79 | `Volume_TeaspoonUS` | 5 | `UnitName_TeaspoonUS` → Teaspoons (US) | `UnitAbbreviation_TeaspoonUS` → tsp. (US) | `4.92892159375` <br>`UCD:588` |  | UCD:418 (const at UCD:127) |
| 78 | `Volume_TablespoonUS` | 6 | `UnitName_TablespoonUS` → Tablespoons (US) | `UnitAbbreviation_TablespoonUS` → tbsp. (US) | `14.78676478125` <br>`UCD:588` |  | UCD:417 (const at UCD:126) |
| 71 | `Volume_FluidOunceUS` | 7 | `UnitName_FluidOunceUS` → Fluid ounces (US) | `UnitAbbreviation_FluidOunceUS` → fl oz (US) | `29.5735295625` <br>`UCD:588` |  | UCD:410 (const at UCD:119) |
| 69 | `Volume_CupUS` | 8 | `UnitName_CupUS` → Cups (US) | `UnitAbbreviation_CupUS` → cup (US) | `236.588237` <br>`UCD:588` |  | UCD:408 (const at UCD:117) |
| 77 | `Volume_PintUS` | 9 | `UnitName_PintUS` → Pints (US) | `UnitAbbreviation_PintUS` → pt (US) | `473.176473` <br>`UCD:588` |  | UCD:416 (const at UCD:125) |
| 81 | `Volume_QuartUS` | 10 | `UnitName_QuartUS` → Quarts (US) | `UnitAbbreviation_QuartUS` → qt (US) | `946.352946` <br>`UCD:588` |  | UCD:420 (const at UCD:129) |
| 73 | `Volume_GallonUS` | 11 | `UnitName_GallonUS` → Gallons (US) | `UnitAbbreviation_GallonUS` → gal (US) | `3785.411784` <br>`UCD:588` |  | UCD:412 (const at UCD:121) |
| 66 | `Volume_CubicInch` | 12 | `UnitName_CubicInch` → Cubic inches | `UnitAbbreviation_CubicInch` → in³ | `16.387064` <br>`UCD:588` |  | UCD:405 (const at UCD:114) |
| 65 | `Volume_CubicFoot` | 13 | `UnitName_CubicFoot` → Cubic feet | `UnitAbbreviation_CubicFoot` → ft³ | `28316.846592` <br>`UCD:588` |  | UCD:404 (const at UCD:113) |
| 68 | `Volume_CubicYard` | 14 | `UnitName_CubicYard` → Cubic yards | `UnitAbbreviation_CubicYard` → yd³ | `764554.857984` <br>`UCD:588` |  | UCD:407 (const at UCD:116) |
| 115 | `Volume_TeaspoonUK` | 15 | `UnitName_TeaspoonUK` → Teaspoons (UK) | `UnitAbbreviation_TeaspoonUK` → tsp. (UK) | `5.91938802083333333333` <br>`UCD:588` |  | UCD:421 (const at UCD:156) |
| 116 | `Volume_TablespoonUK` | 16 | `UnitName_TablespoonUK` → Tablespoons (UK) | `UnitAbbreviation_TablespoonUK` → tbsp. (UK) | `17.7581640625` <br>`UCD:588` |  | UCD:422 (const at UCD:157) |
| 70 | `Volume_FluidOunceUK` | 17 | `UnitName_FluidOunceUK` → Fluid ounces (UK) | `UnitAbbreviation_FluidOunceUK` → fl oz (UK) | `28.4130625` <br>`UCD:588` |  | UCD:409 (const at UCD:118) |
| 76 | `Volume_PintUK` | 18 | `UnitName_PintUK` → Pints (UK) | `UnitAbbreviation_PintUK` → pt (UK) | `568.26125` <br>`UCD:588` |  | UCD:415 (const at UCD:124) |
| 80 | `Volume_QuartUK` | 19 | `UnitName_QuartUK` → Quarts (UK) | `UnitAbbreviation_QuartUK` → qt (UK) | `1136.5225` <br>`UCD:588` |  | UCD:419 (const at UCD:128) |
| 72 | `Volume_GallonUK` | 20 | `UnitName_GallonUK` → Gallons (UK) | `UnitAbbreviation_GallonUK` → gal (UK) | `4546.09` <br>`UCD:588` |  | UCD:411 (const at UCD:120) |
| 124 | `Volume_CoffeeCup` | 22 | `UnitName_CoffeeCup` → coffee cups | `UnitAbbreviation_CoffeeCup` → coffee cups | `236.5882` <br>`UCD:588` | yes | UCD:423 (const at UCD:161) |
| 111 | `Volume_Bathtub` | 23 | `UnitName_Bathtub` → bathtubs | `UnitAbbreviation_Bathtub` → bathtubs | `378541.2` <br>`UCD:588` | yes | UCD:424 (const at UCD:153) |
| 125 | `Volume_SwimmingPool` | 24 | `UnitName_SwimmingPool` → swimming pools | `UnitAbbreviation_SwimmingPool` → swimming pools | `3750000000` <br>`UCD:588` | yes | UCD:425 (const at UCD:163) |

**Volume: 23 units, 23 factor entries.**

#### Length

| Id | Const | Order | Name (resw → value) | Abbreviation (resw → value) | Factor | Whimsical | file:line |
|---|---|---|---|---|---|---|---|
| 168 | `Length_Angstrom` | 1 | `UnitName_Angstrom` → Angstroms | `UnitAbbreviation_Angstrom` → A | `0.0000000001` <br>`UCD:549` |  | UCD:349 (const at UCD:206) |
| 38 | `Length_Nanometer` | 2 | `UnitName_Nanometer` → Nanometers | `UnitAbbreviation_Nanometer` → nm | `0.000000001` <br>`UCD:549` |  | UCD:358 (const at UCD:86) |
| 35 | `Length_Micron` | 3 | `UnitName_Micron` → Microns | `UnitAbbreviation_Micron` → µm | `0.000001` <br>`UCD:549` |  | UCD:355 (const at UCD:83) |
| 37 | `Length_Millimeter` | 4 | `UnitName_Millimeter` → Millimeters | `UnitAbbreviation_Millimeter` → mm | `0.001` <br>`UCD:549` |  | UCD:357 (const at UCD:85) |
| 30 | `Length_Centimeter` | 5 | `UnitName_Centimeter` → Centimeters | `UnitAbbreviation_Centimeter` → cm | `0.01` <br>`UCD:549` |  | UCD:350 (const at UCD:78) |
| 34 | `Length_Meter` | 6 | `UnitName_Meter` → Meters | `UnitAbbreviation_Meter` → m | `1` <br>`UCD:549` |  | UCD:354 (const at UCD:82) |
| 33 | `Length_Kilometer` | 7 | `UnitName_Kilometer` → Kilometers | `UnitAbbreviation_Kilometer` → km | `1000` <br>`UCD:549` |  | UCD:353 (const at UCD:81) |
| 32 | `Length_Inch` | 8 | `UnitName_Inch` → Inches | `UnitAbbreviation_Inch` → in | `0.0254` <br>`UCD:549` |  | UCD:352 (const at UCD:80) |
| 31 | `Length_Foot` | 9 | `UnitName_Foot` → Feet | `UnitAbbreviation_Foot` → ft | `0.3048` <br>`UCD:549` |  | UCD:351 (const at UCD:79) |
| 40 | `Length_Yard` | 10 | `UnitName_Yard` → Yards | `UnitAbbreviation_Yard` → yd | `0.9144` <br>`UCD:549` |  | UCD:360 (const at UCD:88) |
| 36 | `Length_Mile` | 11 | `UnitName_Mile` → Miles | `UnitAbbreviation_Mile` → mi | `1609.344` <br>`UCD:549` |  | UCD:356 (const at UCD:84) |
| 39 | `Length_NauticalMile` | 12 | `UnitName_NauticalMile` → Nautical miles | `UnitAbbreviation_NauticalMile` → nmi | `1852` <br>`UCD:549` |  | UCD:359 (const at UCD:87) |
| 105 | `Length_Paperclip` | 13 | `UnitName_Paperclip` → paperclips | `UnitAbbreviation_Paperclip` → paperclips | `0.035052` <br>`UCD:549` | yes | UCD:361 (const at UCD:149) |
| 131 | `Length_Hand` | 14 | `UnitName_Hand` → hands | `UnitAbbreviation_Hand` → hands | `0.18669` <br>`UCD:549` | yes | UCD:362 (const at UCD:169) |
| 107 | `Length_JumboJet` | 15 | `UnitName_JumboJet` → jumbo jets | `UnitAbbreviation_JumboJet` → jumbo jets | `76` <br>`UCD:549` | yes | UCD:363 (const at UCD:150) |

**Length: 15 units, 15 factor entries.**

#### Weight

| Id | Const | Order | Name (resw → value) | Abbreviation (resw → value) | Factor | Whimsical | file:line |
|---|---|---|---|---|---|---|---|
| 82 | `Weight_Carat` | 1 | `UnitName_Carat` → Carats | `UnitAbbreviation_Carat` → CD | `0.0002` <br>`UCD:615` |  | UCD:428 (const at UCD:130) |
| 90 | `Weight_Milligram` | 2 | `UnitName_Milligram` → Milligrams | `UnitAbbreviation_Milligram` → mg | `0.000001` <br>`UCD:615` |  | UCD:436 (const at UCD:138) |
| 83 | `Weight_Centigram` | 3 | `UnitName_Centigram` → Centigrams | `UnitAbbreviation_Centigram` → cg | `0.00001` <br>`UCD:615` |  | UCD:429 (const at UCD:131) |
| 84 | `Weight_Decigram` | 4 | `UnitName_Decigram` → Decigrams | `UnitAbbreviation_Decigram` → dg | `0.0001` <br>`UCD:615` |  | UCD:430 (const at UCD:132) |
| 86 | `Weight_Gram` | 5 | `UnitName_Gram` → Grams | `UnitAbbreviation_Gram` → g | `0.001` <br>`UCD:615` |  | UCD:432 (const at UCD:134) |
| 85 | `Weight_Decagram` | 6 | `UnitName_Decagram` → Dekagrams | `UnitAbbreviation_Decagram` → dag | `0.01` <br>`UCD:615` |  | UCD:431 (const at UCD:133) |
| 87 | `Weight_Hectogram` | 7 | `UnitName_Hectogram` → Hectograms | `UnitAbbreviation_Hectogram` → hg | `0.1` <br>`UCD:615` |  | UCD:433 (const at UCD:135) |
| 88 | `Weight_Kilogram` | 8 | `UnitName_Kilogram` → Kilograms | `UnitAbbreviation_Kilogram` → kg | `1` <br>`UCD:615` |  | UCD:434 (const at UCD:136) |
| 95 | `Weight_Tonne` | 9 | `UnitName_Tonne` → Metric tonnes | `UnitAbbreviation_Tonne` → t | `1000` <br>`UCD:615` |  | UCD:441 (const at UCD:143) |
| 91 | `Weight_Ounce` | 10 | `UnitName_Ounce` → Ounces | `UnitAbbreviation_Ounce` → oz | `0.028349523125` <br>`UCD:615` |  | UCD:437 (const at UCD:139) |
| 92 | `Weight_Pound` | 11 | `UnitName_Pound` → Pounds | `UnitAbbreviation_Pound` → lb | `0.45359237` <br>`UCD:615` |  | UCD:438 (const at UCD:140) |
| 94 | `Weight_Stone` | 12 | `UnitName_Stone` → Stone | `UnitAbbreviation_Stone` → st | `6.35029318` <br>`UCD:615` |  | UCD:440 (const at UCD:142) |
| 93 | `Weight_ShortTon` | 13 | `UnitName_ShortTon` → Short tons (US) | `UnitAbbreviation_ShortTon` → ton (US) | `907.18474` <br>`UCD:615` |  | UCD:439 (const at UCD:141) |
| 89 | `Weight_LongTon` | 14 | `UnitName_LongTon` → Long tons (UK) | `UnitAbbreviation_LongTon` → ton (UK) | `1016.0469088` <br>`UCD:615` |  | UCD:435 (const at UCD:137) |
| 113 | `Weight_Snowflake` | 15 | `UnitName_Snowflake` → snowflakes | `UnitAbbreviation_Snowflake` → snowflakes | `0.000002` <br>`UCD:615` | yes | UCD:442 (const at UCD:154) |
| 133 | `Weight_SoccerBall` | 16 | `UnitName_SoccerBall` → soccer balls | `UnitAbbreviation_SoccerBall` → soccer balls | `0.4325` <br>`UCD:615` | yes | UCD:443 (const at UCD:171) |
| 114 | `Weight_Elephant` | 17 | `UnitName_Elephant` → elephants | `UnitAbbreviation_Elephant` → elephants | `4000` <br>`UCD:615` | yes | UCD:444 (const at UCD:155) |
| 123 | `Weight_Whale` | 18 | `UnitName_Whale` → whales | `UnitAbbreviation_Whale` → whales | `90000` <br>`UCD:615` | yes | UCD:445 (const at UCD:162) |

**Weight: 18 units, 18 factor entries.**

#### Temperature

| Id | Const | Order | Name (resw → value) | Abbreviation (resw → value) | Factor | Whimsical | file:line |
|---|---|---|---|---|---|---|---|
| 46 | `Temperature_DegreesCelsius` | 1 | `UnitName_DegreesCelsius` → Celsius | `UnitAbbreviation_DegreesCelsius` → °C | _(explicit table)_ <br>`UCD:668-687` |  | UCD:376 (const at UCD:94) |
| 47 | `Temperature_DegreesFahrenheit` | 2 | `UnitName_DegreesFahrenheit` → Fahrenheit | `UnitAbbreviation_DegreesFahrenheit` → °F | _(explicit table)_ <br>`UCD:668-687` |  | UCD:377 (const at UCD:95) |
| 48 | `Temperature_Kelvin` | 3 | `UnitName_Kelvin` → Kelvin | `UnitAbbreviation_Kelvin` → K | _(explicit table)_ <br>`UCD:668-687` |  | UCD:378 (const at UCD:96) |

**Temperature: 3 units, 0 factor entries.**

#### Energy

| Id | Const | Order | Name (resw → value) | Abbreviation (resw → value) | Factor | Whimsical | file:line |
|---|---|---|---|---|---|---|---|
| 25 | `Energy_ElectronVolt` | 1 | `UnitName_Electron-Volt` → Electron volts | `UnitAbbreviation_Electron-Volt` → eV | `0.0000000000000000001602176565` <br>`UCD:534` |  | UCD:338 (const at UCD:73) |
| 27 | `Energy_Joule` | 2 | `UnitName_Joule` → Joules | `UnitAbbreviation_Joule` → J | `1` <br>`UCD:534` |  | UCD:340 (const at UCD:75) |
| 29 | `Energy_Kilojoule` | 3 | `UnitName_Kilojoule` → Kilojoules | `UnitAbbreviation_Kilojoule` → kJ | `1000` <br>`UCD:534` |  | UCD:343 (const at UCD:77) |
| 24 | `Energy_Calorie` | 4 | `UnitName_Calorie` → Thermal calories | `UnitAbbreviation_Calorie` → cal | `4.184` <br>`UCD:534` |  | UCD:337 (const at UCD:72) |
| 28 | `Energy_Kilocalorie` | 5 | `UnitName_Kilocalorie` → Food calories | `UnitAbbreviation_Kilocalorie` → kcal | `4184` <br>`UCD:534` |  | UCD:342 (const at UCD:76) |
| 26 | `Energy_FootPound` | 6 | `UnitName_Foot-Pound` → Foot-pounds | `UnitAbbreviation_Foot-Pound` → ft•lb | `1.3558179483314` <br>`UCD:534` |  | UCD:339 (const at UCD:74) |
| 23 | `Energy_BritishThermalUnit` | 7 | `UnitName_BritishThermalUnit` → British thermal units | `UnitAbbreviation_BritishThermalUnit` → BTU | `1055.056` <br>`UCD:534` |  | UCD:336 (const at UCD:71) |
| 103 | `Energy_Battery` | 8 | `UnitName_Battery` → batteries | `UnitAbbreviation_Battery` → batteries | `9000` <br>`UCD:534` | yes | UCD:344 (const at UCD:148) |
| 129 | `Energy_Banana` | 9 | `UnitName_Banana` → bananas | `UnitAbbreviation_Banana` → bananas | `439614` <br>`UCD:534` | yes | UCD:345 (const at UCD:167) |
| 130 | `Energy_SliceOfCake` | 10 | `UnitName_SliceOfCake` → slices of cake | `UnitAbbreviation_SliceOfCake` → slices of cake | `1046700` <br>`UCD:534` | yes | UCD:346 (const at UCD:168) |
| 166 | `Energy_Kilowatthour` | 166 | `UnitName_Kilowatthour` → Kilowatt-hours | `UnitAbbreviation_Kilowatthour` → kWh | `3600000` <br>`UCD:534` |  | UCD:341 (const at UCD:204) |

**Energy: 11 units, 11 factor entries.**

#### Area

| Id | Const | Order | Name (resw → value) | Abbreviation (resw → value) | Factor | Whimsical | file:line |
|---|---|---|---|---|---|---|---|
| 9 | `Area_SquareMillimeter` | 1 | `UnitName_SquareMillimeter` → Square millimeters | `UnitAbbreviation_SquareMillimeter` → mm² | `0.000001` <br>`UCD:476` |  | UCD:284 (const at UCD:57) |
| 3 | `Area_SquareCentimeter` | 2 | `UnitName_SquareCentimeter` → Square centimeters | `UnitAbbreviation_SquareCentimeter` → cm² | `0.0001` <br>`UCD:476` |  | UCD:278 (const at UCD:51) |
| 7 | `Area_SquareMeter` | 3 | `UnitName_SquareMeter` → Square meters | `UnitAbbreviation_SquareMeter` → m² | `1` <br>`UCD:476` |  | UCD:282 (const at UCD:55) |
| 2 | `Area_Hectare` | 4 | `UnitName_Hectare` → Hectares | `UnitAbbreviation_Hectare` → ha | `10000` <br>`UCD:476` |  | UCD:277 (const at UCD:50) |
| 6 | `Area_SquareKilometer` | 5 | `UnitName_SquareKilometer` → Square kilometers | `UnitAbbreviation_SquareKilometer` → km² | `1000000` <br>`UCD:476` |  | UCD:281 (const at UCD:54) |
| 5 | `Area_SquareInch` | 6 | `UnitName_SquareInch` → Square inches | `UnitAbbreviation_SquareInch` → in² | `0.00064516` <br>`UCD:476` |  | UCD:280 (const at UCD:53) |
| 4 | `Area_SquareFoot` | 7 | `UnitName_SquareFoot` → Square feet | `UnitAbbreviation_SquareFoot` → ft² | `0.09290304` <br>`UCD:476` |  | UCD:279 (const at UCD:52) |
| 10 | `Area_SquareYard` | 8 | `UnitName_SquareYard` → Square yards | `UnitAbbreviation_SquareYard` → yd² | `0.83612736` <br>`UCD:476` |  | UCD:285 (const at UCD:58) |
| 1 | `Area_Acre` | 9 | `UnitName_Acre` → Acres | `UnitAbbreviation_Acre` → ac | `4046.8564224` <br>`UCD:476` |  | UCD:276 (const at UCD:49) |
| 8 | `Area_SquareMile` | 10 | `UnitName_SquareMile` → Square miles | `UnitAbbreviation_SquareMile` → mi² | `2589988.110336` <br>`UCD:476` |  | UCD:283 (const at UCD:56) |
| 118 | `Area_Hand` | 11 | `UnitName_Hand` → hands | `UnitAbbreviation_Hand` → hands | `0.012516104` <br>`UCD:476` | yes | UCD:286 (const at UCD:158) |
| 127 | `Area_Paper` | 12 | `UnitName_Paper` → sheets of paper | `UnitAbbreviation_Paper` → sheets of paper | `0.06032246` <br>`UCD:476` | yes | UCD:287 (const at UCD:165) |
| 99 | `Area_SoccerField` | 13 | `UnitName_SoccerField` → soccer fields | `UnitAbbreviation_SoccerField` → soccer fields | `10869.66` <br>`UCD:476` | yes | UCD:288 (const at UCD:144) |
| 128 | `Area_Castle` | 14 | `UnitName_Castle` → castles | `UnitAbbreviation_Castle` → castles | `100000` <br>`UCD:476` | yes | UCD:289 (const at UCD:166) |
| 165 | `Area_Pyeong` | 15 | `UnitName_Pyeong` → Pyeong | `UnitAbbreviation_Pyeong` → Pyeong | `400.0 / 121.0` <br>`UCD:476` |  | UCD:292 (const at UCD:203) |

**Area: 15 units, 15 factor entries.**

#### Speed

| Id | Const | Order | Name (resw → value) | Abbreviation (resw → value) | Factor | Whimsical | file:line |
|---|---|---|---|---|---|---|---|
| 57 | `Speed_CentimetersPerSecond` | 1 | `UnitName_CentimetersPerSecond` → Centimeters per second | `UnitAbbreviation_CentimetersPerSecond` → cm/s | `1` <br>`UCD:635` |  | UCD:391 (const at UCD:105) |
| 62 | `Speed_MetersPerSecond` | 2 | `UnitName_MetersPerSecond` → Meters per second | `UnitAbbreviation_MetersPerSecond` → m/s | `100` <br>`UCD:635` |  | UCD:396 (const at UCD:110) |
| 59 | `Speed_KilometersPerHour` | 3 | `UnitName_KilometersPerHour` → Kilometers per hour | `UnitAbbreviation_KilometersPerHour` → km/h | `27.777777777777777777778` <br>`UCD:635` |  | UCD:393 (const at UCD:107) |
| 58 | `Speed_FeetPerSecond` | 4 | `UnitName_FeetPerSecond` → Feet per second | `UnitAbbreviation_FeetPerSecond` → ft/s | `30.48` <br>`UCD:635` |  | UCD:392 (const at UCD:106) |
| 63 | `Speed_MilesPerHour` | 5 | `UnitName_MilesPerHour` → Miles per hour | `UnitAbbreviation_MilesPerHour` → mph | `44.7` <br>`UCD:635` |  | UCD:397 (const at UCD:111) |
| 60 | `Speed_Knot` | 6 | `UnitName_Knot` → Knots | `UnitAbbreviation_Knot` → kn | `51.44` <br>`UCD:635` |  | UCD:394 (const at UCD:108) |
| 61 | `Speed_Mach` | 7 | `UnitName_Mach` → Mach | `UnitAbbreviation_Mach` → M | `34030` <br>`UCD:635` |  | UCD:395 (const at UCD:109) |
| 121 | `Speed_Turtle` | 8 | `UnitName_Turtle` → turtles | `UnitAbbreviation_Turtle` → turtles | `8.94` <br>`UCD:635` | yes | UCD:398 (const at UCD:159) |
| 126 | `Speed_Horse` | 9 | `UnitName_Horse` → horses | `UnitAbbreviation_Horse` → horses | `2011.5` <br>`UCD:635` | yes | UCD:399 (const at UCD:164) |
| 122 | `Speed_Jet` | 10 | `UnitName_Jet` → jets | `UnitAbbreviation_Jet` → jets | `24585` <br>`UCD:635` | yes | UCD:400 (const at UCD:160) |

**Speed: 10 units, 10 factor entries.**

#### Time

| Id | Const | Order | Name (resw → value) | Abbreviation (resw → value) | Factor | Whimsical | file:line |
|---|---|---|---|---|---|---|---|
| 51 | `Time_Microsecond` | 1 | `UnitName_Microsecond` → Microseconds | `UnitAbbreviation_Microsecond` → µs | `0.000001` <br>`UCD:578` |  | UCD:383 (const at UCD:99) |
| 52 | `Time_Millisecond` | 2 | `UnitName_Millisecond` → Milliseconds | `UnitAbbreviation_Millisecond` → ms | `0.001` <br>`UCD:578` |  | UCD:384 (const at UCD:100) |
| 54 | `Time_Second` | 3 | `UnitName_Second` → Seconds | `UnitAbbreviation_Second` → s | `1` <br>`UCD:578` |  | UCD:386 (const at UCD:102) |
| 53 | `Time_Minute` | 4 | `UnitName_Minute` → Minutes | `UnitAbbreviation_Minute` → min | `60` <br>`UCD:578` |  | UCD:385 (const at UCD:101) |
| 50 | `Time_Hour` | 5 | `UnitName_Hour` → Hours | `UnitAbbreviation_Hour` → hr | `3600` <br>`UCD:578` |  | UCD:382 (const at UCD:98) |
| 49 | `Time_Day` | 6 | `UnitName_Day` → Days | `UnitAbbreviation_Day` → d | `86400` <br>`UCD:578` |  | UCD:381 (const at UCD:97) |
| 55 | `Time_Week` | 7 | `UnitName_Week` → Weeks | `UnitAbbreviation_Week` → wk | `604800` <br>`UCD:578` |  | UCD:387 (const at UCD:103) |
| 56 | `Time_Year` | 8 | `UnitName_Year` → Years | `UnitAbbreviation_Year` → yr | `31557600` <br>`UCD:578` |  | UCD:388 (const at UCD:104) |

**Time: 8 units, 8 factor entries.**

#### Power

| Id | Const | Order | Name (resw → value) | Abbreviation (resw → value) | Factor | Whimsical | file:line |
|---|---|---|---|---|---|---|---|
| 45 | `Power_Watt` | 1 | `UnitName_Watt` → Watts | `UnitAbbreviation_Watt` → W | `1` <br>`UCD:566` |  | UCD:370 (const at UCD:93) |
| 44 | `Power_Kilowatt` | 2 | `UnitName_Kilowatt` → Kilowatts | `UnitAbbreviation_Kilowatt` → kW | `1000` <br>`UCD:566` |  | UCD:369 (const at UCD:92) |
| 43 | `Power_Horsepower` | 3 | `UnitName_Horsepower` → Horsepower (US) | `UnitAbbreviation_Horsepower` → hp (US) | `745.69987158227022` <br>`UCD:566` |  | UCD:368 (const at UCD:91) |
| 42 | `Power_FootPoundPerMinute` | 4 | `UnitName_Foot-PoundPerMinute` → Foot-pounds/minute | `UnitAbbreviation_Foot-PoundPerMinute` → ft•lb/min | `0.0225969658055233` <br>`UCD:566` |  | UCD:367 (const at UCD:90) |
| 41 | `Power_BritishThermalUnitPerMinute` | 5 | `UnitName_BTUPerMinute` → BTUs/minute | `UnitAbbreviation_BTUPerMinute` → BTU/min | `17.58426666666667` <br>`UCD:566` |  | UCD:366 (const at UCD:89) |
| 108 | `Power_LightBulb` | 6 | `UnitName_LightBulb` → light bulbs | `UnitAbbreviation_LightBulb` → light bulbs | `60` <br>`UCD:566` | yes | UCD:371 (const at UCD:151) |
| 109 | `Power_Horse` | 7 | `UnitName_Horse` → horses | `UnitAbbreviation_Horse` → horses | `745.7` <br>`UCD:566` | yes | UCD:372 (const at UCD:152) |
| 132 | `Power_TrainEngine` | 8 | `UnitName_TrainEngine` → train engines | `UnitAbbreviation_TrainEngine` → train engines | `2982799.486329081` <br>`UCD:566` | yes | UCD:373 (const at UCD:170) |

**Power: 8 units, 8 factor entries.**

#### Data

| Id | Const | Order | Name (resw → value) | Abbreviation (resw → value) | Factor | Whimsical | file:line |
|---|---|---|---|---|---|---|---|
| 11 | `Data_Bit` | 1 | `UnitName_Bit` → Bits | `UnitAbbreviation_Bit` → b | `0.000000125` <br>`UCD:494` |  | UCD:296 (const at UCD:59) |
| 167 | `Data_Nibble` | 2 | `UnitName_Nibble` → Nibble | `UnitAbbreviation_Nibble` → nybl | `0.0000005` <br>`UCD:494` |  | UCD:314 (const at UCD:205) |
| 12 | `Data_Byte` | 3 | `UnitName_Byte` → Bytes | `UnitAbbreviation_Byte` → B | `0.000001` <br>`UCD:494` |  | UCD:297 (const at UCD:60) |
| 15 | `Data_Kilobit` | 4 | `UnitName_Kilobit` → Kilobits | `UnitAbbreviation_Kilobit` → Kb | `0.000125` <br>`UCD:494` |  | UCD:308 (const at UCD:63) |
| 149 | `Data_Kibibits` | 5 | `UnitName_Kibibits` → Kibibits | `UnitAbbreviation_Kibibits` → Ki | `0.000128` <br>`UCD:494` |  | UCD:306 (const at UCD:187) |
| 16 | `Data_Kilobyte` | 6 | `UnitName_Kilobyte` → Kilobytes | `UnitAbbreviation_Kilobyte` → KB | `0.001` <br>`UCD:494` |  | UCD:309 (const at UCD:64) |
| 150 | `Data_Kibibytes` | 7 | `UnitName_Kibibytes` → Kibibytes | `UnitAbbreviation_Kibibytes` → KiB | `0.001024` <br>`UCD:494` |  | UCD:307 (const at UCD:188) |
| 17 | `Data_Megabit` | 8 | `UnitName_Megabit` → Megabits | `UnitAbbreviation_Megabit` → Mb | `0.125` <br>`UCD:494` |  | UCD:312 (const at UCD:65) |
| 151 | `Data_Mebibits` | 9 | `UnitName_Mebibits` → Mebibits | `UnitAbbreviation_Mebibits` → Mi | `0.131072` <br>`UCD:494` |  | UCD:310 (const at UCD:189) |
| 18 | `Data_Megabyte` | 10 | `UnitName_Megabyte` → Megabytes | `UnitAbbreviation_Megabyte` → MB | `1` <br>`UCD:494` |  | UCD:313 (const at UCD:66) |
| 152 | `Data_Mebibytes` | 11 | `UnitName_Mebibytes` → Mebibytes | `UnitAbbreviation_Mebibytes` → MiB | `1.048576` <br>`UCD:494` |  | UCD:311 (const at UCD:190) |
| 13 | `Data_Gigabit` | 12 | `UnitName_Gigabit` → Gigabits | `UnitAbbreviation_Gigabit` → Gb | `125` <br>`UCD:494` |  | UCD:304 (const at UCD:61) |
| 147 | `Data_Gibibits` | 13 | `UnitName_Gibibits` → Gibibits | `UnitAbbreviation_Gibibits` → Gi | `134.217728` <br>`UCD:494` |  | UCD:302 (const at UCD:185) |
| 100 | `Data_FloppyDisk` | 13 | `UnitName_FloppyDisk` → floppy disks | `UnitAbbreviation_FloppyDisk` → floppy disks | `1.474560` <br>`UCD:494` | yes | UCD:331 (const at UCD:145) |
| 14 | `Data_Gigabyte` | 14 | `UnitName_Gigabyte` → Gigabytes | `UnitAbbreviation_Gigabyte` → GB | `1000` <br>`UCD:494` |  | UCD:305 (const at UCD:62) |
| 101 | `Data_CD` | 14 | `UnitName_CD` → CDs | `UnitAbbreviation_CD` → CDs | `700` <br>`UCD:494` | yes | UCD:332 (const at UCD:146) |
| 148 | `Data_Gibibytes` | 15 | `UnitName_Gibibytes` → Gibibytes | `UnitAbbreviation_Gibibytes` → GiB | `1073.741824` <br>`UCD:494` |  | UCD:303 (const at UCD:186) |
| 102 | `Data_DVD` | 15 | `UnitName_DVD` → DVDs | `UnitAbbreviation_DVD` → DVDs | `4700` <br>`UCD:494` | yes | UCD:333 (const at UCD:147) |
| 21 | `Data_Terabit` | 16 | `UnitName_Terabit` → Terabits | `UnitAbbreviation_Terabit` → Tb | `125000` <br>`UCD:494` |  | UCD:321 (const at UCD:69) |
| 155 | `Data_Tebibits` | 17 | `UnitName_Tebibits` → Tebibits | `UnitAbbreviation_Tebibits` → Ti | `137438.953472` <br>`UCD:494` |  | UCD:319 (const at UCD:193) |
| 22 | `Data_Terabyte` | 18 | `UnitName_Terabyte` → Terabytes | `UnitAbbreviation_Terabyte` → TB | `1000000` <br>`UCD:494` |  | UCD:322 (const at UCD:70) |
| 156 | `Data_Tebibytes` | 19 | `UnitName_Tebibytes` → Tebibytes | `UnitAbbreviation_Tebibytes` → TiB | `1099511.627776` <br>`UCD:494` |  | UCD:320 (const at UCD:194) |
| 19 | `Data_Petabit` | 20 | `UnitName_Petabit` → Petabits | `UnitAbbreviation_Petabit` → Pb | `125000000` <br>`UCD:494` |  | UCD:317 (const at UCD:67) |
| 153 | `Data_Pebibits` | 21 | `UnitName_Pebibits` → Pebibits | `UnitAbbreviation_Pebibits` → Pi | `140737488.355328` <br>`UCD:494` |  | UCD:315 (const at UCD:191) |
| 20 | `Data_Petabyte` | 22 | `UnitName_Petabyte` → Petabytes | `UnitAbbreviation_Petabyte` → PB | `1000000000` <br>`UCD:494` |  | UCD:318 (const at UCD:68) |
| 154 | `Data_Pebibytes` | 23 | `UnitName_Pebibytes` → Pebibytes | `UnitAbbreviation_Pebibytes` → PiB | `1125899906.842624` <br>`UCD:494` |  | UCD:316 (const at UCD:192) |
| 143 | `Data_Exabits` | 24 | `UnitName_Exabits` → Exabits | `UnitAbbreviation_Exabits` → E | `125000000000` <br>`UCD:494` |  | UCD:298 (const at UCD:181) |
| 145 | `Data_Exbibits` | 25 | `UnitName_Exbibits` → Exbibits | `UnitAbbreviation_Exbibits` → Ei | `144115188075.855872` <br>`UCD:494` |  | UCD:300 (const at UCD:183) |
| 144 | `Data_Exabytes` | 26 | `UnitName_Exabytes` → Exabytes | `UnitAbbreviation_Exabytes` → EB | `1000000000000` <br>`UCD:494` |  | UCD:299 (const at UCD:182) |
| 146 | `Data_Exbibytes` | 27 | `UnitName_Exbibytes` → Exbibytes | `UnitAbbreviation_Exbibytes` → EiB | `1152921504606.846976` <br>`UCD:494` |  | UCD:301 (const at UCD:184) |
| 163 | `Data_Zetabits` | 28 | `UnitName_Zetabits` → Zetabits | `UnitAbbreviation_Zetabits` → Z | `125000000000000` <br>`UCD:494` |  | UCD:329 (const at UCD:201) |
| 161 | `Data_Zebibits` | 29 | `UnitName_Zebibits` → Zebibits | `UnitAbbreviation_Zebibits` → Zi | `147573952589676.412928` <br>`UCD:494` |  | UCD:327 (const at UCD:199) |
| 164 | `Data_Zetabytes` | 30 | `UnitName_Zetabytes` → Zetabytes | `UnitAbbreviation_Zetabytes` → ZB | `1000000000000000` <br>`UCD:494` |  | UCD:330 (const at UCD:202) |
| 162 | `Data_Zebibytes` | 31 | `UnitName_Zebibytes` → Zebibytes | `UnitAbbreviation_Zebibytes` → ZiB | `1180591620717411.303424` <br>`UCD:494` |  | UCD:328 (const at UCD:200) |
| 159 | `Data_Yottabit` | 32 | `UnitName_Yottabit` → Yottabits | `UnitAbbreviation_Yottabit` → Y | `125000000000000000` <br>`UCD:494` |  | UCD:325 (const at UCD:197) |
| 157 | `Data_Yobibits` | 33 | `UnitName_Yobibits` → Yobibits | `UnitAbbreviation_Yobibits` → Yi | `151115727451828646.838272` <br>`UCD:494` |  | UCD:323 (const at UCD:195) |
| 160 | `Data_Yottabyte` | 34 | `UnitName_Yottabyte` → Yottabytes | `UnitAbbreviation_Yottabyte` → YB | `1000000000000000000` <br>`UCD:494` |  | UCD:326 (const at UCD:198) |
| 158 | `Data_Yobibytes` | 35 | `UnitName_Yobibytes` → Yobibytes | `UnitAbbreviation_Yobibytes` → YiB | `1208925819614629174.706176` <br>`UCD:494` |  | UCD:324 (const at UCD:196) |

**Data: 38 units, 38 factor entries.**

#### Pressure

| Id | Const | Order | Name (resw → value) | Abbreviation (resw → value) | Factor | Whimsical | file:line |
|---|---|---|---|---|---|---|---|
| 137 | `Pressure_Atmosphere` | 1 | `UnitName_Atmosphere` → Atmospheres | `UnitAbbreviation_Atmosphere` → atm | `1` <br>`UCD:654` |  | UCD:448 (const at UCD:175) |
| 138 | `Pressure_Bar` | 2 | `UnitName_Bar` → Bars | `UnitAbbreviation_Bar` → ba | `0.9869232667160128` <br>`UCD:654` |  | UCD:449 (const at UCD:176) |
| 139 | `Pressure_KiloPascal` | 3 | `UnitName_KiloPascal` → Kilopascals | `UnitAbbreviation_KiloPascal` → kPa | `0.0098692326671601` <br>`UCD:654` |  | UCD:450 (const at UCD:177) |
| 140 | `Pressure_MillimeterOfMercury` | 4 | `UnitName_MillimeterOfMercury ` → <<MISSING>> | `UnitAbbreviation_MillimeterOfMercury ` → <<MISSING>> | `0.0013155687145324` <br>`UCD:654` |  | UCD:451 (const at UCD:178) |
| 141 | `Pressure_Pascal` | 5 | `UnitName_Pascal` → Pascals | `UnitAbbreviation_Pascal` → Pa | `9.869232667160128e-6` <br>`UCD:654` |  | UCD:452 (const at UCD:179) |
| 142 | `Pressure_PSI` | 6 | `UnitName_PSI` → Pounds per square inch | `UnitAbbreviation_PSI` → psi | `0.068045961016531` <br>`UCD:654` |  | UCD:453 (const at UCD:180) |

**Pressure: 6 units, 6 factor entries.**

#### Angle

| Id | Const | Order | Name (resw → value) | Abbreviation (resw → value) | Factor | Whimsical | file:line |
|---|---|---|---|---|---|---|---|
| 134 | `Angle_Degree` | 1 | `UnitName_Degree` → Degrees | `UnitAbbreviation_Degree` → deg | `1` <br>`UCD:648` |  | UCD:456 (const at UCD:172) |
| 135 | `Angle_Radian` | 2 | `UnitName_Radian` → Radians | `UnitAbbreviation_Radian` → rad | `57.29577951308233` <br>`UCD:648` |  | UCD:457 (const at UCD:173) |
| 136 | `Angle_Gradian` | 3 | `UnitName_Gradian` → Gradians | `UnitAbbreviation_Gradian` → grad | `0.9` <br>`UCD:648` |  | UCD:458 (const at UCD:174) |

**Angle: 3 units, 3 factor entries.**

### 7.5 Temperature — explicit affine table (not factor-based)

```csharp
// src/Calculator.ViewModels/DataLoaders/UnitConverterDataLoader.cs:668-688
            // Build explicit conversion data (Temperature)
            var explicitConversions = new Dictionary<int, Dictionary<int, (double ratio, double offset, bool offsetFirst)>>
            {
                [Temperature_DegreesCelsius] = new Dictionary<int, (double, double, bool)>
                {
                    { Temperature_DegreesCelsius, (1, 0, false) },
                    { Temperature_DegreesFahrenheit, (1.8, 32, false) },
                    { Temperature_Kelvin, (1, 273.15, false) },
                },
                [Temperature_DegreesFahrenheit] = new Dictionary<int, (double, double, bool)>
                {
                    { Temperature_DegreesCelsius, (0.55555555555555555555555555555556, -32, true) },
                    { Temperature_DegreesFahrenheit, (1, 0, false) },
                    { Temperature_Kelvin, (0.55555555555555555555555555555556, 459.67, true) },
                },
                [Temperature_Kelvin] = new Dictionary<int, (double, double, bool)>
                {
                    { Temperature_DegreesCelsius, (1, -273.15, true) },
                    { Temperature_DegreesFahrenheit, (1.8, -459.67, false) },
                    { Temperature_Kelvin, (1, 0, false) },
                },
```

| From | To | Ratio | Offset | OffsetFirst |
|---|---|---|---|---|
| `Temperature_DegreesCelsius` | `Temperature_DegreesCelsius` | `1` | `0` | `false` |
| `Temperature_DegreesCelsius` | `Temperature_DegreesFahrenheit` | `1.8` | `32` | `false` |
| `Temperature_DegreesCelsius` | `Temperature_Kelvin` | `1` | `273.15` | `false` |
| `Temperature_DegreesFahrenheit` | `Temperature_DegreesCelsius` | `0.55555555555555555555555555555556` | `-32` | `true` |
| `Temperature_DegreesFahrenheit` | `Temperature_DegreesFahrenheit` | `1` | `0` | `false` |
| `Temperature_DegreesFahrenheit` | `Temperature_Kelvin` | `0.55555555555555555555555555555556` | `459.67` | `true` |
| `Temperature_Kelvin` | `Temperature_DegreesCelsius` | `1` | `-273.15` | `true` |
| `Temperature_Kelvin` | `Temperature_DegreesFahrenheit` | `1.8` | `-459.67` | `false` |
| `Temperature_Kelvin` | `Temperature_Kelvin` | `1` | `0` | `false` |

Consumption rule — `UnitConverterDataLoader.cs:722-737` vs `:739-758`:

```csharp
// explicit (temperature) wins:
if (explicitConversions.TryGetValue(sourceUnit.Id, out var explicitMap)) { /* Ratio, Offset, OffsetFirst */ }
// otherwise pure factor ratio, offset 0:
else if (conversionFactors.TryGetValue(categoryViewMode, out var categoryFactors))
    ... Ratio = sourceFactor / targetFactor, Offset = 0, OffsetFirst = false;
```

### 7.6 Whimsical / easter-egg units

| Unit | Const | Category | file:line |
|---|---|---|---|
| castles | `Area_Castle` | Area | UCD:289 |
| hands | `Area_Hand` | Area | UCD:286 |
| sheets of paper | `Area_Paper` | Area | UCD:287 |
| soccer fields | `Area_SoccerField` | Area | UCD:288 |
| CDs | `Data_CD` | Data | UCD:332 |
| DVDs | `Data_DVD` | Data | UCD:333 |
| floppy disks | `Data_FloppyDisk` | Data | UCD:331 |
| bananas | `Energy_Banana` | Energy | UCD:345 |
| batteries | `Energy_Battery` | Energy | UCD:344 |
| slices of cake | `Energy_SliceOfCake` | Energy | UCD:346 |
| hands | `Length_Hand` | Length | UCD:362 |
| jumbo jets | `Length_JumboJet` | Length | UCD:363 |
| paperclips | `Length_Paperclip` | Length | UCD:361 |
| horses | `Power_Horse` | Power | UCD:372 |
| light bulbs | `Power_LightBulb` | Power | UCD:371 |
| train engines | `Power_TrainEngine` | Power | UCD:373 |
| horses | `Speed_Horse` | Speed | UCD:399 |
| jets | `Speed_Jet` | Speed | UCD:400 |
| turtles | `Speed_Turtle` | Speed | UCD:398 |
| bathtubs | `Volume_Bathtub` | Volume | UCD:424 |
| coffee cups | `Volume_CoffeeCup` | Volume | UCD:423 |
| swimming pools | `Volume_SwimmingPool` | Volume | UCD:425 |
| elephants | `Weight_Elephant` | Weight | UCD:444 |
| snowflakes | `Weight_Snowflake` | Weight | UCD:442 |
| soccer balls | `Weight_SoccerBall` | Weight | UCD:443 |
| whales | `Weight_Whale` | Weight | UCD:445 |

**Total whimsical units: 26**


---


## 8. GRAPHING

### 8.1 VERDICT: `src/GraphingImpl/` is a 100% MOCK / STUB — NOT a real engine

**Decisive evidence:** every implementation file lives under a directory literally named `Mocks/`, in namespace `MockGraphingImpl`.

```
$ find src/GraphingImpl -type f
src/GraphingImpl/GraphingImpl.vcxproj
src/GraphingImpl/GraphingImpl.vcxproj.filters
src/GraphingImpl/GraphingImpl.rc
src/GraphingImpl/dllmain.cpp
src/GraphingImpl/pch.cpp
src/GraphingImpl/pch.h
src/GraphingImpl/targetver.h
src/GraphingImpl/Mocks/Bitmap.h
src/GraphingImpl/Mocks/Graph.h
src/GraphingImpl/Mocks/GraphRenderer.h
src/GraphingImpl/Mocks/GraphingOptions.h
src/GraphingImpl/Mocks/MathSolver.cpp
src/GraphingImpl/Mocks/MathSolver.h
```

| Fact | Value |
|---|---|
| Total source files | 13 |
| Files under `Mocks/` | **6 of 6** implementation files |
| Namespace | `MockGraphingImpl` |
| `.cpp` files | 2 (`dllmain.cpp` = empty stub, `Mocks/MathSolver.cpp` = factory only) |
| Lines of implementation | 816 (all in `Mocks/`) |

**`dllmain.cpp` is completely empty** — no engine is ever constructed:

```cpp
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

BOOL APIENTRY DllMain(HMODULE /* hModule */, DWORD ul_reason_for_call, LPVOID /* lpReserved */)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
```

### 8.2 Stub returns — quoted evidence

**`ParseInput` returns a `MockExpression` regardless of input** (`src/GraphingImpl/Mocks/MathSolver.h:118-126`):

```cpp
        std::unique_ptr<Graphing::IExpression> ParseInput(const std::wstring& input, int& /*errorCodeOut*/, int& /*errorTypeOut*/) override
        {
            if (input.empty())
            {
                return nullptr;
            }

            return std::make_unique<MockExpression>(MockExpression{});
        }
```

**`Serialize` returns an empty string** (`Mocks/MathSolver.h:136-139`):

```cpp
        std::wstring Serialize(const Graphing::IExpression* /*expression*/) override
        {
            return L"";
        }
```

**`Analyze` returns a default-constructed (empty) analysis struct** (`Mocks/MathSolver.h:141-144`):

```cpp
        Graphing::IGraphFunctionAnalysisData IMathSolver::Analyze(const Graphing::Analyzer::IGraphAnalyzer* /*analyzer*/)
        {
            return Graphing::IGraphFunctionAnalysisData{};
        }
```

**`HRErrorToErrorInfo` is a no-op** (`Mocks/MathSolver.h:128-130`):

```cpp
        void HRErrorToErrorInfo(HRESULT /*hr*/, int& /*errorCodeOut*/, int& /*errorTypeOut*/)
        {
        }
```

**`DrawD2D1` draws NOTHING and returns `S_OK`** (`src/GraphingImpl/Mocks/GraphRenderer.h:34-38`):

```cpp
        virtual HRESULT DrawD2D1(ID2D1Factory* /*pDirect2dFactory*/, ID2D1RenderTarget* /*pRenderTarget*/, bool& hasSomeMissingDataOut)
        {
            hasSomeMissingDataOut = false;
            return S_OK;
        }
```

**`SetGraphSize` / `SetDpi` ignore their arguments** (`Mocks/GraphRenderer.h:25-32`):

```cpp
        virtual HRESULT SetGraphSize(unsigned int /*width*/, unsigned int /*height*/)
        {
            return S_OK;
        }
        virtual HRESULT SetDpi(float /*dpiX*/, float /*dpiY*/)
        {
            return S_OK;
        }
```

**`GetClosePointData` returns all-zero coordinates** (`Mocks/GraphRenderer.h:40-60`):

```cpp
        virtual HRESULT GetClosePointData(
            double /*inScreenPointX*/,
            double /*inScreenPointY*/,
            double precision,
            int& formulaIdOut,
            float& xScreenPointOut,
            float& yScreenPointOut,
            double& xValueOut,
            double& yValueOut,
            double& /*rhoValueOut*/,
            double& /*thetaValueOut*/,
            double& /*tValueOut*/)
        {
            formulaIdOut = 0;
            xScreenPointOut = 0;
            yScreenPointOut = 0;
            precision = 0;
            xValueOut = 0;
            yValueOut = 0;
            return S_OK;
        }
```

**`Bitmap::GetData` returns an empty vector** (`src/GraphingImpl/Mocks/Bitmap.h:12-15`):

```cpp
        virtual std::vector<BYTE> GetData() const
        {
            return std::vector<BYTE>();
        }
```

**`Graph::GetAnalyzer` returns `nullptr`** (`Mocks/Graph.h:61-64`); `TryInitialize` pushes a single `nullptr` equation and one hard-coded variable `varName = L"m"` (`Mocks/Graph.h:19-30`, `Mocks/MathSolver.h:76-94`); `SetArgValue` is empty (`Mocks/Graph.h:47-49`).

### 8.3 `MockExpression` / `MockVariable`

```cpp
// src/GraphingImpl/Mocks/MathSolver.h:59-94
    class MockExpression : public Graphing::IExpression
    {
    public:
        MockExpression()
        {
        }

        unsigned int GetExpressionID() const override
        {
            return 0;
        }
        bool IsEmptySet() const
        {
            return false;
        }
    };

    class MockVariable : public Graphing::IVariable
    {
    public:
        MockVariable()
        {
        }

        int GetVariableID() const override
        {
            return 0;
        }
        const std::wstring& GetVariableName() override
        {
            return varName;
        }

    private:
        const std::wstring& varName = L"m";
    };
```

### 8.4 Factory

```cpp
// src/GraphingImpl/Mocks/MathSolver.cpp:10-26
namespace Graphing
{
    unique_ptr<IMathSolver> IMathSolver::CreateMathSolver()
    {
        return make_unique<MockGraphingImpl::MathSolver>();
    }
}

shared_ptr<Graphing::IGraph> MockGraphingImpl::MathSolver::CreateGrapher()
{
    return make_shared<MockGraphingImpl::Graph>();
}

shared_ptr<Graphing::IGraph> MockGraphingImpl::MathSolver::CreateGrapher(const Graphing::IExpression* /*expression*/)
{
    return make_shared<MockGraphingImpl::Graph>();
}
```

### 8.5 No `throw`, no `TODO`, no `NotImplementedException`

| Marker searched | Hits in `src/GraphingImpl/` |
|---|---|
| `throw ` | 0 |
| `TODO` | 0 |
| `FIXME` | 0 |
| `NotImplemented` | 0 |
| `assert(false` | 0 |
| `#error` | 0 |
| `HACK` | 0 |
| `XXX` | 0 |
| `unimplemented` | 0 |
| `stub` | 0 |
| `placeholder` | 0 |

**There is not a single `throw`/`TODO`/`NotImplemented` in the project** — it does not fail loudly, it silently returns empty/zeros while reporting `S_OK`.

### 8.6 Interfaces in `src/GraphingInterfaces/`

| File | Lines | Size | Interfaces declared |
|---|---|---|---|
| `Common.h` | 108 | 2,750 | `IExpression`, `IVariable`, `IExpressible` |
| `GraphingEnums.h` | 575 | 17,282 | _(types only)_ |
| `IBitmap.h` | 14 | 240 | `IBitmap` |
| `IEquation.h` | 22 | 549 | `IEquation` |
| `IEquationOptions.h` | 39 | 1,360 | `IEquationOptions` |
| `IGraph.h` | 35 | 1,032 | `IGraph` |
| `IGraphAnalyzer.h` | 24 | 811 | `IGraphAnalyzer` |
| `IGraphRenderer.h` | 49 | 1,704 | `ID2D1Factory`, `ID2D1RenderTarget`, `IBitmap`, `IGraphRenderer` |
| `IGraphingOptions.h` | 127 | 5,057 | `IGraphingOptions` |
| `IMathSolver.h` | 78 | 2,690 | `IGraphFunctionAnalysisData`, `IParsingOptions`, `IEvalOptions`, `IFormatOptions`, `IMathSolver` |

### 8.7 Interface list (flattened)

| # | Interface | Declared in | file:line |
|---|---|---|---|
| 1 | `IExpression` | `src/GraphingInterfaces/Common.h` | Common.h:37 |
| 2 | `IVariable` | `src/GraphingInterfaces/Common.h` | Common.h:45 |
| 3 | `IExpressible` | `src/GraphingInterfaces/Common.h` | Common.h:53 |
| 4 | `IBitmap` | `src/GraphingInterfaces/IBitmap.h` | IBitmap.h:10 |
| 5 | `IEquation` | `src/GraphingInterfaces/IEquation.h` | IEquation.h:11 |
| 6 | `IEquationOptions` | `src/GraphingInterfaces/IEquationOptions.h` | IEquationOptions.h:11 |
| 7 | `IGraph` | `src/GraphingInterfaces/IGraph.h` | IGraph.h:15 |
| 8 | `IGraphAnalyzer` | `src/GraphingInterfaces/IGraphAnalyzer.h` | IGraphAnalyzer.h:16 |
| 9 | `ID2D1Factory` | `src/GraphingInterfaces/IGraphRenderer.h` | IGraphRenderer.h:8 |
| 10 | `ID2D1RenderTarget` | `src/GraphingInterfaces/IGraphRenderer.h` | IGraphRenderer.h:9 |
| 11 | `IBitmap` | `src/GraphingInterfaces/IGraphRenderer.h` | IGraphRenderer.h:13 |
| 12 | `IGraphRenderer` | `src/GraphingInterfaces/IGraphRenderer.h` | IGraphRenderer.h:18 |
| 13 | `IGraphingOptions` | `src/GraphingInterfaces/IGraphingOptions.h` | IGraphingOptions.h:13 |
| 14 | `IGraphFunctionAnalysisData` | `src/GraphingInterfaces/IMathSolver.h` | IMathSolver.h:13 |
| 15 | `IParsingOptions` | `src/GraphingInterfaces/IMathSolver.h` | IMathSolver.h:32 |
| 16 | `IEvalOptions` | `src/GraphingInterfaces/IMathSolver.h` | IMathSolver.h:40 |
| 17 | `IFormatOptions` | `src/GraphingInterfaces/IMathSolver.h` | IMathSolver.h:48 |
| 18 | `IMathSolver` | `src/GraphingInterfaces/IMathSolver.h` | IMathSolver.h:57 |

**Total interfaces: 18** — these are the *contract*; `GraphingImpl` implements them with mocks only.

## 9. CURRENCY

### 9.1 VERDICT: 100% MOCK — NO HTTP AT ALL

There is **no base URL and no endpoint constant anywhere**. Both public methods return `Task.FromResult(<hard-coded string literal>)`. There is no `HttpClient.GetAsync`, no `Uri`, no `Windows.Web.Http` request.

### 9.2 URL / endpoint constants search

| Pattern | Files in `src/Calculator.ViewModels/DataLoaders/` matching |
|---|---|
| `http/https URL` | **NONE** |
| `fwlink` | `CurrencyHttpClient.cs` (1) |
| `Uri(...)` | `CurrencyDataLoader.cs` (1) |
| `HTTP verb` | **NONE** |
| `domain TLD` | **NONE** |
| `endpoint` | **NONE** |
| `base url` | **NONE** |

### 9.3 The mock, quoted verbatim

```csharp
// src/Calculator.ViewModels/DataLoaders/CurrencyHttpClient.cs:15-21
        // Keep HttpClient field to ensure Windows.Web.Http assembly is referenced
        // (required for .NET Native AOT compilation stability).
        private static readonly Windows.Web.Http.HttpClient _sharedClient = new Windows.Web.Http.HttpClient();
        private string _responseLanguage;

        // Mock data matching the C++ open-source calculator behavior (fictional planet currencies).
        // The upstream fwlink URLs are dead, so we use hardcoded data as the C++ version does.
```

```csharp
// src/Calculator.ViewModels/DataLoaders/CurrencyHttpClient.cs:22-46
        private const string MockCurrencyStaticData = @"[
  { ""CountryCode"": ""MAR"", ""CountryName"": ""Mars"", ""CurrencyCode"": ""MAR"", ""CurrencyName"": ""The Martian Dollar"", ""CurrencySymbol"": ""¤"" },
  { ""CountryCode"": ""MON"", ""CountryName"": ""Moon"", ""CurrencyCode"": ""MON"", ""CurrencyName"": ""Moon Bucks"", ""CurrencySymbol"": ""¤"" },
  { ""CountryCode"": ""NEP"", ""CountryName"": ""Neptune"", ""CurrencyCode"": ""NEP"", ""CurrencyName"": ""Space Coins"", ""CurrencySymbol"": ""¤"" },
  { ""CountryCode"": ""SAT"", ""CountryName"": ""Saturn"", ""CurrencyCode"": ""SAT"", ""CurrencyName"": ""Rings"", ""CurrencySymbol"": ""¤"" },
  { ""CountryCode"": ""URA"", ""CountryName"": ""Uranus"", ""CurrencyCode"": ""URA"", ""CurrencyName"": ""Galaxy Credits"", ""CurrencySymbol"": ""¤"" },
  { ""CountryCode"": ""VEN"", ""CountryName"": ""Venus"", ""CurrencyCode"": ""VEN"", ""CurrencyName"": ""Venusian Seashells"", ""CurrencySymbol"": ""¤"" },
  { ""CountryCode"": ""JUP"", ""CountryName"": ""Jupiter"", ""CurrencyCode"": ""JUP"", ""CurrencyName"": ""Gas Money"", ""CurrencySymbol"": ""¤"" },
  { ""CountryCode"": ""MER"", ""CountryName"": ""Mercury"", ""CurrencyCode"": ""MER"", ""CurrencyName"": ""Sun Notes"", ""CurrencySymbol"": ""¤"" },
  { ""CountryCode"": ""TEST1"", ""CountryName"": ""Test No Fractional Digits"", ""CurrencyCode"": ""JPY"", ""CurrencyName"": ""Test No Fractional Digits"", ""CurrencySymbol"": ""¤"" },
  { ""CountryCode"": ""TEST2"", ""CountryName"": ""Test Fractional Digits"", ""CurrencyCode"": ""JOD"", ""CurrencyName"": ""Test Fractional Digits"", ""CurrencySymbol"": ""¤"" }
]";

        private const string MockCurrencyConverterData = @"[
  { ""An"": ""MAR"", ""Rt"": 1.00 },
  { ""An"": ""MON"", ""Rt"": 0.50 },
  { ""An"": ""NEP"", ""Rt"": 0.00125 },
  { ""An"": ""SAT"", ""Rt"": 0.25 },
  { ""An"": ""URA"", ""Rt"": 2.75 },
  { ""An"": ""VEN"", ""Rt"": 900.00 },
  { ""An"": ""JUP"", ""Rt"": 1.23456789123456789 },
  { ""An"": ""MER"", ""Rt"": 2.00 },
  { ""An"": ""JPY"", ""Rt"": 0.00125 },
  { ""An"": ""JOD"", ""Rt"": 0.25 }
]";
```

```csharp
// src/Calculator.ViewModels/DataLoaders/CurrencyHttpClient.cs:48-74
        public void Initialize(string sourceCurrencyCode, string responseLanguage)
        {
            _responseLanguage = responseLanguage;
        }

        public Task<string> GetCurrencyMetadataAsync()
        {
#if VIEWMODEL_FOR_UT
            if (ForceWebFailure)
            {
                return Task.FromException<string>(new Exception("Forced web failure for unit testing"));
            }
#endif
            return Task.FromResult(MockCurrencyStaticData);
        }

        public Task<string> GetCurrencyRatiosAsync()
        {
#if VIEWMODEL_FOR_UT
            if (ForceWebFailure)
            {
                return Task.FromException<string>(new Exception("Forced web failure for unit testing"));
            }
#endif
            return Task.FromResult(MockCurrencyConverterData);
        }
    }
```

### 9.4 Name + line of every 'URL-like' constant

| # | Item | Value | file:line |
|---|---|---|---|
| 1 | `MockCurrencyStaticData` (currency list) | hard-coded JSON, 10 fictional planet currencies | src/Calculator.ViewModels/DataLoaders/CurrencyHttpClient.cs:22 |
| 2 | `MockCurrencyConverterData` (rates) | hard-coded JSON, 10 fake rates | src/Calculator.ViewModels/DataLoaders/CurrencyHttpClient.cs:35 |
| 3 | `_sharedClient` | `new Windows.Web.Http.HttpClient()` — **constructed but NEVER used**; comment says it exists only so the assembly is referenced for AOT | src/Calculator.ViewModels/DataLoaders/CurrencyHttpClient.cs:17 |
| 4 | `DefaultFromToCurrency.json` | real ISO codes, locale→pair defaults (no rates) | src/Calculator.ViewModels/DataLoaders/DefaultFromToCurrency.json:1 |

**There is no real base URL constant. The `HttpClient` field is dead code.**

### 9.5 Mock currency table (as shipped)

| # | CountryCode | CountryName | CurrencyCode | CurrencyName | Symbol | file:line |
|---|---|---|---|---|---|---|
| 0 | `CountryCode` | MAR | `Mars` | MAR | `The Martian Dollar` | CurrencyHttpClient.cs:23 |
| 1 | `CountryCode` | MON | `Moon` | MON | `Moon Bucks` | CurrencyHttpClient.cs:24 |
| 2 | `CountryCode` | NEP | `Neptune` | NEP | `Space Coins` | CurrencyHttpClient.cs:25 |
| 3 | `CountryCode` | SAT | `Saturn` | SAT | `Rings` | CurrencyHttpClient.cs:26 |
| 4 | `CountryCode` | URA | `Uranus` | URA | `Galaxy Credits` | CurrencyHttpClient.cs:27 |
| 5 | `CountryCode` | VEN | `Venus` | VEN | `Venusian Seashells` | CurrencyHttpClient.cs:28 |
| 6 | `CountryCode` | JUP | `Jupiter` | JUP | `Gas Money` | CurrencyHttpClient.cs:29 |
| 7 | `CountryCode` | MER | `Mercury` | MER | `Sun Notes` | CurrencyHttpClient.cs:30 |
| 8 | `CountryCode` | TEST1 | `Test No Fractional Digits` | JPY | `Test No Fractional Digits` | CurrencyHttpClient.cs:31 |
| 9 | `CountryCode` | TEST2 | `Test Fractional Digits` | JOD | `Test Fractional Digits` | CurrencyHttpClient.cs:32 |

| # | An | Rt (rate) | file:line |
|---|---|---|---|
| 0 | `An` | `MAR` | CurrencyHttpClient.cs:36 |
| 1 | `An` | `MON` | CurrencyHttpClient.cs:37 |
| 2 | `An` | `NEP` | CurrencyHttpClient.cs:38 |
| 3 | `An` | `SAT` | CurrencyHttpClient.cs:39 |
| 4 | `An` | `URA` | CurrencyHttpClient.cs:40 |
| 5 | `An` | `VEN` | CurrencyHttpClient.cs:41 |
| 6 | `An` | `JUP` | CurrencyHttpClient.cs:42 |
| 7 | `An` | `MER` | CurrencyHttpClient.cs:43 |
| 8 | `An` | `JPY` | CurrencyHttpClient.cs:44 |
| 9 | `An` | `JOD` | CurrencyHttpClient.cs:45 |

### 9.6 `CurrencyDataLoader.cs` — what consumes the mock

| Item | file:line |
|---|---|
| `private readonly CurrencyHttpClient _client;` | CurrencyDataLoader.cs:63 |
| `_client = new CurrencyHttpClient();` | CurrencyDataLoader.cs:117 |
| `string staticDataResponse = await _client.GetCurrencyMetadataAsync();` | CurrencyDataLoader.cs:413 |
| `string allRatiosResponse = await _client.GetCurrencyRatiosAsync();` | CurrencyDataLoader.cs:414 |

## 10. TEST PROJECTS

### 10.1 Existence

| Requested path | Exists? | Actual |
|---|---|---|
| `src/CalculatorUnitTests/` | **YES** | C++ / Windows Test Automation (WTA), `CalculatorUnitTests.vcxproj` |
| `src/CalculatorUITests/` | **YES** | C# / MSTest, `CalculatorUITests.csproj` (Appium/WinAppDriver) |
| `src/Calculator.Tests/` | **YES** | C# / MSTest, `Calculator.Tests.csproj` |

**Is there a C++ test project? YES — exactly one:** `src/CalculatorUnitTests/CalculatorUnitTests.vcxproj` (10 `.cpp` + 6 `.h` files, WTA `TEST_CLASS`/`TEST_METHOD` macros, no gtest).

A fourth, non-test-support project also exists: `src/CalculatorUITestFramework/` (22 `.cs` page-object/helper classes, 0 test methods).

### 10.2 Per-project totals

| Project | Language | `.cpp` | `.h` | `.cs` | Test classes | Test methods |
|---|---|---|---|---|---|---|
| `CalculatorUnitTests` | C++ (WTA) | 10 | 6 | 0 | 5 | **86** |
| `Calculator.Tests` | C# (MSTest) | 0 | 0 | 17 | 24 | **288** |
| `CalculatorUITests` | C# (MSTest) | 0 | 0 | 6 | 6 | **160** |
| `CalculatorUITestFramework` | C# (Appium helpers) | 0 | 0 | 22 | 22 | **0** |
| **TOTAL** | | | | | **57** | **534** |

### 10.3 Test classes + method counts

| # | Project | Class | #Test methods | #Init/Cleanup | File |
|---|---|---|---|---|---|
| 0 | `CalculatorUnitTests` | `CalcEngineTests` | 4 | 2 | `CalculatorUnitTests/CalcEngineTests.cpp` |
| 1 | `CalculatorUnitTests` | `CalcInputTest` | 37 | 2 | `CalculatorUnitTests/CalcInputTest.cpp` |
| 2 | `CalculatorUnitTests` | `CalculatorManagerTest` | 21 | 2 | `CalculatorUnitTests/CalculatorManagerTest.cpp` |
| 3 | `CalculatorUnitTests` | `RationalTest` | 8 | 0 | `CalculatorUnitTests/RationalTest.cpp` |
| 4 | `CalculatorUnitTests` | `UnitConverterTest` | 16 | 1 | `CalculatorUnitTests/UnitConverterTest.cpp` |
| 5 | `Calculator.Tests` | `CopyPasteManagerTests` | 23 | 0 | `Calculator.Tests/CopyPasteManagerTests.cs` |
| 6 | `Calculator.Tests` | `CurrencyConverterLoadTests` | 17 | 0 | `Calculator.Tests/CurrencyConverterTests.cs` |
| 7 | `Calculator.Tests` | `DateCalculatorUnitTests` | 8 | 0 | `Calculator.Tests/DateCalculatorTests.cs` |
| 8 | `Calculator.Tests` | `DateCalculatorViewModelTests` | 20 | 0 | `Calculator.Tests/DateCalculatorTests.cs` |
| 9 | `Calculator.Tests` | `GraphingViewModelTests` | 2 | 0 | `Calculator.Tests/GraphingViewModelTests.cs` |
| 10 | `Calculator.Tests` | `HistoryTests` | 36 | 0 | `Calculator.Tests/HistoryTests.cs` |
| 11 | `Calculator.Tests` | `KeystrokeLatencyTests` | 1 | 0 | `Calculator.Tests/KeystrokeLatencyTests.cs` |
| 12 | `Calculator.Tests` | `LocalizationSettingsTests` | 8 | 0 | `Calculator.Tests/LocalizationSettingsTests.cs` |
| 13 | `Calculator.Tests` | `MultiWindowUnitTests` | 16 | 0 | `Calculator.Tests/MultiWindowTests.cs` |
| 14 | `Calculator.Tests` | `NarratorAnnouncementTests` | 19 | 0 | `Calculator.Tests/NarratorAnnouncementTests.cs` |
| 15 | `Calculator.Tests` | `NavCategoryGroupTests` | 1 | 0 | `Calculator.Tests/NavCategoryTests.cs` |
| 16 | `Calculator.Tests` | `NavCategoryStatesTests` | 14 | 0 | `Calculator.Tests/NavCategoryTests.cs` |
| 17 | `Calculator.Tests` | `NavCategoryTests` | 3 | 0 | `Calculator.Tests/NavCategoryTests.cs` |
| 18 | `Calculator.Tests` | `SnapshotJsonTests` | 13 | 0 | `Calculator.Tests/SnapshotJsonTests.cs` |
| 19 | `Calculator.Tests` | `SnapshotRoundTripTests` | 17 | 0 | `Calculator.Tests/SnapshotRoundTripTests.cs` |
| 20 | `Calculator.Tests` | `StandardCalculatorViewModelTests` | 50 | 1 | `Calculator.Tests/StandardCalculatorViewModelTests.cs` |
| 21 | `Calculator.Tests` | `CategoryViewModelTests` | 3 | 0 | `Calculator.Tests/UnitConverterViewModelTests.cs` |
| 22 | `Calculator.Tests` | `SupplementaryResultsViewModelTests` | 3 | 0 | `Calculator.Tests/UnitConverterViewModelTests.cs` |
| 23 | `Calculator.Tests` | `UnitConverterDataLoaderTests` | 1 | 0 | `Calculator.Tests/UnitConverterViewModelTests.cs` |
| 24 | `Calculator.Tests` | `UnitConverterViewModelTests` | 22 | 0 | `Calculator.Tests/UnitConverterViewModelTests.cs` |
| 25 | `Calculator.Tests` | `UnitViewModelTests` | 2 | 0 | `Calculator.Tests/UnitConverterViewModelTests.cs` |
| 26 | `Calculator.Tests` | `App` | 0 | 0 | `Calculator.Tests/UnitTestApp.xaml.cs` |
| 27 | `Calculator.Tests` | `UtilsTests` | 6 | 0 | `Calculator.Tests/UtilsTests.cs` |
| 28 | `Calculator.Tests` | `ViewModelLifetimeTests` | 3 | 0 | `Calculator.Tests/ViewModelLifetimeTests.cs` |
| 29 | `CalculatorUITests` | `CurrencyConverterFunctionalTests` | 8 | 2 | `CalculatorUITests/CurrencyConverterFunctionalTests.cs` |
| 30 | `CalculatorUITests` | `HistoryFunctionalTests` | 4 | 2 | `CalculatorUITests/HistoryFunctionalTests.cs` |
| 31 | `CalculatorUITests` | `MemoryFunctionalTests` | 5 | 2 | `CalculatorUITests/MemoryFunctionalTests.cs` |
| 32 | `CalculatorUITests` | `ProgrammerModeFunctionalTests` | 60 | 2 | `CalculatorUITests/ProgrammerModeFunctionalTests.cs` |
| 33 | `CalculatorUITests` | `ScientificModeFunctionalTests` | 45 | 2 | `CalculatorUITests/ScientificModeFunctionalTests.cs` |
| 34 | `CalculatorUITests` | `StandardModeFunctionalTests` | 38 | 2 | `CalculatorUITests/StandardModeFunctionalTests.cs` |
| 35 | `CalculatorUITestFramework` | `CalculatorApp` | 0 | 0 | `CalculatorUITestFramework/CalculatorApp.cs` |
| 36 | `CalculatorUITestFramework` | `CalculatorDriver` | 0 | 0 | `CalculatorUITestFramework/CalculatorDriver.cs` |
| 37 | `CalculatorUITestFramework` | `CalculatorResults` | 0 | 0 | `CalculatorUITestFramework/CalculatorResults.cs` |
| 38 | `CalculatorUITestFramework` | `HistoryItem` | 0 | 0 | `CalculatorUITestFramework/HistoryItem.cs` |
| 39 | `CalculatorUITestFramework` | `HistoryPanel` | 0 | 0 | `CalculatorUITestFramework/HistoryPanel.cs` |
| 40 | `CalculatorUITestFramework` | `MemoryItem` | 0 | 0 | `CalculatorUITestFramework/MemoryItem.cs` |
| 41 | `CalculatorUITestFramework` | `MemoryPanel` | 0 | 0 | `CalculatorUITestFramework/MemoryPanel.cs` |
| 42 | `CalculatorUITestFramework` | `NavigationMenu` | 0 | 0 | `CalculatorUITestFramework/NavigationMenu.cs` |
| 43 | `CalculatorUITestFramework` | `NumberPad` | 0 | 0 | `CalculatorUITestFramework/NumberPad.cs` |
| 44 | `CalculatorUITestFramework` | `ProgrammerCalculatorPage` | 0 | 0 | `CalculatorUITestFramework/ProgrammerCalculatorPage.cs` |
| 45 | `CalculatorUITestFramework` | `ProgrammerOperatorsPanel` | 0 | 0 | `CalculatorUITestFramework/ProgrammerOperatorsPanel.cs` |
| 46 | `CalculatorUITestFramework` | `ScientificCalculatorPage` | 0 | 0 | `CalculatorUITestFramework/ScientificCalculatorPage.cs` |
| 47 | `CalculatorUITestFramework` | `ScientificOperatorsPanel` | 0 | 0 | `CalculatorUITestFramework/ScientificOperatorsPanel.cs` |
| 48 | `CalculatorUITestFramework` | `StandardAoTCalculatorPage` | 0 | 0 | `CalculatorUITestFramework/StandardAoTCalculatorPage.cs` |
| 49 | `CalculatorUITestFramework` | `StandardCalculatorPage` | 0 | 0 | `CalculatorUITestFramework/StandardCalculatorPage.cs` |
| 50 | `CalculatorUITestFramework` | `StandardOperatorsPanel` | 0 | 0 | `CalculatorUITestFramework/StandardOperatorsPanel.cs` |
| 51 | `CalculatorUITestFramework` | `UnitConverterOperatorsPanel` | 0 | 0 | `CalculatorUITestFramework/UnitConverterOperatorsPanel.cs` |
| 52 | `CalculatorUITestFramework` | `UnitConverterPage` | 0 | 0 | `CalculatorUITestFramework/UnitConverterPage.cs` |
| 53 | `CalculatorUITestFramework` | `UnitConverterResults` | 0 | 0 | `CalculatorUITestFramework/UnitConverterResults.cs` |
| 54 | `CalculatorUITestFramework` | `WinAppDriverLocalServer` | 0 | 0 | `CalculatorUITestFramework/WinAppDriverLocalServer.cs` |
| 55 | `CalculatorUITestFramework` | `WindowsDriverExtensions` | 0 | 0 | `CalculatorUITestFramework/WindowsDriverExtensions.cs` |
| 56 | `CalculatorUITestFramework` | `WindowsElementExtensions` | 0 | 0 | `CalculatorUITestFramework/WindowsElementExtensions.cs` |

### 10.4 C++ test class breakdown (`TEST_CLASS` / `TEST_METHOD`)

| Class | TEST_METHOD count | Source |
|---|---|---|
| `CalcEngineTests` | 4 | `CalculatorUnitTests/CalcEngineTests.cpp` |
| `CalcInputTest` | 37 | `CalculatorUnitTests/CalcInputTest.cpp` |
| `CalculatorManagerTest` | 21 | `CalculatorUnitTests/CalculatorManagerTest.cpp` |
| `RationalTest` | 8 | `CalculatorUnitTests/RationalTest.cpp` |
| `UnitConverterTest` | 16 | `CalculatorUnitTests/UnitConverterTest.cpp` |

### 10.5 What the C++ tests cover

| File | Lines | Covers |
|---|---|---|
| `CalcEngineTests.cpp` | 243 | `CalcEngine` primitives |
| `CalcInputTest.cpp` | 371 | `CalcInput` parsing / sign toggle / backspace |
| `CalculatorManagerTest.cpp` | 1212 | `CalculatorManager` history / memory / display |
| `RationalTest.cpp` | 221 | `Rational` big-number arithmetic |
| `UnitConverterTest.cpp` | 602 | `UnitConverter` conversion factors |


---


## APPENDIX A — Buttons outside the 9 target files (completeness check)

The 9 files in §2 contain **200** button-like controls. For completeness, the remaining views in `src/Calculator/Views/` also define buttons; they are listed here so nothing is missed.

| File | Button-like elements |
|---|---|
| `Calculator.xaml` | 15 |
| `CalculatorProgrammerDisplayPanel.xaml` | 6 |
| `CalculatorScientificAngleButtons.xaml` | 5 |
| `DateCalculator.xaml` | 11 |
| `EquationInputArea.xaml` | 22 |
| `EquationStylePanelControl.xaml` | 3 |
| `GraphingCalculator.xaml` | 13 |
| `GraphingNumPad.xaml` | 81 |
| `GraphingSettings.xaml` | 8 |
| `KeyGraphFeaturesPanel.xaml` | 1 |
| `MainPage.xaml` | 6 |
| `Settings.xaml` | 4 |
| `TitleBar.xaml` | 2 |
| `UnitConverter.xaml` | 7 |
| **TOTAL (outside the 9 §2 files)** | **184** |

| # | File | Control | x:Name | file:line |
|---|---|---|---|---|
| 0 | `Calculator.xaml` | `<Button` | `ScrollLeft` | Calculator.xaml:77 |
| 1 | `Calculator.xaml` | `<Button` | `ScrollRight` | Calculator.xaml:86 |
| 2 | `Calculator.xaml` | `<Button` | `ScrollLeft` | Calculator.xaml:183 |
| 3 | `Calculator.xaml` | `<Button` | `ScrollRight` | Calculator.xaml:195 |
| 4 | `Calculator.xaml` | `<MenuFlyoutItem` | `CopyMenuItem` | Calculator.xaml:402 |
| 5 | `Calculator.xaml` | `<MenuFlyoutItem` | `PasteMenuItem` | Calculator.xaml:406 |
| 6 | `Calculator.xaml` | `<Button` | `HistoryButton` | Calculator.xaml:670 |
| 7 | `Calculator.xaml` | `CalculatorButton` | `ClearMemoryButton` | Calculator.xaml:739 |
| 8 | `Calculator.xaml` | `CalculatorButton` | `MemRecall` | Calculator.xaml:749 |
| 9 | `Calculator.xaml` | `CalculatorButton` | `MemPlus` | Calculator.xaml:761 |
| 10 | `Calculator.xaml` | `CalculatorButton` | `MemMinus` | Calculator.xaml:773 |
| 11 | `Calculator.xaml` | `CalculatorButton` | `MemButton` | Calculator.xaml:785 |
| 12 | `Calculator.xaml` | `<Button` | `MemoryButton` | Calculator.xaml:795 |
| 13 | `Calculator.xaml` | `<Button` | `PreviousButton` | Calculator.xaml:1123 |
| 14 | `Calculator.xaml` | `<Button` | `NextButton` | Calculator.xaml:1136 |
| 15 | `CalculatorProgrammerDisplayPanel.xaml` | `<RadioButton` | `FullKeypad` | CalculatorProgrammerDisplayPanel.xaml:63 |
| 16 | `CalculatorProgrammerDisplayPanel.xaml` | `<RadioButton` | `BitFlip` | CalculatorProgrammerDisplayPanel.xaml:70 |
| 17 | `CalculatorProgrammerDisplayPanel.xaml` | `<Button` | `QwordButton` | CalculatorProgrammerDisplayPanel.xaml:80 |
| 18 | `CalculatorProgrammerDisplayPanel.xaml` | `<Button` | `DwordButton` | CalculatorProgrammerDisplayPanel.xaml:90 |
| 19 | `CalculatorProgrammerDisplayPanel.xaml` | `<Button` | `WordButton` | CalculatorProgrammerDisplayPanel.xaml:101 |
| 20 | `CalculatorProgrammerDisplayPanel.xaml` | `<Button` | `ByteButton` | CalculatorProgrammerDisplayPanel.xaml:112 |
| 21 | `CalculatorScientificAngleButtons.xaml` | `<Button` | `DegreeButton` | CalculatorScientificAngleButtons.xaml:75 |
| 22 | `CalculatorScientificAngleButtons.xaml` | `<Button` | `RadianButton` | CalculatorScientificAngleButtons.xaml:83 |
| 23 | `CalculatorScientificAngleButtons.xaml` | `<Button` | `GradsButton` | CalculatorScientificAngleButtons.xaml:92 |
| 24 | `CalculatorScientificAngleButtons.xaml` | `<ToggleButton` | `FtoeButton` | CalculatorScientificAngleButtons.xaml:101 |
| 25 | `CalculatorScientificAngleButtons.xaml` | `CommandParameter>` | `` | CalculatorScientificAngleButtons.xaml:113 |
| 26 | `DateCalculator.xaml` | `<Button` | `HeaderButton` | DateCalculator.xaml:807 |
| 27 | `DateCalculator.xaml` | `<Button` | `PreviousButton` | DateCalculator.xaml:816 |
| 28 | `DateCalculator.xaml` | `<Button` | `NextButton` | DateCalculator.xaml:828 |
| 29 | `DateCalculator.xaml` | `<MenuFlyoutItem` | `CopyMenuItem` | DateCalculator.xaml:951 |
| 30 | `DateCalculator.xaml` | `<ComboBox` | `DateCalculationOption` | DateCalculator.xaml:1004 |
| 31 | `DateCalculator.xaml` | `Resources>` | `` | DateCalculator.xaml:1011 |
| 32 | `DateCalculator.xaml` | `<RadioButton` | `AddOption` | DateCalculator.xaml:1137 |
| 33 | `DateCalculator.xaml` | `<RadioButton` | `SubtractOption` | DateCalculator.xaml:1144 |
| 34 | `DateCalculator.xaml` | `<ComboBox` | `YearsValue` | DateCalculator.xaml:1175 |
| 35 | `DateCalculator.xaml` | `<ComboBox` | `MonthsValue` | DateCalculator.xaml:1189 |
| 36 | `DateCalculator.xaml` | `<ComboBox` | `DaysValue` | DateCalculator.xaml:1205 |
| 37 | `EquationInputArea.xaml` | `<ToggleButton` | `` | EquationInputArea.xaml:237 |
| 38 | `EquationInputArea.xaml` | `Resources>` | `` | EquationInputArea.xaml:247 |
| 39 | `EquationInputArea.xaml` | `<ToggleButton` | `EquationButton` | EquationInputArea.xaml:490 |
| 40 | `EquationInputArea.xaml` | `Content>` | `` | EquationInputArea.xaml:495 |
| 41 | `EquationInputArea.xaml` | `Resources>` | `` | EquationInputArea.xaml:508 |
| 42 | `EquationInputArea.xaml` | `Style>` | `` | EquationInputArea.xaml:565 |
| 43 | `EquationInputArea.xaml` | `<MenuFlyoutItem` | `CutMenuItem` | EquationInputArea.xaml:673 |
| 44 | `EquationInputArea.xaml` | `<MenuFlyoutItem` | `CopyMenuItem` | EquationInputArea.xaml:676 |
| 45 | `EquationInputArea.xaml` | `<MenuFlyoutItem` | `PasteMenuItem` | EquationInputArea.xaml:679 |
| 46 | `EquationInputArea.xaml` | `<MenuFlyoutItem` | `UndoMenuItem` | EquationInputArea.xaml:682 |
| 47 | `EquationInputArea.xaml` | `<MenuFlyoutItem` | `SelectAllMenuItem` | EquationInputArea.xaml:685 |
| 48 | `EquationInputArea.xaml` | `<MenuFlyoutItem` | `FunctionAnalysisMenuItem` | EquationInputArea.xaml:687 |
| 49 | `EquationInputArea.xaml` | `Icon>` | `` | EquationInputArea.xaml:688 |
| 50 | `EquationInputArea.xaml` | `<MenuFlyoutItem` | `ChangeFunctionStyleMenuItem` | EquationInputArea.xaml:692 |
| 51 | `EquationInputArea.xaml` | `Icon>` | `` | EquationInputArea.xaml:693 |
| 52 | `EquationInputArea.xaml` | `<MenuFlyoutItem` | `RemoveFunctionMenuItem` | EquationInputArea.xaml:697 |
| 53 | `EquationInputArea.xaml` | `Icon>` | `` | EquationInputArea.xaml:698 |
| 54 | `EquationInputArea.xaml` | `<Button` | `FunctionButton` | EquationInputArea.xaml:706 |
| 55 | `EquationInputArea.xaml` | `<ToggleButton` | `ColorChooserButton` | EquationInputArea.xaml:720 |
| 56 | `EquationInputArea.xaml` | `Resources>` | `` | EquationInputArea.xaml:734 |
| 57 | `EquationInputArea.xaml` | `<Button` | `RemoveButton` | EquationInputArea.xaml:774 |
| 58 | `EquationInputArea.xaml` | `<Button` | `DeleteButton` | EquationInputArea.xaml:788 |
| 59 | `EquationStylePanelControl.xaml` | `<ComboBox` | `StyleChooserBox` | EquationStylePanelControl.xaml:100 |
| 60 | `EquationStylePanelControl.xaml` | `Header>` | `` | EquationStylePanelControl.xaml:109 |
| 61 | `EquationStylePanelControl.xaml` | `ItemTemplate>` | `` | EquationStylePanelControl.xaml:114 |
| 62 | `GraphingCalculator.xaml` | `<ToggleSwitch` | `SwitchModeToggleButton` | GraphingCalculator.xaml:557 |
| 63 | `GraphingCalculator.xaml` | `<MenuFlyoutItem` | `` | GraphingCalculator.xaml:600 |
| 64 | `GraphingCalculator.xaml` | `<ToggleButton` | `ActiveTracing` | GraphingCalculator.xaml:612 |
| 65 | `GraphingCalculator.xaml` | `Resources>` | `` | GraphingCalculator.xaml:621 |
| 66 | `GraphingCalculator.xaml` | `<Button` | `` | GraphingCalculator.xaml:644 |
| 67 | `GraphingCalculator.xaml` | `<Button` | `GraphSettingsButton` | GraphingCalculator.xaml:656 |
| 68 | `GraphingCalculator.xaml` | `<RepeatButton` | `ZoomInButton` | GraphingCalculator.xaml:707 |
| 69 | `GraphingCalculator.xaml` | `KeyboardAccelerators>` | `` | GraphingCalculator.xaml:721 |
| 70 | `GraphingCalculator.xaml` | `<RepeatButton` | `ZoomOutButton` | GraphingCalculator.xaml:726 |
| 71 | `GraphingCalculator.xaml` | `KeyboardAccelerators>` | `` | GraphingCalculator.xaml:740 |
| 72 | `GraphingCalculator.xaml` | `<ToggleButton` | `` | GraphingCalculator.xaml:745 |
| 73 | `GraphingCalculator.xaml` | `Content>` | `` | GraphingCalculator.xaml:755 |
| 74 | `GraphingCalculator.xaml` | `KeyboardAccelerators>` | `` | GraphingCalculator.xaml:766 |
| 75 | `GraphingNumPad.xaml` | `OperatorPanelButton` | `TrigButton` | GraphingNumPad.xaml:365 |
| 76 | `GraphingNumPad.xaml` | `FlyoutMenu>` | `` | GraphingNumPad.xaml:373 |
| 77 | `GraphingNumPad.xaml` | `<ToggleButton` | `TrigShiftButton` | GraphingNumPad.xaml:401 |
| 78 | `GraphingNumPad.xaml` | `<ToggleButton` | `HypButton` | GraphingNumPad.xaml:412 |
| 79 | `GraphingNumPad.xaml` | `CalculatorButton` | `SinButton` | GraphingNumPad.xaml:439 |
| 80 | `GraphingNumPad.xaml` | `CalculatorButton` | `CosButton` | GraphingNumPad.xaml:449 |
| 81 | `GraphingNumPad.xaml` | `CalculatorButton` | `TanButton` | GraphingNumPad.xaml:461 |
| 82 | `GraphingNumPad.xaml` | `CalculatorButton` | `SecButton` | GraphingNumPad.xaml:473 |
| 83 | `GraphingNumPad.xaml` | `CalculatorButton` | `CscButton` | GraphingNumPad.xaml:485 |
| 84 | `GraphingNumPad.xaml` | `CalculatorButton` | `CotButton` | GraphingNumPad.xaml:498 |
| 85 | `GraphingNumPad.xaml` | `CalculatorButton` | `InvsinButton` | GraphingNumPad.xaml:527 |
| 86 | `GraphingNumPad.xaml` | `CalculatorButton` | `InvcosButton` | GraphingNumPad.xaml:538 |
| 87 | `GraphingNumPad.xaml` | `CalculatorButton` | `InvtanButton` | GraphingNumPad.xaml:550 |
| 88 | `GraphingNumPad.xaml` | `CalculatorButton` | `InvsecButton` | GraphingNumPad.xaml:562 |
| 89 | `GraphingNumPad.xaml` | `CalculatorButton` | `InvcscButton` | GraphingNumPad.xaml:574 |
| 90 | `GraphingNumPad.xaml` | `CalculatorButton` | `InvcotButton` | GraphingNumPad.xaml:587 |
| 91 | `GraphingNumPad.xaml` | `CalculatorButton` | `SinhButton` | GraphingNumPad.xaml:616 |
| 92 | `GraphingNumPad.xaml` | `CalculatorButton` | `CoshButton` | GraphingNumPad.xaml:627 |
| 93 | `GraphingNumPad.xaml` | `CalculatorButton` | `TanhButton` | GraphingNumPad.xaml:639 |
| 94 | `GraphingNumPad.xaml` | `CalculatorButton` | `SechButton` | GraphingNumPad.xaml:651 |
| 95 | `GraphingNumPad.xaml` | `CalculatorButton` | `CschButton` | GraphingNumPad.xaml:663 |
| 96 | `GraphingNumPad.xaml` | `CalculatorButton` | `CothButton` | GraphingNumPad.xaml:676 |
| 97 | `GraphingNumPad.xaml` | `CalculatorButton` | `InvsinhButton` | GraphingNumPad.xaml:704 |
| 98 | `GraphingNumPad.xaml` | `CalculatorButton` | `InvcoshButton` | GraphingNumPad.xaml:715 |
| 99 | `GraphingNumPad.xaml` | `CalculatorButton` | `InvtanhButton` | GraphingNumPad.xaml:727 |
| 100 | `GraphingNumPad.xaml` | `CalculatorButton` | `InvsechButton` | GraphingNumPad.xaml:739 |
| 101 | `GraphingNumPad.xaml` | `CalculatorButton` | `InvcschButton` | GraphingNumPad.xaml:751 |
| 102 | `GraphingNumPad.xaml` | `CalculatorButton` | `InvcothButton` | GraphingNumPad.xaml:764 |
| 103 | `GraphingNumPad.xaml` | `OperatorPanelButton` | `InequalityButton` | GraphingNumPad.xaml:782 |
| 104 | `GraphingNumPad.xaml` | `FlyoutMenu>` | `` | GraphingNumPad.xaml:789 |
| 105 | `GraphingNumPad.xaml` | `CalculatorButton` | `LessThanFlyoutButton` | GraphingNumPad.xaml:813 |
| 106 | `GraphingNumPad.xaml` | `CalculatorButton` | `LessThanOrEqualFlyoutButton` | GraphingNumPad.xaml:823 |
| 107 | `GraphingNumPad.xaml` | `CalculatorButton` | `EqualsFlyoutButton` | GraphingNumPad.xaml:834 |
| 108 | `GraphingNumPad.xaml` | `CalculatorButton` | `GreaterThanOrEqualFlyoutButton` | GraphingNumPad.xaml:845 |
| 109 | `GraphingNumPad.xaml` | `CalculatorButton` | `GreaterThanFlyoutButton` | GraphingNumPad.xaml:856 |
| 110 | `GraphingNumPad.xaml` | `OperatorPanelButton` | `FuncButton` | GraphingNumPad.xaml:872 |
| 111 | `GraphingNumPad.xaml` | `FlyoutMenu>` | `` | GraphingNumPad.xaml:880 |
| 112 | `GraphingNumPad.xaml` | `CalculatorButton` | `AbsFlyoutButton` | GraphingNumPad.xaml:901 |
| 113 | `GraphingNumPad.xaml` | `CalculatorButton` | `FloorButton` | GraphingNumPad.xaml:912 |
| 114 | `GraphingNumPad.xaml` | `CalculatorButton` | `CeilButton` | GraphingNumPad.xaml:924 |
| 115 | `GraphingNumPad.xaml` | `<ToggleButton` | `ShiftButton` | GraphingNumPad.xaml:941 |
| 116 | `GraphingNumPad.xaml` | `CalculatorButton` | `PiButton` | GraphingNumPad.xaml:955 |
| 117 | `GraphingNumPad.xaml` | `CalculatorButton` | `EulerButton` | GraphingNumPad.xaml:968 |
| 118 | `GraphingNumPad.xaml` | `CalculatorButton` | `ClearButton` | GraphingNumPad.xaml:992 |
| 119 | `GraphingNumPad.xaml` | `CalculatorButton` | `BackSpaceButton` | GraphingNumPad.xaml:1004 |
| 120 | `GraphingNumPad.xaml` | `CalculatorButton` | `XPower2Button` | GraphingNumPad.xaml:1033 |
| 121 | `GraphingNumPad.xaml` | `CalculatorButton` | `SquareRootButton` | GraphingNumPad.xaml:1043 |
| 122 | `GraphingNumPad.xaml` | `CalculatorButton` | `PowerButton` | GraphingNumPad.xaml:1054 |
| 123 | `GraphingNumPad.xaml` | `CalculatorButton` | `PowerOf10Button` | GraphingNumPad.xaml:1067 |
| 124 | `GraphingNumPad.xaml` | `CalculatorButton` | `LogBase10Button` | GraphingNumPad.xaml:1078 |
| 125 | `GraphingNumPad.xaml` | `CalculatorButton` | `LogBaseEButton` | GraphingNumPad.xaml:1089 |
| 126 | `GraphingNumPad.xaml` | `CalculatorButton` | `XPower3Button` | GraphingNumPad.xaml:1112 |
| 127 | `GraphingNumPad.xaml` | `CalculatorButton` | `CubeRootButton` | GraphingNumPad.xaml:1122 |
| 128 | `GraphingNumPad.xaml` | `CalculatorButton` | `YSquareRootButton` | GraphingNumPad.xaml:1133 |
| 129 | `GraphingNumPad.xaml` | `CalculatorButton` | `TwoPowerXButton` | GraphingNumPad.xaml:1145 |
| 130 | `GraphingNumPad.xaml` | `CalculatorButton` | `LogBaseY` | GraphingNumPad.xaml:1156 |
| 131 | `GraphingNumPad.xaml` | `CalculatorButton` | `PowerOfEButton` | GraphingNumPad.xaml:1167 |
| 132 | `GraphingNumPad.xaml` | `CalculatorButton` | `InvertButton` | GraphingNumPad.xaml:1191 |
| 133 | `GraphingNumPad.xaml` | `CalculatorButton` | `AbsButton` | GraphingNumPad.xaml:1201 |
| 134 | `GraphingNumPad.xaml` | `CalculatorButton` | `XButton` | GraphingNumPad.xaml:1212 |
| 135 | `GraphingNumPad.xaml` | `CalculatorButton` | `YButton` | GraphingNumPad.xaml:1224 |
| 136 | `GraphingNumPad.xaml` | `CalculatorButton` | `OpenParenthesisButton` | GraphingNumPad.xaml:1237 |
| 137 | `GraphingNumPad.xaml` | `CalculatorButton` | `CloseParenthesisButton` | GraphingNumPad.xaml:1250 |
| 138 | `GraphingNumPad.xaml` | `CalculatorButton` | `EqualButton` | GraphingNumPad.xaml:1263 |
| 139 | `GraphingNumPad.xaml` | `CalculatorButton` | `DivideButton` | GraphingNumPad.xaml:1289 |
| 140 | `GraphingNumPad.xaml` | `CalculatorButton` | `MultiplyButton` | GraphingNumPad.xaml:1301 |
| 141 | `GraphingNumPad.xaml` | `CalculatorButton` | `MinusButton` | GraphingNumPad.xaml:1313 |
| 142 | `GraphingNumPad.xaml` | `CalculatorButton` | `PlusButton` | GraphingNumPad.xaml:1325 |
| 143 | `GraphingNumPad.xaml` | `CalculatorButton` | `SubmitButton` | GraphingNumPad.xaml:1337 |
| 144 | `GraphingNumPad.xaml` | `CalculatorButton` | `NegateButton` | GraphingNumPad.xaml:1348 |
| 145 | `GraphingNumPad.xaml` | `CalculatorButton` | `Num0Button` | GraphingNumPad.xaml:1362 |
| 146 | `GraphingNumPad.xaml` | `CalculatorButton` | `Num1Button` | GraphingNumPad.xaml:1372 |
| 147 | `GraphingNumPad.xaml` | `CalculatorButton` | `Num2Button` | GraphingNumPad.xaml:1382 |
| 148 | `GraphingNumPad.xaml` | `CalculatorButton` | `Num3Button` | GraphingNumPad.xaml:1392 |
| 149 | `GraphingNumPad.xaml` | `CalculatorButton` | `Num4Button` | GraphingNumPad.xaml:1402 |
| 150 | `GraphingNumPad.xaml` | `CalculatorButton` | `Num5Button` | GraphingNumPad.xaml:1412 |
| 151 | `GraphingNumPad.xaml` | `CalculatorButton` | `Num6Button` | GraphingNumPad.xaml:1422 |
| 152 | `GraphingNumPad.xaml` | `CalculatorButton` | `Num7Button` | GraphingNumPad.xaml:1432 |
| 153 | `GraphingNumPad.xaml` | `CalculatorButton` | `Num8Button` | GraphingNumPad.xaml:1442 |
| 154 | `GraphingNumPad.xaml` | `CalculatorButton` | `Num9Button` | GraphingNumPad.xaml:1452 |
| 155 | `GraphingNumPad.xaml` | `CalculatorButton` | `DecimalSeparatorButton` | GraphingNumPad.xaml:1462 |
| 156 | `GraphingSettings.xaml` | `<RadioButton` | `Radians` | GraphingSettings.xaml:199 |
| 157 | `GraphingSettings.xaml` | `<RadioButton` | `Degrees` | GraphingSettings.xaml:204 |
| 158 | `GraphingSettings.xaml` | `<RadioButton` | `Gradians` | GraphingSettings.xaml:210 |
| 159 | `GraphingSettings.xaml` | `<ComboBox` | `` | GraphingSettings.xaml:222 |
| 160 | `GraphingSettings.xaml` | `Items>` | `` | GraphingSettings.xaml:226 |
| 161 | `GraphingSettings.xaml` | `ItemTemplate>` | `` | GraphingSettings.xaml:232 |
| 162 | `GraphingSettings.xaml` | `<RadioButton` | `` | GraphingSettings.xaml:257 |
| 163 | `GraphingSettings.xaml` | `<RadioButton` | `` | GraphingSettings.xaml:260 |
| 164 | `KeyGraphFeaturesPanel.xaml` | `<Button` | `BackButton` | KeyGraphFeaturesPanel.xaml:238 |
| 165 | `MainPage.xaml` | `<Button` | `CopyButton` | MainPage.xaml:93 |
| 166 | `MainPage.xaml` | `<Button` | `PasteButton` | MainPage.xaml:96 |
| 167 | `MainPage.xaml` | `<Button` | `CopyButtonAlternate` | MainPage.xaml:99 |
| 168 | `MainPage.xaml` | `<Button` | `PasteButtonAlternate` | MainPage.xaml:102 |
| 169 | `MainPage.xaml` | `<Button` | `NormalAlwaysOnTopButton` | MainPage.xaml:165 |
| 170 | `MainPage.xaml` | `KeyboardAccelerators>` | `` | MainPage.xaml:175 |
| 171 | `Settings.xaml` | `<Button` | `BackButton` | Settings.xaml:52 |
| 172 | `Settings.xaml` | `<RadioButton` | `LightThemeRadioButton` | Settings.xaml:98 |
| 173 | `Settings.xaml` | `<RadioButton` | `DarkThemeRadioButton` | Settings.xaml:101 |
| 174 | `Settings.xaml` | `<RadioButton` | `SystemThemeRadioButton` | Settings.xaml:104 |
| 175 | `TitleBar.xaml` | `<Button` | `ExitAlwaysOnTopButton` | TitleBar.xaml:76 |
| 176 | `TitleBar.xaml` | `KeyboardAccelerators>` | `` | TitleBar.xaml:93 |
| 177 | `UnitConverter.xaml` | `<MenuFlyoutItem` | `CopyMenuItem` | UnitConverter.xaml:279 |
| 178 | `UnitConverter.xaml` | `<MenuFlyoutItem` | `PasteMenuItem` | UnitConverter.xaml:283 |
| 179 | `UnitConverter.xaml` | `<ComboBox` | `Units1` | UnitConverter.xaml:560 |
| 180 | `UnitConverter.xaml` | `<ComboBox` | `Units2` | UnitConverter.xaml:611 |
| 181 | `UnitConverter.xaml` | `CalculatorButton` | `ClearEntryButtonPos0` | UnitConverter.xaml:734 |
| 182 | `UnitConverter.xaml` | `CalculatorButton` | `BackSpaceButtonSmall` | UnitConverter.xaml:743 |
| 183 | `UnitConverter.xaml` | `CalculatorButton` | `ConverterNegateButton` | UnitConverter.xaml:765 |

## APPENDIX B — Extraction counts (machine summary)

| § | Metric | Count |
|---|---|---|
| 2 | Buttons in `CalculatorStandardOperators.xaml` | 13 |
| 2 | Buttons in `CalculatorScientificOperators.xaml` | 65 |
| 2 | Buttons in `CalculatorProgrammerOperators.xaml` | 4 |
| 2 | Buttons in `CalculatorProgrammerRadixOperators.xaml` | 38 |
| 2 | Buttons in `NumberPad.xaml` | 11 |
| 2 | Buttons in `Memory.xaml` | 1 |
| 2 | Buttons in `MemoryListItem.xaml` | 3 |
| 2 | Buttons in `HistoryList.xaml` | 1 |
| 2 | Buttons in `CalculatorProgrammerBitFlipPanel.xaml` | 64 |
| 2 | **Buttons total (§2)** | **200** |
| 2 | Button-like elements outside §2 | 184 |
| 3 | XAML shortcut bindings | 220 |
| 3 | `KeyboardShortcutManager` attached DPs | 6 |
| 5 | `Resources.resw` entries | 1166 |
| 5 | `CEngineStrings.resw` entries | 108 |
| 7 | Unit ID constants / `AddUnit` calls | 158 / 158 |
| 7 | Conversion factor entries | 155 |
| 7 | Temperature affine entries | 9 |
| 8 | `GraphingImpl` files (all under `Mocks/`) | 13 |
| 10 | Test methods (all projects) | 534 |

## APPENDIX C — Files read by this extraction

```
src/Calculator.ViewModels/Common/NavCategory.cs
src/Calculator.ViewModels/Common/MyVirtualKey.cs
src/Calculator.ViewModels/Common/BitLength.cs
src/Calculator.ViewModels/Common/NumberBase.cs
src/Calculator.ViewModels/Common/RadixType.cs
src/Calculator.ViewModels/Common/NumbersAndOperatorsEnum.cs
src/Calculator.ViewModels/Common/AppResourceProvider.cs
src/Calculator.ViewModels/StandardCalculatorViewModel.cs
src/Calculator.ViewModels/HistoryViewModel.cs
src/Calculator.ViewModels/ApplicationViewModel.cs
src/Calculator.ViewModels/DataLoaders/UnitConverterDataLoader.cs
src/Calculator.ViewModels/DataLoaders/CurrencyHttpClient.cs
src/Calculator.ViewModels/DataLoaders/CurrencyDataLoader.cs
src/Calculator.ViewModels/DataLoaders/DefaultFromToCurrency.json
src/Calculator/Common/KeyboardShortcutManager.cs
src/Calculator/Utils/ResourceString.cs
src/Calculator/Utils/ResourceVirtualKey.cs
src/Calculator/Resources/en-US/Resources.resw
src/Calculator/Resources/en-US/CEngineStrings.resw
src/Calculator/Controls/FlipButtons.cs
src/Calculator/Views/Settings.xaml(.cs)
src/Calculator/Views/*.xaml (all, recursive)
src/CalcManager/CalculatorManager.cpp
src/CalcManager/CalculatorHistory.cpp
src/CalcManager/CEngine/calc.cpp
src/CalcManager/CEngine/CalcInput.cpp
src/CalcManager/Header Files/CalcInput.h
src/CalcManager/Header Files/CalcEngine.h
src/CalcManager.Interop/CalcManagerImpl.cpp
src/GraphingImpl/** (all)
src/GraphingInterfaces/** (all)
src/CalculatorUnitTests/** (all)
src/Calculator.Tests/** (all)
src/CalculatorUITests/** (all)
src/CalculatorUITestFramework/** (all)
```

## APPENDIX D — REAL vs MOCK VERDICTS

| Subsystem | Verdict | Primary evidence |
|---|---|---|
| **Graphing engine** | **MOCK (100%)** | All 6 impl files live in `src/GraphingImpl/Mocks/`, namespace `MockGraphingImpl`. `dllmain.cpp` is empty; `ParseInput` returns a `MockExpression` for any input (`Mocks/MathSolver.h:118`); `DrawD2D1` draws nothing and returns `S_OK` (`Mocks/GraphRenderer.h:34`); `Bitmap::GetData` returns an empty vector (`Mocks/Bitmap.h:12`); zero `throw`/`TODO`/`NotImplemented` in the whole project. |
| **Currency data** | **MOCK (100%)** | `CurrencyHttpClient.cs:22-46` holds two hard-coded JSON literals (`MockCurrencyStaticData`, `MockCurrencyConverterData`) of fictional planets (Mars/Moon/Neptune/…). Both `GetCurrencyMetadataAsync` (`:53-62`) and `GetCurrencyRatiosAsync` (`:64-73`) `return Task.FromResult(<literal>)`. Comment at `:20-21` states *"Mock data … The upstream fwlink URLs are dead". The `HttpClient` at `:17` is constructed and never used. **No URL constant, no endpoint, no base URL exists in the repo.** |
| Unit converter data | **REAL** | 155 full double-precision SI/US-customary factors (e.g. `Area_Acre = 4046.8564224`, `Pressure_PSI = 6894.757293168`) plus a 9-entry affine temperature table. No placeholder/repeated filler values. |
| Mode manifest | **REAL** | 18 entries, 5 calculator + 13 converter, all resolved from resw. |
| Button layouts | **REAL** | 200 controls, all glyphs/styles resolved; nothing stubbed. |
| Shortcut table | **REAL** | 220 bindings, 100% resolve to non-missing resw values. |