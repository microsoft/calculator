# Calculator Manual Tests
These manual tests are run before every release of the Calculator app.

## Smoke Tests

### Calculators

### Math in Standard Calculator 

**Test 1**
Steps:
1. From the Standard Calculator page, input “3”, “+”, “3”, “Enter” on the keyboard 
*Expected: “6” shows up in the display *
2. Input “4”, “-”, “2”, “=” using the in-app buttons 
*Expected: “2” shows up in the display*
 
 **Test 2**
 Steps:
1. From the Standard Calculator page, input “3”, “+”, “3”, “Enter” on the keyboard 
2. Navigate to the History pane, and verify that “3 + 3 = 6” shows up in the pane 
3. Input “MS” using the in-app buttons 
4. Navigate to the Memory pane
*Expected: “6” shows up in the pane*

### Math in Scientific Calculator

**Test 1**
Steps: 
1. From the Scientific Calculator page, input “3”, “^”, “3”, “Enter” on the keyboard 
*Expected: “27” shows up in the display*

**Test 2**
Steps:
1. Input “5”, “n!”, “=” using the in-app buttons 
*Expected: “120” shows up in the display*

### Math in Programmer Calculator 

**Test 1**
Steps:
1. From the Programmer Calculator page, input “1”, “&”, “0”, “Enter” on the keyboard 
*Expected: “0” shows up in the display*

**Test 2**
Steps:
1. Input “15” using the in-app buttons and select “HEX” 
*Expected: “F” shows up in the display and the letters A-F show up as in-app buttons*

### Converters 

**Converter Usage**
Steps:
1. From the Length Converter page, select “kilometers” as the unit type in the input field and input “5” using the keyboard 
2. Select “miles” as the unit type in the output field 
*Expected: The output starts with is “3.106856”*

### Always-on-Top

**Test 1**
Steps:
1. Launch the "Calculator" app and navigate to "Standard" Calculator
*Expected: Always-on-Top button's tooltip says "Keep on top"*
2. Click the Always-on-Top button
*Expected: Always-on-Top button's tooltip now says "Back to full view"*
3. Launch the "Notepad" app and put it in full-screen mode
*Expected: Calculator is still on top of Notepad and in Always-on-Top mode*

**Test 2**
Steps: 
1. Launch the "Calculator" app and from "Standard" Calculator, input “3”, “+”, “3” (do not press “Enter”)
2. Tab over the Always-on-Top button and press "Enter" on the keyboard
*Expected: The application title, hamburger menu, calculator type title, calculation expression (the secondary line above the main display), history button and memory buttons are no longer visible. The main display shows "3"*
2. Press “Enter”
*Expected: The main display shows "6"*
3. Press "Ctrl-H" on the keyboard
*Expected: Nothing happens (history keyboard shortcuts are disabled)*
4. Press "Ctrl-P" on the keyboard, then tab over the Always-on-Top button and press "Enter" on the keyboard again
5. Open the Memory panel
*Expected: Nothing is stored in memory (memory keyboard shortcuts are disabled in Always-on-Top mode) and "6" is in history*

**Test 3**
Steps: 
1. Launch the "Calculator" app and from "Standard" Calculator, click the Always-on-Top button
2. Resize the window horizontally
*Expected: The buttons automatically expand or shrink to fit the available screen size*
3. Resize the window vertically
*Expected: The buttons automatically expand or shrink to fit the available screen size and the percent, square-root, squared and reciprocal buttons disappear when the screen height is small*
4. Click the Always-on-Top button again
*Expected: Calculator is in Standard mode and the original window layout from before Step 1 is restored*
5. Click the Always-on-Top button again
*Expected: Calculator is in Always-on-Top mode and the window size from after Step 3 is restored*
6. Close the "Calculator" app
7. Launch the "Calculator" app again and click the Always-on-Top button
*Expected: The window size from right before closing from Always-on-Top mode (ie. after Step 5) is restored*

