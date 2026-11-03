# Task - Wi-Fi Station Mode: Smart Locker Connection Monitor

Build a connection monitor for a smart locker. It runs in Station mode at a fixed address on the network, shows its connection and signal strength on LEDs, and logs every Wi-Fi drop with the reason it happened. You'll use what you learned in the [Station Mode Resource](../Resource/Week-10-WiFi-Station-Mode.md) and [Activity](../Activity/10_Activity_10_WiFi_Station_Mode.md) to a new scenario.

**Scenario:**

A school library is installing smart lockers that students unlock from an app. Each locker controller is an ESP32-PICO that joins the library's Wi-Fi in Station mode. The library's front-desk computer sends unlock commands straight to each locker, so every locker must always be at the same IP address and must appear in the router's device list with a clear hostname. The lockers have been dropping off the network now and then, and nobody knows why. Your controller must show at a glance whether it's online and whether the signal is weak. It must beep when the connection drops and keep a record of the last five drops, with the reason for each. A staff member presses a button to print a full health report to the Serial Monitor. The controller must never freeze while it waits for Wi-Fi.

**Components required:**
- ESP32-PICO development board
- 1 × green LED (online status)
- 1 × red LED (weak-signal warning)
- 2 × 220 Ω resistor
- 1 × passive buzzer (drop alert)
- 1 × push button (print report)
- Jumper wires, breadboard

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
> **Your pin choices** (choose any suitable GPIO, following the pin rules, and use the same numbers in your schematic and your code):
>
> | Connection | Pin type needed | Your GPIO |
> |---|---|---|
> | Green LED (through a 220 Ω resistor; LED cathode → GND) | Digital output | GPIO ___ |
> | Red LED (through a 220 Ω resistor; LED cathode → GND) | Digital output | GPIO ___ |
> | Passive buzzer + (buzzer − → GND) | Digital output (`tone()`) | GPIO ___ |
> | Push button (other leg → GND, `INPUT_PULLUP`) | Digital input | GPIO ___ |

