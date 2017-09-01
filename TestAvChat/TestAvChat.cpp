// TestAvChat.cpp : 定义应用程序的类行为。
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


// CTestAvChatApp 构造

CTestAvChatApp::~CTestAvChatApp()
{
	CoUninitialize();	
}

CTestAvChatApp::CTestAvChatApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	CoInitialize(NULL);
}


// 唯一的一个 CTestAvChatApp 对象

CTestAvChatApp theApp;


// CTestAvChatApp 初始化

BOOL CTestAvChatApp::InitInstance()
{
	//AllocConsole(); // 将当前程序附着到父进程上
	//freopen("CONIN$", "r+t", stdin); // 重定向 STDIN
	//freopen("CONOUT$", "w+t", stdout); // 重定向STDOUT

	MIPAVCodecEncoder::initAVCodec();
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	//SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CTestAvChatDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此处放置处理何时用“确定”来关闭
		//  对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用“取消”来关闭
		//  对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}
