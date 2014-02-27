#define _WIN32_IE 0x0600
#define UNICODE
#define _UNICODE // ÿ���ļ���ǰ�涼Ҫ���� 
#include <windows.h>
#include <commctrl.h>
#include <Shlwapi.h>
#include <strsafe.h>
#include <Shlobj.h>
#include <Shellapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "main.h"
#include "rsrc.inc"



void SearchFilesCondition(LPFILESINFO lpfilesinfo,HWND hdlg,
					LPHCONTROLS lphctls,LPTSTR  lpSearchPath,LPTSTR lpFileExtension);
BOOL CompareExtension(LPTSTR lpExt);

////////////////////////////////////��ȡ�ؼ���� ////////////////////////////////
void GetControlsHandle(LPHCONTROLS phCtl,HWND hDlg)
{
	phCtl->hBTN_ABOVE=GetDlgItem(hDlg,IDC_BTN_ABOVE);
	phCtl->hBTN_NEXT=GetDlgItem(hDlg,IDC_BTN_NEXT);
	phCtl->hBTN_PLAY_PAUSE=GetDlgItem(hDlg,IDC_BTN_PLAY_PAUSE);
	phCtl->hBTN_STOP=GetDlgItem(hDlg,IDC_BTN_STOP);
	phCtl->hCHK_MUTE=GetDlgItem(hDlg,IDC_CHK_MUTE);
	phCtl->hLST_PLAYLIST=GetDlgItem(hDlg,IDC_LST_PLAYLIST);
	phCtl->hSTC_CURRENT=GetDlgItem(hDlg,IDC_STC_CURRENT);
	phCtl->hSTC_REMAIN=GetDlgItem(hDlg,IDC_STC_REMAIN);
	phCtl->hTRB_PROGRESS=GetDlgItem(hDlg,IDC_TRB_PROGRESS);
	phCtl->hTRB_SETBALANCE=GetDlgItem(hDlg,IDC_TRB_SETBALANCE);
	phCtl->hTRB_SETVOLUME=GetDlgItem(hDlg,IDC_TRB_SETVOLUME);
	phCtl->hIDC_UDN1=GetDlgItem(hDlg,IDC_UDN1);
}




//////////////////////////////������ؿؼ�////////////////////////////
void InitiateControls(LPHCONTROLS lphCtl)
{
	//��������ƽ��ؼ� 
	SendMessage(lphCtl->hTRB_SETBALANCE,TBM_SETRANGE,(WPARAM)TRUE,MAKELONG(0,200));
	//SendMessage(lphCtl->hTRB_SETBALANCE,TBM_SETLINESIZE,0,(long)10);
	//��������ƽ��ؼ�Ĭ��λ��
	SendMessage(lphCtl->hTRB_SETBALANCE,TBM_SETPOS,(WPARAM)TRUE,100); 
	SendMessage(lphCtl->hTRB_SETBALANCE, TBM_SETTICFREQ, 10, 0);
 	SendMessage(lphCtl->hTRB_SETBALANCE,TBM_SETLINESIZE,0,(LPARAM)10);
	SendMessage(lphCtl->hTRB_SETBALANCE,TBM_SETPAGESIZE,0,(LPARAM)10);
	
	//���ò����б��и߶�
	SendMessage(lphCtl->hLST_PLAYLIST,LB_SETITEMHEIGHT,0,17);
	//SendMessage(lphCtl->hLST_PLAYLIST,LB_SETHORIZONTALEXTENT,5,0);
	
	//���������ؼ�
	 SendMessage(lphCtl->hTRB_SETVOLUME,TBM_SETRANGE,(WPARAM)TRUE,MAKELONG(0,1000));
	 SendMessage(lphCtl->hTRB_SETVOLUME,TBM_SETLINESIZE,0,(LPARAM)50);
	 SendMessage(lphCtl->hTRB_SETVOLUME,TBM_SETPAGESIZE,0,(LPARAM)50);
	 SendMessage(lphCtl->hTRB_SETVOLUME, TBM_SETTICFREQ, 50, 0);
} 


