#include "Keyboard.h"
#include <string>

std::string g_DeviceName[DEVICE_NUM] = {
	"MasterKeys Pro L", "MasterKeys Pro L White",
	"MasterKeys Pro M", "MasterKeys Pro M White",
	"MasterKeys Pro S", "MasterKeys Pro S White",
	"MasterMouse Pro L", "MasterMouse Pro S",
	"CK372", "CK530", "CK550/552", "CK551"
	"MM520", "MM530", "MM830",
	"MK750", "MK850"
};


std::string g_EffectName[EFFECT_NUM] = {
	"Full ON", "Breathing", "Color Cycle", "Single Key",
	"Wave Color", "Ripple", "Cross Mode", "Rain Drop",
	"Star Effect", "Game Snake", "Customization", "Indicator",
	"Fireball", "Water Ripple", "Reactive Punch", "Snowing",
	"Heartbeat", "Circle Spectrum", "Reactive Tornado", "MultiLayer"
};

EFF_INDEX  g_EffectMap[EFFECT_NUM] = {
	EFF_FULL_ON, EFF_BREATH, EFF_BREATH_CYCLE, EFF_SINGLE, EFF_WAVE,
	EFF_RIPPLE, EFF_CROSS, EFF_RAIN, EFF_STAR, EFF_SNAKE, EFF_REC, EFF_INDICATOR,
	EFF_FIRE_BALL, EFF_WATER_RIPPLE, EFF_REACTIVE_PUNCH, EFF_SNOWING,
	EFF_HEART_BEAT, EFF_SPECTRUM, EFF_REACTIVE_TORNADO, EFF_MULTI_1
};

int Keyboard::init() {
	printf("CoolerMaster SDK DLL version: %d\n", GetCM_SDK_DllVer());

	// Detect compatible keyboard
   for (int dev = 0; dev < 16; dev++) {
      if (IsDevicePlug((DEVICE_INDEX) dev)) {
         this->model = (DEVICE_INDEX) dev;
         break;
      }
   }

   if (this->model == DEV_DEFAULT) {
      printf("No compatible keyboard detected\n");
      return -1;
   }

	// Enable software LED control
	if (!EnableLedControl(true, this->model)) {
		printf("Failed to enable LED control\n");
		return -1;
	}

	if (!SetFullLedColor(128, 128, 128, this->model)) {
		printf("Failed to set LED color\n");
	}

	return 0;
}

void Keyboard::update(float left, float right) {
	if (this->clipAlert && (left > 0.95 || right > 0.95)) {
		this->alerted = true;
		SetFullLedColor(255, 0, 0, this->model);
	} else {
		// Clear alert
		if (this->alerted) {
			this->alerted = false;
			SetFullLedColor(128, 128, 128, this->model);
		}

		COLOR_MATRIX colors = {};
		for (int row = 0; row < 6; row++) {
			for (int col = 0; col < this->maxCol; col++) {
				this->getColumnColorRange(col, 0, this->maxCol, row <= 3 ? left : right, colors.KeyColor[row][col]);
			}
		}
		SetAllLedColor(colors, this->model);

		/*for (int row = 0; row < 6; row++) {
			for (int col = this->minCol; col < this->maxCol; col++) {
				this->getColumnColor(col, left, right, &rgb[0]);
				SetLedColor(row, col, rgb[0], rgb[1], rgb[2], this->type);
			}
		}*/
	}
}

float mix (float a, float b, float t) {
	return (a * (1 - t)) + (b * t);
}

void Keyboard::getColumnColor(int col, float left, float right, KEY_COLOR& color) {
	if (this->stereo) {
		float middle = mix(this->minCol, this->maxCol, 0.5);
		if (col < middle) {
			this->getColumnColorRange(col, this->minCol, middle, left, color);
		} else {
			this->getColumnColorRange(col, this->maxCol, middle, right, color);
		}
	} else {
		this->getColumnColorRange(col, 0, this->maxCol, max(left, right), color);
	}
}

void Keyboard::getColumnColorRange(int col, float startCol, float endCol, float level, KEY_COLOR& color) {
	float range = abs(startCol - endCol);
	float c = abs(0.5 + col - startCol) / range;
	float yellow = 0.5;

	if (level > 0.95) {
		color.r = 255;
		color.g = 0;
		color.b = 0;
	} else if (c > level) {
		color.r = 128;
		color.g = 128;
		color.b = 128;
	} else {
		if (!gradient)
			c = level;

		if (c < yellow) {
			color.r = mix(0, 255, c / yellow);
			color.g = 255;
			color.b = 0;
		}
		else {
			color.r = 255;
			color.g = mix(255, 0, (c - yellow) / (1.0 - yellow));
			color.b = 0;
		}
	}
}

void Keyboard::cleanup() {
	EnableLedControl(false, this->model);
}