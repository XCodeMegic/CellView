#pragma once
#include "TaskBase.h"
class CDocDownTask :
	public CTaskBase
{
public:
	CDocDownTask();
	CDocDownTask(CXmppInstance *ins,XDocMessage *msg);
	~CDocDownTask();
public:
	virtual bool Work();
	virtual void WorkEnd(bool bSuccess = true);
	virtual CTaskBase* clone() const;
private:
	XDocInfoList	m_DocList;
	std::string		m_path;
	int				m_Qtype;
private:
	void AddDoc(std::string &name, std::string &url, std::string& status, std::string& version);
	bool CheckFile(const std::string &filename, const std::string &version);
public:
	void SetLocalPath(LPCTSTR path) { m_path = path; }
};

