#pragma once
#include <Arduino.h>

extern int ScrollSpeed;

void sendFrame(String frame);
void scrollText(String msg, int direction, int loops = 3);