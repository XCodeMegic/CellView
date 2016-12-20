#pragma once
#include "TaskBase.h"
class CDocPlayTask :
	public CTaskBase
{
public:
	CDocPlayTask();
	CDocPlayTask(CXmppInstance *ins, XDocMessage *msg);
	virtual ~CDocPlayTask();

public:
	virtual bool Work();
	virtual void WorkEnd(bool bSuccess = true);
	virtual CTaskBase* clone() const;
private:
	std::string m_Name;
	std::string m_Status;
	std::string m_path;

	//xmpp login info
	std::string m_jid;
	std::string m_pass;
	std::string m_sIP;

	//hisntance
	void*	m_Hinstance;
public:
	void SetPath(LPCSTR path) { m_path = path; }
	//need HINSTANCE type
	void SetProcHandle(void *hins)	{ m_Hinstance = hins; }
	//xmppinfo
	void SetLogininfo(LPCSTR jid, LPCSTR pass, LPCSTR sIP);
};

