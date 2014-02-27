#define _WIN32_WINNT 0x0501
#define _WIN32_IE 0x0600
#define NO_WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE  //定义这两个是为了使用UNICODE版的API 
#include <windows.h>
#include <windowsx.h>
#include <strmif.h>
#include <commctrl.h>
#include <Shlwapi.h>
#include <Shellapi.h>
#include "Player_Core.h"
#include <strsafe.h>
#include "main.h"
#include "dialogs.h"
#include "resource.h"
#include "rsrc.inc"



HCONTROLS hCtl; // 存放控件句柄 
FILESINFO filesinfo; // 存放列表信息 
RANDOMNUMBERS rNums; // 存放随机播放时的序号 
HMENU g_hmenu,g_hPlayList_Menu; // 存放菜单句柄
UINT g_CURTRPSelected=IDM_Opaque; // 存放透明菜单当前选择的项 ，默认为不透明 
UINT g_CURPMSelected=IDM_Sortorder; // 存放播放模式菜单当前选项，默认为 顺序播放 
int g_currentPlayIndex; // 当前播放的索引 

NOTIFYICONDATA AppNodifyIcon;
BOOL isWinVisible=TRUE;


BOOL WINAPI Main_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    switch(uMsg)
    {
    	
        //BEGIN MESSAGE CRACK
        HANDLE_MSG(hWnd, WM_INITDIALOG, Main_OnInitDialog);
        HANDLE_MSG(hWnd, WM_COMMAND, Main_OnCommand);
        HANDLE_MSG(hWnd,WM_CLOSE, Main_OnClose);
        HANDLE_MSG(hWnd,WM_HSCROLL, Form1_OnHScroll);
        HANDLE_MSG(hWnd,WM_VSCROLL, Form1_OnVScroll);
        //WM_CONTEXTMENU 
        case WM_VKEYTOITEM:
        {
       		int nDeletingItem=SendMessage(hCtl.hLST_PLAYLIST,LB_GETCURSEL,0,0);
        	if(VK_DELETE==LOWORD(wParam))
        	{
        		WPARAM wParamsend=MAKEWPARAM((WORD)IDM_DELETE,0);
				SendMessage(hWnd,WM_COMMAND,wParamsend,(LPARAM)0);
        	}
        	SetWindowLong(hWnd, DWL_MSGRESULT, nDeletingItem);
        	return nDeletingItem;
		}

        //END MESSAGE CRACK
        case WM_RBUTTONDOWN:
        {
        	POINT PMenuPos;
			PMenuPos.x=GET_X_LPARAM(lParam);
			PMenuPos.y=GET_Y_LPARAM(lParam);
			ClientToScreen(hWnd,&PMenuPos);
			TrackPopupMenuEx(g_hmenu,TPM_LEFTALIGN|TPM_TOPALIGN,
						PMenuPos.x,PMenuPos.y,hWnd,NULL);
			
        	return TRUE;
		}

		
        case WM_CONTEXTMENU:
        {
        	if((HWND)wParam==hCtl.hLST_PLAYLIST)
        	{
        		 
        		int nTopIndex;
        		POINT point={GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
        		nTopIndex=SendMessage(hCtl.hLST_PLAYLIST,LB_GETTOPINDEX,0,0);
        		ScreenToClient(hCtl.hLST_PLAYLIST,&point);
        		SendMessage(hCtl.hLST_PLAYLIST,LB_SETCURSEL,nTopIndex+point.y/17,0); // 用右键模拟左键 
        		
				//如果播放列表中没有歌曲，则禁用相应的菜单项 和按钮 
				if(0==filesinfo.iCountOfFiles)
				{
					DisableControls(g_hPlayList_Menu,&hCtl);
				}
				TrackPopupMenuEx(g_hPlayList_Menu,TPM_LEFTALIGN|TPM_TOPALIGN,
							GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam),hWnd,NULL);

        	}
        	return TRUE;
		}

		
		/*实现激活时不透明*/
		case WM_ACTIVATE:
		{
			
			if(MF_CHECKED==GetMenuState(g_hmenu,IDM_OpaqueWhenActive,MF_BYCOMMAND)) // 如果选择了激活时不透明
			{ 
					
					if((WA_INACTIVE==LOWORD(wParam))&&
						0==HIWORD(wParam)) // 如果失去激活状态 
					{
						WPARAM wParamsend=MAKEWPARAM((WORD)g_CURTRPSelected,0);
						SendMessage(hWnd,WM_COMMAND,wParamsend,(LPARAM)0);
					}
					else // 如果激活 
					{
						SetLayeredWindowAttributes(hWnd,RGB(100,100,100),255,LWA_ALPHA);
					}
					
					SendMessage(hWnd,WM_PAINT,0,0);
			}
			return TRUE;
		}
 
		case WM_NOTIFY:
		{
			NMUPDOWN *lpnmud=(LPNMUPDOWN)lParam;
			if(lpnmud->hdr.idFrom==IDC_UDN1&&LB_ERR!=SendMessage(hCtl.hLST_PLAYLIST,LB_GETCURSEL,0,0))
			{
				if((lpnmud->iDelta)<0) // 如果小于0则按了向上键 
				{
			 		MoveUp(&filesinfo,&hCtl,&g_currentPlayIndex,hWnd);	
				}
				else
				{
					MoveDown(&filesinfo,&hCtl,&g_currentPlayIndex,hWnd);
				}
				SetDialogText(hWnd,g_currentPlayIndex,&hCtl,&AppNodifyIcon); // 重新设置标题栏 
			 	SetWindowLong(hWnd,DWL_MSGRESULT,TRUE);//
			}
			return TRUE;
		}

		

		case WM_SIZE:
		{
			if (SIZE_MINIMIZED==wParam)
	        {
	            ShowWindow(hWnd,SW_HIDE);
	        }
			return TRUE;
		}

		case WM_DROPFILES:
		{
			AddDropFilesToList((HDROP) wParam,&filesinfo,&hCtl,hWnd);
			return TRUE;
		}

    }

    return FALSE;
}
////////////////////////////////////////////////////////////////////////////////
//  Main_OnInitDialog
BOOL Main_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    // Set app icons
	HMENU hMenuPlayMode,hMenuTransparent,hMenuRate;
    static HMENU hPlayList_Menu,hpop_Menu;
    MENUITEMINFO sMIInfo;
    HICON hIcon = LoadIcon((HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE) ,MAKEINTRESOURCE(IDC_ICON1));
      
    SendMessage(hwnd, WM_SETICON, TRUE,  (LPARAM)hIcon);
    SendMessage(hwnd, WM_SETICON, FALSE, (LPARAM)hIcon);
	/////////////////////获取控件句柄，待用 /////////////////////////
    GetControlsHandle(&hCtl,hwnd);
    InitiateControls(&hCtl);
    AddNodifyIcon(hwnd,&AppNodifyIcon);
    
    //要使用透明效果，必须修改窗口属性
	SetWindowLong(hwnd,GWL_EXSTYLE,WS_EX_LAYERED); 
    SetLayeredWindowAttributes(hwnd,RGB(100,100,100),255,LWA_ALPHA);
      
    //
    // Add initializing code here
    
    /* 创建弹出式菜单 */
   	hMenuPlayMode=LoadMenu(hInst,MAKEINTRESOURCE(IDR_PlayMode));
   	hMenuTransparent=LoadMenu(hInst,MAKEINTRESOURCE(IDR_Transparent));
   	hMenuRate=LoadMenu(hInst,MAKEINTRESOURCE(IDR_Rate));
	
	
	hpop_Menu=CreatePopupMenu();
			
	AppendMenu(hpop_Menu,MF_POPUP|MF_STRING,(UINT_PTR)hMenuPlayMode,TEXT("播放模式")); 
	AppendMenu(hpop_Menu,MF_POPUP|MF_STRING,(UINT_PTR)hMenuTransparent,TEXT("透明度")); 
	AppendMenu(hpop_Menu,MF_POPUP|MF_STRING,(UINT_PTR)hMenuRate,TEXT("播放速率")); 
	
	//添加菜单项 
	sMIInfo.cbSize=sizeof(sMIInfo);
	sMIInfo.fMask=MIIM_ID|MIIM_STRING;
	sMIInfo.wID=IDC_TopMost;
	sMIInfo.dwTypeData=TEXT("总在最前");
	sMIInfo.cch=sizeof(TEXT("总在最前"))/sizeof(TCHAR);
	InsertMenuItem(hpop_Menu,3,TRUE,&sMIInfo);
	AppendMenu(hpop_Menu,MF_STRING,IDC_ABOUT,TEXT("关于"));
	g_hmenu=hpop_Menu;
