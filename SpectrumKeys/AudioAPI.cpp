#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

static BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
static void DrawPeakMeter(HWND, float);

// Timer ID and period (in milliseconds)
#define ID_TIMER  1
#define TIMER_PERIOD  125

#define EXIT_ON_ERROR(hr)  \
              if (FAILED(hr)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

//-----------------------------------------------------------
// WinMain -- Opens a dialog box that contains a peak meter.
//   The peak meter displays the peak sample value that plays
//   through the default rendering device.
//-----------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance,
   HINSTANCE hPrevInstance,
   LPSTR lpCmdLine,
   int nCmdShow)
{
   HRESULT hr;
   IMMDeviceEnumerator* pEnumerator = NULL;
   IMMDevice* pDevice = NULL;
   IAudioMeterInformation* pMeterInfo = NULL;

   if (hPrevInstance)
   {
      return 0;
   }

   CoInitialize(NULL);

   // Get enumerator for audio endpoint devices.
   hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
      NULL, CLSCTX_INPROC_SERVER,
      __uuidof(IMMDeviceEnumerator),
      (void**)&pEnumerator);
   EXIT_ON_ERROR(hr)

      // Get peak meter for default audio-rendering device.
      hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
   EXIT_ON_ERROR(hr)

      hr = pDevice->Activate(__uuidof(IAudioMeterInformation),
         CLSCTX_ALL, NULL, (void**)&pMeterInfo);
   EXIT_ON_ERROR(hr)

      DialogBoxParam(hInstance, L"PEAKMETER", NULL, (DLGPROC)DlgProc, (LPARAM)pMeterInfo);

Exit:
   if (FAILED(hr))
   {
      MessageBox(NULL, TEXT("This program requires Windows Vista."),
         TEXT("Error termination"), MB_OK);
   }
   SAFE_RELEASE(pEnumerator)
   SAFE_RELEASE(pDevice)
   SAFE_RELEASE(pMeterInfo)
   CoUninitialize();
   return 0;
}

//-----------------------------------------------------------
// DlgProc -- Dialog box procedure
//-----------------------------------------------------------

BOOL CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
   static IAudioMeterInformation* pMeterInfo = NULL;
   static HWND hPeakMeter = NULL;
   static float peak = 0;
   HRESULT hr;

   switch (message)
   {
   case WM_INITDIALOG:
      pMeterInfo = (IAudioMeterInformation*)lParam;
      SetTimer(hDlg, ID_TIMER, TIMER_PERIOD, NULL);
      hPeakMeter = GetDlgItem(hDlg, IDC_PEAK_METER);
      return TRUE;

   case WM_COMMAND:
      switch ((int)LOWORD(wParam))
      {
      case IDCANCEL:
         KillTimer(hDlg, ID_TIMER);
         EndDialog(hDlg, TRUE);
         return TRUE;
      }
      break;

   case WM_TIMER:
      switch ((int)wParam)
      {
      case ID_TIMER:
         // Update the peak meter in the dialog box.
         hr = pMeterInfo->GetPeakValue(&peak);
         if (FAILED(hr))
         {
            MessageBox(hDlg, TEXT("The program will exit."),
               TEXT("Fatal error"), MB_OK);
            KillTimer(hDlg, ID_TIMER);
            EndDialog(hDlg, TRUE);
            return TRUE;
         }
         DrawPeakMeter(hPeakMeter, peak);
         return TRUE;
      }
      break;

   case WM_PAINT:
      // Redraw the peak meter in the dialog box.
      ValidateRect(hPeakMeter, NULL);
      DrawPeakMeter(hPeakMeter, peak);
      break;
   }
   return FALSE;
}

//-----------------------------------------------------------
// DrawPeakMeter -- Draws the peak meter in the dialog box.
//-----------------------------------------------------------

void DrawPeakMeter(HWND hPeakMeter, float peak)
{
   HDC hdc;
   RECT rect;

   GetClientRect(hPeakMeter, &rect);
   hdc = GetDC(hPeakMeter);
   FillRect(hdc, &rect, (HBRUSH)(COLOR_3DSHADOW + 1));
   rect.left++;
   rect.top++;
   rect.right = rect.left +
      max(0, (LONG)(peak * (rect.right - rect.left) - 1.5));
   rect.bottom--;
   FillRect(hdc, &rect, (HBRUSH)(COLOR_3DHIGHLIGHT + 1));
   ReleaseDC(hPeakMeter, hdc);
}