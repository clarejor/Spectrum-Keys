#pragma once

#include "SDKDLL.h"

#define EFFECT_NUM			  20
#define DEVICE_NUM			  17

class Keyboard {

public:
   int init();
   void update(float left, float right);
   void cleanup();

private:
   const int minCol = 1;
   const int maxCol = 18;
   DEVICE_INDEX model = DEV_DEFAULT;
   const bool gradient = true;
   const bool stereo = true;
   const bool clipAlert = true;
   bool alerted = false;

   void getColumnColor(int col, float left, float right, KEY_COLOR& color);
   void getColumnColorRange(int col, float min, float max, float level, KEY_COLOR& color);
};