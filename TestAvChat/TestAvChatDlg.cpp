// TestAvChatDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestAvChat.h"
#include "TestAvChatDlg.h"
#include <DShow.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <iostream>
using namespace std;

#include "Utilities.h"

#ifdef _DEBUG
	#pragma comment(lib,"jthread_d.lib")
	#pragma comment(lib,"jrtplib_d.lib")
	#pragma comment(lib,"emiplib_d.lib")
#else
	#pragma comment(lib,"jthread.lib")
	#pragma comment(lib,"jrtplib.lib")
	#pragma comment(lib,"emiplib.lib")
#endif // _DEBUG

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"swscale.lib")

#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"amstrmid.lib")


class MyMIPAudioSession:public MIPAudioSession
{
	void onInputThreadExit(bool err, const std::string &compName, const std::string &errStr)
	{
		if(err)
		TRACE("0x%03x MIPAudioSession::onInputThreadExit,comp:%s,err:%s",
			GetCurrentThreadId(),compName.c_str(),errStr.c_str());
	}
	void onOutputThreadExit(bool err, const std::string &compName, const std::string &errStr)	
	{
		if(err)
		TRACE("0x%03x MIPAudioSession::onOutputThreadExit,comp:%s,err:%s",
			GetCurrentThreadId(),compName.c_str(),errStr.c_str());
	}
	void onIOThreadExit(bool err, const std::string &compName, const std::string &errStr)	
	{
		if(err)
		TRACE("0x%03x MIPAudioSession::onIOThreadExit,comp:%s,err:%s",
			GetCurrentThreadId(),compName.c_str(),errStr.c_str());
	}
};
class MyMIPVideoSession:public MIPVideoSession
{
	void onInputThreadExit(bool err, const std::string &compName, const std::string &errStr)
	{
		if(err)
		TRACE("0x%03x MIPVideoSession::onInputThreadExit,comp:%s,err:%s",
			GetCurrentThreadId(),compName.c_str(),errStr.c_str());
	}
	void onOutputThreadExit(bool err, const std::string &compName, const std::string &errStr)	
	{
		if(err)
		TRACE("0x%03x MIPVideoSession::onOutputThreadExit,comp:%s,err:%s",
			GetCurrentThreadId(),compName.c_str(),errStr.c_str());
	}
	void onIOThreadExit(bool err, const std::string &compName, const std::string &errStr)	
	{
		if(err)
		TRACE("0x%03x MIPVideoSession::onIOThreadExit,comp:%s,err:%s",
			GetCurrentThreadId(),compName.c_str(),errStr.c_str());
	}
};

CTestAvChatDlg::CTestAvChatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestAvChatDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	ZeroAll();

	m_bLocalRgb=false;
	m_bLocalV=false;
	m_bRemoteRgb=false;
	m_bRemoteV=false;

	m_nLocalFrameRate=0;
	m_nRemoteFrameRate=0;

	m_pFullWnd=NULL;
	m_bLocalFull=false;
	m_bRemoteFull=false;
}

CTestAvChatDlg::~CTestAvChatDlg()
{
}

void CTestAvChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestAvChatDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_Start, OnBnClickedButton_Start)
	ON_BN_CLICKED(IDC_BUTTON_Stop, OnBnClickedButton_Stop)
	ON_BN_CLICKED(IDC_CHECK_LOCAL_RGB, OnBnClickedCheckLocalRgb)
	ON_BN_CLICKED(IDC_CHECK_LOCAL_V, OnBnClickedCheckLocalV)
	ON_BN_CLICKED(IDC_CHECK_REMOTE_RGB, OnBnClickedCheckRemoteRgb)
	ON_BN_CLICKED(IDC_CHECK_REMOTE_V, OnBnClickedCheckRemoteV)
	ON_STN_DBLCLK(IDC_STATIC_LOCAL_VIDEO_WND, OnStnDblclickStaticLocalVideoWnd)
	ON_STN_DBLCLK(IDC_STATIC_REMOTE_VIDEO_WND, OnStnDblclickStaticRemoteVideoWnd)
END_MESSAGE_MAP()

