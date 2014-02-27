#define _WIN32_WINNT 0x0501
#define _WIN32_IE 0x0600
#define NO_WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE  //������������Ϊ��ʹ��UNICODE���API 
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



HCONTROLS hCtl; // ��ſؼ���� 
FILESINFO filesinfo; // ����б���Ϣ 
RANDOMNUMBERS rNums; // ����������ʱ����� 
HMENU g_hmenu,g_hPlayList_Menu; // ��Ų˵����
UINT g_CURTRPSelected=IDM_Opaque; // ���͸���˵���ǰѡ����� ��Ĭ��Ϊ��͸�� 
UINT g_CURPMSelected=IDM_Sortorder; // ��Ų���ģʽ�˵���ǰѡ�Ĭ��Ϊ ˳�򲥷� 
int g_currentPlayIndex; // ��ǰ���ŵ����� 

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
        		SendMessage(hCtl.hLST_PLAYLIST,LB_SETCURSEL,nTopIndex+point.y/17,0); // ���Ҽ�ģ����� 
        		
				//��������б���û�и������������Ӧ�Ĳ˵��� �Ͱ�ť 
				if(0==filesinfo.iCountOfFiles)
				{
					DisableControls(g_hPlayList_Menu,&hCtl);
				}
				TrackPopupMenuEx(g_hPlayList_Menu,TPM_LEFTALIGN|TPM_TOPALIGN,
							GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam),hWnd,NULL);

        	}
        	return TRUE;
		}

		
		/*ʵ�ּ���ʱ��͸��*/
		case WM_ACTIVATE:
		{
			
			if(MF_CHECKED==GetMenuState(g_hmenu,IDM_OpaqueWhenActive,MF_BYCOMMAND)) // ���ѡ���˼���ʱ��͸��
			{ 
					
					if((WA_INACTIVE==LOWORD(wParam))&&
						0==HIWORD(wParam)) // ���ʧȥ����״̬ 
					{
						WPARAM wParamsend=MAKEWPARAM((WORD)g_CURTRPSelected,0);
						SendMessage(hWnd,WM_COMMAND,wParamsend,(LPARAM)0);
					}
					else // ������� 
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
				if((lpnmud->iDelta)<0) // ���С��0�������ϼ� 
				{
			 		MoveUp(&filesinfo,&hCtl,&g_currentPlayIndex,hWnd);	
				}
				else
				{
					MoveDown(&filesinfo,&hCtl,&g_currentPlayIndex,hWnd);
				}
				SetDialogText(hWnd,g_currentPlayIndex,&hCtl,&AppNodifyIcon); // �������ñ����� 
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
	/////////////////////��ȡ�ؼ���������� /////////////////////////
    GetControlsHandle(&hCtl,hwnd);
    InitiateControls(&hCtl);
    AddNodifyIcon(hwnd,&AppNodifyIcon);
    
    //Ҫʹ��͸��Ч���������޸Ĵ�������
	SetWindowLong(hwnd,GWL_EXSTYLE,WS_EX_LAYERED); 
    SetLayeredWindowAttributes(hwnd,RGB(100,100,100),255,LWA_ALPHA);
      
    //
    // Add initializing code here
    
    /* ��������ʽ�˵� */
   	hMenuPlayMode=LoadMenu(hInst,MAKEINTRESOURCE(IDR_PlayMode));
   	hMenuTransparent=LoadMenu(hInst,MAKEINTRESOURCE(IDR_Transparent));
   	hMenuRate=LoadMenu(hInst,MAKEINTRESOURCE(IDR_Rate));
	
	
	hpop_Menu=CreatePopupMenu();
			
	AppendMenu(hpop_Menu,MF_POPUP|MF_STRING,(UINT_PTR)hMenuPlayMode,TEXT("����ģʽ")); 
	AppendMenu(hpop_Menu,MF_POPUP|MF_STRING,(UINT_PTR)hMenuTransparent,TEXT("͸����")); 
	AppendMenu(hpop_Menu,MF_POPUP|MF_STRING,(UINT_PTR)hMenuRate,TEXT("��������")); 
	
	//��Ӳ˵��� 
	sMIInfo.cbSize=sizeof(sMIInfo);
	sMIInfo.fMask=MIIM_ID|MIIM_STRING;
	sMIInfo.wID=IDC_TopMost;
	sMIInfo.dwTypeData=TEXT("������ǰ");
	sMIInfo.cch=sizeof(TEXT("������ǰ"))/sizeof(TCHAR);
	InsertMenuItem(hpop_Menu,3,TRUE,&sMIInfo);
	AppendMenu(hpop_Menu,MF_STRING,IDC_ABOUT,TEXT("����"));
	g_hmenu=hpop_Menu;
//////////////////////////// ���������б�˵� /////////////////////////////////////    
    hPlayList_Menu=CreatePopupMenu();
    g_hPlayList_Menu=hPlayList_Menu;
    AppendMenu(hPlayList_Menu,MF_STRING,IDM_PlayCurrentSelect,TEXT("����"));
	AppendMenu(hPlayList_Menu,MF_STRING,IDM_AddFiles,TEXT("����ļ�"));
	AppendMenu(hPlayList_Menu,MF_STRING,IDM_AddFolder,TEXT("����ļ���"));
	AppendMenu(hPlayList_Menu,MF_STRING,IDM_DELETE,TEXT("ɾ��"));
	AppendMenu(hPlayList_Menu,MF_STRING,IDM_CLEAR,TEXT("����б�"));
/////////////////////////////////��ʼ���˵������ò˵�Ĭ��ѡ��///////////////////////////
 	CheckMenuRadioItem(GetSubMenu(g_hmenu,0),IDM_Random,
	 		IDM_SingleLoop,IDM_Sortorder,MF_BYCOMMAND);
 	CheckMenuRadioItem(GetSubMenu(g_hmenu,1),IDM_Opaque,IDM_P90,IDM_Opaque,MF_BYCOMMAND);
	CheckMenuRadioItem(GetSubMenu(g_hmenu,2),IDM_Half,IDM_Twice,IDM_Nomal,MF_BYCOMMAND);
///////////////////////////////// ��ʼ��filesinfo ///////////////////////
    ZeroMemory(filesinfo.szFilePath,sizeof(filesinfo.szFilePath));
    filesinfo.iCountOfFiles=0;

/////////////////////////////���ز����б�////////////////////////////// 
	if(LoadPlayList(&filesinfo,hInst))
	{
		AddFilesNameToPlayList(&filesinfo,&hCtl,hwnd,filesinfo.iCountOfFiles);
	}
    
    if(0==filesinfo.iCountOfFiles)
    {
		DisableControls(g_hPlayList_Menu,&hCtl);
	}
	DragAcceptFiles(hwnd,TRUE); // ������ק�ļ�,������ں��棬��֪��Ϊ�� 
	
	
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//  Main_OnCommand
void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch(id)
    {
   	/////////////����/////////////
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
		
//////////////////////����֪ͨͼ����Ϣ////////////////////////////
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
	        	
	        	AppendMenu(g_hmenu,MF_STRING,IDM_CLOSE,TEXT("�˳�")); // ���һ�ͼ��Ĳ˵�����ӡ��˳��� 
	        	TrackPopupMenuEx(g_hmenu,TPM_LEFTALIGN|TPM_TOPALIGN,
						rclickpoint.x,rclickpoint.y,hwnd,NULL);
					
				DeleteMenu(g_hmenu,IDM_CLOSE,MF_BYCOMMAND); // ɾ���ղ���ӵĲ˵� 
			}
	        
	    }
	    break;
	    
////////////////////////�����б�˵�///////////////////////////////
		case IDM_PlayCurrentSelect:
		{
			g_currentPlayIndex=SendMessage(hCtl.hLST_PLAYLIST,LB_GETCURSEL,0,0);
			PlaySpecifySong(hwnd,g_currentPlayIndex,&filesinfo,&hCtl);
			
		} 
		break;
		
		
		case IDM_AddFiles:
		{
			AddFiles(&filesinfo,hwnd,&hCtl);
			if(0!=filesinfo.iCountOfFiles) // �жϵ�ǰ������Ŀ�Ƿ�Ϊ��,ʹ����Ӧ�Ĳ˵��� 
				{
					EnableControls(g_hPlayList_Menu,&hCtl);
					GetrandomNumbers(&rNums,&filesinfo); // ��ȡ�������˳��
				}
			
		}
		break;
		
		
		case IDM_AddFolder:
		{
			AddFolder(&filesinfo,hwnd,&hCtl);
			if(0!=filesinfo.iCountOfFiles) // �жϵ�ǰ������Ŀ�Ƿ�Ϊ��,ʹ����Ӧ�Ĳ˵��� 
				{
					EnableControls(g_hPlayList_Menu,&hCtl);
					GetrandomNumbers(&rNums,&filesinfo); // ��ȡ�������˳��
					
				}
			
		}
    	break;
    	
    	
    	case IDM_CLEAR: //����б�
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
			else if(g_currentPlayIndex==nCurrentSelected) // �����ǰҪɾ���ĸ������ڲ��ţ���ֹͣ
			{
				Stop_Music();
				ResetControls(hwnd);
			} 
			else if(g_currentPlayIndex>nCurrentSelected) // ���ɾ���ĸ����ڵ�ǰ���Ÿ�����ǰ�棬�������ǰ����������  
			{
				g_currentPlayIndex--;
				SetDialogText(hwnd,g_currentPlayIndex,&hCtl,&AppNodifyIcon);
			}
			
		}
		break;
		
		
