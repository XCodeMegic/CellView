#pragma once
#include "gloox.h"	
#include "client.h"
#include "x_motion.h"
#include "CellIqManager.h"

#ifndef CFG_FILE 
#define CFG_FILE	_T("config.ini")
//xmppinfo
#define CFG_APP		_T("XMPPInfo")
#define CFG_SN		_T("name")
#define CFG_SIP		_T("ip")
#define CFG_CUSER	_T("user")
#define CFG_PASS	_T("pass")
#define CFG_RESOURCE	_T("resource")

//exe info
#define CFG_EXENAME _T("EXEInfo")
#define CFG_EYEBEAM_NAME _T("eyename")
#define CFG_EYEBEAM_PATH _T("eyepath")
#define CFG_VNCNAME		_T("vncname")
#define CFG_VNCPATH		_T("vncpath")

//path info
#define CFG_PATHINFO _T("PATHInfo")
#define CFG_LOCAL_PATH	_T("local")
#define CFG_REMOTE_PATH	_T("remote")
#endif

#define X_XMPPRES_CTRL	_T("Ctrl")
#define X_XMPPRES_UNIT	_T("Unit")

#define X_GUEST_ID		_T("guest")
#define X_GUEST_PASS	_T("guest")

class CTaskManager;
class CTaskBase;
class CCellConnection;

class X_JID
{
private:
	std::string m_JID;
	std::string m_Server;
	std::string m_User;
	std::string m_Resource;

	//Resource like Ctrl_ID_PASS
	std::string m_Res_Pref;
	std::string m_Res_ID;
	std::string m_Res_Pass;
public:
	X_JID();
	X_JID(std::string &jid);
	void Parse(std::string& jid);
	std::string& JID();
	std::string& User(){ return m_User; }
	std::string& Server(){ return m_Server; }
	std::string& Resource();

	void SetResPref(LPCSTR pref){ m_Res_Pref = pref; }
	void SetResID(LPCSTR resId){ m_Res_ID = resId; }
	void SetResPass(LPCSTR resPass){ m_Res_Pass = resPass; }
};

class CXmppInstance
{
public:
	CXmppInstance();
	~CXmppInstance();
private:
	//xmpp client
	gloox::Client *m_client;
	CCellIqManager*	m_IqManager;
	CCellConnection* m_ConnMgr;

	//task manager
	CTaskManager*	m_TaskMgr;

	//xmpp jid,ip,server ...
	char m_ServerIp[128];
	char m_Pass[128];
	X_JID	m_JID;

	char m_LocalPath[MAX_PATH];
	char m_RemotePath[MAX_PATH];

	//xmpp thread handle
	CWinThread*	m_XmppThread;

	//process Instance
	HINSTANCE	m_ProcCourse;

	//motion
	XMotion *m_motion;
public:
	void runConnect();

public:
	bool LoadConfig(LPCSTR lpcfgFile);
	void	SetJidResource(LPCSTR resID,LPCSTR resPass);
	void	AddTask(CTaskBase*);

	//paths
	LPCSTR	GetLocalPath(){ return m_LocalPath; }
	LPCSTR	GetRemoePath(){ return m_RemotePath; }
	HINSTANCE	*GetProcessInstance(){ return &m_ProcCourse; }

	//xmppLogininfo
	LPCSTR	GetJid() { return m_JID.JID().c_str(); }
	LPCSTR	GetPass() { return m_Pass; }
	LPCSTR	GetSIP(){ return m_ServerIp; }

	//xmpp IQ message
	void Connect();
	void Send(XMessage *msg);
	void DisConnect();

	//thread
	static UINT ThreadProc(LPVOID param);
};