**Test 4**
Steps:
1. Launch the "Calculator" app and from "Standard" Calculator, click the Always-on-Top button
2. Input "/", "0", “Enter” on the keyboard
*Expected: "Result is undefined" is displayed in the system default app language*
3. Click the Always-on-Top button again
*Expected: Calculator is in Standard mode and all operator (except for "CE", "C", "Delete" and "=") and memory buttons are disabled

**Test 5**
Steps:
1. Launch the "Calculator" app and navigate to "Scientific" Calculator
*Expected: The Always-on-Top button is hidden*
2. Navigate to "Standard" Calculator
*Expected: The Always-on-Top button is visible*

## Basic Verification Tests

**Launch App Test**
Steps:
1.	Press the Windows key.
2.	Navigate to "all apps".
3.	Look for "Calculator".
4.	Click to launch the "Calculator" app.
*Expected: The calculator app launches gracefully.*


**All Calculators Test: Verify All Numbers & Input Methods**
Steps:
1.	Launch the "Calculator" app.
2.	Navigate to "Standard" Calculator.
3.	Mouse Input
*Expected: All numbers work via mouse click.*
4.	Keyboard Input:
*Expected: All numbers work via number pad.*
5.	Navigate to "Scientific" Calculator and Repeat Steps 3-5
*Expected: Steps 3-5 pass in Scientific mode*
6.	Navigate to "Programmer" Calculator and Repeat Steps 3-5
*Expected: Steps 3-5 pass in Programmer mode*


**All Calculators Test: Verify Basic Arithmetic Functions**
Steps:
1.	Launch the "Calculator" app.
2.	Navigate to "Standard" Calculator.
3.	Using the Number Pad and Mouse perform the following arithmetic functions and verify the result.
    a.	(+) Addition
    b.	(-) Subtraction
    c.	(x) Multiplication
    d.	(÷) Division
    e.	(1/x) Reciprocal
    f.	(√) Square Root
    g.	(x²) Squared
    h.	(x³) Cubed
    i.	(%) Percent
    j.	(±) Positive / Negative
    k.	(=) Equals
    l.	Delete Button (flag with x in it)
    m.	[CE] Clear
    n.	[C] Global Clear
    o.	(.) Decimal
4.	Navigate to "Scientific" Calculator and Repeat Steps 3-19.
5.	Navigate to "Programmer" Calculator and Repeat Steps 3-18 (No Decimal in Programming Calc).


**Scientific Calculator Test: Verify advanced arithmetic functions**
Steps:
1.	Launch the "Calculator" app.
2.	Navigate to "Scientific" Calculator.
3.	Using the Number Pad and Mouse perform the following arithmetic functions and verify the result.
    a.	(xʸ) Xth Power of Y
    b.	(y√x) Y Root of X
    c.	(10ˣ) 10 Power of X
    d.	(ex) E Power of X
    e.	(π) Pi
    f.	(n!) Factorial
    g.	(Ln) Natural Logarithm
    h.	(Log) Logarithm
    i.	(Exp) Exponential
    j.	(dms) Degrees, Minutes, Seconds
    k.	(deg) Degrees
    l.	(Mod) Modulo
    m.	“( )" Parenthesis


**All Calculators Test: Verify memory functions**
Steps:
1.	Launch the "Calculator" app.
2.	Navigate to "Standard" Calculator.
3.	Perform a calculation and press the MS button.
4.	If small scale, Select the (M) with the drop down arrow
*Expected: Calculation from previous step is present.*
5.	Click the (M+) Add to Memory.
*Expected: Previous calculation is added to itself.*
6.	Click the (M-) Subtract from Memory.
*Expected: Previous calculation is subtracted from the base calculation.*
7.	Click the (MR) Memory Recall.
*Expected: Previous calculation is made primary (This is not available in the Programmer mode).*
8.	Check the MC button.
*Expected: The stored information is cleared.*
9.	Navigate to "Scientific" Calculator and Repeat Steps 3-8.
*Expected: All in "Scientific" mode.*
10.	Navigate to "Programmer" Calculator and Repeat Steps 3-8.
*Expected: All in "Programmer" mode.*


