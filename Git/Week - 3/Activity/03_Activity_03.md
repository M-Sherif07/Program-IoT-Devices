# Activity 3 - Reading Inputs and Selection & Conditional Logic (Week 3)

## Task 1 - Pull-Up Button LED Using `if`

**Scenario:**

A storeroom needs a simple indicator light that turns on for as long as a staff member holds down a wall button, and turns off the instant they let go.

Your task is to program an ESP32-S3 to read a push button wired with the internal pull-up resistor and drive an LED to match.

**Components:**

- ESP32-S3, 1 
- Push button
- LED
- Resistor
- Jumper wires
 
> **Wiring:** 
> 
> 

```mermaid
flowchart LR
    ESP32["ESP32-S3"]

    ESP32 -- GPIO4 --- Button["Push Button"]
    Button --- GND1["GND"]

    ESP32 -- GPIO2 --- R["220 Ω Resistor"]
    R --- Anode["LED Anode (+)"]
    Anode --- Cathode["LED Cathode (−)"]
    Cathode --- GND2["GND"]
```
Configure the button with `INPUT_PULLUP`. Read it every loop with `digitalRead()`. Use a single `if` statement (no `else`) to turn the LED on when pressed, and a second `if` to turn it off when released — practise reasoning about a condition with no `else` branch before Task 2 introduces one.
>
> Wokwi link: https://wokwi.com/projects/472093176967560193
> 
**Check yourself:**
- [ ] The button is configured with `INPUT_PULLUP`, not an external resistor
- [ ] The LED is on only while the button is held down
- [ ] Pressed reads `LOW` and this is reflected correctly in the condition

**Finished Task 1** https://wokwi.com/projects/472117696878376961

---

## Task 2 - Pull-Down Comparison Using `if / else`

**Scenario:**

The same storeroom light now needs to be rebuilt using a pull-down button instead, so the electronics team can compare both wiring styles side by side.

**Components:**

- ESP32-S3
- Button
- LED
- Resistor
- Jumper wires
  
>**Wiring:** 


```mermaid
flowchart LR
    ESP32["ESP32-S3"]

    ESP32 -- GPIO4 --- Button["Push Button"]
    Button --- V33["3.3V"]

    ESP32 -- GPIO2 --- R["220 Ω Resistor"]
    R --- Anode["LED Anode (+)"]
    Anode --- Cathode["LED Cathode (−)"]
    Cathode --- GND["GND"]
```

Configure the button with `INPUT_PULLDOWN`. This time use one `if / else` statement instead of two separate `if`s. Print `"Pressed"` or `"Released"` to the Serial Monitor from inside each branch.

>
> Wokwi Link https://wokwi.com/projects/472093662810633217
> 
**Check yourself:**
- [ ] The button is configured with `INPUT_PULLDOWN`
- [ ] Pressed reads `HIGH` (opposite of Task 1) and the condition matches
- [ ] A single `if / else` replaces the two separate `if` statements from Task 1

**Finished task 2** https://wokwi.com/projects/472142277149305857

---

## Task 3 - PIR Motion Alert Using Boolean Logic

**Scenario:**

A warehouse wants a security buzzer that only sounds when motion is detected **and** it's currently after hours — motion during the day (staff walking around) shouldn't trigger anything.

**Components:**

- ESP32-S3
- PIR motion sensor
- Passive buzzer
- Jumper wires.
- 
> **Wiring:** 

```mermaid
flowchart LR
    ESP32["ESP32-S3"]

    ESP32 -- GPIO4 --- PIR["PIR Sensor "]
    ESP32 -- GPIO7 --- Buzzer["Buzzer (+)"]
```

Declare `bool afterHours` near the top of your program and set it to `true` or `false` manually to simulate the time of day (no real clock needed). Read the PIR sensor every loop. Use `&&` so the buzzer only sounds when **both** motion is detected **and** `afterHours` is `true`.


>
> Wokwi link: https://wokwi.com/projects/472093910048097281
>
**Check yourself:**
- [ ] The buzzer only sounds when motion **and** `afterHours` are both true
- [ ] Changing `afterHours` to `false` in code stops the buzzer even with motion present
- [ ] (Challenge) `!alarmEnabled` correctly overrides everything else when the system is disabled

**Finished task 3** https://wokwi.com/projects/472391424621680641

---

## Task 4 - Potentiometer Dimmer With a Low-Brightness Warning

**Scenario:**

A reading lamp should dim smoothly as a potentiometer knob is turned, but a small indicator LED should light up whenever the brightness is set so low it's barely useful, warning the user.

**Components:**

