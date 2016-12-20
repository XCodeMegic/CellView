
// CellViewDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CellView.h"
#include "CellViewDlg.h"
#include "afxdialogex.h"
#include "XButton.h"
#include "XLabel.h"
#include "XPicture.h"
#include "char_code_change.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCellViewDlg 对话框



CCellViewDlg::CCellViewDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CCellViewDlg::IDD, pParent), m_Id(0), m_BkImage(NULL), m_MouseTrack(TRUE)
, m_StringID(_T(""))
, m_StringPass(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCellViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_UID, m_StringID);
	DDV_MaxChars(pDX, m_StringID, 16);
	DDX_Text(pDX, IDC_EDIT_UPASS, m_StringPass);
	DDV_MaxChars(pDX, m_StringPass, 32);
}

BEGIN_MESSAGE_MAP(CCellViewDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_XBTN_CLICK, OnClickMessage)
	ON_MESSAGE(WM_INITEND, OnInitMsg)
	ON_MESSAGE(WM_XMPPMSG, OnXmppMsg)
	ON_MESSAGE(WM_TASKMSG, OnTaskMsg)
	ON_WM_MOVE()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


//-------------------------------- init env -------------------------------

static UINT EnvStartUp(LPVOID param)
{
	CCellViewApp *app = (CCellViewApp*)param;

	char bufPath[MAX_PATH];
	std::string path, eyeName, eyePath, vncName, vncPath;
	char bufTemp[MAX_PATH];

	GetModuleFileName(NULL, bufPath, MAX_PATH);

	char *p = strrchr(bufPath, '\\');

	if (p) *(p + 1) = 0;

	path = bufPath;
	path += CFG_FILE;

	app->m_XmppConn = new CXmppInstance();

	GetPrivateProfileString(CFG_EXENAME, CFG_EYEBEAM_NAME, "", bufTemp, MAX_PATH, path.c_str());
	eyeName = bufTemp;

	GetPrivateProfileString(CFG_EXENAME, CFG_EYEBEAM_PATH, "", bufTemp, MAX_PATH, path.c_str());
	eyePath = bufTemp;

	GetPrivateProfileString(CFG_EXENAME, CFG_VNCNAME, "", bufTemp, MAX_PATH, path.c_str());
	vncName = bufTemp;

	GetPrivateProfileString(CFG_EXENAME, CFG_VNCPATH, "", bufTemp, MAX_PATH, path.c_str());
	vncPath = bufTemp;

	if ((!app->m_XmppConn->LoadConfig(path.c_str())) || eyeName.empty() || eyePath.empty() ||
		vncName.empty() || vncPath.empty())
	{
		PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_INITEND, INIT_FAIL, 0);
		return 0;
	}

	//temp comment
	/*app->m_hinsEyeBeam = ShellExecute(NULL, "open", eyeName.c_str(), NULL, eyePath.c_str(), SW_HIDE);

	if ((UINT)app->m_hinsEyeBeam <= 32)
	{
	PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_INITEND, INIT_FAIL, 0);
	return 0;
	}

	app->m_hinsVNC = ShellExecute(NULL, "open", vncName.c_str(), NULL, vncPath.c_str(), SW_HIDE);
	if ((UINT)app->m_hinsVNC <= 32)
	{
	TerminateProcess(app->m_hinsEyeBeam, 0);
	PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_INITEND, INIT_FAIL, 0);
	return 0;
	}*/

	PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_INITEND, INIT_OK, 0);
	return 0;
}
//-------------------------------------------------------------------------

// CCellViewDlg 消息处理程序