///////////////////������ǰ///////////////////////////////////////////////// 
    	case IDC_TopMost:
    	{
  			if(MF_CHECKED==GetMenuState(g_hmenu,IDC_TopMost,MF_BYCOMMAND)) // ���ѡ����������ǰ 
			{ 
				CheckMenuItem(g_hmenu,IDC_TopMost,MF_BYCOMMAND|MF_UNCHECKED); // ȥ��ǰ��Ĺ�
				SetWindowPos(hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
			}
			else
			{
				CheckMenuItem(g_hmenu,IDC_TopMost,MF_BYCOMMAND|MF_CHECKED); // ��ǰ��� 
				SetWindowPos(hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
			}
    		
		}
		break;
		
///////////////////////////���ò���ģʽ�˵�////////////////////////////////////////////////////// 
    	
    	/*�������*/
		case IDM_Random:
		{
			SetPlayModeMenu(g_hmenu,&g_CURPMSelected,IDM_Random);
			GetrandomNumbers(&rNums,&filesinfo); // ��ȡ�������˳��
			
		}
		break;
		
		/*˳�򲥷�*/
		case IDM_Sortorder:
		{
			SetPlayModeMenu(g_hmenu,&g_CURPMSelected,IDM_Sortorder);
			
		}
		break;
		
		/*ѭ������*/
		case IDM_Loop:
		{
			SetPlayModeMenu(g_hmenu,&g_CURPMSelected,IDM_Loop);
			
		}
		break;
		
		/*��������*/ 
		case IDM_Single:
		{
			SetPlayModeMenu(g_hmenu,&g_CURPMSelected,IDM_Single);
			
		}
		break;
		
		/*����ѭ��*/ 
		case IDM_SingleLoop:
		{
			SetPlayModeMenu(g_hmenu,&g_CURPMSelected,IDM_SingleLoop);
			
		}
		break;
	
/////////////////////����͸���Ȳ˵�///////////////////////////////////////////////////////// 
		
		/*10%͸��*/
		case IDM_P10:
		{
			SetTransparentMune(g_hmenu,&g_CURTRPSelected,IDM_P10);
			SetLayeredWindowAttributes(hwnd,RGB(100,100,100),230,LWA_ALPHA);
			
		}
		break;
		
		/*20%͸��*/
		case IDM_P20:
		{
			SetTransparentMune(g_hmenu,&g_CURTRPSelected,IDM_P20);
			SetLayeredWindowAttributes(hwnd,RGB(100,100,100),204,LWA_ALPHA);
			
		}
		break;
		
		/*30%͸��*/
		case IDM_P30:
		{
			SetTransparentMune(g_hmenu,&g_CURTRPSelected,IDM_P30);
			SetLayeredWindowAttributes(hwnd,RGB(100,100,100),179,LWA_ALPHA);
			
		}
		break;
		
		/*40%͸��*/
		case IDM_P40:
		{
			SetTransparentMune(g_hmenu,&g_CURTRPSelected,IDM_P40);
			SetLayeredWindowAttributes(hwnd,RGB(100,100,100),153,LWA_ALPHA);
			
		}
		break;
		
		/*50%͸��*/
		case IDM_P50:
		{
			SetTransparentMune(g_hmenu,&g_CURTRPSelected,IDM_P50);
			SetLayeredWindowAttributes(hwnd,RGB(100,100,100),128,LWA_ALPHA);
			
		}
		break;
		
		/*60%͸��*/
		case IDM_P60:
		{
			SetTransparentMune(g_hmenu,&g_CURTRPSelected,IDM_P60);
			SetLayeredWindowAttributes(hwnd,RGB(100,100,100),102,LWA_ALPHA);
			
		}
		break;
		
		/*70%͸��*/
		case IDM_P70:
		{
			SetTransparentMune(g_hmenu,&g_CURTRPSelected,IDM_P70);
			SetLayeredWindowAttributes(hwnd,RGB(100,100,100),77,LWA_ALPHA);
			
		}
		break;
		
		/*80%͸��*/
		case IDM_P80:
		{
			SetTransparentMune(g_hmenu,&g_CURTRPSelected,IDM_P80);
			SetLayeredWindowAttributes(hwnd,RGB(100,100,100),51,LWA_ALPHA);
			
		}
		break;
		
		/*90%͸��*/
		case IDM_P90:
		{
			SetTransparentMune(g_hmenu,&g_CURTRPSelected,IDM_P90);
			SetLayeredWindowAttributes(hwnd,RGB(100,100,100),20,LWA_ALPHA);
			
		}
		break;
		
		/*��͸��*/
		case IDM_Opaque:
		{
			SetTransparentMune(g_hmenu,&g_CURTRPSelected,IDM_Opaque);
			SetLayeredWindowAttributes(hwnd,RGB(100,100,100),255,LWA_ALPHA);
			
		}
		break;

		/*����ʱ��͸��*/
		case IDM_OpaqueWhenActive:
		{
			//�ж� ������ʱ��͸�����Ƿ��Ѿ���ѡ�� 
			if(MF_CHECKED==GetMenuState(g_hmenu,IDM_OpaqueWhenActive,MF_BYCOMMAND))
			{
				
				WPARAM wParamsend=MAKEWPARAM((WORD)g_CURTRPSelected,0);
				SendMessage(hwnd,WM_COMMAND,wParamsend,(LPARAM)0);
				CheckMenuItem(g_hmenu,IDM_OpaqueWhenActive,MF_BYCOMMAND|MF_UNCHECKED); // ȥ��ǰ��Ĺ�
			}
			else
			{
				CheckMenuItem(g_hmenu,IDM_OpaqueWhenActive,MF_BYCOMMAND|MF_CHECKED); // ��ǰ��� 
				SetLayeredWindowAttributes(hwnd,RGB(100,100,100),255,LWA_ALPHA);
			}
			
		}
		break;
		
///////////////////////////���ò�������///////////////////////////////////

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
		
////////////////////////////���ť/////////////////////////////////////////////
 
 		//ֹͣ��ť 
        case IDC_BTN_STOP:
        {
			Stop_Music();//ֹͣ������ 
			ResetControls(hwnd);
        	
		}
		break;
		
		//������ͣ��ť 
		case IDC_BTN_PLAY_PAUSE:
		{	
			OAFilterState oafs;
			GetCurrentState(&oafs);
			
			if(State_Running==oafs) // �����ǰ״̬Ϊ���ţ�����ͣ 
			{
				Pause_Music();
				SetDlgItemText(hwnd,IDC_BTN_PLAY_PAUSE,TEXT("����")); 
			}
			
			else if(State_Paused==oafs) // �����ǰ״̬Ϊ��ͣ ���򲥷�
			{
				Play_Music();
				SetDlgItemText(hwnd,IDC_BTN_PLAY_PAUSE,TEXT("��ͣ")); 
			}
			else //���ŵ�һ�� 
			{
				PlaySpecifySong(hwnd,0,&filesinfo,&hCtl);
			}
			
		}
		break;
		
		//�����б� 
		case IDC_LST_PLAYLIST:
		{
			if(LBN_DBLCLK==codeNotify)
			{
			 	g_currentPlayIndex=SendMessage(hCtl.hLST_PLAYLIST,LB_GETCURSEL,0,0); // ��ȡ��ǰ˫���ĸ��������� 
			 	PlaySpecifySong(hwnd,g_currentPlayIndex,&filesinfo,&hCtl);
			} 
			
		}
		break;
		
		// ���� 
		case IDC_CHK_MUTE:
		{
			GetVolumeSate(&hCtl);
			
		}
		break;
		
		//��һ��
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
		
		//��һ��
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
	Stop_Music(); // ֹͣ��ǰ���ŵ�����
	SavePlayList(&filesinfo,hInst);
	DestroyMenu(g_hmenu); // �ͷŲ˵� 
	DestroyMenu(g_hPlayList_Menu); // �ͷŲ˵�
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
	
	else if(hCtl.hTRB_SETBALANCE==hwndCtl) // ��������ƽ�� 
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
	
	
	if(State_Stopped!=fState) //����û��ֹͣ 
	{
		int nCurrentpos;
		TCHAR szCurrenttime[10],szRemaintime[10];
		
		GetPlayerCurrentPosition(&nCurrentpos); // ��ȡ������ǰλ�� 
		
		GetCurrentAndRemain(szCurrenttime,sizeof(szCurrenttime)/sizeof(TCHAR),
					szRemaintime,sizeof(szRemaintime)/sizeof(TCHAR)); // ��ȡ��ǰ��ʣ��ʱ�� 
		SetDlgItemText(hwnd,IDC_STC_CURRENT,szCurrenttime);
		SetDlgItemText(hwnd,IDC_STC_REMAIN,szRemaintime);
		SendMessage(hCtl.hTRB_PROGRESS,TBM_SETPOS,TRUE,(LPARAM)nCurrentpos);
	}
	else // ����Ѿ�ֹͣ 
	{
		
		ResetControls(hwnd);
		if(IDM_Random==g_CURPMSelected) // ���ݵ�ǰ���˳��������±�(�����ݵڼ��������) 
		{
			g_currentPlayIndex=GetNextPlayIndex(g_CURPMSelected,rNums.nNumsCount,
									&rNums,&filesinfo);											
		}
		else if(IDM_SingleLoop==g_CURPMSelected)
		{
			//ʲô�������������ı䵱ǰ�����������ﵽ����ѭ����Ŀ�� 
		}
		else // ���統ǰ����������ţ��򴫵ݵ�ǰ������ 
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
	SendMessage(hCtl.hTRB_PROGRESS,TBM_SETPOS,TRUE,(LPARAM)0);// ������������ 
	SetDlgItemText(hdlg,IDC_STC_CURRENT,TEXT(""));
	SetDlgItemText(hdlg,IDC_STC_REMAIN,TEXT(""));
	EnableWindow(hCtl.hTRB_PROGRESS,FALSE);
	SetWindowText(hdlg,TEXT(""));
	SetDlgItemText(hdlg,IDC_BTN_PLAY_PAUSE,TEXT("����"));
	KillTimer(hdlg,IDC_TimerID);
}


// ����������ʾ�ڱ������� 
void SetDialogText(HWND hdlg,int nTextIndex,LPHCONTROLS lphctl,PNOTIFYICONDATA pAppNodifyIcon)
{
	TCHAR szCurrentText[MAX_PATH];
		
 	SendMessage(lphctl->hLST_PLAYLIST,LB_GETTEXT,
			(WPARAM)nTextIndex,(LPARAM)szCurrentText);
 	SetWindowText(hdlg,szCurrentText); // ����ǰ���ŵĸ�������ʾ�ڱ������� 

 	StringCchCopy(pAppNodifyIcon->szTip,128,szCurrentText);
 	pAppNodifyIcon->uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;
 	Shell_NotifyIcon(NIM_MODIFY,pAppNodifyIcon);
}

void PlaySpecifySong(HWND hDlg,int index,LPFILESINFO lpfi,LPHCONTROLS lphctl)
{
		HRESULT hr;
		int nSongLen;
		SetDialogText(hDlg,g_currentPlayIndex,&hCtl,&AppNodifyIcon);
	 	Stop_Music(); // ֹ֮ͣǰ�Ĳ��� 
	 	
		hr=SpecifyMusicFile(lpfi->szFilePath[index]); // ָ��Ҫ���ŵ��ļ� 
		if(hr!=S_OK)
		{					
			// ���ļ������ڣ��ļ��޷�ʶ��ȣ�����ʧ�� 
			return;
		}
		SetTimer(hDlg,(UINT_PTR)IDC_TimerID,(UINT)1000,(TIMERPROC)TimerProc); // ������ʱ��
		Play_Music(); // ��ʼ����  
		EnableWindow(hCtl.hTRB_PROGRESS,TRUE);
		
		SendMessage(hCtl.hLST_PLAYLIST,LB_SETCURSEL,(WPARAM)index,0); // ָ����ǰ���ŵ��� ;
		
		SetDlgItemText(hDlg,IDC_BTN_PLAY_PAUSE,TEXT("��ͣ")); // �� IDC_BTN_PLAY_PAUSE��ť�ı�����Ϊ��ͣ 
		
		Get_Length(&nSongLen); // ��ȡ�������ȣ�Ȼ�����ò��Ź������ĳ��� 
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