//////////////////////////// 创建管理列表菜单 /////////////////////////////////////    
    hPlayList_Menu=CreatePopupMenu();
    g_hPlayList_Menu=hPlayList_Menu;
    AppendMenu(hPlayList_Menu,MF_STRING,IDM_PlayCurrentSelect,TEXT("播放"));
	AppendMenu(hPlayList_Menu,MF_STRING,IDM_AddFiles,TEXT("添加文件"));
	AppendMenu(hPlayList_Menu,MF_STRING,IDM_AddFolder,TEXT("添加文件夹"));
	AppendMenu(hPlayList_Menu,MF_STRING,IDM_DELETE,TEXT("删除"));
	AppendMenu(hPlayList_Menu,MF_STRING,IDM_CLEAR,TEXT("清空列表"));
/////////////////////////////////初始化菜单，设置菜单默认选项///////////////////////////
 	CheckMenuRadioItem(GetSubMenu(g_hmenu,0),IDM_Random,
	 		IDM_SingleLoop,IDM_Sortorder,MF_BYCOMMAND);
 	CheckMenuRadioItem(GetSubMenu(g_hmenu,1),IDM_Opaque,IDM_P90,IDM_Opaque,MF_BYCOMMAND);
	CheckMenuRadioItem(GetSubMenu(g_hmenu,2),IDM_Half,IDM_Twice,IDM_Nomal,MF_BYCOMMAND);
