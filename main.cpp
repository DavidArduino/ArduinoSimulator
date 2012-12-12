#include <windows.h>

#include "SerialClass.h"
#include "EEPROM.h"
#include <Wire.h>
#include "ViewConnections.h"
#include "ArduinoComponent.h"
#include "HighLevelMenu.h"
#include "KeyboardMonitor.h"
#include "MomentaryDepress.h"
#include "SevenSeg.h"
#include "LCDDisplay.h"
#include "Menus.h"

#define MYCLASS "ListBox1"

// static char   text[2048];
static HINSTANCE BCX_hInstance = NULL;
static HINSTANCE g_hInst = NULL;

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
SerialClass Serial;
EEPROMClass EEPROM;
TwoWire Wire;

// Declare Utility Windows
KeyboardMonitor keyboardMonitor;
ViewConnections viewConnections (0,0);
HighLevelMenu highLevelMenu (&viewConnections);

// Declare components
// ArduinoComponent arduino;

/* Definitions of Arduino utilities/externals */
char pgm_read_byte ( char * ch) {return *ch;};
unsigned long millisValue = 0;
unsigned long millis() { 
  millisValue += 20; // Because computer is slow
  return millisValue;
}
void delay (int milliseconds){}
int analogRead (int which){return 0;}
float log (float value){return 0.0;}
void digitalWrite (int pin, int value ) 
{
  highLevelMenu.DigitalWrite (pin,value);
  HighLevelMenu::Instance()->ComputeSystem();
  HighLevelMenu::Instance()->Refresh();
}

int digitalRead (int pin)
{
  int value = highLevelMenu.DigitalRead(pin);
  return value;
}

void pinMode (int pin, int mode)
{
  int p = pin;
}
int bitRead ( unsigned int value, int bit )
{
  int val = 0;
  unsigned int values[] = {
    1,     2,     4,     8,     0x10, 0x20, 0x40, 0x80, 
    0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000 };
    
  if (value & values[bit]) 
    val = 1;
  return val;                
}

void bitWrite (unsigned int & value, int bit, int val)
{
   unsigned int values[] = {    1,     2,     4,     8,   0x10,   0x20,   0x40,   0x80, 
                            0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000};
   if (val)                  
     value |= values[bit];
   else
     value &= ~values[bit];
}



#include "example.ino"         // Actual arduino script

void PaintComponents(HWND hWnd)
{
  highLevelMenu.Paint (hWnd);
}

void HandleMouseDown(HWND hWnd, int _x, int _y)
{
  highLevelMenu.HandleMouseDown (hWnd, _x, _y);
}

void HandleMouseUp(HWND hWnd)
{
  highLevelMenu.HandleMouseUp (hWnd);
}

void HandleMenuCommand (int command )
{
  highLevelMenu.HandleMenu(command);
}

// this is the main() function under Windows GUI
int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrev,LPSTR CmdLine,int CmdShow)
{ 
 WNDCLASS Wc;
 MSG      Msg; 
 BCX_hInstance    =  hInst;
 HWND hWnd = 0;
 
 // ******************************************************
 Wc.style         =  CS_HREDRAW | CS_VREDRAW | LBS_WANTKEYBOARDINPUT;
 Wc.lpfnWndProc   =  WndProc;
 Wc.cbClsExtra    =  0;
 Wc.cbWndExtra    =  0;
 Wc.hInstance     =  hInst;
 Wc.hIcon         =  LoadIcon(NULL,IDI_WINLOGO);
 Wc.hCursor       =  LoadCursor(NULL,IDC_ARROW);
 Wc.hbrBackground =  (HBRUSH)(COLOR_BTNFACE+1);
 Wc.lpszMenuName  =  NULL;
 Wc.lpszClassName =  MYCLASS;
 RegisterClass(&Wc);

 // Draw all the windows
 hWnd = highLevelMenu.DrawWindow ( "High Level Menu", hInst, "", 20, 10 );
 (void) keyboardMonitor.DrawWindow ( "Serial Monitor", hInst, "", 20,120 );
 
 // Setup timer for calling loop
 SetTimer(hWnd,1,5,NULL); // Will create a WM_Timer message Every 5 ms
  
 // the event message loop
 while(GetMessage(&Msg,NULL,0,0)) {
   HWND hActiveWindow = GetActiveWindow();
   if (!IsWindow(hActiveWindow) || !IsDialogMessage(hActiveWindow,&Msg)) {
     TranslateMessage(&Msg);
     DispatchMessage(&Msg);
   }
 }
 
 return Msg.wParam; 
}

// standard windows message handler
LRESULT CALLBACK WndProc (HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  static char lastKey = 0;
  static TCHAR szText[512] = "";
  static unsigned long timerCount = 0;
  int len;
             
  switch (Msg) {
    case WM_CREATE:
      break;  
        
    case WM_PAINT:
      PaintComponents(hWnd);
      break;  
   
    case WM_TIMER:
      if (!timerCount++) 
        setup(); // Call setup once in .ino file
      
      loop(); // Call loop in the .ino file
      break;           
      
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
      HandleMouseDown(hWnd, LOWORD (lParam), HIWORD (lParam));   
      break; 
      
    case WM_MOUSEMOVE:
      highLevelMenu.HandleMouseMove (hWnd, LOWORD (lParam), HIWORD (lParam));
      break;         
      
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
      HandleMouseUp(hWnd);   
      break;        
                
    case WM_COMMAND:
      if (LOWORD (wParam) == 11) // Edit Dialog
        if (HIWORD(wParam) == EN_UPDATE)
        {
          keyboardMonitor.GetWindowTxt (szText);
          len = strlen (szText);
          for (int i=0; i<len; i++)
          {
            if (szText[i] == '\r') 
            {                           
              Serial.println ( &szText[0] );
              Serial.saveLine ( &szText[0]);               
              break;
            }  
          }
        }  
        
      HandleMenuCommand (LOWORD(wParam));  
      
      //  list box item clicked (selected) 
      if (LOWORD(wParam)==1009) {}
                       
      if (LOWORD(wParam)==ID_Exit)
        PostQuitMessage(0);
        
      // do something with other menu items here, 
      // normally one calls up a file load/save dialog box
      if (LOWORD(wParam)==ID_Load) 
      {
        MessageBox(hWnd, "TBD Not yet connected", "TBD", MB_OK | MB_ICONEXCLAMATION);  
      }
      if (LOWORD(wParam)==ID_Save)  {
        MessageBox(hWnd, "TBD Not yet connected", "TBD", MB_OK | MB_ICONEXCLAMATION);  
      }
      
      if (LOWORD (wParam) == ID_Run) {
        MessageBox(hWnd, "TBD Not yet connected", "TBD", MB_OK | MB_ICONEXCLAMATION);  
      }
      return 0;
      break;  
        
      /*     
      case WM_KEYDOWN:
        break;  
      */  
      case WM_KEYUP:
      	highLevelMenu.HandleKeyUp(LOWORD(wParam));
        break;       
      
    }
    
  
  // tidy up and exit the program via the form's upper left corner x 
  
  if ((Msg==WM_DESTROY) && (hWnd != viewConnectionHandle)) 
  {
    UnregisterClass(MYCLASS,BCX_hInstance);
    PostQuitMessage(0);
  }
  
  
  return DefWindowProc(hWnd,Msg,wParam,lParam);
}
