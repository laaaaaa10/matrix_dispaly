#pragma once
#include <Arduino.h>

const int UTC_OFFSET = -4;  // hours from UTC (-4 EST, -3 EDT)
const int START_HOUR =  8;  // starts displaying (start of college)
const int END_HOUR   = 23;  // stops displating (end of college)
static bool WeekendSLP;          // if true, sleep on weekends too

void   syncTime(unsigned long epoch);
bool   isTimeSynced();
bool   inActiveTime();
String getCurrentTimeString(); // returns "HH:MM:SS" or "not synced"
int    getCurrentHour();