#pragma once
#include <Arduino.h>

#define DISP_TX 22

extern int ScrollSpeed;

void sendFrame(String frame);
void scrollText(String msg, int direction, int loops = 3);
