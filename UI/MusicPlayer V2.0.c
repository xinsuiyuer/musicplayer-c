
#include "main.h"
#include "dialogs.h"

HINSTANCE hInst;
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	HWND hwnd;
	InitCommonControls();
	hInst=hInstance;
	if(hwnd=FindWindow(TEXT("#32770"),TEXT("MusicPlayer V2.0")))
	{
		ShowWindow(hwnd,SW_NORMAL);			 
	}
	else
	{
    	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, Main_Proc);
	}
    return(0);
}
