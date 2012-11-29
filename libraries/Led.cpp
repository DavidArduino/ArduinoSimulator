#include "Led.h"
#include "Menus.h"
Led::Led(int _x, int _y):Component()
{ 
  offOn = false;
  gnd = new Pin(this);
  power = new Pin(this);
  gnd->WriteValue (-1);
  power->WriteValue (-1);
  power->xOffset = 45;
  power->yOffset = 170;
  gnd->xOffset = 61;
  gnd->yOffset = 140;
  x = _x;
  y = _y;
  gnd->x = x + gnd->xOffset;
  gnd->y = y + gnd->yOffset;    
  power->x = x + power->xOffset;
  power->y = y + power->yOffset; 
  power->SetName ("LED +");
  gnd->SetName ("LED -");     
}

Led::~Led()
{
  if (gnd)
    delete (gnd);
  if (power)  
    delete (power);
}

void Led::AddMenu ()
{
  HMENU  MainMenu;
  HMENU  FileMenu;
     
  //arduinoMain.setHwnd ( hwndOwner);
  MainMenu=CreateMenu();
  FileMenu=CreateMenu();
  InsertMenu(MainMenu,ID_Edit,MF_POPUP,(UINT_PTR)FileMenu,"Help");
  AppendMenu(FileMenu,MF_STRING,COMPONENT_ABOUT,"&About");
  AppendMenu(FileMenu,MF_SEPARATOR,0,"");
  AppendMenu(FileMenu,MF_STRING,COMPONENT_HELP,"&Component Help");
  //  activate menu 
  (void) SetMenu(windowHandle,MainMenu);
}

void Led::HandleMouseDown (HWND hWnd, int _x, int _y)
{
  Component::HandleMouseDown (hWnd,_x,_y);
  if (gnd->isActive) // we are over the ground spot     
    gnd->Select(!gnd->isSelected);
  else if (power->isActive)
    power->Select(!power->isSelected);  
}

// Move the ports and any connections
void Led::MoveTo (int _x, int _y)
{
  x = _x-xOffset;
  y = _y-yOffset; // Get the x location of the LED after adjusting for mouse click location
  gnd->MoveTo (x + gnd->xOffset - gnd->bm.bmWidth/2, y + gnd->yOffset - gnd->bm.bmHeight/2 );
  power->MoveTo (x + power->xOffset - power->bm.bmWidth/2, y + power->yOffset - power->bm.bmHeight / 2);
}

void Led::Select ( bool select)
{
  gnd->Select(false);
  power->Select(false);
}

Pin * Led::PortSelected(){
  Pin * pin = 0;
  if (gnd->isSelected)
  	pin = gnd;
  if (power->isSelected)
  	pin = power;
    
  return pin;  
}

Pin * Led::PinActive ()
{
  Pin * pin = 0;
  if (gnd->isActive)
    pin = gnd;
  else if (power->isActive)
    pin = power;
  return pin;
}

// Check if the mouse has moved over one of the ports.
// This should only set isActive to true (not move the item)
void Led::HandleMouseMove (HWND hWnd, int _x, int _y)
{
  Component::HandleMouseMove (hWnd,_x,_y);
  gnd->HandleMouseMove ( hWnd, _x, _y );
  power->HandleMouseMove (hWnd, _x, _y );
}

void Led::HandleMouseUp (HWND hWnd)
{
}

void Led::HandleMenu ( int command )
{
}

void Led::PaintStart ( HDC & _hdcWindow, HDC & _hdcMemory, PAINTSTRUCT &_ps)
{
  Component::PaintStart ( _hdcWindow, _hdcMemory, _ps);
  
  gnd->PaintStart ( _hdcWindow, _hdcMemory, _ps);
  power->PaintStart ( _hdcWindow, _hdcMemory, _ps);
}

bool Led::IsSet()
{
  bool set = (gnd->IsSet() && power->IsSet());
  return set;
}

void Led::Reset()
{
  gnd->Reset();
  power->Reset();
}

void Led::Paint(HWND hWnd)
{
  bool ledOn; 
   
  if (hWnd == windowHandle)
  {
    // Paint the background
    Component::Paint (hWnd);   

    // Paint the light on/off
    if (gnd->GetValue() == 1)
	  ledOn = false;
	else if (power->GetValue() == 0)
      ledOn = false;
    else if (gnd->GetValue() == -1)
	  ledOn = false;
	else if (power->GetValue() == -1)
	  ledOn = false;  
    else
	  ledOn = true;  
      
    if (ledOn)
      SelectObject(hdcMemory, hbmRedDot);
    else
      SelectObject(hdcMemory, hbmBlackDot);     
    BitBlt(hdcWindow, x+31,y, bmRedDot.bmWidth, bmRedDot.bmHeight, hdcMemory, 0, 0, SRCAND);
    BitBlt(hdcWindow, x+31,y, bmRedDot.bmWidth, bmRedDot.bmHeight, hdcMemory, 0, 0, SRCPAINT);
    
    // Paint the hotspots
    gnd->Paint(hdcMemory,hdcWindow);
    power->Paint(hdcMemory,hdcWindow);
  }    
}

void Led::LoadBMap (char * bmpResource, HBITMAP &hBitMap, BITMAP &bitMap )
{
  Component::LoadBMap (bmpResource, hBitMap, bitMap );     
  Component::LoadBMap ("REDLED", hbmRedDot, bmRedDot);
  Component::LoadBMap ("BLACKLED", hbmBlackDot, bmBlackDot);
  gnd->LoadBMap (g_hInst);
  power->LoadBMap (g_hInst);
}

