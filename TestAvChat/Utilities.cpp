// Utilities.cpp
// 淡月清风 2011年8月31日
// 

#include "stdafx.h"
#include "Utilities.h"
#include <sys/timeb.h>

#define _CRT_SECURE_NO_WARNINGS

CUtilities::CUtilities()
{

}

CUtilities::~CUtilities()
{

}

/*
void CUtilities::DrawString(Graphics& g,CString strText,
				REAL x,REAL y,CString csFontName/ *="微软雅黑"* /,
				float fontSize/ *=10.5f* /,Color clr/ *=Color(255,255,255,255)* /)
{
	USES_CONVERSION;
	Gdiplus::Font myFont(A2W(csFontName), fontSize);
	PointF origin(x,y);
	SolidBrush blackBrush(clr);

	g.DrawString(
		A2W(strText),
		(INT)wcslen(A2W(strText)),
		&myFont,
		origin,
		&blackBrush);
}
*/

void CUtilities::GetScaleRect(
	IN int nScaleWidth,
	IN int nScaleHeight,
	IN CRect& oldRect,
	OUT CRect& NewRect)
{
	int nWidth, nHeight;
	int nLeft, nTop;
	if (oldRect.Width() >= oldRect.Height())
	{
		nHeight = oldRect.Height();// 优先短边
		nWidth = (nScaleWidth * nHeight) / nScaleHeight;
	}
	else if (oldRect.Width() < oldRect.Height())
	{
		nWidth = oldRect.Width();
		nHeight = (nScaleHeight * nWidth) / nScaleWidth;
	}
	
	nLeft = (oldRect.Width() - nWidth) / 2;
	nTop = (oldRect.Height() - nHeight) / 2;
	
	NewRect.left = nLeft;
	NewRect.top = nTop;
	NewRect.right = NewRect.left + nWidth;
	NewRect.bottom = NewRect.top + nHeight;
}

BOOL CUtilities::GetRegValue(HKEY hRootKey, LPCTSTR lpSubKey,
							 LPCTSTR lpKey, DWORD& dwValue)
{
	HKEY hKey;
	DWORD dwLen = sizeof(DWORD);
	if (ERROR_SUCCESS == RegOpenKeyEx(hRootKey, lpSubKey,
									  NULL, KEY_ALL_ACCESS, &hKey))
	{
		DWORD dwType = REG_DWORD;
		if (ERROR_SUCCESS == RegQueryValueEx(hKey, lpKey, NULL, &dwType,
											 (LPBYTE)&dwValue, &dwLen))
		{
			RegCloseKey(hKey);
			return TRUE;
		}
		RegCloseKey(hKey);
	}
	return FALSE;
}

void CUtilities::WriteRegKey(HKEY hRootKey, LPCTSTR lpSubKey,
							 LPCTSTR lpKey, DWORD dwValue)
{
	HKEY hk;
	if (RegOpenKey(hRootKey, lpSubKey, &hk) != ERROR_SUCCESS)
	{
		RegCreateKey(hRootKey, lpSubKey, &hk);
	}
	RegSetValueEx(hk, lpKey, NULL, REG_DWORD, (BYTE*)&dwValue,
				  (DWORD)sizeof(dwValue));
	RegCloseKey(hk);
}


//当前exe所在目录
CString CUtilities::GetAppDirectory(void)
{
	CString strPath = _T("");
	
	TCHAR szPath[MAX_PATH] = {0};
	DWORD dwLength = GetModuleFileName(AfxGetInstanceHandle(), szPath, MAX_PATH);
	if (dwLength > 0)
	{
		strPath = szPath;
		strPath = strPath.Mid(0, strPath.ReverseFind('\\') + 1);
	}
	return strPath;
}

CString CUtilities::GetExtName(IN CString csName)
{
	int nPos = csName.ReverseFind('.');
	return csName.Mid(nPos);
}

void CUtilities::UTF8ToGB2312(OUT CString &dstGB2312 , IN CString srcUTF8)
{
	//Utf8ToUnicode
	int iUnicode = MultiByteToWideChar(CP_UTF8, 0, srcUTF8, -1, NULL, 0 );
	WCHAR* wszUnicode = new WCHAR[iUnicode+1];
	MultiByteToWideChar(CP_UTF8, 0, srcUTF8, -1, wszUnicode, iUnicode);
	
	//UnicodeToGB2312
	int iGB2312Length = WideCharToMultiByte(CP_ACP, 0, wszUnicode, -1, NULL, 0, NULL, NULL);
	TCHAR* szGB2312 = new TCHAR[iGB2312Length+1];
	WideCharToMultiByte(CP_ACP, 0, wszUnicode, -1, szGB2312, iGB2312Length, NULL, NULL);
	
	dstGB2312 = szGB2312;
	delete []wszUnicode;
	delete []szGB2312;
}

