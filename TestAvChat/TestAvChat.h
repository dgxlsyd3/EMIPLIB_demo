// TestAvChat.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTestAvChatApp:
// �йش����ʵ�֣������ TestAvChat.cpp
//

class CTestAvChatApp : public CWinApp
{
public:
	CTestAvChatApp();
	~CTestAvChatApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTestAvChatApp theApp;