BOOL CTestAvChatDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);	

	m_Font.CreatePointFont(90,"Consolas");

	CIPAddressCtrl *pIPAddressCtrl=(CIPAddressCtrl *)GetDlgItem(IDC_IPADDRESS_REMOTE);
	BYTE nField0=127, nField1=0, nField2=0, nField3=2; 
	pIPAddressCtrl->SetAddress(nField0, nField1, nField2, nField3);

	CComboBox* pDeviceIdComboBox=(CComboBox*)GetDlgItem(IDC_COMBO_DEVICEID);
	pDeviceIdComboBox->ResetContent();

	m_strCaptureList = GetCaptureList();
	for(int i=0;i!=m_strCaptureList.size();i++)
	{
		pDeviceIdComboBox->AddString(m_strCaptureList[i].c_str());
	}

	pDeviceIdComboBox->SetCurSel(0);

	GetDlgItem(IDC_BUTTON_Start)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_Stop)->EnableWindow(FALSE);

	return TRUE;
}

vector<string> CTestAvChatDlg::GetCaptureList()
{
	vector<string> strCaptureList;
	
	UINT uIndex = 0;
	ICreateDevEnum *pCreateDevEnum=0;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	if(hr != NOERROR)
	{
		return strCaptureList;
	}

	IEnumMoniker *pEm=0;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
	if(hr != NOERROR)
	{
		return strCaptureList;
	}

	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;

	while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK)
	{
		IPropertyBag *pBag=0;

		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if(hr == NOERROR)
			{
				USES_CONVERSION;
				strCaptureList.push_back(W2T(var.bstrVal));
				SysFreeString(var.bstrVal);
				pM->AddRef();
			}
			pBag->Release();
		}
		pM->Release();
		uIndex++;
	}
	pEm->Release();

	return strCaptureList;
}

void CTestAvChatDlg::ShowFullWnd(bool bShow)
{
	if(m_pFullWnd->GetSafeHwnd()==NULL)
	{
		m_pFullWnd=new CFullFrameWnd(this);
		m_pFullWnd->Create(NULL,NULL);
		m_pFullWnd->ModifyStyle(WS_TILEDWINDOW,0,SWP_DRAWFRAME);
		m_pFullWnd->ModifyStyleEx(WS_EX_CLIENTEDGE,0,SWP_DRAWFRAME);
		m_pFullWnd->ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW,0);
		m_pFullWnd->SetWindowPos(&wndTop,0,0,
			GetSystemMetrics(SM_CXSCREEN),
			GetSystemMetrics(SM_CYSCREEN),SWP_SHOWWINDOW);
	}

	if (bShow)
		m_pFullWnd->ShowWindow(SW_SHOW);
	else
		m_pFullWnd->ShowWindow(SW_HIDE);
}

void CTestAvChatDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); 

		SendMessage(WM_ICONERASEBKGND, 
			reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CTestAvChatDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestAvChatDlg::OnDestroy()
{
	DeleteAll();

	if (m_pFullWnd->GetSafeHwnd()!=NULL)
	{
		m_pFullWnd->DestroyWindow();
	}
	CDialog::OnDestroy();
}

CExactTimer t;
void CTestAvChatDlg::OnOutput( MIPRawRGBVideoMessage *pRawRGBMsg )
{
	t.Start();
	uint64_t nCurrentSSRC=pRawRGBMsg->getSourceID();
	double TimePassage=m_Timer.GetTimePassage(CExactTimer::unitMilliseconds);

	int width=pRawRGBMsg->getWidth();
	int height=pRawRGBMsg->getHeight();
	
	BITMAPINFO bmpInfo;
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
	bmpInfo.bmiHeader.biWidth = width; 

	//图形倒立问题，乘以-1解决
	bmpInfo.bmiHeader.biPlanes = 1; 
	bmpInfo.bmiHeader.biBitCount = 24;
	bmpInfo.bmiHeader.biSizeImage = width*height*3;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	memcpy(m_ImgData,pRawRGBMsg->getImageData(),width*height*3);
	
	CWnd *pWnd=NULL;//取得合适的显示区域
	if (m_LocalVideoSSRC==nCurrentSSRC)
	{
		m_nLocalFrameRate++;

		if (!m_bLocalRgb)
		{
			//t.Start();
			CUtilities::BGR24_RGB24(m_ImgData,width*height*3);//2ms
			//t.ShowTimePassage();
		}

		bmpInfo.bmiHeader.biHeight =  1 * height;
		if (!m_bLocalV)
		{
			bmpInfo.bmiHeader.biHeight =  -1 * height;
		}
		pWnd=GetDlgItem(IDC_STATIC_LOCAL_VIDEO_WND);
	}
	else
	{
		m_nRemoteFrameRate++;

		if (!m_bRemoteRgb)
		{
			CUtilities::BGR24_RGB24(m_ImgData,width*height*3);//2ms
		}
		bmpInfo.bmiHeader.biHeight =  1 * height;
		if (!m_bRemoteV)
		{
			bmpInfo.bmiHeader.biHeight =  -1 * height;
		}

		pWnd=GetDlgItem(IDC_STATIC_REMOTE_VIDEO_WND);
	}

	if (m_bLocalFull || m_bRemoteFull)
	{
		pWnd=m_pFullWnd;
	}
	
	if((m_bLocalFull && m_LocalVideoSSRC==nCurrentSSRC) ||
	   (m_bRemoteFull && m_LocalVideoSSRC!=nCurrentSSRC) ||
	   (!m_bLocalFull && !m_bRemoteFull))
	{
		
		CRect rcWnd,rcOutputRect;
		pWnd->GetClientRect(&rcWnd);
		CUtilities::GetScaleRect(width,height,rcWnd,rcOutputRect);

		CDC *pDC=pWnd->GetDC();
		CDC dcMemory;
		dcMemory.CreateCompatibleDC(pDC);

		CBitmap bmp;
		bmp.CreateCompatibleBitmap(pDC,rcWnd.Width(),rcWnd.Height());
		dcMemory.SelectObject(&bmp);
		dcMemory.SelectObject(&m_Font);
		dcMemory.SetBkMode(TRANSPARENT);
		dcMemory.SetTextColor(RGB(0xff,0xff,0xff));

		SetStretchBltMode(dcMemory.m_hDC,COLORONCOLOR);
		StretchDIBits(
			dcMemory.m_hDC, 
			rcOutputRect.left,rcOutputRect.top,
			rcOutputRect.Width(),rcOutputRect.Height(),
			0,0,width,height,
			(void*)m_ImgData, &bmpInfo, DIB_RGB_COLORS, SRCCOPY); //2ms | 14ms

		CString csTextTimer;
		if(TimePassage>=1000.0)
		{
			if (m_LocalVideoSSRC==nCurrentSSRC)
			{
				m_sText.Format("frame rate: %d fps",m_nLocalFrameRate);
			}
			else
			{
				m_sText.Format("frame rate: %d fps",m_nRemoteFrameRate);
			}
		}


		dcMemory.TextOut(rcOutputRect.left+10,rcOutputRect.top+10,m_sText);
		    csTextTimer.Format(" take time: %1.2f ms",t.GetTimePassage());
		dcMemory.TextOut(rcOutputRect.left+10,rcOutputRect.top+30,csTextTimer);

		pDC->BitBlt(0,0,rcWnd.Width(),rcWnd.Height(),&dcMemory,0,0,SRCCOPY);

		dcMemory.DeleteDC();
		bmp.DeleteObject();

		pWnd->ReleaseDC(pDC);
	}

	if (TimePassage>=1000.0)
	{
		m_nLocalFrameRate=0;
		m_nRemoteFrameRate=0;
		m_Timer.Start();
	}
}

void CTestAvChatDlg::OnBnClickedButton_Start()
{	
	CIPAddressCtrl *pIPAddressCtrl=(CIPAddressCtrl *)GetDlgItem(IDC_IPADDRESS_REMOTE);
	DWORD dwAddress;
	int nRet=pIPAddressCtrl->GetAddress(dwAddress);
	if (nRet!=4)
	{
		AfxMessageBox("请输入有效IP.");
		return;
	}

	if (!InitVideoSession(dwAddress))
	{
		return;
	}
	
	if (!InitAudioSession(dwAddress))
	{
		DeleteAll();
		return;
	}

	GetDlgItem(IDC_BUTTON_Start)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_Stop)->EnableWindow(TRUE);
}