- ESP32-S3
- Potentiometer
- LED 
- LED (warning indicator)
- Resistors
- Jumper wires
  
> **Wiring:** 

```mermaid
flowchart LR
    ESP32["ESP32-S3"]

    ESP32 -- GPIO1 --- Pot["Potentiometer Wiper"]

    ESP32 -- GPIO9 --- R1["220 Ω Resistor"]
    R1 --- Anode1["Dimmable LED Anode (+)"]
    Anode1 --- Cathode1["Dimmable LED Cathode (−)"]
    Cathode1 --- GND1["GND"]

    ESP32 -- GPIO10 --- R2["220 Ω Resistor"]
    R2 --- Anode2["Warning LED Anode (+)"]
    Anode2 --- Cathode2["Warning LED Cathode (−)"]
    Cathode2 --- GND2["GND"]
```

Read the potentiometer with `analogRead()` (0–4095) and convert it to a 0–255 range with `map()`. Drive the dimmable LED with `analogWrite()`. Use an `if / else` so the warning LED turns on whenever brightness is below 25, and off otherwise.

>
> Wokwi link: https://wokwi.com/projects/472094438850270209
> 
**Check yourself:**
- [ ] The dimmable LED's brightness changes smoothly as the knob turns
- [ ] `map()` correctly converts the 0–4095 reading into the 0–255 PWM range
- [ ] The warning LED turns on only when brightness is below the threshold

**Finished task 4** https://wokwi.com/projects/472392743933831169

---

## Task 5 - Comfort Monitor Using `if / else if / else`

**Scenario:**

A greenhouse controller needs to classify the current temperature into one of three bands — Cold, Comfortable, or Hot — and show the result on the Serial Monitor, using a DHT22 sensor.

**Components:**

- ESP32-S3
- DHT22 sensor
- Resistor
- Jumper wires
  
> **Wiring:** 


```mermaid
flowchart LR
    ESP32["ESP32-S3"]

    ESP32 -- GPIO2 --- DATA["DHT22 DATA"]
    DATA --- R["10 kΩ Resistor"]
    R --- V33["3.3V"]
```

Read the temperature each loop with the DHT library (read no more than once every 2 seconds). Use `if / else if / else`, checking the **most specific/narrow condition first**: below 15 °C → `"Cold"`; 15–29 °C → `"Comfortable"`; 30 °C and above → `"Hot"`. Handle a failed read (`isnan()`) before the temperature check, as shown in the resource notes.

>
> Wokwi link: https://wokwi.com/projects/471760934303645697
> 
**Check yourself:**
- [ ] A failed sensor read is detected with `isnan()` before the temperature is classified
- [ ] Exactly one of the three labels prints per reading, never more than one
- [ ] Conditions are ordered so a later, broader condition can't accidentally catch a case meant for an earlier one

**Finished task 5** https://wokwi.com/projects/472394873536330753

---

## Task 6 - Nested `if` Arm/Disarm System

**Scenario:**

A small security system should only watch for motion while "armed." A single push button toggles the system between armed and disarmed each time it's pressed once (not held).

**Components:**

- ESP32-S3
- Push button
- PIR sensor
- Passive buzzer
- Jumper wires
  
> 
> **Wiring:** 
> 

```mermaid
flowchart LR
    ESP32["ESP32-S3"]

    ESP32 -- GPIO4 --- Button["Push Button"]
    Button --- GND["GND"]

    ESP32 -- GPIO5 --- PIR["PIR Sensor (OUT)"]

    ESP32 -- GPIO7 --- Buzzer["Buzzer (+)"]
```

Track `lastButtonState` so you can detect the *edge* of a press (see the resource notes) rather than reacting every loop the button is held down. When a fresh press is confirmed, flip a `bool systemArmed` flag. Nest your logic: only read and react to the PIR sensor **inside** the block where `systemArmed` is `true`; sound the buzzer while motion is detected within that nested block, and make sure the buzzer is stopped whenever the system is disarmed.

Wokwi link: https://wokwi.com/projects/471661119539001345
**Check yourself:**
- [ ] One press toggles the armed state once — holding the button doesn't rapidly toggle it
- [ ] The PIR sensor is only checked when `systemArmed` is `true` (nested inside that condition)
- [ ] The buzzer stops immediately when the system is disarmed, even mid-alarm

**FInished Task 6** https://wokwi.com/projects/472400499410393089

---

## Task 7 - Spot-the-Bug Worksheet (Extension)



**Round 1:**
```cpp
void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Button pressed");
  }
}


```
**Fixed**

<details><summary>Answer</summary>With <code>INPUT_PULLUP</code>, a press reads <code>LOW</code>, not <code>HIGH</code> — the condition has the logic backwards, so the LED lights when the button is released instead of pressed.</details>

