# Task - Wi-Fi AP + Station Mode: Community Pantry Fridge Monitor

Build a fridge monitor that logs to the cloud through the building's Wi-Fi, keeps its own network for volunteers' phones, raises a local alarm even when the internet is down, and never loses readings during an outage. You'll apply everything from the AP + Station Mode Resource and Activity to a new scenario.

> **Choose your board:** build it on the **ESP32-PICO** or the **Raspberry Pi Pico 2 W**. Where the code differs, the requirements say what each board uses. The Resource's **Choose your board** table lists every difference.

**Scenario:**

A community pantry keeps donated food in a fridge in a church hall. Food safety rules say the fridge must stay at or below **5 °C**, and the pantry's coordinator wants a record they can show the council inspector. The hall has Wi-Fi, but the internet drops out a few times a week.

The monitor runs the microcontroller in **AP + Station mode**:
- On the **station side** it joins the hall's Wi-Fi and logs a reading to **Supabase** every minute. Readings taken while the internet is down are queued and uploaded later, marked as late.
- On the **access point side** it runs its own network, `"Pantry-<your initials>"`, so volunteers can check the fridge on their phones at any time, internet or not.
- If the fridge gets too warm, a red LED flashes and a buzzer beeps, **whether or not** the internet is up. A volunteer can mute the buzzer for 10 minutes with the button on the box or from the dashboard, but the LED keeps flashing until the temperature is back to normal.
- The alarm temperature can only be changed by someone standing at the fridge, joined to the monitor's own network. Anyone on the hall's Wi-Fi can view the dashboard, but they get `403` for settings.

**Components required:**
- ESP32-PICO or Raspberry Pi Pico 2 W development board
- 1 × DHT11 sensor (3-pin board; stands in for the fridge probe)
- 1 × red LED (alarm)
- 1 × green LED (cloud online)
- 2 × 220 Ω resistor
- 1 × passive buzzer
- 1 × push button (mute)
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
> **Your pin choices** (choose any suitable pin, following the pin rules for your board, and use the same numbers in your schematic and your code):
>
> | Connection | Pin type needed | Your pin |
> |---|---|---|
> | DHT11 OUT/DATA (+ → 3.3V, − → GND) | Digital input/output | ___ |
> | Red alarm LED (through a 220 Ω resistor; LED cathode → GND) | Digital output | ___ |
> | Green cloud LED (through a 220 Ω resistor; LED cathode → GND) | Digital output | ___ |
> | Passive buzzer + (buzzer − → GND) | Digital output (`tone()`) | ___ |
> | Mute button (other leg → GND, `INPUT_PULLUP`) | Digital input | ___ |

