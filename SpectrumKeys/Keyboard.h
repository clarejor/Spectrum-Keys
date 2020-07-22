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
   const int numCols = 18;
   const DEVICE_INDEX type = DEV_CK530;
   const bool gradient = true;

   void getColumnColor(int col, float left, float right, char* color);
};