**Round 2:**
```cpp
void loop() {
  int potValue = analogRead(potPin);
  int brightness = map(potValue, 0, 4095, 0, 255);
  analogWrite(ledPin, brightness);
}
```
**Fixed**

<details><summary>Answer</summary>The <code>map()</code> arguments are reversed — the potentiometer reading is 0–4095 and needs to map <em>into</em> 0–255 for <code>analogWrite()</code>. It should be <code>map(potValue, 0, 4095, 0, 255)</code>.</details>

**Round 3:**
```cpp
void loop() {
  int motionDetected = digitalRead(PIR_PIN);

  if (motionDetected == HIGH) {
    digitalWrite(BUZZER_PIN, HIGH);
  }
}
```
**Fixed**

<details><summary>Answer</summary><code>=</code> is assignment, not comparison — this always sets the condition true regardless of the sensor. It should be <code>if (motionDetected == HIGH)</code>.</details>

**Round 4:**
```cpp
void loop() {
  float humidity = dht.readHumidity();
  float temperatureC = dht.readTemperature();
  if (isnan(humidity) || isnan(temperatureC)) {
    Serial.println("Failed to read from DHT sensor!");
  }
  Serial.print("Temp: ");
  Serial.println(temperatureC);
  
  delay(2000);
}
```
**Fixed**
<details><summary>Answer</summary>Two problems: there's no <code>isnan()</code> check for a failed read, and the DHT22 is polled every 100 ms — it can only be read reliably about once every 2 seconds, so this will frequently return stale or invalid data.</details>

**Round 5:**
```cpp
void loop() {
  int redButton = digitalRead(redButtonPin);

  if (redButton == LOW) {
    digitalWrite(redLED, HIGH);
  } else if (greenButton == LOW) {
    digitalWrite(greenLED, HIGH);
  }
}
```
**Fixed**
<details><summary>Answer</summary>Both branches check the exact same condition (<code>redButton == LOW</code>) — the second, unreachable branch was meant to check a different variable, e.g. <code>greenButton == LOW</code>.</details>

**Round 6:**
```cpp
bool systemArmed = false;
int lastButtonState = HIGH;

void loop() {
  int buttonState = digitalRead(ARM_BUTTON_PIN);

  if (buttonState == LOW && lastButtonState == HIGH) {
    systemArmed = !systemArmed;
  }
  lastButtonState = buttonState;
  Serial.println(systemArmed);
  delay(10);
}
```
**Fixed**
<details><summary>Answer</summary>There's no edge detection (no <code>lastButtonState</code> comparison) — while the button is held down, this toggles <code>systemArmed</code> on almost every single loop iteration (every ~10 ms), instead of once per physical press.</details>

**Round 7:**
```cpp
void loop() {
  int motionDetected = digitalRead(PIR_PIN);
  bool afterHours = true;

  if (motionDetected == HIGH) {
    if (afterHours) {
      digitalWrite(BUZZER_PIN, HIGH);
    }
  }
}
```
**Fixed**
<details><summary>Answer</summary>The nested <code>if (afterHours)</code> block is empty — <code>digitalWrite(BUZZER_PIN, HIGH)</code> sits outside it, so the buzzer fires on motion alone and <code>afterHours</code> has no effect at all. The buzzer line needs to move inside the nested block.</details>

**Self-check:** How many did you spot correctly before looking?

---

## Task 8 - Smart Environment Monitor 

**Scenario:**

A community garden shed needs one combined monitoring system: it should only run its full logic while "armed" via a button, warn about unwanted humans after hours using the PIR sensor, warn about greenhouse conditions using the DHT22, and let a potentiometer act as a manual override for the alert buzzer's volume/duration via PWM-controlled intensity — bringing together every input type and every selection structure from this week.

**Components:**

- ESP32-S3
- Push button
- PIR sensor
- DHT22 sensor with 10 kΩ resistor,
- Potentiometer
- Passive buzzer
- LED
- Resistors
- Jumper wires

> 
> **Wiring**
> 

```mermaid
flowchart LR
    ESP32["ESP32-S3"]

    ESP32 -- GPIO4 --- Button["Arm Button (+)"]
    Button --- GND1["GND"]

    ESP32 -- GPIO5 --- PIR["PIR Sensor (OUT)"]

    ESP32 -- GPIO6 --- DHT["DHT22 DATA"]
    DHT --- R1["10 kΩ Resistor"]
    R1 --- V33["3.3V"]

    ESP32 -- GPIO1 --- Pot["Potentiometer Wiper"]

    ESP32 -- GPIO7 --- Buzzer["Buzzer (+)"]

    ESP32 -- GPIO2 --- R2["220 Ω Resistor"]
    R2 --- Anode["Status LED Anode (+)"]
    Anode --- Cathode["Status LED Cathode (−)"]
    Cathode --- GND2["GND"]
```

