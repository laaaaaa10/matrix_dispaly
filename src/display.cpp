#include "display.h"

#define DISP_TX 22
int ScrollSpeed = 150;

void sendFrame(String frame) {
  while (frame.length() < 6) frame += " ";
  if (frame.length() > 6) frame = frame.substring(0, 6);

  Serial2.write(0x02);
  Serial2.print("01");
  Serial2.print(frame);
  Serial2.write(0x03);
}

void scrollText(String msg, int direction, int loops) {
  String padded = "      " + msg + "      ";
  int totalFrames = padded.length() - 6;

  for (int rep = 0; rep < loops; rep++) {
    for (int i = 0; i <= totalFrames; i++) {
      int pos = (direction == 1) ? i : (totalFrames - i);
      sendFrame(padded.substring(pos, pos + 6));
      delay(ScrollSpeed);
    }
  }
}