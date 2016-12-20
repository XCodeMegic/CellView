#include "stdafx.h"
#include "TaskBase.h"


CTaskBase::CTaskBase(CXmppInstance *ins, XMessage *msg) :m_Instance(ins), m_Msginfo(NULL)
{
	if (msg)
		m_Msginfo = new XMessage(msg->Jid, msg->Id, msg->IqType, msg->GetType());
}

void CTaskBase::InitInstance(CXmppInstance *ins, XMessage *msg)
{
	m_Instance = ins;
	if (m_Msginfo)
		delete m_Msginfo;
	if (msg)
		m_Msginfo = new XMessage(msg->Jid, msg->Id, msg->IqType, msg->GetType());
}

CTaskBase::~CTaskBase()
{
	if (m_Msginfo)
		delete m_Msginfo;
}

