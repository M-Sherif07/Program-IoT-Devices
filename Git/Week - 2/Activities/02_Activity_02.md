# Activity 2 - Arduino Digital Output and Functions (Week 2)


## Task 1 - Buzzer SOS Using a Function

**Scenario:**

A hiking safety company is developing a small emergency signalling device for people travelling in remote areas. When activated, the device must repeatedly sound the international SOS distress pattern using an active buzzer.

Your task is to program an ESP32-S3 to produce three short beeps, followed by three long beeps, and then three short beeps. After completing the SOS signal, the device must wait two seconds before repeating it.

**Components:**

ESP32-S3, 1 × active buzzer, jumper wires.
**Wiring:** buzzer (+) → GPIO8, buzzer (−) → GND.

Create a reusable function named `beep(int onTime)`. The `onTime` parameter controls how long the buzzer remains on. The function must turn the buzzer off and wait 200 milliseconds after every beep. Use the same function for both short and long beeps.

Wokwi https://wokwi.com/projects/471515514429761537

**Check yourself:**
- [ ] `beep()` is reused for both short and long beeps — no duplicated `digitalWrite`/`delay` blocks
- [ ] The SOS pattern is clearly recognisable (short-short-short, long-long-long, short-short-short)
- [ ] There is a 2-second pause between each full SOS sequence

---

## Task 2 - RGB Colour Cycle Using a Function

**Scenario:**

An event organiser needs a small decorative light for a concert stage model. The light must cycle through red, green, and blue, with only one colour illuminated at a time. Each colour should remain on for a specified duration before the next colour appears.

Your task is to program an ESP32-S3 to control a common-cathode RGB LED.

**Components:**

ESP32-S3, 1 × common-cathode RGB LED, 3 × 220 Ω resistors, jumper wires, breadboard.

| RGB LED Pin | ESP32-S3 GPIO |
|-------------|-------------|
| Red (+) | GPIO14 |
| Green (+) | GPIO18 |
| Blue (+) | GPIO17 |
| Common (−) | GND |

Create a reusable function named `showColor(int pin, int onTime)`. The `pin` parameter selects the colour, while `onTime` determines how long it remains illuminated. Use this function in `loop()` to display red, green, and blue in sequence.

**Challenge:** As an additional challenge, create a second function named `showWhite(int onTime)`. This function must turn on all three colours simultaneously to produce a white flash after each complete colour cycle.

Wokwi https://wokwi.com/projects/471515878453487617

**Check yourself:**
- [ ] `showColor()` is used for all three colours instead of writing `digitalWrite`/`delay` three separate times
- [ ] Only one colour is on at a time during the colour cycle
- [ ] (Challenge) `showWhite()` turns all three colours on at once

---

## Task 3 - Function With a Return Value

**Scenario:**

A smart home company is building a simple app to remotely control a porch light. Whenever the app sends a command to turn the light on or off, the device needs to confirm back what state the light actually ended up in, so the app can keep its on-screen switch in sync with the real light.

**Components:**

ESP32-S3, 1 × LED, 1 × 220 Ω resistor, jumper wires, breadboard.
**Wiring:** LED anode (+) → 220 Ω resistor → GPIO13, LED cathode (−) → GND.

Write a function `bool controlLED(bool turnOn)` that sets `ledPin` (`= 13`) HIGH or LOW based on the `turnOn` parameter, and returns the new state.

In `loop()`, call it to turn the LED on, print the returned state to Serial, wait 1 second, then call it to turn the LED off and print the returned state again.

Wokwi https://wokwi.com/projects/471516108090616833

**Check yourself:**
- [ ] `controlLED()` returns a `bool`, not `void`
- [ ] The Serial Monitor prints `1` when the LED is on and `0` when it is off
- [ ] The same function handles both turning the LED on and off

---

## Task 4 - LED Chase Sequence Using a Function

**Scenario:**

A school is preparing for a celebration and needs a miniature decorative lighting display. Your task is to program several LEDs to turn on and off one after another, creating a "chasing light" effect.

The sequence should move from the first LED to the last LED. After reaching the final LED, it should repeat continuously. Use a function to control the sequence instead of placing all the instructions directly inside `loop()`.

**Components:**

ESP32-S3, at least 4 × ordinary LEDs, 220 Ω resistors, jumper wires, breadboard. Connect the LEDs to GPIOs 4, 5, 6, and 7 (each with its own 220 Ω resistor).

**Program requirements:**
- Use at least four LEDs.
- Turn on only one LED at a time.
- Use a function with parameters to control an LED: `void lightLED(int ledPin, int delayTime)`
  - The `ledPin` parameter selects the LED.
  - The `delayTime` parameter controls the speed.
- Call the function for each LED from `loop()`, moving from the first LED to the last, then repeating.

> **Challenge (optional):** Add a buzzer sound or an OLED/LCD message that plays or updates each time the sequence reaches the last LED.

Wokwi https://wokwi.com/projects/471516524377458689