**How to draw the schematic:** before building the circuit, draw it in [Circuit Studio](https://circuit-studio.pages.dev/).
- Include every component in the list above: the microcontroller, the DHT11, both LEDs with their 220 Ω resistors, the passive buzzer and the push button.
- **Choose your own pins** and record them in the **Your pin choices** table above. Draw the same pins on the schematic and use the same numbers as named constants in your code (`dhtPin`, `alarmLedPin`, `cloudLedPin`, `buzzerPin`, `muteButtonPin`). Connect every ground to GND.
- Follow the pin rules for your board:
  - **ESP32-PICO:** the LEDs and buzzer can't use input-only GPIO 34-39; never use GPIO 1, 3, 6-11, 16 or 17; avoid boot-mode pins GPIO 0, 2, 5, 12 and 15; safe digital choices are GPIO 4, 13, 14, 18, 19, 21-23, 25-27, 32 and 33
  - **Raspberry Pi Pico 2 W:** never use GP23, GP24, GP25 or GP29 (wired to the Wi-Fi chip); safe digital choices are GP2-GP22; use the GP number in your code, not the physical pin number
- Export it with **Export → Schematic image** and **Export → Share link**, and paste both into the Schematic box above.

**Set up the Supabase database (one time):** in the **SQL Editor** of your Supabase project, run:
```sql
create table fridge_log (
  id          bigint generated always as identity primary key,
  created_at  timestamptz not null default now(),   -- when the row arrived
  temperature real,
  humidity    real,
  alarm       boolean,                              -- was the alarm on at the time?
  uptime_s    bigint,                               -- when it was measured
  buffered    boolean default false                 -- true if it waited in the queue
);

alter table fridge_log enable row level security;

create policy "Device can insert fridge rows"
  on fridge_log for insert
  to anon
  with check (true);
```
Use the **publishable** (anon) key on the device, never the `service_role` key.

**Testing tip:** a DHT11 can't go below 0 °C and the room is warmer than a fridge anyway, so test the alarm by setting the alarm temperature **below** the room temperature (e.g. 15 °C on a 22 °C day), or by holding the sensor between your fingers to warm it past a setting just above room temperature.

**Requirements:** the finished program must meet all of the following.
- **Both sides:** `WIFI_AP_STA`. The access point `"Pantry-<your initials>"` uses an 8+ character password and channel `AP_CHANNEL`, set to **your router's channel** (find it by printing `WiFi.channel()` once, as in Activity Task 1), and has its address set to **`192.168.7.1`** with `softAPConfig()` (called before `softAP()`). Check both return values. The station side joins your router with `SSID`/`PASSWORD` constants, with **no** blocking `while` loop. On the Pico 2 W, call `WiFi.setTimeout(8000)`, start the AP and web server first, and call `WiFi.begin()` last. Replace every real credential with placeholders before you submit.
- **Noticing Wi-Fi changes:**
  - **ESP32:** one event function handles `STA_GOT_IP`, `STA_DISCONNECTED`, `AP_STACONNECTED` and `AP_STADISCONNECTED` with only `volatile` flags (and the reason).
  - **Pico 2 W:** a `checkWiFiChanges()` function runs every 500 ms and reacts only when the station side's state (`WiFi.status() == WL_CONNECTED`) or the phone count (`WiFi.softAPgetStationNum()`) has changed. A failed `WiFi.begin()` reports the reason from `cyw43_wifi_link_status()`.
  - On both, `loop()` prints a line for each change: the router-side IP when online, the reason when lost or a join fails, and the phone count on join/leave.
- **Sensor:** the DHT11 is read every `READ_INTERVAL = 2000` ms; the latest good reading is kept.
- **Alarm (works offline):** while the temperature is **above** `alarmThreshold`, the red LED flashes (250 ms on / 250 ms off) and the buzzer beeps `BEEP_FREQUENCY = 1000` Hz for `BEEP_DURATION = 200` ms every `BEEP_INTERVAL = 2000` ms, all timed with `millis()`. When the temperature drops back to the threshold or below, both stop. The alarm code must not depend on `WiFi.status()` in any way. Print `[ALARM] ON` / `[ALARM] cleared` only when the state changes.
- **Mute:** a debounced (50 ms) press of the mute button, or a POST to `/api/mute` from either network, silences the buzzer for `MUTE_DURATION = 600000` ms (10 minutes). The LED keeps flashing while muted. Muting when there's no alarm does nothing harmful, and the device must **not** start up muted.
- **Cloud with offline queue:** every `LOG_INTERVAL = 60000` ms, a row with `temperature`, `humidity`, `alarm`, `uptime_s` and `buffered` goes to `fridge_log` over HTTPS (both key headers, success only on `201`, `http.end()` always). Send directly only if online with an empty queue; otherwise enqueue. The queue is a circular buffer of `QUEUE_SIZE = 30` structs that drops (and counts) the oldest when full. While online, upload one queued row every `FLUSH_INTERVAL = 3000` ms, removing it only on `201`.
- **Green LED:** on while the station side is connected. **Reconnect:** while offline, call `WiFi.begin()` every `RECONNECT_CHECK_INTERVAL = 30000` ms. On the Pico 2 W each attempt waits up to 8 s, so after a failed attempt call `stopStation()`; the alarm keeps its last state during the attempt and catches up straight afterwards.
- **Dashboard (`/`, both networks):** a raw-string page that fetches `/api/status` every 2 s and shows temperature, humidity, alarm ON/off, muted (with minutes left), the alarm temperature, cloud online, queued and dropped counts, and phones joined. It has a **Mute 10 min** button (POST) and a link to `/settings`. `/api/status` must be **valid JSON** even before the first reading (`null`, not `nan`).
- **Settings (AP side only):** `/settings` shows a form with the alarm temperature, sent with `method='post'` to `/settings/save`. Both routes reply `403` to requests from the router side. The value must be a whole number from **0 to 15**, otherwise `400` with a message. A valid value is used straight away and saved in flash (ESP32: `Preferences`; Pico 2 W: `EEPROM` plus `EEPROM.commit()`). Invalid values are never saved. The saved value is loaded in `setup()`, with a default of **5** if nothing has been saved yet.
- Every function has a one-line comment above it saying what it does.
- `loop()` contains no `delay()` anywhere.

**Extension (optional):**
- Sync the real time with NTP once the station side is online, and send a real `measured_at` timestamp with each row (add the column), using `uptime_s` only for readings taken before the first sync.
- Add a **Last 10 readings** table to the dashboard, served from a second JSON endpoint and kept in RAM, so volunteers can see the trend without the internet.

**Testing on the real board:**
1. Join `Pantry-<initials>` from a phone and open `http://192.168.7.1`. From a laptop on your router, open the router-side address printed in the Serial Monitor. Both show the dashboard.
2. Open `/settings` from both: the phone gets the form, the laptop gets `403`. Set the alarm temperature below room temperature on the phone and save. The alarm starts.
3. Press the mute button: the beeping stops, the LED keeps flashing, and the dashboard shows the minutes left. Raise the alarm temperature again and check the alarm clears.
4. Switch the router off for at least **5 minutes** with the alarm on. The alarm, mute and dashboard must all keep working on the AP side, and the queue count must rise. Switch it back on: the queue uploads one row every 3 s.
5. Restart the board: the saved alarm temperature is loaded, and the alarm is **not** muted.

>
> Evidence: _(attach a video covering tests 1-5; plus screenshots of the Serial Monitor, the `403` on `/settings` from the router side, a `400` for an invalid alarm temperature, and the `fridge_log` table showing late rows with `buffered = true`, increasing `uptime_s`, and `alarm = true` rows)_

### Questions

- Explain why the monitor uses AP + Station mode rather than Station mode alone. Use a specific moment from your testing to support your answer.
  ```


  ```

- You set `AP_CHANNEL` to your router's channel. What would have happened to volunteers' phones if you'd left it on channel 1 and your router was on channel 11? When else in your program does the radio leave the AP's channel?
  ```


  ```

- The coordinator will show the `fridge_log` table to a council inspector. Explain how a row uploaded late can be identified, how you'd work out roughly when it was really measured, and what the inspector would see for a 5-minute internet outage.
  ```


  ```

- Why is changing the alarm temperature restricted to the AP side, while muting is allowed from both networks? Explain how your code tells the two networks apart.
  ```


  ```

- A volunteer says: "When the internet goes down, does the alarm stop working?" Answer them, and point to the parts of your design that make sure it doesn't.
  ```


  ```

### Spot-the-Bug Worksheet

**Format:** For each round, read the snippet and write down what's wrong **before** revealing the answer.

**Round 1:**
```cpp
void loop() {
  server.handleClient();
  readSensor();
  if (WiFi.status() == WL_CONNECTED) {
    updateAlarm();
    logReading();
    flushQueue();
  }
}
```
<details><summary>Answer</summary>`updateAlarm()` is inside the `WiFi.status() == WL_CONNECTED` block, so the alarm **stops working whenever the internet or router is down**, which is exactly when nobody is watching the cloud data. Local safety features must never depend on the station side. Call `updateAlarm()` on every pass, outside the `if`. (Queued logging should also run outside it, because `logReading()` has to enqueue readings while offline.)</details>

**Round 2:**
```cpp
void handle_settings() {
  if (!cameFromAP()) {
    server.send(403, "text/plain", "Settings are only available on the pantry network");
    return;
  }
  server.send(200, "text/html", build_settings_page(""));
}

void handle_settings_save() {
  int value = server.arg("threshold").toInt();
  alarmThreshold = value;
  prefs.putInt("threshold", alarmThreshold);   // (Pico: EEPROM.put() + EEPROM.commit())
  server.sendHeader("Location", "/settings");
  server.send(303);
}
```
<details><summary>Answer</summary>Only the **form page** checks `cameFromAP()`; the route that actually **saves** doesn't. Anyone on the hall's Wi-Fi can skip the form and send a POST to `/settings/save` directly (e.g. with `curl`) to change the alarm temperature. The value isn't validated either, so `abc` becomes 0 °C and `99` disables the alarm. Every route that changes a setting must do its own `cameFromAP()` check and validation.</details>

**Round 3:**
```cpp
const int AP_CHANNEL = 1;   // the hall router is on channel 11

void setup() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(AP_SSID, AP_PASSWORD, AP_CHANNEL);   // phones stay on channel 1
  WiFi.begin(SSID, PASSWORD);
}
```
<details><summary>Answer</summary>The comment is wrong. The microcontroller has one radio, and the station side must use the router's channel, so as soon as it joins the router the AP is moved to channel 11. Phones joined on channel 1 drop off and have to rejoin, and it happens again after every reconnect. Set `AP_CHANNEL` to the router's channel (11) so nothing has to move.</details>

**Round 4:**
```cpp
void enqueue(const Reading& reading) {
  int tail = (queueHead + queueCount) % QUEUE_SIZE;
  queue[tail] = reading;
  queueCount++;
}
```
<details><summary>Answer</summary>There's no check for a full queue. After 30 readings, `tail` wraps round and overwrites the **oldest** unsent reading at `queueHead`, while `queueCount` keeps growing past 30, so the queue claims to hold more readings than it can and sends overwritten ones more than once. When full, move `queueHead` forward and reduce `queueCount` first (and count the dropped reading), then add the new one.</details>

**Round 5:**
```cpp
unsigned long muteStart = 0;
const unsigned long MUTE_DURATION = 600000;

// Returns true while the buzzer is muted.
bool isMuted() {
  return millis() - muteStart < MUTE_DURATION;
}
```
<details><summary>Answer</summary>`muteStart` starts at 0, so for the first 10 minutes after power-up `millis() - 0 < 600000` is true and the device is **muted from the moment it starts**. If the fridge is already warm after a power cut, the buzzer stays silent for 10 minutes. Keep a separate `muted` flag that's only set by a mute request (or start `muteStart` so the first check is false), and clear it when the time runs out.</details>

**Round 6:**
```cpp
void handle_status() {
  String json = "{\"temperature\":" + String(lastTemp, 1) +
                ",\"alarm\":" + (alarmOn ? "true" : "false") + "}";
  server.send(200, "application/json", json);
}
```
<details><summary>Answer</summary>Before the first good DHT11 reading, `lastTemp` is `NAN`, and `String(NAN, 1)` produces `nan`. The text `{"temperature":nan,...}` isn't valid JSON, so the dashboard's `response.json()` fails and shows "no connection" for the first few seconds (or for good, if the sensor is unplugged). Send `null` when the value is `NAN`: `isnan(lastTemp) ? String("null") : String(lastTemp, 1)`.</details>
