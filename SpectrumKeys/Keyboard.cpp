#include "Keyboard.h"
#include "SDKDLL.h"
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

	if (!IsDevicePlug(DEV_CK530)) {
		printf("No compatible keyboard detected\n");
		return -1;
	}
	printf("CK530 detected\n");


	return 0;
}

void Keyboard::cleanup() {

}