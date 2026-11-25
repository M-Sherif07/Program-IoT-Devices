# Task - Debugging 

Fix the buggy study-pod monitor below so it meets every requirement. Work through it in order: compile first and fix each red error one at a time, then run it and use `Serial.print()` tracing to find the bugs that only show up as wrong behaviour. Don't rewrite it from scratch. The working structure is already there.

**Scenario:**

A library's bookable study pods need a noise monitor. A potentiometer stands in for a sound-level meter mounted in the pod. When someone checks in with a button press, the current noise level is logged as a timestamped record and compared with that pod's noise limit. A PIR sensor confirms the pod is occupied, but only for the Serial log; it doesn't control anything. If the logged noise level is over the pod's limit, a non-blocking LED and buzzer alert fires so staff at the front desk can see it.

> **Schematic:** before you build the circuit, draw its schematic in [Circuit Studio](https://circuit-studio.pages.dev/) with every component listed below. The GPIO pins are assigned for you in the **Assigned pins** table and match the pin constants in the buggy code, so use exactly those pins on your schematic and on the breadboard. Export the schematic image (Export → Schematic image) and a share link (Export → Share link), and paste both into the Schematic box.

**Components required:**
- ESP32-S3 development board
- 1 × potentiometer (simulated sound-level meter)
- 1 × PIR sensor (occupancy indicator)
- 1 × push button (`INPUT_PULLUP`) for check-in
- 1 × LED (alert indicator)
- 1 × 220 Ω resistor
- Passive buzzer
- Jumper wires, breadboard

> **Schematic:**
>
> _Schematic image placeholder: draw this circuit in [Circuit Studio](https://circuit-studio.pages.dev/), then paste your exported schematic image here (Export → Schematic image) and add your share link (Export → Share link) below it._
>
> <details><summary>Schematic link</summary>
>
> [Open the example schematic in Circuit Studio]()
>
> </details>
>
> **Assigned pins** (use exactly these GPIOs on your schematic, on the breadboard and in the code; they match the pin constants in the code below):
>
> | Connection | Pin type | GPIO |
> |---|---|---|
> | Potentiometer wiper (outer legs → 3V3 and GND) | Analog input (ADC) | GPIO 1 |
> | PIR sensor OUT (PIR VCC → 3V3, GND → GND) | Digital input | GPIO 3 |
> | Check-in button (other leg → GND; uses `INPUT_PULLUP`) | Digital input | GPIO 4 |
> | Alert LED (through a 220 Ω resistor; LED cathode → GND) | Digital output | GPIO 10 |
> | Passive buzzer + (buzzer − → GND) | Digital output (`tone()`) | GPIO 6 |

**Buggy code:**
```cpp
struct StudySession {
  unsigned long timestamp;
  int noiseLevel;
};

struct NoiseLimit {
  String pod;
  int maxLevel;
}

const int potPin = 1;
const int pirPin = 3;
const int checkinButtonPin = 4;
const int alertLedPin = 10;
const int buzzerPin = 6;

const int MAX_SESSIONS = 12;
StudySession sessions[MAX_SESSIONS];
int session_count = 0;

NoiseLimit limits[1] = { {"pod-a", 60} };

bool lastButtonReading = HIGH;
bool buttonState = HIGH;
unsigned long debounceStart = 0;
const long DEBOUNCE_TIME = 40;

int get_limit(String pod) {
  for (int i = 0; i < 1; i++) {
    if (limits[i].pod == pod) return limits[i].maxLevel;
  }
  return -1;
}

void log_session(int noiseLevel) {
  if (session_count >= MAX_SESSIONS) return;
  sessions[session_count] = {millis(), noiseLevel};
  session_count++;

  int i = session_count - 1;
  while (i > 0 && sessions[i - 1].timestamp < sessions[i].timestamp) {
    StudySession temp = sessions[i];
    sessions[i] = sessions[i - 1];
    sessions[i - 1] = temp;
    i--;
  }
}

void trigger_alert() {
  digitalWrite(alertLedPin, HIGH);
  tone(buzzerPin, 1000);
  delay(300);
  digitalWrite(alertLedPin, LOW);
  noTone(buzzerPin);
}

void setup() {
  Serial.begin(115200);
  pinMode(potPin, INPUT);
  pinMode(pirPin, OUTPUT);
  pinMode(checkinButtonPin, INPUT);
  pinMode(alertLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  bool reading = digitalRead(checkinButtonPin);
  if (reading != lastButtonReading) debounceStart = millis();
  if (millis() - debounceStart > DEBOUNCE_TIME && reading != buttonState) {
    buttonState = reading;
    if (buttonState == LOW) {
      int raw = analogRead(potPin);
      int noiseLevel = raw / 4095 * 100;
      log_session(noiseLevel);

      int limit = get_limit("pod-a");
      if (noiseLevel > limit) trigger_alert();
    }
  }
  lastButtonReading = reading;

  bool occupied = digitalRead(pirPin) == HIGH;
  if (occupied) Serial.println("Pod occupied");
}
```

**Requirements. Your submission must:**
- Include a Circuit Studio schematic (image + share link) showing every component, wired to the assigned pins.

**The fixed program must:**
- Compile with no errors.
- Debounce the Check-in button properly (`lastButtonReading`/`buttonState`/`debounceStart`, `DEBOUNCE_TIME = 40`), so one physical press logs exactly one session, never several.
- Convert the check-in reading into a `0-100` noise-level percentage using floating-point division, not truncated integer division, before it is passed to `log_session()`.
- Insert every logged `StudySession` in **ascending** timestamp order, the same direction used everywhere else in this course.
- Check `get_limit()`'s sentinel (`-1`, returned for an unknown pod name) before comparing the noise level against it.
- Time the alert (LED + buzzer, `tone()`/`noTone()`) with `millis()`, not `delay()`. The Check-in button and PIR must both keep responding while an alert is sounding.
- Set the PIR pin up as an input, and print the occupancy state to Serial only when it changes, not on every pass of `loop()`.

**Extension (optional):**
- Add a second `NoiseLimit` entry (e.g. `"pod-b"`) and a way to choose which pod's limit applies to the current check-in (e.g. a second button that cycles between pods).
- Write `void print_log()` that prints every stored session's `timestamp` and `noiseLevel` in order. Trigger it with a long press (held for more than 1000 ms) of the Check-in button, kept separate from a normal short press.

### Questions

- Before you fix anything, the compiler's first error message points at one specific line. Which line is it, and why does fixing it first (rather than skipping ahead to a later error) matter here?
  ```


  ```

- `raw / 4095 * 100` and `raw / 4095.0 * 100` look almost identical. Explain exactly why they give different results for the same `raw` value.
  ```


  ```

- Suppose `get_limit()`'s sentinel were never checked, and a pod name with a typo (e.g. `"pod-A"` instead of `"pod-a"`) were looked up. What would the device do, and what would you see on the LED and buzzer?
  ```


  ```

- Why does `pinMode(pirPin, OUTPUT)` compile with no error at all, even though it's wrong for a sensor input? What category of error is this, and how would you notice it?
  ```


  ```

- If you weren't sure whether the noise-level conversion or the insertion-sort step was producing wrong values, what `Serial.print()` statements would you add, and in what order, to work out which one it is?
  ```


  ```

### Spot-the-Bug Worksheet

**Format:** For each round, read the snippet and write down what's wrong **before** opening the answer.

**Round 1:**
```cpp
void loop() {
  if (digitalRead(checkinButtonPin) == LOW) {
    log_session(analogRead(potPin));
  }
}
```
<details><summary>Answer</summary>There's no debouncing at all. The mechanical bounce of one press is read as several quick `LOW` readings, so `log_session()` runs (and adds a record) several times for one press, quickly filling `sessions[]` with near-duplicate entries. A settle-time debounce check needs to control the call.</details>

**Round 2:**
```cpp
struct StudySession {
  unsigned long timestamp;
  int noiseLevel;
}

StudySession sessions[MAX_SESSIONS];
```
<details><summary>Answer</summary>The struct definition is missing the semicolon after its closing brace. `struct StudySession { ... };` needs a `;` straight after `}`. Without it, the compiler fails because it tries to read `StudySession sessions[MAX_SESSIONS];` as part of the same declaration.</details>

**Round 3:**
```cpp
int get_limit(String pod) {
  for (int i = 0; i <= NUM_LIMITS; i++) {
    if (limits[i].pod == pod) return limits[i].maxLevel;
  }
  return -1;
}
```
<details><summary>Answer</summary>The loop condition should be `i < NUM_LIMITS`, not `i <= NUM_LIMITS`. As written, the loop also checks `limits[NUM_LIMITS]`, one slot past the end of the array, which is out of bounds.</details>

**Round 4:**
```cpp
void loop() {
  bool occupied = digitalRead(pirPin) == HIGH;
  Serial.println(occupied ? "Occupied" : "Empty");
}
```
<details><summary>Answer</summary>There's no edge detection, so this prints the occupancy state on *every* pass of `loop()`. The Serial Monitor fills with about a thousand identical lines a second instead of one line when the state actually changes. It needs to compare with a stored `lastOccupiedState` and only print when the state changes.</details>

**Round 5:**
```cpp
int limit = get_limit("pod-a");
if (noiseLevel > limit) {
  trigger_alert();
}
```
<details><summary>Answer</summary>The sentinel from `get_limit()` (`-1`, when the pod name isn't found) is never checked before the comparison. If `"pod-a"` were misspelled or missing from the table, `limit` would be `-1`, and `noiseLevel > -1` is true for almost any real reading, so the alert would fire on every check-in for a pod that was never set up. The caller must check `if (limit >= 0 && noiseLevel > limit)`.</details>

**Round 6:**
```cpp
void trigger_alert() {
  digitalWrite(alertLedPin, HIGH);
  tone(buzzerPin, 1000);
  delay(300);
  digitalWrite(alertLedPin, LOW);
  noTone(buzzerPin);
}
```
<details><summary>Answer</summary>The `delay(300)` inside `trigger_alert()` stops the whole of `loop()` for 300 ms each time it's called. During that time the Check-in button and PIR can't be read, so a press or occupancy change during the alert is missed. It needs the non-blocking `millis()`-timed `start_alert()`/`update_alert()` pattern instead of one blocking function.</details>

**Round 7:**
```cpp
int i = session_count - 1;
while (i > 0 && sessions[i - 1].timestamp < sessions[i].timestamp) {
  StudySession temp = sessions[i];
  sessions[i] = sessions[i - 1];
  sessions[i - 1] = temp;
  i--;
}
```
<details><summary>Answer</summary>The comparison is backwards. `<` sorts the log into *descending* timestamp order (newest first), the opposite of every other struct log in this course. It should be `>`: only swap left while the earlier record's timestamp is *larger* than the new one's, which keeps the log in ascending order.</details>
