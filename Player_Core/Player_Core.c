#include <windows.h>
#include <windowsx.h>
#include <dshow.h>
#include <initguid.h>
#include <qnetwork.h>
#include <oleauto.h>
#include <strsafe.h>
#include <math.h>
#include "Common.h"
#include "Player_Core.h"

HRESULT Initiate_Player_Core();
HRESULT UnInitiate_Player_Core();

//所有DirectShow的功能都已COM的形式提供，所以我们必须得到它们的接口 
IGraphBuilder *global_pGraph;//图表管理器接口 
IMediaControl *global_pControl;   //此接口支持暂停，播放，停止等操作 
IMediaEventEx *global_pEvent;  //事件接口 
IMediaSeeking *global_pseeking;  //此接口支持改变播放位置，获取歌曲长度等操作 
IBasicAudio *global_pbasic; //此接口支持改变音量，改变播放速率等操作 
BOOL bIsInitiateSucceeded; //判断初始化是否已经成功 

BOOL APIENTRY DllMain (HINSTANCE hInst     /* Library instance handle. */ ,
                       DWORD reason        /* Reason this function is being called. */ ,
                       LPVOID reserved     /* Not used. */ )
{
	switch (reason)
	{
		case DLL_PROCESS_ATTACH:
		break;

		case DLL_PROCESS_DETACH:
		break;

		case DLL_THREAD_ATTACH:
		break;

		case DLL_THREAD_DETACH:
		break;
	}

	/* Returns TRUE on success, FALSE on failure */
	return TRUE;
}

HRESULT Initiate_Player_Core()
{
	HRESULT hr=E_FAIL;
	//IGraphBuilder *pGraph;
	//IMediaControl *pControl;
	//IMediaEventEx *pEvent; 
	//IMediaSeeking *pseeking;
	//IBasicAudio *pbasic;
	
	//判断是否已经初始化，因为只需初始化一次 
	if(!bIsInitiateSucceeded)
	{
		/*初始化COM库*/ 
		hr=CoInitialize(NULL);
		bIsInitiateSucceeded=SUCCEEDED(hr);
		
		/*获取IGraphBuilder接口(即图表管理器)，如果CoCreateInstance成功则返回 S_OK */
		hr=CoCreateInstance((REFCLSID)&CLSID_FilterGraph,(LPUNKNOWN)NULL,
							(DWORD)CLSCTX_INPROC_SERVER,
							(REFIID)&IID_IGraphBuilder,(LPVOID *)&global_pGraph);
		/*#define S_OK  ((HRESULT)0x00000000L)*/
		bIsInitiateSucceeded=bIsInitiateSucceeded&(S_OK==hr);
		
		/*获取IMediaControl接口，如果QueryInterface成功则返回 S_OK */
		hr=global_pGraph->lpVtbl->QueryInterface(global_pGraph,&IID_IMediaControl,(void**)&global_pControl);
		bIsInitiateSucceeded=bIsInitiateSucceeded&(S_OK==hr);
		
		hr=global_pGraph->lpVtbl->QueryInterface(global_pGraph,&IID_IMediaEventEx,(void**)&global_pEvent);
		bIsInitiateSucceeded=bIsInitiateSucceeded&(S_OK==hr);
		
		hr=global_pGraph->lpVtbl->QueryInterface(global_pGraph,&IID_IMediaSeeking,(void**)&global_pseeking);
		bIsInitiateSucceeded=bIsInitiateSucceeded&(S_OK==hr);
		
		hr=global_pGraph->lpVtbl->QueryInterface(global_pGraph,&IID_IBasicAudio,(void**)&global_pbasic);
		bIsInitiateSucceeded=bIsInitiateSucceeded&(S_OK==hr);
		
		if(bIsInitiateSucceeded)
		{
			hr=S_OK;
		}
		else
		{
			hr=E_FAIL;
		}
	}
	return hr;
}


HRESULT UnInitiate_Player_Core()
{
	//当我们不再使用时必须释放所有的COM对象 
	//if global_pbasic!=NULL
	if(global_pbasic)
	{
		global_pbasic->lpVtbl->Release(global_pbasic);
	}
	
	if(global_pEvent)
	{
		global_pEvent->lpVtbl->Release(global_pEvent);
	}
	
	if(global_pControl)
	{
		global_pControl->lpVtbl->Release(global_pControl);
	}
	
	if(global_pseeking)
	{
		global_pseeking->lpVtbl->Release(global_pseeking);
	}
	
	if(global_pGraph)
	{
		global_pGraph->lpVtbl->Release(global_pGraph);
	}
	
	bIsInitiateSucceeded=FALSE;
	//我们不再需要COM库，所以将它释放 
	CoUninitialize();
	return S_OK;
}

