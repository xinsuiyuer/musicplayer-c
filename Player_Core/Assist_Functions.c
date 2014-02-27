#include <windows.h>
#include <windowsx.h>
#include <strsafe.h>
#include <math.h>
#include "Common.h"

HRESULT ConvertTimeToStringA(LONGLONG duration,
					LPSTR szreturn_string,int sizeof_szreturn_string)
{
	HRESULT hr;
	double fduration=0.0,the_fraction=0.0,the_integer=0.0,no_use;
	char sztemp[10];
	duration=(duration/10000000);
	fduration=duration/60.0;
	the_fraction=modf(fduration,&the_integer)*60;
	if(the_fraction<10)
	{
		if(modf(the_fraction,&no_use)>0.500000)
		{
			hr=StringCchPrintfA(sztemp,sizeof(sztemp),"%3.0f:0%d",the_integer,(int)(the_fraction+1));
		}
		else
		{
			hr=StringCchPrintfA(sztemp,sizeof(sztemp),"%3.0f:0%d",the_integer,(int)the_fraction);
		}
	}
	else 
	{
		hr=StringCchPrintfA(sztemp,sizeof(sztemp),"%3.0f:%d",the_integer,(int)the_fraction);
	}
	
	if(SUCCEEDED(hr))
	{
		hr=StringCchCopyA(szreturn_string,sizeof_szreturn_string,sztemp);
	}
	return hr;
}


HRESULT ConvertTimeToStringW(LONGLONG duration,
					LPWSTR szWreturn_string,int sizeof_szWreturn_string)
{
	HRESULT hr;
	double fduration=0.0,the_fraction=0.0,the_integer=0.0,no_use;
	WCHAR Wsztemp[10];
	duration=(duration/10000000);
	fduration=duration/60.0;
	the_fraction=modf(fduration,&the_integer)*60;
	if(the_fraction<10)
	{
		if(modf(the_fraction,&no_use)>0.500000)
		{
			hr=StringCchPrintfW(Wsztemp,sizeof(Wsztemp)/sizeof(WCHAR),L"%3.0f:0%d",the_integer,(int)(the_fraction+1));
		}
		else
		{
			hr=StringCchPrintfW(Wsztemp,sizeof(Wsztemp)/sizeof(WCHAR),L"%3.0f:0%d",the_integer,(int)the_fraction);
		}
	}
	else 
	{
		hr=StringCchPrintfW(Wsztemp,sizeof(Wsztemp)/sizeof(WCHAR),L"%3.0f:%d",the_integer,(int)the_fraction);
	}
	if(SUCCEEDED(hr))
	{
		hr=StringCchCopyW(szWreturn_string,sizeof_szWreturn_string,Wsztemp);
	}
	
	return hr;
}

HRESULT ConvertRemainTimeToStringA(LONGLONG duration,
					LPSTR szreturn_string,int sizeof_szreturn_string)
{
	HRESULT hr;
	double fduration=0.0,the_fraction=0.0,the_integer=0.0,no_use;
	char sztemp[10];
	duration=(duration/10000000);
	fduration=duration/60.0;
	the_fraction=modf(fduration,&the_integer)*60;
	if(the_fraction<10)
	{
		if(modf(the_fraction,&no_use)>0.500000)
		{
			hr=StringCchPrintfA(sztemp,sizeof(sztemp),"-%3.0f:0%d",the_integer,(int)(the_fraction+1));
		}
		else
		{
			hr=StringCchPrintfA(sztemp,sizeof(sztemp),"-%3.0f:0%d",the_integer,(int)the_fraction);
		}
	}
	else 
	{
		hr=StringCchPrintfA(sztemp,sizeof(sztemp),"-%3.0f:%d",the_integer,(int)the_fraction);
	}
	
	if(SUCCEEDED(hr))
	{
		hr=StringCchCopyA(szreturn_string,sizeof_szreturn_string,sztemp);
	}
	return hr;
}


HRESULT ConvertRemainTimeToStringW(LONGLONG duration,
					LPWSTR szWreturn_string,int sizeof_szWreturn_string)
{
	HRESULT hr;
	double fduration=0.0,the_fraction=0.0,the_integer=0.0,no_use;
	WCHAR Wsztemp[10];
	duration=(duration/10000000);
	fduration=duration/60.0;
	the_fraction=modf(fduration,&the_integer)*60;
	if(the_fraction<10)
	{
		if(modf(the_fraction,&no_use)>0.500000)
		{
			hr=StringCchPrintfW(Wsztemp,sizeof(Wsztemp)/sizeof(WCHAR),L"-%3.0f:0%d",the_integer,(int)(the_fraction+1));
		}
		else
		{
			hr=StringCchPrintfW(Wsztemp,sizeof(Wsztemp)/sizeof(WCHAR),L"-%3.0f:0%d",the_integer,(int)the_fraction);
		}
	}
	else 
	{
		hr=StringCchPrintfW(Wsztemp,sizeof(Wsztemp)/sizeof(WCHAR),L"-%3.0f:%d",the_integer,(int)the_fraction);
	}
	if(SUCCEEDED(hr))
	{
		hr=StringCchCopyW(szWreturn_string,sizeof_szWreturn_string,Wsztemp);
	}
	
	return hr;
}