BOOL CCellViewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

	//init env
	AfxBeginThread(EnvStartUp, &theApp);

	//init ctrl
	InitCtrl();

	m_ctrls = &m_Page0Ctrls;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCellViewDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		//SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		//// 使图标在工作区矩形中居中
		//int cxIcon = GetSystemMetrics(SM_CXICON);
		//int cyIcon = GetSystemMetrics(SM_CYICON);
		//CRect rect;
		//GetClientRect(&rect);
		//int x = (rect.Width() - cxIcon + 1) / 2;
		//int y = (rect.Height() - cyIcon + 1) / 2;

		//// 绘制图标
		//dc.DrawIcon(x, y, m_hIcon);
		dc.SetPixel(10, 10, RGB(255, 255, 255));
	}
	else
	{
		PAINTSTRUCT ps;
		CRect rect;
		GetClientRect(&rect);
		CDC *dc = BeginPaint(&ps);
		
		HBITMAP hbmp = CreateCompatibleBitmap(dc->GetSafeHdc(), rect.Width(), rect.Height());
		HDC		hdc = CreateCompatibleDC(dc->GetSafeHdc());
		SelectObject(hdc, hbmp);
		DrawBkImage(hdc);
		DrawControl(hdc);
		BitBlt(dc->GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), hdc, 0, 0, SRCCOPY);
		DeleteDC(hdc);
		DeleteObject(hbmp);
		EndPaint(&ps);
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCellViewDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCellViewDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	BOOL Done = FALSE;
	XCtrlList::iterator it = m_ctrls->begin();
	for (; it != m_ctrls->end(); it++)
	{
		if ((*it)->PtInCtrl(&point))
		{
			Done = TRUE;
			(*it)->OnLButtonDown();
		}
		else
			(*it)->OnLeave();
	}
	CRect rect;
	GetClientRect(&rect);
	rect.bottom = 20;
	if (!Done && PtInRect(&rect, point))
	{
		ReleaseCapture();
		PostMessage(WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}


BOOL CCellViewDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	//return CDialogEx::OnEraseBkgnd(pDC);
	return TRUE;
}

