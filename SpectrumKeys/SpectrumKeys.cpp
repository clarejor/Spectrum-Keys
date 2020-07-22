#include <iostream>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <Functiondiscoverykeys_devpkey.h>
#include "Keyboard.h"

#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

bool stereo = true;

void quit(int exitCode) {
   CoUninitialize();
   exit(exitCode);
}

BOOL WINAPI consoleHandler(DWORD signal) {
   if (signal == CTRL_C_EVENT)
      quit(0);
   return TRUE;
}

IAudioMeterInformation* getAudioMeter() {
   HRESULT hr;
   IMMDeviceEnumerator* pEnumerator = NULL;
   IMMDevice* pDevice = NULL;
   IAudioMeterInformation* pMeterInfo = NULL;
   IPropertyStore* pProps = NULL;

   CoInitialize(NULL);

   // Get enumerator for audio endpoint devices.
   std::cout << "Discovering audio devices...\n";
   hr = CoCreateInstance(
      __uuidof(MMDeviceEnumerator),
      NULL,
      CLSCTX_INPROC_SERVER,
      __uuidof(IMMDeviceEnumerator),
      (void**)&pEnumerator);

   if (FAILED(hr)) {
      std::cerr << "CoCreateInstance returned " << hr << std::endl;
      goto Exit;
   }

   // Get default audio-rendering device.
   hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
   if (FAILED(hr)) {
      std::cerr << "GetDefaultAudioEndpoint returned " << hr << std::endl;
      goto Exit;
   }

   // Open device properties
   hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
   if (FAILED(hr)) {
      std::cerr << "IMMDevice->OpenPropertyStore returned " << hr << std::endl;
      goto Exit;
   }

   // Get the device's friendly name
   PROPVARIANT varName;
   PropVariantInit(&varName);
   hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
   if (FAILED(hr)) {
      std::cerr << "IMMDevice->OpenPropertyStore returned " << hr << std::endl;
      goto Exit;
   }

   // Print endpoint info
   printf("Default audio playback device: \"%S\"\n", varName.pwszVal);
   PropVariantClear(&varName);

   hr = pDevice->Activate(
      __uuidof(IAudioMeterInformation),
      CLSCTX_ALL,
      NULL,
      (void**)&pMeterInfo);

   if (FAILED(hr)) {
      std::cerr << "IMMDevice->Activate returned " << hr << std::endl;
      goto Exit;
   }

Exit:
   if (!pMeterInfo) {
      SAFE_RELEASE(pEnumerator)
      SAFE_RELEASE(pDevice)
      SAFE_RELEASE(pMeterInfo)
      SAFE_RELEASE(pProps)
   }
   return pMeterInfo;
}

int main() {
   // Catch CTRL+C
   if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
      std::cerr << "Could not set control handler" << std::endl;
      quit(-1);
   }

   // Initialize audio device API
   auto audioMeter = getAudioMeter();
   if (!audioMeter) {
      quit(-1);
   }

   // Initialize keyboard
   Keyboard keys;
   keys.init();

   float peaks[2];
   while (true) {
      if (stereo) {
         audioMeter->GetChannelsPeakValues(2, &peaks[0]);
         printf("L: %.2f  R: %.2f\n", peaks[0], peaks[1]);
      } else {
         audioMeter->GetPeakValue(&peaks[0]);
         printf("%.3f\n", peaks[0]);
      }
      Sleep(200);
   }
   quit(0);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
