#pragma once
#include "TaskBase.h"
class CScreenCapTask :
	public CTaskBase
{
public:
	CScreenCapTask();
	CScreenCapTask(CXmppInstance *ins, XScMessage *msg);
	virtual ~CScreenCapTask();

public:
	virtual bool Work();
	virtual void WorkEnd(bool bSuccess = true);
	virtual CTaskBase* clone() const;
private:
	std::string m_RemoteFileName;
};