////////////////////////////////////��������ļ�///////////////////////////////////
void AddFiles(LPFILESINFO lpfilesinfo,HWND hdlg,LPHCONTROLS lphctls) 
{
	TCHAR szfiledata[256*MAX_PATH];
	TCHAR filterstr[]=__TEXT("�����ļ�(mp3��wma��wav��APE��FLA\
			C��MIDI��OGG��AAC)\0*.mp3;*.wma;*.wav;*.APE;*.FLAC;*.mid;*.OGG;*.m4a\0\
			�����ļ�(*.*)\0*.*\0\
			mp3��wma\0*.mp3;*.wma\0\
			APE��FLAC��MIDI��OGG��AAC\0*.ape;*.flac;*.mid;*.ogg;*.m4a\0\0");
	OPENFILENAME ofn;
	ZeroMemory(&ofn,sizeof(ofn));
	ofn.lStructSize=sizeof(OPENFILENAME);
	ofn.hwndOwner=hdlg;
	ofn.lpstrFilter=filterstr;
	ofn.lpstrCustomFilter=NULL;
	ofn.nFilterIndex=1;
	ofn.lpstrFile=szfiledata;
	ofn.nMaxFile=sizeof(szfiledata)/sizeof(TCHAR);
	ofn.lpstrFile[0]=TEXT('\0');
	ofn.lpstrFileTitle=NULL;
	ofn.nMaxFileTitle=0;
	ofn.lpstrTitle=NULL;
	ofn.Flags=OFN_HIDEREADONLY|OFN_FILEMUSTEXIST
                        |OFN_PATHMUSTEXIST|OFN_EXPLORER|OFN_ALLOWMULTISELECT;
 	ofn.lpstrDefExt=__TEXT("*.mp3");
 	if(GetOpenFileName(&ofn))
 	{
		TCHAR szfilespath[MAX_PATH]={0};
		LPTSTR p=NULL;
		int nlen=0,nAddFilesCount=0;
		
		//�õ���ѡ�ļ���·�� 
		StringCchCopyN(szfilespath,sizeof(szfilespath)/sizeof(TCHAR),szfiledata,ofn.nFileOffset);
		//��·���ַ�����ӽ����� 
		szfilespath[ofn.nFileOffset]=TEXT('\0');
		
		//��\��Ӷ�·��ĩβ 
		StringCchCat(szfilespath,sizeof(szfilespath)/sizeof(TCHAR),__TEXT("\\"));
		
		//ָ��ָ��(szfiledata��ַ+ofn.nFileOffset��һ���ļ���ƫ����) 
		p=szfiledata+ofn.nFileOffset;

		while(*p)
		{
			//��·�������� lpfilesinfo->szFilePath
			StringCchCopy(lpfilesinfo->szFilePath[lpfilesinfo->iCountOfFiles],
					sizeof(lpfilesinfo->szFilePath)/sizeof(TCHAR),szfilespath);
			
			//���ļ���������lpfilesinfo->szFilePath(·��+�ļ���)		
			StringCchCat(lpfilesinfo->szFilePath[lpfilesinfo->iCountOfFiles],
					sizeof(lpfilesinfo->szFilePath)/sizeof(TCHAR),p);
					
			StringCchLength(p,MAX_PATH,&nlen);
			p +=nlen+1;
			lpfilesinfo->iCountOfFiles++;
			nAddFilesCount++;
		}
		
		AddFilesNameToPlayList(lpfilesinfo,lphctls,hdlg,nAddFilesCount);
		
 	}

	
} 

/////////////////////////////�򲥷��б���Ӹ����� ////////////////////////////
void AddFilesNameToPlayList(LPFILESINFO lpfilesinfo,LPHCONTROLS lphctls,HWND hdlg,int nfiles)
{
	int loop=lpfilesinfo->iCountOfFiles-nfiles;
	for(;loop<lpfilesinfo->iCountOfFiles&&lpfilesinfo->iCountOfFiles<=256;loop++)
	{
		TCHAR szAddMark[MAX_PATH];
		TCHAR szTempstring[MAX_PATH];
		TCHAR szTempstring2[MAX_PATH];
		 
		StringCchCopy(szAddMark,MAX_PATH,lpfilesinfo->szFilePath[loop]);
		//ȥ��������չ�� 
		PathRemoveExtension(szAddMark);
		//��������ӱ�� 
		StringCchPrintf(szTempstring,sizeof(szTempstring)/sizeof(TCHAR),
					__TEXT("%d. %s"),loop+1,PathFindFileName(szAddMark));
		
		StringCchCopy(szTempstring2,MAX_PATH,szTempstring);
		//�����ز����б�ʱ���ж�ָ�����ļ��Ƿ����
		if(!PathFileExists(lpfilesinfo->szFilePath[loop]))
		{
			StringCchPrintf(szTempstring2,sizeof(szTempstring2)/sizeof(TCHAR),
					__TEXT("��Ч %s"),szTempstring);
		} 
		SendMessage(lphctls->hLST_PLAYLIST,LB_ADDSTRING,0,(LPARAM)szTempstring2);
	}
	
}

