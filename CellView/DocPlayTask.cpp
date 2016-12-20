#include "stdafx.h"
#include "DocPlayTask.h"
#include "CellView.h"
#include "CellViewDlg.h"
#include "XmppInstance.h"


CDocPlayTask::CDocPlayTask() :CTaskBase(NULL, NULL)
{

}

CDocPlayTask::CDocPlayTask(CXmppInstance *ins, XDocMessage *msg) :CTaskBase(ins, msg)
{
	m_Name = "";
	m_Status = "";
	if (msg->Docs.size() <= 0) return;
	XDocInfoList::iterator it = msg->Docs.begin();
	m_Name = (*it)->Name;
	m_Status = (*it)->Status;
}


CDocPlayTask::~CDocPlayTask()
{
}

bool CDocPlayTask::Work()
{
	if (m_Name.empty() || m_Status.empty() || m_path.empty())
		return false;

	string strExeName = m_path + m_Name;

	X_JID jid(m_jid);

	jid.SetResPref(X_XMPPRES_UNIT);

	string strJid = jid.JID();

	CString strCmd;
	// cmd like: exename fromjid loginjid loginpass loginserverIP
	strCmd.Format(_T("\"%s\" \"%s\" \"%s\" %d \"%s\""), m_Msginfo->Jid.c_str(), m_pass.c_str(), m_sIP.c_str(), 5222, strJid.c_str());

	HINSTANCE hProcess = NULL;// = ShellExecute(NULL, ) wait command

	//test
	hProcess = ShellExecute(NULL, _T("open"), strExeName.c_str(), strCmd.GetBuffer(), m_path.c_str(), SW_SHOWNORMAL);

	if ((UINT)hProcess <= 32) //failure
	{
		(*(HINSTANCE*)m_Hinstance) = NULL;
		return false;
	}
	else
	{
		PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_TASKMSG, m_Msginfo->GetType(), Act_Success);
		(*(HINSTANCE*)m_Hinstance) = hProcess;
	}
	return true;
}

void CDocPlayTask::WorkEnd(bool bSuccess)
{
	XIQType tp = bSuccess ? IQ_Result : IQ_Error;
	XDocMessage msg(m_Msginfo->Jid, m_Msginfo->Id, tp, EXT_TYPE_DOCPLAY);
	if (!m_Name.empty())
		msg.AddDoc(m_Name, std::string(""), m_Status, std::string(""));
	m_Instance->Send(&msg);
}

CTaskBase* CDocPlayTask::clone() const
{
	CDocPlayTask *t = new CDocPlayTask;
	t->InitInstance(m_Instance, m_Msginfo);
	t->m_Name = m_Name;
	t->m_Status = m_Status;
	t->m_path = m_path;
	t->m_Hinstance = m_Hinstance;

	t->m_jid = m_jid;
	t->m_pass = m_pass;
	t->m_sIP = m_sIP;

	return t;
}

void CDocPlayTask::SetLogininfo(LPCSTR jid, LPCSTR pass, LPCSTR sIP)
{
	m_jid = jid;
	m_pass = pass;
	m_sIP = sIP;
}
