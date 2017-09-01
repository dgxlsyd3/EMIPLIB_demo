// TestAvChatDlg.h : Í·ÎÄ¼þ
//

#pragma once

#include <rtpipv4address.h>
#include <rtpudpv4transmitter.h>

#include <mipwin32output.h>

#include <rtpsession.h>
#include <rtpsessionparams.h>

#include <mipvideosession.h>
#include <mipvideosession.h>
#include <mipaudiosession.h>

#include "ExactTimer.h"
#include "FullFrameWnd.h"

#include <iostream>
#include <vector>
using namespace std;

class MyMIPAudioSession;
class MyMIPVideoSession;

class CTestAvChatDlg : public CDialog,MIPWin32OutputEvent
{	
	DECLARE_MESSAGE_MAP()
public:
	CTestAvChatDlg(CWnd* pParent = NULL);
	~CTestAvChatDlg();

	enum { IDD = IDD_TESTAVCHAT_DIALOG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
protected:
	HICON m_hIcon;
public:
	virtual void OnOutput(MIPRawRGBVideoMessage *pRawRGBMsg);
public:
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButton_Start();
	afx_msg void OnBnClickedButton_Stop();
	afx_msg void OnBnClickedCheckLocalRgb();
	afx_msg void OnBnClickedCheckLocalV();
	afx_msg void OnBnClickedCheckRemoteRgb();
	afx_msg void OnBnClickedCheckRemoteV();
	afx_msg void OnStnDblclickStaticLocalVideoWnd();
	afx_msg void OnStnDblclickStaticRemoteVideoWnd();
public:
	MyMIPAudioSession *m_pAudioSession;
	RTPSession *m_pAudioRTPSession;
public:
	MyMIPVideoSession *m_pMIPVideoSession;
	RTPSession *m_pVideoRTPSession;
	uint32_t m_LocalVideoSSRC;

	int m_nLocalFrameRate;
	int m_nRemoteFrameRate;
	CExactTimer m_Timer;
	CFont m_Font;
	CString m_sText;
	CFullFrameWnd* m_pFullWnd;
	uint8_t m_ImgData[320*240*3];

	bool m_bLocalRgb;
	bool m_bLocalV;
	bool m_bRemoteRgb;
	bool m_bRemoteV;
	
	bool m_bLocalFull;
	bool m_bRemoteFull;
public:
	void DeleteAll();
	void ZeroAll();
	BOOL InitVideoSession(DWORD dwRemoteAddress);
	BOOL InitAudioSession(DWORD dwRemoteAddress);
	void ShowFullWnd(bool bShow);
	vector<string> GetCaptureList();
	vector<string> m_strCaptureList;
};