///////////////////////////////// 初始化filesinfo ///////////////////////
    ZeroMemory(filesinfo.szFilePath,sizeof(filesinfo.szFilePath));
    filesinfo.iCountOfFiles=0;

/////////////////////////////加载播放列表////////////////////////////// 
	if(LoadPlayList(&filesinfo,hInst))
	{
		AddFilesNameToPlayList(&filesinfo,&hCtl,hwnd,filesinfo.iCountOfFiles);
	}
    
    if(0==filesinfo.iCountOfFiles)
    {
		DisableControls(g_hPlayList_Menu,&hCtl);
	}
	DragAcceptFiles(hwnd,TRUE); // 允许拖拽文件,必须放在后面，不知道为何 
	
	
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//  Main_OnCommand
void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch(id)
    {
   	/////////////关于/////////////
	    case IDC_ABOUT:
		{
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DLGABOUT), NULL, Main_AboutProc);
			
		} 
		break;
		
		case IDM_CLOSE:
		{
			SendMessage(hwnd,WM_CLOSE,0,0);	
		}
		break;
		
//////////////////////处理通知图表消息////////////////////////////
		case IDC_NODIFYICON:
	    {
	        if ((UINT)hwndCtl==WM_LBUTTONDOWN)
	        {
	            if (isWinVisible)
	            {
	                ShowWindow(hwnd,SW_HIDE);
	                isWinVisible=FALSE;
	            }
	            else
	            {
	                ShowWindow(hwnd,SW_SHOWNORMAL);
	                SetForegroundWindow(hwnd);
	                isWinVisible=TRUE;
	            }
	        }
	        else if((UINT)hwndCtl==WM_RBUTTONDOWN)
	        {
	        	POINT rclickpoint;
	        	ShowWindow(hwnd,SW_SHOWNORMAL);
		          SetForegroundWindow(hwnd);
		          isWinVisible=TRUE;
	        	GetCursorPos(&rclickpoint);
	        	
	        	AppendMenu(g_hmenu,MF_STRING,IDM_CLOSE,TEXT("退出")); // 在右击图标的菜单中添加“退出” 
	        	TrackPopupMenuEx(g_hmenu,TPM_LEFTALIGN|TPM_TOPALIGN,
						rclickpoint.x,rclickpoint.y,hwnd,NULL);
					
				DeleteMenu(g_hmenu,IDM_CLOSE,MF_BYCOMMAND); // 删除刚才添加的菜单 
			}
	        
	    }
	    break;
	    
