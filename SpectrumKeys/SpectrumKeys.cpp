#include <iostream>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <Functiondiscoverykeys_devpkey.h>

#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

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
   LPWSTR pwszID = NULL;

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

   // Get device properties
   hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
   if (FAILED(hr)) {
      std::cerr << "IMMDevice->OpenPropertyStore returned " << hr << std::endl;
      goto Exit;
   }

   // Get the endpoint ID string.
   hr = pDevice->GetId(&pwszID);
   if (FAILED(hr)) {
      std::cerr << "IMMDevice->GetId returned " << hr << std::endl;
      goto Exit;
   }

   // Get the endpoint's friendly-name property
   PROPVARIANT varName;
   PropVariantInit(&varName);
   hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
   if (FAILED(hr)) {
      std::cerr << "IMMDevice->OpenPropertyStore returned " << hr << std::endl;
      goto Exit;
   }

   // Print endpoint friendly name and endpoint ID.
   printf("Default audio playback device found:\n  \"%S\" (%S)\n", varName.pwszVal, pwszID);
   CoTaskMemFree(pwszID);
   pwszID = NULL;
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

   auto audioMeter = getAudioMeter();
   if (!audioMeter) {
      quit(-1);
   }

   float peak;
   while (true) {
      audioMeter->GetPeakValue(&peak);
      printf("%.3f\n", peak);
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
