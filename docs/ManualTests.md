# Calculator Manual Tests
These manual tests are run before every release of the Calculator app.

## Smoke Tests

### Calculators

### Math in Standard Calculator 

**Test 1**
Steps:
1. From the Standard Calculator page, input “3”, “+”, “3”, “Enter” on the keyboard 
Expected: “6” shows up in the display 
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
*Expected: All buttons are present and the up arrow is grayed out.*
6.	For Scientific Mode: At a Smaller Scale
*Expected: All buttons are present and the up arrow is able to be toggled.*
7.	For Programmer Mode: At a Any Scale
*Expected: All buttons are present and the up arrow is able to be toggled.*
8.	For Converter Mode: While Scaling
*Expected: The number pad and input areas move around each other gracefully.*
9.	Changing Language: Open Settings app > Time & language > Region & language > Add a language > Select a Right to Left (RTL) language such as Hebrew > Install the associated files> Set it to the system default.
10.	Set the system number format preference: Open a Run dialog (WIN + R) > type ‘intl.cpl’ > Enter > In the format dropdown list > Select Hebrew > Apply.
11.	Initiating the change: Package has completed installing > Sign out > Sign in. (This change to the app may also require a reinstallation of the build)
12.	Repeat Steps 2-6 again in a (RTL) language.
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


**Date Calculation Test: Verify dates can be calculated.**
Steps:
1.	Launch the "Calculator" app.
2.	Navigate to "Date Calculation" Calculator.
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

    For All Applicable Modes verify the following:
2.	Press **Alt +1** to Enter "Standard" mode
*Expected: Move to "Standard" screen.*
3.	Press **Alt +2** to Enter "Scientific" mode
*Expected: Move to "Scientific" screen.*
4.	Press **Alt +3** to Enter "Programmer" mode
*Expected: Move to "Programming" screen.*
5.	Press **Alt +4** to Enter "Date Calculation" mode
*Expected: Move to "Date Calculation" screen.*
6.	Press **Ctrl +M** to Store in Memory
7.	Press **Ctrl +P** to Add to Active Memory
8.	Press **Ctrl +Q** to Subtract form Active Memory
9.	Press **Ctrl +R** to Recall from Memory
10.	Press **Ctrl +L** to Clear from Memory
11.	Press **Delete** to Clear Current Input 'CE'
12.	Press **Esc** to Full Clear Input 'C'
13.	Press **F9** to Toggle '±'
14.	Press **R** to Select '1/x'
15.	Press **@** to Select '√'
16.	Press **Ctrl + H** to Toggle History Panel
*Expected: Function when in small scale window.*
17.	Press **Up arrow** to Move up History Panel
*Expected: Function when in small scale window.*
18.	Press **Down arrow** to Move Down History Panel
*Expected: Function when in small scale window.*
19.	Press **Ctrl + Shift + D** to Clear History Panel
*Expected: Function when in small scale window.*
20.	Press **Spacebar** to Repeat Last Input

    Verify the following in Scientific Mode
21.	Press **F3** to Select 'DEG'
22.	Press **F4** to Select 'RAD'
23.	Press **F5** to Select 'GRAD'
24.	Press **Ctrl +G** to Select '10ˣ'
25.	Press **Ctrl +Y** to Select 'y√x'
26.	Press **Shift +O** to Select 'sin-1'
27.	Press **Shift + S** to Select 'cos-1'
28.	Press **Shift +T** to Select 'tan-1'
29.	Press **Ctrl +O** to Select 'Cosh'
30.	Press **Ctrl +S** to Select 'Sinh'
31.	Press **Ctrl +T** to Select 'Tanh'
32.	Press **D** to Select 'Mod'
33.	Press **L** to Select 'log'
34.	Press **M** to Select 'dms'
35.	Press **N** to Select 'ln'
36.	Press **Ctrl +N** to Select 'ex'
37.	Press **O** to Select 'Cos'
38.	Press **P** to Select 'π'
39.	Press **Q** to Select 'x²'
40.	Press **S** to Select 'Sin'
41.	Press **T** to Select 'Tan'
42.	Press **V** to Toggle 'F-E'
43.	Press **X** to Select 'Exp'
44.	Press **Y** or **^** to Select 'xʸ'
45.	Press **#** to Select 'x³'
46.	Press **!** to Select 'n!'

    Verify the following in Programmer Mode
47.	Press **F2** to Select 'DWORD'
48.	Press **F3** to Select 'WORD'
49.	Press **F4** to Select 'BYTE'
50.	Press **F5** to Select 'HEX'
51.	Press **F6** to Select 'DEC'
52.	Press **F7** to Select 'OCT'
53.	Press **F8** to Select 'BIN'
54.	Press **F12** to Select 'QWORD'
55.	Press **A-F** to Input in HEX
56.	Press **J** to Select 'RoL'
57.	Press **K** to Select 'RoR'
58.	Press **<** to Select 'Lsh'
59.	Press **>** to Select 'Rsh'
60.	Press **%** to Select 'Mod'
61.	Press **|** to Select 'Or'
62.	Press **~** to Select 'Not'
63.	Press **&** to Select 'And'
