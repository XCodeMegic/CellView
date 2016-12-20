#pragma once

#include "x_motion.h"
#include "XmppInstance.h"

class CTaskBase
{
public:
	CTaskBase(CXmppInstance *ins, XMessage *msg);
	virtual ~CTaskBase();

	void InitInstance(CXmppInstance *ins, XMessage *msg);

public:
	virtual bool Work() = 0;
	virtual void WorkEnd(bool bSuccess) = 0;
	virtual CTaskBase* clone() const = 0;
protected:
	CXmppInstance	*m_Instance;
	XMessage		*m_Msginfo;
};

typedef std::list<CTaskBase*> TaskList;