////////////////////////播放列表菜单///////////////////////////////
		case IDM_PlayCurrentSelect:
		{
			g_currentPlayIndex=SendMessage(hCtl.hLST_PLAYLIST,LB_GETCURSEL,0,0);
			PlaySpecifySong(hwnd,g_currentPlayIndex,&filesinfo,&hCtl);
			
		} 
		break;
		
		
		case IDM_AddFiles:
		{
			AddFiles(&filesinfo,hwnd,&hCtl);
			if(0!=filesinfo.iCountOfFiles) // 判断当前歌曲数目是否为零,使能相应的菜单项 
				{
					EnableControls(g_hPlayList_Menu,&hCtl);
					GetrandomNumbers(&rNums,&filesinfo); // 获取随机播放顺序
				}
			
		}
		break;
		
		
		case IDM_AddFolder:
		{
			AddFolder(&filesinfo,hwnd,&hCtl);
			if(0!=filesinfo.iCountOfFiles) // 判断当前歌曲数目是否为零,使能相应的菜单项 
				{
					EnableControls(g_hPlayList_Menu,&hCtl);
					GetrandomNumbers(&rNums,&filesinfo); // 获取随机播放顺序
					
				}
			
		}
    	break;
    	
    	
    	case IDM_CLEAR: //清空列表
		{
			Stop_Music();
			SendMessage(hCtl.hLST_PLAYLIST,LB_RESETCONTENT,0,0);
			ZeroMemory(&filesinfo,sizeof(filesinfo));
			DisableControls(g_hPlayList_Menu,&hCtl);
			
		} 
		break;
		
		case IDM_DELETE:
		{
			int nCurrentSelected=SendMessage(hCtl.hLST_PLAYLIST,LB_GETCURSEL,0,0);
			DeleteItem(&filesinfo,nCurrentSelected,&hCtl,hwnd);
			if(filesinfo.iCountOfFiles==0)
			{
				DisableControls(g_hPlayList_Menu,&hCtl);
			}
			else if(g_currentPlayIndex==nCurrentSelected) // 如果当前要删除的歌曲正在播放，则将停止
			{
				Stop_Music();
				ResetControls(hwnd);
			} 
			else if(g_currentPlayIndex>nCurrentSelected) // 如果删除的歌曲在当前播放歌曲的前面，则调整当前歌曲的索引  
			{
				g_currentPlayIndex--;
				SetDialogText(hwnd,g_currentPlayIndex,&hCtl,&AppNodifyIcon);
			}
			
		}
		break;
		
		
