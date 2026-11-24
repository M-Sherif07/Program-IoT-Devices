# Activity - Wi-Fi AP + Station Mode

Work through these tasks after reading [Wi-Fi Modes - AP + Station Mode](../Resource/Week-11-WiFi-AP-Station-Mode.md). Every task runs the microcontroller in **`WIFI_AP_STA`** mode: it creates its own network for phones **and** joins your 2.4 GHz router or phone hotspot at the same time.

> **Choose your board:** you can use the **ESP32-PICO** or the **Raspberry Pi Pico 2 W**. Where the two need different code, the task says what each board uses; the Resource's **Choose your board** table lists every difference. The two biggest:
> - **Noticing Wi-Fi changes.** ESP32: one event function (`WiFi.onEvent()`) that only sets `volatile` flags. Pico 2 W: a `checkWiFiChanges()` function, called from `loop()`, that checks both sides every 500 ms and reacts when something changes (Resource, Key Concept 5).
> - **Joining the router.** ESP32: `WiFi.begin()` returns straight away, so never wait for it. Pico 2 W: `WiFi.begin()` waits until it joins or times out, so call `WiFi.setTimeout(8000)`, start the AP and web server **first**, call `WiFi.begin()` **last**, and after a failed attempt call `stopStation()` (Resource, Key Concept 2).

> **What you need for testing:** a 2.4 GHz router or phone hotspot for the station side, plus a **second** device (another phone, a tablet or a laptop) to join the microcontroller's own network. If you use your phone as the hotspot, join the microcontroller's network from a laptop or another phone. When a device joined to the microcontroller warns "No internet connection", choose **Stay connected**: the microcontroller doesn't share its internet (Resource, Key Concept 3).

