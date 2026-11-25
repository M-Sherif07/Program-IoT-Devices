# Activity - Debugging

Each task below gives you buggy code for a circuit you build first. Compile it and watch it fail or misbehave. Then find and fix the bugs **one at a time**, recompiling after each fix, rather than rewriting the file from scratch. Don't just copy a fixed pattern from memory: use the Arduino IDE's error message, the line it names, and `Serial.print()` tracing to locate each bug.

> **Schematics:** for every task that has a circuit (Tasks 1-7), draw the schematic **before** you build it, using [Circuit Studio](https://circuit-studio.pages.dev/).
> - Include **every component** in that task's Components list, with the ESP32-S3 board.
> - **The GPIO pins are assigned for you.** Each task's **Assigned pins** table matches the pin constants in its code. Use exactly those pins on your schematic and in Wokwi, or the code won't work with your circuit.
> - Show the resistors, and connect every ground to GND.
> - When it's finished, use **Export → Schematic image** to save a picture and **Export → Share link** to get a link, and paste both into the Schematic box in each task.

## Task 1 - Parameterised Blink Function (Compile Errors)

**Scenario:**

A workshop bench needs two LEDs to flash different patterns, one fast and one slow, from one reusable function. The code below was meant to do this but won't compile. Even once it compiles, the two LEDs don't behave as intended.

**Components:**

- ESP32-S3 development board
- 2x LED
- 2x 220 Ω resistor
- Jumper wires

> **Schematic:**
>
> _[Schematic image placeholder: draw this circuit in [Circuit Studio](https://circuit-studio.pages.dev/), then paste your exported schematic image here (Export → Schematic image) and add your share link (Export → Share link) below it.]_
>
> <details><summary>Schematic link</summary>
>
> [Open the example schematic in Circuit Studio]()
>
> </details>
>
> **Assigned pins** (use exactly these GPIOs on your schematic, in Wokwi and in the code; they match the pin constants in the code below):
>
> | Connection | Pin type | GPIO |
> |---|---|---|
> | LED 1 (through a 220 Ω resistor; LED cathode → GND) | Digital output | GPIO 4 |
> | LED 2 (through a 220 Ω resistor; LED cathode → GND) | Digital output | GPIO 5 |

**Buggy code (find and fix 4 bugs):**
```cpp
int ledPin1 = 4
int ledPin2 = 5;

void blink_led(int pin, int times, int onTime) {
  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(onTime);
    digitalWrite(pin, LOW);
    delay(onTime);
  }
}

voit setup() {
  pinMode(ledPin1, OUTPUT);
  blink_led(ledPin1, 6, 150);
  blink_led(ledPin2, 6, 150);
}

void loop() {
}
```

**Intended behaviour:** at startup, LED 1 blinks 6 times fast (150 ms), then LED 2 blinks 6 times noticeably slower.


**Check yourself:**
- [ ] Schematic drawn in Circuit Studio with every listed component and the assigned pins, with the image and share link pasted in the Schematic box
- [ ] The sketch compiles with no red errors
- [ ] Both LEDs are set to `OUTPUT` before either is written to
- [ ] LED 1 and LED 2 visibly blink at two different speeds
- [ ] `blink_led()` itself was not rewritten; only the bugs around it were fixed

---

## Task 2 - Debounced Toggle Button (Logic Errors)

**Scenario:**

A bench light should toggle on or off with a single press. The code below toggles several times per press, and sometimes doesn't respond at all.

**Components:**

- ESP32-S3 development board
- 1x push button (`INPUT_PULLUP`)
- 1x LED
- 1x 220 Ω resistor
- Jumper wires

> **Schematic:**
>
> Schematic image placeholder: draw this circuit in [Circuit Studio](https://circuit-studio.pages.dev/), then paste your exported schematic image here (Export → Schematic image) and add your share link (Export → Share link) below it.
>
> <details><summary>Schematic link</summary>
>
> [Open the example schematic in Circuit Studio]()
>
> </details>
>
> **Assigned pins** (use exactly these GPIOs on your schematic, in Wokwi and in the code; they match the pin constants in the code below):
>
> | Connection | Pin type | GPIO |
> |---|---|---|
> | Toggle button (other leg → GND; uses `INPUT_PULLUP`) | Digital input | GPIO 4 |
> | LED (through a 220 Ω resistor; LED cathode → GND) | Digital output | GPIO 10 |

**Buggy code (find and fix 3 bugs):**
```cpp
const int buttonPin = 4;
const int ledPin = 10;
bool ledOn = false;

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  if (digitalRead(buttonPin) == HIGH) {
    ledOn = !ledOn;
    digitalWrite(ledPin, ledOn);
  }
}
```

**Intended behaviour:** the LED toggles exactly once per physical press, using settle-time debouncing (`lastButtonReading`/`buttonState`/`debounceStart`, `DEBOUNCE_TIME = 40`) instead of the raw read shown above.



**Check yourself:**
- [ ] Schematic drawn in Circuit Studio with every listed component and the assigned pins, with the image and share link pasted in the Schematic box
- [ ] `INPUT_PULLUP` is used, and a press is read as `LOW`
- [ ] The LED toggles exactly once per physical press, not several times from bounce
- [ ] Settle-time debouncing (`lastButtonReading`/`buttonState`/`debounceStart`) controls the toggle, not a raw `digitalRead()` check
- [ ] Holding the button down does not keep toggling the LED

---

## Task 3 - Armed PIR Alert (Blocking and Boolean-Logic Errors)

**Scenario:**

A security node should sound an alert only when motion is detected **and** the system is armed. The code below fires on either condition alone, keeps retriggering while motion continues, and ignores the arm button while the buzzer sounds.

**Components:**

- ESP32-S3 development board
- 1x PIR sensor
- 1x push button (`INPUT_PULLUP`) for arm/disarm
- 1x LED
- 1x 220 Ω resistor
- Passive buzzer
- Jumper wires

> **Schematic:**
>
> Schematic image placeholder: draw this circuit in [Circuit Studio](https://circuit-studio.pages.dev/), then paste your exported schematic image here (Export → Schematic image) and add your share link (Export → Share link) below it.
>
> <details><summary>Schematic link</summary>
>
> [Open the example schematic in Circuit Studio]()
>
> </details>
>
> **Assigned pins** (use exactly these GPIOs on your schematic, in Wokwi and in the code; they match the pin constants in the code below):
>
> | Connection | Pin type | GPIO |
> |---|---|---|
> | PIR sensor OUT (PIR VCC → 3V3, GND → GND) | Digital input | GPIO 3 |
> | Arm/disarm button (other leg → GND; uses `INPUT_PULLUP`) | Digital input | GPIO 4 |
> | LED (through a 220 Ω resistor; LED cathode → GND) | Digital output | GPIO 10 |
> | Passive buzzer + (buzzer − → GND) | Digital output (`tone()`) | GPIO 6 |

**Buggy code (find and fix 4 bugs):**
```cpp
const int pirPin = 3;
const int armPin = 4;
const int ledPin = 10;
const int buzzerPin = 6;

bool armed = true;

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(armPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  bool motionNow = digitalRead(pirPin) == HIGH;

  if (armed || motionNow) {
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    delay(300);
    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, LOW);
  }
}
```
The arm/disarm button is wired but not read anywhere yet. That's expected: wiring it into the code is part of the fix.

**Intended behaviour:** pressing the (debounced) arm button toggles `armed`. The alert fires only on a new PIR motion edge while `armed` is `true`. The alert is timed with `millis()`, not `delay()`, so the arm button keeps responding the whole time.

>


**Check yourself:**
- [ ] Schematic drawn in Circuit Studio with every listed component and the assigned pins, with the image and share link pasted in the Schematic box
- [ ] The alert needs **both** `armed` **and** a new motion edge, not either one alone
- [ ] Motion is edge-detected (`motionNow && !lastMotionState`), so it doesn't retrigger on every pass while it stays high
- [ ] The alert is timed with `millis()`, never `delay()`
- [ ] The arm button toggles `armed` and still responds while an alert is sounding

---

## Task 4 - Single-Pass Min/Max/Average Scan (Type and Bounds Errors)

**Scenario:**

A workshop wants the minimum, maximum and average of ten potentiometer readings, worked out in a single pass. The code below writes outside the array, and its statistics are wrong even when it doesn't crash.

**Components:**

- ESP32-S3 development board
- Potentiometer
- Jumper wires

> **Schematic:**
>
> _[Schematic image placeholder: draw this circuit in [Circuit Studio](https://circuit-studio.pages.dev/), then paste your exported schematic image here (Export → Schematic image) and add your share link (Export → Share link) below it.]_
>
> <details><summary>Schematic link</summary>
>
> [Open the example schematic in Circuit Studio]()
>
> </details>
>
> **Assigned pins** (use exactly these GPIOs on your schematic, in Wokwi and in the code; they match the pin constants in the code below):
>
> | Connection | Pin type | GPIO |
> |---|---|---|
> | Potentiometer wiper (outer legs → 3V3 and GND) | Analog input (ADC) | GPIO 1 |

**Buggy code (find and fix 3 bugs):**
```cpp
const int potPin = 1;
const int NUM_READINGS = 10;
int readings[NUM_READINGS];

void setup() {
  Serial.begin(115200);
  for (int i = 0; i <= NUM_READINGS; i++) {
    readings[i] = analogRead(potPin);
    delay(300);
  }

  int minVal = 0, maxVal = 0, sum = 0;
  for (int i = 0; i < NUM_READINGS; i++) {
    if (readings[i] < minVal) minVal = readings[i];
    if (readings[i] > maxVal) maxVal = readings[i];
    sum += readings[i];
  }
  int average = sum / NUM_READINGS;

  Serial.println(minVal);
  Serial.println(maxVal);
  Serial.println(average);
}

void loop() {
}
```

**Intended behaviour:** all ten readings are stored inside the array bounds. `minVal` and `maxVal` hold the real minimum and maximum of the readings (a reading can never be below `0`, so starting `minVal` at `0` means it never changes). The average is a proper floating-point value, not rounded down to a whole number.

>


**Check yourself:**
- [ ] Schematic drawn in Circuit Studio with every listed component and the assigned pins, with the image and share link pasted in the Schematic box
- [ ] The capture loop never writes outside `readings[0]` to `readings[NUM_READINGS - 1]`
- [ ] `minVal` and `maxVal` start from `readings[0]`, not `0`
- [ ] The average is calculated and printed as a floating-point value, not truncated integer division
- [ ] The capture loop and the scan loop are still two separate loops

---

## Task 5 - Array-Driven LED Chaser (Off-by-One and Array-Size Errors)

**Scenario:**

A control panel needs a 4-LED chase pattern driven entirely from one array. The code below leaves the LEDs lit instead of chasing, and it configures and writes to a pin that was never wired.

**Components:**

- ESP32-S3 development board
- 4x LED
- 4x 220 Ω resistor
- Jumper wires

> **Schematic:**
>
> Schematic image placeholder: draw this circuit in [Circuit Studio](https://circuit-studio.pages.dev/), then paste your exported schematic image here (Export → Schematic image) and add your share link (Export → Share link) below it.
>
> <details><summary>Schematic link</summary>
>
> [Open the example schematic in Circuit Studio]()
>
> </details>
>
> **Assigned pins** (use exactly these GPIOs on your schematic, in Wokwi and in the code; they match the pin constants in the code below):
>
> | Connection | Pin type | GPIO |
> |---|---|---|
> | LED 1 (through a 220 Ω resistor; LED cathode → GND) | Digital output | GPIO 4 |
> | LED 2 (through a 220 Ω resistor; LED cathode → GND) | Digital output | GPIO 5 |
> | LED 3 (through a 220 Ω resistor; LED cathode → GND) | Digital output | GPIO 6 |
> | LED 4 (through a 220 Ω resistor; LED cathode → GND) | Digital output | GPIO 7 |

**Buggy code (find and fix 3 bugs):**
```cpp
const int NUM_LEDS = 4;
int ledPins[NUM_LEDS] = {4, 5, 6, 7};

void run_chase(int stepDelay) {
  for (int i = 0; i <= NUM_LEDS; i++) {
    digitalWrite(ledPins[i], HIGH);
    delay(stepDelay);
  }
}

void setup() {
  for (int i = 0; i <= NUM_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  run_chase(150);
}
```

**Intended behaviour:** each LED lights in turn for `stepDelay` ms and turns off before the next one lights, cycling through all four with no array access out of bounds.



**Check yourself:**
- [ ] Schematic drawn in Circuit Studio with every listed component and the assigned pins, with the image and share link pasted in the Schematic box
- [ ] Neither `setup()` nor `run_chase()` reads `ledPins[]` past index `NUM_LEDS - 1`
- [ ] Each LED turns off (`LOW`) before the next one lights
- [ ] All four LEDs visibly chase in order, with no LED left lit
- [ ] `ledPins[]` was not resized or renumbered to fix this; only the loop logic was changed

---

## Task 6 - Struct Log With Dictionary Threshold Lookup (Struct and Sentinel Errors)

**Scenario:**

A bench monitor logs potentiometer readings as timestamped records and checks each one against a named threshold. The code below won't compile. Once it does, it sorts new records into the wrong position, uses the threshold without checking whether the lookup actually found one, and logs several records for one press.

**Components:**

- ESP32-S3 development board
- Potentiometer
- 1x push button (`INPUT_PULLUP`) for logging
- 1x LED (alert indicator)
- 1x 220 Ω resistor
- Jumper wires

> **Schematic:**
>
> Schematic image placeholder: draw this circuit in [Circuit Studio](https://circuit-studio.pages.dev/), then paste your exported schematic image here (Export → Schematic image) and add your share link (Export → Share link) below it.
>
> <details><summary>Schematic link</summary>
>
> [Open the example schematic in Circuit Studio]()
>
> </details>
>
> **Assigned pins** (use exactly these GPIOs on your schematic, in Wokwi and in the code; they match the pin constants in the code below):
>
> | Connection | Pin type | GPIO |
> |---|---|---|
> | Potentiometer wiper (outer legs → 3V3 and GND) | Analog input (ADC) | GPIO 1 |
> | Log button (other leg → GND; uses `INPUT_PULLUP`) | Digital input | GPIO 4 |
> | Alert LED (through a 220 Ω resistor; LED cathode → GND) | Digital output | GPIO 10 |

**Buggy code (find and fix 4 bugs):**
```cpp
struct Reading {
  unsigned long timestamp;
  int value;
}

const int MAX_READINGS = 10;
Reading readings[MAX_READINGS];
int reading_count = 0;

struct Threshold { String sensor; float limit; };
Threshold thresholds[1] = { {"bench", 3000.0} };

float get_threshold(String sensor) {
  for (int i = 0; i < 1; i++) {
    if (thresholds[i].sensor == sensor) return thresholds[i].limit;
  }
  return -1.0;
}

void log_reading(int value) {
  if (reading_count >= MAX_READINGS) return;
  readings[reading_count] = {millis(), value};
  reading_count++;

  int i = reading_count - 1;
  while (i > 0 && readings[i - 1].timestamp < readings[i].timestamp) {
    Reading temp = readings[i];
    readings[i] = readings[i - 1];
    readings[i - 1] = temp;
    i--;
  }
}

void setup() {
  pinMode(4, INPUT_PULLUP);
  pinMode(10, OUTPUT);
}

void loop() {
  if (digitalRead(4) == LOW) {
    int value = analogRead(1);
    log_reading(value);
    float limit = get_threshold("bench");
    if (value > limit) digitalWrite(10, HIGH);
  }
}
```

**Intended behaviour:** the sketch compiles. A debounced log button press adds one record and moves it into ascending-timestamp order using the insertion step from Week 6. The alert LED lights only when the logged value is above a threshold that was actually *found*, never for an unknown sensor name.



**Check yourself:**
- [ ] Schematic drawn in Circuit Studio with every listed component and the assigned pins, with the image and share link pasted in the Schematic box
- [ ] The `Reading` struct definition has a semicolon after its closing brace
- [ ] The insertion step moves a new record into ascending-timestamp order (the same direction as every other week's log)
- [ ] `get_threshold()`'s sentinel is checked (`limit >= 0`) before the alert comparison uses it
- [ ] The log button is debounced before `log_reading()` is called, not read raw

---

## Task 7 - Capstone: Revision Bench Station Bug Hunt

**Scenario:**

The full Week 8 revision bench station (struct log, dictionary threshold, armed PIR alert, non-blocking timing) has been copied into a new project, and it now has **six** bugs. It still compiles, which is what makes this one hard: every bug only shows up as wrong behaviour once it's running, so you'll need `Serial.print()` tracing to pin them down.

**Components:**

- ESP32-S3 development board
- Potentiometer
- PIR sensor
- 2x push button (`INPUT_PULLUP`): Log button, Arm button
- 1x LED (alert indicator)
- 1x 220 Ω resistor
- Passive buzzer
- Jumper wires

> **Schematic:**
>
> Schematic image placeholder: draw this circuit in [Circuit Studio](https://circuit-studio.pages.dev/), then paste your exported schematic image here (Export → Schematic image) and add your share link (Export → Share link) below it.
>
> <details><summary>Schematic link</summary>
>
> [Open the example schematic in Circuit Studio]()
>
> </details>
>
> **Assigned pins** (use exactly these GPIOs on your schematic, in Wokwi and in the code; they match the pin constants in the code below):
>
> | Connection | Pin type | GPIO |
> |---|---|---|
> | Potentiometer wiper (outer legs → 3V3 and GND) | Analog input (ADC) | GPIO 1 |
> | PIR sensor OUT (PIR VCC → 3V3, GND → GND) | Digital input | GPIO 3 |
> | Log button (other leg → GND; uses `INPUT_PULLUP`) | Digital input | GPIO 4 |
> | Arm button (other leg → GND; uses `INPUT_PULLUP`) | Digital input | GPIO 5 |
> | Alert LED (through a 220 Ω resistor; LED cathode → GND) | Digital output | GPIO 10 |
> | Passive buzzer + (buzzer − → GND) | Digital output (`tone()`) | GPIO 6 |

**Buggy code (find and fix 6 bugs):**
```cpp
struct Reading {
  unsigned long timestamp;
  int value;
};

struct Threshold { String sensor; float limit; };

const int potPin = 1;
const int pirPin = 3;
const int logButtonPin = 4;
const int armButtonPin = 5;
const int alertLedPin = 10;
const int buzzerPin = 6;

const int MAX_READINGS = 10;
Reading readings[MAX_READINGS];
int reading_count = 0;

Threshold thresholds[1] = { {"bench", 3000.0} };

bool armed = false;
bool lastArmReading = HIGH;
bool armState = HIGH;
unsigned long armDebounceStart = 0;
const long DEBOUNCE_TIME = 40;

bool lastMotionState = false;

bool alertActive = false;
unsigned long alertStart = 0;
const long ALERT_DURATION = 300;

float get_threshold(String sensor) {
  for (int i = 0; i < 1; i++) {
    if (thresholds[i].sensor = sensor) return thresholds[i].limit;   // Bug 1
  }
  return -1.0;
}

void log_reading(int value) {
  if (reading_count > MAX_READINGS) return;   // Bug 2
  readings[reading_count] = {millis(), value};
  reading_count++;

  int i = reading_count - 1;
  while (i > 0 && readings[i - 1].timestamp > readings[i].timestamp) {
    Reading temp = readings[i];
    readings[i] = readings[i - 1];
    readings[i - 1] = temp;
    i--;
  }
}

void start_alert() {
  digitalWrite(alertLedPin, HIGH);
  tone(buzzerPin, 1000);
  alertActive = true;
  alertStart = millis();
}

void update_alert() {
  if (alertActive && millis() - alertStart >= ALERT_DURATION) {
    digitalWrite(alertLedPin, LOW);
    noTone(buzzerPin);
    // Bug 3
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(logButtonPin, INPUT_PULLUP);
  pinMode(armButtonPin, INPUT_PULLUP);
  pinMode(pirPin, INPUT);
  pinMode(alertLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  // Debounced arm button
  bool armReading = digitalRead(armButtonPin);
  if (armReading != lastArmReading) armDebounceStart = millis();
  if (millis() - armDebounceStart > DEBOUNCE_TIME && armReading != armState) {
    armState = armReading;
    if (armState == LOW) armed = !armed;
  }
  lastArmReading = armReading;

  // Log button (Bug 4)
  if (digitalRead(logButtonPin) == LOW) {
    int value = analogRead(potPin);
    log_reading(value);
    float limit = get_threshold("bench");
    if (limit >= 0 && value > limit) start_alert();
  }

  // Armed PIR alert
  bool motionNow = digitalRead(pirPin) == HIGH;
  if (motionNow && !lastMotionState || armed) {   // Bug 5
    start_alert();
  }
  lastMotionState = motionNow;

  update_alert();
  delay(5);   // Bug 6
}
```

**Intended behaviour:** the same as the Week 8 Activity Task 7 capstone. A debounced Log button adds a sorted struct record and alerts only on a real, checked threshold breach. A debounced Arm button toggles `armed`. The alert also fires on a new PIR motion edge *while* armed. The alert clears itself after `ALERT_DURATION` and can fire again on the next real event. Nothing in `loop()` blocks.


**Check yourself:**
- [ ] Schematic drawn in Circuit Studio with every listed component and the assigned pins, with the image and share link pasted in the Schematic box
- [ ] `get_threshold()` compares `sensor` with `==`, not `=`
- [ ] `log_reading()`'s bounds check rejects the write as soon as `reading_count` reaches `MAX_READINGS`, not one later
- [ ] `update_alert()` sets `alertActive` back to `false` when the alert clears, so a later event can trigger it again
- [ ] The Log button is debounced with the same pattern the Arm button already uses
- [ ] The PIR alert needs motion **and** armed together (correct operator precedence), not either one alone
- [ ] `loop()` contains no `delay()` at all

---

## Task 8 - Spot-the-Bug Worksheet (Extension)

**Format:** For each round, read the snippet and write down what's wrong **before** opening the answer.

**Round 1:**
```cpp
float readHumidity() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
  return humidity.relative_humidity;
}

void loop() {
  float h = readHumidity();
  if (h > 70.0) digitalWrite(alertPin, HIGH);
}
```
<details><summary>Answer</summary>The reading is never checked with `isnan()` before it's used. If the I²C sensor read fails, `humidity.relative_humidity` can come back as `NaN` (not a number). Any comparison with `NaN` is always false, so `NaN > 70.0` quietly skips the alert. A failed read looks exactly like a normal, safe reading instead of being caught and reported.</details>

**Round 2:**
```cpp
struct ShelfLimit { String tool; int maxWeight; };
ShelfLimit limits[3] = {
  {200, "drill"},     // Bug
  {"hammer", 500},
  {"saw", 350}
};
```
<details><summary>Answer</summary>The first entry's values are in the wrong order. `{200, "drill"}` tries to put an `int` (`200`) into `tool` (a `String`) and a string literal (`"drill"`) into `maxWeight` (an `int`). Struct initialisers must list values in the same order the fields were declared: `{"drill", 200}`.</details>

**Round 3:**
```cpp
const int NUM_ZONES = 5;
int zoneReadings[NUM_ZONES];

for (int i = 0; i < NUM_ZONES; i++) {
  zoneReadings[i + 1] = analogRead(zonePins[i]);   // Bug
}
```
<details><summary>Answer</summary>The write goes to `zoneReadings[i + 1]` instead of `zoneReadings[i]`. On the last pass (`i == NUM_ZONES - 1`) it writes to `zoneReadings[NUM_ZONES]`, one past the end of the array, and `zoneReadings[0]` is never written at all.</details>

**Round 4:**
```cpp
bool lastButtonReading = HIGH;
bool buttonState = HIGH;
unsigned long debounceStart = 0;

void loop() {
  bool reading = digitalRead(buttonPin);
  if (millis() - debounceStart > DEBOUNCE_TIME && reading != buttonState) {
    buttonState = reading;
    if (buttonState == LOW) count++;
  }
}
```
<details><summary>Answer</summary>`debounceStart` starts at `0` and is never updated when `reading != lastButtonReading`. After the first 40 ms the settle-time check always passes, so every reading is trusted straight away and the debouncing does nothing. The missing line is `if (reading != lastButtonReading) debounceStart = millis();`, and `lastButtonReading` also needs to be set to `reading` at the end of `loop()` (that's missing too).</details>

**Round 5:**
```cpp
int* sortedValues = new int[reading_count];
for (int i = 0; i < reading_count; i++) sortedValues[i] = readings[i].value;
// ... insertion sort, then binary_search() ...
delete sortedValues;   // Bug
```
<details><summary>Answer</summary>An array created with `new int[reading_count]` (note the `[]`) must be freed with `delete[] sortedValues;`, not plain `delete sortedValues;`. Using plain `delete` on an array is undefined behaviour. It might free only part of the memory, or corrupt the heap.</details>

**Round 6:**
```cpp
String sensorName = "temperature";

if (sensorName != "temperature") {
  Serial.println("Unknown sensor");
} else if (sensorName = "humidity") {   // Bug
  Serial.println("Humidity sensor");
}
```
<details><summary>Answer</summary>`sensorName = "humidity"` uses `=` (assignment) instead of `==` (comparison). It compiles, quietly overwrites `sensorName` with `"humidity"`, and the condition is always true (an Arduino `String` that holds text counts as true). It should be `sensorName == "humidity"`.</details>

**Round 7:**
```cpp
void checkThreshold(float temp) {
  if (temp > HIGH_TEMP) {
    Serial.println("High");
  }
  if (temp < LOW_TEMP) {
    Serial.println("Low");
  }
  if (temp >= LOW_TEMP && temp <= HIGH_TEMP)
    Serial.println("Normal");
    digitalWrite(alertLedPin, LOW);   // Bug
}
```
<details><summary>Answer</summary>The last `if` has no braces `{}`, so only the next statement (`Serial.println("Normal");`) depends on the condition. `digitalWrite(alertLedPin, LOW);` runs on *every* call, whatever the temperature, even when the alert should stay on. Indentation doesn't group statements in C++. Both lines need to be inside braces.</details>

---

## Questions

Answer these in your own words before moving on:

1. You get a compile error with wording you've never seen before. What is your first step, and why does that step matter more than guessing at the fix?
   ```


   ```

2. Why does a logic error never produce an IDE error message, even though it's often harder to find than a compile error?
   ```


   ```

3. Explain why `raw / 4095` and `raw / 4095.0` give different results even though `raw` holds the same value in both.
   ```


   ```

4. Describe a situation where `Serial.print()` tracing would find a bug that carefully re-reading the code would not.
   ```


   ```

5. Why is it good practice to delete `Serial.print()` debug lines once a bug is fixed, rather than leaving them commented out forever?
   ```


   ```

6. In Task 7, the PIR alert bug (`motionNow && !lastMotionState || armed`) compiles and looks almost right at a glance. What about C++ operator precedence makes this bug easy to miss when reading?
   ```


   ```
