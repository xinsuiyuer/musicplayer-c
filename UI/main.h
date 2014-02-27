#ifndef _MAIN_H
#define _MAIN_H
#define UNICODE
#define _UNICODE


#define IDC_TimerID 1023  //定时器ID 
#define IDC_TopMost 1025 //置顶菜单标识 
#define IDC_ABOUT  1026//关于菜单 

#define IDM_PlayCurrentSelect 12001
#define IDM_AddFiles 12003
#define IDM_AddFolder 12004
#define IDM_DELETE 12002
#define IDM_CLEAR 12005
#define IDM_CLOSE 12006


#include <windows.h>
#include <commctrl.h>

typedef struct	_HCONTROLS{
	HWND hTRB_PROGRESS;
	HWND hLST_PLAYLIST;
	HWND hBTN_STOP;
	HWND hBTN_ABOVE;
	HWND hBTN_PLAY_PAUSE;
	HWND hBTN_NEXT;
	HWND hTRB_SETVOLUME;
	HWND hCHK_MUTE;
	HWND hBTN_OPENFILE;
	HWND hBTN_OPENFOLDER;
	HWND hSTC_REMAIN;
	HWND hSTC_CURRENT;
	HWND hTRB_SETBALANCE;
	HWND hIDC_UDN1;
	
}HCONTROLS,*LPHCONTROLS;

typedef struct _FILESINFO{
	TCHAR szFilePath[256][MAX_PATH];
	int iCountOfFiles; // 歌曲数 
}FILESINFO,*LPFILESINFO;

typedef struct _RANDOMNUMBERS  // 用来存放随机数 
{
	char arrayNums[256];
	int nNumsCount; // 在产生随机数时用于记录当前已经产生的随机数个数；在用于随机播放时，记录当前前播放的arrayNums下标数 
}RANDOMNUMBERS,*LPRANDOMNUMBERS;

BOOL WINAPI Main_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL Main_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void Main_OnClose(HWND hwnd);
void Form1_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
void Form1_OnVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
VOID CALLBACK TimerProc(
  HWND hwnd,         // handle to window
  UINT uMsg,         // WM_TIMER message
  UINT_PTR idEvent,  // timer identifier
  DWORD dwTime       // current system time
);
void ResetControls(HWND hdlg);
void PlaySpecifySong(HWND hDlg,int index,LPFILESINFO lpfi,LPHCONTROLS lphctl);
void GetVolumeSate(LPHCONTROLS lpcctls);
void SetDialogText(HWND hdlg,int nTextIndex,LPHCONTROLS lphctl,PNOTIFYICONDATA pAppNodifyIcon);
int GetAbovePlayIndex(UINT nPlayMode,int nCurrentPlayIndex,
			LPRANDOMNUMBERS lprn,LPFILESINFO lpfi);

extern HINSTANCE hInst;
extern void GetControlsHandle(LPHCONTROLS phCtl,HWND hDlg);
extern void InitiateControls(LPHCONTROLS lphCtl);
extern void AddFiles(LPFILESINFO lpfilesinfo,HWND hdlg,LPHCONTROLS lphctls);
extern void AddFolder(LPFILESINFO lpfilesinfo,HWND hdlg,LPHCONTROLS lphctls);
extern void GetrandomNumbers(LPRANDOMNUMBERS lprn,LPFILESINFO lpfi);
extern int GetNextPlayIndex(UINT nPlayMode,int nCurrentPlayIndex,
			LPRANDOMNUMBERS lprn,LPFILESINFO lpfi);
			
extern void EnableControls(HMENU hMenu_PlayList,LPHCONTROLS lpctls);
extern void DisableControls(HMENU hMenu_PlayList,LPHCONTROLS lpctls);
extern void AddFilesNameToPlayList(LPFILESINFO lpfilesinfo,LPHCONTROLS lphctls,HWND hdlg,int nfiles);
extern void MoveUp(LPFILESINFO lpfsi,LPHCONTROLS lphcs,int *ncurrentplay,HWND hdlg);
extern void MoveDown(LPFILESINFO lpfsi,LPHCONTROLS lphcs,int *ncurrentplay,HWND hdlg);
extern void DeleteItem(LPFILESINFO lpfsi,int nBeDeleted,LPHCONTROLS lphctls,HWND hdlg);
extern void SavePlayList(LPFILESINFO lpfsi,HINSTANCE hinstance); 
extern BOOL LoadPlayList(LPFILESINFO lpfsi,HINSTANCE hinstance);
extern BOOL AddNodifyIcon(HWND hWnd,PNOTIFYICONDATA pAppNodifyIcon);
extern void SetPlayModeMenu(HMENU hmenu,UINT *pselected,UINT nMenuID);
extern void SetTransparentMune(HMENU hmenu,UINT *pselected,UINT nMenuID);
extern BOOL WINAPI Main_AboutProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern void AddDropFilesToList(HDROP hDropFiles,LPFILESINFO lpsfi,LPHCONTROLS lphCtls,HWND hdlg);
#endif