**Check yourself:**
- [ ] At least four LEDs are wired and controlled
- [ ] Only one LED is lit at a time
- [ ] `lightLED()` is reused for every LED — no repeated `digitalWrite`/`delay` blocks in `loop()`
- [ ] The sequence moves from the first LED to the last, then repeats continuously
- [ ] (Challenge) A buzzer sound or OLED/LCD message marks the end of each full sequence

---

## Task 5 - Spot-the-Bug Worksheet (Extension)



**Round 1:**
```cpp
void beep(onTime) {
  digitalWrite(buzzerPin, HIGH);
  delay(onTime);
  digitalWrite(buzzerPin, LOW);
  delay(200);
}
```
<details><summary>Answer</summary>The parameter <code>onTime</code> is missing its type — it must be <code>void beep(int onTime)</code>.</details>

**Round 2:**
```cpp
void showColor(int pin, int onTime) {
  digitalWrite(pin, HIGH);
  delay(onTime);
  digitalWrite(pin, LOW);
}

void loop() {
  showColor(redPin);
}
```
<details><summary>Answer</summary><code>showColor</code> expects two arguments (<code>pin</code> and <code>onTime</code>), but only one was passed in — this won't compile.</details>

**Round 3:**
```cpp
void controlLED(bool turnOn) {
  digitalWrite(ledPin, turnOn);
  return turnOn;
}
```
<details><summary>Answer</summary>The function is declared <code>void</code> but tries to <code>return turnOn;</code> — a <code>void</code> function can't return a value. It should be declared <code>bool controlLED(bool turnOn)</code>.</details>

**Round 4:**
```cpp
bool controlLED(bool turnOn) {
  digitalWrite(ledPin, turnOn);
}

void loop() {
  bool ledState = controlLED(true);
  Serial.println(ledState);
}
```
<details><summary>Answer</summary><code>controlLED</code> is declared to return a <code>bool</code>, but it never uses a <code>return</code> statement — it needs <code>return turnOn;</code> at the end so <code>ledState</code> actually receives a value.</details>

**Round 5:**
```cpp
void lightLED(int ledPin, int delayTime) {
  digitalWrite(ledPin, HIGH);
  delay(delayTime);
  digitalWrite(ledPin, LOW);
}

void loop() {
  lightLED(200, 4);
}
```
<details><summary>Answer</summary>The arguments are swapped — <code>lightLED(int ledPin, int delayTime)</code> expects the pin number first and the delay second, but this call passes <code>200</code> as the pin (not a real GPIO) and <code>4</code> as the delay (4 ms, barely visible). It should be <code>lightLED(4, 200)</code>.</details>

**Round 6:**
```cpp
int getNoteDuration() {
  return 300;
}

void loop() {
  getNoteDuration();
  playNote(440, 500);
}
```
<details><summary>Answer</summary><code>getNoteDuration()</code> is called but its returned value is never stored or used — the code then hardcodes <code>500</code> into <code>playNote()</code> instead. Calling a value-returning function without using what it returns defeats the point of it; it should be <code>playNote(440, getNoteDuration());</code> or store the result first: <code>int duration = getNoteDuration(); playNote(440, duration);</code>.</details>

**Round 7:**
```cpp
bool checkAnswer(string answer) {
  return answer == "red";
}
```
<details><summary>Answer</summary><code>string</code> (lowercase) isn't a recognised type in Arduino — the built-in text type is <code>String</code> (capital S). This won't compile; it should be <code>bool checkAnswer(String answer)</code>.</details>

**Round 8:**
```cpp
void setRGBColour(int red, int green, int blue) {
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
}

void loop() {
  setRGBColour(255, 0);
}
```
<details><summary>Answer</summary><code>setRGBColour</code> expects three arguments (<code>red</code>, <code>green</code>, and <code>blue</code>), but only two were passed in — this won't compile. It needs a third value, e.g. <code>setRGBColour(255, 0, 0);</code>.</details>

**Self-check:** How many did you spot correctly before looking?

---

## Task 6 - Miniature Traffic-Light System 

**Scenario:**

The local council needs a miniature traffic-light system for a model intersection. The device cycles through green, yellow, and red using the RGB LED. The ordinary LED represents a pedestrian crossing light, the buzzer warns when the lights change, and the OLED/LCD displays instructions such as "STOP" and "GO".

**Components:**

ESP32-S3, 1 × common-cathode RGB LED, 1 × ordinary LED, 1 × active buzzer, 1 × OLED/LCD display (I2C), resistors, jumper wires, breadboard.

| Component | ESP32-S3 GPIO |
|-----------|-------------|
| RGB LED — Red (+) | GPIO14 |
| RGB LED — Green (+) | GPIO18 |
| RGB LED — Blue (+) | GPIO17 |
| RGB LED — Common (−) | GND |
| Pedestrian LED (+) | GPIO5 |
| Buzzer (+) | GPIO4 |
| OLED/LCD — SDA | GPIO8 |
| OLED/LCD — SCL | GPIO9 |

Build the system using three functions, one of each kind covered in this activity:

1. **Function without parameters — `warnChange()`**
   Beeps the buzzer once and briefly flashes the pedestrian LED. Call it every time the traffic light is about to change colour, so it always behaves the same way with no inputs needed.

