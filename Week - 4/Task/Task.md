# Task - Combining Multiple Sensors (Week 4)

The Smart Alert System from class into a non-blocking, multi-input system using functions, selection, boolean logic, and `millis()`.

**Scenario:**

A security node needs to watch two inputs — a PIR motion sensor and a manual panic button — and sound a buzzer alert on either one, without ever freezing and missing a trigger while the buzzer is sounding. An SSD1306 OLED shows the current status at a glance, instead of only logging to Serial.

**Components required:**
- ESP32-S3 development board
- 1 × PIR sensor
- 1 × push button (`INPUT_PULLUP`)
- 1 × red LED
- 1 × passive buzzer
- 1 × SSD1306 OLED display (128×64, I²C)
- 220 Ω resistor for the LED
- Jumper wires, breadboard

>
> **Wiring:**
>

```mermaid
flowchart LR
    ESP32["ESP32-S3"]

    ESP32 -- GPIO3 --- PIR["PIR Sensor"]
    PIR --- GND1["GND"]

    ESP32 -- GPIO2 --- BTN["Push Button"]
    BTN --- GND2["GND"]

    ESP32 -- GPIO13 --- R1["220 Ω Resistor"]
    R1 --- LEDAnode["Red LED Anode (+)"]
    LEDAnode --- LEDCathode["Red LED Cathode (−)"]
    LEDCathode --- GND3["GND"]

    ESP32 -- GPIO7 --- BUZZ["Passive Buzzer"]
    BUZZ --- GND4["GND"]

    ESP32 -- GPIO8 --- OLEDSDA["OLED SDA"]
    ESP32 -- GPIO9 --- OLEDSCL["OLED SCL"]
    OLEDSDA --- OLED["SSD1306 OLED"]
    OLEDSCL --- OLED
    OLED --- GND5["GND"]
```



**Requirements:**
- Declare every pin number as `constexpr uint8_t`, not `int` — e.g. `constexpr uint8_t pirPin = 3;`.
- Configure the PIR as `INPUT`, the button as `INPUT_PULLUP`, and the LED and buzzer as `OUTPUT`.
- Write one function per action — e.g. `read_pir()`, `read_button()`, `set_led()`, `start_buzzer()`, `update_buzzer()`, `update_display()`, `log_event()`. `loop()` should only call these, never contain raw `digitalRead`/`digitalWrite` calls.
- Use `if` / `else if` / `else` selection to decide what `loop()` does each pass.
- Combine the two sensor readings with boolean OR (`||`) so **either** the PIR or the button can trigger the alert.
- This is a **passive** buzzer, so `start_buzzer()` must use `tone(buzzerPin, buzzerFrequency)` (not `digitalWrite()`) and `update_buzzer()` must call `noTone(buzzerPin)` when it stops.
- Time the buzzer with `millis()`, **not** `delay()` — `update_buzzer()` must be called every single pass of `loop()` so the PIR and button are still read while the buzzer is sounding.
- Declare every `millis()`-based timing variable as `unsigned long`.
- Set up the OLED as in the Week 2 resource notes: `#include <Adafruit_GFX.h>` and `#include <Adafruit_SSD1306.h>`, `Wire.begin(8, 9)` and `display.begin(SSD1306_SWITCHCAPVCC, 0x3C)` in `setup()`.
- Write `update_display(String status)` — it should call `display.clearDisplay()`, `display.setCursor(0, 0)`, `display.println(status)`, and finish with `display.display()` so the buffer actually appears on screen. Call it only when the status text actually **changes**, not on every pass of `loop()`, so the screen isn't rewritten dozens of times a second for no reason.
- Print a labelled log message via `log_event()` each time an alert triggers, stating which sensor caused it, and pass the same status text to `update_display()`.

**Extension (optional):**
- Add a second condition using boolean AND (`&&`) — e.g. only sound a longer "priority alert" pattern if motion is detected **and** the button is also held.
- Add counters that track how many times the PIR and the button each triggered, and print both counts in the log output and on the OLED.

>
> Wokwi Link : https://wokwi.com/projects/472676603111416833
>

### Questions

- Why must `update_buzzer()` run on every pass of `loop()`, rather than only inside the `if` block that starts the buzzer?
  ```


  ```

- What would happen to the PIR and button readings if `update_buzzer()`'s timing check used `delay()` instead of comparing against `millis()`?
  ```


  ```

