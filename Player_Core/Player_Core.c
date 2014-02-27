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

//����DirectShow�Ĺ��ܶ���COM����ʽ�ṩ���������Ǳ���õ����ǵĽӿ� 
IGraphBuilder *global_pGraph;//ͼ��������ӿ� 
IMediaControl *global_pControl;   //�˽ӿ�֧����ͣ�����ţ�ֹͣ�Ȳ��� 
IMediaEventEx *global_pEvent;  //�¼��ӿ� 
IMediaSeeking *global_pseeking;  //�˽ӿ�֧�ָı䲥��λ�ã���ȡ�������ȵȲ��� 
IBasicAudio *global_pbasic; //�˽ӿ�֧�ָı��������ı䲥�����ʵȲ��� 
BOOL bIsInitiateSucceeded; //�жϳ�ʼ���Ƿ��Ѿ��ɹ� 

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
	
	//�ж��Ƿ��Ѿ���ʼ������Ϊֻ���ʼ��һ�� 
	if(!bIsInitiateSucceeded)
	{
		/*��ʼ��COM��*/ 
		hr=CoInitialize(NULL);
		bIsInitiateSucceeded=SUCCEEDED(hr);
		
		/*��ȡIGraphBuilder�ӿ�(��ͼ�������)�����CoCreateInstance�ɹ��򷵻� S_OK */
		hr=CoCreateInstance((REFCLSID)&CLSID_FilterGraph,(LPUNKNOWN)NULL,
							(DWORD)CLSCTX_INPROC_SERVER,
							(REFIID)&IID_IGraphBuilder,(LPVOID *)&global_pGraph);
		/*#define S_OK  ((HRESULT)0x00000000L)*/
		bIsInitiateSucceeded=bIsInitiateSucceeded&(S_OK==hr);
		
		/*��ȡIMediaControl�ӿڣ����QueryInterface�ɹ��򷵻� S_OK */
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
	//�����ǲ���ʹ��ʱ�����ͷ����е�COM���� 
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
	//���ǲ�����ҪCOM�⣬���Խ����ͷ� 
	CoUninitialize();
	return S_OK;
}

EXPORT HRESULT Get_Length_StringA(LPSTR szLength,int sizeof_szLength)
{
	HRESULT hr;
	LONGLONG lduration;
	
	/*���û�г�ʼ�����߳�ʼ��ʧ�ܣ��򷵻�*/
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
	
	/*���û�г�ʼ�����߳�ʼ��ʧ�ܣ��򷵻�*/
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
	/*���û�г�ʼ�����߳�ʼ��ʧ�ܣ��򷵻�*/
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
	//����ֹͣ�� 
	global_pControl->lpVtbl->Stop(global_pControl);
	//Ȼ��ö�ٲ��ر�Filter Graph Manager�е�source filter��transform filter��renderer filter��
	//����Ϊ�˱�ֻ֤��һ����������·������������ָ��������һ���ļ�ʱ������������
	//ͬʱ����ʱ�����������׸�ͬ���� 
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
		//���뽫·��ת��Ϊunicode���룬COMֻ����unicode�ַ��� 
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
	//���Ƚ���������newPositionת��Ϊ LONGLONG���͵� newi64position�Ի�ȡ�㹻�ĳ��� 
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
			//���� 
			case State_Running:
			{
				long evCode, param1, param2;
				*pFilterState=State_Running;
				//�ж��ļ��Ƿ��Ѿ�������ϣ���Ϊ������Ϻ�Filter Graph(ͼ�������)������ 
				//������״̬��Ϊ State_Stopped,����������Ҫ�ж� Filter Graph��Ϣ���е���Ϣ��
				//�鿴�Ƿ��� EC_COMPLETE(ý���ļ��Ѿ��ط����)��Ϣ,�����У������Stop���� 
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
			//��ͣ 
			case State_Paused:
			{
				*pFilterState=State_Paused;
				break;
			}
			case State_Stopped:
			{
				//�û���ֹ�ļ��ط� 
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