//////////////////////////����ļ��� /////////////////////////
void AddFolder(LPFILESINFO lpfilesinfo,HWND hdlg,LPHCONTROLS lphctls)
{
	BROWSEINFO sBrowseInfo;
	LPCITEMIDLIST lpitem=NULL;
	ZeroMemory(&sBrowseInfo,sizeof(sBrowseInfo));
	sBrowseInfo.hwndOwner=hdlg;
	sBrowseInfo.lpszTitle=__TEXT("��ѡ��Ҫ��ӵ��ļ���");
	sBrowseInfo.ulFlags=BIF_RETURNONLYFSDIRS|BIF_BROWSEFORCOMPUTER;
	
	if((lpitem=SHBrowseForFolder(&sBrowseInfo))!=NULL)
	{
		TCHAR szBuffer_Directory[2*MAX_PATH]={0};
		
		SHGetPathFromIDList(lpitem,szBuffer_Directory);
		SearchFilesCondition(lpfilesinfo,hdlg,lphctls,
					szBuffer_Directory,__TEXT("mp3"));
					
		SearchFilesCondition(lpfilesinfo,hdlg,lphctls,
					szBuffer_Directory,__TEXT("wma"));
					
		SearchFilesCondition(lpfilesinfo,hdlg,lphctls,
					szBuffer_Directory,__TEXT("wav"));
					
		SearchFilesCondition(lpfilesinfo,hdlg,lphctls,
					szBuffer_Directory,__TEXT("APE"));
					
		SearchFilesCondition(lpfilesinfo,hdlg,lphctls,
					szBuffer_Directory,__TEXT("FLAC"));
					
		SearchFilesCondition(lpfilesinfo,hdlg,lphctls,
					szBuffer_Directory,__TEXT("mid"));
					
		SearchFilesCondition(lpfilesinfo,hdlg,lphctls,
					szBuffer_Directory,__TEXT("m4a"));
					
		SearchFilesCondition(lpfilesinfo,hdlg,lphctls,
					szBuffer_Directory,__TEXT("ogg"));
	}
	
	
}

///////////////////////////////���ݸ����ĺ�׺�������ļ�//////////////////////////// 
void SearchFilesCondition(LPFILESINFO lpfilesinfo,HWND hdlg,
					LPHCONTROLS lphctls,LPTSTR  lpSearchPath,LPTSTR lpFileExtension)
{
	TCHAR szTempPath[2*MAX_PATH]={0};
	HANDLE hFindFile;
	WIN32_FIND_DATA sfd;
	StringCchCat(szTempPath,sizeof(szTempPath)/sizeof(TCHAR),lpSearchPath);
	StringCchCat(szTempPath,sizeof(szTempPath)/sizeof(TCHAR),__TEXT("\\*."));
	StringCchCat(szTempPath,sizeof(szTempPath)/sizeof(TCHAR),lpFileExtension);
	if((hFindFile=FindFirstFile(szTempPath,&sfd))!=INVALID_HANDLE_VALUE)
	{
		int nCountOfFiles=0;
		
		do
		{
			if(sfd.cFileName[0]==__TEXT('.'))
			continue;
			
			StringCchCopy(lpfilesinfo->szFilePath[lpfilesinfo->iCountOfFiles],
						MAX_PATH,lpSearchPath);
			StringCchCat(lpfilesinfo->szFilePath[lpfilesinfo->iCountOfFiles],
					MAX_PATH,__TEXT("\\"));
			StringCchCat(lpfilesinfo->szFilePath[lpfilesinfo->iCountOfFiles],
					MAX_PATH,sfd.cFileName);		
			nCountOfFiles++;
			lpfilesinfo->iCountOfFiles++;
		
		}while(FindNextFile(hFindFile,&sfd));
		
		AddFilesNameToPlayList(lpfilesinfo,lphctls,hdlg,nCountOfFiles);
	}
	
		
}