///////////////////总在最前///////////////////////////////////////////////// 
    	case IDC_TopMost:
    	{
  			if(MF_CHECKED==GetMenuState(g_hmenu,IDC_TopMost,MF_BYCOMMAND)) // 如果选择了总在最前 
			{ 
				CheckMenuItem(g_hmenu,IDC_TopMost,MF_BYCOMMAND|MF_UNCHECKED); // 去掉前面的钩
				SetWindowPos(hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
			}
			else
			{
				CheckMenuItem(g_hmenu,IDC_TopMost,MF_BYCOMMAND|MF_CHECKED); // 在前面打钩 
				SetWindowPos(hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
			}
    		
		}
		break;
		
///////////////////////////设置播放模式菜单////////////////////////////////////////////////////// 
    	
    	/*随机播放*/
		case IDM_Random:
		{
			SetPlayModeMenu(g_hmenu,&g_CURPMSelected,IDM_Random);
			GetrandomNumbers(&rNums,&filesinfo); // 获取随机播放顺序
			
		}
		break;
		
		/*顺序播放*/
		case IDM_Sortorder:
		{
			SetPlayModeMenu(g_hmenu,&g_CURPMSelected,IDM_Sortorder);
			
		}
		break;
		
		/*循环播放*/
		case IDM_Loop:
		{
			SetPlayModeMenu(g_hmenu,&g_CURPMSelected,IDM_Loop);
			
		}
		break;
		
		/*单曲播放*/ 
		case IDM_Single:
		{
			SetPlayModeMenu(g_hmenu,&g_CURPMSelected,IDM_Single);
			
		}
		break;
		
		/*单曲循环*/ 
		case IDM_SingleLoop:
		{
			SetPlayModeMenu(g_hmenu,&g_CURPMSelected,IDM_SingleLoop);
			
		}
		break;
	
/////////////////////设置透明度菜单///////////////////////////////////////////////////////// 
		
		/*10%透明*/
		case IDM_P10:
		{
			SetTransparentMune(g_hmenu,&g_CURTRPSelected,IDM_P10);
			SetLayeredWindowAttributes(hwnd,RGB(100,100,100),230,LWA_ALPHA);
			
		}
		break;
		
		/*20%透明*/
		case IDM_P20:
		{
			SetTransparentMune(g_hmenu,&g_CURTRPSelected,IDM_P20);
			SetLayeredWindowAttributes(hwnd,RGB(100,100,100),204,LWA_ALPHA);
			
		}
		break;
		
		/*30%透明*/
		case IDM_P30:
		{
			SetTransparentMune(g_hmenu,&g_CURTRPSelected,IDM_P30);
			SetLayeredWindowAttributes(hwnd,RGB(100,100,100),179,LWA_ALPHA);
			
		}
		break;
		
		/*40%透明*/
		case IDM_P40:
		{
			SetTransparentMune(g_hmenu,&g_CURTRPSelected,IDM_P40);
			SetLayeredWindowAttributes(hwnd,RGB(100,100,100),153,LWA_ALPHA);
			
		}
		break;
		
		/*50%透明*/
		case IDM_P50:
		{
			SetTransparentMune(g_hmenu,&g_CURTRPSelected,IDM_P50);
			SetLayeredWindowAttributes(hwnd,RGB(100,100,100),128,LWA_ALPHA);
			
		}
		break;
		
		/*60%透明*/
		case IDM_P60:
		{
			SetTransparentMune(g_hmenu,&g_CURTRPSelected,IDM_P60);
			SetLayeredWindowAttributes(hwnd,RGB(100,100,100),102,LWA_ALPHA);
			
		}
		break;
		
		/*70%透明*/
		case IDM_P70:
		{
			SetTransparentMune(g_hmenu,&g_CURTRPSelected,IDM_P70);
			SetLayeredWindowAttributes(hwnd,RGB(100,100,100),77,LWA_ALPHA);
			
		}
		break;
		
		/*80%透明*/
		case IDM_P80:
		{
			SetTransparentMune(g_hmenu,&g_CURTRPSelected,IDM_P80);
			SetLayeredWindowAttributes(hwnd,RGB(100,100,100),51,LWA_ALPHA);
			
		}
		break;
		
		/*90%透明*/
		case IDM_P90:
		{
			SetTransparentMune(g_hmenu,&g_CURTRPSelected,IDM_P90);
			SetLayeredWindowAttributes(hwnd,RGB(100,100,100),20,LWA_ALPHA);
			
		}
		break;
		
		/*不透明*/
		case IDM_Opaque:
		{
			SetTransparentMune(g_hmenu,&g_CURTRPSelected,IDM_Opaque);
			SetLayeredWindowAttributes(hwnd,RGB(100,100,100),255,LWA_ALPHA);
			
		}
		break;

		/*激活时不透明*/
		case IDM_OpaqueWhenActive:
		{
			//判断 “激活时不透明”是否已经被选中 
			if(MF_CHECKED==GetMenuState(g_hmenu,IDM_OpaqueWhenActive,MF_BYCOMMAND))
			{
				
				WPARAM wParamsend=MAKEWPARAM((WORD)g_CURTRPSelected,0);
				SendMessage(hwnd,WM_COMMAND,wParamsend,(LPARAM)0);
				CheckMenuItem(g_hmenu,IDM_OpaqueWhenActive,MF_BYCOMMAND|MF_UNCHECKED); // 去掉前面的钩
			}
			else
			{
				CheckMenuItem(g_hmenu,IDM_OpaqueWhenActive,MF_BYCOMMAND|MF_CHECKED); // 在前面打钩 
				SetLayeredWindowAttributes(hwnd,RGB(100,100,100),255,LWA_ALPHA);
			}
			
		}
		break;
		
///////////////////////////设置播放速率///////////////////////////////////

		case IDM_Half:
		{
			HMENU hSubMenu1=GetSubMenu(g_hmenu,2);
			SetMusicRate(0.5);
			CheckMenuRadioItem(hSubMenu1,IDM_Half,IDM_Twice,IDM_Half,MF_BYCOMMAND);			
			
		} 
		break;
		
		case IDM_Nomal:
		{
			HMENU hSubMenu1=GetSubMenu(g_hmenu,2);
			SetMusicRate(1);
			CheckMenuRadioItem(hSubMenu1,IDM_Half,IDM_Twice,IDM_Nomal,MF_BYCOMMAND);			
			
		} 
		break;
		
		case IDM_Twice:
		{
			HMENU hSubMenu1=GetSubMenu(g_hmenu,2);
			SetMusicRate(2);
			CheckMenuRadioItem(hSubMenu1,IDM_Half,IDM_Twice,IDM_Twice,MF_BYCOMMAND);			
			
		} 
		break;
		
////////////////////////////命令按钮/////////////////////////////////////////////
 
 		//停止按钮 
        case IDC_BTN_STOP:
        {
			Stop_Music();//停止播放器 
			ResetControls(hwnd);
        	
		}
		break;
		
		//播放暂停按钮 
		case IDC_BTN_PLAY_PAUSE:
		{	
			OAFilterState oafs;
			GetCurrentState(&oafs);
			
			if(State_Running==oafs) // 如果当前状态为播放，则暂停 
			{
				Pause_Music();
				SetDlgItemText(hwnd,IDC_BTN_PLAY_PAUSE,TEXT("播放")); 
			}
			
			else if(State_Paused==oafs) // 如果当前状态为暂停 ，则播放
			{
				Play_Music();
				SetDlgItemText(hwnd,IDC_BTN_PLAY_PAUSE,TEXT("暂停")); 
			}
			else //播放第一首 
			{
				PlaySpecifySong(hwnd,0,&filesinfo,&hCtl);
			}
			
		}
		break;
		
		//播放列表 
		case IDC_LST_PLAYLIST:
		{
			if(LBN_DBLCLK==codeNotify)
			{
			 	g_currentPlayIndex=SendMessage(hCtl.hLST_PLAYLIST,LB_GETCURSEL,0,0); // 获取当前双击的歌曲的索引 
			 	PlaySpecifySong(hwnd,g_currentPlayIndex,&filesinfo,&hCtl);
			} 
			
		}
		break;
		
		// 静音 
		case IDC_CHK_MUTE:
		{
			GetVolumeSate(&hCtl);
			
		}
		break;
		
		//下一曲
		case IDC_BTN_NEXT:
		{

			ResetControls(hwnd);
			g_currentPlayIndex=GetNextPlayIndex(g_CURPMSelected,g_currentPlayIndex,
					&rNums,&filesinfo);
			if(-1!=g_currentPlayIndex)
			{
				PlaySpecifySong(hwnd,g_currentPlayIndex,&filesinfo,&hCtl);
			}
			else
			{
				Stop_Music();
			}	
			
		} 
		break;
		
		//上一曲
		case IDC_BTN_ABOVE:
		{
			ResetControls(hwnd);
			g_currentPlayIndex=GetAbovePlayIndex(g_CURPMSelected,g_currentPlayIndex,
					&rNums,&filesinfo);
			if(-1!=g_currentPlayIndex)
			{
				PlaySpecifySong(hwnd,g_currentPlayIndex,&filesinfo,&hCtl);
			}
			else
			{
				Stop_Music();
			}			
			
		} 
		break;
		
        default:break;
    }

}

