#include "XControlBase.h"

//---------------------------------- XColor ---------------------------------------------------
XColor::XColor(COLORREF color)
{
	a = (BYTE)(color >> 24);
	r = (BYTE)((color & 0x00FF0000) >> 16);
	g = (BYTE)((color & 0x0000FF00) >> 8);
	b = (BYTE)(color & 0x000000FF);
}

XColor::XColor(BYTE xa, BYTE xr, BYTE xg, BYTE xb)
{
	a = xa;
	r = xr;
	g = xg;
	b = xb;
}

Gdiplus::ARGB XColor::_ARGB()
{
	return Gdiplus::Color::MakeARGB(a, r, g, b);
}

Gdiplus::ARGB XColor::_ABGR()
{
	return Gdiplus::Color::MakeARGB(a, b, g, r);
}


//--------------------------------------- XControlBase ---------------------------------------------------------------------
XControlBase::XControlBase(HWND hwnd, UINT id, LPRECT rect) : m_MainWnd(hwnd), m_Status(X_BTNS_NONE), m_WorkRect(*rect),
m_OldStatus(X_BTNS_HOVER), m_Id(id), m_Visible(true)
{

}

XControlBase::XControlBase(HWND hwnd, UINT id, LPPOINT ptSrc, LPSIZE szWh) : m_MainWnd(hwnd), m_Status(X_BTNS_NONE),
m_OldStatus(X_BTNS_HOVER), m_Id(id), m_Visible(true)
{
	m_WorkRect.left = ptSrc->x;
	m_WorkRect.top = ptSrc->y;
	m_WorkRect.right = szWh->cx + ptSrc->x;
	m_WorkRect.bottom = szWh->cy + ptSrc->y;
}

XControlBase::XControlBase(HWND hwnd, UINT id, int x, int y, int w, int h) : m_MainWnd(hwnd), m_Status(X_BTNS_NONE),
m_OldStatus(X_BTNS_HOVER), m_Id(id), m_Visible(true)
{
	m_WorkRect.left = x;
	m_WorkRect.top = y;
	m_WorkRect.right = w + x;
	m_WorkRect.bottom = h + y;
}

XControlBase::~XControlBase()
{

}

BOOL XControlBase::LoadImageFromResource(Gdiplus::Image **img, HGLOBAL *hgb, UINT uResId, LPCTSTR strResType)
{
	HRSRC hRes;

	HGLOBAL hGlobal;

	LPVOID mSrc, mDst;

	IStream *stm;

	DWORD dwSize;

	hRes = FindResource(NULL, MAKEINTRESOURCE(uResId), strResType);
	if (!hRes)
		goto fail;
	hGlobal = LoadResource(NULL, hRes);
	if (!hGlobal)
		goto fail;
	mSrc = LockResource(hGlobal);
	dwSize = SizeofResource(NULL, hRes);

	hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwSize);

	if (!hGlobal) goto fail;

	mDst = GlobalLock(hGlobal);
	memcpy(mDst, mSrc, dwSize);
	GlobalUnlock(hGlobal);

	if (FAILED(CreateStreamOnHGlobal(hGlobal, FALSE, &stm)))
	{
		GlobalFree(hGlobal);
		return FALSE;
	}

	*img = new Gdiplus::Image(stm);

	*hgb = hGlobal;

	return TRUE;

fail:
	return FALSE;
}

void XControlBase::OnHover()
{
	if (m_Status == X_BTNS_HOVER) return;
	if (m_Status == X_BTNS_DOWN) return;
	m_Status = X_BTNS_HOVER;
	InvalidateRect(m_MainWnd, &m_WorkRect, TRUE);
}

void XControlBase::OnLeave()
{
	if (m_Status == X_BTNS_NONE) return;
	m_Status = X_BTNS_NONE;
	InvalidateRect(m_MainWnd, &m_WorkRect, TRUE);
}

void XControlBase::OnLButtonDown()
{
	if (m_Status == X_BTNS_DOWN) return;
	m_Status = X_BTNS_DOWN;
	InvalidateRect(m_MainWnd, &m_WorkRect, TRUE);
}

void XControlBase::OnLButtonUp()
{
	if (m_Status == X_BTNS_DOWN) OnLButtonClick();
	if (m_Status == X_BTNS_UP) return;
	m_Status = X_BTNS_UP;
	InvalidateRect(m_MainWnd, &m_WorkRect, TRUE);
}

void XControlBase::OnLButtonClick()
{
	PostMessage(m_MainWnd, WM_XBTN_CLICK, m_Id, 0);
}

BOOL XControlBase::PtInCtrl(POINT *pt)
{
	return PtInRect(&m_WorkRect, *pt);
}

BOOL XControlBase::NeedPaint(void)
{
	return m_OldStatus != m_Status;
}

void XControlBase::ReqRePaint()
{
	m_OldStatus = X_BTNS_NEEDRP;
}

bool XControlBase::OnPaint(HDC hdc)
{
	if (m_OldStatus == m_Status) return false;
	m_OldStatus = m_Status;
	if (!m_Visible) return false;
	return true;
}

void XControlBase::Show(bool Visible)
{
	if (m_Visible == Visible) return;
	m_Visible = Visible;
	InvalidateRect(m_MainWnd, &m_WorkRect, TRUE);
}