**Scientific Calculator Test: Verify trigonometric functions**
Steps:
1.	Launch the "Calculator" app.
2.	Navigate to "Scientific" Calculator.
3.  Using the Number Pad and Mouse perform the following trigonometric functions and verify the result.
3.	Sine (sin)
4.	Cosine (cos)
5.	Tangent (tan)
6.	Inverse Sine (sin-1)
7.	Inverse Cosine (cos-1)
8.	Inverse Tangent (tan-1) Inverse Tangent:
9.	Press (HYP) for Hyperbolic trig functions: 
*Expected: Trig function buttons show hyperbolic trig functions.*
10.	Hyperbolic Sine (sinh)
11.	Hyperbolic Tangent (tanh) 
12.	Hyperbolic Cosine (cosh) 
13.	Inverse Hyperbolic Sine (sinh-1) 
14.	Inverse Hyperbolic Tangent (tanh-1) 
15.	Inverse Hyperbolic Cosine (cosh-1) 


**Programmer Calculator Test: Verify logical functions**
Steps:
1.	Launch the "Calculator" app
2.	Navigate to "Programmer" Calculator.
3.  Using the Number Pad and Mouse perform the following trigonometric functions and verify the result.
4.	Rotate Left (RoL) Logical Operator:
    01011001 rol 3 = 11001010.
5.	Rotate Right (RoR) Logical Operator:
    01011001 RoR 3 = 00101011.
6.	(Lsh) Logical Operator:
    (10 multiplied by 2 three times)
    10 Lsh 3 = gives 80.
    10.345 Lsh 3 = also gives 80.
7.	(Rsh) Logical Operator:
    (16 divided by 2 twice)
    16 Rsh 2 = gives 4.
    16.999 Rsh 2 = also gives 4.
7.	(Or) Logical Operator
    101 OR 110 = gives 111.
9.	Exclusive Or (Xor) Logical Operator:
    101 XOR 110 = gives 11.
9.	(Not) Logical Operator
    NOT 1001100111001001 =
    0110011000110110.
10.	(And) Logical Operator
    101 AND 110 = gives 100.
11.	(Mod) Logical Operator
    Remainder of integer division (Modulo x)
12.	"( )" Parenthesis


**All Calculators and Converters Test: Verify scaling functions and languages**
Steps:
1.	Launch the "Calculator" app.
2.	For All Modes: While scaling in both directions to capacity
*Expected: Elements like Memory and History are shifted or integrated appropriately.*
3.	In Any Mode: While at the Smallest scale, Select the Menu Button
*Expected: The menu items are scrollable with nothing overlapping.*
4.	While in the Menu: Check the About Page
*Expected:  Everything in the about page fits into its window*
5.	For Scientific Mode: At a Larger Scale
*Expected: All buttons are present and the 2nd button is grayed out.*
6.	For Scientific Mode: At a Smaller Scale
*Expected: All buttons are present and the 2nd button is able to be toggled.*
7.	For Programmer Mode: At a Any Scale
*Expected: All buttons are present and the 2nd button is able to be toggled.*
8.	For Converter Mode: While Scaling
*Expected: The number pad and input areas move around each other gracefully.*
9.  For Graphing Mode: While Scaling
*Expected: The number pad, graph area, and input areas move around each other gracefully.*
10.	Changing Language: Open Settings app > Time & language > Region & language > Add a language > Select a Right to Left (RTL) language such as Hebrew > Install the associated files> Set it to the system default.
11.	Set the system number format preference: Open a Run dialog (WIN + R) > type ‘intl.cpl’ > Enter > In the format dropdown list > Select Hebrew > Apply.
12.	Initiating the change: Package has completed installing > Sign out > Sign in. (This change to the app may also require a reinstallation of the build)
13.	Repeat Steps 2-6 again in a (RTL) language.
*Expected: No elements fall out of intended boundaries.*


