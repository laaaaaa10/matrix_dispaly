// display.cpp - low-level LED matrix communication

// ----- includes ----- //
#include "display.h"
#include "time_manager.h"

// ----- variables ----- //
int ScrollSpeed = 150;
int totalFrames;
int pos;

// ----- functions ----- //
// Scrolls msg across the display — direction: 1 = right, -1 = left
void scrollText(String msg, int direction, int loops) {
  String padded = "      " + msg + "      ";
  totalFrames = padded.length() - 6;

  for (int rep = 0; rep < loops; rep++) {
    for (int i = 0; i <= totalFrames; i++) {

      pos = (direction == 1) ? i : (totalFrames - i);

      if (inActiveTime()) {
        sendFrame(padded.substring(pos, pos + 6));
        delay(ScrollSpeed);
      } 
      // if its not in active time
      else {
        sendFrame("      ");
        return;
      }
    }
  }
}


// Sends exactly 6 characters to the matrix as one frame
void sendFrame(String frame) {  
  while (frame.length() < 6) {
    frame += " ";
  }
  if (frame.length() > 6) {
    frame = frame.substring(0, 6);
  }

  Serial2.write(0x02);
  Serial2.print("01");
  Serial2.print(frame);
  Serial2.write(0x03);
}

