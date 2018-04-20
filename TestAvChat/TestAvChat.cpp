// TestAvChat.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "TestAvChat.h"
#include "TestAvChatDlg.h"

#include <mipavcodecencoder.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestAvChatApp

BEGIN_MESSAGE_MAP(CTestAvChatApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTestAvChatApp ����

CTestAvChatApp::~CTestAvChatApp()
{
	CoUninitialize();	
}

CTestAvChatApp::CTestAvChatApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
	CoInitialize(NULL);
}


// Ψһ��һ�� CTestAvChatApp ����

CTestAvChatApp theApp;


// CTestAvChatApp ��ʼ��

BOOL CTestAvChatApp::InitInstance()
{
	//AllocConsole(); // ����ǰ�����ŵ���������
	//freopen("CONIN$", "r+t", stdin); // �ض��� STDIN
	//freopen("CONOUT$", "w+t", stdout); // �ض���STDOUT

	MIPAVCodecEncoder::initAVCodec();
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	//SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CTestAvChatDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˴����ô����ʱ�á�ȷ�������ر�
		//  �Ի���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ�á�ȡ�������ر�
		//  �Ի���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
