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
   const int maxCol = 14;
   const DEVICE_INDEX type = DEV_CK530;
   const bool gradient = true;
   const bool stereo = true;

   void getColumnColor(int col, float left, float right, char* color);
   void getColumnColorRange(int col, float min, float max, float level, char* color);
};