void CUtilities::GB2312ToUTF8(OUT CString &dstUTF8 , CString srcGB2312 )
{
	//GB2312ToUnicode
	int nUnicode = MultiByteToWideChar( CP_ACP, 0, srcGB2312, -1, NULL, 0 );
	WCHAR *wszUnicode = new WCHAR[nUnicode];
	::MultiByteToWideChar( CP_ACP, 0, srcGB2312, -1, wszUnicode, nUnicode);
	
	//UnicodeToUTF8
	int nUTF8 = WideCharToMultiByte( CP_UTF8, 0, wszUnicode, -1, 0, 0, 0, 0 );
	TCHAR *szUtf8 = new TCHAR[nUTF8];
	::WideCharToMultiByte( CP_UTF8, 0, wszUnicode, -1, szUtf8, nUTF8, 0, 0 );
	
	dstUTF8 = szUtf8;
	delete []szUtf8;
	delete []wszUnicode;
}

//将unicode转义字符序列转换为GB2312字符串
string CUtilities::UnEscape(char* p_unicode_escape_chars)
{
	int nBytes = unicode_bytes(p_unicode_escape_chars, NULL, UNICODE_CALC_SIZE);
	
	wchar_t *p_bytes = new wchar_t[nBytes+sizeof(wchar_t)];
	unicode_bytes(p_unicode_escape_chars, p_bytes, UNICODE_GET_BYTES);
	p_bytes[nBytes] = 0;
	
	USES_CONVERSION;
	string cs_return = W2A((wchar_t*)p_bytes);
	
	delete[] p_bytes;
	
	return cs_return;
}

//将unicode转义字符序列转换为内存中的unicode字符串
int CUtilities::unicode_bytes(char* p_unicode_escape_chars, wchar_t *bytes, int flag)
{
	/*
	char* p_unicode_escape_chars="pp\\u4fddp\\u5b58\\u6210pp\\u529f0a12";
	
	//通过此函数获知转换后需要的字节数
	int n_length=unicode_bytes(p_unicode_escape_chars,NULL,UNICODE_CALC_SIZE);
	
	//再次调用此函数，取得字节序列
	wchar_t *bytes=new wchar_t[n_length+sizeof(wchar_t)];
	unicode_bytes(p_unicode_escape_chars,bytes,UNICODE_GET_BYTES);
	bytes[n_length]=0;
	
	//此时的bytes中是转换后的字节序列
	delete[] bytes;
	*/
	
	int unicode_count = 0;
	size_t length = strlen(p_unicode_escape_chars);
	for (size_t char_index = 0;char_index < length;char_index++)
	{
		char unicode_hex[5];
		memset(unicode_hex, 0, 5);
		
		char ascii[2];
		memset(ascii, 0, 2);
		
		if (*(p_unicode_escape_chars + char_index) == '\\')
		{
			char_index++;
			if (char_index < length)
			{
				if (*(p_unicode_escape_chars + char_index) == 'u')
				{
					if (flag == UNICODE_GET_BYTES)
					{
						memcpy(unicode_hex, p_unicode_escape_chars + char_index + 1, 4);
						
						//sscanf不可以使用unsigned short类型
						//否则：Run-Time Check Failure #2 - Stack around the variable 'a' was corrupted.
						unsigned int a = 0;
						sscanf_s(unicode_hex, "%04x", &a);
						bytes[unicode_count++] = a;
					}
					else if (flag == UNICODE_CALC_SIZE)
					{
						unicode_count++;
					}
					char_index += 4;
				}
			}
		}
		else
		{
			if (flag == UNICODE_GET_BYTES)
			{
				memcpy(ascii, p_unicode_escape_chars + char_index, 1);
				unsigned int a = 0;
				sscanf_s(ascii, "%c", &a);
				bytes[unicode_count++] = a;
			}
			else if (flag == UNICODE_CALC_SIZE)
			{
				unicode_count++;
			}
		}
	}
	
	return unicode_count;
}

void CUtilities::SpilitString(CString csString , CString csSeps, OUT CStringArray& strArray)
{
	strArray.RemoveAll();
	
	char *seps = csSeps.GetBuffer();
	
	char *next_token = NULL;
	char *token = strtok_s( csString.GetBuffer(csString.GetLength()), seps , &next_token);
	while ( token != NULL )
	{
		strArray.Add(token);
		token = strtok_s( NULL, seps , &next_token);
	}
	csString.ReleaseBuffer();
	csSeps.ReleaseBuffer();
}

CString CUtilities::GetCurrentSysTime(BOOL bUseMillitm/* = TRUE*/,BOOL bOnltShowDate/*=FALSE*/)
{
	CTime now = CTime::GetCurrentTime();
	CString cs_time = "";
	cs_time = now.Format("%Y-%m-%d %H:%M:%S");
	if (bOnltShowDate)
	{
		cs_time = now.Format("%Y-%m-%d");
	}
	
	if (bUseMillitm)
	{
		_timeb t;
		_ftime_s(&t);
		CString cs_millitm;
		cs_millitm.Format("%03d", t.millitm);
		return cs_time + "." + cs_millitm;
	}
	else
	{
		return cs_time;
	}
}

