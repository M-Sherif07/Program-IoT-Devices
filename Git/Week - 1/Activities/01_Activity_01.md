# Activity 1 - IoT Programming Introduction (Week 1)



**Goal:** Practice declaring variables, choosing data types, writing comments, and using the `setup()` / `loop()` structure to build and upload a working device start-up + LED blink program.



## Before You Start

- [ ] Wokwi project open with the correct board selected
- [ ] LED wired in the diagram (with a resistor) to a GPIO pin — note which pin, some boards don't simulate `LED_BUILTIN`
- [ ] Serial Monitor baud rate will match `Serial.begin()` in your code

---

## Task 1 - Build the Base Program 

Recreate the base start-up + blink program yourself, without copy-pasting from the lesson notes.

**Open this Wokwi simulation:** https://wokwi.com/projects/470567521110438913

1. Declare two variables:
   - `ledPin` (type `int`) — use `pin 13` if your board simulates one, otherwise set it to whichever GPIO you wired the LED to in the diagram
   - `deviceName` (type `String`), set to any name you choose
2. In `setup()`:
   - Start Serial communication at `115200`
   - Set `ledPin` as an `OUTPUT`
   - Print a start-up message that includes `deviceName`
3. In `loop()`:
   - Turn the LED on, print `"LED: ON"`, wait 1000 ms
   - Turn the LED off, print `"LED: OFF"`, wait 1000 ms
4. Upload the code and confirm the LED blinks in time with the Serial Monitor output.

**Check yourself:**
- [ ] Program compiles with no errors
- [ ] Serial Monitor shows the start-up message once
- [ ] LED blinks in sync with `"LED: ON"` / `"LED: OFF"` messages

---

## Task 2 - Custom Device Start-Up Sequence

**Open this Wokwi simulation:** https://wokwi.com/projects/470564412054187009 

Modify your Task 1 program to include:

1. At least **three variables** — one `int`, one `float`, one `String`.
2. Print all three values in `setup()` as part of a formatted start-up message, for example:

   ```
   === Smart Sensor Node ===
   Device: TempSensor_01
   Pin: 13
   Threshold: 28.5
   Status: READY
   =========================
   ```

3. In `loop()`, blink the LED **three times quickly** (200 ms on / 200 ms off), then pause for **2 seconds**, then repeat.

---

## Task 3 - Blink Patterns (Extension)

**Open this Wokwi simulation:** https://wokwi.com/projects/470569149081367553

Write a program that creates a **"heartbeat" pattern**: two quick blinks followed by a long pause.

Requirements:
- Use clearly named variables for every delay value, e.g.:
  ```cpp
  int shortBlink = 100;
  int longPause = 1500;
  ```
- Do not hardcode delay numbers directly inside `loop()` — always reference your variables.

---

## Task 4 - Annotated Code (Extension)

Take your Task 1 or Task 2 program and add a comment above **every line** explaining what it does, in your own words.

- Focus on explaining **why** a line exists, not just restating what it says.
- Use the comment best practices from the lesson (explain why, comment formulas, label pins, don't over-comment obvious lines).

---

## Task 5 - Find and Fix the Bugs (Extension)

Each bug set below is a broken sketch containing **three separate mistakes** covered in the lesson. For each set: copy it into the IDE, use the **commenting-out technique** to isolate and fix each bug one at a time (don't fix everything at once), and write a one-line comment above each fixed line explaining what was wrong.

### Bug Set A 
```cpp
const int LED_PIN = 13;

void Setup() {
  Serial.begin(115200);
  LED_PIN = 9;
  Serial.println("Ready");
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
}
```


**Check yourself:**
- [ ] Program compiles with no errors
- [ ] LED blinks as expected
- [ ] Each fix has a comment explaining the original mistake

---

### Bug Set B 

```cpp
int 2ledPin = LED_BUILTIN;
float temperature = "23.5";
String device name = "Node1";

void setup() {
  Serial.begin(9600);
  pinMode(2ledPin, OUTPUT);
}

void loop() {
  digitalWrite(2ledPin, HIGH);
  delay(500);
  digitalWrite(2ledPin, LOW);
  delay(500);
}
```



**Check yourself:**
- [ ] Program compiles with no errors
- [ ] All variable names follow the naming rules
- [ ] Each fix has a comment explaining the original mistake

---

### Bug Set C

```cpp
int ledPin = LED_BUILTIN;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.println("Starting...")
}

void loop() {
  digitalWrite(ledPin, HIGH);  // Turn LED off
  delay(1000);
  digitalWrite(ledPin, LOW);
  delay(1000);
}
```


**Check yourself:**
- [ ] Program compiles with no errors
- [ ] Serial Monitor shows "Starting..." on upload
- [ ] Every comment accurately describes the line below it


---

## Questions

Answer these in your own words before moving on:

1. What data type would you use to store a decimal temperature reading? Why?
2. What is the difference between `Serial.print()` and `Serial.println()`?
3. What happens if you remove `Serial.begin(115200)` from `setup()`?
4. What does `void` mean at the start of `setup()` and `loop()`?
5. Why is it useful to use a variable (or constant) for a pin number instead of typing `13` directly in the code?
6. What is the difference between a `variable` and a `constant`? Give one example of when you'd use each.

---



## Self-Check Before Submitting

- [ ] Program compiles and uploads without errors
- [ ] Serial Monitor shows the correct start-up message
- [ ] LED blinks in sync with Serial Monitor output
- [ ] Variables are declared with appropriate, descriptive data types and names
- [ ] Constants (if used) are in `UPPER_CASE`
- [ ] Comments explain *why*, not just *what*
- [ ] I can explain the purpose of `setup()` and `loop()` out loud
