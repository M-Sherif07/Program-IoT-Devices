# Task - Wi-Fi Access Point Mode: Junior Sports Scoreboard Controller

Build a scoreboard controller that creates its own Wi-Fi network at a sports ground with no internet, opens its control page automatically on the scorer's phone, keeps the score live on every connected phone, and protects the reset with a PIN. You'll apply everything from the [Access Point Mode Resource](../Resource/Week-10-WiFi-Access-Point-Mode.md) and [Activity](../Activity/10_Activity_10_WiFi_Access_Point_Mode.md) to a new scenario.

**Scenario:**

A junior football club plays on a council oval with no Wi-Fi and patchy mobile coverage. They want a small scoreboard controller that runs the ESP32-PICO in **Access Point mode**. The scorer and the referee join its network on their phones (no more than **two** devices at once), and the control page must **open by itself** when they join. Both phones must show the same live score, updating every second. Tapping **Home +1** or **Away +1** adds a goal: that team's LED flashes and a horn sounds, without the controller ever freezing. Only someone who knows the club's **PIN** can reset the score from a phone, and a physical button on the box resets it too. For the club's records, every phone that joins or leaves is logged in the Serial Monitor.

**Components required:**
- ESP32-PICO development board
- 1 × LED (home goal light)
- 1 × LED (away goal light)
- 2 × 220 Ω resistor
- 1 × passive buzzer (horn)
- 1 × push button (local reset)
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
> | Home LED (through a 220 Ω resistor; LED cathode → GND) | Digital output | GPIO ___ |
> | Away LED (through a 220 Ω resistor; LED cathode → GND) | Digital output | GPIO ___ |
> | Passive buzzer + (buzzer − → GND) | Digital output (`tone()`) | GPIO ___ |
> | Reset button (other leg → GND, `INPUT_PULLUP`) | Digital input | GPIO ___ |

