#pragma once
#include <Arduino.h>

const int UTC_OFFSET = -4;  // hours from UTC (-4 EST, -3 EDT)
const int START_HOUR =  8;  // starts displaying (start of college)
const int END_HOUR   = 18;  // stops displaying at 6 PM
const int THURSDAY_SLEEP_START = 12;
const int THURSDAY_SLEEP_END   = 18;
const int FRIDAY_SLEEP_START   = 8;
const int FRIDAY_SLEEP_END     = 14;
static bool WeekendSLP;          // if true, sleep on weekends too

void   syncTime(unsigned long epoch);
bool   isTimeSynced();
bool   inActiveTime();
String getCurrentTimeString(); // returns "HH:MM:SS" or "not synced"
int    getCurrentHour();