- Why does `buzzerStartTime` need to be `unsigned long` instead of `int` or a signed `long`?
  ```


  ```

- In `if (motionDetected || buttonPressed)`, what happens on a pass where both are `true` at once?
  ```


  ```

- Why is it useful for `read_pir()` to return `bool` rather than calling `digitalRead()` directly inside `loop()`?
  ```


  ```

- Why does `update_display()` only get called when the status text changes, rather than on every pass of `loop()` like `update_buzzer()` does?
  ```


  ```

### Spot-the-Bug Worksheet

**Format:** For each round, read the snippet and write down what's wrong **before** revealing the answer.

**Round 1:**
```cpp
void loop() {
  bool motionDetected = read_pir();
  bool afterHours = true;

  if (motionDetected & afterHours) {
    start_buzzer(500);
  }
}
```
<details><summary>Answer</summary><code>&</code> is the bitwise AND operator, not the logical AND — it happens to give the right answer here because both sides are <code>bool</code>, but it's the wrong operator to reach for and breaks silently on non-boolean values. It should be <code>&&</code>.</details>

**Round 2:**
```cpp
long previousMillis = 0;
const long interval = 1000;

void loop() {
  long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    update_buzzer();
  }
}
```
<details><summary>Answer</summary><code>previousMillis</code> and <code>currentMillis</code> are declared as signed <code>long</code>, but <code>millis()</code> returns <code>unsigned long</code>. Once <code>millis()</code>'s value grows large enough, storing it in a signed <code>long</code> can produce incorrect or negative values, breaking the comparison. Both variables should be <code>unsigned long</code>.</details>

**Round 3:**
```cpp
void activate_buzzer(int duration) {
  tone(buzzerPin, buzzerFrequency);
  delay(duration);
  noTone(buzzerPin);
}

void loop() {
  if (read_pir() || read_button()) {
    activate_buzzer(2000);
  }
}
```
<details><summary>Answer</summary><code>delay(2000)</code> blocks the entire program for 2 seconds — the PIR and button can't be read at all during that time, so any trigger that happens while the buzzer is sounding is missed. This should use the non-blocking <code>start_buzzer()</code> / <code>update_buzzer()</code> pattern with <code>millis()</code> instead.</details>

**Round 4:**
```cpp
void loop() {
  bool motionDetected = read_pir();
  bool buttonPressed  = read_button();

  if (buttonPressed) {
    log_event("Manual trigger");
  } else if (motionDetected) {
    log_event("Motion trigger");
  } else if (motionDetected && buttonPressed) {
    log_event("Both triggered");
  }
}
```
<details><summary>Answer</summary>The third branch can never run. <code>else if</code> only evaluates when every earlier condition was false, and the first branch already catches every case where <code>buttonPressed</code> is true — so <code>motionDetected && buttonPressed</code> is unreachable. The combined "both" case needs to be checked first, before the individual ones.</details>

**Round 5:**
```cpp
bool read_button() {
  bool pressed = digitalRead(buttonPin) == LOW;
}

void loop() {
  if (read_button()) {
    set_led(true);
  }
}
```
<details><summary>Answer</summary><code>read_button()</code> computes <code>pressed</code> but never returns it — it's missing <code>return pressed;</code>. A non-<code>void</code> function that doesn't return a value gives an unreliable result, so the <code>if</code> condition in <code>loop()</code> can't be trusted.</details>

**Round 6:**
```cpp
void update_display(String status) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(status);
}

void loop() {
  update_display("System OK");
}
```
<details><summary>Answer</summary><code>update_display()</code> never calls <code>display.display()</code> — <code>println()</code> only writes into the internal buffer, and nothing actually appears on the physical screen until <code>display.display()</code> sends that buffer to the SSD1306 chip.</details>

**Round 7:**
```cpp
int pirPin = 3;
int buttonPin = 2;
int ledPin = 13;
int buzzerPin = 7;

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}
```
<details><summary>Answer</summary>These pin numbers are declared as <code>int</code> (16 or 32 bits, signed, and modifiable at runtime) when every value fits comfortably in a single byte and never changes after being set. They should be <code>constexpr uint8_t</code> — <code>uint8_t</code> matches the actual range needed (0–255) and <code>constexpr</code> tells the compiler the value is fixed at compile time, e.g. <code>constexpr uint8_t pirPin = 3;</code>.</details>

