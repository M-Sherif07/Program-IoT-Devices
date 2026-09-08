# Revision


## Introduction
You already have the core toolkit an IoT device needs: variables and functions, digital output, reading inputs with debouncing and selection, combining sensors with boolean logic and non-blocking timing, arrays and loops, and structs with dictionary-style lookups. Each week's Resource introduced one of those tools on its own, under its own heading.


## Key Concepts

### Variables, Input-Process-Output, and Functions
Every IoT program is the same three-step pattern:

| Step | What it means | Example |
|---|---|---|
| **Input** | Read the environment | A temperature sensor sends a reading |
| **Process** | Decide or calculate | If temperature > 30, set alert = true |
| **Output** | Act on the result | Turn on the red LED or buzzer |

This maps directly onto a sketch's structure: `setup()` does none of these three (it's one-time configuration), and every pass of `loop()` is one Input → Process → Output cycle — read a pin, decide with `if`/`switch`, act with `digitalWrite()`/`analogWrite()`/`Serial.print()`.

A **variable** is a named container that stores a value in memory (`int temperature = 25;`), declared as `type name = value;`. The type matters because it fixes both what the variable can hold and how much memory it costs:

| Type | Holds | Example |
|---|---|---|
| `int` | Whole numbers | `int count = 10;` |
| `long` | Larger whole numbers than `int`'s range allows | `long timestamp = 99999;` |
| `float` | Decimal numbers | `float temp = 22.5;` |
| `char` | A single character | `char grade = 'A';` |
| `String` | Text | `String name = "sensor";` |
| `bool` | `true`/`false` | `bool isOn = true;` |
| `byte` | Small positive numbers, 0–255 | `byte b = 255;` |

Where a variable is *declared* controls where it can be used — its **scope**. A variable declared outside every function (at the top of the file) is **global** and visible everywhere, including inside every function and across every pass of `loop()`; one declared inside a function or a `{ }` block is **local**, only exists inside that block, and is destroyed, its value lost the moment the block ends. This is why a counter that needs to keep increasing across loop cycles (`int count = 0;` at the top of the file, `count++;` inside `loop()`) must be global: a local variable declared inside `loop()` would be recreated at `0` on every single pass.

A **constant** is a variable whose value is locked at declaration with the `const` keyword and can never change afterwards, the compiler rejects any later assignment to it. Pin numbers, thresholds, and other fixed values are declared `const` (conventionally in `UPPER_CASE`) rather than as plain variables, both to signal intent to a reader and to catch an accidental reassignment at compile time instead of it silently causing a bug at runtime.

```cpp
const int LED_PIN = 13;    // fixed for the life of the program
int sensorValue = 0;       // expected to change every loop() cycle
```

A **function** is a named, reusable block of code — write the logic once, then run it again with `functionName()` instead of copying the same lines everywhere they're needed. A function's signature has four parts:

```cpp
returnType functionName(parameters) {
  // body
  return value;   // only present when returnType isn't void
}
```