EXPORT HRESULT Get_Length_StringA(LPSTR szLength,int sizeof_szLength)
{
	HRESULT hr;
	LONGLONG lduration;
	
	/*如果没有初始化或者初始化失败，则返回*/
	if(!bIsInitiateSucceeded)
	{
		return E_FAIL;
	}
	
	global_pseeking->lpVtbl->GetDuration(global_pseeking,&lduration);
	
	hr=ConvertTimeToStringA(lduration,szLength,sizeof_szLength);
	
	return hr;
}


EXPORT HRESULT Get_Length_StringW(LPWSTR lpWszLength,int sizeof_WszLength_in_Characters)
{
	HRESULT hr;
	LONGLONG lduration;
	
	/*如果没有初始化或者初始化失败，则返回*/
	if(!bIsInitiateSucceeded)
	{
		return E_FAIL;
	}
	
	global_pseeking->lpVtbl->GetDuration(global_pseeking,&lduration);
	hr=ConvertTimeToStringW(lduration,lpWszLength,sizeof_WszLength_in_Characters);
	return hr;
}


EXPORT HRESULT Get_Length(int *lLength)
{
	LONGLONG lsonglength;
	HRESULT hr=E_FAIL;
	/*如果没有初始化或者初始化失败，则返回*/
	if(!bIsInitiateSucceeded)
	{
		return E_FAIL;
	}
	
	hr=global_pseeking->lpVtbl->GetDuration(global_pseeking,&lsonglength);
	lsonglength=lsonglength/10000000;
	*lLength=(int)lsonglength;
	return hr;
}

EXPORT HRESULT Play_Music()
{
	HRESULT hr=E_FAIL;
	if(!bIsInitiateSucceeded)
	{
		return E_FAIL;
	}
	
	hr=global_pControl->lpVtbl->Run(global_pControl);
	return hr;
}

EXPORT HRESULT Pause_Music()
{
	HRESULT hr=E_FAIL;
	if(!bIsInitiateSucceeded)
	{
		return E_FAIL;
	}
	
	hr=global_pControl->lpVtbl->Pause(global_pControl);
	return hr;
}

EXPORT HRESULT Stop_Music()
{
	HRESULT hr=E_FAIL;
	IEnumFilters *pEnum=NULL;
	
	if(!bIsInitiateSucceeded)
	{
		return E_FAIL;
	}
	//首先停止流 
	global_pControl->lpVtbl->Stop(global_pControl);
	//然后枚举并关闭Filter Graph Manager中的source filter，transform filter，renderer filter。
	//这是为了保证只有一条数据流链路，否则当我们再指定播放另一个文件时，两条数据链
	//同时工作时，将导致两首歌同播放 
	hr=UnInitiate_Player_Core();
	return hr;
}

EXPORT HRESULT SpecifyMusicFileA(LPCSTR szMusicFile)
{
	HRESULT hr=E_FAIL;
	int cbMultiByte;
	WCHAR *WlpMusicFile;
	Initiate_Player_Core();
	if(!bIsInitiateSucceeded)
	{
		return E_FAIL;
	}
	
	cbMultiByte=MultiByteToWideChar(CP_ACP,0,szMusicFile,-1,NULL,0); 
	WlpMusicFile=(WCHAR*)calloc(cbMultiByte,sizeof(WCHAR));
	if(WlpMusicFile)
	{
		//必须将路径转换为unicode编码，COM只处理unicode字符串 
		MultiByteToWideChar(CP_ACP,0, szMusicFile, -1, WlpMusicFile, cbMultiByte); 
		
		hr=global_pGraph->lpVtbl->RenderFile(global_pGraph,WlpMusicFile,NULL);
		free(WlpMusicFile);
	}
	return hr;
}


EXPORT HRESULT SpecifyMusicFileW(LPCWSTR WszMusicFile)
{
	HRESULT hr=E_FAIL;
	Initiate_Player_Core();
	if(!bIsInitiateSucceeded)
	{
		return E_FAIL;
	}
	hr=global_pGraph->lpVtbl->RenderFile(global_pGraph,WszMusicFile,NULL);
	return hr;
}

EXPORT HRESULT SetMusicRate(double dRate)
{
	HRESULT hr=E_FAIL;
	if(!bIsInitiateSucceeded)
	{
		return E_FAIL;
	}
	hr=global_pseeking->lpVtbl->SetRate(global_pseeking,dRate);
	return hr;
}

EXPORT HRESULT SetPlayerBalance(long lBalance)
{
	HRESULT hr=E_FAIL;
	if(!bIsInitiateSucceeded)
	{
		return E_FAIL;
	}
	hr=global_pbasic->lpVtbl->put_Balance(global_pbasic,lBalance);
	return hr;
}

EXPORT HRESULT SetPlayerVolume(long lVolume)
{
	HRESULT hr=E_FAIL;
	if(!bIsInitiateSucceeded)
	{
		return E_FAIL;
	}
	hr=global_pbasic->lpVtbl->put_Volume(global_pbasic,lVolume);
	return hr;
}