///////////////��ȡ���������������ŵ�ʱ����/////////////////////// 
void GetrandomNumbers(LPRANDOMNUMBERS lprn,LPFILESINFO lpfi)
{
	int i,ncheck=0;
	srand(time(NULL)); // ����������� 
	if((lpfi->iCountOfFiles)==0) // ��������б�û�и����Ͳ������������ 
	{
		return;
	}
	lprn->arrayNums[0]=rand()%(lpfi->iCountOfFiles); // ���Ƚ�һ��������ŵ� arrayNums[0]��������ķ�Χ��0~(lpfi->iCountOfFiles-1) 
	lprn->nNumsCount=1;
	
	for(i=1; i<(lpfi->iCountOfFiles); i++)
	{
		
		int randnum=rand()%(lpfi->iCountOfFiles);  // ���������
		for(ncheck=0;ncheck<lprn->nNumsCount;ncheck++)  // �Ƚ��Ѿ���ŵ�ÿһ������� 
		{
			if(randnum==lprn->arrayNums[ncheck]) // ���������������Ѿ����ڣ��������µ������ 
			{
				randnum=rand()%(lpfi->iCountOfFiles);
				ncheck=-1; // ���¼����Ƚ� 
			}
		}
		
		lprn->arrayNums[i]=randnum;
		lprn->nNumsCount++;
		
	}
	lprn->nNumsCount=0;  // ���õ�ǰ������ŵ�����Ϊ0 
}

//////////////////////////��ȡ��һ�׵����� 
int GetNextPlayIndex(UINT nPlayMode,int nCurrentPlayIndex,
			LPRANDOMNUMBERS lprn,LPFILESINFO lpfi)
{
	switch(nPlayMode)
	{
		case IDM_Random: //������� 
		{
			if( (lprn->nNumsCount) < (lpfi->iCountOfFiles) )
			{
				int nNextIndex=lprn->arrayNums[lprn->nNumsCount];
				(lprn->nNumsCount)++; // ��������һ���������� 
				return nNextIndex;
			}
			else
			{
				return -1;
			}
		}
		
		case IDM_Sortorder: // ˳�򲥷� 
		{
			nCurrentPlayIndex++;
			if(nCurrentPlayIndex<(lpfi->iCountOfFiles)) // �����ǰֵ������Ч����֮�ڣ��򷵻� 
			{
				return nCurrentPlayIndex;
			}
			else
			{
				return 0; // ���򷵻�-1 
			}
			
		}
		
		case IDM_SingleLoop:
		case IDM_Loop: // ѭ������ 
		{
			nCurrentPlayIndex++;
			return (nCurrentPlayIndex%(lpfi->iCountOfFiles));
		}
		
	}
	return -1; // �����ǵ������ţ��򷵻�-1 
}

//////////////////////////����һ�׵����� 
int GetAbovePlayIndex(UINT nPlayMode,int nCurrentPlayIndex,
			LPRANDOMNUMBERS lprn,LPFILESINFO lpfi)
{
	switch(nPlayMode)
	{
		case IDM_Random: //������� 
		{
			if( (lprn->nNumsCount) >=0 )
			{
				int nAboveIndex=lprn->arrayNums[lprn->nNumsCount];
				lprn->nNumsCount--;
				return nAboveIndex;
			}
			else
			{
				return -1;
			}
		}
		
		case IDM_Sortorder: // ˳�򲥷� 
		{
			nCurrentPlayIndex--;
			if(nCurrentPlayIndex>=0) // �����ǰֵ������Ч����֮�ڣ��򷵻� 
			{
				return nCurrentPlayIndex;
			}
			else
			{
				return lpfi->iCountOfFiles-1; // ���򷵻����һ�� 
			}
			
		}
		
		case IDM_SingleLoop:
		case IDM_Loop: // ѭ������ 
		{
			int nAboveIndex;
			if(nCurrentPlayIndex!=0)
			{
				nAboveIndex=--nCurrentPlayIndex;
			}
			else
			{
				nAboveIndex=(lpfi->iCountOfFiles)-1; // ��������ʱ��һ�ף���ô��һ���������һ�� 
			}
			
			return nAboveIndex;
		}
	}
	return -1;  // �����ǵ������ţ��򷵻�-1 
}

