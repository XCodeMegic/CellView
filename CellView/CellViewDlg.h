
// CellViewDlg.h : 头文件
//

#pragma once


#include "XControlBase.h"

#define WM_INITEND	WM_USER + 30
#define INIT_OK		0
#define INIT_FAIL	1

#define WM_XMPPMSG	WM_USER + 31

#define WM_TASKMSG	WM_USER + 32

#define STR_INIT_S _T("正在初始化环境")
#define STR_INIT_ERR	_T("初始化失败,请检查配置")
#define STR_DOWN	_T("正在下载文件...")
#define STR_DOWN_F	_T("下载完毕")
#define STR_PLAYING	_T("正在使用课件")
#define STR_PLAYEND	_T("课件已关闭")
#define STR_LOGINIT	_T("正在登录...")
#define STR_LOGINS	_T("登录成功！ ")
#define STR_LOGINF	_T("登录失败,请返回并重新登录")
#define STR_DISCONN	_T("已断开服务器连接")

class CCellViewDlg;

typedef void(CCellViewDlg::*FunClick)(void);

typedef std::map<int,FunClick> ClickList;

// CCellViewDlg 对话框
class CCellViewDlg : public CDialogEx
{
private:
	UINT				m_Id;
	XCtrlList*			m_ctrls;
	//3 pages 
	XCtrlList			m_Page0Ctrls;
	XCtrlList			m_Page1Ctrls;
	XCtrlList			m_Page2Ctrls;
	XCtrlList			m_Page3Ctrls;

	Gdiplus::Image *	m_BkImage;
	BOOL				m_MouseTrack;
	ClickList			m_BtnClicks;
	std::string			m_string;
public:
	void InitCtrl(void);
	void DrawControl(HDC hdc);
	void DrawBkImage(HDC hdc);
	void DrawString(HDC hdc);

	//some event
	void OnClickClose();
	void OnClickMin();
	void OnClickLast();
	void OnClickNext();
	void OnClickULogin();
	void OnClickGstLogin();

	void ShowPage(int page);

	void SetWindowInfo(LPCSTR info);

	afx_msg LRESULT OnClickMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInitMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnXmppMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTaskMsg(WPARAM wParam, LPARAM lParam);

// 构造
public:
	CCellViewDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CELLVIEW_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMove(int x, int y);
	CString m_StringID;
	CString m_StringPass;
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};
