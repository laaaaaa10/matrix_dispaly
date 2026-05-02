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