`returnType` is `void` when the function only performs an action (`digitalWrite`, printing, blinking) and sends nothing back; it's a real type (`int`, `bool`, `float`, …) when the function calculates or checks something and needs to hand that result back to whoever called it, via `return`. `parameters` are optional input values inside the `()` — a function with parameters (`blink_led(int pin, int times)`) can behave differently on each call depending on what's passed in, while a function with an empty `()` always does exactly the same fixed thing every time. These two choices are independent of each other: a function can take parameters and still be `void` (act, don't report back), or take no parameters and still return a value.

```cpp
bool controlLed(bool turnOn) {   // parameter in, value returned — both at once
  digitalWrite(LED_PIN, turnOn);
  return turnOn;                  // confirms the new state to the caller
}

void blink_led(int pin, int times, int onTime) {   // parameters in, nothing returned
  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(onTime);
    digitalWrite(pin, LOW);
    delay(onTime);
  }
}
```

`blink_led(13, 3, 200)` and `blink_led(6, 1, 1000)` run the exact same body with different `pin`/`times`/`onTime` values substituted in — the parameters are what let one function definition stand in for what would otherwise be several near-identical, copy-pasted blocks.

### Reading Inputs, Debouncing, and Selection
A digital input pin reports one of exactly two voltage states, read with `digitalRead(pin)` as `HIGH` or `LOW`. A pin that isn't connected to anything definite is **floating**, its voltage isn't held at a clear level, so it can pick up electrical noise and return random readings — which is why a button needs a **pull-up** or **pull-down** resistor rather than being wired to the input pin alone. `pinMode(pin, INPUT_PULLUP)` holds the pin HIGH by default and wires the button so pressing it pulls the pin to GND, so "pressed" reads `LOW` — the opposite of what's often assumed on first read, which is exactly why it's worth stating explicitly rather than guessing from the variable name:

| Mode | Pin's default state | Pressed reads as |
|---|---|---|
| `INPUT_PULLUP` | `HIGH` | `LOW` |
| `INPUT_PULLDOWN` | `LOW` | `HIGH` |

A mechanical button's contacts "bounce," registering several rapid ON/OFF transitions for one physical press unless you **debounce** it (check that the reading has held steady for a settle time before trusting it). Without debouncing, one press can be read as several, a menu jumps forward multiple steps, or a counter increments by more than one because the code reacted to every bounce, not just the real change. The fix needs three pieces of state, not one:

| Variable | What it holds |
|---|---|
| `reading` | The raw value from `digitalRead()` this pass — may still be mid-bounce. |
| `lastButtonReading` | The raw value from the *previous* pass, used only to notice when the raw signal changes at all. |
| `buttonState` | The last value that has actually been *trusted*, updated only after the settle time has passed. |
| `debounceStart` | The `millis()` timestamp of the most recent raw change, used to measure how long the signal has held steady. |

```cpp
int reading = digitalRead(buttonPin);
if (reading != lastButtonReading) debounceStart = millis();
if (millis() - debounceStart > DEBOUNCE_TIME && reading != buttonState) {
  buttonState = reading;                 // only trust the reading once it has settled
  if (buttonState == LOW) { /* validated press */ }
}
lastButtonReading = reading;
```

Walking through what happens on a real bounce: each bounce transition changes `reading` relative to `lastButtonReading`, so the first `if` resets `debounceStart` every time — the clock keeps restarting as long as the signal is still noisy. Only once the physical contact has actually settled does a full `DEBOUNCE_TIME` (e.g. 40ms) pass without another reset, at which point the second `if` fires exactly once, `buttonState` is updated, and the "validated press" logic runs. `lastButtonReading = reading` runs unconditionally, outside every `if`, on every single pass — skipping it (or putting it inside a conditional) breaks the very next comparison, because the code would no longer know what the raw signal last looked like. This comparison is always written against `millis()`, never `delay()`, so debouncing one button doesn't freeze every other input in `loop()` while it settles.

**Selection** (`if`/`else`) lets the program branch on a condition instead of always running the same steps in the same order. An `if`/`else if`/`else` chain is checked top to bottom, and only the **first** branch whose condition is true runs — later conditions are never even evaluated once an earlier one matches, so ordering the branches from most specific to most general (or least specific to most, depending on the logic) matters whenever more than one condition could be true for the same values.

```cpp
if (temperature > 30) {
  Serial.println("Hot");
} else if (temperature > 20) {
  Serial.println("Warm");     // only reached if temperature is 30 or below
} else {
  Serial.println("Cold");
}
```

When one variable needs to choose between several specific, known values — a mode, a state, a menu choice — `switch`/`case` (also called **Select Case**) expresses that more clearly than a chain of `else if`s. It behaves exactly like an `if`/`else if`/`else` chain that only ever checks equality against one variable; it's a different way of writing the same idea, not a new capability, and it comes with two restrictions that an `if` chain doesn't have: each `case` label must be a compile-time constant (a literal or a `const`, never a range like `case 1...5` or a comparison), and the type being switched on must be an integer-like type (`int`, `char`, an `enum`) — never a `String` or `float`.

```cpp
switch (mode) {
  case 0:
    // runs only if mode == 0
    break;
  case 1:
    // runs only if mode == 1
    break;
  default:
    // runs if mode matched none of the cases above
    break;
}
```
`break` stops execution "falling through" into the next `case`; `default` is the optional catch-all, equivalent to a chain's final `else`. Omitting a `break` is usually a bug execution "falls through" and keeps running the *next* case's code too — but it's occasionally intentional, when several values should share the same handling: stacking `case 0:` directly above `case 1:` with no code or `break` between them means both values run the same block below.

### Boolean Logic, Combining Sensors, and Non-blocking Timing
`&&` (AND), `||` (OR), and `!` (NOT) combine multiple conditions into one decision. `&&` is true only when **both** sides are true (`if (motionDetected && armed)` alarm only if motion is seen *and* the system is armed); `||` is true when **either** side is (`if (doorOpen || windowOpen)` alert if any entry point is open); `!` inverts a single condition (`if (!locked)` reads as "if not locked"). A truth table makes the difference concrete:

| A | B | `A && B` | `A \|\| B` |
|---|---|---|---|
| true | true | true | true |
| true | false | false | true |
| false | true | false | true |
| false | false | false | false |

Two pitfalls come up constantly. First, `&&`/`||` **short-circuit**: in `if (sensorReady() && sensorReady() ... )`-style chains, C++ stops evaluating as soon as the result is already decided — `false && anything` never evaluates the right side, `true || anything` never evaluates the right side. This is more than an optimisation: `if (index < count && readings[index] > 0)` relies on it, because if `index` is out of bounds, the left side being `false` stops the right side from ever reading `readings[index]` and going out of bounds. Reordering that condition (`readings[index] > 0 && index < count`) is a genuine bug, not a style choice. Second, `&&`/`||` are not the same as `=`/`==`: a stray single `&` or `|` compiles (they're bitwise operators) but silently does the wrong thing, and `if (buttonState = LOW)` (one `=`) assigns rather than compares and is always "true" — a classic hard-to-spot typo.

Combining sensors often means detecting a **change**, not just a level "the door just closed" rather than "the door is closed right now." That needs the current reading compared against the *previous* pass's reading, exactly like the debounce pattern's `lastButtonReading`:

```cpp
bool dockNow = digitalRead(dockSensorPin) == HIGH;
if (dockNow && !lastDockState) {
  // rising edge: this is the pass where it just became true, not every pass while it's true
}
lastDockState = dockNow;
```

Without the `!lastDockState` term, the block would run on *every* pass the sensor reads HIGH, not once on the transition the same "one physical event, one response" problem debouncing solves for buttons.

Once a program has more than one thing to watch, `delay()` becomes a bug: it freezes the *entire* `loop()` for its full duration, including every sensor and button you're not currently waiting on — a 2-second `delay()` in one part of the code makes a button press elsewhere invisible for those 2 seconds. The fix is the **non-blocking `millis()` pattern**: record a start time, then compare elapsed time on every pass instead of pausing.

```cpp
unsigned long previousMillis = 0;
const long interval = 2000;

if (millis() - previousMillis >= interval) {
  previousMillis = millis();
  // do the timed thing — loop() never actually paused
}
```

`millis()` returns the milliseconds since the board booted, as an `unsigned long`, and it's always the *subtraction* `millis() - previousMillis` that's compared to `interval`, never `millis()` on its own — subtracting is what makes the pattern immune to the board's clock eventually wrapping back to `0` after about 49 days: unsigned arithmetic wraps the subtraction result correctly too, so the elapsed-time calculation stays correct straight through the rollover without any special-case code. Because each timer is just one `previousMillis` variable and one `interval` constant, a program can run several independent timers side by side in the same `loop()` — an LED blink, a sensor poll, and an alert timeout — none of them blocking each other or needing to know about the others, which is exactly what nested `delay()` calls could never do.

### Arrays, Loops, and Single-Pass Scans
An **array** stores a fixed-size collection of same-typed values under one name, indexed from `0` to `size - 1` (`float readings[5];` reserves five `float` slots; `readings[0]` is the first, `readings[4]` the last). The size is fixed at declaration — you cannot grow or shrink it at runtime — and C++ does **no bounds checking**: reading or writing `readings[5]` on a 5-element array doesn't crash or throw an error, it silently reads/corrupts whatever memory happens to sit next to the array. Getting the loop condition wrong (`i <= count` instead of `i < count`) is the single most common way this bug gets introduced, so it's worth reading that line twice.

A `for` loop is the standard tool for walking an array, because its three clauses map directly onto "start at the first index, keep going while there are more, move to the next one each time":

```cpp
for (int i = 0; i < count; i++) {   // init; condition; increment
  // runs once per index, i = 0, 1, 2, ... count-1
}
```

`count` is usually a separate variable or `const`, not re-derived from the array itself, because a function that receives an array parameter cannot recover its size from the array alone — the array "decays" to a pointer. If you do have the whole array in scope (not passed as a parameter), `sizeof(readings) / sizeof(readings[0])` gives the element count without hard-coding it, which keeps the loop correct if the array's declared size ever changes.

A **single-pass scan** means visiting every element exactly once and updating one or more running results as you go — a minimum, a maximum, a running sum, a count of matches — rather than looping over the array separately for each result, or re-reading earlier elements to compare against later ones. The pattern below finds the minimum, maximum, and average in one pass:

```cpp
float minVal = readings[0], maxVal = readings[0], sum = 0;
for (int i = 0; i < count; i++) {
  if (readings[i] < minVal) minVal = readings[i];
  if (readings[i] > maxVal) maxVal = readings[i];
  sum += readings[i];
}
float average = sum / count;
```

`minVal`/`maxVal` are seeded from `readings[0]`, **never** a hard-coded value like `0`, so a run of all-negative readings (every value less than `0`) or all-above-zero readings still produces a correct min/max — seeding from a made-up constant that happens to sit inside the expected range is a bug that only shows up on data outside that range, which is exactly the kind of case an exam or a real sensor will eventually feed it. The loop then starts at index `1`, not `0`, when the seed already came from index `0` — re-comparing index `0` against itself is harmless but pointless, so most single-pass scans write `for (int i = 1; ...)` once the seed line exists.

The same one-pass shape answers questions well beyond min/max/average — counting how many readings exceed a threshold, for example — needs no separate loop, just another running variable updated inside the same pass:

```cpp
int overLimitCount = 0;
for (int i = 0; i < count; i++) {
  if (readings[i] > LIMIT) overLimitCount++;
}
```

Two or more arrays that must always be updated together to describe one thing — index `i` in `pins[]` and index `i` in `states[]` refer to the same physical LED — are **parallel arrays**. They work, but every update site has to remember to touch both arrays at the matching index; forgetting one is the exact bug class structs (see below) are designed to remove.

```cpp
const int NUM_LEDS = 3;
int ledPins[NUM_LEDS]   = {5, 6, 7};
bool ledStates[NUM_LEDS] = {false, false, false};

for (int i = 0; i < NUM_LEDS; i++) {
  digitalWrite(ledPins[i], ledStates[i] ? HIGH : LOW);   // index i means the same LED in both arrays
}
```

### Structs and Dictionary-Style Lookups
A `struct` groups a fixed set of named fields — possibly of different types — into one record type:

```cpp
struct Reading {
  String sensor;
  float value;
};                              // trailing semicolon required — easy to forget, and forgetting it
                                // produces a confusing compiler error on the *next* line, not this one

Reading latest = {"temperature", 23.6};   // values matched to fields BY POSITION, in declaration order
latest.value = 24.1;                      // dot notation — the struct equivalent of array[i], by name instead of index
```

The definition (`struct Reading { ... };`) only declares a blueprint — it creates nothing in memory by itself, exactly like defining a function doesn't run it. `Reading latest;` on its own creates one real record with every field uninitialised (garbage), the same as writing `int x;` with no assignment; `Reading latest = {...}` creates and fills it in one step. That initializer list is matched to fields **by position**, not by name, so `{23.6, "temperature"}` (fields swapped) would silently assign the number to the `String` field and the text to the `float` field instead of raising an error — always keep the initializer's order matching the struct's declaration order.

An **array of structs** (`Reading readings[10];`) replaces the Week 5 parallel-array pattern — two or more arrays (e.g. `buttonPins[]`/`ledPins[]`) that must always be updated together at the same index to describe one thing. Once that pairing is a struct field instead of a separate array, `readings[i].timestamp` and `readings[i].value` can never fall out of sync, because they're not two array slots any more, just two fields of the one record at index `i` — the entire "updated one array but forgot the other" bug class is removed by construction, not by discipline.

C++ has no dictionary built into the language the way some higher-level languages do (and the standard library's `std::map` uses dynamic memory allocation usually avoided on a microcontroller for the same predictability reasons — a `struct` is preferred in the first place), so a name-keyed lookup table is simulated with a small **array of key-value structs**, searched with a plain loop — a **linear search** — checking each entry in turn until one matches:

```cpp
struct Threshold { String sensor; float limit; };
Threshold thresholds[3] = { {"temperature", 28.0}, {"humidity", 70.0}, {"light", 800.0} };

float get_threshold(String sensor) {
  for (int i = 0; i < 3; i++) {
    if (thresholds[i].sensor == sensor) return thresholds[i].limit;
  }
  return -1.0;   // sentinel — caller must check before trusting this
}
```

The `-1.0` **sentinel** is the function's only way to signal "no match" back through the same `float` return type that real limits use, so every call site must check it before trusting the result (`if (limit >= 0 && value > limit)`) — the same "did it actually work?" check `isnan()` forces after a failed DHT22 read. Skipping that check lets an unrecognised sensor name either silently pass every comparison or silently fail every comparison, depending on which operator happens to be used against `-1.0` — a bug that stays invisible until a name is misspelled or a sensor is removed from the table. A linear search's lookup time grows with the table's size, which is a non-issue for the handful of entries a microcontroller's settings or thresholds ever need, but is exactly why a real hash-map (constant-time lookup regardless of size) becomes worth its extra memory once a table grows into the hundreds or thousands of entries.

Keeping a struct array **sorted** as records are added rather than only sorting when asked uses **insertion sort**: after appending the new record at the end, compare it against its left neighbour and swap backward one step at a time until it reaches its correct position, then stop. Only the single new record ever moves, because every record already in the array was already in sorted order before this insert.

```cpp
int i = reading_count - 1;
while (i > 0 && readings[i - 1].timestamp > readings[i].timestamp) {
  Reading temp = readings[i];
  readings[i] = readings[i - 1];
  readings[i - 1] = temp;
  i--;
}
```
Because a struct assignment (`readings[i] = readings[i - 1]`) copies every field of the record in one line, this swap works identically whether the struct has two fields or ten — nothing here needs to change if a field is added to `Reading` later.

### Which Concept Solves Which Problem
The hardest part of an integration build isn't any single concept — it's recognising which one a requirement is actually asking for. A real assessment brief never says "use debouncing" or "use a struct"; it describes a *behaviour*, in plain English, and expects you to translate that behaviour. Two habits make that translation reliable: reading a requirement clause by clause rather than as one sentence (a single requirement often stacks two or three concepts at once — see the worked example below), and learning to recognise a concept from several different phrasings of the same underlying need, not just one memorised trigger word.

| The requirement says… | The concept it needs | Week |
|---|---|---|
| "…without duplicating this code for every LED/sensor" | A parameterised function | 2 |
| "…one physical press should count once" | Debouncing | 3 |
| "…only when both/either condition is true" | Boolean logic (`&&`/`\|\|`/`!`) | 4 |
| "…pick one of several known values/modes for a single variable" | `switch`/`case` (Select Case) | 3 |
| "…while everything else keeps working" / "…without freezing other inputs" | Non-blocking `millis()` timing, not `delay()` | 4 |
| "…the smallest/largest/average of several readings" | Single-pass array scan | 5 |
| "…only react the moment it changes, not the whole time it's true" | Edge detection (compare against last pass's reading) | 4 |
| "…several related values that must move together" | An array of structs, not parallel arrays | 5-6 |
| "…look this up by name, not by position" | A key-value struct array (dictionary) | 6 |
| "…report clearly if the value/sensor wasn't found" | A sentinel return value, checked before use | 6 |
| "…always stays in order as new data arrives" | Insertion sort on every insert | 6 |

#### Concepts that get confused with each other
Some pairs solve *related* but distinct problems, and picking the wrong one of the pair is a more common mistake than missing the concept family entirely:

| Easy to confuse… | …with | The actual difference |
|---|---|---|
| Debouncing | Edge detection | Debouncing filters electrical *noise* out of one still-changing signal before trusting it at all; edge detection compares two already-trusted readings (this pass vs. last pass) to catch the moment a stable signal *changes*. A clean digital sensor (a PIR, a dock switch) needs edge detection but not debouncing; a mechanical button typically needs both, debounced first, then edge-detected. |
| `if`/`else if` chain | `switch`/`case` | Interchangeable only when every branch checks the *same one variable* for *equality* against specific constants. A chain testing ranges (`temperature > 30`), multiple variables, or non-constant values can't be written as a `switch` at all. |
| `delay()` timing | Non-blocking `millis()` timing | Both "wait, then do something," but `delay()` freezes every other line of `loop()` for its duration; `millis()` timing does not. A requirement mentioning only one timed action in an otherwise simple sketch may still tolerate `delay()` — it's "while everything else keeps working" that rules it out. |
| Parallel arrays | Array of structs | Both can represent "several related values, index by index." Parallel arrays *can* be made to work; an array of structs makes the "always move together" constraint impossible to violate. If a requirement only implies related values, either can satisfy it — but "must move together" or "must never fall out of sync" specifically signals the struct version is expected. |
| Linear search sentinel | `isnan()` | Both answer "did this actually produce a usable value?" before the result is trusted, but `isnan()` checks whether a *sensor read* failed, while a sentinel (`-1`, `-1.0`) checks whether a *lookup* found a match — a `Threshold` array has no concept of `isnan()` at all. |

#### Worked example: reading one multi-part requirement
A brief phrased as one sentence often names three or four concepts back to back. Take: *"When the courier presses Confirm, only register the drop-off once per press, and only if the door sensor shows the door is actually closed; log the event with its time so the log always displays oldest-to-newest, and flash a warning LED without freezing the rest of the system if the battery reading is below the bike's configured minimum."*

| Clause | Concept it names | Week |
|---|---|---|
| "…only register the drop-off once per press" | Debouncing (or edge detection, if the source is already a clean digital signal) | 3-4 |
| "…only if the door sensor shows the door is actually closed" | Boolean AND combining two conditions | 4 |
| "…log the event with its time so the log always displays oldest-to-newest" | A struct record + insertion sort on every insert | 5-6 |
| "…the bike's configured minimum" | A key-value struct array (dictionary) lookup, sentinel-checked | 6 |
| "…flash a warning LED without freezing the rest of the system" | Non-blocking `millis()` timing, not `delay()` | 4 |

This is exactly the shape Example 7 below builds in code — the skill being practiced here is stopping to make this table in your head (or on paper) *before* writing a single line, rather than starting to code from the top of the sentence and improvising the rest as you go.

## Code Examples

Examples 1-6 below each isolate **one** week's concept in a short, runnable sketch — a quick individual refresher, all drawn from a single running scenario (a smart parcel-locker bank) so they read as one progressively-built system rather than six unrelated snippets. Example 7 is the full integrated build that combines everything at once, exactly as a written assessment would ask you to.

### Example 1 — Parameterised Locker-Release Function 

```cpp
#include <Arduino.h>

const int lockerAPin = 4;
const int lockerBPin = 5;

// Activates a locker solenoid for the specified time
void open_locker(int pin, unsigned long openTime) {
  digitalWrite(pin, HIGH);       // Energise the latch solenoid
  delay(openTime);               // Keep the locker unlocked
  digitalWrite(pin, LOW);        // De-energise and lock it again
}

void setup() {
  Serial.begin(115200);

  pinMode(lockerAPin, OUTPUT);
  pinMode(lockerBPin, OUTPUT);

  // Ensure both solenoids are off at startup
  digitalWrite(lockerAPin, LOW);
  digitalWrite(lockerBPin, LOW);

  Serial.println("Locker system ready");
}

void loop() {
  Serial.println("Opening Locker A");
  open_locker(lockerAPin, 1000);

  Serial.println("Opening Locker B");
  open_locker(lockerBPin, 2000);

  Serial.println("Waiting 5 seconds");
  delay(5000);
}
```
>[!NOTE]
>
> Open the Wokwi link :  https://wokwi.com/projects/474460961380768769
> 


**Code Walkthrough:**

| Step | Code | What it does |
|---|---|---|
| 1 | `const int lockerAPin = 4; const int lockerBPin = 5;` | Fixed pin numbers for the two lockers' latch solenoids, declared `const` since they never change. |
| 2 | `void open_locker(int pin, unsigned long openTime) {` | Defines one reusable function taking the pin *and* the release duration as parameters. |
| 3 | `digitalWrite(pin, HIGH); delay(openTime); digitalWrite(pin, LOW);` | Energises whichever latch `pin` refers to, holds it for `openTime` ms, then releases it — the body never mentions Locker A or B by name. |
| 4 | `pinMode(lockerAPin, OUTPUT); pinMode(lockerBPin, OUTPUT);` | Configures both physical pins as outputs once, in `setup()`. |
| 5 | `open_locker(lockerAPin, 1000);` | Calls the one function with Locker A's pin and a 1 s duration. |
| 6 | `open_locker(lockerBPin, 2000);` | Same function, called again with Locker B's pin and a different, longer duration — no code was copied to do this. |
| 7 | `delay(5000);` | Pauses 5 s before `loop()` repeats the whole two-locker cycle. |

### Example 2 — Debounced Locker-Select Button (Week 3 Recap)

```cpp
const int selectButtonPin = 4;
const int lockerALed = 5;
const int lockerBLed = 6;

bool lastButtonReading = HIGH;
bool buttonState = HIGH;
unsigned long debounceStart = 0;
const long DEBOUNCE_TIME = 40;

bool lockerBSelected = false;

void setup() {
  pinMode(selectButtonPin, INPUT_PULLUP);
  pinMode(lockerALed, OUTPUT);
  pinMode(lockerBLed, OUTPUT);
}

void loop() {
  bool reading = digitalRead(selectButtonPin);
  if (reading != lastButtonReading) debounceStart = millis();
  if (millis() - debounceStart > DEBOUNCE_TIME && reading != buttonState) {
    buttonState = reading;
    if (buttonState == LOW) lockerBSelected = !lockerBSelected;   // validated fresh press only
  }
  lastButtonReading = reading;

  if (lockerBSelected) {
    digitalWrite(lockerALed, LOW);
    digitalWrite(lockerBLed, HIGH);
  } else {
    digitalWrite(lockerALed, HIGH);
    digitalWrite(lockerBLed, LOW);
  }
}
```
>[!NOTE]
> Open the Wokwi link: https://wokwi.com/projects/474462085306563585
#### How it works
The settle-time debounce (`lastButtonReading`/`buttonState`/`debounceStart`) makes sure exactly one selection change happens per physical press, however many raw transitions the mechanical bounce produces; `if`/`else` selection then decides which locker's LED reflects the current choice.

**Code Walkthrough:**

| Step | Code | What it does |
|---|---|---|
| 1 | `bool reading = digitalRead(selectButtonPin);` | Reads the raw button state fresh at the top of every `loop()` pass. |
| 2 | `if (reading != lastButtonReading) debounceStart = millis();` | Restarts the settle-time clock every time the raw signal moves at all — including mid-bounce. |
| 3 | `if (millis() - debounceStart > DEBOUNCE_TIME && reading != buttonState)` | Only proceeds once the raw signal has held steady past `DEBOUNCE_TIME`, and only if that settled value is actually new. |
| 4 | `buttonState = reading; if (buttonState == LOW) lockerBSelected = !lockerBSelected;` | Commits the validated reading, then flips the selection exactly once per real press (`INPUT_PULLUP`, so a press reads `LOW`). |
| 5 | `lastButtonReading = reading;` | Runs unconditionally on every pass, so the next pass's comparison in Step 2 is against the correct prior raw value. |
| 6 | `if (lockerBSelected) { ... } else { ... }` | Selection decides which locker's LED is lit based on the current (debounced) choice — Locker B's pair when `true`, Locker A's when `false`. |

### Example 3 — Boolean-Guarded, Non-blocking Motion Confirmation

```cpp
#include <Arduino.h>

const int confirmButtonPin = 4;  // Courier confirmation button
const int pirSensorPin = 3;      // PIR OUT pin
const int confirmLedPin = 10;    // Confirmation LED

bool confirmActive = false;
unsigned long confirmStart = 0;

const unsigned long CONFIRM_DURATION = 2000;

// Prevents repeated confirmation while the button is held
bool lastConfirmPressed = false;

void setup() {
  Serial.begin(115200);

  pinMode(confirmButtonPin, INPUT_PULLUP);
  pinMode(pirSensorPin, INPUT);
  pinMode(confirmLedPin, OUTPUT);

  digitalWrite(confirmLedPin, LOW);

  Serial.println("Locker drop-off system ready");
}

void loop() {
  // INPUT_PULLUP means LOW when the button is pressed
  bool confirmPressed =
      digitalRead(confirmButtonPin) == LOW;

  // HIGH means the PIR detected movement
  bool motionDetected =
      digitalRead(pirSensorPin) == HIGH;

  // Detect a new press instead of a held button
  bool newConfirmPress =
      confirmPressed && !lastConfirmPressed;

  // Confirm only if movement is detected
  if (newConfirmPress && motionDetected && !confirmActive) {
    confirmActive = true;
    confirmStart = millis();

    digitalWrite(confirmLedPin, HIGH);
    Serial.println("Drop-off confirmed: movement detected");
  }

  // Reject confirmation when no movement is detected
  if (newConfirmPress && !motionDetected) {
    Serial.println("Cannot confirm: no movement detected");
  }

  // Turn off the LED after two seconds
  if (confirmActive &&
      millis() - confirmStart >= CONFIRM_DURATION) {

    digitalWrite(confirmLedPin, LOW);
    confirmActive = false;

    Serial.println("Confirmation complete");
  }

  lastConfirmPressed = confirmPressed;

  // Other sensors can still be checked because delay() is not used
}
```
>[!NOTE]
> Open the Wokwi link: https://wokwi.com/projects/474463588556916737
> 
#### How it works
`&&` requires both a genuinely *new* button press **and** motion being detected before confirmation starts — a plain `if (confirmPressed)` alone would re-trigger on every pass the button is held, and would confirm a drop-off even with no motion present. The confirmation LED's 2-second hold is timed against `millis()`, never `delay()`, so `loop()` keeps running every other check while it's lit.

**Code Walkthrough:**

| Step | Code | What it does |
|---|---|---|
| 1 | `bool confirmPressed = digitalRead(confirmButtonPin) == LOW;` | Converts the raw button pin into a plain-English boolean (`INPUT_PULLUP`, so a press reads `LOW`). |
| 2 | `bool motionDetected = digitalRead(pirSensorPin) == HIGH;` | Same conversion for the PIR sensor — `HIGH` means movement was detected. |
| 3 | `bool newConfirmPress = confirmPressed && !lastConfirmPressed;` | Edge detection on the button: true only on the pass it *becomes* pressed, not on every pass it's held down. |
| 4 | `if (newConfirmPress && motionDetected && !confirmActive) {` | `&&` requires all three at once: a fresh press, motion detected, and not already mid-confirmation — the last term stops the block re-triggering itself every pass while the LED is lit. |
| 5 | `confirmActive = true; confirmStart = millis(); digitalWrite(confirmLedPin, HIGH);` | Starts the confirmation: records the start time and lights the LED, without pausing execution. |
| 6 | `if (newConfirmPress && !motionDetected) { ... }` | A fresh press with no motion detected is rejected outright rather than silently ignored. |
| 7 | `if (confirmActive && millis() - confirmStart >= CONFIRM_DURATION) {` | Checks elapsed time against `millis()` on every pass — never `delay()` — so this only fires once the hold period has actually passed. |
| 8 | `digitalWrite(confirmLedPin, LOW); confirmActive = false;` | Ends the confirmation: LED off, ready to be triggered again by a fresh press. |
| 9 | `lastConfirmPressed = confirmPressed;` | Runs unconditionally on every pass, outside every `if`, so Step 3's edge-detection comparison is correct on the next pass. |

### Example 4 - Single-Pass Busiest/Quietest Locker Scan 

```cpp
#include <Arduino.h>

const int NUM_LOCKERS = 5;

// Usage data for lockers 1–5
int usageCount[NUM_LOCKERS] = {12, 4, 9, 4, 20};

void report_usage() {
  // Store array indexes, not usage values
  int busiest = 0;
  int quietest = 0;

  for (int i = 1; i < NUM_LOCKERS; i++) {
    if (usageCount[i] > usageCount[busiest]) {
      busiest = i;
    }

    if (usageCount[i] < usageCount[quietest]) {
      quietest = i;
    }
  }

  // Add 1 so locker numbers are displayed as 1–5
  Serial.print("Busiest locker: ");
  Serial.print(busiest + 1);
  Serial.print(" with ");
  Serial.print(usageCount[busiest]);
  Serial.println(" uses");

  Serial.print("Quietest locker: ");
  Serial.print(quietest + 1);
  Serial.print(" with ");
  Serial.print(usageCount[quietest]);
  Serial.println(" uses");
}

void setup() {
  Serial.begin(115200);
  report_usage();
}

void loop() {
  // No repeated action required
}
```

>[!NOTE]
> Open the Wokwi link: https://wokwi.com/projects/473995555083265025
> 
#### How it works
This is a variant of the min/max scan above: instead of tracking the extreme *value*, `busiest`/`quietest` track the extreme value's *index*, still seeded from index `0` rather than a hard-coded guess — the same "never initialise from a made-up constant" rule, just applied one level of indirection further.

**Code Walkthrough:**

| Step | Code | What it does |
|---|---|---|
| 1 | `int usageCount[NUM_LOCKERS] = {12, 4, 9, 4, 20};` | The array being scanned — one usage count per locker. |
| 2 | `int busiest = 0, quietest = 0;` | Seeds both winning indices at `0` (index `0`, not a guessed value), so the scan is correct even if every count were identical or all lower than some hard-coded assumption. |
| 3 | `for (int i = 1; i < NUM_LOCKERS; i++) {` | Starts at index `1`, since index `0` is already the seed — comparing it against itself would be harmless but pointless. |
| 4 | `if (usageCount[i] > usageCount[busiest]) busiest = i;` | Updates `busiest` to the *index* `i` whenever a higher count is found, not the count itself. |
| 5 | `if (usageCount[i] < usageCount[quietest]) quietest = i;` | Same single-pass scan, tracking the lowest count's index at the same time — no second loop needed. |
| 6 | `Serial.print(...); Serial.println(busiest);` / `quietest` | Reports both results after the one pass over the array has finished. |

### Example 5 — Struct Log With Dictionary Weight-Limit Lookup 

```cpp
#include <Arduino.h>

// Stores information about each accepted parcel
struct Parcel {
  unsigned long timestamp;
  String size;
  int weightGrams;
};

// Stores the maximum weight allowed for each parcel size
struct SizeLimit {
  String size;
  int maxWeightGrams;
};

const int MAX_PARCELS = 10;

Parcel parcels[MAX_PARCELS];
int parcel_count = 0;

const int NUM_SIZES = 2;

SizeLimit sizeLimits[NUM_SIZES] = {
  {"small", 500},
  {"large", 5000}
};

// Returns the maximum weight for the selected parcel size
int get_max_weight(String size) {
  for (int i = 0; i < NUM_SIZES; i++) {
    if (sizeLimits[i].size == size) {
      return sizeLimits[i].maxWeightGrams;
    }
  }

  return -1;  // The parcel size was not found
}

// Checks and records a parcel
void log_parcel(String size, int weightGrams) {
  // Check whether the array is full
  if (parcel_count >= MAX_PARCELS) {
    Serial.println("Rejected: parcel storage is full");
    return;
  }

  int limit = get_max_weight(size);

  // Reject an unknown parcel size
  if (limit == -1) {
    Serial.println("Rejected: unknown parcel size");
    return;
  }

  // Reject parcels exceeding the selected size's limit
  if (weightGrams > limit) {
    Serial.print("Rejected: ");
    Serial.print(size);
    Serial.println(" parcel is over the weight limit");
    return;
  }

  // Store the accepted parcel
  parcels[parcel_count] = {
    millis(),
    size,
    weightGrams
  };

  parcel_count++;

  Serial.print("Accepted: ");
  Serial.print(size);
  Serial.print(" parcel, ");
  Serial.print(weightGrams);
  Serial.println(" g");
}

// Prints all accepted parcel records
void print_parcels() {
  Serial.println();
  Serial.println("Accepted parcel records:");

  if (parcel_count == 0) {
    Serial.println("No parcels recorded");
    return;
  }

  for (int i = 0; i < parcel_count; i++) {
    Serial.print("Parcel ");
    Serial.print(i + 1);

    Serial.print(" | Time: ");
    Serial.print(parcels[i].timestamp);
    Serial.print(" ms");

    Serial.print(" | Size: ");
    Serial.print(parcels[i].size);

    Serial.print(" | Weight: ");
    Serial.print(parcels[i].weightGrams);
    Serial.println(" g");
  }
}

void setup() {
  Serial.begin(115200);

  Serial.println("Smart locker parcel system");
  Serial.println("--------------------------");

  log_parcel("small", 400); // Accepted: below the small-parcel limit
  log_parcel("small", 700); // Rejected: exceeds the 500 g small-parcel limit
  log_parcel("large", 4200);   // Accepted: below the large-parcel limit
  log_parcel("large", 6000); // Rejected: exceeds the 5000 g large-parcel limit
  log_parcel("medium", 1000); // Rejected: medium is not a configured size

  print_parcels();
}

void loop() {
  // Nothing repeats in this demonstration
}

```

>[!NOTE]
> Wokwi link: https://wokwi.com/projects/473995555083265025
> 
#### How it works
`get_max_weight()` returns the `-1` sentinel for an unrecognised size, and `log_parcel()` checks `limit >= 0` before ever comparing it to a real weight — skipping that check would let an unconfigured size either reject every parcel or accept every parcel, depending on which comparison operator was used.

**Code Walkthrough:**

| Section | What it does |
|---|---|
| `struct Parcel` | Groups one parcel's `timestamp` and `size` into a single logged record. |
| `struct SizeLimit` | Groups a size name (the key) with its `maxWeightGrams` (the value) — one key-value pair. |
| `parcels[]` / `parcel_count` | Fixed-capacity array of `Parcel` structs, and how many of its slots are currently filled. |
| `sizeLimits[]` | The dictionary itself: a 2-entry array of `SizeLimit` records for "small" and "large". |
| `get_max_weight()` | Linear search — loops through `sizeLimits[]` comparing `size` against the requested name; returns the matching limit, or the sentinel `-1` if nothing matches. |
| `log_parcel()` — lookup | Calls `get_max_weight(size)` and checks `limit >= 0` *before* trusting it as a real weight limit. |
| `log_parcel()` — reject | If the sentinel check passes and the parcel is over the size's limit, prints a rejection message and returns without logging anything. |
| `log_parcel()` — append | Otherwise writes `{millis(), size}` into the next free slot and increments `parcel_count`, exactly the struct-array append pattern from the Key Concepts section above. |

### Example 6 — Switch/Case Locker-Bay Mode Selector 

A debounced Mode button cycles a locker bay through three states — Available, Occupied, Maintenance — and `switch`/`case` picks which status LED lights up for the current mode, instead of a chain of `else if`s repeating `if (lockerMode == ...)` three times.

```cpp
const int modeButtonPin = 4;
const int availableLed = 5;
const int occupiedLed = 6;
const int maintenanceLed = 7;

int lockerMode = 0;    // 0 = Available, 1 = Occupied, 2 = Maintenance
bool lastButtonReading = HIGH;
bool buttonState = HIGH;
unsigned long debounceStart = 0;
const long DEBOUNCE_TIME = 40;

void setup() {
  pinMode(modeButtonPin, INPUT_PULLUP);
  pinMode(availableLed, OUTPUT);
  pinMode(occupiedLed, OUTPUT);
  pinMode(maintenanceLed, OUTPUT);
}

void loop() {
  bool reading = digitalRead(modeButtonPin);
  if (reading != lastButtonReading) debounceStart = millis();
  if (millis() - debounceStart > DEBOUNCE_TIME && reading != buttonState) {
    buttonState = reading;
    if (buttonState == LOW) {
      lockerMode = lockerMode + 1;
      if (lockerMode > 2) lockerMode = 0;   // wrap back to Available
    }
  }
  lastButtonReading = reading;

  switch (lockerMode) {
    case 0:
      digitalWrite(availableLed, HIGH);
      digitalWrite(occupiedLed, LOW);
      digitalWrite(maintenanceLed, LOW);
      break;

    case 1:
      digitalWrite(availableLed, LOW);
      digitalWrite(occupiedLed, HIGH);
      digitalWrite(maintenanceLed, LOW);
      break;

    case 2:
      digitalWrite(availableLed, LOW);
      digitalWrite(occupiedLed, LOW);
      digitalWrite(maintenanceLed, HIGH);
      break;

    default:
      digitalWrite(availableLed, LOW);
      digitalWrite(occupiedLed, LOW);
      digitalWrite(maintenanceLed, LOW);
      break;
  }
}
```
>[!NOTE]
> Open the Wokwi link: https://wokwi.com/projects/474470208794608641
> 
**Code walkthrough:**

| Step | Code | What it does |
|---|---|---|
| 1 | `bool reading = digitalRead(modeButtonPin);` | Reads the Mode button fresh at the top of every `loop()` pass. |
| 2 | `if (millis() - debounceStart > DEBOUNCE_TIME && reading != buttonState)` | Settle-time debounce — the same pattern as Example 2 — only trusts the reading once it has held steady. |
| 3 | `lockerMode = lockerMode + 1; if (lockerMode > 2) lockerMode = 0;` | Advances the mode on each validated press and wraps back to `0` after the last mode (`2`). |
| 4 | `switch (lockerMode)` | Starts the select-case block — compares `lockerMode` against each `case` in order, instead of a repeated `if (lockerMode == ...)` chain. |
| 5 | `case 0: ... break;` | Runs only when `lockerMode == 0` (Available LED on, others off); `break` stops execution "falling through" into `case 1`. |
| 6 | `case 1:` / `case 2:` | Same pattern for Occupied and Maintenance — exactly one `case` runs per loop. |
| 7 | `default: ... break;` | Safety net — if `lockerMode` were ever something unexpected, every LED turns off instead of leaving a stale mode lit. |
| 8 | `lastButtonReading = reading;` | Runs unconditionally, outside every `if`, so the debounce state is correct on the next pass. |

### Example 7 - Integrated Revision Build: Campus Bike-Share Docking Station
This single example deliberately touches every concept above: a debounced Return button appends a struct `Checkin` to a sorted log (Week 3, 5, 6); a dictionary-style battery-threshold table is looked up by name with a checked sentinel to drive a non-blocking low-battery alert (Week 4, 6); a dock sensor's edge is detected and reported independently (Week 4); and parameterised functions (`start_alert()`/`update_alert()`, `log_checkin()`, `get_min_battery()`) keep `loop()` itself short (Week 2).

```cpp
#include <Arduino.h>

// Stores one bike check-in
struct Checkin {
  unsigned long timestamp;  // Time since the simulation started
  int batteryLevel;         // Simulated battery percentage
};

// Stores the minimum battery level for a bike type
struct Threshold {
  String bikeType;
  int minBattery;
};

// Wokwi component pins
const int batteryPin = 1;       // Potentiometer SIG
const int dockSensorPin = 3;    // Slide switch common pin
const int returnButtonPin = 4;  // Push button
const int alertLedPin = 10;     // Red alert LED

// Check-in record storage
const int MAX_CHECKINS = 10;
Checkin checkins[MAX_CHECKINS];
int checkin_count = 0;

// Battery threshold table
const int NUM_THRESHOLDS = 1;

Threshold thresholds[NUM_THRESHOLDS] = {
  {"standard", 20}  // Minimum battery level is 20%
};

// Button-debounce variables
bool lastButtonReading = HIGH;
bool buttonState = HIGH;

unsigned long debounceStart = 0;
const unsigned long DEBOUNCE_TIME = 40;

// Previous simulated dock state
bool lastDockState = false;

// Non-blocking LED alert
bool alertActive = false;
unsigned long alertStart = 0;
const unsigned long ALERT_DURATION = 300;

// Finds the minimum battery level for a bike type
int get_min_battery(String bikeType) {
  for (int i = 0; i < NUM_THRESHOLDS; i++) {
    if (thresholds[i].bikeType == bikeType) {
      return thresholds[i].minBattery;
    }
  }

  return -1;  // Bike type not found
}

// Adds a bike check-in to the array
void log_checkin(int batteryLevel) {
  if (checkin_count >= MAX_CHECKINS) {
    Serial.println("Check-in log is full");
    return;
  }

  checkins[checkin_count] = {
    millis(),
    batteryLevel
  };

  checkin_count++;

  // Keep the records ordered by check-in time
  int i = checkin_count - 1;

  while (i > 0 &&
         checkins[i - 1].timestamp > checkins[i].timestamp) {

    Checkin temp = checkins[i];
    checkins[i] = checkins[i - 1];
    checkins[i - 1] = temp;

    i--;
  }

  Serial.print("Bike checked in | Battery: ");
  Serial.print(batteryLevel);
  Serial.println("%");
}

// Turns on the alert LED and records its start time
void start_alert() {
  digitalWrite(alertLedPin, HIGH);

  alertActive = true;
  alertStart = millis();
}

// Turns off the alert LED after 300 milliseconds
void update_alert() {
  if (alertActive &&
      millis() - alertStart >= ALERT_DURATION) {

    digitalWrite(alertLedPin, LOW);
    alertActive = false;
  }
}

void setup() {
  Serial.begin(115200);

  // Button is connected between GPIO 4 and GND
  pinMode(returnButtonPin, INPUT_PULLUP);

  // Slide switch supplies HIGH or LOW
  pinMode(dockSensorPin, INPUT);

  pinMode(alertLedPin, OUTPUT);
  digitalWrite(alertLedPin, LOW);

  Serial.println("Wokwi bike dock system ready");
  Serial.println("Move the dock switch, then press Return.");
}

void loop() {
  // Read the return button
  bool reading = digitalRead(returnButtonPin);

  // Restart debounce timing when the reading changes
  if (reading != lastButtonReading) {
    debounceStart = millis();
  }

  // Accept the button state after it remains stable
  if ((millis() - debounceStart >= DEBOUNCE_TIME) &&
      (reading != buttonState)) {

    buttonState = reading;

    // LOW means the button was pressed
    if (buttonState == LOW) {
      bool docked =
          digitalRead(dockSensorPin) == HIGH;

      // Do not check in a bike unless the switch says it is docked
      if (!docked) {
        Serial.println("Check-in rejected: bike is not docked");
      } else {
        // Convert ESP32 ADC range to a percentage
        int batteryLevel =
            map(analogRead(batteryPin), 0, 4095, 0, 100);

        log_checkin(batteryLevel);

        int minBattery =
            get_min_battery("standard");

        if (minBattery >= 0 &&
            batteryLevel < minBattery) {

          Serial.println("Warning: battery is below 20%");
          start_alert();
        } else {
          Serial.println("Bike return accepted");
        }
      }
    }
  }

  lastButtonReading = reading;

  // Read the current simulated dock state
  bool dockNow =
      digitalRead(dockSensorPin) == HIGH;

  // Report only when the dock changes state
  if (dockNow != lastDockState) {
    if (dockNow) {
      Serial.println("Dock sensor: bike detected");
    } else {
      Serial.println("Dock sensor: dock empty");
    }

    lastDockState = dockNow;
  }

  // Update the LED timer without using delay()
  update_alert();
}
```

>[!NOTE]
> Open the Wokwi link: https://wokwi.com/projects/474471052779162625
> 

#### How it works
Every concept sits inside its own named function — `get_min_battery()`, `log_checkin()`, `start_alert()`/`update_alert()` — so `loop()` reads as a short list of *what* happens, not *how*. Nothing in `loop()` blocks: the button debounce, the dock-sensor edge-check, and the alert timer are all compared against `millis()` on every single pass, so an active alert never makes the program miss another input.

**Code Walkthrough:**

| Section | What it does |
|---|---|
| `struct Checkin` / `struct Threshold` | The sorted check-in log record and the battery-threshold dictionary record, defined once and reused throughout `loop()`. |
| `checkins[]` / `checkin_count` | Fixed-capacity struct array holding the sorted log, and how many slots are filled. |
| `thresholds[]` | The dictionary: one `{"standard", 20}` entry mapping a bike type to its minimum acceptable battery level. |
| `lastButtonReading` / `buttonState` / `debounceStart` / `DEBOUNCE_TIME` | The Week 3 debounce state for the Return button — identical pattern to Examples 2 and 6. |
| `lastDockState` | Edge-detection state for the dock sensor, used to report a change only once per transition, not on every pass it holds that value. |
| `alertActive` / `alertStart` / `ALERT_DURATION` | Non-blocking timer state for the low-battery alert LED. |
| `get_min_battery()` | Linear-search dictionary lookup by bike type; returns the sentinel `-1` if the type isn't in `thresholds[]`. |
| `log_checkin()` — append & guard | `if (checkin_count >= MAX_CHECKINS) return;` then appends `{millis(), batteryLevel}` — the same fixed-array overflow guard used since Week 5. |
| `log_checkin()` — insertion sort | The `while` loop bubbles the just-appended record left by timestamp, keeping `checkins[]` sorted after every insert. |
| `start_alert()` / `update_alert()` | A parameterised, non-blocking on/off pair: `start_alert()` turns the LED on and records the start time; `update_alert()` turns it off once `ALERT_DURATION` has elapsed, checked against `millis()`. |
| `loop()` — debounced button block | Reads and debounces the Return button exactly as in Example 2; on a validated press, reads the battery, logs the check-in, looks up `"standard"`'s minimum, and starts an alert only if the sentinel check passes **and** the reading is too low. |
| `loop()` — dock sensor block | `dockNow != lastDockState` fires only on the pass the dock reading actually changes (Week 4 edge detection), reporting docked/undocked once per transition instead of on every pass it holds that value. |
| `loop()` — `update_alert(...)` | Called unconditionally on every pass, so an active alert is always checked for expiry regardless of which trigger started it. |
---
---
## Vocabulary
| Term | Definition |
|---|---|
| Variable | A named container that stores a value in memory. |
| Function | A named, reusable block of code, optionally with parameters. |
| Debounce | Waiting for a settle time before trusting a changed digital reading, so one physical press registers once. |
| Selection | Branching (`if`/`else`) so a program's path depends on a condition, not always the same sequence. |
| `switch` / Select Case | A multi-branch statement comparing one variable against a list of specific values (`case`s) — an alternative to an `if`/`else if` chain when checking a single variable for equality. |
| `break` | Inside a `switch`, stops execution from "falling through" into the next `case` once the current one has run. |
| `default` | The optional `case` in a `switch` that runs when the variable matched none of the listed values — equivalent to a chain's final `else`. |
| Boolean operator | `&&` (AND), `\|\|` (OR), `!` (NOT) — combine or invert conditions. |
| Non-blocking timing | Comparing elapsed `millis()` time on every pass instead of pausing with `delay()`, so other inputs stay responsive. |
| Array | A fixed-size, indexed collection of same-typed values. |
| Parallel arrays | Two or more arrays that must always be updated together to describe one thing — the bug class structs remove. |
| Struct | A user-defined record type grouping related fields under one name, accessed by dot notation. |
| Dictionary / key-value lookup | A name-keyed lookup, simulated in C++ with a small key-value struct array and a linear search. |
| Sentinel value | A specific return value (e.g. `-1`) signalling "not found," which every caller must check before trusting the result. |
| Insertion sort | Bubbling a single newly-inserted record into its correct sorted position, rather than re-sorting everything. |