> **Schematics:** for every task (all have a circuit), draw the schematic **before** you build it on the breadboard, using [Circuit Studio](https://circuit-studio.pages.dev/).
> - Include **every component** in that task's Components list.
> - **You choose the pins.** Fill in the task's **Your pin choices** table, draw those pins on your schematic, and use the same numbers as named constants in your code (e.g. `const int STA_LED = 27;`). All three must match.
> - Show the resistors, and connect every ground to GND.
> - When it's finished, use **Export → Schematic image** to save a picture and **Export → Share link** to get a link, and paste both into the Schematic box near the top of each task.

> **Pin rules for the ESP32-PICO** (a pin that breaks one of these rules can stop the sketch working, or stop the board booting):
> - **Analog inputs** (potentiometer) must use an **ADC1** pin: GPIO 32, 33, 34, 35, 36 or 39. ADC2 pins return nothing useful while the Wi-Fi radio is on.
> - **Outputs** (LEDs) can't use GPIO 34, 35, 36 or 39, because they can only be inputs.
> - **Never use** GPIO 6-11, 16 or 17 (wired to the ESP32-PICO's internal flash) or GPIO 1 and 3 (Serial TX/RX, which the Serial Monitor needs).
> - **Avoid the boot-mode (strapping) pins** GPIO 0, 2, 5, 12 and 15. Something connected to them can stop the board from booting or uploading.
> - **Safe choices** for digital parts (LEDs, button, DHT11): GPIO 4, 13, 14, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33.
> - **Buttons** are wired between the GPIO and GND and use `INPUT_PULLUP`, so a press reads `LOW`.
> - **Check your board's pinout.** Not every GPIO is broken out to a header pin on every ESP32-PICO board.
> - **Within one task, every connection needs its own pin.**

> **Pin rules for the Raspberry Pi Pico 2 W:**
> - **Analog inputs** (potentiometer) must use an ADC pin: **GP26, GP27 or GP28**.
> - **Never use GP23, GP24, GP25 or GP29.** They're wired to the Wi-Fi chip (they aren't on the header pins).
> - **Safe choices** for digital parts (LEDs, button, buzzer, DHT11): GP2-GP22. GP0 and GP1 also work (the Serial Monitor uses USB, not these pins).
> - **Buttons** are wired between the GP pin and GND and use `INPUT_PULLUP`, so a press reads `LOW`.
> - Use the **GP number** in your code (`const int STA_LED = 14;` for GP14), not the physical pin number printed next to it on the pinout.
> - **Within one task, every connection needs its own pin.**

> **Supabase setup (do this once, before Task 3):** Tasks 3, 4 and 6 save readings to a cloud database on [Supabase](https://supabase.com). Reuse your Week 10 project or create a new free one, then run this in the **SQL Editor**:
> ```sql
> create table ap_sta_log (
>   id          bigint generated always as identity primary key,
>   created_at  timestamptz not null default now(),   -- when the row arrived
>   pot_value   integer,                              -- Tasks 3-4
>   temperature real,                                 -- Task 6
>   humidity    real,                                 -- Task 6
>   uptime_s    bigint,                               -- when it was measured
>   buffered    boolean default false                 -- true if it waited in the queue
> );
>
> alter table ap_sta_log enable row level security;
>
> create policy "Device can insert ap_sta rows"
>   on ap_sta_log for insert
>   to anon
>   with check (true);
> ```
> Copy the **Project URL** and **publishable** (anon) key into `SUPABASE_URL` and `SUPABASE_KEY`. Never use the `service_role` key on the device.

> **Credentials:** replace your real details with `"YOUR_WIFI_NAME"`, `"YOUR_WIFI_PASSWORD"`, `"YOUR_AP_PASSWORD"`, `"YOUR_PROJECT_ID"` and `"YOUR_SUPABASE_PUBLISHABLE_KEY"` before you submit any code.

## Task 1 - Dual-Interface Status Monitor

**Scenario:**

A technician installing a barn controller needs to see, at a glance, whether the controller's own network has phones on it and whether it's online through the farm router. These two things must be shown separately, because one can fail without the other.

**Components:**

- ESP32-PICO or Raspberry Pi Pico 2 W development board
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
> **Your pin choices** (choose any suitable pin, following the pin rules for your board, and use the same numbers in your schematic and your code):
>
> | Connection | Pin type needed | Your pin |
> |---|---|---|
> | AP LED, "a phone is joined" (through a 220 Ω resistor; LED cathode → GND) | Digital output | ___ |
> | STA LED, "online through the router" (through a 220 Ω resistor; LED cathode → GND) | Digital output | ___ |

Set `WiFi.mode(WIFI_AP_STA)`. Set the access point's address to **`192.168.6.1`** with `WiFi.softAPConfig()`, then start the access point `"Barn-<your initials>"` on `AP_CHANNEL = 1` with `WiFi.softAP()`, and check both return values. Then join your router with `WiFi.begin()`, with **no** blocking `while` loop. Notice changes on both sides:
- **ESP32:** write one event function that handles `ARDUINO_EVENT_WIFI_STA_GOT_IP`, `ARDUINO_EVENT_WIFI_STA_DISCONNECTED`, `ARDUINO_EVENT_WIFI_AP_STACONNECTED` and `ARDUINO_EVENT_WIFI_AP_STADISCONNECTED` by only setting `volatile` flags (and saving the disconnect reason).
- **Pico 2 W:** write `checkWiFiChanges()`, which runs every 500 ms and compares `WiFi.status() == WL_CONNECTED` and `WiFi.softAPgetStationNum()` with the values from the last check. After a failed `WiFi.begin()`, save the reason from `cyw43_wifi_link_status()`.

In `loop()`, with no `delay()`:
- when the station side comes online, call `printBothSides()`, which prints for **each** interface its IP address and MAC address, plus the AP's SSID and phone count, the router's SSID, gateway and RSSI, and the radio's current channel
- if `WiFi.channel()` is not `AP_CHANNEL`, print `[RADIO] AP moved from channel 1 to <n>`
- write `bool sameSubnet(IPAddress a, IPAddress b, IPAddress mask)` and print a warning if the router's network uses the same range as the AP
- print a line for every phone joining or leaving (with the new count), and when the router connection is lost or a join fails (with the reason)
- the AP LED is on while `WiFi.softAPgetStationNum() > 0`; the STA LED is on while the station side is online
- `maybe_reconnect()` calls `WiFi.begin()` while the station side is offline: every 10 s on the ESP32; every 30 s on the Pico 2 W, with `WiFi.setTimeout(8000)` and `stopStation()` after a failed attempt

Test it: join the AP from your second device, then switch your router or hotspot off for 30 seconds and back on.

>
> Evidence: _(video of both LEDs as a device joins and the router is switched off and on; Serial Monitor screenshot showing both interfaces' details, the channel move and the router being lost and regained while the AP stays up)_
>
**Check yourself:**
- [ ] Pin table filled in with pins that follow the pin rules, and the same pins used in both the schematic and the code constants
- [ ] Schematic drawn in Circuit Studio using every component listed above, pasted into the Schematic box with its share link
- [ ] AP address set with `softAPConfig()` before `softAP()`, both before `WiFi.begin()`, with both return values checked
- [ ] ESP32: one event function handles all four events with only `volatile` flags. Pico 2 W: `checkWiFiChanges()` reacts only to changes, every 500 ms
- [ ] Both IP addresses and both MAC addresses are printed (they differ on the ESP32 and are the same on the Pico 2 W)
- [ ] The channel move is reported if your router isn't on channel 1
- [ ] Switching the router off turns only the STA LED off; the joined device stays connected to the AP
- [ ] `loop()` contains no `delay()`

---

## Task 2 - One Web Server, Two Doors

**Scenario:**

A shared workshop's light can be switched by anyone on the building's Wi-Fi. The supervisor, standing in the workshop and joined to the controller's own network, must be able to **lock** it so that only people on the controller's own network can switch it.

**Components:**

- ESP32-PICO or Raspberry Pi Pico 2 W development board
- 1x LED
- 1x 220 Ω resistor
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
> **Your pin choices** (choose any suitable pin, following the pin rules for your board, and use the same numbers in your schematic and your code):
>
> | Connection | Pin type needed | Your pin |
> |---|---|---|
> | Workshop light LED (through the 220 Ω resistor; LED cathode → GND) | Digital output | ___ |

Run `WIFI_AP_STA` with an access point `"Workshop-<your initials>"` at `192.168.6.1`, and one `WebServer`. Write `bool cameFromAP()` using `server.client().localIP()`. Routes:
- `/` says which network the visitor came through (with the matching microcontroller address), shows the light state and whether control is locked, and has a **Toggle light** form (POST) and a link to `/admin`
- `/toggle` (POST only) flips the light and redirects to `/` with `303`, **unless** control is locked and the request came from the router side, in which case reply `403` with `Locked by the supervisor`
- `/admin` is **AP side only** (`403` otherwise). It shows the router's SSID, the microcontroller's router-side address, the number of phones on the AP, the lock state, and a **Lock / Unlock** form (POST to `/admin/lock`)
- `/admin/lock` (POST, AP side only) flips the lock and redirects to `/admin`
- anything else `404`

In `loop()`, print the router-side address once when the station side connects, and call `server.handleClient()` every pass, with no `delay()`. On the Pico 2 W, start the web server before calling `WiFi.begin()`.

Test from both doors: a device on the AP and a laptop (or phone) on your router.

>
> Evidence: _(screenshots from both devices showing the "came through" line; the router-side device getting `403` on `/admin`; the router-side toggle working while unlocked and getting `Locked by the supervisor` while locked; video of the LED)_
>
**Check yourself:**
- [ ] Pin table filled in with pins that follow the pin rules, and the same pins used in both the schematic and the code constants
- [ ] Schematic drawn in Circuit Studio using every component listed above, pasted into the Schematic box with its share link
- [ ] `cameFromAP()` compares `server.client().localIP()` with `WiFi.softAPIP()`
- [ ] The home page correctly names the door on both devices
- [ ] `/admin` and `/admin/lock` refuse the router side with `403`
- [ ] The lock blocks only the router side; the AP side can always toggle
- [ ] Actions use POST; `loop()` has no `delay()`

---

## Task 3 - Local Dashboard + Cloud Logging

**Scenario:**

A water-level gauge in a pump shed logs to the cloud for the council's office, and pump staff on site check it on their phones through the gauge's own network. The local dashboard must keep working whatever happens to the internet.

**Components:**

- ESP32-PICO or Raspberry Pi Pico 2 W development board
- Potentiometer (stands in for the water-level sensor)
- 1x LED (cloud status)
- 1x 220 Ω resistor
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
> **Your pin choices** (choose any suitable pin, following the pin rules for your board, and use the same numbers in your schematic and your code):
>
> | Connection | Pin type needed | Your pin |
> |---|---|---|
> | Potentiometer wiper (outer legs → 3.3V and GND) | Analog input (ESP32: **ADC1 only**; Pico: GP26-28) | ___ |
> | Cloud LED (through the 220 Ω resistor; LED cathode → GND) | Digital output | ___ |

Run `WIFI_AP_STA` with an access point `"Gauge-<your initials>"` at `192.168.6.1`. Then:
- read the potentiometer every `READ_INTERVAL = 500` ms into a variable
- every `LOG_INTERVAL = 30000` ms, if `WiFi.status() == WL_CONNECTED`, insert a row into `ap_sta_log` with `pot_value` and `uptime_s` (`millis() / 1000`) using HTTPS (`WiFiClientSecure`, `apikey` + `Authorization` headers). Count it as saved only on `201`. Keep a count of rows saved and the last HTTP code. If offline, print `[CLOUD] Offline - reading skipped`
- `/` sends a raw-string dashboard page that fetches `/api/status` every 2 s and shows the level, whether the cloud is online, rows saved, last HTTP code and the router RSSI (or `--` when offline)
- `/api/status` returns valid JSON with `pot`, `cloudOnline`, `sent`, `lastCode` and `rssi` (`null` when offline)
- the cloud LED shows the station side's state; `maybe_reconnect()` retries while offline (ESP32: every 10 s; Pico 2 W: every 30 s, as in Task 1)
- `loop()` has no `delay()`

Test: watch rows arrive in **Table Editor → ap_sta_log**, then switch the router off. The dashboard on the AP must keep updating.

>
> Evidence: _(video of the dashboard on a device joined to the AP while you turn the potentiometer, before and during a router outage; screenshot of the `ap_sta_log` rows; Serial Monitor showing saves and skipped readings)_
>
**Check yourself:**
- [ ] Pin table filled in with pins that follow the pin rules, and the same pins used in both the schematic and the code constants
- [ ] Schematic drawn in Circuit Studio using every component listed above, pasted into the Schematic box with its share link
- [ ] Rows appear in `ap_sta_log` every 30 s with `pot_value` and `uptime_s`; success judged by `201`
- [ ] `WiFi.status()` checked immediately before each send; `http.end()` always called
- [ ] Dashboard keeps updating during the router outage, and shows the cloud as offline
- [ ] `/api/status` is valid JSON, with `null` RSSI when offline
- [ ] `loop()` has no `delay()`

---

## Task 4 - Offline Queue: Nothing Gets Lost

**Scenario:**

The council noticed gaps in the gauge's data whenever the shed's internet dropped. Extend Task 3 so readings taken while offline are kept and uploaded once the connection returns, without ever freezing the local dashboard.

**Components:**

- Same circuit as Task 3

> **Schematic:**
>
> _[Schematic image placeholder: reuse your Task 3 schematic (same circuit), then paste the exported image and share link here.]_
>
> <details><summary>Schematic link</summary>
>
> [Open the example schematic in Circuit Studio]()
>
> </details>
>
> **Your pin choices** (same as Task 3):
>
> | Connection | Pin type needed | Your pin |
> |---|---|---|
> | Potentiometer wiper (outer legs → 3.3V and GND) | Analog input (ESP32: **ADC1 only**; Pico: GP26-28) | ___ |
> | Cloud LED (through the 220 Ω resistor; LED cathode → GND) | Digital output | ___ |

Starting from your Task 3 sketch:
- add a `struct LevelReading { int potValue; unsigned long uptimeSeconds; }` and a circular buffer of `QUEUE_SIZE = 10` readings with `queueHead` and `queueCount`
- write `void enqueue(const LevelReading& r)`: when the queue is full, drop the **oldest** reading and add one to a `droppedCount`
- every 30 s, send the new reading straight away (`buffered: false`) only if online **and** the queue is empty; otherwise enqueue it
- write `void flushQueue()`: while online, send the **oldest** queued reading with `buffered: true` **once every `FLUSH_INTERVAL = 3000` ms**, and remove it from the queue only if the reply is `201`
- add `queued` and `dropped` to `/api/status` and the dashboard

Test: switch the router off for **3 minutes** (6 readings), then back on. Then do a **7-minute** outage (14 readings) to see the queue overflow.

>
> Evidence: _(screenshots of the dashboard during an outage showing the queue growing; the `ap_sta_log` table after reconnecting, showing the late rows with `buffered = true` and their `uptime_s` values in order; the dashboard after the 7-minute outage showing readings dropped (about 4, depending on timing))_
>
**Check yourself:**
- [ ] Pin table filled in with pins that follow the pin rules, and the same pins used in both the schematic and the code constants
- [ ] Schematic drawn in Circuit Studio using every component listed above, pasted into the Schematic box with its share link
- [ ] Queue indices wrap with `% QUEUE_SIZE`; no write past the end of the array
- [ ] When full, the oldest reading is dropped and counted
- [ ] A new reading never jumps ahead of queued ones
- [ ] Only one queued row is sent every 3 s, and it's only removed after a `201`
- [ ] The dashboard stays responsive while the queue uploads
- [ ] `loop()` has no `delay()`

---

## Task 5 - Wi-Fi Provisioning Portal

**Scenario:**

A company sells a smart doorbell chime. Customers can't edit code, so the chime must get their home Wi-Fi details through a setup page, test them, remember them, and switch to normal Station mode once it's online.

**Components:**

- ESP32-PICO or Raspberry Pi Pico 2 W development board
- 1x LED (status)
- 1x 220 Ω resistor
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
> **Your pin choices** (choose any suitable pin, following the pin rules for your board, and use the same numbers in your schematic and your code):
>
> | Connection | Pin type needed | Your pin |
> |---|---|---|
> | Status LED (through the 220 Ω resistor; LED cathode → GND) | Digital output | ___ |

Write a provisioning **state machine** with `enum ProvisionState { STATE_TRY_SAVED, STATE_PORTAL, STATE_TESTING, STATE_ONLINE }`:
- **Start-up:** load `ssid` and `password` from flash (ESP32: `Preferences`; Pico 2 W: `EEPROM`, with a marker value and `EEPROM.commit()` after every save). If an SSID is saved, try it in Station mode (`STATE_TRY_SAVED`); after `CONNECT_TIMEOUT = 15000` ms without connecting, open the portal. If nothing is saved, open the portal straight away.
- **Portal:** stop the station side (ESP32: `WiFi.disconnect()`; Pico 2 W: `stopStation()`), then `WIFI_AP_STA` with the setup network `"Chime-Setup-<your initials>"` (8+ character password), a `DNSServer` captive portal, and an asynchronous scan. The setup page lists the scanned networks in a `<select>` (names passed through your own `htmlEscape()`), has a box for a hidden network name and a password box, and sends the form with `method='post'` to `/connect`.
- **`/connect` (POST only):** the SSID must be 1-32 characters, and the password empty or 8-63 characters, otherwise show the page again with a message and `400`. Otherwise go to `STATE_TESTING` and call `WiFi.begin()` with them (the AP stays up). On the Pico 2 W, reply first and call `WiFi.begin()` from `loop()` about a second later, because it waits.
- **Testing:** the page shows "Connecting..." and fetches `/status` every second. On `WL_CONNECTED`, save the details in flash and go to `STATE_ONLINE`; the page shows the new IP address. If the test fails (ESP32: 15 s without connecting; Pico 2 W: `WiFi.begin()` didn't join), stop the station side, go back to `STATE_PORTAL`, and show a helpful message on the setup page.
- **Online:** 10 seconds after a successful test, close the setup network (`dnsServer.stop()`, then `WiFi.softAPdisconnect(true)` on the ESP32 or `WiFi.disconnectAP()` on the Pico 2 W).
- **LED:** fast blink (150 ms) in the portal, slow blink (600 ms) while trying or testing, solid when online.
- **Never** print the password or put it back into a page. `loop()` has no `delay()`.

Test, in this order: (1) a wrong password, (2) the right password, (3) restart the board (it must connect without the portal), (4) switch your hotspot off and restart (the portal must open after 15 s).

>
> Evidence: _(video of the full flow on your phone: the portal opening by itself, the wrong-password message, the successful connection with the new IP, and the setup network disappearing; Serial Monitor screenshots of all four tests)_
>
**Check yourself:**
- [ ] Pin table filled in with pins that follow the pin rules, and the same pins used in both the schematic and the code constants
- [ ] Schematic drawn in Circuit Studio using every component listed above, pasted into the Schematic box with its share link
- [ ] Four named states in an `enum`, handled in one `switch`
- [ ] Setup page opens by itself; network names are escaped; the form uses POST
- [ ] Details are saved **only** after a successful test
- [ ] A failed test stops the station side and returns to the portal with a message
- [ ] The setup network closes 10 s after success
- [ ] Saved details are used on restart; a missing network falls back to the portal after 15 s
- [ ] The password never appears in the Serial Monitor or a page

---

## Task 6 - Capstone: Greenhouse Controller

**Scenario:**

A market garden wants greenhouse controllers it can install without a laptop. Each controller always runs its own network, so staff can check it on their phones at any time. It's set up through that same network, logs temperature and humidity to Supabase through the garden's Wi-Fi, keeps readings during internet outages, and can be reset for a new site by holding a button.

**Components:**

- ESP32-PICO or Raspberry Pi Pico 2 W development board
- DHT11 sensor (3-pin board)
- 2x LED (Wi-Fi status, cloud status)
- 2x 220 Ω resistor
- 1x push button (factory reset)
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
> **Your pin choices** (choose any suitable pin, following the pin rules for your board, and use the same numbers in your schematic and your code):
>
> | Connection | Pin type needed | Your pin |
> |---|---|---|
> | DHT11 OUT/DATA (+ → 3.3V, − → GND) | Digital input/output | ___ |
> | Wi-Fi status LED (through a 220 Ω resistor; LED cathode → GND) | Digital output | ___ |
> | Cloud LED (through a 220 Ω resistor; LED cathode → GND) | Digital output | ___ |
> | Factory reset button (other leg → GND, `INPUT_PULLUP`) | Digital input | ___ |

Requirements:
- **Always-on access point:** `"Greenhouse-<your initials>"` at `192.168.6.1` in `WIFI_AP_STA`. It is **never** closed, so the local dashboard is always available.
- **Provisioning:** the Task 5 state machine, but the setup form lives at **`/setup`**. In `STATE_PORTAL` (and `STATE_TESTING`), the captive portal redirects unknown paths to `/setup`. Once online, `/setup` is **AP side only** (`403` from the router side), so the network can be changed later from the greenhouse but not from the garden's Wi-Fi.
- **Dashboard (`/`):** a raw-string page fetching `/api/status` every 2 s, showing temperature, humidity, Wi-Fi state (`trying`/`setup`/`testing`/`online`), the router SSID when online, cloud online, queued and dropped counts, and rows saved, with a link to `/setup`.
- **Sensor:** DHT11 read every 2 s; the latest good reading kept.
- **Cloud with offline queue:** every 30 s, a row with `temperature`, `humidity`, `uptime_s` and `buffered` goes to `ap_sta_log`, using the Task 4 queue (`QUEUE_SIZE = 20`, one queued row every 3 s, removed only on `201`).
- **LEDs:** Wi-Fi LED as in Task 5 (fast blink setup, slow blink trying/testing, solid online, off if the saved router is lost); cloud LED on while the station side is connected.
- **Reconnect:** once online, if the router is lost, retry every **30 s** (less often than usual, because each retry disturbs the AP).
- **Factory reset:** holding the button for 3 s erases the saved details and restarts (ESP32: `prefs.clear()` + `ESP.restart()`; Pico 2 W: clear the `EEPROM` data + `rp2040.reboot()`).
- `loop()` calls short functions only and contains no `delay()`.

>
> Evidence: _(video showing: first-time setup through the captive portal, the dashboard on a phone joined to the AP, a router outage with the queue growing and then uploading, `/setup` refused from the router side, and a factory reset returning to setup mode; screenshots of the `ap_sta_log` rows including buffered ones, and the Serial Monitor)_
>
**Check yourself:**
- [ ] Pin table filled in with pins that follow the pin rules, and the same pins used in both the schematic and the code constants
- [ ] Schematic drawn in Circuit Studio using every component listed above, pasted into the Schematic box with its share link
- [ ] The AP is never closed; the dashboard works in every state
- [ ] Provisioning tests before saving; failures fall back to setup; saved details are used on restart
- [ ] `/setup` is refused from the router side once online
- [ ] Offline readings are queued and uploaded oldest first, one every 3 s, with `buffered = true`
- [ ] Router retries every 30 s, not faster
- [ ] Holding the button for 3 s returns the controller to setup mode
- [ ] `loop()` has no `delay()`

---

## Task 7 - Spot-the-Bug Worksheet

**Format:** For each round, read the snippet and write down what's wrong **before** revealing the answer.

**Round 1:**
```cpp
const char PAGE[] = R"rawliteral(
<!DOCTYPE html>
<html><head>
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body><canvas id="chart"></canvas> ...
)rawliteral";
// Served to phones on the device's access point; the device is online through the router, so this works
```
<details><summary>Answer</summary>The page loads Chart.js from the internet, but phones joined to the device's access point **have no internet**: the microcontroller doesn't route or share its station-side connection. The script never loads, so the chart never appears (it would only work for devices on the router's network). Pages for the AP side must only use what the microcontroller itself serves: plain HTML, inline JavaScript, or files stored on the board.</details>

**Round 2:**
```cpp
// The home router hands out 192.168.1.x addresses
IPAddress apIP(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

void setup() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, subnet);
  WiFi.softAP("Shed-Panel", "iot12345");
  WiFi.begin(SSID, PASSWORD);
}
```
<details><summary>Answer</summary>The access point uses the **same address range** as the router (`192.168.1.x`), and even the router's own address (`192.168.1.1`). The microcontroller now has two interfaces on the same network numbers and can't tell which side a reply belongs on, so traffic goes out the wrong interface: cloud sends fail, or phones can't load pages. Choose an AP range nobody else uses, such as `192.168.6.1`.</details>

**Round 3:**
```cpp
bool cameFromAP() {
  return server.client().remoteIP() == WiFi.softAPIP();
}
```
<details><summary>Answer</summary>`remoteIP()` is the **visitor's** address (e.g. the phone's `192.168.6.2`), which is never the microcontroller's own AP address, so this always returns `false`. Every AP-only page refuses everyone, even the supervisor standing next to the device. Use `server.client().localIP()`, the microcontroller's address that the request arrived on.</details>

**Round 4:**
```cpp
void handle_connect() {
  String ssid = server.arg("ssid");
  String password = server.arg("password");
  prefs.putString("ssid", ssid);           // (Pico: saveWiFi(ssid, password);)
  prefs.putString("password", password);
  WiFi.begin(ssid.c_str(), password.c_str());
  state = STATE_TESTING;
  stateStart = millis();
  server.sendHeader("Location", "/");
  server.send(303);
}
```
<details><summary>Answer</summary>The details are saved **before** they've been tested. If the user mistyped the password, the bad details are now stored, and every restart wastes 15 s trying them before falling back to setup. Validation (SSID 1-32 characters, password empty or 8-63) is also missing. Save only in the testing state, after `WiFi.status() == WL_CONNECTED`.</details>

**Round 5:**
```cpp
void flushQueue() {
  while (queueCount > 0 && WiFi.status() == WL_CONNECTED) {
    if (!sendToSupabase(queue[queueHead], true)) break;
    queueHead = (queueHead + 1) % QUEUE_SIZE;
    queueCount--;
  }
}
```
<details><summary>Answer</summary>The `while` loop sends **every** queued row in one go. Each HTTPS send blocks for roughly 0.3-2 seconds, so 20 queued rows can freeze `loop()` for 10-40 seconds: the local dashboard stops answering and sensor reads stop, just when staff are checking whether the device survived the outage. Send one row per call, spaced out with a `millis()` timer (e.g. every 3 s).</details>

**Round 6:**
```cpp
case STATE_TESTING:
  if (WiFi.status() == WL_CONNECTED) {
    // ...save and go online...
  } else if (millis() - stateStart >= CONNECT_TIMEOUT) {
    message = "Couldn't join that network.";
    state = STATE_PORTAL;
  }
  break;
```
<details><summary>Answer</summary>After a failed test the station side is left **still retrying** the bad network. While it searches it hops between channels, so the setup network keeps disappearing from under the user's phone, and the page they need to try again won't load reliably. Stop the station side before going back to `STATE_PORTAL` (ESP32: `WiFi.disconnect()`; Pico 2 W: `stopStation()`).</details>

---

## Questions

Answer these in your own words before moving on:

1. In `WIFI_AP_STA` mode the microcontroller has two IP addresses. Explain why, and give one situation where each address is the one you'd type into a browser. On your board, are the two MAC addresses the same or different?
   ```


   ```

2. You asked for the access point on channel 1, but the Serial Monitor shows channel 11. Explain what happened, what the phones joined to the AP may have noticed, and how you could avoid it.
   ```


   ```

3. A user complains: "Your device is online and uploading to the cloud, so why does my phone say No internet when I join its network?" Write a short, clear reply for them.
   ```


   ```

4. In the offline-first logger, why is the queue sent one row every 3 seconds rather than all at once? Why does each row carry `uptime_s` and `buffered` when Supabase already adds `created_at`?
   ```


   ```

5. Describe the provisioning state machine's four states and what moves it between them. Why must the details only be saved after the test succeeds, and why does the access point need to stay up during the test?
   ```


   ```