**All Calculators Test: Verify toggling functions**
Steps:
1.	Launch the "Calculator" app.
2.	For Standard & Scientific Modes: While in the Smallest scale, verify that the History Icon brings up the history panel gracefully and is displayed appropriately.
For Scientific Mode: At a Smaller Scale
Verify the following:
3.	Grad / Deg / Rad
    Perform a trig function
*Expected: The answer to the function is in the selected grad/deg/rad. Repeat for each of the modes.*
4.	(Hyp) Hyperbolic
*Expected: Sin toggles to Sinh, Cos toggles to Cosh, Tan toggles to Tanh.*
5.	(F-E key) Floating Point Notation & Scientific Notation.
*Expected: Display toggles between floating point and Scientific notation.*
For Programmer Mode
Verify the following:
6.	"Bit Toggling Keypad":
*Expected: In app keypad changes to represent Bits (1s and 0s).*
7.	"QWORD / DWORD / WORD / BYTE":
*Expected: Toggles as expected.*
8.	"Hex" Hexadecimal:
*Expected: A B C D E F become active and user can use them. A maximum of 16 characters can be entered.*
9.	"Dec" Decimal:
*Expected: A B C D E F are inactive. A maximum of 19 characters can be entered.*
10.	"Oct" Octal:
*Expected: A B C D E F 8 9 are inactive. A maximum of 22 characters can be entered.*
11.	"Bin" Binary:
*Expected: A B C D E F 2 3 4 5 6 7 8 9 are inactive. A maximum of 64 characters can be entered.*

