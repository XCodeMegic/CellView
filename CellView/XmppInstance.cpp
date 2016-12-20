#include "stdafx.h"
#include <string.h>
#include "XmppInstance.h"
#include "ViewMotion.h"
#include "CellConnection.h"
#include "CellPresenceMgr.h"
#include "CellView.h"
#include "TaskManager.h"

//---------------------------------- X_JID --------------------------------------------------
X_JID::X_JID()
{
	m_JID = "";
	m_Resource = "";
	m_Server = "";
	m_User = "";

	m_Res_ID = "";
	m_Res_Pass = "";
	m_Res_Pref = "";
}

X_JID::X_JID(std::string &jid)
{
	Parse(jid);
}

void X_JID::Parse(std::string& jid)
{
	m_JID = jid;
	int pos = jid.find("@");
	m_User = jid.substr(0, pos);

	std::string tmp = jid.substr(pos + 1);
	pos = tmp.find("/");
	if (pos<0 || pos >= tmp.length())
	{
		m_Server = tmp;
		m_Resource = "";
		m_Res_ID = "";
		m_Res_Pass = "";
		m_Res_Pref = "";
	}
	else
	{
		m_Server = tmp.substr(0, pos);
		m_Resource = tmp.substr(pos + 1);
		pos = m_Resource.find("_");
		if (pos < 0 || pos >= m_Resource.length())
		{
			m_Res_Pref = m_Resource;
			m_Res_ID = "";
			m_Res_Pass = "";
		}
		else
		{
			m_Res_Pref = m_Resource.substr(0, pos);
			tmp = m_Resource.substr(pos + 1);
			pos = tmp.find("_");
			if (pos < 0 || pos >= tmp.length())
			{
				m_Res_ID = tmp;
				m_Res_Pass = "";
			}
			else
			{
				m_Res_ID = tmp.substr(0, pos);
				m_Res_Pass = tmp.substr(pos + 1);
			}
		}
	}
}

std::string& X_JID::JID()
{
	m_JID = m_User + "@" + m_Server;
	if (!Resource().empty())
		m_JID += "/" + m_Resource;
	return m_JID;
}

std::string& X_JID::Resource()
{
	if (!m_Res_Pass.empty())
		m_Resource = m_Res_Pref + "_" + m_Res_ID + "_" + m_Res_Pass;
	else if (!m_Res_ID.empty())
		m_Resource = m_Res_Pref + "_" + m_Res_ID;
	else
		m_Resource = m_Res_Pref;
	return m_Resource;
}

//---------------------------------- CXmppInstance ----------------------------------------------
CXmppInstance::CXmppInstance() :m_client(NULL), m_motion(NULL), m_IqManager(NULL), m_TaskMgr(NULL), m_ConnMgr(NULL)
, m_XmppThread(NULL)
{
}


CXmppInstance::~CXmppInstance()
{
}

bool CXmppInstance::LoadConfig(LPCSTR lpcfgFile)
{
	char user[128], server[128];
	GetPrivateProfileString(CFG_APP, CFG_SN, _T(""), server, 128, lpcfgFile);
	GetPrivateProfileString(CFG_APP, CFG_SIP, _T(""), m_ServerIp, 128, lpcfgFile);
	GetPrivateProfileString(CFG_APP, CFG_CUSER, _T(""), user, 128, lpcfgFile);
	GetPrivateProfileString(CFG_APP, CFG_PASS, _T(""), m_Pass, 128, lpcfgFile);
	GetPrivateProfileString(CFG_PATHINFO, CFG_LOCAL_PATH, _T(""), m_LocalPath, MAX_PATH, lpcfgFile);
	GetPrivateProfileString(CFG_PATHINFO, CFG_REMOTE_PATH, _T(""), m_RemotePath, MAX_PATH, lpcfgFile);

	if (!strcmp(server, "") || !strcmp(m_ServerIp, "") || !strcmp(user, "") || !strcmp(m_Pass, "")) return false;

	m_JID.User() = user;
	m_JID.Server() = server;

	return true;
}

void	CXmppInstance::SetJidResource(LPCSTR resID, LPCSTR resPass)
{
	m_JID.SetResID(resID);
	m_JID.SetResPass(resPass);
}

void CXmppInstance::runConnect()
{
	if (!m_client)
	{
		m_client = new gloox::Client(gloox::JID(m_JID.JID()), m_Pass);
		m_client->setServer(m_ServerIp);
	}
	else
	{
		m_client->setUsername(m_JID.User());
		m_client->setResource(m_JID.Resource());
	}

	if (!m_motion)
		m_motion = new CViewMotion(this);

	if (!m_ConnMgr)
	{
		m_ConnMgr = new CCellConnection(m_client);
		m_ConnMgr->SetMotion(m_motion);	//roster,
	}

	if (!m_IqManager)
	{
		m_IqManager = new CCellIqManager(m_client);
		m_IqManager->SetMotion(m_motion);
	}

	//CCellPresenceMgr *m = new CCellPresenceMgr(m_client);
	//m->SetMotion(m_motion);	//presence

	if (!m_TaskMgr)
	{
		m_TaskMgr = new CTaskManager;
		m_TaskMgr->Run();
	}

	m_client->connect();
}

void CXmppInstance::Send(XMessage *msg)
{
	if (m_IqManager)
	{
		m_IqManager->Send(msg);
	}
}

UINT CXmppInstance::ThreadProc(LPVOID param)
{
	CXmppInstance* _this = (CXmppInstance *)param;
	_this->runConnect();
	
	return 0;
}

void CXmppInstance::Connect()
{
	m_XmppThread = AfxBeginThread(CXmppInstance::ThreadProc, this);
}

void CXmppInstance::AddTask(CTaskBase* task)
{
	if (m_TaskMgr)
		m_TaskMgr->AddTask(task);
}

void CXmppInstance::DisConnect()
{
	if (m_client->state() != gloox::ConnectionState::StateConnected) return;
	if (m_client)
		m_client->disconnect();
	if (m_XmppThread)
		WaitForSingleObject(m_XmppThread->m_hThread, INFINITE);
	m_XmppThread = NULL;
}