void CCellViewDlg::InitCtrl(void)
{
	CRect rect;
	GetClientRect(&rect);
	rect.left = rect.right - 30;
	rect.bottom = rect.top + 20;
	XControlBase *ctrl = new XButton(GetSafeHwnd(), m_Id, &rect);
	((XButton*)ctrl)->SetBkImage(IDB_BTNCLOSE, TEXT("PNG"));
	m_Page0Ctrls.push_back(ctrl);
	m_Page1Ctrls.push_back(ctrl);
	m_Page2Ctrls.push_back(ctrl);
	m_Page3Ctrls.push_back(ctrl);
	m_BtnClicks.insert(std::make_pair(m_Id, &CCellViewDlg::OnClickClose));
	m_Id++;
	rect.right = rect.left;
	rect.left -= 30;

	ctrl = new XButton(GetSafeHwnd(), m_Id, &rect);
	((XButton*)ctrl)->SetBkImage(IDB_BTNMIN, TEXT("PNG"));
	m_Page0Ctrls.push_back(ctrl);
	m_Page1Ctrls.push_back(ctrl);
	m_Page2Ctrls.push_back(ctrl);
	m_Page3Ctrls.push_back(ctrl);
	m_BtnClicks.insert(std::make_pair(m_Id, &CCellViewDlg::OnClickMin));
	m_Id++;

	GetClientRect(&rect);
	rect.left = rect.Width() / 2 - 100;
	rect.top = rect.Height() / 2 - 80;
	rect.right = rect.left + 200;
	rect.bottom = rect.top + 70;
	ctrl = new XLabel(GetSafeHwnd(), m_Id, &rect, 0x0000AAAA, false);
	((XLabel*)ctrl)->SetFont(16, 0x00FFFFFF, true);
	((XLabel*)ctrl)->SetText(string("-- 学号登录 -- "));
	m_Page1Ctrls.push_back(ctrl);
	m_BtnClicks.insert(std::make_pair(m_Id, &CCellViewDlg::OnClickULogin));
	m_Id++;

	rect.top = rect.bottom + 20;
	rect.bottom = rect.top + 70;
	ctrl = new XLabel(GetSafeHwnd(), m_Id, &rect, 0x0000AAAA, false);
	((XLabel*)ctrl)->SetFont(16, 0x00FFFFFF, true);
	((XLabel*)ctrl)->SetText(string("-- 游客登录 -- "));
	m_Page1Ctrls.push_back(ctrl);
	m_BtnClicks.insert(std::make_pair(m_Id, &CCellViewDlg::OnClickGstLogin));
	m_Id++;

	GetClientRect(&rect);
	CRect rcEdit;
	GetDlgItem(IDC_EDIT_UID)->GetClientRect(&rcEdit);
	rect.top = 70;
	rect.right = 100;
	rect.bottom = rcEdit.bottom - rcEdit.top + rect.top;
	ctrl = new XLabel(GetSafeHwnd(), m_Id, &rect, 0, false, false);
	((XLabel*)ctrl)->SetFont(12, 0x00EEEEEE, true);
	((XLabel*)ctrl)->SetText(string("学号: "));
	m_Page2Ctrls.push_back(ctrl);
	m_Id++;

	GetDlgItem(IDC_EDIT_UPASS)->GetClientRect(&rcEdit);
	rect.top = rect.bottom + 30;
	rect.bottom = rcEdit.bottom - rcEdit.top + rect.top;
	ctrl = new XLabel(GetSafeHwnd(), m_Id, &rect, 0, false, false);
	((XLabel*)ctrl)->SetFont(12, 0x00EEEEEE, true);
	((XLabel*)ctrl)->SetText(string("密码: "));
	m_Page2Ctrls.push_back(ctrl);
	m_Id++;

	GetClientRect(&rect);
	rect.left += 50;
	rect.bottom -= 30;
	rect.right = rect.left + 54;
	rect.top = rect.bottom - 40;
	ctrl = new XPicture(GetSafeHwnd(), m_Id, &rect, 0x0000EEFF);
	((XPicture*)ctrl)->SetImage(IDB_BTN_LAST1, TEXT("PNG"));
	m_Page2Ctrls.push_back(ctrl);
	m_Page3Ctrls.push_back(ctrl);
	m_BtnClicks.insert(std::make_pair(m_Id, &CCellViewDlg::OnClickLast));
	m_Id++;

	GetClientRect(&rect);
	rect.right -= 50;
	rect.bottom -= 30;
	rect.left = rect.right - 54;
	rect.top = rect.bottom - 40;
	ctrl = new XPicture(GetSafeHwnd(), m_Id, &rect, 0x00109950);
	((XPicture*)ctrl)->SetImage(IDB_BTN_NEXT1, TEXT("PNG"));
	m_Page2Ctrls.push_back(ctrl);
	m_BtnClicks.insert(std::make_pair(m_Id, &CCellViewDlg::OnClickNext));
	m_Id++;

	GetClientRect(&rect);
	rect.left += 4;
	rect.right -= 4;
	rect.top = (rect.bottom - rect.top) / 2 - 60;
	rect.bottom = rect.top + 120;
	ctrl = new XLabel(GetSafeHwnd(), m_Id, &rect, 0, false, false);
	((XLabel*)ctrl)->SetFont(12, 0x00FFFFFF, true);
	((XLabel*)ctrl)->SetText(string(STR_INIT_S));
	m_Page0Ctrls.push_back(ctrl);
	m_Page3Ctrls.push_back(ctrl);
	m_Id++;
}

void CCellViewDlg::DrawControl(HDC hdc)
{
	XCtrlList::iterator it = m_ctrls->begin();
	for (; it != m_ctrls->end(); it++)
	{
		if ((*it)->NeedPaint())
			(*it)->OnPaint(hdc);
	}
}

void CCellViewDlg::DrawBkImage(HDC hdc)
{
	if (!m_BkImage)
	{
		HGLOBAL hgb;
		XControlBase::LoadImageFromResource(&m_BkImage, &hgb, IDB_DLGBK1, TEXT("JPG"));
	}
	if (m_BkImage)
	{
		CRect rect;
		Gdiplus::Graphics g(hdc);
		GetClientRect(&rect);
		g.DrawImage(m_BkImage, 0, 0, rect.Width(), rect.Height());
	}
}