**Program requirements:**
1. **Edge-detected toggle (nested `if`):** A single press of the arm button (debounced, edge-detected as in Task 6) flips `bool systemArmed`. The status LED should be on whenever `systemArmed` is `true`.
2. **Nested `if` for motion:** Only when `systemArmed` is `true`, read the PIR sensor. Nest a second check inside: `if (motionDetected == HIGH && afterHours)` sounds the buzzer (use a `bool afterHours` flag set manually, as in Task 3).
3. **`if / else if / else` for climate:** Read the DHT22 (no more than once every 2 seconds) and classify temperature as `"Cold"`, `"Comfortable"`, or `"Hot"` as in Task 5, printing the result and the humidity to Serial regardless of the armed state.
4. **Potentiometer override:** Read the potentiometer and `map()` it to a 0–255 range. Use this value as the buzzer's PWM intensity (`analogWrite`) whenever the motion alert is active, so turning the knob controls how loud/intense the alert is.
5. Handle a failed DHT22 read with `isnan()` before using its values in any condition.

**Suggested sequence in `loop()`:**
1. Check the arm button for a fresh press (edge detection) → toggle `systemArmed`, update the status LED.
2. If `systemArmed`: read the PIR sensor; if motion **and** `afterHours`, read the potentiometer and drive the buzzer with `analogWrite()` at that intensity; otherwise keep the buzzer off.
3. Independently of the armed state: read the DHT22, check for a failed read, then classify and print the temperature band and humidity.

**Check yourself:**
- [ ] The arm button toggles `systemArmed` once per physical press, not repeatedly while held
- [ ] The PIR sensor and buzzer logic only run when `systemArmed` is `true` (nested correctly)
- [ ] The buzzer only activates when motion **and** `afterHours` are both true
- [ ] The potentiometer's mapped value visibly changes the buzzer's PWM intensity
- [ ] The DHT22 read is guarded with `isnan()` and never polled faster than every 2 seconds
- [ ] Temperature classification uses `if / else if / else` with conditions ordered correctly (narrowest first)

**Finished** https://wokwi.com/projects/472489745688589313

---

## Questions

Answer these in your own words before moving on:
1. Why does `INPUT_PULLUP` read `LOW` when pressed, while `INPUT_PULLDOWN` reads `HIGH` when pressed?
  ```
  INPUT_PULLUP is HIGH by default, but when you press the button it connects to GND, making it LOW, INPUT_PULLDOWN is LOW by default, but when you press the button it connects to VCC, making it HIGH

  ```
2. What's the practical difference between `if`, `if / else`, and `if / else if / else` in terms of how many branches can run per loop?
  ```
  In every language, if and else are used to work when a condition is fulfilled. You use else when something is the opposite of the if, so it gives two conditions. else if is used when there are multiple conditions that need to be identified, so the program won't have any unexpected answers. These conditions can also be nested inside each other when needed.

  ```
3. Why does `&&` require both conditions to be true, while `||` only needs one? Give a real-world example of each from this activity.
  ```
  && is used when your program needs two specific values to be true in order for it to run. If one is false and the other is true, it won't run, only if they are both true. For example, the buzzer activates when motion is detected AND it is after hours. || is used when you have two conditions and at least one of them needs to be true. For example, the buzzer could activate if motion is detected OR it is after hours.

  ```
4. In Task 6/8, why is edge detection (comparing to `lastButtonState`) necessary instead of just checking `if (buttonState == LOW)` on its own?

  ```
 We use the lastButtonState to save the previous button state, while if we use buttonState == LOW, it will just read as LOW. The lastButtonState lets us know when the button has changed from HIGH to LOW, so it only detects a new press instead of repeatedly detecting the button while it is held.

  ```
5. Why must the most specific condition be checked first in an `if / else if / else` chain?
  ```
  Because the program checks the condition from top to bottom, so checking the most specific condition first ensures the correct condition is chosen and prevents an unexpected answer.

  ```

6. Why does the DHT22 need to be read with `millis()`-style timing instead of a short `delay()`, unlike a button or PIR sensor?
  ```
    The DHT22 needs to be read after a certain amount of time, so we use millis() to remember when it was last read and check when it is time to read it again. Unlike delay() millis() doesn't stop the board from doing other things while waiting to read the DHT22.

  ```



