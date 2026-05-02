# LED Matrix Display Controller

A web-based controller for a salvaged industrial LED matrix display, driven by an ESP32. Connect your phone or laptop to the ESP's WiFi hotspot, open the page, type a message, and it scrolls across the display in real time.

---

## What It Is

The display is a 6-character, 5×7 dot-matrix LED unit made by Electronic Displays Inc. (EDI) — the kind of thing you'd see in a factory floor or scoreboard installation from the 90s. It was acquired second-hand with a mystery message already scrolling on it and no documentation.

The ESP32 sits behind it, hosts a small web server, and handles everything: WiFi hotspot, web UI, time sync, and pushing text to the display over a serial line.

The whole protocol — baud rate, packet format, logic inversion — was reverse engineered from scratch through brute force testing since EDI doesn't publish datasheets for their custom OEM units.

---

## How To Use It

1. Power on the display and the ESP32
2. On your phone or laptop, connect to the WiFi network:
   - **SSID:** `ESP_Display`
   - **Password:** `12345678`
3. Open `http://192.168.4.1` in your browser
4. Type a message, set the speed and direction, hit send

The message starts scrolling immediately. You can update it anytime without refreshing the page.

There's also a clock mode — toggle it to switch the display from scrolling text to a live `HH:MM` clock. Time is synced from your browser the moment you open the page.

The display automatically goes blank during college hours (8 AM – 6 PM on weekdays) and comes back on in the evening.

---

## The Web UI

Clean single-page interface served directly from the ESP32's flash storage:

- **Text input** — auto-growing textarea, type as much as you want and it scrolls
- **Speed slider** — 0–100% maps to fast vs slow scroll
- **Direction toggle** — switch between scrolling left or right
- **Clock toggle** — switches between message mode and live clock
- **Status label** — confirms send success/failure with a 3 second timeout

---

## The Display

The unit is a custom EDI dot-matrix display with an internal `1700-259 ver F` controller board. There's no public datasheet for it — EDI builds these to order for specific clients. When it powers on it briefly flashes its own config:

```
A=01  B=96  P=2
```

Which tells you: address 01, baud 9600, even parity. That's what unlocked the rest of it.

The connection is a DB9 serial connector wired to just two pins — DATA and GND. Despite looking like it might be RS232, it runs at 3.3V TTL levels and connects directly to the ESP32 with no adapter or level shifter. The signal also needs to be inverted.

The working packet format is:
```
STX (0x02) + "01" + your text + ETX (0x03)
```

The `01` is the display's address — it strips it and doesn't show it.

Scrolling via serial command was never cracked. Every known MCS protocol scroll command was tested (ESC+SCL, SOH+TA, control bytes 0x01–0x1F, dollar-sign delimiters, broadcast address variants — all of it). The display was originally programmed to scroll using EDI's own proprietary software and doesn't expose that functionality over serial. Scrolling is handled by the ESP32 instead, which sends one 6-character frame at a time and steps through the message at a configurable speed.

---

## Active Hours

The display blanks itself automatically outside of active hours so it's not lighting up a room at 3 AM or during class.

Configured in `time_manager.h`:
```cpp
const int UTC_OFFSET  = -4;   // EST
const int START_HOUR  =  8;   // on at 8 AM
const int END_HOUR    = 18;   // off at 6 PM
```

Weekday vs weekend is derived directly from the Unix timestamp — no RTC needed. Time is synced from the browser on page load and tracked internally after that.

---

## Building & Flashing

Built with PlatformIO. Uses `espressif32@6.3.2` — version 7.0.0 has a known broken package and won't compile.

```ini
[env:esp32dev]
platform = espressif32@6.3.2
board = esp32dev
framework = arduino
board_build.filesystem = spiffs
```

Two upload steps are required:
1. **Upload firmware** — normal PlatformIO upload
2. **Upload filesystem** — PlatformIO → Project Tasks → Platform → Upload Filesystem Image

The second step puts the HTML/CSS/JS onto the ESP32's flash. If you skip it the web UI won't load.

---

## File Structure

```
├── src/
│   ├── main.cpp          — WiFi, web server, display orchestration
│   ├── display.cpp       — Frame sending and software scroll
│   ├── display.h
│   ├── time_manager.cpp  — Time sync and active-hours gating
│   └── time_manager.h
└── data/
    ├── index.html
    ├── app.js
    ├── style.css
    └── send.png
```
