#pragma once

#include "iqhandler.h"
#include "x_motion.h"

using namespace gloox;

class CCellIqManager :
	public IqHandler
{
public:
	CCellIqManager(gloox::Client *client);
	virtual ~CCellIqManager();
protected:
	//IqHandler virtual method
	virtual bool handleIq(const IQ& iq);
	virtual void handleIqID(const IQ& iq, int context);

private:
	gloox::Client *m_client;
	XMotion *m_motion;

public:
	void Init();
	void SetMotion(XMotion *mt) { m_motion = mt; }
	void Send(XMessage *msg);
	void Send(XMemberList &jidList, XMessage *);
};