void EnableControls(HMENU hMenu_PlayList,LPHCONTROLS lpctls)
{
	EnableMenuItem(hMenu_PlayList,IDM_PlayCurrentSelect,MF_ENABLED);
	EnableMenuItem(hMenu_PlayList,IDM_DELETE,MF_ENABLED);
	EnableMenuItem(hMenu_PlayList,IDM_CLEAR,MF_ENABLED);
	EnableWindow(lpctls->hBTN_STOP,TRUE);
	EnableWindow(lpctls->hBTN_ABOVE,TRUE);
	EnableWindow(lpctls->hBTN_PLAY_PAUSE,TRUE);
	EnableWindow(lpctls->hBTN_NEXT,TRUE);
	EnableWindow(lpctls->hTRB_PROGRESS,TRUE);
	EnableWindow(lpctls->hIDC_UDN1,TRUE);
}

void DisableControls(HMENU hMenu_PlayList,LPHCONTROLS lpctls)
{
	EnableMenuItem(hMenu_PlayList,IDM_PlayCurrentSelect,MF_DISABLED|MF_GRAYED);
	EnableMenuItem(hMenu_PlayList,IDM_DELETE,MF_DISABLED|MF_GRAYED);
	EnableMenuItem(hMenu_PlayList,IDM_CLEAR,MF_DISABLED|MF_GRAYED);
	EnableWindow(lpctls->hBTN_STOP,FALSE);
	EnableWindow(lpctls->hBTN_ABOVE,FALSE);
	EnableWindow(lpctls->hBTN_PLAY_PAUSE,FALSE);
	EnableWindow(lpctls->hBTN_NEXT,FALSE);
	EnableWindow(lpctls->hTRB_PROGRESS,FALSE);
	EnableWindow(lpctls->hIDC_UDN1,FALSE);
}

void RefillFilesInfo(LPFILESINFO lpfsi,int nNoNeedToCopy)
{
	FILESINFO sfi;
	ZeroMemory(&sfi,sizeof(sfi));
	sfi.iCountOfFiles=lpfsi->iCountOfFiles; // ��������� 
	if(0==nNoNeedToCopy) // ����һ�׼������Ƶ�ʱ�򽫻��õ� 
	{
		int nHaveLooped=1;
		
		//�ӵڶ��׿�ʼ���� 
		for(;nHaveLooped<(lpfsi->iCountOfFiles);nHaveLooped++)
		{
			//�ӵڶ��׿�ʼ���Ƶ����һ�� 
			StringCchCopy(sfi.szFilePath[nHaveLooped-1],MAX_PATH,lpfsi->szFilePath[nHaveLooped]);
		}
		//�ٽ���һ�׸��Ƶ�ĩβ 
		StringCchCopy(sfi.szFilePath[nHaveLooped-1],MAX_PATH,lpfsi->szFilePath[0]);
	}
	else // �������һ���Ƶ���һ�׵�λ��ʱ�õ� 
	{
		int nHaveLooped=0;
		// �Ƚ����һ�׸��Ƶ� sfi�ĵ�һ�׵�λ�� 
		StringCchCopy(sfi.szFilePath[0],MAX_PATH,lpfsi->szFilePath[lpfsi->iCountOfFiles-1]);
		// �ӵ�һ��һֱ���Ƶ������ڶ��� 
		for(;nHaveLooped<(lpfsi->iCountOfFiles-1);nHaveLooped++)
		{
			StringCchCopy(sfi.szFilePath[nHaveLooped+1],MAX_PATH,lpfsi->szFilePath[nHaveLooped]);
		}
	}
	
	CopyMemory(lpfsi,&sfi,sizeof(sfi));
}
//////////////////////////////���Ƹ���////////////////////////// 
void MoveUp(LPFILESINFO lpfsi,LPHCONTROLS lphcs,int *ncurrentplay,HWND hdlg)
{
	TCHAR szTemp[MAX_PATH];
	int nIndexDest;
	int nIndexSrc=SendMessage(lphcs->hLST_PLAYLIST,LB_GETCURSEL,0,0);
	if(nIndexSrc!=0) // �����ǰҪ�ƶ��ĸ������ǵ�һ�� 
	{
		nIndexDest=nIndexSrc-1;
		StringCchCopy(szTemp,MAX_PATH,
			lpfsi->szFilePath[nIndexDest]);
		StringCchCopy(lpfsi->szFilePath[nIndexDest],MAX_PATH,
			lpfsi->szFilePath[nIndexSrc]);
		StringCchCopy(lpfsi->szFilePath[nIndexSrc],MAX_PATH,
			szTemp);
	}
	else
	{
		RefillFilesInfo(lpfsi,0);
		nIndexDest=lpfsi->iCountOfFiles-1; // �ƶ���ĸ�����λ�� 
	}
	SendMessage(lphcs->hLST_PLAYLIST,LB_RESETCONTENT,0,0); // ��ղ����б� 
	AddFilesNameToPlayList(lpfsi,lphcs,hdlg,lpfsi->iCountOfFiles); // ˢ�²����б�
	SendMessage(lphcs->hLST_PLAYLIST,LB_SETCURSEL,nIndexDest,0); // ��ʾ�ƶ��ĸ���
	*ncurrentplay=nIndexDest; // ���´�ŵ�ǰ���ŵ����� 
}


