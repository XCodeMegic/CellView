
// CellView.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "XmppInstance.h"

// CCellViewApp: 
// �йش����ʵ�֣������ CellView.cpp
//

class CCellViewApp : public CWinApp
{
public:
	CXmppInstance *m_XmppConn;

	HINSTANCE m_hinsEyeBeam;

	HINSTANCE m_hinsVNC;
public:
	CCellViewApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCellViewApp theApp;