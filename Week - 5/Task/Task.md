# Task - Lists, Arrays & Loops (Week 5)

Build a non-blocking, 3-zone security panel: each zone has its own arm/disarm button and status LED, and triggering an armed zone plays that zone's own buzzer alert pattern, looked up from a 2-D array — all without a single `delay()`.

**Scenario:**

A workshop bench station has 3 independent security zones. Pressing a zone's button toggles whether that zone is armed, lighting its matching LED. A separate "Trigger" button simulates an intrusion: when pressed, the program checks the zones in order and, for the first one it finds armed, plays that zone's own beep pattern on a buzzer — each zone has a different pattern, stored as one row of a 2-D array of beep durations. Every button on the panel must stay independently debounced and responsive, even while an alert is mid-playback.

**Components required:**
- ESP32-S3 development board
- 3 × push button (`INPUT_PULLUP`) — zone arm/disarm
- 3 × LED — zone status
- 3 × 220 Ω resistor
- 1 × push button (`INPUT_PULLUP`) — trigger
- 1 × passive buzzer
- Jumper wires, breadboard

>
> **Wiring:**
>

```mermaid
flowchart LR
    ESP32["ESP32-S3"]

    ESP32 -- GPIO12 --- ZBtn1["Zone 1 Button"]
    ZBtn1 --- GND1["GND"]
    ESP32 -- GPIO4 --- R1["220 Ω Resistor"]
    R1 --- Anode1["Zone 1 LED Anode (+)"]
    Anode1 --- Cathode1["Zone 1 LED Cathode (−)"]
    Cathode1 --- GND2["GND"]

    ESP32 -- GPIO13 --- ZBtn2["Zone 2 Button"]
    ZBtn2 --- GND3["GND"]
    ESP32 -- GPIO5 --- R2["220 Ω Resistor"]
    R2 --- Anode2["Zone 2 LED Anode (+)"]
    Anode2 --- Cathode2["Zone 2 LED Cathode (−)"]
    Cathode2 --- GND4["GND"]

    ESP32 -- GPIO14 --- ZBtn3["Zone 3 Button"]
    ZBtn3 --- GND5["GND"]
    ESP32 -- GPIO6 --- R3["220 Ω Resistor"]
    R3 --- Anode3["Zone 3 LED Anode (+)"]
    Anode3 --- Cathode3["Zone 3 LED Cathode (−)"]
    Cathode3 --- GND6["GND"]

    ESP32 -- GPIO2 --- TrigBtn["Trigger Button"]
    TrigBtn --- GND7["GND"]

    ESP32 -- GPIO7 --- Buzzer["Passive Buzzer"]
    Buzzer --- GND8["GND"]
```

**Requirements:**
- Declare parallel arrays `const int zonePins[3]`, `const int zoneLedPins[3]`, and `bool zoneArmed[3]` — index `i` refers to the same zone across all three.
- Give every zone button (and the separate Trigger button) its own settle-based debounce state, using per-index arrays (e.g. `lastZoneReading[3]`/`debounceStart[3]`/`buttonState[3]` for the zone buttons, and separate scalars for the single Trigger button) — the same technique as this week's Resource examples, not a burst-sample.
- A debounced press of a zone button toggles `zoneArmed[i]` and drives `zoneLedPins[i]` to match.
- Declare `const int NUM_ZONES = 3;` and `const int PATTERN_LENGTH = 4;`, then `const int alertPatterns[NUM_ZONES][PATTERN_LENGTH]` — one row of beep/gap durations (in ms) per zone, each zone's pattern different.
- On a debounced Trigger press, use a `for` loop to find the **first** armed zone (`zoneArmed[i] == true`) and start playing `alertPatterns[i]` non-blocking (if no zone is armed, do nothing).
- Write `updatePlayback()`, called every pass of `loop()`, which uses `millis()` to step through the active pattern one entry at a time — toggling the buzzer tone on/off — and stops cleanly once the pattern's `PATTERN_LENGTH` entries are exhausted.
- Do not use `delay()` anywhere in `loop()` — every button must be readable on every single pass, including while a pattern is playing.

**Extension (optional):**
- Add `int triggerCount[NUM_ZONES]`, incremented every time that zone's pattern is triggered, and print the full array whenever it changes.
- Add a fourth "silence" button that immediately stops whatever pattern is currently playing (without disarming any zone).

>
> Wokwi Link : _(build this circuit at wokwi.com, save the project, and paste your link here)_

### Questions

- Why must `zonePins[]`, `zoneLedPins[]`, and `zoneArmed[]` always be read and written at the same index `i`, rather than any one of them ever drifting out of sync with the others?
  ```


  ```

- Why does each zone button need its own entry in the debounce arrays, instead of one shared set of debounce variables covering all three?
  ```


  ```

- In `alertPatterns[NUM_ZONES][PATTERN_LENGTH]`, why does the order of the two indices matter, even though `alertPatterns[zone][step]` and `alertPatterns[step][zone]` would allocate the same total amount of memory?
  ```


  ```

- Why does the search for "the first armed zone" need a `for` loop that can stop as soon as it finds one, rather than checking all 3 zones unconditionally every time?
  ```


  ```

- What would go wrong if `updatePlayback()` used `delay()` between each pattern step instead of checking `millis()` every pass?
  ```


  ```

