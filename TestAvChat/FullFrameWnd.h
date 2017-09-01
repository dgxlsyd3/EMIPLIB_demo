#pragma once


// CFullFrameWnd
class CTestAvChatDlg;
class CFullFrameWnd : public CFrameWnd
{
	DECLARE_DYNAMIC(CFullFrameWnd)

public:
	CFullFrameWnd(CTestAvChatDlg* pTestAvChatDlg);
	virtual ~CFullFrameWnd();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnClose();
public:
	CTestAvChatDlg* m_pTestAvChatDlg;	
	void Hide();
};


