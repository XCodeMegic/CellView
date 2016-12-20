
// CellViewDlg.h : ͷ�ļ�
//

#pragma once


#include "XControlBase.h"

#define WM_INITEND	WM_USER + 30
#define INIT_OK		0
#define INIT_FAIL	1

#define WM_XMPPMSG	WM_USER + 31

#define WM_TASKMSG	WM_USER + 32

#define STR_INIT_S _T("���ڳ�ʼ������")
#define STR_INIT_ERR	_T("��ʼ��ʧ��,��������")
#define STR_DOWN	_T("���������ļ�...")
#define STR_DOWN_F	_T("�������")
#define STR_PLAYING	_T("����ʹ�ÿμ�")
#define STR_PLAYEND	_T("�μ��ѹر�")
#define STR_LOGINIT	_T("���ڵ�¼...")
#define STR_LOGINS	_T("��¼�ɹ��� ")
#define STR_LOGINF	_T("��¼ʧ��,�뷵�ز����µ�¼")
#define STR_DISCONN	_T("�ѶϿ�����������")

class CCellViewDlg;

typedef void(CCellViewDlg::*FunClick)(void);

typedef std::map<int,FunClick> ClickList;

// CCellViewDlg �Ի���
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

// ����
public:
	CCellViewDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CELLVIEW_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
