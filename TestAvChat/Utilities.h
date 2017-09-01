#pragma once

#include <string.h>

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include <atlconv.h>
class CUtilities
{
public:
	enum
	{
		UNICODE_CALC_SIZE	= 1,
		UNICODE_GET_BYTES	= 2
	};
	
public:
	CUtilities();
	virtual ~CUtilities();
	
public:
	static BOOL ParseFilePath(IN CString path,OUT CString& dir,OUT CString& filename);

	/** 按照nScaleWidth与nScaleHeight的比例,在oldRect中找到一个最大的不变形NewRect */
	static void GetScaleRect(IN int nScaleWidth, IN int nScaleHeight, IN CRect& oldRect, OUT CRect& NewRect);

	static BOOL GetRegValue(HKEY hRootKey, LPCTSTR lpSubKey, LPCTSTR lpKey, DWORD& dwValue);
	static void WriteRegKey(HKEY hRootKey, LPCTSTR lpSubKey, LPCTSTR lpKey, DWORD dwValue);
	static CString GetAppDirectory(void);
	static CString GetExtName(IN CString csName);
	static void UTF8ToGB2312(OUT CString &dstGB2312 , CString srcUTF8);
	static void GB2312ToUTF8(OUT CString &dstUTF8 , CString srcGB2312);
	static void SpilitString(CString csString , CString csSeps, OUT CStringArray& strArray);
	static string UnEscape(char* p_unicode_escape_chars);
	static int unicode_bytes(char* p_unicode_escape_chars, wchar_t *bytes, int flag);
	static CString GetCurrentSysTime(BOOL bUseMillitm = TRUE,BOOL bOnltShowDate=FALSE);
	static __int64 GetMillisecondFrom1970();
	static CString CutString(CString pm_strText, LPCTSTR pm_lpStartChars, LPCTSTR pm_lpEndChars = NULL, BOOL pm_bIncludeStart = FALSE, BOOL pm_bIncludeEnd = FALSE);
	static CStringW CutStringW(const CStringW pm_wstrText, LPCWSTR pm_lpwStartChars, LPCWSTR pm_lpwEndChars = NULL, BOOL pm_bIncludeStart = FALSE, BOOL pm_bIncludeEnd = FALSE);
	static CSize GetScreenSize();

	/** BGR24 转 RGB24，原理是1-3字节对调 */
	static void BGR24_RGB24(BYTE *dataBgr24,int length);
	//static void DrawString(Graphics& g,CString strText, REAL x,REAL y,CString csFontName/*="微软雅黑"*/, float fontSize/*=10.5f*/,Color clr/*=Color(255,255,255,255)*/);
	
	static vector<string> get_local_ipaddress();
};