//////////////////////////////���Ƹ���////////////////////////// 
void MoveDown(LPFILESINFO lpfsi,LPHCONTROLS lphcs,int *ncurrentplay,HWND hdlg)
{
	TCHAR szTemp[MAX_PATH];
	int nIndexDest;
	int nIndexSrc=SendMessage(lphcs->hLST_PLAYLIST,LB_GETCURSEL,0,0);
	if(nIndexSrc!=(lpfsi->iCountOfFiles-1)) // �����ǰҪ�ƶ��ĸ����������һ�� 
	{
		nIndexDest=nIndexSrc+1;
		StringCchCopy(szTemp,MAX_PATH,
			lpfsi->szFilePath[nIndexDest]);
		StringCchCopy(lpfsi->szFilePath[nIndexDest],MAX_PATH,
			lpfsi->szFilePath[nIndexSrc]);
		StringCchCopy(lpfsi->szFilePath[nIndexSrc],MAX_PATH,
			szTemp);
	}
	else
	{
		nIndexDest=0; // ָ���һ�� 
		RefillFilesInfo(lpfsi,lpfsi->iCountOfFiles-1);
	}
	SendMessage(lphcs->hLST_PLAYLIST,LB_RESETCONTENT,0,0); // ��ղ����б� 
 	AddFilesNameToPlayList(lpfsi,lphcs,hdlg,lpfsi->iCountOfFiles); // ˢ�²����б�
  	SendMessage(lphcs->hLST_PLAYLIST,LB_SETCURSEL,nIndexDest,0); // ��ʾ�ƶ��ĸ���
	 *ncurrentplay=nIndexDest; // ���´�ŵ�ǰ���ŵ����� 

}

void DeleteItem(LPFILESINFO lpfsi,int nBeDeleted,LPHCONTROLS lphctls,HWND hdlg)
{
	FILESINFO sfsiTemp;
	int nLoops=0,i=0; 
	
	ZeroMemory(&sfsiTemp,sizeof(sfsiTemp));
	for(nLoops=0;nLoops<(lpfsi->iCountOfFiles);nLoops++)
	{
		if(nLoops==nBeDeleted) 
		{
			continue;
		}
		StringCchCopy(sfsiTemp.szFilePath[sfsiTemp.iCountOfFiles],MAX_PATH,lpfsi->szFilePath[nLoops]);
		sfsiTemp.iCountOfFiles++;
	}
	CopyMemory(lpfsi,&sfsiTemp,sizeof(sfsiTemp));
	SendMessage(lphctls->hLST_PLAYLIST,LB_RESETCONTENT,0,0); // ��ղ����б� 
 	AddFilesNameToPlayList(lpfsi,lphctls,hdlg,lpfsi->iCountOfFiles); // ˢ�²����б�
}


BOOL LoadPlayList(LPFILESINFO lpfsi,HINSTANCE hinstance)
{
	BOOL bSuccess=FALSE;
	TCHAR szplaylistpath[MAX_PATH];
	HANDLE hfile;
	// ��ȡ��ִ���ļ���·�� 
	GetModuleFileName(hinstance,szplaylistpath,sizeof(szplaylistpath)/sizeof(TCHAR));
	PathRemoveFileSpec(szplaylistpath);
	StringCchCat(szplaylistpath,MAX_PATH,TEXT("\\PlayList.PLST"));
	hfile=CreateFile(szplaylistpath,GENERIC_READ,FILE_SHARE_READ,NULL,
					OPEN_EXISTING,0,NULL);
	if(INVALID_HANDLE_VALUE!=hfile)
	{
		DWORD dwFileSize;
		if(0<(dwFileSize=GetFileSize(hfile,NULL)))
		{
			LPTSTR lpfilepath=(LPTSTR)GlobalAlloc(GPTR,dwFileSize);
			if(NULL!=lpfilepath)
			{
				DWORD dwread;
				
				if(ReadFile(hfile,lpfilepath,dwFileSize,&dwread,NULL))
				{
					LPTSTR ptmp=lpfilepath;
					int icntfile=0,i=0;
					while(ptmp<=(lpfilepath+dwFileSize/sizeof(TCHAR)))
					{
						if(*ptmp==TEXT('\n'))
						{
							*ptmp=TEXT('\0');
							icntfile++;
						}
						ptmp++;
					}
					
					for(;i<icntfile;i++)
					{
						int ilen=0;
						StringCchCopy(lpfsi->szFilePath[i],MAX_PATH,lpfilepath);
						StringCchLength(lpfilepath,MAX_PATH,&ilen);
						lpfilepath+=ilen+1;
						lpfsi->iCountOfFiles++;
					}
					
					bSuccess=TRUE;
				}
			}
			GlobalFree(lpfilepath);
		}
	}
	CloseHandle(hfile);
	return bSuccess;
}		