BOOL CTestAvChatDlg::InitVideoSession(DWORD dwRemoteAddress)
{
	MIPVideoSessionParams videoParams;

	if(m_strCaptureList.size()>0)
	{
		CComboBox* pDeviceIdComboBox=(CComboBox*)GetDlgItem(IDC_COMBO_DEVICEID);
		videoParams.setSessionType(MIPVideoSessionParams::InputOutput);
		videoParams.setDevice(pDeviceIdComboBox->GetCurSel());
	}
	else
	{
		AfxMessageBox("没有检测到摄像头，对方不能看到你！");
		videoParams.setSessionType(MIPVideoSessionParams::OutputOnly);
	}

	videoParams.setFrameRate(15.0);
	videoParams.setAvcodecFrameRate(25.0);
	videoParams.setWidth(320);
	videoParams.setHeight(240);
	videoParams.setPortbase(6100);
	videoParams.setBandwidth(1024*1024);
	videoParams.setMaximumPayloadSize(1024);
	videoParams.setUseWin32Output(true);
	videoParams.setMIPWin32OutputEvent(this);
	videoParams.setEncodingType(MIPVideoSessionParams::H263);
	videoParams.setWaitForKeyframe(true);

	RTPSessionParams rtpSessionParams;
	rtpSessionParams.SetOwnTimestampUnit(1.0/25.0);
	rtpSessionParams.SetMaximumPacketSize(videoParams.getMaximumPayloadSize()+12);
	rtpSessionParams.SetAcceptOwnPackets(true);

	RTPUDPv4TransmissionParams rtpUDPv4TransmissionParams;
	rtpUDPv4TransmissionParams.SetPortbase(videoParams.getPortbase());
	rtpUDPv4TransmissionParams.SetRTPSendBuffer(1024*512);
	rtpUDPv4TransmissionParams.SetRTPReceiveBuffer(1024*512);

	m_pVideoRTPSession=new RTPSession();
	int status = m_pVideoRTPSession->Create(rtpSessionParams,&rtpUDPv4TransmissionParams);
	if (status<0)
	{
		AfxMessageBox(("m_pVideoRTPSession->Create:"+RTPGetErrorString(status)).c_str());
		DeleteAll();
		return FALSE;
	}

	m_pMIPVideoSession=new MyMIPVideoSession();
	bool bInit=m_pMIPVideoSession->init(&videoParams,0,m_pVideoRTPSession);
	if (!bInit)
	{
		AfxMessageBox(("m_pMIPVideoSession->init:"+m_pMIPVideoSession->getErrorString()).c_str());
		DeleteAll();
		return FALSE;
	}

	m_LocalVideoSSRC = m_pVideoRTPSession->GetLocalSSRC();
	bool bAddDest = m_pMIPVideoSession->addDestination("127.0.0.1",videoParams.getPortbase());
	if(!bAddDest)
	{
		AfxMessageBox(("m_pMIPVideoSession.addDestination:"+m_pMIPVideoSession->getErrorString()).c_str());
		DeleteAll();
		return FALSE;
	}

	bAddDest=m_pMIPVideoSession->addDestination(RTPIPv4Address(dwRemoteAddress,videoParams.getPortbase()));
	if (!bAddDest)
	{
		AfxMessageBox(("m_pMIPVideoSession.addDestination:"+m_pMIPVideoSession->getErrorString()).c_str());
		DeleteAll();
		return FALSE;
	}

	m_nLocalFrameRate=0;
	m_nRemoteFrameRate=0;
	m_Timer.Start();

	return TRUE;
}