void CCellViewDlg::DrawString(HDC hdc)
{
	Gdiplus::Graphics g(hdc);
	wchar_t *buf = AnsiToUnicode(m_string.c_str());
	LOGFONT font;
	CRect rect;
	memset(&font, 0, sizeof LOGFONT);
	font.lfHeight = -15;
	font.lfWidth = 0;
	font.lfWeight = FW_NORMAL;
	font.lfItalic = FALSE;
	font.lfUnderline = FALSE;
	font.lfStrikeOut = FALSE;
	font.lfCharSet = DEFAULT_CHARSET;
	font.lfOutPrecision = OUT_DEFAULT_PRECIS;
	font.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	font.lfQuality = CLEARTYPE_QUALITY;
	font.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
	_tcscpy(font.lfFaceName, "微软雅黑");

	Gdiplus::Font hf(hdc, &font);

	GetClientRect(&rect);
	rect.top += 20;
	Gdiplus::StringFormat fmt;
	fmt.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
	//g.DrawString(buf, lstrlenW(buf),&hf,Gdiplus::RectF(rect.left,rect.top,rect.right - rect.left,rect.bottom - rect.top),&fmt,Gdiplus::Brush)
}

void CCellViewDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_MouseTrack)
	{
		TRACKMOUSEEVENT csTME;
		memset(&csTME, 0, sizeof(csTME));
		csTME.cbSize = sizeof(TRACKMOUSEEVENT);
		csTME.dwFlags = TME_LEAVE | TME_HOVER;
		csTME.dwHoverTime = 2;
		csTME.hwndTrack = GetSafeHwnd();
		::TrackMouseEvent(&csTME);
		m_MouseTrack = FALSE;
	}
	XCtrlList::iterator it = m_ctrls->begin();
	for (; it != m_ctrls->end(); it++)
	{
		if ((*it)->PtInCtrl(&point))
			(*it)->OnHover();
		else
			(*it)->OnLeave();
	}

	CDialogEx::OnMouseMove(nFlags, point);
}

void CCellViewDlg::OnMouseLeave()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	XCtrlList::iterator it = m_ctrls->begin();
	for (; it != m_ctrls->end(); it++)
	{
		(*it)->OnLeave();
	}

	m_MouseTrack = TRUE;

	CDialogEx::OnMouseLeave();
}

void CCellViewDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	XCtrlList::iterator it = m_ctrls->begin();
	for (; it != m_ctrls->end(); it++)
	{
		if ((*it)->PtInCtrl(&point))
		{
			(*it)->OnLButtonUp();
			break;
		}
	}
	CDialogEx::OnLButtonUp(nFlags, point);
}

LRESULT CCellViewDlg::OnClickMessage(WPARAM wParam, LPARAM lParam)
{
	ClickList::iterator it = m_BtnClicks.find(wParam);
	if (it != m_BtnClicks.end())
	{
		FunClick f = it->second;
		(this->*f)();
	}
	return 0;
}

LRESULT CCellViewDlg::OnInitMsg(WPARAM wParam, LPARAM lParam)
{
	if (wParam == INIT_FAIL)
	{
		SetWindowInfo(STR_INIT_ERR);
	}
	else
	{
		ShowPage(1);
	}
	return 0;
}

LRESULT CCellViewDlg::OnXmppMsg(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case EXT_TYPE_USERLOGIN:
	case EXT_TYPE_DOCDOWN:
	{
							 if (wParam == EXT_TYPE_DOCDOWN && lParam == (int)Act_Begin)
								 SetWindowInfo(STR_DOWN);
							 else if (wParam == EXT_TYPE_USERLOGIN && lParam == (int)Act_Success)
								 SetWindowInfo(STR_LOGINS);
							 else if (wParam == EXT_TYPE_USERLOGIN && lParam == (int)Act_Failure)
							 {
								 SetWindowInfo(STR_LOGINF);
								 theApp.m_XmppConn->DisConnect();
							 }
							 else if (wParam == EXT_TYPE_USERLOGIN && lParam == (int)Act_Cancel)
								 SetWindowInfo(STR_LOGINF);
	}
		break;
	case EXT_TYPE_DOCPLAY:
		break;
	}
	return 0;
}