void SavePlayList(LPFILESINFO lpfsi,HINSTANCE hinstance)
{
	int loop=0;
	LPTSTR pstring,palloc;	
	DWORD dwsize;
	DWORD dwtotalsizeinbytes=0;
	// �˴��������Ҫ������ֽ��� 
	for(loop=0;loop<lpfsi->iCountOfFiles;loop++)
	{
		StringCbLength(lpfsi->szFilePath[loop],MAX_PATH,&dwsize);
		dwtotalsizeinbytes+=dwsize+sizeof(TCHAR);
	}
		
	pstring=palloc=GlobalAlloc(GPTR,dwtotalsizeinbytes+sizeof(TCHAR));
	if(NULL!=palloc)
	{
		HANDLE hfile;
		DWORD dwwrite=0;
		TCHAR szFilePath[MAX_PATH];
				
		// ������·���ŵ�����Ŀؼ��� 
		for(loop=0;loop<lpfsi->iCountOfFiles;loop++)
		{
			int len;
			StringCchCopy(pstring,dwtotalsizeinbytes/sizeof(TCHAR),lpfsi->szFilePath[loop]);
			StringCchLength(lpfsi->szFilePath[loop],MAX_PATH,&len);
									
			pstring=pstring+len;
			*pstring=TEXT('\n');
			pstring++;
		}
		*pstring=TEXT('\0');
		 	
 		// ��ȡ��ִ���ļ���·�� 
		GetModuleFileName(hinstance,szFilePath,sizeof(szFilePath)/sizeof(TCHAR));
		PathRemoveFileSpec(szFilePath);
		StringCchCat(szFilePath,MAX_PATH,TEXT("\\PlayList.PLST"));
		// ��ִ���ļ���Ŀ¼�´��� PlayList.PLST
		hfile=CreateFile(szFilePath,GENERIC_WRITE,FILE_SHARE_READ ,NULL,
				CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		//MessageBox(NULL,TEXT("���ڴ����ļ�"),TEXT(""), MB_OK);						
		if(INVALID_HANDLE_VALUE!=hfile)
		{
			DWORD dwbytetowrite;
			StringCbLength(palloc,dwtotalsizeinbytes+sizeof(TCHAR),&dwbytetowrite);
			WriteFile(hfile,palloc,dwbytetowrite,&dwwrite,NULL);
			//MessageBox(NULL,TEXT("����ɹ�"),TEXT(""), MB_OK);
			CloseHandle(hfile);
		}
		
		GlobalFree(palloc);
	}
	
}

//////////////////���֪ͨ�����ͼ��//////////////////////// 
BOOL AddNodifyIcon(HWND hWnd,PNOTIFYICONDATA pAppNodifyIcon)
{


    pAppNodifyIcon->cbSize=sizeof(NOTIFYICONDATA);
    pAppNodifyIcon->hWnd=hWnd;
    pAppNodifyIcon->uID=IDC_NODIFYICON;
    pAppNodifyIcon->uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP|NIF_INFO;
    pAppNodifyIcon->uCallbackMessage=WM_COMMAND;
    pAppNodifyIcon->hIcon=(HICON)LoadIcon((HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE),MAKEINTRESOURCE(IDC_NODIFYICON));
	StringCchCopy(pAppNodifyIcon->szTip,128,TEXT("���ֲ�����V2.0"));
    StringCchCopy(pAppNodifyIcon->szInfo,256,TEXT("���ֲ�����V2.0"));
    StringCchCopy(pAppNodifyIcon->szInfoTitle,64,TEXT("������"));
    pAppNodifyIcon->dwInfoFlags=NIIF_INFO|NIIF_NOSOUND;
    pAppNodifyIcon->dwStateMask=NIS_HIDDEN;
    pAppNodifyIcon->dwState=NIS_HIDDEN;
    Shell_NotifyIcon(NIM_ADD,pAppNodifyIcon);
    return TRUE;

}

void SetPlayModeMenu(HMENU hmenu,UINT *pselected,UINT nMenuID)
{
	HMENU hSubMenu1=GetSubMenu(hmenu,0);
	CheckMenuRadioItem(hSubMenu1,IDM_Random,IDM_SingleLoop,nMenuID,MF_BYCOMMAND);
	*pselected=nMenuID;
}

void SetTransparentMune(HMENU hmenu,UINT *pselected,UINT nMenuID)
{
	HMENU hSubMenu1=GetSubMenu(hmenu,1);
	CheckMenuRadioItem(hSubMenu1,IDM_Opaque,IDM_P90,nMenuID,MF_BYCOMMAND);
	*pselected=nMenuID;
}

BOOL WINAPI Main_AboutProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    switch(uMsg)
    {
		case WM_COMMAND:
		{
			if(IDC_BTN==LOWORD(wParam))
			{
				EndDialog(hWnd,0);
			}
			break;
		}
    }
    return FALSE;
}