2. **Function with parameters — `setTrafficLight(int redPin, int greenPin, int bluePin, int onTime)`**
   Turns on only the correct RGB colour pin for `onTime` milliseconds (the other two stay off), then turns it off. Use it three times in `loop()` — once each for green, yellow (red + green together), and red — so the same function drives the whole cycle.

3. **Function with a return value — `bool pedestrianCanCross()`**
   Returns `true` while the traffic light is red (safe to cross) and `false` otherwise. Use the returned value to decide whether the OLED/LCD shows `"GO"` or `"STOP"`, and whether the pedestrian LED is lit.


**Suggested sequence in `loop()`:**
1. Show green (traffic light) → OLED shows `"STOP"` for pedestrians.
2. Call `warnChange()`.
3. Show yellow.
4. Call `warnChange()`.
5. Show red (traffic light) → check `pedestrianCanCross()` → OLED shows `"GO"`, pedestrian LED on.
6. Pause, then repeat.

Wokwi https://wokwi.com/projects/471514510742063105

**Check yourself:**
- [ ] All three functions exist and match the required kind (no parameters / with parameters / with a return value)
- [ ] `setTrafficLight()` is reused for all three colours — no repeated `digitalWrite`/`delay` blocks
- [ ] `warnChange()` runs every time the light changes colour
- [ ] `pedestrianCanCross()` returns `true` only when the traffic light is red
- [ ] The OLED/LCD text matches the pedestrian light state (`"STOP"` vs `"GO"`)

---

## Task 7 - Electronic Music Box (Capstone)

**Scenario:**

A toy company wants a small electronic music box. It plays a melody through the buzzer while the LED flashes with each note. The RGB LED changes colour during the song, and the OLED/LCD shows the number or name of the current note.

**Components:**

ESP32-S3, 1 × active buzzer, 1 × ordinary LED, 1 × common-cathode RGB LED, 1 × OLED/LCD display (I2C), resistors, jumper wires, breadboard.

| Component | ESP32-S3 GPIO |
|-----------|-------------|
| Buzzer (+) | GPIO4 |
| Note LED (+) | GPIO5 |
| RGB LED — Red (+) | GPIO14 |
| RGB LED — Green (+) | GPIO18 |
| RGB LED — Blue (+) | GPIO17 |
| RGB LED — Common (−) | GND |
| OLED/LCD — SDA | GPIO8 |
| OLED/LCD — SCL | GPIO9 |

Build the system using three functions, one of each kind covered in this activity:

1. **Function without parameters — `void startMelody()`**
   Displays a "Now Playing" message on the OLED/LCD and briefly cycles the RGB LED through its colours. Call it once from `setup()`, before the song starts, so it always runs the same way with no inputs needed.

2. **Function with parameters — `void playNote(int frequency, int duration)`**
   Plays a single note on the buzzer at `frequency` for `duration` milliseconds, flashing the note LED for the same length of time. Use it once per note in `loop()`, passing in a different frequency and duration each time, so the same function plays every note in the melody.

3. **Function with a return value — `int getNoteDuration()`**
   Looks up how long the *current* note in the melody should last (e.g. from an array of durations, indexed by a note counter) and returns that value. Use the returned value as the `duration` argument passed into `playNote()`, and to update the note number/name shown on the OLED/LCD.

**Suggested sequence in `loop()`:**
1. Call `startMelody()` once (or move it into `setup()`).
2. For each note in the melody: call `getNoteDuration()` to find how long it should play, update the OLED/LCD with the note's number or name, then call `playNote()` with that note's frequency and the returned duration.
3. Change the RGB LED colour every few notes.
4. Pause after the last note, then repeat the song.
   
Wokwi https://wokwi.com/projects/471518431484812289

**Check yourself:**
- [ ] All three functions exist and match the required kind (no parameters / with parameters / with a return value)
- [ ] `playNote()` is reused for every note — no repeated `tone()`/`digitalWrite`/`delay` blocks
- [ ] `getNoteDuration()` returns an `int` and that value is used both to play the note and to update the OLED/LCD
- [ ] The note LED flashes in time with each note
- [ ] The RGB LED visibly changes colour as the melody plays

---


## Questions

Answer these in your own words before moving on:

1. What is the difference between a function with parameters and a function without parameters?
2. Why is `showColor(pin, onTime)` more flexible than a fixed `showColor()` with no parameters?
3. What does `return` do inside a function, and why don't `void` functions use it?
4. In Task 1, what would happen if `beep()` didn't exist and you had to write out every beep manually?
5. Why do `setup()` and `loop()` not need to be called manually anywhere in your code?

---

## Self-Check Before Submitting

- [ ] All programs compile and upload without errors
- [ ] Every repeated on/off pattern is written once as a function and reused, not copy-pasted
- [ ] Functions with parameters are used where behaviour needs to change (e.g. speed, pin, duration)
- [ ] `controlLED()` correctly returns a `bool` reflecting the LED's new state
- [ ] I can explain, out loud, the difference between a function with parameters, without parameters, and one with a return value
