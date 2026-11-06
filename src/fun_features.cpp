// fun_features.cpp - fun features 

// ----- includes ----- //
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include "display.h"
#include "time_manager.h"
#include "fun_features.h"

// ----- variables ----- //
int dailyCouple[2];

// ----- functions ----- //

// daily coupple generator
String getDailyCouple() {
    dailyCouple[0] = random(0, nameCount);
    dailyCouple[1] = random(0, nameCount);

    // make sure second person is different
    while (dailyCouple[1] == dailyCouple[0]) {
        dailyCouple[1] = random(0, nameCount);
    }

    return names[dailyCouple[0]] + " & " + names[dailyCouple[1]];
}

// stopwatch
void stopwatch() {

}

// countdown timer
void countdown() {

}

// the worlds msot basic llm
// i hove no idea if its even possible to do this on an esp32
String basicLLM(String prompt) {
    if (prompt.indexOf("") >= 0) {
        return getDailyCouple();
    }
    return "";
}


// ----- stack mode ----- //
extern bool showClock;
extern bool stackMode;
extern int currentDirection;

// letters enter one at a time, stack up, hold 2 s, then slide off.
// written for text moving left, direction -1 mirrors the text and every frame.

static String mirror(String s) {
    String out = "";
    for (int i = s.length() - 1; i >= 0; i--) out += s[i];
    return out;
}

// one frame + one speed step, false when out of active hours
static bool stackStep(String frame, int direction) {
    if (!inActiveTime()) {
        sendFrame("      ");
        return false;
    }
    sendFrame(direction == 1 ? frame : mirror(frame));
    delay(ScrollSpeed);
    return true;
}

static bool stackIn(String s, int direction) {
    for (int k = 0; k < (int)s.length(); k++) {
        for (int col = 5; col >= k; col--) {
            String frame = "      ";
            for (int c = 0; c < k; c++) frame.setCharAt(c, s[c]);
            frame.setCharAt(col, s[k]);
            if (!stackStep(frame, direction)) return false;
        }
    }
    return true;
}

static bool stackOut(String s, int direction) {
    for (int shift = 1; shift <= (int)s.length(); shift++) {
        String frame = "      ";
        for (int c = 0; c + shift < (int)s.length(); c++) frame.setCharAt(c, s[c + shift]);
        if (!stackStep(frame, direction)) return false;
    }
    return true;
}

// typed text, max 6 letters
void stackText(String text, int direction) {
    text = text.substring(0, 6);
    if (direction != 1) text = mirror(text);
    if (!stackIn(text, direction)) return;
    delay(2000);
    stackOut(text, direction);
}

// "HH:MM" stays until the minute changes, then slides off
void stackClock(int direction) {
    String t = getCurrentTimeString().substring(0, 5);
    String s = (direction == 1) ? t : mirror(t);
    if (!stackIn(s, direction)) return;
    while (getCurrentTimeString().substring(0, 5) == t && showClock && stackMode && currentDirection == direction && inActiveTime()) {
        delay(50);
    }
    stackOut(s, direction);
}
