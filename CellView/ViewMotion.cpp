#include "stdafx.h"
#include "CellView.h"
#include "ViewMotion.h"
#include "XmppInstance.h"
#include "CellViewDlg.h"
#include "DocDownTask.h"
#include "DocPlayTask.h"
#include "ScreenCapTask.h"


CViewMotion::CViewMotion(CXmppInstance *ins)
{
	m_Instance = ins;
}


CViewMotion::~CViewMotion()
{
}

bool CViewMotion::handleLogin(XULoginMessage *msg)
{
	if (msg->QType == 1)
		PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_XMPPMSG, EXT_TYPE_USERLOGIN, Act_Success);
	else
		PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_XMPPMSG, EXT_TYPE_USERLOGIN, Act_Failure);
	return true;
}

bool CViewMotion::handleDocDown(XDocMessage *msg)
{
	//start one down task
	CDocDownTask task(m_Instance, msg);
	task.SetLocalPath(m_Instance->GetLocalPath());
	m_Instance->AddTask(&task);
	return true;
}

bool CViewMotion::handleDocPlay(XDocMessage *msg)
{
	//start one play task
	CDocPlayTask	task(m_Instance, msg);
	task.SetPath(m_Instance->GetLocalPath());
	task.SetProcHandle(m_Instance->GetProcessInstance());
	task.SetLogininfo(m_Instance->GetJid(), m_Instance->GetPass(), m_Instance->GetSIP());
	m_Instance->AddTask(&task);
	return true;
}

bool CViewMotion::handleScCap(XScMessage *msg)
{
	//start one sc task
	CScreenCapTask task(m_Instance, msg);
	m_Instance->AddTask(&task);
	return true;
}

bool CViewMotion::handleSenceCtrl(XSenceMessage *msg)
{
	//nothing to do

	return true;
}

bool CViewMotion::handleSenceUpdate(XSenceMessage *msg)
{
	//nothing to do
	return true;
}

bool CViewMotion::handleConnected(void)
{
	//nothing to do
	MessageBox(NULL, _T("Connected!!!"), _T("info"), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool CViewMotion::handleDisConnected(void)
{
	PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_XMPPMSG, EXT_TYPE_USERLOGIN, Act_Cancel);
	return true;
}