void AddDropFilesToList(HDROP hDropFiles,LPFILESINFO lpsfi,LPHCONTROLS lphCtls,HWND hdlg)
{
	TCHAR szFilename[MAX_PATH];
	int nFileCount=0,i=0;
	nFileCount=DragQueryFile(hDropFiles, (UINT)0xffffffff, NULL, 0);
	if(0<nFileCount)
	{
		for(;i<nFileCount;i++)
		{
			DragQueryFile(hDropFiles,i, szFilename, MAX_PATH);
			if(CompareExtension(PathFindExtension(szFilename)))
			{
				StringCchCopy(lpsfi->szFilePath[lpsfi->iCountOfFiles],MAX_PATH,szFilename);
				lpsfi->iCountOfFiles++;
			}
		}
	}
	
	DragFinish(hDropFiles);
	SendMessage(lphCtls->hLST_PLAYLIST,LB_RESETCONTENT,0,0);
	AddFilesNameToPlayList(lpsfi,lphCtls,hdlg,lpsfi->iCountOfFiles);
}

BOOL CompareExtension(LPTSTR lpExt)
{
	BOOL bequal=FALSE;
	bequal=bequal|(CSTR_EQUAL==CompareString(LOCALE_USER_DEFAULT,NORM_IGNORESYMBOLS|NORM_IGNORECASE,
				lpExt,-1,TEXT(".mid"),-1));
	bequal=bequal|(CSTR_EQUAL==CompareString(LOCALE_USER_DEFAULT,NORM_IGNORESYMBOLS|NORM_IGNORECASE,
				lpExt,-1,TEXT(".mp3"),-1));
	bequal=bequal|(CSTR_EQUAL==CompareString(LOCALE_USER_DEFAULT,NORM_IGNORESYMBOLS|NORM_IGNORECASE,
				lpExt,-1,TEXT(".wma"),-1));
	bequal=bequal|(CSTR_EQUAL==CompareString(LOCALE_USER_DEFAULT,NORM_IGNORESYMBOLS|NORM_IGNORECASE,
				lpExt,-1,TEXT(".wav"),-1));
	bequal=bequal|(CSTR_EQUAL==CompareString(LOCALE_USER_DEFAULT,NORM_IGNORESYMBOLS|NORM_IGNORECASE,
				lpExt,-1,TEXT(".OGG"),-1));
	bequal=bequal|(CSTR_EQUAL==CompareString(LOCALE_USER_DEFAULT,NORM_IGNORESYMBOLS|NORM_IGNORECASE,
				lpExt,-1,TEXT(".m4a"),-1));
	bequal=bequal|(CSTR_EQUAL==CompareString(LOCALE_USER_DEFAULT,NORM_IGNORESYMBOLS|NORM_IGNORECASE,
				lpExt,-1,TEXT(".flac"),-1));
	bequal=bequal|(CSTR_EQUAL==CompareString(LOCALE_USER_DEFAULT,NORM_IGNORESYMBOLS|NORM_IGNORECASE,
				lpExt,-1,TEXT(".ape"),-1));
	return bequal;
}