**How to draw the schematic:** before building the circuit, draw it in [Circuit Studio](https://circuit-studio.pages.dev/).
- Include every component in the list above: the ESP32-PICO, both LEDs with their 220 Ω resistors, the passive buzzer and the push button.
- **Choose your own GPIO pins** and record them in the **Your pin choices** table above. Draw the same pins on the schematic and use the same numbers as named constants in your code (`greenLedPin`, `redLedPin`, `buzzerPin`, `buttonPin`). Connect every ground to GND.
- Follow the ESP32-PICO pin rules:
  - the LEDs and buzzer can't use input-only GPIO 34-39
  - never use GPIO 1, 3, 6-11, 16 or 17
  - avoid boot-mode pins GPIO 0, 2, 5, 12 and 15
  - safe digital choices are GPIO 4, 13, 14, 18, 19, 21-23, 25-27, 32 and 33
- Export it with **Export → Schematic image** and **Export → Share link**, and paste both into the Schematic box above.

**Before you start:** connect once without a static IP and note your network's IP address, gateway, subnet mask and DNS server. Choose a static IP in the same range that no other device uses.

**Requirements:** the finished program must meet all of the following.
- Runs in **Station mode** on a **2.4 GHz** network using named `SSID`/`PASSWORD` constants. Replace your real details with `"YOUR_WIFI_NAME"` / `"YOUR_WIFI_PASSWORD"` before you submit the code.
- Sets the hostname to `"locker-<your initials>"` with `WiFi.setHostname()` **before** `WiFi.mode()`, and sets a **static IP** with `WiFi.config()` **before** `WiFi.begin()`, checking `WiFi.config()`'s return value. The static IP, gateway and subnet must match your network.
- Is **event-driven**: registers an event function with `WiFi.onEvent()` before `WiFi.begin()`, with **no** blocking connection loop anywhere. The event function handles `ARDUINO_EVENT_WIFI_STA_GOT_IP` and `ARDUINO_EVENT_WIFI_STA_DISCONNECTED` by only setting `volatile` flags and saving the disconnect reason.
- **Green LED:** solid while online, blinking every 250 ms while offline (`millis()`).
- **Signal check:** every `SIGNAL_CHECK_INTERVAL = 2000` ms while online, reads `WiFi.RSSI()`. If it's below `WEAK_SIGNAL_THRESHOLD = -70` dBm, the red LED turns on; otherwise it turns off. Print `[WARN] Weak signal: <rssi> dBm` only at the moment the signal **becomes** weak, and `[OK] Signal recovered: <rssi> dBm` at the moment it recovers, not every 2 seconds. While offline, the red LED is off.
- **Drop alert and log:** each time a working connection drops (online → offline, **not** every failed retry), the buzzer beeps once for 200 ms using non-blocking `tone()`/`noTone()` timing, the drop counter goes up, and a `DropRecord` struct (`unsigned long uptimeSeconds`, `uint8_t reason`) is saved into an array that keeps the **last 5 drops** (when it's full, the oldest is overwritten).
- **Report button:** a debounced press (50 ms, `millis()`) prints a report with: SSID, IP address, gateway, hostname, RSSI with a quality word (`excellent`/`good`/`fair`/`weak`/`unusable`), the total number of drops, and each saved drop as `<uptime>s - reason <code>: <reasonText>`, oldest first. If offline, the report says so instead of printing the connection details.
- `reasonText()` decodes at least reasons 15, 200, 201, 202 and 204.
- A `maybe_reconnect()` safety net calls `WiFi.begin()` every 10 s while offline, with no `while`/`delay()` around it.
- Every function has a one-line comment above it saying what it does.
- `loop()` contains no `delay()` anywhere.

**Extension (optional):**
- Hold the button for more than 1 second to start an **asynchronous** scan instead of printing the report. When it finishes, print how many other networks share the locker's channel (`WiFi.channel(i) == WiFi.channel()`), and suggest the router may need a less crowded channel if there are 3 or more.
- Track the total time spent offline and add `Online <percent>% of the time since start-up` to the report.

**Testing on the real board:**
1. Restart the board three times: the IP must be the same each time. From a laptop on the same network, `ping` the static IP and screenshot the replies.
2. Move the board away from the router (or cover it with your hand/a metal tin) until the red LED comes on, then bring it back. Check the warning and recovery lines each appear once.
3. Switch your hotspot off and on at least **six** times. The buzzer must beep once per drop, and the report must show the correct total and only the **last five** drops.
4. Throughout, the green LED must keep blinking while offline and the button must keep working, proving nothing freezes.

>
> Evidence: _(attach a short video of the LEDs, buzzer and button working through a hotspot off/on cycle, plus screenshots of: the Serial Monitor showing the static IP and hostname, the `ping` replies from your laptop, the weak/recovered signal lines, and a report showing more than five drops with only the last five listed)_

### Questions

- The front-desk computer sends commands to each locker by IP address. Explain why a static IP (or a DHCP reservation) is needed here, when the Activity's Supabase loggers worked fine with DHCP.
  ```


  ```

- Your locker shows `WL_CONNECTED` and the green LED is on, but the front-desk computer can't reach it. List two possible causes related to how you configured Station mode.
  ```


  ```

- Why does the drop counter only increase on an online → offline change, and not on every `ARDUINO_EVENT_WIFI_STA_DISCONNECTED` event? What would the library staff see in the report if you got this wrong?
  ```


  ```

- Look at the reasons in your own drop log. What did each one tell you about the cause, and what would you recommend the library change if real lockers showed mostly reason 200?
  ```


  ```

- Why does the weak-signal warning only print when the signal *changes* between weak and OK, instead of every 2 seconds? How did you detect that change in your code?
  ```


  ```

### Spot-the-Bug Worksheet

**Format:** For each round, read the snippet and write down what's wrong **before** revealing the answer.

**Round 1:**
```cpp
IPAddress staticIP(192, 168, 1, 210);
IPAddress gateway(192, 168, 1, 210);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);
```
<details><summary>Answer</summary>The gateway is set to the locker's own address (`.210`) instead of the router's (usually `.1`). The locker joins the network and can still be reached by devices on the same network, but anything that has to go through the router (the internet, NTP, Supabase, or a computer on a different subnet) fails, because the locker is sending that traffic to itself. The gateway must be the router's real address, found by printing `WiFi.gatewayIP()` once with DHCP.</details>

**Round 2:**
```cpp
void setup() {
  Serial.begin(115200);
  WiFi.setHostname("locker-ab");
  WiFi.mode(WIFI_STA);
  WiFi.config(staticIP, gateway, subnet, dns);
  WiFi.begin(SSID, PASSWORD);
  WiFi.onEvent(onWiFiEvent);
}
```
<details><summary>Answer</summary>`WiFi.onEvent()` is registered **after** `WiFi.begin()`. If the connection completes quickly, the `GOT_IP` event fires before the function is registered, so it's never delivered: the locker is online but the code still thinks it's offline (green LED blinking, reconnect attempts every 10 s). Register the event function before `WiFi.begin()`.</details>

**Round 3:**
```cpp
bool gotIPFlag = false;
bool disconnectedFlag = false;
uint8_t lastReason = 0;
```
<details><summary>Answer</summary>These variables are written by the event function, which runs on the Wi-Fi system's own task, and read by `loop()`. Without `volatile`, the compiler may keep a copy of the value in a register and never notice the change, so `loop()` can miss events. All three must be declared `volatile`.</details>

**Round 4:**
```cpp
void handleWiFiFlags() {
  if (disconnectedFlag) {
    disconnectedFlag = false;
    if (online) {
      dropCount++;
      online = false;
      tone(buzzerPin, 1500);
      delay(200);
      noTone(buzzerPin);
    }
  }
}
```
<details><summary>Answer</summary>The 200 ms beep uses `delay()`, which freezes `loop()`: the green LED stops blinking and button presses are missed. Start the tone and save the start time with `millis()` (e.g. `beepStart = millis(); beeping = true;`), then call `noTone()` from a separate update function once 200 ms have passed.</details>

**Round 5:**
```cpp
DropRecord drops[5];
int dropCount = 0;

void recordDrop(uint8_t reason) {
  drops[dropCount].uptimeSeconds = millis() / 1000;
  drops[dropCount].reason = reason;
  dropCount++;
}
```
<details><summary>Answer</summary>The array index is the total drop count, with no wrap-around. On the sixth drop it writes to `drops[5]`, which is past the end of the 5-element array, corrupting other variables or crashing the board. Use `drops[dropCount % 5]` (or a separate `nextSlot` index that wraps back to 0) so the oldest record is overwritten.</details>

**Round 6:**
```cpp
void checkSignal() {
  if (millis() - lastSignalCheck < SIGNAL_CHECK_INTERVAL) return;
  lastSignalCheck = millis();

  int rssi = WiFi.RSSI();
  if (rssi < WEAK_SIGNAL_THRESHOLD) {
    digitalWrite(redLedPin, HIGH);
    Serial.printf("[WARN] Weak signal: %d dBm\n", rssi);
  } else {
    digitalWrite(redLedPin, LOW);
  }
}
```
<details><summary>Answer</summary>Two bugs. First, it doesn't check that the locker is online before reading `WiFi.RSSI()`, so while offline it reads a meaningless value (and the red LED can be wrong). Second, the warning prints every 2 seconds while the signal stays weak, instead of only when it changes from OK to weak, and there's no "recovered" message. Keep a `signalWeak` variable and print only when the new result differs from it.</details>