LRESULT CCellViewDlg::OnTaskMsg(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case EXT_TYPE_USERLOGIN:
	case EXT_TYPE_DOCPLAY:
	case EXT_TYPE_DOCDOWN:
	{
							 if (wParam == EXT_TYPE_DOCDOWN && lParam == (int)Act_Begin)
								 SetWindowInfo(STR_DOWN);
							 else if (wParam == EXT_TYPE_DOCDOWN && lParam == (int)Act_Success)
								 SetWindowInfo(STR_DOWN_F);
							 else if (wParam == EXT_TYPE_DOCPLAY && lParam == (int)Act_Begin)
								 SetWindowInfo(STR_PLAYING);
							 else if (wParam == EXT_TYPE_DOCPLAY && lParam == (int)Act_Cancel)
								 SetWindowInfo(STR_PLAYEND);
	}
		break;
	}
	return 0;
}

void CCellViewDlg::SetWindowInfo(LPCSTR info)
{
	XCtrlList::iterator it = m_Page3Ctrls.begin();
	for (; it != m_Page3Ctrls.end(); it++)
	{
		if ((*it)->GetCtrlType() == XCONTROL_LABEL)
			break;
	}
	XLabel *x = (XLabel*)(*it);
	x->SetText(string(info));
}

void CCellViewDlg::OnClickClose()
{
	PostMessage(WM_CLOSE, 0, 0);
}

void CCellViewDlg::OnClickMin()
{
	ShowWindow(SW_MINIMIZE);
}

void CCellViewDlg::OnClickLast()
{
	if (m_ctrls == &m_Page2Ctrls)
	{
		ShowPage(1);
		m_StringID = "";
		m_StringPass = "";
		UpdateData(FALSE);
	}
	else
	{
		theApp.m_XmppConn->DisConnect();
		ShowPage(2);
	}
}

void CCellViewDlg::OnClickNext()
{
	UpdateData(TRUE);
	SetWindowInfo(STR_LOGINIT);
	ShowPage(3);
	//do login xmpp
	theApp.m_XmppConn->SetJidResource(m_StringID.GetBuffer(),m_StringPass.GetBuffer());

	//start xmpp recv thread
	theApp.m_XmppConn->Connect();
}

void CCellViewDlg::OnClickULogin()
{
	ShowPage(2);
}

void CCellViewDlg::OnClickGstLogin()
{
	SetWindowInfo(STR_LOGINIT);
	ShowPage(3);
	//do login xmpp
	theApp.m_XmppConn->SetJidResource(X_GUEST_ID, X_GUEST_PASS);

	theApp.m_XmppConn->Connect();
}

void CCellViewDlg::ShowPage(int page)
{
	page = (page > 3) ? 3 : ((page < 0) ? 0 : page);
	switch (page)
	{
	case 0:
		m_ctrls = &m_Page0Ctrls;
		GetDlgItem(IDC_EDIT_UID)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_UPASS)->ShowWindow(SW_HIDE);
		InvalidateRect(NULL, TRUE);
		break;
	case 1:
		m_ctrls = &m_Page1Ctrls;
		GetDlgItem(IDC_EDIT_UID)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_UPASS)->ShowWindow(SW_HIDE);
		InvalidateRect(NULL, TRUE);
		break;
	case 2:
		m_ctrls = &m_Page2Ctrls;
		GetDlgItem(IDC_EDIT_UID)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_UPASS)->ShowWindow(SW_SHOW);
		InvalidateRect(NULL, TRUE);
		break;
	case 3:
		m_ctrls = &m_Page3Ctrls;
		GetDlgItem(IDC_EDIT_UID)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_UPASS)->ShowWindow(SW_HIDE);
		InvalidateRect(NULL, TRUE);
		break;
	}
	XCtrlList::iterator it = m_ctrls->begin();
	for (; it != m_ctrls->end(); it++)
	{
		(*it)->ReqRePaint();
	}
}

void CCellViewDlg::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);

	// TODO:  在此处添加消息处理程序代码

	XCtrlList::iterator it = m_ctrls->begin();
	for (; it != m_ctrls->end(); it++)
	{
		(*it)->ReqRePaint();
	}
}

void CCellViewDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);
	
	// TODO:  在此处添加消息处理程序代码
	XCtrlList::iterator it = m_ctrls->begin();
	for (; it != m_ctrls->end(); it++)
	{
		(*it)->ReqRePaint();
	}
}