- Why does `updatePlayback()` need to check the pattern index against `PATTERN_LENGTH` before reading `alertPatterns[zone][step]`?
  ```


  ```

### Spot-the-Bug Worksheet



**Round 1:**
```cpp
const int NUM_ZONES = 3;
bool zoneArmed[NUM_ZONES];

void printZones() {
  for (int i = 0; i <= NUM_ZONES; i++) {
    Serial.println(zoneArmed[i]);
  }
}
```
<details><summary>Answer</summary>The condition should be <code>i &lt; NUM_ZONES</code>, not <code>i &lt;= NUM_ZONES</code>. As written, the loop reads <code>zoneArmed[3]</code> on its last pass — one index past the end of a 3-element array, which is out of bounds.</details>

**Round 2:**
```cpp
const int zonePins[3] = {12, 13, 14};
const int zoneLedPins[3] = {4, 5, 6};
bool zoneArmed[3] = {false, false, false};

void toggleZone(int i) {
  zoneArmed[0] = !zoneArmed[0];
  digitalWrite(zoneLedPins[0], zoneArmed[0]);
}
```
<details><summary>Answer</summary><code>toggleZone()</code> receives the zone index <code>i</code> as a parameter but never uses it — every call hard-codes index <code>0</code> instead. Toggling zone 2 or 3 silently toggles zone 1 again. It should use <code>zoneArmed[i]</code> and <code>zoneLedPins[i]</code> throughout.</details>

**Round 3:**
```cpp
int lastZoneReading[3] = {HIGH, HIGH, HIGH};
unsigned long debounceStart[3] = {0, 0, 0};

void checkZoneButtons() {
  for (int i = 0; i < 3; i++) {
    int reading = digitalRead(zonePins[i]);
    debounceStart[i] = millis();

    if (millis() - debounceStart[i] >= 50 && reading != lastZoneReading[i]) {
      toggleZone(i);
    }
    lastZoneReading[i] = reading;
  }
}
```
<details><summary>Answer</summary><code>debounceStart[i]</code> is reset to <code>millis()</code> on <em>every</em> pass, not only when the raw reading changes, so <code>millis() - debounceStart[i]</code> never has a chance to reach 50 ms. The reset should only happen inside an <code>if (reading != lastZoneReading[i])</code> check.</details>

**Round 4:**
```cpp
const int NUM_ZONES = 3;
const int PATTERN_LENGTH = 4;
const int alertPatterns[NUM_ZONES][PATTERN_LENGTH] = {
  {200, 200, 200, 200},
  {400, 400, 400, 400},
  {100, 100, 100, 100}
};

int getBeep(int step, int zone) {
  return alertPatterns[step][zone];
}
```
<details><summary>Answer</summary><code>alertPatterns</code> was declared <code>[NUM_ZONES][PATTERN_LENGTH]</code> — zone first, step second — but <code>getBeep()</code> reads <code>alertPatterns[step][zone]</code>, the axes reversed. Since <code>NUM_ZONES</code> is only 3, any <code>step</code> value of 3 used as the first index reads out of bounds. It needs to match the declared order: <code>alertPatterns[zone][step]</code>.</details>

**Round 5:**
```cpp
bool patternPlaying = false;
int currentStep = 0;
int activeZone = 0;

void updatePlayback() {
  if (!patternPlaying) return;

  Serial.println(alertPatterns[activeZone][currentStep]);
  currentStep++;
}
```
<details><summary>Answer</summary>There's no timing check at all — <code>updatePlayback()</code> advances <code>currentStep</code> and prints a new pattern entry on <em>every single pass</em> of <code>loop()</code> (potentially thousands of times per second) instead of once per beep duration. It needs a <code>millis()</code>-based check against the current step's duration before advancing, and it never checks <code>currentStep</code> against <code>PATTERN_LENGTH</code> either, so it will keep reading past the end of the pattern array once the real pattern is exhausted.</details>

**Round 6:**
```cpp
int triggerCount[3] = {0, 0, 0};

void onTrigger(int zone) {
  triggerCount[zone]++;
  Serial.println(triggerCount);
}
```
<details><summary>Answer</summary><code>Serial.println(triggerCount)</code> prints the array's memory address (since <code>triggerCount</code> decays to a pointer), not its contents. Printing the actual values requires a <code>for</code> loop over the array, printing <code>triggerCount[i]</code> for each index.</details>

**Round 7:**
```cpp
int lastTriggerReading = HIGH;
unsigned long triggerDebounceStart = 0;

void loop() {
  int reading = digitalRead(triggerPin);
  if (reading != lastTriggerReading) {
    triggerDebounceStart = millis();
  }

  if (millis() - triggerDebounceStart >= 50 && reading == LOW) {
    fireTrigger();
  }

  lastTriggerReading = reading;
}
```
<details><summary>Answer</summary>There's no check against a stored, debounced <code>triggerState</code> before calling <code>fireTrigger()</code> — once the button has been held LOW for 50 ms, this condition stays true on <em>every subsequent pass</em> for as long as the button remains held, calling <code>fireTrigger()</code> repeatedly instead of once per press. It needs to compare against a stored debounced state and only fire on the transition, the same way the zone buttons do.</details>