__int64 CUtilities::GetMillisecondFrom1970()
{
	//tb.time:Time in seconds since midnight (00:00:00), January 1, 1970, coordinated universal time (UTC).
	//millitm:Fraction of a second in milliseconds.
	_timeb tb;
	_ftime_s(&tb);
	
	//TRACE("%I64d\n",GetMillisecondFrom1970());
	return tb.time*1000 + tb.millitm;
}

CStringW CUtilities::CutStringW(const CStringW pm_wstrText, LPCWSTR pm_lpwStartChars, LPCWSTR pm_lpwEndChars/*=NULL*/, BOOL pm_bIncludeStart/*=FALSE*/, BOOL pm_bIncludeEnd/*=FALSE*/)
{
	ASSERT(pm_lpwStartChars != NULL);
	if (pm_lpwEndChars != NULL)
	{
		if (wcscmp(pm_lpwEndChars, L"") == 0)
		{
			pm_lpwEndChars = NULL;
		}
	}
	
	int nStartLen = (pm_lpwStartChars != NULL) ? (int)wcslen(pm_lpwStartChars) : 0;
	int nEndLen = (pm_lpwEndChars != NULL) ? (int)wcslen(pm_lpwEndChars) : 0;
	
	int nPosStart = pm_wstrText.Find(pm_lpwStartChars);
	if (-1 == nPosStart)
	{
		return L"";
	}
	
	CStringW strCutStart = pm_wstrText.Mid(nPosStart + (pm_bIncludeStart ? 0 : nStartLen));
	
	CStringW strCut = strCutStart;
	if (pm_lpwEndChars != NULL)
	{
		int nPosEnd = strCutStart.Find(pm_lpwEndChars, pm_bIncludeStart ? nStartLen : 0);
		if (-1 == nPosEnd)
		{
			return L"";
		}
		
		CStringW strCutStartEnd = strCutStart.Mid(0, nPosEnd + (pm_bIncludeEnd ? nEndLen : 0));
		strCut = strCutStartEnd;
	}
	
	return strCut;
}

CString CUtilities::CutString(CString pm_strText, LPCTSTR pm_lpStartChars, LPCTSTR pm_lpEndChars/*=NULL*/, BOOL pm_bIncludeStart/*=FALSE*/, BOOL pm_bIncludeEnd/*=FALSE*/)
{
	// 截取字符串
	// s=abcd[<123>]efg
	// s1=CutString(s,"[<",">]");
#ifdef _UNICODE
	return CutStringW(pm_strText, pm_lpStartChars, pm_lpEndChars, pm_bIncludeStart, pm_bIncludeEnd);
#else
	USES_CONVERSION;
	CStringW wstrCut = CutStringW(A2W(pm_strText), A2W(pm_lpStartChars), A2W(pm_lpEndChars), pm_bIncludeStart, pm_bIncludeEnd);
	return W2A(wstrCut);
#endif
}

CSize CUtilities::GetScreenSize()
{
	int X=GetSystemMetrics(SM_CXSCREEN);
	int Y=GetSystemMetrics(SM_CYSCREEN);

	X=1024;
	Y=768;
	return CSize(X,Y);
}

BOOL CUtilities::ParseFilePath( IN CString path,OUT CString& dir,OUT CString& filename )
{
	//E:\Desktop\QueryDlg.cpp
	path.Replace('/','\\');
	int nLastSlash=path.ReverseFind('\\');
	if (nLastSlash==-1)
	{
		return FALSE;
	}

	dir=path.Mid(0,nLastSlash+1);
	filename=path.Mid(nLastSlash+1);

	return TRUE;
}

void CUtilities::BGR24_RGB24(BYTE *dataBgr24,int length)
{
	if((length%3) != 0)
	{
		cerr<< "输入数据可能不是 RGB24 格式." << endl;
		return;
	}
	for (int i=0;i<length;i+=3)
	{
		BYTE tmp=dataBgr24[i];
		dataBgr24[i]=dataBgr24[i+2];
		dataBgr24[i+2]=tmp;
	}
}

vector<string> CUtilities::get_local_ipaddress()
{
	//本机的IP地址列表(从本地局域网的角度来看)
	vector<string> ips;

	char hostname[200];
	if(gethostname(hostname,200) == SOCKET_ERROR)
	{
		return ips;
	}

	hostent* hptr = gethostbyname(hostname);
	if (hptr == NULL)
	{
		return ips;
	}

	if (hptr->h_addrtype==AF_INET || 
		hptr->h_addrtype==AF_INET6)
	{
		//h_addr_list竟然定义成了char FAR * FAR *，实际上是in_addr**
		//Null-terminated list of addresses for the host. Addresses are returned in network byte order. 
		in_addr **ppaddr=(in_addr **)hptr->h_addr_list;
		for (;*ppaddr!=NULL;ppaddr++)
		{
			char * ip=inet_ntoa(**ppaddr);
			if (ip!=NULL)
			{
				ips.push_back(ip);
			}
		}
	}

	return ips;
}