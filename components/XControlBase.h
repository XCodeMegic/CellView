#pragma once

#include <Windows.h>
#include <gdiplus.h>
#include <list>
#include <map>
#include <iostream>

#pragma comment(lib,"gdiplus.lib")

using namespace std;

/*Some Windows Message SelfDefined
 */
#define WM_XBTN_CLICK WM_USER + 20

#define MAX_A(a,b,c) ((a > b) && (a>c))

typedef enum
{
	XCONTROL_BUTTON,
	XCONTROL_LABEL,
	XCONTROL_PICTURE
}XCtrlType;

typedef enum
{
	X_BTNS_NONE,
	X_BTNS_HOVER,
	X_BTNS_DOWN,
	X_BTNS_UP,
	X_BTNS_NEEDRP
} XBtnStatus;

class XColor
{
public:
	XColor(COLORREF color);
	XColor(BYTE xa, BYTE xr, BYTE xg, BYTE xb);
public:
	BYTE	a;
	BYTE	r;
	BYTE	g;
	BYTE	b;
public:
	Gdiplus::ARGB _ARGB();
	Gdiplus::ARGB _ABGR();
};

class XControlBase
{
protected:
	HWND				m_MainWnd;
	RECT				m_WorkRect;
	XBtnStatus			m_Status;
	XBtnStatus			m_OldStatus;
	UINT				m_Id;
	bool				m_Visible;
public:
	XControlBase(HWND hwnd, UINT id, LPRECT rect);
	XControlBase(HWND hwnd, UINT id, LPPOINT ptSrc, LPSIZE szWh);
	XControlBase(HWND hwnd, UINT id, int x, int y, int w, int h);
	virtual ~XControlBase();
public:
	virtual void OnHover();
	virtual void OnLeave();
	virtual void OnLButtonDown();
	virtual void OnLButtonUp();
	virtual void OnLButtonClick();
	virtual bool OnPaint(HDC hdc);
	virtual void ReqRePaint();
	virtual BOOL PtInCtrl(POINT *pt);
	virtual BOOL NeedPaint(void);
	virtual void Show(bool Visible);
	virtual XCtrlType GetCtrlType() = 0;
public:
	static BOOL LoadImageFromResource(Gdiplus::Image **img, HGLOBAL *hgb, UINT uResId, LPCTSTR strResType);
};

typedef std::list<XControlBase *> XCtrlList;
