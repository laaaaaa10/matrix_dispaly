// time_manager.cpp - time sync and querying

// ----- includes ----- //
#include "time_manager.h"

// ----- variables ----- //
static unsigned long _syncEpoch  = 0; // sync time + elapsed since sync
static unsigned long _syncMillis = 0; // millis() at last sync

static bool _synced = false; // has time ever been synced?

// time values
int weekday;
unsigned long day;
int hour;
int minute;
int second;
unsigned long now; // renamed from 'time' — conflicts with the C standard library function time()

char format[9]; // formatting time "HH:MM:SS"


// ----- functions ----- //
// Syncs the ESP's clock to the given Unix timestamp (seconds since Jan 1, 1970)
void syncTime(unsigned long epoch) {
    _syncEpoch = epoch;
    _syncMillis = millis();
    _synced = true;
}

// Returns true if time has been synced at least once
bool isTimeSynced() {
    return _synced;
}

// Returns current local time as a Unix timestamp
static unsigned long localNow() {
    return _syncEpoch + (millis() - _syncMillis) / 1000 + UTC_OFFSET * 3600UL; // add offset in seconds
}

// Returns current local hour (0-23) or -1 if time is not synced
int getCurrentHour() {
    if (!_synced) {
        return -1;
    }
    return (localNow() % 86400) / 3600; // seconds in a day, then seconds in an hour
}


// Determines if current local time is within the active time
bool inActiveTime() {
    if (!_synced) return false;
    day = localNow() / 86400;
    weekday = day % 7;

    // if its the weekday, check if its in the sleep window
    if (weekday != 2 && weekday != 3) { // 2 = Saturday, 3 = Sunday
        // Handles both normal and overnight sleep windows
        hour = getCurrentHour();
        if (hour >= START_HOUR && hour <= END_HOUR) { 
            return true;
        }
    }
    return false;
}

// Returns "HH:MM:SS" or "not synced"
String getCurrentTimeString() {
    // unsynced case
    if (!_synced) {
        return "not synced";
    }

    // calculate local time components
    now    = localNow();
    hour   = (now % 86400) / 3600;
    minute = (now % 3600)  / 60;
    second = (now % 60);

    snprintf(format, sizeof(format), "%02d:%02d:%02d", hour, minute, second);
    return String(format);
}