////////////////////////////////////////////////////////////////////////////////
//  Main_OnClose
void Main_OnClose(HWND hwnd)
{
	Stop_Music(); // 停止当前播放的音乐
	SavePlayList(&filesinfo,hInst);
	DestroyMenu(g_hmenu); // 释放菜单 
	DestroyMenu(g_hPlayList_Menu); // 释放菜单
	Shell_NotifyIcon(NIM_DELETE,(PNOTIFYICONDATA)&AppNodifyIcon);
    EndDialog(hwnd, 0);
}

void Form1_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	
	if(hCtl.hTRB_PROGRESS==hwndCtl)
	{
		if(SB_THUMBTRACK==code)
		{
			SetPlayerPosition(pos);
		}
		else 
		{
			int nCurrentpos;
			GetPlayerCurrentPosition(&nCurrentpos);
			SendMessage(hCtl.hTRB_PROGRESS,TBM_SETPOS,TRUE,nCurrentpos
			);
		}
	}
	
	else if(hCtl.hTRB_SETBALANCE==hwndCtl) // 设置声道平衡 
	{
		if(code==TB_THUMBPOSITION||code==TB_LINEUP||code==TB_LINEDOWN||
	                code==TB_PAGEDOWN||code==TB_PAGEUP)
  		{
        	long ncurrentpos=SendMessage(hCtl.hTRB_SETBALANCE,TBM_GETPOS,0,0);
        	long newBalance=(100-ncurrentpos)*100;
          	SetPlayerBalance(newBalance);
  		}
	}
}