**How to draw the schematic:** before building the circuit, draw it in [Circuit Studio](https://circuit-studio.pages.dev/).
- Include every component in the list above: the ESP32-PICO, both LEDs with their 220 Ω resistors, the passive buzzer and the push button.
- **Choose your own GPIO pins** and record them in the **Your pin choices** table above. Draw the same pins on the schematic and use the same numbers as named constants in your code (`homeLedPin`, `awayLedPin`, `buzzerPin`, `resetButtonPin`). Connect every ground to GND.
- Follow the ESP32-PICO pin rules:
  - the LEDs and buzzer can't use input-only GPIO 34-39
  - never use GPIO 1, 3, 6-11, 16 or 17
  - avoid boot-mode pins GPIO 0, 2, 5, 12 and 15
  - safe digital choices are GPIO 4, 13, 14, 18, 19, 21-23, 25-27, 32 and 33
- Export it with **Export → Schematic image** and **Export → Share link**, and paste both into the Schematic box above.

**Requirements:** the finished program must meet all of the following.
- **Access point:** runs in **Access Point mode** with the SSID `"Scoreboard-<your initials>"`, a WPA2 password of 8+ characters, channel `AP_CHANNEL = 6`, visible, and at most `AP_MAX_CLIENTS = 2` devices. Moves the access point to **`192.168.50.1`** with `WiFi.softAPConfig()`. Checks the return values of both `softAP()` and `softAPConfig()`. Replace your real password with `"YOUR_AP_PASSWORD"` before you submit.
- **Captive portal:** a `DNSServer` answers every name with `WiFi.softAPIP()` (started **after** `softAPConfig()`), and every unknown path is redirected (`302`) to the control page, so it opens by itself when a phone joins.
- **Client log:** an event function handles `ARDUINO_EVENT_WIFI_AP_STACONNECTED` and `ARDUINO_EVENT_WIFI_AP_STADISCONNECTED` by only copying the MAC address into `volatile` variables and setting `volatile` flags. `loop()` prints `[JOIN] <mac> (devices: <n>)` and `[LEAVE] <mac> (devices: <n>)`.
- **Control page (`/`):** stored in a raw string literal. Its JavaScript fetches `/api/score` every 1000 ms and shows both scores in large text and the number of connected devices, or `no connection` if a fetch fails. It has **Home +1** and **Away +1** buttons, and a PIN box with a **Reset** button. Any error message from the server is shown on the page.
- **`/api/score` (GET):** replies with valid JSON `{"home":<n>,"away":<n>,"clients":<n>}`.
- **`/api/goal?team=home|away` (POST only):** adds one goal to that team. A missing or unknown `team` gets `400`. A score can't go above `MAX_SCORE = 99` (reply `400` if it would). Replies with the new score as JSON.
- **`/api/reset?pin=NNNN` (POST only):** resets both scores to 0 only if `pin` matches `RESET_PIN` (a 4-digit constant). A missing PIN gets `400`; a wrong PIN gets `403` with the message `Wrong PIN`.
- **Goal effects:** on each goal, that team's LED stays on for `FLASH_DURATION = 300` ms and the horn plays `HORN_FREQUENCY = 800` Hz for `HORN_DURATION = 500` ms, all timed with `millis()` (`tone()`/`noTone()`, never `delay()`). A goal for the other team during a flash must not cut the first team's flash short.
- **Local reset button:** a debounced press (50 ms, `millis()`) resets both scores and prints `Score reset (button)`.
- Every score change prints the new score to Serial, e.g. `GOAL home -> 2 : 1`.
- Every function has a one-line comment above it saying what it does.
- `loop()` contains no `delay()` anywhere.

**Extension (optional):**
- Save the score with `Preferences` every time it changes (only then, never in `loop()`), and load it in `setup()`, so a flat battery at half-time doesn't lose the score.
- Add a 25-minute half-time countdown: **Start**/**Pause** buttons on the page (POST), the time left shown on the page from the JSON, and a long horn when it reaches zero, all without `delay()`.

**Testing on the real board:**
1. Join from your phone: the control page must open by itself. The Serial Monitor shows `[JOIN]`.
2. Join from a second device: both pages show the same score within a second of each goal. A third device must be refused.
3. Tap **Home +1** and then quickly **Away +1**: both LEDs flash for their full 300 ms and the horn sounds, while both pages keep updating.
4. Try a reset with a wrong PIN (`Wrong PIN` shown), then the right PIN. Then score some goals and press the physical button.
5. Use a laptop on the network to send bad requests and screenshot the replies: `/api/goal?team=visitors` (POST), `/api/goal` with no team (POST), `/api/reset` with no PIN (POST), and `/api/goal?team=home` typed into the address bar (a GET, which must **not** add a goal). On a Mac or Linux terminal, `curl -X POST "http://192.168.50.1/api/goal?team=visitors"` sends a POST.

>
> Evidence: _(attach a video showing the page opening by itself, goals scored from two phones with the LEDs and horn, a wrong-PIN and right-PIN reset, and the physical reset button; plus screenshots of the Serial Monitor (joins, leaves, goals), the third device being refused, and the four bad-request replies)_

### Questions

- Why is Access Point mode the right choice for this scoreboard, rather than Station mode? What would you need to change if the club later wanted the scores uploaded to a website as well?
  ```


  ```

- Your page sets `<input type="number">` limits and only shows valid teams as buttons. Why does the ESP32-PICO still have to check `team` and `pin` itself? Give an example from your testing.
  ```


  ```

- Why are `/api/goal` and `/api/reset` POST-only? Describe what could happen during a match if adding a goal were a plain GET link.
  ```


  ```

- Explain how your code makes sure one team's LED flash isn't cut short by a goal for the other team, and why the horn and flashes must not use `delay()` in a web-server sketch.
  ```


  ```

- The club's PIN and access point password are written in the sketch. Describe two weaknesses of the security in this design, and one improvement for each.
  ```


  ```

### Spot-the-Bug Worksheet

**Format:** For each round, read the snippet and write down what's wrong **before** revealing the answer.

**Round 1:**
```cpp
void setup() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP("Scoreboard-AB", "");
  server.on("/api/reset", HTTP_POST, handle_reset);
}
```
<details><summary>Answer</summary>The password is `""`, so the scoreboard runs an **open** network. Anyone at the ground can join from their phone and add goals, and with only two client slots a spectator could even take a slot so the referee can't join. A scoreboard that controls something people rely on needs a WPA2 password of 8+ characters (plus the PIN for reset).</details>

**Round 2:**
```cpp
void handle_reset() {
  if (server.arg("pin") = RESET_PIN) {
    homeScore = 0;
    awayScore = 0;
  }
  server.send(200, "application/json", "{\"reset\":true}");
}
```
<details><summary>Answer</summary>`=` **assigns** instead of comparing (`==`). It sets the copy of the PIN to `RESET_PIN`, and a non-empty `String` counts as `true`, so **any** PIN (or no PIN) resets the score. It also always replies `200`. Use `server.arg("pin") == RESET_PIN`, reply `400` if the PIN is missing and `403` if it's wrong.</details>

**Round 3:**
```cpp
server.on("/api/goal", handle_goal);
// ...and in the page:
// <a href="/api/goal?team=home">Home +1</a>
```
<details><summary>Answer</summary>Adding a goal is a **GET** link, and the route accepts any method. Browsers can load links early, the back button or a refresh can repeat the request, and the address can stay in the browser history, so goals get added that nobody scored. Register the route with `HTTP_POST` and call it from the page with `fetch(..., { method: 'POST' })`.</details>

**Round 4:**
```cpp
void handle_goal() {
  // ...team checked, score increased...
  digitalWrite(homeLedPin, HIGH);
  tone(buzzerPin, 800);
  delay(500);
  noTone(buzzerPin);
  digitalWrite(homeLedPin, LOW);
  server.send(200, "application/json", scoreJson());
}
```
<details><summary>Answer</summary>`delay(500)` inside a handler blocks the whole sketch: the web server can't answer the other phone, the DNS server and button aren't checked, and the reply to this phone is delayed by half a second. Start the flash and horn, save the start times with `millis()`, reply straight away, and switch them off from separate update functions in `loop()`.</details>

**Round 5:**
```cpp
String scoreJson() {
  return "{\"home\":" + String(homeScore) + "\"away\":" + String(awayScore) + "}";
}
```
<details><summary>Answer</summary>There's no comma between the two fields, so the text is `{"home":2"away":1}`, which isn't valid JSON. The page's `response.json()` fails and shows `no connection`, even though the scoreboard is working. Add the comma: `... + String(homeScore) + ",\"away\":" + ...`.</details>

**Round 6:**
```cpp
void setup() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD, AP_CHANNEL, 0, AP_MAX_CLIENTS);
  dnsServer.start(53, "*", WiFi.softAPIP());
  WiFi.softAPConfig(apIP, apIP, subnet);   // move to 192.168.50.1
}
```
<details><summary>Answer</summary>The DNS server is started **before** `softAPConfig()`, so `WiFi.softAPIP()` is still the default `192.168.4.1`. Every name is answered with an address that no longer exists on this network, so the captive portal never opens. Call `softAPConfig()` first, then start the DNS server with `WiFi.softAPIP()`.</details>