**Graphing Mode Test: Verify Graphing mode functions**
Steps:
1.  Launch the "Calculator" app
2.  Navigate to "Graphing" Calculator
3.  Enter a function of x in the input field <br>
*Expected: Function is plotted in the graph area. Line color matches the colored square next to the input field*
4.  Select the "+" button below the function input and enter more functions in the fields that appear <br>
*Expected: All functions are plotted in the graph area and match the colors of the input field squares*
5.  Select the colored square for any function <br>
*Expected: Visibility of the function in the graph is toggled off/on*
6.  Select the "Zoom In", "Zoom Out", and "Reset View' buttons in the graph area <br>
*Expected: Both X and Y axes zoom in, out, and revert to default settings, respectively*
7.  Select the Trace button, then click + drag the graph until the red square is near a graphed function<br>
*Expected: Closest (X, Y) coordinates of the function to the red square are displayed with a black dot to indicate the location*
8.  Enter "y=mx+b" into a function input field, then select "Variables" button <br>
*Expected: y=x+1 function is plotted in the graph, "Variables" modal window shows two variables "m" and "b" with values set to 1.*
9.  Adjust the value, minimum, maximum, and step for each variable <br>
*Expected: y=mx+b graph adjusts to the new values for m and b, step size changes the increments of the slider for each value*
10.  Share the graph via OneNote, Outlook/Mail, Twitter, and Feedback Hub <br>
*Expected: Modifiable message that contains an image of the graph customized for the chosen application opens*
11.  Verify Key Graph Features tab shows the correct information for the following functions: <br>
    *(Note: IP = Inflection Points, VA = Vertical Asymptotes, HA = Horizontal Asymptotes, OA = Oblique Asymptotes)* <br>
    a.  **y=x** <br>
    *Expected: Domain: ⁅𝑥∈ℝ⁆; Range: ⁅y∈ℝ⁆; X/Y Intercepts: (0)/(0); Max: none; Min: none; IP: none; VA: none; HA: none; OA: none; Parity: Odd; Monotonicity: (-∞, ∞) Increasing* <br>
    b.  **y=1/x** <br>
    *Expected: Domain: ⁅𝑥≠0⁆; Range: ⁅y∈ℝ\{0}⁆; X/Y Intercepts: ø/ø; Max: none; Min: none; IP: none; VA: x=0; HA: y=0; OA: none; Parity: Odd; Monotonicity: (0, ∞) Decreasing, (-∞, 0) Increasing* <br>
    c.  **y=x^2** <br>
    *Expected: Domain: ⁅𝑥∈ℝ⁆; Range: ⁅y∈{0, ∞)⁆; X/Y Intercepts: (0)/(0); Max: none; Min: (0,0); IP: none; VA: none; HA: none; OA: none; Parity: Even; Monotonicity: (0, ∞) Increasing, (-∞, 0) Decreasing* <br>
    d.  **y=x^3** <br>
    *Expected: Domain: ⁅𝑥∈ℝ⁆; Range: ⁅y∈ℝ⁆; X/Y Intercepts: (0)/(0); Max: none; Min: none; IP: (0,0); VA: none; HA: none; OA: none; Parity: Odd; Monotonicity: (-∞, ∞) Increasing* <br>
    e.  **y=e^x** <br>
    *Expected: Domain: ⁅𝑥∈ℝ⁆; Range: ⁅y∈(0, ∞)⁆; X/Y Intercepts: ø/(1); Max: none; Min: none; IP: none; VA: none; HA: y=0; OA: none; Parity: none; Monotonicity: (-∞, ∞) Increasing* <br>
    f.  **y=ln(x)** <br>
    *Expected: Domain: ⁅𝑥>0⁆; Range: ⁅y∈ℝ⁆; X/Y Intercepts: (1)/ø; Max: none; Min: none; IP: none; VA: x=0; HA: none; OA: none; Parity: none; Monotonicity: (0, ∞) Increasing* <br>
    g.  **y=sin(x)** <br>
    *Expected: Domain: ⁅𝑥∈ℝ⁆; Range: ⁅𝑦∈[−1,1]⁆; X/Y Intercepts: (⁅𝜋n1,n1∈ℤ⁆)/(0); Max: ⁅(2𝜋n1+𝜋/2,1),n1∈ℤ⁆; Min: ⁅(2𝜋n1+3𝜋/2,−1),n1∈ℤ⁆; IP: ⁅(𝜋n1,0),n1∈ℤ⁆; VA: none; HA: none; OA: none; Parity: Odd; Monotonicity: ⁅(2𝜋n1+𝜋/2,2𝜋n1+3𝜋/2),n1∈ℤ⁆ Decreasing; ⁅(2𝜋n1+3𝜋/2,2𝜋n1+5𝜋/2),n1∈ℤ⁆ Increasing; Period: 2𝜋* <br>
    h.  **y=cos(x)** <br>
    *Expected: Domain: ⁅𝑥∈ℝ⁆; Range: ⁅𝑦∈[−1,1]⁆; X/Y Intercepts: (⁅𝜋n1+𝜋/2,n1∈ℤ⁆)/(1); Max: ⁅(2𝜋n1,1),n1∈ℤ⁆; Min: ⁅(2𝜋n1+𝜋,-1),n1∈ℤ⁆; IP: ⁅(𝜋n1+𝜋/2,0),n1∈ℤ⁆; VA: none; HA: none; OA: none; Parity: Even; Monotonicity: ⁅(2𝜋n1+𝜋,2𝜋n1+2𝜋),n1∈ℤ⁆ Increasing, ⁅(2𝜋n1,2𝜋n1+𝜋),n1∈ℤ⁆ Decreasing; Period: 2𝜋* <br>
    i.  **y=tan(x)** <br>
    *Expected: Domain: ⁅x≠𝜋n1+𝜋/2,∀n1∈ℤ⁆; Range: ⁅𝑦∈ℝ⁆; X/Y Intercepts: (x=𝜋n1, n1 ∈ℤ)/(0); Max: none; Min: none; IP: x=𝜋n1, n1 ∈ℤ; VA: x=𝜋n1+𝜋/2, n1∈ℤ; HA: none; OA: none; Parity: Odd; Monotonicity: ⁅(𝜋n1+𝜋/2,𝜋n1+3𝜋/2),n1∈ℤ⁆ Increasing; Period: 𝜋* <br>
    j.  **y=sqrt(25-x^2)** <br>
    *Expected: Domain: ⁅x∈[-5,5]⁆; Range: ⁅𝑦∈[0,5]⁆; X/Y Intercepts: (5),(-5)/(5); Max: (0,5); Min: (-5,0) and (5,0); IP: none; VA: none; HA: none; OA: none; Parity: Even; Monotonicity: (0,5) Decreasing, (-5,0) Increasing* <br>
    k.  **y=(-3x^2+2)/(x-1)** <br>
    *Expected: Domain: ⁅x≠1⁆; Range: ⁅𝑦∈(-∞, -2√3 - 6}U{2√3 -6,∞⁆; X/Y Intercepts: (-√6/3),(√6/3)/(-2); Max: ⁅(√3/3+1,-2√3−6)⁆; Min: ⁅(−√3/3+1,2√3−6)⁆; IP: none; VA: x=1; HA: none; OA: y=-3x-3; Parity: none; Monotonicity: (√3/3+1,∞) Decreasing, (1,√3/3+1,) Increasing(-√3/3+1,1), Increasing, (-∞,-√3/3+1) Decreasing* <br>
    l. **y=sin(sin(x))** ("too complex" error test) <br>
    *Expected: Domain: ⁅𝑥∈ℝ⁆; Range: Unable to calculate range for this function; X/Y Intercepts: none; Max: none; Min: none; IP: none; VA: none; HA: none; OA: none; Parity: odd; Monotonicity: Unable to determine the monotonicity of the function* <br>
    *These features are too complex for Calculator to calculate: Range, X Intercept, Period, Minima, Maxima, Inflection Points, Monotonicity*
    m. **y=mx+b** <br>
    *Expected: Analysis is not supported for this function*

**Date Calculation Test: Verify dates can be calculated.**
Steps:
1.	Launch the "Calculator" app
2.	Navigate to "Date Calculation" Calculator
3.  With "Difference between dates" Selected
    Change the various date input fields
*Expected: From and To reflect dates input respectively.*
5.	With "Add or Subtract days" Selected
    Change the various date input fields
*Expected: Verify changes made to both add and subtract reflect input respectively.*


**Currency Converter Test: Verify conversion & updating current currency rates.**
Steps:
1.	Launch the "Calculator" app.
2.	Navigate to "Currency Converter" Calculator.
3.	Select 2 Currency types from the dropdowns & enter a "1" into a conversion slot.
*Expected: The currency is slotted properly and converted rate matches the ratio provided under the selected currency types.*
4.	Click "Updated"
*Expected: Display matches PC's date and time.*
5.	After at least a minute: Select "Update rates" & Check "Updated" again:
*Expected: The "Update Rates" button changes the date and time to match the computer's current date and time.*


**All Calculators Test: Hotkeys: Verify Hot Key function.**
Steps:
1.	Launch the "Calculator" app.

    For All Applicable Modes verify the following (note: only 11-15 and 20 work in Always-on-Top mode):
2.	Press **Alt +1** to enter "Standard" mode
*Expected: Move to "Standard" screen.*
3.	Press **Alt +2** to enter "Scientific" mode
*Expected: Move to "Scientific" screen.*
4.	Press **Alt +3** to enter "Programmer" mode
*Expected: Move to "Programming" screen.*
5.	Press **Alt +4** to enter "Date Calculation" mode
*Expected: Move to "Date Calculation" screen.*
6   Press **Alt +5** to enter "Graphing" mode
*Expected: Move to "Graphing" screen.*
7.	Press **Ctrl +M** to Store in Memory
8.	Press **Ctrl +P** to Add to Active Memory
9.	Press **Ctrl +Q** to Subtract form Active Memory
10.	Press **Ctrl +R** to Recall from Memory
11.	Press **Ctrl +L** to Clear from Memory
12.	Press **Delete** to Clear Current Input 'CE'
13.	Press **Esc** to Full Clear Input 'C'
14.	Press **F9** to Toggle '±'
15.	Press **R** to Select '1/x'
16.	Press **@** to Select '√'
17.	Press **Ctrl + H** to Toggle History Panel
*Expected: Function when in small scale window.*
18.	Press **Up arrow** to Move up History Panel
*Expected: Function when in small scale window.*
19.	Press **Down arrow** to Move Down History Panel
*Expected: Function when in small scale window.*
20.	Press **Ctrl + Shift + D** to Clear History Panel
*Expected: Function when in small scale window.*
21.	Press **Spacebar** to Repeat Last Input

    Verify the following in Scientific Mode
22.	Press **F3** to Select 'DEG'
23.	Press **F4** to Select 'RAD'
24.	Press **F5** to Select 'GRAD'
25.	Press **Ctrl +G** to Select '10ˣ'
26.	Press **Ctrl +Y** to Select 'y√x'
27.	Press **Shift +O** to Select 'sin-1'
28.	Press **Shift + S** to Select 'cos-1'
29.	Press **Shift +T** to Select 'tan-1'
30.	Press **Ctrl +O** to Select 'Cosh'
31.	Press **Ctrl +S** to Select 'Sinh'
32.	Press **Ctrl +T** to Select 'Tanh'
33.	Press **D** to Select 'Mod'
34.	Press **L** to Select 'log'
35.	Press **M** to Select 'dms'
36.	Press **N** to Select 'ln'
37.	Press **Ctrl +N** to Select 'ex'
38.	Press **O** to Select 'Cos'
39.	Press **P** to Select 'π'
40.	Press **Q** to Select 'x²'
41.	Press **S** to Select 'Sin'
42.	Press **T** to Select 'Tan'
43.	Press **V** to Toggle 'F-E'
44.	Press **X** to Select 'Exp'
45.	Press **Y** or **^** to Select 'xʸ'
46.	Press **#** to Select 'x³'
47.	Press **!** to Select 'n!'

    Verify the following in Programmer Mode
48.	Press **F2** to Select 'DWORD'
49.	Press **F3** to Select 'WORD'
50.	Press **F4** to Select 'BYTE'
51.	Press **F5** to Select 'HEX'
52.	Press **F6** to Select 'DEC'
53.	Press **F7** to Select 'OCT'
54.	Press **F8** to Select 'BIN'
55.	Press **F12** to Select 'QWORD'
56.	Press **A-F** to Input in HEX
57.	Press **J** to Select 'RoL'
58.	Press **K** to Select 'RoR'
59.	Press **<** to Select 'Lsh'
60.	Press **>** to Select 'Rsh'
61.	Press **%** to Select 'Mod'
62.	Press **|** to Select 'Or'
63.	Press **~** to Select 'Not'
64.	Press **&** to Select 'And'

    Verify the following in Graphing Mode
65. Press **x** to Select 'x'
66. Press **y** to Select 'y'
67. Press **Ctrl +[Numpad+]** to Select 'Zoom In'
68. Press **Ctrl +[Numpad-]** to Select 'Zoom Out'

## Localization Tests

### Always-on-Top

**Test 1**
Steps:
1. Change the system default app language to Arabic
2. Launch the "Calculator" app and from "Standard" Calculator, click the Always-on-Top button
*Expected: UI/Menu is localized (for example, the title bar buttons is in right-to-left order)*
3. Input "/", "0", “Enter” on the keyboard
*Expected: Error message is in Arabic*

## Ease of Access Tests

### Always-on-Top

**Test 1**
Steps:
1. Open the "Narrator" app
2. Launch the "Calculator" app and from "Standard" Calculator, click the Always-on-Top button
3. Tab over the Always-on-Top button
*Expected: Narrator reads the localized version of "Back to full view"*
4. Tab over the main results field
*Expected: Narrator reads the localized version of exactly what's displayed (ie. "0")*
5. Tab over the rest of the UI elements
*Expected: Narrator reads the localized version of the UI elements' contents*