VOID CALLBACK TimerProc(
  HWND hwnd,         // handle to window
  UINT uMsg,         // WM_TIMER message
  UINT_PTR idEvent,  // timer identifier
  DWORD dwTime       // current system time
						)
{
	OAFilterState fState;
	GetCurrentState(&fState);
	
	
	if(State_Stopped!=fState) //假如没有停止 
	{
		int nCurrentpos;
		TCHAR szCurrenttime[10],szRemaintime[10];
		
		GetPlayerCurrentPosition(&nCurrentpos); // 获取歌曲当前位置 
		
		GetCurrentAndRemain(szCurrenttime,sizeof(szCurrenttime)/sizeof(TCHAR),
					szRemaintime,sizeof(szRemaintime)/sizeof(TCHAR)); // 获取当前和剩余时间 
		SetDlgItemText(hwnd,IDC_STC_CURRENT,szCurrenttime);
		SetDlgItemText(hwnd,IDC_STC_REMAIN,szRemaintime);
		SendMessage(hCtl.hTRB_PROGRESS,TBM_SETPOS,TRUE,(LPARAM)nCurrentpos);
	}
	else // 如果已经停止 
	{
		
		ResetControls(hwnd);
		if(IDM_Random==g_CURPMSelected) // 传递当前随机顺序数组的下标(即传递第几个随机数) 
		{
			g_currentPlayIndex=GetNextPlayIndex(g_CURPMSelected,rNums.nNumsCount,
									&rNums,&filesinfo);											
		}
		else if(IDM_SingleLoop==g_CURPMSelected)
		{
			//什么都不做，即不改变当前播放索引，达到单曲循环的目的 
		}
		else // 假如当前不是随机播放，则传递当前的索引 
		{
			g_currentPlayIndex=GetNextPlayIndex(g_CURPMSelected,g_currentPlayIndex,
									&rNums,&filesinfo);
		}
		
		if(g_currentPlayIndex!=-1)
		{
			PlaySpecifySong(hwnd,g_currentPlayIndex,&filesinfo,&hCtl);
		}
		else
		{
			Stop_Music();
		}
	}
}