EXPORT HRESULT GetPlayerVolume(long *plVolume)
{
	HRESULT hr=E_FAIL;
	if(!bIsInitiateSucceeded)
	{
		return E_FAIL;
	}
	hr=global_pbasic->lpVtbl->get_Volume(global_pbasic,plVolume);
	return hr;
}


EXPORT HRESULT GetPlayerCurrentPosition(int *pCurrentPosition)
{
	HRESULT hr=E_FAIL;
	LONGLONG ltemp;
	if(!bIsInitiateSucceeded)
	{
		return E_FAIL;
	}
	hr=global_pseeking->lpVtbl->GetCurrentPosition(global_pseeking,&ltemp);
	ltemp=ltemp/10000000;
	*pCurrentPosition=(int)ltemp;
	return hr;
}

EXPORT HRESULT SetPlayerPosition(int newPosition)
{
	HRESULT hr=E_FAIL;
	LONGLONG lstopposition;
	LONGLONG newi64position;
	//TCHAR sztemp[255];
	if(!bIsInitiateSucceeded)
	{
		return E_FAIL;
	}
	//首先将传进来的newPosition转换为 LONGLONG类型的 newi64position以获取足够的长度 
	newi64position=newPosition;
	newi64position=newi64position*10000000;

	hr=global_pseeking->lpVtbl->GetDuration(global_pseeking,&lstopposition);
	
	if(SUCCEEDED(hr))
	{
		hr=global_pseeking->lpVtbl->SetPositions(global_pseeking,&newi64position,
			AM_SEEKING_AbsolutePositioning,&lstopposition,AM_SEEKING_AbsolutePositioning);
	}
	return hr;
}


EXPORT HRESULT GetCurrentState(OAFilterState *pFilterState)
{
	HRESULT hr=E_FAIL;
	OAFilterState fs;

	if(!bIsInitiateSucceeded)
	{
		return E_FAIL;
	}
	hr=global_pControl->lpVtbl->GetState(global_pControl,0,&fs);
	if(S_OK==hr)
	{
		switch(fs)
		{
			//运行 
			case State_Running:
			{
				long evCode, param1, param2;
				*pFilterState=State_Running;
				//判断文件是否已经播放完毕，因为播放完毕后Filter Graph(图标管理器)并不会 
				//把它的状态变为 State_Stopped,所以我们需要判断 Filter Graph消息泵中的消息，
				//查看是否有 EC_COMPLETE(媒体文件已经回放完毕)消息,假如有，则调用Stop函数 
				while(hr=global_pEvent->lpVtbl->GetEvent(global_pEvent,&evCode,
				&param1,&param2,0),SUCCEEDED(hr))
				{
					global_pEvent->lpVtbl->FreeEventParams(global_pEvent,evCode,param1,param2);
					if(EC_COMPLETE==evCode)
					{
						global_pControl->lpVtbl->Stop(global_pControl);
						*pFilterState=State_Stopped;
					}
				}
				break;
			}
			//暂停 
			case State_Paused:
			{
				*pFilterState=State_Paused;
				break;
			}
			case State_Stopped:
			{
				//用户终止文件回放 
				*pFilterState=State_Stopped;
				break;
			}
		}
		
	}
	return hr;
}


EXPORT HRESULT GetCurrentAndRemainA(LPSTR szCurrent,int sizeof_szCurrent_in_characters,
						LPSTR szRemain,int sizeof_szRemain_in_characters)
{
	HRESULT hr;
	LONGLONG lcurrent=0,lstop=0;
	hr=global_pseeking->lpVtbl->GetPositions(global_pseeking,&lcurrent,&lstop);
	if(SUCCEEDED(hr))
	{
		hr=ConvertTimeToStringA(lcurrent,szCurrent,sizeof_szCurrent_in_characters);
	}
	if(SUCCEEDED(hr))
	{
		hr=ConvertRemainTimeToStringA(lstop-lcurrent,szRemain,sizeof_szRemain_in_characters);
	}
	return hr;
}

EXPORT HRESULT GetCurrentAndRemainW(LPWSTR szWCurrent,int sizeof_szWCurrent_in_characters,
						LPWSTR szWRemain,int sizeof_szWRemain_in_characters)
{
	HRESULT hr;
	LONGLONG lcurrent=0,lstop=0;
	hr=global_pseeking->lpVtbl->GetPositions(global_pseeking,&lcurrent,&lstop);
	
	ConvertTimeToStringW(lcurrent,szWCurrent,sizeof_szWCurrent_in_characters);
	ConvertRemainTimeToStringW(lstop-lcurrent,szWRemain,sizeof_szWRemain_in_characters);
	
	return hr;
}
