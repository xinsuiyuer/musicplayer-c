#ifndef _DLLMAIN_H
#define _DLLMAIN_H
typedef long OAFilterState;


#ifdef      __cplusplus
        
#define     EXPORT extern "C" __declspec (dllexport)
        
#else
        
#define     EXPORT __declspec (dllexport)
        
#endif




/* 获取歌曲长度字符串(ANSC)，只是方便显示 */
EXPORT HRESULT Get_Length_StringA(LPSTR lpszLength,int sizeof_szLength_in_characters);

/* 获取歌曲长度字符串(UniCode),第二个参数为szWLength所能容纳的字符个数*/
EXPORT HRESULT Get_Length_StringW(LPWSTR lpWLength,int sizeof_szWLength_in_characters);




/*获取当前和剩余时间*/
EXPORT HRESULT GetCurrentAndRemainA(LPSTR szCurrent,int sizeof_szCurrent_in_characters,
						LPSTR szRemain,int sizeof_szRemain_in_characters);
												
EXPORT HRESULT GetCurrentAndRemainW(LPWSTR szWCurrent,int sizeof_szWCurrent_in_characters,
						LPWSTR szWRemain,int sizeof_szWRemain_in_characters);



/* 获取歌曲长度,一秒为单位 */
EXPORT HRESULT Get_Length(int *);




/* 播放 */
EXPORT HRESULT Play_Music();



/* 暂停 */
EXPORT HRESULT Pause_Music();




/* 停止 */
EXPORT HRESULT Stop_Music();



/*指定音乐文件*/
EXPORT HRESULT SpecifyMusicFileA(LPCSTR szMusicFile);
EXPORT HRESULT SpecifyMusicFileW(LPCWSTR WszMusicFile);




/* 设置播放速率 */
EXPORT HRESULT SetMusicRate(double dRate);




/*设置声道平衡(-10000~0~10000),0则左右声道平衡。10000则左声道无声，-10000则右声道无声*/
EXPORT HRESULT SetPlayerBalance(long lBalance);




/*设置音量(-10000~0),0为最大。-10000则静音*/
EXPORT HRESULT SetPlayerVolume(long lVolume);



/*获取音量(-10000~0),0为最大。-10000则静音*/
EXPORT HRESULT GetPlayerVolume(long *plVolume);




/*获取当前播放位置,以秒为单位*/
EXPORT HRESULT GetPlayerCurrentPosition(int *pCurrentPosition);




/*设置播放位置,以秒为单位*/ 
EXPORT HRESULT SetPlayerPosition(int newPosition);




/*获取当前状态
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

