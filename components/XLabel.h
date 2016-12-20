#pragma once

#include "XControlBase.h"

class XLabel : public XControlBase
{
public:
	XLabel(HWND hwnd, UINT id, LPRECT rect, COLORREF clBack, bool defaultFont = true, bool DrawBK = true);
	XLabel(HWND hwnd, UINT id, LPPOINT ptSrc, LPSIZE szWh, COLORREF clBack, bool defaultFont = true, bool DrawBK = true);
	XLabel(HWND hwnd, UINT id, int x, int y, int w, int h, COLORREF clBack, bool defaultFont = true, bool DrawBK = true);
	virtual ~XLabel();
protected:
	//XControlBase method
	virtual bool OnPaint(HDC hdc); 
	virtual void OnHover() { if (m_DrawBK) XControlBase::OnHover(); }
	virtual void OnLeave() { if (m_DrawBK) XControlBase::OnLeave(); }
	virtual void OnLButtonDown() { if (m_DrawBK) XControlBase::OnLButtonDown(); }
	virtual void OnLButtonUp() { if (m_DrawBK) XControlBase::OnLButtonUp(); }
	virtual void OnLButtonClick() { if (m_DrawBK) XControlBase::OnLButtonClick(); }
	virtual BOOL PtInCtrl(POINT *pt) { if (!m_DrawBK) return false; else return XControlBase::PtInCtrl(pt); }
	virtual XCtrlType GetCtrlType() { return XCONTROL_LABEL; }

private:
	Gdiplus::Font*		m_Font;
	Gdiplus::Brush*		m_BrushFont;
	LOGFONT				m_lfont;

	//
	Gdiplus::Brush*		m_BrushFrame;
	Gdiplus::Brush*		m_BrushRect;

	//string
	string				m_String;
	bool				m_DrawBK;

private:
	void InitBackColor(COLORREF color);
public:
	void SetFont(int dwSize, COLORREF color, bool bBold = false, bool bItalic = false);
	void SetText(std::string &str);
	
};