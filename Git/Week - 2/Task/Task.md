# Task — Digital Output and Functions (Week 2)

## In-Class Task

Build the LED circuit and write a program that blinks it using a function.

**Components required:**
- ESP32-S3 development board
- 1 × LED
- 1 × 220 Ω resistor
- Jumper wires
- Breadboard

**Requirements:**
- Wire the LED to GPIO13 through the 220 Ω resistor.
- Write a function that blinks the LED, and call it from `loop()`.
- Give the function parameters so it can blink at different speeds.
- Write a function that turns the LED on or off and returns its new state, and print that state to the Serial Monitor.

### Guided Questions
- Why does the LED behave the same whether the blink logic is in `loop()` directly or inside a function?
  ```


- What does `pinMode` do, and why do we only call it once, in `setup()`?
  ```


- Why does a function need parameters if you want it to blink at different speeds?
  ```


- Why does a function need a return type other than `void` if you want it to report back the LED's new state?
  ````


### Spot-the-Bug Worksheet



**Format:** For each round, read the snippet and write down what's wrong **before** revealing the answer.

**Round 1:**
```cpp
void blinkLED() {
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
  delay(1000);
}

void loop() {
  blinkLED;
}
```
<details><summary>Answer</summary>Missing parentheses when calling the function — it must be <code>blinkLED();</code>, not <code>blinkLED;</code>.</details>

**Round 2:**
```cpp
void blinkLED(int pin, int onTime) {
  digitalWrite(pin, HIGH);
  delay(onTime);
  digitalWrite(pin, LOW);
  delay(onTime);
}

void loop() {
  blinkLED(13);
}
```
<details><summary>Answer</summary><code>blinkLED</code> expects two arguments (<code>pin</code> and <code>onTime</code>), but only one was passed in — this won't compile.</details>

**Round 3:**
```cpp
void blinkLED() {
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
  delay(1000);
  return true;
}
```
<details><summary>Answer</summary><code>blinkLED()</code> is declared as <code>void</code>, meaning it shouldn't return a value — <code>return true;</code> won't compile. A <code>void</code> function can only use a bare <code>return;</code> (or no <code>return</code> at all).</details>

**Round 4:**
```cpp
int ledPin = 13;

void blinkLED() {
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
  delay(1000);
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  blinkLED();
}
```
<details><summary>Answer</summary><code>pinMode(ledPin, OUTPUT)</code> is missing from <code>setup()</code> — the code compiles and uploads, but the LED won't behave as an output correctly.</details>

**Round 5:**
```cpp
void blinkLED(int pin, int onTime) {
  digitalWrite(pin, HIGH);
  delay(onTime);
  digitalWrite(pin, LOW);
  delay(onTime);
}

void loop() {
  blinkLED(500, 13);
}
```
<details><summary>Answer</summary>The arguments are in the wrong order — <code>pin</code> comes first and <code>onTime</code> second, but this call passes <code>500</code> as the pin and <code>13</code> as the delay. It compiles fine, but tries to blink pin 500 for 13 ms instead of blinking pin 13 for 500 ms.</details>

**Round 6:**
```cpp
bool controlLED(bool turnOn) {
  digitalWrite(LED_PIN, turnOn);
}

void loop() {
  bool ledState = controlLED(true);
  Serial.println(ledState);
}
```
<details><summary>Answer</summary><code>controlLED</code> is declared to return a <code>bool</code>, but it never uses a <code>return</code> statement — it needs <code>return turnOn;</code> at the end, otherwise <code>ledState</code> won't reliably hold the value it's meant to.</details>

**Round 7:**
```cpp
void blinkLED(int pin, int onTime) {
  digitalWrite(pin, HIGH);
  delay(onTime);
  digitalWrite(pin, LOW);
  delay(onTime);
}

void loop() {
  blinkLed(13, 500);
}
```
<details><summary>Answer</summary>The function name is spelled <code>blinkLED</code> (capital LED) but called as <code>blinkLed</code> — C++ is case-sensitive, so this won't compile: <code>blinkLed</code> was never declared.</details>

**Self-check:** How many did you spot correctly before looking?