BOOL CTestAvChatDlg::InitAudioSession(DWORD dwRemoteAddress)
{
	MIPAudioSessionParams Aparams;
	Aparams.setPortbase(6000);//接收音频RTP数据包的UDP端口
	Aparams.setAcceptOwnPackets(false);
	Aparams.setCompressionType(MIPAudioSessionParams::ULaw);
	//Aparams.setSpeexEncoding(MIPAudioSessionParams::NarrowBand);
	Aparams.setUseHighPriority(true);

	RTPSessionParams sessParams;
	sessParams.SetOwnTimestampUnit(1.0/8000.0);
	sessParams.SetMaximumPacketSize(2000);
	sessParams.SetAcceptOwnPackets(Aparams.getAcceptOwnPackets());

	RTPUDPv4TransmissionParams transParams;
	transParams.SetPortbase(Aparams.getPortbase());

	m_pAudioRTPSession=new RTPSession();
	int status = m_pAudioRTPSession->Create(sessParams,&transParams);
	if (status < 0)
	{
		AfxMessageBox(("m_pAudioRTPSession->Create:"+RTPGetErrorString(status)).c_str());
		DeleteAll();
		return FALSE;
	}

	m_pAudioSession = new MyMIPAudioSession();
	if (!m_pAudioSession->init(&Aparams,0,m_pAudioRTPSession))
	{
		AfxMessageBox(("m_pAudioSession->init:"+m_pAudioSession->getErrorString()).c_str());
		DeleteAll();
		return FALSE;
	}

// 	int bAddDest=m_pAudioSession->addDestination("127.0.0.1",Aparams.getPortbase());
// 	if (!bAddDest)
// 	{
// 		AfxMessageBox(("m_pAudioSession->addDestination:"+m_pAudioSession->getErrorString()).c_str());
// 		DeleteAll();
// 		return FALSE;
// 	}

	int bAddDest=m_pAudioSession->addDestination(RTPIPv4Address(dwRemoteAddress,Aparams.getPortbase()));
	if (!bAddDest)
	{
		AfxMessageBox(("m_pAudioSession->addDestination:"+m_pAudioSession->getErrorString()).c_str());
		DeleteAll();
		return FALSE;
	}

	return TRUE;
}

void CTestAvChatDlg::DeleteAll()
{
	if (m_pMIPVideoSession!=NULL)
	{
		m_pMIPVideoSession->destroy();
		delete m_pMIPVideoSession;
	}
	
	if (m_pVideoRTPSession!=NULL)
	{
		//等待200ms
		//m_pVideoRTPSession->BYEDestroy(RTPTime(0,200),0,0);
		m_pVideoRTPSession->Destroy();
		delete m_pVideoRTPSession;
	}

	if (m_pAudioSession!=NULL)
	{
		m_pAudioSession->destroy();
		delete m_pAudioSession;
	}

	if (m_pAudioRTPSession!=NULL)
	{
		//m_pAudioRTPSession->BYEDestroy(RTPTime(0,200),0,0);
		m_pAudioRTPSession->Destroy();
		delete m_pAudioRTPSession;
	}

	ZeroAll();
}

void CTestAvChatDlg::ZeroAll()
{
	m_pVideoRTPSession=NULL;
	m_pMIPVideoSession=NULL;
	m_LocalVideoSSRC=0;

	m_pAudioSession=NULL;
	m_pAudioRTPSession=NULL;
}

void CTestAvChatDlg::OnBnClickedButton_Stop()
{
	DeleteAll();
	GetDlgItem(IDC_BUTTON_Start)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_Stop)->EnableWindow(FALSE);
}

void CTestAvChatDlg::OnBnClickedCheckLocalRgb()
{
	m_bLocalRgb=(((CButton*)GetDlgItem(IDC_CHECK_LOCAL_RGB))->GetCheck()==1);
}

void CTestAvChatDlg::OnBnClickedCheckLocalV()
{
	m_bLocalV=(((CButton*)GetDlgItem(IDC_CHECK_LOCAL_V))->GetCheck()==1);
}

void CTestAvChatDlg::OnBnClickedCheckRemoteRgb()
{
	m_bRemoteRgb=(((CButton*)GetDlgItem(IDC_CHECK_REMOTE_RGB))->GetCheck()==1);
}

void CTestAvChatDlg::OnBnClickedCheckRemoteV()
{
	m_bRemoteV=(((CButton*)GetDlgItem(IDC_CHECK_REMOTE_V))->GetCheck()==1);
}

void CTestAvChatDlg::OnStnDblclickStaticLocalVideoWnd()
{
	m_bRemoteFull=false;
	ShowFullWnd(m_bLocalFull=true);
}

void CTestAvChatDlg::OnStnDblclickStaticRemoteVideoWnd()
{
	m_bLocalFull=false;
	ShowFullWnd(m_bRemoteFull=true);
}
