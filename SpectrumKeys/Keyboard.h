#pragma once

#define TIMER_GET_SYS_INFO   0x10001
#define MAX_COLOR_VALUE		  255
#define EFFECT_NUM			  20
#define DEVICE_NUM			  17

class Keyboard {
public:
   int init();
   void cleanup();
};