void Form1_OnVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	if(hCtl.hTRB_SETVOLUME==hwndCtl)
	{
 			if(code==TB_THUMBPOSITION||code==TB_LINEUP||code==TB_LINEDOWN||
	                code==TB_PAGEDOWN||code==TB_PAGEUP)
	        {
	        	int pos=SendMessage(hCtl.hTRB_SETVOLUME,TBM_GETPOS,0,0);
	            SetPlayerVolume(-(pos)*10);
	        }
	}
}

void ResetControls(HWND hdlg)
{
	SendMessage(hCtl.hTRB_PROGRESS,TBM_SETPOS,TRUE,(LPARAM)0);// 将滚动条归零 
	SetDlgItemText(hdlg,IDC_STC_CURRENT,TEXT(""));
	SetDlgItemText(hdlg,IDC_STC_REMAIN,TEXT(""));
	EnableWindow(hCtl.hTRB_PROGRESS,FALSE);
	SetWindowText(hdlg,TEXT(""));
	SetDlgItemText(hdlg,IDC_BTN_PLAY_PAUSE,TEXT("播放"));
	KillTimer(hdlg,IDC_TimerID);
}


// 将歌曲名显示在标题栏中 
void SetDialogText(HWND hdlg,int nTextIndex,LPHCONTROLS lphctl,PNOTIFYICONDATA pAppNodifyIcon)
{
	TCHAR szCurrentText[MAX_PATH];
		
 	SendMessage(lphctl->hLST_PLAYLIST,LB_GETTEXT,
			(WPARAM)nTextIndex,(LPARAM)szCurrentText);
 	SetWindowText(hdlg,szCurrentText); // 将当前播放的歌曲名显示在标题栏中 

 	StringCchCopy(pAppNodifyIcon->szTip,128,szCurrentText);
 	pAppNodifyIcon->uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;
 	Shell_NotifyIcon(NIM_MODIFY,pAppNodifyIcon);
}

void PlaySpecifySong(HWND hDlg,int index,LPFILESINFO lpfi,LPHCONTROLS lphctl)
{
		HRESULT hr;
		int nSongLen;
		SetDialogText(hDlg,g_currentPlayIndex,&hCtl,&AppNodifyIcon);
	 	Stop_Music(); // 停止之前的播放 
	 	
		hr=SpecifyMusicFile(lpfi->szFilePath[index]); // 指定要播放的文件 
		if(hr!=S_OK)
		{					
			// 如文件不存在，文件无法识别等，将会失败 
			return;
		}
		SetTimer(hDlg,(UINT_PTR)IDC_TimerID,(UINT)1000,(TIMERPROC)TimerProc); // 启动计时器
		Play_Music(); // 开始播放  
		EnableWindow(hCtl.hTRB_PROGRESS,TRUE);
		
		SendMessage(hCtl.hLST_PLAYLIST,LB_SETCURSEL,(WPARAM)index,0); // 指定当前播放的项 ;
		
		SetDlgItemText(hDlg,IDC_BTN_PLAY_PAUSE,TEXT("暂停")); // 将 IDC_BTN_PLAY_PAUSE按钮文本设置为暂停 
		
		Get_Length(&nSongLen); // 获取歌曲长度，然后设置播放滚动条的长度 
		SendMessage(lphctl->hTRB_PROGRESS,TBM_SETRANGE,(WPARAM)TRUE,MAKELONG(0,nSongLen));
		GetVolumeSate(lphctl);
}


void GetVolumeSate(LPHCONTROLS lpcctls)
{
	int chbstate=SendMessage(lpcctls->hCHK_MUTE,BM_GETCHECK,0,0);
	if(BST_CHECKED==chbstate)
	{
		SetPlayerVolume(-10000);
	}
	else if(BST_UNCHECKED==chbstate)
	{
		long lpos;
		lpos=SendMessage(lpcctls->hTRB_SETVOLUME,TBM_GETPOS,0,0);
		SetPlayerVolume(-lpos*10);
	}
}
