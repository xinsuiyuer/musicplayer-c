#ifndef _DLLMAIN_H
#define _DLLMAIN_H
typedef long OAFilterState;


#ifdef      __cplusplus
        
#define     EXPORT extern "C" __declspec (dllexport)
        
#else
        
#define     EXPORT __declspec (dllexport)
        
#endif




/* ��ȡ���������ַ���(ANSC)��ֻ�Ƿ�����ʾ */
EXPORT HRESULT Get_Length_StringA(LPSTR lpszLength,int sizeof_szLength_in_characters);

/* ��ȡ���������ַ���(UniCode),�ڶ�������ΪszWLength�������ɵ��ַ�����*/
EXPORT HRESULT Get_Length_StringW(LPWSTR lpWLength,int sizeof_szWLength_in_characters);




/*��ȡ��ǰ��ʣ��ʱ��*/
EXPORT HRESULT GetCurrentAndRemainA(LPSTR szCurrent,int sizeof_szCurrent_in_characters,
						LPSTR szRemain,int sizeof_szRemain_in_characters);
												
EXPORT HRESULT GetCurrentAndRemainW(LPWSTR szWCurrent,int sizeof_szWCurrent_in_characters,
						LPWSTR szWRemain,int sizeof_szWRemain_in_characters);



/* ��ȡ��������,һ��Ϊ��λ */
EXPORT HRESULT Get_Length(int *);




/* ���� */
EXPORT HRESULT Play_Music();



/* ��ͣ */
EXPORT HRESULT Pause_Music();




/* ֹͣ */
EXPORT HRESULT Stop_Music();



/*ָ�������ļ�*/
EXPORT HRESULT SpecifyMusicFileA(LPCSTR szMusicFile);
EXPORT HRESULT SpecifyMusicFileW(LPCWSTR WszMusicFile);




/* ���ò������� */
EXPORT HRESULT SetMusicRate(double dRate);




/*��������ƽ��(-10000~0~10000),0����������ƽ�⡣10000��������������-10000������������*/
EXPORT HRESULT SetPlayerBalance(long lBalance);




/*��������(-10000~0),0Ϊ���-10000����*/
EXPORT HRESULT SetPlayerVolume(long lVolume);



/*��ȡ����(-10000~0),0Ϊ���-10000����*/
EXPORT HRESULT GetPlayerVolume(long *plVolume);




/*��ȡ��ǰ����λ��,����Ϊ��λ*/
EXPORT HRESULT GetPlayerCurrentPosition(int *pCurrentPosition);




/*���ò���λ��,����Ϊ��λ*/ 
EXPORT HRESULT SetPlayerPosition(int newPosition);




/*��ȡ��ǰ״̬
typedef enum _FilterState
{   
    State_Stopped   = 0,
    State_Paused    = State_Stopped + 1,
    State_Running   = State_Paused + 1
} FILTER_STATE;
*/

EXPORT HRESULT GetCurrentState(OAFilterState *pFilterState); 

#ifdef      UNICODE
        
#define     SpecifyMusicFile SpecifyMusicFileW
#define     Get_Length_String Get_Length_StringW
#define		GetCurrentAndRemain GetCurrentAndRemainW
        
#else
        
#define     SpecifyMusicFile SpecifyMusicFileA
#define     Get_Length_String Get_Length_StringA    
#define		GetCurrentAndRemain GetCurrentAndRemainA    
#endif

#endif

