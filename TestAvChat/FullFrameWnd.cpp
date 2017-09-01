// FullFrameWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "TestAvChat.h"
#include "FullFrameWnd.h"
#include "TestAvChatDlg.h"

// CFullFrameWnd

IMPLEMENT_DYNAMIC(CFullFrameWnd, CWnd)

CFullFrameWnd::CFullFrameWnd(CTestAvChatDlg* pTestAvChatDlg)
{
	m_pTestAvChatDlg=pTestAvChatDlg;
}

CFullFrameWnd::~CFullFrameWnd()
{
}


BEGIN_MESSAGE_MAP(CFullFrameWnd, CWnd)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


void CFullFrameWnd::Hide()
{
	m_pTestAvChatDlg->m_bLocalFull=false;
	m_pTestAvChatDlg->m_bRemoteFull=false;
	ShowWindow(SW_HIDE);
}

void CFullFrameWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	Hide();
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CFullFrameWnd::OnClose()
{
	Hide();
}

BOOL CFullFrameWnd::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message==WM_KEYDOWN)
	{
		if (pMsg->wParam==VK_ESCAPE)
		{
			Hide();
			return TRUE;
		}
	}
	
	return CFrameWnd::PreTranslateMessage(pMsg);
}
